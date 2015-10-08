/* ������                         */
#ifndef __NCPOSTDEF___
#define __NCPOSTDEF___
#define NC_WBM_ID        651129L
#define NC_FILE_BUF       8192
#define NC_MAX_POSTBUF    64000L
#define NC_WBM_From      0
#define NC_WBM_To        1
#define NC_WBM_Cc        2
#define NC_WBM_Bcc       3
#define NC_WBM_Sub       4
#define NC_WBM_Conetnt   5
#define NC_WBM_Attach    6
#define NC_WBM_TITLE     7
#define NC_WBM_CHARSET   8    // �ַ���
#define NC_WBM_UID
#define NC_WBM_MAXITEM   16
#define NC_MAX_FILEREAD  2048    // ÿ�ζ��ļ����������  
#define NC_MAX_POSTTYPE  16
// ƥ�䷽ʽ
#define NCPOST_MATCH_ALL        0x00
#define NCPOST_MATCH_START      0x01
#define NCPOST_MATCH_END        0x02
#define NCPOST_MATCH_INC        0x03
#define NCPOST_MATCH_CASE       0x10

typedef struct ncXmlVarValue_s {  /* ������    */
    char            *pVar;              /* ������        */
    char            *pValue;            /* ����ֵ        */
    struct ncXmlVarValue_s *next;
} ncXmlVarValue;

/* ��������    */
typedef struct ncPostCont_s {  /* ������    */
    char            *pVar;              /* ������        */
    char            *pValue;            /* ����ֵ        */
    char            *pContentType;      /* �ļ�����      */
    unsigned long   lFileId;            /* �ļ����,�� proeim#����  0--��ʾ��һ���ֶ�  */
    struct   ncPostCont_s *next;
} ncPostCont;

#define NC_POST_FORM     1
#define NC_POST_UPLOAD   2
#define NC_POST_XML      3
#define NC_POST_EXCHANGE 4
#define NC_POST_STREAM   5
typedef struct ncPostHead_s {   /* Httpͷ��Ϣ(POST����)   */
    char    *pHost;     /* ������       */
    char    *pUrl;      /* Url����      */
    char    *pBound;    /* �ֽ��       */
    int     iLen;       /* ���ݵĳ���   */
    short     iType;      /* ����   1--FORM��    2--Upload��    3--XML��*/
    unsigned  cCode;      /* ����          */
    unsigned  cRev;       /* ����          */
    unsigned long lAsize;  /* ������С     */
    unsigned long lTime;
    ncPostCont *psCont;
} ncPostHead;

/* WebMail���ʼ�ͷ                    */
typedef struct ncWbmMailHead_s {
    unsigned long lUserid;                    /* �û�ID   */
    char         *pHost;                      /* ������   */
    ncPostCont   *psCont[NC_WBM_MAXITEM];
    struct ncWbmMailHead_s *next;
} ncWbmMailHead;


/* ����Hash���������
       1--������Webmail��ʽģ��
             name    Ϊ 0
             iType --- 0 ��������������һ��   1 ���������ݷֿ�
 */
 
typedef struct ncWebMailPlate_s {
    char  caHost[16];     /* ����������Ϊ0,��ʾȱʡ  */
    char  caName[16];     /* ����                    */
    unsigned short nLen;  /* ����                    */
    unsigned short nType; /* ����  1-From 2-To  3-Cc  4-Bcc 5-Subject 6--Text 7--Attach  9--CharSet*/
} ncWebMailPlate;

typedef struct ncWMPList_s {
    char     caName[16];     /* ����                    */
    unsigned short nLen;     /* ��������                */
    unsigned short nType;    /* ����                    */
} ncWMPList;


typedef struct ncWMPIndex_s {
    char            caHost[16];   /* ������  */
    int             iStart;
    unsigned short  nLen;         /* �������� */
    unsigned short  nSum;
} ncWMPindex;

typedef struct ncWMP_s {
    int             iId;
    int             iSumIndex;
    int             iSumPlate;
    ncWMPindex      *psIndex;
    ncWMPList       *psList;
} ncWMP;


/* ���� Post���˺Ż�ȡ����
   ����Hash��ķ�ʽ���
       ����URL���� ��ؼ�¼,�����ж�
       ֧������ģ�� ��: sina.com = *.sina.com
*/
#define NC_POST_NAMEPASS  1
#define NC_POST_NAMEONLY  2

