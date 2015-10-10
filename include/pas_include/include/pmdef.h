#ifndef __PMDEF_DEF__
#define __PMDEF_DEF__        1
#define PM_LNK_ONLINE       31    /* �����û���Ϣ  */
#define PM_LNK_ALLUSER      32    /* ȫ���û�      */
#define PM_LNK_ADDRESS      33    /* ȫ����ַ      */
#define PM_LNK_GROUP        34    /* ȫ������      */
#define NC_LNK_PROCESS      35
#define NC_LNK_ONTIME       36
#define NC_LNK_ACCESSIP     37
#define PM_ACTION_ADMLOGIN    1
#define PM_ACTION_POPLOGIN    2
#define PM_ACTION_SMTPLOGIN   3
#define PM_ACTION_WEBLOGIN    4
#define PM_ACTION_SMTPSEND    5
#define PM_ACTION_SMTPRECV    6
#define PM_ACTION_MAXSEND     7    /* ��������  */
#define PM_ACTION_RECOVER     8    /* �������  */

#define PM_COND_EQUAL           1
#define PM_COND_INCLUDE         2
#define PM_COND_START           3
#define PM_COND_END             4   

#define PM_ACCESS_ALLOWED       1
#define PM_ACCESS_DENCY         0
#define PM_ACCESS_END           9

#define PM_PID_QUEUE          1
#define PM_PID_GSEND          2
#define PM_PID_POP            3
#define PM_PID_PROXY          4
#define PM_PID_ONTIME         5
#define PM_PID_CHKMAILSEND    6
#define PM_PID_VIRUS          7





/* Right �ֶ��У�ÿһλ 1--��ʾӵ�и���Ȩ��  0--��ʾû��Ȩ��   */
#define PM_ROLE_SYSTEM        0        /* ϵͳ����Ա   */
#define PM_ROLE_DOMAIN        1        /* �����Ա     */
#define PM_ROLE_GROUP         2        /* �����Ա     */
#define PM_ROLE_USER          3        /* һ���û�     */
#define PM_ROLE_QSEND         4        /* ����Ⱥ��     */


/* �������÷�Χ                         */
#define PM_OWNER_SYSTEM       0        /* ϵͳ   */
#define PM_OWNER_USER         1        /* ����   */

/* ������                             */
#define PM_DO_DELETE          1        /* ֱ��ɾ��   */
#define PM_DO_MOVE            2        /* �Ƶ�ָ���ļ���   */
#define PM_DO_FORWARD         4        /* ת��             */
#define PM_DO_REPLAY          8        /* �Զ�Ӧ��         */


/* �����û���Ϣ   
 */
typedef struct pmOnlineUser_s {
    char sid[24];
    char username[32];
    char maildir[63];
    char vhost[63];               /* ����          */
    int  groupid;                 /* ����          */
    char role[8];                 /* �û�Ȩ��      */
    unsigned char editmode;       /* ȱʡ�༭��ʽ 0--�ı� 1--Html */
    unsigned char maxatt;         /* ��󸽼���                   */
    unsigned char savecopy;       /* Save Copy      */
    unsigned char logmode;        /* 0--Web  1--Adm */
    unsigned char scanvirus;      /* 1 ��ʾ��ʱ���Զ���ⲡ��   0--�����        */
    unsigned char gsend;          /* 0--������Ⱥ��   1--����Ⱥ��        */
    unsigned char utype;          /* 0--��ͨ�û�   1--ϵͳ����Ա  2--�����Ա  */    
    unsigned char reverse[2];    
    unsigned long lMaxMail;       /* ����Ⱥ��������ʼ���   */
    unsigned long lMasSize;       /* ����޶�      */
    unsigned long lIp;            /* �ͻ���IP��ַ  */
    unsigned long lLastTime;      /* ������ʱ��  */
} pmOnlineUser;

/* �ļ����б�  */
typedef struct {
    unsigned long iFileNum;
    char     cname[32];
} pmFoldLst;


struct pmTimename
{
        time_t mtime;
        char name[64];
};

typedef  struct {
	unsigned int	num;		/* total num of mail in folder */
	unsigned int	news;		/* total num of new mail in folder */
	unsigned long	size;		/* total size of mail in folder */
} pmFoldInfo;

typedef struct pmScanList_s {
    char caName[32];          /* ����     */
    char caFile[128];          /* �ļ���   */
    struct pmScanList_s *next;
} pmScanList;

/* �û���Ϣ  */
typedef struct pmUserInfo_s {
    char   alias[32];
    char   passwd[32];
    int    groupid;
    unsigned long lMaxSize;
    char   vhost[40];
    char   userrole[20];
    char   maildir[100];
    char   address[64];         /* Ĭ�ϵ�ַ          */
    unsigned long lCount;       /* ��¼����          */ 
    unsigned long lLasttime;    /* ������ʱ��      */
} pmUserInfo;

