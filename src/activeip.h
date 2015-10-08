#ifndef _ACTIVEIP_H_
#define _ACTIVEIP_H_ 1

/*
 * $Log: activeip.h,v $
 * Revision 1.1  1998/10/12 14:32:35  yhuang
 * Initial revision
 *
 * Revision 1.1  1998/10/11 15:14:39  yhuang
 * Initial revision
 *
 */

struct active_ip
  {
    unsigned char ip_addr[4];    /* 客户的IP地址. */
    char user_id[18];            /* 用户标识名. */
    char user_name[20];          /* 用户名   */
    long auto_submit_time;       /* 自动下网时间*/
    time_t start_time;           /* 开始使用的时间. */
    time_t last_active_time;     /* 结束时间. */
    long up_flow; 
    long down_flow; 
    char status;
#define _AF_UNUSED 0             /* 该表项目前没用到. */
#define _AF_START  1             /* 该表项当前正在计时. */
#define _AF_END    2             /* 该表项当前已经结束计时, 但还没有保存到数据库中. */
  };


#define _ACTIVE_KEY  8787        /* active 共享内存的key. */
#define _ACTIVEIPTABLE_COUNT 300 /* 活动IP表的项数. */

#endif
