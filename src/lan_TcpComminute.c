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


//通知客户端升级-----0X00000001
int Lan_UpdateVer(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int   compid = 0;
	char  query  = '\0';
	int   status = 0;
	char  serverip[17];
	short port = 0;
	char  packet[257];
	int   len = 0;
	char  version[21];
	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);

	memset(serverip, 0, sizeof(serverip));
	memset(packet,   0,  sizeof(packet));
	memset(version,  0,  sizeof(version));


    	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		                        "compid",  UT_TYPE_LONG,    4, &compid,
					"query",   UT_TYPE_STRUCT,  1, &query);


///////////////////////////////手工赋值 测试//////////////
	status = 1;
	port   = 10004;
	len = 200;
	snprintf(serverip, sizeof(serverip)-1, "192.168.20.190");
	snprintf(packet,   sizeof(packet)-1,  "/home/ncmysql/nw/111.txt");
	snprintf(version, sizeof(version)-1,  "0.12.1");




	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 7,
					 "compid",  UT_TYPE_LONG,    compid,
                     			 "status",  UT_TYPE_LONG,    status,
					 "serverip",UT_TYPE_STRING,  serverip,
					 "port",    UT_TYPE_SHORT,   port,
					 "packet",  UT_TYPE_STRING,  packet,
					 "len",     UT_TYPE_LONG,    len,
					 "version", UT_TYPE_STRING,  version	 
					 );

}

// 获取客户端信息 ------0X00000002
int Lan_GetClientInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int   compid = 0;
	char  query = 0;
	char  type = 0;
	char  subtypename[20];
	char  Pdnameid[20];
	int   Namelines = 0;
	char  Name[130];
	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);

	memset(subtypename, 0, sizeof(subtypename));
	memset(Pdnameid, 0, sizeof(Pdnameid));
	memset(Name, 0, sizeof(Name)); 


   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		                        "compid",  UT_TYPE_LONG,   4,  &compid,
					"query",   UT_TYPE_STRUCT, 1,  &query);



///////////////////////////////手工赋值 测试//////////////
	type = '1';
	Namelines = 12;
	snprintf(subtypename, sizeof(subtypename)-1, "win-7EF12");
	snprintf(Pdnameid,  sizeof(Pdnameid) -1, "111111909");
	snprintf(Name,  sizeof(Name)-1, "who am i?");

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 6,
					 "compid",     UT_TYPE_LONG,   compid,
                     			 "type",       UT_TYPE_STRUCT, &type,1,
					 "subtypename",UT_TYPE_STRING, subtypename,
					 "Pdnameid",   UT_TYPE_STRING, Pdnameid,
					 "Namelines",  UT_TYPE_LONG,   Namelines,
					 "Name",       UT_TYPE_STRING, Name
					 );

}

