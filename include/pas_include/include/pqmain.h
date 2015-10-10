#ifndef __PQSMAINDEF____
#define __PQSMAINDEF____
#ifdef	_WIN32
#include "utotype.h"
#else
#include "utoall.h"
#endif

#define PQ_MSG_CODE         990118
#define PQ_MAXTCPFUN        50
#define PQ_MAXUDPFUN        50

#define PQ_CODE_REQUEST      990118101
#define PQ_CODE_RESPONSE     990118102
#define PQ_CODE_ANNOUNCE     990118103
#define PQ_CODE_CONFIRM      990118104



#define PQ_STA_OFFLINE      0     /* ����            */
#define PQ_STA_ONLINE       1     /* ����            */
#define PQ_STA_AWAY         2     /* �߿�            */
#define PQ_STA_QUIET        3     /* �����          */

#define PQ_SHA_UDPPORT      11    /* UDP�˿ں�       */
#define PQ_SHA_TCPPORT      12    /* TCP�˿ں�       */
#define PQ_SHA_TCPTIME      13    /* TCP��ʱʱ��     */



#define PQ_LNK_PICQRESERVE  11    /* ����PICQ����    */
#define PQ_LNK_ACTIVEPICQ   12    /* Picq��ǰ��Ϣ��  */
#define PQ_LNK_PICQNUMBER   13    /* Ҫ�����PICQ�����  */
#define PQ_LNK_HOSTLIST     14    /* �������б�      */
#define PQ_LNK_NETMASK      15    /* �����ʶ        */
#define PQ_LNK_ADVERTISING  16    /* �����Ϣ        */


/* ������Ϣ�����̶�  */
#define PQ_SEC_BASE         0     /* ������Ϣ����    */
#define PQ_SEC_BUSINESS     1     /* ������Ϣ����    */
#define PQ_SEC_PRIVATE      2     /* ������Ϣ����    */
#define PQ_SEC_PUBLIC       3     /* ȫ������        */
#define PQ_SEC_CONTROL      9     /* ������Ϣ(�ڲ�ʹ��) */

/* ��ָ���˵Ķ���  */
#define PQ_OPR_NORMAL       0     /* ����            */
#define PQ_OPR_HIDEN        1     /* ����            */
#define PQ_OPR_SHIELD       2     /* ����            */
#define PQ_OPR_DELETE       4     /* ɾ��            */
#define PQ_OPR_UNAUTH       5     /* û��ȷ��        */

/* ��������������  */
#define PQ_ONLINE_NEWS      1     /* ���ŷ�����      */
#define PQ_ONLINE_MEET      2     /* ���������      */

/* ���ܱ���        */
#define PQ_FUN_STATUS       100   /* ״̬��Ϣ        */
#define PQ_FUN_REQNUMBER    101   /* �������        */
#define PQ_FUN_CANCELNUM    102   /* ȡ����������    */
#define PQ_FUN_REGISTER     103   /* �û�ע��        */
#define PQ_FUN_LOGIN        104   /* �û���¼        */
#define PQ_FUN_RENUMBER     105   /* �����������    */
#define PQ_FUN_VERIFYNET    106   /* ������ȷ��      */
#define PQ_FUN_FINDUSER     107   /* ��Ѱ����        */
#define PQ_FUN_ADDCONTACTLIST   108  /* ����ͨ���б�  */

