#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>

#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "ncdef.h"
#include "mail.h"
#define g_READ_BLOCK_SIZE 20*1024

static void rtrim(char* str)
{
    while(isspace(str[strlen(str)-1]))
            str[strlen(str)-1]='\0';
}
static void ltrim(char* str)
{
    char* buf;
    char* ptr;
    int len;

    len=strlen(str);
    buf=(char *)malloc((len+1)*sizeof(char));
    strcpy(buf,str);

    ptr=buf;
    while(isspace(*ptr))
    {
            ptr++;
    }

    strcpy(str,ptr);
    free(buf);
}
/************************************************************
删除字符串左右侧空格
************************************************************/
 void trim(char* str)
{
    rtrim(str);
    ltrim(str);
}

  struct iplog_s{
  	char dispname[32];
  	long userid;
  	char sip[16];
  	char dip[16];
  	long port;
  	long sport;
  	char prot[8];
  	char service[32];
  	char ubyte[20];
  	char dbyte[20];
  	char sdate[24];
  	long contime;
  };

  struct httplog_s{
  	char dispname[32];
  	long userid;
  	char sip[16];
  	char dip[16];
  	long port;
  	char host[68];
  	char url[130];
  	char byte[20];
  	char caType[40];
  	char title[130];
  	char sdate[24];
  };

//排序  按时间排序
static int ncUtlSortIplog(char *p1,char *p2)
{
    int i;
    long long l;
   
    struct httplog_s psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct iplog_s));
    memcpy(&psC2,p2,sizeof(struct iplog_s));    
    l=strcmp(psC2.sdate,psC1.sdate);
    if(l>0) return -1;
    if(l<0) return 1;
    if(l==0) return 0;
  
}

static int ncUtlSortHttplog(char *p1,char *p2)
{
    int i;
    long long l;
   
    struct iplog_s psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct iplog_s));
    memcpy(&psC2,p2,sizeof(struct iplog_s));    
    l=strcmp(psC2.sdate,psC1.sdate);
    if(l>0) return -1;
    if(l<0) return 1;
    if(l==0) return 0;
  
}


