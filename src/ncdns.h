#ifndef __NCDNSCASH__
#define __NCDNSCASH__
typedef struct ncDnsHead_s {
        unsigned short nId;       // ID

        unsigned char  rd:1;
        unsigned char  tc:1;
        unsigned char  aa:1;
        unsigned char  opcode:4;  // 0--标准查询  1--反向查询  2--服务器状态请求
        unsigned char  qr:1;      // 0--查询  1--应答

        unsigned char  rcode:4;  // 0--无错  3--名字差错
        unsigned char  z:3;
        unsigned char  ra:1;
        unsigned short qdcount;  //问题数
        unsigned short ancount;  //应答数
        unsigned short nscount;
        unsigned short arcount;
} ncDnsHead;

/* DNS 缓存     NC_LNK_DNSCASH 69              
        DNS缓存保存在共享内存中
 */
typedef struct ncDnsCash_s {
	char     		host[64];     /* 主机名称                              */
	unsigned long 	ip[5];        /* IP地址,一个DNS可以有5各不同的IP地址   */
	unsigned long   lasttime;     /* 最后访问时间                          */
	unsigned long   count;        /* 访问次数                              */
} ncDnsCash;

typedef struct ncDnsHostIp_s {
	char     		host[64];     /* 主机名称                              */
	unsigned long 	ip;           /* IP地址,一个DNS可以有5各不同的IP地址   */
	unsigned long   lasttime;     /* 最后访问时间                          */
} ncDnsHostIp;


typedef struct ncDnsIpHost_s {
    unsigned long   ip;           /* Ip地址                                */
	char     		host[64];     /* 主机名称                              */
	unsigned long   lasttime;     /* 最后访问时间                          */
} ncDnsIpHost;


#endif

