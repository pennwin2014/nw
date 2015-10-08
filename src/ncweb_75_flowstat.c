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


struct Service_s {
        unsigned long service;
        unsigned long long ubytes;
        unsigned long long dbytes;      //
        float rate;
}; 

struct flow_s{
    	long long ubytes;
    	long long dbytes;
};

static char *convert(const char *codefrom, const char *codeto, const char *src)
{

	return 0;
}

int draw_trends(struct flow_s *st_flow, char **caXX, int lcount, char *fontname, char *filename, long minup, long mindown,
                long maxup, long maxdown,  float avgup, float avgdown,   float avg96up,  float avg96down,
                char *caSdate, char *caEdate,long lBand,long lDband)
{
	
	
  
  return 0;
} 




//排序  按UP流量排序
static int ncUtlSortOnUp(char *p1,char *p2)
{
    int i;
    long long l;
   
    struct Service_s psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct Service_s));
    memcpy(&psC2,p2,sizeof(struct Service_s));    
    l=psC2.ubytes-psC1.ubytes;
    if(l>0) return 1;
    if(l<0) return -1;
    if(l==0) return 0;
  
}


//排序  按Down流量排序
static int ncUtlSortOnDown(char *p1,char *p2)
{
    int i;
    long long l;
   
    struct Service_s psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct Service_s));
    memcpy(&psC2,p2,sizeof(struct Service_s));    
    l=psC2.dbytes-psC1.dbytes;
    if(l>0) return 1;
    if(l<0) return -1;
    if(l==0) return 0;
  
}

//部门带宽排序
static int orderflag,dsflag;            
static int ncUtlSortOnDepband(char *p1,char *p2)
{
    int i;
    long long l;
   
    struct Service_s psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct Service_s));
    memcpy(&psC2,p2,sizeof(struct Service_s));   
    
    if(dsflag==0){
    	if(orderflag==1){
    		l=psC2.dbytes+psC2.ubytes-psC1.dbytes-psC1.ubytes;
    	}
    	else if(orderflag==2){
    		l=psC2.ubytes-psC1.ubytes;
    	}
    	else if(orderflag==3){
    		l=psC2.dbytes-psC1.dbytes;
    	}
    	else{
    		l=psC2.dbytes+psC2.ubytes-psC1.dbytes-psC1.ubytes;
    	}
    }
    else{
    	
    	  if(orderflag==1){
    			l=psC1.dbytes+psC1.ubytes-psC2.dbytes-psC2.ubytes;
    	}
    	else if(orderflag==2){
    		l=psC1.ubytes-psC2.ubytes;
    	}
    	else if(orderflag==3){
    		l=psC1.dbytes-psC2.dbytes;
    	}
    	else{
    		l=psC1.dbytes+psC1.ubytes-psC2.dbytes-psC2.ubytes;
    	}
    }
    
     
  
    
    
    
    if(l>0) return 1;
    if(l<0) return -1;
    if(l==0) return 0;
  
}



int getMinNum(int lStart,int lTime){
	return (int)(lTime-lStart)/120;
 }
int getHourNum(int lStart,int lTime){
	return (int)(lTime-lStart)/3600;
 }

//7.5显示流量综合统计报表
int ncWebShowAllFlowStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    pasDbCursor *psCur;  
    //char caFlag[8];
    char caAv_un[8];  
    int      iReturn,i;
    long lTime; 
    char caTemp[2048]; 
    char caName[32];
    char caStime[16]; 
    long y_sum; 
    int iNum;
    int mm;
    //char caTime[16];
    //long unit;
    //long lSid,lConntime;
    //long long lMax=0;
    long x_sum;              //index  X轴下标   x_sum X轴下标总数   
    //long count_d=0;                //Y轴数据数  
    char caSdate[20]; 
    //long long lBytes;
    long lService,lGroupid;
    long lSumindex;

    char caEdate[20];  
    //long lCount;                            
    char caSdate_all[20];                                    

    char group_id[15],caUserid[15];
    char caPdf[12];

    //int rowrow;                     
    //int column; 
    //char caTitle[128];

    //long long unit_lBytes;
    
    char cawSdate[12],cawEdate[12],catDate[12];
    char caEtime[12];
    
    struct flow_s stFlow[1440];
    long long lUbytes,lDbytes;
    //char caDate[16];
    long lStime,lEtime;
    long y_tot;
    struct Service_s  stService[500];                  //存放网络服务数据
    struct Service_s  stService_data[6][10];          //我的数据
    long long lTotubytes,lTotdbytes;
    char caService[64];
    long scal;   
    long lBand,lDband;
    long long lMinup,lMindown,lMaxup,lMaxdown,lRat,lAvup96,lAvdown96,lAvup,lAvdown;
    memset(caUserid,0,sizeof(caUserid));
    memset(&stFlow,0,1440*sizeof(struct flow_s));
    utMsgPrintMsg(psMsgHead);
    utMsgGetSomeNVar(psMsgHead,7,"stime",    UT_TYPE_STRING,10,caStime,				//
   		                            "sdate",    UT_TYPE_STRING,10,caSdate,
   		                            "edate",    UT_TYPE_STRING,10,caEdate,
   		                            "etime",    UT_TYPE_STRING,10,caEtime,
   		                            "av_un",    UT_TYPE_STRING,8,caAv_un,					//报表类型1日报，2.周报
																	"groupid",  UT_TYPE_STRING,14,group_id,
																  "pdf",      UT_TYPE_STRING,8,caPdf);
																  
	//获取某部门分配的带宽
	if(strlen(group_id)>0){
		sprintf(caTemp,"select band,dband from ncgroupband where gid=%s ",group_id);
	}
	else{
		sprintf(caTemp,"select band,dband from ncgroupband where gid=	999999 ");
	}
	printf("caTemp = %s\n", caTemp);
	lBand=0;
	lDband=0;
	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lBand,
	                        UT_TYPE_LONG,4,&lDband);														 
    
 	                   
  psDbHead = utPltInitDb();
  
  if(lBand==0) lBand=2048;
  if(lDband==0) lDband=2048;
   if(strlen(caAv_un)==0) strcpy(caAv_un,"1");										//默认按分钟统计 日报
  if(strlen(caAv_un)==0){
  	strcpy(caAv_un,"1");
  }
   
  lTime=time(0)-24*3600;
  
  ncUtlWeekDate(lTime,cawSdate,cawEdate);
  strcpy(catDate,utTimFormat("%Y/%m/%d",lTime));
  //输出星期开始日和结束日
  utPltPutVar(psDbHead,"wsdate",cawSdate);
  utPltPutVar(psDbHead,"wedate",cawEdate);
  utPltPutVar(psDbHead,"tdate",catDate);
  
  if(strlen(caSdate)==0){
  	strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));
  }
//  if(strlen(caEdate)==0){
//  	strcpy(caEdate,utTimFormat("%Y/%m/%d",lTime));
//  }
  if(strlen(caStime)==0){
  	strcpy(caStime,"00:00");
  }
