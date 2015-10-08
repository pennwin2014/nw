#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "pasdb.h"
/* 处理服务器发过来的请求    */
int ncUtlServerLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caUsername[33],caKey[33],caIp[16];
   	char caPid[64];
	unsigned long lSip,nPort;
   	int iReturn;
#ifdef LDEBUG
      utMsgPrintMsg(psMsgHead);
#endif        
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
					"username",  UT_TYPE_STRING, 31, caUsername,
					"password",  UT_TYPE_STRING, 32, caKey);
    utComGetSockIp(iFd,&lSip,&nPort);
	strcpy(caIp,utComHostIp(lSip));
	iReturn = dsCltUserLogin0(caUsername,caKey,caIp,1);
    if(iReturn != 0) {
        printf(" Auth Error %s \n",caUsername);
        pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* 密钥，暂时不用  */
                    2,
                    "status",    UT_TYPE_LONG,2,
                    "message",   UT_TYPE_STRING,"Auth Error");
        return 0;
    }
    else {
		strcpy(caPid,dsCltGetPasId());
        pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* 密钥，暂时不用  */
                    3,
                    "status",    UT_TYPE_LONG,0,
                    "pasid",     UT_TYPE_STRING,caPid,
                    "sip",       UT_TYPE_STRING,caIp);
    }
    return 0;
}

/* 检查客户端是否已登录  */
int ncUtlServerIsLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	int iReturn;
#ifdef LDEBUG
      utMsgPrintMsg(psMsgHead);
#endif        
    iReturn = dsCltClientIsLogin(psShmHead,psMsgHead);
	pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* 密钥，暂时不用  */
                    1,
                    "status",    UT_TYPE_LONG,iReturn);
    return 0;
}


