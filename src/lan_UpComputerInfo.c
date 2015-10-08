#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "string.h"
#include "utoall.h"
#include "ncdef.h"
#include "nwdef.h"
#include "pasdb.h"
#include <iconv.h>

typedef struct _dev{
	char	InfoType[24];      // 信息类型
	char 	hardType[128];        // 设备类型
    	char    deviceDesc[128];      // 设备描述
	struct _dev *pre;
	struct _dev *next;
}DevInfo, *pDevInfo;

typedef struct _softInfo{
    char                 InfoType[24];   //信息类型
    char		      	sname[72];	// 软件名称
    char		 	stype[20];	// 类别
    unsigned int 	ssize;          // 软件大小
    char		 	version[16];    // 版本
    char		 	instime[16];    // 安装时间
    char			business[50];   // 发行商
    struct _softInfo   *pre;
    struct _softInfo   *next;
}SoftInfo, *pSoftInfo;

typedef struct _serviceInfo{
	char		 InfoType[24];   // 信息类型
	char		 sname[72];	// 服务名称
	char                           display[72];          //   显示名称
	char		 sdemo[256];     // 服务描述
        char                           path[256];             // 路径
	char	     	 status[20];     // 服务状态
    	char		 stype[20];	// 启动类型  
    	char		 log[20];        // 登陆为	
    	struct _serviceInfo   *pre;
   	struct _serviceInfo   *next;
	
}ServiceInfo, *pServiceInfo;

typedef struct _processInfo{
	char		 InfoType[24];   // 信息类型
	char		 name[72];	// 进程名称
	char		 path[256];      // 进程路径
	char	     	 content[256];   // 进程描述
    	char		 indexid[48];
	char         	pindexid[48];
	int		pid;
	int		cpuusage;
	int		cpus;
	int		mem;
	int		vmem;  
	struct _processInf   *pre;
   	struct _processInf   *next;
      		
}ProcessInfo, *pProcessInfo;

//释放链表
int DevRelease(DevInfo *pInfoHead)
{
	DevInfo *pp = pInfoHead;
	DevInfo *pnext = NULL;
	while(pp){
		pnext = pp->next;	
		free(pp);
		pp = pnext;
	}
	return 1;
}
int SoftRelease(SoftInfo *pInfoHead)
{
	SoftInfo *pp = pInfoHead;
	SoftInfo *pnext = NULL;
	while(pp){
		pnext = pp->next;	
		free(pp);
		pp = pnext;
	}
	return 1;
}
int ServiceRelease(ServiceInfo *pInfoHead)
{
	ServiceInfo *pp = pInfoHead;
	ServiceInfo *pnext = NULL;
	while(pp){
		pnext = pp->next;	
		free(pp);
		pp = pnext;
	}
	return 1;
}
int ProcessRelease(ProcessInfo *pInfoHead)
{
	ProcessInfo *pp = pInfoHead;
	ProcessInfo *pnext = NULL;
	while(pp){
		pnext = pp->next;
		free(pp);
		pp = pnext;
	}
	return 1;
}


int8 pasGetLongTimeId();

/*
void AdjuestTextBuf(char* text){
	int len=strlen(text);
	if((len<0)||(len>256))return;
	if(text[len-1]>0)return;

	int count=0;
	int i;
	for(i=0;i<len;i++){
		if(text[i]<0)count++;
		if(count==2)count=0;
	}
	if(count==1)text[len-1]=0;
	
}
*/

int  myconvert(char * from_charset,  char * to_charset,  
		char *inbuf, size_t inlen, char * outbuf, size_t outlen)
{
	if( (NULL == from_charset)||
	    (NULL == to_charset)||
  	    (NULL == inbuf)||
	    (NULL == outbuf)||
	    (inlen <= 0)||
	    (outlen <= 0)){
		return -1;
	}

	char	buffer[1024];
	int	length = 0;
	char	mesg[512];
	memset(buffer, 0, sizeof(buffer));
	memset(mesg, 0, sizeof(mesg));
	memset(outbuf, 0, outlen);

	//准备转码
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout= &outbuf;

	cd = iconv_open(to_charset,  from_charset);
	if(0 == cd){
		snprintf(mesg, sizeof(mesg)-1, " error:%d",  errno);
		recodeLogInfo("转码open -------------", " iconv_open error", mesg);
		return -1;
	}

	snprintf(mesg, sizeof(mesg)-1, " cd:%d, pin:%0x, pout:%0x, inlen:%d, outlen:%d", cd, pin, pout, inlen, outlen);
	recodeLogInfo("转码之前1 -----------------", " ", mesg);

	snprintf(mesg, sizeof(mesg)-1,  " inlen:%d,  strlen(inbuf):%d ---",  inlen,   strlen(inbuf));
	recodeLogInfo("转码之前2 -----------------", mesg, inbuf);
	
	if(-1 == iconv(cd, pin, &inlen , pout, &outlen)){
		snprintf(mesg, sizeof(mesg)-1, " error:%d cd:%d, pin:%0x, pout:%0x, inlen:%d, outlen:%d",  errno, cd, pin, pout, inlen, outlen);
		recodeLogInfo("转码 -----------------", " iconv error", mesg);

		iconv_close(cd);
		return -1;
	}

//	snprintf(mesg, sizeof(mesg)-1, " cd:%d, pin:%0x, pout:%0x, inlen:%d, outlen:%d", cd, pin, pout, inlen, outlen);
//	recodeLogInfo("转码 -----------------", " ", mesg);

	iconv_close(cd);
	return 0;
}


int  myGBKToBIG5(char *inbuf,   char *outbuf,  int outlen)
{

	if( (NULL == inbuf)||
	    (NULL == outbuf)||
	    (outlen <= 0)){
		return -1;
	}

	char	buffer[1024];
	char	temp[1024];
	char	mesg[1024];
	memset(buffer, 0, sizeof(buffer));
	memset(temp, 0, sizeof(temp));
	memset(mesg, 0, sizeof(mesg));

	//把inbuf中  含有 转义字符'\'的汉字 多加上一个'\'
	if( strstr(inbuf, "\\") ){
		int xx =0,  yy =0;
		int  inlen = strlen(inbuf);
		for(xx= 0 ;  xx < inlen ;  xx++){
			if(inbuf[xx] == '\\'){
				temp[yy++] = '\\';
			}
			temp[yy++] =  inbuf[xx] ;
		}
		snprintf( buffer, sizeof(buffer)-1, "%s",  temp);
	}else{
		snprintf( buffer, sizeof(buffer)-1, "%s",  inbuf);
	}

	//去除多余的半个汉字字符
	AdjuestTextBuf(buffer);

	if(0 == strncmp(buffer,  "[BIG5]", 6)){
		memset(temp, 0, sizeof(temp));
		snprintf(temp, sizeof(temp)-1,  "%s",   buffer+6);
		snprintf(buffer,  sizeof(buffer)-1, "%s",   temp);
	
	}else if(0 == strncmp(buffer,  "[GBK]", 5) ){
		memset(temp, 0, sizeof(temp));
		int res = myconvert( "GBK","BIG5",   buffer+5,  strlen(buffer)-5,   temp,  sizeof(temp)-1 );
		if(res <0 ){
			snprintf(mesg, sizeof(mesg)-1, " error:%d, demo:%s",  errno, buffer);
			recodeLogInfo(" 收到一条BIG5聊天日志from:","GBK to BIG5   error0000000...................", mesg);
			return -1;
		}else{
		//	recodeLogInfo(" 收到一条BIG5聊天日志from:","GBK to BIG5   success1111111", "");
			snprintf(buffer,  sizeof(buffer)-1, "%s",   temp);
		}
	}else{
		memset(temp, 0, sizeof(temp));
		int res = myconvert( "GBK","BIG5",   buffer+5,  strlen(buffer)-5,   temp,  sizeof(temp)-1 );
		if(res <0 ){
			snprintf(mesg, sizeof(mesg)-1, " error:%d, demo:%s",  errno, buffer);
			recodeLogInfo(" 收到一条BIG5聊天日志from:","GBK to BIG5   error0000000...................", mesg);
			return -1;
		}else{
		//	recodeLogInfo(" 收到一条BIG5聊天日志from:","GBK to BIG5   success1111111", "");
			snprintf(buffer,  sizeof(buffer)-1, "%s",   temp);
		}
	}

	snprintf(outbuf,  outlen,   "%s",  buffer);
	return 0;
}