//  if(strlen(caEtime)==0){
//  	strcpy(caEtime,"23:59");
//  }
  sprintf(caSdate_all,"%s %s",caSdate,caStime);
  lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
  if(atol(caAv_un)==1){
 		lEtime=lStime+86340;
 		y_tot=720;
  }
  else{
  	lEtime=lStime+86400*7;
  	y_tot=168;
  }
  strcpy(caEdate,utTimFormat("%Y/%m/%d",lEtime));
  strcpy(caEtime,utTimFormat("%H:%M",lEtime));
  
  
  psCur=pasDbOpenSql("select groupid,groupname from ncgroup order by groupid ",0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
     }
  iNum=0;
  
      iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      while((iReturn==0)||(iReturn==1405)){     	
      	utPltPutLoopVarF(psDbHead,"gid",iNum+1,"%d",lGroupid);
      	utPltPutLoopVar(psDbHead,"gname",iNum+1,caName);
      	iNum++;
      	lGroupid=0;
        memset(caName,0,sizeof(caName));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                    	 UT_TYPE_STRING,30,caName);
      }                               
      pasDbCloseCursor(psCur); 
  
  
  
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"stime",caStime);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"groupid",group_id);
  	utPltPutVar(psDbHead,"avun",caAv_un);
    utPltPutVarF(psDbHead,"upband","%.2f",(float)lBand/1024);
    utPltPutVarF(psDbHead,"downband","%.2f",(float)lDband/1024);
  
  if(strlen(group_id)>0){
  	utPltPutVar(psDbHead,"groupname",ncUtlGetGroupNameByid(psShmHead,atol(group_id),"\0"));
  }
  else{
//  	utPltPutVar(psDbHead,"groupname","全部");
      utPltPutVar(psDbHead,"groupname",utComGetVar_sd(psShmHead,"rootname",ncLang("0269公司")));
  }
 
  
	 if(atol(caAv_un)==1){									//按分钟统计
	 	scal=2;
	    x_sum=25;
	    y_sum=25;
	    utPltPutVar(psDbHead,"title","一分钟平均");
	 }
	 else if(atol(caAv_un)==2){						  //按小时统计(周报)
	 	x_sum=8;
	// 	y_sum=24;
	 	scal=60;
	 	 utPltPutVar(psDbHead,"title","一小时平均");
	}
  else{
  	scal=2;
  }

  
  //按分钟平均
   
  				sprintf(caTemp,"select ubytes,dbytes,rtime,conntime  from ncflowloggroup where 1=1 ");
  				sprintf(caTemp+strlen(caTemp)," and rtime>=%lu and rtime<=%lu ",lStime,lEtime);
  				if(strlen(group_id)>0){
  					sprintf(caTemp+strlen(caTemp)," and (groupid=%s or groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  				}
  				
  		
  				
  			
  		

//	 printf("**111**caTemp=%s\n",caTemp);
     psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错2"));
        return 0;
     }
     lUbytes=0;
     lDbytes=0;
     lTime=0;
     unsigned long lConntime=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
                                   UT_TYPE_LONG8,8,&lDbytes,
                                   UT_TYPE_LONG,4,&lTime,
                                   UT_TYPE_LONG,4,&lConntime);
                                  
                                  
    lSumindex=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	if(atol(caAv_un)==1){
    	lSumindex=getMinNum(lStime,lTime);
      }
      else{
      	lSumindex=getHourNum(lStime,lTime);
      }
    	if(lConntime>0){
    	stFlow[lSumindex].ubytes=stFlow[lSumindex].ubytes+lUbytes/(scal*lConntime);
    	stFlow[lSumindex].dbytes=stFlow[lSumindex].dbytes+lDbytes/(scal*lConntime);
    	
      }
    
     lUbytes=0;
     lDbytes=0;
     lTime=0;
     lConntime=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
                                   UT_TYPE_LONG8,8,&lDbytes,
                                   UT_TYPE_LONG,4,&lTime,
                                   UT_TYPE_LONG,4,&lConntime);
    }
    
    pasDbCloseCursor(psCur);
   
   
  
  //输出X轴刻度
  char *caXX[25];

	for(mm=0;mm<x_sum;mm++){
	  	    utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
	  	    if(atol(caAv_un)==1){  	    	
             strcpy(caTemp,utTimFormat("%H:%M",lStime+mm*3600));  
             
	  		  }
	  		  else{
	  		  	strcpy(caTemp,utTimFormat("%m-%d",lStime+mm*24*3600));
	  		  }
	  		  caXX[mm] = strdup(caTemp);			
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);

	  	  	
	   }
	   
 //输出图形数据 
 lMinup=0;
 lMindown=0;
 lMaxup=0;
 lMaxdown=0;
 lRat=0;
 lAvup96=0;
 lAvdown96=0;
 lAvup=0;
 lAvdown=0;
 for(i=0;i<y_tot;i++){
 	utPltPutLoopVarF(psDbHead,"num2",i+1,"%d",i);
 	
 if(lBand>0){
 	lRat=(100*8*stFlow[i].ubytes)/(lBand*1024);
 	lAvup=lAvup+lRat;
 	if(lMinup>lRat)
 	   lMinup=lRat;
 	 
 	 if(lMaxup<lRat)
 	   lMaxup=lRat;  
 	
 	 if(lRat>=96)
 	    lAvup96++;
 	    
 	 lRat= (100*8*stFlow[i].dbytes)/(lDband*1024);  
 	 lAvdown=lAvdown+lRat;
 	 if(lMindown>lRat)
 	   lMindown=lRat;
 	 
 	 if(lMaxdown<lRat)
 	   lMaxdown=lRat;  
 	
 	 if(lRat>=96)
 	    lAvdown96++;  
 	    
 	    
 	    
 	    
 	utPltPutLoopVarF(psDbHead,"ubytes",i+1,"%lld",(100*8*stFlow[i].ubytes)/(lBand*1024));
	}
	else{
	 	utPltPutLoopVarF(psDbHead,"ubytes",i+1,"%lld",0);
	}
	if(lDband>0){
		utPltPutLoopVarF(psDbHead,"dbytes",i+1,"%lld",(100*8*stFlow[i].dbytes)/(lDband*1024));
	}
	else{
 	  utPltPutLoopVarF(psDbHead,"dbytes",i+1,"%lld",stFlow[i].dbytes);
 	}
 }

//输出上行下行速率最大，最小，96%，平均
utPltPutVarF(psDbHead,"minup","%lld",lMinup);
utPltPutVarF(psDbHead,"mindown","%lld",lMindown);
utPltPutVarF(psDbHead,"maxup","%lld",lMaxup);
utPltPutVarF(psDbHead,"maxdown","%lld",lMaxdown);
utPltPutVarF(psDbHead,"avup96","%0.2f",(float)(100*lAvup96)/y_tot);
utPltPutVarF(psDbHead,"avdown96","%0.2f",(float)(100*lAvdown96)/y_tot);
utPltPutVarF(psDbHead,"avup","%0.2f",(float)lAvup/y_tot);
utPltPutVarF(psDbHead,"avdown","%0.2f",(float)lAvdown/y_tot);










//统计网络服务
  sprintf(caTemp,"select sum(ubytes),sum(dbytes),service from ncipservice where 1=1");
  if(strcmp(caSdate,caEdate)!=0){
  	sprintf(caTemp+strlen(caTemp)," and ((sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' ) )  ", caSdate,caEdate,caSdate,caStime,caEtime,caEtime);
  }
	else{
		sprintf(caTemp+strlen(caTemp)," and (sdate='%s' and stime>='%s' and stime<='%s' )",caSdate,caStime,caEtime);		
	}
  if(strlen(group_id)>0){
  		sprintf(caTemp+strlen(caTemp)," and (userid in (select userid from ncuser where groupid=%s ))",group_id);
  }
	
	sprintf(caTemp+strlen(caTemp)," group by service limit 0,500");
	
//	printf("caTemp=%s\n",caTemp);
	
	   psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错2"));
        return 0;
     }
     lUbytes=0;
     lDbytes=0;
     lService=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
                                   UT_TYPE_LONG8,8,&lDbytes,
                                   UT_TYPE_LONG,4,&lService);
                                  
                                  
    lSumindex=0;
    lTotubytes=0;
    lTotdbytes=0;
    while((iReturn == 0)||(iReturn==1405)) {      
    	stService[lSumindex].ubytes=lUbytes;
    	stService[lSumindex].dbytes=lDbytes;
      stService[lSumindex].service=lService;
      lTotubytes=lTotubytes+lUbytes;
      lTotdbytes=lTotdbytes+lDbytes;
    lSumindex++;
     lUbytes=0;
     lDbytes=0;
     lService=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
                                   UT_TYPE_LONG8,8,&lDbytes,
                                   UT_TYPE_LONG,4,&lService);
    }
    
    pasDbCloseCursor(psCur);
	
	
