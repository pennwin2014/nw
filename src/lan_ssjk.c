#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "nwdef.h"
#include "ncdef.h"

#define		PRO_CENTER_COMMANDID_CAPTURESCREEN	0X00000010	//实时屏幕
#define		PRO_CENTER_COMMANDID_REMOTECTR		0X00000020
#define		PRO_CENTER_COMMANDID_CLIENTOP		0X00000040		//远程控制
#define	PRO_CENTER_COMMANDID_CAPTURESCREEN2	0X00040000

int lan_sspm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
		//utMsgPrintMsg(psMsgHead);
		
		char mode_in[4] = "";
		char compid_in[16] = "";
		char position_in[8] = "";
		char ip_in[16] = "";
	
		utMsgGetSomeNVar(psMsgHead,4,
    		"mode",		UT_TYPE_STRING,  sizeof(mode_in)-1, 		mode_in,
    		"compid",		UT_TYPE_STRING,  sizeof(compid_in)-1, compid_in,
    		"position",		UT_TYPE_STRING,  sizeof(position_in)-1, position_in,
    		"ip",		UT_TYPE_STRING,  sizeof(ip_in)-1, ip_in);
    		
    unsigned int ip = strtoul(ip_in,NULL,10);
    snprintf(ip_in,sizeof(ip_in),"%u.%u.%u.%u",ip&0xFF,(ip>>8)&0xFF,(ip>>16)&0xFF,ip>>24);
    		
    unsigned int compid = strtoul(compid_in,NULL,10);
		nwCompIp *psCompIp = NULL;
		psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
		if(psCompIp){
			if(atoi(mode_in)){
				psCompIp->lCtrl|=PRO_CENTER_COMMANDID_REMOTECTR;
			}else if(atoi(mode_in)== 0){
				psCompIp->lCtrl|=PRO_CENTER_COMMANDID_CAPTURESCREEN;
			}
		}
		
		char compidVar[24] = "";
		snprintf(compidVar,sizeof(compidVar),"ssip%s",compid_in);
		utComSetVar_s(psShmHead,compidVar,ip_in);
				
		char position[8] = "";
		
		snprintf(compidVar,sizeof(compidVar),"sspo%s",compid_in);
		utComSetVar_s(psShmHead,compidVar,position_in);
	
  	utPltDbHead *psDbHead = utPltInitDb();
		utPltPutVar(psDbHead,"buff","{success:true}");
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");

		return 0;
}

int getEachScreen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	
	char position_in[8] = "";
	
	utMsgGetSomeNVar(psMsgHead,1,"position",  UT_TYPE_STRING, sizeof(position_in), position_in);
	
	utPltDbHead *psDbHead = utPltInitDb();
	utPltPutVar(psDbHead,"num",position_in);
	utPltPutVarF(psDbHead,"position","%d",atoi(position_in)+5000);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/ssjk/lansspm.html");
	
	return 0;
}

int lan_getAccess(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	
	char compid_in[16] = "";
	char mode_in[8] = "";
  utPltDbHead *psDbHead = utPltInitDb();
	
	utMsgGetSomeNVar(psMsgHead,2,
  		"compid",		UT_TYPE_STRING,  sizeof(compid_in)-1, 		compid_in,
  		"mode",			UT_TYPE_STRING,  sizeof(mode_in)-1, 			mode_in);
  char compidVar[16] = "";
  //printf("mode:%s\tcompid:%s\n",mode_in,compid_in);
	char *recei = NULL;
	int num = 0;
	while(1){
		num++;
		if(!strcmp(mode_in,"1")){
			if(num>30){
				utPltPutVar(psDbHead,"buff","{success:true,msg:2}");
				break;
			}
	  	sprintf(compidVar,"ac%s",compid_in);
			recei = (char*)utComGetVar_sd(psShmHead,compidVar,"NULL");
			
		}else if(!strcmp(mode_in,"2")){
			if(num>5){
				utPltPutVar(psDbHead,"buff","{success:true,msg:2}");
				break;
			}
	  	sprintf(compidVar,"ak%s",compid_in);
			recei = (char*)utComGetVar_sd(psShmHead,compidVar,"NULL");
		}
		
		//printf("onlineScreen:%s\n",recei);
		
		if(recei != NULL && !strcmp(recei,"receive")){
			utPltPutVar(psDbHead,"buff","{success:true,msg:5}");
			utComSetVar_s(psShmHead,compidVar,"NULL");
			break;
		}
		
		sleep(1);
	}
	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	
	return 0;
}

