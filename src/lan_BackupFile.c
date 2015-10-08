#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>
#include "string.h"
#include "utoall.h"
#include "utomon.h"
#include "ncdef.h"
#include "nwdef.h"
#include "pasdb.h"
#include "lan_md5.h"
#include <stdlib.h>
#include <errno.h>


int8 pasGetLongTimeId();


#include <sys/stat.h>
#include <sys/types.h>


#define  BACKUPFILEPATH		"/home/ncmysql/nw/BackUpFile"
#define RESPONSEBG	990118198

#define  PACKAGE_SIZE	10*1024*1024



int  Lan_SendLittleFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int	len = 0;
	int	number = 0;
	
	int 	totalFileSize=0;
	


	char	targetpath[216];
	char 	md5_client[64];
	char	fileMd5_client[64];

	char    sql[1024];
	char 	*myData=NULL;

	memset(md5_client,0,sizeof(md5_client));
	memset(fileMd5_client,0,sizeof(fileMd5_client));
	memset(targetpath,0,sizeof(targetpath));
	memset(sql,0,sizeof(sql));

	int iReturn=utMsgGetSomeNVar(psMsgHead,4,"CurrPagLen",UT_TYPE_ULONG,4,&len,
							"number",UT_TYPE_ULONG,4,&number,
							"MD5",UT_TYPE_STRING,sizeof(md5_client)-1,md5_client,
							"fileMD5",UT_TYPE_STRING,sizeof(fileMd5_client)-1,fileMd5_client);
	
	myData=malloc(len+1);
	
	if(!myData)
	{
		recodeLogInfo("*********Lan_SendLittleFile申请空间失败***********",md5_client,fileMd5_client);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,
					 "state",   UT_TYPE_LONG,    -3);
		return -3;
	}
	
	memset(myData,0,len+1);
	int ilen = MyTcpRecv(iFd, myData, len);

	if( (len <= 0)||(0==strcmp("", md5_client))||(0==strcmp("", fileMd5_client)))
	{
		recodeLogInfo("文件上传数据参数错误",md5_client,fileMd5_client);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,
					 "state",   UT_TYPE_LONG,    -2);
		return -2;
	}
	
	if(ilen != len)
	{
		recodeLogInfo("文件上传 ", "接收文字  ", " 接收数据长度有误");
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,
					 "state",   UT_TYPE_LONG,    -6);
		free(myData );
		return -6;
	}

	
	//查询数据库，找到当前unique对应的文件路径
	snprintf(sql, sizeof(sql)-1, "select ServerFilePath, Filesize   from    nwAuditFileBackup  where Szmd5=\'%s\'  ",fileMd5_client);
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) 
	{ //查询数据库失败，记录日志
        		recodeLogInfo("从nwAuditFileBackup数据表中查询数据失败！" ,"" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -7);
		return -7;
        	}
	else
	{
		iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING, sizeof(targetpath)-1,targetpath,UT_TYPE_LONG, 4, &totalFileSize);
		pasDbCloseCursor(psCur);
		
		if((iReturn!=0)  || (totalFileSize<= 0)|| (strcmp("", targetpath )==0)){//如果没有查询到记录
			pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,
					 "state",   UT_TYPE_LONG,    -8);
			return -8;
		}

		/*
		if(number>=totalFileSize)
		{
			recodeLogInfo("文件已经传送完毕 ",targetpath, "&&&&&&&&&&&&&&&");
			pasTcpResponse(iFd,psMsgHead,NULL, 1, "state",   UT_TYPE_LONG,1);
			return 1;
		}
		*/
	}



	recodeLogInfo("文件正在上传************* ",targetpath, "");
	
	//得到MD5值
	char   md5_res[64];
	memset(md5_res, 0, sizeof(md5_res)); 
	lan_GetMD5((unsigned char*)myData, ilen , (unsigned char*)md5_res);

	recodeLogInfo("文件备份 ", "验证MD5  ", "");

	if(0 != strcmp(md5_client,  md5_res)){
		recodeLogInfo("软件上传 ", "接收文字  ", " MD5验证失败");
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,
					 "state",   UT_TYPE_LONG,    -9);

		free(myData);
		return -9;
	}

	FILE * fp = fopen(targetpath, "a+b");
	if(!fp){
		recodeLogInfo("软件上传 ", "打开路径失败  ", targetpath);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */1,
					 "state",   UT_TYPE_LONG,    -10);

		free(myData );
		return -10;
	}

	fwrite(myData, 1, ilen , fp);
	fclose(fp);


	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不?? */ 1,
					 "state",   UT_TYPE_LONG,    1);

	recodeLogInfo("软件上传 ", "开始  ", "结束");

	free(myData );
	return 0;
}		



