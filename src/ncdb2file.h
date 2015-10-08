#ifndef  __NCCVTDB2FILE_DEF__
#define  __NCCVTDB2FILE_DEF__

#define NCCVT_DEFSID                "131253768809244361"
#define NCCVT_DBFROM_MYSQL              1 
#define NCCVT_DBFROM_MAILFILE           2
#define NCCVT_DBFROM_POSTFILE           3 
#define NCCVT_MAX_DOFIELD            64     // �������ֶ���
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

#define NCCVT_DBFROM_SID            120     // ��ǰʱ���Ӧ�����к�
#define NCCVT_DBFROM_ZERO           121
#define NCCVT_DBFROM_NULL           122
#define NCCVT_MAXFIELD              256

#define NCCVT_TYPE_DATE01           201    // ��ʽΪ '%Y-%m-%d %H:%i:%S' ������
#define NCCVT_TYPE_DATE02           202    // ��ʽΪ '%Y/%m/%d %H:%i:%S' ������
#define NCCVT_TYPE_LONGIP           203    // Long IP��ʽ

// �û���Ϣ��
typedef struct ncDbCvtUserInfo_s {
        uint4  did;                                 // ��λID
        uint4  uid;                                 // �û�ID
        uint4  gid;                                 // ����
        char   mac[8];                              // Mac��ַ
        char   username[30];                        // �û���¼��
        char   dispname[32];                        // �û���ʾ��
        char   position[32];                        // λ��
        char   idno[20];                            // ���֤����
        char   idtype[4];                           // ���֤���
        uint4  lasttime;                            // ���һ��ʹ��ʱ��
}  ncDbCvtUserInfo; 


    
// ��λ��������Ϣ���ձ�
typedef struct ncDbCvtDeptInfo_s {
        uint4  did;                                // ��λID
        uint4  tid;                                // ��λ���
        char   servicecode[16];                    // �����˱���
} ncDbCvtDeptInfo; 



typedef struct ncDbCvtFieldList_s {
        unsigned char    fname[64];                     // Դ�����ֶ���
        unsigned char    dfname[32];                    // Ŀ���ֶ���
        unsigned char    iFrom;                         // ��Դ
        unsigned char    iType;                         // ��������
        char             iSelect;                       // 
        unsigned char    iRev;                          // ��Ӧ�����ļ����е����
        uint4            iMax;                          // ��������ֽ���
} ncDbCvtFieldList;


typedef struct ncDbCvtTableList_s {
        char    stable[32];
        char    dtable[32];
        int     iSumField;
        ncDbCvtFieldList  *psField;
        struct ncDbCvtTableList_s  *next;
} ncDbCvtTableList;



#endif