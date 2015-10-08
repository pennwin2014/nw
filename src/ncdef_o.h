#ifndef __NETCHARGDEF__
#define __NETCHARGDEF__
#define   NC_IPCONTROL_ID       918123123L
#define   NC_MAX_PROCESS        16   /* ����������  */
#define   NC_MAX_CONTROL        32   /* �����Ʊ���  */
#define   NC_MAX_ONTIMEDO       16   /* ���ʱ����  */
#define   NC_DEFAULT_IP_CHECK   20
#define   NC_DEFAULT_IP_TIMEOUT 30
#define   NC_MAX_USERNUM      257
#define   NC_MAX_IPPKG        9999
#define   NC_MAX_HTTPNUM      999
#define   NC_MAX_LIMITED      100
#define   NC_MAX_MONITOR      100
#define   NC_MAX_INTERNAL     100
#define   NC_MAX_LASTIP       1000
#define   NC_CONTENT_GET      10
#define   NC_CONTENT_POST1    11
#define   NC_CONTENT_POST2    12
#define   NC_CONTENT_POST3    13
#define   NC_CONTENT_SMTP1    21
#define   NC_CONTENT_SMTP2    22
#define   NC_CONTENT_POP3     23
#define NC_PF_UNUSED       0
#define NC_PF_INUSED       1
#define NC_LNK_THIRDAUTH  25  /* ��������֤���û�  */
#define NC_LNK_ALLUSER    26  /* ȫ���û���Ϣ      */
#define NC_LNK_MAILCTL    27  /* �ʼ�����          */
#define NC_LNK_CONTROL    28  /* ���ʿ���          */
#define NC_LNK_IPFLOW     29  /* ������            */
#define NC_LNK_MAIL       30  /* �ʼ����ݰ�        */
#define NC_LNK_IPPKG      31  /* IP���ݰ�          */
#define NC_LNK_WEB        32  /* http���ݰ�        */
#define NC_LNK_LIMIT      33  /* ����ʹ�õ�IP��ַ  */
#define NC_LNK_NETWORK    34  /* Ҫ��ص�IP��ַ  Add 2002/9/6 By Liyunming Replace 
                                          NC_LNK_MONITOR NC_LNK_INTERNAL */

/* #define NC_LNK_MONITOR    34 
   #define NC_LNK_INTERNAL   35 
      Delete 2002/9/6 Replace by NC_LNK_NETWORK
 */
#define NC_IPCONTROL_BY_ICMP      0
#define NC_IPCONTROL_BY_IPTABLES  1

#define NC_LNK_RELOADCONTROL 35 /* ����װ�ؿ���  */
#define NC_LNK_USERINFO   36  /* �û���Ϣ          */
#define NC_LNK_SERVICE    37  /* ������֪�Ķ˿�    */
#define NC_LNK_LASTIPNUM  38  /* �����ʵ�1000��IP���ݰ� 0--��ǰ���� 1--IP���� 2--Http����  */
#define NC_LNK_WEBLIMIT   39  /* Web��������       */

#define NC_SERVICE_OTHER  0    /* ��������           */
#define NC_SERVICE_TELNET 1    /* Telnet ����        */
#define NC_SERVICE_FTP    2    /* Ftp    ����        */
#define NC_SERVICE_HTTP   3    /* Web����            */
#define NC_SERVICE_HTTP1  300  /* Web����            */
#define NC_SERVICE_SMTP   4    /* SMTP����           */
#define NC_SERVICE_POP    5    /* POP����            */
#define NC_SERVICE_ICQ    6    /* ICQ                */
#define NC_SERVICE_SSH    7    /* ssh                */
#define NC_SERVICE_IRC    8    /* IRC                */

#define NC_PROTOCOL_ICMP  1
#define NC_PROTOCOL_IGMP  2
#define NC_PROTOCOL_TCP   6
#define NC_PROTOCOL_UDP   17

