/* 监控软件---报表定制
                     Modified 2009/07/07
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
#include "nclimit.h"
#include "ncsysinfo.h"
#include "ncpkg.h"
#include "ncreport.h" 



//显示自定义报表向导
int ncDispReportWizard(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    pasDbCursor *psCur;
    char caSid[16];

    char caGroupname[32];
    long iNum,iReturn;
    char caTemp[2048];
    char caName[64],caMailreceiver[256];
    long lReporttype,lReporttable,lReportfield,lGroupid,lService,lDateunit,lRecnum,lMailflag;
    utMsgGetSomeNVar(psMsgHead,16,"sid",    UT_TYPE_STRING,10,caSid);	
    
 		                             
   psDbHead = utPltInitDb(); 
 
//输出部门	  
	  
	      sprintf(caTemp,"select groupid,groupname from ncgroup where groupid>0 order by groupname ");
        psCur=pasDbOpenSql(caTemp,0);
        if(psCur == NULL) {
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
                return 0;
        }
        lGroupid=0;
        memset(caGroupname,0,sizeof(caGroupname));
        iNum=0;
              
            iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                           UT_TYPE_STRING,31,caGroupname);
            while((iReturn == 0||iReturn==1405))
            {
              
             iNum++;
             utPltPutLoopVarF(psDbHead,"gid",iNum,"%d",lGroupid);
             utPltPutLoopVar(psDbHead,"gname",iNum,caGroupname);
             lGroupid=0;
             memset(caGroupname,0,sizeof(caGroupname));
             iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                           UT_TYPE_STRING,31,caGroupname);
            }
             pasDbCloseCursor(psCur);
	  
 
	  //输出网络服务大类
long lCgid;

     	psCur = pasDbOpenSqlF("select groupid,name  from ncservicecgroup   order by groupid ");
                    
	    if(psCur == NULL) {
	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
	        return 0;
	    }
     
    lCgid=0;
    memset(caName,0,sizeof(caName));
    iNum=0;
	    iReturn = pasDbFetchInto(psCur,     	                           
	      	                           UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
	  	                             
   	
   	while(iReturn == 0 ||iReturn==1405) {
   		
    utPltPutLoopVarF(psDbHead,"cgid",iNum+1,"%d",lCgid);
    utPltPutLoopVar(psDbHead,"cgname",iNum+1,caName);
   	
   	iNum++;
   		
    lCgid=0;
    memset(caName,0,sizeof(caName));
	    iReturn = pasDbFetchInto(psCur,     	                           
	      	                           UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
        }  
    pasDbCloseCursor(psCur);
	  
	  if(strlen(caSid)>0){
	  	sprintf(caTemp,"select name,reporttype,reporttable,reportfield,groupid,service,dateunit,recnum,mailreceiver,mailflag from ncreportplate where sid=%s ",caSid);
	  	memset(caName,0,sizeof(caName));
	  	lReporttype=0;
	  	lReporttable=0;
	  	lReportfield=0;
	  	lGroupid=0;
	  	lService=0;
	  	lDateunit=0;
	  	lRecnum=0;
	  	memset(caMailreceiver,0,sizeof(caMailreceiver));
	  	lMailflag=0;
	  	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,60,caName,
	  	                        UT_TYPE_LONG,4,&lReporttype,
	  	                        UT_TYPE_LONG,4,&lReporttable,
	  	                        UT_TYPE_LONG,4,&lReportfield,
	  	                        UT_TYPE_LONG,4,&lGroupid,
	  	                        UT_TYPE_LONG,4,&lService,
	  	                        UT_TYPE_LONG,4,&lDateunit,
	  	                        UT_TYPE_LONG,4,&lRecnum,
	  	                        UT_TYPE_STRING,254,caMailreceiver,
	  	                        UT_TYPE_LONG,4,&lMailflag);
	  	utPltPutVarF(psDbHead,"reporttype","%d",lReporttype);
	  	utPltPutVarF(psDbHead,"reporttable","%d",lReporttable);                        
	  	utPltPutVarF(psDbHead,"dateunit","%d",lDateunit);                             
	  	utPltPutVar(psDbHead,"reportname",caName);
	  	utPltPutVar(psDbHead,"mailreceiver",caMailreceiver);
	  	if(lGroupid>0)
	  	   utPltPutVarF(psDbHead,"groupid","%d",lGroupid);
	  	   
	  	if(lService>0)
	  	   utPltPutVarF(psDbHead,"service","%d",lService);
	  	   
	  	utPltPutVarF(psDbHead,"mailflag","%d",lMailflag);
	  	
	  	utPltPutVarF(psDbHead,"recnum","%d",lRecnum);
	  	
	  	if(lReportfield==1){
	  		utPltPutVar(psDbHead,"flowck","checked");
	  	}
	  	else if(lReportfield==2){
	  		utPltPutVar(psDbHead,"countck","checked");
	  	}
	  	else{
	  		utPltPutVar(psDbHead,"flowck","checked");
	  		utPltPutVar(psDbHead,"countck","checked");
	  	}
	  	if(lMailflag==1){
	  		utPltPutVar(psDbHead,"mailflag","checked");
	  	}
	  }
	  else{
	  		utPltPutVar(psDbHead,"flowck","checked");
	  		utPltPutVar(psDbHead,"countck","checked");
	  		utPltPutVar(psDbHead,"recnum","30");
	  		utPltPutVar(psDbHead,"reporttype","0");
	  		utPltPutVarF(psDbHead,"reporttable","2"); 
	  	 utPltPutVarF(psDbHead,"dateunit","1"); 
	  }
	  
	  utPltPutVar(psDbHead,"sid",caSid);
	  
	  
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/selfreportwizard.htm");
    
    return 0;
}


//返回值为分拆的数量，caQue 输入的分拆字符串，caObj,分拆成的字符串两级指针*/
int char_Sep(char *caQue,char c,char caObj[100][128])
{
        int i=0,j=0,z=0,flag=0;
        char caTemp[100][50];
        if(strlen(caQue) == 0)
        	return 0;
        while( (*caQue)!= '\0' )
        {
        		if(flag==0)
        				strcpy(caTemp[i],"");
                if( (*caQue) == c )
                {
                        caQue++;
                        caTemp[i][j] = '\0';
                        i++;
                        j=0;
                        flag=0;
                }
                else
                {
                        caTemp[i][j] = (*caQue);
                        caQue++;
                        j++;
                        flag=1;
                }
                 if(j>49||i>99)
                	break;
    	}
    	caTemp[i][j]='\0';
    	for(z=0;z<i+1;z++)
    	{
        	strcpy(caObj[z],caTemp[z]);
        }
        return i+1;
}




//保存模板
int ncReportWizardSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caSid[16];
    long lReportfield;
    char caUsername[32],caReporttype[16],caReporttable[16],caFlowck[12],caCountck[12],caGroupid[16];
    char caService[16],caDateunit[16],caRecnum[16],caReportname[64],caMailflag[12],caMailreceiver[256];
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif    
    memset(caUsername,0,sizeof(caUsername));
    dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername);
   	iReturn = utMsgGetSomeNVar(psMsgHead,12,
					"sid",   UT_TYPE_STRING, 10, caSid,
					"reporttype",   UT_TYPE_STRING, 8, caReporttype,
					"reporttable",   UT_TYPE_STRING, 8, caReporttable,
					"flowck",  UT_TYPE_STRING, 8, caFlowck,
					"countck",   UT_TYPE_STRING, 8, caCountck,
					"groupid",  UT_TYPE_STRING, 10, caGroupid,
					"service", UT_TYPE_STRING,10,caService,
					"dateunit",    UT_TYPE_STRING, 10, caDateunit,
					"recnum", UT_TYPE_STRING, 10, caRecnum,
					"reportname",  UT_TYPE_STRING, 60, caReportname,
					"mailflag",  UT_TYPE_STRING, 8, caMailflag,
					"mailreceiver",    UT_TYPE_STRING,255,caMailreceiver);
			
			
			    if(atol(caFlowck)==1&&atol(caCountck)==1){
			    	lReportfield=3;
			    }
			    else if(atol(caFlowck)==1&&atol(caCountck)==0){
			    	lReportfield=1;
			    }
			    else if(atol(caFlowck)==0&&atol(caCountck)==1){
			    	lReportfield=2;
			    }
			    else{
			    	lReportfield=3;
			    }
         if(strlen(caSid)==0){
             iReturn = pasDbExecSqlF("insert into ncreportplate(name,reporttype,reporttable,reportfield,groupid,service,dateunit,recnum,mailreceiver,mailflag,addby,stime)\
              values ('%s',%d,%lu,%lu,%d,%d,%d,%d,'%s',%d,'%s',%d)",
                           caReportname,atol(caReporttype),atol(caReporttable),lReportfield,atol(caGroupid),atol(caService),atol(caDateunit),atol(caRecnum),caMailreceiver,atol(caMailflag),caUsername,time(0));
             if(iReturn != 0) {
                  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0608网络设置"),ncLang("0310记录增加出错"));
                  pasDbRollback(NULL);
                  return 0;
             }
             else {
                  pasDbCommit(NULL);
                 
             }
         }
         else {
             iReturn=pasDbExecSqlF("update ncreportplate set name='%s',reporttype=%d,reporttable=%d,reportfield=%d,groupid=%d,service=%d,dateunit=%d,recnum=%d,mailreceiver='%s',mailflag=%d where sid=%s ",
             caReportname,atol(caReporttype),atol(caReporttable),lReportfield,atol(caGroupid),atol(caService),atol(caDateunit),atol(caRecnum),caMailreceiver,atol(caMailflag),caSid);
             if(iReturn!=0){
                  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0608网络设置"),ncLang("0310记录增加出错"));
                  pasDbRollback(NULL);
                  return 0;
             }
             else {
                  pasDbCommit(NULL);
                 
             }
             
             
         }


    ncReportPlateList(psShmHead, iFd,psMsgHead);
    return 0;
}


/* 报表模板列表  */
int ncReportPlateList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    utPltDbHead *psDbHead;
   	int iReturn,iNum,i;
   	char caTemp[1024];
   long lSid;
   long lReporttype,lReporttable,lReportfield,lGroupid,lService,lDateunit,lStime,lMailflag,lReportflag;
   char caAddby[32],caName[64],caMailreceiver[256];
   char caDel[16],caStop[16],caStart[16];
   pasDbCursor *psCur;
   	char *p;

#ifdef LDEBUG
  
    utMsgPrintMsg(psMsgHead);
