/* 监控软件---报表定制
                     Modified 2003/03/01
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "ncdef.h"
#include "ncsysinfo.h"
#include "ncpkg.h"
/* 显示网络服务饼图*/
int ncWebShowServiceGrp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char sqlbuf[1024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
    char serv_name[32];
    long lService,iNum;
    long long lSum;
    char caDate[20];
    int iReturn,i;
    char caRefresh_flag[8];
    struct ncFlowService_s {
        unsigned long lService;
        unsigned long long lBytes;
    } sService[10];
    
    char preflag[8],nextflag[8],ctime[12];
   lTime=time(0);
 lSum=0;
    utMsgGetSomeNVar(psMsgHead,4,"refresh_flag",UT_TYPE_STRING,1,caRefresh_flag,
                                 "preflag",UT_TYPE_STRING,1,preflag,
                                 "nextflag",UT_TYPE_STRING,1,nextflag,
                                 "ctime",UT_TYPE_STRING,10,ctime);
   if(strlen(ctime)>0){
   	lTime=utTimStrToLong("%Y/%m/%d",ctime);
   	
  }
   if(atol(preflag)==1)
    lTime=lTime-86400;
    
    if(atol(nextflag)==1)
       lTime=lTime+86400;
    
    
   if(atol(caRefresh_flag)==1){
    	  //统计各种服务占的比例
   pasDbExecSql("delete from ncipservicestat",0);
   strcpy(caDate,utTimFormat("%Y/%m/%d",time(0)));  
   sprintf(sqlbuf,"select sum(ubytes+dbytes) byt,service from ncipservice where sdate='%s' group by service order by byt desc ",caDate);

    psCur=pasDbOpenSql(sqlbuf,0);
     if(psCur == NULL) {
           return 0;
       }
      
        iReturn = 0;
        lBytes=0;
        lService=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&lService);                               
	     iNum=0;
        while(iNum<10&&((iReturn==0)||(iReturn==1405)))
        {
        	sService[iNum].lService=lService;
        	sService[iNum].lBytes=lBytes;

	        lBytes=0;
	        lService=0;
	        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                       UT_TYPE_LONG,4,&lService);
	         iNum++;
	  
	        }
       pasDbCloseCursor(psCur); 
   
    for(i=0;i<iNum;i++){
    	      sprintf(sqlbuf,"insert into ncipservicestat(service,sumflow) values(%lu,%llu) ",sService[i].lService,sService[i].lBytes);
 //    	printf("sqlbuf=%s\n",sqlbuf);
        	pasDbExecSql(sqlbuf,0);        
        }
    }

   
  
  if((atol(preflag)==1)||(atol(nextflag)==1)){
  	psDbHead = utPltInitDb();
  	   strcpy(caDate,utTimFormat("%Y/%m/%d",lTime));  
   sprintf(sqlbuf,"select sum(ubytes+dbytes) byt,service from ncipservice where sdate='%s' group by service order by byt desc ",caDate);
//printf("sqlbuf=%s\n",sqlbuf);
   psCur=pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
           return 0;
       }
      
        iReturn = 0;
        lBytes=0;
        lService=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&lService);                               
	     iNum=0;
        while(iNum<10&&((iReturn==0)||(iReturn==1405)))
        {
       
				 strcpy(serv_name,ncCvtSetService(psShmHead,lService)); 

  				if(strcmp(serv_name,"其它")!=0){
         utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	lBytes);        
         utPltPutLoopVar(psDbHead,"service",  iNum+1,ncCvtSetService(psShmHead,lService));
         utPltPutLoopVarF(psDbHead,"lservice",iNum+1,"%d",lService);
         utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",lTime);
         utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
         
         lSum=lSum+lBytes;
         
         sService[iNum].lBytes=lBytes;
         
         iNum++;
					}
	        lBytes=0;
	        lService=0;
	        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                       UT_TYPE_LONG,4,&lService);
	        
	  
	        }
       pasDbCloseCursor(psCur); 
  }
  else{
  	strcpy(caDate,utTimFormat("%Y/%m/%d",time(0)));
   sprintf(sqlbuf,"select sumflow,service from ncipservicestat order by sumflow desc limit 0,10");
//printf("sqlbuf=%s\n",sqlbuf);
        psCur=pasDbOpenSql(sqlbuf,0);

        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0159部门统计"),ncLang("0549数据库出错"));
            return 0;
        }
        psDbHead = utPltInitDb();     
        iReturn = 0;
        lBytes=0;
        lService=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&lService);                               
	     iNum=0;
        while(iNum<10&&((iReturn==0)||(iReturn==1405)))
        {
  				strcpy(serv_name,ncCvtSetService(psShmHead,lService)); 

  				if(strcmp(serv_name,"其它")!=0){
         utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	lBytes);        
         utPltPutLoopVar(psDbHead,"service",  iNum+1,ncCvtSetService(psShmHead,lService));
         utPltPutLoopVarF(psDbHead,"lservice",iNum+1,"%d",lService);
         utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",time(0));
          sService[iNum].lBytes=lBytes;
          utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
         
         iNum++;
         lSum=lSum+lBytes;
        
					}
        lBytes=0;
        lService=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&lService);
         
  
        }

       pasDbCloseCursor(psCur);     
    }
  
    utPltPutVar(psDbHead,"today",caDate);  
    //输出比例
    for(i=0;i<iNum;i++){
    	if(lSum==0){
    		utPltPutLoopVar(psDbHead,"rate",i+1,"0.00");
    	}
    	else{
    	  utPltPutLoopVarF(psDbHead,"rate",i+1,"%0.2f",(float)100*sService[i].lBytes/lSum);
    	}
    }       
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_netservicebt.htm");  
    return 0;
}

//出口流量趋势图
int ncWebShowOutflowGrp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    pasDbCursor *psCur;
    char caDate[20];  
    char sqlbuf[2048];
    unsigned long iMaxItem,lSize;
    unsigned long long lUbytes,lDbytes,lMaxBytes;
    unsigned long stime;
    int      iReturn,i,j,iMax;
    long lStep,lTime,lTime1,lTime2,lTime0,lBase,lBaseX,lBaseY,lHight,lWidth;
    char caSid[32],caSdate[32],caTime[32],caUnit[16],caType[16],caItem[16];
    char caSdate1[32],caSdate2[32],caTime1[32],caTime2[32];
    char caLabel[128];
    char caTemp[8192];
    char caTemp1[32];
    char caName[32];
    int m,n,iNum;
    int mm,nn,temp_nn;
    struct data_s{
    	char caDevname[32];
    	unsigned long long lUbytes[30];
    	unsigned long lCount;
    	unsigned long stime[30];
    } devname[20];
    
    
//    char caDevname[20][32];
 //取设备名   
    sprintf(caTemp,"select name from ncnetdev where flags=0 ");   
    psCur = pasDbOpenSql(caTemp,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错"));
        return 0;
    }
    psDbHead = utPltInitDb();    
    iReturn=0;
    iNum=0;
   
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName);

    while((iReturn == 0)||(iReturn==1405)) {
    	if(iNum>20) break;
    	strcpy(devname[iNum].caDevname,caName);
    	iNum++;
      memset(caName,0,sizeof(caName));
      iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName);
    }
    
    pasDbCloseCursor(psCur);
    
    for(i=0;i<iNum;i++){
 //    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%lu",i+1);
 //    	utPltPutLoopVar(psDbHead,"devname",i+1,caDevname[i]);
     	sprintf(caTemp,"select nowv,stime from ncthisinfo where name='%s' order by stime desc limit 0,30",devname[i].caDevname);
  //  	printf("caTemp=%s\n",caTemp);
     	psCur = pasDbOpenSql(caTemp,0);
    	if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错"));
        return 0;
    	}
       m=0;
       stime=0;
	   	lUbytes=0;
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
	                                   UT_TYPE_LONG,4,&stime);
	     while((iReturn == 0)||(iReturn==1405)) {
	     	
	     	devname[i].lUbytes[m]=lUbytes;
	     	devname[i].stime[m]=stime;
	    	m++;
	    	
//	    	 utPltPutLoopVar2(psDbHead,"x_scal",i+1,m," ");
//    			utPltPutLoopVarF2(psDbHead,"sumbit",i+1,m,"%0.1f",(float)lUbytes/1024);
	    	lUbytes=0;
	    	stime=0;
	      iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
	                                   UT_TYPE_LONG,4,&stime);
	    }
	    devname[i].lCount=m;
	    pasDbCloseCursor(psCur);
	    
	  }
	    for(mm=0;mm<iNum;mm++){
	     	utPltPutLoopVarF(psDbHead,"iNum",mm+1,"%lu",mm+1);
      	utPltPutLoopVar(psDbHead,"devname",mm+1,devname[mm].caDevname);
	    	for(nn=0;nn<devname[mm].lCount;nn++)
	    	{
	    		 temp_nn=devname[mm].lCount-nn-1;
	    		 utPltPutLoopVar2(psDbHead,"x_scal",mm+1,nn,utTimFormat("%H:%M",devname[mm].stime[temp_nn]));
    			 utPltPutLoopVarF2(psDbHead,"sumbit",mm+1,nn,"%0.1f",(float)devname[mm].lUbytes[temp_nn]/1024);
	      }
	    
    }
    
  
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_mainfirst_outflow.htm");
    

    return 0;
}



