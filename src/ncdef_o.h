#ifndef __NETCHARGDEF__
#define __NETCHARGDEF__
#define   NC_IPCONTROL_ID       918123123L
#define   NC_MAX_PROCESS        16   /* 最大进程类型  */
#define   NC_MAX_CONTROL        32   /* 最大控制变量  */
#define   NC_MAX_ONTIMEDO       16   /* 最大定时函数  */
#define   NC_DEFAULT_IP_CHECK   20
#define   NC_DEFAULT_IP_TIMEOUT 30
#define   NC_MAX_USERNUM      257
#define   NC_MAX_IPPKG        9999
#define   NC_MAX_HTTPNUM      999
#define   NC_MAX_LIMITED      100
#define   NC_MAX_MONITOR      100
#define   NC_MAX_INTERNAL     100
#define   NC_MAX_LASTIP       1000
#define   NC_CONTENT_GET      10
#define   NC_CONTENT_POST1    11
#define   NC_CONTENT_POST2    12
#define   NC_CONTENT_POST3    13
#define   NC_CONTENT_SMTP1    21
#define   NC_CONTENT_SMTP2    22
#define   NC_CONTENT_POP3     23
#define NC_PF_UNUSED       0
#define NC_PF_INUSED       1
#define NC_LNK_THIRDAUTH  25  /* 第三方认证的用户  */
#define NC_LNK_ALLUSER    26  /* 全部用户信息      */
#define NC_LNK_MAILCTL    27  /* 邮件控制          */
#define NC_LNK_CONTROL    28  /* 访问控制          */
#define NC_LNK_IPFLOW     29  /* 总流量            */
#define NC_LNK_MAIL       30  /* 邮件数据包        */
#define NC_LNK_IPPKG      31  /* IP数据包          */
#define NC_LNK_WEB        32  /* http数据包        */
#define NC_LNK_LIMIT      33  /* 限制使用的IP地址  */
#define NC_LNK_NETWORK    34  /* 要监控的IP地址  Add 2002/9/6 By Liyunming Replace 
                                          NC_LNK_MONITOR NC_LNK_INTERNAL */

/* #define NC_LNK_MONITOR    34 
   #define NC_LNK_INTERNAL   35 
      Delete 2002/9/6 Replace by NC_LNK_NETWORK
 */
#define NC_IPCONTROL_BY_ICMP      0
#define NC_IPCONTROL_BY_IPTABLES  1

#define NC_LNK_RELOADCONTROL 35 /* 重新装载控制  */
#define NC_LNK_USERINFO   36  /* 用户信息          */
#define NC_LNK_SERVICE    37  /* 众所周知的端口    */
#define NC_LNK_LASTIPNUM  38  /* 最后访问的1000个IP数据包 0--当前总数 1--IP总数 2--Http总数  */
#define NC_LNK_WEBLIMIT   39  /* Web访问限制       */

#define NC_SERVICE_OTHER  0    /* 其它服务           */
#define NC_SERVICE_TELNET 1    /* Telnet 服务        */
#define NC_SERVICE_FTP    2    /* Ftp    服务        */
#define NC_SERVICE_HTTP   3    /* Web服务            */
#define NC_SERVICE_HTTP1  300  /* Web服务            */
#define NC_SERVICE_SMTP   4    /* SMTP服务           */
#define NC_SERVICE_POP    5    /* POP服务            */
#define NC_SERVICE_ICQ    6    /* ICQ                */
#define NC_SERVICE_SSH    7    /* ssh                */
#define NC_SERVICE_IRC    8    /* IRC                */

#define NC_PROTOCOL_ICMP  1
#define NC_PROTOCOL_IGMP  2
#define NC_PROTOCOL_TCP   6
#define NC_PROTOCOL_UDP   17

