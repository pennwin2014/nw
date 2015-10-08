/* 最后修改日期  2010/06/26

*/
#ifndef __NCLIMITDEF__
#define __NCLIMITDEF__
#define NC_LIMIT_INTERNET  1              /* Internet网 */
#define NC_LIMIT_SERVICE   2              /* 服务       */
#define NC_LIMIT_IPADDRESS 3              /* 地址范围   */
#define NC_LIMIT_IPLIST    4              /* Ip列表     */
#define NC_LIMIT_WEBLIST   5              /* 网址库策略    */
#define NC_LIMIT_WEBHOST   6              /* Web地址    */
#define NC_LIMIT_VID       7              /* 虚拟身份   */
#define NC_LIMIT_WEBCLASS  8              /* 网址类别   */
#define NC_LIMIT_FUNLIST   9              /* 功能列表   */

/* 访问控制参数   
               100以上为流控ID
*/
#define NC_LIMIT_REJECT         0         /* 禁止       */
#define NC_LIMIT_ACCEPT         1         /* 允许       */
#define NC_LIMIT_REJECTWARN     2         /* 禁止告警   */
#define NC_LIMIT_ACCEPTWARN     3         /* 允许告警   */
#define NC_LIMIT_NOMON          4         /* 免监控     */
#define NC_LIMIT_SETREDIR       5         /* 端口重定向        */
#define NC_LIMIT_DELREDIR       6         /* 取消端口重定向    */
#define NC_LIMIT_NOEXIST        9 
#define NC_LIMIT_ERROR          -1

#define NC_LIMIT_DELQOS         9         /* 删除QOS中的规则  */
#define NC_LIMIT_REJECTUSER     20        /* 整个用户被禁     */
#define NC_LIMIT_WIDTHBAND1     31        /* 带宽管理1        */
#define NC_LIMIT_WIDTHBAND2     32        /* 带宽管理2        */
#define NC_LIMIT_WIDTHBAND3     33        /* 带宽管理2        */




#define NC_CTLSTA_ACCEPT        0
#define NC_CTLSTA_DROPWARN      1 
#define NC_CTLSTA_ACCEPTWARN    2
#define NC_CTLSTA_DROP          3


#define NC_LIMIT_BYUSER    1
#define NC_LIMIT_BYGROUP   2
#define NC_LIMIT_BYALL     3
#define NC_LIMIT_BYPOLCY   4
#define NC_LIMIT_BYCUSER   6
#define NC_LIMIT_BYCGROUP  7
#define NC_LIMIT_BYCALL    8

#define NC_LIMDATE_DAY    1      /* 独立日  */
#define NC_LIMDATE_WEEK   2      /* 每周    */
#define NC_LIMDATE_MONTH  3      /* 每月    */
#define NC_LIMDATE_YEAR   4      /* 每年    */

#define NC_DATE_WORKID    1      /* 工作日      */
#define NC_DATE_NOWORK    2      /* 非工作日    */
/* 时间  */
typedef struct ncLimDate_s {
    unsigned long lType;                    // 时间类别   1--Day     2--Week 3--Month 4--Year    
    unsigned long lLower;                   // 时间下限  1-YYYYMMDD 2--0/6  3--1/31  4--MMDD
    unsigned long lUpper;                   // 时间上限
} ncLimDate;

typedef struct ncLimDateIndex_s {
    int       lStart;                  // 起始位置
    int       lSum;                    // 记录总数
    long      lId;
    char      name[32];                // 类别名称
} ncLimDateIndex;

typedef struct ncLimDateHead_s {
    ncLimDate       *psDate;
    ncLimDateIndex  *psIndex;
    int             lSumIndex;
    int             lSumDate;
} ncLimDateHead;





/* 虚拟身份的限制  */
typedef struct ncLimVid_s {       
    unsigned long   lId;               /* 阻断ID                      */
    unsigned long   lService;          /* 服务代码, 0 全部            */
    unsigned long   lVidList;          /* 虚拟身份列表ID              */
    unsigned long   lVidNum;           /* 针对Uid的序号,From 0,在装载的时候自动生成   */
    unsigned char   vid[64];           /* 标识                        */
    unsigned long   lStart;            /* 起始时间 HHMMSS                             */
    unsigned long   lEnd;              /* 终止时间 HHMMSS                             */
    unsigned long   lCid;              /* 客户端策略ID                              */
    unsigned long   lSid;              /* 服务器端策略ID                              */
    unsigned long   lValidTime;        /* 有效期                                      */
    unsigned long   lWarntype;
    unsigned char   cTimeType;         /* 时间类型                                    */
    unsigned char   cFun;              /* 动作类型 0--全部  1--上线  2--下线  3--发送文件   */
    unsigned short  cVister;           /* 0 禁止   1 允许  2 阻断告警  3 允许告警     */
} ncLimVid;

