/* ������---������
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
#include "nclimit.h"
#include "ncsysinfo.h"
#include "ncpkg.h"
#include "ncreport.h" 

char *ncUtlCvtWorkTime2Sql(char *field);
char *ncUtlCvtWorkTime2Sql_s(char *field1,char *field2);
//7.5�汾������������ͼ
int ncWebShowOutflowMore_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    pasDbCursor *psCur;
 
    char caFlag[8];
    char caPre_flag[8],caAv_un[8];
//    char sqlbuf[2048];
//    unsigned long iMaxItem,lSize;
    unsigned long long lUbytes,lDbytes,lMaxBytes;
 //   unsigned long stime;
    int      iReturn,i;
    long lTime,lStarttime,lEndtime;
    char caSid[32],caUnit[16];
//    char caSdate1[32],caSdate2[32],caTime1[32],caTime2[32];
//    char caLabel[128];
    char caTemp[8192];
//    char caTemp1[32];
    char caName[32];
    char caStime[16];
    float tot[60];
    long y_sum;
    char caSdate_temp1[20],caSdate_temp2[20];
    int m,iNum;
    int mm,nn;

 //   char temp_t[16];
    struct data_s{
    	long sid;
    	char caDevname[32];
    	unsigned long long lUbytes[60];
      unsigned long lConntime[60];
      long index[60];
    } devname[20];
    char caTime[16];
    long long unit;
    long lSid,lConntime;
//    long long lMax=0;
    long index,x_sum;              //index  X���±�   x_sum X���±�����
    float rate;
    long count_d=0;                //Y��������
    char caHour[12];
    long long lMaxbytes,lRat;
    
    char caPdf[12];   
    struct chart_datasetf *pChart;           //����PDF�ĵ����
    int rowrow;                     
    int column; 
    char caTitle[128];
    char filename[128];
    char caStatdate[128];

    char **desc;
    
    
//    long lMaxconntime;
//    long lMaxrate;
    for(i=0;i<60;i++){
    	tot[i]=0;
    }
    index=0;
    x_sum=0;
		utMsgGetSomeNVar(psMsgHead,5,"av_un",UT_TYPE_STRING,2,caAv_un,
																 "pre_flag",UT_TYPE_STRING,2,caPre_flag,
																 "flag",     UT_TYPE_STRING, 2, caFlag,
		                             "stime",    UT_TYPE_STRING, 14,caStime,
		                             "pdf",      UT_TYPE_STRING,8,caPdf);
		                             

		                             
  if(atol(caAv_un)==2){
    x_sum=24;
    y_sum=24;
  }
  else{
    x_sum=30;
    y_sum=60;
  }
    
    
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
		    
		     strcpy(caSdate_temp1,utTimFormat("%Y/%m/%d %H",lTime));
//		     sprintf(caSdate_temp2,"%s 00:00:00",caSdate_temp1);
		     lStarttime=utTimStrToLong("%Y/%m/%d %H:00:00",caSdate_temp1);
		    lEndtime=lStarttime+3600;
		

		    
//		    lStarttime=lTime-3600;
//		    lEndtime=lTime;
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
		  
		  		strcpy(caHour,utTimFormat("%H",lStarttime));    
//	        printf("caHour=%s\n",caHour);
		}
//    char caDevname[20][32];
 //ȡ�豸��   
    sprintf(caTemp,"select sid,name from ncnetdev where flags=0 ");   
    psCur = pasDbOpenSql(caTemp,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����"));
        return 0;
    }
    psDbHead = utPltInitDb();    
    iReturn=0;
    iNum=0;
   
   lSid=0;
   memset(caName,0,sizeof(caName));
    memset(&devname[0],0,sizeof(struct data_s));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                   UT_TYPE_STRING,31,caName);

    while((iReturn == 0)||(iReturn==1405)) {
    	if(iNum>20) break;
    	devname[iNum].sid=lSid;
    	sprintf(devname[iNum].caDevname,"%s%s",caName,ncLang("0504����"));
    	iNum++;
    	devname[iNum].sid=lSid;
    	sprintf(devname[iNum].caDevname,"%s%s",caName,ncLang("0677����"));
    	
    	iNum++;
      lSid=0;
      memset(&devname[iNum],0,sizeof(struct data_s));
      
      memset(caName,0,sizeof(caName));
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                   UT_TYPE_STRING,31,caName);
    }
    
    pasDbCloseCursor(psCur);
    
    for(i=0;i<iNum;i++){
    	for(m=0;m<y_sum;m++){
    	  devname[i].lUbytes[m]=0;
    	}
    	
    	
    	if(atol(caAv_un)==2){
 //��Сʱ����
    		sprintf(caTemp,"select sum(ubytes),sum(dbytes),sum(conntime),from_unixtime(rtime,'%cH') aa from ncflowlog where devnum=%d and rtime>=%lu and rtime<=%lu group by aa order by aa  limit 0,24 ",'%',devname[i].sid,lStarttime,lEndtime);
    	}
    	else{
 //�����ӷ���
     		sprintf(caTemp,"select sum(ubytes),sum(dbytes),sum(conntime),from_unixtime(rtime,'%ci') aa from ncflowlog where devnum=%d and rtime>=%lu and rtime<=%lu group by aa order by aa  limit 0,60",'%',devname[i].sid,lStarttime,lEndtime);
      }
//    printf("caTemp=%s\n",caTemp);
     	psCur = pasDbOpenSql(caTemp,0);
    	if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����"));
        return 0;
    	}
       m=0;
      lUbytes=0;
      lDbytes=0;
      lConntime=0;
      memset(caTime,0,sizeof(caTime));
      count_d=0; 
      lMaxbytes=0;
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
	                                   UT_TYPE_LONG8,8,&lDbytes,
	                                   UT_TYPE_LONG,4,&lConntime,
	                                   UT_TYPE_STRING,8,caTime);
	   while((iReturn == 0)||(iReturn==1405)) { 
	   	if(lConntime>0){
				if((lUbytes+lDbytes)/lConntime>lMaxbytes){
				   lMaxbytes=(lUbytes+lDbytes)/lConntime;
				  }
       }
      index=atol(caTime);
//iż�� ������������������ 	   	 
      if(i%2==0){
        devname[i].lUbytes[index]=lUbytes;
       
      }
      else{
        devname[i].lUbytes[index]=lDbytes;
      
      }
      
      devname[i].lConntime[index]=lConntime;
      devname[i].index[index]=index;
     
//printf("i=%d,index=%d,ubytes=%llu,lCuntime=%d,caTime=%d\n",i,index,devname[i].lUbytes[index],devname[i].lConntime[index],devname[i].index[index]);   
//��ȡ���ֵ�������ӿ����ʵ�λ
 //  if(lConntime>0){
 //     if(lMax<(lUbytes+lDbytes)/lConntime)
 //         lMax=(lUbytes+lDbytes)/lConntime;
//    } 
         
      m++;
      lUbytes=0;
      lDbytes=0;
      lConntime=0;
      memset(caTime,0,sizeof(caTime));
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lUbytes,
	                                   UT_TYPE_LONG8,8,&lDbytes,
	                                   UT_TYPE_LONG,4,&lConntime,
	                                   UT_TYPE_STRING,8,caTime);
	    }

	//   devname[i].lCount=m;
	   
	   pasDbCloseCursor(psCur);
	   

	}
 
	  count_d=index+1;
	  
	  
	  
	  //�����������ֵ����ѡ��bps,Kbps,Mbps,Gbps
	  
	//��ȡ���ݿ���������� ,�������ֵȡ���ֵ������
//sprintf(caTemp,"select max(ubytes+dbytes),conntime,from_unixtime(rtime,'%ci') aa from ncflowlog group by aa ",'%');
//lMaxbytes=0;
//lMaxconntime=0;
//pasDbOneRecord(caTemp,0,UT_TYPE_LONG8,8,&lMaxbytes,
//                        UT_TYPE_LONG,4,&lMaxconntime,
//                        UT_TYPE_STRING,8,caTime);  
	    lRat=8*lMaxbytes/10240; 
	     
	  	if(lRat<=1){
					unit=1;
					strcpy(caUnit,"bps");
					
			 }
				else if(lRat>1&&lRat<=1024){
					unit=1024;
					strcpy(caUnit,"Kbps");
				}
				else if(lRat>1024&&lRat<=1024*1024){
					unit=1024*1024;
					strcpy(caUnit,"Mbps");
				}
				else if(lRat>1024*1024){
					unit=1024*1024*1024;
					strcpy(caUnit,"Gbps");
				}    
				else{
					unit=1;
					strcpy(caUnit,"bps");
				}
	  
	  
	/*  
				if(lMaxbytes*8<=1024){
					unit=1;
					strcpy(caUnit,"bps");
				}
				else if(1024<lMaxbytes*8<=1024*1024){
					unit=1024;
					strcpy(caUnit,"Kbps");
				}
				else if(1024*1024<lMaxbytes*8<=1024*1024*1024){
					unit=1024*1024;
					strcpy(caUnit,"Mbps");
				}
				else if(1024*1024*1024<lMaxbytes*8){
					unit=1024*1024*1024;
					strcpy(caUnit,"Gbps");
				}    
				else{
					unit=1;
					strcpy(caUnit,"bps");
				} 
*/


//if(lMaxconntime>0){
//   lMaxrate=8*lMaxbytes/(lMaxconntime*unit);
//  }
//  else{
//  	lMaxrate=10;
//  }
  
 // utPltPutVarF(psDbHead,"maxrate","%d",lMaxrate);


//���X��̶�

	  for(mm=0;mm<x_sum;mm++){
	  		utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
	  		if(x_sum==30){
	  			sprintf(caTemp,"%s:%02d",caHour,2*mm);
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
	  		}
	  		else{
	  			sprintf(caTemp,"%02d:00",mm);
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
	  		}
	  }
	   
  

	    
	    
	    for(mm=0;mm<iNum;mm++){
	     	utPltPutLoopVarF(psDbHead,"iNum",mm+1,"%lu",mm);
      	utPltPutLoopVar(psDbHead,"devname",mm+1,devname[mm].caDevname);
      	
    
      	
      		for(nn=0;nn<y_sum;nn++){
      			
   			
//printf("nn=%d,ubytes=%llu,conntime=%d,uinit=%d\n",nn,devname[mm].lUbytes[nn],devname[mm].lConntime[nn],unit);     			
     		   utPltPutLoopVarF2(psDbHead,"num",mm+1,nn+1,"%d",mm);
      		 utPltPutLoopVarF2(psDbHead,"num2",mm+1,nn+1,"%d",nn);
     			if(x_sum==30){
	  			   sprintf(caTemp,"%s:%02d",caHour,nn);
	 		    }  
  		    else{
  		    	sprintf(caTemp,"%02d:00",nn);
	  		    } 
    		  utPltPutLoopVar2(psDbHead,"sdate",mm+1,nn+1,caTemp); 
      		   
      		   
      		   if(devname[mm].lConntime[nn]==0){
 //���ĳ������û�в�����ǰ�������ȡǰ��ƽ��
      		   	if(nn>0&&nn<y_sum-1&&devname[mm].lConntime[nn-1]!=0&&(devname[mm].lConntime[nn+1]!=0)){
      		   		rate=8*(devname[mm].lUbytes[nn-1]+devname[mm].lUbytes[nn+1])/(devname[mm].lConntime[nn-1]+devname[mm].lConntime[nn+1])/unit;
      		      utPltPutLoopVarF2(psDbHead,"data",mm+1,nn+1,"%0.2f",rate);
      		      tot[nn]=tot[nn]+rate;
      		   	}
      		   	else
      		      utPltPutLoopVarF2(psDbHead,"data",mm+1,nn+1,"%d",0);
      		    }
      		   else{
//      		   	printf("nn=%d,ubytes=%llu,conntime=%d,uinit=%d\n",nn,devname[mm].lUbytes[nn],devname[mm].lConntime[nn],unit);
                rate=8*devname[mm].lUbytes[nn]/devname[mm].lConntime[nn]/unit;
      		      utPltPutLoopVarF2(psDbHead,"data",mm+1,nn+1,"%0.2f",rate);
      		      tot[nn]=tot[nn]+rate;
             }

    			}
    	}
    		
	    
	      //���
	    for(i=0;i<y_sum;i++){
	    	if(x_sum==30){
	  			sprintf(caTemp,"%s:%02d",caHour,i);
	  		  utPltPutLoopVar(psDbHead,"sdatea",i+1,caTemp);
	  		}
	  		else{
	  			sprintf(caTemp,"%02d:00",i);
	  		  utPltPutLoopVar(psDbHead,"sdatea",i+1,caTemp);
	  		}
	  		utPltPutLoopVarF(psDbHead,"tot",i+1,"%0.2f",tot[i]);
	    }
	    
	    
	    
	    
    
    utPltPutVarF(psDbHead,"sum","%d",iNum);       //������Ŀ
    utPltPutVarF(psDbHead,"sum_d","%d",count_d);     //���ݼ�¼��
    utPltPutVar(psDbHead,"unit",caUnit);
    sprintf(caTemp,"%s(%s)",ncLang("1800����ӿ���������ͼ"),caUnit);
    utPltPutVar(psDbHead,"unit_title",caTemp);
    utPltPutVar(psDbHead,"stime",utTimFormat("%Y/%m/%d %H",lStarttime+100));
    
    if(strlen(caPdf)>0){
    	desc=malloc((iNum+1)*sizeof(char*));
    	for(i=0;i<iNum;i++){
    		desc[i]=malloc(32);
    		strcpy(desc[i],devname[i].caDevname);
    	}
    	 //�����PDF��     
	   pChart=(struct chart_datasetf*)malloc(sizeof(struct chart_datasetf)*(y_sum+1));
	    if(pChart==NULL){
	  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("1863�����ڴ����"));
	      return 0;
	   }
    for(i=0;i<y_sum;i++){
    	if(y_sum==60)
    	   sprintf(caTemp,"%s:%02d",caHour,i);
    	else
    	   sprintf(caTemp,"%02d:00",i);
    	pChart[i].name=strdup(caTemp);
    	pChart[i].dataset=malloc((iNum+2)*sizeof(float)) ;
    	for(mm=0;mm<iNum;mm++){
    		 if(devname[mm].lConntime[i]==0){
 //���ĳ������û�в�����ǰ�������ȡǰ��ƽ��
      		   	if(i>0&&i<y_sum-1&&devname[mm].lConntime[i-1]!=0&&(devname[mm].lConntime[i+1]!=0)){
      		   		rate=8*(devname[mm].lUbytes[i-1]+devname[mm].lUbytes[i+1])/(devname[mm].lConntime[i-1]+devname[mm].lConntime[i+1])/unit;
      		      pChart[i].dataset[mm]=rate;
      		    
      		   	}
      		   	else
      		      pChart[i].dataset[mm]=0;
      		    }
      		   else{
//      		   	printf("nn=%d,ubytes=%llu,conntime=%d,uinit=%d\n",nn,devname[mm].lUbytes[nn],devname[mm].lConntime[nn],unit);
                rate=8*devname[mm].lUbytes[i]/devname[mm].lConntime[i]/unit;
      		      pChart[i].dataset[mm]=rate;      		      
             }
    		
    		
    	}
    	
    }
    	
    	rowrow=y_sum;
      column=iNum;
    	
    sprintf(caTitle,ncLang("1410����ӿ�����ͼ"));
    sprintf(filename,"%s.PDF",caTitle); 

    sprintf(caStatdate,ncLang("1967ͳ��ʱ��:%s"),utTimFormat("%Y/%m/%d %H",lStarttime+100));
    sprintf(caTemp,ncLang("1942��λ(%s)"),caUnit);
   ncWebShowServiceBytime_PDF(iFd,pChart,desc,rowrow,column,caTitle,caTemp,caStatdate,filename);

    //�ͷ��ڴ�     
   for(i=0;i<rowrow;i++){
   	 free(pChart[i].name);       
	   free(pChart[i].dataset);
	 } 
    
   if(pChart) free(pChart);
   
   for(i=0;i<iNum;i++){
   	 
   	   free(desc[i]);
   }
   if(desc) free(desc);
    
     utPltFreeDb(psDbHead);  
     return 0;
    	
    	
    	
    	
    	
    	
    	
    }
    
    
    
    
    
    
  if(atol(caFlag)==1)
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_mainfirst_outflowtab.htm");
  else
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_mainfirst_outflowmore.htm");
    
    return 0;
}




//7.5�汾CPU��Ϣ
int ncWebShowCpu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    int iReturn;
    char caTemp[64];
    unsigned long mem,freemem,swap,freeswap;
    psDbHead = utPltInitDb();
      iReturn = pasGetMemInfo(&mem, &freemem,&swap,&freeswap);
    if(iReturn == 0 ) {
        if( mem > 0) {
           
            utPltPutVarF(psDbHead,"mem","%s",utStrLtoF(mem,caTemp));
            utPltPutVarF(psDbHead,"free","%s",utStrLtoF(freemem,caTemp));
        }
        
        if(swap > 0) {
          
            utPltPutVarF(psDbHead,"swap","%s",utStrLtoF(swap,caTemp));
            utPltPutVarF(psDbHead,"freeswap","%s",utStrLtoF(freeswap,caTemp));
        }
    } 
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_mainfirstleft_cpu.htm");
    
    return 0;
}

//7.5�汾CPU��Ϣ
int ncWebShowCpuAjx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int iReturn;
	unsigned long mem,freemem,swap,freeswap;
	long lCpunum;
	double fRate;
	double rate,cpu;
	char caTemp[32];
    utPltDbHead *psDbHead;
    psDbHead = utPltInitDb();
    
    lCpunum=atol(utComGetVar_sd(psShmHead,"cpunumber","1"));
    
        /* ��ǰ�������  */    
    iReturn = pasGetCpuRate("nc",&cpu,&mem);

    if(iReturn >= 0) {
        fRate = cpu / lCpunum;
       
        utPltPutVarF(psDbHead,"cpu","%0.0f",fRate/4); 
        
      sprintf(caTemp,"%0.1f%c",fRate,'%');  
       
        utPltPutVar(psDbHead,"cpurat",caTemp);
    }
    iReturn = pasGetMemInfo(&mem, &freemem,&swap,&freeswap);
    if(iReturn == 0 ) {
        if( mem > 0) {
            fRate = (double) (mem - freemem)/ (double) mem;
            rate=fRate*100;
            sprintf(caTemp,"%0.1f%c",rate,'%');
            utPltPutVar(psDbHead,"memrat",caTemp);
            
            utPltPutVarF(psDbHead,"mem","%0.0f",rate/4);
        
            
        }
        
        if(swap > 0) {
            fRate = (double) (swap - freeswap)/(double) swap;
            
            rate=fRate * 100;
            sprintf(caTemp,"%0.1f%c",rate,'%');
            
            utPltPutVar(psDbHead,"swaprate",caTemp);
            utPltPutVarF(psDbHead,"swap","%0.0f",rate/4);
            
           
        }
    }
    
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/cpu_ajx.htm");
    
    return 0;
}


/* ��ʾ����ʱ��  */
int ncWebWorkTimeList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caMessage[128];
    char caModi[16],caDel[16],caSid[16],caAdd[16];
    char caApp[16];
    char *p;
    unsigned long lSid;
    int i,iNum,iReturn;
    utPltDbHead *psDbHead;
  
  char caSql_t[1024];  
   
    
    
//   	long lStartRec,lSumUser,lCount,lCurPg,lTotRec,lRowNum,lSid1,lId1;
   	long lUdate,lLdate;
    char caTimetype[16],caLower[16],caUpper[16],caStarttime[16],caStoptime[16],caFlags[8];
    long lTimetype,lLower,lUpper,lStarttime,lStoptime,lFlags;
 	pasDbCursor *psCur;
 	
 	
// 	 strcpy(caSql_t,ncUtlCvtWorkTime2Sql("a.stime"));
 //strcpy(caSql_t,ncUtlCvtWorkTime2Sql_s("a.sdate","a.stime"));