#endif
    	iReturn = utMsgGetSomeNVar(psMsgHead,3,
					"del",   UT_TYPE_STRING, 10, caDel,
					"stop",  UT_TYPE_STRING, 10, caStop,
					"start", UT_TYPE_STRING, 10, caStart);

  if(!utStrIsSpaces(caDel)){
  	for(i=0;i<500;i++){
  		sprintf(caTemp,"chk%d",i+1);
  		p=utMsgGetVar_s(psMsgHead,caTemp);
  		if(p){
  			iReturn=pasDbExecSqlF("delete from ncreportplate where sid=%s ",p);
  		}
    }
  }
  
   if(!utStrIsSpaces(caStop)){
  	for(i=0;i<500;i++){
  		sprintf(caTemp,"chk%d",i+1);
  		p=utMsgGetVar_s(psMsgHead,caTemp);
  		if(p){
  			iReturn=pasDbExecSqlF("update ncreportplate set reportflag=1 where sid=%s ",p);
  		}
    }
  }
  
     if(!utStrIsSpaces(caStart)){
  	for(i=0;i<500;i++){
  		sprintf(caTemp,"chk%d",i+1);
  		p=utMsgGetVar_s(psMsgHead,caTemp);
  		if(p){
  			iReturn=pasDbExecSqlF("update ncreportplate set reportflag=0 where sid=%s ",p);
  		}
    }
  }
  
      psCur = pasDbOpenSqlF("select sid,name,reporttype,reporttable,reportfield,groupid,service,dateunit,addby,stime,mailflag,reportflag,mailreceiver from ncreportplate  order by stime desc");
  
    	if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0549数据库出错"));
            return 0;
        }
        lSid=0;
        memset(caName,0,sizeof(caName));
        lReporttype=0;
        lReporttable=0;
        lReportfield=0;
        lGroupid=0;
        lService=0;
        lDateunit=0;
        memset(caAddby,0,sizeof(caAddby));
        lStime=0;
        lMailflag=0;
        lReportflag=0;
        memset(caMailreceiver,0,sizeof(caMailreceiver));
    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
    	                               UT_TYPE_STRING,60,caName,
    	                               UT_TYPE_LONG,4,&lReporttype,
    	                               UT_TYPE_LONG,4,&lReporttable,
    	                               UT_TYPE_LONG,4,&lReportfield,
    	                               UT_TYPE_LONG,4,&lGroupid,
    	                               UT_TYPE_LONG,4,&lService,
    	                               UT_TYPE_LONG,4,&lDateunit,
    	                               UT_TYPE_STRING,30,caAddby,
    	                               UT_TYPE_LONG,4,&lStime,
    	                               UT_TYPE_LONG,4,&lMailflag,
    	                               UT_TYPE_LONG,4,&lReportflag,
    	                               UT_TYPE_STRING,255,caMailreceiver);
    	                              
    	iNum = 0;                                   
    	psDbHead = utPltInitDb();

    	while(iReturn == 0 && iNum < 500) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum);
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%d",lSid);
            utPltPutLoopVar(psDbHead,"name",iNum,caName);
            if(lReporttype==0){
            	utPltPutLoopVar(psDbHead,"rpeorttype",iNum,"排行报表");
            }
            if(lReporttable==2){
            	utPltPutLoopVar(psDbHead,"reporttable",iNum,"网站访问排行");           	
            }
            else if(lReporttable==1){
            	utPltPutLoopVar(psDbHead,"reporttable",iNum,"使用服务目标IP排行");
            }
            if(lDateunit==1){
            	utPltPutLoopVar(psDbHead,"dateunit",iNum,"每日统计");
            }
            else if(lDateunit==2){
            	utPltPutLoopVar(psDbHead,"dateunit",iNum,"每周统计");
            }
            else if(lDateunit==3){
            	utPltPutLoopVar(psDbHead,"dateunit",iNum,"每月统计");
            }
            if(lGroupid>0){
            	utPltPutLoopVar(psDbHead,"group",iNum,ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
            }
            if(lService>0){
            	utPltPutLoopVar(psDbHead,"service",iNum,ncUtlGetServiceName(psShmHead,lService,"\0"));
            }
            utPltPutLoopVar(psDbHead,"addby",iNum,caAddby);
            utPltPutLoopVar(psDbHead,"mailreceiver",iNum,caMailreceiver);
            if(lMailflag==1)
              utPltPutLoopVar(psDbHead,"mailflag",iNum,"邮件订阅");
              
            if(lReportflag==1)
              utPltPutLoopVar(psDbHead,"reportflag",iNum,"<font color=red>停用</font>");
            else
              utPltPutLoopVar(psDbHead,"reportflag",iNum,"<font color=green>启用</font>");
            
          
            
            
        lSid=0;
        memset(caName,0,sizeof(caName));
        lReporttype=0;
        lReporttable=0;
        lReportfield=0;
        lGroupid=0;
        lService=0;
        lDateunit=0;
        memset(caAddby,0,sizeof(caAddby));
        lStime=0;
        lMailflag=0;
        lReportflag=0;
        memset(caMailreceiver,0,sizeof(caMailreceiver));
    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
    	                               UT_TYPE_STRING,60,caName,
    	                               UT_TYPE_LONG,4,&lReporttype,
    	                               UT_TYPE_LONG,4,&lReporttable,
    	                               UT_TYPE_LONG,4,&lReportfield,
    	                               UT_TYPE_LONG,4,&lGroupid,
    	                               UT_TYPE_LONG,4,&lService,
    	                               UT_TYPE_LONG,4,&lDateunit,
    	                               UT_TYPE_STRING,30,caAddby,
    	                               UT_TYPE_LONG,4,&lStime,
    	                               UT_TYPE_LONG,4,&lMailflag,
    	                               UT_TYPE_LONG,4,&lReportflag,
    	                               UT_TYPE_STRING,255,caMailreceiver);
       }
     

    pasDbCloseCursor(psCur);                                  
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncreportplate_list.htm");
    return 0;
}




//显示报表历史记录
int ncReportHis(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caTemp[1024];
    pasDbCursor *psCur;
    char caTotPg[16],caCurPg[16],caCx[12],caDel[12],caDelall[12];
    int  i,iReturn,iNum;
    char caAction[6];
    char caResult[16];
    char caSdate[20],caEdate[20];
    char caDesc_temp[300];
    char caSdate_all[24],caEdate_all[24];
    long lStime,lEtime;
    char caExport[16];
    char caWhere[256],caIp[16],caDesc[256],caName[16],caSear[64];
    long lCurPg,lRowNum,lStartRec,lTotPg,lCount;
    unsigned long lSid;
    utPltDbHead *psDbHead;
    char caDescr[256];
     char caUsername2[32];
   memset(caUsername2,0,sizeof(caUsername2));
   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
    
 	utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,7,
   	                "sear",       UT_TYPE_STRING,60,caSear,
   	                "cx",         UT_TYPE_STRING, 11, caCx,
   	                "del",        UT_TYPE_STRING, 11, caDel,   	   
										"CurPg",      UT_TYPE_STRING, 10,  caCurPg,
										"TotPg",      UT_TYPE_STRING, 10,  caTotPg,
										"sdate",      UT_TYPE_STRING,10,caSdate,
										"edate",      UT_TYPE_STRING,10,caEdate);
			
					


    lCurPg = atol(caCurPg);
    if(lCurPg == 0) {
        lCurPg = 1;
    }
   
    lRowNum = 32;
 
    lStartRec = (lCurPg - 1) * lRowNum;
    if(lStartRec <= 0) lStartRec=0;
    /* 删除  */
    if(!utStrIsSpaces(caDel)) { /* 删除选择记录  */
        char caVar[12];
        char *p;
        for(i=0;i<lRowNum;i++) {
            sprintf(caVar,"ch%d",i+1);
            p = utMsgGetVar_s(psMsgHead,caVar);
            if(p) {
                 iReturn = pasDbExecSqlF("delete from ncreportplatelog where sid = %s",p);
                 if(iReturn==0){
                 	pasDbExecSqlF("delete from ncreportdet where tid=%s ",p);
                }
            }
        }
        pasDbCommit(NULL);
    }
     


    

 
    sprintf(caTemp,"select count(*) from ncreportplatelog where 1=1 ");
    if(strlen(caSear)>0){
    	sprintf(caTemp+strlen(caTemp)," and name like '%c%s%c' ",'%',caSear,'%' );
    }
    if(strlen(caSdate)>0){
    	sprintf(caSdate_all,"%s 00:00:00",caSdate);
    	lStime=utTimStrToLong("%Y/%m/%d %H:%M:%S",caSdate_all);
    	sprintf(caTemp+strlen(caTemp)," and stime>=%d ",lStime);
    }
    if(strlen(caEdate)>0){
    	sprintf(caEdate_all,"%s 23:59:59",caEdate);
    	lEtime=utTimStrToLong("%Y/%m/%d %H:%M:%S",caEdate_all);
    	sprintf(caTemp+strlen(caTemp)," and stime<=%d ",lEtime);
    }

    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0053IP查询"),ncLang("0551数据库出错%s"),pasDbErrorMsg(NULL));
        return 0;
    }
    lTotPg = (lCount -1 ) / lRowNum + 1;

    sprintf(caTemp,"select sid,name,stime,descr from ncreportplatelog where 1=1 ");
        if(strlen(caSear)>0){
    	sprintf(caTemp+strlen(caTemp)," and name like '%c%s%c' ",'%',caSear,'%' );
    }
    if(strlen(caSdate)>0){
    	sprintf(caSdate_all,"%s 00:00:00",caSdate);
    	lStime=utTimStrToLong("%Y/%m/%d %H:%M:%S",caSdate_all);
    	sprintf(caTemp+strlen(caTemp)," and stime>=%d ",lStime);
    }
    if(strlen(caEdate)>0){
    	sprintf(caEdate_all,"%s 23:59:59",caEdate);
    	lEtime=utTimStrToLong("%Y/%m/%d %H:%M:%S",caEdate_all);
    	sprintf(caTemp+strlen(caTemp)," and stime<=%d ",lEtime);
    }
    sprintf(caTemp+strlen(caTemp)," order by stime desc limit %lu,%lu",lStartRec,lRowNum);
    psCur = pasDbOpenSql(caTemp,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }

    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
  
    lSid=0;
    memset(caName,0,sizeof(caName));
    lStime=0;
    memset(caDescr,0,sizeof(caDescr));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                   UT_TYPE_STRING,60,caName,
                                   UT_TYPE_LONG,4,&lStime,
                                   UT_TYPE_STRING,200,caDescr);
                                  
    iNum = 0;                                   
    while((iReturn == 0 ||iReturn==1405)&& iNum < lRowNum) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"num", iNum,"%lu",iNum);
        utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
        utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
        utPltPutLoopVar(psDbHead,"name",  iNum,caName);
        utPltPutLoopVar(psDbHead,"descr",iNum,caDescr);
        
     lSid=0;
    memset(caName,0,sizeof(caName));
    lStime=0;
    memset(caDescr,0,sizeof(caDescr));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                   UT_TYPE_STRING,60,caName,
                                   UT_TYPE_LONG,4,&lStime,
                                   UT_TYPE_STRING,200,caDescr);
    }
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
   
    if(!utStrIsSpaces(caSear)) {
        utPltPutVar(psDbHead,"sear", caSear);
    }
    pasDbCloseCursor(psCur);
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_report_hislog.htm");
    return 0;
}



