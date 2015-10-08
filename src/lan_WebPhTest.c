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



int8 pasGetLongTimeId();


#include <sys/stat.h>
#include <sys/types.h>

#define BLOCKDATA_FILE_PATH "/home/ncmysql/nw/html/v8/outfile/data.txt"
#define TASKID_RESULT	"/home/ncmysql/nw/html/v8/outfile/taskid.txt"
char BLOCKDATA[180][64]={""};
void readData()
{
	if(BLOCKDATA[1][0]!=0)
		return;
	FILE *fp;
	char data[512];
	fp=fopen(BLOCKDATA_FILE_PATH,"r");
	
	if(fp){
		memset(data,0,sizeof(data));
		while(NULL!=(fgets(data,sizeof(data),fp))){
			if(strlen(data)>0){
				int i=0;
				for(i=0;i<strlen(data);i++){
					if(data[i]=='#'||data[i]=='&')
						data[i]=' ';
				}
				char count[5];
				memset(count,0,sizeof(count));
				char num[8];
				memset(num,0,sizeof(num));
				char myData[64];
				memset(myData,0,sizeof(myData));

				sscanf(data,"%5s%8s%64s",count,num,myData);
				int myNum=atoi(num);
				if(myNum>=0&&myNum<180){
					snprintf(BLOCKDATA[myNum],64,"%s",myData);
				}
			}
			memset(data,0,sizeof(data));
		}
	}
}
void SaveDataToFile(char *filepath,char *data){
	if(strlen(filepath)<=0||strlen(data)<=0)
		return;
	FILE *fp=fopen(filepath,"a+");
	if(fp){
		fprintf(fp,"%s\r\n",data);
		fclose(fp);
	}
}

void GetMarkData(int mark,char *data)
{
	switch(mark)
	{
		case 12289:
			snprintf(data,strlen("HTTP文件外发")+1,"%s","HTTP文件外发");
		break;
		case 12290:
			snprintf(data,strlen("SMTP文件外发")+1,"%s","SMTP文件外发");
		break;
		case 12291:
			snprintf(data,strlen("FTP文件外发")+1,"%s","FTP文件外发");
		break;
		case 12293:
			snprintf(data,strlen("聊天文件外发")+1,"%s","聊天文件外发");
		break;
		case 12294:
			snprintf(data,strlen("云盘文件外发")+1,"%s","云盘文件外发");
		break;
		case 12295:
			snprintf(data,strlen("U盘文件外发")+1,"%s","USB文件外发");
		break;
		case 12296:
			snprintf(data,strlen("共享目录外发")+1,"%s","共享目录外发");
		break;
		case 12297:
			snprintf(data,strlen("其他程序外发")+1,"%s","其他程序外发");
		break;
		case 12298:
			snprintf(data,strlen("htts外发")+1,"%s","htts外发");
		break;
	}
}
unsigned int CheckIp(char *data){
	if(strlen(data)<=0)
		return 1;

	char buffer[512];
	memset(buffer,0,sizeof(buffer));
	snprintf(buffer,sizeof(buffer),"%s",data);
	int i=0;
	int count=0;
	for(i=0;i<strlen(buffer);i++){
		if(buffer[i]=='.'){
			buffer[i]=' ';
			count++;
		}
	}
	if(count!=3)
		return 1;
	char data1[12];
	char data2[12];
	char data3[12];
	char data4[12];
	memset(data1,0,sizeof(data1));
	memset(data2,0,sizeof(data2));
	memset(data3,0,sizeof(data3));
	memset(data4,0,sizeof(data4));
	sscanf(buffer,"%10s%10s%10s%10s",data1,data2,data3,data4);
	if(atoi(data1)<=0||atoi(data1)>255) return 1;
	if(atoi(data2)<0||atoi(data2)>255) return 1;
	if(atoi(data3)<0||atoi(data3)>255) return 1;
	if(atoi(data4)<0||atoi(data4)>255) return 1;
	
	unsigned int k = inet_addr(data);
    	k=htonl(k);
	return k;
	return 1;
}
void getIpChar(int data,char *ipChar){
	
}
void getBlocktypeStr(int num,char *typeStr){
    snprintf(typeStr,sizeof(BLOCKDATA[num]),"%s",BLOCKDATA[num]);

}
/******************************************************************************************/
/** 
 * 参数： 
 * originalString[] ：原始字符串 
 * key[] ： 待替换的字符串 
 * swap[] : 新字符串 
 */  
