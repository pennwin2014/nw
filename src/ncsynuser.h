#ifndef __NCSYNUSER___
#define __NCSYNUSER___

/* �û���Ϣͬ��   */
#define SYN_ONBASE_IP       1    // ����IP��ַ
#define SYN_ONBASE_MAC      0    // ����MAC��ַ
#define SYN_FROM_CAMS       5
typedef struct ncSynUserInfo_s {
    unsigned long  ip;          /* IP��ַ                       */
    unsigned char  mac[6];      /* Mac��ַ                      */
    unsigned char  flags;       /* ��¼��Դ 1--�̶����� 8--Wins 9--DHCP */
    unsigned char  status;      /* ״̬ 0-������  1-����                */
    unsigned char  uname[32];   /* �û���      */
    unsigned char  cname[32];   /* ������      */
    unsigned char  dept[32];    /* ����      */
    unsigned long  lLastTime;   /* ������ʱ��  */
} ncSynUserInfo;

typedef struct ncCamsGroup_s {
    unsigned long groupid;
    unsigned char groupname[32];       /* ��������  */
    struct ncCamsGroup_s *next;
} ncCamsGroup;

#endif
