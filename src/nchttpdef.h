/* nchttpdef.h       
                         Create By Liyunming at 2004/03/08
*/                         
#ifndef __NCHTTPDEF__
#define __NCHTTPDEF__

#define NC_HTTP_XHTTP          0                 /* ת����http������  */
#define NC_HTTP_XPROXY         1                 /* ת������һ��Proxy */

#define NC_HTTP_RHTTP          1                 /* Http����          */
#define NC_HTTP_RPROXY         2                 /* Http Proxy����    */
#define NC_HTTP_RSSL           3                 /* SSL Proxy����     */

#define NC_HTTP_CACHE_DISC     1
#define NC_HTTP_CACHE_MEM      0
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
	unsigned long       lMaxFileno;     /* �ļ����������                        */
	unsigned long       lMaxHttpSize;   /* ����ļ���С��ֻ�бȸ��ļ�С�Ĳ���cache */
	unsigned long       lCurFileno;     /* �ļ��������                          */
    unsigned long       lCacheKeep;     /* Cache�����ʱ��                       */
	unsigned long long  lMaxDiskSize;   /* �����̿ռ�                          */
	unsigned long long  lMaxMemSize;    /* ����ڴ��С                          */
	unsigned long long  lCurDiskSize;   /* ��ǰ���ÿռ�                          */
} ncHttpCacheHead;

#endif

