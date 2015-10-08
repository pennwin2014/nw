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

#include <sys/types.h>  
#include <sys/stat.h>  

#define		PRO_CENTER_COMMANDID_CAPTURESCREEN	0X00000010	//实时屏幕

typedef struct _processInfo{
	unsigned int	compid;
	char		mac[24];
	char		ip[24];
	unsigned int	StartTime;   	//启动时间
	char		name[72];	// 进程名称
	char		path[256];      // 进程路径
	char	     	content[256];   // 进程描述
    	char		indexid[48];
	char         	pindexid[48];
	int		pid;
	int		cpuusage;
	int		cpus;
	int		mem;
	//int		vmem;        		
}ProcessInfo, *pProcessInfo;




#define  JPG_MAXLEN     1024*1024


#define ONLINE_DIR    	     "/home/ncmysql/nw/online/"
#define ONLINE_JPG_DIR       "/home/ncmysql/nw/online/screen/"
#define ONLINE_PROC_DIR      "/home/ncmysql/nw/online/process/"



int Lan_Online_Screen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int          iReturn = 0;
	unsigned int compid = 0;
	unsigned int filelenth = 0;
	char         filename[512];
	char	    *data = NULL;

	int 	     status = 0;
	int          quality = 0;
	
	char         destPath[1024];
	char         tempPath[1024];

	memset(filename, 0, sizeof(filename));
	memset(destPath, 0, sizeof(destPath));
	memset(tempPath, 0, sizeof(tempPath));


	data = (char *)malloc(JPG_MAXLEN);
	if(!data){
		
		recodeLogInfo("接收实时屏幕：","动态开辟空间失败！", "");
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "status",   UT_TYPE_LONG,   0,
					 "quality",  UT_TYPE_LONG,   0
					);
		return 0;
		
	}

	iReturn = utMsgGetSomeNVar(psMsgHead,4,
		                     "compid",     UT_TYPE_LONG,  4, &compid,
					"filelenth", UT_TYPE_LONG, 4, &filelenth,
					"filename",  UT_TYPE_STRING,  sizeof(filename)-1, filename,
					"data",      UT_TYPE_STRUCT, JPG_MAXLEN, data
					);

	char mesg[512];
	memset(mesg, 0, sizeof(mesg));
	snprintf(mesg, sizeof(mesg)-1, "iReturn=%d, compid = %u,filelen=%d, filename=%s",iReturn ,compid, filelenth, filename);
	recodeLogInfo("接收实时屏幕：","接收到数据 ", mesg);

	if( (iReturn != 4) ||
	    (compid <= 0)||
	    (filelenth > JPG_MAXLEN) ||
	    (filelenth <= 0)){
		status = 0;
		quality = 0;

		recodeLogInfo("接收实时屏幕：","接收到数据有误, ", mesg);
	}else{

		//创建图片文件
		snprintf(tempPath, sizeof(tempPath)-1, "%s%u.jpg.bak", ONLINE_JPG_DIR, compid);
		snprintf(destPath, sizeof(destPath)-1, "%s%u.jpg", ONLINE_JPG_DIR, compid);
	   	FILE * fp = fopen(tempPath, "wb");
		if(!fp){
			snprintf(tempPath, sizeof(tempPath)-1, "%s", ONLINE_DIR);
			mkdir(tempPath,S_IRWXG);
			snprintf(tempPath, sizeof(tempPath)-1, "%s", ONLINE_JPG_DIR);
			mkdir(tempPath,S_IRWXG);

			snprintf(tempPath, sizeof(tempPath)-1, "%s%u.jpg.bak", ONLINE_JPG_DIR, compid);
			fp = fopen(tempPath, "w");
		}

		if(!fp){
			status = 0;
			quality = 0;

			recodeLogInfo("接收实时屏幕：","创建目录失败 ", tempPath);
		}else{
			int nRet = fwrite(data, 1, filelenth, fp);
			fclose(fp);
			if(nRet != filelenth){
				status = 0;
				quality = 0;
				recodeLogInfo("接收实时屏幕：","存储文件失败", tempPath);
			}else{
				remove(destPath);
				rename(tempPath, destPath);
				remove(tempPath);

				quality =atol(utComGetVar_sd(psShmHead,"screenquality","50"));
				status = 1;
				
				//将共享内存中compid的 “控制命令”置0
				nwCompIp * psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
				if(!psCompIp){
					recodeLogInfo("接收实时屏幕：","?刂泼钪?失败", mesg);	
				}else{
					psCompIp->lCtrl=0;
				}
			}
		}
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "status",   UT_TYPE_LONG,   status,
					 "quality",  UT_TYPE_LONG,   quality
					);

	free(data);
	return 0;

}

