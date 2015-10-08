#ifndef __NCUPDATEDEF__
#define __NCUPDATEDEF__
#define NC_UPDATE_KEY    "Pro*&6%432asd"
#define NC_UPDATE_ID     65041133L
#define NC_UPDATE_FUN_STATUS     11
typedef  struct proEimProdInfo_s {
    unsigned char caProdSn[16];      /* ��Ʒ���к�  */
    unsigned char caVersion[16];     /* �汾��      */
    unsigned char caHostInfo[48];    /* ������Ϣ    */
    unsigned long lUsers;            /* �û���      */
    unsigned long lExpire;           /* ʧЧ����    */
    unsigned long lFun;              /* ������      */
    int iLicense;                    /* 1--���� 2--���� <=0 ����ȷ  */
    unsigned long lStartTime;
    unsigned long lOnlineUser;
    unsigned long lOnlineIp;
} proEimProdInfo;

typedef struct proEimPkgHead_s {
        unsigned long   lId;         /* �ļ�ID, �����ֽ�     */
        unsigned short  nFun;        /* ���ܴ���, �����ֽ�   */
        unsigned short  nLen;        /* ����, �����ֽ�       */
} proEimPkgHead;

#endif