typedef struct ncPostAccount_s {
    char url[64];  // ��ַ
    int  sid;      // ���к�
    int  from;     // �Ӻδ���
    char account[32];   // �˺Ŷ�Ӧ�ı��� 
    char passwd[32];    // �����Ӧ�ı���
    char email[32];
    char regid[32];
    unsigned short service;
    unsigned char  flags;
    unsigned char  code;          // ����
} ncPostAccount;

/* �ļ��嵥   */
typedef struct ncPostDirList_s {
    unsigned long lTime;        // ������ʱ��
    char     caFile[64];        // �ļ���
} ncPostDirList;



/* ������
   ���ݱ������Ա����з���
     1.���б�
     2.URL���� (����+����)
     
*/

typedef struct ncPostFVarList_s {
    char     var[20];     // ������
    short    service;     // �������---0������
    short    vartype;     // �������, ÿ�ַ��������벻һ��
    struct   ncPostFVarList *next;
} ncPostFVarList;


typedef struct ncPostFilter_s {
    char url[32];                    // ��ַ
    ncPostFVarList  *psFlist;        // ���б�
} ncPostFilter;


typedef struct ncPostVarList_s1 {
    char   var[20];                   // ������
    char   disp[16];                  // ��ʾ��
    short  vartype;
    short  cFlags;                    // 1--����    0--���п���
} ncPostVarList1;
    
/* ������     */
typedef struct ncPostDesc_s {
    char  name[16];                     // ��������
    ncPostVarList1  psList;             // ���������б�
    int   iMaxVar;                      // ��������
    int   (*fFunName)(utShmHead *);     /* ִ�к�������  */
} ncPostDesc;    


typedef struct ncPostDataList_s1 {
    short service;
    short vartype;
    char  var[20];
    char  *pValue;
} ncPortDataList1;



/* ���жϽṹ    
        0----Webmail
        1----BBS
        2----UserAccount
        3----Boke
        4----�����¼
   ÿһ�ֱ�������8������
*/

/* �����б� 
 */

#define NC_POSTPAT_INCLUDE         0
#define NC_POSTPAT_START           1
#define NC_POSTPAT_END             2   
#define NC_POSTPAT_EQUAL           3

#define NC_POSTPAT_STRING          0
#define NC_POSTPAT_TEXT            1
#define NC_POSTPAT_DIGITAL         2
#define NC_POSTPAT_DATE            3
#define NC_POSTPAT_FILE            5


#define NC_POSTPAT_MUST            2
#define NC_POSTPAT_NOCOUNT         0
#define NC_POSTPAT_COUNT           1

#define NC_POSTTYPE_BBS             0
#define NC_POSTTYPE_ACCOUNT         1
#define NC_POSTPAT_BBS_CONTENT      0
#define NC_POSTPAT_BBS_TITLE        1
#define NC_POSTPAT_BBS_BBSNAME      2
#define NC_POSTPAT_BBS_ATTFILE      3
#define NC_POSTPAT_BBS_BBSID        4
#define NC_POSTPAT_BBS_USERNAME     5

#define NC_POSTPAT_ACC_USERNAME     0
#define NC_POSTPAT_ACC_DISPNAME     1
#define NC_POSTPAT_ACC_PASSWORD     2

// ������
typedef struct ncPostPatVarList_s {
    unsigned char    caVar[32];                 // ��������
    unsigned char    cType;                     // �������� 1-�ַ���  2-�ı�  3-����  4--�ı� 5--�ļ�
    unsigned char    cMust;                     // 0-���Ǳ���  1-����
    unsigned short   nMaxLen;                   // ���� 0-������
} ncPostPatVarList;

// �����
typedef struct ncPostPatType_s {
    unsigned char    caName[32];                // ������� WebMail,BBS,Boke,Chat,...
    unsigned char    cMaxVar;                   // ��������(�������ڴ�)
    unsigned char    cMinVar;                   // ���ٱ�����
    unsigned char    cSumVar;                   // ��������
    unsigned char    cDefMinVar;                // ȱʡʱ����С������
    ncPostPatVarList *psVar;                    // �����б�
} ncPostPatType;

typedef struct ncPostPatVarNum_s {
    unsigned char  cPost;             // ����������
    unsigned char  cMust;
    unsigned short nVar;              // �������
} ncPostPatVarNum;

