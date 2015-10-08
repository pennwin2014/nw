#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "ncdef.h"
#include "nclimit.h"
#include "ncsysinfo.h"
#include "ncpkg.h"
#include "ncreport.h" 
/* Internet服务流量排行榜 */
int ncWebSortint_75(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  pasDbCursor *psCur;
    char sqlbuf[9024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
    long iNum;
    long long lSum;
    char caTemp[9024];
    int iReturn,i;
    char caOrder[16];
    long lGroupid0;
    char caExport[12];
    char caSum[12];
    char caSdate[16],caEdate[16],caShour[8],caEhour[8],caGroupid[16],caGitem[12];
    char caSdate_temp[20],caEdate_temp[20],caUnit[16];
    long lStime,lEtime,lConntime;
    long lSumShow;

    struct ncFlowSort_s {
    		unsigned long lId;
        char caUsername[32];
        char caGroupname[32];
        char caIp[16];
        unsigned long long lBytes;      
    };    
    struct ncFlowSort_s *pFlow;
    ncUserCont *psUser;
    long long lSumbytes;
    unsigned long long l_Count;
    long lSumtime;
    long lGitem;
    long lGroupid;
    long m;
		unsigned long lTimes,lIp;
    char caServ_flag[12];
    char caName[32],caHost[65];
    char caPdf[32];

    struct piechart_datasetf *pChart;           //用于PDF文档输出

    int rowrow;                     
    int column; 

    char caTitle[128];
    char filename[128];
    column=4;
    char **desc;
    char caStatdate[128];
    char caUsername[32],caIp[20],caGroupname[32];
    char caService[10];
    long lService;
    long lUserid;
    char *pSelect;
    char *pBuf;
    char *p;
    long lCgid;
		long lSid;
		long k;
		char llflag[3];
		char servicesum[88888];
		int t;
		long total;
		long ltocount;
		char caService_buf[2024];
   lTime=time(0);
   lSum=0;
   utMsgPrintMsg(psMsgHead);
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    printf("== lGroupid0=%lu\n",lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
   
   
      iReturn = utMsgGetSomeNVar(psMsgHead,13,"order",     UT_TYPE_STRING,12,caOrder,
																							"exppdf",    UT_TYPE_STRING,8, caPdf,
																							"exp",       UT_TYPE_STRING,6, caExport,
																							"sunrec",    UT_TYPE_STRING,8, caSum,
																							"sdate",     UT_TYPE_STRING,11,caSdate,
																							"edate",     UT_TYPE_STRING,11,caEdate,
																							"shour",     UT_TYPE_STRING,2, caShour,
																							"ehour",     UT_TYPE_STRING,2, caEhour,
																							"groupid",   UT_TYPE_STRING,20,caGroupid,
																							"service",   UT_TYPE_STRING,8, caGitem,
																							"servicecgid",UT_TYPE_STRING,8,caService,
																							"llflag",    UT_TYPE_STRING, 2, llflag,
																							"servicesum",UT_TYPE_STRING,88887,servicesum);
																							
																							

 																					
																					
																							
	 if(strlen(caService)!=0)
	 		lService = atol(caService);	
	 if(strlen(caService)==0&&strlen(caGitem)!=0)
	 	 	lService= atol(caGitem);	
	 //printf("==*== lService=%d\n",lService);		
   if(strlen(caSum)==0) strcpy(caSum,"20");
   lSumShow=atol(caSum);
   
   if(strlen(caSdate)==0){
   	lTime=time(0);
   	ncUtlWeekDate(lTime,caSdate,caEdate);

   }  
   else if(strlen(caEdate)==0){
   	lTime=time(0);
   	strcpy(caEdate,utTimFormat("%Y/%m/%d ",lTime));

   } 
   if(strlen(caShour)==0) strcpy(caShour,"00");
   if(strlen(caEhour)==0) strcpy(caEhour,"23");

   sprintf(caSdate_temp,"%s %s",caSdate,caShour);
   sprintf(caEdate_temp,"%s %s",caEdate,caEhour);         

  	psDbHead = utPltInitDb();
 
   
    utPltPutVar(psDbHead,"sunrec",caSum);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"shour",caShour);
    utPltPutVar(psDbHead,"ehour",caEhour);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    
    if(strlen(caGitem)==0&&strlen(caService)==0){
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_internet/ncwebsortint_75.htm"); 
		return 0;
	}	
    
    
    
		if((strlen(caExport)<=0)&&(strlen(caPdf)<=0)){	
			memset(servicesum,0,sizeof(servicesum)); 
		}			
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~小类id~~~~~~~~~~~服务程序~~~~~选了大类，并且选了一部分小类的情况
 //   if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
       char caVar[16];
       char *p2;
       p = utMsgGetVar_s(psMsgHead,"lsum");
       
       if(p){	       
       //判断服务中是否存在全部，如果存在全部，只插入全部，并将前面的服务记录删除   
       total=0;    		
       	 for(k=0;k<atol(p);k++){
            sprintf(caVar,"chm%d",k);           
   	        p2 = utMsgGetVar_s(psMsgHead,caVar);   	       
            if(k!=0&&p2&&(strlen(servicesum)!=0))					//第一个没选，第一个选了，
            	strcat(servicesum,",");
            if(p2){    
            	strcat(servicesum,p2);  
            	total++;
            	if(strlen(p2)==3)  
            		lService=0;
            	else	     	        
            		lService=atol(p2)/1000;              //服务ID            	
            }
   			 }
   			 if(p2)																						//最后一个是否选定
   			 		 p2 = NULL;   			   			
   		 }
   		 
   		 
 memset(caService_buf,0,sizeof(caService_buf));
 if(strlen(servicesum)>0){
 	sprintf(caTemp,"select name from ncipindex where sid in(%s) ",servicesum);
 	printf("caTemp=%s\n",caTemp);
 	psCur = pasDbOpenSql(caTemp,0);
 	 if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0152部门查询"),ncLang("0171查询数据出错"));
        return 0;
    }   
 	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,30,caName);
   																
   				       
  iNum=0;                           
   while((iReturn == 0)||(iReturn==1405)) {
   	if(strstr(caName,ncLang("0446全部"))==NULL){
   		if(iNum<3){
   		sprintf(caService_buf+strlen(caService_buf),"%s,",caName);
   	  }
   	  if(iNum>=3){
   	  	sprintf(caService_buf+strlen(caService_buf),"%s...",caName);
   	  	break;
   	  }
   	  iNum++;
     }
     	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,30,caName);
  
   }                
   pasDbCloseCursor(psCur);
 }		 
   		 
   		 
   		 
   		 
   		 
   		
   		psCur = pasDbOpenSqlF("select count(*) from ncipindex where groupid=%d order by groupid,sid",lService);
   		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&ltocount);
   		pasDbCloseCursor(psCur);	
   		  
   		if(total==ltocount) memset(servicesum,0,sizeof(servicesum)); 
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      //网络服务功能
     	psCur = pasDbOpenSqlF("select  groupid,name  from ncservicecgroup order by groupid ");
                    
	    if(psCur == NULL) {
	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
	        return 0;
	    }
     
    lCgid=0;
    memset(caName,0,sizeof(caName));
    iNum=0;
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
	  	                                	
   	while(iReturn == 0 ||iReturn==1405) {   		
    	utPltPutLoopVarF(psDbHead,"cgid",iNum+1,"%d",lCgid);
    	utPltPutLoopVar(psDbHead,"cgname",iNum+1,caName);   	
   		iNum++;   		
    	lCgid=0;
    	memset(caName,0,sizeof(caName));
	    iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG,4,&lCgid, 	                           
	  	                                UT_TYPE_STRING,31,caName);
    }  
    pasDbCloseCursor(psCur);
    //服务小类
    //网络服务列表
     	psCur = pasDbOpenSqlF("select sid,name,groupid from ncipindex order by groupid,sid");                   
	    if(psCur == NULL) {
	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
	        return 0;
	    }
    iNum=0;
      
    lSid=0;
    memset(caName,0,sizeof(caName));
    lGroupid=0;
   
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid, 	                           
	  	                               UT_TYPE_STRING,31,caName,
	  	                               UT_TYPE_LONG,4,&lGroupid);
	  	                                	
   	while(iReturn == 0 ||iReturn==1405) {  
   		if(lSid%1000!=0){		
    	utPltPutLoopVarF(psDbHead,"serviceid",iNum+1,"%d",lSid);
    	utPltPutLoopVar(psDbHead,"servicename",iNum+1,caName);
   		utPltPutLoopVarF(psDbHead,"servicegid",iNum+1,"%d",lGroupid);
   		utPltPutLoopVarF(psDbHead,"num2",iNum+1,"%d",iNum);   	
   		iNum++;   	
   	}	
    	lSid=0;
    	memset(caName,0,sizeof(caName));
    	lGroupid=0;  
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid, 	                           
	  	                               UT_TYPE_STRING,31,caName,
	  	                               UT_TYPE_LONG,4,&lGroupid);
    }  
    pasDbCloseCursor(psCur);
  	utPltPutVarF(psDbHead,"snumber","%d",iNum);
   
    if(strlen(caGroupid)>0){
    	utPltPutVar(psDbHead,"groupname",ncUtlGetGroupNameByid(psShmHead,atol(caGroupid)));
    }
    else{
    	utPltPutVar(psDbHead,"groupname",ncLang("1889点击选择"));
    }

 
   lStime=utTimStrToLong("%Y/%m/%d %H:00",caSdate_temp);
   lEtime=utTimStrToLong("%Y/%m/%d %H:59",caEdate_temp);

   pFlow=(struct ncFlowSort_s *)malloc(lSumShow*sizeof(struct ncFlowSort_s));
   if(pFlow==NULL){
   	    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0405内存分配出错"),ncLang("0405内存分配出错"));
        return 0;
   }
                       //Internet服务流量
  		lGitem=5;
  		  sprintf(sqlbuf,"select a.userid,b.dispname,inet_ntoa(b.ip) aa,c.groupname,sum(a.ubytes+a.dbytes)/1024 abyte from ncipservice a,ncuser b,ncgroup c \
                where a.userid=b.userid and b.groupid=c.groupid ");
      	if(strcmp(caSdate,caEdate)!=0)
            sprintf(sqlbuf+strlen(sqlbuf)," and ((a.sdate > '%s' and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
        else
        		sprintf(sqlbuf+strlen(sqlbuf)," and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s' ) ",caSdate,caShour,caEhour);
       
        if(lGroupid0!=0){
        	 sprintf(sqlbuf+strlen(sqlbuf)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
      
      	}
        if(strlen(caGroupid)!=0){
        	  sprintf(sqlbuf+strlen(sqlbuf)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
      
      	}
    	  if(strlen(llflag)<=0){																														//all
      		if(strlen(servicesum)!=0)																												//if small type is not null ,that's it
      			sprintf(sqlbuf+strlen(sqlbuf)," and a.service in(%s) ",servicesum);
      		else{
      			if(lService==0)
      				sprintf(sqlbuf+strlen(sqlbuf)," and (a.service=%d or a.service>100 and a.service<1000)",lService);
      			else{
      				for(t=0;t<80;t++){
					  		if(lService==t)
      						sprintf(sqlbuf+strlen(sqlbuf)," and (a.service=%d or a.service>=%d000 and a.service<%d000)",t,t,t+1);
							}
						}
					}
				}else{
					 printf("**all*");	
				}
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by a.userid,b.dispname,aa,c.groupname ");
  		  
  		  
     			strcpy(caUnit,"MB");
     			strcpy(caOrder,ncLang("0369流量"));
     			sprintf(sqlbuf+strlen(sqlbuf)," order by abyte desc ");
		 
        sprintf(sqlbuf+strlen(sqlbuf)," limit 0,%lu ",lSumShow);
  	
  	printf("== * ===sqlbuf=%s\n",sqlbuf);
  	 utPltPutVar(psDbHead,"order",caOrder);
  	
    psCur=pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
      
        iReturn = 0;
        lUserid=0;
        lBytes=0;
        lTimes=0;

        memset(caUsername,0,sizeof(caUsername));
        memset(caIp,0,sizeof(caIp));
        memset(caGroupname,0,sizeof(caGroupname));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lUserid,
                                   		 UT_TYPE_STRING,30,caUsername,
                                   		 UT_TYPE_STRING,15,caIp,
                                   		 UT_TYPE_STRING,30,caGroupname,
                                   		 UT_TYPE_LONG8, 8 ,&lBytes);
        
	     iNum=0;
	     lSumbytes=0;

	     while(iReturn==0||iReturn==1405){	     	
	     	strcpy(pFlow[iNum].caIp,caIp);
	     	pFlow[iNum].lBytes=lBytes;
	     	pFlow[iNum].lId=lUserid;
	     	strcpy(pFlow[iNum].caUsername,caUsername);
	     	strcpy(pFlow[iNum].caGroupname,caGroupname);
	     	lSumbytes=lSumbytes+lBytes;
	  
	     	iNum++;
	     	lBytes=0;
        lIp=0;
        lUserid=0;
        memset(caUsername,0,sizeof(caUsername));
        memset(caIp,0,sizeof(caIp));
        memset(caGroupname,0,sizeof(caGroupname));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lUserid,
                                   		 UT_TYPE_STRING,30,caUsername,
                                   		 UT_TYPE_STRING,15,caIp,
                                   		 UT_TYPE_STRING,30,caGroupname,
                                   		 UT_TYPE_LONG8, 8 ,&lBytes);
        
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
        fprintf(fp,ncLang("1915%-2s网络服务%s排行榜(%s--%s)\n"),"\0",caOrder,caSdate_temp,caEdate_temp);
       

	fprintf(fp,ncLang("1916序号,部门,姓名,ip地址,流量\n"));
	for(i=0;i<iNum;i++){
		 fprintf(fp,"%d,\"%s\",\"%s\",\"%s\",%0.1f\n",i+1,pFlow[i].caGroupname,pFlow[i].caUsername,pFlow[i].caIp,(float)pFlow[i].lBytes/1024);
  } 
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
         free(pFlow);
        utPltFreeDb(psDbHead);        
		    return 0;
	
}
	     
	 if(strlen(caPdf)>0){																//输出PDF文档
	     //表格标签   	                    
			 desc=malloc(lGitem*sizeof(char*));
			 for(i=0;i<lGitem;i++){
			 	desc[i]=malloc(32);
		   }
		   strcpy(desc[0],ncLang("0147部门"));
		   strcpy(desc[1],ncLang("0692姓名"));
		   strcpy(desc[2],ncLang("1887IP地址"));		  
	
		   sprintf(desc[3],"%s(%s)",ncLang("1103总流量"),"MB");
		   strcpy(desc[4],ncLang("1894流量百分比(%)"));
		 			
		  
	     	 //输出到PDF中    
		   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*iNum);
		    if(pChart==NULL){
		  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("1863分配内存出错"));
		      return 0;
		    }
		  for(i=0;i<iNum;i++){
		  	  	//输出表格的补充字段 				    				  
  				  pChart[i].append=malloc((lGitem-3)*sizeof(char*));
  				  for(m=0;m<lGitem-3;m++){														//lGitem=5,m<2
  				  	pChart[i].append[m]=malloc(32);
  				  }
  						pChart[i].name=strdup(pFlow[i].caGroupname);  
  					
  					//按流量排序     ---对应上门汉字顺序，-次数-流量-流量百分比   
  						sprintf(pChart[i].append[0],pFlow[i].caUsername);
  						sprintf(pChart[i].append[1],pFlow[i].caIp);
	 				 			
  				//百分比				按流量排序     
        			pChart[i].amount=(float)pFlow[i].lBytes/1024; 	       
              if(lSumbytes==0)
          	  	pChart[i].percent=strdup("0.00");
          		else{
          			sprintf(caTemp,"%0.2f",(float)100*pFlow[i].lBytes/lSumbytes);
          			pChart[i].percent=strdup(caTemp);
          		} 
		 					     
        }       	
	     	rowrow=iNum;    
        sprintf(filename,"%s.PDF","flowsort"); 	   
     							//按流量排序     
        	strcpy(caTitle,ncLang("1917网络服务流量排行榜"));
		 
      sprintf(caStatdate,ncLang("1897统计时间：从%s %s 到%s %s"),caSdate,caShour,caEdate,caEhour);  
      ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,lGitem,lGitem-3, caTitle,caUnit,caStatdate,filename); 	     	
	     	//释放内存     
		   for(i=0;i<rowrow;i++){

			      free(pChart[i].name);
			      free(pChart[i].percent);
			      for(m=0;m<lGitem-3;m++){
	      	     free(pChart[i].append[m]);
	          }
	          	         
	          if(pChart[i].append) free(pChart[i].append);
		   }  
			   if(pChart) free(pChart);			   
			   for(i=0;i<lGitem;i++){
			   	   free(desc[i]);
			   }
		    if(desc) free(desc);  
         	 free(pFlow);   
           utPltFreeDb(psDbHead);
	     	   return 0;	     		
	      }