//    printf("caSql=%s\n",caSql_t);
 	
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebWorkTimeList");
 	}
 	
	iReturn = utMsgGetSomeNVar(psMsgHead,11,
	                "timetype",      UT_TYPE_STRING, 10,caTimetype,
	                "add",        UT_TYPE_STRING, 10, caAdd,
	                "modi",       UT_TYPE_STRING, 10, caModi,
	                "app",        UT_TYPE_STRING, 10, caApp,
	                "del",        UT_TYPE_STRING, 10, caDel,
	                "sid",        UT_TYPE_STRING, 10, caSid,
	                "llower",       UT_TYPE_STRING, 10, caLower,
	                "lupper",      UT_TYPE_STRING, 10, caUpper,
	                "starttime",      UT_TYPE_STRING, 10, caStarttime,
	                "stoptime",       UT_TYPE_STRING, 10, caStoptime,
	                "flags",        UT_TYPE_STRING,4,caFlags);
    if(!utStrIsSpaces(caAdd)) {  /* ���Ӽ�¼  */           
        lLdate = ncLimCheckValidDate(caTimetype,caLower,caMessage);
        if(lLdate < 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0462���ڵ�����"),
                             caMessage);
            return 0;
        }            
        if(utStrIsSpaces(caUpper)) {
            lUdate = lLdate;
        }
        else {
            lUdate = ncLimCheckValidDate(caTimetype,caUpper,caMessage);
            if(lUdate < 0) {
                 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0462���ڵ�����"),
                             caMessage);
                 return 0;
            }
        }  
        lStarttime = ncUtlIsValidTime(caStarttime);
        lStoptime = ncUtlIsValidTime(caStoptime);
        if(lStarttime < 0 || lStoptime < 0) {
              utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245��������"),ncLang("0435��ʼ����ֹʱ�䲻��ȷ"));
              return 0;
        }
        lSid = pasGetSid("ncworktime","sid");
        iReturn = pasDbExecSqlF("insert into ncworktime(sid,timetype,llower,lupper,starttime,stoptime,flags) values (%lu,%s,%lu,%lu,%d,%d,%d)",
                                             lSid,caTimetype,lLdate,lUdate,lStarttime,lStoptime,atol(caFlags));
        if(iReturn != 0) {
            sprintf(caMessage,"insert into ncworktime(sid,timetype,llower,lupper,starttime,stoptime,flags) values (0,%s,%lu,%lu,%d,%d,%d)",
                                             caTimetype,lLdate,lUdate,lStarttime,lStoptime,atol(caFlags));
            utSysLog("Sql:%s",caMessage);                                             
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip��ַ�б�"),ncLang("0314��¼���ӳ���, ����Ϊ��¼�ظ�"));
            pasDbRollback(NULL);
            return 0;
        }        
        else {
            pasDbCommit(NULL);
        }
    }
    else if(!utStrIsSpaces(caDel)) { /* ɾ����¼  */
        char caVar[16];
   	    for(i=0;i<50;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
                iReturn = pasDbExecSqlF("delete from ncworktime where sid = %s",p);
                if(iReturn == 0) {
                    pasDbCommit(NULL);
                }
   	        }
   	    }
    }
    else if(!utStrIsSpaces(caModi)) { /* �޸ļ�¼  */       
        lLdate = ncLimCheckValidDate(caTimetype,caLower,caMessage);
        if(lLdate < 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0462���ڵ�����"),
                             caMessage);
            return 0;
        }            
        if(utStrIsSpaces(caUpper)) {
            lUdate = lLdate;
        }
        else {
            lUdate = ncLimCheckValidDate(caTimetype,caUpper,caMessage);
            if(lUdate < 0) {
                 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0462���ڵ�����"),
                             caMessage);
                 return 0;
            }
        }   
        
        lStarttime = ncUtlIsValidTime(caStarttime);
        lStoptime = ncUtlIsValidTime(caStoptime);
        if(lStarttime < 0 || lStoptime < 0) {
              utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245��������"),ncLang("0435��ʼ����ֹʱ�䲻��ȷ"));
              return 0;
        }
        
           
        iReturn = pasDbExecSqlF("update ncworktime set timetype=%d,llower=%d,lupper=%d,starttime=%d,stoptime=%d,flags=%d where sid=%s",
                               atol(caTimetype),lLdate,lUdate,lStarttime,lStoptime,atol(caFlags),caSid);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip��ַ�б�"),ncLang("0314��¼���ӳ���, ����Ϊ��¼�ظ�"));
            pasDbRollback(NULL);
            return 0;
        }        
        else {
            pasDbCommit(NULL);
        }
    }
    else if(!utStrIsSpaces(caApp)) { /* Ӧ��  */
        ncWebLimApp(psShmHead,iFd,psMsgHead);
        return 0;
    }

     psCur = pasDbOpenSqlF("select sid,timetype,llower,lupper,starttime,stoptime,flags  from ncworktime  order by sid " );
 
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
        return 0;
    }
    psDbHead = utPltInitDb();
    
    utPltPutVar(psDbHead,"starttime","00:00:00");
    utPltPutVar(psDbHead,"stoptime","23:59:59");
    lSid=0;
    lTimetype=0;
    lLower=0;
    lUpper=0;
    lStarttime=0;
    lStoptime=0;
    lFlags=0;
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lTimetype,
      	                           UT_TYPE_LONG,4,&lLower,
      	                           UT_TYPE_LONG,4,&lUpper,
      	                           UT_TYPE_LONG,4,&lStarttime,
      	                           UT_TYPE_LONG,4,&lStoptime,
      	                           UT_TYPE_LONG,4,&lFlags);
      	                          
   	iNum = 0;
   	while(iReturn == 0) {
        iNum++;
        switch(lTimetype) {
            case NC_LIMDATE_DAY:
                utPltPutLoopVar(psDbHead,"stype",iNum,ncLang("0395ĳ��"));
                break;
            case NC_LIMDATE_WEEK:
                utPltPutLoopVar(psDbHead,"stype",iNum,ncLang("0386ÿ��"));
                break;
            case NC_LIMDATE_MONTH:
                utPltPutLoopVar(psDbHead,"stype",iNum,ncLang("0385ÿ��"));
                break;
            case NC_LIMDATE_YEAR:
                utPltPutLoopVar(psDbHead,"stype",iNum,ncLang("0381ÿ��"));
                break;
        }
        utPltPutLoopVar(psDbHead,"sdate",  iNum,ncLimDateCvt(lTimetype,lLower));
        utPltPutLoopVar(psDbHead,"edate",  iNum,ncLimDateCvt(lTimetype,lUpper));

        utPltPutLoopVarF(psDbHead,"timetype", iNum,"%lu",lTimetype);
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%lu",iNum);
        utPltPutLoopVar(psDbHead,"starttime",iNum, ncLimCvtTime(lStarttime));
        utPltPutLoopVar(psDbHead,"stoptime",iNum, ncLimCvtTime(lStoptime));
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%d",lSid);
        if(lFlags==0)
         utPltPutLoopVar(psDbHead,"flags_desc",iNum,ncLang("1806�ϰ�ʱ��"));
         else
         utPltPutLoopVar(psDbHead,"flags_desc",iNum,ncLang("1807���ϰ�ʱ��"));
         
         utPltPutLoopVarF(psDbHead,"flags",iNum,"%d",lFlags);
        
		     lSid=0;
		    lTimetype=0;
		    lLower=0;
		    lUpper=0;
		    lStarttime=0;
		    lStoptime=0;
		    lFlags=0;
		    iReturn = pasDbFetchInto(psCur,
		      	                           UT_TYPE_LONG,4,&lSid,
		      	                           UT_TYPE_LONG,4,&lTimetype,
		      	                           UT_TYPE_LONG,4,&lLower,
		      	                           UT_TYPE_LONG,4,&lUpper,
		      	                           UT_TYPE_LONG,4,&lStarttime,
		      	                           UT_TYPE_LONG,4,&lStoptime,
		      	                           UT_TYPE_LONG,4,&lFlags);
    }
    pasDbCloseCursor(psCur);
    i = 0;
    while(iNum < 30 + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"defworktime",utComGetVar_sd(psShmHead,"DefWorkDayTime","WorkTime"));
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/nc_worktimelist.htm");
    return 0;
}


/* ��ʾ����������ǰ15����*/
int ncWebShowServiceClassFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char sqlbuf[1024];
    utPltDbHead *psDbHead;
    unsigned long lTime;
    unsigned long long lBytes;
//    char serv_name[32];
    long iNum;
    long long lSum;
    char caDate[20];
    char caDate_end[20];
    char caTemp[1024];
    int iReturn,i;
//    char caRefresh_flag[8];
    struct ncFlowService_s {
        unsigned long lService;
        unsigned long long lBytes;
    } sService[10];
    long long lBytes_other;
    char caName[32];
    long lGroupid;
//    char preflag[8],nextflag[8],ctime[12];
    char caServ_flag[12];
    
    char caPdf[32];
//    long lRowrow;
    struct piechart_datasetf *pChart;           //����PDF�ĵ����
//   char *desc[]={"�û���","����","����","�ٷֱ�"};
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
   lTime=time(0);
   lSum=0;
    utMsgGetSomeNVar(psMsgHead,2,"serv_flag",UT_TYPE_STRING,8,caServ_flag,
                                 "pdf_flag", UT_TYPE_STRING,8,caPdf);
                     

  	psDbHead = utPltInitDb();
 
 
  	strcpy(caDate,utTimFormat("%H:%M",lTime)); 
  	strcpy(caDate_end,utTimFormat("%Y/%m/%d %H:%M",lTime-900));
  	utPltPutVarF(psDbHead,"stime","%d",lTime-900);
  	utPltPutVarF(psDbHead,"etime","%d",lTime);
  	utPltPutVar(psDbHead,"flag",caServ_flag);
  

  	
  	   if(atol(caServ_flag)==2){
  	   	  utPltPutVar(psDbHead,"title",ncLang("1808������������ֲ�ͼ"));
  	   		sprintf(sqlbuf,"select sum(ubytes+dbytes) aa,b.groupid,c.name from nciplog a,ncipindex b,ncservicecgroup c where vtime>=%d and a.service=b.sid and b.groupid=c.groupid  group by b.groupid order by aa desc  ",lTime-900);
  	   }
  	   else{
  	   	utPltPutVar(psDbHead,"title",ncLang("1809ϸ�ַ��������ֲ�ͼ"));
  	   	sprintf(sqlbuf,"select sum(ubytes+dbytes) aa,service,b.name from nciplog a,ncipindex b where vtime>=%d and a.service=b.sid group by service order by aa desc ",lTime-900);
  	  }
 //  sprintf(sqlbuf,"select sum(ubytes+dbytes) byt,service from ncipservice where sdate='%s' group by service order by byt desc ",caDate);
printf("sqlbuf=%s\n",sqlbuf);
   lBytes_other=0;
   psCur=pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
        return 0;
       }
      
        iReturn = 0;
        lBytes=0;
        lGroupid=0;
        memset(caName,0,sizeof(caName));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&lGroupid,
                                       UT_TYPE_STRING,30,caName);                               
	     iNum=0;
	     
	     if(atol(caPdf)==2){                                                //����PDF
	     		//����ǩ
			 desc=malloc(3*sizeof(char*));
			 for(i=0;i<3;i++){
			 	desc[i]=malloc(32);
		   }
		   strcpy(desc[0],ncLang("1647�������"));		  
		   sprintf(desc[1],"%s(%s)",ncLang("1103������"),"KB");
		   strcpy(desc[2],ncLang("1827�ٷֱ�%"));
	     	 //�����PDF��     
		   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*12);
		    if(pChart==NULL){
		  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("1863�����ڴ����"));
		      return 0;
		   }
		  
		   iNum=0;
   
	     	
	     	
	     	while((iReturn==0)||(iReturn==1405)&&iNum<10)
        {
  			if(iNum<10){
  				pChart[iNum].name=strdup(caName);
  				pChart[iNum].amount=(float)lBytes/1024; 	
  			 if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
          sService[iNum].lService=lGroupid;
          sService[iNum].lBytes=lBytes;
           iNum++;
        }
        else{
        	//�����ŵ�������
        	lBytes_other=lBytes_other+lBytes;
        }
         lSum=lSum+lBytes;
				
        lBytes=0;
        lGroupid=0;
        memset(caName,0,sizeof(caName));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&lGroupid,
                                       UT_TYPE_STRING,30,caName); 
	        
	  
	     }
       pasDbCloseCursor(psCur); 
       //�������
        for(i=0;i<iNum;i++){
        	  if(lSum==0){
        	  	sprintf(caTemp,"0.00");		    	
			    	}
			    	else{
			    		 sprintf(caTemp,"%0.2f",(float)100*sService[i].lBytes/lSum);			    	  
			    	}
			    	pChart[i].percent=strdup(caTemp);
        }
       
	     	//�������
	     	 if(lBytes_other>0){
	  	//�������
	  	 		pChart[iNum].name=strdup(ncLang("1700�������"));
  				pChart[iNum].amount=lBytes_other/1024; 	
  				if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
	  	    if(lSum==0){
        	  	sprintf(caTemp,"0.00");		    	
			    	}
			    	else{
			    		 sprintf(caTemp,"%0.2f",(float)100*lBytes_other/lSum);			    	  
			    	}
			    	pChart[iNum].percent=strdup(caTemp);
	  	     
	  	     iNum++;
	         
	        }
	     	
	     	rowrow=iNum;
   
    
        sprintf(filename,"%s.PDF","service"); 

      strcpy(caTitle,ncLang("1456�������ʹ�����"));
      sprintf(caStatdate,ncLang("1934ͳ��ʱ�䣺��%s ��%s"),caDate_end,caDate);
      
      
       ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,3,0, caTitle,ncLang("1955��λ(KB)"),caStatdate,filename); 
	     	
	     	//�ͷ��ڴ�     
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
	     
	   
//���������	   
	   
        while((iReturn==0)||(iReturn==1405)&&iNum<10)
        {
  			if(iNum<10){
         utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	lBytes);        
         utPltPutLoopVar(psDbHead,"name",  iNum+1,caName);
         utPltPutLoopVarF(psDbHead,"groupid",iNum+1,"%d",lGroupid);
         utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",lTime);
         utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
         sService[iNum].lService=lGroupid;
         sService[iNum].lBytes=lBytes;
          iNum++;
        }
        else{
        	//�����ŵ�������
        	lBytes_other=lBytes_other+lBytes;
        }
         lSum=lSum+lBytes;
         
         
        
				
        lBytes=0;
        lGroupid=0;
        memset(caName,0,sizeof(caName));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                       UT_TYPE_LONG,4,&lGroupid,
                                       UT_TYPE_STRING,30,caName); 
	        
	  
	        }
       pasDbCloseCursor(psCur); 

  if(lBytes_other>0){
  	//�������
  	     utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	lBytes_other);        
         utPltPutLoopVar(psDbHead,"name",  iNum+1,ncLang("1700�������"));
         utPltPutLoopVarF(psDbHead,"groupid",iNum+1,"%d",9999);
         utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",lTime);
         utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
         
         if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    			}
    			else{
    	  		utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*lBytes_other/lSum);
    			}
         
         
  }
  
    utPltPutVar(psDbHead,"today",caDate);  
    utPltPutVar(psDbHead,"endtime",caDate_end);
    //�������
    for(i=0;i<iNum;i++){
    	if(lSum==0){
    		utPltPutLoopVar(psDbHead,"rate",i+1,"0.00");
    	}
    	else{
    	  utPltPutLoopVarF(psDbHead,"rate",i+1,"%0.2f",(float)100*sService[i].lBytes/lSum);
    	}
    }       
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_servicecalssfirstbt.htm");  
    return 0;
}