/* 从文件中查询IPLOG */
int ncShowIpFromFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  char caTitlename[64];
	  int group_flag;
	  ncService *psService;
	  unsigned long lId_s;
	  char caName_s[32];
    char caWhere[512];
    char caStime0[6];
    char caEtime[6];
    char caStemptime[17];
    char caEtemptime[17];
    char caUserid[17];
    char byt_rflag[2];
    long lstime=0;
    long letime=0;
    char caByt[10];
    char byt_in[2];
    char cadel[17];
    unsigned long lByt=0;
    char caTotPg[16],caCurPg[16];
    char caUsername[32],caSdate[32];
    char caTemp[1024],caGroupid[12],caMac[20];
    char caEdate[20],caOrder[12],caOrderby[12];
    char caDdd[12],caUnit[12],caStatus[12],caDtype[12],caBef[12];
    char caSip[16],caDip[16],caSport[10],caDport[10];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0;
    unsigned long lTime,l,lTime1,lSip,lDip;
    int i,iNum;
    utPltDbHead *psDbHead;
   	int iReturn,iPos;
   	char caDispname[32],caProt[12],caService[12];
   	char caStime[32];
   	char caIdfyno[20];
    char caIp[20];
   	char ids[256];
   	int i2,lUserid2;
   	char caUsername_match[8],caIdfyno_match[8],caIp_match[8],caMac_match[8],caDip_match[8];
   	float lRat;
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount,lStatus1;
	int     lSumUser;
	char sqlbuf[2048]="";
	unsigned long lStatrow,lMintime,lMaxtime,lSum,lStarttime,lEndtime,lMaxin,lMinin,lIn,lVtime;
   	unsigned long lUbytes,lDbytes,lConntime,lService,lPort,lType;
    char caWhereStr[10];

 	pasDbCursor *psCur;
 	char caDelwhere[512];
    strcpy(caDelwhere,"\0");
 	strcpy(caWhere,"\0");

 	lSum=0;
 	lMintime=0;
 	lMaxtime=0;

 	long lStart;
 	char caFile_date[128];
 	long filelen[14]={11,16,11,9,6,16,16,6,3,10,11,20};
  int  iBlockSize;	// 每块的大小；
	int  iRead;			// 每次实际读取的行数；
	char *pLineHead;		// 每块的头指针；
 	FILE *flog;
 	long g_iLineLength=136;
 	char caFile[256];
 	long iBlockLines;

  struct iplog_s *pIplog; 
  long lRow,flag_cont;
  ncUserCont *psUser;
  char t_dispname[32];
   long w_flag=1;
   long cong_flag,lFize;
   long lt_time,lMum;
   char t_sport[8];
   char t_mac[24],caProt_t[20],caService_t[20];
   char caFile_rz[20];
   char caExport[20];
  char t_id[20],t_sdate[24],t_sip[20],t_dip[20],t_port[16],t_prot[16],t_ubyte[20],t_dbyte[20],t_service[24];
 
  lSum=0;
  lRow=1000;
  long iField=0;
  char *p;
  char caBuf[2048];
  char t_conntime[20];
  pIplog=(struct iplog_s*)malloc(60000*sizeof(struct iplog_s));
  if(pIplog==NULL){
  	return 0;
  }
  memset(pIplog,0,sizeof(struct iplog_s)*lRow);
  lMum=0;
  
	iReturn = utMsgGetSomeNVar(psMsgHead,23,
					        "mac",        UT_TYPE_STRING,18, caMac,
					        "userid",     UT_TYPE_STRING,14,caUserid,
	                "dispname",   UT_TYPE_STRING, 31, caDispname,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "sip",        UT_TYPE_STRING, 31, caSip,
	                "dip",        UT_TYPE_STRING, 31, caDip,
	                "dport",      UT_TYPE_STRING, 15, caDport,
	                "prot",       UT_TYPE_STRING, 11, caProt,
	                "service",       UT_TYPE_STRING, 11, caService,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "bef",        UT_TYPE_STRING, 11, caBef,
	                "dtype",      UT_TYPE_STRING, 11,caDtype,
	                "stime",	  UT_TYPE_STRING, 5,caStime0,
	                "etime",      UT_TYPE_STRING, 5,caEtime,
	                "byt_rflag",  UT_TYPE_STRING, 1,byt_rflag,
	                "byt_in",     UT_TYPE_STRING, 1,byt_in,
	                "byt",        UT_TYPE_STRING, 9,caByt,
   	              "CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					        "TotPg",      UT_TYPE_STRING, 12,  caTotPg,
					        "export",     UT_TYPE_STRING,  10,caExport,
					        "sport",      UT_TYPE_STRING,6,caSport);
	
	//	utMsgPrintMsg(psMsgHead);			        
	
		if(utStrIsSpaces(caExport)) {
        lRow = 1000;
    }
    else {    
        lRow=59999;
    }
	
	utStrDelSpaces(caMac);
	utStrDelSpaces(caUserid);
	utStrDelSpaces(caDispname);
	utStrDelSpaces(caGroupid);
	utStrDelSpaces(caSip);
	utStrDelSpaces(caDip);
	
	utStrDelSpaces(caDport);
	utStrDelSpaces(caProt);
	utStrDelSpaces(caService);
	utStrDelSpaces(caIdfyno);
	utStrDelSpaces(caSdate);
	utStrDelSpaces(caEdate);
	
	utStrDelSpaces(caEtime);
	utStrDelSpaces(caStime0);
	
	
	flag_cont=1;
	flag_cont=flag_cont&&(strlen(caMac)==0)&&(strlen(caUserid)==0)&&(strlen(caDispname)==0)&&(strlen(caGroupid)==0)&&(strlen(caSip)==0)&&(strlen(caDip)==0)&&(strlen(caProt)==0);
	flag_cont=flag_cont&&(atol(caDport)==0)&&(strlen(caService)==0)&&(strlen(caIdfyno)==0);

	
 if(atol(caUserid)>0)
    strcpy(caDispname,ncUtlBufGetUsername(psShmHead,atol(caUserid)));  	              
                 	              
	strcpy(caStemptime,"\0");
	strcpy(caEtemptime,"\0");
	lByt=atol(caByt);
	lstime=0;
	letime=0;
	if(atol(caProt)==6){ 
	   strcpy(caProt_t,"TCP");
	}
	else if(atol(caProt)==17){
	 	strcpy(caProt_t,"UDP");
	}
	else{
		strcpy(caProt_t,"");
	}
	if(utStrIsSpaces(caSdate)){
		strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
	
	}
	
	
	if(!utStrIsSpaces(caSdate)){
		if(utStrIsSpaces(caStime0)) {
			strcpy(caStime0,"00:00");
		}
		sprintf(caStemptime,"%s %s",caSdate,caStime0);
		lstime=utTimStrToLong("%Y/%m/%d %H:%M",caStemptime);
	}
	if(utStrIsSpaces(caEdate)){
		strcpy(caEdate,caSdate);
	}
	
	if(!utStrIsSpaces(caEdate)){
		if(utStrIsSpaces(caEtime)) {
			strcpy(caEtime,"23:59");
		}
		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		letime=utTimStrToLong("%Y/%m/%d %H:%M",caEtemptime);
	}	
	lStart=letime;
	lCurPg=atol(caCurPg);
	
	lRowNum=32;
	
	if(lCurPg==0) lCurPg=1;
	 lStartRec = (lCurPg - 1) * lRowNum;
   if(lStartRec <= 0) lStartRec=0;
	 lCount=0;
	
   for(lStart=lstime;lStart<letime;lStart=lStart+3600){
   	strcpy(caFile_rz,utTimFormat("%Y-%m-%d",lStart));
   	strcpy(caFile_date,utTimFormat("%Y-%m-%d-%H",lStart));   	
   	sprintf(caFile,"/home/ncmysql/nc/log/iplog/%s/iplog-%s.log",caFile_rz,caFile_date);
//  	  	printf("caFile_date=%s\n",caFile);
   
    if(flag_cont==1){
    	lFize=utFileSize(caFile);

    	if(lFize-150>0){
    		lCount=lCount+(lFize-146)/g_iLineLength;
    		
    	}
    }
     
     if(flag_cont==1&&lSum>lRow) continue;
 
 
   	flog=fopen(caFile,"rb");
   	if(flog==NULL) continue;
   	
   	
   		char *p1;
   		char p_temp[256];
   		long i_t=0;
   		iField=0;
  		
  //处理首行
        memset(caBuf,0,sizeof(caBuf));
   		  p1=fgets(caBuf,256,flog);
   //		  printf("caBuf=%s\n",caBuf);
   		  if(p1&&caBuf[0]=='#'){
   		  	for(i=0;i<256;i++){
   		  		p_temp[i_t]=caBuf[i];
   		  		if(caBuf[i]==';'){
   		  			p_temp[i_t]='\0';
   		  			i_t=0;
				     	p=strstr(p_temp,"n,");
				     	if(p){
				     		filelen[iField]=atol(p+2);
				     		iField++;
				     	}
				     	else{
				     		p=strstr(p_temp,"s,");
				     		if(p){
				     					filelen[iField]=atol(p+2);
				     		      iField++;
				     		}
				     	}
   		  		}
   		  	
   		  		i_t++;
   		  	}
   		  }
		   	     		
	   		if(iField>0){
	   			g_iLineLength=0;
	   			for(i=0;i<iField;i++){
	   				g_iLineLength=g_iLineLength+filelen[i];
	   			}
	   			g_iLineLength=g_iLineLength+1;
	   		 }
 
 		
 		while(fgets(caBuf,256,flog)!=NULL&&lSum<lRow){
 			if(strlen(caBuf)<g_iLineLength-1)
 			   continue;
 			
 			pLineHead=caBuf;
// printf("pLineHead=%s\n",pLineHead);			
 				
 					memset(t_id,0,sizeof(t_id));
      		memset(t_sdate,0,sizeof(t_sdate));
      		memset(t_sip,0,sizeof(t_sip));
      		memset(t_dip,0,sizeof(t_dip));
      		memset(t_port,0,sizeof(t_port));
      		memset(t_prot,0,sizeof(t_prot));
      		memset(t_ubyte,0,sizeof(t_ubyte));
      		memset(t_dbyte,0,sizeof(t_dbyte));
      		memset(t_service,0,sizeof(t_service));
          memset(t_dispname,0,sizeof(t_dispname));
          memset(t_sport,0,sizeof(t_sport));
          memset(t_conntime,0,sizeof(t_conntime));
          
      		memcpy(t_id,pLineHead,filelen[0]);

      		memcpy(t_sdate,pLineHead+filelen[0]+filelen[1],filelen[2]+filelen[3]);
      		
      		memcpy(t_conntime,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3],filelen[4]);
      		memcpy(t_sip,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4],filelen[5]);
      		memcpy(t_dip,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5],filelen[6]);
      		memcpy(t_port,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6],filelen[7]);
      		memcpy(t_prot,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6]+filelen[7],filelen[8]);
      		memcpy(t_ubyte,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6]+filelen[7]+filelen[8],filelen[9]);
      		memcpy(t_dbyte,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6]+filelen[7]+filelen[8]+filelen[9],filelen[10]);
      	  memcpy(t_service,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6]+filelen[7]+filelen[8]+filelen[9]+filelen[10],filelen[11]);
      	  
    if(strlen(caBuf)>140){
    	memcpy(t_sport,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6]+filelen[7]+filelen[8]+filelen[9]+filelen[10]+filelen[11],filelen[12]);
    }  	  
      	  
      	  utStrDelSpaces(t_sdate);
          utStrDelSpaces(t_sip);
          utStrDelSpaces(t_dip);
          trim(t_port);
          utStrDelSpaces(t_ubyte);
      		utStrDelSpaces(t_dbyte);
      		utStrDelSpaces(t_conntime);
      		
          trim(t_service);
      		utStrDelSpaces(t_prot);
      		lt_time=utTimStrToLong("%Y/%m/%d %H:%M:%S",t_sdate);
      	  w_flag=1;
      	
      	  //对流量进行范围进行查询    		
		      lByt=atol(caByt);
			    if(lByt>0){
			    if(atoi(byt_rflag)==1){
			    	if(atoi(byt_in)==1){
			    		w_flag=w_flag&&(atol(t_ubyte)<=lByt);
			    	
			    	}
			    	else{
			    		w_flag=w_flag&&(atol(t_ubyte)>=lByt);
			        }
			    }
			    else if(atoi(byt_rflag)==2){
		    	    if(atoi(byt_in)==1){
			    	  w_flag=w_flag&&(atol(t_dbyte)<=lByt);
			    	}
			    	else{
			    		
			    		w_flag=w_flag&&(atol(t_dbyte)>=lByt);
			        }
			    }
		       else {
		           	if(atoi(byt_in)==1){
		           		w_flag=w_flag&&(atol(t_ubyte)+atol(t_dbyte)<=lByt);
			    	
			    	}
			    	else{
			    		  w_flag=w_flag&&(atol(t_ubyte)+atol(t_dbyte)>=lByt);
			        }
			    }
			 }
      	
      	  w_flag=w_flag&&(strlen(caUserid)==0||atol(caUserid)==atol(t_id));
      		w_flag=w_flag&&(strlen(caSip)==0||strcmp(caSip,t_sip)==0);
      		w_flag=w_flag&&(strlen(caDip)==0||strcmp(caDip,t_dip)==0);
      		w_flag=w_flag&&(atol(caDport)==0||atol(caDport)==atol(t_port));
      		w_flag=w_flag&&(lt_time>=lstime&&lt_time<=letime);
      		w_flag=w_flag&&(strlen(caProt_t)==0||strcmp(caProt_t,t_prot)==0);
      		
      		w_flag=w_flag&&(atol(caSport)==0||atol(caSport)==atol(t_sport));
      		
      		strcpy(caService_t,ncUtlGetServiceName(psShmHead,atol(caService),"\0"));

      		w_flag=w_flag&&(strlen(caService)==0||strcmp(caService_t,t_service)==0);
      	
      	  psUser = ncUtlGetUserCont(psShmHead,atol(t_id));
      	
      	 if(psUser){
	        	strcpy(t_dispname,psUser->dispname);
	        	strcpy(t_mac,ncCvtMac(psUser->mac));
	        	w_flag=w_flag&&(strlen(caMac)==0||strstr(t_mac,caMac));
	        	w_flag=w_flag&&(strlen(caIdfyno)==0||strstr(psUser->cardid,caIdfyno));
	        	w_flag=w_flag&&(strlen(caDispname)==0||strstr(psUser->dispname,caDispname)||strstr(psUser->username,caDispname));

	        	w_flag=w_flag&&(strlen(caGroupid)==0||atol(caGroupid)==psUser->groupid);
	        }
	        else{
	        	w_flag=0;
	        }
	        if(lSum<lRow&&w_flag==1){
	      		if(lMum>=lStartRec){
		      		pIplog[lSum].userid=atol(t_id);
		      		strcpy(pIplog[lSum].sip,t_sip);
		      		strcpy(pIplog[lSum].dip,t_dip);
		      		pIplog[lSum].port=atol(t_port);
		      		pIplog[lSum].sport=atol(t_sport);
		      		strcpy(pIplog[lSum].ubyte,t_ubyte);
		      		strcpy(pIplog[lSum].dbyte,t_dbyte);
		      		strcpy(pIplog[lSum].sdate,t_sdate);
		      		strcpy(pIplog[lSum].service,t_service);
		      		strcpy(pIplog[lSum].prot,t_prot);
		      		strcpy(pIplog[lSum].dispname,t_dispname);
	      		  pIplog[lSum].contime=atol(t_conntime);
	      		lSum++;
	      	 }
      	 }
	        
	        if(w_flag==1){
      			lMum++;
      		  }
	        				
 			 
 			   }
 			
 	
  	
   		fclose(flog);
   	}
   	 	
 

	if(lSum>0){
		//按时间排序
		qsort(pIplog,lSum,sizeof(struct iplog_s),ncUtlSortIplog); 
	}
   
