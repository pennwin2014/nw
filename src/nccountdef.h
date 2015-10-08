/* 自动统计  */

#ifndef __NCCOUNTDEF_DEF__
#define __NCCOUNTDEF_DEF__   1
#define NC_COUNT_IPLOG      1
#define NC_COUNT_SERVICE    2
#define NC_COUNT_SERVICEIP  3
#define NC_MAX_COUNT            1000000L
#define NC_MAX_HTTP_COUNT       200000L

typedef struct ncCountLogPos_s {
    char  caName[32]; // 变量名
    short nStart;     // 起始位置，从0开始, -1表示无记录
    short nLen;       // 长度
} ncCountLogPos;

typedef struct ncIpCountBuf_s {
    unsigned long   userid;      // 用户ID
    unsigned long   lTime;       // 时间
    unsigned long   lIp;         // 当 nType=1 目标IP地址   nType=2 服务代码   nType=3: userid== lIp DipService
    unsigned short  nType;       // 1 目标地址   2 服务代码 3 ServiceIp
    unsigned short  nCtime;      // 连接时间
    unsigned long   long lUb;    // 上行字节数
    unsigned long   long lDb;    // 下行字节数
} ncIpCountBuf;



typedef struct ncHttpCountBuf_s {
    unsigned long   userid;      // 用户ID
    unsigned long   lTime;       // 时间
    unsigned char   host[64];    // 主机名
    unsigned long   lDip;        // 目标地址
    unsigned long   lCtime;      // 连接时间
    unsigned long   lCount;      // 请求次数
    unsigned long   lTid;        // 网址类别
    unsigned long   long lBytes; // 字节数
} ncHttpCountBuf;



int ncCountGetDataFromBuf(ncCountLogPos *psLogPos,char *pBuf,int iSum,...);    


#endif
