#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "pasdb.h"
#include "ncwarn.h"
#include "nclimit.h"
#include "pasutl.h"

static char* ncCvtWarnlevel(int id){
	static char msg[16];
	if(id==1){
		strcpy(msg,"严重");
	}
	else if(id==2){
		strcpy(msg,"中度");
	}
	else{
		strcpy(msg,"一般");
	}
	return msg;
}

/* 即时通信内容查询  */
int ncWebImList(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
    char caWhere[512],caCond[64],caCond0[64],caDel[16];
    char caTotPg[16],caCurPg[16],caUserid[16]="";
    char caUsername[32],caTo[64],caFrom[64],caMesg[256]="";
    char caTemp[256];
    char caCount[32];
    char caExport[16];
    char caSip[16];
	char caVar[32];
	char caPlate[64];
	char caGroupid[32]="";
	char camid[17]="";
	long lStime=0;
	long lEtime=0;
	char *p;
	char caMesg8[128];
	long lNomatch;
	int group_flag;
	char caSdate_temp[24],caEdate_temp[24],caSdate[20],caEdate[20],caSdate1[8],caEdate1[8];
    char caWhereStr[6]="where";
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0,lSid,lTime,lService;
    int i,iNum,iPos;
    utPltDbHead *psDbHead;
   	int iReturn;
   	char caSearch[16];
   	char desMesg[256]="";
   	long lGroupid0,lRowNum,lStartRec,lCount,lSumUser,lGroupid9;
   	long sum_count=0;
   	unsigned long lFuncode;
   	char username[33]="";
   	char groupname[33]="";
   	 struct msginfo_s {
   		unsigned long lUserid;
   		unsigned long lSid;
   		unsigned long lTime;
   		unsigned long lService;
   		unsigned long lFuncode;
   		char caFrom[48];
   		char caTo[48];
   		char caMesg[256];
   		char sip[16];
   	};
   	char caConflag[16];
   	struct msginfo_s *pMsgInfo;
   	unsigned long lcurtime;
   	char caImdir[256];
   	char caMsgfile[256];
   	char caMsg[300];
   	char caMsg1[300],caMsg2[300];
   	char caDir[128],caFile[128];
   	char caCont[256];
   	char caPathfile[256];
   	char *p1,*p2;
   	int iLen;
   	char caTitlename[32];
   	char caName[32];
   	char caOrder[32];
   	unsigned long lpsCount,lptCount,ltCount;
 	pasDbCursor *psCur;
 	ncUserCont *psUser;
 	char caDispname[32],caIdfyno[20],caService[12],caFuncode[12],caIp[16];
 	
 	
 	  long lMbflag;
    char caKey[1024],caMname[32]; 
    char cbcode[1024],caSerno[32];
    char caOk[16],caXzmb[16];
    char szBuf[1024];
    long lId;
 	
 	
 	
 	
 	
   psDbHead = utPltInitDb();
//utMsgPrintMsg(psMsgHead); 
  utMsgDelVar(psMsgHead,"action");
	utMsgAdd_s(&psMsgHead,"action","06");
  ncOperWriteLog(psMsgHead);

	
	iReturn = utMsgGetSomeNVar(psMsgHead,13,
		            "del",        UT_TYPE_STRING, 12, caDel,
	                "search",     UT_TYPE_STRING, 12, caSearch,
	                "userid",     UT_TYPE_STRING, 12, caUserid,
	                "groupid",    UT_TYPE_STRING, 31, caGroupid,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					        "CurPg",      UT_TYPE_STRING, 12, caCurPg,
   	                "cond",       UT_TYPE_STRING, 63, caCond,
   	                "where",      UT_TYPE_STRING, 500,caWhere,
   	                "lcount",     UT_TYPE_STRING, 12, caCount,
   	                "plate",      UT_TYPE_STRING,63,caPlate,
   	                "mid",        UT_TYPE_STRING,16,camid,
   	                "exp",        UT_TYPE_STRING, 10, caExport,
   	                "con_flag",UT_TYPE_STRING,6,caConflag);
      
      	iReturn = utMsgGetSomeNVar(psMsgHead,13,
	                "username",   UT_TYPE_STRING, 31, caDispname,
	                "from",       UT_TYPE_STRING, 47, caFrom,
	                "to",         UT_TYPE_STRING, 47, caTo,
	                "mesg",       UT_TYPE_STRING, 47, caMsg,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "sdate1",     UT_TYPE_STRING, 5,caSdate1,
	                "edate1",     UT_TYPE_STRING, 5,caEdate1,
	                "service",    UT_TYPE_STRING, 11,caService,
	                "funcode",    UT_TYPE_STRING,8,caFuncode,
	                "ip",         UT_TYPE_STRING,15,caIp,
	                "osort",      UT_TYPE_STRING, 11, caOrder);
	                
      
    
    
       lMbflag=0;
        dsCltGetSessionValue(1,"mbflag",UT_TYPE_LONG,sizeof(long),&lMbflag);
        if(lMbflag!=10){
    	  iReturn = utMsgGetSomeNVar(psMsgHead,3,"mname",  UT_TYPE_STRING,12,caMname,
                                         "ok",     UT_TYPE_STRING,10,caOk,
                                         "xzmb",   UT_TYPE_STRING,10,caXzmb);
          if(strlen(caMname)>0){              
          memset(caKey,0,sizeof(caKey)); 
          dsCltGetSessionValue(1,"id",UT_TYPE_LONG,sizeof(long),&lId);
				  sprintf(szBuf,"select mbserno from ncuserbindmb where userid=%d ",lId);

				  pasDbOneRecord(szBuf,0,UT_TYPE_STRING,16,caSerno);
				  sprintf(szBuf,"select mbkey from ncmbserno where mbserno='%s' ",caSerno);
				 
				  pasDbOneRecord(szBuf,0,UT_TYPE_STRING,1000,caKey); 
			  

			  if(strlen(caXzmb)>0){
			   iReturn=ncMBAuth(psShmHead,caKey,caMname,caSerno,2);
			   if(iReturn<1){
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","校正失败");
					utPltPutVar(psDbHead,"fname","ncWebImList");
					utPltPutVar(psDbHead,"plate",caPlate);
   	      utPltPutVar(psDbHead,"con_flag",caConflag);
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
			   }
			   else{
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","校正成功");
					utPltPutVar(psDbHead,"fname","ncWebImList");
					utPltPutVar(psDbHead,"plate",caPlate);
   	      utPltPutVar(psDbHead,"con_flag",caConflag);
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
   				}
				}
				else{
				
			   iReturn=ncMBAuth(psShmHead,caKey,caMname,caSerno,1);

			   if(iReturn<1){
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","验证失败");
					utPltPutVar(psDbHead,"fname","ncWebImList");
					utPltPutVar(psDbHead,"plate",caPlate);
   	      utPltPutVar(psDbHead,"con_flag",caConflag);
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
			   }
			   else{

			   	dsCltSetSessionValue(1,"mbflag",UT_TYPE_LONG,4,10);
			   }
			  }
			    
				
				
			}
			else{
					psDbHead = utPltInitDb();
					   	utPltPutVar(psDbHead,"fname","ncWebImList");
   	          utPltPutVar(psDbHead,"con_flag",caConflag);
   	          utPltPutVar(psDbHead,"plate",caPlate);
   	          if(atol(caConflag)>0)
      					utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauthonline.htm");
      				else
   	          utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
   	          return 0;
				}                       
    }
    
    
      
                 
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caDispname,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),"未知"));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	                          	                
	if(!utStrIsSpaces(camid))
	{
		sprintf(caTemp,"select stime from nccontlog where sid=%s ",camid);
		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lStime);
		lStime=lStime-24*3600;
		lEtime=lStime+48*3600;
		}
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    if(!utStrIsSpaces(caDel)) {  /* 删除选中的邮件  */
		for(i=0;i<40;i++) {
        	sprintf(caVar,"chk%d",i);
        	p = utMsgGetVar_s(psMsgHead,caVar);
        	if(p) {
            	lSid = atol(p);
            	iReturn = pasDbExecSqlF("delete from nccontlog where sid = %lu ",lSid);
        	}
    	}
    }
		
    sprintf(caCond0,"'%c%s%c'",'%',caCond,'%');

    if(!utStrIsSpaces(caSearch)) {
        strcpy(caCount,"\0");
        strcpy(caWhere,"\0");
        lCurPg = 1;
    }
    else {
        lCurPg = atol(caCurPg);
        if(lCurPg <= 0) lCurPg = 1;
    }
    if(!utStrIsSpaces(caWhere)) {
    }
    else {
        lUserid0 = atol(caUserid);
        lGroupid9=atol(caGroupid);
        iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
        if(iReturn == 0 && lGroupid0 > 0) {
        	group_flag=1;
 //           sprintf(caWhere," and a.groupid = %lu ",lGroupid0);
        }
      if(group_flag==1||strlen(caUserid)>0||strlen(caGroupid)>0||atol(caConflag)>0){
     	  if(strlen(caSdate)==0)
    		strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0))); 
    		if(strlen(caEdate)==0)  	
    		strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
    		if(strlen(caSdate1)==0)
    		strcpy(caSdate1,"00:00");
    		if(strlen(caEdate1)==0)
    		strcpy(caEdate1,"23:59"); 
    		
    		sprintf(caSdate_temp,"%s %s",caSdate,caSdate1);
    		sprintf(caEdate_temp,"%s %s",caEdate,caEdate1);
    		lStime=utTimStrToLong("%Y/%m/%d %H:%M:%S",caSdate_temp);
    		lEtime=utTimStrToLong("%Y/%m/%d %H:%M:%S",caEdate_temp);  
    		
    		utStrAddF(caWhere," and b.stime>=%lu ",lStime);
    		utStrAddF(caWhere," and b.stime<=%lu ",lEtime); 	
      }
       if(lUserid0 > 0) {
            utStrAddF(caWhere," and b.userid = %lu ",lUserid0);
        }
        if(group_flag==1){
        	utStrAddF(caWhere," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu))",lGroupid0,lGroupid0);
        }
        

        if(strlen(caGroupid)!=0) {
        	 if(strcmp(caGroupid,"0")==0)
        	     utStrAddF(caWhere," and (a.groupid = 0 ) ");
        	  else
        			sprintf(caWhere+strlen(caWhere)," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu))",lGroupid9,lGroupid9);
        }
       
 //  printf("caWhere=%s\n",caWhere);
 //  printf("camid=%s\n",camid);     
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caWhere," and (b.mfrom like %s or b.mto like %s or b.mesg like Binary(%s))",
                                      caCond0,caCond0,caCond0);
        }
       
    }
    
    if(!utStrIsSpaces(caExport)) {
        lStartRec = 0;
        lRowNum = 49999L;
    }
    else
    lRowNum = 36;
    
    pMsgInfo = malloc(lRowNum * sizeof(struct msginfo_s));
    if(pMsgInfo == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","ncMsgInfo",
                               "Memory Error");
        return 0;
    }        
    memset(pMsgInfo,0,sizeof(pMsgInfo));
    lStartRec = (lCurPg - 1) * lRowNum;  
    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }




    if(utStrIsSpaces(caCount)) {
        if( strstr(caWhere,"a.")) { /* 涉及用户信息  */
            sprintf(caTemp,"select count(*) from nccontlog b,ncuser a %s b.userid=a.userid and  %s",
                     caWhereStr,ncUtlTrimOrder(caWhere+iPos));
        }
        else {
            sprintf(caTemp,"select count(*) from  nccontlog b %s %s",caWhereStr,ncUtlTrimOrder(caWhere+iPos));
        }
//     printf("caTemp=%s\n",caTemp);
        iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0301记录查询出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
    }
    else {
        lCount = atol(caCount);
    }
  
    if(strstr(caWhere,"order by")==NULL){
    	if(strlen(caOrder)>0){
    		sprintf(caWhere+strlen(caWhere)," order by %s desc ",caOrder);
    	}
    	else
       strcat(caWhere," order by b.stime desc ");
    }
    
    if( strstr(caWhere,"a.")) { /* 涉及用户信息  */
            
        psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),b.sid,b.userid,b.stime,b.service,b.mfrom,b.mto,b.mesg,b.funcode \
                from  nccontlog b,ncuser a  where b.userid = a.userid  %s limit %d,%d",caWhere,lStartRec,lRowNum);
    }
    else { 

        psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),b.sid,b.userid,b.stime,b.service,b.mfrom,b.mto,b.mesg,b.funcode \
                from  nccontlog b %s %s limit %d,%d",caWhereStr,caWhere+iPos,lStartRec,lRowNum);
    }
    
 //  printf("select inet_ntoa(b.sip),b.sid,b.userid,b.stime,b.service,b.mfrom,b.mto,b.mesg,b.funcode \
 //               from  nccontlog b %s %s limit %d,%d\n",caWhereStr,caWhere+iPos,lStartRec,lRowNum);
     
  #ifdef LDEBUG
	 printf("caWhere=%s\n",caWhere);
	#endif     
  ncUtlGetWordBetween(caWhere,"Binary(\'\%","\%\')",caMesg8,100);
  lNomatch=0;
  
 // printf("caMesg8=%s\n",caMesg8);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
   
    iReturn = 0;
    utPltSetCvtHtml(0);
    utPltPutVar(psDbHead,"cond",caCond);
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"from",caFrom);
    utPltPutVar(psDbHead,"to",caTo);
    utPltPutVar(psDbHead,"mesg",caMsg);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    
    if(strlen(caSdate)==0){
    strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
  }
  if(strlen(caEdate)==0){
    strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
  }
   if(strlen(caSdate1)==0){
   	strcpy(caSdate1,"00:00");
  }
  if(strlen(caEdate1)==0){
  	strcpy(caEdate1,"23:59");
  }
    
    
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"sdate1",caSdate1);
    utPltPutVar(psDbHead,"edate1",caEdate1);
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"funcode",caFuncode);
    utPltPutVar(psDbHead,"ip",caIp);
    if(strlen(caOrder)==0){
    	utPltPutVar(psDbHead,"osort","b.stime");
    }
    else{
    	utPltPutVar(psDbHead,"osort",caOrder);
    }
      
      
    utPltSetCvtHtml(1);
    lUserid=0;
    memset(caMesg,0,sizeof(caMesg));
    memset(caTo,0,sizeof(caTo));
    memset(caFrom,0,sizeof(caFrom));
    memset(caSip,0,sizeof(caSip));
    lFuncode=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_LONG,4,&lService,
  	                               UT_TYPE_STRING,47,caFrom,
  	                               UT_TYPE_STRING,47,caTo,
  	                               UT_TYPE_STRING,255,caMesg,
  	                               UT_TYPE_LONG,4,&lFuncode);
   	iNum = 0;
  