int Lan_Online_Proclist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	unsigned int compid = 0;
	unsigned int userid = 0;
	unsigned int status = 0;
	int	     nums = 0;
	char         mac[24];
	char 	     ip[24];
	ProcessInfo  *myProclist = NULL;

	int	     iReturn = 0;
	int   	     num = 0;
	char         tempPath[1024];
	char         destPath[1024];
	char	     str[10][24];

	memset(mac, 0, sizeof(mac));
	memset(ip, 0, sizeof(ip));
	memset(tempPath, 0, sizeof(tempPath));
	memset(destPath, 0, sizeof(destPath));
	memset(str, 0, 10*24);


	//recodeLogInfo();


	iReturn = utMsgGetSomeNVar(psMsgHead,5,
		                     "compid",   UT_TYPE_LONG,  4, &compid,
				     "userid", 	UT_TYPE_LONG, 4, &userid,
				     "mac",  	UT_TYPE_STRING,  sizeof(mac)-1, mac,
				     "ip",      UT_TYPE_STRING, sizeof(ip)-1, ip,
				     "num",     UT_TYPE_LONG,   4, &nums
				    );

	
	if((iReturn !=5) ||
	   (compid == 0) ||
	   (strlen(mac) == 0) ||
	   (strlen(ip) == 0) ||
	   (nums <= 0)){
		char mesg[512];
		memset(mesg, 0, sizeof(mesg));
		snprintf(mesg, sizeof(mesg)-1, "iRetrun=%d, compid=%u, userid=%u, mac=%s, ip=%s, nums=%d",iReturn,compid, userid, mac, ip,nums);
		recodeLogInfo("接收实时进程信息："," ERROR: ", mesg);

		status = 2;
	}else{
		myProclist = (ProcessInfo *)malloc(nums*sizeof(struct _processInfo));
		if(NULL == myProclist){
			recodeLogInfo("接收实时进程信息：", " 动态开辟空间失败！", "");

			status = -3;
		}else{
			//创建临时存储文件
			snprintf(tempPath, sizeof(tempPath)-1, "%s%u.proc.bak", ONLINE_PROC_DIR, compid);
			snprintf(destPath, sizeof(destPath)-1, "%s%u.proc", ONLINE_PROC_DIR, compid);
	   		FILE * fp = fopen(tempPath, "wb");
			if(!fp){
				snprintf(tempPath, sizeof(tempPath)-1, "%s", ONLINE_DIR);
				mkdir(tempPath,S_IRWXG);
				snprintf(tempPath, sizeof(tempPath)-1, "%s", ONLINE_PROC_DIR);
				mkdir(tempPath,S_IRWXG);

				snprintf(tempPath, sizeof(tempPath)-1, "%s%u.proc.bak", ONLINE_PROC_DIR, compid);
				fp = fopen(tempPath, "w");
			}

			if(!fp){
				recodeLogInfo("接收实时进程信息：", " 创建临时文件失败", "");
				status = 2;
			}else{
				for(num = 0; num < nums; num ++){
					snprintf(str[0], 50, "StartTime%d", num );
					snprintf(str[1], 50, "name%d", num );		
					snprintf(str[2], 50, "path%d", num );
					snprintf(str[3], 50, "content%d", num );
					snprintf(str[4], 50, "indexid%d", num );
					snprintf(str[5], 50, "pindexid%d", num );
					snprintf(str[6], 50, "pid%d", num );
					snprintf(str[7], 50, "cpuusage%d", num );
					snprintf(str[8], 50, "cpus%d", num );		
					snprintf(str[9], 50, "mem%d", num );

					iReturn = utMsgGetSomeNVar(psMsgHead,10,
						str[0],  UT_TYPE_LONG,4, &(myProclist[num].StartTime),
						str[1],  UT_TYPE_STRING,   sizeof(myProclist[num].name)-1,  myProclist[num].name,
						str[2], UT_TYPE_STRING,  sizeof(myProclist[num].path)-1,  myProclist[num].path,
						str[3], UT_TYPE_STRING, sizeof(myProclist[num].content)-1,myProclist[num].content,
						str[4], UT_TYPE_STRING,   sizeof(myProclist[num].indexid)-1,  myProclist[num].indexid,
						str[5], UT_TYPE_STRING, sizeof(myProclist[num].pindexid)-1, myProclist[num].pindexid,
						str[6], UT_TYPE_LONG,   4, &(myProclist[num].pid),
						str[7], UT_TYPE_LONG,   4,  &(myProclist[num].cpuusage),
						str[8],  UT_TYPE_LONG, 4, &(myProclist[num].cpus),
						str[9],  UT_TYPE_LONG, 4, &(myProclist[num].mem)
						);
					myProclist[num].compid = compid;
					snprintf(myProclist[num].mac,  sizeof(myProclist[num].mac)-1, "%s", mac);
					snprintf(myProclist[num].ip,   sizeof(myProclist[num].ip)-1, "%s", ip);
					int x = 0;
					int    y =  strlen(myProclist[num].path);
					for(x = 0; x < y; x++){
						if(myProclist[num].path[x] == '\\'){
							myProclist[num].path[x] = '/';
						}
					}
				}

				for(num = 0;  num < nums; num++){
					fprintf(fp, "%u^%s^%s^%u^%s^%s^%s^%s^%s^%d^%d^%d^%d\n", 
						    myProclist[num].compid, myProclist[num].mac, myProclist[num].ip ,     myProclist[num].StartTime,
						    myProclist[num].name,   myProclist[num].path,myProclist[num].content, myProclist[num].indexid, myProclist[num].pindexid,
						    myProclist[num].pid,    myProclist[num].cpuusage,myProclist[num].cpus,myProclist[num].mem);					
				}
				fclose(fp);
				
				remove(destPath);
				rename(tempPath, destPath);
				remove(tempPath);

				nwCompIp *psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
				if(psCompIp){
					psCompIp->lCtrl=0;
					recodeLogInfo("实时进程：", "将指令置0成功！","");
				}

				status = 1;
				recodeLogInfo("实时进程：", "接收成功！","");
			}
		}
			

	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2,
					 "compid",   UT_TYPE_LONG,   compid,
					 "status",   UT_TYPE_LONG,   status
					);


	return 0;

}

