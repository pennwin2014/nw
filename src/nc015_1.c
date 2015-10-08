#define  PAS_SRCFILE      2115 
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#ifdef NC_V35
#include "ncdef_o.h"
#else
#include "ncdef.h"
#endif
#include "nclimit.h"
#include "nwdef.h"
#include "pasutl.h"
static int  iSumAuthRoute=0;
ncAuthRoute *psAuthRoute=NULL;

//判断是否手机  caUsragent 为手机HTTP引擎，返回值1为手机 0-PC
int ncIsMobile(char* caUsragent){
	int lFlag=0;
	int    lLogin=0;
      utStrToLower(caUsragent);
		    if(strstr(caUsragent,"mozilla")&&strlen(caUsragent)>0){
		    	lFlag=1;	    	
		    }
		    else if(strlen(caUsragent)>0){
		    	lLogin=1;
		    }
		    else{
		    	lLogin=0;
		    }
		    
		    if(lFlag>0){
		    	if(strstr(caUsragent,"android"))
		    	  lLogin=1;
		     if(strstr(caUsragent,"apple"))
		    	  lLogin=1;
		     if(strstr(caUsragent,"iphone"))
		    	  lLogin=1;
		     if(strstr(caUsragent,"windows ce"))
		    	  lLogin=1;
		      if(strstr(caUsragent,"blackberry"))
		    	  lLogin=1;
		    	if(strstr(caUsragent,"symbian"))
		    	  lLogin=1;
		    }
		    
		    return lLogin;
}

//判断验证页面的目录
char *ncGetAuthPath(utShmHead *psShmHead,int lLogin,int lEng)
{
  static char caPlatePath[256];
  memset(caPlatePath,0,sizeof(caPlatePath));
  if((strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"mobile")==0||strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"GSMModen")==0)&&(strcasecmp(utComGetVar_sd(psShmHead,"LocalAuth","Yes"),"NTDOMAIN")==0)){
  	sprintf(caPlatePath+strlen(caPlatePath),"/sms");
  }
   else if((strcasecmp(utComGetVar_sd(psShmHead,"StartSysJGUser","No"),"Yes")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"HotelAuth","No"),"Yes")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"center")==0&&strcasecmp(utComGetVar_sd(psShmHead,"LocalAuth","Yes"),"NTDOMAIN")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"StartXMZAJK","No"),"Yes")==0)){
  	sprintf(caPlatePath+strlen(caPlatePath),"/hotel");
  }
  if(lEng==1){
    sprintf(caPlatePath+strlen(caPlatePath),"/%s","english");
    if(lLogin==1){
    	  sprintf(caPlatePath+strlen(caPlatePath),"/%s","sj");
    	  	   		//手机英文页面
    	  	   		 //是否新网116116116验证
    	  if(strcasecmp(utComGetVar_sd(psShmHead,"116sms","Yes"),"Yes")==0&&strcasecmp(utComGetVar_sd(psShmHead,"StartSysJGUser","No"),"Yes")!=0&&strcasecmp(utComGetVar_sd(psShmHead,"HotelAuth","No"),"Yes")!=0){
    	  	sprintf(caPlatePath+strlen(caPlatePath),"/116sms");
    	  }    	  	   		   
    }
    else{
 //   	  sprintf(caPlatePath+strlen(caPlatePath),"/%s","pc");
    	  if(strcasecmp(utComGetVar_sd(psShmHead,"116sms","Yes"),"Yes")==0&&strcasecmp(utComGetVar_sd(psShmHead,"StartSysJGUser","No"),"Yes")!=0&&strcasecmp(utComGetVar_sd(psShmHead,"HotelAuth","No"),"Yes")!=0){
    	  	sprintf(caPlatePath+strlen(caPlatePath),"/116sms");
        }
    }
  }
  else{
 //   	 sprintf(caPlatePath+strlen(caPlatePath),"/%s","chinese");
    	 if(lLogin==1){
    	  	sprintf(caPlatePath+strlen(caPlatePath),"/sj");
   // 	  if(strcasecmp(utComGetVar_sd(psShmHead,"116sms","Yes"),"Yes")==0&&strcasecmp(utComGetVar_sd(psShmHead,"StartSysJGUser","No"),"Yes")!=0&&strcasecmp(utComGetVar_sd(psShmHead,"HotelAuth","No"),"Yes")!=0){
   if(strcasecmp(utComGetVar_sd(psShmHead,"116sms","Yes"),"Yes")==0&&(!strstr(caPlatePath,"hotel"))){
    	  	   sprintf(caPlatePath+strlen(caPlatePath),"/116sms");
	    	  }    	  	   		  	
    	  	   		  	//手机中文页面
    	  	   		  	
    	  }
    	 else{
//    	  	 sprintf(caPlatePath+strlen(caPlatePath),"/pc");
    	  	   		  	   //是否新网116116116验证
  //  	   if(strcasecmp(utComGetVar_sd(psShmHead,"116sms","Yes"),"Yes")==0&&strcasecmp(utComGetVar_sd(psShmHead,"StartSysJGUser","No"),"Yes")!=0&&strcasecmp(utComGetVar_sd(psShmHead,"HotelAuth","No"),"Yes")!=0){
   if(strcasecmp(utComGetVar_sd(psShmHead,"116sms","Yes"),"Yes")==0&&(!strstr(caPlatePath,"hotel"))){
    	  	   	sprintf(caPlatePath+strlen(caPlatePath),"/116sms");
			    	}
   	            		
    	   }
    }
    printf("caPlatePath=%s\n",caPlatePath);
    	          return &caPlatePath[0];
}

//判断验证页面的目录
char *ncGetAuthPath_2(utShmHead *psShmHead,int lLogin,int lEng)
{
  static char caPlatePath[256];
  memset(caPlatePath,0,sizeof(caPlatePath));
  if(strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"mobile")==0||strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"GSMModen")==0){
  	sprintf(caPlatePath+strlen(caPlatePath),"/sms");
  }
  else if((strcasecmp(utComGetVar_sd(psShmHead,"StartSysJGUser","No"),"Yes")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"HotelAuth","No"),"Yes")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"center")==0&&strcasecmp(utComGetVar_sd(psShmHead,"LocalAuth","Yes"),"NTDOMAIN")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"StartXMZAJK","No"),"Yes")==0)){
  	sprintf(caPlatePath+strlen(caPlatePath),"/hotel");
  }
  if(lEng==1){
    sprintf(caPlatePath+strlen(caPlatePath),"/%s","english");
    if(lLogin==1){
    	  sprintf(caPlatePath+strlen(caPlatePath),"/%s","sj");
    	  	   		//手机英文页面
   	  	   		   
    }

  }
  else{
 //   	 sprintf(caPlatePath+strlen(caPlatePath),"/%s","chinese");
    	 if(lLogin==1){
    	  	sprintf(caPlatePath+strlen(caPlatePath),"/sj");    	  	   		  	
    	  }

    }
    	          return &caPlatePath[0];
}


/*由域插件验证*/
int ncAuthByNtSoft(utShmHead *psShmHead,char *username,char *passwd)
{
    
    unsigned long lIp;

    utMsgHead *psMsgHead0;
    char temp_compname[64];
    char temp_pid[16];
    int iNum,k;
    char caOldpassword[64],caPassword[64],caUsername[32];
    char *pIp,*pPort;
    int iSock1,iReturn;
    utComSockAddr sSockAddr;
    char sBuf[36];
    long addrlen;
    utPltDbHead *psDbHead;
    if((strlen(username)==0)||strlen(passwd)==0)
    return -3;
    
    
    pIp=utComGetVar_s(psShmHead,"NtDoMainServer");
    if(pIp == NULL) {
      
       return -3;
    }
    lIp=utComHostAddress(pIp);
    pPort=utComGetVar_sd(psShmHead,"NtDoMainPort","1001");
  
    
    
    
   
  
   psMsgHead0 = utMsgInit(NW_MSG_CODE_REQ,NC_CLIENT_AUTHBYNT,utStrGetId(),4096);
      utMsgAdd_s(&psMsgHead0,"username",username);
      utMsgAdd_s(&psMsgHead0,"passwd",passwd);
   
    iSock1 = utComUdpSock();

    utComSetAddr0(&sSockAddr,lIp,htons(atol(pPort)));
    iReturn = utComUdpMsgSendTo(iSock1,&sSockAddr,psMsgHead0);
   
    memset(sBuf,0,sizeof(sBuf));
    recvfrom(iSock1,sBuf,30,0,&sSockAddr,&addrlen); 
 printf("sBuf=%s\n",sBuf);   
    close(iSock1);   
    
                 
   utMsgFree(psMsgHead0);
  if(strlen(sBuf)==0)
  strcpy(sBuf,"-1"); 
  
  return atol(sBuf);
}




/* 用户登陆  */
ncUserInfo *ncUtlUserAuth(utShmHead *psShmHead,char *pUsername,char *pPassword,unsigned long lIp,
               char *caMessage);

/* 本地验证登录  */               
int ncWebInternetLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

}




/* 通过Proxy登录  */
int ncWebProxyLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caUsername[32],caIp[20];
   	char caDispname[32];
   	char caGroupname[32],caKey[32];
   	char caPassword[32],caPassword0[32];
   	char caMod[32];
   	char caUrl[256];
   	char caMsg[128];
   	
   	long8 lThisFlow,lFlowLimit;
   	unsigned long lTimeLimit,lThisTime;
   	long lLimitid,lPostflags,lWebipflags;
   	utPltDbHead *psDbHead;
   	unsigned long lUserid,lGroupid,lUseflags,lIp,lRate;
   	int iReturn;
   	ncUserInfo *psUserInfo;
    char *p;
    strcpy(caMod,"\0");
   	iReturn = utMsgGetSomeNVar(psMsgHead,5,
				"username",    UT_TYPE_STRING, 31, caUsername,
				"password",    UT_TYPE_STRING, 31, caPassword,
				"ModiPass",    UT_TYPE_STRING, 31, caMod,
				"myip",       UT_TYPE_STRING, 19, caIp,
				"url",         UT_TYPE_STRING,255,caUrl);
    utStrDelSpaces(caUsername);
    utStrDelSpaces(caPassword); 
    utStrDelSpaces(caIp);
    /* 验证  */
    lIp = ntohl(utComHostAddress(caIp));
    
    psUserInfo = ncUtlUserAuth(psShmHead,caUsername,caPassword,lIp,caMsg);
    if(psUserInfo == NULL) {
         psDbHead = utPltInitDb();
         utPltPutVar(psDbHead,"myurl",utComGetVar_sd(psShmHead,"AdminHost","192.168.0.1"));
         utPltPutVar(psDbHead,"port",utComGetVar_sd(psShmHead,"AdminPort","80"));
         utPltPutVar(psDbHead,"url",caUrl);
         utPltPutVar(psDbHead,"myip",caIp);
         utPltPutVar(psDbHead,"message",caMsg+1);
         utPltPutVarF(psDbHead,"sid","%lu",utStrGetId());
         utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_proxyauth.htm");
         return 0;
    }
    else {
         psDbHead = utPltInitDbHead();
         if(!utStrIsSpaces(caMod)) { /* 修改口令  */
              utPltPutVar(psDbHead,"myurl",utComGetVar_sd(psShmHead,"AdminHost","192.168.0.1"));
              utPltPutVar(psDbHead,"port",utComGetVar_sd(psShmHead,"AdminPort","80"));
              utPltPutVar(psDbHead,"myip",caIp);
              utPltPutVar(psDbHead,"username",caUsername);
              utPltPutVar(psDbHead,"url",caUrl);
              utPltPutVarF(psDbHead,"sid","%lu",utStrGetId());
              utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_proxy_modpass.htm");
         }
         else {
             utPltPutVar(psDbHead,"adminip",utComGetVar_sd(psShmHead,"AdminHost","192.168.0.1"));
             utPltPutVar(psDbHead,"port",utComGetVar_sd(psShmHead,"AdminPort","80"));
             utPltPutVar(psDbHead,"message",ncLang("0197登陆成功，您可以上网了"));
             utPltPutVar(psDbHead,"url",caUrl);
             if(strcasecmp(utComGetVar_sd(psShmHead,"AuthSendHtml","No"),"No")==0) {
                ncCtlIptDelRedict(lIp);
             }
             utPltPutVarF(psDbHead,"sid","%lu",utStrGetId());
             utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_proxyauth_ok.htm");
             if(strcasecmp(utComGetVar_sd(psShmHead,"BandWidthLimit","NO"),"YES") == 0) {
                ncUtlTcSetUserWidthA(psShmHead,psUserInfo);
              	        /*     ncUtlSetBindWidth(lIp,0,0); */
             }
             return 0;
        }
    }                       
    return 0;
}


/* 用户验证    
           

*/

