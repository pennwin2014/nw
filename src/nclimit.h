/* ����޸�����  2010/06/26

*/
#ifndef __NCLIMITDEF__
#define __NCLIMITDEF__
#define NC_LIMIT_INTERNET  1              /* Internet�� */
#define NC_LIMIT_SERVICE   2              /* ����       */
#define NC_LIMIT_IPADDRESS 3              /* ��ַ��Χ   */
#define NC_LIMIT_IPLIST    4              /* Ip�б�     */
#define NC_LIMIT_WEBLIST   5              /* ��ַ�����    */
#define NC_LIMIT_WEBHOST   6              /* Web��ַ    */
#define NC_LIMIT_VID       7              /* �������   */
#define NC_LIMIT_WEBCLASS  8              /* ��ַ���   */
#define NC_LIMIT_FUNLIST   9              /* �����б�   */

/* ���ʿ��Ʋ���   
               100����Ϊ����ID
*/
#define NC_LIMIT_REJECT         0         /* ��ֹ       */
#define NC_LIMIT_ACCEPT         1         /* ����       */
#define NC_LIMIT_REJECTWARN     2         /* ��ֹ�澯   */
#define NC_LIMIT_ACCEPTWARN     3         /* ����澯   */
#define NC_LIMIT_NOMON          4         /* ����     */
#define NC_LIMIT_SETREDIR       5         /* �˿��ض���        */
#define NC_LIMIT_DELREDIR       6         /* ȡ���˿��ض���    */
#define NC_LIMIT_NOEXIST        9 
#define NC_LIMIT_ERROR          -1

#define NC_LIMIT_DELQOS         9         /* ɾ��QOS�еĹ���  */
#define NC_LIMIT_REJECTUSER     20        /* �����û�����     */
#define NC_LIMIT_WIDTHBAND1     31        /* �������1        */
#define NC_LIMIT_WIDTHBAND2     32        /* �������2        */
#define NC_LIMIT_WIDTHBAND3     33        /* �������2        */




#define NC_CTLSTA_ACCEPT        0
#define NC_CTLSTA_DROPWARN      1 
#define NC_CTLSTA_ACCEPTWARN    2
#define NC_CTLSTA_DROP          3


#define NC_LIMIT_BYUSER    1
#define NC_LIMIT_BYGROUP   2
#define NC_LIMIT_BYALL     3
#define NC_LIMIT_BYPOLCY   4
#define NC_LIMIT_BYCUSER   6
#define NC_LIMIT_BYCGROUP  7
#define NC_LIMIT_BYCALL    8

#define NC_LIMDATE_DAY    1      /* ������  */
#define NC_LIMDATE_WEEK   2      /* ÿ��    */
#define NC_LIMDATE_MONTH  3      /* ÿ��    */
#define NC_LIMDATE_YEAR   4      /* ÿ��    */

#define NC_DATE_WORKID    1      /* ������      */
#define NC_DATE_NOWORK    2      /* �ǹ�����    */
/* ʱ��  */
typedef struct ncLimDate_s {
    unsigned long lType;                    // ʱ�����   1--Day     2--Week 3--Month 4--Year    
    unsigned long lLower;                   // ʱ������  1-YYYYMMDD 2--0/6  3--1/31  4--MMDD
    unsigned long lUpper;                   // ʱ������
} ncLimDate;

typedef struct ncLimDateIndex_s {
    int       lStart;                  // ��ʼλ��
    int       lSum;                    // ��¼����
    long      lId;
    char      name[32];                // �������
} ncLimDateIndex;

typedef struct ncLimDateHead_s {
    ncLimDate       *psDate;
    ncLimDateIndex  *psIndex;
    int             lSumIndex;
    int             lSumDate;
} ncLimDateHead;





/* ������ݵ�����  */
typedef struct ncLimVid_s {       
    unsigned long   lId;               /* ���ID                      */
    unsigned long   lService;          /* �������, 0 ȫ��            */
    unsigned long   lVidList;          /* ��������б�ID              */
    unsigned long   lVidNum;           /* ���Uid�����,From 0,��װ�ص�ʱ���Զ�����   */
    unsigned char   vid[64];           /* ��ʶ                        */
    unsigned long   lStart;            /* ��ʼʱ�� HHMMSS                             */
    unsigned long   lEnd;              /* ��ֹʱ�� HHMMSS                             */
    unsigned long   lCid;              /* �ͻ��˲���ID                              */
    unsigned long   lSid;              /* �������˲���ID                              */
    unsigned long   lValidTime;        /* ��Ч��                                      */
    unsigned long   lWarntype;
    unsigned char   cTimeType;         /* ʱ������                                    */
    unsigned char   cFun;              /* �������� 0--ȫ��  1--����  2--����  3--�����ļ�   */
    unsigned short  cVister;           /* 0 ��ֹ   1 ����  2 ��ϸ澯  3 ����澯     */
} ncLimVid;

