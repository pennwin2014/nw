#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "nwdef.h"
#include "ncdef.h"

//Just for test
#define HARD_INFO 1
#define BASE_INFO 1
#define SOFT_INFO 0
#define SERVICE_INFO 1
#define DISK_INFO 1

#define     PRO_CENTER_COMMANDID_STARTSERVICE   0x00008000      //启动某个服务
#define     PRO_CENTER_COMMANDID_STOPSERVICE    0x00010000      //停止某个服务
#define     PRO_CENTER_COMMANDID_REFRESHSERVICE 0x00020000   //刷新服务

//计算机基本信息
int lan_RTC_base_info_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){

#if BASE_INFO
		utMsgPrintMsg(psMsgHead);
#endif

		char compid_in[16] = "";
		utMsgGetSomeNVar(psMsgHead,1,"compId",UT_TYPE_STRING,sizeof(compid_in)-1,compid_in);
		
		if(!utStrIsSpaces(compid_in)){
			
				printf("%s\n",compid_in);
				char sql[1024] = "";
				pasDbCursor *psCur = NULL;
		    utPltDbHead *psDbHead = utPltInitDb();
		    snprintf(sql,sizeof(sql),"select compid,devname,gname,userid,inet_ntoa(ip),compname,mac,os,ie,addtime,lasttime,version,authkey,status,htime,stime,ctime from nwcompute left join nwgroup on groupid=gid where compid = %s",compid_in);
		
				//权限
				char *auth = getDsGroupid();
				if(!utStrIsSpaces(auth)){
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)," and groupid in (%s)",auth);
				}
		
#if BASE_INFO
				printf("sql:%s\n",sql);
#endif	
		
		    psCur = pasDbOpenSql(sql, 0);
				if(psCur != NULL){
		  			unsigned int compid = 0;
		  			char devname[32] = "";
		  			char groupName[32] = "";
		  			unsigned int userid = 0;
		  			char ip[16] = "";
		  			char compname[32] = "";
		  			char mac[18] = "";
		  			char os[32] = "";
		  			char ie[32] = "";
		  			unsigned int addtime = 0;
		  			unsigned int lasttime = 0;
		  			char version[16] = "";
		  			char authkey[24] = "";
		  			int status = 0;
		  			unsigned int htime = 0;
		  			unsigned int stime = 0;
		  			unsigned int ctime = 0;
		   			
		   			pasDbFetchInto(psCur,
		  					UT_TYPE_ULONG,	4,								&compid,
		  					UT_TYPE_STRING,	sizeof(devname)-1,devname,
		  					UT_TYPE_STRING,	sizeof(groupName)-1,groupName,
		  					UT_TYPE_ULONG,	4,								&userid,
								UT_TYPE_STRING,	sizeof(ip)-1,			ip,
								UT_TYPE_STRING,	sizeof(compname)-1,compname,
								UT_TYPE_STRING,	sizeof(mac)-1,		mac,
								UT_TYPE_STRING,	sizeof(os)-1,			os,
								UT_TYPE_STRING,	sizeof(ie)-1,			ie,
		  					UT_TYPE_ULONG,	4,								&addtime,
		  					UT_TYPE_ULONG,	4,								&lasttime,
								UT_TYPE_STRING,	sizeof(version)-1,version,
								UT_TYPE_STRING,	sizeof(authkey)-1,authkey,
		  					UT_TYPE_LONG,		4,								&status,
		  					UT_TYPE_ULONG,	4,								&htime,
		  					UT_TYPE_ULONG,	4,								&stime,
		  					UT_TYPE_ULONG,	4,								&ctime);
		  					
						int iNum = 1;
			 			
						utPltPutLoopVarF(psDbHead,"compid",iNum,"%u",compid);
						utPltPutLoopVar(psDbHead,"devname",iNum,devname);
						utPltPutLoopVar(psDbHead,"groupid",iNum,groupName);
						utPltPutLoopVarF(psDbHead,"userid",iNum,"%u",userid);
						utPltPutLoopVar(psDbHead,"ip",iNum,ip);
						utPltPutLoopVar(psDbHead,"compname",iNum,compname);
						utPltPutLoopVar(psDbHead,"mac",iNum,mac);
						utPltPutLoopVar(psDbHead,"os",iNum,os);
						utPltPutLoopVar(psDbHead,"ie",iNum,ie);
						utPltPutLoopVar(psDbHead,"addtime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S", addtime));
						utPltPutLoopVar(psDbHead,"lasttime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S", lasttime));
						utPltPutLoopVar(psDbHead,"version",iNum,version);
						utPltPutLoopVar(psDbHead,"authkey",iNum,authkey);
						utPltPutLoopVarF(psDbHead,"status",iNum,"%d",status);
						if(htime > 80000){
							utPltPutLoopVar(psDbHead,"htime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S", htime));
						}
						if(stime > 80000){
							utPltPutLoopVar(psDbHead,"stime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S", stime));
						}
						if(ctime > 80000){
							utPltPutLoopVar(psDbHead,"ctime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S", ctime));
			 			}
		
					
		  			pasDbCloseCursor(psCur);
		  			
		#if BASE_INFO
		  			utPltShowDb(psDbHead);
		#endif  			
		
						utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_comptue_base_info.htm");
						if(atoi(compid_in) != 0){
							char msg[256] = "";
							char *compName = findCompNameById(strtoul(compid_in,NULL,10));
							snprintf(msg,sizeof(msg),"查询[%s]计算机信息",compName);
							free(compName);
		 					writeSysLog(psMsgHead,"06","Success",msg);
		 				}
		  	}
	  }  
		return 0;
}

