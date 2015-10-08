#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>
#include "string.h"
#include "utoall.h"
#include "utomon.h"
#include "ncdef.h"
#include "nwdef.h"
#include "pasdb.h"
#include "lan_md5.h"
#include <stdlib.h>
#include <errno.h>


int8 pasGetLongTimeId();


#include <sys/stat.h>
#include <sys/types.h>


#define  DISTRIBUTEFILE		"/home/ncmysql/nw/DistributeFile"
#define  BACKUPFILEPATH		"/home/ncmysql/nw/BackUpFile"
#define RESPONSEBG	990118198

#define  PACKAGE_SIZE	10*1024*1024

int MyTcpRecv(int iSocket,char *sBuf,int iBuflen)
{
	int iRet;
	int iLen;
	
	iRet=1;
	iLen=0;
	while(iRet>0)
	{
		iRet=recv(iSocket,sBuf+iLen,iBuflen-iLen,0);
		if(iRet>0)
		{
			iLen+=iRet;
		}

		if(iLen>=iBuflen)
			return iBuflen;

	}
	return iLen;
}


int MypasMsgHeadInit(int iSock,utMsgHead *psMsgHead0,char *pKey,short nVarNum,...)
{
    char *pVar;
    char *pValue;
    short nType,nValue;
    char caNum[8];
    long  lValue,lMsgCode,lBytes;
    long long llValue;
    register int i,iReturn;
    utMsgHead *psMsgHead;
    va_list ap;
    lMsgCode = psMsgHead0->lCode / 1000;
    lMsgCode = lMsgCode * 1000 + 102;
    va_start(ap, nVarNum);
    psMsgHead = utMsgInit(RESPONSEBG,psMsgHead0->lFunCode,psMsgHead0->lMsgId,5*1024*1024);
    if(psMsgHead == (utMsgHead *)NULL) 
        return (-1);
    psMsgHead->lReserve[0] = psMsgHead0->lReserve[1];
    psMsgHead->lReserve[1] = psMsgHead0->lReserve[0];
    psMsgHead->lFunCode  = psMsgHead0->lFunCode;
    for(i=0;i<nVarNum;i++) {
        pVar = va_arg(ap,char *);
        nType = va_arg(ap,int);
        switch (nType) {
            case UT_TYPE_SHORT:
                nValue = va_arg(ap,int);
                utMsgAdd_n(&psMsgHead,pVar,nValue);
                break;
            case UT_TYPE_ULONG:    
            case UT_TYPE_LONG:
                lValue = va_arg(ap,long);
                utMsgAdd_l(&psMsgHead,pVar,lValue);
                break;
            case UT_TYPE_STRING:
                pValue = va_arg(ap,char *);
                utMsgAdd_s(&psMsgHead,pVar,pValue);
                break;
            case UT_TYPE_STRUCT:
                pValue = va_arg(ap,char *);
                lBytes = va_arg(ap,long);
                utMsgAdd_v(&psMsgHead,pVar,pValue,lBytes);
                break;
            case UT_TYPE_LONG8:
                llValue = va_arg(ap,long long);
                utMsgAdd_ll(&psMsgHead,pVar,llValue);
                break;
            default:
                utLogError(" Unknow Type");
                pValue = va_arg(ap,char *);
                break;
                
        }
    }
    va_end(ap);
    return iReturn;
}


int MypasMsgResponse(int iSock,utMsgHead *psMsgHead,char *pKey)
{

    //int iReturn = pasTcpMsgSend(iSock,psMsgHead,pKey);
    
	int iReturn = write(iSock,(char *)psMsgHead,psMsgHead->lBytes);

  //  utMsgFree(psMsgHead);
    return iReturn ;
}

int    RemoveHeartCommand(utShmHead *psShmHead , unsigned int compid,  int command)
{
	if(NULL == psShmHead ){
		return 0;
	}

	nwCompIp * psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
	if(!psCompIp){
		return -1;	
	}else{
		psCompIp->lCtrl &= ~command;
		return 1;
	}

	return 0;
}


