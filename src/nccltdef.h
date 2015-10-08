/* ���綽��������Ŀͻ���  */
#ifndef __NCCLTDEF___
#define __NCCLTDEF___
#include "/home/ncmysql/ncs/src/ncsdef.h"
#define  NCS_MAX_SERVERIP          8
#define  NCS_LOG_FILE             "../log/ncclient.log"
#define  NC_CLT_VERSION           NC_VERSION     // �ͻ��˰汾  */
#define  NCS_STA_ERROR             -1      // δ��¼
#define  NCS_STA_NOTLOGIN          0       // δ��¼
#define  NCS_STA_SUCCESS           1       // ��¼�ɹ�
#define  NCS_FIL_CASECTL           1       // ��������
extern int iNcClient;                      // 0-�����ӹ�������  1-���ӹ�������
extern int iNcSendGA;                      // 1-����Ϣ�������Ϲ�����׼�Ĺ�������

typedef struct ncsCltInfo_s {
    unsigned long  lId;                   // �ͻ���ID
    unsigned long  lGid;                  // ���ID
    unsigned long  lSip;                  // ��������ַ
    unsigned short nTcpPort;              // �������˿�
    short nStatus;                        // ��ǰ״̬
    unsigned long  lLasttime;             // ������ʱ��
    int            iBaseTime;             // ʱ��ƫ����
    unsigned short nMyPort;               // ����TCP�˿�
    unsigned short nUdpPort;              // ͬ����Ϣ 0--����Ҫ 1--��Ҫ
    char           caName[32];
    char           caPass[32];             
    char           caKey[32];             // ͨ����Կ
    char           caMessage[128];        // ������
    unsigned long  lSynCheck;             // У����
    unsigned long  lTimeStep[2];          // ��Ϣ����ʱ����
    ncsSynInfo     syninfo[5];            // ͬ����Ϣ  4--��������ͬ������Ϣ
    ncIpAddress    sNcsAdd[NCS_MAX_SERVERIP];   // ���綽���������
    ncIpAddress    sProAdd[NCS_MAX_SERVERIP];   // ���綽����������
    ncIpAddress    sDsAdd[NCS_MAX_SERVERIP];    // �������ķ�����
    unsigned  long lCaseTime;                   // ����������ʱ��
    unsigned  long lMsgTime;                    // ��Ϣ�������ʱ��
    unsigned  long lMsgSndTime;                 // ��Ϣ�������ʱ��
    unsigned  long lSumCaseIndex;               // ������������
    unsigned  long lDbSndStime;                 // ������־��Ϣ�����ʱ��--��ʼʱ��
    unsigned  long lDbSndEtime;                 // ������־��Ϣ�����ʱ��--��ֹʱ��
    unsigned  char cModMon;                     // 1--�������   0--����δ���� 9--���ڽ���
    unsigned  char cModCtl;                     // 1--�������   0--δ����     9--���ڽ���
    unsigned  char cSynTime;                    // �Ƿ�ͬ��ʱ��
    unsigned  char cRev1;
    unsigned  long lComTime;                    // �������ͬ��ʱ��
    unsigned  long lTopCaseTime;                  // ���ϼ��������ĵ�CaseTime  LLastCaseTime
    unsigned  char cRev[52];                    // ����
} ncsCltInfo;

#define NC_CSYN_MAX         2
#define NC_CSYN_GROUP       0
#define NC_CSYN_USER        1

/* ʵʱ���ݷ��ͻ�����   */
typedef struct ncsCltSendBuf_s {
    unsigned long  lMax;                  // ���ֽ���
    unsigned long  lPut;                  // ����λ��
    unsigned long  lSend;                 // ����λ��
    unsigned long  lSendTime;             // �����ʱ��
    unsigned long  lPutTime;              // ������󱣴�ʱ��
    long           lBytes;                // ��ǰδ���͵�����
    int            iLock;
    unsigned long  lLockTime;             // ��¼����ʱ��
} ncsCltSendBuf;

/* ���ֹ��˹���  */
typedef struct ncCltFilterIndex_s {
    unsigned long    lPos[2];       // ��ʼλ�� 0 ʵʱ���  1 ��־
    unsigned short   nLen[2];       // ��¼����
    unsigned short   nSum[2];       // ��¼����
    unsigned long    defsend[2];    // ȱʡ�Ƿ���
    unsigned long    enable[2];     // �����Ƿ�����
    char             *pBuf[2];      // ��Ӧ��ָ��          
} ncCltFilterIndex;


