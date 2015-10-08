/* Last Update Time:2009/07/02  */
#ifndef __NCHASH_API_H
#define __NCHASH_API_H
#define META_MAX_CONFLICT    32
#define META_ACL_AUTH2    5      // ��Ԫ�����֤   
#define META_ACL_AUTH     4
#define META_ACL_MARK     3       // MarkIDΪQOSID��ֵ
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
    unsigned short  nRev;               /* �������Mark  */
//    unsigned long   lRev;
} ncProIptIp;


typedef struct ncMetaStatus_s {
	unsigned long lStatus; 	//�����ڴ��Ƿ��ʼ�����  0x676f6f64UL   ��ʼ�����  0x6675636bUL   ���ڳ�ʼ��  0 ����״̬�������ڴ�δ����ʼ��
	unsigned long lSize;	// �����ڴ��С����λΪByte
	unsigned long lVersion; 	// ��ǰInit�����ڴ�Ľ��̰汾��
	unsigned long lFlags; 	// Ĭ��opts����
	unsigned long lMark; 	// Ĭ��QosID
	unsigned long lReport;	// Ĭ���ض���˿�
	unsigned long lPercent; 	// hash�ռ���ռ�����ڴ�ٷֱ�
	unsigned long lAging; 	// Ĭ�ϳ�ʱʱ��
	unsigned long lHashSize; // Hash�ռ�����
	unsigned long lHashCount;	// Hash �ռ������м�¼��
	unsigned long lClliSize;		// ��ͻ������ 
	unsigned long lClliCount;	// ��ͻ�����м�¼��
	unsigned long lOffset;	//��ͻ��βָ��
	unsigned long lCurrTime;	//��ǰʱ����jiffiesֵ����λΪ��
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