int lan_getLink(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	
	char compid_in[16] = "";
  utPltDbHead *psDbHead = utPltInitDb();
  
	utMsgGetSomeNVar(psMsgHead,1,
  		"compid",		UT_TYPE_STRING,  sizeof(compid_in)-1, 		compid_in);
  		
  unsigned long compid = strtoul(compid_in,NULL,10);
	nwCompIp *psCompIp = NULL;
	psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
	
	if(psCompIp){
		psCompIp->lCtrl|=PRO_CENTER_COMMANDID_CLIENTOP;
	}
			
			
	utPltPutVar(psDbHead,"buff","{success:true}");
	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	return 0;
}

int lan_sspmdwpic(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	int iReturn;
	unsigned long lSid;
	char caFile[256],caPath[128],caFilename[128],caSid[16];
	unsigned int pic_time;
	unsigned int time_def;
	time_t now;
	time(&now);
	char opera[256] = "";
	
	iReturn = utMsgGetSomeNVar(psMsgHead,1,
	          "file",  UT_TYPE_STRING, 16, caSid);
	                    
	sprintf(caPath,"/home/ncmysql/nw/online/screen/");
	sprintf(caFilename,"%s.jpg",caSid);
	
	sprintf(caFile,"%s%s",caPath,caFilename);
	
	//printf("实时屏幕，下载图片：%s\n",caFile);
	
	pic_time = GetFileAttr(caFile);
	time_def = now - pic_time;;
	if(time_def > 12){
		sprintf(opera,"cp -f %s %s","/home/ncmysql/nw/online/screen/heiping.jpg",caFile);
		system(opera);
	}
			
	utPltFileDownload(iFd,"image/jpeg",caPath,caFilename,caFilename);
	
	return 0;
}

int lan_getOnlineSreen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	//utMsgPrintMsg(psMsgHead);
	
	char gname_in[32] = "";
	char sip_in[16] = "";
	char compname_in[32] = "";
	char path_in[128] = "";
	char width_in[8] = "";
	char height_in[8] = "";
	char compid_in[16] = "";
	
	utMsgGetSomeNVar(psMsgHead,7,
			"gname",		UT_TYPE_STRING,  sizeof(gname_in)-1, 		gname_in,
			"sip",			UT_TYPE_STRING,  sizeof(sip_in)-1, 			sip_in,
			"compname",	UT_TYPE_STRING,  sizeof(compname_in)-1, compname_in,
			"path",			UT_TYPE_STRING,  sizeof(path_in)-1, 		path_in,
			"compid",		UT_TYPE_STRING,  sizeof(compid_in)-1, 	compid_in,
			"width",		UT_TYPE_STRING,  sizeof(width_in)-1, 		width_in,
			"height",		UT_TYPE_STRING,  sizeof(height_in)-1, 	height_in);
			
	utPltDbHead *psDbHead = utPltInitDb();
			
	utPltPutVar(psDbHead,"gname",gname_in);
	utPltPutVar(psDbHead,"sip",sip_in);
	utPltPutVar(psDbHead,"compname",compname_in);
	utPltPutVarF(psDbHead,"path","/pronline/Msg?FunName@lan_sspmdwpic&file@%s",compid_in);
	utPltPutVar(psDbHead,"compid",compid_in);
	utPltPutVarF(psDbHead,"width","%d",atoi(width_in)-20);
	utPltPutVarF(psDbHead,"height","%d",atoi(height_in)-40);
	
	char sql[512] = "";
	char fieldName[32] = "";
	char devname[32] = "";
	char tmp[64] = "";
	char auth[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	sprintf(sql,"select display from usercommon where auth = '%s'",auth);
	pasDbOneRecord(sql,0,UT_TYPE_STRING,sizeof(fieldName),fieldName);
	//printf("在线计算机: %s\n",fieldName);
	int ireturn=-1;
	
	if(strcmp(fieldName,"devname")==0){
		sprintf(sql,"select devname from nwcompute where compid=%s",compid_in);
		ireturn = pasDbOneRecord(sql,0,UT_TYPE_STRING,sizeof(devname),devname);
		if(ireturn==0){
		  sprintf(tmp,"<b>用户名：</b>%s",devname);
		}
	}else if(strcmp(fieldName,"ip")==0){
	  sprintf(tmp,"<b>IP：</b>%s",sip_in);
	}else if(strcmp(fieldName,"compname")==0){
	  sprintf(tmp,"<b>机器名：</b>%s",compname_in);
	}
	utPltPutVar(psDbHead,"value",tmp);
	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/ssjk/lan_screen.html");
	
	return 0;
}

int lan_setScreenHeart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	
	char compid_in[16] = "";
	//utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();
	
	utMsgGetSomeNVar(psMsgHead,1,"compid",UT_TYPE_STRING,sizeof(compid_in)-1,compid_in);
	
	nwCompIp *psCompIp_tmp = NULL;
	unsigned long compid = strtoul(compid_in,NULL,10);
	
	psCompIp_tmp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);

	if(psCompIp_tmp){
		psCompIp_tmp->lCtrl|=PRO_CENTER_COMMANDID_CAPTURESCREEN2;
	}
	
	utPltPutVar(psDbHead,"buff","{success:true}");
	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	
	return 0;
}

