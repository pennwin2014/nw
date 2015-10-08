#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include <stdarg.h>
#include "utoall.h"
#include "pasdb.h"
#include "pasutl.h"
#include "ncdef.h"
#include "nwdef.h"
#include <stdio.h>
#define  PAS_SRCFILE   6001
int nwUtlConfigLog(unsigned long lCompId,char *pType,char *pValue,...);

/* 处理退出事宜  */
int nwUtlLogOut(utShmHead *psShmHead,nwCompIp *psCompIp)
{
    
    psCompIp->psComp->status = 0;
    utShmHashDel(psShmHead,NC_LNK_ONLINECOMP,psCompIp);
}

/* 处理登录事宜  
   外网监控中在线用户情况
 */
int nwUtlLogin(utShmHead *psShmHead,nwCompInfo *psComp,char *pCompname,char *pMac,unsigned long lSip,char *pVersion)
{
    ncUserInfo *psUserInfo;
    char caSip[16],caDip[16];
    char caMac[20];
    int iReturn;
//    printf(" Do Login ....%lu \n",psComp->compid);
    
    psUserInfo = (ncUserInfo *)ncGetUserInfoByIp(psShmHead,lSip);
    if(psUserInfo) { /* 该机器的用户已在线  */
        psUserInfo->lCltId = psComp->compid;
        if(psComp->userid != psUserInfo->lId) {
            nwUtlConfigLog(psComp->compid,ncLang("0532使用者"),ncLang("0747用户从%s(%lu)变为%s(%lu)"),
                ncUtlBufGetUsername(psShmHead,psComp->userid,"\0"),psComp->userid,
                psUserInfo->psUser?psUserInfo->psUser->username:" ",psUserInfo->lId);
            psComp->userid = psUserInfo->lId;
            psComp->groupid = psUserInfo->lGid;
        }
    }
    /* 更新本机信息  */
    if(psComp->ip != lSip) { /* IP地址发生变化  */
        strcpy(caSip,utComHostIp(htonl(psComp->ip)));
        strcpy(caDip,utComHostIp(htonl(lSip)));
        nwUtlConfigLog(psComp->compid,ncLang("0054IP地址"),ncLang("0058IP地址从%s变为%s"),caSip,caDip);
        psComp->ip = lSip;
    }
    ncCvtMacI(pMac,caMac);
    if(memcmp(psComp->mac,caMac,6)!=0) {
        nwUtlConfigLog(psComp->compid,ncLang("0072MAC地址"),ncLang("0075MAC地址从%s变为%s"),ncCvtMac(psComp->mac),pMac);
        memcpy(psComp->mac,caMac,6);
    }
    if(strcmp(psComp->compname,pCompname)!=0) {
        nwUtlConfigLog(psComp->compid,ncLang("0290计算机名"),ncLang("0294计算机名从%s变为%s"),psComp->compname,pCompname);
        strcpy(psComp->compname,pCompname);
    }
    if(strcmp(psComp->version,pVersion)!=0) {
        nwUtlConfigLog(psComp->compid,ncLang("0345客户端版本"),ncLang("0346客户端版本%s变为%s"),psComp->version,pVersion);
        strcpy(psComp->version,pVersion);
    }
    iReturn = pasDbExecSqlF("update nwcompute set userid=%lu,groupid=%lu,ip=%lu,compname='%s',mac='%s',lasttime=%lu,version='%s' where compid = %lu",
            psComp->userid,psComp->groupid,psComp->ip,psComp->compname,ncCvtMac(psComp->mac),time(0),psComp->version,psComp->compid);
    if(iReturn != 0) {
        ncSysLog(NC_LOG_WARN,"nwcompute update error %s ",pasDbErrorMsg(NULL));
    }
    else {
        pasDbCommit(NULL);
    }

    return 0;
}


int nwUtlConfigLog(unsigned long lCompId,char *pType,char *pValue,...)
{
    va_list ap;
    char buf[128];
    int iReturn;

    va_start(ap, pValue);
    vsnprintf(buf,128,pValue,ap);
    va_end(ap);
    iReturn = pasDbExecSqlF("insert into nwconfiglog(sid,compid,mtime,cname,cvalues) values (%lu,%lu,%lu,'%s','%s')",
                  pasGetSid("nwconfiglog","sid"),lCompId,time(0),pType,buf);
    if(iReturn != 0) {
        ncSysLog(NC_LOG_ERROR,"nwconfiglog insert error %s ",pasDbErrorMsg(NULL));
        pasDbRollback(NULL);
    }
    else{
    	pasDbCommit(NULL);
    }
    return iReturn;
}

