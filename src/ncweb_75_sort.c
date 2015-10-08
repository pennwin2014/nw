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
/* 流量最多  */

/*



int ncWebSortUserByFlow_75(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
        unsigned long lTime;
    };
    struct ncFlowSort_s *pFlow;
     ncUserCont *psUser;
    long long lSumbytes;
    long lSumtime;
    long lGitem;
    char caName[32];
    long lGroupid;
    long m;
//    char preflag[8],nextflag[8],ctime[12];
    char caServ_flag[12];
    
    char caPdf[32];
//    long lRowrow;
    struct piechart_datasetf *pChart;           //用于PDF文档输出

    int rowrow;                     
    int column; 
//    char **pTemp;
    char caTitle[128];
    char filename[128];
//    int m;
    column=4;
//    long lDate;
    char **desc;
    char caStatdate[128];
    char caUsername[32],caIp[20],caGroupname[32];
   lTime=time(0);
   lSum=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
   
   
      iReturn = utMsgGetSomeNVar(psMsgHead,10,
					"order",   UT_TYPE_STRING, 12, caOrder,
					"exppdf",   UT_TYPE_STRING,8,caPdf,
					"exp",  UT_TYPE_STRING, 6,  caExport,
					"sunrec",UT_TYPE_STRING,8, caSum,
					"sdate",UT_TYPE_STRING,11,caSdate,
					"edate",UT_TYPE_STRING,11,caEdate,
					"shour",UT_TYPE_STRING,2,caShour,
					"ehour",UT_TYPE_STRING,2,caEhour,
					"groupid",UT_TYPE_STRING,20,caGroupid,
					"gitem", UT_TYPE_STRING,8,caGitem);
					
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
    lGitem=0;
  	if(atol(caGitem)==101){                        //按部门统计上网流量
  		lGitem=4;
  		  sprintf(sqlbuf,"select a.groupid,sum(b.sumflow + b.upflow) abyte,sum(conntime) atime from ncuserlog b,\
        	ncuser a where b.userid=a.userid  and conntime<1000000 and b.sumflow + b.upflow>1 ");
        sprintf(sqlbuf+strlen(sqlbuf)," and b.etime>=%lu and b.stime<=%lu ",lStime,lEtime);
        
        if(lGroupid0!=0){
        	 sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
//        
      }
        if(strlen(caGroupid)!=0){
        	  sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
 //      
      }
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by a.groupid  ");
  		 if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T') 
        {
        
         strcpy(caOrder,ncLang("0521时间"));
         sprintf(sqlbuf+strlen(sqlbuf)," order by atime desc ");
         
         strcpy(caUnit,ncLang("1698小时"));
        }
        else
        {
        	  strcpy(caUnit,"MB");
            strcpy(caOrder,ncLang("0369流量"));
            sprintf(sqlbuf+strlen(sqlbuf)," order by abyte desc ");
        }
        sprintf(sqlbuf+strlen(sqlbuf)," limit 0,%lu ",lSumShow);
  		  
  	}
  	else{                                                      //按用户统计
  		
  		  lGitem=6;
  		  sprintf(sqlbuf,"select a.userid,sum(b.sumflow + b.upflow) abyte,sum(conntime) atime from ncuserlog b,\
        	ncuser a where b.userid=a.userid  and conntime<1000000 and b.sumflow + b.upflow>1 ");
        sprintf(sqlbuf+strlen(sqlbuf)," and b.etime>=%lu and b.stime<=%lu ",lStime,lEtime);
        
        if(lGroupid0!=0){
        	 sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
//        
      }
        if(strlen(caGroupid)!=0){
        	  sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
 //      
      }
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by a.userid  ");
  		 if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T') 
        {
        
         strcpy(caOrder,ncLang("0521时间"));
         sprintf(sqlbuf+strlen(sqlbuf)," order by atime desc ");
         
         strcpy(caUnit,ncLang("1698小时"));
        }
        else
        {
        	  strcpy(caUnit,"MB");
            strcpy(caOrder,ncLang("0369流量"));
            sprintf(sqlbuf+strlen(sqlbuf)," order by abyte desc ");
        }
        sprintf(sqlbuf+strlen(sqlbuf)," limit 0,%lu ",lSumShow);
  		
  	}
  	printf("sqlbuf=%s\n",sqlbuf);
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
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                       UT_TYPE_LONG8,8,&lBytes,                                      
                                       UT_TYPE_LONG,4,&lConntime);                               
	     iNum=0;
	     lSumbytes=0;
	     lSumtime=0;
	     while(iReturn==0||iReturn==1405){
	     	pFlow[iNum].lId=lGroupid;
	     	pFlow[iNum].lBytes=lBytes;
	     	pFlow[iNum].lTime=lConntime;
	     	lSumbytes=lSumbytes+lBytes;
	     	lSumtime=lSumtime+lConntime;
	     	iNum++;
	     	lBytes=0;
        lGroupid=0;
        lConntime=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                       UT_TYPE_LONG8,8,&lBytes,                                      
                                       UT_TYPE_LONG,4,&lConntime);  
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
        fprintf(fp,ncLang("0002%-2s网络访问%s排行榜(%s--%s)\n"),"\0",caOrder,caSdate_temp,caEdate_temp);
        
if(atol(caGitem)==101){                 //统计部门 
	fprintf(fp,ncLang("1890序号,部门,流量,时间\n"));
	for(i=0;i<iNum;i++){
		 fprintf(fp,"%d,\"%s\",%0.1f,\"%s\"\n",i+1,ncUtlGetGroupNameByid(psShmHead,pFlow[i].lId,ncLang("0629未知")),(float)pFlow[i].lBytes/1024,ncCvtConntime(pFlow[i].lTime));
       
	}
}
else{                                     //统计用户
	 fprintf(fp,ncLang("1002序号,部门,用户名,流量,时间\n"));
	 for(i=0;i<iNum;i++){
	 		psUser = ncUtlGetUserContById(psShmHead,pFlow[i].lId);
         	 if(psUser){
         	 	strcpy(caUsername,psUser->dispname);
         	 	strcpy(caIp,utComHostIp(htonl(psUser->ip)));
         	 	if(psUser->psGroup){
         	 		strcpy(caGroupname,psUser->psGroup->groupname);
         	 	}
         	 	else{
         	 		strcpy(caGroupname,ncLang("0629未知"));
         	 	}
         	}
         	else{
         		strcpy(caUsername,ncLang("0629未知"));
         		strcpy(caIp,"");
         		strcpy(caGroupname,ncLang("0629未知"));
         	}
         	 fprintf(fp,"%d,\"%s\",\"%s(%s)\",%0.1f,\"%s\"\n",i+1,caGroupname,caUsername,caIp,(float)pFlow[i].lBytes/1024,ncCvtConntime(pFlow[i].lTime));
       
	}
}
	   
       
       
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
         free(pFlow);
        utPltFreeDb(psDbHead);
        
		    return 0;
	}
	
	     
	     if(strlen(caPdf)>0){
	     	//输出PDF文档
	     		//表格标签
    	                    
			 desc=malloc(lGitem*sizeof(char*));
			 for(i=0;i<lGitem;i++){
			 	desc[i]=malloc(32);
		   }
		   if(atol(caGitem)==101){
		   strcpy(desc[0],ncLang("0147部门"));		  
		  
		  
		   if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T'){
		   	 strcpy(desc[1],ncLang("1891上网流量(MB)"));
		    sprintf(desc[2],"%s(%s)",ncLang("1100上网时间"),ncLang("1698小时"));
		   	strcpy(desc[3],ncLang("1892时间百分比(%)"));
		   }
		   else{
		   	 strcpy(desc[1],ncLang("1699上网时间(小时)"));
		   	 sprintf(desc[2],"%s(%s)",ncLang("1103总流量"),"MB");
		   		strcpy(desc[3],ncLang("1894流量百分比(%)"));
		    }
		  }
		  else{
		  	strcpy(desc[0],ncLang("0762用户名"));
		  	strcpy(desc[1],ncLang("0147部门"));
		  	strcpy(desc[2],ncLang("1887IP地址"));
		   
		    
		    if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T'){
		    	strcpy(desc[3],ncLang("1891上网流量(MB)"));
		    	 sprintf(desc[4],"%s(%s)",ncLang("1100上网时间"),ncLang("1698小时"));
		   	    strcpy(desc[5],ncLang("1892时间百分比(%)"));
		    }
		    else{
		    	strcpy(desc[3],ncLang("1699上网时间(小时)"));
		    	 sprintf(desc[4],"%s(%s)",ncLang("1103总流量"),"MB");
		   		strcpy(desc[5],ncLang("1894流量百分比(%)"));
		    }
		  
		    
		  }
	     	 //输出到PDF中     
		   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*iNum);
		    if(pChart==NULL){
		  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0405内存分配出错"));
		      return 0;
		   }
		  for(i=0;i<iNum;i++){
		  	  	//输出表格的补充字段
  				  pChart[i].append=malloc((lGitem-3)*sizeof(char*));
  				  for(m=0;m<lGitem-3;m++){
  				  	pChart[i].append[m]=malloc(32);
  				  }
  				 
		  	if(atol(caGitem)==101){
  				pChart[i].name=strdup(ncUtlGetGroupNameByid(psShmHead,pFlow[i].lId,ncLang("0629未知")));
  				
  				if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T'){
  					sprintf(pChart[i].append[0],"%0.1f",(float)pFlow[i].lBytes/1024);
  				}
  				else{
  				   sprintf(pChart[i].append[0],"%0.1f",(float)pFlow[i].lTime/3600);
  				  }
  			}
  			else{


  				  
  				  psUser = ncUtlGetUserContById(psShmHead,pFlow[i].lId);
         	  if(psUser){        	  
  				    pChart[i].name=strdup(psUser->dispname);
  				    strcpy(pChart[i].append[1],utComHostIp(htonl(psUser->ip)));
  				    if(psUser->psGroup)
  				      strcpy(pChart[i].append[0],psUser->psGroup->groupname);
  				    else
  				      strcpy(pChart[i].append[0],ncLang("0629未知"));
  				  }
  				  else{
  				  	pChart[i].name=strdup(ncLang("0629未知"));
  				  	strcpy(pChart[i].append[0],ncLang("0629未知"));
  				  	strcpy(pChart[i].append[1],ncLang("0629未知"));
  				  }
  				 if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T'){
  					sprintf(pChart[i].append[2],"%0.1f",(float)pFlow[i].lBytes/1024);
  				}
  				else{
  				   sprintf(pChart[i].append[2],"%d",pFlow[i].lTime/3600);
  				  }
  				
  //	printf("aa=%s\n",	pChart[i].append[2]);		  
  				  
  			}
  				if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T')                //按时间排序
          {
          	pChart[i].amount=(float)pFlow[i].lTime/3600;
          	if(lSumtime==0)
          	  pChart[i].percent=strdup("0.00");
          	else{
          		sprintf(caTemp,"%0.2f",(float)100*pFlow[i].lTime/lSumtime);
          		pChart[i].percent=strdup(caTemp);
          	}
          	  
          }
          else
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
	if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T')                //按时间排序
	{
		strcpy(caTitle,ncLang("1895上网时间排行榜"));
	}
	else{
      strcpy(caTitle,ncLang("1896上网流量排行榜"));
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
         }
         else{
         	psUser = ncUtlGetUserContById(psShmHead,pFlow[i].lId);
         	 if(psUser){
         	   utPltPutLoopVar(psDbHead,"name",  i+1,psUser->dispname);
         	   utPltPutLoopVar(psDbHead,"ip",i+1,utComHostIp(htonl(psUser->ip)));
         	   if(psUser->psGroup) {
         	   	utPltPutLoopVar(psDbHead,"dept",i+1,psUser->psGroup->groupname);
         	   }
         	   utPltPutLoopVarF(psDbHead,"time",i+1,"%0.1f",(float)pFlow[i].lTime/3600);
         	}
         	else
         	   utPltPutLoopVar(psDbHead,"name",  i+1,ncLang("0629未知"));
         	   

         	   
         }
        if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T')                //按时间排序
        {
            utPltPutLoopVarF(psDbHead,"data",i+1,"%0.1f",(float)pFlow[i].lTime/3600); 
         }
         else{
         	utPltPutLoopVarF(psDbHead,"data",i+1,"%0.0f",((float)pFlow[i].lBytes/1024)); 
        }
         utPltPutLoopVarF(psDbHead,"num",i+1,"%d",i);
        utPltPutLoopVarF(psDbHead,"num1",i+1,"%02d",i+1);
        utPltPutLoopVarF(psDbHead,"uid",i+1,"%d",pFlow[i].lId);
        
         utPltPutLoopVarF(psDbHead,"dbytes",i+1,"%0.1f",(float)pFlow[i].lBytes/1024);
         utPltPutLoopVarF(psDbHead,"time",i+1,"%0.1f",(float)pFlow[i].lTime/3600);
        
        
        
        
      }
     utPltPutVar(psDbHead,"unit",caUnit);
         
      
    
     free(pFlow);
   
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/ncwebsortflow_75.htm");  
    return 0;
}

*/


/* 流量最多  */
int ncWebSortUserByFlow_75(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  pasDbCursor *psCur;
    char sqlbuf[2048];
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
        unsigned long lTime;
    };
    struct ncFlowSort_s *pFlow;
     ncUserCont *psUser;
    long long lSumbytes;
    long lSumtime;
    long lGitem;
    char caName[32];
    long lGroupid;
    long m;
//    char preflag[8],nextflag[8],ctime[12];
    char caServ_flag[12];
    
    char caPdf[32];
//    long lRowrow;
    struct piechart_datasetf *pChart;           //用于PDF文档输出

    int rowrow;                     
    int column; 
//    char **pTemp;
    char caTitle[128];
    char filename[128];
//    int m;
    column=4;
//    long lDate;
    char **desc;
    char caStatdate[128];
    char caUsername[32],caIp[20],caGroupname[32];
    char caGroupids[4048];
    long groupids[100];
    char caGroupid2[20];
   lTime=time(0);
   lSum=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
   
   
      iReturn = utMsgGetSomeNVar(psMsgHead,10,
					"order",   UT_TYPE_STRING, 12, caOrder,
					"exppdf",   UT_TYPE_STRING,8,caPdf,
					"exp",  UT_TYPE_STRING, 6,  caExport,
					"sunrec",UT_TYPE_STRING,8, caSum,
					"sdate",UT_TYPE_STRING,11,caSdate,
					"edate",UT_TYPE_STRING,11,caEdate,
					"shour",UT_TYPE_STRING,2,caShour,
					"ehour",UT_TYPE_STRING,2,caEhour,
					"groupid",UT_TYPE_STRING,20,caGroupid,
					"gitem", UT_TYPE_STRING,8,caGitem);

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
   	    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0405内存分配出错"),ncLang("0405内存分配出错"));
        return 0;
   }
    lGitem=0;
  	if(atol(caGitem)==101){                        //按部门统计上网流量
  		lGitem=4;
 		   	
 		   sprintf(sqlbuf,"select a.groupid,sum(b.sumflow + b.upflow) abyte,sum(conntime) atime from ncuserlog b,\
        	ncuser a where b.userid=a.userid and b.stime>=%lu and b.stime<=%lu and conntime<1000000 and b.sumflow + b.upflow>1 ",lStime,lEtime);
 		   	
 //      sprintf(sqlbuf,"select a.groupid,sum(case when stime<=%d and etime>=%d and etime-stime>0 then (%d-%d)/(etime-stime)*(b.sumflow + b.upflow)  when stime<=%d and etime<%d and etime-stime>0 then (etime-%d)*(b.sumflow + b.upflow)/(etime-stime)  \
 //      when stime>=%d and etime>%d and etime-stime>0 then (%d-stime)*(b.sumflow+b.upflow)/(etime-stime) else b.sumflow + b.upflow End) abyte,\
 //      sum(case when stime<=%d and etime>=%d and etime-stime>0 then (%d-%d)*conntime/(etime-stime)  when stime<=%d and etime<%d and etime-stime>0 then (etime-%d)*conntime/(etime-stime)  \
//       when stime>=%d and etime>%d and etime-stime>0 then (%d-stime)*conntime/(etime-stime) else conntime End) atime from ncuserlog b,\
//        	ncuser a where b.userid=a.userid  and conntime<1000000 and b.sumflow + b.upflow>1 ",lStime,lEtime,lEtime,lStime,lStime,lEtime,lStime,lStime,lEtime,lEtime\
 //       	,lStime,lEtime,lEtime,lStime,lStime,lEtime,lStime,lStime,lEtime,lEtime);
        	
//        sprintf(sqlbuf+strlen(sqlbuf)," and b.stime>=%lu and b.stime<=%lu ",lStime,lEtime);
        
        if(lGroupid0!=0){
        	 sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
//        
      }
        if(strlen(caGroupid)!=0){
        	if(strcmp(caGroupid,"0")==0){
        		sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=0) ");
        	}
        	else{
        		sprintf(sqlbuf+strlen(sqlbuf)," and a.groupid in (%s) ",caGroupids);
   //     	  sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
        	}
 //      
      }
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by a.groupid  ");
  		 if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T') 
        {
        
         strcpy(caOrder,ncLang("0521时间"));
         sprintf(sqlbuf+strlen(sqlbuf)," order by atime desc ");
         
         strcpy(caUnit,ncLang("1698小时"));
        }
        else
        {
        	  strcpy(caUnit,"MB");
            strcpy(caOrder,ncLang("0369流量"));
            sprintf(sqlbuf+strlen(sqlbuf)," order by abyte desc ");
        }
        sprintf(sqlbuf+strlen(sqlbuf)," limit 0,%lu ",lSumShow);
  		  
  	}
  	else{                                                      //按用户统计
  		
  		  lGitem=6;
//  		  sprintf(sqlbuf,"select a.userid,sum(b.sumflow + b.upflow) abyte,sum(conntime) atime from ncuserlog b,\
//        	ncuser a where b.userid=a.userid  and conntime<1000000 and b.sumflow + b.upflow>1 ");
//        sprintf(sqlbuf+strlen(sqlbuf)," and b.etime>=%lu and b.stime<=%lu ",lStime,lEtime);
    
    
         sprintf(sqlbuf,"select a.userid,sum(b.sumflow + b.upflow) abyte,sum(conntime) atime from ncuserlog b,\
        	ncuser a where b.userid=a.userid and b.stime>=%lu and b.stime<=%lu and conntime<1000000 and b.sumflow + b.upflow>1 ",\
        	lStime,lEtime);
   //     sprintf(sqlbuf+strlen(sqlbuf)," and b.stime>=%lu and b.stime<=%lu ",lStime,lEtime);
  
    
 //      sprintf(sqlbuf,"select a.userid,sum(case when stime<=%d and etime>=%d and etime-stime>0 then (%d-%d)/(etime-stime)*(b.sumflow + b.upflow)  when stime<=%d and etime<%d and etime-stime>0 then (etime-%d)*(b.sumflow + b.upflow)/(etime-stime)  \
 //      when stime>=%d and etime>%d and etime-stime>0 then (%d-stime)*(b.sumflow+b.upflow)/(etime-stime) else b.sumflow + b.upflow End) abyte,\
 //      sum(case when stime<=%d and etime>=%d and etime-stime>0 then (%d-%d)*conntime/(etime-stime)  when stime<=%d and etime<%d and etime-stime>0 then (etime-%d)*conntime/(etime-stime)  \
//       when stime>=%d and etime>%d and etime-stime>0 then (%d-stime)*conntime/(etime-stime) else conntime End) atime from ncuserlog b,\
 //       	ncuser a where b.userid=a.userid  and conntime<1000000 and b.sumflow + b.upflow>1 ",lStime,lEtime,lEtime,lStime,lStime,lEtime,lStime,lStime,lEtime,lEtime\
 //       	,lStime,lEtime,lEtime,lStime,lStime,lEtime,lStime,lStime,lEtime,lEtime);
 //       sprintf(sqlbuf+strlen(sqlbuf)," and b.stime>=%lu and b.stime<=%lu ",lStime,lEtime);
    
        
        if(lGroupid0!=0){
        	 sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",lGroupid0,lGroupid0);
//        
      }
        if(strlen(caGroupid)!=0){
        	if(strcmp(caGroupid,"0")==0){
        		sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=0 )");
        	}
        	else{
        		sprintf(sqlbuf+strlen(sqlbuf)," and a.groupid in (%s) ",caGroupids);
  //      	  sprintf(sqlbuf+strlen(sqlbuf)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));
        	}
 //      
      }
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by a.userid  ");
  		 if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T') 
        {
        
         strcpy(caOrder,ncLang("0521时间"));
         sprintf(sqlbuf+strlen(sqlbuf)," order by atime desc ");
         
         strcpy(caUnit,ncLang("1698小时"));
        }
        else
        {
        	  strcpy(caUnit,"MB");
            strcpy(caOrder,ncLang("0369流量"));
            sprintf(sqlbuf+strlen(sqlbuf)," order by abyte desc ");
        }
        sprintf(sqlbuf+strlen(sqlbuf)," limit 0,%lu ",lSumShow);
  		
  	}
  	printf("sqlbuf=%s\n",sqlbuf);
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
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                       UT_TYPE_LONG8,8,&lBytes,                                      
                                       UT_TYPE_LONG,4,&lConntime);                               
	     iNum=0;
	     lSumbytes=0;
	     lSumtime=0;
	     while(iReturn==0||iReturn==1405){
	     	pFlow[iNum].lId=lGroupid;
	     	pFlow[iNum].lBytes=lBytes;
	     	pFlow[iNum].lTime=lConntime;
	     	lSumbytes=lSumbytes+lBytes;
	     	lSumtime=lSumtime+lConntime;
	     	iNum++;
	     	lBytes=0;
        lGroupid=0;
        lConntime=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                       UT_TYPE_LONG8,8,&lBytes,                                      
                                       UT_TYPE_LONG,4,&lConntime);  
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
        fprintf(fp,ncLang("0002%-2s网络访问%s排行榜(%s--%s)\n"),"\0",caOrder,caSdate_temp,caEdate_temp);
        
if(atol(caGitem)==101){                 //统计部门 
	fprintf(fp,ncLang("1890序号,部门,流量,时间\n"));
	for(i=0;i<iNum;i++){
		 fprintf(fp,"%d,\"%s\",%0.1f,\"%s\"\n",i+1,ncUtlGetGroupNameByid(psShmHead,pFlow[i].lId,ncLang("0629未知")),(float)pFlow[i].lBytes/1024,ncCvtConntime(pFlow[i].lTime));
       
	}
}
else{                                     //统计用户
	 fprintf(fp,ncLang("1002序号,部门,用户名,流量,时间\n"));
	 for(i=0;i<iNum;i++){
	 		psUser = ncUtlGetUserContById(psShmHead,pFlow[i].lId);
         	 if(psUser){
         	 	strcpy(caUsername,psUser->dispname);
         	 	strcpy(caIp,utComHostIp(htonl(psUser->ip)));
         	 	if(psUser->psGroup){
         	 		strcpy(caGroupname,psUser->psGroup->groupname);
         	 	}
         	 	else{
         	 		strcpy(caGroupname,ncLang("0629未知"));
         	 	}
         	}
         	else{
         		strcpy(caUsername,ncLang("0629未知"));
         		strcpy(caIp,"");
         		strcpy(caGroupname,ncLang("0629未知"));
         	}
         	 fprintf(fp,"%d,\"%s\",\"%s(%s)\",%0.1f,\"%s\"\n",i+1,caGroupname,caUsername,caIp,(float)pFlow[i].lBytes/1024,ncCvtConntime(pFlow[i].lTime));
       
	}
}
	   
       
       
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
         free(pFlow);
        utPltFreeDb(psDbHead);
        
		    return 0;
	}
	
	     
	     if(strlen(caPdf)>0){
	     	//输出PDF文档
	     		//表格标签
    	                    
			 desc=malloc(lGitem*sizeof(char*));
			 for(i=0;i<lGitem;i++){
			 	desc[i]=malloc(32);
		   }
		   if(atol(caGitem)==101){
		   strcpy(desc[0],ncLang("0147部门"));		  
		  
		  
		   if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T'){
		   	 strcpy(desc[1],ncLang("1891上网流量(MB)"));
		    sprintf(desc[2],"%s(%s)",ncLang("1100上网时间"),ncLang("1698小时"));
		   	strcpy(desc[3],ncLang("1892时间百分比(%)"));
		   }
		   else{
		   	 strcpy(desc[1],ncLang("1699上网时间(小时)"));
		   	 sprintf(desc[2],"%s(%s)",ncLang("1103总流量"),"MB");
		   		strcpy(desc[3],ncLang("1894流量百分比(%)"));
		    }
		  }
		  else{
		  	strcpy(desc[0],ncLang("0762用户名"));
		  	strcpy(desc[1],ncLang("0147部门"));
		  	strcpy(desc[2],ncLang("1887IP地址"));
		   
		    
		    if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T'){
		    	strcpy(desc[3],ncLang("1891上网流量(MB)"));
		    	 sprintf(desc[4],"%s(%s)",ncLang("1100上网时间"),ncLang("1698小时"));
		   	    strcpy(desc[5],ncLang("1892时间百分比(%)"));
		    }
		    else{
		    	strcpy(desc[3],ncLang("1699上网时间(小时)"));
		    	 sprintf(desc[4],"%s(%s)",ncLang("1103总流量"),"MB");
		   		strcpy(desc[5],ncLang("1894流量百分比(%)"));
		    }
		  
		    
		  }
	     	 //输出到PDF中     
		   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*iNum);
		    if(pChart==NULL){
		  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0405内存分配出错"));
		      return 0;
		   }
		  for(i=0;i<iNum;i++){
		  	  	//输出表格的补充字段
  				  pChart[i].append=malloc((lGitem-3)*sizeof(char*));
  				  for(m=0;m<lGitem-3;m++){
  				  	pChart[i].append[m]=malloc(32);
  				  }
  				 
		  	if(atol(caGitem)==101){
  				pChart[i].name=strdup(ncUtlGetGroupNameByid(psShmHead,pFlow[i].lId,ncLang("0629未知")));
  				
  				if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T'){
  					sprintf(pChart[i].append[0],"%0.1f",(float)pFlow[i].lBytes/1024);
  				}
  				else{
  				   sprintf(pChart[i].append[0],"%0.1f",(float)pFlow[i].lTime/3600);
  				  }
  			}
  			else{


  				  
  				  psUser = ncUtlGetUserContById(psShmHead,pFlow[i].lId);
         	  if(psUser){        	  
  				    pChart[i].name=strdup(psUser->dispname);
  				    strcpy(pChart[i].append[1],utComHostIp(htonl(psUser->ip)));
  				    if(psUser->psGroup)
  				      strcpy(pChart[i].append[0],psUser->psGroup->groupname);
  				    else
  				      strcpy(pChart[i].append[0],ncLang("0629未知"));
  				  }
  				  else{
  				  	pChart[i].name=strdup(ncLang("0629未知"));
  				  	strcpy(pChart[i].append[0],ncLang("0629未知"));
  				  	strcpy(pChart[i].append[1],ncLang("0629未知"));
  				  }
  				 if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T'){
  					sprintf(pChart[i].append[2],"%0.1f",(float)pFlow[i].lBytes/1024);
  				}
  				else{
  				   sprintf(pChart[i].append[2],"%d",pFlow[i].lTime/3600);
  				  }
  				
  //	printf("aa=%s\n",	pChart[i].append[2]);		  
  				  
  			}
  				if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T')                //按时间排序
          {
          	pChart[i].amount=(float)pFlow[i].lTime/3600;
          	if(lSumtime==0)
          	  pChart[i].percent=strdup("0.00");
          	else{
          		sprintf(caTemp,"%0.2f",(float)100*pFlow[i].lTime/lSumtime);
          		pChart[i].percent=strdup(caTemp);
          	}
          	  
          }
          else
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
	if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T')                //按时间排序
	{
		strcpy(caTitle,ncLang("1895上网时间排行榜"));
	}
	else{
      strcpy(caTitle,ncLang("1896上网流量排行榜"));
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
         }
         else{
         	psUser = ncUtlGetUserContById(psShmHead,pFlow[i].lId);
         	 if(psUser){
         	   utPltPutLoopVar(psDbHead,"name",  i+1,psUser->dispname);
         	   utPltPutLoopVar(psDbHead,"ip",i+1,utComHostIp(htonl(psUser->ip)));
         	   if(psUser->psGroup) {
         	   	utPltPutLoopVar(psDbHead,"dept",i+1,psUser->psGroup->groupname);
         	   }
         	   utPltPutLoopVarF(psDbHead,"time",i+1,"%0.1f",(float)pFlow[i].lTime/3600);
         	}
         	else
         	   utPltPutLoopVar(psDbHead,"name",  i+1,ncLang("0629未知"));
         	   

         	   
         }
        if(strcmp(caOrder,ncLang("0521时间"))==0 || caOrder[0] == 'T')                //按时间排序
        {
            utPltPutLoopVarF(psDbHead,"data",i+1,"%0.1f",(float)pFlow[i].lTime/3600); 
         }
         else{
         	utPltPutLoopVarF(psDbHead,"data",i+1,"%0.0f",((float)pFlow[i].lBytes/1024)); 
        }
         utPltPutLoopVarF(psDbHead,"num",i+1,"%d",i);
        utPltPutLoopVarF(psDbHead,"num1",i+1,"%02d",i+1);
        utPltPutLoopVarF(psDbHead,"uid",i+1,"%d",pFlow[i].lId);
        
         utPltPutLoopVarF(psDbHead,"dbytes",i+1,"%0.1f",(float)pFlow[i].lBytes/1024);
         utPltPutLoopVarF(psDbHead,"time",i+1,"%0.1f",(float)pFlow[i].lTime/3600);
        
        
        
        
      }
     utPltPutVar(psDbHead,"unit",caUnit);
         
      
    
     free(pFlow);
   
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/ncwebsortflow_75.htm");  
    return 0;
}





//用户上网明细
int ncWebShowUserlog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    unsigned long lStime;
    unsigned long lEtime;
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
 	
 	 
 //	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,9,
	    "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			"CurPg",      UT_TYPE_STRING, 12, caCurPg,
			"uid",    UT_TYPE_STRING, 12, caUserid,
			"sdate",UT_TYPE_STRING,19,caSdate,
			"edate",UT_TYPE_STRING,19,caEdate,
			"shour",UT_TYPE_STRING,2,caShour,
			"ehour",UT_TYPE_STRING,2,caEhour,
			"git",UT_TYPE_STRING,8,caGit,
   	  "exp",     UT_TYPE_STRING,10,caExport);
   	  
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
   
		sprintf(caCond+strlen(caCond)," and a.etime>=%lu ",utTimStrToLong("%Y/%m/%d %H:%M",caSdate_temp));
		sprintf(caCond+strlen(caCond)," and a.stime<=%lu ",utTimStrToLong("%Y/%m/%d %H:%M",caEdate_temp));
   
   if(atol(caGit)==101){
   	sprintf(caCond+strlen(caCond)," and (b.groupid=%d or b.groupid in (select groupid from ncgroup where pid=%d ))",atol(caUserid),atol(caUserid));
  } 
  else{
  	sprintf(caCond+strlen(caCond)," and a.userid=%d ",atol(caUserid));
  }
    
	
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;

 

    if(!utStrIsSpaces(caExport)){
    	lStartRec=0;
    lRowNum = 99999;
    }
    else{
    lRowNum=32;
    
    lStartRec = (lCurPg - 1) * lRowNum;    
    }

    sprintf(caTemp,"select count(*) from ncuserlog a,ncuser b where a.userid=b.userid  ");    
 
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
   #ifdef LDEBUG
	  printf("caTemp=%s\n",caTemp);
	#endif 
    iReturn = pasDbOneRecord(caTemp,0,
                            UT_TYPE_LONG,4,&lCount);

    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0152部门查询"),ncLang("0171查询数据出错"));
        return 0;
    }  
    
      sprintf(caTemp,"select a.userid,a.stime,a.conntime,a.ip,a.mac,a.sumflow+a.upflow from ncuserlog a,ncuser b where a.userid=b.userid  ");    
 
   
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
   

strcat(caTemp," order by a.stime desc ");


    sprintf(caTemp+strlen(caTemp)," limit %lu,%lu ",lStartRec,lRowNum);
printf("caTemp=%s\n",caTemp);
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
                       UT_TYPE_STRING,ncLang("1919连接时间(秒)"),
                       UT_TYPE_STRING,ncLang("1103总流量KBytes"));

                                      
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;
       lStime=0;
       lEtime=0;
       lIp=0;
       lUbytes=0;
        
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 UT_TYPE_ULONG,4,&lStime,
   																 UT_TYPE_ULONG,4,&lEtime,
   																 UT_TYPE_ULONG,4,&lIp,
   																 UT_TYPE_STRING,19,caMac,
   																 UT_TYPE_ULONG, 4, &lUbytes);
   																
   				       
  	                              
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
        strcpy(caTime,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
        strcpy(caUbyte,utStrLtoF(lUbytes,caTemp));
      
        ncUtlPrintCsv(fp,7,UT_TYPE_STRING,caDep,
                       UT_TYPE_STRING,caUdip,
                       UT_TYPE_STRING,utComHostIp(htonl(lIp)),
                       UT_TYPE_STRING,caMac,
                       UT_TYPE_STRING,caTime,
                       UT_TYPE_LONG,lEtime,
                       UT_TYPE_STRING,caUbyte);
                                          
       memset(caMac,0,sizeof(caMac));      
       memset(caDep,0,sizeof(caDep));   
       lUserid=0;
       lStime=0;
       lEtime=0;
       lIp=0;
       lUbytes=0;
        
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 UT_TYPE_ULONG,4,&lStime,
   																 UT_TYPE_ULONG,4,&lEtime,
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
       lStime=0;
       lEtime=0;
       lIp=0;
       lUbytes=0;
        
     	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 UT_TYPE_ULONG,4,&lStime,
   																 UT_TYPE_ULONG,4,&lEtime,
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
        utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
        utPltPutLoopVarF(psDbHead,"edate", iNum,"%d",lEtime);
        utPltPutLoopVar(psDbHead,"ubytes",iNum,caUbyte);
        utPltPutLoopVar(psDbHead,"mac",iNum,caMac);
        memset(caMac,0,sizeof(caMac));      
        memset(caDep,0,sizeof(caDep));   
        lUserid=0;
        lStime=0;
        lEtime=0;
        lIp=0;
        lUbytes=0;
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
   																 UT_TYPE_ULONG,4,&lStime,
   																 UT_TYPE_ULONG,4,&lEtime,
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


     utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/nc_sortuserloglist.htm");
    return 0;
}



/* 显示选择网址类别树  */
int ncWebTreeDispWebClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
   	int iReturn,i;
    char caName[32];
    long lId,iNum;
#ifdef LDEBUG1 
    utMsgPrintMsg(psMsgHead);
#endif  	
  
   
 
    psCur = pasDbOpenSqlF("select name,id from ncwebclass where 1=1  order by groupid ");
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    
    memset(caName,0,sizeof(caName));
    lId=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,30,caName,
                                   UT_TYPE_LONG,4,&lId);
                                   
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        
       
        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum-1);
        utPltPutLoopVar(psDbHead,"name", iNum,caName);
        utPltPutLoopVarF(psDbHead,"id",   iNum,"%ld",lId);
       
    memset(caName,0,sizeof(caName));
    lId=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,30,caName,
                                   UT_TYPE_LONG,4,&lId);
    }
    pasDbCloseCursor(psCur);
   
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/tree_webclass_select.htm");
    return 0;
}




 /*显示网络服务流量统计条件界面*/   
