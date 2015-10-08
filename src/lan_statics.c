#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "pasdb.h"
#include "pdflib.h"
#include "utoplt01.h"

#define GYS 1

void proc1_combine_table(char *table_name,char *sdatet,char *edatet){
  char sdate[24] = "";
  char edate[24] = "";
  
  strcpy(sdate,sdatet);
  strcpy(edate,edatet);
  
	sdate[4] = '\0';
	edate[4] = '\0';
	
	sdate[7] = '\0';
	edate[7] = '\0';
	
	char sql[64] = "";
	
	int syear = atoi(sdate);
	int smonth = atoi(sdate+5);
	int eyear = atoi(edate);
	int emonth = atoi(edate+5);
	
	if(syear==eyear && smonth==emonth)
				snprintf(table_name+strlen(table_name),1024-strlen(table_name),"nwproclog_%4u%02u as new_table",syear,smonth);
	else{
			int i,j=smonth,iNum=0;
			strcat(table_name,"(");
			
			pasDbCursor *psCur = NULL;
			for(i=syear;i<=eyear&&j<=emonth;){
					snprintf(sql,sizeof(sql),"select * from nwproclog_%4u%02u",i,j);
					psCur = pasDbOpenSql(sql,0);
					if(psCur!=NULL){
							if(iNum>0){
									strcat(table_name," union all ");
							}
							iNum++;
							snprintf(table_name+strlen(table_name),1024-strlen(table_name),sql);
							pasDbCloseCursor(psCur);
					}
					
					if(j>11){
							i++;
							j=0;
					}
					j++;
			}
			snprintf(table_name+strlen(table_name),1024-strlen(table_name),")new_table");
	}
	printf("table_name:%s\n",table_name);
	
	
}

void compute1_combine_table(char *table_name,char *sdatet,char *edatet){
  char sdate[24] = "";
  char edate[24] = "";
  
  strcpy(sdate,sdatet);
  strcpy(edate,edatet);
  
	sdate[4] = '\0';
	edate[4] = '\0';
	
	sdate[7] = '\0';
	edate[7] = '\0';
	
	char sql[64] = "";
	
	int syear = atoi(sdate);
	int smonth = atoi(sdate+5);
	int eyear = atoi(edate);
	int emonth = atoi(edate+5);
	
	if(syear==eyear && smonth==emonth)
				snprintf(table_name+strlen(table_name),1024-strlen(table_name),"nwcomputelog_%4u%02u as new_table",syear,smonth);
	else{
			int i,j=smonth,iNum=0;
			strcat(table_name,"(");
			
			pasDbCursor *psCur = NULL;
			for(i=syear;i<=eyear&&j<=emonth;){
					snprintf(sql,sizeof(sql),"select * from nwcomputelog_%4u%02u",i,j);
					psCur = pasDbOpenSql(sql,0);
					if(psCur!=NULL){
							if(iNum>0){
									strcat(table_name," union all ");
							}
							iNum++;
							snprintf(table_name+strlen(table_name),1024-strlen(table_name),sql);
							pasDbCloseCursor(psCur);
					}
					
					if(j>11){
							i++;
							j=0;
					}
					j++;
			}
			snprintf(table_name+strlen(table_name),1024-strlen(table_name),")new_table");
	}
	printf("table_name:%s\n",table_name);
	
	
}

void web1_combine_table(char *table_name,char *sdatet,char *edatet){
  char sdate[24] = "";
  char edate[24] = "";
  
  strcpy(sdate,sdatet);
  strcpy(edate,edatet);
  
	sdate[4] = '\0';
	edate[4] = '\0';
	
	sdate[7] = '\0';
	edate[7] = '\0';
	
	char sql[64] = "";
	
	int syear = atoi(sdate);
	int smonth = atoi(sdate+5);
	int eyear = atoi(edate);
	int emonth = atoi(edate+5);
	
	if(syear==eyear && smonth==emonth)
				snprintf(table_name+strlen(table_name),1024-strlen(table_name),"nwweblog_%4u%02u as new_table",syear,smonth);
	else{
			int i,j=smonth,iNum=0;
			strcat(table_name,"(");
			
			pasDbCursor *psCur = NULL;
			for(i=syear;i<=eyear&&j<=emonth;){
					snprintf(sql,sizeof(sql),"select * from nwweblog_%4u%02u",i,j);
					psCur = pasDbOpenSql(sql,0);
					if(psCur!=NULL){
							if(iNum>0){
									strcat(table_name," union all ");
							}
							iNum++;
							snprintf(table_name+strlen(table_name),1024-strlen(table_name),sql);
							pasDbCloseCursor(psCur);
					}
					
					if(j>11){
							i++;
							j=0;
					}
					j++;
			}
			snprintf(table_name+strlen(table_name),1024-strlen(table_name),")new_table");
	}
	printf("table_name:%s\n",table_name);
	
	
}

int lanWebStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	
	char groupid_in[16] = "";
	char compid_in[16] = "";
	char bdate_in[24] = "";
	char edate_in[24] = "";
	
	utMsgGetSomeNVar(psMsgHead,4,
		"groupid",UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in,
		"compid", UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"sdate",  UT_TYPE_STRING, sizeof(bdate_in)-1,   bdate_in,
		"edate",  UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in);
	
	char sql[512] = "";
	char sql_tmp[256] = "";
	
	unsigned long stime=utTimStrToLong("%Y-%m-%d", bdate_in);
	unsigned long etime=utTimStrToLong("%Y-%m-%d", edate_in)+86400;
	
	if(!utStrIsSpaces(groupid_in)){
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp)," and groupid in(%s)",getGroupid(strtoul(groupid_in,NULL,10)));
	}
	if(!utStrIsSpaces(compid_in)){
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp)," and compid=%s",compid_in);
	}
	if(!utStrIsSpaces(bdate_in)){
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp)," and stime > %lu",stime);
	}
	if(!utStrIsSpaces(edate_in)){
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp)," and stime < %lu",etime);
	}
	
	char tables[256] = "";
	
	web1_combine_table(tables,bdate_in,edate_in);
	
	sprintf(sql,"select count(sid) as num,host from %s where host<>'' %s group by host order by num desc limit 10",tables,sql_tmp);
	
	pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
	
	writeLog("webStatics.txt","websearch",sql);
	
	psCur = pasDbOpenSql(sql, 0);
	
	if(psCur!=NULL){
		unsigned int count = 0;
		char host[32] = "";
		
		int iret = 0,iNum = 0;
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(count), 	&count,
			 UT_TYPE_STRING, sizeof(host)-1,  host)) || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"ranking",iNum,"%u",iNum);
			utPltPutLoopVarF(psDbHead,"clickNum",iNum,"%u",count);
			utPltPutLoopVar(psDbHead,"host",iNum,host);
		}
		
		pasDbCloseCursor(psCur);
	}
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_webstatic.htm");
	
	return 0;
}

int lanPesWebStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	
	utMsgPrintMsg(psMsgHead);
	
	char deal_in[8] = "";
	unsigned long compids[10]={0};
	unsigned long counts[10]={0};
	char groupid_in[16] = "";
	char compid_in[16] = "";
	char bdate_in[24] = "";
	char edate_in[24] = "";
	
	memset(compids,0,sizeof(compids));
	memset(counts,0,sizeof(counts));
	memset(deal_in,0,sizeof(deal_in));
		
	utMsgGetSomeNVar(psMsgHead,5,
		"deal",UT_TYPE_STRING,sizeof(deal_in)-1,deal_in,
		"compid",UT_TYPE_STRING,sizeof(compid_in)-1,compid_in,
		"groupid",UT_TYPE_STRING,sizeof(groupid_in)-1,groupid_in,
		"sdate",UT_TYPE_STRING,sizeof(bdate_in)-1,bdate_in,
		"edate",UT_TYPE_STRING,sizeof(edate_in)-1,edate_in);
		
	char sql[512] = "";
	char sql_tmp[256] = "";
	
	if(!utStrIsSpaces(groupid_in)){
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp)," and new_table.groupid in (%s)",getGroupid(strtoul(groupid_in,NULL,10)));
	}
	if(!utStrIsSpaces(compid_in)){
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp)," and new_table.compid = %s",compid_in);
	}
	if(!utStrIsSpaces(bdate_in)){
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp)," and new_table.stime > %lu",utTimStrToLong("%Y-%m-%dT%H:%M:%S", bdate_in));
	}
	if(!utStrIsSpaces(edate_in)){
		snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp)," and new_table.stime < %lu",utTimStrToLong("%Y-%m-%dT%H:%M:%S", edate_in)+86400);
	}
	
	char tables[256] = "";
	
	web1_combine_table(tables,bdate_in,edate_in);
	
	sprintf(sql,"select count(*) as sum,compid from %s where 1=1 and host<>'' %s group by compid order by sum desc limit 10",tables,sql_tmp);
	
	pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
	int i = 0;
	
	writeLog("webStatics.txt","pessearch",sql);
	psCur = pasDbOpenSql(sql, 0);
	
	if(psCur!=NULL){
		unsigned long count1 = 0;
		unsigned long compid = 0;
		
		int iret = 0;
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_ULONG, sizeof(count1), 	&count1,
			 UT_TYPE_ULONG, sizeof(compid),&compid)) || 1405==iret){
			
			compids[i] = compid;
			counts[i]  = count1;
			
			i++;
		}
		
		pasDbCloseCursor(psCur);
	}
	
	int iret=-1,iNum = 0,j=0,oldj=-1;
	for(i = 0;i<10;i++){
		if(compids[i]==0){
			break;
		}
		
		sprintf(sql,"select count(sid) as sum,host,nwcompute.devname from %s left join nwcompute on nwcompute.compid=new_table.compid where 1=1 and host<>'' %s and new_table.compid = %lu group by host order by sum desc limit 10",tables,sql_tmp,compids[i]);
	
    writeLog("webStatics.txt","mainSearch",sql);
		psCur = pasDbOpenSql(sql, 0);
	
		if(psCur!=NULL){
			unsigned int count = 0;
			char host[128] = "";
			char compname[32] = "";
			
			while(0==(iret = pasDbFetchInto(psCur,
				 UT_TYPE_ULONG, sizeof(count), 	&count,
				 UT_TYPE_STRING, sizeof(host)-1, 	host,
				 UT_TYPE_STRING, sizeof(compname)-1,  compname)) || 1405==iret){
				j++;
				
				if(atoi(deal_in)==1){
					//printf("adsfasdf\n");
					if(oldj!=i){
						oldj=i;
						if(i>0){
							utPltPutLoopVar(psDbHead,"dh",i+1,",");
						}
						utPltPutLoopVar(psDbHead,"compname",i+1,compname);
						utPltPutLoopVarF(psDbHead,"clickNum",i+1,"%u",counts[i]);
					}
				}else{				
					iNum++;
					if(iNum>1){
						utPltPutLoopVar(psDbHead,"dh",iNum,",");
					}
					
					utPltPutLoopVarF(psDbHead,"compname",iNum,"排名:%02d,电脑:%s,总点击数:%u",i+1,compname,counts[i]);
					utPltPutLoopVarF(psDbHead,"ranking",iNum,"%u",j);
					utPltPutLoopVarF(psDbHead,"clickNum",iNum,"%u",count);
					utPltPutLoopVar(psDbHead,"host",iNum,host);
				}
			}
			
			pasDbCloseCursor(psCur);
		}
	}
	
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_webstatic.htm");
	
	return 0;
}

