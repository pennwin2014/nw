#ifndef  ___PASC_DEF___
#define  ___PASC_DEF___ 
#define PASC_FUN_HEAT            11    /* ��������  */
#define PASC_FUN_ERROR           12    /* ����      */
#define PASC_FUN_NEWHEAT         13    /* �µ���������  */
#define PASC_ERR_CONNECT         101   /* ���ݿ����ӳ���  */
#define PASC_ERR_NORESP          102   /* û��Ӧ��        */
#define PASC_FLAGS_NOKEEP  0     /* ����Ҫ����    */
#define PASC_FLAGS_KEEP    1     /* ��Ҫ��������  */
#define PASC_FLAGS_ACTIVE  2     /* �������ڻ  */
#define PASC_STATUS_BUSY   1     /* ����æ        */
#define PASC_STATUS_FREE   0     /* ����æ        */
#define PASC_STATUS_ERROR  9     /* ���ӳ���      */
#define PAS_MAX_PASSERVER  8
typedef struct pasIpAddress_s {
    unsigned long    lIp;
    unsigned short   nPort;
    unsigned char    cFlags;
    unsigned char    cRev;
} pasIpAddress;


/* ����Tcp���� Msg->proxy */
typedef struct pasSLocal_s {
    int iFd;                    /* Sock Id             */
    int iLocalSock;             /* ����SockId          */
    unsigned long  lIp;         /* ����IP��ַ         */
    unsigned short nPort;       /* �˿ں�             */
    unsigned char  cFlags; 
    unsigned char  cStatus;      /* ״̬               */
    unsigned long  lPid;         /* PasId, ��Ϊ0 ����  */
    unsigned long  lLastTime;    /* ������ʱ��       */
    char           caKey[32];    /* ����Key            */
    utMsgHead      *psMsgHead;   /* ������Ϣ           */
} pasSLocal;


typedef struct pasSrvInfo_s {
    int iMaxThread;                   /* ����߳�����  */
    int iSumThread;                   /* ��ǰ�߳���    */
    int iMaxTcp;
    int iThisMaxTcp;                  /* ��ǰ�õ������Tcp������ */
    unsigned long  lLastTime;         /* ������ʱ��  */
    unsigned long  lStartTime;        /* ����ʱ�� */
    char     caPort[64];
    pasSLocal  *psLocal;
    int     iTcpSock[PAS_MAX_TCPPORT];                     /* Proxy�˿�        */
    int (*_pasSGetOnlineInfo)(utShmHead *,unsigned long ,char *);     /* ȡ��������Ϣ  */
    int (*_pasSSetOnlineInfo)(utShmHead *,unsigned long);             /* ����������Ϣ  */
    utShmHead  *psShmHead;
} pasSrvInfo;

typedef struct pasSrvThread_s {
    pasSrvInfo *psSrv;
    int        iFd;
} pasSrvThread;


/* Pas proxy�ͻ�����Ϣ  */

typedef struct pasCSrvList_s {
    unsigned long  lPid;           /* �ͻ���ID         */
    unsigned long  lIp;            /* Ip��ַ, ����˳��  */
    unsigned short nPort;          /* �˿�, ����˳��    */
    unsigned short nStatus;        /* ״̬              */
    int            iSock;          /* ����Sock          */
    unsigned long  lLastTime;      /* �������ʱ��      */
    char           caKey[32];      /* ͨ����Կ          */
} pasCSrvList;

/* �ͻ��˽�����Ϣ   */
typedef struct pasCltProcess_s {
    unsigned long iPid;        // ����Id
    unsigned long lStartTime;  // ʱ��
    unsigned long lLastTime;   // ������ʱ��
} pasCltProcess;


typedef struct pasCltInfo_s {
    unsigned long  lLocalIp;    /* ����Pas��ַ  */
    unsigned short nLocalPort;  /* ���ض˿�     */
    unsigned short nSumServer;  /* ����������  */
    unsigned short nMaxServer;  
    unsigned short nMaxThread;
    unsigned short nSumThread;
    unsigned short nCurServer;
    unsigned long  lTimeStep;      /* ��������  */
    unsigned long  lTimeOut;       /* ��ʱ      */
    unsigned long  lStartTime;
    unsigned long  lLastTime;
    pasCSrvList    *psSrv;     /* �������б�  */
    pasCltProcess  *psProcess;
} pasCltInfo;

typedef struct pasCltThread_s {
    pasCltInfo  *psClt;
    pasCSrvList *psSrv;
    int        iFd;
} pasCltThread;

typedef struct pasConfig_s {
    char caVar[32];
    char caValue[128];
    struct pasConfig_s *next;
} pasConfig;
    

/* pasproxy.c */
int pasProxyServer(utShmHead *psShmHead,
                    int _pasSGetOnlineInfo(utShmHead *,unsigned long ,char *),
                    int _pasSSetOnlineInfo(utShmHead *,unsigned long));
int pasProxySrv(utShmHead *psShmHead,int *iTcpSock,int _pasSGetOnlineInfo(utShmHead *,unsigned long ,char *),
    int _pasSSetOnlineInfo(utShmHead *,unsigned long));
int pasProxySendMsg(unsigned long lIpAddress,unsigned short nTcpPort,
                unsigned long lFunCode,unsigned long lSid,unsigned long lRid);
int pasProxySendMsgToSock(int iSock,unsigned long lFunCode,unsigned long lSid,unsigned long lRid,int iCode);
int _pasCCvtIpPort(char *caSip,unsigned long *_lSip,unsigned short *_nSport);
/* pasproxy_c.c */
pasCltInfo *_pasProxyCltInit(utShmHead *psShmHead);
int pasProxyClt(utShmHead *psShmHead,int iFlags);
int pasProxyCltProcessIsFree(pasCltInfo *psClt,pasCltProcess *psProcess,int iTimeOut);
int pasProxyDOServerMsg(utMsgHead *psMsgHead,int iFd,pasCSrvList *psSrv,pasCltInfo *psClt);
int pasProxySetServerIp(utShmHead *psShmHead,unsigned long lIp,unsigned short nPort,unsigned long lProId,char *pKey);
pasConfig *pasUtlLoadConfig(char *pFile);
int pasUtlConfigFree(pasConfig *ps);
char *pasUtlGetConfVarValue(pasConfig *ps,char *pVar,char *pDef);
char *pasUtlGetConfVarValueNum(pasConfig *ps,int n,char *pVar,char *pDef);
unsigned long pasUtlGetConfVarValue_l(pasConfig *ps,char *pVar,unsigned long lValue);
int pasProxySetSynApi(int (*fFunName)(utShmHead *));
    
#endif