/* 统计更多的网站目标IP*/
int ncShowMoreWebSiteIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  pasDbCursor *psCur;
    char sqlbuf[1024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
//    char serv_name[32];
    long iNum;
    long long lSum;

    char caTemp[1024];
    int iReturn,i;
    char caOrder[16];
    long lGroupid0;
    char caExport[12];
    char caSum[12];
    char caSdate[16],caEdate[16],caShour[8],caEhour[8],caGroupid[16],caGitem[12];
    char caSdate_temp[20],caEdate_temp[20],caUnit[16];
    long lStime,lEtime,lConntime;
    long lSumShow;
//    char caRefresh_flag[8];
    struct ncFlowSort_s {
        unsigned long long lBytes;
        unsigned long long l_Count;       
        char caHost[64];
        char dip[16];
    };
    struct ncFlowSort_s *pFlow;
    ncUserCont *psUser;
    long long lSumbytes;
    unsigned long long l_Count;
    long lSumcount;
    long lGitem;
    long lGroupid;
    long m;

    char caServ_flag[12];
    char caName[32],caHost[65];
    char caPdf[32];

    unsigned long lIp;
    char caArea[64],caName2[64];
    int rowrow;                     
    int column; 

    char caTitle[128];
    char filename[128];
    column=4;
    char **desc;
    char caStatdate[128];
    char caUsername[32],caGroupname[32];
    long Webid;
    char caIp[16];
    char caExpsql[32];
    char caStattype[32];
   lTime=time(0);
   lSum=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
//    printf("== lGroupid0=%lu\n",lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
   
   
      iReturn = utMsgGetSomeNVar(psMsgHead,7,"order",  UT_TYPE_STRING,12,caOrder,
																							"exp",    UT_TYPE_STRING,6, caExport,
																							"sunrec", UT_TYPE_STRING,8, caSum,
																							"sdate",  UT_TYPE_STRING,11,caSdate,
																							"edate",  UT_TYPE_STRING,11,caEdate,
																							"expsql", UT_TYPE_STRING,10,caExpsql,
																							"stattype",UT_TYPE_STRING,8,caStattype);
																						
																					
	if(strlen(caOrder)==0) strcpy(caOrder,"C");				
   if(strlen(caSum)==0) strcpy(caSum,"100");
   lSumShow=atol(caSum);
   
 if((strlen(caSdate)==0)||strlen(caEdate)==0){
	lTime=time(0);
//	ncUtlWeekDate(lTime,caSdate,caEdate);
  strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));
	strcpy(caEdate,caSdate);
	sprintf(caSdate_temp,"%s 00:00",caSdate);
	sprintf(caEdate_temp,"%s 23:59",caEdate);
	lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_temp);
	lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_temp);
}
else{
	  sprintf(caSdate_temp,"%s 00:00",caSdate);
	  sprintf(caEdate_temp,"%s 23:59",caEdate);
	  lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_temp);
	  lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_temp);
	  
	  
	}
       



  		  sprintf(sqlbuf,"select sum(b.bytes)/1024 abyte,sum(b.lcount) acount,b.host,inet_ntoa(b.sid) aa from nchttpcount b \
        	 where 1=1  ");
  //    	if(strcmp(caSdate,caEdate)!=0)
            sprintf(sqlbuf+strlen(sqlbuf)," and (b.sdate >= '%s' and b.sdate <= '%s') ",caSdate,caEdate);
     
       
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by b.host,sid  ");
  		  
        if  (strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {
        		strcpy(caOrder,ncLang("0369流量"));
        		sprintf(sqlbuf+strlen(sqlbuf)," order by abyte desc ");
         		strcpy(caUnit,"MB");
				}else{
						strcpy(caOrder,ncLang("0180次数"));
						sprintf(sqlbuf+strlen(sqlbuf)," order by acount desc ");
	          strcpy(caUnit,ncLang("1898次"));
				}
        sprintf(sqlbuf+strlen(sqlbuf)," limit 0,%lu ",lSumShow);
  		  
//  printf("sqlbuf=%s\n",sqlbuf);
  
  if(atol(caStattype)==1){
  	char sql2[2048];
  	sprintf(sql2,"002;%s",sqlbuf);
		utComSetVar_s(psShmHead,"reportsql",sql2);
		
		utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","进入后台统计，统计结果请在<历史报表查看>中查阅!","进入后台统计，统计结果请在<历史报表查看>中查阅!");
    return 0;
		
		
	}			
	else{
		utComSetVar_s(psShmHead,"reportsql","");
	}	
  
  
  
  
    	psDbHead = utPltInitDb();
 
   
    utPltPutVar(psDbHead,"sunrec",caSum);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"order",caOrder);  
    utPltPutVar(psDbHead,"order",caOrder);


   pFlow=(struct ncFlowSort_s *)malloc(lSumShow*sizeof(struct ncFlowSort_s));
   if(pFlow==NULL){
   	    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0404内存分配"),ncLang("0404内存分配"));
        return 0;
   }
  

  	
    psCur=pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
      
        iReturn = 0;
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp);
                                                                  
	     iNum=0;
	     lSumbytes=0;
	     lSumcount=0;
	     while(iReturn==0||iReturn==1405){
	     
	     	pFlow[iNum].lBytes=lBytes;
	     	pFlow[iNum].l_Count=l_Count;
	     
	     	strcpy(pFlow[iNum].caHost,caHost);
	      strcpy(pFlow[iNum].dip,caIp);
	     	lSumbytes=lSumbytes+lBytes;
	     	lSumcount=lSumcount+l_Count;
	     	iNum++;
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp); 
	    }
	     pasDbCloseCursor(psCur); 
	
	if(strlen(caExport)>0){                                      //输出到EXEC中
		    FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"ncsort%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613网站访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,ncLang("0003%-2s网站访问%s排行榜(%s--%s)\n"),"\0",caOrder,caSdate_temp,caEdate_temp);
       
	 fprintf(fp,"序号,网址,目标IP,IP位置,流量,次数\n");
	 for(i=0;i<iNum;i++){	 
	 	     lIp=ntohl(pasIpcvtLong(pFlow[i].dip)); 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iReturn == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar(psDbHead,"dip_location",i+1,caTemp);
          }
          else{
          	strcpy(caTemp,"");
          }
	 	
	 			
         	 fprintf(fp,"%d,\"%s\",\"%s\",\"%s\",%0.1f,%llu\n",i+1,pFlow[i].caHost,pFlow[i].dip,caTemp,(float)pFlow[i].lBytes/1024,pFlow[i].l_Count);
 	 }

        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
         free(pFlow);
        utPltFreeDb(psDbHead);        
		    return 0;
}

//保存查询结果
if(strlen(caExpsql)>0){
	char caDesc[256];
	char caTdesc[240];
	long lSid;
	lSid=pasGetSid("ncreportplatelog","sid");

	sprintf(caTemp,"统计时间%s-%s",caSdate,caEdate);
	strcpy(caTdesc,utTimFormat("%Y%m%d%H%M%S",time(0)));
	sprintf(caDesc,"网址请求数前N排行(%s)",caTdesc);
	iReturn=pasDbExecSqlF("insert into ncreportplatelog(sid,name,descr,stime,pid) values(%d,'%s','%s',%d,2)",lSid,caDesc,caTemp,time(0));
	if(iReturn==0){
		for(i=0;i<iNum;i++){
			pasDbExecSqlF("insert into ncreportdet(tid,webname,ip,bytes,lcount) values(%d,'%s','%s',%llu,%d)",\
			lSid,pFlow[i].caHost,pFlow[i].dip,pFlow[i].lBytes,pFlow[i].l_Count);
		}
	}
}

	   
//输出到界面	   
	   for(i=0;i<iNum;i++){
        
         utPltPutLoopVar(psDbHead,"host",  i+1,pFlow[i].caHost);	
         utPltPutLoopVar(psDbHead,"dip", i+1,pFlow[i].dip);		
         lIp=ntohl(pasIpcvtLong(pFlow[i].dip)); 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iReturn == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar(psDbHead,"dip_location",i+1,caTemp);
          }

         																																	//按次数排序     
				 utPltPutLoopVarF(psDbHead,"count",i+1,"%llu",pFlow[i].l_Count);				      
         utPltPutLoopVarF(psDbHead,"num",i+1,"%d",i);        
         utPltPutLoopVarF(psDbHead,"dbytes",i+1,"%0.1f",(float)pFlow[i].lBytes/1024);
         utPltPutLoopVar(psDbHead,"sdate",  i+1,caSdate);
         utPltPutLoopVar(psDbHead,"edate",i+1,caEdate);
         
                
      }
     
     free(pFlow);
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_websitipemore.htm"); 

}




/* 显示历史统计结果*/
int ncShowHisReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  pasDbCursor *psCur;
    char sqlbuf[1024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
//    char serv_name[32];
    long iNum;
    long long lSum;

    char caTemp[1024];
    int iReturn,i;
    char caOrder[16];
    long lGroupid0;
    char caExport[12];
    char caSum[12];
    char caSdate[16],caEdate[16],caShour[8],caEhour[8],caGroupid[16],caGitem[12];
    char caSdate_temp[20],caEdate_temp[20],caUnit[16];
    long lStime,lEtime,lConntime;
    long lSumShow;
    long long lUbytes,lDbytes;
//    char caRefresh_flag[8];
   
    ncUserCont *psUser;
    long long lSumbytes;
    unsigned long  l_Count;
    long lSumcount;
    long lGitem;
    long lGroupid;
    long m;

    char caServ_flag[12];
    char caName[32],caHost[65];
    char caPdf[32];

    
    int rowrow;                     
    int column; 

    char caTitle[128];
    char filename[128];
    column=4;
    char **desc;
    char caStatdate[128];
    char caUsername[32],caGroupname[32];
    long Webid;
    char caIp[16];
    char caExpsql[32];
    char caSid[16];
    char caDescr[256];
    long iRet;
    unsigned long lIp;
    char caArea[64],caName2[64];
    long lRow;
    char caWebsite[64],caIplocal[64];
    long lPid=0;
   lTime=time(0);
   lSum=0;

   
      iReturn = utMsgGetSomeNVar(psMsgHead,4,"sid",  UT_TYPE_STRING,12,caSid,
																							"exp",    UT_TYPE_STRING,6, caExport,
																							"website",UT_TYPE_STRING,63,caWebsite,
																							"localip",UT_TYPE_STRING,63,caIplocal);
				if(strlen(caExport)>0){
					lRow=100000;
				}
				else{
					lRow=10000;
				}																			
		memset(caDescr,0,sizeof(caDescr));																			
	   sprintf(caTemp,"select pid,descr from ncreportplatelog where sid=%s ",caSid);
	   pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lPid,
	                           UT_TYPE_STRING,250,caDescr);

  sprintf(sqlbuf,"select bytes,lcount,webname,ip,ubytes,dbytes from ncreportdet where tid=%d ",atol(caSid));
  
  if(strlen(caWebsite)>0){
  	sprintf(sqlbuf+strlen(sqlbuf)," and webname like '%c%s%c' ",'%',caWebsite,'%');
  }
   if(lPid==2){
    sprintf(sqlbuf+strlen(sqlbuf)," order by lcount desc ");
  }
  else {
  	sprintf(sqlbuf+strlen(sqlbuf)," order by bytes desc ",atol(caSid));
  }
  sprintf(sqlbuf+strlen(sqlbuf)," limit 0,%d",lRow);
  printf("sqlbuf=%s\n",sqlbuf);
  	
    psCur=pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
      
        iReturn = 0;
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        lUbytes=0;
        lDbytes=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG8,8,&lUbytes,
                                       UT_TYPE_LONG8,8,&lDbytes);
                                                                  
	     iNum=0;
	     lSumbytes=0;
	     lSumcount=0;
	     
	    if(strlen(caExport)==0){ 
	     	psDbHead = utPltInitDb();
	     	utPltPutVar(psDbHead,"sid",caSid);
	     	utPltPutVar(psDbHead,"desc",caDescr);
	     while(iReturn==0||iReturn==1405){
	     
	     if(strlen(caIplocal)>0){
	     	 lIp=ntohl(pasIpcvtLong(caIp)); 
         iRet = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iRet == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
          	if(strstr(caTemp,caIplocal)){
            utPltPutLoopVar(psDbHead,"dip_location",iNum+1,caTemp);
            
             utPltPutLoopVar(psDbHead,"host",  iNum+1,caHost);	
		         utPltPutLoopVar(psDbHead,"dip", iNum+1,caIp);																																						//按次数排序     
						 utPltPutLoopVarF(psDbHead,"count",iNum+1,"%lu",l_Count);				      
		         utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum+1);        
		         utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%0.1f",(float)lBytes/1024);
		         utPltPutLoopVarF(psDbHead,"ubytes",iNum+1,"%0.1f",(float)lUbytes/1024);
		         utPltPutLoopVarF(psDbHead,"dbytes",iNum+1,"%0.1f",(float)lDbytes/1024);
   

   
   
   
   
	     	iNum++;
            
            
             }
            
            
          }
	     	
	     	
	     	
	    }
	    else{
	       lIp=ntohl(pasIpcvtLong(caIp)); 
         iRet = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iRet == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar(psDbHead,"dip_location",iNum+1,caTemp);
          }
          else{
          	strcpy(caTemp,"");
          }
	        
	     
	     
         utPltPutLoopVar(psDbHead,"host",  iNum+1,caHost);	
         utPltPutLoopVar(psDbHead,"dip", iNum+1,caIp);																																						//按次数排序     
				 utPltPutLoopVarF(psDbHead,"count",iNum+1,"%lu",l_Count);				      
         utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum+1);        
         utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%0.1f",(float)lBytes/1024);
         utPltPutLoopVarF(psDbHead,"ubytes",iNum+1,"%0.1f",(float)lUbytes/1024);
         utPltPutLoopVarF(psDbHead,"dbytes",iNum+1,"%0.1f",(float)lDbytes/1024);
   

   
   
   
   
	     	iNum++;
	     }
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        lUbytes=0;
        lDbytes=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG8,8,&lUbytes,
                                       UT_TYPE_LONG8,8,&lDbytes);
	    }
	     pasDbCloseCursor(psCur); 
	    }
	    else{
      //输出到EXEC中
		    FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"ncsort%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613网站访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        
       if(lPid==2){ 
        
        fprintf(fp,"网站访问排行榜\n");      
	      fprintf(fp,"序号,网址,目标IP,流量,次数\n");
	      }
	      else if(lPid==1){
	      	    fprintf(fp,"%s\n",caDescr);    
	            fprintf(fp,"序号,目标IP,Ip位置,总流量(MB),上行流量(MB),下行流量(MB)\n");
	      }
	     while(iReturn==0||iReturn==1405){
	     
	      lIp=ntohl(pasIpcvtLong(caIp)); 
         iRet = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iRet == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
          
          }
          else{
          	strcpy(caTemp,"");
          }
	     
	     
	     
        iNum++;
        if(lPid==2){
	      fprintf(fp,"%d,\"%s\",\"%s\",\"%s\",%0.1f,%lu\n",iNum,caHost,caIp,caTemp,(float)lBytes/1024,l_Count);
	      }
	      else if(lPid==1){
	      	 fprintf(fp,"%d,\"%s\",\"%s\",%0.1f,%0.1f,%0.1f\n",iNum,caIp,caTemp,(float)lBytes/1024,(float)lUbytes/1024,(float)lDbytes/1024);
	      }
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        lUbytes=0;
        lDbytes=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG8,8,&lUbytes,
                                       UT_TYPE_LONG8,8,&lDbytes);
	    }
	     pasDbCloseCursor(psCur);


        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
     
		    return 0;
   }
   
   //输出到界面	   
	if(lPid==2){
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_reporthis.htm"); 
	}
	else if(lPid==1){
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_reporthis_serdip.htm"); 
	}

}




