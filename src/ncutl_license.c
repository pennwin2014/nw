#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "ncdef.h"

/* 产品License信息  */
int ncWebSystemSet_License(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
    unsigned long lExpire,lUsers,lStart,lTime,lFun;
    utPltDbHead *psDbHead;
    int iReturn;
    char *pProdsn,*pHost,*pDispName;
    long lGroupid0=0;

    dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","License","You Can't Access this pages");
    	return 0;
    }
    psDbHead = utPltInitDb();
    lTime = time(0);
    pProdsn = utComGetVar_sd(psShmHead,"ProductSn","\0");
    
    iReturn = pasInstGetLicense(pProdsn,&lExpire,&lUsers,&lFun);
    if(iReturn == 1) { /* License 正常  */
        utPltPutVarF(psDbHead,"prodsn","%s",pProdsn);
        utPltPutVarF(psDbHead,"users","%lu",lUsers);
        if(lExpire - lTime > 90000000L) {
            utPltPutVarF(psDbHead,"expire","%s","Unlimited");
        }
        else {
            utPltPutVarF(psDbHead,"expire","%s",utTimFormat("%Y-%m-%d",lExpire));
        }
    }
    pDispName = utComGetVar_sd(psShmHead,"YourDispName","\0");
    utPltPutVarF(psDbHead,"name","%s",pDispName);
    pHost = pasInstGetHostInfo("eth0");
    if(pHost) {
         utPltPutVarF(psDbHead,"hostinfo","%s",pHost);
    }
    else {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","License","Internl Error");
    	return 0;
    }
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_sys_licenseinfo.htm");
    return 0;
}
             

/* 保存并检验License  */
int ncWebSystemSave_License(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char *pHost;
    char *pConfig;
    pasConfList *psConfig;
    unsigned long lFun;
    char caName[64],caKey[64],caProdsn[64];
#ifdef LDEBUG
utMsgPrintMsg(psMsgHead);
#endif
    iReturn = utMsgGetSomeNVar(psMsgHead,3,
                    "name",    UT_TYPE_STRING,63,caName,
                    "lkey",    UT_TYPE_STRING,63,caKey,
                    "prodsn",  UT_TYPE_STRING,63,caProdsn);
    pHost = pasInstGetHostInfo("eth0");
    if(pHost == NULL) {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","License","Intenal Error");
    	return 0;
    }
    iReturn = pasInstWriteLicense(caProdsn,"eth0",caKey);
printf(" Pid=%d iReturn == %d \n",getpid(),iReturn);
    if(iReturn == 0) {
        unsigned long lExpire,lUsers,lStart,lTime;
        utPltDbHead *psDbHead;
        char *pProdsn,*pDispName;
        char *pConfig;
        pasConfList *psConfig;
        pConfig = pasGetConfigFileName();
        psConfig = pasUtlReadConfig(pConfig);
        if(psConfig == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880自动备份设定"),ncLang("0427配置文件出错"));
            return 0;
        }
        pasUtlModiConfig(psConfig,"ProductSn",caProdsn);  
        pasUtlSaveConfig(pConfig,psConfig);
        pasUtlFreeConfig(psConfig);
        ncUtlReReadConFig(psShmHead);
        psDbHead = utPltInitDb();
        pProdsn = utComGetVar_sd(psShmHead,"ProductSn","\0");
        iReturn = pasInstGetLicense(pProdsn,&lExpire,&lUsers,&lFun);
        if(iReturn == 1) {
            lTime = time(0);
            utPltPutVarF(psDbHead,"prodsn","%s",pProdsn);
            utPltPutVarF(psDbHead,"users","%lu",lUsers);
            if(lExpire - lTime > 90000000L) {
                utPltPutVarF(psDbHead,"expire","%s","Unlimited");
            }
            else {
                utPltPutVarF(psDbHead,"expire","%s",utTimFormat("%Y-%m-%d",lExpire));
            }
        }
        pDispName = utComGetVar_sd(psShmHead,"YourDispName","\0");
        utPltPutVarF(psDbHead,"name","%s",pDispName);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_sys_licenseok.htm");
    	return 0;
    }
    else {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","License","0937License Error");
    	return 0;
    }
}



