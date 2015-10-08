/* 最后更新时间    2005/09/21 */
/* #include  "ncshmdebug.h"   */
#ifndef __NCSYSINFOEF__
/* 网络信息    */
#define NC_SYS_NETSUMFLOW         8    /* 采集数据缓存     */
#define NC_SYS_MAXDEV            16    /* 最大网络设备数   */
typedef struct ncSysNetFlow_s {
    unsigned long   lRecvBytes;         /* 接收字节  */
    unsigned long   lSendBytes;
    unsigned long   lRecvPkgs;
    unsigned long   lSendPkgs;
    unsigned long   lRerrs;
    unsigned long   lSerrs;
    unsigned long   lTestTime;           /* 采集时间  */
    unsigned long   lStepTime;
} ncSysNetFlow;
    
typedef struct ncSysDevInfo_s {
    char            caDevName[16];  /* 设备名  */
    unsigned char   cLink;          /* 连接状态 2--No Link  1--Link OK    8--NoCheck 9--NoDEv  */
    unsigned char   cStatus;        /* 连通状态 0--不通     1--正常                 */
    unsigned char   cRev[2];
    unsigned long   lWidth;         /* 网络带宽                                     */
    ncSysNetFlow    sBaseFlow;      /* 上一时段的采集流量                           */
    ncSysNetFlow    sFlow[NC_SYS_NETSUMFLOW];      /* 最近30分钟的流量,目前暂时使用第一个              */
} ncSysDevInfo;

typedef struct ncSysNetInfo_s {
    int             iSumDev;
    int             iCur;
    ncSysDevInfo    sDev[NC_SYS_MAXDEV];
} ncSysNetInfo;


typedef struct ncSysNetIp_s {
    char   caDevName[16];
    char   caIp[16];
    char   caMac[18];
    char   caMask[16];
} ncSysNetIp;

/* 进程的CPU占有率  */
typedef struct ncSysCpuRate_s {
    unsigned char caName[16];     /* 进程名      */
    unsigned long iPid;           /* 进程号      */
    double        fCpuRate;          /* Cpu占有率   */
    double        fMemRate;          /* Cpu占有率   */
} ncSysCpuRate;


typedef struct ncSysProcessList_s {
    unsigned char caUser[32];
    unsigned char caName[32];
    unsigned char caStart[240];
    unsigned char caStop[240];
    unsigned long lMinProc;
    unsigned long lMaxProc;
    unsigned long lThisProc;
}   ncSysProcessList;


typedef struct ncSysProcHead_s {
    ncSysProcessList    *psList;
    int                 iSum;
    int                 iMax;
} ncSysProcHead;


#define __NCSYSINFOEF__
#endif
