#ifndef  __UTOINSTDEF__
#define  __UTOINSTDEF__
#define  UTINS_FILEID       19651129L
#define  UTINS_PKGFILEID    19990928L
#define  UTINS_VERSION      100
#define  UTINS_MAXFILE      1024
#define  UTINS_STARTINDEX   512

typedef struct utInsPkgHead_S {
    unsigned long  lFileId;        /* �ļ���ʶ            */
    unsigned long  lVersion;       /* ��İ汾��          */
    unsigned long  lSumFile;       /* �ļ�����            */
    unsigned long  lSumBytes;      /* ���ֽ���            */
    unsigned long  lMaxFile;       /* �����ļ����������  */
    unsigned long  lStartIndex;    /* ��������ʼλ��      */
    char    caProduct[32];         /* ��Ʒ����            */
    char    caDes[64];             /* ��Ʒ����            */
    FILE    *fp;
} utInsPkgHead;

typedef struct utInsFileIndex_S {
    char            caFileName[64];
    char            caInsPath[64];
    int             iMode;              /* �ļ�����  */
    unsigned long   lStartPos;
    unsigned long   lSize;
} utInsFileIndex;
     

typedef struct utInsVarList_S {
    char   caVarName[16];           /* ������    */
    int    nType;                   /* ����      */
    int    nBytes;
    int    nStartPos;               /* ��ʼλ��  */
} utInsVarList;

typedef struct utInsFunList_S {
    char  caFunName[16];
    int   nSumPar;
    int   nParType[10];
} utInsFunList;


typedef struct utInsPreHead_S {
    int   nSumVar;       /* ��������          */
    int   nMaxVar;       /* ��������        */
    int   nSumBytes;     /* �������ܵ��ֽ���  */
    int   nMaxBytes;     /* ����ֽ���        */
    int   nSumFun;       /* ����������        */
    int   nMaxFun;       /* �������������    */
    struct   utInsVarList_S  *psVarList;
    struct   utInsFunList_S  *psFunList;
    char     *pData;
} utInsPreHead;


typedef struct utInsSmtInfo_S {
    int    nFunCode;
    int    nSumNum;
    int    nVarNum[10];
} utInsSmtInfo;

typedef struct utInsFileHead_S {
    int    iId;
    int    iVersion;
    int    iVarPos;
    int    iSumVar;
    int    iSumBytes;
    int    iReserve[27];
} utInsFileHead;

/* utoins01.c */
int utInsGetStatement(FILE *fp, utInsPreHead *psPreHead,
                                        utInsSmtInfo *psSmtInfo);
int utInsPrepSmt(utInsPreHead *psPreHead,int iFunNum,char *pInstr,
                                    utInsSmtInfo *psSmtInfo);
int utInsAdd_s(utInsPreHead *psPreHead, char *pInstr);
int utInsAdd_l(utInsPreHead *psPreHead, long lValue);
int utInsLookVar(utInsPreHead *psPreHead,char *pVarName);
int utInsLookFun(utInsPreHead *psPreHead,char *pFunName);
int utInsAddFun(utInsPreHead *psPreHead,char *pFunName,
            unsigned short nSumPar,...);
int utInsAddVar(utInsPreHead *psPreHead,char *pVarName,int iType,int iBytes);
utInsPreHead *utInsPreHeradinit();
/* utoins02.c */
utInsPkgHead *utInsCreateFile(char *pFileName,unsigned long lMaxFile,
        char *pProduct, char *pDes);
utInsPkgHead *utInsOpenFile(char *pFileName,char *pMode);
int utInsCloseFile(utInsPkgHead *psInsPkgHead);
int utInsAddFileToPkg(utInsPkgHead *psInsPkgHead,
             char *pFileName,char *pInsPath,int iMode);
int utInsGetFileFromPkg(utInsPkgHead *psInsPkgHead,utInsFileIndex *psInsFileIndex,
                            int iFlags);
int utInsExpand(utInsPkgHead *psInsPkgHead,int iFlags);

                                            
#endif

