/* Э�����  */
#ifndef __NCANYPROTDEF___
#define __NCANYPROTDEF___

#define NC_ANY_TAG          1
#define NC_ANY_TAGEND       2
#define NC_ANY_VARVALUE     3
#define NC_ANY_REMARK       4
#define NC_ANY_ERROR        5

#define NC_ANY_SEND         0
#define NC_ANY_RECV         1
#define NC_ANY_ALL          9


#define NC_ANY_MATCH_START      0           // ����ʼλ�ÿ�ʼƥ��
#define NC_ANY_MATCH_END        1           // �����ݰ�����λ�ÿ�ʼƥ��
#define NC_ANY_MATCH_CUR        2           // ����һ��ƥ�俪ʼ
#define NC_ANY_MATCH_ANY        3           // ����λ��ƥ��
#define NC_ANY_MATCH_COND       4           // �߼����ʽ
#define NC_ANY_MATCH_REGULAR    5           // ������ʽ(Դ��δ�����)
#define NC_ANY_MATCH_REGULAROK  6           // �ѱ���
#define NC_ANY_MATCH_ANYSTART   7           // �ӿ�ʼλ�õ�ƫ����������λ��ƥ��
#define NC_ANY_MATCH_ANYEND     8           // �ӽ���λ�õ�ƫ����������λ��ƥ��
#define NC_ANY_MATCH_ERROR      9           // ����

#define NC_PAT_MAXPORTVALUE      65535  // �˿ڵ����ֵ
#define NC_PAT_MINPORTVALUE      1      // �˿ڵ����ֵ

#define NC_PAT_MAXPKG            1500   // ���ݰ�����󳤶�
#define NC_PAT_MINPKG            1      // ���ݰ�����С����

#define NC_PATOP_AND             0      // And
#define NC_PATOP_OR              1      // Or

#define NC_PAT_PAGESIZE          4096   // ÿ��ת����¼��

#define NC_DOSERVICE_GETPORTSERVICE    10000
#define NC_DOSERVICE_GETIPSERVICE      20000
#define NC_DOSERVICE_GETPATSERVICE     30000

#define NC_PAT_MAXBIT 20000
#define NC_PAT_SUMBIT 2500

#define NC_PAT_KEY              "Pro$Netway20090501"
#define NC_PAT_MAXHOSTLEN   48


typedef struct ncPatHostServiceKey_s {
        char        caName[NC_PAT_MAXHOSTLEN];
        unsigned    short nCount;
} ncPatHostServiceKey;

typedef struct ncAnyNameValue_s {
    unsigned char name[16];
    unsigned char value[128];      /* ��������  */
    int      iLine;                // �����ļ����к�
    struct ncAnyNameValue_s *next;
} ncAnyNameValue;

typedef struct ncAnyTag_s {
    unsigned char   tagname[16];
    ncAnyNameValue *psNameValue;
    int      iLine;                 // �к�
    struct ncAnyTag_s *son;
    struct ncAnyTag_s *next;
} ncAnyTag;


typedef struct ncAnyTagHead_s {
    ncAnyTag *psTag;
}ncAnyTagHead;


#define NC_PAT_FILEID               20080420L    // �ļ���ʶ
#define NC_PAT_FILEVERSION          70503L       // ����汾��

#define NC_PAT_ID_PATNAME           1110
#define NC_PAT_ID_IPSERVICE         1111
#define NC_PAT_ID_PORTSERVICE       1112
#define NC_PAT_ID_SERVICEDEF        1113
#define NC_PAT_ID_FUNDEF            1114
#define NC_PAT_ID_PATSERVICE        1115
#define NC_PAT_ID_PATRECORD         1116

#define NC_PAT_ID_HOSTSERVICE       1116
#define NC_PAT_ID_END               1999

#define NC_PAT_WORDNAME_USERNAME      1
#define NC_PAT_WORDNAME_DISPNAME      2
#define NC_PAT_WORDNAME_COMPNAME      3
#define NC_PAT_WORDNAME_POSITION      4
#define NC_PAT_WORDNAME_GROUP         7

#define NC_PAT_CODE_GB                0
#define NC_PAT_CODE_BIG5              1
#define NC_PAT_CODE_UTF8              2
#define NC_PAT_CODE_JP                3
#define NC_PAT_CODE_IPADDRESS         10
#define NC_PAT_CODE_MAC6              11
#define NC_PAT_CODE_MAC12             12
#define NC_PAT_CODE_MAC17             13


