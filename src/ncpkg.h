#ifndef __NCPKGDEF___
#define __NCPKGDEF___
#define NC_PKG_TCP_SYN           1
#define NC_PKG_TCP_FIN           2
#define NC_PKG_TCP_ACK           3

#define NC_PKG_HTTP              1
#define NC_PKG_POST              2
#define NC_PKG_GET               3
#define NC_PKG_MAIL              4
#define NC_PKG_POP3              5
#define NC_MTIME                 1000000L
#define NC_SAVE_ERROR            1              /* ������Ϣ  */
#define NC_SAVE_SLOG             2              /* �Ự��־  */
#define NC_SAVE_HTTPLOG          4              /* HttpLog   */
#define NC_SAVE_IPLOG            5              /* IpLog     */

#define NC_MAX_DEV               8              /* ������豸  */
typedef unsigned long long   UINT64;

typedef struct ncPkgBuf_s {
	unsigned char  cUse;           /* 0 -- None  1-- Ready  2--Put Busy   9--��ͷ��ʼ */
	unsigned char  cFlags;         /* 0 -- eth0  1-- eth1               */
	unsigned short nLen;           /* ���ݰ��ĳ���,����ǰ���ͷ         */
	unsigned long  nVid;           /* Vlan Id                           */
    unsigned char  Indev;         /* ���豸 NSA���õ�                  */
    unsigned char  Outdev;        /* ���豸 NSA���õ�                  */
    unsigned char  nsavid;        /* ͨ�����  */
    unsigned char  rev;
	unsigned long  lPos;           /* �����ݻ������е�λ��              */
} ncPkgBuf;

#define NC_MAX_PKGDO    16
typedef struct ncPkgHead_s {
    struct ncPkg000_s {
	    unsigned long  lMax;          /* ������ݰ�    */
	    unsigned long  lCur;          /* ��ǰ���ڲ���  */
	    unsigned long  lDo;           /* ���ڴ���      */
        unsigned long  lPkgPos;
        int            iPid;          /* ������̡�����*/
	} s[NC_MAX_PKGDO];
    unsigned long  lPos;          /* Ŀǰλ��      */
    unsigned long  lMaxPos;       /* ����ֽ���    */
    unsigned long  lBasePos;      /* ��ʼλ��      */
	unsigned long  long         lSumBytes[NC_MAX_DEV][2];  /* �ܼ����� 0-����      */
	unsigned long  long         lCurBytes[NC_MAX_DEV][2];  /* ��ǰ���� 1-����      */
	unsigned long  lCurPkg[NC_MAX_DEV][2];    /* ��ǰ���ݰ�           */
	
	unsigned long  lSumPkg[NC_MAX_DEV][2];    /* �ܼ����ݰ�           */
	unsigned long  lLastTime;     /* ���ͳ��ʱ��  */
	unsigned long  lStartTime;    /* ��ʼ��ʱ��    */
	int            iSumPkgDo;
	int            iMark;         /* 1-�������� 0--������ ������������  */
	int            iLock;         /* 0-- Free 1-- Busy                  */
} ncPkgHead;

#define NC_PKG_BUFPRELEN         12

typedef struct ncPkgCont_s {
    unsigned long  lId;           /* ���ݰ�ID,�������е����           */
    char     buffer[3000];        /* ����                              */
} ncPkgCont;


/* IP���ݰ�����  */
typedef struct ncPkgContBuf_s {
	unsigned long lPos;            /* �� ncPkgBuf�е�λ��  */
	unsigned long lId;             /*  ���ݰ��� ID          */
} ncPkgContBuf;


/* ���ƺ͸���     */
typedef struct ncPkgCtl_s {
    unsigned char cCtl;           /* ������Ϣ�ı仯  */
    unsigned char cLog;           /* Log�ı仯       */
    unsigned char cLock;          /* ��              */
    unsigned char cRev;           /* ����            */
    unsigned long lLastTime;      /* ���ʱ��        */
    unsigned long lCur;           /* ��ǰλ��        */
    unsigned long lDo;            /* ���ڴ���        */
    unsigned long lMax;           /* ���λ��        */
    char caCtl[128];              /* ������Ϣ��ÿһ���ֽڶ�Ӧһ�����ܲ��� 
                                      ����һ���ֽ�Ϊ0��ʱ�򣬱��������κ��޸�
                                         */
} ncPkgCtl;

#define NC_LOG_ABNORMITY 1            /* �쳣  */
#define NC_LOG_SESSION   2            /* �Ự  */
#define NC_LOG_PKGSES       3            /* Ҫ���������ݰ�  */
#define NC_CTL_LOADNETSET         1     /* װ���������ò���   */

