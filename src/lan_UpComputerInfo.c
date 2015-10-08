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
	char	InfoType[24];      // ��Ϣ����
	char 	hardType[128];        // �豸����
    	char    deviceDesc[128];      // �豸����
	struct _dev *pre;
	struct _dev *next;
}DevInfo, *pDevInfo;

typedef struct _softInfo{
    char                 InfoType[24];   //��Ϣ����
    char		      	sname[72];	// �������
    char		 	stype[20];	// ���
    unsigned int 	ssize;          // �����С
    char		 	version[16];    // �汾
    char		 	instime[16];    // ��װʱ��
    char			business[50];   // ������
    struct _softInfo   *pre;
    struct _softInfo   *next;
}SoftInfo, *pSoftInfo;

typedef struct _serviceInfo{
	char		 InfoType[24];   // ��Ϣ����
	char		 sname[72];	// ��������
	char                           display[72];          //   ��ʾ����
	char		 sdemo[256];     // ��������
        char                           path[256];             // ·��
	char	     	 status[20];     // ����״̬
    	char		 stype[20];	// ��������  
    	char		 log[20];        // ��½Ϊ	
    	struct _serviceInfo   *pre;
   	struct _serviceInfo   *next;
	
}ServiceInfo, *pServiceInfo;

typedef struct _processInfo{
	char		 InfoType[24];   // ��Ϣ����
	char		 name[72];	// ��������
	char		 path[256];      // ����·��
	char	     	 content[256];   // ��������
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

//�ͷ�����
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

	//׼��ת��
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout= &outbuf;

	cd = iconv_open(to_charset,  from_charset);
	if(0 == cd){
		snprintf(mesg, sizeof(mesg)-1, " error:%d",  errno);
		recodeLogInfo("ת��open -------------", " iconv_open error", mesg);
		return -1;
	}

	snprintf(mesg, sizeof(mesg)-1, " cd:%d, pin:%0x, pout:%0x, inlen:%d, outlen:%d", cd, pin, pout, inlen, outlen);
	recodeLogInfo("ת��֮ǰ1 -----------------", " ", mesg);

	snprintf(mesg, sizeof(mesg)-1,  " inlen:%d,  strlen(inbuf):%d ---",  inlen,   strlen(inbuf));
	recodeLogInfo("ת��֮ǰ2 -----------------", mesg, inbuf);
	
	if(-1 == iconv(cd, pin, &inlen , pout, &outlen)){
		snprintf(mesg, sizeof(mesg)-1, " error:%d cd:%d, pin:%0x, pout:%0x, inlen:%d, outlen:%d",  errno, cd, pin, pout, inlen, outlen);
		recodeLogInfo("ת�� -----------------", " iconv error", mesg);

		iconv_close(cd);
		return -1;
	}

//	snprintf(mesg, sizeof(mesg)-1, " cd:%d, pin:%0x, pout:%0x, inlen:%d, outlen:%d", cd, pin, pout, inlen, outlen);
//	recodeLogInfo("ת�� -----------------", " ", mesg);

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

	//��inbuf��  ���� ת���ַ�'\'�ĺ��� �����һ��'\'
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

	//ȥ������İ�������ַ�
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
			recodeLogInfo(" �յ�һ��BIG5������־from:","GBK to BIG5   error0000000...................", mesg);
			return -1;
		}else{
		//	recodeLogInfo(" �յ�һ��BIG5������־from:","GBK to BIG5   success1111111", "");
			snprintf(buffer,  sizeof(buffer)-1, "%s",   temp);
		}
	}else{
		memset(temp, 0, sizeof(temp));
		int res = myconvert( "GBK","BIG5",   buffer+5,  strlen(buffer)-5,   temp,  sizeof(temp)-1 );
		if(res <0 ){
			snprintf(mesg, sizeof(mesg)-1, " error:%d, demo:%s",  errno, buffer);
			recodeLogInfo(" �յ�һ��BIG5������־from:","GBK to BIG5   error0000000...................", mesg);
			return -1;
		}else{
		//	recodeLogInfo(" �յ�һ��BIG5������־from:","GBK to BIG5   success1111111", "");
			snprintf(buffer,  sizeof(buffer)-1, "%s",   temp);
		}
	}

	snprintf(outbuf,  outlen,   "%s",  buffer);
	return 0;
}

