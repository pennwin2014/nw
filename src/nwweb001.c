/* 内网管理  */
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
char *nwUtlGetMflags(int iFlags);
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "ncdef.h"
#include "nwdef.h"
char *nwGetCompName(long lGtype,long lId);

/* 当前屏幕信息  */
int nwTcpScreenInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    nwCompIp       *psCompIp;
    FILE *fp;
    char caFile[128];
    char caPath[128];
    char *p;
    int iNum,iReturn,i,k;
    unsigned long lCompId,lSum,lBytes,lSid,lIp,lSid0;
    unsigned short nPort;
#ifdef LDEBUG
    printf(" nwTcpScreenInfo  \n");
 //   utMsgPrintMsg(psMsgHead);
#endif
    /* 数据报校验  */
    lCompId = psMsgHead->lReserve[0];
    utComGetSockIp(iFd,&lIp,&nPort);
    lIp = ntohl(lIp);
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&lIp);
    if(psCompIp == NULL || psCompIp->psComp == NULL) {
#ifdef LDEBUG
        printf(" Not Online ....\n");
 //       utMsgPrintMsg(psMsgHead);
#endif
        return 0;
    }
    if(psCompIp->compid != lCompId) {
#ifdef LDEBUG
        printf(" CompId not equal ....\n");
#endif
        return 0;
    }
    if(!utMsgCheckSum(psMsgHead,psCompIp->caKey)) { /* 数据不合法  */
#ifdef LDEBUG
        printf(" It is invalid message, CheckSum Error\n");
#endif        
        return 0;
    }
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
   	                "sid",        UT_TYPE_LONG,4, &lSid,
   	                "sumbytes",   UT_TYPE_LONG,4, &lSum);
    if(lSum > 0) {
    	   sprintf(caPath,"%s/tmp/screen",utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog"));
         if(!utFileIsExist(caPath)){
         	mkdir(caPath,555);
        }
        sprintf(caFile,"%s/tmp/screen/%lu.jpg",utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog"),
               lSid);
#ifdef LDEBUG
       printf(" Write:%s \n",caFile);
#endif       
       lSid0 = 0;
        p =utMsgGetVar_v(psMsgHead,"content",&lBytes);
        if(p) {
            fp = fopen(caFile,"w");
            if(fp) {
                fwrite(p,lSum,1,fp);
                fclose(fp);
                lSid0 = psCompIp->sDevHead.lScrId;    
                psCompIp->sDevHead.lScrId = lSid;
                psCompIp->sDevHead.lScrTime = time(0);
                psCompIp->cScreen = 1;
            }
            if(lSid0 > 0) {
                sprintf(caFile,"%s/tmp/screen/%lu.jpg",utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog"),
                        lSid0);
                remove(caFile);
#ifdef LDEBUG
       printf(" Remove:%s \n",caFile);
#endif       
            }
        }
    }
    pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* 密钥，暂时不用  */
                    1,
                    "status",    UT_TYPE_LONG,0);
    return 0;
}
	
	
	/* 发送文件  */
long nwDeskUpdataSendFile(utShmHead *psShmHead,int iFd,char *pFile)
{
    utMsgHead *psMsgHead;
    FILE *fp;
    int iReturn,iSize;
    char *pBuf;
    long lRead,lPos,lId,lStatus;
    long iWrite;
    int iBytes;
    unsigned long lBytes;
    char caFile[128];

    sprintf(caFile,"%s/html/soft/%s",utComGetVar_sd(psShmHead,"NcHome","/home/ncmysql/nc"),pFile);
    iSize = utFileSize(caFile);
    if(iSize <= 0) {
        printf("file not exist\n");
        return (-1);
    }
    
    pBuf = malloc(iSize+1);
    if(pBuf == NULL) {
        printf(" Memory Alloc Error \n");
        return (-1);
    }
    lPos = 0;
    lId = 0;
    fp = fopen(caFile,"rb");

        iBytes = fread(pBuf,1,iSize,fp);
        if(iBytes > 0) {

            iWrite = write(iFd,pBuf,iBytes);
            if(iWrite != iBytes) {
                fclose(fp);
                free(pBuf);
                return 0;
            }
           
        }

    fclose(fp);
    free(pBuf);
    return 0;
}
	
	
/* 在线升级客户端*/
int nwTcpUpdaeDesk(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    nwCompIp       *psCompIp;
    FILE *fp;
    char caFile[128];
    char caPath[128];
    char *p;
    int iNum,iReturn,i,k;
    unsigned long lCompId,lSum,lBytes,lSid,lIp,lSid0;
    unsigned short nPort;
#ifdef LDEBUG
   
 //   utMsgPrintMsg(psMsgHead);
#endif
    /* 数据报校验  */
    lCompId = psMsgHead->lReserve[0];
    utComGetSockIp(iFd,&lIp,&nPort);
    lIp = ntohl(lIp);
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&lIp);
    if(psCompIp == NULL || psCompIp->psComp == NULL) {
#ifdef LDEBUG
        printf(" Not Online ....\n");
 //       utMsgPrintMsg(psMsgHead);
#endif
        return 0;
    }
    if(psCompIp->compid != lCompId) {
#ifdef LDEBUG
        printf(" CompId not equal ....\n");
#endif
        return 0;
    }
    if(!utMsgCheckSum(psMsgHead,psCompIp->caKey)) { /* 数据不合法  */
#ifdef LDEBUG
        printf(" It is invalid message, CheckSum Error\n");
#endif        
        return 0;
    }
    iReturn = utMsgGetSomeNVar(psMsgHead,1,
   	                "filename",        UT_TYPE_STRING,63, caFile);
   	               
//   printf("caFile=%s\n");
    if(strlen(caFile)>0)
       nwDeskUpdataSendFile(psShmHead,iFd,caFile);
       
       return 0;

}	
	
	
	
					
/* 显示在线计算机信息  */
int nwWebShowOnlineCompute(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    nwCompInfo     *psComp;    /* 计算机信息  */
    nwCompIp       *psCompIp;
    ncUserInfo     *psUserInfo;
    pasHashInfo sHashInfo;
    char *pHash;
    int iNum,iReturn,i,k;
    unsigned long lGroupid0,lGroupid;
    utPltDbHead *psDbHead;
    char caCond[32],caSear[16],caGroupid[16],caCurpg[16];
    char caPlate[64];
    char mac[6];
    long lRowNum,lCurpg,lStartRec,lTotpg;
    unsigned long lIp;
#ifdef LDEBUG
    printf(" ncWebShowOnlineCompute  \n");
    utMsgPrintMsg(psMsgHead);
#endif
    ncOperWriteLog(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,5,
   	                "cond",      UT_TYPE_STRING, 31, caCond,
   	                "groupid",   UT_TYPE_STRING, 15, caGroupid,
   	                "search",    UT_TYPE_STRING, 15, caSear,
					"CurPg",     UT_TYPE_STRING, 12, caCurpg,
					"plate",     UT_TYPE_STRING, 63, caPlate);
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
    if(!utStrIsSpaces(caGroupid)) {
        lGroupid = atol(caGroupid);
        if(lGroupid > 0 && lGroupid0 > 0 && lGroupid != lGroupid0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0170查询权限"),ncLang("0424您没有权利查看本部门数据的权利"));
            return 0;
        }
        lGroupid0 = lGroupid;
    }
    lRowNum = 33;
    if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * lRowNum;
    if(utStrIsValidIp(caCond)) {
        lIp = ntohl(pasIpcvtLong(caCond));
    }
    else {
        lIp = 0;
    }
    if(ncUtlStrIsMacAddress(caCond)) {
        ncCvtMacI(caCond,mac);
    }
    else {
        memset(mac,0,6);
    }
    psDbHead = utPltInitDb();
    pHash = (char *)utShmHashHead(psShmHead,NC_LNK_ONLINECOMP);
    if(pHash == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0209调试软件"),ncLang("0833在线信息不存在"));
        return 0;
    }
    
    psCompIp  = (nwCompIp *)pasHashFirst(pHash,&sHashInfo);
    i = 0;
    while(psCompIp && i < lStartRec) {
        if(psCompIp->psComp) {
            if( (strstr(psCompIp->psComp->devname,caCond) || lIp & psCompIp->ip == lIp ||
                   strstr(psCompIp->psComp->compname,caCond) || memcmp(psCompIp->psComp->mac,mac,6)==0) &&
                   (lGroupid0 == 0 || psCompIp->psComp->groupid == lGroupid0) ) {
                   	
                   	
                    i++;
            }
        }
        psCompIp  = (nwCompIp *)pasHashNext(&sHashInfo);
    }
    iNum = 0;
    while(psCompIp && iNum < lRowNum) {
        if(psCompIp->psComp) {
        	psUserInfo = ncGetUserInfoByIp(psShmHead,psCompIp->ip);
        	
        	
            if( (strstr(psCompIp->psComp->devname,caCond) || lIp & psCompIp->ip == lIp ||
                   strstr(psCompIp->psComp->compname,caCond) || memcmp(psCompIp->psComp->mac,mac,6)==0) &&
                   (lGroupid0 == 0 || psCompIp->psComp->groupid == lGroupid0)||(psUserInfo&&psUserInfo->psUser&&strstr(psUserInfo->psUser->dispname,caCond)) ) {
                i++;
                iNum++;
                utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
                utPltPutLoopVarF(psDbHead, "compid",iNum,"%lu",psCompIp->compid);
                utPltPutLoopVar(psDbHead, "ip",    iNum,utComHostIp(htonl(psCompIp->ip)));
                utPltPutLoopVar(psDbHead, "mac",   iNum,ncCvtMac(psCompIp->psComp->mac));
                utPltPutLoopVar(psDbHead, "compname", iNum,psCompIp->psComp->compname);
                utPltPutLoopVar(psDbHead, "devname", iNum,psCompIp->psComp->devname);
                utPltPutLoopVarF(psDbHead,"flow",iNum,"%lu",psCompIp->lFlow);
                utPltPutLoopVarF(psDbHead,"netlink",iNum,"%lu",psCompIp->lNetlink);
                utPltPutLoopVarF(psDbHead, "groupid", iNum,"%lu",psCompIp->psComp->groupid);
                utPltPutLoopVar(psDbHead,"version",iNum,psCompIp->psComp->version);

                

  //            psUserInfo = ncGetUserInfoById(psShmHead,psCompIp->psComp->userid);                                       
                if(psUserInfo) {
                    if(psUserInfo->psUser) {            	
                        utPltPutLoopVar(psDbHead, "username", iNum,psUserInfo->psUser->dispname);
                                utPltPutLoopVar(psDbHead, "groupname", iNum,
                                ncUtlGetGroupNameByid(psShmHead,psUserInfo->lGid,"\0"));
                    }
                }
                utPltPutLoopVar(psDbHead,"logtime",iNum,utTimFormat("%m-%d %H:%M:%S",psCompIp->lLoginTime));
                utPltPutLoopVar(psDbHead,"lasttime",iNum,utTimFormat("%m-%d %H:%M:%S",psCompIp->lLastTime));
            }
        }
        psCompIp  = (nwCompIp *)pasHashNext(&sHashInfo);
    }
    
    k=0;
    while(iNum < lRowNum) {
        k++;
        utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
        iNum++;
    }

    while(psCompIp) {
        if(psCompIp->psComp) {
            if( (strstr(psCompIp->psComp->devname,caCond) || lIp & psCompIp->ip == lIp ||
                   strstr(psCompIp->psComp->compname,caCond) || memcmp(psCompIp->psComp->mac,mac,6)==0) &&
                   (lGroupid0 == 0 || psCompIp->psComp->groupid == lGroupid0) ) {
                i++;
            }
        }
        psCompIp  = (nwCompIp *)pasHashNext(&sHashInfo);
    }

    lTotpg = (i - 1) / lRowNum + 1;
    utPltPutVar(psDbHead,"cond",caCond);
    utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
    utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
    utPltPutVarF(psDbHead,"TotRec","%d",i);
    if(utStrIsSpaces(caPlate)) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nc_onlinecomp.htm");
    }
    else {
        utPltPutVar(psDbHead,"plate",caPlate);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    }        
    return 0;
}    

/* 显示用户树  */
int nwWebCompTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    char caFname[64];
    char  caPlate[128],caOpt[2];
    int  iReturn;
    iReturn = utMsgGetSomeNVar(psMsgHead,3,
                    "fname",      UT_TYPE_STRING,63,   caFname,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"opt",        UT_TYPE_STRING, 1,   caOpt);
    psDbHead = utPltInitDb();
    if(!utStrIsSpaces(caPlate)) {
        utPltPutVar(psDbHead,"plate",caPlate);
    }
    if(!utStrIsSpaces(caFname)) {
        utPltPutVar(psDbHead,"fname",caFname);
    }
    utPltPutVar(psDbHead,"opt",caOpt);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
}

/* 显示部门下面的计算机  */
int nwWebTreeDispComp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
   	char caGroupname[32];
   	char caDevname[32];
   	char caUsername[32],caDispname[32];
   	char caFname[32];
   	char caName[32];
   	char caBfname[32];
   	char caWhere[256];
   	char caVar[256];
   	char caValue[256];
   	char caVar1[32];
   	char caApp[16];
   	char *pVar,*p;
   	char caSql[256];
   	int  iNum;
   	char caGroupid[16],caCurPg[16],caTotRec[16],caOpt[12];
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec,lUserid,lTotPg,lCompid;
    utPltDbHead *psDbHead;
   	int iReturn,i;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,8,
   	                "groupid",    UT_TYPE_STRING, 12,  caGroupid,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"TotRec",     UT_TYPE_STRING, 12,  caTotRec,
   	                "name",       UT_TYPE_STRING, 63,  caName,
					"fname",      UT_TYPE_STRING, 31,  caFname,
					"opt",        UT_TYPE_STRING, 1,   caOpt,
					"app",        UT_TYPE_STRING, 12,  caApp);

    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),ncLang("0554数据库出现问题，请与管理员联系"));
        return 0;
    }
    lGroupid = atol(caGroupid);
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
    if(iReturn == 0 && lGroupid0 > 0 && lGroupid != lGroupid0) {
        psDbHead = utPltInitDb();
        if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
        if(!utStrIsSpaces(caName)) utPltPutVar(psDbHead,"name",caName);
        utPltPutVarF(psDbHead, "TotRec","%lu",0);
        utPltPutVarF(psDbHead, "TotPg", "%lu",0);
        utPltPutVarF(psDbHead, "CurPg", "%lu",0);
        utPltPutVarF(psDbHead, "CurPg", "%s",caOpt);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
        return 0;
    }
    else {
    	if((utStrIsSpaces(caName))&&(strlen(caGroupid)!=0))
            sprintf(caWhere," where groupid = %lu ",lGroupid);
        else
            sprintf(caWhere," where 1=1 ");
    }
    if(!utStrIsSpaces(caName)) {
        utStrAddF(caWhere," and (devname like '%c%s%c' or compname like '%c%s%c') ",'%',caName,'%','%',caName,'%');
    }
    lCurPg = atol(caCurPg);
    if(strcmp(caApp,"prev") == 0) {
//        lCurPg = lCurPg - 1;
    }
    else if(strcmp(caApp,"next") == 0) {
//        lCurPg = lCurPg + 1;
    }
    else {
        lCurPg = 1;
        strcpy(caTotRec,"\0");
    }
    if(lCurPg <= 0) lCurPg = 1;
    if(utStrIsSpaces(caTotRec) || utStrIsSpaces(caApp)) {
        sprintf(caSql,"select count(*) from nwcompute %s",caWhere);
// printf("caSql=%s\n",caSql);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lTotRec);
        if(iReturn != 0) {
            printf("1Sql:[select count(*) from nwcompute %s ]\n",caWhere);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"DataBase Error %s",pasDbErrorMsg(NULL));
            return 0;
        }
    }
    else {
        lTotRec = atol(caTotRec);
    }
    lRowNum = utComGetVar_ld(psShmHead,"UserTreeRow",200);
    lTotPg = 1 + (lTotRec - 1) / lRowNum;
    if(lCurPg > lTotPg) lCurPg = lTotPg;

    lStartRec = (lCurPg - 1) * lRowNum; 
//    printf("select devname,compid from nwcompute %s order by compid limit %lu,%lu\n",caWhere,lStartRec,lRowNum);
     
    psCur = pasDbOpenSqlF("select devname,compid from nwcompute %s order by compid limit %lu,%lu",caWhere,lStartRec,lRowNum);
    if(psCur == NULL) {
        printf("Sql:[select devname,compname,compid from nwcompute %s order by compid limit %lu,%lu",caWhere,lStartRec,lRowNum);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caDevname,
                                   UT_TYPE_LONG,4,&lCompid);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caName)) utPltPutVar(psDbHead,"name",caName);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"compid",   iNum,"%lu",lCompid);
        utPltPutLoopVar(psDbHead, "fname",iNum,caFname);
        utPltPutLoopVar(psDbHead, "opt",iNum,caOpt);
        utPltPutLoopVar(psDbHead, "devname", iNum,caDevname);
        iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caDevname,
                                   UT_TYPE_LONG,4,&lCompid);
    }
    pasDbCloseCursor(psCur);
    utPltPutVarF(psDbHead, "TotRec","%lu",lTotRec);
    utPltPutVarF(psDbHead, "TotPg", "%lu",lTotPg);
    utPltPutVarF(psDbHead, "CurPg", "%lu",lCurPg);
    utPltPutVarF(psDbHead, "opt", "%s",caOpt);
    utPltPutVarF(psDbHead, "groupid","%ld",lGroupid);
    if(!utStrIsSpaces(caFname)) {
        utPltPutVar(psDbHead,"fname",caFname);
    }
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}



