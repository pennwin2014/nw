#ifndef __NCHOTELDEF___
#define __NCHOTELDEF___
/* ���ù�ͬ����Ϣ���������ڡ�NC_LNK_HOTELINFO����  */
typedef struct ncHotelUser_s {
    char caRoom[16];    /* �����    */
    char caName[32];    /* ����      */
    char caIdType[4];   /* ֤�����  */
    char caIdNumber[20]; /* ֤������  */
    unsigned long lInTime; /* ��סʱ��  */
    unsigned long lOutTime; /* �˷�ʱ��  */
    unsigned long lSynTime; /* ͬ��ʱ��  */
} ncHotelUser;

    
#endif
