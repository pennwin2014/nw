/* ������---������
                     Modified 2009/08/03
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


/* ��ʾ��������ѯ*/
int ncWebShowServiceDisCx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    struct ncSflow_s  *sService;
    struct ncSflow_s  sGservice[200];
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
    char user_Id[15]="";
    char dpname[32]="";
    char gpname[32]="";
    char rtname[32]="";
    char expexcel[12]="";
    char f_name[120]="";
    char cp_command[320]="";
  	char rm_command[320]="";
  	char caSdate_All[20],caEdate_All[20];
  	FILE *fp; 
  	char group_id[32]="";
  	char webid[20000];
  	char webname[20000];
  	char caSum[12];
  	long lSumShow;
  	long locoindex;
  	char caWorkflag[12];
  	char caWorksql[1024],caWorksql2[1024];
  	
   lTime=time(0);
   lSum=0;
   lRecenttime=0;
   psDbHead = utPltInitDb();
      utMsgGetSomeNVar(psMsgHead,18,"gitem",UT_TYPE_STRING,10,caGitem,					  //ͳ����Ŀ
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//��ʼʱ������
                                    "shour",UT_TYPE_STRING,2,caShour,						  //��ʼʱ���Сʱ
                                    "smin", UT_TYPE_STRING,2,caSmin,
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "ehour",UT_TYPE_STRING,2,caEhour,
                                    "emin", UT_TYPE_STRING,2,caEmin,
                                    "exppdf",UT_TYPE_STRING,8,caPdf,							//����pdf
                                    "groupid",UT_TYPE_STRING,30,group_id,					//����id
                                    "userid",UT_TYPE_STRING,14,user_Id,				    //�û�id
                                    "groupname",UT_TYPE_STRING,30,gpname,					//�����еĲ�����
                                    "dispname",UT_TYPE_STRING,30,dpname,				  //������ʾ��
                                    "expexcel",UT_TYPE_STRING,10,expexcel,				//����pdf	
                                    "rootname",UT_TYPE_STRING,30,rtname,
                                    "webid",UT_TYPE_STRING,19999,webid,
                                    "webname",UT_TYPE_STRING,19999,webname,
                                    "sunrec",    UT_TYPE_STRING,8, caSum,
                                    "workflag",UT_TYPE_STRING,8,caWorkflag);				
  strcpy(caSdate_All,utTimFormat("%Y-%m-%d",utTimStrToLong("%Y/%m/%d %H:%M",caSdate))); 
  strcpy(caEdate_All,utTimFormat("%Y-%m-%d",utTimStrToLong("%Y/%m/%d %H:%M",caEdate)));
  
  if(strlen(caSum)==0) strcpy(caSum,"10");
   lSumShow=atol(caSum);
  
 //����ַ�����Ϣ����sGservice �� select name,id from ncwebclass where 1=1  order by groupid
   	 psCur=pasDbOpenSql("select id,name from ncwebclass where 1=1  order by id ",0);
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
  	 utPltPutVar(psDbHead,"sunrec",caSum);
 
     utPltPutVar(psDbHead,"workflag",caWorkflag);
 
 //��ȡnchttpcount�����������ֵ 	���ֵ�����ж϶�ʱͳ�Ƴ���ǰ�Ѿ�����ķ������ݣ��������ʱ���û��������Ҫ��nchttplog�л�ȡ 
  	 memset(caMaxdate,0,sizeof(caMaxdate));
  	 pasDbOneRecord("select max(sdate) from nchttpcount ",0,UT_TYPE_STRING,10,caMaxdate);
  	 sprintf(caTemp,"select max(stime) from nchttpcount where sdate='%s' ",caMaxdate);
  	 memset(caMaxtime,0,sizeof(caMaxtime));
  	 pasDbOneRecord(caTemp,0,UT_TYPE_STRING,2,caMaxtime);
  	 
  	 
  	 if(strlen(caMaxdate)>0&&(strlen(caMaxtime))){
  	 	 sprintf(caRectime,"%s %s",caMaxdate,caMaxtime);
  	 	 lRecenttime=utTimStrToLong("%Y/%m/%d %H",caRectime);
  	 }
  	 sService=(struct ncSflow_s *)malloc(2*(lSumShow+1)*sizeof(struct ncSflow_s));
  	 locoindex=0;
  	 if(lEtime>lRecenttime){
 				//ͳ��nchttplog�еķ���
      	if(lStime>lRecenttime){
      		lStime1=lStime;
      	}else{
      		lStime1=lRecenttime;
      	}
      	sprintf(caTemp,"select count(*) aa,tid from nchttplog where vtime>%d and vtime<=%d ",lStime1,lEtime);
      	if(strlen(group_id)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and (groupid =%s or groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  	else if(strlen(user_Id)>0)
  	  		 sprintf(caTemp+strlen(caTemp)," and userid in (%s) ",user_Id);	
  	  	else
  	  		 sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  		 
  	  		//�Ƿ���ʱ��
			if(atol(caWorkflag)==1){
				 strcpy(caWorksql,ncUtlCvtWorkTime2Sql("vtime"));
				 if(strlen(caWorksql)>0){
				 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql);
			   }  
			  }
  	  		 
  	  		 
  	    if(strlen(webid)!=0)
  	    	  sprintf(caTemp+strlen(caTemp)," and tid in(%s) ",webid);	
  	  	sprintf(caTemp+strlen(caTemp),"  group by tid order by aa desc limit 0,%lu",lSumShow);
      	printf("**nchttplog**caTemp=%s\n",caTemp);
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
       		sService[locoindex].lBytes=lBytes;
	       	sService[locoindex].lService=lService;	      
      	 locoindex++;
	       lBytes=0;
	       lService=0;
	       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                      UT_TYPE_LONG,4,&lService);
       }                               
      pasDbCloseCursor(psCur);                           
 
  	 }
  	 
  	  	  		  //�Ƿ���ʱ��
					if(atol(caWorkflag)==1){
						 strcpy(caWorksql2,ncUtlCvtWorkTime2Sql_s("a.sdate","a.stime"));
//		printf("caWorksql2=%s\n",caWorksql2);
  	  		  }	
  	 
  	  //ͳ��nchttpcount�е�����
  	  if(lStime<lRecenttime){           //ͳ�ƿ�ʼʱ����nchttpcount �е����ʱ��ǰ������
  	  	if(strcmp(caSdate,caEdate)!=0){
  	  	    sprintf(caTemp,"select sum(lcount) aa,a.tid from nchttpcount a,ncuser b where a.userid=b.userid and ((a.sdate>'%s' and a.sdate<'%s') or (a.sdate='%s' and a.stime>='%s') or (a.sdate='%s' and a.stime<='%s')) ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour);
  	  		  if(strlen(group_id)>0)
  	  		  	sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",group_id);
  	  		  else if(strlen(user_Id)>0)
  	  		    sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",user_Id);	
  	  		  else
  	  		  	sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  		  	
  	  		  //�Ƿ���ʱ��
					if(atol(caWorkflag)==1){
						 strcpy(caWorksql2,ncUtlCvtWorkTime2Sql_s("a.sdate","a.stime"));
						 if(strlen(caWorksql2)>0){
						 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
					   } 	
  	  		  }	
  	  		  	
  	  		  if(strlen(webid)!=0)
  	    	  sprintf(caTemp+strlen(caTemp)," and tid in(%s) ",webid);	
  	  		  sprintf(caTemp+strlen(caTemp),"  group by tid order by aa desc limit 0,%lu",lSumShow);
  	  	}else{
  	  		sprintf(caTemp,"select sum(lcount) aa,a.tid from nchttpcount a,ncuser b where a.userid=b.userid and a.sdate='%s' and a.stime>='%s' and a.stime<='%s' ",caSdate,caShour,caEhour);
  	  		if(strlen(group_id)>0)
  	  		  	sprintf(caTemp+strlen(caTemp)," and (b.groupid =%s or b.groupid in (select groupid from ncgroup where pid=%s ))",group_id,group_id);
  	  		else if(strlen(user_Id)>0)
  	  		    sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",user_Id);	
  	  		else
  	  		  	sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  		  	
  	  		 //�Ƿ���ʱ��
					if(atol(caWorkflag)==1){
						 strcpy(caWorksql2,ncUtlCvtWorkTime2Sql_s("a.sdate","a.stime"));
						 if(strlen(caWorksql2)>0){
						 	sprintf(caTemp+strlen(caTemp)," and %s ",caWorksql2);
					   } 	
  	  		 }	  	
  	  		  	
  	  		  	
  	  		if(strlen(webid)!=0)
  	    	  sprintf(caTemp+strlen(caTemp)," and tid in(%s) ",webid);	
  	  		sprintf(caTemp+strlen(caTemp),"  group by tid order by aa desc limit 0,%lu",lSumShow);
  	  	}
  	  	printf("**nchttpcount**caTemp=%s\n",caTemp);
  	  psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
        return 0;
       }
       lBytes=0;
       lService=0;
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_LONG,4,&lService);
                                      		
       while((iReturn==0)||(iReturn==1405)){
       		sService[locoindex].lBytes=lBytes;
       		sService[locoindex].lService=lService;   
       		
	       lBytes=0;
	       lService=0;
	       locoindex++;
	       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                     UT_TYPE_LONG,4,&lService);
       }                               
      pasDbCloseCursor(psCur); 
  	} 
  lSj=0; 	 