//上传硬件设备信息
int Lan_PutClientInfo_Hard(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char   sql_CreateTable[3000];
	char   sql_InsertInfo[3000];
	int     compid = 0;
	char   type = '\0';
	char   subtypename[20];
	char   Pdnameid[20];
	short  Nums = 0;
	char   Dname0[20];
	char   Dattr0[20];
	int    Dvalue0 = 0;
	int    status  = 0;
	int    Nums2 = 0;
	long   iReturn = 0;
	int    iRet = 0;
	utMsgPrintMsg(psMsgHead);
	

	memset(sql_CreateTable,  0, sizeof(sql_CreateTable));
	memset(sql_InsertInfo, 0, sizeof(sql_InsertInfo));
	memset(subtypename, 0, sizeof(subtypename));
	memset(Pdnameid, 0 , sizeof(Pdnameid));
	memset(Dname0, 0, sizeof(Dname0));
	memset(Dattr0,     0 , sizeof(Dattr0));

   	iReturn = utMsgGetSomeNVar(psMsgHead,5,
		                        "compid",      UT_TYPE_LONG,   4,  &compid,
					"type",        UT_TYPE_STRUCT, 1,  &type,
					"subtypename", UT_TYPE_STRING, 16, subtypename,
					"Pdnameid",    UT_TYPE_STRING, 16, Pdnameid,
					"Nums",        UT_TYPE_SHORT,  2,  &Nums
					);

	int      Numbers;
	char  str[3][50];
	memset(str, 0, 3*50);

	for(Numbers=0;  Numbers < Nums; Numbers++)
	{
		snprintf(str[0], 50, "Dname%d", Numbers);
		snprintf(str[1], 50, "Dattr%d",    Numbers);
		snprintf(str[2], 50, "Dvalue%d", Numbers);

		iReturn = utMsgGetSomeNVar(psMsgHead,3,
							str[0],  UT_TYPE_STRING, 16, Dname0,
							str[1],  UT_TYPE_STRING, 128,Dattr0,
							str[2],  UT_TYPE_LONG,   4,  &Dvalue0
							);
	}
	time_t now;
	time(&now);

	//往表中插入数据语句
	snprintf(sql_InsertInfo, 3000, "  insert into  \
				lan_nwhardware(sid, compid, devtype, dname, dattr, dvalue, nameid, panmeid, addtime, lasttime)  \
				values(9, compid, subtypename,  Dname0,  Dattr0, Dvalue0, Pdnameid, Pdnameid, now, now )      \
				");

	//如果表lan_nwhardware不存在， 则创建表，然后插入数据
	if(pasDbExecSql("select * from lan_nwhardware", 0) != 0)
	{
		//读取表结构
		iReturn = readTableStruct("../src/table.sql","lan_nwhardware", sql_CreateTable, 3000);

		//读取lan_nwhardware表结构成功
		if(iReturn == 0) 
		{
			iReturn = pasDbExecSql(sql_CreateTable,0);
			if(iReturn == 0)//创建表成功
			{
				iReturn = pasDbExecSql(sql_InsertInfo,0);
				if(iReturn == 0) //向表中插入数据成功
				{	
					status = 1;
					iRet = 0;
				}
				else
				{
					status = 0;
					iRet = -1;
				}
			}
			else
			{
				status = 0; 
				iRet = -1;
			}
		}
		else
		{
			status = 0;
			iRet = -1;
		}
	}
	else//如果表存在，则直接插入数据
	{
		//向表中插入数据
		iReturn = pasDbExecSql(sql_InsertInfo,0);
		if(iReturn == 0) //插入表成功
		{	
			status = 1;
			iRet = 0;
		}
		else
		{
			status = 0;
			iRet = -1;
		}
	}


///////////////////////////手动赋值，测试//////////////
	Nums2 = 99;

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2 ,
					 "status",  UT_TYPE_LONG,  status,
                     			 "Nums",    UT_TYPE_LONG,  Nums2
					 );
	return iRet;
}

//上传已安装的软件信息
int Lan_PutClientInfo_SetupProgram(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int   compid = 0;
	char  type = 0;
	char  subtypename[80];
	short Nums = 0;
	char  sname0[80];
	char  stype0[80];
	char  Sattrib0[130];
	int   ssize0 = 0;
	char  version[80];
	char  instime[80];
	int   status = 0;
	int   Nums2 = 0;
	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);

 	memset(subtypename, 0, sizeof(subtypename));
	memset(sname0, 0, sizeof(sname0));
	memset(stype0,  0, sizeof(stype0));
	memset(Sattrib0, 0, sizeof(Sattrib0));
	memset(version, 0,  sizeof(version));
	memset(instime, 0, sizeof(instime));

    	iReturn = utMsgGetSomeNVar(psMsgHead,4,
		                        "compid",      UT_TYPE_LONG,   4,  &compid,
					"type",        UT_TYPE_STRUCT, 1,  &type,
				 	"subtypename", UT_TYPE_STRING, 16, subtypename,
					"Nums",        UT_TYPE_SHORT,  2,  &Nums
					);
	int      Numbers;
	char  str[6][50];
	memset(str, 0, 6*50);

	for(Numbers=0;  Numbers < Nums; Numbers++)
	{
		snprintf(str[0], 50, "sname%d",   Numbers);
		snprintf(str[1], 50, "stype%d",   Numbers);
		snprintf(str[2], 50, "Sattrib%d", Numbers);
		snprintf(str[3], 50, "ssize%d",   Numbers);
		snprintf(str[4], 50, "version%d", Numbers);
		snprintf(str[5], 50, "instime%d", Numbers);

		iReturn = utMsgGetSomeNVar(psMsgHead,6,
							   str[0], UT_TYPE_STRING, 72,  sname0,
							   str[1], UT_TYPE_STRING, 16,  stype0,
							   str[2], UT_TYPE_STRING, 128, Sattrib0,
							   str[3], UT_TYPE_LONG,   4,  &ssize0,
							   str[4], UT_TYPE_STRING, 16,  version,
							   str[5], UT_TYPE_STRING, 16,  instime
							   );
	}
