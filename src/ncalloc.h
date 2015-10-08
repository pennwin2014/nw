#ifndef __NETALLOCDEF__
#define __NETALLOCDEF__
#ifdef MTEST_1
#include <stdio.h>
#include <stdlib.h>
#define malloc(x)    nc_malloc(x,__LINE__,__FILE__)
#define calloc(x,y)  nc_calloc(x,y,__LINE__,__FILE__)
#define free(x)      nc_free(x,__LINE__,__FILE__)
#define realloc(x,y) nc_realloc(x,y,__LINE__,__FILE__)
#define utStrDup(x)  nc_utStrDup(x,__LINE__,__FILE__)
#define utStrnDup(x,y)  nc_utStrnDup(x,y,__LINE__,__FILE__)
#define fopen(x,y)   nc_fopen(x,y,__LINE__,__FILE__)
#define fclose(x)    nc_fclose(x,__LINE__,__FILE__)
#define open(x,y,z)  nc_open(x,y,z,__LINE__,__FILE__)
#define close(x)     nc_close(x,__LINE__,__FILE__)

#endif
#endif
