/* 2009/05/06 免监用户                       */
#ifndef __NCNOMONDEF__
#define __NCNOMONDEF__

/*
    ncdef.h  增加 
#define NC_LNK_NOMONLIST       133    免监控列表                 
    ncdb001.c 增加
    ncutl012.c
*/
/* 免监控用户列表   */
typedef struct ncNoMonUser_s {
    unsigned long lSip;              // Ip地址
    unsigned char cStatus;           // NC_USER_NOMON(2)-免监控  0-超时
    unsigned char cRev[3];       
    unsigned long lLastTime;         // 最后更新时间
} ncNoMonUser;
#endif