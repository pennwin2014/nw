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


//排序  按流量排序
static int ncUtlSortOnServiceId(char *p1,char *p2)
{
    int i;
    long long l;
   
    struct ncSflow_s psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct ncSflow_s));
    memcpy(&psC2,p2,sizeof(struct ncSflow_s));    
    l=psC2.lBytes-psC1.lBytes;
    if(l>0) return 1;
    if(l<0) return -1;
    if(l==0) return 0;
  
}


//排序  按服务ID排序
static int ncUtlSortOnServiceId2(char *p1,char *p2)
{
    int i;
    long l;
   
    struct ncSflow_s psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct ncSflow_s));
    memcpy(&psC2,p2,sizeof(struct ncSflow_s));    
    l=psC1.lService-psC2.lService;
    if(l>0) return 1;
    if(l<0) return -1;
    if(l==0) return 0;
  
}
//排序  按服务ID降序排序
static int ncUtlSortOnServiceId3(char *p1,char *p2)
{
    int i;
    long l;
   
    struct ncSflow_s psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct ncSflow_s));
    memcpy(&psC2,p2,sizeof(struct ncSflow_s));    
    l=psC1.lService-psC2.lService;
    if(l>0) return -1;
    if(l<0) return 1;
    if(l==0) return 0;
  
}