//计算机硬件信息
int lan_RTC_hard_info_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){

#if HARD_INFO
		utMsgPrintMsg(psMsgHead);
#endif
		
		char compid_in[16] = "";
		
		utMsgGetSomeNVar(psMsgHead,1,"compId",UT_TYPE_STRING,sizeof(compid_in)-1,compid_in);

		if(!utStrIsSpaces(compid_in)){
				char sql[1024] = "";
				pasDbCursor *psCur = NULL;
		    utPltDbHead *psDbHead = utPltInitDb();
		    
		    //权限
				char *auth = getDsGroupid();
				printf("auth:%s\n",auth);
				if(!utStrIsSpaces(auth)){
					snprintf(sql,sizeof(sql),"select sid,hardType,deviceDesc from nwhardware where compid = %s and compid in (select compid from nwcompute where groupid in(%s))and InfoType = 'hardtype' order by hardType",compid_in,auth);
				}else{
					snprintf(sql,sizeof(sql),"select sid,hardType,deviceDesc from nwhardware where compid = %s and InfoType = 'hardtype' order by hardType",compid_in);				
				}
				
				
		#if HARD_INFO		
				printf("sql：%s\n",sql);
		#endif
		
				psCur = pasDbOpenSql(sql,0);
				int iNum = 0,jNum = 0;
				
				if(psCur != NULL){
						unsigned int sid = 0;
						char hardType[64] = "";
						char devDesc[512] = "";
						char hardType_tmp[64] = "";
						char icon[64] = "";
						while(0 == pasDbFetchInto(psCur,
								UT_TYPE_ULONG,4,&sid,
								UT_TYPE_STRING,	sizeof(hardType)-1,hardType,
								UT_TYPE_STRING,	sizeof(devDesc)-1,devDesc)){
								if(strcmp(hardType,hardType_tmp)){
										iNum++;
										if(iNum > 1){
												utPltPutLoopVar(psDbHead,"dhf",iNum,",");
										}
										jNum = 0;
										
										if(!strcmp(hardType,ncLang("0048软盘驱动器控制器"))){
											strcpy(icon,"floppyIcon");
										}else if(!strcmp(hardType,ncLang("0049鼠标和其他指针设备"))){
											strcpy(icon,"mouseIcon");
										}else if(!strcmp(hardType,ncLang("0050声音、视频和游戏控制器"))){
											strcpy(icon,"soundIcon");
										}else if(!strcmp(hardType,ncLang("0051IDE ATA/ATAPI 控制器"))){
											strcpy(icon,"IDEIcon");
										}else if(!strcmp(hardType,ncLang("0052显示适配器"))){
											strcpy(icon,"displayAdapterIcon");
										}else if(!strcmp(hardType,ncLang("0053通用串行总线控制器"))){
											strcpy(icon,"USBIcon");
										}else if(!strcmp(hardType,ncLang("0054系统设备"))){
											strcpy(icon,"systemIcon");
										}else if(!strcmp(hardType,ncLang("0055网络适配器"))){
											strcpy(icon,"netAdapterIcon");
										}else if(!strcmp(hardType,ncLang("0056磁盘驱动器"))){
											strcpy(icon,"DISKIcon");
										}else if(!strcmp(hardType,ncLang("0057处理器"))){
											strcpy(icon,"CPUIcon");
										}else if(!strcmp(hardType,ncLang("0058端口 (COM 和 LPT)"))){
											strcpy(icon,"COMIcon");
										}else if(!strcmp(hardType,ncLang("0059监视器"))){
											strcpy(icon,"displayIcon");
										}else if(!strcmp(hardType,ncLang("0060键盘"))){
											strcpy(icon,"keyboardIcon");
										}else if(!strcmp(hardType,ncLang("0061计算机"))){
											strcpy(icon,"computeIcon");
										}else if(!strcmp(hardType,ncLang("0062存储卷"))){
											strcpy(icon,"storeIcon");
										}else if(!strcmp(hardType,ncLang("0063人体学输入设备"))){
											strcpy(icon,"humanInputIcon");
										}else{
											strcpy(icon,"otherIcon");
										}
										
										
										utPltPutLoopVar(psDbHead,"hardType",iNum,hardType);
										utPltPutLoopVar(psDbHead,"iconCls1",iNum,icon);
										
										strcpy(hardType_tmp,hardType);
								}
								jNum++;
								if(jNum > 1){
										utPltPutLoopVar2(psDbHead,"dhs",iNum,jNum,",");
								}
								utPltPutLoopVar2(psDbHead,"devDesc",iNum,jNum,devDesc);
								utPltPutLoopVar2(psDbHead,"iconCls2",iNum,jNum,icon);
						}
						pasDbCloseCursor(psCur);
						utPltShowDb(psDbHead);
						utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_comp_hard.htm");
						if(!utStrIsSpaces(compid_in)){
							char msg[256] = "";
							char *compname = findCompNameById(strtoul(compid_in,NULL,10));
							snprintf(msg,sizeof(msg),"查询[%s]电脑硬件信息",compname);
							free(compname);
		 					writeSysLog(psMsgHead,"06","Success",msg);
		 				}
				}
		}

		return 0;
}