ncUserInfo *ncUtlUserAuth(utShmHead *psShmHead,char *pUsername,char *pPassword,unsigned long lIp,
               char *caMessage)
{
    ncUserInfo *psUserInfo;
   	char caDispname[32],caUsername[32];
   	char caGroupname[32],caKey[32];
   	char caPassword[32],caPassword0[32],caIp0[32],caMac0[32];
   	long8 lThisFlow,lFlowLimit;
   	unsigned long lTimeLimit,lThisTime,lIp0;
   	long lLimitid,lPostflags,lWebipflags;
   	unsigned long lUserid,lGroupid,lUseflags,lRate;
   	int iReturn,iStatus;
   	ncUserCont *psUser;
    char caIp[20];
    char caHost0[64];
    sprintf(caPassword0,"%s%s",pUsername,pPassword);
    utMd5Ascii22(caPassword0,strlen(caPassword0),NULL,caKey);
    iReturn = pasDbOneRecord("select userid,username,dispname,password,groupid,useflags,ip,mac,compname \
                  from ncuser  where username = :name ",
                   1,"name",UT_TYPE_STRING,pUsername,
                   UT_TYPE_LONG,4,&lUserid,
                   UT_TYPE_STRING,31,caUsername,
                   UT_TYPE_STRING,31,caDispname,
                   UT_TYPE_STRING,31,caPassword,
                   UT_TYPE_LONG,4,&lGroupid,
                   UT_TYPE_LONG,4,&lUseflags,
                   UT_TYPE_ULONG,4,&lIp0,
                   UT_TYPE_STRING,31,caMac0,
                   UT_TYPE_STRING,63,caHost0);
    pasLogs(PAS_SRCFILE,1002,"ireturn=%d  Pass=%s %s \n",iReturn,caPassword,caKey);
    if(iReturn == 0 && strcmp(caPassword,caKey)==0) {
        iReturn = pasDbExecSqlF("update ncuser set lasttime = %lu where userid = %lu ", 
                         time(0),lUserid);
        psUser = ncUtlGetUserCont(psShmHead,lUserid);
        psUserInfo = ncUtlDoLogin(psShmHead,psUser,NC_LOGIN_AUTH,lIp,NULL,caMessage);
        return psUserInfo;
    }
    sprintf(caMessage,ncLang("00429用户不存在或口令出错 "));
    return NULL;
    
}


/* 显示Logout画面  */
int ncWebInternetLogout(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caIp[34];
    char caSid[32];
   	char caUsername[34],caGroupname[34],caDip[32],caPort[16],caPlate[128],caPlatePath[64];
    ncUserInfo *psUserInfo;
    
    unsigned long lIp,lGroupid,lUserid,lUse,lSid;
   	int iReturn,i;
   	int iSumRecord;
   	char *p;
	utPltDbHead *psDbHead;
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
   	                "sid",          UT_TYPE_STRING, 16, caSid,
					"clientip",     UT_TYPE_STRING, 16, caIp);
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
    lIp = ntohl(pasIpcvtLong(caIp));	
    if(utStrIsSpaces(caSid)) {
        lSid = lIp;
    }
    else {
        sscanf(caSid,"%lu",&lSid);
    }
    ncUtlGetAuthIp(psShmHead,lIp,caDip,caPort,caPlatePath);
    psDbHead = utPltInitDb();

    psUserInfo = ncGetUserInfoByIp(psShmHead,lIp);

    if(psUserInfo) {
    	
    	      if(psUserInfo->psUser->sSumm.psBill) { /* 计费信息  */
            unsigned long long thisflow,lBytes;
            unsigned long thistime,contime;
            char caTemp[1024];
            double fee;       
            thisflow = psUserInfo->psUser->thisflow / 1024;
            thistime = psUserInfo->psUser->thistime;
            if(psUserInfo->psUser->sSumm.psBill->timeway == BILL_TIMEWAY_CONNECT) {
                contime = time(0) - psUserInfo->lStartTime;
            }
            else {
                contime = psUserInfo->lConnTime;
            }
            if(contime < 1 && contime > 8640000L) {
                contime = 0;
            }
            lBytes = (psUserInfo->lBytes[0] + psUserInfo->lBytes[1]) / 1024;

            fee = billCompOneFee(psUserInfo->psUser->sSumm.psBill,&thistime,&thisflow,
                                 psUserInfo->lStartTime,
                                 contime,lBytes);
        utPltPutVarF(psDbHead,"userid","%lu",psUserInfo->lId);                         
        utPltPutVar(psDbHead,"username",psUserInfo->psUser->username);
        utPltPutVar(psDbHead,"starttime",utTimFormat("%Y-%m-%d %H:%M:%S",psUserInfo->lStartTime));
            utPltPutVarF(psDbHead,"sumfee","%10.2f",fee);
            utPltPutVarF(psDbHead,"totfee","%10.2f",psUserInfo->psUser->thisfee + fee);
            utPltPutVarF(psDbHead,"sumflow",utStrLtoF8( lBytes,caTemp));
            utPltPutVarF(psDbHead,"sumtime","%s",billCvtTime(contime));
            utPltPutVarF(psDbHead,"totflow",utStrLtoF8( thisflow + lBytes,caTemp));
            utPltPutVarF(psDbHead,"tottime",billCvtTime( thistime + contime));
                         
        }
        else{
     	
        	  unsigned long long thisflow,lBytes;
            unsigned long thistime,contime;
            char caTemp[1024];
            thisflow = psUserInfo->psUser->thisflow / 1024;
            thistime = psUserInfo->psUser->thistime;
            contime = psUserInfo->lConnTime;
            lBytes = (psUserInfo->lBytes[0] + psUserInfo->lBytes[1]) / 1024;
            utPltPutVar(psDbHead,"username",psUserInfo->psUser->username);
            utPltPutVarF(psDbHead,"userid","%lu",psUserInfo->lId);        
            utPltPutVar(psDbHead,"starttime",utTimFormat("%Y-%m-%d %H:%M:%S",psUserInfo->lStartTime));
            utPltPutVarF(psDbHead,"sumflow",utStrLtoF8( lBytes,caTemp));
            utPltPutVarF(psDbHead,"sumtime","%s",billCvtTime(contime));
            utPltPutVarF(psDbHead,"totflow",utStrLtoF8( thisflow + lBytes,caTemp));
            utPltPutVarF(psDbHead,"tottime",billCvtTime( thistime + contime));
         
        }
    	
    	
    	
    	
//    	printf("pid=%d\n",getpid());
//    	sleep(20);
    	
    	
        ncUtlDoLogout(psShmHead,psUserInfo,"User Logout at Web");

    }

    utPltPutVar(psDbHead,"ip",caDip);
    utPltPutVar(psDbHead,"port",caPort);
    utPltPutVarF(psDbHead,"sid","%lu",lSid);
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,utStrCompose(caPlate,"%s/nc_login_exit.htm",caPlatePath));
    return 0;
}


/* 修改密码  */
int ncWebProxyModpass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caUsername[32],caPassword[32],caPassword1[32],caIp[32],caKey[32];
    char caUrl[256];
    int iReturn;
    unsigned long lIp;
   	utPltDbHead *psDbHead;
    ncUserInfo *psUserInfo;	
   	iReturn = utMsgGetSomeNVar(psMsgHead,5,
				"username",     UT_TYPE_STRING, 31, caUsername,
				"password",     UT_TYPE_STRING, 31, caPassword,
				"password1",    UT_TYPE_STRING, 31, caPassword1,
				"myip",         UT_TYPE_STRING, 20, caIp,
				"url",          UT_TYPE_STRING,255,caUrl);
    utStrDelSpaces(caUsername);
    psUserInfo = ncGetUserInfoByName(psShmHead,caUsername);
    if(psUserInfo == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),ncLang("0422您还没有登陆"));
        return 0;
    }
    lIp = ntohl(utComHostAddress(caIp));
    if(psUserInfo->lIp != lIp) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),ncLang("0248非正常修改用户口令"));
        return 0;
    }
    if(strcmp(caPassword,caPassword1) != 0) { /* 确认口令不一致  */
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),ncLang("0352口令不一致"));
        return 0;
    }
    sprintf(caPassword1,"%s%s",caUsername,caPassword);
    utMd5Ascii22(caPassword1,strlen(caPassword1),NULL,caKey);
    iReturn = pasDbExecSqlF("update ncuser set password='%s',lasttime = %lu where username='%s'",
                 caKey,time(0),caUsername);
    if(iReturn == 0) {
        psDbHead = utPltInitDb();
        utPltPutVar(psDbHead,"url",caUrl);
        utPltPutVar(psDbHead,"myip",utComGetVar_sd(psShmHead,"AdminHost","192.168.0.1"));
        utPltPutVar(psDbHead,"port",utComGetVar_sd(psShmHead,"AdminPort","8080"));
        utPltPutVar(psDbHead,"message",ncLang("0390密码修改成功，您可以上网了"));
        if(strcasecmp(utComGetVar_sd(psShmHead,"AuthSendHtml","No"),"No")==0) {
             ncCtlIptDelRedict(lIp);
        }
        utPltPutVarF(psDbHead,"sid","%lu",lIp);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_proxyauth_ok.htm");
        pasDbCommit(NULL);
    }
    else {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),ncLang("0388密码修改不成功 %d"),iReturn);
        pasDbRollback(NULL);
    }        
    return 0;
}


/* 修改密码  */
int ncWebInternetModpass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caUsername[32],caPassword[32],caPassword1[32],caIp[32],caKey[32];
    int iReturn;
    unsigned long lIp;
    char caOldpass[32];
   	utPltDbHead *psDbHead;
    ncUserInfo *psUserInfo;	
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,5,
				"username",     UT_TYPE_STRING, 31, caUsername,
				"password",     UT_TYPE_STRING, 31, caPassword,
				"password1",    UT_TYPE_STRING, 31, caPassword1,
				"clientip",    UT_TYPE_STRING, 20, caIp,
				"oldpassword",  UT_TYPE_STRING,31,caOldpass);
	//	 if(strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"mobile")!=0){	
		if(iReturn==5){
			char caPw[32],caPassword0[64],caKey[32];
			char caTemp[1024];
			strcpy(caPw,"-1+++");
			sprintf(caTemp,"select password from ncuser where username='%s' ",caUsername);
			pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caPw);

			 sprintf(caPassword0,"%s%s",caUsername,caOldpass);
			utMd5Ascii22(caPassword0,strlen(caPassword0),NULL,caKey);
	
			if(strcmp(caPw,caKey)!=0){
				utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),"旧密码不正确");
        return 0;
			}
//		}
				
			}	
				
    utStrDelSpaces(caUsername);
    psUserInfo = ncGetUserInfoByName(psShmHead,caUsername);
    if(psUserInfo == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),ncLang("0422您还没有登陆"));
        return 0;
    }
    lIp = ntohl(pasIpcvtLong(caIp));
    if(psUserInfo->lIp != lIp) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),ncLang("0248非正常修改用户口令"));
        return 0;
    }
    if(strcmp(caPassword,caPassword1) != 0) { /* 确认口令不一致  */
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),ncLang("0352口令不一致"));
        return 0;
    }
    

//远程手机验证
    if((strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"mobile")==0)&&(strcasecmp(utComGetVar_sd(psShmHead,"LocalAuth","Yes"),"NTDOMAIN")==0))
     {
          iReturn = ncModMobilePasswd(psShmHead,caUsername,caPassword);
         
     }
    else{
        iReturn = pasDbExecSqlF("update ncmobuser set password='%s' where username='%s'",
                 caPassword,caUsername);
                 
    sprintf(caPassword1,"%s%s",caUsername,caPassword);
    utMd5Ascii22(caPassword1,strlen(caPassword1),NULL,caKey);
    iReturn = pasDbExecSqlF("update ncuser set password='%s',lasttime = %lu where username='%s'",
                 caKey,time(0),caUsername);

    }
    if(iReturn == 0) {
        psDbHead = utPltInitDb();
        char caDip[20],caPort[16],caPlatePath[128];
        ncUtlGetAuthIp(psShmHead,lIp,caDip,caPort,caPlatePath);
        
//        utPltPutVar(psDbHead,"ip",utComGetVar_sd(psShmHead,"AdminHost","192.168.0.1"));
//        utPltPutVar(psDbHead,"port",utComGetVar_sd(psShmHead,"AuthHttpPort","8080"));
        utPltPutVar(psDbHead,"ip",caDip);
        utPltPutVar(psDbHead,"port",caPort);

        utPltPutVar(psDbHead,"message",ncLang("0390密码修改成功，您可以上网了"));
        utPltPutVarF(psDbHead,"sid","%lu",lIp);
        utPltPutVar(psDbHead,"username",caUsername);

        
        
        if(ncIpControlBy()) {
        
							      if(strcasecmp(utComGetVar_sd(psShmHead,"BeatHeat","NO"),"YES") == 0){
									  	utPltPutVar(psDbHead,"beat_flag","1");
									  	long lBeattime=1000/3*utComGetVar_ld(psShmHead,"beattimeout",3600);
									  	utPltPutVarF(psDbHead,"beattime","%d",lBeattime);
									  }
        	          if(strcasecmp(utComGetVar_sd(psShmHead,"AuthSendHtml","No"),"No")==0) {
                        ncCtlIptDelRedict(lIp);
                    }
        	
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_login_ok.htm");
        }
        else {
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncmsg_back.htm");
        }            
        pasDbCommit(NULL);
    }
    else {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),ncLang("0388密码修改不成功 %d"),iReturn);
        pasDbRollback(NULL);
    }        
    return 0;
}



/* 显示登陆画面  */
int ncWebDispLogin2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caIp[128];
    char caUri[128];
   	char caPort[16];
   	char caPlatePath[64],caPlate[128];
   	utPltDbHead *psDbHead;
   	int iReturn;
   	unsigned long lIp;
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
				"clientip",    UT_TYPE_STRING, 20, caIp,
				"httpref",     UT_TYPE_STRING, 127,caUri);
    
    lIp = ntohl(pasIpcvtLong(caIp));
    ncUtlGetAuthIp(psShmHead,lIp,caIp,caPort,caPlate);
    
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"ip",caIp);
    utPltPutVar(psDbHead,"port",caPort);
    utPltPutVar(psDbHead,"uri",caUri);
    utPltPutVarF(psDbHead,"sid","%lu",lIp);
    sprintf(caPlate,"%s/nc_login2.htm",caPlatePath);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}


int ncWebDispLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caIp[128];
    char caUrl[2560];
    char caPlatePath[64],caPlate[128],caPort[16];
    char caSid[16];
    char caSrvname[64],caRequir[2560];
    char caFlag[8];
    char caUsragent[256];
    char caMac[20];
    char caUrl2[2560];
    char caTemp2[1024];
    char *p;
    int iReturn;
    unsigned long lIp,lSid;
    long lFlag,lLogin;
    char caEng_flag[16];   
    char caPath_all[256];
    char caLogincount[16];
    char caMac1[16];
   	utPltDbHead *psDbHead;
    pasDbCursor *psCur;
    
    utMsgPrintMsg(psMsgHead);

  	iReturn = utMsgGetSomeNVar(psMsgHead,10,
				"clientip",    UT_TYPE_STRING, 20, caIp,
				"httpref",     UT_TYPE_STRING,127,caUrl,
				"sid",         UT_TYPE_STRING, 11, caSid,
				"srvname",     UT_TYPE_STRING,63,caSrvname,
				"requri",      UT_TYPE_STRING,2000,caRequir,
				"flag",        UT_TYPE_STRING,1,caFlag,
				"usragent",    UT_TYPE_STRING,255,caUsragent,
				"eng_flag",    UT_TYPE_STRING,8,caEng_flag,
				"logincount",  UT_TYPE_STRING,8,caLogincount,
				"url",         UT_TYPE_STRING,2048,caUrl2);
				
				if(strstr(caUrl2,"URL=http")){
					strcpy(caRequir,caUrl2);
				}
    if(pasLogsIsOut(PAS_SRCFILE,1001,NULL)) {
        utMsgPrintMsg(psMsgHead);
    }
 //   printf("caIp=%s\n",caIp);

    lIp = ntohl(pasIpcvtLong(caIp));	
    iReturn = ncUtlUserBaseIs(psShmHead,lIp);
 //   printf("iReturn=%d\n",iReturn);
    if(iReturn==NC_NETWORK_CLIENT){
    	//弹出客户端下载页面
    	      psDbHead = utPltInitDb();
    	      utPltPutVar(psDbHead,"ip",utComGetVar_sd(psShmHead,"AdminIp",""));
    	      utPltPutVar(psDbHead,"adminport",utComGetVar_sd(psShmHead,"AdminPort","80"));
    	      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_downclient.htm");
            return 0;
    }
    /* 单点登录  */
    if(iReturn == NC_NETWORK_SPECLOGIN) { 
    	
        char caBuf[1024];
        char *pUrl = ncUtlGetOneLoginUrl();
        if(pUrl) {
/*            
            sprintf(caBuf,"\n\n<html>\r\n<head>\r\n");
            utStrAddF(caBuf,"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">\r\n");
            utStrAddF(caBuf,"<META HTTP-EQUIV=\"Pragma\" CONTENT=\"no-cache\">\r\n");
            utStrAddF(caBuf,"<META HTTP-EQUIV=\"Jon Eyrick\" CONTENT=\"no-cache\">\r\n");
            utStrAddF(caBuf,"</head>\r\n<body  onload=\"self.location='%s' \">\r\n</body>\r\n</html>",pUrl);
            utComTcpResponse(iFd,psMsgHead,1,
                       "html",UT_TYPE_STRING,caBuf);
                       
*/
            psDbHead = utPltInitDb();
            utPltPutVar(psDbHead,"reurl",pUrl);
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_login_one.htm");
            return 0;
        }
    }
    lFlag=0;
    lLogin=0;
    lLogin=ncIsMobile(caUsragent);

			
			
			
			if(atol(caFlag)==1)
			  strcpy(caUrl,"");
			else	
				sprintf(caUrl,"%s",caRequir);


    psDbHead = utPltInitDb();
    
    utPltPutVar(psDbHead,"cip",caIp);
    
    ncUtlGetAuthIp(psShmHead,lIp,caIp,caPort,caPlatePath);
    utPltPutVar(psDbHead,"logincount",caLogincount);
    
    if(utStrIsSpaces(caSid)) {
        lSid = lIp;
    }
    else {
        sscanf(caSid,"%lu",&lSid);
    }
//  printf("caIp=%s\n",caIp);                    
    utPltPutVar(psDbHead,"ip",caIp);
    utPltPutVar(psDbHead,"port",caPort);
    utPltPutVar(psDbHead,"adminport",utComGetVar_sd(psShmHead,"AdminPort","80"));
    utPltPutVar(psDbHead,"siteid",utComGetVar_sd(psShmHead,"NcName",""));
     pasCharsetCvt(PAS_CCODE_GB,PAS_CCODE_UTF8,utComGetVar_sd(psShmHead,"rootname",""),caTemp2,63);  
    utPltPutVar(psDbHead,"sitename",caTemp2);
    
    ncUtlGetMacFromIpMacLst(psShmHead,lIp,caMac1);
   strcpy(caMac,ncCvtMac(caMac1));
 //  printf("caMac=%s\n",caMac);
   utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"url",caUrl);
    utPltPutVarF(psDbHead,"sid","%lu",lSid);
    p = utComGetVar_sd(psShmHead,"LocalAuth","Yes");
    if(strcasecmp(p,"IP") == 0 || strcasecmp(p,"mac") == 0) {  /* 基于IP地址   */
        sprintf(caPlate,"%s/nc_login_noauth.htm",caPlatePath);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
        return 0;
    }
    else {
    	  char news_title[256],sqlbuf[1024];
    	  int iNum,iReturn,lSid,lStime;
    		sprintf(sqlbuf,"select sid,news_title,stime from ncnews where dflag=0 order by stime desc limit 0,3 ");
        psCur = pasDbOpenSql(sqlbuf,0);
        if(psCur!=NULL){
        	memset(news_title,0,sizeof(news_title));
        	  iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid, 
        															UT_TYPE_STRING,255,news_title,
        															UT_TYPE_LONG,4,&lStime);                                  
		  iNum=0;
			 while((iReturn==0)||(iReturn==1405)){
				 	  iNum++;
				 	  
				 	                  	
				 	  
				 	  
				    utPltPutLoopVar(psDbHead,"news_title",iNum,news_title);
				    
				    pasCharsetCvt(PAS_CCODE_GB,PAS_CCODE_UTF8,news_title,caTemp2,100);  
            utPltPutLoopVar(psDbHead,"news_title_u",iNum,caTemp2); 
				    
				 	  utPltPutLoopVarF(psDbHead,"iNum",iNum,"%lu",iNum);
				 	  utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
				 	  utPltPutLoopVar(psDbHead,"stime",iNum,utTimFormat("%Y/%m/%d",lStime));
				 	  memset(news_title,0,sizeof(news_title));
        	  iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid, 
        															UT_TYPE_STRING,255,news_title,
        															UT_TYPE_LONG,4,&lStime); 
				   }
    	    pasDbCloseCursor(psCur);
    	  }
    	  
    	  
//lLogin=1;
 //   	  if(lLogin==1){
  //  	        sprintf(caPlate,"%s/nc_login_main_mob.htm",caPlatePath);
  //    	  	    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
 //   	  }
 //   	  else{
// printf("lLogin=%d,caUsragent=%s,caEng_flag=%s\n",lLogin,caUsragent,caEng_flag);
    	  	   if((strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"mobile")==0||strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"GSMModen")==0)&&(strcasecmp(utComGetVar_sd(psShmHead,"LocalAuth","Yes"),"NTDOMAIN")==0)){
    	  	 
    	  	
                 sprintf(caPlate,"%s%s/nc_login_main.htm",caPlatePath,ncGetAuthPath(psShmHead,lLogin,atol(caEng_flag)));

//                 printf("11caPlate=%s\n",caPlate);
                 sprintf(caPath_all,"/home/ncmysql/nc/plate/%s",caPlate);
			          if(!utFileIsExist(caPath_all)){
			          	strcpy(caPlate,"nc/nc_login_main_sb.htm");
			          }
//           printf("caPlate=%s\n",caPlate);
    	 	       utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    	 	       return 0;
    	          }
    	          else if((strcasecmp(utComGetVar_sd(psShmHead,"StartSysJGUser","No"),"Yes")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"HotelAuth","No"),"Yes")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"center")==0&&strcasecmp(utComGetVar_sd(psShmHead,"LocalAuth","Yes"),"NTDOMAIN")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"StartXMZAJK","No"),"Yes")==0)){
    	          	
//    	    printf("caPlate=%s\n",caPlate);
          sprintf(caPlate,"%s%s/nc_login_main.htm",caPlatePath,ncGetAuthPath(psShmHead,lLogin,atol(caEng_flag)));
//                 printf("caPlate=%s\n",caPlate);
          sprintf(caPath_all,"/home/ncmysql/nc/plate/%s",caPlate);
          if(!utFileIsExist(caPath_all)){
          	strcpy(caPlate,"nc/nc_login_main_jg.htm");
          }
   //        printf("11caPlate=%s\n",caPlate);


                utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    	          	return 0;
    	          }
    	        else{
    	        	if(atol(caEng_flag)==1){
    	        		sprintf(caPlate,"%s/english/nc_login_main.htm",caPlatePath);
    	        	}
    	        	else{
    	          	sprintf(caPlate,"%s/nc_login_main.htm",caPlatePath);
    	          }
    
                utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
                return 0;
              }
  //      }

    }

    return 0;
}


//显示手机验证界面
int ncWebDispLoginMob(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caIp[128];
    char caUrl[128];
    char caPlatePath[64],caPlate[128],caPort[16];
    char caSid[16];
    char caSrvname[64],caRequir[256];
    char caFlag[8];
    char caUsragent[256];
    char *p;
    int iReturn;
    unsigned long lIp,lSid;
    long lFlag,lLogin;
    char caEng_flag[16]; 
    char caPath_all[256];  
    char caUrl2[256];
   	utPltDbHead *psDbHead;
    pasDbCursor *psCur;
   	iReturn = utMsgGetSomeNVar(psMsgHead,9,
				"clientip",    UT_TYPE_STRING, 20, caIp,
				"httpref",     UT_TYPE_STRING,127,caUrl,
				"sid",         UT_TYPE_STRING, 11, caSid,
				"srvname",     UT_TYPE_STRING,63,caSrvname,
				"requri",      UT_TYPE_STRING,255,caRequir,
				"flag",        UT_TYPE_STRING,1,caFlag,
				"usragent",    UT_TYPE_STRING,255,caUsragent,
				"eng_flag",    UT_TYPE_STRING,8,caEng_flag,
				"url",         UT_TYPE_STRING,255,caUrl2);
   
   		if(strstr(caUrl2,"URL=http")){
					strcpy(caRequir,caUrl2);
			}
   
    lIp = ntohl(pasIpcvtLong(caIp));	
  
    lFlag=0;
    	
    lLogin=0;
    lLogin=ncIsMobile(caUsragent);
    psDbHead = utPltInitDb();
    ncUtlGetAuthIp(psShmHead,lIp,caIp,caPort,caPlatePath);

    if(utStrIsSpaces(caSid)) {
        lSid = lIp;
    }
    else {
        sscanf(caSid,"%lu",&lSid);
    }
                      
    utPltPutVar(psDbHead,"ip",caIp);
    utPltPutVar(psDbHead,"port",caPort);
    utPltPutVar(psDbHead,"adminport",utComGetVar_sd(psShmHead,"AdminPort","80"));

    utPltPutVarF(psDbHead,"sid","%lu",lSid);
   if((strcasecmp(utComGetVar_sd(psShmHead,"StartSysJGUser","No"),"Yes")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"HotelAuth","No"),"Yes")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"center")==0&&strcasecmp(utComGetVar_sd(psShmHead,"LocalAuth","Yes"),"NTDOMAIN")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"StartXMZAJK","No"),"Yes")==0)){
     	 sprintf(caPlate,"%s%s/nc_login_main_hotelmob.htm",caPlatePath,ncGetAuthPath(psShmHead,lLogin,atol(caEng_flag)));
    }
    else{
   	 		sprintf(caPlate,"%s%s/nc_login_main.htm",caPlatePath,ncGetAuthPath(psShmHead,lLogin,atol(caEng_flag)));
  	}
                 printf("caPlate=%s\n",caPlate);
                 sprintf(caPath_all,"/home/ncmysql/nc/plate/%s",caPlate);
          if(!utFileIsExist(caPath_all)){
          	strcpy(caPlate,"nc/nc_login_main_sb.htm");
          }
           printf("caPlate=%s\n",caPlate);
   				sprintf(caUrl,"%s",caRequir);
   				utPltPutVar(psDbHead,"url",caUrl);
   				 	  	  
  //  sprintf(caPlate,"%s/nc_login_main_sb.htm",caPlatePath);

    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
   
    return 0;
}








