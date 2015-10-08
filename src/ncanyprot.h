/* 协议分析  */
#ifndef __NCANYPROTDEF___
#define __NCANYPROTDEF___

#define NC_ANY_TAG          1
#define NC_ANY_TAGEND       2
#define NC_ANY_VARVALUE     3
#define NC_ANY_REMARK       4
#define NC_ANY_ERROR        5

#define NC_ANY_SEND         0
#define NC_ANY_RECV         1
#define NC_ANY_ALL          9


#define NC_ANY_MATCH_START      0           // 从起始位置开始匹配
#define NC_ANY_MATCH_END        1           // 从数据包结束位置开始匹配
#define NC_ANY_MATCH_CUR        2           // 从上一次匹配开始
#define NC_ANY_MATCH_ANY        3           // 任意位置匹配
#define NC_ANY_MATCH_COND       4           // 逻辑表达式
#define NC_ANY_MATCH_REGULAR    5           // 正则表达式(源，未编译过)
#define NC_ANY_MATCH_REGULAROK  6           // 已编译
#define NC_ANY_MATCH_ANYSTART   7           // 从开始位置的偏移量，任意位置匹配
#define NC_ANY_MATCH_ANYEND     8           // 从结束位置的偏移量，任意位置匹配
#define NC_ANY_MATCH_ERROR      9           // 错误

#define NC_PAT_MAXPORTVALUE      65535  // 端口的最大值
#define NC_PAT_MINPORTVALUE      1      // 端口的最大值

#define NC_PAT_MAXPKG            1500   // 数据包的最大长度
#define NC_PAT_MINPKG            1      // 数据包的最小长度

#define NC_PATOP_AND             0      // And
#define NC_PATOP_OR              1      // Or

#define NC_PAT_PAGESIZE          4096   // 每次转换记录数

#define NC_DOSERVICE_GETPORTSERVICE    10000
#define NC_DOSERVICE_GETIPSERVICE      20000
#define NC_DOSERVICE_GETPATSERVICE     30000

#define NC_PAT_MAXBIT 20000
#define NC_PAT_SUMBIT 2500

#define NC_PAT_KEY              "Pro$Netway20090501"
#define NC_PAT_MAXHOSTLEN   48


typedef struct ncPatHostServiceKey_s {
        char        caName[NC_PAT_MAXHOSTLEN];
        unsigned    short nCount;
} ncPatHostServiceKey;

typedef struct ncAnyNameValue_s {
    unsigned char name[16];
    unsigned char value[128];      /* 服务名称  */
    int      iLine;                // 所在文件的行号
    struct ncAnyNameValue_s *next;
} ncAnyNameValue;

typedef struct ncAnyTag_s {
    unsigned char   tagname[16];
    ncAnyNameValue *psNameValue;
    int      iLine;                 // 行号
    struct ncAnyTag_s *son;
    struct ncAnyTag_s *next;
} ncAnyTag;


typedef struct ncAnyTagHead_s {
    ncAnyTag *psTag;
}ncAnyTagHead;


#define NC_PAT_FILEID               20080420L    // 文件标识
#define NC_PAT_FILEVERSION          70503L       // 引擎版本号

#define NC_PAT_ID_PATNAME           1110
#define NC_PAT_ID_IPSERVICE         1111
#define NC_PAT_ID_PORTSERVICE       1112
#define NC_PAT_ID_SERVICEDEF        1113
#define NC_PAT_ID_FUNDEF            1114
#define NC_PAT_ID_PATSERVICE        1115
#define NC_PAT_ID_PATRECORD         1116

#define NC_PAT_ID_HOSTSERVICE       1116
#define NC_PAT_ID_END               1999

#define NC_PAT_WORDNAME_USERNAME      1
#define NC_PAT_WORDNAME_DISPNAME      2
#define NC_PAT_WORDNAME_COMPNAME      3
#define NC_PAT_WORDNAME_POSITION      4
#define NC_PAT_WORDNAME_GROUP         7

#define NC_PAT_CODE_GB                0
#define NC_PAT_CODE_BIG5              1
#define NC_PAT_CODE_UTF8              2
#define NC_PAT_CODE_JP                3
#define NC_PAT_CODE_IPADDRESS         10
#define NC_PAT_CODE_MAC6              11
#define NC_PAT_CODE_MAC12             12
#define NC_PAT_CODE_MAC17             13