//指令处理结果反馈
int  Lan_CommandReturn(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	recodeLogInfo("反馈指令", "----------------------------------------", "");	

	unsigned int  compid = 0;
	int           command = 0;
	int           status  = 0; 
	int           iReturn = 0;

	iReturn = utMsgGetSomeNVar(psMsgHead,3,
		                     "compid",     UT_TYPE_ULONG,  4, &compid,
					"command",     UT_TYPE_LONG,  4, &command,
					"status",   UT_TYPE_LONG, 4,  &status					
					);

	if( compid != 0){
		//将共享内存中compid的 “控制命令”置0
		nwCompIp * psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
		if(!psCompIp){
			recodeLogInfo("将服务器控制命令置0：", " 置0失败 ", "");	
			status = 2;
		}else{
			psCompIp->lCtrl=0;
			status = 1;
			if((0X00000800 == command) ||  //如果是重启指令
			  (0X00001000 == command) ||   //如果是关机指令
			 (0X00002000 == command)){   //如果是卸载客户端指令
			  
			  pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "command",UT_TYPE_LONG, command,
					 "status",   UT_TYPE_LONG,   status
					);
					
			 	sleep(3);
				utShmHashDel(psShmHead,NC_LNK_ONLINECOMP,psCompIp);
				return 0;
			}
		}
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "command",UT_TYPE_LONG, command,
					 "status",   UT_TYPE_LONG,   status
					);

	

	return 0;
}