/* 远程文件系统  */
int ncUtlRemoteFile(utShmHead *psShmHead,ncIpControlInfo *psControl,struct ncTestRemote_s *psBuf,char *caMsg)
{
    int iReturn;
    strcpy(caMsg,"\0");
    if(psControl->cProt == NC_REMOTE_MOUNT || psControl->cProt == NC_REMOTE_MOUNTTEST) {
        if(strcasecmp(utComGetVar_sd(psShmHead,"RemoteFs","smbfs"),"cifs")==0) {
            iReturn = pasUtlMountCifFs(psBuf->caIp,psBuf->caHost,psBuf->caShare,psBuf->caUser,
                         psBuf->caPass,psBuf->caMount);
        }
        else {
            iReturn = pasUtlMountSmbFs(psBuf->caIp,psBuf->caHost,psBuf->caShare,psBuf->caUser,
                         psBuf->caPass,psBuf->caMount);
        }
        if(iReturn == 0) {
            char caFile[128];
            FILE *fp;
            char caFile1[32];
            if(psControl->cProt == NC_REMOTE_MOUNTTEST) {
                    sprintf(caFile1,"nctest%d.txt",utStrGetId());
                    sprintf(caFile,"%s/%s",psBuf->caMount,caFile1);
                    fp = fopen(caFile,"w");
                    if(fp == NULL) {
                        sprintf(caMsg,ncLang("00391文件传送失败"));
                        iReturn = 1;
                    }
                    else {
                        fprintf(fp,"This Is test file\n");
                        fclose(fp);
                        sprintf(caMsg,ncLang("00360文件传送测试成功, 在共享目录上您将会看到文件 %s\n"),caFile1);
                        pasUtlUmountSmbFs(psBuf->caMount);
                    }
            }
            else {
                    sprintf(caMsg,ncLang("00370文件装载成功\n"));
            }
            iReturn = 0;
        }
        else {
            sprintf(caMsg,ncLang("00402系统连接不成功，请检查配置 %d\n"),iReturn);
            iReturn = 2;
        }
    }
    else if(psControl->cProt == NC_REMOTE_UNMOUNT) {
        iReturn = pasUtlUmountSmbFs(psBuf->caMount);
        sprintf(caMsg,"%d\n",iReturn);
    }
    else if(psControl->cProt == NC_REMOTE_MAILLOAD) {
        sprintf(caMsg,"0\n");
        iReturn = ncUtlRemoteLoadMail(psShmHead,psBuf);
        if(iReturn < 0) {
           sprintf(caMsg,ncLang("00402系统连接不成功，请检查配置 %d\n"),iReturn);
        }            
        else {
           sprintf(caMsg,"0\n");
        }                
    }
    else if(psControl->cProt == NC_REMOTE_MAILEXPORT) {
        sprintf(caMsg,"0\n");
        iReturn = ncUtlRemoteExportMail(psShmHead,psBuf);
        if(iReturn < 0) {
            sprintf(caMsg,ncLang("00402系统连接不成功，请检查配置\n"));
        }
        else {
            sprintf(caMsg,"0\n");
        }                            
    }
    return 0;
}

/* 远程控制  */
int ncSendRemoteFile(char cFlags,char *pIp,char *pHost,char *pUser,
            char *pPass,char *pShare,char *pMount,char *pSdate,char *pEdate,char *pOpt,char *pMessage)
{
	utShmHead *psShmHead;
    struct ncTestRemote_s  sBuf;
    int iFd,iReturn,iRead;
    ncIpControlInfo  s;
    s.lId = NC_IPCONTROL_ID;
    s.lSip = 0;
    s.lDip = 0;
    s.nDport = 0;
    s.cProt  = cFlags;
    s.cComm = NC_TABLES_REMOTEFILE;
    s.lStop = 0;
    memset(&sBuf,0,sizeof(sBuf));
    if(pIp)     strcpy(sBuf.caIp,pIp);
    if(pHost)   strcpy(sBuf.caHost,pHost);
    if(pUser)   strcpy(sBuf.caUser,pUser);
    if(pMount)  strcpy(sBuf.caMount,pMount);
    if(pShare)  strcpy(sBuf.caShare,pShare);
    if(pPass)   strcpy(sBuf.caPass,pPass);
    if(pSdate)  strcpy(sBuf.caSdate,pSdate);
    if(pEdate)  strcpy(sBuf.caEdate,pEdate);
    if(pOpt)    strcpy(sBuf.caOpt,pOpt);
    psShmHead = utShmCurHead();
    ncUtlRemoteFile(psShmHead,&s,&sBuf,pMessage);
    return 0;
}


/* 导入邮件   */
int ncUtlRemoteLoadMail(utShmHead *psShmHead,struct ncTestRemote_s *psBuf)
{
    unsigned long lTime1,lSize;
    unsigned long lTime2;
    char caFile1[128],caFile3[128];
    char caFile2[128],caFile[128];
    char *pMailDir;
    char caLogFile[128];
    int iCount=0,iReturn,iSum=0;
    pMailDir = utComGetVar_sd(psShmHead,"maildir","/home/ncmysql/nc/maillog");
    iReturn = pasConnect(psShmHead);
    sprintf(caLogFile,"%s/tmp/ncmailremote.log",pMailDir);
    if(iReturn != 0) {
        utLogMsg(caLogFile,ncLang("0557数据库连接失败"));
        return (-1);
    }
    if(strcasecmp(utComGetVar_sd(psShmHead,"RemoteFs","smbfs"),"cifs")==0) {
        iReturn = pasUtlMountCifFs(psBuf->caIp,psBuf->caHost,psBuf->caShare,psBuf->caUser,
                         psBuf->caPass,psBuf->caMount);
    }
    else {    
        iReturn = pasUtlMountSmbFs(psBuf->caIp,psBuf->caHost,psBuf->caShare,psBuf->caUser,
                         psBuf->caPass,psBuf->caMount);
    }
    if(iReturn != 0) {
        utLogMsg(caLogFile,ncLang("0823远程文件连接失败"));
        return (-1);
    }
    ncMailCvtDate997(psBuf->caSdate);            
    lTime1 = ncCvtLongTime(psBuf->caSdate,"00:00:00");
    ncMailCvtDate997(psBuf->caEdate);            
    lTime2 = ncCvtLongTime(psBuf->caEdate,"23:59:59");
    while(lTime1 <= lTime2) {
        if(psBuf->caOpt[0]=='1') {
             sprintf(caFile1,"mailctl%s.zip",utTimFormat("%Y-%m-%d",lTime1));
             sprintf(caFile,"%s/%s",psBuf->caMount,caFile1);
             if(utFileIsExist(caFile)) {
                sprintf(caFile2,"%s/tmp",pMailDir);
                iReturn = pasUtlUnzipFile(caFile,caFile2);
                if(iReturn == 0) {
                    sprintf(caFile3,"%s/mailctl%s",caFile2,utTimFormat("%Y-%m-%d",lTime1));
                    iCount = ncMailCtlRecoveMail(psShmHead,caFile3);
                    if(iCount >= 0) {
                        iSum = iSum + iCount;
                        utLogMsg(caLogFile,ncLang("0641文件%-64s  共包含 %d 封邮件, 导入成功"),
                                  caFile1,iCount);
                    }
                    else {
                        utLogMsg(caLogFile,ncLang("0639文件%-64s  导入出错"),
                                  caFile1);
                    }
                }
                utFilRmDir(caFile3);
            }
            else {
                utLogMsg(caLogFile,ncLang("0638文件%-64s  不存在"),
                                  caFile1);
            }                
        }
        if(psBuf->caOpt[1]=='1') {
             sprintf(caFile1,"mailmon%s.zip",utTimFormat("%Y-%m-%d",lTime1));
             sprintf(caFile,"%s/%s",psBuf->caMount,caFile1);
             
             if(utFileIsExist(caFile)) {
                sprintf(caFile2,"%s/tmp",pMailDir);
                iReturn = pasUtlUnzipFile(caFile,caFile2);
                if(iReturn == 0) {
                    sprintf(caFile3,"%s/mailmon%s",caFile2,utTimFormat("%Y-%m-%d",lTime1));
                    iCount = ncMailCtlRecoveMail1(psShmHead,caFile3);
                    if(iCount >= 0) {
                        iSum = iSum + iCount;
                        utLogMsg(caLogFile,ncLang("0641文件%-64s  共包含 %d 封邮件, 导入成功"),
                                  caFile1,iCount);
                    }
                    else {
                        utLogMsg(caLogFile,ncLang("0640文件%-64s  导入失败"),
                                  caFile1);
                    }
                }
                /* 删除文件  */
                utFilRmDir(caFile3);
            }
            else {
                utLogMsg(caLogFile,ncLang("0638文件%-64s  不存在"),
                                  caFile1);
            }                
        }
        lTime1 = lTime1 + 60 * 24 * 60;
    }
    utLogMsg(caLogFile,ncLang("0794邮件导入结束, 总共 %d 封邮件导入 !\n<EOF>"),iSum);
    pasUtlUmountSmbFs(psBuf->caMount); 
    pasDbClose(NULL);
    return 0;
}
        
/* 导出邮件   */
int ncUtlRemoteExportMail(utShmHead *psShmHead,struct ncTestRemote_s *psBuf)
{
    unsigned long lTime1,lSize;
    unsigned long lTime2;
    char caFile1[128],caFile3[128];
    char caFile2[128],caFile[128];
    char caSdate[32];
    char *pBackDir;
    char *pMailDir;
    char caLogFile[128];
    int iCount=0,iReturn,iSum=0;
    pMailDir = utComGetVar_sd(psShmHead,"maildir","/home/ncmysql/nc/maillog");
    iReturn = pasConnect(psShmHead);
printf(" Connect Database %d \n",iReturn);
    sprintf(caLogFile,"%s/tmp/ncmailexport.log",pMailDir);    
    if(iReturn != 0) {
        utLogMsg(caLogFile,ncLang("0557数据库连接失败"));
        return (-1);
    }
    if(strcasecmp(utComGetVar_sd(psShmHead,"RemoteFs","smbfs"),"cifs")==0) {
        iReturn = pasUtlMountCifFs(psBuf->caIp,psBuf->caHost,psBuf->caShare,psBuf->caUser,
                         psBuf->caPass,psBuf->caMount);
    }
    else {
        iReturn = pasUtlMountSmbFs(psBuf->caIp,psBuf->caHost,psBuf->caShare,psBuf->caUser,
                         psBuf->caPass,psBuf->caMount);
    }
            
printf(" Mount  %d \n",iReturn);
    if(iReturn != 0) {
        utLogMsg(caLogFile,ncLang("0823远程文件连接失败"));
        return (-1);
    }
    ncMailCvtDate997(psBuf->caSdate);            
    lTime1 = ncCvtLongTime(psBuf->caSdate,"00:00:00");
    ncMailCvtDate997(psBuf->caEdate);            
    lTime2 = ncCvtLongTime(psBuf->caEdate,"23:59:59");
printf(" lTime1==%lu lTime2==%lu caOpt--[%s]\n",
          lTime1,lTime2,psBuf->caOpt);
    while(lTime1 <= lTime2) {
        if(psBuf->caOpt[0]=='1') {
             sprintf(caSdate,utTimFormat("%Y-%m-%d",lTime1));
             sprintf(caFile1,"mailctl%s.zip",caSdate);
             pBackDir = utComGetVar_sd(psShmHead,"BackupPath","/home/ncmysql/nc/back");
             sprintf(caFile,"%s/%s",pBackDir,caFile1);
printf(" CaFile==[%s\n",caFile);
             if(utFileIsExist(caFile)) { /* 直接拷贝  */
                 utLogMsg(caLogFile,ncLang("0857正在复制 %s"),caFile1);
                 sprintf(caFile2,"%s/%s",psBuf->caMount,caFile1);
                 utFilFilCopy(caFile,caFile2);
             }
             else {
                ncMailCtlBackup(psShmHead,caSdate);
                if(!utFileIsExist(caFile)) { 
                    utLogMsg(caLogFile,ncLang("0856 正在导出  %s  邮件不存在"),caFile1);
                }
                else {
                    utLogMsg(caLogFile,ncLang("0855 正在导出  %s  OK!"),caFile1);
                    utFilFilCopy(caFile,caFile2);
                }
             }
        }
        if(psBuf->caOpt[1]=='1') {
             sprintf(caSdate,utTimFormat("%Y-%m-%d",lTime1));
             sprintf(caFile1,"mailmon%s.zip",caSdate);
             pBackDir = utComGetVar_sd(psShmHead,"BackupPath","/home/ncmysql/nc/back");
             sprintf(caFile,"%s/%s",pBackDir,caFile1);
             if(utFileIsExist(caFile)) { /* 直接拷贝  */
                 utLogMsg(caLogFile,ncLang("0857正在复制 %s"),caFile1);
                 sprintf(caFile2,"%s/%s",psBuf->caMount,caFile1);
                 utFilFilCopy(caFile,caFile2);
             }
             else {
                ncMailMonBackup(psShmHead,caSdate);
                if(!utFileIsExist(caFile)) { 
                    utLogMsg(caLogFile,ncLang("0856 正在导出  %s  邮件不存在"),caFile1);
                }
                else {
                    utFilFilCopy(caFile,caFile2);
                    utLogMsg(caLogFile,ncLang("0855 正在导出  %s  OK!"),caFile1);
                }
             }
        }
        lTime1 = lTime1 + 60 * 24 * 60;
    }
    utLogMsg(caLogFile,ncLang("0793邮件导出结束 !\n<EOF>"));
    pasUtlUmountSmbFs(psBuf->caMount); 
    pasDbClose(NULL);
    return 0;
}


