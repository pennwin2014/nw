/* 监控软件---访问限制的Web应用
                     Modified 2003/03/01
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
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
#include "ncdef.h"
#include "nclimit.h"

extern int iDebugLevel;
char *ncLimGetDateName(long lDateid);
char *ncLimCvtTime(long lStime);
 char *ncLimGetIplistName(long lId);
 char *ncLimGetServices(long lServices);
char *ncLimGetAppName(long lGtype,long lId);
char *ncLimGetProt(long lTran1);
static char *ncLimGetWeblistName(long lId);

int ncWebLim_Dispmenu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caPlate[128],caGtype[16],caId[16];
    int   iReturn;
    long lGtype,lId;
    long lGroupid0;
    char caAppname[32];
    utPltDbHead *psDbHead;
    
 
    
	iReturn = utMsgGetSomeNVar(psMsgHead,4,
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All 4-Polcy */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
	                "plate",    UT_TYPE_STRING,127,caPlate,
	                "appname",  UT_TYPE_STRING,31,caAppname);
	                
	        iReturn=dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if((iReturn==0)&&(lGroupid0>0)){
    	if(atol(caGtype)==3){
    		strcpy(caGtype,"2");
    		sprintf(caId,"%d",lGroupid0);
    	}
   }          
	                
	                
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"id",caId);
    utPltPutVar(psDbHead,"gtype",caGtype);
    lGtype = atol(caGtype);
    lId = atol(caId);
    if(strlen(caAppname)>0)
    utPltPutVar(psDbHead,"appname",caAppname);
    else
    utPltPutVar(psDbHead,"appname",ncLimGetAppName(lGtype,lId));
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}


