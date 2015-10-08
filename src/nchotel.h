#ifndef __NCHOTELDEF___
#define __NCHOTELDEF___
/* 宾旅馆同步信息，　保存在　NC_LNK_HOTELINFO　中  */
typedef struct ncHotelUser_s {
    char caRoom[16];    /* 房间号    */
    char caName[32];    /* 姓名      */
    char caIdType[4];   /* 证件类别  */
    char caIdNumber[20]; /* 证件号码  */
    unsigned long lInTime; /* 入住时间  */
    unsigned long lOutTime; /* 退房时间  */
    unsigned long lSynTime; /* 同步时间  */
} ncHotelUser;

    
#endif
