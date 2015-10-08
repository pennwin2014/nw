#ifndef __NCURLDEF__
#define __NCURLDEF__
#define  NC_URL_UPFILE_ID       62666
#define  NC_URL_UPFILE_URL      11760
#define  NC_URL_UPFILE_CLASS    11761

#define  NC_URL_UPFLAGS_UNKNOW         0         // 未整理的记录
#define  NC_URL_UPFLAGS_LOCK           1         // 正在处理的记录
#define  NC_URL_UPFLAGS_AUTO           2         // 自动整理的记录
#define  NC_URL_UPFLAGS_DONE           5         // 整理完毕的记录
#define  NC_URL_UPFLAGS_FIX            9         // 固定不能修改的记录

#define NC_URL_KEY              "1234Pr0626ay1234"

/* 网址更新文件记录   */
typedef struct ncUrlUpRecord_s {
    unsigned short nId;                /* 记录ID,每条记录的第一个数字为NC_URL_UPFILE_ID */
    unsigned short nType;              /* 记录类别, 11760--URL   11761--类别            */
    unsigned char  cOpt;               /* 操作  d--删除  a-增加                         */
    unsigned char  cLen;               /* 内容长度                                      */
    unsigned short nClass;             /* 类别  0--未分类, >0 具体类别                  */
    unsigned long  lTime;              /* 记录插入时间                                  */
    unsigned char  buf[128];           /* 内容,加密后的内容                             */
} ncUrlUpRecord;

    

typedef struct ncUrlWebName_s {
    unsigned long   lId;                   /* 类别ID                 */
    unsigned char   flags;                 /* 1-- 类别   2-- 网址库  */
    char     name[31];
}ncUrlWebName;

/*  
    网址库初始化后保存在文件中
 */
typedef struct ncUrlCode_s {
    unsigned char   urlCode[16];                // Url的Md5格式
    unsigned short  nId;                        // 所属类别
    unsigned char   cRev;                       // 保留字段
    unsigned char   cFlags;                     // 0--正常  1--新增   9--人工修改
    unsigned long   lCount;                     // 访问次数
    unsigned long   lLastTime;                  // 最后一次访问时间
} ncUrlCode;

typedef struct ncUrlValue_s {
    unsigned short  nId;                    // 所属类别
    unsigned char   nLen;                   // caUrl的长度
    unsigned char   cFlags;                 // 保留字段, 1-新增   9--人工修改
    unsigned long   lCount;                 // 访问次数
    unsigned long   lLastTime;              // 最后一次访问时间
    unsigned char   caUrl[128];             // Url & 说明,以逗号分开，使用Tea算法加密
} ncUrlValue;

/* 在数据库中保存的记录结构   */
typedef struct ncUrlRecord_s {
    unsigned char   urlCode[16];            // 键值
    ncUrlValue      urlValue;               // 内容
} ncUrlRecord;

/* 网址库特殊信息
   综合信息
   ProeimUrlv7.pronetway.com     url,v1,v2,uptime
   类别信息
   ###.webclass.proeim.com
   更新日志
   Num0--保存总体信息            最后更新序号
   ####.updatelog.proeim.com     url,UpLogMesg
   
*/
/* 网址库的头文件             */
typedef struct ncUrlBufHead_s {
    unsigned long   lSize;                  // 字节数
    unsigned long   lMaxUrl;                // 最大Url数量
    unsigned char   cLock;                  // 0-未锁  1-锁定
    unsigned char   cFull;                  // 1 内存已满，需要整理
    unsigned char   cShm;                   // 
    unsigned char   cUpMark;                // 更新标志  0--正常  1--有更新信息   */
    unsigned long   lTimeOut;               // 超时换出时间
    unsigned char   *pHash;
} ncUrlBufHead;



/* 网址搜集计划  
   使用Hash保存, Key为Url
 */
typedef struct ncCapUrl_s {
    unsigned char  caUrl[64];                // Url信息
    unsigned long  lStartTime;               // 起始时间
    unsigned long  lLastTime;                // 终止时间
    unsigned long  lCount;                   // 请求次数
    unsigned long  lSid;                     // 0--新记录  >0--保存过的记录, 其值对应于表中的Sid
} ncCapUrl;

/* Url错误信息搜集  */
typedef struct ncCapErrUrl_s {
    unsigned char   caUrl[128];               // Url信息
    unsigned long   lSip;                     // 访问者IP
    unsigned long   lDip;                     // 目标地址
    long            lStatus;                  // 错误代码
    unsigned char   caMsg[64];                // 错误信息
    unsigned long   lStartTime;               // 起始时间
    unsigned long   lLastTime;                // 终止时间
    unsigned long   lCount;                   // 请求次数
    unsigned long   lSid;                     // 
} ncCapErrUrl;



int ncUrlInitUrlBuf(int iFlags,int iMaxUrl,char *pDbFile);
int ncUrlFreeUrlBuf();
int ncUrlInitShmUrl(char *pDbFile);
int ncUrlOpenUrlDb(char *pFile);
int ncUrlCloseUrlDb();
int  ncUrlGetUrlInfo(char *pUrl,char *pDesc,int *lId);
int ncUrlGetUrlClassId(char *pUrl);
int ncUrlGetUrlFromDbFile(char *pUrl,ncUrlRecord *psRecord);
int ncUrlGenUrlDb(char *pDbFile,int iMaxUrl);
// int ncUrlUpdateVersion(DEPOT *dp,unsigned long lVersion,unsigned long lDbVersion,unsigned long lUpdate);
int ncUrlGetUrlVersion(char *pEngVer,char *pDbVer,char *pUpdate,int *iSumRecord);
int ncUrlCvtMySql2Qdbm(char *pDbFile,unsigned long lTime);
int ncUrlIsInUrlClass(char *pUrl,long lId);
int ncUrlGetClassIdByName(char *pName);
int ncUrlAddUnkonwUrl(char *pUrl);
int ncUrlCvtText2UrlDb(char *pFile,char *pDbFile);
int ncUrlDumpUrlFromDb(char *pDbFile,char *pFile);
int ncUrlLoadNameCache();
char *ncUrlGetChcheName(int flags,unsigned long lId,char *pDefault);
int ncUrlGetChcheIdByName(int flags,char *pName);
char *ncUrlGetClassNameById(unsigned long lId,char *pDefault);
int ncUrlCheckUrlBuf(int iTimeOut);
int ncUrlUpdateUrl(char *pUrl,int lId,char *pDesc);

#endif