///////////////////////////////手工赋值，测试//////////////////////////
	status = 1;
	Nums2 = 120;

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2 ,
					 "status",  UT_TYPE_LONG,  status,
                     			 "Nums",    UT_TYPE_LONG,  Nums2
					 );


}

//上传正在运行的进程信息
int Lan_PutClientInfo_RunProcess(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int   compid = 0;
	char  type = 0;
	char  subtypename[17];
	short Nums = 0;
	char  Name0[25];
	char  Path0[257];
	char  Content0[129];
	char  indexid0[25];
	char  Pindexid0[25];
	int   Pid0 = 0;
	int   Cpuusage0 = 0;
	char  Cpus0 = 0;
	int   Mem0 = 0;
	int   Vmem0 = 0;
	char  User0[25];

	int   status = 0;
	int   Nums2 = 0;
	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);

	memset(subtypename, 0 , sizeof(subtypename));
	memset(Name0, 0, sizeof(Name0));
	memset(Path0, 0, sizeof(Path0));
	memset(Content0, 0, sizeof(Content0));
	memset(indexid0, 0, sizeof(indexid0));
	memset(Pindexid0, 0, sizeof(Pindexid0));
	memset(User0, 0, sizeof(User0));

    	iReturn = utMsgGetSomeNVar(psMsgHead,4,
		                        "compid",      UT_TYPE_LONG,    4,  &compid,
					"type",        UT_TYPE_STRUCT,  1,  &type,
					"subtypename", UT_TYPE_STRING,  16, subtypename,
					"Nums",        UT_TYPE_SHORT,   2,  &Nums
					);
	int   Numbers;
	char  str[11][50];
	memset(str, 0, 11*50);

	for(Numbers=0;  Numbers < Nums; Numbers++)
	{
		snprintf(str[0], 50, "Name%d",   Numbers);
		snprintf(str[1], 50, "Path%d",   Numbers);
		snprintf(str[2], 50, "Content%d", Numbers);
		snprintf(str[3], 50, "indexid%d",  Numbers);
		snprintf(str[4], 50, "Pindexid%d",  Numbers);
		snprintf(str[5], 50, "Pid%d",  Numbers);
		snprintf(str[6], 50, "Cpuusage%d", Numbers);
		snprintf(str[7], 50, "Cpus%d", Numbers);
		snprintf(str[8], 50, "Mem%d", Numbers);
		snprintf(str[9], 50, "Vmem%d", Numbers);
		snprintf(str[10], 50, "User%d", Numbers);

		iReturn = utMsgGetSomeNVar(psMsgHead,11,
							   str[0], UT_TYPE_STRING, 24, Name0,
							   str[1], UT_TYPE_STRING, 256,Path0,
							   str[2], UT_TYPE_STRING, 128,Content0,
							   str[3], UT_TYPE_STRING, 24, indexid0,
							   str[4], UT_TYPE_STRING, 24, Pindexid0,
							   str[5], UT_TYPE_LONG,   4,  &Pid0,
							   str[6], UT_TYPE_LONG,   4,  &Cpuusage0,
							   str[7], UT_TYPE_STRUCT, 1,  &Cpus0,
							   str[8], UT_TYPE_LONG,   4,  &Mem0,
							   str[9], UT_TYPE_LONG,   4,  &Vmem0,
							   str[10],UT_TYPE_STRING, 24, User0
							   );
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2 ,
					 "status",  UT_TYPE_LONG,  status,
                     			 "Nums",    UT_TYPE_LONG,  Nums2
					 );
}