//计算机处理结果反馈
int  Lan_Unsetup_cmd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	unsigned int  compid = 0;
	int           command = 0;
	int           status  = 0; 
	int           iReturn = 0;

	iReturn = utMsgGetSomeNVar(psMsgHead,3,
		                     "compid",     UT_TYPE_ULONG,  4, &compid,
					"command",     UT_TYPE_LONG,  4, &command,
					"status",   UT_TYPE_LONG, 4,  &status					
					);

	if( compid != 0){
		//将共享内存中compid的 “控制命令”置0
		nwCompIp * psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
		if(!psCompIp){
			recodeLogInfo("将服务器控制命令置0：", " 置0失败 ", "");	
			status = 2;
		}else{
			psCompIp->lCtrl=0;
			status = 1;
			utShmHashDel(psShmHead,NC_LNK_ONLINECOMP,psCompIp);
		}
	}
	
	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "command",UT_TYPE_LONG, command,
					 "status",   UT_TYPE_LONG,   status
					);

	

	return 0;
}

//实时抓屏 获取IP和Port
int Lan_Request_IPAndPort(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	unsigned long compid = 0;
	unsigned long status = 0;
	char port[8] = "";
	char ip[16] = "";
	int iReturn = 0;
	
	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		"compid",   UT_TYPE_ULONG,  4, &compid,
		"status", 	UT_TYPE_ULONG, 	4, &status);

		char tmp[16] = "";
		sprintf(tmp,"%u",compid);
	if(iReturn == 2){
		char compidVar[24] = "";
		
		nwCompIp *psCompIp = NULL;
		psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
		
		if(psCompIp){
			printf("lctr:%u\t command:%u\n",status,PRO_CENTER_COMMANDID_CAPTURESCREEN);
			if(status == PRO_CENTER_COMMANDID_CAPTURESCREEN){
				char compidVar[24] = "";
		
				snprintf(compidVar,sizeof(compidVar),"ak%u",compid);
				utComSetVar_s(psShmHead,compidVar,"receive");
			}
			psCompIp->lCtrl=0;
		}
		
		snprintf(compidVar,sizeof(compidVar),"ssip%u",compid);
		strcpy(ip,utComGetVar_sd(psShmHead,compidVar,""));	
		
		printf("ssip:%s\n",ip);
		
		utComSetVar_s(psShmHead,compidVar,"");	
		
		snprintf(compidVar,sizeof(compidVar),"sspo%u",compid);
		strcpy(port,utComGetVar_sd(psShmHead,compidVar,""));
		
		printf("sspo:%s\n",port);
		utComSetVar_s(psShmHead,compidVar,"");
		
		
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */4,
		 "compid",   UT_TYPE_ULONG,   compid,
		 "status",   UT_TYPE_ULONG,   status,
		 "Ip",			 UT_TYPE_STRING,	ip,
		 "Port",		 UT_TYPE_ULONG,		strtoul(port,NULL,10));
	}
	
	return 0;	
}

int Lan_CommandReturn_RemoteCtrl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	unsigned long compid = 0;
	unsigned long status = 0;
	unsigned long command = 0;
	
	int iReturn = 0;
	
	iReturn = utMsgGetSomeNVar(psMsgHead,3,
		"compid",   UT_TYPE_ULONG,  4, &compid,
		"command", 	UT_TYPE_ULONG, 	4, &command,
		"status", 	UT_TYPE_ULONG, 	4, &status);
		
	if(iReturn == 3){
		char compidVar[24] = "";
		
		snprintf(compidVar,sizeof(compidVar),"ac%u",compid);
		if(status == 1){		
			utComSetVar_s(psShmHead,compidVar,"receive");
		}else{
			utComSetVar_s(psShmHead,compidVar,"noreceive");
		}
		
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */3,
			 "compid",   UT_TYPE_ULONG,   compid,
			 "command",   UT_TYPE_ULONG,   command,
			 "return",	 UT_TYPE_ULONG,	  1);
	}
	
	return 0;
}

