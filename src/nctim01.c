#define ncmax(a,b) ((a) > (b) ? (a) : (b))
#define ncMin(a,b) ((a) < (b) ? (a) : (b))
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "utoall.h"
#include "ncdef.h"
int ncUtlCheckCamsUser(utShmHead *psShmHead);
int ncDBSynSeque(utShmHead *psShmHead);
int ncCheckIp(utShmHead  *psShmHead);
int ncTimCtl(utShmHead *psShmHead);
int ncCtlCheckTables(utShmHead *psShmHead);
int ncCountLog(utShmHead *psShmHead);
int ncUtlTcWidthInit(utShmHead *psShmHead);
int ncDelDetailLog(utShmHead *psShmHead);
// int ncUserSysUserDb(utShmHead *psShmHead);
int ncUtlCheckMailProxy_1(utShmHead *psShmHead);
int ncLimReloadAllLim(utShmHead *psShmHead);
int ncUtlCountIpLog(utShmHead *psShmHead);
int ncPkgCheckPkgSeq(utShmHead *psShmHead0);
int ncUtlCheckIpExcept(utShmHead *psShmHead);
int  ncUtlSaveOnlineUser(utShmHead *psShmHead);
// int ncProcessCheck(utShmHead *psShmHead);
int ncUtlBackupMacList(utShmHead *psShmHead);
int ncUtlCheckIcBuf(utShmHead *psShmHead);
int  ncWarnCheckWarnInfo(utShmHead *psShmHead);
int ncUtlCheckHashData(utShmHead *psShmHead);
int ncAdCheck(utShmHead *psShmHead);
int ncUtlCheckNoMonIp(utShmHead *psShmHead);
int ncUtlBackupDns(utShmHead *psShmHead);
int ncUtlBackupMacTableToFile(utShmHead *psShmHead);
int ncCheckStockHashData(utShmHead *psShmHead);
int ncUtlCheckOnlineCamsUser(utShmHead *psShmHead);

