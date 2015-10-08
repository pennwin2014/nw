#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "nwdef.h"
#include "ncdef.h"

#define OUTBUFSIZE 51200
#define LAN_CLIENT_PROC 0X00000400
#define PRO_CENTER_COMMANDID_TASKKILL       0x00004000      //杀死某个进程

typedef struct _processInfo{
	unsigned int	compid;
	char	mac[24];
	char	ip[24];
	unsigned int	StartTime;   	//启动时间
	char	name[72];	// 进程名称
	char	path[256];      // 进程路径
	char	content[256];   // 进程描述
  char	indexid[48];
	char  pindexid[48];
	unsigned int		pid;
	int		cpuusage;
	int		cpus;
	int		mem;      		
}ProcessInfo1;

struct node{
	ProcessInfo1 data;
	struct node *next;
};



void showLink(struct node *head){
	struct node *linkTmp = head->next;
	
	while(linkTmp != NULL){
		printf("%s\t%u\n",linkTmp->data.name,linkTmp->data.pid);
		linkTmp = linkTmp->next;
	}
}

struct node *insertNewNode(struct node *head,ProcessInfo1 *data){	
	if(NULL == head){
		return NULL;
	}

	struct node *tmp = head;
	struct node *newNode =(struct node*)malloc(sizeof(struct node));
	
	if(newNode == NULL){
		return NULL;
	}
	newNode->next = NULL;
	newNode->data = *data;
	while(tmp->next != NULL){
		if(strcmp(tmp->next->data.pindexid,newNode->data.pindexid)>=0){
			break;
		}
		tmp = tmp->next;
	}
	newNode->next = tmp->next;
	tmp->next = newNode;
	
	
//	showLink(head);
	
	return newNode;	
}

//删除该节点后一个节点
void removeNode(struct node *removeNodePre){
	struct node *tmp;
	
	if(NULL == removeNodePre){
		return;
	}
	
	tmp = removeNodePre->next;
	
	if(tmp != NULL){
		removeNodePre->next = tmp->next;
		free(tmp);
		tmp = NULL;
	}
}

void destroyLink(struct node *head){
	struct node *tmp = head;
	
	while(head != NULL){
		head = head->next;
		free(tmp);
		tmp = head;	
	}
}
void lookParent(struct node *head){
		struct node *tmp1 = head;
		struct node *tmp2 = NULL;
		
		if(head == NULL){
			return;
		}
		
			printf("here1\n");
		while(tmp1->next!=NULL){
			tmp2 = head->next;
			while(tmp2!=NULL){
				//printf("tmp1:%s\n",tmp1->next->data.pindexid);
				if(!strcmp(tmp1->next->data.pindexid,tmp2->data.indexid)){
					break;
				}
				if(strtoul(tmp1->next->data.pindexid,NULL,10)==tmp2->data.pid){
					break;
				}
				tmp2 = tmp2->next;
			}
			if(tmp2 == NULL && strtoul(tmp1->next->data.pindexid,NULL,10) != 0){
				strcpy(tmp1->next->data.pindexid,"0");
				tmp2 = tmp1->next;
				tmp1->next = tmp2->next;
				tmp2->next = head->next;
				head->next = tmp2;
			}else{
				tmp1 = tmp1->next;
			}
		}
}

//添加树节点
void addTreeNode(char *outBuff,struct node *head,struct node *preNode){
	struct node *nodeTmp = head;
	int flag = 0;
	char time_tmp[24] = "";
	
	if(preNode->next->data.StartTime){
		strcpy(time_tmp,utTimFormat("%m/%d %H:%M:%S",preNode->next->data.StartTime));
	}
	snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),"compid:%u,pid:%u,procName:'%s',mac:'%s',ip:'%s',startTime:'%s',procPath:'%s',procContent:'%s',cpuUse:%d,memUse:%d,expanded:true,children:[",preNode->next->data.compid,preNode->next->data.pid,preNode->next->data.name,preNode->next->data.mac,preNode->next->data.ip,time_tmp,preNode->next->data.path,preNode->next->data.content,preNode->next->data.cpuusage,preNode->next->data.mem);	
	
	int iNum = 0;
	int leafFlag = 0;
	while(nodeTmp->next!=NULL){
		if(atoi(nodeTmp->next->data.pindexid) != 0 && (!strcmp(nodeTmp->next->data.pindexid,preNode->next->data.indexid) || strtoul(nodeTmp->next->data.pindexid,NULL,10)==preNode->next->data.pid)){
			flag = 1;
			leafFlag = 1;
			
			if(iNum>0){
				snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),",");
			}
			snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),"{");
			addTreeNode(outBuff,head,nodeTmp);
			iNum++;
		}
		
		if(flag == 1){
			flag = 0;
			snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),"}");
		}
		nodeTmp = nodeTmp->next;
	}
	snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),"]");
	
	if(leafFlag == 0){
		snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),",leaf:true");
	}
	
	//printf("outBuff:%s\n\n\n\n\n",outBuff);
	//removeNode(preNode);
}