void replaceStr(char originalString[], char key[], char swap[]){  
    int lengthOfOriginalString, lengthOfKey, lengthOfSwap, i, j , flag;  
    char tmp[1000];   
  
    //获取各个字符串的长度  
    lengthOfOriginalString = strlen(originalString);  
    lengthOfKey = strlen(key);  
    lengthOfSwap = strlen(swap);  
  
    for( i = 0; i <= lengthOfOriginalString - lengthOfKey; i++){  
        flag = 1;  
        //搜索key  
        for(j  = 0; j < lengthOfKey; j ++){  
            if(originalString[i + j] != key[j]){  
                flag = 0;   
                break;  
            }  
        }  
        //如果搜索成功，则进行替换  
        if(flag){  
            strcpy(tmp, originalString);  
            strcpy(&tmp[i], swap);  
            strcpy(&tmp[i + lengthOfSwap], &originalString[i  + lengthOfKey]);  
            strcpy(originalString, tmp);  
            i += lengthOfSwap - 1;  
            lengthOfOriginalString = strlen(originalString);  
        }  
    }  
} 
int lan_log_getFileLogTest(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);
	readData();
	utPltDbHead *psDbHead = utPltInitDb();
	long iReturn=0;
	
	char _keyword[33]="";
	char _uid[17]="";
	char ip1[24]="";
	char sip1[24]="";
	char department1[16]="";
	char procname1[64]="";
	char filename1[72]="";
	char mac1[72]="";
	char compid1[32]="";
	char compid[32]="";
	char mark1[32]="";
	char sdate[11] = "";
	char edate[11] = "";
	char stime1[11] = "";
	char etime1[11] = "";
	char time_flag[2] = "";
	char start[17] = "";
 	char limit[17] = "";
 	char sort[16] = "";
 	char dir[16] = "";
	char blocktype[10]="";
 	char caExport[12];
	char filesizelimit[24]="";
	int filesize=0;
	int llcount=0;
 	
 	memset(caExport,0,sizeof(caExport));
	
	
	utMsgGetSomeNVar(psMsgHead, 23,
   				     "keyword",				UT_TYPE_STRING,  			sizeof(_keyword)-1,  	 _keyword,
   				     "uid",          		UT_TYPE_STRING,  			sizeof(_uid)-1,       	_uid,
   				     "ip1",       			UT_TYPE_STRING,  			sizeof(ip1)-1,     		ip1,
   				     "sip1",        		UT_TYPE_STRING,  			sizeof(sip1)-1,       	sip1,
   				     "department1",  		UT_TYPE_STRING,  			sizeof(department1)-1, 	department1,
   				     "procname1",			UT_TYPE_STRING,  			sizeof(procname1)-1,    procname1,  
   				     "filename1",    		UT_TYPE_STRING,  			sizeof(filename1)-1,   	filename1,
   				     "compid1",				UT_TYPE_STRING, 			sizeof(compid1)-1, 		compid1,
					 "compid",				UT_TYPE_STRING, 			sizeof(compid)-1, 		compid,
					 "mac1",				UT_TYPE_STRING, 			sizeof(mac1)-1, 		mac1,
   				     "mark1",      			UT_TYPE_STRING,  			sizeof(mark1)-1,       	mark1, 
				     "BlockType",			UT_TYPE_STRING,  			sizeof(blocktype)-1,         	blocktype,				     
   				     "sdate", 	   			UT_TYPE_STRING,  			sizeof(sdate)-1,       	sdate,
   				     "edate", 	   			UT_TYPE_STRING,  			sizeof(edate)-1,       	edate,
					"stime1", 	   			UT_TYPE_STRING,  			sizeof(stime1)-1,       stime1,
   				     "etime1", 	   			UT_TYPE_STRING,  			sizeof(etime1)-1,       etime1,
   				     "time_flag",  			UT_TYPE_STRING,  			sizeof(time_flag)-1,   	time_flag,
   				     "start",      			UT_TYPE_STRING,  			sizeof(start)-1,       	start,
   				     "limit",      			UT_TYPE_STRING,  			sizeof(limit)-1,       	limit,
   				     "sort",       			UT_TYPE_STRING,  			sizeof(sort)-1,        	sort,
   				     "dir",        			UT_TYPE_STRING,  			sizeof(dir)-1,         	dir,
   				     "export2",    			UT_TYPE_STRING,  			10,  					caExport,
					 "filesizelimit",    	UT_TYPE_STRING,  			sizeof(filesizelimit)-1,filesizelimit);

  	     
  char mesg[512];
  memset(mesg,0,sizeof(mesg));
  filesize=atoi(filesizelimit);
	
	int start_time=-1;
	int end_time=-1;

	int time_type=atoi(time_flag);

	int flag_type=-1;
	char data[24];
	memset(data,0,sizeof(data));
	time_t temp;
	struct tm *local_time = NULL;
	int count=0;
	char filenames[128];
	memset(filenames,0,sizeof(filenames));
	char ip[128];
	memset(ip,0,sizeof(ip));
	if(strlen(sdate)<=0||strlen(edate)<=0){
		
		time(&temp);
		local_time = localtime(&temp);
		strftime(data, sizeof(data), "%Y%m", local_time);
		end_time=atoi(data);
		switch(time_type)
		{
			case 1:
				flag_type=1;
				temp=temp-0;
				break;
			case 2:
				flag_type=1;
				temp=temp-3*24*3600;
				break;
			case 3:
				flag_type=1;
				temp=temp-7*24*3600;
				break;
			case 4:
				flag_type=2;
				temp=temp-30*24*3600;
				break;
			case 5:
				flag_type=2;
				temp=temp-90*24*3600;
				break;
			case 6:
				flag_type=2;
				temp=temp-180*24*3600;
				break;
		} 
		
		local_time = localtime(&temp);
		memset(data,0,sizeof(data));
		strftime(data, sizeof(data), "%Y%m", local_time);
		start_time=atoi(data);
	}
	else{
		flag_type = 0;
		memset(data,0,sizeof(data));

		int j=0;
		unsigned int i=0;
		for(i=0;i<strlen(sdate);i++){
			if(count==2)
				break;
			else{
				if(sdate[i]=='/'){
					count++;
					continue;
				}else{
					data[j]=sdate[i];
					j++;
				}
			}
		}
		j=0;
		start_time=atoi(data);
		count=0;
		memset(data,0,sizeof(data));
		unsigned int m=0;
		for(m=0;m<strlen(edate);m++){
			if(count==2)
				break;
			else{
				if(edate[m]=='/'){
					count++;
					continue;
				}else{
					data[j]=edate[m];
					j++;
				}
			}
		}
		end_time=atoi(data);
	}
	char *mysql=NULL;
	mysql=(char *)malloc(1024*10);
	if(!mysql)
		return 0;
	else
		memset(mysql,0,1024*10);
	int k=0;
	for(k=start_time;k<=end_time;k++){
		char tableanme[24];
		memset(tableanme,0,sizeof(tableanme));
		snprintf(tableanme,24,"nwoutfilelog_%d",k);
		bool flag=false;
		char year[5];
		char month[5];
		char day[5];
		memset(year,0,sizeof(year));
		memset(month,0,sizeof(month));
		memset(day,0,sizeof(day));
		int n=0;
		if(k==start_time){
			if(flag_type==1){
				flag = true;
				time(&temp);
				local_time = localtime(&temp);
				if(time_type==1){
					temp=temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec;
				}else if(time_type==2){
					temp=temp-3*24*3600-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec;
				}else if(time_type==3){
					temp=temp-7*24*3600-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec;
				}
			}else if(flag_type==0){
				flag=true;
				count=0;
				unsigned int m=0;
				for(m=0;m<strlen(sdate);m++){
					if(count==0){
						if(sdate[m]=='/'){
							count++;
							n=0;
							continue;
						}else{
							year[n]=sdate[m];
							n++;
						}
					}else if(count==1){
						if(sdate[m]=='/'){
							count++;
							n=0;
							continue;
						}else{
							month[n]=sdate[m];
							n++;
						}
					}else if(count==2){
						day[n]=sdate[m];
						n++;
					}
				}
				struct tm when;
				when.tm_year = atoi(year)-1900;
				when.tm_mday = atoi(day);
				when.tm_mon=atoi(month);
				when.tm_hour=0;
				when.tm_min=0;
				when.tm_sec=0;
				temp = mktime(&when);
			}

		}else if(k==end_time&&start_time!=end_time){
			if(flag_type==2){
				flag=true;
				time(&temp);
			}else if(flag_type==0){
				flag=true;
				count=0;
				unsigned int k=0;
				for(k=0;k<strlen(edate);k++){
					if(count==0){
						if(edate[k]=='/'){
							count++;
							n=0;
							continue;
						}else{
							year[n]=edate[k];
							n++;
						}
					}else if(count==1){
						if(edate[k]=='/'){
							count++;
							n=0;
							continue;
						}else{
							month[n]=edate[k];
							n++;
						}
					}else if(count==2){
						day[n]=edate[k];
						n++;
					}
				}
				struct tm when1;
				when1.tm_year = atoi(year)-1900;
				when1.tm_mday = atoi(day);
				when1.tm_mon=atoi(month);
				when1.tm_hour=23;
				when1.tm_min=59;
				when1.tm_sec=59;
				temp = mktime(&when1);
			}
		}
		
		char sql[1024*2];
		memset(sql,0,sizeof(sql));
		snprintf(sql, sizeof(sql)-1, "select count(*) from %s",tableanme);
	
		long lCount=0;
		int ret=pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
		if(ret==1146)
			continue;
		memset(sql,0,sizeof(sql));
		if(flag){
			if(flag_type==1){
				snprintf(sql,sizeof(sql),"select *  from %s where stime >=%lu",tableanme,temp);
			}else{
				
				if(k==start_time){
					snprintf(sql,sizeof(sql),"select * from %s where stime >=%lu",tableanme,temp);
				}else if(k==end_time){
					snprintf(sql,sizeof(sql),"select * from %s where stime <=%lu",tableanme,temp);
				}
			}
		}else{
			snprintf(sql,sizeof(sql),"select * from %s where stime <=%lu",tableanme,temp);
		}

		if(strlen(mark1)>0){
			unsigned int marknum=atoi(mark1);
			if(marknum>0)
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and mark=%u",marknum);
		}
		
		if(strlen(blocktype)>0){
			int int_blocktype=atoi(blocktype);
			if(int_blocktype>0){
				int begin_num=0;
				int end_num=0;
				int gloabtype=10000;
				switch(int_blocktype)
				{
					case 1:
						begin_num=20;
						end_num=31;
						gloabtype=1;
						break;
					case 2:
						begin_num=41;
						end_num=52;
						gloabtype=2;
						break;
					case 3:
						begin_num=60;
						end_num=67;
						gloabtype=3;
						break;
					case 4:
						begin_num=170;
						end_num=178;
						break;
					case 5:
						begin_num=100;
						end_num=107;
						gloabtype=5;
						break;
					case 6:
						begin_num=110;
						end_num=115;
						gloabtype=6;
						break;
					case 7:
						begin_num=120;
						end_num=125;
 						gloabtype=7;
						break;
					case 8:
						begin_num=141;
						end_num=145;
						gloabtype=8;
						break;
					case 9:
						begin_num=160;
						end_num=164;
						gloabtype=9;
						break;
				}

				if(begin_num>0&&end_num>0){
					snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and (Blacktype>=%u and Blacktype<=%u)or Blacktype=%u",begin_num,end_num,gloabtype);
				}
			}
		}
		if(strlen(ip1)>0){
			
			unsigned int ip_i=0,v=0,z=0;
			bool ip_flag=false;
			for(ip_i=0;ip_i<strlen(ip1);ip_i++){
				if(ip1[ip_i]=='/'){
					ip_flag=true;
					continue;
				}
				if(!ip_flag){
					filenames[v]=ip1[ip_i];
					v++;
				}else{
					ip[z]=ip1[ip_i];
					z++;
				}
			}
			if(strlen(filenames)>0)
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and inet_ntoa(sip) like '%%%s%%' or souPath like '%%%s%%' ",filenames,filenames);
			if(strlen(ip)>0)
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and inet_ntoa(sip) like '%%%s%%' or souPath like '%%%s%%' ",ip,ip);
		}


		

	//printf sql
	//recodeLogInfo("web log test data...","******MYSQL*******",mysql);
	if(filesize>0){
		switch(filesize){
		case 1:
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and filesize<=%ld",1024*1024);
			break;
		case 2:
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and (filesize >=%ld and filesize <= %ld)",1024*1024,10*1024*1024);
			break;

		case 3:
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and (filesize >=%ld and filesize <= %ld)",10*1024*1024,50*1024*1024);
			break;

		case 4:
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and (filesize >=%ld and filesize <= %ld)",50*1024*1024,100*1024*1024);
			break;

		case 5:
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and filesize >=%ld",100*1024*1024);
			break;
		}
	}
	if(k!=end_time)
			snprintf(mysql+strlen(mysql),1024*10-strlen(mysql)-1,"%s union ",sql);
		else
			snprintf(mysql+strlen(mysql),1024*10-strlen(mysql)-1,"%s",sql);
	}
	recodeLogInfo("web log test data...","******594*******",mysql);

	pasDbCursor *psCur = NULL;
	
	
	int iNum=0;
	int iret = 0;
	char tempsql[1024]="";
	snprintf(tempsql,sizeof(tempsql)-1,"select count(*) from ((%s)as a)",mysql);
	//replaceStr(tempsql,"*","count(*)");
	//snprintf();
	recodeLogInfo("web log test data...","******604*******",tempsql);
	pasDbOneRecord(tempsql,0,UT_TYPE_ULONG,4,&llcount);
	memset(tempsql,0,sizeof(tempsql)-1);
	 //导出到文件
			   	FILE *fp;
			  	char caFilename[32];
			  	char caFile[128];
			  	char caPath[128];
			  	
			    if(strlen(caExport)>0){	
			    	//recodeLogInfo("web log test data...","**********537*******",caExport);
			  		time_t time_tmp;
						struct tm *tm_tmp;
						time(&time_tmp);
						tm_tmp = gmtime(&time_tmp);
					
						sprintf(caFilename,"nwoutfilelog_%04u%02u%02u.csv",tm_tmp->tm_year+1900,tm_tmp->tm_mon+1,tm_tmp->tm_mday);
			      sprintf(caPath,"%s/html","/home/ncmysql/nw");
			      sprintf(caFile,"%s/%s",caPath,caFilename);
			  		fp = fopen(caFile,"w");
			      if(fp == NULL) {
			      		 pasDbCloseCursor(psCur);
			       		printf("open failed!\n");
			        writeSysLog(psMsgHead,"06","Fail","创建外发日志文件失败");
			          return 0;
			      }
			      char dataStr[1024]="";
			      snprintf(dataStr,sizeof(dataStr),"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",ncLang("0172时间"),ncLang("0173操作"),ncLang("0174部门"),ncLang("0175计算机"),ncLang("0176文件大小"),ncLang("0177文件名"),ncLang("0178操作进程"),ncLang("0179IP地址"),ncLang("0180源文件"),ncLang("0181阻挡类型"));
			      SaveDataToFile(caFile,dataStr);
				  recodeLogInfo("web log test data...","************640************",caExport);
					}else{
							if(!utStrIsSpaces(sort)){
									snprintf(mysql+strlen(mysql),sizeof(mysql)-strlen(mysql)," order by %s %s",sort,dir);
							}else{
									snprintf(mysql+strlen(mysql),sizeof(mysql)-strlen(mysql)," order by stime desc");
							}
							snprintf(mysql+strlen(mysql),sizeof(mysql)-strlen(mysql)," limit %d,%d",atoi(start),atoi(limit));
					}
	
	recodeLogInfo("web log test data...","************643************",mysql);
    psCur = pasDbOpenSql(mysql, 0);
	if(psCur!=NULL){
		long long sid=0;
		unsigned int compid=0;
		unsigned int userid=0;
		char mac[24]="";
		unsigned int groupid=0;
		char devname[40]="";
		char udisp[40]="";
		unsigned int sip=0;
		char procname[80]="";
		int mark=0;
		char indexid[30]="";
		unsigned long int stime=0;
		char souPath[1024]="";
		char destPath[1024]="";
		char content[1024]="";
		int Blacktype=0;
		int Backtype=0;
		char BlocktypeStr[32]="";
		int filesize=0;
		char dataStr[1024]="";
		
		while(0==(iret=pasDbFetchInto(psCur,
			UT_TYPE_LONG8,			sizeof(sid),			&sid,
			UT_TYPE_ULONG,			sizeof(compid),		&compid,
			UT_TYPE_ULONG,			sizeof(userid),		&userid,
			UT_TYPE_STRING,			sizeof(mac)-1,		mac,
			UT_TYPE_ULONG,			sizeof(groupid),		&groupid,
			UT_TYPE_STRING,			sizeof(devname)-1,		devname,
			UT_TYPE_STRING,			sizeof(udisp)-1,		udisp,
			UT_TYPE_ULONG,			sizeof(sip),			&sip,
			UT_TYPE_STRING,			sizeof(procname)-1,		procname,
			UT_TYPE_ULONG,			sizeof(mark),			&mark,
			UT_TYPE_STRING,			sizeof(indexid),		indexid,
			UT_TYPE_ULONG,			sizeof(stime),		&stime,
			UT_TYPE_STRING,			sizeof(souPath)-1,		souPath,
			UT_TYPE_STRING,			sizeof(destPath)-1,		destPath,
			UT_TYPE_STRING,			sizeof(content)-1,		content,
			UT_TYPE_ULONG,			sizeof(Blacktype),		&Blacktype,
			UT_TYPE_ULONG,			sizeof(Backtype),		&Backtype,
			UT_TYPE_ULONG,			sizeof(filesize),		&filesize)) || 1405 == iret){
			iNum++;
			//printf("-------------iNum=%d--------------",iNum);
			char filename[256];
			memset(filename,0,sizeof(filename));
			int h=0;
			for(h=strlen(souPath)-1;h>0;h--){
				if(souPath[h]=='/')
					break;
			}
			int ss=0;
			for(h=h+1;h<strlen(souPath);h++){
				filename[ss]=souPath[h];
				ss++;
			}
			
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			
			utPltPutLoopVarF(psDbHead,"sid",iNum,"%llu",sid);
			utPltPutLoopVarF(psDbHead,"compid",iNum,"%u",compid);
			utPltPutLoopVarF(psDbHead,"userid",iNum,"%u",userid);
			utPltPutLoopVar(psDbHead,"mac",iNum,mac);

			// get group
			
			utPltPutLoopVar(psDbHead,"groupid",iNum,lanGetGroup(psShmHead,groupid));
			utPltPutLoopVar(psDbHead,"devname",iNum,devname);
			utPltPutLoopVar(psDbHead,"udisp",iNum,udisp);


			struct in_addr addr;
			addr.s_addr=htonl(sip);
			char webIp[128];
			memset(webIp,0,sizeof(webIp));
			snprintf(webIp,sizeof(webIp)-1,"%s",inet_ntoa(addr));
			//recodeLogInfo("web log test data...","************************",webIp);
			utPltPutLoopVar(psDbHead,"webIp",iNum,webIp);

			utPltPutLoopVar(psDbHead,"procname",iNum,procname);

			char data[128];
			memset(data,0,128);
			GetMarkData(mark,data);
			if(!data){
			    utPltPutLoopVar(psDbHead,"mark",iNum,"类型未定义");
			}else{
			    utPltPutLoopVar(psDbHead,"mark",iNum,(char *)utf8convert(psShmHead,data));
			 
			}
			
			utPltPutLoopVar(psDbHead,"indexid",iNum,indexid);

			char mywebtime[128];
			memset(mywebtime,0,sizeof(mywebtime));
			local_time = localtime(&stime);
			strftime(mywebtime,sizeof(mywebtime),"%H:%M:%S %Y-%m-%d",local_time);
			utPltPutLoopVar(psDbHead,"mywebtime",iNum,mywebtime);

			utPltPutLoopVar(psDbHead,"souPath",iNum,souPath);
			

			if(strlen(BLOCKDATA[Backtype])>0)
				utPltPutLoopVar(psDbHead,"destPath",iNum,BLOCKDATA[Backtype]);
			else
				utPltPutLoopVar(psDbHead,"destPath",iNum," ");
			

			//get file name
			utPltPutLoopVar(psDbHead,"content",iNum,content);
			utPltPutLoopVar(psDbHead,"filename",iNum,filename);
			getBlocktypeStr(Blacktype,BlocktypeStr);
			utPltPutLoopVarF(psDbHead,"Blacktype",iNum,BlocktypeStr);
			utPltPutLoopVarF(psDbHead,"Backtype",iNum,"%u",Backtype);
			char filesizeInfo[32]="";
			if(filesize>1024*1024*1024){
				snprintf(filesizeInfo,sizeof(filesizeInfo)-1,"%.2f G",(float)filesize/1024/1024/1024);
			}else if(filesize>1024*1024){
				snprintf(filesizeInfo,sizeof(filesizeInfo)-1,"%.2f M",(float)filesize/1024/1024);
			}else if(filesize>102.4){
				snprintf(filesizeInfo,sizeof(filesizeInfo)-1,"%.2f K",(float)filesize/1024);
			}else{
				snprintf(filesizeInfo,sizeof(filesizeInfo)-1,"%u B",filesize);
			} 
			
			utPltPutLoopVar(psDbHead,"filesize",iNum,filesizeInfo);
		if(strlen(caExport)>0){
			memset(dataStr,0,sizeof(dataStr));
			snprintf(dataStr,sizeof(dataStr),"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",mywebtime,(char *)utf8convert(psShmHead,data),lanGetGroup(psShmHead,groupid),devname,filesizeInfo,filename,procname,webIp,souPath,BlocktypeStr);
			SaveDataToFile(caFile,dataStr);
			}
			sid=0;
			compid=0;
			userid=0;
			memset(mac,0,sizeof(mac));
			groupid=0;
			memset(devname,0,sizeof(devname));
			
			sip=0;
			memset(procname,0,sizeof(procname));
			mark=0;
			memset(indexid,0,sizeof(indexid));
			stime=0;
			memset(souPath,0,sizeof(souPath));
			memset(destPath,0,sizeof(destPath));
			memset(content,0,sizeof(content));
			Blacktype=0;
			Backtype=0;
			memset(BlocktypeStr,0,sizeof(BlocktypeStr));
			filesize=0;
		}
		
		
		//memset(mysql,0,sizeof(mysql));
		pasDbCloseCursor(psCur);
	}
	if(strlen(caExport)>0){
		 		printf("download!\n");
		 		fclose(fp);   
	        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
	        remove(caFile);
	        utPltFreeDb(psDbHead);
	        writeSysLog(psMsgHead,"06","Success","导出文件日志");
	        return 0;
		 }
	//fclose(fp);
	
	utPltPutVarF(psDbHead,"TotRec","%d",llcount);
	//utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/outfile/lan_outfile_list.htm");
	free(mysql);
	return 0;
}
///*************************************************************************************************************
//***************************************************************************************************************