typedef struct ncPkgLog_s { /* Log�Ľṹ  */
    unsigned char  cFlags;       /* 0--��  1--��  9--����      */
    unsigned char  cType;        /* 1--�쳣����   2--�Ự����  */
    unsigned short nLen;         /* ���ݰ��ĳ���               */
	unsigned short nService;     /* ��������                   */
	unsigned short nFun;         /* FunCode                    */
    unsigned long  lUsid;        /* �û���½ID                 */
    unsigned long  lPos;         /* λ�ã���ʵ�ʵ����ֱ���һ�� */
    unsigned long  lTime;        /* ʱ��                       */
    unsigned long  lUserid;      /* �û�ID                     */
} ncPkgLog;



typedef struct ncPkgSession_s {     /* �Ự���ݷ���  */
    unsigned long    lSid;          /* ͬ tcp��sid, �Ự�ı�ʶ  */
	unsigned long    lSeq;          /* ���ݰ������к�   */
	unsigned long    lSip;          /* Դ��ַ           */
	unsigned long    lDip;          /* Ŀ���ַ         */
	unsigned short   nSport;        /* Դ�˿�           */
	unsigned short   nDport;        /* Ŀ��˿�         */
	unsigned short   nLen;          /* ���ݰ��ĳ���     */
	unsigned char    cMark;         /* ����             */
	unsigned char    cDict;         /* ���ݰ����� 0--����  1--Ӧ��  */
	unsigned long    lAckSeq;
	unsigned char    pBuf[3000];    /* ���ݰ�������    */
} ncPkgSession;


/* �����������ݷ��������к�  */
typedef struct ncPkgSeqInfo_s {
	unsigned long  lSid;         /* Session Id     */
	unsigned long  lSeq;         /* ��ʼ���к�     */
	unsigned short nCount;       /* �ļ�������     */
	unsigned char  nMark;        /* ��ʶ 0--������  1--������  ����Э��Ĳ�ͬ����ͬ  9--�˳� */
	unsigned char  cFun;         /* �ӹ���          */ 
	unsigned long  lLastTime;    /* ������ʱ��    */
	unsigned long  lLen;         /* ��ǰ���ݳ���    */ 
	unsigned long  lSid1;        /* ���ļ�����������ļ�ID */
	unsigned long  lSid2;        /* ���ã���ͬ��Ӧ���в�ͬ���ô�  */
	unsigned char  caFrom[48];
	unsigned char  caTo[48];
} ncPkgSeqInfo;

/* ��Ա�����Ľṹ    
     �� ncPkgSeqInfo �ĵ�24���ֽڿ�ʼ
 */
typedef struct ncPkgPostSeq_s {
    unsigned short  nNum;         /* ��ǰ���   */
    unsigned short  nRev;         /* ����       */
    struct   ncPkgPostSeqLst_s {
        unsigned long   lCount;   /* ���       */
        unsigned long   lSeq;     /* ��ʼ���к� */
    } sSeq[12];
} ncPkgPostSeq;


/* �����������ݿ����ݷ��������к�  
*/
typedef struct ncDbmonSeqInfo_s {
	unsigned long  lSip;         /* Դ��ַ          */ 
	unsigned long  lDip;         /* Ŀ���ַ        */
	unsigned short nSport;       /* Դ�˿�          */
	unsigned short nDport;       /* Ŀ��˿�        */
	
	unsigned long  lSid;         /* Session Id     */
	unsigned long  lSeq;         /* �������к�     */
	unsigned short nService;     /* �������       */
	unsigned char  cMark;        /* ��ʶ 1--AuthRequest  2--AuthResponse   */
	unsigned char  cFun;         /* �ӹ���          */ 
	unsigned long  lLastTime;    /* ������ʱ��    */
	unsigned char  caUser[32];       // �û���
	unsigned char  caDbname[32];     // ���ݿ���
	unsigned long  lStartTime;       // ��ʼʱ��
	unsigned long  long lBytes[2];   // ���� 0--����  1--����
    unsigned long  lAckSeq;          // ����������к�
	unsigned char  caRev[8];         // ����  caRev[0] = k
} ncDbmonSeqInfo;



#define NC_ATTFILE_ID  19651129L
typedef struct ncPkgAttFile_s {
    unsigned long  lId;
    unsigned long  lSid;
    unsigned long  lUserid;
    unsigned long  lSip;
    unsigned long  lDip;
    unsigned short nSport;
    unsigned short nDport;
    unsigned char  caFile[128];
    unsigned char  caFrom[64];
    unsigned char  caTo[64];
    unsigned long  lSize;
} ncPkgAttFile;
    