//计算机软件信息
int lan_comp_soft_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
#if SOFT_INFO	
		utMsgPrintMsg(psMsgHead);
#endif	
	
		char comp_id_in[16] = "";
		char soft_class_in[8] = "";
		char group_id_in[16] = "";
		char keywords_in[64] = "";
		char start_in[8] = "";
		char limit_in[8] = "";
		
		utMsgGetSomeNVar(psMsgHead,6,
    		"compId",	UT_TYPE_STRING,  sizeof(comp_id_in)-1, 	comp_id_in,
    		"softClass",	UT_TYPE_STRING,  sizeof(soft_class_in)-1, 		soft_class_in,
    		"groupId",	UT_TYPE_STRING,  sizeof(group_id_in)-1, 		group_id_in,
    		"keywords",	UT_TYPE_STRING,  sizeof(keywords_in)-1, 		keywords_in,
    		"start",	UT_TYPE_STRING,  sizeof(start_in)-1, 	start_in,
    		"limit",		UT_TYPE_STRING,  	 sizeof(limit_in)-1, 	limit_in);
    
    char sql[1024] = "";
    char sql_tmp[1024] = "";
    char comp_id_group[128] = "";
    pasDbCursor *psCur = NULL;
    
       
    if(!utStrIsSpaces(group_id_in)){
		    snprintf(sql,sizeof(sql),"select compid from nwcompute where groupid in (%s)",getGroupid(strtoul(group_id_in,NULL,10)));
		
#if SERVICE_INFO	
		printf("comp_id_group_sql:%s\n",sql);
#endif 		    
		    psCur = pasDbOpenSql(sql,0);
		    if(psCur != NULL){
		    		int iNum = 0;
		    		int iret = -1;
		    		unsigned int comp_id = 0;
		    		
		    		while(0==(iret = pasDbFetchInto(psCur,
		    			UT_TYPE_ULONG, 4, &comp_id)) || 1405 == iret){
								if(iNum>0){
										strcat(comp_id_group,",");
								}
								snprintf(comp_id_group+strlen(comp_id_group),sizeof(comp_id_group)-strlen(comp_id_group),"%lu",comp_id);
								iNum++;
		 				}
		 				pasDbCloseCursor(psCur);
		 				
		 				if(iNum == 0){
		 					strcat(comp_id_group,"0");
		 				}
		 				
		 				snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp), " and compid in (%s)", comp_id_group);
		    }
	  }
	  
	   //权限
		char *auth = getDsGroupid();
		if(!utStrIsSpaces(auth)){
			snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp), " and compid in (select compid from nwcompute where groupid in(%s))", auth);	
		}
   
		if(!utStrIsSpaces(comp_id_in)){
			snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp), " and compid = %s", comp_id_in);		
		}		
		
		if(!utStrIsSpaces(soft_class_in)){
			snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp), " and stype = %s", soft_class_in);		
		}	
		
		if(!utStrIsSpaces(keywords_in)){
			char *keywords_gbk = utf8convert(psShmHead,keywords_in);
			snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp), " and (sname like '%%%s%%' or stype like '%%%s%%' or demo like '%%%s%%' or business like '%%%s%%')",keywords_gbk,keywords_gbk,keywords_gbk,keywords_gbk);		
		}		

		utPltDbHead *psDbHead = utPltInitDb();
		int db_count = 0;
	
		snprintf(sql,sizeof(sql),"select count(*) from nwsoftware where 1=1 %s",sql_tmp);

		psCur = pasDbOpenSql(sql,0);
    pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&db_count);
    pasDbCloseCursor(psCur);
		utPltPutVarF(psDbHead,"TotRec","%lu",db_count);	
		
		snprintf(sql,sizeof(sql),"select sname,ssize,version,instime,addtime,lasttime,demo,business,stype from nwsoftware where 1=1 %s limit %u,%u",sql_tmp,atoi(start_in),atoi(limit_in));
 
		writeLog("compute.txt","software",sql);
    psCur = pasDbOpenSql(sql, 0);
    
    if(psCur != NULL){
    		char soft_name[72] = "";
    		char soft_nameT[72] = "";
    		unsigned long long soft_size = 0;  
    		char version[16] = "";
    		unsigned long inst_time=0;
    		unsigned long add_time=0;
    		unsigned long last_time=0;
    		char business[50] = "";
    		char info_type[24] = "";
    		char demo[256] = "";
    		
    		int iret = -1;
    		int iNum = 0;
    		
    		while(0==(iret = pasDbFetchInto(psCur,
				 UT_TYPE_STRING, sizeof(soft_name), soft_name,
				 UT_TYPE_ULONG,  sizeof(soft_size), &soft_size,
				 UT_TYPE_STRING, sizeof(version), version,
				 UT_TYPE_ULONG,  sizeof(inst_time), &inst_time,
				 UT_TYPE_ULONG,  sizeof(add_time),  &add_time,
				 UT_TYPE_ULONG,  sizeof(last_time),	&last_time,
				 UT_TYPE_STRING, sizeof(demo), demo,
				 UT_TYPE_STRING, sizeof(business), business,
				 UT_TYPE_STRING, sizeof(info_type), info_type))|| 1405 == iret){
						iNum++;
						if(iNum>1){
								utPltPutLoopVar(psDbHead,"dh",iNum,",");
						}
						char sotf_size_str[8] = "";
						if(soft_size/1000 == 0){
							snprintf(sotf_size_str,sizeof(sotf_size_str),"%uB",soft_size);
						}else if(soft_size/100000 == 0){
							snprintf(sotf_size_str,sizeof(sotf_size_str),"%.1fKB",soft_size/1000.0);
						}else if(soft_size/100000000 == 0){
							snprintf(sotf_size_str,sizeof(sotf_size_str),"%.1fMB",soft_size/1000000.0);
						}else if(soft_size/100000/1000000 == 0){
							snprintf(sotf_size_str,sizeof(sotf_size_str),"%.1fGB",soft_size/1000000000.0);
						}
						
						utPltPutLoopVar(psDbHead,"softSize",iNum,sotf_size_str);
						pasStrCvtUcode(soft_name,soft_nameT);
						utPltPutLoopVar(psDbHead,"softName",iNum,soft_nameT);
						utPltPutLoopVar(psDbHead,"version",iNum,version);
						utPltPutLoopVar(psDbHead,"instTime",iNum,inst_time==0?"":utTimFormat("%Y/%m/%d", inst_time));
						utPltPutLoopVar(psDbHead,"addTime",iNum,add_time==0?"":utTimFormat("%Y/%m/%d", add_time));
						utPltPutLoopVar(psDbHead,"lastTime",iNum,last_time==0?"":utTimFormat("%Y/%m/%d", last_time));
						utPltPutLoopVar(psDbHead,"demo",iNum,demo);
						utPltPutLoopVar(psDbHead,"business",iNum,business);
						utPltPutLoopVar(psDbHead,"infoType",iNum,info_type);
				}
				pasDbCloseCursor(psCur);
    }
    