/* 今日网站访问次数  */
int ncWebMainSortWeb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    ncUserInfo *psUserInfo;
    char sqlbuf[2048];
    char title_name[32];
    char caGroupid[22],caUserid[22];
    long lTime,lTime1,lTime2;
    long lUserid,lGroupid0;
    int iReturn,iNum;
    char caTime[16],sDate1[16],sDate2[16],caHost[65];
    char caGroup[12],caOrder[12],caSum[12],caExport[12];
    unsigned long lTimes,lCount,iSumRec;
    char caSdate[12],caEdate[12],caStime[3],caEtime[3],caRecn[7];
    unsigned long lBytes;
    char *pSelect;
    int i;
    pasDbCursor *psCur;   
    struct ncSortWeb_s {
        char host[64];
        unsigned long  lBytes;
        unsigned long lTime;
        unsigned long lCount;
    } sSortWeb[10];
    char caRefresh_flag[8];
     lTime=time(0);
     strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime)); 
     
    utMsgGetSomeNVar(psMsgHead,1,"refresh_flag",UT_TYPE_STRING,1,caRefresh_flag);
    if(atol(caRefresh_flag)==1){
	    pasDbExecSql("delete from ncwebstat",0);  
	    sprintf(sqlbuf,"select a.host,sum(a.bytes)/1024 abyte,sum(a.ctime) atime,sum(a.lcount) acount from nchttpcount a where a.sdate='%s' ",caSdate);       
	    sprintf(sqlbuf+strlen(sqlbuf)," group by a.host ");  
		  sprintf(sqlbuf+strlen(sqlbuf)," order by acount desc limit 0,10 ");	
// printf("sqlbuf=%s\n",sqlbuf);     
      psCur = pasDbOpenSql(sqlbuf,0);
    
      if(psCur == NULL) {
         return 0;
      }
     
     iReturn = 0;
    iNum = 0;                                   
    iReturn = pasDbFetchInto(psCur,
                                   UT_TYPE_STRING,63,caHost,
                                   UT_TYPE_LONG, 4 , &lBytes,
                                   UT_TYPE_LONG, 4,  &lTimes,
                                   UT_TYPE_LONG, 4,  &lCount);

   
     
        while(iReturn == 0 ) {
         
            strcpy(sSortWeb[iNum].host,caHost);
            sSortWeb[iNum].lBytes=lBytes;
            sSortWeb[iNum].lTime=lTimes;
            sSortWeb[iNum].lCount=lCount;
           
           
            iReturn = pasDbFetchInto(psCur,
                                   UT_TYPE_STRING,63,caHost,
                                   UT_TYPE_LONG, 4 , &lBytes,
                                   UT_TYPE_LONG, 4,  &lTimes,
                                   UT_TYPE_LONG, 4,  &lCount);
          iNum++;                         
        }
        pasDbCloseCursor(psCur);
       
       for(i=0;i<iNum;i++){
       	    sprintf(sqlbuf,"insert into ncwebstat(host,sumflow,sumctime,sumcount) values('%s',%lu,%lu,%lu) ",sSortWeb[i].host,sSortWeb[i].lBytes,sSortWeb[i].lTime,sSortWeb[i].lCount);
//   printf("sqlbuf=%s\n",sqlbuf);    	          	    
        	  pasDbExecSql(sqlbuf,0);
        	}
    
    }
     
     
     
 //   sprintf(sqlbuf,"select a.host,sum(a.bytes)/1024 abyte,sum(a.ctime) atime,sum(a.lcount) acount from nchttpcount a where a.sdate='%s' ",caSdate);       
 //   sprintf(sqlbuf+strlen(sqlbuf)," group by a.host ");  
//	  sprintf(sqlbuf+strlen(sqlbuf)," order by acount desc limit 0,10 ");	
sprintf(sqlbuf,"select host,sumflow,sumctime,sumcount from ncwebstat order by sumcount desc limit 0,10 ");
	
//	printf("sqlbuf=%s\n",sqlbuf);
	psCur = pasDbOpenSql(sqlbuf,0);
    
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0582统计"),ncLang("0550数据库出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }

    iReturn = 0;
    iNum = 0;                                   
    iReturn = pasDbFetchInto(psCur,
                                   UT_TYPE_STRING,63,caHost,
                                   UT_TYPE_LONG, 4 , &lBytes,
                                   UT_TYPE_LONG, 4,  &lTimes,
                                   UT_TYPE_LONG, 4,  &lCount);

   
        psDbHead = utPltInitDb();
    
      
     
        while(iReturn == 0 ) {
            iNum++;
        if(iNum%2==0)
        utPltPutLoopVar(psDbHead,"color",iNum,"#f5F5F5");
        else
        utPltPutLoopVar(psDbHead,"color",iNum,"#ffFfFf");
            utPltPutLoopVarF(psDbHead, "num", iNum,"%d",iNum);
            utPltPutLoopVar(psDbHead, "host", iNum,caHost);
            utPltPutLoopVarF(psDbHead,"bytes",   iNum,"%ld",lBytes);
            utPltPutLoopVar(psDbHead, "ctime",   iNum,ncCvtConntime(lTimes));
            utPltPutLoopVarF(psDbHead, "lcount",   iNum,"%ld",lCount);
            iReturn = pasDbFetchInto(psCur,
                                   UT_TYPE_STRING,63,caHost,
                                   UT_TYPE_LONG, 4 , &lBytes,
                                   UT_TYPE_LONG, 4,  &lTimes,
                                   UT_TYPE_LONG, 4,  &lCount);
        }
        pasDbCloseCursor(psCur);
        
        i = 0;
	     while(iNum < 10) {
	        i++;
	        iNum++;
	        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
	    }
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_ncsortweb.htm");
   
    return 0;
}

/* 归档信息统计  */
int ncWebGenSumRecHtml(utShmHead *psShmHead)
{
    utPltDbHead *psDbHead;
    char sqlbuf[2048];
    char caName[32];
    int iNum;
    int iReturn;
    unsigned long lTime;
    char sDate1[20],sDate2[20];
    unsigned long lRectot,lRecday,lRecmonth,lRecweek,lCount;
    
    char sDatew1[20],sDatew2[20],sDatew1_t[20],sDatew2_t[20];
    char sDatem1[20],sDatem2[20],sDatem1_t[20],sDatem2_t[20];
    char sDated1[20],sDated2[20],sDated1_t[20],sDated2_t[20];
    unsigned long lStimew,lEtimew,lStimem,lEtimem,lStimed,lEtimed;
    unsigned long lip,lhttp,lim,lmails,lmailr,lmailw,lform,lwarn,lerr,lftp;   
    unsigned long lip_t,lhttp_t,lim_t,lmails_t,lmailr_t,lmailw_t,lform_t,lwarn_t,lerr_t,lftp_t;
    char caPath[128];
    pasDbCursor *psCur;   
    lTime=time(0);
    ncUtlWeekDate(lTime,sDatew1,sDatew2);;
   sprintf(sDatew1_t,"%s 00:00:00",sDatew1);
   sprintf(sDatew2_t,"%s 23:59:59",sDatew2);   
   lStimew = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDatew1_t);
   lEtimew = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDatew2_t);

   ncUtlMonthDate(lTime,sDatem1,sDatem2);
   sprintf(sDatem1_t,"%s 00:00:00",sDatem1);
   sprintf(sDatem2_t,"%s 23:59:59",sDatem2);   
   lStimem = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDatem1_t);
   lEtimem = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDatem2_t);

   ncUtlDayDate(lTime,sDated1,sDated2);
   
   sprintf(sDated1_t,"%s 00:00:00",sDated1);
   sprintf(sDated2_t,"%s 23:59:59",sDated2);   
   lStimed = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDated1_t);
   lEtimed = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDated2_t);
   
   
 psDbHead = utPltInitDb();
 //  今日
 sprintf(sqlbuf,"select sum(recip),sum(rechttp),sum(recim),sum(recmails),sum(recmailr),sum(recmailw),sum(recform),sum(recwarn),sum(recerr),sum(recftp) from ncstat where stime>=%lu and stime<=%lu ",lStimed,lEtimed);
// printf("sqlbuf=%s\n",sqlbuf);
 iReturn=pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lip_t,
                         UT_TYPE_LONG,4,&lhttp_t,
                         UT_TYPE_LONG,4,&lim_t,
                         UT_TYPE_LONG,4,&lmails_t,
                         UT_TYPE_LONG,4,&lmailr_t,
                         UT_TYPE_LONG,4,&lmailw_t,
                         UT_TYPE_LONG,4,&lform_t,
                         UT_TYPE_LONG,4,&lwarn_t,
                         UT_TYPE_LONG,4,&lerr_t,
                         UT_TYPE_LONG,4,&lftp_t);
   
   if(iReturn==0){
   	utPltPutVarF(psDbHead,"lip_t","%lu",lip_t);
   	utPltPutVarF(psDbHead,"lhttp_t","%lu",lhttp_t);
   	utPltPutVarF(psDbHead,"lim_t","%lu",lim_t);
   	utPltPutVarF(psDbHead,"lmails_t","%lu",lmails_t);
   	utPltPutVarF(psDbHead,"lmailr_t","%lu",lmailr_t);
   	utPltPutVarF(psDbHead,"lmailw_t","%lu",lmailw_t);
   	utPltPutVarF(psDbHead,"lform_t","%lu",lform_t);
   	utPltPutVarF(psDbHead,"lwarn_t","%lu",lwarn_t);
   	utPltPutVarF(psDbHead,"lerr_t","%lu",lerr_t);
   	utPltPutVarF(psDbHead,"lftp_t","%lu",lftp_t);
  }
        
    
   //  本周
 sprintf(sqlbuf,"select sum(recip),sum(rechttp),sum(recim),sum(recmails),sum(recmailr),sum(recmailw),sum(recform),sum(recwarn),sum(recerr),sum(recftp) from ncstat where stime>=%lu and stime<=%lu ",lStimew,lEtimew);