/* 执行各种定时程序  */
int ncStartOnTimeDo(utShmHead *psShmHead)
{
    int iReturn;
    char *p;
    iReturn = ncUtlInitOntimeFun(psShmHead,20);
    if(iReturn < 0) {
        return (-1);
    }

/*
    ncUtlSetOntimeFun(psShmHead,"ncDBSynSeque",ncDBSynSeque,3600,0);
    ncUtlSetOntimeFun(psShmHead,"ncCheckIp",ncCheckIp,30,0);
  	if(strcasecmp(utComGetVar_sd(psShmHead,"StartNoMonIp","No"),"Yes")==0) {
        ncUtlSetOntimeFun(psShmHead,"ncUtlCheckNoMonIp",ncUtlCheckNoMonIp,10,0);
    } 
//    ncUtlSetOntimeFun(psShmHead,"ncUserSysUserDb",ncUserSysUserDb,33,0);
//    ncUtlSetOntimeFun(psShmHead,"ncCountLog",ncCountLog,utComGetVar_ld(psShmHead,"IpCountTime",3600),0);
    ncUtlSetOntimeFun(psShmHead,"ncTimCtl",ncTimCtl,utComGetVar_ld(psShmHead,"TimeCtlTime",300),0);
    ncUtlSetOntimeFun(psShmHead,"ncLimReloadAllLim",ncLimReloadAllLim,0,1);
    ncUtlSetOntimeFun(psShmHead,"ncPkgCheckPkgSeq",ncPkgCheckPkgSeq,60,0);
    ncUtlSetOntimeFun(psShmHead,"ncUtlCheckIpExcept",ncUtlCheckIpExcept,60,0);
    ncUtlSetOntimeFun(psShmHead,"ncUtlCheckHashData",ncUtlCheckHashData,60,0);
     ncUtlSetOntimeFun(psShmHead,"ncCheckStockHashData",ncCheckStockHashData,60,0);
    ncUtlSetOntimeFun(psShmHead,"ncUtlSaveOnlineUser",ncUtlSaveOnlineUser,180,0);   
/
    if(strcasecmp(utComGetVar_sd(psShmHead,"AutoBakArp","Yes"),"Yes") == 0) {
        ncUtlSetOntimeFun(psShmHead,"BackUpSniff",ncUtlBackupMacList,180,0);
    }
    
   
    if(( strcasecmp(utComGetVar_sd(psShmHead,"HttpProxy","No"),"Yes") == 0) ||
         strcasecmp(utComGetVar_sd(psShmHead,"SmtpProxy","No"),"Pronetway") == 0 ||
         strcasecmp(utComGetVar_sd(psShmHead,"Pop3Proxy","No"),"Yes") == 0
    ) {
        ncUtlSetOntimeFun(psShmHead,"ncUtlCheckMailProxy_1",ncUtlCheckMailProxy_1,60,0);
    }
   
    if(strcasecmp(utComGetVar_sd(psShmHead,"BandWidthLimit","No"),"YES") == 0) {
        ncUtlSetOntimeFun(psShmHead,"ncUtlTcWidthInit",ncUtlTcWidthInit,43200,0);
    }

//    ncUtlSetOntimeFun(psShmHead,"ncProcessCheck",ncProcessCheck,utComGetVar_ld(psShmHead,"ProcessCheck",60),0);
    if(strcasecmp(utComGetVar_sd(psShmHead,"ClientAuth","No"),"Yes")==0) {
        ncUtlSetOntimeFun(psShmHead,"ncUtlCheckIcBuf",ncUtlCheckIcBuf,30,0);
    }
  
    if(strcasecmp(utComGetVar_sd(psShmHead,"WarnSystem","Yes"),"Yes")==0) {
        ncUtlSetOntimeFun(psShmHead,"ncWarnCheckWarnInfo",ncWarnCheckWarnInfo,30,0);
    }
    
    if(strcasecmp(utComGetVar_sd(psShmHead,"AnyDnsProtocol","No"),"Yes")==0) {
        ncUtlSetOntimeFun(psShmHead,"BackupDnsService",ncUtlBackupDns,1800,0);
    }

    if(strcasecmp(utComGetVar_sd(psShmHead,"SynUserFromCams","No"),"Yes") == 0) {
        int lTimeOut;
        char *p;
        p = utComGetVar_sd(psShmHead,"CheckCamsOnTime","Yes");
        if(strcasecmp(p,"Yes")==0) {
            lTimeOut = utComGetVar_ld(psShmHead,"ImcCheckTime",300);
            ncUtlSetOntimeFun(psShmHead,"CheckOnCamsUser",ncUtlCheckOnlineCamsUser,lTimeOut,0);
        }
        // ncUtlSetOntimeFun(psShmHead,"ncUtlCheckCamsUser",ncUtlCheckCamsUser,1800,0); 不用
    }
    if(strcasecmp(utComGetVar_sd(psShmHead,"StartNtAuth","No"),"Yes") == 0) {
        ncUtlSetOntimeFun(psShmHead,"ncAdCheck",ncAdCheck,60,0);
    }
    p = utComGetVar_sd(psShmHead,"PppOeUserSyn","No");
    if(strcasecmp(p,"Yes") == 0 || strcasecmp(p,"Process") == 0) {
        ncUtlSetOntimeFun(psShmHead,"CheckMacTab",ncUtlBackupMacTableToFile,600,0);
    }
  
//    ncUtlSetOntimeFun(psShmHead,"ncCtlCheckTables",ncCtlCheckTables,60,0);	


    /*
    ncUtlSetOntimeFun(psShmHead,"ncCheckIp",ncCheckIp,utComGetVar_ld(psShmHead,"IpCheckTime",60),0);
    ncUtlSetOntimeFun(psShmHead,"ncSendCheckTables",ncSendCheckTables,utComGetVar_ld(psShmHead,"CheckIpTable",60),0);
    
    ncUtlSetOntimeFun(psShmHead,"ncUtlInitUserBuf",ncUtlInitUserBuf,0,1);
    ncUtlSetOntimeFun(psShmHead,"ncSendResetTables",ncSendResetTables,3600,0);
    ncUtlSetOntimeFun(psShmHead,"ncCheckSoftWare",ncUtlCheckSoftware,3600,0);

    if(strcasecmp(utComGetVar_sd(psShmHead,"BandWidthLimit","YES"),"YES") == 0) {
        ncUtlSetOntimeFun(psShmHead,"ncUtlTcWidthInit",ncUtlTcWidthInit,43200,0);
    }
    */
    iReturn = ncUtlStartOnTimeFun(psShmHead);
    return iReturn;
}


/*/* 定时程序       */
int ncUtlInitOntimeFun(utShmHead *psShmHead,int iMaxFun)
{
    ncOnTimeFun *psFun;
    int iReturn;
    utShmFreeArray(psShmHead,NC_LNK_ONTIME);
    iReturn = utShmInitArray(psShmHead,NC_LNK_ONTIME,sizeof(ncOnTimeFun),iMaxFun);
    if(iReturn < 0) {
        return (-1);
    }
    psFun = (ncOnTimeFun *)utShmArray(psShmHead,NC_LNK_ONTIME);
    if(psFun == NULL) {
        return (-1);
    }
    memset(psFun,0,iMaxFun * sizeof(ncOnTimeFun));
    return 0;
}