// ���½ṹ 2011/12/17 ���������޸�,�����������ֶ� cMatch,cRev, nSum��nMax�� short ��Ϊ char
typedef struct ncPostPatVar_s {
    unsigned char            caVar[32];           // ������
    unsigned char            nSum;                // ��������,������������ķ�������
    unsigned char            nMax;
    unsigned char            cMatch;              // ƥ�䷽ʽ 0--��ȫ 1--Start 2--Stop 3--Include
    unsigned char            cLen;                // ����
    ncPostPatVarNum          *psVarNum;
}  ncPostPatVar;
    
/* �����������б����ݰ���������  */
typedef struct ncPostPatHost_s {
    unsigned char     caHost[64];        // ������, �ڴ��Զ�����
    unsigned char     caName[32];        // ������,�ڴ��Զ�����
    unsigned short    nSumVar;           // ��������
    unsigned short    nMaxVar;           // ����ռ�
    unsigned char     cCode;             // ���� 99-δ֪���Զ��ж�
    unsigned char     cHostLen;          // pHost �ĳ���
    unsigned short    nService;          // �������
    ncPostPatVar      *psVar;
} ncPostPatHost;

typedef struct ncPostPatHead_s {
    unsigned short  nMaxType;
    unsigned short  nSumType;
    unsigned long   nMaxHost;
    unsigned long   nSumHost;
    ncPostPatType  *psPostType;
    ncPostPatHost  *psPostHost;
} ncPostPatHead;


typedef struct ncPostDoPostVar_s {
    unsigned short          nPost;
    unsigned short          nVar;
    ncPostCont              *psCont;
} ncPostDoPostVar;
    

typedef struct ncPostResult_s {
    unsigned short   nMax;               // ���ֵ
    unsigned short   nSum;               // ��ǰ����
    unsigned char    *pHost;             // ������
    unsigned char    *pUrl;              // Url
    unsigned char    *pName;             // ������
    unsigned char     caPost[NC_MAX_POSTTYPE];         // ����ʶ 
    unsigned short   nService;
    unsigned short   nRev;
    ncPostDoPostVar  *psDoPost;
} ncPostResult;


#define NC_POSTPAT_FILEID      20091122L
#define NC_POSTPAT_ENGVER      750L
#define NC_POSTPAT_POSTTYPE    8001
#define NC_POSTPAT_HOST        9002

/* �ļ�ͷ 128���ֽ�  */
typedef struct ncPostPatFileHead_s {
    unsigned long   lId;                // �ļ�ID
    unsigned long   lVersion;           // ����汾��      7.50.01
    unsigned long   lCreateTime;        // �ļ�����ʱ��
    unsigned long   lLastUpdate;        // ������ʱ��
    unsigned long   lEncrypt;           // �Ƿ����
    unsigned long   lDataVersion;       // Э���İ汾��  09.00.01
    unsigned long   lSumType;           // �������
    unsigned long   lSumHost;           // ��������
    unsigned char   cRev[32];           // ����
} ncPostPatFileHead;


/* �ļ���¼         */
typedef struct ncPostPatFileRecord_s {
    unsigned short  lRecordId;          // ��¼��ʶ
    unsigned short  lSumRecord;         // ��¼����
    unsigned long   lCrc;               // У��λ
    unsigned long   lSrcBytes;          // ԭ�����ֽ���(δ����)
    unsigned long   lNowBytes;          // �����ֽ���(���ܺ�)
} ncPostPatFileRecord;



/* ���ļ�����    */
typedef struct ncPostSynCont_s {
    unsigned long  lId;             // �ļ�ID
    unsigned long  lLen;            // �����ܳ���
    unsigned long  lPos;            // ��ʼλ��
    unsigned short nThisLen;        // �����ݰ�����
    unsigned short nMark;           // 1.��������  9.�������   8.��ֹ����
}  ncPostSynCont;

/* ���ļ�       */
typedef struct ncPostSynBuf_s {
    unsigned char   caName[24];     // �û���ʶ
    unsigned char   caMd5[16];      // ����Md5
    unsigned long   lSip;           // Դ��ַ
    unsigned long   lDip;           // Ŀ���ַ
    unsigned long   lSize;          // �ļ���С
    unsigned long   lTime;          // ʱ��
    unsigned short  nPost;          // �����
    unsigned short  nCode;          // ����
    unsigned long   lUserid;         // �û�ID
}  ncPostSynBuf;


