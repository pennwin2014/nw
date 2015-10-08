#ifndef __NCTEST_DEF__
#define __NCTEST_DEF__   1

#define NC_DEVCHKSTA_ERROR    -1
#define NC_DEVCHKSTA_SAFE     0
#define NC_DEVCHKSTA_NORMAL   1
#define NC_DEVCHKSTA_EXCEED   2

#define NC_MAXNETDEV          8
typedef struct ncPingInfo_s {
    char caHost[64];
    int  iStatus;               /*  0--����   9--��ͨ  */
    unsigned long lLastTime;    /*  ������ʱ��       */
    double        fTime;        /* ƽ��Ӧ��ʱ��        */
    int           iCount;       /* �ɹ��յ����ݰ�����  */
    struct ncPingInfo_s *next;
} ncPingInfo;
    
typedef struct ncNetPingSta_s {
    char caDev[16];
    int  iStatus;               /*  0--����   9--��ͨ  */
    double        fTime;        /* ƽ��Ӧ��ʱ��        */
    int           iCount;
    ncPingInfo *psPing;
} ncNetPingSta;


typedef struct ncChkLimit_s {
    int           iTestStepTime;           // ���Լ��ʱ��
    double        fPingStep;               // Ping����ʱ����
    int           iPingNumber;             // Ping��������
    int           iStatus;                 // ��ǰ״̬.... 1-��������  2-��ȫģʽ  
    int           iErrorTime;
    int           iSafeTime;
    int           iMaxErrorTime;           // ���ʱ����
    int           iMaxSafeTime;            // ���ȫ����
    unsigned long lMaxFlow[8];             // ������������
    unsigned long lMaxPkg[8];              // �������ݰ�����
    unsigned long lSafeFlow[8];            // ����������ȫ
    unsigned long lSafePkg[8];             // ���ݰ���ȫ
    unsigned long lMaxLost;                // ��������
    unsigned long lSafeLost;               // ������ȫ��
    double        fMaxTime;                // ��ʱʱ��
    double        fSafeTime;               // ��ȫʱ��
    char          caSafeStart[128];        // ��ȫģʽ����
    char          caSysStart[128];         // ����ģʽ����
    char          caLogFile[128];
    int           iSumDev;
    ncNetPingSta  sPing[NC_MAXNETDEV];
} ncChkLimit;



#endif
