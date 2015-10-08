/* ģʽƥ��   
     2007/09/09
 */
#ifndef __NCCONTMATCHDEF___
#define __NCCONTMATCHDEF___
#define NC_MATCH_MAXVAR     512
#define NC_MATCHIP_ALL        0
#define NC_MATCHIP_TCP        6
#define NC_MATCHIP_UDP       17

#define NC_CHECKLEVEL_IPORCONT   1
#define NC_CHECKLEVEL_IPANDCONT  2
#define NC_CHECKLEVEL_IPONLY     3
#define NC_CHECKLEVEL_CONTONLY   4

// Tcp ����
typedef struct ncMatchIpConnect_s {
    unsigned long  lSip;              /* �ڲ�IP��ַ                         */
    unsigned long  lDip;              /* �ⲿIP��ַ                         */
    unsigned short nDport;            /* Ŀ��˿ں�                         */
    unsigned short nSport;            /* Դ�˿ں�                           */
    unsigned char  cProt;             /* Э��   6-TCP  17 UDP               */
    unsigned char  cConnect;          /* ����״̬  1 Syn  2 ����  9 Close   */
    unsigned char  cDiction;          /* ���� 0 ����   1 ����               */
    unsigned char  cStatus;           /*      0 δ֪   1 ����      2 ����   */
    unsigned long  lStartTime;        /* ��ʼʱ��                           */
    unsigned long  lLastTime;         /* ����޸�ʱ��                       */
    unsigned char  cFlags;            /* ���ھ���Э���еı�ʶ               */
    unsigned char  cProxy;            /* 0 -- ����   1 -- ����Proxy��Э��   */
    unsigned char  cRev[2];
    unsigned long  lSeq;              /* �������к�                          */
    unsigned long  lAck_seq;          /* �������к�                          */
    unsigned long  lSid;              /* Session ID                          */
    unsigned long  lServiceId;        /* ����ID                              */
    unsigned long  lAction;           /* ����ID                              */
    unsigned long  lRight;            /* Ȩֵ                                */
} ncMatchIpConnect;

typedef struct ncWordCont_s {
    int      iLen;
    unsigned char     *pData;
    struct ncWordCont_s *next;
} ncWordCont;

// ƥ��һ����
typedef struct ncMatchWord_s {
    unsigned short nOffset;          // ƫ����
    unsigned char  cOrder;           // 0--��ͷ��ʼ   1--�ӽ�β��ʼ   2--�ӵ�ǰλ��   3--����
    unsigned char  cFlags;           // ƥ�䷽ʽ      0--��ȷ         1--֧��ͨ���( *--����  ?--һ��λ��)   
    unsigned short nSumWord;         // ����
    unsigned short nRev;
    ncWordCont     *psCont;          // ��������
    struct ncMatchWord_s *next;      // �������һ��
} ncMatchWord;

// ƥ��һ�����ݰ�
#define NC_LENTYPE_NOCHECK       0
#define NC_LENTYPE_LE            1
#define NC_LENTYPE_EQ            2
#define NC_LENTYPE_GE            3

typedef struct ncMatchPkg_s {
    unsigned char  cPkgLenType;       // 0-- �����    1--С�ڵ���  2--����  3--���ڵ���
    unsigned char  cOrder;            // 0-- ˳���޹�  1--������˳��
    unsigned char  cThisRight;        // Ȩֵ
    unsigned char  cRev;
    unsigned short nPkgLen[3];
    int  (*fCheckFun)(ncMatchIpConnect *psTcp,unsigned char *pkg,int iLen,unsigned char *pPar); 
    unsigned char caPar[16];
    ncMatchWord  *psWord;
    struct ncMatchPkg_s *next;
} ncMatchPkg;


typedef struct ncMatchAction_s {
    char                caActionName[16];
    unsigned short      nActionId;
    unsigned char       cActionRight;
    unsigned char       cThisRight;
    ncMatchPkg          *psPkg;
    struct ncMatchAction_s *next;
} ncMatchAction;

typedef struct ncMatchService_s {
    char                    caServiceName[16];       // ��������
    unsigned long           lServiceId;
    int                     iCheckLevel;             // ��鼶��
    ncMatchAction           *psAction;
    struct ncMatchService_s *next;
} ncMatchService;

/* ������  */
typedef struct ncMatchFun_s {
    char       caFunName[32];
    int  (*fCheckFun)(ncMatchIpConnect *psTcp,unsigned char *pkg,int iLen,unsigned char *pPar); 
    struct ncMatchFun_s *next;
} ncMatchFun;


/* ƥ����   */
typedef struct ncMatchRet_s {
    ncMatchService      *psService;
    ncMatchAction       *psAction;
    unsigned char       cRight;
    unsigned char       cRev[3];
    struct ncMatchRet_s *next;
}ncMatchRet;

typedef struct ncMatchIp_s {
    unsigned long  lIp;
    unsigned short nPort;
    unsigned char  cProt;
    unsigned char  cMask;
    unsigned long  lServiceId;
}ncMatchIp;


#define NC_MATCH_PUBLIC     1
#define NC_MATCH_ACTION     2
#define NC_MATCH_SERVICE    3

typedef struct ncMatchVar_s {
    int   iVarType;                /* ���� 1--������Ϣ  2--������Ϣ  3--������Ϣ */
    char  caVarName[16];           /* ������                                     */
    char  caVarValue[64];          /* ֵ                                         */
} ncMatchVar;


typedef struct ncMatchVarBuf_s {
    char  caVarName[16];           /* ������                                     */
    char  *pValue;                 /* ֵ                                         */
    struct ncMatchVarBuf_s *next;
} ncMatchVarBuf;


typedef struct ncMatchVarHead_s {
    ncMatchVarBuf *first;
    ncMatchVarBuf *last;
    int           iSum;
} ncMatchVarHead;


typedef struct ncMatchHead_s {
    int             iStatus;      /* ����״̬  0--ԭʼ   1--�Ż�  */
    ncMatchService *psService;
    ncMatchFun     *psFun;
    unsigned char  *pTcp;         /* Tcp ������Ϣ,Hash��          */
    unsigned char  *pIplist;      /* Ip �б�                      */
    unsigned char  *pVar;         /* ������Ϣ                     */
    unsigned char  cMask[32];     /* ��������                     */
    int            iSumMask;      /* ��������                     */
} ncMatchHead;


#endif
