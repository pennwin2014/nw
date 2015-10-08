#ifndef __UTOTYPEDEF__
#define __UTOTYPEDEF__

typedef short               int2;
typedef long                int4;
typedef long  long          int8; 
typedef long  long          long8;
typedef unsigned short      uint2;
typedef unsigned long       uint4;
/* typedef unsigned long long  uint8;  */
typedef unsigned char       uchar;
#ifndef __cplusplus	/* -- added by client */
#define UT_TYPE_CHAR        1
#define UT_TYPE_SHORT       2
#define UT_TYPE_LONG        3
#define UT_TYPE_FLOAT       4
#define UT_TYPE_DOUBLE      5
#define UT_TYPE_STRING      6
#define UT_TYPE_STRUCT      7
#define UT_TYPE_LONG8       8
#define UT_TYPE_ULONG       9
#endif /* !__cplusplus */
#endif