//输出到界面	   
	   for(i=0;i<iNum;i++){                 							
         utPltPutLoopVarF(psDbHead,"data",i+1,"%0.0f",((float)pFlow[i].lBytes/1024));					//按流量排序     
				      
         utPltPutLoopVarF(psDbHead,"num",i+1,"%d",i);
         utPltPutLoopVarF(psDbHead,"num1",i+1,"%02d",i+1);
         utPltPutLoopVarF(psDbHead,"uid",i+1,"%d",pFlow[i].lId);        
         utPltPutLoopVarF(psDbHead,"dbytes",i+1,"%0.1f",(float)pFlow[i].lBytes/1024);
         utPltPutLoopVar(psDbHead,"ip",i+1,pFlow[i].caIp);
         utPltPutLoopVar(psDbHead,"name",i+1,pFlow[i].caUsername);
         utPltPutLoopVar(psDbHead,"dept",i+1,pFlow[i].caGroupname);   
      }
      
     utPltPutVar(psDbHead,"unit",caUnit);    
     if(strlen(caService)==0)
     		utPltPutVar(psDbHead,"service",caGitem); 
	 	 else
     		utPltPutVar(psDbHead,"service",caService); 
     utPltPutVar(psDbHead,"servicesum",servicesum);    
     free(pFlow);
     memset(servicesum,0,sizeof(servicesum)); 
     
     utPltPutVar(psDbHead,"service_buf",caService_buf);
     //utPltShowDb(psDbHead);    
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_internet/ncwebsortint_75.htm"); 
   return 0;
}

