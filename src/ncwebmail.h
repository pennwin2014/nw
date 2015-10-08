/* Webmail的分析  */
#ifndef __NC_WEMAIL_DEF__
#define __NC_WEMAIL_DEF__
/* 这是一个Hash表  */
#define NC_WBM_From      1
#define NC_WBM_To        2
#define NC_WBM_Cc        3
#define NC_WBM_Bcc       4
#define NC_WBM_Conetnt   5
#define NC_WBM_Attach    6

typedef struct ncWebMailPlate_s {
    char  caHost[16];     /* 主机名，若为0,表示缺省  */
    char  caName[16];     /* 名字                    */
    int   iType;          /* 类型  1-From 2-To  3-Cc  4-Bcc 5-Subject 6--Text 7--Attach  */
} ncWebMailPlate;

    
typedef struct ncWebMailWay_s {
    char  caHost[16];     /* 主机名，若为0,表示缺省  */
    int   iWay;           /* 邮件附件方式  0--在一起  1--分开  */
} ncWebMailWay;

    

#endif
