#ifndef __NCPKGDEF___
#define __NCPKGDEF___
#define NC_PKG_TCP_SYN           1
#define NC_PKG_TCP_FIN           2
#define NC_PKG_TCP_ACK           3

#define NC_PKG_HTTP              1
#define NC_PKG_POST              2
#define NC_PKG_GET               3
#define NC_PKG_MAIL              4
#define NC_PKG_POP3              5
#define NC_MTIME                 1000000L
#define NC_SAVE_ERROR            1              /* 错误信息  */
#define NC_SAVE_SLOG             2              /* 会话日志  */
#define NC_SAVE_HTTPLOG          4              /* HttpLog   */
#define NC_SAVE_IPLOG            5              /* IpLog     */

#define NC_MAX_DEV               8              /* 最大监控设备  */
typedef unsigned long long   UINT64;

typedef struct ncPkgBuf_s {
	unsigned char  cUse;           /* 0 -- None  1-- Ready  2--Put Busy   9--从头开始 */
	unsigned char  cFlags;         /* 0 -- eth0  1-- eth1               */
	unsigned short nLen;           /* 数据包的长度,包括前面的头         */
	unsigned long  nVid;           /* Vlan Id                           */
    unsigned char  Indev;         /* 进设备 NSA中用到                  */
    unsigned char  Outdev;        /* 出设备 NSA中用到                  */
    unsigned char  nsavid;        /* 通道序号  */
    unsigned char  rev;
	unsigned long  lPos;           /* 在数据缓冲区中的位置              */
} ncPkgBuf;

#define NC_MAX_PKGDO    16
typedef struct ncPkgHead_s {
    struct ncPkg000_s {
	    unsigned long  lMax;          /* 最大数据包    */
	    unsigned long  lCur;          /* 当前正在操作  */
	    unsigned long  lDo;           /* 正在处理      */
        unsigned long  lPkgPos;
        int            iPid;          /* 处理进程　　　*/
	} s[NC_MAX_PKGDO];
    unsigned long  lPos;          /* 目前位置      */
    unsigned long  lMaxPos;       /* 最大字节数    */
    unsigned long  lBasePos;      /* 起始位置      */
	unsigned long  long         lSumBytes[NC_MAX_DEV][2];  /* 总计流量 0-上行      */
	unsigned long  long         lCurBytes[NC_MAX_DEV][2];  /* 当前流量 1-下行      */
	unsigned long  lCurPkg[NC_MAX_DEV][2];    /* 当前数据包           */
	
	unsigned long  lSumPkg[NC_MAX_DEV][2];    /* 总计数据包           */
	unsigned long  lLastTime;     /* 最后统计时间  */
	unsigned long  lStartTime;    /* 初始化时间    */
	int            iSumPkgDo;
	int            iMark;         /* 1-计算流量 0--不计算 用于流量控制  */
	int            iLock;         /* 0-- Free 1-- Busy                  */
} ncPkgHead;

#define NC_PKG_BUFPRELEN         12

typedef struct ncPkgCont_s {
    unsigned long  lId;           /* 数据包ID,在索引中的序号           */
    char     buffer[3000];        /* 内容                              */
} ncPkgCont;


/* IP数据包内容  */
typedef struct ncPkgContBuf_s {
	unsigned long lPos;            /* 在 ncPkgBuf中的位置  */
	unsigned long lId;             /*  数据包的 ID          */
} ncPkgContBuf;


/* 控制和更新     */
typedef struct ncPkgCtl_s {
    unsigned char cCtl;           /* 控制信息的变化  */
    unsigned char cLog;           /* Log的变化       */
    unsigned char cLock;          /* 锁              */
    unsigned char cRev;           /* 保留            */
    unsigned long lLastTime;      /* 最后时间        */
    unsigned long lCur;           /* 当前位置        */
    unsigned long lDo;            /* 正在处理        */
    unsigned long lMax;           /* 最大位置        */
    char caCtl[128];              /* 控制信息，每一个字节对应一个功能操作 
                                      当第一个字节为0的时候，表明不作任何修改
                                         */
} ncPkgCtl;

#define NC_LOG_ABNORMITY 1            /* 异常  */
#define NC_LOG_SESSION   2            /* 会话  */
#define NC_LOG_PKGSES       3            /* 要分析的数据包  */
#define NC_CTL_LOADNETSET         1     /* 装载网络设置参数   */

