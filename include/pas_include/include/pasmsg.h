/*   pasmsg.h
     PAS��Ϣ��ʽ
     2011/5/1
 */
#ifndef  __PASMSGDEF__
#define  __PASMSGDEF__

#include "utotype.h"


#define PAS_MSGHEAD_ID   "Pas4"
#define PAS_MSG_VERSION   0x00010000

#define PAS_MSGTYPE_PAS      0x01     // PASר�ø�ʽ
#define PAS_MSGTYPE_BINARY   0x02     // �����Ƹ�ʽ     
#define PAS_MSGTYPE_TEXT     0x03     // һ���ı���ʽ
#define PAS_MSGTYPE_POST     0x04     //  POST
#define PAS_MSGTYPE_XML      0x05     // XML
#define PAS_MSGTYPE_LIST     0x06     // LIST
#define PAS_MSGTYPE_FILE     0x07     // File 


/* Message Head */
typedef struct {
    uchar   MsgCode[4];     /*  ��Ϣͷ���̶�Pas4                    */
    uint4   Version;        /*  �汾��                              */
    uint4   Length;         /*  ���ݰ�����                          */
    uint4   CheckSum;       /*  Check Sum                           */
    uint4   SendId;         /*  Send Id                             */
    uint4   MsgId;          /*  Message Id                          */
    uint4   TimeStamp;      /*  ʱ���                              */
    uint4   FunCode;        /* ���ܴ���                             */
    uchar   Secutity;       /* ���ܱ�־ 0x00--������  0x03--DES����  */
    uchar   MsgType;        /* ��Ϣ�����ݸ�ʽ                        */
    uchar   CharSet;        /* �ַ�������   0x00-GBK  0x01--BIG5 0x02--UTF8
                                            0x04-UTF-16                 */
    uchar   Status;         /* ״̬                                  */
    uchar   AuthCode[16];   /* ��Ȩ��                                */
    uchar   cCurSta;        /* ��Ϣ��ǰ״̬ 0-Դ���� 1-�Ѵ���        */
    uchar   cRev[3];        /* ����                                  */
    uchar   Data[8];        /* ����                                  */
}  pasMsgHead;


#endif