/* 对一些树的处理  */
int nwWebTreeDispGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
    char caValue[256];
    char caVar[256];
    char caOpt[2];
    char caVar1[32];
    char caBfname[32];
   	char caGroupname[32],caGroupid[16];
   	char caFname[32];
   	char caName[32];
   	char caWhere[256];
   	char caSql[256];
   	char caCurPg[16],caTotRec[16];
   	int  iNum;
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
    char *p,*pVar;
   	int iReturn,i;
   	iReturn = utMsgGetSomeNVar(psMsgHead,6,
   	                "opt",        UT_TYPE_STRING, 1,   caOpt,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"TotRec",     UT_TYPE_STRING, 12,  caTotRec,
   	                "name",       UT_TYPE_STRING, 63,  caName,
					"fname",      UT_TYPE_STRING, 31,  caFname);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"System Error");
        return 0;
    }
    
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
    if(iReturn == 0 && lGroupid0 > 0) {
        sprintf(caWhere," where groupid = %lu ",lGroupid0);
    }
    if(utStrIsSpaces(caName)) {
        if(utStrIsSpaces(caWhere)) {
            sprintf(caWhere,"where groupname like '%c%s%c' ",'%',caName,'%');
        }
        else {
            utStrAddF(caWhere," and groupname like '%c%s%c' ",'%',caName,'%');
        }
    }
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    if(utStrIsSpaces(caTotRec)) {
        sprintf(caSql,"select count(*) from ncgroup %s",caWhere);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lTotRec);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"DataBase Error %s",pasDbErrorMsg(NULL));
            return 0;
        }
    }
    else {
        lTotRec = atol(caTotRec);
    }
    lRowNum = utComGetVar_ld(psShmHead,"GroupTreeRow",200);
    lStartRec = (lCurPg - 1) * lRowNum;  
    psCur = pasDbOpenSqlF("select groupname,groupid from ncgroup %s order by groupid limit %lu,%lu",caWhere,lStartRec,lRowNum);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caFname)) utPltPutVar(psDbHead,"fname",caFname);
    utPltPutVar(psDbHead,"opt",caOpt);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        utPltPutLoopVar(psDbHead,"groupname", iNum,caGroupname);
        utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%ld",lGroupid);
        utPltPutLoopVar(psDbHead,"plate",    iNum,caPlate);
        utPltPutLoopVar(psDbHead,"opt",     iNum,caOpt);
        utPltPutLoopVar(psDbHead,"fname",iNum,caFname);
        iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid);
    }
    pasDbCloseCursor(psCur);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}
 
/* 显示计算机资产信息菜单  */
int  nwWebCompInfo_menu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caPlate[128],caGtype[16],caId[16],caOpt[2];
    int   iReturn;
    long lGtype,lId;
    utPltDbHead *psDbHead;
	iReturn = utMsgGetSomeNVar(psMsgHead,3,
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All 4-Polcy */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
	                "opt",     UT_TYPE_STRING, 1,  caOpt);
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"id",caId);
    utPltPutVar(psDbHead,"gtype",caGtype);
    utPltPutVar(psDbHead,"opt", caOpt);
    utPltPutVar(psDbHead,"dispname",nwGetCompName(atol(caGtype),atol(caId)));
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_zc_menu.htm");
    return 0;
}

char *nwGetCompName(long lGtype,long lId)
{
    static char caName[32];
    int iReturn;
    iReturn = 1;
    if(lGtype ==  1) {
        iReturn = pasDbOneRecord("select devname from nwcompute where compid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_STRING,31,caName);
    }  
    else if(lGtype == 2) {
        iReturn = pasDbOneRecord("select groupname from ncgroup where groupid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_STRING,31,caName);
    }  
    else if(lGtype ==  3) {
        iReturn = 0;
        strcpy(caName,ncLang("0446全部"));
    }
    if(iReturn == 0) {
        return caName;
    }
    else {
        strcpy(caName,"\0");
        return caName;
    }
}

   
/* 显示计算机有关信息  */
int nwWebCompInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caOpt[4];
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"opt",    UT_TYPE_STRING, 1,  caOpt);      /* 1--Base 2--HardInfo  3--HardLog  4--SoftInfo 5--Softlog  */
    switch (caOpt[0]) {
        case '1':
            nwWebCompInfo_Base(psShmHead,iFd,psMsgHead);
            break;
        case '2':
            nwWebCompInfo_HardInfo(psShmHead,iFd,psMsgHead);
            break;
        case '3':
            nwWebCompInfo_HardChange(psShmHead,iFd,psMsgHead);
            break;
        case '4':
            nwWebCompInfo_SoftInfo(psShmHead,iFd,psMsgHead);
            break;
        case '5':
            nwWebCompInfo_SoftChange(psShmHead,iFd,psMsgHead);
            break;

    }
    return 0;
}

/* 显示计算机硬件信息  */

int nwWebCompInfo_HardInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caCurPg[12];
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lRowNum,lStartRec,lCurpg,lCompid,lUserid;
    char caDname[16],caAttr[128];
    char caDevname[32];
    unsigned long lAddtime,lLasttime,lStatus,lTotpg,lSid;
    char caGroupid[16];
    
    int iNum,k;
    
    char caSmt[256],caSql[256];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    printf("nwWebCompInfo_Base\n");
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
   	                "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId,
					"cond",   UT_TYPE_STRING, 31, caCond,
					"CurPg",      UT_TYPE_STRING, 11,  caCurPg);
    lId = atol(caId);
    if(utStrIsSpaces(caCurPg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurPg);
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }
    if(caGtype[0] == '2' || caGtype[0] == '3') {  /* 显示设备列表 */
        if(lGroupid0 > 0) {
            lGroupid = lGroupid0;
            sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lGroupid);
        }
        else {
            if(caGtype[0] == '2') {
                sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lId);
            }
            else {
                sprintf(caSmt," where a.compid = b.compid");
            }
        }
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caSmt,"and (b.dname like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,"or  b.dattr  like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,"or  a.devname  like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,")");
        }
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwcompute a,nwhardware b %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
#ifdef LDEBUG
        printf(" Count==%d \n",lCount);
#endif
        psCur = pasDbOpenSqlF("select b.sid,a.compid,a.devname,a.groupid,b.dname,b.num,b.dattr,b.lasttime from nwcompute a,nwhardware b %s order by a.compid limit %lu,%lu",caSmt,lStartRec,lRowNum);

        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
#ifdef LDEBUG
        printf("[SQL]select b.sid,a.compid,a.devname,a.groupid,b.dname,b.num,b.dattr,b.lasttime from nwcompute a,nwhardware b %s order by a.compid limit %lu,%lu\n",caSmt,lStartRec,lRowNum);
#endif        
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_STRING,15,caDname,
                                       UT_TYPE_LONG,4,&lNum,
                                       UT_TYPE_STRING,127,caAttr,
                                       UT_TYPE_ULONG,4,&lLasttime);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"groupname", iNum,
                                   ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
            utPltPutLoopVarF(psDbHead,"compid", iNum,"%lu",lCompid);
            utPltPutLoopVar(psDbHead,"devname", iNum,caDevname);
            utPltPutLoopVar(psDbHead,"dname", iNum,caDname);
            utPltPutLoopVar(psDbHead,"dattr", iNum,caAttr);
            utPltPutLoopVarF(psDbHead,"dnum", iNum, "%d",lNum);
            utPltPutLoopVar(psDbHead,"lasttime", iNum,utTimFormat("%m-%d %H:%M:%S",lLasttime));
            iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_STRING,15,caDname,
                                       UT_TYPE_LONG,4,&lNum,
                                       UT_TYPE_STRING,127,caAttr,
                                       UT_TYPE_ULONG,4,&lLasttime);
        }
        pasDbCloseCursor(psCur);
        k=0;
        lRowNum = 34;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"cond", caCond);
        utPltPutVar(psDbHead,"gtype",caGtype);
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_hardlist.htm");
    }
    else {  /* 显示单个用户  */
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwhardware where compid = %lu ",lId);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }

        psCur = pasDbOpenSqlF("select sid,dname,num,dattr,addtime,lasttime from nwhardware where compid = %lu order by sid limit %lu,%lu",lId,lStartRec,lRowNum);
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_STRING,15,caDname,
                                       UT_TYPE_LONG,4,&lNum,
                                       UT_TYPE_STRING,127,caAttr,
                                       UT_TYPE_LONG,4,&lAddtime,
                                       UT_TYPE_ULONG,4,&lLasttime);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"dname", iNum,caDname);
            utPltPutLoopVar(psDbHead,"dattr", iNum,caAttr);
            utPltPutLoopVarF(psDbHead,"dnum", iNum, "%d",lNum);
            utPltPutLoopVar(psDbHead,"lasttime", iNum,utTimFormat("%m-%d %H:%M:%S",lLasttime));
            utPltPutLoopVar(psDbHead,"addtime", iNum,utTimFormat("%m-%d %H:%M:%S",lAddtime));

            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_STRING,15,caDname,
                                       UT_TYPE_LONG,4,&lNum,
                                       UT_TYPE_STRING,127,caAttr,
                                       UT_TYPE_LONG,4,&lAddtime,
                                       UT_TYPE_ULONG,4,&lLasttime);

        }
        pasDbCloseCursor(psCur);
        k=0;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_hardlist_u.htm");
    }
}


/* 设备基本信息  */
int nwWebCompInfo_Base(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caCurPg[12];
    long lId,lGroupid0,lGroupid,lCount;
    unsigned long lIp,lRowNum,lStartRec,lCurpg,lCompid,lUserid;
    char caCompname[32],caMac[18],caStatus[12],*pBuf,*pSelect;
    char caDevname[32];
    char caOs[32],caIe[32],caVersion[32];
    unsigned long lAddtime,lLasttime,lStatus,lTotpg;
    char *pSave;
    char caGroupid[16];
    
    int iNum,k;
    
    char caSmt[256],caSql[256];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    printf("nwWebCompInfo_Base\n");
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
   	                "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId,
					"cond",   UT_TYPE_STRING, 31, caCond,
					"CurPg",      UT_TYPE_STRING, 11,  caCurPg);
    lId = atol(caId);
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }
    if(caGtype[0] == '2' || caGtype[0] == '3') {  /* 显示设备列表 */
        if(utStrIsSpaces(caCurPg)) {
            lCurpg = 1;
        }
        else {
            lCurpg = atol(caCurPg);
        }
        if(lGroupid0 > 0) {
            lGroupid = lGroupid0;
            sprintf(caSmt," where groupid = %lu ",lGroupid);
        }
        else {
            if(caGtype[0] == '2') {
                sprintf(caSmt," where groupid = %lu ",lId);
            }
            else {
                sprintf(caSmt," where groupid >= 0 ");
            }
        }
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caSmt,"and (devname like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,"or compname like '%c%s%c' ",'%',caCond,'%');
            if(utStrIsValidIp(caCond)) {
                lIp = ntohl(pasIpcvtLong(caCond));
                utStrAddF(caSmt,"or ip = %lu ",lIp);
            }
            if(ncUtlStrIsMacAddress(caCond)) {
                utStrAddF(caSmt,"or mac = '%s' ",caCond);
            }
            utStrAddF(caSmt,")");
        }
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwcompute %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
#ifdef LDEBUG
        printf(" Count==%d \n",lCount);
#endif
        psCur = pasDbOpenSqlF("select compid,devname,groupid,userid,ip,compname,mac,addtime,lasttime,status from nwcompute %s order by compid limit %lu,%lu",caSmt,lStartRec,lRowNum);
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
#ifdef LDEBUG
        printf("[SQL]select compid,devname,groupid,userid,ip,compname,mac,addtime,lasttime,status from nwcompute %s order by compid limit %lu,%lu\n",caSmt,lStartRec,lRowNum);
#endif        
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_ULONG,4,&lUserid,
                                       UT_TYPE_ULONG,4,&lIp,
                                       UT_TYPE_STRING,31,caCompname,
                                       UT_TYPE_STRING,17,caMac,
                                       UT_TYPE_ULONG,4,&lAddtime,
                                       UT_TYPE_ULONG,4,&lLasttime,
                                       UT_TYPE_LONG, 4,&lStatus);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVar(psDbHead,"groupname", iNum,
                                   ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
            utPltPutLoopVarF(psDbHead,"compid", iNum,"%lu",lCompid);
            utPltPutLoopVar(psDbHead,"devname", iNum,caDevname);
            utPltPutLoopVar(psDbHead,"compname", iNum,caCompname);
            utPltPutLoopVar(psDbHead,"username", iNum,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
            utPltPutLoopVar(psDbHead,"ip", iNum, utComHostIp(htonl(lIp)));
            utPltPutLoopVar(psDbHead,"mac", iNum,caMac);
            utPltPutLoopVar(psDbHead,"addtime",  iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime));
            utPltPutLoopVar(psDbHead,"lasttime", iNum,utTimFormat("%m-%d %H:%M:%S",lLasttime));
            if(lStatus == 0) {
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0850正常"));
            }
            else if(lStatus == 1) {
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0331禁用"));
            }
            else {
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0391免监控"));
            }
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_ULONG,4,&lUserid,
                                       UT_TYPE_ULONG,4,&lIp,
                                       UT_TYPE_STRING,31,caCompname,
                                       UT_TYPE_STRING,17,caMac,
                                       UT_TYPE_ULONG,4,&lAddtime,
                                       UT_TYPE_ULONG,4,&lLasttime,
                                       UT_TYPE_LONG, 4,&lStatus);
        }
        pasDbCloseCursor(psCur);
        k=0;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"cond", caCond);
        utPltPutVar(psDbHead,"gtype",caGtype);
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_baselist.htm");
    }
    else {  /* 显示单个用户  */
        pSave = utMsgGetVar_s(psMsgHead,"save");
        if(pSave) { /* 保存  */
     	    iReturn = utMsgGetSomeNVar(psMsgHead,3,
   	                "groupid",UT_TYPE_STRING, 11, caGroupid,
					"devname",UT_TYPE_STRING, 31, caDevname,
					"status",  UT_TYPE_STRING, 1,  caStatus);
			iReturn = pasDbExecSqlF("update nwcompute set devname = '%s',groupid=%lu,status=%d where compid=%lu",
			          caDevname,atol(caGroupid),atol(caStatus),lId);
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),ncLang("0543数据保存出错, %s"),pasDbErrorMsg(NULL));
                return 0;
            }
            else {
                pasDbCommit(NULL);
            }
        }
        iReturn = pasDbOneRecord("select devname,groupid,userid,ip,compname,mac,addtime,lasttime,status,os,ie,version from nwcompute where compid = :id",1,
                                       "id",UT_TYPE_LONG,lId,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_ULONG,4,&lUserid,
                                       UT_TYPE_ULONG,4,&lIp,
                                       UT_TYPE_STRING,31,caCompname,
                                       UT_TYPE_STRING,17,caMac,
                                       UT_TYPE_ULONG,4,&lAddtime,
                                       UT_TYPE_ULONG,4,&lLasttime,
                                       UT_TYPE_LONG, 4,&lStatus,
                                       UT_TYPE_STRING,31,caOs,
                                       UT_TYPE_STRING,31,caIe,
                                       UT_TYPE_STRING,31,caVersion);
        if(iReturn == 0) { /* 显示记录内容  */
            psDbHead = utPltInitDb();
            sprintf(caGroupid,"%lu",lGroupid);
            if(lGroupid0 > 0) {
                sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu order by groupname",
                  lGroupid0);
            }
            else {
                sprintf(caSmt,"select groupname,groupid from ncgroup  order by groupname");
            }
            pBuf = pasUtlCvtSqlSelectA(caSmt,"\0");
            if(pBuf) {
                pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
                free(pBuf);
                utPltPutVar(psDbHead,"groupid", pSelect);
                free(pSelect);
            }
            utPltPutVar(psDbHead,"ip",utComHostIp(htonl(lIp)));
            utPltPutVar(psDbHead,"groupname",ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
            utPltPutVarF(psDbHead,"compid", "%lu",lId);
            utPltPutVar(psDbHead,"devname", caDevname);
            utPltPutVar(psDbHead,"compname",caCompname);
            utPltPutVar(psDbHead,"username",ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
            utPltPutVar(psDbHead,"mac", caMac);
            utPltPutVar(psDbHead,"addtime",utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime));
            utPltPutVar(psDbHead,"lasttime",utTimFormat("%Y-%m-%d %H:%M:%S",lLasttime));
            sprintf(caStatus,"%d",lStatus);
            pSelect = pasUtlCvtInputSelect("status",caStatus,ncLang("0852正常,0;禁用,1;免监控,9;"));
            if(pSelect) {
                utPltPutVar(psDbHead,"status", pSelect);
                free(pSelect);
            }
            utPltPutVar(psDbHead,"ie", caIe);
            utPltPutVar(psDbHead,"os", caOs);
            utPltPutVar(psDbHead,"ver",caVersion);
            utPltPutVar(psDbHead,"cond", caCond);
            utPltPutVar(psDbHead,"id",   caId);
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_form_devedit.htm");
        }
        else {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        }
    }
    return 0;
}


/* 硬件变化信息  */
int nwWebCompInfo_HardChange(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caCurPg[12];
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lRowNum,lStartRec,lCurpg,lCompid,lUserid;
    char caDname[16],caAttr[128];
    char caDevname[32];
    unsigned long lAddtime,lLasttime,lStatus,lTotpg,lSid,lFlags;
    char caGroupid[16];
    
    int iNum,k;
    
    char caSmt[256],caSql[256];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    printf("nwWebCompInfo_HardChange\n");
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
   	                "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId,
					"cond",   UT_TYPE_STRING, 31, caCond,
					"CurPg",      UT_TYPE_STRING, 11,  caCurPg);
    lId = atol(caId);
    if(utStrIsSpaces(caCurPg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurPg);
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }
    if(caGtype[0] == '2' || caGtype[0] == '3') {  /* 显示设备列表 */
        if(lGroupid0 > 0) {
            lGroupid = lGroupid0;
            sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lGroupid);
        }
        else {
            if(caGtype[0] == '2') {
                sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lId);
            }
            else {
                sprintf(caSmt," where a.compid = b.compid ");
            }
        }
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caSmt,"and (b.dname like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,"or  b.dvalue  like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,"or  a.devname  like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,")");
        }
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwcompute a,nwhardlog b %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
#ifdef LDEBUG
        printf(" Count==%d \n",lCount);