/* ��ַ��Ϣ   */
typedef struct pmAddress_s {
    char  address[64];
    char  alias[32];
} pmAddress;

typedef struct pmGroup_s {
    long  groupid;
    char  groupname[20];
} pmGroup;

typedef struct ncProcessInfo_s {
    int iPid;
    unsigned long lStartTime;      /* ����ʱ��  */
    unsigned long lTimeOut;        /* ��ʱʱ��  */
    unsigned long lOntime;         /* ��ÿ��ָ��ʱ����������  */
    unsigned char caControl[32];   /* ������Ϣ  */
    char     caName[32];           /* ��������  */
    int   (*fFunName)(utShmHead *); /* ������   */
    int   iFlags;                   /* 0--��  1--�ڲ�����  2--�ⲿ  */
} ncProcessInfo;

typedef struct ncOnTimeFun_s {
    unsigned long lLastTime;            /* ���һ��ִ��ʱ��  */
    unsigned long lUsedTime;            /* ���һ��ִ��ʹ��ʱ��  */
    unsigned long lCount;               /* ִ�д���      */
    unsigned long lStepTime;            /* ���ʱ��      */
    unsigned long lOnTime;              /* ָ��ʱ�� hhmmss       */
    int      iStatus;                   /* 0--����  1--�ȴ� 2--����ִ��  */
    char     caName[32];                /* ��������      */
    int   (*fFunName)(utShmHead *);     /* ִ�к�������  */
} ncOnTimeFun;


/* ��֤����     */
/*  Include     pronetway.com       192.168.0.1  192.168.0.255   Allowed     */
/*  StartWith   lym                 192.168.0.1  192.168.0.255   Allowed     */
/*  EndOf       .com                192.168.1.0  192.168.1.255   Dency       */
/*  Equal       lym@pronetway.com   192.168.1.0  192.168.1.255   Dency       */
/*  Equal       any                 0.0.0.0     255.255.255.255 Allowed      */

typedef struct pmAccessIp_s {
    char     caDomain[64];             /* ����                                         */
    unsigned long lSip;                /* ��ʼIP��ַ                                   */
    unsigned long lEip;                /* ��ֹIP��ַ                                   */
    unsigned short  iCond;             /* ����  1-- Equal 2--Include  3--Start  4--End */
    unsigned short  iFlags;            /* 0--��ֹ  1--����   */
} pmAccessIp;

/* �ʼ���������                                           */
/*  From include lym To startwith pronetway.com  allowed  */
/*  From Equal   any to include   pronetway.com  allowed  */

typedef struct pmSmtpSend_s {
    char     caFrom[64];
    char     caTo[64];
    unsigned char     cFromCond;
    unsigned char     cToCond;
    unsigned short    nFlags;
} pmSmtpSend;
/* pasutl_vir.c */
int pmUtlSock(char *pPath);
int pmUtlKavStart(utShmHead *psShmHead);
int pmUtlKavTest(int iSock);
int pmUtlKavInfo(int iSock,char *pDate,char *pVersion);
int pmUtlKavKeyInfo(int iSock,char *pSn,char *pExpire);
int pmUtlKavScan(int iSock,char *pFile,int iOpt);
int pmUtlKavEnd(int iSock);
int pmUtlScanDir(int iSock,char *pDir,int iOpt);
int pmUtlScanMailDir(int iSock,char *pDir);
pmScanList *pmUtlKavScanMail(int iSock,char *pFile);
int pmUtlKavDoMail(char *caFile,pmScanList *psList,int iOpt,int iSender);
int pmUtlGetStr322(char *p,char *caName,char *caFile);
pmScanList *pmUtlAddScanList(pmScanList *psList,char *pName,char *pFile);
int pmUtlFreeScanList(pmScanList *psList);
int pmUtlClamDoMail(char *caFile,char *caVname,int iOpt,int iSender);
int pmUtlClamSock(char *pPath);
int pmUtlClamSend(char *pComm,char *pResult,int iMax);
int pmUtlClamTest();
int pmUtlClamInfo(char *pVersion,char *pDate);
int pmUtlClamReload();
int pmUtlClamScan(char *pFile,char *pOut);
int pmUtlClamScanStart();
int pmUtlClamScanEnd(int iSock);
int pmUtlClamScanOneFile(int iSock,char *pFile,char *pOut);
char *pmStrGetWord(char *p,char *pOut,
                   short nMaxLen,char *pTerm);
char *pmUtlGetLine(char *pBuf);
int pmStrIsSpaces(char *pIn);
int pmUtlReturnMail555(char *caFile);
char *pmUtlCvtVdate(char *pDate);
#endif