/* ͬ���ļ��б�ṹ   */
typedef  struct ncPostSynFile_s {
        unsigned long   lCode;          // ��¼��ʶ NC_POSTSYN_SYNFILEID
        unsigned char   caFile[24];     // �ļ���
        ncPostSynBuf    sSynBuf;
} ncPostSynFile;

/* �ͻ���ͬ����Ϣ   
       ����Ψһ
   ��ʽ:
   # Name   Key   Ip
   Name -- �ͻ�������
   Key  -- ͨ����Կ
   IP   -- IP��ַ����ֵΪ Any ��ʾ�����ַ,����ֻ��Ϊָ����ַ   
 */
typedef struct ncPostSynClient_s {
    unsigned char  caName[32];      // ����
    unsigned char  md5[24];         // MD5 Key
    unsigned long  lSip;            // ��֤��ַ
    unsigned long  lLastTime;       // ͬ��ʱ��
    unsigned long  lCount;          // ������
    unsigned long  lSyn;            // ͬ������
    unsigned char  cStatus;         // ��ǰ״̬
    unsigned char  cRev[3];
}  ncPostSynClient;



#define NC_POSTSYN_VERSION      100
#define NC_POSTSYN_ERROR        1
#define NC_POSTSYN_READY        100
#define NC_POSTSYN_LOGIN        101
#define NC_POSTSYN_SENDFORM     102
#define NC_POSTSYN_FORMEND      103

#define NC_POSTSYN_REQSYN       106
#define NC_POSTSYN_SYNFILEID    1820674539L



/* �û�IP���ձ�   
       ���ڴ��ж��ڸ���
 */
typedef struct ncPostSynUserIp_s {
    unsigned long lSip;                  /* IP��ַ  */
    unsigned long lUserid;               /* �û�ID  */
} ncPostSynUserIp;


/* ����ͷ�ļ�       */
typedef struct ncPostSynReq_s {
    unsigned short  nVersion;       // �汾
    unsigned char   cFun;           // ���ܴ���
    unsigned char   cSec;           // �Ƿ���� 0-�� 1-��
    unsigned char   md5[16];        // ��֤��
    unsigned short  nRev;
    unsigned short  nNum;           // �ڷ������е���ţ���1��ʼ
    unsigned long   lTime;          // ʱ��
    unsigned long   lLen;           // ���ݳ���
}  ncPostSynReq;


    


// �ͻ�����Ϣ
typedef struct ncPostSynCltInfo_s {
    int             iSock;                    // ����Sock
    unsigned short  nNum;                     // ���
    unsigned short  nSta;                     // ״̬  0-����  1-����
    unsigned long   lSip;                     // �ͻ���IP��ַ(���ڼ��)
    unsigned long   lThisIp;                  // ��ǰ��ַ
    unsigned char   caName[16];               // �ͻ�������
    unsigned long   caKey[32];
    unsigned char   caMd5[16];                // Md5ֵ
    unsigned long   lLastTime;                // ���ͬ��ʱ��
    char            caHome[128];              // ͬ��Ŀ¼
} ncPostSynCltInfo;


/* ����Md5��Ϣ  */
typedef struct ncPostSynPostMd5_s {
    unsigned char caMd5[22];             /* Md5ֵ   */
    unsigned char cRev[2];               /* ����    */
    unsigned long lTime;                 /* ʱ��    */
} ncPostSynPostMd5;


/* ͬ������������    
 */
typedef struct ncPostSynServer_s {
    int              iReady;         // 0--������δ׼��  1--��������׼��
    unsigned short   nSumClient;     // �ͻ�������
    unsigned short   nMaxClient;     // ���ͻ�����
    unsigned long    lSumUser;       // �û�����
    unsigned long    lTimeOut;       // ��ʱʱ��
    unsigned long    lCheckTime;     // ���ʱ��
    unsigned long    lStepTime;      // ���ʱ����
    unsigned long    lMinTime;       // ����ʱ��
    unsigned long    lMaxTime;       // ���ʱ��
    unsigned long    lMaxMd5;
    unsigned long    lSid;           // ����SID
    ncPostSynClient  *psClient;       // �ͻ�������
    utShmHead        *psShmHead;
} ncPostSynServer;

typedef struct ncPostProc_s {
    int         iPid;
    unsigned    long lStartTime;
    unsigned    long lLastTime;
    char        caFile[128];
    char        caDoFile[128];
    unsigned    int  iSumFile;
    unsigned    int  iSumDoFile;
    unsigned    int  iSumCurFile;
} ncPostProc;

#endif