int lan_ssProc_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	 utMsgPrintMsg(psMsgHead);
	 
  char compid_in[16] = "";
  char groupid_in[16] = "";
  char style_in[8] = "";
  char pid_in[16] = "";
  char flag_in[8] = "";
	
	utMsgGetSomeNVar(psMsgHead,5,
			"compid",UT_TYPE_STRING,sizeof(compid_in)-1,compid_in,
			"groupid",UT_TYPE_STRING,sizeof(groupid_in)-1,groupid_in,
			"styleId",UT_TYPE_STRING,sizeof(style_in)-1,style_in,
			"pid",UT_TYPE_STRING,sizeof(pid_in)-1,pid_in,
			"flag",UT_TYPE_STRING,sizeof(flag_in)-1,flag_in);
			
			
  utPltDbHead *psDbHead = utPltInitDb();
	if(atoi(flag_in)!=1){
	  utPltPutVar(psDbHead,"data","{text:'.'}");
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_ssproc_treeGrid.htm");
	  return 0;
	}
	
	char compidVar[24] = "";
	
	char sql[1024] = "";
	
	strcpy(sql,"select compid from nwcompute where 1=1 ");
	
	//权限
	char *auth = getDsGroupid();
	if(!utStrIsSpaces(auth)){
		sprintf(sql+strlen(sql)," and groupid in (%s)",auth);
	}
	
	if(!utStrIsSpaces(compid_in)){
		sprintf(sql+strlen(sql)," and compid = %s",compid_in);
	}
	
	if(!utStrIsSpaces(groupid_in)){
		sprintf(sql+strlen(sql)," and groupid in(%s)",getGroupid(strtoul(groupid_in,NULL,10)));
	}
	
	printf("实时进程：sql-%s\n",sql);
	pasDbCursor *psCur = pasDbOpenSql(sql, 0);
	
	unsigned long compid = 0;
	int iret;
	
	nwCompIp *psCompIp_tmp = NULL;
	
	if(psCur){
		while(0==(iret = pasDbFetchInto(psCur,
				UT_TYPE_ULONG,4,&compid)) || 1405==iret){
			psCompIp_tmp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
			
			if(psCompIp_tmp != NULL){
				break;
			}	else{
				compid = 0;
			}
		}
		
   	pasDbCloseCursor(psCur);
	}
	
	snprintf(compidVar,sizeof(compidVar),"proc%u",compid);
	utComSetVar_s(psShmHead,compidVar,pid_in);
	
	nwCompIp *psCompIp = NULL;
	psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
	
	char msg[256] = "";
	char *compname = findCompNameById(compid);
	if(psCompIp){
		if(utStrIsSpaces(pid_in)){
			psCompIp->lCtrl=LAN_CLIENT_PROC;
			snprintf(msg,sizeof(msg),"查看了电脑[%s]上的进程",compname);
			writeSysLog(psMsgHead,"06","Success",msg);
		}else{
			psCompIp->lCtrl=PRO_CENTER_COMMANDID_TASKKILL;
			snprintf(msg,sizeof(msg),"结束了电脑[%s]上的[PID:%s]进程",compname,pid_in);
			writeSysLog(psMsgHead,"06","Success",msg);
		}
	}
	free(compname);
	
	FILE *fp = NULL;
	char filePath[64];
	
	snprintf(filePath,sizeof(filePath),"/home/ncmysql/nw/online/process/%u.proc",compid);
	
//	printf("filePath:%s\n",filePath);

	sleep(2);
	
	fp = fopen(filePath,"r");
	
	if(fp == NULL){
		printf("read file failed\n");
		return -1;
	}

	printf("filePath:%s\n",filePath);	
	