//进程处理 获取进程的id号
int Lan_Request_Pid(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	unsigned long compid = 0;
	unsigned long status = 0;
	int iReturn = 0;
	
	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		"compid",   UT_TYPE_ULONG,  4, &compid,
		"status", 	UT_TYPE_ULONG, 4, &status);
		
	if(iReturn == 2){
		char compidVar[24] = "";
		
		nwCompIp *psCompIp = NULL;
		psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
		
		if(psCompIp){
			psCompIp->lCtrl=0;
		}
		snprintf(compidVar,sizeof(compidVar),"proc%u",compid);
		
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */3,
		 "compid",   UT_TYPE_ULONG,   compid,
		 "status",   UT_TYPE_ULONG,   status,
		 "pid",			 UT_TYPE_ULONG,	  strtoul(utComGetVar_sd(psShmHead,compidVar,""),NULL,10)
		);
		utComSetVar_s(psShmHead,compidVar,"");
	}				
	return 0;

}

//服务处理 获取服务的名称
int Lan_Request_ServiceName(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	unsigned long compid = 0;
	unsigned long status = 0;
	int iReturn = 0;
	
	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		"compid",   UT_TYPE_ULONG,  4, &compid,
		"status", 	UT_TYPE_ULONG, 4, &status);

	if(iReturn == 2){
		char compidVar[24] = "";
		nwCompIp *psCompIp = NULL;
		psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
		
		if(psCompIp){
			psCompIp->lCtrl=0;
		}
		
		snprintf(compidVar,sizeof(compidVar),"ser%u",compid);
		
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */3,
		 "compid",   UT_TYPE_ULONG,   compid,
		 "status",   UT_TYPE_ULONG,   status,
		 "name",		 UT_TYPE_STRING,	utComGetVar_sd(psShmHead,compidVar,"")
		);
		utComSetVar_s(psShmHead,compidVar,"");
	}				
	return 0;

}

//根据IP判断是否在线
int Lan_Request_Online(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	unsigned long ip_in = 0;
	
	int iReturn = utMsgGetSomeNVar(psMsgHead,1,
		"ip",   UT_TYPE_ULONG,  4, &ip_in);
	
	unsigned long compid = 0;

	pasDbOneRecord("select compid from nwcompute where ip=:ip",1,"ip",UT_TYPE_ULONG,ip_in,UT_TYPE_ULONG,4,&compid);
	
	unsigned long status = 0;
	
	nwCompIp *psCompIp_tmp = NULL;
	
	psCompIp_tmp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);	
	
	if(psCompIp_tmp != NULL){
		status = 1;
	}else{
		status = 2;
	}
	
	//status:	1	online;		2 offline
	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */1,
		 "status",   UT_TYPE_ULONG,   status);
	
	return 0;
}