//Internet服务流量排行榜明细
int ncWebShowintlog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caTemp[2048];
    char caCond[1024],caSorta[12],caOrdera[12];
    char caGroupid[16],caSip[20];
    char caUserid[16],caUsername[32],caMac[24],caSdate[24],caEdate[24];
    char caStime[10];
    char caTotPg[16],caCurPg[16];
    char caUdip[64],caDep[64];
    char caIdfyno[20];
    unsigned long lUserid,lCtime;
    long long lUbytes,lDbytes;
    //unsigned long lStime;
    //unsigned long lEtime;
    char lStime[12];
    char lEtime[3];
    unsigned long lIp;
    unsigned long lCurPg,lTotPg;
    char caUbyte[16],caDbyte[16];
    char caTime[24];
    int lCount;
    int i,iNum;
    char caIp[20],caContent[256],caAttfile[128];
   	char ids[256];
   	int i2,lUserid2;
    char caDel[16],caExport[16];
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lRowNum,lStartRec,lTotRec;
 	  pasDbCursor *psCur;
 	  ncUserCont *psUser;
    char caShour[12],caEhour[12],uname[22],caSdate_temp[20],caEdate_temp[20];
 	  char caTitlename[32];
 	 
 	  int group_flag,lGroupid0;
 		char caHost[64];
 	  char caName[32];
 	  long de_cout;
 	  char de_Cout[20];
 	  unsigned long lTimes;
 	  char service[32];
 	  char servicesum[88888];
 	  char caSdate_t[20],caStime_t[8];
 	  char caSdate_all[32];
 	  long lService;
 	  long t;
 	  char caService_buf[1024];
 	//utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,12,"TotPg",UT_TYPE_STRING, 12, caTotPg,
																				  "CurPg",UT_TYPE_STRING, 12, caCurPg,
																				  "uid",  UT_TYPE_STRING, 12, caUserid,
																				  "sdate",UT_TYPE_STRING,19,caSdate,
																				  "edate",UT_TYPE_STRING,19,caEdate,
																				  "shour",UT_TYPE_STRING,2,caShour,
																				  "ehour",UT_TYPE_STRING,2,caEhour,
																				  "groupid",UT_TYPE_STRING,15,caGroupid,
																				  "uname",  UT_TYPE_STRING,20,uname,
   	                                      "exp",  UT_TYPE_STRING,10,caExport,
   	                                      "service",  UT_TYPE_STRING,30,service,
   	                                      "servicesum",  UT_TYPE_STRING,88887,servicesum);

 
   memset(caCond,0,sizeof(caCond));
   
   memset(caTitlename,0,sizeof(caTitlename));
   
   if(strlen(caUserid)!=0){   	
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));    
    }
  
    sprintf(caSdate_temp,"%s %s:00",caSdate,caShour);
    sprintf(caEdate_temp,"%s %s:59",caEdate,caEhour);
   
	//	sprintf(caCond+strlen(caCond)," and a.stime>=%lu ",utTimStrToLong("%Y/%m/%d %H:%M",caSdate_temp));
	//	sprintf(caCond+strlen(caCond)," and a.stime<=%lu ",utTimStrToLong("%Y/%m/%d %H:%M",caEdate_temp));
   
   
  		sprintf(caCond+strlen(caCond)," and a.userid=%s ",caUserid);
			if(strlen(servicesum)!=0)
   				sprintf(caCond+strlen(caCond)," and a.service in(%s) ",servicesum);
   		else{
   				//sprintf(caCond+strlen(caCond)," and a.service=%s  ",service);
   				if(atoi(service)==0)
      				sprintf(caCond+strlen(caCond)," and (a.service=%d or a.service>100 and a.service<1000)",atoi(service));
      		else{
      				for(t=0;t<80;t++){
					  		if(atoi(service)==t)
      						sprintf(caCond+strlen(caCond)," and (a.service=%d or a.service>=%d000 and a.service<%d000)",t,t,t+1);
							}
					}
  	  }
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;

    if(!utStrIsSpaces(caExport)){
    lRowNum = 99999;
    }
    else
    lRowNum=32;
    if(!utStrIsSpaces(caExport))
    	lStartRec=0;
    else	
    	lStartRec = (lCurPg - 1) * lRowNum;    


    sprintf(caTemp,"select count(*) from ncuser b,ncipservice a  where  b.userid=a.userid  ");    
 
 		if(strcmp(caSdate,caEdate)!=0)
    sprintf(caTemp+strlen(caTemp)," and ((a.sdate > '%s' and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  else
    sprintf(caTemp+strlen(caTemp)," and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s' ) ",caSdate,caShour,caEhour);

   if(strlen(caCond)!=0)
  		 strcat(caTemp,caCond);
		//strcat(caTemp,"  order by c.sdate desc ");
	 printf("=1= caTemp=%s\n",caTemp);
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);

    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0152部门查询"),ncLang("0171查询数据出错"));
        return 0;
    }  
    
  sprintf(caTemp,"select b.userid,b.ip,b.mac,(a.ubytes+a.dbytes)/1024,a.sdate,a.stime,a.service from ncuser b,ncipservice a  where  b.userid=a.userid  ");    
  
  if(strcmp(caSdate,caEdate)!=0)
    sprintf(caTemp+strlen(caTemp)," and ((a.sdate > '%s' and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  else
    sprintf(caTemp+strlen(caTemp)," and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s' ) ",caSdate,caShour,caEhour);
       
//  if(lGroupid0!=0){
//    sprintf(caTemp+strlen(caTemp)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
// 	}
  	if(strlen(caGroupid)!=0){
      sprintf(caTemp+strlen(caTemp)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
  	}
  	
    if(strlen(caCond)!=0)
   		 strcat(caTemp,caCond);
   
	//strcat(caTemp,"  order by c.sdate desc,c.stime desc ");

    sprintf(caTemp+strlen(caTemp)," limit %lu,%lu ",lStartRec,lRowNum);
		printf("=2= caTemp=%s\n",caTemp);
    psCur = pasDbOpenSql(caTemp,0);

    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0152部门查询"),ncLang("0171查询数据出错"));
        return 0;
    }   

    if(!utStrIsSpaces(caExport)){
    		FILE *fp;
    		char caFilename[32];
    		char caFile[128];
    		char caPath[128];
        sprintf(caFilename,"ncnetuser%d.csv",utStrGetId());
        sprintf(caPath,"%s/html","/home/ncmysql/nc");
        sprintf(caFile,"%s/%s",caPath,caFilename);
    		fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
            return 0;
        }
        
        fprintf(fp,ncLang("1101用户上网日志"));
      	ncUtlPrintCsv(fp,7,UT_TYPE_STRING,ncLang("0147部门"),
                         	 UT_TYPE_STRING,ncLang("0728用户名"),
                         	 UT_TYPE_STRING,ncLang("0054IP地址"),
                         	 UT_TYPE_STRING,ncLang("0072MAC地址"),
                         	 UT_TYPE_STRING,ncLang("1100上网时间"),
                         	 UT_TYPE_STRING,ncLang("1918服务名"),
                         	 UT_TYPE_STRING,ncLang("1909总流量KBytes"));

                                      
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;     
       lIp=0;
       lUbytes=0;
       lTimes=0;
       memset(caSdate_t,0,sizeof(caSdate_t));
       memset(caStime_t,0,sizeof(caStime_t));
       lService=0;
   			iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 		 UT_TYPE_ULONG,4,&lIp,
   																 		 UT_TYPE_STRING,19,caMac,
   																 		 UT_TYPE_LONG8, 8, &lUbytes,
   																 		 UT_TYPE_STRING,10,caSdate_t,
   																 		 UT_TYPE_STRING,2,caStime_t,
   																 		 UT_TYPE_LONG,4,&lService);
   																
   				       
  	                              
        while((iReturn == 0)||(iReturn==1405)) {
     
        		psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
		     		if(psUser){   
          		strcpy(caUdip,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
          		if(psUser->psGroup)
          			strcpy(caDep,psUser->psGroup->groupname);         
        			}else{
        				strcpy(caUdip,"");
        				strcpy(caDep,"");       
       			  }
        		//strcpy(caTime,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
        		
        		
        		strcpy(caUbyte,utStrLtoF8(lUbytes,caTemp));
        		if(strlen(caName)==0)
        	 		strcpy(caName,ncLang("1908未知类别"));
        	 		
        	 		
        	 	sprintf(caSdate_all,"%s %s",caSdate_t,caStime_t);	
        	 		
        		ncUtlPrintCsv(fp,7,UT_TYPE_STRING,caDep,
            		           		 UT_TYPE_STRING,caUdip,
                		       		 UT_TYPE_STRING,utComHostIp(htonl(lIp)),
                    		   		 UT_TYPE_STRING,caMac,
                    		   		 UT_TYPE_STRING,caSdate_all,
                    		   		 UT_TYPE_STRING,ncUtlGetServiceName(psShmHead,lService,ncLang("0431其它")),
                       		  	 UT_TYPE_STRING,caUbyte); 
                                          
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;     
       lIp=0;
       lUbytes=0;
       lTimes=0;
       memset(caSdate_t,0,sizeof(caSdate_t));
       memset(caStime_t,0,sizeof(caStime_t));
       lService=0;
   			iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 		 UT_TYPE_ULONG,4,&lIp,
   																 		 UT_TYPE_STRING,19,caMac,
   																 		 UT_TYPE_LONG8, 8, &lUbytes,
   																 		 UT_TYPE_STRING,10,caSdate_t,
   																 		 UT_TYPE_STRING,2,caStime_t,
   																 		 UT_TYPE_LONG,4,&lService);
        }
        
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);         
        return 0;
    }
        
    psDbHead = utPltInitDb();
    
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"shour",caShour);
    utPltPutVar(psDbHead,"ehour",caEhour);
    utPltPutVar(psDbHead,"userid",caUserid);
    
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;     
       lIp=0;
       lUbytes=0;
       lTimes=0;
       memset(caSdate_t,0,sizeof(caSdate_t));
       memset(caStime_t,0,sizeof(caStime_t));
       lService=0;
   			iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 		 UT_TYPE_ULONG,4,&lIp,
   																 		 UT_TYPE_STRING,19,caMac,
   																 		 UT_TYPE_LONG8, 8, &lUbytes,
   																 		 UT_TYPE_STRING,10,caSdate_t,
   																 		 UT_TYPE_STRING,2,caStime_t,
   																 		 UT_TYPE_LONG,4,&lService);
   	iNum = 0;   	
   	while(iReturn == 0||iReturn==1405 && iNum < lRowNum) {
        iNum++;
   
//   printf("lUserid=%d\n",lUserid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%d",lUserid);
         psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
		     if(psUser){   
          strcpy(caUdip,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
//          printf("caUdip=%s\n",caUdip);
          if(psUser->psGroup)
             strcpy(caDep,psUser->psGroup->groupname);
           else
             strcpy(caDep,"");
 //         printf("caDp=%s\n",caDep);
          strcpy(caIp,utComHostIp(htonl(psUser->ip)));
          utPltPutLoopVar(psDbHead,"position",iNum,psUser->position);
        }else{
        	strcpy(caUdip,"");
        	strcpy(caDep,"");
        	strcpy(caIp,"");
        }
        
        utPltPutLoopVar(psDbHead,"dept",   iNum,caDep);
        utPltPutLoopVar(psDbHead,"username",iNum,caUdip);
        utPltPutLoopVarF(psDbHead,"sdate",iNum,"%s %s",caSdate_t,caStime_t);
        utPltPutLoopVar(psDbHead,"servname",iNum,ncUtlGetServiceName(psShmHead,lService,ncLang("0431其它")));
     
//        sprintf(caTime,"%s %s",caSdate,caStime);
        strcpy(caUbyte,utStrLtoF8(lUbytes,caTemp));
        
        utPltPutLoopVar(psDbHead,"ip",    iNum,utComHostIp(htonl(lIp)));
        
        //utPltPutLoopVarF(psDbHead,"ubytes", iNum,"%d",lUbytes);
        utPltPutLoopVar(psDbHead,"ubytes",iNum,caUbyte);
        utPltPutLoopVar(psDbHead,"mac",iNum,caMac);      
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;     
       lIp=0;
       lUbytes=0;
       lTimes=0;
       memset(caSdate_t,0,sizeof(caSdate_t));
       memset(caStime_t,0,sizeof(caStime_t));
       lService=0;
   			iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 		 UT_TYPE_ULONG,4,&lIp,
   																 		 UT_TYPE_STRING,19,caMac,
   																 		 UT_TYPE_LONG8, 8, &lUbytes,
   																 		 UT_TYPE_STRING,10,caSdate_t,
   																 		 UT_TYPE_STRING,2,caStime_t,
   																 		 UT_TYPE_LONG,4,&lService);
    }
    pasDbCloseCursor(psCur);

    lTotPg = (lCount - 1) / lRowNum + 1;
    lTotRec = lCount;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
  
    utPltPutVar(psDbHead,"cond",caCond);

    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);

    utPltPutVar(psDbHead,"title_name",uname);
		utPltPutVar(psDbHead,"service",service);
		utPltPutVar(psDbHead,"servicesum",servicesum);

     utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_internet/nc_sortintloglist.htm");
    return 0;
}
