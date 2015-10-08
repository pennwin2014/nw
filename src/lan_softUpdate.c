#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pasdb.h"
#include "utoplt01.h"

#define UPDATE_LOG 1
#define UPDATE_DETAIL_LOG 1

unsigned int totalUpdateNum(unsigned int softId){
	unsigned int totalNum = 0;
	
	char sql[512] = "";
	
	snprintf(sql,sizeof(sql),"select count(distinct nwcompute.compid) from nwcompute,ncupsoftprocy where ncupsoftprocy.softid = %u and (nwcompute.compid = ncupsoftprocy.objid and ncupsoftprocy.objtype = 2) or (nwcompute.groupid = ncupsoftprocy.objid and ncupsoftprocy.objtype = 1) or (ncupsoftprocy.objtype = 0)",softId);
	
	//printf("totalNum sql:%s\n",sql);
	pasDbCursor *psCur = NULL;
	
	psCur = pasDbOpenSql(sql,0);
	pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&totalNum);
	pasDbCloseCursor(psCur);
	
	return totalNum;
}

int lan_softUpdateLog_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
#if UPDATE_LOG
	utMsgPrintMsg(psMsgHead);
#endif
	
	writeSysLog(psMsgHead,"06","Success","查询了升级日志");
	char sql[1024] = "";
	
	pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
	
	unsigned int db_count = 0;
	
	snprintf(sql,sizeof(sql),"select count(*) from (select * from ncupsoftlog,ncupsoftlist where ncupsoftlog.softid=ncupsoftlist.sid group by ncupsoftlog.softid)new_table");

#if UPDATE_LOG
	printf("sqlcount: %s\n",sql);
#endif

	psCur = pasDbOpenSql(sql,0);
	pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&db_count);
	utPltPutVarF(psDbHead,"TotRec","%u",db_count);
	pasDbCloseCursor(psCur);
	
//	snprintf(sql,sizeof(sql),"select ncupsoftlog.softid,ncupsoftlist.softname,ncupsoftlist.filename,ncupsoftlist.version,count(ncupsoftlog.compid),count(distinct ncupsoftlog.compid),sum(ncupsoftlog.status) from ncupsoftlog,ncupsoftlist where ncupsoftlog.softid=ncupsoftlist.sid group by ncupsoftlog.softid order by softid desc");
	
	snprintf(sql,sizeof(sql),"select softid,softname,filename,version,count(compid),sum(status) from (select ncupsoftlog.softid,ncupsoftlist.softname,ncupsoftlist.filename,ncupsoftlog.compid,ncupsoftlist.version,min(ncupsoftlog.status)status from ncupsoftlog,ncupsoftlist where ncupsoftlog.softid=ncupsoftlist.sid group by ncupsoftlog.softid,ncupsoftlog.compid)new_table group by softid order by softid desc");

#if UPDATE_LOG
	printf("sql: %s\n",sql);
#endif
	int iret = -1;
	int iRetSum = 0;
	char caResult[40960] = "";
	
	unsigned int softId = 0;
	char softName[32] = "";
	char fileName[128] = "";
	char version[16] = "";
	unsigned int total = 0;
	unsigned int totalNum = 0;
	unsigned int sumStatus = 0;
	
  iret = pasDbRecordSet(sql,0,40,&iRetSum,caResult,6,
  	UT_TYPE_ULONG,	4,								&softId,
		UT_TYPE_STRING, sizeof(softName),	softName,
		UT_TYPE_STRING, sizeof(fileName),	fileName,
		UT_TYPE_STRING, sizeof(version),	version,
		UT_TYPE_ULONG,  4,								&totalNum,
		UT_TYPE_ULONG,  4,								&sumStatus);
		
	if(0 == iret){
		char *p = caResult;
  	int iNum = 0;
  	
		printf("caResult1:%s\n",p);
		while(*p && p) {
			
			p = pasDbRecordValue(p,1,6,
		  	UT_TYPE_ULONG,	4,								&softId,
				UT_TYPE_STRING, sizeof(softName),	softName,
				UT_TYPE_STRING, sizeof(fileName),	fileName,
				UT_TYPE_STRING, sizeof(version),	version,
				UT_TYPE_ULONG,  4,								&totalNum,
				UT_TYPE_ULONG,  4,								&sumStatus);
			
			iNum++;
			printf("caResult%d:%s\n",iNum,p);
			printf("softName%d:%s\n",iNum,softName);
			
			
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			total = totalUpdateNum(softId);
			if(total<totalNum){
				total = totalNum;
			}
			utPltPutLoopVarF(psDbHead,"softId",iNum,"%u",softId);
			utPltPutLoopVar(psDbHead,"softName",iNum,softName);
			utPltPutLoopVar(psDbHead,"fileName",iNum,fileName);
			utPltPutLoopVar(psDbHead,"version",iNum,version);
			utPltPutLoopVarF(psDbHead,"totalNum",iNum,"%u",total);
			utPltPutLoopVarF(psDbHead,"successNum",iNum,"%u",2*totalNum-sumStatus);
			utPltPutLoopVarF(psDbHead,"failedNum",iNum,"%u",sumStatus - totalNum);
			utPltPutLoopVarF(psDbHead,"unUpdateNum",iNum,"%u",total-totalNum);
			
		//	printf("totalNummain:%u，iNum：%u\n",total,iNum);
   	}
//#if UPDATE_LOG
//		utPltShowDb(psDbHead);
//#endif

		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_soft_updateLog.htm");
	}
	
	return 0;
}