/*域中修改密码*/
int ncWeDomainChangePasswd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    
    unsigned long lIp;

    utMsgHead *psMsgHead0;
    char temp_compname[64];
    char temp_pid[16];
    int iNum,k;
    char caOldpassword[64],caPassword[64],caUsername[32];
    char *pIp,*pPort;
    int iSock1,iReturn;
    utComSockAddr sSockAddr;
    char sBuf[36];
    long addrlen;
    utPltDbHead *psDbHead;
    pIp=utComGetVar_s(psShmHead,"NtDoMainServer");
    memset(sBuf,0,sizeof(sBuf));
    if(pIp == NULL) {
       utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0778用户验证"),ncLang("0817域名服务器没有定义"));
       return 0;
    }
    lIp=utComHostAddress(pIp);
    pPort=utComGetVar_sd(psShmHead,"NtDoMainPort","1001");
  
    
    
    
   	iReturn = utMsgGetSomeNVar(psMsgHead,3,"username",UT_TYPE_STRING, 31, caUsername,
   	                                       "oldpassword",UT_TYPE_STRING,63,caOldpassword,
   	                                       "password",UT_TYPE_STRING,63,caPassword);
  
   psMsgHead0 = utMsgInit(NW_MSG_CODE_REQ,NC_CLIENT_PASSCHG,utStrGetId(),4096);
      utMsgAdd_s(&psMsgHead0,"username",caUsername);
      utMsgAdd_s(&psMsgHead0,"oldpasswd",caOldpassword);
      utMsgAdd_s(&psMsgHead0,"passwd",caPassword);
   
    iSock1 = utComUdpSock();

    utComSetAddr0(&sSockAddr,lIp,htons(atol(pPort)));
    iReturn = utComUdpMsgSendTo(iSock1,&sSockAddr,psMsgHead0);
    recvfrom(iSock1,sBuf,30,0,&sSockAddr,&addrlen); 
 printf("sBuf=%s\n",sBuf);   
    close(iSock1);   
      
                 
   utMsgFree(psMsgHead0);
    
    

    psDbHead = utPltInitDb();
   
   if(strcmp(sBuf,"86")==0)
    utPltPutVar(psDbHead,"message","旧密码不正确");
   else if(strcmp(sBuf,"2351")==0)
    utPltPutVar(psDbHead,"message","非法电脑或域名");
   else if(strcmp(sBuf,"0")==0)
    utPltPutVar(psDbHead,"message","密码修改成功");
   else if(strcmp(sBuf,"2245")==0)
    utPltPutVar(psDbHead,"message","密码有效性不符密码策略");  
   else if(strcmp(sBuf,"2221")==0)
    utPltPutVar(psDbHead,"message","没法找到用户"); 
   else if(strcmp(sBuf,"5")==0)
    utPltPutVar(psDbHead,"message","权限不允许"); 
   else
      utPltPutVar(psDbHead,"message","网络或其它原因错误"); 
      
      
   utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_login_modpass_nt.htm");
  return 0;
}



/*同步域用户*/
int ncWebSynDomain(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
 
   
  return 0;
}


/*接收域用户信息*/
int ncSysDepUserByDomain(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caTemp[1056];
    char caName[32],caDispname[32],caTime[32],caIp[16],caGroup[512],caOu[511];
    char *p,*p1;
    int lCount,preid,lGroupid,i,lCount2;
    char caSql[1024];
    int iReturn;
    long lSum,ii;
    char username_temp[32],dispname_temp[32],ou_temp[32],group_temp[32];
    char caObj[100][50];
//utMsgPrintMsg(psMsgHead);




lSum=0;
      utMsgGetSomeNVar(psMsgHead,1,"sum",UT_TYPE_LONG,4,&lSum);
 //     printf("lSum=%d\n",lSum);
      for(ii=0;ii<lSum;ii++){
      	sprintf(username_temp,"username%d",ii);
      	sprintf(dispname_temp,"dispname%d",ii);
      	sprintf(ou_temp,"ou%d",ii);
      	sprintf(group_temp,"group%d",ii);
      	utMsgGetSomeNVar(psMsgHead,4,
														      	username_temp,UT_TYPE_STRING,31,caName,
														      	dispname_temp,UT_TYPE_STRING,31,caDispname,
														      	ou_temp,UT_TYPE_STRING,511,caOu,
														      	group_temp,UT_TYPE_STRING,511,caGroup);



//printf("caOu=%s\n",caOu);

       lGroupid=0;
      if(strcasecmp(utComGetVar_sd(psShmHead,"AddGroupByNt","Yes"),"YES")==0){
      	if((strcasecmp(utComGetVar_sd(psShmHead,"NtOu","No"),"Group")==0)&&strlen(caOu)>0){
      		lCount=nc_SepChar(caOu,',',caObj);
      		if((strcasecmp(utComGetVar_sd(psShmHead,"SkipFirt","No"),"Yes")==0)&&lCount>1){
      			lCount=lCount-1;
      		}
      		lGroupid=0;
      	
      		for(i=0;i<lCount;i++){
      			lGroupid=0;
    				utStrDelSpaces(caObj[lCount-i-1]);
    				if(strlen(caObj[lCount-i-1])>0){
      				sprintf(caSql,"select groupid from ncgroup where groupname='%s' and level=%d ",caObj[lCount-i-1],i);
      				pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lGroupid);
      				if(i==0)
      			  	preid=0;
      			
      			  
      				if(lGroupid<=0){
      				 	lGroupid =  pasGetSid("ncgroup","groupid"); 
      				 	pasDbExecSqlF("insert into ncgroup (groupname,groupid,addtime,pid,level) values ('%s',%ld,%lu,%lu,%lu)",caObj[lCount-i-1],lGroupid,time(0),preid,i);
      				}
      				else{
      				 	pasDbExecSqlF("update ncgroup set groupname='%s' where groupid=%d ",caObj[lCount-i-1],lGroupid);
      				}
      				preid=lGroupid;
      			}
      		}
      	}     	
      	else if((strcasecmp(utComGetVar_sd(psShmHead,"NtGroup","No"),"Group")==0)&&strlen(caGroup)>0){
      		lCount=nc_SepChar(caGroup,',',caObj);
      		if((strcasecmp(utComGetVar_sd(psShmHead,"SkipFirt","No"),"Yes")==0)&&lCount>1){
      			lCount=lCount-1;
      		}
      		lGroupid=0;
      	
      		for(i=0;i<lCount;i++){
      			lGroupid=0;
    				utStrDelSpaces(caObj[lCount-i-1]);
    				if(strlen(caObj[lCount-i-1])>0){
      				sprintf(caSql,"select groupid from ncgroup where groupname='%s' and level=%d ",caObj[lCount-i-1],i);
      				pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lGroupid);
      				if(i==0)
      			  	preid=0;
      			
      			  
      				if(lGroupid<=0){
      				 	lGroupid =  pasGetSid("ncgroup","groupid"); 
      				 	pasDbExecSqlF("insert into ncgroup (groupname,groupid,addtime,pid,level) values ('%s',%ld,%lu,%lu,%lu)",caObj[lCount-i-1],lGroupid,time(0),preid,i);
      				}
      				else{
      				 	pasDbExecSqlF("update ncgroup set groupname='%s' where groupid=%d ",caObj[lCount-i-1],lGroupid);
      				}
      				preid=lGroupid;
      			}
      		}
      	}
      	
      }

     		if(strlen(caDispname)==0) strcpy(caDispname,caName);
      		if(strlen(caName)>0){
      			
      			
      			lCount2=0;
      			sprintf(caTemp,"select count(*) from ncuser where username='%s' ",caName);
      			pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount2);
//printf("caTemp=%s\n",caTemp);      			
      			if(lCount2>0){
			      		if(strcasecmp(utComGetVar_sd(psShmHead,"NtDispName","No"),"Dispname")==0){
			      			if((strcasecmp(utComGetVar_sd(psShmHead,"NtOu","No"),"Group")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"NtGroup","No"),"Group")==0)){
			      			   pasDbExecSqlF("update ncuser set dispname='%s',groupid=%d,upid=10 where username='%s' ",caDispname,lGroupid,caName);
			      			  }
			      			  else{
			      			  	pasDbExecSqlF("update ncuser set dispname='%s' where username='%s',upid=10 ",caDispname,caName);
			      			  }
			      			  }
			      		else{
			      			  if((strcasecmp(utComGetVar_sd(psShmHead,"NtOu","No"),"Group")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"NtGroup","No"),"Group")==0)){
			      		     pasDbExecSqlF("update ncuser set groupid=%d where username='%s',upid=10 ",lGroupid,caName);
			      		    }
			      		    }
			      	}
			      else{	
      		  iReturn=pasDbExecSqlF("insert into ncuser (userid,username,password,dispname,groupid,addtime,lasttime,upid) values(%d,'%s','0','%s',%d,%lu,%lu,10)",pasGetSid("ncuser","userid"),caName,caDispname,lGroupid,time(0),time(0));
      		  if(iReturn!=0)
      		    printf("insert into ncuser (userid,username,password,dispname,groupid,addtime) values(%d,'%s','0','%s',%d,%d)\n",pasGetSid("ncuser","userid"),caName,caDispname,lGroupid,time(0));
            }
      		}

  }

pasDbCommit(NULL);

  return 0;
}


/*域用户同步结束 如果要删除不在域里的用户，pqconf_nc.cnf中DelUserNotInDomain="YES"*/
int ncSysDomainOver(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

//utMsgPrintMsg(psMsgHead);

if(strcasecmp(utComGetVar_sd(psShmHead,"DelUserNotInDm","No"),"YES")==0){
	pasDbExecSql("delete from ncuser where upid!=10 ",0);
	pasDbExecSql("update ncuser set upid=0 ",0);
}
	 ncUtlReReadConFig(psShmHead);
	
	        if(ncUtlIsLock(psShmHead,NC_LOCK_SYSREFRESH,5)==0) {
	        ncInitProeimA(psShmHead,1);
	        ncUtlUnLock(psShmHead,NC_LOCK_SYSREFRESH);
	        }
 
  return 0;
}

//显示手机用户的上网流量
int ncWebShowMoveuser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caTemp[2048];
    char caCond[1024],caSorta[12],caOrdera[12];
    char caGroupid[16],caSip[20];
    char caUserid[16],caUsername[32],caMac[24],caSdate[24],caEdate[24];
    char caStime[10];
    char caTotPg[16],caCurPg[16];
    char caUdip[64],caDep[64];
    char caIdfyno[20];
    unsigned long lUserid,lCtime;
    unsigned long lUbytes,lDbytes;
    unsigned long lStime;
    unsigned long lEtime;
    unsigned long lIp;
    unsigned long lCurPg,lTotPg;
    char caUbyte[16],caDbyte[16];
    char caTime[24];
    int lCount;
    int i,iNum;
    char caIp[20],caContent[256],caAttfile[128];
    char caPort[12];
   	char ids[256];
   	int i2,lUserid2;
    char caDel[16],caExport[16];
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lRowNum,lStartRec,lTotRec;
   char caUsername_match[8],caIdfyno_match[8];
 	  pasDbCursor *psCur;
 	  ncUserCont *psUser;
    long *pNum;
 	  char caTitlename[32];
 	  char caPosition[32];
 	  char caPassword[32],caModpass[20];
    char caMesg[32];
    char caCip[20];
    ncUserInfo *psUserInfo;	

    memset(caMesg,0,sizeof(caMesg));
 	   
//	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,9,
	    "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			"CurPg",      UT_TYPE_STRING, 12, caCurPg,			
			"username",UT_TYPE_STRING,63,caUsername,
			"password1",UT_TYPE_STRING,30,caPassword,
			"modpass", UT_TYPE_STRING,15,caModpass,	
   	   "exp",     UT_TYPE_STRING,10,caExport,
   	   "ip",      UT_TYPE_STRING,15,caIp,
   	   "port",    UT_TYPE_STRING,10,caPort,
   	  "clientip",    UT_TYPE_STRING, 20, caCip);
   
    utStrDelSpaces(caUsername);
    psUserInfo = ncGetUserInfoByName(psShmHead,caUsername);
    if(psUserInfo == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),ncLang("0422您还没有登陆"));
        return 0;
    }
    lIp = ntohl(pasIpcvtLong(caCip));
    if(psUserInfo->lIp != lIp) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0588退出"),"您不能查看别人的日志!");
        return 0;
    }
   
   
   if(strlen(caModpass)>0&&strlen(caUsername)>0&&strlen(caPassword)>0){
   	char caKey[32],caPassword1[64];
   	 sprintf(caPassword1,"%s%s",caUsername,caPassword);
    utMd5Ascii22(caPassword1,strlen(caPassword1),NULL,caKey);
    iReturn = pasDbExecSqlF("update ncuser set password='%s',lasttime=%lu where username='%s'",
                 caKey,time(0),caUsername);
     pasDbExecSqlF("update ncmobuser set password='%s' where username='%s'",
                 caPassword,caUsername);     
     if(iReturn==0){
     strcpy(caMesg,"修改密码成功");
    }
    else{
    	strcpy(caMesg,"密码修改失败");
    }  
                 
   	
  }
   
   
   
   	  
       sprintf(caTemp,"select userid from ncuser where username='%s' ",caUsername);
       lUserid=0;
       pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lUserid);
    	     
  
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;

 

    if(!utStrIsSpaces(caExport)){
    	lCurPg = 1;
    lRowNum = 99999;
    }
    else
    lRowNum=500;
    
    lStartRec = (lCurPg - 1) * lRowNum;    



 
  	sprintf(caTemp,"select count(*) from ncuserlog  where userid=%d ",lUserid);
  
    
    iReturn = pasDbOneRecord(caTemp,0,
                            UT_TYPE_LONG,4,&lCount);

    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","部门查询","查询数据出错 ");
        return 0;
    }  
    
    	sprintf(caTemp," select a.stime,a.etime,a.ip,a.mac,a.sumflow+a.upflow from ncuserlog a where a.userid=%d ",lUserid);  
    