/* 显示访问控制信息  */
int ncWebLim_Listout(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caMessage[556],caMessage1[556];
    char caMod[16],caDel[16],caAdd[16],caApp[16],caCurPg[16];
    char caSum[16],caGtype[16],caFlags[16],caId[16],caRnum[16],caService[16];
    char caSid[16],caLimctl[16],caDateid[16],caEtime[16],caStime[16];
    char caUrlid[24];
    char caLip[16],caUip[16],caLport[16],caUport[16],caProt[12],caListid[16],caUrl[64];
    char *pBuf,*pSelect,*p;
    unsigned long lLip;
    long  lStart,lStartRec,lCurPg,lTotPg;
    long  lRnum,lSum,lGtype,lFlags,lId,lDateid,lLimctl,lStime,lEtime,lSid,lService;
    long  lLport,lUport,lProt,lCount,lCount1,lRowNum;
    int   iReturn,i;
    long lQosid,lQoslevel,iNum;
    char caQosname[32],caTemp[1024];
    char caWarntype[16];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,25,
	                "sum",     UT_TYPE_STRING, 12, caSum,    /* 记录总数                     */
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All 4-Polcy */
	                "flags",   UT_TYPE_STRING, 12, caFlags,   /* 1--Internet 2--Services ..   */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
                    "rnum",    UT_TYPE_STRING, 12, caRnum,
	                "add",     UT_TYPE_STRING, 12, caAdd,
	                "mod",     UT_TYPE_STRING, 12, caMod,
	                "del",     UT_TYPE_STRING, 12, caDel,
	                "sid",     UT_TYPE_STRING, 12, caSid,
	                "limctl",  UT_TYPE_STRING, 12, caLimctl,
	                "dateid",     UT_TYPE_STRING, 12, caDateid,
	                "etime",      UT_TYPE_STRING, 8,  caEtime,
	                "stime",      UT_TYPE_STRING, 8,  caStime,
	                "ser1",      UT_TYPE_STRING, 12, caService,
	                "sip",       UT_TYPE_STRING, 15, caLip,
	                "eip",       UT_TYPE_STRING, 15, caUip,
	                "lport",     UT_TYPE_STRING, 5,  caLport,
	                "uport",     UT_TYPE_STRING, 5,  caUport,
	                "prot",      UT_TYPE_STRING, 2,  caProt,
	                "listid",    UT_TYPE_STRING, 12, caListid,
	                "app",       UT_TYPE_STRING,12,caApp,
	                "urlname",   UT_TYPE_STRING, 63, caUrl,
	                "CurPg",     UT_TYPE_STRING, 12, caCurPg,
	                "urlid",     UT_TYPE_STRING,12,caUrlid,
	                "warntype",  UT_TYPE_STRING,10,caWarntype);
	lCurPg = atol(caCurPg);
	if(lCurPg == 0) lCurPg = 1;
	lRowNum = 24;
	lStart = (lCurPg - 1) * lRowNum;          
	lRnum = atol(caRnum);                
    lSum = atol(caSum);
    lGtype = atol(caGtype);
    lFlags = atol(caFlags);
    lId = atol(caId);
    lDateid = atol(caDateid);
    lLimctl = atol(caLimctl);
    lService = atol(caService);
    lLport = atol(caLport);
    lUport = atol(caUport);
    lProt = atol(caProt);
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
        if(lFlags == NC_LIMIT_IPADDRESS) {
            if(utStrIsSpaces(caLip) || !utStrIsValidIp(caLip)) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0433起始IP地址为空或格式不对,正确的格式应为 xxx.xxx.xxx.xxx"));
                return 0;
            }
            if(utStrIsSpaces(caUip)) {
                strcpy(caUip,caLip);
            }
            if(!utStrIsValidIp(caUip)) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0864终止IP地址格式不对,正确的格式应为 xxx.xxx.xxx.xxx"));
                return 0;
            }
            if(lLport < 0 || lLport > 65535L) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0434起始端口号不对，应为不超过65535的数字"));
                return 0;
            }
            if(utStrIsSpaces(caUport)) {
                lUport = lLport;
            }
            else if(lUport < 0 || lUport > 65535L) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0865终止端口号不对，应为不超过65535的数字"));
                return 0;
            }
            if(lProt != 0 && lProt != NC_PROTOCOL_TCP && lProt != NC_PROTOCOL_UDP) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0687协议类型不对"));
                return 0;
            }
       }
       else if(lFlags == NC_LIMIT_IPLIST){
            strcpy(caLip,caListid);
       }
       else if(lFlags == NC_LIMIT_WEBLIST){
            strcpy(caUrl,caListid);
       }
       else if(lFlags == NC_LIMIT_WEBHOST) {
            if(utStrIsSpaces(caUrl)) {
               utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0621网址不能为空"));
               return 0;
            }
       }
       lStime = ncUtlIsValidTime(caStime);
       lEtime = ncUtlIsValidTime(caEtime);
       if(lStime < 0 || lEtime < 0) {
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0435起始或终止时间不正确"));
           return 0;
       }
       
       switch(lFlags) {
            case  NC_LIMIT_IPLIST:
            case  NC_LIMIT_IPADDRESS:  
            case  NC_LIMIT_INTERNET:   /* 禁止访问Internet */
            case NC_LIMIT_SERVICE:
                if(lFlags == NC_LIMIT_IPLIST) {
                    lLip = atol(caLip);
                }
                else {
                    lLip = ntohl(pasIpcvtLong(caLip));
                }         
                lSid = pasGetSid("nclimip","sid");
                sprintf(caMessage,"insert into nclimip(sid,gtype,id,num,lowerip,upperip,trantype,service,lowerport,upperport,timetype,starttime,stoptime,conflags,flags,warntype) values (%lu,%d,%lu,%d,'%s','%s',%d,%d,%d,%d,%lu,'%s','%s',%d,%d,%d)",
                     lSid,lGtype,lId,lRnum,caLip,caUip,lProt,lService,lLport,lUport,lDateid,caStime,caEtime,lFlags,lLimctl,atol(caWarntype));  
                sprintf(caMessage1,"%s:[%s]",ncLang("1153增加上网控制规则"),caMessage);      
                                       
                utMsgDelVar(psMsgHead,"opmsg");
                utMsgAdd_s(&psMsgHead,"opmsg",caMessage1);
                utMsgDelVar(psMsgHead,"action");
                utMsgAdd_s(&psMsgHead,"action","04");
                ncOperWriteLog(psMsgHead);
                iReturn = pasDbExecSqlF("insert into nclimip(sid,gtype,id,num,lowerip,upperip,\
                     trantype,service,lowerport,upperport,timetype,starttime,stoptime,conflags,\
                     flags,warntype) values \
                    (%lu,%d,%lu,%d,%lu,%lu,%d,%d,%d,%d,%lu,%lu,%lu,%d,%d,%d)",
                     lSid,lGtype,lId,lRnum,lLip,ntohl(pasIpcvtLong(caUip)),lProt,lService,lLport,lUport,lDateid,lStime,lEtime,lFlags,lLimctl,atol(caWarntype));
                if(iReturn != 0) {
                     sprintf(caMessage,"insert into nclimip(sid,gtype,id,num,lowerip,upperip,\
                     trantype,service,lowerport,upperport,timetype,starttime,stoptime,conflags,\
                     flags) values \
                    (0,%d,%lu,%d,'%s','%s',%d,%d,%d,%d,%lu,%lu,%lu,%d,%d)",
                     lGtype,lId,lRnum,"\0","\0",0,0,0,0,lDateid,lStime,lEtime,lFlags,lLimctl);
                    printf(" Sql=%s\n",caMessage);
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0314记录增加出错, 可能为记录重复"));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                    pasDbCommit(NULL);
                    break;
                }
            case  NC_LIMIT_WEBHOST:
            case  NC_LIMIT_WEBLIST:
            	    lSid = pasGetSid("nclimweb","sid");
            	  sprintf(caMessage,"insert into nclimweb(sid,gtype,id,num,url,timetype,starttime,stoptime,conflags,flags,warntype) values \
                    (%lu,%d,%lu,%d,'%s',%lu,%lu,%lu,%d,%d,%d)",
                     lSid,lGtype,lId,lRnum,caUrl,lDateid,lStime,lEtime,lFlags,lLimctl,atol(caWarntype));
                sprintf(caMessage1,"%s:[%s]",ncLang("1153增加上网控制规则"),caMessage);      
                                       
                utMsgDelVar(psMsgHead,"opmsg");
                utMsgAdd_s(&psMsgHead,"opmsg",caMessage1);
                utMsgDelVar(psMsgHead,"action");
                utMsgAdd_s(&psMsgHead,"action","04");
                ncOperWriteLog(psMsgHead);
            	
                iReturn = pasDbExecSqlF("insert into nclimweb(sid,gtype,id,num,url,timetype,starttime,stoptime,conflags,flags,warntype) values \
                    (%lu,%d,%lu,%d,'%s',%lu,%lu,%lu,%d,%d,%d)",
                     lSid,lGtype,lId,lRnum,caUrl,lDateid,lStime,lEtime,lFlags,lLimctl,atol(caWarntype));
                if(iReturn != 0) {
                    sprintf(caMessage,"insert into nclimweb(sid,gtype,id,num,url,timetype,starttime,stoptime,conflags,flags) values \
                    (0,%d,%lu,%d,'%s',%lu,%lu,%lu,%d,%d)",
                     lGtype,lId,lRnum,caUrl,lDateid,lStime,lEtime,lFlags,lLimctl);
                    printf(" Sql=%s\n",caMessage);
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0314记录增加出错, 可能为记录重复"));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                    pasDbCommit(NULL);
                    break;
                }
                case NC_LIMIT_WEBCLASS:
                    lSid = pasGetSid("nclimweb","sid");
                    iReturn = pasDbExecSqlF("insert into nclimweb(sid,gtype,id,num,url,timetype,starttime,stoptime,conflags,flags,urlid,warntype) values \
                    (%lu,%d,%lu,%d,'%s',%lu,%lu,%lu,%d,%d,%d,%d)",
                     lSid,lGtype,lId,lRnum,"",lDateid,lStime,lEtime,lFlags,lLimctl,atol(caUrlid),atol(caWarntype));
                if(iReturn != 0) {
              
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0314记录增加出错, 可能为记录重复"));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                    pasDbCommit(NULL);
                    break;
                }
                
        }
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
   	    for(i=0;i<lSum;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
   	            if(lFlags == NC_LIMIT_WEBLIST || lFlags == NC_LIMIT_WEBHOST||lFlags==NC_LIMIT_WEBCLASS) {	            	
   	            sprintf(caMessage,"delete from nclimweb where sid = %s",p);
   	             sprintf(caMessage1,"%s:[%s]",ncLang("1154删除上网控制规则"),caMessage); 
                utMsgDelVar(psMsgHead,"opmsg");
                utMsgAdd_s(&psMsgHead,"opmsg",caMessage1);
                utMsgDelVar(psMsgHead,"action");
                utMsgAdd_s(&psMsgHead,"action","04");
                ncOperWriteLog(psMsgHead);
   	            	  	            	
                    iReturn = pasDbExecSqlF("delete from nclimweb where sid = %s",p);
                }
                else {
	                sprintf(caMessage,"delete from nclimip where sid = %s",p);
	                sprintf(caMessage1,"%s:[%s]",ncLang("1154删除上网控制规则"),caMessage); 
	                utMsgDelVar(psMsgHead,"opmsg");
	                utMsgAdd_s(&psMsgHead,"opmsg",caMessage1);
	                utMsgDelVar(psMsgHead,"action");
                  utMsgAdd_s(&psMsgHead,"action","04");
	                ncOperWriteLog(psMsgHead);
                    iReturn = pasDbExecSqlF("delete from nclimip where sid = %s",p);
                }
                if(iReturn == 0) {
                    pasDbCommit(NULL);
                }
   	        }
   	    }
    }
    else if(!utStrIsSpaces(caMod)) { /* 修改记录  */
        lSid = atol(caSid);
        if(lFlags == NC_LIMIT_IPADDRESS) {
            if(utStrIsSpaces(caLip) || !utStrIsValidIp(caLip)) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0433起始IP地址为空或格式不对,正确的格式应为 xxx.xxx.xxx.xxx"));
                return 0;
            }
            if(utStrIsSpaces(caUip)) {
                strcpy(caUip,caLip);
            }
            if(!utStrIsValidIp(caUip)) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0864终止IP地址格式不对,正确的格式应为 xxx.xxx.xxx.xxx"));
                return 0;
            }
            if(lLport < 0 || lLport > 65535L) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0434起始端口号不对，应为不超过65535的数字"));
                return 0;
            }
            if(utStrIsSpaces(caUport)) {
                lUport = lLport;
            }
            else if(lUport < 0 || lUport > 65535L) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0865终止端口号不对，应为不超过65535的数字"));
                return 0;
            }
            if(lProt != 0 && lProt != NC_PROTOCOL_TCP && lProt != NC_PROTOCOL_UDP) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0687协议类型不对"));
                return 0;
            }
        }
        else if(lFlags == NC_LIMIT_IPLIST){
            strcpy(caLip,caListid);
        }
        else if(lFlags == NC_LIMIT_WEBLIST){
            strcpy(caUrl,caListid);
        }
        lStime = ncUtlIsValidTime(caStime);
        lEtime = ncUtlIsValidTime(caEtime);
        if(lStime < 0 || lEtime < 0) {
              utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0435起始或终止时间不正确"));
              return 0;
        }
        switch(lFlags) {
            case NC_LIMIT_IPLIST:
            case NC_LIMIT_IPADDRESS:
            case NC_LIMIT_INTERNET:
            case NC_LIMIT_SERVICE:
                if(lFlags == NC_LIMIT_IPADDRESS) {
                    lLip = ntohl(pasIpcvtLong(caLip));
                }
                else {
                    lLip = atol(caLip);
                }
                	sprintf(caMessage,"update nclimip set gtype=%lu,id=%lu,num=%lu,timetype=%lu,starttime='%s',stoptime='%s',conflags=%lu,flags=%lu,service=%lu,lowerip='%s',upperip='%s',lowerport=%lu,upperport=%lu,trantype=%d,warntype=%d where sid = %lu",
                           lGtype,lId,lRnum,lDateid,caStime,caEtime,lFlags,lLimctl,lService,caLip,caUip,lLport,lUport,lProt,atol(caWarntype),lSid);
                           
                   sprintf(caMessage1,"%s:[%s]",ncLang("1155修改上网控制规则"),caMessage);         
	                utMsgDelVar(psMsgHead,"opmsg");
	                utMsgAdd_s(&psMsgHead,"opmsg",caMessage1);
	                utMsgDelVar(psMsgHead,"action");
                  utMsgAdd_s(&psMsgHead,"action","04");
	                ncOperWriteLog(psMsgHead);
	                
	                
                iReturn = pasDbExecSqlF("update nclimip set gtype=%lu,id=%lu,num=%lu,timetype=%lu,starttime=%lu,stoptime=%lu,conflags=%lu,flags=%lu,service=%lu,lowerip=%lu,upperip=%lu,lowerport=%lu,upperport=%lu,trantype=%d,warntype=%d where sid = %lu",
                           lGtype,lId,lRnum,lDateid,lStime,lEtime,lFlags,lLimctl,lService,lLip,ntohl(pasIpcvtLong(caUip)),lLport,lUport,lProt,atol(caWarntype),lSid);
                if(iReturn != 0) {
                    sprintf(caMessage,"update nclimip set gtype=%lu,id=%lu,num=%lu,timetype=%lu,starttime=%lu,stoptime=%lu,conflags=%lu,flags=%lu,service=%lu,lowerip='%s',upperip='%s',lowerport=%lu,upperport=%lu,trantype=%d where sid = %lu",
                           lGtype,lId,lRnum,lDateid,lStime,lEtime,lFlags,lLimctl,lService,caLip,caUip,lLport,lUport,lProt,lSid);
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0308记录修改出错, 可能为记录重复 %s"),pasDbErrorMsg(NULL));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                    pasDbCommit(NULL);
                    break;
                }
            case  NC_LIMIT_WEBHOST:
            case  NC_LIMIT_WEBLIST:
            	    sprintf(caMessage,"update nclimweb set gtype=%lu,id=%lu,num=%lu,timetype=%lu,starttime='%s',stoptime='%s',conflags=%lu,flags=%lu,url='%s' where sid = %lu",
                           lGtype,lId,lRnum,lDateid,caStime,caEtime,lFlags,lLimctl,caUrl,lSid);
                  sprintf(caMessage1,"%s:[%s]",ncLang("1155修改上网控制规则"),caMessage);                   
	                utMsgDelVar(psMsgHead,"opmsg");
	                utMsgAdd_s(&psMsgHead,"opmsg",caMessage1);
	                utMsgDelVar(psMsgHead,"action");
                  utMsgAdd_s(&psMsgHead,"action","04");
	                ncOperWriteLog(psMsgHead);
            	           	
            	
                iReturn = pasDbExecSqlF("update nclimweb set gtype=%lu,id=%lu,num=%lu,timetype=%lu,starttime=%lu,stoptime=%lu,conflags=%lu,flags=%lu,url='%s',warntype=%d where sid = %lu",
                           lGtype,lId,lRnum,lDateid,lStime,lEtime,lFlags,lLimctl,caUrl,atol(caWarntype),lSid);
                if(iReturn != 0) {
                	
                    sprintf(caMessage,"update nclimweb set gtype=%lu,id=%lu,num=%lu,timetype=%lu,starttime=%lu,stoptime=%lu,conflags=%lu,flags=%lu,url='%s' where sid = %lu",
                           lGtype,lId,lRnum,lDateid,lStime,lEtime,lFlags,lLimctl,caUrl,lSid);
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0308记录修改出错, 可能为记录重复 %s"),pasDbErrorMsg(NULL));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                    pasDbCommit(NULL);
                    break;
                }
                case NC_LIMIT_WEBCLASS:
                iReturn = pasDbExecSqlF("update nclimweb set gtype=%lu,id=%lu,num=%lu,timetype=%lu,starttime=%lu,stoptime=%lu,conflags=%lu,flags=%lu,url='',urlid=%d,warntype=%d where sid = %lu",
                           lGtype,lId,lRnum,lDateid,lStime,lEtime,lFlags,lLimctl,atol(caUrlid),atol(caWarntype),lSid);
                if(iReturn != 0) {
                	
                    sprintf(caMessage,"update nclimweb set gtype=%lu,id=%lu,num=%lu,timetype=%lu,starttime=%lu,stoptime=%lu,conflags=%lu,flags=%lu,url='%s' where sid = %lu",
                           lGtype,lId,lRnum,lDateid,lStime,lEtime,lFlags,lLimctl,caUrl,lSid);
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0308记录修改出错, 可能为记录重复 %s"),pasDbErrorMsg(NULL));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                    pasDbCommit(NULL);
                    break;
                }
                
        }
    }
    else if(!utStrIsSpaces(caApp)) { /* 应用  */
        ncWebLimApp(psShmHead,iFd,psMsgHead);
        return 0;    
    }
    
    psDbHead = utPltInitDb();
    
    lCount = 0;
    iReturn = ncWebLimSumRule(lGtype,lId,lFlags,&lCount,&lCount1);
    if(lFlags==0 || lFlags == NC_LIMIT_WEBLIST || lFlags == NC_LIMIT_WEBHOST||lFlags==NC_LIMIT_WEBCLASS) {
        lSum = lCount1;
        lCount = ncWebLimShowWebLim(psDbHead,lGtype,lId,lFlags,1,lStart,lRowNum);
    }
    if(lFlags==0 || lFlags == NC_LIMIT_INTERNET || lFlags == NC_LIMIT_SERVICE ||
       lFlags==NC_LIMIT_IPADDRESS || lFlags==NC_LIMIT_IPLIST) {
        lSum = lCount;
        lCount = ncWebLimShowIpLim(psDbHead,lGtype,lId,lFlags,1,lStart,lRowNum);
    }
    if(lCount < 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0548数据库查询出错"));
        return 0;
    }
    pBuf = pasUtlCvtSqlSelectA("select name,id from nclimdatedes order by name",ncLang("0384每天,0;"));
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("dateid",caDateid,pBuf);
        free(pBuf);
    }
    utPltPutVar(psDbHead,"dateid", pSelect);
    free(pSelect);
    if(lFlags == NC_LIMIT_SERVICE) {
        pBuf = pasUtlCvtSqlSelect("select distinct name,id from ncipindex");
        if(pBuf) {
            pSelect= pasUtlCvtInputSelect("ser1",caService,pBuf);
            free(pBuf);
        }
        utPltPutVar(psDbHead,"service", pSelect);
        free(pSelect);
    }
    else if(lFlags == NC_LIMIT_IPADDRESS) {
        pSelect= pasUtlCvtInputSelect("prot",caProt,ncLang("0453全部,0;TCP,6;UDP,17;"));
        utPltPutVar(psDbHead,"prot", pSelect);
        free(pSelect);
        utPltPutVarF(psDbHead,"lport", "%lu",0);
        utPltPutVarF(psDbHead,"uport", "%lu",65535L);
        utPltPutVarF(psDbHead,"lip", "%s","0.0.0.0");
        utPltPutVarF(psDbHead,"uip", "%s","255.255.255.255");
    }
    else if(lFlags == NC_LIMIT_IPLIST) {
        pBuf = pasUtlCvtSqlSelect("select name,pid from ncippoltype");
        if(pBuf) {
            pSelect= pasUtlCvtInputSelect("listid",caListid,pBuf);
            free(pBuf);
        }
        utPltPutVar(psDbHead,"listid", pSelect);
        free(pSelect);
    }
    else if(lFlags == NC_LIMIT_WEBLIST) {
        pBuf = pasUtlCvtSqlSelect("select name,pid from ncwebpolclass ");
        if(pBuf) {
            pSelect= pasUtlCvtInputSelect("listid",caListid,pBuf);
            free(pBuf);
        }
        utPltPutVar(psDbHead,"listid", pSelect);
        free(pSelect);
    }
    else if(lFlags == NC_LIMIT_WEBCLASS) {
        pBuf = pasUtlCvtSqlSelect("select name,id from ncwebclass where status=1 order by name ");
        if(pBuf) {
            pSelect= pasUtlCvtInputSelect("urlid",caListid,pBuf);
            free(pBuf);
        }
        utPltPutVar(psDbHead,"urlid", pSelect);
        free(pSelect);
    }
        if(strcasecmp(utComGetVar_sd(psShmHead,"BandWidthLimit","Yes"),"Yes") == 0){
        	for(i=1;i<=3;i++){
        	utPltPutLoopVarF(psDbHead,"proid",i,"%d",i);
          }
        } 
    
    utPltPutVar(psDbHead,"stime","00:00:00");
    utPltPutVar(psDbHead,"etime","23:59:59");
    utPltPutVarF(psDbHead,"flags","%d",lFlags);
    utPltPutVarF(psDbHead,"gtype","%lu",lGtype);
    utPltPutVarF(psDbHead,"id","%lu",lId);
    utPltPutVar(psDbHead,"appname",ncLimGetAppName(lGtype,lId));
    utPltPutVarF(psDbHead,"sum","%lu",lSum);
    utPltPutVarF(psDbHead,"rnum", "%04d",lCount+1);
    utPltPutVarF(psDbHead,"CurPg", "%d",lCurPg);
    if(lSum > 0) {
        lTotPg = (lSum - 1) / lRowNum + 1;
    }
    else {
        lTotPg = 0;
    }
    utPltPutVarF(psDbHead,"TotPg", "%d",lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%d",lSum);

//输出带宽控制策略

       psCur=pasDbOpenSqlF("select qosid,qosname,qoslevel from ncnsaqos order by qoslevel ");
        if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
        memset(caQosname,0,sizeof(caQosname));
        lQosid=0;
        lQoslevel=0;
        iNum=0;
        iReturn=pasDbFetchInto(psCur,
        															UT_TYPE_LONG,4,&lQosid,
        															UT_TYPE_STRING,30,caQosname,
        															UT_TYPE_LONG,4,&lQoslevel);
 
        while(iReturn==0||iReturn==1405){
        	utPltPutLoopVarF(psDbHead,"qosid",iNum+1,"%d",lQosid);
        	if(lQoslevel==1){
        	  sprintf(caTemp,"%s(%s)",caQosname,ncLang("1824一级"));
        	}
        	else if(lQoslevel==2){
        		sprintf(caTemp,"%s(%s)",caQosname,ncLang("1825二级"));
        	}
        	else{
        		strcpy(caTemp,"");
        	}
        	utPltPutLoopVar(psDbHead,"qosname",iNum+1,caTemp);
        	
        	
        	iNum++;
        memset(caQosname,0,sizeof(caQosname));
        lQosid=0;
        lQoslevel=0;
        
        iReturn=pasDbFetchInto(psCur,
        															UT_TYPE_LONG,4,&lQosid,
        															UT_TYPE_STRING,30,caQosname,
        															UT_TYPE_LONG,4,&lQoslevel);

        }
        pasDbCloseCursor(psCur);
        

    i = 0;
    while(lCount < lRowNum) {
        i++;
        lCount++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",lCount);
    }
    
    if(lFlags == NC_LIMIT_INTERNET) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_out.htm");
    }
    else if(lFlags == NC_LIMIT_SERVICE) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_service.htm");
    }   
    else if(lFlags == NC_LIMIT_IPADDRESS) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_ip.htm");
    }
    else if(lFlags == NC_LIMIT_IPLIST) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_iplist.htm");
    }      
    else if(lFlags == NC_LIMIT_WEBHOST) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_web.htm");
    }  
    else if(lFlags == NC_LIMIT_WEBLIST) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_weblist.htm");
    }
    else if(lFlags == NC_LIMIT_WEBCLASS) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_webclasslist.htm");
    }
    return 0;
}