typedef struct ncMsnP2PHead_s {
        unsigned long lId;   /* Channel Session Id */
        unsigned long lSid;
        unsigned long long lOffset;
        unsigned long long lTotal;
        unsigned long lMsgSize;
        unsigned long lFlags;
        unsigned long lAckId;
        unsigned long lAckUid;
        unsigned long long lAckSize;
} ncMsnP2PHead;
        
typedef struct ncPkgIpList_s {
    unsigned long   lLip;               /* IP��ַ����           */
    unsigned long   lUip;               /* IP��ַ����           */
} ncPkgIpList;

typedef struct ncPkgPortList_s {
    unsigned long   lLport;               /* �˿�����           */
    unsigned long   lUport;               /* �˿�����           */
} ncPkgPortList;


/* ���ݰ�����   */
typedef struct ncPkgFilter_s {
    int             iProtocol;    /* 0-- ȫ��  6--Tcp  17--Udp  */
    unsigned short  nSumSip;
    unsigned short  nMaxSip;
    unsigned short  nSumDip;
    unsigned short  nMaxDip;
    unsigned short  nSumSport;
    unsigned short  nMaxSport;
    unsigned short  nSumDport;
    unsigned short  nMaxDport;
    ncPkgIpList     *psSip;
    ncPkgIpList     *psDip;
    ncPkgPortList   *psSport;
    ncPkgPortList   *psDport;
} ncPkgFilter;

/*  ��·���   
    ץ������
 */
typedef struct ncPkgCapFilter_s {
    unsigned long   lDid;              /* �豸���     */
    unsigned long   lLip;              /* ��ʼIP��ַ   */
    unsigned long   lUip;              /* ��ֹIP��ַ   */
    unsigned short  nLport;            /* ��ʼ�˿�     */
    unsigned short  nUport;            /* ��ֹ�˿�     */
    unsigned char   cProt;             /* Э�� 0-- ALL  6 Tcp  17 Udp  */
    unsigned char   cFlags;            /* 0--�����   1--���          */
    unsigned char   cRev[2];           /* ����         */
} ncPkgCapFilter;


typedef struct ncPkgCapDev_s {
    unsigned long lSid;
    char  caName[32];                  /* ˵��       */
    char  caDev[64];                   /* �豸��     */
    int   iType;                       /* ץ����ʽ   */
    int   iStart;                      /* �� Filter�е���ʼλ��, From 0  */
    int   iSum;                        /* ��¼��                         */
    int   iLine;                       /* �������е����                 */
    int   iPromisc;                    /* ץ��ģʽ                       */
    ncPkgCapFilter *psFilter;
} ncPkgCapDev;

typedef struct ncPkgCapDevHead_s {
    int iSumDev;
    int iSumFilter;
    ncPkgCapDev     *psDev;
    ncPkgCapFilter  *psFilter;
} ncPkgCapDevHead;

typedef struct ncPkgBaseVar_s {
    unsigned short  nOff;             // ��ʼλ��
    unsigned char   cType;            // ����  1-String,��0��β  2-String, ָ������  
    unsigned char   cLen;             // ����
} ncPkgBaseVar;

typedef struct ncPkgBase001_s {
    unsigned long   lId;              // Service Id
    unsigned short  nFun;             // ���ܴ���
    unsigned char   cProc;            // ������  0--��ֹ  >0 ��������
    unsigned char   cFlags;           // ���ȴ�����  0-���жϳ���   1-��󳤶�   2-�̶�����  3-һ���ֽڳ���  4-�����ֽڳ���
    unsigned short  nBase;            // �̶�����
    unsigned short  nLen[2];          // ��������
    unsigned char   cSum;             // ƥ������
    unsigned char   cSumV;            // ��������
    unsigned short  nOff[16];         // ƫ����
    unsigned char   cCont[16];        // ����
    ncPkgBaseVar    sVar[4];
} ncPkgBase001;


typedef struct ncTagAttr_s {
    unsigned char name[32];
    unsigned char value[128];
} ncTagAttr;


typedef struct ncFlowInfo_s {
    unsigned long lLongTime;            // ����ʱ��,��
    unsigned long lConTime;             // ����ʱ��,����
    unsigned long long  lBytes[2];      // 0--����  1--����
    unsigned long       lPkg[2];        // ���ݰ�
}ncFlowInfo;

typedef struct ncDevFlowInfo_s {
    unsigned char caName[16];            // �豸��
    int                 sid;
    ncFlowInfo          sBase;           // ��׼ֵ����ŵ��Ǿ���ֵ
    ncFlowInfo          *psFlow;         // ���ɼ�������ݣ���ŵ�������
}ncDevFlowInfo;

