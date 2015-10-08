#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>

#include "pasdb.h"
#include "utoplt01.h"

int readCnfNum(char *filePath,char *label);

int lan_procy_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	
  char compid_in[256] = "";
  char groupid_in[11] = "";
  char keywords_in[16] = "";
  
	utPltDbHead *psDbHead = utPltInitDb();
  
	utMsgGetSomeNVar(psMsgHead,3,
		"compid",		UT_TYPE_STRING,	sizeof(compid_in)-1,		compid_in,
    "groupid",	UT_TYPE_STRING, sizeof(groupid_in)-1, 	groupid_in,
    "keywords",	UT_TYPE_STRING, sizeof(keywords_in)-1, 	keywords_in);
    
	char caResult[4096] = "";
	int iret = -1;
	int iRetSum = 0;
  pasDbCursor *psCur = NULL;
	
	char sql[1024] = "";
	char sql_tmp[1024] = "";
	
	if(!utStrIsSpaces(groupid_in)){		
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp),"and groupid = %s",groupid_in);
	}
	if(!utStrIsSpaces(compid_in)){
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp),"and compid = %s",compid_in);
	}
	
	if(!utStrIsSpaces(keywords_in)){
		char *keyword_gbk = utf8convert(psShmHead,keywords_in);			
		snprintf(sql,sizeof(sql),"select compid from nwcompute where compname like '%%%s%%'",keyword_gbk);
		psCur = pasDbOpenSql(sql,0);
 		printf("sql : %s\n",sql);
 		if(NULL != psCur){
			unsigned int compid_tmp = 0;
			int iNum = 0;
 			while((iret=pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&compid_tmp)) == 0 || 1405 == iret){
 					if(iNum>0){
 						strcat(compid_in,",");
 					}else{
 						strcpy(compid_in,"(");
 					}
 					iNum++;
 					printf("%d",iNum);
 					snprintf(compid_in+strlen(compid_in), sizeof(compid_in)-strlen(compid_in),"%u",compid_tmp);
 			}
 			if(iNum != 0){
   			strcat(compid_in,")");
   			
   			printf("here:%s\n",compid_in);
   			snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp)," and compid in %s",compid_in);
   		}
   		pasDbCloseCursor(psCur);
 		}
   	
   	snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp), " or (inet_ntoa(ip) like '%%%s%%' or mac like '%%%s%%')", keyword_gbk, keyword_gbk);
	}
  char *auth = getDsGroupid();
  
  if(!utStrIsSpaces(auth)){
  	snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp)," and (groupid in (%s) or groupid in (select gid from nwgroup where pid in (%s)))",auth,auth);
  }
	
	snprintf(sql,sizeof(sql),"select compid,compname,inet_ntoa(ip),mac from nwcompute where 1=1 %s",sql_tmp);
		
 	printf("looksql : %s\n",sql);
 		
  unsigned int compid_tmp = 0;
  char compName_tmp[32] = "";
  char ip_tmp[16] = "";
  char mac_tmp[18] = "";
  
	iret = pasDbRecordSet(sql,0,40,&iRetSum,caResult,4,
		UT_TYPE_ULONG,	sizeof(compid_tmp),	&compid_tmp,
		UT_TYPE_STRING, sizeof(compName_tmp),	compName_tmp,
		UT_TYPE_STRING, sizeof(ip_tmp),	ip_tmp,
		UT_TYPE_STRING, sizeof(mac_tmp),	mac_tmp);
		
	
  printf("sql:%s\n",sql);	
  //printf("care:%s\n",caResult);
	
	struct dirent* ent = NULL;
  DIR *pDir;
  int iNum = 0;
  char *p = NULL;
  char path[128] = "/home/ncmysql/nw/baseprocy/";
  int mainNum = 0;

  pDir=opendir(path);
  while (NULL != (ent=readdir(pDir))){
    if (ent->d_reclen==24){
      if (ent->d_type==4){
      	//printf("普通文件:%s\n", ent->d_name);
      	unsigned int compid = strtoul(ent->d_name,NULL,10);
      	
      	p = caResult;
  			//printf("care:%s\n",p);
  			while(*p && p) {
					p = pasDbRecordValue(p,1,4,
						UT_TYPE_ULONG,	4,&compid_tmp,
						UT_TYPE_STRING, sizeof(compName_tmp),	compName_tmp,
						UT_TYPE_STRING, sizeof(ip_tmp),	ip_tmp,
						UT_TYPE_STRING, sizeof(mac_tmp),	mac_tmp);
		  		if(compid_tmp == compid){ 
			  		char path_tmp[128] = "";
		  			snprintf(path_tmp,sizeof(path_tmp),"%s%u/main.ini",path,compid);
						mainNum = readCnfNum(path_tmp,"[law");
		  			if(mainNum != 0){
							iNum++;
		  				if(iNum>1){
								utPltPutLoopVar(psDbHead,"dh",iNum,",");
							}
							utPltPutLoopVarF(psDbHead,"compid",iNum,"%u",compid);
							utPltPutLoopVar(psDbHead,"compName",iNum,compName_tmp);
							utPltPutLoopVar(psDbHead,"ip",iNum,ip_tmp);
							utPltPutLoopVar(psDbHead,"mac",iNum,mac_tmp);
							
			  			char outBuff[256] = "";
						
			  			snprintf(outBuff,sizeof(outBuff),"%u",mainNum);
							utPltPutLoopVar(psDbHead,"mainProcy",iNum,outBuff);
			  			
			  			snprintf(path_tmp,sizeof(path_tmp),"%s%u/File_law.ini",path,compid);
			  			snprintf(outBuff,sizeof(outBuff),"文件控制(%u)、",readCnfNum(path_tmp,"[law"));
			  			
			  			snprintf(path_tmp,sizeof(path_tmp),"%s%u/usb_law.ini",path,compid);
			  			snprintf(outBuff+strlen(outBuff),sizeof(outBuff)-strlen(outBuff),"外设控制(%u)、",readCnfNum(path_tmp,"[law"));
			  			
			  			snprintf(path_tmp,sizeof(path_tmp),"%s%u/Outconnect_law.ini",path,compid);
			  			snprintf(outBuff+strlen(outBuff),sizeof(outBuff)-strlen(outBuff),"非法外联控制(%u)、",readCnfNum(path_tmp,"[law"));
			  			
			  			snprintf(path_tmp,sizeof(path_tmp),"%s%u/ProcessType_law.ini",path,compid);
			  			snprintf(outBuff+strlen(outBuff),sizeof(outBuff)-strlen(outBuff),"应用程序控制(%u)、",readCnfNum(path_tmp,"[law"));
			  			
			  			snprintf(path_tmp,sizeof(path_tmp),"%s%u/Process_law.ini",path,compid);
			  			snprintf(outBuff+strlen(outBuff),sizeof(outBuff)-strlen(outBuff),"进程控制(%u)、",readCnfNum(path_tmp,"[law"));
			  			
			  			snprintf(path_tmp,sizeof(path_tmp),"%s%u/Port_law.ini",path,compid);
			  			snprintf(outBuff+strlen(outBuff),sizeof(outBuff)-strlen(outBuff),"网络控制(%u)、",readCnfNum(path_tmp,"[law"));
			  			
			  			snprintf(path_tmp,sizeof(path_tmp),"%s%u/webfilter_law.ini",path,compid);
			  			snprintf(outBuff+strlen(outBuff),sizeof(outBuff)-strlen(outBuff),"网址控制(%u)、",readCnfNum(path_tmp,"[law"));
			  			
			  			snprintf(path_tmp,sizeof(path_tmp),"%s%u/other_law.ini",path,compid);
			  			snprintf(outBuff+strlen(outBuff),sizeof(outBuff)-strlen(outBuff),"日志传送策略(%u)",readCnfNum(path_tmp,"[law"));
			  			
							utPltPutLoopVar(psDbHead,"slaveProcy",iNum,outBuff);
						}
		  		}
		  	}
    	}
    }
  }

	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_procy.htm");
	
	return 0;
}