//显示对比报表定义界面
int ncWebDispCmpReportDef(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    unsigned long lTime;
    utPltDbHead *psDbHead;
    char caFlag[12];
    iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"flag",    UT_TYPE_STRING, 2,  caFlag);
					
    
    psDbHead = utPltInitDb();
 
  if(atol(caFlag)==1){
  	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_report_cmpdef2.htm");
  }
  else{
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_report_cmpdef.htm");
  }
    return 0;
} 


//显示历史报表列表，供对比报表选择

int ncReportPlateList_comp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    utPltDbHead *psDbHead;
   	int iReturn,iNum,iN,i;
   	char caTemp[1024];
   long lSid;
   long lReporttype,lReporttable,lReportfield,lGroupid,lService,lDateunit,lStime,lMailflag,lReportflag;
   char caAddby[32],caName[64],caReporttype[16],caDescr[256];
   char caDel[16],caStop[16],caStart[16];
   pasDbCursor *psCur;
   	char *p;

#ifdef LDEBUG
  
    utMsgPrintMsg(psMsgHead);
#endif
    	iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"reporttype",   UT_TYPE_STRING, 10, caReporttype);

  psDbHead = utPltInitDb();
  if(strlen(caReporttype)>0){
  
//  printf("select sid,name,descr from ncreportplatelog where pid=%s order by stime desc limit 0,50\n",caReporttype);
      psCur = pasDbOpenSqlF("select sid,name,descr from ncreportplatelog where pid=%s order by stime desc limit 0,50",caReporttype);
  
    	if(psCur == NULL) {
            return 0;
        }
        lSid=0;
        memset(caName,0,sizeof(caName));
        memset(caDescr,0,sizeof(caDescr));

    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
    	                               UT_TYPE_STRING,60,caName,
    	                               UT_TYPE_STRING,250,caDescr);
    	                              
    	                              
    	iNum = 0; 
    	iN=0;                                  
    	

    	while(iReturn == 0 && iNum < 50) {
          iNum++;
            if(iNum%2==1){iN++;
            utPltPutLoopVarF(psDbHead,"numm",iN,"%d",iNum);
            utPltPutLoopVarF(psDbHead,"sidd", iN,"%d",lSid);
            utPltPutLoopVar(psDbHead,"namem",iN,caName);
            utPltPutLoopVar(psDbHead,"descc",iN,caDescr);
           }
           else{
            utPltPutLoopVarF(psDbHead,"num",iN,"%d",iNum);
            utPltPutLoopVarF(psDbHead,"sid", iN,"%d",lSid);
            utPltPutLoopVar(psDbHead,"name",iN,caName);
            utPltPutLoopVar(psDbHead,"desc",iN,caDescr);
          }
            
            
        lSid=0;
        memset(caName,0,sizeof(caName));
        memset(caDescr,0,sizeof(caDescr));

    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
    	                               UT_TYPE_STRING,60,caName,
    	                               UT_TYPE_STRING,250,caDescr);
       }
     utPltPutVarF(psDbHead,"sum","%d",iNum);

    pasDbCloseCursor(psCur);  
  }                                
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncreporlog_comp.htm");
    return 0;
}

typedef struct ncFlowIp_s {
    	  unsigned long lMd5;
        char Ip[16];
        char host[100];
        unsigned long long lBytes[50];
        unsigned long long lUbytes[50];
        unsigned long long lDbytes[50];
        unsigned long lCount[50];       
    } ncFlowIp;
static long iOrderflow;
static int ncUtlSortOnflow(struct ncFlowIp_s *p1,struct ncFlowIp_s *p2)
{
        long long l;
        if(iOrderflow==1){
	        l=p2->lBytes[0]-p1->lBytes[0];
	        if(l>0) return 1;
	        else if(l<0) return (-1);
	        else return 0;
        }
        else {
        return (p2->lCount[0]-p1->lCount[0]);
	       
        }
       
}


/* 显示对比报表*/
int ncShowCompReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  pasDbCursor *psCur;
    char sqlbuf[1024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
//    char serv_name[32];
    long iNum;
    long long lSum;

    char caTemp[1024];
    int iReturn,i;
    char caOrder[16];
    long lGroupid0;
    char caExport[12];
    char caSum[12];
    char caSdate[16],caEdate[16],caShour[8],caEhour[8],caGroupid[16],caGitem[12];
    char caSdate_temp[20],caEdate_temp[20],caUnit[16];
    long lStime,lEtime,lConntime;
    long lSumShow;
    long long lUbytes,lDbytes;
//    char caRefresh_flag[8];
   
    ncUserCont *psUser;
    long long lSumbytes;
    unsigned long  l_Count;
    long lSumcount;
    long lGitem;
    long lGroupid;
    long m;

    char caServ_flag[12];
    char caName[32],caHost[65];
    char caPdf[32];

    
    int rowrow;                     
    int column; 

    char caTitle[128];
    char filename[128];
    column=4;
    char **desc;
    char caStatdate[128];
    char caUsername[32],caGroupname[32];
    long Webid;
    char caIp[16];
    char caExpsql[32];
    char caSid[16];
    char caDescr[256];
    char *p;
    long lPid=0;
    long repid[50];
    
    unsigned char *pHash;
	  pasLHashInfo  sHashInfo;
	  unsigned long lMax=4000;
	  long lCode;
    unsigned long lIp;
    char caArea[64],caName2[64];
    char caReporttype[16];
    ncFlowIp *stUserflow;
    long iSumno;
    ncFlowIp *psFlow;
    long lRow;
   long lRepnum=0;
   lTime=time(0);
   lSum=0;
	  psDbHead = utPltInitDb();
   
      iReturn = utMsgGetSomeNVar(psMsgHead,3,"sum",  UT_TYPE_STRING,10,caSum,
                                             "reporttype",UT_TYPE_STRING,10,caReporttype,
																							"exp",    UT_TYPE_STRING,6, caExport);
		 if(strlen(caExport)>0){
		 	lRow=100000;
		}
		else{
			lRow=10000;
		}
			lPid=atol(caReporttype);	
			utPltPutVar(psDbHead,"reporttype",caReporttype);	
			for(i=0;i<atol(caSum);i++){
				  sprintf(caTemp,"ch%d",i+1);
		  		p=utMsgGetVar_s(psMsgHead,caTemp);
		  		if(p){
		//  			printf("p=%s\n",p);
		  			if(lRepnum>20) break;
		  			
		  			sprintf(caTemp,"select descr from ncreportplatelog where sid=%s ",p);
		  			pasDbOneRecord(caTemp,0,UT_TYPE_STRING,250,caDescr);
		  			
		  			utPltPutLoopVar(psDbHead,"descr",lRepnum+1,caDescr);
		  			utPltPutLoopVarF(psDbHead,"repnum",lRepnum+1,"%d",lRepnum+1);
		  			
		  			repid[lRepnum]=atol(p);
		  			lRepnum++;
		  			utPltPutLoopVar(psDbHead,"sid",lRepnum,p);
		  			
		  		}
			}	
			utPltPutVarF(psDbHead,"sum","%d",lRepnum);
				
			pHash = (unsigned char *)pasLHashInit(lMax,lMax,sizeof(ncFlowIp),0,4); 
			if(pHash == NULL) {
		        printf(" Memory HttpHash Error \n");
		        return (-1);
		  }
			
			for(i=0;i<lRepnum;i++){
				sprintf(caTemp,"select bytes,lcount,webname,ip,ubytes,dbytes from ncreportdet where tid=%d order by bytes desc ",repid[i]);
				sprintf(caTemp+strlen(caTemp)," limit 0,%d",lRow);
				psCur=pasDbOpenSql(caTemp,0);
		    if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
		        return 0;
		       }
				        iReturn = 0;
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        lUbytes=0;
        lDbytes=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG8,8,&lUbytes,
                                       UT_TYPE_LONG8,8,&lDbytes);
        while(iReturn==0||iReturn==1405){                              
          sprintf(caTemp,"%s%s",caIp,caHost);
          lCode=utMd5Code(caTemp,strlen(caTemp),"");     
                 
        psFlow = (ncFlowIp *)pasLHashLookA(pHash,&lCode);
				if(psFlow) { 
					strcpy(psFlow->Ip,caIp);
					strcpy(psFlow->host,caHost);
					psFlow->lBytes[i]=lBytes;
					psFlow->lUbytes[i]=lUbytes;
					psFlow->lDbytes[i]=lDbytes;
					psFlow->lCount[i]=l_Count;
				}    
                                       
				lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        lUbytes=0;
        lDbytes=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG8,8,&lUbytes,
                                       UT_TYPE_LONG8,8,&lDbytes);
         }
        pasDbCloseCursor(psCur); 
         
				
			}
																							
	    iSumno=pasLHashRecord(pHash);
      stUserflow=(ncFlowIp*)malloc((iSumno+2)*sizeof(ncFlowIp));
	    memset(stUserflow,0,(iSumno+2)*sizeof(ncFlowIp));
	     psFlow= (ncFlowIp *)pasLHashFirst(pHash,&sHashInfo);
      		  i=0;
				    while(psFlow) {
				        /* 输出该结构  */
				        strcpy(stUserflow[i].Ip,psFlow->Ip);
				        strcpy(stUserflow[i].host,psFlow->host);
				        for(m=0;m<lRepnum;m++){
				        	stUserflow[i].lCount[m]=psFlow->lCount[m];
				        	stUserflow[i].lBytes[m]=psFlow->lBytes[m];
				        	stUserflow[i].lUbytes[m]=psFlow->lUbytes[m];
				        	stUserflow[i].lDbytes[m]=psFlow->lDbytes[m];
				        }
				        
				        

				          i++;


				        /* 取下一条记录  */
				        psFlow = (ncFlowIp *)pasLHashNextS(&sHashInfo);
				}
	     free(pHash);
	//排序    
	    if(iSumno>0){
	    	if(lPid==2){
	    	   iOrderflow=0;
	    	  }
	    	  else{
	    	  	iOrderflow=1;
	    	  }
	
     	      qsort(stUserflow,iSumno,sizeof(ncFlowIp),ncUtlSortOnflow);
        }
	    
	    
	    if(strlen(caExport)==0){ 
	    	
	    	 for(i=0;i<iSumno;i++){
	    	 	  utPltPutLoopVarF(psDbHead,"num",i+1,"%d",i+1);
	    	 	          utPltPutLoopVar(psDbHead,"host",i+1,stUserflow[i].host);
                  utPltPutLoopVar(psDbHead,"ip",i+1,stUserflow[i].Ip);
                  
         lIp=ntohl(pasIpcvtLong(stUserflow[i].Ip)); 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iReturn == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar(psDbHead,"ip_desc",i+1,caTemp);
          }
          else{
          	strcpy(caTemp,"");
          }
                  
                  for(m=0;m<lRepnum;m++){
                  	utPltPutLoopVarF2(psDbHead,"count",i+1,m+1,"%d",stUserflow[i].lCount[m]);
                  	utPltPutLoopVarF2(psDbHead,"bytes",i+1,m+1,"%0.1f",(float)stUserflow[i].lBytes[m]/1024);
                  	utPltPutLoopVarF2(psDbHead,"ubytes",i+1,m+1,"%0.1f",(float)stUserflow[i].lUbytes[m]/1024);
                  	utPltPutLoopVarF2(psDbHead,"dbytes",i+1,m+1,"%0.1f",(float)stUserflow[i].lDbytes[m]/1024);
				          }
				         
	    	}
	    	free(stUserflow);
	    	if(lPid==2){
	    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_reportcomp.htm"); 
		    }
		    else if(lPid==1){
		    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_reportcomp_service.htm"); 
		    }
	    	return 0;
       


	    }
	    else{
      //输出到EXEC中
		    FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"nccomp%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613网站访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        
       if(lPid==2){ 
        
        fprintf(fp,"网站访问排行榜对比报表\n");      
	      fprintf(fp,"序号,网址,目标IP,Ip位置");
	      for(m=0;m<lRepnum;m++){
	      	fprintf(fp,",请求数,流量");
	      }
	        fprintf(fp,"\n");
	      
	      }
	      else if(lPid==1){
	      	    fprintf(fp,"%s\n","目标IP访问对比报表");    
	            fprintf(fp,"序号,目标IP,IP位置");
	            for(m=0;m<lRepnum;m++){
	            	fprintf(fp,",总流量(MB),上行流量(MB),下行流量(MB)");
	            }
	            fprintf(fp,"\n");
	            
	      }
	     for(i=0;i<iSumno;i++){
	 	     lIp=ntohl(pasIpcvtLong(stUserflow[i].Ip)); 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iReturn == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar(psDbHead,"dip_location",i+1,caTemp);
          }
          else{
          	strcpy(caTemp,"");
          }
	     
	     
	     
        if(lPid==2){
	      fprintf(fp,"%d,\"%s\",\"%s\",\"%s\"",i+1,stUserflow[i].host,stUserflow[i].Ip,caTemp);
	      for(m=0;m<lRepnum;m++){
	      	fprintf(fp,",%0.1f,%lu",(float)stUserflow[i].lBytes[m]/1024,stUserflow[i].lCount[m]);
	      }
	      fprintf(fp,"\n");
	     
	      }
	      else if(lPid==1){
	      	 fprintf(fp,"%d,\"%s\",\"%s\"",i+1,stUserflow[i].Ip,caTemp);
	      	 for(m=0;m<lRepnum;m++){
	      	 	fprintf(fp,",%0.1f,%0.1f,%0.1f",(float)stUserflow[i].lBytes[m]/1024,(float)stUserflow[i].lUbytes[m]/1024,(float)stUserflow[i].lDbytes[m]/1024);
	      	}
	      	fprintf(fp,"\n");
	      	 

	      }
	    }
        free(stUserflow);
         utPltFreeDb(psDbHead);  
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
     
		    return 0;
   }
   
  
		

}