// printf("sqlbuf=%s\n",sqlbuf);
 iReturn=pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lip,
                         UT_TYPE_LONG,4,&lhttp,
                         UT_TYPE_LONG,4,&lim,
                         UT_TYPE_LONG,4,&lmails,
                         UT_TYPE_LONG,4,&lmailr,
                         UT_TYPE_LONG,4,&lmailw,
                         UT_TYPE_LONG,4,&lform,
                         UT_TYPE_LONG,4,&lwarn,
                         UT_TYPE_LONG,4,&lerr,
                         UT_TYPE_LONG,4,&lftp);
   
   if(iReturn==0){
   	utPltPutVarF(psDbHead,"lip_w","%lu",lip);
   	utPltPutVarF(psDbHead,"lhttp_w","%lu",lhttp);
   	utPltPutVarF(psDbHead,"lim_w","%lu",lim);
   	utPltPutVarF(psDbHead,"lmails_w","%lu",lmails);
   	utPltPutVarF(psDbHead,"lmailr_w","%lu",lmailr);
   	utPltPutVarF(psDbHead,"lmailw_w","%lu",lmailw);
   	utPltPutVarF(psDbHead,"lform_w","%lu",lform);
   	utPltPutVarF(psDbHead,"lwarn_w","%lu",lwarn);
   	utPltPutVarF(psDbHead,"lerr_w","%lu",lerr);
   	utPltPutVarF(psDbHead,"lftp_w","%lu",lftp);
  }    
     
     //  本月
 sprintf(sqlbuf,"select sum(recip),sum(rechttp),sum(recim),sum(recmails),sum(recmailr),sum(recmailw),sum(recform),sum(recwarn),sum(recerr),sum(recftp) from ncstat where stime>=%lu and stime<=%lu ",lStimem,lEtimem);
//printf("sqlbuf=%s\n",sqlbuf);
 iReturn=pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lip,
                         UT_TYPE_LONG,4,&lhttp,
                         UT_TYPE_LONG,4,&lim,
                         UT_TYPE_LONG,4,&lmails,
                         UT_TYPE_LONG,4,&lmailr,
                         UT_TYPE_LONG,4,&lmailw,
                         UT_TYPE_LONG,4,&lform,
                         UT_TYPE_LONG,4,&lwarn,
                         UT_TYPE_LONG,4,&lerr,
                         UT_TYPE_LONG,4,&lftp);
   
   if(iReturn==0){
   	utPltPutVarF(psDbHead,"lip_m","%lu",lip);
   	utPltPutVarF(psDbHead,"lhttp_m","%lu",lhttp);
   	utPltPutVarF(psDbHead,"lim_m","%lu",lim);
   	utPltPutVarF(psDbHead,"lmails_m","%lu",lmails);
   	utPltPutVarF(psDbHead,"lmailr_m","%lu",lmailr);
   	utPltPutVarF(psDbHead,"lmailw_m","%lu",lmailw);
   	utPltPutVarF(psDbHead,"lform_m","%lu",lform);
   	utPltPutVarF(psDbHead,"lwarn_m","%lu",lwarn);
   	utPltPutVarF(psDbHead,"lerr_m","%lu",lerr);
   	utPltPutVarF(psDbHead,"lftp_m","%lu",lftp);
  }     
   //合计
 
  pasDbOneRecord("select count(*) from nciplog",0,UT_TYPE_LONG,4,&lip);
  pasDbOneRecord("select count(*) from nchttplog",0,UT_TYPE_LONG,4,&lhttp);  
  pasDbOneRecord("select count(*) from nccontlog",0,UT_TYPE_LONG,4,&lim); 
  pasDbOneRecord("select count(*) from ncmailmon where flags=1",0,UT_TYPE_LONG,4,&lmails);
  pasDbOneRecord("select count(*) from ncmailmon where flags=2",0,UT_TYPE_LONG,4,&lmailr);
  pasDbOneRecord("select count(*) from ncmailmon where flags=3",0,UT_TYPE_LONG,4,&lmailw);
  
  pasDbOneRecord("select count(*) from ncformmon",0,UT_TYPE_LONG,4,&lform);
  pasDbOneRecord("select count(*) from ncwarnlog",0,UT_TYPE_LONG,4,&lwarn);
  pasDbOneRecord("select count(*) from ncerrlog",0,UT_TYPE_LONG,4,&lerr);
  pasDbOneRecord("select count(*) from ncnetseslog",0,UT_TYPE_LONG,4,&lftp);
  
    utPltPutVarF(psDbHead,"lip_s","%lu",lip);
   	utPltPutVarF(psDbHead,"lhttp_s","%lu",lhttp);
   	utPltPutVarF(psDbHead,"lim_s","%lu",lim);
   	utPltPutVarF(psDbHead,"lmails_s","%lu",lmails);
   	utPltPutVarF(psDbHead,"lmailr_s","%lu",lmailr);
   	utPltPutVarF(psDbHead,"lmailw_s","%lu",lmailw);
   	utPltPutVarF(psDbHead,"lform_s","%lu",lform);
   	utPltPutVarF(psDbHead,"lwarn_s","%lu",lwarn);
   	utPltPutVarF(psDbHead,"lerr_s","%lu",lerr);
   	utPltPutVarF(psDbHead,"lftp_s","%lu",lftp);
  
   sprintf(caPath,"%s/html/nc_mainfirst_sumrec.htm",utComGetVar_sd(psShmHead,"NCHOME","/home/ncmysql/nc"));
   utPltFileToHtmlFile("nc/iframe_mainfirst_sumrec.htm",caPath,psDbHead);
   sprintf(sqlbuf,"chmod 777 %s",caPath);
   system(sqlbuf);
   
   
   iReturn=pasDbOneRecord("select count(*) from ncstattoday ",0,UT_TYPE_LONG,4,&lCount);
   if(iReturn!=0) return -1;
   
   if(lCount>0){
   sprintf(sqlbuf,"update ncstattoday set iptoday=%lu,httptoday=%lu,imtoday=%lu,mailstoday=%lu,mailrtoday=%lu,mailwtoday=%lu,formtoday=%lu,warntoday=%lu,errtoday=%lu,ftptoday=%lu,iptot=%lu,httptot=%lu,imtot=%lu,mailstot=%lu,mailrtot=%lu,mailwtot=%lu,formtot=%lu,warntot=%lu,errtot=%lu,ftptot=%lu,stime=%lu ",\
   lip_t,lhttp_t,lim_t,lmails_t,lmailr_t,lmailw_t,lform_t,lwarn_t,lerr_t,lftp_t,lip,lhttp,lim,lmails,lmailr,lmailw,lform,lwarn,lerr,lftp,lTime);
//   printf("sqlbuf=%s\n",sqlbuf);
   iReturn=pasDbExecSql(sqlbuf,0);
  }
  else
   {
   sprintf(sqlbuf,"insert into ncstattoday(iptoday,httptoday,imtoday,mailstoday,mailrtoday,mailwtoday,formtoday,warntoday,errtoday,ftptoday,iptot,httptot,imtot,mailstot,mailrtot,mailwtot,formtot,warntot,errtot,ftptot,stime) \
   	values(%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu)",\
   	   lip_t,lhttp_t,lim_t,lmails_t,lmailr_t,lmailw_t,lform_t,lwarn_t,lerr_t,lftp_t,lip,lhttp,lim,lmails,lmailr,lmailw,lform,lwarn,lerr,lftp,lTime);
   	
 //  	printf("sqlbuf=%s\n",sqlbuf);
   	pasDbExecSql(sqlbuf,0);
  }
    return 0;
}




/* 显示主页Right*/
int ncWebShowFirstRight(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char sqlbuf[1024];
    char caDate[20];
    utPltDbHead *psDbHead;
    psDbHead = utPltInitDb();     
         
    strcpy(caDate,utTimFormat("%Y/%m/%d",time(0)));
    utPltPutVar(psDbHead,"sdate",caDate); 
    
    utPltPutVar(psDbHead,"onuserstep",utComGetVar_sd(psShmHead,"onuserstep","6"));
    
            
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main_first_right.htm");  
    return 0;
}

