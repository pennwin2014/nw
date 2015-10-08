// �ļ������������ݽṹ

#ifndef __NCFILESERVERDEF___
#define __NCFILESERVERDEF___
#include "pasmessage.h"
#define  NCFSS_VERSION    "V2.00.02"
#define  NCFSS_RELEASE    "2013/06/18"

#define NC_LNK_FSSERVERLNK    146
#define NCFSS_FILEBUF_LEN     8192
#define NCFSS_MAX_EVENT       512

#define NCFSS_FUN_SERVERREADY       210000000L         // ������׼������
#define NCFSS_FUN_FILESEND          210008001L         // �ļ��ϴ�
#define NCFSS_FUN_FILEDOWN          210008002L         // �ļ�����

#define NCFSS_FUN_REQSEND           210008003L         // �����ϴ��ļ�
#define NCFSS_FUN_ENDSEND           210008004L         // ���ݴ��ͽ���
#define NCFSS_FUN_REQDOWN           210008005L         // �����´��ļ�
#define NCFSS_FUN_ATTFILESTART      210008006L         // �����ϴ���ʼ
#define NCFSS_FUN_SYNCLIENT         210008101L         // ͬ���ͻ�����Ϣ



#define NCFSS_OPT_FILEUPLOAD    1                  // �ļ��ϴ�
#define NCFSS_OPT_FILEDOWNLOAD  2                  // �ļ��´�
#define NCFSS_OPT_FILEDO        3                  // �ļ�����

typedef struct ncFSSClient_s {
    unsigned long lDid;                          // �ͻ���ID
    char          caName[32];                    // �ͻ�������
    char          caDispName[64];                // ��ʾ��
    char          caProdSn[16];                  // ��Ʒ���к�
    char          caDoTime[64];                  // ����ʱ�� 
    unsigned long lSip;                          // �ͻ���IP��ַ
    unsigned long lLastSynTime;                  // ���һ��ͬ��ʱ��
    unsigned long lLastTime;                     // ������ʱ��
    unsigned long lDataTime;                     // �������ʱ��
    unsigned char AuthCode[16];                  // ��Ȩ��
    unsigned long lBaseTime;                     // ��һʱ���ʱ��
    unsigned long long lBaseBytes[2];            // ��һ��ʱ��������   0--�ϴ�  1--����
    unsigned long long lBytes[2];                // �ֽ���
    unsigned long lFiles[2];                     // �ļ�����
    int           iFd;                           // ͨ�ž��
    unsigned long lLastFdTime;                   // ���ͨ��ʱ��
    unsigned char cStatus;                       // 0--������       1--����
    unsigned char cData;                         // 0--���ڴ�����   1--���ڴ�����
    unsigned char cKeep;                         // 0--����         1--���ڱ���
    unsigned char cDo;                           // 0--������       1--������  2--���մ�
} ncFSSClient;

/* �ͻ���״̬��Ϣ  */
typedef struct ncFSSClientStatus_s {
    unsigned long lDid;                          // �ͻ���ID
    char          caName[32];                    // �ͻ�������
    char          caDispName[64];                // ��ʾ��
    char          caProdSn[16];                  // ��Ʒ���к�
    char          caDoTime[64];                  // �����ϴ�ʱ��
    unsigned long lSip;                          // �ͻ���IP��ַ
    unsigned char AuthCode[16];                  // ��Ȩ��
    unsigned char cStatus;                       // 0--����  1--����   2--����
    unsigned char cKeep;                         // 1--���ڱ���
    unsigned char cDo;                           // 0--������   1--����
    unsigned char cRev;
} ncFSSClientStatus;

typedef struct ncFSSProcess_s {
    int           iPid;                            // ���̺�
    unsigned long lDid;                            // ��Ӧ�Ŀͻ���ID
    unsigned long lMyNum;                          // ����¼�����
    unsigned long lStartTime;                      // ������ʼʱ��
    unsigned long lLastTime;                       // ������ʱ��
    unsigned char cStatus;                         // 0--����  2--�Ѿ���ռ�� 1--���ڹ���
    ncFSSClient   *psClient;
    unsigned char cRev[3];    
} ncFSSProcess;