// setlocale(LC_ALL, "zh_CN.GBK"); 
  char caTmesg[256],caTmesg8[256];
   	while((iReturn == 0||iReturn==1405) && iNum < lRowNum&& iNum < lCount) {
   	    i++;
      /*将caMesg按50字符宽度加回车*/ 
//对MYSQL单个汉字非命中过滤





     pasCharsetCvt(PAS_CCODE_GB,PAS_CCODE_UTF8,caMesg8,caTmesg8,255);
      pasCharsetCvt(PAS_CCODE_GB,PAS_CCODE_UTF8,caMesg,caTmesg,255);  

  
  
     if((strlen(caTmesg8)>0&&strstr(caTmesg,caTmesg8))||strlen(caTmesg8)==0){
      
     


 		if(!utStrIsSpaces(caMesg)){
 			    if(lFuncode==4||lFuncode==5){
      	  strcpy(pMsgInfo[iNum].caMesg,caMesg);
      	}
      	else
	        ncStrFixedCol(caMesg,pMsgInfo[iNum].caMesg,48,255);
        } 
        else
        strcpy(pMsgInfo[iNum].caMesg,"");
        
        strcpy(pMsgInfo[iNum].sip,caSip);
        pMsgInfo[iNum].lUserid=lUserid;
        pMsgInfo[iNum].lSid=lSid;
        pMsgInfo[iNum].lTime=lTime;
        pMsgInfo[iNum].lService=lService;
        strcpy(pMsgInfo[iNum].caFrom,caFrom);
        strcpy(pMsgInfo[iNum].caTo,caTo);
        
        pMsgInfo[iNum].lFuncode=lFuncode;               
         iNum++;
     }
    lUserid=0;  
    memset(caMesg,0,sizeof(caMesg));
    memset(caTo,0,sizeof(caTo));
    memset(caFrom,0,sizeof(caFrom));
    memset(caSip,0,sizeof(caSip));
    lFuncode=0;        
     iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_LONG,4,&lService,
  	                               UT_TYPE_STRING,47,caFrom,
  	                               UT_TYPE_STRING,47,caTo,
  	                               UT_TYPE_STRING,255,caMesg,
  	                               UT_TYPE_LONG,4,&lFuncode);
   }
    pasDbCloseCursor(psCur);
    
        if(!utStrIsSpaces(caExport)) {
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"ncimmsg%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0891阻断查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,ncLang("1050即时通讯内容导出\n"));
       
       
        ncUtlPrintCsv(fp,7,
                       UT_TYPE_STRING,ncLang("0693姓  名"),
                       UT_TYPE_STRING,ncLang("0147部门"),
                       UT_TYPE_STRING,ncLang("0521时间"),
                       UT_TYPE_STRING,ncLang("0253服务"),
                       UT_TYPE_STRING,ncLang("1050发送者"),
                       UT_TYPE_STRING,ncLang("1051接收者"),
                       UT_TYPE_STRING,ncLang("0407内容"));
                       
        for(i=0;i<iNum;i++)
    		{
    			  psUser=ncUtlBufGetUserInfo(psShmHead,pMsgInfo[i].lUserid);
		        if(psUser){
		        	if(psUser->psGroup){
		        	   			  ncUtlPrintCsv(fp,7,
                       UT_TYPE_STRING,ncUtlBufGetUsername(psShmHead,pMsgInfo[iNum-1-i].lUserid,"\0"),
                       UT_TYPE_STRING,psUser->psGroup->groupname,
                       UT_TYPE_STRING,utTimFormat("%Y-%m-%d %H:%M:%S",pMsgInfo[iNum-1-i].lTime),
                       UT_TYPE_STRING,ncUtlImName(pMsgInfo[iNum-1-i].lService),
                       UT_TYPE_STRING,pMsgInfo[iNum-1-i].caFrom,
                       UT_TYPE_STRING,pMsgInfo[iNum-1-i].caTo,
                       UT_TYPE_STRING,pMsgInfo[iNum-1-i].caMesg);
                      }
                      else{
                       ncUtlPrintCsv(fp,7,
                       UT_TYPE_STRING,ncUtlBufGetUsername(psShmHead,pMsgInfo[iNum-1-i].lUserid,"\0"),
                       UT_TYPE_STRING,"未知",
                       UT_TYPE_STRING,utTimFormat("%Y-%m-%d %H:%M:%S",pMsgInfo[iNum-1-i].lTime),
                       UT_TYPE_STRING,ncUtlImName(pMsgInfo[iNum-1-i].lService),
                       UT_TYPE_STRING,pMsgInfo[iNum-1-i].caFrom,
                       UT_TYPE_STRING,pMsgInfo[iNum-1-i].caTo,
                       UT_TYPE_STRING,pMsgInfo[iNum-1-i].caMesg);
                      }
		        	
		        }
		        else
    			  ncUtlPrintCsv(fp,7,
                       UT_TYPE_STRING,ncUtlBufGetUsername(psShmHead,pMsgInfo[iNum-1-i].lUserid,"\0"),
                       UT_TYPE_STRING,"",
                       UT_TYPE_STRING,utTimFormat("%Y-%m-%d %H:%M:%S",pMsgInfo[iNum-1-i].lTime),
                       UT_TYPE_STRING,ncUtlImName(pMsgInfo[iNum-1-i].lService),
                       UT_TYPE_STRING,pMsgInfo[iNum-1-i].caFrom,
                       UT_TYPE_STRING,pMsgInfo[iNum-1-i].caTo,
                       UT_TYPE_STRING,pMsgInfo[iNum-1-i].caMesg);
    			
        }
        if(pMsgInfo) free(pMsgInfo); 
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        
         char caUsername2[32];
			   memset(caUsername2,0,sizeof(caUsername2));
			   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
			   ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"导出即时通讯日志");
      
        
        
        
        return 0;
    }
    
    
    for(i=0;i<iNum;i++)
    {
    	utPltPutLoopVar(psDbHead,"service", iNum-i,ncUtlImName(pMsgInfo[i].lService));
        utPltPutLoopVarF(psDbHead,"num",   iNum-i,"%lu",i);
        utPltPutLoopVarF(psDbHead,"sid",   iNum-i,"%lu",pMsgInfo[i].lSid);
        utPltPutLoopVar(psDbHead,"sip",iNum-i,pMsgInfo[i].sip);
        utPltPutLoopVarF(psDbHead,"userid",   iNum-i,"%d",pMsgInfo[i].lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum-i,
             ncUtlBufGetUsername(psShmHead,pMsgInfo[i].lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"sdate", iNum-i,utTimFormat("%Y-%m-%d %H:%M:%S",pMsgInfo[i].lTime));
        
//        strReplace(pMsgInfo[i].caFrom,caTemp,"#","^");
//        utPltPutLoopVar(psDbHead,"from", iNum-i,caTemp);
//        strReplace(pMsgInfo[i].caTo,caTemp,"#","^");
//        utPltPutLoopVar(psDbHead,"from", iNum-i,caTemp);
//        utPltPutLoopVar(psDbHead,"to", iNum-i,caTemp);
       utPltPutLoopVar(psDbHead,"from", iNum-i,pMsgInfo[i].caFrom);
        utPltPutLoopVar(psDbHead,"to", iNum-i,pMsgInfo[i].caTo);
utStrReplaceWith(pMsgInfo[i].caFrom,"#","^^");
utStrReplaceWith(pMsgInfo[i].caTo,"#","^^");
       utPltPutLoopVar(psDbHead,"from1", iNum-i,pMsgInfo[i].caFrom);
        utPltPutLoopVar(psDbHead,"to1", iNum-i,pMsgInfo[i].caTo);
        if(pMsgInfo[i].lFuncode==1)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,ncLang("1487登录"));
        else if(pMsgInfo[i].lFuncode==2)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,ncLang("0588退出"));
        else if(pMsgInfo[i].lFuncode==3)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,ncLang("1488发送消息"));
        else if(pMsgInfo[i].lFuncode==4)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,ncLang("1025发送文件"));
        else if(pMsgInfo[i].lFuncode==5)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,ncLang("1026接收文件"));
        else if(pMsgInfo[i].lFuncode==6)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,ncLang("1489接收消息"));
 
 
 
       if(pMsgInfo[i].lFuncode==4||pMsgInfo[i].lFuncode==5){
       	  utStrDelSpaces(pMsgInfo[i].caMesg);
       		p1=strrchr(pMsgInfo[i].caMesg,' ');
       		if(p1){
       			strcpy(caImdir,p1+1);
       			iLen=p1-&pMsgInfo[i].caMesg[0];
       			memset(caMsgfile,0,sizeof(caMsgfile));
       			strncpy(caMsgfile,pMsgInfo[i].caMesg,iLen);
       		}
  //    		iReturn=sscanf(pMsgInfo[i].caMesg,"%s %s",caMsgfile,caImdir);
 //     		if(iReturn==2){
      		if(strlen(caMsgfile)>0&&strlen(caImdir)>0){
      			if(pMsgInfo[i].lFuncode==4)
      			sprintf(caMsg,"%s %s",ncLang("1025发送文件"),caMsgfile);
      			else
      			sprintf(caMsg,"%s %s",ncLang("1026接收文件"),caMsgfile);
      	
      			if(pMsgInfo[i].lFuncode==4)
      			sprintf(caMsg,"%s %s",ncLang("1025发送文件"),caMsgfile);
      			else
      			sprintf(caMsg,"%s %s",ncLang("1026接收文件"),caMsgfile);
      		

						p1=&caImdir[0];
						while(p1){
							p2=strchr(p1,'/');
							if(p2){
							   p1=p2+1;
							  }
							else{
								 strcpy(caFile,p1);
								iLen=strlen(caImdir)-strlen(p1)-1;
								memcpy(caDir,caImdir,iLen);		
								caDir[iLen]='\0';

								break;
							}
						}
 
      			
      			strcpy(caMsg2,"</a>");
      			sprintf(caMsg1,"<a href=\"#xxm\" onClick=goto(\"/pronline/Msg?FunName@ncWebBackFileDownload&path@%s/Im/%s&fname@%s&fname2@%s\")>",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"),caDir,caFile,caMsgfile);
  utPltSetCvtHtml(0);     			
      			utPltPutLoopVar(psDbHead,"mesg1", iNum-i,caMsg1);
      			utPltPutLoopVar(psDbHead,"mesg2", iNum-i,caMsg2);
  utPltSetCvtHtml(1);
		  		}
		  		else
		  		strcpy(caMsg,pMsgInfo[i].caMesg);
		  	}
		  	else
		  	  strcpy(caMsg,pMsgInfo[i].caMesg);
		  	  
        utPltPutLoopVar(psDbHead,"mesg", iNum-i,caMsg);
        
        psUser=ncUtlBufGetUserInfo(psShmHead,pMsgInfo[i].lUserid);
        if(psUser){
        	if(psUser->psGroup)
        	  utPltPutLoopVar(psDbHead,"dept",iNum-i,psUser->psGroup->groupname);
        	  
        	  
        	utPltPutLoopVar(psDbHead,"position",iNum-i,psUser->position);
        }
        
    }

    lTotPg = (lCount - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum ) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
 
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
   free(pMsgInfo); 
   
   
   
//输出即时通讯服务
    psCur = pasDbOpenSqlF("select sid,name from ncipindex where groupid=1 order by name ");
          
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
  
   lSid=0;
   memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
  	                               UT_TYPE_STRING,31,caName);
  	                              
  	                              
   	iNum = 0;
   	while(iReturn == 0 ||iReturn==1405) {
        iNum++;
       
        utPltPutLoopVarF(psDbHead,"msid",   iNum,"%lu",lSid);
        utPltPutLoopVar(psDbHead, "mname",  iNum,caName);
    
      
    lSid=0;
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
  	                               UT_TYPE_STRING,31,caName);
    }
    pasDbCloseCursor(psCur);
   
   
   
   
   
   
   
   
    utPltPutVar(psDbHead,"title_name",caTitlename);
    if(atol(caConflag)>0)
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/nc_immononlinelist.htm");
    else
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}