/*
   虚拟身份集
 */
typedef struct ncLimVidList_s {
    unsigned long   lSid;              /* 对应在数据库中的Sid         */
    unsigned char   uid[64];           /* 标识                        */
} ncLimVidList;

/* 索引,按lID排序
*/
typedef struct ncLimVidIndex_s {
    unsigned long   lId;               /* 类别Id                      */
    unsigned long   lStart;            /* 起始位置                    */
    unsigned long   lSum;              /* 全部记录数                  */
    unsigned char   name[32];          /* 类别名称                    */
}ncLimVidIndex;


/* 虚拟身份集的头信息   */
typedef struct ncLimVidHead_s {
    ncLimVidList    *psList;           /* 具体内容指针                */
    ncLimVidIndex   *psIndex;          /* 索引信息指针                */
    unsigned long   lSumIndex;         /* 索引总数                    */
    unsigned long   lSumVid;           /* 总虚拟集                    */
}ncLimVidHead;

/* 结构保存在 NC_LNK_VIDSET 中   */
typedef struct ncLimIp_s {             /* 限制访问的Ip地址  */
    unsigned long   lId;               /*                             */
    unsigned long   lIpLower;          /* 下限, If flags==4 策略ID 2|9 --服务    */
    unsigned long   lIpUpper;          /* 上限, If flags==4 策略序号   9-功能ID  */
    unsigned short  nLowerport;        /* 端口号下限                                  */
    unsigned short  nUpperport;        /* 端口号上限                                  */
    unsigned char   cService;          /* 服务类型                                    */
    unsigned char   cFlags;            /* 限制方式 1--外部网  2--服务  3--IP地址和端口  4--服务策略 9-功能 */
    unsigned char   cAccessFlags;      /* 访问限制范围: 1--User 2--Group  3--All 4--Default */
    unsigned char   cTranType;         /* 传送类型  0--全部   17--UDP  6--TCP          */
    unsigned long   lStart;            /* 起始时间 HHMMSS                              */
    unsigned long   lEnd;              /* 终止时间 HHMMSS                              */
    unsigned long   lWarnid;           /* 告警类别  Add 20051017                       */
    unsigned long   lCid;              /* 服务器端策略ID                               */
    unsigned long   lSid;              /* 服务器端策略ID                               */
    unsigned long   lValidTime;        /* 有效期                                       */
    unsigned short  cTimeType;         /* 时间类型                                     */
    unsigned short  nQosId;        
    unsigned long   cVister;           /* 0 禁止   1 允许, > 100  QOSID                */
    unsigned long   lNum;              /* 序号                                         */
}ncLimIp;


typedef struct ncLimWeb_s {       /* 限制访问的网站     */
    unsigned long   lId;               /*                     */
    char            caUrl[64];         /* 网站地址            */
    unsigned long   lUrlid;            /* 网址列表            */
    unsigned long   lLstnum;           /* Weblist序号         */
    unsigned long   lStart;            /* 起始时间 HHMMSS                             */
    unsigned long   lEnd;              /* 终止时间 HHMMSS                             */
    unsigned long   cVister;           /* 0  禁止  1 允许   > 1000 QOSID              */
    unsigned char   cAccessFlags;      /* 访问限制范围: 1--User 2--Group  3--All 4--Default */
    unsigned char   cFlags;            /* 0--网址     5--Web列表, caUrl中显示的是ID   8--网址类别*/
    unsigned short  cTimeType;         /* 时间类型                                    */
    unsigned long   lWarnid;           /* 告警类别                                    */
    unsigned long   lCid;              /* 服务器端策略ID                              */
    unsigned long   lSid;              /* 服务器端策略ID                              */
    unsigned long   lValidTime;        /* 有效期                                      */
}ncLimWeb;

