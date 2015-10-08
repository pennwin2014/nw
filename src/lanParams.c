#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pasdb.h"
#include "utoplt01.h"

#define ADD_BASETYPEINFO	"/home/ncmysql/nw/html/v8/outfile/basetypeinfo.txt"
void GetBaseTypeInfo(utShmHead *psShmHead)
{
	FILE * fp;
	fp=fopen(ADD_BASETYPEINFO,"r");
	if(fp){
		char data[512]="";
		char auth[32] = "";
		char shortname[32] = "";
		dsCltGetMyInfo(1,"USERNAME",auth);
	
		findShortName(auth,shortname);
		//frist delete paramtype=30
		char sql[1024]="";
		snprintf(sql,sizeof(sql)-1,"delete from paramsmanage where paramtype=%d",30);
		pasDbExecSql(sql, 0);
		while(NULL!=fgets(data,sizeof(data),fp)){
			if(strlen(data)>0){
				// Paramtype^keyname^keyvalue^detail
				char paramtype[24]="";
				char keyname[32]="";
				char keyvalue[32]="";
				char detail[128]="";
				int i=0;
				for(i=0;i<strlen(data);i++){
					if(data[i]=='^'){
						data[i]=' ';
					}
				}
				sscanf(data,"%24s%32s%32s%128s",paramtype,keyname,keyvalue,detail);
				int paramtypeToint=atoi(paramtype);
				memset(sql,0,sizeof(sql));
				sprintf(sql,"insert into paramsmanage(paramtype,keyname,keyvalue,company,detail)values(%d,'%s','%s','%s','%s')",
					paramtypeToint,keyname,keyvalue,shortname,utf8convert(psShmHead,detail));
				pasDbExecSql(sql, 0);
				recodeLogInfo("data test...","%%%%%%%%%%%%%%% insert data error",sql); 
			}
		}
		remove(ADD_BASETYPEINFO);	
	}
}
int lanparamsType(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	GetBaseTypeInfo(psShmHead);
	char deal_in[8] = "";
	char recno_in[8] = "";
	char keyname_in[32] = "";
	char keyvalue_in[32] = "";
	char paramtype_in[32] = "";
	char detail_in[32] = "";
	
	utMsgGetSomeNVar(psMsgHead,6,
		"deal",UT_TYPE_STRING,sizeof(deal_in)-1,deal_in,
		"recno",UT_TYPE_STRING,sizeof(recno_in)-1,recno_in,
		"keyname",UT_TYPE_STRING,sizeof(keyname_in)-1,keyname_in,
		"keyvalue",UT_TYPE_STRING,sizeof(keyvalue_in)-1,keyvalue_in,
		"paramtype",UT_TYPE_STRING,sizeof(paramtype_in)-1,paramtype_in,
		"detail",UT_TYPE_STRING,sizeof(detail_in)-1,detail_in);
	
	char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
	
	char sql[512] = "";
	
	if(!strcmp(deal_in,"insert")){
		sprintf(sql,"insert into paramsmanage(paramtype,keyname,keyvalue,company,detail)values(%s,'%s','%s','%s','detail')",paramtype_in,utf8convert(psShmHead,keyname_in),utf8convert(psShmHead,keyvalue_in),shortname,utf8convert(psShmHead,detail_in));
	}else if(!strcmp(deal_in,"update")){
		sprintf(sql,"update paramsmanage set keyname='%s',keyvalue='%s',detail='%s' where recno=%s",utf8convert(psShmHead,keyname_in),utf8convert(psShmHead,keyvalue_in),utf8convert(psShmHead,detail_in),recno_in);
	}else if(!strcmp(deal_in,"delete")){
		sprintf(sql,"delete from paramsmanage where recno=%s",recno_in);
	}
	
	printf("参数设置: %s\n",sql);
	pasDbExecSqlF(sql);
	
	sprintf(sql,"select recno,keyname,keyvalue,detail from paramsmanage where paramtype=%s and company='%s' and keyvalue<>''",paramtype_in,shortname);
	
	pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
	printf("参数设置：sql:%s\n",sql);
	psCur = pasDbOpenSql(sql, 0);
	
	if(psCur!=NULL){
		unsigned int recno = 0;
		char keyname[32] = "";
		char keyvalue[32] = "";
		char detail[128] = "";
		
		int iret = 0,iNum = 0;
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(recno), 	&recno,
			 UT_TYPE_STRING, sizeof(keyname)-1,  keyname,
			 UT_TYPE_STRING, sizeof(keyvalue)-1, keyvalue,
			 UT_TYPE_STRING, sizeof(detail)-1, detail)) || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"recno",iNum,"%u",recno);
			utPltPutLoopVar(psDbHead,"keyname",iNum,keyname);
			utPltPutLoopVar(psDbHead,"keyvalue",iNum,keyvalue);
			utPltPutLoopVar(psDbHead,"detail",iNum,detail);
		}
		
		pasDbCloseCursor(psCur);
	}
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_params.htm");
	
	return 0;
}

