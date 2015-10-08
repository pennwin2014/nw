#ifndef  __MSNDEF__
#define  __MSNDEF__          1
#define MSN_PORT        1863
#define QUEUELEN        1000
#define	MSN_BUFF_SIZE	8192
typedef struct msn_data_buff{
	unsigned long  ip_src;
	unsigned long  ip_dest;
	unsigned short port_src;
	unsigned short port_dest;
	unsigned long datalen;
	char data[1500];
} MSN_DATA_BUF;

typedef struct msn_data_head{
    unsigned long  lCurPutPos;
    unsigned long  lCurGetPos;
    unsigned long  lMaxLen;
    unsigned long  lReserve;
} MSN_DATA_HEAD;


typedef struct ncMsnPkg_s {
        unsigned long  lSip;       /* ԴIP��ַ     */
        unsigned long  lDip;       /* Ŀ��IP��ַ   */
        unsigned short nDport;     /* Ŀ��˿�     */
        unsigned short nSport;     /* Դ�˿�       */
        unsigned long  lLen;       /* ���ݰ�����   */
} ncMsnPkg;


typedef struct conv{
	int            iTotal;
	int            iRecv;
	int            iUsed;
	long           lLasetUsed;
	unsigned long  ulSrcIp;
	unsigned short usSrcPort;
	unsigned long  ulDstIp;
	unsigned short usDstPort;
	char *szBuf;
//	char szBuf[MSN_BUFF_SIZE];
}CONVMSG;

typedef struct msn_user{
	int            iUser;
	int            iFriend;
	unsigned long  ulSrcIp;
	unsigned short usSrcPort;
	unsigned long  ulDstIp;
	unsigned short usDstPort;
	char szUser[50];
	char szFriend[50];
	long lLastUsed;
} MSNUSER;

#endif