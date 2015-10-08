#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include "utoall.h"
#include "pasdb.h"
#include "pasutl.h"
#include "ncdef.h"
#include "nwdef.h"
#define  PAS_SRCFILE      4101

/* 心跳信息  */
int nwUdpHeart_noMonKey(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead)
{
    int iReturn;
    unsigned long lHtime,lStime,lCtime,lCompId,lTime,lIp,lSendtime;
    char caVersion[10],caKey[18],caMac[20];
    unsigned long lFlow,lNetlink;
    ncUserInfo *psUserInfo;
    nwCompInfo *psComp;
    nwCompIp   *psCompIp;
    long lProclasttime;
    long lMflag;
    long lUserid;
    long lCtl;
    nwProcCtl *psProc;

#ifdef LDEBUG
    printf(" It Is NoMon Heart Info....\n");
    utMsgPrintMsg(psMsgHead);
#endif

  
   

    /* 数据报校验  */
  
    lIp = ntohl(psSockAddr->sSin.sin_addr.s_addr);
   
        
   if(!utMsgCheckSum(psMsgHead,NW_DES_KEY)) { /* 数据不合法  */
#ifdef LDEBUG
            unsigned long lCode;            
            printf(" It is invalid message, CheckSum Error key=%s\n",psCompIp->caKey);
            lCode = utMd5Code( (char *)psMsgHead + sizeof(utMsgHead),
                        psMsgHead->lBytes - sizeof(utMsgHead),psCompIp->caKey);
            printf(" CheckSum==%lu   %lu    %lu \n",
                      lCode,ntohl(lCode),psMsgHead->lCheckSum);                        
#endif        
            return 0;
        }
       ncUtlSetNoMonIp(psShmHead,lIp,2);
       utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                       "status",  UT_TYPE_LONG,0);
       ncCtlIptDelRedict(lIp);
       
    return 0;
}


/* 同步usbkey用户
 */
int usbKeyUtlLogin(utShmHead *psShmHead,nwCompInfo *psComp,unsigned long lSip,char *pVersion)
{
    ncUserInfo *psUserInfo;
    char caSip[16],caDip[16];
    char caMac[20];
    int iReturn;
//    printf(" Do Login ....%lu \n",psComp->compid);
    
    psUserInfo = (ncUserInfo *)ncGetUserInfoByIp(psShmHead,lSip);
    if(psUserInfo) { /* 该机器的用户已在线  */
        psUserInfo->lCltId = psComp->compid;
       
    }
    /* 更新本机信息  */
   
    iReturn = pasDbExecSqlF("update nwcompute set userid=%lu,groupid=%lu,ip=%lu,compname='%s',lasttime=%lu,version='%s' where compid = %lu",
            psComp->userid,psComp->groupid,psComp->ip,psComp->compname,time(0),psComp->version,psComp->compid);
    if(iReturn != 0) {
        pasLogs(PAS_SRCFILE,1000 ,"nwcompute update error %s ",pasDbErrorMsg(NULL));
    }
    else {
        pasDbCommit(NULL);
    }

    return 0;
}


