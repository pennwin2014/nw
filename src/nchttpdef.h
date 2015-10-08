/* nchttpdef.h       
                         Create By Liyunming at 2004/03/08
*/                         
#ifndef __NCHTTPDEF__
#define __NCHTTPDEF__

#define NC_HTTP_XHTTP          0                 /* 转发到http服务器  */
#define NC_HTTP_XPROXY         1                 /* 转发到下一个Proxy */

#define NC_HTTP_RHTTP          1                 /* Http请求          */
#define NC_HTTP_RPROXY         2                 /* Http Proxy请求    */
#define NC_HTTP_RSSL           3                 /* SSL Proxy请求     */

#define NC_HTTP_CACHE_DISC     1
#define NC_HTTP_CACHE_MEM      0
/* DNS 缓存     NC_LNK_HTTPCACHEINDEX   70         索引
                NC_LNK_HTTPCACHECONT    71         内容
        这是在内存中保留的Hash表
 */
typedef struct ncHttpCache_s {
	char            url[128];     /* host/url                              */
	unsigned long   lPos;         /* 文件位置                              */
	unsigned long   lsize;        /* 文件大小                              */
	unsigned long   modtime;      /* 创建时间                              */
	unsigned long   count;        /* 访问次数                              */
	int             flags;        /* 0--内存  >0 文件                      */
} ncHttpCache;

typedef struct ncHttpCacheHead_s {
	char                headid[64];     /* nchttpcachehead                       */
	int                 iUse;           /* 1--该Hash表正在维护，暂时不提供服务   */
	unsigned long       lMaxFileno;     /* 文件的最大数量                        */
	unsigned long       lMaxHttpSize;   /* 最大文件大小，只有比该文件小的才做cache */
	unsigned long       lCurFileno;     /* 文件所用序号                          */
    unsigned long       lCacheKeep;     /* Cache保存的时间                       */
	unsigned long long  lMaxDiskSize;   /* 最大磁盘空间                          */
	unsigned long long  lMaxMemSize;    /* 最大内存大小                          */
	unsigned long long  lCurDiskSize;   /* 当前所用空间                          */
} ncHttpCacheHead;

#endif

