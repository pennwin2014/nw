/* Created By Liyunming at 2008/09/05
*/
#ifndef __NCSNMPDEF___
#define __NCPKGDEF___
typedef struct ncSnmpSwitch_s {
    char        caName[32];        /* 名称  */
    char        caIp[32];          /* 地址                      */
    unsigned short nPort;          /* 端口                      */
    unsigned short nMax;           /* 最大MAC地址数             */
    char        caLogin[32];               /* 登录名                    */
    char        caKey[32];                /* Key                       */
    char        caType[32];                /* 方式: SNMP,Telnet,Http    */
    unsigned long lLastTime;       /* 最后查询时间              */
    unsigned long lTimeStep;       /* 查询时间间隔              */
} ncSnmpSwitch;

#endif