/* 定时自动生成报表                                          */
int ncAutoGenReport(utShmHead *psShmHead)
{
    unsigned long lStopTime,lStartTime,lTime,lBackupTime;
    char *pTime;
    char *p;
    pasDbCursor *psCur;
    int iNum;
    char caSdate[32];
    char caStime[32];
    
    typedef struct ncReportplate_s {
    	  unsigned long sid;
        char name[64];
        long reporttype;
        int reporttable;
        int reportfield;
        int groupid;
        int service;
        int dateunit;
        int recnum;
        char mailreceiver[256];
        int mailflag;  
    } ncReportplate;
    char caTemp[2048];
    char caCom[2048];
    long lSid,lReporttype,lReportfield,lGroupid,lService,lDateunit,lRecnum,lMailflag;
    long lReporttable,lTid,i;
    char caDesc[256];
    char caName[64],caMailreceiver[256];
    char caEdate[20],caHour[16];
    ncReportplate stReportplate[30];
    long lCount;
    int iReturn;
    iReturn = pasConnect(psShmHead);
    if(iReturn < 0) {
       
        sleep(60);
        return 0;
    }
    iNum=0;
    while(1) {

     lTime=time(0);
       strcpy(caHour,utTimFormat("%%H",lTime));
       if(atol(caHour)>5||atol(caHour)==0){
 //如果有后台生成报表命令，则生成报表
 strcpy(caCom,utComGetVar_sd(psShmHead,"reportsql",""));
 
       
       	long lLen=strlen(caCom);
       	if(lLen>4){
       		if(strncmp(caCom,"002",3)==0){
       			//网站排行统计
       			strcpy(caTemp,caCom+4);
       		  ncGenWebSortReport(psShmHead,caTemp,100000);
       			utComSetVar_s(psShmHead,"reportsql","");
       			
       		}
       		else if(strncmp(caCom,"001",3)==0){
       			//网络服务目标ip统计
       			  strcpy(caTemp,caCom+4);
       			strcpy(caName,utComGetVar_sd(psShmHead,"reportname",""));       			  
       		  ncGenServiceSortReport(psShmHead,caTemp,100000,caName);
       			utComSetVar_s(psShmHead,"reportsql","");
       		  utComSetVar_s(psShmHead,"reportname","");
       		}
       	}
       	
       	
       	
       	sleep(3);
      	continue;
      }
    
     
     sprintf(caTemp,"select sid,name,reporttype,reporttable,reportfield,groupid,service,dateunit,recnum,mailreceiver,mailflag from ncreportplate where reportflag=0 order by stime desc limit 0,20");
 //    printf("caTemp=%s\n",caTemp);
     psCur=pasDbOpenSql(caTemp,0);
		    if(psCur == NULL) {
		      
		        return 0;
		       }
				iReturn = 0;
        lSid=0;
        memset(caName,0,sizeof(caName));
        lReporttype=0;
        lReporttable=0;
        lReportfield=0;
        lGroupid=0;
        lService=0;
        lDateunit=0;
        lRecnum=0;
        memset(caMailreceiver,0,sizeof(caMailreceiver));
        lMailflag=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG,4,&lSid,
                                       UT_TYPE_STRING,50,caName,
                                       UT_TYPE_LONG,4,&lReporttype,
                                       UT_TYPE_LONG,4,&lReporttable,
                                       UT_TYPE_LONG,4,&lReportfield,
                                       UT_TYPE_LONG,4,&lGroupid,
                                       UT_TYPE_LONG,4,&lService,
                                       UT_TYPE_LONG,4,&lDateunit,
                                       UT_TYPE_LONG,4,&lRecnum,
                                       UT_TYPE_STRING,255,caMailreceiver,
                                       UT_TYPE_LONG,4,&lMailflag);
        iNum=0;                          
        while(iReturn==0||iReturn==1405){                              
       stReportplate[iNum].sid=lSid;
       strcpy(stReportplate[iNum].name,caName);
       stReportplate[iNum].reporttype=lReporttype;
       stReportplate[iNum].reporttable=lReporttable;
       stReportplate[iNum].reportfield=lReportfield;
       stReportplate[iNum].groupid=lGroupid;
       stReportplate[iNum].service=lService;
       stReportplate[iNum].dateunit=lDateunit;
       stReportplate[iNum].recnum=lRecnum;
       stReportplate[iNum].mailflag=lMailflag;
       strcpy(stReportplate[iNum].mailreceiver,caMailreceiver);
         iNum++;                              
        lSid=0;
        memset(caName,0,sizeof(caName));
        lReporttype=0;
        lReportfield=0;
        lGroupid=0;
        lService=0;
        lDateunit=0;
        lRecnum=0;
        lReporttable=0;
        memset(caMailreceiver,0,sizeof(caMailreceiver));
        lMailflag=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG,4,&lSid,
                                       UT_TYPE_STRING,50,caName,
                                       UT_TYPE_LONG,4,&lReporttype,
                                       UT_TYPE_LONG,4,&lReporttable,
                                       UT_TYPE_LONG,4,&lReportfield,
                                       UT_TYPE_LONG,4,&lGroupid,
                                       UT_TYPE_LONG,4,&lService,
                                       UT_TYPE_LONG,4,&lDateunit,
                                       UT_TYPE_LONG,4,&lRecnum,
                                       UT_TYPE_STRING,255,caMailreceiver,
                                       UT_TYPE_LONG,4,&lMailflag);
         }
        pasDbCloseCursor(psCur); 
     

        for(i=0;i<iNum;i++){
        	if(stReportplate[i].reporttable==2){
 //网站访问排行
              if(stReportplate[i].dateunit==1){
 // 日报            	
		                   strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime-3600*24));
		                   sprintf(caDesc,"统计时间：%s到%s",caSdate,caSdate);
		            			 sprintf(caTemp,"select count(*) from ncreportplatelog where tid=%d and descr='%s' ",stReportplate[i].sid,caDesc);
		            			 printf("caTemp=%s\n",caTemp);
								       lCount=0;
								       pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
								       if(lCount==0){
		                        
			                   lTid=pasGetSid("ncreportplatelog","sid");            
			                   sprintf(caTemp,"insert into ncreportdet(tid,bytes,lcount,webname,ip) (select %d,sum(bytes) aa,sum(lcount),host,inet_ntoa(sid)  from nchttpcount where sdate='%s' ",lTid,caSdate);
			                   if(stReportplate[i].groupid>0){
			            	        sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where (groupid=%d or groupid in (select groupid from ncgroup where pid=%d)))",stReportplate[i].groupid,stReportplate[i].groupid);
			                   }
			                   sprintf(caTemp+strlen(caTemp)," group by host,inet_ntoa(sid) order by aa desc limit 0,%d) ",stReportplate[i].recnum);
			             
			             
			             
			                   printf("caTemp=%s\n",caTemp);
			             
			                   iReturn=pasDbExecSqlF(caTemp);
			                   if(iReturn==0){             
			             	        sprintf(caName,"%s(%s)",stReportplate[i].name,utTimFormat("%Y%m%d%H%M%S",lTime));
			             	        pasDbExecSqlF("insert into ncreportplatelog(sid,name,pid,tid,descr,stime) values(%d,'%s',2,%d,'%s',%d)",lTid,caName,stReportplate[i].sid,caDesc,lTime);
			             	        
	//如果需要发送邮件        
	if((stReportplate[i].mailflag==1)&&strlen(stReportplate[i].mailreceiver)>2){
		ncGenReportHtmlMail(psShmHead,lTid,stReportplate[i].reporttable,stReportplate[i].name,stReportplate[i].mailreceiver);
	}	        
			                    }
		                  }
              }
              else if(stReportplate[i].dateunit==2){
 //周报             
                  			ncUtlWeekDate(lTime-168*3600,caSdate,caEdate);
                  			sprintf(caDesc,"统计时间：%s到%s",caSdate,caEdate);
						      			sprintf(caTemp,"select count(*) from ncreportplatelog where tid=%d and descr='%s' ",stReportplate[i].sid,caDesc);
						      			lCount=0;
						      			pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
						      			if(lCount==0){						 	
						 	     					lTid=pasGetSid("ncreportplatelog","sid");            
	                 					sprintf(caTemp,"insert into ncreportdet(tid,bytes,lcount,webname,ip) (select %d,sum(bytes) aa,sum(lcount),host,inet_ntoa(sid)  from nchttpcount where sdate>='%s' and sdate<='%s' ",lTid,caSdate,caEdate);
	                 					if(stReportplate[i].groupid>0){
	                 						 sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where (groupid=%d or groupid in (select groupid from ncgroup where pid=%d)))",stReportplate[i].groupid,stReportplate[i].groupid);
	//            	    							sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where groupid=%d )",stReportplate[i].groupid);
	                					}
                  					sprintf(caTemp+strlen(caTemp)," group by host,inet_ntoa(sid) order by aa desc limit 0,%d) ",stReportplate[i].recnum);
             
                  					printf("caTemp=%s\n",caTemp);
             
                  					iReturn=pasDbExecSqlF(caTemp);
		               					if(iReturn==0){             
		                						sprintf(caName,"%s(%s)",stReportplate[i].name,utTimFormat("%Y%m%d%H%M%S",lTime));
		             	    					pasDbExecSqlF("insert into ncreportplatelog(sid,name,pid,tid,descr,stime) values(%d,'%s',2,%d,'%s',%d)",lTid,caName,stReportplate[i].sid,caDesc,lTime);
		             	    					
		             	    						//如果需要发送邮件        
	if((stReportplate[i].mailflag==1)&&strlen(stReportplate[i].mailreceiver)>2){
		ncGenReportHtmlMail(psShmHead,lTid,stReportplate[i].reporttable,stReportplate[i].name,stReportplate[i].mailreceiver);
	}	  
		             	    					
		             	    					
		                				}
						 	
						 	
						     				}
 
              }
              else if(stReportplate[i].dateunit==3){
        //月报
                  		ncUtlMonthDate(lTime-30*24*3600,caSdate,caEdate);
                  		sprintf(caDesc,"统计时间：%s到%s",caSdate,caEdate);
						      		sprintf(caTemp,"select count(*) from ncreportplatelog where tid=%d and descr='%s' ",stReportplate[i].sid,caDesc);
						      		lCount=0;
						      		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
						      		if(lCount==0){					 	
						 	       			lTid=pasGetSid("ncreportplatelog","sid");            
	                   			sprintf(caTemp,"insert into ncreportdet(tid,bytes,lcount,webname,ip) (select %d,sum(bytes) aa,sum(lcount),host,inet_ntoa(sid)  from nchttpcount where sdate>='%s' and sdate<='%s' ",lTid,caSdate,caEdate);
	                  	if(stReportplate[i].groupid>0){
	                  		 sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where (groupid=%d or groupid in (select groupid from ncgroup where pid=%d)))",stReportplate[i].groupid,stReportplate[i].groupid);
	//            	    			sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where groupid=%d )",stReportplate[i].groupid);
	                		}
                  				sprintf(caTemp+strlen(caTemp)," group by host,inet_ntoa(sid) order by aa desc limit 0,%d) ",stReportplate[i].recnum);
             
                         
                  				printf("caTemp=%s\n",caTemp);
             
                  			iReturn=pasDbExecSqlF(caTemp);
                  			if(iReturn==0){
             
             	       				sprintf(caName,"%s(%s)",stReportplate[i].name,utTimFormat("%Y%m%d%H%M%S",lTime));
             	       				pasDbExecSqlF("insert into ncreportplatelog(sid,name,pid,tid,descr,stime) values(%d,'%s',2,%d,'%s',%d)",lTid,caName,stReportplate[i].sid,caDesc,lTime);
             	       				
             	       			//如果需要发送邮件        
	if((stReportplate[i].mailflag==1)&&strlen(stReportplate[i].mailreceiver)>2){
		ncGenReportHtmlMail(psShmHead,lTid,stReportplate[i].reporttable,stReportplate[i].name,stReportplate[i].mailreceiver);
	}	  		
             	       				
                   			}
             
             
                 		}
             
        	     	}
           }
        	else if(stReportplate[i].reporttable==1){
   //网络服务目标IP     		
        		
        		  	if(stReportplate[i].dateunit==1){
 // 日报            	
              					strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime-3600*24));
              					sprintf(caDesc,"统计时间：%s到%s",caSdate,caSdate);
            						sprintf(caTemp,"select count(*) from ncreportplatelog where tid=%d and descr='%s' ",stReportplate[i].sid,caDesc);
            						printf("caTemp=%s\n",caTemp);
						      			lCount=0;
						      			pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
						      			if(lCount==0){
            
            
              								lTid=pasGetSid("ncreportplatelog","sid");            
              								sprintf(caTemp,"insert into ncreportdet(tid,bytes,ubytes,dbytes,lcount,ip) (select %d,sum(ubytes+dbytes) aa,sum(ubytes),sum(dbytes),0,inet_ntoa(dip)  from ncserviceip where sdate='%s' ",lTid,caSdate);
								              if(stReportplate[i].service>0){
											            	 if(stReportplate[i].service==2000){
											            	 	sprintf(caTemp+strlen(caTemp)," and service>=2000 and service<=2999 ");
											            	}
											            	else if(stReportplate[i].service==3000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=3000 and service<=3999 ");
											            	}
											             else if(stReportplate[i].service==1000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=1000 and service<=1999 ");
											            	}
											            	else if(stReportplate[i].service==4000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=4000 and service<=4999 ");
											            	}
											            	else if(stReportplate[i].service==9000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=9000 and service<=9999 ");
											            	}
											            	else if(stReportplate[i].service==10000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=10000 and service<=10999 ");
											            	}
											            	else if(stReportplate[i].service==7000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=7000 and service<=7999 ");
											            	}
											            	else if(stReportplate[i].service==8000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=8000 and service<=8999 ");
											            	}
											            	else if(stReportplate[i].service==11000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=11000 and service<=11999 ");
											            	}
											            	else if(stReportplate[i].service==12000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=12000 and service<=12999 ");
											            	}
											            	else if(stReportplate[i].service==13000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=13000 and service<=13999 ");
											            	}
											            	else if(stReportplate[i].service==14000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=14000 and service<=14999 ");
											            	}
											            	else if(stReportplate[i].service==15000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=15000 and service<=15999 ");
											            	}
											            	else if(stReportplate[i].service==17000){
											            		sprintf(caTemp+strlen(caTemp)," and service>=17000 and service<=17999 ");
											            	}
											            	else{
											            		sprintf(caTemp+strlen(caTemp)," and service=%d ",stReportplate[i].service);
											            	}
              									}
                								sprintf(caTemp+strlen(caTemp)," group by inet_ntoa(dip) order by aa desc limit 0,%d) ",stReportplate[i].recnum);
             
             
             
            										printf("caTemp=%s\n",caTemp);
             
                								iReturn=pasDbExecSqlF(caTemp);
                								if(iReturn==0){
             
             	   											sprintf(caName,"%s(%s)",stReportplate[i].name,utTimFormat("%Y%m%d%H%M%S",lTime));
             	   											pasDbExecSqlF("insert into ncreportplatelog(sid,name,pid,tid,descr,stime) values(%d,'%s',1,%d,'%s',%d)",lTid,caName,stReportplate[i].sid,caDesc,lTime);
             	   											
             	   												//如果需要发送邮件        
	if((stReportplate[i].mailflag==1)&&strlen(stReportplate[i].mailreceiver)>2){
		ncGenReportHtmlMail(psShmHead,lTid,stReportplate[i].reporttable,stReportplate[i].name,stReportplate[i].mailreceiver);
	}	  
             	   											
             	   											
                								}
               							}
              }
              else if(stReportplate[i].dateunit==2){
 //周报             
					                  ncUtlWeekDate(lTime-168*3600,caSdate,caEdate);
					                  sprintf(caDesc,"统计时间：%s到%s",caSdate,caEdate);
											      sprintf(caTemp,"select count(*) from ncreportplatelog where tid=%d and descr='%s' ",stReportplate[i].sid,caDesc);
											      lCount=0;
											      pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
											      if(lCount==0){						 	
											 	     lTid=pasGetSid("ncreportplatelog","sid");            
						                 sprintf(caTemp,"insert into ncreportdet(tid,bytes,ubytes,dbytes,lcount,ip) (select %d,sum(ubytes+dbytes) aa,sum(ubytes),sum(dbytes),0,inet_ntoa(dip)  from ncserviceip where sdate>='%s' and sdate<='%s' ",lTid,caSdate,caEdate);
							              if(stReportplate[i].service>0){
							            	 		if(stReportplate[i].service==2000){
							            	 				sprintf(caTemp+strlen(caTemp)," and service>=2000 and service<=2999 ");
							            			}
							            			else if(stReportplate[i].service==1000){
																		sprintf(caTemp+strlen(caTemp)," and service>=1000 and service<=1999 ");
													  		}
							            			else if(stReportplate[i].service==3000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=3000 and service<=3999 ");
							            			}
							            			else if(stReportplate[i].service==4000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=4000 and service<=4999 ");
							            			}
							            			else if(stReportplate[i].service==9000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=9000 and service<=9999 ");
							            			}
							            			else if(stReportplate[i].service==10000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=10000 and service<=10999 ");
							            			}
							            			else if(stReportplate[i].service==7000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=7000 and service<=7999 ");
							            			}
							            			else if(stReportplate[i].service==8000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=8000 and service<=8999 ");
							            			}
							            			else if(stReportplate[i].service==11000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=11000 and service<=11999 ");
							            			}
							            			else if(stReportplate[i].service==12000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=12000 and service<=12999 ");
							            			}
							            			else if(stReportplate[i].service==13000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=13000 and service<=13999 ");
							            			}
							            			else if(stReportplate[i].service==14000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=14000 and service<=14999 ");
							            			}
							            			else if(stReportplate[i].service==15000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=15000 and service<=15999 ");
							            			}
							            			else if(stReportplate[i].service==17000){
							            					sprintf(caTemp+strlen(caTemp)," and service>=17000 and service<=17999 ");
							            			}
							            			else{
							            					sprintf(caTemp+strlen(caTemp)," and service=%d ",stReportplate[i].service);
							            			}
		              					}
		               					 sprintf(caTemp+strlen(caTemp)," group by inet_ntoa(dip) order by aa desc limit 0,%d) ",stReportplate[i].recnum);
		             
             
             
             
                  					printf("caTemp=%s\n",caTemp);
             
                  					iReturn=pasDbExecSqlF(caTemp);
		               					if(iReturn==0){             
		                						sprintf(caName,"%s(%s)",stReportplate[i].name,utTimFormat("%Y%m%d%H%M%S",lTime));
		             	   						pasDbExecSqlF("insert into ncreportplatelog(sid,name,pid,tid,descr,stime) values(%d,'%s',1,%d,'%s',%d)",lTid,caName,stReportplate[i].sid,caDesc,lTime);
		             	   						
		             	   					//如果需要发送邮件        
	if((stReportplate[i].mailflag==1)&&strlen(stReportplate[i].mailreceiver)>2){
		ncGenReportHtmlMail(psShmHead,lTid,stReportplate[i].reporttable,stReportplate[i].name,stReportplate[i].mailreceiver);
	}	  		
		             	   						
		               					}
						 	
						 	
						     				}
 
              }
              else if(stReportplate[i].dateunit==3){
        //月报
                  					ncUtlMonthDate(lTime-30*24*3600,caSdate,caEdate);
                  					sprintf(caDesc,"统计时间：%s到%s",caSdate,caEdate);
						     					  sprintf(caTemp,"select count(*) from ncreportplatelog where tid=%d and descr='%s' ",stReportplate[i].sid,caDesc);
						      					lCount=0;
						      					pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
						      					if(lCount==0){
            
            
             										 lTid=pasGetSid("ncreportplatelog","sid");            
              									 sprintf(caTemp,"insert into ncreportdet(tid,bytes,ubytes,dbytes,lcount,ip) (select %d,sum(ubytes+dbytes) aa,sum(ubytes),sum(dbytes),0,inet_ntoa(dip)  from ncserviceip where sdate>='%s' and sdate<='%s' ",lTid,caSdate,caEdate);
              									 if(stReportplate[i].service>0){
            	                        if(stReportplate[i].service==2000){
            	 														sprintf(caTemp+strlen(caTemp)," and service>=2000 and service<=2999 ");
            													}
            												 else if(stReportplate[i].service==1000){
											            		    sprintf(caTemp+strlen(caTemp)," and service>=1000 and service<=1999 ");
											            	 }
												            	else if(stReportplate[i].service==3000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=3000 and service<=3999 ");
												            	}
												            	else if(stReportplate[i].service==4000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=4000 and service<=4999 ");
												            	}
												            	else if(stReportplate[i].service==9000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=9000 and service<=9999 ");
												            	}
												            	else if(stReportplate[i].service==10000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=10000 and service<=10999 ");
												            	}
												            	else if(stReportplate[i].service==7000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=7000 and service<=7999 ");
												            	}
												            	else if(stReportplate[i].service==8000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=8000 and service<=8999 ");
												            	}
												            	else if(stReportplate[i].service==11000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=11000 and service<=11999 ");
												            	}
												            	else if(stReportplate[i].service==12000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=12000 and service<=12999 ");
												            	}
												            	else if(stReportplate[i].service==13000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=13000 and service<=13999 ");
												            	}
												            	else if(stReportplate[i].service==14000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=14000 and service<=14999 ");
												            	}
												            	else if(stReportplate[i].service==15000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=15000 and service<=15999 ");
												            	}
												            	else if(stReportplate[i].service==17000){
												            		  sprintf(caTemp+strlen(caTemp)," and service>=17000 and service<=17999 ");
												            	}
												            	else{
												            		  sprintf(caTemp+strlen(caTemp)," and service=%d ",stReportplate[i].service);
												            	}
                                 }
                                 sprintf(caTemp+strlen(caTemp)," group by inet_ntoa(dip) order by aa desc limit 0,%d) ",stReportplate[i].recnum);
             
             
             
                                printf("caTemp=%s\n",caTemp);
             
                                iReturn=pasDbExecSqlF(caTemp);
                                if(iReturn==0){
             
             	                       sprintf(caName,"%s(%s)",stReportplate[i].name,utTimFormat("%Y%m%d%H%M%S",lTime));
             	                       pasDbExecSqlF("insert into ncreportplatelog(sid,name,pid,tid,descr,stime) values(%d,'%s',1,%d,'%s',%d)",lTid,caName,stReportplate[i].sid,caDesc,lTime);
             	                       
             	             	//如果需要发送邮件        
	if((stReportplate[i].mailflag==1)&&strlen(stReportplate[i].mailreceiver)>2){
		ncGenReportHtmlMail(psShmHead,lTid,stReportplate[i].reporttable,stReportplate[i].name,stReportplate[i].mailreceiver);
	}	            
             	                       
                                }
                              }
             
             
                
             
        	        }
              
 		
        		
        		
        	}
        	
        }
     
       

        sleep(3600);
    }
    return 0;
}

