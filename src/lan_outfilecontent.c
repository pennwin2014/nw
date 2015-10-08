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


//文件外发内容获取策略收集文件路径
#define DEBUG
#define OUTFILE_CONTENT_LAW_PATH	"/home/ncmysql/nw/OutFileContentLaw/"
#define OUTFILE_CONTENT_PATH	"/home/ncmysql/nw/html/v8/outfile/backfile/"
typedef struct{
	unsigned long int compid;
	char md5[128];
}MYDATA;
typedef struct _buf_arr{
	struct _buf_arr *pNext;
	union{
		char buffer[1024];
		MYDATA item;
	}info;
}BUFARR;
BUFARR *MyAddBufItem(BUFARR *pbufroot,BUFARR *pbufitem){
	BUFARR *pitem=(BUFARR*) malloc(sizeof(BUFARR));
	memset(pitem,0,sizeof(BUFARR));
	memcpy(pitem,pbufitem,sizeof(BUFARR));
	if(pbufroot==NULL)
		return pitem;
	pitem->pNext=pbufroot;
	return pitem;
}

static int8 pasGetLongTimeId()
{
    int8 llTime;
    static int lSid=0;
    struct timeval tp;
    gettimeofday(&tp,NULL);

    if(lSid >= 100) lSid = 0;
    llTime = ((long long)tp.tv_sec * 1000000L + (long long)(tp.tv_usec + getpid())) * 100 + lSid;
    lSid ++;
    return llTime;
}

void MyClearBufItem(BUFARR *pbufroot){
	BUFARR *pitem=NULL;
	while(pbufroot!=NULL){
		pitem=pbufroot->pNext;
		free(pbufroot);
		pbufroot=pitem;
	}
}
void WriteLogToFile(char *mesg){
	char path[128]="";
	snprintf(path,sizeof(path)-1,"%slog.txt",OUTFILE_CONTENT_LAW_PATH);
	FILE *fp=fopen(path,"a+");
	if(fp){
		fprintf(fp,"%s\r\n",mesg);
		fclose(fp);
	}
}
void AddLogToFile(char *title,char *funcName,char *err){
	char mesg[1024*4]="";
	snprintf(mesg,sizeof(mesg)-1,"%s\t%s\t%s",title,funcName,err);
#ifdef DEBUG
	WriteLogToFile(mesg);
#endif
}
//策略收集函数
bool writelaw(char *filemd5,unsigned long int computerid){
	char path[256]="";
	memset(path,0,sizeof(path));
	snprintf(path,sizeof(path)-1,"%s%lu",OUTFILE_CONTENT_LAW_PATH,computerid);
	if(!utFileIsExist(path)){
		char comd[512]="";
		snprintf(comd,sizeof(comd)-1,"mkdir -p %s",path);
		system(comd);
	}
	char newpath[512]="";
	snprintf(newpath,sizeof(newpath)-1,"%s/%lu.txt",path,computerid);
	FILE *fp;
	fp=fopen(newpath,"a+");
	if(fp){
		fprintf(fp,"%s\n",filemd5);
		fclose(fp);
		return true;
	}
	return false;
}