// printf("**yanzheng*****\n");
  		//��ַ���
  		strcpy(caTitle,ncLang("1956��ַ�����ʴ����ֲ����"));
  	      utPltPutVar(psDbHead,"title",ncLang("1957��ַ�����ʴ����ֲ�ͼ"));
  	      utPltPutVar(psDbHead,"title_1",ncLang("1958��ַ�����ʴ����ֲ���"));       
  	      for(i=0;i< locoindex;i++){  	      
  	      //sGservice:Ŀ������  lSumGindex:����   3��Ŀ��
  	      	lSum=lSum+sService[i].lBytes;
  	      	//�ڲ�ѯ�ṹ����������ƥ������id
  	      	index=ncUtlGetServicenumById(&sGservice,lSumGindex,sService[i].lService);   //sService[i].lGroupidֵ����i��û��
  	      	if(index!=-1){
  	      		sGservice[index].lBytes=sGservice[index].lBytes+sService[i].lBytes;  	      		  	      		
  	        }else{  	        	
  	        	lOth=lOth+sService[i].lBytes;
  	        }
  	      }
 //���� 	 
  	    qsort(sGservice,lSumGindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId); 
  	    iNum=0;
  	  printf("==***33***=lSum=%d \n",lSum);
  	    for(i=0;i<lSumGindex;i++){ 	    	
  	    	if(sGservice[i].lBytes>0){
  	    
  	    			//��������	 
  	    		 	utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%llu",	sGservice[i].lBytes); 
  	    		 	utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	    		 	utPltPutLoopVar(psDbHead,"name_1",  iNum+1,sGservice[i].caGname);
  	    		 	if(lSum==0){
    						utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    					}else{
    	  				utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
    					}
  	    		  
  	   			//�����ͼ		  
  	    		 if(i<20) {
    		 	
  	    		  	lSj=lSj+1;
  	    		 		utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	sGservice[i].lBytes);  
             		utPltPutLoopVar(psDbHead,"name",  iNum+1,sGservice[i].caGname);
             		utPltPutLoopVarF(psDbHead,"groupid",iNum+1,"%d",sGservice[i].lService);
             		utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",lTime);
             		utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
           
     						if(lSum==0){
    							utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    						}else{
    	  					utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
    						}
           		}else{
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
    				}else{
    	  			utPltPutLoopVarF(psDbHead,"rate",lSj+1,"%0.2f",(float)100*lOth/lSum); 	  			
    				}             
     }
   
  	if(strlen(caPdf)>0){
			//����ǩ
	 		desc=malloc(3*sizeof(char*));
	 		for(i=0;i<3;i++){
	 			desc[i]=malloc(32);
   		}
		   strcpy(desc[0],ncLang("0624��ַ���"));		  
		   sprintf(desc[1],"%s(%s)",ncLang("0180����"),ncLang("1898��"));
		   strcpy(desc[2],ncLang("1827�ٷֱ�%"));
     
 		 //�����PDF��     
   	  pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*(lSumShow+3));
      if(pChart==NULL){
  	  	 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("1863�����ڴ����"));
      	 return 0;
   	  } 
   	 iNum=0;
   
   	 for(i=0;i<lSj;i++){   	   
   	 	if(iNum<20){
   	 		//if(atol(caGitem)==100){                                            
   	 	  	pChart[iNum].name=strdup(sGservice[i].caGname);
  				pChart[iNum].amount=(float)sGservice[i].lBytes; 	
  			 	if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
          	if(lSum==0){
   	 					sprintf(caTemp,"0.00");
   	 				}else{
   	 			  	sprintf(caTemp,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
   	 				}
 	 		
   	 				pChart[iNum].percent=strdup(caTemp); 
           	iNum++;
        	/*}else{      	
       			pChart[iNum].name=strdup(ncUtlGetServiceName(psShmHead,sService[i].lService,ncLang("0431����")));                       //ϸ�ַ���
  					pChart[iNum].amount=(float)sService[i].lBytes; 	
  			 		if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
          	if(lSum==0){
   	 					sprintf(caTemp,"0.00");
   	 				}else{
   	 			  	sprintf(caTemp,"%0.2f",(float)100*sService[i].lBytes/lSum);
   	 				}
 	 		
   	 				pChart[iNum].percent=strdup(caTemp); 
            iNum++;       	
      		} */  
     		}
  	 }
  	         // �������
     if(lOth>0){
     		pChart[iNum].name=strdup(ncLang("0431����"));  
     	   	if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lOth)/lSum);
   	 			}  	 			
     	   	pChart[iNum].percent=strdup(caTemp);  	 		
   	 			pChart[iNum].amount=(float)lOth;                  
          iNum++;                                     
     	}
     
      rowrow=iNum;
   
    	sprintf(filename,"%s.PDF",caTitle); 
	//		printf("rowrow=%d\n",rowrow);
     	ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,3,0, caTitle,ncLang("1949��λ:��"),caStatdate,filename); 
     
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
   //����excel
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
			strcat(f_name,ncLang("1948��վ���ʷֲ������ѯ--"));
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
    	fprintf(fp,ncLang("1959���\t��ַ���\t���ʴ���(��)\t�ٷֱ�\n")); 
    	iNum=0; 
    	for(i=0;i<lSumGindex;i++){
    		if(sGservice[i].lBytes>0){
  	    	if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}else{
   	 				sprintf(caTemp,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
   	 			}
    			fprintf(fp,"%d\t%s\t%llu\t%s\n",iNum+1,sGservice[i].caGname,sGservice[i].lBytes,strdup(caTemp));
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
    	utPltPutVar(psDbHead,"groupname",gpname); 
			utPltPutVar(psDbHead,"dispname",dpname);  
			utPltPutVar(psDbHead,"groupid",group_id); 
			utPltPutVar(psDbHead,"userid",user_Id); 
			utPltPutVar(psDbHead,"rootname",rtname); 
			utPltPutVar(psDbHead,"webid",webid); 
			if(strlen(webname)==0)
				utPltPutVar(psDbHead,"webname",ncLang("1889���ѡ��"));
			else
				utPltPutVar(psDbHead,"webname",webname); 
			memset(webid,0,sizeof(webid));
			memset(webname,0,sizeof(webname));
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_website/iframe_servicedisbtcx.htm");  
    }
    
    memset(sGservice,0,sizeof(sGservice));
    memset(sService,0,sizeof(sService));
    return 0;
}
               
