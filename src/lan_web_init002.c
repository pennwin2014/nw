#include <stdio.h>
#include "utoall.h"
#include "ncdef.h"

int lan_log_peri(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_log_web(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


int lanWeb_initFun002(utShmHead *psShmHead)
{
    //int iReturn;
		//iReturn = pasSetTcpFunName("lan_log_peri", lan_log_peri, 0);
		pasSetTcpFunName("lan_log_peri", lan_log_peri, 0);
		pasSetTcpFunName("lan_log_web", lan_log_web, 0);

    return 0;
}