//	pasFileSetFileLock(fp);
	
	ProcessInfo1 ssProcTmp;
	struct node *head =(struct node*)malloc(sizeof(struct node));
	struct node *linkTmp = NULL;
	char str[512] = "";
  
  head->next = NULL;
  char ip_tmp[24] = "";
  
  int iNum = 0;
	//读文件

	while(NULL != fgets(str,sizeof(str),fp)){
		utStrGetSomeWordBy(str,"^\r\n",13,
			UT_TYPE_ULONG,sizeof(ssProcTmp.compid),&ssProcTmp.compid,
			UT_TYPE_STRING,sizeof(ssProcTmp.mac),ssProcTmp.mac,
			UT_TYPE_STRING,sizeof(ssProcTmp.ip),ssProcTmp.ip,
			UT_TYPE_ULONG,sizeof(ssProcTmp.StartTime),&ssProcTmp.StartTime,
			UT_TYPE_STRING,sizeof(ssProcTmp.name),ssProcTmp.name,
			UT_TYPE_STRING,sizeof(ssProcTmp.path),ssProcTmp.path,
			UT_TYPE_STRING,sizeof(ssProcTmp.content),ssProcTmp.content,
			UT_TYPE_STRING,sizeof(ssProcTmp.indexid),ssProcTmp.indexid,
			UT_TYPE_STRING,sizeof(ssProcTmp.pindexid),ssProcTmp.pindexid,
			UT_TYPE_LONG,sizeof(ssProcTmp.pid),&ssProcTmp.pid,
			UT_TYPE_LONG,sizeof(ssProcTmp.cpuusage),&ssProcTmp.cpuusage,
			UT_TYPE_LONG,sizeof(ssProcTmp.cpus),&ssProcTmp.cpus,
			UT_TYPE_LONG,sizeof(ssProcTmp.mem),&ssProcTmp.mem);
			
			strcpy(ip_tmp,ssProcTmp.ip);
			
		if(!utStrIsSpaces(style_in)){
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead,"compId",iNum,"%u",ssProcTmp.compid);
			utPltPutLoopVar(psDbHead,"procName",iNum,ssProcTmp.name);
			utPltPutLoopVar(psDbHead,"mac",iNum,ssProcTmp.mac);
			utPltPutLoopVar(psDbHead,"ip",iNum,ssProcTmp.ip);
			if(ssProcTmp.StartTime>0){
				utPltPutLoopVar(psDbHead,"startTime",iNum,utTimFormat("%m/%d %H:%M:%S",ssProcTmp.StartTime));
			}else{
				utPltPutLoopVar(psDbHead,"startTime",iNum,"");
			}
			utPltPutLoopVar(psDbHead,"procPath",iNum,ssProcTmp.path);
			utPltPutLoopVar(psDbHead,"procContent",iNum,ssProcTmp.content);
			utPltPutLoopVarF(psDbHead,"pid",iNum,"%u",ssProcTmp.pid);
			utPltPutLoopVarF(psDbHead,"cpuUse",iNum,"%u",ssProcTmp.cpuusage);
			utPltPutLoopVarF(psDbHead,"memUse",iNum,"%u",ssProcTmp.mem);			
		}else{
			insertNewNode(head,&ssProcTmp);	
		}
	}
	
/*typedef struct{
	int		treeitem;
	char*	processinfo;
}ITEM;

for(int i=0;i<count;i++){
		inserttree(ITEM[i]);
}
ITEM* inserttree(ITEM* pitem)
{
	ITEM* paretn=findNoExist(pitem->pindexid);
	if((!pitem->pindexid[0])){
		pitem->treeitem=InsertTree(pitem);
		return;
	}
	if(paretn==NULL){
		pitem->treeitem=InsertTree(pitem);
		return;
	}
	if(parent->treeitem==0){
		parent->treeitem=inserttree(parent);
	}
	
	
}
	*/
	if(!utStrIsSpaces(style_in)){
		free(head);
		
	 utPltShowDb(psDbHead);
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_ssproc_grid.htm");
	}else{
		//showLink(head);
		lookParent(head);
	
		showLink(head);
		
		char outBuff[OUTBUFSIZE] = "";
		int flag = 0;
		
		snprintf(outBuff,OUTBUFSIZE,"{text:'.',children:[{procName:'IP:%s',expanded: true",ip_tmp);
		
		if(atoi(head->next->data.pindexid)==0){
			flag = 1;
			snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),",children:[");
		}
		int iNum = 0;
		//printf("linkTmp:%d\n",atoi(head->next->data.pindexid));
		linkTmp = head;
		while(linkTmp->next != NULL && atoi(linkTmp->next->data.pindexid)==0){
			if(iNum>0){
				snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),",");
			}
			iNum++;
			snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),"{");
			addTreeNode(outBuff,head,linkTmp);
			snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),"}");
			linkTmp = linkTmp->next;
		}
		
		if(flag == 1){
			snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),"]");
		}
		snprintf(outBuff+strlen(outBuff),OUTBUFSIZE-strlen(outBuff),"}]}");
		
	//	printf("outBuf:%s\n",outBuff);
	//	pasFileSetFileUnLock(fp);
		fclose(fp);
		destroyLink(head);
		 
		utPltPutVar(psDbHead,"data",outBuff);
		
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_ssproc_treeGrid.htm");
		
		if(psCompIp){
			psCompIp->lCtrl=0;
		}
	}
	return 0;
}