#define PQ_FUN_SYNINFO          109  /* TCP ��Ϣͬ��  */
#define PQ_FUN_SYNUSERINFO      110  /* �û���������  */
#define PQ_FUN_STOPMSG          111  /* TCP �������ݰ� */
#define PQ_FUN_SYNCONTACTINFO   112  /* ͨ���б��е��û�����  */
#define PQ_FUN_CONFIRMMSG       113  /* ȷ�����ݰ�            */
#define PQ_FUN_MODUSRSTATUS     114  /* �û�״̬�޸�          */
#define PQ_FUN_MODCONTACTLIST   115  /* �޸�ͨ���б��еĲ�����ʽ */
#define PQ_FUN_AUTH             116  /* �û���֤    */
#define PQ_FUN_GETUSRINFO       117  /* ȡ�û���Ϣ  */
#define PQ_FUN_LOGOUT           118  /* �û�����    */
#define PQ_FUN_MODUSRINFOR      119  /* �û����ϸ��� */
#define PQ_FUN_SENDMAIL         120  /* Send Mail    */
#define	PQ_FUN_FINDUSERINFO		121
#define PQ_FUN_FINDUSERNETREC	122
#define PQ_FUN_SENDMESSAGE		123  /* ������Ϣ  */
#define PQ_FUN_CHATBEGIN		124
#define PQ_FUN_CHATMESSAGE		125
#define PQ_FUN_CHATEND			126
#define PQ_FUN_SENDFILEBEGIN	127
#define PQ_FUN_SENDFILEDATA		128
#define PQ_FUN_SENDFILEEND		129
#define	PQ_FUN_FINDMEETSVR		130    /* ���һ��������  */
#define PQ_FUN_PLACARD          131    /* �������        */
#define PQ_FUN_REQONLINE        132    /* ������������    */


/*
#define PQ_FUN_WEATHERCITYLIST	139
#define PQ_FUN_WEATHERCUSTOM	140
#define PQ_FUN_WEATHER			141
*/
#define PQ_FUN_NEWSSERVER		142
/*
#define PQ_FUN_NEWSLIST			143
#define PQ_FUN_NEWSCUSTOM		144
#define PQ_FUN_NEWSCONTENT		145
*/
#define PQ_FUN_TCPTEST          160
#define PQ_FUN_TCPTEST1         161
#define PQ_FUN_GETUSERSTATUS    162   /* ȡָ���û��ĵ�ǰ״̬ */

/*  �������   */

#define PQ_WRN_LOGONLINE	1	  /* �����û�������      */
#define PQ_ERR_RQNALLOCMEM  10101 /* �ڴ��������        */
#define PQ_ERR_RQNSQLCUR    10102 /* ���ݿ��α����      */

#define PQ_ERR_RGUNOPQNUM   10301 /* ���ݰ���û��PICQ����*/
#define PQ_ERR_RGUNOPASSWD  10302 /* ���ݰ�����password  */
#define PQ_ERR_RGUINVPQNUM  10303 /* picq����ѡ��Χ��  */
#define PQ_ERR_RGUINVMSGID  10304 /* Message Id ���Ϸ�   */
#define PQ_ERR_RGUSQLINSERT 10305 /* ���ݲ������        */

/* �û���¼  */
#define PQ_ERR_LOGNOEXIST	10401 /* PICQ�Ų�����        */
#define PQ_ERR_LOGPWRONG	10402 /* ���벻��ȷ          */
#define PQ_ERR_LOGDBSERR    10403 /* ���ݿ��������      */
#define PQ_ERR_LOGPKGERROR	10404 /* ���ݰ�������        */
#define PQ_ERR_LOGSVRFAIL	10405 /* �������ڲ���������  */

#define PQ_ERR_VFNNOIP      10601 /* �����ʽ����IP��ַ  */

#define PQ_ERR_FUSRDBERR    10701 /* ���ݿ����          */
#define PQ_ERR_FUSRNOFOUND  10702 /* �û�������          */
#define PQ_ERR_FUSRMEMERR   10703 /* �ڴ�������        */
#define PQ_ERR_FUSRFMTERR   10704 /* �������ݰ�����ȷ    */

#define PQ_ERR_ADCOFMTERR   10801 /* ���ݰ�����ȷ        */
#define PQ_ERR_ADCODBSERR   10802 /* ���ݿ��������ȷ    */
#define PQ_ERR_ADCOMEMERR   10803 /* ��ʼ����Ϣʱ�ڴ治�� */
#define PQ_ERR_ADCOUSRINFO  10804 /* ԴPICQ���û���Ϣ���� */
#define PQ_ERR_ADCOMODI     10805 /* ͨ���б����ݿ��޸ĳ��� */

