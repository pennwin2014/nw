/* Last Update Time:2009/07/02  */
#ifndef __NCHASH_API_H
#define __NCHASH_API_H
#define META_MAX_CONFLICT    32
#define META_ACL_AUTH2    5      // 两元祖的验证   
#define META_ACL_AUTH     4
#define META_ACL_MARK     3       // MarkID为QOSID的值
#define META_ACL_DROP     2
#define META_ACL_ACCEPT   1
#define META_MAX_TIME     65535
typedef struct ncProIptIp_s {
    unsigned long lSip;
    unsigned long lDip;
    unsigned short nSport;
    unsigned short nDport;
    union 
    {
	    unsigned long lFlags;
	    struct Actions_s
	    {
		    unsigned long cPort:8;	    /* 6--Tcp 17--Udp */
		    unsigned long opts:3;	
		    unsigned long timeout:5;    /* no use */
		    unsigned long qosid:16;	    /* QOSID */
	    } a;
    } b;
    unsigned short  nTime;
    unsigned short  nRev;               /* 反方向的Mark  */
//    unsigned long   lRev;
} ncProIptIp;


typedef struct ncMetaStatus_s {
	unsigned long lStatus; 	//共享内存是否初始化完成  0x676f6f64UL   初始化完成  0x6675636bUL   正在初始化  0 置零状态，共享内存未被初始化
	unsigned long lSize;	// 共享内存大小，单位为Byte
	unsigned long lVersion; 	// 当前Init共享内存的进程版本号
	unsigned long lFlags; 	// 默认opts策略
	unsigned long lMark; 	// 默认QosID
	unsigned long lReport;	// 默认重定向端口
	unsigned long lPercent; 	// hash空间所占共享内存百分比
	unsigned long lAging; 	// 默认超时时间
	unsigned long lHashSize; // Hash空间容量
	unsigned long lHashCount;	// Hash 空间中现有记录数
	unsigned long lClliSize;		// 冲突区容量 
	unsigned long lClliCount;	// 冲突区现有记录数
	unsigned long lOffset;	//冲突区尾指针
	unsigned long lCurrTime;	//当前时间点的jiffies值，单位为秒
} ncMetaStatus;

typedef struct ncMetaTimeOut_s {
    unsigned short nThisTime;
    unsigned short nTimeOut;
    unsigned char *pTcpHash;
    char     caFile[32];
} ncMetaTimeOut;


/* the apis */

int (*ncProIptGetSysInfo) (unsigned int *iVersion, unsigned int *iBufSize, unsigned int *iUseRate);
int (*ncProIptBufInit) (unsigned int iUseRate, unsigned int iDefault, unsigned int ID);
int (*ncProIptSetIpAcl) (ncProIptIp *psIp);
int (*ncProIptGetIpAcl) (ncProIptIp *psIp);
int (*ncProIptDelIpAcl) (ncProIptIp *psIp, unsigned long lFlags);
int (*ncProIptDoIpAcl) (char *pHead,
      int(*fDoFun)(char *pHead, void *elem));
int (*UserDel)  (void *elem);
unsigned short (*UserGetJiffies) (void);
void (*ncProIptEchoArgsAcl) (ncMetaStatus *ncAS);      
#endif /*__HASH_API_H*/