typedef struct ncCltMonIp_s {
    unsigned long  sip;
    unsigned long  eip;
    unsigned long  sport;
    unsigned long  dport;
    unsigned long  service;
    unsigned long  prot;
    unsigned long  flags;
} ncCltMonIp;

typedef struct ncCltMonWeb_s {
    char           url[64];
    unsigned long  urlid;
    unsigned long cond;
    unsigned long flags;
} ncCltMonWeb;

typedef struct ncCltMonMail_s {
    char            sender[64];
    unsigned long   sendid;
    char            toname[64];
    unsigned long   toid;
    char            subject[64];
    char            attname[64];
    unsigned long   msize;
    char            content[64];
    int             flags;
} ncCltMonMail;


typedef struct ncCltMonIm_s {
    unsigned long   service;
    char            uname[64];
    unsigned long   uid;
    unsigned long   fun;
    char            content[64];
    int             flags;
} ncCltMonIm;

typedef struct ncCltMonForm_s {
    char            url[64];
    unsigned long   urlid;
    char            varname[256];
    char            content[256];
    char            attname[64];    /* ��������  */
    int             asize;          /* ������С  */
    int             flags;
} ncCltMonForm;


typedef struct ncCltMonFtp_s {
    unsigned long   service;
    unsigned long   sip;
    unsigned long   eip;
    int             flags;
} ncCltMonFtp;

typedef struct ncCltMonBbs_s {
    char            url[64];
    unsigned long   urlid;
    char            bbsname[64];
    char            content[64];
    int             flags;
} ncCltMonBbs;


/* �ʼ����û�ID��ַ�б�  

 */
#define NCS_MAX_INFOLIST     4
#define NCS_INFO_MAILLIST    0
#define NCS_INFO_USERIDLIST  1

typedef struct ncCltInfoIndex_s {
    unsigned long lStart;
    unsigned long lSum;
} ncCltInfoIndex;

typedef struct ncCltInfoList_s {
    unsigned long lId;
    char      name[64];       /* �ʼ���ַ  */
} ncCltInfoList;

// ���������й����ݽṹ///////////////////////////

typedef struct ncCltCaseCtlH_s {
    unsigned long lFrom;
    unsigned long lCid;
    unsigned long lSid;
} ncCltCaseCtlH;

// �����û�����, ��Hash��
typedef struct ncCltCaseUser_s {
    ncCltCaseCtlH sCase;
    ncsCaseUser   sUser;
} ncCltCaseUser;


// ���������˺�, ��Hash��
typedef struct ncCltCaseNetId_s {
    ncCltCaseCtlH sCase;
    ncsCaseNetId  sNetId;
} ncCltCaseNetId;

// ����IP��ַ, һ������, ����С����
typedef struct ncCltCaseIp_s {
    ncCltCaseCtlH sCase;
    ncsCaseIp     sIp;
} ncCltCaseIp;

// ���ڷ�����ַ, ƥ��
typedef struct ncCltCaseWeb_s {
    ncCltCaseCtlH sCase;
    ncsCaseWeb    sWeb;
} ncCltCaseWeb;


// ���ڷ��ʱ�
typedef struct ncCltCasePost_s {
    ncCltCaseCtlH sCase;
    ncsCasePost   sPost;
} ncCltCasePost;


//  ��������
typedef struct ncCltCaseIm_s {
    ncCltCaseCtlH sCase;
    ncsCaseIm     sIm;
} ncCltCaseIm;

// �ʼ�����
typedef struct ncCltCaseMail_s {
    ncCltCaseCtlH sCase;
    ncsCaseMail   sMail;
} ncCltCaseMail;

// �ʼ�����
typedef struct ncCltCaseWarn_s {
    ncCltCaseCtlH sCase;
    ncsCaseWarn   sWarn;
} ncCltCaseWarn;

typedef struct ncCltCaseSens_s {
    ncCltCaseCtlH sCase;
    ncsCaseSens   sSens;
} ncCltCaseSens;


typedef struct ncCltCaseBuf_s {
    unsigned long  lSum[NCS_MAX_CASECONT];    /* ��¼��              */
    unsigned short nLen[NCS_MAX_CASECONT];    /* ÿ����¼�ĳ���      */
    unsigned long  lPos[NCS_MAX_CASECONT];    /* ����λ��            */
    unsigned long  lCur[NCS_MAX_CASECONT];    /* ��ǰλ��            */
    unsigned char  *pBuf;                     /* ÿ����¼����ʼָ��  */
} ncCltCaseBuf;

typedef struct ncCltCaseIndex_s {
    unsigned long lCfrom;
    unsigned long lCid;
    unsigned long lLastTime;
    int           iFlags;
} ncCltCaseIndex;

#endif