////////////////////////////////////////////////////////////////////
//Funtion: 得到数据库中机器号为compid的所有设备信息，存放于链表中
//参数：
//	 compid         : 机器的ID号，据此从数据库中查找设备信息 
//       pSqlDevInfoHead: 存放设备信息的链表的表头
//返回值:
//       0:   成功
//      -1:   失败
/////////////////////////////////////////////////////////////////////
int   GetSqlDevInfo(int compid,char * myInfoType, DevInfo ** pSqlDevInfoHead)
{
	int   	     iRet = -1;
	int   	     iReturn = 0;
	char  	     sql[2048];
	char	          mesg[512];
	pasDbCursor *psCur = NULL;
	DevInfo 	    *pCurrentPos = NULL;
	memset(sql,0, sizeof(sql));
	memset(mesg,0, sizeof(mesg));


	if(myInfoType == NULL)
		return -1;

	if(pSqlDevInfoHead == NULL){
		return -1;
	}else{
		while((*pSqlDevInfoHead)!=NULL){//如果链表不为空，则清空该链表
			DevInfo * p = *pSqlDevInfoHead;
			*pSqlDevInfoHead = (*pSqlDevInfoHead)->next;
			free(p);
		}
		*pSqlDevInfoHead = NULL;
	}
	//printf("GetSql: step1: 参数不为空\n");

	//查询语句
	snprintf(sql, sizeof(sql)-1, "select InfoType, hardType, deviceDesc from nwhardware where compid = %u and  InfoType=\'%s\'", compid, myInfoType);
	//printf("GetSql: step2: sql=%s\n", sql);

    	psCur = pasDbOpenSql(sql,0);
    	if(psCur == NULL) { //查询数据库失败，则直接返回-1
        		printf(" Do SQL Error, sqlcode is  %d, compid:%u, InfoType:%s\n", pasDbSqlCode(NULL), compid, myInfoType);
        		iRet = -1;
    	}else{ //查询数据库成?Γ蛱崛∈莸搅幢碇?
	//printf("GetSql: step3: 查询成功！\n");
		iReturn = 0;
		while( (0 == iReturn) || (1405 == iReturn)) {
			DevInfo * pNewNode = ( DevInfo *)malloc(sizeof( DevInfo));
			memset(pNewNode,0, sizeof(DevInfo));
			pNewNode->pre = NULL;
			pNewNode->next= NULL;

			iReturn = pasDbFetchInto(psCur,  UT_TYPE_STRING, sizeof(pNewNode->InfoType)-1 , pNewNode->InfoType,  
							UT_TYPE_STRING, sizeof(pNewNode->hardType)-1, pNewNode->hardType, 
							UT_TYPE_STRING, sizeof(pNewNode->deviceDesc)-1, pNewNode->deviceDesc
							);
			snprintf(mesg, sizeof(mesg)-1,  "GetSql:  设备提取结果iReturn=%d, hardtype:%s,  devicedesc=%s", iReturn,pNewNode->hardType, pNewNode->deviceDesc);
			//recodeLogInfo("设备信息：",  mesg, "");

			
		   	//如果获取数据成功,则插入到链表尾
			if((0 == iReturn) || (1405 == iReturn)){
				if(NULL == *pSqlDevInfoHead){
					*pSqlDevInfoHead =  pNewNode;
					pCurrentPos  = pNewNode;
				}else{
					pCurrentPos->next = pNewNode;
					pNewNode->pre =  pCurrentPos;
					pCurrentPos = pCurrentPos->next;
				}
			}else{//如果提取数?萁崾耍蛲顺鲅?
				free(pNewNode);
				break;
			}
		}
		pasDbCloseCursor(psCur);
		iRet = 0;
	}

	printf("GetSql: step5:iRet=%d\n",iRet);
	return iRet;
}

////////////////////////////////////////////////////////////////////
//Funtion: 得到数据库中机器号为compid的所有已安装的软?畔ⅲ娣庞诹幢碇?
//参数：
//	 compid         : 机器的ID号，据此从数据库中查找已安装的软件信息 
//       pSqlSoftInfoHead: 存放设备信息的链表的表头
//返回值:
//       0:   成功
//      -1:   失败
/////////////////////////////////////////////////////////////////////
int   GetSqlSoftInfo(int compid, SoftInfo ** pSqlSoftInfoHead)
{
	int   	     iRet = -1;
	int   	     iReturn = 0;
	char  	     sql[2048];
	char 	     mesg[512];
	pasDbCursor *psCur = NULL;
	SoftInfo     *pCurrentPos = NULL;
	memset(sql,0, sizeof(sql));
	memset(mesg,0, sizeof(mesg));
	


	if(pSqlSoftInfoHead == NULL){
		return -1;
	}else{
		while(*pSqlSoftInfoHead!=NULL){//如果链表不为空，则清空该链表
			SoftInfo * p = *pSqlSoftInfoHead;
			*pSqlSoftInfoHead = (*pSqlSoftInfoHead)->next;
			free(p);
		}
		*pSqlSoftInfoHead = NULL;
	}
	//printf("GetSql: step1: 参数不为?誠n");

	//查询语??
	snprintf(sql, sizeof(sql)-1, "select InfoType,  sname,stype, ssize,version,instime,business from nwsoftware where compid = %u", compid);
	//printf("GetSql: step2: sql=%s\n", sql);

    	psCur = pasDbOpenSql(sql,0);
    	if(psCur == NULL) { //查询数据库失败，则直接返回-1
		snprintf(mesg, sizeof(mesg)-1, "SQL Error, sqlcode is  %d, compid:%u, InfoType:soft\n", pasDbSqlCode(NULL), compid);
        		recodeLogInfo("软件信息：", mesg, "");
        		iRet = -1;
    	}else{ //查询数据库成?Γ蛱崛∈莸搅幢碇?
		iReturn = 0;
		while( (0 == iReturn) || (1405 == iReturn)) {
			SoftInfo * pNewNode = ( SoftInfo *)malloc(sizeof( SoftInfo));
			memset(pNewNode,0, sizeof(SoftInfo));
			pNewNode->pre = NULL;
			pNewNode->next= NULL;
 
			iReturn = pasDbFetchInto(psCur,  UT_TYPE_STRING, sizeof(pNewNode->InfoType)-1, pNewNode->InfoType,
							UT_TYPE_STRING, sizeof(pNewNode->sname)-1 , pNewNode->sname,  
							UT_TYPE_STRING, sizeof(pNewNode->stype)-1,  pNewNode->stype, 
							UT_TYPE_ULONG, 4, &(pNewNode->ssize),
							UT_TYPE_STRING, sizeof(pNewNode->version)-1, pNewNode->version,
							UT_TYPE_STRING, sizeof(pNewNode->instime)-1, pNewNode->instime,
							UT_TYPE_STRING, sizeof(pNewNode->business)-1, pNewNode->business				
							);
			
			snprintf(mesg, sizeof(mesg)-1,  "GetSql: 软件提取结果iReturn=%d, sname=%s, stype:%s, business:%s", iReturn,pNewNode->sname,  pNewNode->stype, pNewNode->business);
			//recodeLogInfo("软件信息：",  mesg, "");

		   	//如果获取数据成功,则插入到链表尾
			if( (0 == iReturn) || (1405 == iReturn) ){ // 0---正确，1405---有NULL值
				if(NULL == *pSqlSoftInfoHead){
					*pSqlSoftInfoHead =  pNewNode;
					pCurrentPos  = pNewNode;
				}else{
					pCurrentPos->next = pNewNode;
					pNewNode->pre =  pCurrentPos;
					pCurrentPos = pCurrentPos->next;
				}
			}else{//如果提取数据结束了，则退出循环
				free(pNewNode);
				break;
			}
		}
		pasDbCloseCursor(psCur);
		iRet = 0;
	}

	//printf("GetSql: step5:iRet=%d\n",iRet);
	return iRet;
}

