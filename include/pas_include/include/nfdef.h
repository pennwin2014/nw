#ifndef  NFDEF__
#define NFDEF__
#define NF_MSG_CODE       872783212
#define NF_MSG_VERSION    100
#define NF_MAX_BUFFER     8168
#define NF_MAX_ALLOC      4096
#define NF_TYPE_CHAR        1
#define NF_TYPE_SHORT       2
#define NF_TYPE_LONG        3
#define NF_TYPE_FLOAT       4
#define NF_TYPE_DOUBLE      5
#define NF_TYPE_STRING      6
#define NF_TYPE_STRUCT      7

#define NF_OPEN         1
#define NF_READ         2
#define NF_WRITE        3
#define NF_CLOSE        4

typedef struct {
    unsigned long       lMsgCode;   /* ��ϢУ����                  */
    unsigned long       lBytes;     /* ���ݰ�����                  */
    unsigned short      nFunCode;   /* ���ܴ���                    */
    unsigned short      nVersion;   /* �汾��                      */
    unsigned long       lIpAddress; /* ������IP��ַ,�����ݽ���ʱ��д       */
    int                 iFd;        /* �������ļ����              */
    unsigned long       lSumVar;    /* ��������                    */
} nfMsgHead;

typedef struct {
        unsigned short nType;    /* ��������  */
        unsigned short nBytes;   /* �ֽ���    */
} nfMsgVar; 

nfMsgHead *nfTcpMsgRecv(int iSocket);
int nfMsgGet(nfMsgHead *psMsgHead,...);
char *nfGetThisData(nfMsgHead *psMsgHead,int iNum,int *iBytes);
int nfTcpMsgSend(int iSock,int iFd,short nFunCode,int iNumVar,...);
int nfWriten(int iSock,char *pBuffer,int iBytes);
int nfReadn(int iSock,char *pBuffer,int iBytes);
int nfUdpMsgSend(int iSock,struct sockaddr_in *to,
                          int iFd,short nFunCode,int iNumVar,...);
nfMsgHead *nfUdpMsgRecv(int iSocket,struct sockaddr_in *from);     
int nfUdpMsgSend0(int iSock,struct sockaddr_in *to,nfMsgHead *psMsgHead);
int nfTcpMsgSend0(int iSock,nfMsgHead *psMsgHead);                    
#endif