///111111111111111111
	if(lSumindex>0){
		//按UP流量排序
		qsort(stService,lSumindex,sizeof(struct Service_s),ncUtlSortOnUp); 
	}
	iNum=0;
	//输出网络服务上行流量
	long long lUptop=0;
	for(i=0;i<lSumindex&&iNum<10;i++){
	   strcpy(caService,ncUtlGetServiceName(psShmHead,stService[i].service,"\0"));
	   if(strlen(caService)>0){
	   	lUptop=lUptop+stService[i].ubytes;
	   	
	   	
	   	iNum++;
		utPltPutLoopVarF(psDbHead,"aubyts",iNum,"%0.2f",(float)stService[i].ubytes/(1024*1024));
		utPltPutLoopVar(psDbHead,"serviceup",iNum,ncUtlGetServiceName(psShmHead,stService[i].service,"\0"));
		stService_data[0][iNum-1].ubytes = stService[i].ubytes;//
		stService_data[0][iNum-1].service = stService[i].service;//
		if(lTotubytes>0){
			utPltPutLoopVarF(psDbHead,"uprate",iNum,"%.2f",100*(float)stService[i].ubytes/lTotubytes);
			stService_data[0][iNum-1].rate = 100*(float)stService[i].ubytes/lTotubytes;//
		}
		else{
			utPltPutLoopVar(psDbHead,"uprate",iNum,"0.0");
			stService_data[0][iNum-1].rate = 0.0;//
		}
   }

	}
	
	int num1 = iNum;

	
	
	
	
	
	///22222222222222
		if(lSumindex>0){
		//按Down流量排序
		qsort(stService,lSumindex,sizeof(struct Service_s),ncUtlSortOnDown); 
	}
	int iNum_down=0;
	//输出网络服务下行流量
	long long lDowntop=0;
	for(i=0;i<lSumindex&&iNum_down<10;i++){
	   strcpy(caService,ncUtlGetServiceName(psShmHead,stService[i].service,"\0"));
	   if(strlen(caService)>0){
	   	lDowntop=lDowntop+stService[i].dbytes;
	   	iNum_down++;
		utPltPutLoopVarF(psDbHead,"adbyts",iNum_down,"%0.2f",(float)stService[i].dbytes/(1024*1024));
		utPltPutLoopVar(psDbHead,"servicedown",iNum_down,ncUtlGetServiceName(psShmHead,stService[i].service,"\0"));
		stService_data[1][iNum_down-1].dbytes = stService[i].dbytes;//
    stService_data[1][iNum_down-1].service = stService[i].service;//
		if(lTotdbytes>0){
			utPltPutLoopVarF(psDbHead,"drate",iNum_down,"%.2f",100*(float)stService[i].dbytes/lTotdbytes);
			stService_data[1][iNum_down-1].rate = 100*(float)stService[i].dbytes/lTotdbytes;//
		}
		else{
			utPltPutLoopVar(psDbHead,"drate",iNum_down,"0.0");
			stService_data[1][iNum_down-1].rate = 0.0;//
		}
   }

	}
	
  int num2 = iNum_down;
	
	
	//统计目标IP流量排行
		
	 sprintf(caTemp,"select sum(ubytes) aa,sum(dbytes),ip from ncipcount where 1=1");
  if(strcmp(caSdate,caEdate)!=0){
  	sprintf(caTemp+strlen(caTemp)," and ((sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' ) )  ", caSdate,caEdate,caSdate,caStime,caEtime,caEtime);
  }
	else{
		sprintf(caTemp+strlen(caTemp)," and (sdate='%s' and stime>='%s' and stime<='%s' )",caSdate,caStime,caEtime);		
	}
  if(strlen(group_id)>0){
  		sprintf(caTemp+strlen(caTemp)," and (userid in (select userid from ncuser where groupid=%s ))",group_id);
  }
	
	sprintf(caTemp+strlen(caTemp)," group by ip order by aa desc ");
	
//	printf("caTemp=%s\n",caTemp);
struct Service_s *pFlow;	
unsigned long lIp;
long ipcount;
long memnum,iNumip,iNumip_down;
long long lTotIpubytes,lTotIpdbytes,lDowntopip,lUptopip;
	   psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错2"));
        return 0;
     }
 pFlow=(struct Service_s *)malloc(1024*sizeof(struct Service_s));
     lUbytes=0;
     lDbytes=0;
     lIp=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
                                   UT_TYPE_LONG8,8,&lDbytes,
                                   UT_TYPE_ULONG,4,&lIp);
                                  
                                  
   ipcount=0;
   lTotIpubytes=0;
   lTotIpdbytes=0;
    while((iReturn == 0)||(iReturn==1405)) { 
    	
    	  
    	pFlow[ipcount].ubytes=lUbytes;
    	pFlow[ipcount].dbytes=lDbytes;
      pFlow[ipcount].service=lIp;
      lTotIpubytes=lTotIpubytes+lUbytes;
      lTotIpdbytes=lTotIpdbytes+lDbytes;
      ipcount++;
      
      if(ipcount%1000==0){
      	memnum=ipcount/1000;
      	pFlow=(struct Service_s*)realloc(pFlow,1024*(memnum+1)*sizeof(struct Service_s));
      }
      
     lUbytes=0;
     lDbytes=0;
     lIp=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
                                   UT_TYPE_LONG8,8,&lDbytes,
                                   UT_TYPE_ULONG,4,&lIp);
    }
    
    pasDbCloseCursor(psCur);
	
	///3333333333333333333333
//输出目标IP排行上行	
iNumip=0;
lUptopip=0;
lDowntopip=0;
		for(i=0;i<ipcount&&iNumip<10;i++){
	   	lUptopip=lUptopip+pFlow[i].ubytes;
	   	iNumip++;
		utPltPutLoopVarF(psDbHead,"ipubyts",iNumip,"%0.2f",(float)pFlow[i].ubytes/(1024*1024));
		utPltPutLoopVar(psDbHead,"dip",iNumip,utComHostIp(htonl(pFlow[i].service)));
		stService_data[2][iNumip-1].ubytes = pFlow[i].ubytes;//
    stService_data[2][iNumip-1].service = htonl(pFlow[i].service);//dip;
		if(lTotIpubytes>0){
			utPltPutLoopVarF(psDbHead,"ipurate",iNumip,"%.2f",100*(float)pFlow[i].ubytes/lTotIpubytes);
			stService_data[2][iNumip-1].rate = 100*(float)pFlow[i].ubytes/lTotIpubytes;
		}
		else{
			utPltPutLoopVar(psDbHead,"ipurate",iNumip,"0.0");
			stService_data[2][iNumip-1].rate = 0.0;
		}

	}
	 int num3 = iNumip;
		///44444444444444444444
	//输出目标IP排行流入排行榜
 if(ipcount>0){
		//按Down流量排序
		qsort(pFlow,ipcount,sizeof(struct Service_s),ncUtlSortOnDown); 
	}
	
	iNumip_down=0;
		for(i=0;i<ipcount&&iNumip_down<10;i++){
	   	lDowntopip=lDowntopip+pFlow[i].dbytes;
	   	iNumip_down++;
		utPltPutLoopVarF(psDbHead,"ipdbyts",iNumip_down,"%0.2f",(float)pFlow[i].dbytes/(1024*1024));
		utPltPutLoopVar(psDbHead,"dip_d",iNumip_down,utComHostIp(htonl(pFlow[i].service)));
		
		
		stService_data[3][iNumip_down-1].dbytes = pFlow[i].dbytes;//
		stService_data[3][iNumip_down-1].service = htonl(pFlow[i].service);//dip;
    
		if(lTotIpdbytes>0){
			utPltPutLoopVarF(psDbHead,"ipdrate",iNumip_down,"%.2f",100*(float)pFlow[i].dbytes/lTotIpdbytes);
			stService_data[3][iNumip_down-1].rate = 100*(float)pFlow[i].dbytes/lTotIpdbytes;
			
		}
		else{
			utPltPutLoopVar(psDbHead,"ipdrate",iNumip_down,"0.0");
			stService_data[3][iNumip_down-1].rate = 0.0;
		}

	}
	
	int num4 = iNumip_down;
	free(pFlow);
	
	
	
	
	
	
	
	//统计源IP流量排行
		
	 sprintf(caTemp,"select sum(ubytes) aa,sum(dbytes),userid from ncipcount where 1=1");
  if(strcmp(caSdate,caEdate)!=0){
  	sprintf(caTemp+strlen(caTemp)," and ((sdate > '%s' and sdate < '%s') or (sdate='%s' and stime>='%s' ) or (sdate='%s' and stime<='%s' ) )  ", caSdate,caEdate,caSdate,caStime,caEtime,caEtime);
  }
	else{
		sprintf(caTemp+strlen(caTemp)," and (sdate='%s' and stime>='%s' and stime<='%s' )",caSdate,caStime,caEtime);		
	}
  if(strlen(group_id)>0){
  		sprintf(caTemp+strlen(caTemp)," and (userid in (select userid from ncuser where groupid=%s ))",group_id);
  }
	
	sprintf(caTemp+strlen(caTemp)," group by userid order by aa desc ");
	
