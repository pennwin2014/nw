/* ���������  */

#ifndef __NCPROXY_DEF__
#define __NCPROXY_DEF__   1
#define ProxyDebugFile   "../log/ncproxy.log"

#define  NC_NMAP_PROXY      1
#define  NC_NMAP_SOCKS5     2
#define  NC_NMAP_SOCKS4     4

typedef struct proxyCommand_s {
    unsigned long  lIp;               /* IP��ַ                  */
    unsigned short nPort;             /* �˿�                    */
    unsigned short nRev;              /* ����                    */
    unsigned long  lTime;             /* ʱ���                  */
    unsigned long  lCode;             /* У����                  */
} proxyCommand;

typedef struct proxyDesHead_s {
    unsigned long lSlen;     // Դ���ݰ�����
    unsigned long lDlen;     // Ŀ�����ݰ�����
} proxyDesHead;


/* ɨ���IP��ַ״̬    

 */
typedef struct ncNmapIpStatus_s {
    unsigned long    lIp;         // IP��ַ
    unsigned char    cSum;        // �˿�����
    unsigned char    cMax;        // �����ڴ��С
    unsigned char    cFlags;
    unsigned char    cProxy;      // 0--����Proxy 
    int              iPid;        // ���̻��߳�ID
    unsigned long    lLastTime;   // ������ʱ��
    unsigned short   *psPort;     // �˿�
    struct ncNmapIpStatus_s *next;
} ncNmapIpStatus;


typedef struct ncNmapProxyList_s {
    unsigned long    lIp;         // IP��ַ
    unsigned short   nPort[7];    // �˿�
    unsigned char    cProxy;      // �������
    unsigned char    cFlags;
    unsigned char    caMsg[128];  // �����Ϣ (80:Ccproxy  808:Http)
} ncNmapProxyList;


typedef struct ncNmapProxyHead_s {
    unsigned long       lMax;        // ����¼��
    unsigned long       lSum;        // ��ǰ��¼��
    ncNmapProxyList     *psProxy;
} ncNmapProxyHead;


typedef struct ncNmapSendList_s {
    unsigned long    lIp;         // IP��ַ     
    unsigned short   nPort;       // �˿�
    unsigned char    cProxy;      // ��� 
    unsigned char    cRev;        // ����
    int              iFd;         // Socket ID
    unsigned long    lLastTime;   // ������ʱ��
} ncNmapSendList;



typedef struct ncNmapSendHead_s {
    unsigned long    lMax;        // ����¼��
    unsigned long    lSum;        // ��ǰ��¼��
    unsigned char    cSort;       // 0--δ����   1--����
    unsigned char    cRev[3];
    ncNmapSendList   *psSend;
} ncNmapSendHead;


/* ���˿�    */
typedef struct ncNmapProxyPort_s {
    unsigned short  nPort;         // �˿�
    unsigned char   cProxy;        // 1--HttpProxy   2--Socks5  4---Socks4  0--NotScan
    unsigned char   cService;      // cProxy==0, cService > 0 ����
    unsigned char   caName[16];    // ��������
} ncNmapProxyPort;

typedef struct ncNmapPortHead_s {
    unsigned long   lMax;
    unsigned long   lSum;
    ncNmapProxyPort *psPort;
} ncNmapPortHead;


#endif