/* �ļ�ͷ 48���ֽ�  */
typedef struct ncPatFileHead_s {
    unsigned long   lId;                // �ļ�ID
    unsigned long   lVersion;           // ����汾��      7.00.01
    unsigned long   lCreateTime;        // �ļ�����ʱ��
    unsigned long   lLastUpdate;        // ������ʱ��
    unsigned long   lLastTime;          // ������ʱ��
    unsigned long   lEncrypt;           // �Ƿ����
    unsigned long   lMd5;               // Md5У����
    unsigned long   lBytes;             // �ļ��ֽ���
    unsigned long   lSumDoName;         // ģ������
    unsigned long   lSumService;        // ��������
    unsigned long   lSumFunction;       // Ӧ�÷�������
    unsigned long   lSumIpService;      // IP��������
    unsigned long   lSumPortService;    // �˿�����
    unsigned long   lSumHostService;    // �˿�����
    unsigned long   lSumPatService;     // ģʽ����
    unsigned long   lDataVersion;       // Э���İ汾�� 9.05.21
    unsigned char   cRev[32];           // ����
} ncPatFileHead;


/* �ļ���¼         */
typedef struct ncPatFileRecord_s {
    unsigned long  lRecordId;          // ��¼��ʶ
    unsigned long  lSumRecord;         // ��¼����
    unsigned long  lSrcBytes;          // ԭ�����ֽ���(δ����)
    unsigned long  lNowBytes;          // �����ֽ���(���ܺ�)
} ncPatFileRecord;



/* ����Ľṹ���ڴ�Ź����ڴ��е�����  */
typedef struct ncPatWord_s {
    unsigned short  nOffSet;             // ��ʼλ��
    unsigned short  nEndPos;             // ��ֹλ��
    unsigned char   cLen;                // ����
    unsigned char   cMatch;              // ƥ�䷽ʽ
    unsigned char   cName:4;             // 1--ȡ����  2--ȡ����  3--��ʾ��   4--λ����Ϣ
    unsigned char   cEncode:4;           //   ���뷽ʽ
    unsigned char   cNameLen;            //   ���ֳ���, ��Ϊ0,caContΪ��׺�ַ�ֵ
    unsigned char   caCont[64];          //   �ַ�����
} ncPatWord;


/* ÿ����¼��Ϣ   
     һ����¼Ϊһ������������
 */
typedef struct ncPatRecord_s {
    unsigned short  nService;    // �������
    unsigned char   cFun;        // ��������
    unsigned char   cSum;        // ƥ������(Word����)
    unsigned short  nMinLen;     // ���ݰ���С����
    unsigned short  nMaxLen;     // ���ݰ���󳤶�
    unsigned char   cProt;       // Э�� 0-TCP  1-UDP
    unsigned char   cDirect:4;   // ���� 0-���� 1-����
    unsigned char   cLevel:4;    // ���򼶱�
    unsigned short  nNext;       // ��һ���ж� 0--���� 1--���ñ�Ĵ������ 
    unsigned short  nCount;      // ��СƵ��,�����ݰ�������
    unsigned char   cPortLen;    // �˿��б���ֽ���
    unsigned char   cRule;       // �����������ֽ��� 
    unsigned short  sid;         // �������к�
    unsigned char   prio;        // �������ȼ� ��С����, ȱʡΪ32
    unsigned char   status:4;    // ����״̬  0-δȷ��  1-����
    unsigned char   rev:4;         // ����
    unsigned char  *pPortList;   // �˿��б�
    unsigned char  *pRulename;   // ��������
    ncPatWord      *psWord;
}  ncPatRecord;


typedef struct ncPatSearRec_s {
    ncPatRecord *psRecord;
    int    iDeep;                  // ���
    struct ncPatSearRec_s *next;   // ��һ��
    struct ncPatSearRec_s *left;   // ͬһ������һ����¼
} ncPatSearRec;



/* �˿���Ϣ   */
typedef struct ncPatPortIndex_s {
    ncPatRecord     **psRecord;       // ������� psRecord��ָ��
    ncPatSearRec    *psSear;
    unsigned short  nLport;           // ��ʼ�˿�
    unsigned short  nUport;           // ��ֹ�˿�
    unsigned short  nSum;             // ���� 
    unsigned short  nMax;             // ����¼��
} ncPatPortIndex;


/* �˿���Ϣ   */
typedef struct ncPatRecIndex_s {
    ncPatPortIndex  *psIndex;
    unsigned short  nSum;             // ���� 
    unsigned short  nMax;             // ����¼��
} ncPatRecIndex;


/* ������   */
typedef struct ncPatService_s {
    unsigned char        caName[16];      /* ģʽ����                               */
    int                  iId;             /* ID         */
    unsigned long        nSum;            /* psRecord ��¼����                      */
    unsigned long        nMax;            /* ���������¼��                       */
    ncPatRecIndex        sPort[2][2];     /* 0: 0-TCP 1-UDP    1: 0-Send 1-Recv     */
    ncPatRecord          **psRecord;        /* ��¼ָ��                             */
} ncPatService;


