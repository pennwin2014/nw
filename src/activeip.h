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
    unsigned char ip_addr[4];    /* �ͻ���IP��ַ. */
    char user_id[18];            /* �û���ʶ��. */
    char user_name[20];          /* �û���   */
    long auto_submit_time;       /* �Զ�����ʱ��*/
    time_t start_time;           /* ��ʼʹ�õ�ʱ��. */
    time_t last_active_time;     /* ����ʱ��. */
    long up_flow; 
    long down_flow; 
    char status;
#define _AF_UNUSED 0             /* �ñ���Ŀǰû�õ�. */
#define _AF_START  1             /* �ñ��ǰ���ڼ�ʱ. */
#define _AF_END    2             /* �ñ��ǰ�Ѿ�������ʱ, ����û�б��浽���ݿ���. */
  };


#define _ACTIVE_KEY  8787        /* active �����ڴ��key. */
#define _ACTIVEIPTABLE_COUNT 300 /* �IP�������. */

#endif