strcat(caTemp," order by a.stime desc ");


    sprintf(caTemp+strlen(caTemp)," limit %lu,%lu ",lStartRec,lRowNum);



    psCur = pasDbOpenSql(caTemp,0);

    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","部门查询","查询数据出错 ");
        return 0;
    }   

    if(!utStrIsSpaces(caExport)){
    	FILE *fp;
    	char caFilename[32];
    	char caFile[128];
    	char caPath[128];
        sprintf(caFilename,"ncnetuser%d.csv",utStrGetId());
        sprintf(caPath,"%s/html","/home/ncmysql/nc");
        sprintf(caFile,"%s/%s",caPath,caFilename);
    	fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
            return 0;
        }
        
        fprintf(fp,ncLang("1101用户上网日志"));
      ncUtlPrintCsv(fp,7,
                       UT_TYPE_STRING,ncLang("0728用户名"),
                       UT_TYPE_STRING,ncLang("0054IP地址"),
                       UT_TYPE_STRING,ncLang("0072MAC地址"),
                       UT_TYPE_STRING,ncLang("1100上网时间"),
                       UT_TYPE_STRING,ncLang("1102离线时间"),
                       UT_TYPE_STRING,"上网时间",
                       UT_TYPE_STRING,ncLang("1103总流量KBytes"));

                                      
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;
       lStime=0;
       lEtime=0;
       lIp=0;
       lUbytes=0;
        
   	iReturn = pasDbFetchInto(psCur,
   																 UT_TYPE_ULONG,4,&lStime,
   																 UT_TYPE_ULONG,4,&lEtime,
   																 UT_TYPE_ULONG,4,&lIp,
   																 UT_TYPE_STRING,19,caMac,
   																 UT_TYPE_ULONG, 4, &lUbytes);
   																
   				       
  	                              
        while((iReturn == 0)||(iReturn==1405)) {
        
        psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
		     if(psUser){   
          strcpy(caUdip,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
         
        }
        else{
        	strcpy(caUdip,"");
        	strcpy(caDep,"");
       
        }
        strcpy(caTime,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
        strcpy(caUbyte,utStrLtoF(lUbytes,caTemp));
      
        ncUtlPrintCsv(fp,7,
                       UT_TYPE_STRING,caUdip,
                       UT_TYPE_STRING,utComHostIp(htonl(lIp)),
                       UT_TYPE_STRING,caMac,
                       UT_TYPE_STRING,caTime,
                       UT_TYPE_STRING,utTimFormat("%Y/%m/%d %H:%M:%S",lEtime),
                       UT_TYPE_STRING,ncCvtConntime(lEtime-lStime),
                       UT_TYPE_STRING,caUbyte);
                                          
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;
       lStime=0;
       lEtime=0;
       lIp=0;
       lUbytes=0;
        
   	iReturn = pasDbFetchInto(psCur,
   																 UT_TYPE_ULONG,4,&lStime,
   																 UT_TYPE_ULONG,4,&lEtime,
   																 UT_TYPE_ULONG,4,&lIp,
   																 UT_TYPE_STRING,19,caMac,
   																 UT_TYPE_ULONG, 4, &lUbytes);
        }
        
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        
         if(strcasecmp(utComGetVar_sd(psShmHead,"HotelVesion","No"),"Yes")==0){ 
 	        free(pNum);
          } 
        return 0;
    }
         
    psDbHead = utPltInitDb();
    
    utPltPutVar(psDbHead,"message",caMesg);
    utPltPutVar(psDbHead,"username",caUsername);
    utPltPutVar(psDbHead,"ip",caIp);
    utPltPutVar(psDbHead,"port",caPort);
    
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;
       lStime=0;
       lEtime=0;
       lIp=0;
       lUbytes=0;
        
     	iReturn = pasDbFetchInto(psCur,
   																 UT_TYPE_ULONG,4,&lStime,
   																 UT_TYPE_ULONG,4,&lEtime,
   																 UT_TYPE_ULONG,4,&lIp,
   																 UT_TYPE_STRING,19,caMac,
   																 UT_TYPE_ULONG, 4, &lUbytes);
   	iNum = 0;
   	
   	while(iReturn == 0||iReturn==1405 && iNum < lRowNum) {
        iNum++;
      
        utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum);
       

        sprintf(caTime,"%s %s",caSdate,caStime);
        strcpy(caUbyte,utStrLtoF(lUbytes,caTemp));
        
          
        utPltPutLoopVar(psDbHead,"ip",    iNum,utComHostIp(htonl(lIp)));
        utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
        utPltPutLoopVar(psDbHead,"edate", iNum,utTimFormat("%Y/%m/%d %H:%M:%S",lEtime));
        utPltPutLoopVar(psDbHead,"ubytes",iNum,caUbyte);
        utPltPutLoopVar(psDbHead,"mac",iNum,caMac);
        utPltPutLoopVar(psDbHead,"time",iNum,ncCvtConntime(lEtime-lStime));
        memset(caMac,0,sizeof(caMac));      
        memset(caDep,0,sizeof(caDep));   
        lUserid=0;
        lStime=0;
        lEtime=0;
        lIp=0;
        lUbytes=0;
        iReturn = pasDbFetchInto(psCur,
   																 UT_TYPE_ULONG,4,&lStime,
   																 UT_TYPE_ULONG,4,&lEtime,
   																 UT_TYPE_ULONG,4,&lIp,
   																 UT_TYPE_STRING,19,caMac,
   																 UT_TYPE_ULONG, 4, &lUbytes);
    }
    pasDbCloseCursor(psCur);
    
    
    
    lTotPg = (lCount - 1) / lRowNum + 1;
    lTotRec = lCount;
    i = 0;
//    while(iNum < lRowNum) {
//        i++;
//        iNum++;
//        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
 //   }
  
   
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"yp/UserInfo.html");
    return 0;
}


/* Squid验证  */
int ncSquidAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caUsername[32];
   	char caIp[16];
   	unsigned long lIp;
   	int iReturn;
    pasLogs(PAS_SRCFILE,1002,"[ncSquidAuth]Accept Request From %s",utComGetSockIpS(iFd,NULL));
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
					"username",  UT_TYPE_STRING, 31, caUsername,
					"srcip",     UT_TYPE_STRING, 15, caIp);
    if(iReturn == 2) {
        /* 更新用户信息和IP地址  */
        lIp = ntohl(pasIpcvtLong(caIp));
        iReturn = ncNtUserLogin0(psShmHead,caUsername,lIp,NULL);
//        ncUtlAddCompInfo(psShmHead,lIp,NULL,caUsername,MACIP_FROM_PROXY,1);
        pasLogs(PAS_SRCFILE,1002,"[ncSquidAuth]Accept UserName=%s  Ip=%s",caUsername,caIp);
    }
    pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* 密钥，暂时不用  */
                    1,
                    "status",    UT_TYPE_LONG, iReturn);
    return 0;                    
}



/* Squid验证  */
int ncSquidNcAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncUserInfo *psUserInfo;
   	char caUsername[32];
   	char caPassword[32];
   	char caIp[16];
   	char caMsg[256];
   	unsigned long lIp;
   	int iReturn;
    pasLogs(PAS_SRCFILE,1002,"[ncSquidNcAuth]Accept Request From %s",utComGetSockIpS(iFd,NULL));
   	iReturn = utMsgGetSomeNVar(psMsgHead,3,
					"username",  UT_TYPE_STRING, 31, caUsername,
					"password",  UT_TYPE_STRING, 31, caPassword,
					"srcip",     UT_TYPE_STRING, 15, caIp);
    if(iReturn == 3) {
        /* 更新用户信息和IP地址  */
        lIp = ntohl(pasIpcvtLong(caIp));
        psUserInfo = (ncUserInfo *)ncUtlUserAuth(psShmHead,caUsername,caPassword,lIp,caMsg);
        if(psUserInfo) {
            pasLogs(PAS_SRCFILE,1002,"[ncSquidAuth]Accept UserName=%s  Ip=%s",caUsername,caIp);
            pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* 密钥，暂时不用  */
                    1,
                    "status",UT_TYPE_LONG, 1);
            return 0;
        }
        else {
            pasLogs(PAS_SRCFILE,1002,"[ncSquidAuth]Accept UserName=%s  Ip=%s iReturn=%d Msg=%s ",caUsername,caIp,iReturn,caMsg);
        }
    }
    else {
         pasLogs(PAS_SRCFILE,1002,"[ncSquidAuth]Accept UserName=%s  Ip=%s iReturn=%d",caUsername,caIp,iReturn);
    }
    
    pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* 密钥，暂时不用  */
                    1,
                    "status",    UT_TYPE_LONG, 0);
    return 0;                    
}



/* 本地验证路由选择

   #起始IP地址   终止IP地址        验证地址或端口        界面目录(plate/xx/pppppp)
   192.168.0.1  192.168.1.254     192.168.1.28:8080    nc
 */
 
 int ncUtlLoadAuthRoute(char *pFile)
 {
    FILE *fp;
    ncAuthRoute *ps;
    int iSum=0,iMax=32;
    char *p;
    char caBuf[256],caSip[32],caDip[32],caPlate[64],caAuth[32];
    int n;
    fp = fopen(pFile,"r");
    if(fp == NULL) {
        pasLog(PAS_LOG_ERROR,"ncUtlLoadAuthRoute %s Load error",pFile);
        return 0;
    }
    ps = (ncAuthRoute *)malloc(iMax*sizeof(ncAuthRoute));
    if(ps == NULL) {
        pasLog(PAS_LOG_ERROR,"ncUtlLoadAuthRoute Memory alloc error");
    }
    p = fgets(caBuf,255,fp);
    while(p) {
        p = utStrSkipSpaces(p);
        if(*p != 0 && *p != '#') {
            caSip[0] = 0;
            caDip[0] = 0;
            caAuth[0]= 0;
            caPlate[0] = 0;
            n = utStrGetSomeWordBySp(p, 4,UT_TYPE_STRING,31,caSip,
                                          UT_TYPE_STRING,31,caDip,
                                          UT_TYPE_STRING,31,caAuth,
                                          UT_TYPE_STRING,63,caPlate);
            if(n > 3 && utStrIsValidIp(caSip) && utStrIsValidIp(caDip)) {
                if(iSum >= iMax) {
                    ps = realloc(ps,(iMax + 32)*sizeof(ncAuthRoute));
                    if(ps == NULL) {
                        pasLog(PAS_LOG_ERROR,"ncUtlLoadAuthRoute Memory alloc error");
                    }
                    iMax += 32;
                }
                ps[iSum].lSip = ntohl(pasIpcvtLong(caSip));
                ps[iSum].lEip = ntohl(pasIpcvtLong(caDip));
                utStr2IpPort(caAuth,&ps[iSum].lAuthIp,&ps[iSum].nPort);
                ps[iSum].lAuthIp = ntohl(ps[iSum].lAuthIp);
                ps[iSum].nPort = ntohs(ps[iSum].nPort);
                if(utStrIsSpaces(caPlate)) {
                    strcpy(caPlate,"nc");
                }
                strcpy(ps[iSum].caPlate,caPlate);
                
                pasLogs(PAS_SRCFILE,1001,"ncUtlLoadAuthRoute [%s] [%s] [%s] [%s]",
                        caSip,caDip,caAuth,caPlate);
                iSum++;
            }
            
        }
        p = fgets(caBuf,255,fp);
    }
    fclose(fp);
    if(iSum < iMax) {
        psAuthRoute = realloc(ps,iSum*sizeof(ncAuthRoute));
    }
    else {
        psAuthRoute = ps;
    }
    iSumAuthRoute = iSum;
    
    return 0;
}


int ncUtlGetAuthIp(utShmHead *psShmHead,
                      unsigned long lIp,
                      char *pIp,char *pPort,char *pPlate)
{
    int i;
    unsigned short nPort;

    for(i=0;i<iSumAuthRoute;i++) {
        if((lIp & psAuthRoute[i].lEip) == psAuthRoute[i].lSip) {
            strcpy(pIp,utComHostIp(htonl(psAuthRoute[i].lAuthIp)));
     //       printf("pIp=%s\n",pIp);
            sprintf(pPort,"%lu",psAuthRoute[i].nPort);
            strcpy(pPlate,psAuthRoute[i].caPlate);
            return 1;
        }
    }
    strcpy(pIp,utComGetVar_sd(psShmHead,"AdminHost","192.168.0.1"));
    strcpy(pPort,utComGetVar_sd(psShmHead,"AuthHttpPort","8080"));
    strcpy(pPlate,"nc");
    return 0;
}
     
    

//手机验证方式的
    /* 本地验证登录  */               