#define NCDNS_MAX_DNSSERVICE     1024
#define NCDNS_MAX_IPSERVICE      8192

#define NCDNS_FROM_DYNAMIC       1          // ϵͳ��̬���
#define NCDNS_FROM_CONFIG        2          // �����ļ�
#define NCDNS_FROM_DNS           8          // DNS


/* ĳЩ������������ձ�                        */
typedef struct ncPatHostService_s {
    char           caName[NC_PAT_MAXHOSTLEN];  /* ����        */
    unsigned short nCount;      // ���,��0��ʼ
    unsigned short nSum;        // �ܵļ�¼��
    unsigned short nService;     /* �������    */
    unsigned char  cFun;         /* ���ܴ���    */
    unsigned char  cProt;       // Э��: 0--ȫ��  6--TCP    7--UDP
    unsigned short nSport;      // ��ʼ�˿�
    unsigned short nDport;      // ��ֹ�˿�
    unsigned short nId;         // ������ID
    unsigned short nNextId;
    unsigned char  cFrom;       // ��Դ
    unsigned char  cRev[3];
} ncPatHostService;

/* IP ��ַ�ͷ�����ձ�
      �ؼ���Ϊǰ7���ֽ�

 */
typedef struct ncPatIpService_s {
    unsigned       long lIp;
    unsigned short nId;         // ��ַ��ʶ
    unsigned char  cCount;      // ���,��0��ʼ
    unsigned char  cSum;        // �ܵļ�¼��
    unsigned short nSport;      // ��ʼ�˿�
    unsigned short nDport;      // ��ֹ�˿�
    unsigned short nService;    // �������
    unsigned char  cFun;        // ����
    unsigned char  cProt;       // Э��: 0--ȫ��  6-TCP  7-UDP
    unsigned char  cFrom;       // ��¼��Դ
    unsigned short nNext;       // ��������
    unsigned char  cRev;        // ����
    unsigned long  lLasttime;   // ������ʱ��
    unsigned long  sid;         // ��¼��ʶ
} ncPatIpService;


/* ���صķ���  */
typedef struct ncRetService_s {
    unsigned short nService;          // ����
    unsigned char  cFun;              // ���ܶ���
    unsigned char  cCont:2;     	  // ������Ϣ 0-������  1-������
    unsigned char  cPortService:1;    // 
    unsigned char  cIpService:1;      // 
    unsigned char  cPatService:1;     // 
    unsigned char  cFunService:1;     // 
    unsigned char  cContinue:1;       // 0--���� 1--���� 
    unsigned char  cRev:1;            // 
    unsigned short cCount;      // Ƕ�״���
    unsigned short nId;         // DebugID
} ncRetService;

#define NC_PATDO_UNKNOW         0    // δ֪
#define NC_PATDO_PORTSERVICE    1    // �˿ڷ����嵥
#define NC_PATDO_IPSERVICE      2    // IP����
#define NC_PATDO_PATSERVICE     3    // ģʽƥ��
#define NC_PATDO_PROGRAM        4    // ����ʵ��
#define NC_PATDO_HOSTSERVICE    5    // �������ձ�

// ģʽ���ƶ��ձ�
typedef struct ncPatNameIndex_s {
    unsigned short nId;              // ���, ��0��ʼ
    unsigned char  cType;            // ���
    unsigned char  cRev;
    int      (*fFunName)(ncTcp *psTcp,unsigned char *pkg,int iLen,ncRetService *psRet);     /* ������     */
    unsigned char caName[16];        // ����
    unsigned char caMemo[128];        // ˵��
} ncPatNameIndex;




/* �˿ڷ�����ձ�     */
typedef struct ncPortService_s {
    unsigned short nId;         // �˿ڱ�ʶ
    unsigned short nPort;       // �˿�
    unsigned char  cProt;       // 6-Tcp  17-Udp
    unsigned char  cFun;        // ���ܶ���
    unsigned short nService;    // ����
    unsigned short nNext;       // ��һ������  
    unsigned char  cType;       // ������� 1-PortService 2-IpService 3-PatSevice 4-Function
    unsigned char  cRev;
    unsigned long  sid;         // ��¼��ʶ
} ncPortService;


/* �������   */
typedef struct ncServiceName_s {
    unsigned short   nService;        /* ��������  */
    unsigned short   nGacode;         /* ��������  */
    unsigned char    caName[24];      /* ��������  */
    unsigned char    caMemo[96];      /* ˵��      */
    unsigned char    cRev;
    unsigned char    cNext;           /* �Ƿ��н�һ���ķ��� 0--��  1--��  */
    unsigned char    cMark;           /* ��ʶ  0-δ������  1-��������       */
    unsigned char    cFlags;          /* ����ʶ   */
} ncServiceName;