/* 显示基本信息  */
int ncWebLim_Listbase(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caOk[16],caDel[16],caCurPg[16];
    char caSum[16],caGtype[16],caId[16];
    char caCtype[16],caCnum[16],caSid[16];
    char *pBuf,*pSelect,*p;
    long  lSum,lGtype,lFlags,lId,lStart,lRowNum,lCurPg,lTotPg;
    long  lCount,lCount1,lSumRec;
    int   iReturn,i;
    utPltDbHead *psDbHead;

#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
 	
	iReturn = utMsgGetSomeNVar(psMsgHead,9,
	                "sum",     UT_TYPE_STRING, 12, caSum,    /* 记录总数                     */
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All 4-Polcy */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
	                "ok",       UT_TYPE_STRING, 12, caOk,
	                "del",      UT_TYPE_STRING, 12, caDel,
	                "CurPg",    UT_TYPE_STRING, 12, caCurPg,
	                "sid",      UT_TYPE_STRING,12, caSid,
	                "ctype",    UT_TYPE_STRING,12, caCtype,
	                "cnum",     UT_TYPE_STRING,12, caCnum);
    lSum = atol(caSum);
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    lRowNum = 25;
    lStart = (lCurPg - 1) * lRowNum;
    lGtype = atol(caGtype);
    lId = atol(caId);
    if(!utStrIsSpaces(caOk)) {  /* 保存  */
        if(!utStrIsSpaces(caSid)) {
            if(caCtype[0] == '0') {
                iReturn = pasDbExecSqlF("update nclimip set num = %lu where sid = %s and gtype= %lu",atol(caCnum),caSid,lGtype);
                if(iReturn == 0) {
                    pasDbCommit(NULL);
                }
            }
            else {
                iReturn = pasDbExecSqlF("update nclimweb set num = %lu where sid= %s and gtype=%lu ",atol(caCnum),caSid,lGtype);
                if(iReturn == 0) {
                    pasDbCommit(NULL);
                }
            } 
        }               
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
   	    for(i=0;i<lRowNum;i++) {
   	        sprintf(caVar,"ch0%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
                iReturn = pasDbExecSqlF("delete from nclimip where sid = %s and gtype=%lu ",p,lGtype);
                if(iReturn == 0) {
                    pasDbCommit(NULL);
                }
   	        }
   	        else {
      	        sprintf(caVar,"ch1%d",i+1);
   	            p = utMsgGetVar_s(psMsgHead,caVar);
                iReturn = pasDbExecSqlF("delete from nclimweb where sid = %s and gtype=%lu ",p,lGtype);
                if(iReturn == 0) {
                    pasDbCommit(NULL);
                }
            }
   	    }
   	    lSum = 0;
    }
    
    if(lSum == 0) {  /* 统计记录总数  */
        iReturn = ncWebLimSumRule(lGtype,lId,0,&lCount,&lCount1);
    }

    if(iReturn < 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0548数据库查询出错"));
        return 0;
    }
    else {
        lSumRec = lCount + lCount1;
    }
    psDbHead = utPltInitDb();
    
    
    if(lStart < lCount) {
       lCount = ncWebLimShowIpLim(psDbHead,lGtype,lId,0,1,lStart,lRowNum);
       if(lCount < 0) {
           utPltFreeDb(psDbHead);  
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0548数据库查询出错"));
            return 0;
        }
        lStart = 0;
        lSum = lRowNum - lCount;
   }
    else {
        lStart = lStart - lCount;
        lSum = lRowNum;
        lCount = 0;
    }
                
    if(lSum > 0) {
        lCount = ncWebLimShowWebLim(psDbHead,lGtype,lId,0,lCount+1,lStart,lSum);
  //      printf("lCount=%d\n",lCount);
        if(lCount < 0) {
            utPltFreeDb(psDbHead);  
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0548数据库查询出错"));
            return 0;
        }
    }
    i = 0;
    while(lCount < lRowNum) {
        i++;
        lCount++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",lCount);
    }

    utPltPutVarF(psDbHead,"CurPg","%d",lCurPg);
    if(lSumRec == 0) lTotPg = 0;
    else {
        lTotPg = (lSumRec -1 ) / lRowNum  + 1;
    }
    utPltPutVarF(psDbHead,"gtype","%d",lGtype);
    utPltPutVarF(psDbHead,"flags","%d",lFlags);
    utPltPutVarF(psDbHead,"id","%d",lId);
    utPltPutVarF(psDbHead,"TotPg","%d",lTotPg);
    utPltPutVarF(psDbHead,"TotRec","%d",lSumRec);
    if(lGtype == NC_LIMIT_BYPOLCY) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_base1.htm");
    }
    else {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_base.htm");
    }
    return 0;
}


