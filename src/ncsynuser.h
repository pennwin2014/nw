#ifndef __NCSYNUSER___
#define __NCSYNUSER___

/* 用户信息同步   */
#define SYN_ONBASE_IP       1    // 基于IP地址
#define SYN_ONBASE_MAC      0    // 基于MAC地址
#define SYN_FROM_CAMS       5
typedef struct ncSynUserInfo_s {
    unsigned long  ip;          /* IP地址                       */
    unsigned char  mac[6];      /* Mac地址                      */
    unsigned char  flags;       /* 记录来源 1--固定不变 8--Wins 9--DHCP */
    unsigned char  status;      /* 状态 0-不在线  1-在线                */
    unsigned char  uname[32];   /* 用户名      */
    unsigned char  cname[32];   /* 主机名      */
    unsigned char  dept[32];    /* 部门      */
    unsigned long  lLastTime;   /* 最后操作时间  */
} ncSynUserInfo;

typedef struct ncCamsGroup_s {
    unsigned long groupid;
    unsigned char groupname[32];       /* 部门名称  */
    struct ncCamsGroup_s *next;
} ncCamsGroup;

#endif
