// ѹ���ļ�ϵͳ
#ifndef  ___PASZLDFILE_DEF___
#define  ___PASZLDFILE_DEF___

#define PAS_ZLDFILEID     1269558547L

typedef struct pasZldHead_s {
    uint4  lId;                    // �ļ���ʼ��־
    uint4  lSlen;                  // ѹ��ǰ�ֽ���
    uint4  lDlen;                  // ѹ�����ֽ���
} pasZldHead;


typedef struct pasZldRecord_s {
    uint4  lSlen;                  // ѹ��ǰ�ֽ���
    uint4  lDlen;                  // ѹ�����ֽ���
} pasZldRecord;


#define PAS_ZLDMODE_READ       1
#define PAS_ZLDMODE_WRITE      2
#define PAS_ZLDMODE_APPEND     3

typedef struct pasZldFile_s {
    pasZldHead    sHead;
    FILE          *fp;             // �ļ���ʶ
    int           mode;            // 
    int           isNew;           // 0-- ���ļ�  1--���ļ�
} pasZldFile;


#endif