/*
   ������ݼ�
 */
typedef struct ncLimVidList_s {
    unsigned long   lSid;              /* ��Ӧ�����ݿ��е�Sid         */
    unsigned char   uid[64];           /* ��ʶ                        */
} ncLimVidList;

/* ����,��lID����
*/
typedef struct ncLimVidIndex_s {
    unsigned long   lId;               /* ���Id                      */
    unsigned long   lStart;            /* ��ʼλ��                    */
    unsigned long   lSum;              /* ȫ����¼��                  */
    unsigned char   name[32];          /* �������                    */
}ncLimVidIndex;


/* ������ݼ���ͷ��Ϣ   */
typedef struct ncLimVidHead_s {
    ncLimVidList    *psList;           /* ��������ָ��                */
    ncLimVidIndex   *psIndex;          /* ������Ϣָ��                */
    unsigned long   lSumIndex;         /* ��������                    */
    unsigned long   lSumVid;           /* �����⼯                    */
}ncLimVidHead;

/* �ṹ������ NC_LNK_VIDSET ��   */
typedef struct ncLimIp_s {             /* ���Ʒ��ʵ�Ip��ַ  */
    unsigned long   lId;               /*                             */
    unsigned long   lIpLower;          /* ����, If flags==4 ����ID 2|9 --����    */
    unsigned long   lIpUpper;          /* ����, If flags==4 �������   9-����ID  */
    unsigned short  nLowerport;        /* �˿ں�����                                  */
    unsigned short  nUpperport;        /* �˿ں�����                                  */
    unsigned char   cService;          /* ��������                                    */
    unsigned char   cFlags;            /* ���Ʒ�ʽ 1--�ⲿ��  2--����  3--IP��ַ�Ͷ˿�  4--������� 9-���� */
    unsigned char   cAccessFlags;      /* �������Ʒ�Χ: 1--User 2--Group  3--All 4--Default */
    unsigned char   cTranType;         /* ��������  0--ȫ��   17--UDP  6--TCP          */
    unsigned long   lStart;            /* ��ʼʱ�� HHMMSS                              */
    unsigned long   lEnd;              /* ��ֹʱ�� HHMMSS                              */
    unsigned long   lWarnid;           /* �澯���  Add 20051017                       */
    unsigned long   lCid;              /* �������˲���ID                               */
    unsigned long   lSid;              /* �������˲���ID                               */
    unsigned long   lValidTime;        /* ��Ч��                                       */
    unsigned short  cTimeType;         /* ʱ������                                     */
    unsigned short  nQosId;        
    unsigned long   cVister;           /* 0 ��ֹ   1 ����, > 100  QOSID                */
    unsigned long   lNum;              /* ���                                         */
}ncLimIp;


typedef struct ncLimWeb_s {       /* ���Ʒ��ʵ���վ     */
    unsigned long   lId;               /*                     */
    char            caUrl[64];         /* ��վ��ַ            */
    unsigned long   lUrlid;            /* ��ַ�б�            */
    unsigned long   lLstnum;           /* Weblist���         */
    unsigned long   lStart;            /* ��ʼʱ�� HHMMSS                             */
    unsigned long   lEnd;              /* ��ֹʱ�� HHMMSS                             */
    unsigned long   cVister;           /* 0  ��ֹ  1 ����   > 1000 QOSID              */
    unsigned char   cAccessFlags;      /* �������Ʒ�Χ: 1--User 2--Group  3--All 4--Default */
    unsigned char   cFlags;            /* 0--��ַ     5--Web�б�, caUrl����ʾ����ID   8--��ַ���*/
    unsigned short  cTimeType;         /* ʱ������                                    */
    unsigned long   lWarnid;           /* �澯���                                    */
    unsigned long   lCid;              /* �������˲���ID                              */
    unsigned long   lSid;              /* �������˲���ID                              */
    unsigned long   lValidTime;        /* ��Ч��                                      */
}ncLimWeb;