int  Lan_SoftwareUp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int	len = 0;
	int	number = 0;
	int	sqltotal =0;
	int	sqlnumber = 0;
	char	MD5[64];
	char	unique[64];
	char 	*mydata = NULL;
	char  targetpath [256];
	char 	mesg[512];
	char	sql[1024];

	memset(MD5, 0, sizeof(MD5));
	memset(unique, 0, sizeof(unique));
	memset(targetpath  , 0, sizeof(targetpath ));
	memset(mesg, 0, sizeof(mesg));
	memset(sql, 0, sizeof(sql));

	//��ȡ�ؼ��ֶ�
	int iReturn = utMsgGetSomeNVar(psMsgHead,4,
		                        "len",  UT_TYPE_ULONG,  4,  &len,
		                        "number",  UT_TYPE_ULONG,  4,  &number,
					"MD5",  UT_TYPE_STRING,  sizeof(MD5)-1,  MD5,
					"unique",    UT_TYPE_STRING,  sizeof(unique)-1,  unique);

	mydata = malloc(len+1);
	if(!mydata){
		snprintf(mesg, sizeof(mesg)-1, "len:%d, number:%d, MD5:%s, unique:%s",  len, number, MD5, unique);
		recodeLogInfo("����ϴ� ", "�������� ��̬���ٿռ�ʧ��", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					 "state",   UT_TYPE_LONG,    -3,
					 "path",   UT_TYPE_STRING,  "");
		return -3;
	}

	memset(mydata, 0, len+1);
	int ilen = MyTcpRecv(iFd, mydata, len);

	if( (len <= 0)||
	    (0==strcmp("", MD5))||
	    (0==strcmp("", unique))){
		snprintf(mesg, sizeof(mesg)-1, "len:%d, number:%d,  MD5:%s,  unique:%s",  len, number,  MD5,  unique);
		recodeLogInfo("����ϴ� ", "��������  ", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					 "state",   UT_TYPE_LONG,    -2,
					 "path",   UT_TYPE_STRING,  "");
		return -2;
	}

	if(ilen != len){
		recodeLogInfo("����ϴ� ", "��������  ", " �������ݳ�������");
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					 "state",   UT_TYPE_LONG,    -6,
					 "path",   UT_TYPE_STRING,  "");
		free(mydata );
		return -6;
	}


	//��ѯ���ݿ⣬�ҵ���ǰunique��Ӧ���ļ�·��
	snprintf(sql, sizeof(sql)-1, "select total, number, targetpath  from    nwSoftwareInfo   where packageunique=\'%s\'  ",   unique);
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
        		recodeLogInfo("��nwSoftwareInfo���ݱ��в�ѯ����ʧ�ܣ�" ,"" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -4);
		return -4;
        	}else{
		iReturn = pasDbFetchInto(psCur,   UT_TYPE_LONG, 4, &sqltotal ,
						UT_TYPE_LONG, 4, &sqlnumber ,
						UT_TYPE_STRING, sizeof(targetpath)-1, targetpath);
		pasDbCloseCursor(psCur);

		if((iReturn!=0)  || (sqltotal <= 0)|| (strcmp("", targetpath )==0)){//���û�в�ѯ����¼
			snprintf(mesg, sizeof(mesg)-1, "Error:û����ȡ������  iReturn:%d,  sqltotal:%u,  sqlnumber:%d,  targetpath:%s",  iReturn, sqltotal,  sqlnumber,  targetpath);
			recodeLogInfo("����ϴ� ",  mesg, sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					 "state",   UT_TYPE_LONG,    -5,
					 "path",   UT_TYPE_STRING,  "");
			return -5;
		}

		if( (sqltotal <= sqlnumber) ||
		     (number < sqlnumber)){
			snprintf(mesg, sizeof(mesg)-1, "sqltotal:%u,  sqlnumber:%d,  number:%d",  sqltotal,  sqlnumber,  number);
			recodeLogInfo("����ϴ� ", "�ļ����Ѿ�������ϣ�  ", mesg);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ?�Կ����ʱ���? */ 2,
					 "state",   UT_TYPE_LONG,    2,
					 "path",   UT_TYPE_STRING,  "");
			return 2;
		}
	}

	snprintf(mesg, sizeof(mesg)-1, "len:%d,  MD5:%s, unique:%s, ilen :%d",  len, MD5,  unique, ilen);
	//recodeLogInfo("����ϴ� ", "����?��? ", mesg);

	//�õ�MD5ֵ
	char   md5_res[64];
	memset(md5_res, 0, sizeof(md5_res)); 
	lan_GetMD5((unsigned char*)mydata, ilen , (unsigned char*)md5_res);

	snprintf(mesg, sizeof(mesg)-1, "len:%d,  MD5:%s, ��֤MD5 :%s, unique:%s, ilen :%d,",  len, MD5,md5_res,  unique, ilen);
	recodeLogInfo("����ϴ� ", "��֤MD5  ", mesg);

	if(0 != strcmp(MD5,  md5_res)){
		recodeLogInfo("����ϴ� ", "��������  ", " MD5��֤ʧ��");
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					 "state",   UT_TYPE_LONG,    -7,
					 "path",   UT_TYPE_STRING,  "");

		free(mydata);
		return -7;
	}

	FILE * fp = fopen(targetpath, "a+");
	if(!fp){
		recodeLogInfo("����ϴ� ", "��·��ʧ��  ", targetpath);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					 "state",   UT_TYPE_LONG,    -8,
					 "path",   UT_TYPE_STRING,  "");

		free(mydata );
		return -8;
	}

	fwrite(mydata, 1, ilen , fp);
	fclose(fp);

	int  state = (number+len >= sqltotal)?2:1;
	//�޸����ݿ�
	snprintf(sql ,sizeof(sql)-1,  "update nwSoftwareInfo  set  number = %d, state=%d  where packageunique=\"%s\" ",  number+len,  state,  unique);
	if(pasDbExecSql(sql, 0)){
		recodeLogInfo("���� number��nwSoftwareInfo���ݱ���ʧ�ܣ�", " " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					 "state",   UT_TYPE_LONG,    -9,
					 "path",   UT_TYPE_STRING,  "");
		free(mydata );
		return -9;	
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ��?? */ 2,
					 "state",   UT_TYPE_LONG,    1,
					 "path",   UT_TYPE_STRING,  targetpath);

	recodeLogInfo("����ϴ� ", "��ʼ  ", "����");

	free(mydata );
	return 0;
}
int  Lan_SoftwareInfoUp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char	compname[128];
	char	name[128];		//��������
	time_t	createtime=0;		//����ʱ��
	time_t	droptime  =0;		//�޸�ʱ��
	char	droper[128];		//�޸���
	int	system =0;			//����ϵͳ
	int	language=0;	//ϵͳ����

	int	size=0;			//�ļ���С
	char	computer[128];		//���ڼ����
	char	path[256];		//�ļ�Դ·��
	char	targetpath[256];		//�ļ�Ŀ��·��
	char	commandline[128];	//������
	int	dmodel=0;		//�ַ�ģʽ
	int	rmodel=0;		//����ģʽ
	char	filename[128];		//�ļ���
	char	fileversion[128];	//�ļ��汾
	time_t	filedroptime=0;		//�ļ��޸�ʱ��

	int	DAnyone=0;		//������� ����һ������
	int	NAnyone=0;		//��Ҫ���� ����һ������

	int	total =0;		//���ݰ�����
	int	number=0;		//��ǰ���͵��ڼ���
	char	packageunique[128];      //����Ψһ��ʶ��
	char	MD5[128];        	//�ļ�MD5
	char	companyid[128];		//�ϴ��ߵĹ�˾

	char	sql[2048];
	char	mesg[512];
	char	oldMD5[128];
	char	oldtargetpath[512];
	char	newunique[128];
	int	updatemodel= 0;
	unsigned int	MyTaskRunList[10240]={0};
	
	memset(compname, 0, sizeof(compname));
	memset(name, 0, sizeof(name));
	memset(droper, 0, sizeof(droper));
	memset(computer, 0, sizeof(computer));
	memset(path, 0, sizeof(path));
	memset(targetpath, 0, sizeof(targetpath));
	memset(commandline, 0, sizeof(commandline));
	memset(filename, 0, sizeof(filename));
	memset(fileversion, 0, sizeof(fileversion));
	memset(packageunique, 0, sizeof(packageunique));
	memset(MD5, 0, sizeof(MD5));
	memset(companyid, 0, sizeof(companyid));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));
	memset(oldMD5, 0, sizeof(oldMD5));
	memset(oldtargetpath, 0, sizeof(oldtargetpath));
	memset(newunique, 0, sizeof(newunique));


	int iReturn = utMsgGetSomeNVar(psMsgHead,24,
		                        "name",   	UT_TYPE_STRING, sizeof(name)-1, name,
					"createtime",  	UT_TYPE_LONG,  4, &createtime,
					"droptime",    	UT_TYPE_LONG,  4, &droptime,
					"droper",    	UT_TYPE_STRING,  sizeof(droper)-1, droper,
					"os",    	UT_TYPE_LONG,  4, &system,
					"systemlanguage", UT_TYPE_LONG,  4, &language,
					"size",   	UT_TYPE_LONG,  4, &size,
					"computer",    	UT_TYPE_STRING,  sizeof(computer)-1, computer,
					"path",    	UT_TYPE_STRING,  sizeof(path)-1, path,
					"commandline",  UT_TYPE_STRING,  sizeof(commandline)-1, commandline,
					"dmodel",    	UT_TYPE_LONG,  4, &dmodel,
					"rmodel",    	UT_TYPE_LONG,  4, &rmodel,
					"filename",   	UT_TYPE_STRING,  sizeof(filename)-1, filename,
					"fileversion",  UT_TYPE_STRING,  sizeof(fileversion)-1, fileversion,
					"filedroptime", UT_TYPE_LONG,  4, &filedroptime,
					"danyone",    	UT_TYPE_LONG,  4, &DAnyone,
					"nanyone",    	UT_TYPE_LONG,  4, &NAnyone,
					"total",    	UT_TYPE_LONG,  4, &total,
					"number",    	UT_TYPE_LONG,  4, &number,
					"unique",    	UT_TYPE_STRING,  sizeof(packageunique)-1, packageunique,
					"MD5",    	UT_TYPE_STRING,  sizeof(MD5)-1, MD5,
					"companyid",	UT_TYPE_STRING,  sizeof(companyid)-1, companyid,
					"updatemodel",  UT_TYPE_LONG, 4, &updatemodel,
					"compname",	UT_TYPE_STRING, sizeof(compname)-1, compname);

	//��� unique �ֶ��Ƿ�Ϊ��
	if(0==strcmp("", packageunique)){
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -2);
		return -2;
	}

	//���� �ļ�����·�� targetpath
	time_t  now;
	struct tm *time1;
	time(&now);
	time1 = localtime(&now);

	char	year[24];
	char	mon[24];
	memset(year, 0, sizeof(year));
	memset(mon, 0, sizeof(mon));		
	//��ǰ���
	snprintf(year, 5, "%d", time1->tm_year+1900);
	//��?��·?
	if( (time1->tm_mon+1 > 0) && (time1->tm_mon+1 <= 9) ){
		snprintf(mon, 3, "0%d", time1->tm_mon+1);
	}else{
		snprintf(mon, 3, "%d", time1->tm_mon+1 );
	}

	char  savedir[512];
	memset(savedir, 0, sizeof(savedir));
	snprintf(savedir, sizeof(savedir)-1, "%s", DISTRIBUTEFILE);
	mkdir(savedir, S_IRWXU);

	snprintf(savedir, sizeof(savedir)-1, "%s/%s%s", DISTRIBUTEFILE,  year, mon);
	mkdir(savedir, S_IRWXU);

	snprintf(savedir, sizeof(savedir)-1, "%s/%s%s/%s%u", DISTRIBUTEFILE,  year, mon, MD5, droptime);
	snprintf(targetpath, sizeof(targetpath)-1, "%s",  savedir);

	//�жϸ��ļ��Ƿ��Ѿ��ϴ����ˣ�����ϴ�����Ӧ���£��������
	snprintf(sql, sizeof(sql)-1, "select  MD5, targetpath  from    nwSoftwareInfo   where packageunique=\'%s\'  ",   packageunique);
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
        		recodeLogInfo("��nwSoftwareInfo���ݱ��в�ѯ����ʧ�ܣ�" ,"" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        	}else{
		iReturn = pasDbFetchInto(psCur,  UT_TYPE_STRING, sizeof(oldMD5)-1, oldMD5,
						UT_TYPE_STRING, sizeof(oldtargetpath)-1, oldtargetpath);
		pasDbCloseCursor(psCur);
		if(strlen(oldMD5)==0){
			//���û�鵽��¼�������һ����¼
			snprintf(sql, sizeof(sql)-1, "insert into nwSoftwareInfo(name, createtime, droptime, droper, system, language," 
				" size, state, computer, path, targetpath, command," 
				" dmodel, rmodel, filename, fileversion, filedroptime, "
				"  DAnyone, NAnyone, total, number , packageunique, MD5, companyid )"
				"  values(\'%s\', %u, %u, \'%s\',  %d, %d, "
				" %d, %d,  \'%s\',  \'%s\',  \'%s\',  \'%s\',"
				"  %d, %d, \'%s\',  \'%s\',  %u,"
				"  %d, %d,  %d, %d,  \'%s\', \'%s\', \'%s\')",
				name,  createtime,  droptime  , droper, system, language, 
				size,  0,  computer,  path,  targetpath,  commandline,
				dmodel,  rmodel,  filename, fileversion, filedroptime,
				DAnyone, NAnyone,   size , number,  packageunique, MD5,  compname);
			if(pasDbExecSql(sql, 0)){
				//recodeLogInfo("�����¼��nwSoftwareInfo���ݱ�?�ʧ�ܣ?, " " , sql);
				pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -4);
				return -4;	
			}else{
				pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
				return 0;
			}
		}
	}

	//��������޸������
	recodeLogInfo("��Unique�����Ѿ��ϴ��� " ," �޸������" , sql);
			

	//����ǵͼ���ģʽ�����жϸ�������Ƿ��·����ͻ��ˣ�����·��ˣ����޸�ʧ��
	if(0 == updatemodel){

		int 	flag=0;
		snprintf(sql, sizeof(sql)-1, "select taskunique from nwSoftwareDistribution  where  packageunique=\'%s\' ",  packageunique);

/*		memset(newunique, 0, sizeof(newunique));
		int iReturn = pasDbOneRecord(sql,0,UT_TYPE_STRING, sizeof(newunique)-1, newunique);
/*		if( iReturn != 0){ 
			recodeLogInfo(" �޸������"," ��ѯ���ݿ�nwSoftwareDistributionʧ��!", sql);
			pasTcpResponse(iFd,psMsgHead,NULL,  1,"state",   UT_TYPE_LONG,    -3);
			return -3;
		}

		if( 0 != strlen(newunique)){
			flag = 1;
		}

*/
		pasDbCursor *psCur = pasDbOpenSql(sql,0);
		if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
			recodeLogInfo(" �޸������"," ��ѯ���ݿ�nwSoftwareDistributionʧ��!", sql);
			pasTcpResponse(iFd,psMsgHead,NULL, 1,"state",   UT_TYPE_LONG,    -3);
			return -3;
        		}else{
			int 	iReturn =0;
			while(iReturn == 0){
				memset(newunique, 0, sizeof(newunique));
				iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(newunique)-1, newunique);
				if(0 != strlen(newunique)){				
					flag = 1;
					//break;			
				}
			}
			pasDbCloseCursor(psCur);
		}

			if(1 == flag){
				recodeLogInfo(" �޸������"," ȷ���޸���Ӧ�������б�!", "");
				pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    2);
				return 2;
			}else{
				snprintf(newunique, sizeof(newunique)-1,  "%s%u",  MD5,  droptime);
				int flag = 0;
				if(0 == strcmp(MD5, oldMD5)){
					flag = 1;
					snprintf(sql, sizeof(sql)-1, "update nwSoftwareInfo"
						" set  nwSoftwareInfo.name=\'%s\', 	nwSoftwareInfo.droptime=%u, 	nwSoftwareInfo.droper = \'%s\', 	nwSoftwareInfo.system=%d, "
						" nwSoftwareInfo.language=%d,		nwSoftwareInfo.size= %d,   	nwSoftwareInfo.computer= \'%s\',	nwSoftwareInfo.path= \'%s\',  "
						" nwSoftwareInfo.command= \'%s\',	nwSoftwareInfo.dmodel=%d, 	nwSoftwareInfo.rmodel=%d, 	nwSoftwareInfo.filename= \'%s\', "
						" nwSoftwareInfo.fileversion= \'%s\', 	nwSoftwareInfo.filedroptime=%u, 	nwSoftwareInfo.DAnyone=%d, 	nwSoftwareInfo.NAnyone=%d, "
						" nwSoftwareInfo.total=%d,               nwSoftwareInfo.number=%d, 	nwSoftwareInfo.packageunique= \'%s\', nwSoftwareInfo.MD5=\'%s\' "
						" where  nwSoftwareInfo.packageunique=\'%s\' ",

						name, droptime  , droper, system, 
						language, size,  computer, path, 
						commandline, dmodel, rmodel, filename, 
						fileversion, filedroptime, DAnyone, NAnyone, 
						size, number , newunique, MD5,  packageunique);
				}else{
					flag = 2;
					snprintf(sql, sizeof(sql)-1, "update nwSoftwareInfo"
						" set  nwSoftwareInfo.name=\'%s\', 	nwSoftwareInfo.droptime=%u, 	nwSoftwareInfo.droper = \'%s\', 	nwSoftwareInfo.system=%d, "
						" nwSoftwareInfo.language=%d,		nwSoftwareInfo.size= %d,   	nwSoftwareInfo.computer= \'%s\',	nwSoftwareInfo.path= \'%s\',  "
						" nwSoftwareInfo.command= \'%s\',	nwSoftwareInfo.dmodel=%d, 	nwSoftwareInfo.rmodel=%d, 	nwSoftwareInfo.filename= \'%s\', "
						" nwSoftwareInfo.fileversion= \'%s\', 	nwSoftwareInfo.filedroptime=%u, 	nwSoftwareInfo.DAnyone=%d, 	nwSoftwareInfo.NAnyone=%d, "
						" nwSoftwareInfo.total=%d,               nwSoftwareInfo.number=%d, 	nwSoftwareInfo.packageunique= \'%s\', nwSoftwareInfo.MD5=\'%s\', "
						" nwSoftwareInfo.state=%d,  nwSoftwareInfo.targetpath= \'%s\'"
						" where  nwSoftwareInfo.packageunique = \'%s\' ",

						name, droptime  , droper, system, 
						language, size,  computer, path, 
						commandline, dmodel, rmodel, filename, 
						fileversion, filedroptime, DAnyone, NAnyone, 
						size, number , newunique, MD5,  
						0, targetpath,  packageunique);
				}

				if(pasDbExecSql(sql, 0)){
					recodeLogInfo("  �޸������!",   " �޸������ʧ��: " , sql);
					pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -6);
					return -6;	
				}else{
					recodeLogInfo("  �޸��������", " �޸�������ɹ�: " , sql);
				}

				if(1 == flag){
					pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    3);
					return 3;
				}else{
					//ɾ�������Դ�ļ�
					if(!remove(oldtargetpath)){
						recodeLogInfo("  �޸��������", " �޸������Դ�ļ�ʧ��: " , oldtargetpath);
					}

					pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    4);
					return 4;
				}
			
		}
	}else{//����Ǹ߼���ģʽ����ǿ�Ƹ��� �������deletetask == 1��
		//��ѯ��ǰ������·�����Щ��������ȡ��Щ������ID
		snprintf(sql, sizeof(sql)-1, "select computerid from nwSoftwareDistribution, nwSoftwareDistributionSpeed  "
			" where  nwSoftwareDistribution.taskunique= nwSoftwareDistributionSpeed.taskunique  and  packageunique=\'%s\' ",  packageunique);
		pasDbCursor *psCur = pasDbOpenSql(sql,0);
		if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
			recodeLogInfo(" �޸��������"," ��ѯcomputerid ʧ��!", sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -6);
			return -6;
        		}else{
			int 	iReturn =0;
			while(iReturn == 0){
				int  compid= 0;
				iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &compid );
				if(0 != iReturn){
					break;
				}

				snprintf(mesg, sizeof(mesg)-1, " compid:%u ",    compid);
				recodeLogInfo("  �޸��������", "��ѯ��compid : " , mesg);

				MyTaskRunList[number++] = compid;
			}
			pasDbCloseCursor(psCur);
		}

		//�޸��������Ϣ�Լ���Ӧ�������б�
		snprintf(newunique, sizeof(newunique)-1,  "%s%u",  MD5,  droptime);
		int flag = 0;
		
		if(0 == strcmp(MD5, oldMD5)){
			flag = 1;
			snprintf(sql, sizeof(sql)-1, "update nwSoftwareInfo, nwSoftwareDistribution "
				" set  nwSoftwareInfo.name=\'%s\', 	nwSoftwareInfo.droptime=%u, 	nwSoftwareInfo.droper = \'%s\', 	nwSoftwareInfo.system=%d, "
				" nwSoftwareInfo.language=%d,		nwSoftwareInfo.size= %d,   	nwSoftwareInfo.computer= \'%s\',	nwSoftwareInfo.path= \'%s\',  "
				" nwSoftwareInfo.command= \'%s\',	nwSoftwareInfo.dmodel=%d, 	nwSoftwareInfo.rmodel=%d, 	nwSoftwareInfo.filename= \'%s\', "
				" nwSoftwareInfo.fileversion= \'%s\', 	nwSoftwareInfo.filedroptime=%u, 	nwSoftwareInfo.DAnyone=%d, 	nwSoftwareInfo.NAnyone=%d, "
				" nwSoftwareInfo.total=%d, 		nwSoftwareInfo.number=%d, 	nwSoftwareInfo.packageunique= \'%s\', nwSoftwareInfo.MD5=\'%s\', "
				" nwSoftwareDistribution.packageunique=\'%s\' "
				" where  nwSoftwareInfo.packageunique = nwSoftwareDistribution.packageunique "
				" and  nwSoftwareInfo.packageunique=\'%s\' ",  

				name, droptime  , droper, system, 
				language, size,  computer, path, 
				commandline, dmodel, rmodel, filename, 
				fileversion, filedroptime, DAnyone, NAnyone, 
				size, number , newunique, MD5,  
				newunique,packageunique);
		}else{
			flag = 2;
			snprintf(sql, sizeof(sql)-1, "update nwSoftwareInfo, nwSoftwareDistribution "
				" set  nwSoftwareInfo.name=\'%s\', 	nwSoftwareInfo.droptime=%u, 	nwSoftwareInfo.droper = \'%s\', 	nwSoftwareInfo.system=%d, "
				" nwSoftwareInfo.language=%d,		nwSoftwareInfo.size= %d,   	nwSoftwareInfo.computer= \'%s\',	nwSoftwareInfo.path= \'%s\',  "
				" nwSoftwareInfo.command= \'%s\',	nwSoftwareInfo.dmodel=%d, 	nwSoftwareInfo.rmodel=%d, 	nwSoftwareInfo.filename= \'%s\', "
				" nwSoftwareInfo.fileversion= \'%s\', 	nwSoftwareInfo.filedroptime=%u, 	nwSoftwareInfo.DAnyone=%d, 	nwSoftwareInfo.NAnyone=%d, "
				" nwSoftwareInfo.total=%d, 		nwSoftwareInfo.number=%d, 	nwSoftwareInfo.packageunique= \'%s\', nwSoftwareInfo.MD5=\'%s\', "
				" nwSoftwareDistribution.packageunique=\'%s\' , nwSoftwareInfo.state=%d,  nwSoftwareInfo.targetpath= \'%s\'"
				" where  nwSoftwareInfo.packageunique = nwSoftwareDistribution.packageunique "
				" and  nwSoftwareInfo.packageunique=\'%s\' ",  

				name, droptime  , droper, system, 
				language, size,  computer, path, 
				commandline, dmodel, rmodel, filename, 
				fileversion, filedroptime, DAnyone, NAnyone, 
				size, number , newunique, MD5,  
				newunique, 0, targetpath,  packageunique);
		}

		if(pasDbExecSql(sql, 0)){
			recodeLogInfo("  �޸��������", " �޸������ʧ��: " , sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -6);
			return -6;	
		}

		int num=0;
		for(num = 0; num < number; num++){
			nwCompIp * psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&(MyTaskRunList[num]));
			if(!psCompIp){
				snprintf(mesg, sizeof(mesg)-1, " compid:%u ������",    MyTaskRunList[num]);
				recodeLogInfo("  �޸��������", "ERROR: " , mesg);
			}else{
				snprintf(mesg, sizeof(mesg)-1, " compid:%u ����",    MyTaskRunList[num]);
				recodeLogInfo("  �޸��������", "Success : " , mesg);
				psCompIp->lCtrl |= 0x80000;
			}
		}

		if(1 == flag){
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    3);
			return 3;
		}else{
			//ɾ�������Դ�ļ�
			if(!remove(oldtargetpath)){
				recodeLogInfo("  �޸��������", " �޸������Դ�ļ�ʧ��: " , oldtargetpath);
			}

			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    4);
			return 4;
		}
	}

	//����������Ϣ�ɹ�
	pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
	return 0;
}