//根据IP获取客户端信息
int Lan_Request_ClientInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	unsigned long 	ip_in = 0;
	unsigned char	company[128]; //公司简称
	unsigned int	groupid = 0;
	unsigned char  	compname[32]; //客户端机器名
	unsigned char  	os[32];	      //客户?瞬僮飨低?
	unsigned char  	version[24];  //客户端版本号
    	unsigned char  	mac[24]; 	//客户端mac地址
	unsigned int    addtime;		//客户端安装时间
	unsigned char	*buffer = NULL;
	int	 	myGroupid[512]={0};
	int		number = 0;
	
	memset(company, 0, sizeof(company));
	memset(compname, 0, sizeof(compname));
	memset(os, 0, sizeof(os));
	memset(version, 0, sizeof(version));
	memset(mac, 0, sizeof(mac));


	int iReturn = utMsgGetSomeNVar(psMsgHead,2,
		"ip",   UT_TYPE_ULONG,  4, &ip_in,
		"company", UT_TYPE_STRING, sizeof(company)-1, company);
	
	unsigned long compid = 0;

	pasDbOneRecord("select compid from nwcompute where ip=:ip",1,"ip",UT_TYPE_ULONG,ip_in,UT_TYPE_ULONG,4,&compid);

	unsigned long status = 0;
	
	nwCompIp *psCompIp_tmp = NULL;
	
	psCompIp_tmp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
	if(psCompIp_tmp != NULL){
		
		snprintf(company+strlen(company),  sizeof(company)-strlen(company)-1, "admin");
		iReturn = pasDbOneRecord("select groupid from dsuser where name=:name",1,"name",UT_TYPE_STRING,company,UT_TYPE_ULONG,4,&groupid);
		buffer = getGroupid(groupid);
		
		char mesg[512];
		memset(mesg, 0, sizeof(mesg));

		snprintf(mesg, sizeof(mesg)-1, "    iReturn :%d, groupid:%d, buffer:%s", iReturn, groupid, buffer );
		recodeLogInfo("GetInfo..............................", company, mesg);

		char * value = strtok(buffer, ",\0");
		while(value){
			myGroupid[number++] = atoi(value);
			value = strtok(NULL, ",\0");
		}
		
		snprintf(mesg, sizeof(mesg)-1, "  iReturn :%d, groupid:%d, number:%d", iReturn, groupid, number);
		recodeLogInfo("GetInfo..............................", company, mesg);
		

		int num = 0;
		for(num = 0; num < number; num++){
			snprintf(mesg, sizeof(mesg)-1, "  number:%d", num);
			recodeLogInfo("GetInfo..............................", company, mesg);

			iReturn =  pasDbOneRecord("select compname,os, addtime, version,mac from nwcompute where ip=:ip and groupid=:groupid",2,
				"ip",UT_TYPE_ULONG,ip_in,
				"groupid",UT_TYPE_ULONG,myGroupid[num],
       				UT_TYPE_STRING,sizeof(compname)-1,compname,
       				UT_TYPE_STRING,sizeof(os)-1,os,
				UT_TYPE_LONG,sizeof(addtime),&addtime,
       				UT_TYPE_STRING,sizeof(version)-1,version,
       				UT_TYPE_STRING,sizeof(mac)-1,mac);

			if(iReturn == 0){
				status = 1;
				//status:	1	online;		2 offline
				pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 6,
		 			"status",   UT_TYPE_ULONG,   status,
					"addtime",   UT_TYPE_ULONG,  addtime,
					"compname",UT_TYPE_STRING, compname,
					"os",	UT_TYPE_STRING, os,
					"version", UT_TYPE_STRING, version,
					"mac",  UT_TYPE_STRING, mac);

				snprintf(mesg, sizeof(mesg)-1, "找到一条信息，iReturn:%d, groupid:%d, compname:%s", iReturn, myGroupid[num], compname);
				recodeLogInfo("GetInfo..............................", company, mesg);
				return 0;
			}
		}
				
	}	
	
	status = 2;
	//status:	1	online;		2 offline
	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */1,
		 "status",   UT_TYPE_ULONG,   status);
	
	return 0;
}



int ncInitNwWebFun_OnlineInfo(utShmHead *psShmHead)
{
	pasSetTcpFunName("Lan_Online_Screen",  Lan_Online_Screen,  NULL,0,0);
	pasSetTcpFunName("Lan_Unsetup_cmd",  Lan_Unsetup_cmd,  NULL,0,0);
	pasSetTcpFunName("Lan_Online_Proclist",  Lan_Online_Proclist,  NULL,0,0);
	pasSetTcpFunName("Lan_CommandReturn",     Lan_CommandReturn,    NULL,0,0);
	pasSetTcpFunName("Lan_Request_Pid",     Lan_Request_Pid,    NULL,0,0);
	pasSetTcpFunName("Lan_Request_ServiceName",   Lan_Request_ServiceName,    NULL,0,0);
	pasSetTcpFunName("Lan_Request_IPAndPort",     Lan_Request_IPAndPort,    NULL,0,0);
	pasSetTcpFunName("Lan_CommandReturn_RemoteCtrl",     Lan_CommandReturn_RemoteCtrl,    NULL,0,0);
	pasSetTcpFunName("Lan_Request_Online",     Lan_Request_Online,    NULL,0,0);
	pasSetTcpFunName("Lan_Request_ClientInfo",     Lan_Request_ClientInfo,    NULL,0,0);

	
	return 0;
}