/*   Define the Debug Level   */
#define NC_DEBUG_SHOWTCP      1
#define NC_DEBUG_SHOWCONTROL  2
#define NC_DEBUG_SHOWUDP      3
#define NC_DEBUG_SHOWICMP     4
#define NC_DEBUG_SHOWIGMP     5
#define NC_DEBUG_SHOWCHECKMAC 6
#define NC_DEBUG_SHOWSMTP     7
#define NC_DEBUG_SHOWPOP      8
#define NC_DEBUG_NTAUTH       9
#define NC_DEBUG_UPLOAD       10
#define NC_DEBUG_IPCONTROL    11
#define NC_DEBUG_MAILAUDIT    12     /* 邮件审计  */
#define NC_DEBUG_MAILNON      13     /* 邮件监控  */
#define NC_DEBUG_TIMECTL      14     /* 时间控制  */
#define NC_DEBUG_MSGINFO      15     /* 显示数据包   */
#define NC_DEBUG_CHECKNETIP   16     /* 检查网络情况 */
#define NC_DEBUG_THIRDAUTH    17     /* 第三方验证   */

/*针对cStatus而言 */
#define NC_USER_NOUSE      0        /* Nouse    */
#define NC_USER_NORMAL     1        /* Normal   */
#define NC_USER_TIMEOUT    2        /* 用户超时 */
#define NC_USER_NOEXIST    3        /* 用户不存在  */
#define NC_USER_MACERROR   4        /* MAC地址不一致  */
#define NC_USER_FORBID     7        /* 禁用           */

typedef struct pasSearchHead_s {
    char caFname[32];        /* 字段名               */
    char caDispname[32];     /* 显示名               */
    int  iSize;
    int  iMaxLen;
    int  iType;              /* 输入类型  1-文本  2-选择框  */
    char *pValue;
} pasSearchHead;

/* 内存中的用户信息  */
typedef struct ncUserBuf_s {       /* 用户信息       */
    long     lUserid;              /* 用户Id  0--没用         */
    char     caUsername[16];       /* 姓名           */
    char     caDispname[16];       /* 显示名         */
} ncUserBuf;

/* 第三方认证的用户信息  */
typedef struct ncThirdAuth_s {
    char          caUsername[16];    /* 用户名                */
    int           iIndex;            /* 在用户信息中的索引号  */
    unsigned long lIp;               /* IP地址 0 --表示没有内容 */
    unsigned long lLastTime;         /* 最后更新时间          */
} ncThirdAuth;


typedef struct ncUserInfo_s {      /* 用户信息       */
    char     caGroup[16];          /* 部门           */
    char     caName[16];           /* 姓名           */
    char     caDispname[16];       /* 显示名         */
    long     lId;                  /* 用户Id  0--没用         */
    unsigned long  lGid;           /* 组Id         */
    unsigned char  cUseflags;      /* 使用状态0--正常 1--禁用  9--免监控            */
    unsigned char  cStatus;        /* 0--没用 1--正常  2--用户不存在  3--MAC不一致  9--删除 */
    unsigned char  mac[6];         /* MAC地址        */
    unsigned long  lIp;            /* 0 表示没有用户 */
    unsigned long  lStartTime;     /* 开始连接时间   */
    unsigned long  lLastTime;      /* 最后访问时间   */
    short    nWeb;                 /* 网站限制访问   */
    short    nLimit;               /* IP限制访问     */
    unsigned long lDo;             /* 正在访问的内容 */
    long8    lBytes[2];            /* 0-总的上行流量 1--下行流量 从连接时间开始计算  */
    unsigned long lConntime;       /* 连接时间, 从计到数据库到当前的时间             */
    unsigned long lSumtime;        /* 从开始计时到当前时间总数                       */
    long8    lSumflow;             /* 从开始计时到当前流量总数                       */
    unsigned short nGroup;         /* 在Group中的序号   */
    unsigned short nReserv;        /* 保留  */
    unsigned long  lReserv;      /* 保留                                           */
    unsigned long lRandWidth;      /* 带宽限制       */
    unsigned long lRandLower;      /* 带宽下限       */
    long8    lLimitBytes;          /* 限制流量 0--不限制   */
    unsigned long lLimitTimes;     /* 限制时间 0--不限制   */
} ncUserInfo;



/* 用户同步信息   */
typedef struct ncNtUser_s {        /* NT用户信息     */
    char          caName[32];      /* 姓名           */
    unsigned long lIp;             /* IP地址         */
    int           iStatus;        /* 1--登陆  0--退出  */
} ncNtUser;