//	printf("caTemp=%s\n",caTemp);
	
unsigned long lUserid;
long usercount;
long iNumuser,iNumuser_down;
long long lTotUserubytes,lTotUserdbytes,lDowntopuser,lUptopuser;
ncUserCont *psUser = NULL;

	   psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587图表统计"),ncLang("0549数据库出错2"));
        return 0;
     }
 pFlow=(struct Service_s *)malloc(1024*sizeof(struct Service_s));
     lUbytes=0;
     lDbytes=0;
     lUserid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
                                   UT_TYPE_LONG8,8,&lDbytes,
                                   UT_TYPE_ULONG,4,&lUserid);
                                  
                                  
   usercount=0;
   lTotUserubytes=0;
   lTotUserdbytes=0;
    while((iReturn == 0)||(iReturn==1405)) { 
    	
    	  
    	pFlow[usercount].ubytes=lUbytes;
    	pFlow[usercount].dbytes=lDbytes;
      pFlow[usercount].service=lUserid;
      lTotUserubytes=lTotUserubytes+lUbytes;
      lTotUserdbytes=lTotUserdbytes+lDbytes;
      usercount++;
      
      if(usercount%1000==0){
      	memnum=usercount/1000;
      	pFlow=(struct Service_s*)realloc(pFlow,1024*(memnum+1)*sizeof(struct Service_s));
      }
      
     lUbytes=0;
     lDbytes=0;
     lUserid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
                                   UT_TYPE_LONG8,8,&lDbytes,
                                   UT_TYPE_ULONG,4,&lUserid);
    }
    
    pasDbCloseCursor(psCur);
	
		///5555555555555555555555
//输出源IP排行上行	
iNumuser=0;
lUptopuser=0;
lDowntopuser=0;
		for(i=0;i<usercount&&iNumuser<10;i++){
			 psUser = (ncUserCont *)ncUtlBufGetUserInfo(psShmHead,pFlow[i].service);
			 if(psUser){
			 	
			
	   	lUptopuser=lUptopuser+pFlow[i].ubytes;
	   	iNumuser++;
		utPltPutLoopVarF(psDbHead,"userubyts",iNumuser,"%0.2f",(float)pFlow[i].ubytes/(1024*1024));
		utPltPutLoopVar(psDbHead,"useruip",iNumuser,utComHostIp(htonl(psUser->ip)));
		stService_data[4][iNumuser-1].ubytes = pFlow[i].ubytes;//
    stService_data[4][iNumuser-1].service = htonl(psUser->ip);//ip;
		if(lTotUserubytes>0){
			utPltPutLoopVarF(psDbHead,"userurate",iNumuser,"%.2f",100*(float)pFlow[i].ubytes/lTotUserubytes);
			stService_data[4][iNumuser-1].rate = 100*(float)pFlow[i].ubytes/lTotUserubytes;
		}
		else{
			utPltPutLoopVar(psDbHead,"userurate",iNumip,"0.0");
			stService_data[4][iNumuser-1].rate = 0.0;
		}
   }
	}
	int num5 = iNumuser;
	
	///66666666666666666666666666666666
	//输出源IP排行下行流量排行榜
 if(usercount>0){
		//按Down流量排序
		qsort(pFlow,usercount,sizeof(struct Service_s),ncUtlSortOnDown); 
	}
	
	iNumuser_down=0;
		for(i=0;i<usercount&&iNumuser_down<10;i++){
			 psUser = (ncUserCont *)ncUtlBufGetUserInfo(psShmHead,pFlow[i].service);
			 if(psUser){
			 	
			
	   	lDowntopuser=lDowntopuser+pFlow[i].dbytes;
	   	iNumuser_down++;
		utPltPutLoopVarF(psDbHead,"userdbyts",iNumuser_down,"%0.2f",(float)pFlow[i].dbytes/(1024*1024));
		utPltPutLoopVar(psDbHead,"userdip",iNumuser_down,utComHostIp(htonl(psUser->ip)));
		stService_data[5][iNumuser_down-1].dbytes = pFlow[i].dbytes;//
    stService_data[5][iNumuser_down-1].service = htonl(psUser->ip);//ip;
    //printf("stService_data[5][%d].dbytes = %llu\n", iNumuser_down-1, stService_data[5][iNumuser_down-1].dbytes);
		if(lTotUserdbytes>0){
			utPltPutLoopVarF(psDbHead,"userdrate",iNumuser_down,"%.2f",100*(float)pFlow[i].dbytes/lTotUserdbytes);
			stService_data[5][iNumuser_down-1].rate = 100*(float)pFlow[i].dbytes/lTotUserdbytes;
		}
		else{
			utPltPutLoopVar(psDbHead,"userdrate",iNumuser_down,"0.0");
			stService_data[5][iNumuser_down-1].rate = 0.0;
		}
   }
	}
  int num6 = iNumuser_down;
	free(pFlow);
	
		
