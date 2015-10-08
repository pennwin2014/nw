#ifndef __NCBILLDEF__
#define __NCBILLDEF__
#define NC_BILL_WORKGROUP      101      /* 计费组    */
#define NC_BILL_ENDGROUP       201      /* 结束Group */
#define NC_BILL_DEFEXPIRE      2592000L /* 帐户缺省超时时间 30天   */
#define BILL_TYPE_PREFEE       1        /* 预付费  */
#define BILL_TYPE_AFTERFEE     0        /* 后付费  */

#define BILL_TIMEWAY_CONNECT   0        /* 连接时间   */
#define BILL_TIMEWAY_REAL      1       /* 实际使用时间  */
/* 时间  */
typedef struct ncBillTime_s {
    unsigned long lUtime;      /* 时间上限 0--不限,秒   */
    double   dBasefee;         /* 基本费,    元         */
    double   dTimerate;        /* 时间费率   元/小时    */
} ncBillTime;

typedef struct ncBillFlow_s {
    unsigned long long lUflow;      /* 流量上限 0--不限,字节   */
    double   dBasefee;              /* 基本费             */
    double   dFlowrate;             /* 流量费率  元/MB    */
} ncBillFlow;

/* 促销信息  */
typedef struct ncBillProm_s {
    unsigned long lStart;      /* 起始时间           */
    unsigned long lEnd;        /* 终止时间           */
    double   dRate;            /* 优惠率             */
} ncBillProm;


typedef struct ncBillRate_s {
    unsigned long lStart;      /* 起始时间           */
    unsigned long lEnd;        /* 终止时间           */
    double   dRate;            /* 优惠率             */
    struct   ncBillRate_s   *next;
} ncBillRate;


typedef struct ncBill_s {
    unsigned long  lId;         /* 计费策略           */
    char           feename[32]; /* 策略名称          */
    unsigned char  feetype;    /* 付费方式 0-先用后付 1-预付费 */
    unsigned char  feeway;     /* 结算方式 0-一次性结算  1-每天   2 每周  3-每月  4-每季   5-每年  */
    unsigned short timeway;    /* 时间计算方式 0-退出和开始之差  1-实际数据包时间    */
    unsigned long  feeday;     /*                     */
    unsigned long  feemon;     /*                     */
    unsigned long  feehou;     /*                     */
    int            sumtime;
    int            sumflow;
    int            sumprom;
    ncBillTime    *psTime;
    ncBillFlow    *psFlow;
    ncBillProm    *psProm;
} ncBill;

typedef struct ncBillHead_s {
    ncBill          *psBill;
    ncBillTime      *psTime;
    ncBillFlow      *psFlow;
    ncBillProm      *psProm;
    unsigned long   sumbill;
    unsigned long   lStarttime;     /* 当前起始时间          */
    unsigned long   lStoptime;      /* 当前终止时间          */
    unsigned long   lLoadtime;      /* 计费策略装载时间      */
    unsigned long   lLasttime;      /* 计费策略最后使用时间  */
    unsigned long   lDocount;       /* 操作次数              */
} ncBillHead;


int billLoadBillTime(ncBillHead *psHead);
int billLoadBillFlow(ncBillHead *psHead);
ncBillRate *bilUtlAdd0(ncBillRate *ps0,ncBillRate *ps1,
                 unsigned long stime,unsigned long etime,double rate);
ncBillRate *bilUtlAdd(ncBillRate *psRate,unsigned long stime,unsigned long etime,
                double rate);
int bilUtlIsInDid(unsigned long *lpId1,int iSum1,unsigned long dtype);
int billLoadBillRate(ncBillHead *psHead,
              unsigned long lStartTime,unsigned long lEnd);
char *billCvtTime(long lTime);
int billLoadBill(unsigned long lStartTime);
ncBill *billGetBill(ncBill *psBill,int iSumbill,unsigned long lFeeid);
double  billCompFee0(ncBill *psBill,int iSumbill,
                                  unsigned long lFeeid,
                                  unsigned long *lSumtime0,
                                  unsigned long long *lSumflow0,
                                  unsigned long lStarttime,
                                  unsigned long lThistime,
                                  unsigned long long lThisflow); 
double  billCompFee(unsigned long lFeeid,
                                  unsigned long *lSumtime0,
                                  unsigned long long *lSumflow0,
                                  unsigned long lStarttime,
                                  unsigned long lThistime,
                                  unsigned long long lThisflow);                                                    

int billCompTime(unsigned long lFeeid,
                                  unsigned long lStarttime,
                                  unsigned long lThistime,
                                  unsigned long *lRealtime,
                                  unsigned long long lThisflow,
                                  unsigned long long *lRealflow);
                                  
double billCompFee_Time(ncBill *ps,unsigned long lSumtime);
double billCompFee_Flow(ncBill *ps,unsigned long long lSumflow);
double billCompOneFee(ncBill *ps,unsigned long *lSumtime0,unsigned long long *lSumflow0,
                         unsigned long lStarttime,unsigned long lThistime,unsigned long long lThisflow);

#endif