typedef struct data{
  char proctype[128];
  char procname[64];
  char devname[64];
  unsigned long runtime;
  unsigned long nettime;
  unsigned long activetime;
}Data;

typedef struct node{
  Data data;
  struct node *next;
}Node;

Node *insertNode(Node *Head,Data *data){
  if(Head==NULL||data==NULL){
    return NULL;
  }
  Node *pNode=Head;
  
  Node *newNode = (Node*)malloc(sizeof(Node));
  if(newNode==NULL){
    return NULL;
  }
  
  newNode->data=*data;
  newNode->next=NULL;
  
  while(pNode->next){
    pNode=pNode->next;
  }
  
  pNode->next=newNode;
  
  return newNode;
}

void destroyNode(Node *Head){
  if(Head==NULL){
    return ;
  }
  Node *pNode = Head->next;
  
  while(pNode){
    pNode=pNode->next;
    free(pNode);
  }
  
  free(Head);
}

void showNode(Node *Head){
  if(Head==NULL){
    return ;
  }
  
  Node *pNode = Head->next;
  
  while(pNode){
    printf("%s\t%s\t%s\t%lu\t%lu\t\%lu\n",pNode->data.proctype,pNode->data.procname,pNode->data.devname,pNode->data.runtime,pNode->data.nettime,pNode->data.activetime);
    pNode=pNode->next;
  }
  
}

int lanProcessStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	
	char style_in[8] = "";
	char groupid_in[16] = "";
	char compid_in[16] = "";
	char num_in[16] = "";
	char sdate_in[24] = "";
	char edate_in[24] = "";
	int num = 0;
	
	utMsgGetSomeNVar(psMsgHead,6,
		"style",UT_TYPE_STRING,sizeof(style_in)-1,style_in,
		"num",UT_TYPE_STRING,sizeof(num_in)-1,num_in,
		"groupid",UT_TYPE_STRING,sizeof(groupid_in)-1,groupid_in,
		"compid",UT_TYPE_STRING,sizeof(compid_in)-1,compid_in,
		"sdate",UT_TYPE_STRING,sizeof(sdate_in)-1,sdate_in,
		"edate",UT_TYPE_STRING,sizeof(edate_in)-1,edate_in);
	
	char sdates[24] = "";
	char edates[24] = "";
	char tables[256] = "";
	
	strcpy(sdates,utTimFormat("%Y/%m/%d", strtoul(sdate_in,NULL,10)));
	strcpy(edates,utTimFormat("%Y/%m/%d", strtoul(edate_in,NULL,10)));
	
	proc1_combine_table(tables,sdates,edates);
	
	num = atoi(num_in);
	if(num==0){
	  num = 10;
	}
	
	utPltDbHead *psDbHead = utPltInitDb();
	
	char sql[512] = "";
	char sql_tmp[128] = "";
	char sql_tmp1[128] = "";
	
	char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
	
	//printf("11111111111111\n");
	
	//取忽略表数据
  unsigned int irecSum = 0;
  char caResult[40960] = "";
	
	sprintf(sql,"select processname from nwprocignore where company='%s'",shortname);
	pasDbRecordSet(sql,1,9999,&irecSum,caResult,0);
	
	char *authgroup = getDsGroupid();
	
	memset(sql,0,sizeof(sql));
	memset(sql_tmp1,0,sizeof(sql_tmp1));
	memset(sql_tmp,0,sizeof(sql_tmp));
	switch(atoi(style_in)){
	  case 1: 
	    strcpy(sql_tmp,",bsum desc");
	    break;
	  case 2:
	    strcpy(sql_tmp,",wsum desc");
	    break;
	  case 3:
	    strcpy(sql_tmp,",nsum desc");
	    break;
	  default:
	    strcpy(sql_tmp,",bsum desc");
	    break;
	}
	//printf("22222222222222222\n");
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp1," and compid = %s",compid_in);
	}else if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp1," and groupid = %s",groupid_in);
	}else{
	  sprintf(sql_tmp1+strlen(sql_tmp1)," and groupid in (%s)",authgroup);
	}
	
	if(!utStrIsSpaces(sdate_in)){
	  sprintf(sql_tmp1+strlen(sql_tmp1)," and stime >= %s",sdate_in);
	}
	
	//printf("333333333333333333\n");
	if(!utStrIsSpaces(edate_in)){
	  sprintf(sql_tmp1+strlen(sql_tmp1)," and stime <= %s",edate_in);
	}
	pasDbCursor *psCur = NULL;
	
	//printf("4444444444444\n");
  sprintf(sql,"select nwprocStaticsType.statstype,devname,procname,sum(btime) as bsum,sum(nettime) as nsum,sum(wndtime) as wsum from %s left join nwprocStatics on procname=processname and nwprocStatics.company='%s' and nwprocStatics.statstype<>0 left join nwprocStaticsType on nwprocStaticsType.recno=nwprocStatics.statstype where btime>wndtime and btime>nettime %s group by procname order by nwprocStaticsType.recno desc%s",tables,shortname,sql_tmp1,sql_tmp);
  
  writeLog("procStatics.txt","mainSearch",sql);
  
  psCur = pasDbOpenSql(sql, 0);
  
  Node *Head = (Node*)malloc(sizeof(Node));
  Head->next=NULL;
  
  
	char runtimes[16] = "";
	char nettimes[16] = "";
	char activetimes[16] = "";
	char typename[5][10]={"办公软件","邮件处理","网页浏览","视频查看","聊天活动"};
	
	int tmp = 0;
  char proctypeOld[128] = "";
  char procname16[512] = "";
	
	int iret = 0;
	int iNum = 0;
	char iNumS[8] = "";
	
  int i = 0;//计数用
  if(psCur!=NULL){		
		Data data;
		memset(&data,0,sizeof(data));
		while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_STRING,sizeof(data.proctype)-1, data.proctype,
        UT_TYPE_STRING,sizeof(data.devname)-1,  data.devname,
        UT_TYPE_STRING,sizeof(data.procname)-1, data.procname,
        UT_TYPE_ULONG, sizeof(data.runtime),    &data.runtime,
        UT_TYPE_ULONG, sizeof(data.nettime),    &data.nettime,
        UT_TYPE_ULONG, sizeof(data.activetime), &data.activetime)) || 1405==iret){
      
     // writeLog("ignoreProc.txt","caResult",caResult);
			if(strstr(caResult,data.procname)){
			  memset(&data,0,sizeof(data));
			  continue;
			}
          
			if(utStrIsSpaces(data.proctype)){
			  strcpy(data.proctype,"其他");
			}
			
			insertNode(Head,&data);
			memset(&data,0,sizeof(data));
			
		}
    pasDbCloseCursor(psCur);
  }
  //printf("5555555555555555555\n");
  showNode(Head);
  
  Node *pNode = Head;
  Node *qNode = Head;
  Node *tmpNode=NULL;
  while(pNode->next){
    //printf("1");
    if(!strcmp(pNode->next->data.devname,"8500")){
      strcpy(pNode->next->data.proctype,"办公软件");
      qNode = Head;
      while(qNode->next){
        if(!strcmp(qNode->next->data.proctype,"办公软件") && qNode!=pNode->next){
          if(atoi(style_in)==1){
      	    if(pNode->next->data.runtime>qNode->next->data.runtime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else if(atoi(style_in)==2){
        	  if(pNode->next->data.nettime>qNode->next->data.nettime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else if(atoi(style_in)==3){
        	  if(pNode->next->data.activetime>qNode->next->data.activetime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else{
        	  if(pNode->next->data.runtime>qNode->next->data.runtime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}
        }
        qNode=qNode->next;
      }
    }else if(!strcmp(pNode->next->data.devname,"8400")){
      strcpy(pNode->next->data.proctype,"邮件处理");
      qNode = Head;
      while(qNode->next){
        if(!strcmp(qNode->next->data.proctype,"邮件处理") && qNode!=pNode->next){
          if(atoi(style_in)==1){
      	    if(pNode->next->data.runtime>qNode->next->data.runtime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else if(atoi(style_in)==2){
        	  if(pNode->next->data.nettime>qNode->next->data.nettime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else if(atoi(style_in)==3){
        	  if(pNode->next->data.activetime>qNode->next->data.activetime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else{
        	  if(pNode->next->data.runtime>qNode->next->data.runtime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}
        }
        qNode=qNode->next;
      }
    }else if(!strcmp(pNode->next->data.devname,"8300")){
      strcpy(pNode->next->data.proctype,"网页浏览");
      qNode = Head;
      while(qNode->next){
        if(!strcmp(qNode->next->data.proctype,"网页浏览") && qNode!=pNode->next){
          if(atoi(style_in)==1){
      	    if(pNode->next->data.runtime>qNode->next->data.runtime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else if(atoi(style_in)==2){
        	  if(pNode->next->data.nettime>qNode->next->data.nettime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else if(atoi(style_in)==3){
        	  if(pNode->next->data.activetime>qNode->next->data.activetime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else{
        	  if(pNode->next->data.runtime>qNode->next->data.runtime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}
        }
        qNode=qNode->next;
      }
    }else if(!strcmp(pNode->next->data.devname,"8200")){
      strcpy(pNode->next->data.proctype,"视频查看");
      qNode = Head;
      while(qNode->next){
        if(!strcmp(qNode->next->data.proctype,"视频查看") && qNode!=pNode->next){
          if(atoi(style_in)==1){
      	    if(pNode->next->data.runtime>qNode->next->data.runtime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else if(atoi(style_in)==2){
        	  if(pNode->next->data.nettime>qNode->next->data.nettime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else if(atoi(style_in)==3){
        	  if(pNode->next->data.activetime>qNode->next->data.activetime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else{
        	  if(pNode->next->data.runtime>qNode->next->data.runtime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}
        }
        qNode=qNode->next;
      }
    }else if(!strcmp(pNode->next->data.devname,"8100")){
      strcpy(pNode->next->data.proctype,"聊天活动");
      qNode = Head;
      while(qNode->next){
        if(!strcmp(qNode->next->data.proctype,"聊天活动") && qNode!=pNode->next){
          if(atoi(style_in)==1){
      	    if(pNode->next->data.runtime>qNode->next->data.runtime){
      	      //printf("here-----------%lu\t---------%lu\n",pNode->next->data.runtime,qNode->next->data.runtime);
      	      //printf("before=======pNode=%p\tpNode->next=%p\tpNode->next->next=%p\n",pNode,pNode->next,pNode->next->next);
      	      //printf("before=======qNode=%p\tqNode->next=%p\tqNode->next->next=%p\n",qNode,qNode->next,qNode->next->next);
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      //printf("after=======pNode=%p\tpNode->next=%p\tpNode->next->next=%p\n",pNode,pNode->next,pNode->next->next);
      	      //printf("after=======qNode=%p\tqNode->next=%p\tqNode->next->next=%p\n",qNode,qNode->next,qNode->next->next);
      	      break;
      	    }
        	}else if(atoi(style_in)==2){
        	  if(pNode->next->data.nettime>qNode->next->data.nettime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else if(atoi(style_in)==3){
        	  if(pNode->next->data.activetime>qNode->next->data.activetime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}else{
        	  if(pNode->next->data.runtime>qNode->next->data.runtime){
      	      tmpNode=pNode->next;
      	      pNode->next = pNode->next->next;
      	      tmpNode->next= qNode->next;
      	      qNode->next = tmpNode;
      	      break;
      	    }
        	}
        }
        qNode=qNode->next;
      }
    }
    pNode = pNode->next;
  }
  
  //printf("6666666666666666666\n");
  //showNode(Head);
  
  pNode = Head->next;
  while(pNode){
    iNum++;
			
		if(!strcmp(proctypeOld,pNode->data.proctype)){
		  tmp++;
		  if(tmp>num-1){
		    pNode=pNode->next;
		    iNum--;
		    continue;
		  }
		}else{
		  tmp=0;
		  /*
		  while(i<5&&strcmp(pNode->data.proctype,typename[i])){
		    if(iNum>1){
			    utPltPutLoopVar(psDbHead,"dh",iNum,",");
			  }
  			utPltPutLoopVar(psDbHead,"typename",iNum,typename[i]);
  			utPltPutLoopVar(psDbHead,"procname",iNum,"-");
  			utPltPutLoopVar(psDbHead,"runtime",iNum,"-");
  			utPltPutLoopVar(psDbHead,"activetime",iNum,"-");
  			utPltPutLoopVar(psDbHead,"nettime",iNum,"-");
		    i++;
		    iNum++;
		  }
		  */
		  i++;
		 
		  strcpy(proctypeOld,pNode->data.proctype);
		}
		
		if(iNum>1){
	    utPltPutLoopVar(psDbHead,"dh",iNum,",");
	  }
		
		pasStrCvtBin2Hex(pNode->data.procname,strlen(pNode->data.procname),procname16);
		utPltPutLoopVar(psDbHead,"typename",iNum,pNode->data.proctype);
		utPltPutLoopVar(psDbHead,"procname",iNum,pNode->data.procname);
		utPltPutLoopVar(psDbHead,"procname16",iNum,procname16);
		utPltPutLoopVarF(psDbHead,"runtime",iNum,"%lu",pNode->data.runtime);
		utPltPutLoopVarF(psDbHead,"activetime",iNum,"%lu",pNode->data.activetime);
		utPltPutLoopVarF(psDbHead,"nettime",iNum,"%lu",pNode->data.nettime);
		pNode=pNode->next;
  }
  
	destroyNode(Head);
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_procstatic.htm");
	
	return 0;
}

int lanSearchProcType(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  utMsgPrintMsg(psMsgHead);
  
  char deal_in[16] = "";
  char id_in[16]  = "";
	char statstype_in[32] = "";
  
  utMsgGetSomeNVar(psMsgHead,3,
		"deal",     UT_TYPE_STRING, sizeof(deal_in)-1,  deal_in,
		"id",       UT_TYPE_STRING, sizeof(id_in)-1,    id_in,
		"name",     UT_TYPE_STRING, sizeof(statstype_in)-1,statstype_in);
  
  char sql[256] = "";
  
  char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
  
  if(!strcmp(deal_in,"insert")){
    sprintf(sql,"insert nwprocStaticsType(statstype,company)values('%s','%s')",utf8convert(psShmHead,statstype_in),shortname);
  }else if(!strcmp(deal_in,"update")){
    sprintf(sql,"update nwprocStaticsType set statstype='%s' where recno=%s",utf8convert(psShmHead,statstype_in),id_in);
  }else if(!strcmp(deal_in,"delete")){
    sprintf(sql,"delete from nwprocStaticsType where recno=%s",id_in);
    pasDbExecSqlF(sql);
    sprintf(sql,"update nwprocStatics set statstype=0 where statstype=%s",id_in);
  }
  writeLog("procStatics.txt","insert",sql);
  pasDbExecSqlF(sql);
  
  sprintf(sql,"select recno,statstype from nwprocStaticsType where company = '%s' or company='' order by recno desc",shortname);
  
	pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
  
  printf("ad3\n");
	psCur = pasDbOpenSql(sql, 0);
  if(psCur!=NULL){
    char statstype[32] = "";
		unsigned long recno = 0;
		
		int iret = 0;
		int iNum = 0;
		while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_ULONG,  sizeof(recno), 	    &recno,
        UT_TYPE_STRING, sizeof(statstype)-1,  statstype)) || 1405==iret){
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"id",iNum,"%lu",recno);
			utPltPutLoopVar(psDbHead,"text",iNum,statstype);
			utPltPutLoopVar(psDbHead,"leaf",iNum,"true");			
    }
    pasDbCloseCursor(psCur);
  }
  
  utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_proctypestatic.htm");
	
  return 0;
}

int lanSearchProc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  utMsgPrintMsg(psMsgHead);
  
	char deal_in[16] = "";
	char name_in[256] = "";
	char detail_in[128] = "";
	char statstype_in[16] = "";
	char oldtype_in[16] = "";
	char id_in[16] = "";
	
	utMsgGetSomeNVar(psMsgHead,6,
		"deal",   UT_TYPE_STRING, sizeof(deal_in)-1,  deal_in,
		"id",     UT_TYPE_STRING, sizeof(id_in)-1,    id_in,
		"statstype",UT_TYPE_STRING, sizeof(statstype_in)-1,statstype_in,
		"oldtype",UT_TYPE_STRING, sizeof(oldtype_in)-1,oldtype_in,
		"name",   UT_TYPE_STRING, sizeof(name_in)-1,  name_in,
		"detail", UT_TYPE_STRING, sizeof(detail_in)-1,detail_in);
		
		
  char procname[256] = "";
  pasStrCvtHex2Bin(name_in,procname);

  char sql[512] = "";
  
  char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
  
  if(!strcmp(deal_in,"insert")){
    sprintf(sql,"insert nwprocStatics(statstype,processname,content,company)values(%s,'%s','%s','%s')",statstype_in,procname,detail_in,shortname);
  }else if(!strcmp(deal_in,"update")){
    sprintf(sql,"update nwprocStatics set processname='%s',content='%s' where recno=%s",procname,utf8convert(psShmHead,detail_in),id_in);
  }else if(!strcmp(deal_in,"move")){
    sprintf(sql,"update nwprocStatics set statstype=%s where recno=%s",statstype_in,id_in);
    strcpy(statstype_in,oldtype_in);
  }else if(!strcmp(deal_in,"delete")){
    sprintf(sql,"update nwprocStatics set statstype=0 where recno=%s",id_in);
  }
  writeLog("procStatics.txt","detail",sql);
  int iret = pasDbExecSqlF(sql);
  if(iret!=0){
    sprintf(sql,"update nwprocStatics set statstype=%s where processname='%s' and company='%s'",statstype_in,procname,shortname);
     writeLog("procStatics.txt","detail2",sql);
    pasDbExecSqlF(sql);
  }
  
  sprintf(sql,"select recno,statstype,processname,content from nwprocStatics where (company='%s' or company='') and statstype=%s and processname not in(select processname from nwprocignore where company='%s') group by processname",shortname,statstype_in,shortname);
  
  pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
  
  writeLog("procStatics.txt","select",sql);
	psCur = pasDbOpenSql(sql, 0);
  if(psCur!=NULL){
    char typename[64] = "";
		unsigned long recno = 0;
		unsigned long statstype = 0;
    char detail[128] = "";
		
		int iret = 0;
		int iNum = 0;
		while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_ULONG,  sizeof(recno), 	    &recno,
        UT_TYPE_ULONG,  sizeof(statstype), 	&statstype,
        UT_TYPE_STRING, sizeof(typename)-1, typename,
        UT_TYPE_STRING, sizeof(detail)-1,   detail)) || 1405==iret){
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			} printf("ad2\n");
			utPltPutLoopVarF(psDbHead,"id",iNum,"%lu",recno);
			utPltPutLoopVar(psDbHead,"name",iNum,typename);
			utPltPutLoopVarF(psDbHead,"statstype",iNum,"%lu",statstype);	
			utPltPutLoopVar(psDbHead,"detail",iNum,detail);			
    }
    pasDbCloseCursor(psCur);
  }
  
  utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_procnamestatic.htm");
	
  return 0;
}

void checkMacRun(){
  char buff[32] = "";
  char path[] = "/home/ncmysql/nw/html/conf/nw.cnf";
  char sql[512] = "";
  char tables[1024]="";
  
  time_t now = time(0);
  char nows[16] = "";
  char nows2[16] = "";
  strcpy(nows,utTimFormat("%Y/%m/%d",now));
  strcpy(nows2,nows);
  
  unsigned long stimet2 = 0;
  unsigned long atimet2 = 0;
  
  int iret = GetFileItem(path,"complog=",buff,sizeof(buff));
  if(iret == -2){
    stimet2 = 0;
    strcpy(buff,nows);
  }else if(strcmp(nows,buff)){
    stimet2 = utTimStrToLong("%Y/%m/%d %H:%M:%S",buff);
  }else{
    return;
  }
    
  char caResult[40960] = "";
  int iRetSum = 0;
  unsigned long sid = 0;
  unsigned long compid = 0;
  unsigned long userid = 0;
  char mac[24] = "";
  unsigned long groupid = 0;
  char devname[24] = "";
  unsigned long stime = 0;
  unsigned long itime = 0;
  unsigned long atime = 0;
  unsigned long stimet = 0;
  unsigned long atimet = 0;
  char *p = NULL;
  
  printf("1111111111111111\n");
  
  comp_combine_table(tables,buff,nows);
  snprintf(sql,sizeof(sql),"select sid,compid,userid,mac,groupid,devname,startmactime,idletime,activetime from %s where startmactime>=%lu and activetime<=%lu",tables,stimet2,now+86400);
  writeLog("comstatics.txt","div",sql);
  pasDbRecordSet(sql,0,9999,&iRetSum,caResult,0);
  p = caResult;
  
  tables[strlen(tables)-12] = 0;
  printf("2222222222222222\n");
  while(*p && p) {
    
  //printf("333333333333\n");
		p = pasDbRecordValue(p,1,9,
	  	UT_TYPE_ULONG,  sizeof(sid),      &sid,
			UT_TYPE_ULONG,  sizeof(compid),   &compid,
			UT_TYPE_ULONG,  sizeof(userid),   &userid,
			UT_TYPE_STRING, sizeof(mac)-1,    mac,
			UT_TYPE_ULONG,  sizeof(groupid),  &groupid,
			UT_TYPE_STRING, sizeof(devname)-1,devname,
			UT_TYPE_ULONG,  sizeof(stime),    &stime,
			UT_TYPE_ULONG,  sizeof(itime),    &itime,
			UT_TYPE_ULONG,  sizeof(atime),    &atime);
		stimet=(stime+28800)/86400;
		
		while(stimet<(atime+28799)/86400){
		  if(!strcmp(devname,"user_pc")){
		    printf("devname:%s,stime:%lu,atime:%lu,stimet:%lu,atime:%lu,atimemd:%lu\n",devname,stime,atime,stimet,(atime+28800)/86400,(atime+28800)%86400);
		  }
      if(stimet==(stime+28740)/86400){
        stimet2 = stime;
      }else{
        stimet2 = stimet*86400-28800;
      }
      
      atimet2 = stimet*86400+57599;
      
		  sprintf(sql,"insert %s(compid,userid,mac,groupid,devname,startmactime,idletime,activetime)values(%lu,%lu,'%s',%lu,'%s',%lu,%lu,%lu)",tables,compid,userid,mac,groupid,devname,stimet2,itime,atimet2);
		  stimet++;
		  
		  if(!strcmp(devname,"user_pc"))
		    writeLog("comstatics.txt","insert",sql);
		  pasDbExecSqlF(sql);
		}
		
		if(!strcmp(devname,"user_pc")){
		  printf("111stime:%lu,atime:%lu,stimet:%lu,stime:%lu\n",stime,atime,stimet,(stime-28740)/86400);
		}
		if(stimet!=(stime+28800)/86400){
		  stimet2 = stimet*86400-28800;
		  atimet2 = atime;
      sprintf(sql,"delete from %s where sid=%lu",tables,sid);
      
		  if(!strcmp(devname,"user_pc"))
        writeLog("comstatics.txt","delete",sql);
        
      pasDbExecSqlF(sql);
      sprintf(sql,"insert %s(compid,userid,mac,groupid,devname,startmactime,idletime,activetime)values(%lu,%lu,'%s',%lu,'%s',%lu,%lu,%lu)",tables,compid,userid,mac,groupid,devname,stimet2,itime,atimet2);
      
		  if(!strcmp(devname,"user_pc"))
		    writeLog("comstatics.txt","insert2",sql);
      pasDbExecSqlF(sql);
		}
 	}
 	
  //printf("1666666611111111\n");
  //writeLog("filetest.txt","now2",nows2);
 	SetFileItem(path,"complog=",nows2);
}

/*
int lanSearchCompStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  checkMacRun();
  utMsgPrintMsg(psMsgHead);
  char sdate_in[24] = "";
  char edate_in[24] = "";
  char compid_in[24] = "";
  char groupid_in[24] = "";
  
  utMsgGetSomeNVar(psMsgHead,4,
		"sdate",  UT_TYPE_STRING, sizeof(sdate_in)-1,   sdate_in,
		"edate",  UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in,
		"compid", UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"groupid",UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in);
		
	char sdates[24] = "";
	char edates[24] = "";
	char tables[256] = "";
	
	strcpy(sdates,utTimFormat("%Y/%m/%d", strtoul(sdate_in,NULL,10)));
	strcpy(edates,utTimFormat("%Y/%m/%d", strtoul(edate_in,NULL,10)));
	
	compute1_combine_table(tables,sdates,edates);
	
	char sql_tmp[256] = "";
	char sql[256] = "";
	
	char *auth = getDsGroupid();
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp," and compid = %s group by compid",compid_in,compid_in);
	}else if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp," and groupid = %s group by groupid",groupid_in,groupid_in);
	}else{
	  snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp)," and groupid in (%s)",auth);
	}
  		
  sprintf(sql,"select sum(activetime-startmactime),sum(idletime) from %s where startmactime>=%s and activetime<=%s %s and activetime>startmactime",tables,sdate_in,edate_in,sql_tmp);
  
  pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
  
  writeLog("procStatics.txt","compute",sql);
  
	unsigned long runtime = 0;
	unsigned long idletime = 0;
  pasDbOneRecord(sql, 0, 
    UT_TYPE_ULONG,  sizeof(runtime), 	  &runtime,
    UT_TYPE_ULONG,  sizeof(idletime),   &idletime);
  utPltPutVarF(psDbHead,"buff","{runtime:'%lu',idletime:'%lu'}",runtime,idletime);
	
  utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");

  return 0;
}
*/
int lanSearchCompStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  checkMacRun();
  utMsgPrintMsg(psMsgHead);
  char sdate_in[24] = "";
  char edate_in[24] = "";
  char compid_in[24] = "";
  char groupid_in[24] = "";
  
  utMsgGetSomeNVar(psMsgHead,4,
		"sdate",  UT_TYPE_STRING, sizeof(sdate_in)-1,   sdate_in,
		"edate",  UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in,
		"compid", UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"groupid",UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in);
		
	char sdates[24] = "";
	char edates[24] = "";
	char tables[256] = "";
	
	strcpy(sdates,utTimFormat("%Y/%m/%d", strtoul(sdate_in,NULL,10)));
	strcpy(edates,utTimFormat("%Y/%m/%d", strtoul(edate_in,NULL,10)));
	
	compute1_combine_table(tables,sdates,edates);
	
	char sql_tmp[256] = "";
	char sql[256] = "";
	
	char *auth = getDsGroupid();
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp," and compid = %s group by compid",compid_in,compid_in);
	}else if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp," and groupid = %s group by groupid",groupid_in,groupid_in);
	}else{
	  snprintf(sql_tmp+strlen(sql_tmp), sizeof(sql_tmp)-strlen(sql_tmp)," and groupid in (%s)",auth);
	}
  		
  sprintf(sql,"select sum(activetime-startmactime) as runtime from %s where activetime>startmactime and startmactime>=%s and activetime<=%s %s",tables,sdate_in,edate_in,sql_tmp);
  
  pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
  
  writeLog("procStatics.txt","compute",sql);
  
	unsigned long runtime = 0;
	unsigned long idletime = 0;
  pasDbOneRecord(sql, 0, UT_TYPE_ULONG,  sizeof(runtime), 	  &runtime);
  memset(tables,0,sizeof(tables));
  proc_combine_table(tables,sdates,edates);
  sprintf(sql,"select sum(wndtime) from %s where stime>=%s and stime<=%s %s",tables,sdate_in,edate_in,sql_tmp);
  
  writeLog("procStatics.txt","compute",sql);
  pasDbOneRecord(sql, 0, UT_TYPE_ULONG,  sizeof(idletime), 	  &idletime);
  if(idletime>runtime){
    idletime=runtime;
  }
  utPltPutVarF(psDbHead,"buff","{runtime:'%lu',idletime:'%lu'}",runtime,idletime);
  
  utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");

  return 0;
}

int lanDownPDFReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
/*  char style_in[8] = "";
	char groupid_in[16] = "";
	char compid_in[16] = "";
	char num_in[16] = "";
	char sdate_in[24] = "";
	char edate_in[24] = "";
	int num = 0;
	
	utMsgGetSomeNVar(psMsgHead,6,
		"style",UT_TYPE_STRING,sizeof(style_in)-1,style_in,
		"num",UT_TYPE_STRING,sizeof(num_in)-1,num_in,
		"groupid",UT_TYPE_STRING,sizeof(groupid_in)-1,groupid_in,
		"compid",UT_TYPE_STRING,sizeof(compid_in)-1,compid_in,
		"sdate",UT_TYPE_STRING,sizeof(sdate_in)-1,sdate_in,
		"edate",UT_TYPE_STRING,sizeof(edate_in)-1,edate_in);
		
  utPltDbHead *psDbHead = utPltInitDb();
	
	char sql[512] = "";
	char sql_tmp[128] = "";
	char sql_tmp1[128] = "";
	
	memset(sql,0,sizeof(sql));
	memset(sql_tmp1,0,sizeof(sql_tmp1));
	memset(sql_tmp,0,sizeof(sql_tmp));
	
	char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);*/
	
	//生成PDF 文件
	PDF *p;
	int font, font2;
	int col=1, row=1;
  int tbl = -1;
	char optlist[1024];
	char filename[64] = "";
	char file[128] = "";
	setlocale(LC_ALL, "zh_CN.UTF-8");
  if ((p = PDF_new()) == (PDF *) 0) {
    printf("Couldn't create PDFlib object (out of memory)!\n");
    return (2);
  }
  
  PDF_TRY(p) {
    PDF_set_parameter(p, "errorpolicy", "return");
		PDF_set_parameter(p, "SearchPath", "/etc/CMap");
		PDF_set_parameter(p, "charref", "true");
		PDF_set_info(p, "Creator", " ");
		PDF_set_info(p, "Title", "Report");
		
		memset(filename, 0, sizeof(filename));
		sprintf(filename, "lanReport_%lu.pdf", time(0));
		sprintf(file, "/home/ncmysql/nw/temp/%s",filename);
    if (PDF_begin_document(p, file, 0, "") == -1) {/* PDF 文档开始*/
			printf("Error: %s\n", PDF_get_errmsg(p));
			PDF_delete(p);
			return (2);
		}
		
		font = PDF_load_font(p, "AdobeSongStd-Light", 0, "GBK-EUC-H", ""); /*Adobe 内置字体 */
    if (font == -1) {
			 printf("Error: %s\n", PDF_get_errmsg(p));
			 PDF_delete(p);
			 return (2);
		}
		
		font2 = PDF_load_font(p, "simhei", 0, "GBKp-EUC-H", "noembedding");/* 表头的黑体 */
    if (font2 == -1) {
			printf("Error: %s\n", PDF_get_errmsg(p));
			PDF_delete(p);
			return (2);
    }
    
    PDF_begin_page_ext(p, 0, 0, "width=1024 height=768");
    sprintf(optlist,"font=%d fontsize=25 fakebold=true position={center bottom}",font);//标题
    //printf("caTitle2 == %s\n", caTitle2);
    PDF_fit_textline(p, "我的第一个PDF", 0,  512, 717, optlist);
    
    //第一列
    sprintf(optlist,"colwidth=7%% fittextline={font=%d fontsize=20 position={center center}}",font2);
    tbl = PDF_add_table_cell(p, tbl, col, row, "第一列", 0, optlist);
    if (tbl == -1) {
  		printf("Error: adding cell: %s\n", PDF_get_errmsg(p));
  		PDF_delete(p);
  		return -2;
    }
    col++;
    
    //第二列
    sprintf(optlist,"colwidth=7%% fittextline={font=%d fontsize=20 position={center center}}",font2);
    tbl = PDF_add_table_cell(p, tbl, col, row, "第二列", 0, optlist);
    if (tbl == -1) {
  		printf("Error: adding cell: %s\n", PDF_get_errmsg(p));
  		PDF_delete(p);
  		return -2;
    }
    PDF_delete_table(p, tbl, "");
	  PDF_end_page_ext(p, "");
  }PDF_CATCH(p) {
    printf("PDFlib exception occurred: [%d] %s: %s\n",PDF_get_errnum(p), PDF_get_apiname(p),PDF_get_errmsg(p));
    PDF_delete(p);
    return (2);
  }
	
  PDF_delete(p);
  
  char caPath[64]="";
  strcat(caPath,"/home/ncmysql/nw/temp/");
  remove_watermark(file);
  //rename(filename, filename_new);//strcat(caPath,filename_new);
  utPltFileDownload(iFd, "application/pdf", caPath, filename, filename);
  remove(filename);
	
	//生成标题
	
	
	//生成表格
/*	
	switch(atoi(style_in)){
	  case 1: 
	    strcpy(sql_tmp,",bsum desc");
	    break;
	  case 2:
	    strcpy(sql_tmp,",wsum desc");
	    break;
	  case 3:
	    strcpy(sql_tmp,",nsum desc");
	    break;
	  default:
	    strcpy(sql_tmp,",bsum desc");
	    break;
	}
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp1," and compid = %s",compid_in);
	}else if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp1," and groupid = %s",groupid_in);
	}
	
	if(!utStrIsSpaces(sdate_in)){
	  sprintf(sql_tmp1+strlen(sql_tmp1)," and stime >= %s",sdate_in);
	}
	
	if(!utStrIsSpaces(edate_in)){
	  sprintf(sql_tmp1+strlen(sql_tmp1)," and stime <= %s",edate_in);
	}
	pasDbCursor *psCur = NULL;
	
  sprintf(sql,"select nwprocStaticsType.statstype,procname,sum(btime) as bsum,sum(nettime) as nsum,sum(wndtime) as wsum from nwproclog_201405 left join nwprocStatics on procname=processname and nwprocStatics.company='%s' left join nwprocStaticsType on nwprocStaticsType.recno=nwprocStatics.statstype where 1=1 %s group by procname order by nwprocStaticsType.recno desc%s",shortname,sql_tmp1,sql_tmp);
  
  writeLog("procStatcs.txt","mainSearch",sql);
  
  psCur = pasDbOpenSql(sql, 0);
  
  if(psCur!=NULL){
    char proctype[64] = "";
    char proctypeOld[64] = "";
    char procname[32] = "";
		unsigned long runtime = 0;
		unsigned long nettime = 0;
		unsigned long activetime = 0;
		
		char runtimes[16] = "";
		char nettimes[16] = "";
		char activetimes[16] = "";
		
		int tmp = 0;
		
		int iret = 0;
		int iNum = 0;
		while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_STRING,sizeof(proctype)-1,  proctype,
        UT_TYPE_STRING,sizeof(procname)-1,  procname,
        UT_TYPE_ULONG, sizeof(runtime), 	&runtime,
        UT_TYPE_ULONG, sizeof(nettime),   &nettime,
        UT_TYPE_ULONG, sizeof(activetime),&activetime)) || 1405==iret){
          
      //writeLog("procStatcs.txt","test",proctype);    
			if(utStrIsSpaces(proctype)){
			  continue;
			}
			iNum++;
			if(iNum==1){
			//writeLog("procStatcs.txt","test","here3"); 
			  strcpy(proctypeOld,proctype);
			//writeLog("procStatcs.txt","test","here4");
			}
			if(iNum>1){
			//writeLog("procStatcs.txt","test","here1"); 
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			//writeLog("procStatcs.txt","test","here2");
			} 
			if(!strcmp(proctypeOld,proctype)){
			  tmp++;
			  if(tmp>num){
			    continue;
			  }
			}else{
			  tmp=0;
			  strcpy(proctypeOld,proctype);
			}
			
      //writeLog("procStatcs.txt","test",proctype); 
      
			//switchToRemainTime(runtimes,runtime);
			//switchToRemainTime(nettimes,nettime);
			//switchToRemainTime(activetimes,activetime);
			
			//writeLog("procStatcs.txt","runtime",runtimes);
			//writeLog("procStatcs.txt","nettime",nettimes);
			//writeLog("procStatcs.txt","activetime",activetimes);
			
			utPltPutLoopVar(psDbHead,"typename",iNum,proctype);
			utPltPutLoopVar(psDbHead,"procname",iNum,procname);
			utPltPutLoopVarF(psDbHead,"runtime",iNum,"%lu",runtime);
			utPltPutLoopVarF(psDbHead,"activetime",iNum,"%lu",activetime);
			utPltPutLoopVarF(psDbHead,"nettime",iNum,"%lu",nettime);
			//memset(runtimes,0,sizeof(runtimes));
			//memset(nettimes,0,sizeof(nettimes));	
			//memset(activetimes,0,sizeof(activetimes));	
			memset(proctype,0,sizeof(proctype));
		}
    pasDbCloseCursor(psCur);
  }
*/	
//	utPltShowDb(psDbHead);
		
		return 0;
}

int lanDownCSVReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  char sdate_in[24] = "";
  char edate_in[24] = "";
  char compid_in[16] = "";
  char groupid_in[16] = "";
  char style_in[8] = "";
  char num_in[8] = "";
  char runtime_in[16] = "";
  char idletime_in[16] = "";
  
  utMsgGetSomeNVar(psMsgHead,8,
		"sdate",    UT_TYPE_STRING, sizeof(sdate_in)-1,   sdate_in,
		"edate",    UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in,
		"compid",   UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"groupid",  UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in,
		"style",    UT_TYPE_STRING, sizeof(style_in)-1,   style_in,
		"num",      UT_TYPE_STRING, sizeof(num_in)-1,     num_in,
		"runtime",  UT_TYPE_STRING, sizeof(runtime_in)-1, runtime_in,
		"idletime", UT_TYPE_STRING, sizeof(idletime_in)-1,idletime_in);
		
	char sdates[24] = "";
	char edates[24] = "";
	char tables[256] = "";
	
	strcpy(sdates,utTimFormat("%Y/%m/%d", strtoul(sdate_in,NULL,10)));
	strcpy(edates,utTimFormat("%Y/%m/%d", strtoul(edate_in,NULL,10)));
	
	proc1_combine_table(tables,sdates,edates);
  
  char runtimes_in[16] = "";
  char idletimes_in[16] = "";
  
  FILE *fp;
	char caFilename[32];
	char caFile[128];
	char caPath[128];
	
	time_t time_tmp;
	struct tm *tm_tmp;
	time(&time_tmp);
	tm_tmp = gmtime(&time_tmp);

	sprintf(caFilename,"nwProcReport_%04u%02u%02u.csv",tm_tmp->tm_year+1900,tm_tmp->tm_mon+1,tm_tmp->tm_mday);
	
	sprintf(caPath,"%s/html","/home/ncmysql/nw");
  sprintf(caFile,"%s/%s",caPath,caFilename);
  
  fp = fopen(caFile,"w");
  if(fp == NULL) {
    printf("open failed!\n");
    return 0;
  }
  #if GYS
    fprintf(fp,"Program Statistics\n");
  #else
    fprintf(fp,"%s\n",ncLang("0118程序统计报表"));
  #endif
  
  char *p = NULL;
  char tmp1[128] = "";
  char tmp2[128] = "";
  
  if(!utStrIsSpaces(compid_in)){
    p=findCompNameById(strtoul(compid_in,NULL,10));
    #if GYS
      sprintf(tmp1,"Compute: %s\n",p);
    #else
      sprintf(tmp1,"%s: %s\n",ncLang("0061计算机"),p);
    #endif
    free(p);
  }else if(!utStrIsSpaces(groupid_in)){
    p=findGroupNameById(strtoul(groupid_in,NULL,10));
    #if GYS
      sprintf(tmp1,"Group: %s\n",p);
    #else
      sprintf(tmp1,"%s: %s\n",ncLang("0069部门"),p);
    #endif
    free(p);
  }else{
    #if GYS
      sprintf(tmp1,"Group: ALL\n");
    #else
      sprintf(tmp1,"%s: %s\n",ncLang("0069部门"),ncLang("0011全部"));
    #endif
  }
  
	ncUtlPrintCsv(fp,1,UT_TYPE_STRING,"");
	#if GYS
    sprintf(tmp2,"Start Date: %s,  End Date: %s",utTimFormat("%Y/%m/%d", strtoul(sdate_in,NULL,10)),utTimFormat("%Y/%m/%d", strtoul(edate_in,NULL,10)));
  #else
    sprintf(tmp2,"%s: %s,  %s: %s",ncLang("0119起始日期"),utTimFormat("%Y/%m/%d", strtoul(sdate_in,NULL,10)),ncLang("0120终止日期"),utTimFormat("%Y/%m/%d", strtoul(edate_in,NULL,10)));
  #endif
	ncUtlPrintCsv(fp,2,UT_TYPE_STRING,tmp1,UT_TYPE_STRING,tmp2);
  
  #if GYS
    switchToRemainTime1(runtimes_in,strtoul(runtime_in,NULL,10));
  	switchToRemainTime1(idletimes_in,strtoul(idletime_in,NULL,10));
  	
	  sprintf(tmp1,"Run Time: %s , Idle Time: %s",runtimes_in,idletimes_in);
	  ncUtlPrintCsv(fp,2,UT_TYPE_STRING,"Compute Run State",UT_TYPE_STRING,tmp1);
	  ncUtlPrintCsv(fp,1,UT_TYPE_STRING,"");
	
	  ncUtlPrintCsv(fp,7,
	    UT_TYPE_STRING,"Categories",
	    UT_TYPE_STRING,"Name",
	    UT_TYPE_STRING,"Run Time",
	    UT_TYPE_STRING,"Percentage",
	    UT_TYPE_STRING,"Activity Time",
	    UT_TYPE_STRING,"Percentage",
	    UT_TYPE_STRING,"Internet Time");
	#else
	  switchToRemainTime(runtimes_in,strtoul(runtime_in,NULL,10));
  	switchToRemainTime(idletimes_in,strtoul(idletime_in,NULL,10));
  	
	  sprintf(tmp1,"%s: %s ,%s: %s",ncLang("0084运行时间"),runtimes_in,ncLang("0084运行时间"),idletimes_in);
	  ncUtlPrintCsv(fp,2,UT_TYPE_STRING,"%s",ncLang("0122电脑运行"),UT_TYPE_STRING,tmp1);
	  ncUtlPrintCsv(fp,1,UT_TYPE_STRING,"");
	
	  ncUtlPrintCsv(fp,7,
	    UT_TYPE_STRING,ncLang("0123程序类别"),
	    UT_TYPE_STRING,ncLang("0124程序名称"),
	    UT_TYPE_STRING,ncLang("0084运行时间"),
	    UT_TYPE_STRING,ncLang("0125占百分比"),
	    UT_TYPE_STRING,ncLang("0126活动时间"),
	    UT_TYPE_STRING,ncLang("0125占百分比"),
	    UT_TYPE_STRING,ncLang("0127上网时间"));
	#endif
	
	
	
		int num = atoi(num_in);
	if(num==0){
	  num = 10;
	}
	pasDbCursor *psCur = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
	
	char sql[512] = "";
	char sql_tmp[128] = "";
	char sql_tmp1[128] = "";
	
	char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	
	findShortName(auth,shortname);
	
	char *authgroup = getDsGroupid();
	
	memset(sql,0,sizeof(sql));
	memset(sql_tmp1,0,sizeof(sql_tmp1));
	memset(sql_tmp,0,sizeof(sql_tmp));
	switch(atoi(style_in)){
	  case 1: 
	    strcpy(sql_tmp,",bsum desc");
	    break;
	  case 2:
	    strcpy(sql_tmp,",wsum desc");
	    break;
	  case 3:
	    strcpy(sql_tmp,",nsum desc");
	    break;
	  default:
	    strcpy(sql_tmp,",bsum desc");
	    break;
	}
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp1," and compid = %s",compid_in);
	}else if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp1," and groupid = %s",groupid_in);
	}else{
	  sprintf(sql_tmp1+strlen(sql_tmp1)," and groupid in (%s)",authgroup);
	}
	
	if(!utStrIsSpaces(sdate_in)){
	  sprintf(sql_tmp1+strlen(sql_tmp1)," and stime >= %s",sdate_in);
	}
	
	if(!utStrIsSpaces(edate_in)){
	  sprintf(sql_tmp1+strlen(sql_tmp1)," and stime <= %s",edate_in);
	}
	
  sprintf(sql,"select nwprocStaticsType.statstype,procname,sum(btime) as bsum,sum(nettime) as nsum,sum(wndtime) as wsum from %s left join nwprocStatics on procname=processname and nwprocStatics.company='%s' and nwprocStatics.statstype<>0 left join nwprocStaticsType on nwprocStaticsType.recno=nwprocStatics.statstype where 1=1 %s group by procname order by nwprocStaticsType.recno desc%s",tables,shortname,sql_tmp1,sql_tmp);
  
  writeLog("procStatics.txt","export",sql);
  
  psCur = pasDbOpenSql(sql, 0);
  
  if(psCur!=NULL){
    char proctype[128] = "";
    char proctypeOld[64] = "";
    char procname[64] = "";
		unsigned long runtime = 0;
		unsigned long nettime = 0;
		unsigned long activetime = 0;
		
		char runtimes[32] = "";
		char nettimes[32] = "";
		char activetimes[32] = "";
		int tmp3 = 0;
		
		int iret = 0;
		int iNum = 0;
		while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_STRING,sizeof(proctype)-1,  proctype,
        UT_TYPE_STRING,sizeof(procname)-1,  procname,
        UT_TYPE_ULONG, sizeof(runtime), 	&runtime,
        UT_TYPE_ULONG, sizeof(nettime),   &nettime,
        UT_TYPE_ULONG, sizeof(activetime),&activetime)) || 1405==iret){
          
      //writeLog("procStatics.txt","test",proctype);  
			if(utStrIsSpaces(proctype)){
			  strcpy(proctype,"其他");
			}
			
			if(!strcmp(proctypeOld,proctype)){
			  tmp3++;
			  if(tmp3>num-1){
			    continue;
			  }
			}else{
			  tmp3=0;
			 
			  strcpy(proctypeOld,proctype);
			}
			
      //writeLog("procStatics.txt","test2",procname);
      #if GYS
  		  switchToRemainTime1(runtimes,runtime);
  		  switchToRemainTime1(activetimes,activetime);
  		  switchToRemainTime1(nettimes,nettime);
		  #else
		    switchToRemainTime(runtimes,runtime);
  		  switchToRemainTime(activetimes,activetime);
  		  switchToRemainTime(nettimes,nettime);
		  #endif
		  
      //writeLog("procStatics.txt","test3",runtimes);
		  unsigned long tmpnum = strtoul(runtime_in,NULL,10);
		  sprintf(tmp1,"%lu%%",runtime<tmpnum?runtime*100/tmpnum:100);
		  sprintf(tmp2,"%lu%%",runtime<tmpnum?activetime*100/tmpnum:100);
		  
      //writeLog("procStatics.txt","test4",activetimes);
		  
			ncUtlPrintCsv(fp,7,
    	    UT_TYPE_STRING,proctype,
    	    UT_TYPE_STRING,procname,
    	    UT_TYPE_STRING,runtimes,
    	    UT_TYPE_STRING,tmp1,
    	    UT_TYPE_STRING,activetimes,
    	    UT_TYPE_STRING,tmp2,
    	    UT_TYPE_STRING,nettimes);
    	    
     //writeLog("procStatics.txt","test5",proctype);
			
			memset(tmp1,0,sizeof(tmp1));
			memset(tmp2,0,sizeof(tmp2));
			memset(runtimes,0,sizeof(runtimes));
			memset(activetimes,0,sizeof(activetimes));
			memset(nettimes,0,sizeof(nettimes));
			memset(proctype,0,sizeof(proctype));
			
      //writeLog("procStatics.txt","test6",nettimes);
		}
    pasDbCloseCursor(psCur);
  }
  
  fclose(fp);
  utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
  remove(caFile);
  utPltFreeDb(psDbHead);
  return 0;
}

//当天首次开机排行
int lanFirstCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  checkMacRun();
  utMsgPrintMsg(psMsgHead);
  
  char sdate_in[16] = "";
  char edate_in[16] = "";
  char compid_in[16] = "";
  char groupid_in[16] = "";
  
  utMsgGetSomeNVar(psMsgHead,4,
		"sdate",  UT_TYPE_STRING, sizeof(sdate_in)-1,   sdate_in,
		"edate",  UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in,
		"compid", UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"groupid",UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in);
	
	char sql[512] = "";
	char sql_tmp[256] = "";
	
	if(!utStrIsSpaces(sdate_in)){
	  sprintf(sql_tmp," and startmactime>%s+60 and activetime<%s",sdate_in,edate_in);
	}
	
	if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.groupid in(%s)",getGroupid(strtoul(groupid_in,NULL,10)));
	}
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.compid =%s",compid_in);
	}
	
	char newtable[128] = "";
	char stimes[16] = "";
	char etimes[16] = "";
	strcpy(stimes, utTimFormat("%Y/%m/%d ", strtoul(sdate_in,NULL,10)));
	strcpy(etimes, utTimFormat("%Y/%m/%d ", strtoul(edate_in,NULL,10)));
	comp_combine_table(newtable,stimes,etimes);
	
	sprintf(sql,"select compname,gname,nwcompute.devname,inet_ntoa(nwcompute.ip),from_unixtime(min(startmactime),'%%H:%%i:%%s') as mintime,from_unixtime(activetime,'%%H:%%i:%%s') from %s left join nwcompute on new_table.compid=nwcompute.compid left join nwgroup on new_table.groupid=gid where 1=1 %s group by new_table.compid order by mintime",newtable,sql_tmp);	
	
  pasDbCursor *psCur = NULL;
  utPltDbHead *psDbHead = utPltInitDb();
	writeLog("comstatics.txt","seach",sql);
	psCur = pasDbOpenSql(sql, 0);
	if(psCur != NULL){
	  char compname[32] = "";
	  char groupname[32] = "";
	  char devname[32] = "";
	  char sip[32] = "";
	  char stime[24] = "";
	  char lasttime[24] = "";
	  int iNum = 0,iret=-1;
	  
	  while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_STRING, sizeof(compname)-1, compname,
        UT_TYPE_STRING, sizeof(groupname)-1,groupname,
        UT_TYPE_STRING, sizeof(devname)-1,  devname,
        UT_TYPE_STRING, sizeof(sip)-1,      sip,
        UT_TYPE_STRING, sizeof(stime)-1, 	  stime,
        UT_TYPE_STRING, sizeof(lasttime)-1, lasttime)) || 1405==iret){
      iNum++;
      if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
      utPltPutLoopVarF(psDbHead,"rank",iNum,"%d",iNum);
      utPltPutLoopVar(psDbHead,"compname",iNum,compname);
      utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
      utPltPutLoopVar(psDbHead,"devname",iNum,devname);
      utPltPutLoopVar(psDbHead,"sip",iNum,sip);
      utPltPutLoopVar(psDbHead,"stime",iNum,stime);
      utPltPutLoopVar(psDbHead,"lasttime",iNum,lasttime);
    }
    pasDbCloseCursor(psCur);
	}
	utPltShowDb(psDbHead);	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_compStatic.htm");
	
  return 0;
}

//运行时间排行
int lanRuntimeCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  checkMacRun();
  utMsgPrintMsg(psMsgHead);
  
  char sdate_in[16] = "";
  char edate_in[16] = "";
  char compid_in[16] = "";
  char groupid_in[16] = "";
  
  utMsgGetSomeNVar(psMsgHead,4,
		"sdate",  UT_TYPE_STRING, sizeof(sdate_in)-1,   sdate_in,
		"edate",  UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in,
		"compid", UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"groupid",UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in);
	
	char sql[512] = "";
	char sql_tmp[256] = "";
	
	if(!utStrIsSpaces(sdate_in)){
	  sprintf(sql_tmp," and startmactime>%s and activetime<%s",sdate_in,edate_in);
	}
	
	if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.groupid in(%s)",getGroupid(strtoul(groupid_in,NULL,10)));
	}
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.compid =%s",compid_in);
	}
	char newtable[128] = "";
	char stimes[16] = "";
	char etimes[16] = "";
	strcpy(stimes, utTimFormat("%Y/%m/%d ", strtoul(sdate_in,NULL,10)));
	strcpy(etimes, utTimFormat("%Y/%m/%d ", strtoul(edate_in,NULL,10)));
	comp_combine_table(newtable,stimes,etimes);
	
	sprintf(sql,"select compname,gname,nwcompute.devname,inet_ntoa(nwcompute.ip),sum(activetime-startmactime) as runtime,from_unixtime(activetime,'%%Y/%%m/%%d %%H:%%i:%%s') from %s left join nwcompute on new_table.compid=nwcompute.compid left join nwgroup on new_table.groupid=gid where 1=1 %s group by new_table.compid order by runtime desc",newtable,sql_tmp);	
	
  pasDbCursor *psCur = NULL;
  utPltDbHead *psDbHead = utPltInitDb();
	writeLog("comstatics.txt","seach",sql);
	psCur = pasDbOpenSql(sql, 0);
	if(psCur != NULL){
	  char compname[32] = "";
	  char groupname[32] = "";
	  char devname[32] = "";
	  char sip[32] = "";
	  unsigned long runtime = 0;
	  char runtimes[24] = "";
	  char lasttime[24] = "";
	  int iNum = 0,iret=-1;
	  
	  while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_STRING, sizeof(compname)-1, compname,
        UT_TYPE_STRING, sizeof(groupname)-1,groupname,
        UT_TYPE_STRING, sizeof(devname)-1,  devname,
        UT_TYPE_STRING, sizeof(sip)-1,      sip,
        UT_TYPE_ULONG,  sizeof(runtime), 	  &runtime,
        UT_TYPE_STRING, sizeof(lasttime)-1, lasttime)) || 1405==iret){
      iNum++;
      if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
      utPltPutLoopVarF(psDbHead,"rank",iNum,"%d",iNum);
      utPltPutLoopVar(psDbHead,"compname",iNum,compname);
      utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
      utPltPutLoopVar(psDbHead,"devname",iNum,devname);
      utPltPutLoopVar(psDbHead,"sip",iNum,sip);
      
      switchToRemainTime(runtimes,runtime);
      utPltPutLoopVar(psDbHead,"stime",iNum,runtimes);
      utPltPutLoopVar(psDbHead,"lasttime",iNum,lasttime);
      memset(runtimes,0,sizeof(runtimes));
    }
    pasDbCloseCursor(psCur);
	}
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_compStatic.htm");
	
  return 0;
}