//集中输出	服务流量 源IP 目标IP排行中的其他和合计
//去流量最大的上行流量和下行流量当作上行流量和下行流量的合计
long long lTotup,lTotdown;
lTotup=0;
lTotdown=0;
if(lTotup<lTotubytes)
     lTotup=lTotubytes;
 if(lTotup<lTotIpubytes)
    lTotup=lTotIpubytes;
    
 if(lTotup<lTotUserubytes)
    lTotup=lTotUserubytes;
    
 if(lTotdown<lTotdbytes)
    lTotdown=lTotdbytes;
    
  if(lTotdown<lTotIpdbytes)
    lTotdown=lTotIpdbytes;
    
  if(lTotdown<lTotUserdbytes)
    lTotdown=lTotUserdbytes;
    
    
	if(lTotup>lUptop){
//输出服务上行流量排行其他和全部
  iNum++;
  utPltPutLoopVarF(psDbHead,"aubyts",iNum,"%0.2f",(float)(lTotup-lUptop)/(1024*1024));
  utPltPutLoopVar(psDbHead,"serviceup",iNum,"其他");
  utPltPutLoopVarF(psDbHead,"uprate",iNum,"%.2f",100*(float)(lTotup-lUptop)/lTotup);
  iNum++;
  utPltPutLoopVarF(psDbHead,"aubyts",iNum,"%0.2f",(float)lTotup/(1024*1024));
  utPltPutLoopVar(psDbHead,"serviceup",iNum,"合计");
  utPltPutLoopVarF(psDbHead,"uprate",iNum,"%d",100);
	}
	
	if(lTotdown>lDowntop){
//输出服务留下下行其他和全部
  iNum_down++;
  utPltPutLoopVarF(psDbHead,"adbyts",iNum_down,"%0.2f",(float)(lTotdown-lDowntop)/(1024*1024));
  utPltPutLoopVar(psDbHead,"servicedown",iNum_down,"其他");
  utPltPutLoopVarF(psDbHead,"drate",iNum_down,"%.2f",100*(float)(lTotdown-lDowntop)/lTotdown);
  iNum_down++;
  utPltPutLoopVarF(psDbHead,"adbyts",iNum_down,"%0.2f",(float)lTotdown/(1024*1024));
  utPltPutLoopVar(psDbHead,"servicedown",iNum_down,"合计");
  utPltPutLoopVarF(psDbHead,"drate",iNum_down,"%d",100);
	}

 if(lTotup>lUptopip){
//输出目标IP上行流量排行其他和全部
  iNumip++;
  utPltPutLoopVarF(psDbHead,"ipubyts",iNumip,"%0.2f",(float)(lTotup-lUptopip)/(1024*1024));
  utPltPutLoopVar(psDbHead,"dip",iNumip,"其他");
  utPltPutLoopVarF(psDbHead,"ipurate",iNumip,"%.2f",100*(float)(lTotup-lUptopip)/lTotup);
  iNumip++;
  utPltPutLoopVarF(psDbHead,"ipubyts",iNumip,"%0.2f",(float)lTotup/(1024*1024));
  utPltPutLoopVar(psDbHead,"dip",iNumip,"合计");
  utPltPutLoopVarF(psDbHead,"ipurate",iNumip,"%d",100);
	}
	
	if(lTotdown>lDowntopip){
//输出目标IP下行流量排行其他和全部
  iNumip_down++;
  utPltPutLoopVarF(psDbHead,"ipdbyts",iNumip_down,"%0.2f",(float)(lTotdown-lDowntopip)/(1024*1024));
  utPltPutLoopVar(psDbHead,"dip_d",iNumip_down,"其他");
  utPltPutLoopVarF(psDbHead,"ipdrate",iNumip_down,"%.2f",100*(float)(lTotdown-lDowntopip)/lTotdown);
  iNumip_down++;
  utPltPutLoopVarF(psDbHead,"ipdbyts",iNumip_down,"%0.2f",(float)lTotdown/(1024*1024));
  utPltPutLoopVar(psDbHead,"dip_d",iNumip_down,"合计");
  utPltPutLoopVarF(psDbHead,"ipdrate",iNumip_down,"%d",100);
	}
	
	if(lTotup>lUptopuser){
//输出源IP上行流量排行其他和全部
  iNumuser++;
  utPltPutLoopVarF(psDbHead,"userubyts",iNumuser,"%0.2f",(float)(lTotup-lUptopuser)/(1024*1024));
  utPltPutLoopVar(psDbHead,"useruip",iNumuser,"其他");
  utPltPutLoopVarF(psDbHead,"userurate",iNumuser,"%.2f",100*(float)(lTotup-lUptopuser)/lTotup);
  iNumuser++;
  utPltPutLoopVarF(psDbHead,"userubyts",iNumuser,"%0.2f",(float)lTotup/(1024*1024));
  utPltPutLoopVar(psDbHead,"useruip",iNumuser,"合计");
  utPltPutLoopVarF(psDbHead,"userurate",iNumuser,"%d",100);
	}
	
	if(lTotdown>lDowntopuser){
//输出源IP下行流量排行其他和全部
  iNumuser_down++;
  utPltPutLoopVarF(psDbHead,"userdbyts",iNumuser_down,"%0.2f",(float)(lTotdown-lDowntopuser)/(1024*1024));
  utPltPutLoopVar(psDbHead,"userdip",iNumuser_down,"其他");
  utPltPutLoopVarF(psDbHead,"userdrate",iNumuser_down,"%.2f",100*(float)(lTotdown-lDowntopuser)/lTotdown);
  iNumuser_down++;
  utPltPutLoopVarF(psDbHead,"userdbyts",iNumuser_down,"%0.2f",(float)lTotdown/(1024*1024));
  utPltPutLoopVar(psDbHead,"userdip",iNumuser_down,"合计");
  utPltPutLoopVarF(psDbHead,"userdrate",iNumuser_down,"%d",100);
	}
	
	
	      
      
if(strlen(caPdf) > 0 ) //导出pdf
{
	
	
	
	
}

	
	
	
	
	
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_dept/iframe_allflow.htm");
    
    return 0;

}




/*部门带宽对照表 仅用于统计部门带宽使用百分率*/

int ncGroupBandSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect;
   
    char *p;
    char caVar[32];
    int i=0;
    char caTemp[1024];
   	char caGid[16],caAdd[16],caDel[16];
	char caModi[16],caApp[16];
  long lGid,lBand,lCount,lDband;
	char caGroupname[32];
  char caBand[20],caDband[20];
 	pasDbCursor *psCur;
    long lGroupid0=0;
  char caUsername2[32];
  long lPid;
  memset(caUsername2,0,sizeof(caUsername2));
 dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
	iReturn = utMsgGetSomeNVar(psMsgHead,6,
	                "gid",  UT_TYPE_STRING,10,caGid,
	                "band",  UT_TYPE_STRING,10,caBand,	
	                "dband", UT_TYPE_STRING,10,caDband,          
	                "add",    UT_TYPE_STRING,12,caAdd,
	                "del",    UT_TYPE_STRING,12,caDel,
	                "modi",   UT_TYPE_STRING,12,caModi);
	                

    if(!utStrIsSpaces(caAdd)) { /* 增加一个组  */
    	lCount=0;
         	 sprintf(caTemp,"select count(*) from ncgroupband where gid=%s ",caGid);
         	 pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
         	 if(lCount>0){
         	 	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","该部门的带宽已经存在","该部门的带宽已经存在");
    					return 0;
         	}
          iReturn = pasDbExecSqlF("insert into ncgroupband(gid,band,dband) values (%lu,%lu,%lu)",atol(caGid),atol(caBand),atol(caDband));
            
                                             
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0151部门ip地址段"),ncLang("0310记录增加出错"));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                pasDbCommit(NULL);
            }                
       
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除一个组  */
    for(i=0;i<32;i++){
    	sprintf(caVar,"chk%d",i+1);
    	p=utMsgGetVar_s(psMsgHead,caVar);
    	if(p)
    	{
    		    
 	       iReturn = pasDbExecSqlF("delete from ncgroupband where gid= %s",p);
	        if(iReturn != 0) {
	            pasDbRollback(NULL);
	        }  
	        else{
	        	 pasDbCommit(NULL);
	        	}              
	}           
    }
    }
    else if(!utStrIsSpaces(caModi)) { /* 修改一个组  */   	
    	      
             iReturn = pasDbExecSqlF("update ncgroupband set band=%lu,dband=%lu where gid = %d ",
                         atol(caBand),atol(caDband),atol(caGid));
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0700修改出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }                                     

        pasDbCommit(NULL);
    }
  
         
      psDbHead = utPltInitDb();
      
       psCur=pasDbOpenSql("select groupid,groupname from ncgroup order by groupid ",0);
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0151部门ip地址段"),ncLang("0174出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
      
       lGid=0;       
       memset(caGroupname,0,sizeof(caGroupname));       
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid,
      				                     UT_TYPE_STRING,20,caGroupname);

      iNum=0;
      while(iReturn==0){
      	iNum++;   
      	utPltPutLoopVarF(psDbHead,"groupid",iNum,"%lu",lGid);
      	utPltPutLoopVar(psDbHead,"gname",iNum,caGroupname);

      
       lGid=0;       
       memset(caGroupname,0,sizeof(caGroupname));       
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid,
      				                     UT_TYPE_STRING,20,caGroupname);
  				}
       
       pasDbCloseCursor(psCur);
      
      
      
      
      pBuf=pasUtlCvtSqlSelectTreeBm();
