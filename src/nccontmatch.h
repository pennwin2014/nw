/* 模式匹配   
     2007/09/09
 */
#ifndef __NCCONTMATCHDEF___
#define __NCCONTMATCHDEF___
#define NC_MATCH_MAXVAR     512
#define NC_MATCHIP_ALL        0
#define NC_MATCHIP_TCP        6
#define NC_MATCHIP_UDP       17

#define NC_CHECKLEVEL_IPORCONT   1
#define NC_CHECKLEVEL_IPANDCONT  2
#define NC_CHECKLEVEL_IPONLY     3
#define NC_CHECKLEVEL_CONTONLY   4

// Tcp 连接
typedef struct ncMatchIpConnect_s {
    unsigned long  lSip;              /* 内部IP地址                         */
    unsigned long  lDip;              /* 外部IP地址                         */
    unsigned short nDport;            /* 目标端口号                         */
    unsigned short nSport;            /* 源端口号                           */
    unsigned char  cProt;             /* 协议   6-TCP  17 UDP               */
    unsigned char  cConnect;          /* 连接状态  1 Syn  2 建立  9 Close   */
    unsigned char  cDiction;          /* 方向 0 正向   1 反向               */
    unsigned char  cStatus;           /*      0 未知   1 正常      2 禁用   */
    unsigned long  lStartTime;        /* 起始时间                           */
    unsigned long  lLastTime;         /* 最后修改时间                       */
    unsigned char  cFlags;            /* 用于具体协议中的标识               */
    unsigned char  cProxy;            /* 0 -- 正常   1 -- 基于Proxy的协议   */
    unsigned char  cRev[2];
    unsigned long  lSeq;              /* 上行序列号                          */
    unsigned long  lAck_seq;          /* 下行序列号                          */
    unsigned long  lSid;              /* Session ID                          */
    unsigned long  lServiceId;        /* 服务ID                              */
    unsigned long  lAction;           /* 动作ID                              */
    unsigned long  lRight;            /* 权值                                */
} ncMatchIpConnect;

typedef struct ncWordCont_s {
    int      iLen;
    unsigned char     *pData;
    struct ncWordCont_s *next;
} ncWordCont;

// 匹配一个词
typedef struct ncMatchWord_s {
    unsigned short nOffset;          // 偏移量
    unsigned char  cOrder;           // 0--从头开始   1--从结尾开始   2--从当前位置   3--随意
    unsigned char  cFlags;           // 匹配方式      0--精确         1--支持通配符( *--任意  ?--一个位置)   
    unsigned short nSumWord;         // 数量
    unsigned short nRev;
    ncWordCont     *psCont;          // 数据内容
    struct ncMatchWord_s *next;      // 链表的下一个
} ncMatchWord;

// 匹配一个数据包
#define NC_LENTYPE_NOCHECK       0
#define NC_LENTYPE_LE            1
#define NC_LENTYPE_EQ            2
#define NC_LENTYPE_GE            3

typedef struct ncMatchPkg_s {
    unsigned char  cPkgLenType;       // 0-- 不检查    1--小于等于  2--等于  3--大于等于
    unsigned char  cOrder;            // 0-- 顺序无关  1--按排列顺序
    unsigned char  cThisRight;        // 权值
    unsigned char  cRev;
    unsigned short nPkgLen[3];
    int  (*fCheckFun)(ncMatchIpConnect *psTcp,unsigned char *pkg,int iLen,unsigned char *pPar); 
    unsigned char caPar[16];
    ncMatchWord  *psWord;
    struct ncMatchPkg_s *next;
} ncMatchPkg;


typedef struct ncMatchAction_s {
    char                caActionName[16];
    unsigned short      nActionId;
    unsigned char       cActionRight;
    unsigned char       cThisRight;
    ncMatchPkg          *psPkg;
    struct ncMatchAction_s *next;
} ncMatchAction;

typedef struct ncMatchService_s {
    char                    caServiceName[16];       // 服务名称
    unsigned long           lServiceId;
    int                     iCheckLevel;             // 检查级别
    ncMatchAction           *psAction;
    struct ncMatchService_s *next;
} ncMatchService;

/* 处理函数  */
typedef struct ncMatchFun_s {
    char       caFunName[32];
    int  (*fCheckFun)(ncMatchIpConnect *psTcp,unsigned char *pkg,int iLen,unsigned char *pPar); 
    struct ncMatchFun_s *next;
} ncMatchFun;


/* 匹配结果   */
typedef struct ncMatchRet_s {
    ncMatchService      *psService;
    ncMatchAction       *psAction;
    unsigned char       cRight;
    unsigned char       cRev[3];
    struct ncMatchRet_s *next;
}ncMatchRet;

typedef struct ncMatchIp_s {
    unsigned long  lIp;
    unsigned short nPort;
    unsigned char  cProt;
    unsigned char  cMask;
    unsigned long  lServiceId;
}ncMatchIp;


#define NC_MATCH_PUBLIC     1
#define NC_MATCH_ACTION     2
#define NC_MATCH_SERVICE    3

typedef struct ncMatchVar_s {
    int   iVarType;                /* 类型 1--公共信息  2--动作信息  3--服务信息 */
    char  caVarName[16];           /* 变量名                                     */
    char  caVarValue[64];          /* 值                                         */
} ncMatchVar;


typedef struct ncMatchVarBuf_s {
    char  caVarName[16];           /* 变量名                                     */
    char  *pValue;                 /* 值                                         */
    struct ncMatchVarBuf_s *next;
} ncMatchVarBuf;


typedef struct ncMatchVarHead_s {
    ncMatchVarBuf *first;
    ncMatchVarBuf *last;
    int           iSum;
} ncMatchVarHead;


typedef struct ncMatchHead_s {
    int             iStatus;      /* 处理状态  0--原始   1--优化  */
    ncMatchService *psService;
    ncMatchFun     *psFun;
    unsigned char  *pTcp;         /* Tcp 连线信息,Hash表          */
    unsigned char  *pIplist;      /* Ip 列表                      */
    unsigned char  *pVar;         /* 变量信息                     */
    unsigned char  cMask[32];     /* 掩码内容                     */
    int            iSumMask;      /* 掩码总数                     */
} ncMatchHead;


#endif