////////////////////////////////////////////////////////////////////
//Funtion: 得到数据库中机器号为compid的所有已安装的软件信息，存放于链表中
//参数：
//	 compid         : 机器的ID号，据此从数据库中查找已安装的软件信息 
//       pSqlSoftInfoHead: 存放设备信息的链表的表头
//返回值:
//       0:   成功
//      -1:   失败
/////////////////////////////////////////////////////////////////////
int   GetSqlServiceInfo(int compid, ServiceInfo ** pSqlServiceInfoHead)
{
	int   	     iRet = -1;
	int   	     iReturn = 0;
	char  	     sql[2048];
	pasDbCursor *psCur = NULL;
	ServiceInfo     *pCurrentPos = NULL;
	memset(sql,0, sizeof(sql));


	if(pSqlServiceInfoHead == NULL){
		return -1;
	}else{
		while(*pSqlServiceInfoHead!=NULL){//如果链表不为空，则清空该链表
			ServiceInfo * p = *pSqlServiceInfoHead;
			*pSqlServiceInfoHead = (*pSqlServiceInfoHead)->next;
			free(p);
		}
		*pSqlServiceInfoHead = NULL;
	}
	//printf("GetSql: step1: 参数不为?誠n");

	//查询语句
	snprintf(sql, sizeof(sql)-1, "select InfoType,  sname,demo, status, stype,log from nwservice where compid = %u", compid);
	//printf("GetSql: step2: sql=%s\n", sql);

    	psCur = pasDbOpenSql(sql,0);
    	if(psCur == NULL) { //查询数据库失败，则直接返回-1
		char  mesg[512];
		memset(mesg, 0, sizeof(mesg));
        		snprintf(mesg, sizeof(mesg)-1, " Do SQL Error, sqlcode is  %d, compid:%u, InfoType:service\n", pasDbSqlCode(NULL), compid);
		recodeLogInfo("服务信息：", "GetSqlServiceInfo: ", mesg);
        		iRet = -1;
    	}else{ //查询数据库成?Γ蛱崛∈莸搅幢碇?
	//printf("GetSql: step3: 查询成功！\n");
		iReturn = 0;
		while( (0 == iReturn) || (1405 == iReturn)) {
			ServiceInfo * pNewNode = ( ServiceInfo *)malloc(sizeof( ServiceInfo));
			memset(pNewNode,0, sizeof(ServiceInfo));
			pNewNode->pre = NULL;
			pNewNode->next= NULL;
 
			iReturn = pasDbFetchInto(psCur,  UT_TYPE_STRING, 24, pNewNode->InfoType,
							UT_TYPE_STRING, 72 , pNewNode->sname,  
							UT_TYPE_STRING, 255,  pNewNode->sdemo, 
							UT_TYPE_STRING,  20,pNewNode->status,
							UT_TYPE_STRING, 20, pNewNode->stype,
							UT_TYPE_STRING, 20, pNewNode->log			
							);
			//printf("GetSql: step4: 提取结果iReturn=%d, sname=%s\n", iReturn,pNewNode->sname);
		   	//如果获取数据成功,则插入到链表尾
			if((0 == iReturn) || (1405 == iReturn)){
				if(NULL == *pSqlServiceInfoHead){
					*pSqlServiceInfoHead =  pNewNode;
					pCurrentPos  = pNewNode;
				}else{
					pCurrentPos->next = pNewNode;
					pNewNode->pre =  pCurrentPos;
					pCurrentPos = pCurrentPos->next;
				}
			}else{//如果提取数据结束了，则退出循环
				free(pNewNode);
				break;
			}
		}
		pasDbCloseCursor(psCur);
		iRet = 0;
	}

	printf("GetSql: step5:iRet=%d\n",iRet);
	return iRet;
}



