/*
    ͨ����Ϣ
    
    ���� 2011/05/01    By Liyunming
*/
#ifndef  __PASMESGDEF__
#define  __PASMESGDEF__
#include "utotype.h"

#define PAS_MSGHEAD_ID_H    0x34736150  // MsgID "Pas4" �����ֽ���
#define PAS_MSGHEAD_ID_N    0x50617334  // MsgID "Pas4" �����ֽ���

#define PAS_MSG_VERSION   0x00010000

#define PAS_MSGTYPE_PAS      0x01     // PASר�ø�ʽ
#define PAS_MSGTYPE_BINARY   0x02     // �����Ƹ�ʽ
#define PAS_MSGTYPE_TEXT     0x03     // һ���ı���ʽ
#define PAS_MSGTYPE_POST     0x04     //  POST
#define PAS_MSGTYPE_XML      0x05     // XML
#define PAS_MSGTYPE_LIST     0x06     // LIST
#define PAS_MSGTYPE_FILE     0x07     // File

typedef struct pasMsgHead_s {
    uint4   lCode;       /*  ��Ϣ��ʶ,ͳһʹ���൱��"Pas4" ��ֵ  */
    uint4   lVersion;    /*  �汾��                              */
    uint4   lBytes;      /*  ��Ϣ���ݰ����ֽ���,����ͷ           */
    uint4   lCheckSum;   /*  CRC32У��ֵ(�ӵ�17���ֽڿ�ʼ����    */
    uint4   lSendId;     /*  ���Ͷ�ID                            */
    uint4   lMsgId;      /*  lMsgId                              */
    uint4   lSendTime;   /*  ����ʱ���                          */
    uint4   lFunCode;    /*  ���ܴ���                            */
    uchar   caAuthCode[16];  /* ��Ȩ��                           */
    uchar   cCrypt;      /*  0-- No Crypt 1--DES  2--Tea         */
    uchar   cMsgType;    /* ��Ϣ���ʽ   0x01--ר�ø�ʽ  
                                        0x02--������  
                                        0x03--һ���ı�  
                                        0x04--POST 
                                        0x05--XML 
                                        0x06--�����б� 
                                        0x07--�ļ�
                                                                 */
    uchar   cCharset;    /* �ַ�������                           */
    uchar   cStatus;     /* ״̬                                 */
    uint4   lNum;        /* �����������ڴ��е����                */
    uint4   l1;
    uchar   cOrder;      /* 0--�����ֽ�˳��   1--�����ֽ�˳��     */
    uchar   c2;
    uchar   c3;
    uchar   c4;
} pasMsgHead;



typedef struct pasMsgFileSend_s {
    uint4   lStart;             /* ��ʼλ��    */
    uint4   lBytes;             /* �ֽ���      */
    uint4   lSum;               /* �ܹ��ֽ���  */
} pasMsgFileSend;


/* pasmsg01.c */
int pasMsgSetMyInfo(unsigned long lSendId,unsigned char *caAuth);
int pasMsgHeadInit(pasMsgHead *psMsgHead,unsigned long lFunCode,unsigned long lBytes,int iMsgType);
int pasMsgHead_Ntoh(pasMsgHead *psMsgHead);
int pasMsgHead_Hton(pasMsgHead *psMsgHead);
int pasMsgIsValidMsg(pasMsgHead *psMsgHead);
int pasMsgPostVar(char *pBuf,int iSum,...);


#endif