#if SOFT_INFO	
		utPltShowDb(psDbHead);
#endif

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_comp_soft.htm");
    char msg[256] = "";
	 
		if(!utStrIsSpaces(group_id_in)){
			snprintf(msg,sizeof(msg),"查询[%s]电脑软件信息",lanGetGroup(psShmHead,atoi(group_id_in)));
		}else if(!utStrIsSpaces(comp_id_in)){
			char *compname = findCompNameById(strtoul(comp_id_in,NULL,10));
			snprintf(msg,sizeof(msg),"查询[%s]电脑软件信息",compname);
			free(compname);
		}else {
			snprintf(msg,sizeof(msg),"查询[全部]电脑软件信息");
		}
	
		return 0;
}

//计算机服务信息
int lan_comp_service_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
#if SERVICE_INFO	
		utMsgPrintMsg(psMsgHead);
#endif	
	
		char comp_id_in[16] = "";
		char group_id_in[16] = "";
		char keywords_in[64] = "";
		char serverName_in[74] = "";
		char deal_in[2] = "";
		char start_in[8] = "";
		char limit_in[8] = "";
		
		utMsgGetSomeNVar(psMsgHead,7,
    		"compId",	UT_TYPE_STRING,  sizeof(comp_id_in)-1, 	comp_id_in,
    		"groupId",	UT_TYPE_STRING,  sizeof(group_id_in)-1, 	group_id_in,
    		"keywords",	UT_TYPE_STRING,  sizeof(keywords_in)-1, 	keywords_in,
    		"serverName",	UT_TYPE_STRING,  sizeof(serverName_in)-1, 	serverName_in,
    		"deal",	UT_TYPE_STRING,  sizeof(deal_in)-1, 	deal_in,
    		"start",	UT_TYPE_STRING,  sizeof(start_in)-1, 	start_in,
    		"limit",		UT_TYPE_STRING,  	 sizeof(limit_in)-1, 	limit_in);
    
    if(!utStrIsSpaces(serverName_in)){	
		  char compidVar[24] = "";
			snprintf(compidVar,sizeof(compidVar),"ser%s",comp_id_in);
			
			utComSetVar_s(psShmHead,compidVar,serverName_in);
			
			unsigned int compid = strtoul(comp_id_in,NULL,10);
			nwCompIp *psCompIp = NULL;
			psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
			
			if(psCompIp){
				switch(atoi(deal_in)){
					case 1: //停止
						psCompIp->lCtrl=PRO_CENTER_COMMANDID_STOPSERVICE;
						break;
					case 2:
						psCompIp->lCtrl=0;
						break;
					case 3: //启动
						psCompIp->lCtrl=PRO_CENTER_COMMANDID_STARTSERVICE;
						break;
					case 4: //刷新
						psCompIp->lCtrl=PRO_CENTER_COMMANDID_REFRESHSERVICE;
						break;
					default: ;
				}
			}
    }
    
    
    
	sleep(1);
    
    char sql[1024] = "";
    char sql_tmp[1024] = "";
    char comp_id_group[128] = "";
    pasDbCursor *psCur = NULL;
    
    
    if(!utStrIsSpaces(group_id_in)){
		    snprintf(sql,sizeof(sql),"select compid from nwcompute where groupid in (%s)",getGroupid(strtoul(group_id_in,NULL,10)));
		   
#if SERVICE_INFO	
		printf("comp_id_group_sql:%s\n",sql);
#endif 		    
		    psCur = pasDbOpenSql(sql,0);
		    if(psCur != NULL){
		    		int iNum = 0;
		    		int iret = -1;
		    		unsigned int comp_id = 0;
		    		
		    		while(0==(iret = pasDbFetchInto(psCur,
		    			UT_TYPE_ULONG, 4, &comp_id)) || 1405 == iret){
								if(iNum>0){
										strcat(comp_id_group,",");
								}
								snprintf(comp_id_group+strlen(comp_id_group),sizeof(comp_id_group)-strlen(comp_id_group),"%lu",comp_id);
								iNum++;
		 				}
		 				pasDbCloseCursor(psCur);
		 				
		 				if(iNum == 0){
		 					strcat(comp_id_group,"0");
		 				}
		 				
		 				snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp), " and compid in (%s)", comp_id_group);
		    }
	  }
	  
