/* 表单分析                         */
#ifndef __NCPOSTDEF___
#define __NCPOSTDEF___
#define NC_WBM_ID        651129L
#define NC_FILE_BUF       8192
#define NC_MAX_POSTBUF    64000L
#define NC_WBM_From      0
#define NC_WBM_To        1
#define NC_WBM_Cc        2
#define NC_WBM_Bcc       3
#define NC_WBM_Sub       4
#define NC_WBM_Conetnt   5
#define NC_WBM_Attach    6
#define NC_WBM_TITLE     7
#define NC_WBM_CHARSET   8    // 字符集
#define NC_WBM_UID
#define NC_WBM_MAXITEM   16
#define NC_MAX_FILEREAD  2048    // 每次读文件的最大数量  
#define NC_MAX_POSTTYPE  16
// 匹配方式
#define NCPOST_MATCH_ALL        0x00
#define NCPOST_MATCH_START      0x01
#define NCPOST_MATCH_END        0x02
#define NCPOST_MATCH_INC        0x03
#define NCPOST_MATCH_CASE       0x10

typedef struct ncXmlVarValue_s {  /* 表单内容    */
    char            *pVar;              /* 变量名        */
    char            *pValue;            /* 变量值        */
    struct ncXmlVarValue_s *next;
} ncXmlVarValue;

/* 表单变量名    */
typedef struct ncPostCont_s {  /* 表单内容    */
    char            *pVar;              /* 变量名        */
    char            *pValue;            /* 变量值        */
    char            *pContentType;      /* 文件类型      */
    unsigned long   lFileId;            /* 文件序号,以 proeim#保存  0--表示是一般字段  */
    struct   ncPostCont_s *next;
} ncPostCont;

#define NC_POST_FORM     1
#define NC_POST_UPLOAD   2
#define NC_POST_XML      3
#define NC_POST_EXCHANGE 4
#define NC_POST_STREAM   5
typedef struct ncPostHead_s {   /* Http头信息(POST请求)   */
    char    *pHost;     /* 主机名       */
    char    *pUrl;      /* Url名称      */
    char    *pBound;    /* 分界符       */
    int     iLen;       /* 内容的长度   */
    short     iType;      /* 类型   1--FORM表单    2--Upload表单    3--XML表单*/
    unsigned  cCode;      /* 编码          */
    unsigned  cRev;       /* 保留          */
    unsigned long lAsize;  /* 附件大小     */
    unsigned long lTime;
    ncPostCont *psCont;
} ncPostHead;

/* WebMail的邮件头                    */
typedef struct ncWbmMailHead_s {
    unsigned long lUserid;                    /* 用户ID   */
    char         *pHost;                      /* 主机名   */
    ncPostCont   *psCont[NC_WBM_MAXITEM];
    struct ncWbmMailHead_s *next;
} ncWbmMailHead;


/* 这是Hash表保存的内容
       1--主机的Webmail格式模板
             name    为 0
             iType --- 0 附件和内容连在一起   1 附件和内容分开
 */
 
typedef struct ncWebMailPlate_s {
    char  caHost[16];     /* 主机名，若为0,表示缺省  */
    char  caName[16];     /* 名字                    */
    unsigned short nLen;  /* 长度                    */
    unsigned short nType; /* 类型  1-From 2-To  3-Cc  4-Bcc 5-Subject 6--Text 7--Attach  9--CharSet*/
} ncWebMailPlate;

typedef struct ncWMPList_s {
    char     caName[16];     /* 名字                    */
    unsigned short nLen;     /* 变量长度                */
    unsigned short nType;    /* 类型                    */
} ncWMPList;


typedef struct ncWMPIndex_s {
    char            caHost[16];   /* 主机名  */
    int             iStart;
    unsigned short  nLen;         /* 主机长度 */
    unsigned short  nSum;
} ncWMPindex;

typedef struct ncWMP_s {
    int             iId;
    int             iSumIndex;
    int             iSumPlate;
    ncWMPindex      *psIndex;
    ncWMPList       *psList;
} ncWMP;


/* 这是 Post表单账号获取规则
   表单以Hash表的方式存放
       根据URL查找 相关记录,进行判断
       支持域名模糊 如: sina.com = *.sina.com
*/
#define NC_POST_NAMEPASS  1
#define NC_POST_NAMEONLY  2