/* 显示主页left*/
int ncWebShowFirstLeft(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char sqlbuf[1024];
    utPltDbHead *psDbHead;
    FILE *fp;
    char *p;
    int iCount;
    double fRate,cpu;
    char caBuffer[132];
    char caDev[32];
    char caRate[32];
    char caMount[320];
    char caTemp[1024];
    char caCurTime[32];
    char caDispeth[128];
   char caDevname[10][32];
    long lTotal,lUsed,lFree,lW,lMax,lCur,lRate;
    unsigned long mem,freemem,swap,freeswap;
    int i,iReturn,j,iNum;
    pasDbCursor *psCur;
    char caUsername[32];
    long lGroupid,lUserid;
    unsigned long lPkg,lFlow;
    long lId,lUsrlevel;
    long ethcount;
    char caName[32],caGroupname[32];
    long long lPacket[2],lBytes[2];
    char caIp[20],caMask[20],caMac[20];
    long iUp;
    char mount_desc[320];
    char caSoftver[32],caDbver[32],caUpdate[32];
    char caMsg[128];
    long lSumRecord;
    char caDbrate[12];
    float fDiskrate;
    strcpy(caDbrate,utComGetVar_sd(psShmHead,"diskrate","98"));
    fDiskrate=atof(caDbrate);
    memset(caMsg,0,sizeof(caMsg));
    
    
    psDbHead = utPltInitDb(); 
    
    memset(caUsername,0,sizeof(caUsername));
    lGroupid=0;
    lUserid=0;
    
    iReturn=dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername);

    if(iReturn==0){
    	sprintf(caTemp,"select id,name,groupname,usrlevel from dsuser where dispname='%s' ",caUsername);
    	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lId,
    	                        UT_TYPE_STRING,30,caName,
    	                        UT_TYPE_STRING,30,caGroupname,
    	                        UT_TYPE_LONG,4,&lUsrlevel);
    	                        
  //  	utPltPutVar(psDbHead,"lev",nccvtUsrLevel(lUsrlevel));
    	utPltPutVar(psDbHead,"username",caName);
    	utPltPutVar(psDbHead,"dispname",caUsername);
    	utPltPutVar(psDbHead,"groupname",caGroupname);
    	
    	
    	sprintf(caTemp,"select dispname from dsuserrole a,dsrole b where roleid=b.id and a.id=%d ",lId);
    psCur = pasDbOpenSql(caTemp,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错"));
        return 0;
    }
      
    iReturn=0;
   
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName);
    memset(caTemp,0,sizeof(caTemp));
    while((iReturn == 0)||(iReturn==1405)) {
      sprintf(caTemp+strlen(caTemp),"%s  ",caName);
      memset(caName,0,sizeof(caName));
      iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName);
     }
    
    	pasDbCloseCursor(psCur);
    
    	utPltPutVar(psDbHead,"right",caTemp);
    	   	
    }

    ethcount=atol(utComGetVar_sd(psShmHead,"ethcount","3"));
    iNum=0;
    for(i=0;i<ethcount;i++){
    	sprintf(caDev,"eth%d",i);
    	strcpy(caDispeth,utComGetVar_sd(psShmHead,"dispeth","eth0,eth1,eth2"));
    	if(strstr(caDispeth,caDev)){
    	iReturn=gdgGetNetdevInfo(caDev,caIp,caMask,caMac,lPacket,lBytes,&iUp);
    	if(iReturn==1){
    		iNum++;
    		utPltPutLoopVar(psDbHead,"dev1",iNum,caDev);
    		utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
    		utPltPutLoopVar(psDbHead,"mask",iNum,caMask);
    		utPltPutLoopVar(psDbHead,"mac",iNum,caMac);
    		if(iUp==0){
    		utPltPutLoopVar(psDbHead,"state",iNum,"<font color=red>停用</font>");
    	  }
    	  else if(iUp==1){
    	  	utPltPutLoopVar(psDbHead,"state",iNum,"<font color=green>启用</font>");
    	  }
    	
    	}
    }
    } 
     
    iReturn=gdgGetNetdevInfo("pronc",caIp,caMask,caMac,lPacket,lBytes,&iUp); 
     if(iReturn==1){
     	iNum++;
     	   utPltPutLoopVar(psDbHead,"dev1",iNum,"网桥");
    		utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
    		utPltPutLoopVar(psDbHead,"mask",iNum,caMask);
    		utPltPutLoopVar(psDbHead,"mac",iNum,caMac);
    		if(iUp==0){
    		utPltPutLoopVar(psDbHead,"state",iNum,"<font color=red>停用</font>");
    	  }
    	  else if(iUp==1){
    	  	utPltPutLoopVar(psDbHead,"state",iNum,"<font color=green>启用</font>");
    	  }
       } 

        /* 磁盘使用情况  */
    iCount = 0;
    fp = popen("df -k","r");
    if(fp) {
        p = fgets(caBuffer,131,fp);
        p = fgets(caBuffer,131,fp);
        while(p) {
            sscanf(caBuffer,"%s %d %d %d %s %s",
                caDev,&lTotal,&lUsed,&lFree,caRate,caMount);
            if(strncmp(caDev,"/dev/",5) == 0) {
                iCount++;
                utPltPutLoopVar(psDbHead,"total",iCount,utStrLtoF(lTotal,caTemp));
                utPltPutLoopVar(psDbHead,"used",iCount,utStrLtoF(lUsed,caTemp));
                utPltPutLoopVar(psDbHead,"free",iCount,utStrLtoF(lFree,caTemp));
                utPltPutLoopVar(psDbHead,"dev",iCount,caDev);
                utPltPutLoopVar(psDbHead,"mount",iCount,caMount);
                if(strcmp(caMount,"/")==0){
                	strcpy(mount_desc,ncLang("1801根分区"));
                }
                else if(strcmp(caMount,"/boot")==0){
                	strcpy(mount_desc,ncLang("1802启动分区"));
                }
                else if(strcmp(caMount,"/home")==0){
                	strcpy(mount_desc,ncLang("1805系统分区"));
                }
                else if(strcmp(caMount,"/db")==0){
                	strcpy(mount_desc,ncLang("1804数据库"));
                }
                else if(strcmp(caMount,"/data")==0){
                	strcpy(mount_desc,ncLang("1803日志文件"));
                }
                else{
                	strcpy(mount_desc,caMount);
                }
                utPltPutLoopVar(psDbHead,"mount_desc",iCount,mount_desc);
                if(lTotal > 0) {
                    fRate = (double)lUsed / (double)lTotal;
                    utPltPutLoopVarF(psDbHead,"rate",iCount,"%6.2f%",fRate*(double)100);
                    
                    if(fRate*100>=fDiskrate){
                    	strcpy(caMsg,"磁盘快要用完，请尽快采取措施!");
                    }
                    
                    
                    lW = fRate * 70;
                    utPltPutLoopVarF(psDbHead,"usedt",iCount,"%d",lW);
                    utPltPutLoopVarF(psDbHead,"freet",iCount,"%d",70-lW);
                    
                }
            }
            p = fgets(caBuffer,132,fp);
        }
        pclose(fp);
    }
    /* 当前运行情况  */
    long lCpunum;    
    lCpunum=atol(utComGetVar_sd(psShmHead,"cpunumber","1"));
    
    iReturn = pasGetCpuRate("nc",&cpu,&mem);
    if(iReturn >= 0) {
        fRate = cpu / (100*lCpunum);
        lW = fRate * 70;
        utPltPutVarF(psDbHead,"r1used","%d",lW);
        utPltPutVarF(psDbHead,"r1free","%d",70-lW);
        utPltPutVarF(psDbHead,"r1","%6.2f",fRate * 100);
        utPltPutVarF(psDbHead,"sumprocess","%ld",iReturn);
    }
    iReturn = pasGetMemInfo(&mem, &freemem,&swap,&freeswap);
    if(iReturn == 0 ) {
        if( mem > 0) {
            fRate = (double) (mem - freemem)/ (double) mem;
            lW = fRate * 70;
            utPltPutVarF(psDbHead,"r5used","%d",lW);
            utPltPutVarF(psDbHead,"r5free","%d",70-lW);
            utPltPutVarF(psDbHead,"r5","%6.2f",fRate * 100);
            utPltPutVarF(psDbHead,"mem","%s",utStrLtoF(mem,caTemp));
            utPltPutVarF(psDbHead,"free","%s",utStrLtoF(freemem,caTemp));
        }
        
        if(swap > 0) {
            fRate = (double) (swap - freeswap)/(double) swap;
            lW = fRate * 70;
            utPltPutVarF(psDbHead,"r6used","%d",lW);
            utPltPutVarF(psDbHead,"r6free","%d",70-lW);
            utPltPutVarF(psDbHead,"r6","%6.2f",fRate * 100);
            utPltPutVarF(psDbHead,"swap","%s",utStrLtoF(swap,caTemp));
            utPltPutVarF(psDbHead,"freeswap","%s",utStrLtoF(freeswap,caTemp));
        }
    } 
    
    p = utComGetVar_s(psShmHead,"ProductSn");
    if(p) {
        unsigned long lExpire,lUsers,lStart;
        iReturn = pasInstGetLicense(p,&lExpire,&lUsers,&lStart);
        if(iReturn == 1) {
            utPltPutVarF(psDbHead,"prodsn","%s",p);
            utPltPutVarF(psDbHead,"users","%lu",lUsers);
        }
    } 
    
      
    sprintf(caTemp,"select name from ncnetdev  where flags=0 ");   
    psCur = pasDbOpenSql(caTemp,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错"));
        return 0;
    }
      
    iReturn=0;
    iNum=0;
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName);

    while((iReturn == 0)||(iReturn==1405)) {
    	if(iNum>5) break;
    	strcpy(caDevname[iNum],caName);
    	iNum++;
      memset(caName,0,sizeof(caName));
      iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName);
     }
    
    pasDbCloseCursor(psCur);
    for(i=0;i<iNum;i++){
    	sprintf(caTemp,"select maxv,nowv from ncthisinfo where name='%s' order by stime desc limit 0,5",caDevname[i]);
   
      pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lPkg,
                              UT_TYPE_LONG,4,&lFlow);
       utPltPutLoopVar(psDbHead,"devname",i+1,caDevname[i]);
       utPltPutLoopVarF(psDbHead,"devbase",i+1,"%d",lPkg);
       utPltPutLoopVarF(psDbHead,"devstat",i+1,"%0.1f",(float)lFlow/1024);
      utPltPutLoopVarF(psDbHead,"iNum",i+1,"%lu",i+1);          
     
    } 
    utPltPutVar(psDbHead,"version",NC_VERSION);  
    utPltPutVar(psDbHead,"msg",caMsg);
    

//网址库版本
    ncUrlGetUrlVersion(caSoftver,caDbver,caUpdate,&lSumRecord);   
    utPltPutVar(psDbHead,"wzk",caDbver);
 
 //协议库版本    
    ncPatGetSysInfo(caSoftver,caUpdate,&lSumRecord,caDbver);
    utPltPutVar(psDbHead,"xyk",caDbver);
     
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main_first_left.htm");  
    return 0;
}

//系统统计参数统计
int ncWebSysParStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   char caTemp[1024];   
 
    int i,iNum; 
    utPltDbHead *psDbHead;
   	int iReturn;
 	pasDbCursor *psCur;
#ifdef LDEBUG
    printf(" Pid == %d \n",getpid());
#endif
   char caName[16];
   unsigned long maxv,maxnowv; 
   double avnowv;   
 	utMsgPrintMsg(psMsgHead);

   sprintf(caTemp,"select name,max(maxv),max(nowv),avg(nowv) from ncthisinfo group by name ");
   
    
   
    psCur = pasDbOpenSql(caTemp,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }        
    psDbHead = utPltInitDb();
    
    memset(caName,0,sizeof(caName));
    maxv=0;
    maxnowv=0;
    avnowv=0;
   
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,16,caName,
   	                               UT_TYPE_LONG,4,&maxv,
   	                               UT_TYPE_LONG,4,&maxnowv,
   	                               UT_TYPE_FLOAT,sizeof(double),&avnowv);
  	                             
 
   	iNum = 0;
   	while(iReturn == 0||iReturn==1405) {
        iNum++;
      
        utPltPutLoopVar(psDbHead,"name",iNum,caName);
        utPltPutLoopVarF(psDbHead,"maxv",iNum,"%lu",maxv);
        utPltPutLoopVarF(psDbHead,"maxnowv",iNum,"%lu",maxnowv);
        utPltPutLoopVarF(psDbHead,"avnowv",iNum,"%0.2f",avnowv);
             
		    memset(caName,0,sizeof(caName));
		    maxv=0;
		    maxnowv=0;
		    avnowv=0;   
  	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,16,caName,
   	                               UT_TYPE_LONG,4,&maxv,
   	                               UT_TYPE_LONG,4,&maxnowv,
   	                               UT_TYPE_FLOAT,sizeof(double),&avnowv);
    }
    pasDbCloseCursor(psCur);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_list_sysparstat.htm");
    return 0;
}




