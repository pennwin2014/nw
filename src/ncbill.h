#ifndef __NCBILLDEF__
#define __NCBILLDEF__
#define NC_BILL_WORKGROUP      101      /* �Ʒ���    */
#define NC_BILL_ENDGROUP       201      /* ����Group */
#define NC_BILL_DEFEXPIRE      2592000L /* �ʻ�ȱʡ��ʱʱ�� 30��   */
#define BILL_TYPE_PREFEE       1        /* Ԥ����  */
#define BILL_TYPE_AFTERFEE     0        /* �󸶷�  */

#define BILL_TIMEWAY_CONNECT   0        /* ����ʱ��   */
#define BILL_TIMEWAY_REAL      1       /* ʵ��ʹ��ʱ��  */
/* ʱ��  */
typedef struct ncBillTime_s {
    unsigned long lUtime;      /* ʱ������ 0--����,��   */
    double   dBasefee;         /* ������,    Ԫ         */
    double   dTimerate;        /* ʱ�����   Ԫ/Сʱ    */
} ncBillTime;

typedef struct ncBillFlow_s {
    unsigned long long lUflow;      /* �������� 0--����,�ֽ�   */
    double   dBasefee;              /* ������             */
    double   dFlowrate;             /* ��������  Ԫ/MB    */
} ncBillFlow;

/* ������Ϣ  */
typedef struct ncBillProm_s {
    unsigned long lStart;      /* ��ʼʱ��           */
    unsigned long lEnd;        /* ��ֹʱ��           */
    double   dRate;            /* �Ż���             */
} ncBillProm;


typedef struct ncBillRate_s {
    unsigned long lStart;      /* ��ʼʱ��           */
    unsigned long lEnd;        /* ��ֹʱ��           */
    double   dRate;            /* �Ż���             */
    struct   ncBillRate_s   *next;
} ncBillRate;


typedef struct ncBill_s {
    unsigned long  lId;         /* �ƷѲ���           */
    char           feename[32]; /* ��������          */
    unsigned char  feetype;    /* ���ѷ�ʽ 0-���ú� 1-Ԥ���� */
    unsigned char  feeway;     /* ���㷽ʽ 0-һ���Խ���  1-ÿ��   2 ÿ��  3-ÿ��  4-ÿ��   5-ÿ��  */
    unsigned short timeway;    /* ʱ����㷽ʽ 0-�˳��Ϳ�ʼ֮��  1-ʵ�����ݰ�ʱ��    */
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
    unsigned long   lStarttime;     /* ��ǰ��ʼʱ��          */
    unsigned long   lStoptime;      /* ��ǰ��ֹʱ��          */
    unsigned long   lLoadtime;      /* �ƷѲ���װ��ʱ��      */
    unsigned long   lLasttime;      /* �ƷѲ������ʹ��ʱ��  */
    unsigned long   lDocount;       /* ��������              */
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
