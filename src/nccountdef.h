/* �Զ�ͳ��  */

#ifndef __NCCOUNTDEF_DEF__
#define __NCCOUNTDEF_DEF__   1
#define NC_COUNT_IPLOG      1
#define NC_COUNT_SERVICE    2
#define NC_COUNT_SERVICEIP  3
#define NC_MAX_COUNT            1000000L
#define NC_MAX_HTTP_COUNT       200000L

typedef struct ncCountLogPos_s {
    char  caName[32]; // ������
    short nStart;     // ��ʼλ�ã���0��ʼ, -1��ʾ�޼�¼
    short nLen;       // ����
} ncCountLogPos;

typedef struct ncIpCountBuf_s {
    unsigned long   userid;      // �û�ID
    unsigned long   lTime;       // ʱ��
    unsigned long   lIp;         // �� nType=1 Ŀ��IP��ַ   nType=2 �������   nType=3: userid== lIp DipService
    unsigned short  nType;       // 1 Ŀ���ַ   2 ������� 3 ServiceIp
    unsigned short  nCtime;      // ����ʱ��
    unsigned long   long lUb;    // �����ֽ���
    unsigned long   long lDb;    // �����ֽ���
} ncIpCountBuf;



typedef struct ncHttpCountBuf_s {
    unsigned long   userid;      // �û�ID
    unsigned long   lTime;       // ʱ��
    unsigned char   host[64];    // ������
    unsigned long   lDip;        // Ŀ���ַ
    unsigned long   lCtime;      // ����ʱ��
    unsigned long   lCount;      // �������
    unsigned long   lTid;        // ��ַ���
    unsigned long   long lBytes; // �ֽ���
} ncHttpCountBuf;



int ncCountGetDataFromBuf(ncCountLogPos *psLogPos,char *pBuf,int iSum,...);    


#endif
