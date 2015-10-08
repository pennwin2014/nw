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


int8 pasGetLongTimeId();
#include <sys/stat.h>
#include <sys/types.h>

#define DEBUG_OUTFILE_KEY
#define OUTFILE_KEY_LAW_PATH	"/home/ncmysql/nw/OutFileKeyLaw/"
typedef struct{
	unsigned long long int sid;
	unsigned long long int tasksid;
	unsigned long int taskid;
	unsigned long int Computerid;
	unsigned long int Sip;
	char Cpname[64];
	unsigned long int Gpid;
	unsigned long int Statues;
	unsigned long int stime;
	char userer[64];
	int runlevel;
}LOG_ITEM;
typedef struct{
	unsigned long int userid;
	char mac[32];
	unsigned long int groupid;
	char devname[32];
	char udisp[32];
	unsigned long int sip;
	char procname[72];
	unsigned long int mark;
	char indexid[32];
	unsigned long int stime;
	char souPath[256];
	char destPath[256];
	char content[256];
	int Blacktype;
	int Backuptype;
	unsigned long int filesize;
}OUTFILEKEYDATA;
typedef struct _buf_array{
	struct _buf_array* pnext;
	union{
		char	buffer[1024];
		LOG_ITEM	item;
		OUTFILEKEYDATA outfilekey;
	}info;
}BUF_ARRAY;


