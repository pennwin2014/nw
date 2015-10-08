#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>

#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "ncdef.h"

/* 显示MAC地址和IP地址对照表  */
int ncWebShowMacIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncComputeMacIp *psMacIp;
    pasHashInfo sHashInfo;
     ncUserInfo  *psUserInfo;
    char *pHash;
    int iNum,iReturn;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
    psDbHead = utPltInitDb();
    pHash = (char *)utShmHashHead(psShmHead,NC_LNK_MACIPLIST);
    if(pHash == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0209调试软件"),ncLang("0071MAC、IP地址对照表不存在"));
        return 0;
    }
    psMacIp  = (ncComputeMacIp *)pasHashFirst(pHash,&sHashInfo);
    iNum = 0;
    while(psMacIp) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead, "maca",iNum,ncCvtMac(psMacIp->mac));
        utPltPutLoopVar(psDbHead, "mac",iNum,ncCvtMac(psMacIp->mac));
        utPltPutLoopVar(psDbHead, "ip",  iNum,utComHostIp(htonl(psMacIp->ip)));
        utPltPutLoopVar(psDbHead, "compname", iNum,psMacIp->cname);
        utPltPutLoopVarF(psDbHead, "flags",    iNum,"%d",psMacIp->flags);
        utPltPutLoopVarF(psDbHead, "status",   iNum,"%d",psMacIp->status);
        psUserInfo = (ncUserInfo *)ncGetUserInfoByIp(psShmHead,psMacIp->ip);
        if(psUserInfo) {
            if(psUserInfo->psUser) {
                utPltPutLoopVar(psDbHead, "username",  iNum,psUserInfo->psUser->username);
                utPltPutLoopVar(psDbHead, "dispname",  iNum,psUserInfo->psUser->dispname);
            }
        }            
        psMacIp  = (ncComputeMacIp *)pasHashNext(&sHashInfo);
    }
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_maciplist.htm");
    return 0;
}    

int ncWebShowIpMac(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncComputeIpMac *psIpMac;
    pasHashInfo     sHashInfo;
    ncUserInfo      *psUserInfo;
    char flag_desc[16];
    char            *pHash,*p,*pSelect;
    int             iNum,iReturn,i,iFlags,iSear,iStart,k,n;
    char  caIp[16],caMac[18],caCname[16],caSear[12],caDel[12],caModi[12],caApp[12],caMacI[6],caCurpg[12];
    char  caUserName[32],caDispName[32],caCompName[32];
    char  caFlags[12],caExport[12],caVar[16];
    char  caFilename[64],caPath[64],caFile[128];
    FILE  *fp;
    long lSflag;
    unsigned long   lIp;
    long            lRowNum,lCurPg,lTotPg,lStartRec;
    utPltDbHead *psDbHead;
        long lGroupid0=0;
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
    
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,10,
   	                "ip", UT_TYPE_STRING, 15, caIp,
   	                "mac",UT_TYPE_STRING, 17, caMac,
   	                "cname",UT_TYPE_STRING, 16, caCname,
   	                "flags",UT_TYPE_STRING,2,caFlags,
					"CurPg",UT_TYPE_STRING, 11, caCurpg,
					"sear", UT_TYPE_STRING, 11, caSear,
					"del",  UT_TYPE_STRING, 11, caDel,
					"modi", UT_TYPE_STRING, 11, caModi,
					"app",      UT_TYPE_STRING,11,caApp,
					"exp",  UT_TYPE_STRING,11,caExport);
 iSear = 1;
 if(strlen(caFlags)==0){
 	lSflag=999;
}else{
lSflag=atol(caFlags);
}
    lRowNum = 23;
    if(utStrIsSpaces(caCurpg)) {
        lCurPg = 1;
    }
    else {
        lCurPg = atol(caCurpg);
    }
    lStartRec = (lCurPg - 1) * lRowNum;
    
    if(!utStrIsSpaces(caModi)) {  /* 更新  */
         if(utStrIsValidIp(caIp)) {
            lIp = ntohl(utComHostAddress(caIp));
         }
         else {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0048IP&MAC地址维护"),ncLang("0059IP地址格式不正确"));
            return 0;
         }
         iReturn = ncCvtMacI(caMac,caMacI);
         if(iReturn == (-1)) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0048IP&MAC地址维护"),ncLang("0077MAC地址格式不正确"));
            return 0;
         }
         iFlags = atol(caFlags);
        iSear = 0;  
         ncUtlAddArpTable(psShmHead,lIp,caMacI,iFlags,0,3,
                    NC_ARPATTR_USERNAME, caCname, strlen(caCname)+1,
                    NC_ARPATTR_DISPNAME, caCname, strlen(caCname)+1,
                    NC_ARPATTR_COMPUTER, caCname, strlen(caCname)+1);
    }
    if(!utStrIsSpaces(caDel)) {  /* 删除  */
    	char caVar2[32];
    	char *p2;
    	strcpy(caIp,"");
    	strcpy(caMac,"");
    	strcpy(caCname,"");
        for(i=0;i<lRowNum;i++) {
        	sprintf(caVar2,"flag%d",i+1);
        	p2=utMsgGetVar_s(psMsgHead,caVar2);
            sprintf(caVar,"ch%d",i+1);
            
            p = utMsgGetVar_s(psMsgHead,caVar);
            if(p&&p2) {
                ncComputeIpMac s;
                memset(&s,0,sizeof(s));
                lIp = ntohl(utComHostAddress(p));
             
                s.ip = lIp;
                s.flags = atol(p2);
                utShmHashDel(psShmHead,NC_LNK_IPMACLIST,&s);
            }
        }
       
        
    }
    if(!utStrIsSpaces(caApp)) {  /* 应用  */
        ncUtlBackupMacList(psShmHead);
        ncUtlRestoreMacList(psShmHead);
    }
    if(!utStrIsSpaces(caExport)) {  /* 导出  */
        pHash = (char *)utShmHashHead(psShmHead,NC_LNK_IPMACLIST);
        if(pHash == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0209调试软件"),ncLang("0071MAC、IP地址对照表不存在"));
            return 0;
        }
        psIpMac  = (ncComputeIpMac *)pasHashFirst(pHash,&sHashInfo);
        if(psIpMac == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0209调试软件"),ncLang("0298记录不存在"));
            return 0;
        }            
        sprintf(caFilename,"ncipmac%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0216对照表"),ncLang("0546数据导出出错"));
            return 0;
        }
        ncUtlPrintCsv(fp,7,UT_TYPE_STRING,ncLang("0711序号"),
                       UT_TYPE_STRING,ncLang("0054IP地址"),
                       UT_TYPE_STRING,ncLang("0072MAC地址"),
                       UT_TYPE_STRING,ncLang("0728用户名"),
                       UT_TYPE_STRING,ncLang("0290计算机名"),
                       UT_TYPE_STRING,ncLang("1835显示名"),
                       UT_TYPE_STRING,ncLang("0357来源"));
        iNum = 0;
        while(psIpMac) {
            iNum++;
            strcpy(caUserName,"\0");
            strcpy(caDispName,"\0");
            strcpy(caCompName,"\0");
            n = ncStrGetSomeAttrValue(psIpMac->attr,3,
                    NC_ARPATTR_USERNAME,caUserName,31,
                    NC_ARPATTR_DISPNAME,caDispName,31,
                    NC_ARPATTR_COMPUTER,caCompName,31);
                    
            ncUtlPrintCsv(fp,7,UT_TYPE_LONG,iNum,
                       UT_TYPE_STRING,utComHostIp(htonl(psIpMac->ip)),
                       UT_TYPE_STRING,ncCvtMac(psIpMac->mac),
                       UT_TYPE_STRING,caUserName,
                       UT_TYPE_STRING,caCompName,
                       UT_TYPE_STRING,caDispName,
                       UT_TYPE_LONG,psIpMac->flags);
            psIpMac  = (ncComputeIpMac *)pasHashNext(&sHashInfo);
        }
        fclose(fp);
        utPltFileDownload(iFd,"application/csv",caPath,caFilename,caFilename);
        remove(caFile);
        return 0;
    }
    if(!utStrIsSpaces(caSear)) {
        if(utStrIsValidIp(caIp)) {
           lIp = ntohl(utComHostAddress(caIp));
        }
        iReturn = ncCvtMacI(caMac,caMacI);
        iSear = 1;
        lCurPg = 1;
    }
 //   else {
 //       iSear = 0;
//    }
    psDbHead = utPltInitDb();
    pHash = (char *)utShmHashHead(psShmHead,NC_LNK_IPMACLIST);
    if(pHash == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0209调试软件"),ncLang("0071MAC、IP地址对照表不存在"));
        return 0;
    }
    psIpMac  = (ncComputeIpMac *)pasHashFirst(pHash,&sHashInfo);
    iStart = 0;
    while(psIpMac && iStart < lStartRec) {
        strcpy(caUserName,"\0");
        strcpy(caDispName,"\0");
        strcpy(caCompName,"\0");
        n = ncStrGetSomeAttrValue(psIpMac->attr,3,
                    NC_ARPATTR_USERNAME,caUserName,31,
                    NC_ARPATTR_DISPNAME,caDispName,31,
                    NC_ARPATTR_COMPUTER,caCompName,31);
        if(iSear == 0 || ( ( utStrIsSpaces(caIp) || lIp == psIpMac->ip) &&
                               ( utStrIsSpaces(caMac) || memcmp(caMacI,psIpMac->mac,6) == 0) &&
                               ( utStrIsSpaces(caCname) || strstr(caUserName,caCname) || strstr(caDispName,caCname) ||
                                strstr(caCompName,caCname))&&(lSflag==999||lSflag==psIpMac->flags))) {
                iStart++;
        }
        psIpMac  = (ncComputeIpMac *)pasHashNext(&sHashInfo);
    }
    iNum = 0;
    while(psIpMac && iNum < lRowNum) {
        strcpy(caUserName,"\0");
        strcpy(caDispName,"\0");
        strcpy(caCompName,"\0");
        n = ncStrGetSomeAttrValue(psIpMac->attr,3,
                    NC_ARPATTR_USERNAME,caUserName,31,
                    NC_ARPATTR_DISPNAME,caDispName,31,
                    NC_ARPATTR_COMPUTER,caCompName,31);
            
        if(iSear == 0 || ( ( utStrIsSpaces(caIp) || lIp == psIpMac->ip) &&
                               ( utStrIsSpaces(caMac) || memcmp(caMacI,psIpMac->mac,6) == 0) &&
                               ( utStrIsSpaces(caCname) || strstr(caUserName,caCname) || strstr(caDispName,caCname) ||
                                strstr(caCompName,caCname))&&(lSflag==999||lSflag==psIpMac->flags))) {
                iStart++;
                iNum++;
                utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
                utPltPutLoopVar(psDbHead, "lmac",iNum,ncCvtMac(psIpMac->mac));
                utPltPutLoopVar(psDbHead, "lip",      iNum,utComHostIp(htonl(psIpMac->ip)));
                utPltPutLoopVar(psDbHead, "lcname", iNum,caDispName);
                if(psIpMac->flags==1){
                	strcpy(flag_desc,"ARP");
                }
                else if(psIpMac->flags==2){
                	strcpy(flag_desc,"WINS");
                }
                else if(psIpMac->flags==3){
                	strcpy(flag_desc,"THIRD");
                }
                else if(psIpMac->flags==4){
                	strcpy(flag_desc,"CLIENT");
                }
                else if(psIpMac->flags==5){
                	strcpy(flag_desc,"SNMP");
                }
                else if(psIpMac->flags==6){
                	strcpy(flag_desc,"SWITCH");
                }
                else if(psIpMac->flags==7){
                	strcpy(flag_desc,"DHCP");
                }
                else if(psIpMac->flags==8){
                	strcpy(flag_desc,"POST");
                }
                else if(psIpMac->flags==9){
                	strcpy(flag_desc,"FIXED");
                }
                else if(psIpMac->flags==10){
                	strcpy(flag_desc,"PROXY");
                }
                else if(psIpMac->flags==11){
                	strcpy(flag_desc,"RADIUS");
                }
                else if(psIpMac->flags==12){
                	strcpy(flag_desc,"CAMS");
                }
                else if(psIpMac->flags==MACIP_FROM_PORT){
                	strcpy(flag_desc,"PORT");
                }
                else if(psIpMac->flags==MACIP_FROM_VLAN){
                	strcpy(flag_desc,"VLAN");
                }
                else if(psIpMac->flags==MACIP_FROM_POP3){
                	strcpy(flag_desc,"POP3");
                }
                else if(psIpMac->flags==MACIP_FROM_PPPOE){
                	strcpy(flag_desc,"PPPOE");
                }
                else if(psIpMac->flags==MACIP_FROM_QQ){
                	strcpy(flag_desc,"QQ");
                }
                else if(psIpMac->flags==0){
                	strcpy(flag_desc,"Packet");
                }
                else 
                  strcpy(flag_desc,"");
                utPltPutLoopVar(psDbHead, "lflags",    iNum,flag_desc);
                utPltPutLoopVarF(psDbHead, "status",   iNum,"%d",psIpMac->status);
                utPltPutLoopVarF(psDbHead,"typeflag",iNum,"%d",psIpMac->flags);
                psUserInfo = (ncUserInfo *) ncGetUserInfoByIp(psShmHead,psIpMac->ip);
                if(psUserInfo) {
                    if(psUserInfo->psUser) {
                        utPltPutLoopVar(psDbHead, "username",  iNum,psUserInfo->psUser->username);
                        utPltPutLoopVar(psDbHead, "dispname",  iNum,psUserInfo->psUser->dispname);
                    }
                }
        }
        psIpMac  = (ncComputeIpMac *)pasHashNext(&sHashInfo);
    }
    while(psIpMac) {
        strcpy(caUserName,"\0");
        strcpy(caDispName,"\0");
        strcpy(caCompName,"\0");
        n = ncStrGetSomeAttrValue(psIpMac->attr,3,
                    NC_ARPATTR_USERNAME,caUserName,31,
                    NC_ARPATTR_DISPNAME,caDispName,31,
                    NC_ARPATTR_COMPUTER,caCompName,31);
        if(iSear == 0 || ( ( utStrIsSpaces(caIp) || lIp == psIpMac->ip) &&
                               ( utStrIsSpaces(caMac) || memcmp(caMacI,psIpMac->mac,6) == 0) &&
                               ( utStrIsSpaces(caCname) || strstr(caUserName,caCname) || strstr(caDispName,caCname) ||
                                strstr(caCompName,caCname))&&(lSflag==999||lSflag==psIpMac->flags))) {
            iStart++;
        }
        psIpMac  = (ncComputeIpMac *)pasHashNext(&sHashInfo);
    }
    k=0;
    while(iNum < lRowNum) {
        k++;
        utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
        iNum++;
    }

    lTotPg = (iStart - 1) / lRowNum + 1;
    utPltPutVarF(psDbHead,"ip","%s",caIp);
    utPltPutVarF(psDbHead,"mac","%s",caMac);
    utPltPutVarF(psDbHead,"cname","%s",caCname);
    pSelect = (char *)pasUtlCvtInputSelect("flags",caFlags,"All,;FIXED,9;DHCP,7;WINS,2;THIRD,3;SWITCH,6;CLIENT,4;SNMP,5;Packet,0;ARP,1;POST,8;PROXY,10;Radius,11;CAMS,12;");
    if(pSelect) {
        utPltPutVar(psDbHead,"flags", pSelect);
        free(pSelect);
    }
    utPltPutVarF(psDbHead,"CurPg","%d",lCurPg);
    utPltPutVarF(psDbHead,"TotPg","%d",lTotPg);
    utPltPutVarF(psDbHead,"TotRec","%d",iStart);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_list_ipmac.htm");
    return 0;
}    