//timebuf 2014-12-23 23:02:03
int transDataTimeToInt1(char* timebuf)
{
	int month=0;
	int year=0;
	int day=0;
	int hour=0;
	int min=0;
	int second=0;
	char tmpbuf[128];
	memset(tmpbuf,0,sizeof(tmpbuf));
	snprintf(tmpbuf,sizeof(tmpbuf)-1,"%s",timebuf);
	char* pos=strchr(tmpbuf,'-');
	while(pos!=NULL){
		(*pos)=' ';
		pos=strchr(tmpbuf,'-');
	}
	pos=strchr(tmpbuf,':');
	if(pos==NULL){
		snprintf(tmpbuf+strlen(tmpbuf),sizeof(tmpbuf)-strlen(tmpbuf)-1," 00:00:00");
	}
	while(pos!=NULL){
		(*pos)=' ';
		pos=strchr(tmpbuf,':');
	  }
	sscanf(tmpbuf,"%d%d%d%d%d%d",&year,&month,&day,&hour,&min,&second);

	struct tm when;
		memset(&when,0,sizeof(when));
				when.tm_year = year-1900;
				when.tm_mday = day;
				when.tm_mon=month-1;
				when.tm_hour=hour;
				when.tm_min=min;
				when.tm_sec=second;
	return  mktime(&when);
}

