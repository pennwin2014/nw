/* 网络督察管理中心客户端  */
#ifndef __NCCLTDEF___
#define __NCCLTDEF___
#include "/home/ncmysql/ncs/src/ncsdef.h"
#define  NCS_MAX_SERVERIP          8
#define  NCS_LOG_FILE             "../log/ncclient.log"
#define  NC_CLT_VERSION           NC_VERSION     // 客户端版本  */
#define  NCS_STA_ERROR             -1      // 未登录
#define  NCS_STA_NOTLOGIN          0       // 未登录
#define  NCS_STA_SUCCESS           1       // 登录成功
#define  NCS_FIL_CASECTL           1       // 案件控制
extern int iNcClient;                      // 0-不连接管理中心  1-连接管理中心
extern int iNcSendGA;                      // 1-把信息传给符合公安标准的管理中心

typedef struct ncsCltInfo_s {
    unsigned long  lId;                   // 客户端ID
    unsigned long  lGid;                  // 类别ID
    unsigned long  lSip;                  // 服务器地址
    unsigned short nTcpPort;              // 服务器端口
    short nStatus;                        // 当前状态
    unsigned long  lLasttime;             // 最后访问时间
    int            iBaseTime;             // 时间偏移量
    unsigned short nMyPort;               // 本人TCP端口
    unsigned short nUdpPort;              // 同步信息 0--不需要 1--需要
    char           caName[32];
    char           caPass[32];             
    char           caKey[32];             // 通信密钥
    char           caMessage[128];        // 最后操作
    unsigned long  lSynCheck;             // 校验码
    unsigned long  lTimeStep[2];          // 消息发送时间间隔
    ncsSynInfo     syninfo[5];            // 同步信息  4--保存向上同步的信息
    ncIpAddress    sNcsAdd[NCS_MAX_SERVERIP];   // 网络督察管理中心
    ncIpAddress    sProAdd[NCS_MAX_SERVERIP];   // 网络督察代理服务器
    ncIpAddress    sDsAdd[NCS_MAX_SERVERIP];    // 数据中心服务器
    unsigned  long lCaseTime;                   // 案件最后更新时间
    unsigned  long lMsgTime;                    // 消息接收最后时间
    unsigned  long lMsgSndTime;                 // 消息发送最后时间
    unsigned  long lSumCaseIndex;               // 案件索引总数
    unsigned  long lDbSndStime;                 // 发送日志信息的最后时间--起始时间
    unsigned  long lDbSndEtime;                 // 发送日志信息的最后时间--终止时间
    unsigned  char cModMon;                     // 1--规则更新   0--规则未更新 9--正在进行
    unsigned  char cModCtl;                     // 1--规则更新   0--未更新     9--正在进行
    unsigned  char cSynTime;                    // 是否同步时钟
    unsigned  char cRev1;
    unsigned  long lComTime;                    // 命令最后同步时间
    unsigned  long lTopCaseTime;                  // 上上级管理中心的CaseTime  LLastCaseTime
    unsigned  char cRev[52];                    // 保留
} ncsCltInfo;

#define NC_CSYN_MAX         2
#define NC_CSYN_GROUP       0
#define NC_CSYN_USER        1

/* 实时数据发送缓冲区   */
typedef struct ncsCltSendBuf_s {
    unsigned long  lMax;                  // 总字节数
    unsigned long  lPut;                  // 保存位置
    unsigned long  lSend;                 // 发送位置
    unsigned long  lSendTime;             // 最后发送时间
    unsigned long  lPutTime;              // 数据最后保存时间
    long           lBytes;                // 当前未发送的数据
    int            iLock;
    unsigned long  lLockTime;             // 记录上锁时间
} ncsCltSendBuf;

/* 各种过滤规则  */
typedef struct ncCltFilterIndex_s {
    unsigned long    lPos[2];       // 起始位置 0 实时监控  1 日志
    unsigned short   nLen[2];       // 记录长度
    unsigned short   nSum[2];       // 记录总数
    unsigned long    defsend[2];    // 缺省是否发送
    unsigned long    enable[2];     // 规则是否启用
    char             *pBuf[2];      // 对应的指针          
} ncCltFilterIndex;


typedef struct ncCltMonIp_s {
    unsigned long  sip;
    unsigned long  eip;
    unsigned long  sport;
    unsigned long  dport;
    unsigned long  service;
    unsigned long  prot;
    unsigned long  flags;
} ncCltMonIp;