int  Lan_SoftwareConditionUp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int 	Count = 0;
	int	optype = 0;
	char	oldunique[128];
	char	myunique[128];
	int	type=0;
	int	Operator=0;
	char	filename[128];
	char	registryValue[128];
	char	content[128];
	char	sql[1024];

	memset(oldunique, 0, sizeof(oldunique));
	memset(myunique, 0, sizeof(myunique));
	memset(filename, 0, sizeof(filename));
	memset(registryValue, 0, sizeof(registryValue));
	memset(content, 0, sizeof(content));
	memset(sql, 0, sizeof(sql));

	int iReturn = utMsgGetSomeNVar(psMsgHead,3, 
					"Count",   	UT_TYPE_LONG, 4, &Count,
					"oldunique",	UT_TYPE_STRING, sizeof(oldunique)-1, oldunique,
					"unique",	UT_TYPE_STRING, sizeof(myunique)-1, myunique);
	if(Count <=0){
		recodeLogInfo("�ϴ����������Ϣ " ," Error���ϴ�����Ϊ0" , "");
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
						"state",   UT_TYPE_LONG,    -2,
						"number", UT_TYPE_LONG,  0);
		return -2;
	}

	snprintf(sql , sizeof(sql)-1, "delete from nwSoftwareConditions where packageunique=\'%s\' ",  oldunique);
	if(pasDbExecSql(sql, 0)){
		recodeLogInfo("  �޸������������", " �޸����������ʧ��: " , sql);			
	}else{
		recodeLogInfo("  �޸������������", " �޸�����������ɹ�: " , sql);
	}

	int number = 0;
	char   str[8][24];
	memset(str, 0, sizeof(char)*8*24);
	for(number = 0;  number < Count;  number++){
		snprintf(str[0],  sizeof(str[0])-1,  "optype%d",  number);
		snprintf(str[1],  sizeof(str[2])-1,  "type%d",  number);
		snprintf(str[2],  sizeof(str[3])-1,  "operator%d",  number);
		snprintf(str[3],  sizeof(str[4])-1,  "filename%d",  number);
		snprintf(str[4],  sizeof(str[5])-1,  "registryvalue%d",  number);
		snprintf(str[5],  sizeof(str[6])-1,  "content%d",  number);

		iReturn = utMsgGetSomeNVar(psMsgHead,7, 
						str[0],   	UT_TYPE_LONG, 4, &optype,
						str[1],   	UT_TYPE_LONG, 4, &type,
						str[2],   	UT_TYPE_LONG, 4, &Operator,
						str[3],   	UT_TYPE_STRING, sizeof(filename)-1, filename,
						str[4],   	UT_TYPE_STRING, sizeof(registryValue)-1, registryValue,
						str[5],   	UT_TYPE_STRING, sizeof(content)-1, content);


		snprintf(sql, sizeof(sql)-1, "insert into nwSoftwareConditions( optype,  packageunique,  type, operator, filename, registryvalue, content)  "
			" values(%d,  \'%s\',  %d,  %d,  \'%s\', \'%s\', \'%s\')"  ,
			optype,  myunique,  type,  Operator,  filename,  registryValue,  content);

		if(pasDbExecSql(sql, 0)){
			recodeLogInfo("����������nwSoftwareConditions���ݱ���ʧ�ܣ�", " " , sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					"state",   UT_TYPE_LONG,    -2,
					"number", UT_TYPE_LONG,  number);
			return -2;	
		}
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					"state",   UT_TYPE_LONG,    1,
					"number", UT_TYPE_LONG,  Count);
	return 0;
}


int  Lan_GetSoftwarePackageList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char	name[128];		//��������
	time_t	createTime=0;		//����ʱ��
	time_t	dropTime  =0;		//�޸�ʱ��
	char	droper[128];		//�޸���
	int	OS =0;			//����ϵͳ
	int	SystemLanguage=0;	//ϵͳ����

	int	size=0;			//�ļ���С
	char	computer[128];		//���ڼ����
	char	path[256];		//�ļ�Դ·��
	char	targetpath[256];		//�ļ�Ŀ��·��
	char	commandLine[128];	//������
	int	DModel=0;		//�ַ�ģʽ
	int	RModel=0;		//����ģʽ
	char	filename[128];		//�ļ���
	char	fileversion[128];	//�ļ��汾
	time_t	filedroptime=0;		//�ļ��޸�ʱ��

	int	DAnyone=0;		//������� ����һ������
	int	NAnyone=0;		//��Ҫ���� ����һ������

	int	total =0;		//���ݰ�����
	int	number=0;		//��ǰ���͵��ڼ���
	char	myunique[128];        	//����Ψһ��ʶ��
	char	companyid[128];		//�ϴ��ߵĹ�˾

	int	state =0;

	char	sql[1024];
	
	memset(name, 0, sizeof(name));
	memset(droper, 0, sizeof(droper));
	memset(computer, 0, sizeof(computer));
	memset(path, 0, sizeof(path));
	memset(targetpath, 0, sizeof(targetpath));
	memset(commandLine, 0, sizeof(commandLine));
	memset(filename, 0, sizeof(filename));
	memset(fileversion, 0, sizeof(fileversion));
	memset(myunique, 0, sizeof(myunique));
	memset(companyid, 0, sizeof(companyid));
	memset(sql, 0, sizeof(sql));
 
	//��ȡ����б�
	snprintf(sql, sizeof(sql)-1, "select name,createtime,droptime,droper, system, language, "
				       "  size,state, computer, path, targetpath, command, "
				       "  dmodel, rmodel, filename, fileversion, filedroptime, DAnyone, NAnyone, "
				      "  total,number, packageunique, companyid   from    nwSoftwareInfo order by sid desc");

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
        		recodeLogInfo("?�ȡ�??��? ��nwSoftwareInfo���ݱ��в�ѯ����ʧ��","" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        	}else{
		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
		utMsgAdd_l(&psMsgHead, "state",  1);
		
		int 	iReturn =0;
		int 	num=0;
		char 	str[24][24];
		memset(str, 0, sizeof(str));
		
		while(iReturn == 0){
			iReturn = pasDbFetchInto(psCur,  UT_TYPE_STRING, sizeof(name)-1, name,
							UT_TYPE_LONG, 4, &createTime,
							UT_TYPE_LONG, 4, &dropTime,
							UT_TYPE_STRING, sizeof(droper)-1, droper,
							UT_TYPE_LONG, 4, &OS,
							UT_TYPE_LONG, 4, &SystemLanguage,
							UT_TYPE_LONG, 4, &size,
							UT_TYPE_LONG, 4, &state,
							UT_TYPE_STRING, sizeof(computer)-1, computer,
							UT_TYPE_STRING, sizeof(path)-1, path,
							UT_TYPE_STRING, sizeof(targetpath)-1, targetpath,
							UT_TYPE_STRING, sizeof(commandLine)-1, commandLine,
							UT_TYPE_LONG, 4, &DModel,
							UT_TYPE_LONG, 4, &RModel,
							UT_TYPE_STRING, sizeof(filename)-1, filename,
							UT_TYPE_STRING, sizeof(fileversion)-1, fileversion,
							UT_TYPE_LONG, 4, &filedroptime,
							UT_TYPE_LONG, 4, &DAnyone,
							UT_TYPE_LONG, 4, &NAnyone,
							UT_TYPE_LONG, 4, &total,
							UT_TYPE_LONG, 4, &number,
							UT_TYPE_STRING, sizeof(myunique)-1, myunique,
							UT_TYPE_STRING, sizeof(companyid )-1, companyid 
							);

			if(0 != iReturn ){
				break;
			}
			
			snprintf(str[0], sizeof(str[0])-1, "name%d", num);
			snprintf(str[1], sizeof(str[1])-1, "createtime%d", num);
			snprintf(str[2], sizeof(str[2])-1, "droptime%d", num);
			snprintf(str[3], sizeof(str[3])-1, "droper%d", num);
			snprintf(str[4], sizeof(str[4])-1, "os%d", num);
			snprintf(str[5], sizeof(str[5])-1, "language%d", num);
			snprintf(str[6], sizeof(str[6])-1, "size%d", num);
			snprintf(str[7], sizeof(str[7])-1, "computer%d", num);
			snprintf(str[8], sizeof(str[8])-1, "path%d", num);
			snprintf(str[9], sizeof(str[9])-1, "targetpath%d", num);
			snprintf(str[10], sizeof(str[10])-1, "commandline%d", num);
			snprintf(str[11], sizeof(str[11])-1, "dmodel%d", num);
			snprintf(str[12], sizeof(str[12])-1, "rmodel%d", num);
			snprintf(str[13], sizeof(str[13])-1, "filename%d", num);
			snprintf(str[14], sizeof(str[14])-1, "fileversion%d", num);
			snprintf(str[15], sizeof(str[15])-1, "filedroptime%d", num);
			snprintf(str[16], sizeof(str[16])-1, "danyone%d", num);
			snprintf(str[17], sizeof(str[17])-1, "nanyone%d", num);
			snprintf(str[18], sizeof(str[18])-1, "total%d", num);
			snprintf(str[19], sizeof(str[19])-1, "number%d", num);
			snprintf(str[20], sizeof(str[20])-1, "unique%d", num);
			snprintf(str[21], sizeof(str[21])-1, "companyid%d", num);
			snprintf(str[22], sizeof(str[22])-1, "state%d", num);
			num++;

			utMsgAdd_s(&psMsgHead, str[0],  name);
			utMsgAdd_l(&psMsgHead, str[1],  createTime);
			utMsgAdd_l(&psMsgHead, str[2],  dropTime);
			utMsgAdd_s(&psMsgHead, str[3],  droper);
			utMsgAdd_l(&psMsgHead, str[4],  OS);
			utMsgAdd_l(&psMsgHead, str[5],  SystemLanguage);
			utMsgAdd_l(&psMsgHead, str[6],  size);
			utMsgAdd_s(&psMsgHead, str[7],  computer);
			utMsgAdd_s(&psMsgHead, str[8],  path);
			utMsgAdd_s(&psMsgHead, str[9],  targetpath);
			utMsgAdd_s(&psMsgHead, str[10],  commandLine);
			utMsgAdd_l(&psMsgHead, str[11],  DModel);
			utMsgAdd_l(&psMsgHead, str[12],  RModel);
			utMsgAdd_s(&psMsgHead, str[13],  filename);
			utMsgAdd_s(&psMsgHead, str[14],  fileversion);
			utMsgAdd_l(&psMsgHead, str[15],  filedroptime);
			utMsgAdd_l(&psMsgHead, str[16],  DAnyone);
			utMsgAdd_l(&psMsgHead, str[17],  NAnyone);
			utMsgAdd_l(&psMsgHead, str[18],  total);
			utMsgAdd_l(&psMsgHead, str[19],  number);
			utMsgAdd_s(&psMsgHead, str[20],  myunique);
			utMsgAdd_s(&psMsgHead, str[21],  companyid);
			utMsgAdd_l(&psMsgHead, str[22],  state);
		}
		
		utMsgAdd_l(&psMsgHead, "number",  num);
		pasDbCloseCursor(psCur);

		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}
	
	return 0;
}