#endif
        psCur = pasDbOpenSqlF("select b.sid,a.compid,a.devname,a.groupid,b.dname,b.dvalue,b.mtime,b.mflags from nwcompute a,nwhardlog b %s order by  b.mtime desc limit %lu,%lu",caSmt,lStartRec,lRowNum);

        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_STRING,15,caDname,
                                       UT_TYPE_STRING,127,caAttr,
                                       UT_TYPE_ULONG,4,&lLasttime,
                                       UT_TYPE_LONG,4,&lFlags);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"groupname", iNum,
                                   ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
            utPltPutLoopVarF(psDbHead,"compid", iNum,"%lu",lCompid);
            utPltPutLoopVar(psDbHead,"devname", iNum,caDevname);
            utPltPutLoopVar(psDbHead,"dname", iNum,caDname);
            utPltPutLoopVar(psDbHead,"dattr", iNum,caAttr);
            utPltPutLoopVar(psDbHead,"mflags", iNum,nwUtlGetMflags(lFlags));
            utPltPutLoopVar(psDbHead,"lasttime", iNum,utTimFormat("%m-%d %H:%M:%S",lLasttime));
           iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_STRING,15,caDname,
                                       UT_TYPE_STRING,127,caAttr,
                                       UT_TYPE_ULONG,4,&lLasttime,
                                       UT_TYPE_LONG,4,&lFlags);
        }
        pasDbCloseCursor(psCur);
        k=0;
        lRowNum = 34;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"cond", caCond);
        utPltPutVar(psDbHead,"gtype",caGtype);
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_hardchange.htm");
    }
    else {  /* 显示单个用户  */
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwhardlog where compid = %lu",lId);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }

        psCur = pasDbOpenSqlF("select sid,dname,dvalue,mtime,mflags from nwhardlog where compid = %lu order by mtime desc limit %lu,%lu",lId,lStartRec,lRowNum);
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_STRING,15,caDname,
                                       UT_TYPE_STRING,127,caAttr,
                                       UT_TYPE_ULONG,4,&lLasttime,
                                       UT_TYPE_LONG,4,&lFlags);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"dname", iNum,caDname);
            utPltPutLoopVar(psDbHead,"dattr", iNum,caAttr);
            utPltPutLoopVar(psDbHead,"mflags", iNum,nwUtlGetMflags(lFlags));
            utPltPutLoopVar(psDbHead,"lasttime", iNum,utTimFormat("%m-%d %H:%M:%S",lLasttime));
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_STRING,15,caDname,
                                       UT_TYPE_STRING,127,caAttr,
                                       UT_TYPE_ULONG,4,&lLasttime,
                                       UT_TYPE_LONG,4,&lFlags);

        }
        pasDbCloseCursor(psCur);
        k=0;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_hardchange_u.htm");
    }
}


/* 显示计算机软件信息  */

int nwWebCompInfo_SoftInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caCurPg[12];
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lRowNum,lStartRec,lCurpg,lCompid,lUserid;
    
    char caSname[72];
    char caDevname[32];
    unsigned long lLasttime,lStatus,lTotpg,lSid,lType;
    char caGroupid[16],caInsttime[20];
    
    int iNum,k;
    
    char caSmt[256],caSql[256];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    printf("nwWebCompInfo_SoftInfo\n");
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
   	                "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId,
					"cond",   UT_TYPE_STRING, 31, caCond,
					"CurPg",      UT_TYPE_STRING, 11,  caCurPg);
    lId = atol(caId);
    if(utStrIsSpaces(caCurPg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurPg);
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }
    if(caGtype[0] == '2' || caGtype[0] == '3') {  /* 显示设备列表 */
        if(lGroupid0 > 0) {
            lGroupid = lGroupid0;
            sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lGroupid);
        }
        else {
            if(caGtype[0] == '2') {
                sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lId);
            }
            else {
                sprintf(caSmt," where a.compid = b.compid ");
            }
        }
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caSmt,"and (b.sname like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,"or  a.devname  like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,")");
        }
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwcompute a,nwsoftware b %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
#ifdef LDEBUG
        printf(" Count==%d \n",lCount);
#endif
        psCur = pasDbOpenSqlF("select b.sid,a.compid,a.devname,a.groupid,b.sname,b.instime,b.lasttime from nwcompute a,nwsoftware b %s order by a.compid limit %lu,%lu",caSmt,lStartRec,lRowNum);

        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_STRING,71,caSname,
                                       UT_TYPE_STRING,15,caInsttime,
                                       UT_TYPE_ULONG,4,&lLasttime);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"groupname", iNum,
                                   ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
            utPltPutLoopVarF(psDbHead,"compid", iNum,"%lu",lCompid);
            utPltPutLoopVar(psDbHead,"devname", iNum,caDevname);
            utPltPutLoopVar(psDbHead,"sname", iNum,caSname);
            utPltPutLoopVar(psDbHead,"insttime", iNum,caInsttime);
            utPltPutLoopVar(psDbHead,"lasttime", iNum,utTimFormat("%m-%d %H:%M:%S",lLasttime));
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_STRING,71,caSname,
                                       UT_TYPE_STRING,15,caInsttime,
                                       UT_TYPE_ULONG,4,&lLasttime);
        }
        pasDbCloseCursor(psCur);
        k=0;
        lRowNum = 34;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"cond", caCond);
        utPltPutVar(psDbHead,"gtype",caGtype);
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_softlist.htm");
    }
    else {  /* 显示单个用户  */
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwsoftware where compid = %lu ",lId);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
        psCur = pasDbOpenSqlF("select sid,sname,instime,lasttime from nwsoftware where compid = %lu order by sid limit %lu,%lu",lId,lStartRec,lRowNum);
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_STRING,71,caSname,
                                       UT_TYPE_STRING,15,caInsttime,
                                       UT_TYPE_ULONG,4,&lLasttime);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"sname", iNum,caSname);
            utPltPutLoopVar(psDbHead,"insttime", iNum,caInsttime);
            utPltPutLoopVar(psDbHead,"lasttime", iNum,utTimFormat("%m-%d %H:%M:%S",lLasttime));
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_STRING,71,caSname,
                                       UT_TYPE_STRING,15,caInsttime,
                                       UT_TYPE_ULONG,4,&lLasttime);

        }
        pasDbCloseCursor(psCur);
        k=0;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_softlist_u.htm");
    }
}


/* 显示计算机软件变化信息  */
int nwWebCompInfo_SoftChange(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caCurPg[12];
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lRowNum,lStartRec,lCurpg,lCompid,lUserid;
    
    char caSname[72];
    char caDevname[32];
    unsigned long lLasttime,lStatus,lTotpg,lSid,lFlags;
    char caGroupid[16];
    long lType;
    
    int iNum,k;
    
    char caSmt[256],caSql[256];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    printf("nwWebCompInfo_SoftChange\n");
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
   	                "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId,
					"cond",   UT_TYPE_STRING, 31, caCond,
					"CurPg",      UT_TYPE_STRING, 11,  caCurPg);
    lId = atol(caId);
    if(utStrIsSpaces(caCurPg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurPg);
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }
    if(caGtype[0] == '2' || caGtype[0] == '3') {  /* 显示设备列表 */
        if(lGroupid0 > 0) {
            lGroupid = lGroupid0;
            sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lGroupid);
        }
        else {
            if(caGtype[0] == '2') {
                sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lId);
            }
            else {
                sprintf(caSmt," where a.compid = b.compid  ");
            }
        }
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caSmt,"and (b.sname like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,"or  a.devname  like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,")");
        }
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwcompute a,nwsoftlog b %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
#ifdef LDEBUG
        printf(" Count==%d \n",lCount);
#endif
        psCur = pasDbOpenSqlF("select b.sid,a.compid,a.devname,a.groupid,b.sname,b.mflags,b.mtime from nwcompute a,nwsoftlog b %s order by b.mtime desc limit %lu,%lu",caSmt,lStartRec,lRowNum);

        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_STRING,71,caSname,
                                       UT_TYPE_LONG,4,&lFlags,
                                       UT_TYPE_ULONG,4,&lLasttime);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"groupname", iNum,
                                   ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
            utPltPutLoopVarF(psDbHead,"compid", iNum,"%lu",lCompid);
            utPltPutLoopVar(psDbHead,"devname", iNum,caDevname);
            utPltPutLoopVar(psDbHead,"sname", iNum,caSname);
            utPltPutLoopVar(psDbHead,"mflags", iNum,nwUtlGetMflags(lFlags));
            utPltPutLoopVar(psDbHead,"mtime", iNum,utTimFormat("%m-%d %H:%M:%S",lLasttime));
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_STRING,71,caSname,
                                       UT_TYPE_LONG,4,&lFlags,
                                       UT_TYPE_ULONG,4,&lLasttime);
        }
        pasDbCloseCursor(psCur);
        k=0;
        lRowNum = 34;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"cond", caCond);
        utPltPutVar(psDbHead,"gtype",caGtype);
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_softchange.htm");
    }
    else {  /* 显示单个用户  */
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwsoftlog where compid = %lu ",lId);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
        psCur = pasDbOpenSqlF("select sid,sname,mflags,mtime from nwsoftlog where compid = %lu order by mtime desc limit %lu,%lu",lId,lStartRec,lRowNum);
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_STRING,71,caSname,
                                       UT_TYPE_LONG,4,&lFlags,
                                       UT_TYPE_ULONG,4,&lLasttime);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"sname", iNum,caSname);
            utPltPutLoopVar(psDbHead,"mflags", iNum,nwUtlGetMflags(lFlags));
            utPltPutLoopVar(psDbHead,"mtime", iNum,utTimFormat("%m-%d %H:%M:%S",lLasttime));
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_STRING,71,caSname,
                                       UT_TYPE_LONG,4,&lFlags,
                                       UT_TYPE_ULONG,4,&lLasttime);

        }
        pasDbCloseCursor(psCur);
        k=0;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_softchange_u.htm");
    }
}


char *nwUtlGetMflags(int iFlags)
{
    static  char caType[8];
    if(iFlags == 'M') {
        strcpy(caType,ncLang("0266更新"));
    }
    else if(iFlags == 'A') {
        strcpy(caType,ncLang("0836增加"));
    }
    else if(iFlags == 'D'){
        strcpy(caType,ncLang("0466删除"));
    }
    else {
        strcpy(caType,"\0");
    }
    return caType;
}


/* 显示在线计算机信息  */
int nwWebPrintOnline(utShmHead *psShmHead)
{
    nwCompInfo     *psComp;    /* 计算机信息  */
    nwCompIp       *psCompIp;
    ncUserInfo     *psUserInfo;
    pasHashInfo sHashInfo;
    char *pHash;
    int iNum,iReturn,i,k;
    unsigned long lGroupid0,lGroupid;
    utPltDbHead *psDbHead;
    char caCond[32],caSear[16],caGroupid[16],caCurpg[16];
    char caPlate[64];
    char mac[6];
    long lRowNum,lCurpg,lStartRec,lTotpg;
    unsigned long lIp;
    pHash = (char *)utShmHashHead(psShmHead,NC_LNK_ONLINECOMP);
    if(pHash == NULL) {
        printf(" Hash Error \n");
        return 0;
    }
    psCompIp  = (nwCompIp *)pasHashFirst(pHash,&sHashInfo);
    i = 0;
    iNum = 0;
    while(psCompIp) {
        if(psCompIp->psComp) {
                iNum++;
                printf(" %3d  %lu %s  %s  %s %s %lu %s \n",
                     iNum,
                     psCompIp->compid,
                     utComHostIp(htonl(psCompIp->ip)),
                     ncCvtMac(psCompIp->psComp->mac),
                     psCompIp->psComp->compname,
                     psCompIp->psComp->devname,
                     psCompIp->psComp->groupid,
//                     ncUtlGetGroupNameByid(psShmHead,psCompIp->psComp->groupid,"\0"),
                     utTimFormat("%Y-%m-%d %H:%M:%S",psCompIp->lLoginTime));
        }
        psCompIp  = (nwCompIp *)pasHashNext(&sHashInfo);
    }
    return 0;
}    
 
/* 显示进程集合列表 */
int ncWebProcClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId;
   	char caName[64];
   	char caPlate[128];
 	pasDbCursor *psCur;
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebLimIpGroup");
 	}
	iReturn = utMsgGetSomeNVar(psMsgHead,1,
	                "plate",  UT_TYPE_STRING,127,caPlate);
 	
    psCur = pasDbOpenSqlF("select id,name from nwprocclass");
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0087Web组信息出错"));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lId,
  	                               UT_TYPE_STRING,63,caName);
   	iNum = 0;
   	while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"id",   iNum,"%u",lId);
        utPltPutLoopVarF(psDbHead,"name", iNum,"%s",caName);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum);
        iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lId,
  	                               UT_TYPE_STRING,63,caName);
    }
    pasDbCloseCursor(psCur);
    utPltPutVar(psDbHead,"plate",caPlate);
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
} 

//编辑进程集合
int ncWebProcClassEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId,lSid,i;
   	char caName[64];
   	char caPlate[128];
   	char caVar[64];

   	char caGid[16],caDid[16],caAdd[16],caDel[16];
	char caNname[32],caNid[16],caModi[16];
 	char *p;
 	pasDbCursor *psCur;
 	int mod_flag=0;
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebLimIpGroup");
 	}
	iReturn = utMsgGetSomeNVar(psMsgHead,8,
	                "gname",  UT_TYPE_STRING,63,caName,
	                "sid",    UT_TYPE_STRING,12,caGid,
	                "del",    UT_TYPE_STRING,12,caDel,
	                "add",    UT_TYPE_STRING,12,caAdd,
	                "plate",  UT_TYPE_STRING,127,caPlate,
	                "nid",    UT_TYPE_STRING,12,caNid,
	                "modi",   UT_TYPE_STRING,12,caModi,
	                "nname",  UT_TYPE_STRING,31,caNname
	                );
    if(!utStrIsSpaces(caAdd)) { /* 增加一个组  */
       
            iReturn = pasDbExecSqlF("insert into nwprocclass(id,name) values (%lu,'%s')",
                                             pasGetSid("nwprocclass","id"),caName);
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0316记录增加出错,可能为ID重复"));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                pasDbCommit(NULL);
                mod_flag++;
            }                
        }

    else if(!utStrIsSpaces(caDel)) { /* 删除一个组  */
    	
    	  for(i=0;i<999;i++) {
   	        sprintf(caVar,"ch%d",i+1);

   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
   	            lSid = atol(p);
	        iReturn = pasDbExecSqlF("delete from nwprocclass where id= %lu",lSid);
		        if(iReturn != 0) {
	            pasDbRollback(NULL);
	        }                
	        else {
	        	mod_flag++;
	            iReturn = pasDbExecSqlF("delete from nwproclist where id= %lu",lSid);
	            pasDbCommit(NULL);
	        }            
   	        }
             	        
   	    }
    	

    }
    else if(!utStrIsSpaces(caModi) && !utStrIsSpaces(caName)) { /* 修改一个组  */
        iReturn = pasDbExecSqlF("update nwprocclass set name='%s' where id = %s",
                         caName,caGid);
        pasDbCommit(NULL);
        mod_flag++;
    }
    if(mod_flag>0)
      ncSetLastInfo_l("ProcCtlLastTime",time(0));
      
    return ncWebProcClass(psShmHead,iFd,psMsgHead);
    return 0;
} 