/* 先将用户信息全部调入内存中, 放在一个Hash表中   */

int nwUtlLoadUserInfo(utShmHead *psShmHead)
{
    
    unsigned long   lMaxUser;
    int             iReturn,iRet;
    char            *p;
    char caMac[20];
    unsigned long lExpire,lUsers,lStart;
    pasDbCursor *psCur;
    nwCompInfo s;
// printf(" Load UserInfo.....\n");
    p = utComGetVar_s(psShmHead,"ProductSn");
    if(p == NULL) {
        ncSysLog(NC_LOG_ERROR,"ProductSn does not define");
        return (-1);
    }
    iReturn = pasInstGetLicense(p,&lExpire,&lUsers,&lStart);
    if(iReturn != 1) {
        ncSysLog(NC_LOG_ERROR,"License Error");
        return (-1);
    }
    printf(" lUsers==%d \n",lUsers);    
    utShmFreeHash(psShmHead,NC_LNK_ONLINECOMP); 
    iReturn = utShmHashInit(psShmHead,NC_LNK_ONLINECOMP,
                 lUsers,lUsers,sizeof(nwCompIp),0,4);
    if(iReturn < 0) {
        ncSysLog(NC_LOG_ERROR, "Share memory Error, nwUtlLoadUserInfo");
        return (-1);
    }
    utShmFreeHash(psShmHead,NC_LNK_COMPUTE);  /* 先释放  */
    lMaxUser = utComGetVar_ld(psShmHead,"MaxRegDev",1500);
    iReturn = utShmHashInit(psShmHead,NC_LNK_COMPUTE,
                 lMaxUser,lMaxUser,sizeof(nwCompInfo),0,4);
    if(iReturn != 0) {
        ncSysLog(NC_LOG_ERROR,"Share Memory alloc Error at nwUtlLoadUserInfo");
        return (-1);
    }
   
  
    
    
    
    
    /* 将用户资料导入到内存中  */
    psCur = pasDbOpenSqlF("select compid,devname,groupid,userid,ip,compname,mac,os,ie,version,authkey,htime,stime,ctime from nwcompute");
    if(psCur == NULL) {
        ncSysLog(NC_LOG_ERROR,"Cursor open error %s at nwUtlLoadUserInfo",pasDbErrorMsg(NULL));
        return (-1);
    }
    
    memset(&s,0,sizeof(s));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&s.compid,
                                    UT_TYPE_STRING,15,s.devname,
                                    UT_TYPE_ULONG,4,&s.groupid,
                                    UT_TYPE_ULONG,4,&s.userid,
                                    UT_TYPE_ULONG,4,&s.ip,
                                    UT_TYPE_STRING,15,s.compname,
                                    UT_TYPE_STRING,17,caMac,
                                    UT_TYPE_STRING,31,s.os,
                                    UT_TYPE_STRING,31,s.ie,
                                    UT_TYPE_STRING,16,s.version,
                                    UT_TYPE_STRING,16,s.caKey,
                                    UT_TYPE_ULONG,4,&s.htime,
                                    UT_TYPE_ULONG,4,&s.stime,
                                    UT_TYPE_ULONG,4,&s.ctime);
    while(iReturn == 0) {
// #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
        ncCvtMacI(caMac,s.mac);    
        iRet = utShmHashAdd(psShmHead,NC_LNK_COMPUTE,&s,1);
        if(iRet < 0) {
            ncSysLog(NC_LOG_ERROR,"Compute Add Error");
            pasDbCloseCursor(psCur);
            return (-1);
        }
        
        
        memset(&s,0,sizeof(s));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&s.compid,
                                    UT_TYPE_STRING,15,s.devname,
                                    UT_TYPE_ULONG,4,&s.groupid,
                                    UT_TYPE_ULONG,4,&s.userid,
                                    UT_TYPE_ULONG,4,&s.ip,
                                    UT_TYPE_STRING,15,s.compname,
                                    UT_TYPE_STRING,17,caMac,
                                    UT_TYPE_STRING,31,s.os,
                                    UT_TYPE_STRING,31,s.ie,
                                    UT_TYPE_STRING,16,s.version,
                                    UT_TYPE_STRING,16,s.caKey,
                                    UT_TYPE_ULONG,4,&s.htime,
                                    UT_TYPE_ULONG,4,&s.stime,
                                    UT_TYPE_ULONG,4,&s.ctime);
    }
    pasDbCloseCursor(psCur);
    return 0;
}