//输出到excel中

    if(!utStrIsSpaces(caExport)){
		FILE *fp;
		char caFilename[32];
		char caFile[128];
		char caPath[128];
		    sprintf(caFilename,"ncIP%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0065IP访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,ncLang("0066IP访问明细表\n"));
        ncUtlPrintCsv(fp,11,UT_TYPE_STRING,ncLang("0711序号"),
                       UT_TYPE_STRING,ncLang("0693姓  名"),
                       UT_TYPE_STRING,ncLang("0821源地址"),
                       UT_TYPE_STRING,ncLang("0398目标地址"),
                       UT_TYPE_STRING,ncLang("0686协  议"),
                       UT_TYPE_STRING,ncLang("0237访问端口"),
                       UT_TYPE_STRING,ncLang("0253服务"),
                       UT_TYPE_STRING,ncLang("0241访问时间"),
                       UT_TYPE_STRING,ncLang("0505上行流量"),
                       UT_TYPE_STRING,ncLang("0678下行流量"));
        for(i=0;i<lSum;i++){
        	
        	        ncUtlPrintCsv(fp,10,UT_TYPE_LONG,i,
                       UT_TYPE_STRING,pIplog[i].dispname,
                       UT_TYPE_STRING,pIplog[i].sip,
                       UT_TYPE_STRING,pIplog[i].dip,
                       UT_TYPE_STRING,pIplog[i].prot,
                       UT_TYPE_LONG,pIplog[i].port,
                       UT_TYPE_STRING,pIplog[i].service,
                       UT_TYPE_STRING,pIplog[i].sdate,
                       UT_TYPE_STRING,pIplog[i].ubyte,
                       UT_TYPE_STRING,pIplog[i].dbyte);
        	
        }
       
       
       
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        free(pIplog);
        return 0;
    }
      

   
 
    
    

    /* 显示服务信息  */
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"dip",caDip);
    utPltPutVar(psDbHead,"dport",caDport);
    utPltPutVar(psDbHead,"prot",caProt);
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"bef",caBef);
    utPltPutVar(psDbHead,"dtype",caDtype);
    utPltPutVar(psDbHead,"stime",caStime0);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"byt_rflag",byt_rflag);
    utPltPutVar(psDbHead,"byt_in",byt_in);
    utPltPutVar(psDbHead,"byt",caByt);
    utPltPutVar(psDbHead,"username_match",caUsername_match);
    utPltPutVar(psDbHead,"sport",caSport);
    
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caDispname,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		      strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
    
    utPltPutVar(psDbHead,"title_name",caTitlename);
     
 
  
  
  
  for(i=0;i<32&&i<lSum;i++){
  	utPltPutLoopVarF(psDbHead,"userid",i+1,"%d",pIplog[i].userid);
  	utPltPutLoopVar(psDbHead,"sip",i+1,pIplog[i].sip);
  	utPltPutLoopVar(psDbHead,"dip",i+1,pIplog[i].dip);
  	utPltPutLoopVar(psDbHead,"prot",i+1,pIplog[i].prot);
  	utPltPutLoopVarF(psDbHead,"dport",i+1,"%d",pIplog[i].port);
  	utPltPutLoopVar(psDbHead,"service",i+1,pIplog[i].service);
  	utPltPutLoopVar(psDbHead,"sdate",i+1,pIplog[i].sdate);
  	utPltPutLoopVar(psDbHead,"ubytes",i+1,pIplog[i].ubyte);
  	utPltPutLoopVar(psDbHead,"dbytes",i+1,pIplog[i].dbyte);
  	utPltPutLoopVar(psDbHead,"dispname",i+1,pIplog[i].dispname);
  	utPltPutLoopVarF(psDbHead,"sport",i+1,"%d",pIplog[i].sport);
  	utPltPutLoopVarF(psDbHead,"contime",i+1,"%d",pIplog[i].contime);
  }
  
//    while(iNum < lRowNum + 1) {
 //       i++;
 //       iNum++;
//        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
//    }
  
    
    for(i=0;i<ncPatGetSumService();i++) {
           utPltPutLoopVarF(psDbHead,"id_s",i+1,"%lu",ncUtlGetServiceIdByNum(psShmHead,i));
           utPltPutLoopVar(psDbHead,"name_s",i+1,ncUtlGetServiceByNum(psShmHead,i));
    }
        
//输出网络服务大类
long lCgid;
char caName[32];
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
 
        
        
     free(pIplog);  
        
      if(flag_cont==1&&lMum>=1000)
       lMum=lCount;
        
     lTotPg = (lMum ) /lRowNum + 1;
    utPltPutVarF(psDbHead,"TotRec", "%lu", lMum);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    
    if(strcasecmp(utComGetVar_sd(psShmHead,"SaveSport","No"),"Yes") == 0){
    	    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_iplog_hsear_sport.htm");
    }
    else{
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_iplog_hsear.htm");
   }
    return 0;
}




/* 从文件中查询HTTPLOG */
int ncShowWebFromFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	  char caTitlename[64];
	  int group_flag;
	  ncService *psService;
	  unsigned long lId_s;
	  char caName_s[32];
    char caWhere[512];
    char caStime0[6];
    char caEtime[6];
    char caStemptime[17];
    char caEtemptime[17];
    char caUserid[17];
    char byt_rflag[2];
    long lstime=0;
    long letime=0;
    char caByt[10];
    char byt_in[2];
    char cadel[17];
    unsigned long lByt=0;
    char caTotPg[16],caCurPg[16];
    char caUsername[32],caSdate[32];
    char caTemp[1024],caGroupid[12],caMac[20];
    char caEdate[20],caOrder[12],caOrderby[12];
    char caDdd[12],caUnit[12],caStatus[12],caDtype[12],caBef[12];
    char caSip[16],caDip[16],caSport[10],caDport[10];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0;
    unsigned long lTime,l,lTime1,lSip,lDip;
   	char caUri[128];
   	char caHost[64];
    char caUrlid[16],caWebtitle[64];
   	char caUrlid2[16];
    int i,iNum;
    utPltDbHead *psDbHead;
   	int iReturn,iPos;
   	char caDispname[32],caProt[12],caService[12];
   	char caStime[32];
   	char caIdfyno[20];
    char caIp[20];
   	char ids[256];
   	int i2,lUserid2;
   	char caUsername_match[8],caIdfyno_match[8],caIp_match[8],caMac_match[8],caDip_match[8];
   	float lRat;
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount,lStatus1;
	int     lSumUser;
	char sqlbuf[2048]="";
	unsigned long lStatrow,lMintime,lMaxtime,lSum,lStarttime,lEndtime,lMaxin,lMinin,lIn,lVtime;
   	unsigned long lUbytes,lDbytes,lConntime,lService,lPort,lType;
    char caWhereStr[10];

 	pasDbCursor *psCur;
 	char caDelwhere[512];
    strcpy(caDelwhere,"\0");
 	strcpy(caWhere,"\0");

 	lSum=0;
 	lMintime=0;
 	lMaxtime=0;

 	long lStart;
 	char caFile_date[128];
 	long filelen[12]={11,16,11,9,6,17,64,128,104,16,8,10};
  int  iBlockSize;	// 每块的大小；
	int  iRead;			// 每次实际读取的行数；
	char *pBlock,*pLineHead;		// 每块的头指针；
 	FILE *flog;
 	long g_iLineLength=401;
 	char caFile[256];
 	long iBlockLines;

  
  struct httplog_s *pHttplog; 
  long lRow,flag_cont;
  ncUserCont *psUser;
  char t_dispname[32];
   long w_flag=1;
   long cong_flag,lFize;
   long lt_time,lMum;
   char t_mac[24],caProt_t[20],caService_t[20];
   char caFile_rz[20];
   char caExport[20];
   char caUrlName[128];
   char caUrl0[200],caUrl[200],caUrname[64];
  char t_id[20],t_sdate[28],t_sip[20],t_dip[20],t_port[16],t_host[70],t_byte[20],t_url[200],t_title[130],t_type[40];
  long iField=0;
  char *p;
    char caBuf[2048];
  lSum=0;
  lRow=1000;
  pHttplog=(struct httplog_s*)malloc(60000*sizeof(struct httplog_s));
  if(pHttplog==NULL){
  	return 0;
  }
  memset(pHttplog,0,sizeof(struct httplog_s)*lRow);
  lMum=0;
  memset(caUrname,0,sizeof(caUrname));
	iReturn = utMsgGetSomeNVar(psMsgHead,21,
                 "mac",        UT_TYPE_STRING, 18, caMac,
	                "dispname",   UT_TYPE_STRING, 31, caDispname,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "userid",     UT_TYPE_STRING,11,caUserid,
	                "sip",        UT_TYPE_STRING, 31, caSip,
	                "host",       UT_TYPE_STRING, 15, caHost,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "stime",	  UT_TYPE_STRING, 5,caStime0,
	                "etime",      UT_TYPE_STRING, 5,caEtime,
	                "byt_in",     UT_TYPE_STRING, 1,byt_in,
	                "byt",        UT_TYPE_STRING, 9,caByt,
	                "dip",        UT_TYPE_STRING,15,caDip,
	                "idfyno",     UT_TYPE_STRING,18,caIdfyno,
	                 "uri",       UT_TYPE_STRING,100,caUri,
   	               "urlid",UT_TYPE_STRING,10,caUrlid,
   	               "urlid2",UT_TYPE_STRING,10,caUrlid2,
   	               "webtitle",UT_TYPE_STRING,63,caWebtitle,
   	              "CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					        "TotPg",      UT_TYPE_STRING, 12,  caTotPg,
					        "export",     UT_TYPE_STRING,  10,caExport);
	