/*审计USBKEY登录 */
int nwUdpLogin_Monkey(utShmHead *psShmHead,
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead)
{
    char    caMac[20],caName[32],caVersion[10],caOs[32],caIe[32];
    char    caKey[32],caKey0[32],caKey1[32],caCompname[20],caCompname0[20],caNac[18],caTemp[32],caMac0[20];
    int iReturn;  
    ncUserInfo *psUserInfo;
    int lMflag;
    long lUserid;
    ncUserCont *psUser;
    long lDef;
    char caDef[32];
    unsigned long lIp,lIp_s,lCompId,lTime,lTcpport,lUdpport,lIp0;
    nwCompInfo *psComp;
    nwCompIp   *psCompIp;
    char caIp[20],caUname[32];
    char caPasswd[32];
    char caPassword1[32];
    char caMessage[128];
    long lGid;
//#ifdef LDEBUG
//printf("pid=%d\n",getpid());
//sleep(20);
 //   printf(" User Login \n");
 //   utMsgPrintMsg(psMsgHead);
//#endif
    iReturn = utMsgGetSomeNVar(psMsgHead,2,
        "username",  UT_TYPE_STRING,31,  caUname,
		    "version",UT_TYPE_STRING, 8,  caVersion);
        
    
    /* 检查内存中是否存在相应的记录  */

    lIp = ntohl(psSockAddr->sSin.sin_addr.s_addr);
  

    lTime=time(0);
    psUserInfo = (ncUserInfo *)ncGetUserInfoByIp(psShmHead,lIp);
    if(psUserInfo) { /* 用户已登录  */
        ncUtlDoLogout(psShmHead,psUserInfo,"User already Login");
    }
    psUser = ncUtlGetUserContByName(psShmHead,caUname);
    if(psUser == NULL) { /* 用户不存在  */
    	pasLogs(PAS_SRCFILE,1000,"User %s does not exist ",caUname);
   
       utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                       "status",  UT_TYPE_LONG,9);
        return (0);
    }
     
    psUserInfo = ncUtlDoLogin(psShmHead,psUser,NC_LOGIN_CLIENT,lIp,caMac,caMessage);
    if(psUserInfo == NULL) {
       utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                       "status",  UT_TYPE_LONG,-1);
        return (0);
    }
    if(strcasecmp(utComGetVar_sd(psShmHead,"AuthSendHtml","No"),"No")==0) {
        ncCtlIptDelRedict(lIp);
    }
    
    lUserid=psUserInfo->lId;
    lGid= psUserInfo->lGid;
    iReturn = pasDbOneRecord("select compid,authkey from nwcompute where devname = :devname",1,
                    "devname",UT_TYPE_STRING,caUname,
                    UT_TYPE_ULONG,4,&lCompId,
                    UT_TYPE_STRING,16,caKey);
    
    if(iReturn == 0) { /* 记录已存在  */
        utDesEncryptA(caKey,caTemp,NW_DES_KEY);
    	}
    	else if(iReturn==1403){
            lCompId = pasGetSid("nwcompute","compid");
               sprintf(caTemp,"ProNetway%lu%lu",time(0),lCompId);
            utMd5Ascii16(caTemp,strlen(caTemp),"pronetway",caKey);
            iReturn = pasDbExecSqlF("insert into nwcompute(compid,ip,devname,compname,mac,addtime,lasttime,version,authkey) values (%lu,%lu,'%s','%s','%s',%lu,%lu,'%s','%s')",
                        lCompId,lIp,caUname,caUname,"",time(0),time(0),caVersion,caKey);
           if(iReturn != 0) {
           	pasDbRollback(NULL);
            utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                       "status",  UT_TYPE_LONG,12);
                       
            pasLogs(PAS_SRCFILE,1000,"INSERT INTO NWCOMPUTE ERR ");      
           	return 0;
    		   }
    		   else{
    		   	pasDbCommit(NULL);
    		   	utDesEncryptA(caKey,caTemp,NW_DES_KEY);
    		   	psComp =  (nwCompInfo *)utShmHashLookA(psShmHead,NC_LNK_COMPUTE,&lCompId);
                if(psComp) {
                    psComp->ip = lIp;
                    strcpy(psComp->devname,caUname);
                    strcpy(psComp->compname,caUname);
                    
                    strcpy(psComp->version,caVersion);
                    strcpy(psComp->caKey,caKey);
                    psComp->userid=lUserid;
                    psComp->groupid = lGid;
                }	   	
    		  }
       }
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&lIp);   
    if(psCompIp) { /* 用户已存在  */
        if(psCompIp->compid == lCompId) {
            psComp = psCompIp->psComp;
            psCompIp->lLastTime = lTime;           
            utDesEncryptA(psCompIp->caKey,caTemp,NW_DES_KEY); 
           utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,5,
                       "status",  UT_TYPE_ULONG,NW_LOG_ISLOGIN,
                       "basetime",UT_TYPE_ULONG,lTime,
                       "id",UT_TYPE_ULONG,lUserid,
                       "key",     UT_TYPE_STRING,caTemp,
                       "did",UT_TYPE_ULONG,lCompId);
            return 0;
        }
        else { /* IP地址不一致,用户需要重新登录  */   
            nwUtlLogOut(psShmHead,psCompIp);  /* 用户退出  */
            
                        pasLogs(PAS_SRCFILE,1000,"ip diffient logout "); 
        }
    }
    else {
        usleep(20);
    }   
     
    psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lCompId);
    if(psComp) {
        utMd5Ascii16(psComp->caKey,strlen(psComp->caKey),NW_DES_KEY,caKey1);
            psCompIp = (nwCompIp *)utShmHashLookA(psShmHead,NC_LNK_ONLINECOMP,&lIp);
            if(psCompIp == NULL) {

                utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                            "status",  UT_TYPE_ULONG,NW_ERR_MAXUSER,
                            "message", UT_TYPE_STRING,"Exceed Max User");  
              return 0;
            }
            else {
                psCompIp->psComp = psComp;
                psComp->status = 1;
                psCompIp->lLoginTime = lTime;
                psCompIp->lLastTime = lTime;
                psCompIp->nUdp = (unsigned short)1004;
                psCompIp->nTcp = (unsigned short)1004;
                psCompIp->ip  = lIp;
                psCompIp->compid = lCompId;
                sprintf(caTemp,"pronetway%lu%lu",lTime,lCompId);
                utMd5Ascii16(caTemp,strlen(caTemp),NW_DES_KEY,psCompIp->caKey);
                iReturn = usbKeyUtlLogin(psShmHead,psComp,lIp,caVersion); 
                if(iReturn == 0) {
                    utDesEncryptA(psCompIp->caKey,caTemp,NW_DES_KEY);
                    utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,5,
                        "status",  UT_TYPE_ULONG,0,
                        "basetime",UT_TYPE_ULONG,lTime,
                        "id",UT_TYPE_ULONG,lUserid,
                        "key",     UT_TYPE_STRING,caTemp,
                        "did",UT_TYPE_ULONG,lCompId);
                }
                else {
                    utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,5,
                        "status",  UT_TYPE_LONG,1,
                         "basetime",UT_TYPE_ULONG,lTime,
                         "id",UT_TYPE_ULONG,lUserid,
                         "key",     UT_TYPE_STRING,caTemp,
                         "did",UT_TYPE_ULONG,lCompId);
                }                    
                return 0;
            }
       }
       else{
       	 utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,3,
                            "status",  UT_TYPE_ULONG,-1,
                            "basetime",UT_TYPE_ULONG,lTime,
                            "key",     UT_TYPE_STRING,caTemp);
      }
        
   
}