/* 表的结构信息  */
typedef struct ncDbField_s {
    int  iFlags;
    char caField[32];
    char caType[32];
    char caNull[32];
  	char caKey[32];
  	char caDefault[64];
  	char caExtra[32];
} ncDbField;

typedef struct ncDbIndex_s {
    int  iFlags;
    char caIndex[32];
    char caUnique[4];    /* 0-- Unique  1-- No */
    char caSeq[4];       /* 序号               */
    char caField[32];
} ncDbIndex;


  	
#define NC_NETWORK_NOMAL          0    /* 正常访问    */
#define NC_NETWORK_INTERNET       1    /* Internet网  */
#define NC_NETWORK_IPBASE         2    /* 基于IP地址  */
#define NC_NETWORK_MACBASE        3    /* 基于MAC地址 */
#define NC_NETWORK_AUTH           4    /* 本机验证    */
#define NC_NETWORK_NTAUTH         5    /* 第三方验证  */
#define NC_NETWORK_NOMON          6    /* 免监控      */
#define NC_NETWORK_FORBID         7    /* 禁用        */

typedef struct ncIpNetwork_s {      /* 内部IP地址    */
    unsigned long   lIpLower;       /* 起始IP地址    */
    unsigned long   lIpUpper;       /* 终止IP地址    */
    unsigned long   lAuthIp;        /* 验证服务器IP地址  */
    unsigned short  nAuthPort;      /* 验证服务器端口号  */
    unsigned short  iFlags;         /* 见宏定义      */
} ncIpNetwork;

/* Ip的头部信息  */
typedef struct ncIpHead_s {
	unsigned long  lSip;       /* 源IP地址     */
	unsigned long  lDip;       /* 目标IP地址   */
	unsigned short nDport;     /* 目标端口     */
	unsigned short nSport;     /* 源端口       */
	char     caSmac[6];        /* 源MAC地址    */
	char     caDmac[6];        /* 目标MAC地址  */
	short    cTran;            /* 传送方向 0--正常 1--反向    */
	short    cProt;            /* 协议  6 UDP      17 TCP     */
} ncIpHead;

/*   Delete 2002/9/6  Replace with ncIpNetwork 
typedef struct ncIpInternal_s {      
    unsigned long   lIpLower;          
    unsigned long   lIpUpper;    
} ncIpInternal;
    

typedef struct ncIpMonitor_s {   
    unsigned long   lUid;        
    unsigned long   lIpLower;    
    unsigned long   lIpUpper;    
    unsigned short  nTranType;   
    unsigned short  nPort;       
} ncIpMonitor;

*/

/* 限制访问  */
typedef struct ncIpLimited_s {       /* 限制访问的Ip地址  */
    unsigned long   lUid;              /* 用户Id  0 表示全部  */
    unsigned long   lIpLower;          /* 下限  */
    unsigned long   lIpUpper;          /* 上限  */
    unsigned char   cTranType;         /* 传送类型  0--全部   1--UDP  2--TCP          */
    unsigned char   cTimeType;         /* 时间类型                                    */
    unsigned short  nLowerport;        /* 端口号下限                                  */
    unsigned short  nUpperport;        /* 端口号上限                                  */
    unsigned char   cService;          /* 服务类型                                    */
    unsigned char   cFlags;            /* 限制方式 1--外部网  2--服务  3--IP地址和端口 */
    unsigned long   lStart;            /* 起始时间 HHMMSS                             */
    unsigned long   lEnd;              /* 终止时间 HHMMSS                             */
    unsigned char   cVister;           /* 0 禁止   1 允许                             */
    unsigned char   cReserve[3];       /* 保留  */
} ncIpLimited;

typedef struct ncWebLimited_s {       /* 限制访问的网站     */
    unsigned long   lUid;              /* 用户Id  0 表示全部  */
    char            caUrl[64];         /* 网站地址            */
    unsigned long   lStart;            /* 起始时间 HHMMSS                             */
    unsigned long   lEnd;              /* 终止时间 HHMMSS                             */
    unsigned char   cTimeType;         /* 时间类型                                    */
    unsigned char   cVister;           /* 0  禁止  1 允许                             */
    unsigned char   cReserve[2];       /* 保留  */
} ncWebLimited;


