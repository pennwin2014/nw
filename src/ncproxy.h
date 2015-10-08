/* 代理服务器  */

#ifndef __NCPROXY_DEF__
#define __NCPROXY_DEF__   1
#define ProxyDebugFile   "../log/ncproxy.log"

#define  NC_NMAP_PROXY      1
#define  NC_NMAP_SOCKS5     2
#define  NC_NMAP_SOCKS4     4

typedef struct proxyCommand_s {
    unsigned long  lIp;               /* IP地址                  */
    unsigned short nPort;             /* 端口                    */
    unsigned short nRev;              /* 保留                    */
    unsigned long  lTime;             /* 时间戳                  */
    unsigned long  lCode;             /* 校验码                  */
} proxyCommand;

typedef struct proxyDesHead_s {
    unsigned long lSlen;     // 源数据包长度
    unsigned long lDlen;     // 目标数据包长度
} proxyDesHead;


/* 扫描的IP地址状态    

 */
typedef struct ncNmapIpStatus_s {
    unsigned long    lIp;         // IP地址
    unsigned char    cSum;        // 端口总数
    unsigned char    cMax;        // 分配内存大小
    unsigned char    cFlags;
    unsigned char    cProxy;      // 0--不是Proxy 
    int              iPid;        // 进程或线程ID
    unsigned long    lLastTime;   // 最后操作时间
    unsigned short   *psPort;     // 端口
    struct ncNmapIpStatus_s *next;
} ncNmapIpStatus;


typedef struct ncNmapProxyList_s {
    unsigned long    lIp;         // IP地址
    unsigned short   nPort[7];    // 端口
    unsigned char    cProxy;      // 代理服务
    unsigned char    cFlags;
    unsigned char    caMsg[128];  // 检测信息 (80:Ccproxy  808:Http)
} ncNmapProxyList;


typedef struct ncNmapProxyHead_s {
    unsigned long       lMax;        // 最大记录数
    unsigned long       lSum;        // 当前记录数
    ncNmapProxyList     *psProxy;
} ncNmapProxyHead;


typedef struct ncNmapSendList_s {
    unsigned long    lIp;         // IP地址     
    unsigned short   nPort;       // 端口
    unsigned char    cProxy;      // 类别 
    unsigned char    cRev;        // 保留
    int              iFd;         // Socket ID
    unsigned long    lLastTime;   // 最后访问时间
} ncNmapSendList;



typedef struct ncNmapSendHead_s {
    unsigned long    lMax;        // 最大记录数
    unsigned long    lSum;        // 当前记录数
    unsigned char    cSort;       // 0--未排序   1--排序
    unsigned char    cRev[3];
    ncNmapSendList   *psSend;
} ncNmapSendHead;


/* 检测端口    */
typedef struct ncNmapProxyPort_s {
    unsigned short  nPort;         // 端口
    unsigned char   cProxy;        // 1--HttpProxy   2--Socks5  4---Socks4  0--NotScan
    unsigned char   cService;      // cProxy==0, cService > 0 服务
    unsigned char   caName[16];    // 服务名称
} ncNmapProxyPort;

typedef struct ncNmapPortHead_s {
    unsigned long   lMax;
    unsigned long   lSum;
    ncNmapProxyPort *psPort;
} ncNmapPortHead;


#endif
