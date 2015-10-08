#ifndef __NCNSAQOSDEF__
#define __NCNSAQOSDEF__
#define NC_QOS_MAXONEDEV     8
#define NC_NSAQOS_MAXDEV    16
#include "ncpkg.h"

/* ��·����          
   
              action-- 0x10-��ת��/���� 
                    -- 0x08-ֱ�Ӷ���
                    -- 0x60-ת�����
                    -- 0x40-ת��    
 */
typedef struct ncNsaQosLine_s {
    int            cid;
    int            nsaid;
    unsigned char  cInDevId[NC_QOS_MAXONEDEV];            /* ���豸ID,��0��ʼ           */
    unsigned char  cOutDevId[NC_QOS_MAXONEDEV];           /* ���豸ID,��0��ʼ           */
    unsigned char  cSumInDev;                             /* ��������            */
    unsigned char  cSumOutDev;                            /* ��������            */
    unsigned short status;              /* 0--ͣ��  1--����    */
    int            action;              /* ����                 */
    unsigned long  bandrate;            /* ��������,���е�λ KBps         */
    unsigned long  uprate;              /* �������ʣ�����                 */
} ncNsaQosLine;


/* QosId
      һ��: 1-1024   
      ����: 1-1891
      */
typedef struct ncNsaQosInfo_s {
    unsigned long  lQosId;      /* QOS�߼�ID                                 */    
    unsigned long  lQos0;       /* �ϼ�QOS���,��1��ʼ, 0��ʾ�����          */
    unsigned long  lRid[2];     /* �����ʵ��QOSID,Ҳ����nsa��ʵ��ʹ�õ�     */
    unsigned long  lMin[2];     /* ������֤,��λ KBps  0--����  1--����      */
    unsigned long  lMax[2];     /* ��������,��λ KBps  0--����  1--����      */
    unsigned char  cLevel;      /* ͨ������,1--1��  2--2��                   */
    unsigned char  cPrio;       /* ���ȼ���                                  */
    unsigned char  cBounded;    /* �ӱ�ĵط����ô���                        */
    unsigned char  cIsolated;   /* ���ô������ĵط�                        */
    unsigned char  cType;       /* 0--����   1--����                         */
    unsigned char  cLine;       /* ��·                                      */
    unsigned char  cRev[2];     /* ����                                      */
} ncNsaQosInfo;


/* NC_LNK_QOSID    
     QOS ID �����ڹ����ڴ���
   ����һ��Hash��
 */
typedef struct ncNsaQosID_s {
    unsigned long  lUid;              // IP��ַ,���� Uid=0
    unsigned long  lQos;              // �ڲ�ʹ�õ�ID
    unsigned long  lQosId_s;          // ��̬QOS
    unsigned long  lQosId_r;          // ��̬QOS
    unsigned long  lLastTime;         // ���ʹ��ʱ��
} ncNsaQosID;   


typedef struct ncNsaQosIdHead_s {
    unsigned long  lStartTime;       // ��ʼʹ��ʱ��
    unsigned long  lQosId1;          // Ŀǰһ��QOS���ID
    unsigned long  lQosId2;          // Ŀǰ����QOS���ID
    unsigned long  lMaxQosId1;       // �������һ��QOSID 
    unsigned long  lMaxQosId2;       // ����������QOSID 
    unsigned char  *pHash;
} ncNsaQosIdHead;


typedef struct ncNsaQosHead_s {
    int             iOpt;           /* ���Ʒ�ʽ                             */
    int             iSumLine;       /* ��·����                             */
    int             iSumQos;        /* Qos��������                          */
    ncNsaQosLine    *psLine;        /* �豸��Ϣ                             */
    ncNsaQosInfo    *psQos;         /* QOS��Ϣ                              */
    ncNsaQosIdHead  *psQosId;       /* ��̬QOS,��Ϣ�����ڹ����ڴ���         */
    ncPkgCapDevHead *psDevHead;     /* ����豸���                         */
    utShmHead       *psShmHead;
} ncNsaQosHead;


typedef struct ncMetaTcClassFlow_s {
    unsigned long        lQos;
    int         iPrio;
    unsigned long        lMin; 
    unsigned long        lMax;
    unsigned long        lSent;
    unsigned long        lPkts;
    unsigned long        lRate;
    unsigned long        lPps;
    struct ncMetaTcClassFlow *next;
} ncMetaTcClassFlow;


typedef struct ncMetaTcFilterFlow_s {
    unsigned long        lQos;
    unsigned long        lQos0;   /* �ϼ�QOSID  */
    unsigned long        lRate;
    unsigned long        lSent;
    unsigned long        lPkts;
    struct ncMetaTcFilterFlow *next;
} ncMetaTcFilterFlow;

typedef struct ncMetaTcInfo_s {
   int iSumFilter;
   int iSumClass;
   ncMetaTcFilterFlow *psFilter;
   ncMetaTcClassFlow  *psClass;
} ncMetaTcInfo;

#endif
