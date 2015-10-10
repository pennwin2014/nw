#ifndef  ___DSSRV_DEF___
#define  ___DSSRV_DEF___
#define DS_MSG_CODE         990118101L
#define DS_DEFAULT_TCPPORT      8086
#define DS_DEFAULT_STATUSTIME   20
#define DS_DEFAULT_DSCLTTIMEOUT 60

#define DS_DEFAULT_MINDSCLIENTS  20    /* ��С�ͻ������� */
#define DS_DEFAULT_MAXDSCLIENTS  60    /* ���ͻ������� */
#define DS_DEFAULT_STEPCLIENTS   20    /* ÿ�����ӵ����� */
#define DS_DEFAULT_CHECKAPPTIME  15    /* ������Ϣȱʡ���ʱ�� */
#define DS_DEFAULT_KEEPAPPTIME   30    /* ����Ӧ��ȷʡ����ʱ�� */
#define DS_DEFAULT_MINDUSERS    255   /* Ŀ¼����֧�ֵ��û�����  */
#define DS_DEFAULT_MAXDUSERS    1024  /* ���ͻ������� */
#define DS_DEFAULT_STEPUSERS    100   /* ÿ�����ӵ����� */

#define PAS_SERVER_KEY   "pro56321123"


/* Ӧ�÷���������      */
#define AP_TYP_DSSRV                   1               /* Ŀ¼������     */
#define AP_TYP_PASSRV                  2               /* һ���PAS����  */
#define DSSRV_PASS_KEY                 "12pronetway"
/* Ӧ�÷������ڴ�ṹ  */
#define DS_MAX_ROLELEN                 128
#define DS_MESSAGE_OK                  0               /* ����*/
#define DS_ERROR_APPNOEXIST            100001L         /* ���������ֲ�����      */
#define DS_ERROR_KEYERR                100002L         /* �������              */
#define DS_ERROR_EXCEEDAPP             100103L         /* �������������������  */
#define DS_ERROR_MSGTIMEOUT            100104L         /* ��Ϣ��ʱ              */
#define DS_ERROR_DBERROR               200000L         /* �������������������  */

/* ������Ϣ  */

#define DS_ERROR_DATABASE_ERROR        1000000        /* ���ݿ����        */

#define DS_ERROR_LOGIN_PASSERR         1000101        /* �������          */
#define DS_ERROR_LOGIN_NOUSER          1000102        /* �û�������        */
#define DS_ERROR_LOGIN_MEMERR          1000103        /* �ڴ����          */

#define DS_ERROR_LOGIN_NODICT          1000102        /* �������ֵ�        */

#define DS_ERROR_RECADD_NONAME         1000101        /* name�ֶ�û�л�Ϊ��  */
#define DS_ERROR_RECADD_NOGROUP        1000102        /* group�ֶ�û�л�Ϊ�� */
#define DS_ERROR_RECADD_GROUPNOEXIST   1000103        /* group������         */


#define DS_ERROR_USRADD_USREXIST       1000102        /* �û��Ѿ�����      */
#define DS_ERROR_RECADD_FLSTERR        1000103        /* �ֶ����Ʋ���      */
#define DS_ERROR_USRADD_PASSWORD       1000104        /* ����Ϊ��          */
#define DS_ERROR_USRADD_LOGNAME        1000105        /* �û���Ϊ��        */
#define DS_ERROR_USRADD_DATABASE       1100000        /* ���ݿ����        */

#define DS_ERROR_GLIST_NODICT          1000201        /* �ֶ������������ֵ��в�����  */
#define DS_ERROR_GLIST_INVMSG          1000202        /* ��Ϣ���ݲ���ȷ    */

#define DS_ERROR_DICT_DSNOEXIST        2000101        /* �����ֵ䲻����    */

#define DS_ERROR_DICT_FATTRIB          2000101        /* �����ֵ����������ݲ���ȷ */

#define DS_ERROR_MSG_VARNOEXIST        2000102        /* ��Ϣ�еı���������  */
typedef  struct  {  /* �����ֵ�  */
    char            caDname[32];          /* �ֵ���ID     */
    char            caName[32];           /* �ֵ�����     */
    unsigned short  nStart;               /* ��ʼ���     */
    unsigned short  nSum;                 /* ����         */
    char            caRight[8];           /* ����Ȩ�� 0--ϵͳ����Ա 1--�����Ա  2--�Լ�  3--ͬ�� 4--����   */
} dsIndex;

typedef  struct  {  /* �����ֵ�����  */
    char            caDname[32];           /* �ֵ�ID     */
    char            caFname[32];           /* �ֶ�����   */
    char            cFlags;                /* 0--�̶�  1--����  2--��̬  */
    char            cDtype;                /* ��������   */
    unsigned short  nBytes;               /* ���ݳ���   */
    char            caRight[8];           /* ����Ȩ�� 0--ϵͳ����Ա 1--�����Ա  2--�Լ�  3--ͬ�� 4--����   */
} dsAttr;




typedef  struct  {  /* �ֶ�����  */
    char            caFname[32];           /* �ֶ�����   */
    char            cFlags;                /* 0--�ڲ� 1--�̶�  2--����  3--��̬  */
    char            cDtype;                /* ��������   */
    unsigned short  nBytes;                /* ���ݳ���   */
    char            caRight[8];            /* ����Ȩ�� 0--ϵͳ����Ա 1--�����Ա  2--�Լ�  3--ͬ�� 4--����   */
} dsFattr;


typedef union {
    dsIndex       psIndex;
    dsAttr        psAttr;
} dsDiction;
        