int ncWebLoginMob(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caUsername[32],caIp[20],mac[12];
   	char caDispname[32],caDip[20],caPort[16],caPlatePath[32],caPlate[128];
   	char caGroupname[32],caKey[32];
   	char caPassword[32],caPassword0[32];
   	char caIBMMesg[128];
   	char caMod[32];
   	char caUrl[2560];
   	char caMsg[128];
   	char caIp0[16];
   	char caMac0[32];
   	char caHost0[64];
   	char caMessage[128];
   	int  iStatus;
   	char *pMac;
   	char caMac[20];
   	char Ou[256],Group[256];
   	long lCount;
   	long8 lThisFlow,lFlowLimit;
   	char caUsragent[256];   					
   	unsigned long lTimeLimit,lThisTime;
   	long lLimitid,lPostflags,lWebipflags,level;
   	utPltDbHead *psDbHead;
   	char caOk[8];
   	char caSid[16];
   	unsigned long lUserid,lGroupid,lUseflags,lIp,lRate,lIp0;
   	int iReturn,lLocalflag;
   	ncUserInfo *psUserInfo=NULL;
   	ncUserCont *psUser;
    char *p;
    pasDbCursor *psCur;
    char news_title[256],sqlbuf[1024];
    int iNum,lSid,lStime;
    lGroupid=0;
    long lNettime=0;
    char caEng_flag[16];
    char caPath_all[256];
    long lFlag,lLogin;
    char caTemp2[1024],caTemp[1024];
     char caOutUrl[2560];
    ncHoteluser stHoteluser;
    
    ncLimGroup *psGroup=NULL;
    memset(&stHoteluser,0,sizeof(ncHoteluser));
    memset(Group,0,sizeof(Group));
    memset(Ou,0,sizeof(Ou));
    memset(caDispname,0,sizeof(caDispname));
    memset(caGroupname,0,sizeof(caGroupname));
    strcpy(caMod,"\0");
    strcpy(caMessage,"\0");

 //           utMsgPrintMsg(psMsgHead);
    
   	iReturn = utMsgGetSomeNVar(psMsgHead,10,
				"username",    UT_TYPE_STRING, 31, caUsername,
				"password",    UT_TYPE_STRING, 31, caPassword,
				"ModiPass",    UT_TYPE_STRING, 31, caMod,
				"clientip",    UT_TYPE_STRING, 19, caIp,
				"ok",          UT_TYPE_STRING, 6,  caOk,
				"url",         UT_TYPE_STRING,2048,caUrl,
				"sid",         UT_TYPE_STRING, 12, caSid,
				"mesg",        UT_TYPE_STRING,60,caMsg,
				"eng_flag",    UT_TYPE_STRING,6,caEng_flag,
				"usragent",    UT_TYPE_STRING,255,caUsragent);
				
							  
			  memset(caOutUrl,0,sizeof(caOutUrl));
			  if(p=strstr(caUrl,"URL=http:")){                           //用作夏阳湖接口重定向
			  	strcpy(caOutUrl,p+4);
	//		  	printf("caOutUrl=%s\n",caOutUrl);
			  }
				
//		printf("fffffffffffffffffffffff\n");
	//判断是否手机还是PC
		lFlag=0;
    lLogin=0;
    lLogin=ncIsMobile(caUsragent);
    memset(&stHoteluser,0,sizeof(stHoteluser));
    lIp=ntohl(pasIpcvtLong(caIp));	

    ncUtlGetAuthIp(psShmHead,lIp,caDip,caPort,caPlatePath);
     if((strcasecmp(utComGetVar_sd(psShmHead,"StartSysJGUser","No"),"Yes")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"HotelAuth","No"),"Yes")==0)){
     	 sprintf(caMsg,"%s%s/nc_login_main_hotelmob.htm",caPlatePath,ncGetAuthPath(psShmHead,lLogin,atol(caEng_flag)));
    }
     else{    
          sprintf(caMsg,"%s%s/nc_login_main.htm",caPlatePath,ncGetAuthPath(psShmHead,lLogin,atol(caEng_flag)));
        }

          sprintf(caPath_all,"/home/ncmysql/nc/plate/%s",caMsg);
       
  printf("caPath_all=%s\n",caPath_all);
          if(!utFileIsExist(caPath_all)){
          	strcpy(caMsg,"nc/nc_login_main_sb.htm");
          }
         
    
    
    
    utStrDelSpaces(caUsername);
    utStrDelSpaces(caPassword); 
    utStrDelSpaces(caIp);
    /* 防止用户名或密码为空  */
    strcpy(caDispname,caUsername);
    if(utStrIsSpaces(caUsername)) {
        strcpy(caUsername,"请输入用户名");
    }
    if(utStrIsSpaces(caPassword)) {
        strcpy(caPassword,"****************");
    }
    /* 验证  */
    lIp = ntohl(pasIpcvtLong(caIp));	
    if(utStrIsSpaces(caSid)) {
        lSid = lIp;
    }
    else {
        sscanf(caSid,"%lu",&lSid);
    }
    
 //   ncUtlGetAuthIp(psShmHead,lIp,caDip,caPort,caPlatePath);
     

          //手机短信验证   iReturn=0 验证成功 <0 验证失败
          if(strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"GSMModen")==0){
          	iReturn = ncAuthInLockMobile(psShmHead,caUsername,caPassword);
          }
          else{
           iReturn = ncSysAuthMobile(psShmHead,caUsername,caPassword);
          }
    
          if(iReturn!=0){
          	//再给一次机会，也许远程服务有问题，在本地验证一下
          	iReturn = ncAuthMobileInLocal(caUsername,caPassword);
          }
          //对于外迈迪注册用户，还需要在外迈迪用户库中验证一下
          if(strcasecmp(utComGetVar_sd(psShmHead,"AuthByWmd","No"),"Yes")==0){
          	if(iReturn<0){
          	iReturn=ncAuthWmdCenter(psShmHead,caUsername,caPassword,&stHoteluser);
          	   if(iReturn==0){
          	   	strcpy(caUsername,stHoteluser.caUsername);
          	   	strcpy(caDispname,stHoteluser.caDispname);
            }
          }
        }
          
          
          
          
          if(iReturn<0){
          	    psDbHead = utPltInitDb();
                utPltPutVar(psDbHead,"ip",caDip);
                utPltPutVar(psDbHead,"port",caPort);
                utPltPutVar(psDbHead,"url",caUrl);
                
                if(atol(caEng_flag)==1){
                	utPltPutVar(psDbHead,"message","Invalid input,please try again.");
                }
                else{
                	utPltPutVar(psDbHead,"message","验证失败,请重新验证!");
                	strcpy(caTemp,"验证失败,请重新验证!");
                  pasCharsetCvt(PAS_CCODE_GB,PAS_CCODE_UTF8,caTemp,caTemp2,63);  
     	
                	utPltPutVar(psDbHead,"message_u",caTemp2);                	

                }
                utPltPutVarF(psDbHead,"sid","%lu",lSid);
    
  //              utPltShowDb(psDbHead);
 printf("caMsg=%s\n",caMsg);
                utPltOutToHtml(iFd,psMsgHead,psDbHead,caMsg);
                return 0;
          }
          


           	
             psUser = ncUtlGetUserContByName(psShmHead,caUsername);
             if(psUser == NULL) { /* 用户不存在  */
      
                if(strcasecmp(utComGetVar_sd(psShmHead,"AddUserNoExist","Yes"),"Yes") == 0) {
     

                    ncUtlAddNewUser(caUsername,caDispname,NULL,NULL,NULL,NULL,0,lIp,NC_NETWORK_AUTH);
 
                    psUser = ncUtlGetUserContByName(psShmHead,caUsername);
                    if(psUser == NULL) {
 
                        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0778用户验证"),ncLang("0780用户增加出错"));
                        return 0;
                    }
    
                    
                }
                else {

                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0778用户验证"),ncLang("0742用户不存在"));
                    return 0;
                }
                
            }

          
      
            psUserInfo = ncUtlDoLogin(psShmHead,psUser,NC_LOGIN_AUTH,lIp,NULL,caMessage);
            if(psUserInfo == NULL) {
  
            	
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0778用户验证"),caMessage);
                return 0;
            }
            else {
                char caTemp[32];
                char caMessage1[128];
  
                  
                if(strcasecmp(utComGetVar_sd(psShmHead,"AuthSendHtml","No"),"No")==0) {
                    ncCtlIptDelRedict(lIp);
                }
                psDbHead = utPltInitDb();
        
   //对于迈外迪用户，同步管理中心的用户信息     
    if(strcasecmp(utComGetVar_sd(psShmHead,"AuthByWmd","No"),"Yes")==0){    
        char caAddr[36];
      	if(strlen(stHoteluser.caIdfyno)>6){
      		lUserid=psUser->userid;
      		strcpy(psUser->position,stHoteluser.caPosition);
      		strcpy(psUser->cardid,stHoteluser.caIdfyno);
      		
      	 if(strlen(stHoteluser.caFamaddr)>0){
				  	strcpy(caAddr,utStrGetCharHan(stHoteluser.caFamaddr,30));
				  }
				  else{
				  	strcpy(caAddr,"");
				  }
      //printf("update ncuser set position='%s',idfytype='%s',idfyno='%s',country='%s',idfyunit='%s' where userid=%d \n",stHoteluser.caPosition,stHoteluser.caIdfytype,stHoteluser.caIdfyno,stHoteluser.caCountry,stHoteluser.caFamaddr,lUserid);     		
      		pasDbExecSqlF("update ncuser set position='%s',idfytype='%s',idfyno='%s',country='%s',idfyunit='%s' where userid=%d ",stHoteluser.caPosition,stHoteluser.caIdfytype,stHoteluser.caIdfyno,stHoteluser.caCountry,stHoteluser.caFamaddr,lUserid);
      }
    }
        
						 if(strcasecmp(utComGetVar_sd(psShmHead,"BeatHeat","NO"),"YES") == 0){
						  	utPltPutVar(psDbHead,"beat_flag","1");
						  	long lBeattime=1000/3*utComGetVar_ld(psShmHead,"beattimeout",3600);
						  	utPltPutVarF(psDbHead,"beattime","%d",lBeattime);
						  }
                
                utPltPutVar(psDbHead,"username",caUsername);
                utPltPutVar(psDbHead,"ip",caDip);
                utPltPutVar(psDbHead,"port",caPort);
                utPltPutVar(psDbHead,"url",caUrl);
                utPltPutVarF(psDbHead,"sid","%lu",lSid);
                strcpy(caMessage1,"\0");
                if(strcasecmp(utComGetVar_sd(psShmHead,"TimeControl","No"),"Yes") == 0) {
                
                    if(psUser->sSumm.timelimit > 0) {
                        int iLeftTime;
                        utStrAddF(caMessage1,ncLang("0503上网时间:%s"),billCvtTime(psUser->thistime + psUserInfo->lConnTime));
                        iLeftTime = psUser->sSumm.timelimit - psUser->thistime - psUserInfo->lConnTime;
                        if(iLeftTime > 0) {
                            utStrAddF(caMessage1,ncLang("0031(剩余时间:%s)"),billCvtTime(iLeftTime));
                        }
                        else {
                            utPltPutVarF(psDbHead,"message",ncLang("0415您的上网时间已经用完"));
                            utPltPutVarF(psDbHead,"sid","%lu",lSid);
                            utPltOutToHtml(iFd,psMsgHead,psDbHead,caMsg);
                            ncUtlDoLogout(psShmHead,psUserInfo,"Time Over");
                            return 0;
                        }
                    }                    
                }
                if(strcasecmp(utComGetVar_sd(psShmHead,"FlowControl","No"),"Yes") == 0) {
                    if(psUser->sSumm.flowlimit > 0) {
                        long long lLeftflow;
                        utStrAddF(caMessage1,ncLang("0502上网流量:%s KB"),utStrLtoF8((psUser->thisflow + psUserInfo->lBytes[0] + psUserInfo->lBytes[1])/1024,caTemp));
                        lLeftflow = psUser->sSumm.flowlimit - psUser->thisflow - psUserInfo->lBytes[0] - psUserInfo->lBytes[1];
                        if(lLeftflow > 0) {
                            utStrAddF(caMessage1,ncLang("0030(剩余流量:%s KB)"),utStrLtoF8(lLeftflow / 1024,caTemp));
                        }
                        else {
                            utPltPutVarF(psDbHead,"message",ncLang("0414您的上网流量已经用完"));
                            utPltPutVarF(psDbHead,"sid","%lu",lSid);
                            utPltOutToHtml(iFd,psMsgHead,psDbHead,caMsg);
                            ncUtlDoLogout(psShmHead,psUserInfo,"Flow Over");
                            return 0;
                        }
                    }                    
                }
                if(!utStrIsSpaces(caMessage1)) {
                    utPltPutVarF(psDbHead,"curinfo",ncLang("0426您总共 %s"),caMessage1);
                }
                if(utStrIsSpaces(caMessage)) {
                    utPltPutVar(psDbHead,"message",ncLang("0197登陆成功, 您可以上网了"));
                }
                else {
                    utPltPutVar(psDbHead,"message",caMessage);
                }
                
        
          sprintf(caMsg,"%s%s/nc_login_ok.htm",caPlatePath,ncGetAuthPath_2(psShmHead,lLogin,atol(caEng_flag)));
 
          sprintf(caPath_all,"/home/ncmysql/nc/plate/%s",caMsg);
          if(!utFileIsExist(caPath_all)){
          	strcpy(caMsg,"nc/nc_login_ok.htm");
          }
                  
                utPltPutVar(psDbHead,"gourl",caOutUrl);
                 printf("caMsg=%s\n",caMsg);               
                utPltOutToHtml(iFd,psMsgHead,psDbHead,caMsg);
          
                return 0;
            }    

}