/*   Define the Debug Level   */
#define NC_DEBUG_SHOWTCP      1
#define NC_DEBUG_SHOWCONTROL  2
#define NC_DEBUG_SHOWUDP      3
#define NC_DEBUG_SHOWICMP     4
#define NC_DEBUG_SHOWIGMP     5
#define NC_DEBUG_SHOWCHECKMAC 6
#define NC_DEBUG_SHOWSMTP     7
#define NC_DEBUG_SHOWPOP      8
#define NC_DEBUG_NTAUTH       9
#define NC_DEBUG_UPLOAD       10
#define NC_DEBUG_IPCONTROL    11
#define NC_DEBUG_MAILAUDIT    12     /* �ʼ����  */
#define NC_DEBUG_MAILNON      13     /* �ʼ����  */
#define NC_DEBUG_TIMECTL      14     /* ʱ�����  */
#define NC_DEBUG_MSGINFO      15     /* ��ʾ���ݰ�   */
#define NC_DEBUG_CHECKNETIP   16     /* ���������� */
#define NC_DEBUG_THIRDAUTH    17     /* ��������֤   */

/*���cStatus���� */
#define NC_USER_NOUSE      0        /* Nouse    */
#define NC_USER_NORMAL     1        /* Normal   */
#define NC_USER_TIMEOUT    2        /* �û���ʱ */
#define NC_USER_NOEXIST    3        /* �û�������  */
#define NC_USER_MACERROR   4        /* MAC��ַ��һ��  */
#define NC_USER_FORBID     7        /* ����           */

typedef struct pasSearchHead_s {
    char caFname[32];        /* �ֶ���               */
    char caDispname[32];     /* ��ʾ��               */
    int  iSize;
    int  iMaxLen;
    int  iType;              /* ��������  1-�ı�  2-ѡ���  */
    char *pValue;
} pasSearchHead;

/* �ڴ��е��û���Ϣ  */
typedef struct ncUserBuf_s {       /* �û���Ϣ       */
    long     lUserid;              /* �û�Id  0--û��         */
    char     caUsername[16];       /* ����           */
    char     caDispname[16];       /* ��ʾ��         */
} ncUserBuf;

/* ��������֤���û���Ϣ  */
typedef struct ncThirdAuth_s {
    char          caUsername[16];    /* �û���                */
    int           iIndex;            /* ���û���Ϣ�е�������  */
    unsigned long lIp;               /* IP��ַ 0 --��ʾû������ */
    unsigned long lLastTime;         /* ������ʱ��          */
} ncThirdAuth;


typedef struct ncUserInfo_s {      /* �û���Ϣ       */
    char     caGroup[16];          /* ����           */
    char     caName[16];           /* ����           */
    char     caDispname[16];       /* ��ʾ��         */
    long     lId;                  /* �û�Id  0--û��         */
    unsigned long  lGid;           /* ��Id         */
    unsigned char  cUseflags;      /* ʹ��״̬0--���� 1--����  9--����            */
    unsigned char  cStatus;        /* 0--û�� 1--����  2--�û�������  3--MAC��һ��  9--ɾ�� */
    unsigned char  mac[6];         /* MAC��ַ        */
    unsigned long  lIp;            /* 0 ��ʾû���û� */
    unsigned long  lStartTime;     /* ��ʼ����ʱ��   */
    unsigned long  lLastTime;      /* ������ʱ��   */
    short    nWeb;                 /* ��վ���Ʒ���   */
    short    nLimit;               /* IP���Ʒ���     */
    unsigned long lDo;             /* ���ڷ��ʵ����� */
    long8    lBytes[2];            /* 0-�ܵ��������� 1--�������� ������ʱ�俪ʼ����  */
    unsigned long lConntime;       /* ����ʱ��, �ӼƵ����ݿ⵽��ǰ��ʱ��             */
    unsigned long lSumtime;        /* �ӿ�ʼ��ʱ����ǰʱ������                       */
    long8    lSumflow;             /* �ӿ�ʼ��ʱ����ǰ��������                       */
    unsigned short nGroup;         /* ��Group�е����   */
    unsigned short nReserv;        /* ����  */
    unsigned long  lReserv;      /* ����                                           */
    unsigned long lRandWidth;      /* ��������       */
    unsigned long lRandLower;      /* ��������       */
    long8    lLimitBytes;          /* �������� 0--������   */
    unsigned long lLimitTimes;     /* ����ʱ�� 0--������   */
} ncUserInfo;



/* �û�ͬ����Ϣ   */
typedef struct ncNtUser_s {        /* NT�û���Ϣ     */
    char          caName[32];      /* ����           */
    unsigned long lIp;             /* IP��ַ         */
    int           iStatus;        /* 1--��½  0--�˳�  */
} ncNtUser;