/* ����ID��ȡ�±�*/
int ncUtlGetServicenumById(struct ncSflow_s *arrIn,int iSumArr,unsigned long lService)
{
   int bottom,middle,top;
//    int n;
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



/* ��ʾ��������ѯ*/
int ncWebShowServiceFirstCx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    char caDate_end[20];
    int iReturn,i;
    char caRefresh_flag[8];
    struct ncSflow_s  sService[500];
    struct ncSflow_s  sGservice[40];
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
    
     struct piechart_datasetf *pChart;           //����PDF�ĵ����
//   char *desc[]={"�û���","����","����","�ٷֱ�"};
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
   lTime=time(0);
   lSum=0;
   lRecenttime=0;
   //~~~~~~~~~~~~~~~excel~~~����~~~~~~~~~~~~~~~
   char expexcel[12]="";
   char f_name[120]="";
   char cp_command[320]="";
   char rm_command[320]="";
   char caSdate_All[20],caEdate_All[20];
  	FILE *fp; 
      utMsgGetSomeNVar(psMsgHead,9,"gitem",UT_TYPE_STRING,10,caGitem,
                                 "sdate",UT_TYPE_STRING,12,caSdate,
                                 "shour",UT_TYPE_STRING,2,caShour,
                                 "smin", UT_TYPE_STRING,2,caSmin,
                                 "edate",UT_TYPE_STRING,12,caEdate,
                                 "ehour",UT_TYPE_STRING,2,caEhour,
                                 "emin", UT_TYPE_STRING,2,caEmin,
                                 "exppdf",UT_TYPE_STRING,8,caPdf,
                                 "expexcel",UT_TYPE_STRING,10,expexcel);
  
   strcpy(caSdate_All,utTimFormat("%Y-%m-%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate))); 
   strcpy(caEdate_All,utTimFormat("%Y-%m-%d",utTimStrToLong("%Y/%m/%d %H:%M",caEdate)));
 //~~~~~~~~~~~~~~~~~~~~~~~~~����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   
 //�Ƚ����������Ϣ�ŵ��ڴ�
    sprintf(caTemp,"select a.sid,a.groupid from ncipindex a ");
    if((atol(caGitem)!=100)&&(atol(caGitem)!=101)&&strlen(caGitem)>0){
    	sprintf(caTemp+strlen(caTemp)," where a.groupid=%d ",atol(caGitem));
    }
    sprintf(caTemp+strlen(caTemp)," order by a.sid limit 0,500 ",0);
    
   	 psCur=pasDbOpenSql(caTemp,0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
        return 0;
     }
     lGroupid=0;
     lSid=0;
     memset(caName,0,sizeof(caName));
     lSumindex=0;
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                    UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      while((iReturn==0)||(iReturn==1405))
      {
      	sService[lSumindex].lService=lSid;
      	sService[lSumindex].lGroupid=lGroupid;

      	sService[lSumindex].lBytes=0;
      	
      	lSumindex++;
      	lGroupid=0;
      	lSid=0;
        memset(caName,0,sizeof(caName));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                    UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      }                               
      pasDbCloseCursor(psCur); 
 
 
 //�����������Ϣ����sGservice ��
   	 psCur=pasDbOpenSql("select groupid,name from ncservicecgroup order by groupid ",0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
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
 
 
   if((strlen(caSdate)==0)||(strlen(caEdate)==0)){
  	   strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime)); 
  	   strcpy(caEdate,utTimFormat("%Y/%m/%d",lTime));
  	   strcpy(caShour,utTimFormat("%H",lTime-3600));
  	   strcpy(caSmin,utTimFormat("%M",lTime-3600));
  	   
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
  	
    sprintf(caStatdate,ncLang("1952ͳ��ʱ�䣺��%s %s:%s ��%s %s:%s"),	caSdate,caShour,caSmin,caEdate,caEhour,caEmin);	  
  	 utPltPutVar(psDbHead,"shour",caShour);
  	 utPltPutVar(psDbHead,"smin",caSmin);
  	 utPltPutVar(psDbHead,"ehour",caEhour);
  	 utPltPutVar(psDbHead,"emin",caEmin);
  	 utPltPutVar(psDbHead,"sdate",caSdate);
  	 utPltPutVar(psDbHead,"edate",caEdate);
  	 utPltPutVar(psDbHead,"gitem",caGitem);
 
 //��ȡncipservice�����������ֵ 	���ֵ�����ж϶�ʱͳ�Ƴ���ǰ�Ѿ�����ķ������ݣ��������ʱ���û��������Ҫ��IPLOG�л�ȡ 
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
 //ͳ��IPlog�еķ���
      if(lStime>lRecenttime){
      	lStime1=lStime;
      }
      else{
      	lStime1=lRecenttime;
      }
      sprintf(caTemp,"select sum(ubytes+dbytes),service from nciplog where vtime>%d and vtime<=%d group by service order by service limit 0,500",lStime1,lEtime);
      printf("caTemp=%s\n",caTemp);
      psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
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
  	 
  	  //ͳ��ncIPSERVICE�е�����
  	  if(lStime<lRecenttime){           //ͳ�ƿ�ʼʱ����ncipservice �е����ʱ��ǰ������
  	  	if(strcmp(caSdate,caEdate)!=0)
  	  	    sprintf(caTemp,"select sum(ubytes+dbytes),service from ncipservice where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s')) group by service ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	  	else{
  	  		sprintf(caTemp,"select sum(ubytes+dbytes),service from ncipservice where sdate='%s' and stime>='%s' and stime<='%s' group by service ",caSdate,caShour,caEhour);
  	  		
  	  	}
  	  	printf("caTemp=%s\n",caTemp);
  	  psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
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
  	 
  //����������	 
  	for(i=0;i<lSumGindex;i++){
      	utPltPutLoopVarF(psDbHead,"gid",i+1,"%d",sGservice[i].lService);
      	utPltPutLoopVar(psDbHead,"gname",i+1,sGservice[i].caGname);
      	

      }                               
     
  	
  	
  	if(atol(caGitem)==100){
  		//�������
  		strcpy(caTitle,ncLang("1829������������ֲ����"));
  	      utPltPutVar(psDbHead,"title",ncLang("1808������������ֲ�ͼ"));
  	      utPltPutVar(psDbHead,"title_1",ncLang("1810������������ֲ���"));
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
 //���� 	
 
 
    
  	    qsort(sGservice,lSumGindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId); 
  	    iNum=0;
  	   
  	    for(i=0;i<lSumGindex;i++){
  	    	if(sGservice[i].lBytes>0){
  	    
  	    //��������	 
  	    		 utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF8(sGservice[i].lBytes/1024,caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	    		 utPltPutLoopVar(psDbHead,"name_1",  iNum+1,sGservice[i].caGname);
  	    		if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
    				}
  	    		  
  	    //�����ͼ		  
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
     
    // �������
     if(lOth>0){
     	  	   utPltPutLoopVarF(psDbHead,"bytes",lSj+1,"%llu",	lOth);  
             utPltPutLoopVar(psDbHead,"name",  lSj+1,ncLang("0431����"));
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
  	 else {
  	 	if(atol(caGitem)==101){
  	 		   strcpy(caTitle,ncLang("1830"));
  	 	  	 utPltPutVar(psDbHead,"title",ncLang("1809ϸ�ַ��������ֲ�ͼ"));
  	 	  	 utPltPutVar(psDbHead,"title_1",ncLang("1811ϸ�ַ��������ֲ���"));
  	 	 }
  	 	 else{
  	 	 	sprintf(caTemp,"select name from ncservicecgroup where groupid=%d ",atol(caGitem));
  	 	 	memset(caName,0,sizeof(caName));
  	 	 	
  	 	 	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caName);
  	 	 	
  	 	 	sprintf(caTitle,ncLang("1831%s�����ֲ����"),caName);
  	 	 	sprintf(caTemp,ncLang("1812%s�����ֲ�ͼ"),caName);
  	 	 	utPltPutVar(psDbHead,"title",caTemp);
  	 	 	
  	 	 	sprintf(caTemp,ncLang("1813%s�����ֲ���"),caName);
  	 	 	utPltPutVar(psDbHead,"title_1",caTemp);
  	 	 }
  	 	//����
  	 	 	 qsort(sService,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId);  
 //��������
          lSum=0; 	 	 	 
  	 	 	  for(i=0;i<lSumindex;i++){
  	      	lSum=lSum+sService[i].lBytes;
  	      	
  	       }
  	 	 	 
  	 	 	 
  	 	 	 iNum=0;
  	 	 	
  	 	 	 for(i=0;i<lSumindex;i++){
  	 	 	 	if(sService[i].lBytes>0){
  //	printf("i=%d\n",i); 	 	 		
  	 	 	 		  	    //��������	 
  	    		 utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF8(sService[i].lBytes/1024,caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
 	    		   utPltPutLoopVar(psDbHead,"name_1",  iNum+1,ncUtlGetServiceName(psShmHead,sService[i].lService,ncLang("0431����")));
  	    		 if(lSum==0){
    					 utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				 }
    				 else{
    	  			  utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*sService[i].lBytes/lSum);
    				 }
  	 	 	 		 //�����ͼ		  
  	    		 if(i<10) {
  	    		  lSj=lSj+1;
  	    		 utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	sService[i].lBytes);  
             utPltPutLoopVar(psDbHead,"name",  iNum+1,ncUtlGetServiceName(psShmHead,sService[i].lService,ncLang("0431����")));
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
  	 	      // �������
     if(lOth>0){

     	  	   utPltPutLoopVarF(psDbHead,"bytes",lSj+1,"%llu",	lOth);  
             utPltPutLoopVar(psDbHead,"name",  lSj+1,ncLang("0431����"));
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
	//����ǩ
	 desc=malloc(3*sizeof(char*));
	 for(i=0;i<3;i++){
	 	desc[i]=malloc(32);
   }
		   strcpy(desc[0],ncLang("1647�������"));		  
		   sprintf(desc[1],"%s(%s)",ncLang("1103������"),"KB");
		   strcpy(desc[2],ncLang("1827�ٷֱ�%"));
     
 //�����PDF��     
   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*12);
    if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("1863�����ڴ����"));
      return 0;
   }
  
   iNum=0;
   


   	
   for(i=0;i<lSj;i++){
   	   
   	 if(iNum<10){
   	 	if(atol(caGitem)==100){                                              //�������
   	 	  pChart[iNum].name=strdup(sGservice[i].caGname);
  			pChart[iNum].amount=(float)sGservice[i].lBytes/1024; 	
  			 if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
          if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
   	 			}
 	 		
   	 			pChart[iNum].percent=strdup(caTemp); 
           iNum++;
        }
       else{      	
       	pChart[iNum].name=strdup(ncUtlGetServiceName(psShmHead,sService[i].lService,ncLang("0431����")));                       //ϸ�ַ���
  			pChart[iNum].amount=(float)sService[i].lBytes/1024; 	
  			 if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
          if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*sService[i].lBytes/lSum);
   	 			}
 	 		
   	 			pChart[iNum].percent=strdup(caTemp); 
           iNum++;
       	
      }
       
     }
  }
  	         // �������
     if(lOth>0){
     		pChart[iNum].name=strdup(ncLang("0431����"));  
     	   	if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lOth)/lSum);
   	 			}
   	 			
     	   	pChart[iNum].percent=strdup(caTemp); 
   	 		
   	 			pChart[iNum].amount=(float)lOth/1024;
                  
          iNum++;
                                     
     }
     
     
     
     
     
      rowrow=iNum;
   
    
     
    sprintf(filename,"%s.PDF",caTitle); 

//printf("rowrow=%d\n",rowrow);
     ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,3,0, caTitle,ncLang("1939��λ:KB"),caStatdate,filename); 
     
//�ͷ��ڴ�     
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
  	 
  	 
  //~~~~~~~~~~~~~~~excel~~~����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	 
	//����excel
   long flag=0;
   if(strlen(expexcel)!=0){			 		
			strcat(f_name,ncLang("1938������������ֲ������ѯ--"));
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
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613��������ͳ��"),ncLang("0546���ݵ�������"));
            return 0;
        }
    	fprintf(fp,ncLang("1954���\t�������\t����(KB)\t�ٷֱ�\n")); 
    	iNum=0; 
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
     flag=1;   
		 fclose(fp); 
  	 utPltFileDownload(iFd,"application/text",caPath,f_name,f_name);
  	 //ɾ��binĿ¼�µ������ļ�
		 strcpy(rm_command,"rm ");
		 strcat(rm_command,caFile);
		 system(rm_command);
		 utPltFreeDb(psDbHead);
   }	 
		
		if(flag!=1){   
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_servicefirstbtcx.htm");  
  	}
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~����~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//7.5���������������ͼ
int ncWebShowServiceBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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

    char caTemp[8192];
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
    char caGitem[16],caSdate[16],caShour[16],caSmin[16];
    struct ncSflow_s  sService[20];                   //��Ŵ�����Ϣ
    struct ncSflow_s  sGservice[20];                  //���ͳ�����ݣ�ǰʮ�ַ������0-10�У���������sGservice ���һ��
    long long lBytes;
    long lService,lGroupid;
    long lSumindex,findex;
    long lSumGindex; 
    long long lRat;
    char caEdate[20];                                 //�����
    char caPdf[12];
    
    struct chart_datasetf *pChart;           //����PDF�ĵ����
    int rowrow;                     
    int column; 
    char caTitle[128];
    char filename[128];
    char caStatdate[128];

    char **desc;
    
    
    
    lMaxbytes=0;                                     
    
    
     //�����������Ϣ����sService ��
   	 psCur=pasDbOpenSql("select groupid,name from ncservicecgroup order by groupid ",0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
        return 0;
     }
     lSumGindex=0;
     
     lGroupid=0;
     memset(caName,0,sizeof(caName));
    
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      while((iReturn==0)||(iReturn==1405))
      {
      	sService[lSumGindex].lService=lGroupid;
      	strcpy(sService[lSumGindex].caGname,caName);
      	sService[lSumGindex].lBytes=0;
      	 for(i=0;i<32;i++){
    		   sService[lSumGindex].lBytesqs[i]=0;
    	    }
      	lSumGindex++;
      	lGroupid=0;
        memset(caName,0,sizeof(caName));
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      }                               
      pasDbCloseCursor(psCur); 
    
    
    
   		utMsgGetSomeNVar(psMsgHead,7,"av_un",UT_TYPE_STRING,2,caAv_un,
																 "pre_flag",UT_TYPE_STRING,2,caPre_flag,
																 "gitem",     UT_TYPE_STRING,10, caGitem,
		                             "sdate",    UT_TYPE_STRING, 14,caSdate,
		                             "shour",    UT_TYPE_STRING,8,caShour,
		                             "smin",     UT_TYPE_STRING,8,caSmin,
		                             "pdf",      UT_TYPE_STRING,8,caPdf);
		
                         
  if(strlen(caGitem)==0){
   	strcpy(caGitem,"101");
  }		                             
		                             
  psDbHead = utPltInitDb(); 
  
  
   //����������	 
  	for(i=0;i<lSumGindex;i++){
      	utPltPutLoopVarF(psDbHead,"gid",i+1,"%d",sService[i].lService);
      	utPltPutLoopVar(psDbHead,"gname",i+1,sService[i].caGname);     	

      }         
  
  	
  if(strlen(caAv_un)==0) strcpy(caAv_un,"2");
		                             
	  if(atol(caAv_un)==2){               //��Сʱͳ��(ncipservice)
	    x_sum=24;
	    y_sum=24;
	  }
	  else{
	    x_sum=30;
	    y_sum=30;
	  }
    
    
	if(atol(caAv_un)==2){                                         //Сʱͳ��
			if(strlen(caSdate)==0){
		    lTime=time(0);
		   
		     strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));
		     strcpy(caShour,"00");
		     strcpy(caSmin,"00");
		  }
		   lStarttime=utTimStrToLong("%Y/%m/%d",caSdate);
		  
		  if(atol(caPre_flag)==1){
		  	lStarttime=lStarttime-24*3600;
		  	strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		  }
		  else if(atol(caPre_flag)==2){
		  	lStarttime=lStarttime+24*3600;
		  	strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		  }
		  strcpy(caShour,"00");
	}
	else if(atol(caAv_un)==3){ 
		if(strlen(caSdate)==0){
		    lTime=time(0);
		 }  
		 else{
		 	lTime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
		 }                           //��ͳ��{
		ncUtlMonthDate(lTime,caSdate,caEdate);
 		lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
		lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate);
		
	
			if(atol(caPre_flag)==1){
		  	lStarttime=lStarttime-24*3600*15;
		  	ncUtlMonthDate(lStarttime,caSdate,caEdate);
		  }
		  else if(atol(caPre_flag)==2){
		  	lStarttime=lEndtime+24*3600*15;
		  	ncUtlMonthDate(lStarttime,caSdate,caEdate);
		  }
		  strcpy(caShour,"00");
		lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
		lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate);
		
	}
	else{
			if(strlen(caSdate)==0){
		    lTime=time(0);
		    
		     strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));
		     strcpy(caShour,utTimFormat("%H",lTime));
		     strcpy(caSmin,"00");


		  }
		   	 sprintf(caSdate_temp2,"%s %s:%s",caSdate,caShour,caSmin);
		     lStarttime=utTimStrToLong("%Y/%m/%d %H:%M:00",caSdate_temp2);
		     lEndtime=lStarttime+1800;
		  
		  if(atol(caPre_flag)==1){
		  	lStarttime=lStarttime-1800;
		  	lEndtime=lEndtime-1800;
		  	
		  	 strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		     strcpy(caShour,utTimFormat("%H",lStarttime));
		     strcpy(caSmin,utTimFormat("%M",lStarttime));
		  	
		  }
		  else if(atol(caPre_flag)==2){
		  	lStarttime=lStarttime+1800;
		  	lEndtime=lEndtime+1800;
		  	
		  	 strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		     strcpy(caShour,utTimFormat("%H",lStarttime));
		     strcpy(caSmin,utTimFormat("%M",lStarttime));
		  }
		  
		  	
		}
    utPltPutVarF(psDbHead,"stime1","%d",lStarttime); 
    utPltPutVar(psDbHead,"sdate",caSdate);
  	utPltPutVar(psDbHead,"shour",caShour);
  	utPltPutVar(psDbHead,"smin",caSmin);
  	utPltPutVar(psDbHead,"gitem",caGitem);
  	utPltPutVar(psDbHead,"avun",caAv_un);
  	
 //��ȡͳ��ʱ�����������������񣬽���Щ������ͼ�����������ķ���鵽���������
  	if(atol(caGitem)==101){                         //������ϸ  	
  		    utPltPutVar(psDbHead,"title",ncLang("1815ϸ�ַ�����������ͼ"));
  	      utPltPutVar(psDbHead,"title_1",ncLang("1817ϸ�ַ�����������ͼ"));
  			
		   if(atol(caAv_un)==2){
		     sprintf(caTemp,"select sum(ubytes+dbytes) aa,service,name from ncipservice ,ncipindex  where sdate='%s' and service=sid  group by service order by aa desc limit 0,10",caSdate);
		    }
		    else if(atol(caAv_un)==3){
		    
		    	sprintf(caTemp,"select sum(ubytes+dbytes) aa,service,name from ncipservice,ncipindex where sdate>='%s' and sdate<='%s' and service=sid group by service order by aa desc limit 0,10",caSdate,caEdate);
		
		    }
		    else{ 
		    	sprintf(caTemp,"select sum(ubytes+dbytes) aa,service,name from nciplog,ncipindex where vtime>=%d and vtime<=%d and service=sid group by service order by aa desc limit 0,10",lStarttime,lEndtime);                                                                         //����ͳ�ư�СʱNCIPLOG��¼
		    }
    }
    else if(atol(caGitem)==100){                     //�������
    	
    	  	utPltPutVar(psDbHead,"title",ncLang("1814������������ֲ�ͼ"));
  	      utPltPutVar(psDbHead,"title_1",ncLang("1816������������ֲ���"));
    		if(atol(caAv_un)==2){
		     sprintf(caTemp,"select sum(ubytes+dbytes) aa,c.groupid,c.name from ncipservice a,ncipindex b,ncservicecgroup c where sdate='%s' and a.service=b.sid and b.groupid=c.groupid group by b.groupid order by aa desc limit 0,10",caSdate);
		    }
		    else if(atol(caAv_un)==3){		    
		 		     sprintf(caTemp,"select sum(ubytes+dbytes) aa,c.groupid,c.name from ncipservice a,ncipindex b,ncservicecgroup c where sdate='%s' and a.service=b.sid and b.groupid=c.groupid group by b.groupid order by aa desc limit 0,10",caSdate,caEdate);
		
		    }
		    else{
		    	sprintf(caTemp,"select sum(ubytes+dbytes) aa,c.groupid,c.name from nciplog a,ncipindex b,ncservicecgroup c where vtime>=%d and vtime<=%d and a.service=b.sid and b.groupid=c.groupid group by b.groupid order by aa desc limit 0,10",lStarttime,lEndtime); 
		    }
    }
    else{                                       //ͳ��ĳ�������
    	  sprintf(caTemp,"select name from ncservicecgroup where groupid=%d ",atol(caGitem));
  	 	 	memset(caName,0,sizeof(caName));
  	 	 	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caName);
    		sprintf(caTemp,ncLang("1818%s�����ֲ�����ͼ"),caName);
  	 	 	utPltPutVar(psDbHead,"title",caTemp);
  	 	 	
  	 	 	sprintf(caTemp,ncLang("1819%s�����ֲ����Ʊ�"),caName);
  	 	 	utPltPutVar(psDbHead,"title_1",caTemp);
  			
		   if(atol(caAv_un)==2){
		     sprintf(caTemp,"select sum(ubytes+dbytes) aa,service,name from ncipservice ,ncipindex  where sdate='%s' and service=sid and groupid=%d group by service order by aa desc limit 0,10",caSdate,atol(caGitem));
		    }
		    else if(atol(caAv_un)==3){
		    
		    	sprintf(caTemp,"select sum(ubytes+dbytes) aa,service,name from ncipservice,ncipindex where sdate>='%s' and sdate<='%s' and service=sid and groupid=%d group by service order by aa desc limit 0,10",caSdate,caEdate,atol(caGitem));
		
		    }
		    else{
		      sprintf(caTemp,"select sum(ubytes+dbytes) aa,service,name from nciplog a,ncipindex where vtime>=%d and vtime<=%d and service=sid and a.groupid=%d group by service order by aa desc limit 0,10",lStarttime,lEndtime,atol(caGitem)); 
		    }
    }
     printf("caTemp=%s\n",caTemp);
     psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����2"));
        return 0;
     }
     lBytes=0;
     lService=0;
     memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                   UT_TYPE_LONG,4,&lService,
                                   UT_TYPE_STRING,30,caName);
                                  
    lSumindex=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	
    		sGservice[lSumindex].lService=lService;
    		sGservice[lSumindex].lGroupid=0;
    		sGservice[lSumindex].lBytes=lBytes;
    		strcpy(sGservice[lSumindex].caGname,caName);
    	for(i=0;i<32;i++){
    		sGservice[lSumindex].lBytesqs[i]=0;
    	}
    	
    	
    lSumindex++;
    lBytes=0;
    lService=0;
     memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                   UT_TYPE_LONG,4,&lService,
                                   UT_TYPE_STRING,30,caName);
    }
    
    pasDbCloseCursor(psCur);
   
    
    //���һ������������������
  
    	  sGservice[lSumindex].lService=9999;
    		sGservice[lSumindex].lGroupid=0;
    		sGservice[lSumindex].lBytes=0;
    		strcpy(sGservice[lSumindex].caGname,ncLang("0431����"));
        for(i=0;i<32;i++){
    	    sGservice[lSumindex].lBytesqs[i]=0;
    	    
        }
        
//        printf("����ǰ\n");
//        for(i=0;i<lSumindex;i++){
//        	printf("service=%d\n",sGservice[i].lService);
//        }
  //��serviceID����  Ϊ������ַ�������׼��		
    		qsort(sGservice,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId2); 
    	
//           printf("�����\n");
//        for(i=0;i<lSumindex;i++){
//        	printf("service=%d\n",sGservice[i].lService);
 //       }
 
 if(atol(caGitem)==101){                         //������ϸ  	
  
	   if(atol(caAv_un)==3){
	        sprintf(caTemp,"select sum(ubytes+dbytes),substring(sdate,9,2) bb,service from ncipservice,ncipindex  where sdate>='%s' and sdate<='%s' and service=sid group by bb,service ",caSdate,caEdate);
	    }
	    else if(atol(caAv_un)==2){
	    	  sprintf(caTemp,"select sum(ubytes+dbytes),stime,service from ncipservice,ncipindex  where sdate='%s' and service=sid group by stime,service ",caSdate);
	    }
	    else{
	    		sprintf(caTemp,"select sum(ubytes+dbytes),from_unixtime(vtime,'%ci') bb,service from nciplog,ncipindex where vtime>=%d and vtime<=%d and service=sid  group by bb,service ",'%',lStarttime,lEndtime);
	    }
	  }
	  else if(atol(caGitem)==100){                   //���������ͳ��	  	
	  	if(atol(caAv_un)==3){
	        sprintf(caTemp,"select sum(ubytes+dbytes),substring(a.sdate,9,2) bb,b.groupid from ncipservice a,ncipindex b,ncservicecgroup c  where a.sdate>='%s' and a.sdate<='%s' and a.service=b.sid and b.groupid=c.groupid group by bb,b.groupid ",caSdate,caEdate);
	    }
	    else if(atol(caAv_un)==2){
	    	  sprintf(caTemp,"select sum(ubytes+dbytes),stime,b.groupid from ncipservice a,ncipindex b,ncservicecgroup c  where a.sdate='%s' and a.service=b.sid and b.groupid=c.groupid group by stime,b.groupid ",caSdate);
	    }
	  	else{
	  		 sprintf(caTemp,"select sum(ubytes+dbytes),from_unixtime(vtime,'%ci') bb,b.groupid from nciplog a,ncipindex b,ncservicecgroup c where vtime>=%d and vtime<=%d and a.service=b.sid and b.groupid=c.groupid  group by bb,b.groupid ",'%',lStarttime,lEndtime);
	  	}
	  }
    else{        //��ĳһ��������µ�ϸ�ַ���ͳ��
    		 if(atol(caAv_un)==3){
	        sprintf(caTemp,"select sum(ubytes+dbytes),substring(a.sdate,9,2) bb,a.service from ncipservice a,ncipindex b  where a.sdate>='%s' and a.sdate<='%s' and a.service=b.sid  and b.groupid=%d group by bb,a.service ",caSdate,caEdate,atol(caGitem));
	    }
	    else if(atol(caAv_un)==2){
	    	  sprintf(caTemp,"select sum(ubytes+dbytes),stime,a.service from ncipservice a,ncipindex b  where a.sdate='%s' and a.service=b.sid  and b.groupid=%d group by stime,a.service ",caSdate,atol(caGitem));
	    }
	    else{
	    	 sprintf(caTemp,"select sum(ubytes+dbytes),from_unixtime(vtime,'%ci') bb,a.service from nciplog a,ncipindex b where vtime>=%d and vtime<=%d and a.service=b.sid and b.groupid=%d  group by bb,a.service ",'%',lStarttime,lEndtime,atol(caGitem));
	    }
    }
   printf("caTemp=%s\n",caTemp);
   psCur = pasDbOpenSql(caTemp,0);
   if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����1"));
        return 0;
    }
    lBytes=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
                                  
    iNum=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	
   	 if(lBytes>=lMaxbytes){ 
   	 lMaxbytes=lBytes;
   	      
   	    }
  
   	 
    	findex=atol(caTime);
    	if(findex>=30){                           //������ ʱ��ֵ�������±����30
    		findex=findex-30;
    	}
    	  index=ncUtlGetServicenumById(&sGservice,lSumindex,lService);
//    	  printf("index=%d\n",index);
//    	  printf("lService=%d\n",lService);
	   	  if(index!=-1){	
	          sGservice[index].lBytesqs[findex]=sGservice[index].lBytesqs[findex]+lBytes;         
	      }
    	  else{
    	  	sGservice[lSumindex].lBytesqs[findex]=sGservice[lSumindex].lBytesqs[findex]+lBytes; 
    	  }
       	
  
    lBytes=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
    }
    
    pasDbCloseCursor(psCur);
   
  //���������� 
//    		qsort(sGservice,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId);
   
  //  �������ֵ����ʹ��������λ Bytes,KB,MB,GB
     lRat=lMaxbytes/10240;
 // printf("lMaxbytes=%llu\n",lRat);
			if(lRat<=1){
					unit=1;
					strcpy(caUnit,"Bytes");
					
			 }
				else if(lRat>1&&lRat<=1024){
					unit=1024;
					strcpy(caUnit,"KB");
				}
				else if(lRat>1024&&lRat<=1024*1024){
					unit=1024*1024;
					strcpy(caUnit,"MB");
				}
				else if(lRat>1024*1024){
					unit=1024*1024*1024;
					strcpy(caUnit,"GB");
				}    
				else{
					unit=1;
					strcpy(caUnit,"Bytes");
				}

 //   printf("unit=%d\n",unit);
    lMaxrate=3000;
  
    utPltPutVarF(psDbHead,"maxrate","%d",lMaxrate);


