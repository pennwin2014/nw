#ifndef __NCDNSCASH__
#define __NCDNSCASH__
typedef struct ncDnsHead_s {
        unsigned short nId;       // ID

        unsigned char  rd:1;
        unsigned char  tc:1;
        unsigned char  aa:1;
        unsigned char  opcode:4;  // 0--��׼��ѯ  1--�����ѯ  2--������״̬����
        unsigned char  qr:1;      // 0--��ѯ  1--Ӧ��

        unsigned char  rcode:4;  // 0--�޴�  3--���ֲ��
        unsigned char  z:3;
        unsigned char  ra:1;
        unsigned short qdcount;  //������
        unsigned short ancount;  //Ӧ����
        unsigned short nscount;
        unsigned short arcount;
} ncDnsHead;

/* DNS ����     NC_LNK_DNSCASH 69              
        DNS���汣���ڹ����ڴ���
 */
typedef struct ncDnsCash_s {
	char     		host[64];     /* ��������                              */
	unsigned long 	ip[5];        /* IP��ַ,һ��DNS������5����ͬ��IP��ַ   */
	unsigned long   lasttime;     /* ������ʱ��                          */
	unsigned long   count;        /* ���ʴ���                              */
} ncDnsCash;

typedef struct ncDnsHostIp_s {
	char     		host[64];     /* ��������                              */
	unsigned long 	ip;           /* IP��ַ,һ��DNS������5����ͬ��IP��ַ   */
	unsigned long   lasttime;     /* ������ʱ��                          */
} ncDnsHostIp;


typedef struct ncDnsIpHost_s {
    unsigned long   ip;           /* Ip��ַ                                */
	char     		host[64];     /* ��������                              */
	unsigned long   lasttime;     /* ������ʱ��                          */
} ncDnsIpHost;


#endif