typedef  struct {  /* ����Ӧ��  */
    unsigned long lId;           /* �ͻ���ID               */
    unsigned long lGroupId;      /* ��ID                   */
    char     caName[32];         /* ����                   */
    char     caSid[24];          /* ��֤��Key              */
    unsigned long  lIp;          /* IP��ַ                 */
    unsigned short nTcpPort;     /* Tcp�˿ں�              */
    unsigned short nUdpPort;     /* UDP�˿ں�              */
    unsigned short nCltType;     /* �ͻ������� 1--Ds������ 2--PAS�û�  3--Web�û� */
    unsigned short nStatus;      /* 0--������  1--����     */
    unsigned short nLeavel;      /* �û�����               */
    unsigned short nTimeOut;     /* ��ʱʱ��               */
    unsigned long  lLoginTime;   /* ��½ʱ��               */
    unsigned long  lLastTime;    /* ���һ�β���ʱ��       */
} dsAppOnline;


typedef  struct {  /* ����Ӧ��  */
    unsigned long lId;           /* �ͻ���ID 0--����ֵ 1--�Ѿ�ɾ�� 2--��ʱռ��  */
    char     caGroup[32];        /* ����                   */
    char     caName[32];         /* ����                   */
    char     caSid[24];          /* ��֤��Key              */
    unsigned long  lIp;          /* IP��ַ                 */
    unsigned short nTcpPort;     /* Tcp�˿ں�              */
    unsigned short nUdpPort;     /* UDP�˿ں�              */
    unsigned short nCltType;     /* �ͻ������� 1--Ds������ 2--PAS�û�  3--Web�û� */
    unsigned short nStatus;      /* 0--������  1--����     */
    unsigned short nLevel;      /* �û�����               */
    unsigned short nTimeOut;     /* ��ʱʱ��               */
    unsigned long  lOnlineNum;  /* ��Ŀ¼�������ϵ�λ��   */
    unsigned long  lLoginTime;   /* ��½ʱ��               */
    unsigned long  lLastTime;    /* ���һ�β���ʱ��       */
} dsSrvUserInfo;



/* �û�Ȩ����Ϣ---���������ļ��Ķ���  */
typedef struct dsRightList_s {
    unsigned long  lRightId;                 /* Ȩ��ID   */
    char           caAlias[16];              /* Ȩ�ޱ��� */
    char           caDesc[34];               /* Ȩ������ */
    unsigned short nSumFun;                  /* �������� */
    unsigned long *lpFunList;
    struct dsRightList_s    *psNext;
} dsRightList;


typedef struct {
    unsigned long  lSumRight;                /* Ȩ������ */
    unsigned long  lSumBytes;                /* �ֽ����� */
    dsRightList    *psRight;
} dsRightHead;


/* �û�Ȩ����Ϣ ----�ڹ����ڴ��еĽṹ */
typedef struct dsShmRightList_s {
    unsigned long  lRightId;                 /* Ȩ��ID   */
    char           caAlias[16];              /* Ȩ�ޱ��� */
    char           caDesc[34];               /* Ȩ������ */
    unsigned short nSumFun;                  /* �������� */
    unsigned long  iFunStart;
} dsShmRightList;


typedef struct {
    unsigned long  lSumRight;                /* Ȩ������ */
    unsigned long  lSumBytes;                /* �ֽ����� */
} dsShmRightHead;

    

/* ��ɫ��Ϣ  */
typedef struct {
    unsigned long     lRoleId;              /* ��ɫID              */
    char              caRight[128];         /* ��ɫ��Ӧ��Ȩ��      */   
} dsRoleInfo;


/* dssrv001.c */
int dsSrvInit000(utShmHead *psShmHead);
int dsSrvTcpInit(utShmHead *psShmHead);
int dsSrvUdpInit(utShmHead *psShmHead);

/* dssrv002.c */
int dsSrvAppLogin(utShmHead *psShmHead, 
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int dsSrvAppStatus(utShmHead *psShmHead, 
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead);            
char *dsSrvCreateSid();


/* dssrv003.c   �й������ֵ�Ĳ���  */
int dsSrvSyncCltRight(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvDicList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvDictModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvDictAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvDictDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* dssrv004.c  �й��û���Ĳ���    */
int dsSrvGroupList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvGroupModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvGroupAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvGroupDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
dsFattr *dsDictCvtFattr(utShmHead *psShmHead,char *caFlist,char *pName,int *iSum);
dsFattr *dsDictAttr(utShmHead *psShmHead,char *pName,int *iSum);
long  dsUtlGetGroupId(char *pGname);
int dsSrvSeque(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* dssrv005.c  �й��û��Ĳ���      */
int dsSrvUserList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvUserModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvUserAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvUserDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* dssrv006.c �йط������Ĳ���  */
int dsSrvServerList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvServerModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvServerAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvServerDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* dssrv007.c �йؼ�¼�Ĳ���  */
int dsSrvRecordList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvRecordModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvRecordAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvRecordDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvRecordDelWhere(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvRecordAddSome(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* dssrv008.c �й�SQL�Ĳ���  */
int dsSrvSqlRecordSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvSqlExec(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSrvSqlExecSome(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* dssrv009.c �û���½  */
int dsSrvUserLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
char *dsSrvGenSid();
dsSrvUserInfo *dsSrvGetUserInfo(utShmHead *psShmHead,char *pSid,int *iNum);
dsSrvUserInfo *dsSrvGetFreeUserInfo(utShmHead *psShmHead,int *iNum);
int dsSrvUserLoginConfirm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
dsSrvUserInfo *dsSrvGetUserInfoByNum(utShmHead *psShmHead,int iNum,char *pSid);
dsSrvUserInfo *dsSrvGetUserInfoByName(utShmHead *psShmHead,char *caName,char *caIp,int *iNum);
#endif