/* 检查用户信息的唯一性   */
int ncWebCheckUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncUserCont *psUser;
    char *pSelect;
    char *p;
    unsigned long lUserid,lAddtime,lLasttime,lIp;
   	char caDispname[32],caUsername[32],caMac[32],caCompname[32],caDel[16],caSum[16];
    unsigned long lUserid0,lAddtime0,lLasttime0,lIp0;
   	char caDispname0[32],caUsername0[32],caMac0[32],caCompname0[32];
    char caVar[16];
    int i,iNum,iDup,iStatus,lSum;
    utPltDbHead *psDbHead;
   	int iReturn;
   	char caOpt[16];
   	char caOrder[32];
 	pasDbCursor *psCur;
 	   long lGroupid0=0;
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
 	
 	iDup = 0;
 	iStatus = 0;
	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,3,
					"opt",   UT_TYPE_STRING, 1, caOpt,
					"del",   UT_TYPE_STRING, 1, caDel,
					"sum",   UT_TYPE_STRING, 8, caSum);
    if(!utStrIsSpaces(caDel)) {
        lSum = atol(caSum);
        for(i=0;i<lSum;i++) {
            sprintf(caVar,"ch%d",i+1);
            p = utMsgGetVar_s(psMsgHead,caVar);
            if(p) {
                lUserid = atol(p);
                iReturn = pasDbExecSqlF("delete from ncuser where userid = %lu",lUserid);
            }
        }
    }
   	if(caOpt[0] == '0') { /* Ip地址唯一性检查  */
   	    strcpy(caOrder,"ip");
    }
    else if(caOpt[0] == '1') { /* Mac地址唯一性检查  */
        strcpy(caOrder,"mac");
    }
    else if(caOpt[0] == '2') { /* 计算机名唯一性检查  */
        strcpy(caOrder,"compname");
    }
    else if(caOpt[0] == '3') { /* 用户名唯一性检查  */
        strcpy(caOrder,"username");
    }
    else if(caOpt[0] == '4') { /* 显示名唯一性检查  */
        strcpy(caOrder,"dispname");
    }
    else {
        strcpy(caOrder,"ip");
    }        
    psCur = pasDbOpenSqlF("select userid,dispname,username,compname,mac,ip,addtime,lasttime from ncuser order by %s",caOrder);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
    psDbHead = utPltInitDb();

   	lUserid0 = 0;
   	strcpy(caDispname0,"\0");
    strcpy(caMac0,"\0");
   	strcpy(caUsername0,"\0");
   	lIp0 = 0;
   	lAddtime0 = 0;
   	lLasttime0 = 0;
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lUserid,
  	                               UT_TYPE_STRING,31,caDispname,
  	                               UT_TYPE_STRING,31,caUsername,
  	                               UT_TYPE_STRING,31,caCompname,
  	                               UT_TYPE_STRING,31,caMac,
  	                               UT_TYPE_ULONG,4,&lIp,
  	                               UT_TYPE_ULONG,4,&lAddtime,
  	                               UT_TYPE_ULONG,4,&lLasttime);
   	iNum = 0;
   	iDup = 0;
   	while(iReturn == 0 && iNum < 100) {
   	    if(caOpt[0] == '0') { /* Ip地址唯一性检查  */
   	        iStatus = (lIp != 0 && lIp == lIp0);
   	    }
   	    else if(caOpt[0] == '1') { /* Mac地址唯一性检查  */
   	        iStatus = (!utStrIsSpaces(caMac) && strcmp(caMac,"00:00:00:00:00:00")!=0 && strcmp(caMac,caMac0) == 0);
   	    }
   	    else if(caOpt[0] == '2') { /* 计算机名唯一性检查  */
   	        iStatus = (strcmp(caCompname,caCompname0) == 0);
   	    }
   	    else if(caOpt[0] == '3') { /* 用户名唯一性检查  */
   	        iStatus = (strcmp(caUsername,caUsername0) == 0);
   	    }
   	    else if(caOpt[0] == '4') { /* 显示名唯一性检查  */
   	        iStatus = (strcmp(caDispname,caDispname0) == 0);
   	    }
   	    else {
   	        iStatus = (lIp != 0 && lIp == lIp0);
        }   	        
   	    if(iStatus) {
   	        if(iDup == 0) {
   	            iNum++;
                utPltPutLoopVarF(psDbHead,"userid", iNum,"%lu",lUserid0);
                utPltPutLoopVar(psDbHead,"username", iNum,caUsername0);
                utPltPutLoopVar(psDbHead,"dispname", iNum,caDispname0);
                psUser = (ncUserCont *)ncUtlGetUserContById(psShmHead,lUserid0);
                if(psUser && psUser->psGroup) {
                    utPltPutLoopVar(psDbHead,"groupname", iNum,psUser->psGroup->groupname);
                }
                utPltPutLoopVar(psDbHead,"ip", iNum,utComHostIp(htonl(lIp0)));
                utPltPutLoopVar(psDbHead,"mac", iNum,caMac0);
                utPltPutLoopVar(psDbHead,"addtime", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime0));
                utPltPutLoopVar(psDbHead,"lasttime", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lLasttime0));
                utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
   	        }
   	        iDup++;
            iNum++;
            utPltPutLoopVarF(psDbHead,"userid", iNum,"%lu",lUserid);
            utPltPutLoopVar(psDbHead,"username", iNum,caUsername);
            utPltPutLoopVar(psDbHead,"dispname", iNum,caDispname);
            psUser = (ncUserCont *)ncUtlGetUserContById(psShmHead,lUserid);
            if(psUser  && psUser->psGroup) {
                utPltPutLoopVar(psDbHead,"groupname", iNum,psUser->psGroup->groupname);
            }
            utPltPutLoopVar(psDbHead,"ip", iNum,utComHostIp(htonl(lIp)));
            utPltPutLoopVar(psDbHead,"mac", iNum,caMac);
            utPltPutLoopVar(psDbHead,"addtime", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime));
            utPltPutLoopVar(psDbHead,"lasttime", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lLasttime));
            utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
   	    }
   	    else {
   	        iDup = 0;
   	        lUserid0 = lUserid;
   	        strcpy(caDispname0,caDispname);
   	        strcpy(caMac0,caMac);
   	        strcpy(caUsername0,caUsername);
   	        strcpy(caCompname0,caCompname);
   	        lIp0 = lIp;
   	        lAddtime0 = lAddtime;
   	        lLasttime0 = lLasttime;
   	    }
   	    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lUserid,
  	                               UT_TYPE_STRING,31,caDispname,
  	                               UT_TYPE_STRING,31,caUsername,
  	                               UT_TYPE_STRING,31,caCompname,
  	                               UT_TYPE_STRING,31,caMac,
  	                               UT_TYPE_ULONG,4,&lIp,
  	                               UT_TYPE_ULONG,4,&lAddtime,
  	                               UT_TYPE_ULONG,4,&lLasttime);
    }
    pasDbCloseCursor(psCur);
    pSelect = (char *)pasUtlCvtInputSelect("opt",caOpt,ncLang("0055IP地址,0;MAC地址,1;电脑名,2;用户名,3;显示名,4;"));
    if(pSelect) {
        utPltPutVar(psDbHead,"opt", pSelect);
        free(pSelect);
    }
    if(iNum == 0) {
        utPltPutVar(psDbHead,"message", (char *)ncLang("0377没有发现重复记录"));
    }
    utPltPutVarF(psDbHead,"sum","%lu",iNum);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_dupuser.htm");
    return 0;
}
    


/* 显示数据同步状态  */
int ncWebShowDbSyncStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    ncUtlDBSynInfo *psDbSyn;
    int iReturn;
    char caTemp[32];
    utPltDbHead *psDbHead;
       long lGroupid0=0;
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
    
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
    psDbSyn = (ncUtlDBSynInfo *)utShmArray(psShmHead,NC_LNK_DBSYNCINFO);
    if(psDbSyn == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0209调试软件"),ncLang("0581同步服务没有启动"));
        return 0;
    }
    psDbHead = utPltInitDb();
    
    utPltPutVarF(psDbHead,"fpid", "%d",psDbSyn->iFileServerId);
    utPltPutVarF(psDbHead,"dpid", "%d",psDbSyn->iProcessId);
    utPltPutVarF(psDbHead,"starttime", "%s", utTimFormat("%Y-%m-%d %H:%M:%S",psDbSyn->lStartTime));
    utPltPutVarF(psDbHead,"sumfile", "%s", utStrLtoF(psDbSyn->lMailMon+psDbSyn->lFormMon+psDbSyn->lMailCtl,caTemp));
    utPltPutVarF(psDbHead,"sumdbfile", "%s", utStrLtoF(psDbSyn->lDbLog,caTemp));
    utPltPutVarF(psDbHead,"bytes", "%s", utStrLtoF8(psDbSyn->lBytes,caTemp));
    utPltPutVarF(psDbHead,"lasttime", "%s", utTimFormat("%Y-%m-%d %H:%M:%S",psDbSyn->lLastTime));
    utPltPutVarF(psDbHead,"lastdotime", "%s", utTimFormat("%Y-%m-%d %H:%M:%S",psDbSyn->lLastDoTime));
    utPltPutVarF(psDbHead,"thistime", "%s", utTimFormat("%Y-%m-%d %H:%M:%S",time(0)));
    utPltPutVarF(psDbHead,"undofile", "%lu", psDbSyn->lUndoFile);
    utPltPutVarF(psDbHead,"dofile", "%s", psDbSyn->caDoFile);
    utPltPutVarF(psDbHead,"curfile", "%s", psDbSyn->caCurFile);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_dbsync.htm");
    return 0;
}    