/* 显示网络服务查询*/
int ncWebShowServiceDeptCx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{   
    pasDbCursor *psCur;
    char sqlbuf[2024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
    char serv_name[32];
    long lService,iNum;
    long long lSum;
    char caDate[20];
    char caDate_end[20];
    int iReturn,i;
    char caRefresh_flag[8];
    struct ncSflow_s  sService[500];
    struct ncSflow_s  sGservice[20];
    long long lBytes_other;
    char caName[32];
    long lGroupid;
    char preflag[8],nextflag[8],ctime[12];
    char caServ_flag[12];
    char caDate_sh[16],caDate_sm[16],caDate_eh[16],caDate_em[16];
    char caGitem[16],caSdate[16],caShour[8],caSmin[8],caEdate[16],caEhour[8],caEmin[8];
    char caSdate_all[20],caEdate_all[20];
    long lStime,lEtime,lRecenttime,lStime1,lSid;
    long lSumindex;
    long lSumGindex;
    char caMaxdate[16],caMaxtime[12],caRectime[16],caTemp[1024];
    long long lOth=0;
    long index;
    long lSj;
    char caWorksql[1024],caWorksql2[1024];
//iReturn=ncMailAttachSend("aa@pronetway.com","lyf@pronetway.com","test","test","/home/ncmysql/nc/bin/aa.PDF","reprot");

//printf("iReturn=%d\n",iReturn);
    
     struct piechart_datasetf *pChart;           //用于PDF文档输出
//   char *desc[]={"用户名","部门","流量","百分比"};
    int rowrow;                     
    int column; 
    char **pTemp;
    char caTitle[128];
    char filename[128];
    int m;
    column=4;
    long lDate;
    char **desc;
    char caPdf[16];
    char caStatdate[128];
    char user_Id[15]="";
    char dpname[32]="";
    char gpname[32]="";
    char rtname[32]="";
    char expexcel[12]="";
    char f_name[120]="";
    char cp_command[320]="";
  	char rm_command[320]="";
  	char caSdate_All[20],caEdate_All[20];
  	char caWorkflag[12];
  	FILE *fp; 
  	char group_id[32]="";
   lTime=time(0);
   lSum=0;
   lRecenttime=0;
   
      utMsgGetSomeNVar(psMsgHead,15,"gitem",UT_TYPE_STRING,10,caGitem,					  //统计项目
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//起始时间日期
                                    "shour",UT_TYPE_STRING,2,caShour,						  //起始时间的小时
                                    "smin", UT_TYPE_STRING,2,caSmin,
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "ehour",UT_TYPE_STRING,2,caEhour,
                                    "emin", UT_TYPE_STRING,2,caEmin,
                                    "exppdf",UT_TYPE_STRING,8,caPdf,							//导出pdf
                                    "groupid",UT_TYPE_STRING,30,group_id,					//部门id
                                    "userid",UT_TYPE_STRING,14,user_Id,				    //用户id
                                    "groupname",UT_TYPE_STRING,30,gpname,					//标题中的部门名
                                    "dispname",UT_TYPE_STRING,30,dpname,				  //个人显示名
                                    "expexcel",UT_TYPE_STRING,10,expexcel,				//导出pdf	
                                    "rootname",UT_TYPE_STRING,30,rtname,
                                    "workflag",UT_TYPE_STRING,8,caWorkflag);				
  strcpy(caSdate_All,utTimFormat("%Y-%m-%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate))); 
  strcpy(caEdate_All,utTimFormat("%Y-%m-%d",utTimStrToLong("%Y/%m/%d %H:%M",caEdate)));
//  printf("**部门名称***expexcel=%s\n",expexcel);  
 //先将网络服务信息放到内存
    sprintf(caTemp,"select a.sid,a.groupid from ncipindex a ");
    if((atol(caGitem)!=100)&&(atol(caGitem)!=101)&&strlen(caGitem)>0){
    	sprintf(caTemp+strlen(caTemp)," where a.groupid=%d ",atol(caGitem));
    }
    sprintf(caTemp+strlen(caTemp)," order by a.sid limit 0,500 ",0);
    
   	 psCur=pasDbOpenSql(caTemp,0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
     }
     lGroupid=0;
     lSid=0;
     lSumindex=0;
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                    UT_TYPE_LONG,4,&lGroupid);
      while((iReturn==0)||(iReturn==1405))
      {
      	sService[lSumindex].lService=lSid;
      	sService[lSumindex].lGroupid=lGroupid;

      	sService[lSumindex].lBytes=0;
      	
      	lSumindex++;
      	lGroupid=0;
      	lSid=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                    UT_TYPE_LONG,4,&lGroupid);
      }                               
      pasDbCloseCursor(psCur); 
 
 
 //将服务大类信息放在sGservice 中
   	 psCur=pasDbOpenSql("select groupid,name from ncservicecgroup order by groupid ",0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
     }
     lSumGindex=0;
     
     lGroupid=0;
     memset(caName,0,sizeof(caName));
    
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      while((iReturn==0)||(iReturn==1405))
      {
      	sGservice[lSumGindex].lService=lGroupid;
      	strcpy(sGservice[lSumGindex].caGname,caName);
      	sGservice[lSumGindex].lBytes=0;
      	
      	lSumGindex++;
      	lGroupid=0;
        memset(caName,0,sizeof(caName));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      }                               
      pasDbCloseCursor(psCur); 
 

                     
  	psDbHead = utPltInitDb();
    if(strlen(caGitem)==0) strcpy(caGitem,"100");
   utPltPutVar(psDbHead,"workflag",caWorkflag);
 
   if((strlen(caSdate)==0)||(strlen(caEdate)==0)){
  	   strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime)); 
  	   strcpy(caEdate,utTimFormat("%Y/%m/%d",lTime));  	  
  	   strcpy(caShour,utTimFormat("%H",lTime-7200));
  	   strcpy(caSmin,utTimFormat("%M",lTime-7200));
  	   
  	   strcpy(caEhour,utTimFormat("%H",lTime));
  	   strcpy(caEmin,utTimFormat("%M",lTime));

  	  }
  	   sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
  	   sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
  	   lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
  	   lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
  	   
  	utPltPutVarF(psDbHead,"stime","%d",lStime);
  	utPltPutVarF(psDbHead,"etime","%d",lEtime);
  	
  	if(atol(caGitem)==100)
  	  utPltPutVar(psDbHead,"flag","2");
  	else
  	  utPltPutVar(psDbHead,"flag","1");
  	
    sprintf(caStatdate,"统计时间：从%s %s:%s 到%s %s:%s",	caSdate,caShour,caSmin,caEdate,caEhour,caEmin);	  
  	 utPltPutVar(psDbHead,"shour",caShour);
  	 utPltPutVar(psDbHead,"smin",caSmin);
  	 utPltPutVar(psDbHead,"ehour",caEhour);
  	 utPltPutVar(psDbHead,"emin",caEmin);
  	 utPltPutVar(psDbHead,"sdate",caSdate);
  	 utPltPutVar(psDbHead,"edate",caEdate);
  	 utPltPutVar(psDbHead,"gitem",caGitem);
 
 //获取ncipservice表中日期最大值 	这个值用于判断定时统计程序当前已经整理的服务数据，大于这个时间的没有整理，需要从IPLOG中获取 
  	 memset(caMaxdate,0,sizeof(caMaxdate));
  	 pasDbOneRecord("select max(sdate) from ncipservice ",0,UT_TYPE_STRING,10,caMaxdate);
  	 sprintf(caTemp,"select max(stime) from ncipservice where sdate='%s' ",caMaxdate);
  	 memset(caMaxtime,0,sizeof(caMaxtime));
  	 pasDbOneRecord(caTemp,0,UT_TYPE_STRING,2,caMaxtime);
  	 
  	 
  	 if(strlen(caMaxdate)>0&&(strlen(caMaxtime))){
  	 	 sprintf(caRectime,"%s %s",caMaxdate,caMaxtime);
  	 	 lRecenttime=utTimStrToLong("%Y/%m/%d %H",caRectime);
  	 }
  	 
  	 if(lEtime>lRecenttime){
 //统计IPlog中的服务
      if(lStime>lRecenttime){
      	lStime1=lStime;
      }
      else{
      	lStime1=lRecenttime;
      }
      sprintf(caTemp,"select sum(ubytes+dbytes),service from nciplog where vtime>%d and vtime<=%d ",lStime1,lEtime);
      	if(strlen(group_id)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and (groupid =%s or groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  	else if(strlen(user_Id)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and userid in (%s) ",user_Id);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  		 

//是否工作时间
if(atol(caWorkflag)==1){
	 strcpy(caWorksql,ncUtlCvtWorkTime2Sql("vtime"));
	 if(strlen(caWorksql)>0){
	 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql);
   }  
  }	  		 
  	  	sprintf(caTemp+strlen(caTemp),"  group by service limit 0,500");
      printf("**iplog**caTemp=%s\n",caTemp);
      psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lBytes=0;
       lService=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                     UT_TYPE_LONG,4,&lService);
       while((iReturn==0)||(iReturn==1405))
       {
       	index=ncUtlGetServicenumById(&sService,lSumindex,lService);
	   	  if(index!=-1){
	
	          sService[index].lBytes=sService[index].lBytes+lBytes;         
	      }
	     else{
//	     	if(lService>0)
//	     	   lOth=lOth+lBytes;
       }
      	
	       lBytes=0;
	       lService=0;
	       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                      UT_TYPE_LONG,4,&lService);
       }                               
      pasDbCloseCursor(psCur);                           
 
  	 }
  	  //统计ncIPSERVICE中的数据
  	  if(lStime<lRecenttime){           //统计开始时间在ncipservice 中的最大时间前面数据
  	  	if(strcmp(caSdate,caEdate)!=0){
  	  	    sprintf(caTemp,"select sum(ubytes+dbytes),a.service from ncipservice a,ncuser b where a.userid=b.userid and ((a.sdate>'%s' and a.sdate<'%s') or (a.sdate='%s' and a.stime>='%s') or (a.sdate='%s' and a.stime<='%s')) ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	  		  if(strlen(group_id)>0)
  	  		  	sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",group_id);
  	  		  else if(strlen(user_Id)>0)
  	  		    sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",user_Id);	
  	  		  else
  	  		  	sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  		  	
  	  		  	
  	  		  //是否工作时间
					if(atol(caWorkflag)==1){
						 strcpy(caWorksql2,ncUtlCvtWorkTime2Sql_s("a.sdate","a.stime"));
						 if(strlen(caWorksql2)>0){
						 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
					   } 	
  	  		  }	
  	  		  	
  	  		  	
  	  		  sprintf(caTemp+strlen(caTemp),"  group by service");
  	  	}else{
  	  		sprintf(caTemp,"select sum(ubytes+dbytes),a.service from ncipservice a,ncuser b where a.userid=b.userid and a.sdate='%s' and a.stime>='%s' and a.stime<='%s' ",caSdate,caShour,caEhour);
  	  		if(strlen(group_id)>0)
  	  		  	sprintf(caTemp+strlen(caTemp)," and (b.groupid =%s or b.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  		else if(strlen(user_Id)>0)
  	  		    sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",user_Id);	
  	  		  else
  	  		  	sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  		  	
  	  		//是否工作时间
					if(atol(caWorkflag)==1){
						 strcpy(caWorksql2,ncUtlCvtWorkTime2Sql_s("a.sdate","a.stime"));
						 if(strlen(caWorksql2)>0){
						 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
					   } 	
  	  		  }	  	
  	  		  	
  	  		  	
  	  		  	
  	  		sprintf(caTemp+strlen(caTemp),"  group by service");
  	  	}
  	 	printf("**ipservice**caTemp=%s\n",caTemp);
  	  psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lBytes=0;
       lService=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                     UT_TYPE_LONG,4,&lService);
       while((iReturn==0)||(iReturn==1405))
       {
       	index=ncUtlGetServicenumById(&sService,lSumindex,lService);			//2分查找
	   	  if(index!=-1){
	
	          sService[index].lBytes=sService[index].lBytes+lBytes;         
	      }
	     else{
	//     	if(lService>0)
	//     	   lOth=lOth+lBytes;
       }
      	
	       lBytes=0;
	       lService=0;
	       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                     UT_TYPE_LONG,4,&lService);
       }                               
      pasDbCloseCursor(psCur); 
  	  	
 	  	
  	  }
  lSj=0; 	 
  	 
  //输出服务大类	 
  	for(i=0;i<lSumGindex;i++){
      	utPltPutLoopVarF(psDbHead,"gid",i+1,"%d",sGservice[i].lService);
      	utPltPutLoopVar(psDbHead,"gname",i+1,sGservice[i].caGname);
      	

      }                               
     
  	
  	
  	if(atol(caGitem)==100){
  		//服务大类
  		strcpy(caTitle,ncLang("1829服务大类流量分布情况"));
  	      utPltPutVar(psDbHead,"title",ncLang("1808服务大类流量分布图"));
  	      utPltPutVar(psDbHead,"title_1",ncLang("1810服务大类流量分布表"));
  	      for(i=0;i<lSumindex;i++){
  	      	lSum=lSum+sService[i].lBytes;
  	      	index=ncUtlGetServicenumById(&sGservice,lSumGindex,sService[i].lGroupid);
  	      	if(index!=-1){
  	      		sGservice[index].lBytes=sGservice[index].lBytes+sService[i].lBytes;
  	        }
  	        else{
  	        	lOth=lOth+sService[i].lBytes;
  	        }
  	    }
 //排序 	
 
 
    
  	    qsort(sGservice,lSumGindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId); 
  	    iNum=0;
  	   
  	    for(i=0;i<lSumGindex;i++){
  	    	if(sGservice[i].lBytes>0){
  	    
  	    //输出到表格	 
  	    	if(i<10){
  	    		 utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF8(sGservice[i].lBytes/1024,caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	    		 utPltPutLoopVar(psDbHead,"name_1",  iNum+1,sGservice[i].caGname);
  	    		if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
    				}
  	    		} 
  	    //输出到图		  
  	    		 if(i<10) {
    		 	
  	    		  lSj=lSj+1;
  	    		 utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	sGservice[i].lBytes);  
             utPltPutLoopVar(psDbHead,"name",  iNum+1,sGservice[i].caGname);
             utPltPutLoopVarF(psDbHead,"groupid",iNum+1,"%d",sGservice[i].lService);
             utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",lTime);
             utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
           
            
     				if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
    				}
           }
            else{
            	lOth=lOth+sGservice[i].lBytes;
            }
            iNum++;
  	    	}
     }
     
    // 输出其它
     if(lOth>0){
     	  	   utPltPutLoopVarF(psDbHead,"bytes",lSj+1,"%llu",	lOth);  
             utPltPutLoopVar(psDbHead,"name",  lSj+1,ncLang("0431其它"));
             utPltPutLoopVarF(psDbHead,"groupid",lSj+1,"%d",999999);
             utPltPutLoopVarF(psDbHead,"ltime",lSj+1,"%d",lTime);
             utPltPutLoopVarF(psDbHead,"num",lSj+1,"%d",lSj);
             if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",lSj+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",lSj+1,"%0.2f",(float)100*lOth/lSum);  	  			
    				}             
     }
    }else {
  	 	if(atol(caGitem)==101){
  	 		   strcpy(caTitle,ncLang("1830"));
  	 	  	 utPltPutVar(psDbHead,"title",ncLang("1809细分服务流量分布图"));
  	 	  	 utPltPutVar(psDbHead,"title_1",ncLang("1811细分服务流量分布表"));
  	 	 }
  	 	 else{
  	 	 	sprintf(caTemp,"select name from ncservicecgroup where groupid=%d ",atol(caGitem));
  	 	 	memset(caName,0,sizeof(caName));
  	 	 	
  	 	 	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caName);
  	 	 	
  	 	 	sprintf(caTitle,ncLang("1831%s流量分布情况"),caName);
  	 	 	sprintf(caTemp,ncLang("1812%s流量分布图"),caName);
  	 	 	utPltPutVar(psDbHead,"title",caTemp);
  	 	 	
  	 	 	sprintf(caTemp,ncLang("1813%s流量分布表"),caName);
  	 	 	utPltPutVar(psDbHead,"title_1",caTemp);
  	 	 }
  	 	//排序
  	 	 	 qsort(sService,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId);  
 //求总流量
          lSum=0; 	 	 	 
  	 	 	  for(i=0;i<lSumindex;i++){
  	      	lSum=lSum+sService[i].lBytes; 	      	
  	       }
  	 	 	 
  	 	 	 iNum=0;
  	 	 	
  	 	 	 for(i=0;i<lSumindex;i++){
  	 	 	 	if(sService[i].lBytes>0){
  //	printf("i=%d\n",i); 	 	 		
  	 	 	 		  	    //输出到表格	 
  	 	 	 		 if(i<10){
  	    		 utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF8(sService[i].lBytes/1024,caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
 	    		   utPltPutLoopVar(psDbHead,"name_1",  iNum+1,ncUtlGetServiceName(psShmHead,sService[i].lService,ncLang("0431其它")));
  	    		 if(lSum==0){
    					 utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				 }
    				 else{
    	  			  utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*sService[i].lBytes/lSum);
    				 }
    				 }
  	 	 	 		 //输出到图		  
  	    		 if(i<10) {
  	    		  lSj=lSj+1;
  	    		 utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	sService[i].lBytes);  
             utPltPutLoopVar(psDbHead,"name",  iNum+1,ncUtlGetServiceName(psShmHead,sService[i].lService,ncLang("0431其它")));
             utPltPutLoopVarF(psDbHead,"groupid",iNum+1,"%d",sService[i].lService);
             utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",lTime);
             utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
           
            
     				if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*sService[i].lBytes/lSum);
    				 }
           }
            else{
            	lOth=lOth+sService[i].lBytes;
            }
            iNum++;
  	    	}
	 	 	 
  	 	  }
  	 	      // 输出其它
     if(lOth>0){

     	  	   utPltPutLoopVarF(psDbHead,"bytes",lSj+1,"%llu",	lOth);  
             utPltPutLoopVar(psDbHead,"name",  lSj+1,ncLang("0431其它"));
             utPltPutLoopVarF(psDbHead,"groupid",lSj+1,"%d",999999);
             utPltPutLoopVarF(psDbHead,"ltime",lSj+1,"%d",lTime);
             utPltPutLoopVarF(psDbHead,"num",lSj+1,"%d",lSj);
             if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",lSj+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",lSj+1,"%0.2f",(float)100*lOth/lSum);
    	  			//   	  			
    				}
  	 	  }
  	 	  	 
  	 }
  	 
  	if(strlen(caPdf)>0){
			//表格标签
	 		desc=malloc(3*sizeof(char*));
	 		for(i=0;i<3;i++){
	 			desc[i]=malloc(32);
   		}
		   strcpy(desc[0],ncLang("1647网络服务"));		  
		   sprintf(desc[1],"%s(%s)",ncLang("1103总流量"),"KB");
		   strcpy(desc[2],ncLang("1827百分比%"));
     
 		 //输出到PDF中     
   	  pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*12);
      if(pChart==NULL){
  	  	 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),"分配内存出错");
      	 return 0;
   	  } 
   	 iNum=0;
   
   	 for(i=0;i<lSj;i++){   	   
   	 	if(iNum<10){
   	 		if(atol(caGitem)==100){                                              //服务大类
   	 	  	pChart[iNum].name=strdup(sGservice[i].caGname);
  				pChart[iNum].amount=(float)sGservice[i].lBytes/1024; 	
  			 	if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
          	if(lSum==0){
   	 					sprintf(caTemp,"0.00");
   	 				}else{
   	 			  	sprintf(caTemp,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
   	 				}
 	 		
   	 				pChart[iNum].percent=strdup(caTemp); 
           	iNum++;
        	}else{      	
       			pChart[iNum].name=strdup(ncUtlGetServiceName(psShmHead,sService[i].lService,ncLang("0431其它")));                       //细分服务
  					pChart[iNum].amount=(float)sService[i].lBytes/1024; 	
  			 		if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
          	if(lSum==0){
   	 					sprintf(caTemp,"0.00");
   	 				}else{
   	 			  	sprintf(caTemp,"%0.2f",(float)100*sService[i].lBytes/lSum);
   	 				}
 	 		
   	 				pChart[iNum].percent=strdup(caTemp); 
            iNum++;       	
      		}   
     		}
  	 }
  	         // 输出其它
     if(lOth>0){
     		pChart[iNum].name=strdup(ncLang("0431其它"));  
     	   	if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lOth)/lSum);
   	 			}  	 			
     	   	pChart[iNum].percent=strdup(caTemp);  	 		
   	 			pChart[iNum].amount=(float)lOth/1024;                  
          iNum++;                                     
     	}
     
      rowrow=iNum;
   
    	sprintf(filename,"%s.PDF",caTitle); 
			printf("rowrow=%d\n",rowrow);
     	ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,3,0, caTitle,"单位:KB",caStatdate,filename); 
     
			//释放内存     
   		for(i=0;i<rowrow;i++){
	      free(pChart[i].name);
	      free(pChart[i].percent);	      
   		}  
   		if(pChart) free(pChart);
   
   		for(i=0;i<3;i++){
   	   	free(desc[i]);
   		}
   	  if(desc) free(desc);
   
      utPltFreeDb(psDbHead);
    
      return 0;
   }
   //导出excel
   long flag=0;
    if(strlen(expexcel)!=0){		
	 		if(strlen(gpname)!=0){
				strcpy(f_name,gpname);
				strcat(f_name,"-");
			}
			if(strlen(dpname)!=0){
				strcat(f_name,dpname);
				strcat(f_name,"-");
			}
			strcat(f_name,ncLang("1953网络服务流量分布情况查询--"));
			strcat(f_name,caSdate_All);
			strcat(f_name,"~");
			strcat(f_name,caEdate_All);	
  		strcat(f_name,".xls");
  		//sprintf(f_name,"ncsort%d.csv",utStrGetId());
  		char caPath[120];
  		char caFile[320];
		  sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
		  sprintf(caFile,"%s/%s",caPath,f_name);
  		fp = fopen(caFile,"w");
  		if(fp == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613网络流量统计"),ncLang("0546数据导出出错"));
            return 0;
        }
    	fprintf(fp,ncLang("1954序号\t网络服务\t流量(KB)\t百分比\n")); 
    	iNum=0; 
    if(atol(caGitem)==100){
    	for(i=0;i<lSumGindex;i++){
    		if(sGservice[i].lBytes>0){
  	    	if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}else{
   	 				sprintf(caTemp,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
   	 			}
    			fprintf(fp,"%d\t%s\t%s\t%s\n",iNum+1,sGservice[i].caGname,utStrLtoF8(sGservice[i].lBytes/1024,caTemp),strdup(caTemp));
    			iNum++;
    		}
    	}
    }else{		//小类
    			//排序
  	 	 	 qsort(sService,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId);  
 					//求总流量
          lSum=0; 	 	 	 
  	 	 	  for(i=0;i<lSumindex;i++){
  	      	lSum=lSum+sService[i].lBytes; 	      	
  	       }
  	 	 	 
  	 	 	 iNum=0;
  	 	 	 lSj=0; 	
  	 	 	 for(i=0;i<lSumindex;i++){
  	 	 	 	if(sService[i].lBytes>0){
  	 	 	 		  	    //输出到表格	 
  	 	 	 		 if(i<10){
  	 	 	 		 	 lSj=lSj+1;
	  	    		 if(lSum==0){
	    					 utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
	    					 fprintf(fp,"%d\t%s\t%s\t%s\n",iNum+1,ncUtlGetServiceName(psShmHead,sService[i].lService,ncLang("0431其它")),	utStrLtoF8(sService[i].lBytes/1024,caTemp),"0.00");            
	    				 }else{
	    				 		char aab[20]="";
	    				 		sprintf(aab,"%.2f",(float)100*sService[i].lBytes/lSum);
	    				 		
	    	  			  fprintf(fp,"%d\t%s\t%s\t%s\n",iNum+1,ncUtlGetServiceName(psShmHead,sService[i].lService,ncLang("0431其它")),	utStrLtoF8(sService[i].lBytes/1024,caTemp),aab);            
	    				 }
    				 }
  	 	 	 		 //fprintf(fp,"%d\t%s\t%s\t%s\n",iNum+1,ncUtlGetServiceName(psShmHead,sService[i].lService,ncLang("0431其它")),	utStrLtoF8(sService[i].lBytes/1024,caTemp),strdup(caTemp));
            iNum++;
  	    	}
	 	 	 
  	 	  }
  	 	   if(lOth>0){
             if(lSum==0){
    					fprintf(fp,"%d\t%s\t%llu\t%s\n",lSj+1,ncLang("0431其它"),lOth,"0.00");
    				}
    				else{
    					char aab[20]="";
	    				sprintf(aab,"%.2f",(float)100*lOth/lSum);
	    				
    					fprintf(fp,"%d\t%s\t%llu\t%s\n",lSj+1,ncLang("0431其它"),lOth,aab);
    				}
    		 }
    }
     flag=1;   
		 fclose(fp);
		 
		 //复制到upload 目录
		 /*
		 strcpy(cp_command,"cp  ");
		 strcat(cp_command,f_name);
		 strcat(cp_command,caPath);
		 system(cp_command); 
		 
		 */		 
  	 utPltFileDownload(iFd,"application/text",caPath,f_name,f_name);
  	 //删除bin目录下的生成文件
		 strcpy(rm_command,"rm ");
		 strcat(rm_command,caFile);
		 system(rm_command);
		 utPltFreeDb(psDbHead);
   }	
		
		if(flag!=1){
    	utPltPutVar(psDbHead,"groupname",gpname); 
			utPltPutVar(psDbHead,"dispname",dpname);  
			utPltPutVar(psDbHead,"groupid",group_id); 
			utPltPutVar(psDbHead,"userid",user_Id); 
			utPltPutVar(psDbHead,"rootname",rtname); 
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_dept/iframe_servicedeptbtcx.htm");  
    }
    return 0;
}
//显示某部门或某人某种服务的明细
int ncShowFirstServiceDepDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)                  
{
	  unsigned long urlid;
    char urlname[64];
    pasDbCursor *psCur;
    char caTemp[1055];
    char caTotPg[16];
    static int pp;
    int  i,iReturn,iNum,iMark1,iMark2,lSumUser,iPos;
    char caSdate[32],caUserid[32],caUsername[36],caCurPg[16],caUrl[200],caUrl0[200];
    char caSmt[512],caSip[16],caDip[16],caStime[12],caGroupid[32];
    char caUri[129],caHost[65],caTitle[128];
    char caCount[16];
    char caDel[7];
    char caDelwhere[500];
    char sqlbuf[1024];
    unsigned long lUserid,lStart,lSip;
    long lCount;
    int iTitleMark=0;
    long lCurPg,lRowNum,lStartRec,lTotPg,lPort,lGroupid0;
    char caSelect[256],caWhere[256];
    char caExport[16];
    ncUserCont *psUser;
    utPltDbHead *psDbHead;
    char caTitlename[32];
    char temp_group[32];
    char caWhereStr[10];
    char caEdate[20],caUrname[64],caName[64];
    unsigned long lCurtime,lScurtime;
    unsigned long lwtype;
    char caStime0[8],caEtime[8],byt_in[8];
    char caByt[16],caIdfyno[20],caUsername_match[8];
    char caUrlid[16],caUrlid2[16],caWebtitle[64];
    float lRat,group_flag;
    unsigned long lStatrow,lMintime,lMaxtime,lSum,lStarttime,lEndtime,lMaxin,lMinin,lIn,lVtime;
    long lStime,lEtime,lService,dport;
    unsigned long dip;
    char caService[256],caType[32],caDipPort[32],caDate[20];
    unsigned long lDip;
    long long lUbytes,lDbytes,lBytes;
	  long lConntime,lType;
	  char caToday[20];
	  long lFlags,lSid,lAtt,lSize,lCode;
	  char caFile[256],caFrom[64],caTo[64],caSub[128];
	  char caMesg[256];
	  long lFuncode,lTime;
		char grname[32]="";   
    char dpname[32]="";
    char group_id[15]="";
    char gaName[32]="";
    char indexsid[15]="";
		char index_sid[2009]="";
		char rtname[32]="";
		char Ggitem[12]="";
   	iReturn = utMsgGetSomeNVar(psMsgHead,12,"service",     UT_TYPE_STRING, 250, caService,
   	                                        "userid",      UT_TYPE_STRING, 10, caUserid,
   	                                        "stime",       UT_TYPE_STRING,10,caStime,
   	                                        "etime",       UT_TYPE_STRING,10,caEtime,
					                                  "CurPg",       UT_TYPE_STRING, 12, caCurPg,
					                                  "TotPg",       UT_TYPE_STRING, 12, caTotPg,
					                                  "groupname",   UT_TYPE_STRING,30,grname,					//标题中的部门名
                                            "dispname",    UT_TYPE_STRING,30,dpname,
                                            "groupid",     UT_TYPE_STRING,14,group_id,				//部门id
                                            "gname",       UT_TYPE_STRING,30,gaName,
                                            "rootname",    UT_TYPE_STRING,30,rtname,
                                            "gitem",    	 UT_TYPE_STRING,10,Ggitem);
			
		printf("pid=%d\n",getpid());
		printf("== caUserid=%s  ===\n",caUserid);	
		printf("== rootname=%s  ===\n",rtname);	   
		printf("== Ggitem=%s  ===\n",Ggitem);	  
		printf("== caService=%s  ===\n",caService);	  
    lUserid = atol(caUserid);
    
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
    	strcpy(caUsername,caTitlename);
    }