//ID-报表ID  caRname-报表名称 pMail-收件人 reporttable——报表类型 2：网站 1：服务目标IP

/* 生成报表，发送邮件*/
int ncGenReportHtmlMail(utShmHead *psShmHead,long id,long reporttable,char *caRname,char *pMail)
{
    utPltDbHead *psDbHead;
    char sqlbuf[2048];
    char caName[32];
    char caSmt[2048];
    int iNum;
    int iReturn;
    char caFileplate[128];
    unsigned long lTime;
    int i,m;
    pasDbCursor *psCur;
    char caTemp[2048];
    char *pTimeStr;
    char caSql[1024];
    char caDispname[32],caUsername[16],caGroupname[32];
    long lCtime;

    long long lBytes,lSumbytes;
    long lSumtime;    
    
    char caPath[128];
    char caHost[68],caIp[20];
    char caArea[64],caName2[64];
    long long lUbytes,lDbytes;
    long l_Count;
    unsigned long lIp;
    long iRet;
    char *pHtml;
  lSumtime=0;
  lSumbytes=0;
  memset(caSmt,0,sizeof(caSmt));
  
  
  if(reporttable==2){
  	 sprintf(sqlbuf,"select bytes,lcount,webname,ip,ubytes,dbytes from ncreportdet where tid=%d order by lcount desc ",id);
  }
  else if(reporttable==1){
  	sprintf(sqlbuf,"select bytes,lcount,webname,ip,ubytes,dbytes from ncreportdet where tid=%d order by bytes desc ",id);
  }
   
   psCur = pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
        printf("open psCur2 error\n");
        return 0;
    }
	  iReturn = 0;
    psDbHead = utPltInitDb();


  
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        lUbytes=0;
        lDbytes=0;
        lBytes=0;
        l_Count=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG8,8,&lUbytes,
                                       UT_TYPE_LONG8,8,&lDbytes);
	   iNum=0;
	   while((iReturn==0)||(iReturn==1405))
	   {  
         utPltPutLoopVar(psDbHead,"host",  iNum+1,caHost);	
         utPltPutLoopVar(psDbHead,"dip", iNum+1,caIp);																																						//按次数排序     
				 utPltPutLoopVarF(psDbHead,"count",iNum+1,"%lu",l_Count);				      
         utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum+1);        
         utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%0.1f",(float)lBytes/1024);
         utPltPutLoopVarF(psDbHead,"ubytes",iNum+1,"%0.1f",(float)lUbytes/1024);
         utPltPutLoopVarF(psDbHead,"dbytes",iNum+1,"%0.1f",(float)lDbytes/1024);
   
   	 	   lIp=ntohl(pasIpcvtLong(caIp)); 
         iRet = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iRet == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar(psDbHead,"dip_location",iNum+1,caTemp);
          }
          else{
          	strcpy(caTemp,"");
          }
	   	
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        lUbytes=0;
        lDbytes=0;
        lBytes=0;
        l_Count=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG8,8,&lUbytes,
                                       UT_TYPE_LONG8,8,&lDbytes);
		         iNum++;
		  
		  }
	    pasDbCloseCursor(psCur); 
	  


	utPltPutVar(psDbHead,"rname",caRname);
  if(reporttable==2){
   	sprintf(caFileplate,"%s/nc75/iframe_reporthis_mail.htm",utComGetVar_sd(psShmHead,"PlatePath","/home/ncmysql/nc/plate"));
	}
	else if(reporttable==1){
		sprintf(caFileplate,"%s/nc75/mail_reporthis_serdip.htm",utComGetVar_sd(psShmHead,"PlatePath","/home/ncmysql/nc/plate"));
	}
    if(!utFileIsExist(caFileplate)){
    	return 0;
    }
   if(reporttable==2){
  			pHtml = utPltFileToHtml("nc75/iframe_reporthis_mail.htm",psDbHead);
   }
   else if(reporttable==1){
   	    pHtml = utPltFileToHtml("nc75/mail_reporthis_serdip.htm",psDbHead);
  }
   if(pHtml) {
         char caObj[100][128];
    pHtml = utStrSkipSpaces(pHtml);
//将收件人以','拆分成多个收件人
    iReturn=char_Sep(pMail,',',caObj);

    for(i=0;i<iReturn;i++){
    	 ncMailCtlHtmlMailSend(utComGetVar_sd(psShmHead,"NcMailFrom","ProEIM@proeim.com"),caObj[i],caRname,pHtml);
    	
    }
       
       
        utPltFreeDb(psDbHead);
       
        return 0;
    }
    else {
        
        utPltFreeDb(psDbHead);    
        return (-1);
    }        
  
  