#define PQ_ERR_SYNINFO01    10901 /* �������ݰ�������     */
#define PQ_ERR_SYNINFO02    10902 /* ��Ϣ��ʼ������ȷ     */
#define PQ_ERR_SYNINFO03    10903 /* �û���Ϣ����ȷ       */

#define PQ_ERR_MODSTA001    11401 /* �������ݰ�������     */
#define PQ_ERR_MODSTA002    11402 /* ״̬�޸ĳ���         */


#define PQ_ERR_MODCOML001   11501 /* �������ݰ�������     */
#define PQ_ERR_MODCOML002   11502 /* �޸Ĳ��ɹ�           */

#define PQ_ERR_AUTH001      11601  /* ���ݰ�������         */
#define PQ_ERR_AUTH002      11602  /* �û�������           */
#define PQ_ERR_AUTH003      11603  /* Password����ȷ       */
#define PQ_ERR_AUTH004      11604  /* �û�����������       */

#define PQ_ERR_GETUSR001    11601  /* ���ݰ�������         */
#define PQ_ERR_GETUSR002    11602  /* ��Ϣ��ʼ������       */
#define PQ_ERR_GETUSR003    11603  /* ȡ�û���Ϣ����       */
#define PQ_ERR_LOGOUT001    11801  /* �����û�������       */

#define PQ_ERR_MODUSR001    11901  /* ���ݰ�����           */
#define PQ_ERR_MODUSR002    11902  /* Password����         */
#define PQ_ERR_MODUSR003    11903   /* Data Base Error     */

#define PQ_ERR_MAIL001      12001   /* ���ݰ�����          */
#define PQ_ERR_MAIL002      12002   /* �ܵ��򿪳���        */

#define PQ_ERR_FINDMSRV01   13001   /* ���ݰ�����          */
#define PQ_ERR_FINDMSRV02   13002   /* û�л��������      */

#define PQ_ERR_PLACARD01    13101   /* û�й������        */
#define PQ_ERR_PLACARD02    13102   /* ����ļ�������      */
#define PQ_ERR_PLACARD03    13103   /* �ڴ��������        */
#define PQ_ERR_PLACARD04    13104   /* GraphCode Error     */

#define PQ_ERR_REQONLINE01      13201  /* �Ѿ�Ԥ����    */
#define PQ_ERR_REQONLINE02      13202  /* Ԥ������      */

#define PQ_ERR_GETSTATUS01  16201   /* ���ݰ�����          */

/* ���������û�����  */

typedef struct {
			long lMsgId;		    /* �����ID��          */ 
		    long laPqnumber[6];     /* 6������             */
            long lReqTime;	        /* ����ʱ��			   */
} pqMsgRevNum;

/* �������б�         */
typedef struct {
    char caIpAddress[16];
    char caUdpPort[16];
} pqSrvHostList;


/*  Active Picq Information */

typedef struct {
    long   lPicqNum;     /* Picq���� */
    long   lMsgId;       /* ��¼��Ϣʱ��Ϣ��MessageId */
    long   lIpAddr;      /* Picq ��IP��ַ  */
    short  nUdpPort;     /* Udp�˿ں�, ����˳��  */
    short  nTcpPort;     /* Tcp�˿ں�, ����˳��  */
    short  nNetWork;     /*  0--ֱ�� >0--Proxy   no used */
    short  nNetMask;     /*  0--163   1--169     */
    short  nVersion;     /* �汾��                   */
    short  nStatus;      /* �û�״̬                 */
    long   lLoginTime;   /* ��¼ʱ��                 */
    long   lLastTime;    /* ���һ���յ���Ϣʱ��     */
    char   caPublicKey[24]; /* ͨ����Կ               */ 
    short  nPower;
    short  nUserKind;       /* 0 Picq >0 OnlineServer */
    char   caName[16];   /* UserName */
    long   lOIpaddr;     /* �Է�Ip��ַ  */
    short  nAccess;      /* ���뷽ʽ 0--����=����  1--����  2--����  */
    short  nReserve;
} pqStaActive;

typedef struct {
    long lSumNum;
    long lMaxNum;
    long lTime;
    long *lPicq;
} pqTimeList;
    