typedef struct ncPostAccount_s {
    char url[64];  // 网址
    int  sid;      // 序列号
    int  from;     // 从何处来
    char account[32];   // 账号对应的变量 
    char passwd[32];    // 密码对应的变量
    char email[32];
    char regid[32];
    unsigned short service;
    unsigned char  flags;
    unsigned char  code;          // 编码
} ncPostAccount;

/* 文件清单   */
typedef struct ncPostDirList_s {
    unsigned long lTime;        // 最后操作时间
    char     caFile[64];        // 文件名
} ncPostDirList;



/* 表单整理
   根据表单特征对表单进行分类
     1.表单列表
     2.URL特征 (主机+特征)
     
*/

typedef struct ncPostFVarList_s {
    char     var[20];     // 变量名
    short    service;     // 服务代码---0不保存
    short    vartype;     // 变量类别, 每种服务类别代码不一样
    struct   ncPostFVarList *next;
} ncPostFVarList;


typedef struct ncPostFilter_s {
    char url[32];                    // 网址
    ncPostFVarList  *psFlist;        // 表单列表
} ncPostFilter;


typedef struct ncPostVarList_s1 {
    char   var[20];                   // 变量名
    char   disp[16];                  // 显示名
    short  vartype;
    short  cFlags;                    // 1--必须    0--可有可无
} ncPostVarList1;
    
/* 表单描述     */
typedef struct ncPostDesc_s {
    char  name[16];                     // 服务名称
    ncPostVarList1  psList;             // 包含变量列表
    int   iMaxVar;                      // 变量总数
    int   (*fFunName)(utShmHead *);     /* 执行函数名称  */
} ncPostDesc;    


typedef struct ncPostDataList_s1 {
    short service;
    short vartype;
    char  var[20];
    char  *pValue;
} ncPortDataList1;



/* 表单判断结构    
        0----Webmail
        1----BBS
        2----UserAccount
        3----Boke
        4----聊天记录
   每一种表单可以有8个特征
*/

/* 变量列表 
 */

#define NC_POSTPAT_INCLUDE         0
#define NC_POSTPAT_START           1
#define NC_POSTPAT_END             2   
#define NC_POSTPAT_EQUAL           3

#define NC_POSTPAT_STRING          0
#define NC_POSTPAT_TEXT            1
#define NC_POSTPAT_DIGITAL         2
#define NC_POSTPAT_DATE            3
#define NC_POSTPAT_FILE            5


#define NC_POSTPAT_MUST            2
#define NC_POSTPAT_NOCOUNT         0
#define NC_POSTPAT_COUNT           1

#define NC_POSTTYPE_BBS             0
#define NC_POSTTYPE_ACCOUNT         1
#define NC_POSTPAT_BBS_CONTENT      0
#define NC_POSTPAT_BBS_TITLE        1
#define NC_POSTPAT_BBS_BBSNAME      2
#define NC_POSTPAT_BBS_ATTFILE      3
#define NC_POSTPAT_BBS_BBSID        4
#define NC_POSTPAT_BBS_USERNAME     5

#define NC_POSTPAT_ACC_USERNAME     0
#define NC_POSTPAT_ACC_DISPNAME     1
#define NC_POSTPAT_ACC_PASSWORD     2

// 表单变量
typedef struct ncPostPatVarList_s {
    unsigned char    caVar[32];                 // 变量名称
    unsigned char    cType;                     // 数据类型 1-字符串  2-文本  3-数字  4--文本 5--文件
    unsigned char    cMust;                     // 0-不是必须  1-必须
    unsigned short   nMaxLen;                   // 长度 0-不限制
} ncPostPatVarList;

// 表单类别
typedef struct ncPostPatType_s {
    unsigned char    caName[32];                // 类别名称 WebMail,BBS,Boke,Chat,...
    unsigned char    cMaxVar;                   // 最大变量数(予分配的内存)
    unsigned char    cMinVar;                   // 最少变量数
    unsigned char    cSumVar;                   // 变量总数
    unsigned char    cDefMinVar;                // 缺省时的最小变量数
    ncPostPatVarList *psVar;                    // 变量列表
} ncPostPatType;

typedef struct ncPostPatVarNum_s {
    unsigned char  cPost;             // 服务代码序号
    unsigned char  cMust;
    unsigned short nVar;              // 变量序号
} ncPostPatVarNum;