//		utMsgPrintMsg(psMsgHead);			        
	   if(atol(caUrlid)>0){
    	sprintf(caTemp,"select name from ncwebclass where id=%s ",caUrlid);
   
    	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caUrname);

    	
    } 
		if(utStrIsSpaces(caExport)) {
        lRow = 1000;
    }
    else {    
        lRow=59999;
    }
	
	utStrDelSpaces(caMac);
	utStrDelSpaces(caUserid);
	utStrDelSpaces(caDispname);
	utStrDelSpaces(caGroupid);
	utStrDelSpaces(caSip);
	utStrDelSpaces(caDip);
	
	utStrDelSpaces(caDport);
	utStrDelSpaces(caProt);
	utStrDelSpaces(caService);
	utStrDelSpaces(caIdfyno);
	utStrDelSpaces(caSdate);
	utStrDelSpaces(caEdate);
	
	utStrDelSpaces(caEtime);
	utStrDelSpaces(caStime0);
	
	
	flag_cont=1;
	flag_cont=flag_cont&&(strlen(caMac)==0)&&(strlen(caUserid)==0)&&(strlen(caDispname)==0)&&(strlen(caGroupid)==0)&&(strlen(caSip)==0)&&(strlen(caDip)==0)&&(strlen(caProt)==0);
	flag_cont=flag_cont&&(atol(caDport)==0)&&(strlen(caService)==0)&&(strlen(caIdfyno)==0);

	
 if(atol(caUserid)>0)
    strcpy(caDispname,ncUtlBufGetUsername(psShmHead,atol(caUserid)));  	              
                 	              
	strcpy(caStemptime,"\0");
	strcpy(caEtemptime,"\0");
	lByt=atol(caByt);
	lstime=0;
	letime=0;

	if(utStrIsSpaces(caSdate)){
		strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
	
	}
	
	
	if(!utStrIsSpaces(caSdate)){
		if(utStrIsSpaces(caStime0)) {
			strcpy(caStime0,"00:00");
		}
		sprintf(caStemptime,"%s %s",caSdate,caStime0);
		lstime=utTimStrToLong("%Y/%m/%d %H:%M",caStemptime);
	}
	if(utStrIsSpaces(caEdate)){
		strcpy(caEdate,caSdate);
	}
	
	if(!utStrIsSpaces(caEdate)){
		if(utStrIsSpaces(caEtime)) {
			strcpy(caEtime,"23:59");
		}
		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		letime=utTimStrToLong("%Y/%m/%d %H:%M",caEtemptime);
	}	
	lStart=letime;
	lCurPg=atol(caCurPg);
	
	lRowNum=32;
	
	if(lCurPg==0) lCurPg=1;
	 lStartRec = (lCurPg - 1) * lRowNum;
   if(lStartRec <= 0) lStartRec=0;
	 lCount=0;
	
   for(lStart=lstime;lStart<letime;lStart=lStart+3600){
   	strcpy(caFile_rz,utTimFormat("%Y-%m-%d",lStart));
   	strcpy(caFile_date,utTimFormat("%Y-%m-%d-%H",lStart));   	
   	sprintf(caFile,"/home/ncmysql/nc/log/httplog/%s/httplog-%s.log",caFile_rz,caFile_date);
   	
   	 	  
    if(flag_cont==1){
    	lFize=utFileSize(caFile);

    	if(lFize-143>0){
    		lCount=lCount+(lFize-143)/g_iLineLength;
    		
    	}
    }

     if(flag_cont==1&&lSum>lRow) continue;
 
   	flog=fopen(caFile,"rb");
   		if(flog==NULL) continue;

   		char *p1;
   		char p_temp[256];
   		long i_t=0;
   		iField=0;
   		
  //处理首行
        memset(caBuf,0,sizeof(caBuf));
   		  p1=fgets(caBuf,1024,flog);
   //		  printf("caBuf=%s\n",caBuf);
   		  if(p1&&caBuf[0]=='#'){
   		  	for(i=0;i<1024;i++){
   		  		p_temp[i_t]=caBuf[i];
   		  		if(caBuf[i]==';'){
   		  			p_temp[i_t]='\0';
   		  			i_t=0;
				     	p=strstr(p_temp,"n,");
				     	if(p){
				     		filelen[iField]=atol(p+2);
				     		iField++;
				     	}
				     	else{
				     		p=strstr(p_temp,"s,");
				     		if(p){
				     					filelen[iField]=atol(p+2);
				     		      iField++;
				     		}
				     	}
   		  		}
   		  	
   		  		i_t++;
   		  	}
   		  }
	   		
	   		if(iField>0){
	   			g_iLineLength=0;
	   			for(i=0;i<iField;i++){
	   				g_iLineLength=g_iLineLength+filelen[i];
	   			}
	   			g_iLineLength=g_iLineLength+1;
	   		}
	  	
   		while(fgets(caBuf,1024,flog)!=NULL&&lSum<lRow){

 			if(strlen(caBuf)<g_iLineLength-1)
 			   continue;
 			
 			pLineHead=caBuf;
   		
   
      		memset(t_id,0,sizeof(t_id));
      		memset(t_sdate,0,sizeof(t_sdate));
      		memset(t_sip,0,sizeof(t_sip));
      		memset(t_dip,0,sizeof(t_dip));
      		memset(t_port,0,sizeof(t_port));
      		memset(t_host,0,sizeof(t_host));
      		memset(t_url,0,sizeof(t_url));
      		memset(t_byte,0,sizeof(t_byte));
      		memset(t_title,0,sizeof(t_title));
          memset(t_type,0,sizeof(t_type));
   
      		memcpy(t_id,pLineHead,filelen[0]);

      		memcpy(t_sdate,pLineHead+filelen[0]+filelen[1],filelen[2]+filelen[3]);
      		memcpy(t_sip,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4],filelen[5]);
      		memcpy(t_host,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5],filelen[6]);
      		memcpy(t_url,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6],filelen[7]);
      		memcpy(t_title,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6]+filelen[7],filelen[8]);
      		memcpy(t_dip,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6]+filelen[7]+filelen[8],filelen[9]);
      		memcpy(t_port,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6]+filelen[7]+filelen[8]+filelen[9],filelen[10]);
      		memcpy(t_byte,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5]+filelen[6]+filelen[7]+filelen[8]+filelen[9]+filelen[10],filelen[11]);
     	
          utStrDelSpaces(t_sdate);
          utStrDelSpaces(t_sip);
          utStrDelSpaces(t_dip);
          trim(t_port);
          utStrDelSpaces(t_byte);
      		trim(t_title);
          trim(t_url);
      		trim(t_host);
