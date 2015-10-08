#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "nwdef.h"
#include "ncdef.h"

int appEnpLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	
	char userName_in[32] = "";
	char password_in[16] = "";
	
	utMsgGetSomeNVar(psMsgHead,2,
		"username",	UT_TYPE_STRING, sizeof(userName_in)-1, 		userName_in,
		"password",	UT_TYPE_STRING,	sizeof(password_in)-1, 		password_in);
		
  utPltDbHead *psDbHead = utPltInitDb();
	
	if(!strcmp(userName_in,"admin") && !strcmp(password_in,"admin")){
		utPltPutVarF(psDbHead,"buff","{success:true,msg:%s}","LoginSuccess");
	}else{
		utPltPutVarF(psDbHead,"buff","{success:false,msg:%s}","UserNameOrPasswordError");
	}
	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	
	return 0;
}

int appEnpZxjsj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	
	printf("I am in\n");
	
	char sql[512] = "";
	
	sprintf(sql,"select compid,compname from nwcompute");
	
  pasDbCursor *psCur = NULL;
  utPltDbHead *psDbHead = utPltInitDb();
  
  int iret = 0;
  int iNum = 0;
  
  psCur = pasDbOpenSql(sql,0);
  
  if(psCur){
  	unsigned int compid = 0;
  	char compname[32] = "";
		nwCompIp *psCompIp = NULL;
  	
  	while(0 == (iret = pasDbFetchInto(psCur,
				UT_TYPE_ULONG,  sizeof(compid),     &compid,
				UT_TYPE_STRING, sizeof(compname)-1, compname)) || 1405 == iret){
			psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
			if(psCompIp){
				iNum++;
				if(iNum > 1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
				utPltPutLoopVarF(psDbHead,"compid",iNum,"%u",compid);
				utPltPutLoopVar(psDbHead,"compname",iNum,compname);
			}
		}
		
		pasDbCloseCursor(psCur);
		utPltShowDb(psDbHead);
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/appEnterprise/app_ssjsj.htm");
  }

	return 0;
}