/* 统计规则总数  */
int ncWebLimSumRule(long lGtype,long lId,long lFlags,int *iSumIp,int *iSumWeb)
{
   	int iReturn;
   	long lCount,lCount1;
   	unsigned long lGroupid,lType1;
   	long lGroupid0;
   	lGroupid0=0;
   	iReturn=dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);

   	
   	
   	if(lFlags == 0) {
   		if(lGtype==2){
   			if(iReturn==0&&lGroupid0>0){
   				   iReturn = pasDbOneRecord("select count(*) from nclimip where (gtype=:gtype and id=:id) ",2,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_LONG,4,iSumIp);
   			}
   			else{
   			  iReturn = pasDbOneRecord("select count(*) from nclimip where (gtype=:gtype and id=:id) or (gtype=3 and id=0)",2,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_LONG,4,iSumIp);
            }
   		}
   		else if(lGtype==1){
   			  pasDbOneRecord("select groupid from ncuser where userid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_LONG,4,&lGroupid);
           if(iReturn==0&&lGroupid0>0){
           	  iReturn = pasDbOneRecord("select count(*) from nclimip where (gtype=:gtype and id=:id) or (gtype=2 and id=:gid) ",3,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              "gid",UT_TYPE_LONG,lGroupid,
              UT_TYPE_LONG,4,iSumIp);
          }
          else{
          iReturn = pasDbOneRecord("select count(*) from nclimip where (gtype=:gtype and id=:id) or (gtype=2 and id=:gid) or (gtype=3 and id=0)",3,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              "gid",UT_TYPE_LONG,lGroupid,
              UT_TYPE_LONG,4,iSumIp);
            }
   		}
   		else
   	    iReturn = pasDbOneRecord("select count(*) from nclimip where gtype=:gtype and id=:id",2,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_LONG,4,iSumIp);
    }
    else {
   	    iReturn = pasDbOneRecord("select count(*) from nclimip where gtype=:gtype and id=:id and conflags=:flags",3,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              "flags",UT_TYPE_LONG,lFlags,
              UT_TYPE_LONG,4,iSumIp);
    }        
    if(iReturn != 0) {
        return (-1);
    }
   	if(lFlags == 0) {
   		if(lGtype==2){
   			if(lGroupid0>0){
   				 iReturn = pasDbOneRecord("select count(*) from nclimweb where (gtype=:gtype and id=:id) ",2,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_LONG,4,iSumWeb);
   			}
   			else{
   			iReturn = pasDbOneRecord("select count(*) from nclimweb where (gtype=:gtype and id=:id) or (gtype=3 or id=0)",2,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_LONG,4,iSumWeb);
            }
   		}
   		else if(lGtype==1){
   			 pasDbOneRecord("select groupid from ncuser where userid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_LONG,4,&lGroupid);
              if(lGroupid0>0){
              iReturn = pasDbOneRecord("select count(*) from nclimweb where (gtype=:gtype and id=:id) or (gtype=2 and id=:gid) ",3,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              "gid",UT_TYPE_LONG,lGroupid,
              UT_TYPE_LONG,4,iSumWeb);
              }
              else{
              iReturn = pasDbOneRecord("select count(*) from nclimweb where (gtype=:gtype and id=:id) or (gtype=2 and id=:gid) or (gtype=3 and id=0)",3,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              "gid",UT_TYPE_LONG,lGroupid,
              UT_TYPE_LONG,4,iSumWeb);
            }
   		}
   		else
   	    iReturn = pasDbOneRecord("select count(*) from nclimweb where gtype=:gtype and id=:id",2,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_LONG,4,iSumWeb);
    }
    else {
   	    iReturn = pasDbOneRecord("select count(*) from nclimweb where gtype=:gtype and id=:id and conflags=:flags",3,
              "gtype",UT_TYPE_LONG,lGtype,
              "id",UT_TYPE_LONG,lId,
              "flags",UT_TYPE_LONG,lFlags,
              UT_TYPE_LONG,4,iSumWeb);
    }        
    if(iReturn != 0) {
        return (-1);
    }
    return 0;
}

 char *ncLimGetServices1(long lServices);

/* 显示IP地址列表  */
int ncWebLimShowIpLim(utPltDbHead *psDbHead,long lType,long lId,long lFlags,long lStartNum,long lStart,long lSum)
{
   	long lSid1,lNum1,lTran1,lService1,lLport1,lUport1,lDateid1,lStime1,lEtime1,lFlags1,lAccept1;
   	long lCount;
   	unsigned long lGroupid;
   	unsigned long lLip1,lUip1,lType1,lWarntype;
   	char caLip1[16],caUip1[16];
   	int iNum,i,iReturn;
   	char *pBuffer,*p;
   	char caName[32],caTemp[1024];
   	long lGroupid0;
   	 iReturn=dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
   	 
   	 


// printf(" IpLim...lSum==%lu\n",lSum);   	    
    pBuffer = malloc(lSum * 256);
    if(pBuffer == NULL) {
        utSysLog(" Memory Error \n");
        return (-1);
    }
   	if(lFlags == 0) {
   		  if(lType==2){
   		  	 if((iReturn==0)&&(lGroupid0>0)){
   		  	 	iReturn = pasDbRecordSetB("select gtype,sid,num,lowerip,upperip,trantype,service,lowerport,upperport,timetype,starttime,stoptime,conflags,flags,warntype from nclimip where (gtype = :gtype and id = :id)  order by conflags,gtype,num limit :start,:sum",
            0,9999,&lCount,pBuffer,4,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);
   		  	}
   		  	else{
   		  	  iReturn = pasDbRecordSetB("select gtype,sid,num,lowerip,upperip,trantype,service,lowerport,upperport,timetype,starttime,stoptime,conflags,flags,warntype from nclimip where (gtype = :gtype and id = :id) or (gtype=3 and id=0) order by conflags,gtype,num limit :start,:sum",
            0,9999,&lCount,pBuffer,4,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);
            }
   		  }
   		  else if(lType==1){
   		  	lGroupid=-9;
   		  	pasDbOneRecord("select groupid from ncuser where userid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_LONG,4,&lGroupid);
              
             if((iReturn==0)&&(lGroupid0>0)){
             	
             iReturn = pasDbRecordSetB("select gtype,sid,num,lowerip,upperip,trantype,service,lowerport,upperport,timetype,starttime,stoptime,conflags,flags,warntype from nclimip where (gtype = :gtype and id = :id) or (gtype=2 and id=:gid)  order by conflags,gtype,num limit :start,:sum",
            0,9999,&lCount,pBuffer,5,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "gid",UT_TYPE_LONG,lGroupid,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);  
             	
             	
            }
            else{ 
              
           iReturn = pasDbRecordSetB("select gtype,sid,num,lowerip,upperip,trantype,service,lowerport,upperport,timetype,starttime,stoptime,conflags,flags,warntype from nclimip where (gtype = :gtype and id = :id) or (gtype=2 and id=:gid) or (gtype=3 and id=0) order by conflags,gtype,num limit :start,:sum",
            0,9999,&lCount,pBuffer,5,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "gid",UT_TYPE_LONG,lGroupid,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);  
            }
              
   		  }
   		  else{
   		  	
        iReturn = pasDbRecordSetB("select gtype,sid,num,lowerip,upperip,trantype,service,lowerport,upperport,timetype,starttime,stoptime,conflags,flags,warntype from nclimip where gtype = :gtype and id = :id order by conflags,gtype,num limit :start,:sum",
            0,9999,&lCount,pBuffer,4,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);
            }
    }
    else {
    	
        iReturn = pasDbRecordSetB("select gtype,sid,num,lowerip,upperip,trantype,service,lowerport,upperport,timetype,starttime,stoptime,conflags,flags,warntype from nclimip where gtype = :gtype and id = :id and conflags=:flags order by gtype,num limit :start,:sum",
            0,9999,&lCount,pBuffer,5,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "flags",UT_TYPE_LONG,lFlags,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);
    }        
    if(iReturn != 0) {
        free(pBuffer);
        return (-1);
    }
    p = pasDbRecordValue(pBuffer,1,15,
    															 UT_TYPE_LONG,4,&lType1,
      	                           UT_TYPE_LONG,4,&lSid1,
      	                           UT_TYPE_LONG,4,&lNum1,
      	                           UT_TYPE_ULONG,4,&lLip1,
      	                           UT_TYPE_ULONG,4,&lUip1,
  	                               UT_TYPE_LONG,4,&lTran1,
  	                               UT_TYPE_LONG,4,&lService1,
  	                               UT_TYPE_LONG,4,&lLport1,
  	                               UT_TYPE_LONG,4,&lUport1,
  	                               UT_TYPE_LONG,4,&lDateid1,
  	                               UT_TYPE_LONG,4,&lStime1,
  	                               UT_TYPE_LONG,4,&lEtime1,
  	                               UT_TYPE_LONG,4,&lFlags1,
  	                               UT_TYPE_LONG,4,&lAccept1,
  	                               UT_TYPE_LONG,4,&lWarntype);
//  	printf("iType=%d\n",lType1);
   	iNum = lStartNum - 1;
   	while(p) {
        iNum++;
        if(lType1==1) 
        utPltPutLoopVar(psDbHead,"type_desc",iNum,"User");
        else if(lType1==2)
        utPltPutLoopVar(psDbHead,"type_desc",iNum,"Dept");
        else
        utPltPutLoopVar(psDbHead,"type_desc",iNum,"Corp");
        
     
        utPltPutLoopVarF(psDbHead,"lff", iNum,"%d",0);
        utPltPutLoopVarF(psDbHead,"lrnum", iNum,"%04d",lNum1);
        utPltPutLoopVarF(psDbHead,"lnum",  iNum,"%lu",iNum);
        utPltPutLoopVarF(psDbHead,"lsid", iNum,"%lu",lSid1);
        utPltPutLoopVarF(psDbHead,"ldid", iNum,"%lu",lDateid1);
        utPltPutLoopVar(psDbHead,"ldatename",iNum, ncLimGetDateName(lDateid1));
        
        utPltPutLoopVar(psDbHead,"lstime",iNum, ncLimCvtTime(lStime1));
        utPltPutLoopVar(psDbHead,"letime",iNum, ncLimCvtTime(lEtime1));
        if(lFlags1 != NC_LIMIT_IPLIST) {
            if(lAccept1 == 0) {
                utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("0332禁止服务"));
            }
            else if(lAccept1 == 3){
            	 utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("1645告警"));
            }
            else if(lAccept1 == 4){
            	 utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("1844免监控"));
            }
            else if(lAccept1==20){
            	  utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("1313禁止上网"));
            }
            else if(lAccept1==31){
            	  utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("1310使用带宽1"));
            }
            else if(lAccept1==32){
            	  utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("1311使用带宽2"));
            }
            else if(lAccept1==33){
            	  utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("1312使用带宽3"));
            }
            else if(lAccept1>=100){
            	char caQosname[32];
//使用带宽控制策略
             sprintf(caTemp,"select qosname from ncnsaqos where qosid=%d ",lAccept1);
// printf("caTemp=%s\n",caTemp);            
             pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caQosname);
             sprintf(caTemp,"%s[%s]",ncLang("1826使用带宽策略"),caQosname);
             utPltPutLoopVar(psDbHead,"llimctl",iNum, caTemp);
            }
            else {
                utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("0827允许"));
            }
        }
        utPltPutLoopVarF(psDbHead,"lctlid",iNum, "%d",lAccept1);
        utPltPutLoopVarF(psDbHead,"lflags",iNum, "%d",lFlags1);
        
        utPltPutLoopVarF(psDbHead,"warntype",iNum,"%d",lWarntype);
        if(lWarntype==1){
        	utPltPutLoopVar(psDbHead,"warntype_desc",iNum,"严重");
        }
        else if(lWarntype==2){
        	utPltPutLoopVar(psDbHead,"warntype_desc",iNum,"中度");
        }
        else{
        	utPltPutLoopVar(psDbHead,"warntype_desc",iNum,"一般");
        }
        switch(lFlags1) {
            case NC_LIMIT_INTERNET:
                utPltPutLoopVarF(psDbHead,"lctldes",iNum, ncLang("0244访问外部网"));
                break;
            case NC_LIMIT_SERVICE:
                utPltPutLoopVarF(psDbHead,"lctldes",iNum, ncLang("0530使用网络服务[%s]"),ncLimGetServices(lService1));
                utPltPutLoopVarF(psDbHead,"lservice",iNum, "%lu",lService1);              
                break;
            case NC_LIMIT_FUNLIST:
            	sprintf(caTemp,"select name from ncservicefun where id=%d ",lLport1);
            	memset(caName,0,sizeof(caName));
            	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,31,caName);
            	  utPltPutLoopVarF(psDbHead,"lctldes",iNum, "使用功能[%s-%s]",caName,ncLimGetServices1(lService1));
                utPltPutLoopVarF(psDbHead,"lservice",iNum, "%lu",lService1);
            	  break;
            case NC_LIMIT_IPADDRESS:
                utPltPutLoopVar(psDbHead,"llip",iNum, utComHostIp(htonl(lLip1)));
                utPltPutLoopVar(psDbHead,"luip",iNum, utComHostIp(htonl(lUip1)));
                utPltPutLoopVarF(psDbHead,"llport",iNum, "%lu",lLport1);
                utPltPutLoopVarF(psDbHead,"luport",iNum, "%lu",lUport1);
                utPltPutLoopVarF(psDbHead,"lprot", iNum, "%lu",lTran1);
                strcpy(caLip1,utComHostIp(htonl(lLip1)));
                utPltPutLoopVarF(psDbHead,"lctldes",iNum, ncLang("0232访问IP地址%s-%s 端口%d-%d 协议%s"),
                             caLip1,utComHostIp(htonl(lUip1)),
                             lLport1,lUport1,ncLimGetProt(lTran1));
                break;
            case NC_LIMIT_IPLIST:
                utPltPutLoopVarF(psDbHead,"llistid",iNum, "%lu",lLip1);
                utPltPutLoopVarF(psDbHead,"lctldes",iNum, ncLang("0527使用[%s]的网络服务策略"),
                             ncLimGetIplistName(lLip1));
                break;
        }
    p = pasDbRecordValue(p,1,15,
    															 UT_TYPE_LONG,4,&lType1,
      	                           UT_TYPE_LONG,4,&lSid1,
      	                           UT_TYPE_LONG,4,&lNum1,
      	                           UT_TYPE_ULONG,4,&lLip1,
      	                           UT_TYPE_ULONG,4,&lUip1,
  	                               UT_TYPE_LONG,4,&lTran1,
  	                               UT_TYPE_LONG,4,&lService1,
  	                               UT_TYPE_LONG,4,&lLport1,
  	                               UT_TYPE_LONG,4,&lUport1,
  	                               UT_TYPE_LONG,4,&lDateid1,
  	                               UT_TYPE_LONG,4,&lStime1,
  	                               UT_TYPE_LONG,4,&lEtime1,
  	                               UT_TYPE_LONG,4,&lFlags1,
  	                               UT_TYPE_LONG,4,&lAccept1,
  	                               UT_TYPE_LONG,4,&lWarntype);
    }
    free(pBuffer);
    return iNum;
}