int  Lan_CheackFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char md5[216];
	char sql[1024];
	int    totalFileSize=0;
	


	char	targetpath[216];
	memset(md5,0,sizeof(md5));
	memset(sql,0,sizeof(sql));
	memset(targetpath,0,sizeof(targetpath));
	
	int iReturn = utMsgGetSomeNVar(psMsgHead,14,
					"fileMD5",   UT_TYPE_STRING, sizeof(md5)-1, md5);

	

	recodeLogInfo("**************************************" ,"*************************************" , sql);

	snprintf(sql, sizeof(sql)-1, "select ServerFilePath, Filesize   from    nwAuditFileBackup  where Szmd5=\'%s\'  ",md5);
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) 
	{ 
		//查询数据库失败，记录日志
        		recodeLogInfo("从nwAuditFileBackup数据表中查询数据失败！" ,"" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-1);
		return -1;
        	}
	else
	{
		iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING, sizeof(targetpath)-1,targetpath,UT_TYPE_LONG, 4, &totalFileSize);
		pasDbCloseCursor(psCur);
		
		if((iReturn!=0)  || (totalFileSize<= 0)|| (strcmp("", targetpath )==0)){//如果没有查询到记录
			recodeLogInfo("************************************文件检测无*************************************",  totalFileSize, sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,
					 "state",   UT_TYPE_LONG,-4);
			return 0;
		}
		if(access(targetpath, 0) == 0)
		{
			pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */1,
					 "state",   UT_TYPE_LONG,-5);
			return 0;
		}
	}
	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */1,
					 "state",   UT_TYPE_LONG,0);
	return 0;
}



