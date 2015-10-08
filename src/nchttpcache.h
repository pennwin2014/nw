#ifndef __NCHTTPCACHE__
#define __NCHTTPCACHE__
/* DNS ����     NC_LNK_HTTPCACHEINDEX   70         ����
                NC_LNK_HTTPCACHECONT    71         ����
        �������ڴ��б�����Hash��
 */
typedef struct ncHttpCache_s {
	char            url[128];     /* host/url                              */
	unsigned long   lPos;         /* �ļ�λ��                              */
	unsigned long   lsize;        /* �ļ���С                              */
	unsigned long   modtime;      /* ����ʱ��                              */
	unsigned long   count;        /* ���ʴ���                              */
	int             flags;        /* 0--�ڴ�  >0 �ļ�                      */
} ncHttpCache;

typedef struct ncHttpCacheHead_s {
	char                headid[64];     /* nchttpcachehead                       */
	int                 iUse;           /* 1--��Hash������ά������ʱ���ṩ����   */
	unsigned long long  lMaxDiskSize;   /* �����̿ռ�                          */
	unsigned long long  lMaxMemSize;    /* ����ڴ��С                          */
	unsigned long       lMaxHttpSize;   /* ����ļ���С��ֻ�бȸ��ļ�С�Ĳ���cache */
	unsigned long       lCurFileno;     /* �ļ��������                          */
	unsigned long       lCurDiskSize;   /* ��ǰ���ÿռ�                          */
    unsigned long       lCacheKeep;     /* Cache�����ʱ��                       */
} ncHttpCacheHead;

#endif

