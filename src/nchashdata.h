#ifndef __NCHASHDATADEF___
#define __NCHASHDATADEF___
/* ���һЩ��ʱ����    
   NC_LNK_HASHDATA  �б��������
*/
#define NC_HASH_MSNF1       1         /* MSN�ļ�����Ϣ, �ļ���������  */
#define NC_HASH_MSNF2       2         /* MSN�ļ�����Ϣ, P2P���Ӻ���Ϣ */
#define NC_HASH_MAIL        3         /* �ʼ���ַ��Ϣ                 */
#define NC_HASH_POST        3         /* 2009/04/02 ��webmail��post���ϲ�   */
#define NC_HASH_FTP         6         /* 6*100 + port                 */
#define NC_HASH_POSTUID     7         /* �û���                       */
#define NC_HASH_MSNNAME     8         /* Msn�˺�                      */
#define NC_HASH_MSNTMP      1
#define NC_HASH_QQNUMBER    9         /* Qq��                         */
typedef struct ncHashData_s {
    unsigned long  lId;        
    unsigned long  nType;        /* ����                 */
    unsigned long  lLastTime;    /* ������ʱ��         */
    unsigned long  lSid;
    unsigned char  caValue[64];  /* ����                 */
} ncHashData;

typedef struct ncHashMailInfo_s {
    unsigned long  lSip;      /* ԴIP��ַ            */
    unsigned long  lDip;      /* Ŀ��Ip��ַ          */
    unsigned char  caTo[56];  /* ����                */
} ncHashMailInfo;


typedef struct ncHashPostInfo_s {
    unsigned long  lSip;      /* ԴIP��ַ            */
    unsigned long  lDip;      /* Ŀ��Ip��ַ          */
    unsigned char  cCode;     /* ����                */
    unsigned char  cRev[3];
    unsigned long  lCtime;    /* ����¼ʱ��        */
    unsigned char  caTo[56];  /* ����                */
} ncHashPostInfo;



typedef struct ncHashMsnTemp_s {
    unsigned long  lSid;
    unsigned long  nType;        /* ����                 */
    unsigned long  nLen;         /* ����                 */
    unsigned long  lLastTime;    /* ������ʱ��         */
    char     *pData;
} ncHashMsnTemp;


#endif