int  Lan_GetSoftwareConditionList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int	optype = 0;
	char	unique[128];
	int	type =0 ;
	int	operator=0;
	char	filename[128];
	char 	registryvalue[128];
	char	content[128];
	char	sql[1024];

	memset(unique, 0, sizeof(unique));
	memset(filename, 0, sizeof(filename));
	memset(registryvalue, 0, sizeof(registryvalue));
	memset(content, 0, sizeof(content));
	memset(sql, 0, sizeof(sql));

	//��ȡ�����б�
	snprintf(sql, sizeof(sql)-1, "select optype,packageunique,type,operator,filename,registryvalue,content from  nwSoftwareConditions");

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
        		recodeLogInfo("��ȡ����б�  ��nwSoftwareInfo���ݱ��в�ѯ����ʧ�ܣ�" ,"" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        	}else{
		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
		utMsgAdd_l(&psMsgHead, "state",  1);
		
		int 	iReturn =0;
		int 	num=0;
		char 	str[8][24];
		memset(str, 0, sizeof(str));
		
		while(iReturn == 0){
			iReturn = pasDbFetchInto(psCur, 
							UT_TYPE_LONG, 4, &optype,
							UT_TYPE_STRING, sizeof(unique)-1, unique,
							UT_TYPE_LONG, 4, &type,
							UT_TYPE_LONG, 4, &operator,
							UT_TYPE_STRING, sizeof(filename)-1, filename,
							UT_TYPE_STRING, sizeof(registryvalue)-1, registryvalue,
							UT_TYPE_STRING, sizeof(content)-1, content);

			if(0 != iReturn ){
				break;
			}
			
			snprintf(str[0], sizeof(str[0])-1, "optype%d", num);
			snprintf(str[1], sizeof(str[1])-1, "unique%d", num);
			snprintf(str[2], sizeof(str[2])-1, "type%d", num);
			snprintf(str[3], sizeof(str[3])-1, "Operator%d", num);
			snprintf(str[4], sizeof(str[4])-1, "filename%d", num);
			snprintf(str[5], sizeof(str[5])-1, "registryValue%d", num);
			snprintf(str[6], sizeof(str[6])-1, "content%d", num);
			num++;

			utMsgAdd_l(&psMsgHead, str[0],  optype);
			utMsgAdd_s(&psMsgHead, str[1],  unique);
			utMsgAdd_l(&psMsgHead, str[2],  type);
			utMsgAdd_l(&psMsgHead, str[3],  operator);
			utMsgAdd_s(&psMsgHead, str[4],  filename);
			utMsgAdd_s(&psMsgHead, str[5],  registryvalue);
			utMsgAdd_s(&psMsgHead, str[6],  content);
		}
		
		utMsgAdd_l(&psMsgHead, "number",  num);
		pasDbCloseCursor(psCur);

		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}
	
	return 0;
}

int  Lan_DistributionTaskUp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char	compname[128];
	char	taskname[128];
	char	taskunique[128];
	char	taskunique2[128];
	char	packagename[128];
	char	packageunique[128];
	time_t	createtime = 0;
	time_t  droptime = 0;
	int		retry = 0;
	int		state = 0;
	int		rmodel= 0;
	int		rcycle= 0;
	int		date1 = 0;
	int		date2 = 0;
	int		date3 = 0;
	int		mytime= 0;
	int		total = 0;

	unsigned int 	compid = 0;
	int		type = 0;
	int		groupid=0;
	char		name[128];
	char		groupname[128];

	char		sql[1024];
	char		mesg[512];

	int		number = 0;
	int		MyTaskRunList[10240]={0};

	memset(compname,0 , sizeof(compname));
	memset(taskname,0 , sizeof(taskname));
	memset(taskunique,0 , sizeof(taskunique));
	memset(taskunique2,0 , sizeof(taskunique2));
	memset(packagename,0 , sizeof(packagename));
	memset(packageunique,0 , sizeof(packageunique));
	memset(name,0 , sizeof(name));
	memset(groupname,0 , sizeof(groupname));
	memset(sql,0 , sizeof(sql));
	memset(mesg,0 , sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,16, 
					"total",   	UT_TYPE_LONG, 4, &total,
					"taskname",	UT_TYPE_STRING, sizeof(taskname)-1, taskname,
					"taskunique",	UT_TYPE_STRING, sizeof(taskunique)-1, taskunique,
					"packagename",	UT_TYPE_STRING, sizeof(packagename)-1, packagename,
					"packageunique",	UT_TYPE_STRING, sizeof(packageunique)-1, packageunique,
					"createtime",   	UT_TYPE_LONG, 4, &createtime,
					"droptime",   	UT_TYPE_LONG, 4, &droptime,
					"retry",   	UT_TYPE_LONG, 4, &retry,
					"state",   	UT_TYPE_LONG, 4, &state,
					"rmodel",   	UT_TYPE_LONG, 4, &rmodel,
					"rcycle",   	UT_TYPE_LONG, 4, &rcycle,
					"date1",   	UT_TYPE_LONG, 4, &date1,
					"date2",   	UT_TYPE_LONG, 4, &date2,
					"date3",   	UT_TYPE_ULONG, 4, &date3,
					"mytime",   	UT_TYPE_ULONG, 4, &mytime,
					"compname",	UT_TYPE_STRING, sizeof(compname)-1, compname);

	//��ѯ��ǰtaskunique�Ƿ���ڣ�������ڣ����������
	snprintf(sql, sizeof(sql)-1, "select  taskname  from  nwSoftwareDistribution  where  taskunique=\'%s\'",   taskunique);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
        		recodeLogInfo("��ȡ����б�  ��nwSoftwareDistribution" ,"" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -2);
		return -2;
        	}else{
		char  	buffer[128];
		memset(buffer, 0, sizeof(buffer));
		iReturn = pasDbFetchInto(psCur, 	UT_TYPE_STRING, sizeof(buffer)-1, buffer);
		pasDbCloseCursor(psCur);

		//����������Ѵ��ڣ�����¸�����
		if(0 != strlen(buffer)){

			snprintf(taskunique2, sizeof(taskunique2)-1,  "%s%u",  packageunique,  droptime);
			snprintf(sql, sizeof(sql)-1, "update  nwSoftwareDistribution  set  taskname=\'%s\', taskunique=\'%s\', packagename=\'%s\', packageunique=\'%s\', "
					" droptime=%d,  maxretry=%d,  state=%d,   rmodel=%d,  rcycle=%d,  "
					" date1=%d, date2=%d, date3=%d, mytime=%d,  total=%d, preping=%d where  taskunique=\'%s\'  ", 
					taskname,  taskunique2, packagename, packageunique, 
					droptime, retry, state,rmodel, rcycle, 
					date1, date2, date3, mytime, total, total, taskunique);
	
			if(pasDbExecSql(sql, 0)){
				//recodeLogInfo("��������nwSoftwareDistribution���ݱ���ʧ��??, " " , sql);
				pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					"state",   UT_TYPE_LONG,    -3,
					"number", UT_TYPE_LONG,  0);
				return -3;
			}else{
				recodeLogInfo("��������nwSoftwareDistribution��", " �������ݳɹ�" , sql);
			}

			//��ѯ��ǰ�����·�����Щ�ͻ��ˣ���¼���ǵ�compid
			snprintf(sql, sizeof(sql)-1, "select  computerid from nwSoftwareDistributionSpeed  where  taskunique=\'%s\' ",  taskunique);
			pasDbCursor *psCur2 = pasDbOpenSql(sql,0);
			if(psCur2 == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
				recodeLogInfo(" ɾ������"," ��ѯ���ݿ�nwSoftwareDistributionSpeedʧ��!", sql);
       			}else{
				int 	iReturn =0;
				while(iReturn == 0){
					compid= 0;
					iReturn = pasDbFetchInto(psCur2, UT_TYPE_ULONG, 4, &compid );
					if(0 != iReturn){
						break;
					}

					snprintf(mesg, sizeof(mesg)-1, " compid:%u ",    compid);
					recodeLogInfo(" ɾ������", "��ѯ��compid : " , mesg);

					MyTaskRunList[number++] = compid;
				}
				pasDbCloseCursor(psCur2);
			}

			//ɾ���������·����ͻ����б�
			snprintf(sql, sizeof(sql)-1, "delete from nwSoftwareDistributionSpeed  where  taskunique=\'%s\' ; ",  taskunique);
			if(pasDbExecSql(sql, 0)){
				recodeLogInfo(" ɾ������", "ERROR: " , sql);	
			}else{
				recodeLogInfo(" ɾ������", "SUCCESS: " , sql);	
			}

			//֪ͨ��Ӧ�Ŀͻ��˸��������б�
			int num=0;
			for(num = 0; num < number; num++){
				nwCompIp * psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&(MyTaskRunList[num]));
				if(!psCompIp){
					snprintf(mesg, sizeof(mesg)-1, " compid:%u ������",    MyTaskRunList[num]);
					//recodeLogInfo(" ɾ��?���?, "ERROR: " , mesg);	
				}else{
					snprintf(mesg, sizeof(mesg)-1, " compid:%u ����",    MyTaskRunList[num]);
					recodeLogInfo(" ɾ������", "Success : " , mesg);
					psCompIp->lCtrl |= 0x80000;
				}
			}
		
			snprintf(taskunique, sizeof(taskunique)-1,  "%s",  taskunique2);

		}else{//������������
			snprintf(sql, sizeof(sql)-1, "insert into nwSoftwareDistribution(taskname, taskunique, packagename, packageunique, "
					" createtime,droptime,  maxretry,  state,  "
					" rmodel,  rcycle,  date1, date2, date3, mytime,  total, preping, companyid)  "
					" values(\'%s\', \'%s\', \'%s\', \'%s\', "
					"  %u, %u, %d, %d, "
					"  %d, %d, %d, %d, %d, %d, %d, %d, \'%s\')" , 
					taskname,  taskunique, packagename, packageunique, 
					createtime, droptime, retry, state,
					rmodel, rcycle, date1, date2, date3, mytime, total, total, compname);
			recodeLogInfo("��������nwSoftwareDistribution��", " ׼����������" , sql);
	
			if(pasDbExecSql(sql, 0)){
				recodeLogInfo("��������nwSoftwareDistribution���ݱ���ʧ�ܣ�", " " , sql);
				pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
					"state",   UT_TYPE_LONG,    -2,
					"number", UT_TYPE_LONG,  0);
				return -2;
			}else{
				recodeLogInfo("��������nwSoftwareDistribution��", " �������ݳɹ�" , sql);
			}
		}

		//�����µļ�����б�
		int number = 0;
		char  str[6][24];
		memset(str, 0, sizeof(str));
		for(number = 0;  number < total;  number++){
			snprintf(str[0], sizeof(str[0])-1,  "compid%d",  number);
			snprintf(str[1], sizeof(str[0])-1,  "type%d",  number);
			snprintf(str[2], sizeof(str[0])-1,  "groupid%d",  number);
			snprintf(str[3], sizeof(str[0])-1,  "name%d",  number);
			snprintf(str[4], sizeof(str[0])-1,  "groupname%d",  number);

			int iReturn = utMsgGetSomeNVar(psMsgHead,5, 
					str[0],   	UT_TYPE_LONG, 4, &compid,
					str[1],   	UT_TYPE_LONG, 4, &type,
					str[2],   	UT_TYPE_LONG, 4, &groupid,
					str[3],   	UT_TYPE_STRING, sizeof(name)-1, name,
					str[4],   	UT_TYPE_STRING, sizeof(groupname)-1, groupname);

			recodeLogInfo("��������nwSoftwareDistribution��", " ��ȡ�����б�" , "");

			//�����ǰcompid���� ����ID�������ǰcompid���� ����ID,����ȥ
			snprintf(sql, sizeof(sql)-1, "insert into nwSoftwareDistributionSpeed( "
					" taskunique, computerid, computer, groupid, groupname, maxretry) "
					" values(\'%s\', %u,  \'%s\', %d,  \'%s\', %d)"  ,
					taskunique, compid, name, groupid, groupname,retry);
			if(pasDbExecSql(sql, 0)){
				recodeLogInfo("�����������nwSoftwareDistributionSpeed���ݱ���ʧ�ܣ�", " " , sql);
				continue;
			}


			//�������ڴ���compid�� ����������� 0x80000
			nwCompIp * psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
			if(!psCompIp){
				snprintf(mesg, sizeof(mesg)-1, "����compid:%u������",  compid);
				recodeLogInfo("�·�����nwSoftwareDistributionSpeed���ݱ�"," ", mesg);	
			}else{
				psCompIp->lCtrl |= 0x80000;
			}
		}
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 2,
				"state",   UT_TYPE_LONG,    1,
				"number", UT_TYPE_LONG,  number);

	return 0;
}