int lan_softUpdateDetailLog_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
#if UPDATE_DETAIL_LOG
	utMsgPrintMsg(psMsgHead);
#endif
	char softId_in[16] = "";
	char sort_in[16] = "";
	char dir_in[8] = "";
	char start_in[16] = "";
	char limit_in[16] = "";
	char status_in[8] = "";
	char groupId_in[32] = "";

	 utMsgGetSomeNVar(psMsgHead,7,
		"softId",		UT_TYPE_STRING,  sizeof(softId_in)-1, 		softId_in,
		"dir",			UT_TYPE_STRING,  sizeof(dir_in)-1, 				dir_in,
		"sort",			UT_TYPE_STRING,  sizeof(sort_in)-1, 			sort_in,
		"status",		UT_TYPE_STRING,  sizeof(status_in)-1, 		status_in,
		"groupId",		UT_TYPE_STRING,  sizeof(groupId_in)-1, 	groupId_in,
		"start",		UT_TYPE_STRING,  sizeof(start_in)-1, 			start_in,
		"limit",		UT_TYPE_STRING,  sizeof(limit_in)-1, 			limit_in);
	
	char sql[1024] = "";
	pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
	char sql_tmp[1024] = "";
	
	char msg[256] = "";
	snprintf(msg,sizeof(msg),"查询了升级包ID[%s]的详细升级信息",softId_in);
	writeSysLog(psMsgHead,"06","Success",msg);
	
	if(!utStrIsSpaces(status_in)){
		snprintf(sql_tmp,sizeof(sql_tmp)," and status = '%s'",status_in);
	}
	
	if(!utStrIsSpaces(groupId_in)){
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp)," and groupid = '%s'",groupId_in);		
	}
	
	snprintf(sql,sizeof(sql),"select count(*) from (select nwcompute.compid,nwcompute.groupid,nwcompute.compname,ncupsoftlog.stime,ncupsoftlog.status from ncupsoftlog,nwcompute where ncupsoftlog.compid = nwcompute.compid and ncupsoftlog.softid = %s union all select distinct nwcompute.compid,nwcompute.groupid,nwcompute.compName,0,3 from nwcompute,ncupsoftprocy,ncupsoftlog where ncupsoftprocy.softid = %s and ((nwcompute.compid = ncupsoftprocy.objid and ncupsoftprocy.objtype = 2) or (nwcompute.groupid = ncupsoftprocy.objid and ncupsoftprocy.objtype = 1) or (ncupsoftprocy.objtype = 0)) and (nwcompute.compid not in (select ncupsoftlog.compid from ncupsoftlog where ncupsoftlog.softid = %s)))newtable where 1=1 %s",softId_in,softId_in,softId_in,sql_tmp);

#if UPDATE_DETAIL_LOG
	printf("sql: %s\n",sql);
#endif
	
	psCur = pasDbOpenSql(sql,0);
	unsigned int db_count = 0;
	pasDbFetchInto(psCur,UT_TYPE_ULONG,  4,&db_count);
	pasDbCloseCursor(psCur);

	utPltPutVarF(psDbHead,"TotRec","%u",db_count);
	
	snprintf(sql,sizeof(sql),"select groupid,compname,stime,status from (select nwcompute.compid,nwcompute.groupid,nwcompute.compname,ncupsoftlog.stime,ncupsoftlog.status from ncupsoftlog,nwcompute where ncupsoftlog.compid = nwcompute.compid and ncupsoftlog.softid = %s union all select distinct nwcompute.compid,nwcompute.groupid,nwcompute.compName,0,3 from nwcompute,ncupsoftprocy,ncupsoftlog where ncupsoftprocy.softid = %s and ((nwcompute.compid = ncupsoftprocy.objid and ncupsoftprocy.objtype = 2) or (nwcompute.groupid = ncupsoftprocy.objid and ncupsoftprocy.objtype = 1) or (ncupsoftprocy.objtype = 0)) and (nwcompute.compid not in (select ncupsoftlog.compid from ncupsoftlog where ncupsoftlog.softid = %s)))newtable where 1=1 %s",softId_in,softId_in,softId_in,sql_tmp);
	
	if(!utStrIsSpaces(sort_in)){
		if(!strcmp(sort_in,"groupName")){
			strcpy(sort_in,"groupid");
		}
		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," order by %s %s",sort_in,dir_in);
	}else{
		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," order by stime desc");
	}
	
	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," limit %s,%s",start_in,limit_in);
#if UPDATE_DETAIL_LOG
	printf("sql: %s\n",sql);
