/* ��ɫ����  */
#ifndef __NCGDGDEF__
#define __NCGDGDEF__

#define MAX_HTTP_SERVER        32           /* ���http���������  */
#define MAX_SMB_SHARE          128          /* ���Samb����Ŀ¼��  */
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
    char caDev[32];    // �豸��
    char caLip[16];    // ���ص�ַ
    char caRip[16];    // �Է���ַ
    char caMask[16];   // ����
    int  iUp;
    long long lPacket[2];    // ���ݰ� 0-����  1-����
    long long lBytes[2];     // ����   0-����  1-����
    struct gdPppDevInfo_s *next;   // ��һ����¼ָ��
} gdPppDevInfo;

#endif