//    lStime=atol(caStime);
//    lEtime=atol(caEtime);
strcpy(caToday,utTimFormat("%Y-%m-%d 00:00:00",time(0)));
lStime=utTimStrToLong("%Y-%m-%d %H:%M:%S",caToday);
lEtime=time(0);
if(strstr(caService,",")==NULL)
     lService=atol(caService);
else
     lService=0;
    
    lCurPg = atol(caCurPg);
    lRowNum = 32;
    if(lCurPg <= 1){
    	 lCurPg = 1;
    	
    	}
    lStartRec = (lCurPg - 1) * lRowNum;
    if(lStartRec <= 0)lStartRec = 0;

   psDbHead = utPltInitDb();

 printf("=22=  到这里了吗lService==%d\n",lService);
    lCount=0;/*
    if(lService==3){
	    sprintf(caTemp,"select count(*) from nchttplog where vtime>=%d and vtime<=%d and userid=%d ",lStime,lEtime,lUserid);
	   
	    pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
   }
   else if(lService==4||lService==5){
   	 sprintf(caTemp,"select count(*) from ncmailmon where sdate>='%s' and userid=%d ",caToday,lUserid);
   	 pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
   }
   else if(lService==6||lService==10||lService==11||lService==21||lService==1004||lService==1011||lService==1007){
   	sprintf(caTemp,"select count(*) from nccontlog where stime>=%d and stime<=%d and userid=%d ",lStime,lEtime,lUserid);
   	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
  }
   else{*/
    lStime=atol(caStime);
    lEtime=atol(caEtime);
    if(strstr(caService,",")){
      sprintf(caTemp,"select count(*) from nciplog a,ncipindex b where a.vtime>=%d and a.vtime<=%d and a.service=b.sid ",lStime,lEtime);
    	if(strlen(group_id)>0){
    		sprintf(caTemp+strlen(caTemp)," and (a.groupid =%s or a.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
//  	  	sprintf(caTemp+strlen(caTemp)," and a.groupid =%s ",group_id);
  	  }
  	  else if(strlen(caUserid)>0)
  	  	sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");	  	
  	  if(strlen(caService)>0&&atol(Ggitem)==100)
  	  	 sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",caService); 	
  	  else
  	  	 sprintf(caTemp+strlen(caTemp)," and a.service =%s ",caService); 	  
    }else{
   		sprintf(caTemp,"select count(*) from nciplog a,ncipindex b where a.vtime>=%d and a.vtime<=%d and a.service=b.sid ",lStime,lEtime);
    	if(strlen(group_id)>0){
    	  sprintf(caTemp+strlen(caTemp)," and (a.groupid =%s or a.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
    	}
  //	  	sprintf(caTemp+strlen(caTemp)," and a.groupid =%s ",group_id);
  	  else if(strlen(caUserid)>0)
  	  	sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");	  	
  	  if(strlen(caService)>0&&atol(Ggitem)==100)
  	  	 sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",caService); 	
  	  else
  	  	 sprintf(caTemp+strlen(caTemp)," and a.service =%s ",caService); 		  
	  }
	   printf("**11**caTemp=%s\n",caTemp);
	    pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
  //}
    lTotPg = (lCount -1 ) / lRowNum + 1;
 
   
    utPltPutVarF(psDbHead,"stime","%d",lStime);
    utPltPutVarF(psDbHead,"etime","%d",lEtime);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVar(psDbHead,"userid",caUserid);  
    utPltPutVar(psDbHead,"groupname",grname); 
		utPltPutVar(psDbHead,"dispname",dpname); 	
		utPltPutVar(psDbHead,"groupid",group_id); 	
		utPltPutVar(psDbHead,"gname",gaName);  
		utPltPutVar(psDbHead,"rootname",rtname);    
    utPltPutVar(psDbHead,"title_name",caTitlename);
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"servicename",ncCvtSetService(psShmHead,atol(caService)));
   
    utPltPutVar(psDbHead,"gitem",Ggitem);
    utPltPutVar(psDbHead,"sdate",utTimFormat("%Y-%m-%d %H:%M:%S",lStime));
    utPltPutVar(psDbHead,"edate",utTimFormat("%Y-%m-%d %H:%M:%S",lEtime));
   
  /* 
    if(lService==3){
           psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.uri,b.host,b.title,b.sdate,b.stime,b.bytes,b.dip,b.dport,b.name,b.wtype \
		                from (select vtime,userid,sip,uri,host,title,sdate,stime,bytes,dip,dport,c.name,wtype from nchttplog left join ncwebclass c on (tid=c.id) where vtime>=%d and vtime<=%d ) b where b.userid=%d order by b.vtime desc limit %d,%d",lStime,lEtime,lUserid,lStartRec,32);
		            
       
		       
		    
		    if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
		        return 0;
		    }        
		
		    iReturn = 0;
		    
		        dip=0;
		        dport=0;
		        memset(caType,0,sizeof(caType));
		        lwtype=0;
		        lVtime=0;
		        lUserid=0;
		        memset(caUrl,0,sizeof(caUrl));
		        memset(caHost,0,sizeof(caHost));
		        memset(caTitle,0,sizeof(caTitle));
		        memset(caSdate,0,sizeof(caSdate));
		        memset(caStime,0,sizeof(caStime));
		        lBytes=0;
		   
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lVtime,
		    															 UT_TYPE_LONG,  4, &lUserid,
		                                   UT_TYPE_ULONG, 4, &lSip,
		                                   UT_TYPE_STRING,127,caUrl,
		                                   UT_TYPE_STRING,63,caHost,
		                                   UT_TYPE_STRING,127,caTitle,
		                                   UT_TYPE_STRING,12,caSdate,
		                                   UT_TYPE_STRING,8,caStime,
		                                   UT_TYPE_LONG8,  8, &lBytes,
		                                   UT_TYPE_ULONG,4,&dip,
		                                   UT_TYPE_LONG,4,&dport,
		                                   UT_TYPE_STRING,30,caType,
		                                   UT_TYPE_LONG,4,&lwtype);

		    iNum = 0;                                   
		    while(iReturn == 0||iReturn==1405)  {
		    	
		    	if(iNum<lRowNum){
		        iNum++;
		        psUser = ncUtlGetUserContById(psShmHead,lUserid);
		        if(psUser) {
		            utPltPutLoopVar(psDbHead,"dispname", iNum,psUser->dispname);
		            utPltPutLoopVar(psDbHead,"username", iNum,psUser->username);
		            if(psUser->psGroup) {
		                utPltPutLoopVar(psDbHead,"groupname", iNum,psUser->psGroup->groupname);
		            }
		        }
		        memset(caDipPort,0,sizeof(caDipPort));
		        if(dip!=0)
		        sprintf(caDipPort,"%s:%lu",utComHostIp(htonl(dip)),dport);
		        utPltPutLoopVar(psDbHead,"dip",iNum,caDipPort);
		        utPltPutLoopVar(psDbHead,"sip",      iNum,utComHostIp(htonl(lSip)));
		        sprintf(caDate,"%s %s",caSdate+5,caStime);
		        utPltPutLoopVar(psDbHead,"sdate",    iNum,caDate);
		        utPltPutLoopVar(psDbHead,"bytes",    iNum,utStrLtoF8(lBytes,caTemp));
		        sprintf(caUrl0,"http://%s%s",caHost,caUrl);
		        strcpy(caUrl,caUrl0);
		        caUrl[78]=0;
		        utPltSetCvtHtml(1);
		        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid); 
		        utPltPutLoopVar(psDbHead,"url",   iNum,caUrl);
		        utPltPutLoopVar(psDbHead,"urllnk",   iNum,caUrl0);
		        utPltPutLoopVar(psDbHead,"title",   iNum,caTitle);
		        ncLimGetWebNameByUrl(psShmHead,caHost,caType,caName);
		        utPltPutLoopVar(psDbHead,"urltype", iNum,caType);
		        }
		
		
		    utPltSetCvtHtml(0);
		       
		      		       
		        dip=0;
		        dport=0;
		        memset(caType,0,sizeof(caType));
		        lwtype=0;
		        lVtime=0;
		        lUserid=0;
		        memset(caUrl,0,sizeof(caUrl));
		        memset(caHost,0,sizeof(caHost));
		        memset(caTitle,0,sizeof(caTitle));
		        memset(caSdate,0,sizeof(caSdate));
		        memset(caStime,0,sizeof(caStime));
		        lBytes=0;
		   
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lVtime,
		    															 UT_TYPE_LONG,  4, &lUserid,
		                                   UT_TYPE_ULONG, 4, &lSip,
		                                   UT_TYPE_STRING,127,caUrl,
		                                   UT_TYPE_STRING,63,caHost,
		                                   UT_TYPE_STRING,127,caTitle,
		                                   UT_TYPE_STRING,12,caSdate,
		                                   UT_TYPE_STRING,8,caStime,
		                                   UT_TYPE_LONG8,  8, &lBytes,
		                                   UT_TYPE_ULONG,4,&dip,
		                                   UT_TYPE_LONG,4,&dport,
		                                   UT_TYPE_STRING,30,caType,
		                                   UT_TYPE_LONG,4,&lwtype);

		                                  
		    
		  }
		    pasDbCloseCursor(psCur);
		    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_showfirsthttplog.htm");
		    return 0;
		}
		else if(lService==4||lService==5){
		//	邮件
		
		    psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),b.flags,b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode \
                from  ncmailmon b  where b.sdate>='%s' and b.userid=%d order by b.sid desc limit %d,%d",caToday,lUserid,lStartRec,32);
		
          
		    
		    if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
		        return 0;
		    }        
		
		    iReturn = 0;
		    memset(caSip,0,sizeof(caSip));
		    lFlags=0;
		    lSid=0;
		    lUserid=0;
		    memset(caSdate,0,sizeof(caSdate));
		    memset(caFile,0,sizeof(caFile));
		    memset(caFrom,0,sizeof(caFrom));
		    memset(caTo,0,sizeof(caTo));
		    memset(caSub,0,sizeof(caSub));
		    lAtt=0;
		    lSize=0;
		    lCode=0;
		    
	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
  																 UT_TYPE_LONG,4,&lFlags,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,23,caSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lCode);

		    iNum = 0;                                   
		    while(iReturn == 0||iReturn==1405)  {
		    	
		    	if(iNum<lRowNum){
		        iNum++;
        utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
        if(lFlags==2)
        	utPltPutLoopVar(psDbHead,"flags_desc",iNum,"收邮件");
        else if(lFlags==1)
        	utPltPutLoopVar(psDbHead,"flags_desc",iNum,"发邮件");
        else if(lFlags==3)
          utPltPutLoopVar(psDbHead,"flags_desc",iNum,"WebMail");
         
        psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
        if(psUser){
        	utPltPutLoopVar(psDbHead,"position",iNum,psUser->position);
        }   
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVar(psDbHead,"file", iNum,caFile);
        
        utPltSetCvtHtml(1); 
        utPltPutLoopVar(psDbHead,"from", iNum,pasCvtGBK(lCode,caFrom,caTemp,128));
        utPltPutLoopVar(psDbHead,"to", iNum,pasCvtGBK(lCode,caTo,caTemp,128));
        ncUtlStrCvtCtl(caSub);
        utPltPutLoopVar(psDbHead,"sub", iNum,pasCvtGBK(lCode,caSub,caTemp,255));
        utPltSetCvtHtml(0); 
        utPltPutLoopVarF(psDbHead,"size", iNum,"%lu",lSize);
        utPltPutLoopVarF(psDbHead,"ccode", iNum,"%lu",lCode);
        if(lAtt > 0) {
            utPltPutLoopVar(psDbHead,"att", iNum,ncLang("0815有"));
        }
		      }
		      		       
		    memset(caSip,0,sizeof(caSip));
		    lFlags=0;
		    lSid=0;
		    lUserid=0;
		    memset(caSdate,0,sizeof(caSdate));
		    memset(caFile,0,sizeof(caFile));
		    memset(caFrom,0,sizeof(caFrom));
		    memset(caTo,0,sizeof(caTo));
		    memset(caSub,0,sizeof(caSub));
		    lAtt=0;
		    lSize=0;
		    lCode=0;
		    
	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
  																 UT_TYPE_LONG,4,&lFlags,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,23,caSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lCode);

		                                  
		    
		  }
		    pasDbCloseCursor(psCur);
		    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_showfirstmaillog.htm");
		    return 0;
		}		
		else if(lService==6||lService==10||lService==11||lService==21||lService==1004||lService==1011||lService==1007){
			
			printf("select inet_ntoa(b.sip),b.sid,b.userid,b.stime,b.service,b.mfrom,b.mto,b.mesg,b.funcode \
                from nccontlog b where b.stime>=%d and b.stime<=%d and b.userid=%d order by b.stime desc limit %d,32\n",lStime,lEtime,lUserid,lStartRec);
			
			 psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),b.sid,b.userid,b.stime,b.service,b.mfrom,b.mto,b.mesg,b.funcode \
                from nccontlog b where b.stime>=%d and b.stime<=%d and b.userid=%d order by b.stime desc limit %d,32",lStime,lEtime,lUserid,lStartRec);
       if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
		        return 0;
		    }  
		 memset(caMesg,0,sizeof(caMesg));
    memset(caTo,0,sizeof(caTo));
    memset(caFrom,0,sizeof(caFrom));
    memset(caSip,0,sizeof(caSip));
    lFuncode=0;
    iNum=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_LONG,4,&lService,
  	                               UT_TYPE_STRING,47,caFrom,
  	                               UT_TYPE_STRING,47,caTo,
  	                               UT_TYPE_STRING,255,caMesg,
  	                               UT_TYPE_LONG,4,&lFuncode);
		   	while(iReturn == 0||iReturn==1405){
		   		iNum++;
		   		    	utPltPutLoopVar(psDbHead,"service", iNum,ncUtlImName(lService));
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%lu",i);
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%m-%d %H:%M:%S",lTime));
        utPltPutLoopVar(psDbHead,"from", iNum,caFrom);
        utPltPutLoopVar(psDbHead,"to", iNum,caTo);
        if(lFuncode==1)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"登录");
        else if(lFuncode==2)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"退出");
        else if(lFuncode==3)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"发送消息");
        else if(lFuncode==4)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"发送文件");
        else if(lFuncode==5)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"接收文件");
        else if(lFuncode==6)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"接收消息");
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
		    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_showfirstimlog.htm");
		   	        
		}
		else {*/
			//iplog							
			if(strstr(caService,",")){
				sprintf(caTemp,"select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype,b.groupid \
          from (select vtime,userid,sip,dip,sdate,stime,ubytes,dbytes,conntime,service,port,trantype,groupid from nciplog where vtime>=%d and vtime<=%d ) b,ncipindex a where   b.service=a.sid ",lStime,lEtime);
				if(strlen(group_id)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",group_id);
  	  	else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and b.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  	if(strlen(caService)>0&&atol(Ggitem)==100)
  	  	 sprintf(caTemp+strlen(caTemp)," and a.groupid =%s ",caService); 	
  	  	else
  	  	 sprintf(caTemp+strlen(caTemp)," and b.service =%s ",caService); 
  	  		  sprintf(caTemp+strlen(caTemp)," order by b.vtime desc limit %d,%d ",lStartRec,lRowNum);
	
			}else{
		    sprintf(caTemp,"select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype,b.groupid \
          from (select vtime,userid,sip,dip,sdate,stime,ubytes,dbytes,conntime,service,port,trantype,groupid from nciplog where vtime>=%d and vtime<=%d ) b,ncipindex a  where   b.service=a.sid ",lStime,lEtime);     
	      if(strlen(group_id)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",group_id);
  	  	else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and b.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	    if(strlen(caService)>0&&atol(Ggitem)==100)
  	  	 sprintf(caTemp+strlen(caTemp)," and a.groupid =%s ",caService); 	
  	  	else
  	  	 sprintf(caTemp+strlen(caTemp)," and b.service =%s ",caService); 
  	  		  sprintf(caTemp+strlen(caTemp)," order by b.vtime desc limit %d,%d ",lStartRec,lRowNum);
		  }
		  printf("**222**caTemp=%s\n",caTemp);
		    psCur=pasDbOpenSql(caTemp,0);
		    if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
		        return 0;
		    }        
		
		    iReturn = 0;
		    
		        lVtime=0;
		        lUserid=0;
		        lSip=0;
		        lDip=0;
		        memset(caSdate,0,sizeof(caSdate));
		        memset(caStime,0,sizeof(caStime));
		        lUbytes=0;
		        lDbytes=0;
		        lConntime=0;
		        lService=0;
		        lPort=0;
		        lType=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lVtime,
    															 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG8,  8, &lUbytes,
                                   UT_TYPE_LONG8,  8, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
		    iNum = 0;                                   
		    while(iReturn == 0||iReturn==1405)  {
		    	
		    	if(iNum<lRowNum){
		    		iNum++;
        utPltPutLoopVar(psDbHead,"dispname", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltSetCvtHtml(0);  
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid);  
        utPltPutLoopVar(psDbHead,"sip",      iNum,utComHostIp(htonl(lSip)));
        utPltPutLoopVar(psDbHead,"dip",      iNum,utComHostIp(htonl(lDip)));
        utPltPutLoopVar(psDbHead,"sdate",    iNum,caSdate);
        utPltPutLoopVar(psDbHead,"stime",    iNum,caStime);
        utPltPutLoopVar(psDbHead,"conn",    iNum,utStrLtoF(lConntime,caTemp));
        utPltPutLoopVar(psDbHead,"ubytes",  iNum,utStrLtoF8(lUbytes,caTemp));
        utPltPutLoopVar(psDbHead,"dbytes",   iNum,utStrLtoF8(lDbytes,caTemp));
        if(lService == 0) {
            utPltPutLoopVar(psDbHead,"service",  iNum,"&nbsp;&nbsp;");
        }
        else {
            utPltPutLoopVar(psDbHead,"service",  iNum,ncCvtSetService(psShmHead,lService));
        }
        utPltPutLoopVarF(psDbHead,"dport",    iNum,"%lu",lPort);
        if(lType == 6) {
            utPltPutLoopVar(psDbHead,"prot",   iNum,"TCP");
        }
        else {
            utPltPutLoopVar(psDbHead,"prot",   iNum,"UDP");
        } 
		     }  
		        lVtime=0;
		        lUserid=0;
		        lSip=0;
		        lDip=0;
		        memset(caSdate,0,sizeof(caSdate));
		        memset(caStime,0,sizeof(caStime));
		        lUbytes=0;
		        lDbytes=0;
		        lConntime=0;
		        lService=0;
		        lPort=0;
		        lType=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lVtime,
    															 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG8,  8, &lUbytes,
                                   UT_TYPE_LONG8,  8, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
		       
		  }
		    pasDbCloseCursor(psCur);		  	
		    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_dept/nc_showdeptiplog.htm");			
		//}    
    return 0;
}



/* 邮件拦截统计*/
int ncWebShowMailCtlBt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{   
    pasDbCursor *psCur;
    char sqlbuf[2024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
    char serv_name[32];
    long lService,iNum;
    long long lSum;
    char caDate[20];
    char caDate_end[20];
    int iReturn,i;
    char caRefresh_flag[8];

    long long lBytes_other;
    char caName[32];
    long lGroupid;
    char preflag[8],nextflag[8],ctime[12];
    char caServ_flag[12];
    char caDate_sh[16],caDate_sm[16],caDate_eh[16],caDate_em[16];
    char caGitem[16],caSdate[16],caShour[8],caSmin[8],caEdate[16],caEhour[8],caEmin[8];
    char caSdate_all[20],caEdate_all[20];
    long lStime,lEtime,lRecenttime,lStime1,lSid;
    long lSumindex;
    long lSumGindex;
    char caMaxdate[16],caMaxtime[12],caRectime[16],caTemp[1024];
    long long lOth=0;
    long index;
    long lSj;
    char caWorksql[1024],caWorksql2[1024];
//iReturn=ncMailAttachSend("aa@pronetway.com","lyf@pronetway.com","test","test","/home/ncmysql/nc/bin/aa.PDF","reprot");

//printf("iReturn=%d\n",iReturn);
    
     struct piechart_datasetf *pChart;           //用于PDF文档输出
//   char *desc[]={"用户名","部门","流量","百分比"};
    int rowrow;                     
    int column; 
    char **pTemp;
    char caTitle[128];
    char filename[128];
    int m;
    column=4;
    long lDate;
    char **desc;
    char caPdf[16];
    char caStatdate[128];
    char user_Id[15]="";
    char dpname[32]="";
    char gpname[32]="";
    char rtname[32]="";
    char expexcel[12]="";
    char f_name[120]="";
    char cp_command[320]="";
  	char rm_command[320]="";
  	char caSdate_All[20],caEdate_All[20];
  	char caWorkflag[12];
  	FILE *fp; 
  	char group_id[32]="";
  	
  	unsigned char *pHash;
  	pasLHashInfo  sHashInfo;
  	char caMailfrom[64],caMailto[64],caKey[64];
  	struct ncSflow_s *psData;
  	long lStat,lSumservice;
  	char *p;
  	struct ncSflow_s stService[2000];
   lTime=time(0);
   lSum=0;
   lRecenttime=0;
   
      utMsgGetSomeNVar(psMsgHead,15,"gitem",UT_TYPE_STRING,10,caGitem,					  //统计项目
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//起始时间日期
                                    "shour",UT_TYPE_STRING,2,caShour,						  //起始时间的小时
                                    "smin", UT_TYPE_STRING,2,caSmin,
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "ehour",UT_TYPE_STRING,2,caEhour,
                                    "emin", UT_TYPE_STRING,2,caEmin,
                                    "exppdf",UT_TYPE_STRING,8,caPdf,							//导出pdf
                                    "groupid",UT_TYPE_STRING,30,group_id,					//部门id
                                    "userid",UT_TYPE_STRING,14,user_Id,				    //用户id
                                    "groupname",UT_TYPE_STRING,30,gpname,					//标题中的部门名
                                    "dispname",UT_TYPE_STRING,30,dpname,				  //个人显示名
                                    "expexcel",UT_TYPE_STRING,10,expexcel,				//导出pdf	
                                    "rootname",UT_TYPE_STRING,30,rtname,
                                    "workflag",UT_TYPE_STRING,8,caWorkflag);	
                                    
      if((strlen(caSdate)==0)||(strlen(caEdate)==0)){
  	   strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime)); 
  	   strcpy(caEdate,utTimFormat("%Y/%m/%d",lTime));  	  
  	   strcpy(caShour,utTimFormat("%H",lTime-72000));
  	   strcpy(caSmin,utTimFormat("%M",lTime-72000));
  	   
  	   strcpy(caEhour,utTimFormat("%H",lTime));
  	   strcpy(caEmin,utTimFormat("%M",lTime));

  	  }                          			
   	   sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
  	   sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
  	   lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
  	   lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);