int lan_procyInfo_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	
	char compid_in[16] = "";
	char slave_in[5] = "";
	
	utMsgGetSomeNVar(psMsgHead,2,
		"compid",		UT_TYPE_STRING,	sizeof(compid_in)-1,		compid_in,
    "slave",		UT_TYPE_STRING, sizeof(slave_in)-1, 	slave_in);
    
  FILE *fp = NULL;
  char filePath[128] = "";
  char buff[128] = "";
  int iNum = 0;
  utPltDbHead *psDbHead = utPltInitDb();
  
  snprintf(filePath,sizeof(filePath),"/home/ncmysql/nw/baseprocy/%s/",compid_in);
    
  switch(atoi(slave_in)){
 		case 0: //主策略
  		snprintf(filePath+strlen(filePath),sizeof(filePath)-strlen(filePath),"main.ini");
  		
  		if(NULL == (fp = fopen(filePath,"r"))){
  			printf("read file failed!\n");
  			return -1;
  		}
  		char sql[512] = "";
  		char *id = NULL;
    	pasDbCursor *psCur = NULL;
  		
  		while(NULL != fgets(buff,sizeof(buff)-1,fp)){
				if(!strncmp(buff,"[law",strlen("[law"))){
					iNum++;
				}
				
				if(iNum>1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
				
				if(!strncmp(buff,"id=",3)){
					id = buff+3;
					snprintf(sql,sizeof(sql),"select lawname,constatus,createtime,createuser from nwmainprocy where sid = %s",id);
					
					psCur = pasDbOpenSql(sql,0);
	    		
	    		if(psCur != NULL){
	    			char lawname[24] = "";
	    			unsigned int status = 0;
	    			unsigned int createtime = 0;
	    			char createuser[24] = "";
	    			
	    			pasDbFetchInto(psCur,
	    				UT_TYPE_STRING,sizeof(lawname),lawname,
	    				UT_TYPE_ULONG,4,&status,
	    				UT_TYPE_ULONG,4,&createtime,
	    				UT_TYPE_STRING,sizeof(createuser),createuser);
	    			utPltPutLoopVar(psDbHead,"name",iNum,lawname);
	    			utPltPutLoopVarF(psDbHead,"type",iNum,"%u",status);
	    			utPltPutLoopVar(psDbHead,"createtime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S", createtime));
	    			utPltPutLoopVar(psDbHead,"createuser",iNum,createuser);
	    			
	    			pasDbCloseCursor(psCur);
	    		}
	    	}
				
			}
			fclose(fp);
 			break;
 		case 1: //文件策略
 			snprintf(filePath+strlen(filePath),sizeof(filePath)-strlen(filePath),"File_law.ini");
 			
 			if(NULL == (fp = fopen(filePath,"r"))){
  			printf("read file failed!\n");
  			return -1;
  		}
  		
  		while(NULL != fgets(buff,sizeof(buff)-1,fp)){
  			buff[strlen(buff)-2] = '\0';
				if(!strncmp(buff,"[law",strlen("[law"))){
					iNum++;
				}else if(!strncmp(buff,"filename=",9)){
	    		utPltPutLoopVar(psDbHead,"name",iNum,buff+9);
				}else if(!strncmp(buff,"optype=",7)){
	    		utPltPutLoopVar(psDbHead,"optype",iNum,buff+7);
				}else if(!strncmp(buff,"snap=",5)){
	    		utPltPutLoopVar(psDbHead,"snap",iNum,buff+5);
				}else if(!strncmp(buff,"type=",5)){
	    		utPltPutLoopVar(psDbHead,"type",iNum,buff+5);
				}
				
				if(iNum>1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			}
  		
			fclose(fp);
  		break;
  	case 2: //进程策略
 			snprintf(filePath+strlen(filePath),sizeof(filePath)-strlen(filePath),"Process_law.ini");
 			
 			if(NULL == (fp = fopen(filePath,"r"))){
  			printf("read file failed!\n");
  			return -1;
  		}
  		
  		while(NULL != fgets(buff,sizeof(buff)-1,fp)){
  			buff[strlen(buff)-2] = '\0';
				if(!strncmp(buff,"[law",strlen("[law"))){
					iNum++;
				}else if(!strncmp(buff,"pname=",6)){
	    		utPltPutLoopVar(psDbHead,"name",iNum,buff+6);
				}else if(!strncmp(buff,"optype=",7)){
	    		utPltPutLoopVar(psDbHead,"optype",iNum,buff+7);
				}else if(!strncmp(buff,"stamp=",5)){
	    		utPltPutLoopVar(psDbHead,"stamp=",iNum,buff+6);
				}else if(!strncmp(buff,"snap=",5)){
	    		utPltPutLoopVar(psDbHead,"snap",iNum,buff+5);
				}
				
				if(iNum>1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			}
  		
			fclose(fp);
  		break;
  	case 3: //网络策略
 			snprintf(filePath+strlen(filePath),sizeof(filePath)-strlen(filePath),"Port_law.ini");
 			
 			if(NULL == (fp = fopen(filePath,"r"))){
  			printf("read file failed!\n");
  			return -1;
  		}
  		
  		while(NULL != fgets(buff,sizeof(buff)-1,fp)){
  			buff[strlen(buff)-2] = '\0';
				if(!strncmp(buff,"[law",strlen("[law"))){
					iNum++;
				}else if(!strncmp(buff,"trantype=",9)){
	    		utPltPutLoopVar(psDbHead,"trantype",iNum,buff+9);
				}else if(!strncmp(buff,"sport=",6)){
	    		utPltPutLoopVar(psDbHead,"sport",iNum,buff+6);
				}else if(!strncmp(buff,"eport=",6)){
	    		utPltPutLoopVar(psDbHead,"eport",iNum,buff+6);
				}else if(!strncmp(buff,"optype=",7)){
	    		utPltPutLoopVar(psDbHead,"optype",iNum,buff+7);
				}else if(!strncmp(buff,"snap=",5)){
	    		utPltPutLoopVar(psDbHead,"snap",iNum,buff+5);
				}else if(!strncmp(buff,"type=",5)){
	    		utPltPutLoopVar(psDbHead,"type",iNum,buff+5);
				}
				
				if(iNum>1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			}
			fclose(fp);
  		break;
  	case 4: //应用程序策略
 			snprintf(filePath+strlen(filePath),sizeof(filePath)-strlen(filePath),"ProcessType_law.ini");
 			
 			if(NULL == (fp = fopen(filePath,"r"))){
  			printf("read file failed!\n");
  			return -1;
  		}
  		
  		while(NULL != fgets(buff,sizeof(buff)-1,fp)){
  			buff[strlen(buff)-2] = '\0';
				if(!strncmp(buff,"[law",strlen("[law"))){
					iNum++;
				}else if(!strncmp(buff,"gtype=",6)){
					pasDbCursor *psCur = NULL;
    			char sql[128] = "";
    			char name[32] = "";
    			snprintf(sql,sizeof(sql),"select name from nwsoftclass where id = %s",buff+6);
    			psCur = pasDbOpenSql(sql,0);
    			
    			if(NULL != psCur){
    				pasDbFetchInto(psCur,UT_TYPE_STRING,sizeof(name),name);
	    			utPltPutLoopVar(psDbHead,"gtype",iNum,name);
    				pasDbCloseCursor(psCur);
    			}
				}else if(!strncmp(buff,"optype=",7)){
	    		utPltPutLoopVar(psDbHead,"optype",iNum,buff+7);
				}else if(!strncmp(buff,"snap=",5)){
	    		utPltPutLoopVar(psDbHead,"snap",iNum,buff+5);
				}else if(!strncmp(buff,"type=",5)){
	    		utPltPutLoopVar(psDbHead,"type",iNum,buff+5);
				}
				
				if(iNum>1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			}
			fclose(fp);
  		break;
  	case 5: //外设策略
 			snprintf(filePath+strlen(filePath),sizeof(filePath)-strlen(filePath),"usb_law.ini");
 			
 			if(NULL == (fp = fopen(filePath,"r"))){
  			printf("read file failed!\n");
  			return -1;
  		}
  		
  		while(NULL != fgets(buff,sizeof(buff)-1,fp)){
  			buff[strlen(buff)-2] = '\0';
				if(!strncmp(buff,"[law",strlen("[law"))){
					iNum++;
				}else if(!strncmp(buff,"optype=",7)){
	    		utPltPutLoopVar(psDbHead,"optype",iNum,buff+7);
				}else if(!strncmp(buff,"snap=",5)){
	    		utPltPutLoopVar(psDbHead,"snap",iNum,buff+5);
				}else if(!strncmp(buff,"freehaordid=",12)){
    			pasDbCursor *psCur = NULL;
    			char sql[128] = "";
    			char name[32] = "";
    			snprintf(sql,sizeof(sql),"select name from nwusbclass where id = %s",buff+12);
    			psCur = pasDbOpenSql(sql,0);
    			
    			if(NULL != psCur){
    				pasDbFetchInto(psCur,UT_TYPE_STRING,sizeof(name),name);
	    			utPltPutLoopVar(psDbHead,"freehaordid",iNum,name);
    				pasDbCloseCursor(psCur);
    			}
				}else if(!strncmp(buff,"type=",5)){
	    		utPltPutLoopVar(psDbHead,"type",iNum,buff+5);
				}
				
				if(iNum>1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			}
			fclose(fp);
  		break;
  	case 6: //网址策略
 			snprintf(filePath+strlen(filePath),sizeof(filePath)-strlen(filePath),"webfilter_law.ini");
 			
 			if(NULL == (fp = fopen(filePath,"r"))){
  			printf("read file failed!\n");
  			return -1;
  		}
  		
  		while(NULL != fgets(buff,sizeof(buff)-1,fp)){
  			buff[strlen(buff)-2] = '\0';
				if(!strncmp(buff,"[law",strlen("[law"))){
					iNum++;
				}else if(!strncmp(buff,"url=",4)){
	    		utPltPutLoopVar(psDbHead,"url",iNum,buff+4);
				}else if(!strncmp(buff,"urlclass=",9)){
					pasDbCursor *psCur = NULL;
    			char sql[128] = "";
    			char name[32] = "";
    			snprintf(sql,sizeof(sql),"select name from nwsoftclass where id = %s",buff+9);
    			psCur = pasDbOpenSql(sql,0);
    			
    			if(NULL != psCur){
    				pasDbFetchInto(psCur,UT_TYPE_STRING,sizeof(name),name);
	    			utPltPutLoopVar(psDbHead,"urlclass",iNum,name);
    				pasDbCloseCursor(psCur);
    			}
				}else if(!strncmp(buff,"optype=",7)){
	    		utPltPutLoopVar(psDbHead,"optype",iNum,buff+7);
				}else if(!strncmp(buff,"snap=",5)){
	    		utPltPutLoopVar(psDbHead,"snap",iNum,buff+5);
				}else if(!strncmp(buff,"type=",5)){
	    		utPltPutLoopVar(psDbHead,"type",iNum,buff+5);
				}
				
				if(iNum>1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			}
			fclose(fp);
  		break;
  	case 7: //屏幕聊天策略
 			snprintf(filePath+strlen(filePath),sizeof(filePath)-strlen(filePath),"other_law.ini");
 			
 			if(NULL == (fp = fopen(filePath,"r"))){
  			printf("read file failed!\n");
  			return -1;
  		}
  		
  		while(NULL != fgets(buff,sizeof(buff)-1,fp)){
  			buff[strlen(buff)-2] = '\0';
				if(!strncmp(buff,"[law",strlen("[law"))){
					iNum++;
				}else if(!strncmp(buff,"ctype=",6)){
	    		utPltPutLoopVar(psDbHead,"ctype",iNum,buff+6);
				}else if(!strncmp(buff,"ftime=",6)){
	    		utPltPutLoopVar(psDbHead,"ftime",iNum,buff+6);
				}else if(!strncmp(buff,"optype=",7)){
	    		utPltPutLoopVar(psDbHead,"optype",iNum,buff+7);
				}else if(!strncmp(buff,"snap=",5)){
	    		utPltPutLoopVar(psDbHead,"snap",iNum,buff+5);
				}else if(!strncmp(buff,"type=",5)){
	    		utPltPutLoopVar(psDbHead,"type",iNum,buff+5);
				}
				
				if(iNum>1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			}
			fclose(fp);
  		break;
  	case 8: //外连策略
 			snprintf(filePath+strlen(filePath),sizeof(filePath)-strlen(filePath),"Outconnect_law.ini");
 			
 			if(NULL == (fp = fopen(filePath,"r"))){
  			printf("read file failed!\n");
  			return -1;
  		}
  		
  		while(NULL != fgets(buff,sizeof(buff)-1,fp)){
  			buff[strlen(buff)-2] = '\0';
				if(!strncmp(buff,"[law",strlen("[law"))){
					iNum++;
				}else if(!strncmp(buff,"optype=",7)){
	    		utPltPutLoopVar(psDbHead,"optype",iNum,buff+7);
				}else if(!strncmp(buff,"snap=",5)){
	    		utPltPutLoopVar(psDbHead,"snap",iNum,buff+5);
				}else if(!strncmp(buff,"type=",5)){
	    		utPltPutLoopVar(psDbHead,"type",iNum,buff+5);
				}
				
				if(iNum>1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			}
			fclose(fp);
  		break;
  	default: ;
 	}  
 	utPltShowDb(psDbHead);
  utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_procyInfo.htm");  
  return 0;
}
//**************************************************************************************************************
//
//***********************************************************************************************************
int lan_procyList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  utMsgPrintMsg(psMsgHead);
  
  char deal_in[8] = "";
  char recno_in[16] = "";
  char procytype_in[32] = "";
  char timeid_in[8] = "";
  char softid_in[8] = "";
  char usbid_in[8] = "";
  char text_in[128] = "";
  char controlmark_in[8] = "";
  char controlmark1_in[8] = "";
  char controlmark2_in[8] = "";
  char snap_in[32] = "";
  char groupid_in[8] = "";
  char type_in[8] = "";
  char compid_in[32] = "";
  char delall_in[1024] = "";
  char seachFlag_in[8] = "";
  
  utMsgGetSomeNVar(psMsgHead,16,
		"deal",		      UT_TYPE_STRING,	sizeof(deal_in)-1,		      deal_in,
		"recno",		    UT_TYPE_STRING,	sizeof(recno_in)-1,		      recno_in,
    "procytype",    UT_TYPE_STRING, sizeof(procytype_in)-1,     procytype_in,
    "timeid",		    UT_TYPE_STRING, sizeof(timeid_in)-1, 	      timeid_in,
    "softid",		    UT_TYPE_STRING, sizeof(softid_in)-1, 	      softid_in,
    "usbid",		    UT_TYPE_STRING, sizeof(usbid_in)-1, 	      usbid_in,
    "text",		      UT_TYPE_STRING, sizeof(text_in)-1, 	        text_in,
    "controlmark",  UT_TYPE_STRING, sizeof(controlmark_in)-1,   controlmark_in,
    "controlmark1", UT_TYPE_STRING, sizeof(controlmark1_in)-1,  controlmark1_in,
    "controlmark2", UT_TYPE_STRING, sizeof(controlmark2_in)-1,  controlmark2_in,
    "snap",		      UT_TYPE_STRING, sizeof(snap_in)-1,          snap_in,
    "type",		      UT_TYPE_STRING, sizeof(type_in)-1,          type_in,
    "groupid",      UT_TYPE_STRING, sizeof(groupid_in)-1,       groupid_in,
    "compid",		    UT_TYPE_STRING, sizeof(compid_in)-1,        compid_in,
    "delprocy",		  UT_TYPE_STRING, sizeof(delall_in)-1,        delall_in,
    "seachflag",		UT_TYPE_STRING, sizeof(seachFlag_in)-1,     seachFlag_in);
  
  char tmp[16] = "";
  char sql[1024] = "";
  char sql_tmp[256] = "";
  char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
  int userlevel=0;
	userlevel = findUserLevel(auth);
	
	printf("here df\n");
	
  char *authgroup = getDsGroupid();
  if(!utStrIsSpaces(groupid_in)){
    sprintf(sql_tmp+strlen(sql_tmp),"groupid in(%s)",getGroupid(strtoul(groupid_in,NULL,10)));
  }else if(!utStrIsSpaces(compid_in)){
    sprintf(sql_tmp+strlen(sql_tmp),"compid=%s",compid_in);
  }else if(!utStrIsSpaces(authgroup)){
		sprintf(sql_tmp+strlen(sql_tmp),"groupid in(%s)",authgroup);
	}
	
	if(!strcmp(deal_in,"insert") && !utStrIsSpaces(compid_in)){
	  sprintf(groupid_in,"%lu",findGroupidByCompid(strtoul(compid_in,NULL,10)));
	}
	printf("here dfs\n");
	if(!strcmp(deal_in,"delall")){
	  char *p = delall_in;
	  char *endp=delall_in;
	  while(*p&&p){
	    if(!strncmp(p,"file",4)){
	      sprintf(sql,"update nwfileprocy set flag=1,updatetime=%lu where recno=%lu",time(0),strtoul(p+5,&endp,10));
	      p=endp;
	    }else if(!strncmp(p,"outlink",7)){
	      sprintf(sql,"update nwoutlinkprocy set flag=1,updatetime=%lu where recno=%lu",time(0),strtoul(p+8,&endp,10));
	      p=endp;
	    }else if(!strncmp(p,"app",3)){
	      sprintf(sql,"update nwappprocy set flag=1,updatetime=%lu where recno=%lu",time(0),strtoul(p+4,&endp,10));
	      p=endp;
	    }else if(!strncmp(p,"device",6)){
	      sprintf(sql,"update nwdeviceprocy set flag=1,updatetime=%lu where recno=%lu",time(0),strtoul(p+7,&endp,10));
	      p=endp;
	    }else if(!strncmp(p,"url",3)){
	      sprintf(sql,"update nwurlprocy set flag=1,updatetime=%lu where recno=%lu",time(0),strtoul(p+4,&endp,10));
	      p=endp;
	    }else if(!strncmp(p,"net",3)){
	      sprintf(sql,"update nwnetprocy set flag=1,updatetime=%lu where recno=%lu",time(0),strtoul(p+4,&endp,10));
	      p=endp;
	    }else if(!strncmp(p,"log",3)){
	      sprintf(sql,"update nwlogprocy set flag=1,updatetime=%lu where recno=%lu",time(0),strtoul(p+4,&endp,10));
	      p=endp;
	    }
	    printf("procy:%s\n",sql);
	    pasDbExecSqlF(sql);
	    p++;
	  }
	}
	
  if(!strcmp(procytype_in,"file")){
    if(!strcmp(deal_in,"insert")){
      sprintf(sql,"delete from nwfileprocy where compid=%lu and groupid=%lu and timeid=%s",strtoul(compid_in,NULL,10),strtoul(groupid_in,NULL,10),timeid_in);
      printf("insert procy %s\n",sql);
      pasDbExecSqlF(sql);
      sprintf(sql,"insert into nwfileprocy(filename,controlmark,groupid,compid,company,blocktime,timeid,updatetime,snap,createuser,userlevel,type) values('%s',%s,%lu,%lu,'%s',0,%s,%lu,%s,'%s',%d,%s)",text_in,controlmark_in,strtoul(groupid_in,NULL,10),strtoul(compid_in,NULL,0),shortname,timeid_in,time(0),snap_in,auth,userlevel,type_in);
    }else if(!strcmp(deal_in,"delete")){
      sprintf(sql,"update nwfileprocy set flag=1,updatetime=%lu where recno=%s",time(0),recno_in);
    }
  }else if(!strcmp(procytype_in,"outlink")){
    if(!strcmp(deal_in,"insert")){
      sprintf(sql,"delete from nwoutlinkprocy where compid=%lu and groupid=%lu and timeid=%s",strtoul(compid_in,NULL,10),strtoul(groupid_in,NULL,10),timeid_in);
      printf("insert procy %s\n",sql);
      pasDbExecSqlF(sql);
      sprintf(sql,"insert into nwoutlinkprocy(controlmark,groupid,compid,company,blocktime,timeid,updatetime,snap,createuser,userlevel,type) values(%s,%lu,%lu,'%s',0,%s,%lu,%s,'%s',%d,%s)",controlmark_in,strtoul(groupid_in,NULL,10),strtoul(compid_in,NULL,10),shortname,timeid_in,time(0),snap_in,auth,userlevel,type_in);
    }else if(!strcmp(deal_in,"delete")){
      sprintf(sql,"update nwoutlinkprocy set flag=1,updatetime=%lu where recno=%s",time(0),recno_in);
    }
  }else if(!strcmp(procytype_in,"app")){
    if(!strcmp(deal_in,"insert")){
      sprintf(sql,"delete from nwappprocy where compid=%lu and groupid=%lu and timeid=%s",strtoul(compid_in,NULL,10),strtoul(groupid_in,NULL,10),timeid_in);
      pasDbExecSqlF(sql);
      sprintf(sql,"insert into nwappprocy(controlmark,runtime,apptype,groupid,compid,company,blocktime,timeid,updatetime,snap,createuser,userlevel,type) values(%s,%d,%s,%lu,%lu,'%s',0,%s,%lu,%s,'%s',%d,%s)",controlmark_in,atoi(text_in),softid_in,strtoul(groupid_in,NULL,10),strtoul(compid_in,NULL,10),shortname,timeid_in,time(0),snap_in,auth,userlevel,type_in);
    }else if(!strcmp(deal_in,"delete")){
      sprintf(sql,"update nwappprocy set flag=1,updatetime=%lu where recno=%s",time(0),recno_in);
    }
  }else if(!strcmp(procytype_in,"device")){
    if(!strcmp(deal_in,"insert")){
      sprintf(sql,"delete from nwdeviceprocy where compid=%lu and groupid=%lu and timeid=%s",strtoul(compid_in,NULL,10),strtoul(groupid_in,NULL,10),timeid_in);
      pasDbExecSqlF(sql);
      sprintf(sql,"insert into nwdeviceprocy(controlmark,controlmark2,usbid,groupid,compid,company,blocktime,timeid,updatetime,snap,createuser,userlevel,type) values(%s,%s,%lu,%lu,%lu,'%s',0,%s,%lu,%s,'%s',%d,%s)",controlmark_in,controlmark2_in,strtoul(usbid_in,NULL,10),strtoul(groupid_in,NULL,10),strtoul(compid_in,NULL,10),shortname,timeid_in,time(0),snap_in,auth,userlevel,type_in);
    }else if(!strcmp(deal_in,"delete")){
      sprintf(sql,"update nwdeviceprocy set flag=1,updatetime=%lu where recno=%s",time(0),recno_in);
    }
  }else if(!strcmp(procytype_in,"url")){
    if(!strcmp(deal_in,"insert")){
      sprintf(sql,"delete from nwurlprocy where compid=%lu and groupid=%lu and timeid=%s",strtoul(compid_in,NULL,10),strtoul(groupid_in,NULL,10),timeid_in);
      
      writeLog("procy.txt","exec",sql);
      pasDbExecSqlF(sql);
      sprintf(sql,"insert into nwurlprocy(keywords,groupid,compid,company,blocktime,timeid,updatetime,snap,createuser,userlevel,type) values('%s',%lu,%lu,'%s',0,%s,%lu,%s,'%s',%d,%s)",text_in,strtoul(groupid_in,NULL,10),strtoul(compid_in,NULL,10),shortname,timeid_in,time(0),snap_in,auth,userlevel,type_in);
    }else if(!strcmp(deal_in,"delete")){
      sprintf(sql,"update nwurlprocy set flag=1,updatetime=%lu where recno=%s",time(0),recno_in);
    }
  }else if(!strcmp(procytype_in,"net")){
    if(!strcmp(deal_in,"insert")){
      sprintf(sql,"delete from nwnetprocy where compid=%lu and groupid=%lu and timeid=%s",strtoul(compid_in,NULL,10),strtoul(groupid_in,NULL,10),timeid_in);
      pasDbExecSqlF(sql);
      sprintf(sql,"insert into nwnetprocy(port,controlmark,groupid,compid,company,blocktime,timeid,updatetime,snap,createuser,userlevel,type) values('%s',%d,%lu,%lu,'%s',0,%s,%lu,%s,'%s',%d,%s)",text_in,atoi(controlmark1_in),strtoul(groupid_in,NULL,10),strtoul(compid_in,NULL,10),shortname,timeid_in,time(0),snap_in,auth,userlevel,type_in);
      printf("sql=%s\n",sql);
    }else if(!strcmp(deal_in,"delete")){
      sprintf(sql,"update nwnetprocy set flag=1,updatetime=%lu where recno=%s",time(0),recno_in);
    }
  }else if(!strcmp(procytype_in,"log")){
    if(!strcmp(deal_in,"insert")){
      sprintf(sql,"delete from nwlogprocy where compid=%lu and groupid=%lu and timeid=%s",strtoul(compid_in,NULL,10),strtoul(groupid_in,NULL,10),timeid_in);
  //    printf("sql=%s\n",sql);
      pasDbExecSqlF(sql);
      sprintf(sql,"insert into nwlogprocy(timelimit,controlmark,groupid,compid,company,blocktime,timeid,updatetime,createuser,userlevel,type,controlmark2,timelimit2)\
       values(%s,%s,%lu,%lu,'%s',0,%s,%lu,'%s',%d,%s,%lu,%lu)",\
      text_in,controlmark_in,strtoul(groupid_in,NULL,10),strtoul(compid_in,NULL,10),shortname,timeid_in,time(0),auth,userlevel,type_in,strtoul(controlmark1_in,NULL,10),strtoul(controlmark2_in,NULL,10));
      printf("sql=%s\n",sql);
    }else if(!strcmp(deal_in,"delete")){
      sprintf(sql,"update nwlogprocy set flag=1,updatetime=%lu where recno=%s",time(0),recno_in);
    }
  }
  
  if(atoi(seachFlag_in)==1){
    strcpy(procytype_in,"");
  }
  
  if(strlen(deal_in)>0){
    writeLog("procy.txt","exec",sql);
    pasDbExecSqlF(sql);
  }
  
  memset(tmp,0,sizeof(tmp));
  if(!utStrIsSpaces(groupid_in) || !utStrIsSpaces(compid_in) || !utStrIsSpaces(authgroup)){
    sprintf(tmp," and nwfileprocy.");
  }
  sprintf(sql,"select nwfileprocy.recno,filename,controlmark,snap,gname,inet_ntoa(ip),timeid,nwdatedes.name,userlevel,keyname,from_unixtime(updatetime),type from nwfileprocy left join nwgroup on nwfileprocy.groupid=nwgroup.gid left join nwcompute on nwcompute.compid = nwfileprocy.compid left join nwdatedes on nwdatedes.id=nwfileprocy.timeid left join paramsmanage on paramsmanage.keyvalue=nwfileprocy.userlevel and paramtype=1 where nwfileprocy.company='%s' and nwfileprocy.flag<>1 %s%s and ('file'='%s' or ''='%s')",shortname,tmp,sql_tmp,procytype_in,procytype_in);
  
  pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
  writeLog("procy.txt","seach",sql);
	psCur = pasDbOpenSql(sql, 0);
  int iret = 0,iNum = 0;
  
  if(psCur!=NULL){
    unsigned long recno = 0;
    char filename[32] = "";
    unsigned long controlmark = 0;
    unsigned long snap = 0;
		char groupname[32] = "";
		char ip[16] = "";
		unsigned timeid = 0;
		char timename[32] = "";
		unsigned int level1=0;
		char levelname[32] = "";
		char modtime[24] = "";
		unsigned int type=0;
		
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(recno),      &recno,
			 UT_TYPE_STRING,sizeof(filename)-1, filename,
			 UT_TYPE_ULONG, sizeof(controlmark),&controlmark,
			 UT_TYPE_ULONG, sizeof(snap),       &snap,
			 UT_TYPE_STRING,sizeof(groupname)-1,groupname,
			 UT_TYPE_STRING,sizeof(ip)-1,       ip,
			 UT_TYPE_ULONG, sizeof(timeid), 	  &timeid,
			 UT_TYPE_STRING,sizeof(timename)-1, timename,
			 UT_TYPE_LONG,  sizeof(level1), 	  &level1,
			 UT_TYPE_STRING,sizeof(levelname)-1,levelname,
			 UT_TYPE_STRING,sizeof(modtime)-1, 	modtime,
			 UT_TYPE_LONG,  sizeof(type), 	    &type))  || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0030文件策略"));
			utPltPutLoopVarF(psDbHead,"recno",iNum,"%u",recno);
			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
			utPltPutLoopVar(psDbHead,"text",iNum,filename);
			utPltPutLoopVarF(psDbHead,"controlmark",iNum,"%u",controlmark);
			utPltPutLoopVarF(psDbHead,"snap",iNum,"%u",snap);
			utPltPutLoopVar(psDbHead,"ip",iNum,ip);
			utPltPutLoopVarF(psDbHead,"timeid",iNum,"%u",timeid);
			utPltPutLoopVar(psDbHead,"timename",iNum,timename);
			utPltPutLoopVarF(psDbHead,"levelremain",iNum,"%d",level1-userlevel);	
			utPltPutLoopVar(psDbHead,"levelname",iNum,levelname);
			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);	
			utPltPutLoopVarF(psDbHead,"type",iNum,"%u",type);	
			
			memset(ip,0,sizeof(ip));
			memset(groupname,0,sizeof(groupname));
		}
		
		pasDbCloseCursor(psCur);
	}
	
	memset(tmp,0,sizeof(tmp));
  if(!utStrIsSpaces(groupid_in) || !utStrIsSpaces(compid_in) || !utStrIsSpaces(authgroup)){
    sprintf(tmp," and nwoutlinkprocy.");
  }
	sprintf(sql,"select nwoutlinkprocy.recno,controlmark,snap,gname,inet_ntoa(ip),timeid,nwdatedes.name,userlevel,keyname,from_unixtime(updatetime),type from nwoutlinkprocy left join nwgroup on nwoutlinkprocy.groupid=nwgroup.gid left join nwcompute on nwcompute.compid = nwoutlinkprocy.compid left join nwdatedes on nwdatedes.id=nwoutlinkprocy.timeid left join paramsmanage on paramsmanage.keyvalue=nwoutlinkprocy.userlevel and paramtype=1 where nwoutlinkprocy.company='%s' and nwoutlinkprocy.flag<>1 %s%s and ('outlink'='%s' or ''='%s')",shortname,tmp,sql_tmp,procytype_in,procytype_in);
  
	//printf("策略：sql:%s\n",sql);
	psCur = pasDbOpenSql(sql, 0);
  
  if(psCur!=NULL){
    unsigned long recno = 0;
    unsigned long controlmark = 0;
    unsigned long snap = 0;
		char groupname[32] = "";
		char ip[16] = "";
		unsigned timeid = 0;
		char timename[32] = "";
		unsigned int level1=0;
		char levelname[32] = "";
		char modtime[24] = "";
		unsigned int type=0;
		
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(recno),      &recno,
			 UT_TYPE_ULONG, sizeof(controlmark),&controlmark,
			 UT_TYPE_ULONG, sizeof(snap),       &snap,
			 UT_TYPE_STRING,sizeof(groupname)-1,groupname,
			 UT_TYPE_STRING,sizeof(ip)-1,       ip,
			 UT_TYPE_ULONG, sizeof(timeid), 	  &timeid,
			 UT_TYPE_STRING,sizeof(timename)-1, timename,
			 UT_TYPE_LONG,  sizeof(level1), 	  &level1,
			 UT_TYPE_STRING,sizeof(levelname)-1,levelname,
			 UT_TYPE_STRING,sizeof(modtime)-1, 	modtime,
			 UT_TYPE_LONG,  sizeof(type), 	    &type))  || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0031外联策略"));
			utPltPutLoopVarF(psDbHead,"recno",iNum,"%u",recno);
			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
			utPltPutLoopVarF(psDbHead,"controlmark",iNum,"%u",controlmark);
			utPltPutLoopVarF(psDbHead,"snap",iNum,"%u",snap);
			utPltPutLoopVar(psDbHead,"ip",iNum,ip);
			utPltPutLoopVarF(psDbHead,"timeid",iNum,"%u",timeid);
			utPltPutLoopVar(psDbHead,"timename",iNum,timename);
			utPltPutLoopVarF(psDbHead,"levelremain",iNum,"%d",level1-userlevel);	
			utPltPutLoopVar(psDbHead,"levelname",iNum,levelname);
			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);	
			utPltPutLoopVarF(psDbHead,"type",iNum,"%u",type);	
			
			memset(ip,0,sizeof(ip));
			memset(groupname,0,sizeof(groupname));
		}
		
		pasDbCloseCursor(psCur);
	}
	
	memset(tmp,0,sizeof(tmp));
  if(!utStrIsSpaces(groupid_in) || !utStrIsSpaces(compid_in) || !utStrIsSpaces(authgroup)){
    sprintf(tmp," and nwappprocy.");
  }
	sprintf(sql,"select controlmark,runtime,nwappprocy.recno,nwsoftclass.name,apptype,snap,gname,inet_ntoa(ip),timeid,nwdatedes.name,userlevel,keyname,from_unixtime(updatetime),type from nwappprocy left join nwgroup on nwappprocy.groupid=nwgroup.gid left join nwcompute on nwcompute.compid = nwappprocy.compid left join nwdatedes on nwdatedes.id=nwappprocy.timeid left join nwsoftclass on nwsoftclass.id=apptype left join paramsmanage on paramsmanage.keyvalue=nwappprocy.userlevel and paramtype=1 where nwappprocy.company='%s' and nwappprocy.flag<>1 %s%s and ('app'='%s' or ''='%s')",shortname,tmp,sql_tmp,procytype_in,procytype_in);
  
	//printf("策略：sql:%s\n",sql);
	psCur = pasDbOpenSql(sql, 0);
  
  if(psCur!=NULL){
    unsigned long recno = 0;
    unsigned long snap = 0;
		char groupname[32] = "";
		unsigned long runtime = 0;
    unsigned long controlmark = 0;
		char ip[16] = "";
		unsigned long timeid = 0;
		unsigned long softid = 0;
		char timename[32] = "";
		char softname[32] = "";
		unsigned int level1=0;
		char levelname[32] = "";
		char modtime[24] = "";
		unsigned int type=0;
		
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(controlmark),&controlmark,
			 UT_TYPE_ULONG, sizeof(runtime),    &runtime,
			 UT_TYPE_ULONG, sizeof(recno),      &recno,
			 UT_TYPE_STRING,sizeof(softname)-1, softname,
			 UT_TYPE_ULONG, sizeof(softid), 	  &softid,
			 UT_TYPE_ULONG, sizeof(snap),       &snap,
			 UT_TYPE_STRING,sizeof(groupname)-1,groupname,
			 UT_TYPE_STRING,sizeof(ip)-1,       ip,
			 UT_TYPE_ULONG, sizeof(timeid), 	  &timeid,
			 UT_TYPE_STRING,sizeof(timename)-1, timename,
			 UT_TYPE_LONG,  sizeof(level1), 	  &level1,
			 UT_TYPE_STRING,sizeof(levelname)-1,levelname,
			 UT_TYPE_STRING,sizeof(modtime)-1, 	modtime,
			 UT_TYPE_LONG,  sizeof(type), 	    &type)) || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0032应用策略"));
			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",recno);
			utPltPutLoopVarF(psDbHead,"text",iNum,"%lu",runtime);
			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
			utPltPutLoopVarF(psDbHead,"controlmark",iNum,"%lu",controlmark);
			utPltPutLoopVarF(psDbHead,"snap",iNum,"%u",snap);
			utPltPutLoopVar(psDbHead,"ip",iNum,ip);
			utPltPutLoopVarF(psDbHead,"timeid",iNum,"%u",timeid);
			utPltPutLoopVar(psDbHead,"timename",iNum,timename);
			utPltPutLoopVarF(psDbHead,"softid",iNum,"%u",softid);
			utPltPutLoopVar(psDbHead,"softname",iNum,softname);
			utPltPutLoopVarF(psDbHead,"levelremain",iNum,"%d",level1-userlevel);	
			utPltPutLoopVar(psDbHead,"levelname",iNum,levelname);
			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);	
			utPltPutLoopVarF(psDbHead,"type",iNum,"%u",type);
			
			memset(ip,0,sizeof(ip));
			memset(groupname,0,sizeof(groupname));
		}
		
		pasDbCloseCursor(psCur);
	}
	
	memset(tmp,0,sizeof(tmp));
  if(!utStrIsSpaces(groupid_in) || !utStrIsSpaces(compid_in) || !utStrIsSpaces(authgroup)){
    sprintf(tmp," and nwdeviceprocy.");
  }
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select controlmark,controlmark2,nwdeviceprocy.recno,nwusbclass.name,usbid,snap,gname,inet_ntoa(ip),timeid,nwdatedes.name,userlevel,keyname,from_unixtime(updatetime),type from nwdeviceprocy left join nwgroup on nwdeviceprocy.groupid=nwgroup.gid left join nwcompute on nwcompute.compid = nwdeviceprocy.compid left join nwdatedes on nwdatedes.id=nwdeviceprocy.timeid left join nwusbclass on nwusbclass.id=usbid left join paramsmanage on paramsmanage.keyvalue=nwdeviceprocy.userlevel and paramtype=1 where nwdeviceprocy.company='%s' and nwdeviceprocy.flag<>1 %s%s and ('device'='%s' or ''='%s')",shortname,tmp,sql_tmp,procytype_in,procytype_in);
  
	//printf("策略：sql:%s\n",sql);
	psCur = pasDbOpenSql(sql, 0);
  
  if(psCur!=NULL){
    unsigned long recno = 0;
    unsigned long snap = 0;
		char groupname[32] = "";
    unsigned long controlmark = 0;
    unsigned long controlmark2 = 0;
		char ip[16] = "";
		unsigned long timeid = 0;
		unsigned long usbid = 0;
		char timename[32] = "";
		char usbname[32] = "";
		unsigned int level1=0;
		char levelname[32] = "";
		char modtime[24] = "";
		unsigned int type=0;
		
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(controlmark),  &controlmark,
			 UT_TYPE_ULONG, sizeof(controlmark2), &controlmark2,
			 UT_TYPE_ULONG, sizeof(recno),        &recno,
			 UT_TYPE_STRING,sizeof(usbname)-1,    usbname,
			 UT_TYPE_ULONG, sizeof(usbid), 	      &usbid,
			 UT_TYPE_ULONG, sizeof(snap),         &snap,
			 UT_TYPE_STRING,sizeof(groupname)-1,  groupname,
			 UT_TYPE_STRING,sizeof(ip)-1,         ip,
			 UT_TYPE_ULONG, sizeof(timeid), 	    &timeid,
			 UT_TYPE_STRING,sizeof(timename)-1,   timename,
			 UT_TYPE_LONG,  sizeof(level1), 	    &level1,
			 UT_TYPE_STRING,sizeof(levelname)-1,  levelname,
			 UT_TYPE_STRING,sizeof(modtime)-1, 	  modtime,
			 UT_TYPE_ULONG,  sizeof(type), 	      &type)) || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0036外设策略"));
			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",recno);
			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
			utPltPutLoopVarF(psDbHead,"controlmark",iNum,"%lu",controlmark);
			utPltPutLoopVarF(psDbHead,"controlmark2",iNum,"%lu",controlmark2);
			utPltPutLoopVarF(psDbHead,"snap",iNum,"%u",snap);
			utPltPutLoopVar(psDbHead,"ip",iNum,ip);
			utPltPutLoopVarF(psDbHead,"timeid",iNum,"%u",timeid);
			utPltPutLoopVar(psDbHead,"timename",iNum,timename);
			utPltPutLoopVarF(psDbHead,"usbid",iNum,"%u",usbid);
			utPltPutLoopVar(psDbHead,"usbname",iNum,usbname);
			utPltPutLoopVarF(psDbHead,"levelremain",iNum,"%d",level1-userlevel);	
			utPltPutLoopVar(psDbHead,"levelname",iNum,levelname);
			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);	
			utPltPutLoopVarF(psDbHead,"type",iNum,"%u",type);
			
			memset(ip,0,sizeof(ip));
			memset(groupname,0,sizeof(groupname));
		}
		
		pasDbCloseCursor(psCur);
	}
	
	memset(tmp,0,sizeof(tmp));
  if(!utStrIsSpaces(groupid_in) || !utStrIsSpaces(compid_in) || !utStrIsSpaces(authgroup)){
    sprintf(tmp," and nwurlprocy.");
  }
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select keywords,nwurlprocy.recno,snap,gname,inet_ntoa(ip),timeid,nwdatedes.name,userlevel,keyname,from_unixtime(updatetime),type from nwurlprocy left join nwgroup on nwurlprocy.groupid=nwgroup.gid left join nwcompute on nwcompute.compid = nwurlprocy.compid left join nwdatedes on nwdatedes.id=nwurlprocy.timeid left join paramsmanage on paramsmanage.keyvalue=nwurlprocy.userlevel and paramtype=1 where nwurlprocy.company='%s' and nwurlprocy.flag<>1 %s%s and ('url'='%s' or ''='%s')",shortname,tmp,sql_tmp,procytype_in,procytype_in);
  
	psCur = pasDbOpenSql(sql, 0);
  
  if(psCur!=NULL){
    unsigned long recno = 0;
    unsigned long snap = 0;
		char groupname[32] = "";
		char text[32] = "";
		char ip[16] = "";
		unsigned long timeid = 0;
		char timename[32] = "";
		unsigned int level1=0;
		char levelname[32] = "";
		char modtime[24] = "";
		unsigned int type=0;
		
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_STRING,sizeof(text)-1,       text,
			 UT_TYPE_ULONG, sizeof(recno),        &recno,
			 UT_TYPE_ULONG, sizeof(snap),         &snap,
			 UT_TYPE_STRING,sizeof(groupname)-1,  groupname,
			 UT_TYPE_STRING,sizeof(ip)-1,         ip,
			 UT_TYPE_ULONG, sizeof(timeid), 	    &timeid,
			 UT_TYPE_STRING,sizeof(timename)-1,   timename,
			 UT_TYPE_LONG,  sizeof(level1), 	    &level1,
			 UT_TYPE_STRING,sizeof(levelname)-1,  levelname,
			 UT_TYPE_STRING,sizeof(modtime)-1, 	  modtime,
			 UT_TYPE_ULONG,  sizeof(type), 	      &type)) || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0033网址策略"));
			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",recno);
			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
			utPltPutLoopVar(psDbHead,"text",iNum,text);
			utPltPutLoopVarF(psDbHead,"snap",iNum,"%u",snap);
			utPltPutLoopVar(psDbHead,"ip",iNum,ip);
			utPltPutLoopVarF(psDbHead,"timeid",iNum,"%u",timeid);
			utPltPutLoopVar(psDbHead,"timename",iNum,timename);
			utPltPutLoopVarF(psDbHead,"levelremain",iNum,"%d",level1-userlevel);	
			utPltPutLoopVar(psDbHead,"levelname",iNum,levelname);
			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);	
			utPltPutLoopVarF(psDbHead,"type",iNum,"%u",type);
			
			memset(ip,0,sizeof(ip));
			memset(groupname,0,sizeof(groupname));
		}
		
		pasDbCloseCursor(psCur);
	}
	
	memset(tmp,0,sizeof(tmp));
  if(!utStrIsSpaces(groupid_in) || !utStrIsSpaces(compid_in) || !utStrIsSpaces(authgroup)){
    sprintf(tmp," and nwnetprocy.");
  }
	sprintf(sql,"select nwnetprocy.recno,port,controlmark,snap,gname,inet_ntoa(ip),timeid,nwdatedes.name,userlevel,keyname,from_unixtime(updatetime),type from nwnetprocy left join nwgroup on nwnetprocy.groupid=nwgroup.gid left join nwcompute on nwcompute.compid = nwnetprocy.compid left join nwdatedes on nwdatedes.id=nwnetprocy.timeid left join paramsmanage on paramsmanage.keyvalue=nwnetprocy.userlevel and paramtype=1 where nwnetprocy.company='%s' and nwnetprocy.flag<>1 %s%s and ('net'='%s' or ''='%s')",shortname,tmp,sql_tmp,procytype_in,procytype_in);
  
	printf("策略：sql:%s\n",sql);
	psCur = pasDbOpenSql(sql, 0);
  
  if(psCur!=NULL){
    unsigned long recno = 0;
    char port[128] = "";
    unsigned long controlmark = 0;
    unsigned long snap = 0;
		char groupname[32] = "";
		char ip[16] = "";
		unsigned long timeid = 0;
		char timename[32] = "";
		unsigned int level1=0;
		char levelname[32] = "";
		char modtime[24] = "";
		unsigned int type=0;
		
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(recno),      &recno,
			 UT_TYPE_STRING,sizeof(port)-1,     port,
			 UT_TYPE_ULONG, sizeof(controlmark),&controlmark,
			 UT_TYPE_ULONG, sizeof(snap),       &snap,
			 UT_TYPE_STRING,sizeof(groupname)-1,groupname,
			 UT_TYPE_STRING,sizeof(ip)-1,       ip,
			 UT_TYPE_ULONG, sizeof(timeid), 	  &timeid,
			 UT_TYPE_STRING,sizeof(timename)-1, timename,
			 UT_TYPE_LONG,  sizeof(level1), 	  &level1,
			 UT_TYPE_STRING,sizeof(levelname)-1,levelname,
			 UT_TYPE_STRING,sizeof(modtime)-1, 	modtime,
			 UT_TYPE_ULONG, sizeof(type), 	    &type))  || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0034网络策略"));
			utPltPutLoopVarF(psDbHead,"recno",iNum,"%u",recno);
			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
			utPltPutLoopVar(psDbHead,"text",iNum,port);
			utPltPutLoopVarF(psDbHead,"controlmark1",iNum,"%u",controlmark);
			utPltPutLoopVarF(psDbHead,"snap",iNum,"%u",snap);
			utPltPutLoopVar(psDbHead,"ip",iNum,ip);
			utPltPutLoopVarF(psDbHead,"timeid",iNum,"%u",timeid);
			utPltPutLoopVar(psDbHead,"timename",iNum,timename);
			utPltPutLoopVarF(psDbHead,"levelremain",iNum,"%d",level1-userlevel);	
			utPltPutLoopVar(psDbHead,"levelname",iNum,levelname);
			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);	
			utPltPutLoopVarF(psDbHead,"type",iNum,"%u",type);
			
			memset(ip,0,sizeof(ip));
			memset(groupname,0,sizeof(groupname));
		}
		
		pasDbCloseCursor(psCur);
	}
	
	memset(tmp,0,sizeof(tmp));
  if(!utStrIsSpaces(groupid_in) || !utStrIsSpaces(compid_in) || !utStrIsSpaces(authgroup)){
    sprintf(tmp," and nwlogprocy.");
  }
	sprintf(sql,"select nwlogprocy.recno,timelimit,controlmark,gname,inet_ntoa(ip),timeid,nwdatedes.name,userlevel,keyname,from_unixtime(updatetime),type,controlmark2,timelimit2 from nwlogprocy left join nwgroup on nwlogprocy.groupid=nwgroup.gid left join nwcompute on nwcompute.compid = nwlogprocy.compid left join nwdatedes on nwdatedes.id=nwlogprocy.timeid left join paramsmanage on paramsmanage.keyvalue=nwlogprocy.userlevel and paramtype=1 where nwlogprocy.company='%s' and nwlogprocy.flag<>1 %s%s and ('log'='%s' or ''='%s')",shortname,tmp,sql_tmp,procytype_in,procytype_in);
  
  writeLog("procy.txt","seach",sql);
	psCur = pasDbOpenSql(sql, 0);
  
  if(psCur!=NULL){
    unsigned long recno = 0;
    unsigned long timelimit = 0;
    unsigned long timelimit2 = 0;
    unsigned long controlmark = 0;
    unsigned long controlmark2 = 0;
		char groupname[32] = "";
		char ip[16] = "";
		unsigned long timeid = 0;
		char timename[32] = "";
		unsigned int level1=0;
		char levelname[32] = "";
		char modtime[24] = "";
		unsigned int type=0;
		printf("11111111 \n");
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(recno),      &recno,
			 UT_TYPE_ULONG, sizeof(timelimit),  &timelimit,
			 UT_TYPE_ULONG, sizeof(controlmark),&controlmark,
			 UT_TYPE_STRING,sizeof(groupname)-1,groupname,
			 UT_TYPE_STRING,sizeof(ip)-1,       ip,
			 UT_TYPE_ULONG, sizeof(timeid), 	  &timeid,
			 UT_TYPE_STRING,sizeof(timename)-1, timename,
			 UT_TYPE_LONG,  sizeof(level1), 	  &level1,
			 UT_TYPE_STRING,sizeof(levelname)-1,levelname,
			 UT_TYPE_STRING,sizeof(modtime)-1, 	modtime,
			 UT_TYPE_ULONG, sizeof(type), 	    &type,
			 UT_TYPE_ULONG, sizeof(controlmark2),&controlmark2,
			 UT_TYPE_ULONG, sizeof(timelimit2), &timelimit2))  || 1405==iret){
			
		printf("2222222 \n");
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0035其他策略"));
			utPltPutLoopVarF(psDbHead,"recno",iNum,"%u",recno);
			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
			utPltPutLoopVarF(psDbHead,"text",iNum,"%lu",timelimit);
			utPltPutLoopVarF(psDbHead,"controlmark",iNum,"%u",controlmark);
			utPltPutLoopVar(psDbHead,"ip",iNum,ip);
			utPltPutLoopVarF(psDbHead,"timeid",iNum,"%u",timeid);
			utPltPutLoopVar(psDbHead,"timename",iNum,timename);
			utPltPutLoopVarF(psDbHead,"levelremain",iNum,"%d",level1-userlevel);	
			utPltPutLoopVar(psDbHead,"levelname",iNum,levelname);
			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);	
			utPltPutLoopVarF(psDbHead,"type",iNum,"%u",type);
			printf("hereer \n");
			utPltPutLoopVarF(psDbHead,"controlmark2",iNum,"%lu",controlmark2);
			printf("hereer2 \n");
			utPltPutLoopVarF(psDbHead,"controlmark1",iNum,"%lu",timelimit2);
			
			memset(ip,0,sizeof(ip));
			memset(groupname,0,sizeof(groupname));
		}
		
		pasDbCloseCursor(psCur);
	}
	
	//utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_procy.htm");

  return 0; 
}

int lan_SenseCombo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
	
	char sql[128] = "";
	
	sprintf(sql,"select recno,keyname from paramsmanage where paramtype=2 and company='%s' group by keyname",shortname);
	writeLog("procy.txt","敏感进程",sql);
	pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
	psCur = pasDbOpenSql(sql, 0);
	
	if(psCur!=NULL){
		unsigned int recno = 0;
		char keyname[32] = "";
		
		int iret = 0,iNum = 0;
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(recno), 	&recno,
			 UT_TYPE_STRING, sizeof(keyname)-1,  keyname)) || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"id",iNum,"%u",recno);
			utPltPutLoopVar(psDbHead,"text",iNum,keyname);
		}
		
		pasDbCloseCursor(psCur);
	}
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_senseParams.htm");
	
	return 0;
}