int  Lan_RequestTaskList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	recodeLogInfo("���������б�", " start..." , "");
	
	unsigned int	compid = 0;
	char		taskunique[128];
	int		state = 0;
	int		total = 0;
	char		sql[1024];
	char		mesg[512];

	memset(taskunique, 0, sizeof(taskunique));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,1, "compid",   	UT_TYPE_LONG, 4, &compid);
	if(compid == 0){
		snprintf(mesg, sizeof(mesg)-1, "compid:%u", compid);
		recodeLogInfo("���������б�", " ��������" , mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -2);
		return -1;
	}

	//�������ڴ���compid�� ���������0x80000ȥ��
	int ret = RemoveHeartCommand(psShmHead, compid, 0x80000);
	if(ret < 0){
		snprintf(mesg, sizeof(mesg)-1, "�����compid:%u������",  compid);
		recodeLogInfo("���������б�"," ", mesg);
	}

	recodeLogInfo("���������б�", " ��ѯ���ݿ�..." , "");

	snprintf(sql, sizeof(sql)-1, "select taskunique from  nwSoftwareDistributionSpeed where computerid=%u",   compid);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		//recodeLogInfo("?��������б�?," ��ѯ���ݿ�ʧ��!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        	}else{
		recodeLogInfo("���������б�", " ��ѯ���ݿ��??.." , "  ��ȡ�����б�");
		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 0);
		utMsgAdd_l(&psMsgHead, "state",  1);
		
		int 	iReturn =0;
		int 	num=0;	
		while(iReturn == 0){
			memset(taskunique, 0, sizeof(taskunique));
			iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(taskunique)-1, taskunique);
							
			if(0 != iReturn ){
				break;
			}
			
			char str[24];
			memset(str, 0, sizeof(str));
			snprintf(str, sizeof(str)-1, "taskunique%d", num);
			utMsgAdd_s(&psMsgHead, str,  taskunique);
			num++;
		}
		
		utMsgAdd_l(&psMsgHead, "total",  num);
		pasDbCloseCursor(psCur);

		MypasMsgResponse(iFd,psMsgHead,NULL);

		recodeLogInfo("���������б�", " ������Ϣ�ɹ�..." , " ");
		return 0;
	}
	
	return 0;
}

int  Lan_RequestTaskConditions(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	recodeLogInfo("�����������", " start..." , "");
	
	unsigned int	compid = 0;
	char		taskunique[128];
	int		state = 0;
	int		total = 0;
	char		sql[1024];
	char		sql2[1024];
	char		mesg[512];

	char		packageunique[128];
	int		maxretry= 0;
	int		rmodel = 0;
	int		rcycle = 0;
	int		date1 = 0;
	int		date2 = 0;
	int		date3 = 0;
	int		mytime =0;

	int		system= 0;
	int		language =0 ;
	int		size = 0;
	char		commandline[128];
	int		dmodel = 0;
	int		rmodel2 = 0;
	char		filename[128];
	int		danyone = 0;
	int		nanyone = 0;

	int		optype =0 ;
	int		type = 0;
	int		operator=0;
	char		myfilename[128];
	char		registryvalue[128];
	char		content[128];

	int		number1 = 0;
	int		number2 = 0;
	int		maxlen = 2*1024*1024;
	char		*buffer1 = (char *)malloc(maxlen +1);
	char		*buffer2 = (char *)malloc(maxlen +1);

	if( (NULL == buffer1)  || (NULL == buffer2)){
		if(buffer1){
			free(buffer1);
		}

		if(buffer2){
			free(buffer2);
		}
		recodeLogInfo("�����������", " ��̬���ٿռ�ʧ��" , "");
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -1);
		return -1;
	}

	memset(taskunique, 0, sizeof(taskunique));
	memset(sql, 0, sizeof(sql));
	memset(sql2, 0, sizeof(sql2));
	memset(mesg, 0, sizeof(mesg));

	memset(packageunique, 0, sizeof(packageunique));
	memset(commandline, 0, sizeof(commandline));
	memset(filename, 0, sizeof(filename));
	memset(myfilename, 0, sizeof(myfilename));
	memset(registryvalue, 0, sizeof(registryvalue));
	memset(content, 0, sizeof(content));
	memset(buffer1, 0, maxlen+1);
	memset(buffer2, 0, maxlen+1);

	int iReturn = utMsgGetSomeNVar(psMsgHead,1, "compid",   	UT_TYPE_LONG, 4, &compid);
	if(compid == 0){
		snprintf(mesg, sizeof(mesg)-1, "compid:%u", compid);
		recodeLogInfo("�����������", " ��������" , mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -2);
		return -1;
	}

	//�������ڴ���compid�� ���������0x80000ȥ��
	int ret = RemoveHeartCommand(psShmHead, compid, 0x80000);
	if(ret < 0){
		snprintf(mesg, sizeof(mesg)-1, "�����compid:%u������",  compid);
		recodeLogInfo("�������������"," ", mesg);
	}

	recodeLogInfo("�����������", " ��ѯ���ݿ�nwSoftwareDistributionSpeed��nwSoftwareDistribution��nwSoftwareInfo  ..." , "");

	snprintf(sql, sizeof(sql)-1, "select nwSoftwareDistribution.taskunique, nwSoftwareDistribution.packageunique,nwSoftwareDistribution.maxretry,nwSoftwareDistribution.rmodel,rcycle,date1,date2,date3,mytime,"
		" system,language,size,command,dmodel,nwSoftwareInfo.rmodel,nwSoftwareInfo.filename,DAnyone,NAnyone "
		" from  nwSoftwareDistribution,nwSoftwareDistributionSpeed,nwSoftwareInfo "
		" where nwSoftwareDistribution.taskunique = nwSoftwareDistributionSpeed.taskunique  and nwSoftwareDistribution.packageunique=nwSoftwareInfo.packageunique     and  computerid=%u",   compid);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo("�������������"," ��?����ݿ�nwSoftwareDistributionSpeed��nwSoftwareDistribution��nwSoftwareInfo  ʧ��!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		recodeLogInfo("�������������", " ��ѯ���ݿ�ɹ�...  ��ȡ������� " , sql);		
		int 	iReturn =0;
		int 	num=0;	
		while(iReturn == 0){
			memset(taskunique, 0, sizeof(taskunique));
			memset(packageunique, 0, sizeof(packageunique));
			iReturn = pasDbFetchInto(psCur, 
								UT_TYPE_STRING, sizeof(taskunique)-1, taskunique,
								UT_TYPE_STRING, sizeof(packageunique)-1, packageunique,
								UT_TYPE_LONG, 4, &maxretry,
								UT_TYPE_LONG, 4, &rmodel ,
								UT_TYPE_LONG, 4, &rcycle ,
								UT_TYPE_LONG, 4, &date1 ,
								UT_TYPE_LONG, 4, &date2 ,
								UT_TYPE_ULONG, 4, &date3 ,
								UT_TYPE_ULONG, 4, &mytime,
								UT_TYPE_LONG, 4, &system,
								UT_TYPE_LONG, 4, &language ,
								UT_TYPE_LONG, 4, &size,
								UT_TYPE_STRING, sizeof(commandline)-1, commandline,
								UT_TYPE_LONG, 4, &dmodel ,
								UT_TYPE_LONG, 4, &rmodel2 ,
								UT_TYPE_STRING, sizeof(filename)-1, filename,
								UT_TYPE_LONG, 4, &danyone,
								UT_TYPE_LONG, 4, &nanyone  );
			if( (iReturn != 0) || 
			    (0==strcmp("", taskunique)) ||
			    (0==strcmp("", packageunique))){
				continue;
			}
				
			snprintf(buffer1+strlen(buffer1),  maxlen-strlen(buffer1)-1,  "[TASK%d]\ntaskunique=%s\npackageunique=%s\nmaxretry=%d\nrmodel=%d\nrcycle=%d\ndate1=%d\ndate2=%d\ndate3=%u\nmytime=%u\n"
				"system=%d\nlanguage=%d\nsize=%d\ncommandline=%s\ndmodel=%d\nrmodel2=%d\nfilename=%s\ndanyone=%d\nnanyone=%d\n\n", 
				number1++,  taskunique, packageunique,maxretry,rmodel,rcycle,date1,date2,date3,mytime,
				system, language,  size, commandline, dmodel , rmodel2 ,filename,danyone,nanyone );
		}
		pasDbCloseCursor(psCur);
	}

	recodeLogInfo("�����������", " ��ѯ���ݿ�nwSoftwareDistributionSpeed��nwSoftwareDistribution��nwSoftwareConditioins  ..." , "");

	snprintf(sql, sizeof(sql)-1, "select  nwSoftwareDistribution.packageunique,optype,type,operator,nwSoftwareConditions.filename,registryvalue,content "
		" from  nwSoftwareConditions, nwSoftwareDistribution,nwSoftwareDistributionSpeed "
		" where nwSoftwareDistribution.packageunique=nwSoftwareConditions.packageunique and nwSoftwareDistribution.taskunique = nwSoftwareDistributionSpeed.taskunique and  computerid=%u "
		" group by  nwSoftwareDistribution.packageunique,optype,type,operator,nwSoftwareConditions.filename,registryvalue,content " ,compid);

	pasDbCursor *psCur2 = pasDbOpenSql(sql,0);
	if(psCur2 == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo("�������������"," ��ѯ���ݿ�nwSoftwareDistributionSpeed��nwSoftwareDistribution��nwSoftwareConditions ʧ��!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		recodeLogInfo("�����������", " ��ѯ���ݿ�ɹ�...  ��ȡ������� " , sql);		
		int 	iReturn =0;
		int 	num=0;	
		while(iReturn == 0){
			memset(taskunique, 0, sizeof(taskunique));
			memset(packageunique, 0, sizeof(packageunique));
			iReturn = pasDbFetchInto(psCur2, 
							UT_TYPE_STRING, sizeof(packageunique)-1, packageunique,
							UT_TYPE_LONG, 4, &optype ,
							UT_TYPE_LONG, 4, &type,
							UT_TYPE_LONG, 4, &operator,
							UT_TYPE_STRING, sizeof(myfilename)-1, myfilename,
							UT_TYPE_STRING, sizeof(registryvalue)-1, registryvalue,
							UT_TYPE_STRING, sizeof(content)-1, content);
			if( (iReturn != 0) || (0==strcmp("", packageunique))){
				continue;
			}
				
			snprintf(buffer2+strlen(buffer2),  maxlen-strlen(buffer2)-1,  "[TASK%d]\npackageunique=%s\noptype=%d\ntype=%d\noperator=%d\nmyfilename=%s\nregistryvalue=%s\ncontent=%s\n\n" , 
				number2++,  packageunique,  optype, type, operator, myfilename, registryvalue, content);
		}
		pasDbCloseCursor(psCur2);
	}


	MypasMsgHeadInit(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 0);
	utMsgAdd_l(&psMsgHead, "state",  1);
	utMsgAdd_s(&psMsgHead, "buffer1",  buffer1);
	utMsgAdd_s(&psMsgHead, "buffer2",  buffer2);
	MypasMsgResponse(iFd,psMsgHead,NULL);



//	pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 3,"state",   UT_TYPE_LONG,    1,
//									"buffer1", UT_TYPE_STRING, buffer1,
//									"buffer2",  UT_TYPE_STRING, buffer2);
	recodeLogInfo("�����������", " ������Ϣ�ɹ�..." , " ");
	return 0;
}

int  Lan_RequestPackage(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	unsigned int	compid = 0;
	int 		state = 0;
	int		downloadsize = 0;
	int		size = 0;
	char		taskunique[128];
	char		path[512];
	char		sql[1024];
	char		mesg[512];
	char		*data = NULL;

	memset(taskunique, 0, sizeof(taskunique));
	memset(path, 0, sizeof(path));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,3, "compid",   	UT_TYPE_LONG, 4, &compid,
						   "taskunique", UT_TYPE_STRING, sizeof(taskunique)-1, taskunique,
						     "downloadsize",UT_TYPE_LONG, 4, &downloadsize);

	if( (3 != iReturn) ||
	    (0 == compid) ||
	    (0==strcmp("", taskunique))||
	    (downloadsize < 0)){
		TcpSend(iFd,  "1", 1);
		TcpRecv(iFd,   mesg,  1);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -2);
		return -1;
	}
	
	data = (char *)malloc(PACKAGE_SIZE+1);
	if(NULL == data){
		TcpSend(iFd,  "1", 1);
		TcpRecv(iFd,   mesg,  1);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -1;
	}

	snprintf(sql, sizeof(sql)-1, "select targetpath,size  from nwSoftwareInfo  where packageunique=\'%s\'  ",  taskunique);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		snprintf(mesg, sizeof(mesg)-1, "  ?�ѯ���ݿ�nwSoftwareInfo ʧ��!  compid:%u",  compid);
		recodeLogInfo("����������飺",mesg, sql);
		TcpSend(iFd,  "1", 1);
		TcpRecv(iFd,   mesg,  1);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -4);
		free(data);
		return -3;
        }else{
		snprintf(mesg, sizeof(mesg)-1, " ��ѯ���ݿ�ɹ�...��ȡ������洢·��  compid:%u",  compid);
		recodeLogInfo("����������飺",mesg, sql);		
		iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(path)-1, path,
						UT_TYPE_LONG, 4, &size);
		pasDbCloseCursor(psCur);

		if( (0 != iReturn)||
		    (0 == strcmp(path, ""))){
			snprintf(mesg, sizeof(mesg)-1, "  ?�ȡpath ʧ��!  compid:%u, iReturn:%d, path:%s",  compid, iReturn, path);
			recodeLogInfo("����������飺",mesg, sql);
			TcpSend(iFd,  "1", 1);
			TcpRecv(iFd,   mesg,  1);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -5);
			free(data);
			return -3;
		}

		FILE * fp = fopen(path, "rb");
		if(NULL == fp){
			snprintf(mesg, sizeof(mesg)-1, " ���ļ�ʧ��!  errno:%d, compid:%u, path:%s,",  errno, compid, path);
			recodeLogInfo("����������飺",mesg, "");
			TcpSend(iFd,  "1", 1);
			TcpRecv(iFd,   mesg,  1);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ?���ʱ���? */ 1,"state",   UT_TYPE_LONG,    -6);
			free(data);
			return -3;
		}

		fseek(fp, downloadsize,SEEK_SET);
		int len = fread(data, 1, PACKAGE_SIZE, fp);
		fclose(fp);

		if(size > len+downloadsize){
			state = 1;
		}else{
			state = 2;
		}

		char    MD5[64];
		memset(MD5, 0, sizeof(MD5));
		lan_GetMD5((unsigned char*)data, len , (unsigned char*)MD5);

		TcpSend(iFd,  data, len);
		TcpRecv(iFd,   mesg,  1);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 3,"state",   UT_TYPE_LONG,    state,
									  "len", UT_TYPE_LONG, len,
									  "MD5", UT_TYPE_STRING, MD5);

		snprintf(mesg, sizeof(mesg)-1, "����������ɹ��� compid:%u, state:%d, len:%d,path:%s",  compid,state, len, path);
		recodeLogInfo("",mesg, "");

		free(data);
		return 0;
	}

	return 0;
}