//���X��̶�

	  for(mm=0;mm<x_sum;mm++){
	  		utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
	  		if(atol(caAv_un)==2){
	  			sprintf(caTemp,"%02d:00",mm);
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
	  		}
	  		else if(atol(caAv_un)==3){
	  			
	  			sprintf(caTemp,"%s/%02d",utTimFormat("%m",lStarttime),mm+1);
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
	  		}
	  		else{
	  			sprintf(caTemp,"%s:%02d",utTimFormat("%H",lStarttime),mm+atol(caSmin));
	 
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
	  		}
	  		
	  		//��������ͳ������
	  		for(i=0;i<=lSumindex;i++){
	  			if(strcmp(caUnit,"Bytes")!=0){
	  				if(atol(caAv_un)==3)                                  //���ڰ���ͳ�ƣ������1��ʼ�������0��ʼ
	  			   	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.2f",(float)sGservice[i].lBytesqs[mm+1]/unit,caTemp);
	  			 	else  
	  			  	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.2f",(float)sGservice[i].lBytesqs[mm]/unit,caTemp);   
	  		   }
	  		   else{
	  		   	if(atol(caAv_un)==3)                                  //���ڰ���ͳ�ƣ������1��ʼ�������0��ʼ
	  			   	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm+1]/unit,caTemp);
	  			 	else  
	  			  	utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm]/unit,caTemp);  
	  		  }
	  	 }
	  	
	  }
	   
 //�������ͼ������ 
    for(i=0;i<=lSumindex;i++){
    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);
//    	printf("gservice=%d\n",sGservice[i].lService);
    	utPltPutLoopVar(psDbHead,"sevname",i+1,sGservice[i].caGname);
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",sGservice[i].lService);
 
    	for(mm=0;mm<x_sum;mm++){
//    		printf("i=%d;mm=%d;service=%d;bytes=%lld\n",i,mm,sGservice[i][mm].lService);
    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);
    		if(atol(caAv_un)==3)                             //���ڰ���ͳ�������1��ʼ�������0��ʼ
    		  utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%0.0f",(float)sGservice[i].lBytesqs[mm+1]/unit);
    		else
    		  utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%0.0f",(float)sGservice[i].lBytesqs[mm]/unit);
    	}
    }
  utPltPutVarF(psDbHead,"sum","%d",lSumindex+1);
  utPltPutVarF(psDbHead,"sum_d","%d",x_sum);
   utPltPutVar(psDbHead,"unit",caUnit);
   
   
   
   
   
    if(strlen(caPdf)>0){
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
   		  if(atol(caAv_un)==2){
	  			sprintf(caTemp,"%02d:00",i);	  		  
	  		}
	  		else if(atol(caAv_un)==3){	  			
	  			sprintf(caTemp,"%s/%02d",utTimFormat("%m",lStarttime),i+1);	  		 
	  		}
	  		else{
	  			sprintf(caTemp,"%s:%02d",utTimFormat("%H",lStarttime),i+atol(caSmin));	  		 
	  		}  	
   	   pChart[i].name=strdup(caTemp);
   	  
   	  pChart[i].dataset=malloc((lSumindex+2)*sizeof(float)) ;
   	  for(mm=0;mm<lSumindex+1;mm++){
   	  	if(atol(caAv_un)==3)                             //���ڰ���ͳ�������1��ʼ�������0��ʼ
   	  	  pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i+1]/unit;
   	  	else
   	  	 pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i]/unit;
   	  
   	  }
   	} 
   	
     
   
     
     
      rowrow=x_sum;
      column=lSumindex+1;
   
    strcpy(caTitle,ncLang("1455������������ֲ�����ͼ"));
     
    sprintf(filename,"%s.PDF",caTitle); 

    sprintf(caStatdate,ncLang("1962ͳ��ʱ��:%s %s:%s"),caSdate,caShour,caSmin);

/*
   for(i=0;i<rowrow;i++){
   	for(m=0;m<lSumindex+1;m++)
   	  printf("name=%s,i=%d,m=%d,dataset=%f\n",pChart[i].name,i,m,pChart[i].dataset[m]);
   	 
   }
   for(i=0;i<lSumindex+1;i++){
   	  printf("i=%d,desc=%s\n",i,desc[i]);

   }
printf("caTitle=%s,filename=%s,rowrow=%d,column=%d,caUnit=%s\n",caTitle,filename,rowrow,column,caUnit);
*/
sprintf(caTemp,ncLang("1942��λ(%s)"),caUnit);
   ncWebShowServiceBytime_PDF(iFd,pChart,desc,rowrow,column,caTitle,caTemp,caStatdate,filename);


//printf("end\n"); 
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
   
   
   
   
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_mainfirst_servicebytime.htm");
    
    return 0;
}



/* ��ʾĳ��ҳ��������û�������ϸ*/
int ncWebServiceFirstDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    
    struct ncSflow_s sService[2000];
    struct ncSflow_s  sGservice[2000];
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
    char caFlag[10],caTopnum[10],caStime[12],caEtime[12],caService[12];
    struct ncSflow_s *psService;
    ncUserCont *psUser;
    long long lTsum;
    char caPdf[32];
    long lRowrow;
    struct piechart_datasetf *pChart;           //����PDF�ĵ����
//   char *desc[]={"�û���","����","����","�ٷֱ�"};
    int rowrow;                     
    int column; 
    char **pTemp;
    char caTitle[128];
    char filename[128];
    char caStatdate[128];
    int m;
    column=4;
    long lDate;
    
   lTime=time(0);
   lSum=0;
   lRecenttime=0;
    char **desc;
    char caWorksql[1024],caWorksql2[1024];
    char caWorkflag[12];
    lSumindex=0;
      utMsgGetSomeNVar(psMsgHead,14,"gitem",UT_TYPE_STRING,12,caGitem,
                                 "sdate",UT_TYPE_STRING,12,caSdate,
                                 "shour",UT_TYPE_STRING,2,caShour,
                                 "smin", UT_TYPE_STRING,2,caSmin,
                                 "edate",UT_TYPE_STRING,12,caEdate,
                                 "ehour",UT_TYPE_STRING,2,caEhour,
                                 "emin", UT_TYPE_STRING,2,caEmin,
                                 "flag", UT_TYPE_STRING,2,caFlag,
                                 "topnum",UT_TYPE_STRING,8,caTopnum,
                                 "stime",UT_TYPE_STRING,10,caStime,
                                 "etime",UT_TYPE_STRING,10,caEtime,
                                 "service",UT_TYPE_STRING,10,caService,
                                 "pdf",UT_TYPE_STRING,8,caPdf,
                                 "workflag",UT_TYPE_STRING,8,caWorkflag);
   
 // printf("caStime=%s,caEtime=%s,caSdate=%s,csShour=%s,caSmin=%s,caEhour=%s,caEmin=%s\n",caStime,caEtime,caSdate,caShour,caSmin,caEhour,caEmin);
 
 //printf("caPdf=%s\n",caPdf);
 
  psDbHead = utPltInitDb(); 
  if(strlen(caTopnum)==0)
       strcpy(caTopnum,"10");
       
  if(strlen(caFlag)==0) strcpy(caFlag,"1"); 
  
  if(atol(caFlag)==1){
  	sprintf(caTitle,ncLang("1828%s����ʹ�����"),ncUtlGetServiceName(psShmHead,atol(caService),ncLang("0431����")));
  	if(atol(caService)>0){
  	utPltPutVar(psDbHead,"servname",ncUtlGetServiceName(psShmHead,atol(caService),ncLang("0431����")));
    }
  }
	else if(atol(caFlag)==3){
		utPltPutVar(psDbHead,"servname","");                                  //ȫ����������
	}
  else {
  	sprintf(caTemp,"select name from ncservicecgroup where groupid=%d ",atol(caService));
  	memset(caName,0,sizeof(caName));
  	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caName);
  	 sprintf(caTitle,ncLang("1828%s����ʹ�����"),caName);
  	utPltPutVar(psDbHead,"servname",caName);
  }
  psService=(struct ncSflow_s *)malloc(sizeof(struct ncSflow_s)*(4*atol(caTopnum)+1));
  if(psService==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("1863�����ڴ����"));
      return 0;
  }

   
  if(strlen(caEtime)==0){
  	sprintf(caEtime,"%d",time(0));
  	if(strlen(caSdate)==0){
  	strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
  	sprintf(caStime,"%d",utTimStrToLong("%Y/%m/%d 00:00:00",caSdate));
    }
  }
   

   
   if(strlen(caStime)>0){
   	strcpy(caSdate,utTimFormat("%Y/%m/%d",atol(caStime))); 
   	strcpy(caShour,utTimFormat("%H",atol(caStime))); 
   	strcpy(caSmin,utTimFormat("%M",atol(caStime)));
   	lStime=atol(caStime);
    }
    if(strlen(caEtime)>0){
    	if(strlen(caEdate)==0){
    	strcpy(caEdate,utTimFormat("%Y/%m/%d",atol(caEtime)));
    	strcpy(caEhour,utTimFormat("%H",atol(caEtime)));
    	strcpy(caEmin,utTimFormat("%M",atol(caEtime)));
    }
    	lEtime=atol(caEtime);
    }               
  
  if(strlen(caSdate)>0){
  	
  	  sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);	  
  	  lStime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
  	  
  }
  if(strlen(caEdate)>0){
  	 sprintf(caEdate_all,"%s %s:%s",caEdate,caEhour,caEmin);
  	 lEtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all);
  }
  
    if(strlen(caGitem)==0) strcpy(caGitem,"1");
 
 
  
 // 	   printf("fffcaSdate=%s,csShour=%s,caSmin=%s,caEhour=%s,caEmin=%s\n",caSdate,caShour,caSmin,caEhour,caEmin);   
  	   
  	   
  	 utPltPutVar(psDbHead,"shour",caShour);
  	 utPltPutVar(psDbHead,"smin",caSmin);
  	 utPltPutVar(psDbHead,"ehour",caEhour);
  	 utPltPutVar(psDbHead,"emin",caEmin);
  	 utPltPutVar(psDbHead,"sdate",caSdate);
  	 utPltPutVar(psDbHead,"edate",caEdate);
  	 utPltPutVar(psDbHead,"gitem",caGitem);
  	 utPltPutVar(psDbHead,"service",caService);
  	 utPltPutVar(psDbHead,"flag",caFlag);
  	 utPltPutVar(psDbHead,"topnum",caTopnum);
 
     utPltPutVar(psDbHead,"workflag",caWorkflag);
 
 //��ȡncipservice�����������ֵ 	���ֵ�����ж϶�ʱͳ�Ƴ���ǰ�Ѿ�����ķ������ݣ��������ʱ���û��������Ҫ��IPLOG�л�ȡ 
  	 memset(caMaxdate,0,sizeof(caMaxdate));
  	 pasDbOneRecord("select max(sdate) from ncipservice ",0,UT_TYPE_STRING,10,caMaxdate);
  	 sprintf(caTemp,"select max(stime) from ncipservice where sdate='%s' ",caMaxdate);
  	 memset(caMaxtime,0,sizeof(caMaxtime));
  	 pasDbOneRecord(caTemp,0,UT_TYPE_STRING,2,caMaxtime);
  	 
  	 
  	 if(strlen(caMaxdate)>0&&(strlen(caMaxtime))){
  	 	 sprintf(caRectime,"%s %s",caMaxdate,caMaxtime);
  	 	 lRecenttime=utTimStrToLong("%Y/%m/%d %H",caRectime);
  	 }
  lSum=0;	 
  	 if(lEtime>lRecenttime){
 //ͳ��IPlog�еķ���
      if(lStime>lRecenttime){
      	lStime1=lStime;
      }
      else{
      	lStime1=lRecenttime;
      }
      
      //ͳ�����ַ����������
      lTsum=0;
      if(atol(caFlag)==1){
      	if(atol(caService)>0){
          sprintf(caTemp,"select sum(ubytes+dbytes) from nciplog where vtime>%d and vtime<=%d and service=%d ",lStime1,lEtime,atol(caService));
        }
        else{
        	sprintf(caTemp,"select sum(ubytes+dbytes) from nciplog where vtime>%d and vtime<=%d ",lStime1,lEtime);
        }
      }
      else if(atol(caFlag)==3){
      	  sprintf(caTemp,"select sum(ubytes+dbytes) from nciplog where vtime>%d and vtime<=%d  ",lStime1,lEtime);
      }
      else{
      	sprintf(caTemp,"select sum(ubytes+dbytes) from nciplog a,ncipindex b where vtime>%d and vtime<=%d and a.service=b.sid and b.groupid=%d ",lStime1,lEtime,atol(caService));
      }
      
      //�Ƿ���ʱ��
		if(atol(caWorkflag)==1){
			 strcpy(caWorksql,ncUtlCvtWorkTime2Sql("vtime"));
			 if(strlen(caWorksql)>0){
			 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql);
		   }  
		  }	  
      
      
      pasDbOneRecord(caTemp,0,UT_TYPE_LONG8,8,&lTsum);

      lSum=lSum+lTsum;
       printf("caTemp=%s\n",caTemp);
 //     printf("lSum=%llu\n",lSum);     
      if(atol(caFlag)==1){                                                 //ͳ�Ƶ�������
      	if(atol(caGitem)==1){                                              //���û�ͳ��
      	if(atol(caService)>0){
      		sprintf(caTemp,"select sum(ubytes+dbytes) bb,userid from nciplog where vtime>%d and vtime<=%d and service=%d ",lStime1,lEtime,atol(caService));
      	}
      	else{
      		sprintf(caTemp,"select sum(ubytes+dbytes) bb,userid from nciplog where vtime>%d and vtime<=%d  ",lStime1,lEtime);
      	}
      		if(atol(caWorkflag)==1){
	//		 strcpy(caWorksql,ncUtlCvtWorkTime2Sql("vtime"));
				 if(strlen(caWorksql)>0){
				 	  sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql);
			    }  
      	}
      		
      		sprintf(caTemp+strlen(caTemp)," group by userid order by bb desc limit 0,%d ",2*atol(caTopnum));
      	}
      	else{                                                              //������ͳ��
      		   if(atol(caService)>0){
      					sprintf(caTemp,"select sum(ubytes+dbytes) bb,b.groupid from nciplog a,ncuser b where vtime>%d and vtime<=%d and a.userid=b.userid and service=%d ",lStime1,lEtime,atol(caService));
      				}
      				else{
      					sprintf(caTemp,"select sum(ubytes+dbytes) bb,b.groupid from nciplog a,ncuser b where vtime>%d and vtime<=%d and a.userid=b.userid  ",lStime1,lEtime);
      				}
      		if(atol(caWorkflag)==1){
	//		 strcpy(caWorksql,ncUtlCvtWorkTime2Sql("vtime"));
				  if(strlen(caWorksql)>0){
				 	   sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql);
			     }  
      	 }
      		
      		sprintf(caTemp+strlen(caTemp)," group by groupid order by bb desc limit 0,%d ",2*atol(caTopnum));
      	}
      }
      else if(atol(caFlag)==3){                                             //ͳ��ȫ������
      	  if(atol(caGitem)==1){                                              //���û�ͳ��
      		sprintf(caTemp,"select sum(ubytes+dbytes) bb,userid from nciplog where vtime>%d and vtime<=%d  ",lStime1,lEtime);
      		if(atol(caWorkflag)==1){
	//		 strcpy(caWorksql,ncUtlCvtWorkTime2Sql("vtime"));
				  if(strlen(caWorksql)>0){
				 	   sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql);
			     }  
      	  }
      		
      		sprintf(caTemp+strlen(caTemp)," group by userid order by bb desc limit 0,%d ",2*atol(caTopnum));
      	}
      	else{                                                              //������ͳ��
      		sprintf(caTemp,"select sum(ubytes+dbytes) bb,b.groupid from nciplog a,ncuser b where vtime>%d and vtime<=%d and a.userid=b.userid ",lStime1,lEtime);
      		if(atol(caWorkflag)==1){
	//		 strcpy(caWorksql,ncUtlCvtWorkTime2Sql("vtime"));
				  if(strlen(caWorksql)>0){
				 	   sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql);
			     }  
      	  }
      		
      		 sprintf(caTemp+strlen(caTemp)," group by groupid order by bb desc limit 0,%d ",2*atol(caTopnum));
      	}
      }
      else {
      	 if(atol(caGitem)==1){                               //ͳ�Ʒ������
      	  sprintf(caTemp,"select sum(ubytes+dbytes) bb,userid from nciplog a,ncipindex b where  vtime>%d and vtime<=%d  and a.service=b.sid and b.groupid=%d  ", lStime1,lEtime,atol(caService));
      	  if(atol(caWorkflag)==1){
	//		 strcpy(caWorksql,ncUtlCvtWorkTime2Sql("vtime"));
				  if(strlen(caWorksql)>0){
				 	   sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql);
			     }  
      	  }
      	  
      	  sprintf(caTemp+strlen(caTemp)," group by userid order by bb desc limit 0,%d ",2*atol(caTopnum));     
      	}
      	else{
      		sprintf(caTemp,"select sum(ubytes+dbytes) bb,b.groupid from nciplog a,ncuser b,ncipindex c where vtime>%d and vtime<=%d and a.userid=b.userid and a.service=c.sid and c.groupid=%d ",lStime1,lEtime,atol(caService));
      		if(atol(caWorkflag)==1){
	//		 strcpy(caWorksql,ncUtlCvtWorkTime2Sql("vtime"));
				  if(strlen(caWorksql)>0){
				 	   sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql);
			     }  
      	  }
      		
      		
      		sprintf(caTemp+strlen(caTemp)," group by b.groupid order by bb desc limit 0,%d ",lStime1,lEtime,atol(caService),2*atol(caTopnum));
      	}                                                      //ͳ�Ʒ������
      }
      printf("[pid=%d]caTemp=%s\n",getpid(),caTemp);
      psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
        return 0;
       }
       lBytes=0;
       lService=0;
       lSumindex=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                     UT_TYPE_LONG,4,&lService);
       while((iReturn==0)||(iReturn==1405))
       {
       	psService[lSumindex].lService=lService;
       	psService[lSumindex].lBytes=lBytes;
       	lSumindex++;
       
      	
	       lBytes=0;
	       lService=0;
	       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                     UT_TYPE_LONG,4,&lService);
       }                               
      pasDbCloseCursor(psCur);                           
 
  	 }
 
   	  		  //�Ƿ���ʱ��
					if(atol(caWorkflag)==1){
						 strcpy(caWorksql2,ncUtlCvtWorkTime2Sql_s("sdate","stime"));
						}
 
  	 
  	//��USERID��������Ϊ���ַ���ѯ��׼�� 
  	if(lSumindex>0)
  	   qsort(psService,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId2); 
  	 
  	 
  	  //ͳ��ncIPSERVICE�е�����
  	  if(lStime<lRecenttime){           //ͳ�ƿ�ʼʱ����ncipservice �е����ʱ��ǰ������
  	  	//ͳ��IPSERVICE�е����з����������
  	  	if(atol(caFlag)==1){
  	  	 if(strcmp(caSdate,caEdate)!=0){
            
  	  			sprintf(caTemp,"select sum(ubytes+dbytes) from ncipservice where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s'))   ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	  		}
  	  	 else{
  	  			sprintf(caTemp,"select sum(ubytes+dbytes) from ncipservice where sdate='%s' and stime>='%s' and stime<='%s' ",caSdate,caShour,caEhour);
  	  		 }
  	  		 
  	  		  if(atol(caService)>0){
  	  		  	sprintf(caTemp+strlen(caTemp)," and service=%d ",atol(caService));
  	  		  }
  	  		}
  	  	else if(atol(caFlag)==3){
  	  		
  	  	if(strcmp(caSdate,caEdate)!=0)
  	  			sprintf(caTemp,"select sum(ubytes+dbytes) from ncipservice where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s'))   ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	  	 else
  	  			sprintf(caTemp,"select sum(ubytes+dbytes) from ncipservice where sdate='%s' and stime>='%s' and stime<='%s'   ",caSdate,caShour,caEhour);
  	  			
  	  	}
  	  	else{
  	  		 if(strcmp(caSdate,caEdate)!=0)
  	  				sprintf(caTemp,"select sum(ubytes+dbytes) from ncipservice,ncipindex where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s')) and service=sid and groupid=%d  ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour,atol(caService));
  	  	 	 else
  	  			  sprintf(caTemp,"select sum(ubytes+dbytes) from ncipservice,ncipindex where sdate='%s' and stime>='%s' and stime<='%s' and service=sid and groupid=%d  ",caSdate,caShour,caEhour,atol(caService));
  	  		
  	  		}
  	  		
  	  		  	  		//�Ƿ���ʱ��
					if(atol(caWorkflag)==1){
						 if(strlen(caWorksql2)>0){
						 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
					   } 	
  	  		}	
  	  		
  	  		
  	  		
  	  	    pasDbOneRecord(caTemp,0,UT_TYPE_LONG8,8,&lTsum);
           lSum=lSum+lTsum;
 printf("[pid=%d] caTemp=%s\n",getpid(),caTemp); 	  	
  	  	
  	  	if(atol(caFlag)==1){
  	  		if(atol(caGitem)==1){
  	  				if(strcmp(caSdate,caEdate)!=0){
  	  				  	sprintf(caTemp,"select sum(ubytes+dbytes) bb,userid from ncipservice where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s'))  ",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	  				  //�Ƿ���ʱ��
  	  				  if(atol(caService)>0){
  	  				  	sprintf(caTemp+strlen(caTemp)," and service=%d ",atol(caService));
  	  				  }
								if(atol(caWorkflag)==1){
									 if(strlen(caWorksql2)>0){
									 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
								   } 	
			  	  		}
  	  				  	  				  	
  	  				  	 sprintf(caTemp+strlen(caTemp)," group by userid order by bb desc limit 0,%d ",2*atol(caTopnum));
  	  				  }
  	  				 else{
  	  				   sprintf(caTemp,"select sum(ubytes+dbytes) bb,userid from ncipservice where sdate='%s' and stime>='%s' and stime<='%s' ",caSdate,caShour,caEhour);
  	  				   if(atol(caService)>0){
  	  				  	 sprintf(caTemp+strlen(caTemp)," and service=%d ",atol(caService));
  	  				   }
  	  				  //�Ƿ���ʱ��
								if(atol(caWorkflag)==1){
									 if(strlen(caWorksql2)>0){
									 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
								   } 	
			  	  		}
  	  				  	  				  	
  	  				  	 sprintf(caTemp+strlen(caTemp)," group by userid order by bb desc limit 0,%d ",2*atol(caTopnum));
  	  				  }
  	  		}
  	  		else{
  	  			  if(strcmp(caSdate,caEdate)!=0){
  	  				  	sprintf(caTemp,"select sum(ubytes+dbytes) bb,groupid from ncipservice a,ncuser b where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s')) and a.userid=b.userid  ",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	  				  if(atol(caService)>0){
  	  				  	sprintf(caTemp+strlen(caTemp)," and service=%d ",atol(caService));
  	  				  }
  	  				  	
  	  				  	//�Ƿ���ʱ��
									if(atol(caWorkflag)==1){
										 if(strlen(caWorksql2)>0){
										 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
									   } 	
				  	  		}
  	  				  	
  	  				  	sprintf(caTemp+strlen(caTemp)," group by groupid order by bb desc limit 0,%d ",2*atol(caTopnum));
  	  				  }
  	  				 else{
  	  				   sprintf(caTemp,"select sum(ubytes+dbytes) bb,groupid from ncipservice a,ncuser b where sdate='%s' and stime>='%s' and stime<='%s' and a.userid=b.userid  ",caSdate,caShour,caEhour);
  	  				   if(atol(caService)>0){
  	  				  	 sprintf(caTemp+strlen(caTemp)," and service=%d ",atol(caService));
  	  				   }
  	  				  	//�Ƿ���ʱ��
									if(atol(caWorkflag)==1){
										 if(strlen(caWorksql2)>0){
										 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
									   } 	
				  	  		}
  	  				  	
  	  				  	sprintf(caTemp+strlen(caTemp)," group by groupid order by bb desc limit 0,%d ",2*atol(caTopnum));
  	  				  }
  	  		}
  	  	}
  	  	else if(atol(caFlag)==3){
  	  		
  	  		if(atol(caGitem)==1){
  	  				if(strcmp(caSdate,caEdate)!=0){
  	  				  	sprintf(caTemp,"select sum(ubytes+dbytes) bb,userid from ncipservice where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s')) ",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	  				  	 //�Ƿ���ʱ��
									if(atol(caWorkflag)==1){
										 if(strlen(caWorksql2)>0){
										 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
									   } 	
				  	  		}
  	  				  	
  	  				  	 sprintf(caTemp+strlen(caTemp)," group by userid order by bb desc limit 0,%d ", 2*atol(caTopnum));
  	  				  }
  	  				 else{
  	  				   sprintf(caTemp,"select sum(ubytes+dbytes) bb,userid from ncipservice where sdate='%s' and stime>='%s' and stime<='%s' ",caSdate,caShour,caEhour);
  	  				    //�Ƿ���ʱ��
									if(atol(caWorkflag)==1){
										 if(strlen(caWorksql2)>0){
										 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
									   } 	
				  	  		}
  	  				  	
  	  				  	 sprintf(caTemp+strlen(caTemp)," group by userid order by bb desc limit 0,%d ", 2*atol(caTopnum));
  	  				   
  	  				     
  	  				  }
  	  		}
  	  		else{
  	  			  if(strcmp(caSdate,caEdate)!=0){
  	  				  	sprintf(caTemp,"select sum(ubytes+dbytes) bb,groupid from ncipservice a,ncuser b where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s')) and a.userid=b.userid ",caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	  				  	//�Ƿ���ʱ��
									if(atol(caWorkflag)==1){
										 if(strlen(caWorksql2)>0){
										 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
									   } 	
				  	  		}
  	  				  	 sprintf(caTemp+strlen(caTemp)," group by groupid order by bb desc limit 0,%d ", 2*atol(caTopnum));
  	  				  }
  	  				 else{
  	  				   sprintf(caTemp,"select sum(ubytes+dbytes) bb,groupid from ncipservice a,ncuser b where sdate='%s' and stime>='%s' and stime<='%s' and a.userid=b.userid ",caSdate,caShour,caEhour);
  	  				    //�Ƿ���ʱ��
									if(atol(caWorkflag)==1){
										 if(strlen(caWorksql2)>0){
										 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
									   } 	
				  	  		}
  	  				  	 sprintf(caTemp+strlen(caTemp)," group by groupid order by bb desc limit 0,%d ", 2*atol(caTopnum));
  	  				   
  	  				  }
  	  		}
  	  		
  	  	}
  	  	else{
  	  		
  	  		  if(atol(caGitem)==1){
  	  				if(strcmp(caSdate,caEdate)!=0){
  	  				  	sprintf(caTemp,"select sum(ubytes+dbytes) bb,userid from ncipservice a,ncipindex b where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s')) and a.service=b.sid and b.groupid=%d ",caSdate,caEdate,caSdate,caShour,caEdate,caEhour,atol(caService));
  	  				    
  	  				  	 //�Ƿ���ʱ��
									if(atol(caWorkflag)==1){
										 if(strlen(caWorksql2)>0){
										 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
									   } 	
				  	  		}
  	  				  	
  	  				  	sprintf(caTemp+strlen(caTemp)," group by userid order by bb desc limit 0,%d ", 2*atol(caTopnum));
  	  				  }
  	  				 else{
  	  				   sprintf(caTemp,"select sum(ubytes+dbytes) bb,userid from ncipservice a,ncipindex b  where sdate='%s' and stime>='%s' and stime<='%s' and a.service=b.sid and b.groupid=%d ",caSdate,caShour,caEhour,atol(caService));
  	  				   //�Ƿ���ʱ��
									if(atol(caWorkflag)==1){
										 if(strlen(caWorksql2)>0){
										 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
									   } 	
				  	  		}
  	  			      sprintf(caTemp+strlen(caTemp)," group by userid order by bb desc limit 0,%d ", 2*atol(caTopnum));
  	  				  }
  	  		  }
  	  		else{
  	  			  if(strcmp(caSdate,caEdate)!=0){
  	  				  	sprintf(caTemp,"select sum(ubytes+dbytes) bb,b.groupid from ncipservice a,ncuser b,ncipindex c where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s')) and a.userid=b.userid and a.service=c.sid and c.groupid=%d ",caSdate,caEdate,caSdate,caShour,caEdate,caEhour,atol(caService));
  	  				  	//�Ƿ���ʱ��
									if(atol(caWorkflag)==1){
										 if(strlen(caWorksql2)>0){
										 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
									   } 	
				  	  		}
  	  				  	
  	  				  	sprintf(caTemp+strlen(caTemp)," group by b.groupid order by bb desc limit 0,%d ", 2*atol(caTopnum));
  	  				  }
  	  				 else{
  	  				   sprintf(caTemp,"select sum(ubytes+dbytes) bb,b.groupid from ncipservice a,ncuser b,ncipindex c where sdate='%s' and stime>='%s' and stime<='%s' and a.service=c.sid and a.userid=b.userid and c.groupid=%d ",caSdate,caShour,caEhour,atol(caService));
  	  				  	//�Ƿ���ʱ��
									if(atol(caWorkflag)==1){
										 if(strlen(caWorksql2)>0){
										 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
									   } 	
				  	  		}
  	  				  	
  	  				  	sprintf(caTemp+strlen(caTemp)," group by b.groupid order by bb desc limit 0,%d ", 2*atol(caTopnum));
  	  				  }
  	  		}
  	  	}
  	  	 	  	
  	  	printf("%d caTemp=%s\n",getpid(),caTemp);
  	  psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
        return 0;
       }
       lBytes=0;
       lService=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                     UT_TYPE_LONG,4,&lService);
       while((iReturn==0)||(iReturn==1405))
       {
       	index=ncUtlGetServicenumById(psService,lSumindex,lService);
	   	  if(index!=-1){
	
	          psService[index].lBytes=psService[index].lBytes+lBytes;         
	      }
	     else{
	          psService[lSumindex].lService=lService;
	          psService[lSumindex].lBytes=lBytes;
	          lSumindex++;
	          qsort(sGservice,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId2); 
       }
      	
	       lBytes=0;
	       lService=0;
	       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                     UT_TYPE_LONG,4,&lService);
       }                               
      pasDbCloseCursor(psCur); 
  	  	
 	  	
  	  }
  
  //��������С����	 
  	qsort(psService,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId);
  	
  	    iNum=0;
  	    lSj=0;
  	    for(i=0;i<lSumindex;i++){
  	    	if(iNum<atol(caTopnum)){
  	    
  	        if(atol(caGitem)==1){                    //���û�ͳ��
  	        psUser = ncUtlGetUserContById(psShmHead,psService[i].lService);
           if(psUser) {  
  	      
  	    //��������	 
  	    		 utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF8(psService[i].lBytes/1024,caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	         lOth=lOth+psService[i].lBytes;	    		 
  	    		 utPltPutLoopVar(psDbHead,"name_1",  iNum+1,psUser->dispname);
  	    		 if(psUser->psGroup)
  	    		   utPltPutLoopVar(psDbHead,"name_dept",iNum+1,psUser->psGroup->groupname);
  	    		 else
  	    		   utPltPutLoopVar(psDbHead,"name_dept",iNum+1,"");
  	    		    
  	    		if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*psService[i].lBytes/lSum);
    				}
  	    		  
  	    //�����ͼ		  
  	    		
  	    		 utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	psService[i].lBytes);  
             utPltPutLoopVar(psDbHead,"name",  iNum+1,psUser->dispname);
          
             utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
           
           
           
            
     				if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*psService[i].lBytes/lSum);
    				}
       
           
            iNum++;
  	    	}
       }
       else{                                                                         //������ͳ��
       	     	  
  	      
  	    //��������	 
  	    		 utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF8(psService[i].lBytes/1024,caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	         lOth=lOth+psService[i].lBytes;	    		 
  	    		 utPltPutLoopVar(psDbHead,"name_1",  iNum+1,ncLang("1820ȫ���û�"));
  	    		 printf("groupid=%d\n",psService[i].lService);
  	    		 utPltPutLoopVar(psDbHead,"name_dept",iNum+1,ncUtlGetGroupNameByid(psShmHead,psService[i].lService,ncLang("0629δ֪")));
  	    		if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*psService[i].lBytes/lSum);
    				}
  	    		  
  	    //�����ͼ		  
  	    		
  	    		 utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	psService[i].lBytes);  
             utPltPutLoopVar(psDbHead,"name",  iNum+1,ncUtlGetGroupNameByid(psShmHead,psService[i].lService,ncLang("0629δ֪")));
          

             utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
           
            
     				if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*psService[i].lBytes/lSum);
    				}
       
           
            iNum++;
  	    	
       	
       	
       	
       	
       }
       
       
       
       
       
      }
    }
     
    // �������
     if(lSum-lOth>0){
     	  	   utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	lSum-lOth);  
             utPltPutLoopVar(psDbHead,"name",  iNum+1,ncLang("0431����"));
            
             utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
             if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*(lSum-lOth)/lSum);
    	  			//   	  			
    				}
             
             
             utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF8((lSum-lOth)/1024,caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	             		 
  	    		 utPltPutLoopVar(psDbHead,"name_1",  iNum+1,ncLang("0431����"));
  	    		 utPltPutLoopVar(psDbHead,"name_dept",iNum+1,ncLang("0431����"));
  	    		if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				}
    				else{
    	  			utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*(lSum-lOth)/lSum);
    				} 
             
                                     
     }