//   printf("t_sdate=%s\n",t_sdate);   
          memset(caUrlName,0,sizeof(caUrlName));		
      		ncLimGetWebNameByUrl(t_host,t_type,caUrlName);
      		if((strlen(t_title)==0)&&(strlen(caUrlName)>0)){
      			strcpy(t_title,caUrlName);
      		}
      		
      		lt_time=utTimStrToLong("%Y/%m/%d %H:%M:%S",t_sdate);
    		
      		w_flag=1;
      		
  //对流量进行范围进行查询    		
      lByt=atol(caByt);
	    if(lByt>0){
	 
	    	if(atoi(byt_in)==1){
	    		w_flag=w_flag&&(atol(t_byte)<=lByt);
	    	
	    	}
	    	else{
	    		w_flag=w_flag&&(atol(t_byte)>=lByt);
	        }
	   
	   }
      		
      		
     
      		
      		
      		
      		
      		w_flag=w_flag&&(strlen(caUserid)==0||atol(caUserid)==atol(t_id));
      		w_flag=w_flag&&(strlen(caSip)==0||strcmp(caSip,t_sip)==0);
      		w_flag=w_flag&&(strlen(caDip)==0||strcmp(caDip,t_dip)==0);
      		w_flag=w_flag&&(atol(caDport)==0||atol(caDport)==atol(t_port));
      		w_flag=w_flag&&(lt_time>=lstime&&lt_time<=letime);
      		
      		w_flag=w_flag&&(strlen(caUri)==0||strstr(t_url,caUri));
      		w_flag=w_flag&&(strlen(caWebtitle)==0||strstr(t_title,caWebtitle));
      		w_flag=w_flag&&(strlen(caHost)==0||strstr(t_host,caHost));
      		w_flag=w_flag&&(strlen(caUrname)==0||strstr(t_type,caUrname));
//printf("t_host=%s\n",t_host);
      	  psUser = ncUtlGetUserCont(psShmHead,atol(t_id));         
	        if(psUser){
	        	strcpy(t_dispname,psUser->dispname);
	        	strcpy(t_mac,ncCvtMac(psUser->mac));
	        	w_flag=w_flag&&(strlen(caMac)==0||strstr(t_mac,caMac));
	        	w_flag=w_flag&&(strlen(caIdfyno)==0||strstr(psUser->cardid,caIdfyno));
	        	w_flag=w_flag&&(strlen(caDispname)==0||strstr(psUser->dispname,caDispname)||strstr(psUser->username,caDispname));

	        	w_flag=w_flag&&(strlen(caGroupid)==0||atol(caGroupid)==psUser->groupid);
	        }
	        else{
	        	w_flag=0;
	        }
      		if(lSum<lRow&&w_flag==1){
	      		if(lMum>=lStartRec){
		      		pHttplog[lSum].userid=atol(t_id);
		      		strcpy(pHttplog[lSum].sip,t_sip);
		      		strcpy(pHttplog[lSum].dip,t_dip);
		      		pHttplog[lSum].port=atol(t_port);
		      		strcpy(pHttplog[lSum].byte,t_byte);		      		
		      		strcpy(pHttplog[lSum].sdate,t_sdate);		      	  
		      		strcpy(pHttplog[lSum].dispname,t_dispname);
	      		  strcpy(pHttplog[lSum].host,t_host);
	      		  strcpy(pHttplog[lSum].url,t_url);
	      		  strcpy(pHttplog[lSum].title,t_title);
	      		  strcpy(pHttplog[lSum].caType,t_type);
	      		
	      		lSum++;
	      	 }
      	 }
      		if(w_flag==1){
      			lMum++;
      		  }
      	
      
		}

   		fclose(flog);
   	
  }

	if(lSum>0){
		//按时间排序
		qsort(pHttplog,lSum,sizeof(struct httplog_s),ncUtlSortHttplog); 
	}
   
//输出到excel中

    if(!utStrIsSpaces(caExport)){
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        ncUserCont *psUser;
         char caUsername[32],caGroupname[32],caDipPort[20];
        long id;
        sprintf(caFilename,"nchttp%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613网站访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,ncLang("0614网站访问明细表\n"));
    ncUtlPrintCsv(fp,12,UT_TYPE_STRING,ncLang("0711序号"),
                       UT_TYPE_STRING,ncLang("0693姓  名"),
                       UT_TYPE_STRING,ncLang("0762用户名"),
                       UT_TYPE_STRING,ncLang("0122本人IP"),
                       UT_TYPE_STRING,ncLang("0147部  门"),
                       UT_TYPE_STRING,ncLang("0241访问时间"),
                       UT_TYPE_STRING,ncLang("0618网址"),
                       UT_TYPE_STRING,ncLang("0396目标IP"),
                       UT_TYPE_STRING,ncLang("0685详细目录"),
                       UT_TYPE_STRING,ncLang("0624网址类别"),
                       UT_TYPE_STRING,ncLang("0125标题"),
                       UT_TYPE_STRING,ncLang("0369流量"));
        for(i=0;i<lSum;i++){
        psUser = ncUtlGetUserContById(psShmHead,pHttplog[i].userid);
        if(psUser) {
           
            strcpy(caUsername, psUser->username);
            strcpy(caGroupname,psUser->psGroup->groupname);
        }
        	
      sprintf(caDipPort,"%s:%lu",pHttplog[i].dip,pHttplog[i].port);  	
        ncUtlPrintCsv(fp,12,UT_TYPE_LONG,i+1,
                       UT_TYPE_STRING,pHttplog[i].dispname,
                       UT_TYPE_STRING,caUsername,
                       UT_TYPE_STRING,pHttplog[i].sip,
                       UT_TYPE_STRING,caGroupname,
                       UT_TYPE_STRING,pHttplog[i].sdate,
                       UT_TYPE_STRING,pHttplog[i].host,
                       UT_TYPE_STRING,caDipPort,
                       UT_TYPE_STRING,pHttplog[i].url,                       
                       UT_TYPE_STRING,pHttplog[i].caType,
                       UT_TYPE_STRING,pHttplog[i].title,
                       UT_TYPE_STRING,pHttplog[i].byte);
        	
        }
       
       
       
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        free(pHttplog);
        return 0;
    }
      

   
 
    
    

    /* 显示服务信息  */
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"host",caHost);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"bef",caBef);
    utPltPutVar(psDbHead,"dtype",caDtype);
    utPltPutVar(psDbHead,"stime",caStime0);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"byt_in",byt_in);
    utPltPutVar(psDbHead,"byt",caByt);
    utPltPutVar(psDbHead,"dip",caDip);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"uri",caUri);
    utPltPutVar(psDbHead,"username_match",caUsername_match);
    utPltPutVar(psDbHead,"urlid",caUrlid);
    utPltPutVar(psDbHead,"webtitle",caWebtitle);
    utPltPutVar(psDbHead,"urname",caUrname);
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caDispname,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		      strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
    
    utPltPutVar(psDbHead,"title_name",caTitlename);
     

  
  
  
  for(i=0;i<32&&i<lSum;i++){
  	utPltPutLoopVarF(psDbHead,"userid",i+1,"%d",pHttplog[i].userid);
  	utPltPutLoopVar(psDbHead,"sip",i+1,pHttplog[i].sip);
  	utPltPutLoopVar(psDbHead,"dip",i+1,pHttplog[i].dip);
  	utPltPutLoopVar(psDbHead,"bytes",i+1,utStrLtoF(atol(pHttplog[i].byte),caTemp));
   sprintf(caUrl0,"http://%s%s",pHttplog[i].host,pHttplog[i].url);
   strcpy(caUrl,caUrl0);
   caUrl[78]=0;
  	
  	utPltPutLoopVar(psDbHead,"url",   i+1,caUrl);
    utPltPutLoopVar(psDbHead,"urllnk",   i+1,caUrl0);
  	utPltSetCvtHtml(1);
  	 utPltPutLoopVar(psDbHead,"title",i+1,pHttplog[i].title);
  	utPltPutLoopVar(psDbHead,"urltype",i+1,pHttplog[i].caType);
  	    utPltSetCvtHtml(0);
  	utPltPutLoopVar(psDbHead,"sdate",i+1,pHttplog[i].sdate);

  	utPltPutLoopVar(psDbHead,"dispname",i+1,pHttplog[i].dispname);
  	
  	
  }
  