if(strlen(caGitem)==0) strcpy(caGitem,"100");
 
    	pHash = (unsigned char *)pasLHashInit(2000,2000,sizeof(struct ncSflow_s),272,64); 
			if(pHash == NULL) {
		        printf(" Memory HttpHash Error \n");
		        return (-1);
		  }

      sprintf(caTemp,"select mailfrom,mailto,mailstat from ncmailctl where 1=1 ");
      	if(strlen(group_id)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where groupid =%s )",group_id);
  	  	if(strlen(user_Id)>0){
  	  		 sprintf(caTemp+strlen(caTemp)," and userid =%s ",user_Id);	
  	  	}
  	  	
  	  	sprintf(caTemp+strlen(caTemp)," and sdate>=%d and sdate<=%d ",lStime,lEtime);
  	  	
  printf("caTemp=%s\n",caTemp);    
      psCur=pasDbOpenSql(caTemp,0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
     }
     memset(caMailfrom,0,sizeof(caMailfrom));
     memset(caMailto,0,sizeof(caMailto));
     lStat=0;
		 iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,63,caMailfrom,
		                                UT_TYPE_STRING,63,caMailto,
		                                UT_TYPE_LONG,4,&lStat);
		                                   
		 while((iReturn==0)||(iReturn==1405))
		 {
		 	 
		 	 	if(atol(caGitem)==100){                        //发件邮箱
			 	p=strstr(caMailfrom,"@");
			 	if(p){
			 		memset(caKey,0,sizeof(caKey));
			 		strcpy(caKey,p+1);
//			 		printf("caKey1=%s\n",caKey);
			 		psData = (struct ncSflow_s *)pasLHashLookA(pHash,caKey);
			 		if(psData){
			 			psData->lService++;
			 		}
			 	}
			 		
		 	}
		 	else if(atol(caGitem)==101){                      //收件邮箱
		 		p=strstr(caMailto,"@");
			 	if(p){
			 		memset(caKey,0,sizeof(caKey));
			 		strcpy(caKey,p+1);
//			 		printf("caKey=%s\n",caKey);
			 		psData = (struct ncSflow_s *)pasLHashLookA(pHash,caKey);
			 		if(psData){
			 			psData->lService++;
			 		}
			 	}
			}
			 else if(atol(caGitem)==102){ 
			 	memset(caKey,0,sizeof(caKey));                     //状态
			 	if(lStat==1){
			 		strcpy(caKey,"正常发送");
			 	}
			 	else if(lStat==2){
			 		strcpy(caKey,"拒绝发送");
			 	}
			 	else if(lStat==3){
			 		strcpy(caKey,"转发");
			 	}
			 	else if(lStat==4){
			 		strcpy(caKey,"待审计");
			 	}
			 	else if(lStat==5){
			 		strcpy(caKey,"已审计");
			 	}
			 	else{
			 		strcpy(caKey,"其它");
			 	}
//			 	printf("caKey=%s\n",caKey);
			 		psData = (struct ncSflow_s *)pasLHashLookA(pHash,caKey);
			 		if(psData){
			 			psData->lService++;
			 		}
			 }
		 	
		
	     memset(caMailfrom,0,sizeof(caMailfrom));
	     memset(caMailto,0,sizeof(caMailto));
	     lStat=0;
			 iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,63,caMailfrom,
			                                UT_TYPE_STRING,63,caMailto,
			                                UT_TYPE_LONG,4,&lStat);
		 }
     pasDbCloseCursor(psCur); 


    psData= (struct ncSflow_s *)pasLHashFirst(pHash,&sHashInfo);
   lSumservice=0;
   while(psData&&lSumservice<2000) {
   stService[lSumservice].lService=psData->lService;   
   strcpy(stService[lSumservice].caGname,psData->caGname);
	
   	lSumservice++;
   	psData = (struct ncSflow_s *)pasLHashNextS(&sHashInfo);
   }
   free(pHash);

    if(lSumservice>0){
       qsort(&stService,lSumservice,sizeof(struct ncSflow_s),ncUtlSortOnServiceId3);
    }
    
  // for(i=0;i<lSumservice;i++){
  //  	printf("i=%d,service=%d,gname=%s\n",i,stService[i].lService,stService[i].caGname);
 //   }

 

     

 

                     
  	psDbHead = utPltInitDb();
  
 
   if((strlen(caSdate)==0)||(strlen(caEdate)==0)){
  	   strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime)); 
  	   strcpy(caEdate,utTimFormat("%Y/%m/%d",lTime));  	  
  	   strcpy(caShour,utTimFormat("%H",lTime-7200));
  	   strcpy(caSmin,utTimFormat("%M",lTime-7200));
  	   
  	   strcpy(caEhour,utTimFormat("%H",lTime));
  	   strcpy(caEmin,utTimFormat("%M",lTime));

  	  }
  	   sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
  	   sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
  	   lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
  	   lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
  	   
  	utPltPutVarF(psDbHead,"stime","%d",lStime);
  	utPltPutVarF(psDbHead,"etime","%d",lEtime);
  	
  	
    sprintf(caStatdate,"统计时间：从%s %s:%s 到%s %s:%s",	caSdate,caShour,caSmin,caEdate,caEhour,caEmin);	  
  	 utPltPutVar(psDbHead,"shour",caShour);
  	 utPltPutVar(psDbHead,"smin",caSmin);
  	 utPltPutVar(psDbHead,"ehour",caEhour);
  	 utPltPutVar(psDbHead,"emin",caEmin);
  	 utPltPutVar(psDbHead,"sdate",caSdate);
  	 utPltPutVar(psDbHead,"edate",caEdate);
  	 utPltPutVar(psDbHead,"gitem",caGitem);
 
	  
  lSj=0; 	 
  	   	
  	

  		//服务大类
  		strcpy(caTitle,"邮件拦截数量分布情况");
  	      utPltPutVar(psDbHead,"title","邮件拦截数量分布图");
  	      utPltPutVar(psDbHead,"title_1","邮件拦截数量分布表");
  	     
  	
  	    lSumGindex=0;
  	    lOth=0;
  	    lSum=0;
        for(i=0;i<lSumservice;i++){
        
        	 if(i<20)
        	 {
        	 	lSumGindex++;}
  	      	lSum=lSum+stService[i].lService;
  	      	if(i>20){
  	        lOth=lOth+stService[i].lService;
  	         }
  	     }

  	    iNum=0;
  	   
  	    for(i=0;i<lSumGindex;i++){
  	    	if(stService[i].lService>0){
  	    
  	    //输出到表格	 
  	    	if(i<20){

  	    		 utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF(stService[i].lService,caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	    		 utPltPutLoopVar(psDbHead,"name_1",  iNum+1,stService[i].caGname);
  	    		if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*stService[i].lService/lSum);
    				}
  	    		} 
  	    //输出到图		  
  	    		 if(i<20) {
    		 	
  	    		  lSj=lSj+1;
  	    		 utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%lu",	stService[i].lService);  
             utPltPutLoopVar(psDbHead,"name",  iNum+1,stService[i].caGname);
             utPltPutLoopVarF(psDbHead,"groupid",iNum+1,"%s",stService[i].caGname);
             utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",0);
             utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
           
            
     				if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*stService[i].lService/lSum);
    				}
           }
            else{
            	lOth=lOth+stService[i].lService;
            }
            iNum++;
  	    	}
     }
     
    // 输出其它
     if(lOth>0){
     	  	   utPltPutLoopVarF(psDbHead,"bytes",lSj+1,"%llu",	lOth);  
             utPltPutLoopVar(psDbHead,"name",  lSj+1,ncLang("0431其它"));
             utPltPutLoopVarF(psDbHead,"groupid",lSj+1,"%d",999999);
             utPltPutLoopVarF(psDbHead,"ltime",lSj+1,"%d",0);
             utPltPutLoopVarF(psDbHead,"num",lSj+1,"%d",lSj);
             if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",lSj+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",lSj+1,"%0.2f",(float)100*lOth/lSum);  	  			
    				}             
    
  	 	  	 
  	 }
  	 
  	if(strlen(caPdf)>0){
			//表格标签
	 		desc=malloc(3*sizeof(char*));
	 		for(i=0;i<3;i++){
	 			desc[i]=malloc(32);
   		}
		   strcpy(desc[0],"邮箱名");		  
		   sprintf(desc[1],"%s(%s)","数量","封");
		   strcpy(desc[2],ncLang("1827百分比%"));
     
 		 //输出到PDF中     
   	  pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*12);
      if(pChart==NULL){
  	  	 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),"分配内存出错");
      	 return 0;
   	  } 
   	 iNum=0;
   
   	 for(i=0;i<lSj;i++){   	   
   	 	if(iNum<20){
                                             //服务大类
   	 	  	pChart[iNum].name=strdup(stService[i].caGname);
  				pChart[iNum].amount=(float)stService[i].lService; 	
  			 	if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
          	if(lSum==0){
   	 					sprintf(caTemp,"0.00");
   	 				}else{
   	 			  	sprintf(caTemp,"%0.2f",(float)100*stService[i].lService/lSum);
   	 				}
 	 		
   	 				pChart[iNum].percent=strdup(caTemp); 
           	iNum++;
        	
     		}
  	 }
  	         // 输出其它
     if(lOth>0){
     		pChart[iNum].name=strdup(ncLang("0431其它"));  
     	   	if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lOth)/lSum);
   	 			}  	 			
     	   	pChart[iNum].percent=strdup(caTemp);  	 		
   	 			pChart[iNum].amount=(float)lOth/1024;                  
          iNum++;                                     
     	}
     
      rowrow=iNum;
   
    	sprintf(filename,"%s.PDF",caTitle); 

			
     	ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,3,0, caTitle,"单位:封",caStatdate,filename); 
     
			//释放内存     
   		for(i=0;i<rowrow;i++){
	      free(pChart[i].name);
	      free(pChart[i].percent);	      
   		}  
   		if(pChart) free(pChart);
   
   		for(i=0;i<3;i++){
   	   	free(desc[i]);
   		}
   	  if(desc) free(desc);
   
      utPltFreeDb(psDbHead);
    
      return 0;
   }

		

    	utPltPutVar(psDbHead,"groupname",gpname); 
			utPltPutVar(psDbHead,"dispname",dpname);  
			utPltPutVar(psDbHead,"groupid",group_id); 
			utPltPutVar(psDbHead,"userid",user_Id); 
			utPltPutVar(psDbHead,"rootname",rtname); 
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/iframe_mailctlbt.htm");  
  
    return 0;
}