int ncFlowTimeDispForm_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char *p;
    unsigned long lTime;
    char sqlbuf[1024];
    char service_name[17]="";
    utPltDbHead *psDbHead;
    char caSdate1[32],caSdate2[32],caGroupid[16];
    char caStime[12],caEtime[12];
    char caSort[16],caRecnum[16],caTime[32],caWay[6],caOpt[6];
    char caService[1024]="";
    char *pBuf,*pSelect;
    int lSumCol=0;
    int i,m;
    int iNum=0;
    int iNum1=0;
    int m1=0;
    int i1=0;
    int ck_flag=0;
    char caVar[17]="";
    long iSumCol;
    char ck_id[1024],name[32];
    long id;
    char t_id[16];
    char caSdate[20],caEdate[20];
    pasDbCursor *psCur;
   
    
    iReturn = utMsgGetSomeNVar(psMsgHead,7,
					"sdate1",    UT_TYPE_STRING, 31,  caSdate1,
					"sdate2",    UT_TYPE_STRING, 31,  caSdate2,
					"stime",     UT_TYPE_STRING, 11,  caStime,
					"eitme",   UT_TYPE_STRING, 11,  caEtime,
					"groupid",  UT_TYPE_STRING, 11,  caGroupid,
					"way",      UT_TYPE_STRING, 5,   caWay,
					"opt",      UT_TYPE_STRING, 5,   caOpt);
					
					
		
    /* 显示部门  */
   
    psDbHead = utPltInitDb();
    lTime = time(0);
    ncUtlMonthDate(lTime,caSdate,caEdate);
    pBuf=pasUtlCvtSqlSelectTreeBmA(ncLang("0455全部,;"));
//    pBuf = pasUtlCvtSqlSelectA("select groupname,groupid from ncgroup order by groupid",
//               ncLang("0455全部,;"));
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
        free(pBuf);
        utPltPutVar(psDbHead,"sgroup", pSelect);
        free(pSelect);
    }
    if(!utStrIsSpaces(caGroupid)) {
        utPltPutVar(psDbHead,"groupid", caGroupid);
    }
    if(!utStrIsSpaces(caSdate1)) {
        utPltPutVar(psDbHead,"sdate1", caSdate1);
    }
    else {
        utPltPutVar(psDbHead,"sdate1", caSdate);
    }
    if(!utStrIsSpaces(caSdate2)) {
        utPltPutVar(psDbHead,"sdate2", caSdate2);
    }
    else {
        utPltPutVar(psDbHead,"sdate2", caEdate);
    }

    if(!utStrIsSpaces(caStime)) {
        utPltPutVar(psDbHead,"stime", caStime);
    }
    else {
        utPltPutVar(psDbHead,"stime", "00");
    }
    if(!utStrIsSpaces(caEtime)) {
        utPltPutVar(psDbHead,"etime", caEtime);
    }
    else {
        utPltPutVar(psDbHead,"etime", "23");
    }
    if(!utStrIsSpaces(caWay)) {
        utPltPutVar(psDbHead,"way", caWay);
    }
    else {
        utPltPutVar(psDbHead,"way", "1");
    }
   if(!utStrIsSpaces(caOpt)) {
        utPltPutVar(psDbHead,"opt", caOpt);
    }
    else {
        utPltPutVar(psDbHead,"opt", "1");
    }
    sprintf(sqlbuf,"select count(*) from ncservicecgroup ");
    pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&iSumCol);
    
    strcpy(ck_id,",");
    for(i=0;i<iSumCol;i++)
    {
      sprintf(caVar,"chk%d",i);
    	p=utMsgGetVar_s(psMsgHead,caVar);
    	if(p)
    	{
    		sprintf(ck_id+strlen(ck_id),"%s,",p);
    		
    	}
    }
    
    sprintf(sqlbuf,"select groupid,name from ncservicecgroup order by groupid");
    psCur=pasDbOpenSql(sqlbuf,0);
     if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0444取服务"),ncLang("0549数据库出错"));
            return 0;
        }
        id=0;
        memset(name,0,sizeof(name));
        iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,
        			     UT_TYPE_STRING,63,name);
        i=0;
        iNum=0;
        while((iReturn==0)||(iReturn==1405))
        {
        	
        	if(i%6==0)
    			{
    		 			iNum++;
    		 			m=0;
    	   }
		    	m++;	
		    	utPltPutLoopVar2(psDbHead,"service_name",iNum,m,name);
		    	utPltPutLoopVarF2(psDbHead,"i",iNum,m,"%d",i);
		      utPltPutLoopVarF2(psDbHead,"id",iNum,m,"%d",id);
		      
		      sprintf(t_id,",%d,",id);
		      if((strstr(ck_id,t_id))||(strcmp(ck_id,",")==0)){
		      	utPltPutLoopVar2(psDbHead,"check",iNum,m,"checked");
		      }
        	
        	i++;
        	
	        id=0;
	        memset(name,0,sizeof(name));
	        iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,
	        			     UT_TYPE_STRING,63,name);
        	
        }
         pasDbCloseCursor(psCur);
      
    
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/nc_flowtime_menunew.htm");
    return 0;
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

/* 上网流量统计 新版  */
int ncWebFlowCount_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char sqlbuf[2024];
    char *p,caVar[16];
    char caStime[3]="";
    char caEtime[3]="";
    char caMac[20];
    char caDispname[20];
    long width=0;
    int     i,iReturn,iNum,lRowNum,iSumRec,lSumPage;
    long lGroupid0;
    char caOpt[32],caWay[32],caOptlabel[16];
    char caSub1[32],caSub2[32],caSub3[32],caGroupid[31];
    char caExport[32];
    char caSdate[32],caSdate1[32],caSdate2[32],caSdate9[32],caUserid[32],caStart[32],caUsername[36],caTemp[30],caSip[32],caDip[32];
    unsigned long lUserid,lStart,lConntime,lService,lSip,lDip,lTime;
    unsigned long *lpTime;
    long lSumCol,lNum,lGroupid;
    long tempSumCol=0;
    long lserv[50];
    long qt_flag=0;
    long ser_flag=0;
    long max_count=0;
    long temp_i=0;
    long long *lpBytes;
    long long lUbytes,lDbytes;
    char caService[1024]="";
    char *pSelect;
    char caIp[20];
    long iNum_2,iNum_8,iNum_9,yx_flag,p2p_flag,gp_flag;
    utPltDbHead *psDbHead;
    struct ncSflow_s stGserver[32];
    long id;
    char name[32];
    memset(&stGserver,0,32*sizeof(struct ncSflow_s));
    
    lRowNum = 50;
   

    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
    utMsgPrintMsg(psMsgHead);
    memset(caUserid,0,sizeof(caUserid));
   	iReturn = utMsgGetSomeNVar(psMsgHead,12,
   	                "opt",   UT_TYPE_STRING, 8,  caOpt,   /* 选择分组对象 1-天 2-时 3-月 4-部门 5-用户 */
   	                "way",   UT_TYPE_STRING, 8,  caWay,   /* 计量单位  1-流量  2-时间    */
					"sdate1",  UT_TYPE_STRING, 31, caSdate1,
					"sdate2",  UT_TYPE_STRING, 31, caSdate2,
					"userid",  UT_TYPE_STRING, 31, caUserid,
					"groupid", UT_TYPE_STRING, 31, caGroupid,
					"CurPg",  UT_TYPE_STRING,31, caStart,
					"export", UT_TYPE_STRING,31,caExport,
					"stime",  UT_TYPE_STRING,2,caStime,
					"etime",  UT_TYPE_STRING,2,caEtime,
					"mac",    UT_TYPE_STRING,18,caMac,
					"dispname",UT_TYPE_STRING,18,caDispname);
   if(utStrIsSpaces(caStart)) strcpy(caStart,"1");
  
  
  
  
  
  
    sprintf(sqlbuf,"select count(*) from ncservicecgroup ");
    pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&tempSumCol);
    
   
//    tempSumCol = ncPatGetSumService();
    lSumCol=0;
    memset(caService,0,sizeof(caService));
    for(i=0;i<tempSumCol;i++) {                              //获取选中的网络服务类别
        sprintf(caVar,"chk%d",i);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p)
        {       	
        	lSumCol++;
 //       	lserv[lSumCol-1]=atoi(p);
        	if(lSumCol==1)
        	   sprintf(caService,"%s",p);
        	 else
        	   sprintf(caService+strlen(caService),",%s",p);

        }
    }
    
    
    sprintf(sqlbuf,"select groupid,name from ncservicecgroup where 1=1 ");
    if(strlen(caService)>0){
    	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
    }
    sprintf(sqlbuf+strlen(sqlbuf)," order by groupid ");
    
   
    
    psCur=pasDbOpenSql(sqlbuf,0);
     if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0444取服务"),ncLang("0549数据库出错"));
            return 0;
        }
        id=0;
        memset(name,0,sizeof(name));
        iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,
        			     UT_TYPE_STRING,63,name);
        
       lSumCol=0;
        while((iReturn==0)||(iReturn==1405))
        {
        	stGserver[lSumCol].lService=id;
        	strcpy(stGserver[lSumCol].caGname,name);
        	
		      lSumCol++;        	
	        id=0;
	        memset(name,0,sizeof(name));
	        iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,
	        			     UT_TYPE_STRING,63,name);
        	
        }
        pasDbCloseCursor(psCur);
 
 //按lService排序   
    	qsort(stGserver,lSumCol,sizeof(struct ncSflow_s),ncUtlSortOnServiceId2); 
    
    
    
    
    
    width=90+lSumCol*100;
    if(width<890) width=850;




    if( utStrIsSpaces(caSdate1)) {
        ncCvtStime(time(0),caSdate1,caTemp);
        strcpy(caSdate1 +8,"01");
    }
    if( utStrIsSpaces(caSdate2)) {
        ncCvtStime(time(0),caSdate2,caTemp);
    }
    lUserid = atol(caUserid);
    
  if(strlen(caMac)!=0)
  {
  	lUserid=0;
  	sprintf(sqlbuf,"select userid from ncuser where mac='%s' ",caMac);
  	pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
   } 
   if(strlen(caDispname)!=0)
  {
  	lUserid=0;
  	sprintf(sqlbuf,"select userid from ncuser where dispname='%s' or username='%s' ",caDispname,caDispname);
  	pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
   }    
    
    
    /* 取用户名  */
    if(lUserid != 0) {
        iReturn = pasDbOneRecord("select dispname from ncuser where userid = :uid ",
                        1,"uid", UT_TYPE_LONG, lUserid,
                        UT_TYPE_STRING,30,caUsername);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0445权限检查"),ncLang("0743用户不存在 %d"),iReturn);
            return 0;
        }
    }
    else if(!utStrIsSpaces(caGroupid)) {
            lGroupid = atol(caGroupid);
            if(lGroupid0 == 0) {
                lGroupid0 = lGroupid;
            }
            else {
                if(lGroupid0 != lGroupid) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0445权限检查"),ncLang("0658无权访问"));
                    return 0;
                }
            }
            iReturn = pasDbOneRecord("select groupname from ncgroup where groupid = :uid ",
                        1,"uid", UT_TYPE_LONG, lGroupid0,
                        UT_TYPE_STRING,30,caUsername);
            if(iReturn != 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0445权限检查"),ncLang("0743用户不存在 %d"),iReturn);
                    return 0;
            }
    }   
    else {
        strcpy(caUsername,"\0");
    }
             
    if(caOpt[0] == '3') { /* 按月统计  */
        strcpy(caOptlabel,ncLang("0109按月分组"));
        if(lUserid == 0) {
            if((utStrIsSpaces(caGroupid))&&(lGroupid0==0)) {
            	if(strcmp(caSdate1,caSdate2)!=0){
            		   sprintf(sqlbuf,"select aa.adate,'',b.groupid,sum(aa.actime),sum(aa.aubytes),sum(aa.adbytes) from (");
            		   sprintf(sqlbuf+strlen(sqlbuf),"select substring(sdate,1,7) adate,'',sum(ctime) actime,sum(ubytes) aubytes,sum(dbytes) adbytes,service from ncipservice where (sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' )  and ctime<1000000 ", caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
            		   sprintf(sqlbuf+strlen(sqlbuf)," group by service,substring(sdate,1,7)) aa,ncipindex b where aa.service=b.sid ");
            		    if(strlen(caService)>0){
                    	sprintf(sqlbuf+strlen(sqlbuf)," and b.groupid in (%s) ",caService);
                    }
            		   sprintf(sqlbuf+strlen(sqlbuf)," group by aa.adate,b.groupid order by aa.adate ");
            		   
            		
            		
           // 	     sprintf(sqlbuf,"select substring(sdate,1,7),'',groupid,sum(ctime),sum(ubytes),sum(dbytes) \
          //              from ncipservice,ncipindex  where (sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' ) and service=sid and ctime<1000000 ", caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
           //         if(strlen(caService)>0){
           //         	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
           //         }
           //         sprintf(sqlbuf+strlen(sqlbuf)," group by substring(sdate,1,7),groupid order by substring(sdate,1,7)  ");
                       
               }
               else{
               	  sprintf(sqlbuf,"select substring(sdate,1,7),'',groupid,sum(ctime),sum(ubytes),sum(dbytes) \
                        from ncipservice,ncipindex where (sdate='%s' and stime>='%s' and stime<='%s' ) and service=sid and ctime<1000000 " ,caSdate1,caStime,caEtime); 
                  if(strlen(caService)>0){
                    	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
                    }     
                        
                  sprintf(sqlbuf+strlen(sqlbuf)," group by substring(sdate,1,7),groupid  order by substring(sdate,1,7)  ");
                        
                } 
             printf("sqlbuf=%s\n",sqlbuf);       
                psCur = pasDbOpenSql(sqlbuf,0);
             }
             else {
             	if(strcmp(caSdate1,caSdate2)!=0){
             		sprintf(sqlbuf,"select substring(a.sdate,1,7),'',c.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes) \
                        from ncipservice a,ncuser b,ncipindex c where a.userid = b.userid and a.service=c.sid and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu and level=1 )) and ((a.sdate > '%s' \
                         and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' )) and a.ctime<1000000 " ,lGroupid0,lGroupid0,caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
                if(strlen(caService)>0){
                	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
                }
                sprintf(sqlbuf+strlen(sqlbuf)," group by substring(a.sdate,1,7),c.groupid order by substring(a.sdate,1,7)  ");

                      }
                      else{
                      	 sprintf(sqlbuf,"select substring(a.sdate,1,7),'',c.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes) \
                        from ncipservice a,ncuser b,ncipindex c where a.userid = b.userid and a.service=c.sid and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu and level=1)) and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s') and a.ctime<1000000 ",lGroupid0,lGroupid0,caSdate1,caStime,caEtime);
                        if(strlen(caService)>0){
                        	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
                        }
                        sprintf(sqlbuf+strlen(sqlbuf)," group by substring(a.sdate,1,7),c.groupid order by substring(a.sdate,1,7)  ");
                        
                      }
    //                 printf("sqlbuf=%s\n",sqlbuf);   
                    psCur = pasDbOpenSql(sqlbuf,0);
            }
        }
        else {
        	if(strcmp(caSdate1,caSdate2)!=0){
        		sprintf(sqlbuf,"select substring(sdate,1,7),'',groupid,sum(ctime),sum(ubytes),sum(dbytes) \
                     from ncipservice,ncipindex  \
                     where userid = %d and ((sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' )) and service=sid and ctime<1000000 ",lUserid,caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
             if(strlen(caService)>0){
             	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
            }
            sprintf(sqlbuf+strlen(sqlbuf)," group by substring(sdate,1,7),groupid  order by substring(sdate,1,7) ");
                    
            }
            else{
             sprintf(sqlbuf,"select substring(sdate,1,7),'',groupid,sum(ctime),sum(ubytes),sum(dbytes) \
                     from ncipservice,ncipindex  \
                     where userid = %d and (sdate='%s' and stime>='%s' and stime<='%s') and service=sid and ctime<1000000 ", lUserid,caSdate1,caStime,caEtime);
             if(strlen(caService)>0){
             	  sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
            }
            sprintf(sqlbuf+strlen(sqlbuf)," group by substring(sdate,1,7),groupid  order by substring(sdate,1,7) ");
                   
            }
   //         printf("sqlbuf=%s\n",sqlbuf);   
						psCur = pasDbOpenSql(sqlbuf,0);
        }
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0510设置网络"),ncLang("0549数据库出错"));
            return 0;
        }
    }
    else if(caOpt[0] == '1') { /* 按日  */
        strcpy(caOptlabel,ncLang("0103按日分组"));
        if(lUserid == 0) {
            if((utStrIsSpaces(caGroupid))&&(lGroupid0==0)) {
            	if(strcmp(caSdate1,caSdate2)!=0){
            		sprintf(sqlbuf,"select sdate,'',groupid,sum(ctime),sum(ubytes),sum(dbytes) \
                                    from ncipservice,ncipindex  \
                                    where ((sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' )) and service=sid and ctime<1000000 ",caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
                if(strlen(caService)>0){
                	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
                }
                sprintf(sqlbuf+strlen(sqlbuf)," group by sdate,groupid order by sdate ");
                                   
            	}
            	else{
            		sprintf(sqlbuf,"select sdate,'',groupid,sum(ctime),sum(ubytes),sum(dbytes) \
                                    from ncipservice,ncipindex \
                                    where (sdate='%s' and stime>='%s' and stime<='%s') and service=sid and ctime<1000000 ", caSdate1,caStime,caEtime);
                if(strlen(caService)>0){
                	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
                }
                 sprintf(sqlbuf+strlen(sqlbuf)," group by sdate,groupid order by sdate ");
                                    
            	}
              psCur = pasDbOpenSql(sqlbuf,0);
             }
             else {
             	if(strcmp(caSdate1,caSdate2)!=0){
             		sprintf(sqlbuf,"select a.sdate,'',c.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice  a,ncuser b,ncipindex c \
                                    where (a.userid = b.userid and a.service=c.sid and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu and level=1)) ) and ((a.sdate > '%s' and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' )) and ctime<1000000 ",lGroupid0,lGroupid0,caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
                 if(strlen(caService)>0){
                 	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
                  }
                  sprintf(sqlbuf+strlen(sqlbuf)," group by a.sdate,c.groupid order by a.sdate ");
                                    
                                    
             	}
             	else{
             		sprintf(sqlbuf,"select a.sdate,'',c.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice  a,ncuser b,ncipindex c \
                                    where (a.userid = b.userid and a.service=c.sid  and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu and level=1))) and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s') and a.ctime<1000000 ",lGroupid0,lGroupid0,caSdate1,caStime,caEtime);
                 if(strlen(caService)>0){
                 	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
                 }
                  sprintf(sqlbuf+strlen(sqlbuf)," group by a.sdate,c.groupid order by a.sdate ");
                                    
             	   
             	}
             	psCur = pasDbOpenSql(sqlbuf,0);
                
             
             }
             
             printf("sqlbuf=%s\n",sqlbuf);
        }
        else {
        		if(strcmp(caSdate1,caSdate2)!=0){
        			sprintf(sqlbuf,"select sdate,'',groupid,sum(ctime),sum(ubytes),sum(dbytes) \
                                    from ncipservice,ncipindex \
                                    where userid = %d and service=sid and ((sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' )) and ctime<1000000 ",lUserid,caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
             if(strlen(caService)>0){
             	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
             }
             sprintf(sqlbuf+strlen(sqlbuf)," group by sdate,groupid order by sdate ");
                                 
        		}
        		else{
        			sprintf(sqlbuf,"select sdate,'',groupid,sum(ctime),sum(ubytes),sum(dbytes) \
                                    from ncipservice,ncipindex \
                                    where userid = %d and service=sid and (sdate='%s' and stime>='%s' and stime<='%s') and ctime<1000000 ",lUserid,caSdate1,caStime,caEtime);
               if(strlen(caService)>0){
               	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
                 }
                 sprintf(sqlbuf+strlen(sqlbuf)," group by sdate,groupid order by sdate ");
                                   
        		   }
        		   psCur = pasDbOpenSql(sqlbuf,0);
        	}
            

   
   //     printf("sqlbuf=%s\n",sqlbuf);  
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0582统计"),ncLang("0549数据库出错"));
            return 0;
        }
    }
    
      else if (caOpt[0] == '4')  {  /* 按部门  */
        strcpy(caOptlabel,ncLang("0101按部门分组"));
        strcpy(caOpt,"4");
        if(lUserid == 0) {
            if((utStrIsSpaces(caGroupid))&&(lGroupid0==0)) {
            	if(strcmp(caSdate1,caSdate2)!=0){
            		sprintf(sqlbuf,"select c.groupname,'',d.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice a,ncuser b,ncgroup c,ncipindex d where (a.userid=b.userid and b.groupid=c.groupid and a.service=d.sid) and ((a.sdate > '%s' and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' )) and a.ctime<1000000 ", caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
                if(strlen(caService)>0){
                	sprintf(sqlbuf+strlen(sqlbuf)," and d.groupid in (%s) ",caService);
                }
                sprintf(sqlbuf+strlen(sqlbuf)," group by c.groupname,d.groupid order by c.groupname ");
                                  
            	}
            	else{
            		sprintf(sqlbuf,"select c.groupname,'',d.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice a,ncuser b,ncgroup c,ncipindex d where (a.userid=b.userid and b.groupid=c.groupid and a.service=d.sid) and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s') and a.ctime<1000000 ", caSdate1,caStime,caEtime);
               if(strlen(caService)>0){
               	sprintf(sqlbuf+strlen(sqlbuf)," and d.groupid in (%s) ",caService);
              }
              sprintf(sqlbuf+strlen(sqlbuf)," group by c.groupname,d.groupid order by c.groupname ");
                                    
            	}
 //       	printf("sqlbuf=%s\n",sqlbuf);   
               psCur = pasDbOpenSql(sqlbuf,0);
            }
            else {
            	if(strcmp(caSdate1,caSdate2)!=0){
            		sprintf(sqlbuf,"select c.groupname,'',d.groupid,sum(a.ctime),\
                                    sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice a,ncuser b,ncgroup c,ncipindex d  where (a.userid = b.userid and b.groupid=c.groupid and a.service=d.sid and (b.groupid = %lu  or b.groupid in (select groupid from ncgroup where pid=%lu and level=1))) \
                                    and ((a.sdate > '%s' and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' )) and a.ctime<1000000",lGroupid0,lGroupid0,caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
              if(strlen(caService)>0){
              	sprintf(sqlbuf+strlen(sqlbuf)," and d.groupid in (%s) ",caService);
              }
              sprintf(sqlbuf+strlen(sqlbuf)," group by c.groupname,d.groupid order by c.groupname ");
                                   
            	}
            	else{
            		sprintf(sqlbuf,"select c.groupname,'',d.groupid,sum(a.ctime),\
                                    sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice a,ncuser b,ncgroup c,ncipindex d  where (a.userid = b.userid and b.groupid=c.groupid and a.service=d.sid and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu and level=1))) \
                                    and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s')  and a.ctime<1000000 ",lGroupid0,lGroupid0,caSdate1,caStime,caEtime);
                if(strlen(caService)>0){
                	sprintf(sqlbuf+strlen(sqlbuf)," and d.groupid in (%s) ",caService);
                }
                sprintf(sqlbuf+strlen(sqlbuf)," group by c.groupname,d.groupid order by c.groupname");
                                    
            	}
                psCur = pasDbOpenSql(sqlbuf,0);
            }                
        }
        else {
        	if(strcmp(caSdate1,caSdate2)!=0){
        		sprintf(sqlbuf,"select c.groupname,'',d.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes)\
                                    from ncipservice a,ncuser b,ncgroup c,ncipindex d where (a.userid = b.userid and b.groupid=c.groupid and a.service=d.sid and a.userid = %d) and ((sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' )) and ctime<1000000 ",lUserid,caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
          if(strlen(caService)>0){
          	sprintf(sqlbuf+strlen(sqlbuf)," and d.groupid in (%s) ",caService);
          }
          sprintf(sqlbuf+strlen(sqlbuf)," group by c.groupname,d.groupid order by c.groupname");
                                    
        	}
        	else{
        	  sprintf(sqlbuf,"select c.groupname,'',d.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes)\
                                    from ncipservice a,ncuser b,ncgroup c,ncipindex d where (a.userid = b.userid and b.groupid=c.groupid and a.service=d.sid and a.userid = %d) and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s') and a.ctime<1000000 ",lUserid,caSdate1,caStime,caEtime);
            if(strlen(caService)>0){
            	sprintf(sqlbuf+strlen(sqlbuf)," and d.groupid in (%s) ",caService);
            }
            sprintf(sqlbuf+strlen(sqlbuf)," group by c.groupname,d.groupid order by c.groupname");
                                    
        	}
             psCur = pasDbOpenSql(sqlbuf,0);
        }
   //     printf("sqlbuf=%s\n",sqlbuf);   
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0549数据库出错"));
            return 0;
        }
    }
    
     else if (caOpt[0] == '5')  {  /* 按用户  */
        strcpy(caOptlabel,ncLang("0107按用户分组"));
        strcpy(caOpt,"5");
    
        if(lUserid == 0) {
            if((utStrIsSpaces(caGroupid))&&(lGroupid0==0)) {
            	if(strcmp(caSdate1,caSdate2)!=0){
            		sprintf(sqlbuf,"select b.dispname,inet_ntoa(b.ip) aa,c.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice a,ncuser b,ncipindex c where (a.userid=b.userid and a.service=c.sid) and ((a.sdate > '%s' and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' )) and a.ctime<1000000 ", caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
               if(strlen(caService)>0){
               	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
              }
              sprintf(sqlbuf+strlen(sqlbuf)," group by b.dispname,aa,c.groupid order by b.dispname ");
                                    
            	}
            	else{
            		sprintf(sqlbuf,"select b.dispname,inet_ntoa(b.ip) aa,c.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice a,ncuser b,ncipindex c where (a.userid=b.userid and a.service=c.sid) and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s') and a.ctime<1000000 ",caSdate1,caStime,caEtime);
                if(strlen(caService)>0){
                	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
                }
                sprintf(sqlbuf+strlen(sqlbuf)," group by b.dispname,aa,c.groupid order by b.dispname ");
                                    
            	}
                psCur = pasDbOpenSql(sqlbuf,0);
            }
            else {
            	if(strcmp(caSdate1,caSdate2)!=0){
            		sprintf(sqlbuf,"select b.dispname,inet_ntoa(b.ip) aa,c.groupid,sum(a.ctime),\
                                    sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice a,ncuser b,ncipindex c where (a.userid = b.userid and a.service=c.sid and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu and level=1))) \
                                    and ((a.sdate > '%s' and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' )) and a.ctime<1000000 ",lGroupid0,lGroupid0,caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime); 
                if(strlen(caService)>0){
                	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
                }
                sprintf(sqlbuf+strlen(sqlbuf),"  group by b.dispname,aa,c.groupid order by b.dispname ");
                                    
            	}
            	else {
            		sprintf(sqlbuf,"select b.dispname,inet_ntoa(b.ip) aa,c.groupid,sum(a.ctime),\
                                    sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice a,ncuser b,ncipindex c  where (a.userid = b.userid and a.service=c.sid and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu and level=1))) \
                                    and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s') and a.ctime<1000000 ",lGroupid0,lGroupid0,caSdate1,caStime,caEtime);
                if(strlen(caService)>0){
                	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
                }
                sprintf(sqlbuf+strlen(sqlbuf)," group by b.dispname,aa,c.groupid order by b.dispname ");
                                    
            	}
               psCur = pasDbOpenSql(sqlbuf,0);
            }                
        }
        else {
        	if(strcmp(caSdate1,caSdate2)!=0){
        		sprintf(sqlbuf,"select b.dispname,inet_ntoa(b.ip) aa,c.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes)\
                                    from ncipservice a,ncuser b,ncipindex c where (a.userid = b.userid and a.service=c.sid and a.userid = %d) and ((sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' )) and ctime<1000000 ",lUserid,caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
           if(strlen(caService)>0){
           	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
          }
          sprintf(sqlbuf+strlen(sqlbuf)," group by b.dispname,aa,c.groupid order by b.dispname desc ");
                                   
        	}
        	else{
        		 sprintf(sqlbuf,"select b.dispname,inet_ntoa(b.ip) aa,c.groupid,sum(a.ctime),sum(a.ubytes),sum(a.dbytes)\
                                    from ncipservice a,ncuser b,ncipindex c where (a.userid = b.userid,a.service=c.sid and a.userid = %d) and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s') and a.ctime<1000000 ",lUserid,caSdate1,caStime,caEtime);
             if(strlen(caService)>0){
             	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
            }
            sprintf(sqlbuf+strlen(sqlbuf)," group by b.dispname,aa,c.groupid order by b.dispname desc ");
                                    
        	}  			
            psCur = pasDbOpenSql(sqlbuf,0);
  
        }
    //    printf("sqlbuf=%s\n",sqlbuf);   
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0549数据库出错"));
            return 0;
        }
    }
    
    else {  /* 按时  */
        strcpy(caOptlabel,ncLang("0105按时分组"));
        strcpy(caOpt,"2");
        if(lUserid == 0) {
            if((utStrIsSpaces(caGroupid))&&(lGroupid0==0)) {
            	if(strcmp(caSdate1,caSdate2)!=0){
            		sprintf(sqlbuf,"select concat(sdate,' ',substring(stime,1,2)),'',groupid,sum(ctime),sum(ubytes),sum(dbytes) \
                                    from ncipservice,ncipindex where ( service=sid and (sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' )) and ctime<1000000 ",caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
               if(strlen(caService)>0){
               	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
              }
              sprintf(sqlbuf+strlen(sqlbuf)," group by sdate,substring(stime,1,2),groupid order by sdate,substring(stime,1,2) ");
                                   
            	}
            	else{
            	  sprintf(sqlbuf,"select concat(sdate,' ',substring(stime,1,2)),'',groupid,sum(ctime),sum(ubytes),sum(dbytes) \
                                    from ncipservice,ncipindex where service=sid and (sdate='%s' and stime>='%s' and stime<='%s') and ctime<1000000 ",caSdate1,caStime,caEtime);
                if(strlen(caService)>0){
                	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
                }
                sprintf(sqlbuf+strlen(sqlbuf)," group by sdate,substring(stime,1,2),groupid  order by sdate,substring(stime,1,2) ");
                                  
            	}
							psCur = pasDbOpenSql(sqlbuf,0);
            }
            else {
            	if(strcmp(caSdate1,caSdate2)!=0){
            	   sprintf(sqlbuf,"select concat(a.sdate,' ',substring(a.stime,1,2)),'',c.groupid,sum(a.ctime),\
                                    sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice a,ncuser b,ncipindex c where (a.userid = b.userid and a.service=c.sid and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu and level=1))) \
                                    and ((a.sdate > '%s' and a.sdate < '%s') or (a.sdate='%s' and a.stime>='%s' ) or (a.sdate='%s' and a.stime<='%s' )) and a.ctime<1000000 ",lGroupid0,lGroupid0,caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
                 if(strlen(caService)>0){
                 	sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
                }
                sprintf(sqlbuf+strlen(sqlbuf)," group by a.sdate,substring(a.stime,1,2),c.groupid order by a.sdate,substring(a.stime,1,2) ");
                                    
                 }
                 else{
                 	 sprintf(sqlbuf,"select concat(a.sdate,' ',substring(a.stime,1,2)),'',c.groupid,sum(a.ctime),\
                                    sum(a.ubytes),sum(a.dbytes) \
                                    from ncipservice a,ncuser b,ncipindex c where (a.userid = b.userid and a.service=c.sid and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu and level=1))) \
                                    and (a.sdate='%s' and a.stime>='%s' and a.stime<='%s') and a.ctime<1000000 ",lGroupid0,lGroupid0,caSdate1,caStime,caEtime);
                 if(strlen(caService)>0){
                 	 sprintf(sqlbuf+strlen(sqlbuf)," and c.groupid in (%s) ",caService);
                  }
                  sprintf(sqlbuf+strlen(sqlbuf)," group by a.sdate,substring(a.stime,1,2),c.groupid \
                                    order by a.sdate,substring(a.stime,1,2) ");
                                    
                }                   
               psCur = pasDbOpenSql(sqlbuf,0);
            }                
        }
        else {
        	if(strcmp(caSdate1,caSdate2)!=0){
        		sprintf(sqlbuf,"select concat(sdate,' ',substring(stime,1,2)),'',groupid,sum(ctime),sum(ubytes),sum(dbytes)\
                                    from ncipservice,ncipindex where userid = %d and service=sid and ((sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' ))  and ctime<1000000 ",lUserid,caSdate1,caSdate2,caSdate1,caStime,caSdate2,caEtime);
           if(strlen(caService)>0){
           	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
          }
          sprintf(sqlbuf+strlen(sqlbuf),"  group by sdate,substring(stime,1,2),groupid order by sdate,substring(stime,1,2) ");
                         
        	}
        	else{
        		  sprintf(sqlbuf,"select concat(sdate,' ',substring(stime,1,2)),'',groupid,sum(ctime),sum(ubytes),sum(dbytes)\
                                    from ncipservice,ncipindex where userid = %d and service=sid and (sdate='%s' and stime>='%s' and stime<='%s') and ctime<1000000 ",lUserid,caSdate1,caStime,caEtime);
          if(strlen(caService)>0){
          	sprintf(sqlbuf+strlen(sqlbuf)," and groupid in (%s) ",caService);
          }
          sprintf(sqlbuf+strlen(sqlbuf)," group by sdate,substring(stime,1,2),groupid order by sdate,substring(stime,1,2) ");
                                    
        	}
 					psCur = pasDbOpenSql(sqlbuf,0);
        }
   //    printf("sqlbuf=%s\n",sqlbuf);   
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0549数据库出错"));
            return 0;
        }
    }
    

    

    lStart = (atol(caStart) - 1) * lRowNum;
    if(lStart <= 0) {
        lStart = 1;
    }
    iReturn = 0;

   
    lpTime = (long *)malloc((lSumCol + 1) * sizeof(long));
    lpBytes = (long long *)malloc((lSumCol + 1) * sizeof(long long));
    memset(lpTime,0,(lSumCol + 1) * sizeof(long));
    memset(lpBytes,0,(lSumCol + 1) * sizeof(long long));
    if(utStrIsSpaces(caExport)) {
        psDbHead = utPltInitDb();
        if(atoi(caWay)==1){
     	   utPltPutVar(psDbHead,"flow",ncLang("0369流量"));
     	   utPltPutVar(psDbHead,"unit",ncLang("0882字节"));
        }
        if(atoi(caWay)==2){
     	   utPltPutVar(psDbHead,"flow",ncLang("0521时间"));
     	   utPltPutVar(psDbHead,"unit",ncLang("0393秒"));
        }
        utPltPutVar(psDbHead,"caService",caService);
        utPltPutVar(psDbHead,"username",caUsername);
        utPltPutVar(psDbHead,"groupid",caGroupid);
        utPltPutVar(psDbHead,"CurPg",caStart);
        utPltPutVar(psDbHead,"caWay",caWay);
        utPltPutVar(psDbHead,"caOpt",caOpt);
        utPltPutVar(psDbHead,"sdate1",caSdate1);
        utPltPutVar(psDbHead,"sdate2",caSdate2);
        utPltPutVar(psDbHead,"stime",caStime);
        utPltPutVar(psDbHead,"etime",caEtime);
        utPltPutVarF(psDbHead,"userid","%d",lUserid);
        if(atoi(caOpt)==4)
        utPltPutVar(psDbHead,"title_name",ncLang("0147部门"));
        else if(atoi(caOpt)==5){
        utPltPutVar(psDbHead,"title_name",ncLang("0729用户"));
        utPltPutVar(psDbHead,"iptitle","<td  width=90  align=\"center\" class=\"bord2\"> <font color=\"#FFFFFF\">&nbsp;IP</font></td>");
      }
        else
        utPltPutVar(psDbHead,"title_name",ncLang("0521时间"));
        for(i=0;i<lSumCol;i++)
        {
        	utPltPutLoopVarF(psDbHead,"i",i+1,"%d",i);
        	utPltPutLoopVarF(psDbHead,"serv",i+1,"%d",stGserver[i].lService);
        }
//        pSelect= pasUtlCvtInputSelect("opt",caOpt,ncLang("0567天,1;时,2;月,3;"));
//        utPltPutVar(psDbHead,"sopt", pSelect);
 //       free(pSelect);
        utPltPutVarF(psDbHead,"width","%d",width);
        utPltPutVar(psDbHead,"opt",caOpt);
        utPltPutVar(psDbHead,"optlabel",caOptlabel);
        iSumRec = 0;
        iReturn = 0;
        strcpy(caSdate9,"\0");
        iSumRec=0;
        memset(caIp,0,sizeof(caIp));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,30,caSdate,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG,  4, &lService,
                                       UT_TYPE_LONG,  4, &lTime,
                                       UT_TYPE_LONG8,  8, &lUbytes,
                                       UT_TYPE_LONG8,  8, &lDbytes);
        while(iSumRec<lStart-1 && iReturn==0) {
            if(strcmp(caSdate9,caSdate) != 0) {
                iSumRec ++;
                strcpy(caSdate9,caSdate);
            } 
            memset(caSdate,0,sizeof(caSdate));
            lService=0;
            lTime=0;
            lUbytes=0;
            lUbytes=0; 
            memset(caIp,0,sizeof(caIp));          
            iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,30,caSdate,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG,  4, &lService,
                                       UT_TYPE_LONG,  4, &lTime,
                                       UT_TYPE_LONG8,  8, &lUbytes,
                                       UT_TYPE_LONG8,  8, &lDbytes);
        }
        iNum = 0;                                   
        for(i=0;i<lSumCol;i++) {
           utPltPutLoopVar(psDbHead,"service", i+1,stGserver[i].caGname);

        }
        strcpy(caSdate9,"\0");
        while(iReturn == 0 && iNum < lRowNum) {
            if(strcmp(caSdate9,caSdate) != 0) {
                if(iNum > 0) {
                	
                    for(i=0;i<lSumCol;i++) {
                    
                    	if(caWay[0] == '2')
                        utPltPutLoopVarF2(psDbHead,"sbytes", iNum,i+1,"%+12s",utStrLtoF(lpTime[i],caTemp));
                        else
                        utPltPutLoopVarF2(psDbHead,"sbytes",iNum,i+1,"%+12s",utStrLtoF8(lpBytes[i],caTemp));
                    }
                }
                iSumRec++;
                iNum++;
                utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
                
                if(atoi(caOpt)==5)
                utPltPutLoopVarF(psDbHead,"ip", iNum,"<td  valign=\"bottom\" class=\"bord1\">&nbsp;%s </td>",caIp);
                
                for(i=0;i<=lSumCol;i++) {
                    lpTime[i] = 0;
                    lpBytes[i] = 0;
                }
                strcpy(caSdate9,caSdate);
            }
         
            lNum=ncUtlGetServicenumById(&stGserver,lSumCol,lService);
           if(lNum!=-1){
            lpTime[lNum] = lpTime[lNum] + lTime;
            lpBytes[lNum] = lpBytes[lNum] + lUbytes + lDbytes;
          }
           
            
            lService=0;
            lTime=0;
            lUbytes=0;
            lUbytes=0;    
            iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,30,caSdate,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG,  4, &lService,
                                       UT_TYPE_LONG,  4, &lTime,
                                       UT_TYPE_LONG8,  8, &lUbytes,
                                       UT_TYPE_LONG8,  8, &lDbytes);
        }
        if(iNum > 0) {
            for(i=0;i<lSumCol;i++) {
            
            	if(caWay[0] == '2')
               utPltPutLoopVarF2(psDbHead,"sbytes", iNum,i+1,"%+12s", utStrLtoF(lpTime[i],caTemp));
               else
               utPltPutLoopVarF2(psDbHead,"sbytes",iNum,i+1,"%+12s",utStrLtoF8(lpBytes[i],caTemp));
            }
            utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        }
        while(iReturn==0) {
            if(strcmp(caSdate9,caSdate) != 0) {
                iSumRec ++;
                strcpy(caSdate9,caSdate);
            }            
            iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,30,caSdate,
                                       UT_TYPE_STRING,15,caIp,
                                       UT_TYPE_LONG,  4, &lService,
                                       UT_TYPE_LONG,  4, &lTime,
                                       UT_TYPE_LONG8,  8, &lUbytes,
                                       UT_TYPE_LONG8,  8, &lDbytes);
        }
        lSumPage = (iSumRec - 1) / lRowNum  + 1;
        utPltPutVarF(psDbHead,"TotPg", "%lu", lSumPage);
        pasDbCloseCursor(psCur);
 //       if(caWay[0] == '1') {
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncflowcount.htm");
//        }
//        else {
//            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nctimecount.htm");
//        }
    }
    else {  /* 导出  */
        FILE *fp;
        char caFilename[128];
        char caPath[128];
        char caFile[128];
        char caFile1[128];
        char caFile2[128];
        char caFile3[128];
        sprintf(caFile3,"ncnetcount%d",utStrGetId());
        sprintf(caFilename,"%s.csv",caFile3);
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613网站访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
       
        if(caWay[0] == '1') {
            fprintf(fp,ncLang("0006%-32s %s网络访问流量统计表(%s-%s)\n"),"\0",caUsername,caSdate1,caSdate2);
           ncWebExportNetCount_new(psShmHead,psCur,fp,lpTime,lpBytes,lSumCol,1,lserv,&stGserver);
        }
        else {
            fprintf(fp,ncLang("0009%-32s %s网络访问时间统计表(%s-%s)\n"),"\0",caUsername,caSdate1,caSdate2);
            ncWebExportNetCount_new(psShmHead,psCur,fp,lpTime,lpBytes,lSumCol,0,lserv,&stGserver);
        }
       
        pasDbCloseCursor(psCur);
                printf("caPath=%s\n",caPath);
        fclose(fp);

       /* 
        
        sprintf(caFile1,"%s/%s",caPath,caFilename);
        
        printf("caFile1=%s\n",caFile1);
        sprintf(caTemp,"perl txt2pdf.pl %s",caFile1);
        system(caTemp);
        sprintf(caFilename,"%s.pdf",caFile3);
       */    
        
        
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);

    }
    free(lpTime);
    free(lpBytes);
    return 0;
}

/* 导出上网统计  */
int ncWebExportNetCount_new(utShmHead *psShmHead, pasDbCursor *psCur,FILE *fp,
               long *lpTime,long long *lpBytes,int lSumCol,int iFlags,long *lserv,struct ncSflow_s *stGserver)
{
    int iNum,i,iReturn;
    char caSdate[32],caSdate9[32],caTemp[32];
    long lService,lTime,lNum;
    long long lUbytes,lDbytes;
    char caIp[16];

    long temp_i=0;
    long ser_flag=0;
    iNum = 0; 
    
   
    
    
                  
    fprintf(fp,"\"%s\"",ncLang("0522时间或部门用户"));
   
    for(i=0;i<lSumCol;i++) {
        fprintf(fp,",\"%s\"",stGserver[i].caGname);
    }
    fprintf(fp,"\n");
    strcpy(caSdate9,"\0");
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,30,caSdate,
                                    UT_TYPE_STRING,15,caIp,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lTime,
                                   UT_TYPE_LONG8,  8, &lUbytes,
                                   UT_TYPE_LONG8,  8, &lDbytes);
    while(iReturn == 0) {
        if(strcmp(caSdate9,caSdate) != 0) {
            if(iNum > 0) {
                for(i=0;i<lSumCol;i++) {
                	
                    if(iFlags == 0) {
                        fprintf(fp,",\"%s\"",utStrLtoF(lpTime[i],caTemp));
                    }
                    else {
                        fprintf(fp,",\"%s\"",utStrLtoF8(lpBytes[i],caTemp));
                    }
                    
                }
                fprintf(fp,"\n");
            }
            iNum++;
            if(strlen(caIp)>0)
            fprintf(fp,"\"%s(%s)\"",caSdate,caIp);
            else
             fprintf(fp,"\"%s\"",caSdate);
             
            for(i=0;i<=lSumCol;i++) {
                lpTime[i] = 0;
                lpBytes[i] = 0;
            }
            strcpy(caSdate9,caSdate);
        }

       lNum=ncUtlGetServicenumById(stGserver,lSumCol,lService);
   //    printf("lNum=%d\n",lNum);
  //     printf("lService=%d\n",lService);
          if(lNum!=-1){
            lpTime[lNum] = lpTime[lNum] + lTime;
            lpBytes[lNum] = lpBytes[lNum] + lUbytes + lDbytes;
          }
        
             
        
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,30,caSdate,
                                   UT_TYPE_STRING,15,caIp,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lTime,
                                   UT_TYPE_LONG8,  8, &lUbytes,
                                   UT_TYPE_LONG8,  8, &lDbytes);
    }

    if(iNum > 0) {
        for(i=0;i<lSumCol;i++) {
        
            if(iFlags == 0) {
                 fprintf(fp,",\"%s\"",utStrLtoF(lpTime[i],caTemp));
            }
            else {
                 fprintf(fp,",\"%s\"",utStrLtoF8(lpBytes[i],caTemp));
            }
        }
        fprintf(fp,"\n");
    }

    return 0;
}








