/* PAS���ļ���������  */
#ifndef __PASFILE_DEF_
#define __PASFILE_DEF_
typedef struct pasFile_s {
    int            iFd;               /* �ļ����                          */
    char           *pBuf;             /* �ļ�������                        */
    unsigned long  lBufLen;           /* �ļ��������Ĵ�С��ȱʡΪ 8192     */
    unsigned long  lOffSet;           /* �ļ���ǰλ���ڻ������е�ƫ����    */
    unsigned long  lBufStart;         /* ��������һ��ָ���Ӧ�ļ���λ��    */
    unsigned long  lLen;              /* ���������ݵĴ�С                  */
    unsigned long  lSize;             /* �ļ���С                          */
} pasFile;

#endif
