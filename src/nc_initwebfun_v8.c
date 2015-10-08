#include <stdio.h>
#include "utoall.h"
#include "ncdef.h"
#include "ncrtl_v8.h"
int ncInitWebFun_v8()
{
	long iReturn;
	iReturn=pasSetTcpFunName("ncShowNetflowAjax_v8",ncShowNetflowAjax_v8,0);
	iReturn=pasSetTcpFunName("ncWebShowNetflowJs",ncWebShowNetflowJs,0);
	
	return 0;
}