typedef struct {
    long           lNetType;     /* �����ʶ  */
    unsigned long  lIpAddress;   /* ������ַ  */
    unsigned long  lStartIp;     /* ��ʼ��ַ  */
    unsigned long  lNetMask;     /* ��������  */
} pqNetMask;


typedef struct {
    long   graphcode;
    char   caFileName[32];
    short  nType;       /*  1-- bmp  2--gif  3---jepg  */
    long   lCount;
} pqAdvertising;    /* �����Ϣ  */

#define PQ_GIF_BMP     1
#define PQ_GIF_GIF     2
#define PQ_GIF_JEPG    3

/* ͨ���б����Ϣ  */
typedef struct {
    long lPicqNum;   /* PICQ����  */
    long lStatus;    /* ״̬      */
    long lTime;      
} pqContactList;

/* �����û���Ϣ  */
typedef struct {
    long  lPicqNum;        /* �������ֽ�˳����   */
	char  caMicqNum[12];    
	char  caUserName[16];
    char  caRealName[20];
    char  caEmail[64];
    long  lStatus;          /* �������ֽ�˳����   */
} pqFUserInfo;

/* �û�״̬��Ϣ  */
typedef  struct {
    long   lPicqNum;    /* PICQ��    */
    short  nStatus;     /* �û�״̬  */
    short  nNetWork;    /* ��������  */
    short  nUdpPort;    /* Udp�˿ں� */
    short  nTcpPort;    /* Tcp�˿ں� */
    long   lIpAddress;  /* IP��ַ    */
    char   caPubKey[24]; /* ������Կ  */
} pqUserStatus;

/* ������������Ϣ  */
typedef struct {
    char            caName[16];
    long            lPicqNum;
    unsigned long   ulAddr;
    unsigned short  unUdp;
    unsigned short  unTcp;
    unsigned short  unType;
    unsigned short  unNoUsed;
} pqOnlineInfo;

