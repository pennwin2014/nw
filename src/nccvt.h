#ifndef __NCCVTDEF___
#define __NCCVTDEF___
#define NC_CVT_MAXFIELD  64
typedef struct ncCvtStrList_s {
    char caOld[32];
    char caNew[32];
} ncCvtStrList;

typedef struct ncCvtField_s {
    char   sname[64];
    char   dname[64];
    char   stype[8];
    char   dtype[8];
    short  nFlags;             /* 0 Õý³£  1 ²Î¿¼×Ö¶Î  */
    short  iSumValue;
    ncCvtStrList *pValue;
} ncCvtField;


typedef struct ncCvtVarValue_s {
    char    name[32];
    char    value[64];
} ncCvtVarValue;

#endif
