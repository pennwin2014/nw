/* 金色网关  */
#ifndef __NCGDGDEF__
#define __NCGDGDEF__

#define MAX_HTTP_SERVER        32           /* 最大http虚拟服务器  */
#define MAX_SMB_SHARE          128          /* 最大Samb共享目录数  */
#define GDG_SERVICE_DHCP        1
#define GDG_SERVICE_DNS         2
#define GDG_SERVICE_FIREWALL    3
#define GDG_SERVICE_FTP         5
#define GDG_SERVICE_MAIL        6
#define GDG_SERVICE_VPN         7    
#define GDG_SERVICE_BANDWIDTH   8
#define GDG_SERVICE_NETMON      9
#define GDG_SERVICE_JAVA       10
#define GDG_SERVICE_SMB        11
#define GDG_SERVICE_IPSEC      12
#define GDG_SERVICE_TELNET     13
#define GDG_SERVICE_SSH        14
#define GDG_SERVICE_DDNS       15

typedef struct gdPppDevInfo_s {
    char caDev[32];    // 设备名
    char caLip[16];    // 本地地址
    char caRip[16];    // 对方地址
    char caMask[16];   // 掩码
    int  iUp;
    long long lPacket[2];    // 数据包 0-发送  1-接收
    long long lBytes[2];     // 流量   0-发送  1-接收
    struct gdPppDevInfo_s *next;   // 下一个记录指针
} gdPppDevInfo;

#endif