/* ���ܱ���   */
typedef struct ncFunName_s {
    unsigned short   nFun;            /* ���ܱ���  */
    unsigned short   nRev;            /* ����      */
    unsigned char    caName[16];      /* ��������  */
} ncFunName;


typedef struct ncPatLanIp_s {
    unsigned long lSip;
    unsigned long lEip;
    int      iFlags;
} ncPatLanIp;

/* ��¼������������   */
typedef struct ncCondLen_s {
    short    k;                   // ϵ��
    short    pos:10;              // λ��
    short    type:6;              // ��� 0-���� 1-��ʼ  2-���� 3-��ǰλ��
} ncCondLen;


/* ���ط������    
   ��һ��λ�ô�ż�¼����
   �ڶ���λ�ô�����ռ�����
 */
#define  NC_PAT_SERVICEBASE  10000000L
typedef struct ncNomonService_s {
    unsigned long   lService;             /* ����:   10000000 + Service  */
} ncNomonService;



typedef struct ncPatPkgAccName_s {
    char       name[63];
    uint4      iType;
} ncPatPkgAccName;

typedef struct  ncPatWordHead_s {
    uint4       iSumWord;
    uint4       iService;
    ncPatWord   *psWord;
}  ncPatWordHead;

typedef struct ncPatAccountIp_s {
    uint4               lIp;
    uint4               nPort;
    uint4               nSum;
    uint4               nMax;
    ncPatWordHead       *psWordHead;
} ncPatAccountIp;


typedef struct  ncPatAccountHead_s {
    uint4               nSumIp;
    uint4               nMaxIp;
    ncPatAccountIp      *psIpAccount;
    uint4               iSumWord;
    uint4               iMaxWord;
    ncPatWordHead       *psWordHead;
} ncPatAccountHead;


/* �����������    */
typedef struct ncPatHead_s {
    unsigned    long    lCreateTime;              // ����ʱ��
    unsigned    long    lLastUpdate;              // ������ʱ��
    unsigned    long    lLastTime;                // �������ʱ��
    unsigned    long    lVersion;                 // ����汾
    ncNomonService      *psNomonService;          // ���ط������
    ncServiceName       *psServiceName;           // �������
    int                 iSumService;              // ��ǰ��������
    int                 iMaxService;              // ��ǰ���ķ�����
    ncFunName           *psFunction;              // ��ǰ���ܱ���
    int                 iSumFunction;             // ��ǰ��������
    int                 iMaxFunction;
    unsigned  char      *pPortService;           // �˿ڷ�����ձ�
    unsigned  long      lMaxPortService;         // ���˿���
    unsigned  long      lPortBufSize;            // Hash��ռ�õĴ�С
    unsigned  char      *pIpService;             // IP������ձ�
    unsigned long       lMaxIpService;           // ���IP����
    unsigned long       lIpBufSize;              // Hash��ռ�ô�С
    unsigned long       lMaxHostService;         // ������Ϣ
    unsigned long       lHostBufSize;
    unsigned char       *pHostService;
    ncPatService        *psPatService;
    int                 iSumPatService;
    int                 iMaxPatService;
    ncPatNameIndex      *psDoName;               // ����������
    int                 iSumDoName;              // ��ǰ����ģʽ����
    int                 iMaxDoName;              // �ѷ���ռ��ģʽ����
    int                 iSortMark;               // �����ʶ 0-δ���� 1-������
    int                 iSysAtShm;               // �Ƿ�ʹ�ù����ڴ�
    ncPatLanIp          *psLanIp;                 // ����Ip��ַ,���ڵ���
    int                 iSumLanIp;
    int                 iMaxLanIp;
    unsigned char       cServiceSort;
    unsigned char       cFunctionSort;
    unsigned char       cCheckMark;
    unsigned char       cReguMark;
    unsigned char       cDes;                     // �Ƿ�Ӽ����ļ�������
	unsigned char       cP2PIntelligent;             // �Ƿ������жϷ���, ���7/8��
    unsigned char       cRev[2];
    unsigned long       lP2PMinPkg;                  // ��С���ݰ�����
    unsigned long       lP2PMinTcp;                  // ��С������
    unsigned long       lP2PMinFlow;                 // ��С����
    unsigned long       lP2PHttpFlow;                // ��С����
    unsigned long       lP2PHttpsFlow;               // ��С����
	unsigned long   	lDataVersion;       		// Э���İ汾�� 9.05.21
    unsigned char       *pContService;              // �����Ƿ�������� 2048���ֽ�
} ncPatHead;


#endif