//��ʾĳ���Ż�ĳ��ĳ����ַ������ϸ
int ncShowFirstServiceDisDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    
    struct ncSflow_s sService[20];
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
    
	 char gp_id[32]="";
	 char us_id[15]="";
	 char caHost[64];
	 long q;
      utMsgGetSomeNVar(psMsgHead,15,"gitem",UT_TYPE_STRING,12,caGitem,
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
                                 "groupid",UT_TYPE_STRING,30,gp_id,
                                 "userid",UT_TYPE_STRING,14,us_id);
   
 // printf("caStime=%s,caEtime=%s,caSdate=%s,csShour=%s,caSmin=%s,caEhour=%s,caEmin=%s\n",caStime,caEtime,caSdate,caShour,caSmin,caEhour,caEmin);
 
 // printf("caService=%s\n",caService);
 	for(q=0;q<30;q++){
 		if(psService[q].caGname) 
 			psService=NULL;
 			//free(psService[q].caGname);
 	}
  printf("gp_id=%s\n",gp_id);
  printf("us_id=%s\n",us_id);
  psDbHead = utPltInitDb(); 
  if(strlen(caTopnum)==0)
       strcpy(caTopnum,"10");
       
  if(strlen(caFlag)==0) strcpy(caFlag,"1");   
  if(atol(caFlag)==1){
  	sprintf(caTitle,"%s%s",ncUtlGetServiceName(psShmHead,atol(caService),ncLang("0431����")),ncLang("1960���ʴ������"));
  	utPltPutVar(psDbHead,"servname",ncUtlGetServiceName(psShmHead,atol(caService),ncLang("0431����")));
  }else if(atol(caFlag)==2){
  	sprintf(caTemp," select name from ncwebclass where id=%d ",atol(caService));
  	memset(caName,0,sizeof(caName));
  	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caName);
  	sprintf(caTitle,"%s%s",caName,ncLang("1960���ʴ������"));
  	utPltPutVar(psDbHead,"servname",caName);
  }
  psService=(struct ncSflow_s *)malloc(sizeof(struct ncSflow_s)*(5*atol(caTopnum)+1));
  if(psService==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("1863�����ڴ����"));
      return 0;
  } 

   if(strlen(caStime)>0){
   		strcpy(caSdate,utTimFormat("%Y/%m/%d",atol(caStime))); 
   		strcpy(caShour,utTimFormat("%H",atol(caStime))); 
   		strcpy(caSmin,utTimFormat("%M",atol(caStime)));
   		lStime=atol(caStime);
   }
   if(strlen(caEtime)>0){
    	strcpy(caEdate,utTimFormat("%Y/%m/%d",atol(caEtime)));
    	strcpy(caEhour,utTimFormat("%H",atol(caEtime)));
    	strcpy(caEmin,utTimFormat("%M",atol(caEtime)));
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
 
 //��ȡnchttpcount�����������ֵ 	���ֵ�����ж϶�ʱͳ�Ƴ���ǰ�Ѿ�����ķ������ݣ��������ʱ���û��������Ҫ��nchttplog�л�ȡ 
  	 memset(caMaxdate,0,sizeof(caMaxdate));
  	 pasDbOneRecord("select max(sdate) from nchttpcount ",0,UT_TYPE_STRING,10,caMaxdate);
  	 sprintf(caTemp,"select max(stime) from nchttpcount where sdate='%s' ",caMaxdate);
  	 memset(caMaxtime,0,sizeof(caMaxtime));
  	 pasDbOneRecord(caTemp,0,UT_TYPE_STRING,2,caMaxtime);
  	 
  	 if(strlen(caMaxdate)>0&&(strlen(caMaxtime))){
  	 	 sprintf(caRectime,"%s %s",caMaxdate,caMaxtime);
  	 	 lRecenttime=utTimStrToLong("%Y/%m/%d %H",caRectime);
  	 }
  		lSum=0;	 
  		lSumindex=0;
  	if(lEtime>lRecenttime){
 //ͳ��nchttplog�еķ���  ---��httpcount��ʱ�����ֵ����ǰlendtimeʱ��
      	if(lStime>lRecenttime){
      		lStime1=lStime;
      	}else{
      		lStime1=lRecenttime;
      	}
      printf("11=========\n");
      //ͳ��������ַ�����ܷ��ʴ���
      lTsum=0;
      if(atol(caFlag)==1){								//flag=1ͳ�Ƶ�������
      	sprintf(caTemp,"select count(*) from nchttplog where vtime>%d and vtime<=%d and tid=%d ",lStime1,lEtime,atol(caService));
     		if(strlen(gp_id)>0)
  	  		sprintf(caTemp+strlen(caTemp)," and groupid =%s ",gp_id);
  	  	else if(strlen(us_id)>0)
  	  		sprintf(caTemp+strlen(caTemp)," and userid in (%s) ",us_id);	
  	  	else
  	  		sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
      }else{															//flag=2ͳ�Ƹ���ַ���ķ��ʴ���
      	sprintf(caTemp,"select count(*) from nchttplog  where vtime>%d and vtime<=%d  and tid=%d ",lStime1,lEtime,atol(caService));
      	if(strlen(gp_id)>0)
  	  		sprintf(caTemp+strlen(caTemp)," and groupid =%s ",gp_id);
  	  	else if(strlen(us_id)>0)
  	  		sprintf(caTemp+strlen(caTemp)," and userid in (%s) ",us_id);	
  	  	else
  	  		sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
      }
      pasDbOneRecord(caTemp,0,UT_TYPE_LONG8,8,&lTsum);

      lSum=lSum+lTsum;
       printf("11caTemp=%s\n",caTemp);
 //     printf("lSum=%llu\n",lSum);     
      if(atol(caFlag)==1){                                                 //ͳ�Ƶ�������
      	if(atol(caGitem)==1){                                              //���û�ͳ��
      		sprintf(caTemp,"select count(*) bb,userid,host from nchttplog where vtime>%d and vtime<=%d and tid=%d ",lStime1,lEtime,atol(caService));
      		if(strlen(gp_id)>0)
  	  			sprintf(caTemp+strlen(caTemp)," and groupid =%s ",gp_id);
  	  		else if(strlen(us_id)>0)
  	  			sprintf(caTemp+strlen(caTemp)," and userid in (%s) ",us_id);	
  	  		else
  	  			sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  		sprintf(caTemp+strlen(caTemp)," group by host order by bb desc limit 0,%d  ",2*atol(caTopnum));	
      	}else{                                                              //������ͳ��
      		sprintf(caTemp,"select count(*) bb,b.groupid,a.host from nchttplog a,ncuser b where a.vtime>%d and a.vtime<=%d and a.userid=b.userid and a.tid=%d  ",lStime1,lEtime,atol(caService));
      		if(strlen(gp_id)>0)
  	  			sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  		else if(strlen(us_id)>0)
  	  			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  		else
  	  			sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  		sprintf(caTemp+strlen(caTemp)," group by a.host order by bb desc limit 0,%d  ",2*atol(caTopnum));	
      	}
      }else if(atol(caFlag)==2){
      	 if(atol(caGitem)==1){                               //ͳ�Ʒ������
      	  	sprintf(caTemp,"select count(*) bb,userid,host from nchttplog where  vtime>%d and vtime<=%d  and tid=%d ", lStime1,lEtime,atol(caService));     
      	 		if(strlen(gp_id)>0)
  	  			sprintf(caTemp+strlen(caTemp)," and groupid =%s ",gp_id);
  	  		else if(strlen(us_id)>0)
  	  			sprintf(caTemp+strlen(caTemp)," and userid in (%s) ",us_id);	
  	  		else
  	  			sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  		sprintf(caTemp+strlen(caTemp)," group by host order by bb desc limit 0,%d  ",2*atol(caTopnum));	
      	 }else{
      			sprintf(caTemp,"select count(*) bb,b.groupid,a.host from nchttplog a,ncuser b where a.vtime>%d and a.vtime<=%d and a.userid=b.userid and a.tid=%d  ",lStime1,lEtime,atol(caService));
      	 		if(strlen(gp_id)>0)
  	  			sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  		else if(strlen(us_id)>0)
  	  			sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  		else
  	  			sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  		sprintf(caTemp+strlen(caTemp)," group by a.host order by bb desc limit 0,%d  ",2*atol(caTopnum));	
      	 }                                                      //ͳ�Ʒ������
      }
      printf("111caTemp=%s\n",caTemp);
      psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
        return 0;
      }
       lBytes=0;
       lService=0;
       lSumindex=0;
       memset(caHost,0,sizeof(caHost));      
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_LONG,4,&lService,
                                      UT_TYPE_STRING,63,caHost);
       while((iReturn==0)||(iReturn==1405)){
       	 psService[lSumindex].lService=lService;
         psService[lSumindex].lBytes=lBytes;
         strcpy(psService[lSumindex].caGname,caHost);
        // printf("== 0011== caGname=%s\n",psService[lSumindex].caGname);
       	 lSumindex++;	
	       lBytes=0;
	       lService=0;
	       memset(caHost,0,sizeof(caHost));      
	       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                      UT_TYPE_LONG,4,&lService,
	                                      UT_TYPE_STRING,63,caHost);
       }                               
       pasDbCloseCursor(psCur);                           
 
  	 }
  	//��USERID��������Ϊ���ַ���ѯ��׼�� 
  	 qsort(psService,lSumindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId2); 
  	 
  	  //ͳ��nchttpcount�е�����
  	  if(lStime<lRecenttime){           //ͳ�ƿ�ʼʱ����nchttpcount �е����ʱ��ǰ������
  	  	//ͳ��nchttpcount�е����з����������
  	  	if(atol(caFlag)==1){
  	  	 		if(strcmp(caSdate,caEdate)!=0){
  	  				sprintf(caTemp,"select sum(a.lcount) from nchttpcount a,ncuser b where a.userid=b.userid and ((a.sdate>'%s' and a.sdate<'%s') or (a.sdate='%s' and a.stime>='%s') or (a.sdate='%s' and a.stime<='%s')) and a.tid=%d  ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour,atol(caService));
  	  	 			if(strlen(gp_id)>0)
  	  					sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  				else if(strlen(us_id)>0)
  	  					sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  				else
  	  				sprintf(caTemp+strlen(caTemp)," and 1=1 ");	
  	  	 		}else{
  	  				sprintf(caTemp,"select sum(a.lcount) from nchttpcount a,ncuser b where a.userid=b.userid and a.sdate='%s' and a.stime>='%s' and a.stime<='%s' and a.tid=%d  ",caSdate,caShour,caEhour,atol(caService));
  	  				if(strlen(gp_id)>0)
  	  					sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  				else if(strlen(us_id)>0)
  	  					sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  				else
  	  				sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  			}
  	  	}else{
  	  		 if(strcmp(caSdate,caEdate)!=0){
  	  				sprintf(caTemp,"select sum(a.lcount) from nchttpcount a,ncuser b where a.userid=b.userid and ((a.sdate>'%s' and a.sdate<'%s') or (a.sdate='%s' and a.stime>='%s') or (a.sdate='%s' and a.stime<='%s'))  and a.tid=%d  ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour,atol(caService));
  	  	 	 		if(strlen(gp_id)>0)
  	  					sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  				else if(strlen(us_id)>0)
  	  					sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  				else
  	  				sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  	 	 }else{
  	  			  sprintf(caTemp,"select sum(a.lcount) from nchttpcount a,ncuser b where a.userid=b.userid and a.sdate='%s' and a.stime>='%s' and a.stime<='%s'  and a.tid=%d  ",caSdate,caShour,caEhour,atol(caService));
  	  				if(strlen(gp_id)>0)
  	  					sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  				else if(strlen(us_id)>0)
  	  					sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  				else
  	  				sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  		 }
  	  	}
  	  	    pasDbOneRecord(caTemp,0,UT_TYPE_LONG8,8,&lTsum);
           lSum=lSum+lTsum;
 printf("221caTemp=%s\n",caTemp); 	  	
  	  	
  	  	if(atol(caFlag)==1){																										//����
  	  		if(atol(caGitem)==1){																									//�û�
  	  				if(strcmp(caSdate,caEdate)!=0){
  	  				  	sprintf(caTemp,"select sum(a.lcount) bb,a.userid,a.host from nchttpcount a,ncuser b where a.userid=b.userid and ((a.sdate>'%s' and a.sdate<'%s') or (a.sdate='%s' and a.stime>='%s') or (a.sdate='%s' and a.stime<='%s')) and a.tid=%d  ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour,atol(caService));
  	  						if(strlen(gp_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  						else if(strlen(us_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  						else
  	  							sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  						sprintf(caTemp+strlen(caTemp)," group by a.host order by bb desc limit 0,%d  ",2*atol(caTopnum));	
  	  				}else{
  	  				   sprintf(caTemp,"select sum(a.lcount) bb,a.userid,a.host from nchttpcount a,ncuser b where a.userid=b.userid and a.sdate='%s' and a.stime>='%s' and a.stime<='%s' and a.tid=%d   ",caSdate,caShour,caEhour,atol(caService));
  	  					 if(strlen(gp_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  						else if(strlen(us_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  						else
  	  							sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  						sprintf(caTemp+strlen(caTemp)," group by a.host order by bb desc limit 0,%d  ",2*atol(caTopnum));
  	  				}			
  	  		}else{
  	  			  if(strcmp(caSdate,caEdate)!=0){
  	  				  	sprintf(caTemp,"select sum(lcount) bb,groupid,host from nchttpcount a,ncuser b where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s')) and a.userid=b.userid and tid=%d  ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour,atol(caService));
  	  						if(strlen(gp_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  						else if(strlen(us_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  						else
  	  							sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  						sprintf(caTemp+strlen(caTemp)," group by host order by bb desc limit 0,%d ",2*atol(caTopnum));
  	  				 }else{
  	  				   sprintf(caTemp,"select sum(lcount) bb,groupid,host from nchttpcount a,ncuser b where a.userid=b.userid and sdate='%s' and stime>='%s' and stime<='%s' and tid=%d group by groupid limit 0,%d order by bb desc ",caSdate,caShour,caEhour,atol(caService));
  	  					 if(strlen(gp_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  						else if(strlen(us_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  						else
  	  							sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  						sprintf(caTemp+strlen(caTemp)," group by host order by bb desc limit 0,%d ",2*atol(caTopnum));
  	  				 }			
  	  		}
  	  }else{  																																//ĳ����ַ���	  		
  	  		  if(atol(caGitem)==1){
  	  				 if(strcmp(caSdate,caEdate)!=0){
  	  				  	sprintf(caTemp,"select sum(a.lcount) bb,a.userid,a.host from nchttpcount a,ncuser b where a.userid=b.userid and ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s')) and  tid=%d  ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour,atol(caService));
  	  				 		if(strlen(gp_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  						else if(strlen(us_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  						else
  	  							sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  						sprintf(caTemp+strlen(caTemp)," group by a.host order by bb desc limit 0,%d ",2*atol(caTopnum));
  	  				 }else{
  	  				   sprintf(caTemp,"select sum(a.lcount) bb,a.userid,a.host from nchttpcount a,ncuser b where a.userid=b.userid and sdate='%s' and stime>='%s' and stime<='%s' and  tid=%d ",caSdate,caShour,caEhour,atol(caService));
  	  		  		 if(strlen(gp_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  						else if(strlen(us_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  						else
  	  							sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  						sprintf(caTemp+strlen(caTemp)," group by a.host order by bb desc limit 0,%d ",2*atol(caTopnum));
  	  				 }
  	  		  }else{
  	  			   if(strcmp(caSdate,caEdate)!=0){
  	  				  	sprintf(caTemp,"select sum(lcount) bb,b.groupid,host from nchttpcount a,ncuser b where ((sdate>'%s' and sdate<'%s') or (sdate='%s' and stime>='%s') or (sdate='%s' and stime<='%s')) and a.userid=b.userid and  tid=%d  ", caSdate,caEdate,caSdate,caShour,caEdate,caEhour,atol(caService));
  	  				 		if(strlen(gp_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  						else if(strlen(us_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  						else
  	  							sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  						sprintf(caTemp+strlen(caTemp)," group by a.host order by bb desc limit 0,%d ",2*atol(caTopnum));
  	  				 }else{
  	  				   sprintf(caTemp,"select sum(lcount) bb,b.groupid,host from nchttpcount a,ncuser b where sdate='%s' and stime>='%s' and stime<='%s' and tid=%d  ",caSdate,caShour,caEhour,atol(caService));
  	  					 if(strlen(gp_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and b.groupid =%s ",gp_id);
  	  						else if(strlen(us_id)>0)
  	  							sprintf(caTemp+strlen(caTemp)," and a.userid in (%s) ",us_id);	
  	  						else
  	  							sprintf(caTemp+strlen(caTemp)," and 1=1 ");
  	  						sprintf(caTemp+strlen(caTemp)," group by host order by bb desc limit 0,%d ",2*atol(caTopnum));
  	  				 }
  	  		}
  	  }
  	  	 	  	
  	  	printf("222caTemp=%s\n",caTemp);
  	  psCur=pasDbOpenSql(caTemp,0);
   	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
        return 0;
       }
       lBytes=0;
       lService=0;
       memset(caHost,0,sizeof(caHost));      
       iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
                                      UT_TYPE_LONG,4,&lService,
                                      UT_TYPE_STRING,63,caHost);
       while((iReturn==0)||(iReturn==1405)){
       
	          psService[lSumindex].lService=lService;
	          psService[lSumindex].lBytes=lBytes;
	           strcpy(psService[lSumindex].caGname,caHost);	           
	          lSumindex++;
	         
	       		lBytes=0;
	       		lService=0;
	       		memset(caHost,0,sizeof(caHost));      
	       		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lBytes,
	                                     		 UT_TYPE_LONG,4,&lService,
	                                     		 UT_TYPE_STRING,63,caHost);
       }                               
      pasDbCloseCursor(psCur); 
  	  
  	}
  printf("lSumindex=%lu\n",lSumindex);   
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
  	    		 		utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%llu",	psService[i].lBytes); 
  	    		 		utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	         		lOth=lOth+psService[i].lBytes;	    		 
  	    		 		utPltPutLoopVar(psDbHead,"name_1",  iNum+1,psUser->dispname);
  	    		 		//if(psService[i].caGname)
  	    		 			utPltPutLoopVar(psDbHead,"host",  iNum+1,psService[i].caGname);
  	    		 		//printf("== 11== name_1=%s\n",psUser->dispname);
  	    		 		//printf("== 11== caGname=%s\n",psService[i].caGname);
  	    		 		if(psUser->psGroup)
  	    		   		utPltPutLoopVar(psDbHead,"name_dept",iNum+1,psUser->psGroup->groupname);
  	    		 		else
  	    		   		utPltPutLoopVar(psDbHead,"name_dept",iNum+1,"");
  	    		    
  	    				if(lSum==0){
    							utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    						}else{
    	  					utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*psService[i].lBytes/lSum);
    						}  	    		  
  	    		//�����ͼ		    	    		
  	    		 		utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	psService[i].lBytes);  
             		utPltPutLoopVar(psDbHead,"name",  iNum+1,psService[i].caGname);
          
             		utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
     						if(lSum==0){
    							utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    						}else{
    	  					utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*psService[i].lBytes/lSum);
    						}
       
            		iNum++;
  	    			}printf("= = iNum=%d\n",iNum);
       			}else{                                                                         //������ͳ��  
  	    		//��������	 
  	    		 	utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%llu",	psService[i].lBytes); 
  	    		 	utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	         	lOth=lOth+psService[i].lBytes;	    		 
  	    		 	utPltPutLoopVar(psDbHead,"name_1",  iNum+1,ncLang("1820ȫ���û�"));
  	    		 	printf("groupid=%d\n",psService[i].lService);
  	    		 	utPltPutLoopVar(psDbHead,"name_dept",iNum+1,ncUtlGetGroupNameByid(psShmHead,psService[i].lService,ncLang("0629δ֪")));
  	    		 	
  	    		 	utPltPutLoopVar(psDbHead,"host",  iNum+1,psService[i].caGname);
  	    		 	//printf("== 22== caGname=%s\n",psService[i].caGname);
  	    		 	if(lSum==0){
    						utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				 	}else{
    	  				utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*psService[i].lBytes/lSum);
    				 	}  	    		  
  	    	 //�����ͼ		  
  	    		
  	    		 	utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	psService[i].lBytes);  
           	 // utPltPutLoopVar(psDbHead,"name",  iNum+1,ncUtlGetGroupNameByid(psShmHead,psService[i].lService,"δ֪"));   
             	utPltPutLoopVar(psDbHead,"name",  iNum+1,psService[i].caGname);        
             	utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);            
     					if(lSum==0){
    						utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    					}else{
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
    				}else{
    	  			utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*(lSum-lOth)/lSum);	  			
    				}
            
             utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",	utStrLtoF8((lSum-lOth),caTemp)); 
  	    		 utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",iNum+1);
  	             		 
  	    		 utPltPutLoopVar(psDbHead,"name_1",  iNum+1,ncLang("0431����"));
  	    		 utPltPutLoopVar(psDbHead,"name_dept",iNum+1,ncLang("0431����"));
  	    		if(lSum==0){
    					utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    				}else{
    	  			utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*(lSum-lOth)/lSum);
    				} 
             
                                     
     }

if(strlen(caPdf)>0){
	//����ǩ
	 desc=malloc(5*sizeof(char*));
	 for(i=0;i<5;i++){
	 	desc[i]=malloc(32);
   }
   strcpy(desc[0],ncLang("0728�û���"));
   strcpy(desc[1],ncLang("0147����"));
   strcpy(desc[2],ncLang("0594��ַ"));
    sprintf(desc[3],"%s(%s)",ncLang("0234���ʴ���"),ncLang("1898��"));
   strcpy(desc[4],ncLang("1827�ٷֱ�%"));
     
 //�����PDF��     
   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*(atol(caTopnum)+3));
    if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("1863�����ڴ����"));
      return 0;
   }
  
   iNum=0;
   
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
   
   	 			pChart[iNum].amount=(float)psService[i].lBytes; 	
  			 if(pChart[iNum].amount==0) pChart[iNum].amount=0.01;
  			 
          	 pChart[iNum].append=malloc(2*sizeof(char*));
          	 for(m=0;m<2;m++){
          			pChart[iNum].append[m]=malloc(32);
          	 }
          	 strcpy(pChart[iNum].append[0],ncUtlGetGroupNameByid(psShmHead,psService[i].lService,ncLang("0629δ֪")));   
          	 strcpy(pChart[iNum].append[1],psService[i].caGname);      
  	 			iNum++;
   	 		}
   	 		}else{
        	 
        	 pChart[iNum].name=strdup(ncUtlGetGroupNameByid(psShmHead,psService[i].lService,ncLang("0629δ֪")));
   	 			
   	 			if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*psService[i].lBytes/lSum);
   	 			}
 	 		
   	 			pChart[iNum].percent=strdup(caTemp); 
   	 			lDate=psService[i].lBytes;
   	 			pChart[iNum].amount=(float)psService[i].lBytes; 	

          pChart[iNum].append=malloc(2*sizeof(char*));
          for(m=0;m<2;m++){
          	pChart[iNum].append[m]=malloc(32);
          }
          strcpy(pChart[iNum].append[0],ncUtlGetGroupNameByid(psShmHead,psService[i].lService,ncLang("0629δ֪")));  
          strcpy(pChart[iNum].append[1],psService[i].caGname);        
  	 			iNum++;
        	
        }        
     }
    }
     
         // �������
     if(lSum-lOth>0){
     		pChart[iNum].name=strdup(ncLang("0431����"));  
     	   	if(lSum==0){
   	 				sprintf(caTemp,"0.00");
   	 			}else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(lSum-lOth)/lSum);
   	 			}  	 			
     	   	pChart[iNum].percent=strdup(caTemp); 
   	 			lDate=lSum-lOth;
   	 			pChart[iNum].amount=lDate; 	

          pChart[iNum].append=malloc(2*sizeof(char*));
          for(m=0;m<2;m++){
          	pChart[iNum].append[m]=malloc(32);
          }
          strcpy(pChart[iNum].append[0],ncLang("0431����")); 
          strcpy(pChart[iNum].append[1],"");                   
          iNum++;                                     
     }
     
      rowrow=iNum;   
    sprintf(filename,"%s.PDF",caTitle); 
    sprintf(caStatdate,ncLang("1952ͳ��ʱ�䣺��%s %s:%s ��%s %s:%s"),	caSdate,caShour,caSmin,caEdate,caEhour,caEmin);	

     ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,5,2, caTitle,ncLang("1961��λ(��)"),caStatdate,filename); 
     
//�ͷ��ڴ�     
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
     free(psService);
     return 0;
    }
    
    free(psService);
    utPltPutVar(psDbHead,"groupid",gp_id);
    utPltPutVar(psDbHead,"userid",us_id);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_website/iframe_servicedisdet.htm");  
    return 0;
}



/* ͳ���Զ�����ַ����*/
int ncWebShowSelfWeb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
    struct ncSflow_s  *sService;

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
    char user_Id[15]="";
    char dpname[32]="";
    char gpname[32]="";
    char rtname[32]="";
    char expexcel[12]="";
    char f_name[120]="";
    char cp_command[320]="";
  	char rm_command[320]="";
  	char caSdate_All[20],caEdate_All[20];
  	FILE *fp; 
  	char group_id[32]="";
  	char webid[20000];
  	char webname[20000];
  	char caSum[12];
  	long lSumShow;
  	long locoindex;
  	char caWorkflag[12];
  	char caWorksql[1024],caWorksql2[1024];
  	
  	pasLHashInfo sHashInfo;
  	struct stData_web{
  		char host[64];
      unsigned long userid;
    	long long bytes;
    	long count;
    };
    struct stKey{
      char host[64];
      unsigned long userid;
    };
    struct stKey psKey;
  	unsigned char *pHash_c;          //��Ű���ַ���������
  	unsigned char *pHash_w;          //��Ű���ַ���û����������
  	struct stData_web *psData,*psData_c;
    struct ncSflow_s  sGservice[200],sGservice_w[200];
  	char caHost[68];
  	ncUserCont *psUser;
  	long lUserid,lCount;
  	long lSumGindex_c;
   lTime=time(0);
   lSum=0;
   lRecenttime=0;
 
   
      utMsgGetSomeNVar(psMsgHead,8,
                                    "sdate",UT_TYPE_STRING,12,caSdate,						//��ʼʱ������
                                    "edate",UT_TYPE_STRING,12,caEdate,
                                    "exppdf",UT_TYPE_STRING,8,caPdf,							//����pdf
                                    "groupid",UT_TYPE_STRING,30,group_id,					//����id
                                    "userid",UT_TYPE_STRING,14,user_Id,				    //�û�id
                                    "expexcel",UT_TYPE_STRING,10,expexcel,				//����pdf	
                                    "sunrec",    UT_TYPE_STRING,8, caSum,
                                    "workflag",UT_TYPE_STRING,8,caWorkflag);				
 
  if(strlen(caSum)==0) strcpy(caSum,"100");
   lSumShow=atol(caSum);
  
  
  
  
  pHash_w = (unsigned char *)pasLHashInit(25000,25000,sizeof(struct stData_web),0,68);    
  if(pHash_w==NULL){
  	    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","pHash�����ڴ����","pHash�����ڴ����");
        return 0;
  }   	  
  
 
   if(strlen(caSdate)==0){
  	   strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime)); 
  	}
  	if(strlen(caEdate)==0){
  	   strcpy(caEdate,utTimFormat("%Y/%m/%d",lTime)); 
  	} 	
  	
   if(strlen(expexcel)==0){	
    psDbHead = utPltInitDb();	  	  
  	 utPltPutVar(psDbHead,"sdate",caSdate);
  	 utPltPutVar(psDbHead,"edate",caEdate);
  	 utPltPutVar(psDbHead,"sunrec",caSum); 
     utPltPutVar(psDbHead,"workflag",caWorkflag); 	
   }
  pHash_c = (unsigned char *)pasLHashInit(1000,1000,sizeof(struct stData_web),0,64); 
  if(pHash_c==NULL){
  	    free(pHash_w);
  	  	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","pHash�����ڴ����","pHash�����ڴ����");
        return 0;
  }
  
  sprintf(caTemp,"select host,userid,sum(bytes),sum(lcount) from nchttpcount where 1=1 ");
  if(strlen(caSdate)>0){
  	sprintf(caTemp+strlen(caTemp)," and sdate>='%s' ",caSdate);
  }
  if(strlen(caEdate)>0){
  	sprintf(caTemp+strlen(caTemp)," and sdate<='%s' ",caEdate);
  }
  if(atol(caWorkflag)==1){
  	sprintf(caTemp+strlen(caTemp)," and worktime=1 ");
  }
   sprintf(caTemp+strlen(caTemp)," and host in (select url from ncweblist where id>=10000)");
   sprintf(caTemp+strlen(caTemp)," group by host,userid ");
   
 printf("caTemp=%s\n",caTemp);
  	 psCur=pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
    	   free(pHash_c);
    	   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746�û���ѯ"),ncLang("0171��ѯ���ݳ���"));
         return 0;
     }
       memset(caHost,0,sizeof(caHost));
       lUserid=0;
       lBytes=0;
       lCount=0;
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_STRING,64,caHost,
      	                             UT_TYPE_LONG,4,&lUserid,
      	                             UT_TYPE_LONG8,8,&lBytes,
      	                             UT_TYPE_LONG,4,&lCount);
      	                             
       while(iReturn==0||iReturn==1405)
       {
       	 strcpy(psKey.host,caHost);
       	 psKey.userid=lUserid;
       	 psData = (struct stData_web *)pasLHashLookA(pHash_w,&psKey);  
       	 if(psData){
       	 	psData->bytes=psData->bytes+lBytes;
       	 	psData->count=psData->count+lCount;
       	 }  
       	
       memset(caHost,0,sizeof(caHost));
       lUserid=0;
       lBytes=0;
       lCount=0;
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_STRING,64,caHost,
      	                             UT_TYPE_LONG,4,&lUserid,
      	                             UT_TYPE_LONG8,8,&lBytes,
      	                             UT_TYPE_LONG,4,&lCount);
       }
      pasDbCloseCursor(psCur); 
  
  
//����ַ���»���
         psData = (struct stData_web *)pasLHashFirst(pHash_w,&sHashInfo);
      	 while(psData){
              psData_c = (struct stData_web *)pasLHashLookA(pHash_c,psData->host);  
              if(psData_c){
              	psData_c->bytes=psData_c->bytes+psData->bytes;
              	psData_c->count=psData_c->count+psData->count;
              }
  		    psData = (struct stData_web *)pasLHashNext(&sHashInfo);
  		   }
  //��ȡ�ܵ����,����ַ�������ݷŵ�sGservice�У�Ϊ������׼��
        psData=(struct stData_web *)pasLHashFirst(pHash_c,&sHashInfo);
        lSumGindex=0;
        lSum=0;
        while(psData){
        	strcpy(sGservice[lSumGindex].caGname,psData->host);

          lSum=lSum+psData->bytes;
        	sGservice[lSumGindex].lBytes=psData->bytes;
        	sGservice[lSumGindex].lGroupid=psData->count;   //����lGroupd��ŵ����
        	psData = (struct stData_web *)pasLHashNext(&sHashInfo);
        	lSumGindex++;
        }
   //���� 
   
   
    if(lSumGindex>0){
  	    qsort(sGservice,lSumGindex,sizeof(struct ncSflow_s),ncUtlSortOnServiceId); 
    }
    
    
    
  if(strlen(expexcel)==0){
    
    
    
    iNum=0;
    for(i=0;i<lSumGindex;i++){
    	if(i<20){
    		   	  //��������	 

  	    		 	utPltPutLoopVarF(psDbHead,"bytes_1",iNum+1,"%s",utStrLtoF8(sGservice[i].lBytes,caTemp)); 
  	    		 	utPltPutLoopVarF(psDbHead,"num_1",iNum+1,"%d",i+1);
  	    		 	utPltPutLoopVar(psDbHead,"name_1",  iNum+1,sGservice[i].caGname);
  	    		 	utPltPutLoopVarF(psDbHead,"count_l",iNum+1,"%d",sGservice[i].lGroupid);
  	    		 	if(lSum==0){
    						utPltPutLoopVar(psDbHead,"rate_1",iNum+1,"0.00");
    					}else{
    	  				utPltPutLoopVarF(psDbHead,"rate_1",iNum+1,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
    					}
    			//�����ͼ	
    		  	    utPltPutLoopVarF(psDbHead,"bytes",iNum+1,"%llu",	sGservice[i].lBytes);  
             		utPltPutLoopVar(psDbHead,"name",  iNum+1,sGservice[i].caGname);
             		utPltPutLoopVarF(psDbHead,"groupid",iNum+1,"%d",sGservice[i].lService);
             		utPltPutLoopVarF(psDbHead,"ltime",iNum+1,"%d",0);
             		utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",i);
           
     						if(lSum==0){
    							utPltPutLoopVar(psDbHead,"rate",iNum+1,"0.00");
    						}else{
    	  					utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)100*sGservice[i].lBytes/lSum);
    						}
    						
    			//�����ϸ	
    					lSumGindex_c=0;
    				psData = (struct stData_web *)pasLHashFirst(pHash_w,&sHashInfo);
      	    while(psData){
      	 
      	    	if(strcmp(sGservice[i].caGname,psData->host)==0){
      	    		  sGservice_w[lSumGindex_c].lBytes=psData->bytes;
      	    		  sGservice_w[lSumGindex_c].lGroupid=psData->count;
      	    		  sGservice_w[lSumGindex_c].lService=psData->userid;
      	    		  lSumGindex_c++;
      	    	}
    					psData = (struct stData_web *)pasLHashNext(&sHashInfo);
    				}
    				  //����   
					    if(lSumGindex_c>0){
					  	    qsort(sGservice_w,lSumGindex_c,sizeof(struct ncSflow_s),ncUtlSortOnServiceId); 
					    }
    					//�����ϸ
    					for(m=0;m<lSumGindex_c;m++){
    						if(m<lSumShow){
    							psUser = ncUtlGetUserContById(psShmHead,sGservice_w[m].lService);
           		    if(psUser) {    
           		    	if(psUser->psGroup)
		  	    		   		utPltPutLoopVar2(psDbHead,"dept_u",iNum+1,m+1,psUser->psGroup->groupname);
		  	    		 		else
		  	    		   		utPltPutLoopVar2(psDbHead,"dep_u",iNum+1,m+1,"");
		  	    		   		
		  	    		   		utPltPutLoopVar2(psDbHead,"name_u",iNum+1,m+1,psUser->dispname);
		  	    		   		utPltPutLoopVar2(psDbHead,"ip_u",iNum+1,m+1,utComHostIp(htonl(psUser->ip)));
		  	    		   		utPltPutLoopVar2(psDbHead,"mac_u",iNum+1,m+1,ncCvtMac(psUser->mac));
    						  }
    							
    							utPltPutLoopVarF2(psDbHead,"num_u",iNum+1,m+1,"%d",m+1);
    							utPltPutLoopVar2(psDbHead,"bytes_u",iNum+1,m+1,utStrLtoF8(sGservice_w[m].lBytes,caTemp));
    							utPltPutLoopVarF2(psDbHead,"count_u",iNum+1,m+1,"%d",sGservice_w[m].lGroupid);
    						}
    					}
    						
    						
    						
    		    iNum++;
    	  }
     }
  	    		free(pHash_w);
  	    		free(pHash_c);
  	   			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc75_website/iframe_selfwebbt.htm"); 
  	   			return 0;	  
  	   }
  	   else{
  	   	 	 char caFilename[32];
		        char caFile[128];
		        char caPath[128];
		        char caReportname[128];
		        char caGroup[32],caDispname[32],caSip[20],caMac[20];
		        sprintf(caFilename,"webreport%d.csv",time(0));
		        sprintf(caPath,"%s","/home/ncmysql/nc/maillog/tmp");
		        sprintf(caFile,"%s/%s",caPath,caFilename);
			  		fp = fopen(caFile,"w");
			  		if(fp == NULL) {
			            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613��������ͳ��"),ncLang("0546���ݵ�������"));
			            return 0;
			        }
  	       	fprintf(fp,"�Զ�����վ����ͳ�Ʊ���(%s--%s)\n",caSdate,caEdate);
  	   	    fprintf(fp,"��ַ,����,����,IP��ַ,MAC��ַ,����(�ֽ�),������\n");

  	        for(i=0;i<lSumGindex;i++){  
  	        	fprintf(fp,"%s,,,,,%s,%d\n",sGservice[i].caGname,utStrLtoF8(sGservice[i].lBytes,caTemp),sGservice[i].lGroupid);
  	        	    			//�����ϸ	
    					lSumGindex_c=0;
    					psData = (struct stData_web *)pasLHashFirst(pHash_w,&sHashInfo);
      	    	while(psData){
      	 
      	    		if(strcmp(sGservice[i].caGname,psData->host)==0){
      	    		  	sGservice_w[lSumGindex_c].lBytes=psData->bytes;
      	    		  	sGservice_w[lSumGindex_c].lGroupid=psData->count;
      	    		  	sGservice_w[lSumGindex_c].lService=psData->userid;
      	    		  	lSumGindex_c++;
      	    		}
    						psData = (struct stData_web *)pasLHashNext(&sHashInfo);
    					}
    				  //����   
					    if(lSumGindex_c>0){
					  	    qsort(sGservice_w,lSumGindex_c,sizeof(struct ncSflow_s),ncUtlSortOnServiceId); 
					    }
    					//�����ϸ
    					for(m=0;m<lSumGindex_c;m++){

  	        	    psUser = ncUtlGetUserContById(psShmHead,sGservice_w[m].lService);
  	        	    memset(caGroup,0,sizeof(caGroup));
  	        	    memset(caDispname,0,sizeof(caDispname));
  	        	    memset(caSip,0,sizeof(caSip));
  	        	    memset(caMac,0,sizeof(caMac));
           		    if(psUser) {
           		    	if(psUser->psGroup){
           		    		strcpy(caGroup,psUser->psGroup->groupname);
           		    	}
           		    	else{
           		    		strcpy(caGroup,"");
           		    	}
  	        	      strcpy(caDispname,psUser->dispname);
  	        	      strcpy(caSip,utComHostIp(htonl(psUser->ip)));
  	        	      strcpy(caMac,ncCvtMac(psUser->mac));
  	        	    }
  	        	    fprintf(fp," ,%s,%s,%s,%s,%s,%d\n",caDispname,caGroup,caSip,caMac,utStrLtoF8(sGservice_w[m].lBytes,caTemp),sGservice_w[m].lGroupid);
  	        	
  	            } 
  	          }	    
  	   		  fclose(fp);
	          fflush(fp);
  	   	
  	   	  	free(pHash_w);
  	    		free(pHash_c);
  	    		
  	    		utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
            remove(caFile);
  	    		
  	    		
  	    		return 0;
  	  }	
	  	
  	  	
 
   

}
