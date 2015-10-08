#ifndef __TC_DEF__
#define __TC_DEF__             1
#define TC_DOWNLOAD_HAND     10
#define TC_UPLOAD_HAND       20
#define TC_CTL_ALL           3
#define TC_CTL_UPLOAD        1
#define TC_CTL_DOWNLOAD      2
typedef struct ncTcClass_s {
	char     caName[16];
	unsigned long  lURate;              /* ������kbΪ��λ  */
	unsigned long  lDRate;              /* ������kbΪ��λ  */
	char     cUShare;                   /* 0--Isolated 1--Sharing */
	char     cUBorrow;                  /* 0--bounded  1--borrow  */
	char     cDShare;                   /* 0--Isolated 1--Sharing */
	char     cDBorrow;                  /* 0--bounded  1--borrow  */
	unsigned short nPrio; 
	unsigned long lId;
} ncTcClass;


/* 2007/06/15 �޸�����ʱ���   */
typedef struct ncTcFilter_s {
	unsigned long  lSip;         // ԴIP��ַ        
	unsigned long  lDip;         // Ŀ��IP
	unsigned short nSport;       // Դ�˿� 
	unsigned short nDport;       // Ŀ��˿�
	unsigned short nProtocol; 
	unsigned char  cSmask;
	unsigned char  cDmask;
	unsigned short nPrio;
	unsigned short nNum;
	unsigned long  lURate;
	unsigned long  lDRate;
	unsigned long  lId;
	unsigned long  lLastTime;
	char     caName[16];         /* �������                      */
} ncTcFilter;


typedef struct ncTcUser_s {
    unsigned long  utype;
    unsigned long  id;
    unsigned long  dservice;
    unsigned long  dip;
    unsigned long  dmask;
    unsigned short prio;
    unsigned short num;
    unsigned long  urate;
    unsigned long  drate;
    unsigned long  dateid;
    unsigned long  stime;
    unsigned long  etime;
    int            status;    // 0-- No  1-Active
    char           cname[16];
} ncTcUser;


typedef struct ncTcIp_s {
    unsigned long  sip;
    unsigned long  smask;
    unsigned short sport;
    unsigned short dport;
    unsigned long  dip;
    unsigned long  dmask;
    unsigned long  protocol;
    unsigned short prio;
    unsigned short num; 
    unsigned long  urate;
    unsigned long  drate;
    unsigned long  dateid;
    unsigned long  stime;
    unsigned long  etime;
    int            status;
    char           cname[16];
} ncTcIp;


/* ���ʿ��ƴ���   */
typedef struct ncTcLim_s {
	unsigned long  lSid;         /* Sid                           */
	unsigned long  lURate;
	unsigned long  lDRate;
	char     caName[16];         /* �������                      */
} ncTcLim;

typedef struct ncTcHead_s {
	char  caIndev[16];
	char  caOutdev[16];
	unsigned long lUBand;
	unsigned long lURate;
	unsigned long lDBand;
	unsigned long lDRate;
	unsigned long lSumClassId;
	unsigned long lSumFilterId;
	unsigned long lFilterOffset;
	unsigned long lUserOffset;
	unsigned long lIpOffset;
	unsigned long lLimOffset;
	unsigned long lSumUser;
	unsigned long lSumIp;
	unsigned long lSumLim;
	int      iControl;               /*  0--ȫ��  1--����  2--����   */
	char    *psClass;
	char    *psFilter;
	ncTcUser *psTcUser;
	ncTcIp   *psTcIp;
	ncTcLim  *psTcLim;
	int      sNum[16];
} ncTcHead;


#endif