////////////////////////////////////////////////////////////////////
//Funtion: �õ����ݿ��л�����Ϊcompid�������豸��Ϣ�������������
//������
//	 compid         : ������ID�ţ��ݴ˴����ݿ��в����豸��Ϣ 
//       pSqlDevInfoHead: ����豸��Ϣ������ı�ͷ
//����ֵ:
//       0:   �ɹ�
//      -1:   ʧ��
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
		while((*pSqlDevInfoHead)!=NULL){//�������Ϊ�գ�����ո�����
			DevInfo * p = *pSqlDevInfoHead;
			*pSqlDevInfoHead = (*pSqlDevInfoHead)->next;
			free(p);
		}
		*pSqlDevInfoHead = NULL;
	}
	//printf("GetSql: step1: ������Ϊ��\n");

	//��ѯ���
	snprintf(sql, sizeof(sql)-1, "select InfoType, hardType, deviceDesc from nwhardware where compid = %u and  InfoType=\'%s\'", compid, myInfoType);
	//printf("GetSql: step2: sql=%s\n", sql);

    	psCur = pasDbOpenSql(sql,0);
    	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���ֱ�ӷ���-1
        		printf(" Do SQL Error, sqlcode is  %d, compid:%u, InfoType:%s\n", pasDbSqlCode(NULL), compid, myInfoType);
        		iRet = -1;
    	}else{ //��ѯ���ݿ��?�������ȡ���ݵ������?
	//printf("GetSql: step3: ��ѯ�ɹ���\n");
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
			snprintf(mesg, sizeof(mesg)-1,  "GetSql:  �豸��ȡ���iReturn=%d, hardtype:%s,  devicedesc=%s", iReturn,pNewNode->hardType, pNewNode->deviceDesc);
			//recodeLogInfo("�豸��Ϣ��",  mesg, "");

			
		   	//�����ȡ���ݳɹ�,����뵽����β
			if((0 == iReturn) || (1405 == iReturn)){
				if(NULL == *pSqlDevInfoHead){
					*pSqlDevInfoHead =  pNewNode;
					pCurrentPos  = pNewNode;
				}else{
					pCurrentPos->next = pNewNode;
					pNewNode->pre =  pCurrentPos;
					pCurrentPos = pCurrentPos->next;
				}
			}else{//�����ȡ��?ݽ����ˣ����˳�ѭ�?
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
//Funtion: �õ����ݿ��л�����Ϊcompid�������Ѱ�װ����?���Ϣ������������?
//������
//	 compid         : ������ID�ţ��ݴ˴����ݿ��в����Ѱ�װ�������Ϣ 
//       pSqlSoftInfoHead: ����豸��Ϣ������ı�ͷ
//����ֵ:
//       0:   �ɹ�
//      -1:   ʧ��
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
		while(*pSqlSoftInfoHead!=NULL){//�������Ϊ�գ�����ո�����
			SoftInfo * p = *pSqlSoftInfoHead;
			*pSqlSoftInfoHead = (*pSqlSoftInfoHead)->next;
			free(p);
		}
		*pSqlSoftInfoHead = NULL;
	}
	//printf("GetSql: step1: ������Ϊ?�\n");

	//��ѯ��??
	snprintf(sql, sizeof(sql)-1, "select InfoType,  sname,stype, ssize,version,instime,business from nwsoftware where compid = %u", compid);
	//printf("GetSql: step2: sql=%s\n", sql);

    	psCur = pasDbOpenSql(sql,0);
    	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���ֱ�ӷ���-1
		snprintf(mesg, sizeof(mesg)-1, "SQL Error, sqlcode is  %d, compid:%u, InfoType:soft\n", pasDbSqlCode(NULL), compid);
        		recodeLogInfo("�����Ϣ��", mesg, "");
        		iRet = -1;
    	}else{ //��ѯ���ݿ��?�������ȡ���ݵ������?
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
			
			snprintf(mesg, sizeof(mesg)-1,  "GetSql: �����ȡ���iReturn=%d, sname=%s, stype:%s, business:%s", iReturn,pNewNode->sname,  pNewNode->stype, pNewNode->business);
			//recodeLogInfo("�����Ϣ��",  mesg, "");

		   	//�����ȡ���ݳɹ�,����뵽����β
			if( (0 == iReturn) || (1405 == iReturn) ){ // 0---��ȷ��1405---��NULLֵ
				if(NULL == *pSqlSoftInfoHead){
					*pSqlSoftInfoHead =  pNewNode;
					pCurrentPos  = pNewNode;
				}else{
					pCurrentPos->next = pNewNode;
					pNewNode->pre =  pCurrentPos;
					pCurrentPos = pCurrentPos->next;
				}
			}else{//�����ȡ���ݽ����ˣ����˳�ѭ��
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
//Funtion: �õ����ݿ��л�����Ϊcompid�������Ѱ�װ�������Ϣ�������������
//������
//	 compid         : ������ID�ţ��ݴ˴����ݿ��в����Ѱ�װ�������Ϣ 
//       pSqlSoftInfoHead: ����豸��Ϣ������ı�ͷ
//����ֵ:
//       0:   �ɹ�
//      -1:   ʧ��
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
		while(*pSqlServiceInfoHead!=NULL){//�������Ϊ�գ�����ո�����
			ServiceInfo * p = *pSqlServiceInfoHead;
			*pSqlServiceInfoHead = (*pSqlServiceInfoHead)->next;
			free(p);
		}
		*pSqlServiceInfoHead = NULL;
	}
	//printf("GetSql: step1: ������Ϊ?�\n");

	//��ѯ���
	snprintf(sql, sizeof(sql)-1, "select InfoType,  sname,demo, status, stype,log from nwservice where compid = %u", compid);
	//printf("GetSql: step2: sql=%s\n", sql);

    	psCur = pasDbOpenSql(sql,0);
    	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���ֱ�ӷ���-1
		char  mesg[512];
		memset(mesg, 0, sizeof(mesg));
        		snprintf(mesg, sizeof(mesg)-1, " Do SQL Error, sqlcode is  %d, compid:%u, InfoType:service\n", pasDbSqlCode(NULL), compid);
		recodeLogInfo("������Ϣ��", "GetSqlServiceInfo: ", mesg);
        		iRet = -1;
    	}else{ //��ѯ���ݿ��?�������ȡ���ݵ������?
	//printf("GetSql: step3: ��ѯ�ɹ���\n");
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
			//printf("GetSql: step4: ��ȡ���iReturn=%d, sname=%s\n", iReturn,pNewNode->sname);
		   	//�����ȡ���ݳɹ�,����뵽����β
			if((0 == iReturn) || (1405 == iReturn)){
				if(NULL == *pSqlServiceInfoHead){
					*pSqlServiceInfoHead =  pNewNode;
					pCurrentPos  = pNewNode;
				}else{
					pCurrentPos->next = pNewNode;
					pNewNode->pre =  pCurrentPos;
					pCurrentPos = pCurrentPos->next;
				}
			}else{//�����ȡ���ݽ����ˣ����˳�ѭ��
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
//Funtion: ??���������бȽϣ��ж��?������Ӳ��������Ӳ�����Լ����Ӳ�?
//������
//       pSqlDevInfoHead: �����ݿ��ж�ȡ����?���Ϣ������ı��?
//       pUpDevInfoHead: �û��ϴ����豸��Ϣ������ı�ͷ
//       pAddDevInfoHead: ���ӵ����豸������ı�ͷ
//       pDelDevInfoHead: ɾ�����豸������ı�ͷ
//����ֵ:
//       0:   �ɹ�
//      -1:   ʧ��
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

	//printf("GetDevInfoChange: step1: ������Ϊ��\n");

	while(pCurrentSqlPos != NULL){
		pCurrentPos = *pUpDevInfoHead;
		flag_find   = 0;
		//printf("GetDevInfoChange: step2: ����pSqlDevInfoHead ��Ϊ��\n");
		
		//��pUpDevInfoHead������б��������ҵ���pSqlDevInfoHead�еĵ�ǰ�����ͬ��Ӳ����Ϣ�����pUpDevInfoHead�еĸý��?�ɾ�?
		while(pCurrentPos != NULL){


			if((!strcmp(pCurrentSqlPos->InfoType, pCurrentPos->InfoType)) && 
			   (!strcmp(pCurrentSqlPos->hardType, pCurrentPos->hardType)) &&
			   (!strcmp(pCurrentSqlPos->deviceDesc, pCurrentPos->deviceDesc))){
				
				if((pCurrentPos->pre == NULL) && (pCurrentPos->next == NULL)){//�����ǰɾ������Ψһ��һ�����
					*pUpDevInfoHead = NULL;
				}else if((pCurrentPos->pre == NULL) && (pCurrentPos->next != NULL)){//ɾ������ͷ���
					pCurrentPos->next->pre = NULL;
					*pUpDevInfoHead = pCurrentPos->next;
				}else if((pCurrentPos->pre != NULL) && (pCurrentPos->next == NULL)){//ɾ������β���
					pCurrentPos->pre->next = NULL;
				}else {                            //ɾ�������м���
					pCurrentPos->next->pre = pCurrentPos->pre;
					pCurrentPos->pre->next = pCurrentPos->next;
				}
				free(pCurrentPos);
				flag_find = 1;
				break;
			}
			pCurrentPos = pCurrentPos->next;

		}

		//printf("GetDevInfoChange: step3:��pUpDevInfoHead�в���pSqlDevInfoHead�ɹ�, flag_find=%d\n", flag_find);

		//��������ϴ���Ӳ����Ϣ������û���ҵ���?�Ӳ����Ϣ��˵����Ӳ��������ˣ??Ѹ�Ӳ����Ϣ����?pDelDevInfoHead������
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
			//printf("GetDevInfoChange: step3.1:��pUpDevInfoHead��ɾ��һ���ڵ�ɹ�\n");
		}

		pCurrentSqlPos = pCurrentSqlPos->next;
	}

	*pAddDevInfoHead = *pUpDevInfoHead;
	*pUpDevInfoHead = NULL;

	//printf("GetDevInfoChange: step4:�����ɹ�\n");
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
//Funtion: ������������бȽϣ��ж��Ƿ�װ/ж�����
//������
//       pSqlSoftInfoHead: �����ݿ��ж�ȡ���Ѱ�װ�������Ϣ������ı�ͷ
//       pUpSoftInfoHead: �û��ϴ����Ѱ�װ�������Ϣ������ı�ͷ
//       pAddSoftInfoHead: �°�װ�������Ϣ����ı�ͷ
//       pDelSoftInfoHead: ж�ص������Ϣ����ı�ͷ
//����ֵ:
//       0:   �ɹ�
//      -1:   ʧ��
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

	//printf("GetSoftInfoChange: step1: ������Ϊ��\n");

	while(pCurrentSqlPos != NULL){
		pCurrentPos = *pUpSoftInfoHead;
		flag_find   = 0;
		//printf("GetSoftInfoChange: step2: ����pSqlSoftInfoHead ��Ϊ��\n");
		
		//��pUpSoftInfoHead������б��������ҵ���pSqlSoftInfoHead�еĵ�ǰ�����ͬ�������Ϣ�����pUpSoftInfoHead�еĸý���ɾ��
		while(pCurrentPos != NULL){

			if((!strcmp(pCurrentSqlPos->InfoType, pCurrentPos->InfoType))&&
			   (!strcmp(pCurrentSqlPos->sname, pCurrentPos->sname)) && 
			   (!strcmp(pCurrentSqlPos->version, pCurrentPos->version)) &&
			   (!strcmp(pCurrentSqlPos->business, pCurrentPos->business))&&
			   (!strcmp(pCurrentSqlPos->stype , pCurrentPos->stype)) &&
			   (pCurrentSqlPos->ssize == pCurrentPos->ssize)){
				
				if((pCurrentPos->pre == NULL) && (pCurrentPos->next == NULL)){//�����ǰɾ������Ψһ��һ�����
					*pUpSoftInfoHead = NULL;
				}else if((pCurrentPos->pre == NULL) && (pCurrentPos->next != NULL)){//ɾ������ͷ���
					pCurrentPos->next->pre = NULL;
					*pUpSoftInfoHead = pCurrentPos->next;
				}else if((pCurrentPos->pre != NULL) && (pCurrentPos->next == NULL)){//ɾ������β���
					pCurrentPos->pre->next = NULL;
				}else {                            //ɾ�������м���
					pCurrentPos->next->pre = pCurrentPos->pre;
					pCurrentPos->pre->next = pCurrentPos->next;
				}
				free(pCurrentPos);
				flag_find = 1;
				break;
			}
			pCurrentPos = pCurrentPos->next;

		}

		//printf("GetSoftInfoChange: step3:��pUpSoftInfoHead�в���pSqlSoftInfoHead�ɹ�, flag_find=%d\n", flag_find);

		//��������ϴ���Ӳ����Ϣ������û���ҵ���ǰӲ����Ϣ��˵����Ӳ��������ˣ���Ѹ�Ӳ��?�Ϣ����?pDelSoftInfoHead������
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
			//printf("GetSoftInfoChange: step3.1:��pUpSoftInfoHead��ɾ��һ���ڵ�ɹ�\n");
		}

		pCurrentSqlPos = pCurrentSqlPos->next;
	}

	*pAddSoftInfoHead = *pUpSoftInfoHead;
	*pUpSoftInfoHead  = NULL;

	//printf("GetSoftInfoChange: step4:�����ɹ�\n");
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
//Funtion: ������������бȽϣ��ж��Ƿ�??ж�ط���
//��?��?
//       pSqlServiceInfoHead: �����ݿ��ж�ȡ���Ѱ�װ�ķ�����Ϣ������ı�ͷ
//       pUpServiceInfoHead: �û��ϴ����Ѱ�װ�ķ�����Ϣ������ı�ͷ
//       pAddServiceInfoHead: �°�װ�ķ�����Ϣ����ı�ͷ
//       pDelServiceInfoHead: ж�صķ�����Ϣ����ı�ͷ
//����ֵ:
//       0:   �ɹ�
//      -1:   ʧ��
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

	//printf("GetServiceInfoChange: step1: ������Ϊ��\n");

	while(pCurrentSqlPos != NULL){
		pCurrentPos = *pUpServiceInfoHead;
		flag_find   = 0;
	//printf("GetServiceInfoChange: step2: ����pSqlServiceInfoHead ��Ϊ��\n");
		
		//��pUpServiceInfoHead������б��������ҵ���pSqlServiceInfoHead�еĵ�ǰ�����ͬ��Ӳ����Ϣ�����pUpServiceInfoHead�еĸý���ɾ��
		while(pCurrentPos != NULL){

			if((!strcmp(pCurrentSqlPos->InfoType, pCurrentPos->InfoType))&&
			   (!strcmp(pCurrentSqlPos->sname, pCurrentPos->sname)) && 
			   (!strcmp(pCurrentSqlPos->status, pCurrentPos->status)) &&
			   (!strcmp(pCurrentSqlPos->stype, pCurrentPos->stype))&&
			   (!strcmp(pCurrentSqlPos->sdemo , pCurrentPos->sdemo)) &&
			   (!strcmp(pCurrentSqlPos->log , pCurrentPos->log)) ){
				
				if((pCurrentPos->pre == NULL) && (pCurrentPos->next == NULL)){//�����ǰɾ������Ψһ��һ�����
					*pUpServiceInfoHead = NULL;
				}else if((pCurrentPos->pre == NULL) && (pCurrentPos->next != NULL)){//ɾ������ͷ���
					pCurrentPos->next->pre = NULL;
					*pUpServiceInfoHead = pCurrentPos->next;
				}else if((pCurrentPos->pre != NULL) && (pCurrentPos->next == NULL)){//ɾ��?���β��?
					pCurrentPos->pre->next = NULL;
				}else {                            //ɾ�������м���
					pCurrentPos->next->pre = pCurrentPos->pre;
					pCurrentPos->pre->next = pCurrentPos->next;
				}
				free(pCurrentPos);
				flag_find = 1;
				break;
			}
			pCurrentPos = pCurrentPos->next;

		}

		//printf("GetServiceInfoChange: step3:��pUpServiceInfoHead�в���pSqlServiceInfoHead�ɹ�, flag_find=%d\n", flag_find);

		//��������ϴ���Ӳ����Ϣ������û���ҵ���ǰ������Ϣ��˵���÷���ɾ���ˣ���Ѹ÷�����Ϣ���浽 pDelServiceInfoHead������
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
			//printf("GetServiceInfoChange: step3.1:��pUpServiceInfoHead��ɾ��һ���ڵ�ɹ�\n");
		}

		pCurrentSqlPos = pCurrentSqlPos->next;
	}

	*pAddServiceInfoHead = *pUpServiceInfoHead;
	*pUpServiceInfoHead  = NULL;

	//printf("GetServiceInfoChange: step4:�����ɹ�\n");
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

	DevInfo *pUpDevInfoHead = NULL;  //�ͻ����ϴ����豸��Ϣ����ı�ͷ
	DevInfo *pSqlDevInfoHead = NULL; //���ݿ��д�ŵ��豸��Ϣ����ı�ͷ
	DevInfo *pAddDevInfoHead = NULL;  //��?��ӵ��豸��Ϣ����ı��?
	DevInfo *pDelDevInfoHead = NULL;  //ɾ���豸������ı�ͷ
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

	//��ѯ??ǰ�ͻ�����?
	nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);

	//��ȡuserid
	if(psComp!= NULL){
		userid = psComp->userid;
	}
	

	char  str[3][50];
	memset(str, 0, 3*50);
	int Numbers;

	for(Numbers = 0; Numbers < Nums; Numbers++)
	{
		//����һ���ڵ㣬�洢��ǰ���յ��豸��Ϣ
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

		//�ѽڵ���뵽������
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
		//recodeLogInfo("�豸��Ϣ��", mesg, "");	
				
	}
	recodeLogInfo("�豸��Ϣ��","step2:�ѽ��յ�����Ϣ���뵽�����У���������ɣ�", "");

	if(pUpDevInfoHead){
		//�����ݿ���ȡ��compid��Ӳ���豸��Ϣ��������
		GetSqlDevInfo(compid,pUpDevInfoHead->InfoType, &pSqlDevInfoHead);
		snprintf(mesg, sizeof(mesg)-1, "step3:�����ݿ��л�ȡ��Ϣ����������ɣ�infoType=%s",pUpDevInfoHead->InfoType);
		recodeLogInfo("Ӳ����Ϣ��", mesg, " ");
	}


	//������������бȽϣ�?ж��Ƿ������Ӳ���Ͳ��Ӳ�?
	GetDevInfoChange(pSqlDevInfoHead, &pUpDevInfoHead, &pAddDevInfoHead, &pDelDevInfoHead);
	recodeLogInfo("�豸��Ϣ��","step4:�Ի�ȡ�����ݽ��бȽϣ���������ɣ�", "");


	//�����ݿ���в���
	//1�������ݿ���ɾ���Ѳ�����豸��Ϣ
	pCurrentPos = pDelDevInfoHead;
	while(NULL != pCurrentPos){

		snprintf(sql, sizeof(sql)-1, "delete from nwhardware where compid=%u and  InfoType = \'%s\' and hardType = \'%s\' and deviceDesc= \'%s\'", compid, pCurrentPos->InfoType, pCurrentPos->hardType, pCurrentPos->deviceDesc);
		iReturn2 = pasDbExecSql(sql, 0);

		pCurrentPos = pCurrentPos->next;
		
		snprintf(sql+strlen(sql), sizeof(sql)-1-strlen(sql),  "  iReturn:%d, compid:%u", iReturn2, compid);
		//recodeLogInfo("�豸��Ϣ��","ɾ������: ", sql);

	}
	recodeLogInfo("�豸��Ϣ��", "step5:�����ݿ���ɾ���豸��Ϣ�����������!", "");

	//2����??��豸��Ϣ�����ݿ��?
	pCurrentPos = pAddDevInfoHead;
	while(NULL != pCurrentPos){
		unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
		time_t now;
		time(&now);
	
		snprintf(sql, sizeof(sql)-1, "insert into nwhardware(compid, userid, mac, InfoType,hardType, deviceDesc, addtime, lasttime, sid) values(%u,%d, \'%s\',\'%s\',\'%s\', \'%s\', %d, %d,%llu)",\
									compid,  userid,mac,  \
									pCurrentPos->InfoType, pCurrentPos->hardType, pCurrentPos->deviceDesc,\
									now, now, currentTime);		
		
		iReturn = pasDbExecSql(sql,0);
		pCurrentPos = pCurrentPos->next;

		snprintf(sql+strlen(sql), sizeof(sql)-1-strlen(sql),  "  iReturn:%d, compid:%u", iReturn, compid);
		//recodeLogInfo("�豸��Ϣ��","��������: ", sql);
	}

	recodeLogInfo("�豸��Ϣ��","step6:�������豸��Ϣ�����ݿ��У���������ɣ�", "");
	

	Status = 1;
	
	iReturn = pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "Status",   UT_TYPE_STRUCT,  &Status, 1,
					 "SuccessNums",     UT_TYPE_LONG,     Nums);

	snprintf(mesg, sizeof(mesg)-1, "step7:������Ϣ�ɹ�  iReturn =%d, Status=%d, nums=%d\n", iReturn,Status, Nums);
	recodeLogInfo("�豸��Ϣ��",mesg, "");	

	DevRelease(pUpDevInfoHead);
	DevRelease(pSqlDevInfoHead);
	DevRelease(pAddDevInfoHead);
	DevRelease(pDelDevInfoHead);
	recodeLogInfo("�豸��Ϣ��","step8������ռ��ͷ���ϣ������ѳɹ���", "");

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

	SoftInfo *pUpSoftInfoHead = NULL;  //�ͻ����ϴ����Ѱ�װ�������Ϣ����ı�ͷ
	SoftInfo *pSqlSoftInfoHead = NULL; //���ݿ��д�ŵ��Ѱ�װ�������Ϣ�����?��?
	SoftInfo *pAddSoftInfoHead = NULL;  //�°�װ�������Ϣ����ı�ͷ
	SoftInfo *pDelSoftInfoHead = NULL;  //ɾ����ж�ص����������ı�ͷ
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
	recodeLogInfo("�����Ϣ��", mesg, "");

	//��ѯ��ǰ�ͻ�����Ϣ
	nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);

	//��ȡuserid
	if(psComp!= NULL){
		userid = psComp->userid;
	}
	

	char  str[10][50];
	memset(str, 0, 10*50);
	int Numbers;
	for(Numbers = 0; Numbers < Nums; Numbers++)
	{
		//��?�һ�?ڵ㣬�洢��ǰ���յ��Ѱ�װ��������?
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
			recodeLogInfo("���յ�һ�������Ϣ��", "sname = NULL", "");
		
		}
		if (strlen(pNewNode->stype) <= 0){
			recodeLogInfo("���յ�һ�������Ϣ��", "stype = NULL", "");
		
		}

		if (strlen(pNewNode->version) <= 3){
			recodeLogInfo("���յ�һ�������Ϣ��", "version = NULL", "");
		
		}

		if (strlen(pNewNode->business) <= 3){
			recodeLogInfo("���յ�һ�������Ϣ��", "business = NULL", "");
		
		}
		

		//�ѽڵ���뵽������
		if(pUpSoftInfoHead == NULL){
			pUpSoftInfoHead = pNewNode;
			pCurrentPos  = pNewNode;
		}else{
			pCurrentPos->next = pNewNode;
			pNewNode->pre =  pCurrentPos;
			pCurrentPos = pCurrentPos->next;
		}
				
	}

	recodeLogInfo("�����Ϣ��", "step2:�ѽ��յ��������Ϣ���뵽�����гɹ���", "");

	//�����ݿ���ȡ��compid��??����Ϣ������?
	GetSqlSoftInfo(compid, &pSqlSoftInfoHead);
	recodeLogInfo("�����Ϣ��","step3:�����ݿ��л�ȡ��Ϣ����������ɣ�", "");


	//������������бȽϣ��ж��Ƿ�����������ж�����
	GetSoftInfoChange(pSqlSoftInfoHead, &pUpSoftInfoHead, &pAddSoftInfoHead, &pDelSoftInfoHead);
	recodeLogInfo("�����Ϣ��", "step4:�Ի�ȡ�����ݽ��бȽϣ���������ɣ�", "");

 
	//�����ݿ���в���
	//1�������ݿ���ɾ���Ѳ���������Ϣ
	pCurrentPos = pDelSoftInfoHead;
	while(NULL != pCurrentPos){

		snprintf(sql, sizeof(sql)-1, "delete from nwsoftware where compid=%u and  sname = \'%s\' and stype = \'%s\' and ssize = %u and version = \'%s\' and business = \'%s\'",compid, pCurrentPos->sname, pCurrentPos->stype, pCurrentPos->ssize,pCurrentPos->version, pCurrentPos->business);
		iReturn2 = pasDbExecSql(sql, 0);

		pCurrentPos = pCurrentPos->next;

		snprintf(sql+strlen(sql), sizeof(sql)-1-strlen(sql),  "  iReturn:%d", iReturn2);
		//recodeLogInfo("�����Ϣ��","ɾ������: ", sql);
	}
	recodeLogInfo("�����Ϣ��", "step5:�����ݿ���ɾ�������Ϣ����������ɣ�", "");

	//2�������������Ϣ�����ݿ���
	pCurrentPos = pAddSoftInfoHead;
	
	//���ҹ�˾����
	char company[32] = "";
  
  sprintf(sql,"select name from dsuser where groupid = %lu and name like '%%admin%%'",findCompanyByCompid(compid));
  writeLog("softUpload.txt","���ҹ�˾��",sql);
	pasDbOneRecord(sql, 0, UT_TYPE_STRING, sizeof(company), company);
	
	company[strlen(company)-5]=0;
	
	while(NULL != pCurrentPos){
		unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
		time_t now;
		time(&now);
	
		snprintf(sql, sizeof(sql)-1, "insert into nwsoftware(compid, userid, mac, sname,stype, ssize,version,instime,business, addtime, lasttime, sid) values(%u,%d, \'%s\',\'%s\',\'%s\', %u,\'%s\', \'%s\', \'%s\', %d, %d,%llu)",
									compid,  userid,mac,  
									pCurrentPos->sname, pCurrentPos->stype, pCurrentPos->ssize,pCurrentPos->version,pCurrentPos->instime,pCurrentPos->business,
									now, now, currentTime);		
		iReturn = pasDbExecSql(sql,0);
		
		//��ӵ�����ͳ����ȥ
		sprintf(sql,"insert into nwprocStatics(statstype,processname,company)values(0,'%s','%s')",pCurrentPos->sname,company);
		writeLog("softUpload.txt","���",sql);
		iReturn = pasDbExecSql(sql,0);
		
		pCurrentPos = pCurrentPos->next;

		snprintf(sql+strlen(sql), sizeof(sql)-1-strlen(sql),  "  iReturn:%d", iReturn);
		//recodeLogInfo("�����Ϣ��","��������: ", sql);
	}
	recodeLogInfo("�����Ϣ��",  "step6:�����������Ϣ�����ݿ��У���������ɣ�", "");

	

	Status = 1;
	
	iReturn = pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "Status",   UT_TYPE_STRUCT,  &Status, 1,
					 "SuccessNums",     UT_TYPE_LONG,     Nums);

	snprintf(mesg, sizeof(mesg)-1, "step7:������Ϣ�ɹ�iReturn =%d, Status=%d, nums=%d\n", iReturn,Status, Nums);	
	recodeLogInfo("�����Ϣ��",  mesg, "");

	//SoftRelease(pUpSoftInfoHead);
	SoftRelease(pSqlSoftInfoHead);
	SoftRelease(pAddSoftInfoHead);
	SoftRelease(pDelSoftInfoHead);
	recodeLogInfo("�����Ϣ", "step8������ռ��ͷ���ϣ������ѳɹ���", "");

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

	ServiceInfo *pUpServiceInfoHead = NULL;  //�ͻ����ϴ��ķ�����Ϣ��?�ı��?
	ServiceInfo *pSqlServiceInfoHead = NULL; //���ݿ��д�ŵķ�����Ϣ����ı�ͷ
	ServiceInfo *pAddServiceInfoHead = NULL;  //�·�����Ϣ����ı�ͷ
	ServiceInfo *pDelServiceInfoHead = NULL;  //ɾ����ж�صķ��������ı�ͷ
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
	recodeLogInfo("������Ϣ��",mesg, "");

	//��ѯ��ǰ�ͻ�����??
	nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);

	//��ȡuserid
	if(psComp!= NULL){
		userid = psComp->userid;
	}
	

	char  str[10][50];
	memset(str, 0, 10*50);
	int Numbers;
	for(Numbers = 0; Numbers < Nums; Numbers++)
	{
		//����һ���ڵ㣬�洢��ǰ���յ��Ѱ�װ�������Ϣ
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

		//��������ķ���ϵͳ����ת��
		if( 0 == strcmp(ulang  ,  "big5")){
			if(myGBKToBIG5(pNewNode->sdemo,  buffer, sizeof(buffer)-1)   < 0){
				continue;
			}else{
				snprintf( pNewNode->sdemo, sizeof( pNewNode->sdemo)-1, "%s",  buffer);
			}
		}

		//�ѽڵ���뵽������
		if(pUpServiceInfoHead == NULL){
			pUpServiceInfoHead = pNewNode;
			pCurrentPos  = pNewNode;
		}else{
			pCurrentPos->next = pNewNode;
			pNewNode->pre =  pCurrentPos;
			pCurrentPos = pCurrentPos->next;
		}
	}

	recodeLogInfo("������Ϣ��", "step2:�ѽ��յ���service��Ϣ���뵽�����У���������ɣ�", "");

	//�����ݿ���?���compid�ķ�����Ϣ��������
	GetSqlServiceInfo(compid, &pSqlServiceInfoHead);
	recodeLogInfo("������Ϣ��", "step3:�����ݿ��л�ȡservice��Ϣ������pSqlServiceInfoHead��!", "");


	//������������бȽϣ��ж��Ƿ�����·����ɾ������
	GetServiceInfoChange(pSqlServiceInfoHead, &pUpServiceInfoHead, &pAddServiceInfoHead, &pDelServiceInfoHead);
	recodeLogInfo("������Ϣ��", "step4:�Ի�ȡ�����ݽ��бȽϣ���������ɣ�", "");

	//�����ݿ���в���
	//1�������ݿ���ɾ���Ѳ���ķ�����Ϣ
	pCurrentPos = pDelServiceInfoHead;
	while(NULL != pCurrentPos){

		snprintf(sql, sizeof(sql)-1, "delete from nwservice where compid=%u and  InfoType = \'%s\' and  sname = \'%s\' and display= \'%s\'      and demo =  \'%s\'and path=\'%s\' and  status =  \'%s\'  and stype =  \'%s\'  and log= \'%s\'",compid,  pCurrentPos->InfoType, pCurrentPos->sname, pCurrentPos->display,  pCurrentPos->sdemo,pCurrentPos->path,  pCurrentPos->status,pCurrentPos->stype, pCurrentPos->log);
		iReturn2 = pasDbExecSql(sql, 0);

		pCurrentPos = pCurrentPos->next;
	}
	recodeLogInfo("������Ϣ", "step5:�����ݿ���ɾ��service��Ϣ����������ɣ�", "");

	//2�������·�����Ϣ�����ݿ���
	pCurrentPos = pAddServiceInfoHead;

	while(NULL != pCurrentPos){

		unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
		time_t now;
		time(&now);
		snprintf(sql, sizeof(sql)-1, "insert into nwservice(compid, userid, mac,InfoType, sname,display, demo, path, status, stype,log, addtime, lasttime, sid) values(%u,%d, \'%s\',\'%s\',\'%s\',\'%s\', \'%s\', \'%s\', \'%s\', \'%s\',  \'%s\',%d, %d, %llu)",\
									compid,  userid,mac,  \
									pCurrentPos->InfoType, pCurrentPos->sname, pCurrentPos->display,  pCurrentPos->sdemo,pCurrentPos->path, pCurrentPos->status,pCurrentPos->stype, pCurrentPos->log,\
									now, now, currentTime);	
		
		iReturn = pasDbExecSql(sql,0);
		pCurrentPos = pCurrentPos->next;
	}
	recodeLogInfo("������Ϣ��", "step6:������service��Ϣ�����ݿ���,��������ɣ�", "");

	Status = 1;
	
	iReturn = pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 3,
					 "compid",   UT_TYPE_LONG,   compid,
					 "Status",   UT_TYPE_STRUCT,  &Status, 1,
					 "SuccessNums",     UT_TYPE_LONG,     Nums);


	snprintf(mesg, sizeof(mesg)-1, "step7:������Ϣ�ɹ�����������ɣ�compid=%u, iReturn =%d, Status=%d, nums=%d", compid, iReturn,Status, Nums);
	recodeLogInfo("������Ϣ��", mesg, "");	

	//ServiceRelease(pUpServiceInfoHead);
	ServiceRelease(pSqlServiceInfoHead);
	ServiceRelease(pAddServiceInfoHead);
	ServiceRelease(pDelServiceInfoHead);
	recodeLogInfo("������Ϣ��", "step8������ռ��ͷ���ϣ������ѳɹ���", "");

}

int Lan_UpProcessInfo(utShmHead *psShmHead, int iFd,  utMsgHead *psMsgHead)
{
	unsigned int	compid = 0;
	unsigned int	userid = 0;
	char		mac[30];
	int		Nums = 0;
	char		 name[72];	// ��������
	char		 path[256];      // ����·��
	char	     	 content[256];   // ��������
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

	ProcessInfo *pUpProcessInfoHead = NULL;  //�ͻ����ϴ��ķ�����Ϣ����ı�ͷ
	ProcessInfo *pSqlProcessInfoHead = NULL; //���ݿ��д�ŵķ�����Ϣ����ı�ͷ
	ProcessInfo *pAddProcessInfoHead = NULL;  //�·�����Ϣ����ı�ͷ
	ProcessInfo *pDeProcessInfoHead = NULL;  //ɾ����ж�صķ��������ı�ͷ
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

	//��ѯ��ǰ�ͻ�����Ϣ
	nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);

	//��?�userid
	if(psComp!= NULL){
		userid = psComp->userid;
	}
	
	Status = 1;
	iReturn = pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 3,
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