/* 显示某集合的进程列表*/
int ncWebProcClassPlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caWhere[255],caTemp[255];
    char caCond[64];
    char caTotPg[16],caCurPg[16],caTotRec[16];
    char caId[16],caSear[16],caModi[16],caDel[16],caSid[16],caAdd[16];
    char caName[64],caApp[16],caDescr[64];
    char caUip1[32],caLip1[32],caName1[64],caCname[64],caId1[16];
    long lId;
    char *p;
    int i,iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lStartRec,lSumUser,lCount,lCurPg,lTotRec,lRowNum,lSid1,lId1,lLport1,lUport1;
   	unsigned long lSid,lTotPg;
   	int mod_flag=0;
 	pasDbCursor *psCur;

	iReturn = utMsgGetSomeNVar(psMsgHead,9,
	                "cid",      UT_TYPE_STRING, 12, caId,
	                "add",        UT_TYPE_STRING, 12, caAdd,
	                "modi",       UT_TYPE_STRING, 12, caModi,
	                "del",        UT_TYPE_STRING, 12, caDel,
	                "sid",        UT_TYPE_STRING, 12, caSid,	         
	                "descr",       UT_TYPE_STRING,63, caDescr,
	                "name2",       UT_TYPE_STRING, 32, caName,
					"CurPg",      UT_TYPE_STRING, 12, caCurPg,
					"TotRec",     UT_TYPE_STRING, 12, caTotRec);

    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
    	lCount=0;
       sprintf(caTemp,"select count(*) from nwprocclass where id=%d and name='%s' ",caId,caName);
       pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
       if(lCount>0){
       	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0319记录重复"),ncLang("0319记录重复"),pasDbErrorMsg(NULL));
                pasDbRollback(NULL);
                return 0;
        }
        iReturn = pasDbExecSqlF("insert into nwproclist(sid,id,name,descr) values (%lu,%lu,'%s','%s')",
            pasGetSid("nwproclist","sid"),atol(caId),caName,caDescr);

            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0311记录增加出错,%s"),pasDbErrorMsg(NULL));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                strcpy(caTotPg,"\0");
                pasDbCommit(NULL);
                mod_flag++;
            }
      }    
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
   	    for(i=0;i<50;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
                iReturn = pasDbExecSqlF("delete from nwproclist where sid = %s",p);
                if(iReturn == 0) {
                    strcpy(caTotPg,"\0");
                    pasDbCommit(NULL);
                    mod_flag++;
                }
   	        }
   	    }
    }
    else if(!utStrIsSpaces(caModi)) { /* 修改记录  */
        if(!utStrIsSpaces(caSid)) {
           
            
           
            iReturn = pasDbExecSqlF("update nwproclist set id=%lu,name='%s',descr='%s' where sid = %s",atol(caId),caName,caDescr,caSid);
            if(iReturn == 0) {
                pasDbCommit(NULL);
                mod_flag++;
            }
            else {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0303记录修改出错"));
                pasDbRollback(NULL);
            }
        }
    }
   
    sprintf(caTemp,"select count(*) from nwproclist where 1=1 ");
    if(strlen(caId)>0){
     sprintf(caTemp+strlen(caTemp)," and id=%s ",caId);       
     }
     
     iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
     if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0561数据统计出错"));
            return 0;
      }
      lTotRec = lCount;
   

    
    lRowNum = 30;
    lStartRec = (lCurPg - 1) * lRowNum; 
    sprintf(caTemp,"select a.sid,a.id,a.name,a.descr,b.name from nwproclist a,nwprocclass b where a.id=b.id ");
    if(strlen(caId)>0){
    	sprintf(caTemp+strlen(caTemp)," and a.id=%s ",caId);
    }  
    sprintf(caTemp+strlen(caTemp)," order by a.id,a.name limit %lu,%lu ",lStartRec,lRowNum);
    psCur = pasDbOpenSqlF(caTemp);
         
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"cid",caId);
    lSid=0;
    lId=0;
    memset(caName,0,sizeof(caName));
    memset(caDescr,0,sizeof(caDescr));
    memset(caCname,0,sizeof(caCname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lId,
      	                           UT_TYPE_STRING,32,caName,
      	                           UT_TYPE_STRING,63,caDescr,
      	                           UT_TYPE_STRING,32,caCname);
  	                              
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%u",iNum);
        utPltPutLoopVarF(psDbHead,"lsid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"lid",    iNum,"%lu", lId);
        
        utPltPutLoopVar(psDbHead,"lname",   iNum,caName);
        utPltPutLoopVar(psDbHead,"cname2",iNum,caCname);
        utPltPutLoopVar(psDbHead,"descr",iNum,caDescr);
        
		    lSid=0;
		    lId=0;
		    memset(caName,0,sizeof(caName));
		    memset(caDescr,0,sizeof(caDescr));
		    memset(caCname,0,sizeof(caCname));
		    iReturn = pasDbFetchInto(psCur,
		      	                           UT_TYPE_LONG,4,&lSid,
		      	                           UT_TYPE_LONG,4,&lId,
		      	                           UT_TYPE_STRING,32,caName,
		      	                           UT_TYPE_STRING,63,caDescr,
		      	                           UT_TYPE_STRING,32,caCname);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lTotRec - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }

    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
   
   
   psCur = pasDbOpenSqlF("select id,name from nwprocclass order by name ");   
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    
    lSid=0;
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_STRING,32,caName);
      	                           
   	iNum = 0;

   	while(iReturn == 0 ) {
        iNum++;
   
        utPltPutLoopVarF(psDbHead,"cid",   iNum,"%lu",lSid);
        utPltPutLoopVar(psDbHead,"cname",iNum,caName);
      
           
    	lSid=0;
    	memset(caName,0,sizeof(caName));
    	iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_STRING,32,caName);
    }
    pasDbCloseCursor(psCur);
   
   if(mod_flag>0)
    ncSetLastInfo_l("ProcCtlLastTime",time(0));
    
//   utPltShowDb(psDbHead);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_webproclist.htm");
    return 0;
}


/* 显示进程控制规则*/
int nwWebLim_Proc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caMessage[556],caMessage1[556];
    char caMod[16],caDel[16],caAdd[16],caApp[16],caCurPg[16];
    char caSum[16],caGtype[16],caFlags[16],caId[16],caRnum[16],caService[16];
    char caSid[16],caEtime[16],caStime[16];
    char caUrlid[24];
   
    char *pBuf,*pSelect,*p;
    unsigned long lLip;
    long  lStart,lStartRec,lCurPg,lTotPg;
    long  lRnum,lSum,lGtype,lFlags,lId,lDateid,lLimctl,lStime,lEtime,lSid,lService;
    long  lCount,lRowNum;
    int   iReturn,i;
    char caCtype[12],caDid[16],caProcname[64];
    long lDid,lCtype,iNum,lTotRec;
    char sqlbuf[1024];
    int mod_flag=0;
    char caDname[32];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,13,
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All  */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
	                "add",     UT_TYPE_STRING, 12, caAdd,
	                "mod",     UT_TYPE_STRING, 12, caMod,
	                "del",     UT_TYPE_STRING, 12, caDel,
	                "app",     UT_TYPE_STRING,12,caApp,
	                "sid",     UT_TYPE_STRING, 12, caSid,
	                "ctype",  UT_TYPE_STRING, 8, caCtype,
	                "did",     UT_TYPE_STRING, 12, caDid,
	                "procname",UT_TYPE_STRING,63,caProcname,
	                "etime",      UT_TYPE_STRING, 8,  caEtime,
	                "stime",      UT_TYPE_STRING, 8,  caStime,	              
	                "CurPg",     UT_TYPE_STRING, 12, caCurPg);
	                
	lCurPg = atol(caCurPg);
	if(lCurPg == 0) lCurPg = 1;
	lRowNum = 24;
	lStart = (lCurPg - 1) * lRowNum;          
	
    lGtype = atol(caGtype);
   
    lId = atol(caId);
    lDid = atol(caDid);
    lCtype = atol(caCtype);
   
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
       
       lStime = ncUtlIsValidTime(caStime);
       lEtime = ncUtlIsValidTime(caEtime);
       if(lStime < 0 || lEtime < 0) {
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0435起始或终止时间不正确"));
           return 0;
       }
       
       iReturn = pasDbExecSqlF("insert into nwctlproc(sid,stype,uid,ctype,procname,did,stime,etime,mtime)\
                      values(%lu,%d,%lu,%d,'%s',%lu,%lu,%lu,%d)", \
                     pasGetSid("nwctlproc","sid"),lGtype,lId,lCtype,caProcname,lDid,lStime,lEtime,time(0));
                if(iReturn != 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0314记录增加出错, 可能为记录重复"));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                	  mod_flag++;
                    pasDbCommit(NULL);
                 
                }
            
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
   	    for(i=0;i<24;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
   	                mod_flag++;
                    iReturn = pasDbExecSqlF("delete from nwctlproc where sid = %s",p);
                    pasDbCommit(NULL);
                }
               
   	       }
   	   
    }
    else if(!utStrIsSpaces(caMod)) { /* 修改记录  */
        lSid = atol(caSid);	 
        lStime = ncUtlIsValidTime(caStime);
        lEtime = ncUtlIsValidTime(caEtime);     
        iReturn = pasDbExecSqlF("update nwctlproc set stype=%lu,uid=%lu,ctype=%lu,procname='%s',stime=%lu,etime=%lu,mtime=%lu,did=%d where sid = %lu",
         lGtype,lId,lCtype,caProcname,lStime,lEtime,time(0),atol(caDid),lSid);
                          
                if(iReturn != 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0308记录修改出错, 可能为记录重复 %s"),pasDbErrorMsg(NULL));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                    pasDbCommit(NULL);
                    mod_flag++;
                }
                         
       
    }
  else if(!utStrIsSpaces(caApp)) { /* 应用  */
  	nwUtlLoadProcCtl(psShmHead);
  }
    psDbHead = utPltInitDb();
    
    lCount = 0;
    
    sprintf(sqlbuf,"select count(*) from nwctlproc  where stype=%d ",lGtype);
    
    if(lId>0&&lGtype<3){
    	sprintf(sqlbuf+strlen(sqlbuf)," and uid=%lu ",lId);
    }
    pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lCount);
    
    sprintf(sqlbuf,"select sid,ctype,procname,stime,etime,did,name from nwctlproc left join nclimdatedes on (did=id) where stype=%d ",lGtype);
    if(lId>0&&lGtype<3){
    	sprintf(sqlbuf+strlen(sqlbuf)," and uid=%lu ",lId);
    }
    sprintf(sqlbuf+strlen(sqlbuf)," order by procname limit %lu,%lu ",lStart,lRowNum);
    
//    printf("sqlbuf=%s\n",sqlbuf);
    psCur=pasDbOpenSqlF(sqlbuf);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    lSid=0;
    lCtype=0;
    memset(caProcname,0,sizeof(caProcname));
    lStime=0;
    lEtime=0;
    lDid=0;
    memset(caDname,0,sizeof(caDname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lCtype,
      	                           UT_TYPE_STRING,63,caProcname,
      	                           UT_TYPE_LONG,4,&lStime,
      	                           UT_TYPE_LONG,4,&lEtime,
      	                           UT_TYPE_LONG,4,&lDid,
      	                           UT_TYPE_STRING,30,caDname);
      	iNum=0;
      	while(iReturn==0||iReturn==1405){
      		iNum++;
      		utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
      		utPltPutLoopVarF(psDbHead,"num",iNum,"%u",iNum);
      		utPltPutLoopVar(psDbHead,"procname",iNum,caProcname);
      		utPltPutLoopVar(psDbHead,"stime",iNum, ncLimCvtTime(lStime));
          utPltPutLoopVar(psDbHead,"etime",iNum, ncLimCvtTime(lEtime));                    
          if(lCtype==0)
           utPltPutLoopVar(psDbHead,"ctldesc",iNum,ncLang("1419禁止使用"));
          else if(lCtype==1)
           utPltPutLoopVar(psDbHead,"ctldesc",iNum,ncLang("1420允许使用"));
           
          utPltPutLoopVarF(psDbHead,"ctype",iNum,"%u",lCtype);
          
          utPltPutLoopVarF(psDbHead,"did",iNum,"%d",lDid);
          if(lDid>0)
         		utPltPutLoopVar(psDbHead,"datename",iNum, caDname);
          else
          	utPltPutLoopVar(psDbHead,"datename",iNum, ncLang("0383每天"));
    
        lSid=0;
    lCtype=0;
    memset(caProcname,0,sizeof(caProcname));
    lStime=0;
    lEtime=0;
    lDid=0;
    memset(caDname,0,sizeof(caDname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lCtype,
      	                           UT_TYPE_STRING,63,caProcname,
      	                           UT_TYPE_LONG,4,&lStime,
      	                           UT_TYPE_LONG,4,&lEtime,
      	                           UT_TYPE_LONG,4,&lDid,
      	                           UT_TYPE_STRING,30,caDname);
     }
     pasDbCloseCursor(psCur);
     
    lTotRec=lCount;
    lTotPg = (lTotRec ) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }

    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
     
     
  
    utPltPutVar(psDbHead,"stime","00:00:00");
    utPltPutVar(psDbHead,"etime","23:59:59");
 
    utPltPutVarF(psDbHead,"gtype","%lu",lGtype);
    utPltPutVarF(psDbHead,"id","%lu",lId);
  
   if(mod_flag>0){
   	ncSetLastInfo_l("ProcCtlLastTime",time(0));
   }
   
    pBuf = pasUtlCvtSqlSelectA("select name,id from nclimdatedes order by name",ncLang("0384每天,0;"));
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("did","0",pBuf);
        free(pBuf);
    }
    utPltPutVar(psDbHead,"dateid", pSelect);
    free(pSelect);
   
   utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nw_lim_proc.htm");
    
    return 0;
}


/* 显示进程集控制规则*/
int nwWebLim_Proclist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caMessage[556],caMessage1[556];
    char caMod[16],caDel[16],caAdd[16],caApp[16],caCurPg[16];
    char caSum[16],caGtype[16],caFlags[16],caId[16],caRnum[16],caService[16];
    char caSid[16],caEtime[16],caStime[16];
    char caUrlid[24];
   
    char *pBuf,*pSelect,*p;
    unsigned long lLip;
    long  lStart,lStartRec,lCurPg,lTotPg;
    long  lRnum,lSum,lGtype,lFlags,lId,lDateid,lLimctl,lStime,lEtime,lSid,lService;
    long  lCount,lRowNum;
    int   iReturn,i,lPid;
    char caCtype[12],caDid[16],caPid[64];
    long lDid,lCtype,iNum,lTotRec;
    char sqlbuf[1024];
    char caName[32];
    char caPname[32];
    char caDname[32];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    int mod_flag=0;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,13,
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All  */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
	                "add",     UT_TYPE_STRING, 12, caAdd,
	                "mod",     UT_TYPE_STRING, 12, caMod,
	                "del",     UT_TYPE_STRING, 12, caDel,
	                "app",     UT_TYPE_STRING,12,caApp,
	                "sid",     UT_TYPE_STRING, 12, caSid,
	                "ctype",  UT_TYPE_STRING, 8, caCtype,
	                "did",     UT_TYPE_STRING, 12, caDid,
	                "pid",UT_TYPE_STRING,63,caPid,
	                "etime",      UT_TYPE_STRING, 8,  caEtime,
	                "stime",      UT_TYPE_STRING, 8,  caStime,	              
	                "CurPg",     UT_TYPE_STRING, 12, caCurPg);
	                
	lCurPg = atol(caCurPg);
	if(lCurPg == 0) lCurPg = 1;
	lRowNum = 24;
	lStart = (lCurPg - 1) * lRowNum;          
	
    lGtype = atol(caGtype);
   
    lId = atol(caId);
    lDid = atol(caDid);
    lCtype = atol(caCtype);
    lPid=atol(caPid);
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
       
       lStime = ncUtlIsValidTime(caStime);
       lEtime = ncUtlIsValidTime(caEtime);
       if(lStime < 0 || lEtime < 0) {
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0435起始或终止时间不正确"));
           return 0;
       }
       
       iReturn = pasDbExecSqlF("insert into nwctlproclist(sid,stype,uid,ctype,pid,did,stime,etime,mtime)\
                      values(%lu,%d,%lu,%d,%d,%lu,%lu,%lu,%d)", \
                     pasGetSid("nwctlproclist","sid"),lGtype,lId,lCtype,lPid,lDid,lStime,lEtime,time(0));
                if(iReturn != 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0314记录增加出错, 可能为记录重复"));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                	mod_flag++;
                    pasDbCommit(NULL);
                 
                }
            
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
   	    for(i=0;i<24;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
   	                 mod_flag++;
                    iReturn = pasDbExecSqlF("delete from nwctlproclist where sid = %s",p);
                }
               
   	       }
   	   
    }
    else if(!utStrIsSpaces(caMod)) { /* 修改记录  */
        lSid = atol(caSid);	 
        lStime = ncUtlIsValidTime(caStime);
        lEtime = ncUtlIsValidTime(caEtime);     
        iReturn = pasDbExecSqlF("update nwctlproclist set stype=%lu,uid=%lu,ctype=%lu,pid=%d,stime=%lu,etime=%lu,mtime=%lu,did=%d where sid = %lu",
         lGtype,lId,lCtype,lPid,lStime,lEtime,time(0),atol(caDid),lSid);
                          
                if(iReturn != 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0308记录修改出错, 可能为记录重复 %s"),pasDbErrorMsg(NULL));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                	mod_flag++;
                    pasDbCommit(NULL);
                
                }
                         
       
    }
    else if(!utStrIsSpaces(caApp)) { /* 应用  */
  	nwUtlLoadProcCtl(psShmHead);
  }
    psDbHead = utPltInitDb();
    
    lCount = 0;
    
    sprintf(sqlbuf,"select count(*) from nwctlproclist,nwprocclass  where pid=id and stype=%d ",lGtype);
    
    if(lId>0&&lGtype<3){
    	sprintf(sqlbuf+strlen(sqlbuf)," and uid=%lu ",lId);
    }
    pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lCount);
    
    sprintf(sqlbuf,"select sid,ctype,a.name,stime,etime,pid,did,b.name from nwctlproclist,nwprocclass a left join nclimdatedes b on (did=b.id) where pid=a.id and stype=%d ",lGtype);
    if(lId>0&&lGtype<3){
    	sprintf(sqlbuf+strlen(sqlbuf)," and uid=%lu ",lId);
    }
    sprintf(sqlbuf+strlen(sqlbuf)," order by a.name limit %lu,%lu ",lStart,lRowNum);
    
