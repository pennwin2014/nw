#ifndef __NCIPTDEF__
#define __NCIPTDEF__          1

typedef struct iptnat_s {
    unsigned long  nattype;    
    int      prot;
    unsigned long sip;
    unsigned long dip;
    unsigned long sport;
    unsigned long dport;
    char     indev[16];
    char     outdev[16];
    unsigned long toip;
    unsigned long toport;
    int      flags;        /*  0--∆Ù”√  1--Õ£”√  */
} iptnat;


#endif