typedef struct ncPkgLog_s { /* Log的结构  */
    unsigned char  cFlags;       /* 0--无  1--有  9--结束      */
    unsigned char  cType;        /* 1--异常访问   2--会话内容  */
    unsigned short nLen;         /* 数据包的长度               */
	unsigned short nService;     /* 服务类型                   */
	unsigned short nFun;         /* FunCode                    */
    unsigned long  lUsid;        /* 用户登陆ID                 */
    unsigned long  lPos;         /* 位置，和实际的文字保持一致 */
    unsigned long  lTime;        /* 时间                       */
    unsigned long  lUserid;      /* 用户ID                     */
} ncPkgLog;



typedef struct ncPkgSession_s {     /* 会话内容分析  */
    unsigned long    lSid;          /* 同 tcp中sid, 会话的标识  */
	unsigned long    lSeq;          /* 数据包的序列号   */
	unsigned long    lSip;          /* 源地址           */
	unsigned long    lDip;          /* 目标地址         */
	unsigned short   nSport;        /* 源端口           */
	unsigned short   nDport;        /* 目标端口         */
	unsigned short   nLen;          /* 数据包的长度     */
	unsigned char    cMark;         /* 编码             */
	unsigned char    cDict;         /* 数据包方向 0--请求  1--应答  */
	unsigned long    lAckSeq;
	unsigned char    pBuf[3000];    /* 数据包的内容    */
} ncPkgSession;


/* 用来保存内容分析的序列号  */
typedef struct ncPkgSeqInfo_s {
	unsigned long  lSid;         /* Session Id     */
	unsigned long  lSeq;         /* 初始序列号     */
	unsigned short nCount;       /* 文件计数器     */
	unsigned char  nMark;        /* 标识 0--无数据  1--有数据  根据协议的不同而不同  9--退出 */
	unsigned char  cFun;         /* 子功能          */ 
	unsigned long  lLastTime;    /* 最后访问时间    */
	unsigned long  lLen;         /* 当前数据长度    */ 
	unsigned long  lSid1;        /* 若文件，保存的是文件ID */
	unsigned long  lSid2;        /* 备用，不同的应用有不同的用处  */
	unsigned char  caFrom[48];
	unsigned char  caTo[48];
} ncPkgSeqInfo;

/* 针对表单后面的结构    
     从 ncPkgSeqInfo 的第24个字节开始
 */
typedef struct ncPkgPostSeq_s {
    unsigned short  nNum;         /* 当前序号   */
    unsigned short  nRev;         /* 保留       */
    struct   ncPkgPostSeqLst_s {
        unsigned long   lCount;   /* 序号       */
        unsigned long   lSeq;     /* 起始序列号 */
    } sSeq[12];
} ncPkgPostSeq;


/* 用来保存数据库内容分析的序列号  
*/
typedef struct ncDbmonSeqInfo_s {
	unsigned long  lSip;         /* 源地址          */ 
	unsigned long  lDip;         /* 目标地址        */
	unsigned short nSport;       /* 源端口          */
	unsigned short nDport;       /* 目标端口        */
	
	unsigned long  lSid;         /* Session Id     */
	unsigned long  lSeq;         /* 上行序列号     */
	unsigned short nService;     /* 服务类别       */
	unsigned char  cMark;        /* 标识 1--AuthRequest  2--AuthResponse   */
	unsigned char  cFun;         /* 子功能          */ 
	unsigned long  lLastTime;    /* 最后访问时间    */
	unsigned char  caUser[32];       // 用户名
	unsigned char  caDbname[32];     // 数据库名
	unsigned long  lStartTime;       // 开始时间
	unsigned long  long lBytes[2];   // 流量 0--上行  1--下行
    unsigned long  lAckSeq;          // 最后下行序列号
	unsigned char  caRev[8];         // 保留  caRev[0] = k
} ncDbmonSeqInfo;



#define NC_ATTFILE_ID  19651129L
typedef struct ncPkgAttFile_s {
    unsigned long  lId;
    unsigned long  lSid;
    unsigned long  lUserid;
    unsigned long  lSip;
    unsigned long  lDip;
    unsigned short nSport;
    unsigned short nDport;
    unsigned char  caFile[128];
    unsigned char  caFrom[64];
    unsigned char  caTo[64];
    unsigned long  lSize;
} ncPkgAttFile;
    

typedef struct ncMsnP2PHead_s {
        unsigned long lId;   /* Channel Session Id */
        unsigned long lSid;
        unsigned long long lOffset;
        unsigned long long lTotal;
        unsigned long lMsgSize;
        unsigned long lFlags;
        unsigned long lAckId;
        unsigned long lAckUid;
        unsigned long long lAckSize;
} ncMsnP2PHead;
        
