#ifndef __NCCLUSTERDEF___
#define __NCCLUSTERDEF___

#define NC_CLU_HEATREQ      27811          /* 心跳请求             */
#define NC_CLU_HEATRES      27812          /* 心跳应答             */
#define NC_CLU_MAXDEV      8             /* 最大设备数          */
#define NC_CLU_MAXPROCESS  8             /* 监控进程的最大数量  */

/* 正在连接的客户端列表  */
typedef struct ncCluProcess_s {
    char      caName[16];    // 进程名称
    int       iSum;           // 进程数
} ncCluProcess;


/* 网络状态   */
typedef struct ncClusterDevSta_s {
    char              caDev[16];    // 检测设备名，如 eth0,eth1
    unsigned long     lBytes[2];    // 流量    0--上行   1--下行
    unsigned long     lPkgs[2];     // 数据包  0--上行   1--下行
    unsigned char     cLink;        // 0--Unknow 1--Link  2--NoLink
    unsigned char     cPing;        // 0--不通    1--通
    unsigned char     cWork;        // 0--正常　  1--异常   
    unsigned char     cRev;    
} ncClusterDevSta;

/* 网络对照表   */
typedef struct ncCluDevInfo_s {
    char                    caDev[16];     // 本地设备
    char                    caRDev[16];    // 对方设备
    unsigned long           lBytes[2];     // 阀值流量    0--上行   1--下行
    unsigned long           lPkgs[2];       // 阀值数据包  0--上行   1--下行
    unsigned char           cLink;         // 0--Link  1--NoLink
    unsigned char           cPing;         // 0--不通    1--通
    unsigned char           cWork;         // 0--休息    1--工作    
    unsigned char           cRev;
    unsigned char           caPingIp[128]; // 测试网络联通的IP地址
} ncCluDevInfo;



/* 心跳信息       */
#define NC_CLUSTA_NORMAL           1        // 正常
#define NC_CLUSTA_ERROR            0        // 异常
typedef struct ncCluHeat_s {
    unsigned short      nFunId;              // 功能代码
    unsigned char       cSec;                // 0-明文  1-密文
    unsigned char       cLen;                // 长度
    unsigned long       lMd5;                // 校验码
    unsigned long       lLastTime;           // 数据采集时间
    unsigned char       cStatus;             // 检测结果 1--正常   0--不正常
    unsigned char       cSumDev;             // 设备总数
    unsigned char       cSumProc;            // 进程总数
    unsigned char       cRev;                // 保留
    unsigned long       lIps;                             // 在线IP
    unsigned long       lUsers;                           // 在线用户数
    ncClusterDevSta     psDevInfo[NC_CLU_MAXDEV];         // 设备状态    
    ncCluProcess        psProc[NC_CLU_MAXPROCESS];        // 进程状态
} ncCluHeat;


/* 本地信息   */    
typedef struct ncCluMyInfo_s {
    unsigned char       caName[16];                 // 名称
    unsigned char       caKey[16];                  // 通信密钥
    unsigned long       lRemoteIp;                  // 对方IP地址
    unsigned short      nUdpPort;                   // UDP端口
    unsigned short      nTcpPort;                   // TCP端口
    unsigned char       cSumProc;                   // 要检测的进程总数
    unsigned char       cSumDev;                    // 要检测的网络设备总数
    unsigned char       cMyStat;                    // 0-备机状态  1-工作状态  2-正在同步数据
    unsigned char       cSec;                       // 0-不加密    1-加密
    unsigned long       lTimeOut;                   // 超时切换时间
    unsigned long       lCheckTime;                 // 测试数据检查间隔
    ncCluDevInfo        psDevInfo[NC_CLU_MAXDEV];   // 设备总数
    ncCluProcess        psProc[NC_CLU_MAXPROCESS];  // 进程信息
    unsigned long       lIps;     // 在线IP连接数
    unsigned long       lUsers;   // 在线用户数
    ncCluHeat           sHeat;                      // 最后一次通信内容
} ncCluMyInfo;

#endif