typedef struct ncTcpBuffer_s {
    unsigned long  lSip;             /* 源IP地址     */
    unsigned long  lDip;             /* 目标IP地址   */
    unsigned short nSport;           /* 源端口号     */
    unsigned short nDport;           /* 目标端口号   */
    unsigned long  lStartTime;       /* 起始时间     */
    unsigned long  lLastTime;        /* 最后修改时间 */
    long8          lBytes[2];        /* 0-上行字节数  1-下行字节数 */
    unsigned char  cType;            /* 协议         */
    unsigned char  cService;         /* 服务         */
    unsigned short nUrl;             /* 访问http     */
    unsigned char  caMac[6];         /* 源Mac地址    */
    unsigned char  cUseFlags;        /* 使用标识 0--没内容 1--有内容 2--已处理 3--已保存   */
    unsigned char  cFlags;           /* 标识 0--正常 1--数据  */
    unsigned long  lCount;           /* 记数器                */
} ncTcpBuffer;

typedef struct ncHttpBuffer_s {
    unsigned long  lSip;             /* 源IP地址     */
    unsigned long  lStartTime;       /* 起始时间     */
    unsigned long  lLastTime;        /* 起始时间     */
    long8          lBytes;           /* 流量         */
    unsigned char  caMac[6];         /* 源Mac地址    */
    unsigned char  cUseFlags;        /* 标识，0--无内容 1--有内容  3--已保存  */
    unsigned char  cReserve;
    char  caHost[64];
    char  caUrl[128];
} ncHttpBuffer;

typedef struct ncService_s {
    char            caName[32];      /* 服务名称  */
    unsigned char   cServices;       /* 服务类型  */
    unsigned char   cProtocol;       /* 协议      */
    unsigned short  nPort;           /* 端口号    */
} ncService;

typedef struct ncMailSubList_s {
    char pName[64];
    struct ncMailSubList_s *psNext;
} ncMailSubList;


/* 邮件监控过滤规则  */
typedef struct ncMailFilter_s {
    char   caRname[64];
    char   caFrom[64];
    char   caTo[64];
    char   caSub[64];
    char   caCont[64];
    char   caAtt[64];
    int    lSize;
    int    lAsize;
    short  nFlags;
    short  nSflags;
    char   caSmail[64];
} ncMailFilter;


typedef struct ncUploadFilter_s {
    ncMailSubList *Name;
    ncMailSubList *Host;
    ncMailSubList *Content;
    ncMailSubList *Attachname;
    unsigned long lSize;
} ncUploadFilter;



typedef struct ncPostFileList_s {
    char     pOriginFile[128];   /* 源文件    */
    char     pType[12];          /* 文件类型  */
    char     pOutFile[64];
    unsigned long lSize; 
    struct ncPostFileList_s *psNext;   
} ncPosFileList;


typedef struct ncPostDataContent_s {
    unsigned long  lBytes;         /* 总字节数  */
    unsigned long  lTime;          /* 时间      */
    char           caName[32];     /* 用户名    */
    char           caHost[128];    /* 主机名    */
    char           caUrl[128];     /* 访问URL   */
    char           *pValue;        /* 数据内容  */
    int            iSum;           /* 附件总数  */
    ncPosFileList  *psFileList;    /* 文件列表  */
} ncPostDataContent;


#define NC_IPPKG_UPFLOW      0
#define NC_IPPKG_DOWNFLOW    1

typedef struct ncIpPkgInfo_s {
    long8    lIpFlow[2];     /*  0 Upload Bytea  1  DownBytes  */
    unsigned long lStartTime;     /*  起始时间                      */
    long8    lSumPkg;        /*  总数据包                      */
    unsigned long lpPkgPerSec;    /*  每秒数据包                    */
    unsigned long lStartCountTime; /* 记数的起始时间                */
    long8    lPkgCount;      /*  数据包记书数                  */
    unsigned long lCount;         /*  访问次数                      */
    unsigned long lStartControl;  /*  时间控制的起始时间            */
    char          caFilename[128];
} ncIpPkgInfo;