//*******************************************************************************************************
//*************************************外发文件内*********************************************
int lan_outfilecontent_insert(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
 
 
  
  
  //do{
   
     unsigned long long currentTime = pasGetLongTimeId();
     printf("currtime=%llu\n",currentTime);

    utMsgPrintMsg(psMsgHead);
  utPltDbHead *psDbHead = utPltInitDb();
 char mesg[1024]="";
   
  //}while(currentTime>1800000000000000000ll);
	char action[24]="";
	char filemd5[33]="";
	char _devname[72]="";
	char devname[72]="";
	char ip[72]="";
	char compid[72]="";
	char sql[1024]="";
	char sid[64]="";
	
	int result=-1;

	long lCount=0;
	utMsgGetSomeNVar(psMsgHead, 6,
					 "filemd5",				UT_TYPE_STRING,  			sizeof(filemd5)-1,  	 filemd5,
   				     "devname",				UT_TYPE_STRING,  			sizeof(_devname)-1,  	  _devname,
   				     "ip",       		    UT_TYPE_STRING,  			sizeof(ip)-1,			  ip,
   				     "action",  		    UT_TYPE_STRING,  		    sizeof(action)-1, 	      action,
					 "compid",  		    UT_TYPE_STRING,  		    sizeof(compid)-1, 	      compid,
					 "sid",  		        UT_TYPE_STRING,  		    sizeof(sid)-1, 	      sid);
	snprintf(devname,sizeof(devname)-1,"%s",utf8convert(psShmHead,_devname));
	snprintf(mesg,sizeof(mesg)-1,"filemd5=%s   devname=%s    ip=%s    action=%s    compid=%s sid=%s",filemd5,devname,ip,action,compid,sid);
	recodeLogInfo("web task test data...",".......................................................",mesg);  
	
   memset(sql,0,sizeof(sql));
   if(!strcmp(action,"insert")){
	   time_t local_time=0;
	   time(&local_time);
	   memset(mesg,0,sizeof(mesg));
		snprintf(sql,sizeof(sql)-1,"select count(*) from nwoutfilecontent where Filemd5='%s' and Computerid=%u",filemd5,strtoul(compid,NULL,10));
		pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
		if(lCount>0){
			snprintf(sql,sizeof(sql)-1,"update nwoutfilecontent set stime=%u where Filemd5='%s' and Computerid=%u",local_time,filemd5,strtoul(compid,NULL,10));
			result=pasDbExecSqlF(sql);
			utPltPutVar(psDbHead,"buff","{success:true,mesg:\'操作成功!\'}");
    	    utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
			
		}else{
			memset(sql,0,sizeof(sql));
			currentTime++;
			snprintf(mesg,sizeof(mesg)-1,"%llu",currentTime);
			snprintf(sql,sizeof(sql)-1,"insert into nwoutfilecontent(sid,Filemd5,Computerid,Ips,Devname,userer,stime)values(%llu,'%s',%u,%u,'%s','%s',%u)",currentTime,filemd5,strtoul(compid,NULL,10),ntohl(inet_addr(ip)),devname,devname,local_time);
			result=pasDbExecSqlF(sql);
			utPltPutVar(psDbHead,"buff","{success:true,mesg:\'操作成功!\'}");
    	    utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
		 }
       
   }else if(!strcmp(action,"removeone")){
		unsigned long long int mysid=strtoull(sid,NULL,10);
		memset(sql,0,sizeof(sql));
		snprintf(sql,sizeof(sql),"select Filename from nwoutfilecontent where sid=%llu",mysid);
		char path[512]="";
		pasDbOneRecord(sql,0,UT_TYPE_STRING,sizeof(path)-1,path);
		remove(path);
		memset(sql,0,sizeof(path));
		snprintf(sql,sizeof(sql)-1,"delete from nwoutfilecontent where sid=%llu",mysid);
		result=pasDbExecSqlF(sql);
		utPltPutVar(psDbHead,"buff","{success:true,mesg:\'操作成功!\'}");
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
   
   }else if(!strcmp(action,"removeall")){
		memset(sql,0,sizeof(sql));
		snprintf(sql,sizeof(sql),"%s","select Filename from nwoutfilecontent");
		
		pasDbCursor *psCur = NULL;
		psCur = pasDbOpenSql(sql, 0);
		if(psCur){
			int iret = 0;
			char mypath[512]="";
			while(0==(iret=pasDbFetchInto(psCur,UT_TYPE_STRING,			sizeof(mypath)-1,		mypath)) || 1405 == iret){
				if(strlen(mypath)>0)
					remove(mypath);
			}
			pasDbCloseCursor(psCur);

			memset(sql,0,sizeof(sql));
			snprintf(sql,sizeof(sql)-1,"delete from nwoutfilecontent");
			result=pasDbExecSqlF(sql);
			utPltPutVar(psDbHead,"buff","{success:true,mesg:\'操作成功!\'}");
			utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
		}
   }else if(!strcmp(action,"download")){
     //执行下载文件的操作
	unsigned long long psid=strtoull(sid,NULL,10);
	memset(sql,0,sizeof(sql));
	snprintf(sql,sizeof(sql),"select Filename from nwoutfilecontent where sid=%llu",psid);
	char path[512]="";
	pasDbOneRecord(sql,0,UT_TYPE_STRING,sizeof(path)-1,path);
	char myfilename[128]="";
	char *pos=strrchr(path,'/');
	if(pos){
		memcpy(myfilename,pos+1,strlen(pos+1));
	 	utPltFileDownload(iFd,"application/octet-stream",path,myfilename,myfilename);
	}	 
	utPltPutVar(psDbHead,"buff","{success:true,mesg:\'操作成功!\'}");
	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
   
   }else{
	utPltPutVar(psDbHead,"buff","{success:true,mesg:\'操作失败!\'}");
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
   }

  return 0;
}



