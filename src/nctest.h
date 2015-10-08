#ifndef __NCTEST_DEF__
#define __NCTEST_DEF__   1

#define NC_DEVCHKSTA_ERROR    -1
#define NC_DEVCHKSTA_SAFE     0
#define NC_DEVCHKSTA_NORMAL   1
#define NC_DEVCHKSTA_EXCEED   2

#define NC_MAXNETDEV          8
typedef struct ncPingInfo_s {
    char caHost[64];
    int  iStatus;               /*  0--正常   9--不通  */
    unsigned long lLastTime;    /*  最后测试时间       */
    double        fTime;        /* 平均应答时间        */
    int           iCount;       /* 成功收到数据包次数  */
    struct ncPingInfo_s *next;
} ncPingInfo;
    
typedef struct ncNetPingSta_s {
    char caDev[16];
    int  iStatus;               /*  0--正常   9--不通  */
    double        fTime;        /* 平均应答时间        */
    int           iCount;
    ncPingInfo *psPing;
} ncNetPingSta;


typedef struct ncChkLimit_s {
    int           iTestStepTime;           // 测试间隔时间
    double        fPingStep;               // Ping包的时间间隔
    int           iPingNumber;             // Ping包的数量
    int           iStatus;                 // 当前状态.... 1-正常运行  2-安全模式  
    int           iErrorTime;
    int           iSafeTime;
    int           iMaxErrorTime;           // 最大超时次数
    int           iMaxSafeTime;            // 最大安全次数
    unsigned long lMaxFlow[8];             // 网络流量上限
    unsigned long lMaxPkg[8];              // 网络数据包上限
    unsigned long lSafeFlow[8];            // 网络流量安全
    unsigned long lSafePkg[8];             // 数据包安全
    unsigned long lMaxLost;                // 丢包总数
    unsigned long lSafeLost;               // 丢包安全数
    double        fMaxTime;                // 超时时间
    double        fSafeTime;               // 安全时间
    char          caSafeStart[128];        // 安全模式启动
    char          caSysStart[128];         // 正常模式启动
    char          caLogFile[128];
    int           iSumDev;
    ncNetPingSta  sPing[NC_MAXNETDEV];
} ncChkLimit;



#endif