///   	   pBuf = pasUtlCvtSqlSelectA("select groupname,groupid from ncgroup order by groupid","\0");
	               
	    if(pBuf) {
	        pSelect= pasUtlCvtInputSelect("groupid",caGid,pBuf);
	        free(pBuf);
	        utPltPutVar(psDbHead,"sgroup", pSelect);
	        free(pSelect);
	    }
	    lGid=0;
	    lBand=0;
	    lDband=0;
	    pasDbOneRecord("select gid,band,dband from ncgroupband where gid= 999999",0,
	                           UT_TYPE_LONG,4,&lGid,
	                           UT_TYPE_LONG,4,&lBand,
	                           UT_TYPE_LONG,4,&lDband);
	    iNum=0;                       
	    if(lGid==999999){
	    	iNum++;
	    	utPltPutLoopVarF(psDbHead,"iNum",iNum,"%lu",iNum);
	    	utPltPutLoopVarF(psDbHead,"gid",iNum,"%lu",lGid);
	    	utPltPutLoopVar(psDbHead,"groupname",iNum,"公司");
      	utPltPutLoopVarF(psDbHead,"band",iNum,"%lu",lBand);
      	utPltPutLoopVarF(psDbHead,"dband",iNum,"%lu",lDband);
	    }
      
      psCur=pasDbOpenSql("select a.gid,a.band,a.dband,b.groupname from ncgroupband a,ncgroup b where a.gid=b.groupid order by a.gid ",0);
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0151部门ip地址段"),ncLang("0174出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
      
       lGid=0;
       lBand=0;
       lDband=0;
       memset(caGroupname,0,sizeof(caGroupname));
       
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid,
                                   UT_TYPE_LONG,4,&lBand,
                                   UT_TYPE_LONG,4,&lDband,
      				                     UT_TYPE_STRING,20,caGroupname);

     
      while(iReturn==0){
      	iNum++;
     	
	      utPltPutLoopVarF(psDbHead,"iNum",iNum,"%lu",iNum);
      	utPltPutLoopVarF(psDbHead,"gid",iNum,"%lu",lGid);

      	utPltPutLoopVar(psDbHead,"groupname",iNum,caGroupname);

      	utPltPutLoopVarF(psDbHead,"band",iNum,"%lu",lBand);
      	utPltPutLoopVarF(psDbHead,"dband",iNum,"%lu",lDband);
       lGid=0;
       lBand=0;
       lDband=0;
       memset(caGroupname,0,sizeof(caGroupname));
       
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid,
                                   UT_TYPE_LONG,4,&lBand,
                                   UT_TYPE_LONG,4,&lDband,
      				                     UT_TYPE_STRING,20,caGroupname);
  				}
       
       pasDbCloseCursor(psCur);
       
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncbanddepset.htm");
    return 0;
}




//统计每个部门即时带宽
 
int ncWebOnlieDeptBand(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    int i,iReturn,iNum;
    char caTotPg[16],caCurPg[16];
    long lRowNum,lStartRec,lTotRec;
    utPltDbHead *psDbHead;
    long iSnum,lCurPg;
    char caTemp[2048];
    unsigned long lCount,lTotPg;
    long lMaxrtime;
    long long lUbytes,lDbytes,lSbytes;
    long lConntime,lGroupid;
    struct Service_s *pFlow; 
    ncUserInfo  *psUserInfo,**psUser;
    char       *pHash;
    long lSumUser,iNumGroup,j,lSum,flag,lSumTime;
    long long lBytes[2],lBand_up,lBand_down,lBand;
    char nbm[20],sortby[12],orderby[12];
    char caFlag[12];
    long lUnit;
    pasHashInfo sHashInfo;
    
     utMsgGetSomeNVar(psMsgHead,5,"CurPg",   UT_TYPE_STRING, 8,caCurPg,
                                  "nbm",     UT_TYPE_STRING,10,nbm,
                                  "sortby",  UT_TYPE_STRING,10,sortby,
                                  "orderby", UT_TYPE_STRING,10,orderby,
                                  "flag",    UT_TYPE_STRING,10,caFlag);
//  printf("caFlag=%s\n",caFlag);
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;

    lRowNum=32;
    lStartRec = (lCurPg - 1) * lRowNum;  
    
    
    pHash = utShmHashHead(psShmHead,NC_LNK_USERINFO);
    if(pHash == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0445权限检查"),ncLang("0400内部错误"));
        return 0;
    }
   lSumUser=0;
   lSumUser = utShmHashRecord(psShmHead,NC_LNK_USERINFO);
    
   pFlow=(struct Service_s *)malloc(1024*sizeof(struct Service_s));   
   memset(pFlow,0,1024*sizeof(struct Service_s));
   iNumGroup=0;
   psUserInfo = (ncUserInfo *)pasHashFirst(pHash,&sHashInfo);
   j = 0; 
   lSum=0;

   for(i=0;i<lSumUser && psUserInfo&&iNumGroup<1024;i++) {
   	if((strlen(nbm)==0)||(atol(nbm)==psUserInfo->lGid)){
        if(psUserInfo->lBytes[0] + psUserInfo->lBytes[1] > 0)
        {
  //计算带宽           	
             	lBytes[0] = psUserInfo->sCount.lSBytes[0] + psUserInfo->sCount.lCBytes[0] ;
   	          lBytes[1] = psUserInfo->sCount.lSBytes[1] + psUserInfo->sCount.lCBytes[1];
              lSumTime = time(0) - psUserInfo->sCount.lSTime; 	
       				if(lSumTime < 15) { 
       					  lBand_up=lBytes[0]/15;
       					  lBand_down=lBytes[1]/15; 
       	    			lBand = (lBytes[0] + lBytes[1]) / 15;
       				}
       				else {
       					  lBand_up=lBytes[0]/lSumTime;
       					  lBand_down=lBytes[1]/lSumTime;
//       	    			lBand = (lBytes[0] + lBytes[1]) / lSumTime;
        			}
              flag=0;
		        	for(j=0;j<iNumGroup;j++){
		        		if(pFlow[j].service==psUserInfo->lGid){
		        			pFlow[j].ubytes=pFlow[j].ubytes+lBand_up;
		        			pFlow[j].dbytes=pFlow[j].dbytes+lBand_down;
		        			flag=1;
		        			break;
		        	  }
		          }
           	 if(flag==0){
           	 	pFlow[iNumGroup].service=psUserInfo->lGid;
           	 	pFlow[iNumGroup].ubytes=lBand_up;
           	 	pFlow[iNumGroup].dbytes=lBand_down;
           	 	iNumGroup++;
           	}
           }
        }
        psUserInfo = (ncUserInfo *)pasHashNext(&sHashInfo);
    }
    
    psDbHead = utPltInitDb();
    
    orderflag=atol(sortby);
    dsflag=atol(orderby);
    if(atol(caFlag)==1){
    	orderflag=1;
    }
    qsort(pFlow,iNumGroup,sizeof(struct Service_s),ncUtlSortOnDepband); 
    
    
    iNum=0;
    for(i=lStartRec;i<iNumGroup&&iNum<lRowNum;i++){   
    	if(iNum==0){
    		if(pFlow[i].ubytes+pFlow[i].dbytes<1024){
    			lUnit=1;
    			utPltPutVar(psDbHead,"unit","bps");
    		}
    		else if(pFlow[i].ubytes+pFlow[i].dbytes>=1024&&pFlow[i].ubytes+pFlow[i].dbytes<1024*1024){
    			lUnit=1024;
    			utPltPutVar(psDbHead,"unit","Kbps");
    	  } 	
    	  else if(pFlow[i].ubytes+pFlow[i].dbytes>=1024*1024){
    	  	lUnit=1024*1024;
    	  	utPltPutVar(psDbHead,"unit","Mbps");
    	  }
    	}
    	      iNum++;   
    	      utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum-1);
    	      utPltPutLoopVarF(psDbHead,"num1",iNum,"%d",iNum);   
    	      utPltPutLoopVarF(psDbHead,"id",iNum,"%d",pFlow[i].service);   
            utPltPutLoopVarF(psDbHead,"urate",iNum,"%s",utStrLtoF8(8*pFlow[i].ubytes,caTemp));
            utPltPutLoopVarF(psDbHead,"srate2",iNum,"%llu",8*(pFlow[i].ubytes+pFlow[i].dbytes)/lUnit);
            utPltPutLoopVarF(psDbHead,"drate",iNum,"%s",utStrLtoF8(8*pFlow[i].dbytes,caTemp));
            utPltPutLoopVarF(psDbHead,"srate",iNum,"%s",utStrLtoF8(8*(pFlow[i].ubytes+pFlow[i].dbytes),caTemp));
          
           utPltPutLoopVar(psDbHead,"groupname",iNum,ncUtlGetGroupNameByid(psShmHead,pFlow[i].service,"\0"));
              

    }
    
    
    
    
    free(pFlow);
    
 
    lTotPg = iNumGroup / lRowNum+1 ;
    lTotRec = iNumGroup;
           utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
       utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
       utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
  
  if(atol(caFlag)==1){
  	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncdeptbandonlineuser_zzt.htm");
  }
  else{
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncdeptbandonlineuser.htm");
    }
    
    return 0;
}





 static int iSort,iOrder;
 	struct Ippkgt_s{
 		  unsigned long userid;
 		  unsigned long  service;
 		  unsigned long sip;
 		  unsigned long dip;
 		  long long dbytes;
 		  long long ubytes;
 		  long long uband;
 		  long long dband;
 	 };
