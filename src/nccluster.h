#ifndef __NCCLUSTERDEF___
#define __NCCLUSTERDEF___

#define NC_CLU_HEATREQ      27811          /* ��������             */
#define NC_CLU_HEATRES      27812          /* ����Ӧ��             */
#define NC_CLU_MAXDEV      8             /* ����豸��          */
#define NC_CLU_MAXPROCESS  8             /* ��ؽ��̵��������  */

/* �������ӵĿͻ����б�  */
typedef struct ncCluProcess_s {
    char      caName[16];    // ��������
    int       iSum;           // ������
} ncCluProcess;


/* ����״̬   */
typedef struct ncClusterDevSta_s {
    char              caDev[16];    // ����豸������ eth0,eth1
    unsigned long     lBytes[2];    // ����    0--����   1--����
    unsigned long     lPkgs[2];     // ���ݰ�  0--����   1--����
    unsigned char     cLink;        // 0--Unknow 1--Link  2--NoLink
    unsigned char     cPing;        // 0--��ͨ    1--ͨ
    unsigned char     cWork;        // 0--������  1--�쳣   
    unsigned char     cRev;    
} ncClusterDevSta;

/* ������ձ�   */
typedef struct ncCluDevInfo_s {
    char                    caDev[16];     // �����豸
    char                    caRDev[16];    // �Է��豸
    unsigned long           lBytes[2];     // ��ֵ����    0--����   1--����
    unsigned long           lPkgs[2];       // ��ֵ���ݰ�  0--����   1--����
    unsigned char           cLink;         // 0--Link  1--NoLink
    unsigned char           cPing;         // 0--��ͨ    1--ͨ
    unsigned char           cWork;         // 0--��Ϣ    1--����    
    unsigned char           cRev;
    unsigned char           caPingIp[128]; // ����������ͨ��IP��ַ
} ncCluDevInfo;



/* ������Ϣ       */
#define NC_CLUSTA_NORMAL           1        // ����
#define NC_CLUSTA_ERROR            0        // �쳣
typedef struct ncCluHeat_s {
    unsigned short      nFunId;              // ���ܴ���
    unsigned char       cSec;                // 0-����  1-����
    unsigned char       cLen;                // ����
    unsigned long       lMd5;                // У����
    unsigned long       lLastTime;           // ���ݲɼ�ʱ��
    unsigned char       cStatus;             // ����� 1--����   0--������
    unsigned char       cSumDev;             // �豸����
    unsigned char       cSumProc;            // ��������
    unsigned char       cRev;                // ����
    unsigned long       lIps;                             // ����IP
    unsigned long       lUsers;                           // �����û���
    ncClusterDevSta     psDevInfo[NC_CLU_MAXDEV];         // �豸״̬    
    ncCluProcess        psProc[NC_CLU_MAXPROCESS];        // ����״̬
} ncCluHeat;


/* ������Ϣ   */    
typedef struct ncCluMyInfo_s {
    unsigned char       caName[16];                 // ����
    unsigned char       caKey[16];                  // ͨ����Կ
    unsigned long       lRemoteIp;                  // �Է�IP��ַ
    unsigned short      nUdpPort;                   // UDP�˿�
    unsigned short      nTcpPort;                   // TCP�˿�
    unsigned char       cSumProc;                   // Ҫ���Ľ�������
    unsigned char       cSumDev;                    // Ҫ���������豸����
    unsigned char       cMyStat;                    // 0-����״̬  1-����״̬  2-����ͬ������
    unsigned char       cSec;                       // 0-������    1-����
    unsigned long       lTimeOut;                   // ��ʱ�л�ʱ��
    unsigned long       lCheckTime;                 // �������ݼ����
    ncCluDevInfo        psDevInfo[NC_CLU_MAXDEV];   // �豸����
    ncCluProcess        psProc[NC_CLU_MAXPROCESS];  // ������Ϣ
    unsigned long       lIps;     // ����IP������
    unsigned long       lUsers;   // �����û���
    ncCluHeat           sHeat;                      // ���һ��ͨ������
} ncCluMyInfo;

#endif