//    while(iNum < lRowNum + 1) {
 //       i++;
 //       iNum++;
//        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
//    }
  
     
        
      free(pHttplog);  
        
      if(flag_cont==1&&lMum>=1000)
       lMum=lCount;
        
     lTotPg = (lMum ) /lRowNum + 1;
    utPltPutVarF(psDbHead,"TotRec", "%lu", lMum);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_httplog_hsear.htm");
   
    return 0;
    return 0;
}


int ncInsertHttplogFromFile(utShmHead *psShmHead)
{
	long lTime,lMin,lStarttime,lEndtime,lStattime;
	char caSdate[24],caSdate_all[24],caSmin[24];
	char caFile_rz[20];
	char caFile_date[128];
 	long filelen[12]={11,16,11,9,6,17,64,128,104,16,8,10};
  int  iBlockSize;	// 每块的大小；
	int  iRead,i;			// 每次实际读取的行数；
	char *pBlock,*pLineHead;		// 每块的头指针；
 	FILE *flog;
 	long g_iLineLength=401;
 	char caFile[256],caTemp[1024];
 	long iBlockLines;
 	long cid,lt_time,w_flag;
 	long iField=0;
  char *p;
 	char p_temp[256];
  long i_t=0;
 	 char caBuf[2048];
	char t_id[20],t_sdate[28],t_sip[20],t_dip[20],t_port[16],t_host[70],t_byte[20],t_url[200],t_title[130],t_type[40];
			  lTime=time(0);
		    strcpy(caSdate,utTimFormat("%Y/%m/%d %H",lTime));
		    strcpy(caSmin,utTimFormat("%M",lTime));
	//	    printf("caSmin=%s\n",caSmin);
		    lMin=(long)(atol(caSmin)/15)*15;
//		    printf("lMin=%d\n",lMin);
		    sprintf(caSdate_all,"%s:%2d",caSdate,lMin);
		    lEndtime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all);
		    lStarttime=lEndtime-900;
		   
		    strcpy(caFile_rz,utTimFormat("%Y-%m-%d",lStarttime));
		   	strcpy(caFile_date,utTimFormat("%Y-%m-%d-%H",lStarttime));   	
		   	sprintf(caFile,"/home/ncmysql/nc/log/httplog/%s/httplog-%s.log",caFile_rz,caFile_date);
	//	   	printf("caFile=%s\n",caFile);
		    flog=fopen(caFile,"rb");
		    if(flog==NULL) return 0;
		    
   	   
   		char *p1;
   	
   		iField=0;

  //处理首行
        memset(caBuf,0,sizeof(caBuf));
   		  p1=fgets(caBuf,1024,flog);
   //		  printf("caBuf=%s\n",caBuf);
   		  if(p1&&caBuf[0]=='#'){
   		  	for(i=0;i<1024;i++){
   		  		p_temp[i_t]=caBuf[i];
   		  		if(caBuf[i]==';'){
   		  			p_temp[i_t]='\0';
   		  			i_t=0;
				     	p=strstr(p_temp,"n,");
				     	if(p){
				     		filelen[iField]=atol(p+2);
				     		iField++;
				     	}
				     	else{
				     		p=strstr(p_temp,"s,");
				     		if(p){
				     					filelen[iField]=atol(p+2);
				     		      iField++;
				     		}
				     	}
   		  		}
   		  	
   		  		i_t++;
   		  	}
   		  }
	 		
	   		if(iField>0){
	   			g_iLineLength=0;
	   			for(i=0;i<iField;i++){
	   				g_iLineLength=g_iLineLength+filelen[i];
	   			}
	   			g_iLineLength=g_iLineLength+1;
	   		}
		   
			while(fgets(caBuf,1024,flog)!=NULL){

 			if(strlen(caBuf)<g_iLineLength-1)
 			   continue;
 			   
		   pLineHead=caBuf;
		   
	      	
			      		memset(t_sdate,0,sizeof(t_sdate));
			      	
			      		memset(t_host,0,sizeof(t_host));
			      				   
			      		memcpy(t_sdate,pLineHead+filelen[0]+filelen[1],filelen[2]+filelen[3]);			      
			      		memcpy(t_host,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5],filelen[6]);
			      	
			          utStrDelSpaces(t_sdate);
			        
			      		trim(t_host);
  
			         	
			      		ncUrlGetUrlInfo(t_host,t_title,&cid);	
	      		
			      		lt_time=utTimStrToLong("%Y/%m/%d %H:%M:%S",t_sdate);
   		
			      		w_flag=1;
			      		
			      		w_flag=w_flag&&(lt_time>=lStarttime&&lt_time<=lEndtime);
  		
			      		if(cid>0&&w_flag==1){		
			      			
			      				      			
			      			sprintf(caTemp,"insert into httplog_tmp(tid,vtime) values(%d,%d) ",cid,lt_time);
		//      			printf("caTemp=%s\n",caTemp);
			             pasDbExecSql(caTemp,0);
				      	
			      	 }
			      		
			      }		
   		   
		   fclose(flog);
		   
		   
		   
}


struct webclass_count_s{
	long id;
	char name[32];
	long lTCount;
	long lCount[32];
};

/* 根据ID获取下标*/
static int ncUtlGetWebclassById(struct webclass_count_s *arrIn,int iSumArr,unsigned long lId)
{
   int bottom,middle,top;
//    int n;
    top = iSumArr - 1;
    bottom = 0;
    while(top >= bottom) {
        middle = (top + bottom) / 2;   
        if(arrIn[middle].id==lId) {
            return middle;
        }
        else if(arrIn[middle].id>lId) {
            top = middle -1;
        }
        else {
            bottom = middle + 1;
        }
    }
    return (-1);
}

static int ncUtlSortWebclass(char *p1,char *p2)
{
    int i;
    long  l;
   
    struct webclass_count_s psC1,psC2;
    memcpy(&psC1,p1,sizeof(struct webclass_count_s));
    memcpy(&psC2,p2,sizeof(struct webclass_count_s));    
    l=psC2.lTCount-psC1.lTCount;
    if(l>0) return 1;
    if(l<0) return -1;
    if(l==0) return 0;
  
}



/* 从文件中统计首页的网站访问趋势图*/
int ncStatWebTracFromFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    char caSdate[32];
    char caTemp[1024];
    unsigned long lTime;
   	char caUri[128];
   	char caHost[64];  
    int i,iNum;
    utPltDbHead *psDbHead;
   	int iReturn; 
	char sqlbuf[2048]="";
 	pasDbCursor *psCur;
 struct webclass_count_s *pWebclass;
 long lSumwebclass;
 long lId;
 char caName[32];
 char p_temp[256];
 long cid;
 long i_t=0;
 char caBuf[2048];
 long findex;
 char caTime[20];
  	long lStart;
 	char caFile_date[128];
 	long filelen[12]={11,16,11,9,6,17,64,128,104,16,8,10};

	char *pBlock,*pLineHead;		// 每块的头指针；
 	FILE *flog;
 	long g_iLineLength=401;
 	char caFile[256];
 	long iBlockLines;

  char t_dispname[32];
  long w_flag=1;
  long lt_time,lMum;
  char t_mac[24],caProt_t[20],caService_t[20];
  char caFile_rz[20];
  char caExport[20];
  char caUrlName[128];

  char t_id[20],t_sdate[28],t_host[70],t_url[200],t_title[130];
  long iField=0;
   char *p;
   char caStime[16],caSmin[16];
   char caSdate_all[32];
   char caPre_flag[8];
   char caPdf[12];
   char caEdate[20];
   long index,mm;
   long x_sum,y_sum,lMin,lSum,lStarttime,lEndtime;
 
 
 
 pWebclass=(struct webclass_count_s*)malloc(100*sizeof(struct webclass_count_s));
 if(pWebclass==NULL){
  	return 0;
 }
 memset(pWebclass,0,100*sizeof(struct webclass_count_s));
 lSumwebclass=0;
 	
 	sprintf(caTemp,"select id,name from ncwebclass where id>0 order by id limit 0,99");
 	psCur=pasDbOpenSql(caTemp,0);
 	if(psCur==NULL){
 		free(pWebclass);
 		return 0;
 	}
 	lId=0;
 	memset(caName,0,sizeof(caName));
 	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lId,
                                 UT_TYPE_STRING,30,caName);
                                
    lSumwebclass=0;
    while((iReturn == 0)||(iReturn==1405)) {    	
    		pWebclass[lSumwebclass].id=lId;    		
    		strcpy(pWebclass[lSumwebclass].name,caName);
    		pWebclass[lSumwebclass].lTCount=0;
    	for(i=0;i<32;i++){
    		pWebclass[lSumwebclass].lCount[i]=0;
    	}
    	
    	
    lSumwebclass++;
	 	lId=0;
	 	memset(caName,0,sizeof(caName));
	 	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lId,
	                                 UT_TYPE_STRING,30,caName);
    }
    
    pasDbCloseCursor(psCur);
 
  lSum=0;

   utMsgGetSomeNVar(psMsgHead,3,"stime",UT_TYPE_STRING,10,caStime,
																 "pre_flag",UT_TYPE_STRING,2,caPre_flag,
																 "pdf", UT_TYPE_STRING,8,caPdf);
	