/* 网址库在线升级  */
int ncWebOnlineUpdateUrl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    pasConfList *psConfig;
    char        *pConfig;
    int iReturn;
    char        caUrltime[8],caAutourl[8],caUpdateserver[32];
    char        caUpnow[8],caRedown[8],caApp[8],caSave[8];
    unsigned long lUrlLastTime,l1;
    char caSoftver[32],caDbver[32],caUpdate[32];
    long iSumRecord;
    utPltDbHead *psDbHead;
       long lGroupid0=0;
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
    
    
    
    
    
    
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

	iReturn = utMsgGetSomeNVar(psMsgHead,7,
					"urltime",UT_TYPE_STRING, 4, caUrltime,
					"autourl",UT_TYPE_STRING, 4, caAutourl,
					"updateserver",UT_TYPE_STRING,31,caUpdateserver,
					"upnow",  UT_TYPE_STRING, 4, caUpnow,
					"redown", UT_TYPE_STRING, 4, caRedown,
					"app",    UT_TYPE_STRING, 4, caApp,
					"save",   UT_TYPE_STRING, 4, caSave);
    pConfig = (char *)pasGetConfigFileName();
    psConfig = (pasConfList *)pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0623网址库在线升级"),ncLang("0427配置文件出错"));
        return 0;
    }

    if(!utStrIsSpaces(caSave) || !utStrIsSpaces(caApp) || !utStrIsSpaces(caUpnow) || !utStrIsSpaces(caRedown)) {
        pasUtlModiConfig(psConfig,"UrlUptime",caUrltime);
        if(strcasecmp(caAutourl,"Yes") == 0) {
            pasUtlModiConfig(psConfig,"AutoUpdateUrl","Yes");
        }
        else {
            pasUtlModiConfig(psConfig,"AutoUpdateUrl","No");
        }
        pasUtlModiConfig(psConfig,"UpdateServer",caUpdateserver);
        pasUtlSaveConfig(pConfig,psConfig);
        pasUtlFreeConfig(psConfig);
    }
    if(!utStrIsSpaces(caApp) || !utStrIsSpaces(caUpnow) || !utStrIsSpaces(caRedown)) {
        ncUtlReReadConFig(psShmHead);
    }
    if(!utStrIsSpaces(caUpnow)) { /* 立即更新  */
        ncUpdateUrlNow(psShmHead);
    }
    if(!utStrIsSpaces(caRedown)) {
        ncSetLastInfo_l("UrlIndLastTime",0);
        ncSetLastInfo_l("UrlLstLastTime",0);
        ncUpdateUrlNow(psShmHead);
    }
    
    psConfig = (pasConfList *)pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0623网址库在线升级"),ncLang("0427配置文件出错"));
        return 0;
    }

    psDbHead = utPltInitDb();

    ncUrlGetUrlVersion(caSoftver,caDbver,caUpdate,&iSumRecord);
    utPltPutVar(psDbHead,"softversion",caSoftver);
    utPltPutVar(psDbHead,"webversion",caDbver);
    utPltPutVar(psDbHead,"update",caUpdate);
    utPltPutVarF(psDbHead,"isum","%d",iSumRecord);





    utPltPutVar(psDbHead,"productsn",   (char *)pasUtlLookConfig(psConfig,"ProductSn","\0"));
    utPltPutVar(psDbHead,"urltime",(char *)pasUtlLookConfig(psConfig,"UrlUptime","24"));
    if(strcasecmp((char *)pasUtlLookConfig(psConfig,"AutoUpdateUrl","No"),"Yes")==0) {
        utPltPutVar(psDbHead,"autourl", "checked");
    }
    utPltPutVar(psDbHead,"updateserver", (char *)pasUtlLookConfig(psConfig,"UpdateServer","192.168.0.18:5188"));
    
    l1 = ncGetLastInfo_l("UrlLstLastTime",0);
    lUrlLastTime = ncGetLastInfo_l("UrlIndLastTime",0);
    if(lUrlLastTime < l1) lUrlLastTime = l1;
    utPltPutVarF(psDbHead,"lasttime", "%s", utTimFormat("%Y-%m-%d %H:%M:%S",lUrlLastTime));
    utPltPutVarF(psDbHead,"upnum","%d",ncGetLastInfo_l("UrlNum",0));
  
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_online_update.htm");
    return 0;
}   



 /* 协议库在线升级  */
int ncWebOnlineUpdateService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    pasConfList *psConfig;
    char        *pConfig;
    int iReturn;
    char        caUrltime[8],caAutourl[8],caUpdateserver[32];
    char        caUpnow[8],caRedown[8],caApp[8],caSave[8];
    unsigned long lUrlLastTime,l1;
    char caSoftver[32],caDbver[32],caUpdate[32];
    char caMesg2[128];
    char caCtime[24];
    long iSumRecord;
    utPltDbHead *psDbHead;
    long lGroupid0=0;
      
      
    struct stat sb;   
    stat("/home/ncmysql/nc/bin/proeimpat.bin",   &sb);      
    strcpy(caCtime,utTimFormat("%Y-%m-%d %H:%M:%S",sb.st_ctime));
      
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
    
    
     pConfig = (char *)pasGetConfigFileName();
    psConfig = (pasConfList *)pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0623网址库在线升级"),ncLang("0427配置文件出错"));
        return 0;
    }
    
    
    
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

	iReturn = utMsgGetSomeNVar(psMsgHead,8,
					"urltime",UT_TYPE_STRING, 4, caUrltime,
					"autourl",UT_TYPE_STRING, 4, caAutourl,
					"updateserver",UT_TYPE_STRING,31,caUpdateserver,
					"upnow",  UT_TYPE_STRING, 4, caUpnow,
					"redown", UT_TYPE_STRING, 4, caRedown,
					"app",    UT_TYPE_STRING, 4, caApp,
					"save",   UT_TYPE_STRING, 4, caSave,
					"upserv", UT_TYPE_STRING,16,caMesg2);
 
 
    psDbHead = utPltInitDb();

    ncPatGetSysInfo(caSoftver,caUpdate,&iSumRecord,caDbver);

    utPltPutVar(psDbHead,"upserv",caMesg2);
    
//网络服务   
    utPltPutVar(psDbHead,"softversion",caSoftver);
    utPltPutVar(psDbHead,"webversion",caDbver);
    utPltPutVar(psDbHead,"update",caUpdate);
    utPltPutVar(psDbHead,"crtime",caCtime);
    utPltPutVarF(psDbHead,"isum","%d",iSumRecord);
    
    //网址库
     ncUrlGetUrlVersion(caSoftver,caDbver,caUpdate,&iSumRecord);
     stat("/home/ncmysql/nc/conf/url/ncurl.udb",   &sb);      
     strcpy(caCtime,utTimFormat("%Y-%m-%d %H:%M:%S",sb.st_ctime));
    
    
      utPltPutVar(psDbHead,"engurl",caSoftver);
	    utPltPutVar(psDbHead,"urlversion",caDbver);
	    utPltPutVar(psDbHead,"urlupdate",caUpdate);
	    utPltPutVar(psDbHead,"urlcrtime",caCtime);
	    utPltPutVarF(psDbHead,"urlisum","%d",iSumRecord);
    //软件信息
    
     stat("/home/ncmysql/nc/bin/nc",   &sb);      
     strcpy(caCtime,utTimFormat("%Y-%m-%d %H:%M:%S",sb.st_ctime)); 
      utPltPutVar(psDbHead,"ncversion",ncVersion());
      utPltPutVar(psDbHead,"ncupdate",NC_RELEASE);
      utPltPutVar(psDbHead,"nccrtime",caCtime);
    
    
    
    utPltPutVar(psDbHead,"upserverip",   (char *)pasUtlLookConfig(psConfig,"upserverip","\0"));
    utPltPutVar(psDbHead,"upserverport",   (char *)pasUtlLookConfig(psConfig,"upserverport","\0"));
    
    utPltPutVar(psDbHead,"upstep",   (char *)pasUtlLookConfig(psConfig,"upstep","24"));
    
    
    
    utPltPutVar(psDbHead,"autoupdate",   (char *)pasUtlLookConfig(psConfig,"autoupdate","No"));
    
    
    if(strcasecmp(pasUtlLookConfig(psConfig,"upserverflag","No"),"Yes")==0) {
        utPltPutVar(psDbHead,"upserverflag","checked");
    } 
    
    if(strcasecmp(pasUtlLookConfig(psConfig,"upurlflag","No"),"Yes")==0) {
        utPltPutVar(psDbHead,"upurlflag","checked");
    } 
    
    if(strcasecmp(pasUtlLookConfig(psConfig,"upbbsflag","No"),"Yes")==0) {
        utPltPutVar(psDbHead,"upbbsflag","checked");
    } 
    if(strcasecmp(pasUtlLookConfig(psConfig,"upncflag","No"),"Yes")==0) {
        utPltPutVar(psDbHead,"upncflag","checked");
    }
    
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_online_updateservice.htm");
    return 0;
} 

//保存网络服务升级参数
int ncWebUpServiceSave_par(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caApp[20];
    char caMsg[256];
    int iReturn;
    char caDbname[32],caDbuser[32],caDbpass[32];
    char *pConfig;
    char *p,*pFile,*pNat,*pRoute;
    FILE *fp;
    pasConfList *psConfig;
    if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebSystemSave_par");
 	}
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880自动备份设定"),ncLang("0427配置文件出错"));
        return 0;
    }
    
    if(p = utMsgGetVar_s(psMsgHead,"upserverip")) {
        pasUtlModiConfig(psConfig,"upserverip",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"upserverport")) {
        pasUtlModiConfig(psConfig,"upserverport",p);
    }


    pasUtlSaveConfig(pConfig,psConfig);
    pasUtlFreeConfig(psConfig);
   
    ncUtlReReadConFig(psShmHead);
    ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
       
    	
    ncWebOnlineUpdateService(psShmHead,iFd,psMsgHead);

    return 0;
}




int ncWebAutoSave_Update(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   
    int iReturn;
    char *pConfig;
    char *p;
    pasConfList *psConfig;
    char caAutoupdate[16],caUpserverflag[16],caUpurlflag[16],caUpbbsflag[16],caUpncflag[16];
    iReturn = utMsgGetSomeNVar(psMsgHead,5,
                    "autoupdate",    UT_TYPE_STRING, 12, caAutoupdate,
                    "upserverflag",    UT_TYPE_STRING, 12, caUpserverflag,
                    "upurlflag",    UT_TYPE_STRING, 12, caUpurlflag,
                    "upbbsflag",    UT_TYPE_STRING, 12, caUpbbsflag,
                    "upncflag",    UT_TYPE_STRING, 12, caUpncflag);
                    
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880自动备份设定"),ncLang("0427配置文件出错"));
        return 0;
    }
    pasUtlModiConfig(psConfig,"autoupdate",caAutoupdate);

    if(utStrIsSpaces(caUpserverflag)) pasUtlModiConfig(psConfig,"upserverflag","No");
                                 else pasUtlModiConfig(psConfig,"upserverflag","Yes");
    if(utStrIsSpaces(caUpurlflag)) pasUtlModiConfig(psConfig,"upurlflag","No");
                                 else pasUtlModiConfig(psConfig,"upurlflag","Yes");
    if(utStrIsSpaces(caUpbbsflag)) pasUtlModiConfig(psConfig,"upbbsflag","No");
                                 else pasUtlModiConfig(psConfig,"upbbsflag","Yes");
    if(utStrIsSpaces(caUpncflag)) pasUtlModiConfig(psConfig,"upncflag","No");
                                 else pasUtlModiConfig(psConfig,"upncflag","Yes");

    if(p = utMsgGetVar_s(psMsgHead,"upstep")) {
        pasUtlModiConfig(psConfig,"upstep",p);
    }
    pasUtlSaveConfig(pConfig,psConfig);
    pasUtlFreeConfig(psConfig);

    ncUtlReReadConFig(psShmHead);
    ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
   
    
   ncWebOnlineUpdateService(psShmHead,iFd,psMsgHead);
    return 0;
}







