/*  �йؼ��ٿ��Ŀ���               */
/*  2009/12/05   ֧��NSA4          */
/*  2009/6/24    �����޸�          */
/*  2008/12/10   By Liyunming      */
#ifndef __NCNSADEF__
#define __NCNSADEF__
#define NC_NSA_VERSION     "NSA5.01.03"
#define NC_NSA_RELEASE     "2012/12/03"

#define NC_NSA_MAXNSAID     8

#define NC_NSA_OPT_FWD          0x40
#define NC_NSA_OPT_FWD_MIRROR   0x60
#define NC_NSA_OPT_HOST         0x10
#define NC_NSA_OPT_DROP         0x08


/* SSN������Ϣ      */
typedef struct ncNsaSsnBuf_s {	/* ��40���ֽ�              */
	unsigned long  lSip;	/* ԴIP��ַ              */
	unsigned long  lDip;	/* Ŀ��IP��ַ            */
	unsigned short nSport;	/* Դ�˿�                */
	unsigned short nDport;	/* Ŀ��˿�              */
	char cProt;		        /* Э��    0--����  6--TCP   17--UDP    */
	char cMirror;		    /* 0-������   1-����           */
	char cDrop;		        /* 0-������   1-����           */
	char cDev_s;	     	/* �������ݰ��Ľӿںţ�����,SPI�ӿ�ID+1         */
	char cDev_r;	     	/* �������ݰ��Ľӿںţ�����,SPI�ӿ�ID+1         */
	char way;
	char vc_id;
	char cFrom;             /*  1--���Խ�������                 */
	unsigned long qoslv1id_s;	/* 1��lLevel����     ����      */
	unsigned long qoslv2id_s;	/* 2��lLevel����               */
	unsigned long qoslv1id_r;	/* 1��lLevel����     ����      */
	unsigned long qoslv2id_r;	/* 2��lLevel����               */
	long lTtl;		/* �ϻ�ʱ���룬��Ϊ 0���޸ģ�ת��Ϊssn�ڲ���λ /30   */
} ncNsaSsnBuf;


/* QOS������Ϣ

 */
typedef struct ncNsaQosBuf_s {
	unsigned long lLevel;	/* lLevel                      */
	unsigned long lQosId;	/* QOS  ID                     */
	unsigned long lMax;	/* ���������ƣ�ÿ����λ8K�ֽ� */
	unsigned long lMin;	/* ��С������ QOS               */
	unsigned char cPrio;	/* ���ȼ�,��ʱ����              */
	unsigned char cFrom;    /* ������Դ                     */
	unsigned char cRev[18];	/* ����                         */
	unsigned long lPrevQosId;
} ncNsaQosBuf;

/* CrossLink ������Ϣ   */
typedef struct ncNsaCrossLink_s {
	int iInDev;		    /* ����ID    */
	int iOutDev;		/* ����ID    */
	int iOpt;           /* ����      */
	int iUp;
	int iMirror;
	char cFrom;         /* ������Դ   */
	char cRev[19];		/* ����      */
} ncNasCrossLink;


/* �˿��ض���, ��ϵͳֻ֧�ֽ��˿��ض���ָ����IP��ַ�Ͷ˿�   */
typedef struct ncNsaRedir_s {
	unsigned long  lSip;	/* ԴIP��ַ              */
	unsigned long  lDip;	/* Ŀ��IP��ַ            */
	unsigned short nSport;	/* Դ�˿�                */
	unsigned short nDport;	/* Ŀ��˿�              */
	char cProt;		        /* Э��    0--����  6--TCP   17--UDP    */
	char cMirror;		    /* 0-������   1-����           */
	char cDrop;		        /* 0-������   1-����           */
	char cDev_s;	        /* �������ݰ��Ľӿںţ�����,SPI�ӿ�ID+1                           */
	char cDev_r;	        /* �������ݰ��Ľӿںţ�����,SPI�ӿ�ID+1                           */
	char cOpt;              /* 0-�޸�Դ�˿ں͵�ַ  1-�޸�Ŀ��˿ں͵�ַ     */
	unsigned short nMport;  /* �޸ĺ�Ķ˿�   */
	unsigned long  lMip;    /* �޸ĺ�ĵ�ַ   */
	unsigned char cFrom;    /* ������Դ       */
	unsigned cRev[15];      /* ����           */
} ncNsaRedir;


/* �ؼ���   */
typedef struct ncNsaPatBuf_s {
	unsigned short scop;	/* Scop              */
	unsigned short id;	    /* Id                 */
	unsigned short len;	    /* �ؼ��ֳ���         */
	unsigned char  Rev[2];	/* Id              */
	unsigned char  patstr[32];
} ncNsaPatBuf;

