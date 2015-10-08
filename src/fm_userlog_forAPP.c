#include "lan_md5.h"
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "nwdef.h"
#include "utoall.h"
int fm_userlog_forApp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
  utMsgPrintMsg(psMsgHead);
  utPltDbHead *psDbHead = utPltInitDb();
  char username[32]="";
  char password[32]="";
  char clientip[32]="";
  int iReturn;
  char sql[1024]="";
  int lCount=0;
  char sessid[32]="";
  iReturn=utMsgGetSomeNVar(psMsgHead,3,   	   
					"username",  UT_TYPE_STRING, 31, username,
					"password",  UT_TYPE_STRING, 31, password,
					"clientip",  UT_TYPE_STRING, 15, clientip);
	 utStrDelSpaces(username);
	 if(strlen(username)==0||strlen(password)==0){
		 utPltPutVar(psDbHead,"buff","{success:false,sesid:\'\',mesg:\'登录名或密码为空!\'}");
    	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
    	 return 0;
	 }
	 snprintf(sql,sizeof(sql)-1,"select count(*) from fmuserlib where username='%s' and password='%s'",username,password);
	 pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
	 if(lCount!=1){
		 utPltPutVar(psDbHead,"buff","{success:false,sesid:\'\',mesg:\'用户不存在或密码不正确!\'}");
    	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
    	 return 0;
	 }else{
		snprintf(sessid,sizeof(sessid)-1,"u$b9bEx26tn$CFQxqjNV2g");
		utPltPutVar(psDbHead,"buff","{success:true,sesid:\'%s\',mesg:\'验证完成!\'}",sessid);
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");	
	 }

	return 0;
}