int lan_RemoveSerLawFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	char mesg[1024]="";
	unsigned long int compid=0;
	int iRet=utMsgGetSomeNVar(psMsgHead,1,"compid",UT_TYPE_ULONG,4,&compid);
	if(compid<=0){
		AddLogToFile("lan_RemoveSerLawFile params err...","","");
		pasTcpResponse(iFd,psMsgHead,NULL,1,"state",   UT_TYPE_LONG,-1);
		return -1;
	}
	char path[512]="";
	snprintf(path,sizeof(path)-1,"%s%lu/%lu.txt",OUTFILE_CONTENT_LAW_PATH,compid,compid);
	remove(path);
	nwCompIp *psCompIp;
	psCompIp=(nwCompIp*)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
	if(psCompIp) {
		psCompIp->lCtrl&=~0X00100000;
		pasTcpResponse(iFd,psMsgHead,NULL,1,"state",   UT_TYPE_LONG,1);
		return 0;
	}
	pasTcpResponse(iFd,psMsgHead,NULL,1,"state",UT_TYPE_LONG,-1);
	return -1;
}
void Lan_GetFileContent_Law(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	char mesg[512]="";
	char sql[512]="";
	snprintf(sql,sizeof(sql)-1,"select Filemd5,Computerid from nwoutfilecontent where Status=%d",0);
	utPltDbHead *psDbHead = utPltInitDb();
	pasDbCursor *psCur = NULL;
	psCur = pasDbOpenSql(sql, 0);
	int iNum=0;
	int iret = 0;
	BUFARR* prootbuf=NULL;

	AddLogToFile("Lan_GetFileContent_Law","into",sql);	
	if(psCur!=NULL){
		char filemd5[48]="";
		unsigned long int computerid=0;
		BUFARR mydata;
		while(0==(iret=pasDbFetchInto(psCur,
			UT_TYPE_STRING,			sizeof(filemd5)-1,		filemd5,
			UT_TYPE_ULONG,			sizeof(computerid),		&computerid
			)) || 1405 == iret){
			
			snprintf(mesg,sizeof(mesg)-1,"computerid=%lu  filemd5=%s",computerid,filemd5);
			AddLogToFile("Lan_GetFileContent_Law",mesg,"");
			if(strlen(filemd5)>0&&computerid>0){
				memset(&mydata,0,sizeof(mydata));
				mydata.info.item.compid=computerid;
				snprintf(mydata.info.item.md5,sizeof(filemd5),"%s",filemd5);
				prootbuf=MyAddBufItem(prootbuf,&mydata);
			}
		}
		pasDbCloseCursor(psCur);
	}
	BUFARR* pitem=prootbuf;
	while(pitem!=NULL){
		if(writelaw(pitem->info.item.md5,pitem->info.item.compid)){
			nwCompIp *psCompIp;
			psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&(pitem->info.item.compid));
			if(psCompIp) {
				psCompIp->lCtrl|=0X00100000;
			}
			//修改数据库里的字段
			memset(sql,0,sizeof(sql));
			snprintf(sql,sizeof(sql)-1,"update nwoutfilecontent set Status=1 where Computerid=%lu and Filemd5='%s'",pitem->info.item.compid,pitem->info.item.md5);
			if(pasDbExecSql(sql, 0)){
				AddLogToFile("Lan_GetFileContent_Law","sql err!",sql);       					
			}		
		}
		pitem=pitem->pNext;
	}
	MyClearBufItem(prootbuf);
	prootbuf=NULL;
}
int lan_GetOutFileSaveSize(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	char mesg[1024]="";
	unsigned long int compid=0;
	char md5[128]="";

	int iRet=utMsgGetSomeNVar(psMsgHead,2,"compid",UT_TYPE_ULONG,4,&compid,"MD5",UT_TYPE_STRING,sizeof(md5)-1,md5);

	if(compid<=0||strcmp("",md5)==0){
		AddLogToFile("lan_GetOutFileSaveSize parames error","",md5);
		pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "state",   UT_TYPE_LONG,-1);
		return -1;
	}
	char sql[256]="";
	snprintf(sql,sizeof(sql)-1,"select savesize from nwoutfilecontent where Computerid=%lu and Filemd5='%s'",compid,md5);
	int savesize=0;
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) 
	{ 
        AddLogToFile("nwoutfilecontent error..." ,"lan_GetOutFileSaveSize error..." , sql);
		pasTcpResponse(iFd,psMsgHead,NULL,1,"state",UT_TYPE_LONG,-1);
		return -1;
    }
	iRet = pasDbFetchInto(psCur,UT_TYPE_LONG, 4, &savesize);
	pasDbCloseCursor(psCur);

	if((iRet!=0)  ||savesize<0){
		AddLogToFile("lan_GetOutFileSaveSize error...",sql,"iRet!=0)  ||savesize< 0");
		pasTcpResponse(iFd,psMsgHead,NULL,1,"state",   UT_TYPE_LONG,-1);
		return -1;
	}
	pasTcpResponse(iFd,psMsgHead,NULL,1,"state",   UT_TYPE_LONG,savesize);
	return 0;
}
int Lan_OutFileLawDownLoad(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	char mesg[512]="";
	unsigned long int comPid=0;
	unsigned long int type=2;

	int iRet=utMsgGetSomeNVar(psMsgHead,2,"compid",UT_TYPE_ULONG,4,&comPid,
		"filetype",UT_TYPE_ULONG,4,&type);

	if(comPid<=0){
		AddLogToFile("Lan_OutFileLawDownLoad error","","compid <=0");
		return -1;
	}
	char filePath[512]="";
	snprintf(filePath,sizeof(filePath)-1,"%s%lu/%lu.txt",OUTFILE_CONTENT_LAW_PATH,comPid,comPid);
	if(type==0){
		int iSize=utFileSize(filePath);
		if(iSize <= 0) {
			AddLogToFile("Lan_OutFileLawDownLoad error","","iSize <= 0");
			return -1;
		}
		FILE *fp=fopen(filePath,"rb");
		if(fp==NULL){
			AddLogToFile("Lan_OutFileLawDownLoad error","","fp==NULL");
			return -1;
		}
		
		char *pBuf;
		pBuf = malloc(iSize+1);
		if(pBuf == NULL) {
			AddLogToFile("Lan_OutFileLawDownLoad error","","pBuf == NULL");
			return (-1);
		}
		memset(pBuf,0,iSize+1);
		int len=fread(pBuf,1,iSize,fp);
		if(len<=0){
			AddLogToFile("Lan_OutFileLawDownLoad error","","len<=0");
			return -1;
		}
		int iwrite=write(iFd,pBuf,len);
		if(iwrite!=len){
			AddLogToFile("Lan_OutFileLawDownLoad error","","iwrite!=len");
		}
		fclose(fp);
		free(pBuf);
	}
	return 0;
}
void TarFileFunc(char *filepath,char *outbuf){
	if(utFileIsExist(filepath)){
		char *pos=strrchr(filepath,'.');
		if(pos){
			char filetall[64]="";
			memcpy(filetall,pos+1,strlen(pos+1));
			if(strlen(filetall)>0){
				if(strcmp(filetall,"txt")==0||strcmp(filetall,"htm")==0||strcmp(filetall,"html")==0||strcmp(filetall,"c")==0){
					char DirName[512]="";
					pos=strrchr(filepath,'/');
					if(pos){
						memcpy(DirName,filepath,strlen(filepath)-strlen(pos+1));
						char name[64]="";
						char *pos2=strchr(pos+1,'.');
						memcpy(name,pos+1,strlen(pos+1)-strlen(pos2));
						if(strlen(name)>0&&strlen(DirName)>0){
							char comd[512]="";
							snprintf(outbuf,512,"%s%s.zip",DirName,name);
							snprintf(comd,sizeof(comd)-1,"zip %s%s.zip %s",DirName,name,filepath);
							system(comd);
							remove(filepath);
						}
					}
				}else{
					memcpy(outbuf,filepath,strlen(filepath));
				}
			}	
		}
	}
}
void filenamedeal(char *inbuf,char *outbuf){
	if(strlen(inbuf)>0){
		char filename[128]="";
		char path[512]="";
		char *pos=strrchr(inbuf,'/');
		if(pos){
			memcpy(filename,pos+1,strlen(pos+1));
			memcpy(path,inbuf,strlen(inbuf)-strlen(pos+1));
		}
		int i=0;
		for(i=0;i<strlen(filename);i++){
			if(filename[i]==' '||
			   filename[i]=='-'||
			   filename[i]=='\\'||
			   filename[i]=='/'||
			   filename[i]==':'||
			   filename[i]=='*'||
			   filename[i]=='?'||
			   filename[i]=='<'||
			   filename[i]=='>'||
			   filename[i]=='|'){
			     filename[i]='_';
			}
		}
		snprintf(outbuf,512,"%s%s",path,filename);
	}
}
int lan_UpOutFileContent(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	int len=0;
	char mesg[1024]="";
	char md5_client[64];
	char fileMd5_client[64];
	char filename[128]="";
	char filename_[128]="";
	int totalsize=0;
	unsigned long int compid=0;
	char sql[512]="";


	int iReturn=utMsgGetSomeNVar(psMsgHead,6,"CurrPagLen",UT_TYPE_ULONG,4,&len,
							"MD5",UT_TYPE_STRING,sizeof(md5_client)-1,md5_client,
							"fileMD5",UT_TYPE_STRING,sizeof(fileMd5_client)-1,fileMd5_client,
							"filename",UT_TYPE_STRING,sizeof(filename)-1,filename_,
							"totalsize",UT_TYPE_ULONG,4,&totalsize,
							"compid",UT_TYPE_ULONG,4,&compid);
	snprintf(filename,sizeof(filename)-1,"%s",utf8convert(psShmHead,filename_));
	snprintf(mesg,sizeof(mesg)-1,"compid=%lu\tlen=%d\tmd5_client=%s\tfileMd5_client=%s\tfilename=%s\ttotalsize=%d\t",
			compid,len,md5_client,fileMd5_client,filename,totalsize);
	AddLogToFile("UpOutFileContent func...","",mesg);
	if(compid<=0||len<=0||(0==strcmp("",md5_client))||(0==strcmp("",fileMd5_client))||(0==strcmp("",filename))||totalsize<=0){
		snprintf(mesg,sizeof(mesg)-1,"compid=%lu\tlen=%d\tmd5_client=%s\tfileMd5_client=%s\tfilename=%s\ttotalsize=%d\t",
			compid,len,md5_client,fileMd5_client,filename,totalsize);
		AddLogToFile("UpOutFileContent parames error","",mesg);
		pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "state",   UT_TYPE_LONG,-2);
		return -2;
	}
	char *myData=NULL;
	myData=malloc(len+1);
	if(!myData){
		AddLogToFile("*********lan_UpOutFileContent malloc error***********",md5_client,fileMd5_client);
		pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "state",   UT_TYPE_LONG,-3);
		return -3;
	}

	memset(myData,0,len+1);
	int ilen = MyTcpRecv(iFd, myData, len);
	if(ilen!=len){
		AddLogToFile("UpOutFileContent", "recv length error...", "");
		pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "state",   UT_TYPE_LONG,-6);
		free(myData );
		return -6;
	}
	snprintf(sql,sizeof(sql)-1,"select Status,savesize from nwoutfilecontent where Filemd5='%s' and Computerid=%lu",fileMd5_client,compid);
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) 
	{ 
        AddLogToFile("nwoutfilecontent error..." ,"" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL,1,"state",UT_TYPE_LONG,-7);
		free(myData);
		return -7;
    }else{
		int Statues=0;
		int savesize=0;
		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG, 4, &Statues,
			UT_TYPE_LONG, 4, &savesize);
		pasDbCloseCursor(psCur);

		if((iReturn!=0)  ||compid<= 0){
			pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "state",   UT_TYPE_LONG,    -8);
			free(myData);
			return -8;
		}
		if(Statues==3){
			AddLogToFile("Statues=3" ,"the file has saveing in server..." , sql);
			pasTcpResponse(iFd,psMsgHead,NULL,1,
					 "state",   UT_TYPE_LONG,    -1);
			free(myData);
			return -1;
		}

		char   md5_res[64];
		memset(md5_res, 0, sizeof(md5_res)); 
		lan_GetMD5((unsigned char*)myData, ilen , (unsigned char*)md5_res);
		if(0!=strcmp(md5_client,md5_res)){
			AddLogToFile("UpOutFileContent...", "md5 cheack error...", "");
			pasTcpResponse(iFd,psMsgHead,NULL,1,"state",   UT_TYPE_LONG,-9);
			free(myData);
			return -9;
		}
		char path[256]="";
		snprintf(path,sizeof(path)-1,"%scontent/%lu",OUTFILE_CONTENT_LAW_PATH,compid);
		if(!utFileIsExist(path)){
			char comd[512]="";
			snprintf(comd,sizeof(comd)-1,"mkdir -p %s",path);
			system(comd);
		}

		char newpath[512]="";
		char myfilename[128]="";
		char* ps=strrchr(filename,'\\');
		if(ps==NULL)
			return -1;
		memcpy(myfilename,ps+1,strlen(ps+1));
		snprintf(newpath,sizeof(newpath)-1,"%s/%s",path,myfilename);
		char mypath[512]="";
		filenamedeal(newpath,mypath);
		FILE *fp;
		fp=fopen(mypath,"a+b");
		if(!fp){
			AddLogToFile("upoutfilecontent error...", "openfile...  ", newpath);
			pasTcpResponse(iFd,psMsgHead,NULL,1,"state",   UT_TYPE_LONG,    -10);
			free(myData);
			return -10;
		}
		fwrite(myData,1,ilen,fp);
		fclose(fp);
		free(myData);
		
		savesize+=ilen;
		int curStatues=0;
		if(savesize>=totalsize)
			curStatues=3;
		else
			curStatues=2;
		float num=((float)savesize)/totalsize;
		int Upstatus=num*100;
		memset(sql,0,sizeof(sql));

		if(Upstatus>=100){
			//char outbuf[512]="";
			//TarFileFunc(newpath,outbuf);
			//snprintf(sql,sizeof(sql)-1,"update nwoutfilecontent set Filename='%s',Status=%d,Upstatus=%d,savesize=%d where Filemd5='%s' and Computerid=%lu",outbuf,curStatues,Upstatus,savesize,fileMd5_client,compid);
			snprintf(sql,sizeof(sql)-1,"update nwoutfilecontent set Filename='%s',Status=%d,Upstatus=%d,savesize=%d where Filemd5='%s' and Computerid=%lu",mypath,curStatues,Upstatus,savesize,fileMd5_client,compid);
		}else{
			snprintf(sql,sizeof(sql)-1,"update nwoutfilecontent set Filename='%s',Status=%d,Upstatus=%d,savesize=%d where Filemd5='%s' and Computerid=%lu","",curStatues,Upstatus,savesize,fileMd5_client,compid);
		}
		
		
		if(pasDbExecSql(sql, 0)){
			AddLogToFile("update outfilecontent err...", " " , sql);
			pasTcpResponse(iFd,psMsgHead,NULL,1,"state",UT_TYPE_LONG,-11);
			return -11;
		}
	}
	return 0;
}
/********************************************************************************
*******************************查询外发文件内容*******************************
*********************************************************************************/
int lan_outfilecontent(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	
  utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();
	long iReturn=0;
	char _keyword[33]="";
	char keyword[33]="";
	char _filename[72]="";
	char filename[72]="";
	char _uid[17]="";
	char _taskid[32]="";
	char taskid[32]="";
	char _devname[32]="";
	char devname[32]="";
	char ip1[24]="";
	char sip1[24]="";
	char department1[16]="";
	char procname1[64]="";
	char mac1[72]="";
	char compid1[32]="";
	char compid[32]="";
	char sdate[11] = "";
	char edate[11] = "";
	char stime1[11] = "";
	char etime1[11] = "";
	char time_flag[2] = "";
	char start[17] = "";
 	char limit[17] = "";
 	char sort[16] = "";
 	char dir[16] = "";
	char refresh[32]="";
 	char caExport[12];
 	int llcount=0;
 	
 	memset(caExport,0,sizeof(caExport));
	
	utMsgGetSomeNVar(psMsgHead, 11,
					 "devname",				UT_TYPE_STRING,  			sizeof(_devname)-1,  	 _devname,
   				     "keyword",				UT_TYPE_STRING,  			sizeof(_keyword)-1,  	 _keyword,
   				     "ip1",       			UT_TYPE_STRING,  			sizeof(ip1)-1,     		ip1,
   				     "groupid",  		    UT_TYPE_STRING,  			sizeof(department1)-1, 	department1,
   				     "compid",				UT_TYPE_STRING, 			sizeof(compid1)-1, 		compid1,
   				     "sdate", 	   			UT_TYPE_STRING,  			sizeof(sdate)-1,       	sdate,
   				     "edate", 	   			UT_TYPE_STRING,  			sizeof(edate)-1,       	edate,
   				     "time_flag",  			UT_TYPE_STRING,  			sizeof(time_flag)-1,   	time_flag,
					"refresh",  			UT_TYPE_STRING,  			sizeof(time_flag)-1,   	refresh,
   				     "start",      			UT_TYPE_STRING,  			sizeof(start)-1,       	start,
   				     "limit",      			UT_TYPE_STRING,  			sizeof(limit)-1,       	limit,
   				     "sort",       			UT_TYPE_STRING,  			sizeof(sort)-1,        	sort);
   				   

  	     
  char mesg[512];
  memset(mesg,0,sizeof(mesg));
  snprintf(devname,sizeof(devname)-1,"%s",utf8convert(psShmHead,_devname));
  snprintf(filename,sizeof(filename)-1,"%s",utf8convert(psShmHead,_filename));
  snprintf(keyword,sizeof(keyword)-1,"%s",utf8convert(psShmHead,_keyword));
	
	int start_time=-1;
	int end_time=-1;
	int flag_type=-1;
	int time_type=atoi(time_flag);
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
		snprintf(mesg,sizeof(mesg)-1,"start_time:%d\tend_time:%d\tsdate:%s\tedate:%s\ttime_flag:%s",start_time,end_time,sdate,edate,time_flag);

	}else{
		flag_type = 0;
		start_time=transDataTimeToInt2(sdate);
		end_time=transDataTimeToInt2(edate)+24*60*60-1;
		memset(mesg,0,sizeof(mesg));
		snprintf(mesg,sizeof(mesg)-1,"start_time:%d\tend_time:%d\tsdate:%s\tedate:%s\t",start_time,end_time,sdate,edate);
	}
	char *mysql=NULL;
	mysql=(char *)malloc(1024*10);
	if(!mysql)
		return 0;
	else
		memset(mysql,0,1024*10);
	char tableanme[32]="nwoutfilecontent";
		char sql[1024*2];
		memset(sql,0,sizeof(sql));
		long lCount=0;
		int ret=pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);

		memset(sql,0,sizeof(sql));
		snprintf(sql,sizeof(sql),"select sid,Filemd5,Computerid,Ips,Devname,Filename,Status,Upstatus,stime,userer from %s where(stime >=%lu and stime <=%lu)",tableanme,start_time,end_time);
		//add filter conditions here
	if(strlen(ip1)>0){
		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and Ips like '%%%s%%'",ip1);
	}
	if(strlen(devname)>0){
	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and Devname like '%%%s%%'",devname);
	}else if(keyword!=NULL&&strlen(keyword)>0){
		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)-1," and (Ips like '%%%s%%' or Devname like '%%%s%%' or userer like '%%%s%%')",keyword,keyword,keyword);
	}
	pasDbCursor *psCur = NULL;
	char tempsql[1024]="";
	snprintf(tempsql,sizeof(tempsql)-1,sql);
	replaceStr(tempsql,"sid,Filemd5,Computerid,Ips,Devname,Filename,Status,Upstatus,stime,userer","count(*)");
	pasDbOneRecord(tempsql,0,UT_TYPE_ULONG,4,&llcount);
	memset(tempsql,0,sizeof(tempsql)-1);
	recodeLogInfo("web log test data...","************692************",tempsql);
	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql),"order by stime desc limit %d,%d",atoi(start),atoi(limit));	
	recodeLogInfo("web log test data...","************694************",sql);
	psCur = pasDbOpenSql(sql, 0);
	int iNum=0;
	int iret = 0;

	if(psCur!=NULL){
		long long sid=0;
		char filemd5[48]="";
		unsigned int computerid=0;
		unsigned long  int ipsStr=0;
		char filename[256]="";
		unsigned int status=0;
		char statusStr[32]="";
		char devname[48]="";
		int upstatus=0;
		unsigned long int stime=0;
		char userer[24]="";
		char caGroup[32]="";
		nwCompInfo *psComp;
		while(0==(iret=pasDbFetchInto(psCur,
			UT_TYPE_LONG8,			sizeof(sid),			&sid,
			UT_TYPE_STRING,			sizeof(filemd5)-1,		filemd5,
			UT_TYPE_ULONG,			sizeof(computerid),		&computerid,
			UT_TYPE_ULONG,			sizeof(ipsStr),		    &ipsStr,
			UT_TYPE_STRING,			sizeof(devname)-1,		devname,
			UT_TYPE_STRING,			sizeof(filename)-1,		filename,
			UT_TYPE_ULONG,			sizeof(status),			&status,
			UT_TYPE_ULONG,			sizeof(upstatus),		&upstatus,
			UT_TYPE_ULONG,			sizeof(stime),		    &stime,
			UT_TYPE_STRING,			sizeof(userer)-1,		userer
			)) || 1405 == iret){
			iNum++;
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"sid",iNum,"%llu",sid);
			// get group
			
			struct in_addr addr;
			addr.s_addr=htonl(ipsStr);
			char webIp[128];
			memset(webIp,0,sizeof(webIp));
			snprintf(webIp,sizeof(webIp)-1,"%s",inet_ntoa(addr));
			utPltPutLoopVar(psDbHead,"webIp",iNum,webIp);
			char mywebtime[128];
			memset(mywebtime,0,sizeof(mywebtime));
			local_time = localtime(&stime);
			strftime(mywebtime,sizeof(mywebtime),"%H:%M:%S %Y-%m-%d",local_time);
			utPltPutLoopVar(psDbHead,"mywebtime",iNum,mywebtime);
			utPltPutLoopVar(psDbHead,"devname",iNum,devname);
			char myfilename[128]="";
			char *pos=strrchr(filename,'/');
			if(pos){
				memcpy(myfilename,pos+1,strlen(pos+1));
				utPltPutLoopVar(psDbHead,"filename",iNum,myfilename);
			}else{
				utPltPutLoopVar(psDbHead,"filename",iNum,filename);
			}
			
			utPltPutLoopVar(psDbHead,"filepath",iNum,filename);
			utPltPutLoopVar(psDbHead,"filemd5",iNum,filemd5);
			utPltPutLoopVar(psDbHead,"userer",iNum,userer);
			memset(statusStr,0,sizeof(statusStr));
			switch(status){
			case 0:
				strcpy(statusStr,"等待查询");
				break;
			case 1:
				strcpy(statusStr,"准备上传");
				break;
			case 2:
				strcpy(statusStr,"正在上传");
				break;
			case 3:
				strcpy(statusStr,"上传完成");
				break;
			}
		
			utPltPutLoopVar(psDbHead,"status",iNum,statusStr);
			utPltPutLoopVarF(psDbHead,"upstatus",iNum,"%d",upstatus);
			//printf("11111111myfilename=%s  2222222222  upstatus=%d\r\n",myfilename,upstatus);
			psComp =  (nwCompInfo *)utShmHashLookA(psShmHead,NC_LNK_COMPUTE,&computerid);
			strcpy(caGroup,(char *)lanGetGroup(psShmHead,psComp->groupid));
			unsigned long long currentTime = 0;
			currentTime=pasGetLongTimeId();
			snprintf(mesg,sizeof(mesg)-1,"%llu",currentTime);
			utPltPutLoopVar(psDbHead,"department",iNum,caGroup);
			
			
			sid = 0;
			computerid = 0;
			ipsStr = 0;
			status = 0;
			upstatus = 0;
			stime = 0;
			memset(filemd5, 0, sizeof(filemd5));
			memset(filename, 0, sizeof(filename));
			memset(statusStr, 0, sizeof(statusStr));
			memset(devname, 0, sizeof(devname));
			memset(userer, 0, sizeof(userer));
			memset(caGroup,0,sizeof(caGroup));
					
			
		}
		pasDbCloseCursor(psCur);
	}

	utPltPutVarF(psDbHead,"TotRec","%lu",llcount);
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/outfile/lan_outfile_content.htm");
	free(mysql);
	if(strcmp(refresh,"true")){
	//调?盟⑿孪略氐暮?
		Lan_GetFileContent_Law(psShmHead,iFd,psMsgHead);
	}
    return 0;
}

int outfilecontentDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	char filepath[512]="";
	char filename[128]="";
	utMsgGetSomeNVar(psMsgHead, 2,
					"filepath",				UT_TYPE_STRING,  			sizeof(filepath)-1,  	 filepath,
					"filename",				UT_TYPE_STRING,  			sizeof(filename)-1,  	 filename);
	char *pos=strrchr(filepath,'/');
	char _filepath[512]="";
	memcpy(_filepath,filepath,strlen(filepath)-strlen(pos));
	//printf("filename=%s\n filepath=%s\n",filename,_filepath);
	utPltFileDownload(iFd,"application/text",_filepath,filename,filename);
	return 0;
}





int lanWeb_initFun004(utShmHead *psShmHead){
	long int iReturn=0;
	iReturn = pasSetTcpFunName("lan_outfilecontent",lan_outfilecontent,0);
	iReturn = pasSetTcpFunName("lan_outfilecontent_insert",lan_outfilecontent_insert,0);
	iReturn = pasSetTcpFunName("lan_GetOutFileSaveSize",lan_GetOutFileSaveSize,0);
	iReturn = pasSetTcpFunName("Lan_OutFileLawDownLoad",Lan_OutFileLawDownLoad,0);
	iReturn = pasSetTcpFunName("lan_UpOutFileContent",lan_UpOutFileContent,0);
	iReturn = pasSetTcpFunName("lan_RemoveSerLawFile",lan_RemoveSerLawFile,0);
	iReturn = pasSetTcpFunName("outfilecontentDownload",outfilecontentDownload,0);
	return 0;
}