/* 文件头 48个字节  */
typedef struct ncPatFileHead_s {
    unsigned long   lId;                // 文件ID
    unsigned long   lVersion;           // 引擎版本号      7.00.01
    unsigned long   lCreateTime;        // 文件创建时间
    unsigned long   lLastUpdate;        // 最后更新时间
    unsigned long   lLastTime;          // 最后更新时间
    unsigned long   lEncrypt;           // 是否加密
    unsigned long   lMd5;               // Md5校验码
    unsigned long   lBytes;             // 文件字节数
    unsigned long   lSumDoName;         // 模板总数
    unsigned long   lSumService;        // 服务总数
    unsigned long   lSumFunction;       // 应用分类总数
    unsigned long   lSumIpService;      // IP服务总数
    unsigned long   lSumPortService;    // 端口总数
    unsigned long   lSumHostService;    // 端口总数
    unsigned long   lSumPatService;     // 模式总数
    unsigned long   lDataVersion;       // 协议库的版本号 9.05.21
    unsigned char   cRev[32];           // 备用
} ncPatFileHead;


/* 文件记录         */
typedef struct ncPatFileRecord_s {
    unsigned long  lRecordId;          // 记录标识
    unsigned long  lSumRecord;         // 记录总数
    unsigned long  lSrcBytes;          // 原来的字节数(未加密)
    unsigned long  lNowBytes;          // 现在字节数(加密后)
} ncPatFileRecord;



/* 下面的结构用于存放共享内存中的内容  */
typedef struct ncPatWord_s {
    unsigned short  nOffSet;             // 起始位置
    unsigned short  nEndPos;             // 终止位置
    unsigned char   cLen;                // 长度
    unsigned char   cMatch;              // 匹配方式
    unsigned char   cName:4;             // 1--取名字  2--取密码  3--显示名   4--位置信息
    unsigned char   cEncode:4;           //   编码方式
    unsigned char   cNameLen;            //   名字长度, 若为0,caCont为后缀字符值
    unsigned char   caCont[64];          //   字符内容
} ncPatWord;


/* 每条记录信息   
     一条记录为一个完整的特征
 */
typedef struct ncPatRecord_s {
    unsigned short  nService;    // 服务代码
    unsigned char   cFun;        // 动作代码
    unsigned char   cSum;        // 匹配总数(Word总数)
    unsigned short  nMinLen;     // 数据包最小长度
    unsigned short  nMaxLen;     // 数据包最大长度
    unsigned char   cProt;       // 协议 0-TCP  1-UDP
    unsigned char   cDirect:4;   // 方向 0-发送 1-接收
    unsigned char   cLevel:4;    // 规则级别
    unsigned short  nNext;       // 下一步行动 0--正常 1--调用别的处理进程 
    unsigned short  nCount;      // 最小频度,即数据包的数量
    unsigned char   cPortLen;    // 端口列表的字节数
    unsigned char   cRule;       // 规则描述的字节数 
    unsigned short  sid;         // 规则序列号
    unsigned char   prio;        // 规则优先级 从小到大, 缺省为32
    unsigned char   status:4;    // 规则状态  0-未确认  1-检查过
    unsigned char   rev:4;         // 保留
    unsigned char  *pPortList;   // 端口列表
    unsigned char  *pRulename;   // 规则描述
    ncPatWord      *psWord;
}  ncPatRecord;


typedef struct ncPatSearRec_s {
    ncPatRecord *psRecord;
    int    iDeep;                  // 深度
    struct ncPatSearRec_s *next;   // 下一级
    struct ncPatSearRec_s *left;   // 同一级的下一条记录
} ncPatSearRec;



/* 端口信息   */
typedef struct ncPatPortIndex_s {
    ncPatRecord     **psRecord;       // 保存的是 psRecord的指针
    ncPatSearRec    *psSear;
    unsigned short  nLport;           // 起始端口
    unsigned short  nUport;           // 终止端口
    unsigned short  nSum;             // 总数 
    unsigned short  nMax;             // 最大记录数
} ncPatPortIndex;


/* 端口信息   */
typedef struct ncPatRecIndex_s {
    ncPatPortIndex  *psIndex;
    unsigned short  nSum;             // 总数 
    unsigned short  nMax;             // 最大记录数
} ncPatRecIndex;


/* 特征库   */
typedef struct ncPatService_s {
    unsigned char        caName[16];      /* 模式名称                               */
    int                  iId;             /* ID         */
    unsigned long        nSum;            /* psRecord 记录总数                      */
    unsigned long        nMax;            /* 允许的最大记录数                       */
    ncPatRecIndex        sPort[2][2];     /* 0: 0-TCP 1-UDP    1: 0-Send 1-Recv     */
    ncPatRecord          **psRecord;        /* 记录指针                             */
} ncPatService;