typedef struct ncPkgIpList_s {
    unsigned long   lLip;               /* IP地址下限           */
    unsigned long   lUip;               /* IP地址上限           */
} ncPkgIpList;

typedef struct ncPkgPortList_s {
    unsigned long   lLport;               /* 端口下限           */
    unsigned long   lUport;               /* 端口上限           */
} ncPkgPortList;


/* 数据包过滤   */
typedef struct ncPkgFilter_s {
    int             iProtocol;    /* 0-- 全部  6--Tcp  17--Udp  */
    unsigned short  nSumSip;
    unsigned short  nMaxSip;
    unsigned short  nSumDip;
    unsigned short  nMaxDip;
    unsigned short  nSumSport;
    unsigned short  nMaxSport;
    unsigned short  nSumDport;
    unsigned short  nMaxDport;
    ncPkgIpList     *psSip;
    ncPkgIpList     *psDip;
    ncPkgPortList   *psSport;
    ncPkgPortList   *psDport;
} ncPkgFilter;

/*  多路监控   
    抓包过滤
 */
typedef struct ncPkgCapFilter_s {
    unsigned long   lDid;              /* 设备类别     */
    unsigned long   lLip;              /* 起始IP地址   */
    unsigned long   lUip;              /* 终止IP地址   */
    unsigned short  nLport;            /* 起始端口     */
    unsigned short  nUport;            /* 终止端口     */
    unsigned char   cProt;             /* 协议 0-- ALL  6 Tcp  17 Udp  */
    unsigned char   cFlags;            /* 0--不监控   1--监控          */
    unsigned char   cRev[2];           /* 保留         */
} ncPkgCapFilter;


typedef struct ncPkgCapDev_s {
    unsigned long lSid;
    char  caName[32];                  /* 说明       */
    char  caDev[64];                   /* 设备名     */
    int   iType;                       /* 抓包方式   */
    int   iStart;                      /* 在 Filter中的起始位置, From 0  */
    int   iSum;                        /* 记录数                         */
    int   iLine;                       /* 在连接中的序号                 */
    int   iPromisc;                    /* 抓包模式                       */
    ncPkgCapFilter *psFilter;
} ncPkgCapDev;

typedef struct ncPkgCapDevHead_s {
    int iSumDev;
    int iSumFilter;
    ncPkgCapDev     *psDev;
    ncPkgCapFilter  *psFilter;
} ncPkgCapDevHead;

typedef struct ncPkgBaseVar_s {
    unsigned short  nOff;             // 起始位置
    unsigned char   cType;            // 类型  1-String,以0结尾  2-String, 指定长度  
    unsigned char   cLen;             // 长度
} ncPkgBaseVar;

typedef struct ncPkgBase001_s {
    unsigned long   lId;              // Service Id
    unsigned short  nFun;             // 功能代码
    unsigned char   cProc;            // 处理方法  0--终止  >0 继续处理
    unsigned char   cFlags;           // 长度处理方法  0-不判断长度   1-最大长度   2-固定长度  3-一个字节长度  4-两个字节长度
    unsigned short  nBase;            // 固定长度
    unsigned short  nLen[2];          // 长度内容
    unsigned char   cSum;             // 匹配总数
    unsigned char   cSumV;            // 变量总数
    unsigned short  nOff[16];         // 偏移量
    unsigned char   cCont[16];        // 内容
    ncPkgBaseVar    sVar[4];
} ncPkgBase001;


typedef struct ncTagAttr_s {
    unsigned char name[32];
    unsigned char value[128];
} ncTagAttr;


typedef struct ncFlowInfo_s {
    unsigned long lLongTime;            // 采样时间,秒
    unsigned long lConTime;             // 持续时间,毫秒
    unsigned long long  lBytes[2];      // 0--上行  1--下行
    unsigned long       lPkg[2];        // 数据包
}ncFlowInfo;

typedef struct ncDevFlowInfo_s {
    unsigned char caName[16];            // 设备名
    int                 sid;
    ncFlowInfo          sBase;           // 基准值，存放的是绝对值
    ncFlowInfo          *psFlow;         // 各采集点的数据，存放的是速率
}ncDevFlowInfo;

/* 网络接口实时流量数据   

*/
typedef struct ncFlowInfoHead_s {
    unsigned short nSumDev;       // 设备总数
    unsigned short nMaxFlow;
    unsigned short nTimeStep;     // 取样时间间隔,单位为秒
    unsigned short nCurRead;      // 当前读的记录
    unsigned short nCurWrite;     // 最后写入的记录
    unsigned char  cLock;         // 0--正常  1--被锁
    unsigned char  cRev;
    ncDevFlowInfo  *psDev;
}ncFlowInfoHead;


