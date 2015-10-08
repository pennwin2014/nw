#ifndef __NCANYPKGDEF__
#define __NCANYPKGDEF__

/* 所抓数据包的包头  */
typedef struct ncPkgFileHead_s {
    unsigned long  lId;     // 文件ID   "%#&@"
    unsigned long  lSec;    // 时间秒
    unsigned long  lUsec;   // 时间微妙
    unsigned long  lLen;    // 保留
} ncPkgFileHead;

/* 协议分析函数  */
typedef struct ncPkgAnyFun_s {
    char  caName[32];                                         /* 说明       */
    int (*ncPkgDo)(ncIpHead *psIpHead,unsigned char *pkg,int l);    /* 处理函数   */
    struct ncPkgAnyFun_s *next;
} ncPkgAnyFun;

typedef struct ncPkgRawFun_s {
    char  caName[32];                                         /* 说明       */
    int (*ncPkgDo)(unsigned char *pkg,int l);    /* 处理函数   */
    struct ncPkgRawFun_s *next;
} ncPkgRawFun;



#endif