int  Lan_ReturnResult(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		taskunique[128];
	unsigned int	compid = 0;
	int		state = 0;
	int		retry = 0;

	int		total = 0;
	int		success = 0;
	int		fail = 0;
	int		mismatch = 0;
	int		preping = 0;

	char		sql[1024];

	memset(taskunique, 0, sizeof(taskunique));
	memset(sql, 0, sizeof(sql));

	int iReturn = utMsgGetSomeNVar(psMsgHead,4, "compid",   	UT_TYPE_LONG, 4, &compid,
						   "taskunique", UT_TYPE_STRING, sizeof(taskunique)-1, taskunique,
						     "state",UT_TYPE_LONG, 4, &state,
						     "retry",  UT_TYPE_LONG, 4, &retry);
	if( (4 != iReturn) ||
	    (0 == compid)||
	    (0== strlen(taskunique))){
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,-2);
		return -1;
	}

	snprintf(sql , sizeof(sql)-1, "Update nwSoftwareDistributionSpeed  set  state=%d,  retry=%d  where taskunique=\'%s\' and  computerid=%u ",
		state,  retry,  taskunique, compid);

	if(pasDbExecSql(sql, 0)){
		recodeLogInfo("�����ͻ�������·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�", "ERROR: " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,-3);
		return -1;	
	}

	//��ѯ��ǰ�����ִ��״��
	snprintf(sql , sizeof(sql)-1, "select count(*)  from  nwSoftwareDistributionSpeed  where taskunique=\'%s\' and  state=0", taskunique);
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo("�����ͻ�������·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�!"," ��ѯsql ERROR!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
			return 0;
        }else{		
		int iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &preping);
		pasDbCloseCursor(psCur);
		if(0 != iReturn){
			recodeLogInfo("�����ͻ�?�����·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�!"," ��ȡ��� ERROR!", sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ?�Կ����ʱ���? */ 1,"state",   UT_TYPE_LONG,    1);
			return 0;
		}
	}

	snprintf(sql , sizeof(sql)-1, "select count(*)  from  nwSoftwareDistributionSpeed  where taskunique=\'%s\' and  state=1", taskunique);
	psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo("�����ͻ�����?��·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�!"," ��ѯsql ERROR!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
		return 0;
        }else{		
		int iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &success);
		pasDbCloseCursor(psCur);
		if(0 != iReturn){
			recodeLogInfo("�����ͻ�?�����·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�!"," ��ȡ��� ERROR!", sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
			return 0;
		}
	}

	snprintf(sql , sizeof(sql)-1, "select count(*)  from  nwSoftwareDistributionSpeed  where taskunique=\'%s\' and  state=2", taskunique);
	psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo("�����ͻ�������·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�!"," ��ѯsql ERROR!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
		return 0;
        }else{		
		int iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &fail);
		pasDbCloseCursor(psCur);
		if(0 != iReturn){
			recodeLogInfo("�����ͻ�������·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�!"," ��ȡ��� ERROR!", sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
			return 0;	
		}
	}

	snprintf(sql , sizeof(sql)-1, "select count(*)  from  nwSoftwareDistributionSpeed  where taskunique=\'%s\' and  state=3", taskunique);
	psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo("�����ͻ�������·�ִ��״?���nwSoftwareDistributionSpeed���ݱ�!"," ��ѯsql ERROR!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
		return 0;
        }else{		
		int iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &mismatch);
		pasDbCloseCursor(psCur);
		if(0 != iReturn){
			recodeLogInfo("�����ͻ�������·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�!"," ��ȡ��� ERROR!", sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
			return 0;
		}
	}

	snprintf(sql , sizeof(sql)-1, "select total  from nwSoftwareDistribution  where taskunique=\'%s\'", taskunique);
	psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo("�����ͻ�������·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�!"," ��ѯsql ERROR!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ?�Կ����ʱ���? */ 1,"state",   UT_TYPE_LONG,    1);
		return 0;
        }else{		
		int iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &total);
		pasDbCloseCursor(psCur);
		if(0 != iReturn){
			recodeLogInfo("�����ͻ�������·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�!"," ��ȡ��� ERROR!", sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
			return 0;
		}
	}


	if(success >= total){
		snprintf(sql , sizeof(sql)-1, "update  nwSoftwareDistribution set  state = 2,  success=%d,  fail=%d, mismatch=%d, preping=%d  where taskunique=\'%s\'", 
			success,  fail,  mismatch,  preping,  taskunique);

	}else{
		snprintf(sql , sizeof(sql)-1, "update  nwSoftwareDistribution set  success=%d,  fail=%d, mismatch=%d, preping=%d  where taskunique=\'%s\'", 
			success,  fail,  mismatch,  preping,  taskunique);

	}

	if(pasDbExecSql(sql, 0)){
		recodeLogInfo("�����ͻ�������·�ִ��״̬��nwSoftwareDistributionSpeed���ݱ�", "ERROR: " , sql);
	}


	pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,1);
	return 0;
}

int  Lan_GetPackageList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char	name[128];		//���������
	char	packageunique[128];     //�����Ψһ��ʶ��
	time_t	createTime=0;		//����ʱ��
	time_t	dropTime  =0;		//�޸�ʱ��
	char	droper[128];		//�޸���
	char	filename[128];		//�ļ���
	int	size=0;			//�ļ���С
	int	state =0;

	char	compname[128];
	char	sql[1024];
	
	memset(name, 0, sizeof(name));
	memset(packageunique, 0, sizeof(packageunique));
	memset(droper, 0, sizeof(droper));
	memset(filename, 0, sizeof(filename));
	memset(compname, 0, sizeof(compname));
	memset(sql, 0, sizeof(sql));
 
	int iReturn = utMsgGetSomeNVar(psMsgHead,4, "compname", UT_TYPE_STRING, sizeof(compname)-1, compname);
	if(0==strlen(compname)){
		recodeLogInfo("��ȡ������б� ", " ���յĹ�˾����Ϊ��","");
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"mystate",   UT_TYPE_LONG,    -2);
		return -2;
	}

	//��ȡ����б�
	snprintf(sql, sizeof(sql)-1, "select name,packageunique, createtime,droptime,droper, filename, size, state  "
		" from nwSoftwareInfo where companyid=\'%s\' order by sid desc",  compname);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
        		recodeLogInfo(" ��nwSoftwareInfo���ݱ��в�ѯ����ʧ��","" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"mystate",   UT_TYPE_LONG,    -3);
		return -3;
        	}else{
		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"mystate",   UT_TYPE_LONG,    1);
		utMsgAdd_l(&psMsgHead, "mystate",  1);
		
		int 	iReturn =0;
		int 	num=0;
		char 	str[8][24];
		memset(str, 0, sizeof(str));
		
		while(iReturn == 0){
			iReturn = pasDbFetchInto(psCur,  UT_TYPE_STRING, sizeof(name)-1, name,
							UT_TYPE_STRING, sizeof(packageunique)-1, packageunique,
							UT_TYPE_LONG, 4, &createTime,
							UT_TYPE_LONG, 4, &dropTime,
							UT_TYPE_STRING, sizeof(droper)-1, droper,
							UT_TYPE_STRING, sizeof(filename)-1, filename,
							UT_TYPE_LONG, 4, &size,
							UT_TYPE_LONG, 4, &state);

			if(0 != iReturn ){
				break;
			}
			
			snprintf(str[0], sizeof(str[0])-1, "packagename%d", num);
			snprintf(str[1], sizeof(str[1])-1, "packageunique%d", num);
			snprintf(str[2], sizeof(str[2])-1, "filename%d", num);
			snprintf(str[3], sizeof(str[3])-1, "createtime%d", num);
			snprintf(str[4], sizeof(str[4])-1, "droptime%d", num);
			snprintf(str[5], sizeof(str[5])-1, "droper%d", num);
			snprintf(str[6], sizeof(str[6])-1, "size%d", num);
			snprintf(str[7], sizeof(str[7])-1, "state%d", num);
			num++;

			utMsgAdd_s(&psMsgHead, str[0],  name);
			utMsgAdd_s(&psMsgHead, str[1],  packageunique);
			utMsgAdd_s(&psMsgHead, str[2],  filename);
			utMsgAdd_l(&psMsgHead, str[3],  createTime);
			utMsgAdd_l(&psMsgHead, str[4],  dropTime);
			utMsgAdd_s(&psMsgHead, str[5],  droper);
			utMsgAdd_l(&psMsgHead, str[6],  size);
			utMsgAdd_l(&psMsgHead, str[7],  state);
		}
		
		utMsgAdd_l(&psMsgHead, "number",  num);
		pasDbCloseCursor(psCur);

		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}
	
	return 0;
}
int  Lan_GetPackageDetailInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char	packageunique[128];
	char	name[128];		//��������
	time_t	createTime=0;		//����ʱ��
	time_t	dropTime  =0;		//�޸�ʱ��
	char	droper[128];		//�޸���
	int	OS =0;			//����ϵͳ
	int	SystemLanguage=0;	//ϵͳ����

	int	size=0;			//�ļ���С
	int	state =0;
	char	computer[128];		//���ڼ����
	char	path[256];		//�ļ�Դ·��
	char	targetpath[256];		//�ļ�Ŀ��·��
	char	commandLine[128];	//������
	int	DModel=0;		//�ַ�ģʽ
	int	RModel=0;		//����ģʽ
	char	filename[128];		//�ļ���
	char	fileversion[128];	//�ļ��汾
	time_t	filedroptime=0;		//�ļ��޸�ʱ��

	int	DAnyone=0;		//������� ����һ������
	int	NAnyone=0;		//��Ҫ���� ����һ������

	int	total =0;		//���ݰ�����
	int	number=0;		//��ǰ���͵��ڼ���
	char	myunique[128];        	//����Ψһ��ʶ��
	char	companyid[128];		//�ϴ��ߵĹ�˾

	char	sql[1024];
	char	mesg[512];
	
	memset(packageunique, 0, sizeof(packageunique));
	memset(name, 0, sizeof(name));
	memset(droper, 0, sizeof(droper));
	memset(computer, 0, sizeof(computer));
	memset(path, 0, sizeof(path));
	memset(targetpath, 0, sizeof(targetpath));
	memset(commandLine, 0, sizeof(commandLine));
	memset(filename, 0, sizeof(filename));
	memset(fileversion, 0, sizeof(fileversion));
	memset(myunique, 0, sizeof(myunique));
	memset(companyid, 0, sizeof(companyid));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));


	int iReturn = utMsgGetSomeNVar(psMsgHead,1,  "packageunique", UT_TYPE_STRING, sizeof(packageunique)-1, packageunique);
	if( (1 != iReturn)||
	    (0==strlen(packageunique))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  packageunique:%s",  iReturn,  packageunique);
		recodeLogInfo(" ��ȡ�������ϸ��Ϣ","  ���ղ���ʧ��!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"mystate",   UT_TYPE_LONG,-2);
		return -2;
	}
 
	//��ȡ�������ϸ��Ϣ
	snprintf(sql, sizeof(sql)-1, "select name,createtime,droptime,droper, system, language, "
				       "  size,state, computer, path, targetpath, command, "
				       "  dmodel, rmodel, filename, fileversion, filedroptime, DAnyone, NAnyone, "
				      "  total,number, packageunique, companyid   from    nwSoftwareInfo where  packageunique=\'%s\'",  packageunique);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
        		recodeLogInfo(" ��nwSoftwareInfo���ݱ��в�ѯ����ʧ��","" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"mystate",   UT_TYPE_LONG,    -3);
		return -3;
        	}else{			
		iReturn = pasDbFetchInto(psCur,  UT_TYPE_STRING, sizeof(name)-1, name,
							UT_TYPE_LONG, 4, &createTime,
							UT_TYPE_LONG, 4, &dropTime,
							UT_TYPE_STRING, sizeof(droper)-1, droper,
							UT_TYPE_LONG, 4, &OS,
							UT_TYPE_LONG, 4, &SystemLanguage,
							UT_TYPE_LONG, 4, &size,
							UT_TYPE_LONG, 4, &state,
							UT_TYPE_STRING, sizeof(computer)-1, computer,
							UT_TYPE_STRING, sizeof(path)-1, path,
							UT_TYPE_STRING, sizeof(targetpath)-1, targetpath,
							UT_TYPE_STRING, sizeof(commandLine)-1, commandLine,
							UT_TYPE_LONG, 4, &DModel,
							UT_TYPE_LONG, 4, &RModel,
							UT_TYPE_STRING, sizeof(filename)-1, filename,
							UT_TYPE_STRING, sizeof(fileversion)-1, fileversion,
							UT_TYPE_LONG, 4, &filedroptime,
							UT_TYPE_LONG, 4, &DAnyone,
							UT_TYPE_LONG, 4, &NAnyone,
							UT_TYPE_LONG, 4, &total,
							UT_TYPE_LONG, 4, &number,
							UT_TYPE_STRING, sizeof(myunique)-1, myunique,
							UT_TYPE_STRING, sizeof(companyid )-1, companyid);
		pasDbCloseCursor(psCur);

		if(0 == strlen(name) ){
			recodeLogInfo(" ��nwSoftwareInfo���ݱ�����ȡ����ʧ��","" , sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"mystate",   UT_TYPE_LONG,    -4);
			return -4;
		}else{
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 24,"mystate",   UT_TYPE_LONG,1,
										"name",		UT_TYPE_STRING,	name,
										"createtime",	UT_TYPE_LONG,	createTime,
										"droptime",	UT_TYPE_LONG,	dropTime,
										"droper",	UT_TYPE_STRING, droper,
										"system",	UT_TYPE_LONG,	OS,
										"language",	UT_TYPE_LONG,	SystemLanguage,
										"size",		UT_TYPE_LONG,	size,
										"state",		UT_TYPE_LONG,	state,
										"computer",	UT_TYPE_STRING,	computer,
										"path",		UT_TYPE_STRING, path,
										"targetpath",	UT_TYPE_STRING, targetpath,
										"commandline",	UT_TYPE_STRING, commandLine,
										"dmodel",	UT_TYPE_LONG, 	DModel,
										"rmodel",	UT_TYPE_LONG,	RModel,
										"filename",	UT_TYPE_STRING, filename,
										"fileversion",	UT_TYPE_STRING, fileversion,
										"filedroptime", UT_TYPE_LONG, 	filedroptime,
										"danyone",	UT_TYPE_LONG, 	DAnyone,
										"nanyone",	UT_TYPE_LONG, 	NAnyone,
										"total",		UT_TYPE_LONG, 	total,
										"number",	UT_TYPE_LONG, 	number,
										"unique",	UT_TYPE_STRING, 	myunique,
										"companyid",	UT_TYPE_STRING, companyid);
		}
			
	
		
	}
	
	return 0;
}