//   utPltFileToHtmlFile("mail/nc_iframe_mailreport.htm",caFile,psDbHead);
    return 0;
}

typedef struct ncCompIp_s {
    	  unsigned long lMd5;
        char Ip[16];
        char host[100];
        unsigned long long lBytes;
        unsigned long lCount; 
        unsigned long index;  
        unsigned long flag;    
    } ncCompIp;

typedef struct ncCompTwoIp_s {
        char Ip[16];
        char host[100];
        unsigned long index;  
        char Ip_2[16];
        char host_2[100];
        unsigned long index_2;  
    } ncCompTwoIp;
/* 显示对比报表*/
int ncShowCompReport2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  pasDbCursor *psCur;
    char sqlbuf[1024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
//    char serv_name[32];
    long iNum;
    long long lSum;

    char caTemp[1024];
    int iReturn,i;
    char caOrder[16];
    long lGroupid0;
    char caExport[12];
    char caSum[12];
    char caSdate[16],caEdate[16],caShour[8],caEhour[8],caGroupid[16],caGitem[12];
    char caSdate_temp[20],caEdate_temp[20],caUnit[16];
    long lStime,lEtime,lConntime;
    long lSumShow;
    long long lUbytes,lDbytes;
//    char caRefresh_flag[8];

    long long lSumbytes;
    unsigned long  l_Count;
    long lSumcount;
    long lGitem;
    long lGroupid;
    long m;

    char caServ_flag[12];
    char caName[32],caHost[65];
    char caPdf[32];

    
    int rowrow;                     
    int column; 

    char caTitle[128];
    char filename[128];
    column=4;
    char **desc;
    char caStatdate[128];
    char caUsername[32],caGroupname[32];
    long Webid;
    char caIp[16];
    char caExpsql[32];
    char caSid[16];
    char caDescr[256];
    char *p;
    long lPid=0;
    long repid[50];
    char caTemp1[256],caTemp2[256];
    unsigned char *pHash1,*pHash2;
	  pasLHashInfo  sHashInfo;
	  unsigned long lMax=4000;
	  long lCode;
    unsigned long lIp;
    char caArea[64],caName2[64];
    char caReporttype[16];
    ncFlowIp *stUserflow;
    long iSumno;
    ncCompIp *psFlow1,*psFlow2,*psFlow;
    ncCompIp *pData1,*pData2;
    ncCompTwoIp *pTwo;
   long lRepnum=0;
   long iSum1,iSum2,iSum;
   long lRow;
   lTime=time(0);
   lSum=0;
	  psDbHead = utPltInitDb();
   
      iReturn = utMsgGetSomeNVar(psMsgHead,3,"sum",  UT_TYPE_STRING,10,caSum,
                                             "reporttype",UT_TYPE_STRING,10,caReporttype,
																							"exp",    UT_TYPE_STRING,6, caExport);
			if(strlen(caExport)>0){
				lRow=100000;
			}
			else{
				lRow=10000;
			}
			lPid=atol(caReporttype);	
			utPltPutVar(psDbHead,"reporttype",caReporttype);	
			for(i=0;i<atol(caSum);i++){
				  sprintf(caTemp,"ch%d",i+1);
		  		p=utMsgGetVar_s(psMsgHead,caTemp);
		  		if(p){
		//  			printf("p=%s\n",p);
		  			if(lRepnum>2) break;
		  			
		  			sprintf(caTemp,"select descr from ncreportplatelog where sid=%s ",p);
		  			pasDbOneRecord(caTemp,0,UT_TYPE_STRING,250,caDescr);
		  			
		  			utPltPutLoopVar(psDbHead,"descr",lRepnum+1,caDescr);
		  			utPltPutLoopVarF(psDbHead,"repnum",lRepnum+1,"%d",lRepnum+1);
		  			
		  			repid[lRepnum]=atol(p);
		  			lRepnum++;
		  			utPltPutLoopVar(psDbHead,"sid",lRepnum,p);
		  			
		  		}
			}	
			utPltPutVarF(psDbHead,"sum","%d",lRepnum);
				

		 if(lRepnum!=2){
		 	       utPltFreeDb(psDbHead);
		 			  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","请选择两组数据进行对比","请选择两组数据进行对比");
		        return 0;
		}
				
				
			pHash1 = (unsigned char *)pasLHashInit(lMax,lMax,sizeof(ncCompIp),0,4); 
			if(pHash1 == NULL) {
		        printf(" Memory HttpHash Error \n");
		        return (-1);
		  }
		  
		 pHash2 = (unsigned char *)pasLHashInit(lMax,lMax,sizeof(ncCompIp),0,4); 
			if(pHash1 == NULL) {
		        printf(" Memory HttpHash Error \n");
		        return (-1);
		  }
		  pData1=(ncCompIp*)malloc((10000+2)*sizeof(ncCompIp));
	    memset(pData1,0,(10000+2)*sizeof(ncCompIp));
		 
		 	pData2=(ncCompIp*)malloc((10000+2)*sizeof(ncCompIp));
	    memset(pData2,0,(10000+2)*sizeof(ncCompIp)); 
		  
//报表1数据		
	
				sprintf(caTemp,"select bytes,lcount,webname,ip from ncreportdet where tid=%d order by bytes desc ",repid[0]);
				sprintf(caTemp+strlen(caTemp)," limit 0,%d",lRow);
				psCur=pasDbOpenSql(caTemp,0);
		    if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
		        return 0;
		       }
				        iReturn = 0;
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
    
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp);
        iSum1=0;                            
        while((iReturn==0||iReturn==1405)&&iSum1<10000){ 
        	
        	
        	                             
          sprintf(caTemp,"%s%s",caIp,caHost);
          lCode=utMd5Code(caTemp,strlen(caTemp),"");     
                 
        psFlow1 = (ncCompIp *)pasLHashLookA(pHash1,&lCode);
				if(psFlow1) { 
			
					strcpy(pData1[iSum1].host,caHost);
					strcpy(pData1[iSum1].Ip,caIp);
					pData1[iSum1].index=iSum1+1;
					
					
					
					strcpy(psFlow1->Ip,caIp);
					strcpy(psFlow1->host,caHost);
					psFlow1->lBytes=lBytes;
					psFlow1->lCount=l_Count;
					psFlow1->index=iSum1+1;
					iSum1++;
	
				}    
                                       
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
    
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp);
         }
        pasDbCloseCursor(psCur); 
         
			
	
//报表2数据	
				sprintf(caTemp,"select bytes,lcount,webname,ip from ncreportdet where tid=%d order by bytes desc ",repid[1]);
				sprintf(caTemp+strlen(caTemp)," limit 0,%d",lRow);
				psCur=pasDbOpenSql(caTemp,0);
		    if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
		        return 0;
		       }
				 iReturn = 0;
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
    
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp);
        iSum2=0;                            
       while((iReturn==0||iReturn==1405)&&iSum2<10000){                        
          sprintf(caTemp,"%s%s",caIp,caHost);
          lCode=utMd5Code(caTemp,strlen(caTemp),"");     
                 
        psFlow2 = (ncCompIp *)pasLHashLookA(pHash2,&lCode);
				if(psFlow2) { 
					
					strcpy(pData2[iSum2].host,caHost);
					strcpy(pData2[iSum2].Ip,caIp);
					pData2[iSum2].index=iSum2+1;
					
					
					strcpy(psFlow2->Ip,caIp);
					strcpy(psFlow2->host,caHost);
					psFlow2->lBytes=lBytes;
					psFlow2->lCount=l_Count;
					psFlow2->index=iSum2+1;
					iSum2++;
				}    
                                       
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
    
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp);
         }
        pasDbCloseCursor(psCur); 
	
	
	
	if(iSum2>iSum1){
		iSum=iSum2;
	}
	else{
		iSum=iSum1;
	}
	
	 
	
			
