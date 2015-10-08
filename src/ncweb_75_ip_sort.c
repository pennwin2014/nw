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
/* 目标ip连接--时间--流量排行榜 */
int ncWebSortip_75(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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

    struct ncFlowSort_s {
    		unsigned long lId;
        char lIp[32];
        unsigned long lip2;
        unsigned long long lBytes;
        unsigned long lTimes;       
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
   lTime=time(0);
   lSum=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    printf("== lGroupid0=%lu\n",lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
   
   
      iReturn = utMsgGetSomeNVar(psMsgHead,10,"order",  UT_TYPE_STRING,12,caOrder,
																							"exppdf", UT_TYPE_STRING,8, caPdf,
																							"exp",    UT_TYPE_STRING,6, caExport,
																							"sunrec", UT_TYPE_STRING,8, caSum,
																							"sdate",  UT_TYPE_STRING,11,caSdate,
																							"edate",  UT_TYPE_STRING,11,caEdate,
																							"shour",  UT_TYPE_STRING,2, caShour,
																							"ehour",  UT_TYPE_STRING,2, caEhour,
																							"groupid",UT_TYPE_STRING,20,caGroupid,
																							"gitem",  UT_TYPE_STRING,8, caGitem);
					
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
   if(strlen(caGitem)==0) strcpy(caGitem,"100");
   sprintf(caSdate_temp,"%s %s",caSdate,caShour);
   sprintf(caEdate_temp,"%s %s",caEdate,caEhour);         

  	psDbHead = utPltInitDb();
 
   
    utPltPutVar(psDbHead,"sunrec",caSum);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"shour",caShour);
    utPltPutVar(psDbHead,"ehour",caEhour);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    if(strlen(caGroupid)>0){
    	utPltPutVar(psDbHead,"groupname",ncUtlGetGroupNameByid(psShmHead,atol(caGroupid)));
    }
    else{
    	utPltPutVar(psDbHead,"groupname",ncLang("1889点击选择"));
    }
    utPltPutVar(psDbHead,"gitem",caGitem);
 
   lStime=utTimStrToLong("%Y/%m/%d %H:00",caSdate_temp);
   lEtime=utTimStrToLong("%Y/%m/%d %H:59",caEdate_temp);

   pFlow=(struct ncFlowSort_s *)malloc(lSumShow*sizeof(struct ncFlowSort_s));
   if(pFlow==NULL){
   	    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0405内存分配出错"),ncLang("0405内存分配出错"));
        return 0;
   }
                       //目的IP连接统计
  		lGitem=4;
  		  sprintf(sqlbuf,"select a.ip,sum(a.ubytes+a.dbytes)/1024 abyes,sum(a.ctime) atime from ncipcount a,ncuser b,ncgroup c \
                where a.userid=b.userid and b.groupid=c.groupid ");
      	if(strcmp(caSdate,caEdate)!=0)
            sprintf(sqlbuf+strlen(sqlbuf)," and ((a.sdate > '%s' and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
        else
        		sprintf(sqlbuf+strlen(sqlbuf)," and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s' ) ",caSdate,caShour,caEhour);
       
        if(lGroupid0!=0){
        	 sprintf(sqlbuf+strlen(sqlbuf)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
      
      	}
        if(strlen(caGroupid)!=0){
        	if(atol(caGroupid)==0){
        		sprintf(sqlbuf+strlen(sqlbuf)," and (b.groupid=0) ");
        	}
        	else
        	  sprintf(sqlbuf+strlen(sqlbuf)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
      
      	}
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by a.ip  ");
  		  
  		  if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T') {
        	strcpy(caOrder,ncLang("0521时间"));
        	sprintf(sqlbuf+strlen(sqlbuf)," order by atime desc ");          
        	strcpy(caUnit,ncLang("0393秒")); 
     		}else{ 
     			strcpy(caUnit,"MB");
     			strcpy(caOrder,ncLang("0369流量"));
     			sprintf(sqlbuf+strlen(sqlbuf)," order by abyes desc ");
     		}  		 
        sprintf(sqlbuf+strlen(sqlbuf)," limit 0,%lu ",lSumShow);
  	
  	printf("== * ===sqlbuf=%s\n",sqlbuf);
  	 utPltPutVar(psDbHead,"order",caOrder);
  	
    psCur=pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
      
        iReturn = 0;
        lBytes=0;
        lTimes=0;
        lIp=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4, &lIp,
                                   		 UT_TYPE_LONG8,8, &lBytes,
                                   		 UT_TYPE_ULONG,4, &lTimes);
                                     
	     iNum=0;
	     lSumbytes=0;
	     lSumtime=0;
	     while(iReturn==0||iReturn==1405){	     	
	     	strcpy(pFlow[iNum].lIp,utComHostIp(htonl(lIp)));
	     	pFlow[iNum].lBytes=lBytes;
	     	pFlow[iNum].lTimes=lTimes;
	     	pFlow[iNum].lip2=lIp;
	     	lSumbytes=lSumbytes+lBytes;
	     	lSumtime=lSumtime+lTimes;
	     	iNum++;
	     	lBytes=0;
        lTimes=0;
        lIp=0;
        
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4, &lIp,
                                   		 UT_TYPE_LONG8,8, &lBytes,
                                   		 UT_TYPE_ULONG,4, &lTimes);
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
        fprintf(fp,ncLang("1910%-2s目标ip连接%s排行榜(%s--%s)\n"),"\0",caOrder,caSdate_temp,caEdate_temp);
       

	fprintf(fp,ncLang("1911序号,ip,流量,时间\n"));
	for(i=0;i<iNum;i++){
		 fprintf(fp,"%d,\"%s\",%0.1f,%lu\n",i+1,pFlow[i].lIp,(float)pFlow[i].lBytes/1024,pFlow[i].lTimes);
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
		   		strcpy(desc[0],"ip");		  
		 
		    	if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        		strcpy(desc[1],ncLang("1912连接时间(分)"));
		   	 		sprintf(desc[2],"%s(%s)",ncLang("1103总流量"),"MB");
		   			strcpy(desc[3],ncLang("1894流量百分比(%)"));
		 			}else{																																								//按时间排序     
						strcpy(desc[1],ncLang("1891上网流量(MB)"));
		   	 		sprintf(desc[2],"%s(%s)",ncLang("0365连接时间"),ncLang("0250分"));
		   			strcpy(desc[3],ncLang("1892时间百分比(%)"));
		 			}
		 
		  
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
  					pChart[i].name=strdup(pFlow[i].lIp);  
  					if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     ---对应上门汉字顺序，-次数-流量-流量百分比   
        			sprintf(pChart[i].append[0],"%0.0f",(float)pFlow[i].lTimes/60);
		 				}else{	   					  																																//按时间排序     			
        			sprintf(pChart[i].append[0],"%0.1f",(float)pFlow[i].lBytes/1024);				
		 				}
  			
  				//百分比
  				if  (strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        			pChart[i].amount=(float)pFlow[i].lBytes/1024; 	       
              if(lSumbytes==0)
          	  	pChart[i].percent=strdup("0.00");
          		else{
          			sprintf(caTemp,"%0.2f",(float)100*pFlow[i].lBytes/lSumbytes);
          			pChart[i].percent=strdup(caTemp);
          		} 
		 			}else{																																								//按时间排序     							
							pChart[i].amount=(float)pFlow[i].lTimes/60;
          		if(lSumtime==0)
          	  	pChart[i].percent=strdup("0.00");
          		else{
          			sprintf(caTemp,"%0.2f",(float)100*pFlow[i].lTimes/lSumtime);
          			pChart[i].percent=strdup(caTemp);
          		}
		 			}			 			     
        }       	
	     	rowrow=iNum;    
        sprintf(filename,"%s.PDF","flowsort"); 	   
     if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        	strcpy(caTitle,ncLang("1913目标ip连接流量排行榜"));
		 }else{																																								//按次数排序     
					strcpy(caTitle,ncLang("1914目标ip连接时间排行榜"));
		 }
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
         
        if  (strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        		utPltPutLoopVarF(psDbHead,"data",i+1,"%0.0f",((float)pFlow[i].lBytes/1024));
				}else{																																								//按次数排序     
						 utPltPutLoopVarF(psDbHead,"data",i+1,"%lu",pFlow[i].lTimes);
				}       
         utPltPutLoopVarF(psDbHead,"num",i+1,"%d",i);
         utPltPutLoopVarF(psDbHead,"num1",i+1,"%02d",i+1);
         utPltPutLoopVarF(psDbHead,"uid",i+1,"%d",pFlow[i].lId);        
         utPltPutLoopVarF(psDbHead,"dbytes",i+1,"%0.1f",(float)pFlow[i].lBytes/1024);
         utPltPutLoopVar(psDbHead,"ip",i+1,pFlow[i].lIp);
         utPltPutLoopVarF(psDbHead,"time",i+1,"%0.0f",(float)pFlow[i].lTimes/60);  
         utPltPutLoopVarF(psDbHead,"ip2",i+1,"%lu",pFlow[i].lip2);             
      }
      
     utPltPutVar(psDbHead,"unit",caUnit);    
     free(pFlow);
   
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_ip/ncwebsortip_75.htm"); 
   return 0;
}
//目标ip连接--时间--流量明细
int ncWebShowiplog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    unsigned long lUbytes,lDbytes;
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
    char caShour[12],caEhour[12],caGit[12],caSdate_temp[20],caEdate_temp[20];
 	  char caTitlename[32];
 	 
 	  int group_flag,lGroupid0;
 		char caHost[64];
 	  char caName[32];
 	  long de_cout;
 	  char de_Cout[20];
 	  unsigned long lTimes;
 	  char swrq[16],swsj[12],sj[32];
 	  char caIP[32];
 	//utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,11,"TotPg",UT_TYPE_STRING, 12, caTotPg,
																				  "CurPg",UT_TYPE_STRING, 12, caCurPg,
																				  "uid",  UT_TYPE_STRING, 12, caUserid,
																				  "sdate",UT_TYPE_STRING,19,caSdate,
																				  "edate",UT_TYPE_STRING,19,caEdate,
																				  "shour",UT_TYPE_STRING,2,caShour,
																				  "ehour",UT_TYPE_STRING,2,caEhour,
																				  "groupid",UT_TYPE_STRING,15,caGroupid,
																				  "git",  UT_TYPE_STRING,8,caGit,
   	                                      "exp",  UT_TYPE_STRING,10,caExport,
   	                                      "ip",  UT_TYPE_STRING,30,caIP);
   printf("== caSdate=%s\n",caSdate);	  
   printf("== caEdate=%s\n",caEdate);
   printf("== caIP=%s\n",caIP);
   printf("== caGroupid=%s\n",caGroupid);
   printf("== lGroupid0=%d\n",lGroupid0);   
   memset(caCond,0,sizeof(caCond));
   
   memset(caTitlename,0,sizeof(caTitlename));
   
   if(strlen(caUserid)!=0){
   	if(atol(caGit)==101)
     	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caUserid),ncLang("0629未知")));
   	else
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    
    }
  
    sprintf(caSdate_temp,"%s %s:00",caSdate,caShour);
    sprintf(caEdate_temp,"%s %s:59",caEdate,caEhour);
   
	//	sprintf(caCond+strlen(caCond)," and a.stime>=%lu ",utTimStrToLong("%Y/%m/%d %H:%M",caSdate_temp));
	//	sprintf(caCond+strlen(caCond)," and a.stime<=%lu ",utTimStrToLong("%Y/%m/%d %H:%M",caEdate_temp));
   
   if(atol(caGit)==101){
   		sprintf(caCond+strlen(caCond)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",atol(caUserid),atol(caUserid));
   }else{
  		sprintf(caCond+strlen(caCond)," and c.ip=%s ",caIP);
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


    sprintf(caTemp,"select count(*) from ncuser b,ncipcount c  where  b.userid=c.userid "); 
 
 		if(strcmp(caSdate,caEdate)!=0)
     	sprintf(caTemp+strlen(caTemp)," and ((c.sdate > '%s' and c.sdate < '%s') or (c.sdate='%s' and c.stime>='%s' ) or (c.sdate='%s' and c.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	else
     	sprintf(caTemp+strlen(caTemp)," and (c.sdate='%s' and c.stime>='%s' and c.stime<='%s' ) ",caSdate,caShour,caEhour);
       
//  	if(lGroupid0!=0){
//    	sprintf(caTemp+strlen(caTemp)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
// 	  }
  	if(strlen(caGroupid)!=0){
      sprintf(caTemp+strlen(caTemp)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
  	}
  // sprintf(caTemp+strlen(caTemp)," and b.userid=%d ",atol(caUserid)); 
   if(strlen(caCond)!=0)
  		 strcat(caTemp,caCond);
		strcat(caTemp,"  order by c.sdate desc ");
	 printf("=1= caTemp=%s\n",caTemp);
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);

    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0152部门查询"),ncLang("0171查询数据出错"));
        return 0;
    }  
    
  sprintf(caTemp,"select c.sdate,c.stime,b.userid,c.ctime,b.ip,b.mac,(c.ubytes+c.dbytes)/1024 from ncipcount c,ncuser b  where  c.userid=b.userid  ");    
  
  if(strcmp(caSdate,caEdate)!=0)
    sprintf(caTemp+strlen(caTemp)," and ((c.sdate > '%s' and c.sdate < '%s') or (c.sdate='%s' and c.stime>='%s' ) or (c.sdate='%s' and c.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  else
    sprintf(caTemp+strlen(caTemp)," and (c.sdate='%s' and c.stime>='%s' and c.stime<='%s' ) ",caSdate,caShour,caEhour);
       
//  if(lGroupid0!=0){
//    sprintf(caTemp+strlen(caTemp)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
// 	}
  	if(strlen(caGroupid)!=0){
  		if(atol(caGroupid)==0){
  			sprintf(caTemp+strlen(caTemp)," and b.groupid=0 ");
  		}
  			else
      sprintf(caTemp+strlen(caTemp)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
  	}
  	
 // sprintf(caTemp+strlen(caTemp)," and b.userid=%d ",atol(caUserid)); 
    if(strlen(caCond)!=0)
   		 strcat(caTemp,caCond);
   
	strcat(caTemp,"  order by c.sdate desc,c.stime desc ");

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
      	ncUtlPrintCsv(fp,6,UT_TYPE_STRING,ncLang("0147部门"),
                         	 UT_TYPE_STRING,ncLang("0728用户名"),
                         	 UT_TYPE_STRING,ncLang("0054IP地址"),
                         	 UT_TYPE_STRING,ncLang("0072MAC地址"),
                         	 UT_TYPE_STRING,ncLang("1100上网时间"),
                         	 UT_TYPE_STRING,ncLang("1909总流量KBytes"));

                                      
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;     
       lIp=0;
       lUbytes=0;
       lTimes=0;
       memset(swrq,0,sizeof(swrq));
       memset(swsj,0,sizeof(swsj));
   			iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,10,swrq,
   			                               UT_TYPE_STRING,2,swsj,
   			                               UT_TYPE_LONG,4,&lUserid,
   																 		 UT_TYPE_ULONG,4, &lTimes,
   																 		 UT_TYPE_ULONG,4,&lIp,
   																 		 UT_TYPE_STRING,19,caMac,
   																 		 UT_TYPE_ULONG, 4, &lUbytes);
   																
   				       
  	                              
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
        	sprintf(sj,"%s %s",swrq,swsj);	
        		strcpy(caUbyte,utStrLtoF(lUbytes,caTemp));
   //     		sprintf(de_Cout,"%d",lTimes);
        		if(strlen(caName)==0)
        	 		strcpy(caName,ncLang("1908未知类别"));
        		ncUtlPrintCsv(fp,6,UT_TYPE_STRING,caDep,
            		           		 UT_TYPE_STRING,caUdip,
                		       		 UT_TYPE_STRING,utComHostIp(htonl(lIp)),
                    		   		 UT_TYPE_STRING,caMac,
                       				 UT_TYPE_STRING,sj,
                       		  	 UT_TYPE_STRING,caUbyte); 
                                          
       			memset(caMac,0,sizeof(caMac));      
       			memset(caDep,0,sizeof(caDep));   
       			lUserid=0;       
       			lIp=0;
       			lUbytes=0;
       			lTimes=0;       
       memset(swrq,0,sizeof(swrq));
       memset(swsj,0,sizeof(swsj));
   			iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,10,swrq,
   			                               UT_TYPE_STRING,2,swsj,
   			                               UT_TYPE_LONG,4,&lUserid,
   																 		 UT_TYPE_ULONG,4, &lTimes,
   																 		 UT_TYPE_ULONG,4,&lIp,
   																 		 UT_TYPE_STRING,19,caMac,
   																 		 UT_TYPE_ULONG, 4, &lUbytes);
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
    utPltPutVar(psDbHead,"git",caGit);
    utPltPutVar(psDbHead,"userid",caUserid);
    
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;      
       lIp=0;
       lUbytes=0;     
       lTimes=0; 
       memset(swrq,0,sizeof(swrq));
       memset(swsj,0,sizeof(swsj));
   			iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,10,swrq,
   			                               UT_TYPE_STRING,2,swsj,
   			                               UT_TYPE_LONG,4,&lUserid,
   																 		 UT_TYPE_ULONG,4, &lTimes,
   																 		 UT_TYPE_ULONG,4,&lIp,
   																 		 UT_TYPE_STRING,19,caMac,
   																 		 UT_TYPE_ULONG, 4, &lUbytes);
   	iNum = 0;   	
   	while(iReturn == 0||iReturn==1405 && iNum < lRowNum) {
        iNum++;
   
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
     
//        sprintf(caTime,"%s %s",caSdate,caStime);
        strcpy(caUbyte,utStrLtoF(lUbytes,caTemp));
        
        utPltPutLoopVar(psDbHead,"ip",    iNum,utComHostIp(htonl(lIp)));
        
        utPltPutLoopVar(psDbHead,"swrq",iNum,swrq);
        utPltPutLoopVar(psDbHead,"swsj",iNum,swsj);
        //utPltPutLoopVarF(psDbHead,"ubytes", iNum,"%d",lUbytes);
        utPltPutLoopVar(psDbHead,"ubytes",iNum,caUbyte);
        utPltPutLoopVar(psDbHead,"mac",iNum,caMac);
        utPltPutLoopVarF(psDbHead,"de_cout",iNum,"%d",lTimes);        
        memset(caMac,0,sizeof(caMac));      
        memset(caDep,0,sizeof(caDep));   
        lUserid=0;      
        lIp=0;
        lUbytes=0;
        lTimes=0;
       
       memset(swrq,0,sizeof(swrq));
       memset(swsj,0,sizeof(swsj));
   			iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,10,swrq,
   			                               UT_TYPE_STRING,2,swsj,
   			                               UT_TYPE_LONG,4,&lUserid,
   																 		 UT_TYPE_ULONG,4, &lTimes,
   																 		 UT_TYPE_ULONG,4,&lIp,
   																 		 UT_TYPE_STRING,19,caMac,
   																 		 UT_TYPE_ULONG, 4, &lUbytes);
    }
    pasDbCloseCursor(psCur);
    
//printf("pid=%d\n",getpid());
//sleep(15);  
 
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

    utPltPutVar(psDbHead,"title_name",caTitlename);
		utPltPutVar(psDbHead,"ip",caIP);
     utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_ip/nc_sortiploglist.htm");
    return 0;
}
