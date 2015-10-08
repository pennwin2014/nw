#ifndef __NCNSAQOSDEF__
#define __NCNSAQOSDEF__
#define NC_QOS_MAXONEDEV     8
#define NC_NSAQOS_MAXDEV    16
#include "ncpkg.h"

/* 线路描述          
   
              action-- 0x10-不转发/分析 
                    -- 0x08-直接丢弃
                    -- 0x60-转发审计
                    -- 0x40-转发    
 */
typedef struct ncNsaQosLine_s {
    int            cid;
    int            nsaid;
    unsigned char  cInDevId[NC_QOS_MAXONEDEV];            /* 进设备ID,从0开始           */
    unsigned char  cOutDevId[NC_QOS_MAXONEDEV];           /* 出设备ID,从0开始           */
    unsigned char  cSumInDev;                             /* 进线总数            */
    unsigned char  cSumOutDev;                            /* 出线总数            */
    unsigned short status;              /* 0--停用  1--启用    */
    int            action;              /* 动作                 */
    unsigned long  bandrate;            /* 带宽速率,下行单位 KBps         */
    unsigned long  uprate;              /* 带宽速率，上行                 */
} ncNsaQosLine;


/* QosId
      一级: 1-1024   
      二级: 1-1891
      */
typedef struct ncNsaQosInfo_s {
    unsigned long  lQosId;      /* QOS逻辑ID                                 */    
    unsigned long  lQos0;       /* 上级QOS序号,从1开始, 0表示无序号          */
    unsigned long  lRid[2];     /* 分配的实际QOSID,也即在nsa中实际使用的     */
    unsigned long  lMin[2];     /* 流量保证,单位 KBps  0--上行  1--下行      */
    unsigned long  lMax[2];     /* 带宽限制,单位 KBps  0--上行  1--下行      */
    unsigned char  cLevel;      /* 通道级别,1--1级  2--2级                   */
    unsigned char  cPrio;       /* 优先级别                                  */
    unsigned char  cBounded;    /* 从别的地方借用带宽                        */
    unsigned char  cIsolated;   /* 借用带宽给别的地方                        */
    unsigned char  cType;       /* 0--共享   1--独享                         */
    unsigned char  cLine;       /* 线路                                      */
    unsigned char  cRev[2];     /* 保留                                      */
} ncNsaQosInfo;


/* NC_LNK_QOSID    
     QOS ID 保存在共享内存中
   这是一个Hash表
 */
typedef struct ncNsaQosID_s {
    unsigned long  lUid;              // IP地址,共享 Uid=0
    unsigned long  lQos;              // 内部使用的ID
    unsigned long  lQosId_s;          // 动态QOS
    unsigned long  lQosId_r;          // 动态QOS
    unsigned long  lLastTime;         // 最后使用时间
} ncNsaQosID;   


typedef struct ncNsaQosIdHead_s {
    unsigned long  lStartTime;       // 开始使用时间
    unsigned long  lQosId1;          // 目前一级QOS最大ID
    unsigned long  lQosId2;          // 目前二级QOS最大ID
    unsigned long  lMaxQosId1;       // 可用最大一级QOSID 
    unsigned long  lMaxQosId2;       // 可用最大二级QOSID 
    unsigned char  *pHash;
} ncNsaQosIdHead;


typedef struct ncNsaQosHead_s {
    int             iOpt;           /* 控制方式                             */
    int             iSumLine;       /* 线路总数                             */
    int             iSumQos;        /* Qos策略总数                          */
    ncNsaQosLine    *psLine;        /* 设备信息                             */
    ncNsaQosInfo    *psQos;         /* QOS信息                              */
    ncNsaQosIdHead  *psQosId;       /* 动态QOS,信息保存在共享内存中         */
    ncPkgCapDevHead *psDevHead;     /* 监控设备句柄                         */
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
    unsigned long        lQos0;   /* 上级QOSID  */
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