//////////////////////////////////////////////////////////////////////////////////
//Funtion: ??礁隽幢斫斜冉希卸鲜?裉砑有掠布掠布约安鸪布?
//参数：
//       pSqlDevInfoHead: 从数据库中读取的设?感畔⒌牧幢淼谋硗?
//       pUpDevInfoHead: 用户上传的设备信息的链表的表头
//       pAddDevInfoHead: 增加的新设备的链表的表头
//       pDelDevInfoHead: 删除的设备的链表的表头
//返回值:
//       0:   成功
//      -1:   失败
/////////////////////////////////////////////////////////////////////////////////
int  GetDevInfoChange(DevInfo *pSqlDevInfoHead, DevInfo **pUpDevInfoHead, DevInfo **pAddDevInfoHead, DevInfo **pDelDevInfoHead)
{
	DevInfo *pCurrentAddDevInfoPos = NULL;
	DevInfo *pCurrentDelDevInfoPos = NULL;
	DevInfo *pCurrentPos = NULL;
	DevInfo *pCurrentSqlPos = pSqlDevInfoHead;
	int         flag_find = 0;

	if((pUpDevInfoHead == NULL) || (pAddDevInfoHead == NULL) || (pDelDevInfoHead == NULL)){
		return -1;
	}

	//printf("GetDevInfoChange: step1: 参数不为空\n");

	while(pCurrentSqlPos != NULL){
		pCurrentPos = *pUpDevInfoHead;
		flag_find   = 0;
		//printf("GetDevInfoChange: step2: 链表pSqlDevInfoHead 不为空\n");
		
		//对pUpDevInfoHead链表进行遍历，查找到和pSqlDevInfoHead中的当前结点相同的硬件信息，则把pUpDevInfoHead中的该结点?境?
		while(pCurrentPos != NULL){


			if((!strcmp(pCurrentSqlPos->InfoType, pCurrentPos->InfoType)) && 
			   (!strcmp(pCurrentSqlPos->hardType, pCurrentPos->hardType)) &&
			   (!strcmp(pCurrentSqlPos->deviceDesc, pCurrentPos->deviceDesc))){
				
				if((pCurrentPos->pre == NULL) && (pCurrentPos->next == NULL)){//如果当前删除的是唯一的一个结点
					*pUpDevInfoHead = NULL;
				}else if((pCurrentPos->pre == NULL) && (pCurrentPos->next != NULL)){//删除的是头结点
					pCurrentPos->next->pre = NULL;
					*pUpDevInfoHead = pCurrentPos->next;
				}else if((pCurrentPos->pre != NULL) && (pCurrentPos->next == NULL)){//删除的是尾结点
					pCurrentPos->pre->next = NULL;
				}else {                            //删除的是中间结点
					pCurrentPos->next->pre = pCurrentPos->pre;
					pCurrentPos->pre->next = pCurrentPos->next;
				}
				free(pCurrentPos);
				flag_find = 1;
				break;
			}
			pCurrentPos = pCurrentPos->next;

		}

		//printf("GetDevInfoChange: step3:从pUpDevInfoHead中查找pSqlDevInfoHead成功, flag_find=%d\n", flag_find);

		//如果在新上传的硬件信息链表中没有找到当?坝布畔ⅲ得鞲糜布徊鸪耍??迅糜布畔⒈４娴?pDelDevInfoHead链表中
		if(0 == flag_find){
			DevInfo * pNewNode = ( DevInfo *)malloc(sizeof( DevInfo));
			memset(pNewNode,0, sizeof(DevInfo));
			pNewNode->pre = NULL;
			pNewNode->next = NULL;
			snprintf(pNewNode->InfoType, sizeof(pNewNode->InfoType)-1, pCurrentSqlPos->InfoType);
			snprintf(pNewNode->hardType, sizeof(pNewNode->hardType)-1, pCurrentSqlPos->hardType);
			snprintf(pNewNode->deviceDesc, sizeof(pNewNode->deviceDesc)-1, pCurrentSqlPos->deviceDesc);
	

			if(NULL == *pDelDevInfoHead){
				*pDelDevInfoHead = pNewNode;
				pCurrentDelDevInfoPos = pNewNode;
			}else{
				pCurrentDelDevInfoPos->next = pNewNode;
				pNewNode->pre = pCurrentDelDevInfoPos;
				pCurrentDelDevInfoPos = pCurrentDelDevInfoPos->next;
			}
			//printf("GetDevInfoChange: step3.1:从pUpDevInfoHead中删除一个节点成功\n");
		}

		pCurrentSqlPos = pCurrentSqlPos->next;
	}

	*pAddDevInfoHead = *pUpDevInfoHead;
	*pUpDevInfoHead = NULL;

	//printf("GetDevInfoChange: step4:操作成功\n");
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
//Funtion: 对两个链表进行比较，判断是否安装/卸载软件
//参数：
//       pSqlSoftInfoHead: 从数据库中读取的已安装的软件信息的链表的表头
//       pUpSoftInfoHead: 用户上传的已安装的软件信息的链表的表头
//       pAddSoftInfoHead: 新安装的软件信息链表的表头
//       pDelSoftInfoHead: 卸载的软件信息链表的表头
//返回值:
//       0:   成功
//      -1:   失败
/////////////////////////////////////////////////////////////////////////////////
int  GetSoftInfoChange(SoftInfo *pSqlSoftInfoHead, SoftInfo **pUpSoftInfoHead, SoftInfo **pAddSoftInfoHead, SoftInfo **pDelSoftInfoHead)
{
	SoftInfo *pCurrentAddSoftInfoPos = NULL;
	SoftInfo *pCurrentDelSoftInfoPos = NULL;
	SoftInfo *pCurrentPos = NULL;
	SoftInfo *pCurrentSqlPos = pSqlSoftInfoHead;
	int         flag_find = 0;

	if((pUpSoftInfoHead == NULL) ||(pAddSoftInfoHead == NULL) || (pDelSoftInfoHead == NULL)){
		return -1;
	}

	//printf("GetSoftInfoChange: step1: 参数不为空\n");

	while(pCurrentSqlPos != NULL){
		pCurrentPos = *pUpSoftInfoHead;
		flag_find   = 0;
		//printf("GetSoftInfoChange: step2: 链表pSqlSoftInfoHead 不为空\n");
		
		//对pUpSoftInfoHead链表进行遍历，查找到和pSqlSoftInfoHead中的当前结点相同的软件信息，则把pUpSoftInfoHead中的该结点给删除
		while(pCurrentPos != NULL){

			if((!strcmp(pCurrentSqlPos->InfoType, pCurrentPos->InfoType))&&
			   (!strcmp(pCurrentSqlPos->sname, pCurrentPos->sname)) && 
			   (!strcmp(pCurrentSqlPos->version, pCurrentPos->version)) &&
			   (!strcmp(pCurrentSqlPos->business, pCurrentPos->business))&&
			   (!strcmp(pCurrentSqlPos->stype , pCurrentPos->stype)) &&
			   (pCurrentSqlPos->ssize == pCurrentPos->ssize)){
				
				if((pCurrentPos->pre == NULL) && (pCurrentPos->next == NULL)){//如果当前删除的是唯一的一个结点
					*pUpSoftInfoHead = NULL;
				}else if((pCurrentPos->pre == NULL) && (pCurrentPos->next != NULL)){//删除的是头结点
					pCurrentPos->next->pre = NULL;
					*pUpSoftInfoHead = pCurrentPos->next;
				}else if((pCurrentPos->pre != NULL) && (pCurrentPos->next == NULL)){//删除的是尾结点
					pCurrentPos->pre->next = NULL;
				}else {                            //删除的是中间结点
					pCurrentPos->next->pre = pCurrentPos->pre;
					pCurrentPos->pre->next = pCurrentPos->next;
				}
				free(pCurrentPos);
				flag_find = 1;
				break;
			}
			pCurrentPos = pCurrentPos->next;

		}

		//printf("GetSoftInfoChange: step3:从pUpSoftInfoHead中查找pSqlSoftInfoHead成功, flag_find=%d\n", flag_find);

		//如果在新上传的硬件信息链表中没有找到当前硬件信息，说明该硬件被拆除了，则把该硬件?畔⒈４娴?pDelSoftInfoHead链表中
		if(0 == flag_find){
			SoftInfo * pNewNode = ( SoftInfo *)malloc(sizeof( SoftInfo));
			memset(pNewNode,0, sizeof(SoftInfo));
			pNewNode->pre = NULL;
			pNewNode->next = NULL;
			pNewNode->ssize = pCurrentSqlPos->ssize;
			snprintf(pNewNode->sname, sizeof(pNewNode->sname)-1, pCurrentSqlPos->sname);
			snprintf(pNewNode->version, sizeof(pNewNode->version)-1, pCurrentSqlPos->version);
			snprintf(pNewNode->stype, sizeof(pNewNode->stype)-1, pCurrentSqlPos->stype);
			snprintf(pNewNode->business, sizeof(pNewNode->business)-1, pCurrentSqlPos->business);
			
			if(NULL == *pDelSoftInfoHead){
				*pDelSoftInfoHead = pNewNode;
				pCurrentDelSoftInfoPos = pNewNode;
			}else{
				pCurrentDelSoftInfoPos->next = pNewNode;
				pNewNode->pre = pCurrentDelSoftInfoPos;
				pCurrentDelSoftInfoPos = pCurrentDelSoftInfoPos->next;
			}
			//printf("GetSoftInfoChange: step3.1:从pUpSoftInfoHead中删除一个节点成功\n");
		}

		pCurrentSqlPos = pCurrentSqlPos->next;
	}

	*pAddSoftInfoHead = *pUpSoftInfoHead;
	*pUpSoftInfoHead  = NULL;

	//printf("GetSoftInfoChange: step4:操作成功\n");
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
//Funtion: 对两个链表进行比较，判断是否安??卸载服务
//参??
//       pSqlServiceInfoHead: 从数据库中读取的已安装的服务信息的链表的表头
//       pUpServiceInfoHead: 用户上传的已安装的服务信息的链表的表头
//       pAddServiceInfoHead: 新安装的服务信息链表的表头
//       pDelServiceInfoHead: 卸载的服务信息链表的表头
//返回值:
//       0:   成功
//      -1:   失败
/////////////////////////////////////////////////////////////////////////////////
int  GetServiceInfoChange(ServiceInfo *pSqlServiceInfoHead, ServiceInfo **pUpServiceInfoHead, ServiceInfo **pAddServiceInfoHead, ServiceInfo **pDelServiceInfoHead)
{
	ServiceInfo *pCurrentAddServiceInfoPos = NULL;
	ServiceInfo *pCurrentDelServiceInfoPos = NULL;
	ServiceInfo *pCurrentPos = NULL;
	ServiceInfo *pCurrentSqlPos = pSqlServiceInfoHead;
	int         flag_find = 0;

	if((pAddServiceInfoHead == NULL) || (pDelServiceInfoHead == NULL)){
		return -1;
	}

	//printf("GetServiceInfoChange: step1: 参数不为空\n");

	while(pCurrentSqlPos != NULL){
		pCurrentPos = *pUpServiceInfoHead;
		flag_find   = 0;
	//printf("GetServiceInfoChange: step2: 链表pSqlServiceInfoHead 不为空\n");
		
		//对pUpServiceInfoHead链表进行遍历，查找到和pSqlServiceInfoHead中的当前结点相同的硬件信息，则把pUpServiceInfoHead中的该结点给删除
		while(pCurrentPos != NULL){

			if((!strcmp(pCurrentSqlPos->InfoType, pCurrentPos->InfoType))&&
			   (!strcmp(pCurrentSqlPos->sname, pCurrentPos->sname)) && 
			   (!strcmp(pCurrentSqlPos->status, pCurrentPos->status)) &&
			   (!strcmp(pCurrentSqlPos->stype, pCurrentPos->stype))&&
			   (!strcmp(pCurrentSqlPos->sdemo , pCurrentPos->sdemo)) &&
			   (!strcmp(pCurrentSqlPos->log , pCurrentPos->log)) ){
				
				if((pCurrentPos->pre == NULL) && (pCurrentPos->next == NULL)){//如果当前删除的是唯一的一个结点
					*pUpServiceInfoHead = NULL;
				}else if((pCurrentPos->pre == NULL) && (pCurrentPos->next != NULL)){//删除的是头结点
					pCurrentPos->next->pre = NULL;
					*pUpServiceInfoHead = pCurrentPos->next;
				}else if((pCurrentPos->pre != NULL) && (pCurrentPos->next == NULL)){//删除?氖俏步岬?
					pCurrentPos->pre->next = NULL;
				}else {                            //删除的是中间结点
					pCurrentPos->next->pre = pCurrentPos->pre;
					pCurrentPos->pre->next = pCurrentPos->next;
				}
				free(pCurrentPos);
				flag_find = 1;
				break;
			}
			pCurrentPos = pCurrentPos->next;

		}

		//printf("GetServiceInfoChange: step3:从pUpServiceInfoHead中查找pSqlServiceInfoHead成功, flag_find=%d\n", flag_find);

		//如果在新上传的硬件信息链表中没有找到当前服务信息，说明该服务被删除了，则把该服务信息保存到 pDelServiceInfoHead链表中
		if(0 == flag_find){
			ServiceInfo * pNewNode = ( ServiceInfo *)malloc(sizeof( ServiceInfo));
			memset(pNewNode,0, sizeof(ServiceInfo));
			pNewNode->pre = NULL;
			pNewNode->next = NULL;
			snprintf(pNewNode->InfoType, sizeof(pNewNode->InfoType)-1, pCurrentSqlPos->InfoType);
			snprintf(pNewNode->sname, sizeof(pNewNode->sname)-1, pCurrentSqlPos->sname);
			snprintf(pNewNode->sdemo, sizeof(pNewNode->sdemo)-1, pCurrentSqlPos->sdemo);
			snprintf(pNewNode->stype, sizeof(pNewNode->stype)-1, pCurrentSqlPos->stype);
			snprintf(pNewNode->status, sizeof(pNewNode->status)-1, pCurrentSqlPos->status);
			snprintf(pNewNode->log, sizeof(pNewNode->log)-1, pCurrentSqlPos->log);

			
			if(NULL == *pDelServiceInfoHead){
				*pDelServiceInfoHead = pNewNode;
				pCurrentDelServiceInfoPos = pNewNode;
			}else{
				pCurrentDelServiceInfoPos->next = pNewNode;
				pNewNode->pre = pCurrentDelServiceInfoPos;
				pCurrentDelServiceInfoPos = pCurrentDelServiceInfoPos->next;
			}
			//printf("GetServiceInfoChange: step3.1:从pUpServiceInfoHead中删除一个节点成功\n");
		}

		pCurrentSqlPos = pCurrentSqlPos->next;
	}

	*pAddServiceInfoHead = *pUpServiceInfoHead;
	*pUpServiceInfoHead  = NULL;

	//printf("GetServiceInfoChange: step4:操作成功\n");
	return 0;
}


int Lan_UpDevInfo(utShmHead *psShmHead, int iFd,  utMsgHead *psMsgHead)
{
	unsigned int	compid = 0;
	unsigned int	userid = 0;
	char		mac[30];
	int		Nums = 0;
	char		sql[20480];
	char 		mesg[512];
	int  		iReturn  = 0;
	int		iReturn2 = 0;
	int		Status = 0;
	int 		Nums2 = 0;

	DevInfo *pUpDevInfoHead = NULL;  //客户端上传的设备信息链表的表头
	DevInfo *pSqlDevInfoHead = NULL; //数据库中存放的设备信息链表的表头
	DevInfo *pAddDevInfoHead = NULL;  //新?黾拥纳璞感畔⒘幢淼谋硗?
	DevInfo *pDelDevInfoHead = NULL;  //删除设备的链表的表头
	DevInfo *pCurrentPos = NULL;    
	pasDbCursor *psCur = NULL;

	memset(mac,0, sizeof(mac));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	//utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,4,
		                        "compid",  UT_TYPE_LONG,  4, &compid,
					"userid",  UT_TYPE_LONG,  4, &userid,
					"mac",     UT_TYPE_STRING,sizeof(mac)-1, mac,
					"Nums",    UT_TYPE_LONG,  4, &Nums		
					);
	printf("step1: compid = %u, userid = %d, mac=%s, Nums=%d\n", compid, userid, mac, Nums);

	//查询??前客户端信?
	nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);

	//获取userid
	if(psComp!= NULL){
		userid = psComp->userid;
	}
	

	char  str[3][50];
	memset(str, 0, 3*50);
	int Numbers;

	for(Numbers = 0; Numbers < Nums; Numbers++)
	{
		//创建一个节点，存储当前接收的设备信息
		DevInfo * pNewNode = ( DevInfo *)malloc(sizeof( DevInfo));
		memset(pNewNode,0, sizeof(DevInfo));
		pNewNode->pre = NULL;
		pNewNode->next= NULL;

	
		snprintf(str[0], 50, "InfoType%d", Numbers);
		snprintf(str[1], 50, "hardType%d", Numbers);		
		snprintf(str[2], 50, "deviceDesc%d", Numbers);

		iReturn = utMsgGetSomeNVar(psMsgHead,3,
					   	str[0],  UT_TYPE_STRING, sizeof(pNewNode->InfoType)-1,  pNewNode->InfoType,
						str[1],  UT_TYPE_STRING, sizeof(pNewNode->hardType)-1, pNewNode->hardType,
						str[2],  UT_TYPE_STRING, sizeof(pNewNode->deviceDesc)-1, pNewNode->deviceDesc
						);	

		//把节点插入到链表中
		if(pUpDevInfoHead == NULL){
			pUpDevInfoHead = pNewNode;
			pCurrentPos  = pNewNode;
		}else{
			pCurrentPos->next = pNewNode;
			pNewNode->pre =  pCurrentPos;
			pCurrentPos = pCurrentPos->next;
		}

		//if( (strlen(pNewNode->InfoType) == 0) || (snprintf(pNewNode->InfoType,"LocalDisk") == 0)  ){
		//	recodeLogInfo(pNewNode->InfoType,  pNewNode->hardType,  pNewNode->deviceDesc);
		//}
		snprintf(mesg, sizeof(mesg)-1, "compid:%u, mac = %s, InfoTYpe=%s, hardtype=%s, desc=%s\n",compid, mac,  pNewNode->InfoType, pNewNode->hardType,  pNewNode->deviceDesc);	
		//recodeLogInfo("设备信息：", mesg, "");	
				
	}
	recodeLogInfo("设备信息：","step2:把接收到的信息插入到链表中，操作已完成！", "");

	if(pUpDevInfoHead){
		//从数据库中取出compid的硬件设备信息到链表中
		GetSqlDevInfo(compid,pUpDevInfoHead->InfoType, &pSqlDevInfoHead);
		snprintf(mesg, sizeof(mesg)-1, "step3:从数据库中获取信息，操作已完成！infoType=%s",pUpDevInfoHead->InfoType);
		recodeLogInfo("硬件信息：", mesg, " ");
	}


	//对两个链表进行比较，?卸鲜欠裉砑有掠布筒鸪布?
	GetDevInfoChange(pSqlDevInfoHead, &pUpDevInfoHead, &pAddDevInfoHead, &pDelDevInfoHead);
	recodeLogInfo("设备信息：","step4:对获取的数据进行比较，操作已完成！", "");


	//对数据库进行操作
	//1、从数据库中删除已拆除的设备信息
	pCurrentPos = pDelDevInfoHead;
	while(NULL != pCurrentPos){

		snprintf(sql, sizeof(sql)-1, "delete from nwhardware where compid=%u and  InfoType = \'%s\' and hardType = \'%s\' and deviceDesc= \'%s\'", compid, pCurrentPos->InfoType, pCurrentPos->hardType, pCurrentPos->deviceDesc);
		iReturn2 = pasDbExecSql(sql, 0);

		pCurrentPos = pCurrentPos->next;
		
		snprintf(sql+strlen(sql), sizeof(sql)-1-strlen(sql),  "  iReturn:%d, compid:%u", iReturn2, compid);
		//recodeLogInfo("设备信息：","删除数据: ", sql);

	}
	recodeLogInfo("设备信息：", "step5:从数据库中删除设备信息，操作已完成!", "");

	//2、插??律璞感畔⒌绞菘庵?
	pCurrentPos = pAddDevInfoHead;
	while(NULL != pCurrentPos){
		unsigned long long currentTime = pasGetLongTimeId(); //获取时间
		time_t now;
		time(&now);
	
		snprintf(sql, sizeof(sql)-1, "insert into nwhardware(compid, userid, mac, InfoType,hardType, deviceDesc, addtime, lasttime, sid) values(%u,%d, \'%s\',\'%s\',\'%s\', \'%s\', %d, %d,%llu)",\
									compid,  userid,mac,  \
									pCurrentPos->InfoType, pCurrentPos->hardType, pCurrentPos->deviceDesc,\
									now, now, currentTime);		
		
		iReturn = pasDbExecSql(sql,0);
		pCurrentPos = pCurrentPos->next;

		snprintf(sql+strlen(sql), sizeof(sql)-1-strlen(sql),  "  iReturn:%d, compid:%u", iReturn, compid);
		//recodeLogInfo("设备信息：","插入数据: ", sql);
	}

	recodeLogInfo("设备信息：","step6:插入新设备信息到数据库中，操作已完成！", "");
	

	Status = 1;
	
	iReturn = pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "Status",   UT_TYPE_STRUCT,  &Status, 1,
					 "SuccessNums",     UT_TYPE_LONG,     Nums);

	snprintf(mesg, sizeof(mesg)-1, "step7:反馈信息成功  iReturn =%d, Status=%d, nums=%d\n", iReturn,Status, Nums);
	recodeLogInfo("设备信息：",mesg, "");	

	DevRelease(pUpDevInfoHead);
	DevRelease(pSqlDevInfoHead);
	DevRelease(pAddDevInfoHead);
	DevRelease(pDelDevInfoHead);
	recodeLogInfo("设备信息：","step8：链表空间释放完毕，操作已成功！", "");

}