/* 显示高级查询界面  */               
int ncWebImDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    long iReturn,lGroupid,lGroupid0;
    char caSmt[256];
	char caService[12];
	char plate_flag[2]="";
	 char caSdate[24],caEdate[24],caSdate1[24],caEdate1[24];
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect;
    char caGroupid[32];
    ncUserContHead  *psHead;
    ncUserCont      *psUser,*psUser0;
    int i,iNum;
    unsigned long ltime;
    unsigned long userid;
    char caDispname[64];
    pasDbCursor *psCur;
    psDbHead = utPltInitDb();
    iReturn = utMsgGetSomeNVar(psMsgHead,1,
	                "groupid",    UT_TYPE_STRING, 31, caGroupid);  
    ltime=time(0);            
	  ncUtlMonthDate(ltime,caSdate,caEdate);               
	  //用户
    psHead = (ncUserContHead *)utShmArray(psShmHead,NC_LNK_ALLUSER);
    if(psHead == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("0403内存不正确"));
         return 0;
    }
    psUser0 = (ncUserCont *)((char *)psHead + psHead->lContPos); 
   
	    for(i=0,iNum=1;i<psHead->lSumUser ;i++) {
      psUser = &psUser0[i];
      if(strlen(caGroupid)>0){
      	if(psUser->groupid==atol(caGroupid)){
		      utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
		      utPltPutLoopVarF(psDbHead,"uid",iNum,"%lu",psUser->userid);
		      utPltPutLoopVarF(psDbHead,"uname",iNum,"%s",psUser->dispname);
		      iNum++;
	      	if(iNum>500) break;
	      }
	    }
	    else{
	    	    utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
      			utPltPutLoopVarF(psDbHead,"uid",iNum,"%lu",psUser->userid);
      			utPltPutLoopVarF(psDbHead,"uname",iNum,"%s",psUser->dispname);
	    	iNum++;
	    	if(iNum>500) break;
	    }
    
  } 
   utPltPutVarF(psDbHead,"sum","%lu",iNum); 
    
    utMsgGetSomeNVar(psMsgHead,1,
		            "plate_flag",        UT_TYPE_STRING, 1, plate_flag);
	utPltPutVar(psDbHead,"plate_flag",plate_flag);
    strcpy(caGroupid,"\0");
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    psCur = pasDbOpenSql("select groupid,groupname from ncgroup   ",0);
    iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&userid,
    	                               UT_TYPE_STRING,63,caDispname);
    iNum = 0; 	                               
    while((iReturn==0)||(iReturn==1405))
    {
  	
        iNum++;       
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%lu",userid);
        utPltPutLoopVar(psDbHead,"dispname",iNum,caDispname);
    iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&userid,
      	                               UT_TYPE_STRING,63,caDispname);
    }
    pasDbCloseCursor(psCur);
	strcpy(caService,"0");
    pSelect= pasUtlCvtInputSelect("service",caService,ncLang("0452全部,0;QQ,6;MSN,10;ICQ,11;YHMSG,7;"));
    utPltPutVar(psDbHead,"sserv", pSelect);
    free(pSelect);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    
    if(atoi(plate_flag)==1)
    {

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_im_hsearleft.htm");
    return;
	}
    else
    {
    utPltPutVar(psDbHead,"plate","nc/nc_im_listnow.htm");
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_im_hsear.htm");
    return;
	}
}



/* 高级查询  */
int ncWebImHsearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  utPltDbHead *psDbHead;
    char caWhere[512];
    char caUserid[16];
    char caIdfyno[20];
    char caDispname[32],caSdate[32];
    char caGroupid[12];
    char plate_flag[2]="";
	  char caFrom[48],caTo[48],caMsg[128];
    char caEdate[20],caOrder[12],caOrderby[12];
    char caSdate1[10],caEdate1[10],caSdate_temp[24],caEdate_temp[24];
    unsigned long lUserid,lGroupid,lStime,lEtime;
   	int  iReturn;
   	char caService[12];
   	long lGroupid0,lService;
   	unsigned long lcurtime;
   	char caConflag[8];
   	char caFuncode[12];
   	char caIp[20];
   	char ids[256];
   	int i2,lUserid2;
   	int group_flag;
   	char caUsername_match[8],caIdfyno_match[8],caIp_match[8];
   	pasDbCursor *psCur;
	strcpy(caWhere,"\0");
//	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,21,
	                "username",   UT_TYPE_STRING, 31, caDispname,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "userid",     UT_TYPE_STRING, 12,caUserid,
	                "from",       UT_TYPE_STRING, 47, caFrom,
	                "to",         UT_TYPE_STRING, 47, caTo,
	                "mesg",       UT_TYPE_STRING, 47, caMsg,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "sdate1",     UT_TYPE_STRING,5,caSdate1,
	                "edate1",     UT_TYPE_STRING,5,caEdate1,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "orderby",    UT_TYPE_STRING, 2, caOrderby,
	                "service",    UT_TYPE_STRING, 11,caService,
	                "plate_flag", UT_TYPE_STRING,1,plate_flag,
	                "funcode",    UT_TYPE_STRING,8,caFuncode,
	                "ip",         UT_TYPE_STRING,15,caIp,
	                "con_flag",UT_TYPE_STRING,6,caConflag,
	                "username_match",UT_TYPE_STRING,2,caUsername_match,
	                "idfyno_match",   UT_TYPE_STRING,2,caIdfyno_match,
	                "ip_match",       UT_TYPE_STRING,2,caIp_match);
	//  utMsgPrintMsg(psMsgHead);
	  utStrDelSpaces(caSdate);
	  utStrDelSpaces(caEdate);
	  memset(caWhere,0,sizeof(caWhere));
	  if(!utStrIsSpaces(caSdate)) {
	  //	printf("lEN=%d\n",strlen(caSdate));
    	if(strlen(caSdate)==10){
    		if(strlen(caSdate1)==0){
    		  sprintf(caSdate_temp,"%s 00:00",caSdate);
    		}
    		else{
    			 sprintf(caSdate_temp,"%s %s",caSdate,caSdate1);
    		}
    	 }
    	else
    		strcpy(caSdate_temp,caSdate);
    	
		 lStime = utTimStrToLong("%Y/%m/%d %H:%M",caSdate_temp);
       
    }
    if(!utStrIsSpaces(caEdate)) {
   if(strlen(caEdate)==10){
   	   if(strlen(caEdate1)==0)
    	     sprintf(caEdate_temp,"%s 23:59",caEdate);
    	 else
    	     sprintf(caEdate_temp,"%s %s",caEdate,caEdate1);
    }
    else
      strcpy(caEdate_temp,caEdate);
      
      
		  lEtime = utTimStrToLong("%Y/%m/%d %H:%M",caEdate_temp);
       
    }
	  
	  group_flag=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {  /* 部门权利  */
        group_flag=1;
    }
    
    if(group_flag==1||strlen(caUserid)>0||strlen(caGroupid)>0||atol(caConflag)>0||strlen(caDispname)>0||strlen(caFrom)>0||strlen(caTo)>0||strlen(caMsg)>0||strlen(caIdfyno)>0||strlen(caService)>0||strlen(caFuncode)>0||strlen(caIp)>0){
     	  if(strlen(caSdate)==0){
    			strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
    		  utMsgDelVar(psMsgHead,"sdate");
					utMsgAdd_s(&psMsgHead,"sdate",caSdate);
    	  } 
    		if(strlen(caEdate)==0){  	
    			strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
    			utMsgDelVar(psMsgHead,"edate");
					utMsgAdd_s(&psMsgHead,"edate",caEdate);
    	  }
    		if(strlen(caSdate1)==0){
    			strcpy(caSdate1,"00:00");
    			utMsgDelVar(psMsgHead,"sdate1");
					utMsgAdd_s(&psMsgHead,"sdate1",caSdate1);
    	  }
    		if(strlen(caEdate1)==0){
    			strcpy(caEdate1,"23:59"); 
    			utMsgDelVar(psMsgHead,"edate1");
					utMsgAdd_s(&psMsgHead,"edate1",caEdate1);
    	}
    		
    		sprintf(caSdate_temp,"%s %s",caSdate,caSdate1);
    		sprintf(caEdate_temp,"%s %s",caEdate,caEdate1);
    		lStime=utTimStrToLong("%Y/%m/%d %H:%M:%S",caSdate_temp);
    		lEtime=utTimStrToLong("%Y/%m/%d %H:%M:%S",caEdate_temp);  
    		 	
      }
    
     if(lStime>0)
     		utStrAddF(caWhere," and b.stime  >= %lu ",lStime);
      
      if(lEtime>0)
         utStrAddF(caWhere," and b.stime  <= %lu ",lEtime);
    
    if(atol(caUserid)>0) {
        utStrAddF(caWhere," and b.userid = %s",caUserid);
    }
    if(group_flag==1){
    	utStrAddF(caWhere," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu))",lGroupid0,lGroupid0);
    }
    if(!utStrIsSpaces(caGroupid)) {
    	 if(strcmp(caGroupid,"0")==0)
        	 utStrAddF(caWhere," and (a.groupid = 0 ) ");
       else
        utStrAddF(caWhere," and (a.groupid = %s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
    }

     if(!utStrIsSpaces(caIp))
    	{
    		if(atol(caIp_match)==0)
				  sprintf(caWhere+strlen(caWhere)," and b.sip=%lu ",ntohl(utComHostAddress(caIp)));
				else
				  sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caIp,'%');
    }
    
    

    if(strlen(caIdfyno)!=0){
	    if(atol(caIdfyno_match)==0)
		  		utStrAddF(caWhere," and a.idfyno = '%s' ",caIdfyno);
		  else
		      sprintf(caWhere+strlen(caWhere)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
    }

	  if(!utStrIsSpaces(caFuncode)){
    	utStrAddF(caWhere," and b.funcode  = %lu ",atol(caFuncode));
    }
	  
    if((!utStrIsSpaces(caDispname))&&(utStrIsSpaces(caUserid))) {
        utStrAddF(caWhere," and (a.dispname like '%c%s%c' or a.username like '%c%s%c') ",'%',caDispname,'%','%',caDispname,'%');
    }
    if(!utStrIsSpaces(caFrom)) {
        utStrAddF(caWhere," and (b.mfrom like '%c%s%c' or b.mto like '%c%s%c')",'%',caFrom,'%','%',caFrom,'%');
    }
    if(!utStrIsSpaces(caTo)) {
        utStrAddF(caWhere," and (b.mfrom like '%c%s%c' or b.mto like '%c%s%c')",'%',caTo,'%','%',caTo,'%');
    }
    if(!utStrIsSpaces(caMsg)) {
        utStrAddF(caWhere," and b.mesg like Binary('%c%s%c')",'%',caMsg,'%');
    }
	
	if(!utStrIsSpaces(caService)) {
		lService = atol(caService);
		if(lService > 0) {
            utStrAddF(caWhere," and b.service = %d ",lService);
		}
    }
    if(!utStrIsSpaces(caOrder)) {
        if(strcmp(caOrder,"date")==0) {
            utStrAddF(caWhere," order by b.stime ");
        }
        else if(strcmp(caOrder,"from") == 0) {
            utStrAddF(caWhere," order by b.mfrom ");
        }
        else if(strcmp(caOrder,"to") == 0) {
            utStrAddF(caWhere," order by b.mto ");
        }
        else if(strcmp(caOrder,"service") == 0) {
            utStrAddF(caWhere," order by b.service ");
        }
        else if(strcmp(caOrder,"name") == 0) {
            utStrAddF(caWhere," order by b.userid ");
        }
        if(!utStrIsSpaces(caOrderby)) {
            if(caOrderby[0] == '1') {     
                utStrAddF(caWhere," desc ");
            }
        }
    }

  utMsgDelVar(psMsgHead,"where");
	utMsgAdd_s(&psMsgHead,"where",caWhere);

//printf("caWhere2=%s\n",caWhere);	
	ncWebImList(psShmHead,iFd,psMsgHead);
    return 0;
	
}


/* 敏感信息告警设定 */

int ncWebImFilterSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caAdd[16],caModi[16],caApp[16],caDel[16],caFlags[16],caService[16];
    char caUflags[16],caRid[16];
    char caRname[32],caRnum[16],caFrom[64],caTo[64],caMesg[64],caFun[16],caTz[16];
    char caSid[16];
    char caCurPg[16],caTotRec[16];
	char caUsername[64],caGroupname[64],caTzdz[64];
    char caRdesc[512];
    char *p;
    int i,iNum,iReturn;
	int lSource;
    utPltDbHead *psDbHead;
   	long lStartRec,lCount,lCurPg,lTotRec,lRowNum,lTotPg;
   	long lSid,lRnum,lTz,lService,lFun,lFlags;
    char caTemp[1024];
 	pasDbCursor *psCur;
 	
