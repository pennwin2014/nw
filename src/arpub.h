#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pasdb.h"
#include "utoplt01.h"

int appEnterpriseLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	
	char userName_in[32] = "";
	char password_in[16] = "";
	
	utMsgGetSomeNVar(psMsgHead,2,
		"username",	UT_TYPE_STRING, sizeof(userName_in)-1, 		userName_in,
		"password",	UT_TYPE_STRING,	sizeof(password_in)-1, 		password_in);
		
  utPltDbHead *psDbHead = utPltInitDb();
	
	if(!strcmp(userName_in,"admin") && !strcmp(password_in,"admin")){
		utPltPutVarF(psDbHead,"buff","{success:true,msg:%s}","µȂ½³ɹ¦");
	}else{
		utPltPutVarF(psDbHead,"buff","{success:false,msg:%s}","ԃ»§Ļ»󄝂뵭ϳ");
	}
	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	
	return 0;
}