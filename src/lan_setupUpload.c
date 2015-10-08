#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pasdb.h"
#include "utoplt01.h"

#define		WT_BIND_MACFILELENOLD		(1024*1024-256)
#define		WT_BIND_MACFILELEN64		(3*1024*1024-256)
#define		WT_BIND_MACFILELEN32		(3*1024*1024-256)

int lan_setupUpload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	
  utMsgPrintMsg(psMsgHead);
	printf("upload!\n");
	
	char sysVersion_in[8] = "";
	char setupType_in[8] = "";	
	utPltDbHead *psDbHead = utPltInitDb();
  
	utMsgGetSomeNVar(psMsgHead,2,
		"sysVersion",UT_TYPE_STRING,sizeof(sysVersion_in)-1,sysVersion_in,
		"setupType",UT_TYPE_STRING,sizeof(setupType_in)-1,setupType_in);
		
	if(utStrIsSpaces(setupType_in)){
	  strcpy(setupType_in,"0");
	}
		
	char conf[248] = "";
	//权限
	char groupid[16] = "";
	char *auth = getDsGroupid();
	if(!utStrIsSpaces(auth)){
		strcpy(groupid,auth);
		char *tmp = groupid;
		while(*tmp!= ',' && *tmp!='\0'){
			tmp++;
		}
		*tmp = '\0';
	}else{
		strcpy(groupid,"0");
	}
	char buffer1[24];
	memset(buffer1,'1', sizeof(buffer1));	
	
	snprintf(conf,sizeof(conf),"<centerip>%s</centerip><centerid>%s</centerid><centerudpport>%s</centerudpport><setuptype>%s</setuptype><groupid>%s</groupid>",utComGetVar_sd(psShmHead,"AdminIp","192.168.20.190"),utComGetVar_sd(psShmHead,"ProductSN","PK360817"),utComGetVar_sd(psShmHead,"Udpport","10004"),setupType_in,groupid);
	
	printf("conf:%s\n",conf);
	
	char srcFilePath[128] = "";
	char desFilePath[128] = "";
	char fileName[128] = "";
	char srcFile[256] = "";
	char desFile[256] = "";
	
	strcpy(srcFilePath,"/home/ncmysql/nw/upload/soft/");
	strcpy(desFilePath,"/home/ncmysql/nw/upload/");
	
	//三合一安装包
/*	if(atoi(setupType_in)==1){
		strcpy(fileName,"prolansetup.exe");
	}else{
		strcpy(fileName,"prolansetup_s.exe");
	}
*/
	//sysVersion_in =   1:win老版本   2:win8 32bit   3:win7、win8 64bit
	switch(atoi(sysVersion_in)){
		case 1:
			strcpy(fileName,"pronetsetupx86.exe");
			break;
		case 2:
			strcpy(fileName,"pronetsetupwin8_32.exe");
			break;
		case 3:
			strcpy(fileName,"pronetsetup64.exe");
			break;
		default: ;
	}
	
	snprintf(srcFile,sizeof(srcFilePath),"%s%s",srcFilePath,fileName);
	snprintf(desFile,sizeof(desFilePath),"%s%s",desFilePath,fileName);
	
	char sys_tmp[256] = "";
	char msg[256] = "";
	snprintf(sys_tmp,sizeof(sys_tmp),"cp %s %s",srcFile,desFilePath);
	
	system(sys_tmp);
	
	
//	FILE * file = fopen(desFile, "a+");
	printf("src:%s dir:%s\n",srcFile,desFile);
	int iret = 0;
	switch(atoi(sysVersion_in)){
		case 1:
			iret = insertToFile1(desFile,WT_BIND_MACFILELENOLD,conf);
			break;
		case 2:
			iret = insertToFile1(desFile,WT_BIND_MACFILELEN32,conf);
			break;
		case 3:
			iret = insertToFile1(desFile,WT_BIND_MACFILELEN64,conf);
			break;
		default: ;
	}

	 
	snprintf(msg,sizeof(msg),"生成安装包[%s]",fileName);
	
	if(iret != 0){
		utPltPutVar(psDbHead,"result","false");
		utPltPutVar(psDbHead,"msg","生成安装包失败,请检查安装源文件");
		writeSysLog(psMsgHead,"08","Fail",msg);
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_submit.htm");
		return 0;
	}
	
/*	if(!file){
		utPltPutVar(psDbHead,"result","false");
		utPltPutVar(psDbHead,"msg","生成安装包失败,请检查安装源文件");
		writeSysLog(psMsgHead,"08","Fail",msg);
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_submit.htm");
		return 0;
	}else{
		fwrite(buffer1, 1, sizeof(buffer1), file);
		fwrite(conf, 1, sizeof(conf), file);
		fclose(file);
	}
*/	
	writeSysLog(psMsgHead,"08","Success",msg);
	utPltFileDownload(iFd,"application/octet-stream",desFilePath,fileName,fileName);
	
	remove(desFile);
	
	return 0;
}


int lan_scanDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utPltDbHead *psDbHead = utPltInitDb();
	
	char srcFilePath[128] = "";
	char desFilePath[128] = "";
	char fileName[128] = "Scan.exe";
	char srcFile[256] = "";
	char desFile[256] = "";
	
	strcpy(srcFilePath,"/home/ncmysql/nw/upload/soft/");
	strcpy(desFilePath,"/home/ncmysql/nw/upload/");
	
	snprintf(srcFile,sizeof(srcFilePath),"%s%s",srcFilePath,fileName);
	snprintf(desFile,sizeof(desFilePath),"%s%s",desFilePath,fileName);
	
	char sys_tmp[256] = "";
	char msg[256] = "";
	snprintf(sys_tmp,sizeof(sys_tmp),"cp %s %s",srcFile,desFilePath);
	
	system(sys_tmp);
	
	
	FILE * file = fopen(desFile, "a+");

	char buffer[24];
	memset(buffer,0, sizeof(buffer));
	char buffer1[24];
	memset(buffer1,'1', sizeof(buffer1));

	char buffer2[248] = "";
	char sql[128] = "";
	char shortname[32] = "";
	
	char caMyname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",caMyname);
	
	sprintf(sql,"select name from dsuser where groupid=(select groupid from dsuser where name='%s') and name like '%%admin%%'",caMyname);
	printf("扫描包下载：sql %s\n",sql);
	pasDbOneRecord(sql,0,UT_TYPE_STRING,sizeof(shortname),shortname);
	
	shortname[strlen(shortname)-5] = 0;
	
	sprintf(buffer2,"<company>%s</company><serverIp>%s</serverIp><serverPort>%s</serverPort>",shortname,utComGetVar_sd(psShmHead,"AdminIp","192.168.20.190"),utComGetVar_sd(psShmHead,"Tcpport","9185"));
	if(file){
		//fwrite(buffer, 1, sizeof(buffer), file);
		fwrite(buffer1, 1, sizeof(buffer1), file);
		fwrite(buffer2, 1, sizeof(buffer2), file);
		fclose(file);
	}else{
		utPltPutVar(psDbHead,"result","false");
		utPltPutVar(psDbHead,"msg","请检查扫描安装包源文件");
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_submit.htm");
		return 0;
	}
	
	utPltFileDownload(iFd,"application/octet-stream",desFilePath,fileName,fileName);
	
	remove(desFile);
	
	return 0;
}