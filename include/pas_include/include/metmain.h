#ifndef METINCLUDE_DEF
#define METINCLUDE_DEF

/* Mettcpdo.c */
int MetTcpInit(utShmHead *psShmHead);

#define MET_LNK_MEETROOM         20      /* ������      */
#define MET_LNK_MEETPEOPLE       21      /* ����μ���  */

/* FunCOde of Online server */
#define PQ_FUN_REQMEETSVR		300      /* ������������  */
#define PQ_FUN_MEETINVITE		301      /* ��������        */
#define PQ_FUN_MEETJOIN			302      /* ����������    */
#define PQ_FUN_MEETFUNCTION		303      /* ת�����鹦��    */
#define PQ_FUN_MEETDISPLACE		304      /* ����ת��        */
#define PQ_FUN_MEETEXIT			305      /* �˳�����        */
#define PQ_FUN_MEETEND			306      /* �������        */
#define PQ_FUN_MEETINVITERES    307      /* ����Ӧ��        */

/* FunCOde of Meeting  */
#define PQ_MET_ANNOUNCEWORDS     1
#define PQ_MET_REQANNOUNCE       2
#define PQ_MET_FORBIDANNOUNCE    3
#define PQ_MET_ALLOWANNOUNCE     4
#define PQ_MET_REQDISPLACE       5
#define PQ_MET_REJDIDPLACE       6
#define PQ_MET_AGRDISPLACE       7
#define PQ_MET_DISPLACE          8
#define PQ_MET_JOININ            9     /* �������  */
#define PQ_MET_NOTJOININ         10
#define PQ_MET_DISPELOUT         11
#define PQ_MET_EXIT              12
#define PQ_MET_TERMINATE         13

typedef struct {
    long  lMeetId;          /* �����ʶ          */
    long  lPicq;             /* ����������PICQ��  */
    char  caSubject[64];     /* ��������          */
    char  caAgenda[400];     /* ��������          */
    long  lTime;             /* ���鿪ʼʱ��      */
    long  lLastTime;         /* ���һ�β���ʱ��  */
    long  lSumMan;           /* ����μ�������    */
} metRoomInfo;

typedef struct {
    long lMeetId;          /* �����ʶ  */
    long lPicqNum;         /* Picq����  */
    char caName[16];
    long lLastTime;
    unsigned short nNetType;
    short          nType;    /* 0--һ��μ���   1---������   2--�ȴ�ȷ�� */
    short          nSock;
    unsigned short nInPort;
    long     lConnectId;     /* ���ӱ�ʶ   */
} metPeopleInfo;

/* The Value of nType  */
#define MET_TYPE_ATTENDE       0    /* һ��μ���  */
#define MET_TYPE_PRESIDING     1    /* ������       */
#define MET_TYPE_UNAUTH        2    /* û����֤     */


#define MET_ERR_REQMET01           30101     /* ���ݱ�������  */
#define MET_ERR_REQMET02           30102     /* �ڴ�������  */
#define MET_ERR_REQMET03           30102     /* ���������    */

#define MET_ERR_METINV01           30201     /* ���ݰ�������  */
#define MET_ERR_METINV02           30202     /* �����������������  */
#define MET_ERR_METINV03           30203     /* �����������  */
#define MET_ERR_METINV04           30204     /* �����Ҳ�����  */
#define MET_ERR_METINV05           30205     /* û�б�����    */
#define MET_ERR_METINV06           30206     /* �������Ͳ���  */

#define MET_ERR_TRANS01            30301     /* ���ݰ�������  */

#define MET_ERR_EXCH01             30401     /* ���ݰ�������  */
#define MET_ERR_EXCH02             30402     /* ��������  */


#define MET_ERR_EXIT01             30501     /* ���ݰ�������  */
#define MET_ERR_EXIT02             30502     /* �����Ҳ����� */


/* ���ֺ����Ĵ�����Ϣ  */
/* met000.c */
int metInit000(utShmHead *psShmHead);
int metTcpInit(utShmHead *psShmHead);
int metTcpEnd(utShmHead *psShmHead);
int MetUdpInit(utShmHead *psShmHead);

/* met002.c */
int metFunReqMeetSrv(utShmHead *psShmHead, int iFd,
            utMsgHead *psMsgHead);
int metFunMeetInvite(utShmHead *psShmHead, int iFd,
            utMsgHead *psMsgHead);            
int metFunTransFunMsg(utShmHead *psShmHead, int iFd,
            utMsgHead *psMsgHead);
int metFunExchangeMeet(utShmHead *psShmHead, int iFd,
            utMsgHead *psMsgHead);
int metFunExitMeet(utShmHead *psShmHead, int iFd,
               utMsgHead *psMsgHead);
int metFunMeetResInvite(utShmHead *psShmHead, int iFd,
            utMsgHead *psMsgHead);
/* met003.c */
metRoomInfo *metUtlGetRoomInfo(utShmHead *psShmHead,int lMeetId);
metPeopleInfo *metUtlGetPeopleInfo(utShmHead *psShmHead,long lMeetId,long lPicqNum);
int metUtlTcpMsgSend(int iPort,int iNum,long lConnectId,utMsgHead *psMsgHead,int iClose);
int metUtlTcpMsgSendTo(utShmHead *psShmHead, long lMeetId,long lPicqNum,
                        utMsgHead *psMsgHead,int iClose);
int metUtlTcpMsgSendToAll(utShmHead *psShmHead, long lMeetId,
                        utMsgHead *psMsgHead,int iClose);
int metUtlTcpAllStaToMe(utShmHead *psShmHead,long lMeetId,
        metPeopleInfo *psPeopleInfo);
int metUtlPeopleCompare(char *pSource,char *pKey);                                              
#endif