/*
       GbComCapWap 格式
       SRC:  61000   DST: 61000
       数据:  (46个字节)
         4    int L2W DstIp
         4    int L2W SrcIP
         4    int L2W ID
         2    short   Len
         2    OffSet
         1   Df
         1   Lf
         6   L2wRsvPap
         6   VdevDstMac
         6   VdevSrcMac
         2   VdevProto
         1   VdevPriority
         1   VdevRsv
         4   VdevPortMap
         2   VdevPap
*/

typedef struct ncGbcomCapWapData_s {
    uint4 L2wDstIp;
    uint4 L2wSrcIp;
    uint4 L2wId;
    uint2 L2wLen;
    uint2 L2wOffset;
    uchar L2wDf;
    uchar L2wLf;
    uchar   L2wRsvPap[6];
    uchar   VdevDstMac[6];
    uchar   VdevSrcMac[6];
    uint2   VdevProto[2];
    uchar      VdevPriority;
    uchar      VdevRsv;
    uchar   VdevPortMap[4];
    uchar   VdevPap[2];
} ncGbcomCapWapData;


/* NSA  NPCP数据结构   
     cmd--报文操作
     len--报文长度
     epif--输出物理接口
     ipif--输入物理接口
     qoslv1id--一级QOSID
     qoslv0id--0级QOSID
     cause_id---上交原因
     
 */
typedef struct ncNpcpHead_s {
    u_int64_t pktprc:8, pktinf:8, rsv0:4, pkttype:4, type:8, qospri:4, rsv1:2, qoscolor:2,
                  epif:3, rsv2:1, ipif:3, rsv3:1, len:11, rsv4:1, drop:1, host:1, mirror:1, rsv12:1;
    u_int64_t qoslv1id:13, rsv5:3, qoslv0id:10,rsv6:6, ethtype:16, elif:12, rsv7:4;
    u_int64_t acl_id:16, GET:1, rsv8:7, cause_id:8, ssn_id:24, rsv9:4,
                  acl_avl:1, ssn_avl:1, rsv10:2;
    u_int64_t rsv11:32, nsa_tx:16, host_tx:16;    
} ncNpcpHead;

#define SSN_HASH_BUCKET_SIZE            0x400000
#define SSN_HASH_BUCKET_SIZE_BITS   22
#define SSN_HASH_BUCKET_BITS        2
#define SSN_HASH_VC_NUMS1_BITS0                 0
#define SSN_HASH_VC_NUMS2_BITS1                 1
#define SSN_HASH_VC_NUMS4_BITS2                 2
#define SSN_HASH_VC_NUMS8_BITS3                 3

#define SSN_HASH_VC_NUMS1                               1
#define SSN_HASH_VC_NUMS2                               2
#define SSN_HASH_VC_NUMS4                               4
#define SSN_HASH_VC_NUMS8                               8

#define SSN_HASH_VC_NUMS1_MASK                  (SSN_HASH_VC_NUMS1-1)
#define SSN_HASH_VC_NUMS2_MASK                  (SSN_HASH_VC_NUMS2-1)
#define SSN_HASH_VC_NUMS4_MASK                  (SSN_HASH_VC_NUMS4-1)
#define SSN_HASH_VC_NUMS8_MASK                  (SSN_HASH_VC_NUMS8-1)


static inline int ncSsnVcid(int pos, int vc_num)
{
    switch(vc_num) {
                case SSN_HASH_VC_NUMS1:
                        break;
                case SSN_HASH_VC_NUMS2:
                        return (pos)>>(SSN_HASH_BUCKET_SIZE_BITS+SSN_HASH_BUCKET_BITS-SSN_HASH_VC_NUMS2_BITS1);
                        break;
                case SSN_HASH_VC_NUMS4:
                        return (pos)>>(SSN_HASH_BUCKET_SIZE_BITS+SSN_HASH_BUCKET_BITS-SSN_HASH_VC_NUMS4_BITS2);
                        break;
                case SSN_HASH_VC_NUMS8:
                        return (pos)>>(SSN_HASH_BUCKET_SIZE_BITS+SSN_HASH_BUCKET_BITS-SSN_HASH_VC_NUMS8_BITS3);
                        break;
                default:
                        break;
    }
    return 0;
}


#endif