BUF_ARRAY* AddBufItem(BUF_ARRAY* pbufroot,BUF_ARRAY* pbufitem)
{
	BUF_ARRAY* pitem=(BUF_ARRAY*) malloc(sizeof(BUF_ARRAY));
	memset(pitem,0,sizeof(BUF_ARRAY));
	memcpy(pitem,pbufitem,sizeof(BUF_ARRAY));
	if(pbufroot==NULL)return pitem;
	pitem->pnext=pbufroot;
	return pitem;
}
void ClearBufItem(BUF_ARRAY* pbufroot)
{
	BUF_ARRAY* pitem=NULL;
	while(pbufroot!=NULL){
		pitem=pbufroot->pnext;
		free(pbufroot);
		pbufroot=pitem;
	}
}
void DebugLog(char *funcName,char *title,char *data)
{
#ifdef DEBUG_OUTFILE_KEY
	char mesg[1024]="";
	snprintf(mesg,sizeof(mesg),"%s %s %s",funcName,title,data);
       char path[128]="";
	snprintf(path,sizeof(path),"%sdebug.txt",OUTFILE_KEY_LAW_PATH);
	if(strlen(mesg)>0){
		FILE *fp=fopen(path,"a+");
		if(fp){
			fseek(fp,0,SEEK_END);
			int count=ftell(fp);
			if(count>5*1024*1024){
				fclose(fp);
				fp=fopen(path,"w");
			}
			char temp[32]="";
			time_t mytime;
			time(&mytime);
			struct tm *mylocalTime;
			mylocalTime=localtime(&mytime);
			strftime(temp,sizeof(temp),"%Y-%m-%d %H:%M:%S",mylocalTime);
			fprintf(fp,"%s %s\r\n",temp,mesg);
			fclose(fp);
		}
	}
#endif
}
//********************************************************************************************************************
//********************************************************************************************************************
//***************************外发文件任务下发响应函数*****************************************************************
void AnalyzerContent(char *sid,BUF_ARRAY *pitem){
	unsigned long long int tasksid=0;
	unsigned long int tasksid_celue=0;
	int runlevel=0;
	tasksid=strtoull(sid,NULL,10);
	if(tasksid>0){
		char sql[1024]="";
		snprintf(sql,sizeof(sql)-1,"select taskid,runlevel from nwoutfiletask where sid=%llu",tasksid);
		pasDbOneRecord(sql,0,UT_TYPE_ULONG,sizeof(tasksid_celue),&tasksid_celue,
					UT_TYPE_ULONG,sizeof(runlevel),&runlevel);
		if(tasksid_celue>0){
			memset(sql,0,sizeof(sql));
			snprintf(sql,sizeof(sql)-1,"select count(*) from nwoutfilelaw where Computerid=%u and taskid=%u",pitem->info.item.Computerid,tasksid_celue);
			int num=0;
			pasDbOneRecord(sql,0,UT_TYPE_ULONG,sizeof(num),&num);
			if(num<=0){
				unsigned long long int currentTime=pasGetLongTimeId()+tasksid_celue;
				memset(sql,0,sizeof(sql));
				snprintf(sql,sizeof(sql)-1,
					"insert into nwoutfilelaw(sid,taskid,Computerid,Sip,Cpname,Gpid,Status,stime,userer,runlevel) values(%llu,%u,%u,%u,\'%s\',%u,%u,%u,\'%s\',%d)",
					currentTime,tasksid_celue,pitem->info.item.Computerid,pitem->info.item.Sip,
					pitem->info.item.Cpname,pitem->info.item.Gpid,pitem->info.item.Statues,pitem->info.item.stime,pitem->info.item.userer,runlevel);
				pasDbExecSql(sql, 0);
				//DebugLog("AnalyzerContent called....","",sql);
			}
		}
	}
}
int GetMaxTaskid(long taskid){
	char mesg[1024]="";
	long maxtaskid=-1;
	if(!utFileIsExist(OUTFILE_KEY_LAW_PATH)){
		char comd[512]="";
		snprintf(comd,sizeof(comd)-1,"mkdir -p %s",OUTFILE_KEY_LAW_PATH);
		system(comd);
	}
	
	char path[128]="";
	snprintf(path,sizeof(path),"%smaxtaskid.txt",OUTFILE_KEY_LAW_PATH);	
	FILE *fp=fopen(path,"r");
	if(fp){
		char data[64]="";
		fgets(data,sizeof(data),fp);
		fclose(fp);
		if(strlen(data)>0){
			char *pos=strchr(data,'=');
			if(pos){	
				char *pos2=strchr(pos+1,';');
				if(pos2){
					char temp[24]="";
					memcpy(temp,pos+1,pos2-(pos+1));
					long maxtaskid=atol(temp);
					if(maxtaskid&&maxtaskid>0){
						if(taskid>=maxtaskid){
							maxtaskid=taskid;	
						}
						maxtaskid++;
						if(maxtaskid>=5000*10000){
							maxtaskid=1;
							char sql[512]="";
							snprintf(sql,sizeof(sql),"%s","delete from nwoutfiletask");
							if(pasDbExecSql(sql, 0)>0){
								DebugLog("sql err....","GetMaxTaskid",sql);
							}
						}
						fp=fopen(path,"w");
						if(fp){
							fprintf(fp,"maxtaskid=%ld;",maxtaskid);
							fclose(fp);
						}
						return maxtaskid;
					}
				}
			}
		}
	}
	taskid++;
	fp=fopen(path,"w");
	if(fp){
		fprintf(fp,"maxtaskid=%ld;",taskid);
		fclose(fp);
	}
	return taskid;
}
int lan_log_outfile_task(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();

	char type[5]="";
	char gid[128]="";
	char keyword[128]="";
	char taskname[128]="";
	char time_flag[5]="";
	char importantword[128]="";
	char groupname[128]="";
	char PCname[128]="";
	char doneUser[128]="";
	char sdate[32]="";
	char edate[32]="";
	char  compid[64]="";
	char delid[1024*5]="";
	char  listCom[1024*5]="";
	pasDbCursor *psCur=NULL;
	char limit[32]="";
	char start[32]="";
	int llcount=0;
	int runlevel=0;
	char _runlevel[4]="";
	int iNum=0;
	int iret=0;
	utMsgGetSomeNVar(psMsgHead, 17,
					"type",						UT_TYPE_STRING,  			sizeof(type)-1,  					type,
					"compid",						UT_TYPE_STRING,  			sizeof(compid)-1,  					compid,
   				     "gid",						UT_TYPE_STRING,  			sizeof(gid)-1,  					gid,
					 "taskname",				UT_TYPE_STRING,				sizeof(taskname)-1,					taskname,
					 "time_flag",				UT_TYPE_STRING,				sizeof(time_flag)-1,				time_flag,	
					 "importantword",			UT_TYPE_STRING,				sizeof(importantword)-1,			importantword,
					 "groupname",				UT_TYPE_STRING,				sizeof(groupname)-1,				groupname,
					 "PCname",					UT_TYPE_STRING,				sizeof(PCname)-1,					PCname,
					 "doneUser",				UT_TYPE_STRING,				sizeof(doneUser)-1,					doneUser,
					 "sdate",					UT_TYPE_STRING,				sizeof(sdate)-1,					sdate,		
					 "edate",					UT_TYPE_STRING,				sizeof(edate)-1,					edate,
   				     "keyword",          		UT_TYPE_STRING,  			sizeof(keyword)-1,					keyword,
   				     "start",          		   UT_TYPE_STRING,  			sizeof(start)-1,					start,
   				     "limit",          		UT_TYPE_STRING,  			sizeof(limit)-1,					limit,
					"delid",          		UT_TYPE_STRING,  			sizeof(delid)-1,						delid,
					"listCom",          		UT_TYPE_STRING,  			sizeof(listCom)-1,						listCom,
					"runlevel",          		UT_TYPE_STRING,  			sizeof(_runlevel)-1,					_runlevel);

  	     
	char mesg[512];
	memset(mesg,0,sizeof(mesg));

	
	snprintf(mesg,512,"type:%s\t,compid:%s\t,gid:%s\t,keyword:%s\t,taskname:%s\t,time_flag:%s\t,importantword:%s\t,groupname:%s\t,PCname:%s\t,doneUser:%s\t,sdate:%s\t,edate:%s\t,delid:%s\t,listCom:%s\trunlevel=%d\r\n",
		type,compid,gid,utf8convert(psShmHead,keyword),utf8convert(psShmHead,taskname),time_flag,utf8convert(psShmHead,importantword),groupname,PCname,doneUser,sdate,edate,delid,listCom,atoi(_runlevel));
	recodeLogInfo("web task test data...","$$$$$$$$$$$$$$$$$$$$",mesg);


	time_t temp;
	time(&temp);
	unsigned long long currentTime = pasGetLongTimeId();
	int time_type=atoi(time_flag);
	time_t beg_time;
	time_t end_time;
	struct tm *local_time = NULL;
	struct tm when;
	local_time = localtime(&temp);
	end_time =temp;
	int i=0;		//public i for while
	switch(time_type){
		case 0:
			{
				//get begin time
				char year[8];
				memset(year,0,sizeof(year));
				char month[4];
				memset(month,0,sizeof(month));
				char day[4];
				memset(day,0,sizeof(day));
				for(i=0;i<strlen(sdate);i++){
					if(sdate[i]=='/')
						sdate[i]=' ';
				}
				sscanf(sdate,"%8s%4s%4s",year,month,day);
				when.tm_year=atoi(year)-1900;
				when.tm_mday=atoi(day);
				when.tm_mon=atoi(month);
				when.tm_hour=0;
				when.tm_min=0;
				when.tm_sec=0;
				beg_time=mktime(&when);
				memset(year,0,sizeof(year));
				memset(month,0,sizeof(month));
				memset(day,0,sizeof(day));
				for(i=0;i<strlen(edate);i++){
					if(edate[i]=='/')
						edate[i]=' ';
				}
				sscanf(edate,"%8s%4s%4s",year,month,day);
				when.tm_year=atoi(year)-1900;
				when.tm_mday=atoi(day);
				when.tm_mon=atoi(month);
				when.tm_hour=23;
				when.tm_min=59;
				when.tm_sec=59;
				end_time=mktime(&when);
			}
			break;
		case 1:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec;
			break;
		case 2:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec-3*24*3600;
			break;
		case 3:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec-7*24*3600;
			break;
		case 4:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec-30*24*3600;
			break;
		case 5:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec-90*24*3600;
			break;
		case 6:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec-180*24*3600;
			break;
	}

	char sql[1024]="";
	int donetype=atoi(type);
	if(donetype==1){
		//add data
		snprintf(sql, sizeof(sql)-1, "select count(*) from nwoutfiletask  where taskname=\'%s\'and taskkey=\'%s\'",taskname,importantword);
		long lCount=0;
		pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
		if(lCount<=0){
			//get taskid
			snprintf(sql, sizeof(sql)-1,"%s","select max(taskid) from nwoutfiletask");
			pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
			lCount=GetMaxTaskid(lCount);
			if(lCount==-1)
				return -1;
			//get time
			char time_beg_str[128]="";
			char time_end_str[128]="";
			local_time=localtime(&beg_time);
			strftime(time_beg_str,sizeof(time_beg_str),"%Y-%m-%d %H:%M:%S",local_time);
			local_time=localtime(&end_time);
			strftime(time_end_str,sizeof(time_end_str),"%Y-%m-%d %H:%M:%S",local_time);
			currentTime++;
			snprintf(sql, sizeof(sql)-1,
				"insert into nwoutfiletask(sid, taskname, taskid, taskkey, Begtime, Endtime, Gpid ,Computerid, stime,userer,runlevel)  values(%llu, \'%s\', %u, \'%s\',\'%s\',\'%s\', %u,%u,%u,\'%s\',%d)",
				currentTime,utf8convert(psShmHead,taskname),lCount, utf8convert(psShmHead,importantword),time_beg_str,time_end_str,0,0,temp,"",atoi(_runlevel));

			int retValue=pasDbExecSql(sql, 0);
			if(retValue>0){
				memset(mesg,0,sizeof(mesg));
				snprintf(mesg,sizeof(mesg)-1,"$$$$$$$$$$$$$$$$$$$$ insert data error:%d",retValue);
				recodeLogInfo("web task test data...",mesg,sql);       					
			}
		}
	}
	if(donetype==4){
		//doneload task
		int k=0,m=0;
		char data_part1[64]="";
		char data_part2[64]="";
		bool  add_flag=false;
		unsigned long int data_id=0;
		unsigned long int comid=0;
		unsigned long int groupid=0;
		char devname[64]="";
		unsigned long int ip=0;
		char add_data_celue[64]="";

		BUF_ARRAY* prootbuf=NULL;
		BUF_ARRAY mydata;

		for(i=0;i<strlen(listCom);i++){
			if(listCom[i]==','){
				k=0;
				for(m=0;m<strlen(data_part1);m++){
					if(data_part1[m]=='_'){
						add_flag=true;
						k=0;
						continue;
					}
					if(add_flag){
						data_part2[k]=data_part1[m];
						k++;
					}
				}
				add_flag==false;
				k=0;
				data_id=strtoul(data_part2,NULL,10);
				memset(sql,0,sizeof(sql));
				if(data_id==1){
					snprintf(sql,sizeof(sql)-1,"%s","select compid,groupid,devname,ip from nwcompute");
				}else{
					snprintf(sql,sizeof(sql)-1,"select compid,groupid,devname,ip from nwcompute where compid=%u or groupid=%u",data_id,data_id);	
				}
				
				psCur=pasDbOpenSql(sql,0);
				iNum=0;
				iret=0;
				if(psCur!=NULL)
				{
					while(0==(iret=pasDbFetchInto(psCur,
							UT_TYPE_ULONG,			sizeof(comid),					&comid,
							UT_TYPE_ULONG,			sizeof(groupid),				&groupid,
							UT_TYPE_STRING,			sizeof(devname)-1,				devname,
							UT_TYPE_ULONG,			sizeof(ip),						&ip))||1405==iret){
						
							memset(&mydata,0,sizeof(mydata));
							mydata.info.item.sid=0;
							mydata.info.item.taskid=0;
							mydata.info.item.Computerid=comid;
							mydata.info.item.Sip=ip;
							snprintf(mydata.info.item.Cpname,sizeof(devname),"%s",devname);
							mydata.info.item.Gpid=groupid;
							mydata.info.item.Statues=0;
							mydata.info.item.stime=temp;
							//char operateUser[128]=getDsGroupid();
							//recodeLogInfo("web task test data...","**********346***********",operateUser);
							snprintf(mydata.info.item.userer,sizeof(mydata.info.item.userer),"%s","123");
							prootbuf=AddBufItem(prootbuf,&mydata);
					}
					pasDbCloseCursor(psCur);
				}
				memset(data_part2,0,sizeof(data_part2));
				memset(data_part1,0,sizeof(data_part1));
				continue;
			}
			data_part1[k]=listCom[i];
			k++;
			if(i==strlen(listCom)-1){
				for(m=0;m<strlen(data_part1);m++){
					if(data_part1[m]=='_'){
						add_flag=true;
						k=0;
						continue;
					}
					if(add_flag){
						data_part2[k]=data_part1[m];
						k++;
					}
				}
				add_flag==false;
				k=0;
				data_id=strtoul(data_part2,NULL,10);
				memset(sql,0,sizeof(sql));
				if(data_id==1){
					snprintf(sql,sizeof(sql)-1,"%s","select compid,groupid,devname,ip from nwcompute");
				}else{
					snprintf(sql,sizeof(sql)-1,"select compid,groupid,devname,ip from nwcompute where compid=%u or groupid=%u",data_id,data_id);	
				}
				psCur=pasDbOpenSql(sql,0);
				iNum=0;
				iret=0;
				if(psCur!=NULL){
					while(0==(iret=pasDbFetchInto(psCur,
							UT_TYPE_ULONG,			sizeof(comid),					&comid,
							UT_TYPE_ULONG,			sizeof(groupid),				&groupid,
							UT_TYPE_STRING,			sizeof(devname)-1,				devname,
							UT_TYPE_ULONG,			sizeof(ip),						&ip))||1405==iret){
						
							memset(&mydata,0,sizeof(mydata));
							if(comid>0){
								mydata.info.item.sid=0;
								mydata.info.item.taskid=0;
								mydata.info.item.Computerid=comid;
								mydata.info.item.Sip=ip;
								snprintf(mydata.info.item.Cpname,sizeof(devname),"%s",devname);
								mydata.info.item.Gpid=groupid;
								mydata.info.item.Statues=0;
								mydata.info.item.stime=temp;
								snprintf(mydata.info.item.userer,sizeof(mydata.info.item.userer),"%s","123");
								prootbuf=AddBufItem(prootbuf,&mydata);
							}
						}
						pasDbCloseCursor(psCur);
					}

					memset(data_part2,0,sizeof(data_part2));
					memset(data_part1,0,sizeof(data_part1));
			}
		}
	
		BUF_ARRAY* pitem=prootbuf;
		while(pitem!=NULL){
			char data_sid[64]="";
			if(pitem->info.item.Computerid<=0)
				continue;
			if(strlen(delid)>0){
				char* pos1=delid;
				char* pos2=strchr(pos1,',');
				while(pos2!=NULL){
					int len=pos2-pos1;
					if(len<sizeof(data_sid)){
						memset(data_sid,0,sizeof(data_sid));
						memcpy(data_sid,pos1,len);
						AnalyzerContent(data_sid,pitem);
					}
					pos1=pos2+1;
					pos2=strchr(pos1,',');
				}
				if(strlen(pos1)>2){
					AnalyzerContent(pos1,pitem);
				}
			}
			pitem=pitem->pnext;
		}
		ClearBufItem(prootbuf);
		prootbuf=NULL;
	}
	if(donetype==2){
		//find data
		
	}
	if(donetype==3){
		//remove data
		if(strlen(delid)>0){
			int j=0;
			char removeId[64]="";
			unsigned long long int reSid=0;
			unsigned long int newid=0;
			for(i=0;i<strlen(delid);i++){
				if(delid[i]==','){
					j=0;
					reSid=strtoull(removeId,NULL,10);
					if(reSid>0){

						memset(sql,0,sizeof(sql));
						snprintf(sql,sizeof(sql)-1,"select taskid from nwoutfiletask where sid=%llu",reSid);
						pasDbOneRecord(sql,0,UT_TYPE_ULONG,sizeof(newid),&newid);
						if(newid>0){
							memset(sql,0,sizeof(sql));
							snprintf(sql,sizeof(sql)-1,"delete from nwoutfilelaw where taskid=%u",newid);
							if(pasDbExecSql(sql, 0)){
								recodeLogInfo("web task test data...","$$$$$$$$$$$$$$$$$$$$ insert data error",sql);       					
							}else{
								memset(sql,0,sizeof(sql));
								snprintf(sql,sizeof(sql)-1,"delete from nwoutfiletask where sid=%llu",reSid);
								if(pasDbExecSql(sql, 0)){
									recodeLogInfo("web task test data...","$$$$$$$$$$$$$$$$$$$$ insert data error",sql);       					
								}
							}
						}
					}
					memset(removeId,0,sizeof(removeId));
					continue;
				}
				removeId[j]=delid[i];
				j++;
				if(i==strlen(delid)-1){
					reSid=strtoull(removeId,NULL,10);
					if(reSid>0){
						memset(sql,0,sizeof(sql));
						snprintf(sql,sizeof(sql)-1,"select taskid from nwoutfiletask where sid=%llu",reSid);
						pasDbOneRecord(sql,0,UT_TYPE_ULONG,sizeof(newid),&newid);
						if(newid>0){
							memset(sql,0,sizeof(sql));
							snprintf(sql,sizeof(sql)-1,"delete from nwoutfilelaw where taskid=%u",newid);
							if(pasDbExecSql(sql, 0)){
								recodeLogInfo("web task test data...","$$$$$$$$$$$$$$$$$$$$ insert data error",sql);       					
							}else{
								memset(sql,0,sizeof(sql));
								snprintf(sql,sizeof(sql)-1,"delete from nwoutfiletask where sid=%llu",reSid);
								if(pasDbExecSql(sql, 0)){
									recodeLogInfo("web task test data...","$$$$$$$$$$$$$$$$$$$$ insert data error",sql);       					
								}
							}
						}
					}
				}
			}
		}
	}
	
	memset(sql,0,sizeof(sql));
	if(strlen(keyword)>0){
		char select_word[128]="";
		snprintf(select_word,sizeof(select_word)-1,"%s",utf8convert(psShmHead,keyword));
		snprintf(sql,sizeof(sql)-1,"select sid,taskname,taskid,taskkey,Begtime,Endtime,Gpid,Computerid,stime,userer from nwoutfiletask where taskkey like '%%%s%%' or taskname like '%%%s%%' or userer like '%%%s%%'",select_word,select_word,select_word);
	}else{
		snprintf(sql,sizeof(sql)-1,"%s","select sid,taskname,taskid,taskkey,Begtime,Endtime,Gpid,Computerid,stime,userer from nwoutfiletask");
	}
	recodeLogInfo("web task test data...","--------------516--------------",sql);
	char tempsql[1024]="";
	snprintf(tempsql,sizeof(tempsql)-1,sql);
	replaceStr(tempsql,"sid,taskname,taskid,taskkey,Begtime,Endtime,Gpid,Computerid,stime,userer","count(*)");
	recodeLogInfo("web task test data...","--------------520--------------",tempsql);
	pasDbOneRecord(tempsql,0,UT_TYPE_ULONG,4,&llcount);
	memset(tempsql,0,sizeof(tempsql)-1);
	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," limit %d,%d",atoi(start),atoi(limit));
	
	recodeLogInfo("web task test data...","--------------525--------------",sql);
	psCur=pasDbOpenSql(sql,0);
	iNum=0;
	iret=0;
	if(psCur!=NULL){
		long long sid=0;
		char taskname_str[64]="";
		unsigned int taskid_num=0;
		char taskkey_str[512]="";
		char Begtime[512]="";
		char Endtime[512]="";
		unsigned int Gpid=0;
		unsigned int Computerid=0;
		unsigned int stime=0;
		char userer_str[32]="";

		while(0==(iret=pasDbFetchInto(psCur,
			UT_TYPE_LONG8,			sizeof(sid),					&sid,
			UT_TYPE_STRING,			sizeof(taskname_str)-1,			taskname_str,
			UT_TYPE_ULONG,			sizeof(taskid_num),				&taskid_num,
			UT_TYPE_STRING,			sizeof(taskkey_str)-1,			taskkey_str,
			UT_TYPE_STRING,			sizeof(Begtime)-1,				Begtime,
			UT_TYPE_STRING,			sizeof(Endtime)-1,				Endtime,
			UT_TYPE_ULONG,			sizeof(Gpid),					&Gpid,
			UT_TYPE_ULONG,			sizeof(Computerid),				&Computerid,
			UT_TYPE_ULONG,			sizeof(stime),					&stime,
			UT_TYPE_STRING,			sizeof(userer_str)-1,			userer_str))||1405==iret){
			

			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"sid",iNum,"%llu",sid);
			utPltPutLoopVar(psDbHead,"taskname",iNum,taskname_str);
			utPltPutLoopVarF(psDbHead,"taskid",iNum,"%u",taskid_num);
			utPltPutLoopVar(psDbHead,"taskkey",iNum,taskkey_str);
			utPltPutLoopVar(psDbHead,"Begtime",iNum,Begtime);
			utPltPutLoopVar(psDbHead,"Endtime",iNum,Endtime);
			utPltPutLoopVarF(psDbHead,"Gpid",iNum,"%u",Gpid);
			utPltPutLoopVarF(psDbHead,"Computerid",iNum,"%u",Computerid);
			char webtime[512];
			memset(webtime,0,sizeof(webtime));
			time_t timenum=(time_t)stime;
			local_time=localtime(&timenum);
			strftime(webtime,sizeof(webtime)-1,"%Y-%m-%d %H:%M:%S",local_time);
			utPltPutLoopVarF(psDbHead,"stime",iNum,webtime);
			utPltPutLoopVar(psDbHead,"userer",iNum,userer_str);

		}

		pasDbCloseCursor(psCur);
	}
	utPltPutVarF(psDbHead,"TotRec","%d",llcount);
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/outfile/lan_outfile_task.htm");
	return 0;
}
//****************************************************************************************************************
//****************************************************外发文件关键词检索任务下发接口函数BEGIN*********************
//****************************************************************************************************************
bool writeoutfilekeylaw(unsigned long int taskid,unsigned long int computerid,int runlevel){
	char mesg[1024]="";
	char sql[512]="";
	snprintf(sql,sizeof(sql),"select taskkey,Begtime,Endtime from nwoutfiletask where taskid=%u",taskid);
	char taskkey[128]="";
	char begTime[32]="";
	char endTime[32]="";
	pasDbOneRecord(sql,0,UT_TYPE_STRING,sizeof(taskkey),taskkey,
		UT_TYPE_STRING,sizeof(begTime),begTime,
		UT_TYPE_STRING,sizeof(endTime),endTime);
	DebugLog("writeoutfilekeylaw...","",sql);
	if(strlen(taskkey)<=0)
		return false;
	char path[256]="";
	snprintf(path,sizeof(path),"%s%u",OUTFILE_KEY_LAW_PATH,computerid);
	if(!utFileIsExist(path)){
		char comd[512]="";
		snprintf(comd,sizeof(comd)-1,"mkdir -p %s",path);
		system(comd);
	}
	char newpath[512]="";
	snprintf(newpath,sizeof(newpath)-1,"%s/%u.txt",path,computerid);
	FILE *fp;
	fp=fopen(newpath,"a+");
	if(fp){
		DebugLog("writeoutfilekeylaw ...",newpath,begTime);
		fprintf(fp,"taskid=%u;taskkey=%s;begTime=%s;endTime=%s;runlevel=%d;\r\n",taskid,taskkey,begTime,endTime,runlevel);
		fclose(fp);
	}
	return true;
}
void DloadOutFileKeyLaw(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char sql[512]="";
	char mesg[1024]="";
	snprintf(sql,sizeof(sql),"select Computerid,taskid,runlevel from nwoutfilelaw where Status=%d",0);
	utPltDbHead *psDbHead=utPltInitDb();
	pasDbCursor *psCur=NULL;
	psCur=pasDbOpenSql(sql,0);
	int iNUm=0;
	int iret=0;
	BUF_ARRAY *prootbuf=NULL;
	DebugLog("DloadOutFileKeyLaw ...","into....",sql);
	if(psCur!=NULL){
		unsigned long int taskid=0;
		unsigned long int computerid=0;
		int runlevel=0;
		BUF_ARRAY mydata;
		while(0==(iret=pasDbFetchInto(psCur,
			UT_TYPE_ULONG,	sizeof(computerid),&computerid,
			UT_TYPE_ULONG,	sizeof(taskid),&taskid,
			UT_TYPE_ULONG,	sizeof(runlevel),&runlevel))||1405==iret){
				snprintf(mesg,sizeof(mesg),"computerid=%u,taskid=%u,runlevel=%d",computerid,taskid,runlevel);
				DebugLog("DloadOutFileKeyLaw ...","",mesg);
				mydata.info.item.Computerid=computerid;
				mydata.info.item.taskid=taskid;
				mydata.info.item.runlevel=runlevel;
				prootbuf=AddBufItem(prootbuf,&mydata);
		}
		pasDbCloseCursor(psCur);
	}
	BUF_ARRAY *pitem=prootbuf;
	while(pitem!=NULL){
		if(writeoutfilekeylaw(pitem->info.item.taskid,pitem->info.item.Computerid,pitem->info.item.runlevel)){
			nwCompIp *psCompIp;
			psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&(pitem->info.item.Computerid));
			if(psCompIp){
				psCompIp->lCtrl|=0X00200000;
				snprintf(mesg,sizeof(mesg),"psCompIp->lCtrl=%0X",psCompIp->lCtrl);
				DebugLog("DloadOutFileKeyLaw ...","",mesg);
			}

			memset(sql,0,sizeof(sql));
			snprintf(sql,sizeof(sql),"update nwoutfilelaw set Status=1 where Computerid=%u and taskid=%u",pitem->info.item.Computerid,pitem->info.item.taskid);
			if(pasDbExecSql(sql, 0)){
				AddLogToFile("DloadOutFileKeyLaw....","sql err!",sql);       					
			}
		}
		pitem=pitem->pnext;
	}
	ClearBufItem(prootbuf);
	prootbuf=NULL;
}
int DownLaodOutFileKeyLaw(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	char mesg[1024]="";
	unsigned long int compid=0;
	int iRet=utMsgGetSomeNVar(psMsgHead,1,"compid",UT_TYPE_ULONG,4,&compid);
	snprintf(mesg,sizeof(mesg),"compid=%u",compid);
	if(compid<=0){
		DebugLog("DownLaodOutFileKeyLaw....","","compid<=0");
		return -1;
	}
	
	char filePath[512]="";
	snprintf(filePath,sizeof(filePath)-1,"%s%u/%u.txt",OUTFILE_KEY_LAW_PATH,compid,compid);
	int iSize=utFileSize(filePath);
	if(iSize <= 0) {
		DebugLog("DownLaodOutFileKeyLaw error","","iSize <= 0");
		return -1;
	}
	FILE *fp=fopen(filePath,"r");
	if(fp==NULL){
		DebugLog("DownLaodOutFileKeyLaw error","","fp==NULL");
		return -1;
	}
	char data[512]="";
	char sql[512]="";
	while(NULL!=fgets(data,sizeof(data),fp)){
		if(strlen(data)){
			char *temp;
			temp=strchr(data,'=');
			if(temp){
				char taskid[8]="";
				temp++;
				char *str;
				str=strchr(temp,';');
				if(str)
					memcpy(taskid,temp,strlen(temp)-strlen(str));
				if(taskid[0]!=0){
					snprintf(sql,sizeof(sql),"update nwoutfilelaw set Status=2 where Computerid=%u and taskid=%u",compid,atoi(taskid));
					if(pasDbExecSql(sql, 0)){
						AddLogToFile("DownLaodOutFileKeyLaw....","sql err!",sql);       					
					}
				}
			}
		}
	}
	fclose(fp);
	fp=fopen(filePath,"rb");
	if(fp==NULL){
		DebugLog("DownLaodOutFileKeyLaw error","","fp==NULL");
		return -1;
	}
	char *pBuf;
	pBuf=malloc(iSize+1);
	if(pBuf==NULL){
		DebugLog("DownLaodOutFileKeyLaw error","","pBuf == NULL");
		return (-1);
	}
	memset(pBuf,0,iSize+1);
	int len=fread(pBuf,1,iSize,fp);
	if(len<=0){
		DebugLog("DownLaodOutFileKeyLaw error","","len<=0");
		return -1;
	}
	int iwrite=write(iFd,utf8convert(psShmHead,pBuf),len);
	if(iwrite!=len){
		DebugLog("DownLaodOutFileKeyLaw error","","iwrite!=len");
	}
	fclose(fp);
	free(pBuf);
	
	return 0;
}
int RemoveSerOutFileKeyLaw(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char mesg[1024]="";
	unsigned long int compid=0;
	int Status=0;
	int iRet=utMsgGetSomeNVar(psMsgHead,1,"compid",UT_TYPE_ULONG, 4, &compid);
	if(compid<=0){
		DebugLog("RemoveSerOutFileKeyLaw...","","compid<=0");
		pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "Status",   UT_TYPE_STRUCT,  &Status, -1);
		return -1;
	}
	char path[256]="";
	snprintf(path,sizeof(path),"%s%u/%u.txt",OUTFILE_KEY_LAW_PATH,compid,compid);
	remove(path);
	nwCompIp *psCompIp;
	psCompIp=(nwCompIp*)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
	if(psCompIp) {
		psCompIp->lCtrl&=~0X00200000;
		pasTcpResponse(iFd,psMsgHead,NULL,1,"Status",   UT_TYPE_LONG,1);
		return 0;
	}
	pasTcpResponse(iFd,psMsgHead,NULL,1,"Status",UT_TYPE_LONG,-1);
	return -1;
}

int UpDateOutFileKeyLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char mesg[1024]="";
	unsigned long int compid=0;
	unsigned long int taskid=0;
	long int num=0;
	char begtim[32]="";
	char sql[1024]="";
	char endtim[32]="";
	int Status=0;
	int type=0;
	char info[512]="";
	int iRet=utMsgGetSomeNVar(psMsgHead,7,
		            "compid",  UT_TYPE_ULONG, 4, &compid,
					"taskid",  UT_TYPE_ULONG, 4, &taskid,
					"num",    UT_TYPE_LONG, 4, &num,
					"begtim", UT_TYPE_STRING,sizeof(begtim),begtim,
					"endtim", UT_TYPE_STRING,sizeof(endtim),endtim,
					"type",    UT_TYPE_LONG, 4, &type,
					"info", UT_TYPE_STRING,sizeof(info),info);
	if(compid<=0||taskid<=0||num<=0||strlen(begtim)<=0||strlen(endtim)<=0){
		DebugLog("UpDateOutFileKeyLog error","","compid<=0||taskid<=0||num<=0||strlen(begtim)<=0||strlen(endtim)<=0");
		pasTcpResponse(iFd,psMsgHead,NULL,1,"Status",UT_TYPE_STRUCT,  &Status, -1);
		return -1;
	}
	snprintf(mesg,sizeof(mesg),"compid=%u taskid=%u num=%d begtim=%s endtim=%s type=%d info=%s",compid,taskid,num,begtim,endtim,type,utf8convert(psShmHead,info));
	DebugLog("UpDateOutFileKeyLog ...","",mesg);
	snprintf(sql,sizeof(sql),"select count(*) from nwoutfilelaw where Computerid=%u and taskid=%d",compid,taskid);
	int lcount=0;
	pasDbOneRecord(sql,0,UT_TYPE_ULONG,sizeof(lcount),&lcount);
	if(lcount<=0){
		pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "Status",   UT_TYPE_STRUCT,  &Status, -8);
		DebugLog("UpDateOutFileKeyLog called err...","","任务已被删除，任务终止....");
		return -1;
	}
	memset(sql,0,sizeof(sql));
	char *pos=strstr((char *)utf8convert(psShmHead,info),"已检索完成");
	if(pos){
		snprintf(sql,sizeof(sql),"update nwoutfilelaw set Status=3,downcount='%s' where Computerid=%u and taskid=%d",utf8convert(psShmHead,info),compid,taskid);
	}else{
		snprintf(sql,sizeof(sql),"update nwoutfilelaw set downcount='%s' where Computerid=%u and taskid=%d",utf8convert(psShmHead,info),compid,taskid);
	}
	pasDbExecSql(sql, 0);
	if(type==0){
		DebugLog("UpDateOutFileKeyLog ...","type==0","md5 是无效的,不包含关键词....");
		return 0;
	}
	char timeBeg[32]="";
	memcpy(timeBeg,begtim,4);
	memcpy(timeBeg+4,begtim+5,2);
	if(strlen(timeBeg)<=0){
		pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "Status",   UT_TYPE_STRUCT,  &Status, -1);
		DebugLog("UpDateOutFileKeyLog called err...","","strlen(timeBeg)<=0");
		return -1;
	}
	char timeEnd[32]="";
	memcpy(timeEnd,endtim,4);
	memcpy(timeEnd+4,endtim+5,2);
	if(strlen(timeEnd)<=0){
		pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "Status",   UT_TYPE_STRUCT,  &Status, -1);
		DebugLog("UpDateOutFileKeyLog called err...","","strlen(timeEnd)<=0");
		return -1;
	}
	
	int beg_time=atoi(timeBeg);
	int end_time=atoi(timeEnd);
	if(beg_time>end_time){
		pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "Status",   UT_TYPE_STRUCT,  &Status, -1);
		DebugLog("UpDateOutFileKeyLog called err...","","beg_time>end_time");
		return -1;
	}
	int j=beg_time;
	char mytime[32]="";
	char month[4]="";
	char tablename[32]="";
	int i=0;
	char str[2][256];
	char md5[256]="";
	memset(str,0,2*256);
	
	BUF_ARRAY *prootbuf=NULL;
	snprintf(mesg,sizeof(mesg),"beg_time=%d end_time=%d",beg_time,end_time);
	DebugLog("UpDateOutFileKeyLog called ...","",mesg);
	for(j=beg_time;j<=end_time;j++){
		memset(mytime,0,sizeof(mytime));
		memset(month,0,sizeof(month));
		snprintf(mytime,sizeof(mytime),"%d",j);
		memcpy(month,mytime+4,2);
		if(atoi(month)>12)
			continue;
		memset(tablename,0,sizeof(tablename));
		snprintf(tablename,sizeof(tablename)," nwoutfilelog_%d",j);
		for(i=0;i<num;i++){
			snprintf(str[0],256,"md5%d",i);
			iRet=utMsgGetSomeNVar(psMsgHead,1,str[0], UT_TYPE_STRING, sizeof(md5), md5);
			if(strlen(md5)<=0){
				DebugLog("UpDateOutFileKeyLog ...","recv one err data...",md5);
				continue;
			}
			unsigned long int stime=0;
			memset(sql,0,sizeof(sql));
			snprintf(sql,sizeof(sql),"select max(stime) from %s where compid=%u and content='%s'",tablename,compid,md5);
			pasDbOneRecord(sql,0,UT_TYPE_ULONG,sizeof(stime),&stime);
			if(stime<=0)
				continue;
			memset(sql,0,sizeof(sql));
			snprintf(sql,sizeof(sql),"select userid,mac,groupid,devname,udisp,sip,procname,mark,indexid,souPath,destPath,content,Blacktype,Backuptype,filesize from %s where stime=%lu and compid=%u and content='%s'",
				tablename,stime,compid,md5);
			DebugLog("UpDateOutFileKeyLog called ...","",sql);
			utPltDbHead *psDbHead=utPltInitDb();
			pasDbCursor *psCur=NULL;
			psCur=pasDbOpenSql(sql,0);
			int iNUm=0;
			int iret=0;
			if(psCur!=NULL){
				unsigned long int userid=0;
				char mac[32]="";
				unsigned long int groupid=0;
				char devname[32]="";
				char udisp[32]="";
				unsigned long int sip=0;
				char procname[72]="";
				unsigned long int mark=0;
				char indexid[32]="";
				char souPath[256]="";
				char destPath[256]="";
				char content[256]="";
				int Blacktype=0;
				int Backuptype=0;
				unsigned long int filesize=0;
				BUF_ARRAY mydata;

				while(0==(iret=pasDbFetchInto(psCur,
					UT_TYPE_ULONG,4,&userid,
					UT_TYPE_STRING,sizeof(mac),mac,
					UT_TYPE_ULONG,4,&groupid,
					UT_TYPE_STRING,sizeof(devname),devname,
					UT_TYPE_STRING,sizeof(udisp),udisp,
					UT_TYPE_ULONG,4,&sip,
					UT_TYPE_STRING,sizeof(procname),procname,
					UT_TYPE_ULONG,4,&mark,
					UT_TYPE_STRING,sizeof(indexid),indexid,
					UT_TYPE_STRING,sizeof(souPath),souPath,
					UT_TYPE_STRING,sizeof(destPath),destPath,
					UT_TYPE_STRING,sizeof(content),content,
					UT_TYPE_ULONG,4,&Blacktype,
					UT_TYPE_ULONG,4,&Backuptype,
					UT_TYPE_ULONG,4,&filesize))||1405==iret){
						memset(&mydata,0,sizeof(BUF_ARRAY));
						mydata.info.outfilekey.userid=userid;
						if(strlen(mac)>0)
							memcpy(mydata.info.outfilekey.mac,mac,strlen(mac));
						mydata.info.outfilekey.groupid=groupid;
						if(strlen(devname)>0)
							memcpy(mydata.info.outfilekey.devname,(char *)utf8convert(psShmHead,devname),strlen(devname));
						if(strlen(udisp)>0)
							memcpy(mydata.info.outfilekey.udisp,(char *)utf8convert(psShmHead,udisp),strlen(udisp));
						mydata.info.outfilekey.sip=sip;
						if(strlen(procname)>0)
							memcpy(mydata.info.outfilekey.procname,(char *)utf8convert(psShmHead,procname),strlen(procname));
						mydata.info.outfilekey.mark=mark;
						if(strlen(indexid)>0)
							memcpy(mydata.info.outfilekey.indexid,(char *)utf8convert(psShmHead,indexid),strlen(indexid));
						if(strlen(souPath)>0)
							memcpy(mydata.info.outfilekey.souPath,(char *)utf8convert(psShmHead,souPath),strlen(souPath));
						if(strlen(destPath)>0)
							memcpy(mydata.info.outfilekey.destPath,(char *)utf8convert(psShmHead,destPath),strlen(destPath));
						if(strlen(content)>0)
							memcpy(mydata.info.outfilekey.content,(char *)utf8convert(psShmHead,content),strlen(content));
						mydata.info.outfilekey.Blacktype=Blacktype;
						mydata.info.outfilekey.Backuptype=Backuptype;
						mydata.info.outfilekey.filesize=filesize;
						prootbuf=AddBufItem(prootbuf,&mydata);
				}
				
				pasDbCloseCursor(psCur);
			}
		}
	}
	BUF_ARRAY *pitem=prootbuf;
	unsigned long long int curTime;
	while(pitem!=NULL){
		memset(sql,0,sizeof(sql));
		curTime = pasGetLongTimeId();
		unsigned long int stime=0;
		snprintf(sql,sizeof(sql),"select stime from nwoutfilelaw where taskid=%u",taskid);
		pasDbOneRecord(sql,0,UT_TYPE_ULONG,sizeof(stime),&stime);
		if(stime<=0)
			continue;
		memset(sql,0,sizeof(sql));
		snprintf(sql,sizeof(sql),"insert into nwoutfilelog(sid,compid,userid,mac,groupid,devname,udisp,sip,procname,mark,Blacktype,Backuptype,filesize,indexid,stime,souPath,destPath,content,taskid) values(%llu,%u,%u,\'%s\',%u,\'%s\',\'%s\',%u,\'%s\',%u,%u,%u,%u,\'%s\',%u,\'%s\',\'%s\',\'%s\',%u)",
								 curTime,compid,pitem->info.outfilekey.userid,pitem->info.outfilekey.mac,pitem->info.outfilekey.groupid,
								 pitem->info.outfilekey.devname,pitem->info.outfilekey.udisp,pitem->info.outfilekey.sip,pitem->info.outfilekey.procname,
								 pitem->info.outfilekey.mark,pitem->info.outfilekey.Blacktype,pitem->info.outfilekey.Backuptype,pitem->info.outfilekey.filesize,pitem->info.outfilekey.indexid,
								 stime,pitem->info.outfilekey.souPath,pitem->info.outfilekey.destPath,pitem->info.outfilekey.content,taskid);
		if(pasDbExecSql(sql, 0)){
			DebugLog("UpDateOutFileKeyLog....","sql err!",sql);       					
		}
		DebugLog("UpDateOutFileKeyLog....","",sql);
		pitem=pitem->pnext;
	}
	ClearBufItem(prootbuf);
	prootbuf=NULL;
	pasTcpResponse(iFd,psMsgHead,NULL,1,"Status",UT_TYPE_STRUCT,&Status, 1);
	return 0;
}