//远程升级服务协议库
int ncWebUpdateService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   
    int iReturn;

    char caUpfile_flag[12];
    utPltDbHead *psDbHead;
    utMsgGetSomeNVar(psMsgHead,1,"upfile_flag",UT_TYPE_STRING,2,caUpfile_flag);
        
      if(atol(caUpfile_flag)==1){
      	utComSetVar_l(psShmHead,"update_pat",1);
//      	update_pat=1;
      }
      else if(atol(caUpfile_flag)==2){
      	utComSetVar_l(psShmHead,"update_url",1);
 //     	update_url=1;
      }
      else if(atol(caUpfile_flag)==3){
      	utComSetVar_l(psShmHead,"update_bbs",1);
//      	update_nc=1;
      }
      else if(atol(caUpfile_flag)==4){
      	utComSetVar_l(psShmHead,"update_nc",1);
 //     	update_webmail=1;
      }
      else{
      	utComSetVar_l(psShmHead,"update_pat",1);
      }  
                     
		       
   psDbHead = utPltInitDb();
   utPltPutVar(psDbHead,"upfile_flag",caUpfile_flag);
   utPltPutVar(psDbHead,"disable","disabled");
//   utPltPutVar(psDbHead,"mesg","正在升级");
   utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_updateservice_first.htm");
    return 0;
}



//远程升级服务协议库第一步
int ncWebUpdateServiceFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   utPltDbHead *psDbHead;
    int iReturn;
    char caMesg[128];
    FILE *fp;
    char *pBuf;
    char caServerIp[20];
    char caServerPort[16];
    char caFile[256];
    unsigned long lIp;
    long nPort,tPort;
    long lSize;
    long lStart=0;
    long lStatus=0;
    long iSize=0;
    char caTemp[1024];
    char caMd5[64],caMd5_s[64];
    char caUpfile_flag[12];
    char caSn[32];
    struct stat st;
    char caMtime[16];
    utMsgHead *psMsgHead2;
    memset(caMesg,0,sizeof(caMesg));
    memset(caMd5,0,sizeof(caMd5));
    memset(caMd5_s,0,sizeof(caMd5_s));
    strcpy(caServerIp,utComGetVar_sd(psShmHead,"upserverip",""));
    strcpy(caServerPort,utComGetVar_sd(psShmHead,"upserverport",""));
    strcpy(caSn,utComGetVar_sd(psShmHead,"ProductSn",""));
    
    
    
    iReturn = utMsgGetSomeNVar(psMsgHead,1,"upfile_flag",UT_TYPE_STRING, 8, caUpfile_flag);
    
    
    
    
    
    tPort=atol(caServerPort);
    lIp = utComHostAddress(caServerIp);
    nPort=ntohs(tPort);
    if(atol(caUpfile_flag)==1){
       sprintf(caFile,"/home/ncmysql/nc/bin/proeimpat.bin");
    }
    else if(atol(caUpfile_flag)==2){
       sprintf(caFile,"/home/ncmysql/nc/conf/url/ncurl.udb");
    }
    else if(atol(caUpfile_flag)==3){
       sprintf(caFile,"/home/ncmysql/nc/bin/ncpost.bin");
    }
    else if(atol(caUpfile_flag)==4){
       sprintf(caFile,"/home/ncmysql/nc/bin/nc");
    }
    else{
  	   sprintf(caFile,"/home/ncmysql/nc/bin/proeimpat.bin");
    }
     if (stat(caFile,&st) == 0) {
    	printf("stime=%d\n",st.st_mtime);
    }
    
   iReturn=utFsMd5(caFile,caMd5);
 //printf("iReturn=%d\n",iReturn);  
     psDbHead = utPltInitDb();

   utPltPutVar(psDbHead,"upfile_flag",caUpfile_flag);
   utPltPutVarF(psDbHead,"fsize","%d",iReturn);
   utComSetVar_l(psShmHead,"upserverstat",0);

     psMsgHead2 = pasTcpRequest(lIp,nPort,
                         0,        /* CheckSum */
                         0,        /* Encrypt  */
                         "ncsReqPatVersion",
                         0,       /* Sid */
                         0,       /* Rid  */
                         60,      /* 超时  */
                         NULL,    /* key   */
                         2,
                         "file_flag",UT_TYPE_STRING,caUpfile_flag,
                         "ProductSn",UT_TYPE_STRING,caSn);
                      
      if(psMsgHead2) {

           iReturn = utMsgGetSomeNVar(psMsgHead2,2,
                      "md5",UT_TYPE_STRING,32,caMd5_s,
                      "mtime",UT_TYPE_STRING,10,caMtime);		        					
            utMsgFree(psMsgHead2);  
 //     printf("caMtime=%s\n",caMtime);      
            if((strcmp(caMd5,caMd5_s)==0)){
            	strcpy(caMesg,"您的版本已经是最新了。");
            	utPltPutVar(psDbHead,"disable","disabled");
            }
            else{
            	strcpy(caMesg,"已经发现最新的版本，下载请按“继续升级”   ");
            }  
            utPltPutVar(psDbHead,"md5",caMd5_s);
       }

     utPltPutVar(psDbHead,"mesg",caMesg);
     utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_updateservice_first.htm");

  //  ncWebOnlineUpdateService(psShmHead,iFd,psMsgHead);

    return 0;
}



//显示网络服务升级状态
int ncWebShowUpServerStatAjx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int iReturn;
	long lUpServer;
	utPltDbHead *psDbHead;
	lUpServer=utComGetVar_ld(psShmHead,"upserverstat",0);
	

    psDbHead = utPltInitDb();
    utPltPutVarF(psDbHead,"upserver","%d",lUpServer);
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/upserver_ajx.htm");
    
    return 0;
}
 //显示设置手机认证界面 
  int ncMobileAuth_parset(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead){
        utPltDbHead *psDbHead;
        char *pConfig;
        pasConfList *psConfig;
       
        psDbHead = utPltInitDb();
       
        
        pConfig = pasGetConfigFileName();
		    psConfig = pasUtlReadConfig(pConfig);
		    if(psConfig == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0606网络配置"),ncLang("0427配置文件出错"));
		        return 0;
		    }
		    
		   utPltPutVar(psDbHead,"upserverip",   (char *)pasUtlLookConfig(psConfig,"upserverip",""));
       utPltPutVar(psDbHead,"upserverport",   (char *)pasUtlLookConfig(psConfig,"upserverport","7195"));
		   utPltPutVar(psDbHead,"upbkserverip",   (char *)pasUtlLookConfig(psConfig,"upbkserverip",""));
            
       utPltPutVar(psDbHead,"mobserverip",   (char *)pasUtlLookConfig(psConfig,"mobserverip","61.129.34.70"));
       utPltPutVar(psDbHead,"mobserverport",   (char *)pasUtlLookConfig(psConfig,"mobserverport","7195"));
		   utPltPutVar(psDbHead,"mobbkserverip",   (char *)pasUtlLookConfig(psConfig,"mobbkserverip","210.14.93.164"));    
             
        
        pasUtlFreeConfig(psConfig);
           
       		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/mobileauth_parset.htm");
      
         return 0;
}


//保存手机验证参数
int ncMobilePar_Save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caApp[20];
    char caMsg[256];
    int iReturn;
    char caDbname[32],caDbuser[32],caDbpass[32];
    char *pConfig;
    char *p,*pFile,*pNat,*pRoute;
    FILE *fp;
    pasConfList *psConfig;
   
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880自动备份设定"),ncLang("0427配置文件出错"));
        return 0;
    }
    
    if(p = utMsgGetVar_s(psMsgHead,"upserverip")) {
        pasUtlModiConfig(psConfig,"upserverip",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"upserverport")) {
        pasUtlModiConfig(psConfig,"upserverport",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"upbkserverip")) {
        pasUtlModiConfig(psConfig,"upbkserverip",p);
    }
  
  
   if(p = utMsgGetVar_s(psMsgHead,"mobserverip")) {
        pasUtlModiConfig(psConfig,"mobserverip",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"mobserverport")) {
        pasUtlModiConfig(psConfig,"mobserverport",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"mobbkserverip")) {
        pasUtlModiConfig(psConfig,"mobbkserverip",p);
    }
  
  
  
    pasUtlSaveConfig(pConfig,psConfig);
    pasUtlFreeConfig(psConfig);
   
   ncUtlReReadConFig(psShmHead);
   ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
      
    ncMobileAuth_parset(psShmHead,iFd,psMsgHead);

    return 0;
}

//本地手机短信猫验证
int ncAuthInLockMobile(utShmHead *psShmHead,char *pUsername,char *pPass)
{
   utPltDbHead *psDbHead;
    int iReturn;
    char caServerIp[20];
  
    char caPassword[32];
    char caPassword0[32];
    char caTemp[1024];
    char caKey[36];
    long lUserid,lCount;
   
     
     lCount=0;
      iReturn = pasDbOneRecord("select count(*) from ncmobuser where username = :name and password=:password ",
                   2,"name",UT_TYPE_STRING,pUsername,  
                     "password",UT_TYPE_STRING,pPass,               
                      UT_TYPE_LONG,4,&lCount);
            if(lCount >0) {
           
            	return 0;
            } 
            else{
            	return -1;
            }         
      }


//数据短信验证
int ncSysAuthMobile(utShmHead *psShmHead,char *pUsername,char *pPass)
{
   utPltDbHead *psDbHead;
    int iReturn;

    char caServerIp[20];
    char caServerPort[16];

    unsigned long lIp;
    long nPort,tPort;
  
    long lStatus=0;
    char caPassword[32];
    char caTemp[1024];
    char caName[32];
    utMsgHead *psMsgHead2;

    strcpy(caServerIp,utComGetVar_sd(psShmHead,"mobserverip",""));
    strcpy(caServerPort,utComGetVar_sd(psShmHead,"mobserverport",""));
    if(strlen(caServerIp)<2){

    strcpy(caServerIp,utComGetVar_sd(psShmHead,"upserverip",""));
    strcpy(caServerPort,utComGetVar_sd(psShmHead,"upserverport",""));
   }
    strcpy(caName,utComGetVar_sd(psShmHead,"ProductSn",""));
    tPort=atol(caServerPort);
    lIp = utComHostAddress(caServerIp);
    nPort=ntohs(tPort);
    psMsgHead2=NULL;
     psMsgHead2 = pasTcpRequest(lIp,nPort,
                         0,        /* CheckSum */
                         0,        /* Encrypt  */
                         "ncsReqAuthMobile",
                         0,       /* Sid */
                         0,       /* Rid  */
                         30,      /* 超时  */
                         NULL,    /* key   */
                         3,
                         "username",UT_TYPE_STRING,pUsername,
                         "password",UT_TYPE_STRING,pPass,
                         "ncname", UT_TYPE_STRING,caName);
                      
      if(psMsgHead2) {
           lStatus=-1;
           iReturn = utMsgGetSomeNVar(psMsgHead2,1,
                      "status",UT_TYPE_LONG,4,&lStatus);		        					
            utMsgFree(psMsgHead2);  
            if(lStatus==0){
							//同步帐号信息
							sprintf(caTemp,"select password from ncmobuser where username='%s' ",pUsername);
							memset(caPassword,0,sizeof(caPassword));
							iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caPassword);

							if(iReturn==0){
								if(strcmp(caPassword,pPass)!=0){

							      pasDbExecSqlF("update ncmobuser set password='%s' where username='%s' ",pPass,pUsername);
						     }
						    }
							
							if(iReturn==1403){
								pasDbExecSqlF("insert into ncmobuser (username,password,corp,linename,addr,tel) values('%s','%s','','','','') ",pUsername,pPass);
            } 
          } 
            return lStatus;
       }
       else{
 //      	printf("fffffff\n");
 //使用备份服务进行验证
         
          strcpy(caServerIp,utComGetVar_sd(psShmHead,"mobbkserverip",""));

			    if(strlen(caServerIp)<2){
			        strcpy(caServerIp,utComGetVar_sd(psShmHead,"upbkserverip",""));
			    }
          
          
          lIp = utComHostAddress(caServerIp);
		      psMsgHead2 = pasTcpRequest(lIp,nPort,
		                         0,        /* CheckSum */
		                         0,        /* Encrypt  */
		                         "ncsReqAuthMobile",
		                         0,       /* Sid */
		                         0,       /* Rid  */
		                         30,      /* 超时  */
		                         NULL,    /* key   */
		                         3,
		                         "username",UT_TYPE_STRING,pUsername,
		                         "password",UT_TYPE_STRING,pPass,
		                         "ncname", UT_TYPE_STRING,caName);
		       if(psMsgHead2) {
		       	
		           lStatus=-1;
		           iReturn = utMsgGetSomeNVar(psMsgHead2,1,
		                      "status",UT_TYPE_LONG,4,&lStatus);		        					
		            utMsgFree(psMsgHead2);  
		            if(lStatus==0){
									//同步帐号信息
									sprintf(caTemp,"select password from ncmobuser where username='%s' ",pUsername);
									memset(caPassword,0,sizeof(caPassword));
									iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caPassword);
		
									if(iReturn==0){
										if(strcmp(caPassword,pPass)!=0){
		
									      pasDbExecSqlF("update ncmobuser set password='%s' where username='%s' ",pPass,pUsername);
								     }
								    }
									
									if(iReturn==1403){
										pasDbExecSqlF("insert into ncmobuser (username,password,corp,linename,addr,tel) values('%s','%s','','','','') ",pUsername,pPass);
		            } 
		          } 
            	return lStatus;
       		}
 
 
 
      }
   
    return -1;
}