/* ��Ľṹ��Ϣ  */
typedef struct ncDbField_s {
    int  iFlags;
    char caField[32];
    char caType[32];
    char caNull[32];
  	char caKey[32];
  	char caDefault[64];
  	char caExtra[32];
} ncDbField;

typedef struct ncDbIndex_s {
    int  iFlags;
    char caIndex[32];
    char caUnique[4];    /* 0-- Unique  1-- No */
    char caSeq[4];       /* ���               */
    char caField[32];
} ncDbIndex;


  	
#define NC_NETWORK_NOMAL          0    /* ��������    */
#define NC_NETWORK_INTERNET       1    /* Internet��  */
#define NC_NETWORK_IPBASE         2    /* ����IP��ַ  */
#define NC_NETWORK_MACBASE        3    /* ����MAC��ַ */
#define NC_NETWORK_AUTH           4    /* ������֤    */
#define NC_NETWORK_NTAUTH         5    /* ��������֤  */
#define NC_NETWORK_NOMON          6    /* ����      */
#define NC_NETWORK_FORBID         7    /* ����        */

typedef struct ncIpNetwork_s {      /* �ڲ�IP��ַ    */
    unsigned long   lIpLower;       /* ��ʼIP��ַ    */
    unsigned long   lIpUpper;       /* ��ֹIP��ַ    */
    unsigned long   lAuthIp;        /* ��֤������IP��ַ  */
    unsigned short  nAuthPort;      /* ��֤�������˿ں�  */
    unsigned short  iFlags;         /* ���궨��      */
} ncIpNetwork;

/* Ip��ͷ����Ϣ  */
typedef struct ncIpHead_s {
	unsigned long  lSip;       /* ԴIP��ַ     */
	unsigned long  lDip;       /* Ŀ��IP��ַ   */
	unsigned short nDport;     /* Ŀ��˿�     */
	unsigned short nSport;     /* Դ�˿�       */
	char     caSmac[6];        /* ԴMAC��ַ    */
	char     caDmac[6];        /* Ŀ��MAC��ַ  */
	short    cTran;            /* ���ͷ��� 0--���� 1--����    */
	short    cProt;            /* Э��  6 UDP      17 TCP     */
} ncIpHead;

/*   Delete 2002/9/6  Replace with ncIpNetwork 
typedef struct ncIpInternal_s {      
    unsigned long   lIpLower;          
    unsigned long   lIpUpper;    
} ncIpInternal;
    

typedef struct ncIpMonitor_s {   
    unsigned long   lUid;        
    unsigned long   lIpLower;    
    unsigned long   lIpUpper;    
    unsigned short  nTranType;   
    unsigned short  nPort;       
} ncIpMonitor;

*/

/* ���Ʒ���  */
typedef struct ncIpLimited_s {       /* ���Ʒ��ʵ�Ip��ַ  */
    unsigned long   lUid;              /* �û�Id  0 ��ʾȫ��  */
    unsigned long   lIpLower;          /* ����  */
    unsigned long   lIpUpper;          /* ����  */
    unsigned char   cTranType;         /* ��������  0--ȫ��   1--UDP  2--TCP          */
    unsigned char   cTimeType;         /* ʱ������                                    */
    unsigned short  nLowerport;        /* �˿ں�����                                  */
    unsigned short  nUpperport;        /* �˿ں�����                                  */
    unsigned char   cService;          /* ��������                                    */
    unsigned char   cFlags;            /* ���Ʒ�ʽ 1--�ⲿ��  2--����  3--IP��ַ�Ͷ˿� */
    unsigned long   lStart;            /* ��ʼʱ�� HHMMSS                             */
    unsigned long   lEnd;              /* ��ֹʱ�� HHMMSS                             */
    unsigned char   cVister;           /* 0 ��ֹ   1 ����                             */
    unsigned char   cReserve[3];       /* ����  */
} ncIpLimited;

typedef struct ncWebLimited_s {       /* ���Ʒ��ʵ���վ     */
    unsigned long   lUid;              /* �û�Id  0 ��ʾȫ��  */
    char            caUrl[64];         /* ��վ��ַ            */
    unsigned long   lStart;            /* ��ʼʱ�� HHMMSS                             */
    unsigned long   lEnd;              /* ��ֹʱ�� HHMMSS                             */
    unsigned char   cTimeType;         /* ʱ������                                    */
    unsigned char   cVister;           /* 0  ��ֹ  1 ����                             */
    unsigned char   cReserve[2];       /* ����  */
} ncWebLimited;