//7.5显示用户上网流量趋势图
int ncWebShowUserFlowBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    pasDbCursor *psCur;
    char caDate[20];  
    char caFlag[8];
    char caPre_flag[8],caAv_un[8];
    char sqlbuf[2048];
    int      iReturn,i,j;
    long lTime,lStarttime,lEndtime;
    char caSid[32],caUnit[16];
    char caTemp[2048];
    long y_sum;
    int m,n,iNum;
    int mm,nn;
    long lSid,lConntime;
    long index,x_sum;              //index  X轴下标   x_sum X轴下标总数  
    char caGitem[16],caSdate[20];   
    	long long lBt[32];
    	long long lBtup[32];
    	long long lBytesup;
    long long lBytes;
    long lService,lGroupid;
    long lSumindex; 
    char caEdate[20];  
    long lCount;                            
    char caSdate_all[20];                                       
    char group_id[15],caUserid[15];
    char caPdf[12];
    struct chart_datasetf *pChart;           //用于PDF文档输出
    int rowrow;                     
    int column; 
    char caTitle[128];
    char filename[128];
    char caStatdate[128];

    char **desc;
    char caTitlename[64];
    char caEdate_all[24],caItem[16],caScal[16];
    for(i=0;i<32;i++){
    	lBt[i]=0;
    	lBtup[i]=0;
    }
    
    utMsgGetSomeNVar(psMsgHead,6,"sdate",    UT_TYPE_STRING,10,caSdate,
   		                            "av_un",    UT_TYPE_STRING,8,caAv_un,					//按2：小时，1：月，3：日统计
																  "pre_flag", UT_TYPE_STRING,2,caPre_flag,	    //显示前一段时间的数据1：向前，2：向后
																  "userid",   UT_TYPE_STRING, 14, caUserid,
																  "groupid",  UT_TYPE_STRING,14,group_id,
																  "pdf",      UT_TYPE_STRING,8,caPdf);
																 
   
  psDbHead = utPltInitDb(); 
  
  if(strlen(group_id)>0){
  	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(group_id),ncLang("0629未知")));
  }
  else if(atol(caUserid)>0){
  	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
  }
  else
    strcpy(caTitlename,utComGetVar_sd(psShmHead,"rootname",ncLang("0269公司")));
    
    
    utPltPutVar(psDbHead,"titlename",caTitlename);
     

  if(strlen(caAv_un)==0) strcpy(caAv_un,"2");										//默认按分钟统计
  
	 if(atol(caAv_un)==1){									//按月统计(nchttplog)
	    x_sum=12;
	    y_sum=12;
	 }
	 else if(atol(caAv_un)==2){						  //按小时统计(nchttplog)
	 	x_sum=24;
	 	y_sum=24;
	}
	else if(atol(caAv_un)==3){						  //按天统计(nchttplog)
		x_sum=30;
		y_sum=30;
	}

	if(strlen(caSdate)==0){											//默认没时间按系统时间，并按小时统计
		    lTime=time(0);
//		    strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));

	      if(atol(caAv_un)==3){                                        //按天统计
		    	ncUtlMonthDate(lTime,caSdate,caEdate);	
//		   	  printf("lMin=%d\n",lMin);
		        sprintf(caSdate_all,"%s 00:00",caSdate);
		        sprintf(caEdate_all,"%s 23:59",caEdate);
		        lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		        lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
		     }else if(atol(caAv_un)==2){                                   //按小时
		       strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));
		     		sprintf(caSdate_all,"%s 00:00");
		     		lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		     		lEndtime=lStarttime+24*3600;
		    }else{   
		    	 	                   //按月统计
		    	   strcpy(caSdate,utTimFormat("%Y/01/01",lTime));
		    	   strcpy(caEdate,utTimFormat("%Y/12/31",lTime));
				 		lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
						lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate);                                                //按日统计
		    }
	 
	}else {
		  	if(atol(caAv_un)==1){                 //按月统计
		  		lTime=utTimStrToLong("%Y/%m/%d",caSdate);
		  		strcpy(caSdate,utTimFormat("%Y/01/01",lTime));
		    	strcpy(caEdate,utTimFormat("%Y/12/31",lTime));
//	printf("caEdate=%s\n",caEdate);	    	
		    		sprintf(caSdate_all,"%s 00:00",caSdate);
		        sprintf(caEdate_all,"%s 23:59",caEdate);
		    	lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		       lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
		    	

		  		
		  	}else if(atol(caAv_un)==2){           //按小时
		        sprintf(caSdate_all,"%s 00:00",caSdate);
		        sprintf(caEdate_all,"%s 23:59",caSdate);
		        lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		        lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);

		  	}else if(atol(caAv_un)==3){             //按天
		  	 	lStarttime=utTimStrToLong("%Y/%m/%d",caSdate);	
		  	 	ncUtlMonthDate(lStarttime,caSdate,caEdate);

		        sprintf(caSdate_all,"%s 00:00",caSdate);
		        sprintf(caEdate_all,"%s 23:59",caEdate);

		        lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		        lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
		  	}
	}
//处理上一段和下一时间段		  
		if(atol(caPre_flag)==1){                   //按月统计
		  	if(atol(caAv_un)==1){
		  		
		  		lTime=lStarttime-300*24*3600;
		  		strcpy(caSdate,utTimFormat("%Y/01/01",lTime));
		    	strcpy(caEdate,utTimFormat("%Y/12/31",lTime));
		    	
		    	sprintf(caSdate_all,"%s 00:00",caSdate);
		      sprintf(caEdate_all,"%s 23:59",caEdate);
		    	lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		      lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
		  	
		  	}else if(atol(caAv_un)==2){                    //按小时
		  		lStarttime=lStarttime-24*3600;
		  		lEndtime=lEndtime-24*3600;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		  		strcpy(caEdate,utTimFormat("%Y/%m/%d",lStarttime));
		  	  sprintf(caSdate_all,"%s 00:00",caSdate);
		      sprintf(caEdate_all,"%s 23:59",caEdate);
		    	lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		      lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
		  		
		  	}else if(atol(caAv_un)==3){                    //按天统计
		  		lTime=lStarttime-15*24*3600;
		  		ncUtlMonthDate(lTime,caSdate,caEdate);
		  		sprintf(caSdate_all,"%s 00:00",caSdate);
		      sprintf(caEdate_all,"%s 23:59",caEdate);
		    	lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		      lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
		  	}
	}else if(atol(caPre_flag)==2){
		 	  if(atol(caAv_un)==1){
		  		lTime=lEndtime+300*24*3600;
		  		strcpy(caSdate,utTimFormat("%Y/01/01",lTime));
		    	strcpy(caEdate,utTimFormat("%Y/12/31",lTime));
		    	sprintf(caSdate_all,"%s 00:00",caSdate);
		      sprintf(caEdate_all,"%s 23:59",caEdate);
		    	lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		      lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
		    	
		    }else if(atol(caAv_un)==2){
		  		lTime=lEndtime+20*3600;
		  		
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));
		  		strcpy(caEdate,utTimFormat("%Y/%m/%d",lTime));
		  		sprintf(caSdate_all,"%s 00:00",caSdate);
		      sprintf(caEdate_all,"%s 23:59",caEdate);
		    	lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		      lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
		  		
		  	}else if(atol(caAv_un)==3){
		  		lStarttime=lStarttime+35*24*3600;
		  		ncUtlMonthDate(lStarttime,caSdate,caEdate);
		  		sprintf(caSdate_all,"%s 00:00",caSdate);
		      sprintf(caEdate_all,"%s 23:59",caEdate);

		    	lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		      lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);

		  	}
	}
 		utPltPutVar(psDbHead,"sdate",caSdate);
   
   	utPltPutVar(psDbHead,"avun",caAv_un);

    
    
    utPltPutVar(psDbHead,"tjsj",caSdate);
   

 	 	 if(atol(caAv_un)==1){                                           //按月平均		--->统计ncipcount
		 	                                          // 用户统计																																																
		    sprintf(caTemp,"select substr(sdate,6,2) aa,sum(ubytes),sum(dbytes) from ncipcount a,ncuser b where a.userid=b.userid and sdate>='%s' and sdate<='%s' ",caSdate,caEdate);
		    if(strlen(group_id)>0){
		      if(atol(group_id)==0){
		    		sprintf(caTemp+strlen(caTemp)," and b.groupid=0");
		    	}
		    	else
	 				 	sprintf(caTemp+strlen(caTemp)," and (b.groupid =%s or b.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	   	}
		    if(strlen(caUserid)>0){
		    	sprintf(caTemp+strlen(caTemp)," and a.userid=%s ",caUserid);
		    }
		    sprintf(caTemp+strlen(caTemp)," group by aa order by aa ");
		  

		 }else if(atol(caAv_un)==2){  
  	                        //按小时统计
  	   sprintf(caTemp,"select stime aa,sum(ubytes),sum(dbytes) from ncipcount a,ncuser b where a.userid=b.userid and sdate='%s'  ",caSdate);
		    if(strlen(group_id)>0){
		    	if(atol(group_id)==0){
		    		sprintf(caTemp+strlen(caTemp)," and b.groupid=0");
		    	}
		    	else
	 				 	sprintf(caTemp+strlen(caTemp)," and (b.groupid =%s or b.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	   	}
		    if(strlen(caUserid)>0){
		    	sprintf(caTemp+strlen(caTemp)," and a.userid=%s ",caUserid);
		    }
		    sprintf(caTemp+strlen(caTemp)," group by aa order by aa ");
     }else if(atol(caAv_un)==3){                    //按天统计用户

     	 sprintf(caTemp,"select substr(sdate,9,2) aa,sum(ubytes),sum(dbytes) from ncipcount a,ncuser b where a.userid=b.userid and sdate>='%s' and sdate<='%s' ",caSdate,caEdate);
		    if(strlen(group_id)>0){
		    	if(atol(group_id)==0){
		    		sprintf(caTemp+strlen(caTemp)," and b.groupid=0");
		    	}
		    	else
	 				 	sprintf(caTemp+strlen(caTemp)," and (b.groupid =%s or b.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	   	}
		    if(strlen(caUserid)>0){
		    	sprintf(caTemp+strlen(caTemp)," and a.userid=%s ",caUserid);
		    }
		    sprintf(caTemp+strlen(caTemp)," group by aa order by aa ");
     		
     		
     		
     		
 }
 
   printf("**22**caTemp=%s\n",caTemp);
   psCur = pasDbOpenSql(caTemp,0);
   if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错1"));
        return 0;
    }
    memset(caItem,0,sizeof(caItem));
    lBytes=0;
    lBytesup=0;
   
    
    
    iReturn = pasDbFetchInto(psCur,
                                   UT_TYPE_STRING,2,caItem,
                                   UT_TYPE_LONG8,8,&lBytesup,
                                   UT_TYPE_LONG8,8,&lBytes);
                                  
    iNum=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	
    	
        index=atol(caItem);
        
        lBt[index]=lBytes;
    	  lBtup[index]=lBytesup;
    memset(caItem,0,sizeof(caItem));
    lBytes=0;
    lBytesup=0;
    
    iReturn = pasDbFetchInto(psCur,
                                   UT_TYPE_STRING,2,caItem,
                                   UT_TYPE_LONG8,8,&lBytesup,
                                   UT_TYPE_LONG8,8,&lBytes);
    }
    pasDbCloseCursor(psCur);
  
  //输出X轴刻度

	  for(mm=0;mm<x_sum;mm++){
	  	    utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
	  	    if(atol(caAv_un)==2){
            
	  			   sprintf(caTemp,"%02d:00",mm);	 
	  		  }
	  		  else if(atol(caAv_un)==1){
	  		  	sprintf(caScal,utTimFormat("%Y",lStarttime));
	  		  	sprintf(caTemp,"%s/%02d",caScal,mm+1);
	  		  }
	  		  else if(atol(caAv_un)==3){
	  		  	strcpy(caScal,utTimFormat("%m",lStarttime));
	  		  	sprintf(caTemp,"%s/%02d",caScal,mm+1);
	  		  }
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
	  	
	  	//输出到表格

	  	    		if(atol(caAv_un)==3){                                               //对于按日统计，数组从1开始，其余从0开始
	  			  utPltPutLoopVarF2(psDbHead,"data2",mm+1,1,"%0.2f",(float)lBt[mm+1]/(1024*1024)); 
	  			  utPltPutLoopVarF2(psDbHead,"data2",mm+1,2,"%0.2f",(float)lBtup[mm+1]/(1024*1024)); 
	  			  utPltPutLoopVarF(psDbHead,"tot",mm+1,"%0.2f",((float)lBt[mm+1]+(float)lBtup[mm+1])/(1024*1024));
	  			} 
	  		else {
	  			  utPltPutLoopVarF2(psDbHead,"data2",mm+1,1,"%0.2f",(float)lBt[mm]/(1024*1024)); 
	  			  utPltPutLoopVarF2(psDbHead,"data2",mm+1,2,"%0.2f",(float)lBtup[mm]/(1024*1024)); 
	  			  utPltPutLoopVarF(psDbHead,"tot",mm+1,"%0.2f",((float)lBt[mm]+(float)lBtup[mm])/(1024*1024));
    		  }
	  	
	  	
	  	
	  	
	  	
	  		 
	  	  	
	  }
	   
 //输出服务图形数据 

    for(i=0;i<1;i++){
    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);
    	utPltPutLoopVarF(psDbHead,"iNum",i+2,"%d",i+1);
    	//printf("gservice=%s\n",sGservice[i].caGname);
    	utPltPutLoopVar(psDbHead,"sevname",i+1,ncLang("0678下行流量"));
      utPltPutLoopVar(psDbHead,"sevname",i+2,ncLang("0505上行流量"));
    	for(mm=0;mm<x_sum;mm++){
    		//printf("i=%d;mm=%d;service=%d;bytes=%lld\n",i,mm,sGservice[i].lService,sGservice[i].lBytesqs[mm]);
    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);
    		
    		utPltPutLoopVarF2(psDbHead,"num",i+2,mm+1,"%d",i+1);
    		utPltPutLoopVarF2(psDbHead,"num2",i+2,mm+1,"%d",mm);

    		if(atol(caAv_un)==3){                                               //对于按日统计，数组从1开始，其余从0开始
	  			  utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%0.0f",(float)lBt[mm+1]/(1024*1024)); 
	  			  utPltPutLoopVarF2(psDbHead,"data",i+2,mm+1,"%0.0f",(float)lBtup[mm+1]/(1024*1024)); 
	  			} 
	  		else {
    		    utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%0.0f",(float)lBt[mm]/(1024*1024));	
    		    utPltPutLoopVarF2(psDbHead,"data",i+2,mm+1,"%0.0f",(float)lBtup[mm]/(1024*1024));	
    		  }
    }
  }
  utPltPutVarF(psDbHead,"sum","%d",2);
  utPltPutVarF(psDbHead,"sum_d","%d",x_sum);
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
lSumindex=2;
	if(strlen(caPdf)>0){
	//表格标签
	 desc=malloc((lSumindex+1)*sizeof(char*));
	 for(i=0;i<lSumindex+1;i++){
	 	desc[i]=malloc(32);
	
   }
   strcpy(desc[0],ncLang("0678下行流量"));
   strcpy(desc[1],ncLang("0505上行流量"));
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
	  		  }
	  		  else if(atol(caAv_un)==1){
	  		  	sprintf(caScal,utTimFormat("%Y",lStarttime));
	  		  	sprintf(caTemp,"%s/%02d",caScal,i+1);
	  		  }
	  		  else if(atol(caAv_un)==3){
	  		  	strcpy(caScal,utTimFormat("%m",lStarttime));
	  		  	sprintf(caTemp,"%s/%02d",caScal,i+1);
	  		  }
   		  
   	   pChart[i].name=strdup(caTemp);
   	  
   	   pChart[i].dataset=malloc((lSumindex+2)*sizeof(float)) ;
   	   if(atol(caAv_un)==3){
   	   	pChart[i].dataset[0]=(float)lBt[i+1]/(1024*1024); 
   	   	pChart[i].dataset[1]=(float)lBtup[i+1]/(1024*1024);
   	  }
   	  else{
   	  	pChart[i].dataset[0]=(float)lBt[i]/(1024*1024); 
   	   	pChart[i].dataset[1]=(float)lBtup[i]/(1024*1024);
   	  }
   	   	
   	  
   	 }

      rowrow=x_sum;
      column=lSumindex;
   
    sprintf(caTitle,ncLang("1966【%s】上网流量趋势图表"),caTitlename);
    sprintf(filename,"%s.PDF",caTitle); 

    sprintf(caStatdate,ncLang("1967统计时间:%s"),caSdate);

    sprintf(caTemp,ncLang("1942单位(%s)"),"MB");
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


	  utPltPutVar(psDbHead,"userid",caUserid);
	  utPltPutVar(psDbHead,"groupid",group_id);

	  
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_flowbytime.htm");
    
    return 0;
}

struct flow_she{
		long userid;
		long long lBytes;
		unsigned long stime;
		unsigned long etime;
};
struct flow_s{
	  	long userid;
	  	long long lBytes;
};
static int ncUtlSortOnId(char *p1,char *p2)
{
    int i;
    long long l;
   
    struct flow_s psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct flow_s));
    memcpy(&psC2,p2,sizeof(struct flow_s));    
    l=psC2.lBytes-psC1.lBytes;
    if(l>0) return 1;
    if(l<0) return -1;
    if(l==0) return 0;
  
}
struct flow_tar{
		long userid;
		long long lBytes;
};
static int ncUtlSortOn_userId(char *p1,char *p2)
{
    int i;
    long long l;
   
    struct flow_tar psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct flow_tar));
    memcpy(&psC2,p2,sizeof(struct flow_tar));    
    l=psC2.userid-psC1.userid;
    if(l>0) return 1;
    if(l<0) return -1;
    if(l==0) return 0;
  
}