int lan_getOnlineComp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	//utMsgPrintMsg(psMsgHead);
	char gname_in[32] = "";
	char sip_in[16] = "";
	char compname_in[32] = "";
	char path_in[128] = "";
	char compid_in[16] = "";
	
	struct tm *local;
	time_t t;
	t=time(NULL);
	local=localtime(&t);
	
	utMsgGetSomeNVar(psMsgHead,4,
		"gname",		UT_TYPE_STRING,  sizeof(gname_in)-1, 		gname_in,
		"sip",			UT_TYPE_STRING,  sizeof(sip_in)-1, 			sip_in,
		"compname",	UT_TYPE_STRING,  sizeof(compname_in)-1, compname_in,
		"compid",		UT_TYPE_STRING,  sizeof(compid_in)-1, 	compid_in);
			
	utPltDbHead *psDbHead = utPltInitDb();
	
	utPltPutVar(psDbHead,"gname",gname_in);
	utPltPutVar(psDbHead,"sip",sip_in);
	utPltPutVar(psDbHead,"compname",compname_in);
	utPltPutVar(psDbHead,"compid",compid_in);
	
	char sql[512] = "";
	char fieldName[32] = "";
	char devname[32] = "";
	char auth[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	sprintf(sql,"select display from usercommon where auth = '%s'",auth);
	pasDbOneRecord(sql,0,UT_TYPE_STRING,sizeof(fieldName),fieldName);
	//printf("在线计算机: %s\n",fieldName);
	int ireturn=-1;
	
	if(strcmp(fieldName,"devname")==0){
		sprintf(sql,"select devname from nwcompute where compid=%s",compid_in);
		ireturn = pasDbOneRecord(sql,0,UT_TYPE_STRING,sizeof(devname),devname);
		if(ireturn==0){
			utPltPutVar(psDbHead,"value",devname);
		}
	}else if(strcmp(fieldName,"ip")==0){
		utPltPutVar(psDbHead,"value",sip_in);
	}else if(strcmp(fieldName,"compname")==0){
		utPltPutVar(psDbHead,"value",compname_in);
	}
	
	char path[128] = "";
	snprintf(sql,sizeof(sql),"select max(file) from ncscreenlog_%4d%02d where compid = %s group by compid ",local->tm_year+1900,local->tm_mon+1,compid_in);
	//printf("sql:%s\n",sql);
	ireturn = pasDbOneRecord(sql,0,UT_TYPE_STRING,sizeof(path),path);
	if(ireturn == 0 && strlen(path)>0){
		utPltPutVar(psDbHead,"path",path);
	}else{
		utPltPutVar(psDbHead,"path","/home/ncmysql/nw/html/images/v8/computer_monitor.png");
	}
		
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/ssjk/lan_olCompute.html");
	
	return 0;
}

int lan_getOnlineSreen2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	//utMsgPrintMsg(psMsgHead);
	
	char gname_in[32] = "";
	char sip_in[16] = "";
	char compname_in[32] = "";
	char path_in[128] = "";
	char width_in[8] = "";
	char height_in[8] = "";
	char compid_in[16] = "";
	
	utMsgGetSomeNVar(psMsgHead,7,
			"gname",		UT_TYPE_STRING,  sizeof(gname_in)-1, 		gname_in,
			"sip",			UT_TYPE_STRING,  sizeof(sip_in)-1, 			sip_in,
			"compname",	UT_TYPE_STRING,  sizeof(compname_in)-1, compname_in,
			"path",			UT_TYPE_STRING,  sizeof(path_in)-1, 		path_in,
			"compid",		UT_TYPE_STRING,  sizeof(compid_in)-1, 	compid_in,
			"width",		UT_TYPE_STRING,  sizeof(width_in)-1, 		width_in,
			"height",		UT_TYPE_STRING,  sizeof(height_in)-1, 	height_in);
			
	utPltDbHead *psDbHead = utPltInitDb();
			
	utPltPutVar(psDbHead,"gname",gname_in);
	utPltPutVar(psDbHead,"sip",sip_in);
	utPltPutVar(psDbHead,"compname",compname_in);
	utPltPutVarF(psDbHead,"path","/pronline/Msg?FunName@lan_sspmdwpic&file@%s",compid_in);
	utPltPutVar(psDbHead,"compid",compid_in);
	utPltPutVarF(psDbHead,"width","%d",atoi(width_in)-20);
	utPltPutVarF(psDbHead,"height","%d",atoi(height_in)-40);
	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/ssjk/lan_screen2.html");
	
	return 0;
}

