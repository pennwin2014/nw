#ifndef __NETWARNDEF__
#define __NETWARNDEF__
/* Following Add at 2006/02/06
/* ���綽��澯ƽ̨  iType ������ */
#define NC_WARNT_MAXPKG        1      /* ���ݰ���������  */
#define NC_WARNT_MAXFLOW       2      /* ������������    */
#define NC_WARNT_MAXIP         3      /* IP������������  */
#define NC_WARNT_MAXMAIL       4      /* �ʼ���������    */
#define NC_WARNT_ACCESS        5      /* ����Υ��        */
#define NC_WARNT_FINDPROXY     6      /* ���ִ��������  */
#define NC_WARNT_CONTROL       7      /* ���ʿ��Ƹ澯    */

/* ����һ��Hash��  */
typedef struct ncWarnSystem_s {
    unsigned long  lUserid;       /* �û�ID     */
    unsigned long  lWarnid;       /* �澯ID     */
    unsigned char  cType;         /* �澯���   */
    unsigned char  cFlags;        /* �澯��ʶ  1-�µĸ澯��Ϣ 2-�Ѿ����� */
    unsigned short nCount;        /* ���ִ���       */
    unsigned long  lTime;         /* �澯ʱ��(���һ��)  */
    unsigned long  lLastTime;     /* ���һ�δ���ʱ��    */
    char     caMesg[128];         /* �澯����   */
} ncWarnSystem;


#endif