//修改短信密码
int ncModMobilePasswd(utShmHead *psShmHead,char *pUsername,char *pPass)
{
   utPltDbHead *psDbHead;
    int iReturn;

    char caServerIp[20];
    char caServerPort[16];

    unsigned long lIp;
    long nPort,tPort;
  
    long lStatus=0;
   
    char caTemp[1024];
    char caPassword[32];
    utMsgHead *psMsgHead2;

    strcpy(caServerIp,utComGetVar_sd(psShmHead,"mobserverip",""));
    strcpy(caServerPort,utComGetVar_sd(psShmHead,"mobserverport",""));
    if(strlen(caServerIp)<2){
		    strcpy(caServerIp,utComGetVar_sd(psShmHead,"upserverip",""));
		    strcpy(caServerPort,utComGetVar_sd(psShmHead,"upserverport",""));
   }

    tPort=atol(caServerPort);
    lIp = utComHostAddress(caServerIp);
    nPort=ntohs(tPort);
     
     psMsgHead2 = pasTcpRequest(lIp,nPort,
                         0,        /* CheckSum */
                         0,        /* Encrypt  */
                         "ncsReqModMobilePasswd",
                         0,       /* Sid */
                         0,       /* Rid  */
                         30,      /* 超时  */
                         NULL,    /* key   */
                         2,
                         "username",UT_TYPE_STRING,pUsername,
                         "password",UT_TYPE_STRING,pPass);
                      
      if(psMsgHead2) {
           lStatus=-1;
           iReturn = utMsgGetSomeNVar(psMsgHead2,1,
                      "status",UT_TYPE_LONG,4,&lStatus);		        					
            utMsgFree(psMsgHead2);  
            if(lStatus==0){
//同步本地帐号
							sprintf(caTemp,"select password from ncmobuser where username='%s' ",pUsername);
							memset(caPassword,0,sizeof(caPassword));
							iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caPassword);

							if(iReturn==0){
								if(strcmp(caPassword,pPass)!=0){

							      pasDbExecSqlF("update ncmobuser set password='%s' where username='%s' ",pPass,pUsername);
						     }
						    }
							
							if(iReturn==1403){
								pasDbExecSqlF("insert into ncmobuser (username,password,corp,linename,addr,tel) values('%s','%s','','','','') ",pUsername,pPass);
            } 
            }  
            return lStatus;
       }

   
    return -1;
}
//短信在本地验证 0 验证通过，-1 验证没有通过
int ncAuthMobileInLocal(char *pUsername,char *pPass)
{
	char caTemp[1024];
	long iReturn;
	char caPassword[32];
	long lStatus=-1;
	sprintf(caTemp,"select password from ncmobuser where username='%s' ",pUsername);
							memset(caPassword,0,sizeof(caPassword));
							iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caPassword);

							if(iReturn==0){
								if(strcmp(caPassword,pPass)==0){
									lStatus=0;
								}
							}
  return lStatus;
}




//自动升级

int ncUtlAutoUpdate(utShmHead *psShmHead)
{
    int iReturn;
    char caServerIp[20];
    char caServerPort[16];
    unsigned long lIp;
    long nPort,tPort;
    long lPasstime,lLastTime;
    char caSteptime[20],caSn[32];;
    long lSteptime=0;
    long lUpdate_pat,lUpdate_url,lUpdate_nc,lUpdate_webmail,lUpdate_bbs;
    lPasstime=0;
    iReturn = pasConnect(psShmHead);
 
    if(iReturn != 0) {
    	sleep(15);
 //       pasSysLogA(NCS_UPDATE_LOG,"[ncsStartAutoUpdate]Data Base Connect Error");
        return 0;
    }
    utComSetMaxTcpBuf(1024000L);
    
        strcpy(caSn,utComGetVar_sd(psShmHead,"ProductSn",""));
        strcpy(caServerIp,utComGetVar_sd(psShmHead,"upserverip",""));
		    strcpy(caServerPort,utComGetVar_sd(psShmHead,"upserverport",""));
		    strcpy(caSteptime,utComGetVar_sd(psShmHead,"upstep","24"));	
		    lSteptime=3600*atol(caSteptime);
		    tPort=atol(caServerPort);
		    lIp = utComHostAddress(caServerIp);
		    nPort=ntohs(tPort);
    
       lLastTime=ncGetLastInfo_l("UpdateCkTime",0);
       if(lLastTime==0){
       	lLastTime=time(0);
       	ncSetLastInfo_l("UpdateCkTime",time(0));
       }
    while(1) {
    	    
     	    lPasstime=time(0)-lLastTime;
     	    if(lPasstime>=lSteptime){
     	    	lLastTime=time(0);
     	    	 ncSetLastInfo_l("UpdateCkTime",time(0));
			        if((strcasecmp(utComGetVar_sd(psShmHead,"autoupdate","No"),"Yes")==0)&&(strlen(caServerIp)>0)){
			       
			          if(strcasecmp(utComGetVar_sd(psShmHead,"upserverflag","No"),"Yes")==0){
			          //升级协议库
			            ncUpdate_proeimpat(psShmHead,lIp,nPort,caSn);
			          
			          }
			         if(strcasecmp(utComGetVar_sd(psShmHead,"upurlflag","No"),"Yes")==0){
			          //升级网址库
			            ncUpdate_url(psShmHead,lIp,nPort,caSn);
			          
			          }
			          if(strcasecmp(utComGetVar_sd(psShmHead,"upbbsflag","No"),"Yes")==0){
			          //升级BBS库
			          ncUpdate_bbs(psShmHead,lIp,nPort,caSn);
			          }
			          if(strcasecmp(utComGetVar_sd(psShmHead,"upncflag","No"),"Yes")==0){
			          //升级NC程序
			          ncUpdate_nc(psShmHead,lIp,nPort,caSn);
			          }
			       
			       
			        }
             }
             else{                                   //处理界面上的升级指令
             	lUpdate_pat=utComGetVar_ld(psShmHead,"update_pat",0);
             	lUpdate_url=utComGetVar_ld(psShmHead,"update_url",0);
             	lUpdate_nc=utComGetVar_ld(psShmHead,"update_nc",0);
             	lUpdate_webmail=utComGetVar_ld(psShmHead,"update_webmail",0);
             	lUpdate_bbs=utComGetVar_ld(psShmHead,"update_bbs",0);
             	
              if(lUpdate_pat==1){                            //升级网络服务协议
              	ncUpdate_proeimpat(psShmHead,lIp,nPort,caSn);
              	utComSetVar_l(psShmHead,"update_pat",0);
              }
              if(lUpdate_url==1){             	               //升级URL
              	ncUpdate_url(psShmHead,lIp,nPort,caSn);
              	utComSetVar_l(psShmHead,"update_url",0);              	
              }
              if(lUpdate_bbs==1){             	               //升级BBS
              	ncUpdate_bbs(psShmHead,lIp,nPort,caSn);
              	utComSetVar_l(psShmHead,"update_bbs",0);              	
              }
              if(lUpdate_nc==1){             	               //升级nc
              	ncUpdate_nc(psShmHead,lIp,nPort,caSn);
              	utComSetVar_l(psShmHead,"update_nc",0);              	
              }
              
              
            }
     sleep(3);
    }
}