//****************************************************外发文件关键词检索任务下发接口函数END************************
//****************************************************************************************************************
//****************************************************************************************************************

int lan_log_outfile_law(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();

	char time_flag[8]="";
	char groupid[64]="";
	char compid[64]="";
	char keyword[128]="";
	char sdate[32]="";
	char edate[32]="";
	char flush_type[4]="";
	char caExport[32]="";
	char limitStr[32]="";
	char sort[32]="";
	char dir[32]="";
	char start[32]="";
	int llcount=0;

	utMsgGetSomeNVar(psMsgHead,12,
					"time_flag",						UT_TYPE_STRING,  			sizeof(time_flag)-1,  					time_flag,
					"groupid",							UT_TYPE_STRING,  			sizeof(groupid)-1,  					groupid,
   				     "compid",							UT_TYPE_STRING,  			sizeof(compid)-1,  						compid,
					 "keyword",							UT_TYPE_STRING,				sizeof(keyword)-1,						keyword,
					 "sdate",							UT_TYPE_STRING,				sizeof(sdate)-1,						sdate,	
					 "edate",							UT_TYPE_STRING,				sizeof(edate)-1,						edate,
					 "export2",						    UT_TYPE_STRING,				sizeof(caExport)-1,						caExport,
					 "limit",						    UT_TYPE_STRING,				sizeof(limitStr)-1,						limitStr,
					 "sort",						    UT_TYPE_STRING,				sizeof(sort)-1,							sort,
					 "dir",						   		UT_TYPE_STRING,				sizeof(dir)-1,							dir,
					 "start",						   	UT_TYPE_STRING,				sizeof(start)-1,						start,
					"flush_type",						UT_TYPE_STRING,				sizeof(flush_type)-1,					flush_type);

  	     
	char mesg[512];
	memset(mesg,0,sizeof(mesg));

	snprintf(mesg,512,"time_flag:%s\t,groupid:%s\t,compid:%s\t,keyword:%s\t,sdate:%s\t,edate:%s,export2:%s\t\r\n",
		time_flag,groupid,compid,utf8convert(psShmHead,keyword),sdate,edate,caExport);
	recodeLogInfo("web log test data...","**********994*******",mesg);

	time_t temp;
	time(&temp);
	int time_type=atoi(time_flag);
	time_t beg_time;
	time_t end_time;
	struct tm *local_time = NULL;
	struct tm when;
	local_time = localtime(&temp);
	end_time =temp;
	int i=0,j=0;
	switch(time_type){
		case 0:
			{
				//get begin time
				char year[8];
				memset(year,0,sizeof(year));
				char month[4];
				memset(month,0,sizeof(month));
				char day[4];
				memset(day,0,sizeof(day));
				for(i=0;i<strlen(sdate);i++){
					if(sdate[i]=='/')
						sdate[i]=' ';
				}
				sscanf(sdate,"%8s%4s%4s",year,month,day);
				when.tm_year=atoi(year)-1900;
				when.tm_mday=atoi(day);
				when.tm_mon=atoi(month);
				when.tm_hour=0;
				when.tm_min=0;
				when.tm_sec=0;
				beg_time=mktime(&when);
				memset(year,0,sizeof(year));
				memset(month,0,sizeof(month));
				memset(day,0,sizeof(day));
				for(i=0;i<strlen(edate);i++){
					if(edate[i]=='/')
						edate[i]=' ';
				}
				sscanf(edate,"%8s%4s%4s",year,month,day);
				when.tm_year=atoi(year)-1900;
				when.tm_mday=atoi(day);
				when.tm_mon=atoi(month);
				when.tm_hour=23;
				when.tm_min=59;
				when.tm_sec=59;
				end_time=mktime(&when);
			}
			break;
		case 1:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec;
			break;
		case 2:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec-3*24*3600;
			break;
		case 3:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec-7*24*3600;
			break;
		case 4:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec-30*24*3600;
			break;
		case 5:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec-90*24*3600;
			break;
		case 6:
			beg_time = temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec-180*24*3600;
			break;
	}
	if(atoi(flush_type)==1)
		DloadOutFileKeyLaw(psShmHead,iFd,psMsgHead);
	char importword[256]="";
	snprintf(importword,sizeof(importword)-1,"%s",utf8convert(psShmHead,keyword));
	char data1[128]="";
	char data2[128]="";
	
	bool flag=false;
	for(i=0;i<strlen(importword);i++){
		if(importword[i]=='/'){
			flag=true;
			j=0;
			continue;
		}
		if(!flag){
			data1[j]=importword[i];
			j++;
		}else{
			data2[j]=importword[i];
			j++;
		}
	}
	//make sql;
	char sql[1024]="";
	unsigned long int taskid=0;
	snprintf(sql,sizeof(sql)-1,"select sid,taskid,Computerid,Sip,Cpname,Gpid,Status,stime,userer,(select taskname from nwoutfiletask where taskid=a.taskid)as taskname,downcount from nwoutfilelaw as a where stime>=%u and stime<=%u",beg_time,end_time);
	if(strlen(data1)){
		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and inet_ntoa(Sip) like '%%%s%%'",data1);
		taskid=strtoul(data1,NULL,10);
		if(taskid>0){
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," or taskid=%u",taskid);
		}
	}
	if(strlen(data2)){
		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and inet_ntoa(Sip) like '%%%s%%'",data2);
		taskid=strtoul(data2,NULL,10);
		if(taskid>0){
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," or taskid=%u",taskid);
		}
	}
	 
	char tempsql[1024]="";
	snprintf(tempsql,sizeof(tempsql)-1,sql);
	replaceStr(tempsql,"sid,taskid,Computerid,Sip,Cpname,Gpid,Status,stime,userer,(select taskname from nwoutfiletask where taskid=a.taskid)as taskname,downcount","count(*)");
	recodeLogInfo("web log test data...","************1126************",tempsql);
	pasDbOneRecord(tempsql,0,UT_TYPE_ULONG,4,&llcount);
	memset(tempsql,0,sizeof(tempsql)-1);
	recodeLogInfo("web log test data...","************1129************",sql);
	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," limit %d,%d",atoi(start),atoi(limitStr));
	pasDbCursor *psCur=NULL;
	psCur=pasDbOpenSql(sql,0);
	int iNum=0;
	int iret=0;
	
			
	if(psCur){
		
		unsigned long long int sid=0;
		unsigned long int Computerid=0;
		unsigned long int Sip=0;
		char ComName[128]="";
		unsigned long int Gpid=0;
		unsigned long int Statues=0;
		unsigned long int stime=0;
		char userer[64]="";
		taskid =0;
		char taskname[32]="";
		char dataStr[1024]="";
		char  downcount[512]="";
		
		while(0==(iret=pasDbFetchInto(psCur,
			UT_TYPE_LONG8,			sizeof(sid),					&sid,
			UT_TYPE_ULONG,			sizeof(taskid),					&taskid,
			UT_TYPE_ULONG,			sizeof(Computerid),				&Computerid,
			UT_TYPE_ULONG,			sizeof(Sip),					&Sip,
			UT_TYPE_STRING,			sizeof(ComName)-1,				ComName,
			UT_TYPE_ULONG,			sizeof(Gpid),					&Gpid,
			UT_TYPE_ULONG,			sizeof(Statues),				&Statues,
			UT_TYPE_ULONG,			sizeof(stime),					&stime,
			UT_TYPE_STRING,			sizeof(userer)-1,				userer,
			UT_TYPE_STRING,			sizeof(taskname)-1,				taskname,
			UT_TYPE_STRING,			sizeof(downcount)-1,				downcount
		))||1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"sid",iNum,"%llu",sid);
			utPltPutLoopVar(psDbHead,"downcount",iNum,downcount);
			utPltPutLoopVarF(psDbHead,"taskid",iNum,"%u",taskid);
			utPltPutLoopVarF(psDbHead,"Computerid",iNum,"%u",Computerid);


			struct in_addr addr;
			addr.s_addr=htonl(Sip);
			char webip[128]="";
			snprintf(webip,sizeof(webip)-1,"%s",inet_ntoa(addr));
			utPltPutLoopVar(psDbHead,"webip",iNum,webip);
			utPltPutLoopVar(psDbHead,"ComName",iNum,ComName);
			utPltPutLoopVar(psDbHead,"taskname",iNum,taskname);
			recodeLogInfo("web task test data...","***********************",ComName);
			utPltPutLoopVarF(psDbHead,"Gpid",iNum,"%u",Gpid);
			
			char law_statues[32]="";
			if(Statues==0){
				snprintf(law_statues,sizeof(law_statues)-1,"%s","等待下发");
			}else if(Statues==1){
				snprintf(law_statues,sizeof(law_statues)-1,"%s","等待查询");
			}else if(Statues==2){
				snprintf(law_statues,sizeof(law_statues)-1,"%s","正在查询");
			}else if(Statues==3){
				snprintf(law_statues,sizeof(law_statues)-1,"%s","查询完成");
			}
			utPltPutLoopVar(psDbHead,"law_statues",iNum,law_statues);
			
			char web_time[128]="";
			local_time=localtime(&stime);
			strftime(web_time,sizeof(web_time)-1,"%Y-%m-%d %H:%M:%S",local_time);
			utPltPutLoopVar(psDbHead,"web_time",iNum,web_time);
			utPltPutLoopVar(psDbHead,"userer",iNum,userer);

			sid=0;
			Computerid=0;
			Sip=0;
			memset(ComName,0,sizeof(ComName));
			Gpid=0;
			Statues=0;
			stime=0;
			memset(userer,0,sizeof(userer));
			taskid =0;
			memset(taskname,0,sizeof(taskname));
			memset(dataStr,0,sizeof(dataStr));
			memset(downcount,0,sizeof(downcount));
			//recodeLogInfo("web log test data...","***********1214*************",caExport);
		}
		pasDbCloseCursor(psCur);
	}
	utPltPutVarF(psDbHead,"TotRec","%d",llcount);
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/outfile/lan_outfile_law.htm");
	return 0;
}

int ncInitNwWebPHOutFileTaskFunc(utShmHead *psShmHead)
{
	pasSetTcpFunName("lan_log_outfile_task",lan_log_outfile_task,NULL,0,0);
	pasSetTcpFunName("lan_log_outfile_law",lan_log_outfile_law,NULL,0,0);
	pasSetTcpFunName("DownLaodOutFileKeyLaw",DownLaodOutFileKeyLaw,NULL,0,0);
	pasSetTcpFunName("RemoveSerOutFileKeyLaw",RemoveSerOutFileKeyLaw,NULL,0,0);
	pasSetTcpFunName("UpDateOutFileKeyLog",UpDateOutFileKeyLog,NULL,0,0);
	return 0;
}