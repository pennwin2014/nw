/* ģ�崦�����
   Writen by Li Yunming  in 2000/05/30
 */
#ifndef  UTPLTDEF0001
#define  UTPLTDEF0001    1
#include <stdarg.h>
#define UTPLT_NOEXISTVAR_SPACES         0
#define UTPLT_NOEXISTVAR_PRNVAR         1
#define UTPLT_NOEXISTVAR_EXIT           2
#define UTPLT_MAXVARLEN      32
#define UTPLT_ALLOCSTEP      4096      /* ÿ�η���ռ�Ĵ�С    */
#define UTPLT_MARKB          "[#"      /* ������ʶ��ʼ�ַ���    */
#define UTPLT_MARKE          "#]"      /* ������ʶ����ֹ�ַ���  */
#define UTPLT_LOOPB          "SLPB"    /* ѭ����������ʼ��ʶ    */
#define UTPLT_LOOPE          "SLPE"    /* ѭ����������ֹ��ʶ    */
#define UT_WEB_USRNOEXIST              -1   /* �û�������       */
#define UT_WEB_INVUSRINFO              -2   /* ��Ч���û���Ϣ   */


/* ��������  2002/07/11 ����Ч��ԭ���Ϊ������, ���ṹ�Ѳ���  */
typedef struct utPltDbLink_S_bak {
        char  *caVarName;   /* ������     */
        char  *pValue;                       /* ֵ        */
        struct utPltDbLink_S *next;          /* ��һ��ֵ  */
} utPltDbLink_bak;


typedef struct utPltDbLink_S {
        char  *caVarName;   /* ������     */
        char  *pValue;                       /* ֵ        */
        struct utPltDbLink_S *Left;          /* ������    */
        struct utPltDbLink_S *Right;         /* ������    */
} utPltDbLink;



/* Html���ݻ�����  */

typedef struct utPltHtmBuf_S {
        unsigned int iLen;                  /* ��ǰ�ַ�������          */
        unsigned int iMaxBytes;             /* Ŀǰ����ռ����󳤶�  */
        char     *pBuffer;                  /* ���ݻ�����              */
} utPltHtmBuf;


/* ģ����������ͷָ��  */
typedef struct  utPltDbHead_S_bak {
        utPltDbLink  *psFirst;   /* ����ͷָ��  */
        utPltDbLink  *psEnd;     /* ����βָ��  */
        utPltHtmBuf  *psHtmBuf;  /* Html������  */
        int           iCookLen;   /* Cookies�����ĳ���  */
        int           iCookMax;   /* Cookies����ĳ���  */
        char         *pCookies;   /* Cookies ����      */
        int           iVersion;   /* Version > 0 ��ʾ���°汾  */
} utPltDbHead_bak;


typedef struct  utPltDbHead_S {
        utPltDbLink  *psFirst;    /* ����ͷָ��  */
        utPltHtmBuf  *psHtmBuf;   /* Html������  */
        int           iCookLen;   /* Cookies�����ĳ���  */
        int           iCookMax;   /* Cookies����ĳ���  */
        char         *pCookies;   /* Cookies ����      */
        int           iVersion;   /* Version > 0 ��ʾ���°汾  */
} utPltDbHead;


/* utoplt01.c */
int utPltSetCvtHtml(int iFlags);
utPltDbHead *utPltInitDbHead();
utPltDbHead *utPltInitDb();
int utPltFreeDb(utPltDbHead *psDbHead);
int utPltFreeDbLink(utPltDbLink *psDbLink);
int utPltSetPlatePath(char *pPath);
char *utPltGetPlatePath();
int utPltSetVarNoExist(int iFlags);
char *utPltToHtmBuf(char *pIn,utPltDbHead *psDbHead);
int utPltDoLoop(char *pIn,utPltDbHead *psDbHead,char *pPre);
int utPltPutVar(utPltDbHead *psDbHead,char *pVarName,char *pValue);
char *utPltCvtHtmlChar(char *pValue);
int utPltShowDb(utPltDbHead *psDbHead);
int utPltShowDbLink(utPltDbLink *psDbLink);
char *utPltLookVar(utPltDbHead *psDbHead,char *pVarName);
int utPltGetStrToMark(char *pIn, utPltHtmBuf *psBuf,
                    char *pVarName, char *pMarkB,char *pMarkE);
utPltHtmBuf *utPltInitHtmBuf(int iMaxBytes);
int utPltFreeHtmBuf(utPltHtmBuf *psHtmBuf);
int utPltGetMemoryNum();
char *utPltFileToHtml(char *pFile,utPltDbHead *psDbHead);
int utPltFileToHtmlFile(char *pFile,char *pOutFile,utPltDbHead *psDbHead);
int utPltPutSomeVar(utPltDbHead *psDbHead,int iNum,...);
int utPltPutVarF(utPltDbHead *psDbHead,char *pVarName,char *pFormat,...);
int utPltPutLoopVar(utPltDbHead *psDbHead,char *pVarName,int i,char *pValue);
int utPltPutLoopVarF(utPltDbHead *psDbHead,char *pVarName,int iNum,char *pFormat,...);
int utPltPutLoopVar3(utPltDbHead *psDbHead,char *pVarName,int i,int j,int k,char *pValue);
int utPltPutLoopVarF3(utPltDbHead *psDbHead,char *pVarName,int iNum,int iNum1,int iNum2,char *pFormat,...);
int utPltPutLoopVar2(utPltDbHead *psDbHead,char *pVarName,int i,int j,char *pValue);
int utPltPutLoopVarF2(utPltDbHead *psDbHead,char *pVarName,int iNum,int iNum1,char *pFormat,...);
int utPltStrcpy(utPltDbHead *psDbHead,char *pStr);
char *utPltMsgToFile(char *caPltFile,int iSumVar,...);
int utPltOutToHtml(int iFd,utMsgHead *psMsgHead,utPltDbHead *psDbHead,char *pPlate);
int utPltDoLoopNew(char *pIn,utPltDbHead *psDbHead,char *pPre);
int utPltSetCookies(utPltDbHead *psDbHead,char *pName,char *pValue,
        char *pPath,char *pDomain,char *pExpire);
int utPltDelCookies(utPltDbHead *psDbHead,char *pName);
int pasSetCookies(char *pName,char *pValue,
        char *pPath,char *pDomain,char *pExpire);
int pasDelCookies(char *pName,char *pPath,char *pDomain);
int utPltFileDownload(int iSock,char *pContentType,char *pPath,char *pFileName,char *pSave);
int utPltDispBin(int iSock,char *pContentType,char *pContent,
                 int lBytes);
int utPltHtmlFileOut(int iFd,utMsgHead *psMsgHead,char *pFile);
/* utoplt02.c */
int utWebDispMsg(int iFd,utMsgHead *psMsgHead,char *pPltName,char *pTitle,char *pFormat,...);
int utWebSetUserInfo(utPltDbHead *psDbHead,char *UserName,char *caRight,char *ipAddress,char *path,char *domain);
int utWebCheckUser(utMsgHead *psMsgHead,int iRight);
int utWebGetLoginUser(utMsgHead *psMsgHead,char *caUserName,char *caIp);
int utWebRightOk(char *caRight,int iRight);
int utWebSetRight(char *caRight,int iRight);
int utWebDelRight(char *caRight,int iRight);
int utWebClearRight(char *caRight);
/* utoplt03.c */
int pasDispTcpMsg(int iFd,utMsgHead *psMsgHead,int iStatus,char *pPltName,char *pTitle,char *pFormat,...);
#endif