int ncUtlGetMonthNum(char *caDate)
{
    unsigned long lTime;
    struct tm *tm;
    lTime = utTimStrToLong("%Y/%m/%d",caDate);
    tm = localtime(&lTime);
    return tm->tm_mday;
}
//int gettimeofday(struct timeval *tv, struct timezone *tz); 
//先表，后规则
int ncUtlWorkTime_ta_ru(char *field,char *field2,struct flow_she *fflow,int t,int lCount,struct flow_s *endFlow,char *caTime)
{		struct timeval start,end,end1;
	  
	  static char caSql[2048];
	  char caTemp[2048];
	   pasDbCursor *psCur;
	   long iSum;
    char *p;
    long lTimetype;
    long lLower,lUpper,lStarttime,lStoptime;
    long iReturn,g=0,r=0,i=0,j=0,k=0,iNum=0;
    char lLower_ch[20];
    char lUpper_ch[20];
    char stime_ch[20],etime_ch[20],stime_final[32],etime_final[32];
    char ta_sdate[20],ta_stime[20],ta_edate[20],ta_etime[20];
    int ta_days=0;
    long time_sum=0,be_time=0,ru_stime,ru_etime,ta_stime_lo,ta_etime_lo;
    char stime_rule[32],etime_rule[32];
    struct flow_tar{
				  	long userid;
				  	long long lBytes;
		};
		struct flow_tar *tarFlow; 
		struct flow_s{
	  	long userid;
	  	long long lBytes;
		};
		struct flow_s *sFlow;
    long wkcount=0;
    
    struct rule_s{
	  	long timetype;
	  	long lLower;
	  	long lUpper;
	  	long lStarttime;
	  	long lStoptime;
	  	long flags;
	  	long ru_sdate;
	  	long ru_stime;
	  	long ru_edate;
	  	long ru_etime;
		};
		struct rule_s *sRule;
		long flags=0;
		char stime_for[20]="",etime_for[20]="";
		long date_tmp=0,week_tmp=0,month_tmp=0;
		long st_depart=0,ed_depart=0,st_flg=0,et_flg=0,sno_flg=0,eno_flg=0;
		
		long sdate_tmp=0,sweek_tmp=0,smonth_tmp=0;
    long edate_tmp=0,eweek_tmp=0,emonth_tmp=0;
		long ssstime=0,eeetime=0;
		
		//printf("week=%d,month=%d\n",ncUtlGetWeekNum("2010-11-23"),ncUtlGetMonthNum("2010-11-23"));
		//printf("27week=%d,month=%d\n",ncUtlGetWeekNum("2010-11-27"),ncUtlGetMonthNum("2010-11-27"));  //6   27
		//printf("28week=%d,month=%d\n",ncUtlGetWeekNum("2010-11-28"),ncUtlGetMonthNum("2010-11-28"));  //0   28
    sprintf(caTemp,"select count(*) from ncworktime ");
	  iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&wkcount);
	  sRule=(struct rule_s *)malloc(sizeof(struct rule_s)*(wkcount+1));
    psCur=pasDbOpenSql("select timetype,llower,lupper,starttime,stoptime,flags from ncworktime order by flags desc,timetype asc,llower asc,starttime asc ",0);
    if(psCur==NULL){
        return NULL;
    }
    lTimetype=0;lLower=0;lUpper=0;lStarttime=0;lStoptime=0;iSum=0;flags=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
                                   UT_TYPE_LONG,4,&lLower,
                                   UT_TYPE_LONG,4,&lUpper,
                                   UT_TYPE_LONG,4,&lStarttime,
                                   UT_TYPE_LONG,4,&lStoptime,
                                   UT_TYPE_LONG,4,&flags);
    iNum=0;
    while(iReturn==0){
			sRule[iNum].timetype=lTimetype;
			sRule[iNum].lLower=lLower;
			sRule[iNum].lUpper=lUpper;
			sRule[iNum].lStarttime=lStarttime;
			sRule[iNum].lStoptime=lStoptime;
			sRule[iNum].flags=flags;
			sprintf(lLower_ch,"%d",lLower);       
			sprintf(lUpper_ch,"%d",lUpper); 
			if(lTimetype==1){
				sprintf(stime_rule,"%d%06d",lLower,lStarttime);
				sprintf(etime_rule,"%d%06d",lUpper,lStoptime);
			}else if(lTimetype==2||lTimetype==3||lTimetype==4){
				sprintf(stime_rule,"20101101%06d",lStarttime);
				sprintf(etime_rule,"20101101%06d",lStoptime);
			}
			//ru_stime=utTimStrToLong("%Y%m%d%H%M%S",stime_rule)-utTimStrToLong("%Y%m%d",lLower_ch);
			//ru_etime=utTimStrToLong("%Y%m%d%H%M%S",etime_rule)-utTimStrToLong("%Y%m%d",lUpper_ch);
			if(lTimetype==1){
				sRule[iNum].ru_sdate=utTimStrToLong("%Y%m%d",lLower_ch);
				sRule[iNum].ru_stime=utTimStrToLong("%Y%m%d%H%M%S",stime_rule)-utTimStrToLong("%Y%m%d",lLower_ch);				
				sRule[iNum].ru_edate=utTimStrToLong("%Y%m%d",lUpper_ch);
				sRule[iNum].ru_etime=utTimStrToLong("%Y%m%d%H%M%S",etime_rule)-utTimStrToLong("%Y%m%d",lUpper_ch);
			}else if(lTimetype==2){
				sRule[iNum].ru_sdate=lLower;
				sRule[iNum].ru_stime=utTimStrToLong("%Y%m%d%H%M%S",stime_rule)-utTimStrToLong("%Y%m%d","20101101");
				
				sRule[iNum].ru_edate=lUpper;
				sRule[iNum].ru_etime=utTimStrToLong("%Y%m%d%H%M%S",etime_rule)-utTimStrToLong("%Y%m%d","20101101");
			}else if(lTimetype==3){
				sRule[iNum].ru_sdate=lLower;
				sRule[iNum].ru_stime=utTimStrToLong("%Y%m%d%H%M%S",stime_rule)-utTimStrToLong("%Y%m%d","20101101");
				sRule[iNum].ru_edate=lUpper;
				sRule[iNum].ru_etime=utTimStrToLong("%Y%m%d%H%M%S",etime_rule)-utTimStrToLong("%Y%m%d","20101101");
			}else if(lTimetype==4){
				sprintf(stime_for,"%s%04d",utTimFormat("%Y",time(0)),lLower);
				sprintf(etime_for,"%s%04d",utTimFormat("%Y",time(0)),lUpper);
				sRule[iNum].ru_sdate=utTimStrToLong("%Y%m%d",stime_for);
				sRule[iNum].ru_stime=utTimStrToLong("%Y%m%d%H%M%S",stime_rule)-utTimStrToLong("%Y%m%d","20101101");
				sRule[iNum].ru_edate=utTimStrToLong("%Y%m%d",etime_for);
				sRule[iNum].ru_etime=utTimStrToLong("%Y%m%d%H%M%S",etime_rule)-utTimStrToLong("%Y%m%d","20101101");
				memset(stime_for,0,sizeof(stime_for));
				memset(etime_for,0,sizeof(etime_for));
			}
			iNum++;
			memset(lLower_ch,0,sizeof(lLower_ch));
			memset(lUpper_ch,0,sizeof(lUpper_ch));
			memset(stime_rule,0,sizeof(stime_rule));
			memset(etime_rule,0,sizeof(etime_rule));
    	lTimetype=0;lLower=0;lUpper=0;lStarttime=0;lStoptime=0;flags=0;
    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
			                               UT_TYPE_LONG,4,&lLower,
			                               UT_TYPE_LONG,4,&lUpper,
			                               UT_TYPE_LONG,4,&lStarttime,
	                                   UT_TYPE_LONG,4,&lStoptime,
	                                   UT_TYPE_LONG,4,&flags);
    }
    pasDbCloseCursor(psCur);
    
    //gettimeofday( &start, NULL ); 
    tarFlow=(struct flow_tar*)malloc(sizeof(struct flow_tar)*(lCount+1));     
   	r=0;
   	time_sum=0;
   if(strcasecmp(caTime,"worktime") == 0) {	
    for(g=0;g<t;g++){
    		//先判断，开始和结束是否在统计时刻内，若在修改标志位 
    		ssstime=fflow[g].stime;
    		eeetime=fflow[g].etime;
    		sdate_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
    		sweek_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",fflow[g].stime));
    		smonth_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",fflow[g].stime));
    		edate_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		eweek_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",fflow[g].etime));
    		emonth_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",fflow[g].etime));
    		for(j=0;j<iNum;j++){    //循环规则     --非优先级高
    				
    			if(sRule[j].flags==1){			//非工作时间规则
	    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    						//先判断日期
	    						if(sdate_tmp>=sRule[j].ru_sdate&&sdate_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if(edate_tmp>=sRule[j].ru_sdate&&edate_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else if(sRule[j].timetype==2){
	    						if((sweek_tmp>sRule[j].ru_sdate&&sweek_tmp<sRule[j].ru_edate)||sweek_tmp==sRule[j].ru_edate||sweek_tmp==sRule[j].ru_sdate){
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if((eweek_tmp>sRule[j].ru_sdate&&eweek_tmp<sRule[j].ru_edate)||eweek_tmp==sRule[j].ru_edate||eweek_tmp==sRule[j].ru_sdate){
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else if(sRule[j].timetype==3){
    							if(smonth_tmp>=sRule[j].ru_sdate&&smonth_tmp<=sRule[j].ru_edate){
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if(emonth_tmp>=sRule[j].ru_sdate&&emonth_tmp<=sRule[j].ru_edate){
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else{}
    			}else if(sRule[j].flags==0){	
    				if(sRule[j].timetype==1||sRule[j].timetype==4){
	    					//先判断日期
	    					if(sdate_tmp>=sRule[j].ru_sdate&&sdate_tmp<=sRule[j].ru_edate&&sno_flg==0){
	    					//在判断时刻  
	    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							st_flg=1;
	    						}else{			//不在工作时间内执行下条规则
	    						}
	    					}
	    					if(edate_tmp>=sRule[j].ru_sdate&&edate_tmp<=sRule[j].ru_edate&&eno_flg==0){
	    					//在判断时刻  
	    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							et_flg=1;
	    						}else{}
	    					}
    				}else if(sRule[j].timetype==2){
    						if(sweek_tmp>=sRule[j].ru_sdate&&sweek_tmp<=sRule[j].ru_edate&&sno_flg==0){  
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							st_flg=1;
		    						}else{}
	    					}
	    					if(eweek_tmp>=sRule[j].ru_sdate&&eweek_tmp<=sRule[j].ru_edate&&eno_flg==0){  
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							et_flg=1;
		    						}else{}
	    					}
    				}else if(sRule[j].timetype==3){
    						if(smonth_tmp>=sRule[j].ru_sdate&&smonth_tmp<=sRule[j].ru_edate&&sno_flg==0){ 
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							st_flg=1;
		    						}else{}
	    					}
	    					if(emonth_tmp>=sRule[j].ru_sdate&&emonth_tmp<=sRule[j].ru_edate&&eno_flg==0){ 
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							et_flg=1;
		    						}else{}
	    					}
    				}else{}
    			}
    			
    		}
    		sno_flg=0;eno_flg=0;
    		sdate_tmp=0;sweek_tmp=0;smonth_tmp=0;
    		edate_tmp=0;eweek_tmp=0;emonth_tmp=0;
    		//循环表中记录时刻一次+30分
    		date_tmp=fflow[g].stime-utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
   			week_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
   			if(date_tmp%1800>=900){
    				fflow[g].stime=1800*(date_tmp/1800+1)+week_tmp;
    				st_depart=1800-date_tmp%1800;
    		}else{
    				fflow[g].stime-=date_tmp%1800;
    				st_depart=-date_tmp%1800;
    		}
    		date_tmp=0;week_tmp=0;
    		date_tmp=fflow[g].etime-utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		week_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		if(date_tmp%1800>=900){
    				fflow[g].etime=1800*(date_tmp/1800+1)+week_tmp;
    				ed_depart=-(1800-date_tmp%1800);		//网上约少算的时间
    		}else{
    				fflow[g].etime-=date_tmp%1800;		
    				ed_depart=date_tmp%1800;					//往下约多算
    		}
    		date_tmp=0;week_tmp=0;
    		
    		for(i=fflow[g].stime;i<fflow[g].etime;i+=1800){     
    			date_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",i));
    			week_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",i));
    			month_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",i));
    			for(j=0;j<iNum;j++){    //循环规则     --非优先级高
    				if(sRule[j].flags==1){			//非工作时间规则
	    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    						//先判断日期
	    						if(date_tmp>=sRule[j].ru_sdate&&date_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							break;
		    						}else{			//不在工作时间内执行下条规则(因为一次只加30分钟，所以只有在和不在两种结果)
		    						}
	    						}
    						}else if(sRule[j].timetype==2){
	    						if((week_tmp>sRule[j].ru_sdate&&week_tmp<sRule[j].ru_edate)||week_tmp==sRule[j].ru_edate||week_tmp==sRule[j].ru_sdate){
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				
		    							break;
		    						}else{
		    						}
	    						}
    						}else if(sRule[j].timetype==3){
    							if(month_tmp>=sRule[j].ru_sdate&&month_tmp<=sRule[j].ru_edate){
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				
		    							break;
		    						}else{
		    						}
	    						}
    						}else{}
    				}else if(sRule[j].flags==0){//工作时间规则
    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    					//先判断日期
	    					if(date_tmp>=sRule[j].ru_sdate&&date_tmp<=sRule[j].ru_edate){
	    					//在判断时刻  
	    					if(g<100&&fflow[g].userid==124&&ssstime==1274059039)
    			printf("=============cha=%d,ru_stime=%d,ru_etime=%d,i=%d,date_tmp=%d\n",i-date_tmp,sRule[j].ru_stime,sRule[j].ru_etime,i,date_tmp);
	    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							time_sum+=1800;
	    						}else{			//不在工作时间内执行下条规则
	    						}
	    					}
    					}else if(sRule[j].timetype==2){
    						if(week_tmp>=sRule[j].ru_sdate&&week_tmp<=sRule[j].ru_edate){  
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							time_sum+=1800;
		    						}else{
		    						}
	    					}
    					}else if(sRule[j].timetype==3){
    						if(month_tmp>=sRule[j].ru_sdate&&month_tmp<=sRule[j].ru_edate){ 
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							time_sum+=1800;
		    						}else{
		    						}
	    					}
    					}else{}
    				}
    			}
    			date_tmp=0;
    			week_tmp=0;
    			month_tmp=0;
    		}
    		
    		if(st_flg==1&&time_sum>0)
    			time_sum+=st_depart;
    		if(et_flg==1&&time_sum>0)
    			time_sum+=ed_depart;
    		
    		//if(fflow[g].userid==124&&ssstime==1273813945)
    		//	printf("error=%lu,cha2=%lu,lBytes=%llu,time_sum=%lu,userid=%lu,stime=%lu,etime=%lu,st_depart=%d,ed_depart=%d\n",eeetime-ssstime-time_sum,eeetime-ssstime,fflow[g].lBytes,time_sum,fflow[g].userid,ssstime,eeetime,st_depart,ed_depart);	
    		if(fflow[g].etime-fflow[g].stime>0&&eeetime-ssstime>0){
	    			tarFlow[r].lBytes=fflow[g].lBytes*(time_sum)/(eeetime-ssstime);
	    			tarFlow[r].userid=fflow[g].userid;
	    			r++;
	    			
    		}else if(fflow[g].etime-fflow[g].stime==0){
    				if((st_flg==1&&et_flg==1)){
    					tarFlow[r].lBytes=fflow[g].lBytes;
    					tarFlow[r].userid=fflow[g].userid;
	    				r++;
    				}else if((st_flg==1&&et_flg!=1)){
    					tarFlow[r].lBytes=fflow[g].lBytes*(st_depart)/(eeetime-ssstime);
    					tarFlow[r].userid=fflow[g].userid;
	    				r++;
    				}else if((st_flg!=1&&et_flg==1)){
    					tarFlow[r].lBytes=fflow[g].lBytes*(ed_depart)/(eeetime-ssstime);
    					tarFlow[r].userid=fflow[g].userid;
	    				r++;
    				}else{}
    				
	    			//if(tarFlow[r].lBytes>=10000000)
    			//printf("11=%lu,cha2=%lu,lBytes=%llu,time_sum=%lu,userid=%lu,stime=%lu,etime=%lu,st_depart=%d,ed_depart=%d,st_flg=%d,et_flg=%d\n",eeetime-ssstime-time_sum,eeetime-ssstime,fflow[g].lBytes,time_sum,fflow[g].userid,ssstime,eeetime,st_depart,ed_depart,st_flg,et_flg);
    		
    		}
	    	time_sum=0;
	    	st_depart=0;ed_depart=0;st_flg=0;et_flg=0;
	    	eeetime=0;ssstime=0;
	    	
	    	//printf("*****end*********g=%d,t=%d\n",g,t);
    }
   }else{//非工作时间统计
   		
    		
   	for(g=0;g<t;g++){
   		
   			ssstime=fflow[g].stime;
    		eeetime=fflow[g].etime;
    		sdate_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
    		sweek_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",fflow[g].stime));
    		smonth_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",fflow[g].stime));
    		edate_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		eweek_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",fflow[g].etime));
    		emonth_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",fflow[g].etime));
   			for(j=0;j<iNum;j++){    //循环规则     --非优先级高
    				
    				if(sRule[j].flags==1){			//非工作时间规则
	    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    						//先判断日期
	    						if(sdate_tmp>=sRule[j].ru_sdate&&sdate_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if(edate_tmp>=sRule[j].ru_sdate&&edate_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else if(sRule[j].timetype==2){
	    						if((sweek_tmp>sRule[j].ru_sdate&&sweek_tmp<sRule[j].ru_edate)||sweek_tmp==sRule[j].ru_edate||sweek_tmp==sRule[j].ru_sdate){
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if((eweek_tmp>sRule[j].ru_sdate&&eweek_tmp<sRule[j].ru_edate)||eweek_tmp==sRule[j].ru_edate||eweek_tmp==sRule[j].ru_sdate){
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else if(sRule[j].timetype==3){
    							if(smonth_tmp>=sRule[j].ru_sdate&&smonth_tmp<=sRule[j].ru_edate){
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if(emonth_tmp>=sRule[j].ru_sdate&&emonth_tmp<=sRule[j].ru_edate){
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else{}
    			}else if(sRule[j].flags==0){	
    				if(sRule[j].timetype==1||sRule[j].timetype==4){
	    					//先判断日期
	    					if(sdate_tmp>=sRule[j].ru_sdate&&sdate_tmp<=sRule[j].ru_edate&&sno_flg==0){
	    					//在判断时刻  
	    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							st_flg=1;
	    						}else{			//不在工作时间内执行下条规则
	    						}
	    					}
	    					if(edate_tmp>=sRule[j].ru_sdate&&edate_tmp<=sRule[j].ru_edate&&eno_flg==0){
	    					//在判断时刻  
	    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							et_flg=1;
	    						}else{}
	    					}
    				}else if(sRule[j].timetype==2){
    						if(sweek_tmp>=sRule[j].ru_sdate&&sweek_tmp<=sRule[j].ru_edate&&sno_flg==0){  
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							st_flg=1;
		    						}else{}
	    					}
	    					if(eweek_tmp>=sRule[j].ru_sdate&&eweek_tmp<=sRule[j].ru_edate&&eno_flg==0){  
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							et_flg=1;
		    						}else{}
	    					}
    				}else if(sRule[j].timetype==3){
    						if(smonth_tmp>=sRule[j].ru_sdate&&smonth_tmp<=sRule[j].ru_edate&&sno_flg==0){ 
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							st_flg=1;
		    						}else{}
	    					}
	    					if(emonth_tmp>=sRule[j].ru_sdate&&emonth_tmp<=sRule[j].ru_edate&&eno_flg==0){ 
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							et_flg=1;
		    						}else{}
	    					}
    				}else{}
    			}
    			
    			
    		}
    		sno_flg=0;eno_flg=0;
    		sdate_tmp=0;sweek_tmp=0;smonth_tmp=0;
    		edate_tmp=0;eweek_tmp=0;emonth_tmp=0;
    		
   			date_tmp=fflow[g].stime-utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
   			week_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
   			if(date_tmp%1800>=900){
    				fflow[g].stime=1800*(date_tmp/1800+1)+week_tmp;
    				st_depart=1800-date_tmp%1800;
    		}else{
    				fflow[g].stime-=date_tmp%1800;
    				st_depart=-date_tmp%1800;
    		}
    		date_tmp=0;week_tmp=0;
    		date_tmp=fflow[g].etime-utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		week_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		if(date_tmp%1800>=900){
    				fflow[g].etime=1800*(date_tmp/1800+1)+week_tmp;
    				ed_depart=-(1800-date_tmp%1800);
    		}else{
    				fflow[g].etime-=date_tmp%1800;
    				ed_depart=date_tmp%1800;	
    		}
    		date_tmp=0;week_tmp=0;
    		
    		
    		
    		for(i=fflow[g].stime;i<fflow[g].etime;i+=1800){     
    			date_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",i));
    			week_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",i));
    			month_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",i));
    			for(j=0;j<iNum;j++){    //循环规则     --非优先级高
    				if(sRule[j].flags==1){			//非工作时间规则
	    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    						//先判断日期
	    						if(date_tmp>=sRule[j].ru_sdate&&date_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							break;
		    						}else{			//不在工作时间内执行下条规则(因为一次只加30分钟，所以只有在和不在两种结果)
		    						}
	    						}
    						}else if(sRule[j].timetype==2){
	    						if((week_tmp>sRule[j].ru_sdate&&week_tmp<sRule[j].ru_edate)||week_tmp==sRule[j].ru_edate||week_tmp==sRule[j].ru_sdate){
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				
		    							break;
		    						}else{
		    						}
	    						}
    						}else if(sRule[j].timetype==3){
    							if(month_tmp>=sRule[j].ru_sdate&&month_tmp<=sRule[j].ru_edate){
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				
		    							break;
		    						}else{
		    						}
	    						}
    						}else{}
    				}else if(sRule[j].flags==0){//工作时间规则
    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    					//先判断日期
	    					if(date_tmp>=sRule[j].ru_sdate&&date_tmp<=sRule[j].ru_edate){
	    					//在判断时刻
	    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							time_sum+=1800;
	    						}else{			//不在工作时间内执行下条规则
	    						}
	    					}
    					}else if(sRule[j].timetype==2){
    						if(week_tmp>=sRule[j].ru_sdate&&week_tmp<=sRule[j].ru_edate){  
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							time_sum+=1800;;
		    						}else{
		    						}
	    					}
    					}else if(sRule[j].timetype==3){
    						if(month_tmp>=sRule[j].ru_sdate&&month_tmp<=sRule[j].ru_edate){ 
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							time_sum+=1800;
		    						}else{
		    						}
	    					}
    					}else{}
    				}
    			}
    			date_tmp=0;
    			week_tmp=0;
    			month_tmp=0;
    		}
    		//if(fflow[g].stime==1273208400&&fflow[g].userid==112)
    			//printf("st_depart=%d,ed_depart=%d,time_sum=%d,sssum=%d\n",st_depart,ed_depart,time_sum,time_sum+st_depart+ed_depart);
    		if(st_flg==1&&time_sum>0)
    			time_sum+=st_depart;
    		if(et_flg==1&&time_sum>0)
    			time_sum+=ed_depart;
    		if(fflow[g].etime-fflow[g].stime>0&&eeetime-ssstime>0){
    				if(eeetime-ssstime-time_sum>0)
	    				tarFlow[r].lBytes=fflow[g].lBytes*(eeetime-ssstime-time_sum)/(eeetime-ssstime);
	    			else
	    				tarFlow[r].lBytes=0;
	    			tarFlow[r].userid=fflow[g].userid;
	    			r++;
	    			
	    			//if(eeetime-ssstime-time_sum<0)
	    			//	printf("error=%lu,cha2=%lu,lBytes=%llu,time_sum=%lu,userid=%lu,stime=%lu,etime=%lu,st_depart=%d,ed_depart=%d\n",eeetime-ssstime-time_sum,eeetime-ssstime,fflow[g].lBytes,time_sum,fflow[g].userid,ssstime,eeetime,st_depart,ed_depart);
	    			
    		}else if(fflow[g].etime-fflow[g].stime==0){
    				if(st_flg==1&&et_flg!=1){					//开始一点落在工作时间里
	    				tarFlow[r].lBytes=fflow[g].lBytes*(eeetime-ssstime-st_depart)/(eeetime-ssstime);
	    				tarFlow[r].userid=fflow[g].userid;
	    				r++;
	    			}else if(st_flg!=1&&et_flg==1){		//结束一点落在工作时间里
	    				tarFlow[r].lBytes=fflow[g].lBytes*(eeetime-ssstime-ed_depart)/(eeetime-ssstime);
	    				tarFlow[r].userid=fflow[g].userid;
	    				r++;
	    			}else if(st_flg==0&&et_flg==0){   //全落在非工作时间里
	    				tarFlow[r].lBytes=fflow[g].lBytes;
	    				tarFlow[r].userid=fflow[g].userid;
	    				r++;
	    			}
	    					
    		}else{}
	    	time_sum=0;
	    	st_depart=0;ed_depart=0;st_flg=0;et_flg=0;
	    	eeetime=0;ssstime=0;
    }
   } 
    
    //566537M数据有1m误差
    /*
    double timeuse2=0;
    gettimeofday( &end1, NULL );
		timeuse2 = 1000000 * ( end1.tv_sec - start.tv_sec ) + end1.tv_usec - start.tv_usec; 
		timeuse2 /= 1000000;
		printf("=========0=============timeuse2=%.4f\n",timeuse2); 
    */
    long id_mi=0;
    j=0;
    printf("r=%d\n",r);
    sFlow=(struct flow_s*)malloc(sizeof(struct flow_s*)*(8000000));
    //qsort(tarFlow,r,sizeof(struct flow_tar),ncUtlSortOn_userId); 
    for(i=0;i<=r;i++){
    	if(i==0){  
    		id_mi=tarFlow[i].userid;  
    		sFlow[j].userid=tarFlow[i].userid; 
    		sFlow[j].lBytes=tarFlow[i].lBytes;  
    	}else if(i>0&&id_mi==tarFlow[i].userid){
    		sFlow[j].lBytes+=tarFlow[i].lBytes;
    		//printf("=mi==lBytes=%llu,sFlow=%llu\n",tarFlow[i].lBytes,sFlow[j].lBytes);
    	}else if(i>0&&id_mi!=tarFlow[i].userid){
    		id_mi=tarFlow[i].userid;
    		j++;
    		sFlow[j].userid=tarFlow[i].userid; 
    		sFlow[j].lBytes=tarFlow[i].lBytes; 
    	}else{}
    }
    //for(i=0;i<j;i++){
    	//printf("=end==lBytes=%llu,userid=%d\n",sFlow[i].lBytes,sFlow[i].userid);
    //}
    printf("j=%d\n",j);
    qsort(sFlow,j,sizeof(struct flow_s),ncUtlSortOnId); 
    for(i=0;i<10;i++){
    		endFlow[i].userid=sFlow[i].userid;
    		endFlow[i].lBytes=sFlow[i].lBytes;
    	//printf("lBytes=%llu,userid=%d\n",endFlow[i].lBytes,endFlow[i].userid);
    }
    if(j>10) j=10;
    /*
    double timeuse1=0;
    gettimeofday( &end, NULL );
		timeuse1 = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
		timeuse1 /= 1000000;
		printf("=========1=============timeuse1=%.4f\n",timeuse1); */
     //非工作时间     
    
    return j;
    
}
struct piechart_dataset{
	char *name;
	unsigned long  amount;
};
int ncUtlWorkTime_ta_ru_pdf(char *field,char *field2,struct flow_she *fflow,int t,int lCount,struct piechart_dataset *endFlow,char *caTime,utShmHead *psShmHead)
{		struct timeval start,end,end1;
	  
	  static char caSql[2048];
	  char caTemp[2048];
	   pasDbCursor *psCur;
	   long iSum;
    char *p;
    long lTimetype;
    long lLower,lUpper,lStarttime,lStoptime;
    long iReturn,g=0,r=0,i=0,j=0,k=0,iNum=0;
    char lLower_ch[20];
    char lUpper_ch[20];
    char stime_ch[20],etime_ch[20],stime_final[32],etime_final[32];
    char ta_sdate[20],ta_stime[20],ta_edate[20],ta_etime[20];
    int ta_days=0;
    long time_sum=0,be_time=0,ru_stime,ru_etime,ta_stime_lo,ta_etime_lo;
    char stime_rule[32],etime_rule[32];
    struct flow_tar{
				  	long userid;
				  	long long lBytes;
		};
		struct flow_tar *tarFlow; 
		struct flow_s{
	  	long userid;
	  	long long lBytes;
		};
		struct flow_s *sFlow;
    long wkcount=0;
    
    struct rule_s{
	  	long timetype;
	  	long lLower;
	  	long lUpper;
	  	long lStarttime;
	  	long lStoptime;
	  	long flags;
	  	long ru_sdate;
	  	long ru_stime;
	  	long ru_edate;
	  	long ru_etime;
		};
		struct rule_s *sRule;
		long flags=0;
		char stime_for[20]="",etime_for[20]="";
		long date_tmp=0,week_tmp=0,month_tmp=0;
		long st_depart=0,ed_depart=0,st_flg=0,et_flg=0,sno_flg=0,eno_flg=0;
		
		long sdate_tmp=0,sweek_tmp=0,smonth_tmp=0;
    long edate_tmp=0,eweek_tmp=0,emonth_tmp=0;
		long ssstime=0,eeetime=0;
		ncUserCont *psUser;
		
		//printf("week=%d,month=%d\n",ncUtlGetWeekNum("2010-11-23"),ncUtlGetMonthNum("2010-11-23"));
		//printf("27week=%d,month=%d\n",ncUtlGetWeekNum("2010-11-27"),ncUtlGetMonthNum("2010-11-27"));  //6   27
		//printf("28week=%d,month=%d\n",ncUtlGetWeekNum("2010-11-28"),ncUtlGetMonthNum("2010-11-28"));  //0   28
    sprintf(caTemp,"select count(*) from ncworktime ");
	  iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&wkcount);
	  sRule=(struct rule_s *)malloc(sizeof(struct rule_s)*(wkcount+1));
    psCur=pasDbOpenSql("select timetype,llower,lupper,starttime,stoptime,flags from ncworktime order by flags desc,timetype asc,llower asc,starttime asc ",0);
    if(psCur==NULL){
        return NULL;
    }
    lTimetype=0;lLower=0;lUpper=0;lStarttime=0;lStoptime=0;iSum=0;flags=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
                                   UT_TYPE_LONG,4,&lLower,
                                   UT_TYPE_LONG,4,&lUpper,
                                   UT_TYPE_LONG,4,&lStarttime,
                                   UT_TYPE_LONG,4,&lStoptime,
                                   UT_TYPE_LONG,4,&flags);
    iNum=0;
    while(iReturn==0){
			sRule[iNum].timetype=lTimetype;
			sRule[iNum].lLower=lLower;
			sRule[iNum].lUpper=lUpper;
			sRule[iNum].lStarttime=lStarttime;
			sRule[iNum].lStoptime=lStoptime;
			sRule[iNum].flags=flags;
			sprintf(lLower_ch,"%d",lLower);       
			sprintf(lUpper_ch,"%d",lUpper); 
			if(lTimetype==1){
				sprintf(stime_rule,"%d%06d",lLower,lStarttime);
				sprintf(etime_rule,"%d%06d",lUpper,lStoptime);
			}else if(lTimetype==2||lTimetype==3||lTimetype==4){
				sprintf(stime_rule,"20101101%06d",lStarttime);
				sprintf(etime_rule,"20101101%06d",lStoptime);
			}
			//ru_stime=utTimStrToLong("%Y%m%d%H%M%S",stime_rule)-utTimStrToLong("%Y%m%d",lLower_ch);
			//ru_etime=utTimStrToLong("%Y%m%d%H%M%S",etime_rule)-utTimStrToLong("%Y%m%d",lUpper_ch);
			if(lTimetype==1){
				sRule[iNum].ru_sdate=utTimStrToLong("%Y%m%d",lLower_ch);
				sRule[iNum].ru_stime=utTimStrToLong("%Y%m%d%H%M%S",stime_rule)-utTimStrToLong("%Y%m%d",lLower_ch);				
				sRule[iNum].ru_edate=utTimStrToLong("%Y%m%d",lUpper_ch);
				sRule[iNum].ru_etime=utTimStrToLong("%Y%m%d%H%M%S",etime_rule)-utTimStrToLong("%Y%m%d",lUpper_ch);
			}else if(lTimetype==2){
				sRule[iNum].ru_sdate=lLower;
				sRule[iNum].ru_stime=utTimStrToLong("%Y%m%d%H%M%S",stime_rule)-utTimStrToLong("%Y%m%d","20101101");
				
				sRule[iNum].ru_edate=lUpper;
				sRule[iNum].ru_etime=utTimStrToLong("%Y%m%d%H%M%S",etime_rule)-utTimStrToLong("%Y%m%d","20101101");
			}else if(lTimetype==3){
				sRule[iNum].ru_sdate=lLower;
				sRule[iNum].ru_stime=utTimStrToLong("%Y%m%d%H%M%S",stime_rule)-utTimStrToLong("%Y%m%d","20101101");
				sRule[iNum].ru_edate=lUpper;
				sRule[iNum].ru_etime=utTimStrToLong("%Y%m%d%H%M%S",etime_rule)-utTimStrToLong("%Y%m%d","20101101");
			}else if(lTimetype==4){
				sprintf(stime_for,"%s%04d",utTimFormat("%Y",time(0)),lLower);
				sprintf(etime_for,"%s%04d",utTimFormat("%Y",time(0)),lUpper);
				sRule[iNum].ru_sdate=utTimStrToLong("%Y%m%d",stime_for);
				sRule[iNum].ru_stime=utTimStrToLong("%Y%m%d%H%M%S",stime_rule)-utTimStrToLong("%Y%m%d","20101101");
				sRule[iNum].ru_edate=utTimStrToLong("%Y%m%d",etime_for);
				sRule[iNum].ru_etime=utTimStrToLong("%Y%m%d%H%M%S",etime_rule)-utTimStrToLong("%Y%m%d","20101101");
				memset(stime_for,0,sizeof(stime_for));
				memset(etime_for,0,sizeof(etime_for));
			}
			iNum++;
			memset(lLower_ch,0,sizeof(lLower_ch));
			memset(lUpper_ch,0,sizeof(lUpper_ch));
			memset(stime_rule,0,sizeof(stime_rule));
			memset(etime_rule,0,sizeof(etime_rule));
    	lTimetype=0;lLower=0;lUpper=0;lStarttime=0;lStoptime=0;flags=0;
    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
			                               UT_TYPE_LONG,4,&lLower,
			                               UT_TYPE_LONG,4,&lUpper,
			                               UT_TYPE_LONG,4,&lStarttime,
	                                   UT_TYPE_LONG,4,&lStoptime,
	                                   UT_TYPE_LONG,4,&flags);
    }
    pasDbCloseCursor(psCur);
    
    //gettimeofday( &start, NULL ); 
    tarFlow=(struct flow_tar*)malloc(sizeof(struct flow_tar)*(lCount+1));     
   	r=0;
   	time_sum=0;
   if(strcasecmp(caTime,"worktime") == 0) {	
    for(g=0;g<t;g++){
    		//先判断，开始和结束是否在统计时刻内，若在修改标志位 
    		ssstime=fflow[g].stime;
    		eeetime=fflow[g].etime;
    		sdate_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
    		sweek_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",fflow[g].stime));
    		smonth_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",fflow[g].stime));
    		edate_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		eweek_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",fflow[g].etime));
    		emonth_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",fflow[g].etime));
    		for(j=0;j<iNum;j++){    //循环规则     --非优先级高
    				
    			if(sRule[j].flags==1){			//非工作时间规则
	    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    						//先判断日期
	    						if(sdate_tmp>=sRule[j].ru_sdate&&sdate_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if(edate_tmp>=sRule[j].ru_sdate&&edate_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else if(sRule[j].timetype==2){
	    						if((sweek_tmp>sRule[j].ru_sdate&&sweek_tmp<sRule[j].ru_edate)||sweek_tmp==sRule[j].ru_edate||sweek_tmp==sRule[j].ru_sdate){
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if((eweek_tmp>sRule[j].ru_sdate&&eweek_tmp<sRule[j].ru_edate)||eweek_tmp==sRule[j].ru_edate||eweek_tmp==sRule[j].ru_sdate){
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else if(sRule[j].timetype==3){
    							if(smonth_tmp>=sRule[j].ru_sdate&&smonth_tmp<=sRule[j].ru_edate){
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if(emonth_tmp>=sRule[j].ru_sdate&&emonth_tmp<=sRule[j].ru_edate){
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else{}
    			}else if(sRule[j].flags==0){	
    				if(sRule[j].timetype==1||sRule[j].timetype==4){
	    					//先判断日期
	    					if(sdate_tmp>=sRule[j].ru_sdate&&sdate_tmp<=sRule[j].ru_edate&&sno_flg==0){
	    					//在判断时刻  
	    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							st_flg=1;
	    						}else{			//不在工作时间内执行下条规则
	    						}
	    					}
	    					if(edate_tmp>=sRule[j].ru_sdate&&edate_tmp<=sRule[j].ru_edate&&eno_flg==0){
	    					//在判断时刻  
	    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							et_flg=1;
	    						}else{}
	    					}
    				}else if(sRule[j].timetype==2){
    						if(sweek_tmp>=sRule[j].ru_sdate&&sweek_tmp<=sRule[j].ru_edate&&sno_flg==0){  
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							st_flg=1;
		    						}else{}
	    					}
	    					if(eweek_tmp>=sRule[j].ru_sdate&&eweek_tmp<=sRule[j].ru_edate&&eno_flg==0){  
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							et_flg=1;
		    						}else{}
	    					}
    				}else if(sRule[j].timetype==3){
    						if(smonth_tmp>=sRule[j].ru_sdate&&smonth_tmp<=sRule[j].ru_edate&&sno_flg==0){ 
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							st_flg=1;
		    						}else{}
	    					}
	    					if(emonth_tmp>=sRule[j].ru_sdate&&emonth_tmp<=sRule[j].ru_edate&&eno_flg==0){ 
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							et_flg=1;
		    						}else{}
	    					}
    				}else{}
    			}
    			
    		}
    		//if(g==2000) printf("pas\n");
    		sno_flg=0;eno_flg=0;
    		sdate_tmp=0;sweek_tmp=0;smonth_tmp=0;
    		edate_tmp=0;eweek_tmp=0;emonth_tmp=0;
    		//循环表中记录时刻一次+30分
    		date_tmp=fflow[g].stime-utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
   			week_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
   			if(date_tmp%1800>=900){
    				fflow[g].stime=1800*(date_tmp/1800+1)+week_tmp;
    				st_depart=1800-date_tmp%1800;
    		}else{
    				fflow[g].stime-=date_tmp%1800;
    				st_depart=-date_tmp%1800;
    		}
    		date_tmp=0;week_tmp=0;
    		date_tmp=fflow[g].etime-utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		week_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		if(date_tmp%1800>=900){
    				fflow[g].etime=1800*(date_tmp/1800+1)+week_tmp;
    				ed_depart=-(1800-date_tmp%1800);		//网上约少算的时间
    		}else{
    				fflow[g].etime-=date_tmp%1800;		
    				ed_depart=date_tmp%1800;					//往下约多算
    		}
    		date_tmp=0;week_tmp=0;
    		
    		for(i=fflow[g].stime;i<fflow[g].etime;i+=1800){     
    			date_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",i));
    			week_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",i));
    			month_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",i));
    			for(j=0;j<iNum;j++){    //循环规则     --非优先级高
    				if(sRule[j].flags==1){			//非工作时间规则
	    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    						//先判断日期
	    						if(date_tmp>=sRule[j].ru_sdate&&date_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							break;
		    						}else{			//不在工作时间内执行下条规则(因为一次只加30分钟，所以只有在和不在两种结果)
		    						}
	    						}
    						}else if(sRule[j].timetype==2){
	    						if((week_tmp>sRule[j].ru_sdate&&week_tmp<sRule[j].ru_edate)||week_tmp==sRule[j].ru_edate||week_tmp==sRule[j].ru_sdate){
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				
		    							break;
		    						}else{
		    						}
	    						}
    						}else if(sRule[j].timetype==3){
    							if(month_tmp>=sRule[j].ru_sdate&&month_tmp<=sRule[j].ru_edate){
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				
		    							break;
		    						}else{
		    						}
	    						}
    						}else{}
    				}else if(sRule[j].flags==0){//工作时间规则
    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    					//先判断日期
	    					if(date_tmp>=sRule[j].ru_sdate&&date_tmp<=sRule[j].ru_edate){
	    					//在判断时刻  
	    					if(g<100&&fflow[g].userid==124&&ssstime==1274059039)
    			printf("=============cha=%d,ru_stime=%d,ru_etime=%d,i=%d,date_tmp=%d\n",i-date_tmp,sRule[j].ru_stime,sRule[j].ru_etime,i,date_tmp);
	    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							time_sum+=1800;
	    						}else{			//不在工作时间内执行下条规则
	    						}
	    					}
    					}else if(sRule[j].timetype==2){
    						if(week_tmp>=sRule[j].ru_sdate&&week_tmp<=sRule[j].ru_edate){  
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							time_sum+=1800;
		    						}else{
		    						}
	    					}
    					}else if(sRule[j].timetype==3){
    						if(month_tmp>=sRule[j].ru_sdate&&month_tmp<=sRule[j].ru_edate){ 
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							time_sum+=1800;
		    						}else{
		    						}
	    					}
    					}else{}
    				}
    			}
    			date_tmp=0;
    			week_tmp=0;
    			month_tmp=0;
    		}
    		
    		if(st_flg==1&&time_sum>0)
    			time_sum+=st_depart;
    		if(et_flg==1&&time_sum>0)
    			time_sum+=ed_depart;
    		
    		//if(fflow[g].userid==124&&ssstime==1273813945)
    		//	printf("error=%lu,cha2=%lu,lBytes=%llu,time_sum=%lu,userid=%lu,stime=%lu,etime=%lu,st_depart=%d,ed_depart=%d\n",eeetime-ssstime-time_sum,eeetime-ssstime,fflow[g].lBytes,time_sum,fflow[g].userid,ssstime,eeetime,st_depart,ed_depart);	
    		if(fflow[g].etime-fflow[g].stime>0&&eeetime-ssstime>0){
	    			tarFlow[r].lBytes=fflow[g].lBytes*(time_sum)/(eeetime-ssstime);
	    			tarFlow[r].userid=fflow[g].userid;
	    			r++;
	    			
    		}else if(fflow[g].etime-fflow[g].stime==0){
    				if((st_flg==1&&et_flg==1)){
    					tarFlow[r].lBytes=fflow[g].lBytes;
    					tarFlow[r].userid=fflow[g].userid;
	    				r++;
    				}else if((st_flg==1&&et_flg!=1)){
    					tarFlow[r].lBytes=fflow[g].lBytes*(st_depart)/(eeetime-ssstime);
    					tarFlow[r].userid=fflow[g].userid;
	    				r++;
    				}else if((st_flg!=1&&et_flg==1)){
    					tarFlow[r].lBytes=fflow[g].lBytes*(ed_depart)/(eeetime-ssstime);
    					tarFlow[r].userid=fflow[g].userid;
	    				r++;
    				}else{}
    				
	    			//if(tarFlow[r].lBytes>=10000000)
    			//printf("11=%lu,cha2=%lu,lBytes=%llu,time_sum=%lu,userid=%lu,stime=%lu,etime=%lu,st_depart=%d,ed_depart=%d,st_flg=%d,et_flg=%d\n",eeetime-ssstime-time_sum,eeetime-ssstime,fflow[g].lBytes,time_sum,fflow[g].userid,ssstime,eeetime,st_depart,ed_depart,st_flg,et_flg);
    		
    		}
	    	time_sum=0;
	    	st_depart=0;ed_depart=0;st_flg=0;et_flg=0;
	    	eeetime=0;ssstime=0;
	    	
	    	//printf("*****end*********g=%d,t=%d\n",g,t);
    }
   }else{//非工作时间统计
   		
    		
   	for(g=0;g<t;g++){
   		
   			ssstime=fflow[g].stime;
    		eeetime=fflow[g].etime;
    		sdate_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
    		sweek_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",fflow[g].stime));
    		smonth_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",fflow[g].stime));
    		edate_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		eweek_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",fflow[g].etime));
    		emonth_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",fflow[g].etime));
   			for(j=0;j<iNum;j++){    //循环规则     --非优先级高
    				
    				if(sRule[j].flags==1){			//非工作时间规则
	    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    						//先判断日期
	    						if(sdate_tmp>=sRule[j].ru_sdate&&sdate_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if(edate_tmp>=sRule[j].ru_sdate&&edate_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else if(sRule[j].timetype==2){
	    						if((sweek_tmp>sRule[j].ru_sdate&&sweek_tmp<sRule[j].ru_edate)||sweek_tmp==sRule[j].ru_edate||sweek_tmp==sRule[j].ru_sdate){
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if((eweek_tmp>sRule[j].ru_sdate&&eweek_tmp<sRule[j].ru_edate)||eweek_tmp==sRule[j].ru_edate||eweek_tmp==sRule[j].ru_sdate){
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else if(sRule[j].timetype==3){
    							if(smonth_tmp>=sRule[j].ru_sdate&&smonth_tmp<=sRule[j].ru_edate){
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				
		    							sno_flg=1;
		    						}else{}
	    						}
	    						if(emonth_tmp>=sRule[j].ru_sdate&&emonth_tmp<=sRule[j].ru_edate){
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				
		    							eno_flg=1;
		    						}else{}
	    						}
    						}else{}
    			}else if(sRule[j].flags==0){	
    				if(sRule[j].timetype==1||sRule[j].timetype==4){
	    					//先判断日期
	    					if(sdate_tmp>=sRule[j].ru_sdate&&sdate_tmp<=sRule[j].ru_edate&&sno_flg==0){
	    					//在判断时刻  
	    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							st_flg=1;
	    						}else{			//不在工作时间内执行下条规则
	    						}
	    					}
	    					if(edate_tmp>=sRule[j].ru_sdate&&edate_tmp<=sRule[j].ru_edate&&eno_flg==0){
	    					//在判断时刻  
	    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							et_flg=1;
	    						}else{}
	    					}
    				}else if(sRule[j].timetype==2){
    						if(sweek_tmp>=sRule[j].ru_sdate&&sweek_tmp<=sRule[j].ru_edate&&sno_flg==0){  
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							st_flg=1;
		    						}else{}
	    					}
	    					if(eweek_tmp>=sRule[j].ru_sdate&&eweek_tmp<=sRule[j].ru_edate&&eno_flg==0){  
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							et_flg=1;
		    						}else{}
	    					}
    				}else if(sRule[j].timetype==3){
    						if(smonth_tmp>=sRule[j].ru_sdate&&smonth_tmp<=sRule[j].ru_edate&&sno_flg==0){ 
		    						if((fflow[g].stime-sdate_tmp)>=sRule[j].ru_stime&&(fflow[g].stime-sdate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							st_flg=1;
		    						}else{}
	    					}
	    					if(emonth_tmp>=sRule[j].ru_sdate&&emonth_tmp<=sRule[j].ru_edate&&eno_flg==0){ 
		    						if((fflow[g].etime-edate_tmp)>=sRule[j].ru_stime&&(fflow[g].etime-edate_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							et_flg=1;
		    						}else{}
	    					}
    				}else{}
    			}
    			
    			
    		}
    		sno_flg=0;eno_flg=0;
    		sdate_tmp=0;sweek_tmp=0;smonth_tmp=0;
    		edate_tmp=0;eweek_tmp=0;emonth_tmp=0;
    		
   			date_tmp=fflow[g].stime-utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
   			week_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].stime));
   			if(date_tmp%1800>=900){
    				fflow[g].stime=1800*(date_tmp/1800+1)+week_tmp;
    				st_depart=1800-date_tmp%1800;
    		}else{
    				fflow[g].stime-=date_tmp%1800;
    				st_depart=-date_tmp%1800;
    		}
    		date_tmp=0;week_tmp=0;
    		date_tmp=fflow[g].etime-utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		week_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",fflow[g].etime));
    		if(date_tmp%1800>=900){
    				fflow[g].etime=1800*(date_tmp/1800+1)+week_tmp;
    				ed_depart=-(1800-date_tmp%1800);
    		}else{
    				fflow[g].etime-=date_tmp%1800;
    				ed_depart=date_tmp%1800;	
    		}
    		date_tmp=0;week_tmp=0;
    		
    		
    		
    		for(i=fflow[g].stime;i<fflow[g].etime;i+=1800){     
    			date_tmp=utTimStrToLong("%Y%m%d",utTimFormat("%Y%m%d",i));
    			week_tmp=ncUtlGetWeekNum(utTimFormat("%Y/%m/%d",i));
    			month_tmp=ncUtlGetMonthNum(utTimFormat("%Y/%m/%d",i));
    			for(j=0;j<iNum;j++){    //循环规则     --非优先级高
    				if(sRule[j].flags==1){			//非工作时间规则
	    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    						//先判断日期
	    						if(date_tmp>=sRule[j].ru_sdate&&date_tmp<=sRule[j].ru_edate){
		    					//在判断时刻  
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							break;
		    						}else{			//不在工作时间内执行下条规则(因为一次只加30分钟，所以只有在和不在两种结果)
		    						}
	    						}
    						}else if(sRule[j].timetype==2){
	    						if((week_tmp>sRule[j].ru_sdate&&week_tmp<sRule[j].ru_edate)||week_tmp==sRule[j].ru_edate||week_tmp==sRule[j].ru_sdate){
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				
		    							break;
		    						}else{
		    						}
	    						}
    						}else if(sRule[j].timetype==3){
    							if(month_tmp>=sRule[j].ru_sdate&&month_tmp<=sRule[j].ru_edate){
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<=sRule[j].ru_etime){				
		    							break;
		    						}else{
		    						}
	    						}
    						}else{}
    				}else if(sRule[j].flags==0){//工作时间规则
    					if(sRule[j].timetype==1||sRule[j].timetype==4){
	    					//先判断日期
	    					if(date_tmp>=sRule[j].ru_sdate&&date_tmp<=sRule[j].ru_edate){
	    					//在判断时刻
	    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
	    							time_sum+=1800;
	    						}else{			//不在工作时间内执行下条规则
	    						}
	    					}
    					}else if(sRule[j].timetype==2){
    						if(week_tmp>=sRule[j].ru_sdate&&week_tmp<=sRule[j].ru_edate){  
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							time_sum+=1800;;
		    						}else{
		    						}
	    					}
    					}else if(sRule[j].timetype==3){
    						if(month_tmp>=sRule[j].ru_sdate&&month_tmp<=sRule[j].ru_edate){ 
		    						if((i-date_tmp)>=sRule[j].ru_stime&&(i-date_tmp)<sRule[j].ru_etime){				// 在非工作时间内break; 表记录时间+1800秒往后循环
		    							time_sum+=1800;
		    						}else{
		    						}
	    					}
    					}else{}
    				}
    			}
    			date_tmp=0;
    			week_tmp=0;
    			month_tmp=0;
    		}
    		//if(fflow[g].stime==1273208400&&fflow[g].userid==112)
    			//printf("st_depart=%d,ed_depart=%d,time_sum=%d,sssum=%d\n",st_depart,ed_depart,time_sum,time_sum+st_depart+ed_depart);
    		if(st_flg==1&&time_sum>0)
    			time_sum+=st_depart;
    		if(et_flg==1&&time_sum>0)
    			time_sum+=ed_depart;
    		if(fflow[g].etime-fflow[g].stime>0&&eeetime-ssstime>0){
    				if(eeetime-ssstime-time_sum>0)
	    				tarFlow[r].lBytes=fflow[g].lBytes*(eeetime-ssstime-time_sum)/(eeetime-ssstime);
	    			else
	    				tarFlow[r].lBytes=0;
	    			tarFlow[r].userid=fflow[g].userid;
	    			r++;
	    			
	    			//if(eeetime-ssstime-time_sum<0)
	    			//	printf("error=%lu,cha2=%lu,lBytes=%llu,time_sum=%lu,userid=%lu,stime=%lu,etime=%lu,st_depart=%d,ed_depart=%d\n",eeetime-ssstime-time_sum,eeetime-ssstime,fflow[g].lBytes,time_sum,fflow[g].userid,ssstime,eeetime,st_depart,ed_depart);
	    			
    		}else if(fflow[g].etime-fflow[g].stime==0){
    				if(st_flg==1&&et_flg!=1){					//开始一点落在工作时间里
	    				tarFlow[r].lBytes=fflow[g].lBytes*(eeetime-ssstime-st_depart)/(eeetime-ssstime);
	    				tarFlow[r].userid=fflow[g].userid;
	    				r++;
	    			}else if(st_flg!=1&&et_flg==1){		//结束一点落在工作时间里
	    				tarFlow[r].lBytes=fflow[g].lBytes*(eeetime-ssstime-ed_depart)/(eeetime-ssstime);
	    				tarFlow[r].userid=fflow[g].userid;
	    				r++;
	    			}else if(st_flg==0&&et_flg==0){   //全落在非工作时间里
	    				tarFlow[r].lBytes=fflow[g].lBytes;
	    				tarFlow[r].userid=fflow[g].userid;
	    				r++;
	    			}
	    					
    		}else{}
	    	time_sum=0;
	    	st_depart=0;ed_depart=0;st_flg=0;et_flg=0;
	    	eeetime=0;ssstime=0;
    }
   } 
    /*
    //566537M数据有1m误差
    double timeuse2=0;
    gettimeofday( &end1, NULL );
		timeuse2 = 1000000 * ( end1.tv_sec - start.tv_sec ) + end1.tv_usec - start.tv_usec; 
		timeuse2 /= 1000000;
		printf("=========0=============timeuse2=%.4f\n",timeuse2); 
    */
    long id_mi=0;
    j=0;
    printf("r=%d\n",r);
    sFlow=(struct flow_s*)malloc(sizeof(struct flow_s*)*(8000000));
    //qsort(tarFlow,r,sizeof(struct flow_tar),ncUtlSortOn_userId); 
    for(i=0;i<=r;i++){
    	if(i==0){  
    		id_mi=tarFlow[i].userid;  
    		sFlow[j].userid=tarFlow[i].userid; 
    		sFlow[j].lBytes=tarFlow[i].lBytes;  
    	}else if(i>0&&id_mi==tarFlow[i].userid){
    		sFlow[j].lBytes+=tarFlow[i].lBytes;
    		//printf("=mi==lBytes=%llu,sFlow=%llu\n",tarFlow[i].lBytes,sFlow[j].lBytes);
    	}else if(i>0&&id_mi!=tarFlow[i].userid){
    		id_mi=tarFlow[i].userid;
    		j++;
    		sFlow[j].userid=tarFlow[i].userid; 
    		sFlow[j].lBytes=tarFlow[i].lBytes; 
    	}else{}
    }
    for(i=0;i<j;i++){
    	//printf("=end==lBytes=%llu,userid=%d\n",sFlow[i].lBytes,sFlow[i].userid);
    }
    printf("j=%d\n",j);
    qsort(sFlow,j,sizeof(struct flow_s),ncUtlSortOnId); 
    for(i=0;i<10;i++){
    		psUser = ncUtlGetUserContById(psShmHead,sFlow[i].userid);
	  		if(psUser){
	  			endFlow[i].name=strdup(psUser->dispname);
        }
    		endFlow[i].amount=sFlow[i].lBytes/1024;
    		
    	//printf("lBytes=%lu,name=%s\n",endFlow[i].amount,endFlow[i].name);
    }
    if(j>10) j=10;
    /*
    double timeuse1=0;
    gettimeofday( &end, NULL );
		timeuse1 = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec; 
		timeuse1 /= 1000000;
		printf("=========1=============timeuse1=%.4f\n",timeuse1); */
     //非工作时间     
    
    return j;
    
}

