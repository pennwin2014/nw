#ifndef __NETDEBUGDEF__
#define __NETDEBUGDEF__
#ifdef LDEBUG
#include <stdio.h>
#include <stdlib.h>
#define ncdebug(x,y)    nc_debug(x,__LINE__,__FILE__,y)
#endif
#endif