int  Lan_GetPackageConditionList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char	packageunique[128];
	int	optype = 0;
	char	unique[128];
	int	type =0 ;
	int	operator=0;
	char	filename[128];
	char 	registryvalue[128];
	char	content[128];
	char	sql[1024];
	char	mesg[512];

	memset(packageunique, 0, sizeof(packageunique));
	memset(unique, 0, sizeof(unique));
	memset(filename, 0, sizeof(filename));
	memset(registryvalue, 0, sizeof(registryvalue));
	memset(content, 0, sizeof(content));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,1,  "packageunique", UT_TYPE_STRING, sizeof(packageunique)-1, packageunique);
	if( (1 != iReturn)||
	    (0==strlen(packageunique))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  packageunique:%s",  iReturn,  packageunique);
		recodeLogInfo(" ��ȡ����������б�","  ���ղ���ʧ��!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}	

	//��ȡ�����б�
	snprintf(sql, sizeof(sql)-1, "select optype,packageunique,type,operator,filename,registryvalue,content from  nwSoftwareConditions  where packageunique=\'%s\'",  packageunique);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
        		recodeLogInfo("��ȡ����б�  ��nwSoftwareInfo���ݱ��в�ѯ����ʧ�ܣ�" ,"" , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        	}else{
		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    1);
		utMsgAdd_l(&psMsgHead, "state",  1);
		
		int 	iReturn =0;
		int 	num=0;
		char 	str[8][24];
		memset(str, 0, sizeof(str));
		
		while(iReturn == 0){
			iReturn = pasDbFetchInto(psCur, 
							UT_TYPE_LONG, 4, &optype,
							UT_TYPE_STRING, sizeof(unique)-1, unique,
							UT_TYPE_LONG, 4, &type,
							UT_TYPE_LONG, 4, &operator,
							UT_TYPE_STRING, sizeof(filename)-1, filename,
							UT_TYPE_STRING, sizeof(registryvalue)-1, registryvalue,
							UT_TYPE_STRING, sizeof(content)-1, content);

			if(0 != iReturn ){
				break;
			}
			
			snprintf(str[0], sizeof(str[0])-1, "optype%d", num);
			snprintf(str[1], sizeof(str[1])-1, "unique%d", num);
			snprintf(str[2], sizeof(str[2])-1, "type%d", num);
			snprintf(str[3], sizeof(str[3])-1, "Operator%d", num);
			snprintf(str[4], sizeof(str[4])-1, "filename%d", num);
			snprintf(str[5], sizeof(str[5])-1, "registryValue%d", num);
			snprintf(str[6], sizeof(str[6])-1, "content%d", num);
			num++;

			utMsgAdd_l(&psMsgHead, str[0],  optype);
			utMsgAdd_s(&psMsgHead, str[1],  unique);
			utMsgAdd_l(&psMsgHead, str[2],  type);
			utMsgAdd_l(&psMsgHead, str[3],  operator);
			utMsgAdd_s(&psMsgHead, str[4],  filename);
			utMsgAdd_s(&psMsgHead, str[5],  registryvalue);
			utMsgAdd_s(&psMsgHead, str[6],  content);
		}
		
		utMsgAdd_l(&psMsgHead, "number",  num);
		pasDbCloseCursor(psCur);

		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}
	
	return 0;
}

int  Lan_RemovePackage(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		taskunique[128];
	char		targetpath[512];
	char		packageunique[128];
	int		deletetask=0;
	unsigned int	compid =0;
	char		sql[1024];
	char		mesg[256];
	unsigned int	MyTaskRunList[10240] = {0};
	int		number = 0;

	memset(taskunique, 0, sizeof(taskunique));
	memset(targetpath, 0, sizeof(targetpath));
	memset(packageunique, 0, sizeof(packageunique));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,2,  "packageunique", UT_TYPE_STRING, sizeof(packageunique)-1, packageunique,
						    "deletetask",  UT_TYPE_LONG, 4,  &deletetask);
	if( (2 != iReturn)||
	    (0==strlen(packageunique))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  packageunique:%s",  iReturn,  packageunique);
		recodeLogInfo(" ɾ�������","  ���ղ���ʧ��!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	//�����������ѱ��·����ͻ��ˣ���ɾ��ʧ��
	if(0 == deletetask){
		snprintf(sql, sizeof(sql)-1, "select taskunique from nwSoftwareDistribution  where  packageunique=\'%s\' ",  packageunique);
		pasDbCursor *psCur = pasDbOpenSql(sql,0);
		if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
			recodeLogInfo(" ɾ�������"," ��ѯ���ݿ�nwSoftwareDistributionʧ��!", sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
			return -3;
        		}else{
			int 	iReturn =0;
			int 	flag=0;

			while(iReturn == 0){
				memset(taskunique, 0, sizeof(taskunique));
				iReturn = pasDbFetchInto(psCur, 	UT_TYPE_STRING, sizeof(taskunique)-1, taskunique);
				if(0 != strlen(taskunique)){				
					flag = 1;
					break;			
				}
			}
			pasDbCloseCursor(psCur);

			if(1 == flag){
				recodeLogInfo(" ɾ�������"," ����ɾ����Ӧ�������б�!", "");
				pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    3);
				return 3;
			}else{
				snprintf(sql, sizeof(sql)-1, "delete from nwSoftwareInfo  where  packageunique=\'%s\' ",  packageunique);
				if(pasDbExecSql(sql, 0)){
					recodeLogInfo("  ɾ���������", "ERROR: " , sql);
					pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,-4);
					return -4;	
				}else{
					recodeLogInfo("  ɾ���������", "SUCCESS: " , sql);
					pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,1);
					return 0;
				}
			}
		}
	}

	//���Ҫǿ��ɾ�� �������deletetask == 1��

	//��ѯ��ǰ������·�����Щ��������ȡ��Щ������ID
	snprintf(sql, sizeof(sql)-1, "select computerid from nwSoftwareDistribution, nwSoftwareDistributionSpeed  "
		" where  nwSoftwareDistribution.taskunique= nwSoftwareDistributionSpeed.taskunique  and  packageunique=\'%s\' ",  packageunique);
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo(" ɾ���������"," ��ѯcomputerid ʧ��!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -5);
		return -5;
        	}else{
		int 	iReturn =0;
		while(iReturn == 0){
			compid= 0;
			iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &compid );
			if(0 != iReturn){
				break;
			}

			snprintf(mesg, sizeof(mesg)-1, " compid:%u ",    compid);
			recodeLogInfo("  ɾ���������", "��ѯ��compid : " , mesg);

			MyTaskRunList[number++] = compid;
		}
		pasDbCloseCursor(psCur);
	}

	//��ѯ��ǰ���?��Ĵ洢·�?
	snprintf(sql, sizeof(sql)-1, "select targetpath  from nwSoftwareInfo where  packageunique=\'%s\' ",  packageunique);
	pasDbCursor *psCur2 = pasDbOpenSql(sql,0);
	if(psCur2 == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo(" ɾ���������"," ��ѯcomputerid ʧ��!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -5);
		return -5;
        	}else{
		iReturn = pasDbFetchInto(psCur2, UT_TYPE_STRING, sizeof(targetpath)-1, targetpath);
		pasDbCloseCursor(psCur2);		
	}

	//ɾ���������Ϣ����Ӧ�������б�
	snprintf(sql, sizeof(sql)-1, "delete nwSoftwareInfo, nwSoftwareDistribution, nwSoftwareDistributionSpeed  "
		" from nwSoftwareInfo, nwSoftwareDistribution, nwSoftwareDistributionSpeed  "
		" where  nwSoftwareInfo.packageunique = nwSoftwareDistribution.packageunique  "
		"  and  nwSoftwareDistribution.taskunique= nwSoftwareDistributionSpeed.taskunique  "
		" and  nwSoftwareInfo.packageunique=\'%s\' ",  packageunique);
	if(pasDbExecSql(sql, 0)){
		recodeLogInfo("  ɾ���������", " ɾ�������ʧ��: " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -6);
		return -6;	
	}

	//ɾ������
	snprintf(sql, sizeof(sql)-1, "delete from  nwSoftwareConditions  where packageunique=\'%s\' ",  packageunique);
	if(pasDbExecSql(sql, 0)){
		recodeLogInfo("  ɾ���������", " ɾ�����������ʧ��: " , sql);
	}

	//ɾ�������Դ�ļ�
	if(!remove(targetpath)){
		recodeLogInfo("  ɾ���������", " ɾ�������Դ�ļ�ʧ��: " , targetpath);
	}

	int num=0;
	for(num = 0; num < number; num++){
		nwCompIp * psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&(MyTaskRunList[num]));
		if(!psCompIp){
			snprintf(mesg, sizeof(mesg)-1, " compid:%u ������",    MyTaskRunList[num]);
			recodeLogInfo("  ɾ���������", "ERROR: " , mesg);
		}else{
			snprintf(mesg, sizeof(mesg)-1, " compid:%u ����",    MyTaskRunList[num]);
			recodeLogInfo("  ɾ���������", "Success : " , mesg);
			psCompIp->lCtrl |= 0x80000;
		}
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,  1);
	return 0;
}


int  Lan_GetTaskList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		taskunique[128];
	char		taskname[128];
	char		packageunique[128];
	char		packagename[128];
	char		compname[128];
	int		state = 0;
	int		success = 0;
	char		sql[1024];

	memset(taskunique, 0, sizeof(taskunique));
	memset(taskname, 0, sizeof(taskname));
	memset(packageunique, 0, sizeof(packageunique));
	memset(packagename, 0, sizeof(packagename));
	memset(compname, 0, sizeof(compname));
	memset(sql, 0, sizeof(sql));

	int iReturn = utMsgGetSomeNVar(psMsgHead,1,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname);
	if(0 == strlen(compname)){
		recodeLogInfo("��ȡ�����б� ", " ���յĹ�˾����Ϊ��","");
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -2);
		return -2;
	}

	snprintf(sql, sizeof(sql)-1, "select  taskunique, taskname,  packageunique, packagename,  state, success  from nwSoftwareDistribution where  companyid=\'%s\'", compname);
	
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo(" ���������б�"," ��ѯ���ݿ�nwSoftwareDistributionʧ��!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		int 	iReturn =0;
		int 	num=0;	
		char	str[8][24];
		memset(str, 0, sizeof(str));

		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 0);

		while(iReturn == 0){
			memset(taskunique, 0, sizeof(taskunique));
			memset(packageunique, 0, sizeof(packageunique));
			iReturn = pasDbFetchInto(psCur, 
							UT_TYPE_STRING, sizeof(taskunique)-1, taskunique,
							UT_TYPE_STRING, sizeof(taskname)-1, taskname,
							UT_TYPE_STRING, sizeof(packageunique)-1, packageunique,
							UT_TYPE_STRING, sizeof(packagename)-1, packagename,
							UT_TYPE_LONG, 4, &state,
							UT_TYPE_LONG, 4, &success );
			if( (iReturn != 0) || (0==strcmp("", taskunique))){
				continue;
			}
			

			snprintf(str[0], sizeof(str[0])-1, "taskunique%d", num);
			snprintf(str[1], sizeof(str[1])-1, "taskname%d", num);
			snprintf(str[2], sizeof(str[2])-1, "packageunique%d", num);
			snprintf(str[3], sizeof(str[3])-1, "packagename%d", num);
			snprintf(str[4], sizeof(str[4])-1, "state%d", num);
			snprintf(str[5], sizeof(str[5])-1, "success%d", num);
			num++;


			utMsgAdd_s(&psMsgHead, str[0],  taskunique);
			utMsgAdd_s(&psMsgHead, str[1],  taskname);
			utMsgAdd_s(&psMsgHead, str[2],  packageunique);
			utMsgAdd_s(&psMsgHead, str[3],  packagename);
			utMsgAdd_l(&psMsgHead, str[4],  state);
			utMsgAdd_l(&psMsgHead, str[5],  success);
		}
		pasDbCloseCursor(psCur);
		utMsgAdd_l(&psMsgHead, "state",  1);
		utMsgAdd_l(&psMsgHead, "number",  num);
		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}

	return 0;
}