/* 上网行为分析报告  */
int ncWebAllReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  utPltDbHead *psDbHead;
	  struct service_s{            
	  	long long lBytes;
	  	char name[32];
	  	long service;
	  };
	  struct service_s stService[32];                                         //存放网络服务流量
	  char caUserid[16],caGroupid[16],caSdate[16],caEdate[16];
	  char caSdate_temp[20],caEdate_temp[20];
	  long lStime,lEtime;
	  char caTemp[9024];
	  long lTime,i,lSumservice,iReturn;
	  long long lBytes,lSumbytes;
	  char caName[32];
	  
	  struct flow_s{
	  	long userid;
	  	long long lBytes;
	  };
	  struct flow_s stFlow[32];                                            //存放上网流量最大用户
	  
	  
	  struct web_s{
	  	char name[32];                        //网址类别名称
	  	long tid;                             //网址类别ID
	  	long lcount;                         // 请求数
	  };
	  struct web_s stWeb[32];   
	  long lSumweb,lCount,lTid;
	  long lSumflow;
	  long lUserid;
	  long lSumwebcount,lSumwebother;
	  
	  struct website_s{
	  	char host[64];                        //网址名称
	  	long long bytes;                     //流量
	  	long lcount;                         // 请求数
	  };
	  struct website_s stWebsite[32];   
	  char caHost[64];
	  long lSumsite;
	  
	  
	 struct im_s{
	  	char dispname[32];                    //聊天记录           上网用户
	  	long userid;                          //用户名
	  	long lcount;                         // 记录数
	  };
	  struct im_s stIm[32];   
	  char caDispname[32];
	  long lSumIm;
	  
	 struct mail_s{
	  	char dispname[32];                    //邮件记录           上网用户
	  	long userid;                          //用户名
	  	long lcount;                         // 记录数
	  };
	  struct mail_s stMail[32];   

	  long lSumMail;
	  
	  char caTitlename[32];
	  long lService;
	  ncUserCont *psUser;
    pasDbCursor *psCur;
    char caGroupids[4096];
    long lGroupid,iNum;
	  psDbHead = utPltInitDb(); 
	  char caTime[32];
//统计网络服务	  
   utMsgGetSomeNVar(psMsgHead,5,"userid",UT_TYPE_STRING,10,caUserid,					  //统计项目
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//起始时间日期
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "groupid",UT_TYPE_STRING,10,caGroupid,
                                    "time",       UT_TYPE_STRING, 31, caTime);
                                    
                                    
 //拼groupid下用户
 memset(caGroupids,0,sizeof(caGroupids));
 
 if(strlen(caGroupid)>0){
 	if(atol(caGroupid)>0){
               sprintf(caTemp,"select userid from ncuser a,ncgroup b  where a.groupid=b.groupid and (b.groupid=%s or b.groupid in (select groupid from ncgroup where pid=%s )) limit 0,200",caGroupid,caGroupid);
            }
            else{
            	 sprintf(caTemp,"select userid from ncuser a,ncgroup b  where a.groupid=b.groupid and b.groupid=0 ");
            }
 //    printf("caTemp=%s\n",caTemp);             
  psCur=pasDbOpenSql(caTemp,0);
  if(psCur!=NULL) {   
  	    lGroupid=0;
  	   iReturn = pasDbFetchInto(psCur,
                                      UT_TYPE_LONG,4,&lGroupid);
       iNum=0;
       while(iReturn==0||iReturn==1405){
       	if(iNum>0){
       		sprintf(caGroupids+strlen(caGroupids),",%d",lGroupid);
       	}
       	else{
       		sprintf(caGroupids+strlen(caGroupids),"%d",lGroupid);
       	}
       	iNum++;
  	    lGroupid=0;
  	    iReturn = pasDbFetchInto(psCur,
                                      UT_TYPE_LONG,4,&lGroupid);
       }                               
      pasDbCloseCursor(psCur); 
  	                       
                  }
   if(strlen(caGroupids)==0){
   	strcpy(caGroupids,"-1");
    }                  
  }                  
   
 //  printf("caGroups=%s\n",caGroupids);                                 
utPltPutVar(psDbHead,"time",caTime);
memset(caTitlename,0,sizeof(caTitlename));
if(strlen(caGroupid)>0){
	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));
}
if(strlen(caUserid)>0){
	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
}
if(strlen(caTitlename)==0){
	strcpy(caTitlename,utComGetVar_sd(psShmHead,"rootname",ncLang("0269公司")));
}
if((strlen(caSdate)==0)||strlen(caEdate)==0){
	lTime=time(0);
	ncUtlWeekDate(lTime,caSdate,caEdate);
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
	
	utPltPutVar(psDbHead,"sdate",caSdate);
	utPltPutVar(psDbHead,"edate",caEdate);
	utPltPutVar(psDbHead,"userid",caUserid);
	utPltPutVar(psDbHead,"groupid",caGroupid);
	if(strlen(caTitlename)>0){
		utPltPutVar(psDbHead,"dispname",caTitlename);
	}
	else{
		utPltPutVar(psDbHead,"dispname",ncLang("0269公司"));
	}
	
	//统计网络服务
	sprintf(caTemp,"select sum(a.ubytes+a.dbytes) bb,c.name,b.groupid from ncipservice a,ncipindex b,ncservicecgroup c where  a.service=b.sid and b.groupid=c.groupid ");
//	sprintf(caTemp,"select sum(a.ubytes+a.dbytes) bb,c.name,b.groupid from ncipservice a,ncuser d,ncipindex b,ncservicecgroup c where a.userid=d.userid and a.service=b.sid and b.groupid=c.groupid ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
	if(strlen(caGroupid)>0){
		sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caGroupids);
//		if(atol(caGroupid)==0){
//			sprintf(caTemp+strlen(caTemp)," and b.groupid=0 ");
//		}
//		else{
	//		sprintf(caTemp+strlen(caTemp)," and (d.groupid=%s or d.groupid in (select groupid from ncgroup where pid=%s ))",caGroupid,caGroupid);
//		}
	}
		if(!utStrIsSpaces(caTime)) {
        if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        }
        else {
        	sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        }       
    }
	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and d.userid=%s ",caUserid);
	}
	
	sprintf(caTemp+strlen(caTemp)," group by c.name order by bb desc limit 0,20");

printf("caTemp=%s\n",caTemp);
	
	lSumservice=0;
	lSumbytes=0;
	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lBytes=0;
       memset(caName,0,sizeof(caName));
       lService=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,30,caName,
                                      UT_TYPE_LONG,4,&lService);
       while(iReturn==0||iReturn==1405){
        stService[lSumservice].lBytes=lBytes;
        stService[lSumservice].service=lService;
        strcpy(stService[lSumservice].name,caName);
        
        lSumservice++;
        lSumbytes=lSumbytes+lBytes;                   
      
      	
       lBytes=0;
       memset(caName,0,sizeof(caName));
       lService=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,30,caName,
                                      UT_TYPE_LONG,4,&lService);
       }                               
      pasDbCloseCursor(psCur); 
	
	   //输出服务大类统计数据
	   for(i=0;i<lSumservice;i++){
	   	utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
	   	utPltPutLoopVar(psDbHead,"name_s",i+1,stService[i].name);
	   	utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",(float)stService[i].lBytes/(1024*1024));
	   	utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.1f",(float)stService[i].lBytes/(1024*1024));
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(stService[i].lBytes*100)/lSumbytes);
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
	   	
	   	utPltPutLoopVarF(psDbHead,"service_s",i+1,"%d",stService[i].service);
	}
	
	utPltPutVarF(psDbHead,"sumservice","%d",lSumservice);
	//统计上网流量TOP10
	
//	 		  sprintf(caTemp,"select a.userid,sum(b.sumflow + b.upflow) abyte from ncuserlog b,\
//        	ncuser a where b.userid=a.userid  and conntime<1000000 and b.sumflow + b.upflow>1 ");
//        sprintf(caTemp+strlen(caTemp)," and b.etime>=%lu and b.stime<=%lu ",lStime,lEtime);  
//        if(strlen(caGroupid)!=0){
//        	if(atol(caGroupid)==0)
 //       	 sprintf(caTemp+strlen(caTemp)," and a.groupid=0 ");
 //       	else
 //       	  sprintf(caTemp+strlen(caTemp)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));   
 //       }
//        if(!utStrIsSpaces(caTime)) {
//        	if(strcasecmp(caTime,"worktime") == 0) {
//        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql9("b.stime","b.etime"));
 //       	}
 //       	else {
 //       		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql9("b.stime","b.etime"));
//        	}       
 //   		}
//	     sprintf(caTemp+strlen(caTemp)," group by  a.userid order by abyte desc limit 0,10 ");
 //printf("caTemp=%s\n",caTemp);    
//	     	psCur=pasDbOpenSql(caTemp,0);
 //  	  if(psCur == NULL) {
 //       utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
 //       return 0;
 //      }
 //      lBytes=0;
 //      lUserid=0;
 //      iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
  //                                    UT_TYPE_LONG8,8,&lBytes);
  //     lSumflow=0;                           
  //     while(iReturn==0||iReturn==1405){
  //      stFlow[lSumflow].lBytes=lBytes;
  //      stFlow[lSumflow].userid=lUserid;
        
 //       lSumflow++;          
      
      	
 //      lBytes=0;
 //      lUserid=0;
//       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
//                                      UT_TYPE_LONG8,8,&lBytes);
 //      }                               
 //     pasDbCloseCursor(psCur); 
	 
		 if(utStrIsSpaces(caTime)){     //工作时间和非工作时间都为空
       	sprintf(caTemp,"select a.userid,\
sum(case \
when stime<=%lu and etime>=%lu and etime-stime>0 \
then (%lu-%lu)/(etime-stime)*(b.sumflow + b.upflow)  \
when stime<=%lu and etime<%lu and etime-stime>0 \
then (etime-%lu)*(b.sumflow + b.upflow)/(etime-stime) \
when stime>=%lu and etime>%lu and etime-stime>0 \
then (%lu-stime)*(b.sumflow+b.upflow)/(etime-stime) else b.sumflow + b.upflow End) abyte",\
lStime,lEtime,lEtime,lStime,lStime,lEtime,lStime,lStime,lEtime,lEtime);   
        
        //~~
         sprintf(caTemp+strlen(caTemp)," from ncuserlog b,ncuser a where b.userid=a.userid  and conntime<1000000 and b.sumflow + b.upflow>1 ");
        sprintf(caTemp+strlen(caTemp)," and b.etime>=%lu and b.stime<=%lu ",lStime,lEtime);
        if(strlen(caGroupid)!=0){
        	if(atol(caGroupid)==0)
        	 	sprintf(caTemp+strlen(caTemp)," and a.groupid=0 ");
        	else
        	  sprintf(caTemp+strlen(caTemp)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));   
        }
	     sprintf(caTemp+strlen(caTemp)," group by  a.userid order by abyte desc limit 0,10 ");
	 		 printf("caTemp=%s\n",caTemp);    
	     psCur=pasDbOpenSql(caTemp,0);
   	   if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lBytes=0;
       lUserid=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_LONG8,8,&lBytes);
       lSumflow=0;                           
       while(iReturn==0||iReturn==1405){
	        stFlow[lSumflow].lBytes=lBytes;
	        stFlow[lSumflow].userid=lUserid;
	        lSumflow++; 
	        lBytes=0;
	        lUserid=0;
	        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
	                                       UT_TYPE_LONG8,8,&lBytes);
       }                               
       pasDbCloseCursor(psCur); 
        
   }else{//工作时间和非工作时间不为空
    			sprintf(caTemp,"select count(case \
when stime<=%lu and etime>=%lu and etime-stime>0 \
then (%lu-%lu)/(etime-stime)*(b.sumflow + b.upflow)  \
when stime<=%lu and etime<%lu and etime-stime>0 \
then (etime-%lu)*(b.sumflow + b.upflow)/(etime-stime) \
when stime>=%lu and etime>%lu and etime-stime>0 \
then (%lu-stime)*(b.sumflow+b.upflow)/(etime-stime) else b.sumflow + b.upflow End) abyte",\
lStime,lEtime,lEtime,lStime,lStime,lEtime,lStime,lStime,lEtime,lEtime);   
        
          sprintf(caTemp+strlen(caTemp)," from ncuserlog b,ncuser a where b.userid=a.userid  and conntime<1000000 and b.sumflow + b.upflow>1 ");
	        sprintf(caTemp+strlen(caTemp)," and b.etime>=%lu and b.stime<=%lu ",lStime,lEtime);
	        if(strlen(caGroupid)!=0){
	        	if(atol(caGroupid)==0)
	        	 	sprintf(caTemp+strlen(caTemp)," and a.groupid=0 ");
	        	else
	        	  sprintf(caTemp+strlen(caTemp)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));   
	        }
    		 	iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);	
    			
    			printf("====11===lCount=%d\n",lCount);
    			sprintf(caTemp,"select a.userid,if(b.stime<%lu,%lu,b.stime),if(b.etime>%lu,%lu,b.etime),case \
when stime<=%lu and etime>=%lu and etime-stime>0 \
then (%lu-%lu)/(etime-stime)*(b.sumflow + b.upflow)  \
when stime<=%lu and etime<%lu and etime-stime>0 \
then (etime-%lu)*(b.sumflow + b.upflow)/(etime-stime) \
when stime>=%lu and etime>%lu and etime-stime>0 \
then (%lu-stime)*(b.sumflow+b.upflow)/(etime-stime) else b.sumflow + b.upflow End abyte",\
lStime,lStime,lEtime,lEtime,lStime,lEtime,lEtime,lStime,lStime,lEtime,lStime,lStime,lEtime,lEtime);   
        
         	sprintf(caTemp+strlen(caTemp)," from ncuserlog b,ncuser a where b.userid=a.userid  and conntime<1000000 and b.sumflow + b.upflow>1 ");
        	sprintf(caTemp+strlen(caTemp)," and b.etime>=%lu and b.stime<=%lu ",lStime,lEtime);
        	if(strlen(caGroupid)!=0){
	        	if(atol(caGroupid)==0)
	        	 	sprintf(caTemp+strlen(caTemp)," and a.groupid=0 ");
	        	else
	        	  sprintf(caTemp+strlen(caTemp)," and (a.groupid=%d or a.groupid in (select groupid from ncgroup where pid=%d ))",atol(caGroupid),atol(caGroupid));   
        	}
	     		sprintf(caTemp+strlen(caTemp)," order by userid asc ,abyte desc  ");
	     		
	     		 //按页面条件查出满足条件的记录
       //已知这些记录起始时间结束时间和流量 若不是整点，约一下
	     //printf("====11===caTemp=%s\n",caTemp);
	     psCur=pasDbOpenSql(caTemp,0);
   	   if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lBytes=0;lUserid=0;
       int webstime=0,webetime=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
       																UT_TYPE_LONG,4,&webstime,
       																UT_TYPE_LONG,4,&webetime,
                                      UT_TYPE_LONG8,8,&lBytes);
       int g=0;    
       struct flow_she{
				  	long userid;
				  	long long lBytes;
				  	unsigned long stime;
				  	unsigned long etime;
			 };
			 struct flow_she *shetFlow;  
			 //printf("mem****lCount=%d\n",lCount);
    	 shetFlow=(struct flow_she*)malloc(sizeof(struct flow_she)*(lCount+1));                       
       while(iReturn==0||iReturn==1405){
	        shetFlow[g].lBytes=lBytes;
	        shetFlow[g].userid=lUserid;
	        shetFlow[g].stime=webstime;
	        shetFlow[g].etime=webetime;
	        g++; 
	        lBytes=0;
	        lUserid=0;
	        webstime=0;
	        webetime=0;
	        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
	       																 UT_TYPE_LONG,4,&webstime,
	       																 UT_TYPE_LONG,4,&webetime,
	                                       UT_TYPE_LONG8,8,&lBytes);
       }                               
       pasDbCloseCursor(psCur); 
	    
    	 lSumflow=ncUtlWorkTime_ta_ru("b.stime","b.etime",shetFlow,g,lCount,stFlow,caTime);
    } 
	 
	 
	 
	 
	     
	//输出到界面
	  for(i=0;i<lSumflow;i++){
	  	utPltPutLoopVarF(psDbHead,"num_f",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_f",i+1,"%0.0f",(float)stFlow[i].lBytes/1024);
	  	utPltPutLoopVarF(psDbHead,"uid_f",i+1,"%d",stFlow[i].userid);
	  	
	  	psUser = ncUtlGetUserContById(psShmHead,stFlow[i].userid);
	  	if(psUser){
         utPltPutLoopVar(psDbHead,"name_f",  i+1,psUser->dispname);
        }
	  	
	  }     
	  utPltPutVarF(psDbHead,"sumfow","%d",lSumflow);
	     
	     
	//网站访问请求数分布图
	
	sprintf(caTemp,"select sum(a.lcount) aa,a.tid,c.name from nchttpcount a,ncwebclass c where  a.tid=c.id and a.tid!=0 ");
//	sprintf(caTemp,"select sum(a.lcount) aa,a.tid,c.name from nchttpcount a,ncuser b,ncwebclass c where a.userid=b.userid and a.tid=c.id and a.tid!=0 ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
	if(strlen(caGroupid)>0){
		sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caGroupids);
	}
//	if(strlen(caGroupid)>0){
//		if(atol(caGroupid)==0){
//			sprintf(caTemp+strlen(caTemp)," and b.groupid=0 ");
//		}
//		else{
//			sprintf(caTemp+strlen(caTemp)," and (b.groupid=%s or b.groupid in (select groupid from ncgroup where pid=%s ))",caGroupid,caGroupid);
//		}
//	}
	if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        	}       
  }
	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and b.userid=%s ",caUserid);
	}
	sprintf(caTemp+strlen(caTemp)," group by a.tid,c.name order by aa desc ");
	
  printf("caTemp=%s\n",caTemp);
  
 	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lCount=0;
       lTid=0;
       memset(caName,0,sizeof(caName));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lTid,
                                      UT_TYPE_STRING,30,caName);
       lSumweb=0;  
       lSumwebcount=0;
       lSumwebother=0;                         
       while(iReturn==0||iReturn==1405){
       	lSumwebcount=lSumwebcount+lCount;
       	if(lSumweb<20){
        stWeb[lSumweb].lcount=lCount;
        stWeb[lSumweb].tid=lTid;
        strcpy(stWeb[lSumweb].name,caName);
        
        lSumweb++;   
      }
      else{
      	lSumwebother=lSumwebother+lCount;
      }       
      
      	
       lCount=0;
       lTid=0;
       memset(caName,0,sizeof(caName));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lTid,
                                      UT_TYPE_STRING,30,caName);
       }                               
      pasDbCloseCursor(psCur); 
  
  //输出网站访问统计
  	//输出到界面
	  for(i=0;i<lSumweb;i++){
	  	utPltPutLoopVarF(psDbHead,"num_w",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_w",i+1,"%d",stWeb[i].lcount);
	  	if(lSumwebcount>0)
	  	  utPltPutLoopVarF(psDbHead,"rate_w",i+1,"%0.1f",(float)stWeb[i].lcount*100/lSumwebcount);
	  	else
	  	  utPltPutLoopVar(psDbHead,"rate_w",i+1,"0.0");
	  	  
         utPltPutLoopVar(psDbHead,"name_w",  i+1,stWeb[i].name);
       
       utPltPutLoopVarF(psDbHead,"uid_w",i+1,"%d",stWeb[i].tid); 
	  	
	  }    
	  	  utPltPutVarF(psDbHead,"sumwebp","%d",lSumweb);
	  if(lSumwebother>0){
	  	lSumweb++;
	  	utPltPutLoopVarF(psDbHead,"num_w",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_w",i+1,"%d",lSumwebother);
	  	utPltPutLoopVar(psDbHead,"name_w",i+1,ncLang("0431其它"));
	  	if(lSumwebcount>0)
	  	  utPltPutLoopVarF(psDbHead,"rate_w",i+1,"%0.1f",(float)lSumwebother*100/lSumwebcount);
	  	else
	  	  utPltPutLoopVar(psDbHead,"rate_w",i+1,"0.0");
	  }
	    utPltPutVarF(psDbHead,"sumweb","%d",lSumweb);

  
	//统计点击数最大前20个网址
	
	
	sprintf(caTemp,"select sum(a.lcount) aa,sum(a.bytes),a.host from nchttpcount a where 1=1  ");
//	sprintf(caTemp,"select sum(a.lcount) aa,sum(a.bytes),a.host from nchttpcount a,ncuser b where a.userid=b.userid  ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
	if(strlen(caGroupid)>0){
		sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caGroupids);
	}
//	if(strlen(caGroupid)>0){
//		if(atol(caGroupid)==0){
//			sprintf(caTemp+strlen(caTemp)," and b.groupid=0 ");
//		}
//		else{
//			sprintf(caTemp+strlen(caTemp)," and (b.groupid=%s or b.groupid in (select groupid from ncgroup where pid=%s ))",caGroupid,caGroupid);
//		}
//	}
	if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        	}       
  }
	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and a.userid=%s ",caUserid);
	}
	sprintf(caTemp+strlen(caTemp)," group by a.host order by aa desc,a.host asc limit 0,20 ");
	
  printf("caTemp=%s\n",caTemp);
  
 	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lCount=0;
       lBytes=0;
       memset(caHost,0,sizeof(caHost));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,60,caHost);
       lSumsite=0;  
                            
       while(iReturn==0||iReturn==1405){
       
        stWebsite[lSumsite].lcount=lCount;
        stWebsite[lSumsite].bytes=lBytes;
        strcpy(stWebsite[lSumsite].host,caHost);
        
        lSumsite++;   
         	
       lCount=0;
       lBytes=0;
       memset(caHost,0,sizeof(caHost));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,60,caHost);
       }                               
      pasDbCloseCursor(psCur); 
	
		//输出到界面
	  for(i=0;i<lSumsite;i++){
	  	utPltPutLoopVarF(psDbHead,"num_ws",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_ws",i+1,"%d",stWebsite[i].lcount);
      utPltPutLoopVar(psDbHead,"name_ws",  i+1,stWebsite[i].host);  	
	  }     
	  utPltPutVarF(psDbHead,"sumwebsite","%d",lSumsite);
	
	
	//统计聊天记录数前10个用户
	
	
	sprintf(caTemp,"select count(*) aa,a.userid,'' from nccontlog a  where 1=1 ");
//		sprintf(caTemp,"select count(*) aa,a.userid,b.dispname from nccontlog a,ncuser b where a.userid=b.userid  ");
	sprintf(caTemp+strlen(caTemp)," and a.stime>=%lu and a.stime<=%lu ",lStime,lEtime);
	if(strlen(caGroupid)>0){
		sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caGroupids);
	}