typedef struct ncTcpBuffer_s {
    unsigned long  lSip;             /* ԴIP��ַ     */
    unsigned long  lDip;             /* Ŀ��IP��ַ   */
    unsigned short nSport;           /* Դ�˿ں�     */
    unsigned short nDport;           /* Ŀ��˿ں�   */
    unsigned long  lStartTime;       /* ��ʼʱ��     */
    unsigned long  lLastTime;        /* ����޸�ʱ�� */
    long8          lBytes[2];        /* 0-�����ֽ���  1-�����ֽ��� */
    unsigned char  cType;            /* Э��         */
    unsigned char  cService;         /* ����         */
    unsigned short nUrl;             /* ����http     */
    unsigned char  caMac[6];         /* ԴMac��ַ    */
    unsigned char  cUseFlags;        /* ʹ�ñ�ʶ 0--û���� 1--������ 2--�Ѵ��� 3--�ѱ���   */
    unsigned char  cFlags;           /* ��ʶ 0--���� 1--����  */
    unsigned long  lCount;           /* ������                */
} ncTcpBuffer;

typedef struct ncHttpBuffer_s {
    unsigned long  lSip;             /* ԴIP��ַ     */
    unsigned long  lStartTime;       /* ��ʼʱ��     */
    unsigned long  lLastTime;        /* ��ʼʱ��     */
    long8          lBytes;           /* ����         */
    unsigned char  caMac[6];         /* ԴMac��ַ    */
    unsigned char  cUseFlags;        /* ��ʶ��0--������ 1--������  3--�ѱ���  */
    unsigned char  cReserve;
    char  caHost[64];
    char  caUrl[128];
} ncHttpBuffer;

typedef struct ncService_s {
    char            caName[32];      /* ��������  */
    unsigned char   cServices;       /* ��������  */
    unsigned char   cProtocol;       /* Э��      */
    unsigned short  nPort;           /* �˿ں�    */
} ncService;

typedef struct ncMailSubList_s {
    char pName[64];
    struct ncMailSubList_s *psNext;
} ncMailSubList;


/* �ʼ���ع��˹���  */
typedef struct ncMailFilter_s {
    char   caRname[64];
    char   caFrom[64];
    char   caTo[64];
    char   caSub[64];
    char   caCont[64];
    char   caAtt[64];
    int    lSize;
    int    lAsize;
    short  nFlags;
    short  nSflags;
    char   caSmail[64];
} ncMailFilter;


typedef struct ncUploadFilter_s {
    ncMailSubList *Name;
    ncMailSubList *Host;
    ncMailSubList *Content;
    ncMailSubList *Attachname;
    unsigned long lSize;
} ncUploadFilter;



typedef struct ncPostFileList_s {
    char     pOriginFile[128];   /* Դ�ļ�    */
    char     pType[12];          /* �ļ�����  */
    char     pOutFile[64];
    unsigned long lSize; 
    struct ncPostFileList_s *psNext;   
} ncPosFileList;


typedef struct ncPostDataContent_s {
    unsigned long  lBytes;         /* ���ֽ���  */
    unsigned long  lTime;          /* ʱ��      */
    char           caName[32];     /* �û���    */
    char           caHost[128];    /* ������    */
    char           caUrl[128];     /* ����URL   */
    char           *pValue;        /* ��������  */
    int            iSum;           /* ��������  */
    ncPosFileList  *psFileList;    /* �ļ��б�  */
} ncPostDataContent;


#define NC_IPPKG_UPFLOW      0
#define NC_IPPKG_DOWNFLOW    1

typedef struct ncIpPkgInfo_s {
    long8    lIpFlow[2];     /*  0 Upload Bytea  1  DownBytes  */
    unsigned long lStartTime;     /*  ��ʼʱ��                      */
    long8    lSumPkg;        /*  �����ݰ�                      */
    unsigned long lpPkgPerSec;    /*  ÿ�����ݰ�                    */
    unsigned long lStartCountTime; /* ��������ʼʱ��                */
    long8    lPkgCount;      /*  ���ݰ�������                  */
    unsigned long lCount;         /*  ���ʴ���                      */
    unsigned long lStartControl;  /*  ʱ����Ƶ���ʼʱ��            */
    char          caFilename[128];
} ncIpPkgInfo;