/* 个人部门邮件拦截统计*/
int ncWebShowMailCtlBmUserBt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{   
    pasDbCursor *psCur;
    char sqlbuf[2024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
    char serv_name[32];
    long lService,iNum;
    long long lSum;
    char caDate[20];
    char caDate_end[20];
    int iReturn,i;
    char caRefresh_flag[8];

    long long lBytes_other;
    char caName[32];
    long lGroupid;
    char preflag[8],nextflag[8],ctime[12];
    char caServ_flag[12];
    char caDate_sh[16],caDate_sm[16],caDate_eh[16],caDate_em[16];
    char caGitem[16],caSdate[16],caShour[8],caSmin[8],caEdate[16],caEhour[8],caEmin[8];
    char caSdate_all[20],caEdate_all[20];
    long lStime,lEtime,lRecenttime,lStime1,lSid;
    long lSumindex;
    long lSumGindex;
    char caMaxdate[16],caMaxtime[12],caRectime[16],caTemp[1024];
    long long lOth=0;
    long index;
    long lSj;
    char caWorksql[1024],caWorksql2[1024];
//iReturn=ncMailAttachSend("aa@pronetway.com","lyf@pronetway.com","test","test","/home/ncmysql/nc/bin/aa.PDF","reprot");

//printf("iReturn=%d\n",iReturn);
    
     struct piechart_datasetf *pChart;           //用于PDF文档输出
//   char *desc[]={"用户名","部门","流量","百分比"};
    int rowrow;                     
    int column; 
    char **pTemp;
    char caTitle[128];
    char filename[128];
    int m;
    column=4;
    long lDate;
    char **desc;
    char caPdf[16];
    char caStatdate[128];
    char user_Id[15]="";
    char dpname[32]="";
    char gpname[32]="";
    char rtname[32]="";
    char expexcel[12]="";
    char f_name[120]="";
    char cp_command[320]="";
  	char rm_command[320]="";
  	char caSdate_All[20],caEdate_All[20];
  	char caWorkflag[12];
  	FILE *fp; 
  	char group_id[32]="";
  	
  	unsigned char *pHash;
  	pasLHashInfo  sHashInfo;
  	char caMailfrom[64],caMailto[64],caKey[64];
  	struct ncSflow_s *psData;
  	long lStat,lSumservice;
  	char *p;
  	struct ncSflow_s stService[2000];
  	long lUserid;
  	char caDispname[32],caGroupname[32];
  	char caEmail[64];
   lTime=time(0);
   lSum=0;
   lRecenttime=0;
   
      utMsgGetSomeNVar(psMsgHead,15,"gitem",UT_TYPE_STRING,10,caGitem,					  //统计项目
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//起始时间日期
                                    "shour",UT_TYPE_STRING,2,caShour,						  //起始时间的小时
                                    "smin", UT_TYPE_STRING,2,caSmin,
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "ehour",UT_TYPE_STRING,2,caEhour,
                                    "emin", UT_TYPE_STRING,2,caEmin,
                                    "exppdf",UT_TYPE_STRING,8,caPdf,							//导出pdf
                                    "groupid",UT_TYPE_STRING,30,group_id,					//部门id
                                    "userid",UT_TYPE_STRING,14,user_Id,				    //用户id
                                    "groupname",UT_TYPE_STRING,30,gpname,					//标题中的部门名
                                    "dispname",UT_TYPE_STRING,30,dpname,				  //个人显示名
                                    "expexcel",UT_TYPE_STRING,10,expexcel,				//导出pdf	
                                    "rootname",UT_TYPE_STRING,30,rtname,
                                    "workflag",UT_TYPE_STRING,8,caWorkflag);	
                                    
      if((strlen(caSdate)==0)||(strlen(caEdate)==0)){
  	   strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime)); 
  	   strcpy(caEdate,utTimFormat("%Y/%m/%d",lTime));  	  
  	   strcpy(caShour,utTimFormat("%H",lTime-72000));
  	   strcpy(caSmin,utTimFormat("%M",lTime-72000));
  	   
  	   strcpy(caEhour,utTimFormat("%H",lTime));
  	   strcpy(caEmin,utTimFormat("%M",lTime));

  	  }                          			
   	   sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
  	   sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
  	   lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
  	   lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);

      if(strlen(group_id)==0&&strlen(user_Id)==0&&strlen(caGitem)==0){
      	strcpy(caGitem,"101");
      }
      else if(strlen(group_id)>0||strlen(user_Id)>0){
      	strcpy(caGitem,"100");
      }
      else if(strlen(caGitem)==0){
      	strcpy(caGitem,"100");
      }


 
    	pHash = (unsigned char *)pasLHashInit(2000,2000,sizeof(struct ncSflow_s),272,64); 
			if(pHash == NULL) {
		        printf(" Memory HttpHash Error \n");
		        return (-1);
		  }

      sprintf(caTemp,"select a.userid,b.dispname,b.groupid,c.groupname,b.email from ncmailctl a,ncuser b,ncgroup c where a.userid=b.userid and b.groupid=c.groupid ");
      	if(strlen(group_id)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (select userid from ncuser where groupid =%s )",group_id);
  	  	if(strlen(user_Id)>0){
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid =%s ",user_Id);	
  	  	}
  	  	
  	  	sprintf(caTemp+strlen(caTemp)," and a.sdate>=%d and a.sdate<=%d ",lStime,lEtime);
  	  	
  printf("caTemp=%s\n",caTemp);    
      psCur=pasDbOpenSql(caTemp,0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
     }
     lUserid=0;
     lGroupid=0;
     memset(caGroupname,0,sizeof(caGroupname));
     memset(caDispname,0,sizeof(caDispname));
     memset(caEmail,0,sizeof(caEmail));
		 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
		                                UT_TYPE_STRING,30,caDispname,
		                                UT_TYPE_LONG,4,&lGroupid,
		                                UT_TYPE_STRING,30,caGroupname,
		                                UT_TYPE_STRING,63,caEmail);
		                                   
		 while((iReturn==0)||(iReturn==1405))
		 {
		 	 
		 	 	if(atol(caGitem)==100){                        //用户
			 	
			 		memset(caKey,0,sizeof(caKey));
			 		if(strlen(caEmail)>0){
			 			strcpy(caKey,caEmail);
			 		}
			 		else{
			 				strcpy(caKey,caDispname);
			 	  }

			 		psData = (struct ncSflow_s *)pasLHashLookA(pHash,caKey);
			 		if(psData){
			 			psData->lService++;
			 			
			 		}
			 	
		 	}
		 	else if(atol(caGitem)==101){                      //部门

			 		memset(caKey,0,sizeof(caKey));
			 		strcpy(caKey,caGroupname);
			 		printf("caKey=%s\n",caKey);
			 		psData = (struct ncSflow_s *)pasLHashLookA(pHash,caKey);
			 		if(psData){
			 			psData->lService++;
	         }
			}

		 	
		
     lUserid=0;
     lGroupid=0;
     memset(caGroupname,0,sizeof(caGroupname));
     memset(caDispname,0,sizeof(caDispname));
     memset(caEmail,0,sizeof(caEmail));
		 iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
		                                UT_TYPE_STRING,30,caDispname,
		                                UT_TYPE_LONG,4,&lGroupid,
		                                UT_TYPE_STRING,30,caGroupname,
		                                UT_TYPE_STRING,63,caEmail);
		 }
     pasDbCloseCursor(psCur); 


    psData= (struct ncSflow_s *)pasLHashFirst(pHash,&sHashInfo);
   lSumservice=0;
   while(psData&&lSumservice<2000) {
   stService[lSumservice].lService=psData->lService;   
   strcpy(stService[lSumservice].caGname,psData->caGname);
// printf("lcount=%d,gname=%s\n",stService[lSumservice].lService,stService[lSumservice].caGname); 	
   	lSumservice++;
   	psData = (struct ncSflow_s *)pasLHashNextS(&sHashInfo);
   }
   free(pHash);

    if(lSumservice>0){
       qsort(&stService,lSumservice,sizeof(struct ncSflow_s),ncUtlSortOnServiceId3);
    }
    
 //  for(i=0;i<lSumservice;i++){
 //   	printf("i=%d,service=%d,gname=%s\n",i,stService[i].lService,stService[i].caGname);
//    }

 

     

 

                     
  	psDbHead = utPltInitDb();
  
 
   if((strlen(caSdate)==0)||(strlen(caEdate)==0)){
  	   strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime)); 
  	   strcpy(caEdate,utTimFormat("%Y/%m/%d",lTime));  	  
  	   strcpy(caShour,utTimFormat("%H",lTime-7200));
  	   strcpy(caSmin,utTimFormat("%M",lTime-7200));
  	   
  	   strcpy(caEhour,utTimFormat("%H",lTime));
  	   strcpy(caEmin,utTimFormat("%M",lTime));

  	  }
  	   sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
  	   sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
  	   lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
  	   lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
  	   
  	utPltPutVarF(psDbHead,"stime","%d",lStime);
  	utPltPutVarF(psDbHead,"etime","%d",lEtime);
 
 if(atol(caGitem)==100){
 	utPltPutVar(psDbHead,"tname","用户名");
	}
	else if(atol(caGitem)==101){
		utPltPutVar(psDbHead,"tname","部门名");
	} 	
 // printf("aaaaffff\n");
  	
    sprintf(caStatdate,"统计时间：从%s %s:%s 到%s %s:%s",	caSdate,caShour,caSmin,caEdate,caEhour,caEmin);	  
  	 utPltPutVar(psDbHead,"shour",caShour);
  	 utPltPutVar(psDbHead,"smin",caSmin);
  	 utPltPutVar(psDbHead,"ehour",caEhour);
  	 utPltPutVar(psDbHead,"emin",caEmin);
  	 utPltPutVar(psDbHead,"sdate",caSdate);
  	 utPltPutVar(psDbHead,"edate",caEdate);
  	 utPltPutVar(psDbHead,"gitem",caGitem);
 
	  
  lSj=0; 	 
  	   	
  	

  		//服务大类
  		strcpy(caTitle,"邮件拦截数量分布情况");
  	      utPltPutVar(psDbHead,"title","邮件拦截数量分布图");
  	      utPltPutVar(psDbHead,"title_1","邮件拦截数量分布表");
  	     
  	
  	    lSumGindex=0;
  	    lOth=0;
  	    lSum=0;
        for(i=0;i<lSumservice;i++){
        
        	 if(i<20)
        	 {
        	 	lSumGindex++;}
  	      	lSum=lSum+stService[i].lService;
  	      	if(i>20){
  	        lOth=lOth+stService[i].lService;
  	         }
  	     }

  	    iNum=0;
  	   
  	    for(i=0;i<lSumGindex;i++){
  	    	if(stService[i].lService>0){
  	    
  	    //输出到表格	 
  	    	if(i<20){

  	    		 utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF(stService[i].lService,caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	    		 utPltPutLoopVar(psDbHead,"name_1",  iNum+1,stService[i].caGname);
  	    		if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*stService[i].lService/lSum);
    				}
  	    		} 
  	    //输出到图		  
  	    		 if(i<20) {
    		 	
  	    		  lSj=lSj+1;
  	    		 utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%lu",	stService[i].lService);  
             utPltPutLoopVar(psDbHead,"name",  iNum+1,stService[i].caGname);
             utPltPutLoopVarF(psDbHead,"groupid",iNum+1,"%d",0);
             utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",0);
             utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
           
            
     				if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*stService[i].lService/lSum);
    				}
           }
            else{
            	lOth=lOth+stService[i].lService;
            }
            iNum++;
  	    	}
     }
     
    // 输出其它
     if(lOth>0){
     	  	   utPltPutLoopVarF(psDbHead,"bytes",lSj+1,"%llu",	lOth);  
             utPltPutLoopVar(psDbHead,"name",  lSj+1,ncLang("0431其它"));
             utPltPutLoopVarF(psDbHead,"groupid",lSj+1,"%d",999999);
             utPltPutLoopVarF(psDbHead,"ltime",lSj+1,"%d",0);
             utPltPutLoopVarF(psDbHead,"num",lSj+1,"%d",lSj);
             if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",lSj+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",lSj+1,"%0.2f",(float)100*lOth/lSum);  	  			
    				}             
    
  	 	  	 
  	 }
  	 
  	if(strlen(caPdf)>0){
			//表格标签
	 		desc=malloc(3*sizeof(char*));
	 		for(i=0;i<3;i++){
	 			desc[i]=malloc(32);
   		}
		   strcpy(desc[0],"邮箱名");		  
		   sprintf(desc[1],"%s(%s)","数量","封");
		   strcpy(desc[2],ncLang("1827百分比%"));
     
 		 //输出到PDF中     
   	  pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*12);
      if(pChart==NULL){
  	  	 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),"分配内存出错");
      	 return 0;
   	  } 
   	 iNum=0;
   
   	 for(i=0;i<lSj;i++){   	   
   	 	if(iNum<20){
                                             //服务大类
   	 	  	pChart[iNum].name=strdup(stService[i].caGname);
  				pChart[iNum].amount=(float)stService[i].lService; 	
  			 	if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
          	if(lSum==0){
   	 					sprintf(caTemp,"0.00");
   	 				}else{
   	 			  	sprintf(caTemp,"%0.2f",(float)100*stService[i].lService/lSum);
   	 				}
 	 		
   	 				pChart[iNum].percent=strdup(caTemp); 
           	iNum++;
        	
     		}
  	 }
  	         // 输出其它
     if(lOth>0){
     		pChart[iNum].name=strdup(ncLang("0431其它"));  
     	   	if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lOth)/lSum);
   	 			}  	 			
     	   	pChart[iNum].percent=strdup(caTemp);  	 		
   	 			pChart[iNum].amount=(float)lOth/1024;                  
          iNum++;                                     
     	}
     
      rowrow=iNum;
   
    	sprintf(filename,"%s.PDF",caTitle); 

			
     	ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,3,0, caTitle,"单位:封",caStatdate,filename); 
     
			//释放内存     
   		for(i=0;i<rowrow;i++){
	      free(pChart[i].name);
	      free(pChart[i].percent);	      
   		}  
   		if(pChart) free(pChart);
   
   		for(i=0;i<3;i++){
   	   	free(desc[i]);
   		}
   	  if(desc) free(desc);
   
      utPltFreeDb(psDbHead);
    
      return 0;
   }

		

    	utPltPutVar(psDbHead,"groupname",gpname); 
			utPltPutVar(psDbHead,"dispname",dpname);  
			utPltPutVar(psDbHead,"groupid",group_id); 
			utPltPutVar(psDbHead,"userid",user_Id); 
			utPltPutVar(psDbHead,"rootname",rtname); 
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/iframe_mailctlbt_depuser.htm");  
  
    return 0;
}