/*审计USBKEY登录带密码 */
int nwUdpLogin_Passkey(utShmHead *psShmHead,
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead)
{
    char    caMac[20],caName[32],caVersion[10],caOs[32],caIe[32];
    char    caKey[32],caKey0[32],caKey1[32],caCompname[20],caCompname0[20],caNac[18],caTemp[32],caMac0[20];
    int iReturn;  
    ncUserInfo *psUserInfo;
    int lMflag;
    long lUserid;
    ncUserCont *psUser;
    long lDef;
    char caDef[32];
    unsigned long lIp,lIp_s,lCompId,lTime,lTcpport,lUdpport,lIp0;
    nwCompInfo *psComp;
    nwCompIp   *psCompIp;
    char caIp[20],caUname[32];
    char caPasswd[32];
    char caPassword1[64];
    char caMessage[128];
    long lGid;
#ifdef LDEBUG
    printf(" User Login \n");
    utMsgPrintMsg(psMsgHead);
#endif
    iReturn = utMsgGetSomeNVar(psMsgHead,3,
        "username",  UT_TYPE_STRING,31,  caUname,
        "passwd",    UT_TYPE_STRING,31,caPasswd,
		    "version",UT_TYPE_STRING, 8,  caVersion);

    pasLogs(PAS_SRCFILE,1000,"Username:%s  Pass:%s",caUname,caPasswd);
    /* 检查内存中是否存在相应的记录  */

    lIp = ntohl(psSockAddr->sSin.sin_addr.s_addr);
  

    lTime=time(0);
    psUserInfo = (ncUserInfo *)ncGetUserInfoByIp(psShmHead,lIp);
    if(psUserInfo) { /* 用户已登录  */
        ncUtlDoLogout(psShmHead,psUserInfo,"User already Login");
    }
    psUser = ncUtlGetUserContByName(psShmHead,caUname);
    if(psUser == NULL) { /* 用户不存在  */
        pasLogs(PAS_SRCFILE,1000,"Username:%s  Does not exist",caUname);
        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                       "status",  UT_TYPE_LONG,9,
                       "mesg",    UT_TYPE_STRING,"用户不存在");
        return (0);
    }
     
 //验证密码是否存在    
     
   sprintf(caPassword1,"%s%s",caUname,caPasswd);
     utMd5Ascii22(caPassword1,strlen(caPassword1),NULL,caPasswd);
    if(strcmp(caPasswd,psUser->password)!=0) { /* 口令不正确  */
        pasLogs(PAS_SRCFILE,1000," User %s  Pass:[%s] [%s] ",caUname,caPasswd,psUser->password);
        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                       "status",  UT_TYPE_LONG,10);
        return (0);
    }
     
     
