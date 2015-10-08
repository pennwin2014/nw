/* ProWeb  */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "config.h"
#include "utoall.h"
#include "pasutl.h"
#include "ncdef.h"
int     ncTcpInit(utShmHead *psShmHead);
extern int  iDebugLevel;
int         iDefaultAccess=0;
int         iNcClient=0;
int         iNcSendGA=0;

int  ncWebTest1234(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int main(int argc,char **argv)
{
    utShmHead *psShmHead;
    char *pLog;
    char argval;
    char caConfFile[64];
    char caVar[32];
    int  iReturn;
    int  i;
    printf(" Now Start proeim_web...............\n");
    utSetLogFile("/tmp/nclog.log");
    strcpy(caConfFile,"/home/ncmysql/nc/bin/pqconf_nc.cnf");
    argv++;
    argc--;
    iDebugLevel = 0;
    while(argc) {
        if(**argv != '-')
            proEimUsage();
        argval = *(*argv + 1);
        argc--;
        argv++;
        switch(argval) {
            case 'd':
                strcpy(caConfFile,(*argv));
                argc--;
                argv++;
                break;
            case 'h':
                proEimUsage();
                break;
            default:
                proEimUsage();
                exit(9);
        }
    }
     psShmHead=utComShmOpen(caConfFile);
    if(psShmHead == NULL) {
        printf(" Share Memory not Init \n");
        exit(0);
    }
    
    ncLangInit(psShmHead);

    pLog = utComGetVar_s(psShmHead,"PLATEPATH");
    if(pLog) {
        utPltSetPlatePath(pLog);
    }    

    pLog = utComGetVar_s(psShmHead,"DebugInfo");
	i = 1;
	while(pLog || i < 10) {
	    if(pLog) {
    	    printf("Set DebugInfo:[%s]\n",pLog);
        	pasSetDebugLogInfo(pLog);
        }
    	sprintf(caVar,"DebugInfo%d",i);
	    pLog = utComGetVar_s(psShmHead,caVar);
    	i++;
    }

    iReturn = dsCltInit(psShmHead);
        if(iReturn < 0) {
            pasLog(PAS_LOG_ERROR," Start dsserver Error \n");
            exit(9);
        }

    ncUtlSetGonfigValue(psShmHead);
    utComSetMaxTcpBuf(utComGetVar_ld(psShmHead,"MaxTcpBuffer",1024000L));
    ncInitWebFun(psShmHead);
    ncsInitNccFun(psShmHead);
    ncInitWebserviceFun(psShmHead);
    ncInitNwWebFun(psShmHead);
    dsCltSetSessionType("dispname",UT_TYPE_STRING,32,1);
    dsCltSetSessionType("email",UT_TYPE_STRING,63,1);
    dsCltSetSessionType("groupname",UT_TYPE_STRING,31,1);
    dsCltSetSessionType("groupid",  UT_TYPE_LONG,4,1);
    dsCltSetSessionType("usrlevel",  UT_TYPE_LONG,4,1);
    dsCltSetSessionType("mbflag",  UT_TYPE_LONG,4,0);
    dsCltSetSessionType("id",  UT_TYPE_LONG,4,1);
    dsCltSetSessionType("plate", UT_TYPE_STRING,63,0);
    iReturn = dsCltInit(psShmHead);
    if(iReturn < 0) {
        pasLog(PAS_LOG_ERROR," Start dsserver Error \n");
        exit(9);
    }
    iReturn = pasTcpServer(psShmHead,ncTcpInit);    /* Tcp处理进程   */
    if(iReturn < 0) {
        exit(9);
    }
    exit(0);
}


int  proEimUsage()
{
    printf("   proeim_web  Server Version 3.0   \n");
    printf("   Usage:  proeim_web  [-d configfile] \n");
    return 1;
}




int ncSetHttpFun001(utShmHead *psShmHead)
{
    iReturn = pasSetTcpFunName("test1234", ncWebTest1234, 0);
    return 0;
}

int  ncWebTest1234(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    /*
    int iReturn;    
    char    caSearch[32],caUserid[32],caLogin[32];
    pasDbCursor *psCur;
    iReturn = utMsgGetSomeNVar(psMsgHead,2,
	                "username",     UT_TYPE_STRING, 31, caSearch,
	                "password",     UT_TYPE_STRING, 31, caUserid,
	                "login",        UT_TYPE_STRING, 12, caLogin);
 
 
    psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),b.flags,b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode \
                from  %s b  order by b.sid desc limit %d,%d",pTable,lStartRec,lRowNum);
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
  	                               UT_TYPE_STRING,15,caDip,
  																 UT_TYPE_LONG,4,&lFlags,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,23,caSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lCode);
    pasDbCloseCursor(psCur);
    psDbHead = utPltInitDb();                          
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVarF(psDbHead,"lcount", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVar(psDbHead,"title_name",caTitlename);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    */
    return 0;
}
