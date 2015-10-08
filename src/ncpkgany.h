#ifndef __NCANYPKGDEF__
#define __NCANYPKGDEF__

/* ��ץ���ݰ��İ�ͷ  */
typedef struct ncPkgFileHead_s {
    unsigned long  lId;     // �ļ�ID   "%#&@"
    unsigned long  lSec;    // ʱ����
    unsigned long  lUsec;   // ʱ��΢��
    unsigned long  lLen;    // ����
} ncPkgFileHead;

/* Э���������  */
typedef struct ncPkgAnyFun_s {
    char  caName[32];                                         /* ˵��       */
    int (*ncPkgDo)(ncIpHead *psIpHead,unsigned char *pkg,int l);    /* ������   */
    struct ncPkgAnyFun_s *next;
} ncPkgAnyFun;

typedef struct ncPkgRawFun_s {
    char  caName[32];                                         /* ˵��       */
    int (*ncPkgDo)(unsigned char *pkg,int l);    /* ������   */
    struct ncPkgRawFun_s *next;
} ncPkgRawFun;



#endif
