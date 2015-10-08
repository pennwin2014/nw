/* 监控软件---报表定制
                     Modified 2009/08/10
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



//7.5显示网络服务访问趋势图
int ncWebShowTrendsBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
  if(strlen(caGitem)==0)
		strcpy(caGitem,"100");

  utPltPutVar(psDbHead,"gitem",caGitem);          
  //printf("== caAv_un=%s ===\n",caAv_un);
  printf("== === caGitem=%s \n",caGitem);
   //将服务大类信息放在sGservice 中
   	 psCur=pasDbOpenSql("select groupid,name from ncservicecgroup order by groupid ",0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
     }
     lSumGindex=0;
     
     lGroupid=0;
     memset(caName,0,sizeof(caName));
     u=0;
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      while((iReturn==0)||(iReturn==1405)){     	
      	utPltPutLoopVarF(psDbHead,"gid",u+1,"%d",lGroupid);
      	utPltPutLoopVar(psDbHead,"gname",u+1,caName);
      	u++;
      	lGroupid=0;
        memset(caName,0,sizeof(caName));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                    	 UT_TYPE_STRING,30,caName);
      }                               
      pasDbCloseCursor(psCur); 
 		
  if(strlen(caGitem)==0) strcpy(caGitem,"1");
  if(strlen(caAv_un)==0) strcpy(caAv_un,"1");										//默认按分钟统计
  
	 if(atol(caAv_un)==1){									//按分钟统计(nciplog)
	    x_sum=15;
	    y_sum=15;
	    t_time=60;
	 }
	 else if(atol(caAv_un)==2){						  //按小时统计(nciplog)
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
  	t_time=60;
  }
	if((strlen(caStime)==0)&&(strlen(caSdate)==0)){											//默认没时间按系统时间，并按分钟统计
		    lTime=time(0);
		    strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));
		    strcpy(caShour,utTimFormat("%H",lTime));
		    strcpy(caSmin,utTimFormat("%M",lTime));
	//	    printf("caSmin=%s\n",caSmin);
	      if(atol(caAv_un)==1){
		    		lMin=(long)(atol(caSmin)/15)*15;
		    		sprintf(caSmin,"%2d",lMin);
//		   	  printf("lMin=%d\n",lMin);
		        sprintf(caSdate_all,"%s %s:%2d",caSdate,caShour,lMin);
		        lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		        lEndtime=lStarttime+900;	
		     }else if(atol(caAv_un)==2){
		     		strcpy(caSmin,"00");
		     		strcpy(caShour,"00");
		     		sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
		     		lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		     		lEndtime=lStarttime+24*3600;
		    }else{   
		    	  ncUtlMonthDate(lTime,caSdate,caEdate);	
		    	 	strcpy(caSmin,"00");
		     	  strcpy(caShour,"00");  
				 		lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
						lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate);                                                //按日统计
		    }
	}else if(strlen(caStime)>0){
		  	lStarttime=atol(caStime);
		  	strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		    strcpy(caShour,utTimFormat("%H",lStarttime));
		    strcpy(caSmin,utTimFormat("%M",lStarttime));
		    if(atol(caAv_un)==2){
		    	
		    	strcpy(caSmin,"00");
		     	strcpy(caShour,"00");
		     	sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
		     	lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		     	lEndtime=lStarttime+24*3600;		    	 	
		    }else if(atol(caAv_un)==3){
		    	ncUtlMonthDate(lStarttime,caSdate,caEdate);
		    	strcpy(caShour,"00");
		    	strcpy(caSmin,"00");
		    	lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
					lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate); 
		    }else{
		        lEndtime=lStarttime+900;
		   }
	}else if(strlen(caSdate)>0){
		  	if(atol(caAv_un)==1){
		  		lMin=(long)(atol(caSmin)/15)*15;
		  		sprintf(caSdate_all,"%s %s:%2d",caSdate,caShour,lMin);
		  		lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		      lEndtime=lStarttime+900;
		  	}else if(atol(caAv_un)==2){
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  		lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
					lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate); 
		  	}else if(atol(caAv_un)==3){
		  	 	lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);	
		  	 	ncUtlMonthDate(lStarttime,caSdate,caEdate);
		  	 	strcpy(caShour,"00");
		  	 	strcpy(caSmin,"00");
		  	 	lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
					lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate); 
		  	}
	}
//处理上一段和下一时间段		  
		if(atol(caPre_flag)==1){
		  	if(atol(caAv_un)==1){
		  		lStarttime=lStarttime-900;
		  		lEndtime=lEndtime-900;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		      strcpy(caShour,utTimFormat("%H",lStarttime));
		      strcpy(caSmin,utTimFormat("%M",lStarttime));
		  	}else if(atol(caAv_un)==2){
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
		  	}
	}else if(atol(caPre_flag)==2){
		 	  if(atol(caAv_un)==1){
		 			lStarttime=lStarttime+900;
		  		lEndtime=lEndtime+900;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		      strcpy(caShour,utTimFormat("%H",lStarttime));
		      strcpy(caSmin,utTimFormat("%M",lStarttime));
		    }else if(atol(caAv_un)==2){
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
		  	}
	}
 		utPltPutVar(psDbHead,"sdate",caSdate);
   	utPltPutVar(psDbHead,"shour",caShour);
   	utPltPutVar(psDbHead,"smin",caSmin);
   	utPltPutVar(psDbHead,"tid",caTid);  	
   	utPltPutVar(psDbHead,"avun",caAv_un);
		
    
    //utPltPutVarF(psDbHead,"stime1","%d",lStarttime);
    //utPltPutVarF(psDbHead,"etime1","%d",lEndtime);
    
    //strcpy(caSdate,utTimFormat("%Y/%m/%d %H:%M",lStarttime));
    //strcpy(caEdate,utTimFormat("%Y/%m/%d %H:%M",lEndtime));

    utPltPutVar(psDbHead,"tjsj",caSdate);
    //utPltPutVarF(psDbHead,"stime","%d",lEndtime);
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 		//获取统计时间段请求数最大的网址类别，将这些服务在图形上输出，别的服务归到其它类别中
                                     
       // 用户统计  	
  	if(atol(caGitem)==100){printf("==服务大类==\n");
  				 //按分钟平均
  			if(atol(caAv_un)==1){      
  				sprintf(caTemp,"select sum(ubytes+dbytes) aa,b.groupid,b.name from nciplog a,ncservicecgroup b,ncipindex c where a.vtime>=%d and a.vtime<%d and a.service=c.sid and c.groupid=b.groupid ",lStarttime,lEndtime);	
    			if(strlen(group_id)>0){
    				sprintf(caTemp+strlen(caTemp)," and (a.groupid =%s or a.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  		}else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");  				
  	   		sprintf(caTemp+strlen(caTemp)," group by b.name order by aa desc limit 0,500 ");
  		}else if(atol(caAv_un)==2){  
  	      //按小时统计用户
  	  		strcpy(caSdate,utTimFormat("%Y/%m/%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate)));
  	  		sprintf(caTemp,"select sum(ubytes+dbytes) aa,b.groupid,b.name from ncipservice a,ncservicecgroup b,ncuser c,ncipindex d where a.sdate='%s'  and a.userid=c.userid and a.service=d.sid and d.groupid=b.groupid ",caSdate);
  	  		if(strlen(group_id)>0){
  	  			sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id); 
		  	  }else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	   			sprintf(caTemp+strlen(caTemp)," group by b.name order by aa desc limit 0,10 ");
  		}else if(atol(caAv_un)==3){
  	       //按天统计用户
  				sprintf(caTemp,"select sum(ubytes+dbytes) aa,b.groupid,b.name from ncipservice a,ncservicecgroup b,ncuser c,ncipindex d where a.sdate>='%s' and a.sdate<='%s' and a.userid=c.userid and a.service=d.sid and d.groupid=b.groupid ",caSdate,caEdate);		
  				if(strlen(group_id)>0){
  					sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  		}else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");  	  
  	  		  sprintf(caTemp+strlen(caTemp)," group by b.name order by aa desc limit 0,10 ");
  		} 
	}else if(atol(caGitem)==101){		printf("==细分服务==\n");
      	 //按分钟平均
  		if(atol(caAv_un)==1){      
  				sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.service,c.name from nciplog a,ncipindex c where a.vtime>=%d and a.vtime<%d and a.service=c.sid ",lStarttime,lEndtime);	
    			if(strlen(group_id)>0){
    				sprintf(caTemp+strlen(caTemp)," and (a.groupid =%s or a.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  		}else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");  				
  	   		sprintf(caTemp+strlen(caTemp)," group by a.service order by aa desc limit 0,500 ");
  		}else if(atol(caAv_un)==2){  
  	      //按小时统计用户
  	  		strcpy(caSdate,utTimFormat("%Y/%m/%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate)));
  	  		sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.service,d.name from ncipservice a,ncuser c,ncipindex d where a.sdate='%s'  and a.userid=c.userid and a.service=d.sid ",caSdate);
  	  		if(strlen(group_id)>0){
  	  			sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id); 
		  	  }else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");
 	   			sprintf(caTemp+strlen(caTemp)," group by a.service order by aa desc limit 0,10 ");
  		}else if(atol(caAv_un)==3){
  	       //按天统计用户
  				sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.service,d.name from ncipservice a,ncuser c,ncipindex d where a.sdate>='%s' and a.sdate<='%s' and a.userid=c.userid and a.service=d.sid ",caSdate,caEdate);		
  				if(strlen(group_id)>0)
  					sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  		else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  		sprintf(caTemp+strlen(caTemp)," group by a.service order by aa desc limit 0,10 ");
  		}else{} 			    			
	}else if(atol(caGitem)==0){  			printf("==传统类别==\n");
				 //按分钟平均
  		if(atol(caAv_un)==1){      
  				sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.service,c.name from nciplog a,ncipindex c where a.vtime>=%d and a.vtime<%d and a.service=c.sid ",lStarttime,lEndtime);	
    			if(strlen(group_id)>0){
    				sprintf(caTemp+strlen(caTemp)," and (a.groupid =%s or a.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  		}else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");  
  	   		sprintf(caTemp+strlen(caTemp)," and (a.service=%d or a.service>100 and a.service<1000)",atol(caGitem));				
  	   		sprintf(caTemp+strlen(caTemp)," group by a.service order by aa desc limit 0,500 ");
  		}else if(atol(caAv_un)==2){  
  	      //按小时统计用户
  	  		strcpy(caSdate,utTimFormat("%Y/%m/%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate)));
  	  		sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.service,d.name from ncipservice a,ncuser c,ncipindex d where a.sdate='%s'  and a.userid=c.userid and a.service=d.sid ",caSdate);
  	  		if(strlen(group_id)>0){
  	  			sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id); 
		  	  }else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	   		sprintf(caTemp+strlen(caTemp)," and (a.service=%d or a.service>100 and a.service<1000)",atol(caGitem));	
 	   			sprintf(caTemp+strlen(caTemp)," group by a.service order by aa desc limit 0,10 ");
  		}else if(atol(caAv_un)==3){
  	       //按天统计用户
  				sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.service,d.name from ncipservice a,ncuser c,ncipindex d where a.sdate>='%s' and a.sdate<='%s' and a.userid=c.userid and a.service=d.sid ",caSdate,caEdate);		
  				if(strlen(group_id)>0){
  					sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  		}else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  		sprintf(caTemp+strlen(caTemp)," and (a.service=%d or a.service>100 and a.service<1000)",atol(caGitem));	
  	  		sprintf(caTemp+strlen(caTemp)," group by a.service order by aa desc limit 0,10 ");
  		}else{}  	   
  }else{					printf("==其它==\n");
  	  	 //按分钟平均
  		if(atol(caAv_un)==1){      
  				sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.service,c.name from nciplog a,ncipindex c where a.vtime>=%d and a.vtime<%d and a.service=c.sid ",lStarttime,lEndtime);	
    			if(strlen(group_id)>0){
    				sprintf(caTemp+strlen(caTemp)," and (a.groupid =%s or a.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  		}else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");  
  	   		for(t=0;t<80;t++){
						if(atol(caGitem)==t)
      				sprintf(caTemp+strlen(caTemp)," and (a.service=%d or a.service>=%d000 and a.service<%d000)",t,t,t+1);
      		}			
  	   		sprintf(caTemp+strlen(caTemp)," group by a.service order by aa desc limit 0,500 ");
  		}else if(atol(caAv_un)==2){  
  	      //按小时统计用户
  	  		strcpy(caSdate,utTimFormat("%Y/%m/%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate)));
  	  		sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.service,d.name from ncipservice a,ncuser c,ncipindex d where a.sdate='%s'  and a.userid=c.userid and a.service=d.sid ",caSdate);
  	  		if(strlen(group_id)>0){
  	  			sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id); 
		  	  }else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	   		for(t=0;t<80;t++){
						if(atol(caGitem)==t)
      				sprintf(caTemp+strlen(caTemp)," and (a.service=%d or a.service>=%d000 and a.service<%d000)",t,t,t+1);
      		}
 	   			sprintf(caTemp+strlen(caTemp)," group by a.service order by aa desc limit 0,10 ");
  		}else if(atol(caAv_un)==3){
  	       //按天统计用户
  				sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.service,d.name from ncipservice a,ncuser c,ncipindex d where a.sdate>='%s' and a.sdate<='%s' and a.userid=c.userid and a.service=d.sid ",caSdate,caEdate);		
  				if(strlen(group_id)>0){
  					sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  		}else if(strlen(caUserid)>0)
  	   			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				else
  	   			sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  		for(t=0;t<80;t++){
						if(atol(caGitem)==t)
      				sprintf(caTemp+strlen(caTemp)," and (a.service=%d or a.service>=%d000 and a.service<%d000)",t,t,t+1);
      		}
  	  		sprintf(caTemp+strlen(caTemp)," group by a.service order by aa desc limit 0,10 ");
  		}else{}        	
	}
	 printf("**111**caTemp=%s\n",caTemp);
     psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错2"));
        return 0;
     }
     lBytes=0;
     lService=0;
     memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lCount,
                                   UT_TYPE_LONG,4,&lService,
                                   UT_TYPE_STRING,30,caName);
                                  
    lSumindex=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	
    	
    		sGservice[lSumindex].lService=lService;
    		sGservice[lSumindex].lGroupid=0;
    		sGservice[lSumindex].lBytes=lCount;
    		strcpy(sGservice[lSumindex].caGname,caName);
    	for(i=0;i<32;i++){
    		sGservice[lSumindex].lBytesqs[i]=0;
    	}
    	
    
     lSumindex++;
     lBytes=0;
     lService=0;
     memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lCount,
                                   UT_TYPE_LONG,4,&lService,
                                   UT_TYPE_STRING,30,caName);
    }
    
    pasDbCloseCursor(psCur);
   
    
    //最后一个存放其它用户的流量
  
    	  sGservice[lSumindex].lService=99999;
    		sGservice[lSumindex].lGroupid=0;
    		sGservice[lSumindex].lBytes=0;
    		strcpy(sGservice[lSumindex].caGname,ncLang("0431其它"));
        for(i=0;i<32;i++){
    	    sGservice[lSumindex].lBytesqs[i]=0;
    	    
        }
        

  //按userid排序  为下面二分法查找作准备		
    		qsort(sGservice,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId2); 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if(atol(caGitem)==100){printf("==服务大类=22=\n");
  		if(atol(caAv_un)==1){                //按分钟平均		--->统计nciplog细表里 vtime:1243240009=2009-05-25 16:26:49=sdate(2009-05-25)+stime(16:26:49)
		 	  // 用户统计																																																
		    sprintf(caTemp,"select sum(ubytes+dbytes) aa,from_unixtime(a.vtime,'%ci') bb,b.groupid from nciplog a, ncservicecgroup b,ncipindex c where a.service=c.sid and c.groupid=b.groupid and a.vtime>=%d and a.vtime<=%d  ",'%',lStarttime,lEndtime);
	 				 if(strlen(group_id)>0){
	 				 	sprintf(caTemp+strlen(caTemp)," and (a.groupid =%s or a.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	   		 }else if(strlen(caUserid)>0)
  	   				sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  				 else
  	   				sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	   				sprintf(caTemp+strlen(caTemp)," group by bb,c.groupid order by aa desc ");
		 }else if(atol(caAv_un)==2){  
  	     //按小时统计用户     --->统计ncipservice时段表提高效率
  	    strcpy(caSdate,utTimFormat("%Y/%m/%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate)));
  	    sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.stime,b.groupid from ncipservice a,ncuser c, ncservicecgroup b,ncipindex d where a.service=d.sid and d.groupid=b.groupid and a.userid=c.userid and a.sdate='%s'  ",caSdate);
  	    if(strlen(group_id)>0){
  	    	sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	    }else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  		 sprintf(caTemp+strlen(caTemp)," group by a.stime,d.groupid  order by aa desc ");
     }else if(atol(caAv_un)==3){                    //按天统计用户
     		sprintf(caTemp,"select sum(ubytes+dbytes) aa,substring(a.sdate,9,2) bb,b.groupid from ncipservice a,ncuser c, ncservicecgroup b,ncipindex d where a.service=d.sid and d.groupid=b.groupid and  a.userid=c.userid and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
     		if(strlen(group_id)>0){
     			sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  	}else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");  	  	
  	  		 sprintf(caTemp+strlen(caTemp)," group by bb,d.groupid  order by aa desc ");
  	 }else{}
 }else if(atol(caGitem)==101){		printf("=22=细分服务==\n");
 		 if(atol(caAv_un)==1){                //按分钟平均		--->统计nciplog细表里 vtime:1243240009=2009-05-25 16:26:49=sdate(2009-05-25)+stime(16:26:49)
		 	  // 用户统计																																																
		    sprintf(caTemp,"select sum(ubytes+dbytes) aa,from_unixtime(vtime,'%ci') bb,service from nciplog where vtime>=%d and vtime<=%d  ",'%',lStarttime,lEndtime);
	 				 if(strlen(group_id)>0){
	 				 	sprintf(caTemp+strlen(caTemp)," and (groupid =%s or groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	   		 }else if(strlen(caUserid)>0)
  	   				sprintf(caTemp+strlen(caTemp)," and userid in (%s) ",caUserid);	
  				 else
  	   				sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	   				sprintf(caTemp+strlen(caTemp)," group by bb,service order by aa desc ");
		 }else if(atol(caAv_un)==2){  
  	     //按小时统计用户     --->统计ncipservice时段表提高效率
  	    strcpy(caSdate,utTimFormat("%Y/%m/%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate)));
  	    sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.stime,a.service from ncipservice a,ncuser c where a.userid=c.userid and a.sdate='%s'  ",caSdate);
  	    if(strlen(group_id)>0){
  	    	sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	    }else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  		 sprintf(caTemp+strlen(caTemp)," group by a.stime,a.service order by aa desc ");
     }else if(atol(caAv_un)==3){                    //按天统计用户
     		sprintf(caTemp,"select sum(ubytes+dbytes) aa,substring(a.sdate,9,2) bb,a.service from ncipservice a,ncuser c where a.userid=c.userid and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
     		if(strlen(group_id)>0){
     			sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  	}else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");  	  	
  	  		 sprintf(caTemp+strlen(caTemp)," group by bb,a.service  order by aa desc ");
  	 }else{}
 }else if(atol(caGitem)==0){  			printf("=22=传统类别==\n");
			if(atol(caAv_un)==1){                //按分钟平均		--->统计nciplog细表里 vtime:1243240009=2009-05-25 16:26:49=sdate(2009-05-25)+stime(16:26:49)
		 	  // 用户统计																																																
		    sprintf(caTemp,"select sum(ubytes+dbytes) aa,from_unixtime(vtime,'%ci') bb,service from nciplog where vtime>=%d and vtime<=%d  ",'%',lStarttime,lEndtime);
	 				 if(strlen(group_id)>0){
	 				 	sprintf(caTemp+strlen(caTemp)," and (groupid =%s or groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	   		 }else if(strlen(caUserid)>0)
  	   				sprintf(caTemp+strlen(caTemp)," and userid in (%s) ",caUserid);	
  				 else
  	   				sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	   		 sprintf(caTemp+strlen(caTemp)," and (service=%d or service>100 and service<1000)",atol(caGitem));	
  	   		 sprintf(caTemp+strlen(caTemp)," group by bb,service order by aa desc ");
		 	}else if(atol(caAv_un)==2){  
  	     //按小时统计用户     --->统计ncipservice时段表提高效率
  	    strcpy(caSdate,utTimFormat("%Y/%m/%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate)));
  	    sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.stime,a.service from ncipservice a,ncuser c where a.userid=c.userid and a.sdate='%s'  ",caSdate);
  	    if(strlen(group_id)>0){
  	    	sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	    }else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  	sprintf(caTemp+strlen(caTemp)," and (a.service=%d or a.service>100 and a.service<1000)",atol(caGitem));	
  	  	sprintf(caTemp+strlen(caTemp)," group by a.stime,a.service  order by aa desc ");
     }else if(atol(caAv_un)==3){                    //按天统计用户
     		sprintf(caTemp,"select sum(ubytes+dbytes) aa,substring(a.sdate,9,2) bb,a.service from ncipservice a,ncuser c where a.userid=c.userid and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
     		if(strlen(group_id)>0){
     			sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  	}else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");  
  	  	sprintf(caTemp+strlen(caTemp)," and (a.service=%d or a.service>100 and a.service<1000)",atol(caGitem));		  	
  	  	sprintf(caTemp+strlen(caTemp)," group by bb,a.service  order by aa desc ");	
		 }else{}   
  }else{					printf("==其它=22=\n");
  	 if(atol(caAv_un)==1){                //按分钟平均		--->统计nciplog细表里 vtime:1243240009=2009-05-25 16:26:49=sdate(2009-05-25)+stime(16:26:49)
		 	  // 用户统计																																																
		    sprintf(caTemp,"select sum(ubytes+dbytes) aa,from_unixtime(vtime,'%ci') bb,service from nciplog where vtime>=%d and vtime<=%d  ",'%',lStarttime,lEndtime);
	 			if(strlen(group_id)>0){
	 				 	sprintf(caTemp+strlen(caTemp)," and (groupid =%s or groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	   	}else if(strlen(caUserid)>0)
  	   				sprintf(caTemp+strlen(caTemp)," and userid in (%s) ",caUserid);	
  			else
  	   				sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	   	for(t=0;t<80;t++){
					if(atol(caGitem)==t)
      				sprintf(caTemp+strlen(caTemp)," and (service=%d or service>=%d000 and service<%d000)",t,t,t+1);
      	}		
  	   	sprintf(caTemp+strlen(caTemp)," group by bb,service order by aa desc ");
		 	}else if(atol(caAv_un)==2){  
  	     //按小时统计用户     --->统计ncipservice时段表提高效率
  	    strcpy(caSdate,utTimFormat("%Y/%m/%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate)));
  	    sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.stime,a.service from ncipservice a,ncuser c where a.userid=c.userid and a.sdate='%s'  ",caSdate);
  	    if(strlen(group_id)>0){
  	    	sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	    }else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  	for(t=0;t<80;t++){
						if(atol(caGitem)==t)
      				sprintf(caTemp+strlen(caTemp)," and (a.service=%d or a.service>=%d000 and a.service<%d000)",t,t,t+1);
      	}		
  	  	sprintf(caTemp+strlen(caTemp)," group by a.stime,a.service  order by aa desc ");
     }else if(atol(caAv_un)==3){                    //按天统计用户
     		sprintf(caTemp,"select sum(ubytes+dbytes) aa,substring(a.sdate,9,2) bb,a.service from ncipservice a,ncuser c where a.userid=c.userid and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
     		if(strlen(group_id)>0){
     			sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  	}else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");    	  		   	   			
  	  	for(t=0;t<80;t++){
						if(atol(caGitem)==t)
      				sprintf(caTemp+strlen(caTemp)," and (a.service=%d or a.service>=%d000 and a.service<%d000)",t,t,t+1);
      	}		
  	  	sprintf(caTemp+strlen(caTemp)," group by bb,a.service order by aa desc ");	
		 }else{}   	  	      	
	}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 	 
 
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
    unit_lBytes=lCount/t_time;
    while((iReturn == 0)||(iReturn==1405)) {
    	findex=atol(caTime);
    	if(atol(caAv_un)==1){
    		if(findex>=15&&findex<30){                           //后半分钟 时间值和数组下标相差30
    			findex=findex-15;
    		}else if(findex>=30&&findex<45){
    			findex=findex-30;
    		}else if(findex>=45){
    			findex=findex-45;
    		}
    	}   	
    	  index=ncUtlGetServicenumById(&sGservice,lSumindex,lService);
 //  	  printf("index=%d\n",index);
//    	  printf("findex=%d\n",findex);
	   	  if(index!=-1){	
	          sGservice[index].lBytesqs[findex]=sGservice[index].lBytesqs[findex]+lCount/t_time;         
	      }else{   
    	  	sGservice[lSumindex].lBytesqs[findex]=sGservice[lSumindex].lBytesqs[findex]+lCount/t_time; 
    	  }
       	//printf("sGservice[lSumindex].lBytesqs[findex]=%lld\n",sGservice[lSumindex].lBytesqs[findex]);
    lCount=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lCount,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
    }
    
    pasDbCloseCursor(psCur);
  long unitflag=0;
  //输出X轴刻度

	  for(mm=0;mm<x_sum;mm++){
	  	    utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
	  	    if(atol(caAv_un)==1){
             strcpy(caSmin,utTimFormat("%M",lStarttime));  
	  			   sprintf(caTemp,"%s:%02d",utTimFormat("%H",lStarttime),mm+atol(caSmin));	 
	  		  }
	  		  else if(atol(caAv_un)==2){
	  		  	sprintf(caTemp,"%02d:00",mm);
	  		  }
	  		  else if(atol(caAv_un)==3){
	  		  	sprintf(caTemp,"%s/%02d",utTimFormat("%m",lStarttime),mm+1);
	  		  }
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
	  		  //表格中输出统计数据
	  		  for(i=0;i<=lSumindex;i++){
	  				if(atol(caAv_un)==3){                                               //对于按日统计，数组从1开始，其余从0开始
	  			  	 if(unit_lBytes/1048576>9999){											//超过9Gb---》转换单位为Gb
	  			  	 	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.2f",(float)sGservice[i].lBytesqs[mm+1]/1048576/1024);  
	  			  	 	unitflag=2;
	  			  	 }else if(unit_lBytes/1024>99999){									//超过99Mb---》转换单位为Mb
	  			  	 	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm+1]/1048576);  
	  			  	 	unitflag=1;
	  			  	 }else	
	  			  	 	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm+1]/1024);  
	  				}else{  
	  					 if(unit_lBytes/1048576>9999){											//超过9Gb---》转换单位为Gb
	  			  	 	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.2f",(float)sGservice[i].lBytesqs[mm]/1048576/1024);  
	  			  	 	unitflag=2;
	  			  	 }else if(unit_lBytes/1024>99999){
	  			  	 	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm]/1048576);  
	  			  	 	unitflag=1;
	  			  	 }else					
	  			  	 	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm]/1024);  
	  		 		}
	  	  	}
	  	  	
	  }
	   
 //输出服务图形数据 

    for(i=0;i<=lSumindex;i++){
    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);
    	utPltPutLoopVar(psDbHead,"sevname",i+1,sGservice[i].caGname);
    	//utPltPutLoopVar(psDbHead,"sevname",i+1,ncUtlGetServiceName(psShmHead,sGservice[i].lService,ncLang("0431其它")));
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",sGservice[i].lService);
      
    	for(mm=0;mm<x_sum;mm++){
    		//printf("i=%d;mm=%d;service=%d;bytes=%lld\n",i,mm,sGservice[i].lService,sGservice[i].lBytesqs[mm]);
    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);

    		if(atol(caAv_un)==3){                                               //对于按日统计，数组从1开始，其余从0开始
    				if(unit_lBytes/1048576>9999)
    					utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%lld",sGservice[i].lBytesqs[mm+1]/1048576/1024);  
    				else if(unit_lBytes/1024>99999)
    					utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%lld",sGservice[i].lBytesqs[mm+1]/1048576);  
    				else
	  			  	utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%lld",sGservice[i].lBytesqs[mm+1]/1024);  
	  		}else{ 
	  				if(unit_lBytes/1048576>9999)
    					utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%lld",sGservice[i].lBytesqs[mm]/1048576/1024);  
    				else if(unit_lBytes/1024>99999)
    					utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%lld",sGservice[i].lBytesqs[mm]/1048576);  
    				else
    		    	utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%lld",sGservice[i].lBytesqs[mm]/1024);	
    		}
    }
  }
  utPltPutVarF(psDbHead,"sum","%d",lSumindex+1);
  utPltPutVarF(psDbHead,"sum_d","%d",x_sum);
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
   	   if(atol(caAv_un)==1){
             strcpy(caSmin,utTimFormat("%M",lStarttime));  
	  			   sprintf(caTemp,"%s:%02d",utTimFormat("%H",lStarttime),i+atol(caSmin));	 
	  	 }else if(atol(caAv_un)==2){
	  			sprintf(caTemp,"%02d:00",i);	  		  
	  	 }else if(atol(caAv_un)==3){	  			
	  			sprintf(caTemp,"%s/%02d",utTimFormat("%m",lStarttime),i+1);	  		 
	  	 }
   		  
   	   pChart[i].name=strdup(caTemp);
   	  
   	   pChart[i].dataset=malloc((lSumindex+2)*sizeof(float)) ;
   	   for(mm=0;mm<lSumindex+1;mm++){  	  	
   	  	 if(atol(caAv_un)==3){                             //对于按日统计数组从1开始，其余从0开始
   	  	  	if(unit_lBytes/1048576>9999)
    					pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i+1]/1048576/1024;  
    				else if(unit_lBytes/1024>99999)
    					pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i+1]/1048576;  
    				else
   	  	  		pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i+1]/1024;
   	  	 }else{
   	  	 		if(unit_lBytes/1048576>9999)
    					pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i]/1048576/1024;  
    				else if(unit_lBytes/1024>99999)
    					pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i]/1048576;  
    				else   	  	  		
   	  	 			pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i]/1024;   	  
   	 	 	 }	
   	 	 } 
   	 }

      rowrow=x_sum;
      column=lSumindex+1;
   
    strcpy(caTitle,ncLang("1941网络服务流量速率访问趋势图"));
    sprintf(filename,"%s.PDF",caTitle); 

    sprintf(caStatdate,ncLang("1962统计时间:%s %s:%s"),caSdate,caShour,caSmin);
		if(unitflag==1)
			sprintf(caTemp,ncLang("1942单位(%s)"),"MBytes/S");
		else if(unitflag==2)	
			sprintf(caTemp,ncLang("1942单位(%s)"),"GBytes/S");
		else
    	sprintf(caTemp,ncLang("1942单位(%s)"),"KBytes/S");
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
 		
		if(unitflag==1)			
			strcpy(unit_page,"MBytes/S");
		else if(unitflag==2)	
			strcpy(unit_page,"GBytes/S");
		else
			strcpy(unit_page,"KBytes/S");

    utPltPutVar(psDbHead,"unit_page",unit_page);
	  utPltPutVar(psDbHead,"groupname",gpname);
	  utPltPutVar(psDbHead,"dispname",dpname);
	  utPltPutVar(psDbHead,"userid",caUserid);
	  utPltPutVar(psDbHead,"groupid",group_id);
	  utPltPutVar(psDbHead,"rootname",rtname);
	  
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_dept/iframe_trends_webclassbytime.htm");
    
    return 0;
}