//点击查看拦截邮件列表

int ncMailCtlDet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
    char caWhere[512],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caUserid[16];
    char caUsername[32],caTo[64],caFrom[64],caSub[128],caSdate[32];
    char caFile[128],caTemp[512],caGroupid[12];
    char caMsize[12],caAsize[12],caEdate[20],caOrder[12],caOrderby[12];
    char caDdd[12],caUnit[12],caStatus[12],caDtype[12],caBef[12];
    char caWhereStr[10];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lAtt,lStat,lSize,lUserid0,lSid;
    unsigned long lTime,l,lTime1,lCode,lSdate;
    char caId[16];
    int i,iNum,iPos;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount,lStatus1,lSumUser;
 	pasDbCursor *psCur;
 	int iCharCode=0;
	if(strcasecmp(utComGetVar_sd(psShmHead,"MailCharCode","Auto"),"Auto") == 0) {
	    iCharCode = 1;
	}
	else {
	    iCharCode = 0;
	}
 	char caStime[16],caEtime[16],caMaddr[64],caDispname[32],caGroupname[32],caGitem[12];
  char caGname[32];
   	strcpy(caWhere,"\0");
	iReturn = utMsgGetSomeNVar(psMsgHead,11,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
                  "userid",     UT_TYPE_STRING,10,caUserid,
                  "stime",      UT_TYPE_STRING,10,caStime,
                  "etime",      UT_TYPE_STRING,10,caEtime,
                  "maddr",      UT_TYPE_STRING,63,caMaddr,
                  "dispname",   UT_TYPE_STRING,30,caDispname,
                  "groupname",  UT_TYPE_STRING,30,caGroupname,
                  "gitem",      UT_TYPE_STRING,10,caGitem,
                  "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					        "CurPg",      UT_TYPE_STRING, 12, caCurPg,
					        "gname",         UT_TYPE_STRING,30,caGname);
	 
	 printf("caMaddr=%s\n",caMaddr);              
  if(atol(caGitem)==102){
  	ncMailCtlKeyStat(psShmHead,iFd,psMsgHead);
  	return 0;
  }

printf("gitem=%s,gname=%s\n",caGitem,caGname);

     lCurPg = atol(caCurPg);
     if(lCurPg <= 0) lCurPg = 1;

    lStartRec = (lCurPg - 1) * lRowNum;  
    lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;
   
    if(!utStrIsSpaces(caGroupid)) {
        utStrAddF(caWhere," and a.groupid = %s ",caGroupid);
    }
    if(!utStrIsSpaces(caUserid)) {
        utStrAddF(caWhere," and a.userid =%s ",caUserid);
    }
    if(strlen(caStime)>0){
    	utStrAddF(caWhere," and b.sdate>=%s ",caStime);
    }
    if(strlen(caEtime)>0){
    	utStrAddF(caWhere," and b.sdate<=%s ",caEtime);
    }
    if(strlen(caMaddr)>0){
    	if(atol(caGitem)==101){
    		 utStrAddF(caWhere," and b.mailto like '%c%s%c'",'%',caMaddr,'%');
    		}
    		else if(atol(caGitem)==100){
    			utStrAddF(caWhere," and b.mailfrom like '%c%s%c'",'%',caMaddr,'%');
    		}
    }
    
    if(strlen(caGname)>0){
    	if(atol(caGitem)==101){
    		utStrAddF(caWhere," and a.groupid in (select groupid from ncgroup where groupname='%s') ",caGname);
    	}
    	else if(atol(caGitem)==100){
    		utStrAddF(caWhere," and a.dispname='%s' ",caGname);
    	}
    }
    

    

    sprintf(caTemp,"select count(*) from ncmailctl b,ncuser a where a.userid = b.userid  %s",caWhere);
printf("caTemp=%s\n",caTemp);     
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
  
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0301记录查询出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
    
   

        psCur = pasDbOpenSqlF("select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode \
                from ncmailctl b,ncuser a  where b.userid = a.userid  %s limit %d,%d",caWhere,lStartRec,lRowNum);
 printf("select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode \
                from ncmailctl b,ncuser a  where b.userid = a.userid  %s limit %d,%d\n",caWhere,lStartRec,lRowNum);     
   
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
    psDbHead = utPltInitDb();
  	iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_ULONG,4,&lSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lStat,
  	                               UT_TYPE_LONG,4,&lStatus1,
  	                               UT_TYPE_LONG,4,&lCode);
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
   	    i++;
        iNum++;
        if(iCharCode == 0) {
            lCode = pasDefaultCode();
        }
        if(lStat == 0 ) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0655无内容"));
            strcpy(caFile,"\0");
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0655无内容"));
        }
        else if(lStat == 1) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0859直接发送"));
        }
        else if(lStat == 4 ) {
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0626未处理"));
        }
        else if(lStat == 5){
            if(lStatus1 == 1){
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0511审计发送"));
            }                
            else if(lStatus1 == 2){
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0512审计拒绝"));
            }                
            else if(lStatus1 == 3) {
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0513审计转发"));
            }                
        }
        else if(lStat == 2) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0860直接拒绝"));
        }
        else if(lStat == 3) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0861直接转发"));
        }
        else {
            strcpy(caFile,"\0");
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0655无内容"));
        }
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lSdate));
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVar(psDbHead,"file", iNum,caFile);
        utPltPutLoopVar(psDbHead,"from", iNum,pasCvtGBK(lCode,caFrom,caTemp,128));
        utPltPutLoopVar(psDbHead,"to", iNum,pasCvtGBK(lCode,caTo,caTemp,128));
        utPltPutLoopVar(psDbHead,"sub", iNum,pasCvtGBK(lCode,caSub,caTemp,255));
        utPltPutLoopVarF(psDbHead,"size", iNum,"%lu",lSize);
        if(lAtt > 0) {
            utPltPutLoopVar(psDbHead,"att", iNum,ncLang("0815有"));
        }
        utPltPutLoopVarF(psDbHead,"ccode", iNum,"%lu",lCode);
      	iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_ULONG,4,&lSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lStat,
  	                               UT_TYPE_LONG,4,&lStatus1,
  	                               UT_TYPE_LONG,4,&lCode);
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
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"stime",caStime);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"maddr",caMaddr);
    utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"groupname",caGroupname);
    utPltPutVar(psDbHead,"gitem",caGitem);
    utPltPutVar(psDbHead,"gname",caGname);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mailctl_sfdet.htm");
    return 0;
}

//点击状态对拦截邮件关键字进行统计

int ncMailCtlKeyStat(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
    char caWhere[512],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caUserid[16];
    char caUsername[32],caTo[64],caFrom[64],caSub[128],caSdate[32];
    char caFile[128],caTemp[512],caGroupid[12];
    char caMsize[12],caAsize[12],caEdate[20],caOrder[12],caOrderby[12];
    char caDdd[12],caUnit[12],caStatus[12],caDtype[12],caBef[12];
    char caWhereStr[10];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lAtt,lStat,lSize,lUserid0,lSid;
    unsigned long lTime,l,lTime1,lCode,lSdate;
    int i,iNum,iPos;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount,lStatus1,lSumUser;
 	pasDbCursor *psCur;

 	char caStime[16],caEtime[16],caMaddr[64],caDispname[32],caGroupname[32],caGitem[12];
  char caKey[64];
  long lSum;
   	strcpy(caWhere,"\0");
	iReturn = utMsgGetSomeNVar(psMsgHead,10,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
                  "userid",     UT_TYPE_STRING,10,caUserid,
                  "stime",      UT_TYPE_STRING,10,caStime,
                  "etime",      UT_TYPE_STRING,10,caEtime,
                  "maddr",      UT_TYPE_STRING,63,caMaddr,
                  "dispname",   UT_TYPE_STRING,30,caDispname,
                  "groupname",  UT_TYPE_STRING,30,caGroupname);
                  
	   if(strcmp(caMaddr,"正常发送")==0){
	   	lStat=1;
	  }
	  else if(strcmp(caMaddr,"拒绝发送")==0){
	   	lStat=2;
	  }
		else if(strcmp(caMaddr,"转发")==0){
	   	lStat=3;
	  }
		else if(strcmp(caMaddr,"待审计")==0){
	   	lStat=4;
	  }
		else if(strcmp(caMaddr,"已审计")==0){
	   	lStat=5;
	  }
    else{
    	lStat=0;
    }
     
    if(!utStrIsSpaces(caGroupid)) {
        utStrAddF(caWhere," and b.groupid = %s ",caGroupid);
    }
    if(!utStrIsSpaces(caUserid)) {
        utStrAddF(caWhere," and b.userid =%s ",caUserid);
    }
    if(strlen(caStime)>0){
    	utStrAddF(caWhere," and b.sdate>=%s ",caStime);
    }
    if(strlen(caEtime)>0){
    	utStrAddF(caWhere," and b.sdate<=%s ",caEtime);
    }
    utStrAddF(caWhere," and mailstat=%d ",lStat);
    
    
    

    

   psCur=pasDbOpenSqlF("select sum(lcount) aa,mkey from ncmailctlkey a,ncmailctl b where a.sid=b.sid %s group by mkey order by aa desc ",caWhere);
   printf("select sum(lcount) aa,mkey from ncmailctlkey a,ncmailctl b where a.sid=b.sid %s group by mkey order by aa desc \n",caWhere);
    
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
    psDbHead = utPltInitDb();
    lSum=0;
    memset(caKey,0,sizeof(caKey));
  	iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSum,
      	                           UT_TYPE_STRING,63,caKey);
      	                          
   	iNum = 0;
   	while(iReturn == 0 ||iReturn==1405) {
 
        iNum++;

        utPltPutLoopVarF(psDbHead,"count",   iNum,"%lu",lSum);
        utPltPutLoopVar(psDbHead,"key",   iNum,caKey);
       

    lSum=0;
    memset(caKey,0,sizeof(caKey));
  	iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSum,
      	                           UT_TYPE_STRING,63,caKey);
    }
    pasDbCloseCursor(psCur);
  
  
    utPltPutVar(psDbHead,"maddr",caMaddr);
    utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"groupname",caGroupname);

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mailkey_stat.htm");
    return 0;
}