//    printf("sqlbuf=%s\n",sqlbuf);
    psCur=pasDbOpenSqlF(sqlbuf);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    lSid=0;
    lCtype=0;
    memset(caName,0,sizeof(caName));
    lStime=0;
    lEtime=0;
    lPid=0;
    lDid=0;
    memset(caDname,0,sizeof(caDname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lCtype,
      	                           UT_TYPE_STRING,31,caName,
      	                           UT_TYPE_LONG,4,&lStime,
      	                           UT_TYPE_LONG,4,&lEtime,
      	                           UT_TYPE_LONG,4,&lPid,
      	                           UT_TYPE_LONG,4,&lDid,
      	                           UT_TYPE_STRING,30,caDname);
      	iNum=0;
      	while(iReturn==0||iReturn==1405){
      		iNum++;
      		utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
      		utPltPutLoopVarF(psDbHead,"num",iNum,"%u",iNum);
      		utPltPutLoopVar(psDbHead,"pname",iNum,caName);
      		utPltPutLoopVar(psDbHead,"stime",iNum, ncLimCvtTime(lStime));
          utPltPutLoopVar(psDbHead,"etime",iNum, ncLimCvtTime(lEtime));                    
          if(lCtype==0)
           utPltPutLoopVar(psDbHead,"ctldesc",iNum,ncLang("1419禁止使用"));
          else if(lCtype==1)
           utPltPutLoopVar(psDbHead,"ctldesc",iNum,ncLang("1420允许使用"));
           
          utPltPutLoopVarF(psDbHead,"ctype",iNum,"%u",lCtype);
          utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
          
          utPltPutLoopVarF(psDbHead,"did",iNum,"%d",lDid);
          if(lDid>0)
         		utPltPutLoopVar(psDbHead,"datename",iNum, caDname);
          else
          	utPltPutLoopVar(psDbHead,"datename",iNum, ncLang("0383每天"));
          
    lSid=0;
    lCtype=0;
    memset(caName,0,sizeof(caName));
    lStime=0;
    lEtime=0;
    lPid=0;
    lDid=0;
    memset(caDname,0,sizeof(caDname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lCtype,
      	                           UT_TYPE_STRING,31,caName,
      	                           UT_TYPE_LONG,4,&lStime,
      	                           UT_TYPE_LONG,4,&lEtime,
      	                           UT_TYPE_LONG,4,&lPid,
      	                           UT_TYPE_LONG,4,&lDid,
      	                           UT_TYPE_STRING,30,caDname);
     }
     pasDbCloseCursor(psCur);
     
    lTotRec=lCount;
    lTotPg = (lTotRec ) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }


    pBuf = pasUtlCvtSqlSelectA("select name,id from nclimdatedes order by name",ncLang("0384每天,0;"));
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("did","0",pBuf);
        free(pBuf);
    }
    utPltPutVar(psDbHead,"dateid", pSelect);
    free(pSelect);


    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
     
     
  
    utPltPutVar(psDbHead,"stime","00:00:00");
    utPltPutVar(psDbHead,"etime","23:59:59");
 
    utPltPutVarF(psDbHead,"gtype","%lu",lGtype);
    utPltPutVarF(psDbHead,"id","%lu",lId);
  
  
  //显示进程集
    psCur=pasDbOpenSqlF("select id,name from nwprocclass order by name ");
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    lPid=0;
    memset(caPname,0,sizeof(caPname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lPid,
      	                           UT_TYPE_STRING,31,caPname);
      	                         
      	iNum=0;
      	while(iReturn==0||iReturn==1405){
      		iNum++;
      		utPltPutLoopVarF(psDbHead,"id",iNum,"%lu",lPid);

      		utPltPutLoopVar(psDbHead,"name",iNum,caPname);

    
    lPid=0;
    memset(caPname,0,sizeof(caPname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lPid,
      	                           UT_TYPE_STRING,31,caPname);
     }
     pasDbCloseCursor(psCur);
  
  
  
  if(mod_flag>0){
  	 
        	ncSetLastInfo_l("ProcCtlLastTime",time(0));
      
  }
  
  
   utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nw_lim_proclist.htm");
    
    return 0;
}


	struct nwallprocrule_s{
 		int sid;
 		int limtype;   //1-nwctlproc 2-nwctlproclist 
 		int ctype;     //0=禁止使用 1－允许使用 
 		char flag_desc[32];
 		char name[64]; //策略对象
 		int objid;     //对象ID
 		int lGtype;
 		int timetype;  //时间类型 0 每天，其余ncLimGetDateName(timetype));
 		char time_desc[32];  //时间描述
 		char content[128];
 	};
 	
 	static long iOrderrule;
 	
 	static int ncUtlSortProcRule(struct nwallprocrule_s *p1,struct nwallprocrule_s *p2)
{
	if(iOrderrule==1)
	return (p1->objid-p2->objid);
	else
	return (p1->lGtype-p2->lGtype);

}
 	
//查询全部进程规则
int nwProcRuleSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caWhere[512],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caSort[16],caSortby[6];
    char caUsername[32],caGroupname[32],caMac[32],caIp[32],caDate[32];
    char caGroupid[16];
    char caDispname[36];
    char caSql[2024];
    long lLocalflag,lUsestatus;   
    char caEmail[128];
    char caPosition[36],caIdfyno[20];
    char idfytype[4],idfyunit[64],idfytype_desc[32];
    long dservice,dmask,urate,drate,dateid;
    char *pSelect,*pBuf;
    unsigned long lUserid,lCurPg,lTotPg,lCount,lIp,lAddtime,lExpiretime;
    int i,iNum;
    char *pSear;
    char caSmt[2512],caOrder[64];
    utPltDbHead *psDbHead;
   	int iReturn,lAddtime1;
   	long lGroupid,lRowNum,lUseflags,lStartRec,lUseflags0,lTotRec;
   	char caLevel[16];
   	char caTemp[4024];
    long lCtype;
    char caProcname[32];
    long lImrecord,lScreenrecord;
 	pasDbCursor *psCur;
 	
 
 	long lSum,lSid,lGtype,lId,lNum,lTrantype,lService,lLowerport,lUpperport,lTimetype,lStarttime,lStoptime,lConflags;
 	unsigned long lLowerip,lUpperip;
 	char caSt[30],caEt[30];
 	struct nwallprocrule_s *pRule;
 	long iMax,lFlags;
 	char caSip[20],caDip[20];
 	char caUrl[64];
 	long lUrlid;
 	char caName[64];
 	char temp_str[1024];
 	char caUserid[32];
 	char caApp[16];
 	char caUserid2[32],caGroupid2[32];
 	long m;
 	nwCompInfo *psComp;
 	long lIpaccess,lHttpaccess,lPostsend,lHttpbyip,lLimitunit,lFlowlimit,lTimelimit,lOverctl,lOverid,lIpbind,lMacbind,lNamebind,lOnlyone,lPolid,lPkgmax,lFlowmax,lIpmax,lMailmax,lPkgflags,lAccflags,lHttp80chk;
 	lSum=0;
 	iMax=9096;
 	iReturn = utMsgGetSomeNVar(psMsgHead,6,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					        "CurPg",      UT_TYPE_STRING, 12, caCurPg,
					        "groupid",    UT_TYPE_STRING, 12, caGroupid,
   	              "cond",       UT_TYPE_STRING, 63, caCond,
   	              "app",     UT_TYPE_STRING,12,caApp,
   	               "userid",       UT_TYPE_STRING, 12, caUserid);
 	  strcpy(caGroupid2,caGroupid);
 	  strcpy(caUserid2,caUserid);
 	  lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
 	
 	  lRowNum = 20;
    lStartRec = (lCurPg - 1) * lRowNum;    
 	
 	
 	  if(!utStrIsSpaces(caApp)) { /* 应用  */
  	nwUtlLoadProcCtl(psShmHead);
    }
 	
 	
 	
 	
 	pRule=(struct nwallprocrule_s*)malloc(iMax*sizeof(struct nwallprocrule_s));
 	if(pRule==NULL){
 		 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),ncLang("0404内存出错"));
 		 return 0;
 	}
 	psCur = pasDbOpenSqlF("select sid,stype,uid,ctype,did,stime,etime,procname from nwctlproc where 1=1 "); 
 	if(psCur==NULL){
 		 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),"open nclimip error");
 		 return 0;
 	}
 	lSid=0;
 	lGtype=0;
 	lId=0;
 	lCtype=0;
 	lTimetype=0;
 	lStarttime=0;
 	lStoptime=0;
 	memset(caProcname,0,sizeof(caProcname));
 	 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
 	                                UT_TYPE_LONG,4,&lGtype,
 	                                UT_TYPE_LONG,4,&lId,
 	                                UT_TYPE_LONG,4,&lCtype,
 	                                UT_TYPE_LONG,4,&lTimetype,
 	                                UT_TYPE_LONG,4,&lStarttime,
 	                                UT_TYPE_LONG,4,&lStoptime,
 	                                UT_TYPE_STRING,31,caProcname);
 	                                                       
	    while((iReturn==0)||(iReturn==1405))
	    {
	    	 pRule[lSum].lGtype=lGtype;
	    	 pRule[lSum].sid=lSid;
	    	 pRule[lSum].limtype=1;

	    	 pRule[lSum].ctype=lCtype;
	    	 
	    	 if(lCtype==0)
	    	  strcpy(pRule[lSum].flag_desc,ncLang("1419禁止使用"));
	    	 else if(lCtype==1)
	    	   strcpy(pRule[lSum].flag_desc,ncLang("1420允许使用"));
	    	 
	    	 if(lGtype==3)
	    	  strcpy(pRule[lSum].name,ncLang("0446全部"));
	    	 else if(lGtype==2)
	    	  strcpy(pRule[lSum].name,ncUtlGetGroupNameByid(psShmHead,lId,ncLang("0629未知")));
	    	 else if(lGtype==1){
	    	 	 psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lId);
	    	 	 if(psComp)
	    	      strcpy(pRule[lSum].name,psComp->devname);
	    	   else
	    	      strcpy(pRule[lSum].name,"");
	    	}
	    	 else
	    	  strcpy(pRule[lSum].name,"");
	    	  
	    	 pRule[lSum].objid=lId;
	    	 pRule[lSum].timetype=lTimetype;
	    	 
	    	 strcpy(caSt,ncLimCvtTime(lStarttime));
	    	 strcpy(caEt,ncLimCvtTime(lStoptime));
	    	 sprintf(pRule[lSum].time_desc,"%s--%s",caSt,caEt);
	    	 
	    	 sprintf(pRule[lSum].content,ncLang("1421进程%s"),caProcname);
	    	 
          
          lSum++;
          if(lSum>iMax){
          	iMax=iMax+128;
          	pRule=realloc(pRule,iMax*sizeof(struct nwallprocrule_s));
          }
			 	lSid=0;
			 	lGtype=0;
			 	lId=0;
			 	lCtype=0;
			 	lTimetype=0;
			 	lStarttime=0;
			 	lStoptime=0;
			 	memset(caProcname,0,sizeof(caProcname));
			 	 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
			 	                                UT_TYPE_LONG,4,&lGtype,
			 	                                UT_TYPE_LONG,4,&lId,
			 	                                UT_TYPE_LONG,4,&lCtype,
			 	                                UT_TYPE_LONG,4,&lTimetype,
			 	                                UT_TYPE_LONG,4,&lStarttime,
			 	                                UT_TYPE_LONG,4,&lStoptime,
			 	                                UT_TYPE_STRING,31,caProcname);
	    	 
	    }
	    pasDbCloseCursor(psCur);
	    
	    
	    
//进程集策略  
 psCur = pasDbOpenSqlF("select sid,stype,uid,ctype,did,stime,etime,name from nwctlproclist,nwprocclass where pid=id "); 
 	if(psCur==NULL){
 		 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),"open nwctlproclist error");
 		 return 0;
 	}
 lSid=0;
 lGtype=0;
 lId=0;
 lCtype=0;
 lStarttime=0;
 lStoptime=0;
 	memset(caName,0,sizeof(caName));
 	 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
 	                                UT_TYPE_LONG,4,&lGtype,
 	                                UT_TYPE_LONG,4,&lId,
 	                                UT_TYPE_LONG,4,&lCtype,
 	                                UT_TYPE_LONG,4,&lTimetype,
 	                                UT_TYPE_LONG,4,&lStarttime,
 	                                UT_TYPE_LONG,4,&lStoptime,
 	                                UT_TYPE_STRING,32,caName);                             
	    while((iReturn==0)||(iReturn==1405))
	    {
	    	 pRule[lSum].lGtype=lGtype;
	    	 pRule[lSum].sid=lSid;
	    	 pRule[lSum].limtype=2;

	    	 pRule[lSum].ctype=lCtype;
	    	 
	    	 if(lCtype==0)
	    	  strcpy(pRule[lSum].flag_desc,ncLang("1419禁止使用"));
	    	 else if(lCtype==1)
	    	   strcpy(pRule[lSum].flag_desc,ncLang("1420允许使用"));
	    	 if(lGtype==3)
	    	  strcpy(pRule[lSum].name,ncLang("0446全部"));
	    	 else if(lGtype==2)
	    	  strcpy(pRule[lSum].name,ncUtlGetGroupNameByid(psShmHead,lId,ncLang("0629未知")));
	    	 else if(lGtype==1){
	    	 	 psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lId);
	    	 	 if(psComp)
	    	      strcpy(pRule[lSum].name,psComp->devname);
	    	   else
	    	      strcpy(pRule[lSum].name,"");
	    	}
	    	 else
	    	  strcpy(pRule[lSum].name,"");
	    	  
	    	 pRule[lSum].objid=lId;
	    	 pRule[lSum].timetype=lTimetype;
	    	 
	    	 strcpy(caSt,ncLimCvtTime(lStarttime));
	    	 strcpy(caEt,ncLimCvtTime(lStoptime));
	    	 sprintf(pRule[lSum].time_desc,"%s--%s",caSt,caEt);
	    	 
	    	 sprintf(pRule[lSum].content,ncLang("1422进程集%s"),caName);
	    	
         
          
          
          lSum++;
          if(lSum>iMax){
          	iMax=128+lSum;
          	pRule=realloc(pRule,sizeof(struct nwallprocrule_s) * (lSum + 128));
          }
 lSid=0;
 lGtype=0;
 lId=0;
 lCtype=0;
 lStarttime=0;
 lStoptime=0;
 	memset(caName,0,sizeof(caName));
 	 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
 	                                UT_TYPE_LONG,4,&lGtype,
 	                                UT_TYPE_LONG,4,&lId,
 	                                UT_TYPE_LONG,4,&lCtype,
 	                                UT_TYPE_LONG,4,&lTimetype,
 	                                UT_TYPE_LONG,4,&lStarttime,
 	                                UT_TYPE_LONG,4,&lStoptime,
 	                                UT_TYPE_STRING,32,caName); 
	    	 
	    }
	    pasDbCloseCursor(psCur);
	    
	    
	    
//客户端监视策略	    
	    
	 psCur = pasDbOpenSqlF("select sid,stype,uid,imrecord,screenrecord from nwctlwatch where 1=1 "); 
 	if(psCur==NULL){
 		 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),"open nwctlwatch error");
 		 return 0;
 	}
 	lSid=0;
 	lGtype=0;
 	lId=0;
 
  lImrecord=0;
  lScreenrecord=0;
 	 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
 	                                UT_TYPE_LONG,4,&lGtype,
 	                                UT_TYPE_LONG,4,&lId,
 	                                UT_TYPE_LONG,4,&lImrecord,
 	                                UT_TYPE_LONG,4,&lScreenrecord);
 	                               
 	                                                       
	    while((iReturn==0)||(iReturn==1405))
	    {
	    	 pRule[lSum].lGtype=lGtype;
	    	 pRule[lSum].sid=lSid;
	    	 pRule[lSum].limtype=3;

	    	 pRule[lSum].ctype=0;
	    	 
	    	 strcpy(pRule[lSum].flag_desc,"");
	    	 if(lImrecord==0)
	    	  strcpy(pRule[lSum].content,ncLang("1423不监控客户端即时通讯"));
	    	 else if(lImrecord==1)
	    	   strcpy(pRule[lSum].content,ncLang("1424监控客户端即时通讯"));
	    	 else 
	    	   strcpy(pRule[lSum].content,ncLang("1425客户端即时通讯监控策略未设置"));
	    	 
	    	 
	    	 if(lScreenrecord==0)
	    	   sprintf(pRule[lSum].content+strlen(pRule[lSum].content),ncLang("1426,不监控客户端屏幕"));
	    	  else if(lScreenrecord==1)
	    	    sprintf(pRule[lSum].content+strlen(pRule[lSum].content),ncLang("1427,监控客户端屏幕"));
	    	  else
	    	    sprintf(pRule[lSum].content+strlen(pRule[lSum].content),ncLang("1428,客户端屏幕监控策略未设置"));
	    	 
	    	 if(lGtype==3)
	    	  strcpy(pRule[lSum].name,ncLang("0446全部"));
	    	 else if(lGtype==2)
	    	  strcpy(pRule[lSum].name,ncUtlGetGroupNameByid(psShmHead,lId,ncLang("0629未知")));
	    	 else if(lGtype==1){
	    	 	 psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lId);
	    	 	 if(psComp)
	    	      strcpy(pRule[lSum].name,psComp->devname);
	    	   else
	    	      strcpy(pRule[lSum].name,"");
	    	}
	    	 else
	    	  strcpy(pRule[lSum].name,"");
	    	  
	    	 pRule[lSum].objid=lId;
	    	 pRule[lSum].timetype=0;
	    	 

	    	 strcpy(pRule[lSum].time_desc,"");
	    	 
	    	
          
          lSum++;
          if(lSum>iMax){
          	iMax=iMax+128;
          	pRule=realloc(pRule,iMax*sizeof(struct nwallprocrule_s));
          }
			 	lSid=0;
			 	lGtype=0;
			 	lId=0;
			 	lCtype=0;
			 	lTimetype=0;
			 	lStarttime=0;
			 	lStoptime=0;
  lImrecord=0;
  lScreenrecord=0;
 	 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
 	                                UT_TYPE_LONG,4,&lGtype,
 	                                UT_TYPE_LONG,4,&lId,
 	                                UT_TYPE_LONG,4,&lImrecord,
 	                                UT_TYPE_LONG,4,&lScreenrecord);
	    	 
	    }
	    pasDbCloseCursor(psCur);    
	    
	    
	 
	 //准入规则  
 psCur = pasDbOpenSqlF("select sid,stype,uid,ctype,did,stime,etime,name from nwctljoinlist,nwjoinclass where pid=id "); 
 	if(psCur==NULL){
 		 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),"open nwctlproclist error");
 		 return 0;
 	}
 lSid=0;
 lGtype=0;
 lId=0;
 lCtype=0;
 lStarttime=0;
 lStoptime=0;
 	memset(caName,0,sizeof(caName));
 	 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
 	                                UT_TYPE_LONG,4,&lGtype,
 	                                UT_TYPE_LONG,4,&lId,
 	                                UT_TYPE_LONG,4,&lCtype,
 	                                UT_TYPE_LONG,4,&lTimetype,
 	                                UT_TYPE_LONG,4,&lStarttime,
 	                                UT_TYPE_LONG,4,&lStoptime,
 	                                UT_TYPE_STRING,32,caName);                             
	    while((iReturn==0)||(iReturn==1405))
	    {
	    	 pRule[lSum].lGtype=lGtype;
	    	 pRule[lSum].sid=lSid;
	    	 pRule[lSum].limtype=4;

	    	 pRule[lSum].ctype=lCtype;
	    	 
	    	 if(lCtype==0)
	    	  strcpy(pRule[lSum].flag_desc,ncLang("1429启用准入规则"));
	    	 else if(lCtype==1)
	    	   strcpy(pRule[lSum].flag_desc,ncLang("1430停止使用准入规则"));
	    	 if(lGtype==3)
	    	  strcpy(pRule[lSum].name,ncLang("0446全部"));
	    	 else if(lGtype==2)
	    	  strcpy(pRule[lSum].name,ncUtlGetGroupNameByid(psShmHead,lId,ncLang("0629未知")));
	    	 else if(lGtype==1){
	    	 	 psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lId);
	    	 	 if(psComp)
	    	      strcpy(pRule[lSum].name,psComp->devname);
	    	   else
	    	      strcpy(pRule[lSum].name,"");
	    	}
	    	 else
	    	  strcpy(pRule[lSum].name,"");
	    	  
	    	 pRule[lSum].objid=lId;
	    	 pRule[lSum].timetype=lTimetype;
	    	 
	    	 strcpy(caSt,ncLimCvtTime(lStarttime));
	    	 strcpy(caEt,ncLimCvtTime(lStoptime));
	    	 sprintf(pRule[lSum].time_desc,"%s--%s",caSt,caEt);
	    	 
	    	 sprintf(pRule[lSum].content,"%s",caName);
	    	
         
          
          
          lSum++;
          if(lSum>iMax){
          	iMax=128+lSum;
          	pRule=realloc(pRule,sizeof(struct nwallprocrule_s) * (lSum + 128));
          }
 lSid=0;
 lGtype=0;
 lId=0;
 lCtype=0;
 lStarttime=0;
 lStoptime=0;
 	memset(caName,0,sizeof(caName));
 	 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
 	                                UT_TYPE_LONG,4,&lGtype,
 	                                UT_TYPE_LONG,4,&lId,
 	                                UT_TYPE_LONG,4,&lCtype,
 	                                UT_TYPE_LONG,4,&lTimetype,
 	                                UT_TYPE_LONG,4,&lStarttime,
 	                                UT_TYPE_LONG,4,&lStoptime,
 	                                UT_TYPE_STRING,32,caName); 
	    	 
	    }
	    pasDbCloseCursor(psCur);
	    
	 
	 
	    
	    
 	  
   	  
   	               
 if((strlen(caUserid)>0)||strlen(caGroupid)>0)
  iOrderrule=2;
  else 
  iOrderrule=1;  	               