/* 显示Web访问限制  */
int ncWebLimShowWebLim(utPltDbHead *psDbHead,long lType,long lId,long lFlags,long lStartNum,long lStart,long lSum)
{
   	long lSid1,lNum1,lService1,lDateid1,lStime1,lEtime1,lFlags1,lAccept1,lCount;
   	char caUrl1[64];
   	unsigned long lGroupid;
   	unsigned long lType1,lUrlid;
   	char caUrlid[36];
   	int iNum,i,iReturn;
   	char *pBuffer,*p;
   	long lWarntype;
    char caTemp[1024];
    long lGroupid0;
     iReturn=dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    
// printf(" WebLim...\n");   	
    pBuffer = malloc(lSum * 256);
    if(pBuffer == NULL) {
        utSysLog(" Memory Error \n");
        return (-1);
    }
  

    
    
   	if(lFlags == 0) {
   		if(lType==1){
   			  lGroupid=-9;
   		  	pasDbOneRecord("select groupid from ncuser where userid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_LONG,4,&lGroupid);
           
           if(iReturn==0&&lGroupid0>0){
           	
           	 iReturn = pasDbRecordSetB("select a.gtype,a.sid,a.num,a.url,a.timetype,a.starttime,a.stoptime,a.conflags,a.flags,a.urlid,a.warntype from nclimweb a where (a.gtype = :gtype and a.id = :id) or (a.gtype=2 and a.id=:gid)  order by a.conflags,a.gtype,	a.num limit :start,:sum",
            0,9999,&lCount,pBuffer,5,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "gid",UT_TYPE_LONG,lGroupid,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);
           	
          }
          else{
              
          iReturn = pasDbRecordSetB("select a.gtype,a.sid,a.num,a.url,a.timetype,a.starttime,a.stoptime,a.conflags,a.flags,a.urlid,a.warntype from nclimweb a where (a.gtype = :gtype and a.id = :id) or (a.gtype=2 and a.id=:gid) or (a.gtype=3 and a.id=0) order by a.conflags,a.gtype,	a.num limit :start,:sum",
            0,9999,&lCount,pBuffer,5,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "gid",UT_TYPE_LONG,lGroupid,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);
            }
   		}
   		else if(lType==2){
   			if(iReturn==0&&lGroupid0>0){
   			    iReturn = pasDbRecordSetB("select a.gtype,a.sid,a.num,a.url,a.timetype,a.starttime,a.stoptime,a.conflags,a.flags,a.urlid,a.warntype from nclimweb a  where (a.gtype = :gtype and a.id = :id)  order by a.conflags,a.gtype,a.num limit :start,:sum",
            0,9999,&lCount,pBuffer,4,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);
            }
            else{
            iReturn = pasDbRecordSetB("select a.gtype,a.sid,a.num,a.url,a.timetype,a.starttime,a.stoptime,a.conflags,a.flags,a.urlid,a.warntype from nclimweb a  where (a.gtype = :gtype and a.id = :id) or (a.gtype=3 and a.id=0) order by a.conflags,a.gtype,a.num limit :start,:sum",
            0,9999,&lCount,pBuffer,4,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);
            }
   		}
   	else{
        iReturn = pasDbRecordSetB("select a.gtype,a.sid,a.num,a.url,a.timetype,a.starttime,a.stoptime,a.conflags,a.flags,a.urlid,a.warntype from nclimweb a  where a.gtype = :gtype and a.id = :id order by a.conflags,a.gtype,a.num limit :start,:sum",
            0,9999,&lCount,pBuffer,4,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);
          }
    }
    else {
        iReturn = pasDbRecordSetB("select a.gtype,a.sid,a.num,a.url,a.timetype,a.starttime,a.stoptime,a.conflags,a.flags,a.urlid,a.warntype from nclimweb a  where a.gtype = :gtype and a.id = :id and a.conflags=:flags order by a.num limit :start,:sum",
            0,9999,&lCount,pBuffer,5,
              "gtype",UT_TYPE_LONG,lType,
              "id",UT_TYPE_LONG,lId,
              "flags",UT_TYPE_LONG,lFlags,
              "start",UT_TYPE_LONG,lStart,
              "sum",  UT_TYPE_LONG,lSum);
    }        
    if(iReturn != 0) {
        free(pBuffer);
#ifdef LDEBUG
printf(" %d %s SQL=select a.sid,a.num,a.url,a.timetype,a.starttime,a.stoptime,a.conflags,a.flags,a.urlid,b.name from nclimweb a left join ncwebclass b on a.urlid=b.id where a.gtype = %lu and a.id = %lu order by a.num limit %lu,%lu\n",
                iReturn,pasDbErrorMsg(NULL),lType,lId,lStart,lSum);
#endif
        return (-1);
    }
 
// printf("pBuffer=%s\n",pBuffer);
   
    p = pasDbRecordValue(pBuffer,1,11,
    															 UT_TYPE_LONG,4,&lType1,
      	                           UT_TYPE_LONG,4,&lSid1,
      	                           UT_TYPE_LONG,4,&lNum1,
      	                           UT_TYPE_STRING,63,caUrl1,
  	                               UT_TYPE_LONG,4,&lDateid1,
  	                               UT_TYPE_LONG,4,&lStime1,
  	                               UT_TYPE_LONG,4,&lEtime1,
  	                               UT_TYPE_LONG,4,&lFlags1,
  	                               UT_TYPE_LONG,4,&lAccept1,
  	                               UT_TYPE_LONG,4,&lUrlid,
  	                               UT_TYPE_LONG,4,&lWarntype);
  	                              
   	iNum = lStartNum - 1;
   	while(p) {
        iNum++;
    
        if(lType1==1) 
        utPltPutLoopVar(psDbHead,"type_desc",iNum,"User");
        else if(lType1==2)
        utPltPutLoopVar(psDbHead,"type_desc",iNum,"Dept");
        else
        utPltPutLoopVar(psDbHead,"type_desc",iNum,"Corp");
        utPltPutLoopVarF(psDbHead,"lff", iNum,"%d",1);
        utPltPutLoopVarF(psDbHead,"lrnum", iNum,"%04d",lNum1);
        utPltPutLoopVarF(psDbHead,"lnum",  iNum,"%lu",iNum);
        utPltPutLoopVarF(psDbHead,"lsid", iNum,"%lu",lSid1);
        utPltPutLoopVarF(psDbHead,"ldid", iNum,"%lu",lDateid1);
        utPltPutLoopVar(psDbHead,"ldatename",iNum, ncLimGetDateName(lDateid1));
        
        utPltPutLoopVar(psDbHead,"lstime",iNum, ncLimCvtTime(lStime1));
        utPltPutLoopVar(psDbHead,"letime",iNum, ncLimCvtTime(lEtime1));
        if(lFlags1 != NC_LIMIT_WEBLIST) {
            if(lAccept1 == 0) {
    	        utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("0332禁止"));
            }
            else if(lAccept1 == 3){
            	 utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("1645告警"));
            }
            else if(lAccept1 == 4){
            	 utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("1844免监控"));
            }
            else if(lAccept1>=100){
            	char caQosname[32];
//使用带宽控制策略
             sprintf(caTemp,"select qosname from ncnsaqos where qosid=%d ",lAccept1);
// printf("caTemp=%s\n",caTemp);            
             pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caQosname);
             sprintf(caTemp,"%s[%s]",ncLang("1826使用带宽策略"),caQosname);
             utPltPutLoopVar(psDbHead,"llimctl",iNum, caTemp);
            }
            else {
                utPltPutLoopVar(psDbHead,"llimctl",iNum, ncLang("0827允许"));
            }
        }
        utPltPutLoopVarF(psDbHead,"lctlid",iNum, "%d",lAccept1);
        utPltPutLoopVarF(psDbHead,"lflags",iNum, "%d",lFlags1);
        
        utPltPutLoopVarF(psDbHead,"warntype",iNum,"%d",lWarntype);
        if(lWarntype==1){
        	utPltPutLoopVar(psDbHead,"warntype_desc",iNum,"严重");
        }
        else if(lWarntype==2){
        	utPltPutLoopVar(psDbHead,"warntype_desc",iNum,"中度");
        }
        else{
        	utPltPutLoopVar(psDbHead,"warntype_desc",iNum,"一般");
        }
        
        
        switch(lFlags1) {
            case NC_LIMIT_WEBHOST:
                utPltPutLoopVarF(psDbHead,"llurlname",iNum, "%s",caUrl1);
                utPltPutLoopVarF(psDbHead,"lctldes",iNum, ncLang("0236访问地址包含[%s]的网站"),caUrl1);
                break;
            case NC_LIMIT_WEBLIST:
                utPltPutLoopVarF(psDbHead,"llistid",iNum, "%s",caUrl1);
                utPltPutLoopVarF(psDbHead,"lctldes",iNum, ncLang("0531使用网址库控制策略[%s]"),
                             ncLimGetWeblistName(atol(caUrl1)));
                break;
            case NC_LIMIT_WEBCLASS:
                utPltPutLoopVarF(psDbHead,"urlid",iNum, "%d",lUrlid);
                sprintf(caTemp,"select name from ncwebclass where id=%d ",lUrlid);
                memset(caUrlid,0,sizeof(caUrlid));
                pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caUrlid);
                utPltPutLoopVarF(psDbHead,"lctldes",iNum, ncLang("1240使用网址库[%s]"),caUrlid);                             
                break;
        }
    p = pasDbRecordValue(p,1,11,
    															 UT_TYPE_LONG,4,&lType1,
      	                           UT_TYPE_LONG,4,&lSid1,
      	                           UT_TYPE_LONG,4,&lNum1,
      	                           UT_TYPE_STRING,63,caUrl1,
  	                               UT_TYPE_LONG,4,&lDateid1,
  	                               UT_TYPE_LONG,4,&lStime1,
  	                               UT_TYPE_LONG,4,&lEtime1,
  	                               UT_TYPE_LONG,4,&lFlags1,
  	                               UT_TYPE_LONG,4,&lAccept1,
  	                               UT_TYPE_LONG,4,&lUrlid,
  	                               UT_TYPE_LONG,4,&lWarntype);
  	                              
    }
    free(pBuffer);
    return iNum;
}