static int ncUtlSortIpkg(char *p1,char *p2)
{
    int i;
    struct Ippkgt_s psIp1,psIp2;
    memcpy(&psIp1,p1,sizeof(struct Ippkgt_s));
    memcpy(&psIp2,p2,sizeof(struct Ippkgt_s));    
    long long l;
    unsigned long l2;
//    printf("iOrder=%d\n",iOrder);
        switch(iOrder) {
            case 1:                                       //网络服务
            	return (psIp2.service-psIp1.service);
      
            	break;
            case 2:                                        //源IP
            	return (psIp2.sip-psIp1.sip);
            	
            	break;
            case 3:
            	return (psIp2.dip-psIp1.dip);                     //目标IP
         
            	break;
            case 4:
            	l=(psIp2.ubytes+psIp2.dbytes)-(psIp1.ubytes+psIp1.dbytes);                     //带宽
            	if(l>0) return 1;
            	if(l<0) return -1;
            	if(l==0) return 0;
              	break;
            default :
            	break;
        }
    
    return 1;
}


//统计部门实时网络流量
 
int ncWebOnlieDeptStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;

    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[16];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
    int service_flag;
    char caTemp[4024];
  
   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
    pasDbCursor *psCur;
    char caFlag[12];
    long long lSumbytes;
 	  long sflag,m;
 	  float rate;
 	  long lBsteptime;
 	  float fRate;
 	  char caPdf[12];    
  
    char caTitle[128];
    char filename[128];

    long mm;
//    long lDate;
  
    char caStatdate[128];
 	  
   struct Ippkgt_s stIp[20];
 	  struct Ippkgt_s *pIp;
 	  
 	  long lGroupid;
 	  
 	  lBsteptime=atol(utComGetVar_sd(psShmHead,"BandSteptime","15"));
 	  
	 struct serviceonline_s *stService;
   int lSumservice;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

    iReturn = utMsgGetSomeNVar(psMsgHead,1,"groupid",   UT_TYPE_STRING, 10, caGroupid);
		
	 lGroupid=atol(caGroupid);
   lSumbytes=0;
    
   lSumservice=0;

 
    iCount = 0;
 

    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
    if(lMaxRec <= 0) {
    	 psDbHead = utPltInitDb();
    	    
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncuserviceonlinestat.htm");
           
        return 0;
    }
   if(lMaxRec>400000)
    lMaxRec=400000;
    
    psDbHead = utPltInitDb();
    psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
    
   
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }        
    lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    
    if(lSumRec>400000)
    lSumRec=400000;
    
    
    
    
    
    iSum = 0;
    long lSum=0;
    long memnum=0;
     pIp=(struct Ippkgt_s *)malloc(1024*sizeof(struct Ippkgt_s));
     lTime=time(0);
     for(i=0;i<lSumRec;i++){
     	psUserInfo = psIp[i]->psUser;     	
     	if((psIp[i]->lSip > 0)&&psUserInfo&&(psUserInfo->lGid==lGroupid)){
     		pIp[iSum].userid=psUserInfo->lId;
     		pIp[iSum].service=psIp[i]->lService;
     		pIp[iSum].sip=psIp[i]->lSip;
     		pIp[iSum].dip=psIp[i]->lDip;
     		pIp[iSum].dbytes=psIp[i]->lBytes[1];
     		pIp[iSum].ubytes=psIp[i]->lBytes[0];
     		
     		if(psIp[i]->lStartTime > 0&&psIp[i]->lLastTime>0 && (lTime - psIp[i]->lStartTime > lBsteptime)) {
         	    	if((lBsteptime+psIp[i]->lLastTime>lTime)&&(psIp[i]->lLastTime>psIp[i]->lStartTime)){
         	    		if(lBsteptime+psIp[i]->lLastTime>lTime){        	    			
         	    		   fRate=(float)(lBsteptime+psIp[i]->lLastTime-lTime)/lBsteptime;
         	    		  }
         	    		  else{
         	    		  	fRate=0;
         	    		  }
                   pIp[iSum].uband=psIp[i]->lBytes[0]*fRate/(psIp[i]->lLastTime-psIp[i]->lStartTime);
                   pIp[iSum].dband=psIp[i]->lBytes[1]*fRate/(psIp[i]->lLastTime-psIp[i]->lStartTime);
                   
                  }
                  else{
									  pIp[iSum].uband=0;
									  pIp[iSum].dband=0;
                  }
               
              }
             	else {
             		pIp[iSum].uband=psIp[i]->lBytes[0]/lBsteptime;
             		pIp[iSum].dband=psIp[i]->lBytes[1]/lBsteptime;
                
             	}
     		   iSum++ ;  		
     		   if(iSum%1000==0){
		      	memnum=iSum/1000;
		      	pIp=(struct Ippkgt_s*)realloc(pIp,1024*(memnum+1)*sizeof(struct Ippkgt_s));
		      }
     		
     		
     	}
    }
    free(psIp);
    
 

     if(iSum>0){
     	//按网络服务排序
     	   iOrder=1;
     	
         qsort(pIp,iSum,sizeof(struct Ippkgt_s),ncUtlSortIpkg);
     }

    lSum=0;
    memset(&stIp,0,20*sizeof(struct Ippkgt_s));
    for(i=0;i<iSum&&lSum<19;i++){
    	if(pIp[i].service>0){
    		if(pIp[i].service==stIp[lSum].service){
    			stIp[lSum].dbytes=stIp[lSum].dbytes+pIp[i].dbytes;
    			stIp[lSum].ubytes=stIp[lSum].ubytes+pIp[i].ubytes;
    			stIp[lSum].uband=stIp[lSum].uband+pIp[i].uband;
    			stIp[lSum].dband=stIp[lSum].dband+pIp[i].dband;
    		}
    		else{
    			lSum++;
    			stIp[lSum].service=pIp[i].service;
    			stIp[lSum].dbytes=pIp[i].dbytes;
    			stIp[lSum].ubytes=pIp[i].ubytes;
    			stIp[lSum].uband=pIp[i].uband;
    			stIp[lSum].dband=pIp[i].dband;
    		}
    }
  }

  
   
     iNum=0;
     lSumbytes=0;
         if(lSum>0){
     	//按总流量降序排序
     	   iOrder=4;
     	
         qsort(stIp,lSum+1,sizeof(struct Ippkgt_s),ncUtlSortIpkg);
     }
  
      for(i = 0; i < lSum&&iNum<10 ;i++) {
        
      	strcpy(caService,ncUtlGetServiceName(psShmHead,stIp[i].service,"\0"));
 // printf("caService=%s\n",caService);   	
      	if(strlen(caService)>0){      		
        lSumbytes=lSumbytes+stIp[i].ubytes+stIp[i].dbytes;		
        utPltPutLoopVarF(psDbHead,"id",iNum+1,"%d",stIp[i].service);
        utPltPutLoopVar(psDbHead,"service", iNum+1,ncUtlGetServiceName(psShmHead,stIp[i].service,"\0"));
     
        utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(stIp[i].dbytes,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(stIp[i].ubytes,caTemp));
        
        utPltPutLoopVarF(psDbHead, "subytes2",iNum+1,"%llu",stIp[i].ubytes+stIp[i].dbytes); 
        utPltPutLoopVarF(psDbHead, "subytes",iNum+1,"%s",utStrLtoF8(stIp[i].ubytes+stIp[i].dbytes,caTemp));  
              
        utPltPutLoopVarF(psDbHead, "band",iNum+1,"%s",utStrLtoF8((stIp[i].uband+stIp[i].dband)*8,caTemp));               
        utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);  
 
        
       iNum++;
     }
    }
    utPltPutVarF(psDbHead,"TotService","%d",iNum);
    