//	if(strlen(caGroupid)>0){
//		if(atol(caGroupid)==0){
//			sprintf(caTemp+strlen(caTemp)," and b.groupid=0 ");
//		}
//		else{
//			sprintf(caTemp+strlen(caTemp)," and (b.groupid=%s or b.groupid in (select groupid from ncgroup where pid=%s ))",caGroupid,caGroupid);
//		}
//	}
	if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql("a.stime"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql("a.stime"));
        	}       
  }
	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and a.userid=%s ",caUserid);
	}
	sprintf(caTemp+strlen(caTemp)," group by a.userid order by aa desc limit 0,10 ");
	
  printf("caTemp=%s\n",caTemp);
  
 	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lCount=0;
       lUserid=0;
       memset(caDispname,0,sizeof(caDispname));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_STRING,30,caDispname);
       lSumIm=0;  
                            
       while(iReturn==0||iReturn==1405){
       
        stIm[lSumIm].lcount=lCount;
        stIm[lSumIm].userid=lUserid;
  //      strcpy(stIm[lSumIm].dispname,caDispname);
        strcpy(stIm[lSumIm].dispname,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        lSumIm++;   
         	
       lCount=0;
       lUserid=0;
       memset(caDispname,0,sizeof(caDispname));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_STRING,30,caDispname);
       }                               
      pasDbCloseCursor(psCur); 
	
			//输出到界面
	  for(i=0;i<lSumIm;i++){
	  	utPltPutLoopVarF(psDbHead,"num_im",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_im",i+1,"%d",stIm[i].lcount);
      utPltPutLoopVar(psDbHead,"name_im",  i+1,stIm[i].dispname);  	
	  }     
	  utPltPutVarF(psDbHead,"sumim","%d",lSumIm);
	
	
	
	
		//统计收发邮件前10个用户
	
//		sprintf(caTemp,"select count(*) aa,a.userid,b.dispname from ncmailmon a,ncuser b where a.userid=b.userid  ");
			sprintf(caTemp,"select count(*) aa,a.userid,'' from ncmailmon a where 1=1  ");
	
		strcpy(caSdate_temp,utTimFormat("%Y-%m-%d %H:%M:%S",lStime));
		strcpy(caEdate_temp,utTimFormat("%Y-%m-%d %H:%M:%S",lEtime));
		
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate_temp,caEdate_temp);
	if(strlen(caGroupid)>0){
		sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caGroupids);
	}
//	if(strlen(caGroupid)>0){
//		if(atol(caGroupid)==0){
//			sprintf(caTemp+strlen(caTemp)," and b.groupid=0 ");
//		}
//		else{
//			sprintf(caTemp+strlen(caTemp)," and (b.groupid=%s or b.groupid in (select groupid from ncgroup where pid=%s ))",caGroupid,caGroupid);
//		}
//	}
	if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s9("a.sdate"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s9("a.sdate"));
        	}       
  }
	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and a.userid=%s ",caUserid);
	}
	sprintf(caTemp+strlen(caTemp)," group by a.userid order by aa desc limit 0,10 ");
	
  printf("caTemp=%s\n",caTemp);
  
 	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lCount=0;
       lUserid=0;
       memset(caDispname,0,sizeof(caDispname));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_STRING,30,caDispname);
       lSumMail=0;  
                            
       while(iReturn==0||iReturn==1405){
       
        stMail[lSumMail].lcount=lCount;
        stMail[lSumMail].userid=lUserid;
 //       strcpy(stMail[lSumMail].dispname,caDispname);
        strcpy(stMail[lSumMail].dispname,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        lSumMail++;   
         	
       lCount=0;
       lUserid=0;
       memset(caDispname,0,sizeof(caDispname));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_STRING,30,caDispname);
       }                               
      pasDbCloseCursor(psCur); 
	
	
	
				//输出到界面
	  for(i=0;i<lSumMail;i++){
	  	utPltPutLoopVarF(psDbHead,"num_mail",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_mail",i+1,"%d",stMail[i].lcount);
      utPltPutLoopVar(psDbHead,"name_mail",  i+1,stMail[i].dispname);  	
	  }     
	  utPltPutVarF(psDbHead,"summail","%d",lSumMail);
	
	
	
	
	
	
	
	
	
	
	
	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_allreport.htm"); 
}





/* 统计某一服务大类用户访问情况  */
int ncShowGServiceDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  utPltDbHead *psDbHead;
                                        //存放网络服务流量
	  char caUserid[16],caGroupid[16],caSdate[16],caEdate[16];
	  char caSdate_temp[20],caEdate_temp[20];
	  long lStime,lEtime;
	  char caTemp[1024];
	  long lTime,i,lSumservice,iReturn;
	  long long lBytes,lSumbytes;
	  char caName[32];
	  
	  struct flow_s{
	  	long userid;
	  	long long lBytes;
	  };
	  struct flow_s stFlow[32];                                            //存放上网流量最大用户
	  long lUserid;
	  char caService[16],caGname[32];
	  char caGroupname[32],caIp[20];
	  char caTitlename[32];
	  long lService;
	  long long lOth;
	  ncUserCont *psUser;
    pasDbCursor *psCur;
    
    
    char caPdf[32];
    long lRowrow;
    struct piechart_datasetf *pChart;           //用于PDF文档输出
    int rowrow;                     
    int column; 
    char **pTemp;
    char caTitle[128];
    char filename[128];
    char caStatdate[128];
    int m;
    column=4;

    char **desc;
    
    
    
    
    
    
    
	  psDbHead = utPltInitDb(); 
	  
//统计网络服务	  
   utMsgGetSomeNVar(psMsgHead,7,"userid",UT_TYPE_STRING,10,caUserid,					  //统计项目
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//起始时间日期
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "groupid",UT_TYPE_STRING,10,caGroupid,
                                    "service",UT_TYPE_STRING,10,caService,
                                    "gname",UT_TYPE_STRING,30,caGname,
                                    "pdf",UT_TYPE_STRING,8,caPdf);
memset(caTitlename,0,sizeof(caTitlename));
if(strlen(caGroupid)>0){
	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));
}
if(strlen(caUserid)>0){
	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
}

if((strlen(caSdate)==0)||strlen(caEdate)==0){
	lTime=time(0);
	ncUtlWeekDate(lTime,caSdate,caEdate);
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
	
	utPltPutVar(psDbHead,"sdate",caSdate);
	utPltPutVar(psDbHead,"edate",caEdate);
	utPltPutVar(psDbHead,"userid",caUserid);
	utPltPutVar(psDbHead,"groupid",caGroupid);
  utPltPutVar(psDbHead,"gname",caGname);
  utPltPutVar(psDbHead,"service",caService);

	
	//统计网络服务
	sprintf(caTemp,"select sum(a.ubytes+a.dbytes) bb,a.userid from ncipservice a,ncuser d,ncipindex b where a.userid=d.userid and a.service=b.sid  ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
	if(strlen(caGroupid)>0){
		if(atol(caGroupid)==0){
			sprintf(caTemp+strlen(caTemp)," and d.groupid=0 ");
		}
		else{
			sprintf(caTemp+strlen(caTemp)," and (d.groupid=%s or d.groupid in (select groupid from ncgroup where pid=%s ))",caGroupid,caGroupid);
		}
	}
	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and d.userid=%s ",caUserid);
	}
	if(strlen(caService)>0){
		sprintf(caTemp+strlen(caTemp)," and b.groupid=%s ",caService);
	}
	sprintf(caTemp+strlen(caTemp)," group by a.userid order by bb desc ");
	
	lSumservice=0;
	lSumbytes=0;
	lOth=0;
	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lBytes=0;
       lUserid=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_LONG,4,&lUserid);
       while(iReturn==0||iReturn==1405){
       	if(lSumservice<10){
        stFlow[lSumservice].lBytes=lBytes;
        stFlow[lSumservice].userid=lUserid;
        
        lSumservice++;
        }
        else{
        	lOth=lOth+lBytes;
        }
        lSumbytes=lSumbytes+lBytes;                   
      
      	
       lBytes=0;
       lUserid=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_LONG,4,&lUserid);
       }                               
      pasDbCloseCursor(psCur); 
	
	
	
	
	if(strlen(caPdf)>0){
	//表格标签
	 desc=malloc(5*sizeof(char*));
	 for(i=0;i<5;i++){
	 	desc[i]=malloc(32);
   }
   strcpy(desc[0],ncLang("0728用户名"));
   strcpy(desc[1],ncLang("0147部门"));
   strcpy(desc[2],ncLang("0054IP地址"));
    sprintf(desc[3],"%s(%s)",ncLang("1103总流量"),"MB");
   strcpy(desc[4],ncLang("1827百分比%"));
     
 //输出到PDF中     
   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*(lSumservice+2));
    if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("1863分配内存出错"));
      return 0;
   }
  
   

   	
   for(i=0;i<lSumservice;i++){  	 
   	 		psUser=ncUtlGetUserContById(psShmHead,stFlow[i].userid);
   	 		if(psUser) { 
   	 			
   	 			pChart[i].name=strdup(psUser->dispname);  
   	 			
   	 			if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*stFlow[i].lBytes/lSumbytes);
   	 			}
 	 		
   	 			pChart[i].percent=strdup(caTemp); 
   
   	 			pChart[i].amount=(float)stFlow[i].lBytes/(1024*1024); 	
  			
  			 
          pChart[i].append=malloc(2*sizeof(char*));
          for(m=0;m<2;m++){
          	pChart[i].append[m]=malloc(32);
          }
          
           if(psUser->psGroup){
              strcpy(pChart[i].append[0],psUser->psGroup->groupname);
            }
 			    else{
 			    	strcpy(pChart[i].append[0],ncLang("0629未知"));
 			    }
          strcpy(caIp,utComHostIp(htonl(psUser->ip)));
          strcpy(pChart[i].append[1],caIp);
          
 			
  	 	
   	 		}
      }
      
    
         // 输出其它
     if(lOth>0){
     		pChart[i].name=strdup(ncLang("0431其它"));  
     	   	if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lOth)/lSumbytes);
   	 			}
   	 			
     	   	pChart[i].percent=strdup(caTemp); 
   	 			
   	 			pChart[i].amount=lOth/(1024*1024); 	

          pChart[i].append=malloc(2*sizeof(char*));
          for(m=0;m<2;m++){
          	pChart[i].append[m]=malloc(32);
          }
          strcpy(pChart[i].append[0],ncLang("0431其它"));
          strcpy(pChart[i].append[1],ncLang("0431其它"));          
         
            i++;                         
     }
     
     
     
     
     
      rowrow=i;
   
    strcpy(caTitle,ncLang("1968服务流量统计"));
    sprintf(filename,"%s.PDF",caTitle); 
    sprintf(caStatdate,ncLang("1934统计时间：从%s 到%s"),	caSdate,caEdate);	

     ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,5,2, caTitle,ncLang("1970单位(MB)"),caStatdate,filename); 
     
//释放内存     
   for(i=0;i<rowrow;i++){
	      free(pChart[i].name);
	      free(pChart[i].percent);
	      for(m=0;m<2;m++){
	      	free(pChart[i].append[m]);
	      }
	      if(pChart[i].append) free(pChart[i].append);
   }  
   if(pChart) free(pChart);
   
   for(i=0;i<4;i++){
   	   free(desc[i]);
   }
    if(desc) free(desc);
  
   
   
   
   
   
     utPltFreeDb(psDbHead);
    
     return 0;
    }

	   //输出服务大类统计数据
	   for(i=0;i<lSumservice;i++){
	   	memset(caIp,0,sizeof(caIp));
	   	memset(caGroupname,0,sizeof(caGroupname));
	   	psUser = ncUtlGetUserContById(psShmHead,stFlow[i].userid);
         	 if(psUser){
         	 	utPltPutLoopVar(psDbHead,"name_s",i+1,psUser->dispname);
         	 	strcpy(caIp,utComHostIp(htonl(psUser->ip)));
         	 	if(psUser->psGroup){
         	 		strcpy(caGroupname,psUser->psGroup->groupname);
         	 		utPltPutLoopVar(psDbHead,"dept",i+1,caGroupname);
         	 	}
         	 	utPltPutLoopVar(psDbHead,"ip",i+1,caIp);
       }

	   	
	   	utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
//	   	utPltPutLoopVar(psDbHead,"name_s",i+1,stService[i].name);
	   	utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",(float)stFlow[i].lBytes/(1024*1024));
	   	utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.1f",(float)stFlow[i].lBytes/(1024*1024));
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(stFlow[i].lBytes*100)/lSumbytes);
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
	   	
	  	 if(lOth>0){
	  	 	utPltPutLoopVar(psDbHead,"name_s",i+1,ncLang("0431其它"));
	  	 	utPltPutLoopVar(psDbHead,"dept",i+1,ncLang("0431其它"));
	  	 utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
	   	 utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",(float)lOth/(1024*1024));
	   	 utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.1f",(float)lOth/(1024*1024));
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(lOth*100)/lSumbytes);
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
	  	 	
	  	 	
	   	}
	}
	
	
	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_gservicedet.htm"); 
}


/* 统计某个人的网络服务流量 */
int ncShowUserSortDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  utPltDbHead *psDbHead;
                                        //存放网络服务流量
	  char caUserid[16],caGroupid[16],caSdate[16],caEdate[16];
	  char caSdate_temp[20],caEdate_temp[20];
	  long lStime,lEtime;
	  char caTemp[1024];
	  long lTime,i,lSumservice,iReturn;
	  long long lBytes,lSumbytes;
	  char caName[32];
	  
	  struct flow_s{
	  	long service;
	  	char name[32];                     //服务名
	  	long long lBytes;
	  };
	  struct flow_s stFlow[32];                                            //存放上网流量最大用户
	  long lUserid;
	  char caService[16],caGname[32];
	  char caGroupname[32],caIp[20];
	  char caTitlename[32];
	  long lService;
	  float fOth;
	  ncUserCont *psUser;
    pasDbCursor *psCur;
    
    
    char caPdf[32];
    long lRowrow;
    struct piechart_datasetf *pChart;           //用于PDF文档输出
    int rowrow;                     
    int column; 
    char **pTemp;
    char caTitle[128];
    char filename[128];
    char caStatdate[128];
    int m;
    column=4;

    char **desc;
    
    char caFlow[16];
    float fFlow;
    
    
    
    
	  psDbHead = utPltInitDb(); 
	  
//统计网络服务	  
   utMsgGetSomeNVar(psMsgHead,5,"userid",UT_TYPE_STRING,10,caUserid,					  //统计项目
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//起始时间日期
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "flow",UT_TYPE_STRING,15,caFlow,
                                    "pdf",UT_TYPE_STRING,8,caPdf);
fFlow=atof(caFlow);
if(strlen(caUserid)>0){
	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
}

if((strlen(caSdate)==0)||strlen(caEdate)==0){
	lTime=time(0);
	ncUtlWeekDate(lTime,caSdate,caEdate);
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
	
	utPltPutVar(psDbHead,"sdate",caSdate);
	utPltPutVar(psDbHead,"edate",caEdate);
	utPltPutVar(psDbHead,"userid",caUserid);
	utPltPutVar(psDbHead,"flow",caFlow);
 

	
	//统计网络服务
	sprintf(caTemp,"select sum(a.ubytes+a.dbytes) bb,a.service,b.name from ncipservice a,ncipindex b where  a.service=b.sid  ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
	
	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and a.userid=%s ",caUserid);
	}

	sprintf(caTemp+strlen(caTemp)," group by a.service order by bb desc limit 0,20");
	
	lSumservice=0;
	lSumbytes=0;
	
	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lBytes=0;
       lService=0;
       memset(caName,0,sizeof(caName));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_LONG,4,&lService,
                                      UT_TYPE_STRING,30,caName);
       while(iReturn==0||iReturn==1405){
       
        stFlow[lSumservice].lBytes=lBytes;
        stFlow[lSumservice].service=lUserid;
        strcpy(stFlow[lSumservice].name,caName);
        
        lSumservice++;
        
        lSumbytes=lSumbytes+lBytes;                   
      
      	
       lBytes=0;
       lService=0;
       memset(caName,0,sizeof(caName));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_LONG,4,&lService,
                                      UT_TYPE_STRING,30,caName);
       }                               
      pasDbCloseCursor(psCur); 
	
//	printf("float=%f\n",fFlow);
	
	fOth=fFlow-(float)lSumbytes/(1024*1024);
	
	
//	printf("fOth=%f\n",fOth);
	if(fOth<0) fOth=0;
	

	if(strlen(caPdf)>0){
	//表格标签
	 desc=malloc(3*sizeof(char*));
	 for(i=0;i<3;i++){
	 	desc[i]=malloc(32);
   }
   strcpy(desc[0],ncLang("1918服务名"));

    sprintf(desc[1],"%s(%s)",ncLang("1103总流量"),"MB");
   strcpy(desc[2],ncLang("1827百分比%"));
     
 //输出到PDF中     
   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*(lSumservice+2));
    if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("1863分配内存出错"));
      return 0;
   }
  
   

   	
   for(i=0;i<lSumservice;i++){  	 
   	 		
   	 			pChart[i].name=strdup(stFlow[i].name);  
   	 			
   	 			if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*stFlow[i].lBytes/(fFlow*1024*1024));
   	 			}
 	 		
   	 			pChart[i].percent=strdup(caTemp); 
   
   	 			pChart[i].amount=(float)stFlow[i].lBytes/(1024*1024); 	
  			
  	 	
   	 		
      }
      
    
         // 输出其它
     if(fOth>0){
     		pChart[i].name=strdup(ncLang("0431其它"));  
     	   	if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(fOth)/(fFlow));
   	 			}
   	 			
     	   	pChart[i].percent=strdup(caTemp); 
   	 			
   	 			pChart[i].amount=fOth; 	

                 
         
            i++;                         
     }
     
     
     
     
     
      rowrow=i;
   
    sprintf(caTitle,ncLang("1971%s服务流量统计"),caTitlename);
    sprintf(filename,"%s.PDF",caTitle); 
    sprintf(caStatdate,ncLang("1934统计时间：从%s 到%s"),	caSdate,caEdate);	

     ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,3,0, caTitle,ncLang("1970单位(MB)"),caStatdate,filename); 
     
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

	   //输出服务大类统计数据
	   for(i=0;i<lSumservice;i++){
	   
      utPltPutLoopVar(psDbHead,"name_s",i+1,stFlow[i].name);
      
	   	
	   	utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
//	   	utPltPutLoopVar(psDbHead,"name_s",i+1,stService[i].name);
	   	utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",(float)stFlow[i].lBytes/(1024*1024));
	   	utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.1f",(float)stFlow[i].lBytes/(1024*1024));
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(stFlow[i].lBytes*100)/(fFlow*1024*1024));
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
	  }
	  	 if(fOth>0){
	  	 	utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
	  	 	utPltPutLoopVar(psDbHead,"name_s",i+1,ncLang("0431其它"));
	  	 	
	   	 utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",fOth);
	   	 utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.1f",fOth);
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(fOth*100)/fFlow);
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
	  	 	
	  	 	
	   	}
	
	 utPltPutVar(psDbHead,"gname",caTitlename);
	
	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_usersortflowdet.htm"); 
}





/* 统计某一网址类别用户访问情况  */
int ncShowGWebClassDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  utPltDbHead *psDbHead;
                                        //存放网络服务流量
	  char caUserid[16],caGroupid[16],caSdate[16],caEdate[16];
	  char caSdate_temp[20],caEdate_temp[20];
	  long lStime,lEtime;
	  char caTemp[1024];
	  long lTime,i,lSumservice,iReturn;
	  long long lBytes,lSumbytes;
	  char caName[32];
	  
	  struct flow_s{
	  	long userid;
	  	long long lBytes;
	  };
	  struct flow_s stFlow[32];                                            //存放上网流量最大用户
	  long lUserid;
	  char caService[16],caGname[32];
	  char caGroupname[32],caIp[20];
	  char caTitlename[32];
	  long lService;
	  long long lOth;
	  ncUserCont *psUser;
    pasDbCursor *psCur;
    
    
    char caPdf[32];
    long lRowrow;
    struct piechart_datasetf *pChart;           //用于PDF文档输出
    int rowrow;                     
    int column; 
    char **pTemp;
    char caTitle[128];
    char filename[128];
    char caStatdate[128];
    int m;
    column=4;

    char **desc;
    
    
    
    
    
    
    
	  psDbHead = utPltInitDb(); 
	  
//统计网络服务	  
   utMsgGetSomeNVar(psMsgHead,7,"userid",UT_TYPE_STRING,10,caUserid,					  //统计项目
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//起始时间日期
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "groupid",UT_TYPE_STRING,10,caGroupid,
                                    "service",UT_TYPE_STRING,10,caService,
                                    "gname",UT_TYPE_STRING,30,caGname,
                                    "pdf",UT_TYPE_STRING,8,caPdf);
memset(caTitlename,0,sizeof(caTitlename));
if(strlen(caGroupid)>0){
	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));
}
if(strlen(caUserid)>0){
	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
}

if((strlen(caSdate)==0)||strlen(caEdate)==0){
	lTime=time(0);
	ncUtlWeekDate(lTime,caSdate,caEdate);
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
	
	utPltPutVar(psDbHead,"sdate",caSdate);
	utPltPutVar(psDbHead,"edate",caEdate);
	utPltPutVar(psDbHead,"userid",caUserid);
	utPltPutVar(psDbHead,"groupid",caGroupid);
  utPltPutVar(psDbHead,"gname",caGname);
  utPltPutVar(psDbHead,"service",caService);

	
	//统计网址访问
	sprintf(caTemp,"select sum(a.lcount) bb,a.userid from nchttpcount a,ncuser d,ncwebclass b where a.userid=d.userid and a.tid=b.id  ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
	if(strlen(caGroupid)>0){
		if(atol(caGroupid)==0){
			sprintf(caTemp+strlen(caTemp)," and d.groupid=0 ");
		}
		else{
			sprintf(caTemp+strlen(caTemp)," and (d.groupid=%s or d.groupid in (select groupid from ncgroup where pid=%s ))",caGroupid,caGroupid);
		}
	}
	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and d.userid=%s ",caUserid);
	}
	if(strlen(caService)>0){
		sprintf(caTemp+strlen(caTemp)," and b.id=%s ",caService);
	}
	sprintf(caTemp+strlen(caTemp)," group by a.userid order by bb desc ");
	
	lSumservice=0;
	lSumbytes=0;
	lOth=0;
	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lBytes=0;
       lUserid=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_LONG,4,&lUserid);
       while(iReturn==0||iReturn==1405){
       	if(lSumservice<10){
        stFlow[lSumservice].lBytes=lBytes;
        stFlow[lSumservice].userid=lUserid;
        
        lSumservice++;
        }
        else{
        	lOth=lOth+lBytes;
        }
        lSumbytes=lSumbytes+lBytes;                   
      
      	
       lBytes=0;
       lUserid=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_LONG,4,&lUserid);
       }                               
      pasDbCloseCursor(psCur); 
	
	
	
	
	if(strlen(caPdf)>0){
	//表格标签
	 desc=malloc(5*sizeof(char*));
	 for(i=0;i<5;i++){
	 	desc[i]=malloc(32);
   }
   strcpy(desc[0],ncLang("0728用户名"));
   strcpy(desc[1],ncLang("0147部门"));
   strcpy(desc[2],ncLang("0054IP地址"));
    sprintf(desc[3],"%s(%s)",ncLang("1969请求数"),ncLang("1898次"));
   strcpy(desc[4],ncLang("1827百分比%"));
     
 //输出到PDF中     
   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*(lSumservice+2));
    if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("1863分配内存出错"));
      return 0;
   }
  
   

   	
   for(i=0;i<lSumservice;i++){  	 
   	 		psUser=ncUtlGetUserContById(psShmHead,stFlow[i].userid);
   	 		if(psUser) { 
   	 			
   	 			pChart[i].name=strdup(psUser->dispname);  
   	 			
   	 			if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*stFlow[i].lBytes/lSumbytes);
   	 			}
 	 		
   	 			pChart[i].percent=strdup(caTemp); 
   
   	 			pChart[i].amount=(float)stFlow[i].lBytes; 	
  			
  			 
          pChart[i].append=malloc(2*sizeof(char*));
          for(m=0;m<2;m++){
          	pChart[i].append[m]=malloc(32);
          }
          
           if(psUser->psGroup){
              strcpy(pChart[i].append[0],psUser->psGroup->groupname);
            }
 			    else{
 			    	strcpy(pChart[i].append[0],ncLang("0629未知"));
 			    }
          strcpy(caIp,utComHostIp(htonl(psUser->ip)));
          strcpy(pChart[i].append[1],caIp);
          
 			
  	 	
   	 		}
      }
      
    
         // 输出其它
     if(lOth>0){
     		pChart[i].name=strdup(ncLang("0431其它"));  
     	   	if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lOth)/lSumbytes);
   	 			}
   	 			
     	   	pChart[i].percent=strdup(caTemp); 
   	 			
   	 			pChart[i].amount=lOth; 	

          pChart[i].append=malloc(2*sizeof(char*));
          for(m=0;m<2;m++){
          	pChart[i].append[m]=malloc(32);
          }
          strcpy(pChart[i].append[0],ncLang("0431其它"));
          strcpy(pChart[i].append[1],ncLang("0431其它"));          
         
            i++;                         
     }
     
     
     
     
     
      rowrow=i;
   
    strcpy(caTitle,ncLang("1972网址请求数统计"));
    sprintf(filename,"%s.PDF",caTitle); 
    sprintf(caStatdate,ncLang("1934统计时间：从%s 到%s"),	caSdate,caEdate);	

     ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,5,2, caTitle,ncLang("1961单位(次)"),caStatdate,filename); 
     
//释放内存     
   for(i=0;i<rowrow;i++){
	      free(pChart[i].name);
	      free(pChart[i].percent);
	      for(m=0;m<2;m++){
	      	free(pChart[i].append[m]);
	      }
	      if(pChart[i].append) free(pChart[i].append);
   }  
   if(pChart) free(pChart);
   
   for(i=0;i<4;i++){
   	   free(desc[i]);
   }
    if(desc) free(desc);
  
   
   
   
   
   
     utPltFreeDb(psDbHead);
    
     return 0;
    }

	   //输出服务大类统计数据
	   for(i=0;i<lSumservice;i++){
	   	memset(caIp,0,sizeof(caIp));
	   	memset(caGroupname,0,sizeof(caGroupname));
	   	psUser = ncUtlGetUserContById(psShmHead,stFlow[i].userid);
         	 if(psUser){
         	 	utPltPutLoopVar(psDbHead,"name_s",i+1,psUser->dispname);
         	 	strcpy(caIp,utComHostIp(htonl(psUser->ip)));
         	 	if(psUser->psGroup){
         	 		strcpy(caGroupname,psUser->psGroup->groupname);
         	 		utPltPutLoopVar(psDbHead,"dept",i+1,caGroupname);
         	 	}
         	 	utPltPutLoopVar(psDbHead,"ip",i+1,caIp);
       }

	   	
	   	utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
//	   	utPltPutLoopVar(psDbHead,"name_s",i+1,stService[i].name);
	   	utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",(float)stFlow[i].lBytes);
	   	utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.0f",(float)stFlow[i].lBytes);
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(stFlow[i].lBytes*100)/lSumbytes);
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
	   	
	  	 if(lOth>0){
	  	 	utPltPutLoopVar(psDbHead,"name_s",i+1,ncLang("0431其它"));
	  	 	utPltPutLoopVar(psDbHead,"dept",i+1,ncLang("0431其它"));
	  	 utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
	   	 utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",(float)lOth);
	   	 utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.0f",(float)lOth);
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(lOth*100)/lSumbytes);
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
	  	 	
	  	 	
	   	}
	}
	
	
	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_gwebclassdet.htm"); 
}




/* 统计某一网址用户访问情况  */
int ncShowGWebSiteDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  utPltDbHead *psDbHead;
                                        //存放网络服务流量
	  char caUserid[16],caGroupid[16],caSdate[16],caEdate[16];
	  char caSdate_temp[20],caEdate_temp[20];
	  long lStime,lEtime;
	  char caTemp[1024];
	  long lTime,i,lSumservice,iReturn;
	  long long lBytes,lSumbytes;
	  char caName[32];
	  
	  struct flow_s{
	  	unsigned long userid;
	  	long long lBytes;
	
	  };
	  struct flow_s stFlow[32];                                            //存放上网流量最大用户
	  unsigned long lUserid;
	  char caService[16],caGname[32];
	  char caGroupname[32],caIp[20];
	  char caTitlename[32];
	  long lService;
	  unsigned long lDip;
	  long long lOth;
	  ncUserCont *psUser;
    pasDbCursor *psCur;
    char caOrder[16],caUnit[16];
    
    char caPdf[32];
    long lRowrow;
    struct piechart_datasetf *pChart;           //用于PDF文档输出
    int rowrow;                     
    int column; 
    char **pTemp;
    char caTitle[128];
    char filename[128];
    char caStatdate[128];
    int m;
    column=4;

    char **desc;
    
    char caGitem[20];
    
    
    
    
    
	  psDbHead = utPltInitDb(); 
	  
//统计网络服务	  
   utMsgGetSomeNVar(psMsgHead,9,"userid",UT_TYPE_STRING,10,caUserid,					  //统计项目
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//起始时间日期
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "groupid",UT_TYPE_STRING,10,caGroupid,
                                    "service",UT_TYPE_STRING,10,caService,
                                    "gname",UT_TYPE_STRING,30,caGname,
                                    "pdf",UT_TYPE_STRING,8,caPdf,
                                    "gitem",UT_TYPE_STRING,10,caGitem,
                                    "order",UT_TYPE_STRING,8,caOrder);
memset(caTitlename,0,sizeof(caTitlename));
if(strlen(caGitem)==0) strcpy(caGitem,"2");
if(strlen(caGroupid)>0){
	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));
}
if(strlen(caUserid)>0){
	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
}

if((strlen(caSdate)==0)||strlen(caEdate)==0){
	lTime=time(0);
	ncUtlWeekDate(lTime,caSdate,caEdate);
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
	
	utPltPutVar(psDbHead,"sdate",caSdate);
	utPltPutVar(psDbHead,"edate",caEdate);
	utPltPutVar(psDbHead,"userid",caUserid);
	utPltPutVar(psDbHead,"groupid",caGroupid);
  utPltPutVar(psDbHead,"gname",caGname);
  utPltPutVar(psDbHead,"service",caService);
  utPltPutVar(psDbHead,"gitem",caGitem);
  utPltPutVar(psDbHead,"order",caOrder);
	
	//统计网址访问
	if(atol(caGitem)==1){
		if(caOrder[0]=='F'){
				sprintf(caTemp,"select sum(a.bytes)/(1024*1024) bb,a.sid from nchttpcount a,ncuser d where a.userid=d.userid   ");
		}
		else{
		 		sprintf(caTemp,"select sum(a.lcount) bb,a.sid from nchttpcount a,ncuser d where a.userid=d.userid   ");
		}
	}
	else{
		  if(caOrder[0]=='F'){
		  	sprintf(caTemp,"select sum(a.bytes)/(1024*1024) bb,a.userid from nchttpcount a,ncuser d where a.userid=d.userid   ");
		  }
		  else{
	       sprintf(caTemp,"select sum(a.lcount) bb,a.userid from nchttpcount a,ncuser d where a.userid=d.userid   ");
	      }
  }
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
	if(strlen(caGroupid)>0){
		if(atol(caGroupid)==0){
			sprintf(caTemp+strlen(caTemp)," and d.groupid=0 ");
		}
		else{
			sprintf(caTemp+strlen(caTemp)," and (d.groupid=%s or d.groupid in (select groupid from ncgroup where pid=%s ))",caGroupid,caGroupid);
		}
	}
	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and d.userid=%s ",caUserid);
	}
	if(strlen(caGname)>0){
		sprintf(caTemp+strlen(caTemp)," and a.host='%s' ",caGname);
	}
		if(atol(caGitem)==1){
			sprintf(caTemp+strlen(caTemp),"  group by a.sid order by bb desc ");
		}
		else{
	    sprintf(caTemp+strlen(caTemp),"  group by a.userid order by bb desc ");
	 }
	lSumservice=0;
	lSumbytes=0;
	lOth=0;
//	printf("caTemp=%s\n",caTemp);
	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lBytes=0;
       lUserid=0;
       lDip=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_ULONG,4,&lUserid);
                                     
       while(iReturn==0||iReturn==1405){
       	if(lSumservice<10){
        stFlow[lSumservice].lBytes=lBytes;
        stFlow[lSumservice].userid=lUserid;
       
        lSumservice++;
         lSumbytes=lSumbytes+lBytes;     
        }
        else{
        	lOth=lOth+lBytes;
        }
//        lSumbytes=lSumbytes+lBytes;                   
      
      	
       lBytes=0;
       lUserid=0;
       lDip=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_ULONG,4,&lUserid);
       }                               
      pasDbCloseCursor(psCur); 
	
	
	
	
	if(strlen(caPdf)>0){
	//表格标签
	 desc=malloc(5*sizeof(char*));
	 for(i=0;i<5;i++){
	 	desc[i]=malloc(32);
   }
   strcpy(desc[0],ncLang("0728用户名"));
   strcpy(desc[1],ncLang("0147部门"));
   strcpy(desc[2],ncLang("0054IP地址"));
   if(caOrder[0]=='F')
     sprintf(desc[3],"%s(%s)","流量","MB");
   else
    sprintf(desc[3],"%s(%s)",ncLang("1969请求数"),ncLang("1898次"));
    
   strcpy(desc[4],ncLang("1827百分比%"));
     
 //输出到PDF中     
   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*(lSumservice+2));
    if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("1863分配内存出错"));
      return 0;
   }
  
   

   	
   for(i=0;i<lSumservice;i++){  
   		if(atol(caGitem)==1){
   			pChart[i].name=strdup(utComHostIp(htonl(stFlow[i].userid))); 
   				if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*stFlow[i].lBytes/lSumbytes);
   	 			}
 	 		
   	 			pChart[i].percent=strdup(caTemp); 
   
   	 			pChart[i].amount=(float)stFlow[i].lBytes; 	
  			
  			 
          pChart[i].append=malloc(2*sizeof(char*));
          for(m=0;m<2;m++){
          	pChart[i].append[m]=malloc(32);
          }
          
         
           strcpy(pChart[i].append[0],"");
          
         
          strcpy(pChart[i].append[1],"");
   			
   			
   			
   			
   		}
   		else{
   	
   	
   		 
   	 		psUser=ncUtlGetUserContById(psShmHead,stFlow[i].userid);
   	 		if(psUser) { 
   	 			
   	 			pChart[i].name=strdup(psUser->dispname);  
   	 			
   	 			if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*stFlow[i].lBytes/lSumbytes);
   	 			}
 	 		
   	 			pChart[i].percent=strdup(caTemp); 
   
   	 			pChart[i].amount=(float)stFlow[i].lBytes; 	
  			
  			 
          pChart[i].append=malloc(2*sizeof(char*));
          for(m=0;m<2;m++){
          	pChart[i].append[m]=malloc(32);
          }
          
           if(psUser->psGroup){
              strcpy(pChart[i].append[0],psUser->psGroup->groupname);
            }
 			    else{
 			    	strcpy(pChart[i].append[0],ncLang("0629未知"));
 			    }
          strcpy(caIp,utComHostIp(htonl(psUser->ip)));
          strcpy(pChart[i].append[1],caIp);
            	 	
   	 		}
   	  	}
      }
      
 /*   
         // 输出其它
     if(lOth>0){
     		pChart[i].name=strdup(ncLang("0431其它"));  
     	   	if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lOth)/lSumbytes);
   	 			}
   	 			
     	   	pChart[i].percent=strdup(caTemp); 
   	 			
   	 			pChart[i].amount=lOth; 	

          pChart[i].append=malloc(2*sizeof(char*));
          for(m=0;m<2;m++){
          	pChart[i].append[m]=malloc(32);
          }
          strcpy(pChart[i].append[0],ncLang("0431其它"));
          strcpy(pChart[i].append[1],ncLang("0431其它"));          
         
            i++;                         
     }
     
   */  
     
     
     
      rowrow=i;
    if(caOrder[0]=='F'){
    	sprintf(caTitle,"网址%s流量统计",caGname);
    	  strcpy(caUnit,"单位(MB)");
    }
    else{
      sprintf(caTitle,ncLang("1973网址%s请求数统计"),caGname);
    
      strcpy(caUnit,"单位(次)");
    }
    sprintf(filename,"%s.PDF",caTitle); 
    sprintf(caStatdate,ncLang("1934统计时间：从%s 到%s"),	caSdate,caEdate);	

     ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,5,2, caTitle,caUnit,caStatdate,filename); 
     