typedef struct ncFSSDataProcess_s {
    int           iPid;                            // ���̺�
    char          caName[16];                      // ��������
    unsigned long lStartTime;                      // ������ʼʱ��
    unsigned long lLastTime;                       // ������ʱ��
    unsigned long lCount;                          // �����¼��
    int      (*ncFssDoProcess) (utShmHead *psShmHead,char *pArg);
    char          caArg[32];
} ncFSSDataProcess;

#define NCFSS_MAX_DATAPROCESS                   32

typedef struct ncFSSHead_s {
    unsigned long   lMaxProcess;
    unsigned long   lSumProcess;
    ncFSSProcess    *psProcess;
    unsigned char   *pClientHash;
    char            caUploadPath[128];             // �ϴ��ļ���ŵ�·��
    char            caAttPath[128];                // ������ŵ�·��
    char            caDownPath[128];               // ����·��
    unsigned short  nDataPort;                     // ���ݷ������˿�
    unsigned short  nCtlPort;                      // ���Ʒ������˿�
    int             iMaxCtlSock;
    int             iSumCtlSock;
    char            caDoTime[128];                 // �����ϴ�ʱ��
    char            caDownTime[128];                 // �����´�ʱ��
    int             iLockDoDb;                     // 0--��������  1--��ͣ����
    unsigned long   lLockTime;                     // ����ʱ��
    unsigned long   lLockTimeOut;                  // ������ʱ
    char            caName[256];                   // Ҫ�����ı�
    int             iSumDataProcess;               // ���ݴ����������
    unsigned long   lStartTime;                    // ��������ʱ��
    ncFSSDataProcess sDataProc[NCFSS_MAX_DATAPROCESS];
} ncFSSHead;



ncFSSHead    *ncFSSGetHead(utShmHead *psShmHead);  
ncFSSProcess *ncFSSGetProcessNum(ncFSSHead *psFSSHead);
ncFSSClient  *ncFSSGetClient(ncFSSHead *psHead,unsigned long lDid);
ncFSSClient  *ncFSSCheckClient(ncFSSHead *psHead,pasMsgHead *psMsgHead);


#define NCFSC_MATCH_STRING               0
#define NCFSC_MATCH_REGEX                1

#define NCFSC_SEND_PAS                   1
#define NCFSC_SEND_FTP                   2

typedef struct ncFSCForwardScode_s {
    uchar     caCode1[128];                // �����˱��������, ����
    uchar     caCode2[128];                // �����˱��������, ����,��Ϊ����,��������Ľ��
    uchar     cType;                       // 0--һ���ַ�ƥ��   1--������ʽ
    uchar     status;                      // 0--����           1--��ȷ
    uchar     cRev[2];
} ncFSCForwardScode;
    
typedef struct ncFSCForwardClient_s {
    uchar  caName[32];                       // ����������
    uint4  lSip[4];
    uint2  nPort[4];
    uchar  cSendType[4];                     // ���ͷ�ʽ    PAS     FTP
    uchar  caUsername[4][32];                // ��¼���û���
    uchar  caKey[4][32];                     // ͨ����Կ
    uint4  lMyid[4];                         // �Լ���ID
    uchar  caDoTime[128];                    // ���ݴ���ʱ��
    uint4  iStepTime;                        // ����ʱ����
    uint4  iSumCode;                         // ����������
    struct ncFSCForwardScode_s  *psCode;     // �����˱��뷶Χ
} ncFSCForwardClient;
    

/* ת����Ϣ

*/
typedef struct ncFSCForwardInfo_s {
    char                caDataPath[4][128];                  // ��־���ݱ����·��,  �����ж��·��
    char                caOnlineFile[4][128];                // �����û�
    char                caRemote[4][128];
    char                caKeepPath[4][128];
    uchar               cKeep[4];                            // ������־   1--����  0--������
    ncFSCForwardClient  *psClient;
    int4                iSum;
} ncFSCForwardInfo;


#endif