qsort(pRule, lSum,sizeof(struct nwallprocrule_s),ncUtlSortProcRule);







psDbHead = utPltInitDb();



iNum=0;
m=0;
if(atol(caUserid)>0){
	sprintf(caTemp,"select groupid from ncuser where userid=%s ",caUserid);
	printf("caTemp=%s\n",caTemp);
	lGroupid=-1;
	iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lGroupid);
	if(lGroupid>=0)
	  sprintf(caGroupid,"%d",lGroupid);
	  
	  
}
for(i=0;i<lSum;i++){
	
	if((strlen(caGroupid)>0&&pRule[i].lGtype==2&&pRule[i].objid==atol(caGroupid))||(strlen(caGroupid)==0&&strlen(caUserid)==0)||(strlen(caUserid)>0&&pRule[i].lGtype==1&&pRule[i].objid==atol(caUserid))||pRule[i].lGtype==3)
	if(strstr(pRule[i].content,caCond)||strlen(caCond)==0){
		if(m>=lStartRec&&m<lStartRec+lRowNum)
		{
	    m++;
			utPltPutLoopVarF(psDbHead,"sid",iNum+1,"%d",pRule[i].sid);
			utPltPutLoopVarF(psDbHead,"limtype",iNum+1,"%d",pRule[i].limtype);

			utPltPutLoopVar(psDbHead,"name",iNum+1,pRule[i].name);
			utPltPutLoopVar(psDbHead,"time_desc",iNum+1,pRule[i].time_desc);
			if(strlen(pRule[i].time_desc)>0)
				utPltPutLoopVar(psDbHead,"timetype",iNum+1,ncLimGetDateName(pRule[i].timetype));
				
				
			utPltPutLoopVar(psDbHead,"content",iNum+1,pRule[i].content);
			
			if(pRule[i].lGtype==3)
			 utPltPutLoopVar(psDbHead,"gtype",iNum+1,ncLang("0446全部"));
			else if(pRule[i].lGtype==2)
			 utPltPutLoopVar(psDbHead,"gtype",iNum+1,ncLang("0899组"));
			else if(pRule[i].lGtype==1)
			 utPltPutLoopVar(psDbHead,"gtype",iNum+1,ncLang("1612个人"));
			 
			 utPltPutLoopVar(psDbHead,"llimctl",iNum+1, pRule[i].flag_desc);
		
			 utPltPutLoopVarF(psDbHead,"num1",iNum+1,"%d",iNum);
	     iNum++;
	    }
	    else
	      m++;
	   }
}

   lTotRec=m;
   lTotPg = m / lRowNum + 1;
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
   	               
utPltPutVar(psDbHead,"userid",caUserid2);
utPltPutVar(psDbHead,"groupid",caGroupid2);
utPltPutVar(psDbHead,"cond",caCond);
if(pRule) free(pRule);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_allprocrule_search.htm");
    return 0;
}


//删除进程规则
int ncWebProcRuleDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char caTemp[1024];
	char caVar1[32],caVar2[33];
	char *p,*p1;
	int i,iReturn;
  int del_flag=0;
        for(i=0;i<26 && i < 50;i++) {
            sprintf(caVar1,"ch%d",i);
            sprintf(caVar2,"typeindex%d",i);
            p = utMsgGetVar_s(psMsgHead,caVar1);
            p1=utMsgGetVar_s(psMsgHead,caVar2);
            if((p)&&(p1)) {

                if(atol(p1)==1)
                {
                	del_flag++;
                iReturn = pasDbExecSqlF("delete from nwctlproc where sid = %s",p);
                }
                else if(atol(p1)==2)
                {
                	 del_flag++;
                	 iReturn = pasDbExecSqlF("delete from nwctlproclist where sid = %s",p);
                }
                else if(atol(p1)==3)
                {
                	 del_flag++;
                	 iReturn = pasDbExecSqlF("delete from nwctlwatch where sid = %s",p);
                }
                else if(atol(p1)==4)
                {
                	 del_flag++;
                	 iReturn = pasDbExecSqlF("delete from nwctljoinlist where sid = %s",p);
                }
            }
        }
        if(del_flag>0){
        	ncSetLastInfo_l("ProcCtlLastTime",time(0));
        	pasDbCommit(NULL);
        }
        nwProcRuleSearch(psShmHead,iFd,psMsgHead);
}


/* 显示内网监视规则*/
int nwWebLim_Watchlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caMessage[556],caMessage1[556];
    char caMod[16],caDel[16],caAdd[16],caApp[16],caCurPg[16];
    char caSum[16],caGtype[16],caFlags[16],caId[16],caRnum[16],caService[16];
    char caSid[16],caEtime[16],caStime[16];
    char caUrlid[24];
   
    char *pBuf,*pSelect,*p;
    unsigned long lLip;
    long  lStart,lStartRec,lCurPg,lTotPg;
    long  lRnum,lSum,lGtype,lFlags,lId,lDateid,lLimctl,lStime,lEtime,lSid,lService;
    long  lCount,lRowNum;
    int   iReturn,i,lPid;
    char caImrecord[12],caScreenwatch[12];
    long lImrecord,lScreenrecord;
    char sqlbuf[1024];

    utPltDbHead *psDbHead;
    int mod_flag=0;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,9,
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All  */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
	                "add",     UT_TYPE_STRING, 12, caAdd,
	                "mod",     UT_TYPE_STRING, 12, caMod,
	                "del",     UT_TYPE_STRING, 12, caDel,
	                "app",     UT_TYPE_STRING,12,caApp,
	                "sid",     UT_TYPE_STRING, 12, caSid,
	                "imrecord",  UT_TYPE_STRING, 8, caImrecord,
	                "screenwatch",     UT_TYPE_STRING, 8, caScreenwatch);         
	               
	                

    lGtype = atol(caGtype);   
    lId = atol(caId);
    lImrecord=atol(caImrecord);
 lScreenrecord=atol(caScreenwatch);
    if((!utStrIsSpaces(caAdd))||!utStrIsSpaces(caMod)) {  /* 增加记录或修改 */
       
      pasDbExecSqlF("delete from nwctlwatch where stype=%d and uid=%d ",lGtype,lId);
       
       iReturn = pasDbExecSqlF("insert into nwctlwatch(sid,stype,uid,imrecord,screenrecord,mtime)\
                      values(%lu,%d,%lu,%d,%d,%d)", \
                     pasGetSid("nwctlwatch","sid"),lGtype,lId,lImrecord,lScreenrecord,time(0));
                if(iReturn != 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0310记录增加出错"));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                	mod_flag++;
                    pasDbCommit(NULL);
                 
                }
            
    }
    else if(!utStrIsSpaces(caApp)) { /* 应用  */
  	nwUtlLoadProcCtl(psShmHead);
    }
    psDbHead = utPltInitDb();
    
  
    
    sprintf(sqlbuf,"select sid,imrecord,screenrecord from nwctlwatch where  stype=%d ",lGtype);
    if(lId>0&&lGtype<3){
    	sprintf(sqlbuf+strlen(sqlbuf)," and uid=%lu ",lId);
    }
    iReturn=-1;
  iReturn=pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lSid,
                          UT_TYPE_LONG,4,&lImrecord,
                          UT_TYPE_LONG,4,&lScreenrecord);
//    printf("sqlbuf=%s\n",sqlbuf);
  if(iReturn==0){
  	
  	  utPltPutVarF(psDbHead,"imrecord","%d",lImrecord);
  	  utPltPutVarF(psDbHead,"screenwatch","%d",lScreenrecord);
  }
  else{
  	
  	utPltPutVar(psDbHead,"imcheck","9");
  	utPltPutVar(psDbHead,"screenrecord","9");
   
  }
   
 
    utPltPutVarF(psDbHead,"gtype","%lu",lGtype);
    utPltPutVarF(psDbHead,"id","%lu",lId);
  
  
  
  
  
  if(mod_flag>0){
  	 
        	ncSetLastInfo_l("ProcCtlLastTime",time(0));
      
  }
  
  
   utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nw_lim_watchlist.htm");
    
    return 0;
}


/* 显示进准入规则*/
int nwWebLim_Joinlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caMessage[556],caMessage1[556];
    char caMod[16],caDel[16],caAdd[16],caApp[16],caCurPg[16];
    char caSum[16],caGtype[16],caFlags[16],caId[16],caRnum[16],caService[16];
    char caSid[16],caEtime[16],caStime[16];
    char caUrlid[24];
   
    char *pBuf,*pSelect,*p;
    unsigned long lLip;
    long  lStart,lStartRec,lCurPg,lTotPg;
    long  lRnum,lSum,lGtype,lFlags,lId,lDateid,lLimctl,lStime,lEtime,lSid,lService;
    long  lCount,lRowNum;
    int   iReturn,i,lPid;
    char caCtype[12],caDid[16],caPid[64];
    long lDid,lCtype,iNum,lTotRec;
    char sqlbuf[1024];
    char caName[32];
    char caPname[32],caDescr[256],caUrlpath[128];
    struct nwJoindesc_s{
    	int id;
    	char caDescr[256];
    	char caUrlpath[128];
    };
    struct nwJoindesc_s stData[12];
    pasDbCursor *psCur;
    long lSum_d;
    utPltDbHead *psDbHead;
    int mod_flag=0;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,15,
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All  */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
	                "add",     UT_TYPE_STRING, 12, caAdd,
	                "mod",     UT_TYPE_STRING, 12, caMod,
	                "del",     UT_TYPE_STRING, 12, caDel,
	                "app",     UT_TYPE_STRING,12,caApp,
	                "sid",     UT_TYPE_STRING, 12, caSid,
	                "ctype",  UT_TYPE_STRING, 8, caCtype,
	                "did",     UT_TYPE_STRING, 12, caDid,
	                "pid",UT_TYPE_STRING,63,caPid,
	                "etime",      UT_TYPE_STRING, 8,  caEtime,
	                "stime",      UT_TYPE_STRING, 8,  caStime,	              
	                "CurPg",     UT_TYPE_STRING, 12, caCurPg,
	                "descr",     UT_TYPE_STRING,255,caDescr,
	                "urlpath",   UT_TYPE_STRING,127,caUrlpath);
	                
	lCurPg = atol(caCurPg);
	if(lCurPg == 0) lCurPg = 1;
	lRowNum = 24;
	lStart = (lCurPg - 1) * lRowNum;          
	
    lGtype = atol(caGtype);
   
    lId = atol(caId);
    lDid = atol(caDid);
    lCtype = atol(caCtype);
    lPid=atol(caPid);
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
       
       lStime = ncUtlIsValidTime(caStime);
       lEtime = ncUtlIsValidTime(caEtime);
       if(lStime < 0 || lEtime < 0) {
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0435起始或终止时间不正确"));
           return 0;
       }
       
       iReturn = pasDbExecSqlF("insert into nwctljoinlist(sid,stype,uid,ctype,pid,did,stime,etime,mtime)\
                      values(%lu,%d,%lu,%d,%d,%lu,%lu,%lu,%d)", \
                     pasGetSid("nwctljoinlist","sid"),lGtype,lId,lCtype,lPid,lDid,lStime,lEtime,time(0));
                if(iReturn != 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0314记录增加出错, 可能为记录重复"));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                	mod_flag++;
                    pasDbCommit(NULL);
 //将客户端准入描述插入到表里
                    if(strlen(caDescr)>0){
                    	pasDbExecSqlF("insert into ncjoindesc (id,descr,urlpath) values(%d,'%s','%s')",lPid,caDescr,caUrlpath);
                    }
                 
                }
            
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
   	    for(i=0;i<24;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
   	                 mod_flag++;
   	                 pasDbExecSqlF("delete from nwjoinclassdet where id in (select pid from nwctljoinlist where sid=%s )",p);
   	                 pasDbExecSqlF("delete from ncjoindesc where id in (select pid from nwctljoinlist where sid=%s )",p);
                    iReturn = pasDbExecSqlF("delete from nwctljoinlist where sid = %s",p);
                    pasDbCommit(NULL);
                    
                }
               
   	       }
   	   
    }
    else if(!utStrIsSpaces(caMod)) { /* 修改记录  */
        lSid = atol(caSid);	 
        lStime = ncUtlIsValidTime(caStime);
        lEtime = ncUtlIsValidTime(caEtime);     
        iReturn = pasDbExecSqlF("update nwctljoinlist set stype=%lu,uid=%lu,ctype=%lu,pid=%d,stime=%lu,etime=%lu,mtime=%lu where sid = %lu",
         lGtype,lId,lCtype,lPid,lStime,lEtime,time(0),lSid);
                          
                if(iReturn != 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0308记录修改出错, 可能为记录重复 %s"),pasDbErrorMsg(NULL));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                	mod_flag++;
                    pasDbCommit(NULL);
                    pasDbExecSqlF("update ncjoindesc set descr='%s',urlpath='%s' where id=%d ",caDescr,caUrlpath,lSid);
                
                }
                         
       
    }
    else if(!utStrIsSpaces(caApp)) { /* 应用  */
  	nwUtlLoadProcCtl(psShmHead);
  }
    psDbHead = utPltInitDb();
    
    
      psCur=pasDbOpenSqlF("select id,descr,urlpath from ncjoindesc limit 0,10 ");
     if(psCur!= NULL) {
       
    lSid=0;
    memset(caDescr,0,sizeof(caDescr));
    memset(caUrlpath,0,sizeof(caUrlpath));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_STRING,255,caDescr,
      	                           UT_TYPE_STRING,127,caUrlpath);
      	                         
      	lSum_d=0;
      	while(iReturn==0||iReturn==1405){
      		stData[lSum_d].id=lSid;
      		strcpy(stData[lSum_d].caDescr,caDescr);
      		strcpy(stData[lSum_d].caUrlpath,caUrlpath);
      		lSum_d++;
			    lSid=0;
			    memset(caDescr,0,sizeof(caDescr));
			    memset(caUrlpath,0,sizeof(caUrlpath));
			    iReturn = pasDbFetchInto(psCur,
			      	                           UT_TYPE_LONG,4,&lSid,
			      	                           UT_TYPE_STRING,255,caDescr,
			      	                           UT_TYPE_STRING,127,caUrlpath);
     }
     pasDbCloseCursor(psCur);
    
  }
    
    
    
    
    
    
    lCount = 0;
    
    sprintf(sqlbuf,"select count(*) from nwctljoinlist,nwjoinclass  where pid=id and stype=%d ",lGtype);
    
    if(lId>0&&lGtype<3){
    	sprintf(sqlbuf+strlen(sqlbuf)," and uid=%lu ",lId);
    }
    pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lCount);
    
    sprintf(sqlbuf,"select sid,ctype,name,stime,etime,pid from nwctljoinlist,nwjoinclass where pid=id and stype=%d ",lGtype);
    if(lId>0&&lGtype<3){
    	sprintf(sqlbuf+strlen(sqlbuf)," and uid=%lu ",lId);
    }
    sprintf(sqlbuf+strlen(sqlbuf)," order by name limit %lu,%lu ",lStart,lRowNum);
    