int Lan_UpSoftInfo(utShmHead *psShmHead, int iFd,  utMsgHead *psMsgHead)
{
	unsigned int	compid = 0;
	unsigned int	userid = 0;
	char		mac[30];
	int		Nums = 0;
	char		sql[20480];
	int  		iReturn  = 0;
	int		iReturn2 = 0;
	int		Status = 0;
	int 		Nums2 = 0;

	SoftInfo *pUpSoftInfoHead = NULL;  //客户端上传的已安装的软件信息链表的表头
	SoftInfo *pSqlSoftInfoHead = NULL; //数据库中存放的已安装的软件信息链表的?硗?
	SoftInfo *pAddSoftInfoHead = NULL;  //新安装的软件信息链表的表头
	SoftInfo *pDelSoftInfoHead = NULL;  //删除已卸载的软件的链表的表头
	SoftInfo *pCurrentPos = NULL;    
	pasDbCursor *psCur = NULL;

	memset(mac,0, sizeof(mac));
	memset(sql, 0, sizeof(sql));

	//utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,4,
		                        "compid",  UT_TYPE_LONG,  4, &compid,
					"userid",  UT_TYPE_LONG,  4, &userid,
					"mac",     UT_TYPE_STRING,sizeof(mac)-1, mac,
					"Nums",    UT_TYPE_LONG,  4, &Nums		
					);

	char   mesg[512];
	memset(mesg, 0, sizeof(mesg));
	snprintf(mesg, sizeof(mesg)-1,  "step1: compid = %u, userid = %d, mac=%s, Nums=%d\n", compid, userid, mac, Nums);
	recodeLogInfo("软件信息：", mesg, "");

	//查询当前客户端信息
	nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);

	//获取userid
	if(psComp!= NULL){
		userid = psComp->userid;
	}
	

	char  str[10][50];
	memset(str, 0, 10*50);
	int Numbers;
	for(Numbers = 0; Numbers < Nums; Numbers++)
	{
		//创?ㄒ桓?诘悖娲⒌鼻敖邮盏囊寻沧暗娜砑畔?
		SoftInfo * pNewNode = ( SoftInfo *)malloc(sizeof( SoftInfo));
		memset(pNewNode,0, sizeof(SoftInfo));
		pNewNode->pre = NULL;
		pNewNode->next= NULL;

	
		snprintf(str[0], 50, "sname%d", Numbers);
		snprintf(str[1], 50, "stype%d", Numbers);		
		snprintf(str[2], 50, "ssize%d", Numbers);
		snprintf(str[3], 50, "version%d", Numbers);
		snprintf(str[4], 50, "instime%d", Numbers);
		snprintf(str[5], 50, "business%d", Numbers);
		snprintf(str[6], 50, "InfoType%d",  Numbers);

		iReturn = utMsgGetSomeNVar(psMsgHead,7,
					   	str[0],  UT_TYPE_STRING, sizeof(pNewNode->sname)-1,  pNewNode->sname,
						str[1],  UT_TYPE_STRING, sizeof(pNewNode->stype)-1, pNewNode->stype,
						str[2],  UT_TYPE_ULONG, 4,  &(pNewNode->ssize),
						str[3],  UT_TYPE_STRING, sizeof(pNewNode->version)-1, pNewNode->version,
						str[4],  UT_TYPE_STRING, sizeof(pNewNode->instime)-1, pNewNode->instime,
						str[5],   UT_TYPE_STRING, sizeof(pNewNode->business)-1,  pNewNode->business,
						str[6],     UT_TYPE_STRING, sizeof(pNewNode->InfoType)-1, pNewNode->InfoType
						);	

		if (strlen(pNewNode->sname) <= 0){
			recodeLogInfo("接收到一条软件信息：", "sname = NULL", "");
		
		}
		if (strlen(pNewNode->stype) <= 0){
			recodeLogInfo("接收到一条软件信息：", "stype = NULL", "");
		
		}

		if (strlen(pNewNode->version) <= 3){
			recodeLogInfo("接收到一条软件信息：", "version = NULL", "");
		
		}

		if (strlen(pNewNode->business) <= 3){
			recodeLogInfo("接收到一条软件信息：", "business = NULL", "");
		
		}
		

		//把节点插入到链表中
		if(pUpSoftInfoHead == NULL){
			pUpSoftInfoHead = pNewNode;
			pCurrentPos  = pNewNode;
		}else{
			pCurrentPos->next = pNewNode;
			pNewNode->pre =  pCurrentPos;
			pCurrentPos = pCurrentPos->next;
		}
				
	}

	recodeLogInfo("软件信息：", "step2:把接收到的软件信息插入到链表中成功！", "");

	//从数据库中取出compid的??件信息到链表?
	GetSqlSoftInfo(compid, &pSqlSoftInfoHead);
	recodeLogInfo("软件信息：","step3:从数据库中获取信息，操作已完成！", "");


	//对两个链表进行比较，判断是否添加新软件和卸载软件
	GetSoftInfoChange(pSqlSoftInfoHead, &pUpSoftInfoHead, &pAddSoftInfoHead, &pDelSoftInfoHead);
	recodeLogInfo("软件信息：", "step4:对获取的数据进行比较，操作已完成！", "");

 
	//对数据库进行操作
	//1、从数据库中删除已拆除的软件信息
	pCurrentPos = pDelSoftInfoHead;
	while(NULL != pCurrentPos){

		snprintf(sql, sizeof(sql)-1, "delete from nwsoftware where compid=%u and  sname = \'%s\' and stype = \'%s\' and ssize = %u and version = \'%s\' and business = \'%s\'",compid, pCurrentPos->sname, pCurrentPos->stype, pCurrentPos->ssize,pCurrentPos->version, pCurrentPos->business);
		iReturn2 = pasDbExecSql(sql, 0);

		pCurrentPos = pCurrentPos->next;

		snprintf(sql+strlen(sql), sizeof(sql)-1-strlen(sql),  "  iReturn:%d", iReturn2);
		//recodeLogInfo("软件信息：","删除数据: ", sql);
	}
	recodeLogInfo("软件信息：", "step5:从数据库中删除软件信息，操作已完成！", "");

	//2、插入新软件信息到数据库中
	pCurrentPos = pAddSoftInfoHead;
	
	//查找公司名称
	char company[32] = "";
  
  sprintf(sql,"select name from dsuser where groupid = %lu and name like '%%admin%%'",findCompanyByCompid(compid));
  writeLog("softUpload.txt","查找公司名",sql);
	pasDbOneRecord(sql, 0, UT_TYPE_STRING, sizeof(company), company);
	
	company[strlen(company)-5]=0;
	
	while(NULL != pCurrentPos){
		unsigned long long currentTime = pasGetLongTimeId(); //获取时间
		time_t now;
		time(&now);
	
		snprintf(sql, sizeof(sql)-1, "insert into nwsoftware(compid, userid, mac, sname,stype, ssize,version,instime,business, addtime, lasttime, sid) values(%u,%d, \'%s\',\'%s\',\'%s\', %u,\'%s\', \'%s\', \'%s\', %d, %d,%llu)",
									compid,  userid,mac,  
									pCurrentPos->sname, pCurrentPos->stype, pCurrentPos->ssize,pCurrentPos->version,pCurrentPos->instime,pCurrentPos->business,
									now, now, currentTime);		
		iReturn = pasDbExecSql(sql,0);
		
		//添加到进程统计中去
		sprintf(sql,"insert into nwprocStatics(statstype,processname,company)values(0,'%s','%s')",pCurrentPos->sname,company);
		writeLog("softUpload.txt","入库",sql);
		iReturn = pasDbExecSql(sql,0);
		
		pCurrentPos = pCurrentPos->next;

		snprintf(sql+strlen(sql), sizeof(sql)-1-strlen(sql),  "  iReturn:%d", iReturn);
		//recodeLogInfo("软件信息：","插入数据: ", sql);
	}
	recodeLogInfo("软件信息：",  "step6:插入新软件信息到数据库中，操作已完成！", "");

	

	Status = 1;
	
	iReturn = pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "Status",   UT_TYPE_STRUCT,  &Status, 1,
					 "SuccessNums",     UT_TYPE_LONG,     Nums);

	snprintf(mesg, sizeof(mesg)-1, "step7:反馈信息成功iReturn =%d, Status=%d, nums=%d\n", iReturn,Status, Nums);	
	recodeLogInfo("软件信息：",  mesg, "");

	//SoftRelease(pUpSoftInfoHead);
	SoftRelease(pSqlSoftInfoHead);
	SoftRelease(pAddSoftInfoHead);
	SoftRelease(pDelSoftInfoHead);
	recodeLogInfo("软件信息", "step8：链表空间释放完毕，操作已成功！", "");

}