//出口流量趋势图
int ncWebShowOutflowMore(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    pasDbCursor *psCur;
    char caDate[20];  
    char caFlag[8];
    char caPre_flag[8],caAv_un[8];
    char sqlbuf[2048];
    unsigned long iMaxItem,lSize;
    unsigned long long lUbytes,lDbytes,lMaxBytes;
    unsigned long stime;
    int      iReturn,i,j,iMax;
    long lStep,lTime,lStarttime,lEndtime,lTime1,lTime2,lTime0,lBase,lBaseX,lBaseY,lHight,lWidth;
    char caSid[32],caSdate[32],caTime[32],caUnit[16],caType[16],caItem[16];
    char caSdate1[32],caSdate2[32],caTime1[32],caTime2[32];
    char caLabel[128];
    char caTemp[8192];
    char caTemp1[32];
    char caName[32];
    char caStime[16];
    char caSdate_temp1[20],caSdate_temp2[20];
    int m,n,iNum;
    int mm,nn,temp_nn;
    unsigned int temp_m;
    char temp_t[16];
    struct data_s{
    	char caDevname[32];
    	unsigned long long lUbytes[60];
    	unsigned long lCount;
    	unsigned long stime[60];
    } devname[20];
    
		utMsgGetSomeNVar(psMsgHead,4,"av_un",UT_TYPE_STRING,2,caAv_un,
																 "pre_flag",UT_TYPE_STRING,2,caPre_flag,
																 "flag",     UT_TYPE_STRING, 2, caFlag,
		                             "stime",    UT_TYPE_STRING, 14,caStime);
	if(atol(caAv_un)==2){
			if(strlen(caStime)==0){
		    lTime=time(0);
		     strcpy(caSdate_temp1,utTimFormat("%Y/%m/%d",lTime));
		     sprintf(caSdate_temp2,"%s 00:00:00",caSdate_temp1);
		     lStarttime=utTimStrToLong("%Y/%m/%d %H:%M:%S",caSdate_temp2);
		    lEndtime=lTime;
		  }
		  else{
		  	 lStarttime=utTimStrToLong("%Y/%m/%d %H",caStime);
		  	 strcpy(caSdate_temp1,utTimFormat("%Y/%m/%d",lStarttime));
		     sprintf(caSdate_temp2,"%s 00:00:00",caSdate_temp1);
		     lStarttime=utTimStrToLong("%Y/%m/%d %H:%M:%S",caSdate_temp2);
		  	
		  	lEndtime=lStarttime+24*3600;
		  }
		  
		  if(atol(caPre_flag)==1){
		  	lStarttime=lStarttime-24*3600;
		  	lEndtime=lEndtime-24*3600;
		  }
		  else if(atol(caPre_flag)==2){
		  	lStarttime=lStarttime+24*3600;
		  	lEndtime=lEndtime+24*3600;
		  }
		
	}
	else{
			if(strlen(caStime)==0){
		    lTime=time(0);
		    lStarttime=lTime-3600;
		    lEndtime=lTime;
		  }
		  else{
		  	lStarttime=utTimStrToLong("%Y/%m/%d %H",caStime);
		  	lEndtime=lStarttime+3600;
		  }
		  
		  if(atol(caPre_flag)==1){
		  	lStarttime=lStarttime-3600;
		  	lEndtime=lEndtime-3600;
		  }
		  else if(atol(caPre_flag)==2){
		  	lStarttime=lStarttime+3600;
		  	lEndtime=lEndtime+3600;
		  }
		}
//    char caDevname[20][32];
 //取设备名   
    sprintf(caTemp,"select name from ncnetdev where flags=0 ");   
    psCur = pasDbOpenSql(caTemp,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错"));
        return 0;
    }
    psDbHead = utPltInitDb();    
    iReturn=0;
    iNum=0;
   
    memset(caName,0,sizeof(caName));
    memset(&devname[0],0,sizeof(struct data_s));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName);

    while((iReturn == 0)||(iReturn==1405)) {
    	if(iNum>20) break;
    	strcpy(devname[iNum].caDevname,caName);
    	iNum++;
      memset(caName,0,sizeof(caName));
      memset(&devname[iNum],0,sizeof(struct data_s));
      iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName);
    }
    
    pasDbCloseCursor(psCur);
    
    for(i=0;i<iNum;i++){
    	if(atol(caAv_un)==2){
    		sprintf(caTemp,"select 60*nowv,stime from ncthisinfo where name='%s' and stime>=%lu and stime<=%lu order by stime desc ",devname[i].caDevname,lStarttime,lEndtime);
    	}
    	else{
     		sprintf(caTemp,"select nowv,stime from ncthisinfo where name='%s' and stime>=%lu and stime<=%lu order by stime desc limit 0,60",devname[i].caDevname,lStarttime,lEndtime);
      }
     	psCur = pasDbOpenSql(caTemp,0);
    	if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错"));
        return 0;
    	}
       m=0;
       stime=0;
	   	lUbytes=0;
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
	                                   UT_TYPE_LONG,4,&stime);
	     while((iReturn == 0)||(iReturn==1405)) { 
	     	if(atol(caAv_un)==2){
	     		strcpy(temp_t,utTimFormat("%H",stime));
	     		temp_m=atol(temp_t);
	     	
	     		devname[i].lUbytes[temp_m]=devname[i].lUbytes[temp_m]+lUbytes;
	     		devname[i].stime[temp_m]=stime;
	     	}
	     	else{  	
	     		devname[i].lUbytes[m]=lUbytes;
	     		devname[i].stime[m]=stime;
	    		m++;
	      }
	    	lUbytes=0;
	    	stime=0;
	      iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
	                                   UT_TYPE_LONG,4,&stime);
	    }
	    if(atol(caAv_un)==2){
	    	devname[i].lCount=24;
	    }
	    else{
	    	devname[i].lCount=m;
	    }
	    pasDbCloseCursor(psCur);
	   
	   
	    
	  }
	  
	  
	    for(mm=0;mm<iNum;mm++){
	     	utPltPutLoopVarF(psDbHead,"iNum",mm+1,"%lu",mm+1);
      	utPltPutLoopVar(psDbHead,"devname",mm+1,devname[mm].caDevname);
      	if(atol(caAv_un)==2){
      		for(nn=0;nn<24;nn++){
      			
      		   utPltPutLoopVarF2(psDbHead,"x_scal",mm+1,nn+1,"%2d",nn);
    			 	utPltPutLoopVarF2(psDbHead,"sumbit",mm+1,nn+1,"%0.1f",(float)devname[mm].lUbytes[nn]/(3600*1024));
    			}
    		}
    		else{
	    		for(nn=0;nn<devname[mm].lCount;nn++)
	    		{
	    		 	temp_nn=devname[mm].lCount-nn-1;
	    		 	utPltPutLoopVar2(psDbHead,"x_scal1",mm+1,nn+1,utTimFormat("%m/%d %H:%M",devname[mm].stime[temp_nn]));
	    		 	utPltPutLoopVar2(psDbHead,"x_scal",mm+1,nn+1,utTimFormat("%H:%M",devname[mm].stime[temp_nn]));
    			 	utPltPutLoopVarF2(psDbHead,"sumbit",mm+1,nn+1,"%0.1f",(float)devname[mm].lUbytes[temp_nn]/1024);
    			
    			}
	     }
	    
    }
    utPltPutVar(psDbHead,"stime",utTimFormat("%Y/%m/%d %H",lStarttime));
  if(atol(caFlag)==1)
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_mainfirst_outflowtab.htm");
  else
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_mainfirst_outflowmore.htm");
    
    return 0;
}

 struct ncFlowService_st {
        unsigned long lService;
        unsigned long long lBytes_d;
        unsigned long long lBytes_w;
        unsigned long long lBytes_m;
    };


int ncUtlLoolServiceById(struct ncFlowService_st *arrIn,int iSumArr,unsigned long lService)
{
   int bottom,middle,top;
    int n;
    top = iSumArr - 1;
    bottom = 0;
    while(top >= bottom) {
        middle = (top + bottom) / 2;   
        if(arrIn[middle].lService==lService) {
            return middle;
        }
        else if(arrIn[middle].lService>lService) {
            top = middle -1;
        }
        else {
            bottom = middle + 1;
        }
    }
    return (-1);
}


 static int ncUtlSortOnService(char *p1,char *p2)
{
    int i;
    long long l;
   
    struct ncFlowService_st psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct ncFlowService_st));
    memcpy(&psC2,p2,sizeof(struct ncFlowService_st));    

    return psC2.lBytes_m-psC1.lBytes_m;
  
}
 