/*  pqm000.c */
int pqInit000(utShmHead *psShmHead);
int pqComTcpInit(utShmHead *psShmHead);
int pqComUdpEnd(utShmHead *psShmHead);
int pqComTcpEnd(utShmHead *psShmHead);
/*  pqm001.c  */
int pqFunVerifyNetT(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunRequestNumber(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunCancelNum(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead);
int pqFunReNumber(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead); 
int pqFunDownPlacard(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead);          
/* pqm002.c */
int pqFunSynInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunModiStatus(utShmHead *psShmHead, int iFd,
                                    utMsgHead *psMsgHead);
/* pqm003.c  */
int pqFunRegister(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunFindUser(utShmHead *psShmHead, int iFd,
        utMsgHead *psMsgHead);
int pqFunModUserInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* pqm004.c   */
int pqFunAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunGetUsrInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunReqOnline(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* pqm005.c */
int pqFunSendMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* pqm006.c */
int pqFunTestConnect(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunTestBigPkg(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunFindMeetSvr(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunGetUserStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* pqm100.c  */
int pqComUdpInit(utShmHead *psShmHead);

/* pqm101.c  */
int pqFunLogin(utShmHead *psShmHead, 
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int pqFunExchangeMsg(utShmHead *psShmHead, 
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
/* pqm102.c */
int pqFunSynStatus(utShmHead *psShmHead, 
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
/* pqm103.c */


/* pqm104.c */
int pqFunAddComList(utShmHead *psShmHead, 
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
/* pqm105.c */
int pqFunStatus(utShmHead *psShmHead, 
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int pqFunVerifyNet(utShmHead *psShmHead, 
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int pqFunLogout(utShmHead *psShmHead, 
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int pqFunModiContactList(utShmHead *psShmHead, 
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
/* pqm201.ec */
int pqDbsAddComList(long lSourcePicq,long lObjectPicq,
        long lStatus,long lOpFlags);
int pqDbsModiComList(long lSourcePicq,long lObjectPicq,
        long lStatus,long lOpFlags);
int pqDbsGetUserInfo(long lPicq,long lSecrecy,long lTime,
    utMsgHead **psMsgHead);
int pqDbsAddSendMsg(long lPicqNum,long lType,utMsgHead *psMsgHead);
int pqUdpSendPendMsg(utShmHead *psShmHead,pqStaActive  *psStaActive);
int pqDbsLogin(long lPicq,char *pPassWord,
    char *pUserName,long *lpPower,long *lpUserKind);
int pqSReadNumberFromDb(utShmHead *psShmHead);
int pqDbsUserAuth(long lPicq,char *pPassWord,long *lpPower,long *lpUserKind);
int pqDbsLogoff(long lPicq);
int pqDbsModiContactList(long lPicq1,long lPicq2,short lOpr);
int pqDbsGetAdver(utShmHead *psShmHead);
int pqDbsInitUserStatus();
int pqDbsLoginAddComLst(utShmHead *psShmHead, long lPicqNum0);
/* pqm202.ec */
int pqFunSynConactInfo(utShmHead *psShmHead, int iFd,
        utMsgHead **psMsgHead,long lPicq0,long lTime0,int iConfirm);
pqUserStatus *pqFunGetComStatus(utShmHead *psShmHead,
    long lPicqNum0,long *lReturn);
int pqUdpDoLogin(utShmHead *psShmHead,long lPicqNum);
int pqDbsAddLogs(pqStaActive *psStatus);
int pqDbsModiLogs(utShmHead *psShmHead,long lPicqNum);
/* pqm203.ec */
int pqFunRegister0(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int pqFunFindUser0(char *pSmt,long lStart,long lNumRec,pqFUserInfo *psUser);
int pqFunModUserInfo0(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);   
   
/* pqm301.c */
pqStaActive *pqUtlGetPicqStatus(utShmHead *psShmHead,
            long lPicqNum);
int pqGetPicqUdpAddr(utShmHead *psShmHead,long lPicqNum, 
        utComSockAddr *psSockAddr);
int pqUdpDoLogOff(utShmHead *psShmhead,long lPicqNum);

/* pqm302.c */
void pqSetPicqNum(long lPicqNum, int nUdpPort);
int pqComUdpRequest1(utShmHead *psShmHead, utComSockAddr *psSockAddr, 
        long lPicqNum,short nFunCode,long lMsgId,int iConfirm,short nVarNum,...);
int pqComUdpResponse1(utShmHead *psShmHead,utComSockAddr *psSockAddr,
        utMsgHead *psMsgHead0,int iConfirm,short nVarNum,...);
int pqComUdpRequest(int iSock, utComSockAddr *psSockAddr, 
        long lPicqNum,short nFunCode,long lMsgId,short nVarNum,...);
int pqNetGetNetwork(utShmHead *psShmHead,unsigned long lAddress,
            unsigned long *lReturn);
int pqSGetPqNumber(utShmHead *psShmHead,char *pPicq,long *lpNumber,int iNumber);
int pqSAddPqNumber(utShmHead *psShmHead,long *lpNumber,int iNumber);
int pqUtlSendStatus(utShmHead *psShmHead, long lPicqNum);
int pqUtlStaToFriend(utShmHead *psShmHead, long lPicqNum,
                                    long lStatus); 
int pqUdpLetLogOut(utShmHead *psShmHead,
        pqStaActive *psStaActive); 
int pqComUdpMsgToPicq(utShmHead *psShmHead,
        long lPicqNum1, long lPicqNum2,
        long lCode, short nFunCode,
        short nConfirm,
        short nVarNum,...);
int pqComUdpMsgToPicq0(utShmHead *psShmHead,
        long lPicqNum,utMsgHead *psMsgHead,int iConfirm);        
pqOnlineInfo *pqUtlGetOnlineSrv(utShmHead *psShmHead,
        int iType,int *iNumber);
int pqUtlStaExchange(utShmHead *psShmHead,long lPicqNum0, 
                 long lPicqNum);       
        
/* pqm303.c */
int pqUtlTimeControl(utShmHead *psShmHead);   
#endif

