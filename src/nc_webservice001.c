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

/* 查询当前时间*/
int wbs_getDateTime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	 unsigned long lTime;
	 char caTime[20];
	 lTime=time(0);
	 strcpy(caTime,utTimFormat("%Y%m%d%H%M%S",lTime));
    
#ifdef LDEBUG
    printf(" wbs_getDateTime  \n");
    printf("caTime=%s\n",caTime);
 //   utMsgPrintMsg(psMsgHead);
#endif
   
 
    pasTcpResponse(iFd,psMsgHead,
                    NULL,                
                    1,
                    "Time",    UT_TYPE_STRING,caTime);
    return 0;
}

/* 设置系统时间*/
int wbs_setDateTime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	 unsigned long lTime;
	 char caDate[16],caTime1[16];
	 char caTime[20];
	 char caTemp[218];
	 int iReturn;
	 iReturn = utMsgGetSomeNVar(psMsgHead,1,
		            "Time",        UT_TYPE_STRING, 14, caTime);
	 
	 	#ifdef LDEBUG
	    printf(" wbs_setDateTime  \n");
	    printf("caTime=%s\n",caTime);
	 //   utMsgPrintMsg(psMsgHead);
	#endif
	 if(strlen(caTime)>0){
	 lTime=utTimStrToLong("%Y%m%d%H%M%S",caTime);
	 sprintf(caTemp,"date -s %s",utTimFormat("%Y/%m/%d",lTime));
	 system(caTemp);
	 sprintf(caTemp,"date -s %s",utTimFormat("%H:%M:%S",lTime));
	 system(caTemp);
    system("clock -w");
	 
   
    pasTcpResponse(iFd,psMsgHead,
                    NULL,                
                    1,
                    "Result",    UT_TYPE_SHORT,0);
   }
   else{
   	    pasTcpResponse(iFd,psMsgHead,
                    NULL,                
                    1,
                    "Result",    UT_TYPE_SHORT,1);
  }
    return 0;
}