utMsgPrintMsg(psMsgHead);	
	iReturn = utMsgGetSomeNVar(psMsgHead,24,
	                "add",        UT_TYPE_STRING, 12, caAdd,
	                "modi",       UT_TYPE_STRING, 12, caModi,
	                "app",        UT_TYPE_STRING, 12, caApp,
	                "del",        UT_TYPE_STRING, 12, caDel,
	                "sid",        UT_TYPE_STRING, 12, caSid,
	                "rname",      UT_TYPE_STRING, 31, caRname,
	                "username",   UT_TYPE_STRING, 47, caUsername,
	                "groupname",  UT_TYPE_STRING, 47, caGroupname,
	                "rnum",       UT_TYPE_STRING, 12, caRnum,
	                "from",      UT_TYPE_STRING, 63, caFrom,
	                "to",        UT_TYPE_STRING, 63, caTo,
	                "mesg",       UT_TYPE_STRING, 63, caMesg,
	                "tz",         UT_TYPE_STRING, 12, caTz,
	                "tzdz",       UT_TYPE_STRING, 63, caTzdz,
	                "service",    UT_TYPE_STRING, 12, caService,
	                "fun",        UT_TYPE_STRING, 12, caFun,
	                "flags",      UT_TYPE_STRING, 12, caFlags,
					"CurPg",      UT_TYPE_STRING, 12, caCurPg,
					"TotRec",     UT_TYPE_STRING, 12, caTotRec);

    lTotRec = atol(caTotRec);
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    lFlags  = atol(caFlags);
    lRnum = atol(caRnum);
    lSid = atol(caSid);
    lTz = atol(caTz);
    lService  = atol(caService);
	lFun = atol(caFun);
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
    	   lSid = pasGetSid("nccontfilter","sid");
    	   sprintf(caTemp,"insert into nccontfilter(sid,num,rname,username,groupname,mfrom,mto,mesg,service,funcode,tzfs,tzdz,flags) \
                    values (%lu,%lu,'%s','%s','%s','%s','%s','%s',%lu,%lu,%lu,'%s',%lu)",
                    lSid,lRnum,caRname,caUsername,caGroupname,caFrom,caTo,caMesg,lService,lFun,lTz,caTzdz,lFlags);
				 utMsgDelVar(psMsgHead,"opmsg");
				 utMsgAdd_s(&psMsgHead,"opmsg",caTemp);
	       utMsgDelVar(psMsgHead,"action");
	       utMsgAdd_s(&psMsgHead,"action","04");
				 ncOperWriteLog(psMsgHead);
        iReturn = pasDbExecSqlF("insert into nccontfilter(sid,num,rname,username,groupname,mfrom,mto,mesg,service,funcode,tzfs,tzdz,flags) \
                    values (%lu,%lu,'%s','%s','%s','%s','%s','%s',%lu,%lu,%lu,'%s',%lu)",
                    lSid,lRnum,caRname,caUsername,caGroupname,caFrom,caTo,caMesg,lService,lFun,lTz,caTzdz,lFlags);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0797邮件分类"),ncLang("0311记录增加出错%s"),pasDbErrorMsg(NULL));
            pasDbRollback(NULL);
            return 0;
        }        
        else {
            if(lTotRec > 0) lTotRec = lTotRec + 1;
            pasDbCommit(NULL);
        }
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
   	    for(i=0;i<50;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
   	        	  sprintf(caTemp,"delete from nccontfilter where sid = %s",p);
				 				utMsgDelVar(psMsgHead,"opmsg");
				 				utMsgAdd_s(&psMsgHead,"opmsg",caTemp);
				 				utMsgDelVar(psMsgHead,"action");
	              utMsgAdd_s(&psMsgHead,"action","04");
				 				ncOperWriteLog(psMsgHead);
                iReturn = pasDbExecSqlF("delete from nccontfilter where sid = %s",p);
                if(iReturn == 0) {
                    if(lTotRec > 0) lTotRec = lTotRec-1;
                    pasDbCommit(NULL);
                }
   	        }
   	    }
    }
    else if(!utStrIsSpaces(caModi)) { /* 修改记录  */
        if(!utStrIsSpaces(caSid)) {
            lSid = atol(caSid);
            sprintf(caTemp,"update nccontfilter set num=%lu,rname='%s',username='%s',groupname='%s',mfrom='%s',mto='%s',mesg='%s',service=%lu,funcode=%d,tzfs=%d,tzdz='%s',flags=%d where sid = %lu",
                       lRnum,caRname,caUsername,caGroupname,caFrom,caTo,caMesg,lService,lFun,lTz,caTzdz,lFlags,lSid);
				 		utMsgDelVar(psMsgHead,"opmsg");
				 	  utMsgAdd_s(&psMsgHead,"opmsg",caTemp);
				 	  utMsgDelVar(psMsgHead,"action");
	          utMsgAdd_s(&psMsgHead,"action","04");
				 		ncOperWriteLog(psMsgHead);
            iReturn = pasDbExecSqlF("update nccontfilter set num=%lu,rname='%s',username='%s',groupname='%s',mfrom='%s',mto='%s',\
				         mesg='%s',service=%lu,funcode=%d,tzfs=%d,tzdz='%s',flags=%d where sid = %lu",
                       lRnum,caRname,caUsername,caGroupname,caFrom,caTo,caMesg,lService,lFun,lTz,caTzdz,lFlags,lSid);
            if(iReturn == 0) {
                pasDbCommit(NULL);
            }
            else {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0797邮件分类"),ncLang("0305记录修改出错, %s"),pasDbErrorMsg(NULL));
                pasDbRollback(NULL);
            }
        }
    }
    else if(!utStrIsSpaces(caApp)) { /* 应用  */
        ncUtlSendProcessControl(psShmHead,"ASYNLOG",NC_RELOAD_NETWORK,1);
    }

    if(utStrIsSpaces(caTotRec)) {
        iReturn = pasDbOneRecord("select count(*) from nccontfilter",0,
                   UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0797邮件分类"),ncLang("0561数据统计出错"));
            return 0;
        }
        lTotRec = lCount;
    }

    lRowNum = 26;

    lStartRec = (lCurPg - 1) * lRowNum;   
    psCur = pasDbOpenSqlF("select sid,num,rname,username,groupname,mfrom,mto,mesg,service,funcode,tzfs,tzdz,flags,source from nccontfilter ");
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0797邮件分类"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();

    utPltPutVar(psDbHead,"sid",caSid);
    utPltPutVar(psDbHead,"rnum",caRnum);
    utPltPutVar(psDbHead,"rname",caRname);
    utPltPutVar(psDbHead,"username",caUsername);
	utPltPutVar(psDbHead,"groupname",caGroupname);
    utPltPutVar(psDbHead,"from",caFrom);
    utPltPutVar(psDbHead,"to",caTo);
    utPltPutVar(psDbHead,"mesg",caMesg);
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"fun",caFun);
    utPltPutVar(psDbHead,"tz",caTz);
    utPltPutVar(psDbHead,"tzdz",caTzdz);
    utPltPutVar(psDbHead,"flags",caFlags);

    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,  4,&lSid,
      	                           UT_TYPE_LONG,  4,&lRnum,
  	                               UT_TYPE_STRING,31,caRname,
  	                               UT_TYPE_STRING,63,caUsername,
  	                               UT_TYPE_STRING,63,caGroupname,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,63,caMesg,
  	                               UT_TYPE_LONG,  4, &lService,
  	                               UT_TYPE_LONG,  4, &lFun,
  	                               UT_TYPE_LONG,  4, &lTz,
  	                               UT_TYPE_STRING,63,caTzdz,
  	                               UT_TYPE_LONG,  4, &lFlags,
  	                               UT_TYPE_LONG,  4, &lSource);
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"lsid", iNum,"%ld",lSid);
        utPltPutLoopVarF(psDbHead,"num", iNum,"%ld",iNum);
		utPltPutLoopVarF(psDbHead,"lrnum",  iNum,"%ld",lRnum);
		utPltPutLoopVarF(psDbHead,"lrname",  iNum,"%s",caRname);
        utPltPutLoopVar(psDbHead, "lusername", iNum,caUsername);
		utPltPutLoopVar(psDbHead, "lgroupname", iNum,caGroupname);
        utPltPutLoopVarF(psDbHead,"lflags", iNum,"%d",lFlags);
		utPltPutLoopVar(psDbHead,"lfrom", iNum,caFrom);
        utPltPutLoopVarF(psDbHead,"lservice", iNum,"%d",lService);
        utPltPutLoopVarF(psDbHead,"lfun", iNum,"%d",lFun);
        utPltPutLoopVar(psDbHead,"lto",   iNum,caTo);
        utPltPutLoopVar(psDbHead,"lmesg",  iNum,caMesg);
        utPltPutLoopVarF(psDbHead,"ltz", iNum,"%d",lTz);
		utPltPutLoopVar(psDbHead,"ltzdz", iNum,caTzdz);
		if(lSource == 0) {
 			utPltPutLoopVar(psDbHead,"src", iNum,ncLang("0119本地"));
		}
		else {
			utPltPutLoopVar(psDbHead,"src", iNum,ncLang("0863中心"));
		}
		utPltPutLoopVarF(psDbHead,"srn", iNum,"%d",lSource);	
        strcpy(caRdesc,"\0");
		if(lService > 0) {
        	utStrAddF(caRdesc,ncLang("0254服务类别:%s;"),ncUtlImName(lService));
		}			
        if(!utStrIsSpaces(caFrom)) {
        	utStrAddF(caRdesc,ncLang("0228发送者:%s;"),caFrom);
        }
		if(lFun > 0) {
			utStrAddF(caRdesc,ncLang("0268功能:%s;"),ncUtlImFun(lFun));
		}
        if(!utStrIsSpaces(caTo)) {
        	utStrAddF(caRdesc,ncLang("0328接收者%s;"),caTo);
        }
        if(!utStrIsSpaces(caMesg)) {
        	utStrAddF(caRdesc,ncLang("0408内容:%s;"),caMesg);
        }
       	utStrAddF(caRdesc,ncLang("0575 通知:%s;"),ncUtlImTz(lTz));
		if(!utStrIsSpaces(caTzdz)) {
			utStrAddF(caRdesc," %s;",caTzdz);
		}
		if(lFlags == 9) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0331禁用"));
		}
        utPltPutLoopVar(psDbHead,"des", iNum,caRdesc);
        iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,  4,&lSid,
      	                           UT_TYPE_LONG,  4,&lRnum,
  	                               UT_TYPE_STRING,31,caRname,
  	                               UT_TYPE_STRING,63,caUsername,
  	                               UT_TYPE_STRING,63,caGroupname,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,63,caMesg,
  	                               UT_TYPE_LONG,  4, &lService,
  	                               UT_TYPE_LONG,  4, &lFun,
  	                               UT_TYPE_LONG,  4, &lTz,
  	                               UT_TYPE_STRING,63,caTzdz,
  	                               UT_TYPE_LONG,  4, &lFlags,
  	                               UT_TYPE_LONG,  4, &lSource);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lTotRec - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVarF(psDbHead,"rnum", "%lu", lCount+1);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_im_filterlist.htm");
    return 0;
}


/* 导出阻断记录  */
int ncWebExportError(utShmHead *psShmHead, pasDbCursor *psCur,FILE *fp)
{
    int iReturn;
    ncUserCont *psUser;
    char caTemp[32];

     char caUsername[32],caPlate[64],caMesg[256];
    unsigned long lSip,lDip,lPort,lCount1;
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0,lSid,lTime,lService,lMid;
    int iCount;
    iCount=0;
    ncUtlPrintCsv(fp,7,UT_TYPE_STRING,ncLang("0711序号"),
                       UT_TYPE_STRING,ncLang("0693姓  名"),
                       UT_TYPE_STRING,ncLang("0521时间"),
                       UT_TYPE_STRING,ncLang("0820源IP地址"),
                       UT_TYPE_STRING,ncLang("0397目标IP地址"),
                       UT_TYPE_STRING,ncLang("0180次数"),
                       UT_TYPE_STRING,ncLang("0407内容"));
                      
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_LONG,4,&lService,
  	                               UT_TYPE_LONG,4,&lMid,
  	                               UT_TYPE_STRING,127,caMesg,
  	                               UT_TYPE_ULONG,4,&lSip,
  	                               UT_TYPE_ULONG,4,&lDip,
  	                               UT_TYPE_ULONG,4,&lPort,
  	                               UT_TYPE_ULONG,5,&lCount1);

    while(iReturn == 0) {
        iCount++;
        ncUtlPrintCsv(fp,7,UT_TYPE_LONG,iCount,
                       UT_TYPE_STRING,ncUtlBufGetUsername(psShmHead,lUserid,"\0"),
                       UT_TYPE_STRING,utTimFormat("%Y-%m-%d %H:%M:%S",lTime),
                       UT_TYPE_STRING,utComHostIp(htonl(lSip)),
                       UT_TYPE_STRING,ncWebUtlDestIp(lDip,lPort),
                       UT_TYPE_LONG,lCount1,
                       UT_TYPE_STRING,ncUtlStrCvtCtl(caMesg));
                    
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_LONG,4,&lService,
  	                               UT_TYPE_LONG,4,&lMid,
  	                               UT_TYPE_STRING,127,caMesg,
  	                               UT_TYPE_ULONG,4,&lSip,
  	                               UT_TYPE_ULONG,4,&lDip,
  	                               UT_TYPE_ULONG,4,&lPort,
  	                               UT_TYPE_ULONG,5,&lCount1);

    }
    return 0;
}

/* 显示最近的错误告警信息

 */
int ncWebErrNewLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caWhere[1024],caCond[64],caCond0[64],caDel[16];
    char caTotPg[16],caCurPg[16],caUserid[16];
    char caUsername[32],caPlate[64],caMesg[256];
    char caTemp[256];
    char caCount[32];
	char caVar[32];
	char caExport[12];
	char *p;
    char caWhereStr[6]="where";
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0,lSid,lTime,lService,lMid;
    unsigned long lSip,lDip,lPort,lCount1;
    int i,iNum,iPos;
    utPltDbHead *psDbHead;
    ncUserCont *psUser;
   	int iReturn;
   	char caSearch[16];
   	char caGroupid[16];
   	long lGroupid0,lRowNum,lStartRec,lCount,lSumUser;
  	pasDbCursor *psCur;
    char caIp[20];
   	char ids[256];
   	int i2,lUserid2;
    char caTitlename[32];
    long group_flag;
    unsigned long lStime,lEtime;
    
    char caDispname[32],caMsg[64],caSdate[20],caEdate[20],caSdate1[8],caEdate1[8];
    char caIdfyno[20],caSip[24],caDip[24],caMac[20],caSdate_temp[24],caEdate_temp[24];
//utMsgPrintMsg(psMsgHead); 	
	lRowNum = 40;
	group_flag=0;
	iReturn = utMsgGetSomeNVar(psMsgHead,13,
		            "del",        UT_TYPE_STRING, 12, caDel,
	                "search",     UT_TYPE_STRING, 12, caSearch,
	                "userid",     UT_TYPE_STRING, 12, caUserid,
	                "groupid",    UT_TYPE_STRING, 12,caGroupid,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					         "CurPg",      UT_TYPE_STRING, 12, caCurPg,
   	                "cond",       UT_TYPE_STRING, 63, caCond,
   	                "where",      UT_TYPE_STRING, 1000,caWhere,
   	                "plate",      UT_TYPE_STRING, 63, caPlate,
   	                "lcount",     UT_TYPE_STRING, 12, caCount,
   	                "exp",        UT_TYPE_STRING, 10, caExport,
   	                "ip",         UT_TYPE_STRING,15,caIp,
   	                "cond",       UT_TYPE_STRING,30,caCond);
   	                
   	  	iReturn = utMsgGetSomeNVar(psMsgHead,10,
	                "username",   UT_TYPE_STRING, 31, caDispname,
	                "mesg",       UT_TYPE_STRING, 47, caMsg,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "sdate1",     UT_TYPE_STRING, 5,caSdate1,
	                "edate1",     UT_TYPE_STRING,5,caEdate1,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
	                "sip",        UT_TYPE_STRING, 20,caSip,
	                "dip",        UT_TYPE_STRING, 20,caDip,
	                "mac",        UT_TYPE_STRING, 18,caMac);
	                          
   	                
   	if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caDispname,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),"未知"));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	                                     
   	utStrDelSpaces(caCond);
   	utStrDelSpaces(caWhere);
    if(!utStrIsSpaces(caDel)) {  
		for(i=0;i<lRowNum;i++) {
        	sprintf(caVar,"chk%d",i+1);
        	p = utMsgGetVar_s(psMsgHead,caVar);
        	if(p) {
            	lSid = atol(p);
            	iReturn = pasDbExecSqlF("delete from ncerrlog where sid = %lu ",lSid);
        	}
    	}
    }
	
    sprintf(caCond0,"'%c%s%c'",'%',caCond,'%');

    if(!utStrIsSpaces(caExport)) {
        lStartRec = 0;
        lRowNum = 99999999L;
    }

    if(!utStrIsSpaces(caSearch)) {
        strcpy(caCount,"\0");
        strcpy(caWhere,"\0");
        lCurPg = 1;
    }
    else {
        lCurPg = atol(caCurPg);
        if(lCurPg <= 0) lCurPg = 1;
    }
    if(!utStrIsSpaces(caWhere)) {
    }
    else {
        lUserid0 = atol(caUserid);
        iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
        if(iReturn == 0 && lGroupid0 > 0) {
        	group_flag=1;
 //           sprintf(caWhere," and a.groupid = %lu ",lGroupid0);
        }
        if(group_flag==1||strlen(caGroupid)>0||strlen(caUserid)>0){
   		  if(strlen(caSdate)==0)
    		strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0))); 
    		if(strlen(caEdate)==0)  	
    		strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
    		if(strlen(caSdate1)==0)
    		strcpy(caSdate1,"00:00");
    		if(strlen(caEdate1)==0)
    		strcpy(caEdate1,"23:59");      		   		
   	} 
   	if(strlen(caSdate)>0&&(strlen(caSdate)<11)){
   	 	sprintf(caSdate_temp,"%s %s",caSdate,caSdate1);
   	 	lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_temp);
   	}
   	if(strlen(caSdate)>11){
   		lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate);
   	}
   	  
   	if(strlen(caEdate)>0&&(strlen(caEdate)<11)){
   	 	sprintf(caEdate_temp,"%s %s",caEdate,caEdate1);
   	 	lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_temp);
   	}
   	if(strlen(caEdate)>11){
   		lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate);
   	} 
   	
   	
      if(strlen(caSdate)>0){
      	 utStrAddF(caWhere," and b.stime >= '%lu'",lStime);      	
      }  
      if(strlen(caEdate)>0){
      	 utStrAddF(caWhere," and b.stime <= '%lu'",lEtime);      	
      }
      if(atol(caUserid)> 0) {
         utStrAddF(caWhere," and b.userid = %lu ",atol(caUserid));
      }
      if(group_flag==1){
      	utStrAddF(caWhere," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu))",lGroupid0,lGroupid0);
      }
        if(strlen(caGroupid)>0){
        	 if(strcmp(caGroupid,"0")==0)
        	 		utStrAddF(caWhere," and (a.groupid = 0 ) ");
       			else
        			sprintf(caWhere," and (a.groupid=%s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
        }
        

        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caWhere," and (b.mesg  like %s  or a.dispname like %s )",caCond0,caCond0);
        }
        utStrAddF(caWhere," order by b.stime desc");
    }
 // printf("caWhere=%s\n",caWhere);
    lStartRec = (lCurPg - 1) * lRowNum;  
    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }

    if(utStrIsSpaces(caCount)) {
        if( strstr(caWhere,"a.")) { /* 涉及用户信息  */
            sprintf(caTemp,"select count(*) from ncerrlog b left join ncuser a on b.userid=a.userid %s  %s",
                     caWhereStr,ncUtlTrimOrder(caWhere+iPos));
        }
        else {
            sprintf(caTemp,"select count(*) from  ncerrlog b %s %s",caWhereStr,ncUtlTrimOrder(caWhere+iPos));
        }
    #ifdef LDEBUG   
      printf("caTemp=%s\n",caTemp);  
    #endif
        iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0301记录查询出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
    }
    else {
        lCount = atol(caCount);
    }
 
    if( strstr(caWhere,"a.")) { /* 涉及用户信息  */
    	
    #ifdef LDEBUG   
      printf("select b.sid,b.userid,b.stime,b.service,b.mid,b.mesg,b.sip,b.dip,b.dport,b.lcount \
                from  ncerrlog b left join ncuser a on b.userid = a.userid where 1=1  %s limit %d,%d\n",caWhere,lStartRec,lRowNum);
    #endif
        psCur = pasDbOpenSqlF("select b.sid,b.userid,b.stime,b.service,b.mid,b.mesg,b.sip,b.dip,b.dport,b.lcount \
                from  ncerrlog b left join ncuser a on b.userid = a.userid where 1=1  %s limit %d,%d",caWhere,lStartRec,lRowNum);
    }
    else { 
    	
   #ifdef LDEBUG   
      printf("select b.sid,b.userid,b.stime,b.service,b.mid,b.mesg,b.sip,b.dip,b.dport,b.lcount \
                from  ncerrlog b %s %s limit %d,%d\n",caWhereStr,caWhere+iPos,lStartRec,lRowNum);
    #endif	       
        psCur = pasDbOpenSqlF("select b.sid,b.userid,b.stime,b.service,b.mid,b.mesg,b.sip,b.dip,b.dport,b.lcount \
                from  ncerrlog b %s %s limit %d,%d",caWhereStr,caWhere+iPos,lStartRec,lRowNum);
    } 
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }  
    
        if(!utStrIsSpaces(caExport)) {
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"ncwarn%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0891阻断查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,ncLang("0893阻断访问明细表\n"));
        ncWebExportError(psShmHead,psCur,fp);
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        char caUsername2[32];
			   memset(caUsername2,0,sizeof(caUsername2));
			   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
			   ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"导出阻断日志");
   
        
        return 0;
    }
    
         
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"dispname",caDispname);
    
  if(strlen(caSdate)==0){
    strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
  }
  if(strlen(caEdate)==0){
    strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
  }
   if(strlen(caSdate1)==0){
   	strcpy(caSdate1,"00:00");
  }
  if(strlen(caEdate1)==0){
  	strcpy(caEdate1,"23:59");
  }
    
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"sdate1",caSdate1);
    utPltPutVar(psDbHead,"edate1",caEdate1);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"dip",caDip);
    utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"mesg",caMsg);
    iReturn = 0;

    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_LONG,4,&lService,
  	                               UT_TYPE_LONG,4,&lMid,
  	                               UT_TYPE_STRING,127,caMesg,
  	                               UT_TYPE_ULONG,4,&lSip,
  	                               UT_TYPE_ULONG,4,&lDip,
  	                               UT_TYPE_ULONG,4,&lPort,
  	                               UT_TYPE_ULONG,5,&lCount1);
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
   	    i++;
        iNum++;
        utPltPutLoopVar(psDbHead,"service", iNum,ncUtlImName(lService));
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%lu",iNum);
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);

        psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
		     if(psUser){            
          utPltPutLoopVar(psDbHead,"position",iNum,psUser->position);
        }

        utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lTime));
        utPltPutLoopVar(psDbHead,"sip", iNum,utComHostIp(htonl(lSip)));
        utPltPutLoopVarF(psDbHead,"dip", iNum,"%s",ncWebUtlDestIp(lDip,lPort));
        utPltPutLoopVarF(psDbHead,"mid", iNum,"%lu",lMid);
        utPltPutLoopVarF(psDbHead,"count", iNum,"%lu",lCount1);
        utPltSetCvtHtml(1);
        utPltPutLoopVar(psDbHead,"mesg", iNum,ncUtlStrCvtCtl(caMesg));
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltSetCvtHtml(0);
        iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_LONG,4,&lService,
  	                               UT_TYPE_LONG,4,&lMid,
  	                               UT_TYPE_STRING,127,caMesg,
  	                               UT_TYPE_ULONG,4,&lSip,
  	                               UT_TYPE_ULONG,4,&lDip,
  	                               UT_TYPE_ULONG,4,&lPort,
  	                               UT_TYPE_ULONG,5,&lCount1);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"cond",caCond);
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVar(psDbHead,"plate",caPlate);
     utPltPutVar(psDbHead,"title_name",caTitlename);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}




/* 显示高级查询界面  */               
int ncWebErrDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    long iReturn,lGroupid,lGroupid0;
    char caSmt[256];
	  char caService[12];
	  char caSdate[24],caEdate[24],caSdate1[24],caEdate1[24];
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect;
    char caGroupid[32];
    unsigned long userid;
    char caDispname[64];
    pasDbCursor *psCur;
    ncUserContHead  *psHead;
    ncUserCont      *psUser,*psUser0;
    int i,iNum;
    unsigned long ltime;
    psDbHead = utPltInitDb();
    strcpy(caGroupid,"\0");
    iReturn = utMsgGetSomeNVar(psMsgHead,1,
	                "groupid",    UT_TYPE_STRING, 31, caGroupid); 
    ltime=time(0);
     
    ncUtlMonthDate(ltime,caSdate,caEdate);
    
    psHead = (ncUserContHead *)utShmArray(psShmHead,NC_LNK_ALLUSER);
    if(psHead == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("0403内存不正确"));
         return 0;
    }
    psUser0 = (ncUserCont *)((char *)psHead + psHead->lContPos); 
   
	  for(i=0,iNum=1;i<psHead->lSumUser ;i++) {
      psUser = &psUser0[i];
      if(strlen(caGroupid)>0){
      	if(psUser->groupid==atol(caGroupid)){
		      utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
		      utPltPutLoopVarF(psDbHead,"uid",iNum,"%lu",psUser->userid);
		      utPltPutLoopVarF(psDbHead,"uname",iNum,"%s",psUser->dispname);
		      iNum++;
	      	if(iNum>500) break;
	      }
	    }
	    else{
	    	    utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
      			utPltPutLoopVarF(psDbHead,"uid",iNum,"%lu",psUser->userid);
      			utPltPutLoopVarF(psDbHead,"uname",iNum,"%s",psUser->dispname);
	    	iNum++;
	    	if(iNum>500) break;
	    }
    
  }
  utPltPutVarF(psDbHead,"sum","%lu",iNum);
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
   psCur = pasDbOpenSql("select groupid,groupname from ncgroup   ",0);
    iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&userid,
    	                               UT_TYPE_STRING,63,caDispname);
    iNum = 0; 	                               
    while((iReturn==0)||(iReturn==1405))
    {
  	
        iNum++;       
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%lu",userid);
        utPltPutLoopVar(psDbHead,"dispname",iNum,caDispname);
    iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&userid,
      	                               UT_TYPE_STRING,63,caDispname);
    }
    pasDbCloseCursor(psCur); 
    
	strcpy(caService,"0");
    pSelect= pasUtlCvtInputSelect("service",caService,ncLang("0452全部,0;QQ,6;MSN,10;ICQ,11;YHMSG,7;"));
    utPltPutVar(psDbHead,"sserv", pSelect);
    free(pSelect);
    
    sprintf(caSdate1,"%s 00:00",caSdate);
    sprintf(caEdate1,"%s 23:59",caEdate);
    utPltPutVar(psDbHead,"sdate",caSdate1);
    utPltPutVar(psDbHead,"edate",caEdate1);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_err_hsear.htm");
}