int lan_dealParams(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  	GetBaseTypeInfo(psShmHead);
	utMsgPrintMsg(psMsgHead);
	char deal_in[18] = "";
	char recno_in[512] = "";
	char keyname_in[32] = "";
	char keyvalue_in[32] = "";
	char paramtype_in[32] = "";
	char detail_in[32] = "";
	
  utMsgGetSomeNVar(psMsgHead,6,
		"deal",UT_TYPE_STRING,sizeof(deal_in)-1,deal_in,
		"recno",UT_TYPE_STRING,sizeof(recno_in)-1,recno_in,
		"paramtype",UT_TYPE_STRING,sizeof(paramtype_in)-1,paramtype_in,
		"keyname",UT_TYPE_STRING,sizeof(keyname_in)-1,keyname_in,
		"keyvalue",UT_TYPE_STRING,sizeof(keyvalue_in)-1,keyvalue_in,
		"detail",UT_TYPE_STRING,sizeof(detail_in)-1,detail_in);
	
  char auth[32] = "";
	char shortname[32] = "";
	int iret;
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
	
	char sql[2512] = "";
	utPltDbHead *psDbHead = utPltInitDb();
	if(strlen(keyname_in)>0||strlen(recno_in)>0){
	if(!strcmp(deal_in,"insert")){
	  sprintf(sql,"delete from paramsmanage where paramtype=%s and keyname='%s' and keyvalue='%s' and company='%s'",paramtype_in,utf8convert(psShmHead,keyname_in),utf8convert(psShmHead,keyvalue_in),shortname);
	  printf("sql=%s\n",sql);
	  pasDbExecSqlF(sql);
		sprintf(sql,"insert into paramsmanage(paramtype,keyname,keyvalue,company,detail,modtime)values(%s,'%s','%s','%s','%s',%lu)",paramtype_in,utf8convert(psShmHead,keyname_in),utf8convert(psShmHead,keyvalue_in),shortname,utf8convert(psShmHead,detail_in),time(0));
	}else if(!strcmp(deal_in,"update")){
		sprintf(sql,"update paramsmanage set keyname='%s',keyvalue='%s',detail='%s',modtime=%lu,flag=0 where recno=%s",utf8convert(psShmHead,keyname_in),utf8convert(psShmHead,keyvalue_in),utf8convert(psShmHead,detail_in),time(0),recno_in);
	}else if(!strcmp(deal_in,"delete")){
		sprintf(sql,"update paramsmanage set flag=1,modtime=%lu where recno=%s",time(0),recno_in);
	}else if(!strcmp(deal_in,"delall")){
		sprintf(sql,"update paramsmanage set flag=1,modtime=%lu where recno in (%s)",time(0),recno_in);
	}
	else if(strcmp(deal_in,"deletetype")==0){
		sprintf(sql,"update paramsmanage set flag=1,modtime=%lu where paramtype=%s and keyname='%s' and company='%s' ",time(0),paramtype_in,utf8convert(psShmHead,keyname_in),shortname);
	}
	printf("sql=%s\n",sql);
	writeLog("procy.txt","logexcl",sql);
	 iret = pasDbExecSqlF(sql);
}
else{
	 iret=-1;
}
	if(iret==0){
	  utPltPutVar(psDbHead,"buff","{success:true,value:'success'}");
	}else{
	  utPltPutVar(psDbHead,"buff","{success:true,value:'failure'}");
	}

	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	return 0;
}

