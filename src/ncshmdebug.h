#ifndef __NETSHMDEBUG__
#define __NETSHMDEBUG__
#include <stdio.h>
#include <stdlib.h>
#define utShmHashInit(x1,x2,x3,x4,x5,x6,x7) nc_utShmHashInit(x1,x2,x3,x4,x5,x6,x7,__LINE__,__FILE__)
#define utShmFreeHash(x1,x2)                nc_utShmFreeHash(x1,x2,__LINE__,__FILE__)
#define utShmInitArray(x1,x2,x3,x4)         nc_utShmInitArray(x1,x2,x3,x4,__LINE__,__FILE__)
#define utShmFreeArray(x1,x2)               nc_utShmFreeArray(x1,x2,__LINE__,__FILE__)
#define utShmFreeBinTree(x1,x2)             nc_utShmFreeBinTree(x1,x2,__LINE__,__FILE__)
#define utShmInitBinTree(x1,x2)             nc_utShmInitBinTree(x1,x2,__LINE__,__FILE__)
#endif