// 以下结构 2011/12/17 被李云明修改,增加了两个字段 cMatch,cRev, nSum和nMax有 short 改为 char
typedef struct ncPostPatVar_s {
    unsigned char            caVar[32];           // 变量名
    unsigned char            nSum;                // 服务总数,这个变量所属的服务总数
    unsigned char            nMax;
    unsigned char            cMatch;              // 匹配方式 0--完全 1--Start 2--Stop 3--Include
    unsigned char            cLen;                // 保留
    ncPostPatVarNum          *psVarNum;
}  ncPostPatVar;
    
/* 特征库主机列表，内容按主机排序  */
typedef struct ncPostPatHost_s {
    unsigned char     caHost[64];        // 主机名, 内存自动分配
    unsigned char     caName[32];        // 表单名称,内存自动分配
    unsigned short    nSumVar;           // 变量总数
    unsigned short    nMaxVar;           // 分配空间
    unsigned char     cCode;             // 编码 99-未知，自动判断
    unsigned char     cHostLen;          // pHost 的长度
    unsigned short    nService;          // 服务代码
    ncPostPatVar      *psVar;
} ncPostPatHost;

typedef struct ncPostPatHead_s {
    unsigned short  nMaxType;
    unsigned short  nSumType;
    unsigned long   nMaxHost;
    unsigned long   nSumHost;
    ncPostPatType  *psPostType;
    ncPostPatHost  *psPostHost;
} ncPostPatHead;


typedef struct ncPostDoPostVar_s {
    unsigned short          nPost;
    unsigned short          nVar;
    ncPostCont              *psCont;
} ncPostDoPostVar;
    

typedef struct ncPostResult_s {
    unsigned short   nMax;               // 最大值
    unsigned short   nSum;               // 当前总数
    unsigned char    *pHost;             // 主机名
    unsigned char    *pUrl;              // Url
    unsigned char    *pName;             // 表单名称
    unsigned char     caPost[NC_MAX_POSTTYPE];         // 表单标识 
    unsigned short   nService;
    unsigned short   nRev;
    ncPostDoPostVar  *psDoPost;
} ncPostResult;


#define NC_POSTPAT_FILEID      20091122L
#define NC_POSTPAT_ENGVER      750L
#define NC_POSTPAT_POSTTYPE    8001
#define NC_POSTPAT_HOST        9002

/* 文件头 128个字节  */
typedef struct ncPostPatFileHead_s {
    unsigned long   lId;                // 文件ID
    unsigned long   lVersion;           // 引擎版本号      7.50.01
    unsigned long   lCreateTime;        // 文件创建时间
    unsigned long   lLastUpdate;        // 最后更新时间
    unsigned long   lEncrypt;           // 是否加密
    unsigned long   lDataVersion;       // 协议库的版本号  09.00.01
    unsigned long   lSumType;           // 类别总数
    unsigned long   lSumHost;           // 主机总数
    unsigned char   cRev[32];           // 备用
} ncPostPatFileHead;


/* 文件记录         */
typedef struct ncPostPatFileRecord_s {
    unsigned short  lRecordId;          // 记录标识
    unsigned short  lSumRecord;         // 记录总数
    unsigned long   lCrc;               // 校验位
    unsigned long   lSrcBytes;          // 原来的字节数(未加密)
    unsigned long   lNowBytes;          // 现在字节数(加密后)
} ncPostPatFileRecord;



/* 表单文件内容    */
typedef struct ncPostSynCont_s {
    unsigned long  lId;             // 文件ID
    unsigned long  lLen;            // 数据总长度
    unsigned long  lPos;            // 起始位置
    unsigned short nThisLen;        // 本数据包长度
    unsigned short nMark;           // 1.正常数据  9.最后数据   8.终止传送
}  ncPostSynCont;

/* 表单文件       */
typedef struct ncPostSynBuf_s {
    unsigned char   caName[24];     // 用户标识
    unsigned char   caMd5[16];      // 内容Md5
    unsigned long   lSip;           // 源地址
    unsigned long   lDip;           // 目标地址
    unsigned long   lSize;          // 文件大小
    unsigned long   lTime;          // 时间
    unsigned short  nPost;          // 表单类别
    unsigned short  nCode;          // 编码
    unsigned long   lUserid;         // 用户ID
}  ncPostSynBuf;


