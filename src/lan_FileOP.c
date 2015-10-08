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


//获取文件大小
int Lan_GetFileSizeFromServer(utShmHead *psShmHead, int iFd,  utMsgHead *psMsgHead)
{
	char  path[257];
	int   status = 0;
	int   Filelen= 0;
	long  iReturn= 0;
	utMsgPrintMsg(psMsgHead);

    	iReturn = utMsgGetSomeNVar(psMsgHead,1,
		                       "path", UT_TYPE_STRING, 256, path);


	FILE *fp = fopen(path,"rb+");       
	if(NULL == fp)
	{	
		//找不到指定文件 //找不到指定路径
		status = -1;
		Filelen = -1;
	}
	else
	{
		status = 0;
		fseek(fp,0L,SEEK_END); /* 定位到文件末尾 */
		Filelen = ftell(fp); /* 得到文件大小 */  
		fclose(fp); 			
	}


	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2 ,
                     			 "status",   UT_TYPE_LONG,  status, 
					 "Filelen",  UT_TYPE_LONG,  Filelen);

	return status;
}

//从服务器获取文件
int Lan_GetFileFromServer(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char  path[257];
	int   Begpos = 0;
	int   Len  = 0;
	int   status = 0;
	char  *Data = NULL;
	long  iReturn= 0;
	utMsgPrintMsg(psMsgHead);

	memset(path, 0, sizeof(path));

    	iReturn = utMsgGetSomeNVar(psMsgHead,3,
		                        "path",  UT_TYPE_STRING, 256, path,
					"Begpos",UT_TYPE_LONG,    4,  &Begpos,
					"Len",   UT_TYPE_LONG,    4,   &Len );

	FILE *fp = fopen(path,"rb+");   
	if(NULL == fp)
	{	
		//找不到指定文件 //找不到指定路径
		status = -1;
		Len = -1;
	}
	else
	{	
		status = 0;
		fseek(fp,Begpos, SEEK_SET);
		if(-1 != Len)
		{
 		    	Data = (char *)malloc(Len);
			Len = fread(Data, 1, Len, fp);			   					
		}
		else
		{
 		    /* int num;
			Len = 0;
			for(num = 0; !feof(fp); num++)
			{
				Len += fread(Data+num*200, 1, 200, fp); 		
			}
		     */

			fseek(fp,0L,SEEK_END); /* 定位到文件末尾 */
			int Filelen = ftell(fp); /* 得到文件大小 */  

			Data = (char *)malloc(Filelen- Begpos);
			fseek(fp,Begpos,SEEK_SET); /* 定位到文件指定位置 开始读取数据*/
			Len = fread(Data, 1, Filelen - Begpos, fp);			
		}		  
		fclose(fp); 			
	}


	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3 ,
                     			 "status", UT_TYPE_LONG,  status, 
					 "Len",    UT_TYPE_LONG,  Len,
					 "Data",   UT_TYPE_STRUCT,Data,Len);

	free(Data);
	return status;
}

//上传文件到服务器
int Lan_PutFileToServer(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char  path[257];
	int   Begpos;
	int   Len;
	int   status;
	char  *Data = NULL;
	long  iReturn;
	utMsgPrintMsg(psMsgHead);

   	iReturn = utMsgGetSomeNVar(psMsgHead,3,
		                        "path",  UT_TYPE_STRING, 256, path,
					"Begpos",UT_TYPE_LONG,    4,   &Begpos,
					"Len",   UT_TYPE_LONG,    4,   &Len);

	Data = (char *)malloc(Len);
	utMsgGetSomeNVar(psMsgHead,1, "Data",  UT_TYPE_STRING, Len, Data);

	FILE *fp = fopen(path,"wb+");
	if(NULL == fp)
	{
		//找不到指定路径
		status = -2;			
	}
	else
	{
		if(Len > 0)
		{
			fwrite(Data,1, Len, fp);
			fclose(fp);
			status = 0;
		}
		else
		{
			status = -1;
		}
	}


	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,
                     "status", UT_TYPE_LONG,  status);

	free(Data);
	return status;
}

int ncInitNwWebFun_FileOP(utShmHead *psShmHead)
{
	pasSetTcpFunName("Lan_GetFileSizeFromServer",Lan_GetFileSizeFromServer,NULL,0,0);
	pasSetTcpFunName("Lan_GetFileFromServer",    Lan_GetFileFromServer,    NULL,0,0);
	pasSetTcpFunName("Lan_PutFileToServer",      Lan_PutFileToServer,      NULL,0,0);

}