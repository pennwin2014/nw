#ifndef __NCHASHDATADEF___
#define __NCHASHDATADEF___
/* 存放一些临时数据    
   NC_LNK_HASHDATA  中保存的内容
*/
#define NC_HASH_MSNF1       1         /* MSN文件名信息, 文件传送请求  */
#define NC_HASH_MSNF2       2         /* MSN文件名信息, P2P连接后信息 */
#define NC_HASH_MAIL        3         /* 邮件地址信息                 */
#define NC_HASH_POST        3         /* 2009/04/02 将webmail和post表单合并   */
#define NC_HASH_FTP         6         /* 6*100 + port                 */
#define NC_HASH_POSTUID     7         /* 用户名                       */
#define NC_HASH_MSNNAME     8         /* Msn账号                      */
#define NC_HASH_MSNTMP      1
#define NC_HASH_QQNUMBER    9         /* Qq号                         */
typedef struct ncHashData_s {
    unsigned long  lId;        
    unsigned long  nType;        /* 类型                 */
    unsigned long  lLastTime;    /* 最后操作时间         */
    unsigned long  lSid;
    unsigned char  caValue[64];  /* 内容                 */
} ncHashData;

typedef struct ncHashMailInfo_s {
    unsigned long  lSip;      /* 源IP地址            */
    unsigned long  lDip;      /* 目标Ip地址          */
    unsigned char  caTo[56];  /* 内容                */
} ncHashMailInfo;


typedef struct ncHashPostInfo_s {
    unsigned long  lSip;      /* 源IP地址            */
    unsigned long  lDip;      /* 目标Ip地址          */
    unsigned char  cCode;     /* 编码                */
    unsigned char  cRev[3];
    unsigned long  lCtime;    /* 表单记录时间        */
    unsigned char  caTo[56];  /* 内容                */
} ncHashPostInfo;



typedef struct ncHashMsnTemp_s {
    unsigned long  lSid;
    unsigned long  nType;        /* 类型                 */
    unsigned long  nLen;         /* 长度                 */
    unsigned long  lLastTime;    /* 最后操作时间         */
    char     *pData;
} ncHashMsnTemp;


#endif