//使用时间排行
/*
int lanUsetimeCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  checkMacRun();
  utMsgPrintMsg(psMsgHead);
  
  char sdate_in[16] = "";
  char edate_in[16] = "";
  char compid_in[16] = "";
  char groupid_in[16] = "";
  
  utMsgGetSomeNVar(psMsgHead,4,
		"sdate",  UT_TYPE_STRING, sizeof(sdate_in)-1,   sdate_in,
		"edate",  UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in,
		"compid", UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"groupid",UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in);
	
	char sql[512] = "";
	char sql_tmp[256] = "";
	
	if(!utStrIsSpaces(sdate_in)){
	  sprintf(sql_tmp," and startmactime>%s and activetime<%s",sdate_in,edate_in);
	}
	
	if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.groupid in(%s)",getGroupid(strtoul(groupid_in,NULL,10)));
	}
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.compid =%s",compid_in);
	}
	char newtable[128] = "";
	char stimes[16] = "";
	char etimes[16] = "";
	strcpy(stimes, utTimFormat("%Y/%m/%d ", strtoul(sdate_in,NULL,10)));
	strcpy(etimes, utTimFormat("%Y/%m/%d ", strtoul(edate_in,NULL,10)));
	comp_combine_table(newtable,stimes,etimes);
	
	sprintf(sql,"select compname,gname,nwcompute.devname,inet_ntoa(nwcompute.ip),sum(activetime-startmactime-idletime) as runtime,from_unixtime(activetime,'%%Y/%%m/%%d %%H:%%i:%%s') from %s left join nwcompute on new_table.compid=nwcompute.compid left join nwgroup on new_table.groupid=gid where 1=1 %s group by new_table.compid order by runtime desc",newtable,sql_tmp);	
	
  pasDbCursor *psCur = NULL;
  utPltDbHead *psDbHead = utPltInitDb();
	writeLog("comstatics.txt","seach",sql);
	psCur = pasDbOpenSql(sql, 0);
	if(psCur != NULL){
	  char compname[32] = "";
	  char groupname[32] = "";
	  char devname[32] = "";
	  char sip[32] = "";
	  unsigned long runtime = 0;
	  char runtimes[24] = "";
	  char lasttime[24] = "";
	  int iNum = 0,iret=-1;
	  
	  while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_STRING, sizeof(compname)-1, compname,
        UT_TYPE_STRING, sizeof(groupname)-1,groupname,
        UT_TYPE_STRING, sizeof(devname)-1,  devname,
        UT_TYPE_STRING, sizeof(sip)-1,      sip,
        UT_TYPE_ULONG,  sizeof(runtime), 	  &runtime,
        UT_TYPE_STRING, sizeof(lasttime)-1, lasttime)) || 1405==iret){
      iNum++;
      if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
      utPltPutLoopVarF(psDbHead,"rank",iNum,"%d",iNum);
      utPltPutLoopVar(psDbHead,"compname",iNum,compname);
      utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
      utPltPutLoopVar(psDbHead,"devname",iNum,devname);
      utPltPutLoopVar(psDbHead,"sip",iNum,sip);
      
      switchToRemainTime(runtimes,runtime);
      utPltPutLoopVar(psDbHead,"stime",iNum,runtimes);
      utPltPutLoopVar(psDbHead,"lasttime",iNum,lasttime);
      memset(runtimes,0,sizeof(runtimes));
    }
    pasDbCloseCursor(psCur);
	}
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_compStatic.htm");
	
  return 0;
}
*/
int lanUsetimeCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  checkMacRun();
  utMsgPrintMsg(psMsgHead);
  
  char sdate_in[16] = "";
  char edate_in[16] = "";
  char compid_in[16] = "";
  char groupid_in[16] = "";
  
  utMsgGetSomeNVar(psMsgHead,4,
		"sdate",  UT_TYPE_STRING, sizeof(sdate_in)-1,   sdate_in,
		"edate",  UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in,
		"compid", UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"groupid",UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in);
	
	char sql[512] = "";
	char sql_tmp[256] = "";
	
	if(!utStrIsSpaces(sdate_in)){
	  sprintf(sql_tmp," and new_table.stime>%s and new_table.stime<%s",sdate_in,edate_in);
	}
	
	if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.groupid in(%s)",getGroupid(strtoul(groupid_in,NULL,10)));
	}
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.compid =%s",compid_in);
	}
	char newtable[128] = "";
	char stimes[16] = "";
	char etimes[16] = "";
	strcpy(stimes, utTimFormat("%Y/%m/%d ", strtoul(sdate_in,NULL,10)));
	strcpy(etimes, utTimFormat("%Y/%m/%d ", strtoul(edate_in,NULL,10)));
	proc_combine_table(newtable,stimes,etimes);
	
	//sprintf(sql,"select compname,gname,nwcompute.devname,inet_ntoa(nwcompute.ip),sum(activetime-startmactime-idletime) as runtime,from_unixtime(activetime,'%%Y/%%m/%%d %%H:%%i:%%s') from %s left join nwcompute on new_table.compid=nwcompute.compid left join nwgroup on new_table.groupid=gid where 1=1 %s group by new_table.compid order by runtime desc",newtable,sql_tmp);	
	sprintf(sql,"select b.devname,c.gname,b.compname,inet_ntoa(b.ip),sum(wndtime) as runtime,from_unixtime(b.lasttime) from %s left join nwcompute as b on new_table.compid=b.compid left join nwgroup as c on new_table.groupid=c.gid where 1=1 %s group by new_table.compid order by runtime desc",newtable,sql_tmp);
	
  pasDbCursor *psCur = NULL;
  utPltDbHead *psDbHead = utPltInitDb();
	writeLog("comstatics.txt","seach",sql);
	psCur = pasDbOpenSql(sql, 0);
	
	unsigned long wholeTime = strtoul(edate_in,NULL,10)-strtoul(sdate_in,NULL,10);
	if(psCur != NULL){
	  char compname[32] = "";
	  char groupname[32] = "";
	  char devname[32] = "";
	  char sip[32] = "";
	  unsigned long runtime = 0;
	  char runtimes[24] = "";
	  char lasttime[24] = "";
	  int iNum = 0,iret=-1;
	  
	  while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_STRING, sizeof(compname)-1, compname,
        UT_TYPE_STRING, sizeof(groupname)-1,groupname,
        UT_TYPE_STRING, sizeof(devname)-1,  devname,
        UT_TYPE_STRING, sizeof(sip)-1,      sip,
        UT_TYPE_ULONG,  sizeof(runtime), 	  &runtime,
        UT_TYPE_STRING, sizeof(lasttime)-1, lasttime)) || 1405==iret){
      iNum++;
      if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
      utPltPutLoopVarF(psDbHead,"rank",iNum,"%d",iNum);
      utPltPutLoopVar(psDbHead,"compname",iNum,compname);
      utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
      utPltPutLoopVar(psDbHead,"devname",iNum,devname);
      utPltPutLoopVar(psDbHead,"sip",iNum,sip);
      if(runtime>wholeTime){
        runtime = wholeTime;
      }
      switchToRemainTime(runtimes,runtime);
      
      utPltPutLoopVar(psDbHead,"stime",iNum,runtimes);
      utPltPutLoopVar(psDbHead,"lasttime",iNum,lasttime);
      memset(runtimes,0,sizeof(runtimes));
    }
    pasDbCloseCursor(psCur);
	}
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_compStatic.htm");
	
  return 0;
}