//释放内存     
   for(i=0;i<rowrow;i++){
	      free(pChart[i].name);
	      free(pChart[i].percent);
	      for(m=0;m<2;m++){
	      	free(pChart[i].append[m]);
	      }
	      if(pChart[i].append) free(pChart[i].append);
   }  
   if(pChart) free(pChart);
   
   for(i=0;i<4;i++){
   	   free(desc[i]);
   }
    if(desc) free(desc);
  
   
   
   
   
   
     utPltFreeDb(psDbHead);
    
     return 0;
    }

	   //输出服务大类统计数据
	   for(i=0;i<lSumservice;i++){
	   	memset(caIp,0,sizeof(caIp));
	   	memset(caGroupname,0,sizeof(caGroupname));
	   	
	   		if(atol(caGitem)==1){
	   			utPltPutLoopVar(psDbHead,"name_s",i+1,utComHostIp(htonl(stFlow[i].userid)));
	   		}
	   		else{
	   	
	   	psUser = ncUtlGetUserContById(psShmHead,stFlow[i].userid);
         	 if(psUser){
         	 	utPltPutLoopVar(psDbHead,"name_s",i+1,psUser->dispname);
         	 	strcpy(caIp,utComHostIp(htonl(psUser->ip)));
         	 	if(psUser->psGroup){
         	 		strcpy(caGroupname,psUser->psGroup->groupname);
         	 		utPltPutLoopVar(psDbHead,"dept",i+1,caGroupname);
         	 	}
         	 	utPltPutLoopVar(psDbHead,"ip",i+1,caIp);
       }
      }
	   
	   	utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
//	   	utPltPutLoopVar(psDbHead,"name_s",i+1,stService[i].name);
	   	utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",(float)stFlow[i].lBytes);
	   	utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.0f",(float)stFlow[i].lBytes);
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(stFlow[i].lBytes*100)/lSumbytes);
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
/*	   	
	  	 if(lOth>0){
	  	 	utPltPutLoopVar(psDbHead,"name_s",i+1,ncLang("0431其它"));
	  	 	utPltPutLoopVar(psDbHead,"dept",i+1,ncLang("0431其它"));
	  	 utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
	   	 utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",(float)lOth);
	   	 utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.0f",(float)lOth);
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(lOth*100)/lSumbytes);
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
	  	 	
	  	 	
	   	}
*/
	}
	 if(caOrder[0]=='F'){
	 	utPltPutVar(psDbHead,"ftitle","流量(MB)");
		}
		else{
			utPltPutVar(psDbHead,"ftitle","连接次数");
		}
	
	if(atol(caGitem)==1){
		
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_websitedet_dip.htm"); 
	}
	else{
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_websitedet.htm"); 
	}
}







/* 用户上网行为分析报告  */
int ncWebAllReport_user(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  utPltDbHead *psDbHead;
	  struct service_s{            
	  	long long lBytes;
	  	char name[32];
	  	long service;
	  };
	  struct service_s stService[32];                                         //存放网络服务流量
	  char caUserid[16],caGroupid[16],caSdate[16],caEdate[16];
	  char caSdate_temp[20],caEdate_temp[20];
	  long lStime,lEtime;
	  char caTemp[1024];
	  long lTime,i,lSumservice,iReturn;
	  long long lBytes,lSumbytes;
	  char caName[32];

	  
	  struct web_s{
	  	char name[32];                        //网址类别名称
	  	long tid;                             //网址类别ID
	  	long lcount;                         // 请求数
	  };
	  struct web_s stWeb[32];   
	  long lSumweb,lCount,lTid;
	  long lSumflow;
	  long lUserid;
	  long lSumwebcount,lSumwebother;
	  
	  struct website_s{
	  	char host[64];                        //网址名称
	  	long long bytes;                     //流量
	  	long lcount;                         // 请求数
	  };
	  struct website_s stWebsite[32];   
	  char caHost[64];
	  long lSumsite;
	  
    long lWebcount,lSmailcount,lFmailcount,lFormcount,lImcount;
	  char caTitlename[32];
	  long lService;
	  ncUserCont *psUser;
    pasDbCursor *psCur;
	  psDbHead = utPltInitDb(); 
	  
	  char caTime[32];
//统计网络服务	  
   utMsgGetSomeNVar(psMsgHead,4,"userid",UT_TYPE_STRING,10,caUserid,					  //统计项目
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//起始时间日期
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "time", UT_TYPE_STRING, 31, caTime);
   utPltPutVar(psDbHead,"time",caTime);                                
memset(caTitlename,0,sizeof(caTitlename));

if(strlen(caUserid)>0){
	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
}

if((strlen(caSdate)==0)||strlen(caEdate)==0){
	lTime=time(0);
	ncUtlWeekDate(lTime,caSdate,caEdate);
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
	
	utPltPutVar(psDbHead,"sdate",caSdate);
	utPltPutVar(psDbHead,"edate",caEdate);
	utPltPutVar(psDbHead,"userid",caUserid);
	utPltPutVar(psDbHead,"groupid",caGroupid);
//	printf("caTitlename=%s\n",caTitlename);
	if(strlen(caTitlename)>0){
		utPltPutVar(psDbHead,"dispname",caTitlename);
	}
	else{
//		utPltPutVar(psDbHead,"dispname",ncLang("0269公司"));
    utPltPutVar(psDbHead,"dispname",utComGetVar_sd(psShmHead,"rootname",ncLang("0269公司")));
	}
	
	//统计网络服务
	sprintf(caTemp,"select sum(a.ubytes+a.dbytes) bb,c.name,b.groupid from ncipservice a,ncuser d,ncipindex b,ncservicecgroup c where a.userid=d.userid and a.service=b.sid and b.groupid=c.groupid ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);

	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and d.userid=%s ",caUserid);
	}
	if(!utStrIsSpaces(caTime)) {
        if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        }
        else {
        	sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        }       
    }
	sprintf(caTemp+strlen(caTemp)," group by c.name order by bb desc limit 0,20");
	
	lSumservice=0;
	lSumbytes=0;
	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lBytes=0;
       memset(caName,0,sizeof(caName));
       lService=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,30,caName,
                                      UT_TYPE_LONG,4,&lService);
       while(iReturn==0||iReturn==1405){
        stService[lSumservice].lBytes=lBytes;
        stService[lSumservice].service=lService;
        strcpy(stService[lSumservice].name,caName);
        
        lSumservice++;
        lSumbytes=lSumbytes+lBytes;                   
      
      	
       lBytes=0;
       memset(caName,0,sizeof(caName));
       lService=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,30,caName,
                                      UT_TYPE_LONG,4,&lService);
       }                               
      pasDbCloseCursor(psCur); 
	
	   //输出服务大类统计数据
	   for(i=0;i<lSumservice;i++){
	   	utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
	   	utPltPutLoopVar(psDbHead,"name_s",i+1,stService[i].name);
	   	utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",(float)stService[i].lBytes/(1024*1024));
	   	utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.1f",(float)stService[i].lBytes/(1024*1024));
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(stService[i].lBytes*100)/lSumbytes);
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
	   	
	   	utPltPutLoopVarF(psDbHead,"service_s",i+1,"%d",stService[i].service);
	}
	
	utPltPutVarF(psDbHead,"sumservice","%d",lSumservice);
     
	//网站访问请求数分布图
	
	sprintf(caTemp,"select sum(a.lcount) aa,a.tid,c.name from nchttpcount a,ncuser b,ncwebclass c where a.userid=b.userid and a.tid=c.id and a.tid!=0 ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
	
	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and b.userid=%s ",caUserid);
	}
	if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        	}       
  }
	sprintf(caTemp+strlen(caTemp)," group by a.tid,c.name order by aa desc ");
	
  printf("caTemp=%s\n",caTemp);
  
 	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lCount=0;
       lTid=0;
       memset(caName,0,sizeof(caName));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lTid,
                                      UT_TYPE_STRING,30,caName);
       lSumweb=0;  
       lSumwebcount=0;
       lSumwebother=0;                         
       while(iReturn==0||iReturn==1405){
       	lSumwebcount=lSumwebcount+lCount;
       	if(lSumweb<20){
        stWeb[lSumweb].lcount=lCount;
        stWeb[lSumweb].tid=lTid;
        strcpy(stWeb[lSumweb].name,caName);
        
        lSumweb++;   
      }
      else{
      	lSumwebother=lSumwebother+lCount;
      }       
      
      	
       lCount=0;
       lTid=0;
       memset(caName,0,sizeof(caName));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lTid,
                                      UT_TYPE_STRING,30,caName);
       }                               
      pasDbCloseCursor(psCur); 
  
  //输出网站访问统计
  	//输出到界面
	  for(i=0;i<lSumweb;i++){
	  	utPltPutLoopVarF(psDbHead,"num_w",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_w",i+1,"%d",stWeb[i].lcount);
	  	if(lSumwebcount>0)
	  	  utPltPutLoopVarF(psDbHead,"rate_w",i+1,"%0.1f",(float)stWeb[i].lcount*100/lSumwebcount);
	  	else
	  	  utPltPutLoopVar(psDbHead,"rate_w",i+1,"0.0");
	  	  
         utPltPutLoopVar(psDbHead,"name_w",  i+1,stWeb[i].name);
       
       utPltPutLoopVarF(psDbHead,"uid_w",i+1,"%d",stWeb[i].tid); 
	  	
	  }    
	  	  utPltPutVarF(psDbHead,"sumwebp","%d",lSumweb);
	  if(lSumwebother>0){
	  	lSumweb++;
	  	utPltPutLoopVarF(psDbHead,"num_w",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_w",i+1,"%d",lSumwebother);
	  	utPltPutLoopVar(psDbHead,"name_w",i+1,ncLang("0431其它"));
	  	if(lSumwebcount>0)
	  	  utPltPutLoopVarF(psDbHead,"rate_w",i+1,"%0.1f",(float)lSumwebother*100/lSumwebcount);
	  	else
	  	  utPltPutLoopVar(psDbHead,"rate_w",i+1,"0.0");
	  }
	    utPltPutVarF(psDbHead,"sumweb","%d",lSumweb);

  
	//统计点击数最大前20个网址
	
	
	
	sprintf(caTemp,"select sum(a.lcount) aa,sum(a.bytes),a.host from nchttpcount a,ncuser b where a.userid=b.userid and a.tid!=0 ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);

	if(strlen(caUserid)>0){
		sprintf(caTemp+strlen(caTemp)," and b.userid=%s ",caUserid);
	}
	if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        	}       
  }
	sprintf(caTemp+strlen(caTemp)," group by a.host order by aa desc limit 0,20 ");
	
  printf("caTemp=%s\n",caTemp);
  
 	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
       lCount=0;
       lBytes=0;
       memset(caHost,0,sizeof(caHost));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,60,caHost);
       lSumsite=0;  
                            
       while(iReturn==0||iReturn==1405){
       
        stWebsite[lSumsite].lcount=lCount;
        stWebsite[lSumsite].bytes=lBytes;
        strcpy(stWebsite[lSumsite].host,caHost);
        
        lSumsite++;   
         	
       lCount=0;
       lBytes=0;
       memset(caHost,0,sizeof(caHost));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,60,caHost);
       }                               
      pasDbCloseCursor(psCur); 
	
		//输出到界面
	  for(i=0;i<lSumsite;i++){
	  	utPltPutLoopVarF(psDbHead,"num_ws",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_ws",i+1,"%d",stWebsite[i].lcount);
      utPltPutLoopVar(psDbHead,"name_ws",  i+1,stWebsite[i].host);  	
	  }     
	  utPltPutVarF(psDbHead,"sumwebsite","%d",lSumsite);
	
	
//统计上网记录数

sprintf(caTemp,"select count(*) from nchttplog where vtime>=%lu and vtime<=%lu and userid=%s ",lStime,lEtime,caUserid);
if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s2("sdate","stime"));
        	}       
  }

//printf("caTemp=%s\n",caTemp);
lWebcount=0;
pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lWebcount);

sprintf(caTemp,"select count(*) from nccontlog where stime>=%lu and stime<=%lu and userid=%s ",lStime,lEtime,caUserid);
if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql("stime"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql("stime"));
        	}       
  }
  lImcount=0;
pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lImcount);

strcpy(caSdate_temp,utTimFormat("%Y-%m-%d %H:%M:%S",lStime));
strcpy(caEdate_temp,utTimFormat("%Y-%m-%d %H:%M:%S",lEtime));

sprintf(caTemp,"select count(*) from ncformmon where sdate>='%s' and sdate<='%s' and userid=%s ",caSdate_temp,caEdate_temp,caUserid);
if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s9("sdate"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s9("sdate"));
        	}       
  }
//printf("caTemp=%s\n",caTemp);

lFormcount=0;
pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lFormcount);

//收邮件
sprintf(caTemp,"select count(*) from ncmailmon where sdate>='%s' and sdate<='%s' and userid=%s and flags=2 ",caSdate_temp,caEdate_temp,caUserid);
if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s9("sdate"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s9("sdate"));
        	}       
  }
//printf("caTemp=%s\n",caTemp);
lSmailcount=0;
pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSmailcount);

//发邮件
sprintf(caTemp,"select count(*) from ncmailmon where sdate>='%s' and sdate<='%s' and userid=%s and flags!=2 ",caSdate_temp,caEdate_temp,caUserid);
if(!utStrIsSpaces(caTime)) {
        	if(strcasecmp(caTime,"worktime") == 0) {
        		sprintf(caTemp+strlen(caTemp),"and %s ",ncUtlCvtWorkTime2Sql_s9("sdate"));
        	}
        	else {
        		sprintf(caTemp+strlen(caTemp),"and not (%s) ",ncUtlCvtWorkTime2Sql_s9("sdate"));
        	}       
  }
//printf("caTemp=%s\n",caTemp);
lFmailcount=0;
pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lFmailcount);
	
	utPltPutVarF(psDbHead,"webcount","%d",lWebcount);
	utPltPutVarF(psDbHead,"smailcount","%d",lSmailcount);
	utPltPutVarF(psDbHead,"fmailcount","%d",lFmailcount);
	utPltPutVarF(psDbHead,"formcount","%d",lFormcount);
	utPltPutVarF(psDbHead,"imcount","%d",lImcount);
	
	
	
	
	
	
	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_allreport_user.htm"); 
}





/* 自动生成上网行为分析报告  */
int ncWebGenAllReport(utShmHead *psShmHead)
{
	  utPltDbHead *psDbHead;
	  struct service_s{            
	  	long long lBytes;
	  	char name[32];
	  	long service;
	  };
	  struct service_s stService[32];                                         //存放网络服务流量
	  char caUserid[16],caGroupid[16],caSdate[16],caEdate[16];
	  char caSdate_temp[20],caEdate_temp[20];
	  long lStime,lEtime;
	  char caTemp[2024];
	  long lTime,i,lSumservice,iReturn;
	  long long lBytes,lSumbytes;
	  char caName[32];
	  char caPath[128];
	  char sqlbuf[128];
	  struct flow_s{
	  	long userid;
	  	long long lBytes;
	  };
	  struct flow_s stFlow[32];                                            //存放上网流量最大用户
	  
	  
	  struct web_s{
	  	char name[32];                        //网址类别名称
	  	long tid;                             //网址类别ID
	  	long lcount;                         // 请求数
	  };
	  struct web_s stWeb[32];   
	  long lSumweb,lCount,lTid;
	  long lSumflow;
	  long lUserid;
	  long lSumwebcount,lSumwebother;
	  
	  struct website_s{
	  	char host[64];                        //网址名称
	  	long long bytes;                     //流量
	  	long lcount;                         // 请求数
	  };
	  struct website_s stWebsite[32];   
	  char caHost[64];
	  long lSumsite;
	  
	  
	 struct im_s{
	  	char dispname[32];                    //聊天记录           上网用户
	  	long userid;                          //用户名
	  	long lcount;                         // 记录数
	  };
	  struct im_s stIm[32];   
	  char caDispname[32];
	  long lSumIm;
	  
	 struct mail_s{
	  	char dispname[32];                    //邮件记录           上网用户
	  	long userid;                          //用户名
	  	long lcount;                         // 记录数
	  };
	  struct mail_s stMail[32];   

	  long lSumMail;
	  
	  char caTitlename[32];
	  long lService;
	  ncUserCont *psUser;
    pasDbCursor *psCur;
	  psDbHead = utPltInitDb(); 
	  char caTime[32];
//统计网络服务	  

utPltPutVar(psDbHead,"time","");
memset(caTitlename,0,sizeof(caTitlename));


	lTime=time(0);
	ncUtlWeekDate(lTime,caSdate,caEdate);
	sprintf(caSdate_temp,"%s 00:00",caSdate);
	sprintf(caEdate_temp,"%s 23:59",caEdate);
	lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_temp);
	lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_temp);

	
	utPltPutVar(psDbHead,"sdate",caSdate);
	utPltPutVar(psDbHead,"edate",caEdate);

  utPltPutVar(psDbHead,"dispname",utComGetVar_sd(psShmHead,"rootname",ncLang("0269公司")));
//	utPltPutVar(psDbHead,"dispname",ncLang("0269公司"));

	
	//统计网络服务
	sprintf(caTemp,"select sum(a.ubytes+a.dbytes) bb,c.name,b.groupid from ncipservice a,ncuser d,ncipindex b,ncservicecgroup c where a.userid=d.userid and a.service=b.sid and b.groupid=c.groupid ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
	
	
	sprintf(caTemp+strlen(caTemp)," group by c.name order by bb desc limit 0,20");
	
	lSumservice=0;
	lSumbytes=0;
	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utPltFreeDb(psDbHead);
        return 0;
       }
       lBytes=0;
       memset(caName,0,sizeof(caName));
       lService=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,30,caName,
                                      UT_TYPE_LONG,4,&lService);
       while(iReturn==0||iReturn==1405){
        stService[lSumservice].lBytes=lBytes;
        stService[lSumservice].service=lService;
        strcpy(stService[lSumservice].name,caName);
        
        lSumservice++;
        lSumbytes=lSumbytes+lBytes;                   
      
      	
       lBytes=0;
       memset(caName,0,sizeof(caName));
       lService=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,30,caName,
                                      UT_TYPE_LONG,4,&lService);
       }                               
      pasDbCloseCursor(psCur); 
	
	   //输出服务大类统计数据
	   for(i=0;i<lSumservice;i++){
	   	utPltPutLoopVarF(psDbHead,"num_s",i+1,"%d",i);
	   	utPltPutLoopVar(psDbHead,"name_s",i+1,stService[i].name);
	   	utPltPutLoopVarF(psDbHead,"bytes_s",i+1,"%0.0f",(float)stService[i].lBytes/(1024*1024));
	   	utPltPutLoopVarF(psDbHead,"bytes_s_t",i+1,"%0.1f",(float)stService[i].lBytes/(1024*1024));
	   	if(lSumbytes>0){
	   		utPltPutLoopVarF(psDbHead,"rate_s",i+1,"%0.1f",(float)(stService[i].lBytes*100)/lSumbytes);
	   	}
	   	else{
	   		utPltPutLoopVar(psDbHead,"rate_s",i+1,"0.0");
	   	}
	   	
	   	utPltPutLoopVarF(psDbHead,"service_s",i+1,"%d",stService[i].service);
	}
	
	utPltPutVarF(psDbHead,"sumservice","%d",lSumservice);
	//统计上网流量TOP10
	
	 		  sprintf(caTemp,"select b.userid,sum(b.sumflow + b.upflow) abyte from ncuserlog b,\
        	ncuser a where b.userid=a.userid   ");
        sprintf(caTemp+strlen(caTemp)," and b.etime>=%lu and b.stime<=%lu and conntime<1000000 and b.sumflow + b.upflow>1 ",lStime,lEtime);  
      
	     sprintf(caTemp+strlen(caTemp)," group by  b.userid order by abyte desc limit 0,10 ");
	// printf("caTemp=%s\n",caTemp);    
	     psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
       utPltFreeDb(psDbHead);
        return 0;
       }
       lBytes=0;
       lUserid=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_LONG8,8,&lBytes);
       lSumflow=0;                           
       while(iReturn==0||iReturn==1405){
        stFlow[lSumflow].lBytes=lBytes;
        stFlow[lSumflow].userid=lUserid;
        
        lSumflow++;          
      
      	
       lBytes=0;
       lUserid=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_LONG8,8,&lBytes);
       }                               
      pasDbCloseCursor(psCur); 
	     
	//输出到界面
	  for(i=0;i<lSumflow;i++){
	  	utPltPutLoopVarF(psDbHead,"num_f",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_f",i+1,"%0.0f",(float)stFlow[i].lBytes/1024);
	  	utPltPutLoopVarF(psDbHead,"uid_f",i+1,"%d",stFlow[i].userid);
	  	
	  	psUser = ncUtlGetUserContById(psShmHead,stFlow[i].userid);
	  	if(psUser){
         utPltPutLoopVar(psDbHead,"name_f",  i+1,psUser->dispname);
        }
	  	
	  }     
	  utPltPutVarF(psDbHead,"sumfow","%d",lSumflow);
	     
	     
	//网站访问请求数分布图
	
	sprintf(caTemp,"select sum(a.lcount) aa,a.tid,c.name from nchttpcount a,ncuser b,ncwebclass c where a.userid=b.userid and a.tid=c.id and a.tid!=0 ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
	
	sprintf(caTemp+strlen(caTemp)," group by a.tid,c.name order by aa desc ");
	
//  printf("caTemp=%s\n",caTemp);
  
 	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
       utPltFreeDb(psDbHead);
        return 0;
       }
       lCount=0;
       lTid=0;
       memset(caName,0,sizeof(caName));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lTid,
                                      UT_TYPE_STRING,30,caName);
       lSumweb=0;  
       lSumwebcount=0;
       lSumwebother=0;                         
       while(iReturn==0||iReturn==1405){
       	lSumwebcount=lSumwebcount+lCount;
       	if(lSumweb<20){
        stWeb[lSumweb].lcount=lCount;
        stWeb[lSumweb].tid=lTid;
        strcpy(stWeb[lSumweb].name,caName);
        
        lSumweb++;   
      }
      else{
      	lSumwebother=lSumwebother+lCount;
      }       
      
      	
       lCount=0;
       lTid=0;
       memset(caName,0,sizeof(caName));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lTid,
                                      UT_TYPE_STRING,30,caName);
       }                               
      pasDbCloseCursor(psCur); 
  
  //输出网站访问统计
  	//输出到界面
	  for(i=0;i<lSumweb;i++){
	  	utPltPutLoopVarF(psDbHead,"num_w",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_w",i+1,"%d",stWeb[i].lcount);
	  	if(lSumwebcount>0)
	  	  utPltPutLoopVarF(psDbHead,"rate_w",i+1,"%0.1f",(float)stWeb[i].lcount*100/lSumwebcount);
	  	else
	  	  utPltPutLoopVar(psDbHead,"rate_w",i+1,"0.0");
	  	  
         utPltPutLoopVar(psDbHead,"name_w",  i+1,stWeb[i].name);
       
       utPltPutLoopVarF(psDbHead,"uid_w",i+1,"%d",stWeb[i].tid); 
	  	
	  }    
	  	  utPltPutVarF(psDbHead,"sumwebp","%d",lSumweb);
	  if(lSumwebother>0){
	  	lSumweb++;
	  	utPltPutLoopVarF(psDbHead,"num_w",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_w",i+1,"%d",lSumwebother);
	  	utPltPutLoopVar(psDbHead,"name_w",i+1,ncLang("0431其它"));
	  	if(lSumwebcount>0)
	  	  utPltPutLoopVarF(psDbHead,"rate_w",i+1,"%0.1f",(float)lSumwebother*100/lSumwebcount);
	  	else
	  	  utPltPutLoopVar(psDbHead,"rate_w",i+1,"0.0");
	  }
	    utPltPutVarF(psDbHead,"sumweb","%d",lSumweb);

  
	//统计点击数最大前20个网址
	
	
	
	sprintf(caTemp,"select sum(a.lcount) aa,sum(a.bytes),a.host from nchttpcount a,ncuser b where a.userid=b.userid  ");
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);

	sprintf(caTemp+strlen(caTemp)," group by a.host order by aa desc,a.host asc limit 0,20 ");
	
//  printf("caTemp=%s\n",caTemp);
  
 	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utPltFreeDb(psDbHead);
        return 0;
       }
       lCount=0;
       lBytes=0;
       memset(caHost,0,sizeof(caHost));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,60,caHost);
       lSumsite=0;  
                            
       while(iReturn==0||iReturn==1405){
       
        stWebsite[lSumsite].lcount=lCount;
        stWebsite[lSumsite].bytes=lBytes;
        strcpy(stWebsite[lSumsite].host,caHost);
        
        lSumsite++;   
         	
       lCount=0;
       lBytes=0;
       memset(caHost,0,sizeof(caHost));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_STRING,60,caHost);
       }                               
      pasDbCloseCursor(psCur); 
	
		//输出到界面
	  for(i=0;i<lSumsite;i++){
	  	utPltPutLoopVarF(psDbHead,"num_ws",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_ws",i+1,"%d",stWebsite[i].lcount);
      utPltPutLoopVar(psDbHead,"name_ws",  i+1,stWebsite[i].host);  	
	  }     
	  utPltPutVarF(psDbHead,"sumwebsite","%d",lSumsite);
	
	
	//统计聊天记录数前10个用户
	
		sprintf(caTemp,"select count(*) aa,a.userid,b.dispname from nccontlog a,ncuser b where a.userid=b.userid  ");
	 sprintf(caTemp+strlen(caTemp)," and a.stime>=%lu and a.stime<=%lu ",lStime,lEtime);


	sprintf(caTemp+strlen(caTemp)," group by a.userid order by aa desc limit 0,10 ");
	
//  printf("caTemp=%s\n",caTemp);
  
 	psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utPltFreeDb(psDbHead);
        return 0;
       }
       lCount=0;
       lUserid=0;
       memset(caDispname,0,sizeof(caDispname));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_STRING,30,caDispname);
       lSumIm=0;  
                            
       while(iReturn==0||iReturn==1405){
       
        stIm[lSumIm].lcount=lCount;
        stIm[lSumIm].userid=lUserid;
        strcpy(stIm[lSumIm].dispname,caDispname);
        
        lSumIm++;   
         	
       lCount=0;
       lUserid=0;
       memset(caDispname,0,sizeof(caDispname));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_STRING,30,caDispname);
       }                               
      pasDbCloseCursor(psCur); 
	
			//输出到界面
	  for(i=0;i<lSumIm;i++){
	  	utPltPutLoopVarF(psDbHead,"num_im",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_im",i+1,"%d",stIm[i].lcount);
      utPltPutLoopVar(psDbHead,"name_im",  i+1,stIm[i].dispname);  	
	  }     
	  utPltPutVarF(psDbHead,"sumim","%d",lSumIm);
	
	
	
	
		//统计收发邮件前10个用户
	
		sprintf(caTemp,"select count(*) aa,a.userid,b.dispname from ncmailmon a,ncuser b where a.userid=b.userid  ");
		
		strcpy(caSdate_temp,utTimFormat("%Y-%m-%d %H:%M:%S",lStime));
		strcpy(caEdate_temp,utTimFormat("%Y-%m-%d %H:%M:%S",lEtime));
		
	sprintf(caTemp+strlen(caTemp)," and a.sdate>='%s' and a.sdate<='%s' ",caSdate_temp,caEdate_temp);


	sprintf(caTemp+strlen(caTemp)," group by a.userid order by aa desc limit 0,10 ");
	
//  printf("caTemp=%s\n",caTemp);
  
 	 psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utPltFreeDb(psDbHead);
        return 0;
       }
       lCount=0;
       lUserid=0;
       memset(caDispname,0,sizeof(caDispname));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_STRING,30,caDispname);
       lSumMail=0;  
                            
       while(iReturn==0||iReturn==1405){
       
        stMail[lSumMail].lcount=lCount;
        stMail[lSumMail].userid=lUserid;
        strcpy(stMail[lSumMail].dispname,caDispname);
        
        lSumMail++;   
         	
       lCount=0;
       lUserid=0;
       memset(caDispname,0,sizeof(caDispname));
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                      UT_TYPE_LONG,4,&lUserid,
                                      UT_TYPE_STRING,30,caDispname);
       }                               
      pasDbCloseCursor(psCur); 
	
	
	
				//输出到界面
	  for(i=0;i<lSumMail;i++){
	  	utPltPutLoopVarF(psDbHead,"num_mail",i+1,"%d",i);
	  	utPltPutLoopVarF(psDbHead,"data_mail",i+1,"%d",stMail[i].lcount);
      utPltPutLoopVar(psDbHead,"name_mail",  i+1,stMail[i].dispname);  	
	  }     
	  utPltPutVarF(psDbHead,"summail","%d",lSumMail);
	
	
	
	
	
	 sprintf(caPath,"%s/html/nc_allreport.htm",utComGetVar_sd(psShmHead,"NCHOME","/home/ncmysql/nc"));   
   utPltFileToHtmlFile("nc75/iframe_allreport.htm",caPath,psDbHead);
   sprintf(sqlbuf,"chmod 777 %s",caPath);
   system(sqlbuf);

	return 0;
}



/* 统计更多的网站*/
int ncShowMoreWebSite(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    char caOrder1[20];
    
    int rowrow;                     
    int column; 

    char caTitle[128];
    char filename[128];
    column=4;
    char **desc;
    char caStatdate[128];
    char caUsername[32],caIp[20],caGroupname[32];
    long Webid;
   lTime=time(0);
   lSum=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
//    printf("== lGroupid0=%lu\n",lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
   
   
      iReturn = utMsgGetSomeNVar(psMsgHead,5,"order",  UT_TYPE_STRING,12,caOrder,
																							"exp",    UT_TYPE_STRING,6, caExport,
																							"sunrec", UT_TYPE_STRING,8, caSum,
																							"sdate",  UT_TYPE_STRING,11,caSdate,
																							"edate",  UT_TYPE_STRING,11,caEdate);
																							
	if(strlen(caOrder)==0) strcpy(caOrder,"C");		
	
	strcpy(caOrder1,caOrder);		
   if(strlen(caSum)==0) strcpy(caSum,"100");
   lSumShow=atol(caSum);
   
 if((strlen(caSdate)==0)||strlen(caEdate)==0){
	lTime=time(0);
	ncUtlWeekDate(lTime,caSdate,caEdate);
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
       

  	psDbHead = utPltInitDb();
 
   
    utPltPutVar(psDbHead,"sunrec",caSum);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"order",caOrder);

 

   pFlow=(struct ncFlowSort_s *)malloc(lSumShow*sizeof(struct ncFlowSort_s));
   if(pFlow==NULL){
   	    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0404内存分配"),ncLang("0404内存分配"));
        return 0;
   }
//  		  sprintf(sqlbuf,"select sum(b.bytes)/1024 abyte,sum(b.lcount) acount,b.host from nchttpcount b,ncuser a where b.userid=a.userid  ");
  		  sprintf(sqlbuf,"select sum(b.bytes)/1024 abyte,sum(b.lcount) acount,b.host from nchttpcount b \
      	 where 1=1  ");
  //    	if(strcmp(caSdate,caEdate)!=0)
            sprintf(sqlbuf+strlen(sqlbuf)," and (b.sdate >= '%s' and b.sdate <= '%s') ",caSdate,caEdate);
     
       
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
  		  
  printf("sqlbuf=%s\n",sqlbuf);
  	 utPltPutVar(psDbHead,"order",caOrder);
  	
    psCur=pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
       }
      
        iReturn = 0;
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&l_Count,
                                       UT_TYPE_STRING,63,caHost);
                                                                  
	     iNum=0;
	     lSumbytes=0;
	     lSumcount=0;
	     while(iReturn==0||iReturn==1405){
	     
	     	pFlow[iNum].lBytes=lBytes;
	     	pFlow[iNum].l_Count=l_Count;
	     
	     	strcpy(pFlow[iNum].caHost,caHost);
	     
	     	lSumbytes=lSumbytes+lBytes;
	     	lSumcount=lSumcount+l_Count;
	     	iNum++;
        lBytes=0;
        l_Count=0;
        memset(caHost,0,sizeof(caHost));
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&l_Count,
                                       UT_TYPE_STRING,63,caHost);   
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
       
	 fprintf(fp,"序号,网址,流量,次数\n");
	 for(i=0;i<iNum;i++){	 		
         	 fprintf(fp,"%d,\"%s\",%0.1f,%llu\n",i+1,pFlow[i].caHost,(float)pFlow[i].lBytes/1024,pFlow[i].l_Count);
 	 }

        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
         free(pFlow);
        utPltFreeDb(psDbHead);        
		    return 0;
}




	   
//输出到界面	   
	   for(i=0;i<iNum;i++){
         utPltPutLoopVar(psDbHead,"order",i+1,caOrder1);
         utPltPutLoopVar(psDbHead,"host",  i+1,pFlow[i].caHost);																																							//按次数排序     
				 utPltPutLoopVarF(psDbHead,"count",i+1,"%llu",pFlow[i].l_Count);				      
         utPltPutLoopVarF(psDbHead,"num",i+1,"%d",i);        
         utPltPutLoopVarF(psDbHead,"dbytes",i+1,"%0.1f",(float)pFlow[i].lBytes/1024);
         utPltPutLoopVar(psDbHead,"sdate",  i+1,caSdate);
         utPltPutLoopVar(psDbHead,"edate",i+1,caEdate);
         
                
      }
     
     free(pFlow);
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_websitemore.htm"); 

}