int Lan_UpServiceInfo(utShmHead *psShmHead, int iFd,  utMsgHead *psMsgHead)
{
	//utMsgPrintMsg(psMsgHead);

	unsigned int	compid = 0;
	unsigned int	userid = 0;
	char		mac[30];
	int		Nums = 0;
	char		sql[20480];
	int  		iReturn  = 0;
	int		iReturn2 = 0;
	int		Status = 0;
	int 		Nums2 = 0;
	
	char  mesg1[1024];
	char  buffer[3096];
	memset(buffer, 0, sizeof(buffer));

	ServiceInfo *pUpServiceInfoHead = NULL;  //客户端上传的服务信息链?淼谋硗?
	ServiceInfo *pSqlServiceInfoHead = NULL; //数据库中存放的服务信息链表的表头
	ServiceInfo *pAddServiceInfoHead = NULL;  //新服务信息链表的表头
	ServiceInfo *pDelServiceInfoHead = NULL;  //删除已卸载的服务的链表的表头
	ServiceInfo *pCurrentPos = NULL;    
	pasDbCursor *psCur = NULL;

  	memset(mesg1,0, sizeof(mesg1));
	memset(mac,0, sizeof(mac));
	memset(sql, 0, sizeof(sql));

	
	iReturn = utMsgGetSomeNVar(psMsgHead,4,
		                        "compid",  UT_TYPE_LONG,  4, &compid,
					"userid",  UT_TYPE_LONG,  4, &userid,
					"mac",     UT_TYPE_STRING,sizeof(mac)-1, mac,
					"Nums",    UT_TYPE_LONG,  4, &Nums		
					);
	char   mesg[512];
	memset(mesg, 0, sizeof(mesg));
	snprintf(mesg, sizeof(mesg)-1,   "step1: compid = %u, userid = %d, mac=%s, Nums=%d\n", compid, userid, mac, Nums);
	recodeLogInfo("服务信息：",mesg, "");

	//查询当前客户端信??
	nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);

	//获取userid
	if(psComp!= NULL){
		userid = psComp->userid;
	}
	

	char  str[10][50];
	memset(str, 0, 10*50);
	int Numbers;
	for(Numbers = 0; Numbers < Nums; Numbers++)
	{
		//创建一个节点，存储当前接收的已安装的软件信息
		ServiceInfo * pNewNode = ( ServiceInfo *)malloc(sizeof( ServiceInfo));
		memset(pNewNode,0, sizeof(ServiceInfo));
		pNewNode->pre = NULL;
		pNewNode->next= NULL;

		snprintf(str[0], 50, "InfoType%d",  Numbers);
		snprintf(str[1], 50, "sname%d", Numbers);
		snprintf(str[2], 50, "display%d", Numbers);
		snprintf(str[3], 50, "sdemo%d", Numbers);
		snprintf(str[4], 50, "path%d", Numbers);
		snprintf(str[5], 50, "status%d", Numbers);
		snprintf(str[6], 50, "stype%d", Numbers);		
		snprintf(str[7], 50, "log%d", Numbers);

		iReturn = utMsgGetSomeNVar(psMsgHead,8,
						str[0],  UT_TYPE_STRING, sizeof(pNewNode->InfoType)-1, pNewNode->InfoType,
					   	str[1],  UT_TYPE_STRING,  sizeof(pNewNode->sname)-1,  pNewNode->sname,
						str[2],  UT_TYPE_STRING,  sizeof(pNewNode->display)-1,  pNewNode->display,
						str[3],  UT_TYPE_STRING,  sizeof(pNewNode->sdemo)-1, pNewNode->sdemo,
						str[4],  UT_TYPE_STRING,  sizeof(pNewNode->path)-1, pNewNode->path,
						str[5],  UT_TYPE_STRING,  sizeof(pNewNode->status)-1,  pNewNode->status,
						str[6],  UT_TYPE_STRING,  sizeof(pNewNode->stype)-1, pNewNode->stype,
						str[7],  UT_TYPE_STRING,  sizeof(pNewNode->log)-1, pNewNode->log	
						);

		int numbers;
		for( numbers = 0;  numbers < strlen(pNewNode->path); numbers++ ){
			if(pNewNode->path[numbers] == '\\'){
				pNewNode->path[numbers] = '/' ;
			}
		}

		char * ulang  = utComGetVar_sd(psShmHead,"nclang","Yes");

		//如果是中文繁体系统，则转码
		if( 0 == strcmp(ulang  ,  "big5")){
			if(myGBKToBIG5(pNewNode->sdemo,  buffer, sizeof(buffer)-1)   < 0){
				continue;
			}else{
				snprintf( pNewNode->sdemo, sizeof( pNewNode->sdemo)-1, "%s",  buffer);
			}
		}

		//把节点插入到链表中
		if(pUpServiceInfoHead == NULL){
			pUpServiceInfoHead = pNewNode;
			pCurrentPos  = pNewNode;
		}else{
			pCurrentPos->next = pNewNode;
			pNewNode->pre =  pCurrentPos;
			pCurrentPos = pCurrentPos->next;
		}
	}

	recodeLogInfo("服务信息：", "step2:把接收到的service信息插入到链表中，操作已完成！", "");

	//从数据库中?〕鯿ompid的服务信息到链表中
	GetSqlServiceInfo(compid, &pSqlServiceInfoHead);
	recodeLogInfo("服务信息：", "step3:从数据库中获取service信息到链表pSqlServiceInfoHead中!", "");


	//对两个链表进行比较，判断是否添加新服务和删除服务
	GetServiceInfoChange(pSqlServiceInfoHead, &pUpServiceInfoHead, &pAddServiceInfoHead, &pDelServiceInfoHead);
	recodeLogInfo("服务信息：", "step4:对获取的数据进行比较，操作已完成！", "");

	//对数据库进行操作
	//1、从数据库中删除已拆除的服务信息
	pCurrentPos = pDelServiceInfoHead;
	while(NULL != pCurrentPos){

		snprintf(sql, sizeof(sql)-1, "delete from nwservice where compid=%u and  InfoType = \'%s\' and  sname = \'%s\' and display= \'%s\'      and demo =  \'%s\'and path=\'%s\' and  status =  \'%s\'  and stype =  \'%s\'  and log= \'%s\'",compid,  pCurrentPos->InfoType, pCurrentPos->sname, pCurrentPos->display,  pCurrentPos->sdemo,pCurrentPos->path,  pCurrentPos->status,pCurrentPos->stype, pCurrentPos->log);
		iReturn2 = pasDbExecSql(sql, 0);

		pCurrentPos = pCurrentPos->next;
	}
	recodeLogInfo("服务信息", "step5:从数据库中删除service信息，操作已完成！", "");

	//2、插入新服务信息到数据库中
	pCurrentPos = pAddServiceInfoHead;

	while(NULL != pCurrentPos){

		unsigned long long currentTime = pasGetLongTimeId(); //获取时间
		time_t now;
		time(&now);
		snprintf(sql, sizeof(sql)-1, "insert into nwservice(compid, userid, mac,InfoType, sname,display, demo, path, status, stype,log, addtime, lasttime, sid) values(%u,%d, \'%s\',\'%s\',\'%s\',\'%s\', \'%s\', \'%s\', \'%s\', \'%s\',  \'%s\',%d, %d, %llu)",\
									compid,  userid,mac,  \
									pCurrentPos->InfoType, pCurrentPos->sname, pCurrentPos->display,  pCurrentPos->sdemo,pCurrentPos->path, pCurrentPos->status,pCurrentPos->stype, pCurrentPos->log,\
									now, now, currentTime);	
		
		iReturn = pasDbExecSql(sql,0);
		pCurrentPos = pCurrentPos->next;
	}
	recodeLogInfo("服务信息：", "step6:插入新service信息到数据库中,操作已完成！", "");

	Status = 1;
	
	iReturn = pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "Status",   UT_TYPE_STRUCT,  &Status, 1,
					 "SuccessNums",     UT_TYPE_LONG,     Nums);


	snprintf(mesg, sizeof(mesg)-1, "step7:反馈信息成功，操作已完成！compid=%u, iReturn =%d, Status=%d, nums=%d", compid, iReturn,Status, Nums);
	recodeLogInfo("服务信息：", mesg, "");	

	//ServiceRelease(pUpServiceInfoHead);
	ServiceRelease(pSqlServiceInfoHead);
	ServiceRelease(pAddServiceInfoHead);
	ServiceRelease(pDelServiceInfoHead);
	recodeLogInfo("服务信息：", "step8：链表空间释放完毕，操作已成功！", "");

}