/* 生成服务统计页面 */
int ncWebGenServiceHtml(utShmHead *psShmHead)
{
    utPltDbHead *psDbHead;
    char sqlbuf[2048];
    char caName[32];
    int iNum;
    int iReturn;
    char caFileplate[128];
    unsigned long lTime;
    char sDate1[20],sDate2[20];
    unsigned long long lBytes,lBytes_m,lBytes_w;
    unsigned long lService;
    unsigned long lLastTime;
    int i,index,m;
    char sDatew1[20],sDatew2[20];
    char sDatem1[20],sDatem2[20];
    char sDated1[20],sDated2[20];
    struct ncFlowService_st sService[200];
    char caPath[128];
    pasDbCursor *psCur;
    
    lLastTime = ncGetLastInfo_l("ServiceStat",0);
    
    
     sprintf(caPath,"%s/html/nc_mainfirst_service.htm",utComGetVar_sd(psShmHead,"NCHOME","/home/ncmysql/nc"));
    lTime=time(0);
    ncUtlWeekDate(lTime,sDatew1,sDatew2);;
 

   ncUtlMonthDate(lTime,sDatem1,sDatem2);
   
   ncUtlDayDate(lTime,sDated1,sDated2);
 
 printf("sDated1=%s\n",sDated1);
 printf("lLastTime=%s\n",utTimFormat("%Y/%m/%d",lLastTime));
 if(strcmp(sDated1,utTimFormat("%Y/%m/%d",lLastTime))!=0)
 { 
 
	 //  本月
	  sprintf(sqlbuf,"select sum(ubytes+dbytes) byt,service from ncipservice where sdate>='%s' and sdate<'%s' and service!=0 group by service order by service limit 0,200 ",sDatem1,sDated1);
printf("sqlbuf=%s\n",sqlbuf);	
	  psCur=pasDbOpenSql(sqlbuf,0);
	  if(psCur == NULL) {
	           return 0;
	   }
	 
	   iReturn = 0;
	   lBytes=0;
	   lService=0;
	   iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                  UT_TYPE_LONG,4,&lService);
	   iNum=0;
	   while((iReturn==0)||(iReturn==1405))
	   {  
	         	sService[iNum].lService=lService;
	        	sService[iNum].lBytes_m=lBytes;
	          sService[iNum].lBytes_w=0;
	          sService[iNum].lBytes_d=0;
		        lBytes=0;
		        lService=0;
		        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
		                                       UT_TYPE_LONG,4,&lService);
		         iNum++;
		  
		  }
	    pasDbCloseCursor(psCur); 
	   
	   
	    //  本周
	  sprintf(sqlbuf,"select sum(ubytes+dbytes) byt,service from ncipservice where sdate>='%s' and sdate<'%s' and service!=0 group by service order by service limit 0,200 ",sDatew1,sDated1);
//	printf("sqlbuf=%s\n",sqlbuf);
	  psCur=pasDbOpenSql(sqlbuf,0);
	  if(psCur == NULL) {
	           return 0;
	   }
	 
	   iReturn = 0;
	   lBytes=0;
	   lService=0;
	   iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                  UT_TYPE_LONG,4,&lService);
	   
	   while((iReturn==0)||(iReturn==1405))
	   {  
	   	index=ncUtlLoolServiceById(&sService,iNum,lService);
	   	if(index!=-1){
	
	          sService[index].lBytes_w=lBytes;         
	    }
	    else{
	    		  sService[iNum].lService=lService;
	        	sService[iNum].lBytes_m=0;
	          sService[iNum].lBytes_w=lBytes;
	          sService[iNum].lBytes_d=0;
	    	iNum++;
	    	   
	    }
		        lBytes=0;
		        lService=0;
		        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
		                                       UT_TYPE_LONG,4,&lService);
		  
		  }
	    pasDbCloseCursor(psCur); 
   
     pasDbExecSql("delete from ncipservicestat",0);
    for(i=0;i<iNum;i++){
    	sprintf(sqlbuf,"insert into ncipservicestat(service,sumflow_m,sumflow_w) values(%lu,%llu,%llu)",\
    	sService[i].lService,sService[i].lBytes_m,sService[i].lBytes_w);
//    	printf("sqlbuf=%s\n",sqlbuf);
      pasDbExecSql(sqlbuf,0);
    }
    ncSetLastInfo_l("ServiceStat",lTime);
  }
   
  //  本日
  sprintf(sqlbuf,"select sum(ubytes+dbytes) byt,service from ncipservice where sdate='%s' and service!=0 group by service order by byt desc limit 0,200 ",sDated1);
//printf("sqlbuf=%s\n",sqlbuf);
  psCur=pasDbOpenSql(sqlbuf,0);
  if(psCur == NULL) {printf("fff\n");
        return 0;
   }

   iReturn = 0;
   lBytes=0;
   lService=0;
   iNum=0;
   iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                  UT_TYPE_LONG,4,&lService);
   
   while((iReturn==0)||(iReturn==1405))
   {  
   		    sService[iNum].lService=lService;
	        sService[iNum].lBytes_d=lBytes;
	        lBytes=0;
	        lService=0;
	        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                       UT_TYPE_LONG,4,&lService);
	        iNum++;
	  }
    pasDbCloseCursor(psCur);
   
   for(i=0;i<iNum;i++){
    	sprintf(sqlbuf,"insert into ncipservicestat(service,sumflow_m,sumflow_w,sumflow) values(%lu,0,0,%llu)",\
    	sService[i].lService,sService[i].lBytes_d);
 //   	printf("sqlbuf=%s\n",sqlbuf);
   iReturn=pasDbExecSql(sqlbuf,0);
	   if(iReturn!=0){
	   	sprintf(sqlbuf,"update ncipservicestat set sumflow=%llu where service=%lu ",sService[i].lBytes_d,sService[i].lService);
//	printf("sqlbuf=%s\n",sqlbuf);   	
	   	iReturn=pasDbExecSql(sqlbuf,0);
	  }
  
   } 
   
   sprintf(caFileplate,"%s/nc/iframe_mainfirst_service.htm",utComGetVar_sd(psShmHead,"PlatePath","/home/ncmysql/nc/plate"));
// printf("caFileplate=%s\n",caFileplate);
    if(!utFileIsExist(caFileplate)){
    	return 0;
    }
 
 
 
 //输出
 sprintf(sqlbuf,"select service,sumflow+sumflow_m mf,sumflow+sumflow_w,sumflow from ncipservicestat order by mf desc limit 0,200");
 //printf("sqlbuf=%s\n",sqlbuf);
 
  psCur=pasDbOpenSql(sqlbuf,0);
  if(psCur == NULL) {
     return 0;
   }
  psDbHead = utPltInitDb();
  i=0;
  lBytes=0;
	lService=0;
	lBytes_m=0;
	lBytes_w=0;
 iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG,4,&lService,
 																 UT_TYPE_LONG8,8,&lBytes_m,
 																 UT_TYPE_LONG8,8,&lBytes_w,
 																 UT_TYPE_LONG8,8,&lBytes);
   
   while((iReturn==0)||(iReturn==1405))
   { 

  	utPltPutLoopVar(psDbHead,"servicename",i+1,ncUtlGetServiceName(psShmHead,lService,"\0"));
  	utPltPutLoopVarF(psDbHead,"bytes_d",i+1,"%llu",lBytes/1024);
  	utPltPutLoopVarF(psDbHead,"bytes_w",i+1,"%llu",lBytes_w/1024);
  	utPltPutLoopVarF(psDbHead,"bytes_m",i+1,"%llu",lBytes_m/1024);
  	
  lBytes=0;
	lService=0;
	lBytes_m=0;
	lBytes_w=0;
  iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG,4,&lService,
 																 UT_TYPE_LONG8,8,&lBytes_m,
 																 UT_TYPE_LONG8,8,&lBytes_w,
 																 UT_TYPE_LONG8,8,&lBytes);
   i++;
  }
  
  m=0;
 while(i < 22 ) {
        m++;
        i++;
        utPltPutLoopVarF(psDbHead,"snum", m,"%d",i);
    }

   utPltFileToHtmlFile("nc/iframe_mainfirst_service.htm",caPath,psDbHead);
   sprintf(sqlbuf,"chmod 777 %s",caPath);
   system(sqlbuf);
   
    return 0;
}

struct ncCountError_st {
	      char mesg[96];
	      unsigned long lCount_m;
	      unsigned long lCount_w;
	      unsigned long lCount_t;
    };
int ncUtlLookErrByMesg(struct ncCountError_st *arrIn,int iSumArr,char *cod)
{
   int bottom,middle,top;
    int n;
    top = iSumArr - 1;
    bottom = 0;
    while(top >= bottom) {
        middle = (top + bottom) / 2;   
        if(strcmp(arrIn[middle].mesg,cod)==0) {
            return middle;
        }
        else if(strcmp(arrIn[middle].mesg,cod)>0) {
            top = middle -1;
        }
        else {
            bottom = middle + 1;
        }
    }
    return (-1);
}