typedef struct ncSumControl_s {
    unsigned long  lStartTime;       /* 起动时间                     */
    unsigned long  lLastTime;        /* 最后操作时间                 */
    unsigned long  lTimeOut;         /* 最长超时时间                 */        
    unsigned short nLock;            /* 锁  0--正常   1--正在检查    */
    unsigned short nR1;              /*                               */
    unsigned long  lR1[2];           /* 保留                          */
} ncSumControl;

/* 控制信息 从1开始  */
#define NC_CONTROL_NOUSE           0
#define NC_CONTROL_DELETE          1
#define NC_CONTROL_TABDROP         5
#define NC_CONTROL_MODPORT         6


/* Iptables 的控制台命令           */
#define NC_TABLES_ADDDROP          0     /* 增加一条禁止规则      */
#define NC_TABLES_CHECKTABLE       1     /* 检查iptables 的内容   */
#define NC_TABLES_RESETTABLE       2     /* 重置Iptables的内容    */
#define NC_TABLES_REDICTPORT       3     /* 端口重定向            */
#define NC_TABLES_CANCELREDICT     4     /* 取消端口重定向        */
#define NC_TABLES_SIGNAL           5     /* 往指定进程发消息      */
#define NC_TABLES_REMOTEFILE       6     /* 远程文件系统          */

struct ncTestRemote_s {     /* 远程文件测试  */
    char  caIp[16];
    char  caHost[64];
    char  caUser[32];
    char  caPass[32];
    char  caMount[64];
    char  caShare[32];
    char  caSdate[12];
    char  caEdate[12];
    char  caOpt[12];       /* 内容       */
};
/* 当为RemoteMount时 cPort的值为具体的命令  */
#define NC_REMOTE_MOUNT         1
#define NC_REMOTE_MOUNTTEST     2
#define NC_REMOTE_MAILBACK      3
#define NC_REMOTE_UNMOUNT       4
#define NC_REMOTE_MAILLOAD      5
#define NC_REMOTE_MAILEXPORT    6    /* 数据导出  */
typedef struct ncControl_s {
    unsigned long  lSip;             /* 起始IP    */
    unsigned long  lDip;
    unsigned short nDport;
    unsigned char  cProt;            /* 协议                         */
    unsigned char  cUse;             /* 见上面的宏                   */
    unsigned long  lLastTime;        /* 起始时间                     */
    unsigned long  lStop;            /* 终止时间                     */
    int            iIndex;
} ncControl;

typedef struct ncIpControlInfo_s {
    unsigned long  lId;              /* ControlId                   */
    unsigned long  lSip;             /* 起始IP                      */
    unsigned long  lDip;
    unsigned long  lStop;            /* 终止时间 hhmmss             */
    unsigned short nDport;
    unsigned char  cProt;            /* 协议                        */
    unsigned char  cComm;            /* 命令 0--Add 1--Del 2--Flush */    
} ncIpControlInfo;

typedef struct ncDevList_s {
    char caDev[32];
    char caIp[32];
    char caNetmask[32];
    char caBoard[32];
    struct ncDevList_s *next;
} ncDevList;

    
#define  NC_PID_COLLECT        0     /* 数据采集    */
#define  NC_PID_MAILFILTER     1     /* 邮件监控    */
#define  NC_PID_TABLECTL       2     /* IpTable控制 */
#define  NC_PID_NTSYNC         3     /* 域同步      */
#define  NC_PID_ONTIME         4     /* 定时        */
#define  NC_PID_ICAUTH         5     /* 第三方验证  */
#define  NC_RELOAD_NETWORK     0
#define  NC_RELOAD_LIMIT       1
#define  NC_RELOAD_MAILMONFILTER  2
#define  NC_RELOAD_DEBUG          4

typedef struct ncReloadControl_s {
    int   iPid[16];              /* 进程号   */
    unsigned long lStime[16];    /* 启动时间 */
    unsigned long lTimeOut[16];  /* 超时时间 */
    unsigned char  caControl[32];         /* 控制信息 */
} ncReloadControl;