//		utMsgPrintMsg(psMsgHead);			        
 
  psDbHead = utPltInitDb(); 
  
  
  	
 
//按分钟统计(nchttplog)
	    x_sum=15;
	    y_sum=15;
	 
      //处理时间
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
	
	
	

   	strcpy(caFile_rz,utTimFormat("%Y-%m-%d",lStarttime));
   	strcpy(caFile_date,utTimFormat("%Y-%m-%d-%H",lStarttime));   	
   	sprintf(caFile,"/home/ncmysql/nc/log/httplog/%s/httplog-%s.log",caFile_rz,caFile_date);
//   		 	  	printf("caFile_date=%s\n",caFile);
   
   	 flog=fopen(caFile,"rb");
		 if(flog==NULL){
		 	free(pWebclass);
		 	 return 0;
		  }
   	   
   		char *p1;
   	
   		iField=0;
     //处理首行
        memset(caBuf,0,sizeof(caBuf));
   		  p1=fgets(caBuf,1024,flog);
   //		  printf("caBuf=%s\n",caBuf);
   		  if(p1&&caBuf[0]=='#'){
   		  	for(i=0;i<1024;i++){
   		  		p_temp[i_t]=caBuf[i];
   		  		if(caBuf[i]==';'){
   		  			p_temp[i_t]='\0';
   		  			i_t=0;
				     	p=strstr(p_temp,"n,");
				     	if(p){
				     		filelen[iField]=atol(p+2);
				     		iField++;
				     	}
				     	else{
				     		p=strstr(p_temp,"s,");
				     		if(p){
				     					filelen[iField]=atol(p+2);
				     		      iField++;
				     		}
				     	}
   		  		}
   		  	
   		  		i_t++;
   		  	}
   		  }
	 		
	   		if(iField>0){
	   			g_iLineLength=0;
	   			for(i=0;i<iField;i++){
	   				g_iLineLength=g_iLineLength+filelen[i];
	   			}
	   			g_iLineLength=g_iLineLength+1;
	   		}
   
   			while(fgets(caBuf,1024,flog)!=NULL){

 			if(strlen(caBuf)<g_iLineLength-1)
 			   continue;
 			   
		      pLineHead=caBuf;
		   
	      	
			      		memset(t_sdate,0,sizeof(t_sdate));
			      	
			      		memset(t_host,0,sizeof(t_host));
			      				   
			      		memcpy(t_sdate,pLineHead+filelen[0]+filelen[1],filelen[2]+filelen[3]);			      
			      		memcpy(t_host,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5],filelen[6]);
			      	
			          utStrDelSpaces(t_sdate);
			        
			      		trim(t_host);
  
			         	
			      		ncUrlGetUrlInfo(t_host,t_title,&cid);	
	      		
			      		lt_time=utTimStrToLong("%Y/%m/%d %H:%M:%S",t_sdate);
   		
			      		w_flag=1;
			      		
			      		w_flag=w_flag&&(lt_time>=lStarttime&&lt_time<=lEndtime);
	
			      		if(cid>0&&w_flag==1){		
			      			index=ncUtlGetWebclassById(pWebclass,lSumwebclass,cid);	 
			      			if(index!=0){
			      				pWebclass[index].lTCount++;
			      				strcpy(caTime,utTimFormat("%M",lt_time));
			      				findex=atol(caTime);

			      				if(findex>=15&&findex<30){                           //后半分钟 时间值和数组下标相差30
							    		findex=findex-15;
							    	}
							    	else if(findex>=30&&findex<45){
							    		findex=findex-30;
							    	}
							    	else if(findex>=45){
							    		findex=findex-45;
							    	}
							    	
							    	pWebclass[index].lCount[findex]=pWebclass[index].lCount[findex]++;
			      			}     			
//			      			sprintf(caTemp,"insert into httplog_tmp(tid,vtime) values(%d,%d) ",cid,lt_time);
		//      			printf("caTemp=%s\n",caTemp);
//			             pasDbExecSql(caTemp,0);
				      	
			      	 }
			      		
			      }		
   		   
		   fclose(flog);
   
   
  	if(lSumwebclass>0){
		//按点击数排序
		qsort(pWebclass,lSumwebclass,sizeof(struct webclass_count_s),ncUtlSortWebclass); 
	} 
   
   
   //输出X轴刻度
   for(mm=0;mm<15;mm++){
   		  	utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
          strcpy(caSmin,utTimFormat("%M",lStarttime));  
	  			sprintf(caTemp,"%s:%02d",utTimFormat("%H",lStarttime),mm+atol(caSmin));	 
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
  }
   
    //输出图形数据 

    for(i=0;i<=lSumwebclass,i<10;i++){
    	if(pWebclass[i].lTCount<=0) break;
    	
    	
    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);

    	utPltPutLoopVar(psDbHead,"sevname",i+1,pWebclass[i].name);
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",pWebclass[i].id);
    	for(mm=0;mm<15;mm++){

    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);
  
    		  utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%d",pWebclass[i].lCount[mm]);
    	
    }
  }
  
      utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);

    	utPltPutLoopVar(psDbHead,"sevname",i+1,pWebclass[i].name);
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",pWebclass[i].id);
    	for(mm=0;mm<15;mm++){

    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);
  
    		  utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%d",pWebclass[i].lCount[mm]);
    	
    }
  
  
  
  utPltPutVarF(psDbHead,"sum","%d",i+1);
  utPltPutVarF(psDbHead,"sum_d","%d",15);


  free(pWebclass); 
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_mainfirst_webclassbytime.htm");
 

   
    return 0;
    
     
}


/* 从文件中统计首页的网站访问趋势图*/
int ncStatWebTracDetFromFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    char caSdate[32];
    char caTemp[1024];
    unsigned long lTime;
   	char caUri[128];
   	char caHost[64];  
    int i,iNum;
    utPltDbHead *psDbHead;
   	int iReturn; 
	char sqlbuf[2048]="";
 	pasDbCursor *psCur;
 struct webclass_count_s *pWebclass;
 long lSumwebclass;
 long lId;
 char caName[32];
 char p_temp[256];
 long cid;
 long i_t=0;
 char caBuf[2048];
 long findex;
 char caTime[20];
  	long lStart;
 	char caFile_date[128];
 	long filelen[12]={11,16,11,9,6,17,64,128,104,16,8,10};

	char *pBlock,*pLineHead;		// 每块的头指针；
 	FILE *flog;
 	long g_iLineLength=401;
 	char caFile[256];
 	long iBlockLines;

  char t_dispname[32];
  long w_flag=1;
  long lt_time,lMum;
  char t_mac[24],caProt_t[20],caService_t[20];
  char caFile_rz[20];
  char caExport[20];
  char caUrlName[128];

  char t_id[20],t_sdate[28],t_host[70],t_url[200],t_title[130];
  long iField=0;
   char *p;
   char caStime[16],caSmin[16];
   char caSdate_all[32];
   char caPre_flag[8];
   char caPdf[12];
   char caEdate[20];
   long index,mm;
   long lCount,lTid;
   char caEtime[16],caTid[16],t_userid[16];
   long x_sum,y_sum,lMin,lSum,lStarttime,lEndtime;
   
   lCount=0;
   pasDbOneRecord("select count(*) from ncuser ",0,UT_TYPE_LONG,4,&lCount);
 
 pWebclass=(struct webclass_count_s*)malloc((lCount+1)*sizeof(struct webclass_count_s));
 if(pWebclass==NULL){
  	return 0;
 }
 memset(pWebclass,0,(lCount+1)*sizeof(struct webclass_count_s));
 lSumwebclass=0;
 	
 	sprintf(caTemp,"select userid,dispname from ncuser  order by userid limit 0,%d",lCount);
 	psCur=pasDbOpenSql(caTemp,0);
 	if(psCur==NULL){
 		free(pWebclass);
 		return 0;
 	}
 	lId=0;
 	memset(caName,0,sizeof(caName));
 	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lId,
                                 UT_TYPE_STRING,30,caName);
                                
    lSumwebclass=0;
    while((iReturn == 0)||(iReturn==1405)) {    	
    		pWebclass[lSumwebclass].id=lId;    		
    		strcpy(pWebclass[lSumwebclass].name,caName);
    		pWebclass[lSumwebclass].lTCount=0;
    	for(i=0;i<32;i++){
    		pWebclass[lSumwebclass].lCount[i]=0;
    	}
    	
    	
    lSumwebclass++;
	 	lId=0;
	 	memset(caName,0,sizeof(caName));
	 	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lId,
	                                 UT_TYPE_STRING,30,caName);
    }
    
    pasDbCloseCursor(psCur);
 
  lSum=0;


     		utMsgGetSomeNVar(psMsgHead,3,"stime",UT_TYPE_STRING,10,caStime,
   		                             "etime", UT_TYPE_STRING,10,caEtime,
   		                             "tid", UT_TYPE_STRING,10,caTid);
   		                            
	
