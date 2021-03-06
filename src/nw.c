/*   网络督察 7.50                最后修改时间:2009/06/30  上海新网程信息技术有限公司
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utoall.h"
#include "pasutl.h"
#include "ncdef.h"
#include "pasdb.h"

// 根据时间生成的随机序列号



int     ncTcpInit(utShmHead *psShmHead);
static  int ncInit(utShmHead *psShmHead);
static  int ncUdpInit(utShmHead *psShmHead);
int     iDebugLevel;
int     iDefaultAccess;
int     iBandWidthFlags;
int     iNcClient = 0;         /* 是否启用客户端  */
int     iNcSendGA = 0;


int main(int argc, char **argv)
{
    int lSize;
    utShmHead *psShmHead;
    int iReturn, iDebug, i;
    char caConfFile[64];
    char caVar[32];
    char *p, *pLog, *pNw;
    int iLogLevel, iLogPort, lOffset = 14;
    char *pLogServer;
    if(argc > 1 && strcasecmp(argv[1], "-v") == 0)
    {
        printf("%s\n", ncReleaseNote());
        exit(9);
    }



    //   pasKillProcess(NULL,"ncdocfilter");
    pasKillProcess(NULL, "nw");
    //    pasSetLastInfoTable("nclastinfo");
    pasSetLogPath("../log");
    pasSetLogFile("../log/proeim.log");
    strcpy(caConfFile, "pqconf_nc.cnf");

    pasHashRemoveLock();
    printf("%s\n", ncReleaseNote());
    if(argc == 2 && (strcasecmp(argv[1], "reset") == 0))
    {
        ncUtlCheckLog("ProEIm Reset........... ");
        printf(" Start Reset ProEIM,......\n");
        psShmHead = utComShmOpen(caConfFile);
        if(psShmHead == NULL)
        {
            printf(" Memory Init Error \n");
            exit(9);
        }
    }
    else
    {
        printf(" Start Init ProEIM,......\n");
        ncUtlCheckLog("ProEIm Start........... ");
        psShmHead = (utShmHead *)ncShmCreate(caConfFile);
        if(psShmHead == NULL)
        {
            printf(" Memory Init Error 1111\n");
            exit(9);
        }
    }

    iLogLevel   = utComGetVar_ld(psShmHead, "SysLogLevel", 7);
    iLogPort    = utComGetVar_ld(psShmHead, "SysLogPort", 9963);
    pLogServer  = utComGetVar_sd(psShmHead, "SysLogServer", "127.0.0.1");


    ncLangInit(psShmHead);
    ncInitCheckDir(psShmHead);

    pLog = utComGetVar_s(psShmHead, "PLATEPATH");
    if(pLog)
    {
        utPltSetPlatePath(pLog);
    }

    iDebug = utComGetVar_ld(psShmHead, "DebugLevel", 1);
    pasSetLogLevel(iDebug);

    pLog = utComGetVar_s(psShmHead, "DebugInfo");
    i = 1;
    while(pLog || i < 10)
    {
        if(pLog)
        {
            printf("Set DebugInfo:[%s]\n", pLog);
            pasSetDebugLogInfo(pLog);
        }
        sprintf(caVar, "DebugInfo%d", i);
        pLog = utComGetVar_s(psShmHead, caVar);
        i++;
    }
    ncUtlSetGonfigValue(psShmHead);
    utComSetMaxTcpBuf(utComGetVar_ld(psShmHead, "MaxTcpBuffer", 1024000L));

    ncUtlPkgSetDebugFlags(psShmHead);
    utShmSetMark(psShmHead, NC_MARK_READY, NC_MARK_NOTREADY);
    ncInitCheckFile(psShmHead);
    ncInit(psShmHead);
    iReturn = pasConnect(psShmHead); /* 连接数据库  */
    if(iReturn != 0)
    {
        pasLog(PAS_LOG_ERROR, "DataBase Connect Error");
        return (-1);
    }
    /*
       iReturn = ncInitProeimMem(psShmHead,2);
       if(iReturn != 0) {
           ncSysLog(NC_LOG_ERROR,"Proeim Init Error \n");
           exit(9);
       }
       else {
           ncSysLog(NC_LOG_EVENT," ProEIM Init OK! \n");
       }
    */

    nwUtlSysInit(psShmHead);
    iReturn = ncUdpInitFun(psShmHead);
    if(iReturn != 0)
    {
        printf(" Proeim Init Error \n");
        return (-1);
    }
    else
    {
        printf(" InNetwork  Init OK! \n");
    }

    pasDbClose(NULL);


    ncInitWebFun(psShmHead);
    ncsInitNccFun(psShmHead);
    ncInitWebserviceFun(psShmHead);
    ncInitNwWebFun(psShmHead);
    lanWeb_initFun(psShmHead);
    lanWeb_initFun001(psShmHead);
    lanWeb_initFun002(psShmHead);
    lanWeb_initFun003(psShmHead);
    lanWeb_initFun004(psShmHead);
    pennwin_initFun(psShmHead);
    clean_data_initFun(psShmHead);

    //        iReturn = fireInit(psShmHead);
    dsCltSetSessionType("dispname", UT_TYPE_STRING, 32, 1);
    dsCltSetSessionType("email", UT_TYPE_STRING, 63, 1);
    dsCltSetSessionType("groupname", UT_TYPE_STRING, 31, 1);
    dsCltSetSessionType("groupid",  UT_TYPE_LONG, 4, 1);
    dsCltSetSessionType("usrlevel",  UT_TYPE_LONG, 4, 1);
    dsCltSetSessionType("mbflag",  UT_TYPE_LONG, 4, 0);
    dsCltSetSessionType("id",  UT_TYPE_LONG, 4, 1);
    dsCltSetSessionType("plate", UT_TYPE_STRING, 63, 0);
    iReturn = dsCltInit(psShmHead);
    if(iReturn < 0)
    {
        pasLog(PAS_LOG_ERROR, " Start dsserver Error \n");
        exit(9);
    }
    iReturn = pasTcpServer(psShmHead, ncTcpInit);   /* Tcp处理进程   */
    if(iReturn < 0)
    {
        exit(9);
    }
    pasOnTimeDo(psShmHead);

    printf("Start Some Process..........................................................\n");
    iReturn = ncStartSomeProcess(psShmHead);


    if(iReturn < 0)
    {
        pasLog(PAS_LOG_ERROR, " Start Process Error");
    }

    iReturn = pasUdpServer(psShmHead, ncUdpInit);
    if(iReturn < 0)
    {
        return (-1);
    }

    utShmClose(psShmHead);

    sleep(1);
    return 0;
}


static int ncInit(utShmHead *psShmHead)
{
    char *pPltPath;
    int iReturn, i, iSum;
    long lTime;
    char *p;
    int nHttpProxyPort;
    iDebugLevel = utComGetVar_ld(psShmHead, "DebugLevel", 999);



    return 0;

}



static int ncUdpInit(utShmHead *psShmHead)
{
    int iReturn;
    iReturn = pasConnect(psShmHead);
    if(iReturn != 0)
    {
        utSysLog("Connect Database Error Sqlcode is %d", iReturn);
        exit(9);
    }
    return 0;
}





/* 检查文件夹是否存在  */
int ncInitCheckFile(utShmHead *psShmHead)
{

    return 0;
}


