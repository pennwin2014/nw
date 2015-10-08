/* ������---������
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



//����  ����������
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


//����  ������ID����
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



//7.5��ʾ��ַ���������ͼ
int ncWebShowWebsiteBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    long index,x_sum;              //index  X���±�   x_sum X���±�����
    float rate;
    long count_d=0;                //Y��������
    char caHour[12];
    long long lMaxbytes;               //����������ֵ
//    long lMaxconntime;
    long lMaxrate;                     // ���Y�����ֵ
    char caGitem[16],caSdate[20],caShour[16],caSmin[16];

   struct ncSflow_s  sGservice[20];                  //���ͳ�����ݣ�ǰʮ�ַ������0-10�У���������sGservice ���һ��
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
    struct chart_datasetf *pChart;           //����PDF�ĵ����
    int rowrow;                     
    int column; 
    char caTitle[128];
    char filename[128];
    char caStatdate[128];

    char **desc;
    char webname[8024];  
    char webname2[8024];  
    char receiveman[1024];  
   	char mail_name[120]="";
   	char caSdate_All[20];
   	char webid[2024];  
    utMsgGetSomeNVar(psMsgHead,16,"stime",    UT_TYPE_STRING,10,caStime,				//
   		                            "sdate",    UT_TYPE_STRING,10,caSdate,
   		                            "shour",    UT_TYPE_STRING,8,caShour,
   		                            "smin",     UT_TYPE_STRING,8,caSmin,
   		                            "av_un",    UT_TYPE_STRING,8,caAv_un,					//��2��Сʱ��1�����ӣ�3����ͳ��
																  "pre_flag", UT_TYPE_STRING,2,caPre_flag,	    //��ʾǰһ��ʱ�������1����ǰ��2�����
																  "groupname",UT_TYPE_STRING,30,gpname,
																  "dispname", UT_TYPE_STRING,30,dpname,
																  "tid",      UT_TYPE_STRING,10,caTid,					//��ַ�����
																  "userid",   UT_TYPE_STRING, 14, caUserid,
																  "groupid",  UT_TYPE_STRING,14,group_id,
																  "pdf",      UT_TYPE_STRING,8,caPdf,
																  "rootname", UT_TYPE_STRING,30,rtname,
                                  "receiveman",UT_TYPE_STRING,1023,receiveman,
                                  "webname",   UT_TYPE_STRING,8023,webname,
                                  "webid",    UT_TYPE_STRING,2023,webid);
    
 		                             
  psDbHead = utPltInitDb(); 
  printf("== webid=%s ===\n",webid);
  //printf("== webname=%s ===\n",webname);
  /*
if(strlen(webname)>0){
	char *haystack;
	haystack=malloc(sizeof(char)*8023);
	strcpy(haystack,webname);
	char* buf = strstr( haystack, ","); 
		//strcat(webname2,haystack);
		 printf("== webname2=%s ===\n",webname2);
	while( buf != NULL ) 
	{ 
	    buf[0]='\0'; 
    	printf( "== %s\n ", haystack); 
			if(strlen(webname2)>0)    
    	strcat(webname2,",");
    	strcat(webname2,"'");  
    	strcat(webname2,haystack);  
    	 printf("=11= webname2=%s ===\n",webname2);   
   	  haystack = buf + strlen(",")+2; 
   	  printf( "=1= %s\n ", haystack); 
	    //Get next token: 
	    buf = strstr( haystack, ","); 
	    strcat(webname2,"'"); 
	     printf("=22= webname2=%s ===\n",webname2);
	} 
             
}*/
  printf("== webname2=%s ===\n",webname2);
  if(strlen(caGitem)==0) strcpy(caGitem,"1");
  if(strlen(caAv_un)==0) strcpy(caAv_un,"1");										//Ĭ�ϰ�����ͳ��
  
	 if(atol(caAv_un)==1){									//������ͳ��(nchttplog)
	    x_sum=15;
	    y_sum=15;
	 }
	 else if(atol(caAv_un)==2){						  //��Сʱͳ��(nchttplog)
	 	x_sum=24;
	 	y_sum=24;
	}
	else if(atol(caAv_un)==3){						  //����ͳ��(nchttplog)
		x_sum=30;
		y_sum=30;
	}

	if((strlen(caStime)==0)&&(strlen(caSdate)==0)){											//Ĭ��ûʱ�䰴ϵͳʱ�䣬��������ͳ��
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
						lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate);                                                //����ͳ��
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
//������һ�κ���һʱ���		  
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
   	utPltPutVar(psDbHead,"webname",webname);
		utPltPutVar(psDbHead,"webid",webid);
    sprintf(caSdate_All,"%s %s:%s",caSdate,caShour,caSmin);
    
    utPltPutVar(psDbHead,"tjsj",caSdate);
    //utPltPutVarF(psDbHead,"stime","%d",lEndtime);
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 		//��ȡͳ��ʱ���������������ַ��𣬽���Щ������ͼ�����������ķ���鵽���������
 if(atol(caAv_un)==1){                                           //������ƽ��
                                       // �û�ͳ��
    	sprintf(caTemp,"select count(*) aa,a.tid,b.name from nchttplog a,ncwebclass b where a.vtime>=%d and a.vtime<%d and a.tid=b.id ",lStarttime,lEndtime);	
    if(strlen(group_id)>0){
    	sprintf(caTemp+strlen(caTemp)," and (a.groupid =%s or a.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	//   sprintf(caTemp+strlen(caTemp)," and a.groupid =%s ",group_id);
  	  }
		else if(strlen(caUserid)>0)
  	   sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	else
  	   sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	if(strlen(webid)!=0)
  		 sprintf(caTemp+strlen(caTemp)," and a.tid in (%s) ",webid);	
  	   sprintf(caTemp+strlen(caTemp)," group by a.tid order by aa desc limit 0,10 ");
  }
  else if(atol(caAv_un)==2){  
  	                       //��Сʱͳ���û�
  	  strcpy(caSdate,utTimFormat("%Y/%m/%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate)));
  	  sprintf(caTemp,"select sum(a.lcount) aa,a.tid,b.name from nchttpcount a,ncwebclass b,ncuser c where a.sdate='%s'  and a.userid=c.userid and a.tid=b.id ",caSdate);
  	  if(strlen(group_id)>0){
  	  	sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  //	   sprintf(caTemp+strlen(caTemp)," and c.userid=a.userid and c.groupid =%s ",group_id);
  	  }
			else if(strlen(caUserid)>0)
  	   sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  		else
  	   sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  if(strlen(webid)!=0)
  		 sprintf(caTemp+strlen(caTemp)," and a.tid in (%s) ",webid);
  	   sprintf(caTemp+strlen(caTemp)," group by a.tid order by aa desc limit 0,10 ");
  } 
  else if(atol(caAv_un)==3){
  	                            //����ͳ���û�
  		sprintf(caTemp,"select sum(a.lcount) aa,a.tid,b.name from nchttpcount a,ncwebclass b,ncuser c where a.sdate>='%s' and a.sdate<='%s' and a.userid=c.userid and a.tid=b.id ",caSdate,caEdate);		
  		if(strlen(group_id)>0){
  			sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
 // 	   sprintf(caTemp+strlen(caTemp)," and c.userid=a.userid and c.groupid =%s ",group_id);
  	  }
			else if(strlen(caUserid)>0)
  	   sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  		else
  	   sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  if(strlen(webid)!=0)
  		 sprintf(caTemp+strlen(caTemp)," and a.tid in (%s) ",webid);
  	   sprintf(caTemp+strlen(caTemp)," group by a.tid order by aa desc limit 0,10 ");
  }   
     printf("**111**caTemp=%s\n",caTemp);
     psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����2"));
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
   
    
    //���һ����������û�������
  
    	  sGservice[lSumindex].lService=99999;
    		sGservice[lSumindex].lGroupid=0;
    		sGservice[lSumindex].lBytes=0;
    		strcpy(sGservice[lSumindex].caGname,ncLang("0431����"));
        for(i=0;i<32;i++){
    	    sGservice[lSumindex].lBytesqs[i]=0;
    	    
        }
        

  //��userid����  Ϊ������ַ�������׼��		
    		qsort(sGservice,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId2); 
 	 
 	 	 if(atol(caAv_un)==1){                                           //������ƽ��		--->ͳ��nchttplogϸ���� vtime:1243240009=2009-05-25 16:26:49=sdate(2009-05-25)+stime(16:26:49)
		 	                                          // �û�ͳ��																																																
		    sprintf(caTemp,"select count(*) aa,from_unixtime(vtime,'%ci') bb,tid from nchttplog where vtime>=%d and vtime<=%d  ",'%',lStarttime,lEndtime);
	 				 if(strlen(group_id)>0){
	 				 	sprintf(caTemp+strlen(caTemp)," and (groupid =%s or groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
 // 	   				sprintf(caTemp+strlen(caTemp)," and groupid =%s ",group_id);
  	   			}
					 else if(strlen(caUserid)>0)
  	   				sprintf(caTemp+strlen(caTemp)," and userid in (%s) ",caUserid);	
  				 else
  	   				sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	   		 if(strlen(webid)!=0)
  		 sprintf(caTemp+strlen(caTemp)," and tid in (%s) ",webid);
  	   				sprintf(caTemp+strlen(caTemp)," group by bb,tid order by aa desc ");
		 }else if(atol(caAv_un)==2){  
  	                        //��Сʱͳ���û�     --->ͳ��nchttpcountʱ�α����Ч��
  	    strcpy(caSdate,utTimFormat("%Y/%m/%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate)));
  	    sprintf(caTemp,"select sum(a.lcount) aa,a.stime,a.tid from nchttpcount a,ncuser c where a.userid=c.userid and a.sdate='%s'  ",caSdate);
  	    if(strlen(group_id)>0){
  	    	sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	    }
  //	  		 sprintf(caTemp+strlen(caTemp)," and c.userid=a.userid and c.groupid =%s ",group_id);
		    else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  	if(strlen(webid)!=0)
  		 sprintf(caTemp+strlen(caTemp)," and a.tid in (%s) ",webid);
  	  		 sprintf(caTemp+strlen(caTemp)," group by a.stime,a.tid ");
     }else if(atol(caAv_un)==3){                    //����ͳ���û�

     		sprintf(caTemp,"select sum(a.lcount) aa,substring(a.sdate,9,2) bb,a.tid from nchttpcount a,ncuser c where a.userid=c.userid and a.sdate>='%s' and a.sdate<='%s' ",caSdate,caEdate);
     		if(strlen(group_id)>0){
     			sprintf(caTemp+strlen(caTemp)," and (c.groupid =%s or c.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
 // 	  		 sprintf(caTemp+strlen(caTemp)," and c.userid=a.userid and c.groupid =%s  ",group_id);
  	  		}
		    else if(strlen(caUserid)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",caUserid);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  	if(strlen(webid)!=0)
  		 		 sprintf(caTemp+strlen(caTemp)," and a.tid in (%s) ",webid);
  	  		 sprintf(caTemp+strlen(caTemp)," group by bb,a.tid ");
 }
 printf("webid=%s\n",webid);
   printf("**22**caTemp=%s\n",caTemp);
   psCur = pasDbOpenSql(caTemp,0);
   if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����1"));
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
    	if(atol(caAv_un)==1){
    	if(findex>=15&&findex<30){                           //������ ʱ��ֵ�������±����30
    		findex=findex-15;
    	}
    	else if(findex>=30&&findex<45){
    		findex=findex-30;
    	}
    	else if(findex>=45){
    		findex=findex-45;
    	}
    	}
    	  index=ncUtlGetServicenumById(&sGservice,lSumindex,lService);
 //  	  printf("index=%d\n",index);
//    	  printf("findex=%d\n",findex);
	   	  if(index!=-1){	
	          sGservice[index].lBytesqs[findex]=sGservice[index].lBytesqs[findex]+lCount;         
	      }else{   
    	  	sGservice[lSumindex].lBytesqs[findex]=sGservice[lSumindex].lBytesqs[findex]+lCount; 
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
  
  //���X��̶�

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
	  		  //��������ͳ������
	  		  for(i=0;i<=lSumindex;i++){	
	  				if(atol(caAv_un)==3)                                               //���ڰ���ͳ�ƣ������1��ʼ�������0��ʼ
	  			  	 utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm+1],caTemp);  
	  				else  			
	  			  	 utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm],caTemp);  
	  		 
	  	  	}
	  	  	
	  }
	   
 //�������ͼ������ 

    for(i=0;i<=lSumindex;i++){
    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);
    	//printf("gservice=%s\n",sGservice[i].caGname);
    	utPltPutLoopVar(psDbHead,"sevname",i+1,sGservice[i].caGname);
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",sGservice[i].lService);
      
    	for(mm=0;mm<x_sum;mm++){
    		//printf("i=%d;mm=%d;service=%d;bytes=%lld\n",i,mm,sGservice[i].lService,sGservice[i].lBytesqs[mm]);
    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);

    		if(atol(caAv_un)==3)                                               //���ڰ���ͳ�ƣ������1��ʼ�������0��ʼ
	  			  utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%lld",sGservice[i].lBytesqs[mm+1]);  
	  		else 
    		    utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%lld",sGservice[i].lBytesqs[mm]);	
    }
  }
  utPltPutVarF(psDbHead,"sum","%d",lSumindex+1);
  utPltPutVarF(psDbHead,"sum_d","%d",x_sum);
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
printf("******caPdf=%s\n",caPdf);
	if(strlen(caPdf)>0||strlen(receiveman)>0){
	//����ǩ
	 desc=malloc((lSumindex+1)*sizeof(char*));
	 for(i=0;i<lSumindex+1;i++){
	 	desc[i]=malloc(32);
	 	strcpy(desc[i],sGservice[i].caGname);
   }

   //�����PDF��     
   pChart=(struct chart_datasetf*)malloc(sizeof(struct chart_datasetf)*(x_sum+1));
   	if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("1863�����ڴ����"));
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
   	  	 if(atol(caAv_un)==3)                             //���ڰ���ͳ�������1��ʼ�������0��ʼ
   	  	  	pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i+1];
   	  	 else
   	  	 		pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i];   	  
   	 	 } 
   	 }

      rowrow=x_sum;
      column=lSumindex+1;
   
    strcpy(caTitle,ncLang("1950��վ���ʴ�������ͼ"));
    sprintf(filename,"%s.PDF",caTitle); 

    sprintf(caStatdate,ncLang("1962ͳ��ʱ��:%s %s:%s"),caSdate,caShour,caSmin);

    sprintf(caTemp,ncLang("1942��λ(%s)"),ncLang("1898��"));
    if(strlen(caPdf)>0)
	    ncWebShowServiceBytime_PDF(iFd,pChart,desc,rowrow,column,caTitle,caTemp,caStatdate,filename);
		if(strlen(receiveman)>0){
     			//flag=2;
    			if(strlen(gpname)!=0){
						strcpy(mail_name,gpname);
						strcat(mail_name,"-");	
					}
					if(strlen(dpname)!=0){
						strcat(mail_name,dpname);
						strcat(mail_name,"-");	
					}
					strcat(mail_name,ncLang("1951��վ���ʴ�������ͼ--"));
					strcat(mail_name,caSdate_All);
					strcat(mail_name,"~");
					if(atol(caAv_un)==1){
		    		strcat(mail_name,ncLang("1963������ͳ��"));
		     	}else if(atol(caAv_un)==2){
		     		strcat(mail_name,ncLang("1964��Сʱͳ��"));
		     	}else{
		     		strcat(mail_name,ncLang("1965����ͳ��"));
		     	}
					
					char fjname[260];
					strcpy(fjname,"/home/ncmysql/nc/report/");	
					strcat(fjname,filename);	
					//printf("== == fjname=%s\n",fjname);
					//printf("== == mail_name=%s\n",mail_name);
					//utPltFileDownload(iFd, "application/pdf", "/home/ncmysql/nc/report", filename_new, filename_new);   filename_new���þ���·��
					ncWebShowServiceBytime_mail_PDF(iFd,pChart,desc,rowrow,column,caTitle,caTemp,caStatdate,fjname,utComGetVar_sd(psShmHead,"NcMailFrom","ProEIM@proeim.com"),receiveman,mail_name,"",fjname,""); 

    		
    	}
//�ͷ��ڴ�     
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
	  
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_website/iframe_website_webclassbytime.htm");
    
    return 0;
}