#define NCDNS_MAX_DNSSERVICE     1024
#define NCDNS_MAX_IPSERVICE      8192

#define NCDNS_FROM_DYNAMIC       1          // 系统动态获得
#define NCDNS_FROM_CONFIG        2          // 配置文件
#define NCDNS_FROM_DNS           8          // DNS


/* 某些服务和域名对照表                        */
typedef struct ncPatHostService_s {
    char           caName[NC_PAT_MAXHOSTLEN];  /* 域名        */
    unsigned short nCount;      // 序号,从0开始
    unsigned short nSum;        // 总的记录数
    unsigned short nService;     /* 服务代码    */
    unsigned char  cFun;         /* 功能代码    */
    unsigned char  cProt;       // 协议: 0--全部  6--TCP    7--UDP
    unsigned short nSport;      // 起始端口
    unsigned short nDport;      // 终止端口
    unsigned short nId;         // 特征库ID
    unsigned short nNextId;
    unsigned char  cFrom;       // 来源
    unsigned char  cRev[3];
} ncPatHostService;

/* IP 地址和服务对照表
      关键字为前7个字节

 */
typedef struct ncPatIpService_s {
    unsigned       long lIp;
    unsigned short nId;         // 地址标识
    unsigned char  cCount;      // 序号,从0开始
    unsigned char  cSum;        // 总的记录数
    unsigned short nSport;      // 起始端口
    unsigned short nDport;      // 终止端口
    unsigned short nService;    // 服务代码
    unsigned char  cFun;        // 动作
    unsigned char  cProt;       // 协议: 0--全部  6-TCP  7-UDP
    unsigned char  cFrom;       // 记录来源
    unsigned short nNext;       // 后续处理
    unsigned char  cRev;        // 保留
    unsigned long  lLasttime;   // 最后访问时间
    unsigned long  sid;         // 记录标识
} ncPatIpService;


/* 返回的服务  */
typedef struct ncRetService_s {
    unsigned short nService;          // 服务
    unsigned char  cFun;              // 功能动作
    unsigned char  cCont:2;     	  // 内容信息 0-无内容  1-有内容
    unsigned char  cPortService:1;    // 
    unsigned char  cIpService:1;      // 
    unsigned char  cPatService:1;     // 
    unsigned char  cFunService:1;     // 
    unsigned char  cContinue:1;       // 0--结束 1--继续 
    unsigned char  cRev:1;            // 
    unsigned short cCount;      // 嵌套次数
    unsigned short nId;         // DebugID
} ncRetService;

#define NC_PATDO_UNKNOW         0    // 未知
#define NC_PATDO_PORTSERVICE    1    // 端口服务清单
#define NC_PATDO_IPSERVICE      2    // IP服务
#define NC_PATDO_PATSERVICE     3    // 模式匹配
#define NC_PATDO_PROGRAM        4    // 程序实现
#define NC_PATDO_HOSTSERVICE    5    // 主机对照表

// 模式名称对照表
typedef struct ncPatNameIndex_s {
    unsigned short nId;              // 序号, 从0开始
    unsigned char  cType;            // 类别
    unsigned char  cRev;
    int      (*fFunName)(ncTcp *psTcp,unsigned char *pkg,int iLen,ncRetService *psRet);     /* 函数名     */
    unsigned char caName[16];        // 名称
    unsigned char caMemo[128];        // 说明
} ncPatNameIndex;




/* 端口服务对照表     */
typedef struct ncPortService_s {
    unsigned short nId;         // 端口标识
    unsigned short nPort;       // 端口
    unsigned char  cProt;       // 6-Tcp  17-Udp
    unsigned char  cFun;        // 功能动作
    unsigned short nService;    // 服务
    unsigned short nNext;       // 下一步动作  
    unsigned char  cType;       // 处理类别 1-PortService 2-IpService 3-PatSevice 4-Function
    unsigned char  cRev;
    unsigned long  sid;         // 记录标识
} ncPortService;


/* 服务编码   */
typedef struct ncServiceName_s {
    unsigned short   nService;        /* 服务类型  */
    unsigned short   nGacode;         /* 公安编码  */
    unsigned char    caName[24];      /* 服务名称  */
    unsigned char    caMemo[96];      /* 说明      */
    unsigned char    cRev;
    unsigned char    cNext;           /* 是否还有进一步的服务 0--无  1--有  */
    unsigned char    cMark;           /* 标识  0-未操作过  1-操作过的       */
    unsigned char    cFlags;          /* 类别标识   */
} ncServiceName;