//timebuf 2014/12/23 23/02/03
int transDataTimeToInt2(char* timebuf)
{
	int month=0;
	int year=0;
	int day=0;
	int hour=0;
	int min=0;
	int second=0;
	char tmpbuf[128];
	memset(tmpbuf,0,sizeof(tmpbuf));
	snprintf(tmpbuf,sizeof(tmpbuf)-1,"%s",timebuf);
	char* pos=strchr(tmpbuf,'/');
	if(pos==NULL){
		//snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and taskid like '%%%s%%' ",taskid);
		snprintf(tmpbuf+strlen(tmpbuf),sizeof(tmpbuf)-strlen(tmpbuf)-1," 00/00/00");
	}
	while(pos!=NULL){
		(*pos)=' ';
		pos=strchr(tmpbuf,'/');
	}
	sscanf(tmpbuf,"%d%d%d%d%d%d",&year,&month,&day,&hour,&min,&second);

	struct tm when;
		memset(&when,0,sizeof(when));
				when.tm_year = year-1900;
				when.tm_mday = day;
				when.tm_mon=month-1;
				when.tm_hour=hour;
				when.tm_min=min;
				when.tm_sec=second;
	return  mktime(&when);
	
}

//outtimebuf 23:02:03 2014-12-23
void transDataTimeToStr1(int timeval,char* outtimebuf)
{
	//
	struct tm *local_time;
	time_t mytime=(time_t)timeval;
	local_time=localtime(&mytime);
	char timeBuf[64]="";
	strftime(timeBuf,sizeof(timeBuf)-1,"%Y-%m-%d %H:%M:%S",local_time);
	//sprintf((timeBuf,sizeof(timeBuf)-1,"%d-%m-%d %H:%M:%S",local_time);

	snprintf(outtimebuf,sizeof(timeBuf)-1,"%s",timeBuf);
}

