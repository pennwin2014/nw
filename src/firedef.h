#ifndef __PronetwayFireDef__
#define __PronetwayFireDef__


#define FIRE_TYPE_DEFAULT    0
#define FIRE_TYPE_FORWARD    1
#define FIRE_TYPE_INPUT      2
#define FIRE_TYPE_OUTPUT     3
#define FIRE_TYPE_IPTABLES   4
#define FIRE_TYPE_RULE       5
#define FIRE_TYPE_PLATE      6
#define FIRE_TYPE_DEFINE     7
#define FIRE_TYPE_SNAT       8
#define FIRE_TYPE_NATIP      9
#define FIRE_TYPE_NATPORT    10
#define FIRE_TYPE_SYSTEM     11

#define FIRE_JUMP_ACCEPT     0
#define FIRE_JUMP_DROP       1
#define FIRE_JUMP_LOG        2

#define FIRE_PROT_ALL        0
#define FIRE_PROT_TCP        1
#define FIRE_PROT_UDP        2
#define FIRE_PROT_ICMP       3

#define FIRE_ID_DELETE       0    /* 删除  */
#define FIRE_ID_SELECTIP     1
#define FIRE_ID_INPUTIP      2
#define FIRE_ID_NAT          3
#define FIRE_ID_RULE         4

/* IP Table内容  */
typedef struct fireIptables_s {
    int      iType;            /* 类型  0--Define   1--Default
                                        2--Forward  3--Input
                                        4--Output   5--Rule
                                        6--Plate
                                */
    unsigned long lNum;        /* 序号                      */
    unsigned long lId;         /* ID  id=0 已被删除 1--基本 2--高级  3--NAT  4--Rule        */
    char     caName[32];       /* 名称             */
    char     caCommand[512];   /* Ip Tables的命令  */
} fireIptables;

typedef struct FireSummary_s {
    int  iDefine;
    int  iSumTables;
    int  iMaxTables;
    int  iPlate;
    int  iForward;     /* Forward的缺省策略 */
    int  iInput;       /* Input的缺省策略   */
    int  iOutput;      /* Output的缺省策略  */
    int  iModify;
    int  iAutoStart;
    char *pDefine;     /* 定义变量句柄      */
    fireIptables *psTables;
} fireSummary;

typedef struct fireNetList_s {
    char     caDes[64];             /* 描述  */
    unsigned long lIp;
    unsigned long lMask;
    struct fireNetList_s *psNext;
} fireNetList;


typedef struct fireNetDev_s {
    char     caDev[32];             /* 接口名称  */
    unsigned long lIp;              /* IP地址    */
    char     caDes[64];             /* 描述      */
    fireNetList  *psNetList;
} fireNetDev;

/* 名字、值  */
typedef struct fireVarValue_s {     
    char caVar[32];
    char *pValue;
    struct fireVarValue_s *psNext;
} fireVarValue;

    
/* 命令  */
typedef struct fireParam_s {
    char          caName[32];         /* 命令名称  */
    fireVarValue  *psParm;            /* 参数内容  */
} fireParm;
   

typedef struct fireDefault_s {
    char    caChain[32];     /* Chain Name */
    char    caPolicy[32];   /* 策略       */
} fireDefault;

/* 转发的数据包  */
typedef struct fireForward_s {
    unsigned long lId;    /* ID        */
    char    caSip[64];    /* 源地址    */
    char    caDip[64];    /* 目标地址  */
    char    caSport[32];  /* 源端口    */
    char    caDport[32];  /* 目标端口  */
    char    caProtocol[32];     /* 协议  */
    char    caJump[32];         /* 跳转  */
    char    caStatus[64];       /* 状态  */
    char    caLimit[64];        /* 限制包的数量  */
    char    caBurst[64];        /* 触发峰值   */
    char    caFragment[64];     /* 碎片   */
    char    caIndev[64];        /* 输入设备  */
    char    caOutdev[64];       /* 输出设备  */
    char    caSyn[64];          /*        */
    char    caLog[64];          /* 日志   */
} fireForward;



typedef struct fireDefine_s {
    char caName[32];
    char caDispname[32];
    char caValues[128];
} fireDefine;

/* 错误信息  */
struct fireErrormsg_s {
    int     iIndex;                /* 索引号    */
    char    *pMsg;                 /* 错误信息  */
    struct fireErrormsg_s *psNext;    
};


typedef struct  fireError_s {
    int iSum;                      /* 错误总数  */
    struct fireErrormsg_s *psHead;
    struct fireErrormsg_s *psEnd;
} fireError;



/* fire001.c */
int fireWebDispDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebListBaseInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebModiDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebRuleEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebRunScript(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebUnRunScript(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebSnatEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebNatipEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebNatportEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
#endif