#if SERVICE_INFO	
		printf("comp_id_group:%s\n",comp_id_group);
#endif      
    
		if(!utStrIsSpaces(comp_id_in)){
			snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp), " and compid = %s", comp_id_in);		
		}

		if(!utStrIsSpaces(keywords_in)){
			char *keywords_gbk = utf8convert(psShmHead,keywords_in);
			snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp), " and (sname like '%%%s%%' or demo like '%%%s%%')", keywords_gbk,keywords_gbk);		
		}
		
		//权限
		char *auth = getDsGroupid();
		if(!utStrIsSpaces(auth)){
			snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp), " and compid in(select compid from nwcompute where groupid in(%s))", auth);
		}
		
		utPltDbHead *psDbHead = utPltInitDb();
		int db_count = 0;
	
		snprintf(sql,sizeof(sql),"select count(*) from nwservice where 1=1 %s",sql_tmp);

    writeLog("compute.txt","service",sql);

		psCur = pasDbOpenSql(sql,0);
    pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&db_count);
    pasDbCloseCursor(psCur);
		utPltPutVarF(psDbHead,"TotRec","%lu",db_count);	
		
		snprintf(sql,sizeof(sql),"select compid,sname,status,stype,log,demo,InfoType from nwservice where 1=1 %s limit %u,%u",sql_tmp,atoi(start_in),atoi(limit_in));
		