typedef struct ncSumControl_s {
    unsigned long  lStartTime;       /* ��ʱ��                     */
    unsigned long  lLastTime;        /* ������ʱ��                 */
    unsigned long  lTimeOut;         /* ���ʱʱ��                 */        
    unsigned short nLock;            /* ��  0--����   1--���ڼ��    */
    unsigned short nR1;              /*                               */
    unsigned long  lR1[2];           /* ����                          */
} ncSumControl;

/* ������Ϣ ��1��ʼ  */
#define NC_CONTROL_NOUSE           0
#define NC_CONTROL_DELETE          1
#define NC_CONTROL_TABDROP         5
#define NC_CONTROL_MODPORT         6


/* Iptables �Ŀ���̨����           */
#define NC_TABLES_ADDDROP          0     /* ����һ����ֹ����      */
#define NC_TABLES_CHECKTABLE       1     /* ���iptables ������   */
#define NC_TABLES_RESETTABLE       2     /* ����Iptables������    */
#define NC_TABLES_REDICTPORT       3     /* �˿��ض���            */
#define NC_TABLES_CANCELREDICT     4     /* ȡ���˿��ض���        */
#define NC_TABLES_SIGNAL           5     /* ��ָ�����̷���Ϣ      */
#define NC_TABLES_REMOTEFILE       6     /* Զ���ļ�ϵͳ          */

struct ncTestRemote_s {     /* Զ���ļ�����  */
    char  caIp[16];
    char  caHost[64];
    char  caUser[32];
    char  caPass[32];
    char  caMount[64];
    char  caShare[32];
    char  caSdate[12];
    char  caEdate[12];
    char  caOpt[12];       /* ����       */
};
/* ��ΪRemoteMountʱ cPort��ֵΪ���������  */
#define NC_REMOTE_MOUNT         1
#define NC_REMOTE_MOUNTTEST     2
#define NC_REMOTE_MAILBACK      3
#define NC_REMOTE_UNMOUNT       4
#define NC_REMOTE_MAILLOAD      5
#define NC_REMOTE_MAILEXPORT    6    /* ���ݵ���  */
typedef struct ncControl_s {
    unsigned long  lSip;             /* ��ʼIP    */
    unsigned long  lDip;
    unsigned short nDport;
    unsigned char  cProt;            /* Э��                         */
    unsigned char  cUse;             /* ������ĺ�                   */
    unsigned long  lLastTime;        /* ��ʼʱ��                     */
    unsigned long  lStop;            /* ��ֹʱ��                     */
    int            iIndex;
} ncControl;

typedef struct ncIpControlInfo_s {
    unsigned long  lId;              /* ControlId                   */
    unsigned long  lSip;             /* ��ʼIP                      */
    unsigned long  lDip;
    unsigned long  lStop;            /* ��ֹʱ�� hhmmss             */
    unsigned short nDport;
    unsigned char  cProt;            /* Э��                        */
    unsigned char  cComm;            /* ���� 0--Add 1--Del 2--Flush */    
} ncIpControlInfo;

typedef struct ncDevList_s {
    char caDev[32];
    char caIp[32];
    char caNetmask[32];
    char caBoard[32];
    struct ncDevList_s *next;
} ncDevList;

    
#define  NC_PID_COLLECT        0     /* ���ݲɼ�    */
#define  NC_PID_MAILFILTER     1     /* �ʼ����    */
#define  NC_PID_TABLECTL       2     /* IpTable���� */
#define  NC_PID_NTSYNC         3     /* ��ͬ��      */
#define  NC_PID_ONTIME         4     /* ��ʱ        */
#define  NC_PID_ICAUTH         5     /* ��������֤  */
#define  NC_RELOAD_NETWORK     0
#define  NC_RELOAD_LIMIT       1
#define  NC_RELOAD_MAILMONFILTER  2
#define  NC_RELOAD_DEBUG          4

typedef struct ncReloadControl_s {
    int   iPid[16];              /* ���̺�   */
    unsigned long lStime[16];    /* ����ʱ�� */
    unsigned long lTimeOut[16];  /* ��ʱʱ�� */
    unsigned char  caControl[32];         /* ������Ϣ */
} ncReloadControl;

