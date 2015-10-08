// 文件服务器的数据结构

#ifndef __NCFILESERVERDEF___
#define __NCFILESERVERDEF___
#include "pasmessage.h"
#define  NCFSS_VERSION    "V2.00.02"
#define  NCFSS_RELEASE    "2013/06/18"

#define NC_LNK_FSSERVERLNK    146
#define NCFSS_FILEBUF_LEN     8192
#define NCFSS_MAX_EVENT       512

#define NCFSS_FUN_SERVERREADY       210000000L         // 服务器准备就绪
#define NCFSS_FUN_FILESEND          210008001L         // 文件上传
#define NCFSS_FUN_FILEDOWN          210008002L         // 文件下载

#define NCFSS_FUN_REQSEND           210008003L         // 请求上传文件
#define NCFSS_FUN_ENDSEND           210008004L         // 数据传送结束
#define NCFSS_FUN_REQDOWN           210008005L         // 请求下传文件
#define NCFSS_FUN_ATTFILESTART      210008006L         // 附件上传开始
#define NCFSS_FUN_SYNCLIENT         210008101L         // 同步客户端信息



#define NCFSS_OPT_FILEUPLOAD    1                  // 文件上传
#define NCFSS_OPT_FILEDOWNLOAD  2                  // 文件下传
#define NCFSS_OPT_FILEDO        3                  // 文件处理

typedef struct ncFSSClient_s {
    unsigned long lDid;                          // 客户端ID
    char          caName[32];                    // 客户端名称
    char          caDispName[64];                // 显示名
    char          caProdSn[16];                  // 产品序列号
    char          caDoTime[64];                  // 处理时间 
    unsigned long lSip;                          // 客户端IP地址
    unsigned long lLastSynTime;                  // 最后一次同步时间
    unsigned long lLastTime;                     // 最后操作时间
    unsigned long lDataTime;                     // 最后生成时间
    unsigned char AuthCode[16];                  // 授权码
    unsigned long lBaseTime;                     // 上一时间点时间
    unsigned long long lBaseBytes[2];            // 上一个时间点的流量   0--上传  1--下行
    unsigned long long lBytes[2];                // 字节数
    unsigned long lFiles[2];                     // 文件数量
    int           iFd;                           // 通信句柄
    unsigned long lLastFdTime;                   // 最后通信时间
    unsigned char cStatus;                       // 0--不在线       1--在线
    unsigned char cData;                         // 0--不在传数据   1--正在传数据
    unsigned char cKeep;                         // 0--正常         1--长期保留
    unsigned char cDo;                           // 0--不允许       1--允许传送  2--遇空传
} ncFSSClient;

/* 客户端状态信息  */
typedef struct ncFSSClientStatus_s {
    unsigned long lDid;                          // 客户端ID
    char          caName[32];                    // 客户端名称
    char          caDispName[64];                // 显示名
    char          caProdSn[16];                  // 产品序列号
    char          caDoTime[64];                  // 数据上传时间
    unsigned long lSip;                          // 客户端IP地址
    unsigned char AuthCode[16];                  // 授权码
    unsigned char cStatus;                       // 0--离线  1--上线   2--更新
    unsigned char cKeep;                         // 1--长期保留
    unsigned char cDo;                           // 0--不允许   1--允许
    unsigned char cRev;
} ncFSSClientStatus;

typedef struct ncFSSProcess_s {
    int           iPid;                            // 进程号
    unsigned long lDid;                            // 对应的客户端ID
    unsigned long lMyNum;                          // 本记录的序号
    unsigned long lStartTime;                      // 进程起始时间
    unsigned long lLastTime;                       // 最后操作时间
    unsigned char cStatus;                         // 0--空闲  2--已经被占用 1--正在工作
    ncFSSClient   *psClient;
    unsigned char cRev[3];    
} ncFSSProcess;

typedef struct ncFSSDataProcess_s {
    int           iPid;                            // 进程号
    char          caName[16];                      // 进程名称
    unsigned long lStartTime;                      // 进程起始时间
    unsigned long lLastTime;                       // 最后操作时间
    unsigned long lCount;                          // 处理记录数
    int      (*ncFssDoProcess) (utShmHead *psShmHead,char *pArg);
    char          caArg[32];
} ncFSSDataProcess;

#define NCFSS_MAX_DATAPROCESS                   32

typedef struct ncFSSHead_s {
    unsigned long   lMaxProcess;
    unsigned long   lSumProcess;
    ncFSSProcess    *psProcess;
    unsigned char   *pClientHash;
    char            caUploadPath[128];             // 上传文件存放的路径
    char            caAttPath[128];                // 附件存放的路径
    char            caDownPath[128];               // 下载路径
    unsigned short  nDataPort;                     // 数据服务器端口
    unsigned short  nCtlPort;                      // 控制服务器端口
    int             iMaxCtlSock;
    int             iSumCtlSock;
    char            caDoTime[128];                 // 数据上传时间
    char            caDownTime[128];                 // 数据下传时间
    int             iLockDoDb;                     // 0--正常操作  1--暂停操作
    unsigned long   lLockTime;                     // 锁定时间
    unsigned long   lLockTimeOut;                  // 锁定超时
    char            caName[256];                   // 要锁定的表
    int             iSumDataProcess;               // 数据处理进程总数
    unsigned long   lStartTime;                    // 进程启动时间
    ncFSSDataProcess sDataProc[NCFSS_MAX_DATAPROCESS];
} ncFSSHead;



ncFSSHead    *ncFSSGetHead(utShmHead *psShmHead);  
ncFSSProcess *ncFSSGetProcessNum(ncFSSHead *psFSSHead);
ncFSSClient  *ncFSSGetClient(ncFSSHead *psHead,unsigned long lDid);
ncFSSClient  *ncFSSCheckClient(ncFSSHead *psHead,pasMsgHead *psMsgHead);


#define NCFSC_MATCH_STRING               0
#define NCFSC_MATCH_REGEX                1

#define NCFSC_SEND_PAS                   1
#define NCFSC_SEND_FTP                   2

typedef struct ncFSCForwardScode_s {
    uchar     caCode1[128];                // 场所端编码的条件, 下限
    uchar     caCode2[128];                // 场所端编码的条件, 上限,若为正则,保存编译后的结果
    uchar     cType;                       // 0--一般字符匹配   1--正则表达式
    uchar     status;                      // 0--错误           1--正确
    uchar     cRev[2];
} ncFSCForwardScode;
    
typedef struct ncFSCForwardClient_s {
    uchar  caName[32];                       // 场所端名称
    uint4  lSip[4];
    uint2  nPort[4];
    uchar  cSendType[4];                     // 传送方式    PAS     FTP
    uchar  caUsername[4][32];                // 登录的用户名
    uchar  caKey[4][32];                     // 通信密钥
    uint4  lMyid[4];                         // 自己的ID
    uchar  caDoTime[128];                    // 数据传送时间
    uint4  iStepTime;                        // 传送时间间隔
    uint4  iSumCode;                         // 场所端总数
    struct ncFSCForwardScode_s  *psCode;     // 场所端编码范围
} ncFSCForwardClient;
    

/* 转发信息

*/
typedef struct ncFSCForwardInfo_s {
    char                caDataPath[4][128];                  // 日志数据保存的路径,  可以有多个路径
    char                caOnlineFile[4][128];                // 在线用户
    char                caRemote[4][128];
    char                caKeepPath[4][128];
    uchar               cKeep[4];                            // 保留标志   1--保留  0--不保留
    ncFSCForwardClient  *psClient;
    int4                iSum;
} ncFSCForwardInfo;


#endif