typedef struct ncCltMonWeb_s {
    char           url[64];
    unsigned long  urlid;
    unsigned long cond;
    unsigned long flags;
} ncCltMonWeb;

typedef struct ncCltMonMail_s {
    char            sender[64];
    unsigned long   sendid;
    char            toname[64];
    unsigned long   toid;
    char            subject[64];
    char            attname[64];
    unsigned long   msize;
    char            content[64];
    int             flags;
} ncCltMonMail;


typedef struct ncCltMonIm_s {
    unsigned long   service;
    char            uname[64];
    unsigned long   uid;
    unsigned long   fun;
    char            content[64];
    int             flags;
} ncCltMonIm;

typedef struct ncCltMonForm_s {
    char            url[64];
    unsigned long   urlid;
    char            varname[256];
    char            content[256];
    char            attname[64];    /* 附件名称  */
    int             asize;          /* 附件大小  */
    int             flags;
} ncCltMonForm;


typedef struct ncCltMonFtp_s {
    unsigned long   service;
    unsigned long   sip;
    unsigned long   eip;
    int             flags;
} ncCltMonFtp;

typedef struct ncCltMonBbs_s {
    char            url[64];
    unsigned long   urlid;
    char            bbsname[64];
    char            content[64];
    int             flags;
} ncCltMonBbs;


/* 邮件或用户ID地址列表  

 */
#define NCS_MAX_INFOLIST     4
#define NCS_INFO_MAILLIST    0
#define NCS_INFO_USERIDLIST  1

typedef struct ncCltInfoIndex_s {
    unsigned long lStart;
    unsigned long lSum;
} ncCltInfoIndex;

typedef struct ncCltInfoList_s {
    unsigned long lId;
    char      name[64];       /* 邮件地址  */
} ncCltInfoList;

// 案件布控有关数据结构///////////////////////////

typedef struct ncCltCaseCtlH_s {
    unsigned long lFrom;
    unsigned long lCid;
    unsigned long lSid;
} ncCltCaseCtlH;

// 基于用户特征, 用Hash表
typedef struct ncCltCaseUser_s {
    ncCltCaseCtlH sCase;
    ncsCaseUser   sUser;
} ncCltCaseUser;


// 基于虚拟账号, 用Hash表
typedef struct ncCltCaseNetId_s {
    ncCltCaseCtlH sCase;
    ncsCaseNetId  sNetId;
} ncCltCaseNetId;

// 基于IP地址, 一般数组, 按大小排序
typedef struct ncCltCaseIp_s {
    ncCltCaseCtlH sCase;
    ncsCaseIp     sIp;
} ncCltCaseIp;

// 基于访问网址, 匹配
typedef struct ncCltCaseWeb_s {
    ncCltCaseCtlH sCase;
    ncsCaseWeb    sWeb;
} ncCltCaseWeb;


// 基于访问表单
typedef struct ncCltCasePost_s {
    ncCltCaseCtlH sCase;
    ncsCasePost   sPost;
} ncCltCasePost;


//  聊天内容
typedef struct ncCltCaseIm_s {
    ncCltCaseCtlH sCase;
    ncsCaseIm     sIm;
} ncCltCaseIm;

// 邮件内容
typedef struct ncCltCaseMail_s {
    ncCltCaseCtlH sCase;
    ncsCaseMail   sMail;
} ncCltCaseMail;

// 邮件内容
typedef struct ncCltCaseWarn_s {
    ncCltCaseCtlH sCase;
    ncsCaseWarn   sWarn;
} ncCltCaseWarn;

typedef struct ncCltCaseSens_s {
    ncCltCaseCtlH sCase;
    ncsCaseSens   sSens;
} ncCltCaseSens;


typedef struct ncCltCaseBuf_s {
    unsigned long  lSum[NCS_MAX_CASECONT];    /* 记录数              */
    unsigned short nLen[NCS_MAX_CASECONT];    /* 每条记录的长度      */
    unsigned long  lPos[NCS_MAX_CASECONT];    /* 所在位置            */
    unsigned long  lCur[NCS_MAX_CASECONT];    /* 当前位置            */
    unsigned char  *pBuf;                     /* 每条记录的起始指针  */
} ncCltCaseBuf;

typedef struct ncCltCaseIndex_s {
    unsigned long lCfrom;
    unsigned long lCid;
    unsigned long lLastTime;
    int           iFlags;
} ncCltCaseIndex;

#endif