/* 下载许可证输入资料  */
int ncWebSystem_LicenseDown(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
    FILE *fp;
    int iReturn;
    char *pHost;
    char *pConfig;
    char *pProdsn;
    pasConfList *psConfig;
    char caName[64],caPath[128],caFile[128];
    char caDown[16],caEnter[16],caContuse[16];
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
    iReturn = utMsgGetSomeNVar(psMsgHead,4,
                    "name",    UT_TYPE_STRING,63,caName,
                    "downinfo",UT_TYPE_STRING,11,caDown,
                    "enterkey",UT_TYPE_STRING,11,caEnter,
                    "contuse", UT_TYPE_STRING,11,caContuse);
    sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog"));
    sprintf(caFile,"%s/licenseinfo.txt",caPath);
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","License","Config File Error");
        return 0;
    }
    pasUtlModiConfig(psConfig,"YourDispName",caName);
    pasUtlSaveConfig(pConfig,psConfig);
    pasUtlFreeConfig(psConfig);
    ncUtlReReadConFig(psShmHead);
    pHost = pasInstGetHostInfo("eth0");
    if(pHost == NULL) {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","License","Internal Error");
    	return 0;
    }
    pProdsn = utComGetVar_sd(psShmHead,"ProductSn","\0");
    if(!utStrIsSpaces(caDown)) {
        fp = fopen(caFile,"w");
        if(fp) {
            fprintf(fp,"# This File is created by ProEIM Software\n");
            fprintf(fp,"Date:[%s]\n",utTimFormat("%Y-%m-%d %H:%M:%S",time(0)));
            fprintf(fp,"Name:[%s]\n",caName);
            fprintf(fp,"ProdSn:[%s]\n",pProdsn);
            fprintf(fp,"Hostinfo:[%s]\n",pHost);
            fclose(fp);
            utPltFileDownload(iFd,"application/text",caPath,"licenseinfo.txt","licenseinfo.txt");
            remove(caFile);
            return 0;
        }
        else {
    	    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","License","License File create error");
    	    return 0;
        }
    }
    if(!utStrIsSpaces(caEnter)) { /* 显示输入License  */
        utPltDbHead *psDbHead;
        char *pDispName;
        psDbHead = utPltInitDb();
        pProdsn = utComGetVar_sd(psShmHead,"ProductSn","\0");
        utPltPutVarF(psDbHead,"prodsn","%s",pProdsn);
        pDispName = utComGetVar_sd(psShmHead,"YourDispName","\0");
        utPltPutVarF(psDbHead,"name","%s",pDispName);
        pHost = pasInstGetHostInfo("eth0");
        if(pHost) {
            utPltPutVarF(psDbHead,"hostinfo","%s",pHost);
        }
        else {
    	    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","License","Intel Error");
    	    return 0;
        }
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_sys_license.htm");
        return 0;
    }
    if(!utStrIsSpaces(caContuse)) {
        ncWebAuth(psShmHead,iFd,psMsgHead);
        return 0;
    }
    return 0;
}

    
/* 重新启动网络督察  */
int ncWebSystem_RestartNc(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    char *pIp,*pPort;
    char caUrl[64];
    psDbHead = utPltInitDb();

    pIp = utComGetVar_sd(psShmHead,"adminip","\0");
    pPort = utComGetVar_sd(psShmHead,"adminport","\0");
    sprintf(caUrl,"%s:%s",pIp,pPort);
    utPltPutVarF(psDbHead,"admin","%s",caUrl);
    ncSysRestartNc("Restart Nc For chang License");
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_reset.htm");
    return 0;
}

