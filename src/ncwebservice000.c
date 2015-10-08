#include <stdio.h>
#include "utoall.h"
#include "ncdef.h"

int wbs_getDateTime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbs_setDateTime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncInitWebserviceFun(utShmHead *psShmHead)
{
    int iReturn;
    iReturn = pasSetTcpFunName("wbs_getDateTime",wbs_getDateTime, 0);
    iReturn = pasSetTcpFunName("wbs_setDateTime",wbs_setDateTime, 0);
    return 0;
}