//    pasLogs(PAS_SRCFILE,1000,"user_login %s",caUname); 
     
     
    psUserInfo = ncUtlDoLogin(psShmHead,psUser,NC_LOGIN_CLIENT,lIp,caMac,caMessage);
    if(psUserInfo == NULL) {
        pasLogs(PAS_SRCFILE,1000,"ncUtlDoLogin Error  User %s  Mesg=%s ",caUname,caMessage);
        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                       "status",  UT_TYPE_LONG,-1);
        return (0);
    }
    if(strcasecmp(utComGetVar_sd(psShmHead,"AuthSendHtml","No"),"No")==0) {
        ncCtlIptDelRedict(lIp);
    }
    

    
    
    
    
    lUserid=psUserInfo->lId;
    lGid= psUserInfo->lGid;
    iReturn = pasDbOneRecord("select compid,authkey from nwcompute where devname = :devname",1,
                    "devname",UT_TYPE_STRING,caUname,
                    UT_TYPE_ULONG,4,&lCompId,
                    UT_TYPE_STRING,16,caKey);
    
      if(iReturn == 0) { /* 记录已存在  */
        utDesEncryptA(caKey,caTemp,NW_DES_KEY);
 /*       
                   utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,5,
                       "status",  UT_TYPE_ULONG,NW_LOG_ISLOGIN,
                       "basetime",UT_TYPE_ULONG,lTime,
                       "id",UT_TYPE_ULONG,lUserid,
                       "key",     UT_TYPE_STRING,caTemp,
                       "did",UT_TYPE_ULONG,lCompId);
 */     
        
    	}
    	else if(iReturn==1403){
            lCompId = pasGetSid("nwcompute","compid");
            
             sprintf(caTemp,"ProNetway%lu%lu",time(0),lCompId);
            utMd5Ascii16(caTemp,strlen(caTemp),"pronetway",caKey);
            iReturn = pasDbExecSqlF("insert into nwcompute(compid,ip,devname,compname,mac,addtime,lasttime,version,authkey) values (%lu,%lu,'%s','%s','%s',%lu,%lu,'%s','%s')",
                        lCompId,lIp,caUname,caUname,"",time(0),time(0),caVersion,caKey);
           if(iReturn != 0) {
                pasLogs(PAS_SRCFILE,1000,"Insert Error %s ",pasDbErrorMsg(NULL));
                utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                       "status",  UT_TYPE_LONG,12);
           	    return 0;
    		   }
    		   else{
    		   	utDesEncryptA(caKey,caTemp,NW_DES_KEY);
    		   	psComp =  (nwCompInfo *)utShmHashLookA(psShmHead,NC_LNK_COMPUTE,&lCompId);
                if(psComp) {
                    psComp->ip = lIp;
                    strcpy(psComp->devname,caUname);
                    strcpy(psComp->compname,caUname);
                    
                    strcpy(psComp->version,caVersion);
                    strcpy(psComp->caKey,caKey);
                    psComp->userid=lUserid;
                    psComp->groupid = lGid;
                }	 
                else {
                    pasLogs(PAS_SRCFILE,1000,"NC_LNK_COMPUTE  utShmHashLookA Error");
                
                }  	
    		  }
       }
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&lIp); 
 //      pasLogs(PAS_SRCFILE,1000,"get CompIp %s",caUname); 
    if(psCompIp) { /* 用户已存在  */
        if(psCompIp->compid == lCompId) {
            psComp = psCompIp->psComp;
            psCompIp->lLastTime = lTime;           
            utDesEncryptA(psCompIp->caKey,caTemp,NW_DES_KEY); 
            pasLogs(PAS_SRCFILE,1000," User already exist");
           utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,5,
                       "status",  UT_TYPE_ULONG,NW_LOG_ISLOGIN,
                       "basetime",UT_TYPE_ULONG,lTime,
                       "id",UT_TYPE_ULONG,lUserid,
                       "key",     UT_TYPE_STRING,caTemp,
                       "did",UT_TYPE_ULONG,lCompId);
                   
            return 0;
        }
        else { /* IP地址不一致,用户需要重新登录  */   
            pasLogs(PAS_SRCFILE,1000,"地址不一致，%s 用户退出",utComHostIp(htonl(lIp)));
            nwUtlLogOut(psShmHead,psCompIp);  /* 用户退出  */
        }
    }
    else {
        
    }   
 
    psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lCompId);
    if(psComp) {
        utMd5Ascii16(psComp->caKey,strlen(psComp->caKey),NW_DES_KEY,caKey1);
        psCompIp = (nwCompIp *)utShmHashLookA(psShmHead,NC_LNK_ONLINECOMP,&lIp);
        if(psCompIp == NULL) {
            pasLogs(PAS_SRCFILE,1000,"NC_LNK_ONLINECOMP  utShmHashLookA Error %s ",utComHostIp(htonl(lIp)));
            utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                            "status",  UT_TYPE_ULONG,NW_ERR_MAXUSER,
                            "message", UT_TYPE_STRING,"Exceed Max User");  
            return 0;
            }
            else {
                psCompIp->psComp = psComp;
                psComp->status = 1;
                psCompIp->lLoginTime = lTime;
                psCompIp->lLastTime = lTime;
                psCompIp->nUdp = (unsigned short)1004;
                psCompIp->nTcp = (unsigned short)1004;
                psCompIp->ip  = lIp;
                psCompIp->compid = lCompId;
                sprintf(caTemp,"pronetway%lu%lu",lTime,lCompId);
                utMd5Ascii16(caTemp,strlen(caTemp),NW_DES_KEY,psCompIp->caKey);
                
                
                iReturn = usbKeyUtlLogin(psShmHead,psComp,lIp,caVersion); 
     pasLogs(PAS_SRCFILE,1000,"usbkeylong %s,iReturn=%d",caUname,iReturn);               
                if(iReturn == 0) {
                    utDesEncryptA(psCompIp->caKey,caTemp,NW_DES_KEY);
                    utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,5,
                        "status",  UT_TYPE_ULONG,0,
                        "basetime",UT_TYPE_ULONG,lTime,
                        "id",UT_TYPE_ULONG,lUserid,
                        "key",     UT_TYPE_STRING,caTemp,
                        "did",UT_TYPE_ULONG,lCompId);
                       
                }
                else {
                    utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,5,
                        "status",  UT_TYPE_LONG,1,
                         "basetime",UT_TYPE_ULONG,lTime,
                         "id",UT_TYPE_ULONG,lUserid,
                         "key",     UT_TYPE_STRING,caTemp,
                         "did",UT_TYPE_ULONG,lCompId);
                        
                }  
                     
                return 0;
            }
       }
       else{
         pasLogs(PAS_SRCFILE,1000,"psComp Error %lu ",lCompId);
       	 utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,3,
                            "status",  UT_TYPE_ULONG,-1,
                            "basetime",UT_TYPE_ULONG,lTime,
                            "key",     UT_TYPE_STRING,caTemp);
      }
      return 0;
        
   
}




