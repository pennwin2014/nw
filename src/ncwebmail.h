/* Webmail�ķ���  */
#ifndef __NC_WEMAIL_DEF__
#define __NC_WEMAIL_DEF__
/* ����һ��Hash��  */
#define NC_WBM_From      1
#define NC_WBM_To        2
#define NC_WBM_Cc        3
#define NC_WBM_Bcc       4
#define NC_WBM_Conetnt   5
#define NC_WBM_Attach    6

typedef struct ncWebMailPlate_s {
    char  caHost[16];     /* ����������Ϊ0,��ʾȱʡ  */
    char  caName[16];     /* ����                    */
    int   iType;          /* ����  1-From 2-To  3-Cc  4-Bcc 5-Subject 6--Text 7--Attach  */
} ncWebMailPlate;

    
typedef struct ncWebMailWay_s {
    char  caHost[16];     /* ����������Ϊ0,��ʾȱʡ  */
    int   iWay;           /* �ʼ�������ʽ  0--��һ��  1--�ֿ�  */
} ncWebMailWay;

    

#endif