/* 功能编码   */
typedef struct ncFunName_s {
    unsigned short   nFun;            /* 功能编码  */
    unsigned short   nRev;            /* 保留      */
    unsigned char    caName[16];      /* 服务名称  */
} ncFunName;


typedef struct ncPatLanIp_s {
    unsigned long lSip;
    unsigned long lEip;
    int      iFlags;
} ncPatLanIp;

/* 记录长度类别的条件   */
typedef struct ncCondLen_s {
    short    k;                   // 系数
    short    pos:10;              // 位置
    short    type:6;              // 类别 0-常数 1-开始  2-结束 3-当前位置
} ncCondLen;


/* 免监控服务代码    
   第一个位置存放记录总数
   第二个位置存放最大空间总数
 */
#define  NC_PAT_SERVICEBASE  10000000L
typedef struct ncNomonService_s {
    unsigned long   lService;             /* 服务:   10000000 + Service  */
} ncNomonService;



typedef struct ncPatPkgAccName_s {
    char       name[63];
    uint4      iType;
} ncPatPkgAccName;

typedef struct  ncPatWordHead_s {
    uint4       iSumWord;
    uint4       iService;
    ncPatWord   *psWord;
}  ncPatWordHead;

typedef struct ncPatAccountIp_s {
    uint4               lIp;
    uint4               nPort;
    uint4               nSum;
    uint4               nMax;
    ncPatWordHead       *psWordHead;
} ncPatAccountIp;


typedef struct  ncPatAccountHead_s {
    uint4               nSumIp;
    uint4               nMaxIp;
    ncPatAccountIp      *psIpAccount;
    uint4               iSumWord;
    uint4               iMaxWord;
    ncPatWordHead       *psWordHead;
} ncPatAccountHead;


/* 网络服务描述    */
typedef struct ncPatHead_s {
    unsigned    long    lCreateTime;              // 创建时间
    unsigned    long    lLastUpdate;              // 最后更新时间
    unsigned    long    lLastTime;                // 最后升级时间
    unsigned    long    lVersion;                 // 引擎版本
    ncNomonService      *psNomonService;          // 免监控服务代码
    ncServiceName       *psServiceName;           // 服务编码
    int                 iSumService;              // 当前服务总数
    int                 iMaxService;              // 当前最大的服务数
    ncFunName           *psFunction;              // 当前功能编码
    int                 iSumFunction;             // 当前服务总数
    int                 iMaxFunction;
    unsigned  char      *pPortService;           // 端口服务对照表
    unsigned  long      lMaxPortService;         // 最大端口数
    unsigned  long      lPortBufSize;            // Hash表占用的大小
    unsigned  char      *pIpService;             // IP服务对照表
    unsigned long       lMaxIpService;           // 最大IP数量
    unsigned long       lIpBufSize;              // Hash表占用大小
    unsigned long       lMaxHostService;         // 主机信息
    unsigned long       lHostBufSize;
    unsigned char       *pHostService;
    ncPatService        *psPatService;
    int                 iSumPatService;
    int                 iMaxPatService;
    ncPatNameIndex      *psDoName;               // 处理函数名称
    int                 iSumDoName;              // 当前处理模式总数
    int                 iMaxDoName;              // 已分配空间的模式总数
    int                 iSortMark;               // 排序标识 0-未排序 1-已排序
    int                 iSysAtShm;               // 是否使用共享内存
    ncPatLanIp          *psLanIp;                 // 内网Ip地址,用于调试
    int                 iSumLanIp;
    int                 iMaxLanIp;
    unsigned char       cServiceSort;
    unsigned char       cFunctionSort;
    unsigned char       cCheckMark;
    unsigned char       cReguMark;
    unsigned char       cDes;                     // 是否从加密文件读出来
	unsigned char       cP2PIntelligent;             // 是否智能判断服务, 针对7/8类
    unsigned char       cRev[2];
    unsigned long       lP2PMinPkg;                  // 最小数据包数量
    unsigned long       lP2PMinTcp;                  // 最小连接数
    unsigned long       lP2PMinFlow;                 // 最小流量
    unsigned long       lP2PHttpFlow;                // 最小流量
    unsigned long       lP2PHttpsFlow;               // 最小流量
	unsigned long   	lDataVersion;       		// 协议库的版本号 9.05.21
    unsigned char       *pContService;              // 服务是否继续开关 2048个字节
} ncPatHead;


#endif