#if SERVICE_INFO	
		printf("%s\n",sql);
#endif  
		
    psCur = pasDbOpenSql(sql, 0);
    
    if(psCur != NULL){
    		unsigned int sid = 0;
    		char service_name[72] = "";
    		char status[20] = "";
    		char stype[20] = "";
    		char log[20] = "";
    		char info_type[24] = "";
    		char demo[1024] = "";
    		char demoT[1024] = "";
    		
    		int iret = -1;
    		int iNum = 0;
    		
    		
    		//mark
    		while(0==(iret = pasDbFetchInto(psCur,
    		 UT_TYPE_ULONG, 4, &sid,
				 UT_TYPE_STRING, sizeof(service_name), service_name,
				 UT_TYPE_STRING, sizeof(status), status,
				 UT_TYPE_STRING, sizeof(stype), stype,
				 UT_TYPE_STRING, sizeof(log), log,
				 UT_TYPE_STRING, sizeof(demo), demo,
				 UT_TYPE_STRING, sizeof(info_type), info_type)) || 1405 == iret){
						iNum++;
						if(iNum>1){
								utPltPutLoopVar(psDbHead,"dh",iNum,",");
						}
						utPltPutLoopVarF(psDbHead,"sid",iNum,"%u",sid);
						utPltPutLoopVar(psDbHead,"serviceName",iNum,service_name);
						utPltPutLoopVar(psDbHead,"status",iNum,status);
						utPltPutLoopVar(psDbHead,"serviceType",iNum,stype);
						//utPltPutLoopVar(psDbHead,"log",iNum,log);
						AdjuestTextBuf(demo);
						//pasStrCvtUcode(demo,demoT);
						utPltPutLoopVar(psDbHead,"demo",iNum,demo);
						utPltPutLoopVar(psDbHead,"infoType",iNum,info_type);
				}
				pasDbCloseCursor(psCur);
    }
    
