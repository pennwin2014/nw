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
/* 网站访问次数 */
int ncWebSortWeb_75(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
        unsigned long lId;
        unsigned long long lBytes;
        unsigned long long l_Count;
        char caName[32];
        char caHost[65];
        long Webid;
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

    struct piechart_datasetf *pChart;           //用于PDF文档输出

    int rowrow;                     
    int column; 

    char caTitle[128];
    char filename[128];
    column=4;
    char **desc;
    char caStatdate[128];
    char caUsername[32],caIp[20],caGroupname[32];
    long Webid;
    char caGroupids[4048];
    long groupids[100];
    char caGroupid2[20];
    
    
   lTime=time(0);
   lSum=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
//    printf("== lGroupid0=%lu\n",lGroupid0);
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
		
		if(atol(caGroupid)>0){
			sprintf(caGroupids,"%s",caGroupid);
			strcpy(caGroupid2,caGroupid);
	//取子部门的ID
//printf("select groupid from ncgroup where pid=%s limit 0,99\n",caGroupid2);
	   		psCur=pasDbOpenSqlF("select groupid from ncgroup where pid=%s limit 0,99 ",caGroupid2);
	   		lGroupid=0;
	   		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid);
                                                                   
	     iNum=0;
	   
	     while(iReturn==0||iReturn==1405){
	     	sprintf(caGroupids+strlen(caGroupids),",%d",lGroupid);
	      groupids[iNum]=lGroupid;
	   		lGroupid=0;
	   		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid);
	   		iNum++;
	    }
	     pasDbCloseCursor(psCur); 
	     for(i=0;i<iNum;i++){
//	     	printf("select groupid from ncgroup where pid=%d limit 0,99\n ",groupids[i]);
	     		  psCur=pasDbOpenSqlF("select groupid from ncgroup where pid=%d limit 0,99 ",groupids[i]);
			   		lGroupid=0;
			   		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid);
		                                                                   
			     
			   
			     while(iReturn==0||iReturn==1405){
			     	sprintf(caGroupids+strlen(caGroupids),",%d",lGroupid);
			     
			   		lGroupid=0;
			   		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid);
			   		
			    }
			     pasDbCloseCursor(psCur); 
	     }
	   		
		}
		
		
					
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
   	    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0404内存分配"),ncLang("0404内存分配"));
        return 0;
   }
    lGitem=0;
  	if(atol(caGitem)==101){                        //按部门统计网站访问次数
  		lGitem=4;
  		  sprintf(sqlbuf,"select a.groupid,sum(b.bytes)/1024 abyte,sum(b.lcount) acount,b.host from nchttpcount b,\
        	ncuser a where b.userid=a.userid  ");
      	if(strcmp(caSdate,caEdate)!=0)
            sprintf(sqlbuf+strlen(sqlbuf)," and ((b.sdate > '%s' and b.sdate < '%s') or (b.sdate='%s' and b.stime>='%s' ) or (b.sdate='%s' and b.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
        else
        sprintf(sqlbuf+strlen(sqlbuf)," and (b.sdate='%s' and b.stime>='%s' and b.stime<='%s' ) ",caSdate,caShour,caEhour);
       
        if(lGroupid0!=0){
        	 sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
      
      	}
      	
       if(strlen(caGroupid)!=0){
        	if(strcmp(caGroupid,"0")==0){
        		sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=0) ");
        	}
        	else{
        		sprintf(sqlbuf+strlen(sqlbuf)," and a.groupid in (%s) ",caGroupids);

        	}
     
      }
      	
      	
 //       if(strlen(caGroupid)!=0){
        	

        	
//        	if(atol(caGroupid)==0){
//        		 sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=0) ");
//        	}
//        	else
 //       	  sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
      
 //     	}
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by a.groupid  ");
  		  
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
  		  
  	}else if(atol(caGitem)==100){                                                      //按用户统计网站访问次数
  		
  		  lGitem=5;
  		  sprintf(sqlbuf,"select a.userid,sum(b.bytes)/1024 abyte,sum(b.lcount) acount,b.host from nchttpcount b,\
        	ncuser a where b.userid=a.userid ");
        if(strcmp(caSdate,caEdate)!=0)
            sprintf(sqlbuf+strlen(sqlbuf)," and ((b.sdate > '%s' and b.sdate < '%s') or (b.sdate='%s' and b.stime>='%s' ) or (b.sdate='%s' and b.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
        else
        		sprintf(sqlbuf+strlen(sqlbuf)," and (b.sdate='%s' and b.stime>='%s' and b.stime<='%s' ) ",caSdate,caShour,caEhour);
        
        if(lGroupid0!=0){
        	 sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
        
      	}
      	if(strlen(caGroupid)!=0){
        	if(strcmp(caGroupid,"0")==0){
        		sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=0) ");
        	}
        	else{
        		sprintf(sqlbuf+strlen(sqlbuf)," and a.groupid in (%s) ",caGroupids);

        	}
     
        }
//        if(strlen(caGroupid)!=0){
//        	if(atol(caGroupid)==0){
//        		 sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=0) ");
//        	}
//        	else
//        	  sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
      
//      	}
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by a.userid  ");
  		  
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
  		
  	}else if(atol(caGitem)==102){                                                      //按网址统计网站访问次数
  			lGitem=5;
  		  sprintf(sqlbuf,"select a.userid,sum(b.bytes)/1024 abyte,sum(b.lcount) acount,b.host,c.name from nchttpcount b,\
        	ncuser a,ncwebclass c where b.userid=a.userid and b.tid=c.id ");
        if(strcmp(caSdate,caEdate)!=0)
            sprintf(sqlbuf+strlen(sqlbuf)," and ((b.sdate > '%s' and b.sdate < '%s') or (b.sdate='%s' and b.stime>='%s' ) or (b.sdate='%s' and b.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
        else
        		sprintf(sqlbuf+strlen(sqlbuf)," and (b.sdate='%s' and b.stime>='%s' and b.stime<='%s' ) ",caSdate,caShour,caEhour);
        
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by b.host  ");
  		  
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
  	}else{																																									//按网址类别统计网站访问次数
  		  lGitem=4;
  		  sprintf(sqlbuf,"select a.groupid,sum(b.bytes)/1024 abyte,sum(b.lcount) acount,b.host,c.name,c.id from nchttpcount b,\
        	ncuser a,ncwebclass c where b.userid=a.userid and b.tid=c.id  ");
      	if(strcmp(caSdate,caEdate)!=0)
            sprintf(sqlbuf+strlen(sqlbuf)," and ((b.sdate > '%s' and b.sdate < '%s') or (b.sdate='%s' and b.stime>='%s' ) or (b.sdate='%s' and b.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
        else
        sprintf(sqlbuf+strlen(sqlbuf)," and (b.sdate='%s' and b.stime>='%s' and b.stime<='%s' ) ",caSdate,caShour,caEhour);
       
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by c.name  ");
  		  
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
  	}
  	printf("== * ===sqlbuf=%s\n",sqlbuf);
  	 utPltPutVar(psDbHead,"order",caOrder);
  	
    psCur=pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
      
        iReturn = 0;
        lBytes=0;
        lGroupid=0;
        lConntime=0;
        l_Count=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4, &lGroupid,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,30,caName,
                                       UT_TYPE_LONG,4, &Webid);                               
	     iNum=0;
	     lSumbytes=0;
	     lSumcount=0;
	     while(iReturn==0||iReturn==1405){
	     	pFlow[iNum].lId=lGroupid;
	     	pFlow[iNum].lBytes=lBytes;
	     	pFlow[iNum].l_Count=l_Count;
	     	pFlow[iNum].Webid=Webid;
	     	strcpy(pFlow[iNum].caHost,caHost);
	     	strcpy(pFlow[iNum].caName,caName);
	     	lSumbytes=lSumbytes+lBytes;
	     	lSumcount=lSumcount+l_Count;
	     	iNum++;
	     	lBytes=0;
        lGroupid=0;
        lConntime=0;
        l_Count=0;
        Webid=0;
        memset(caHost,0,sizeof(caHost));
        memset(caName,0,sizeof(caName));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4, &lGroupid,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&l_Count,
                                       UT_TYPE_STRING,63,caHost,
                                       UT_TYPE_STRING,30,caName,
                                       UT_TYPE_LONG,4, &Webid);       
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
       
if(atol(caGitem)==101){                 //统计部门 
	fprintf(fp,ncLang("1899序号,部门,流量,次数\n"));
	for(i=0;i<iNum;i++){
		 fprintf(fp,"%d,\"%s\",%0.1f,%llu\n",i+1,ncUtlGetGroupNameByid(psShmHead,pFlow[i].lId,ncLang("0629未知")),(float)pFlow[i].lBytes/1024,pFlow[i].l_Count);
               
	}
}else if(atol(caGitem)==100){                                     //统计用户
	 fprintf(fp,ncLang("1900序号,部门,用户名,流量,次数\n"));
	 for(i=0;i<iNum;i++){
	 		psUser = ncUtlGetUserContById(psShmHead,pFlow[i].lId);
         	if(psUser){
         	 	strcpy(caUsername,psUser->dispname);
         	 	if(psUser->psGroup){
         	 		strcpy(caGroupname,psUser->psGroup->groupname);
         	 	}else{
         	 		strcpy(caGroupname,ncLang("0629未知"));
         	 	}
         	}else{
         		strcpy(caUsername,ncLang("0629未知"));
         		strcpy(caIp,"");
         		strcpy(caGroupname,ncLang("0629未知"));
         	}
         	 fprintf(fp,"%d,\"%s\",\"%s\",%0.1f,%llu\n",i+1,caGroupname,caUsername,(float)pFlow[i].lBytes/1024,pFlow[i].l_Count);
       
	}
}else if(atol(caGitem)==102){
	 fprintf(fp,ncLang("1901序号,网址类别,网址,流量,次数\n"));
	 for(i=0;i<iNum;i++){	 		
         	 fprintf(fp,"%d,\"%s\",\"%s\",%0.1f,%llu\n",i+1,pFlow[i].caName,pFlow[i].caHost,(float)pFlow[i].lBytes/1024,pFlow[i].l_Count);
 	 }
}else{
	 fprintf(fp,ncLang("1902序号,网址类别,流量,次数\n"));
	 for(i=0;i<iNum;i++){	 		
         	 fprintf(fp,"%d,\"%s\",%0.1f,%llu\n",i+1,pFlow[i].caName,(float)pFlow[i].lBytes/1024,pFlow[i].l_Count);
 	 }
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
		  if(atol(caGitem)==101){																		//部门
		   		strcpy(desc[0],ncLang("0147部门"));		  
		 
		    	if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        		strcpy(desc[1],ncLang("1903访问次数(次)"));
		   	 		sprintf(desc[2],"%s(%s)",ncLang("1103总流量"),"MB");
		   			strcpy(desc[3],ncLang("1894流量百分比(%)"));
		 			}else{																																								//按次数排序     
						strcpy(desc[1],ncLang("1891上网流量(MB)"));
		   	 		sprintf(desc[2],"%s(%s)",ncLang("1904网站访问次数"),ncLang("1898次"));
		   			strcpy(desc[3],ncLang("1905次数百分比(%)"));
		 			}
		  }else if(atol(caGitem)==100){																										//用户
		  		strcpy(desc[0],ncLang("0728用户名"));
		  		strcpy(desc[1],ncLang("0149部门"));
		  		
		    	if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        		strcpy(desc[2],ncLang("1903访问次数(次)"));        		
		   	 		sprintf(desc[3],"%s(%s)",ncLang("1103总流量"),"MB");
		   			strcpy(desc[4],ncLang("1894流量百分比(%)"));
		 			}else{																																							//按次数排序     
						strcpy(desc[2],ncLang("1891上网流量(MB)"));
		   	 		sprintf(desc[3],"%s(%s)","网站访问次数",ncLang("1898次"));
		   			strcpy(desc[4],ncLang("1905次数百分比(%)"));
		 			}   
		  }else if(atol(caGitem)==102){		
		  		strcpy(desc[0],ncLang("0618网址"));
		  		strcpy(desc[1],ncLang("0624网址类别"));	
		  		
		  		if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        		strcpy(desc[2],ncLang("1903访问次数(次)"));        		
		   	 		sprintf(desc[3],"%s(%s)",ncLang("1103总流量"),"MB");
		   			strcpy(desc[4],ncLang("1894流量百分比(%)"));
		 			}else{																																							//按次数排序     
						strcpy(desc[2],ncLang("1891上网流量(MB)"));
		   	 		sprintf(desc[3],"%s(%s)","网站访问次数",ncLang("1898次"));
		   			strcpy(desc[4],ncLang("1905次数百分比(%)"));
		 			}	
		  }else{
		  		//strcpy(desc[0],"网址");
		  		strcpy(desc[0],ncLang("0624网址类别"));			  		
					
					if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        		strcpy(desc[1],ncLang("1903访问次数(次)"));        		
		   	 		sprintf(desc[2],"%s(%s)",ncLang("1103总流量"),"MB");
		   			strcpy(desc[3],ncLang("1894流量百分比(%)"));
		 			}else{																																							//按次数排序     
						strcpy(desc[1],ncLang("1891上网流量(MB)"));
		   	 		sprintf(desc[2],"%s(%s)","网站访问次数",ncLang("1898次"));
		   			strcpy(desc[3],ncLang("1905次数百分比(%)"));
		 			}
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
		  	if(atol(caGitem)==101){
  					pChart[i].name=strdup(ncUtlGetGroupNameByid(psShmHead,pFlow[i].lId,ncLang("0629未知")));  
  					if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     ---对应上门汉字顺序，-次数-流量-流量百分比   
        			sprintf(pChart[i].append[0],"%llu",pFlow[i].l_Count);
		 				}else{	   					  																																//按次数排序     			
        			sprintf(pChart[i].append[0],"%0.1f",(float)pFlow[i].lBytes/1024);				
		 				}
  			}else if(atol(caGitem)==100){ 																															//用户
  				  psUser = ncUtlGetUserContById(psShmHead,pFlow[i].lId);
         	  if(psUser){        	  
  				    pChart[i].name=strdup(psUser->dispname);  						  				
  				    if(psUser->psGroup){
  				      strcpy(pChart[i].append[0],psUser->psGroup->groupname);
  				    }else{ 						  				
  				      strcpy(pChart[i].append[0],ncLang("0629未知"));
  				    }if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        				sprintf(pChart[i].append[1],"%llu",pFlow[i].l_Count);
		 					}else{																																				//按次数排序     
        				sprintf(pChart[i].append[1],"%0.1f",(float)pFlow[i].lBytes/1024);								
		 					}	
  				  }else{
  				  	pChart[i].name=strdup(ncLang("0629未知"));
  				  	strcpy(pChart[i].append[0],ncLang("0629未知"));
  				  	strcpy(pChart[i].append[1],ncLang("0629未知"));
  				  	if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        				sprintf(pChart[i].append[2],"%llu",pFlow[i].l_Count);
		 					}else{																																				//按次数排序     
        				sprintf(pChart[i].append[2],"%0.1f",(float)pFlow[i].lBytes/1024);								
		 					}	
  				  }  			  
  			}else if(atol(caGitem)==102){		
  				  pChart[i].name=strdup(pFlow[i].caHost);																									//网址																								  					
  					//strcpy(pChart[i].name,pFlow[i].caHost);
  				  strcpy(pChart[i].append[0],pFlow[i].caName);
  				  if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        				sprintf(pChart[i].append[1],"%llu",pFlow[i].l_Count);
		 				}else{																																				//按次数排序     
        				sprintf(pChart[i].append[1],"%0.1f",(float)pFlow[i].lBytes/1024);							
		 				}		 				
  			}else{		
  					pChart[i].name=strdup(pFlow[i].caHost);			
  				  //strcpy(pChart[i].append[0],pFlow[i].caName);
  				  if(strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        				sprintf(pChart[i].append[0],"%llu",pFlow[i].l_Count);
		 				}else{																																				//按次数排序     
        				sprintf(pChart[i].append[0],"%0.1f",(float)pFlow[i].lBytes/1024);	
        		}	
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
		 			}else{																																								//按次数排序     							
							pChart[i].amount=(float)pFlow[i].l_Count;
          		if(lSumcount==0)
          	  	pChart[i].percent=strdup("0.00");
          		else{
          			sprintf(caTemp,"%0.2f",(float)100*pFlow[i].l_Count/lSumcount);
          			pChart[i].percent=strdup(caTemp);
          		}
		 			}			 			     
        }       	
	     	rowrow=iNum;    
        sprintf(filename,"%s.PDF","flowsort"); 	   
     if  (strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        	strcpy(caTitle,ncLang("1906网站访问流量排行榜"));
		 }else{																																								//按次数排序     
					strcpy(caTitle,ncLang("1907网站访问次数排行榜"));
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
         if(atol(caGitem)==101){                 //统计部门               
              utPltPutLoopVar(psDbHead,"name",  i+1,ncUtlGetGroupNameByid(psShmHead,pFlow[i].lId,ncLang("0629未知")));
              utPltPutLoopVar(psDbHead,"dept",  i+1,ncUtlGetGroupNameByid(psShmHead,pFlow[i].lId,ncLang("0629未知")));
         }else if(atol(caGitem)==100){
         		psUser = ncUtlGetUserContById(psShmHead,pFlow[i].lId);
         		if(psUser){
         	   	utPltPutLoopVar(psDbHead,"name",  i+1,psUser->dispname);
         	   	utPltPutLoopVarF(psDbHead,"ip",i+1,"%llu",pFlow[i].l_Count);
         	   	if(psUser->psGroup) {
         	   		utPltPutLoopVar(psDbHead,"dept",i+1,psUser->psGroup->groupname);
         	   	}         	   
         		}else
         	   	utPltPutLoopVar(psDbHead,"name",  i+1,ncLang("0629未知"));         	    
         }else if(atol(caGitem)==102){
         		utPltPutLoopVar(psDbHead,"name",  i+1,pFlow[i].caHost);
         }else{
         		utPltPutLoopVar(psDbHead,"name",  i+1,pFlow[i].caName);	
         }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if  (strcmp(caOrder,ncLang("0369流量"))==0 || caOrder[0]=='F') {								//按流量排序     
        		utPltPutLoopVarF(psDbHead,"data",i+1,"%0.0f",((float)pFlow[i].lBytes/1024));
				}else{																																								//按次数排序     
						 utPltPutLoopVarF(psDbHead,"data",i+1,"%llu",pFlow[i].l_Count);
				}       
         utPltPutLoopVarF(psDbHead,"num",i+1,"%d",i);
         utPltPutLoopVarF(psDbHead,"num1",i+1,"%02d",i+1);
         utPltPutLoopVarF(psDbHead,"uid",i+1,"%d",pFlow[i].lId);        
         utPltPutLoopVarF(psDbHead,"dbytes",i+1,"%0.1f",(float)pFlow[i].lBytes/1024);
         utPltPutLoopVarF(psDbHead,"ip",i+1,"%llu",pFlow[i].l_Count);
         utPltPutLoopVar(psDbHead,"webtype",i+1,pFlow[i].caName); 
         utPltPutLoopVar(psDbHead,"webname",i+1,pFlow[i].caHost); 
         utPltPutLoopVarF(psDbHead,"webid",i+1,"%lu",pFlow[i].Webid);
         
                
      }
     utPltPutVar(psDbHead,"unit",caUnit);    
     free(pFlow);
   if(atol(caGitem)==102)
  		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_top/ncwebsorttop_75_2.htm"); 
   else if(atol(caGitem)==103) 
   		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_top/ncwebsorttop_75_3.htm"); 
   else
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_top/ncwebsorttop_75.htm"); 
   return 0;
}
//部门--用户上网明细
int ncWebShowToplog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
 	  char webid2[32];
 //	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,10,"TotPg",UT_TYPE_STRING, 12, caTotPg,
																				  "CurPg",UT_TYPE_STRING, 12, caCurPg,
																				  "uid",  UT_TYPE_STRING, 12, caUserid,
																				  "sdate",UT_TYPE_STRING,19,caSdate,
																				  "edate",UT_TYPE_STRING,19,caEdate,
																				  "shour",UT_TYPE_STRING,2,caShour,
																				  "ehour",UT_TYPE_STRING,2,caEhour,
																				  "groupid",UT_TYPE_STRING,15,caGroupid,
																				  "git",  UT_TYPE_STRING,8,caGit,
   	                                      "exp",  UT_TYPE_STRING,10,caExport);
 
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
  		sprintf(caCond+strlen(caCond)," and b.userid=%d ",atol(caUserid));
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


    sprintf(caTemp,"select count(*) from ncuser b,nchttpcount c left join ncwebclass a on(c.tid=a.id) where  b.userid=c.userid "); 
 
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
    
  sprintf(caTemp,"select b.userid,c.sdate,c.stime,b.ip,b.mac,c.bytes/1024,c.host,a.name,c.lcount from ncuser b,nchttpcount c left join ncwebclass a on(c.tid=a.id) where  b.userid=c.userid  ");    
  
  if(strcmp(caSdate,caEdate)!=0)
    sprintf(caTemp+strlen(caTemp)," and ((c.sdate > '%s' and c.sdate < '%s') or (c.sdate='%s' and c.stime>='%s' ) or (c.sdate='%s' and c.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  else
    sprintf(caTemp+strlen(caTemp)," and (c.sdate='%s' and c.stime>='%s' and c.stime<='%s' ) ",caSdate,caShour,caEhour);
       
//  if(lGroupid0!=0){
//    sprintf(caTemp+strlen(caTemp)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
// 	}
  	if(strlen(caGroupid)!=0){
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
        
        fprintf(fp,ncLang("1101用户上网日志\n"));
      ncUtlPrintCsv(fp,9,UT_TYPE_STRING,ncLang("0147部门"),
                         UT_TYPE_STRING,ncLang("0728用户名"),
                         UT_TYPE_STRING,ncLang("0054IP地址"),
                         UT_TYPE_STRING,ncLang("0072MAC地址"),
                         UT_TYPE_STRING,ncLang("1100上网时间"),
                         UT_TYPE_STRING,ncLang("1909总流量KBytes"),
                         UT_TYPE_STRING,ncLang("0594网址"),
                         UT_TYPE_STRING,ncLang("0624网址类别"),
                         UT_TYPE_STRING,ncLang("0234访问次数"));

                                      
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;
       memset(lStime,0,sizeof(lStime));      
       memset(lEtime,0,sizeof(lEtime)); 
       lIp=0;
       lUbytes=0;
       de_cout=0;
       memset(caHost,0,sizeof(caHost));      
       memset(caName,0,sizeof(caName)); 
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 UT_TYPE_STRING,10,lStime,
   																 UT_TYPE_STRING,3,lEtime,
   																 UT_TYPE_ULONG,4,&lIp,
   																 UT_TYPE_STRING,19,caMac,
   																 UT_TYPE_ULONG, 4, &lUbytes,
   												 				 UT_TYPE_STRING,63,caHost,
                           				 UT_TYPE_STRING,30,caName,
                           				 UT_TYPE_ULONG, 4, &de_cout);
   																
   				       
  	                              
        while((iReturn == 0)||(iReturn==1405)) {
     
        psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
		     if(psUser){   
          strcpy(caUdip,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
          if(psUser->psGroup)
          strcpy(caDep,psUser->psGroup->groupname);
          
        }
        else{
        	strcpy(caUdip,"");
        	strcpy(caDep,"");
       
        }
        //strcpy(caTime,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
        strcat(lStime," ");
        strcat(lStime,lEtime);
        strcpy(caUbyte,utStrLtoF(lUbytes,caTemp));
        sprintf(de_Cout,"%d",de_cout);
        if(strlen(caName)==0)
        	 strcpy(caName,ncLang("1908未知类别"));
        ncUtlPrintCsv(fp,9,UT_TYPE_STRING,caDep,
                       		 UT_TYPE_STRING,caUdip,
                       		 UT_TYPE_STRING,utComHostIp(htonl(lIp)),
                       		 UT_TYPE_STRING,caMac,
                       		 UT_TYPE_STRING,lStime,
                       		 UT_TYPE_STRING,caUbyte,
   												 UT_TYPE_STRING,caHost,
                           UT_TYPE_STRING,caName,
                           UT_TYPE_STRING,de_Cout); 
                                          
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;
       memset(lStime,0,sizeof(lStime));      
       memset(lEtime,0,sizeof(lEtime)); 
       lIp=0;
       lUbytes=0;
       de_cout=0;
       memset(caHost,0,sizeof(caHost));      
       memset(caName,0,sizeof(caName)); 
       memset(de_Cout,0,sizeof(de_Cout));
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 	 UT_TYPE_STRING,10,lStime,
   																 	 UT_TYPE_STRING,3,lEtime,
   																 	 UT_TYPE_ULONG,4,&lIp,
   																 	 UT_TYPE_STRING,19,caMac,
   																 	 UT_TYPE_ULONG, 4, &lUbytes,
   												 				 	 UT_TYPE_STRING,63,caHost,
                           				 	 UT_TYPE_STRING,30,caName,
                           				 	 UT_TYPE_ULONG, 4, &de_cout);
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
       memset(lStime,0,sizeof(lStime));      
        memset(lEtime,0,sizeof(lEtime)); 
       lIp=0;
       lUbytes=0;
       memset(caHost,0,sizeof(caHost));      
       memset(caName,0,sizeof(caName)); 
     	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 	 UT_TYPE_STRING,10,lStime,
   																 	 UT_TYPE_STRING,3,lEtime,
   																 	 UT_TYPE_ULONG,4,&lIp,
   																 	 UT_TYPE_STRING,19,caMac,
   																 	 UT_TYPE_ULONG, 4, &lUbytes,
   												 				 	 UT_TYPE_STRING,63,caHost,
                           				 	 UT_TYPE_STRING,30,caName,
                           				 	 UT_TYPE_ULONG, 4, &de_cout);
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
        }
        else{
        	strcpy(caUdip,"");
        	strcpy(caDep,"");
        	strcpy(caIp,"");
        }
        
        utPltPutLoopVar(psDbHead,"dept",   iNum,caDep);
        utPltPutLoopVar(psDbHead,"username",iNum,caUdip);
     
