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
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "pasutl.h"
#include "ncdef.h"
#define NC_CLEAN_ALL            0
#define NC_CLEAN_INSTANTDATA    1
#define NC_CLEAN_INSTANTBACK    2
#define NC_CLEAN_FORMMON        3
#define NC_CLEAN_FORMMONBACK    4
#define NC_CLEAN_MAILMON        5
#define NC_CLEAN_MAILMONBACK    6
#define NC_CLEAN_MAILCTL        7
#define NC_CLEAN_MAILCTLBACK    8

#define NC_CLEAN_IPLOG          9
#define NC_CLEAN_IPLOGBACK     10    
#define NC_CLEAN_HTTPLOG       11
#define NC_CLEAN_HTTPLOGBACK   12

#define NC_CLEAN_ERRLOG        13
#define NC_CLEAN_ERRLOGBACK    14
#define NC_CLEAN_ADMLOG        15
#define NC_CLEAN_ADMLOGBACK    16
#define NC_CLEAN_RUNLOG        17
#define NC_CLEAN_RUNLOGBACK    18
#define NC_CLEAN_WARNLOG       19
#define NC_CLEAN_WARNLOGBACK   20
#define NC_CLEAN_COUNT         21
#define NC_CLEAN_BASEINFO      22
#define NC_CLEAN_OTHERLOG      23
#define NC_CLEAN_ACCESSRULE    24
#define NC_CLEAN_USERINFO      25
#define NC_CLEAN_TCSET         26
#define NC_CLEAN_MAILCTLSET    27
#define NC_CLEAN_MAILMONSET    28
#define NC_CLEAN_FORMMONSET    29
#define NC_CLEAN_NETWORKSET    30
#define NC_CLEAN_USERURL       31
#define NC_CLEAN_NETLOG        32
#define NC_CLEAN_SESLOG        33
#define NC_CLEAN_DBLOG        34