//    printf("sqlbuf=%s\n",sqlbuf);
    psCur=pasDbOpenSqlF(sqlbuf);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    lSid=0;
    lCtype=0;
    memset(caName,0,sizeof(caName));
    lStime=0;
    lEtime=0;
    lPid=0;
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lCtype,
      	                           UT_TYPE_STRING,31,caName,
      	                           UT_TYPE_LONG,4,&lStime,
      	                           UT_TYPE_LONG,4,&lEtime,
      	                           UT_TYPE_LONG,4,&lPid);
      	iNum=0;
      	while(iReturn==0||iReturn==1405){
      		iNum++;
      		utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
      		utPltPutLoopVarF(psDbHead,"num",iNum,"%u",iNum);
      		utPltPutLoopVar(psDbHead,"pname",iNum,caName);
      		utPltPutLoopVar(psDbHead,"stime",iNum, ncLimCvtTime(lStime));
          utPltPutLoopVar(psDbHead,"etime",iNum, ncLimCvtTime(lEtime));                    
          if(lCtype==0)
           utPltPutLoopVar(psDbHead,"ctldesc",iNum,ncLang("0437启用"));
          else if(lCtype==1)
           utPltPutLoopVar(psDbHead,"ctldesc",iNum,ncLang("1431停止使用"));
           
          utPltPutLoopVarF(psDbHead,"ctype",iNum,"%u",lCtype);
          utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
          
          for(i=0;i<lSum_d;i++){
          	if(lPid==stData[i].id){
          		utPltPutLoopVar(psDbHead,"descr",iNum,stData[i].caDescr);
          		utPltPutLoopVar(psDbHead,"urlpath",iNum,stData[i].caUrlpath);
          		break;
          	}
          }
        utPltPutLoopVar(psDbHead,"gtype",iNum,caGtype);
        utPltPutLoopVar(psDbHead,"uid",iNum,caId);
          
    lSid=0;
    lCtype=0;
    memset(caName,0,sizeof(caName));
    lStime=0;
    lEtime=0;
    lPid=0;
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lCtype,
      	                           UT_TYPE_STRING,31,caName,
      	                           UT_TYPE_LONG,4,&lStime,
      	                           UT_TYPE_LONG,4,&lEtime,
      	                           UT_TYPE_LONG,4,&lPid);
     }
     pasDbCloseCursor(psCur);
     
    lTotRec=lCount;
    lTotPg = (lTotRec ) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }

    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
     
     
  
    utPltPutVar(psDbHead,"stime","00:00:00");
    utPltPutVar(psDbHead,"etime","23:59:59");
 
    utPltPutVarF(psDbHead,"gtype","%lu",lGtype);
    utPltPutVarF(psDbHead,"id","%lu",lId);
  
  
  //显示准入类型
    psCur=pasDbOpenSqlF("select id,name from nwjoinclass order by name ");
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    lPid=0;
    memset(caPname,0,sizeof(caPname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lPid,
      	                           UT_TYPE_STRING,31,caPname);
      	                         
      	iNum=0;
      	while(iReturn==0||iReturn==1405){
      		iNum++;
      		utPltPutLoopVarF(psDbHead,"id",iNum,"%lu",lPid);

      		utPltPutLoopVar(psDbHead,"name",iNum,caPname);

    
    lPid=0;
    memset(caPname,0,sizeof(caPname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lPid,
      	                           UT_TYPE_STRING,31,caPname);
     }
     pasDbCloseCursor(psCur);
  
  
  
  if(mod_flag>0){
  	 
        	ncSetLastInfo_l("ProcCtlLastTime",time(0));
      
  }
  
  
   utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nw_lim_joinlist.htm");
    
    return 0;
}



/* 显示某种类别的准入规则*/
int ncWebJoinClassDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caWhere[255],caTemp[255];
    char caCond[64];
    char caTotPg[16],caCurPg[16],caTotRec[16];
    char caId[16],caSear[16],caModi[16],caDel[16],caSid[16],caAdd[16];
    char caName[64],caApp[16],caDescr[64];
    char caUip1[32],caLip1[32],caName1[64],caCname[64],caId1[16];
    long lId;
    char *p;
    int i,iNum,iReturn;
    char caCtl[8],caVersion[8];
    char caUid[12];
    char caGtype[10];
    long lCtl;
    char version_desc[16];
    utPltDbHead *psDbHead;
   	long lStartRec,lSumUser,lCount,lCurPg,lTotRec,lRowNum,lSid1,lId1,lLport1,lUport1;
   	unsigned long lSid,lTotPg;
   	int mod_flag=0;
 	pasDbCursor *psCur;

	iReturn = utMsgGetSomeNVar(psMsgHead,12,
	                "pid",      UT_TYPE_STRING, 12, caId,
	                "id",       UT_TYPE_STRING,10,caUid,
	                "add",        UT_TYPE_STRING, 12, caAdd,
	                "modi",       UT_TYPE_STRING, 12, caModi,
	                "del",        UT_TYPE_STRING, 12, caDel,
	                "sid",        UT_TYPE_STRING, 12, caSid,	
	                "gtype",      UT_TYPE_STRING,6,caGtype,         
	                "version",       UT_TYPE_STRING,4, caVersion,
	                "name2",       UT_TYPE_STRING, 32, caName,
	                "ctl",       UT_TYPE_STRING,1,caCtl,
					"CurPg",      UT_TYPE_STRING, 12, caCurPg,
					"TotRec",     UT_TYPE_STRING, 12, caTotRec);

    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
    	lCount=0;
       sprintf(caTemp,"select count(*) from nwjoinclassdet where id=%d and name='%s' ",caId,caName);
       pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
       if(lCount>0){
       	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0319记录重复"),ncLang("0319记录重复"),pasDbErrorMsg(NULL));
                pasDbRollback(NULL);
                return 0;
        }
        iReturn = pasDbExecSqlF("insert into nwjoinclassdet(sid,id,name,version) values (%lu,%lu,'%s','%s')",
                    pasGetSid("nwjoinclassdet","sid"),atol(caId),caName,caVersion);

            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0311记录增加出错,%s"),pasDbErrorMsg(NULL));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                strcpy(caTotPg,"\0");
                pasDbCommit(NULL);
                mod_flag++;
            }
      }    
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
   	    for(i=0;i<50;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
                iReturn = pasDbExecSqlF("delete from nwjoinclassdet where sid = %s",p);
                if(iReturn == 0) {
                    strcpy(caTotPg,"\0");
                    pasDbCommit(NULL);
                    mod_flag++;
                }
   	        }
   	    }
    }
    else if(!utStrIsSpaces(caModi)) { /* 修改记录  */
        if(!utStrIsSpaces(caSid)) {
           
            
           
            iReturn = pasDbExecSqlF("update nwjoinclassdet set id=%lu,name='%s',version='%s',ctl=%d where sid = %s",atol(caId),caName,caVersion,atol(caCtl),caSid);
            if(iReturn == 0) {
                pasDbCommit(NULL);
                mod_flag++;
            }
            else {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0303记录修改出错"));
                pasDbRollback(NULL);
            }
        }
        else if((strlen(caName)>0)&&(strlen(caId)>0)){
        	
			       lCount=0;
			       sprintf(caTemp,"select count(*) from nwjoinclassdet where id=%s and name='%s' ",caId,caName);
			       printf("caTemp=%s\n",caTemp);
			       pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
			       if(lCount>0){
			       	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0319记录重复"),ncLang("0319记录重复"),pasDbErrorMsg(NULL));
			                pasDbRollback(NULL);
			                return 0;
			        }
			       iReturn = pasDbExecSqlF("insert into nwjoinclassdet(sid,id,name,version) values (%lu,%lu,'%s','%s')",
			               pasGetSid("nwjoinclassdet","sid"),atol(caId),caName,caVersion);
			
			            if(iReturn != 0) {
			                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0311记录增加出错,%s"),pasDbErrorMsg(NULL));
			                pasDbRollback(NULL);
			                return 0;
			            }        
			            else {
			                strcpy(caTotPg,"\0");
			                pasDbCommit(NULL);
			                mod_flag++;
			            }
        	
        }
    }
   
   psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"pid",caId);
    utPltPutVar(psDbHead,"id",caUid);
    utPltPutVar(psDbHead,"gtype",caGtype);
 sprintf(caTemp,"select name from nwjoinclass where id=%d ",atol(caId));
    pasDbOneRecord(caTemp,0,UT_TYPE_STRING,31,caName);
    utPltPutVar(psDbHead,"cname",caName);
    
   
    sprintf(caTemp,"select sid,id,name,version,ctl from nwjoinclassdet where id=%d order by sid ",atol(caId));
printf("caTemp=%s\n",caTemp);    
    psCur = pasDbOpenSqlF(caTemp);
         
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    
    
   
    lSid=0;
    lId=0;
    memset(caName,0,sizeof(caName));
    memset(caVersion,0,sizeof(caVersion));
    lCtl=0;
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lId,
      	                           UT_TYPE_STRING,31,caName,
      	                           UT_TYPE_STRING,9,caVersion,
      	                           UT_TYPE_LONG,4,&lCtl);
      	                          
  	                              
   	iNum = 0;
   	while(iReturn == 0 ||iReturn==1405) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%u",iNum);
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"id",    iNum,"%lu", lId);

        
        utPltPutLoopVar(psDbHead,"name",   iNum,caName);
        
        utPltPutLoopVar(psDbHead,"version",iNum,caVersion);
        if(strcmp(caVersion,"0")>0){
        	sprintf(version_desc,"SP%s",caVersion);
        	utPltPutLoopVar(psDbHead,"version_desc",iNum,version_desc);
        }
        utPltPutLoopVarF(psDbHead,"ctl",iNum,"%d",lCtl);
        
        if(lCtl==1)
          utPltPutLoopVar(psDbHead,"ctl_desc",iNum,ncLang("1313禁止上网"));
        else if(lCtl==0)
         utPltPutLoopVar(psDbHead,"ctl_desc",iNum,ncLang("1864允许上网"));
         
        
    lSid=0;
    lId=0;
    memset(caName,0,sizeof(caName));
    memset(caVersion,0,sizeof(caVersion));
    lCtl=0;
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lId,
      	                           UT_TYPE_STRING,31,caName,
      	                           UT_TYPE_STRING,9,caVersion,
      	                           UT_TYPE_LONG,4,&lCtl);
    }
    pasDbCloseCursor(psCur);
   
   
   
   if(mod_flag>0)
    ncSetLastInfo_l("ProcCtlLastTime",time(0));
    
//   utPltShowDb(psDbHead);
if(atol(caId)==2)
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_webjoinclassdet_czxt.htm");
else 
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_webjoinclassdet_fdrj.htm");
    
    return 0;
}



 /* 客户端自动升级设置*/
int ncWebUpdateDeskSoft(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    
    
  
 
    psDbHead = utPltInitDb();

    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880自动备份设定"),ncLang("0427配置文件出错"));
        return 0;
    }
    utPltPutVar(psDbHead,"proname",   pasUtlLookConfig(psConfig,"DeskSoft","proclient.exe"));
    utPltPutVar(psDbHead,"version", pasUtlLookConfig(psConfig,"DeskVersion","00.00.01"));
    pasUtlFreeConfig(psConfig);


    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nc_updatedesksoft.htm");
    return 0;
}



//保存客户端升级参数

int ncWebUpdateDeskSoftSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    
    if(p = utMsgGetVar_s(psMsgHead,"proname")) {
    
        pasUtlModiConfig(psConfig,"DeskSoft",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"version")) {
    	
        pasUtlModiConfig(psConfig,"DeskVersion",p);
    }
   pasUtlSaveConfig(pConfig,psConfig);
   pasUtlFreeConfig(psConfig);
    ncUtlReReadConFig(psShmHead);
          
    ncWebUpdateDeskSoft(psShmHead,iFd,psMsgHead);

    return 0;
}


//上传软件
int ncWebDeskSoft_upload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caCmd[12];
    char caDip[20];
    FILE *fp;
    utPltDbHead *psDbHead;
    unsigned long lSize;
    char caTemp[1024];
    char buf[4256];
    char caBuf[300];
    char caNum[10];
    char caFile2[128];
    long lNum;
    char *p,*p1;
    int i,iReturn;

 	char caFile[256];
 	char imp_file[129]="";
 	char temp_file[129]="";
  memset(caFile2,0,sizeof(caFile2));
   psDbHead = utPltInitDb();
   if(!utFileIsExist("/home/ncmysql/nc/upload")) {
        if(mkdir("/home/ncmysql/nc/upload",555)!=0){
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0194导入"),ncLang("0183打开文件出错"));
        return 0;

        }
   }

   system("chmod +777 /home/ncmysql/nc/upload");


	utMsgGetSomeNVar(psMsgHead,1,"fname",UT_TYPE_STRING,  255,imp_file);
	                           
 printf("imp_file=%s\n",imp_file);                    
  if(strlen(imp_file)!=0)
  {
  	p=utStrSkipSpaces(imp_file);
  	p=utStrGetWord(p,temp_file,200,";\n");
  	if((*p)==';')
  	p=utStrGetWord(p+1,temp_file,200,";\n");
  }
   p=strstr(temp_file,"/");
   while(p){
   	p1=strstr(p,"/");
   	if(p1==NULL){
   		strcpy(caFile2,p);
   	 break;
   	  }
   	p=p1+1;
  }
   

  

  sprintf(caTemp,"mv %s /home/ncmysql/nc/html/soft/%s ",temp_file,caFile2);
  printf("caTemp=%s\n",caTemp);
 iReturn= system(caTemp);

if(iReturn==0){
 sprintf(buf,ncLang("1432文件上传成功"));
//ncUrlSetUrlUpdate(psShmHead);
    
  }
  else{
  	sprintf(buf,ncLang("1433文件上传失败"));
  }
    
    utPltSetCvtHtml(1);

    utPltPutVar(psDbHead,"msg",buf);
    utPltSetCvtHtml(0);
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_tool_urlupload.htm");
    return 0;
}

//显示客户端升级日志
int ncShowDeskSoftUpdateLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)                  
{
    pasDbCursor *psCur;
    char caTemp[1055];
    char caTotPg[16];
    char caMesg[256],caCurPg[16];
    long lStime,iReturn,iNum;
    unsigned long lUserid,lStart,lSip;
    long lCount;

    long lCurPg,lRowNum,lStartRec,lTotPg,lPort,lGroupid0;

    utPltDbHead *psDbHead;

   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
					                                  "CurPg",       UT_TYPE_STRING, 12, caCurPg,
					                                  "TotPg",       UT_TYPE_STRING, 12, caTotPg);
					                                
			
    
    lCurPg = atol(caCurPg);
    lRowNum = 32;
    if(lCurPg <= 1){
    	 lCurPg = 1;
    	
    	}
    lStartRec = (lCurPg - 1) * lRowNum;
    if(lStartRec <= 0)lStartRec = 0;

   psDbHead = utPltInitDb();

 
   sprintf(caTemp,"select count(*) from nwsoftupdatalog"); 		  
	  
	 pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
 
    lTotPg = (lCount -1 ) / lRowNum + 1;
 
   
   
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
   
   
		
		 psCur = pasDbOpenSqlF("select stime,mesg from  nwsoftupdatalog order by sid desc limit %d,%d",lStartRec,32);
		
          
		    
		    if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
		        return 0;
		    }        
		
		    iReturn = 0;
		   
		    lStime=0;
		    memset(caMesg,0,sizeof(caMesg));
		    
	      iReturn = pasDbFetchInto(psCur,
  																 UT_TYPE_LONG,4,&lStime,
  																 UT_TYPE_STRING,250,caMesg);
      	                          

		    iNum = 0;                                   
		    while(iReturn == 0||iReturn==1405)  {
		    iNum++;
		    utPltPutLoopVar(psDbHead,"stime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lStime));
        
        utPltPutLoopVar(psDbHead,"mesg", iNum,caMesg);
            
		    lStime=0;
		    memset(caMesg,0,sizeof(caMesg));
		    
	      iReturn = pasDbFetchInto(psCur,
  																 UT_TYPE_LONG,4,&lStime,
  																 UT_TYPE_STRING,250,caMesg);

		                                  
		    
		  }
		    pasDbCloseCursor(psCur);
		    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_showSoftupdatelog.htm");
		    return 0;
		
}




/* 接收屏幕日志  */
int nwTcpScreenLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    nwCompIp       *psCompIp;
    FILE *fp;
    char caFile[128];
    char caPath[128];
    char caPath1[128];
    long lTime;
    char *p;
    int iNum,iReturn,i,k;
    unsigned long lCompId,lSum,lBytes,lSid,lIp,lSid0;
    unsigned short nPort;
#ifdef LDEBUG
    printf(" nwTcpScreenInfo  \n");
 //   utMsgPrintMsg(psMsgHead);
#endif
    /* 数据报校验  */
    lCompId = psMsgHead->lReserve[0];
    utComGetSockIp(iFd,&lIp,&nPort);
    lIp = ntohl(lIp);
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&lIp);
    if(psCompIp == NULL || psCompIp->psComp == NULL) {
#ifdef LDEBUG
        printf(" Not Online ....\n");
 //       utMsgPrintMsg(psMsgHead);
#endif
        return 0;
    }
    if(psCompIp->compid != lCompId) {
#ifdef LDEBUG
        printf(" CompId not equal ....\n");
#endif
        return 0;
    }
    if(!utMsgCheckSum(psMsgHead,psCompIp->caKey)) { /* 数据不合法  */
#ifdef LDEBUG
        printf(" It is invalid message, CheckSum Error\n");
#endif        
        return 0;
    }
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
   	                "sid",        UT_TYPE_LONG,4, &lSid,
   	                "sumbytes",   UT_TYPE_LONG,4, &lSum);
    if(lSum > 0) {
    	lTime=time(0);
    	   sprintf(caPath,"%s/screen",utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog"));
         if(!utFileIsExist(caPath)){
         	mkdir(caPath,555);
        }
        
         sprintf(caPath,"%s/screen/%s",utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog"),utTimFormat("%Y-%m-%d",lTime));
         if(!utFileIsExist(caPath)){
         	mkdir(caPath,555);
        }
        
         sprintf(caPath1,"%s/screen/%s/%d",utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog"),utTimFormat("%Y-%m-%d",lTime),psCompIp->compid);
         if(!utFileIsExist(caPath1)){
         	mkdir(caPath1,555);
        }
         sprintf(caPath,"%s/%s",caPath1,utTimFormat("%H",lTime));
         if(!utFileIsExist(caPath)){
         	mkdir(caPath,555);
        }
        
        
        sprintf(caFile,"%s/%lu.jpg",caPath,lTime);
 //       printf("caFile=%s\n",caFile);
             
#ifdef LDEBUG
       printf(" Write:%s \n",caFile);
#endif       
       lSid0 = 0;
        p =utMsgGetVar_v(psMsgHead,"content",&lBytes);
        if(p) {
            fp = fopen(caFile,"w");
            if(fp) {
                fwrite(p,lSum,1,fp);
                fclose(fp);
							//将记录写到数据库中
							  if(psCompIp->psComp){
							  	pasDbExecSqlF("insert into ncscreenlog(userid,compid,stime,file) values(%d,%d,%d,'%d.jpg')",psCompIp->psComp->userid,psCompIp->compid,lTime,lTime);
							  }
            }
          
        }
    }
  
    return 0;
}



/* 客户端屏幕的用户树 */
int ncWebTreeDispCompUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caLevel[16];
    char caPlate[128];
   	char caGroupname[32],caGroupid[16];
   	
   	char caName[32];
   	char caWhere[256];
   	char caSql[256];
   	char caCurPg[16],caTotRec[16];
   	char caPid[16];
   	unsigned long lLevel,lPid;
   	int  iNum;
  	char caFilter[256];
  	char caNo[16];
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
   	int iReturn,i;
    unsigned long lUserid,lCompid;
    char caDispname[36];
    long lTotPg;

#ifdef LDEBUG1 
    utMsgPrintMsg(psMsgHead);
#endif  	
char caTemp[256];
char caFname[64];

   	iReturn = utMsgGetSomeNVar(psMsgHead,6,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"groupid",        UT_TYPE_STRING,10,caPid,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"TotRec",     UT_TYPE_STRING, 12,  caTotRec,
					 "name",       UT_TYPE_STRING, 63,  caName,
					 "fname",      UT_TYPE_STRING, 31,  caFname);
				
			
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"System Error");
        return 0;
    }
    lGroupid = atol(caGroupid);
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
   if(iReturn == 0 && lGroupid0 > 0 && lGroupid != lGroupid0) {
        sprintf(caWhere,"and (b.groupid in (select groupid from ncgroup where groupid=%lu or pid=%lu)) ",lGroupid0,lGroupid0);
    }
    else if(strlen(caPid)>0){
    	 if(strcmp(caGroupid,"0")==0){
    		  	sprintf(caWhere," and b.groupid=0 ");
    		  }
    		  else
        	  sprintf(caWhere," and b.groupid  in (select groupid from ncgroup where groupid=%s or pid=%s)  ",caPid,caPid);
    	
    	
    }
    if(!utStrIsSpaces(caName)) {
    	
        utStrAddF(caWhere," and ( dispname like '%c%s%c' or username like '%c%s%c' or inet_ntoa(a.ip)='%s' or position like '%c%s%c' ) ",'%',caName,'%','%',caName,'%',caName,'%',caName,'%');
    }
