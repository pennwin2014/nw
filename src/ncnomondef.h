/* 2009/05/06 ����û�                       */
#ifndef __NCNOMONDEF__
#define __NCNOMONDEF__

/*
    ncdef.h  ���� 
#define NC_LNK_NOMONLIST       133    �����б�                 
    ncdb001.c ����
    ncutl012.c
*/
/* �����û��б�   */
typedef struct ncNoMonUser_s {
    unsigned long lSip;              // Ip��ַ
    unsigned char cStatus;           // NC_USER_NOMON(2)-����  0-��ʱ
    unsigned char cRev[3];       
    unsigned long lLastTime;         // ������ʱ��
} ncNoMonUser;
#endif