/* 数据库操作行为分析统计*/
int ncWebShowDbBt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{   
    pasDbCursor *psCur;
    char sqlbuf[2024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
    char serv_name[32];
    long lService,iNum;
    long long lSum;
    char caDate[20];
    char caDate_end[20];
    int iReturn,i;
    char caRefresh_flag[8];

    long long lBytes_other;
    char caName[32];
    long lGroupid;
    char preflag[8],nextflag[8],ctime[12];
    char caServ_flag[12];
    char caDate_sh[16],caDate_sm[16],caDate_eh[16],caDate_em[16];
    char caGitem[16],caSdate[16],caShour[8],caSmin[8],caEdate[16],caEhour[8],caEmin[8];
    char caSdate_all[20],caEdate_all[20];
    long lStime,lEtime,lRecenttime,lStime1,lSid;
    long lSumindex;
    long lSumGindex;
    char caMaxdate[16],caMaxtime[12],caRectime[16],caTemp[1024];
    long long lOth=0;
    long index;
    long lSj;
    char caWorksql[1024],caWorksql2[1024];
    char caSip[16],caDip[16];
    long lOpttype;
//iReturn=ncMailAttachSend("aa@pronetway.com","lyf@pronetway.com","test","test","/home/ncmysql/nc/bin/aa.PDF","reprot");

//printf("iReturn=%d\n",iReturn);
    
     struct piechart_datasetf *pChart;           //用于PDF文档输出
//   char *desc[]={"用户名","部门","流量","百分比"};
    int rowrow;                     
    int column; 
    char **pTemp;
    char caTitle[128];
    char filename[128];
    int m;
    column=4;
    long lDate;
    char **desc;
    char caPdf[16];
    char caStatdate[128];
    char user_Id[15]="";
    char dpname[32]="";
    char gpname[32]="";
    char rtname[32]="";
    char expexcel[12]="";
    char f_name[120]="";
    char cp_command[320]="";
  	char rm_command[320]="";
  	char caSdate_All[20],caEdate_All[20];
  	char caWorkflag[12];
  	FILE *fp; 
  	char group_id[32]="";
  	char caUsername2[32];
  	unsigned char *pHash;
  	pasLHashInfo  sHashInfo;
  	char caMailfrom[64],caMailto[64],caKey[64];
  	struct ncSflow_s *psData;
  	long lStat,lSumservice;
  	char *p;
  	
    memset(caUsername2,0,sizeof(caUsername2));
 	  dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
  	
  	struct ncSflow_s stService[2000];
   lTime=time(0);
   lSum=0;
   lRecenttime=0;
   
      utMsgGetSomeNVar(psMsgHead,15,"gitem",UT_TYPE_STRING,10,caGitem,					  //统计项目
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//起始时间日期
                                    "shour",UT_TYPE_STRING,2,caShour,						  //起始时间的小时
                                    "smin", UT_TYPE_STRING,2,caSmin,
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "ehour",UT_TYPE_STRING,2,caEhour,
                                    "emin", UT_TYPE_STRING,2,caEmin,
                                    "exppdf",UT_TYPE_STRING,8,caPdf,							//导出pdf
                                    "groupid",UT_TYPE_STRING,30,group_id,					//部门id
                                    "userid",UT_TYPE_STRING,14,user_Id,				    //用户id
                                    "groupname",UT_TYPE_STRING,30,gpname,					//标题中的部门名
                                    "dispname",UT_TYPE_STRING,30,dpname,				  //个人显示名
                                    "expexcel",UT_TYPE_STRING,10,expexcel,				//导出pdf	
                                    "rootname",UT_TYPE_STRING,30,rtname,
                                    "workflag",UT_TYPE_STRING,8,caWorkflag);	
      psDbHead = utPltInitDb();                              
      if((strlen(caSdate)==0)||(strlen(caEdate)==0)){
  	   strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime)); 
  	   strcpy(caEdate,utTimFormat("%Y/%m/%d",lTime)); 
  	   strcpy(caShour,"00");
  	   strcpy(caSmin,"00");
  	   strcpy(caEhour,"23");
  	   strcpy(caEmin,"59"); 	  
//  	   strcpy(caShour,utTimFormat("%H",lTime-72000));
//  	   strcpy(caSmin,utTimFormat("%M",lTime-72000));
  	   
//  	   strcpy(caEhour,utTimFormat("%H",lTime));
//  	   strcpy(caEmin,utTimFormat("%M",lTime));

  	  }                          			
   	   sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
  	   sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
  	   lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
  	   lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);



if(strlen(caGitem)==0) strcpy(caGitem,"101");
 
    	pHash = (unsigned char *)pasLHashInit(2000,2000,sizeof(struct ncSflow_s),272,64); 
			if(pHash == NULL) {
		        printf(" Memory HttpHash Error \n");
		        return (-1);
		  }

      sprintf(caTemp,"select inet_ntoa(sip),inet_ntoa(dip),opttype,bytes from ncdbseslog where 1=1 ");
      	if(strlen(group_id)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where groupid =%s )",group_id);
  	  	if(strlen(user_Id)>0){
  	  		 sprintf(caTemp+strlen(caTemp)," and userid =%s ",user_Id);	
  	  	}
  	  	
  	  	sprintf(caTemp+strlen(caTemp)," and stime>=%d and stime<=%d ",lStime,lEtime);
  	  	
 // printf("caTemp=%s\n",caTemp);    
      psCur=pasDbOpenSql(caTemp,0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
     }
     memset(caSip,0,sizeof(caSip));
     memset(caDip,0,sizeof(caDip));
     lOpttype=0;
     lBytes=0;
		 iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
		                                UT_TYPE_STRING,15,caDip,
		                                UT_TYPE_LONG,4,&lOpttype,
		                                UT_TYPE_LONG,4,&lBytes);
		                                   
		 while((iReturn==0)||(iReturn==1405))
		 {
		 	 
		 	 	if(atol(caGitem)==100){                        //源IP
			 
	
			 		memset(caKey,0,sizeof(caKey));
			 		strcpy(caKey,caSip);
//			 		printf("caKey1=%s\n",caKey);
			 		psData = (struct ncSflow_s *)pasLHashLookA(pHash,caKey);
			 		if(psData){
			 			psData->lService++;
			 			psData->lBytes=psData->lBytes+lBytes;
			 		}
		
			 		
		 	}
		 	else if(atol(caGitem)==101){                      //操作类型

			 		memset(caKey,0,sizeof(caKey));
	
			  strcpy(caKey,ncGetDbOpnamebyid(lOpttype));
//			 		printf("caKey=%s\n",caKey);
			 		psData = (struct ncSflow_s *)pasLHashLookA(pHash,caKey);
			 		if(psData){
			 			psData->lService++;
			 			psData->lBytes=psData->lBytes+lBytes;
			 		}
			 
			}
			 else if(atol(caGitem)==102){ 
			 	memset(caKey,0,sizeof(caKey));                     //目标IP
			 strcpy(caKey,caDip);
//			 	printf("caKey=%s\n",caKey);
			 		psData = (struct ncSflow_s *)pasLHashLookA(pHash,caKey);
			 		if(psData){
			 			psData->lService++;
			 			psData->lBytes=psData->lBytes+lBytes;
			 		}
			 }
		 	
		
     memset(caSip,0,sizeof(caSip));
     memset(caDip,0,sizeof(caDip));
     lOpttype=0;
     lBytes=0;
		 iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
		                                UT_TYPE_STRING,15,caDip,
		                                UT_TYPE_LONG,4,&lOpttype,
		                                UT_TYPE_LONG,4,&lBytes);
		 }
     pasDbCloseCursor(psCur); 


    psData= (struct ncSflow_s *)pasLHashFirst(pHash,&sHashInfo);
   lSumservice=0;
   while(psData&&lSumservice<2000) {
   stService[lSumservice].lService=psData->lService;   
   strcpy(stService[lSumservice].caGname,psData->caGname);
	 stService[lSumservice].lBytes=psData->lBytes;
   	lSumservice++;
   	psData = (struct ncSflow_s *)pasLHashNextS(&sHashInfo);
   }
   free(pHash);

    if(lSumservice>0){
       qsort(&stService,lSumservice,sizeof(struct ncSflow_s),ncUtlSortOnServiceId3);
    }
    
  // for(i=0;i<lSumservice;i++){
  //  	printf("i=%d,service=%d,gname=%s\n",i,stService[i].lService,stService[i].caGname);
 //   }

 

        	if(atol(caGitem)==100){                        //源IP
        		utPltPutVar(psDbHead,"t_name","源ip");
        	}
        	else if(atol(caGitem)==102){                   //目标IP
        		utPltPutVar(psDbHead,"t_name","目标ip");
        	}
        	else{                                          //操作类型
        		utPltPutVar(psDbHead,"t_name","操作类型");
        	}             
  	
  
  	   sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
  	   sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
  	   lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
  	   lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
  	   
  	utPltPutVarF(psDbHead,"stime","%d",lStime);
  	utPltPutVarF(psDbHead,"etime","%d",lEtime);
  	
  	
    sprintf(caStatdate,"统计时间：从%s %s:%s 到%s %s:%s",	caSdate,caShour,caSmin,caEdate,caEhour,caEmin);	  
  	 utPltPutVar(psDbHead,"shour",caShour);
  	 utPltPutVar(psDbHead,"smin",caSmin);
  	 utPltPutVar(psDbHead,"ehour",caEhour);
  	 utPltPutVar(psDbHead,"emin",caEmin);
  	 utPltPutVar(psDbHead,"sdate",caSdate);
  	 utPltPutVar(psDbHead,"edate",caEdate);
  	 utPltPutVar(psDbHead,"gitem",caGitem);
 
	  
  lSj=0; 	 
  	   	
  	

  		//服务大类
  		strcpy(caTitle,"数据库操作统计分析");
  	      utPltPutVar(psDbHead,"title","数据库操作分布图");
  	      utPltPutVar(psDbHead,"title_1","数据库操作分布表");
  	     
  	
  	    lSumGindex=0;
  	    lOth=0;
  	    lSum=0;
        for(i=0;i<lSumservice;i++){
        
        	 if(i<20)
        	 {
        	 	lSumGindex++;}
  	      	lSum=lSum+stService[i].lService;
  	      	if(i>20){
  	        lOth=lOth+stService[i].lService;
  	         }
  	     }

  	    iNum=0;
  	   
  	    for(i=0;i<lSumGindex;i++){
  	    	if(stService[i].lService>0){
  	    
  	    //输出到表格	 
  	    	if(i<20){

  	    		 utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF(stService[i].lService,caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);

  	    		 utPltPutLoopVarF(psDbHead,"bytes_t",iNum+1,"%s",utStrLtoF8(stService[i].lBytes,caTemp));
  	    		 utPltPutLoopVar(psDbHead,"name_1",  iNum+1,stService[i].caGname);
  	    		if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*stService[i].lService/lSum);
    				}
  	    		} 
  	    //输出到图		  
  	    		 if(i<20) {
    		 	
  	    		  lSj=lSj+1;
  	    		 utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%lu",	stService[i].lService);  
             utPltPutLoopVar(psDbHead,"name",  iNum+1,stService[i].caGname);
             utPltPutLoopVarF(psDbHead,"groupid",iNum+1,"%s",stService[i].caGname);
             utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",0);
             utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
           
            
     				if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*stService[i].lService/lSum);
    				}
           }
            else{
            	lOth=lOth+stService[i].lService;
            }
            iNum++;
  	    	}
     }
     
    // 输出其它
     if(lOth>0){
     	  	   utPltPutLoopVarF(psDbHead,"bytes",lSj+1,"%llu",	lOth);  
             utPltPutLoopVar(psDbHead,"name",  lSj+1,ncLang("0431其它"));
             utPltPutLoopVarF(psDbHead,"groupid",lSj+1,"%d",999999);
             utPltPutLoopVarF(psDbHead,"ltime",lSj+1,"%d",0);
             utPltPutLoopVarF(psDbHead,"num",lSj+1,"%d",lSj);
             if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",lSj+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",lSj+1,"%0.2f",(float)100*lOth/lSum);  	  			
    				}             
    
  	 	  	 
  	 }
  	 
  	if(strlen(caPdf)>0){
			//表格标签
	 		desc=malloc(4*sizeof(char*));
	 		for(i=0;i<4;i++){
	 			desc[i]=malloc(32);
   		}
   		
   		   	if(atol(caGitem)==100){                        //源IP
   		     		 strcpy(desc[0],"源ip");		
        	}
        	else if(atol(caGitem)==102){                   //目标IP
        		strcpy(desc[0],"目标ip");	
        		
        	}
        	else{                                          //操作类型
        		strcpy(desc[0],"操作类型");	
        	}             
		  strcpy(desc[1],"流量(字节)");
		   sprintf(desc[2],"%s(%s)","记录数","条");
		   strcpy(desc[3],ncLang("1827百分比%"));
     
 		 //输出到PDF中     
   	  pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*12);
      if(pChart==NULL){
  	  	 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),"分配内存出错");
      	 return 0;
   	  } 
   	 iNum=0;
   
   	 for(i=0;i<lSj;i++){   	   
   	 	if(iNum<20){
          pChart[iNum].append=malloc(1*sizeof(char*)); 
          pChart[iNum].append[0]=malloc(32);
          sprintf(pChart[iNum].append[0],"%llu",stService[iNum].lBytes);
          
                                      //服务大类
   	 	  	pChart[iNum].name=strdup(stService[i].caGname);
  				pChart[iNum].amount=(float)stService[i].lService; 	
  			 	if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
          	if(lSum==0){
   	 					sprintf(caTemp,"0.00");
   	 				}else{
   	 			  	sprintf(caTemp,"%0.2f",(float)100*stService[i].lService/lSum);
   	 				}
 	 		
   	 				pChart[iNum].percent=strdup(caTemp); 
           	iNum++;
        	
     		}
  	 }
  	         // 输出其它
     if(lOth>0){
     		pChart[iNum].name=strdup(ncLang("0431其它"));  
     	   	if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lOth)/lSum);
   	 			}  	 			
     	   	pChart[iNum].percent=strdup(caTemp);  	 		
   	 			pChart[iNum].amount=(float)lOth/1024;                  
          iNum++;                                     
     	}
     
      rowrow=iNum;
   
    	sprintf(filename,"%s.PDF",caTitle); 

			
     	ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,4,1, caTitle,"单位:条",caStatdate,filename); 
     
			//释放内存     
   		for(i=0;i<rowrow;i++){
	      free(pChart[i].name);
	      free(pChart[i].percent);	
	      free(pChart[i].append[0]);
	      free(pChart[i].append);      
   		}  
   		if(pChart) free(pChart);
   
   		for(i=0;i<4;i++){
   	   	free(desc[i]);
   		}
   	  if(desc) free(desc);
   
      utPltFreeDb(psDbHead);
    
      return 0;
   }

		

    	utPltPutVar(psDbHead,"groupname",gpname); 
			utPltPutVar(psDbHead,"dispname",dpname);  
			utPltPutVar(psDbHead,"groupid",group_id); 
			utPltPutVar(psDbHead,"userid",user_Id); 
			utPltPutVar(psDbHead,"rootname",rtname); 
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"db/iframe_dbbt.htm");  
       ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"查看数据库操作统计信息"); 
  
    return 0;
}