int  Lan_GetTaskDetailInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		taskname[128];
	char		taskunique[128];
	char		packagename[128];
	char		packageunique[128];
	time_t		createtime = 0;
	time_t  		droptime = 0;
	int		retry = 0;
	int		state = 0;
	int		rmodel= 0;
	int		rcycle= 0;
	int		date1 = 0;
	int		date2 = 0;
	int		date3 = 0;
	int		mytime= 0;

	int		total = 0;
	int		success=0;
	int		fail = 0;
	int		mismatch=0;
	int		preping = 0;
	char		companyid[128];		//����ϴ���������˾

	char		sql[1024];


	memset(taskunique, 0, sizeof(taskunique));
	memset(taskname, 0, sizeof(taskname));
	memset(packageunique, 0, sizeof(packageunique));
	memset(packagename, 0, sizeof(packagename));
	memset(companyid, 0, sizeof(companyid));
	memset(sql, 0, sizeof(sql));

	int iReturn = utMsgGetSomeNVar(psMsgHead,1, "taskunique", UT_TYPE_STRING, sizeof(taskunique)-1, taskunique);
	if( 0== strlen(taskunique)){
		recodeLogInfo(" ��ȡ������ϸ��Ϣ��", "ERROR:  taskuniqueΪ�գ��޷���ѯ������Ϣ" , "");
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ��?? */ 1,"state",   UT_TYPE_LONG,-3);
		return -1;
	}


	snprintf(sql, sizeof(sql)-1, "select  taskname,taskunique,  packagename,  packageunique,  createtime, droptime, "
		" maxretry, state, rmodel,  rcycle,  date1, date2, date3, mytime, "
		"  total, success,  fail,  mismatch,  preping, companyid  from nwSoftwareDistribution   where  taskunique=\'%s\'",  taskunique);
	
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo(" ��ȡ������ϸ��Ϣ��"," ��ѯ���ݿ�nwSoftwareDistributionʧ��!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -4);
		return -4;
        }else{
		iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(taskname)-1, taskname,
						UT_TYPE_STRING, sizeof(taskunique)-1, taskunique,
						UT_TYPE_STRING, sizeof(packagename)-1, packagename,
						UT_TYPE_STRING, sizeof(packageunique)-1, packageunique,
						UT_TYPE_LONG, 4, &createtime,
						UT_TYPE_LONG, 4, &droptime,
						UT_TYPE_LONG, 4, &retry,
						UT_TYPE_LONG, 4, &state,
						UT_TYPE_LONG, 4, &rmodel,
						UT_TYPE_LONG, 4, &rcycle,
						UT_TYPE_LONG, 4, &date1,
						UT_TYPE_LONG, 4, &date2,
						UT_TYPE_LONG, 4, &date3,
						UT_TYPE_LONG, 4, &mytime,
						UT_TYPE_LONG, 4, &total,
						UT_TYPE_LONG, 4, &success,
						UT_TYPE_LONG, 4, &fail,
						UT_TYPE_LONG, 4, &mismatch,
						UT_TYPE_LONG, 4, &preping,
						UT_TYPE_STRING,  sizeof(companyid )-1, companyid );
		pasDbCloseCursor(psCur);

//		if( (iReturn != 0) ){
//			recodeLogInfo(" ��ȡ������ϸ��Ϣ��"," ��ȡ����ʧ��!", sql);
//			pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ��?? */ 1,"state",   UT_TYPE_LONG,    -5);
//			return -5;
//		}

		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 21, 
									"mystate", 	UT_TYPE_LONG,   1,
									"taskname", 	UT_TYPE_STRING, taskname,
									"taskunique", 	UT_TYPE_STRING,	taskunique,
									"packagename", 	UT_TYPE_STRING, 	packagename,
									"packageunique",	UT_TYPE_STRING, packageunique,
									"createtime", 	UT_TYPE_LONG, 	createtime,
									"droptime", 	UT_TYPE_LONG, 	droptime,
									"retry", 	UT_TYPE_LONG,	retry,
									"state", 	UT_TYPE_LONG, 	state,
									"rmodel", 	UT_TYPE_LONG,	rmodel,
									"rcycle", 	UT_TYPE_LONG, 	rcycle,
									"date1", 	UT_TYPE_LONG, 	date1,
									"date2", 	UT_TYPE_LONG, 	date2,
									"date3", 	UT_TYPE_LONG, 	date3,
									"mytime", 	UT_TYPE_LONG, 	mytime,
									"total",	 	UT_TYPE_LONG, 	total,
									"success", 	UT_TYPE_LONG, 	success,
									"fail", 		UT_TYPE_LONG,	fail,
									"mismatch", 	UT_TYPE_LONG, 	mismatch,
									"preping", 	UT_TYPE_LONG, 	preping,
									"companyid", 	UT_TYPE_STRING,	companyid);
		
	}

	return 0;
}

int  Lan_GetTaskRunList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		taskunique[128];
	unsigned int 	compid = 0;
	char		computer[128];
	int		groupid = 0;
	char		groupname[128];
	int		state = 0;
	int		retry = 0;
	int		lastruntime=0;
	int		sip = 0;
	
	char		sql[1024];
	char		mesg[256];

	memset(taskunique, 0, sizeof(taskunique));
	memset(computer, 0, sizeof(computer));
	memset(groupname, 0, sizeof(groupname));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,1,  "taskunique", UT_TYPE_STRING, sizeof(taskunique)-1, taskunique);
	if( (1 != iReturn)||
	    (0==strlen(taskunique))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  taskunique:%s",  iReturn,  taskunique);
		recodeLogInfo(" ��������ִ�н���б�","  ���ղ���ʧ��!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}					   
	
	snprintf(sql, sizeof(sql)-1, "select  computerid, computer,  nwSoftwareDistributionSpeed.groupid, nwSoftwareDistributionSpeed.groupname, state, retry, lastruntime,ip "
		" from nwSoftwareDistributionSpeed, nwcompute  "
		" where nwSoftwareDistributionSpeed.computerid=  nwcompute.compid  and  taskunique =\'%s\'",  taskunique);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo(" ��������ִ�н���б�"," ��ѯ���ݿ�nwSoftwareDistributionSpeedʧ��!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		int 	iReturn =0;
		int 	num=0;	
		char	str[8][24];
		memset(str, 0, sizeof(str));

		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 0);

		while(iReturn == 0){
			memset(computer, 0, sizeof(computer));
			memset(groupname, 0, sizeof(groupname));
			iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &compid,
							UT_TYPE_STRING, sizeof(computer)-1, computer,
							UT_TYPE_LONG, 4, &groupid,
							UT_TYPE_STRING, sizeof(groupname)-1, groupname,
							UT_TYPE_LONG, 4, &state,
							UT_TYPE_LONG, 4, &retry,
							UT_TYPE_LONG, 4, &lastruntime,
							UT_TYPE_ULONG, 4, &sip);
			if( (iReturn != 0)){
				continue;
			}

			snprintf(str[0], sizeof(str[0])-1, "taskunique%d", num);
			snprintf(str[1], sizeof(str[1])-1, "compid%d", num);
			snprintf(str[2], sizeof(str[2])-1, "computer%d", num);
			snprintf(str[3], sizeof(str[3])-1, "groupid%d", num);
			snprintf(str[4], sizeof(str[4])-1, "groupname%d", num);
			snprintf(str[5], sizeof(str[5])-1, "state%d", num);
			snprintf(str[6], sizeof(str[6])-1, "retry%d", num);
			snprintf(str[7], sizeof(str[7])-1, "lastruntime%d", num);
			snprintf(str[8], sizeof(str[8])-1, "sip%d", num);
			num++;


			utMsgAdd_s(&psMsgHead, str[0],  taskunique);
			utMsgAdd_l(&psMsgHead, str[1],  compid);
			utMsgAdd_s(&psMsgHead, str[2],  computer);
			utMsgAdd_l(&psMsgHead, str[3],  groupid);
			utMsgAdd_s(&psMsgHead, str[4],  groupname);
			utMsgAdd_l(&psMsgHead, str[5],  state);
			utMsgAdd_l(&psMsgHead, str[6],  retry);
			utMsgAdd_l(&psMsgHead, str[7],  lastruntime);
			utMsgAdd_l(&psMsgHead, str[8],  sip);
		}
		pasDbCloseCursor(psCur);
		utMsgAdd_l(&psMsgHead, "state",  1);
		utMsgAdd_l(&psMsgHead, "number",  num);
		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}

	return 0;

}

int  Lan_RemoveTask(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		taskunique[128];
	unsigned int	compid =0;
	char		sql[1024];
	char		mesg[256];
	unsigned int	MyTaskRunList[10240] = {0};
	int		number = 0;

	memset(taskunique, 0, sizeof(taskunique));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,1,  "taskunique", UT_TYPE_STRING, sizeof(taskunique)-1, taskunique);
	if( (1 != iReturn)||
	    (0==strlen(taskunique))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  taskunique:%s",  iReturn,  taskunique);
		recodeLogInfo(" ɾ������","  ���ղ���ʧ��!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}	

	snprintf(sql, sizeof(sql)-1, "delete from nwSoftwareDistribution  where  taskunique=\'%s\' ",  taskunique);
	if(pasDbExecSql(sql, 0)){
		recodeLogInfo(" ɾ������", "ERROR: " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,-3);
		return -1;	
	}

	snprintf(sql, sizeof(sql)-1, "select  computerid from nwSoftwareDistributionSpeed  where  taskunique=\'%s\' ",  taskunique);
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //��ѯ���ݿ�ʧ�ܣ���¼��־
		recodeLogInfo(" ɾ������"," ��ѯ���ݿ�nwSoftwareDistributionSpeedʧ��!", sql);
        }else{
		int 	iReturn =0;
		int 	num=0;
		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 0);

		while(iReturn == 0){
			compid= 0;
			iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &compid );
			if(0 != iReturn){
				break;
			}

			snprintf(mesg, sizeof(mesg)-1, " compid:%u ",    compid);
			recodeLogInfo(" ɾ������", "��ѯ��compid : " , mesg);

			MyTaskRunList[number++] = compid;
		}
		pasDbCloseCursor(psCur);
	}

	snprintf(sql, sizeof(sql)-1, "delete from nwSoftwareDistributionSpeed  where  taskunique=\'%s\' ",  taskunique);
	if(pasDbExecSql(sql, 0)){
		recodeLogInfo(" ɾ������", "ERROR: " , sql);	
	}

	int num=0;
	for(num = 0; num < number; num++){
		nwCompIp * psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&(MyTaskRunList[num]));
		if(!psCompIp){
			snprintf(mesg, sizeof(mesg)-1, " compid:%u ������",    MyTaskRunList[num]);
			recodeLogInfo(" ɾ������", "ERROR: " , mesg);	
		}else{
			snprintf(mesg, sizeof(mesg)-1, " compid:%u ����",    MyTaskRunList[num]);
			recodeLogInfo(" ɾ������", "Success : " , mesg);
			psCompIp->lCtrl |= 0x80000;
		}
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* ��Կ����ʱ����  */ 1,"state",   UT_TYPE_LONG,  1);
	return 0;
}



int ncInitNwWebFun_SoftwareDistribution(utShmHead *psShmHead)
{
	pasSetTcpFunName("Lan_SoftwareUp",     		Lan_SoftwareUp,    NULL,0,0);
	pasSetTcpFunName("Lan_SoftwareInfoUp",     	Lan_SoftwareInfoUp,    NULL,0,0);
	pasSetTcpFunName("Lan_SoftwareConditionUp",     	Lan_SoftwareConditionUp,    NULL,0,0);
	pasSetTcpFunName("Lan_DistributionTaskUp",     	Lan_DistributionTaskUp,    NULL,0,0);

//	pasSetTcpFunName("Lan_GetSoftwarePackageList",   Lan_GetSoftwarePackageList,    NULL,0,0);
//	pasSetTcpFunName("Lan_GetSoftwareConditionList",	Lan_GetSoftwareConditionList,    NULL,0,0);

	pasSetTcpFunName("Lan_RequestTaskList",     	Lan_RequestTaskList,    NULL,0,0);
	pasSetTcpFunName("Lan_RequestTaskConditions",    Lan_RequestTaskConditions,    NULL,0,0);
	pasSetTcpFunName("Lan_RequestPackage",     	Lan_RequestPackage,    NULL,0,0);
	pasSetTcpFunName("Lan_ReturnResult",     	Lan_ReturnResult,    NULL,0,0);

	pasSetTcpFunName("Lan_GetPackageList",     	Lan_GetPackageList,    NULL,0,0);
	pasSetTcpFunName("Lan_GetPackageDetailInfo",     Lan_GetPackageDetailInfo,    NULL,0,0);
	pasSetTcpFunName("Lan_GetPackageConditionList",  Lan_GetPackageConditionList,    NULL,0,0);
	pasSetTcpFunName("Lan_RemovePackage",     	Lan_RemovePackage,    NULL,0,0);

	pasSetTcpFunName("Lan_GetTaskList",     		Lan_GetTaskList,    NULL,0,0);
	pasSetTcpFunName("Lan_GetTaskDetailInfo",     	Lan_GetTaskDetailInfo,    NULL,0,0);
	pasSetTcpFunName("Lan_GetTaskRunList",     	Lan_GetTaskRunList,    NULL,0,0);
	pasSetTcpFunName("Lan_RemoveTask",     		Lan_RemoveTask,    NULL,0,0);

	return 0;
}