/* 统计魔兽服务目标ip*/
int ncShowServiceDip(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  pasDbCursor *psCur;
    char sqlbuf[1024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
  
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
        unsigned long  dip;
        unsigned long long bytes;  
        unsigned long long ubytes;
        unsigned long long dbytes;     
    };
    struct ncFlowSort_s *pFlow;
 
    long long lSumbytes,lUbytes,lDbytes,lBytes;
    unsigned long lDip;
    long lService;
   
    long lSumcount;
    long lGitem;
    long lGroupid;
    long m;

    int rowrow;                     
    int column; 
    char caService[16];
    char caTitle[128];
    char filename[128];
    char caName[32];
    column=4;
    char **desc;
    char caStatdate[128];
    char caUsername[32],caIp[20],caGroupname[32];
    long Webid;
    char caExpsql[16];
    unsigned long lIp;
    char caArea[64],caName2[64];
    char caStattype[32];
    long iRet;
    
   lTime=time(0);
   lSum=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
//    printf("== lGroupid0=%lu\n",lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
   
   
      iReturn = utMsgGetSomeNVar(psMsgHead,7,"service",  UT_TYPE_STRING,10,caService,
																							"exp",    UT_TYPE_STRING,6, caExport,
																							"sunrec", UT_TYPE_STRING,8, caSum,
																							"sdate",  UT_TYPE_STRING,11,caSdate,
																							"edate",  UT_TYPE_STRING,11,caEdate,
																							"expsql", UT_TYPE_STRING,10,caExpsql,
																							"stattype",UT_TYPE_STRING,8,caStattype);
		if(strlen(caService)==0)
		   strcpy(caService,"2203");
		   																			
		strcpy(caName,ncUtlGetServiceName(psShmHead,atol(caService),"\0"));
				
   if(strlen(caSum)==0) strcpy(caSum,"10");
   lSumShow=atol(caSum);
   
 if((strlen(caSdate)==0)||strlen(caEdate)==0){
	lTime=time(0);
	ncUtlWeekDate(lTime,caSdate,caEdate);
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
       





  		  sprintf(sqlbuf,"select sum(b.ubytes+b.dbytes)/1024 abyte,sum(b.ubytes)/1024,sum(b.dbytes)/1024,b.dip  from ncserviceip b \
        	 where 1=1  ");
        sprintf(sqlbuf+strlen(sqlbuf)," and service=%d ",atol(caService));

         sprintf(sqlbuf+strlen(sqlbuf)," and (b.sdate >= '%s' and b.sdate <= '%s') ",caSdate,caEdate);
      
  		  sprintf(sqlbuf+strlen(sqlbuf)," group by b.dip  ");
  		  
        
        sprintf(sqlbuf+strlen(sqlbuf)," order by abyte desc limit 0,%lu ",lSumShow);
  		  
//  printf("sqlbuf=%s\n",sqlbuf);

 if(atol(caStattype)==1){
  	char sql2[2048];
  	sprintf(sql2,"001;%s",sqlbuf);
		utComSetVar_s(psShmHead,"reportsql",sql2);
		utComSetVar_s(psShmHead,"reportname",caName);
		utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","进入后台统计，统计结果请在<历史报表查看>中查阅!","进入后台统计，统计结果请在<历史报表查看>中查阅!");
    return 0;
		
		
	}			
	else{
		utComSetVar_s(psShmHead,"reportsql","");
		utComSetVar_s(psShmHead,"reportname","");
	}	
  	
  	
  	 psDbHead = utPltInitDb(); 
    utPltPutVar(psDbHead,"sunrec",caSum);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"service",caService);
 
  	
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
        lUbytes=0;
        lDbytes=0;
        lDip=0;
        iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG8,8,&lUbytes,
                                       UT_TYPE_LONG8,8,&lDbytes,
                                       UT_TYPE_ULONG,4,&lDip);
                                      
                                                                  
	     iNum=0;
	     lSumbytes=0;
	     lSumcount=0;
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
	
	if(strlen(caExport)>0){                                      //输出到EXEC中
		    FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"ncserviceip%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613网站访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,"%s目标IP流量排行(%s--%s)\n",caName,caSdate_temp,caEdate_temp);
       
	 fprintf(fp,"序号,目标IP,IP位置,总流量(MB),上行流量(MB),下行流量(MB)\n");
	 for(i=0;i<iNum;i++){	 
	 	
	 			lIp=pFlow[i].dip; 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iReturn == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            
          }
          else{
          	strcpy(caTemp,"");
          }
	 	
	 			
         	 fprintf(fp,"%d,\"%s\",\"%s\",%0.1f,%0.1f,%0.1f\n",i+1,utComHostIp(htonl(pFlow[i].dip)),caTemp,(float)pFlow[i].bytes/1024,(float)pFlow[i].ubytes/1024,(float)pFlow[i].dbytes/1024);
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

	sprintf(caTemp,"网络服务%s,统计时间%s-%s",caName,caSdate,caEdate);
	strcpy(caTdesc,utTimFormat("%Y%m%d%H%M%S",time(0)));
	sprintf(caDesc,"网络服务%s目标IP排行(%s)",caName,caTdesc);
	iReturn=pasDbExecSqlF("insert into ncreportplatelog(sid,name,descr,stime,pid) values(%d,'%s','%s',%d,1)",lSid,caDesc,caTemp,time(0));
	if(iReturn==0){
		for(i=0;i<iNum;i++){
			pasDbExecSqlF("insert into ncreportdet(tid,webname,ip,bytes,ubytes,dbytes,lcount) values(%d,'%s','%s',%llu,%llu,%llu,%d)",\
			lSid,"",utComHostIp(htonl(pFlow[i].dip)),pFlow[i].bytes,pFlow[i].ubytes,pFlow[i].dbytes,0);
		}
	}
}


	   
//输出到界面	  
   utPltPutVar(psDbHead,"title",caName); 
	   for(i=0;i<iNum;i++){
         lIp=pFlow[i].dip; 
         iReturn = pasUtlGetIpLocation(lIp,caArea,caName2);    
          if(iReturn == 1) {
          	sprintf(caTemp,"%s,%s",caArea,caName2);
            utPltPutLoopVar(psDbHead,"dip_desc",i+1,caTemp);
          }
          else{
          	strcpy(caTemp,"");
          }
        
         utPltPutLoopVar(psDbHead,"dip",  i+1,utComHostIp(htonl(pFlow[i].dip)));																																							//按次数排序     
		
		

		
		
			   
         utPltPutLoopVarF(psDbHead,"num",i+1,"%d",i);        
         utPltPutLoopVarF(psDbHead,"bytes",i+1,"%0.1f",(float)pFlow[i].bytes/1024);
         utPltPutLoopVarF(psDbHead,"ubytes",i+1,"%0.1f",(float)pFlow[i].ubytes/1024);
         utPltPutLoopVarF(psDbHead,"dbytes",i+1,"%0.1f",(float)pFlow[i].dbytes/1024);
                
      }
     
     free(pFlow);
     
     
     
         for(i=0;i<ncPatGetSumService();i++) {
           utPltPutLoopVarF(psDbHead,"id_s",i+1,"%lu",ncUtlGetServiceIdByNum(psShmHead,i));
           utPltPutLoopVar(psDbHead,"name_s",i+1,ncUtlGetServiceByNum(psShmHead,i));
    }
    
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
     
     
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_serviceip.htm"); 

}



 /*显示上网行为管理周报界面*/   
int ncWeekReport_Con(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char *p;
    unsigned long lTime;
    char sqlbuf[1024];
    char service_name[17]="";
    utPltDbHead *psDbHead;
    char caSdate1[32],caSdate2[32],caGroupid[16];
    char caStime[12],caEtime[12];
    char caSort[16],caRecnum[16],caTime[32],caWay[6],caOpt[6];
    char caService[1024]="";
    char *pBuf,*pSelect;
    int lSumCol=0;
    int i,m;
    int iNum=0;
    int iNum1=0;
    int m1=0;
    int i1=0;
    int ck_flag=0;
    char caVar[17]="";
    long iSumCol;
    char ck_id[1024],name[32];
    long id;
    char t_id[16];
    char caSdate[20],caEdate[20];
    long lId,index1,index2;
    char caName[36];
    char cheIDW[1024],cheIDS[1024];
    char caTemp[1028];
    pasDbCursor *psCur;
   
    
    iReturn = utMsgGetSomeNVar(psMsgHead,9,
					"sdate1",    UT_TYPE_STRING, 31,  caSdate1,
					"sdate2",    UT_TYPE_STRING, 31,  caSdate2,
					"stime",     UT_TYPE_STRING, 11,  caStime,
					"eitme",   UT_TYPE_STRING, 11,  caEtime,
					"groupid",  UT_TYPE_STRING, 11,  caGroupid,
					"way",      UT_TYPE_STRING, 5,   caWay,
					"opt",      UT_TYPE_STRING, 5,   caOpt,
					"cheIDW",   UT_TYPE_STRING,1000,cheIDW,
					"cheIDS",   UT_TYPE_STRING,1000,cheIDS);
					
					
		
    /* 显示部门  */
   if(strlen(cheIDS)==0){
    	strcpy(cheIDS,"1,2,7,10");
    }
    if(strlen(cheIDW)==0){
    	strcpy(cheIDW,"1000,1001,1002,1003,1004,1005,1006,1007,1008,1009,1010,1011,2006,2007,3003,3008,4001,4005,5003,5006,6005,6003,6012,6010,7001,7002,7003,7004,7005,7006,7009,7010,7007,7008");
    }
    psDbHead = utPltInitDb();
    lTime = time(0);
    ncUtlWeekDate(lTime,caSdate,caEdate);
    pBuf=pasUtlCvtSqlSelectTreeBmA(ncLang("0455全部,;"));
//    pBuf = pasUtlCvtSqlSelectA("select groupname,groupid from ncgroup order by groupid",
//               ncLang("0455全部,;"));
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
        free(pBuf);
        utPltPutVar(psDbHead,"sgroup", pSelect);
        free(pSelect);
    }
    if(!utStrIsSpaces(caGroupid)) {
        utPltPutVar(psDbHead,"groupid", caGroupid);
    }
    if(!utStrIsSpaces(caSdate1)) {
        utPltPutVar(psDbHead,"sdate1", caSdate1);
    }
    else {
        utPltPutVar(psDbHead,"sdate1", caSdate);
    }
    if(!utStrIsSpaces(caSdate2)) {
        utPltPutVar(psDbHead,"sdate2", caSdate2);
    }
    else {
        utPltPutVar(psDbHead,"sdate2", caEdate);
    }

    if(!utStrIsSpaces(caStime)) {
        utPltPutVar(psDbHead,"stime", caStime);
    }
    else {
        utPltPutVar(psDbHead,"stime", "00");
    }
    if(!utStrIsSpaces(caEtime)) {
        utPltPutVar(psDbHead,"etime", caEtime);
    }
    else {
        utPltPutVar(psDbHead,"etime", "23");
    }
    if(!utStrIsSpaces(caWay)) {
        utPltPutVar(psDbHead,"way", caWay);
    }
    else {
        utPltPutVar(psDbHead,"way", "1");
    }
   if(!utStrIsSpaces(caOpt)) {
        utPltPutVar(psDbHead,"opt", caOpt);
    }
    else {
        utPltPutVar(psDbHead,"opt", "1");
    }
    
 //显示网络服务大类       
    sprintf(sqlbuf,"select count(*) from ncservicecgroup ");
    pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&iSumCol);
    
   
    
    sprintf(sqlbuf,"select groupid,name from ncservicecgroup order by groupid");
    psCur=pasDbOpenSql(sqlbuf,0);
     if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0444取服务"),ncLang("0549数据库出错"));
            return 0;
        }
        id=0;
        memset(name,0,sizeof(name));
        iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,
        			     UT_TYPE_STRING,63,name);
        i=0;
        iNum=0;
        while((iReturn==0)||(iReturn==1405))
        {
        	
        	if(i%6==0)
    			{
    		 			iNum++;
    		 			m=0;
    	   }
		    	m++;	
		    	utPltPutLoopVar2(psDbHead,"service_name",iNum,m,name);
		    	utPltPutLoopVarF2(psDbHead,"i",iNum,m,"%d",i);
		      utPltPutLoopVarF2(psDbHead,"id",iNum,m,"%d",id);
		      
		      sprintf(t_id,",%d,",id);
		      sprintf(caTemp,",%s,",cheIDS);
		      if(strstr(caTemp,t_id)){
		      	utPltPutLoopVar2(psDbHead,"check",iNum,m,"checked");
		      }
        	
        	i++;
        	
	        id=0;
	        memset(name,0,sizeof(name));
	        iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,
	        			     UT_TYPE_STRING,63,name);
        	
        }
       pasDbCloseCursor(psCur);
       utPltPutVarF(psDbHead,"sum_s","%d",i);
    
    //显示网址类别
    psCur=pasDbOpenSql("select id,name from ncwebclass order by name ",0);
if(psCur==NULL){
	      utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0582统计"),ncLang("0549数据库出错"));
        return 0;
    }
     iReturn=0;
     iNum=0;
     lId=0;
     memset(caName,0,sizeof(caName));
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4, &lId,
                                    UT_TYPE_STRING,31,caName);
     while(iReturn==0||iReturn==1405){
     
     	index1=iNum/6+1;
     	index2=iNum%6+1;
     	utPltPutLoopVarF2(psDbHead,"id_w",index1,index2,"%d",lId);
     	utPltPutLoopVar2(psDbHead,"name_w",index1,index2,caName);
     	utPltPutLoopVarF2(psDbHead,"iw",index1,index2,"%d",iNum);
     	
		      sprintf(t_id,",%d,",lId);
		      sprintf(caTemp,",%s,",cheIDW);
		      if(strstr(caTemp,t_id)){
		      	utPltPutLoopVar2(psDbHead,"check_w",index1,index2,"checked");
		   }
     	
     		iNum++;
     		
     lId=0;
     memset(caName,0,sizeof(caName));
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4, &lId,
                                    UT_TYPE_STRING,31,caName);     		
    }                         
     pasDbCloseCursor(psCur);
     utPltPutVarF(psDbHead,"sum_w","%d",iNum);
    
    

    
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/nc_weekreport_menu.htm");
    return 0;
}






/* 统计上网行为报告周报  */
int ncWeekReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char sqlbuf[2024];
    char *p,caVar[16];
    char caStime[3]="";
    char caEtime[3]="";
    char caMac[20];
    char caDispname[20];
    long width=0;
    int     i,iReturn,iNum,lRowNum,iSumRec,lSumPage;
    long lGroupid0;
    char caOpt[32],caWay[32],caOptlabel[16];
    char caSub1[32],caSub2[32],caSub3[32],caGroupid[31];
    char caExport[32];
    char caSdate[32],caSdate1[32],caSdate2[32],caSdate9[32],caUserid[32],caStart[32],caUsername[36],caTemp[30],caSip[32],caDip[32];
    unsigned long lUserid,lStart,lConntime,lService,lSip,lDip,lTime;
    unsigned long *lpTime;
    long lSumCol,lGroupid;
    long tempSumCol=0;
    long lserv[50];
    long qt_flag=0;
    long ser_flag=0;
    long max_count=0;
    long temp_i=0;
    long long *lpBytes;
    long long lUbytes,lDbytes;
    char caService[1024]="";
    char *pSelect;
    char caIp[20];
    char cheIDW[1024],cheIDS[1024];
    long iNum_2,iNum_8,iNum_9,yx_flag,p2p_flag,gp_flag;
    utPltDbHead *psDbHead;
    struct ncSflow_s stGserver[32];
    long id;
    char name[32];
    memset(&stGserver,0,32*sizeof(struct ncSflow_s));
    struct stData{
    	unsigned long id;
    	unsigned long gid;
    	char caSname[32];
    	char caGname[32];
    };
    struct stData *psData;
    unsigned char *pHash_s;    
    pasLHashInfo sHashInfo; 
    struct stData_g{
      unsigned long gid;
    	unsigned long gsid;   	
    	char caDesc[1024];
    	char caGsname[32];
    	char caGroupname[32];
    	long long bytes;
    };
    struct stData_g *psData_g;
    unsigned char *pHash_g;
    
    unsigned char *pHash_webclass;
    unsigned char *pHash_webdata;
    ncUserCont   *psUser;
    
    
    struct stKey{
    	unsigned long gid;              //部门id
    	unsigned long id;               //服务类别ID

    };
    struct stKey psKey; 
    long long lBytes;
    char caSname[32],caGname[32];
      char caObj[200][12];
      long lCount,lNum,n;
      long long lSum,lSb[1000];
      long m=0;
    
    
    long lSid;
    lRowNum = 50;
   

    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
    utMsgPrintMsg(psMsgHead);
    memset(caUserid,0,sizeof(caUserid));
   	iReturn = utMsgGetSomeNVar(psMsgHead,10,
   	                "opt",   UT_TYPE_STRING, 8,  caOpt,   /* 选择分组对象 1-部门 2-个人 */
					"sdate1",  UT_TYPE_STRING, 31, caSdate1,
					"sdate2",  UT_TYPE_STRING, 31, caSdate2,
					"userid",  UT_TYPE_STRING, 31, caUserid,
					"groupid", UT_TYPE_STRING, 31, caGroupid,
					"CurPg",  UT_TYPE_STRING,31, caStart,
					"export", UT_TYPE_STRING,31,caExport,
					"dispname",UT_TYPE_STRING,18,caDispname,
					"cheIDW",  UT_TYPE_STRING,1000,cheIDW,
					"cheIDS",  UT_TYPE_STRING,1000,cheIDS);
   if(utStrIsSpaces(caStart)) strcpy(caStart,"1");
  
 if(strlen(caExport)==0){    
     psDbHead = utPltInitDb();
     utPltPutVar(psDbHead,"sdate1",caSdate1);
     utPltPutVar(psDbHead,"sdate2",caSdate2);
     utPltPutVar(psDbHead,"cheIDS",cheIDS);
     utPltPutVar(psDbHead,"cheIDW",cheIDW);
     utPltPutVar(psDbHead,"groupid",caGroupid);
     utPltPutVar(psDbHead,"dispname",caDispname);
     utPltPutVar(psDbHead,"opt",caOpt);
    }

//统计网络服务  
  if(strlen(cheIDS)>0){
  	
  	pHash_s = (unsigned char *)pasLHashInit(1500,1500,sizeof(struct stData),0,4);    
    if(pHash_s == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","分配内存出错","分配内存出错");
         return (-1);
    }
    pHash_g = (unsigned char *)pasLHashInit(3500,3500,sizeof(struct stData_g),0,8);    
    if(pHash_g == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","分配内存出错","分配内存出错");
         return (-1);
    }
    
    
    
  	 sprintf(sqlbuf,"select a.sid,a.groupid,a.name,b.name from ncipindex a,ncservicecgroup b where a.groupid=b.groupid and a.groupid in (%s) ",cheIDS);
  	 psCur=pasDbOpenSql(sqlbuf,0);
     if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0444取服务"),ncLang("0549数据库出错"));
            return 0;
        }
        lSid=0;
        lGroupid=0;
        memset(caSname,0,sizeof(caSname));
        memset(caGname,0,sizeof(caGname));
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&lSid,
      	                             UT_TYPE_LONG,4,&lGroupid,
      	                             UT_TYPE_STRING,31,caSname,
      	                             UT_TYPE_STRING,31,caGname);
       while(iReturn==0||iReturn==1405)
       {
       	
       	 psData = (struct stData *)pasLHashLookA(pHash_s,&lSid);  
       	 if(psData){
       	 	psData->gid=lGroupid;
       	 	strcpy(psData->caSname,caSname);
       	 	strcpy(psData->caGname,caGname);
       	 }  
       	
       	 lSid=0;
         lGroupid=0;
         memset(caSname,0,sizeof(caSname));
         memset(caGname,0,sizeof(caGname));
         iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&lSid,
      	                             UT_TYPE_LONG,4,&lGroupid,
      	                             UT_TYPE_STRING,31,caSname,
      	                             UT_TYPE_STRING,31,caGname);
       }
      pasDbCloseCursor(psCur);                            
        
       
        
     sprintf(sqlbuf,"select userid,service ,sum(ubytes+dbytes) from ncipservice where 1=1");
     if(strlen(caSdate1)>0){
     	sprintf(sqlbuf+strlen(sqlbuf)," and sdate>='%s' ",caSdate1);
    }
    if(strlen(caSdate2)>0){
    	sprintf(sqlbuf+strlen(sqlbuf)," and sdate<='%s' ",caSdate2);
    }
    sprintf(sqlbuf+strlen(sqlbuf)," and service in (select sid from ncipindex where groupid in (%s))",cheIDS);
    if(strlen(caGroupid)>0){
    	sprintf(sqlbuf+strlen(sqlbuf)," and userid in (select userid from ncuser where groupid in (%s))",caGroupid);
    }
    if(strlen(caDispname)>0){
    	sprintf(sqlbuf+strlen(sqlbuf)," and userid in (select userid from ncuser where username like '%c%s%c' or dispname like '%c%s%c') ",'%',caDispname,'%','%',caDispname,'%');
    }
    
    sprintf(sqlbuf+strlen(sqlbuf)," group by userid,service limit 0,3499");
    
    printf("sqlbuf=%s\n",sqlbuf);
  	 psCur=pasDbOpenSql(sqlbuf,0);
     if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0444取服务"),ncLang("0549数据库出错"));
            return 0;
        }
        lUserid=0;
        lService=0;
        lBytes=0;
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&lUserid,
      	                             UT_TYPE_LONG,4,&lService,
      	                             UT_TYPE_LONG8,8,&lBytes);
      	                           
       while(iReturn==0||iReturn==1405)
       {

       	  psData = (struct stData *)pasLHashLookA(pHash_s,&lService);   
       	 if(psData){
       	 	 psUser = ncUtlGetUserContById(psShmHead,lUserid);
        	if(psUser) {
        		if(atol(caOpt)==2){
        			psKey.gid=lUserid;
        		}
        		else{
       	 	     psKey.gid=psUser->groupid;
       	 	    }
       	 	     psKey.id=psData->gid;
       	 	     psData_g = (struct stData_g *)pasLHashLookA(pHash_g,&psKey);   
       	       if(psData_g){

       	       	psData_g->bytes=psData_g->bytes+lBytes;
       	       	strcpy(psData_g->caGsname,psData->caGname);
       	       	
       	       if(atol(caOpt)==2){
       	       	strcpy(psData_g->caGroupname,psUser->dispname);
       	       	    }
       	       	else{
       	       	   strcpy(psData_g->caGroupname,psUser->psGroup->groupname);
       	        }
       	       	if((strstr(psData_g->caDesc,psData->caSname)==NULL)&&strlen(psData_g->caDesc)<1000){
       	       		sprintf(psData_g->caDesc+strlen(psData_g->caDesc),"%s、",psData->caSname);
       	       		
       	       	}
       	       }
       	 	     
       	 	 }

       	 }  
       
        lUserid=0;
        lService=0;
        lBytes=0;
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&lUserid,
      	                             UT_TYPE_LONG,4,&lService,
      	                             UT_TYPE_LONG8,8,&lBytes);
       }
      pasDbCloseCursor(psCur);    
    

    
  if(strlen(caExport)==0){  

      lCount=sepcharbydh(cheIDS,caObj); 
 //     printf("lCount=%d\n",lCount);  
      lNum=0;
      for(i=0;i<lCount;i++){
      	m=0;
      	lSum=0;
      	lGroupid=atol(caObj[i]);
      	 psData_g = (struct stData_g *)pasLHashFirst(pHash_g,&sHashInfo);
      	 while(psData_g){
      	 	if(lGroupid==psData_g->gsid){
      	 		if(m==0){lNum++;}
      	 		if(m>=1000) break;
      	 		lSb[m]=psData_g->bytes;
      	 		lSum=lSum+psData_g->bytes;
      	 		m++;
      	 		if(atol(caOpt)==2){
      	 			utPltPutLoopVar(psDbHead,"dpname",lNum,"用户");
      	 		}
      	 		else{
      	 			utPltPutLoopVar(psDbHead,"dpname",lNum,"部门");
      	 		}
      	 		utPltPutLoopVar(psDbHead,"gsname",lNum,psData_g->caGsname);
      	 		utPltPutLoopVarF2(psDbHead,"num_s",lNum,m,"%d",m);
      	 		utPltPutLoopVar2(psDbHead,"dep_s",lNum,m,psData_g->caGroupname);
      	 		utPltPutLoopVar2(psDbHead,"dep_sd",lNum,m,psData_g->caDesc);
      	 		utPltPutLoopVarF2(psDbHead,"bytes",lNum,m,"%s",utStrLtoF8(psData_g->bytes,caTemp));

  //    	 		printf("gname=%s,sdesc=%s,bytes=%llu\n",psData_g->caGroupname,psData_g->caDesc,psData_g->bytes);
      	 	}
      	 	
      	 	
      	 	psData_g = (struct stData_g *)pasLHashNext(&sHashInfo);
      	}
         utPltPutLoopVarF(psDbHead,"ros",lNum,"%d",m+1);
         //输出百分比
         for(n=0;n<m;n++){
         	if(lSum>0){
         	   utPltPutLoopVarF2(psDbHead,"percent",lNum,n+1,"%.2f",100*((float)lSb[n]/lSum));
          }
          else{
          	utPltPutLoopVar2(psDbHead,"percent",lNum,n+1,"0.00");
        }
      }
     }
                
  }
  
}
  //统计网站访问  
  if(strlen(cheIDW)>0){  	
  	pHash_webclass = (unsigned char *)pasLHashInit(100,100,sizeof(struct stData),0,4);    
    if(pHash_webclass == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","分配内存出错","分配内存出错");
         return (-1);
    }
    pHash_webdata = (unsigned char *)pasLHashInit(1500,1500,sizeof(struct stData_g),0,4);    
  
    sprintf(sqlbuf,"select id,name from ncwebclass ");
      	 psCur=pasDbOpenSql(sqlbuf,0);
     if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0444取服务"),ncLang("0549数据库出错"));
            return 0;
        }
        lSid=0;
        memset(caSname,0,sizeof(caSname));       
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&lSid,
      	                             UT_TYPE_STRING,31,caSname);

       while(iReturn==0||iReturn==1405)
       {
       	
       	 psData= (struct stData *)pasLHashLookA(pHash_webclass,&lSid);  
       	 if(psData){       	
       	 	strcpy(psData->caSname,caSname);
       	 }  
       	
        lSid=0;
        memset(caSname,0,sizeof(caSname));       
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&lSid,
      	                             UT_TYPE_STRING,31,caSname);
       }
      pasDbCloseCursor(psCur);   
  
      sprintf(sqlbuf,"select userid,tid,sum(lCount) aa from nchttpcount where 1=1 ");
      if(strlen(caSdate1)>0){
     		sprintf(sqlbuf+strlen(sqlbuf)," and sdate>='%s' ",caSdate1);
    	}
	    if(strlen(caSdate2)>0){
	    	sprintf(sqlbuf+strlen(sqlbuf)," and sdate<='%s' ",caSdate2);
	    }
	    sprintf(sqlbuf+strlen(sqlbuf)," and tid in (%s)",cheIDW);
	    
	   if(strlen(caGroupid)>0){
    	 sprintf(sqlbuf+strlen(sqlbuf)," and userid in (select userid from ncuser where groupid in (%s))",caGroupid);
     }
	  if(strlen(caDispname)>0){
    	sprintf(sqlbuf+strlen(sqlbuf)," and userid in (select userid from ncuser where username like '%c%s%c' or dispname like '%c%s%c') ",'%',caDispname,'%','%',caDispname,'%');
    }
	    
	    
	    
      sprintf(sqlbuf+strlen(sqlbuf)," group by userid,tid ");
      
      printf("sqlbuf=%s\n",sqlbuf);
      psCur=pasDbOpenSql(sqlbuf,0);
      if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0444取服务"),ncLang("0549数据库出错"));
            return 0;
      }
          lUserid=0;
        lService=0;
        lBytes=0;
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&lUserid,
      	                             UT_TYPE_LONG,4,&lService,
      	                             UT_TYPE_LONG8,8,&lBytes);
      	                           
       while(iReturn==0||iReturn==1405)
       {
       	 psData = (struct stData *)pasLHashLookA(pHash_webclass,&lService);   
       	 if(psData){
       	 	 psUser = ncUtlGetUserContById(psShmHead,lUserid);
        	if(psUser) {
        		if(atol(caOpt)==2){
        			 psData_g = (struct stData_g *)pasLHashLookA(pHash_webdata,&lUserid);   
        		}
        		else{
        			 psData_g = (struct stData_g *)pasLHashLookA(pHash_webdata,&psUser->groupid);   
        		}
       	 	    
       	       if(psData_g){
       	       	psData_g->gsid=lService;
       	       	psData_g->bytes=psData_g->bytes+lBytes;
       	    if(atol(caOpt)==2){
       	    	strcpy(psData_g->caGroupname,psUser->dispname);
       	    }
       	    else{
       	       	strcpy(psData_g->caGroupname,psUser->psGroup->groupname);
       	      }
       	       	if((strstr(psData_g->caDesc,psData->caSname)==NULL)&&strlen(psData_g->caDesc)<1000){
       	       		sprintf(psData_g->caDesc+strlen(psData_g->caDesc),"%s、",psData->caSname);
       	       		
       	       	}
       	       }
       	 	     
       	 	 }

       	 }  
       
        lUserid=0;
        lService=0;
        lBytes=0;
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&lUserid,
      	                             UT_TYPE_LONG,4,&lService,
      	                             UT_TYPE_LONG8,8,&lBytes);
       }
      pasDbCloseCursor(psCur); 
  if(strlen(caExport)==0){    
      //输出到页面
         lNum=0;
          m=0;
          lSum=0;
         psData_g = (struct stData_g *)pasLHashFirst(pHash_webdata,&sHashInfo);
      	 while(psData_g){
      	
      	    lNum++;
      	 		if(m>=1000) break;
      	 		lSb[m]=psData_g->bytes;
      	 		lSum=lSum+psData_g->bytes;
      	 		m++;

      	 		utPltPutLoopVarF(psDbHead,"num_w",lNum,"%d",lNum);
      	 		utPltPutLoopVar(psDbHead,"dep_w",lNum,psData_g->caGroupname);
      	 		utPltPutLoopVar(psDbHead,"dep_wd",lNum,psData_g->caDesc);
      	 		utPltPutLoopVarF(psDbHead,"bytes_w",lNum,"%s",utStrLtoF8(psData_g->bytes,caTemp));

  //    	 		printf("gname=%s,sdesc=%s,bytes=%llu\n",psData_g->caGroupname,psData_g->caDesc,psData_g->bytes);
     	  	psData_g = (struct stData_g *)pasLHashNext(&sHashInfo);
     
        }
         utPltPutVarF(psDbHead,"row","%d",m+1);
        if(atol(caOpt)==2){
        	utPltPutVar(psDbHead,"dpname_w","用户");
        }
        else{
        	utPltPutVar(psDbHead,"dpname_w","部门");
        }
                 //输出百分比
         for(n=0;n<m;n++){
         	if(lSum>0){
         	   utPltPutLoopVarF(psDbHead,"percent_w",n+1,"%.2f",100*((float)lSb[n]/lSum));
          }
          else{
          	utPltPutLoopVar(psDbHead,"percent_w",n+1,"0.00");
        }
      }
             
   
  
		  if(strlen(cheIDW)>0){
		  	free(pHash_webclass);
		  	free(pHash_webdata);
		  }
		  if(strlen(cheIDS)>0){
		 		free(pHash_s);
		 		free(pHash_g);
		   }  
  
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/weekReport.htm"); 
    return 0;
    }
  }
  if(strlen(caExport)>0)
 {                                  //导出
  	
  			FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"ncreport%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
        	  if(strlen(cheIDW)>0){
					  	free(pHash_webclass);
					  	free(pHash_webdata);
					   }
					  if(strlen(cheIDS)>0){
					 		free(pHash_s);
					 		free(pHash_g);
					   }  
        	
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613网站访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
  	
  	 fprintf(fp,"上网行为管理统计报表(%s--%s)\n",caSdate1,caSdate2);
  	if(strlen(cheIDW)>0){
  		lCount=sepcharbydh(cheIDS,caObj); 
  		lNum=0;
  		  for(i=0;i<lCount;i++){
      	m=0;
      	lSum=0;
      	lGroupid=atol(caObj[i]);
      	 psData_g = (struct stData_g *)pasLHashFirst(pHash_g,&sHashInfo);
      	 while(psData_g){
      	 	if(lGroupid==psData_g->gsid){
      	 		if(m==0){
      	 			if(atol(caOpt)==2){
      	 				fprintf(fp,"序号,用户,%s,流量（字节）\n",psData_g->caGsname);
      	 			}
      	 			else{
      	 			fprintf(fp,"序号,部门,%s,流量（字节）\n",psData_g->caGsname);
      	 		 }
      	 			lNum++;
      	 			}
      	 		if(m>=1000) break;
      	 		m++;
  		       fprintf(fp,"%d,%s,%s,%llu\n",m,psData_g->caGroupname,psData_g->caDesc,psData_g->bytes);
  		      }
  		    psData_g = (struct stData_g *)pasLHashNext(&sHashInfo);
  		   }
  	 }
  	}
  	
  	if(strlen(cheIDW)>0){
  		   lNum=0;
          m=0;
          lSum=0;
         psData_g = (struct stData_g *)pasLHashFirst(pHash_webdata,&sHashInfo);
      	 while(psData_g){
  		   lNum++;
      	 if(m>=1000) break;
      	 if(m==0){
      	 	if(atol(caOpt)==2){
      	 		fprintf(fp,"序号,用户,主要请求网址类别,点击数\n");
      	 	}
      	 	else{
      	 	  fprintf(fp,"序号,部门,主要请求网址类别,点击数\n");
      	  }
      	}
     
      	fprintf(fp,"%d,%s,%s,%llu\n",lNum,psData_g->caGroupname,psData_g->caDesc,psData_g->bytes);
      	
  		psData_g = (struct stData_g *)pasLHashNext(&sHashInfo);
  	  }
  	}
  	
  	
  if(strlen(cheIDW)>0){
	  	free(pHash_webclass);
	  	free(pHash_webdata);
	  }
	 if(strlen(cheIDS)>0){
	 		free(pHash_s);
	 		free(pHash_g);
	  } 
	    fclose(fp);
      utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
      remove(caFile);
	   
  }
  return 0;
}
//将逗号分割的字符拆分成一个数组
int sepcharbydh(char *in,char caOut[200][12])
{
	char *p,*p2;
	long lCount;
	long m=0;
	lCount=0;
	if(strlen(in)==0) return 0;
	p=in;
	while(*p){
		if(*p==','){
			caOut[lCount][m]='\0';
			m=0;
			p++;
			lCount++;
			if(lCount>=200) return lCount;
		}
		else{
			if(m<12){
			caOut[lCount][m]=*p;
			p++;
			m++;
		  }
		}
	}
	return lCount+1;	
}