if(strlen(caPdf)>0){
	//����ǩ
	 desc=malloc(4*sizeof(char*));
	 for(i=0;i<4;i++){
	 	desc[i]=malloc(32);
   }
   strcpy(desc[0],ncLang("0728�û���"));
   strcpy(desc[1],ncLang("0147����"));
    sprintf(desc[2],"%s(%s)",ncLang("1103������"),"KB");
   strcpy(desc[3],ncLang("1827�ٷֱ�%"));
     
 //�����PDF��     
   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*(atol(caTopnum)+2));
    if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("1863�����ڴ����"));
      return 0;
   }
  
   iNum=0;
   
char caG[36];

   	
   for(i=0;i<lSumindex;i++){
   	   
   	 if(iNum<atol(caTopnum)){
   	 	if(atol(caGitem)==1){
   	 		psUser=ncUtlGetUserContById(psShmHead,psService[i].lService);
   	 		if(psUser) { 
   	 			
   	 			pChart[iNum].name=strdup(psUser->dispname);  
   	 			
   	 			if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*psService[i].lBytes/lSum);
   	 			}
 	 		
   	 			pChart[iNum].percent=strdup(caTemp); 
   
   	 			pChart[iNum].amount=(float)psService[i].lBytes/1024; 	
  			 if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
  			 
          pChart[iNum].append=malloc(1*sizeof(char*));
          for(m=0;m<1;m++){
          	pChart[iNum].append[m]=malloc(32);
          }
          
          
            if(psUser->psGroup)
  	    		   strcpy(caG,psUser->psGroup->groupname);
  	    		 else
  	    		   strcpy(caG,"");
          
          strcpy(pChart[iNum].append[0],caG);
        
 			
  	 		
  	 			iNum++;
   	 		}
        }
        else{
        	 
        	
        	 pChart[iNum].name=strdup(ncUtlGetGroupNameByid(psShmHead,psService[i].lService,ncLang("0629δ֪")));
   	 			
   	 			if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*psService[i].lBytes/lSum);
   	 			}
 	 		
   	 			pChart[iNum].percent=strdup(caTemp); 
   	 			lDate=psService[i].lBytes/1024;
   	 			pChart[iNum].amount=(float)psService[i].lBytes/1024; 	

          pChart[iNum].append=malloc(1*sizeof(char*));
          for(m=0;m<1;m++){
          	pChart[iNum].append[m]=malloc(32);
          }
          strcpy(pChart[iNum].append[0],ncUtlGetGroupNameByid(psShmHead,psService[i].lService,ncLang("0629δ֪")));
        
 			
  	 		
  	 			iNum++;
        	
        	
        	
        }
        
        
        
        
        
        
     }
    }
     
         // �������
     if(lSum-lOth>0){
     		pChart[iNum].name=strdup(ncLang("0431����"));  
     	   	if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lSum-lOth)/lSum);
   	 			}
   	 			
     	   	pChart[iNum].percent=strdup(caTemp); 
   	 			lDate=(lSum-lOth)/1024;
   	 			pChart[iNum].amount=lDate; 	

          pChart[iNum].append=malloc(1*sizeof(char*));
          for(m=0;m<1;m++){
          	pChart[iNum].append[m]=malloc(32);
          }
          strcpy(pChart[iNum].append[0],ncLang("0431����"));
                   
          iNum++;
                                     
     }
     
     
     
     
     
      rowrow=iNum;
   
    
    sprintf(filename,"%s.PDF",caTitle); 
    sprintf(caStatdate,ncLang("1952ͳ��ʱ�䣺��%s %s:%s ��%s %s:%s"),	caSdate,caShour,caSmin,caEdate,caEhour,caEmin);	

     ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,4,1, caTitle,ncLang("1955��λ(KB)"),caStatdate,filename); 
     
//�ͷ��ڴ�     
   for(i=0;i<rowrow;i++){
	      free(pChart[i].name);
	      free(pChart[i].percent);
	      for(m=0;m<1;m++){
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
     free(psService);
     return 0;
    }
    
     
     
     
   free(psService);

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_servicefirstdet.htm");  
    return 0;
}






//7.5��ʾ��ַ���������ͼ
int ncWebShowWebClassBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    long lCount;
    long lMin;                              
    char caSdate_all[20];                                    
    int qt_flag=0;
    long lSj=0;
    
    char caPdf[12];
    struct chart_datasetf *pChart;           //����PDF�ĵ����
    int rowrow;                     
    int column; 
    char caTitle[128];
    char filename[128];
    char caStatdate[128];

    char **desc;    
    memset(caSmin,0,sizeof(caSmin));
    
   if(strcasecmp(utComGetVar_sd(psShmHead,"SaveHttpLog","No"),"File") == 0){
		return ncStatWebTracFromFile(psShmHead,iFd,psMsgHead);
	 }
    
     
    utMsgGetSomeNVar(psMsgHead,3,"stime",UT_TYPE_STRING,10,caStime,
																 "pre_flag",UT_TYPE_STRING,2,caPre_flag,
																 "pdf", UT_TYPE_STRING,8,caPdf);
    

														
 		                             
  psDbHead = utPltInitDb(); 
  
  
  	
 
//������ͳ��(nchttplog)
	    x_sum=15;
	    y_sum=15;
	 
      //����ʱ��
     if(strlen(caStime)==0){
		    lTime=time(0);
		    strcpy(caSdate,utTimFormat("%Y/%m/%d %H",lTime));
		    strcpy(caSmin,utTimFormat("%M",lTime));
	//	    printf("caSmin=%s\n",caSmin);
		    lMin=(long)(atol(caSmin)/15)*15;
//		    printf("lMin=%d\n",lMin);
		    sprintf(caSdate_all,"%s:%2d",caSdate,lMin);
		    lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);

		   lStarttime=lEndtime-900;
		  }
		  else{
		  	lStarttime=atol(caStime)-900;
		  	lEndtime=atol(caStime);
		  }
           
		  
		  if(atol(caPre_flag)==1){
		  	lStarttime=lStarttime-900;
		  	lEndtime=lEndtime-900;
		  }
		  else if(atol(caPre_flag)==2){
		  	lStarttime=lStarttime+900;
		  	lEndtime=lEndtime+900;
		  }
		 




    utPltPutVarF(psDbHead,"stime1","%d",lStarttime);
    utPltPutVarF(psDbHead,"etime1","%d",lEndtime);
    
    strcpy(caSdate,utTimFormat("%Y/%m/%d %H:%M",lStarttime));
    strcpy(caEdate,utTimFormat("%Y/%m/%d %H:%M",lEndtime));



    

    utPltPutVar(psDbHead,"tjsj",caSdate);
    utPltPutVarF(psDbHead,"stime","%d",lEndtime);
  
 //��ȡͳ��ʱ���������������ַ��𣬽���Щ������ͼ�����������ķ���鵽���������
    sprintf(caTemp,"select count(*) aa,tid,name from nchttplog,ncwebclass where vtime>=%d and vtime<%d and tid=id and tid!=0 group by tid order by aa desc limit 0,10 ",lStarttime,lEndtime);		    
     printf("caTemp=%s\n",caTemp);
     psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����2"));
        return 0;
     }
     lBytes=0;
     lService=0;
     memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
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
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                   UT_TYPE_LONG,4,&lService,
                                   UT_TYPE_STRING,30,caName);
    }
    
    pasDbCloseCursor(psCur);
   
    
    //���һ������������������
  
    	  sGservice[lSumindex].lService=99999;
    		sGservice[lSumindex].lGroupid=0;
    		sGservice[lSumindex].lBytes=0;
    		strcpy(sGservice[lSumindex].caGname,ncLang("0431����"));
        for(i=0;i<32;i++){
    	    sGservice[lSumindex].lBytesqs[i]=0;
    	    
        }
        

  //��serviceID����  Ϊ������ַ�������׼��		
    		qsort(sGservice,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId2); 
    	

       sprintf(caTemp,"select count(*) aa,from_unixtime(vtime,'%ci') bb,tid from nchttplog where vtime>=%d and vtime<=%d group by bb,tid ",'%',lStarttime,lEndtime);

 
 	  
   printf("caTemp=%s\n",caTemp);
   psCur = pasDbOpenSql(caTemp,0);
   if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����1"));
        return 0;
    }
    lCount=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
                                  
    iNum=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	
   
  
   	 
    	findex=atol(caTime);
    	if(findex>=15&&findex<30){                           //������ ʱ��ֵ�������±����30
    		findex=findex-15;
    	}
    	else if(findex>=30&&findex<45){
    		findex=findex-30;
    	}
    	else if(findex>=45){
    		findex=findex-45;
    	}
    	
    	  index=ncUtlGetServicenumById(&sGservice,lSumindex,lService);
 //  	  printf("index=%d\n",index);
