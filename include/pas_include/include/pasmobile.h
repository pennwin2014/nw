/* �ֻ���Ӫ��
   
*/
#ifndef __PASMOBILEDEF__
#define __PASMOBILEDEF__



// �ֻ���Ӫ�̶���
#define PAS_MOBILE_INVALID             0       /* �ǹ����ֻ�              */
#define PAS_MOBILE_CHINAMOBILE         1       /* �й��ƶ�  CHINAMOBILE   */
#define PAS_MOBILE_CHINAUNICOM         2       /* ��ͨ      CHINAUNICOM   */
#define PAS_MOBILE_CHINATELECOM        3       /* �й�����  CHINATELECOM  */
#define PAS_MOBILE_UNKNOW              8       /* ����        */
#define PAS_MOBILE_INTERNATIONAL       9       /* ����      INTERNATIONAL */



#define PAS_MOBILE_2G                  100     /* 2G        */
#define PAS_MOBILE_3G                  110     
#define PAS_MOBILE_G3                  120 
#define PAS_MOBILE_NOTEBOOK            130
#define PAS_MOBILESP_HEADLEN           24

#define PAS_MOBILE_UNKNOWAREA          999
// ��Ӫ�̺���ʽ
typedef struct pasMobileType_s {
    uint2 cType;
    uchar caName[14];
} pasMobileType;



// �ֻ��Ŷζ��ձ�
typedef struct pasMobileSeg_s {
    int2   nNum;
    uchar  cType;
    uchar  cSp;
} pasMobileSeg;


typedef struct pasMobileSpHead_s {
    uint2           nId;                 // �ļ�ID
    uint2           nVersion;            // �汾
    uint4           lModiTime;           // ����ʱ��
    uint4           nSumNumber;          
    uint4           lSize;               // ���ݳ���
    uint4           lCheckSum;
    uint2           nSumType;            //
    uint2           nRev;
    uchar           *pBuf;
    pasMobileType   *psType;
    pasMobileSeg    *psSeg;
} pasMobileSpHead;

typedef struct pasMobileNum_s {
    uint4  number;             // ����7λ
    uint2  area;               // ����2λ
    uchar  cType;              // �ƶ���� 3G  2G
    uchar  cSp;                // ��Ӫ��
} pasMobileNum;


typedef struct pasMobileNumHead_s {
    uint2           nId;                 // �ļ�ID
    uint2           nVersion;            // �汾
    uint4           lModiTime;           // ����ʱ��
    uint4           nSumNumber;          // ��������
    uint4           lSize;               // ���ݳ���
    uint4           lCheckSum;
    uint2           nSumType;            // ��������
    uint2           nRev;
    uchar           *pBuf;
    pasMobileType   *psType;
    pasMobileNum    *psNum;
} pasMobileNumHead;

// �����ֻ�
typedef struct pasMobileFor_s {
    uint2  areanum;            // ����
    uchar  lenmin;
    uchar  lenmax;
    uchar  prenum[12];         // ǰ׺
    uchar  cname[32];
    uchar  ename[32];
} pasMobileFor;


int pasCheckMobileNumber(char *pStr,char *pSpname,char *pType);
int pasUtlFreeMobileSp();
int pasUtlFreeMobileArea();
int pasUtlLoadMobileArea(char *pFile);
int pasCheckMobileArea(char *pStr,char *pSpname,char *pType,char *pArea);
int pasUtlLoadMobileAreaFromTxtFile(char *pFile,char *pOutFile,uint4 lMaxType,uint4 lMaxNumber);
int utStrIsMobileNumber(char *pStr);
int pasUtlLoadMobileAreaFromTxtFile(char *pFile,char *pOutFile,uint4 lMaxType,uint4 lMaxNumber);
#endif
