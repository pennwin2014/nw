#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>
#include "string.h"
#include "utoall.h"
#include "ncdef.h"
#include "nwdef.h"
#include "pasdb.h"
#define NC_TESTUDP    19000
#define NC_CREATE_LOG 19001

//���ļ��ж�ȡ���ݿ��Ľṹ�� �����ȡ�ɹ��򷵻�0�����򷵻�-1
//����˵������"filename"�ļ��ж�ȡ"tablename"��ṹ �� "sql" �����У� sql������󳤶�Ϊ len
int  	readTableStruct(char *filename, char *tablename, char *sql, int len)
{
	int   iRet = -1;
	int   start= 0;

	char *first = NULL;
	char  lineData[3000];

	char   tableNameStart[100];
	char   tableNameOver[100];

	//������������򷵻�0
	if(filename == NULL || tablename == NULL || sql == NULL || len <= 0)
	{	
		iRet = -1;
	}
	else
	{
		memset(lineData,   0, sizeof(lineData) );
		memset(tableNameStart, 0, sizeof(tableNameStart));
		memset(tableNameOver,  0, sizeof(tableNameOver));
		sql[0] = '\0';

		FILE *file = fopen(filename,"rb+");
		if(file)
		{
			snprintf(tableNameStart, sizeof(tableNameStart)-1, "[%s", tablename);
			snprintf(tableNameOver,  sizeof(tableNameOver)-1,  "[/%s",tablename);

			while(!feof(file))
			{
		
				fgets(lineData,2000, file);

				first = strtok(lineData,"]--");

				if(first   &&  !strcmp(first,tableNameStart))
				{
					start = 1;
					continue;
				}
				else if(first   &&  !strcmp(first,tableNameOver))
				{
					iRet = 0;
					break;
				}

				if(first  &&  start == 1)
				{	
					if(strlen(sql) + strlen(first) > len )
					{
						iRet = -1;
						break;
					}	
					strcat(sql,first);
				}
			
			}
			fclose(file);				
		}
		else
		{
			iRet = -1;
		}
	}
	return iRet;
}


//����Tcp
int ncTestTcp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncHoteluser stHoteluser;
    char caUsername[64],caPassword[64];
    long iReturn,lStatus;
    char caPass_t[64];
    char caBuf[1024];
    char caNcname[32];
    char caMsg[64];
    char caSlgbm[16];
    long iRet;
    long iAuth=0;
    char caPass_temp[36],caUsername_temp[36];
    char caPassword0[36],caKey[36];
    char caDispname[32],caPosition[32],caIdfytype[8],caIdfyno[36],caFamaddr[150],caCountry[8],caSex[8];
    lStatus=-1;
    printf("fasdfasdfsadfasdf\n");
	utMsgPrintMsg(psMsgHead);

		 utMsgGetSomeNVar(psMsgHead,2, "username",UT_TYPE_STRING,32,caUsername,
		                                "passwd",UT_TYPE_STRING,32,caPassword);
		                                        
	
	
 	     pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* ��Կ����ʱ����  */
                    3,
                    "status",UT_TYPE_LONG,5,
                    "username",UT_TYPE_STRING,caUsername,
                    "password",UT_TYPE_STRING,caPassword);
     return 0;
 	
}

int ncTestUdp(utShmHead *psShmHead, utComSockAddr *psSockAddr,utMsgHead *psMsgHead)
{
  
    char caIp[20],caUname[32];
    char caPasswd[32];
    char caVersion[16];
    long iReturn;
    utMsgPrintMsg(psMsgHead);

    iReturn = utMsgGetSomeNVar(psMsgHead,3,
        "username",  UT_TYPE_STRING,31,  caUname,
        "passwd",    UT_TYPE_STRING,31,caPasswd,
		    "version",UT_TYPE_STRING, 8,  caVersion);

        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                       "status",  UT_TYPE_LONG,9,
                       "mesg",    UT_TYPE_STRING,"�û�������");
        return (0);
   
    
}


int ncCreateLog(utShmHead *psShmHead, utComSockAddr *psSockAddr,utMsgHead *psMsgHead)
{
  
    char cmd[10],cLogName[50],LogName[60];
	char sql[2000];
    char creturn[10];
    long iReturn;
    utMsgPrintMsg(psMsgHead);

    iReturn = utMsgGetSomeNVar(psMsgHead,2, 
							"cmd",     UT_TYPE_STRING,10,  cmd,
							"LogName", UT_TYPE_STRING,50,  cLogName);
	if(!strcmp(cmd,"create"))
	{
		time_t now;     
		struct tm *timenow; 

		//��ȡ���ڵ�ʱ�䣨���ʱ�׼ʱ��Ǳ���ʱ�䣩
		time(&now); 

		//��now���ɱ������е�ʱ�䣨���������õ�ʱ����
		timenow  = localtime(&now); 

		
		if(!strcmp(cLogName,"����ʹ����־"))
		{
			strcpy(cLogName,"nwproclog");
		}
		else if(!strcmp(cLogName,"�ļ�������־"))
		{
			strcpy(cLogName,"nwfilelog");
		}
		else if(!strcmp(cLogName,"���������־"))
		{
			strcpy(cLogName,"nwremovelog");
		}
		else if(!strcmp(cLogName,"��ſͻ��˷��������ļ�ʱͨѶ��"))
		{
			strcpy(cLogName,"ncimclient");
		}
		else if(!strcmp(cLogName,"��ĻJPG��־"))
		{
			strcpy(cLogName,"ncscreenlog");
		}
		else if(!strcmp(cLogName,"��ҳ������־"))
		{
			strcpy(cLogName,"nwweblog");
		}
		else if(!strcmp(cLogName,"�¼�������־"))
		{
			strcpy(cLogName,"nweventlog");
		}
		else
		{
			 utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                     "return",  UT_TYPE_STRING, "error3" );
  
			 return (0);
		}

		//Ҫ�����ı������ ��ʽ: LogName+20130809
		sprintf(LogName,"%s%04d%02d%02d",cLogName,1900+timenow->tm_year,1+timenow->tm_mon,timenow->tm_mday);

		
		//������־���sql���
		sprintf(sql,"create table 111111111( sid  int  primary key,  compid  char(16) );" );


		//����sql��� ������־��
		iReturn = pasDbExecSql("create table tb2( sid  int );",0);
		if(0 == iReturn)
		    strcpy(creturn, "OK");
		else
		    strcpy(creturn, "error");

		utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                     "return",  UT_TYPE_STRING, creturn );
	}
	else
	{		
		utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                     "return",  UT_TYPE_STRING, "error4" );		
	}
 
    return (0);
  
}


int ncInitNwWebFun_new(utShmHead *psShmHead)
{

//  	pasSetTcpFunNameS("ncTestTcp",ncTestTcp,NULL,0,PAS_CRYPT_TEA);         //����ע�ắ��
    	utComSetUdpFun(NC_TESTUDP, ncTestUdp);                                  //udpע��
	utComSetUdpFun(NC_CREATE_LOG, ncCreateLog);	

    	pasSetTcpFunName("ncTestTcp",ncTestTcp,NULL,0,0);                      //tcpע��

	//pasSetTcpFunName(" ",  ,NULL,0,0);
	return 0;
}