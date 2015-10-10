/* Pas Cgi 
   Write by Li Yunming
 */

#ifndef UTDEFCGI_DEF
#define UTDEFCGI_DEF
#include <stdarg.h>
#define UT_CGI_MAXSERVER      8
#define UT_CGI_SERVER_NAME    0
#define UT_CGI_USER_AGENT     1
#define UT_CGI_SERVER_PORT    2
#define UT_CGI_HTTP_REFERER   3
#define UT_CGI_REMOTE_ADDR    4
#define UT_CGI_REQUEST_URI    5
#define UT_CGI_REMOTE_USER    6
#define UT_CGI_AUTH_TYPE      7
#define UT_CGI_REMOTE_PORT    8
#define UT_CGI_REMOTE_IDENT   9

typedef struct  {
  char  *name;             /* ����  */
  char  *val;              /* ֵ    */
} utCgiEntry;


typedef struct  {
  char  *name;             /* ����  */
  char  *val;              /* ֵ    */
  int   iLen;              /* ����  */
  int   iType;             /* ����  0--һ�����  1--�ļ��ַ�   2---�ļ��ṹ  */
} utCgiEntryU;

typedef struct {
    long   lIpAddress;    /* IP��ַ      */
    short  nPort;         /* �˿ں�      */
    short  nFlags;        /* ������״̬ 0 --����  1--������  */
} utCgiServer;

typedef struct {
    short nSumServer;                               /* ����������              */
    short nCurServer;                               /* ��ǰ������              */
    short nSumProxy;
    short nCurProxy;
    utCgiServer    sCgiServer[UT_CGI_MAXSERVER];    /* ��������Ϣ              */
    utCgiServer    sProxy[UT_CGI_MAXSERVER];                          /* �����������Ϣ              */
    long  lTimeOut;                  /* ��ʱʱ��                */
    char           caUpLoadPath[64]; /* �����ļ���ŵ�Ŀ¼      */
    long  lMsgCode;                  /* ��Ϣ��ʶ                */
    long  lMaxTcpPkg;                /* Tcp������ݰ�           */
    long  lShmKey;                   /* �����ڴ��ʶ,��Ϊ0ʱ,����    */
    char  caSendEnv[16];              /* ������������           */ 
    int   iDebug;                    /* iDebug ==1 ��ʾ������Ϣ 0--����ʾ������Ϣ */
    int   lMaxRequest;
} utCgiConfInfo;
     
typedef struct {
    utCgiConfInfo  *sCgiConf;         /* �����ļ���Ϣ  */
    int    iSumVar;                  /* ��������    */
    int    iSumCookies;              /* ���Cookies   */
    utCgiEntry *entries;           /* ������Ϣ      */
    utCgiEntry *cookies;           /* Cookies��Ϣ   */
} utFcgiHead;


char *utCgiMakeWord(char *line, char stop);
char utCgiX2C(char *what);
void utCgiUnEscapeUrl(char *url);
void utCgiPlusToSpace(char *str);
utCgiEntry *utCgiRetrieveArgs(int *iSumVar);
utCgiEntry *utCgiRetrieveCookie(char *cpGetCookie,int *iSum);
char *utCgiMsgErr(char *errorMsg);
int utCgiReadConfig(char *caConfigFile,utCgiConfInfo *psCgiConf);
int utCgiGetIpPort(char *caIp,long *lIpAddress,short *nPort);
int utCgiFreeHead(utFcgiHead *psCgiHead);
int utCgiConnect(utCgiConfInfo *psCgiConf);
int utCgiPrintf(char *pFormat,...);
int utCgiFreeEntries(utCgiEntry *psEntries,int iSum);

char *utCgiMakeWord(char *line, char stop);
char *utCgiFmakeWord(FILE *fp, char stop, int *length);
char utCgiX2c(char *what);
void utCgiUnEscapeUrl(char *url);
void utCgiPlusToSpace(char *str);
utCgiEntry *utCgiRetrieveArgs(int *iSumVar);
utCgiEntry *utCgiRetrieveCookie(char *cpGetCookie, int *iSum);
char *utCgiMsgErr(char *);
int utCgiFreeEntries(utCgiEntry *psEntries,int iSum);
int utCgiPrintf(char *pFormat,...);
int utCgiDo(utCgiConfInfo *psCgiConf);
int utCgiSendEnvTo(char *pEnvMark,utMsgHead **psMsgHead);
/*  utocgi02.c  */
utCgiEntryU *utCgiUpLoad(FILE *fp,char *pTempPath,int *iSumVar);
int utCgiFreeEntriesU(utCgiEntryU *psEntries,int iSum);
int utCgiFgets(char *pBuffer,int iMaxLen,int *iReadLen,FILE *fp);
int utCgiReadUntilF(FILE *fpi,FILE *fpo,char *pTerm,int iLen);
int utCgiReadUntilS(FILE *fpi,char *pBuffer,int *iReadLen,char *pTerm,int iLen);
char *utCgiReadUntilSP(FILE *fpi,int *iReadLen,char *pTerm,int iLen);
int utCgiFReadHead(FILE *fp,char *pBuffer);
int utCgiAnalyLine(char *pBuffer,char *caVarName,char *caFileName,char *caContentType);
int utCgiCvtSaveFile(char *caFileName,char *pTempPath,char *caSaveName);
int utCgiDoUpload(utCgiConfInfo *psCgiConf);
#endif
