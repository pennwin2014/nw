#include "lan_md5.h"
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stddef.h"
#include <math.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "nwdef.h"
#include "utoall.h"
#define PI                      3.1415926
#define EARTH_RADIUS            6378.137        //µØÇò½üËÆ°ë¾¶


int pennwin_set_data(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead){
	char msg[256]="";
	char num[256] = "";
	int iReturn=0;
	utMsgPrintMsg(psMsgHead);
	iReturn=utMsgGetSomeNVar(psMsgHead,2,
					"num", 		 UT_TYPE_STRING,  sizeof(num)-1, 		num,
					"msg",       UT_TYPE_STRING,  sizeof(msg)-1,      	msg);
	time_t local_time=0;
	time(&local_time);
	char sql[2512] = "";
	utPltDbHead *psDbHead = utPltInitDb();
    snprintf(sql, sizeof(sql) - 1,  "insert into people_distribute(userid, collect_time, collect_number, mesg) values(%u, %u, \'%s\', \'%s\')", 1, local_time, num, msg);
    printf("sql = %s\n",  sql);
	int iret = pasDbExecSqlF(sql);
	if(iret==0){
	  utPltPutVar(psDbHead,"buff","{success:true,value:'success'}");
	}else{
	  utPltPutVar(psDbHead,"buff","{success:true,value:'failure'}");
	}
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	return 0;
}

int pennwin_get_data(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	char sql[1024]="";
	char userid[56] = "";
	int iReturn=0;
	utMsgPrintMsg(psMsgHead);
	iReturn=utMsgGetSomeNVar(psMsgHead,1,   	   
					"userid",        UT_TYPE_STRING,  sizeof(userid)-1,  userid);
	printf("userid=%s\n", userid);
	utPltDbHead *psDbHead = utPltInitDb();
	
	snprintf(sql,sizeof(sql)-1,"select collect_time, collect_number from people_distribute where userid=%s", userid);
	int iNum=0;
	int iret = 0;
	pasDbCursor *psCur = NULL;
	printf("%s\n",sql);
	psCur = pasDbOpenSql(sql, 0);
	if(psCur!=NULL){
		long collect_number = 0;
		long collect_time=0;
		while(0==(iret=pasDbFetchInto(psCur,		
			UT_TYPE_LONG,           4,          &collect_time,
			UT_TYPE_LONG,			4,			&collect_number)) || 1405 == iret){

			iNum++;
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead, "collect_number", iNum, "%d", collect_number);
			utPltPutLoopVarF(psDbHead, "collect_time", iNum, "%d", collect_time);
			
			//Çå¿ÕÈÝÆ÷
			collect_number = 0;
			collect_time = 0;
		}	
	}
	pasDbCloseCursor(psCur);
	utPltPutVarF(psDbHead,"totalCount","%lu",iNum);
	printf("final iNum = %lu\n",iNum);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/pennwin.htm");
	return 0;
}

int pennwin_initFun(utShmHead *psShmHead)
{
    int iReturn;
	iReturn = pasSetTcpFunName("pennwin_get_data", pennwin_get_data, 0);
	iReturn = pasSetTcpFunName("pennwin_set_data", pennwin_set_data, 0);
    return 0;
}