//		utMsgPrintMsg(psMsgHead);			        
 
  psDbHead = utPltInitDb(); 
  
  utPltPutVar(psDbHead,"stime",caStime);
  utPltPutVar(psDbHead,"etime",caEtime);
  utPltPutVar(psDbHead,"servname",ncUrlGetClassNameById(atol(caTid),""));	
 
//按分钟统计(nchttplog)
	    x_sum=15;
	    y_sum=15;
	    lStarttime=atol(caStime);
	    lEndtime=atol(caEtime);
	    lTid=atol(caTid);
	 
    
    
    strcpy(caSdate,utTimFormat("%Y/%m/%d %H:%M",lStarttime));
    strcpy(caEdate,utTimFormat("%Y/%m/%d %H:%M",lEndtime));

   

    utPltPutVar(psDbHead,"tjsj",caSdate);
  

   	strcpy(caFile_rz,utTimFormat("%Y-%m-%d",lStarttime));
   	strcpy(caFile_date,utTimFormat("%Y-%m-%d-%H",lStarttime));   	
   	sprintf(caFile,"/home/ncmysql/nc/log/httplog/%s/httplog-%s.log",caFile_rz,caFile_date);
//   		 	  	printf("caFile_date=%s\n",caFile);
   
   	 flog=fopen(caFile,"rb");
		 if(flog==NULL){
		 	free(pWebclass);
		 	 return 0;
		  }
   	   
   		char *p1;
   	
   		iField=0;
     //处理首行
        memset(caBuf,0,sizeof(caBuf));
   		  p1=fgets(caBuf,1024,flog);
   //		  printf("caBuf=%s\n",caBuf);
   		  if(p1&&caBuf[0]=='#'){
   		  	for(i=0;i<1024;i++){
   		  		p_temp[i_t]=caBuf[i];
   		  		if(caBuf[i]==';'){
   		  			p_temp[i_t]='\0';
   		  			i_t=0;
				     	p=strstr(p_temp,"n,");
				     	if(p){
				     		filelen[iField]=atol(p+2);
				     		iField++;
				     	}
				     	else{
				     		p=strstr(p_temp,"s,");
				     		if(p){
				     					filelen[iField]=atol(p+2);
				     		      iField++;
				     		}
				     	}
   		  		}
   		  	
   		  		i_t++;
   		  	}
   		  }
	 		
	   		if(iField>0){
	   			g_iLineLength=0;
	   			for(i=0;i<iField;i++){
	   				g_iLineLength=g_iLineLength+filelen[i];
	   			}
	   			g_iLineLength=g_iLineLength+1;
	   		}
   
   			while(fgets(caBuf,1024,flog)!=NULL){

 			if(strlen(caBuf)<g_iLineLength-1)
 			   continue;
 			   
		      pLineHead=caBuf;
		   
	      	
			      		memset(t_sdate,0,sizeof(t_sdate));			      	
			      		memset(t_host,0,sizeof(t_host));
			      		memset(t_userid,0,sizeof(t_userid));
			      		
			      		memcpy(t_userid,pLineHead,filelen[0]);
			      				   
			      		memcpy(t_sdate,pLineHead+filelen[0]+filelen[1],filelen[2]+filelen[3]);			      
			      		memcpy(t_host,pLineHead+filelen[0]+filelen[1]+filelen[2]+filelen[3]+filelen[4]+filelen[5],filelen[6]);
			      	
			          utStrDelSpaces(t_sdate);
			        
			      		trim(t_host);
  
			         	cid=0;
			      		ncUrlGetUrlInfo(t_host,t_title,&cid);	
	      		
			      		lt_time=utTimStrToLong("%Y/%m/%d %H:%M:%S",t_sdate);
   		
			      		w_flag=1;
			      		
			      		w_flag=w_flag&&(lt_time>=lStarttime&&lt_time<=lEndtime)&&(cid==lTid);

			      		if(w_flag==1){		
			      			index=ncUtlGetWebclassById(pWebclass,lSumwebclass,atol(t_userid));	 
			      			
			      			if(index!=0){
			      				pWebclass[index].lTCount++;
		//	      				printf("lTcount=%d\n",pWebclass[index].lTCount);
			      				
			      				strcpy(caTime,utTimFormat("%M",lt_time));
			      				findex=atol(caTime);

			      				if(findex>=15&&findex<30){                           //后半分钟 时间值和数组下标相差30
							    		findex=findex-15;
							    	}
							    	else if(findex>=30&&findex<45){
							    		findex=findex-30;
							    	}
							    	else if(findex>=45){
							    		findex=findex-45;
							    	}
							    	
							    	pWebclass[index].lCount[findex]=pWebclass[index].lCount[findex]++;
			      			}     			
//			      			sprintf(caTemp,"insert into httplog_tmp(tid,vtime) values(%d,%d) ",cid,lt_time);
		//      			printf("caTemp=%s\n",caTemp);
//			             pasDbExecSql(caTemp,0);
				      	
			      	 }
			      		
			      }		
   		   
		   fclose(flog);
   
   
  	if(lSumwebclass>0){
		//按点击数排序
		qsort(pWebclass,lSumwebclass,sizeof(struct webclass_count_s),ncUtlSortWebclass); 
	} 
   
   
   //输出X轴刻度
   for(mm=0;mm<15;mm++){
   		  	utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
          strcpy(caSmin,utTimFormat("%M",lStarttime));  
	  			sprintf(caTemp,"%s:%02d",utTimFormat("%H",lStarttime),mm+atol(caSmin));	 
	  		  utPltPutLoopVar(psDbHead,"sdate",mm+1,caTemp);
  }
   
   //输出到表格

   
   
 

    for(i=0;i<=lSumwebclass,i<10;i++){
    	if(pWebclass[i].lTCount<=0) break;
    	
    	
    	utPltPutLoopVarF(psDbHead,"iNum",i+1,"%d",i);

    	utPltPutLoopVar(psDbHead,"sevname",i+1,pWebclass[i].name);
      utPltPutLoopVarF(psDbHead,"tid",i+1,"%d",pWebclass[i].id);
    	for(mm=0;mm<15;mm++){

    		utPltPutLoopVarF2(psDbHead,"num",i+1,mm+1,"%d",i);
    		utPltPutLoopVarF2(psDbHead,"num2",i+1,mm+1,"%d",mm);
  
    		  utPltPutLoopVarF2(psDbHead,"data",i+1,mm+1,"%d",pWebclass[i].lCount[mm]);
    	
      }
    }
  
     
   lSum=i;
    	
  
   utPltPutVarF(psDbHead,"sum","%d",i);
   utPltPutVarF(psDbHead,"sum_d","%d",15);
  
  
 
  
  //输出表格数据
  
  for(mm=0;mm<15;mm++){
 // 		utPltPutLoopVarF(psDbHead,"num",mm+1,"%d",mm);
  		for(i=0;i<lSum;i++){
  			utPltPutLoopVarF2(psDbHead,"data2",mm+1,i+1,"%d",pWebclass[i].lCount[mm]); 
  		}
  }
  
 


  free(pWebclass); 
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75/iframe_mainfirst_webclassbytimedetfile.htm");
 

   
    return 0;
  }