//上传已安装的服务信息
int Lan_PutClientInfo_Service(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
	int   compid = 0;
	char  type = '\0';
	char  subtypename[17];
	short Nums = 0;
	char  ServiceName0[25];
	char  Description0[129];
	char  DisplayName0[25];
	char  ImagePath0[257];
	char  ObjectName0[25];
	int   Start0 = 0;
	int   Type0  = 0;
	int   Runing0= 0;

	int   status = 0;
	int   Nums2  = 0;
	long  iReturn= 0;
	utMsgPrintMsg(psMsgHead);

	memset(subtypename, 0, sizeof(subtypename));
	memset(ServiceName0, 0, sizeof(ServiceName0));
	memset(Description0, 0, sizeof(Description0));
	memset(ImagePath0, 0, sizeof(ImagePath0));
	memset(ObjectName0, 0, sizeof(ObjectName0));

    	iReturn = utMsgGetSomeNVar(psMsgHead,4,
		                        "compid",       UT_TYPE_LONG,   4,  &compid,
				        "type",         UT_TYPE_STRUCT, 1,  &type,
					"subtypename",  UT_TYPE_STRING, 16, subtypename,
					"Nums",         UT_TYPE_SHORT,  2,  &Nums
					);
	int   Numbers;
	char  str[8][50];
	memset(str, 0, 8*50);

	for(Numbers=0;  Numbers < Nums; Numbers++)
	{
		snprintf(str[0], 50, "ServiceName%d",   Numbers);
		snprintf(str[1], 50, "Description%d",   Numbers);
		snprintf(str[2], 50, "DisplayName%d", Numbers);
		snprintf(str[3], 50, "ImagePath%d",  Numbers);
		snprintf(str[4], 50, "ObjectName%d",  Numbers);
		snprintf(str[5], 50, "Start%d",  Numbers);
		snprintf(str[6], 50, "Type%d", Numbers);
		snprintf(str[7], 50, "Runing%d", Numbers);

		iReturn = utMsgGetSomeNVar(psMsgHead,8,
					str[0],  UT_TYPE_STRING, 24, ServiceName0,
					str[1],  UT_TYPE_STRING, 128,Description0,
					str[2],  UT_TYPE_STRING, 24, DisplayName0,
					str[3],  UT_TYPE_STRING, 256,ImagePath0,
					str[4],  UT_TYPE_STRING, 24, ObjectName0,
					str[5],  UT_TYPE_LONG,   4,  &Start0,
					str[6],  UT_TYPE_LONG,   4,  &Type0,
					str[7],  UT_TYPE_LONG,   4,  &Runing0							   
					);
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2 ,
					 "status",  UT_TYPE_LONG,  status,
                     			 "Nums",    UT_TYPE_LONG,  Nums2
					 );
}

//获取服务命令--0X00000004
int Lan_ClientInfo_ServiceOp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int  compid = 0;
	char query = '\0';
	char  runstatus = '\0';
	char type = '\0';
	char ServiceName[25];
	char Description[129];
	char DisplayName[25];
	char ImagePath[257];
	char ObjectName[25];
	int  Start = 0;
	int  Type = 0;
	int  Runing = 0;
	int  Status = 0;

	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);

	memset(ServiceName, 0, sizeof(ServiceName));
	memset(Description, 0, sizeof(Description));
	memset(ImagePath, 0, sizeof(ImagePath));
	memset(ObjectName, 0, sizeof(ObjectName));

    	iReturn = utMsgGetSomeNVar(psMsgHead,3,
		                        "compid",  UT_TYPE_LONG,    4,  &compid,
					"query",   UT_TYPE_STRUCT,  1,  &query,
					"runstatus",UT_TYPE_STRUCT, 1,  &runstatus
					);