/* ����ӿ�ʵʱ��������   

*/
typedef struct ncFlowInfoHead_s {
    unsigned short nSumDev;       // �豸����
    unsigned short nMaxFlow;
    unsigned short nTimeStep;     // ȡ��ʱ����,��λΪ��
    unsigned short nCurRead;      // ��ǰ���ļ�¼
    unsigned short nCurWrite;     // ���д��ļ�¼
    unsigned char  cLock;         // 0--����  1--����
    unsigned char  cRev;
    ncDevFlowInfo  *psDev;
}ncFlowInfoHead;


/*
       GbComCapWap ��ʽ
       SRC:  61000   DST: 61000
       ����:  (46���ֽ�)
         4    int L2W DstIp
         4    int L2W SrcIP
         4    int L2W ID
         2    short   Len
         2    OffSet
         1   Df
         1   Lf
         6   L2wRsvPap
         6   VdevDstMac
         6   VdevSrcMac
         2   VdevProto
         1   VdevPriority
         1   VdevRsv
         4   VdevPortMap
         2   VdevPap
*/

typedef struct ncGbcomCapWapData_s {
    uint4 L2wDstIp;
    uint4 L2wSrcIp;
    uint4 L2wId;
    uint2 L2wLen;
    uint2 L2wOffset;
    uchar L2wDf;
    uchar L2wLf;
    uchar   L2wRsvPap[6];
    uchar   VdevDstMac[6];
    uchar   VdevSrcMac[6];
    uint2   VdevProto[2];
    uchar      VdevPriority;
    uchar      VdevRsv;
    uchar   VdevPortMap[4];
    uchar   VdevPap[2];
} ncGbcomCapWapData;


/* NSA  NPCP���ݽṹ   
     cmd--���Ĳ���
     len--���ĳ���
     epif--�������ӿ�
     ipif--��������ӿ�
     qoslv1id--һ��QOSID
     qoslv0id--0��QOSID
     cause_id---�Ͻ�ԭ��
     
 */
typedef struct ncNpcpHead_s {
    u_int64_t pktprc:8, pktinf:8, rsv0:4, pkttype:4, type:8, qospri:4, rsv1:2, qoscolor:2,
                  epif:3, rsv2:1, ipif:3, rsv3:1, len:11, rsv4:1, drop:1, host:1, mirror:1, rsv12:1;
    u_int64_t qoslv1id:13, rsv5:3, qoslv0id:10,rsv6:6, ethtype:16, elif:12, rsv7:4;
    u_int64_t acl_id:16, GET:1, rsv8:7, cause_id:8, ssn_id:24, rsv9:4,
                  acl_avl:1, ssn_avl:1, rsv10:2;
    u_int64_t rsv11:32, nsa_tx:16, host_tx:16;    
} ncNpcpHead;

#define SSN_HASH_BUCKET_SIZE            0x400000
#define SSN_HASH_BUCKET_SIZE_BITS   22
#define SSN_HASH_BUCKET_BITS        2
#define SSN_HASH_VC_NUMS1_BITS0                 0
#define SSN_HASH_VC_NUMS2_BITS1                 1
#define SSN_HASH_VC_NUMS4_BITS2                 2
#define SSN_HASH_VC_NUMS8_BITS3                 3

#define SSN_HASH_VC_NUMS1                               1
#define SSN_HASH_VC_NUMS2                               2
#define SSN_HASH_VC_NUMS4                               4
#define SSN_HASH_VC_NUMS8                               8

#define SSN_HASH_VC_NUMS1_MASK                  (SSN_HASH_VC_NUMS1-1)
#define SSN_HASH_VC_NUMS2_MASK                  (SSN_HASH_VC_NUMS2-1)
#define SSN_HASH_VC_NUMS4_MASK                  (SSN_HASH_VC_NUMS4-1)
#define SSN_HASH_VC_NUMS8_MASK                  (SSN_HASH_VC_NUMS8-1)


static inline int ncSsnVcid(int pos, int vc_num)
{
    switch(vc_num) {
                case SSN_HASH_VC_NUMS1:
                        break;
                case SSN_HASH_VC_NUMS2:
                        return (pos)>>(SSN_HASH_BUCKET_SIZE_BITS+SSN_HASH_BUCKET_BITS-SSN_HASH_VC_NUMS2_BITS1);
                        break;
                case SSN_HASH_VC_NUMS4:
                        return (pos)>>(SSN_HASH_BUCKET_SIZE_BITS+SSN_HASH_BUCKET_BITS-SSN_HASH_VC_NUMS4_BITS2);
                        break;
                case SSN_HASH_VC_NUMS8:
                        return (pos)>>(SSN_HASH_BUCKET_SIZE_BITS+SSN_HASH_BUCKET_BITS-SSN_HASH_VC_NUMS8_BITS3);
                        break;
                default:
                        break;
    }
    return 0;
}


#endif