//outtimebuf 2014/12/23 23/02/03
void transDataTimeToStr2(int timeval,char* outtimebuf)
{
	struct tm *local_time;
	time_t mytime=(time_t)timeval;
	local_time=localtime(&mytime);
	char timeBuf[64]="";
	strftime(timeBuf,sizeof(timeBuf)-1,"%Y/%m/%d %H/%M/%S",local_time);
	snprintf(outtimebuf,sizeof(timeBuf)-1,"%s",timeBuf);
}






//************************************************************************************************************************
//************************************************************************************************************************
//文件外发结果查询响应函数
int GetTaskidFromFile(){
	FILE *fp=fopen(TASKID_RESULT,"r");
	if(fp){
		char data[128]="";
		fgets(data,sizeof(data),fp);
		if(strlen(data)){
			recodeLogInfo("web log test data...","23424324324324324324",data);
			char *pos=strchr(data,'=');
			char temp[12]="";
			memcpy(temp,pos+1,strlen(pos+1));
			fclose(fp);
			return atoi(temp);
		}
		fclose(fp);
	}
	return 0;
}
int lan_log_getFileLogResoult(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	utMsgPrintMsg(psMsgHead);
	readData();
	utPltDbHead *psDbHead = utPltInitDb();
	long iReturn=0;
	
	char _keyword[33]="";
	char keyword[33]="";
	char _filename[72]="";
	char filename[72]="";
	char _uid[17]="";
	char _taskid[32]="";
	unsigned long int taskid=0;
	char _threadname[32]="";
	char threadname[32]="";
	char _fileoperation[32]="";
	char fileoperation[32]="";
	char _devname[32]="";
	char devname[32]="";
	char ip1[24]="";
	char sip1[24]="";
	char department1[16]="";
	char procname1[64]="";
	char mac1[72]="";
	char compid1[32]="";
	char stoptype[32]="";
	char compid[32]="";
	char mark1[32]="";
	char sdate[11] = "";
	char edate[11] = "";
	char stime1[11] = "";
	char etime1[11] = "";
	char time_flag[2] = "";
	char start[17] = "";
 	char limit[17] = "";
 	char sort[16] = "";
 	char dir[16] = "";
	char blocktype[10]="";
 	char caExport[12];
	long filesize=0;
	char filesizelimit[24]="";
 	int llcount=0;
 	memset(caExport,0,sizeof(caExport));	
	utMsgGetSomeNVar(psMsgHead, 19,
					 "fileoperation",		UT_TYPE_STRING,  			sizeof(_fileoperation)-1, _fileoperation,
					 "devname",				UT_TYPE_STRING,  			sizeof(_devname)-1,  	 _devname,
					 "stoptype",			UT_TYPE_STRING,  			sizeof(stoptype)-1,  	 stoptype,
					 "taskid",				UT_TYPE_STRING,  			sizeof(_taskid)-1,  	    _taskid,
					 "filename",		    UT_TYPE_STRING,  			sizeof(_filename)-1,  	 _filename,
					 "threadname",			UT_TYPE_STRING,  			sizeof(_threadname)-1,  _threadname,
   				     "keyword",				UT_TYPE_STRING,  			sizeof(_keyword)-1,  	 _keyword,
   				     "ip1",       			UT_TYPE_STRING,  			sizeof(ip1)-1,     		ip1,
   				     "groupid",  		    UT_TYPE_STRING,  			sizeof(department1)-1, 	department1,
   				     "compid",				UT_TYPE_STRING, 			sizeof(compid1)-1, 		compid1,
					 "mac1",				UT_TYPE_STRING, 			sizeof(mac1)-1, 		mac1, 
   				     "sdate", 	   			UT_TYPE_STRING,  			sizeof(sdate)-1,       	sdate,
   				     "edate", 	   			UT_TYPE_STRING,  			sizeof(edate)-1,       	edate,
   				     "export2",      		UT_TYPE_STRING,  			sizeof(caExport)-1,      caExport,
   				     "time_flag",  			UT_TYPE_STRING,  			sizeof(time_flag)-1,   	time_flag,
   				     "start",      			UT_TYPE_STRING,  			sizeof(start)-1,       	start,
   				     "limit",      			UT_TYPE_STRING,  			sizeof(limit)-1,       	limit,
					 "filesizelimit",      	UT_TYPE_STRING,  			sizeof(filesizelimit)-1,	filesizelimit,
   				     "sort",       			UT_TYPE_STRING,  			sizeof(sort)-1,        	sort);
   char mesg[512]="";
  filesize=atoi(filesizelimit);
  memset(mesg,0,sizeof(mesg));
  snprintf(devname,sizeof(devname)-1,"%s",utf8convert(psShmHead,_devname));
  snprintf(fileoperation,sizeof(fileoperation)-1,"%s",utf8convert(psShmHead,_fileoperation));
  taskid=atoi(_taskid);
  snprintf(filename,sizeof(filename)-1,"%s",utf8convert(psShmHead,_filename));
  snprintf(keyword,sizeof(keyword)-1,"%s",utf8convert(psShmHead,_keyword));
  snprintf(threadname,sizeof(threadname)-1,"%s",utf8convert(psShmHead,_threadname));
	snprintf(mark1,sizeof(mark1)-1,"%s",fileoperation);
	int start_time=-1;
	int end_time=-1;
	int flag_type=-1;
	int time_type=atoi(time_flag);
	int newid=GetTaskidFromFile();
	if(newid>0){
		remove(TASKID_RESULT);
		taskid=newid;
		time_type=4;
		snprintf(mesg,sizeof(mesg),"time_type=%d taskid=%d",time_type,taskid);
	}

	char data[24];
	char sql[1024]="";
	memset(data,0,sizeof(data));
	time_t temp;
	struct tm *local_time = NULL;
	int count=0;
	char filenames[128];
	memset(filenames,0,sizeof(filenames));
	char ip[128];
	memset(ip,0,sizeof(ip));
	if(strlen(sdate)<=0||strlen(edate)<=0){
		time(&temp);
		local_time = localtime(&temp);
		  end_time = temp;
		
		switch(time_type)
		{
			case 1:
				flag_type=1;
				temp=temp-local_time->tm_hour*3600-local_time->tm_min*60-local_time->tm_sec;
				break;
			case 2:
				flag_type=1;
				temp=temp-3*24*3600;
				break;
			case 3:
				flag_type=1;
				temp=temp-7*24*3600;
				break;
			case 4:
				flag_type=2;
				temp=temp-30*24*3600;
				break;
			case 5:
				flag_type=2;
				temp=temp-90*24*3600;
				break;
			case 6:
				flag_type=2;
				temp=temp-180*24*3600;
				break;
		} 
		start_time=(int)temp;

		memset(mesg,0,sizeof(mesg));
		//snprintf(mesg,sizeof(mesg),"%d%d",start_time,end_time);
		snprintf(mesg,sizeof(mesg)-1,"start_time:%d\tend_time:%d\tsdate:%s\tedate:%s\ttime_flag:%s",start_time,end_time,sdate,edate,time_flag);

	}else{
		flag_type = 0;
		start_time=transDataTimeToInt2(sdate);
		end_time=transDataTimeToInt2(edate)+24*60*60-1;
		//strftime(edate, sizeof(data), "%Y%m%d",tmp_time);
		memset(mesg,0,sizeof(mesg));
		//snprintf(mesg,sizeof(mesg),"%d%d",start_time,end_time);
		snprintf(mesg,sizeof(mesg)-1,"start_time:%d\tend_time:%d\tsdate:%s\tedate:%s\t",start_time,end_time,sdate,edate);
	}
		memset(sql,0,sizeof(sql));
		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1,"select sid,compid,userid,mac,groupid,devname,udisp,sip,procname,mark,Blacktype,Backuptype,filesize,indexid,stime,souPath,destPath,content,taskid,(select taskname from nwoutfiletask where taskid=a.taskid)as taskname from nwoutfilelog as a where stime>%u and stime<%u",start_time,end_time);
		long lCount=0;
		int ret=pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);

		
		//add filter conditions here
		if(strlen(fileoperation)>0){
			unsigned int marknum=atoi(mark1);
			if(marknum>0)
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and mark=%u",marknum);
		}
		if(strlen(threadname)>0){
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and procname='%%%s%%' ",threadname);
		}
		if(strlen(department1)>0&&(strcmp("0",department1))){
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and groupid=%d ",atoi(department1));
		}
		
		if(strlen(ip1)>0){
			
			unsigned int ip_i=0,v=0,z=0;
			bool ip_flag=false;
			for(ip_i=0;ip_i<strlen(ip1);ip_i++){
				if(ip1[ip_i]=='/'){
					ip_flag=true;
					continue;
				}
				if(!ip_flag){
					filenames[v]=ip1[ip_i];
					v++;
				}else{
					ip[z]=ip1[ip_i];
					z++;
				}
			}
			if(strlen(filenames)>0)
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and inet_ntoa(sid) like '%%%s%%' or souPath like '%%%s%%' ",filenames,filenames);
			if(strlen(ip)>0)
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and inet_ntoa(sid) like '%%%s%%' or souPath like '%%%s%%' ",ip,ip);
		}
	if(taskid>0){
		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and taskid =%u",taskid);
	}
	if(keyword!=NULL&&strlen(keyword)>0){
		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and inet_ntoa(Ips) like '%%%s%%' or devname like '%%%s%%'",keyword,keyword);
	}
	//printf sql
	if(filesize>0){
		switch(filesize){
		case 1:
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and filesize<=%ld",1024*1024);
			break;
		case 2:
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and (filesize >=%ld and filesize <= %ld)",1024*1024,10*1024*1024);
			break;

		case 3:
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and (filesize >=%ld and filesize <= %ld)",10*1024*1024,50*1024*1024);
			break;

		case 4:
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and (filesize >=%ld and filesize <= %ld)",50*1024*1024,100*1024*1024);
			break;

		case 5:
			snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and filesize >=%ld",100*1024*1024);
			break;
		}
	}
	pasDbCursor *psCur = NULL;
	
	int iNum=0;
	int iret = 0;
	char tempsql[1024]="";
	snprintf(tempsql,sizeof(tempsql)-1,"select count(*) from ((%s) as a)",sql);
	pasDbOneRecord(tempsql,0,UT_TYPE_ULONG,4,&llcount);
	memset(tempsql,0,sizeof(tempsql)-1);
	//导出到文件
				   	FILE *fp;
				  	char caFilename[32];
				  	char caFile[128];
				  	char caPath[128];
				  	
				    if(strlen(caExport)>0){	
				  		time_t time_tmp;
							struct tm *tm_tmp;
							time(&time_tmp);
							tm_tmp = gmtime(&time_tmp);
						
							sprintf(caFilename,"nwoutfileResult_%04u%02u%02u.csv",tm_tmp->tm_year+1900,tm_tmp->tm_mon+1,tm_tmp->tm_mday);
				      sprintf(caPath,"%s/html","/home/ncmysql/nw");
				      sprintf(caFile,"%s/%s",caPath,caFilename);
				  		fp = fopen(caFile,"w");
				      if(fp == NULL) {
				      		 pasDbCloseCursor(psCur);
				       		printf("open failed!\n");
				        writeSysLog(psMsgHead,"06","Fail","创建外发日志文件失败");
				          return 0;
				      }
				      char dataStr[1024]="";
				      snprintf(dataStr,sizeof(dataStr),"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",ncLang("0182"),ncLang("0183"),ncLang("0184"),ncLang("0185"),ncLang("0186"),ncLang("0187"),ncLang("0188"),ncLang("0189"),ncLang("0190"),ncLang("0191"),ncLang("0192"),ncLang("0193"));
				      recodeLogInfo("web log test data...","************1110************",dataStr);
				      SaveDataToFile(caFile,dataStr);
					  recodeLogInfo("web log test data...","************1112************",caFilename);
						}else{
								if(!utStrIsSpaces(sort)){
										snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," order by %s %s",sort,dir);
								}else{
										snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," order by stime desc");
								}
								snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," limit %d,%d",atoi(start),atoi(limit));
						}

	
	recodeLogInfo("web log test data...","************1243************",sql);
	
	psCur = pasDbOpenSql(sql, 0);

	if(psCur!=NULL){
		long long sid=0;
		unsigned int compid=0;
		unsigned int userid=0;
		char mac[24]="";
		unsigned int groupid=0;
		char devname[40]="";
		char udisp[40]="";
		unsigned int sip=0;
		char procname[80]="";
		int mark=0;
		char indexid[30]="";
		unsigned long int stime=0;
		char souPath[1024]="";
		char destPath[1024]="";
		char content[1024]="";
		int Blacktype=0;
		int filesize=0;
		int Backtype=0;
		int taskid=0;
		char taskname[32]="";
		char outTimeStr[32]="";
		char dataStr[1024]="";
		char sql[256]="";
		while(0==(iret=pasDbFetchInto(psCur,
			UT_TYPE_LONG8,			sizeof(sid),			&sid,
			UT_TYPE_ULONG,			sizeof(compid),			&compid,
			UT_TYPE_ULONG,			sizeof(userid),			&userid,
			UT_TYPE_STRING,			sizeof(mac)-1,			mac,
			UT_TYPE_ULONG,			sizeof(groupid),		&groupid,
			UT_TYPE_STRING,			sizeof(devname)-1,		devname,
			UT_TYPE_STRING,			sizeof(udisp)-1,		udisp,
			UT_TYPE_ULONG,			sizeof(sip),			&sip,
			UT_TYPE_STRING,			sizeof(procname)-1,		procname,
			UT_TYPE_ULONG,			sizeof(mark),			&mark,
			UT_TYPE_ULONG,			sizeof(Blacktype),		&Blacktype,
			UT_TYPE_ULONG,			sizeof(Backtype),		&Backtype,
			UT_TYPE_ULONG,			sizeof(filesize),		&filesize,
			UT_TYPE_STRING,			sizeof(indexid),		indexid,
			UT_TYPE_ULONG,			sizeof(stime),			&stime,
			UT_TYPE_STRING,			sizeof(souPath)-1,		souPath,
			UT_TYPE_STRING,			sizeof(destPath)-1,		destPath,
			UT_TYPE_STRING,			sizeof(content)-1,		content,
			UT_TYPE_ULONG,			sizeof(taskid),			&taskid,
			UT_TYPE_STRING,			sizeof(taskname)-1,		taskname
			)) || 1405 == iret){

			iNum++;
			//recodeLogInfo("web log test data...","******LLC**souPath*****",souPath);
			char filename[256];
			memset(filename,0,sizeof(filename));
			int h=0;
			for(h=strlen(souPath)-1;h>0;h--){
				if(souPath[h]=='/')
					break;
			}
			int ss=0;
			for(h=h+1;h<strlen(souPath);h++){
				filename[ss]=souPath[h];
				ss++;
			}
			
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"sid",iNum,"%llu",sid);
			char filesizeInfo[32]="";
			if(filesize>1024*1024*1024){
				snprintf(filesizeInfo,sizeof(filesizeInfo)-1,"%.2f G",(float)filesize/1024/1024/1024);
			}else if(filesize>1024*1024){
				snprintf(filesizeInfo,sizeof(filesizeInfo)-1,"%.2f M",(float)filesize/1024/1024);
			}else if(filesize>102.4){
				snprintf(filesizeInfo,sizeof(filesizeInfo)-1,"%.2f K",(float)filesize/1024);
			}else{
				snprintf(filesizeInfo,sizeof(filesizeInfo)-1,"%u B",filesize);
			} 
			utPltPutLoopVar(psDbHead,"filesize",iNum,filesizeInfo);
			//utPltPutLoopVar(psDbHead,"filesize",iNum,filesizeInfo);
			utPltPutLoopVarF(psDbHead,"compid",iNum,"%u",compid);
			utPltPutLoopVarF(psDbHead,"userid",iNum,"%u",userid);
			utPltPutLoopVar(psDbHead,"mac",iNum,mac);

			// get group
			
			utPltPutLoopVar(psDbHead,"groupid",iNum,lanGetGroup(psShmHead,groupid));
			utPltPutLoopVar(psDbHead,"devname",iNum,devname);
			
			utPltPutLoopVar(psDbHead,"udisp",iNum,udisp);

			
			struct in_addr addr;
			addr.s_addr=htonl(sip);
			char webIp[128];
			memset(webIp,0,sizeof(webIp));
			snprintf(webIp,sizeof(webIp)-1,"%s",inet_ntoa(addr));
			//recodeLogInfo("web log test data...","***********webip*************",webIp);
			utPltPutLoopVar(psDbHead,"webIp",iNum,webIp);
			

			utPltPutLoopVar(psDbHead,"procname",iNum,procname);
			char markStr[32]="";
			char data[128];
			memset(data,0,128);
			GetMarkData(mark,data);
			if(!data){
			    utPltPutLoopVar(psDbHead,"mark",iNum,"  ");
				snprintf(markStr,sizeof(markStr)-1,"  ");
			}else{
			    utPltPutLoopVar(psDbHead,"mark",iNum,(char *)utf8convert(psShmHead,data));
				snprintf(markStr,sizeof(markStr)-1,"%s",(char *)utf8convert(psShmHead,data));
			}
			
			utPltPutLoopVar(psDbHead,"indexid",iNum,indexid);

			char mywebtime[128];
			memset(mywebtime,0,sizeof(mywebtime));
			local_time = localtime(&stime);
			strftime(mywebtime,sizeof(mywebtime),"%H:%M:%S %Y-%m-%d",local_time);
			utPltPutLoopVar(psDbHead,"mywebtime",iNum,mywebtime);
			char data1[32]="";
			snprintf(data,32,"%u",stime);
			//recodeLogInfo("web log test data...","***********data1*************",data1);
			utPltPutLoopVar(psDbHead,"souPath",iNum,souPath);
			

			if(strlen(BLOCKDATA[Backtype])>0)
				utPltPutLoopVar(psDbHead,"destPath",iNum,BLOCKDATA[Backtype]);
			else
				utPltPutLoopVar(psDbHead,"destPath",iNum," ");
			

			//get file name
			utPltPutLoopVar(psDbHead,"content",iNum,content);
			utPltPutLoopVar(psDbHead,"filename",iNum,filename);
			utPltPutLoopVarF(psDbHead,"Blacktype",iNum,"%u",Blacktype);
			utPltPutLoopVarF(psDbHead,"Backtype",iNum,"%u",Backtype);
			utPltPutLoopVarF(psDbHead,"taskid",iNum,"%u",taskid);
			utPltPutLoopVar(psDbHead,"taskname",iNum,taskname);
			if (strlen(caExport) > 0) {
				memset(dataStr, 0, sizeof(dataStr));
				snprintf(dataStr, sizeof(dataStr),"%s,%s,%s,%u,%s,%s,%s,%s,%s,%s,%s,%u",mywebtime,devname,lanGetGroup(psShmHead, groupid), userid, webIp,procname, markStr, Backtype, filename, filesizeInfo,souPath,taskid);
				SaveDataToFile(caFile, dataStr);
			}
			memset(markStr,0,sizeof(markStr));
		}
		pasDbCloseCursor(psCur);
	}
		if (strlen(caExport) > 0) {
			printf("download!\n");
			fclose(fp);
			utPltFileDownload(iFd, "application/text", caPath, caFilename,caFilename);
			remove(caFile);
			utPltFreeDb(psDbHead);
			writeSysLog(psMsgHead, "06", "Success", "导出任务结果日志成功");
			return 0;
		}
	utPltPutVarF(psDbHead,"TotRec","%lu",llcount);
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/outfile/lan_outfile_result.htm");
  
	return 0;
}
int nwOutfilelog_Action(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();
	char _sid[64]="";
	unsigned long long int sid=0;
	char action[32]="";
	int result=0;
	utMsgGetSomeNVar(psMsgHead, 2,
					 "action",		     UT_TYPE_STRING,  			sizeof(action)-1, action,
					 "sid",				UT_TYPE_STRING,				sizeof(_sid)-1,    _sid);
	char data[1024]="";
	snprintf(data,sizeof(data)-1,"action=%s  sid=%s",action,_sid);
    sid=strtoull(_sid,NULL,10);
	char sql[1024]="";
	if(!strcmp(action,"removeone")){
		snprintf(sql,sizeof(sql)-1,"delete from nwoutfilelog where sid=%llu",sid);
		result=pasDbExecSqlF(sql);
		utPltPutVar(psDbHead,"buff","{success:true,mesg:\'操作成功!\'}");
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	}else if(!strcmp(action,"removeall")){
		snprintf(sql,sizeof(sql)-1,"delete from nwoutfilelog");
		result=pasDbExecSqlF(sql);
		utPltPutVar(psDbHead,"buff","{success:true,mesg:\'操作成功!\'}");
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	}else{
		utPltPutVar(psDbHead,"buff","{success:true,mesg:\'操作失败!\'}");
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	}

	return 0;

}
int lan_get_result_task(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	char taskid[12]="";	
	utMsgGetSomeNVar(psMsgHead, 1,"_mytaskid",UT_TYPE_STRING,sizeof(taskid)-1,taskid);
	if(strlen(taskid)>0){
		remove(TASKID_RESULT);
		FILE *fp=fopen(TASKID_RESULT,"w");
		if(fp){
			fprintf(fp,"taskid=%s",taskid);
			fclose(fp);
		}
	}
	return 0;
}
int ncInitNwWebPHTestFunc(utShmHead *psShmHead)
{
	pasSetTcpFunName("nwOutfilelog_Action",nwOutfilelog_Action,  NULL,0,0);
	pasSetTcpFunName("lan_log_getFileLogTest",lan_log_getFileLogTest,  NULL,0,0);
	pasSetTcpFunName("ncInitNwWebPHTestFunc",ncInitNwWebPHTestFunc,  NULL,0,0);
	pasSetTcpFunName("lan_log_getFileLogResoult",lan_log_getFileLogResoult,  NULL,0,0);
	pasSetTcpFunName("lan_get_result_task",lan_get_result_task,  NULL,0,0);
	return 0;
}