/* 生成规则触发频度统计页面 */
int ncWebGenErrorHtml(utShmHead *psShmHead)
{
    utPltDbHead *psDbHead;
    char sqlbuf[2048];
    char caName[32];
    int iNum;
    int iReturn;
    char caFileplate[128];
    unsigned long lTime;
    char sDate1[20],sDate2[20];
    unsigned long long lBytes;
    unsigned long lCount,lmCount,lwCount,ltCount;
    char caMesg[100];
    int i,index,m;
    char sDatew1[20],sDatew2[20],sDatew1_t[20],sDatew2_t[20];
    char sDatem1[20],sDatem2[20],sDatem1_t[20],sDatem2_t[20];
    char sDated1[20],sDated2[20],sDated1_t[20],sDated2_t[20];
    unsigned long lStimew,lEtimew,lStimem,lEtimem,lStimed,lEtimed;
    unsigned long lLastTime;
    struct ncCountError_st sError[200];
    char caPath[128];
    pasDbCursor *psCur;
    
    lLastTime = ncGetLastInfo_l("ErrorStat",0);
       
     sprintf(caPath,"%s/html/nc_mainfirst_error.htm",utComGetVar_sd(psShmHead,"NCHOME","/home/ncmysql/nc"));
    lTime=time(0)-3600*12;
    ncUtlWeekDate(lTime,sDatew1,sDatew2);;
 
   ncUtlMonthDate(lTime,sDatem1,sDatem2);
   
   ncUtlDayDate(lTime,sDated1,sDated2);

  
   sprintf(sDatew1_t,"%s 00:00:00",sDatew1);
   sprintf(sDatew2_t,"%s 00:00:00",sDated1);   
   lStimew = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDatew1_t);
   lEtimew = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDatew2_t);
   
   sprintf(sDatem1_t,"%s 00:00:00",sDatem1);
   sprintf(sDatem2_t,"%s 00:00:00",sDated1);   
   lStimem = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDatem1_t);
   lEtimem = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDatem2_t);
   
   sprintf(sDated1_t,"%s 00:00:00",sDated1);
   sprintf(sDated2_t,"%s 23:59:59",sDated2);   

   lStimed = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDated1_t);
   lEtimed = utTimStrToLong("%Y/%m/%d %H:%M:%S",sDated2_t);
 
 
 if(strcmp(sDated1,utTimFormat("%Y/%m/%d",lLastTime))!=0)
 {
 
 //  本月
  sprintf(sqlbuf,"select sum(lcount),mesg from ncerrlog where stime>=%lu and stime<=%lu  group by mesg order by mesg limit 0,200 ",lStimem,lStimed);
//printf("sqlbuf=%s\n",sqlbuf);
  psCur=pasDbOpenSql(sqlbuf,0);
  if(psCur == NULL) {
     return 0;
   }
 
   iReturn = 0;
   lCount=0;
   memset(caMesg,0,sizeof(caMesg));
   iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                  UT_TYPE_STRING,96,caMesg);
   iNum=0;
   while((iReturn==0)||(iReturn==1405))
   {  
         	sError[iNum].lCount_m=lCount;
        	sError[iNum].lCount_w=0;
        	strcpy(sError[iNum].mesg,caMesg);
         
   				lCount=0;
   				memset(caMesg,0,sizeof(caMesg));
   				iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                  UT_TYPE_STRING,96,caMesg);
	         iNum++;
	  
	  }
    pasDbCloseCursor(psCur); 
   
   
    //  本周
  sprintf(sqlbuf,"select sum(lcount),mesg from ncerrlog where stime>=%lu and stime<=%lu  group by mesg order by mesg limit 0,200 ",lStimew,lStimed);
printf("sqlbuf=%s\n",sqlbuf);
  psCur=pasDbOpenSql(sqlbuf,0);
  if(psCur == NULL) {
     return 0;
   }
 
   iReturn = 0;
   lCount=0;
   memset(caMesg,0,sizeof(caMesg));
   iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                  UT_TYPE_STRING,96,caMesg);
 
   while((iReturn==0)||(iReturn==1405))
   {  
   	   	index=ncUtlLookErrByMesg(&sError,iNum,caMesg);
   			if(index!=-1){
          sError[iNum].lCount_w=lCount;     
    		}
                 
   				lCount=0;
   				memset(caMesg,0,sizeof(caMesg));
   				iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                  UT_TYPE_STRING,96,caMesg);
	        
	  
	  }
    pasDbCloseCursor(psCur); 
    
    pasDbExecSql("delete from ncerrorstat",0);
    for(i=0;i<iNum;i++){
    	sprintf(sqlbuf,"insert into ncerrorstat(mesg,mcount,wcount) values('%s',%lu,%lu) ",sError[i].mesg,sError[i].lCount_m,sError[i].lCount_w);
// printf("sqlbuf=%s\n",sqlbuf);   	
    	pasDbExecSql(sqlbuf,0);
    }
    ncSetLastInfo_l("ErrorStat",lTime);
  }
   
  //  本日
  sprintf(sqlbuf,"select sum(lcount) scount,mesg from ncerrlog where stime>=%lu and stime<=%lu  group by mesg order by scount desc limit 0,200 ",lStimed,lEtimed);
printf("sqlbuf=%s\n",sqlbuf);
  psCur=pasDbOpenSql(sqlbuf,0);
  if(psCur == NULL) {
     return 0;
   }
   iNum=0;
   iReturn = 0;
   lCount=0;
   memset(caMesg,0,sizeof(caMesg));
   iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                  UT_TYPE_STRING,96,caMesg);
   iNum=0;
   while((iReturn==0)||(iReturn==1405))
   {  
         	sError[iNum].lCount_t=lCount;
        	strcpy(sError[iNum].mesg,caMesg);
         
   				lCount=0;
   				memset(caMesg,0,sizeof(caMesg));
   				iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                  UT_TYPE_STRING,96,caMesg);
	         iNum++;
	  
	  }
    pasDbCloseCursor(psCur); 
   
   for(i=0;i<iNum;i++){
      sprintf(sqlbuf,"insert into ncerrorstat (mesg,mcount,wcount,tcount) values('%s',0,0,%lu) ",sError[i].mesg,sError[i].lCount_t);
//printf("sqlbuf=%s\n",sqlbuf);
   iReturn=pasDbExecSql(sqlbuf,0);
	   if(iReturn!=0){
	   	sprintf(sqlbuf,"update ncerrorstat set tcount=%lu where mesg='%s' ",sError[i].lCount_t,sError[i].mesg);
//	printf("sqlbuf=%s\n",sqlbuf);   	
	   	iReturn=pasDbExecSql(sqlbuf,0);
	  }
  
   } 
 
 
  sprintf(caFileplate,"%s/nc/iframe_mainfirst_error.htm",utComGetVar_sd(psShmHead,"PlatePath","/home/ncmysql/nc/plate"));

  if(!utFileIsExist(caFileplate)){
    	return 0;
  }
 psDbHead = utPltInitDb();
   //输出
  sprintf(sqlbuf,"select mesg,mcount+tcount mc,wcount+tcount,tcount from ncerrorstat order by mc desc limit 0,200" );
printf("sqlbuf=%s\n",sqlbuf);
  psCur=pasDbOpenSql(sqlbuf,0);
  if(psCur == NULL) {
     return 0;
   }
   i=0;
   iReturn = 0;
   lmCount=0;
   lwCount=0;
   ltCount=0;
   memset(caMesg,0,sizeof(caMesg));
   iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,96,caMesg,
                                  UT_TYPE_LONG,4,&lmCount,
                                  UT_TYPE_LONG,4,&lwCount,
                                  UT_TYPE_LONG,4,&ltCount);
   															
 
   while((iReturn==0)||(iReturn==1405))
   {  
   	      	   
   	 utPltPutLoopVar(psDbHead,"mesg",i+1,caMesg);
   	 utPltPutLoopVarF(psDbHead,"mcount",i+1,"%d",lmCount);
   	 utPltPutLoopVarF(psDbHead,"wcount",i+1,"%d",lwCount);
   	 utPltPutLoopVarF(psDbHead,"tcount",i+1,"%d",ltCount);
     lmCount=0;
	   lwCount=0;
	   ltCount=0;
	   memset(caMesg,0,sizeof(caMesg));
	   iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,96,caMesg,
	                                  UT_TYPE_LONG,4,&lmCount,
	                                  UT_TYPE_LONG,4,&lwCount,
	                                  UT_TYPE_LONG,4,&ltCount);
	  i++;
	  }
    pasDbCloseCursor(psCur); 
   
  
   m=0;
   while(i < 22 ) {
        m++;
        i++;
        utPltPutLoopVarF(psDbHead,"snum", m,"%d",i);
    }

   utPltFileToHtmlFile("nc/iframe_mainfirst_error.htm",caPath,psDbHead);
   sprintf(sqlbuf,"chmod 777 %s",caPath);
   system(sqlbuf);
   
    return 0;
}



/* 显示网站访问类别统计*/
int ncWebShowWebClassFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char sqlbuf[1024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
    long lSumcount;
    char caClass[32];
    long lTid,iNum;
    long long lSum;
    char caDate[20];
    int iReturn,i;
    char caRefresh_flag[8];
   
    
    char preflag[8],nextflag[8],ctime[12];
   lTime=time(0);
 lSum=0;
    utMsgGetSomeNVar(psMsgHead,4,"refresh_flag",UT_TYPE_STRING,1,caRefresh_flag,
                                 "preflag",UT_TYPE_STRING,1,preflag,
                                 "nextflag",UT_TYPE_STRING,1,nextflag,
                                 "ctime",UT_TYPE_STRING,10,ctime);
   if(strlen(ctime)>0){
   	lTime=utTimStrToLong("%Y/%m/%d",ctime);
   	
   }
   if(atol(preflag)==1)
    lTime=lTime-86400;
    
    if(atol(nextflag)==1)
       lTime=lTime+86400;
    
    
  	psDbHead = utPltInitDb();
  	strcpy(caDate,utTimFormat("%Y/%m/%d",lTime));  
  	   
  	utPltPutVar(psDbHead,"today",caDate);  
  	   
  	   
   sprintf(sqlbuf,"select sum(bytes) byt,sum(lcount) ct,tid from nchttpcount where sdate='%s' group by tid order by ct desc limit 0,10",caDate);
//printf("sqlbuf=%s\n",sqlbuf);
   psCur=pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
           return 0;
       }
      
        iReturn = 0;
        lBytes=0;
        lTid=0;
        lSumcount=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&lSumcount,
                                       UT_TYPE_LONG,4,&lTid);                               
	     iNum=0;
        while(iNum<10&&((iReturn==0)||(iReturn==1405)))
        {
         if(lTid>0){
				 strcpy(caClass,ncUrlGetClassNameById (lTid,"")); 

  				if(strlen(caClass)>0){
         utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	lBytes);        
         utPltPutLoopVarF(psDbHead,"tid",  iNum+1,"%d",lTid);
         utPltPutLoopVarF(psDbHead,"count",iNum+1,"%d",lSumcount);
         utPltPutLoopVar(psDbHead,"classname",iNum+1,caClass);
         utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",lTime);
         utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
         iNum++;
					}
				 }
        lBytes=0;
        lTid=0;
        lSumcount=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&lSumcount,
                                       UT_TYPE_LONG,4,&lTid); 
	        
	  
	        }
       pasDbCloseCursor(psCur); 
 
     utPltPutVarF(psDbHead,"sum","%d",iNum);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_webclassfirst.htm");  
    return 0;
}