//升级网络服务库
int ncUpdate_proeimpat(utShmHead *psShmHead,unsigned long lIp,long nPort,char *pSn){
	long iReturn;
	char caMd5[64];
  FILE *fp;
  char *pBuf;
  char caFile[256];
  utMsgHead *psMsgHead2;
  long lSize,lTot,lStart,lValue;
	long lSize_z=0;
	char caFile_bin[256];
	char caFile_sql[256];
	char caTemp[1024];
	char caMd5_s[64];
	char caVersion[32];
	memset(caMd5,0,sizeof(caMd5));
	memset(caMd5_s,0,sizeof(caMd5_s));
  lStart=0;
	iReturn=ncQueryVersion(1,lIp,nPort,pSn,caMd5,&lSize_z,caVersion);
 if(lSize_z==0) lSize_z=1;



	if(iReturn==1){                                   //存在最新版本
		utComSetVar_l(psShmHead,"upserverstat",0);
		
		 sprintf(caFile,"/home/ncmysql/nc/log/pat_temp.zip");
     remove(caFile);
     fp = fopen(caFile,"a+");
		lSize=1;
    lTot=0;
    while(lSize>0){

     psMsgHead2 = pasTcpRequest(lIp,nPort,
                         0,        /* CheckSum */
                         0,        /* Encrypt  */
                         "ncsSendPatfile",
                          0,       /* Sid */
                          0,       /* Rid  */
                          30,      /* 超时  */
                          NULL,    /* key   */
                          2,
                         "start",UT_TYPE_LONG,lStart,
                         "flag", UT_TYPE_LONG,1);
		      if(psMsgHead2) {

           iReturn = utMsgGetSomeNVar(psMsgHead2,2,
                      "start",UT_TYPE_LONG,4,&lStart,
					            "filesize",  UT_TYPE_LONG,  4, &lSize);
			        lTot=lTot+lSize;  
	
			        lValue=400*lTot/lSize_z;
			      
			         utComSetVar_l(psShmHead,"upserverstat",lValue);

			          
					     pBuf = malloc(lSize+4);     
					     utMsgGetSomeNVar(psMsgHead2,1,
					            "data",UT_TYPE_STRUCT,lSize,pBuf);
					     lStart=lStart+lSize;   
//		  printf("lVar=%d,size=%d,start=%d\n",lValue,lSize,lStart);			     
					          					            
					  if(lSize>0)  {        
					     fwrite(pBuf,lSize,1,fp);
					    }
					
            utMsgFree(psMsgHead2);
            free(pBuf);           
         }

          usleep(100000);
       }
    fclose(fp);
		
		//检查文件的完整性
		
		strcpy(caFile_bin,"/home/ncmysql/nc/log/proeimpat.bin");
		strcpy(caFile_sql,"/home/ncmysql/nc/log/proeimpat.sql");
		remove(caFile_bin);
		remove(caFile_sql);
		sprintf(caTemp,"unzip -o /home/ncmysql/nc/log/pat_temp.zip -d /home/ncmysql/nc/log");
		system(caTemp);
 if(utFileIsExist(caFile_bin)){	

	 lSize=utFsMd5(caFile_bin,caMd5_s);
	 
	 if(strcmp(caMd5,caMd5_s)==0){

	 	sprintf(caTemp,"mv /home/ncmysql/nc/bin/proeimpat.bin /home/ncmysql/nc/bin/proeimpat.bin_1");
	 	system(caTemp);
	 	
	 	
	 	sprintf(caTemp,"cp /home/ncmysql/nc/log/proeimpat.bin /home/ncmysql/nc/bin/proeimpat.bin");
	 	system(caTemp);
	  
	  sprintf(caTemp,"mysql -unc -pnc -Dnc </home/ncmysql/nc/log/proeimpat.sql");
	  system(caTemp);	
 	
	 	pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,fsize,mesg) values(%d,'网络服务协议库','%s',%d,'发现新的特征库版本(%s)，升级成功')",time(0),caMd5_s,lSize,caVersion);
	 	return 1;
	 }
	 else{
	 		 	pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,mesg) values(%d,'网络服务协议库','%s','发现新的特征库，MD5校验错误，升级失败')",time(0),caMd5_s,lSize);
	 }
	}
}
	else{
			pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,mesg) values(%d,'网络服务协议库','%s','已经是最新的特征库了，不需升级')",time(0),caMd5);
	}
	return 0;
}
//检测版本 有最新版本返回1，2.没有最新版本 否则0  caMd5为远程的文件MD5
int ncQueryVersion(long flag,unsigned long lIp,long nPort,char *pSn,char *pMd5,long *lSize,char *pV){
	    struct stat st;
	    char caMd5[64],caMd5_s[64];
	    utMsgHead *psMsgHead2;
	    char caMtime[16];
	    long iReturn;
	    char caFile[256];
	    char caFsize[16];
	    char caFlag[16];
	    char caVersion[32];
	    sprintf(caFlag,"%d",flag);
	    memset(caMd5_s,0,sizeof(caMd5_s));
	    
	    if(flag==1){
       sprintf(caFile,"/home/ncmysql/nc/bin/proeimpat.bin");
    }
    else if(flag==2){
    	
       sprintf(caFile,"/home/ncmysql/nc/conf/url/ncurl.udb");
    }
    else if(flag==3){
    	
       sprintf(caFile,"/home/ncmysql/nc/bin/ncpost.bin");
    }
   else if(flag==4){
    	
       sprintf(caFile,"/home/ncmysql/nc/bin/nc");
    }
    else{
  	   sprintf(caFile,"/home/ncmysql/nc/bin/proeimpat.bin");
    }
     if (stat(caFile,&st) == 0) {
  //   	printf("stime=%d\n",st.st_mtime);
    }
    else{
//    	printf("aaaaa\n");
    	return 0;
    }
    
   iReturn=utFsMd5(caFile,caMd5);

	  psMsgHead2 = pasTcpRequest(lIp,nPort,
                         0,        /* CheckSum */
                         0,        /* Encrypt  */
                         "ncsReqPatVersion",
                         0,       /* Sid */
                         0,       /* Rid  */
                         60,      /* 超时  */
                         NULL,    /* key   */
                         2,
                         "file_flag",UT_TYPE_STRING,caFlag,
                         "ProductSn",UT_TYPE_STRING,pSn);
	      if(psMsgHead2) {

           iReturn = utMsgGetSomeNVar(psMsgHead2,4,
                      "md5",UT_TYPE_STRING,32,caMd5_s,
                      "mtime",UT_TYPE_STRING,10,caMtime,
                      "fsize",UT_TYPE_STRING,10,caFsize,
                      "version",UT_TYPE_STRING,16,caVersion);		        					
            utMsgFree(psMsgHead2);
            printf("caMd5=%s,caMd5_s=%s,caFile=%s\n",caMd5,caMd5_s,caFile);  
	         if((strcmp(caMd5,caMd5_s)==0)){
	         	return 2;
	        }
	        else{
	        	strcpy(pMd5,caMd5_s);
	        	*lSize=atol(caFsize);
	        	strcpy(pV,caVersion);
	        	return 1;
	        }
	  }
	  else{
	  	printf("fffff\n");
	  }
	  return 0;
	
	
}



//升级网址库

int ncUpdate_url(utShmHead *psShmHead,unsigned long lIp,long nPort,char *pSn){
	long iReturn;
	char caMd5[64];
  FILE *fp;
  char *pBuf;
  char caFile[256];
  utMsgHead *psMsgHead2;
  long lSize,lTot,lStart,lValue;
	long lSize_z=0;
	char caFile_bin[256];
	char caFile_sql[256];
	char caTemp[1024];
	char caMd5_s[64];
	char caVersion[32];
	memset(caMd5,0,sizeof(caMd5));
	memset(caMd5_s,0,sizeof(caMd5_s));
  lStart=0;
	iReturn=ncQueryVersion(2,lIp,nPort,pSn,caMd5,&lSize_z,caVersion);
 if(lSize_z==0) lSize_z=1;

printf("lSize_z=%d\n",lSize_z);


	if(iReturn==1){                                   //存在最新版本
		utComSetVar_l(psShmHead,"upserverstat",0);
		
		 sprintf(caFile,"/home/ncmysql/nc/log/url_temp.zip");
     remove(caFile);
     fp = fopen(caFile,"a");
		lSize=1;
    lTot=0;
    while(lSize>0){

     psMsgHead2 = pasTcpRequest(lIp,nPort,
                         0,        /* CheckSum */
                         0,        /* Encrypt  */
                         "ncsSendPatfile",
                          0,       /* Sid */
                          0,       /* Rid  */
                          60,      /* 超时  */
                          NULL,    /* key   */
                          2,
                         "start",UT_TYPE_LONG,lStart,
                         "flag", UT_TYPE_LONG,2);
		      if(psMsgHead2) {

           iReturn = utMsgGetSomeNVar(psMsgHead2,2,
                      "start",UT_TYPE_LONG,4,&lStart,
					            "filesize",  UT_TYPE_LONG,  4, &lSize);
			        lTot=lTot+lSize;  
	//	        printf("lTot=%d,lFsize=%d\n",lTot,lSize);
			        lValue=(float)lTot*400/lSize_z;
//			        printf("lVar=%d\n",lValue);
			         utComSetVar_l(psShmHead,"upserverstat",lValue);

			          
					     pBuf = malloc(lSize+4);     
					     utMsgGetSomeNVar(psMsgHead2,1,
					            "data",UT_TYPE_STRUCT,lSize,pBuf);
					     lStart=lStart+lSize;        					            
					  if(lSize>0)  {        
					     fwrite(pBuf,lSize,1,fp);
					    }
					
            utMsgFree(psMsgHead2);
            free(pBuf);           
         }

          usleep(100000);
       }
    fclose(fp);
		
		//检查文件的完整性
		
		strcpy(caFile_bin,"/home/ncmysql/nc/log/ncurl.udb");
	
		remove(caFile_bin);
	
		sprintf(caTemp,"unzip -o /home/ncmysql/nc/log/url_temp.zip -d /home/ncmysql/nc/log");
		system(caTemp);
 if(utFileIsExist(caFile_bin)){	

	 lSize=utFsMd5(caFile_bin,caMd5_s);
	 
	 if(strcmp(caMd5,caMd5_s)==0){

	 	sprintf(caTemp,"mv /home/ncmysql/nc/conf/url/ncurl.udb /home/ncmysql/nc/conf/url/ncurl.udb_1");
	 	system(caTemp);
	 	
	 	
	 	sprintf(caTemp,"cp /home/ncmysql/nc/log/ncurl.udb /home/ncmysql/nc/conf/url/ncurl.udb");
	 	system(caTemp);
	  
	  
	 	
	 	pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,fsize,mesg) values(%d,'网址库','%s',%d,'发现新的网址库，版本(%s)，升级成功')",time(0),caMd5_s,lSize,caVersion);
	 	return 1;
	 }
	 else{
	 		 	pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,mesg) values(%d,'网址库','%s','发现新的网址库，MD5校验错误，升级失败')",time(0),caMd5_s,lSize);
	 }
	}
}
	else{
			pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,mesg) values(%d,'网址库','%s','已经是最新的特征库了，不需升级')",time(0),caMd5);
	}
	return 0;
}




//升级BBS库
int ncUpdate_bbs(utShmHead *psShmHead,unsigned long lIp,long nPort,char *pSn){
	long iReturn;
	char caMd5[64];
  FILE *fp;
  char *pBuf;
  char caFile[256];
  utMsgHead *psMsgHead2;
  long lSize,lTot,lStart,lValue;
	long lSize_z=0;
	char caFile_bin[256];
	char caFile_sql[256];
	char caTemp[1024];
	char caMd5_s[64];
	char caVersion[32];
	lStart=0;
	memset(caMd5,0,sizeof(caMd5));
	memset(caMd5_s,0,sizeof(caMd5_s));

	iReturn=ncQueryVersion(3,lIp,nPort,pSn,caMd5,&lSize_z,caVersion);
 if(lSize_z==0) lSize_z=1;



	if(iReturn==1){                                   //存在最新版本
		utComSetVar_l(psShmHead,"upserverstat",0);
		
		 sprintf(caFile,"/home/ncmysql/nc/log/bbs_temp.zip");
     remove(caFile);
     fp = fopen(caFile,"a");
		lSize=1;
    lTot=0;
    while(lSize>0){

     psMsgHead2 = pasTcpRequest(lIp,nPort,
                         0,        /* CheckSum */
                         0,        /* Encrypt  */
                         "ncsSendPatfile",
                          0,       /* Sid */
                          0,       /* Rid  */
                          30,      /* 超时  */
                          NULL,    /* key   */
                          2,
                         "start",UT_TYPE_LONG,lStart,
                         "flag", UT_TYPE_LONG,3);
		      if(psMsgHead2) {

           iReturn = utMsgGetSomeNVar(psMsgHead2,2,
                      "start",UT_TYPE_LONG,4,&lStart,
					            "filesize",  UT_TYPE_LONG,  4, &lSize);
			        lTot=lTot+lSize;  
	//		        printf("lTot=%d,lFsize=%d\n",lTot,lFsize);
			        lValue=400*lTot/lSize_z;
//			        printf("lVar=%d\n",lValue);
			         utComSetVar_l(psShmHead,"upserverstat",lValue);

			          
					     pBuf = malloc(lSize+4);     
					     utMsgGetSomeNVar(psMsgHead2,1,
					            "data",UT_TYPE_STRUCT,lSize,pBuf);
					     lStart=lStart+lSize;        					            
					  if(lSize>0)  {        
					     fwrite(pBuf,lSize,1,fp);
					    }
					
            utMsgFree(psMsgHead2);
            free(pBuf);           
         }

          usleep(100000);
       }
    fclose(fp);
		
		//检查文件的完整性
		
		strcpy(caFile_bin,"/home/ncmysql/nc/log/ncpost.bin");	
		remove(caFile_bin);

		sprintf(caTemp,"unzip -o /home/ncmysql/nc/log/bbs_temp.zip -d /home/ncmysql/nc/log");
		system(caTemp);
 if(utFileIsExist(caFile_bin)){	

	 lSize=utFsMd5(caFile_bin,caMd5_s);
	 
	 if(strcmp(caMd5,caMd5_s)==0){

	 	sprintf(caTemp,"mv /home/ncmysql/nc/bin/ncpost.bin /home/ncmysql/nc/bin/ncpost.bin_1");
	 	system(caTemp);
	 	
	 	
	 	sprintf(caTemp,"cp /home/ncmysql/nc/log/ncpost.bin /home/ncmysql/nc/bin/ncpost.bin");
	 	system(caTemp);
	  
	 	
	 	pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,fsize,mesg) values(%d,'BBS协议库','%s',%d,'发现新的特征库(版本%s)，升级成功')",time(0),caMd5_s,lSize,caVersion);
	 	return 1;
	 }
	 else{
	 		 	pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,mesg) values(%d,'BBS协议库','%s','发现新的特征库，MD5校验错误，升级失败')",time(0),caMd5_s,lSize);
	 }
	}
}
	else{
			pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,mesg) values(%d,'BBS协议库','%s','已经是最新的特征库了，不需升级')",time(0),caMd5);
	}
	return 0;
}