#if SERVICE_INFO	
		//utPltShowDb(psDbHead);
#endif

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_comp_service.htm");
    char msg[256] = "";
	 
		if(!utStrIsSpaces(group_id_in)){
			snprintf(msg,sizeof(msg),"查询[%s]电脑服务信息",lanGetGroup(psShmHead,atoi(group_id_in)));
		}else if(!utStrIsSpaces(comp_id_in)){
			char *compname = findCompNameById(strtoul(comp_id_in,NULL,10));
			snprintf(msg,sizeof(msg),"查询[%s]电脑服务信息",compname);
			free(compname);
		}else {
			snprintf(msg,sizeof(msg),"查询[全部]电脑服务信息");
		}	
		return 0;
}

//计算机磁盘信息
int lan_comp_disk_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){

#if DISK_INFO
		utMsgPrintMsg(psMsgHead);
#endif
		
		char compid_in[16] = "";
		
		utMsgGetSomeNVar(psMsgHead,1,"compId",UT_TYPE_STRING,sizeof(compid_in)-1,compid_in);
		
		if(!utStrIsSpaces(compid_in)){
				char sql[1024] = "";
				char sql_tmp[64] = "";
				pasDbCursor *psCur = NULL;
		    utPltDbHead *psDbHead = utPltInitDb();
				snprintf(sql,sizeof(sql),"select hardType,deviceDesc from nwhardware where compid = %s and InfoType = 'LocalDisk'",compid_in);
				 //权限
				char *auth = getDsGroupid();
				if(!utStrIsSpaces(auth)){
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)," and compid in (select compid from nwcompute where groupid in(%s))",auth);
				}
		#if DISK_INFO	
				printf("sql：%s\n",sql);
		#endif
		
				psCur = pasDbOpenSql(sql,0);
				
				if(psCur != NULL){
						char hardType[64] = "";
						char devDesc[64] = "";
						char devDesc_tmp[64] = "";
						int iNum = 0;
						
						char *split1 = NULL;
						char *split2 = NULL;
						char *split3 = NULL;
						
						char *remain = NULL;
						char *use = NULL;
						
						char caNameBig5[32] = "";
						while(0 == pasDbFetchInto(psCur,
								UT_TYPE_STRING,	sizeof(hardType)-1,hardType,
								UT_TYPE_STRING,	sizeof(devDesc)-1,devDesc)){
									
								iNum++;
								if(iNum > 1){
										utPltPutLoopVar(psDbHead,"dh",iNum,",");
								}
								//memset(caNameBig5,0,sizeof(caNameBig5));
								//strcpy(caNameBig5,convert("GBK", "BIG5", caNameBig5));
								utPltPutLoopVar(psDbHead,"diskName",iNum,hardType);
								
								split1 = strtok(devDesc,",");								
								utPltPutLoopVar(psDbHead,"diskType",iNum,split1);
								split2 = strtok(NULL,",");
								split3 = strtok(NULL,",");
								snprintf(devDesc_tmp,sizeof(devDesc_tmp),"%s,%s",split2,split3);
								//memset(caNameBig5,0,sizeof(caNameBig5));
								//strcpy(caNameBig5,convert("GBK", "BIG5", devDesc_tmp));
								utPltPutLoopVar(psDbHead,"diskCapacity",iNum,devDesc_tmp);
								
								strtok(split2,":");
								remain = strtok(NULL,":");
								strtok(split3,":");
								use = strtok(NULL,":");
								
								printf("remain %s\n",remain);
								printf("use %s\n",use);
								if(atoi(use)==0){
									utPltPutLoopVarF(psDbHead,"remain",iNum,"%d",280);
								utPltPutLoopVarF(psDbHead,"use",iNum,"%d",0);
								}else{
									utPltPutLoopVarF(psDbHead,"remain",iNum,"%d",atoi(remain)*280/atoi(use));
									utPltPutLoopVarF(psDbHead,"use",iNum,"%d",(atoi(use)-atoi(remain))*280/atoi(use));
								}
						}
							pasDbCloseCursor(psCur);
							utPltShowDb(psDbHead);
							utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_comp_disk.htm");
							char msg[256] = "";
							char *compname = findCompNameById(strtoul(compid_in,NULL,10));
							snprintf(msg,sizeof(msg),"查询[%s]电脑硬碟信息",compname);
							free(compname);
		 					writeSysLog(psMsgHead,"06","Success",msg);							
				}
		}
		return 0;
}
//计算机网络信息
int lan_comp_net_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){

#if DISK_INFO
		utMsgPrintMsg(psMsgHead);
#endif
		
		char compid_in[16] = "";
		
		utMsgGetSomeNVar(psMsgHead,1,"compId",UT_TYPE_STRING,sizeof(compid_in)-1,compid_in);
		
		if(!utStrIsSpaces(compid_in)){
				char sql[1024] = "";
				char sql_tmp[64] = "";
				pasDbCursor *psCur = NULL;
		    utPltDbHead *psDbHead = utPltInitDb();
				snprintf(sql,sizeof(sql),"select hardType,deviceDesc from nwhardware where compid = %s and InfoType = 'Network'",compid_in);
				 //权限
			char *auth = getDsGroupid();
			if(!utStrIsSpaces(auth)){
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)," and compid in (select compid from nwcompute where groupid in(%s))",auth);
			}
		#if DISK_INFO	
				printf("sql：%s\n",sql);
		#endif
		
				psCur = pasDbOpenSql(sql,0);
				
				if(psCur != NULL){
						char hardType[64] = "";
						char devDesc[128] = "";
						char *netDesc = NULL;
						char *netType = NULL;
						char *netStatus = NULL;
						int iNum = 0;
						char caNameBig5[128] = "";
						while(0 == pasDbFetchInto(psCur,
								UT_TYPE_STRING,	sizeof(hardType)-1,hardType,
								UT_TYPE_STRING,	sizeof(devDesc)-1,devDesc)){
									
								iNum++;
								if(iNum > 1){
										utPltPutLoopVar(psDbHead,"dh",iNum,",");
								}
								utPltPutLoopVar(psDbHead,"netName",iNum,hardType);
								
								//memset(caNameBig5,0,sizeof(caNameBig5));
								//strcpy(caNameBig5,convert("GBK", "BIG5", devDesc));
								
								netDesc = strtok(devDesc,",");
								netType = strtok(NULL,",");
								netStatus = strtok(NULL,",");
								
								utPltPutLoopVar(psDbHead,"netDesc",iNum,netDesc);
								utPltPutLoopVar(psDbHead,"netType",iNum,netType);
								utPltPutLoopVar(psDbHead,"netStatus",iNum,netStatus);
								
						}
						pasDbCloseCursor(psCur);
						utPltShowDb(psDbHead);
						utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_comp_net.htm");
						char msg[256] = "";
						char *compname = findCompNameById(strtoul(compid_in,NULL,10));
						snprintf(msg,sizeof(msg),"查询[%s]计算机网络信息",compname);
						free(compname);
	 					writeSysLog(psMsgHead,"06","Success",msg);							
				}
		}
		return 0;
}