//    	  printf("findex=%d\n",findex);
	   	  if(index!=-1){	
	          sGservice[index].lBytesqs[findex]=sGservice[index].lBytesqs[findex]+lCount;         
	      }
    	  else{
 //   	  	if(lCount>0)
 //   	  	   qt_flag=1;
    	  	   
    	  	sGservice[lSumindex].lBytesqs[findex]=sGservice[lSumindex].lBytesqs[findex]+lCount; 
    	  }
       	
  
    lCount=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
    }
    
    pasDbCloseCursor(psCur);
   


//���X��̶�

	  for(mm=0;mm<x_sum;mm++){
	  	    utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
          strcpy(caSmin,utTimFormat("%M",lStarttime));  
	  			sprintf(caTemp,"%s:%02d",utTimFormat("%H",lStarttime),mm+atol(caSmin));	 
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
	  			  	
	  }
	   
 //�������ͼ������ 

    for(i=0;i<=lSumindex;i++){
    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);
//    	printf("gservice=%d\n",sGservice[i].lService);
    	utPltPutLoopVar(psDbHead,"sevname",i+1,sGservice[i].caGname);
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",sGservice[i].lService);
    	for(mm=0;mm<x_sum;mm++){
//    		printf("i=%d;mm=%d;service=%d;bytes=%lld\n",i,mm,sGservice[i].lService,sGservice[i].lBytesqs[mm]);
    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);
  
    		  utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%lld",sGservice[i].lBytesqs[mm]);
    	
    }
  }
  utPltPutVarF(psDbHead,"sum","%d",lSumindex+1);
  utPltPutVarF(psDbHead,"sum_d","%d",x_sum);




    if(strlen(caPdf)>0){
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
   	      strcpy(caSmin,utTimFormat("%M",lStarttime));  
	  			sprintf(caTemp,"%s:%02d",utTimFormat("%H",lStarttime),i+atol(caSmin));	 
   		  
   	   pChart[i].name=strdup(caTemp);
   	  
   	  pChart[i].dataset=malloc((lSumindex+2)*sizeof(float)) ;
   	  for(mm=0;mm<lSumindex+1;mm++){
   	  	
   	  	 pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i];
   	  
   	  }
   	} 
   	
     
   
     
     
      rowrow=x_sum;
      column=lSumindex+1;
   
    strcpy(caTitle,ncLang("1452��ַ���������ͼ"));
    sprintf(filename,"%s.PDF",caTitle); 

    sprintf(caStatdate,ncLang("1967ͳ��ʱ��:%s"),caSdate);
//printf(caStatdate);
//printf("\n");
/*
   for(i=0;i<rowrow;i++){
   	for(m=0;m<lSumindex+1;m++)
   	  printf("name=%s,i=%d,m=%d,dataset=%f\n",pChart[i].name,i,m,pChart[i].dataset[m]);
   	 
   }
   for(i=0;i<lSumindex+1;i++){
   	  printf("i=%d,desc=%s\n",i,desc[i]);

   }
printf("caTitle=%s,filename=%s,rowrow=%d,column=%d,caUnit=%s\n",caTitle,filename,rowrow,column,caUnit);
*/
  sprintf(caTemp,ncLang("1942��λ(%s)"),ncLang("1898��"));

//printf(caStatdate);
   ncWebShowServiceBytime_PDF(iFd,pChart,desc,rowrow,column,caTitle,caTemp,caStatdate,filename);


//printf("end\n"); 
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




 //  utPltPutVar(psDbHead,"unit","��");
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_mainfirst_webclassbytime.htm");
    
    return 0;
}


//��ֹ�û�����
int ncUserForbit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
     ncUserInfo *psUserInfo;
     unsigned long lSip;
     char caUserid[12],caUname[32],caIp[16],caStime[16];
    
		utMsgGetSomeNVar(psMsgHead,4,"userid",UT_TYPE_STRING,10,caUserid,
		                             "uname", UT_TYPE_STRING,30,caUname,
		                             "ip",    UT_TYPE_STRING,15,caIp,
		                             "stime", UT_TYPE_STRING,10,caStime);
																
//printf("stime=%s\n",caStime);	
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"uname",caUname);  
     lSip = ntohl(utComHostAddress(caIp));
//   printf("caIp=%s\n",caIp);
    psUserInfo = ncGetUserInfoByIp(psShmHead,lSip);
    if(psUserInfo){
   	   ncUtlSetUserStatus(psShmHead,psUserInfo,0,atol(caStime));
   	   utPltPutVar(psDbHead,"mesg",ncLang("1822�����ɹ�"));
    }
    else{
    	utPltPutVar(psDbHead,"mesg",ncLang("1823�������ɹ�"));
    }
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/userforbit.htm");
    
    return 0;
}



//7.5��ʾĳ��ַ���������ͼ  ǰʮ���û���������ʾ��������
int ncWebShowWebClassBytimeDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    char caTid[16];
    long lCount;
    long lMin;                              
    char caSdate_all[20];                                    
    int qt_flag=0;
    long lSj=0;
    long lCode;
    
     char caPdf[12];
    
    struct chart_datasetf *pChart;           //����PDF�ĵ����
    int rowrow;                     
    int column; 
    char caTitle[128];
    char filename[128];
    char caStatdate[128];

    char **desc;
     		utMsgGetSomeNVar(psMsgHead,9,"stime",UT_TYPE_STRING,10,caStime,
   		                             "gitem", UT_TYPE_STRING,10,caGitem,
   		                             "sdate", UT_TYPE_STRING,10,caSdate,
   		                             "shour", UT_TYPE_STRING,8,caShour,
   		                             "smin",  UT_TYPE_STRING,8,caSmin,
   		                             "av_un", UT_TYPE_STRING,8,caAv_un,
																   "pre_flag",UT_TYPE_STRING,2,caPre_flag,
																   "tid",     UT_TYPE_STRING,10,caTid,
																   "pdf",      UT_TYPE_STRING,8,caPdf);
 	//	  lCode=utMd5Code("fasdfasd",7,"");  
     
     
   if(strcasecmp(utComGetVar_sd(psShmHead,"SaveHttpLog","No"),"File") == 0){
		return ncStatWebTracDetFromFile(psShmHead,iFd,psMsgHead);
	 }  
     
                         
		                             
  psDbHead = utPltInitDb(); 
//  printf("caStime=%s\n",caStime);
        if(strlen(caGitem)==0) strcpy(caGitem,"1");
        if(strlen(caAv_un)==0) strcpy(caAv_un,"1");

//  printf("caTid=%s\n",caTid);  
 // printf("classname=%s\n",ncUrlGetClassNameById(atol(caTid),""));
  	utPltPutVar(psDbHead,"servname",ncUrlGetClassNameById(atol(caTid),""));
 sprintf(caTitle,ncLang("1453��ַ���[%s]��������ͼ"),ncUrlGetClassNameById(atol(caTid),""));
//������ͳ��(nchttplog)
 if(atol(caAv_un)==1){
	    x_sum=15;
	    y_sum=15;
	 }
	 else if(atol(caAv_un)==2){
	 	x_sum=24;
	 	y_sum=24;
	}
	else if(atol(caAv_un)==3){
		x_sum=30;
		y_sum=30;
	}
			if((strlen(caStime)==0)&&(strlen(caSdate)==0)){
		    lTime=time(0);
		    strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));
		    strcpy(caShour,utTimFormat("%H",lTime));
		    strcpy(caSmin,utTimFormat("%M",lTime));
	//	    printf("caSmin=%s\n",caSmin);
	      if(atol(caAv_un)==1){
		    lMin=(long)(atol(caSmin)/15)*15;
		    sprintf(caSmin,"%2d",lMin);
//		    printf("lMin=%d\n",lMin);
		        sprintf(caSdate_all,"%s %s:%2d",caSdate,caShour,lMin);
		        lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		        lEndtime=lStarttime+900;
		     }
		     else if(atol(caAv_un)==2){
		     	strcpy(caSmin,"00");
		     	strcpy(caShour,"00");
		     	sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
		     	lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		     	lEndtime=lStarttime+24*3600;
		    }
		    else{   
		    	  ncUtlMonthDate(lTime,caSdate,caEdate);	
		    	 	strcpy(caSmin,"00");
		     	  strcpy(caShour,"00");  
				 		lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
						lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate);                                                //����ͳ��
		    }
		  }
		  else if(strlen(caStime)>0){
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
		    	 	
		    }
		    else if(atol(caAv_un)==3){
		    	ncUtlMonthDate(lStarttime,caSdate,caEdate);
		    	strcpy(caShour,"00");
		    	strcpy(caSmin,"00");
		    	lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
					lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate); 
		    }  
		  else{
//		  		 lMin=(long)(atol(caSmin)/15)*15;
//		        sprintf(caSmin,"%2d",lMin);
//		    printf("lMin=%d\n",lMin);
//		        sprintf(caSdate_all,"%s %s:%2d",caSdate,caShour,lMin);
//		        lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		        lEndtime=lStarttime+900;
		        printf("lStimetime=%d\n",lStarttime);
		   }
		  }
		  else if(strlen(caSdate)>0){
		  	if(atol(caAv_un)==1){
		  		lMin=(long)(atol(caSmin)/15)*15;
		  		sprintf(caSdate_all,"%s %s:%2d",caSdate,caShour,lMin);
		  		lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		      lEndtime=lStarttime+900;
		  	}
		  	else if(atol(caAv_un)==2){
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  		lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
					lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate); 
		  	 }
		  	 else if(atol(caAv_un)==3){
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
		  	}
		  	else if(atol(caAv_un)==2){
		  		lStarttime=lStarttime-24*3600;
		  		lEndtime=lEndtime-24*3600;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  	}
		  	else if(atol(caAv_un)==3){
		  		lStarttime=lStarttime-15*24*3600;
		  		ncUtlMonthDate(lStarttime,caSdate,caEdate);

		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  	}
		  }
		 else if(atol(caPre_flag)==2){
		 	  if(atol(caAv_un)==1){
		 			lStarttime=lStarttime+900;
		  		lEndtime=lEndtime+900;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		      strcpy(caShour,utTimFormat("%H",lStarttime));
		      strcpy(caSmin,utTimFormat("%M",lStarttime));
		    }
		    else if(atol(caAv_un)==2){
		  		lStarttime=lStarttime+24*3600;
		  		lEndtime=lEndtime+24*3600;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  	}
		  	else if(atol(caAv_un)==3){
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

   utPltPutVar(psDbHead,"gitem",caGitem);
   utPltPutVar(psDbHead,"avun",caAv_un);
   
   
 //     strcpy(caSdate,utTimFormat("%Y/%m/%d %H:%M",lStarttime));
//    strcpy(caEdate,utTimFormat("%Y/%m/%d %H:%M",lEndtime));


    utPltPutVar(psDbHead,"tjsj",caSdate);
    utPltPutVarF(psDbHead,"stime","%d",lEndtime);
  
 //��ȡͳ��ʱ���������������ַ��𣬽���Щ������ͼ�����������ķ���鵽���������
 if(atol(caAv_un)==1){                                           //������ƽ��
 	  if(atol(caGitem)==1){                                        // �û�ͳ��
    	sprintf(caTemp,"select count(*) aa,a.userid,b.dispname from nchttplog a,ncuser b where vtime>=%d and vtime<%d and a.userid=b.userid and a.tid=%d group by a.userid order by aa desc limit 0,10 ",lStarttime,lEndtime,atol(caTid));	
    }	
    else if(atol(caGitem)==2){                                  //������ͳ��
    	sprintf(caTemp,"select count(*) aa,b.groupid,c.groupname from nchttplog a,ncuser b,ncgroup c where vtime>=%d and vtime<%d and a.userid=b.userid and b.groupid=c.groupid and a.tid=%d group by a.groupid order by aa desc limit 0,10 ",lStarttime,lEndtime,atol(caTid));	
    	
    }
    else if(atol(caGitem)==3){                                  //��������ַͳ��
    	sprintf(caTemp,"select count(*) aa,0,host from nchttplog where vtime>=%d and vtime<%d and tid=%d group by host order by aa desc limit 0,10  ",lStarttime,lEndtime,atol(caTid));	
    }
  }
  else if(atol(caAv_un)==2){  
  	  if(atol(caGitem)==1){                      //��Сʱͳ���û�
  	   sprintf(caTemp,"select sum(lcount) aa,a.userid,b.dispname from nchttpcount a,ncuser b where sdate='%s' and a.userid=b.userid and a.tid=%d group by a.userid order by aa desc limit 0,10 ",caSdate,atol(caTid));
  	  }
  	  else if(atol(caGitem)==2){                 //��Сʱ����ͳ��
  	  	sprintf(caTemp,"select sum(lcount) aa,b.groupid,c.groupname from nchttpcount a,ncuser b,ncgroup c where sdate='%s' and a.userid=b.userid and b.groupid=c.groupid and a.tid=%d group by b.groupid order by aa desc limit 0,10 ",caSdate,atol(caTid));
  	  }
  	  else if(atol(caGitem)==3){                 //��Сʱͳ����ַ
  	  	sprintf(caTemp,"select sum(lcount) aa,0,host from nchttpcount where sdate='%s' and tid=%d group by host order by aa desc limit 0,10 ",caSdate,atol(caTid));
  	  }
  } 
  else if(atol(caAv_un)==3){
  	if(atol(caGitem)==1){                              //����ͳ���û�
  		sprintf(caTemp,"select sum(lcount) aa,a.userid,b.dispname from nchttpcount a,ncuser b where sdate>='%s' and sdate<='%s' and a.userid=b.userid and a.tid=%d group by a.userid order by aa desc limit 0,10 ",caSdate,caEdate,atol(caTid));		
  	}
  	else if(atol(caGitem)==2){                          //����ͳ�Ʋ���
  		sprintf(caTemp,"select sum(lcount) aa,b.groupid,c.groupname from nchttpcount a,ncuser b,ncgroup c where sdate>='%s' and sdate<='%s' and a.userid=b.userid and b.groupid=c.groupid and a.tid=%d group by b.groupid order by aa desc limit 0,10",caSdate,caEdate,atol(caTid));
  	}
  	else if(atol(caGitem)==3){                         //����ͳ����ַ
  		sprintf(caTemp,"select sum(lcount) aa,0,host from nchttpcount where sdate>='%s' and sdate<='%s' and tid=%d group by host order by aa desc limit 0,10 ",caSdate,caEdate,atol(caTid));
  	}
  }   
     printf("caTemp=%s\n",caTemp);
     psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����2"));
        return 0;
     }
     lBytes=0;
     lService=0;
     memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                   UT_TYPE_LONG,4,&lService,
                                   UT_TYPE_STRING,30,caName);
                                  
    lSumindex=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	if(atol(caGitem)==3){
    		lService=utMd5Code(caName,strlen(caName),"");
    	}
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
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
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
    	



		 if(atol(caAv_un)==1){                                           //������ƽ��
		 	  if(atol(caGitem)==1){                                        // �û�ͳ��
		    	  sprintf(caTemp,"select count(*) aa,from_unixtime(vtime,'%ci') bb,a.userid from nchttplog a where vtime>=%d and vtime<%d  and a.tid=%d group by bb,a.userid   ",'%',lStarttime,lEndtime,atol(caTid));	
		    }
		    else if(atol(caGitem)==2){                                   //������ͳ��
		    		sprintf(caTemp,"select count(*) aa,from_unixtime(vtime,'%ci') bb,b.groupid from nchttplog a,ncuser b where vtime>=%d and vtime<%d and a.userid=b.userid and a.tid=%d group by bb,a.groupid   ",'%',lStarttime,lEndtime,atol(caTid));	
		    }
		    else if(atol(caGitem)==3){                                  //��������ַͳ��
    				sprintf(caTemp,"select count(*) aa,from_unixtime(vtime,'%ci') bb,host from nchttplog where vtime>=%d and vtime<%d and tid=%d group by bb,host   ",'%',lStarttime,lEndtime,atol(caTid));	
    		}
		  }
      else if(atol(caAv_un)==2){  
  	  if(atol(caGitem)==1){                      //��Сʱͳ���û�
  	   sprintf(caTemp,"select sum(lcount) aa,a.stime,a.userid from nchttpcount a where sdate='%s'  and a.tid=%d group by a.stime,a.userid  ",caSdate,atol(caTid));
  	  }
  	  else if(atol(caGitem)==2){                             //��Сʱͳ�Ʋ���
  	  	 sprintf(caTemp,"select sum(lcount) aa,a.stime,b.groupid from nchttpcount a,ncuser b where sdate='%s' and a.userid=b.userid and a.tid=%d group by a.stime,b.groupid ",caSdate,atol(caTid));
  	  }
  	  else if(atol(caGitem)==3){
  	  	sprintf(caTemp,"select sum(lcount) aa,stime,host from nchttpcount where sdate='%s' and tid=%d group by stime,host  ",caSdate,atol(caTid));
  	  }
     }  
     else if(atol(caAv_un)==3){                    //����ͳ���û�
     	if(atol(caGitem)==1){
     		sprintf(caTemp,"select sum(lcount) aa,substring(a.sdate,9,2) bb,userid from nchttpcount a where sdate>='%s' and sdate<='%s' and tid=%d group by bb,userid ",caSdate,caEdate,atol(caTid));
     	}
     	else if(atol(caGitem)==2){                   //����ͳ�Ʋ���
     		sprintf(caTemp,"select sum(lcount) aa,substring(a.sdate,9,2) bb,b.groupid from nchttpcount a,ncuser b where sdate>='%s' and sdate<='%s' and a.userid=b.userid and tid=%d group by bb,b.groupid ",caSdate,caEdate,atol(caTid));
    }  
    else if(atol(caGitem)==3){                     //����ͳ����ַ
    	sprintf(caTemp,"select sum(lcount) aa,substring(sdate,9,2) bb,host from nchttpcount where sdate>='%s' and sdate<='%s' and tid=%d group by bb,host ",caSdate,caEdate,atol(caTid));
    }
 }
 
	  
   printf("caTemp=%s\n",caTemp);
   psCur = pasDbOpenSql(caTemp,0);
   if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����1"));
        return 0;
    }
    lCount=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    memset(caName,0,sizeof(caName));
    if(atol(caGitem)==3){ 
    	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_STRING,30,caName);
    }
    else   
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
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
    	if(atol(caGitem)==3){ 
    		lService=utMd5Code(caName,strlen(caName),"");
    	 }
    	  index=ncUtlGetServicenumById(&sGservice,lSumindex,lService);
 //  	  printf("index=%d\n",index);
//    	  printf("findex=%d\n",findex);
	   	  if(index!=-1){	
	          sGservice[index].lBytesqs[findex]=sGservice[index].lBytesqs[findex]+lCount;         
	      }
    	  else{
 //   	  	if(lCount>0)
 //   	  	   qt_flag=1;
 // printf("findex=%d\n",findex);  	  	   
    	  	sGservice[lSumindex].lBytesqs[findex]=sGservice[lSumindex].lBytesqs[findex]+lCount; 
    	  }
       	
  
    lCount=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    
     memset(caName,0,sizeof(caName));
    if(atol(caGitem)==3){ 
    	  iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_STRING,30,caName);
    }
    else
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
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
//    	printf("gservice=%d\n",sGservice[i].lService);
    	utPltPutLoopVar(psDbHead,"sevname",i+1,sGservice[i].caGname);
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",sGservice[i].lService);
    	for(mm=0;mm<x_sum;mm++){
//    		printf("i=%d;mm=%d;service=%d;bytes=%lld\n",i,mm,sGservice[i].lService,sGservice[i].lBytesqs[mm]);
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






    if(strlen(caPdf)>0){
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
	  		  }
   		  else if(atol(caAv_un)==2){
	  			sprintf(caTemp,"%02d:00",i);	  		  
	  		}
	  		else if(atol(caAv_un)==3){	  			
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
   
    
     
    sprintf(filename,"%s.PDF",caTitle); 

    sprintf(caStatdate,ncLang("1962ͳ��ʱ��:%s %s:%s"),caSdate,caShour,caSmin);


sprintf(caTemp,ncLang("1942��λ(%s)"),ncLang("1898��"));
   ncWebShowServiceBytime_PDF(iFd,pChart,desc,rowrow,column,caTitle,caTemp,caStatdate,filename);


//printf("end\n"); 
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





 //  utPltPutVar(psDbHead,"unit","��");
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_mainfirst_webclassbytimedet.htm");
    
    return 0;
}



//7.5��ʾĳ�����  ǰʮ���û���������ʾ��������
int ncWebServiceBytimeDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    pasDbCursor *psCur;
    char caDate[20];  

    char caPre_flag[8],caAv_un[8];
    char sqlbuf[2048];
    int  iReturn,i,j;
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
    char caTid[16];
    long lCount;
    long lMin;                              
    char caSdate_all[20];                                    
    int qt_flag=0;
    long lSj=0;
    long lCode;
    char caFlag[16];
    char caPdf[12];
    struct chart_datasetf *pChart;           //����PDF�ĵ����
    int rowrow;                     
    int column; 
    char caTitle[128];
    char filename[128];
    char caStatdate[128];

    char **desc;
    
    
    
    lMaxbytes=0;
     		utMsgGetSomeNVar(psMsgHead,10,"stime",UT_TYPE_STRING,10,caStime,
   		                             "gitem", UT_TYPE_STRING,10,caGitem,
   		                             "sdate", UT_TYPE_STRING,10,caSdate,
   		                             "shour", UT_TYPE_STRING,8,caShour,
   		                             "smin",  UT_TYPE_STRING,8,caSmin,
   		                             "av_un", UT_TYPE_STRING,8,caAv_un,
																   "pre_flag",UT_TYPE_STRING,2,caPre_flag,
																   "tid",     UT_TYPE_STRING,10,caTid,
																   "flag",    UT_TYPE_STRING,10,caFlag,
																   "pdf",      UT_TYPE_STRING,8,caPdf);
 	//	  lCode=utMd5Code("fasdfasd",7,"");  
                         
		                             
  psDbHead = utPltInitDb(); 
//  printf("caStime=%s\n",caStime);
        if(strlen(caGitem)==0) strcpy(caGitem,"1");
        if(strlen(caAv_un)==0) strcpy(caAv_un,"1");



 if(atol(caFlag)==100){
  	sprintf(caTemp,"select name from ncservicecgroup where groupid=%d ",atol(caTid));
  	memset(caName,0,sizeof(caName));
  	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caName);
  	utPltPutVar(psDbHead,"servname",caName);
  	 sprintf(caTitle,ncLang("1454�������[%s]�����ֲ�����ͼ"),caName);
  
  }
  else {
  	utPltPutVar(psDbHead,"servname",ncUtlGetServiceName(psShmHead,atol(caTid),ncLang("0431����")));
  	sprintf(caTitle,ncLang("1454�������[%s]�����ֲ�����ͼ"),ncUtlGetServiceName(psShmHead,atol(caTid),ncLang("0431����")));
  }

  