/* �ýṹ����ַ˳������    */
typedef struct ncLimUrlList_s {       /* ���Ʒ��ʵ�Web��ַ               */
    char            url[36];          /* ��ַ����                        */
    unsigned short  nNum;             /* ���,��0��ʼ                    */
    unsigned short  nSum;             /* �ظ�����,ֻ��� nNumΪ 0        */
    unsigned long   lId;              /* ��ַ�����ID                    */
    unsigned long   lSid;             /* ��ַ���Sid                     */
    unsigned long   lCount;           /* ���ʴ���                        */
    unsigned long   lLastTime;        /* ������ʱ�䣬0 ��ʾû�з��ʹ�  */
}ncLimUrlList;



/* �ýṹ����ַ˳������    */
typedef struct ncLimWebList_s {       /* ���Ʒ��ʵ�Web��ַ  */
    char            url[38];          /* ��ַ����           */
    unsigned short  iLen;             /* ����               */
    unsigned long   lId;              /* ���ID             */
    unsigned long   lSid;             /* ��ַ���Sid        */
    unsigned long   lCount;           /* ���ʴ���           */
}ncLimWebList;

typedef struct ncLimWebPol_s {       /* ���ʲ���           */
    unsigned long  lId;
    int            iFlags;
    unsigned long  lWarnid;
    char     name[32];
}ncLimWebPol;


    
typedef struct ncLimIndex_s {     
    unsigned long   lId;             /* ������Ϣ          */
    unsigned long   lStart;          /* ��ʼ���          */
    unsigned long   lSum;            /* ��ֹ���          */
} ncLimIndex;

typedef struct ncLimPolIndex_s {     
    unsigned long   lId;             /* ������Ϣ          */
    unsigned long   lStart;          /* ��ʼ���          */
    unsigned long   lSum;            /* ��ֹ���          */
    int             iFlags;          /* ȱʡ����          */
    unsigned long   lWarnid;
    char            name[32];
} ncLimPolIndex;

typedef struct ncLimWebListHead_s {       /* ���Ʒ��ʵ�Web��ַ��  */
//    unsigned char    *pWebHash;
//    ncLimWebList     *psWeb;
    ncLimWebPol      *psPol;
    ncLimPolIndex    *psPolIndex;
//    unsigned long    lSumWeb;
    unsigned long    lSumPol;
    unsigned long    lSumPolIndex;
} ncLimWebListHead;


typedef struct ncLimIpListHead_s {       /* ���Ʒ��ʵ�Ip��ַ  */
    ncLimWebPol      *psPol;
    ncLimPolIndex    *psPolIndex;        /* ���ʿ��Ʋ���      */
    unsigned long    lSumPol;            /* IP�����������           */
    unsigned long    lSumPolIndex;       /* IP��������������       */
} ncLimIpListHead;


typedef struct ncLimInfo_s {
    ncLimIp     *psIp;
    ncLimWeb    *psWeb;
    int         iSumIp;
    int         iSumWeb;
} ncLimInfo;



/* ������Ϣ    */
typedef struct ncLimHead_s {            /* ������Ϣ         */
    ncLimInfo         sLim;             /* ȫ����Ϣ         */
    ncLimInfo         sLimA;            /* ȫ��             */
    ncLimInfo         sLimG;            /* ��               */
    ncLimInfo         sLimU;            /* �û�             */
    ncLimInfo         sLimD;            /* ����             */
    ncLimInfo         sLimC;            /* ����             */
    ncUserContHead    *psUserHead;      /* ȫ���û���Ϣ     */
    ncLimGroup        *psGroup;         /* ���������Ϣ     */
    ncLimWebListHead  sWebList;         /* Web��ַ�б�      */
    ncLimIpListHead   sIpList;          /* Ip��ַ�б�       */
    ncLimPolInfo      *psPolInfo;       /* ���ʿ��Ʋ������� */
    unsigned long     lSumPol;          /* ��������         */
    unsigned long     lSumUser;         /* �û�����         */
    unsigned short    nSumGroup;        /* �������         */
    unsigned short    nDefault;         /* ȱʡ����    0--  ��ֹ����  1--������� */
    ncLimPolInfo      *psMypol;         /* �ۺϲ���         */
    ncLimSummer       sSumm;
    char              *pUserHash;       /* �����û�Hashָ�� */
    char              *pIpHash;         /* Ip������Ϣ       */
    char              *pTcpHash;        /* Tcp������Ϣ      */
    ncIpNetwork       *psIpNetwork;     /* ��������         */
    unsigned long     lSumNet;          /* �������õ�����   */
    ncLimVid          *psVid;           /* �������         */
    unsigned long     lSumVid;          /* �ܵ��������     */
    ncLimVidHead      *psVidHead;       /* ������ݼ�       */
    ncLimDateHead     *psDateHead;      /* ��������         */
    char              cRev[40];         /* ����             */
} ncLimHead;

/* nc021.c */
                     
#endif
