
/* nMark �ĺ���  */
#define NC_FTP_PORTINFO   141
#define NC_FTP_PASVINFO   142

/* cFun �ĺ���     */
#define NC_FTP_PORTMARK   143   /* ����ͨ��IP����  */
#define NC_FTP_PASVMARK   144   /* ����ͨ��IP����  */
#define NC_FTP_COMMMARK   145   /* ����ͨ��TCP����  */

#define NC_FTP_COMM       146     /* ����      */ 
#define NC_FTP_DATA       147     /* һ������  */
#define NC_FTP_PORTDOWN   148     /* �����ļ�  */
#define NC_FTP_PORTUP     149     /* �ϴ��ļ�  */
#define NC_FTP_PASVDOWN   150     /* �����ļ�  */
#define NC_FTP_PASVUP     151     /* �ϴ��ļ�  */


/* ������Ϣ  */
typedef struct ncPkgSeqInfo_s {
	unsigned long  lSid;         /* Session Id     */
	unsigned long  lSeq;         /* ��ʼ���к�     */
	unsigned short nCount;       /* Dport          */
	unsigned char  nMark;        /* ��ʶ 0--������  1--������        */
	unsigned char  cFun;         /* �ӹ���,�������涨��ĺ�          */ 
	unsigned long  lLastTime;    /* ������ʱ��    */
	unsigned long  lLen;         /* �û�ID    */ 
	unsigned long  lSid1;        /* Sip       */
	unsigned long  lSid2;        /* Dip       */
	unsigned char  caFrom[48];   /* ftp��¼���û���                  */
	unsigned char  caTo[48];     /*   ����ͨ��: Ip��ַ�Ͷ˿� 192.168.0.1:5001
	                                  ����ͨ�������͵��ļ���                     */
} ncPkgSeqInfo;


/* �������ݽ���������  */
psPkgSen

typedef struct ncPkgSession_s {  
    unsigned long    lSid;       /* ͬ tcp��sid, �Ự�ı�ʶ  */
	unsigned long    lSeq;       /* ���ݰ������к�   */
	unsigned long    lSip;
	unsigned long    lDip;
	unsigned short   nSport;
	unsigned short   nDport;
	unsigned short   nLen;       /* ���ݰ��ĳ���    */
	unsigned char    cMark;
	unsigned char    cDict;      /* ���ݰ����� 0--����  1--Ӧ��  */
	unsigned char    pBuf[1600];    /* ���ݰ�������    */
} ncPkgSession; 



psLog 
typedef struct ncPkgLog_s { /* Log�Ľṹ  */
    unsigned char  cFlags;       /* 0--��  1--��  9--����      */
    unsigned char  cType;        /* 1--�쳣����   2--�Ự����  */
    unsigned short nLen;         /* ���ݰ��ĳ���               */
	unsigned long  nService;     /* ��������                   */
	unsigned long  nFun;         /* FunCode                    */
    unsigned long  lPos;         /* λ�ã���ʵ�ʵ����ֱ���һ�� */
    unsigned long  lTime;        /* ʱ��     */
    unsigned long  lUserid;      /* �û�ID   */
} ncPkgLog;

/* �˴���Դ��ַΪ�ͻ��˵�ַ  */
typedef struct ncIp_s {
    unsigned long  lSip;             /* �ڲ�IP��ַ                    */
    unsigned long  lDip;             /* �ⲿIP��ַ                    */
    unsigned short nDport;           /* Ŀ��˿ں�                    */
    unsigned short nSport;           /* Դ�˿�, ȱʡ�����Ϊ 0, ֻ�е������ʱ���Ϊ�� 0  */
    unsigned char  cType;            /* Э��    6--TCP 17 UDP         */
    unsigned char  cMark;            /* ��ʶ, 143 -- p ����ͨ����IP����ָ�� (����)
                                              144 -- p ����ͨ����Ӧ�� TCPָ�� (����)
                                      */    
    unsigned char  cFileMark;        /* cFun  ������ΪFTPʱ�����ݼ� cFun��       */
    unsigned char  cRev;             /* ����  */
    unsigned long  lStartTime;       /* ��ʼʱ��                     */
    unsigned long  lLastTime;        /* ����޸�ʱ��                 */
    unsigned long  lStop;            /* ���ʿ��Ƽ��ʱ�� hhmmss�� ����ֵΪ0��ʾʹ�ó�ʱʱ��  */
    long8    lBytes[2];              /* 0-�����ֽ���  1-�����ֽ���   */
    unsigned char  cStatus;          /* 0--��   1--����  2--����     */
    unsigned char  cUseFlags;        /* ʹ�ñ�ʶ 0--û���� 1--������ 2--�Ѵ��� 3--�ѱ���   */
    unsigned char  caMac[6];         /* ԴMac��ַ                    */
    ncUserInfo     *psUser;          /* �û���Ϣ����Ϊ�ձ�ʾ�û���Ϣ����    */
    unsigned long  lService;         /* ����                                */
    unsigned long  lUrl;             /* ��Ӧ����Sid                         */
    unsigned long  lPreService;      /* Ԥ����Э��                          */
    unsigned char  cProService[8];   /* Э���ʶ,����Э��������ķ���       */
    char    *p;                      /* ����ָ��, ��Ӧ����TCP����ָ��       */
} ncIp;

typedef struct ncTcp_s {
    unsigned long  lSip;              /* �ڲ�IP��ַ                   */
    unsigned long  lDip;              /* �ⲿIP��ַ                   */
    unsigned short nDport;            /* Ŀ��˿ں�                   */
    unsigned short nSport;            /* Դ�˿ں�                     */
    unsigned long  lStartTime;        /* ��ʼʱ��                     */
    unsigned long  lLastTime;         /* ����޸�ʱ��                 */
    unsigned char  cConnect;          /* ����״̬  1 Syn  2 ����  9 Close   */
    unsigned char  cDiction;          /* ���� 0 ����   1 ����               */
    unsigned char  cStatus;           /*      0 δ֪   1 ����      2 ����                   */
    unsigned char  cProt;             /* Э��   6-TCP  17 UDP                               */
    unsigned char  cFlags;            /*        143 -- p ����ͨ����IP����ָ�� (����)        */
    unsigned char  cProxy;            /* 0 -- ����   1 -- ����Proxy��Э��   */
    unsigned char  cRev[2];
    ncIp           *psIp;             /* ��Ӧ��IP��Ϣ,NULL ��ʾ�����Ӳ����� */
    unsigned long  lStop;             /* �������õ���ֹʱ��                 */
    unsigned long  lSeq;              /* �������к�                          */
    unsigned long  lAck_seq;          /* �������к�                          */
    unsigned long  lSid;              /* Session ID                          */
    unsigned long  lUrl;              /* Url���                             */
    char *p;                          /* ����ָ��                            */
} ncTcp;
