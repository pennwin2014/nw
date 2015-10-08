#ifndef __PronetwayFireDef__
#define __PronetwayFireDef__


#define FIRE_TYPE_DEFAULT    0
#define FIRE_TYPE_FORWARD    1
#define FIRE_TYPE_INPUT      2
#define FIRE_TYPE_OUTPUT     3
#define FIRE_TYPE_IPTABLES   4
#define FIRE_TYPE_RULE       5
#define FIRE_TYPE_PLATE      6
#define FIRE_TYPE_DEFINE     7
#define FIRE_TYPE_SNAT       8
#define FIRE_TYPE_NATIP      9
#define FIRE_TYPE_NATPORT    10
#define FIRE_TYPE_SYSTEM     11

#define FIRE_JUMP_ACCEPT     0
#define FIRE_JUMP_DROP       1
#define FIRE_JUMP_LOG        2

#define FIRE_PROT_ALL        0
#define FIRE_PROT_TCP        1
#define FIRE_PROT_UDP        2
#define FIRE_PROT_ICMP       3

#define FIRE_ID_DELETE       0    /* ɾ��  */
#define FIRE_ID_SELECTIP     1
#define FIRE_ID_INPUTIP      2
#define FIRE_ID_NAT          3
#define FIRE_ID_RULE         4

/* IP Table����  */
typedef struct fireIptables_s {
    int      iType;            /* ����  0--Define   1--Default
                                        2--Forward  3--Input
                                        4--Output   5--Rule
                                        6--Plate
                                */
    unsigned long lNum;        /* ���                      */
    unsigned long lId;         /* ID  id=0 �ѱ�ɾ�� 1--���� 2--�߼�  3--NAT  4--Rule        */
    char     caName[32];       /* ����             */
    char     caCommand[512];   /* Ip Tables������  */
} fireIptables;

typedef struct FireSummary_s {
    int  iDefine;
    int  iSumTables;
    int  iMaxTables;
    int  iPlate;
    int  iForward;     /* Forward��ȱʡ���� */
    int  iInput;       /* Input��ȱʡ����   */
    int  iOutput;      /* Output��ȱʡ����  */
    int  iModify;
    int  iAutoStart;
    char *pDefine;     /* ����������      */
    fireIptables *psTables;
} fireSummary;

typedef struct fireNetList_s {
    char     caDes[64];             /* ����  */
    unsigned long lIp;
    unsigned long lMask;
    struct fireNetList_s *psNext;
} fireNetList;


typedef struct fireNetDev_s {
    char     caDev[32];             /* �ӿ�����  */
    unsigned long lIp;              /* IP��ַ    */
    char     caDes[64];             /* ����      */
    fireNetList  *psNetList;
} fireNetDev;

/* ���֡�ֵ  */
typedef struct fireVarValue_s {     
    char caVar[32];
    char *pValue;
    struct fireVarValue_s *psNext;
} fireVarValue;

    
/* ����  */
typedef struct fireParam_s {
    char          caName[32];         /* ��������  */
    fireVarValue  *psParm;            /* ��������  */
} fireParm;
   

typedef struct fireDefault_s {
    char    caChain[32];     /* Chain Name */
    char    caPolicy[32];   /* ����       */
} fireDefault;

/* ת�������ݰ�  */
typedef struct fireForward_s {
    unsigned long lId;    /* ID        */
    char    caSip[64];    /* Դ��ַ    */
    char    caDip[64];    /* Ŀ���ַ  */
    char    caSport[32];  /* Դ�˿�    */
    char    caDport[32];  /* Ŀ��˿�  */
    char    caProtocol[32];     /* Э��  */
    char    caJump[32];         /* ��ת  */
    char    caStatus[64];       /* ״̬  */
    char    caLimit[64];        /* ���ư�������  */
    char    caBurst[64];        /* ������ֵ   */
    char    caFragment[64];     /* ��Ƭ   */
    char    caIndev[64];        /* �����豸  */
    char    caOutdev[64];       /* ����豸  */
    char    caSyn[64];          /*        */
    char    caLog[64];          /* ��־   */
} fireForward;



typedef struct fireDefine_s {
    char caName[32];
    char caDispname[32];
    char caValues[128];
} fireDefine;

/* ������Ϣ  */
struct fireErrormsg_s {
    int     iIndex;                /* ������    */
    char    *pMsg;                 /* ������Ϣ  */
    struct fireErrormsg_s *psNext;    
};


typedef struct  fireError_s {
    int iSum;                      /* ��������  */
    struct fireErrormsg_s *psHead;
    struct fireErrormsg_s *psEnd;
} fireError;



/* fire001.c */
int fireWebDispDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebListBaseInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebModiDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebRuleEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebRunScript(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebUnRunScript(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebSnatEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebNatipEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int fireWebNatportEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
#endif