//空闲时间比率排行
int lanIdleRateCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  checkMacRun();
  utMsgPrintMsg(psMsgHead);
  
  char sdate_in[16] = "";
  char edate_in[16] = "";
  char compid_in[16] = "";
  char groupid_in[16] = "";
  
  utMsgGetSomeNVar(psMsgHead,4,
		"sdate",  UT_TYPE_STRING, sizeof(sdate_in)-1,   sdate_in,
		"edate",  UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in,
		"compid", UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"groupid",UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in);
	
	char sql[512] = "";
	char sql_tmp[256] = "";
	
	if(!utStrIsSpaces(sdate_in)){
	  sprintf(sql_tmp," and startmactime>%s and activetime<%s",sdate_in,edate_in);
	}
	
	if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.groupid in(%s)",getGroupid(strtoul(groupid_in,NULL,10)));
	}
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.compid =%s",compid_in);
	}
	char newtable[128] = "";
	char stimes[16] = "";
	char etimes[16] = "";
	strcpy(stimes, utTimFormat("%Y/%m/%d ", strtoul(sdate_in,NULL,10)));
	strcpy(etimes, utTimFormat("%Y/%m/%d ", strtoul(edate_in,NULL,10)));
	comp_combine_table(newtable,stimes,etimes);
	
	sprintf(sql,"select compname,gname,nwcompute.devname,inet_ntoa(nwcompute.ip),sum(idletime)*100/sum(activetime-startmactime) as runtime,from_unixtime(activetime,'%%Y/%%m/%%d %%H:%%i:%%s') from %s left join nwcompute on new_table.compid=nwcompute.compid left join nwgroup on new_table.groupid=gid where 1=1 %s group by new_table.compid order by runtime desc",newtable,sql_tmp);	
	
  pasDbCursor *psCur = NULL;
  utPltDbHead *psDbHead = utPltInitDb();
	writeLog("comstatics.txt","seach",sql);
	psCur = pasDbOpenSql(sql, 0);
	if(psCur != NULL){
	  char compname[32] = "";
	  char groupname[32] = "";
	  char devname[32] = "";
	  char sip[32] = "";
	  unsigned long runtime = 0;
	  char lasttime[24] = "";
	  int iNum = 0,iret=-1;
	  
	  while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_STRING, sizeof(compname)-1, compname,
        UT_TYPE_STRING, sizeof(groupname)-1,groupname,
        UT_TYPE_STRING, sizeof(devname)-1,  devname,
        UT_TYPE_STRING, sizeof(sip)-1,      sip,
        UT_TYPE_ULONG,  sizeof(runtime), 	  &runtime,
        UT_TYPE_STRING, sizeof(lasttime)-1, lasttime)) || 1405==iret){
      iNum++;
      if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
      utPltPutLoopVarF(psDbHead,"rank",iNum,"%d",iNum);
      utPltPutLoopVar(psDbHead,"compname",iNum,compname);
      utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
      utPltPutLoopVar(psDbHead,"devname",iNum,devname);
      utPltPutLoopVar(psDbHead,"sip",iNum,sip);
      
      utPltPutLoopVarF(psDbHead,"stime",iNum,"%lu%%",runtime>=100?99:runtime);
      utPltPutLoopVar(psDbHead,"lasttime",iNum,lasttime);
    }
    pasDbCloseCursor(psCur);
	}
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_compStatic.htm");
	
  return 0;
}