#define  NC_ONTIME_IPCHECK        0   /* 定时检查IP缓存  */
#define  NC_ONTIME_PROCESSCHECK   1   /* 定时检查进程    */
#define  NC_ONTIME_COUNT          2   /* 数据统计        */
#define  NC_ONTIME_DELDETAILLOG   3   /* 清除详细信息    */
#define  NC_ONTIME_DELTEMPFILE    4   /* 删除临时文件    */
#define  NC_ONTIME_DATABACK       5   /* 数据备份        */
#define  NC_ONTIME_CHECKIPTAB     6   /* 检查IPtables    */
#define  NC_ONTIME_RELOADCNF      7   /* 检查配置重栽    */
#define  NC_ONTIME_TIMECONTROL    8   /* 检查时间控制    */
#define  NC_ONLINE_RESTARTNC      9   /* 重新启动NC      */
typedef struct ncOnTimeControl_s {
    unsigned long lStime[16];    /* 最后执行时间 */
    unsigned long lTime[16];     /* 时间间隔     */
} ncOnTimeControl;



/* 邮件过滤规则  */
typedef struct ncMailCtl_s {
    unsigned long lId;
    char caFrom[64];
    char caTo[64];
    char caSub[64];
    char caCont[64];
    char caAttname[64];
    unsigned long lMsize;
    unsigned long lAsize;
    unsigned char cFlags;     /* 限制标识  1--个人 2--组  3-- 全部 */
    unsigned char cActflags;  /* 处理方式 0--备份 1--不备份  */
    unsigned char control;    /* 邮件控制 0--正常 1--拒绝  2--发到指定地址  3--等待确认 */
    unsigned char cTzflags;   /* 通知方式 0--不通知  1--通知发件人  2--收件人 3--指定邮箱  */
    char caSmail[64];         /* 转发邮箱        */
    char caTzmail[64];        /* 通知帐号        */
    char caTzcont[256];       /* 通知内容        */
} ncMailCtl;

/* Add 2002/11/30 */
typedef struct ncFieldList_s {
    char   caFname[32];   
    char   cType;          /* 类型 n-数字 s-字符 */
    char   cFlags;
    short  nLen; 
} ncFieldList;


/* nc000.c */
                    
int ncIsInternalIp(utShmHead *psShmHead,unsigned long lSip);
int ncIsLimitServers(utShmHead *psShmHead,
                     ncUserInfo     *psUserInfo,
                     unsigned long  lUid,
                     unsigned long  lDip,
                     unsigned short nService,
                     unsigned short nDport,
                     unsigned int   iProtocol,
                     unsigned long  *lStop
                    );
int ncCompIpIndex(unsigned long l1,unsigned long l2,unsigned short n2);
int ncGetHttpInfo(char *pIn,int iLen,char *pHost,char *pUrl,
       char *pAgent);
int ncStrGetVar(char *pIn,
                int inLen,
                char *pVar,
                char *pValue,
                int outLen);
int ncStrGetUrl(char *pIn,int iLen,char *caUrl,int outlen);
ncUserInfo *ncGetUserInfo1(utShmHead *psShmHead,unsigned long lIp,unsigned char *pMac);
ncUserInfo *ncGetUserInfo(utShmHead *psShmHead,unsigned long lIp,unsigned char *pMac);
int ncCompHttpNum(utShmHead *psShmHead);
int ncCheckIp(utShmHead *pHead);
int ncSendCheckTables(char *p);
int ncTablesControl(utShmHead *psShmHead,
             unsigned long  lSip,
             unsigned long  lDip,
             unsigned short nDport,
             char           cProtocol,
             unsigned long lStop);
/* nc001.c */

ncMailFilter *ncMilInitFilter(utShmHead *psShmHead,ncMailFilter *ps,unsigned 
                 long *lChangeTime) ;
ncUploadFilter *ncUploadInitFilter(utShmHead *psShmHead,ncUploadFilter *ps0,
        unsigned long *lChangeTime);
ncUserInfo *ncCheckNetworkIp(utShmHead *psShmHead,ncIpHead *psIpHead,int *iFlags0);
int ncGetService(utShmHead *psShmHead,unsigned char cProtocol,
               unsigned short nPort);


/* nc002.c */
int ncInitIpPkg(utShmHead *psShmHead);
int ncInitInternal(utShmHead *psShmHead);
int ncInitLimit(utShmHead *psShmHead);
int ncInitMonitor(utShmHead *psShmHead);
int ncInitUserInfo(utShmHead *psShmHead);
ncUserInfo *ncGetEmptyUserInfo(utShmHead *psShmHead,ncUserInfo *psUserInfo,
                        unsigned long lIp);
