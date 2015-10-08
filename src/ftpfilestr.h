
/* nMark 的含义  */
#define NC_FTP_PORTINFO   141
#define NC_FTP_PASVINFO   142

/* cFun 的含义     */
#define NC_FTP_PORTMARK   143   /* 数据通道IP连接  */
#define NC_FTP_PASVMARK   144   /* 数据通道IP连接  */
#define NC_FTP_COMMMARK   145   /* 命令通道TCP连接  */

#define NC_FTP_COMM       146     /* 命令      */ 
#define NC_FTP_DATA       147     /* 一般数据  */
#define NC_FTP_PORTDOWN   148     /* 下载文件  */
#define NC_FTP_PORTUP     149     /* 上传文件  */
#define NC_FTP_PASVDOWN   150     /* 下载文件  */
#define NC_FTP_PASVUP     151     /* 上传文件  */


/* 连线信息  */
typedef struct ncPkgSeqInfo_s {
	unsigned long  lSid;         /* Session Id     */
	unsigned long  lSeq;         /* 初始序列号     */
	unsigned short nCount;       /* Dport          */
	unsigned char  nMark;        /* 标识 0--无数据  1--有数据        */
	unsigned char  cFun;         /* 子功能,常见上面定义的宏          */ 
	unsigned long  lLastTime;    /* 最后访问时间    */
	unsigned long  lLen;         /* 用户ID    */ 
	unsigned long  lSid1;        /* Sip       */
	unsigned long  lSid2;        /* Dip       */
	unsigned char  caFrom[48];   /* ftp登录的用户名                  */
	unsigned char  caTo[48];     /*   命令通道: Ip地址和端口 192.168.0.1:5001
	                                  数据通道：传送的文件名                     */
} ncPkgSeqInfo;


/* 内容数据交换缓冲区  */
psPkgSen

typedef struct ncPkgSession_s {  
    unsigned long    lSid;       /* 同 tcp中sid, 会话的标识  */
	unsigned long    lSeq;       /* 数据包的序列号   */
	unsigned long    lSip;
	unsigned long    lDip;
	unsigned short   nSport;
	unsigned short   nDport;
	unsigned short   nLen;       /* 数据包的长度    */
	unsigned char    cMark;
	unsigned char    cDict;      /* 数据包方向 0--请求  1--应答  */
	unsigned char    pBuf[1600];    /* 数据包的内容    */
} ncPkgSession; 



psLog 
typedef struct ncPkgLog_s { /* Log的结构  */
    unsigned char  cFlags;       /* 0--无  1--有  9--结束      */
    unsigned char  cType;        /* 1--异常访问   2--会话内容  */
    unsigned short nLen;         /* 数据包的长度               */
	unsigned long  nService;     /* 服务类型                   */
	unsigned long  nFun;         /* FunCode                    */
    unsigned long  lPos;         /* 位置，和实际的文字保持一致 */
    unsigned long  lTime;        /* 时间     */
    unsigned long  lUserid;      /* 用户ID   */
} ncPkgLog;

/* 此处的源地址为客户端地址  */
typedef struct ncIp_s {
    unsigned long  lSip;             /* 内部IP地址                    */
    unsigned long  lDip;             /* 外部IP地址                    */
    unsigned short nDport;           /* 目标端口号                    */
    unsigned short nSport;           /* 源端口, 缺省情况下为 0, 只有当代理的时候才为非 0  */
    unsigned char  cType;            /* 协议    6--TCP 17 UDP         */
    unsigned char  cMark;            /* 标识, 143 -- p 数据通道的IP连接指针 (命令)
                                              144 -- p 命令通道对应的 TCP指针 (数据)
                                      */    
    unsigned char  cFileMark;        /* cFun  当服务为FTP时，内容见 cFun宏       */
    unsigned char  cRev;             /* 保留  */
    unsigned long  lStartTime;       /* 起始时间                     */
    unsigned long  lLastTime;        /* 最后修改时间                 */
    unsigned long  lStop;            /* 访问控制检查时间 hhmmss， 若该值为0表示使用超时时间  */
    long8    lBytes[2];              /* 0-上行字节数  1-下行字节数   */
    unsigned char  cStatus;          /* 0--新   1--正常  2--禁用     */
    unsigned char  cUseFlags;        /* 使用标识 0--没内容 1--有内容 2--已处理 3--已保存   */
    unsigned char  caMac[6];         /* 源Mac地址                    */
    ncUserInfo     *psUser;          /* 用户信息，若为空表示用户信息不定    */
    unsigned long  lService;         /* 服务                                */
    unsigned long  lUrl;             /* 对应数据Sid                         */
    unsigned long  lPreService;      /* 预分析协议                          */
    unsigned char  cProService[8];   /* 协议标识,用于协议的上下文分析       */
    char    *p;                      /* 保留指针, 对应命令TCP连接指针       */
} ncIp;

typedef struct ncTcp_s {
    unsigned long  lSip;              /* 内部IP地址                   */
    unsigned long  lDip;              /* 外部IP地址                   */
    unsigned short nDport;            /* 目标端口号                   */
    unsigned short nSport;            /* 源端口号                     */
    unsigned long  lStartTime;        /* 起始时间                     */
    unsigned long  lLastTime;         /* 最后修改时间                 */
    unsigned char  cConnect;          /* 连接状态  1 Syn  2 建立  9 Close   */
    unsigned char  cDiction;          /* 方向 0 正向   1 反向               */
    unsigned char  cStatus;           /*      0 未知   1 正常      2 禁用                   */
    unsigned char  cProt;             /* 协议   6-TCP  17 UDP                               */
    unsigned char  cFlags;            /*        143 -- p 数据通道的IP连接指针 (命令)        */
    unsigned char  cProxy;            /* 0 -- 正常   1 -- 基于Proxy的协议   */
    unsigned char  cRev[2];
    ncIp           *psIp;             /* 对应的IP信息,NULL 表示该连接不可用 */
    unsigned long  lStop;             /* 规则作用的终止时间                 */
    unsigned long  lSeq;              /* 上行序列号                          */
    unsigned long  lAck_seq;          /* 下行序列号                          */
    unsigned long  lSid;              /* Session ID                          */
    unsigned long  lUrl;              /* Url序号                             */
    char *p;                          /* 保留指针                            */
} ncTcp;