int Lan_UpProcessInfo(utShmHead *psShmHead, int iFd,  utMsgHead *psMsgHead)
{
	unsigned int	compid = 0;
	unsigned int	userid = 0;
	char		mac[30];
	int		Nums = 0;
	char		 name[72];	// 进程名称
	char		 path[256];      // 进程路径
	char	     	 content[256];   // 进程描述
    	char		 indexid[48];
	char         	pindexid[48];
	int		pid;
	int		cpuusage;
	int		cpus;
	int		mem;
	int		vmem;  
	

	char		sql[20480];
	int  		iReturn  = 0;
	int		iReturn2 = 0;
	int		Status = 0;
	int 		Nums2 = 0;

	ProcessInfo *pUpProcessInfoHead = NULL;  //客户端上传的服务信息链表的表头
	ProcessInfo *pSqlProcessInfoHead = NULL; //数据库中存放的服务信息链表的表头
	ProcessInfo *pAddProcessInfoHead = NULL;  //新服务信息链表的表头
	ProcessInfo *pDeProcessInfoHead = NULL;  //删除已卸载的服务的链表的表头
	ProcessInfo *pCurrentPos = NULL;    
	pasDbCursor *psCur = NULL;

	memset(mac,0, sizeof(mac));
	memset(sql, 0, sizeof(sql));

	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,4,
		                        "compid",  UT_TYPE_LONG,  4, &compid,
					"userid",  UT_TYPE_LONG,  4, &userid,
					"mac",     UT_TYPE_STRING,20, mac,
					"Nums",    UT_TYPE_LONG,  4, &Nums		
					);
	printf("step1: compid = %u, userid = %d, mac=%s, Nums=%d\n", compid, userid, mac, Nums);

	//查询当前客户端信息
	nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);

	//获?serid
	if(psComp!= NULL){
		userid = psComp->userid;
	}
	
	Status = 1;
	iReturn = pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "Status",   UT_TYPE_STRUCT,  &Status, 1,
					 "SuccessNums",     UT_TYPE_LONG,     Nums);

	
}



int ncInitNwWebFun_UpComputerInfo(utShmHead *psShmHead)
{
	pasSetTcpFunName("Lan_UpDevInfo",  Lan_UpDevInfo,  NULL,0,0);
	pasSetTcpFunName("Lan_UpSoftInfo", Lan_UpSoftInfo, NULL,0,0);
	pasSetTcpFunName("Lan_UpServiceInfo", Lan_UpServiceInfo, NULL,0,0);
	pasSetTcpFunName("Lan_UpProcessInfo", Lan_UpProcessInfo, NULL,0,0);


}