/* 根据ID取日期对应的名字  */
char *ncLimGetDateName(long lDateid)
{
    static char caName[32];
    int iReturn;
    if(lDateid == 0) {
        strcpy(caName,ncLang("0383每天"));
    }
    else {
        iReturn = pasDbOneRecord("select name from nclimdatedes where id=:id",1,
              "id",UT_TYPE_LONG,lDateid,
              UT_TYPE_STRING,31,caName);
        if(iReturn != 0) {
            strcpy(caName,"\0");
            printf(" id==%d %s \n",lDateid,pasDbErrorMsg(NULL));
        }
    }
    return caName;
}

 char *ncLimGetServices(long lServices)
{
    static char caName[32];
    char caSname[32],caGname[32];
    int iReturn;
    iReturn = pasDbOneRecord("select a.name,b.name from ncipindex a,ncservicecgroup b where a.groupid=b.groupid and sid=:id",1,
              "id",UT_TYPE_LONG,lServices,
              UT_TYPE_STRING,18,caSname,
              UT_TYPE_STRING,12,caGname);
    if(iReturn != 0) {
        strcpy(caName,"\0");
    }
   else{
   	sprintf(caName,"%s-%s",caGname,caSname);
  }
    return caName;
}


 char *ncLimGetServices1(long lServices)
{
    static char caName[32];
    char caSname[32],caGname[32];
    int iReturn;
    iReturn = pasDbOneRecord("select a.name from ncipindex a where a.sid=:id",1,
              "id",UT_TYPE_LONG,lServices,
              UT_TYPE_STRING,18,caSname);
            
    if(iReturn != 0) {
        strcpy(caName,"\0");
    }
   else{
   	sprintf(caName,"%s",caSname);
  }
    return caName;
}