/* 检查在线用户信息  */
int  nwUtlCheckOnlineComp(utShmHead *psShmHead)
{
    char *pHash;
    unsigned long lTimeOut=60,lTime0;
    pasHashInfo sHashInfo;
    nwCompIp *psCompIp,*psCompIp0;
    
    lTimeOut = utComGetVar_ld(psShmHead,"NwTimeOut",60);
    while(1) {
        pHash = utShmHashHead(psShmHead,NC_LNK_ONLINECOMP);
        if(pHash == NULL) {
            ncSysLog(NC_LOG_ERROR,"[nwUtlCheckOnlineComp] OnlineComp not ready");
            sleep(5);
        }
        else {
            lTime0 = time(0);
            psCompIp = (nwCompIp *)pasHashFirst(pHash,&sHashInfo);
            while(psCompIp) {
                if(psCompIp->lLastTime < lTime0 - lTimeOut) { /* 超时  */
                    psCompIp0 = psCompIp;
                    psCompIp= (nwCompIp *)pasHashNextS(&sHashInfo);
                    nwUtlLogOut(psShmHead,psCompIp0);
                }
                else {
                    psCompIp= (nwCompIp *)pasHashNextS(&sHashInfo);
                }
            }
            sleep(20);
        }
    }
}

int nwUtlFreeDevHead(nwDevHead *psDevHead,nwSwitchHead *psFree)
{
    char caFile[128];
    if(psDevHead->firstSoft) {
        nwUtlFreeSoftInfo(psDevHead,psFree);
        psDevHead->firstSoft = NULL;
    }
    if(psDevHead->firstDev) {
        nwUtlFreeDevInfo(psDevHead,psFree);
        psDevHead->firstDev = NULL;
    }
    if(psDevHead->firstProc) {
        nwUtlFreeProcInfo(psDevHead,psFree);
        psDevHead->firstProc = NULL;
    }
    if(psDevHead->sysInfo) {
        nwUtlFreeBuf(psFree,(nwSwitchBuf  *)psDevHead->sysInfo);
        psDevHead->sysInfo = NULL;
    }
    if(psDevHead->lScrId > 0) {
        sprintf(caFile,"%s/tmp/screen/%lu.jpg",utComGetVar_sd(utShmCurHead(),"maillog","/home/ncmysql/nc/maillog"),
               psDevHead->lScrId);
        remove(caFile);
        psDevHead->lScrId = 0;
    }
    memset(psDevHead,0,sizeof(nwDevHead));
    return 0;
}


int nwUtlCheckDevHeadTimeOut(nwDevHead *psDevHead,nwSwitchHead *psFree,int lTimeOut)
{
    unsigned long lTime;
    char caFile[128];
    lTime = time(0) - lTimeOut;
    if(psDevHead->firstSoft) {
        if(psDevHead->lSoftTime < lTime) {
            nwUtlFreeSoftInfo(psDevHead,psFree);
            psDevHead->firstSoft = NULL;
            psDevHead->lSoftTime = 0;
        }
    }
    if(psDevHead->firstDev) {
        if(psDevHead->lDevTime < lTime) {
            nwUtlFreeDevInfo(psDevHead,psFree);
            psDevHead->firstDev = NULL;
            psDevHead->lDevTime = 0;
        }
    }
    if(psDevHead->firstProc) {
        if(psDevHead->lProcTime < lTime) {
            nwUtlFreeProcInfo(psDevHead,psFree);
            psDevHead->firstProc = NULL;
            psDevHead->lProcTime = 0;
        }
    }
    if(psDevHead->sysInfo) {
        if(psDevHead->lSysTime < lTime) {
            nwUtlFreeBuf(psFree,(nwSwitchBuf  *)psDevHead->sysInfo);
            psDevHead->sysInfo = NULL;
            psDevHead->lSysTime = 0;
        }
    }
    if(psDevHead->lScrId > 0) {
        if(psDevHead->lScrTime < lTime) {
            sprintf(caFile,"%s/tmp/screen/%lu.jpg",utComGetVar_sd(utShmCurHead(),"maillog","/home/ncmysql/nc/maillog"),
               psDevHead->lScrId);
            remove(caFile);
            psDevHead->lScrId = 0;
            psDevHead->lScrTime = 0;
        }
    }
    return 0;
}
  
