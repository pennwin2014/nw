/* ������ʱ��    2005/09/21 */
/* #include  "ncshmdebug.h"   */
#ifndef __NCSYSINFOEF__
/* ������Ϣ    */
#define NC_SYS_NETSUMFLOW         8    /* �ɼ����ݻ���     */
#define NC_SYS_MAXDEV            16    /* ��������豸��   */
typedef struct ncSysNetFlow_s {
    unsigned long   lRecvBytes;         /* �����ֽ�  */
    unsigned long   lSendBytes;
    unsigned long   lRecvPkgs;
    unsigned long   lSendPkgs;
    unsigned long   lRerrs;
    unsigned long   lSerrs;
    unsigned long   lTestTime;           /* �ɼ�ʱ��  */
    unsigned long   lStepTime;
} ncSysNetFlow;
    
typedef struct ncSysDevInfo_s {
    char            caDevName[16];  /* �豸��  */
    unsigned char   cLink;          /* ����״̬ 2--No Link  1--Link OK    8--NoCheck 9--NoDEv  */
    unsigned char   cStatus;        /* ��ͨ״̬ 0--��ͨ     1--����                 */
    unsigned char   cRev[2];
    unsigned long   lWidth;         /* �������                                     */
    ncSysNetFlow    sBaseFlow;      /* ��һʱ�εĲɼ�����                           */
    ncSysNetFlow    sFlow[NC_SYS_NETSUMFLOW];      /* ���30���ӵ�����,Ŀǰ��ʱʹ�õ�һ��              */
} ncSysDevInfo;

typedef struct ncSysNetInfo_s {
    int             iSumDev;
    int             iCur;
    ncSysDevInfo    sDev[NC_SYS_MAXDEV];
} ncSysNetInfo;


typedef struct ncSysNetIp_s {
    char   caDevName[16];
    char   caIp[16];
    char   caMac[18];
    char   caMask[16];
} ncSysNetIp;

/* ���̵�CPUռ����  */
typedef struct ncSysCpuRate_s {
    unsigned char caName[16];     /* ������      */
    unsigned long iPid;           /* ���̺�      */
    double        fCpuRate;          /* Cpuռ����   */
    double        fMemRate;          /* Cpuռ����   */
} ncSysCpuRate;


typedef struct ncSysProcessList_s {
    unsigned char caUser[32];
    unsigned char caName[32];
    unsigned char caStart[240];
    unsigned char caStop[240];
    unsigned long lMinProc;
    unsigned long lMaxProc;
    unsigned long lThisProc;
}   ncSysProcessList;


typedef struct ncSysProcHead_s {
    ncSysProcessList    *psList;
    int                 iSum;
    int                 iMax;
} ncSysProcHead;


#define __NCSYSINFOEF__
#endif