//        sprintf(caTime,"%s %s",caSdate,caStime);
        strcpy(caUbyte,utStrLtoF(lUbytes,caTemp));
        
         

        utPltPutLoopVar(psDbHead,"ip",    iNum,utComHostIp(htonl(lIp)));
        //utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
        strcat(lStime," ");
        strcat(lStime,lEtime);
        utPltPutLoopVar(psDbHead,"sdate", iNum,lStime);
        //utPltPutLoopVarF(psDbHead,"ubytes", iNum,"%d",lUbytes);
        utPltPutLoopVar(psDbHead,"ubytes",iNum,caUbyte);
        utPltPutLoopVar(psDbHead,"mac",iNum,caMac);
        utPltPutLoopVar(psDbHead,"webname",iNum,caHost);
        utPltPutLoopVarF(psDbHead,"de_cout",iNum,"%d",de_cout);
        if(strlen(caName)==0)
        	 strcpy(caName,ncLang("1908未知类别"));
        utPltPutLoopVar(psDbHead,"webtype",iNum,caName);        
        memset(caMac,0,sizeof(caMac));      
        memset(caDep,0,sizeof(caDep));   
        lUserid=0;
        memset(lStime,0,sizeof(lStime));      
        memset(lEtime,0,sizeof(lEtime)); 
        lIp=0;
        lUbytes=0;
        de_cout=0;
        memset(caHost,0,sizeof(caHost));      
        memset(caName,0,sizeof(caName));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 	  UT_TYPE_STRING,10,lStime,
   																 	  UT_TYPE_STRING,3,lEtime,
   																 	  UT_TYPE_ULONG,4,&lIp,
   																 	  UT_TYPE_STRING,19,caMac,
   																 	  UT_TYPE_ULONG, 4, &lUbytes,
   												 				 	  UT_TYPE_STRING,63,caHost,
                           				 	  UT_TYPE_STRING,30,caName,
                           				 	  UT_TYPE_ULONG, 4, &de_cout);
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


     utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_top/nc_sorttoploglist.htm");
    return 0;
}
//网址--网址类别上网明细
int ncWebShowWebsitelog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
 	  char caHost[64];
 	  char caName[32];
 	  char caHost2[64];
 	  char caName2[32]; 	 
 	  int group_flag,lGroupid0;
    long de_cout;
 	  char de_Cout[20];
 	  char webid2[32];
 //	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,13,"TotPg",UT_TYPE_STRING, 12, caTotPg,
																				  "CurPg",UT_TYPE_STRING, 12, caCurPg,
																				  "uid",  UT_TYPE_STRING, 12, caUserid,
																				  "sdate",UT_TYPE_STRING,19,caSdate,
																				  "edate",UT_TYPE_STRING,19,caEdate,
																				  "shour",UT_TYPE_STRING,2,caShour,
																				  "ehour",UT_TYPE_STRING,2,caEhour,
																				  "groupid",UT_TYPE_STRING,15,caGroupid,
																				  "git",  UT_TYPE_STRING,8,caGit,
																				  "webname",UT_TYPE_STRING,63,caHost,
																				  "webtype",  UT_TYPE_STRING,30,caName,
   	                                      "exp",  UT_TYPE_STRING,10,caExport,
   	                                      "webid",  UT_TYPE_STRING,10,webid2);
   printf("== caSdate=%s\n",caSdate);	  
   printf("== caEdate=%s\n",caEdate);
   printf("== caName=%s\n",caName);
   printf("== caUserid=%s\n",caUserid);
   printf("== lGroupid0=%d\n",lGroupid0);
  
   strcpy(caHost2,caHost);
   strcpy(caName2,caName);
   
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
 
   if(atol(caGit)==102){
   		sprintf(caCond+strlen(caCond)," and c.host='%s'",caHost);
   }else{
  		sprintf(caCond+strlen(caCond)," and a.id='%s' ",webid2);
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
    sprintf(caTemp,"select count(*) from nchttpcount c,ncuser b,ncwebclass a where  b.userid=c.userid and c.tid=a.id");    
 
 		if(strcmp(caSdate,caEdate)!=0)
     	sprintf(caTemp+strlen(caTemp)," and ((c.sdate > '%s' and c.sdate < '%s') or (c.sdate='%s' and c.stime>='%s' ) or (c.sdate='%s' and c.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	else
     	sprintf(caTemp+strlen(caTemp)," and (c.sdate='%s' and c.stime>='%s' and c.stime<='%s' ) ",caSdate,caShour,caEhour);
  
  	if(strlen(caGroupid)!=0){
      sprintf(caTemp+strlen(caTemp)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
  	}
   if(strlen(caCond)!=0)
  		 strcat(caTemp,caCond);
		strcat(caTemp,"  order by c.sdate desc ");
	 printf("=1= caTemp=%s\n",caTemp);
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);

    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0152部门查询"),ncLang("0171查询数据出错"));
        return 0;
    }  
    
  sprintf(caTemp,"select b.userid,c.sdate,c.stime,b.ip,b.mac,c.bytes/1024,c.host,a.name,c.lcount from ncuser b,nchttpcount c,ncwebclass a where  b.userid=c.userid and c.tid=a.id ");    
  
  if(strcmp(caSdate,caEdate)!=0)
    sprintf(caTemp+strlen(caTemp)," and ((c.sdate > '%s' and c.sdate < '%s') or (c.sdate='%s' and c.stime>='%s' ) or (c.sdate='%s' and c.stime<='%s' ))",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  else
    sprintf(caTemp+strlen(caTemp)," and (c.sdate='%s' and c.stime>='%s' and c.stime<='%s' ) ",caSdate,caShour,caEhour);
  
  	if(strlen(caGroupid)!=0){
      sprintf(caTemp+strlen(caTemp)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
  	}
  
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
      ncUtlPrintCsv(fp,9,UT_TYPE_STRING,ncLang("0147部门"),
                         UT_TYPE_STRING,ncLang("0728用户名"),
                         UT_TYPE_STRING,ncLang("0054IP地址"),
                         UT_TYPE_STRING,ncLang("0072MAC地址"),
                         UT_TYPE_STRING,ncLang("1100上网时间"),
                         UT_TYPE_STRING,ncLang("1103总流量KBytes"),
                         UT_TYPE_STRING,ncLang("0594网址"),
                         UT_TYPE_STRING,ncLang("0624网址类别"),
                         UT_TYPE_STRING,ncLang("0234访问次数"));
                    
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;
       memset(lStime,0,sizeof(lStime));      
       memset(lEtime,0,sizeof(lEtime)); 
       lIp=0;
       lUbytes=0;
       de_cout=0;
       memset(caHost,0,sizeof(caHost));      
       memset(caName,0,sizeof(caName));  
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 UT_TYPE_STRING,10,lStime,
   																 UT_TYPE_STRING,3,lEtime,
   																 UT_TYPE_ULONG,4,&lIp,
   																 UT_TYPE_STRING,19,caMac,
   																 UT_TYPE_ULONG, 4, &lUbytes,
   															 	 UT_TYPE_STRING,63,caHost,
                                   UT_TYPE_STRING,30,caName,
                           				 UT_TYPE_ULONG, 4, &de_cout); 
   		                           
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
        
        	strcat(lStime," ");
        	strcat(lStime,lEtime);
        	strcpy(caUbyte,utStrLtoF(lUbytes,caTemp));
					sprintf(de_Cout,"%d",de_cout);
        	ncUtlPrintCsv(fp,9,UT_TYPE_STRING,caDep,
                       		 	 UT_TYPE_STRING,caUdip,
                       		 	 UT_TYPE_STRING,utComHostIp(htonl(lIp)),
                       		 	 UT_TYPE_STRING,caMac,
                       		   UT_TYPE_STRING,lStime,
                       		 	 UT_TYPE_STRING,caUbyte,
   												 	 UT_TYPE_STRING,caHost,
                           	 UT_TYPE_STRING,caName,
                           	 UT_TYPE_STRING,de_Cout); 
                              
       		memset(caMac,0,sizeof(caMac));      
       		memset(caDep,0,sizeof(caDep));   
       		lUserid=0;
       		memset(lStime,0,sizeof(lStime));      
       		memset(lEtime,0,sizeof(lEtime)); 
       		lIp=0;
       		lUbytes=0;
       		de_cout=0;
       		memset(de_Cout,0,sizeof(de_Cout));  
       		memset(caHost,0,sizeof(caHost));      
       		memset(caName,0,sizeof(caName)); 
   				iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 			 UT_TYPE_STRING,10,lStime,
   																 			 UT_TYPE_STRING,3,lEtime,
   																 			 UT_TYPE_ULONG,4,&lIp,
   																 			 UT_TYPE_STRING,19,caMac,
   																 			 UT_TYPE_ULONG, 4, &lUbytes,
   												 				 			 UT_TYPE_STRING,63,caHost,
                           				 			 UT_TYPE_STRING,30,caName,
                           				 			 UT_TYPE_ULONG, 4, &de_cout);
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
       memset(lStime,0,sizeof(lStime));      
       memset(lEtime,0,sizeof(lEtime)); 
       lIp=0;
       lUbytes=0;
       memset(caHost,0,sizeof(caHost));      
       memset(caName,0,sizeof(caName));   
     	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 	 UT_TYPE_STRING,10,lStime,
   																 	 UT_TYPE_STRING,3,lEtime,
   																 	 UT_TYPE_ULONG,4,&lIp,
   																 	 UT_TYPE_STRING,19,caMac,
   																 	 UT_TYPE_ULONG, 4, &lUbytes,
   																 	 UT_TYPE_STRING,63,caHost,
                                     UT_TYPE_STRING,30,caName,
                           				 	 UT_TYPE_ULONG, 4, &de_cout);
   	iNum = 0;
   	
   	while(iReturn == 0||iReturn==1405 && iNum < lRowNum) {
        iNum++;
   
        utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%d",lUserid);
         psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
		     if(psUser){   
          strcpy(caUdip,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
          if(psUser->psGroup)
             strcpy(caDep,psUser->psGroup->groupname);
           else
             strcpy(caDep,"");
          strcpy(caIp,utComHostIp(htonl(psUser->ip)));
          utPltPutLoopVar(psDbHead,"position",iNum,psUser->position);
        }
        else{
        	strcpy(caUdip,"");
        	strcpy(caDep,"");
        	strcpy(caIp,"");
        }
        
        utPltPutLoopVar(psDbHead,"dept",   iNum,caDep);
        utPltPutLoopVar(psDbHead,"username",iNum,caUdip);
     
//        sprintf(caTime,"%s %s",caSdate,caStime);
        strcpy(caUbyte,utStrLtoF(lUbytes,caTemp));
        utPltPutLoopVar(psDbHead,"ip",    iNum,utComHostIp(htonl(lIp)));
        strcat(lStime," ");
        strcat(lStime,lEtime);
        utPltPutLoopVar(psDbHead,"sdate", iNum,lStime);
        //utPltPutLoopVarF(psDbHead,"ubytes", iNum,"%d",lUbytes);
        utPltPutLoopVar(psDbHead,"ubytes",iNum,caUbyte);
        utPltPutLoopVar(psDbHead,"mac",iNum,caMac);
        utPltPutLoopVar(psDbHead,"webname",iNum,caHost);
        utPltPutLoopVar(psDbHead,"webtype",iNum,caName);
        utPltPutLoopVarF(psDbHead,"de_cout",iNum,"%d",de_cout);
        memset(caMac,0,sizeof(caMac));      
        memset(caDep,0,sizeof(caDep));   
        lUserid=0;
        memset(lStime,0,sizeof(lStime));      
        memset(lEtime,0,sizeof(lEtime)); 
        lIp=0;
        lUbytes=0;
        de_cout=0;
        memset(caHost,0,sizeof(caHost));      
        memset(caName,0,sizeof(caName)); 
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 	 UT_TYPE_STRING,10,lStime,
   																 	 UT_TYPE_STRING,3,lEtime,
   																 	 UT_TYPE_ULONG,4,&lIp,
   																 	 UT_TYPE_STRING,19,caMac,
   																 	 UT_TYPE_ULONG, 4, &lUbytes,
   																 	 UT_TYPE_STRING,63,caHost,
                                     UT_TYPE_STRING,30,caName,
                           				 	 UT_TYPE_ULONG, 4, &de_cout); 
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

    //utPltPutVar(psDbHead,"title_name",caTitlename);
    utPltPutVar(psDbHead,"webname",caHost2);
    utPltPutVar(psDbHead,"webtype",caName2);
    utPltPutVar(psDbHead,"webid",webid2);
    if(atol(caGit)==102)
    	utPltPutVar(psDbHead,"title_name",caHost2);
    else
    	utPltPutVar(psDbHead,"title_name",caName2);

     utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_top/nc_sortwebsiteloglist.htm");
    return 0;
}

