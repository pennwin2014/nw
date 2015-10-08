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
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include <pcap.h>
#include "ncdef.h"
#include "dsutl.h"
#include "dsclt.h"
//#include "nccltdef.h"


//windows管理端登陆验证
int Lan_Login(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
  	char	username[128];
	char	passwd[128];
	char	compname[128];
	char	sql[1024];
	char 	mesg[512];

	memset(username, 0, sizeof(username));
	memset(passwd, 0, sizeof(passwd));
	memset(compname, 0, sizeof(compname));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));
	  
 	int iReturn = utMsgGetSomeNVar(psMsgHead,3,
		"username", UT_TYPE_STRING, sizeof(username),username,
		"passwd",    UT_TYPE_STRING, sizeof(passwd), passwd,
		"compname", UT_TYPE_STRING, sizeof(compname)-1,  compname);

	snprintf(mesg, sizeof(mesg)-1, " compname:%s, username:%s, passwd:%s",  compname, username, passwd);

	if( (0 == strlen(username)) || 
	    (0 == strlen(passwd))||
	    (0 == strlen(compname))){
		recodeLogInfo("windows管理端登陆：","接收数据有误： ", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL,1,     "state",  UT_TYPE_LONG,  -2);
		return -2;
	}

	//用户名和密码 验证
	iReturn = dsCltUserLogin4(compname, username, passwd);
	if(0 == iReturn){
		//snprintf(mesg, sizeof(mesg)-1, " username:%s登陆！",   username);
		recodeLogInfo("windows管理端登陆：","密码验证成功！： ", mesg);
	 	pasTcpResponse(iFd,psMsgHead,NULL,1,     "state",  UT_TYPE_LONG,  1);
	}else{
		int  count = 0;
		snprintf(sql, sizeof(sql)-1, "select count(*) from nwAuditFileUser  where compname=\'%s\' and  nameid=\'%s\' and passwd=\'%s\' ",compname ,username, passwd);
		pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count);
		if(count > 0){
			//snprintf(mesg, sizeof(mesg)-1, " username:%s登陆！",   username);
			recodeLogInfo("windows管理端登陆：","密码验证成功！： ", mesg);
	 		pasTcpResponse(iFd,psMsgHead,NULL,1,     "state",  UT_TYPE_LONG,  1);
			return 0;
		}

		//snprintf(mesg, sizeof(mesg)-1, " username:%s, passwd:%s",   username, passwd);
		recodeLogInfo("windows管理端登陆：","密码验证失败！ ", mesg);
	 	pasTcpResponse(iFd,psMsgHead,NULL,1,     "state",  UT_TYPE_LONG,  -3);
	}

	return 0;
}