//������ͳ��(nchttplog)
 if(atol(caAv_un)==1){
	    x_sum=30;
	    y_sum=30;
	 }
	 else if(atol(caAv_un)==2){
	 	x_sum=24;
	 	y_sum=24;
	}
	else if(atol(caAv_un)==3){
		x_sum=30;
		y_sum=30;
	}
			if((strlen(caStime)==0)&&(strlen(caSdate)==0)){
		    lTime=time(0);
		    strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime));
		    strcpy(caShour,utTimFormat("%H",lTime));
		    strcpy(caSmin,utTimFormat("%M",lTime));
	//	    printf("caSmin=%s\n",caSmin);
	      if(atol(caAv_un)==1){
		    lMin=(long)(atol(caSmin)/30)*30;
		    sprintf(caSmin,"%2d",lMin);
//		    printf("lMin=%d\n",lMin);
		        sprintf(caSdate_all,"%s %s:%2d",caSdate,caShour,lMin);
		        lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		        lEndtime=lEndtime+1800;
		     }
		     else if(atol(caAv_un)==2){
		     	strcpy(caSmin,"00");
		     	strcpy(caShour,"00");
		     	sprintf(caSdate_all,"%s %s:%s",caSdate,caShour,caSmin);
		     	lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		     	lEndtime=lStarttime+24*3600;
		    }
		    else{   
		    	  ncUtlMonthDate(lTime,caSdate,caEdate);	
		    	 	strcpy(caSmin,"00");
		     	  strcpy(caShour,"00");  
				 		lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
						lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate);                                                //����ͳ��
		    }
		  }
		  else if(strlen(caStime)>0){
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
		    	 	
		    }
		    else if(atol(caAv_un)==3){
		    	ncUtlMonthDate(lStarttime,caSdate,caEdate);
		    	strcpy(caShour,"00");
		    	strcpy(caSmin,"00");
		    	lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
					lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate); 
		    }  
		  else{
//		  		 lMin=(long)(atol(caSmin)/15)*15;
//		        sprintf(caSmin,"%2d",lMin);
//		    printf("lMin=%d\n",lMin);
//		        sprintf(caSdate_all,"%s %s:%2d",caSdate,caShour,lMin);
//		        lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		        lEndtime=lStarttime+1800;
		        printf("lStimetime=%d\n",lStarttime);
		   }
		  }
		  else if(strlen(caSdate)>0){
		  	if(atol(caAv_un)==1){
		  		lMin=(long)(atol(caSmin)/30)*30;
		  		sprintf(caSdate_all,"%s %s:%2d",caSdate,caShour,lMin);
		  		lStarttime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		      lEndtime=lStarttime+1800;
		  	}
		  	else if(atol(caAv_un)==2){
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  		lStarttime=utTimStrToLong("%Y/%m/%d 00:00:00",caSdate);		
					lEndtime=utTimStrToLong("%Y/%m/%d 23:59:59",caEdate); 
		  	 }
		  	 else if(atol(caAv_un)==3){
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
		  		lStarttime=lStarttime-1800;
		  		lEndtime=lEndtime-1800;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		      strcpy(caShour,utTimFormat("%H",lStarttime));
		      strcpy(caSmin,utTimFormat("%M",lStarttime));
		  	}
		  	else if(atol(caAv_un)==2){
		  		lStarttime=lStarttime-24*3600;
		  		lEndtime=lEndtime-24*3600;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  	}
		  	else if(atol(caAv_un)==3){
		  		lStarttime=lStarttime-15*24*3600;
		  		ncUtlMonthDate(lStarttime,caSdate,caEdate);

		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  	}
		  }
		 else if(atol(caPre_flag)==2){
		 	  if(atol(caAv_un)==1){
		 			lStarttime=lStarttime+1800;
		  		lEndtime=lEndtime+1800;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		      strcpy(caShour,utTimFormat("%H",lStarttime));
		      strcpy(caSmin,utTimFormat("%M",lStarttime));
		    }
		    else if(atol(caAv_un)==2){
		  		lStarttime=lStarttime+24*3600;
		  		lEndtime=lEndtime+24*3600;
		  		strcpy(caSdate,utTimFormat("%Y/%m/%d",lStarttime));
		  		strcpy(caShour,"00");
		  		strcpy(caSmin,"00");
		  	}
		  	else if(atol(caAv_un)==3){
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

   utPltPutVar(psDbHead,"gitem",caGitem);
   utPltPutVar(psDbHead,"avun",caAv_un);
   
   
 //     strcpy(caSdate,utTimFormat("%Y/%m/%d %H:%M",lStarttime));
//    strcpy(caEdate,utTimFormat("%Y/%m/%d %H:%M",lEndtime));


    utPltPutVar(psDbHead,"tjsj",caSdate);
    utPltPutVarF(psDbHead,"stime","%d",lEndtime);
  
 //��ȡͳ��ʱ������������û�������Щ������ͼ�����������ķ���鵽���������
 if(atol(caAv_un)==1){                                           //������ƽ��
 	  if(atol(caGitem)==1){   
 	  	if(atol(caFlag)==100){                         //ͳ�Ʒ������
 	  		sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.userid,b.dispname from nciplog a,ncuser b,ncipindex c where vtime>=%d and vtime<%d and a.userid=b.userid and a.service=c.sid and c.groupid=%d group by a.useid order by aa desc limit 0,10 ",lStarttime,lEndtime,atol(caTid));
 	  	}
 	  		else{                                     // �û�ͳ��
    	    sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.userid,b.dispname from nciplog a,ncuser b where vtime>=%d and vtime<%d and a.userid=b.userid and a.service=%d group by a.userid order by aa desc limit 0,10 ",lStarttime,lEndtime,atol(caTid));	
       }
    }	
    else if(atol(caGitem)==2){                                  //������ͳ��
    	if(atol(caFlag)==100){
    		  sprintf(caTemp,"select sum(ubytes+dbytes) aa,b.groupid,c.groupname from nciplog a,ncuser b,ncgroup c,ncipindex d where vtime>=%d and vtime<=%d and a.userid=b.userid and b.groupid=c.groupid and a.service=d.sid and d.groupid=%d group by b.groupid order by aa desc limit 0,10 ",lStarttime,lEndtime,atol(caTid));
    	}
    	else{
    		 sprintf(caTemp,"select sum(ubytes+dbytes) aa,b.groupid,c.groupname from nciplog a,ncuser b,ncgroup c where vtime>=%d and vtime<=%d and a.userid=b.userid and b.groupid=c.groupid  and a.service=%d group by b.groupid order by aa desc limit 0,10 ",lStarttime,lEndtime,atol(caTid));

      }
    }
  }
  else if(atol(caAv_un)==2){  
  	  if(atol(caGitem)==1){                      //��Сʱͳ���û�
  	  	if(atol(caFlag)==100){
  	  		sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.userid,b.dispname from ncipservice a,ncuser b,ncipindex c where sdate='%s' and a.userid=b.userid and a.service=c.sid and c.groupid=%d group by a.userid order by aa desc limit 0,10 ",caSdate,atol(caTid));
  	  	}
  	  	else{
  	  		sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.userid ,b.dispname from ncipservice a,ncuser b where sdate='%s' and a.userid=b.userid and a.service=%d group by a.userid order by aa desc limit 0,10 ",caSdate,atol(caTid));
  	  	}

  	  }
  	  else if(atol(caGitem)==2){                 //��Сʱ����ͳ��
  	  	if(atol(caFlag)==100){
  	  		sprintf(caTemp,"select sum(ubytes+dbytes) aa,b.groupid,c.groupname from ncipservice a,ncuser b,ncgroup c,ncipindex d where sdate='%s' and a.userid=b.userid and b.groupid=c.groupid and a.service=d.sid and d.groupid=%d group by b.groupid order by aa desc limit 0,10",caSdate,atol(caTid));
  	  		
  	  	}
  	  	else{
  	  		sprintf(caTemp,"select sum(ubytes+dbytes) aa,b.groupid,c.groupname from ncipservice a,ncuser b,ncgroup c where sdate='%s' and a.userid=b.userid and b.groupid=c.groupid and a.service=%d group by b.groupid order by aa desc limit 0,10",caSdate,atol(caTid));
  	  	}
  	  	
  	  }
  	 
  } 
  else if(atol(caAv_un)==3){
  	if(atol(caGitem)==1){                              //����ͳ���û�
  		if(atol(caFlag)==100){
  			sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.userid,b.dispname from ncipservice a,ncuser b,ncipindex c where sdate>='%s' and sdate<='%s' and c.groupid=%d group by a.userid order by aa desc limit 0,10",caSdate,caEdate,atol(caTid));
  			
  		}
  		else{
  			sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.userid,b.dispname from ncipservice a,ncuser b where sdate>='%s' and sdate<='%s' and a.userid=b.userid and a.service=%d group by a.userid order by aa desc limit 0,10",caSdate,caEdate,atol(caTid));
  		}
  		
  	}
  	else if(atol(caGitem)==2){                          //����ͳ�Ʋ���
  		if(atol(caFlag)==100){
  			sprintf(caTemp,"select sum(ubytes+dbytes) aa,b.groupid,d.groupname  from ncipservice a,ncuser b,ncipindex c,ncgroup d where sdate>='%s' and sdate<='%s' and a.userid=b.userid and a.service=c.sid and b.groupid=d.groupid and c.groupid=%s group by b.groupid order by aa desc limit 0,10",caSdate,caEdate,atol(caTid));
       }
       else{
       	sprintf(caTemp,"select sum(ubytes+dbytes) aa,b.groupid,c.groupname from ncipservice a,ncuser b,ncgroup c where sdate>='%s' and sdate<='%s' and a.userid=b.userid and b.groupid=c.groupid and a.service=%d group by b.groupid order by aa desc limit 0,10",caSdate,caEdate,atol(caTid));
      }
  	}
  
  }   
     printf("caTemp=%s\n",caTemp);
     psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����2"));
        return 0;
     }
     lBytes=0;
     lService=0;
     memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                   UT_TYPE_LONG,4,&lService,
                                   UT_TYPE_STRING,30,caName);
                                  
    lSumindex=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	
    		sGservice[lSumindex].lService=lService;
    		sGservice[lSumindex].lGroupid=0;
    		sGservice[lSumindex].lBytes=lBytes;
    		strcpy(sGservice[lSumindex].caGname,caName);
    	for(i=0;i<32;i++){
    		sGservice[lSumindex].lBytesqs[i]=0;
    	}
    	
    	
    lSumindex++;
    lBytes=0;
    lService=0;
     memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
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
    	



		 if(atol(caAv_un)==1){                                           //������ƽ��
		 	  if(atol(caGitem)==1){                                        // �û�ͳ��
		 	  	  if(atol(caFlag)==100)
		    	    sprintf(caTemp,"select sum(ubytes+dbytes) aa,from_unixtime(vtime,'%ci') bb,a.userid from nciplog a,ncipindex b where vtime>=%d and vtime<%d and a.service=b.sid and b.groupid=%d group by bb,a.userid   ",'%',lStarttime,lEndtime,atol(caTid));	
		    	  else
		    	   sprintf(caTemp,"select sum(ubytes+dbytes) aa,from_unixtime(vtime,'%ci') bb,userid from nciplog where vtime>=%d and vtime<%d and service=%d group by bb,userid ",'%',lStarttime,lEndtime,atol(caTid));
		    }
		    else if(atol(caGitem)==2){                                   //������ͳ��
		    	 if(atol(caFlag)==100)
		    	   sprintf(caTemp,"select sum(ubytes+dbytes) aa,from_unixtime(vtime,'%ci') bb,b.groupid from nciplog a,ncuser b,ncipindex c where vtime>=%d and vtime<%d and a.userid=b.userid and a.service=c.sid and c.groupid=%d group by bb,b.groupid ",'%',lStarttime,lEndtime,atol(caTid));
           else
             sprintf(caTemp,"select sum(ubytes+dbytes) aa,from_unixtime(vtime,'%ci') bb,b.groupid from nciplog a,ncuser b where vtime>=%d and vtime<%d and a.userid=b.userid and a.service=%d group by bb,b.groupid ",'%',lStarttime,lEndtime,atol(caTid));
		    }
		   
		  }
      else if(atol(caAv_un)==2){  
  	  if(atol(caGitem)==1){                      //��Сʱͳ���û�
  	  	if(atol(caFlag)==100)
  	  	  sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.stime,a.userid from ncipservice a,ncipindex b where sdate='%s' and a.service=b.sid and a.service=%d group by a.stime,a.userid ",caSdate,atol(caTid));
  	  	 else
  	  	  sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.stime,a.userid from ncipservice a where sdate='%s' and a.service=%d group by a.stime,a.userid ",caSdate,atol(caTid));
  	  
  	  }
  	  else if(atol(caGitem)==2){                             //��Сʱͳ�Ʋ���
  	  	if(atol(caFlag)==100)
  	  	  sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.stime,c.groupid from ncipservice a,ncipindex b,ncuser c where sdate='%s' and a.userid=c.userid and a.service=b.sid and b.groupid=%d group by a.stime,c.groupid ",caSdate,atol(caTid));
  	  	else
  	  	 sprintf(caTemp,"select sum(ubytes+dbytes) aa,a.stime,c.groupid from ncipservice a,ncuser c where sdate='%s' and a.userid=c.userid and a.service=%d group  by a.stime,c.groupid ",caSdate,atol(caTid));
  	  	
  	  }
  	 
     }  
     else if(atol(caAv_un)==3){                    //����ͳ���û�
     	if(atol(caGitem)==1){
     		if(atol(caFlag)==100)
     		  sprintf(caTemp,"select sum(ubytes+dbytes) aa,substring(a.sdate,9,2) bb,a.userid from ncipservice a,ncipindex b where sdate>='%s' and sdate<='%s' and a.service=b.sid and b.groupid=%d group by bb,userid ",caSdate,caEdate,atol(caTid));
        else
          sprintf(caTemp,"select sum(ubytes+dbytes) aa,substring(a.sdate,9,2) bb,a.userid from ncipservice a where sdate>='%s' and sdate<='%s' and a.service=%d group by bb,userid ",caSdate,caEdate,atol(caTid));
     	}
     	else if(atol(caGitem)==2){                   //����ͳ�Ʋ���
     		if(atol(caFlag)==100)
     		  sprintf(caTemp,"select sum(ubytes+dbytes) aa,substring(a.sdate,9,2) bb,b.groupid from ncipservice a,ncuser b,ncipindex c where sdate>='%s' and sdate<='%s' and a.service=c.sid and a.userid=b.useid and c.groupid=%d group by bb.b.groupid ",caSdate,caEdate,atol(caTid));
     		 else
     		  sprintf(caTemp,"select sum(ubytes+dbytes) aa,substring(a.sdate,9,2) bb,b.groupid from ncipservice a,ncuser b where sdate>='%s' and sdate<='%s' and a.userid=b.userid and a.service=%d group by bb,b.groupid ",caSdate,caEdate,atol(caTid));

    }  
   
 }
 
	  
   printf("caTemp=%s\n",caTemp);
   psCur = pasDbOpenSql(caTemp,0);
   if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0587ͼ��ͳ��"),ncLang("0549���ݿ����1"));
        return 0;
    }
    lCount=0;
    lBytes=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    memset(caName,0,sizeof(caName));
  
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
                                  
    iNum=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	
 //  printf("caTime=%s\n",caTime);
 //  printf("lbytes=%llu\n",lBytes);
     if(lBytes>=lMaxbytes){ 
   	 lMaxbytes=lBytes;
   	      
   	    }
   	 
    	findex=atol(caTime);
    	
    	if(atol(caAv_un)==1){
	    	if(findex>=30&&findex<60){                           //������ ʱ��ֵ�������±����30
	    		findex=findex-30;
	    	}

    	}
    
    	  index=ncUtlGetServicenumById(&sGservice,lSumindex,lService);
 //  	  printf("index=%d\n",index);
//    	  printf("findex=%d\n",findex);
	   	  if(index!=-1){	
	          sGservice[index].lBytesqs[findex]=sGservice[index].lBytesqs[findex]+lBytes;         
	      }
    	  else{
 //   	  	if(lCount>0)
 //   	  	   qt_flag=1;
 // printf("findex=%d\n",findex);  	  	   
    	  	sGservice[lSumindex].lBytesqs[findex]=sGservice[lSumindex].lBytesqs[findex]+lBytes; 
    	  }
       	
  
    lCount=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    lBytes=0;
     memset(caName,0,sizeof(caName));
   
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
  }
    
    pasDbCloseCursor(psCur);
   




  //  �������ֵ����ʹ��������λ Bytes,KB,MB,GB
     lRat=lMaxbytes/10240;
 // printf("lMaxbytes=%llu\n",lRat);
			if(lRat<=1){
					unit=1;
					strcpy(caUnit,"Bytes");
					
			 }
				else if(lRat>1&&lRat<=1024){
					unit=1024;
					strcpy(caUnit,"KB");
				}
				else if(lRat>1024&&lRat<=1024*1024){
					unit=1024*1024;
					strcpy(caUnit,"MB");
				}
				else if(lRat>1024*1024){
					unit=1024*1024*1024;
					strcpy(caUnit,"GB");
				}    
				else{
					unit=1;
					strcpy(caUnit,"Bytes");
				}



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
	  			if(atol(caAv_un)==3)  {      
	  				                                   //���ڰ���ͳ�ƣ������1��ʼ�������0��ʼ
	  			   utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm+1]/unit);  
	  			  }
	  			else  			
	  			  utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%0.0f",(float)sGservice[i].lBytesqs[mm]/unit);  
	  		 
//	  		 printf("mm+1=%d,i+1=%d,bytes=%llu\n",mm+1,i+1,sGservice[i].lBytesqs[mm]) ;   
	  	 }
	  	
	  }
	   
 //�������ͼ������ 

    for(i=0;i<=lSumindex;i++){
    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);
//    	printf("gservice=%d\n",sGservice[i].lService);
    	utPltPutLoopVar(psDbHead,"sevname",i+1,sGservice[i].caGname);
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",sGservice[i].lService);
    	for(mm=0;mm<x_sum;mm++){
//    		printf("i=%d;mm=%d;service=%d;bytes=%lld\n",i,mm,sGservice[i].lService,sGservice[i].lBytesqs[mm]);
    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);
  
  	  		if(atol(caAv_un)==3)                                               //���ڰ���ͳ�ƣ������1��ʼ�������0��ʼ
	  			   utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%0.0f",(float)sGservice[i].lBytesqs[mm+1]/unit);  
	  			else 
    		    utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%0.0f",(float)sGservice[i].lBytesqs[mm]/unit);
    	
    }
    
    	  	
    
    
  }
  utPltPutVarF(psDbHead,"sum","%d",lSumindex+1);
  utPltPutVarF(psDbHead,"sum_d","%d",x_sum);
  utPltPutVar(psDbHead,"unit",caUnit);


    if(strlen(caPdf)>0){
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
   		  if(atol(caAv_un)==2){
	  			sprintf(caTemp,"%02d:00",i);	  		  
	  		}
	  		else if(atol(caAv_un)==3){	  			
	  			sprintf(caTemp,"%s/%02d",utTimFormat("%m",lStarttime),i+1);	  		 
	  		}
	  		else{
	  			sprintf(caTemp,"%s:%02d",utTimFormat("%H",lStarttime),i+atol(caSmin));	  		 
	  		}  	
   	   pChart[i].name=strdup(caTemp);
   	  
   	  pChart[i].dataset=malloc((lSumindex+2)*sizeof(float)) ;
   	  for(mm=0;mm<lSumindex+1;mm++){
   	  	if(atol(caAv_un)==3)                             //���ڰ���ͳ�������1��ʼ�������0��ʼ
   	  	  pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i+1]/unit;
   	  	else
   	  	 pChart[i].dataset[mm]=(float)sGservice[mm].lBytesqs[i]/unit;
   	  
   	  }
   	} 
   	
     
   
     
     
      rowrow=x_sum;
      column=lSumindex+1;
   
      
    sprintf(filename,"%s.PDF",caTitle); 

    sprintf(caStatdate,ncLang("1962ͳ��ʱ��:%s %s:%s"),caSdate,caShour,caSmin);

