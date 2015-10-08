#ifndef __NETWARNDEF__
#define __NETWARNDEF__
/* Following Add at 2006/02/06
/* 网络督察告警平台  iType 的内容 */
#define NC_WARNT_MAXPKG        1      /* 数据包数量超出  */
#define NC_WARNT_MAXFLOW       2      /* 数据流量超出    */
#define NC_WARNT_MAXIP         3      /* IP连接数量超出  */
#define NC_WARNT_MAXMAIL       4      /* 邮件数量超出    */
#define NC_WARNT_ACCESS        5      /* 访问违禁        */
#define NC_WARNT_FINDPROXY     6      /* 发现代理服务器  */
#define NC_WARNT_CONTROL       7      /* 访问控制告警    */

/* 这是一个Hash表  */
typedef struct ncWarnSystem_s {
    unsigned long  lUserid;       /* 用户ID     */
    unsigned long  lWarnid;       /* 告警ID     */
    unsigned char  cType;         /* 告警类别   */
    unsigned char  cFlags;        /* 告警标识  1-新的告警信息 2-已经处理 */
    unsigned short nCount;        /* 出现次数       */
    unsigned long  lTime;         /* 告警时间(最后一次)  */
    unsigned long  lLastTime;     /* 最后一次处理时间    */
    char     caMesg[128];         /* 告警内容   */
} ncWarnSystem;


#endif