//每日开关机统计
int lanDayCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  checkMacRun();
  utMsgPrintMsg(psMsgHead);
  
  char sdate_in[16] = "";
  char edate_in[16] = "";
  char compid_in[16] = "";
  char groupid_in[16] = "";
  
  utMsgGetSomeNVar(psMsgHead,4,
		"sdate",  UT_TYPE_STRING, sizeof(sdate_in)-1,   sdate_in,
		"edate",  UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in,
		"compid", UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"groupid",UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in);
	
	char sql[512] = "";
	char sql_tmp[256] = "";
	
	if(!utStrIsSpaces(sdate_in)){
	  sprintf(sql_tmp," and startmactime>%s+60 and activetime<%s-60",sdate_in,edate_in);
	}
	
	if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.groupid in(%s)",getGroupid(strtoul(groupid_in,NULL,10)));
	}
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.compid =%s",compid_in);
	}
	char newtable[128] = "";
	char stimes[16] = "";
	char etimes[16] = "";
	strcpy(stimes, utTimFormat("%Y/%m/%d", strtoul(sdate_in,NULL,10)));
	strcpy(etimes, utTimFormat("%Y/%m/%d", strtoul(edate_in,NULL,10)));
	comp_combine_table(newtable,stimes,etimes);
	
	sprintf(sql,"select compname,gname,nwcompute.devname,inet_ntoa(nwcompute.ip),min(startmactime),max(activetime),((startmactime-57600) div 86400) as day,UNIX_TIMESTAMP(now()) from %s left join nwcompute on new_table.compid=nwcompute.compid left join nwgroup on nwcompute.groupid=gid where 1=1 %s group by new_table.compid,day order by new_table.compid,startmactime",newtable,sql_tmp);
	
  pasDbCursor *psCur = NULL;
  utPltDbHead *psDbHead = utPltInitDb();
	writeLog("comstatics.txt","seach",sql);
	psCur = pasDbOpenSql(sql, 0);
	if(psCur != NULL){
	  char compname[32] = "";
	  char groupname[32] = "";
	  char devname[32] = "";
	  char sip[32] = "";
	  
	  unsigned long day = 0;
	  unsigned long lasttime = 0;
	  unsigned long runtime = 0;
	  unsigned long currenttime = 0;
	  int iNum = 0,iret=-1;
	  
	  while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_STRING, sizeof(compname)-1, compname,
        UT_TYPE_STRING, sizeof(groupname)-1,groupname,
        UT_TYPE_STRING, sizeof(devname)-1,  devname,
        UT_TYPE_STRING, sizeof(sip)-1,      sip,
        UT_TYPE_ULONG,  sizeof(runtime), 	  &runtime,
        UT_TYPE_ULONG,  sizeof(lasttime),   &lasttime,
        UT_TYPE_ULONG,  sizeof(day),        &day,
        UT_TYPE_ULONG,  sizeof(currenttime),&currenttime)) || 1405==iret){
      iNum++;
      if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
      utPltPutLoopVarF(psDbHead,"rank",iNum,"%d",iNum);
      utPltPutLoopVar(psDbHead,"compname",iNum,compname);
      utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
      utPltPutLoopVar(psDbHead,"devname",iNum,devname);
      utPltPutLoopVar(psDbHead,"sip",iNum,sip);
      
      utPltPutLoopVar(psDbHead,"stime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S",runtime));
      utPltPutLoopVar(psDbHead,"lasttime",iNum,currenttime-lasttime<120||lasttime-currenttime<120?ncLang("0028正在运行"):utTimFormat("%Y/%m/%d %H:%M:%S",lasttime));
    }
    pasDbCloseCursor(psCur);
	}
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_compStatic.htm");
	
  return 0;
}