/* 数据清除  */
int ncWebSystemDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncComputeMacIp *psMacIp;
    pasHashInfo sHashInfo;
     ncUserInfo  *psUserInfo;
    char *pHash;
    int iNum,iReturn,iLock,i,iCount;
    unsigned long lGroupid0;
    char caDo[64];
    char caStart[12],caEnd[12];
    char *p;
    char caVar[16];
    utPltDbHead *psDbHead;
    char caUsername2[32];
    char caTemp[1024];
    char caSql[1024];
    long lCount;
    iCount = 0;
    
    memset(caUsername2,0,sizeof(caUsername2));
    dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
    
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
    
    utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,2,
					"start",   UT_TYPE_STRING, 10, caStart,
					"end",     UT_TYPE_STRING, 10, caEnd);

    iLock = ncUtlIsLock(psShmHead,NC_LOCK_AUTOCLEAN,0);
    if(iLock) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0881自动整理"),ncLang("0669系统现在正忙%s，请稍后再试"),ncUtlGetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN));
        return 0;
    }
    memset(caDo,0,64);
    for(i=0;i<64;i++) {
        sprintf(caVar,"s%d",i);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            iCount++;
            caDo[i] = 1;
        }
        else {
            caDo[i] = 0;
        }
    }
    if(iCount > 0) {
 /*
    	 for(i=0;i<64;i++){
    	 	if(caDo[i]==1){
    	 		switch(i){
    	 			case NC_CLEAN_ALL:
    	 				break;
    	 			 case NC_CLEAN_INSTANTDATA:
    	 			 	 sprintf(caSql,"select count(*) from ncbackuplog where backtype='instant' and sdate>='%s'  ",ncMailCvtDate998(caEnd));
    	 			 	 pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
    	 			 	 if(lCount==0){
    	 			 	 	   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm","因该段时间的即时通讯日志没有被备份,所以不能删除","因该段时间的即时通讯日志没有被备份，所以不能删除",ncUtlGetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN));
                   return 0;
    	 			 	
    	 			 	 }
    	 			 	break;
    	 			 	 case NC_CLEAN_FORMMON:
    	 			 	 	lCount=0;
    	 			 	 sprintf(caSql,"select count(*) from ncbackuplog where backtype='formmon' and sdate>='%s'  ",ncMailCvtDate998(caEnd));
    	 			 	 pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
    	 			 	 if(lCount==0){
    	 			 	 	   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm","因该段时间的表单日志没有被备份,所以不能删除","因该段时间的表单日志没有被备份，所以不能删除",ncUtlGetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN));
                   return 0;
    	 			 	
    	 			 	 }
    	 			 	break;
    	 			 case NC_CLEAN_MAILMON:
    	 			 	 	lCount=0;
    	 			 	 sprintf(caSql,"select count(*) from ncbackuplog where backtype='mailmon' and sdate>='%s'  ",ncMailCvtDate998(caEnd));
    	 			 	 pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
    	 			 	 if(lCount==0){
    	 			 	 	   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm","因该段时间的邮件日志没有被备份,所以不能删除","因该段时间的邮件日志没有被备份，所以不能删除",ncUtlGetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN));
                   return 0;
    	 			 	
    	 			 	 }
    	 			 	break;
    	 			 	 case NC_CLEAN_IPLOG:
    	 			 	 	lCount=0;
    	 			 	 sprintf(caSql,"select count(*) from ncbackuplog where backtype='iplog' and sdate>='%s'  ",ncMailCvtDate998(caEnd));
    	 			 	 pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
    	 			 	 if(lCount==0){
    	 			 	 	   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm","因该段时间的IPLOG日志没有被备份,所以不能删除","因该段时间的IPLOG日志没有被备份，所以不能删除",ncUtlGetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN));
                   return 0;
    	 			 	
    	 			 	 }
    	 			 	break;
    	 			 	case NC_CLEAN_HTTPLOG:
    	 			 	 	lCount=0;
    	 			 	 sprintf(caSql,"select count(*) from ncbackuplog where backtype='httplog' and sdate>='%s'  ",ncMailCvtDate998(caEnd));
    	 			 	 pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
    	 			 	 if(lCount==0){
    	 			 	 	   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm","因该段时间的HTTPLOG日志没有被备份,所以不能删除","因该段时间的HTTPLOG日志没有被备份，所以不能删除",ncUtlGetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN));
                   return 0;
    	 			 	
    	 			 	 }
    	 			 	break;
    	 			 	case NC_CLEAN_BASEINFO:
    	 			 	 	lCount=0;
    	 			 	 sprintf(caSql,"select count(*) from ncbackuplog where backtype='ncbase' and sdate>='%s'  ",ncMailCvtDate998(caEnd));
    	 			 	 pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
    	 			 	 if(lCount==0){
    	 			 	 	   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm","因该段时间的基础数据日志没有被备份,所以不能删除","因该段时间的基础数据日志没有被备份，所以不能删除",ncUtlGetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN));
                   return 0;
    	 			 	
    	 			 	 }
    	 			 	break;
    	 			 	   case NC_CLEAN_ADMLOG:
    	 			 	 	lCount=0;
    	 			 	 sprintf(caSql,"select count(*) from ncbackuplog where backtype='admlog' and sdate>='%s'  ",ncMailCvtDate998(caEnd));
    	 			 	 pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
    	 			 	 if(lCount==0){
    	 			 	 	   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm","因该段时间的系统访问日志没有被备份,所以不能删除","因该段时间的系统访问日志没有被备份，所以不能删除",ncUtlGetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN));
                   return 0;
    	 			 	
    	 			 	 }
    	 			 	break;
    	 			 	case NC_CLEAN_DBLOG:
    	 			 	 	lCount=0;
    	 			 	 sprintf(caSql,"select count(*) from ncbackuplog where backtype='dblog' and sdate>='%s'  ",ncMailCvtDate998(caEnd));
    	 		//	 	 printf("caSql=%s\n",caSql);
    	 			 	 pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
    	 			 	 if(lCount==0){
    	 			 	 	   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm","因该段时间的数据库监控日志没有被备份,所以不能删除","因该段时间的数据库监控日志没有被备份，所以不能删除",ncUtlGetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN));
                   return 0;
    	 			 	
    	 			 	 }
    	 			 	break;
    	 			 	
    	 			 	
    	 			}
    	 	}
    	 }
   */ 	
        ncUtlDoCleanData(psShmHead,caDo,caStart,caEnd);
        for(i=0;i<64;i++){
        	if(caDo[i]==1){
        		switch(i){
        			case NC_CLEAN_ALL:
        				sprintf(caTemp,"删除%s-%s全部日志",caStart,caEnd);
        				i=64;
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_INSTANTDATA:
        				sprintf(caTemp,"删除%s-%s即时通讯日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		  case NC_CLEAN_INSTANTBACK:
        				sprintf(caTemp,"删除%s-%s即时通讯备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_FORMMON:
        				sprintf(caTemp,"删除%s-%s表单日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_FORMMONBACK:
        				sprintf(caTemp,"删除%s-%s表单备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_MAILMON:
        				sprintf(caTemp,"删除%s-%s邮件日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_MAILMONBACK:
        				sprintf(caTemp,"删除%s-%s邮件日志备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_MAILCTL:
        				sprintf(caTemp,"删除%s-%s邮件拦截日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_MAILCTLBACK:
        				sprintf(caTemp,"删除%s-%s邮件拦截备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_IPLOG:
        				sprintf(caTemp,"删除%s-%sIP连接日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_IPLOGBACK:
        				sprintf(caTemp,"删除%s-%sIP连接备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_HTTPLOG:
        				sprintf(caTemp,"删除%s-%s网站访问日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_HTTPLOGBACK:
        				sprintf(caTemp,"删除%s-%s网站访问备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_ERRLOG:
        				sprintf(caTemp,"删除%s-%s阻断日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_ERRLOGBACK:
        				sprintf(caTemp,"删除%s-%s阻断备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_ADMLOG:
        				sprintf(caTemp,"删除%s-%s系统访问日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_ADMLOGBACK:
        				sprintf(caTemp,"删除%s-%s系统访问日志备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_RUNLOG:
        				sprintf(caTemp,"删除%s-%s系统运行日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_WARNLOG:
        				sprintf(caTemp,"删除%s-%s告警日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			case NC_CLEAN_RUNLOGBACK:
        				sprintf(caTemp,"删除%s-%s系统运行日志备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		  case NC_CLEAN_WARNLOGBACK:
        				sprintf(caTemp,"删除%s-%s告警日志备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		 case NC_CLEAN_BASEINFO:
        				sprintf(caTemp,"删除%s-%s基础数据备份",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		case NC_CLEAN_COUNT:
        				sprintf(caTemp,"删除%s-%s统计数据",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		case NC_CLEAN_OTHERLOG:
        				sprintf(caTemp,"删除%s-%s其它数据",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		case NC_CLEAN_ACCESSRULE:
        				sprintf(caTemp,"删除%s-%s控制规则",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		case NC_CLEAN_USERINFO:
        				sprintf(caTemp,"删除%s-%s上网用户信息",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		case NC_CLEAN_NETWORKSET:
        				sprintf(caTemp,"删除%s-%s网络设置参数",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		case NC_CLEAN_NETLOG:
        				sprintf(caTemp,"删除%s-%s上下线日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		case NC_CLEAN_SESLOG:
        				sprintf(caTemp,"删除%s-%s网络会话日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        		case NC_CLEAN_DBLOG:
        				sprintf(caTemp,"删除%s-%s数据库监控日志",caStart,caEnd);        			
        				ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        				break;
        			}
        		}
        }
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0209调试软件"),ncLang("0674系统正在后台处理您的请求，请稍后...."));
    }
    else {
        ncUtlUnLock(psShmHead,NC_LOCK_AUTOCLEAN);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0209调试软件"),ncLang("0380没有选择要整理的内容"));
    }        
    return 0;
}    

/* 整理数据  */
int ncUtlDoCleanData(utShmHead *psShmHead,char *pDo,char *pStart,char *pEnd)
{
    int iPid;
    int i,iReturn;
    iPid = fork();
    if(iPid > 0) {
        return iPid;
    }
    else if(iPid == (-1)) {
        ncUtlUnLock(psShmHead,NC_LOCK_AUTOCLEAN);
        return (-1);
    }
    iReturn = pasConnect(psShmHead);
    if(iReturn < 0) {
        ncSysLog(NC_LOG_ERROR,"AutoClean, Database connect error");
        return 0;
    }
    ncSysLog(NC_LOG_EVENT," Start Process to do Clean Pid %d ",getpid());
    for(i=0;i<64;i++) {
        if(pDo[i] == 1) {
    	
            switch(i) {
                case NC_CLEAN_ALL:
                     ncUtlUnLock(psShmHead,NC_LOCK_AUTOCLEAN);
                     pasDbClose(NULL);
                     exit(0);
                case NC_CLEAN_INSTANTDATA:  /* 即时通信内容  */
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0479删除即时通信内容"));
                     ncUtlClean_DelTables("nccontlog",pStart,pEnd,"stime",2);
                     ncUtlClean_DelTables("ncimclient",pStart,pEnd,"stime",2);
                     break;
                case NC_CLEAN_INSTANTBACK:  /* 即时通信备份  */
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0478删除即时通信备份"));
                     ncUtlClean_Backup(psShmHead,"instant","instant/*",pStart,pEnd);
                     break;
                case NC_CLEAN_FORMMON:  
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0470删除表单监控内容"));
                     ncUtlClean_Mail(psShmHead,"frm",pStart,pEnd);
                     ncUtlClean_DelTables("ncformmoni",pStart,pEnd,"stime",2);
                     ncUtlClean_DelTables("ncbbsmon",pStart,pEnd,"vtime",2);
                     
                     break;
                case NC_CLEAN_FORMMONBACK:  
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0469删除表单监控备份内容"));
                     ncUtlClean_Backup(psShmHead,"formmon","formmon*",pStart,pEnd);
                     break;
                case NC_CLEAN_MAILMON:  
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0495删除邮件监控内容"));
                     ncUtlClean_Mail(psShmHead,"mon",pStart,pEnd);
                     ncUtlClean_DelTables("ncmailmoni",pStart,pEnd,"stime",2);
                     break;
                case NC_CLEAN_MAILMONBACK:  
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0493删除邮件监控备份内容"));

                     ncUtlClean_Backup(psShmHead,"mailmon","mailmon*",pStart,pEnd);
                     break;
                case NC_CLEAN_MAILCTL:  
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0498删除邮件拦截内容"));
                     ncUtlClean_Mail(psShmHead,"ctl",pStart,pEnd);
                     ncUtlClean_DelTables("ncmailctli",pStart,pEnd,"stime",2);
                     break;
                case NC_CLEAN_MAILCTLBACK:  
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0496删除邮件拦截备份内容"));
                     ncUtlClean_Backup(psShmHead,"mailctl","mailctl*",pStart,pEnd);
                     break;

                case NC_CLEAN_IPLOG:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0467删除IPLOG"));
                     ncUtlClean_DelTables("nciplog",pStart,pEnd,"vtime",2);
                     break;
                case NC_CLEAN_IPLOGBACK:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0468删除IPLOG备份"));
                     ncUtlClean_Backup(psShmHead,"iplog","iplog/*",pStart,pEnd);
                     break;
                case NC_CLEAN_HTTPLOG:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0482删除网站访问纪录"));
                     ncUtlClean_DelTables("nchttplog",pStart,pEnd,"vtime",2);
                     break;
                case NC_CLEAN_HTTPLOGBACK: 
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0483删除网站访问纪录备份"));
                     ncUtlClean_Backup(psShmHead,"httplog","httplog/*",pStart,pEnd);
                     break;
                case NC_CLEAN_ERRLOG:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0499删除阻断信息"));
                     ncUtlClean_DelTables("ncerrlog",pStart,pEnd,"stime",2);
                     break;
                case NC_CLEAN_ERRLOGBACK: 
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0500删除阻断信息备份"));
                     ncUtlClean_Backup(psShmHead,"errlog","errlog/*",pStart,pEnd);
                     break;
                case NC_CLEAN_ADMLOG:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0484删除系统访问信息"));
                     ncUtlClean_DelTables("ncadminlog",pStart,pEnd,"sdate",1);
                     break;
                case NC_CLEAN_ADMLOGBACK: 
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0485删除系统访问信息备份"));
                     ncUtlClean_Backup(psShmHead,"admlog","admlog/*",pStart,pEnd);
                     break;
                case NC_CLEAN_RUNLOG:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0487删除系统运行日志"));
                     ncUtlClean_DelTables("ncsyslog",pStart,pEnd,"stime",1);
                     break;
                case NC_CLEAN_RUNLOGBACK: 
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0488删除系统运行日志备份"));
                     ncUtlClean_Backup(psShmHead,"runlog","runlog/*",pStart,pEnd);
                     break;
                case NC_CLEAN_WARNLOG:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0474删除告警信息"));
                     ncUtlClean_DelTables("ncwarnlog",pStart,pEnd,"stime",2);
                     break;
                case NC_CLEAN_WARNLOGBACK: 
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0475删除告警信息备份"));
                     ncUtlClean_Backup(psShmHead,"warnlog","warnlog/*",pStart,pEnd);
                     break;
                case NC_CLEAN_BASEINFO: 
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0477删除基础数据备份"));
                     ncUtlClean_Backup(psShmHead,"ncbase","ncbase/*",pStart,pEnd);
                     break;
                case NC_CLEAN_COUNT: 
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0476删除各种统计数据"));
                     ncUtlClean_DelTables("ncipcount",pStart,pEnd,"sdate",0);
                     ncUtlClean_DelTables("ncipservice",pStart,pEnd,"sdate",0);
                     ncUtlClean_DelTables("nchttpcount",pStart,pEnd,"sdate",0);
                     break;
                case NC_CLEAN_OTHERLOG:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0480删除其它日志信息"));
                     ncUtlClean_DelTables("ncnetlog",pStart,pEnd,"sdate",0);
                     ncUtlClean_DelTables("ncflowlog",pStart,pEnd,"sdate",0);
                     ncUtlClean_DelTables("ncupdatelog",pStart,pEnd,"starttime",2);
                     ncUtlClean_DelTables("nciclog",pStart,pEnd,"sdate",0);
                     ncUtlClean_DelTables("ncipflow",pStart,pEnd,"sdate",0);
                     ncUtlClean_DelTables("billlog",pStart,pEnd,"starttime",2);
                     ncUtlClean_DelTables("billaccount",pStart,pEnd,"stime",2);
                     ncUtlClean_DelTables("ncsearchlog",pStart,pEnd,"stime",2);
                     
                     break;
                case NC_CLEAN_ACCESSRULE:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0473删除访问控制信息"));
                     ncUtlClean_DelTables("ncippol","\0","\0","\0",0);
                     ncUtlClean_DelTables("ncippoltype","\0","\0","\0",0);
                     ncUtlClean_DelTables("ncwebpol","\0","\0","\0",0);
                     ncUtlClean_DelTables("ncwebpolclass","\0","\0","\0",0);
                     ncUtlClean_DelTables("ncprocy","\0","\0","\0",0);
                     ncUtlClean_DelTables("ncmailfilter","\0","\0","\0",0);
                     ncUtlClean_DelTables("nclimweb","\0","\0","\0",0);
                     ncUtlClean_DelTables("nclimsumm","\0","\0","\0",0);
                     ncUtlClean_DelTables("nclimip","\0","\0","\0",0);
                     ncUtlClean_DelTables("nclimdatedes","\0","\0","\0",0);
                     ncUtlClean_DelTables("nclimdate","\0","\0","\0",0);
                     break;
               case  NC_CLEAN_USERINFO:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0491删除用户信息"));
                     iReturn = pasDbExecSqlF("delete from ncuser where userid > 10");
                     iReturn = pasDbExecSqlF("delete from ncgroup where groupid > 0");
                     iReturn = pasDbExecSqlF("delete from ncusermail");
                     pasDbCommit(NULL);
                     ncSysLog(NC_LOG_EVENT," Tables ncuser,ncgroup,ncusermail Clean ");
                     break;
               case  NC_CLEAN_TCSET:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0472删除带宽管理参数"));
                     ncUtlClean_DelTables("nctcuser","\0","\0","\0",0);
                     ncUtlClean_DelTables("nctcip","\0","\0","\0",0);
                     ncUtlClean_DelTables("nctcclass","\0","\0","\0",0);
                     break;
               case NC_CLEAN_MAILCTLSET:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0497删除邮件拦截参数设置"));
                     ncUtlClean_DelTables("mailctlfl","\0","\0","\0",0);
                     ncUtlClean_DelTables("mailctlrule","\0","\0","\0",0);
                     ncUtlClean_DelTables("mailctltype","\0","\0","\0",0);
                     ncUtlClean_DelTables("ncmailfilter","\0","\0","\0",0);
                     break;
               case NC_CLEAN_MAILMONSET:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0494删除邮件监控参数设置"));
                     ncUtlClean_DelTables("mailfilter1","\0","\0","\0",0);
//                     ncUtlClean_DelTables("mailmonrule","\0","\0","\0",0);
//                     ncUtlClean_DelTables("mailmontype","\0","\0","\0",0);

                     iReturn = pasDbExecSqlF("delete from mailmontype where tid!=1");
                     iReturn = pasDbExecSqlF("delete from mailmonrule where tid!=1");
                     pasDbCommit(NULL);

                     break;
               case NC_CLEAN_FORMMONSET:
//                     ncUtlClean_DelTables("mailfrmrule","\0","\0","\0",0);
//                     ncUtlClean_DelTables("mailfrmtype","\0","\0","\0",0);
                     
                     iReturn = pasDbExecSqlF("delete from mailfrmtype where tid!=1");
                     iReturn = pasDbExecSqlF("delete from mailfrmrule where tid!=1");
                     pasDbCommit(NULL);
                     break;
               case NC_CLEAN_NETWORKSET:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0481删除网络设置参数"));
                     ncUtlClean_DelTables("ncnetwork","\0","\0","\0",0);
                     break;
                     
               case NC_CLEAN_USERURL:
     //                ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0492删除用户自定义网址库"));
     //                iReturn = pasDbExecSqlF("delete from ncwebclass  where flags = 1");
     //                iReturn = pasDbExecSqlF("delete from ncweblist   where flags = 1");
     //                pasDbCommit(NULL);
     //                ncSysLog(NC_LOG_EVENT," Tables url database  Clean ");
                     break;
               case NC_CLEAN_NETLOG:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0486删除上网日志"));
                     ncUtlClean_DelTables("ncuserlog",pStart,pEnd,"stime",2);
                     pasDbCommit(NULL);
                     ncSysLog(NC_LOG_EVENT," Tables system database  Clean ");
                     break;
               case NC_CLEAN_SESLOG:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,ncLang("0486删除网络会话日志"));
										 ncUtlClean_DelTables("ncnetseslog",pStart,pEnd,"stime",2);
                     pasDbCommit(NULL);
                     ncSysLog(NC_LOG_EVENT," Tables system database  Clean ");
                     break;
               case NC_CLEAN_DBLOG:
                     ncUtlSetLockMsg(psShmHead,NC_LOCK_AUTOCLEAN,"删除数据库就爱你快日志");
										 ncUtlClean_DelTables("ncdbseslog",pStart,pEnd,"stime",2);
                     pasDbCommit(NULL);
                     ncSysLog(NC_LOG_EVENT," Tables dblog database  Clean ");
                     break;
            }
        }
    }
    ncUtlUnLock(psShmHead,NC_LOCK_AUTOCLEAN);
    if(ncUtlIsLock(psShmHead,NC_LOCK_SYSREFRESH,5)==0) {
        ncInitProeimA(psShmHead,1);
//        ncUtlSendProcessControl(psShmHead,"ALL",NC_RELOAD_ALL,1);
        ncUtlUnLock(psShmHead,NC_LOCK_SYSREFRESH);
        ncSysLog(NC_LOG_EVENT, "System Reset");
    }
    pasDbClose(NULL);
    exit(0);
}

/* flags == 0--字符格式1 YYYY/MM/DD  1--字符格式 YYYY-MM-DD  2--数字   
        输入的日期格式为: YYYY/MM/DD
 */
int ncUtlClean_DelTables(char *pTable,char *pStart,char *pEnd,char *pField,int flags)
{
    int iReturn;
    if(utStrIsSpaces(pStart) && utStrIsSpaces(pEnd)) {
        iReturn = pasDbExecSqlF("delete from %s",pTable);
#ifdef LDEBUG
        printf("delete from %s",pTable);
#endif

        if(iReturn != 0) {
            ncSysLog(NC_LOG_ERROR," Tables %s delete error %s",pTable,pasDbErrorMsg(NULL));
            pasDbRollback(NULL);
            return (-1);
        }
        else {
            ncSysLog(NC_LOG_EVENT," Tables %s delete ",pTable);
            pasDbCommit(NULL);
            return 0;
        }
    }
    else {
        unsigned long lStime,lEtime;
        char caSdate[20],caEdate[20];
        lStime = 0;
        if(!utStrIsSpaces(pStart)) {
            lStime = utTimStrToLong("%Y/%m/%d",pStart);
        }
        lEtime = time(0);
        if(!utStrIsSpaces(pEnd)) {
            lEtime = utTimStrToLong("%Y/%m/%d",pEnd)+3600*24;
        }
        if(flags == 0) {
            strcpy(caSdate,utTimFormat("%Y/%m/%d %H:%M:%S",lStime));
            strcpy(caEdate,utTimFormat("%Y/%m/%d %H:%M:%S",lEtime));
            iReturn = pasDbExecSqlF("delete from %s where %s >= '%s' and %s <= '%s'",
                         pTable,pField,caSdate,pField,caEdate);
#ifdef LDEBUG
            printf("delete from %s where %s >= '%s' and %s <= '%s'",
                         pTable,pField,caSdate,pField,caEdate);
#endif
        }
        else if(flags == 1) {
            strcpy(caSdate,utTimFormat("%Y-%m-%d %H:%M:%S",lStime));
            strcpy(caEdate,utTimFormat("%Y-%m-%d %H:%M:%S",lEtime));
            iReturn = pasDbExecSqlF("delete from %s where %s >= '%s' and %s <= '%s'",
                         pTable,pField,caSdate,pField,caEdate);
#ifdef LDEBUG
            printf("delete from %s where %s >= '%s' and %s <= '%s'",
                         pTable,pField,caSdate,pField,caEdate);
#endif
        }
        else if(flags == 2) {
            iReturn = pasDbExecSqlF("delete from %s where %s >= %lu and %s <= %lu",
                         pTable,pField,lStime,pField,lEtime);
#ifdef LDEBUG
            printf("delete from %s where %s >= %lu and %s <= %lu",
                         pTable,pField,lStime,pField,lEtime);
#endif
        }
        if(iReturn != 0) {
            ncSysLog(NC_LOG_ERROR," Tables %s delete error %s",pTable,pasDbErrorMsg(NULL));
            pasDbRollback(NULL);
            return (-1);
        }
        else {
            ncSysLog(NC_LOG_EVENT," Tables %s delete ",pTable);
            pasDbCommit(NULL);
            return 0;
        }
    }
    return 0;                 
}

int ncUtlClean_Backup(utShmHead *psShmHead,char *pType,char *pFile,char *pStart,char *pEnd)
{
    int iReturn;
    char caCommand[256];
    pasDbCursor *psCur;
    if(utStrIsSpaces(pStart) && utStrIsSpaces(pEnd)) {
        iReturn = pasDbExecSqlF("delete from ncbackuplog where backtype = '%s'",pType);
#ifdef LDEBUG
        printf("delete from ncbackuplog where backtype = '%s'",pType);
#endif        
        if(iReturn != 0) {
            ncSysLog(NC_LOG_ERROR," Delete backup error %s ",pType);
            pasDbRollback(NULL);
            return (-1);
        }
        else {
            sprintf(caCommand,"rm -Rf %s/%s",utComGetVar_sd(psShmHead,"BackupPath","/home/ncmysql/nc/back"),pFile);
            system(caCommand);
#ifdef LDEBUG
            printf(" caCommand:[%s]\n",caCommand);
#endif        
            ncSysLog(NC_LOG_EVENT," Backup %s deleted",pType);
            pasDbCommit(NULL);
        }
        return 0;
    }
    else {
        unsigned long lStime,lEtime;
        char caSdate[20],caEdate[20];
        char caFile[128],caFilename[64],caPath[64];
        lStime = 0;
        if(!utStrIsSpaces(pStart)) {
            lStime = utTimStrToLong("%Y/%m/%d",pStart);
        }
        lEtime = time(0);
        if(!utStrIsSpaces(pEnd)) {
            lEtime = utTimStrToLong("%Y/%m/%d",pEnd);
        }
        strcpy(caSdate,utTimFormat("%Y-%m-%d",lStime));
        strcpy(caEdate,utTimFormat("%Y-%m-%d",lEtime));
        psCur = pasDbOpenSqlF("select bakpath,filename from ncbackuplog where sdate >= '%s' and sdate <= '%s' and  backtype = '%s'",
               caSdate,caEdate,pType);
#ifdef LDEBUG
        printf("select bakpath,filename from ncbackuplog where sdate >= '%s' and sdate <= '%s' and  backtype = '%s'",
               caSdate,caEdate,pType);
#endif   
        if(psCur == NULL) {
            ncSysLog(NC_LOG_ERROR,"Select ncbackuplog error %s",pasDbErrorMsg(NULL));
            return (-1);
        }
        iReturn =  pasDbFetchInto(psCur,UT_TYPE_STRING,63,caPath,
                                        UT_TYPE_STRING,63,caFilename);
        while(iReturn == 0) {
            sprintf(caFile,"%s/%s",caPath,caFilename);
            remove(caFile);
            iReturn =  pasDbFetchInto(psCur,UT_TYPE_STRING,63,caPath,
                                        UT_TYPE_STRING,63,caFilename);
        }
        pasDbCloseCursor(psCur);
        iReturn = pasDbExecSqlF("delete from ncbackuplog where sdate >= '%s' and sdate <= '%s' and  backtype = '%s'",caSdate,caEdate,pType);
#ifdef LDEBUG
        printf("delete from ncbackuplog where sdate >= '%s' and sdate <= '%s' and  backtype = '%s'",caSdate,caEdate,pType);
#endif   
        if(iReturn == 0) {
            ncSysLog(NC_LOG_EVENT,"delete ncbackuplog %s==>%s OK",caSdate,caEdate);
        }
        else {
            ncSysLog(NC_LOG_ERROR,"delete ncbackuplog %s==>%s Error %s ",caSdate,caEdate,pasDbErrorMsg(NULL));
        }
        return 0;
    }
}

/*  删除邮件信息
       pType--ctl  mon frm
 */
int ncUtlClean_Mail(utShmHead *psShmHead,char *pType,char *pStart,char *pEnd)
{
    int iReturn;
    pasDbCursor *psCur;
    char caTables[32];
    char caCommand[256];
    if(strcmp(pType,"mon")==0) {
        strcpy(caTables,"ncmailmon");
    }
    else if(strcmp(pType,"frm")==0) {
        strcpy(caTables,"ncformmon");
    }
    else {
        strcpy(caTables,"ncmailctl");
    }
    if(utStrIsSpaces(pStart) && utStrIsSpaces(pEnd)) { /* 没有时间限制的特殊处理  */
        iReturn = pasDbExecSqlF("delete from %s",caTables);
#ifdef LDEBUG
        printf("delete from %s\n",caTables);
#endif   
 
        if(iReturn != 0) {
            ncSysLog(NC_LOG_ERROR," Delete backup error %s %s",pType,pasDbErrorMsg(NULL));
            pasDbRollback(NULL);
            return (-1);
        }
        else {
            sprintf(caCommand,"%s/%s",utComGetVar_sd(psShmHead,"maildir","/home/ncmysql/nc/maillog"),pType);
            utFilDeleteDir(caCommand,0,time(0)+1);
#ifdef LDEBUG
            printf(" caCommand:[%s]\n",caCommand);
#endif
            ncSysLog(NC_LOG_EVENT," %s deleted",pType);
            pasDbCommit(NULL);
        }
        return 0;
    }
    else {
        unsigned long lStime,lEtime;
        char caSdate[20],caEdate[20];
        char caFile[128],caFilename[64];
        lStime = 0;
        if(!utStrIsSpaces(pStart)) {
            lStime = utTimStrToLong("%Y/%m/%d",pStart);
        }
        lEtime = time(0);
        if(!utStrIsSpaces(pEnd)) {
            lEtime = utTimStrToLong("%Y/%m/%d",pEnd);
        }
        strcpy(caSdate,utTimFormat("%Y-%m-%d",lStime));
        strcpy(caEdate,utTimFormat("%Y-%m-%d",lEtime));
        if(strcmp(pType,"ctl")==0) {
            psCur = pasDbOpenSqlF("select mailfile from ncmailctl where sdate >= %lu and sdate <= %lu",
                             lStime,lEtime);
#ifdef LDEBUG
        printf("select mailfile from ncmailctl where sdate >= %lu and sdate <= %lu\n",
                             lStime,lEtime);
#endif   
 
        }
        else if(strcmp(pType,"mon")==0) { 
            psCur = pasDbOpenSqlF("select mailfile from ncmailmon where sdate >= '%s' and sdate <= '%s'",
                             caSdate,caEdate);
#ifdef LDEBUG
        printf("select mailfile from ncmailmon where sdate >= '%s' and sdate <= '%s'\n",
                             caSdate,caEdate);
#endif  
        }
        else {
            psCur = pasDbOpenSqlF("select upfile from ncformmon where sdate >= '%s' and sdate <= '%s'",
                             caSdate,caEdate);
#ifdef LDEBUG
        printf("select upfile from ncformmon where sdate >= '%s' and sdate <= '%s'\n",
                             caSdate,caEdate);
#endif  
        }
        if(psCur == NULL) {
            ncSysLog(NC_LOG_ERROR,"Cursor Error %s",pasDbErrorMsg(NULL));
            return (-1);
        }
        iReturn =  pasDbFetchInto(psCur,UT_TYPE_STRING,63,caFilename);
        while(iReturn == 0) {
            sprintf(caFile,"%s/%s/%s",utComGetVar_sd(psShmHead,"maildir","/home/ncmysql/nc/maillog"),
                                       pType,caFilename);
            remove(caFile);
            iReturn =  pasDbFetchInto(psCur,UT_TYPE_STRING,63,caFilename);
        }
        pasDbCloseCursor(psCur);
        if(strcmp(pType,"ctl")==0) {
            iReturn = pasDbExecSqlF("delete from ncmailctl where sdate >= %lu and sdate <= %lu",
                             lStime,lEtime);
#ifdef LDEBUG
        printf("delete from ncmailctl where sdate >= %lu and sdate <= %lu\n",
                             lStime,lEtime);
#endif  
        }
        else if(strcmp(pType,"mon")==0) { 
            iReturn = pasDbExecSqlF("delete from ncmailmon where sdate >= '%s' and sdate <= '%s'",
                             caSdate,caEdate);
#ifdef LDEBUG
        printf("delete from ncmailmon where sdate >= '%s' and sdate <= '%s'\n",
                             caSdate,caEdate);
#endif  
        }
        else {
            iReturn = pasDbExecSqlF("delete from ncformmon where sdate >= '%s' and sdate <= '%s'",
                             caSdate,caEdate);
#ifdef LDEBUG
        printf("delete from ncformmon where sdate >= '%s' and sdate <= '%s'\n",
                             caSdate,caEdate);
#endif  
        }
        if(iReturn == 0) {
            pasDbCommit(NULL);
        }
        else {
            pasDbRollback(NULL);
        }
    }
    return 0;
}


/* 退出系统*/
int ncWebNcExit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{  
    int iNum,iReturn;
    unsigned long lGroupid0;
  
    utPltDbHead *psDbHead;
    char caUsername2[32];
    char caTemp[1024];
   
    memset(caUsername2,0,sizeof(caUsername2));
    dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
    
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
   
    psDbHead = utPltInitDb();
    utMsgPrintMsg(psMsgHead);

    strcpy(caTemp,"退出系统");     
 //   printf("caTemp=%s\n",caTemp);			
    ncsWriteSysLog(psMsgHead,caUsername2,"01","success",caTemp);
   utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");  		
        			
    return 0;
}