#define  NC_ONTIME_IPCHECK        0   /* ��ʱ���IP����  */
#define  NC_ONTIME_PROCESSCHECK   1   /* ��ʱ������    */
#define  NC_ONTIME_COUNT          2   /* ����ͳ��        */
#define  NC_ONTIME_DELDETAILLOG   3   /* �����ϸ��Ϣ    */
#define  NC_ONTIME_DELTEMPFILE    4   /* ɾ����ʱ�ļ�    */
#define  NC_ONTIME_DATABACK       5   /* ���ݱ���        */
#define  NC_ONTIME_CHECKIPTAB     6   /* ���IPtables    */
#define  NC_ONTIME_RELOADCNF      7   /* �����������    */
#define  NC_ONTIME_TIMECONTROL    8   /* ���ʱ�����    */
#define  NC_ONLINE_RESTARTNC      9   /* ��������NC      */
typedef struct ncOnTimeControl_s {
    unsigned long lStime[16];    /* ���ִ��ʱ�� */
    unsigned long lTime[16];     /* ʱ����     */
} ncOnTimeControl;



/* �ʼ����˹���  */
typedef struct ncMailCtl_s {
    unsigned long lId;
    char caFrom[64];
    char caTo[64];
    char caSub[64];
    char caCont[64];
    char caAttname[64];
    unsigned long lMsize;
    unsigned long lAsize;
    unsigned char cFlags;     /* ���Ʊ�ʶ  1--���� 2--��  3-- ȫ�� */
    unsigned char cActflags;  /* ����ʽ 0--���� 1--������  */
    unsigned char control;    /* �ʼ����� 0--���� 1--�ܾ�  2--����ָ����ַ  3--�ȴ�ȷ�� */
    unsigned char cTzflags;   /* ֪ͨ��ʽ 0--��֪ͨ  1--֪ͨ������  2--�ռ��� 3--ָ������  */
    char caSmail[64];         /* ת������        */
    char caTzmail[64];        /* ֪ͨ�ʺ�        */
    char caTzcont[256];       /* ֪ͨ����        */
} ncMailCtl;

/* Add 2002/11/30 */
typedef struct ncFieldList_s {
    char   caFname[32];   
    char   cType;          /* ���� n-���� s-�ַ� */
    char   cFlags;
    short  nLen; 
} ncFieldList;


/* nc000.c */
                    
int ncIsInternalIp(utShmHead *psShmHead,unsigned long lSip);
int ncIsLimitServers(utShmHead *psShmHead,
                     ncUserInfo     *psUserInfo,
                     unsigned long  lUid,
                     unsigned long  lDip,
                     unsigned short nService,
                     unsigned short nDport,
                     unsigned int   iProtocol,
                     unsigned long  *lStop
                    );
int ncCompIpIndex(unsigned long l1,unsigned long l2,unsigned short n2);
int ncGetHttpInfo(char *pIn,int iLen,char *pHost,char *pUrl,
       char *pAgent);
int ncStrGetVar(char *pIn,
                int inLen,
                char *pVar,
                char *pValue,
                int outLen);
int ncStrGetUrl(char *pIn,int iLen,char *caUrl,int outlen);
ncUserInfo *ncGetUserInfo1(utShmHead *psShmHead,unsigned long lIp,unsigned char *pMac);
ncUserInfo *ncGetUserInfo(utShmHead *psShmHead,unsigned long lIp,unsigned char *pMac);
int ncCompHttpNum(utShmHead *psShmHead);
int ncCheckIp(utShmHead *pHead);
int ncSendCheckTables(char *p);
int ncTablesControl(utShmHead *psShmHead,
             unsigned long  lSip,
             unsigned long  lDip,
             unsigned short nDport,
             char           cProtocol,
             unsigned long lStop);
/* nc001.c */

ncMailFilter *ncMilInitFilter(utShmHead *psShmHead,ncMailFilter *ps,unsigned 
                 long *lChangeTime) ;
ncUploadFilter *ncUploadInitFilter(utShmHead *psShmHead,ncUploadFilter *ps0,
        unsigned long *lChangeTime);
ncUserInfo *ncCheckNetworkIp(utShmHead *psShmHead,ncIpHead *psIpHead,int *iFlags0);
int ncGetService(utShmHead *psShmHead,unsigned char cProtocol,
               unsigned short nPort);