/* 处理审计USBKEY心跳信息  */
int usbKeyUdpHeartInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead)
{
    int iReturn;
    unsigned long lHtime,lStime,lCtime,lCompId,lTime,lIp,lSendtime;
    char caVersion[10],caKey[18],caMac[20];
    unsigned long lFlow,lNetlink;
    ncUserInfo *psUserInfo;
    nwCompInfo *psComp;
    nwCompIp   *psCompIp;
    long lProclasttime;
    long lMflag;
    long lUserid;
    
  
#ifdef LDEBUG
    printf(" It Is Heart Info....\n");
    utMsgPrintMsg(psMsgHead);
#endif

   
     
    iReturn = utMsgGetSomeNVar(psMsgHead,2,
		"sendtime",UT_TYPE_LONG,4, &lSendtime,
		"version",   UT_TYPE_STRING,8,caVersion);
		
    /* 数据报校验  */
    lTime = time(0);
    lCompId = psMsgHead->lReserve[0];
    lIp = ntohl(psSockAddr->sSin.sin_addr.s_addr);
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&lIp);
    if(psCompIp) {
//宝信发展修改 2011/04/21
 //       if(psCompIp->lLastTime > lSendtime || lSendtime > lTime + 60) {
//            utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
//                             "status",  UT_TYPE_ULONG,NW_ERR_RELOGIN,
//                             "now",     UT_TYPE_ULONG,lTime);
 //    pasLogs(PAS_SRCFILE,1000,"It is invalid time  lTime=%lu LastTime=%lu ",lTime,psCompIp->lLastTime);                         
//#ifdef LDEBUG
 //           printf(" It is invalid time  lTime=%lu LastTime=%lu \n",lTime,psCompIp->lLastTime);
//#endif
//            return 0;
//        }
        
        if(!utMsgCheckSum(psMsgHead,psCompIp->caKey)) { /* 数据不合法  */
#ifdef LDEBUG
            unsigned long lCode;
            pasLogs(PAS_SRCFILE,1000," It is invalid message, CheckSum Error key=%s",psCompIp->caKey);      
          
            lCode = utMd5Code( (char *)psMsgHead + sizeof(utMsgHead),
                        psMsgHead->lBytes - sizeof(utMsgHead),psCompIp->caKey);
            printf(" CheckSum==%lu   %lu    %lu \n",
                      lCode,ntohl(lCode),psMsgHead->lCheckSum);                        
#endif        
            return 0;
        }
        if(psCompIp->compid != lCompId) {
            printf(" Id different [%lu]  id==%lu\n",psCompIp->compid,lCompId);
            utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                             "status",  UT_TYPE_ULONG,NW_ERR_RELOGIN,
                             "now",     UT_TYPE_ULONG,lTime);
            return 0;
            
        }
        else {
            psCompIp->lLastTime = lTime;

           
            /* 检查当前状态*/
            if(psCompIp->psComp) {	
            	    psUserInfo = ncGetUserInfoByIp(psShmHead,psCompIp->ip);
                	if(psUserInfo==NULL){                   
               		   utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                       "status",  UT_TYPE_ULONG,NW_ERR_RELOGIN,
                       "now",     UT_TYPE_ULONG,lTime);
                        return 0;
                	}
            	    psUserInfo->lCltLastTime=lTime;
        	
            
            	}
            	
             utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                            "status",  UT_TYPE_ULONG,0,
                            "now",     UT_TYPE_ULONG,lTime);	
            	
            	
#ifdef LDEBUG
                printf(" Htime:%lu [%lu]  Stime=%lu [%lu]  Ctime=%lu \n",
                       psCompIp->psComp->htime,lHtime,psCompIp->psComp->stime,lStime,psCompIp->psComp->ctime);
                       
                       
#endif                
            

        }
    }
    else {   /* 要求客户端重新登录  */  
        pasLogs(PAS_SRCFILE,1000," Request Client Relogin");   
        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                            "status", UT_TYPE_ULONG,NW_ERR_NOLOGIN,
                            "now",     UT_TYPE_ULONG,lTime);
//      printf("rlogin=%d\n",NW_ERR_NOLOGIN);
    }
    return 0;
}



/*测试UDP */