//文件审计部门列表的获取
int GetComputerList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
 	unsigned long 	compid = 0;
  	unsigned long 	command = 0;
    	unsigned long 	groupid = 0;
    	unsigned long 	groupid1=0;
    	unsigned int 	sum = 0;

    	char 		buff[40960];
   	char 		compname[128];
    	char 		gname[128];
	char 		sql[1024];
  	char 		mesg[512];

	memset(buff,0,sizeof(buff));
	memset(compname,0,sizeof(compname));
	memset(gname,0,sizeof(gname));
	memset(sql,0,sizeof(sql));
	memset(mesg,0,sizeof(mesg));
  
  	int iReturn = utMsgGetSomeNVar(psMsgHead,3,
		"computerid", UT_TYPE_ULONG, sizeof(compid),  &compid,
		"command",    UT_TYPE_ULONG, sizeof(command), &command,
		"compname", UT_TYPE_STRING, sizeof(compname)-1,  compname);

	if( command <= 0){
		snprintf(mesg, sizeof(mesg)-1, " compid:%u, command:%d, compname:%s",   compid, command, compname);
		recodeLogInfo("windows管理端获取部门列表：","参数有误！ ", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL,1, "state",  UT_TYPE_LONG, -2);
		return 0;
      	}

    	if(1 == command){
        		sprintf(sql,"select compname,groupid,gname from nwcompute left join nwgroup on nwgroup.gid=nwcompute.groupid where compid=%u", compid);
        		pasDbOneRecord(sql, 0, 
         		UT_TYPE_STRING, sizeof(compname)-1, compname,
          		UT_TYPE_ULONG,  sizeof(groupid),    &groupid, 
          		UT_TYPE_STRING, sizeof(gname)-1,    gname);
        
        		pasTcpResponse(iFd,psMsgHead,NULL,3,
    		 	"computername",  UT_TYPE_STRING, compname,
    		 	"groupid",       UT_TYPE_ULONG,  groupid,
    		 	"groupname",	UT_TYPE_STRING,	gname);
    		return 0;
    	}else if(2 == command){
    	  	//groupid = findCompanyByCompid(compid);

		snprintf(sql , sizeof(sql)-1, "select gid  from  nwgroup  where  gname=\'%s\'",   compname);
		pasDbOneRecord(sql, 0, UT_TYPE_ULONG,  4,  &groupid);

    	  	sprintf(buff,"id0:%lu,name0:全部,pid0:0,leaf0:0\n",groupid);
	  	sum = 1;
    	  	findChild(groupid,buff,&sum,1);
    	  
    	  	pasTcpResponse(iFd,psMsgHead,NULL,2,
    		  	"sum",  UT_TYPE_ULONG, sum,
    		  	"buff", UT_TYPE_STRING,buff);
    		return 0;
    	}else if(3 == command){
    	  	groupid1 = findCompanyByCompid(compid);
    	  	sprintf(buff,"id0:%lu,name0:全部,pid0:0,leaf0:0\n",groupid1);
	  	sum = 1;
    	  	findChild(groupid1,buff,&sum,2);

    	  	pasTcpResponse(iFd,psMsgHead,NULL,2,
    		  	"sum",  UT_TYPE_ULONG, sum,
    		  	"buff", UT_TYPE_STRING,buff);
    		return 0;
    	}else if(4 == command){
    	  	sprintf(buff,"id0:%lu,name0:全部,pid0:0,leaf0:0\n",0);
	  	sum = 1;
    	  	findChild(0,buff,&sum,2);

    	  	pasTcpResponse(iFd,psMsgHead,NULL,2,
    		  	"sum",  UT_TYPE_ULONG, sum,
    		  	"buff", UT_TYPE_STRING,buff);
		return 0;
	}else if(5 == command){
		snprintf(sql , sizeof(sql)-1, "select gid  from  nwgroup  where  gname=\'%s\'",   compname);
		pasDbOneRecord(sql, 0, UT_TYPE_ULONG,  4,  &groupid);

		sprintf(buff,"id0:%lu,name0:全部,pid0:0,leaf0:0\n", groupid);
	  	sum = 1;
    	  	findChild(groupid,buff,&sum,2);

    	 	pasTcpResponse(iFd,psMsgHead,NULL,3,
			"state", UT_TYPE_LONG, 1,
    		  	"sum",  UT_TYPE_ULONG, sum,
    		  	"buff", UT_TYPE_STRING,buff);
    		return 0;
    	}
  
	//未知的指令
	pasTcpResponse(iFd,psMsgHead,NULL,1,"sum",  UT_TYPE_ULONG, 2);
  	return 0;
}

int  Lan_GetNickname(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	unsigned int	compid = 0;
	char		devname[128];
	char		nickname[128];

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(devname, 0, sizeof(devname));
	memset(nickname, 0, sizeof(nickname));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,2,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
	     					 "compid", UT_TYPE_ULONG, 4, &compid);
	if( (2 != iReturn)||
	    (0==strlen(compname))||
	    (0==compid)){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, compid:%u",  iReturn,  compname,compid);
		recodeLogInfo(" 获取用户昵称","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}
	
	snprintf(sql, sizeof(sql)-1, "select a.devname  , nwAuditFileNickname.nickname from (select devname from nwcompute where compid=%u) a  left join  nwAuditFileNickname  "
		"  on(  nwAuditFileNickname.compname=\'%s\' and nwAuditFileNickname.compid = %u) ",  compid, compname, compid);
		
	pasDbOneRecord(sql,0,UT_TYPE_STRING,sizeof(devname)-1,devname,
			       UT_TYPE_STRING,sizeof(nickname)-1,nickname);
		
	if( (0 == strlen(nickname))&&
	    (0 == strlen(devname))){
		recodeLogInfo(" 获取用户昵称", "该用户不存在: " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-3);
		return -3;
	}

	if(0 != strlen(nickname)){
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2,"state",   UT_TYPE_LONG,1,
									    "nickname",   UT_TYPE_STRING,nickname);
	}else{
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2,"state",   UT_TYPE_LONG,1,
									    "nickname",   UT_TYPE_STRING,devname);

	}
		
	return 0;
}


int ncInitNwWebFun_WindowsConsole(utShmHead *psShmHead)
{
	pasSetTcpFunName("Lan_Login",  Lan_Login,  NULL,0,0);
	pasSetTcpFunName("GetComputerList",     GetComputerList,    NULL,0,0);
	pasSetTcpFunName("Lan_GetNickname",     Lan_GetNickname,    NULL,0,0);
		
	return 0;
}