/* 高级查询  */
int ncWebErrHsearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caWhere[1012];
    char caUserid[16];
    char caDispname[32],caSdate[32];
    char caGroupid[12];
	char caMsg[128];
	char caSip[21];
	char caDip[21];
	char caDport[7];
	char caMac[20];
	char caIdfyno[20];
    char caEdate[20],caOrder[12],caOrderby[12];
    unsigned long lUserid,lGroupid,lStime,lEtime;
   	int  iReturn;
   	char caService[12];
   	long lGroupid0,lService;
   	char caIp[20];
   	char ids[256];
   	int i2,lUserid2;
   	char caSdate1[8],caEdate1[8];
   	long group_flag;
   	char caSdate_temp[24],caEdate_temp[24];
   	pasDbCursor *psCur;
   	char caUsername_match[8],caIdfyno_match[8],caMac_match[8],caIp_match[8],caDip_match[8];
	strcpy(caWhere,"\0");
	utMsgPrintMsg(psMsgHead);
	group_flag=0;
	iReturn = utMsgGetSomeNVar(psMsgHead,22,
	                "username",   UT_TYPE_STRING, 31, caDispname,
	                "userid",     UT_TYPE_STRING,11,caUserid,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "mesg",       UT_TYPE_STRING, 47, caMsg,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "sdate1",     UT_TYPE_STRING,5,caSdate1,
	                "edate1",     UT_TYPE_STRING,5,caEdate1,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "orderby",    UT_TYPE_STRING, 2, caOrderby,
	                "service",    UT_TYPE_STRING, 11,caService,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
	                "sip",        UT_TYPE_STRING, 20,caSip,
	                "dip",        UT_TYPE_STRING, 20,caDip,
	                "dport",      UT_TYPE_STRING, 6,caDport,
	                "mac",        UT_TYPE_STRING, 18,caMac,
	                "ip",         UT_TYPE_STRING,15,caIp,
	                "username_match",UT_TYPE_STRING,2,caUsername_match,
	                "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
	                "mac_match",UT_TYPE_STRING,2,caMac_match,
	                "ip_match",UT_TYPE_STRING,2,caIp_match,
	                "dip_match",UT_TYPE_STRING,2,caDip_match);
	utStrDelSpaces(caDispname);
	utStrDelSpaces(caGroupid);
	utStrDelSpaces(caMsg);
	utStrDelSpaces(caSdate);
	utStrDelSpaces(caEdate);
	utStrDelSpaces(caSip);
	utStrDelSpaces(caDip);
	utStrDelSpaces(caDport);
	utStrDelSpaces(caMac);
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {  /* 部门权利  */
    	group_flag=1;
//        sprintf(caWhere," and a.groupid = %lu ",lGroupid0);
    }
    
    if(!utStrIsSpaces(caSdate)) {
    	if(strlen(caSdate)>11)
		     lStime = utTimStrToLong("%Y-%m-%d %H:%M:%S",caSdate);
		   else{
		   	sprintf(caSdate_temp,"%s %s",caSdate,caSdate1);
		   	lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_temp);
		  }
        utStrAddF(caWhere," and b.stime >= '%lu'",lStime);
    }
    if(!utStrIsSpaces(caEdate)) {
    	if(strlen(caEdate)>11)
		     lEtime = utTimStrToLong("%Y-%m-%d %H:%M:%S",caEdate);
		   else{
		   	sprintf(caEdate_temp,"%s %s",caEdate,caEdate1);
		   	lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_temp);
		  }
        utStrAddF(caWhere," and b.stime  <= '%lu'",lEtime);
    }

    if(strlen(caUserid)>0){
    	utStrAddF(caWhere," and a.userid=%s ",caUserid);
    }
        if(group_flag==1){
    	utStrAddF(caWhere," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu))",lGroupid0,lGroupid0);
    }
    if(!utStrIsSpaces(caGroupid)) {
        utStrAddF(caWhere," and (a.groupid = %s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
    }
    if(!utStrIsSpaces(caMac)) {
    	 if(atol(caMac_match)==0)
        utStrAddF(caWhere," and a.mac = '%s' ",caMac);
       else
        sprintf(caWhere+strlen(caWhere)," and a.mac like '%c%s%c' ",'%',caMac,'%');
    }
    if(strlen(caIdfyno)!=0){
      if(atol(caIdfyno_match)==0)
	  		utStrAddF(caWhere," and a.idfyno = '%s' ",caIdfyno);
	  	else
	  	  sprintf(caWhere+strlen(caWhere)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
	  	}


    if(!utStrIsSpaces(caDispname)&&strlen(caUserid)==0) {
        utStrAddF(caWhere," and a.dispname like '%c%s%c'",'%',caDispname,'%');
    }
    if(!utStrIsSpaces(caMsg)) {
        utStrAddF(caWhere," and b.mesg like '%c%s%c'",'%',caMsg,'%');
    }
	if(!utStrIsSpaces(caDport)) {
		utStrAddF(caWhere," and b.dport=%s ",caDport);
	}
	if(!utStrIsSpaces(caSip)) {
		  if(atol(caIp_match)==0)
       	utStrAddF(caWhere," and b.sip  =  %lu", ntohl(utComHostAddress(caSip)));	
       else
        sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caSip,'%');
    }
	if(!utStrIsSpaces(caDip)) {
		   if(atol(caDip_match)==0)
      	 utStrAddF(caWhere," and b.dip  =  %lu", ntohl(utComHostAddress(caDip)));	
       else
         sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.dip) like '%c%s%c' ",'%',caDip,'%');
    }	
		 if(!utStrIsSpaces(caIp))
    	{
    		memset(ids,0,sizeof(ids));
    		i2=0;

    		  psCur = pasDbOpenSqlF("select userid from ncuser where ip=%lu",ntohl(utComHostAddress(caIp)));
          if(psCur!=NULL){
          	   iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,sizeof(long),&lUserid2);
          	   while(iReturn==0){
          	   	if(i2>10) break;
          	   	i2++;
          	   	if(i2==1)
          	   	 sprintf(ids,"%lu",lUserid2);
          	   	else
          	   	 sprintf(ids+strlen(ids),",%lu",lUserid2);
          	   	 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,sizeof(long),&lUserid2);
          	  }              
    	    }
    	     pasDbCloseCursor(psCur);
    	     
    	     if(strlen(ids)>0)
    	        sprintf(caWhere+strlen(caWhere)," and b.userid in (%s) ",ids);
    	     else
    	       sprintf(caWhere+strlen(caWhere)," and b.userid=-1 ");
    }
	if(!utStrIsSpaces(caService)) {
		lService = atol(caService);
		if(lService > 0) {
            utStrAddF(caWhere," and b.service = %d ",lService);
		}
    }
    if(!utStrIsSpaces(caOrder)) {
        if(strcmp(caOrder,"date")==0) {
            utStrAddF(caWhere," order by b.stime ");
        }
        else if(strcmp(caOrder,"service") == 0) {
            utStrAddF(caWhere," order by b.service ");
        }
        if(!utStrIsSpaces(caOrderby)) {
            if(caOrderby[0] == '1') {     
                utStrAddF(caWhere," desc ");
            }
        }
    }
	utMsgAdd_s(&psMsgHead,"where",caWhere);
	ncWebErrNewLog(psShmHead,iFd,psMsgHead);
    return 0;
}



/*  显示的的及时告警日志

 */
int ncWebWarnNewLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn;
    int iSum;
    int  iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIpExcept **psIp;
    char caName[32];
    char caUsername[32];
    char caMesg[32];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;
    ncUserCont *psUser;
    ncWarnSystem **pWarnSystem;
    int lSum2;
    char caTitlename[32];
    iUserFlags = 0;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

	lGroupid1 = (-1);
		utMsgDelVar(psMsgHead,"action");
	  utMsgAdd_s(&psMsgHead,"action","06");
		ncOperWriteLog(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead,4,
		"groupid",   UT_TYPE_STRING, 11, caGroupid,
		"userid",    UT_TYPE_STRING, 31, caUserid,
		"username",  UT_TYPE_STRING,31,caUsername,
		"mesg",      UT_TYPE_STRING,31,caMesg);
		
	 if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),"未知"));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	        
	if(!utStrIsSpaces(caGroupid)) {
		lGroupid1 = atol(caGroupid);
	}
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
    psDbHead = utPltInitDb();
    if(!utStrIsSpaces(caUserid)) {
    	iUserFlags = 1;
    	lUserid = atol(caUserid);
    }
    iSum = 0;
    
    pWarnSystem=ncWarnGetWarnInfo(psShmHead, 0,0,&lSum2);
    if(pWarnSystem==NULL)
    {
        lSum2 = 0;
    }
    
    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPEXCEPT);
    if(lMaxRec+lSum2 <= 0) {
    	i=0;
    while(i < 30) {
       i++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",i);
    }
    utPltPutVar(psDbHead,"title_name",caTitlename);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"/nc/nc_list_msgwarn.htm");        
        return 0;
    }
    psIp = (ncIpExcept **) malloc((lMaxRec+1) * sizeof(ncIpExcept *));
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }        
    lSumRec = ncWebGetCurIpExcept(psShmHead,psIp,lMaxRec);  /* 从内存中取当前IP地址  */
   
    iMaxRecord = utComGetVar_ld(psShmHead,"MaxNewIpRec",50);
    utPltSetCvtHtml(1);
    
    for(i=0;i<lSumRec && iSum < iMaxRecord;i++) {
        if((psIp[i]->lSip > 0)&&(psIp[i]->lwarnid>0)&&((strlen(caMesg)==0)||strstr(ncUtlCvtZdyy(psShmHead,psIp[i]),caMesg))) {
            psUserInfo = ncGetUserInfoById(psShmHead,psIp[i]->lUserid);
            if(psUserInfo) {
         	     if( ((iUserFlags == 0 && (lGroupid0 == 0 || psUserInfo->lGid == lGroupid0) ) || (iUserFlags && psUserInfo->lId == lUserid))
         	     && ((strlen(caUsername)==0) || strstr(psUserInfo->psUser->dispname,caUsername)||strstr(psUserInfo->psUser->username,caUsername))
				 	    && (lGroupid1 == (-1) || psUserInfo->lGid == lGroupid1)) {
											utPltPutLoopVarF(psDbHead,"userid",iSum+1,"%lu",psIp[i]->lUserid);
											utPltPutLoopVar(psDbHead,"sip",  iSum+1,utComHostIp(htonl(psIp[i]->lSip)));
            		      utPltPutLoopVar(psDbHead,"username", iSum+1,psUserInfo->psUser->dispname);
            		      utPltPutLoopVar(psDbHead,"position", iSum+1,psUserInfo->psUser->position);
            		      utPltPutLoopVar(psDbHead,"groupname", iSum+1,psUserInfo->psUser->psGroup->groupname);
        	            utPltPutLoopVar(psDbHead,"mesg", iSum+1,ncUtlCvtZdyy(psShmHead,psIp[i]));
                      utPltPutLoopVar(psDbHead, "sdate",iSum+1,
                                    utTimFormat("%m-%d %H:%M:%S",psIp[i]->lLastTime));
        		          iSum++;
                }
            }
            else { /* 无用户信息  */
                 if(utStrIsSpaces(caGroupid) && utStrIsSpaces(caUserid) && lGroupid0 == 0) {

       	              utPltPutLoopVar(psDbHead,"mesg", iSum+1,ncUtlCvtZdyy(psShmHead,psIp[i]));
                      utPltPutLoopVar(psDbHead, "sdate",iSum+1,
                                utTimFormat("%m-%d %H:%M:%S",psIp[i]->lLastTime));
      		          iSum++;
                }
            
            }
        }
    }
    

    for(i=0;i<lSum2&&iSum < iMaxRecord;i++)
    {
    	
    	psUserInfo = ncGetUserInfoById(psShmHead,pWarnSystem[i]->lUserid);
    	psUser = ncUtlGetUserContById(psShmHead,pWarnSystem[i]->lUserid);
    	if((strlen(caMesg)==0)||strstr(pWarnSystem[i]->caMesg,caMesg)){
    	if(psUserInfo){
    		
    		      if( ((iUserFlags == 0 && (lGroupid0 == 0 || psUserInfo->lGid == lGroupid0) ) || (iUserFlags && psUserInfo->lId == lUserid))
         	     && ((strlen(caUsername)==0) || strstr(psUserInfo->psUser->dispname,caUsername)||strstr(psUserInfo->psUser->username,caUsername))
				 	    && (lGroupid1 == (-1) || psUserInfo->lGid == lGroupid1)){
				 	    	
				 	    	
				 	    				utPltPutLoopVarF(psDbHead,"userid",iSum+1,"%lu",pWarnSystem[i]->lUserid);
											utPltPutLoopVar(psDbHead,"sip",  iSum+1,utComHostIp(htonl(psUser->ip)));
            		      utPltPutLoopVar(psDbHead,"username", iSum+1,psUserInfo->psUser->dispname);
            		      utPltPutLoopVar(psDbHead,"position", iSum+1,psUserInfo->psUser->position);
            		      utPltPutLoopVar(psDbHead,"groupname", iSum+1,psUserInfo->psUser->psGroup->groupname);
        	            utPltPutLoopVar(psDbHead,"mesg", iSum+1,pWarnSystem[i]->caMesg);
        	            
        	            
        	            utPltPutLoopVar(psDbHead,"service",iSum+1,ncCvtWarnlevel(pWarnSystem[i]->cType));
        
        	            //printf("warntype=%d\n",pWarnSystem[i]->cType);
        	            
                      utPltPutLoopVar(psDbHead, "sdate",iSum+1,
                                    utTimFormat("%m-%d %H:%M:%S",pWarnSystem[i]->lLastTime));
        		          iSum++;
				 	    	
				 	    }
    		
    		
    	}
    }
  }
    
  utPltSetCvtHtml(0);  
     i=0;
     utPltPutVarF(psDbHead,"CurPg","%d",1);
     utPltPutVarF(psDbHead,"TotPg","%d",1);
     utPltPutVarF(psDbHead,"TotRec","%d",iSum);

    while(iSum < iMaxRecord) {
        i++;
        iSum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iSum);
    }
    
    utPltPutVar(psDbHead, "username",caName);
    free(psIp);
    
    utPltPutVar(psDbHead,"title_name",caTitlename);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_list_msgwarn.htm");
    return 0;
}