/* 显示首页网络接口*/
int ncWebShowNetflowFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    
    ncDevFlowInfo *psDev;
	  int iReturn,i,j,iSumDev,iStep;
    int iSumRecord=10;
    int iNum;
    char caDev[64];
    char caPlate[128];
    iReturn = utMsgGetSomeNVar(psMsgHead,1,"plate",      UT_TYPE_STRING,100,caPlate);
  	psDbHead = utPltInitDb(); 
  	
  	iNum=0;
  	     psDev = ncUtlGetThisFlowInfo(psShmHead,iSumRecord,&iSumDev,&iStep);
       if(psDev) {
//      printf(" iSumDev==%d  Step==%d \n",iSumDev,iStep);
       
         for(i=0;i<iSumDev;i++) {
//           printf(" Dev=%s   Sid=%d\n",psDev[i].caName,psDev[i].sid);
         sprintf(caDev,"%s%s",psDev[i].caName,ncLang("0504上行"));
         utPltPutLoopVar(psDbHead,"dev",iNum+1,caDev);
         if(i>2){
           utPltPutLoopVar(psDbHead,"display",iNum+1,"none");
          }
           
           
         iNum++;
         
         sprintf(caDev,"%s%s",psDev[i].caName,ncLang("0677下行"));
         utPltPutLoopVar(psDbHead,"dev",iNum+1,caDev);
         if(i>2){
           utPltPutLoopVar(psDbHead,"display",iNum+1,"none");
          }
         
         
         iNum++;
           
        }
       
         sprintf(caDev,"%s",ncLang("0276合计"));
         utPltPutLoopVar(psDbHead,"dev",iNum+1,caDev);
         utPltPutLoopVar(psDbHead,"checked",iNum+1,"checked");
       
        	ncUtlFreeDevFlowInfo(psDev,iSumDev);
        
        }
  	
  	
  	
  	
  	if(strlen(caPlate)>0){
    	 utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    }
    else{
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/first_netflow.htm");
    }  
    return 0;
}

//显示网络接口速率动态刷新界面
int ncShowNetflowAjax(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    pasDbCursor *psCur;
    char caDate[20];  
    char sqlbuf[2048];
  
    char caName[32];
    int m,n,iNum;
    int mm,nn,temp_nn;
    struct data_s{
    	char caDevname[32];        //设备名    
      char caUp[2048];           //上行速率
      char caDown[2048];         //下行速率
    } *pdev;
    char caDev_desc[1024];
    float fmaxbytes=0;
    float lMaxBytes=0;
    long unit;
    char caUnit[32];
    float totbytes[200];
    char tot_desc[2048];
    memset(caDev_desc,0,sizeof(caDev_desc));
    char caDev[100];
    ncDevFlowInfo *psDev;
	  int iReturn,i,j,iSumDev,iStep;
    int iSumRecord=120;
    
    
    
     psDev = ncUtlGetThisFlowInfo(psShmHead,iSumRecord,&iSumDev,&iStep);
       if(psDev) {
//       printf(" iSumDev==%d  Step==%d iSumRecord=%d \n",iSumDev,iSumRecord,iStep);
        pdev=(struct data_s*)malloc((iSumDev*2+1)*sizeof(struct data_s));
        if(pdev==NULL){
        	ncUtlFreeDevFlowInfo(psDev,iSumDev);
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","内存分配出错","内存分配出错");
        	return 0;
        }

for(i=0;i<200;i++){
	totbytes[i]=0;
}
//获取流量最大值,和各点所有的流量和

        for(i=0;i<iSumDev;i++) {
 //           printf(" Dev=%s   Sid=%d\n",psDev[i].caName,psDev[i].sid);
            for(j=0;j<iSumRecord;j++) {
                         
                    if(psDev[i].psFlow[j].lConTime>0){
                    	
                    	totbytes[j]=totbytes[j]+(float)(psDev[i].psFlow[j].lBytes[0]+psDev[i].psFlow[j].lBytes[1])*8000/psDev[i].psFlow[j].lConTime;
                    	
                    	fmaxbytes=(float)(psDev[i].psFlow[j].lBytes[0]+psDev[i].psFlow[j].lBytes[1])*1000/psDev[i].psFlow[j].lConTime;
                    if(fmaxbytes>lMaxBytes)
                         lMaxBytes=fmaxbytes;
                    }
                    
            }
        }
//        printf("lMaxBytes=%f",lMaxBytes);
			//根据流量最大值决定选择bps,Kbps,Mbps,Gbps
				if(lMaxBytes*8<=1024){
					unit=1;
					strcpy(caUnit,"bps");
				}
				else if(1024<lMaxBytes*8<=1024*1024){
					unit=1024;
					strcpy(caUnit,"Kbps");
				}
				else if(1024*1024<lMaxBytes*8<=1024*1024*1024){
					unit=1024*1024;
					strcpy(caUnit,"Mbps");
				}
				else if(1024*1024*1024<lMaxBytes*8){
					unit=1024*1024*1024;
					strcpy(caUnit,"Gbps");
				}    
				else{
					unit=1;
					strcpy(caUnit,"bps");
				} 
        
         memset(pdev,0,(iSumDev*2+1)*sizeof(struct data_s));
         for(i=0;i<iSumDev;i++) {
//            printf(" Dev=%s   Sid=%d\n",psDev[i].caName,psDev[i].sid);
            
         if(i==0){
    		    sprintf(caDev_desc,"%s%s",psDev[i].caName,ncLang("0504上行"));
    		    sprintf(caDev_desc+strlen(caDev_desc),"|%s%s",psDev[i].caName,ncLang("0677下行"));
      	 }
	    	else{
	    		   sprintf(caDev_desc+strlen(caDev_desc),"|%s%s",psDev[i].caName,ncLang("0504上行"));
	    	     sprintf(caDev_desc+strlen(caDev_desc),"|%s%s",psDev[i].caName,ncLang("0677下行"));
	        } 
	        
	        
	         
          for(j=0;j<iSumRecord;j++) {
          	
            	if(j==0){
            		if(psDev[i].psFlow[iSumRecord-j-1].lConTime>0){
            		   sprintf(pdev[i].caUp,"%0.2f",(float)psDev[i].psFlow[iSumRecord-j-1].lBytes[0]*8000/(unit*psDev[i].psFlow[iSumRecord-j-1].lConTime));
            		   sprintf(pdev[i].caDown,"%0.2f",(float)psDev[i].psFlow[iSumRecord-j-1].lBytes[1]*8000/(unit*psDev[i].psFlow[iSumRecord-j-1].lConTime));
            	  }
            	  else{
            	  	 sprintf(pdev[i].caUp,"0");
            		   sprintf(pdev[i].caDown,"0");
            	  }
            	}
            	else{
            		   if(psDev[i].psFlow[iSumRecord-j-1].lConTime>0){
            		   sprintf(pdev[i].caUp+strlen(pdev[i].caUp),"|%0.2f",(float)psDev[i].psFlow[iSumRecord-j-1].lBytes[0]*8000/(unit*psDev[i].psFlow[iSumRecord-j-1].lConTime));
            		   sprintf(pdev[i].caDown+strlen(pdev[i].caDown),"|%0.2f",(float)psDev[i].psFlow[iSumRecord-j-1].lBytes[1]*8000/(unit*psDev[i].psFlow[iSumRecord-j-1].lConTime));
            	  }
            	  else{
            	  	 sprintf(pdev[i].caUp+strlen(pdev[i].caUp),"|0");
            		   sprintf(pdev[i].caDown+strlen(pdev[i].caDown),"|0");
            	  }
            		
            	}
 /*           	
               printf("[%3d]    %10lu   %6lu    %10llu    %10llu    %5lu   %5lu\n",
                    j+1,
                    psDev[i].psFlow[j].lLongTime,
                    psDev[i].psFlow[j].lConTime,
                    psDev[i].psFlow[j].lBytes[0],
                    psDev[i].psFlow[j].lBytes[1],
                    psDev[i].psFlow[j].lPkg[0],
                    psDev[i].psFlow[j].lPkg[1]);
 */                   
                  
                    
            }
        }
        
        if(iSumDev>0){
	        	sprintf(caDev_desc+strlen(caDev_desc),"|%s",ncLang("0276合计"));
	       }
        //拼凑总流量
        memset(tot_desc,0,sizeof(tot_desc));
        for(j=0;j<iSumRecord;j++){
        	if(j==0){            		
            		   sprintf(tot_desc,"%0.2f",totbytes[iSumRecord-j-1]/unit);
           }
           else{
           	sprintf(tot_desc+strlen(tot_desc),"|%0.2f",totbytes[iSumRecord-j-1]/unit);
          }
        }
        
        
      psDbHead = utPltInitDb();     
        //输出
       for(i=0;i<iSumDev;i++) {
       	  utPltPutLoopVar(psDbHead,"data_flow",i*2+1,pdev[i].caUp);
       	  utPltPutLoopVar(psDbHead,"data_flow",i*2+2,pdev[i].caDown);
       	  utPltPutLoopVarF(psDbHead,"num",i*2+1,"%d",i*2+1);
       	  utPltPutLoopVarF(psDbHead,"num",i*2+2,"%d",i*2+2);
       }
       if(iSumDev>0)
         utPltPutVarF(psDbHead,"sum","%d",iSumDev*2+1);
       else
         utPltPutVarF(psDbHead,"sum","%d",iSumDev*2);
         
         
       utPltPutVar(psDbHead,"devname",caDev_desc);
       utPltPutVar(psDbHead,"unit",caUnit);
        
        if(iSumDev>0){
        	utPltPutLoopVarF(psDbHead,"num",iSumDev*2+1,"%d",iSumDev*2+1);
        	utPltPutLoopVar(psDbHead,"data_flow",iSumDev*2+1,tot_desc);
        }
        
        
        
        
        
        if(pdev) free(pdev);
        ncUtlFreeDevFlowInfo(psDev,iSumDev);
    }
    

  
  
  
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_netflow_ajx.htm");
    

    return 0;
}