/* 等等科技WEB验证   使用mwduser用户信息进行验证  */               
int ncWifjkAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caUsername[32],caIp[20],mac[12];
   	char caDispname[32],caDip[20],caPort[16],caPlatePath[32],caPlate[128];
   	char caGroupname[32],caKey[32];
   	char caPassword[32],caPassword0[32];
   	char caIBMMesg[128];
   	char caMod[32];
   	char caUrl[2500];
   	char caMsg[128];
   	char caIp0[16];
   	char caMac0[32];
   	char caHost0[64];
   	char caMessage[128];
   	int  iStatus;
   	char *pMac;
   	char caMac[20];
   	char Ou[256],Group[256];
   	long lCount;
   	long8 lThisFlow,lFlowLimit;
   	unsigned long lTimeLimit,lThisTime;
   	long lLimitid,lPostflags,lWebipflags,level;
   	utPltDbHead *psDbHead;
   	char caOk[8];
   	char caSid[16];
   	unsigned long lUserid,lGroupid,lUseflags,lIp,lRate,lIp0;
   	int iReturn,lLocalflag;
   	ncUserInfo *psUserInfo=NULL;
   	ncUserCont *psUser;
   	char caMsgzs[1024];
    char *p;
    pasDbCursor *psCur;
    char news_title[256],sqlbuf[1024];
    int iNum,lSid,lStime;
    char caUsragent[256];
    long lLogin;
    char caEng_flag[16]; 
    lGroupid=0;
    long lNettime=0;
    long i;
    long lUexpirtime,lNetime,iRet_b,lQosid;
    long lUsertype,lSesid;
    char caNettime[20],caBand[64];
    char caPath_all[256];
    char caTemp[1024],caTemp2[1024];
    char caSex[16],caIdtype[16],caIdno[24],caCountry[16];
    char caAddr[256],caMobile[24];
    long lExpiretime,lTime;
    memset(Group,0,sizeof(Group));
    memset(Ou,0,sizeof(Ou));
    memset(caDispname,0,sizeof(caDispname));
    memset(caGroupname,0,sizeof(caGroupname));
    strcpy(caMod,"\0");
    strcpy(caMessage,"\0");

    
  utMsgPrintMsg(psMsgHead);
    
    
   	iReturn = utMsgGetSomeNVar(psMsgHead,3,
				"account",    UT_TYPE_STRING, 31, caUsername,
				"password",    UT_TYPE_STRING, 31, caPassword,
				"clientip",    UT_TYPE_STRING, 19, caIp);
			
		lIp = ntohl(pasIpcvtLong(caIp));	
		if((strlen(caUsername)==0)||strlen(caPassword)==0){
			 psDbHead = utPltInitDb();
			 utPltPutVar(psDbHead,"resp","NO");
       utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/mwd_dnrep.htm");
       return 0;
		}
	 sprintf(caTemp,"select expiretime,dispname,sex,idtype,idno,country,addr,mobile from ncmwduser where username='%s' and password='%s' ",caUsername,caPassword);
	 memset(caDispname,0,sizeof(caDispname));
	 memset(caSex,0,sizeof(caSex));
	 memset(caIdtype,0,sizeof(caIdtype));
	 memset(caIdno,0,sizeof(caIdno));
	 memset(caCountry,0,sizeof(caCountry));
	 memset(caAddr,0,sizeof(caAddr));
	 memset(caMobile,0,sizeof(caMobile));
	 lExpiretime=2;
 printf("caTemp=%s\n",caTemp);
 iReturn=pasDbOneRecord(caTemp,0,
                         UT_TYPE_LONG,4,&lExpiretime,
                         UT_TYPE_STRING,31,caDispname,
                         UT_TYPE_STRING,1,caSex,
                         UT_TYPE_STRING,3,caIdtype,
                         UT_TYPE_STRING,19,caIdno,
                         UT_TYPE_STRING,3,caCountry,
                         UT_TYPE_STRING,100,caAddr,
                         UT_TYPE_STRING,20,caMobile);	
	  lTime=time(0);
	  printf("iReturn=%d\n",iReturn);
	  if((iReturn==0)&&(lExpiretime>lTime||lExpiretime==0)){
	  	    psUser = ncUtlGetUserContByName(psShmHead,caUsername);
          if(psUser == NULL) { /* 用户不存在  */
          	ncUtlAddNewUser(caUsername,caDispname,NULL,NULL,NULL,NULL,0,lIp,NC_NETWORK_AUTH);
          	psUser = ncUtlGetUserContByName(psShmHead,caUsername);
            if(psUser == NULL) {
							 psDbHead = utPltInitDb();
							 utPltPutVar(psDbHead,"resp","NO");
				       utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/mwd_dnrep.htm");
				       return 0;
             }
          }
          strcpy(psUser->cardid,caIdno);
          strcpy(psUser->idtype,caIdtype);
          strcpy(psUser->position,caMobile);
          lUserid=psUser->userid;
           sprintf(caPassword0,"%s%s",caUsername,caPassword);
           utMd5Ascii22(caPassword0,strlen(caPassword0),NULL,caKey);
          	pasDbExecSqlF("update ncuser set password='%s',idfytype='%s',idfyno='%s',country='%s',idfyunit='%s',email='%s',sex='%s',position='%s,lasttime=%lu' where userid=%d ",\
          	caKey,caIdtype,caIdno,caCountry,caAddr,caMobile,caSex,caMobile,time(0),lUserid);

/*          
  printf("update ncuser set password='%s',idfytype='%s',idfyno='%s',country='%s',idfyunit='%s',email='%s',sex='%s',position='%s' where userid=%d \n",\
          	caKey,caIdtype,caIdno,caCountry,caAddr,caMobile,caSex,caMobile,lUserid);
*/          	
          	        
          psUserInfo = ncUtlDoLogin(psShmHead,psUser,NC_LOGIN_AUTH,lIp,NULL,caMessage);
           if(psUserInfo == NULL) {
           		 psDbHead = utPltInitDb();
							 utPltPutVar(psDbHead,"resp","NO");
				       utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/mwd_dnrep.htm");
				       return 0;
           }
           if(strcasecmp(utComGetVar_sd(psShmHead,"AuthSendHtml","No"),"No")==0) {
              ncCtlIptDelRedict(lIp);
            }
               psDbHead = utPltInitDb();
							 utPltPutVar(psDbHead,"resp","OK");
				       utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/mwd_dnrep.htm");
				       return 0;
          
	  }
	  else{
	  	psDbHead = utPltInitDb();
	  	 utPltPutVar(psDbHead,"resp","NO");
       utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/mwd_dnrep.htm");
       return 0;
	  }
	   
}



    /* 本地验证该帐号前面已经登录过了，再次登录  */               
int ncWebMulogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caUsername[32],caIp[20],mac[12];
   	char caDispname[32],caDip[20],caPort[16],caPlatePath[32],caPlate[128];
   	char caGroupname[32],caKey[32];
   	char caPassword[32],caPassword0[32];
   	char caIBMMesg[128];
   	char caMod[32];
   	char caUrl[2560];
   	char caMsg[128];
   	char caIp0[16];
   	char caMac0[32];
   	char caHost0[64];
   	char caMessage[128];
   	int  iStatus;
   	char *pMac;
   	char caMac[20];
   	char Ou[256],Group[256];
   	long lCount;
   	long8 lThisFlow,lFlowLimit;
   	char caUsragent[256];   					
   	unsigned long lTimeLimit,lThisTime;
   	long lLimitid,lPostflags,lWebipflags,level;
   	utPltDbHead *psDbHead;
   	char caOk[8];
   	char caSid[16];
   	unsigned long lUserid,lGroupid,lUseflags,lIp,lRate,lIp0;
   	int iReturn,lLocalflag;
   	ncUserInfo *psUserInfo=NULL;
   	ncUserCont *psUser;
    char *p;
    pasDbCursor *psCur;
    char news_title[256],sqlbuf[1024];
    int iNum,lSid,lStime;
    lGroupid=0;
    long lNettime=0;
    char caEng_flag[16];
    char caPath_all[256];
    long lFlag,lLogin;
    char caTemp2[1024],caTemp[1024];
    char caMulogflag[16];
    
    
    strcpy(caMessage,"\0");

 //           utMsgPrintMsg(psMsgHead);
    
   	iReturn = utMsgGetSomeNVar(psMsgHead,6,
				"username",    UT_TYPE_STRING, 31, caUsername,
				"password",    UT_TYPE_STRING, 31, caPassword,
	      "mulogflag",   UT_TYPE_STRING,2,caMulogflag,
				"clientip",    UT_TYPE_STRING, 19, caIp,
				"ip",          UT_TYPE_STRING,15,caDip,
				"port",        UT_TYPE_STRING,8,caPort);
				
				
							  
			 

   
    
    
    
    utStrDelSpaces(caUsername);
    utStrDelSpaces(caPassword); 
    utStrDelSpaces(caIp);
   
    /* 验证  */
    lIp = ntohl(pasIpcvtLong(caIp));	
   

          iReturn=ncAuthLocal(psShmHead,caUsername,caPassword);
          
          
          if(iReturn<0){
          	    psDbHead = utPltInitDb();
                utPltPutVar(psDbHead,"ip",caDip);
                utPltPutVar(psDbHead,"port",caPort);
                utPltPutVar(psDbHead,"url",caUrl);
                
                if(atol(caEng_flag)==1){
                	utPltPutVar(psDbHead,"message","Invalid input,please try again.");
                }
                else{
                	utPltPutVar(psDbHead,"message","验证失败,请重新验证!");
                	strcpy(caTemp,"验证失败,请重新验证!");
                  pasCharsetCvt(PAS_CCODE_GB,PAS_CCODE_UTF8,caTemp,caTemp2,63);  
     	
                	utPltPutVar(psDbHead,"message_u",caTemp2);                	

                }
                utPltPutVarF(psDbHead,"sid","%lu",lSid);
    
  //              utPltShowDb(psDbHead);
 printf("caMsg=%s\n",caMsg);
                utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_login_main.htm");
                return 0;
          }
           psUser = ncUtlGetUserContByName(psShmHead,caUsername);
          psUserInfo = (ncUserInfo *)ncGetUserInfoById(psShmHead,psUser->userid);
        if(psUserInfo) {
	         if(atol(caMulogflag)==1){
	         	//踢出原来的用户
	         	 ncUtlDoLogout(psShmHead,psUserInfo,"It is same user in system");
	        } 
        }
            psUserInfo = ncUtlDoLogin(psShmHead,psUser,NC_LOGIN_AUTH,lIp,NULL,caMessage);
            if(psUserInfo == NULL) {
  
            	
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0778用户验证"),caMessage);
                return 0;
            }
            else {
                char caTemp[32];
                char caMessage1[128];
  
        
                if(strcasecmp(utComGetVar_sd(psShmHead,"AuthSendHtml","No"),"No")==0) {
                    ncCtlIptDelRedict(lIp);
                }
                psDbHead = utPltInitDb();
        
        
						 if(strcasecmp(utComGetVar_sd(psShmHead,"BeatHeat","NO"),"YES") == 0){
						  	utPltPutVar(psDbHead,"beat_flag","1");
						  	long lBeattime=1000/3*utComGetVar_ld(psShmHead,"beattimeout",3600);
						  	utPltPutVarF(psDbHead,"beattime","%d",lBeattime);
						  }
                
                utPltPutVar(psDbHead,"username",caUsername);
                utPltPutVar(psDbHead,"ip",caDip);
                utPltPutVar(psDbHead,"port",caPort);
                utPltPutVar(psDbHead,"url",caUrl);
                utPltPutVarF(psDbHead,"sid","%lu",lSid);
                strcpy(caMessage1,"\0");
                if(strcasecmp(utComGetVar_sd(psShmHead,"TimeControl","No"),"Yes") == 0) {
                
                    if(psUser->sSumm.timelimit > 0) {
                        int iLeftTime;
                        utStrAddF(caMessage1,ncLang("0503上网时间:%s"),billCvtTime(psUser->thistime + psUserInfo->lConnTime));
                        iLeftTime = psUser->sSumm.timelimit - psUser->thistime - psUserInfo->lConnTime;
                        if(iLeftTime > 0) {
                            utStrAddF(caMessage1,ncLang("0031(剩余时间:%s)"),billCvtTime(iLeftTime));
                        }
                        else {
                            utPltPutVarF(psDbHead,"message",ncLang("0415您的上网时间已经用完"));
                            utPltPutVarF(psDbHead,"sid","%lu",lSid);
                            utPltOutToHtml(iFd,psMsgHead,psDbHead,caMsg);
                            ncUtlDoLogout(psShmHead,psUserInfo,"Time Over");
                            return 0;
                        }
                    }                    
                }
                if(strcasecmp(utComGetVar_sd(psShmHead,"FlowControl","No"),"Yes") == 0) {
                    if(psUser->sSumm.flowlimit > 0) {
                        long long lLeftflow;
                        utStrAddF(caMessage1,ncLang("0502上网流量:%s KB"),utStrLtoF8((psUser->thisflow + psUserInfo->lBytes[0] + psUserInfo->lBytes[1])/1024,caTemp));
                        lLeftflow = psUser->sSumm.flowlimit - psUser->thisflow - psUserInfo->lBytes[0] - psUserInfo->lBytes[1];
                        if(lLeftflow > 0) {
                            utStrAddF(caMessage1,ncLang("0030(剩余流量:%s KB)"),utStrLtoF8(lLeftflow / 1024,caTemp));
                        }
                        else {
                            utPltPutVarF(psDbHead,"message",ncLang("0414您的上网流量已经用完"));
                            utPltPutVarF(psDbHead,"sid","%lu",lSid);
                            utPltOutToHtml(iFd,psMsgHead,psDbHead,caMsg);
                            ncUtlDoLogout(psShmHead,psUserInfo,"Flow Over");
                            return 0;
                        }
                    }                    
                }
                if(!utStrIsSpaces(caMessage1)) {
                    utPltPutVarF(psDbHead,"curinfo",ncLang("0426您总共 %s"),caMessage1);
                }
                if(utStrIsSpaces(caMessage)) {
                    utPltPutVar(psDbHead,"message",ncLang("0197登陆成功, 您可以上网了"));
                }
                else {
                    utPltPutVar(psDbHead,"message",caMessage);
                }
                
        
            
                utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_login_ok.htm");
          
                return 0;
            }    

}