/* 显示高级查询界面  */               
int ncWebWarnDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    long iReturn,lGroupid,lGroupid0;
    char caSmt[256];
	char caService[12];
	char caFlag[8];
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect;
    char caGroupid[32];
    char groupname[65]="";
    long groupid=0;
    long m=0;
    ncUserContHead  *psHead;
    char caSdate[24],caEdate[24],caSdate1[24],caEdate1[24];
    ncUserCont      *psUser,*psUser0;
    int i,iNum;
    unsigned long ltime;
    pasDbCursor *psCur;
    psDbHead = utPltInitDb();
    strcpy(caGroupid,"\0");
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		            "flag",        UT_TYPE_STRING, 4, caFlag,
		            "groupid",     UT_TYPE_STRING, 12,caGroupid);
		            
		     ltime=time(0);
     
    ncUtlMonthDate(ltime,caSdate,caEdate);
    
    psHead = (ncUserContHead *)utShmArray(psShmHead,NC_LNK_ALLUSER);
    if(psHead == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("0403内存不正确"));
         return 0;
    }
    psUser0 = (ncUserCont *)((char *)psHead + psHead->lContPos); 
   
	    for(i=0,iNum=1;i<psHead->lSumUser ;i++) {
      psUser = &psUser0[i];
      if(strlen(caGroupid)>0){
      	if(psUser->groupid==atol(caGroupid)){
		      utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
		      utPltPutLoopVarF(psDbHead,"uid",iNum,"%lu",psUser->userid);
		      utPltPutLoopVarF(psDbHead,"uname",iNum,"%s",psUser->dispname);
		      iNum++;
	      	if(iNum>500) break;
	      }
	    }
	    else{
	    	    utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
      			utPltPutLoopVarF(psDbHead,"uid",iNum,"%lu",psUser->userid);
      			utPltPutLoopVarF(psDbHead,"uname",iNum,"%s",psUser->dispname);
	    	iNum++;
	    	if(iNum>500) break;
	    }
    
  }
	utPltPutVarF(psDbHead,"sum","%lu",iNum);	            
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {  /* 部门权利  */
        sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu order by groupname",
                  lGroupid0);
       
    }
    else {
    	sprintf(caSmt,"select groupname,groupid from ncgroup where groupid>0 order by groupname");
        
    }
    
    psCur = pasDbOpenSql(caSmt,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0152部门查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
   
    iReturn = 0;
    m=0;

    while(iReturn==0)
    { 	
    	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,64,groupname,
    							   UT_TYPE_LONG,4,&groupid);                         
	    if(iReturn==0)
	    {
	    	m++;
	    	utPltPutLoopVar(psDbHead,"groupname",m,groupname);
	    	utPltPutLoopVarF(psDbHead,"groupid",m,"%lu",groupid);
	    }
	}
	pasDbCloseCursor(psCur);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);

	if(atol(caFlag)==1)
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_warn_hsear_h.htm");
	else
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_warn_hsear.htm");
}

int ncWebWarnLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caWhere[1024],caDel[16];
    char caTotPg[16],caCurPg[16],caUserid[16],caGroupid[16];
    char caMesg[256];
    char caTemp[256];
    char caService[13];
    char caSdate[20];
    char caEdate[20];
    char caVar[32];
    char *p;
    char caWhereStr[6]="where";
    char caUsername[65]="";
    char username[65]="";
    char caWarnid[16],caMsg[64];
    unsigned long lstime,letime,lTime1,l;
    char caDdd[12],caUnit[12],caDtype[12],caBef[12];  	
   char caCount[17];
   char groupname[65];
   char caName[32];
	char caExport[12],caCond[64];
   char caCond0[128];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0,lSid,lTime,lService,lMid;
    unsigned long lSip,lDip,lPort,lCount1;
    int i,iNum,iPos;
    utPltDbHead *psDbHead;
   	int iReturn;
   	char caSearch[16];
   	long lGroupid0,lRowNum,lStartRec,lCount,lSumUser;
 	 pasDbCursor *psCur;
 	 ncUserCont *psUser;
 	 char caTitlename[32];
	 char caSorta[10],caOrdera[10];
	 char caIdfyno[20];
	 char caIdfyno_match[8];
   char caIp[20];
   char ids[256];
   int i2,lUserid2;
   long lType;
	lRowNum = 40;
	iReturn = utMsgGetSomeNVar(psMsgHead,25,
		        "del",        UT_TYPE_STRING, 12, caDel,
	                "groupid",     UT_TYPE_STRING, 12, caGroupid,
	                "username",   UT_TYPE_STRING, 31, caUsername,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			             "CurPg",      UT_TYPE_STRING, 12, caCurPg,
   	                "where",      UT_TYPE_STRING, 255,caWhere,
   	                "service",    UT_TYPE_STRING, 12, caService,
   	                "ddd",        UT_TYPE_STRING, 11,caDdd,
	                "unit",       UT_TYPE_STRING, 11,caUnit,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
   	                "dtype",      UT_TYPE_STRING, 11,caDtype,
   	                "ok",         UT_TYPE_STRING,15,caSearch,
   	                "bef",        UT_TYPE_STRING, 2,caBef,
   	                "lcount",     UT_TYPE_STRING, 12,caCount,
   	                "exp",        UT_TYPE_STRING,10,caExport,
   	                "cond",       UT_TYPE_STRING, 63, caCond,
   	                "warnid",     UT_TYPE_STRING,12,caWarnid,
   	                "mesg",       UT_TYPE_STRING,50,caMsg,
   	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
   	                "sorta",      UT_TYPE_STRING,8,caSorta,
   	                "ordera",     UT_TYPE_STRING,8,caOrdera,
   	                "userid",     UT_TYPE_STRING,12,caUserid,
   	                "ip",         UT_TYPE_STRING,15,caIp,
   	                "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match);

   	if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),"未知"));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	

  
  
    if(!utStrIsSpaces(caExport)) {
        lStartRec = 0;
        lRowNum = 99999999L;
    }		
   
    if(!utStrIsSpaces(caSearch)) {
        strcpy(caWhere,"\0");
        lCurPg = 1;
    }
    else {
        lCurPg = atol(caCurPg);
        if(lCurPg <= 0) lCurPg = 1;
    }
    if(!utStrIsSpaces(caWhere)) {
    }
    else {
        iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
        if(iReturn == 0 && lGroupid0 > 0) {
            sprintf(caWhere," and (a.groupid = %lu or a.groupid in (select groupid from ncgroupid where pid=%lu))",lGroupid0,lGroupid0);
        }
       
        if((!utStrIsSpaces(caUsername))&&(strlen(caUserid)==0)) {
            utStrAddF(caWhere," and (a.username  like '%c%s%c' or a.dispname like '%c%s%c') ",'%',caUsername,'%','%',caUsername,'%');
        }
        if(strlen(caUserid)!=0){
        	utStrAddF(caWhere," and b.userid=%s ",caUserid);
        }
        if(strlen(caIdfyno)!=0){
        	if(atol(caIdfyno_match)==0)
	  				utStrAddF(caWhere," and a.idfyno = '%s' ",caIdfyno);
	  			else
	  			  sprintf(caWhere+strlen(caWhere)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
	  			}

        if(strlen(caWarnid)!=0)
        utStrAddF(caWhere," and b.level=%s ",caWarnid);
        if(strlen(caMsg)!=0)
        utStrAddF(caWhere," and b.mesg like '%c%s%c' ",'%',caMsg,'%');
        if(!utStrIsSpaces(caGroupid)) {
        	utStrAddF(caWhere," and a.groupid=%s ",caGroupid);
        }
        if(!utStrIsSpaces(caService)) {
        	utStrAddF(caWhere," and b.service=%s ",caService);
        }
        
      if(!utStrIsSpaces(caIp))
    	{
    		memset(ids,0,sizeof(ids));
    		i2=0;

    		  psCur = pasDbOpenSqlF("select userid from ncuser where ip=%lu",ntohl(utComHostAddress(caIp)));
          if(psCur!=NULL){
          	   iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,sizeof(long),&lUserid2);
          	   while(iReturn==0){
          	   	if(i2>10) break;
          	   	i2++;
          	   	if(i2==1)
          	   	 sprintf(ids,"%lu",lUserid2);
          	   	else
          	   	 sprintf(ids+strlen(ids),",%lu",lUserid2);
          	   	 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,sizeof(long),&lUserid2);
          	  }              
    	    }
    	     pasDbCloseCursor(psCur);
    	     
    	     if(strlen(ids)>0)
    	        sprintf(caWhere+strlen(caWhere)," and b.userid in (%s) ",ids);
    	     else
    	       sprintf(caWhere+strlen(caWhere)," and b.userid=-1 ");
    }
        if(strcmp(caDtype,"unit") == 0) { /* 按天数计  */
        lTime = time(0);
        if(!utStrIsSpaces(caDdd)) { /* 计算几天以内  */
            l = atol(caDdd);
            if(caUnit[0] == '1') { /* 天  */
                lTime1 = lTime - l* 86400L;
            }
            else if(caUnit[0] == '2') { /* 周  */
                lTime1 = lTime - l* 7 * 86400L;
            }
            else {
                lTime1 = lTime - l* 30 * 86400L;
            }
            
            if(caBef[0] == '1') { /* 以内  */
                utStrAddF(caWhere," and b.stime >= %lu",lTime1);
            }
            else { /* 以前  */
                utStrAddF(caWhere," and b.stime <= %lu",lTime1);
            }
        }            
    }
	    else { /* 时间范围  */
	        if(!utStrIsSpaces(caSdate)) {
	            lstime=utTimStrToLong("%Y/%m/%d",caSdate);
	            utStrAddF(caWhere," and b.stime >= %lu",lstime);
	        }
	        if(!utStrIsSpaces(caEdate)) {
	            letime=utTimStrToLong("%Y/%m/%d",caEdate);
	            utStrAddF(caWhere," and b.stime <= %lu",letime);
	        }
	    }
    }
/*    
    if(atol(caSorta)==1)
    utStrAddF(caWhere," order by b.userid ");
    else if(atol(caSorta)==2)
    utStrAddF(caWhere," order by b.service ");
    else if(atol(caSorta)==3)
    utStrAddF(caWhere," order by b.sid ");
    
    if(atol(caOrdera)==2)
    utStrAddF(caWhere," desc ");
    
    if(strstr(caWhere,"order by")==NULL)
    utStrAddF(caWhere," order by b.sid desc ");
 */ 
    lStartRec = (lCurPg - 1) * lRowNum;  
   

    if(utStrIsSpaces(caCount)) {
       
            sprintf(caTemp,"select count(*) from  ncwarnlog b left join ncuser a on ( a.userid=b.userid) where 1=1 %s ",caWhere);
            
//            printf("caTemp=%s\n",caTemp); 
            
    #ifdef LDEBUG   
      printf("caTemp=%s\n",caTemp);  
    #endif     
        iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
       
    }
    else {
        lCount = atol(caCount);
    }
    #ifdef LDEBUG   
      printf("select b.sid,b.userid,a.dispname,b.stime,b.service,b.mid,b.mesg,c.groupname,d.name \
                from  ncwarnlog b,ncuser a,ncgroup c left join nclimwarntype d on b.level=d.id where a.userid=b.userid and a.groupid=c.groupid %s  limit %d,%d\n",caWhere,lStartRec,lRowNum);  
    #endif 
        psCur = pasDbOpenSqlF("select b.sid,b.userid,a.dispname,b.stime,b.service,b.mid,b.mesg,c.groupname,b.level \
                from  (ncwarnlog b) left join ncuser a on (a.userid=b.userid ) left join ncgroup c on (a.groupid=c.groupid) where 1=1  %s  order by b.sid desc limit %d,%d",caWhere,lStartRec,lRowNum);
                