/* 定时执行程序   
           lStepTime ---- 间隔时间
           lOntime    ---- 指定时间(每天)
*/
int ncUtlSetOntimeFun(utShmHead *psShmHead, char *pName, int (*fFunName)(utShmHead *),
              unsigned long lStepTime,
              unsigned long lOnTime)
{
    int iSum,i;
    ncOnTimeFun *psFun;
    psFun = (ncOnTimeFun *)utShmArray(psShmHead,NC_LNK_ONTIME);
    if(psFun == NULL) {
        utSysLog(" Memory not init ");
        return (-1);
    }
    iSum = utShmArrayRecord(psShmHead,NC_LNK_ONTIME);
    for(i=0;i<iSum && psFun[i].iStatus > 0;i++);
    if(i<iSum) {
        psFun[i].iStatus = 1;
        psFun[i].lStepTime = lStepTime;
        psFun[i].lOnTime = lOnTime;
        psFun[i].fFunName = fFunName;
        psFun[i].lLastTime = time(0);
        strncpy(psFun[i].caName,pName,31);
        return 0;
    }
    utSysLog(" Exceed Max Limit Fun ");
    return (-1);
}

/* 启动定时程序
      iFlags --- 0 正常
                 1 调试
 */
int ncUtlStartOnTimeFun(utShmHead *psShmHead)
{
    int iSum,i;
    ncOnTimeFun *psFun;
    struct tm    *tm;
    unsigned long lThisTime,lThisDayTime,lThisDay,lStepTime;
    long lMinTime;
    int l1;
    int iReturn;
    iReturn = pasConnect(psShmHead);
    if(iReturn != 0) {
        utSysLog("Connect Database Error Sqlcode is %d",iReturn);
        exit(9);
    }
    psFun = (ncOnTimeFun *)utShmArray(psShmHead,NC_LNK_ONTIME);
    if(psFun == NULL) {
        utSysLog(" Memory not init ");
        return (-1);
    }
    iSum = utShmArrayRecord(psShmHead,NC_LNK_ONTIME);
    while(1) {

        lThisTime = time(0);
        tm = localtime(&lThisTime);
        lThisDayTime = tm->tm_hour * 10000L + tm->tm_min * 100 + tm->tm_sec;
        lThisDay = (1900 + tm->tm_year) * 10000 + (tm->tm_mon + 1) * 100 + tm->tm_mday;
        /* 检查最近一次的操作时间  */
        lMinTime = 3600;
        for(i=0;i<iSum;i++) {
            if(psFun[i].iStatus > 0) {
                if(psFun[i].lOnTime > 0)  {
                    if(lThisDay > ncUtlGetThatDays(psFun[i].lLastTime)) {
                        lMinTime = ncMin(lMinTime,psFun[i].lOnTime - lThisDayTime);
                    }
                    else {
                        lStepTime = 86400L - (tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec);
                        lMinTime = ncMin(lMinTime,lStepTime);
                    }
                }
                else {
                    l1 = psFun[i].lStepTime - (lThisTime - psFun[i].lLastTime);
                    if(l1 < 0) l1 = 0;
                    lMinTime = ncMin(lMinTime, l1);
                }
            }
        }
        if(lMinTime > 0) {
            sleep(lMinTime);
        }
        else {
            lMinTime = 0;
        }
        for(i=0;i<iSum;i++) {
            if(psFun[i].iStatus > 0) {
                lThisTime = time(0);
                tm = localtime(&lThisTime);
                lThisDayTime = tm->tm_hour * 10000L + tm->tm_min * 100 + tm->tm_sec;
                lThisDay = (1900 + tm->tm_year) * 10000 + (tm->tm_mon + 1) * 100 + tm->tm_mday;
                if(psFun[i].lOnTime > 0) { /* 定时  */
                    if(lThisDayTime >= psFun[i].lOnTime  &
                         lThisDay > ncUtlGetThatDays(psFun[i].lLastTime)) {
                        psFun[i].iStatus = 2;
                        psFun[i].lLastTime = time(0);
                        iReturn = psFun[i].fFunName(psShmHead);
                        psFun[i].lUsedTime = time(0) - psFun[i].lLastTime;
                        psFun[i].lCount++;
                        if(iReturn == 0) {
                            psFun[i].iStatus = 1;
                        }
                        else {
                            psFun[i].iStatus = 3;
                        }
                    }
                }
                else if(lThisTime - psFun[i].lLastTime >= psFun[i].lStepTime) {
                    psFun[i].iStatus = 2;
                    psFun[i].lLastTime = time(0);
                    iReturn = psFun[i].fFunName(psShmHead);
                    psFun[i].lUsedTime = time(0) - psFun[i].lLastTime;
                    psFun[i].lCount++;
                    if(iReturn == 0) {
                        psFun[i].iStatus = 1;
                    }
                    else {
                        psFun[i].iStatus = 3;
                    }
                }
            }
        }
    }
}

