#ifndef UTOBILL_DEF
#define UTOBILL_DEF
typedef struct {
    unsigned long    lLow;      /* ����     */   
    unsigned long    lUp;       /* ����     */
    double           dBase;     /* �������� */
    double           dRate;     /* ����     */
    unsigned         lMode;     /* �Ʒѷ�ʽ 0 -- ��ʱ��   1---���ֽ�*/
} utBilBaseFee;

typedef struct utBilDayDiscount_s {
   unsigned long     lStart;    /* ��ʼ���� */
   unsigned long     lStop;     /* ��ֹ���� */
   double            dDiscount; /* �ۿ�     */
   struct utBilDayDiscount_s  *psNext;
} utBilDayDiscount;

typedef struct utBilWeekDiscount_s {
    unsigned long      lWeekDay;     /* ����  1--7 */
    double             dDisCount;    /* �ۿ�       */
    struct utBilWeekDiscount_s  *psNext;
} utBilWeekDiscount;


typedef struct utBilTimeDiscount_s {
    unsigned long      lStart;       /* ��ʼʱ��  */
    unsigned long      lStop;        /* ��ֹʱ��  */
    double            dDiscount;     /* �ۿ�      */
    struct utBilTimeDiscount_s   *psNext;
} utBilTimeDiscount;


typedef  struct utBilDiscount_s{
    unsigned long    lStart;         /* ��ʼʱ��  */
    unsigned long    lEnd;           /* ��ֹʱ��  */
    double           dDiscount;      /* �ۿ�      */
    struct utBilDiscount_s   *psNext;
} utBilDiscountModule;


typedef  struct utBilFeeTable_s {
    long                              lRoamFlags;
    utBilBaseFee                     *psBilBaseFee;   
    utBilDayDiscount                 *psBilDayDiscount;
    utBilWeekDiscount                *psBilWeekDiscount;
    utBilTimeDiscount                *psBilTimeDiscount;
    utBilDiscountModule              *psBilDiscountModule;
    struct utBilFeeTable_s           *psNext;
}  utBilFeeTable;



typedef struct utBilClass_s {
     unsigned   short      nClass;
     utBilFeeTable         *psBilFeeTable;
     struct utBilClass_s   *psNext;
} utBilClass;

typedef struct utBilAccess_s {
      unsigned short  nAccess;    /* Access Type */
      utBilClass      *psBilClass;
      struct utBilAccess_s   *psNext;
} utBilAccess;


typedef struct utBilNode_s {
      char            caNodeName[16];
      utBilAccess    *psBilAccess;
      struct utBilNode_s   *psNextNode;
}  utBilNode;

typedef struct {
      utBilNode     *psBilNode;
      long	lStart;
      long	lEnd;
} utBilFeeRate;

double  utBilComputeFee(utBilFeeRate *psBilFeeRate,
        	           char *pNodeName, 
                       unsigned short  nClass,
                       unsigned short  nAccess,
                       long lUsedTime, long lStartTime,
                       long lSumTime,  long *lThisTime);
                       
utBilFeeRate  *utBilLoadFeeTable(char *pFeeName);
long   utBilLeftTime(utBilFeeRate *psBilFeeRate,
	           char *pNodeName, 
               unsigned short  nClass,
               unsigned short  nAccess,
               double dLeftFee,long lUsedTime,long lStartTime);
void utBilFreeFeeTable(utBilFeeRate *psBilFeeRate);
int utBilGenFeeModule(utBilFeeRate *psBilFeeRate,long lStart,long lStop);

#endif
