#ifndef __NCRANGEDEF__
#define __NCRANGEDEF__

/* 端口控制        */
typedef struct ncCtlPortList_s {
    unsigned short nSport;
    unsigned short nEport;
    unsigned short nFlags;
    unsigned short nRev;
} ncCtlPortList;

/* IP和端口控制   */
typedef struct ncCtlIpPort_s {
    unsigned long  lSip;
    unsigned long  lEip;
    unsigned short nSum;
    unsigned short nMax;
    ncLimPortList *psPort;
} ncCtlIpPort;

typedef ncCtlUser_s {
    unsigned long  lUid;
    unsigned short nType;
    ncCtlIpPort  *psIpPort;
} ncCtlUser;

    
typedef ncCtlTime_s {
    unsigned long   lStime;
    unsigned long   lEtime;
    unsigned short  nSum;
    unsigned short  nMax;
    ncCtlUser     *psUser;
} ncCtlTime;



#endif