char *ncLimGetIplistName(long lId)
{
    static char caName[32];
    int iReturn;
    iReturn = pasDbOneRecord("select name from ncippoltype where pid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_STRING,31,caName);
    if(iReturn != 0) {
        strcpy(caName,"\0");
    }
    return caName;
}

static char *ncLimGetWeblistName(long lId)
{
    static char caName[32];
    int iReturn;
    iReturn = pasDbOneRecord("select name from ncwebpolclass  where pid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_STRING,31,caName);
    if(iReturn != 0) {
        strcpy(caName,"\0");
    }
    return caName;
}

char *ncLimCvtTime(long lStime)
{
    static char caTime[10];
    int iReturn;
    int l1,l2,l3;
    l1 = lStime / 10000;
    l2 = (lStime - l1 * 10000) / 100;
    l3 = lStime % 100;
    sprintf(caTime,"%02d:%02d:%02d",l1,l2,l3);
    return caTime;
}

/* 取限制名称  */
char *ncLimGetAppName(long lGtype,long lId)
{
    static char caName[32];
    int iReturn;
    iReturn = 1;
    if(lGtype ==  NC_LIMIT_BYUSER) {
        iReturn = pasDbOneRecord("select dispname from ncuser where userid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_STRING,31,caName);
    }  
    else if(lGtype == NC_LIMIT_BYGROUP) {
        iReturn = pasDbOneRecord("select groupname from ncgroup where groupid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_STRING,31,caName);
    }  
    else if(lGtype ==  NC_LIMIT_BYALL) {
        iReturn = 0;
        strcpy(caName,ncLang("0446全部"));
    }
    else if(lGtype == NC_LIMIT_BYPOLCY) {
        iReturn = pasDbOneRecord("select name from ncprocy where id=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_STRING,31,caName);
    }        
    if(iReturn == 0) {
        return caName;
    }
    else {
        strcpy(caName,"\0");
        return caName;
    }
}

char *ncLimGetProt(long lTran1)
{
    static char caName[32];
    strcpy(caName,"\0");
    if(lTran1 == 0) {
        strcpy(caName,ncLang("0446全部"));
    }
    else if(lTran1 == NC_PROTOCOL_TCP) {
        strcpy(caName,"TCP");
    }
    else if(lTran1 == NC_PROTOCOL_UDP) {
        strcpy(caName,"UDP");
    }
    return caName;
}

int ncLimGetLimitid(long lGtype,long lId,char *pLimid,char *pPost,char *pWebip)
{
    int iReturn;
    iReturn = (-1);
    switch(lGtype) {
        case NC_LIMIT_BYUSER:   /* 基于用户  */
            iReturn = pasDbOneRecord("select limitid,postflags,webipflags from ncuser where userid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_STRING,15,pLimid,
              UT_TYPE_STRING,15,pPost,
              UT_TYPE_STRING,15,pWebip);
            break;
        case NC_LIMIT_BYGROUP:  /* 基于组    */
            iReturn = pasDbOneRecord("select limitid,postflags,webipflags from ncgroup where groupid=:id",1,
              "id",UT_TYPE_LONG,lId,
              UT_TYPE_STRING,15,pLimid,
              UT_TYPE_STRING,15,pPost,
              UT_TYPE_STRING,15,pWebip);
            break;
        case NC_LIMIT_BYALL:    /* 全部      */
            iReturn = 0;
            strcpy(pLimid,ncGetSysSetInfo("gobalselectid","0"));
            strcpy(pPost, ncGetSysSetInfo("gobalpostflags","0"));
            strcpy(pWebip,ncGetSysSetInfo("gobalwebip","0"));
            break;
    }
    
    if(iReturn != 0) {
        return (-1);
    }
    else {
        return 0;
    }
}

/* 策略分类  */
int ncWebLimRuleGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId;
   	char caName[64];
   	char caPlate[128];
 	pasDbCursor *psCur;
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebLimRuleGroup");
 	}
	iReturn = utMsgGetSomeNVar(psMsgHead,1,
	                "plate",  UT_TYPE_STRING,127,caPlate);
    psCur = pasDbOpenSqlF("select id,name from ncprocy");
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0524时间信息出错"));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lId,
  	                               UT_TYPE_STRING,63,caName);
   	iNum = 0;
   	while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%u",iNum);
        utPltPutLoopVarF(psDbHead,"id",   iNum,"%u",lId);
        utPltPutLoopVarF(psDbHead,"name", iNum,"%s",caName);
        iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lId,
  	                               UT_TYPE_STRING,63,caName);
    }
    pasDbCloseCursor(psCur);
    utPltPutVar(psDbHead,"plate",caPlate);
    utPltPutVarF(psDbHead,"polid","%lu",utStrGetId());
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}

int ncWebLimRuleGroupEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId,i,lSid;;
   	char caId[16];
   	char *p;
   	char caName[64],caVar[64];
   	char caPlate[128];
   	char caGid[16],caDid[16],caAdd[16],caDel[16];
   	char caNname[32],caNid[16],caModi[16];
 	pasDbCursor *psCur;
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebLimRuleGroupEdit");
 	}
	iReturn = utMsgGetSomeNVar(psMsgHead,9,
	                "gname",  UT_TYPE_STRING,63,caName,
	                "id",    UT_TYPE_STRING,12,caId,
	                "did",    UT_TYPE_STRING,12,caDid,
	                "add",    UT_TYPE_STRING,12,caAdd,
	                "del",    UT_TYPE_STRING,12,caDel,
	                "plate",  UT_TYPE_STRING,127,caPlate,
	                "nid",    UT_TYPE_STRING,12,caNid,
	                "modi",   UT_TYPE_STRING,12,caModi,
	                "nname",  UT_TYPE_STRING,31,caNname
	                );
    if(!utStrIsSpaces(caAdd)) { /* 增加一个组  */
        sprintf(caGid,"%lu",pasGetSid("ncprocy","id"));
         if(!utStrIsSpaces(caName)) {
            iReturn = pasDbExecSqlF("insert into ncprocy(id,name) values (%s,'%s')",
                                              caGid,caName);
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0316记录增加出错,可能为ID重复"));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                pasDbCommit(NULL);
            }                
        }
        else {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0362类别名称为空"));
            return 0;
       }            
            
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除一个组  */
    	   	for(i=0;i<999;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
   	            lSid = atol(p);

				        iReturn = pasDbExecSqlF("delete from ncprocy where id= %lu",lSid);
				        if(iReturn != 0) {
				            pasDbRollback(NULL);
				        }                
				        else {
				            iReturn = pasDbExecSqlF("delete from nclimip where id= %lu and gtype=4",lSid);
				            iReturn = pasDbExecSqlF("delete from nclimweb where id= %lu and gtype=4",lSid);
				            iReturn = pasDbExecSqlF("delete from nclimsumm where polid= %lu ",lSid);
				            pasDbCommit(NULL);
				        }            
   	            

   	        }        
   	    }
    	
             
    }
    else if(!utStrIsSpaces(caModi) && !utStrIsSpaces(caName)) { /* 修改一个组  */
        iReturn = pasDbExecSqlF("update ncprocy set name='%s' where id = %s",
                         caName,caId);
        pasDbCommit(NULL);
    }
    return ncWebLimRuleGroup(psShmHead,iFd,psMsgHead);
    return 0;
}
    



/* 重新装载访问限制内容  */
int ncWebLimApp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
#ifdef LDEBUG
    printf(" Getpid==%d \n",getpid());
#endif       
    if(ncUtlIsLock(psShmHead,NC_LOCK_SYSREFRESH,5)==0) {
        ncInitProeimA(psShmHead,1);
        ncUtlUnLock(psShmHead,NC_LOCK_SYSREFRESH);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back2.htm",ncLang("0086Web地址列表"),ncLang("0662系统参数已重新刷新"));
    }
    else {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back2.htm",ncLang("0086Web地址列表"),ncLang("0672系统正忙，请稍候再试"));
        return 0;
    } 
    return 0;
}

//同步策略组

int ncSysDomainProcySave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId,i,lSid;;
   	char caId[16];
   	char *p;
   	char caName[64],caVar[64];
   	char caPlate[128];
   	char caGid[16],caDid[16],caAdd[16],caDel[16];
   	char caNname[32],caNid[16],caModi[16];
   	char caSum[18],caTemp[1024];
   	long lSum,lCount;

 	pasDbCursor *psCur;

	iReturn = utMsgGetSomeNVar(psMsgHead,2,
	                "sum",  UT_TYPE_STRING,9,caSum,	             
	                "plate",  UT_TYPE_STRING,127,caPlate);
	                
	lSum=atol(caSum);

	for(i=1;i<=lSum;i++){
		 sprintf(caVar,"chk%d",i);
		 p = utMsgGetVar_s(psMsgHead,caVar);
	
   	 if(p) {
   	 		ncUtlGetWordBetween(p,"CN=",",",caName,31);
				if(strlen(caName)==0){
							ncUtlGetWordBetween(p,"cn=",",",caName,31);
				}
				if(strlen(caName)>0){
							sprintf(caTemp,"select count(*) from ncprocy where name='%s' ",caName);
	//						printf("caTemp=%s\n",caTemp);
							lCount=0;
							pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
							if(lCount==0){
								lSid=pasGetSid("ncprocy","id");
								pasDbExecSqlF("insert into ncprocy(id,name) values (%d,'%s')",lSid,caName);
							}
						}
				}
   	 	
   	 }
	                
  pasDbCommit(NULL);
  psDbHead = utPltInitDb();     
  utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main_domainprocy.htm");
 //   return ncWebLimRuleGroup(psShmHead,iFd,psMsgHead);
    return 0;
}


//同步部门

int ncSysDomainDeptSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId,i,lSid;;
   	char caId[16];
   	char *p,*p1;
   	char caName[64],caVar[64];
   	char caPlate[128];
   	char caGid[16],caDid[16],caAdd[16],caDel[16];
   	char caNname[32],caNid[16],caModi[16];
   	char caSum[18],caTemp[1024];
   	long lSum,lCount,j,preid,lGroupid;
   	char caSql[1024];
   	char caObj[20][36];
   	char caMsg[256];
   	long lSumUser,lSumInsert,lSumUpdate;
 	pasDbCursor *psCur;
    lSumUser=0;
    lSumInsert=0;
	iReturn = utMsgGetSomeNVar(psMsgHead,2,
	                "sum",  UT_TYPE_STRING,9,caSum,	             
	                "plate",  UT_TYPE_STRING,127,caPlate);
	                
	lSum=atol(caSum);

	for(i=1;i<=lSum;i++){
		 sprintf(caVar,"chk%d",i);
		 p = utMsgGetVar_s(psMsgHead,caVar);
	
   	 if(p) {
		   	 	 if((strcasecmp(utComGetVar_sd(psShmHead,"NameGroupBy","ou"),"department")==0)||strcasecmp(utComGetVar_sd(psShmHead,"NameGroupBy","ou"),"company")==0){
		   	 	 	  	sprintf(caTemp,"select count(*) from ncgroup where groupname='%s' ",p);
printf("caTemp=%s\n",caTemp);
									lCount=0;
									pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
									if(lCount==0){
										lSid=pasGetSid("ncgroup","groupid");
										pasDbExecSqlF("insert into ncgroup(groupid,groupname) values (%d,'%s')",lSid,p);
										lSumInsert++;
									}
							
		   	 	 }
		   	 	 else if(strcasecmp(utComGetVar_sd(psShmHead,"NameGroupBy","ou"),"Group")==0){

		   	 		p1=ncUtlGetWordBetween(p,"CN=",",",caName,31);
//		   	 		printf("p1=%s\n",p1);
						if(strlen(caName)==0){
									ncUtlGetWordBetween(p,"cn=",",",caName,31);
						}
						if(strlen(caName)>0){
									sprintf(caTemp,"select count(*) from ncgroup where groupname='%s' ",caName);
			//						printf("caTemp=%s\n",caTemp);
									lCount=0;
									pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
									if(lCount==0){
										lSid=pasGetSid("ncgroup","groupid");
										pasDbExecSqlF("insert into ncgroup(groupid,groupname) values (%d,'%s')",lSid,caName);
										lSumInsert++;
									}
								}
						}
						else{ 	
							iNum=0;
							p1=p;		
							while(*p1){	
										  							                                                                    //同步ou作为部门
							 p1=ncUtlGetWordBetween(p,"OU=",",",caName,31); 
							
							 if(strlen(caName)==0){
							 	   p1=ncUtlGetWordBetween(p,"ou=",",",caName,31); 
							 	}
							 	if(strlen(caName)>0){
							 		strcpy(caObj[iNum],caName);
							 		
							 		iNum++;
							 	}
							 	
							 	if(!p1) break;
							 	p=p1;
							 
							} 
							
					for(j=0;j<iNum;j++){
										
      			lGroupid=0;
    			
    	
      				sprintf(caSql,"select groupid from ncgroup where groupname='%s' and level=%d ",caObj[iNum-j-1],j);
      				pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lGroupid);
      				if(j==0)
      			  	preid=0;
      			
      			  
      				if(lGroupid<=0){
      				 	lGroupid =  pasGetSid("ncgroup","groupid"); 
      				 	pasDbExecSqlF("insert into ncgroup (groupname,groupid,addtime,pid,level) values ('%s',%ld,%lu,%lu,%lu)",caObj[iNum-j-1],lGroupid,time(0),preid,j);
      				 	lSumInsert++;
      				}
      				else{
      				 	pasDbExecSqlF("update ncgroup set groupname='%s' where groupid=%d ",caObj[iNum-j-1],lGroupid);
      				}
      				preid=lGroupid;
      		
      				
								
								
							}
							 
							 	                                              
						}
   	 	}
   	 }
	                
  pasDbCommit(NULL);
  psDbHead = utPltInitDb();  
  sprintf(caMsg,"插入部门总数:%d",lSumInsert);
  utPltPutVar(psDbHead,"insertuser",caMsg);
 
  utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_sysdomaindept_tip.htm");
 //   return ncWebLimRuleGroup(psShmHead,iFd,psMsgHead);
    return 0;
}


//显示同步用户下一步
int ncSysDomainUserFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId,i,lSid;;
   	char caId[16];
   	char *p;
   	char caName[64],caVar[64];
   	char caPlate[128];
   	char caGid[16],caDid[16],caAdd[16],caDel[16];
   	char caNname[32],caNid[16],caModi[16];
   	char caSum[18],caTemp[1024];
   	long lSum,lCount;
    char caUser_flag[8];
 	pasDbCursor *psCur;



  psDbHead = utPltInitDb(); 
  iReturn = utMsgGetSomeNVar(psMsgHead,1, "user_flag",  UT_TYPE_STRING,1,caUser_flag); 
    
   if(strlen(caUser_flag)==0){
   	 strcpy(caUser_flag,"1");
   	}
   	utPltPutVar(psDbHead,"user_flag",caUser_flag);
   	
   	
  utPltPutVar(psDbHead,"RootDc",utComGetVar_sd(psShmHead,"RootDc",""));  
  utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_sysdomainuserfirst.htm");

    return 0;
}


//显示同步用户下一步
int ncSysDomainUserNext(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

}


//显示同步部门信息的第一步
int ncSysDomainDeptfirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

}

 //显示域验证综合参数设置界面 
  int ncYyzBase_new(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead){
        utPltDbHead *psDbHead;
        char *pConfig;
        pasConfList *psConfig;
       
        psDbHead = utPltInitDb();
       
        
        pConfig = pasGetConfigFileName();
		    psConfig = pasUtlReadConfig(pConfig);
		    if(psConfig == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0606网络配置"),ncLang("0427配置文件出错"));
		        return 0;
		    }
        utPltPutVar(psDbHead,"NtDoMainServer",pasUtlLookConfig(psConfig,"NtDoMainServer","192.168.0.56"));
        utPltPutVar(psDbHead,"NtDoMainUser",pasUtlLookConfig(psConfig,"NtDoMainUser",""));  
        
        utPltPutVar(psDbHead,"NtDoMainPass",pasUtlLookConfig(psConfig,"NtDoMainPass",""));
        
  //      utPltPutVar(psDbHead,"searchfilter",pasUtlLookConfig(psConfig,"searchfilter","(&(objectCategory=person)(objectClass=User))"));
        
        
        utPltPutVar(psDbHead,"RootDc",pasUtlLookConfig(psConfig,"RootDc",""));
        
        utPltPutVar(psDbHead,"NtDispName",pasUtlLookConfig(psConfig,"NtDispName","None"));
        utPltPutVar(psDbHead,"NameGroupBy",pasUtlLookConfig(psConfig,"NameGroupBy","Ou"));
        utPltPutVar(psDbHead,"CtlProcyBy",pasUtlLookConfig(psConfig,"CtlProcyBy","None"));
//        utPltPutVar(psDbHead,"AddGroupByNt",pasUtlLookConfig(psConfig,"AddGroupByNt","Yes"));
        
        utPltPutVar(psDbHead,"DelUserNotInDm",pasUtlLookConfig(psConfig,"DelUserNotInDm","No"));

        utPltPutVar(psDbHead,"StartSysLdap",pasUtlLookConfig(psConfig,"StartSysLdap","No"));
        utPltPutVar(psDbHead,"SysStep",pasUtlLookConfig(psConfig,"SysStep","24"));
        
        pasUtlFreeConfig(psConfig);
           
       		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/yyz_base_new.htm");
      
         return 0;
}


//保存域验证的综合参数
int ncYyzBas_new_Save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caApp[20];
    char caMsg[256];
    int iReturn;
    char caDbname[32],caDbuser[32],caDbpass[32];
    char *pConfig;
    char *p,*pFile,*pNat,*pRoute;
    FILE *fp;
    pasConfList *psConfig;
   
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880自动备份设定"),ncLang("0427配置文件出错"));
        return 0;
    }
    
    if(p = utMsgGetVar_s(psMsgHead,"NtDoMainServer")) {
        pasUtlModiConfig(psConfig,"NtDoMainServer",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"NtDoMainUser")) {
        pasUtlModiConfig(psConfig,"NtDoMainUser",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"NtDoMainPass")) {
        pasUtlModiConfig(psConfig,"NtDoMainPass",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"RootDc")) {
        pasUtlModiConfig(psConfig,"RootDc",p);
    }
    
//     if(p = utMsgGetVar_s(psMsgHead,"searchfilter")) {
//        pasUtlModiConfig(psConfig,"searchfilter",p);
 //   }
    
    
    if(p = utMsgGetVar_s(psMsgHead,"NtDispName")) {
        pasUtlModiConfig(psConfig,"NtDispName",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"NameGroupBy")) {
        pasUtlModiConfig(psConfig,"NameGroupBy",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"CtlProcyBy")) {
        pasUtlModiConfig(psConfig,"CtlProcyBy",p);
    }
//    if(p = utMsgGetVar_s(psMsgHead,"AddGroupByNt")) {
//        pasUtlModiConfig(psConfig,"AddGroupByNt",p);
//    }
    if(p = utMsgGetVar_s(psMsgHead,"DelUserNotInDm")) {
        pasUtlModiConfig(psConfig,"DelUserNotInDm",p);
    }
    if(p = utMsgGetVar_s(psMsgHead,"StartSysLdap")) {
        pasUtlModiConfig(psConfig,"StartSysLdap",p);
    }
    
    if(p = utMsgGetVar_s(psMsgHead,"SysStep")) {
        pasUtlModiConfig(psConfig,"SysStep",p);
    }
    
    
    pasUtlSaveConfig(pConfig,psConfig);
    pasUtlFreeConfig(psConfig);
   
   ncUtlReReadConFig(psShmHead);
   ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
      
    ncYyzBase_new(psShmHead,iFd,psMsgHead);

    return 0;
}