typedef struct compNode{
  char compname[32];
  char groupname[32];
  char devname[32];
  char sip[32];
  unsigned long compid;
  unsigned long runtime;
  unsigned long lasttime;
  struct compNode *next;
}CompNode;

//持续运行时间统计
int lanLastingTimeStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  checkMacRun();
  utMsgPrintMsg(psMsgHead);
  
  char sdate_in[16] = "";
  char edate_in[16] = "";
  char compid_in[16] = "";
  char groupid_in[16] = "";
  
  utMsgGetSomeNVar(psMsgHead,4,
		"sdate",  UT_TYPE_STRING, sizeof(sdate_in)-1,   sdate_in,
		"edate",  UT_TYPE_STRING, sizeof(edate_in)-1,   edate_in,
		"compid", UT_TYPE_STRING, sizeof(compid_in)-1,  compid_in,
		"groupid",UT_TYPE_STRING, sizeof(groupid_in)-1, groupid_in);
	
	char sql[512] = "";
	char sql_tmp[256] = "";
	
	//printf("111111111111\n");
	if(!utStrIsSpaces(sdate_in)){
	  sprintf(sql_tmp," and startmactime>%s and activetime<%s",sdate_in,edate_in);
	}
	
	if(!utStrIsSpaces(groupid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.groupid in(%s)",getGroupid(strtoul(groupid_in,NULL,10)));
	}
	
	if(!utStrIsSpaces(compid_in)){
	  sprintf(sql_tmp+strlen(sql_tmp)," and new_table.compid =%s",compid_in);
	}
	char newtable[128] = "";
	char stimes[16] = "";
	char etimes[16] = "";
	strcpy(stimes, utTimFormat("%Y/%m/%d", strtoul(sdate_in,NULL,10)));
	strcpy(etimes, utTimFormat("%Y/%m/%d", strtoul(edate_in,NULL,10)));
	comp_combine_table(newtable,stimes,etimes);
	
	sprintf(sql,"select new_table.compid,compname,gname,nwcompute.devname,inet_ntoa(nwcompute.ip),startmactime,activetime from %s left join nwcompute on new_table.compid=nwcompute.compid left join nwgroup on nwcompute.groupid=gid where 1=1 %s order by new_table.compid,startmactime",newtable,sql_tmp);
	
  pasDbCursor *psCur = NULL;
  utPltDbHead *psDbHead = utPltInitDb();
	writeLog("comstatics.txt","seach",sql);
	psCur = pasDbOpenSql(sql, 0);
	if(psCur != NULL){
	  unsigned long runtimeTmp = 0;	  
	  CompNode *Head = (CompNode*)malloc(sizeof(CompNode));
	  Head->next=NULL;
	  CompNode *tempNode=Head;
	  int iNum = 0,iret = -1,jNum=0;
	  
	  tempNode->next=(CompNode*)malloc(sizeof(CompNode));
	  tempNode->next->next = NULL; 
	  while(0==(iret = pasDbFetchInto(psCur,
        UT_TYPE_ULONG,  sizeof(tempNode->next->compid),   &tempNode->next->compid,
        UT_TYPE_STRING, sizeof(tempNode->next->compname)-1, tempNode->next->compname,
        UT_TYPE_STRING, sizeof(tempNode->next->groupname)-1,tempNode->next->groupname,
        UT_TYPE_STRING, sizeof(tempNode->next->devname)-1,  tempNode->next->devname,
        UT_TYPE_STRING, sizeof(tempNode->next->sip)-1,      tempNode->next->sip,
        UT_TYPE_ULONG,  sizeof(tempNode->next->runtime), 	  &tempNode->next->runtime,
        UT_TYPE_ULONG,  sizeof(tempNode->next->lasttime),   &tempNode->next->lasttime)) || 1405==iret){
      
      if(tempNode->next->lasttime<tempNode->next->runtime){
        continue;
      }
      
      if(tempNode->next->runtime < 1000 || tempNode->next->lasttime < 1000){
        continue;
      }
      if(runtimeTmp!=0 && tempNode->next->runtime-runtimeTmp<86400 && tempNode->next->runtime%86400<57660){
        tempNode->next->runtime = runtimeTmp;
        runtimeTmp = 0;
      }
      
      if(tempNode->next->lasttime%86400>57540){
        runtimeTmp = tempNode->next->runtime;
      }else{
        tempNode = tempNode->next;
        tempNode->next=(CompNode*)malloc(sizeof(CompNode));
	      tempNode->next->next = NULL; 
      }      
    }
    free(tempNode->next);
    tempNode->next = NULL;
    int i = 1;
    CompNode *tempNodeJ=NULL;
    CompNode *tempNodeK=NULL;
    char runtimes[24] = "";
    char lasttimes[24] = "";
    
   /* tempNode = Head;
    while(tempNode->next){
      printf("before----compname:%s\t\tstime:%lu\t\tltime:%lu\t\taddr:%p\tnext:%p\tbig:%lu\n",tempNode->next->compname,tempNode->next->runtime,tempNode->next->lasttime,tempNode->next,tempNode->next->next,tempNode->next->lasttime-tempNode->next->runtime);
      tempNode = tempNode->next;
    }
    */
    
    while(i>0){    
      tempNodeJ =Head;
      tempNode = Head->next;
      i=0;
      
      while(tempNode->next){
        i++;
        
        if(tempNodeJ->next->lasttime-tempNodeJ->next->runtime < tempNode->next->lasttime-tempNode->next->runtime){
          tempNodeJ = tempNode;
        }
        
        tempNode=tempNode->next;
      }
      
      if(i==0 || tempNode==Head){
        break;
      }
      
     // printf("after----compname:%s\t\tstime:%lu\t\tltime:%lu\t\taddr:%p\tnext:%p\tbig:%lu\n",tempNodeJ->next->compname,tempNodeJ->next->runtime,tempNodeJ->next->lasttime,tempNodeJ->next,tempNodeJ->next->next,tempNodeJ->next->lasttime-tempNodeJ->next->runtime);
      
      tempNode = Head;
      while(tempNode->next){
        if(tempNodeJ->next->compid==tempNode->next->compid && tempNodeJ->next!=tempNode->next){
          //printf("compidJ:%lu\t\tcompid:%lu\\tJ:%p\t\t:%p\n",tempNodeJ->next->compid,tempNode->next->compid,tempNodeJ->next,tempNode->next);
          
          tempNodeK = tempNode->next;
          if(tempNodeK==tempNodeJ){
            tempNodeJ = tempNode;
          }
          tempNode->next= tempNode->next->next;
          //printf("free0---%p\t\tnext:%p\n",tempNodeK,tempNode->next);
          free(tempNodeK);
          tempNodeK = NULL;
        }else{
          tempNode = tempNode->next;
        }
      }
      
      iNum++;
      if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			
			utPltPutLoopVarF(psDbHead,"rank",iNum,"%d",iNum);
      utPltPutLoopVar(psDbHead,"compname",iNum,tempNodeJ->next->compname);
      utPltPutLoopVar(psDbHead,"groupname",iNum,tempNodeJ->next->groupname);
      utPltPutLoopVar(psDbHead,"devname",iNum,tempNodeJ->next->devname);
      utPltPutLoopVar(psDbHead,"sip",iNum,tempNodeJ->next->sip);
      utPltPutLoopVar(psDbHead,"stime",iNum,utTimFormat("%Y/%m/%d %H:%M:%S",tempNodeJ->next->runtime));
      switchToRemainTime(runtimes,tempNodeJ->next->lasttime-tempNodeJ->next->runtime);
      utPltPutLoopVar(psDbHead,"lastingtime",iNum,runtimes);
      strcpy(lasttimes, utTimFormat("%Y/%m/%d %H:%M:%S", tempNodeJ->next->lasttime));
      utPltPutLoopVar(psDbHead,"lasttime",iNum,lasttimes);
      memset(runtimes,0,sizeof(runtimes));
      tempNodeK = tempNodeJ->next;
      //printf("free---%p\t\tnext:%p\n",tempNodeK,tempNodeJ->next->next);
      tempNodeJ->next= tempNodeJ->next->next;
      free(tempNodeK);
      tempNodeK = NULL;
    }
    
    free(Head);
    pasDbCloseCursor(psCur);
	}
	//utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_compStatic.htm");
	
  return 0;
}

