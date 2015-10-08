#ifndef  __NCCVTDB2FILE_DEF__
#define  __NCCVTDB2FILE_DEF__

#define NCCVT_DEFSID                "131253768809244361"
#define NCCVT_DBFROM_MYSQL              1 
#define NCCVT_DBFROM_MAILFILE           2
#define NCCVT_DBFROM_POSTFILE           3 
#define NCCVT_MAX_DOFIELD            64     // 最大处理的字段数
#define NCCVT_DBFROM_ServiceCode    100
#define NCCVT_DBFROM_Username       101
#define NCCVT_DBFROM_Dispname       102
#define NCCVT_DBFROM_Mac            103
#define NCCVT_DBFROM_Position       104
#define NCCVT_DBFROM_Idno           105
#define NCCVT_DBFROM_Idtype         106
#define NCCVT_DBFROM_Did            107
#define NCCVT_DBFROM_Tid            108
#define NCCVT_DBFROM_Gid            109

#define NCCVT_DBFROM_SID            120     // 当前时间对应的序列号
#define NCCVT_DBFROM_ZERO           121
#define NCCVT_DBFROM_NULL           122
#define NCCVT_MAXFIELD              256

#define NCCVT_TYPE_DATE01           201    // 格式为 '%Y-%m-%d %H:%i:%S' 的日期
#define NCCVT_TYPE_DATE02           202    // 格式为 '%Y/%m/%d %H:%i:%S' 的日期
#define NCCVT_TYPE_LONGIP           203    // Long IP格式

// 用户信息表
typedef struct ncDbCvtUserInfo_s {
        uint4  did;                                 // 单位ID
        uint4  uid;                                 // 用户ID
        uint4  gid;                                 // 组名
        char   mac[8];                              // Mac地址
        char   username[30];                        // 用户登录名
        char   dispname[32];                        // 用户显示名
        char   position[32];                        // 位置
        char   idno[20];                            // 身份证号码
        char   idtype[4];                           // 身份证类别
        uint4  lasttime;                            // 最后一次使用时间
}  ncDbCvtUserInfo; 


    
// 单位场所端信息对照表
typedef struct ncDbCvtDeptInfo_s {
        uint4  did;                                // 单位ID
        uint4  tid;                                // 单位类别
        char   servicecode[16];                    // 场所端编码
} ncDbCvtDeptInfo; 



typedef struct ncDbCvtFieldList_s {
        unsigned char    fname[64];                     // 源数据字段名
        unsigned char    dfname[32];                    // 目标字段名
        unsigned char    iFrom;                         // 来源
        unsigned char    iType;                         // 数据类型
        char             iSelect;                       // 
        unsigned char    iRev;                          // 对应数据文件中列的序号
        uint4            iMax;                          // 数据最大字节数
} ncDbCvtFieldList;


typedef struct ncDbCvtTableList_s {
        char    stable[32];
        char    dtable[32];
        int     iSumField;
        ncDbCvtFieldList  *psField;
        struct ncDbCvtTableList_s  *next;
} ncDbCvtTableList;



#endif