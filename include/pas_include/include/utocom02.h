#ifndef __UTOCOMDEF2__
#define __UTOCOMDEF2__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdarg.h>



/*************************************************************/

typedef struct {
    int      iSock;   /* Sock Id                                   */
    unsigned char    cKind;     /* 0--�ⲿ  1--�ڲ�                */
    unsigned char    cReserve;  /* ����                            */
    unsigned char    cFlags;    /* 0--����  1--���ڵȴ�Ӧ��        */
    unsigned char    cType;     /* 0 ����  1 �����ݰ�  2 �����ݰ�     */
    long     lTime;             /* ������ʱ��        */
    long     lClientIp;         /* Client IP��ַ       */
    int      iClientPort;       /* Client �˿ں�       */
} utTcpSockInfo;


typedef struct {
    int              iPort;        /* �˿ں�  */
    int              iSum;         /* ����    */
    int              iThisPort;    /* Tcp�ڲ��˿ں�       */
    int              iThisIp;      /* ����IP��ַ          */
    int              iPid;         /* ���̺�              */
    long             lOffset;      /* ƫ����  */
} utTcpProSockTable;


/* �߳���Ϣ  */
typedef struct {
    int         iPid;       /* ���̱��  */
    int         iTaskId;    /* �����ʶ  */
    int         iThreadId;  /* �̱߳�ʶ  */
    int         iStartTime; /* ��ʼʱ��  */
} urTcpThreadInfo;


typedef struct {
    utTcpSockInfo     *psSockInfo;     /* Sock��Ϣ      */
    utTcpThreadInfo   *psThreadInfo;   /* �߳���Ϣ      */
    utShmHead         *psShmHead;      /* �����ڴ���  */
} utTcpTransInfo;   

#endif

