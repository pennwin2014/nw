#ifndef __NCURLDEF__
#define __NCURLDEF__
#define  NC_URL_UPFILE_ID       62666
#define  NC_URL_UPFILE_URL      11760
#define  NC_URL_UPFILE_CLASS    11761

#define  NC_URL_UPFLAGS_UNKNOW         0         // δ����ļ�¼
#define  NC_URL_UPFLAGS_LOCK           1         // ���ڴ���ļ�¼
#define  NC_URL_UPFLAGS_AUTO           2         // �Զ�����ļ�¼
#define  NC_URL_UPFLAGS_DONE           5         // ������ϵļ�¼
#define  NC_URL_UPFLAGS_FIX            9         // �̶������޸ĵļ�¼

#define NC_URL_KEY              "1234Pr0626ay1234"

/* ��ַ�����ļ���¼   */
typedef struct ncUrlUpRecord_s {
    unsigned short nId;                /* ��¼ID,ÿ����¼�ĵ�һ������ΪNC_URL_UPFILE_ID */
    unsigned short nType;              /* ��¼���, 11760--URL   11761--���            */
    unsigned char  cOpt;               /* ����  d--ɾ��  a-����                         */
    unsigned char  cLen;               /* ���ݳ���                                      */
    unsigned short nClass;             /* ���  0--δ����, >0 �������                  */
    unsigned long  lTime;              /* ��¼����ʱ��                                  */
    unsigned char  buf[128];           /* ����,���ܺ������                             */
} ncUrlUpRecord;

    

typedef struct ncUrlWebName_s {
    unsigned long   lId;                   /* ���ID                 */
    unsigned char   flags;                 /* 1-- ���   2-- ��ַ��  */
    char     name[31];
}ncUrlWebName;

/*  
    ��ַ���ʼ���󱣴����ļ���
 */
typedef struct ncUrlCode_s {
    unsigned char   urlCode[16];                // Url��Md5��ʽ
    unsigned short  nId;                        // �������
    unsigned char   cRev;                       // �����ֶ�
    unsigned char   cFlags;                     // 0--����  1--����   9--�˹��޸�
    unsigned long   lCount;                     // ���ʴ���
    unsigned long   lLastTime;                  // ���һ�η���ʱ��
} ncUrlCode;

typedef struct ncUrlValue_s {
    unsigned short  nId;                    // �������
    unsigned char   nLen;                   // caUrl�ĳ���
    unsigned char   cFlags;                 // �����ֶ�, 1-����   9--�˹��޸�
    unsigned long   lCount;                 // ���ʴ���
    unsigned long   lLastTime;              // ���һ�η���ʱ��
    unsigned char   caUrl[128];             // Url & ˵��,�Զ��ŷֿ���ʹ��Tea�㷨����
} ncUrlValue;

/* �����ݿ��б���ļ�¼�ṹ   */
typedef struct ncUrlRecord_s {
    unsigned char   urlCode[16];            // ��ֵ
    ncUrlValue      urlValue;               // ����
} ncUrlRecord;

/* ��ַ��������Ϣ
   �ۺ���Ϣ
   ProeimUrlv7.pronetway.com     url,v1,v2,uptime
   �����Ϣ
   ###.webclass.proeim.com
   ������־
   Num0--����������Ϣ            ���������
   ####.updatelog.proeim.com     url,UpLogMesg
   
*/
/* ��ַ���ͷ�ļ�             */
typedef struct ncUrlBufHead_s {
    unsigned long   lSize;                  // �ֽ���
    unsigned long   lMaxUrl;                // ���Url����
    unsigned char   cLock;                  // 0-δ��  1-����
    unsigned char   cFull;                  // 1 �ڴ���������Ҫ����
    unsigned char   cShm;                   // 
    unsigned char   cUpMark;                // ���±�־  0--����  1--�и�����Ϣ   */
    unsigned long   lTimeOut;               // ��ʱ����ʱ��
    unsigned char   *pHash;
} ncUrlBufHead;



/* ��ַ�Ѽ��ƻ�  
   ʹ��Hash����, KeyΪUrl
 */
typedef struct ncCapUrl_s {
    unsigned char  caUrl[64];                // Url��Ϣ
    unsigned long  lStartTime;               // ��ʼʱ��
    unsigned long  lLastTime;                // ��ֹʱ��
    unsigned long  lCount;                   // �������
    unsigned long  lSid;                     // 0--�¼�¼  >0--������ļ�¼, ��ֵ��Ӧ�ڱ��е�Sid
} ncCapUrl;

/* Url������Ϣ�Ѽ�  */
typedef struct ncCapErrUrl_s {
    unsigned char   caUrl[128];               // Url��Ϣ
    unsigned long   lSip;                     // ������IP
    unsigned long   lDip;                     // Ŀ���ַ
    long            lStatus;                  // �������
    unsigned char   caMsg[64];                // ������Ϣ
    unsigned long   lStartTime;               // ��ʼʱ��
    unsigned long   lLastTime;                // ��ֹʱ��
    unsigned long   lCount;                   // �������
    unsigned long   lSid;                     // 
} ncCapErrUrl;



int ncUrlInitUrlBuf(int iFlags,int iMaxUrl,char *pDbFile);
int ncUrlFreeUrlBuf();
int ncUrlInitShmUrl(char *pDbFile);
int ncUrlOpenUrlDb(char *pFile);
int ncUrlCloseUrlDb();
int  ncUrlGetUrlInfo(char *pUrl,char *pDesc,int *lId);
int ncUrlGetUrlClassId(char *pUrl);
int ncUrlGetUrlFromDbFile(char *pUrl,ncUrlRecord *psRecord);
int ncUrlGenUrlDb(char *pDbFile,int iMaxUrl);
// int ncUrlUpdateVersion(DEPOT *dp,unsigned long lVersion,unsigned long lDbVersion,unsigned long lUpdate);
int ncUrlGetUrlVersion(char *pEngVer,char *pDbVer,char *pUpdate,int *iSumRecord);
int ncUrlCvtMySql2Qdbm(char *pDbFile,unsigned long lTime);
int ncUrlIsInUrlClass(char *pUrl,long lId);
int ncUrlGetClassIdByName(char *pName);
int ncUrlAddUnkonwUrl(char *pUrl);
int ncUrlCvtText2UrlDb(char *pFile,char *pDbFile);
int ncUrlDumpUrlFromDb(char *pDbFile,char *pFile);
int ncUrlLoadNameCache();
char *ncUrlGetChcheName(int flags,unsigned long lId,char *pDefault);
int ncUrlGetChcheIdByName(int flags,char *pName);
char *ncUrlGetClassNameById(unsigned long lId,char *pDefault);
int ncUrlCheckUrlBuf(int iTimeOut);
int ncUrlUpdateUrl(char *pUrl,int lId,char *pDesc);

#endif
