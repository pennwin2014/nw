#ifndef  ___DSSYNC_DEF___
#define  ___DSSYNC_DEF___
#define DSSYNC_DB_UPLOAD                   0
#define DSSYNC_DB_DOWNLOAD                 1
#define DSSYNC_DEFAULT_MINSYNCCLIENTS      20    /* ���ٿͻ���      */
#define DSSYNC_DEFAULT_STEPSYNCCLIENTS     10    /* ÿ�����ӿͻ���      */
#define DSSYNC_DEFAULT_MAXSYNCCLIENTS      500   /* ���ͻ���      */

#define DSSYNC_DEFAULT_CHECKCLTTIME        10    /* �ͻ��˼����  */
#define DSSYNC_CLIENT_ONLINE               1
#define DSSYNC_CLIENT_OFFLINE              2
#define DSSYNC_MAXSQLSMT                  16000
#define DSSYNC_MAXLOGLEN                  60000
typedef  struct {
    char caSrvname[48];   /* ���������� srvname      --- �������б�
                                        srvname_srv1 --- ��������Ӧ�ı� */
    char caValues[8192];  /* ����������  */
} dsSyncServerHead;



typedef struct dsSyncServerLst_s {
    char  caSrvname[32];
    char  *psTable;
    struct dsSyncServerLst_s *psNext;
} dsSyncServerLst;


typedef struct dsSyncServer_s {
    char *pUpload;
    dsSyncServerLst *psDownload;
} dsSyncServer;

/* ͬ����¼  */
typedef struct dsSyncSrvRecord_s {
    char             caName[32];    /* ����      */
    char             *pTable;       /* ����     */   
    unsigned long    lTime;     /* ͬ��ʱ��  */
    unsigned long    lPos;      /* �ļ�λ��  */
} dsSyncSrvRecord;

typedef struct dsSyncRecord_s {
    unsigned long  lDbStartTime;      /* ���ݵ���ʼ����  */
    char           caDblogFile[128];  /* DB��־�ļ����� */
    char           caDblogPath[128];  /* ת������־�ļ���ŵ�Ŀ¼  */
    char           caName[32];     /* ����������      */
    char           caSid[32];      /* ͨ����ҪԿ      */
    unsigned long  lLogPos;        /* ��־�ļ������  */
    unsigned long  lLogTime;       /* ��־�ļ���ʱ�� 0--��ʾ��δ�������ʱ�ļ� */
    unsigned short nNum;           /* ���            */
    unsigned long  lServerIp;      /* ��������Ip��ַ  */
    unsigned short nServerPort;    /* �������Ķ˿ں�  */
    unsigned short nSumSync;       /* ��Ҫͬ���ķ���������    */
    unsigned short nCurSrv;        /* ��ǰ����ɲ����ķ�����  */
    short          nStatus;        /* ״̬  <0 ����           */
    dsSyncSrvRecord *psSrv;
    char            *pUpload;      /* ���صı�              */
 } dsSyncRecord;
  

typedef  struct {  /* ͬ���ͻ���Ϣ  */
    char     caName[32];         /* ����                   */
    char     caSid[32];          /* ��֤��Key              */
    unsigned long  lIp;          /* IP��ַ                        */
    unsigned short nStatus;      /* 0--NoUse  1--���� 2--������       */
    unsigned char  cModiFlags;   /* �޸ı�־ 0--δ�޸� 1-���޸�       */
    unsigned char  cLock;        /* ��  0--����  1--��     */
    unsigned long  lLoginTime;   /* ��½ʱ��               */
    unsigned long  lLastTime;    /* ���һ�β���ʱ��       */
    unsigned long  lRecordTime;  /* ��ǰ�ļ���ʱ��         */
    unsigned long  lRecordPos;   /* ��󱣴��¼��λ��     */
} dsSyncOnline;

/* dssync001.c */
int dsSyncInit000(utShmHead *psShmHead);
int dsSyncTcpInit(utShmHead *psShmHead);
int dsSyncLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSyncCvtDownLoad(dsSyncServer *psServer,char *caValue);
dsSyncServer  *dsSyncAuth(utShmHead *psShmHead,char *pName,char *pKey,char *pSid,int *iNum);
int dsSyncRequestUpload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSyncPutLastRecord(char *pName,long lTime,unsigned long lPos);
int dsSyncDataUpload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSyncRequestDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int dsSyncGetSql(char *pTable,    /* Ҫ��ı�     */
                 char *pName,     /* ����������   */
                 unsigned long lTime,
                 unsigned long lPos,
                 unsigned long *lLastPos,
                 unsigned long *iSum,
                 char *pSql);
int dsSyncWriteLog(utShmHead *psShmHead,unsigned long lTime,
                      char *pName,char *pSmt);
int dsSyncWriteDbRecord(char *pSmt,int   iSum);
char *dsSyncGetOneSql(char *pSmt,char *pSql,int iMaxLen);
dsSyncOnline *dsSyncCheckAuth(utShmHead *psShmHead,int iNum,char *pSid);
                                        
/* dssync002.c */
dsSyncServer *dsSyncLoadConfig(char *pConfFile,char *pServername,char *pKey);
dsSyncServer *dsSyncReadOneServer(FILE *fp,char *pServername,char *pKey);
dsSyncServer *dsSyncReadServer(FILE *fp,char *pName,char *pKey);
int dsSyncFreeServerLst(dsSyncServerLst *psServerLst);
dsSyncServerLst *dsSyncAddServer(dsSyncServerLst **psServerLst,
           char *pSrvname,char *pTable);
char *dsSyncAddStr(char *pSrc,char *pAdd);
/* dssync003.c */
unsigned long dsSyncFileSize(char *pFile);
unsigned long dsSyncCvtLongTime(char *pInstr);
int dsSyncGetOneSqlFromFile(FILE *fp,char *pTable,char *pSql);
#endif
