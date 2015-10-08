#ifndef __NCUPDATEDEF__
#define __NCUPDATEDEF__
#define NC_UPDATE_KEY    "Pro*&6%432asd"
#define NC_UPDATE_ID     65041133L
#define NC_UPDATE_FUN_STATUS     11
typedef  struct proEimProdInfo_s {
    unsigned char caProdSn[16];      /* 产品序列号  */
    unsigned char caVersion[16];     /* 版本号      */
    unsigned char caHostInfo[48];    /* 主机信息    */
    unsigned long lUsers;            /* 用户数      */
    unsigned long lExpire;           /* 失效日期    */
    unsigned long lFun;              /* 功能码      */
    int iLicense;                    /* 1--正常 2--过期 <=0 不正确  */
    unsigned long lStartTime;
    unsigned long lOnlineUser;
    unsigned long lOnlineIp;
} proEimProdInfo;

typedef struct proEimPkgHead_s {
        unsigned long   lId;         /* 文件ID, 网络字节     */
        unsigned short  nFun;        /* 功能代码, 网络字节   */
        unsigned short  nLen;        /* 长度, 网络字节       */
} proEimPkgHead;

#endif