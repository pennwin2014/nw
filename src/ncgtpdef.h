/* ncgtpdef.h
   Create Date   2010/08/30

*/
#ifndef __NCGTPDEF__
#define __NCGTPDEF__

#define  NC_PORT_GTPUSER        2152
#define  NC_PORT_GTPCONTROL     2123
#define  NC_TCPPORT_GPRSDATE    3386
#define  NC_UDPPORT_GPRSSIG     3386


#define  NC_GTP_

typedef struct ncGtpFilter_s {
    unsigned char  caId[31];                  // 标识
    unsigned char  cProt;                     // 协议 0-全部  6-TCP  17-UDP
    unsigned short nPort1;                   
    unsigned short nPort2;
    unsigned long  lSip1;
    unsigned long  lSip2;
    unsigned long  lDip1;
    unsigned long  lDip2;
} ncGtpFilter;



typedef struct ncGtpHead_s {
	    unsigned char  ssn:1;
	    unsigned char  sp:3;
	    unsigned char  pt:1;
	    unsigned char  ver:3;
        unsigned char  mtype;
        unsigned short len;
        unsigned long  teid;
        unsigned short sq;                // Sequence
        unsigned char  npdunum;
        unsigned char  nextexi;
} ncGtpHead;


typedef struct ncGtpTypeMessage_s {
    unsigned char   cType;
    unsigned char   caMessage[63];
    unsigned long   lCount;
    unsigned long   lBytes;
} ncGtpTypeMessage;

typedef struct ncGtpCount_s {
    
    unsigned long   lSip;
    unsigned long   lDip;
    unsigned short  nSport;
    unsigned short  nDport;
    unsigned char   cProt;
    unsigned char   cSta;
    unsigned long   lSsec;
    unsigned long   lSusec;
    unsigned long   lEsec;
    unsigned long   lEusec;
    unsigned long   lCount[32];
    unsigned long   lBytes[32];
} ncGtpCount;


    
#endif