// printf("caWhere=%s\n",caWhere);    
   lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
      sprintf(caSql,"select count(*) from nwcompute a,ncuser b where a.userid=b.userid %s",caWhere);
      iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lTotRec);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"DataBase Error %s",pasDbErrorMsg(NULL));
            return 0;
        }

    lRowNum = 1000;
    lStartRec = (lCurPg - 1) * lRowNum;  
    lTotPg = 1 + (lTotRec - 1) / lRowNum;
    psCur = pasDbOpenSqlF("select a.userid,a.compid,b.dispname from nwcompute a,ncuser b where a.userid=b.userid %s  order by a.userid limit %lu,%lu",caWhere,lStartRec,lRowNum);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    
utPltSetCvtHtml(1);
    
    

    lUserid=0;
    lCompid=0;
    memset(caDispname,0,sizeof(caDispname));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_LONG,4,&lCompid,
                                   UT_TYPE_STRING,30,caDispname);
                                 
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
  
    iNum = 0;

    while(iReturn == 0) {


         iNum++;
          utPltPutLoopVarF(psDbHead,"nodep",iNum,"%d",0);
        	utPltPutLoopVarF(psDbHead,"nodes",iNum,"U%d",lUserid);
        	utPltPutLoopVar(psDbHead,"dname",iNum,caDispname);
        	utPltPutLoopVarF(psDbHead,"userid",iNum,"%d",lUserid);
        	utPltPutLoopVarF(psDbHead,"compid",iNum,"%d",lCompid);
        
        
        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum-1);



    lUserid=0;
    lCompid=0;
    memset(caDispname,0,sizeof(caDispname));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_LONG,4,&lCompid,
                                   UT_TYPE_STRING,30,caDispname);
    }
    pasDbCloseCursor(psCur);
 utPltSetCvtHtml(0);  
    utPltPutVarF(psDbHead, "TotRec","%lu",lTotRec);
    utPltPutVarF(psDbHead, "TotPg", "%lu",lTotPg);
    utPltPutVarF(psDbHead, "CurPg", "%lu",lCurPg);
    utPltPutVar(psDbHead,"name",caName);
    utPltPutVar(psDbHead,"fname",caFname);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}


/* 显示屏幕信息*/
int ncWebShowHisScreen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    ncUserInfo *psUserInfo;
    char sqlbuf[2048];
    char title_name[32];
    long lTime,lTime1,lTime2,lGroupid0;
    char caGroupid[22],caUserid[22],caService[16];
    char caGroupname[32];
    long lUserid;
    int iReturn,iNum;
    char *pSelect;
    char caTime[16],sDate1[16],sDate2[16],caUsername[32];
    char caGroup[12],caOrder[12];
    char caExport[12];
    char caSdate[12],caEdate[12],caStime[10],caEtime[10],caRecn[7];
    char caSdate_all[20],caEdate_all[20],caUname[32];
    long lStime,lEtime,lService;
    char caMyaccount[48],caMfrom[48],caMto[48],caMesg[256];
    char caVid[48],caRid[48],caCont[64],caMachway[12],caStamp[12];;
    unsigned long lTimes;
    char caSum[12];
    char caTitlename[36];
    int iSumRec,mCount;
    char caMesg_temp[600];
    long8 lBytes;
    pasDbCursor *psCur;   
   char caName_buf[4048],caDispname[32],caName[32];
    unsigned long lPresid;
    long lSesid;
    long lPrestime=0;
    char caPreMfrom[64];
    char caFriaccount[48];
   long lGroupid,flag,lStart;
    char *pBuf;
   char caTotPg[16],caCurPg[16];
   long lRowNum,lStartRec,lCurPg,lCount,lTotPg;
   	iReturn = utMsgGetSomeNVar(psMsgHead,16,
					"sdate",UT_TYPE_STRING,11,caSdate,
					"edate",UT_TYPE_STRING,11,caEdate,
					"stime",UT_TYPE_STRING,8,caStime,
					"etime",UT_TYPE_STRING,8,caEtime,
					"dispname",UT_TYPE_STRING,30,caDispname,
					"service",UT_TYPE_STRING,10,caService,
					"vid",  UT_TYPE_STRING,48,caVid,
					"rid",  UT_TYPE_STRING,48,caRid,
					"groupid",UT_TYPE_STRING,10,caGroupid,
					"content",UT_TYPE_STRING,63,caCont,
					"machway",UT_TYPE_STRING,10,caMachway,
					"stamp",UT_TYPE_STRING,8,caStamp,
					"userid",UT_TYPE_STRING,10,caUserid,
					"exp",  UT_TYPE_STRING,10,caExport,
					 "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					 "CurPg",      UT_TYPE_STRING, 12, caCurPg);

//printf("caCurPg=%s\n",caCurPg);
if(strlen(caSdate)==0){
	    lTime = time(0);
//    ncUtlMonthDate(lTime,caSdate,caEdate);
       ncUtlWeekDate(lTime,caSdate,caEdate);
    strcpy(caStime,"00:00");
    strcpy(caEtime,"23:59");
  }

  if(utStrIsSpaces(caExport)&&atol(caStamp)==0){
   lRowNum = 30;
  }
  else{
  	lRowNum=999999;
  }
  lCurPg=atol(caCurPg);
  if(lCurPg<1) lCurPg=1;
  
  if((strlen(caStamp)==0)&&utStrIsSpaces(caExport))
    lStartRec = (lCurPg - 1) * lRowNum;    
   else{
   	lStart=(lCurPg - 1) * 30;  
   	lStartRec=0;
  }

  sprintf(caSdate_all,"%s %s",caSdate,caStime);
  sprintf(caEdate_all,"%s %s",caEdate,caEtime);
  
  lStime = utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
  lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
  
lCount=0;
if(strlen(caStamp)==0){
 sprintf(sqlbuf,"select count(*) from  ncimclient a,ncuser b where a.userid=b.userid and a.stime>=%d and a.stime<=%d and sesid>0 ",lStime,lEtime);
 if(strlen(caService)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and a.service=%s ",caService);
  }
  
  if((strlen(caDispname)>0)&&(strlen(caUserid)==0)){
  	sprintf(sqlbuf+strlen(sqlbuf)," and (b.dispname like '%c%s%c' or b.username like '%c%s%c' )",'%',caDispname,'%','%',caDispname,'%');
  }
  if(strlen(caUserid)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and a.userid=%s ",caUserid);
  	strcpy(caDispname,ncUtlBufGetUsername(psShmHead,atol(caUserid)));
  }
  
  if(strlen(caVid)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and a.myaccount='%s' ",caVid);
  }
  if(strlen(caRid)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and a.friendaccount like '%c%s%c' ",'%',caRid,'%');
  }
  if(strlen(caCont)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and a.mesg like '%c%s%c' ",'%',caCont,'%');
  }
  if(strlen(caGroupid)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and b.groupid=%s ",caGroupid);
  }
  pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lCount);

}



  sprintf(sqlbuf,"select a.userid,b.dispname,b.groupid,a.service,a.myaccount,a.stime,a.mfrom,a.mto,a.mesg,a.sesid,a.friendaccount from ncimclient a,ncuser b where a.userid=b.userid and a.stime>=%d and a.stime<=%d and sesid>0 ",lStime,lEtime);
  
  if(strlen(caService)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and a.service=%s ",caService);
  }
  
  if((strlen(caDispname)>0)&&(strlen(caUserid)==0)){
  	sprintf(sqlbuf+strlen(sqlbuf)," and (b.dispname like '%c%s%c' or b.username like '%c%s%c' )",'%',caDispname,'%','%',caDispname,'%');
  }
  if(strlen(caUserid)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and a.userid=%s ",caUserid);
  	strcpy(caDispname,ncUtlBufGetUsername(psShmHead,atol(caUserid)));
  }
  
  if(strlen(caVid)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and a.myaccount='%s' ",caVid);
  }
  if(strlen(caRid)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and a.friendaccount like '%c%s%c' ",'%',caRid,'%');
  }
  if(strlen(caGroupid)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and b.groupid=%s ",caGroupid);
  }
  if(strlen(caCont)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and a.mesg like '%c%s%c' ",'%',caCont,'%');
  }
  
  sprintf(sqlbuf+strlen(sqlbuf)," order by a.sesid desc ,a.stime ");
  sprintf(sqlbuf+strlen(sqlbuf)," limit %d,%d ",lStartRec,lRowNum);
  
 
        
  psCur=pasDbOpenSql(sqlbuf,0);
                  
  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0582统计"),ncLang("0549数据库出错"));
        return 0;
   }

    iReturn = 0;
    iNum = 0; 
  
    lUserid=0;
    lService=0;
    memset(caMyaccount,0,sizeof(caMyaccount));
    lStime=0;
    memset(caMfrom,0,sizeof(caMfrom));
    memset(caMto,0,sizeof(caMto));
    memset(caMesg,0,sizeof(caMesg));
    memset(caUname,0,sizeof(caUname));
    lGroupid=0;
    lSesid=0;
    memset(caFriaccount,0,sizeof(caFriaccount));                       
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4, &lUserid,
                                   UT_TYPE_STRING,31,caUname,
                                   UT_TYPE_LONG,4,&lGroupid,
    															 UT_TYPE_LONG,4,&lService,
    															 UT_TYPE_STRING,47,caMyaccount,
    															 UT_TYPE_LONG,4,&lStime,
    															 UT_TYPE_STRING,47,caMfrom,
    															 UT_TYPE_STRING,47,caMto,
    															 UT_TYPE_STRING,255,caMesg,
    															 UT_TYPE_ULONG,4,&lSesid,
    															 UT_TYPE_STRING,47,caFriaccount);
    															
                            
lPrestime=time(0);
 lPresid=0;

        psDbHead = utPltInitDb();
   
    
    
        
    utPltPutVar(psDbHead,"sdate",caSdate);
		utPltPutVar(psDbHead,"edate",caEdate);
		utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"vid",caVid);
    utPltPutVar(psDbHead,"rid",caRid);
    utPltPutVar(psDbHead,"content",caCont);
    utPltPutVar(psDbHead,"stamp",caStamp);
    if(strlen(caMachway)==0) strcpy(caMachway,"0");
    
    utPltPutVar(psDbHead,"machway",caMachway);
		utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"stime",caStime);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"userid",caUserid);     



    
     while(iReturn==0){
     	if(atol(caStamp)==0){
		  iNum++;
		     if((lPresid==lSesid)&&strstr(caMfrom,caMyaccount)&&!strstr(caPreMfrom,caMyaccount)){
		     	if(lStime-lPrestime>=0)
		     	  utPltPutLoopVar(psDbHead,"reptime",iNum,ncCvtConntime(lStime-lPrestime));
		    }
		    lPrestime=lStime;
		    strcpy(caPreMfrom,caMfrom);
		    lPresid=lSesid;
		    
		  		utPltPutLoopVar(psDbHead,"dispname",iNum,caUname);
		  		utPltPutLoopVar(psDbHead,"sername",iNum,ncUtlGetServiceName(psShmHead,lService,"\0"));
		  		utPltPutLoopVar(psDbHead,"gname",iNum,ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
		  	  utPltPutLoopVarF(psDbHead,"userid",iNum,"%d",lUserid);
		  	  utPltPutLoopVarF(psDbHead,"service",iNum,"%d",lService);
		  	  utPltPutLoopVar(psDbHead,"myaccount",iNum,caMyaccount);
		  	  utPltPutLoopVar(psDbHead,"friaccount",iNum,caFriaccount);
		  	  utPltPutLoopVar(psDbHead,"mfrom",iNum,caMfrom);
		  	  utPltPutLoopVar(psDbHead,"mto",iNum,caMto);
		  	  
		  	  ncStrFixedCol(caMesg,caMesg_temp,40,254);
		  	  utPltPutLoopVar(psDbHead,"mesg",iNum,caMesg_temp);
		  	  utPltPutLoopVar(psDbHead,"stime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
		  	}
		  	else{
		  		flag=0;
		  		  if((lPresid==lSesid)&&strstr(caMfrom,caMyaccount)&&!strstr(caPreMfrom,caMyaccount)&&(lStime-lPrestime>=0)){
			        if(atol(caMachway)==0){
			        	if(lStime-lPrestime>60*atol(caStamp)){
			        		flag=1;
			        	}
			        }
			        else if(atol(caMachway)==1){
			        	if(lStime-lPrestime<60*atol(caStamp)){
			        		flag=1;
			        	}
			        }
			        else{
			        	if(lStime-lPrestime==60*atol(caStamp)){
			        		flag=1;
			        	}
			        }
			        if(flag==1){
			        	lCount++;
			        	if(lCount>lStart&&iNum<30){
			        	 iNum++;
			        	 	utPltPutLoopVar(psDbHead,"reptime",iNum,ncCvtConntime(lStime-lPrestime));
			        	 	utPltPutLoopVar(psDbHead,"dispname",iNum,caUname);
						  		utPltPutLoopVar(psDbHead,"sername",iNum,ncUtlGetServiceName(psShmHead,lService,"\0"));
						  		utPltPutLoopVar(psDbHead,"gname",iNum,ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
						  	  utPltPutLoopVarF(psDbHead,"userid",iNum,"%d",lUserid);
						  	  utPltPutLoopVarF(psDbHead,"service",iNum,"%d",lService);
						  	  utPltPutLoopVar(psDbHead,"myaccount",iNum,caMyaccount);
						  	  utPltPutLoopVar(psDbHead,"mfrom",iNum,caMfrom);
						  	  utPltPutLoopVar(psDbHead,"mto",iNum,caMto);
						  	  utPltPutLoopVar(psDbHead,"friaccount",iNum,caFriaccount);
						  	  ncStrFixedCol(caMesg,caMesg_temp,40,254);
						  	  utPltPutLoopVar(psDbHead,"mesg",iNum,caMesg_temp);
						  	  utPltPutLoopVar(psDbHead,"stime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
							        	 
			        	 }
			        }
			        
			        
			        
			     }

			    
			    lPrestime=lStime;
			    strcpy(caPreMfrom,caMfrom);
			    lPresid=lSesid;
		  		
		  	}
    lUserid=0;
    lService=0;
    memset(caMyaccount,0,sizeof(caMyaccount));
    lStime=0;
    memset(caMfrom,0,sizeof(caMfrom));
    memset(caMto,0,sizeof(caMto));
    memset(caMesg,0,sizeof(caMesg));
    memset(caUname,0,sizeof(caUname));
    lGroupid=0;
    
                               
    lSesid=0;
                               
    memset(caFriaccount,0,sizeof(caFriaccount));                       
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4, &lUserid,
                                   UT_TYPE_STRING,31,caUname,
                                   UT_TYPE_LONG,4,&lGroupid,
    															 UT_TYPE_LONG,4,&lService,
    															 UT_TYPE_STRING,47,caMyaccount,
    															 UT_TYPE_LONG,4,&lStime,
    															 UT_TYPE_STRING,47,caMfrom,
    															 UT_TYPE_STRING,47,caMto,
    															 UT_TYPE_STRING,255,caMesg,
    															 UT_TYPE_ULONG,4,&lSesid,
    															 UT_TYPE_STRING,47,caFriaccount);
		  
		}
		pasDbCloseCursor(psCur);
		
	
		
  
      
      
     lTotPg = (lCount - 1) / 30 + 1;
     utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
     utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
     utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
     
     
     
     
   //输出部门
   sprintf(sqlbuf,"select groupid,groupname from ncgroup order by groupname ");
     
  psCur=pasDbOpenSql(sqlbuf,0);
                  
  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0582统计"),ncLang("0549数据库出错"));
        return 0;
   }

    iReturn = 0;
    iNum = 0; 
  
    memset(caGroupname,0,sizeof(caGroupname));
    lGroupid=0;
                                
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4, &lGroupid,
                                   UT_TYPE_STRING,31,caGroupname);
     
    
     while(iReturn==0){
     	iNum++;
     	utPltPutLoopVarF(psDbHead,"gid",iNum,"%d",lGroupid);
     	utPltPutLoopVar(psDbHead,"gn",iNum,caGroupname);
     	
     
       memset(caGroupname,0,sizeof(caGroupname));
       lGroupid=0;
                                
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4, &lGroupid,
                                   UT_TYPE_STRING,31,caGroupname);
		  
		  }
		  pasDbCloseCursor(psCur);
     
     
     
     
       
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nchisscreen.htm");
   
    return 0;
}