/* �ؼ��ֹ���  */
typedef struct ncNsaPatRule_s {
	unsigned char  rule_type;	/* Scop              */
	unsigned char  protocol;	    /* Id                 */
	unsigned char  action;	    /* �ؼ��ֳ���         */
	unsigned char  cRev;	/* Id              */
	unsigned long  lSip;	/* Id              */
	unsigned long  lDip;	/* Id              */
	unsigned short nSport;	/* Id              */
	unsigned short nDport;	/* Id              */
	unsigned short nScop;	/* Id              */
	unsigned short nId;	    /* Id              */
	unsigned char  caRev[20];
} ncNsaPatRule;


/* cCom�ĺ���    */
#define NC_NSA_SETQOS       1	/* ���û����QOS */
#define NC_NSA_DELQOS       2	/* ɾ��QOS       */
#define NC_NSA_RESETQOS     3	/* ����QOS       */
#define NC_NSA_SETSSN       4	/* ����SSN��     */
#define NC_NSA_DELSSN       5	/* ɾ��SSN��     */
#define NC_NSA_RESETSSN     6	/* ����SSN��     */
#define NC_NSA_SETCROSS     7	/* ���ý�������  */
#define NC_NSA_SETREDIR     8   /* �˿��ض���    */
#define NC_NSA_DELREDIR     9   /* �˿��ض���    */
#define NC_NSA_SHOWQOS      10   /* �˿��ض���    */
#define NC_NSA_SETSSNMIRROR 11	/* ����SSN��     */
#define NC_NSA_SHOWSSN      12
#define NC_NSA_SHOWQOSID    13  /* ��ʾָ��QosID������  */
#define NC_NSA_SETDEVUP     14  /* ���������豸  */
#define NC_NSA_SHOWDEV      15  /* ��ʾ����״̬   */
#define NC_NSA_SETPAT       16  /* ���ùؼ���     */
#define NC_NSA_COMPPAT      17  /* ����ؼ���      */
#define NC_NSA_SETPATRULE   18  /* ���ùؼ��ֹ���  */
#define NC_NSA_DELPAT       19  /* ���ùؼ��ֹ���  */
#define NC_NSA_SHOWKEYLOG   20  /* ��ʾ�ؼ�����־  */
#define NC_NSA_SHOWPKG      21  /* �鿴���ݰ�״��  */
#define NC_NSA_SYNPKG       22  /* ͬ�����ݰ�      */


/* �����ڴ�ṹ   */
typedef struct ncNsaComBuf_s {
	unsigned char  cCom;	    /* ����������涨��ĺ�           */
	unsigned char  cUse;	    /* 0--����  1--����   2--���ڲ���   */
	unsigned short nLen;	    /* ��¼����                         */
	unsigned long  lCheckSum;	/* У���룬��鱾��¼����Ч��       */
	union {
		ncNsaSsnBuf ssn;
		ncNsaQosBuf qos;
		ncNasCrossLink cross;
		ncNsaRedir     redir;
		ncNsaPatBuf    pat;
		ncNsaPatRule   prule;
		char buf[40];
	} s;
} ncNsaComBuf;

typedef struct ncNsaComHead_s {
	int iLock;		            /* 0--����  1--���ڲ���        */
	unsigned long lMax;	        /* ����¼��                     */
	unsigned long lCur;	        /* ��ǰ����λ��,ץ�����̸��µ�    */
	unsigned long lDo;	        /* ���ڴ����¼    */
	unsigned long lStartTime;	/* ����ʱ��        */
	unsigned long lLastTime;	/* ����޸�ʱ��    */
	unsigned char cStartMark;   /* ����ʱֵΪ 0, һ����ʼ�����󽫱�Ϊ 1  */
	unsigned char cRev[3];
} ncNsaComHead;

typedef struct ncNsaCom_s {
	ncNsaComHead *psHead;
	ncNsaComBuf  *psBuf;
} ncNsaCom;

/* ����  */
typedef struct ncNsaComBufIndex_s {
	int nsa_id;
	unsigned long lSum;
	unsigned long lPos;
} ncNsaComBufIndex;

/* ACL���Ʊ�   */
typedef struct ncNsaAclSsn_s {
	ncNsaSsnBuf ssn;
	struct ncNsaAclSsn_s *next;
} ncNsaAclSsn;

/* ����ָ��    */
typedef struct ncNsaDataBuf_s {
    ncNsaComBuf   *psBuf;
    char          *pHead;
} ncNsaDataBuf;

#endif
