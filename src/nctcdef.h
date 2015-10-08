#ifndef __TC_DEF__
#define __TC_DEF__             1
#define TC_DOWNLOAD_HAND     10
#define TC_UPLOAD_HAND       20
#define TC_CTL_ALL           3
#define TC_CTL_UPLOAD        1
#define TC_CTL_DOWNLOAD      2
typedef struct ncTcClass_s {
	char     caName[16];
	unsigned long  lURate;              /* 带宽，以kb为单位  */
	unsigned long  lDRate;              /* 带宽，以kb为单位  */
	char     cUShare;                   /* 0--Isolated 1--Sharing */
	char     cUBorrow;                  /* 0--bounded  1--borrow  */
	char     cDShare;                   /* 0--Isolated 1--Sharing */
	char     cDBorrow;                  /* 0--bounded  1--borrow  */
	unsigned short nPrio; 
	unsigned long lId;
} ncTcClass;


/* 2007/06/15 修改增加时间段   */
typedef struct ncTcFilter_s {
	unsigned long  lSip;         // 源IP地址        
	unsigned long  lDip;         // 目标IP
	unsigned short nSport;       // 源端口 
	unsigned short nDport;       // 目标端口
	unsigned short nProtocol; 
	unsigned char  cSmask;
	unsigned char  cDmask;
	unsigned short nPrio;
	unsigned short nNum;
	unsigned long  lURate;
	unsigned long  lDRate;
	unsigned long  lId;
	unsigned long  lLastTime;
	char     caName[16];         /* 类别名称                      */
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


/* 访问控制带宽   */
typedef struct ncTcLim_s {
	unsigned long  lSid;         /* Sid                           */
	unsigned long  lURate;
	unsigned long  lDRate;
	char     caName[16];         /* 类别名称                      */
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
	int      iControl;               /*  0--全部  1--下行  2--上行   */
	char    *psClass;
	char    *psFilter;
	ncTcUser *psTcUser;
	ncTcIp   *psTcIp;
	ncTcLim  *psTcLim;
	int      sNum[16];
} ncTcHead;


#endif