/* nc002.c */
int ncInitIpPkg(utShmHead *psShmHead);
int ncInitInternal(utShmHead *psShmHead);
int ncInitLimit(utShmHead *psShmHead);
int ncInitMonitor(utShmHead *psShmHead);
int ncInitUserInfo(utShmHead *psShmHead);
ncUserInfo *ncGetEmptyUserInfo(utShmHead *psShmHead,ncUserInfo *psUserInfo,
                        unsigned long lIp);
ncUserInfo *ncGetEmptyUserInfo1(ncUserInfo *psUserInfo,
                        char *caMac);                        
ncUserInfo *ncGetUserInfo3(utShmHead *psShmHead,
                     long lUserid, unsigned char *pMac);                        
int ncGetMonNum(utShmHead *psShmHead, unsigned long lId);
int ncGetLimitNum(utShmHead *psShmHead,unsigned long lId);   
/* nc003.c */
int ncWebAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncHttpWriteDb(ncUserInfo *psUserInfo,ncTcpBuffer *psTcpBuffer,ncHttpBuffer *psHttpBuffer);
int ncWebDispUserAccess(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead); 
int ncWebShowUserStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebReplace(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebQyeIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebModiPass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebFlowCount(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowNewvister(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
char *ncCvtL2TimeStr(unsigned long lTime);
int ncWebSetupInternet_save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupInternet_list(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupInternet_form(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupWeb_list(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupWeb_save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupWeb_form(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowNewweb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShutdown(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispModiPass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc004.c */

int ncWebDelSomeUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserAddForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebListUserTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);                                           
int ncWebUserAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispDeptForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAssignUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminUserList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminDispUserForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminRoleList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncAdminRoleModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminRole(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncAdminUserSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebListGroupTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserModify(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserModiForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserInfo_c(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserSave_c(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc005.c */
int ncWebDispNetwork(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetNetwork(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc006.c */
int ncDelCountLog(char *psShmHead0);
int ncDelDetailLog(char *psShmHead0);
int ncCountLog(utShmHead *psShmHead0);
int ncDelTempFile(char *psShmHead0);

/* nc007.c  */
char *ncCvtConntime(unsigned long lConntime);
int ncWebCountIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
char *ncCvtSetService(utShmHead *psShmHead,int lService);
char *ncCvtMac(unsigned char *caMac);
int ncWebCountTotalFlow(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOutFlowChart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountIpChart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc008.c */
int ncWebQyehttp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserBindAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispBindUserForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispMyaddress(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserAddMyaddress(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCount(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBandWidthSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispBandWidthForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUserRandWidth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebInternetLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebInternetLogout(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebInternetModpass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserBindWidthSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebIpHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebIpDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebHttpHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebHttpDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebRefreshUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserBandModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserBandForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


/* nc009.c */
int ncWebSortUserByFlow(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortWeb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailReadUploadHeads(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncmil001.c */
/* int ncMailReadFoldHeads(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncUploadReadContent(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailUploadRemove(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
*/
/* ncmil002.c */
int ncMailShowRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailDispOneRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailModiOneRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailDelSomeRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlOutHtm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlAuditlst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlAudit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailCtlCheck(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailAudit1(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlHlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlHistorySearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDispHSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlHistoryDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlRead2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncmail003.c */
int ncMailMonListRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailMonDispOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailMonModiOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailMonDelSome(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncmil005.c */
int ncWebMailMonList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonList_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonHsear(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonHsear_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDel_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFw(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFw_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDispSform_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFwOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebUploadList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUploadRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUploadDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebMailExportDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


/* ncmail006.c */
int ncWebPopSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispPopForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPopAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDelSomePop(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDispBakSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlSaveBakSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailByBm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailByU(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCountChart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailByStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDispImport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailSaveImport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDoImport2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDoImport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebMailDispExport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDoExport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailShowExpStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


int ncWebMailCtlSaveDefSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDispDefSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc016.c */
int ncWebTimeCtlReSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc011.c */

ncPostDataContent *ncReadPostData(char *caFile, char *pOutFile);
/* nc013.c */
int ncBackup(utShmHead *psShmHead);
/* nc014.c */
int ncWebPrintWebUse(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUseListGroupTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebuseDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncutl001.c */
char *ncMailCtl_GetFile();

/* nc016 */
int ncWebTimeCtlSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTimeControl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackupBaseInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowBackup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackFileDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDelBackFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
#endif