/* 该结构按网址顺序排列    */
typedef struct ncLimUrlList_s {       /* 限制访问的Web地址               */
    char            url[36];          /* 网址内容                        */
    unsigned short  nNum;             /* 序号,从0开始                    */
    unsigned short  nSum;             /* 重复数量,只检查 nNum为 0        */
    unsigned long   lId;              /* 网址库类别ID                    */
    unsigned long   lSid;             /* 网址库的Sid                     */
    unsigned long   lCount;           /* 访问次数                        */
    unsigned long   lLastTime;        /* 最后访问时间，0 表示没有访问过  */
}ncLimUrlList;



/* 该结构按网址顺序排列    */
typedef struct ncLimWebList_s {       /* 限制访问的Web地址  */
    char            url[38];          /* 网址内容           */
    unsigned short  iLen;             /* 长度               */
    unsigned long   lId;              /* 类别ID             */
    unsigned long   lSid;             /* 网址库的Sid        */
    unsigned long   lCount;           /* 访问次数           */
}ncLimWebList;

typedef struct ncLimWebPol_s {       /* 访问策略           */
    unsigned long  lId;
    int            iFlags;
    unsigned long  lWarnid;
    char     name[32];
}ncLimWebPol;


    
typedef struct ncLimIndex_s {     
    unsigned long   lId;             /* 索引信息          */
    unsigned long   lStart;          /* 起始序号          */
    unsigned long   lSum;            /* 终止序号          */
} ncLimIndex;

typedef struct ncLimPolIndex_s {     
    unsigned long   lId;             /* 索引信息          */
    unsigned long   lStart;          /* 起始序号          */
    unsigned long   lSum;            /* 终止序号          */
    int             iFlags;          /* 缺省策略          */
    unsigned long   lWarnid;
    char            name[32];
} ncLimPolIndex;

typedef struct ncLimWebListHead_s {       /* 限制访问的Web地址库  */
//    unsigned char    *pWebHash;
//    ncLimWebList     *psWeb;
    ncLimWebPol      *psPol;
    ncLimPolIndex    *psPolIndex;
//    unsigned long    lSumWeb;
    unsigned long    lSumPol;
    unsigned long    lSumPolIndex;
} ncLimWebListHead;


typedef struct ncLimIpListHead_s {       /* 限制访问的Ip地址  */
    ncLimWebPol      *psPol;
    ncLimPolIndex    *psPolIndex;        /* 访问控制策略      */
    unsigned long    lSumPol;            /* IP服务策略总数           */
    unsigned long    lSumPolIndex;       /* IP服务策略类别总数       */
} ncLimIpListHead;


typedef struct ncLimInfo_s {
    ncLimIp     *psIp;
    ncLimWeb    *psWeb;
    int         iSumIp;
    int         iSumWeb;
} ncLimInfo;



/* 总体信息    */
typedef struct ncLimHead_s {            /* 总体信息         */
    ncLimInfo         sLim;             /* 全部信息         */
    ncLimInfo         sLimA;            /* 全部             */
    ncLimInfo         sLimG;            /* 组               */
    ncLimInfo         sLimU;            /* 用户             */
    ncLimInfo         sLimD;            /* 策略             */
    ncLimInfo         sLimC;            /* 中心             */
    ncUserContHead    *psUserHead;      /* 全部用户信息     */
    ncLimGroup        *psGroup;         /* 组的索引信息     */
    ncLimWebListHead  sWebList;         /* Web地址列表      */
    ncLimIpListHead   sIpList;          /* Ip地址列表       */
    ncLimPolInfo      *psPolInfo;       /* 访问控制策略索引 */
    unsigned long     lSumPol;          /* 策略总数         */
    unsigned long     lSumUser;         /* 用户总数         */
    unsigned short    nSumGroup;        /* 组的总数         */
    unsigned short    nDefault;         /* 缺省访问    0--  禁止访问  1--允许访问 */
    ncLimPolInfo      *psMypol;         /* 综合策略         */
    ncLimSummer       sSumm;
    char              *pUserHash;       /* 在线用户Hash指针 */
    char              *pIpHash;         /* Ip连线信息       */
    char              *pTcpHash;        /* Tcp连线信息      */
    ncIpNetwork       *psIpNetwork;     /* 网络设置         */
    unsigned long     lSumNet;          /* 网络设置的总数   */
    ncLimVid          *psVid;           /* 虚拟身份         */
    unsigned long     lSumVid;          /* 总的虚拟身份     */
    ncLimVidHead      *psVidHead;       /* 虚拟身份集       */
    ncLimDateHead     *psDateHead;      /* 日期描述         */
    char              cRev[40];         /* 备用             */
} ncLimHead;

/* nc021.c */
                     
#endif