//统计源IP流量
     
        if(iSum>0){
     	//按源IP排序
     	   iOrder=2;    	
         qsort(pIp,iSum,sizeof(struct Ippkgt_s),ncUtlSortIpkg);
     }
    memset(&stIp,0,20*sizeof(struct Ippkgt_s));
    lSum=0;
    for(i=0;i<iSum&&lSum<19;i++){
    	if(pIp[i].sip>0){
    		if(pIp[i].sip==stIp[lSum].sip){
    			stIp[lSum].dbytes=stIp[lSum].dbytes+pIp[i].dbytes;
    			stIp[lSum].ubytes=stIp[lSum].ubytes+pIp[i].ubytes;
    			stIp[lSum].uband=stIp[lSum].uband+pIp[i].uband;
    			stIp[lSum].dband=stIp[lSum].dband+pIp[i].dband;
    		}
    		else{
    			lSum++;
    			stIp[lSum].sip=pIp[i].sip;
    			stIp[lSum].dbytes=pIp[i].dbytes;
    			stIp[lSum].ubytes=pIp[i].ubytes;
    			stIp[lSum].uband=pIp[i].uband;
    			stIp[lSum].dband=pIp[i].dband;
    		}
    }
  }
 
 
     iNum=0;
     lSumbytes=0;
         if(lSum>0){
     	//按总流量降序排序
     	   iOrder=4;   	
         qsort(stIp,lSum+1,sizeof(struct Ippkgt_s),ncUtlSortIpkg);
     }
  
      for(i = 0; i < lSum&&iNum<10 ;i++) {
        
         		
        lSumbytes=lSumbytes+stIp[i].ubytes+stIp[i].dbytes;		
       
        utPltPutLoopVar(psDbHead,"sip", iNum+1,utComHostIp(htonl(stIp[i].sip)));
     
        utPltPutLoopVarF(psDbHead, "inbytes_sip",iNum+1,"%s",utStrLtoF8(stIp[i].dbytes,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes_sip",iNum+1,"%s",utStrLtoF8(stIp[i].ubytes,caTemp));
        
        utPltPutLoopVarF(psDbHead, "subytes2_sip",iNum+1,"%llu",stIp[i].ubytes+stIp[i].dbytes); 
        utPltPutLoopVarF(psDbHead, "subytes_sip",iNum+1,"%s",utStrLtoF8(stIp[i].ubytes+stIp[i].dbytes,caTemp));  
              
        utPltPutLoopVarF(psDbHead, "band_sip",iNum+1,"%s",utStrLtoF8((stIp[i].uband+stIp[i].dband)*8,caTemp));               
        utPltPutLoopVarF(psDbHead,"num_sip",iNum+1,"%d",iNum);  
 
        
       iNum++;
    
    }
    utPltPutVarF(psDbHead,"TotSip","%d",iNum);
 
 
 
 
 //统计目标IP流量
     
        if(iSum>0){
     	//按目标IP排序
     	   iOrder=3;    	
         qsort(pIp,iSum,sizeof(struct Ippkgt_s),ncUtlSortIpkg);
     }
    memset(&stIp,0,20*sizeof(struct Ippkgt_s));
    lSum=0;
    for(i=0;i<iSum&&lSum<19;i++){
    	if(pIp[i].dip>0){
    		if(pIp[i].dip==stIp[lSum].dip){
    			stIp[lSum].dbytes=stIp[lSum].dbytes+pIp[i].dbytes;
    			stIp[lSum].ubytes=stIp[lSum].ubytes+pIp[i].ubytes;
    			stIp[lSum].uband=stIp[lSum].uband+pIp[i].uband;
    			stIp[lSum].dband=stIp[lSum].dband+pIp[i].dband;
    		}
    		else{
    			lSum++;
    			stIp[lSum].dip=pIp[i].dip;
    			stIp[lSum].dbytes=pIp[i].dbytes;
    			stIp[lSum].ubytes=pIp[i].ubytes;
    			stIp[lSum].uband=pIp[i].uband;
    			stIp[lSum].dband=pIp[i].dband;
    		}
    }
  }
 
 
     iNum=0;
     lSumbytes=0;
         if(lSum>0){
     	//按总流量降序排序
     	   iOrder=4;   	
         qsort(stIp,lSum+1,sizeof(struct Ippkgt_s),ncUtlSortIpkg);
     }
  
      for(i = 0; i < lSum&&iNum<10 ;i++) {
        
         		
        lSumbytes=lSumbytes+stIp[i].ubytes+stIp[i].dbytes;		
       
        utPltPutLoopVar(psDbHead,"dip", iNum+1,utComHostIp(htonl(stIp[i].dip)));
     
        utPltPutLoopVarF(psDbHead, "inbytes_dip",iNum+1,"%s",utStrLtoF8(stIp[i].dbytes,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes_dip",iNum+1,"%s",utStrLtoF8(stIp[i].ubytes,caTemp));
        
        utPltPutLoopVarF(psDbHead, "subytes2_dip",iNum+1,"%llu",stIp[i].ubytes+stIp[i].dbytes); 
        utPltPutLoopVarF(psDbHead, "subytes_dip",iNum+1,"%s",utStrLtoF8(stIp[i].ubytes+stIp[i].dbytes,caTemp));  
              
        utPltPutLoopVarF(psDbHead, "band_dip",iNum+1,"%s",utStrLtoF8((stIp[i].uband+stIp[i].dband)*8,caTemp));               
        utPltPutLoopVarF(psDbHead,"num_dip",iNum+1,"%d",iNum);  
 
        
       iNum++;
    
    }
    utPltPutVarF(psDbHead,"TotDip","%d",iNum);
 
 
 
 
 
     
   free(pIp);

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncdeptonlinestat.htm");
    
    return 0;
}