/*
   for(i=0;i<rowrow;i++){
   	for(m=0;m<lSumindex+1;m++)
   	  printf("name=%s,i=%d,m=%d,dataset=%f\n",pChart[i].name,i,m,pChart[i].dataset[m]);
   	 
   }
   for(i=0;i<lSumindex+1;i++){
   	  printf("i=%d,desc=%s\n",i,desc[i]);

   }
printf("caTitle=%s,filename=%s,rowrow=%d,column=%d,caUnit=%s\n",caTitle,filename,rowrow,column,caUnit);
*/
sprintf(caTemp,ncLang("1942��λ(%s)"),caUnit);
   ncWebShowServiceBytime_PDF(iFd,pChart,desc,rowrow,column,caTitle,caTemp,caStatdate,filename);


//printf("end\n"); 
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






    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_mainfirst_servicebytimedet.htm");
    
    return 0;
}


/* ת��ʱ��  
���ϰ�ʱ��ת����SQL���
field Ϊʱ���ֶ���,����ʱ���ֶ�Ϊlong��
����ֵSQL���

 */
char *ncUtlCvtWorkTime2Sql(char *field)
{
	  static char caSql[2048];
	  char caTemp[2048];
	   pasDbCursor *psCur;
	   long iSum;
    char *p;
    long lTimetype;
    long lLower,lUpper,lStarttime,lStoptime;
    long iReturn;
    
    strcpy(caSql,"\0");
 
 //����ʱ��     
    psCur=pasDbOpenSql("select timetype,llower,lupper,starttime,stoptime from ncworktime where flags=0 order by timetype,llower ",0);
    if(psCur==NULL){
        return NULL;
    }
    lTimetype=0;
    lLower=0;
    lUpper=0;
    lStarttime=0;
    lStoptime=0;
    iSum=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
                                   UT_TYPE_LONG,4,&lLower,
                                   UT_TYPE_LONG,4,&lUpper,
                                   UT_TYPE_LONG,4,&lStarttime,
                                   UT_TYPE_LONG,4,&lStoptime);
                                   
    while(iReturn==0){

    	if(lTimetype==1){                     //ĳ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," or (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql),"(");
    		}
    		
    		sprintf(caSql+strlen(caSql)," from_unixtime(%s,\'%%Y%%m%%d\')>='%d' and from_unixtime(%s,\'%%Y%%m%%d\')<='%d' ",field,lLower,field,lUpper);
    		sprintf(caSql+strlen(caSql)," and from_unixtime(%s,\'%%H%%i%%s\')>='%06d' and from_unixtime(%s,\'%%H%%i%%s\')<='%06d' ",field,lStarttime,field,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==2){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," or (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql),"(");
    		}
    		
    		sprintf(caSql+strlen(caSql)," from_unixtime(%s,\'%%w\')>='%d' and from_unixtime(%s,\'%%w\')<='%d' ",field,lLower,field,lUpper);
    		sprintf(caSql+strlen(caSql)," and from_unixtime(%s,\'%%H%%i%%s\')>='%06d' and from_unixtime(%s,\'%%H%%i%%s\')<='%06d' ",field,lStarttime,field,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==3){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," or (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql),"(");
    		}
    		
    		sprintf(caSql+strlen(caSql)," from_unixtime(%s,\'%%m\')>='%02d' and from_unixtime(%s,\'%%m\')<='%02d' ",field,lLower,field,lUpper);
    		sprintf(caSql+strlen(caSql)," and from_unixtime(%s,\'%%H%%i%%s\')>='%06d' and from_unixtime(%s,\'%%H%%i%%s\')<='%06d' ",field,lStarttime,field,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==4){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," or (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql),"(");
    		}
    		
    		sprintf(caSql+strlen(caSql)," from_unixtime(%s,\'%%m%%d\')>='%04d' and from_unixtime(%s,\'%%m%%d\')<='%04d' ",field,lLower,field,lUpper);
    		sprintf(caSql+strlen(caSql)," and from_unixtime(%s,\'%%H%%i%%s\')>='%06d' and from_unixtime(%s,\'%%H%%i%%s\')<='%06d' ",field,lStarttime,field,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	
    	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
                                   UT_TYPE_LONG,4,&lLower,
                                   UT_TYPE_LONG,4,&lUpper,
                                   UT_TYPE_LONG,4,&lStarttime,
                                   UT_TYPE_LONG,4,&lStoptime);
    }
    pasDbCloseCursor(psCur);
    
    if(iSum>0){
    	sprintf(caTemp,"(%s)",caSql);
    	strcpy(caSql,caTemp);
    }
    
    
    
    
     //�ǹ���ʱ��     
    psCur=pasDbOpenSql("select timetype,llower,lupper,starttime,stoptime from ncworktime where flags=1 order by timetype,llower ",0);
    if(psCur==NULL){
        return NULL;
    }
    lTimetype=0;
    lLower=0;
    lUpper=0;
    lStarttime=0;
    lStoptime=0;

    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
                                   UT_TYPE_LONG,4,&lLower,
                                   UT_TYPE_LONG,4,&lUpper,
                                   UT_TYPE_LONG,4,&lStarttime,
                                   UT_TYPE_LONG,4,&lStoptime);
                                   
    while(iReturn==0){

    	if(lTimetype==1){                     //ĳ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," and not (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql)," not (");
    		}
    		
    		sprintf(caSql+strlen(caSql)," from_unixtime(%s,\'%%Y%%m%%d\')>='%d' and from_unixtime(%s,\'%%Y%%m%%d\')<='%d' ",field,lLower,field,lUpper);
    		sprintf(caSql+strlen(caSql)," and from_unixtime(%s,\'%%H%%i%%s\')>='%06d' and from_unixtime(%s,\'%%H%%i%%s\')<='%06d' ",field,lStarttime,field,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==2){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," and not (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql)," not (");
    		}
    		
    		sprintf(caSql+strlen(caSql)," from_unixtime(%s,\'%%w\')>='%d' and from_unixtime(%s,\'%%w\')<='%d' ",field,lLower,field,lUpper);
    		sprintf(caSql+strlen(caSql)," and from_unixtime(%s,\'%%H%%i%%s\')>='%06d' and from_unixtime(%s,\'%%H%%i%%s\')<='%06d' ",field,lStarttime,field,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==3){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," and not (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql),"not (");
    		}
    		
    		sprintf(caSql+strlen(caSql)," from_unixtime(%s,\'%%m\')>='%02d' and from_unixtime(%s,\'%%m\')<='%02d' ",field,lLower,field,lUpper);
    		sprintf(caSql+strlen(caSql)," and from_unixtime(%s,\'%%H%%i%%s\')>='%06d' and from_unixtime(%s,\'%%H%%i%%s\')<='%06d' ",field,lStarttime,field,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==4){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," and not (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql)," not (");
    		}
    		
    		sprintf(caSql+strlen(caSql)," from_unixtime(%s,\'%%m%%d\')>='%04d' and from_unixtime(%s,\'%%m%%d\')<='%04d' ",field,lLower,field,lUpper);
    		sprintf(caSql+strlen(caSql)," and from_unixtime(%s,\'%%H%%i%%s\')>='%06d' and from_unixtime(%s,\'%%H%%i%%s\')<='%06d' ",field,lStarttime,field,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	
    	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
                                   UT_TYPE_LONG,4,&lLower,
                                   UT_TYPE_LONG,4,&lUpper,
                                   UT_TYPE_LONG,4,&lStarttime,
                                   UT_TYPE_LONG,4,&lStoptime);
    }
    pasDbCloseCursor(psCur);
    
    
    
    
    
    return caSql;
    
 
}




/* ת��ʱ��  
���ϰ�ʱ��ת����SQL���
field1 Ϊ�����ֶ��� yyyy/mm/dd,field2 ΪСʱʱ���
����ֵSQL���

 */
char *ncUtlCvtWorkTime2Sql_s(char *field1,char *field2)
{
	  static char caSql[2048];
	  char caTemp[2048];
	   pasDbCursor *psCur;
	   long iSum;
    char *p;
    long lTimetype;
    long lLower,lUpper,lStarttime,lStoptime;
    long iReturn;
    strcpy(caSql,"\0");
 
 //����ʱ��     
    psCur=pasDbOpenSql("select timetype,llower,lupper,starttime,stoptime from ncworktime where flags=0 order by timetype,llower ",0);
    if(psCur==NULL){
        return NULL;
    }
    lTimetype=0;
    lLower=0;
    lUpper=0;
    lStarttime=0;
    lStoptime=0;
    iSum=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
                                   UT_TYPE_LONG,4,&lLower,
                                   UT_TYPE_LONG,4,&lUpper,
                                   UT_TYPE_LONG,4,&lStarttime,
                                   UT_TYPE_LONG,4,&lStoptime);
                                   
    while(iReturn==0){

    	if(lTimetype==1){                     //ĳ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," or (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql),"(");
    		}
    		
    		sprintf(caSql+strlen(caSql)," replace(%s,\"/\",\"\")>='%d' and replace(%s,\"/\",\"\")<='%d' ",field1,lLower,field1,lUpper);
    		sprintf(caSql+strlen(caSql)," and %s>='%06d' and %s<='%06d' ",field2,lStarttime,field2,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==2){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," or (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql),"(");
    		}
    		
    		sprintf(caSql+strlen(caSql)," dayofweek(%s)>=%d and dayofweek(%s)<=%d ",field1,lLower+1,field1,lUpper+1);
    		sprintf(caSql+strlen(caSql)," and %s>='%06d' and %s<='%06d' ",field2,lStarttime,field2,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==3){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," or (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql),"(");
    		}
    		
    		sprintf(caSql+strlen(caSql)," substr(%s,6,2)>='%02d' and substr(%s,6,2)<='%02d' ",field1,lLower,field1,lUpper);
    		sprintf(caSql+strlen(caSql)," and %s>='%06d' and %s<='%06d' ",field2,lStarttime,field2,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==4){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," or (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql),"(");
    		}
    		
    		sprintf(caSql+strlen(caSql)," substr(replace(%s,\"/\",\"\"),5,4)>='%04d' and substr(replace(%s,\"/\",\"\"),5,4)<='%04d' ",field1,lLower,field1,lUpper);
    		sprintf(caSql+strlen(caSql)," and %s>='%06d' and %s<='%06d' ",field2,lStarttime,field2,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	
    	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
                                   UT_TYPE_LONG,4,&lLower,
                                   UT_TYPE_LONG,4,&lUpper,
                                   UT_TYPE_LONG,4,&lStarttime,
                                   UT_TYPE_LONG,4,&lStoptime);
    }
    pasDbCloseCursor(psCur);
    
    if(iSum>0){
    	sprintf(caTemp,"(%s)",caSql);
    	strcpy(caSql,caTemp);
    }
    
    
    
    
     //�ǹ���ʱ��     
    psCur=pasDbOpenSql("select timetype,llower,lupper,starttime,stoptime from ncworktime where flags=1 order by timetype,llower ",0);
    if(psCur==NULL){
        return NULL;
    }
    lTimetype=0;
    lLower=0;
    lUpper=0;
    lStarttime=0;
    lStoptime=0;

    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
                                   UT_TYPE_LONG,4,&lLower,
                                   UT_TYPE_LONG,4,&lUpper,
                                   UT_TYPE_LONG,4,&lStarttime,
                                   UT_TYPE_LONG,4,&lStoptime);
                                   
    while(iReturn==0){

    	if(lTimetype==1){                     //ĳ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," and not (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql)," not (");
    		}
    		
    		sprintf(caSql+strlen(caSql)," replace(%s,\"/\",\"\")>='%d' and replace(%s,\"/\",\"\")<='%d' ",field1,lLower,field1,lUpper);
    		sprintf(caSql+strlen(caSql)," and %s>='%06d' and %s<='%06d' ",field2,lStarttime,field2,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==2){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," and not (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql)," not (");
    		}
    		
    		sprintf(caSql+strlen(caSql)," dayofweek(%s)>'%d and dayofweek(%s)<='%d' ",field1,lLower+1,field1,lUpper+1);
    		sprintf(caSql+strlen(caSql)," and %s>='%06d' and %s<='%06d' ",field2,lStarttime,field2,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==3){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," and not (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql),"not (");
    		}
    		
    		sprintf(caSql+strlen(caSql)," substr(%s,6,2)>='%02d' and substr(%s,6,2)<='%02d' ",field1,lLower,field1,lUpper);
    		sprintf(caSql+strlen(caSql)," and %s>='%06d' and %s<='%06d' ",field2,lStarttime,field2,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	else if(lTimetype==4){                //ÿ��
    		 if(iSum>0){
    				sprintf(caSql+strlen(caSql)," and not (");
    			}
    		else{
    			sprintf(caSql+strlen(caSql)," not (");
    		}
    		
    		sprintf(caSql+strlen(caSql)," substr(replace(%s,\"/\",\"\"),5,4)>='%04d' and substr(replace(%s,\"/\",\"\"),5,4)<='%04d' ",field1,lLower,field1,lUpper);
    		sprintf(caSql+strlen(caSql)," and %s>='%06d' and %s<='%06d' ",field2,lStarttime,field2,lStoptime);
    		sprintf(caSql+strlen(caSql)," )");
    		iSum++;
    	}
    	
    	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTimetype,
                                   UT_TYPE_LONG,4,&lLower,
                                   UT_TYPE_LONG,4,&lUpper,
                                   UT_TYPE_LONG,4,&lStarttime,
                                   UT_TYPE_LONG,4,&lStoptime);
    }
    pasDbCloseCursor(psCur);
    
    
    
    
    
    return caSql;
    
 
}




/* ������վ��������ͼ */
int ncWebGenWebTrac(utShmHead *psShmHead)
{
    char caPath[128];
   
    
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
    long lCount;
    long lMin;                              
    char caSdate_all[20];                                    
    int qt_flag=0;
    long lSj=0;
    
    char caPdf[12];
 
    int rowrow;                     
    int column; 
    char caTitle[128];
    char filename[128];
    char caStatdate[128];

    char **desc;
    long lSt,lEt;
  memset(caSmin,0,sizeof(caSmin));  
   													 		                             
  psDbHead = utPltInitDb(); 
  
  
  	
 
//������ͳ��(nchttplog)
	    x_sum=15;
	    y_sum=15;
	 
      //����ʱ��
  
   lTime=time(0);
   
   //�������ݿ�洢��ʽ��ͳ��ʱ��ȡ���ݿ���ʱ������ʱ��
  	if(strcasecmp(utComGetVar_sd(psShmHead,"SaveHttpLog","No"),"File") != 0){ 
	   pasDbOneRecord("select max(vtime) from nchttplog",0,UT_TYPE_LONG,4,&lTime);
	   if(lTime>time(0)){
	     lTime=time(0);
	    }
    }
//		    lTime=time(0);
		    strcpy(caSdate,utTimFormat("%Y/%m/%d %H",lTime));
		    strcpy(caSmin,utTimFormat("%M",lTime));
	//	    printf("caSmin=%s\n",caSmin);
		    lMin=(long)(atol(caSmin)/15)*15;
//		    printf("lMin=%d\n",lMin);
		    sprintf(caSdate_all,"%s:%2d",caSdate,lMin);
		    lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);

		   lStarttime=lEndtime-900;
		 
      
		  



    utPltPutVarF(psDbHead,"stime1","%d",lStarttime);
    utPltPutVarF(psDbHead,"etime1","%d",lEndtime);
    
    strcpy(caSdate,utTimFormat("%Y/%m/%d %H:%M",lStarttime));
    strcpy(caEdate,utTimFormat("%Y/%m/%d %H:%M",lEndtime));



    

    utPltPutVar(psDbHead,"tjsj",caSdate);
    utPltPutVarF(psDbHead,"stime","%d",lEndtime);
  
 //��ȡͳ��ʱ���������������ַ��𣬽���Щ������ͼ�����������ķ���鵽���������
 
 //������ʱ�� Ŀ���Ƿ�ֹ���������󽫱�����
 iReturn=pasDbExecSql("drop temporary table if exists httplog_temp",0);
 sprintf(caTemp,"create temporary table httplog_tmp(tid int,vtime int unsigned);");
 iReturn=pasDbExecSql(caTemp,0);
 
 
if(iReturn!=0) return 0;

	 if(strcasecmp(utComGetVar_sd(psShmHead,"SaveHttpLog","No"),"File") == 0){
		   ncInsertHttplogFromFile(psShmHead);
	  }
	 else{
	
			for(i=0;i<90;i++){
			lSt=lStarttime+i*10;
			lEt=lSt+10;
			sprintf(caTemp,"insert into httplog_tmp (tid,vtime) select tid,vtime from nchttplog where vtime>=%d and vtime<%d ;",lSt,lEt);
			pasDbExecSql(caTemp,0);
			usleep(100);
		  }
    }
 
 
    sprintf(caTemp,"select count(*) aa,tid,name from httplog_tmp,ncwebclass where tid=id and tid!=0  group by tid order by aa desc limit 0,10 ");		    

     psCur = pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
       utPltFreeDb(psDbHead);
       return -1;
     }
     lBytes=0;
     lService=0;
     memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
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
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                   UT_TYPE_LONG,4,&lService,
                                   UT_TYPE_STRING,30,caName);
    }
    
    pasDbCloseCursor(psCur);
   
    
    //���һ������������������
  
    	  sGservice[lSumindex].lService=99999;
    		sGservice[lSumindex].lGroupid=0;
    		sGservice[lSumindex].lBytes=0;
    		strcpy(sGservice[lSumindex].caGname,ncLang("0431����"));
        for(i=0;i<32;i++){
    	    sGservice[lSumindex].lBytesqs[i]=0;
    	    
        }
        

  //��serviceID����  Ϊ������ַ�������׼��		
    		qsort(sGservice,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId2); 
    	

       sprintf(caTemp,"select count(*) aa,from_unixtime(vtime,'%ci') bb,tid from httplog_tmp  group by bb,tid ",'%',lStarttime,lEndtime);

 
 
	  
//   printf("caTemp=%s\n",caTemp);
   psCur = pasDbOpenSql(caTemp,0);
   if(psCur == NULL) {
       utPltFreeDb(psDbHead);
       return -1;
        
    }
    lCount=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
                                  
    iNum=0;
    while((iReturn == 0)||(iReturn==1405)) {
    	
   
  
   	 
    	findex=atol(caTime);
    	if(findex>=15&&findex<30){                           //������ ʱ��ֵ�������±����30
    		findex=findex-15;
    	}
    	else if(findex>=30&&findex<45){
    		findex=findex-30;
    	}
    	else if(findex>=45){
    		findex=findex-45;
    	}
    	
    	  index=ncUtlGetServicenumById(&sGservice,lSumindex,lService);
 //  	  printf("index=%d\n",index);
//    	  printf("findex=%d\n",findex);
	   	  if(index!=-1){	
	          sGservice[index].lBytesqs[findex]=sGservice[index].lBytesqs[findex]+lCount;         
	      }
    	  else{
 //   	  	if(lCount>0)
 //   	  	   qt_flag=1;
    	  	   
    	  	sGservice[lSumindex].lBytesqs[findex]=sGservice[lSumindex].lBytesqs[findex]+lCount; 
    	  }
       	
  
    lCount=0;
    lService=0;
    memset(caTime,0,sizeof(caTime));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lCount,
                                   UT_TYPE_STRING,8,caTime,
                                   UT_TYPE_LONG,4,&lService);
    }
    
    pasDbCloseCursor(psCur);
   


//���X��̶�

	  for(mm=0;mm<x_sum;mm++){
	  	    utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
          strcpy(caSmin,utTimFormat("%M",lStarttime));  
	  			sprintf(caTemp,"%s:%02d",utTimFormat("%H",lStarttime),mm+atol(caSmin));	 
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
	  		
	  		
	  	
	  	
	  }
	   
 //�������ͼ������ 

    for(i=0;i<=lSumindex;i++){
    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);
//    	printf("gservice=%d\n",sGservice[i].lService);
    	utPltPutLoopVar(psDbHead,"sevname",i+1,sGservice[i].caGname);
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",sGservice[i].lService);
    	for(mm=0;mm<x_sum;mm++){
//    		printf("i=%d;mm=%d;service=%d;bytes=%lld\n",i,mm,sGservice[i].lService,sGservice[i].lBytesqs[mm]);
    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);
  
    		  utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%lld",sGservice[i].lBytesqs[mm]);
    	
    }
  }
  utPltPutVarF(psDbHead,"sum","%d",lSumindex+1);
  utPltPutVarF(psDbHead,"sum_d","%d",x_sum);


   
    
   sprintf(caPath,"%s/html/nc_mainfirst_webtrac.htm",utComGetVar_sd(psShmHead,"NCHOME","/home/ncmysql/nc"));   
   utPltFileToHtmlFile("nc75/iframe_mainfirst_webclassbytime.htm",caPath,psDbHead);
   sprintf(sqlbuf,"chmod 777 %s",caPath);
   system(sqlbuf);
   
    return 0;
}


//����ȱʡ����ʱ��
int ncWebSave_Defworktime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;

    char *pConfig,*pFile;
    char *p;

    pasConfList *psConfig;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif  
  
  
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880�Զ������趨"),ncLang("0427�����ļ�����"));
        return 0;
    }
    

      p=utMsgGetVar_s(psMsgHead,"DefWorkDayTime");
      if(p)
      pasUtlModiConfig(psConfig,"DefWorkDayTime",p);
      
    pasUtlSaveConfig(pConfig,psConfig);
    pasUtlFreeConfig(psConfig);
    ncUtlReReadConFig(psShmHead);
    ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
 
    ncWebWorkTimeList(psShmHead,iFd,psMsgHead);
    return 0;
}