//显示数据库操作趋势图
int ncWebShowDbBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   utPltDbHead *psDbHead;
    pasDbCursor *psCur;
    char caDate[20];  
    char caFlag[8];
    char caPre_flag[8],caAv_un[8];
    char sqlbuf[2048];
    int      iReturn,i,j;
    long lStep,lTime,lStarttime,lEndtime;
    char caSid[32],caUnit[16];
    char caEhour[8],caEmin[8];
    char caEdate_all[20];
    char caTemp[2048];
    char caTemp1[32];
    char caName[32];
    char caStime[16];
    float tot[60];
    long y_sum;
    char caSdate_temp1[20],caSdate_temp2[20];
    int m,n,iNum;
    int mm,nn,temp_nn;
    unsigned int temp_m;
    char temp_t[16];

    char caTime[16];
    long unit;
    long lSid,lConntime;
    long long lMax=0;
    long index,x_sum;              //index  X轴下标   x_sum X轴下标总数
    float rate;
    long count_d=0;                //Y轴数据数
    char caHour[12];
    long long lMaxbytes;               //存放流量最大值
    long lMaxrate;                     // 输出Y轴最大值
    char caGitem[16],caSdate[20],caShour[16],caSmin[16];

   struct ncSflow_s  sGservice[20];                  //存放统计数据，前十种服务放在0-10中，其它放在sGservice 最后一个
    long long lBytes;
    long lOpttype;
    char caOpt[36];
    long lService,lGroupid;
    long lSumindex,findex;
    long lSumGindex; 
    long long lRat;
    char caEdate[20];  
    long long lCount;
    long lMin;                              
    char caSdate_all[20];                                    
    int qt_flag=0;
    long lSj=0;
    char caTid[16];
    char gpname[32];
    char dpname[32];
    char rtname[32]="";
    char group_id[15],caUserid[15];
    char caPdf[12];
    struct chart_datasetf *pChart;           //用于PDF文档输出
    int rowrow;                     
    int column; 
    char caTitle[128];
    char filename[128];
    char caStatdate[128];
    long t_time;
    char **desc;
    long u;
    
    long t;
    char unit_page[10];
    long long unit_lBytes;
    char caUsername2[32];
    
    memset(caUsername2,0,sizeof(caUsername2));
 	  dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
 	  
    utMsgGetSomeNVar(psMsgHead,14,"stime",    UT_TYPE_STRING,10,caStime,				//
   		                            "sdate",    UT_TYPE_STRING,10,caSdate,
   		                            "shour",    UT_TYPE_STRING,8,caShour,
   		                            "smin",     UT_TYPE_STRING,8,caSmin,
   		                            "av_un",    UT_TYPE_STRING,8,caAv_un,					//按2：小时，1：分钟，3：日统计
																  "pre_flag", UT_TYPE_STRING,2,caPre_flag,	    //显示前一段时间的数据1：向前，2：向后
																  "groupname",UT_TYPE_STRING,30,gpname,
																  "dispname", UT_TYPE_STRING,30,dpname,
																  "tid",      UT_TYPE_STRING,10,caTid,					//网址库类别
																  "userid",   UT_TYPE_STRING, 14, caUserid,
																  "groupid",  UT_TYPE_STRING,14,group_id,
																  "pdf",      UT_TYPE_STRING,8,caPdf,
																  "rootname", UT_TYPE_STRING,30,rtname,
																  "gitem", UT_TYPE_STRING,30,caGitem);
    
 	                   
  psDbHead = utPltInitDb(); 
 

          
  //printf("== caAv_un=%s ===\n",caAv_un);
  printf("== === caGitem=%s \n",caGitem);
   
 		
  if(strlen(caGitem)==0) strcpy(caGitem,"2");
  if(strlen(caAv_un)==0) strcpy(caAv_un,"2");										//默认按小时统计
  
  utPltPutVar(psDbHead,"gitem",caGitem);  
  
	if(atol(caAv_un)==2){						  //按小时统计(nciplog)
	 	x_sum=24;
	 	y_sum=24;
	 	t_time=3600;
	}
	else if(atol(caAv_un)==3){						  //按天统计(nciplog)
		x_sum=30;
		y_sum=30;
		t_time=3600*24;
	}
  else{
  	t_time=3600;
  }
	if((strlen(caSdate)==0)){											//默认没时间按系统时间，并按分钟统计
		    lTime=time(0);
		    strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));
		    strcpy(caShour,"00");
		    strcpy(caSmin,"00");
	//	    printf("caSmin=%s\n",caSmin);
	    if(atol(caAv_un)==2){
		     		strcpy(caSmin,"00");
		     		strcpy(caShour,"00");
		     		strcpy(caEhour,"23");
		  		  strcpy(caEmin,"59");
		     		sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
		     		lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		     		lEndtime=lStarttime+24*3600;
		    }else{   
		    	  ncUtlMonthDate(lTime,caSdate,caEdate);	
		    	 	strcpy(caSmin,"00");
		     	  strcpy(caShour,"00");  
		     	  sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
		     	   sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
				 		lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
						lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);                                              //按日统计
		    }
	}else{
		  	if(atol(caAv_un)==2){
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  		strcpy(caEhour,"23");
		  		strcpy(caEmin,"59");
		  		sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
		      lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		  		sprintf(caEdate_all,"%s %s:%s",caSdate,caEhour,caEmin);
		  	 lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);       
		  				     	   
 
		  	}else if(atol(caAv_un)==3){
		  	 	lStarttime=utTimStrToLong("%Y/%m/%d",caSdate);	
		  	 	ncUtlMonthDate(lStarttime,caSdate,caEdate);
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  		strcpy(caEhour,"23");
		  		strcpy(caEmin,"59");
		  		sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
		      lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		  		sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
		  	 lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all); 
		  	}
	}
//处理上一段和下一时间段		
printf("lStarttime=%d,lEndtime=%d\n",lStarttime,lEndtime);  
		if(atol(caPre_flag)==1){
		  	if(atol(caAv_un)==2){
		  		lStarttime=lStarttime-24*3600;
		  		lEndtime=lEndtime-24*3600;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  	}else if(atol(caAv_un)==3){
		  		lStarttime=lStarttime-15*24*3600;
		  		ncUtlMonthDate(lStarttime,caSdate,caEdate);
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  		strcpy(caEhour,"23");
		  		strcpy(caEmin,"59");
		  		sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
		      lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		  		sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
		  	 lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all); 
		  		
		  		
		  	}
	}else if(atol(caPre_flag)==2){
		 	  if(atol(caAv_un)==2){
		  		lStarttime=lStarttime+24*3600;
		  		lEndtime=lEndtime+24*3600;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  	}else if(atol(caAv_un)==3){
		  		lStarttime=lStarttime+35*24*3600;
		  		ncUtlMonthDate(lStarttime,caSdate,caEdate);
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  		strcpy(caEhour,"23");
		  		strcpy(caEmin,"59");
		  		sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
		      lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		  		sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
		  	 lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all); 
		  	}
	}
 		utPltPutVar(psDbHead,"sdate",caSdate);
   	utPltPutVar(psDbHead,"shour",caShour);
   	utPltPutVar(psDbHead,"smin",caSmin);
   	utPltPutVar(psDbHead,"tid",caTid);  	
   	utPltPutVar(psDbHead,"avun",caAv_un);
		
    
    utPltPutVar(psDbHead,"tjsj",caSdate);
    //utPltPutVarF(psDbHead,"stime","%d",lEndtime);
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 		//获取统计时间段请求数最大的网址类别，将这些服务在图形上输出，别的服务归到其它类别中
    if(atol(caGitem)==2){
  	   sprintf(caTemp,"select opttype,count(*) from ncdbseslog where stime>=%d and stime<=%d ",lStarttime,lEndtime);
  	 }
  	 else{
  	 	sprintf(caTemp,"select opttype,sum(bytes)/1024 from ncdbseslog where stime>=%d and stime<=%d ",lStarttime,lEndtime);
  	}
  	 
		    if(strlen(group_id)>0){
		    	if(atol(group_id)==0){
		    		sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where groupid=0) ");
		    	}
		    	else
	 				 	sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where groupid=%s )",group_id);
  	   	}
		    if(strlen(caUserid)>0){
		    	sprintf(caTemp+strlen(caTemp)," and userid=%s ",caUserid);
		    }
		    sprintf(caTemp+strlen(caTemp)," group by opttype order by opttype ");
    
     
     
	 printf("**111**caTemp=%s\n",caTemp);
	 memset(&sGservice,0,20*sizeof(struct ncSflow_s));
     psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错2"));
        return 0;
     }
     lOpttype=0;
     lCount=0;
     
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lOpttype,
                                   UT_TYPE_LONG8,8,&lCount);
                                   
                                  
    lSumindex=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	
    	
    		sGservice[lSumindex].lService=lOpttype;
    		sGservice[lSumindex].lGroupid=0;
    		sGservice[lSumindex].lBytes=lCount;
    		
    		
    		   
      strcpy(caOpt,ncGetDbOpnamebyid(lOpttype));
 
    		strcpy(sGservice[lSumindex].caGname,caOpt);
    	for(i=0;i<32;i++){
    		sGservice[lSumindex].lBytesqs[i]=0;
    	}
    	
    
     lSumindex++;
     lOpttype=0;
     lCount=0;
     
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lOpttype,
                                   UT_TYPE_LONG,4,&lCount);
    }
    
    pasDbCloseCursor(psCur);
   
    
	if(lSumindex>0)
    qsort(sGservice,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId2); 
    
    
     if(atol(caAv_un)==2){  
  	  //按小时统计
  	  if(atol(caGitem)==2){
  	    sprintf(caTemp,"select count(*) bb,from_unixtime(stime,\'%%H\') aa,opttype from ncdbseslog where stime>=%d and stime<=%d ",lStarttime,lEndtime);
  	   }
  	   else{
  	   	sprintf(caTemp,"select sum(bytes)/1024 bb,from_unixtime(stime,\'%%H\') aa,opttype from ncdbseslog where stime>=%d and stime<=%d ",lStarttime,lEndtime);
  	   }
  	 
		    if(strlen(group_id)>0){
		    	if(atol(group_id)==0){
		    		sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where groupid=0) ");
		    	}
		    	else
	 				 	sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where groupid=%s )",group_id);
  	   	}
		    if(strlen(caUserid)>0){
		    	sprintf(caTemp+strlen(caTemp)," and userid=%s ",caUserid);
		    }
		    sprintf(caTemp+strlen(caTemp)," group by aa,opttype order by bb ");
     }else if(atol(caAv_un)==3){                    //按天统计
     	if(atol(caGitem)==2){
        sprintf(caTemp,"select count(*) bb,from_unixtime(stime,\'%%d\') aa,opttype from ncdbseslog where stime>=%d and stime<=%d ",lStarttime,lEndtime);
      }
      else{
      	 sprintf(caTemp,"select sum(bytes)/1024 bb,from_unixtime(stime,\'%%d\') aa,opttype from ncdbseslog where stime>=%d and stime<=%d ",lStarttime,lEndtime);
      }
		    if(strlen(group_id)>0){
			   if(atol(group_id)==0){
		    		sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where groupid=0) ");
		    	}
		    	else
	 				 	sprintf(caTemp+strlen(caTemp)," and userid in (select userid from ncuser where groupid=%s )",group_id);
  	   	}
		    if(strlen(caUserid)>0){
		    	sprintf(caTemp+strlen(caTemp)," and a.userid=%s ",caUserid);
		    }
		    sprintf(caTemp+strlen(caTemp)," group by aa,opttype order by bb ");
     		
     		
     }
   printf("**22**caTemp=%s\n",caTemp);
   psCur = pasDbOpenSql(caTemp,0);
   if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错1"));
        return 0;
    }
    lCount=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lCount,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
                                  
    iNum=0;
   
    while((iReturn == 0)||(iReturn==1405)) {
    	findex=atol(caTime);
  	
    	  index=ncUtlGetServicenumById(&sGservice,lSumindex,lService);
 //  	  printf("index=%d\n",index);
//    	  printf("findex=%d\n",findex);
	   	  if(index!=-1){	
	          sGservice[index].lBytesqs[findex]=sGservice[index].lBytesqs[findex]+lCount;         
	      }
       	//printf("sGservice[lSumindex].lBytesqs[findex]=%lld\n",sGservice[lSumindex].lBytesqs[findex]);
    lCount=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
    }
    
    pasDbCloseCursor(psCur);
  long unitflag=0;
  //输出X轴刻度

	  for(mm=0;mm<x_sum;mm++){
	  	    utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
	  	   if(atol(caAv_un)==2){
	  		  	sprintf(caTemp,"%02d:00",mm);
	  		  }
	  		  else if(atol(caAv_un)==3){
	  		  	sprintf(caTemp,"%s/%02d",utTimFormat("%m",lStarttime),mm+1);
	  		  }
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
	  		  //表格中输出统计数据
	  		  for(i=0;i<lSumindex;i++){
	  				                                              //对于按日统计，数组从1开始，其余从0开始
	  				    										
	  			  	 	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm]);  
	  			  	
	  			  	
	  			  	 	  			  	 
	  	  	}
	  	  	
	  }
	   
 //输出服务图形数据 
//if(lSumindex==0) lSumindex=1;
    for(i=0;i<=lSumindex;i++){
    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);

    	utPltPutLoopVar(psDbHead,"sevname",i+1,sGservice[i].caGname);
    	//utPltPutLoopVar(psDbHead,"sevname",i+1,ncUtlGetServiceName(psShmHead,sGservice[i].lService,ncLang("0431其它")));
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",sGservice[i].lService);
      
    	for(mm=0;mm<x_sum;mm++){
    		//printf("i=%d;mm=%d;service=%d;bytes=%lld\n",i,mm,sGservice[i].lService,sGservice[i].lBytesqs[mm]);
    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);  				
    	 utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%ld",sGservice[i].lBytesqs[mm]);	
    		
    }
  }
  utPltPutVarF(psDbHead,"sum","%d",lSumindex);
  utPltPutVarF(psDbHead,"sum_d","%d",x_sum);
  if(atol(caGitem)==2){
  	utPltPutVar(psDbHead,"unit_page","次");
  }
  else{
  	utPltPutVar(psDbHead,"unit_page","KB");
  }
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
printf("******caPdf=%s\n",caPdf);
	if(strlen(caPdf)>0){
	//表格标签
	 desc=malloc((lSumindex+1)*sizeof(char*));
	 for(i=0;i<lSumindex+1;i++){
	 	desc[i]=malloc(32);
	 	strcpy(desc[i],sGservice[i].caGname);
   }

   //输出到PDF中     
   pChart=(struct chart_datasetf*)malloc(sizeof(struct chart_datasetf)*(x_sum+1));
   	if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("1863分配内存出错"));
      return 0;
   	}
  
   	iNum=0;
 
   	
   	for(i=0;i<x_sum;i++){
      if(atol(caAv_un)==2){
	  			sprintf(caTemp,"%02d:00",i);	  		  
	  	 }else if(atol(caAv_un)==3){	  			
	  			sprintf(caTemp,"%s/%02d",utTimFormat("%m",lStarttime),i+1);	  		 
	  	 }
   		  
   	   pChart[i].name=strdup(caTemp);
   	  
   	   pChart[i].dataset=malloc((lSumindex+2)*sizeof(float)) ;
   	   for(mm=0;mm<lSumindex+1;mm++){  	  	
 	  	  		
   	  	 			pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i];   	  
   	 	 	 	
   	 	 } 
   	 }

      rowrow=x_sum;
      column=lSumindex;
   
    strcpy(caTitle,"数据库操作趋势图");
    sprintf(filename,"%s.PDF",caTitle); 

    sprintf(caStatdate,ncLang("1962统计时间:%s %s:%s"),caSdate,caShour,caSmin);
      if(atol(caGitem)==2){
    		sprintf(caTemp,ncLang("1942单位(%s)"),"次");
    	}
    	else{
    		sprintf(caTemp,ncLang("1942单位(%s)"),"KB");
    	}
    ncWebShowServiceBytime_PDF(iFd,pChart,desc,rowrow,column,caTitle,caTemp,caStatdate,filename);

//释放内存     
   for(i=0;i<rowrow;i++){
   	 free(pChart[i].name);       
	   free(pChart[i].dataset);
	 } 
    
   if(pChart) free(pChart);   
   for(i=0;i<lSumindex+1;i++){   	 
   	   free(desc[i]);
   }
   if(desc) free(desc);    
     utPltFreeDb(psDbHead);  
     return 0;
 }
 		

	  utPltPutVar(psDbHead,"groupname",gpname);
	  utPltPutVar(psDbHead,"dispname",dpname);
	  utPltPutVar(psDbHead,"userid",caUserid);
	  utPltPutVar(psDbHead,"groupid",group_id);
	  utPltPutVar(psDbHead,"rootname",rtname);
	  
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"db/iframe_dbbytime.htm");
    ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"查看数据库操作统计信息"); 
    return 0;
}