//        psCur = pasDbOpenSqlF("select b.sid,b.userid,a.dispname,b.stime,b.service,b.mid,b.mesg,c.groupname,'' \
//                from  ncwarnlog b,ncuser a,ncgroup c where a.userid=b.userid and a.groupid=c.groupid  %s  limit %d,%d",caWhere,lStartRec,lRowNum);     
         if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }  
     
        if(!utStrIsSpaces(caExport)) {
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"ncwarn%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0891阻断查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,ncLang("0893告警信息明细表\n"));
        
        ncUtlPrintCsv(fp,6,UT_TYPE_STRING,ncLang("0692姓名"),
                       UT_TYPE_STRING,ncLang("0147部门"),
                       UT_TYPE_STRING,ncLang("0521时间"),
                       UT_TYPE_STRING,ncLang("0931告警级别"),
                       UT_TYPE_STRING,ncLang("0932信息类别"),
                       UT_TYPE_STRING,ncLang("0933告警内容"));
    iReturn = 0;
    memset(caName,0,sizeof(caName));
    memset(caMesg,0,sizeof(caMesg));
    memset(username,0,sizeof(username));
    memset(groupname,0,sizeof(groupname));
    lType=0;
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_STRING,64,username,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                           UT_TYPE_LONG,4,&lService,
  	                           UT_TYPE_LONG,4,&lMid,
  	                           UT_TYPE_STRING,127,caMesg,
  	                           UT_TYPE_STRING,64,groupname,
  	                           UT_TYPE_LONG,4,&lType); 
  while((iReturn == 0)||(iReturn==1405)) { 	                           
   ncUtlPrintCsv(fp,6,UT_TYPE_STRING,username,
                       UT_TYPE_STRING,groupname,
                       UT_TYPE_STRING,utTimFormat("%Y/%m/%d %H:%M:%S",lTime),
                       UT_TYPE_STRING,ncCvtWarnlevel(lType),
                       UT_TYPE_STRING,ncUtlImName(lService),
                       UT_TYPE_STRING,caMesg);
                       
    memset(caName,0,sizeof(caName));
    memset(caMesg,0,sizeof(caMesg));
    memset(username,0,sizeof(username));
        memset(groupname,0,sizeof(groupname));
    lType=0;
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_STRING,64,username,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                           UT_TYPE_LONG,4,&lService,
  	                           UT_TYPE_LONG,4,&lMid,
  	                           UT_TYPE_STRING,127,caMesg,
  	                           UT_TYPE_STRING,64,groupname,
  	                           UT_TYPE_LONG,4,&lType);                   
                                           
  }	                                        
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
                 char caUsername2[32];
			   memset(caUsername2,0,sizeof(caUsername2));
			   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
			   ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"导出告警日志");
   
        
        return 0;
    }
     
    psDbHead = utPltInitDb();
    iReturn = 0;
    memset(caName,0,sizeof(caName));
        memset(groupname,0,sizeof(groupname));
        memset(caMesg,0,sizeof(caMesg));
    lType=0;
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_STRING,64,username,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                           UT_TYPE_LONG,4,&lService,
  	                           UT_TYPE_LONG,4,&lMid,
  	                           UT_TYPE_STRING,127,caMesg,
  	                           UT_TYPE_STRING,64,groupname,
  	                           UT_TYPE_LONG,4,&lType);
  	                              
   	iNum = 0;
   	while(((iReturn == 0)||(iReturn==1405)) && iNum < lRowNum) {
   	    i++;
        iNum++;
        utPltPutLoopVar(psDbHead,"service", iNum,ncUtlImName(lService));
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%lu",iNum);
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
        if(psUser){
        	utPltPutLoopVar(psDbHead,"position",iNum,psUser->position);
        }
        utPltPutLoopVar(psDbHead,"username", iNum,username);
        utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
        utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%Y/%m/%d %H:%M:%S",lTime));
        utPltPutLoopVarF(psDbHead,"mid", iNum,"%lu",lMid);
        utPltPutLoopVar(psDbHead,"warnname",iNum,ncCvtWarnlevel(lType));
        utPltPutLoopVar(psDbHead,"mesg", iNum,ncUtlStrCvtCtl(caMesg));
        memset(caName,0,sizeof(caName));
        memset(groupname,0,sizeof(groupname));
        memset(caMesg,0,sizeof(caMesg));
    lType=0;
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_STRING,64,username,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                           UT_TYPE_LONG,4,&lService,
  	                           UT_TYPE_LONG,4,&lMid,
  	                           UT_TYPE_STRING,127,caMesg,
  	                           UT_TYPE_STRING,64,groupname,
  	                           UT_TYPE_LONG,4,&lType);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"warnid",caWarnid);
    utPltPutVar(psDbHead,"mesg",caMsg);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVar(psDbHead,"title_name",caTitlename);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_warnlog_list.htm");
    return 0;
}




/* 即时通信收发人聊天会话  */
int ncImdetcontlist(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
   
    char caTotPg[16],caCurPg[16],caUserid[16]="";
    char caUsername[32],caTo[64],caFrom[64],caMesg[256]="";
    char caTemp[256];
    char caCount[32];
    char caExport[16];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0,lSid,lTime,lService;
    int i,iNum,iPos;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lRowNum,lStartRec,lCount;
   	char caMsg[300];
   	char caDir[128],caFile[128];
   	char caCont[256];
   	char caPathfile[256];
   	char caFrom2[48],caTo2[48];
    char caSdate[24],caEdate[24];
    char caSdate1[10],caEdate1[10];
   	unsigned long lpsCount,lptCount,ltCount;
   	char caSdate_all[32],caEdate_all[32];
   	long lStime,lEtime;
 	 pasDbCursor *psCur;
	ncUserCont   *psUser;
	char caDispname[36];
    long lId;
 	
	iReturn = utMsgGetSomeNVar(psMsgHead,10,
		            "from",        UT_TYPE_STRING, 30, caFrom,
	               "to",     UT_TYPE_STRING, 30, caTo,
	               "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					       "CurPg",      UT_TYPE_STRING, 12, caCurPg,
   	             "exp",        UT_TYPE_STRING, 10, caExport,
   	             "sdate",      UT_TYPE_STRING, 10,caSdate,
   	             "edate",      UT_TYPE_STRING, 10,caEdate,
   	             "sdate1",     UT_TYPE_STRING,8,caSdate1,
   	             "edate1",     UT_TYPE_STRING,8,caEdate1,
   	             "userid",     UT_TYPE_STRING,10,caUserid);
           
   psUser = ncUtlGetUserContById(psShmHead,atol(caUserid));    
    if(psUser) {
    	strcpy(caDispname,psUser->dispname);
    }
    else{
    	strcpy(caDispname,"");
    }
            
   	             
   if(strlen(caSdate1)==0){
   	strcpy(caSdate1,"00:00");
  }
  if(strlen(caEdate1)==0){
  	strcpy(caEdate1,"23:59");
  }
   	
   if(strlen(caSdate)==0){
     	strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
     	
   }
   sprintf(caSdate_all,"%s %s",caSdate,caSdate1);
   lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
   if(strlen(caEdate)>0){
   	sprintf(caEdate_all,"%s %s",caEdate,caEdate1);
   	lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
   }
   	strcpy(caFrom2,caFrom);
   	strcpy(caTo2,caTo);               
  
   lCurPg = atol(caCurPg);
   
   
    
      
    
    if(!utStrIsSpaces(caExport)) {
        lStartRec = 0;
        lRowNum = 49999L;
    }
    else{
    		lRowNum = 50;
    }
    
  
   utStrReplaceWith(caFrom,"^^","#");
   utStrReplaceWith(caTo,"^^","#");
    sprintf(caTemp,"select count(*) from nccontlog where ((mfrom='%s' and mto='%s') or (mfrom='%s' and mto='%s')) ",caFrom,caTo,caTo,caFrom);
    if(strlen(caSdate)>0){
    	sprintf(caTemp+strlen(caTemp)," and stime>=%d ",lStime);
    }
    if(strlen(caEdate)>0){
//    	sprintf(caTemp+strlen(caTemp)," and stime<=%d ",lEtime);
    }
      
    printf("caTemp=%s\n",caTemp);
        iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0301记录查询出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
   
   
   lTotPg = (lCount - 1) / lRowNum + 1;
   
   if(utStrIsSpaces(caExport)){ 
//    if(lCurPg <= 0) lCurPg = lTotPg-1;
    if(lCurPg<=0) lCurPg=1;
   }
   else{
   	lCurPg=1;
  }
  lStartRec = (lCurPg - 1) * lRowNum;  
  sprintf(caTemp,"select stime,mfrom,mto,mesg from  nccontlog where ((mfrom='%s' and mto='%s') or (mfrom='%s' and mto='%s')) ",caFrom,caTo,caTo,caFrom);
  
  
      if(strlen(caSdate)>0){
    	sprintf(caTemp+strlen(caTemp)," and stime>=%d ",lStime);
    }
    if(strlen(caEdate)>0){
//    	sprintf(caTemp+strlen(caTemp)," and stime<=%d ",lEtime);
    }
    sprintf(caTemp+strlen(caTemp)," order by stime  limit %d,%d ",lStartRec,lRowNum);
    
  
  
    psCur = pasDbOpenSql(caTemp,0);
 //  printf("caTemp=%s\n",caTemp);
    
  #ifdef LDEBUG
	 printf("caWhere=%s\n",caWhere);
	#endif     
//printf("caWhere=%s\n",caWhere);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
   
    iReturn = 0;
   
    if(utStrIsSpaces(caExport)){
      psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"from",caFrom2);
    utPltPutVar(psDbHead,"to",caTo2);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"sdate1",caSdate1);
    utPltPutVar(psDbHead,"edate1",caEdate1);
   
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
      
    utPltSetCvtHtml(1);
    lTime=0;
    memset(caMesg,0,sizeof(caMesg));
    memset(caTo,0,sizeof(caTo));
    memset(caFrom,0,sizeof(caFrom));
  
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_STRING,47,caFrom,
  	                               UT_TYPE_STRING,47,caTo,
  	                               UT_TYPE_STRING,255,caMesg);
  	                              
   	iNum = 0;
  
   	while((iReturn == 0||iReturn==1405) && iNum < lRowNum) {
   	    utPltPutLoopVar(psDbHead,"stime",iNum+1,utTimFormat("%m-%d %H:%M:%S",lTime));
   	    utPltPutLoopVar(psDbHead,"from",iNum+1,caFrom);
   	    utPltPutLoopVar(psDbHead,"to",iNum+1,caTo);
   	    utPltPutLoopVar(psDbHead,"mesg",iNum+1,caMesg);
   	    
   	    if(strcmp(caFrom,caFrom2)==0){
   	    	utPltPutLoopVar(psDbHead,"color",iNum+1,"blue");
   	    }
   	    else{
   	    	utPltPutLoopVar(psDbHead,"color",iNum+1,"green");
   	    }
                   
         iNum++;
    lTime=0;
    memset(caMesg,0,sizeof(caMesg));
    memset(caTo,0,sizeof(caTo));
    memset(caFrom,0,sizeof(caFrom));
  
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_STRING,47,caFrom,
  	                               UT_TYPE_STRING,47,caTo,
  	                               UT_TYPE_STRING,255,caMesg);
   }
    pasDbCloseCursor(psCur);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncimdet_list.htm");
    return 0;
    
  }
   else {
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"ncimmsg%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
     
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0891IM查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,ncLang("1050即时通讯内容导出\n"));
       
        fprintf(fp,"%s<===>%s聊天内容\n",caFrom,caTo);
       
        ncUtlPrintCsv(fp,5,
                       UT_TYPE_STRING,"用户名",
                       UT_TYPE_STRING,ncLang("0521时间"),
                       UT_TYPE_STRING,ncLang("1050发送者"),
                       UT_TYPE_STRING,ncLang("1051接收者"),
                       UT_TYPE_STRING,ncLang("0407内容"));
          
          
     lTime=0;
    memset(caMesg,0,sizeof(caMesg));
    memset(caTo,0,sizeof(caTo));
    memset(caFrom,0,sizeof(caFrom));
  
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_STRING,47,caFrom,
  	                               UT_TYPE_STRING,47,caTo,
  	                               UT_TYPE_STRING,255,caMesg); 
          
      while(iReturn == 0||iReturn==1405) {

		  
		      ncUtlPrintCsv(fp,5,
		                   UT_TYPE_STRING,caDispname,
                       UT_TYPE_STRING,utTimFormat("%m-%d %H:%M:%S",lTime),
                       UT_TYPE_STRING,caFrom,
                       UT_TYPE_STRING,caTo,
                       UT_TYPE_STRING,caMesg);
                       
		    lTime=0;
		    memset(caMesg,0,sizeof(caMesg));
		    memset(caTo,0,sizeof(caTo));
		    memset(caFrom,0,sizeof(caFrom));
		    iReturn = pasDbFetchInto(psCur,
		      	                           UT_TYPE_ULONG,4,&lTime,
		  	                               UT_TYPE_STRING,47,caFrom,
		  	                               UT_TYPE_STRING,47,caTo,
		  	                               UT_TYPE_STRING,255,caMesg); 
        }  
       pasDbCloseCursor(psCur);
       
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
       
       
        
        return 0;
    }
    
    
   return 0;
    

}