//判断第一天的记录在第二天记录中某一IP是否存在
    for(i=0;i<iSum1;i++){
    					sprintf(caTemp,"%s%s",pData1[i].Ip,pData1[i].host);
              lCode=utMd5Code(caTemp,strlen(caTemp),"");                      
              psFlow2 = (ncCompIp *)pasLHashLook(pHash2,&lCode);
				      if(psFlow2){
				      	pData1[i].flag=1;
				      }
				      else{
				      	pData1[i].flag=0;
				      }
    }
	   
				
		//判断第二天的记录在第一天记录中某一IP是否存在
    for(i=0;i<iSum2;i++){
    					sprintf(caTemp,"%s%s",pData2[i].Ip,pData2[i].host);
              lCode=utMd5Code(caTemp,strlen(caTemp),"");                      
              psFlow2 = (ncCompIp *)pasLHashLook(pHash1,&lCode);
				      if(psFlow2){
				      	pData2[i].flag=1;
				      }
				      else{
				      	pData2[i].flag=0;
				      }
    }
		
				
				
	  //   free(pHash);
   
	    
	    
	    
	    
	    
	    
	    if(strlen(caExport)==0){ 
	    	for(m=0;m<2;m++){
	    		utPltPutLoopVarF(psDbHead,"no",m+1,"%d",m+1);
    		
	    	}
//输出第一组数据   
   iNum=0;
       for(i=0;i<iSum1;i++){
       	if(pData1[i].flag==1){
       		utPltPutLoopVar2(psDbHead,"host",iNum+1,1,pData1[i].host);
       		utPltPutLoopVar2(psDbHead,"ip",iNum+1,1,pData1[i].Ip);
       		utPltPutLoopVarF2(psDbHead,"num",iNum+1,1,"%d",pData1[i].index);
       		
       	lIp=ntohl(pasIpcvtLong(pData1[i].Ip)); 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iReturn == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar2(psDbHead,"ip_desc",iNum+1,1,caTemp);
          }
         
       		
       		
       		iNum++;
       	}
      } 	
      for(i=0;i<iSum1;i++){
       	if(pData1[i].flag==0){
       		utPltPutLoopVar2(psDbHead,"host",iNum+1,1,pData1[i].host);
       		utPltPutLoopVar2(psDbHead,"ip",iNum+1,1,pData1[i].Ip);
       		utPltPutLoopVarF2(psDbHead,"num",iNum+1,1,"%d",pData1[i].index);
       		
       		
       	lIp=ntohl(pasIpcvtLong(pData1[i].Ip)); 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iReturn == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar2(psDbHead,"ip_desc",iNum+1,1,caTemp);
          }
       		
       		
       		iNum++;
       	}
      } 
     while(iNum<iSum){
	    	  	utPltPutLoopVar2(psDbHead,"host",iNum+1,1,"&nbsp;");
	    	  	iNum++;
	   }
      
//输出第二组数据   
   iNum=0;
       for(i=0;i<iSum2;i++){
       	if(pData1[i].flag==1){
       		utPltPutLoopVar2(psDbHead,"host",iNum+1,2,pData2[i].host);
       		utPltPutLoopVar2(psDbHead,"ip",iNum+1,2,pData2[i].Ip);
       		utPltPutLoopVarF2(psDbHead,"num",iNum+1,2,"%d",pData2[i].index);
       		
       	lIp=ntohl(pasIpcvtLong(pData2[i].Ip)); 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iReturn == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar2(psDbHead,"ip_desc",iNum+1,2,caTemp);
          }
       		
       		iNum++;
       	}
      } 	
      for(i=0;i<iSum2;i++){
       	if(pData2[i].flag==0){
       		utPltPutLoopVar2(psDbHead,"host",iNum+1,2,pData2[i].host);
       		utPltPutLoopVar2(psDbHead,"ip",iNum+1,2,pData2[i].Ip);
       		utPltPutLoopVarF2(psDbHead,"num",iNum+1,2,"%d",pData2[i].index);
       	lIp=ntohl(pasIpcvtLong(pData1[i].Ip)); 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iReturn == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar2(psDbHead,"ip_desc",iNum+1,2,caTemp);
          }
       		
       		iNum++;
       	}
      } 
     while(iNum<iSum){
	    	  	utPltPutLoopVar2(psDbHead,"host",iNum+1,2,"&nbsp;");
	    	  	iNum++;
	   }
      
      
	    		
	    	
	    	
	    	
	    	
        free(pData1);
        free(pData2);
        free(pHash1);
        free(pHash2);
	    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_reportcomp_2.htm"); 

	    	return 0;
       


	    }
	    else{
      //输出到EXEC中
		    FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"nccomp%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613网站访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        
        
        fprintf(fp,"目标IP差异对比报表\n");  
        
            
	      fprintf(fp,"序号,网址,目标IP,Ip位置,序号,网址,目标IP,Ip位置\n");
	    
	     	pTwo=(ncCompTwoIp*)malloc((iSum+2)*sizeof(ncCompTwoIp));
	memset(pTwo,0,(iSum+2)*sizeof(ncCompTwoIp));
	iNum=0;
	for(i=0;i<iSum1;i++){
		if(pData1[i].flag==1){
			strcpy(pTwo[iNum].Ip,pData1[i].Ip);
			strcpy(pTwo[iNum].host,pData1[i].host);
			pTwo[iNum].index=pData1[i].index;
			iNum++;
		}
	}
	for(i=0;i<iSum1;i++){
		if(pData1[i].flag==0){
			strcpy(pTwo[iNum].Ip,pData1[i].Ip);
			strcpy(pTwo[iNum].host,pData1[i].host);
			pTwo[iNum].index=pData1[i].index;
			iNum++;
		}
	}
	    
	    
	 iNum=0;
	for(i=0;i<iSum2;i++){
		if(pData2[i].flag==1){
			strcpy(pTwo[iNum].Ip_2,pData2[i].Ip);
			strcpy(pTwo[iNum].host_2,pData2[i].host);
			pTwo[iNum].index_2=pData2[i].index;
			iNum++;
		}
	}
	for(i=0;i<iSum2;i++){
		if(pData2[i].flag==0){
			strcpy(pTwo[iNum].Ip_2,pData2[i].Ip);
			strcpy(pTwo[iNum].host_2,pData2[i].host);
			pTwo[iNum].index_2=pData2[i].index;
			iNum++;
		}
	}
	 for(i=0;i<iSum;i++){
	 		 	 lIp=ntohl(pasIpcvtLong(pTwo[i].Ip)); 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if((iReturn == 1)&&lIp>0) {
          	sprintf(caTemp1,"%s,%s",caArea,caName2);
           
          }
          else{
          	strcpy(caTemp1,"");
          }
	 	
	 		 		lIp=ntohl(pasIpcvtLong(pTwo[i].Ip_2)); 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if((iReturn == 1)&&lIp>0) {
          	sprintf(caTemp2,"%s,%s",caArea,caName2);
           
          }
          else{
          	strcpy(caTemp2,"");
          }
	 	      fprintf(fp,"%d,%s,%s,%s,%d,%s,%s,%s\n",pTwo[i].index,pTwo[i].host,pTwo[i].Ip,caTemp1,pTwo[i].index_2,pTwo[i].host_2,pTwo[i].Ip_2,caTemp2);
	 	
	}
	    
	    
	    
	    
	    
	    
	    
	     
	     
	      free(pData1);
        free(pData2);
        free(pHash1);
        free(pHash2);
        free(pTwo);

      
         utPltFreeDb(psDbHead);  
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
     
		    return 0;
   }
   
  
		

}

//后台生成网站排行报表
int ncGenWebSortReport(utShmHead *psShmHead,char *sql,long lSumShow)
{
	  char caTemp[1024];
		char caDesc[256];
	  char caTdesc[240];
	  char caHost[128],caIp[20];
	  long lSid;
	  pasDbCursor *psCur;
	  unsigned long lTime;
    unsigned long long lBytes;
	  long long lSumbytes;
	  long iReturn,i;
    unsigned long long l_Count;
	  long iNum;
	  
	   struct ncFlowSort_s {
        unsigned long long lBytes;
        unsigned long long l_Count;       
        char caHost[64];
        char dip[16];
    };
    struct ncFlowSort_s *pFlow;
	  
 
	  psCur=pasDbOpenSql(sql,0);
    if(psCur == NULL) {     
        return 0;
    }
    
   pFlow=(struct ncFlowSort_s *)malloc(lSumShow*sizeof(struct ncFlowSort_s));
   if(pFlow==NULL){
        return 0;
   }
    
    
    
       iReturn = 0;
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp);
                                                                  
	     iNum=0;
	     
	    lSid=pasGetSid("ncreportplatelog","sid");
			strcpy(caTdesc,utTimFormat("%Y%m%d%H%M%S",time(0)));
			sprintf(caDesc,"网址请求数前N排行(%s)",caTdesc);
	     
	     
	     while(iReturn==0||iReturn==1405){
	       if(iNum>99999) break;
	      pFlow[iNum].lBytes=lBytes;
	     	pFlow[iNum].l_Count=l_Count;
	     
	     	strcpy(pFlow[iNum].caHost,caHost);
	      strcpy(pFlow[iNum].dip,caIp);
	     
	     
	     	iNum++;
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        memset(caIp,0,sizeof(caIp));
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,15,caIp); 
	    }
	    pasDbCloseCursor(psCur); 
     if(iNum>0){
     	
     		lSid=pasGetSid("ncreportplatelog","sid");

	
			strcpy(caTdesc,utTimFormat("%Y%m%d%H%M%S",time(0)));
			sprintf(caDesc,"网址请求数前N排行(%s)",caTdesc);
			iReturn=pasDbExecSqlF("insert into ncreportplatelog(sid,name,descr,stime,pid) values(%d,'%s','%s',%d,2)",lSid,caDesc,caDesc,time(0));
			if(iReturn==0){
				for(i=0;i<iNum;i++){
					pasDbExecSqlF("insert into ncreportdet(tid,webname,ip,bytes,lcount) values(%d,'%s','%s',%llu,%d)",\
					lSid,pFlow[i].caHost,pFlow[i].dip,pFlow[i].lBytes,pFlow[i].l_Count);
				}
			}
      }
    
    free(pFlow);
	return 0;
}


//后台生成网络服务目标IP排行报表
int ncGenServiceSortReport(utShmHead *psShmHead,char *sql,long lSumShow,char *caName)
{
	  char caTemp[1024];
		char caDesc[256];
	  char caTdesc[240];
	  char caHost[128],caIp[20];
	  long lSid;
	  pasDbCursor *psCur;
	  unsigned long lTime;
    long long lSumbytes,lUbytes,lDbytes,lBytes;
    unsigned long lDip;
    long lService;
	  long iReturn,i;
    unsigned long long l_Count;
	  long iNum;
	  
    struct ncFlowSort_s {
        unsigned long  dip;
        unsigned long long bytes;  
        unsigned long long ubytes;
        unsigned long long dbytes;     
    };
    struct ncFlowSort_s *pFlow;
	  
	  
	  psCur=pasDbOpenSql(sql,0);
    if(psCur == NULL) {     
        return 0;
    }
    
   pFlow=(struct ncFlowSort_s *)malloc(lSumShow*sizeof(struct ncFlowSort_s));
   if(pFlow==NULL){
        return 0;
   }
    
  	printf("sql1=%s\n",sql);   

      
        iReturn = 0;
        lBytes=0;
        lUbytes=0;
        lDbytes=0;
        lDip=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&lUbytes,
                                       UT_TYPE_LONG8,8,&lDbytes,
                                       UT_TYPE_ULONG,4,&lDip);
                                      
                                                                  
	     iNum=0;

	     while(iReturn==0||iReturn==1405){
	     
	     	pFlow[iNum].bytes=lBytes;
	     	pFlow[iNum].ubytes=lUbytes;
	     	pFlow[iNum].dbytes=lDbytes;
	     	pFlow[iNum].dip=lDip;
	     
	    
	     
	     	iNum++;
        lBytes=0;
        lUbytes=0;
        lDbytes=0;
        lDip=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&lUbytes,
                                       UT_TYPE_LONG8,8,&lDbytes,
                                       UT_TYPE_ULONG,4,&lDip);  
	    }
	     pasDbCloseCursor(psCur); 
	 //    printf("iNum=%d\n",iNum);
     if(iNum>0){
     	
     		lSid=pasGetSid("ncreportplatelog","sid");

	
	strcpy(caTdesc,utTimFormat("%Y%m%d%H%M%S",time(0)));
	sprintf(caDesc,"网络服务%s目标IP排行(%s)",caName,caTdesc);

			iReturn=pasDbExecSqlF("insert into ncreportplatelog(sid,name,descr,stime,pid) values(%d,'%s','%s',%d,1)",lSid,caDesc,caDesc,time(0));
			if(iReturn==0){
				for(i=0;i<iNum;i++){
			pasDbExecSqlF("insert into ncreportdet(tid,webname,ip,bytes,ubytes,dbytes,lcount) values(%d,'%s','%s',%llu,%llu,%llu,%d)",\
			lSid,"",utComHostIp(htonl(pFlow[i].dip)),pFlow[i].bytes,pFlow[i].ubytes,pFlow[i].dbytes,0);
				}
			}
      }
    
    free(pFlow);
	return 0;
}