#endif

	psCur = pasDbOpenSql(sql,0);
	
	if(NULL != psCur){
		int iret = -1;
		int iNum = 0;
		
		unsigned int groupId = 0;
		char compName[32] = "";
		unsigned int stime = 0;
		unsigned int status = 0;
		char status2[8] = "";
		
		strcpy(status2,ncLang("0130失败"));
		
		while(0==(iret=pasDbFetchInto(psCur,
			UT_TYPE_ULONG,  4,								&groupId,
			UT_TYPE_STRING, sizeof(compName),	compName,
			UT_TYPE_ULONG,  4,								&stime,
			UT_TYPE_ULONG,  4,								&status			
			)) || 1405 == iret){
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			
			if(status == 1){
				strcpy(status2,ncLang("0131成功"));
			}else if(status == 2){
				strcpy(status2,ncLang("0130失败"));
			}else if(status == 3){
				strcpy(status2,ncLang("0132未升级"));
			}
			if(groupId != 0){			
				utPltPutLoopVar(psDbHead,"groupName",iNum,lanGetGroup(psShmHead,groupId));
			}else{
				utPltPutLoopVar(psDbHead,"groupName",iNum,"未定义");
			}
			utPltPutLoopVar(psDbHead,"compName",iNum,compName);
			if(0 != stime){
				utPltPutLoopVar(psDbHead,"stime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S", stime));
			}else{
				utPltPutLoopVar(psDbHead,"stime",iNum,"...");
			}
			utPltPutLoopVar(psDbHead,"status",iNum,status2);
		}
		pasDbCloseCursor(psCur);
		
#if UPDATE_DETAIL_LOG
		utPltShowDb(psDbHead);
#endif

		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_soft_updateDetailLog.htm");
	}
	
	return 0;
}

int lan_webfileUp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){

	utMsgPrintMsg(psMsgHead);
	
	char caFile[256] = "";
 	char caLocal[128] = "";
 	char caRemote[128] = "";
 	char caType[128] = "";
 	char status[2] = "";
 	char buff[256] = "";
 	char blackname[32] = "";
 	
  utPltDbHead *psDbHead = utPltInitDbHead();

	utMsgGetSomeNVar(psMsgHead,3,
			"filename", UT_TYPE_STRING, sizeof(caFile)-1,caFile,
			"status", UT_TYPE_STRING, sizeof(status)-1, status,
			"blackname", UT_TYPE_STRING, sizeof(blackname)-1, blackname);
			
	pmUtlUploadFile(caFile,caLocal,caRemote,caType);
	
	printf(" caFile==[%s],caLocal=%s,caRemote=%s\n",caFile,caLocal,caRemote);
	if(!strstr(caLocal,".csv")&&!strstr(caLocal,".xls")){
		printf("0.5\n");
		utPltPutVar(psDbHead,"buff","{success:false,msg:\"上传的文件csv或者xls文件\"}");
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
		return -1;
	}
	printf("%s1\n",blackname);
	int num=0;
	
	FILE *fp = fopen(caRemote,"r");
	if(NULL == fp){
		return -1;
	}
	char sql[1024] = "";
	
	char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
	
	
	printf("%s2\n",blackname);
	
	while(fgets(buff,255,fp)!= NULL ){
		memset(sql,0,sizeof(sql));
		num++;
		
		printf("3\n");
		if(num==1){
			if(!strstr(buff,"名单")){
				printf("3.5\n");
				fclose(fp);
				utPltPutVar(psDbHead,"buff","{success:false,msg:\"上传的文件不是黑白名单文件\"}");
				utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
				return -1;
			}
		}else if(num==2){
			if(NULL == strstr(buff,"网址")){
				
				printf("%s4.5\n",buff);
				fclose(fp);
				utPltPutVar(psDbHead,"buff","{success:false,msg:\"上传的文件不是网址黑白名单文件\"}");
				utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
				return -1;
			}
		}else{
			trim(buff);
			clearQuotation(buff);
			int count = 0;
			
			//判断黑白名单是否有重复
    	sprintf(sql,"select count(*) from blacklist where blacktype='web' and blackname='%s' and blackvalue='%s' and flag=1",utf8convert(psShmHead,blackname),buff);
    	pasDbOneRecord(sql,0,UT_TYPE_LONG,sizeof(count),&count);
    	
    	if(count>0){
    	  sprintf(sql,"delete from blacklist where blacktype='web' and blackname='%s' and blackvalue='%s' and flag=1",utf8convert(psShmHead,blackname),buff);
    	  pasDbExecSqlF(sql);
    	}
			
			sprintf(sql,"insert blacklist(blacktype,blackname,blackvalue,blackstatus,updatetime,userid)values('web','%s','%s',%s,%lu,'%s')",utf8convert(psShmHead,blackname),buff,status,time(0),shortname);
			
			pasDbExecSqlF(sql);
		}
		
		memset(buff,0,sizeof(buff));
	}
	
	fclose(fp);
	
	printf("6\n");
	utPltPutVar(psDbHead,"buff","{success:true}");
	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");

	return 0;
}