//升级nc
int ncUpdate_nc(utShmHead *psShmHead,unsigned long lIp,long nPort,char *pSn){
	long iReturn;
	char caMd5[64];
  FILE *fp;
  char *pBuf;
  char caFile[256];
  utMsgHead *psMsgHead2;
  long lSize,lTot,lStart,lValue;
	long lSize_z=0;
	char caFile_bin[256];
	char caFile_sql[256];
	char caTemp[1024];
	char caMd5_s[64];
	char caVersion[32];
	lStart=0;
	memset(caMd5,0,sizeof(caMd5));
	memset(caMd5_s,0,sizeof(caMd5_s));

	iReturn=ncQueryVersion(4,lIp,nPort,pSn,caMd5,&lSize_z,caVersion);
 if(lSize_z==0) lSize_z=1;



	if(iReturn==1){                                   //存在最新版本
		utComSetVar_l(psShmHead,"upserverstat",0);
		
		 sprintf(caFile,"/home/ncmysql/nc/log/nc_temp.zip");
     remove(caFile);
     fp = fopen(caFile,"a");
		lSize=1;
    lTot=0;
    while(lSize>0){

     psMsgHead2 = pasTcpRequest(lIp,nPort,
                         0,        /* CheckSum */
                         0,        /* Encrypt  */
                         "ncsSendPatfile",
                          0,       /* Sid */
                          0,       /* Rid  */
                          30,      /* 超时  */
                          NULL,    /* key   */
                          2,
                         "start",UT_TYPE_LONG,lStart,
                         "flag", UT_TYPE_LONG,4);
		      if(psMsgHead2) {

           iReturn = utMsgGetSomeNVar(psMsgHead2,2,
                      "start",UT_TYPE_LONG,4,&lStart,
					            "filesize",  UT_TYPE_LONG,  4, &lSize);
			        lTot=lTot+lSize;  
	//		        printf("lTot=%d,lFsize=%d\n",lTot,lFsize);
			        lValue=400*lTot/lSize_z;
//			        printf("lVar=%d\n",lValue);
			         utComSetVar_l(psShmHead,"upserverstat",lValue);

			          
					     pBuf = malloc(lSize+4);     
					     utMsgGetSomeNVar(psMsgHead2,1,
					            "data",UT_TYPE_STRUCT,lSize,pBuf);
					     lStart=lStart+lSize;        					            
					  if(lSize>0)  {        
					     fwrite(pBuf,lSize,1,fp);
					    }
					
            utMsgFree(psMsgHead2);
            free(pBuf);           
         }

          usleep(100000);
       }
    fclose(fp);
		
		//检查文件的完整性
		
		strcpy(caFile_bin,"/home/ncmysql/nc/log/nc");
		strcpy(caFile_sql,"/home/ncmysql/nc/log/ncc.str");
		remove(caFile_bin);
		remove(caFile_sql);
		sprintf(caTemp,"unzip -o /home/ncmysql/nc/log/nc_temp.zip -d /home/ncmysql/nc/log");
		system(caTemp);
 if(utFileIsExist(caFile_bin)){	

	 lSize=utFsMd5(caFile_bin,caMd5_s);
	 
	 if(strcmp(caMd5,caMd5_s)==0){

	 	sprintf(caTemp,"mv /home/ncmysql/nc/bin/nc /home/ncmysql/nc/bin/nc_1");
	 	system(caTemp);
	 	
	 	
	 	sprintf(caTemp,"cp /home/ncmysql/nc/log/nc /home/ncmysql/nc/bin/nc");
	 	system(caTemp);
	  
	  sprintf(caTemp,"/home/ncmysql/nc/bin/ncupdatedbstr -u nc -p nc -d nc -f /home/ncmysql/nc/log/ncc.str");
	  system(caTemp);	
	 	
	 	pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,fsize,mesg) values(%d,'软件内核','%s',%d,'发现新的版本,版本(%s)，升级成功')",time(0),caMd5_s,lSize,caVersion);
	 	return 1;
	 }
	 else{
	 		 	pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,mesg) values(%d,'软件内核','%s','发现新的版本，MD5校验错误，升级失败')",time(0),caMd5_s,lSize);
	 }
	}
}
	else{
			pasDbExecSqlF("insert into ncupdatelog(stime,type,md5,mesg) values(%d,'软件内核','%s','已经是最新的版本了，不需升级')",time(0),caMd5);
	}
	return 0;
}


/* 显示系统升级日志*/
int ncShowUpdateLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;

    char sqlbuf[2048];

    int iReturn,iNum;

    char caType[32],caMd5[32],caMesg[252];
    long lSid,lStime,lFsize;
    pasDbCursor *psCur;   

   char caTotPg[16],caCurPg[16];
   long lRowNum,lStartRec,lCurPg,lCount,lTotPg;
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
					 "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					 "CurPg",      UT_TYPE_STRING, 12, caCurPg);



   lRowNum = 32;
 
  lCurPg=atol(caCurPg);
  if(lCurPg<1) lCurPg=1;
  
 lStartRec=(lCurPg-1)*lRowNum;
 
 sprintf(sqlbuf,"select count(*) from  ncupdatelog where 1=1 ");

 
  
  pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lCount);




  sprintf(sqlbuf,"select sid,stime,type,md5,fsize,mesg from ncupdatelog where 1=1 ");
  
 
  
  sprintf(sqlbuf+strlen(sqlbuf)," order by sid desc ");
  sprintf(sqlbuf+strlen(sqlbuf)," limit %d,%d ",lStartRec,lRowNum);
  
 printf("sqlbuf=%s\n",sqlbuf);
        
  psCur=pasDbOpenSql(sqlbuf,0);
                  
  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0582统计"),ncLang("0549数据库出错"));
        return 0;
   }

    psDbHead = utPltInitDb();
       
   


    iReturn = 0;
    iNum = 0; 
  
    lSid=0;
    lStime=0;
    lFsize=0;
    memset(caType,0,sizeof(caType));
    memset(caMd5,0,sizeof(caMd5));
    memset(caMesg,0,sizeof(caMesg));
                         
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4, &lSid,
                                   UT_TYPE_LONG,4,&lStime,
                                   UT_TYPE_STRING,30,caType,
                                   UT_TYPE_STRING,30,caMd5,
                                   UT_TYPE_LONG,4,&lFsize,
                                   UT_TYPE_STRING,200,caMesg);
                                   
    															
    															


    



    
     while(iReturn==0){
     
		  iNum++;
		    		    
		  		utPltPutLoopVar(psDbHead,"type",iNum,caType);
		  		utPltPutLoopVar(psDbHead,"md5",iNum,caMd5);
		  		utPltPutLoopVar(psDbHead,"mesg",iNum,caMesg);
		  		utPltPutLoopVarF(psDbHead,"fsize",iNum,"%d",lFsize);
		  		
		  	  utPltPutLoopVar(psDbHead,"stime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
		  
    lSid=0;
    lStime=0;
    lFsize=0;
    memset(caType,0,sizeof(caType));
    memset(caMd5,0,sizeof(caMd5));
    memset(caMesg,0,sizeof(caMesg));
                         
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4, &lSid,
                                   UT_TYPE_LONG,4,&lStime,
                                   UT_TYPE_STRING,30,caType,
                                   UT_TYPE_STRING,30,caMd5,
                                   UT_TYPE_LONG,4,&lFsize,
                                   UT_TYPE_STRING,200,caMesg);
		  
		}
		pasDbCloseCursor(psCur);
		
	     
      
     lTotPg = (lCount - 1) / lRowNum + 1;
     utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
     utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
     utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
     
     
     
       
     utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncupdatelog.htm");
  
    return 0;
}




//自动同步域用户

int ncUtlAutoSysLdap(utShmHead *psShmHead)
{
    int iReturn;

    long lPasstime,lLastTime;
    char caSteptime[20],caSn[32];
    long lSteptime=0;
  
    lPasstime=0;
    iReturn = pasConnect(psShmHead);
 
    if(iReturn != 0) {
    	sleep(15);
 //       pasSysLogA(NCS_UPDATE_LOG,"[ncsStartAutoUpdate]Data Base Connect Error");
        return 0;
    }
   
    

    while(1) {
    	if((strcasecmp(utComGetVar_sd(psShmHead,"StartSysLdap","No"),"Yes")==0)){
	    	 lLastTime=ncGetLastInfo_l("SysLdapTime",0);
	    	  strcpy(caSteptime,utComGetVar_sd(psShmHead,"SysStep","24"));	
			    lSteptime=3600*atol(caSteptime);	 
	    	 
	       if(lLastTime==0){
	       	lLastTime=time(0);
	       	ncSetLastInfo_l("SysLdapTime",time(0));
	       }
	     	    lPasstime=time(0)-lLastTime;
	     	    if(lPasstime>=lSteptime){
	     	    	lLastTime=time(0);
	     	    	
	     	    	ncUtlSynDomain(psShmHead);
	     	    	
	     	    	 ncSetLastInfo_l("SysLdapTime",time(0));
			       
			        }
			     }
     sleep(1800);
   }
  }
  
  
  
  
  
  /*同步域用户*/
int ncUtlSynDomain(utShmHead *psShmHead)
{

}

  
  
 /* 系统访问设置  */
int ncWebSystemSet_fwset(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    pasConfList *psConfig;
    char        *pConfig;
    int iReturn;
    char        caUrltime[8],caAutourl[8],caUpdateserver[32];
    char        caUpnow[8],caRedown[8],caApp[8],caSave[8];
    unsigned long lUrlLastTime,l1;
    char caSoftver[32],caDbver[32],caUpdate[32];
    char caMesg2[128];
    char caCtime[24];
    long iSumRecord;
    utPltDbHead *psDbHead;
    long lGroupid0=0;
      
      
 
    
     pConfig = (char *)pasGetConfigFileName();
    psConfig = (pasConfList *)pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0623网址库在线升级"),ncLang("0427配置文件出错"));
        return 0;
    }
    
    
    
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

 
    psDbHead = utPltInitDb();

   
    
    
    if(strcasecmp(pasUtlLookConfig(psConfig,"DOSFW","No"),"Yes")==0) {
        utPltPutVar(psDbHead,"DOSFW","checked");
    } 
    
    if(strcasecmp(pasUtlLookConfig(psConfig,"IPSPFW","No"),"Yes")==0) {
        utPltPutVar(psDbHead,"IPSPFW","checked");
    } 
    
    if(strcasecmp(pasUtlLookConfig(psConfig,"TCPFW","No"),"Yes")==0) {
        utPltPutVar(psDbHead,"TCPFW","checked");
    } 
    if(strcasecmp(pasUtlLookConfig(psConfig,"MMFW","No"),"Yes")==0) {
        utPltPutVar(psDbHead,"MMFW","checked");
    } 
      
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_fwsz.htm");
    return 0;
}


   //保存防护参数
int ncWebFwSave_par(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caApp[20];
    char caMsg[256];
    int iReturn;
    char caDbname[32],caDbuser[32],caDbpass[32];
    char *pConfig;
    char *p,*pFile,*pNat,*pRoute;
    FILE *fp;
    pasConfList *psConfig;
    if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebSystemSave_par");
 	}
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880自动备份设定"),ncLang("0427配置文件出错"));
        return 0;
    }
    
    if(p = utMsgGetVar_s(psMsgHead,"DOSFW")) {
        pasUtlModiConfig(psConfig,"DOSFW",p);
    }
    else{
    	 pasUtlDelConfig(psConfig,"DOSFW");
    }
    if(p = utMsgGetVar_s(psMsgHead,"IPSPFW")) {
        pasUtlModiConfig(psConfig,"IPSPFW",p);
    }
    else{
    	 pasUtlDelConfig(psConfig,"IPSPFW");
    }
    if(p = utMsgGetVar_s(psMsgHead,"TCPFW")) {
        pasUtlModiConfig(psConfig,"TCPFW",p);
    }
    else{
    	 pasUtlDelConfig(psConfig,"TCPFW");
    }
    
    if(p = utMsgGetVar_s(psMsgHead,"MMFW")) {
        pasUtlModiConfig(psConfig,"MMFW",p);
    }
    else{
    	 pasUtlDelConfig(psConfig,"MMFW");
    }
    pasUtlSaveConfig(pConfig,psConfig);
    pasUtlFreeConfig(psConfig);
   
    ncUtlReReadConFig(psShmHead);
    ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
       
    	
    ncWebSystemSet_fwset(psShmHead,iFd,psMsgHead);

    return 0;
}



 /* 通过网管协议查看设置  */