int  Lan_SendFileInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int      groupid;
	int      computerid;
	char    company[24];
	int      Sip;
	char    szMd5[48];
	char    Loginuser[24];
	char    LocalFilePath[256];
	char    ProcessName[24];
	char    Authuser[24];
	char	filename[128];		//文件名
	int           Filesize;
	time_t  createtime;
	char    createuser[24];
	int      Level;


	char	sql[2048];


	memset(company,0,sizeof(company));
	memset(szMd5,0,sizeof(szMd5));
	memset(Loginuser,0,sizeof(Loginuser));
	memset(LocalFilePath,0,sizeof(LocalFilePath));
	memset(ProcessName,0,sizeof(ProcessName));
	memset(Authuser,0,sizeof(Authuser));
	memset(filename,0,sizeof(filename));
	memset(createuser,0,sizeof(createuser));

	memset(sql,0,sizeof(sql));

	recodeLogInfo("111111111111111111","","123");
	int iReturn = utMsgGetSomeNVar(psMsgHead,14,
					"groupid",   	UT_TYPE_LONG, 4, &groupid,
					"computerid",   	UT_TYPE_LONG, 4, &computerid,
					"company",   	UT_TYPE_STRING, sizeof(company)-1, company,
					"Sip",   	UT_TYPE_LONG, 4, &Sip,
					"szMd5",   	UT_TYPE_STRING, sizeof(szMd5)-1, szMd5,
					"Loginuser",   	UT_TYPE_STRING, sizeof(Loginuser)-1, Loginuser,
					"LocalFilePath", UT_TYPE_STRING, sizeof(LocalFilePath)-1, LocalFilePath,
					"ProcessName",	 UT_TYPE_STRING, sizeof(ProcessName)-1, ProcessName,
					"Authuser", 	UT_TYPE_STRING, sizeof(Authuser)-1, Authuser,
					"fileName", 	UT_TYPE_STRING, sizeof(filename)-1, filename,
					"Filesize",   	UT_TYPE_LONG, 4, &Filesize,
					"createtime",   	UT_TYPE_LONG, 4, &createtime,
					"createuser", 	UT_TYPE_STRING, sizeof(createuser)-1, createuser,
					"Level",   	UT_TYPE_LONG, 4, &Level);
	
	time_t now;
	struct tm *time1;
	recodeLogInfo("22222222222222222","","123");
	time(&now);
	time1 = localtime(&now);

	char	year[24];
	char	mon[24];
	memset(year, 0, sizeof(year));
	memset(mon, 0, sizeof(mon));
	//当前年份
	snprintf(year, 5, "%d", time1->tm_year+1900);
	//当前月份
	if( (time1->tm_mon+1 > 0) && (time1->tm_mon+1 <= 9) ){
		snprintf(mon, 3, "0%d", time1->tm_mon+1);
	}else{
		snprintf(mon, 3, "%d", time1->tm_mon+1 );
	}

	char  savedir[512];
	memset(savedir, 0, sizeof(savedir));
	snprintf(savedir, sizeof(savedir)-1, "%s", BACKUPFILEPATH);
	mkdir(savedir, S_IRWXU);

	snprintf(savedir, sizeof(savedir)-1, "%s/%s%s", BACKUPFILEPATH,  year, mon);
	mkdir(savedir, S_IRWXU);
	snprintf(savedir, sizeof(savedir)-1, "%s/%s%s/%s%u%u", BACKUPFILEPATH,  year, mon, szMd5, createtime,computerid);
	
	snprintf(sql, sizeof(sql)-1, "select  count(*)  from   nwAuditFileBackup   where computerid=\'%u\' and company=\'%s\' and szMd5=\'%s\'",  computerid,company,szMd5);


	int count=0;
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //查询数据库失败，记录日志
        		recodeLogInfo("从nwAuditFileBackup数据表中查询数据失败！" ,"" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        	}else{
		iReturn = pasDbFetchInto(psCur,  UT_TYPE_ULONG, 4, &count);
		pasDbCloseCursor(psCur);
		if(count==0)
		{
			recodeLogInfo("333333333333333333"," ","123");
			snprintf(sql, sizeof(sql)-1, "insert into nwAuditFileBackup(groupid, computerid, company, Sip, szMd5, Loginuser," 
				" ServerFilePath, LocalFilePath, ProcessName, Authuser, filename, Filesize," 
				" createtime, createuser, Level) "
				"  values(%u, %u, \'%s\', %u,  \'%s\', \'%s\', "
				" \'%s\', \'%s\',  \'%s\',  \'%s\',  \'%s\',  %u,"
				"%u,\'%s\', %u)",
				groupid,computerid,  company, Sip, szMd5, Loginuser, 
				savedir,LocalFilePath,  ProcessName,  Authuser,  filename,  Filesize,
				createtime,  createuser,  Level);
			recodeLogInfo("sql---------------"," ",sql);
			if(pasDbExecSql(sql, 0))
			{
				//recodeLogInfo("插入记录到nwAuditFileBackup数据表?惺О埽?, " " , sql);
				pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG, 0);
				return 0;	
			}
			else
			{
				pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,   -4);
				return 0;
			}
		}
		else
		{
			pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,   -5);
			return 0;
		}
	}

	//接收任务信息成功
	recodeLogInfo("44444444444444444","","123");
	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,0);
	return 0;
}





int ncInitNwWebFun_BackUpFile(utShmHead *psShmHead)
{
	pasSetTcpFunName("Lan_SendFileInfo",     	Lan_SendFileInfo,  NULL,0,0);
	pasSetTcpFunName("Lan_SendLittleFile",     	Lan_SendLittleFile,  NULL,0,0);
	pasSetTcpFunName("Lan_CheackFile",     		Lan_CheackFile,  NULL,0,0);

	return 0;
}