/* 同步文件列表结构   */
typedef  struct ncPostSynFile_s {
        unsigned long   lCode;          // 记录标识 NC_POSTSYN_SYNFILEID
        unsigned char   caFile[24];     // 文件名
        ncPostSynBuf    sSynBuf;
} ncPostSynFile;

/* 客户端同步信息   
       名字唯一
   格式:
   # Name   Key   Ip
   Name -- 客户端名称
   Key  -- 通信密钥
   IP   -- IP地址，若值为 Any 表示任意地址,否则只能为指定地址   
 */
typedef struct ncPostSynClient_s {
    unsigned char  caName[32];      // 名称
    unsigned char  md5[24];         // MD5 Key
    unsigned long  lSip;            // 验证地址
    unsigned long  lLastTime;       // 同步时间
    unsigned long  lCount;          // 表单数量
    unsigned long  lSyn;            // 同步数量
    unsigned char  cStatus;         // 当前状态
    unsigned char  cRev[3];
}  ncPostSynClient;



#define NC_POSTSYN_VERSION      100
#define NC_POSTSYN_ERROR        1
#define NC_POSTSYN_READY        100
#define NC_POSTSYN_LOGIN        101
#define NC_POSTSYN_SENDFORM     102
#define NC_POSTSYN_FORMEND      103

#define NC_POSTSYN_REQSYN       106
#define NC_POSTSYN_SYNFILEID    1820674539L



/* 用户IP对照表   
       在内存中定期更新
 */
typedef struct ncPostSynUserIp_s {
    unsigned long lSip;                  /* IP地址  */
    unsigned long lUserid;               /* 用户ID  */
} ncPostSynUserIp;


/* 请求头文件       */
typedef struct ncPostSynReq_s {
    unsigned short  nVersion;       // 版本
    unsigned char   cFun;           // 功能代码
    unsigned char   cSec;           // 是否加密 0-否 1-是
    unsigned char   md5[16];        // 验证码
    unsigned short  nRev;
    unsigned short  nNum;           // 在服务器中的序号，从1开始
    unsigned long   lTime;          // 时间
    unsigned long   lLen;           // 数据长度
}  ncPostSynReq;


    


// 客户端信息
typedef struct ncPostSynCltInfo_s {
    int             iSock;                    // 请求Sock
    unsigned short  nNum;                     // 序号
    unsigned short  nSta;                     // 状态  0-离线  1-在线
    unsigned long   lSip;                     // 客户端IP地址(用于检查)
    unsigned long   lThisIp;                  // 当前地址
    unsigned char   caName[16];               // 客户端名称
    unsigned long   caKey[32];
    unsigned char   caMd5[16];                // Md5值
    unsigned long   lLastTime;                // 最后同步时间
    char            caHome[128];              // 同步目录
} ncPostSynCltInfo;


/* 表单的Md5信息  */
typedef struct ncPostSynPostMd5_s {
    unsigned char caMd5[22];             /* Md5值   */
    unsigned char cRev[2];               /* 保留    */
    unsigned long lTime;                 /* 时间    */
} ncPostSynPostMd5;


/* 同步服务器参数    
 */
typedef struct ncPostSynServer_s {
    int              iReady;         // 0--服务器未准备  1--服务器已准备
    unsigned short   nSumClient;     // 客户端总数
    unsigned short   nMaxClient;     // 最大客户端数
    unsigned long    lSumUser;       // 用户总数
    unsigned long    lTimeOut;       // 超时时间
    unsigned long    lCheckTime;     // 检查时间
    unsigned long    lStepTime;      // 检查时间间隔
    unsigned long    lMinTime;       // 最早时间
    unsigned long    lMaxTime;       // 最近时间
    unsigned long    lMaxMd5;
    unsigned long    lSid;           // 最后的SID
    ncPostSynClient  *psClient;       // 客户端资料
    utShmHead        *psShmHead;
} ncPostSynServer;

typedef struct ncPostProc_s {
    int         iPid;
    unsigned    long lStartTime;
    unsigned    long lLastTime;
    char        caFile[128];
    char        caDoFile[128];
    unsigned    int  iSumFile;
    unsigned    int  iSumDoFile;
    unsigned    int  iSumCurFile;
} ncPostProc;

#endif