/////////////////////////////手动赋值，测试///////////
	if(query != '\0')
	{  
		type = '1';
		Start = 1;
		Type = 1;
		Runing = 1;
		snprintf(ServiceName, 24, "linux--2385");
		snprintf(Description,    128, "linux--vvvv");
		snprintf(DisplayName, 24,  "ViewSonic");
		snprintf(ImagePath,  256, "C:/device/display/");
		snprintf(ObjectName, 24, "zxb");

		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 10,
					 "compid",      UT_TYPE_LONG,     compid,
					 "type",        UT_TYPE_STRUCT,   &type,1,
					 "ServiceName", UT_TYPE_STRING,   ServiceName,
					 "Description", UT_TYPE_STRING,   Description,
					 "DisplayName", UT_TYPE_STRING,   DisplayName,
					 "ImagePath",   UT_TYPE_STRING,   ImagePath,
					 "ObjectName",  UT_TYPE_STRING,   ObjectName,
					 "Start",       UT_TYPE_LONG,     Start,
					 "Type",        UT_TYPE_LONG,     Type,
					 "Runing",      UT_TYPE_LONG,     Runing
					 );
	}
	else if(runstatus != '\0')
	{
		Status = 1;
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1 ,"Status",  UT_TYPE_LONG,  Status);
	}


}

//下发消息----0X00000008
int Lan_Send_Msg(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

	int  compid = 0;
	char query = '\0';

	char Subject[129];
	char Msg[257];

	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);

	memset(Subject,0, sizeof(Subject));
	memset(Msg,    0, sizeof(Msg));

    	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		                        "compid",  UT_TYPE_LONG,  4, &compid,
					"query",   UT_TYPE_STRUCT, 1, &query);

///////////////////////手动赋值，测试///////////
	snprintf(Subject, 128, "好男人就是我");
	snprintf(Msg,  256,  "我就是好男人！");

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3 ,
					 "compid",  UT_TYPE_LONG,     compid,
					 "Subject", UT_TYPE_STRING,   Subject,
					 "Msg",     UT_TYPE_STRING,   Msg
					 );

}

//抓屏----0X00000010
int Lan_Capture_Screen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int  compid = 0;
	char query = '\0';

	char  Path[257];
	char  Serverip[49];
	short Port = 0;

	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);

	memset(Path,  0,  sizeof(Path));
	memset(Serverip, 0, sizeof(Serverip));

    	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		                        "compid",  UT_TYPE_LONG, 4,  &compid,
					"query",   UT_TYPE_STRUCT,1,  &query);

//////////////////////////////手动赋值，测试///////////////////////
	snprintf(Path,  256, "./QQ/2012");
	snprintf(Serverip, 48, "192.168.20.110");
	Port = 10004;

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 4 ,
					 "compid",   UT_TYPE_LONG,     compid,
					 "Path",     UT_TYPE_STRING,   Path,
					 "Serverip", UT_TYPE_STRING,   Serverip,
					 "Port",     UT_TYPE_SHORT,    Port
					 );
}


//远程控制------0X00000020
int Lan_Remote_Control(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int  compid = 0;
	char query = '\0';

	char  Controlmod = '\0';
	char  Serverip[49];
	short Port = 0;

	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);

	memset(Serverip,0, sizeof(Serverip));

    	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		                        "compid",  UT_TYPE_LONG,  4, &compid,
					"query",   UT_TYPE_STRUCT, 1, &query);


/////////////////////手动赋值，测试///////
	snprintf(Serverip, 48, "192.168.20.190");
	Controlmod = '1';
	Port = 10004;

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 4,
					 "compid",    UT_TYPE_LONG,    compid,
					 "Controlmod",UT_TYPE_STRUCT,  &Controlmod, 1,
					 "Serverip",  UT_TYPE_STRING,   Serverip,
					 "Port",      UT_TYPE_SHORT,    Port
					 );
	return 0;
}

//客户端操作----0X00000040
int Lan_Client_Op(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int  compid = 0;
	char query = '\0';

	char  Optype = '\0';
	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);


    	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		                        "compid",  UT_TYPE_LONG,  4, &compid,
					"query",   UT_TYPE_STRUCT,  1,  &query);

