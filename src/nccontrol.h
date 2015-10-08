/* 访问控制  */
#ifndef __NCCONTROL_DEF__
#define __NCCONTROL_DEF__   1
#define NC_TABLES_ADDDROP          0     /* 增加一条禁止规则      */
#define NC_TABLES_CHECKTABLE       1     /* 检查iptables 的内容   */
#define NC_TABLES_RESETTABLE       2     /* 重置Iptables的内容    */
#define NC_TABLES_REDICTPORT       3     /* 端口重定向            */
#define NC_TABLES_CANCELREDICT     4     /* 取消端口重定向        */
#define NC_TABLES_DELSIP           5     /* 删除指定源地址的规则  */
#define NC_TABLES_ADDDROPS         6     /* 增加一条倒过来的禁止规则    */
#define NC_TABLES_REDICTPORTS      7     /* 增加一条倒过来的重定向规则  */
#define NC_TABLES_ADDDROPP2P       8     /* 增加一条P2P规则             */
#define NC_TCWIDTH_SETUSER         11    /* 设置一个用户的带宽          */
#define NC_TABLES_NONE             99    /* 已经处理              */ 
/*****************************************/
#define NC_TABLES_SIGNAL           5     /* 往指定进程发消息      */
#define NC_TABLES_REMOTEFILE       6     /* 远程文件系统          */
#define NC_TABLES_DISPERROR        7
typedef struct IpControHead_s {
    unsigned char  cMod;             /* 修改标志, 0 没有修改 1 修改       */
    unsigned char  cCheck;           /* 检查标识, 0 正常     1 正在检查   */
    unsigned char  cRev[2];          /* 保留                              */
    unsigned long  lCheckTime;       /* 最后检查时间                      */
    unsigned long  lCur;             /* 当前正在操作                      */
    unsigned long  lDo;              /* 当前正在处理                      */
    unsigned long  lMax;             /* 命令缓冲区最大记录数              */
} IpControlHead;

typedef struct IpControlInfo_s {
    unsigned long  lSip;             /* 起始IP                      */
    unsigned long  lDip;
    unsigned short nDport;
    unsigned char  cProt;            /* 协议                                 */
    unsigned char  cComm;            /* 命令 0--Add 1--Del 2--Flush          */
    unsigned long  lStop;            /* 终止时间 hhmmss                      */
    unsigned long  lLastTime;        /* 最后操作时间,0 表示该记录没用        */
} IpControlInfo;

// 访问控制方式
#define NC_IPT_NETWORK    1
#define NC_IPT_LOCAL      0
#define IPT_COM_SYSTEM    101
#define IPT_COM_IPTABLES  102
#define IPT_COM_TC        103
#define IPT_COM_KEY       "abc^&*543567WE"
typedef struct IptCommand_s {
    unsigned short nFun;              /* 命令  0--System命令  1--Iptables 2--Tc   */
    unsigned short nLen;              /* 长度, 加密后的                           */
    unsigned long  lCheck;            /* 校验位,原文校验                          */
} IptCommand;


/* Ip Tables缓存    */
typedef struct NcIptBuffer_s {
    char Command[132];                // 命令
    unsigned long lTime;              // 命令创建时间
} NcIptBuffer;

    
#endif