int ncWebSendBySnmp_set(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    pasConfList *psConfig;
    char        *pConfig;
    int iReturn;
    char        caUrltime[8],caAutourl[8],caUpdateserver[32];
    char        caUpnow[8],caRedown[8],caApp[8],caSave[8];
    unsigned long lUrlLastTime,l1;
    char caSoftver[32],caDbver[32],caUpdate[32];
    char caMesg2[128];
    char caCtime[24];
    long iSumRecord;
    utPltDbHead *psDbHead;
    long lGroupid0=0;
      
      
 
    
     pConfig = (char *)pasGetConfigFileName();
    psConfig = (pasConfList *)pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0623网址库在线升级"),ncLang("0427配置文件出错"));
        return 0;
    }
    
    
    
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

 
    psDbHead = utPltInitDb();

   
     utPltPutVar(psDbHead,"community",pasUtlLookConfig(psConfig,"community",""));
     utPltPutVar(psDbHead,"access",pasUtlLookConfig(psConfig,"access","rw"));
     utPltPutVar(psDbHead,"snmpip",pasUtlLookConfig(psConfig,"snmpip","any"));
   
   FILE *fp;
   char *p;
   char caBuffer[1024];
   long lFlag=0;
     fp = popen("netstat -nap |more ","r");
    if(fp) {
        p = fgets(caBuffer,1000,fp);
  
        while(p) {
          
 //        printf("caBuffer=%s\n",caBuffer);
         if(strstr(caBuffer,"linuxagent")){
         	lFlag=1;
         	  break;
          }
            p = fgets(caBuffer,1000,fp);
        }
        pclose(fp);
       }
   
      if(lFlag==1){
      	utPltPutVar(psDbHead,"disstart","disabled");
      	utPltPutVar(psDbHead,"state","<font color=red>已启动</font>");
      }
      else{
      	utPltPutVar(psDbHead,"disstop","disabled");
      	utPltPutVar(psDbHead,"state","<font color=green>已停止</font>");
      }
      
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_sendbysnmp_par.htm");
    return 0;
}
int ncGenSnmpConfig(utShmHead *psShmHead)
{
	FILE *fp;
	char caPath[256],caPath1[256];
	char caTemp[512];
	 sprintf(caPath,"%s/AdventNet/LinuxAgent/conf/snmp/authtable.txt",utComGetVar_sd(psShmHead,"SnmpDir","/home/ncmysql/nc/snmp"));
	 sprintf(caPath1,"%s/AdventNet/LinuxAgent/conf/snmp/authtable.txt_bak",utComGetVar_sd(psShmHead,"SnmpDir","/home/ncmysql/nc/snmp"));
	
	sprintf(caTemp,"rm %s",caPath1);
	system(caTemp);
	
	sprintf(caTemp,"move %s %s",caPath,caPath1);
	system(caTemp);
	
	fp = fopen(caPath,"w");
  if(fp == NULL) {
     
      return -1;
  }
  sprintf(caTemp,"%s",utComGetVar_sd(psShmHead,"community",""));
	if(strcasecmp(utComGetVar_sd(psShmHead,"access","ro"),"ro")==0){
		sprintf(caTemp+strlen(caTemp)," READ_ONLY");
	}
	else{
		sprintf(caTemp+strlen(caTemp)," READ_WRITE");
	}
	if(strcasecmp(utComGetVar_sd(psShmHead,"snmpip","any"),"any")==0){
		sprintf(caTemp+strlen(caTemp)," 0.0.0.0");
	}
	else{
		sprintf(caTemp+strlen(caTemp)," %s",utComGetVar_sd(psShmHead,"snmpip","0.0.0.0"));
	}
	sprintf(caTemp+strlen(caTemp)," ACTIVE\n");
	fprintf(fp,"%s",caTemp);
	fclose(fp);
	
	
	return 0;
}

   //保存SNMP参数
int ncWebSendBySnmp_setSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caApp[20];
    char caMsg[256];
    int iReturn;
    char caTemp[512];
    char caDbname[32],caDbuser[32],caDbpass[32];
    char *pConfig;
    char caRestart[16],caStop[16];
    char *p,*pFile,*pNat,*pRoute;
    FILE *fp;
    pasConfList *psConfig;
    
    iReturn = utMsgGetSomeNVar(psMsgHead,3,"app",   UT_TYPE_STRING, 15,caApp,
                                           "restart",UT_TYPE_STRING,15,caRestart,
                                           "stop",UT_TYPE_STRING,15,caStop);
    
    
    
    if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebSystemSave_par");
 	}
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880自动备份设定"),ncLang("0427配置文件出错"));
        return 0;
    }
    
    if(p = utMsgGetVar_s(psMsgHead,"community")) {
        pasUtlModiConfig(psConfig,"community",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"access")) {
        pasUtlModiConfig(psConfig,"access",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"snmpip")) {
        pasUtlModiConfig(psConfig,"snmpip",p);
    }

    pasUtlSaveConfig(pConfig,psConfig);
    pasUtlFreeConfig(psConfig);
   
    ncUtlReReadConFig(psShmHead);
    ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
       
    if(!utStrIsSpaces(caApp)){
     	iReturn=ncGenSnmpConfig(psShmHead);
     	if(iReturn!=0){
     		utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880自动备份设定"),ncLang("0427配置文件出错"));
        return 0;
     	}
    } 
    if(!utStrIsSpaces(caRestart)){
    	 sprintf(caTemp,"%s/AdventNet/LinuxAgent/bin",utComGetVar_sd(psShmHead,"SnmpDir","/home/ncmysql/nc/snmp"));
    	chdir(caTemp);
    	
    	 sprintf(caTemp,"sh ./StopAgent.sh");
    	 system(caTemp);
    	
    	sprintf(caTemp,"sh ./StartAgent.sh");
  //  	printf("caTemp=%s\n",caTemp);
    	 system(caTemp);
  
     sleep(2);
    }   
    if(!utStrIsSpaces(caStop)){
    	sprintf(caTemp,"%s/AdventNet/LinuxAgent/bin",utComGetVar_sd(psShmHead,"SnmpDir","/home/ncmysql/nc/snmp"));
    	chdir(caTemp);
    	
    	sprintf(caTemp,"sh ./StopAgent.sh");
 //   	    	printf("caTemp=%s\n",caTemp);
    	 system(caTemp);
    	
    	 sleep(2);
    }  
    	
    ncWebSendBySnmp_set(psShmHead,iFd,psMsgHead);

    return 0; 
}

//同步农商行用户信息

int ncsSysNshUser(utShmHead *psShmHead)
{
 
  }
  
  
  
//获取短信验证密码
int ncWebGetMobPass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   utPltDbHead *psDbHead;
    int iReturn;

    char caServerIp[20];
    char caServerPort[16];

    unsigned long lIp;
    long nPort,tPort;
  
    long lStatus=0;
   
    char caTemp[1024];
    char caPassword[32];
    utMsgHead *psMsgHead2;
    char caNo[24];
    strcpy(caServerIp,utComGetVar_sd(psShmHead,"mobserverip",""));
    strcpy(caServerPort,utComGetVar_sd(psShmHead,"mobserverport",""));
    if(strlen(caServerIp)<2){
		    strcpy(caServerIp,utComGetVar_sd(psShmHead,"upserverip",""));
		    strcpy(caServerPort,utComGetVar_sd(psShmHead,"upserverport",""));
   }

    tPort=atol(caServerPort);
    lIp = utComHostAddress(caServerIp);
    nPort=ntohs(tPort);
     
     iReturn = utMsgGetSomeNVar(psMsgHead,1,"mobno",UT_TYPE_STRING,20,caNo);	
     
 printf("caNo=%s\n",caNo);    
     psDbHead = utPltInitDb();
     psMsgHead2 = pasTcpRequest(lIp,nPort,
                         0,        /* CheckSum */
                         0,        /* Encrypt  */
                         "ncsReqSendMobilePasswd",
                         0,       /* Sid */
                         0,       /* Rid  */
                         30,      /* 超时  */
                         NULL,    /* key   */
                         1,
                         "mobno",UT_TYPE_STRING,caNo);
                         
                      
      if(psMsgHead2) {
           lStatus=-1;
           iReturn = utMsgGetSomeNVar(psMsgHead2,1,
                      "status",UT_TYPE_LONG,4,&lStatus);	
         printf("lStatus=%d\n",lStatus);	        					
            utMsgFree(psMsgHead2);  
            if(lStatus==0){
                 utPltPutVar(psDbHead,"tmsg","短信已经成功发出");
						
              }
              else{
              	utPltPutVar(psDbHead,"tmsg","短信发送失败");
              }
      }

   utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/smp_mesg.htm");
    return 0;
}


//通过畅游平台获取短信验证密码
int ncWebGetMobPass_Cy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   utPltDbHead *psDbHead;
    int iReturn;

    char caServerIp[20];
    char caServerPort[16];

    unsigned long lIp;
    long nPort,tPort;
  
    long lStatus=0;
   
    char caTemp[1024];
    char caPassword[32];
    utMsgHead *psMsgHead2;
    ncSmsuser  *psHashData;
    char caNo[24];
    if(strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif",""),"GSMModen")==0){
    	memset(caNo,0,sizeof(caNo));
    	iReturn = utMsgGetSomeNVar(psMsgHead,1,"mobno",UT_TYPE_STRING,19,caNo);	
    		  char caMd5[64],caPass[32];
          unsigned long lMd5;
          unsigned long lPs;
          sprintf(caMd5,"%d",time(0));
         lMd5=utMd5Code(caMd5,strlen(caMd5),"ffff");
         lPs=lMd5%1000000;
	  		 sprintf(caPass,"%lu",lPs);
	  	printf("caPass=%s\n",caPass);	 
    	   psHashData=(ncSmsuser*)utShmHashLookA(psShmHead,NC_LNK_SMSMOBNO,caNo);
    	   if(psHashData){
    	printf("caNo=%s\n",caNo);
    	   	strcpy(psHashData->caPass,caPass);
    	  } 
    	  psDbHead = utPltInitDb();
    	  utPltPutVar(psDbHead,"tmsg","短信已经提交");
    	
    	
    	
    	return 0;
    }
    
    
    strcpy(caServerIp,utComGetVar_sd(psShmHead,"mobserverip",""));
    strcpy(caServerPort,utComGetVar_sd(psShmHead,"mobserverport",""));
    if(strlen(caServerIp)<2){
		    strcpy(caServerIp,utComGetVar_sd(psShmHead,"upserverip",""));
		    strcpy(caServerPort,utComGetVar_sd(psShmHead,"upserverport",""));
   }

    tPort=atol(caServerPort);
    lIp = utComHostAddress(caServerIp);
    nPort=ntohs(tPort);
     
     iReturn = utMsgGetSomeNVar(psMsgHead,1,"mobno",UT_TYPE_STRING,20,caNo);	
     
 printf("caNo=%s\n",caNo);    
     psDbHead = utPltInitDb();

     psMsgHead2 = pasTcpRequest(lIp,nPort,
                         0,       
                         0,        
                         "ncsSendMobilePasswd_cy",
                         0,       
                         0,       
                         30,      
                         NULL,    
                         1,
                         "mobno",UT_TYPE_STRING,caNo);
                         
                      
      if(psMsgHead2) {
           lStatus=-1;
           iReturn = utMsgGetSomeNVar(psMsgHead2,1,
                      "status",UT_TYPE_LONG,4,&lStatus);	
         printf("lStatus=%d\n",lStatus);	        					
            utMsgFree(psMsgHead2);  
            if(lStatus==0){
                 utPltPutVar(psDbHead,"tmsg","短信已经成功发出");
						
              }
              else{
              	utPltPutVar(psDbHead,"tmsg","短信发送失败");
              }
      }



//ncUtlSms_CTY("ctyswse-24","8bb2a3",caNo,"this is测试");



   utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/smp_mesg.htm");
    return 0;
}