//////////////////手动赋值，测试/////////
	Optype = '1';

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2,
					 "compid",   UT_TYPE_LONG,   compid,
					 "Optype",   UT_TYPE_STRUCT, &Optype,1
					 );
}

//更换心跳服务器信息-------0X00000100
int Lan_Client_Set(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int  compid = 0;
	char query = '\0';

	char  Optype = '\0';
	char  Serverip[49];
	short Udpport = 0;
	short tcpport = 0;
	char  timelen = '\0';

	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);
	
	memset(Serverip, 0, sizeof(Serverip));

    	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		                        "compid",  UT_TYPE_LONG,   4, &compid,
					"query",   UT_TYPE_STRUCT, 1, &query);


////////////////////////手动赋值，测试//////////
	Optype = '1';
	Udpport = 10004;
	tcpport = 8250;
	timelen = 12;
	snprintf(Serverip, 48, "192.168.20.190");
	

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 6,
					 "compid",   UT_TYPE_LONG,     compid,
					 "Optype",   UT_TYPE_STRUCT,   &Optype,1,
					 "Serverip", UT_TYPE_STRING,   Serverip,
					 "Udpport",  UT_TYPE_SHORT,    Udpport,
					 "tcpport",  UT_TYPE_SHORT,    tcpport,
					 "timelen",  UT_TYPE_STRUCT,   &timelen, 1
					 );

}

//下发策略-----0X00000080
int  Lan_Law_down(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int   compid = 0;
	char  query = 0;
	char  Status = 0;
	char  Nums = 0;
	char  Name0[257];
	long  iReturn = 0;
	utMsgPrintMsg(psMsgHead);

    	iReturn = utMsgGetSomeNVar(psMsgHead,3,
		                        "compid",  UT_TYPE_LONG,  4,  &compid,
					"query",   UT_TYPE_STRUCT,1,  &query,
					"Status",  UT_TYPE_STRUCT,1,  &Status);


	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 3,
		             		  "compid", UT_TYPE_LONG,   compid,
					  "Nums",   UT_TYPE_STRUCT, &Nums, 1,
					  "Name0",  UT_TYPE_STRING, Name0);

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2,
					 "compid", UT_TYPE_LONG,    compid,
					 "Status", UT_TYPE_STRUCT,  &Status, 1);

}


int ncInitNwWebFun_TcpComminute(utShmHead *psShmHead)
{
	pasSetTcpFunName("Lan_UpdateVer",Lan_UpdateVer ,NULL,0,0);

	pasSetTcpFunName("Lan_GetClientInfo",      Lan_GetClientInfo ,     NULL,0,0);
	pasSetTcpFunName("Lan_PutClientInfo_Hard",Lan_PutClientInfo_Hard, NULL,0,0);
	pasSetTcpFunName("Lan_PutClientInfo_SetupProgram", Lan_PutClientInfo_SetupProgram, NULL,0,0);
	pasSetTcpFunName("Lan_PutClientInfo_RunProcess",   Lan_PutClientInfo_RunProcess,   NULL,0,0);
	pasSetTcpFunName("Lan_PutClientInfo_Service",      Lan_PutClientInfo_Service,      NULL,0,0);


	pasSetTcpFunName("Lan_ClientInfo_ServiceOp",Lan_ClientInfo_ServiceOp ,NULL,0,0);

	pasSetTcpFunName("Lan_Send_Msg",       Lan_Send_Msg,      NULL,0,0);
	pasSetTcpFunName("Lan_Capture_Screen", Lan_Capture_Screen,NULL,0,0);
	pasSetTcpFunName("Lan_Remote_Control", Lan_Remote_Control,NULL,0,0);
	pasSetTcpFunName("Lan_Client_Op",      Lan_Client_Op,     NULL,0,0);
	pasSetTcpFunName("Lan_Client_Set",     Lan_Client_Set,    NULL,0,0);
	pasSetTcpFunName("Lan_Law_down",       Lan_Law_down,      NULL,0,0);

	return 0;
}