int lan_searchParamValue(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  GetBaseTypeInfo(psShmHead);
	utMsgPrintMsg(psMsgHead);	
	
  char paramtype_in[8] = "";
  char keyname_in[32] ="";
  
  utMsgGetSomeNVar(psMsgHead,2,
		"paramtype",UT_TYPE_STRING,sizeof(paramtype_in)-1,paramtype_in,
		"keyname",UT_TYPE_STRING,sizeof(keyname_in)-1,keyname_in);
	
//	printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
	char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
	
	char sql[512] = "";
	char sql_tmp[128] = "";
	
	if(!utStrIsSpaces(keyname_in)){
	  sprintf(sql_tmp," and keyname='%s'",utf8convert(psShmHead,keyname_in));
	}
	
	utPltDbHead *psDbHead = utPltInitDb();
	
	sprintf(sql,"select recno,keyvalue,detail from paramsmanage where 1=1 %s and paramtype = %s and keyvalue<>'' and company='%s' and flag=0",sql_tmp,paramtype_in,utf8convert(psShmHead,shortname));
	writeLog("params.txt","lan_searchParamValue",sql);
	
	printf("sql=%s\n",sql);
	pasDbCursor *psCur = pasDbOpenSql(sql, 0);
	
	if(psCur!=NULL){
		unsigned int recno = 0;
		char keyvalue[32] = "";
		char detail[128] = "";
		
		int iret = 0,iNum = 0;
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(recno), 	&recno,
			 UT_TYPE_STRING, sizeof(keyvalue)-1, keyvalue,
			 UT_TYPE_STRING, sizeof(detail)-1, detail)) || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"recno",iNum,"%u",recno);
			utPltPutLoopVar(psDbHead,"name",iNum,keyvalue);
			utPltPutLoopVar(psDbHead,"detail",iNum,detail);
		}
		
		pasDbCloseCursor(psCur);
	}
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_senseprocname.htm");
	return 0;
}

int lan_searchParamName(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  GetBaseTypeInfo(psShmHead);
  char paramtype_in[8] = "";
  printf("bbbbbbbbbbbbbbbbbbbbbbbbb\n");
  utMsgPrintMsg(psMsgHead);
  utMsgGetSomeNVar(psMsgHead,1,
		"paramtype",UT_TYPE_STRING,sizeof(paramtype_in)-1,paramtype_in);
	
	char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
	
	char sql[2512] = "";
	
	utPltDbHead *psDbHead = utPltInitDb();
	
	sprintf(sql,"select keyname from paramsmanage where flag=0 and company='%s' and paramtype=%s group by keyname",utf8convert(psShmHead,shortname),paramtype_in);
	printf("sql=%s\n",sql);
	writeLog("params.txt","lan_searchParamName",sql);
	pasDbCursor *psCur = pasDbOpenSql(sql, 0);
	long iNum = 0;
	if(psCur!=NULL){
		char keyname[32] = "";
		
		int iret = 0;
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_STRING, sizeof(keyname)-1, keyname)) || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"id",iNum,"%d",iNum);
			utPltPutLoopVar(psDbHead,"text",iNum,keyname);
		}
		
		pasDbCloseCursor(psCur);
	}
	iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
	  utPltPutLoopVarF(psDbHead,"id",iNum,"%d",iNum);
		utPltPutLoopVar(psDbHead,"text",iNum,ncLang("0170无"));
	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_combobox.htm");
	return 0;
}
