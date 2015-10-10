#ifndef  ___DSUTL_DEF___
#define  ___DSUTL_DEF___

#define UT_TYPE_FLIST       30
#define _DS_MD5_CODEKEY      "pronetway"



/* ��������  */
typedef struct dsUtlAttrList_s {
    unsigned short nType;             /* ����,                  */
    unsigned short nBytes;            /* ���ݵ��ֽ���           */    
    unsigned long lAttr;        /* �������� md5code(upper(name))  */
    char *pValue;               /* ����ֵ                 */
    struct dsUtlAttrList_s *psNext;
} dsUtlAttrList;

/* ����   */
typedef struct dsUtlAttr_s {
    unsigned long  lCode;                      /* ������Md5������   */
    char           caName[32];                 /* ��������            */
    char           caDispname[32];             /* ��ʾ����            */
    dsUtlAttrList  *psAttr;                    /* ���Ե�����          */
    struct dsUtlAttr_s    *psNext;
} dsUtlAttr;


typedef struct dsUtlConf_s {
    dsUtlAttr *psFun;
    dsUtlAttr *psRight;
    dsUtlAttr *psApp;
    unsigned long lModitime;   /* ��¼�޸�ʱ��  */
} dsUtlConf;


/* dsutl001.c */
int dsUtlLoadConfig(char *pConfFile,dsUtlConf *psConf);
int dsUtlReadOneAttr(FILE *fp,dsUtlConf *psConf);
dsUtlAttr *dsUtlReadFunList(FILE *fp,dsUtlAttr *psAttr,char *pName);
int dsUtlFreeAttr(dsUtlAttr *psAttr);
dsUtlAttrList *dsUtlAttrAdd(dsUtlAttrList *psFun,char *pName,int iType,int iBytes,char *pValue);
dsUtlAttrList *dsUtlAttrAppend(dsUtlAttrList *psFun,char *pName,int iType,int iBytes,char *pValue);
dsUtlAttrList *dsUtlGetAttrByName(dsUtlAttrList *psFun,char *caName);
dsUtlAttrList *dsUtlGetAttrByCode(dsUtlAttrList *psFun,unsigned long lCode);
dsUtlAttr *dsUtlReadRightList(FILE *fp,dsUtlAttr *psAttr,char *pName);
dsUtlAttr *dsUtlReadRoleList(FILE *fp,dsUtlAttr *psAttr,char *pName);
dsUtlAttr *dsUtlReadApp(FILE *fp,dsUtlAttr *psAttr,char *pName);
int dsUtlPrintAttr(dsUtlAttr *psAttr);

/* dsutl002.c */
int _dsUtlSetFun(utShmHead *psShmHead,dsUtlConf *psConf);
int _dsUtlGetAttrList_l(dsUtlAttrList *psFun,char *pName);
char *_dsUtlGetAttrList_s(dsUtlAttrList *psFun,char *pName);
char *_dsUtlGetAttrList_v(dsUtlAttrList *psFun,long lCodee,int *iNum);
char *dsUtlNewRightValue(utShmHead *psShmHead,int *iSize);
int _dsUtlSetRightValueByRight(dsUtlConf *psConf,
            unsigned char *pRightValue,
            unsigned long lRightId);
int _dsUtlSetRightValueByRole(dsUtlConf *psConf,
            unsigned char *pRightValue,
            unsigned long lRoleId);
unsigned long *_dsUtlGetFunlistByRight(dsUtlConf *psConf, 
                           unsigned long lRightId,
                           int *iSum);
unsigned long *_dsUtlGetRightlistByRole(dsUtlConf *psConf, 
                           unsigned long lRoleId,
                           int *iSum);
long  dsUtlGetAppAttr_l(dsUtlConf *psConf, char *pAttrName);
char *dsUtlGetAppAttr_s(dsUtlConf *psConf, char *pAttrName);                           

#endif