ncUserInfo *ncGetEmptyUserInfo1(ncUserInfo *psUserInfo,
                        char *caMac);                        
ncUserInfo *ncGetUserInfo3(utShmHead *psShmHead,
                     long lUserid, unsigned char *pMac);                        
int ncGetMonNum(utShmHead *psShmHead, unsigned long lId);
int ncGetLimitNum(utShmHead *psShmHead,unsigned long lId);   
/* nc003.c */
int ncWebAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncHttpWriteDb(ncUserInfo *psUserInfo,ncTcpBuffer *psTcpBuffer,ncHttpBuffer *psHttpBuffer);
int ncWebDispUserAccess(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead); 
int ncWebShowUserStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebReplace(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebQyeIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebModiPass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebFlowCount(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowNewvister(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
char *ncCvtL2TimeStr(unsigned long lTime);
int ncWebSetupInternet_save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupInternet_list(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupInternet_form(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupWeb_list(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupWeb_save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupWeb_form(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowNewweb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShutdown(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispModiPass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc004.c */

int ncWebDelSomeUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserAddForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebListUserTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);                                           
int ncWebUserAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispDeptForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAssignUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminUserList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminDispUserForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminRoleList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncAdminRoleModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminRole(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncAdminUserSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebListGroupTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserModify(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserModiForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserInfo_c(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserSave_c(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc005.c */
int ncWebDispNetwork(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetNetwork(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc006.c */
int ncDelCountLog(char *psShmHead0);
int ncDelDetailLog(char *psShmHead0);
int ncCountLog(utShmHead *psShmHead0);
int ncDelTempFile(char *psShmHead0);

/* nc007.c  */
char *ncCvtConntime(unsigned long lConntime);
int ncWebCountIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
char *ncCvtSetService(utShmHead *psShmHead,int lService);
char *ncCvtMac(unsigned char *caMac);
int ncWebCountTotalFlow(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOutFlowChart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountIpChart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc008.c */
int ncWebQyehttp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserBindAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispBindUserForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispMyaddress(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserAddMyaddress(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCount(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBandWidthSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispBandWidthForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUserRandWidth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebInternetLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebInternetLogout(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebInternetModpass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserBindWidthSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebIpHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebIpDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebHttpHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebHttpDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebRefreshUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserBandModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserBandForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


/* nc009.c */
int ncWebSortUserByFlow(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortWeb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailReadUploadHeads(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncmil001.c */
/* int ncMailReadFoldHeads(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncUploadReadContent(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailUploadRemove(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
*/
/* ncmil002.c */
int ncMailShowRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailDispOneRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailModiOneRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailDelSomeRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlOutHtm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlAuditlst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlAudit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailCtlCheck(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailAudit1(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlHlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlHistorySearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDispHSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlHistoryDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlRead2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncmail003.c */
int ncMailMonListRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailMonDispOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailMonModiOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailMonDelSome(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncmil005.c */
int ncWebMailMonList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonList_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonHsear(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonHsear_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDel_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFw(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFw_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDispSform_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFwOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebUploadList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUploadRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUploadDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebMailExportDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


/* ncmail006.c */
int ncWebPopSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispPopForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPopAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDelSomePop(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDispBakSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlSaveBakSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailByBm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailByU(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCountChart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailByStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDispImport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailSaveImport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDoImport2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDoImport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebMailDispExport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDoExport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailShowExpStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


int ncWebMailCtlSaveDefSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDispDefSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc016.c */
int ncWebTimeCtlReSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc011.c */

ncPostDataContent *ncReadPostData(char *caFile, char *pOutFile);
/* nc013.c */
int ncBackup(utShmHead *psShmHead);
/* nc014.c */
int ncWebPrintWebUse(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUseListGroupTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebuseDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncutl001.c */
char *ncMailCtl_GetFile();

/* nc016 */
int ncWebTimeCtlSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTimeControl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackupBaseInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowBackup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackFileDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDelBackFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
#endif
