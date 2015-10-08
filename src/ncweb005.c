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
#include "ncdef.h"
#include "mail.h"
#include "pasutl.h"
extern int iDebugLevel;
char *ncUtlTrimOrder(char *pWhere);
/* 通过优化后的邮件查询  增加编码转换 */
int ncUtlMailCtlList(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead,char *pTable,char *pPlate)
{
    char caWhere[512],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caUserid[16];
    char caUsername[32],caTo[64],caFrom[64],caSub[128],caSdate[32];
    char caFile[128],caTemp[256];
    char caCount[32],caVirus[32];
    char caWhereStr[6]="where";
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lAtt,lStat,lSize,lUserid0,lSid,lSdate,lVirus;
    int i,iNum,iPos;
    int iUserFlags=0;   /* 0--独立表  1--用户信息  */
    utPltDbHead *psDbHead;
   	int iReturn;
   	char caSearch[16];
   	long lGroupid0,lRowNum,lflags,lStartRec,lStatus1,lCount,lSumUser,lCode;
 	pasDbCursor *psCur;
 	int iCharCode=0;
	if(strcasecmp(utComGetVar_sd(psShmHead,"MailCharCode","Auto"),"Auto") == 0) {
	    iCharCode = 1;
	}
	else {
	    iCharCode = 0;
	}

 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncUtlMailCtlList");
 	}
	iReturn = utMsgGetSomeNVar(psMsgHead,7,
	                "search",     UT_TYPE_STRING, 12, caSearch,
	                "userid",     UT_TYPE_STRING, 12, caUserid,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					"CurPg",      UT_TYPE_STRING, 12, caCurPg,
   	                "cond",       UT_TYPE_STRING, 63, caCond,
   	                "where",      UT_TYPE_STRING, 255,caWhere,
   	                "lcount",     UT_TYPE_STRING, 12, caCount);
    sprintf(caCond0,"'%c%s%c'",'%',caCond,'%');
    if(!utStrIsSpaces(caSearch)) {
        strcpy(caCount,"\0");
        strcpy(caWhere,"\0");
        lCurPg = 1;
    }
    else {
        lCurPg = atol(caCurPg);
        if(lCurPg <= 0) lCurPg = 1;
    }
    if(!utStrIsSpaces(caWhere)) {
    }
    else {
        lUserid0 = atol(caUserid);
        iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
        if(iReturn == 0 && lGroupid0 > 0) {
            if(!utStrIsSpaces(caCond)) {  
                lflags = 10;
                if(strcmp(caCond,ncLang("0850正常"))==0) {
                    lflags = 1;
                }
                else if(strcmp(caCond,ncLang("0650无"))==0) {
                    lflags = 0;
                }
                else if(strcmp(caCond,ncLang("0710需审计"))==0) {
                    lflags = 4;
                }
                else if(strcmp(caCond,ncLang("0724已审计"))==0) {
                    lflags = 5;
                }
                else if(strcmp(caCond,ncLang("0340拒绝"))==0) {
                    lflags = 2;
                }
                else if(strcmp(caCond,ncLang("0874转发"))==0) {
                    lflags = 3;
                }
                if(lflags < 9) {
                    sprintf(caWhere," and a.groupid = %lu and (b.mailstat = %d)",lGroupid0,lflags);
                }
                else {
                    sprintf(caWhere," and a.groupid = %lu ",lGroupid0);
                }
                if(lUserid0 > 0) {
                    utStrAddF(caWhere," and b.userid = %lu ",lUserid0);
                }
                utStrAddF(caWhere," and (b.sdate like %s or b.mailfrom like %s or b.mailto like %s or b.mailsub like %s)",
                                      caCond0,caCond0,caCond0,caCond0);
            }
            else {
                sprintf(caWhere," and a.groupid = %lu ",lGroupid0);
                if(lUserid0 > 0) {
                    utStrAddF(caWhere," and b.userid = %lu ",lUserid0);
                }
            }
        }
        else {
            if(!utStrIsSpaces(caCond)) {  
                lflags = 10;
                if(strcmp(caCond,ncLang("0850正常"))==0) {
                    lflags = 1;
                }
                else if(strcmp(caCond,ncLang("0650无"))==0) {
                    lflags = 0;
                }
                else if(strcmp(caCond,ncLang("0710需审计"))==0) {
                    lflags = 4;
                }
                else if(strcmp(caCond,ncLang("0340拒绝"))==0) {
                    lflags = 2;
                }
                else if(strcmp(caCond,ncLang("0874转发"))==0) {
                    lflags = 3;
                }
                else if(strcmp(caCond,ncLang("0724已审计"))==0) {
                    lflags = 5;
                }
                if(lflags < 9) {
                    sprintf(caWhere," and b.mailstat = %d",lflags);
                }
                if(lUserid0 > 0) {
                    utStrAddF(caWhere," and b.userid = %lu ",lUserid0);
                }
                utStrAddF(caWhere," and (b.sdate like %s or b.mailfrom like %s or b.mailto like %s or b.mailsub like %s)",
                                      caCond0,caCond0,caCond0,caCond0);
            }
            else {
                if(lUserid0 > 0) {
                    utStrAddF(caWhere," and b.userid = %lu ",lUserid0);
                }
            }
        }
        utStrAddF(caWhere," order by b.sdate desc");
    }
    
    lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;  
    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }
    if(utStrIsSpaces(caCount)) {
        if( strstr(caWhere,"a.")) { /* 涉及用户信息  */
            sprintf(caTemp,"select count(*) from %s b,ncuser a %s b.userid=a.userid and  %s",
                     pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));
        }
        else {
            sprintf(caTemp,"select count(*) from  %s b %s %s",pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));
        }
        iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[Count=%d]%s",lCount,caTemp);
        }
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0301记录查询出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
    }
    else {
        lCount = atol(caCount);
    }
    
    if( strstr(caWhere,"a.")) { /* 涉及用户信息  */
        psCur = pasDbOpenSqlF("select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode,b.virusstat,b.virusname\
                from  %s b,ncuser a  where b.userid = a.userid  %s limit %d,%d",pTable,caWhere,lStartRec,lRowNum);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[User]select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode,b.virusstat,b.virusname \
                from  %s b,ncuser a where b.userid = a.userid  %s limit %d,%d",pTable,caWhere,lStartRec,lRowNum);
        }
    }
    else {        
        psCur = pasDbOpenSqlF("select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode,b.virusstat,b.virusname \
                from %s b %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lRowNum);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[NoUser]select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode,b.virusstat,b.virusname \
                from %s b %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lRowNum);
        }
    } 
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }        
    psDbHead = utPltInitDb();
    iReturn = 0;

    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_ULONG,4,&lSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lStat,
  	                               UT_TYPE_LONG,4,&lStatus1,
  	                               UT_TYPE_LONG,4,&lCode,
  	                               UT_TYPE_LONG,4,&lVirus,
  	                               UT_TYPE_STRING,31,caVirus);
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
   	    i++;
        iNum++;
        if(iCharCode == 0) {
            lCode = pasDefaultCode();
        }
        if(lStat == 0 ) {
            strcpy(caFile,"\0");
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0655无内容"));
        }
        else if(lStat == 1) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0859直接发送"));
        }
        else if(lStat == 4 ) {
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0626未处理"));
        }
        else if(lStat == 5){
            if(lStatus1 == 1){
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0511审计发送"));
            }                
            else if(lStatus1 == 2){
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0512审计拒绝"));
            }                
            else if(lStatus1 == 3) {
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0513审计转发"));
            }                
        }
        else if(lStat == 2) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0860直接拒绝"));
        }
        else if(lStat == 3) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0861直接转发"));
        }
        else {
            strcpy(caFile,"\0");
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0655无内容"));
        }
        if(lStat == 4) {
            utPltPutLoopVar(psDbHead,"stat", iNum,"2");
        }
        else {
            utPltPutLoopVar(psDbHead,"stat", iNum,"1");
        }
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);

        utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lSdate));
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVar(psDbHead,"file", iNum,caFile);
        utPltSetCvtHtml(1);  
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"from", iNum,pasCvtGBK(lCode,caFrom,caTemp,128));
        utPltPutLoopVar(psDbHead,"to", iNum,pasCvtGBK(lCode,caTo,caTemp,128));
        utPltPutLoopVar(psDbHead,"sub", iNum,pasCvtGBK(lCode,caSub,caTemp,255));
        utPltSetCvtHtml(0);     
        utPltPutLoopVarF(psDbHead,"size", iNum,"%lu",lSize);
        utPltPutLoopVarF(psDbHead,"ccode", iNum,"%lu",lCode);
        if(lAtt > 0) {
            utPltPutLoopVar(psDbHead,"att", iNum,ncLang("0815有"));
        }
        if(lVirus == 2) {
            utPltPutLoopVar(psDbHead,"virus", iNum,ncLang("0187带毒"));
            utPltPutLoopVar(psDbHead,"virusname", iNum,caVirus);
        }
        iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_ULONG,4,&lSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lStat,
  	                               UT_TYPE_LONG,4,&lStatus1,
  	                               UT_TYPE_LONG,4,&lCode,
  	                               UT_TYPE_LONG,4,&lVirus,
  	                               UT_TYPE_STRING,31,caVirus);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"cond",caCond);
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"lCount", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    return 0;
}



/*  高级查询  */
int ncUtlMailCtlSearch(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead,char *pPlate,char *pTable)
{
    char caWhere[512],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caUserid[16];
    char caUsername[32],caTo[64],caFrom[64],caSub[128],caSdate[32];
    char caFile[128],caTemp[512],caGroupid[12];
    char caMsize[12],caAsize[12],caEdate[20],caOrder[12],caOrderby[12];
    char caDdd[12],caUnit[12],caStatus[12],caDtype[12],caBef[12];
    char caWhereStr[10];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lAtt,lStat,lSize,lUserid0,lSid;
    unsigned long lTime,l,lTime1,lCode,lSdate;
    int i,iNum,iPos;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount,lStatus1,lSumUser;
 	pasDbCursor *psCur;
 	int iCharCode=0;
	if(strcasecmp(utComGetVar_sd(psShmHead,"MailCharCode","Auto"),"Auto") == 0) {
	    iCharCode = 1;
	}
	else {
	    iCharCode = 0;
	}
 	
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncUtlMailCtlList");
 	}
   	strcpy(caWhere,"\0");
	iReturn = utMsgGetSomeNVar(psMsgHead,16,
	                "username",   UT_TYPE_STRING, 31, caUsername,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "from",       UT_TYPE_STRING, 63, caFrom,
	                "to",         UT_TYPE_STRING, 63, caTo,
	                "sub",        UT_TYPE_STRING, 63, caSub,
	                "msize",      UT_TYPE_STRING, 11, caMsize,
	                "asize",      UT_TYPE_STRING, 11, caAsize,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "orderby",    UT_TYPE_STRING, 2, caOrderby,
	                "ddd",        UT_TYPE_STRING, 11,caDdd,
	                "unit",       UT_TYPE_STRING, 11,caUnit,
	                "status",     UT_TYPE_STRING, 11,caStatus,
	                "dtype",      UT_TYPE_STRING, 11,caDtype,
	                "bef",        UT_TYPE_STRING, 11,caBef);
    lCurPg = 1;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {  /* 部门权利  */
        sprintf(caWhere," and a.groupid = %lu ",lGroupid0);
    }
    if(!utStrIsSpaces(caGroupid)) {
        utStrAddF(caWhere," and a.groupid = %s ",caGroupid);
    }
    if(!utStrIsSpaces(caUsername)) {
        utStrAddF(caWhere," and a.dispname like '%c%s%c'",'%',caUsername,'%');
    }
    if(!utStrIsSpaces(caFrom)) {
        utStrAddF(caWhere," and b.mailfrom like '%c%s%c'",'%',caFrom,'%');
    }
    if(!utStrIsSpaces(caTo)) {
        utStrAddF(caWhere," and b.mailto like '%c%s%c'",'%',caTo,'%');
    }
    if(!utStrIsSpaces(caSub)) {
        utStrAddF(caWhere," and b.mailsub like '%c%s%c'",'%',caSub,'%');
    }
    if(!utStrIsSpaces(caAsize)) {
        utStrAddF(caWhere," and b.mailasize >= %s",caAsize);
    }
    if(!utStrIsSpaces(caMsize)) {
        utStrAddF(caWhere," and b.mailsize >= %s",caMsize);
    }
    if(strcmp(caDtype,"unit") == 0) { /* 按天数计  */
        lTime = time(0);
        if(!utStrIsSpaces(caDdd)) { /* 计算几天以内  */
            l = atol(caDdd);
            if(caUnit[0] == '1') { /* 天  */
                lTime1 = lTime - l * 86400L;
            }
            else if(caUnit[0] == '2') { /* 周  */
                lTime1 = lTime - l * 7 * 86400L;
            }
            else {
                lTime1 = lTime - l * 30 * 86400L;
            }
            strcpy(caSdate,utTimFormat("%Y-%m-%d %H:%M:%S",lTime1));
            if(caBef[0] == '1') { /* 以内  */
                utStrAddF(caWhere," and b.sdate >= %lu", lTime1);
            }
            else { /* 以前  */
                utStrAddF(caWhere," and b.sdate <= '%s'",lTime1);
            }
        }            
    }
    else { /* 时间范围  */
        if(!utStrIsSpaces(caSdate)) {
            utStrAddF(caWhere," and b.sdate >= %lu",utTimStrToLong("%Y-%m-%d %H:%M:%S",caSdate));
        }
        if(!utStrIsSpaces(caEdate)) {
            strcpy(caEdate+10," 23:59:59");
            utStrAddF(caWhere," and b.sdate <= %lu",utTimStrToLong("%Y-%m-%d %H:%M:%S",caEdate));
        }
    }
    if(!utStrIsSpaces(caStatus)) {
        if(caStatus[0] == '1') {  /* 正常  */
            utStrAddF(caWhere," and b.mailstat = 1");
        }
        else if(caStatus[0]=='0') { /* 无内容  */
            utStrAddF(caWhere," and b.mailstat = 0");
        }
        else if(caStatus[0]=='2') { /* 拒绝  */
            utStrAddF(caWhere," and b.mailstat = 2");
        }
        else if(caStatus[0] =='3') { /* 转发  */
            utStrAddF(caWhere," and b.mailstat = 3");
        } 
        else if(caStatus[0] =='4') { /* 审计发送  */
            utStrAddF(caWhere," and b.mailstat = 5 and b.status = 1" );
        } 
        else if(caStatus[0] =='5') { /* 审计拒绝  */
            utStrAddF(caWhere," and b.mailstat = 5 and b.status = 2" );
        } 
        else if(caStatus[0] =='6') { /* 审计转发  */
            utStrAddF(caWhere," and b.mailstat = 5 and b.status = 3" );
        } 
        else if(caStatus[0] =='7') { /* 未处理  */
            utStrAddF(caWhere," and b.mailstat = 4 and b.status = 0" );
        }
    }       
    if(!utStrIsSpaces(caOrder)) {
        if(strcmp(caOrder,"date")==0) {
            utStrAddF(caWhere," order by b.sdate ");
        }
        else if(strcmp(caOrder,"from") == 0) {
            utStrAddF(caWhere," order by b.mailfrom ");
        }
        else if(strcmp(caOrder,"to") == 0) {
            utStrAddF(caWhere," order by b.mailto ");
        }
        else if(strcmp(caOrder,"msize") == 0) {
            utStrAddF(caWhere," order by b.mailsize ");
        }
        if(!utStrIsSpaces(caOrderby)) {
            if(caOrderby[0] == '1') {     
                utStrAddF(caWhere," desc ");
            }
            
        }
    }
    lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;
    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }
    if(strstr(caWhere,"a.")) {
        sprintf(caTemp,"select count(*) from %s b,ncuser a where a.userid = b.userid  %s",pTable,ncUtlTrimOrder(caWhere));
    }
    else {
        if(utStrIsSpaces(caWhere)) {
            sprintf(caTemp,"select count(*) from %s b ",pTable);
        }
        else {
            sprintf(caTemp,"select count(*) from %s b %s %s",pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));
        }
    }        
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
    if(iDebugLevel == NC_DEBUG_MSGINFO) {        
        utSysLog("[Count=%d]%s",lCount,caTemp);
    }
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0301记录查询出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
    
    if(utStrIsSpaces(caWhere)) {
        psCur = pasDbOpenSqlF("select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode \
                from  %s b  limit 0,%d",pTable,lRowNum);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[NoUser]select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode \
                from  %s b  limit 0,%d",pTable,lRowNum);
        }

    }
    else if(strstr(caWhere,"a.")) {
        psCur = pasDbOpenSqlF("select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode \
                from %s b,ncuser a  where b.userid = a.userid  %s limit 0,%d",pTable,caWhere,lRowNum);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[User]select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode \
                from %s b,ncuser a  where b.userid = a.userid  %s limit 0,%d",pTable,caWhere,lRowNum);
        }
    }
    else {
        psCur = pasDbOpenSqlF("select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode \
                from  %s b %s %s limit 0,%d",pTable,caWhereStr,caWhere+iPos,lRowNum);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[NoUser]select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.ccode \
                from  %s b %s %s limit 0,%d",pTable,caWhereStr,caWhere+iPos,lRowNum);
        }
    }
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
    psDbHead = utPltInitDb();
  	iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_ULONG,4,&lSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lStat,
  	                               UT_TYPE_LONG,4,&lStatus1,
  	                               UT_TYPE_LONG,4,&lCode);
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
   	    i++;
        iNum++;
        if(iCharCode == 0) {
            lCode = pasDefaultCode();
        }
        if(lStat == 0 ) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0655无内容"));
            strcpy(caFile,"\0");
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0655无内容"));
        }
        else if(lStat == 1) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0859直接发送"));
        }
        else if(lStat == 4 ) {
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0626未处理"));
        }
        else if(lStat == 5){
            if(lStatus1 == 1){
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0511审计发送"));
            }                
            else if(lStatus1 == 2){
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0512审计拒绝"));
            }                
            else if(lStatus1 == 3) {
                utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0513审计转发"));
            }                
        }
        else if(lStat == 2) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0860直接拒绝"));
        }
        else if(lStat == 3) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0861直接转发"));
        }
        else {
            strcpy(caFile,"\0");
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0655无内容"));
        }
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lSdate));
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVar(psDbHead,"file", iNum,caFile);
        utPltPutLoopVar(psDbHead,"from", iNum,pasCvtGBK(lCode,caFrom,caTemp,128));
        utPltPutLoopVar(psDbHead,"to", iNum,pasCvtGBK(lCode,caTo,caTemp,128));
        utPltPutLoopVar(psDbHead,"sub", iNum,pasCvtGBK(lCode,caSub,caTemp,255));
        utPltPutLoopVarF(psDbHead,"size", iNum,"%lu",lSize);
        if(lAtt > 0) {
            utPltPutLoopVar(psDbHead,"att", iNum,ncLang("0815有"));
        }
        utPltPutLoopVarF(psDbHead,"ccode", iNum,"%lu",lCode);
      	iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_ULONG,4,&lSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lStat,
  	                               UT_TYPE_LONG,4,&lStatus1,
  	                               UT_TYPE_LONG,4,&lCode);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    return 0;
}


/* 监控邮件查询
      查询功能进行了一些优化
 */
int ncUtlMailMonList(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead,char *pTable,char *pPlate)
{
    char caWhere[1024],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caUserid[16];
    char caUsername[32],caTo[64],caFrom[64],caSub[128],caSdate[32];
    char caFile[128],caTemp[2048],caFlags[16],caType[64],caSear[16];
    char caCount[16],caWhereStr[10];
    char caSip[16];
    long lFlags;
    char caGroupid[16];
    char caExport[32];
    long lCount,lCode;
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lAtt,lStat,lSize,lUserid0,lSid,lSumUser;
    int i,iNum,iPos;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lGroupid0,lRowNum,lflags,lStartRec;
 	pasDbCursor *psCur;
 	ncUserCont *psUser;
	char *pBuf,*pSelect;
	char caDip[16];
	int iCharCode=0;
	char caConflag[12];
	char caTitlename[32];
	int group_flag;
	int iRet;
	char caMsize[12],caAsize[12],caIdfyno[20],caIp[16],caUsername_match[8],caContent[252],caAttfile[128];
	char caEdate[20],caSdate1[16],caEdate1[16];
   iReturn = utMsgGetSomeNVar(psMsgHead,1,"con_flag",   UT_TYPE_STRING,10,caConflag);
	
	
	if(strcasecmp(utComGetVar_sd(psShmHead,"MailCharCode","Auto"),"Auto") == 0) {
	    iCharCode = 1;
	}
	else {
	    iCharCode = 0;
	}
	if(iDebugLevel == NC_DEBUG_MSGINFO) {
	    utSysPrintMsg(psMsgHead,"ncUtlMailMonList");
 	}
 	memset(caSdate,0,sizeof(caSdate));
 	memset(caEdate,0,sizeof(caEdate));
 	iReturn = utMsgGetSomeNVar(psMsgHead,10,
	                "flags",      UT_TYPE_STRING, 12, caFlags,
	                "userid",     UT_TYPE_STRING, 12, caUserid,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			            "CurPg",      UT_TYPE_STRING, 12, caCurPg,
			            "search",     UT_TYPE_STRING, 12, caSear,
   	                "cond",       UT_TYPE_STRING, 63, caCond,
   	                "lcount",     UT_TYPE_STRING, 12, caCount,
   	                "where",      UT_TYPE_STRING,255, caWhere,
   	                "exp1",       UT_TYPE_STRING, 30, caExport,
   	                "groupid",    UT_TYPE_STRING,10,caGroupid);
   	                
    iReturn = utMsgGetSomeNVar(psMsgHead,14,	               
	                "from",       UT_TYPE_STRING, 63, caFrom,
	                "to",         UT_TYPE_STRING, 63, caTo,
	                "sub",        UT_TYPE_STRING, 63, caSub,
	                "msize",      UT_TYPE_STRING, 11, caMsize,
	                "asize",      UT_TYPE_STRING, 11, caAsize,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "sdate1",     UT_TYPE_STRING, 5,  caSdate1,
	                "edate1",     UT_TYPE_STRING, 5,  caEdate1,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,             
	                "ip",         UT_TYPE_STRING,15,caIp,
	                "username_match",UT_TYPE_STRING,2,caUsername_match,
	                "content",    UT_TYPE_STRING,250,caContent,
	                "attfile",    UT_TYPE_STRING,127,caAttfile);	                
   	if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caUsername,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),"未知"));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	                          
   	                
    if(!utStrIsSpaces(caSear)) {
        strcpy(caWhere,"\0");
        lCurPg = 1;
        strcpy(caCount,"\0");
    }
    else {
        lCurPg = atol(caCurPg);
        if(lCurPg <= 0) lCurPg = 1;
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {
        	group_flag=1;
     }
     if(group_flag==1||strlen(caUserid)>0||strlen(caGroupid)>0){
     	  if(strlen(caSdate)==0)
    		strcpy(caSdate,utTimFormat("%Y-%m-%d",time(0))); 
    		if(strlen(caEdate)==0)  	
    		strcpy(caEdate,utTimFormat("%Y-%m-%d",time(0)));
    		if(strlen(caSdate1)==0)
    		strcpy(caSdate1,"00:00");
    		if(strlen(caEdate1)==0)
    		strcpy(caEdate1,"23:59");    	
    }
    if(!utStrIsSpaces(caWhere)) {
    }
    else {  
        sprintf(caCond0,"'%c%s%c'",'%',caCond,'%');
        lUserid0 = atol(caUserid); 
        if(strlen(caSdate)>0){
        	  sprintf(caWhere+strlen(caWhere)," and b.sdate>='%s %s' ",caSdate,caSdate1);
        } 
        if(strlen(caEdate)>0){
        	sprintf(caWhere+strlen(caWhere)," and b.sdate<='%s %s' ",caEdate,caEdate1);
        }
        if(lUserid0 > 0) {
           utStrAddF(caWhere," and b.userid = %lu  ",lUserid0);
        }
    	if(group_flag==1){      
     
         sprintf(caWhere+strlen(caWhere)," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu))",lGroupid0,lGroupid0);
      }
      if(atol(caGroupid)>0){
       if(strcmp(caGroupid,"0")==0)
        	utStrAddF(caWhere," and (a.groupid = %s ) ",caGroupid);
       else
          sprintf(caWhere+strlen(caWhere)," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu))",atol(caGroupid),atol(caGroupid));
      }    
       if(atol(caFlags)>0) {               
           sprintf(caWhere+strlen(caWhere)," and (b.flags = %s)",caFlags);
       }	
       if(strlen(caCond)>0){
       	utStrAddF(caWhere," and (b.sdate like %s or b.mailfrom like %s or b.mailto like %s or b.mailsub like %s) order by b.sdate desc ",
                                      caCond0,caCond0,caCond0,caCond0);
      }
     
       
    }
   if(!utStrIsSpaces(caExport))
     lRowNum=99999;
     else
     lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;    
    psDbHead = utPltInitDb();
    
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"flags",caFlags);
      
    	utPltPutVar(psDbHead,"from",caFrom);
    	utPltPutVar(psDbHead,"to",caTo);
    	utPltPutVar(psDbHead,"sub",caSub);
    	utPltPutVar(psDbHead,"msize",caMsize);
    	utPltPutVar(psDbHead,"asize",caAsize);
    	utPltPutVar(psDbHead,"idfyno",caIdfyno);
    	utPltPutVar(psDbHead,"ip",caIp);
    	utPltPutVar(psDbHead,"username_match",caUsername_match);
    	utPltPutVar(psDbHead,"content",caContent);
    	utPltPutVar(psDbHead,"attfile",caAttfile);
    	
    	
   if(strlen(caSdate)==0){
    strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
  }
  if(strlen(caEdate)==0){
    strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
  }
   if(strlen(caSdate1)==0){
   	strcpy(caSdate1,"00:00");
  }
  if(strlen(caEdate1)==0){
  	strcpy(caEdate1,"23:59");
  }
    	
    	
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"sdate1",caSdate1);
    utPltPutVar(psDbHead,"edate1",caEdate1);
    
    pSelect= pasUtlCvtInputSelect("flags",caFlags,ncLang("0454全部,0;发送邮件,1;接收邮件,2;WebMail,3;"));
    utPltPutVar(psDbHead,"sflags", pSelect);
    free(pSelect);
    
    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }


 //   if(utStrIsSpaces(caCount)) { /* 统计记录总数  */

        if( strstr(caWhere,"a.")){ /* 涉及用户信息  */
            sprintf(caTemp,"select count(*) from %s b,ncuser a %s b.userid=a.userid and  %s",
                     pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));
        }
        else {
            sprintf(caTemp,"select count(*) from  %s b %s %s",pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));
           
        }
       printf("caTemp=%s\n",caTemp); 
        iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);

        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[Count=%d]%s",lCount,caTemp);
        }
        if(iReturn != 0) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0788邮件查询"),ncLang("0583统计出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
 //  }
 //   else {
 //       lCount = atol(caCount);
 //   }
   if(strstr(caWhere,"order by")==NULL){
   	utStrAddF(caWhere," order by b.sdate desc " );
  }
printf("caWhere=%s\n",caWhere);
    if( strstr(caWhere,"a.")) { /* 涉及用户信息  */
                  
        psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),inet_ntoa(b.dip),b.flags,b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode  \
                from  %s b,ncuser a where b.userid = a.userid  %s limit %d,%d",
                    pTable,caWhere,lStartRec,lRowNum);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[User]select inet_ntoa(b.sip),b.flags,b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode  \
                from  %s b,ncuser a where b.userid = a.userid  %s limit %d,%d",pTable,caWhere,lStartRec,lRowNum);
        }
    }
    else { 

        psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),inet_ntoa(b.dip),b.flags,b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode \
                from %s b %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lRowNum);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[NoUser]select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode \
                from %s b %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lRowNum);
        }
    } 
   
    if(psCur == NULL) {
        utPltFreeDb(psDbHead);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0788邮件查询"),ncLang("0172查询数据出错 %s"),
            pasDbErrorMsg(NULL));
        return 0;
    } 
    
        iReturn = 0;
    
     if(!utStrIsSpaces(caExport)) {
        FILE *fp;
        ncUserCont *psUser;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        char fj[10];
        sprintf(caFilename,"ncmail%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
    
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0194导出"),ncLang("0183打开文件出错"));
            return 0;
        }
        ncUtlPrintCsv(fp,1,UT_TYPE_STRING,ncLang("0804邮件列表"));
        ncUtlPrintCsv(fp,7,UT_TYPE_STRING,ncLang("0694姓名"),
        		UT_TYPE_STRING,ncLang("0218发件人"),
                       UT_TYPE_STRING,ncLang("0535收件人"),
                       UT_TYPE_STRING,ncLang("0872主题"),
                       UT_TYPE_STRING,ncLang("0225发送时间"),
                       UT_TYPE_STRING,ncLang("0186大小"),
                       UT_TYPE_STRING,ncLang("0256附件"));

     memset(caSip,0,sizeof(caSip));
    lFlags=0;   
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
  	while(iReturn==0)
  	{
  		psUser = ncUtlGetUserCont(psShmHead,lUserid);
  		if(psUser)
  		strcpy(caUsername,psUser->dispname);
  		else
  		strcpy(caUsername,"");
  		
  	        if(lAtt > 0)
  	        {
            		strcpy(fj,ncLang("0815有"));
        	}
        	else
        	        strcpy(fj,ncLang("0650无"));
        	
        	
        	                iRet=pasCharIsUtf8(caSub, strlen(caSub));
                    	      if(iRet){
			                    	pasUtf8ToGBK(caSub,caTemp,255);
			                    	
			                    }
			                    else{
			                    	pasCvtGBK(lCode,caSub,caTemp,255);
			                    }
        	
        	ncUtlPrintCsv(fp,7,UT_TYPE_STRING,caUsername,
                       UT_TYPE_STRING,caFrom,
                       UT_TYPE_STRING,caTo,
                       UT_TYPE_STRING,caTemp,
                       UT_TYPE_STRING,caSdate,
                       UT_TYPE_LONG,lSize,
                       UT_TYPE_STRING,fj); 
    memset(caSip,0,sizeof(caSip));
    memset(caDip,0,sizeof(caDip));
    lFlags=0;      
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
  	}
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        
          char caUsername2[32];
			   memset(caUsername2,0,sizeof(caUsername2));
			   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
			   ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"导出邮件日志");
      
        
        
        return 0;
    }
        
     utPltSetCvtHtml(1);
     memset(caSip,0,sizeof(caSip));
     memset(caDip,0,sizeof(caDip));
     lFlags=0;  
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
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
        iNum++;
        if(iCharCode == 0) {
            lCode = pasDefaultCode();
        }
        utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
        utPltPutLoopVar(psDbHead,"dip",iNum,caDip);
        if(lFlags==2)
        	utPltPutLoopVar(psDbHead,"flags_desc",iNum,"POP3");
        else if(lFlags==1)
        	utPltPutLoopVar(psDbHead,"flags_desc",iNum,"SMTP");
        else if(lFlags==3)
          utPltPutLoopVar(psDbHead,"flags_desc",iNum,"WebMail");
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
        if(psUser){
        	utPltPutLoopVar(psDbHead,"position",iNum,psUser->position);
        }  
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVar(psDbHead,"file", iNum,caFile);
        utPltPutLoopVar(psDbHead,"from", iNum,pasCvtGBK(lCode,caFrom,caTemp,128));
        utPltPutLoopVar(psDbHead,"to", iNum,pasCvtGBK(lCode,caTo,caTemp,128));
        
                                   iRet=pasCharIsUtf8(caSub, strlen(caSub));
                    	      if(iRet){
			                    	pasUtf8ToGBK(caSub,caTemp,255);
			                    	
			                    }
			                    else{
			                    	pasCvtGBK(lCode,caSub,caTemp,255);
			                    }
        
        utPltPutLoopVar(psDbHead,"sub", iNum,caTemp);
        utPltPutLoopVarF(psDbHead,"size", iNum,"%lu",lSize);
        utPltPutLoopVarF(psDbHead,"ccode", iNum,"%lu",lCode);

        if(lAtt > 0) {
            utPltPutLoopVar(psDbHead,"att", iNum,ncLang("0815有"));
        }
        memset(caSip,0,sizeof(caSip));
        memset(caDip,0,sizeof(caDip));
        lFlags=0;
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
    }
    pasDbCloseCursor(psCur);
  utPltSetCvtHtml(0);   
    
    lTotPg = (lCount - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"cond",caCond);
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"lcount", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVar(psDbHead,"title_name",caTitlename);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    return 0;
}

/* 监控邮件的高级查询  */
int ncUtlMailMonSearch(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead,char *pPlate,char *pTable)
{
    char caWhere[512],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caUserid[16];
    char caUsername[32],caTo[64],caFrom[64],caSub[128],caSdate[32],caSdate1[32];
    char caFile[128],caTemp[512],caGroupid[12];
    char caMtype[64],caFlags[12];
    char caSdate2[20],caEdate2[20];
    char caMsize[12],caAsize[12],caEdate[20],caOrder[12],caOrderby[12],caEdate1[32];
    char caDdd[12],caUnit[12],caStatus[12],caDtype[12],caBef[12];
    char caWhereStr[10];
    char caFjflag[10];
    char caTitlename[32];
    char caSip[16];
    long lFlags;
    char caIdfyno[20];
    char caIp[20],caContent[256],caAttfile[128];
    char caSub_u[256],caContent_u[1024];
   	char ids[256];
   	int i2,lUserid2;
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lAtt,lStat,lSize,lUserid0,lSid,lSumUser;
    unsigned long lTime,l,lTime1,lCode;
    unsigned long lpsCount,lptCount,ltCount;
    char caUsername_match[8],caIdfyno_match[8];
    int i,iNum,iPos;
    utPltDbHead *psDbHead;
    char *pSelect;
   	int iReturn,group_flag;
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount,lStatus1;
 	pasDbCursor *psCur;
 	ncUserCont *psUser;
	int iCharCode=0;
	unsigned long lcurtime;
	 char caConflag[12];
	 char caDip[16];
	 int iRet;
   iReturn = utMsgGetSomeNVar(psMsgHead,1,"con_flag",   UT_TYPE_STRING,10,caConflag);
	 ncOperWriteLog(psMsgHead);
	if(strcasecmp(utComGetVar_sd(psShmHead,"MailCharCode","Auto"),"Auto") == 0) {
	    iCharCode = 1;
	}
	else {
	    iCharCode = 0;
	}
 	
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncUtlMailMonSearch");
 	}
    
 	strcpy(caWhere,"\0");
	iReturn = utMsgGetSomeNVar(psMsgHead,28,
	                "mtype",      UT_TYPE_STRING, 31, caMtype,
	                "username",   UT_TYPE_STRING, 31, caUsername,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "from",       UT_TYPE_STRING, 63, caFrom,
	                "to",         UT_TYPE_STRING, 63, caTo,
	                "sub",        UT_TYPE_STRING, 63, caSub,
	                "msize",      UT_TYPE_STRING, 11, caMsize,
	                "asize",      UT_TYPE_STRING, 11, caAsize,
	                "sdate",      UT_TYPE_STRING, 19, caSdate2,
	                "sdate1",     UT_TYPE_STRING, 5,  caSdate1,
	                "edate1",     UT_TYPE_STRING, 5,  caEdate1,
	                "edate",      UT_TYPE_STRING, 19, caEdate2,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "orderby",    UT_TYPE_STRING, 2, caOrderby,
	                "ddd",        UT_TYPE_STRING, 11,caDdd,
	                "unit",       UT_TYPE_STRING, 11,caUnit,
	                "flags",     UT_TYPE_STRING, 11,caFlags,
	                "dtype",      UT_TYPE_STRING, 11,caDtype,
	                "bef",        UT_TYPE_STRING, 11,caBef,
	                "userid",     UT_TYPE_STRING, 10,caUserid,
	                "fjflag",     UT_TYPE_STRING,8,caFjflag,
	                "CurPg",      UT_TYPE_STRING, 12, caCurPg,
	                "ip",         UT_TYPE_STRING,15,caIp,
	                "username_match",UT_TYPE_STRING,2,caUsername_match,
   	               "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
	                "content",    UT_TYPE_STRING,250,caContent,
	                "attfile",    UT_TYPE_STRING,127,caAttfile);
	   pasCharsetCvt(PAS_CCODE_GB,PAS_CCODE_UTF8,caSub,caSub_u,127);
	   pasCharsetCvt(PAS_CCODE_GB,PAS_CCODE_UTF8,caContent,caContent_u,256);
	  
	  
	   strcpy(caSdate,ncMailCvtDate998(caSdate2));
     strcpy(caEdate,ncMailCvtDate998(caEdate2)); 
	  if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caUsername,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),"未知"));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
               
	  if(atol(caConflag)>0) 
	  	lCurPg=atol(caCurPg);
	  else             
    	lCurPg = 1;
    group_flag=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {  /* 部门权利  */
    	group_flag=1;
        
    }
    
 
    
   if(strcmp(caDtype,"unit") == 0) { /* 按天数计  */
        lTime = time(0);
        if(!utStrIsSpaces(caDdd)) { /* 计算几天以内  */
            l = atol(caDdd);
            if(caUnit[0] == '1') { /* 天  */
                lTime1 = lTime - l* 86400L;
            }
            else if(caUnit[0] == '2') { /* 周  */
                lTime1 = lTime - l* 7 * 86400L;
            }
            else {
                lTime1 = lTime - l* 30 * 86400L;
            }
            strcpy(caSdate,utTimFormat("%Y-%m-%d %H:%M:%S",lTime1));
            if(caBef[0] == '1') { /* 以内  */
                utStrAddF(caWhere," and b.sdate >= '%s'",caSdate);
            }
            else { /* 以前  */
                utStrAddF(caWhere," and b.sdate <= '%s'",caSdate);
            }
        }            
    }
    else { /* 时间范围  */
    	
    	
    	if(strlen(caUsername)>0||group_flag==1||strlen(caGroupid)>0||strlen(caUserid)>0||strlen(caFrom)>0||strlen(caTo)>0||strlen(caSub)>0||strlen(caMsize)>0||strlen(caAsize)>0||strlen(caIdfyno)>0||atol(caConflag)>0||strlen(caContent)>0||strlen(caAttfile)>0||strlen(caIp)>0){
    	if(strlen(caSdate)==0){
    		strcpy(caSdate,utTimFormat("%Y-%m-%d",time(0)));
    	}
    	if(strlen(caEdate)==0){
    		strcpy(caEdate,utTimFormat("%Y-%m-%d",time(0)));
    	}
    	if(strlen(caSdate1)==0){
    		strcpy(caSdate1,"00:00");
    	}
    	if(strlen(caEdate1)==0){
    		strcpy(caEdate1,"23:59");
    	}
    }
  	
        if(!utStrIsSpaces(caSdate)) {
            utStrAddF(caWhere," and b.sdate >= '%s %s'",caSdate,caSdate1);
        }
        if(!utStrIsSpaces(caEdate)) {
           
            utStrAddF(caWhere," and b.sdate <= '%s %s'",caEdate,caEdate1);
        }
    }
    
    if(atol(caUserid)>0) {
        utStrAddF(caWhere," and b.userid = %s",caUserid);
    }
    if(group_flag==1){
    	utStrAddF(caWhere," and (a.groupid = %lu or a.groupid in (select groupid from ncgroup where pid=%lu))",lGroupid0,lGroupid0);
    }
    
    if(!utStrIsSpaces(caGroupid)) {
    	 if(strcmp(caGroupid,"0")==0)
        	utStrAddF(caWhere," and (a.groupid = %s ) ",caGroupid);
       else
        utStrAddF(caWhere," and (a.groupid = %s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
    }

    if(!utStrIsSpaces(caUsername)&&utStrIsSpaces(caUserid)) {
    	if(atol(caUsername_match)==0)
    	  utStrAddF(caWhere," and (a.dispname='%s' or a.username='%s' )",caUsername,caUsername);
    	else
        utStrAddF(caWhere," and (a.dispname like '%c%s%c' or a.username like '%c%s%c')",'%',caUsername,'%','%',caUsername,'%');
    }
    if(!utStrIsSpaces(caFrom)) {
        utStrAddF(caWhere," and b.mailfrom like '%c%s%c'",'%',caFrom,'%');
    }
    if(strlen(caIdfyno)!=0){
    	if(atol(caIdfyno_match)==0)
	       utStrAddF(caWhere," and a.idfyno = '%s' ",caIdfyno);
	      else
	       sprintf(caWhere," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
	      }

    if(!utStrIsSpaces(caTo)) {
        utStrAddF(caWhere," and b.mailto like '%c%s%c'",'%',caTo,'%');
    }
    if(!utStrIsSpaces(caSub)) {
        utStrAddF(caWhere," and (b.mailsub  like '%c%s%c' or b.mailsub like '%c%s%c' ) ",'%',caSub,'%','%',caSub_u,'%');
    }
    if(!utStrIsSpaces(caAsize)) {
        utStrAddF(caWhere," and b.mailasize >= %s",caAsize);
    }
    if(!utStrIsSpaces(caMsize)) {
        utStrAddF(caWhere," and b.mailsize >= %s",caMsize);
    }
    if(atol(caFjflag)==1) {
    	 utStrAddF(caWhere," and b.mailasize=0 ");
    	}
    	if(atol(caFjflag)==2)
    	utStrAddF(caWhere," and b.mailasize>0 ");
    	
    	
      if(!utStrIsSpaces(caIp))
    	{
    	     sprintf(caWhere+strlen(caWhere)," and b.sip=%lu ",ntohl(utComHostAddress(caIp)));
    	    
     }	
    if(strlen(caContent)>0){
    	sprintf(caWhere+strlen(caWhere)," and (b.content like '%c%s%c' or b.content like '%c%s%c') ",'%',caContent,'%','%',caContent_u,'%');
    }
    if(strlen(caAttfile)>0){
    	sprintf(caWhere+strlen(caWhere)," and b.attfile like '%c%s%c' ",'%',caAttfile,'%');
    }	
    	

    
    if(!utStrIsSpaces(caFlags)) {
        if(caFlags[0] == '1') {  /* 发送 SMTP */
            utStrAddF(caWhere," and b.flags = 1");
        }
        else if(caFlags[0]=='2') { /* 无内容 POP3 */
            utStrAddF(caWhere," and b.flags = 2");
        }
        else if(caFlags[0] == '3') {
            utStrAddF(caWhere," and b.flags = 3");    
        }
    }
    else {
 //       utStrAddF(caWhere," and b.flags > 0");
    }               
    if(!utStrIsSpaces(caMtype)) {
        utStrAddF(caWhere," and b.mtype = '%s'",caMtype);
    }
//  printf("caOrder=%s\n",caOrder);   
    if(!utStrIsSpaces(caOrder)) {
        if(strcmp(caOrder,"date")==0) {
            utStrAddF(caWhere," order by b.sdate ");
        }
        else if(strcmp(caOrder,"from") == 0) {
            utStrAddF(caWhere," order by b.mailfrom ");
        }
        else if(strcmp(caOrder,"to") == 0) {
            utStrAddF(caWhere," order by b.mailto ");
        }
        else if(strcmp(caOrder,"msize") == 0) {
            utStrAddF(caWhere," order by b.mailsize ");
        }
        else if(strcmp(caOrder,"name") == 0) {
            utStrAddF(caWhere," order by b.userid ");
        }
        else{
        	 utStrAddF(caWhere," order by b.sdate desc ");
        }
        if(!utStrIsSpaces(caOrderby)) {
            if(caOrderby[0] == '1') {     
                utStrAddF(caWhere," desc ");
            }
            
        }
        else{
        	utStrAddF(caWhere," desc ");
        }
        
    }
    else
        utStrAddF(caWhere," order by b.sdate desc ");
        
    lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;  

    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }
    
 /*
    
 if((atol(caConflag)>0)&&(strstr(caWhere,"like")==NULL)&&(strstr(caWhere,"b.mailsize")==NULL)&&(strstr(caWhere,"b.mailasize")==NULL)&&(strstr(caWhere,"b.flags")==NULL)&&(strstr(caWhere,"userid")==NULL)&&(strstr(caWhere,"groupid")==NULL))
{
	sprintf(caTemp,"select count(*) from ncmailmon ");
	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&ltCount);
	
	sprintf(caTemp,"select mailstot+mailrtot+mailwtot,mailstoday+mailrtoday+mailwtoday from ncstattoday ");
	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lpsCount,
	                        UT_TYPE_LONG,4,&lptCount);
	lCount=lptCount+ltCount-lpsCount;
	strcpy(caWhereStr,"\0");
	strcpy(caWhere,"\0");
	iPos=0;
//	printf("lCount=%lu\n",lCount);
}
*/ 
//else
{    
    
       
    
    if(strstr(caWhere,"a.")) {
        sprintf(caTemp,"select count(*) from %s b,ncuser a where b.userid = a.userid %s",pTable,ncUtlTrimOrder(caWhere));
    }
    else {
        if(utStrIsSpaces(caWhere)) {
            sprintf(caTemp,"select count(*) from %s b ",pTable);
        }
        else {
            sprintf(caTemp,"select count(*) from %s b %s %s",pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));
        }
    }
  #ifdef LDEBUG
	  printf("caTemp=%s\n",caTemp);
	#endif       
printf("caTemp=%s\n",caTemp);


//printf("caTemp=%s\n",caTemp);
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
    if(iDebugLevel == NC_DEBUG_MSGINFO) {        
        utSysLog("[Count=%d]%s",lCount,caTemp);
    }
    
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0301记录查询出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
}


    if(utStrIsSpaces(caWhere)) {
        psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),b.flags,b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode \
                from  %s b  order by b.sid desc limit %d,%d",pTable,lStartRec,lRowNum);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[NoUser]select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status \
                from  %s b  limit 0,%d",pTable,lRowNum);
        }

    }
    else if(strstr(caWhere,"a.")) {
        psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),inet_ntoa(b.dip),b.flags,b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode \
                from %s b,ncuser a  where b.userid = a.userid  %s limit %d,%d",pTable,caWhere,lStartRec,lRowNum);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[User]select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status \
                from %s b,ncuser a  where b.userid = a.userid  %s limit 0,%d",pTable,caWhere,lRowNum);
        }
    }
    else {
        psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),inet_ntoa(b.dip),b.flags,b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode \
                from  %s b %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lRowNum);
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[NoUser]select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status \
                from  %s b %s %s limit 0,%d",pTable,caWhereStr,caWhere+iPos,lRowNum);
        }
    }
    
//   printf("select inet_ntoa(b.sip),inet_ntoa(b.dip),b.flags,b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode \
//                from %s b,ncuser a  where b.userid = a.userid  %s limit %d,%d\n",pTable,caWhere,lStartRec,lRowNum); 
    
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
    psDbHead = utPltInitDb();
    
    utPltPutVar(psDbHead,"dispname",caUsername);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"from",caFrom);
    utPltPutVar(psDbHead,"to",caTo);
    utPltPutVar(psDbHead,"sub",caSub);
    utPltPutVar(psDbHead,"msize",caMsize);
    utPltPutVar(psDbHead,"asize",caAsize);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"sdate1",caSdate1);
    utPltPutVar(psDbHead,"edate1",caEdate1);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"flags",caFlags);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"ip",caIp);
    utPltPutVar(psDbHead,"username_match",caUsername_match);
    utPltPutVar(psDbHead,"content",caContent);
    utPltPutVar(psDbHead,"attfile",caAttfile);
    utPltPutVar(psDbHead,"osort",caOrder);
    
    memset(caSip,0,sizeof(caSip));
    memset(caDip,0,sizeof(caDip));
    lFlags=0;
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
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum&& iNum < lCount) {
        if(iCharCode == 0) {
            lCode = pasDefaultCode();
        }

        iNum++;
        utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
        utPltPutLoopVar(psDbHead,"dip",iNum,caDip);
        if(lFlags==1)
        	utPltPutLoopVar(psDbHead,"flags_desc",iNum,"SMTP");
        else if(lFlags==2)
        	utPltPutLoopVar(psDbHead,"flags_desc",iNum,"POP3");
        else if(lFlags==3)
          utPltPutLoopVar(psDbHead,"flags_desc",iNum,"WebMail");
         
        psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
        if(psUser){
        	utPltPutLoopVar(psDbHead,"position",iNum,psUser->position);
        }   
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVar(psDbHead,"file", iNum,caFile);
        
        utPltSetCvtHtml(1); 
        utPltPutLoopVar(psDbHead,"from", iNum,pasCvtGBK(lCode,caFrom,caTemp,128));
        utPltPutLoopVar(psDbHead,"to", iNum,pasCvtGBK(lCode,caTo,caTemp,128));
        ncUtlStrCvtCtl(caSub);
        
                           iRet=pasCharIsUtf8(caSub, strlen(caSub));
                    	      if(iRet){
			                    	pasUtf8ToGBK(caSub,caTemp,255);
			                    	
			                    }
			                    else{
			                    	pasCvtGBK(lCode,caSub,caTemp,255);
			                    }
        
        utPltPutLoopVar(psDbHead,"sub", iNum,caTemp);
        utPltSetCvtHtml(0); 
        utPltPutLoopVarF(psDbHead,"size", iNum,"%lu",lSize);
        utPltPutLoopVarF(psDbHead,"ccode", iNum,"%lu",lCode);
        if(lAtt > 0) {
            utPltPutLoopVar(psDbHead,"att", iNum,ncLang("0815有"));
        }

    lFlags=0;
    memset(caSip,0,sizeof(caSip));
    memset(caDip,0,sizeof(caDip));
    lFlags=0;
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
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
//    pSelect= pasUtlCvtInputSelect("flags",caFlags,ncLang("0448全部, ;发送邮件,1;接收邮件,2;WebMail,3;"));
pSelect= pasUtlCvtInputSelect("flags",caFlags,"全部, ;POP3,2;SMTP,1;WebMail,3;");
    utPltPutVar(psDbHead,"sflags", pSelect);
    free(pSelect);
    
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVarF(psDbHead,"lcount", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVar(psDbHead,"title_name",caTitlename);
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    return 0;
}




/* 查询IP的历史记录 
     2003/1/6 对查询进行了优化
 */

int ncUtilQyeIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead,char *pTable,char *pPlate)
{
		ncService *psService;
	  unsigned long lId_s,group_flag;
	  char caName_s[32];
    char caTemp[255];
    char sqlbuf[1024];
    pasDbCursor *psCur;
    char *pBuf,*pSelect;
    char caTotPg[16];
    char caDel[7];
    char caDelwhere[512];
    int     lSumUser;
    int     i,iReturn,iNum,iMark1,iMark2,iPos;
    long    lGroupid0,lCount;
    char caSdate[32],caEdate[32],caUserid[32],caUsername[36],caCurPg[16],caIp[20],caService[12];
    char caDispname[32],caGroupname[32],caGroupid[32];
    unsigned long lUserid,lStart,lUbytes,lDbytes,lConntime,lService,lSip,lDip;
    char caSmt[512],caSip[16],caDip[16],caStime[12],caCount[16];
    char caWhere[512];
    long lCurPg,lRowNum,lStartRec,lTotPg,lPort,lType;
    utPltDbHead *psDbHead;
    unsigned long lCurtime,lScurtime;
    float lRat;
    unsigned long lStatrow,lMintime,lMaxtime,lSum,lStarttime,lEndtime,lMaxin,lMinin,lIn,lVtime;
    char caWhereStr[10];
    char caExport[16];
    char caTitlename[32];
    char temp_group[32];
    char caDdd[12],caUnit[12],caStatus[12],caDtype[12],caBef[12];
    char caSport[10],caDport[10],caMac[20],caProt[12];
    char caStime0[6];
    char caEtime[6];
    char caByt[10];
    char byt_in[2];
    char caIdfyno[20],caUsername_match[8];
    char byt_rflag[2];
    ncUserCont *psUser;
    strcpy(sqlbuf,"\0");
   
#ifdef LDEBUG
        printf(" pid== %d \n",getpid());
        utMsgPrintMsg(psMsgHead);
#endif
  lCurtime=time(0);
//  lScurtime=lCurtime-2592000;
    strcpy(caSdate,utTimFormat("%Y/%m/%d 00:00",time(0)));
    strcpy(caEdate,utTimFormat("%Y/%m/%d 23:59",time(0)));
    lScurtime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate);
    lCurtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate);
   	iReturn = utMsgGetSomeNVar(psMsgHead,11,
   	      "TotRec",     UT_TYPE_STRING, 12,  caCount,
   	       "userid",     UT_TYPE_STRING, 12,  caUserid,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"TotPg",      UT_TYPE_STRING, 12,  caTotPg,
					"where",      UT_TYPE_STRING, 255, caWhere,
					"groupid",    UT_TYPE_STRING, 12,  caGroupid,
					"groupname",  UT_TYPE_STRING, 31,  caGroupname,
					"dispname",   UT_TYPE_STRING, 31,  caDispname,
					"Del",        UT_TYPE_STRING,  6,  caDel,
					"caDelwhere", UT_TYPE_STRING,  500,caDelwhere,
					"export",     UT_TYPE_STRING,  10,caExport);


    lUserid = atol(caUserid);
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
    	strcpy(caDispname,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }
    lCurPg = atol(caCurPg);
    if(lCurPg <=1 || utMsgGetVar_s(psMsgHead,"search.x")) {
        lCurPg = 1;
        strcpy(caCount,"\0");
    }
    if(utStrIsSpaces(caExport)) {
        lRowNum = 32;
    }
    else {    
        lRowNum = 99999999L;
        lStatrow=99999999L;
    }
    lStartRec = (lCurPg - 1) * lRowNum;
    if(lStartRec <= 0) lStartRec=0;


   lCount=0;	 	
	 lStatrow=32;
	 			


    if(utStrIsSpaces(caWhere)) { 
    	  group_flag=0;
        iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
        if(iReturn==0&&lGroupid0>0){
        	group_flag=1;
        }
        
       if(group_flag==1||(strlen(caGroupid)>0)||(strlen(caUserid)>0)){
	        utStrAddF(caWhere," and b.vtime>%lu and b.vtime<=%lu ",lScurtime,lCurtime); 
	        lStarttime=lScurtime;
	        lEndtime=lCurtime;
	      }     
    
        
        if(group_flag==1) {  /* 部门权利  */
        	
            sprintf(caWhere+strlen(caWhere)," and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu ))",lGroupid0,lGroupid0);
        }
        
        lUserid = atol(caUserid);
        if(!utStrIsSpaces(caUserid) && lUserid > 0) {
            utStrAddF(caWhere," and b.userid = %s",caUserid);
        }  
      
        if(!utStrIsSpaces(caGroupid)) {
        	  if(strcmp(caGroupid,"0")==0)
        	     utStrAddF(caWhere," and (b.groupid = %s ) ",caGroupid);
        	  else
            	utStrAddF(caWhere," and (b.groupid = %s or b.groupid in (select groupid from ncgroup where pid=%s)) ",caGroupid,caGroupid);
        }


    }

    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }

    if((utStrIsSpaces(caExport))&&(strstr(caWhere,"and")==NULL)){ 
    	 lStatrow=32;
        sprintf(caTemp,"select count(*) from  %s b %s %s",pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));   
        iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0053IP查询"),ncLang("0550数据库出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
      }
      else{
      	  lStatrow=atol(utComGetVar_sd(psShmHead,"statrow","10000"));
      }

	  if(strstr(caWhere,"order by")==NULL)
	        utStrAddF(caWhere," order by b.vtime desc ");       
	
	   if(strncmp(caWhere," and",4) == 0) {
	        iPos = 4;
	        strcpy(caWhereStr,"where");
	    }
	    else {
	        iPos = 0;
	        strcpy(caWhereStr,"\0");
	    }
   if(!utStrIsSpaces(caExport)){
        lRowNum = 99999;
        lStatrow=99999;
        lStartRec=0;
      }
   
       if(strstr(caWhere,"a.")){
       	
        	
       	        psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype \
          from %s b,ncuser a %s b.userid=a.userid %s limit %d,%d",pTable,caWhereStr,caWhere,lStartRec,lStatrow);
      }
      else{ 
      	
        psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype \
          from %s b %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
        }
  //      printf("caWhere=%s\n",caWhere);
#ifdef LDEBUG
          printf("[NoUser]select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
                 b.conntime,b.service,b.port,b.trantype \
                 from %s b %s %s limit %d,%d\n",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
#endif
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }


	if(!utStrIsSpaces(caExport)){
		FILE *fp;
		char caFilename[32];
		char caFile[128];
		char caPath[128];
		    sprintf(caFilename,"ncIP%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0065IP访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,ncLang("0066IP访问明细表\n"));
// printf("aaaaaaaaa\n");       
        
        
        ncWebExportIp(psShmHead,psCur,fp);
//printf("bbbbbbb\n");
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        remove(caFile);
        
         char caUsername2[32];
			   memset(caUsername2,0,sizeof(caUsername2));
			   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
			   ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"导出IP访问日志");
      
        
        return 0;
    }

    /* 显示服务信息  */
    psDbHead = utPltInitDb();
    
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    if(strstr(caWhere,"b.vtime")){
    	utPltPutVar(psDbHead,"sdate",utTimFormat("%Y/%m/%d",lScurtime));
//    	utPltPutVar(psDbHead,"stime",utTimFormat("%H:%M",lScurtime));
utPltPutVar(psDbHead,"stime","00:00");
    	
    	utPltPutVar(psDbHead,"edate",utTimFormat("%Y/%m/%d",lCurtime));
    	utPltPutVar(psDbHead,"etime","23:59");
  //  	utPltPutVar(psDbHead,"etime",utTimFormat("%H:%M",lCurtime));
    }
    if(!utStrIsSpaces(caDispname)) {
        utPltPutVar(psDbHead,"dispname", caDispname);
    }
    iReturn = 0;
    lIn=0;
   
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lVtime,
    															 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lUbytes,
                                   UT_TYPE_LONG,  4, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
    iNum = 0;                                   
    while(iReturn == 0 ) {
    	
    	lIn++;
    
    	if(iNum<lRowNum){
	        iNum++;
	       
        utPltSetCvtHtml(1);     
        utPltPutLoopVar(psDbHead,"dispname", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltSetCvtHtml(0);  
        
        psUser = ncUtlGetUserCont(psShmHead,lUserid); 	        
	        if(psUser){
	        	utPltPutLoopVar(psDbHead,"mac",iNum,ncCvtMac(psUser->mac));
	        }
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid);  
        utPltPutLoopVar(psDbHead,"sip",      iNum,utComHostIp(htonl(lSip)));
        utPltPutLoopVar(psDbHead,"dip",      iNum,utComHostIp(htonl(lDip)));
        utPltPutLoopVar(psDbHead,"sdate",    iNum,caSdate);
        utPltPutLoopVar(psDbHead,"stime",    iNum,caStime);
        utPltPutLoopVar(psDbHead,"conn",    iNum,utStrLtoF(lConntime,caTemp));
        utPltPutLoopVar(psDbHead,"ubytes",  iNum,utStrLtoF(lUbytes,caTemp));
        utPltPutLoopVar(psDbHead,"dbytes",   iNum,utStrLtoF(lDbytes,caTemp));
        if(lService == 0) {
            utPltPutLoopVar(psDbHead,"service",  iNum,"&nbsp;&nbsp;");
        }
        else {
            utPltPutLoopVar(psDbHead,"service",  iNum,ncCvtSetService(psShmHead,lService));
        }
        utPltPutLoopVarF(psDbHead,"dport",    iNum,"%lu",lPort);
        if(lType == 6) {
            utPltPutLoopVar(psDbHead,"prot",   iNum,"TCP");
        }
        else {
            utPltPutLoopVar(psDbHead,"prot",   iNum,"UDP");
        } 
      }                       
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lVtime,
        														UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lUbytes,
                                   UT_TYPE_LONG,  4, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
    }
     
    if(strstr(caWhere,"and")&&(lCount==0)){   	
    lCount=lIn+lStartRec;
    }
    
    
    	iReturn = utMsgGetSomeNVar(psMsgHead,30,
					        "mac",        UT_TYPE_STRING,18, caMac,
	                "sip",        UT_TYPE_STRING, 31, caSip,
	                "dip",        UT_TYPE_STRING, 31, caDip,
	                "dport",      UT_TYPE_STRING, 15, caDport,
	                "prot",       UT_TYPE_STRING, 11, caProt,
	                "service",       UT_TYPE_STRING, 11, caService,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "bef",        UT_TYPE_STRING, 11, caBef,
	                "stime",	  UT_TYPE_STRING, 5,caStime0,
	                "etime",      UT_TYPE_STRING, 5,caEtime,
	                "byt_rflag",  UT_TYPE_STRING, 1,byt_rflag,
	                "byt_in",     UT_TYPE_STRING, 1,byt_in,
	                "byt",        UT_TYPE_STRING, 9,caByt,	               
	                "username_match",UT_TYPE_STRING,2,caUsername_match);
   	             
    utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"dip",caDip);
    utPltPutVar(psDbHead,"dport",caDport);
    utPltPutVar(psDbHead,"prot",caDport);
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"bef",caBef);
    utPltPutVar(psDbHead,"stime",caStime0);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"byt_rflag",byt_rflag);
    utPltPutVar(psDbHead,"byt_in",byt_in);
    utPltPutVar(psDbHead,"byt",caByt);
    utPltPutVar(psDbHead,"username_match",caUsername_match);
    
    
    
    lTotPg = (lCount -1 ) / lRowNum + 1;
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    if(!utStrIsSpaces(caUserid)) {
        utPltPutVar(psDbHead,"userid", caUserid);
        utPltPutVar(psDbHead,"dispname", caDispname);
        utPltPutVar(psDbHead,"name", caDispname);
    }
    if(!utStrIsSpaces(caGroupname)) {
        utPltPutVar(psDbHead,"name", caGroupname);
    }
    if(!utStrIsSpaces(caWhere)) {
        utPltPutVar(psDbHead,"where", caWhere);
        utPltPutVar(psDbHead,"caDelwhere",caDelwhere);
    }        
    pasDbCloseCursor(psCur);
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    for(i=0;i<ncPatGetSumService();i++) {
           utPltPutLoopVarF(psDbHead,"id_s",i+1,"%lu",ncUtlGetServiceIdByNum(psShmHead,i));
           utPltPutLoopVar(psDbHead,"name_s",i+1,ncUtlGetServiceByNum(psShmHead,i));
    }
    
   //输出网络服务大类
long lCgid;
char caName[32];
     	psCur = pasDbOpenSqlF("select groupid,name  from ncservicecgroup   order by groupid ");
                    
	    if(psCur == NULL) {
	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
	        return 0;
	    }
     
    lCgid=0;
    memset(caName,0,sizeof(caName));
    iNum=0;
	    iReturn = pasDbFetchInto(psCur,     	                           
	      	                           UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
	  	                             
   	
   	while(iReturn == 0 ||iReturn==1405) {
   		
    utPltPutLoopVarF(psDbHead,"cgid",iNum+1,"%d",lCgid);
    utPltPutLoopVar(psDbHead,"cgname",iNum+1,caName);
   	
   	iNum++;
   		
    lCgid=0;
    memset(caName,0,sizeof(caName));
	    iReturn = pasDbFetchInto(psCur,     	                           
	      	                           UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
        }  
    pasDbCloseCursor(psCur); 
    
    
    utPltPutVar(psDbHead,"title_name",caTitlename);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    return 0;
}

/* 导出上网纪录  */
int ncWebExportIp(utShmHead *psShmHead, pasDbCursor *psCur,FILE *fp)
{
    int iReturn;
    char temp[100];
    ncUserCont *psUser;
    char caTemp[32];
    char caUsername[32],caDispname[32],caGroupname[32];
    char caType[64],caName[128];
    char caDate[21];
    char caSdate[13],caStime[13];
    unsigned long lVtime;
    unsigned long lSip,lUserid,lBytes,lDip,lUbytes,lDbytes,lConntime,lService,lPort,lType;
    int iCount;
    iCount=0;
    ncUtlPrintCsv(fp,11,UT_TYPE_STRING,ncLang("0711序号"),
                       UT_TYPE_STRING,ncLang("0693姓  名"),
                       UT_TYPE_STRING,ncLang("0821源地址"),
                       UT_TYPE_STRING,ncLang("0398目标地址"),
                       UT_TYPE_STRING,ncLang("0686协  议"),
                       UT_TYPE_STRING,ncLang("0237访问端口"),
                       UT_TYPE_STRING,ncLang("0253服务"),
                       UT_TYPE_STRING,ncLang("0241访问时间"),
                       UT_TYPE_STRING,ncLang("0505上行流量"),
                       UT_TYPE_STRING,ncLang("0678下行流量"));
                      
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lVtime,
    															 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lUbytes,
                                   UT_TYPE_LONG,  4, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
    while(iReturn == 0) {
        iCount++;           
        strcpy(caDispname, ncUtlBufGetUsername(psShmHead,lUserid,"\0"));          
        sprintf(caDate,"%s %s",caSdate,caStime);
		if(lType==6){
			strcpy(caType,"TCP");
		}
		else {
			strcpy(caType,"UDP");
		}  

    	strcpy(temp,utComHostIp(htonl(lSip)));
        ncUtlPrintCsv(fp,10,UT_TYPE_LONG,iCount,
                       UT_TYPE_STRING,caDispname,
                       UT_TYPE_STRING,temp,
                       UT_TYPE_STRING,utComHostIp(htonl(lDip)),
                       UT_TYPE_STRING,caType,
                       UT_TYPE_LONG,lPort,
                       UT_TYPE_STRING,ncCvtSetService(psShmHead,lService),
                       UT_TYPE_STRING,caDate,
                       UT_TYPE_LONG,lUbytes,
                       UT_TYPE_LONG,lDbytes);
                      
   iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lVtime,
   																 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lUbytes,
                                   UT_TYPE_LONG,  4, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);

    }
    return 0;
}


/* IP高级查询  */


int ncUtilIpHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead,char *pTable,char *pPlate)
{

	  char caTitlename[64];
	  int group_flag;
	  ncService *psService;
	  unsigned long lId_s;
	  char caName_s[32];
    char caWhere[512];
    char caStime0[6];
    char caEtime[6];
    char caStemptime[17];
    char caEtemptime[17];
    char caUserid[17];
    char byt_rflag[2];
    long lstime=0;
    long letime=0;
    char caByt[10];
    char byt_in[2];
    char cadel[17];
    unsigned long lByt=0;
    char caTotPg[16],caCurPg[16];
    char caUsername[32],caSdate[32];
    char caTemp[1024],caGroupid[12],caMac[20];
    char caEdate[20],caOrder[12],caOrderby[12];
    char caDdd[12],caUnit[12],caStatus[12],caDtype[12],caBef[12];
    char caSip[16],caDip[16],caSport[10],caDport[10];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0;
    unsigned long lTime,l,lTime1,lSip,lDip;
    int i,iNum;
    utPltDbHead *psDbHead;
   	int iReturn,iPos;
   	char caDispname[32],caProt[12],caService[12];
   	char caStime[32];
   	char caIdfyno[20];
    char caIp[20];
   	char ids[256];
   	int i2,lUserid2;
   	char caUsername_match[8],caIdfyno_match[8],caIp_match[8],caMac_match[8],caDip_match[8];
   	float lRat;
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount,lStatus1;
	int     lSumUser;
	char sqlbuf[2048]="";
	unsigned long lStatrow,lMintime,lMaxtime,lSum,lStarttime,lEndtime,lMaxin,lMinin,lIn,lVtime;
   	unsigned long lUbytes,lDbytes,lConntime,lService,lPort,lType;
    char caWhereStr[10];
  ncUserCont *psUser;
 	pasDbCursor *psCur;
 	char caDelwhere[512];
    strcpy(caDelwhere,"\0");
 	strcpy(caWhere,"\0");
	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncUtilIpHSearch");
 	}
 	
 	lStatrow=atol(utComGetVar_sd(psShmHead,"statrow","10000"));
 	lSum=0;
 	lMintime=0;
 	lMaxtime=0;
// 	pasDbOneRecord("select count(*),min(vtime),max(vtime) from nciplog",0,
// 												UT_TYPE_LONG,4,&lSum,
// 												UT_TYPE_LONG,4,&lMintime,
// 												UT_TYPE_LONG,4,&lMaxtime);
 	
	iReturn = utMsgGetSomeNVar(psMsgHead,30,
					        "mac",        UT_TYPE_STRING,18, caMac,
					        "userid",     UT_TYPE_STRING,14,caUserid,
	                "dispname",   UT_TYPE_STRING, 31, caDispname,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "sip",        UT_TYPE_STRING, 31, caSip,
	                "dip",        UT_TYPE_STRING, 31, caDip,
	                "dport",      UT_TYPE_STRING, 15, caDport,
	                "prot",       UT_TYPE_STRING, 11, caProt,
	                "service",       UT_TYPE_STRING, 11, caService,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "orderby",    UT_TYPE_STRING, 2, caOrderby,
	                "ddd",        UT_TYPE_STRING, 11,caDdd,
	                "unit",       UT_TYPE_STRING, 11,caUnit,
	                "bef",        UT_TYPE_STRING, 11, caBef,
	                "dtype",      UT_TYPE_STRING, 11,caDtype,
	                "stime",	  UT_TYPE_STRING, 5,caStime0,
	                "etime",      UT_TYPE_STRING, 5,caEtime,
	                "byt_rflag",  UT_TYPE_STRING, 1,byt_rflag,
	                "byt_in",     UT_TYPE_STRING, 1,byt_in,
	                "byt",        UT_TYPE_STRING, 9,caByt,
	                "ip",         UT_TYPE_STRING,15,caIp,
	                "del",        UT_TYPE_STRING,16,cadel,
	                "username_match",UT_TYPE_STRING,2,caUsername_match,
   	              "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
   	              "ip_match",    UT_TYPE_STRING,2,caIp_match,
   	              "mac_match",   UT_TYPE_STRING,2,caMac_match,
   	              "dip_match",   UT_TYPE_STRING,2,caDip_match);
   	              
   utStrDelSpaces(caIp);           
   utStrDelSpaces(caDip);  
   utStrDelSpaces(caMac);           
   utStrDelSpaces(caDispname);
    
   	              
 if(atol(caUserid)>0)
    strcpy(caDispname,ncUtlBufGetUsername(psShmHead,atol(caUserid)));  	              
                 	              
	strcpy(caStemptime,"\0");
	strcpy(caEtemptime,"\0");
	lByt=atol(caByt);
	lstime=0;
	letime=0;
	if(!utStrIsSpaces(caSdate)){
		if(utStrIsSpaces(caStime0)) {
			strcpy(caStime0,"00:00");
		}
		sprintf(caStemptime,"%s %s",caSdate,caStime0);
		lstime=utTimStrToLong("%Y/%m/%d %H:%M",caStemptime);
	}
	if(!utStrIsSpaces(caEdate)){
		if(utStrIsSpaces(caEtime)) {
			strcpy(caEtime,"00:00");
		}
		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		letime=utTimStrToLong("%Y/%m/%d %H:%M",caEtemptime);
	}	
	
	if(lstime==0)
		lStarttime=lMintime;
	else
	  lStarttime=lstime;
	  
	if(letime==0)
	  lEndtime=lMaxtime;
	 else
	  lEndtime=letime;
	
	
    lCurPg = 1;
    group_flag=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0&&(strcmp(cadel,"del")!=0)) { 
    	  group_flag=1;
       
    }
    
    memset(caWhere,0,sizeof(caWhere));
    if(group_flag==1||strlen(caUserid)>0||strlen(caGroupid)>0||strlen(caDispname)>0||strlen(caMac)>0||strlen(caSip)>0||strlen(caDip)>0||strlen(caDport)>0||strlen(caProt)>0||strlen(caService)>0||strlen(caIdfyno)>0){
    	if(strlen(caSdate)==0){
    		strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
    		strcpy(caStime0,"00:00");
    		sprintf(caStemptime,"%s %s",caSdate,caStime0);
		    lstime=utTimStrToLong("%Y-%m-%d %H:%M",caStemptime);
    	}
    	if(strlen(caEdate)==0){
    		strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
    		strcpy(caEtime,"23:59");
    		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		    letime=utTimStrToLong("%Y-%m-%d %H:%M",caEtemptime);
    	}
    }
    
   
   
    if(strcmp(caDtype,"unit") == 0) { /* 按天数计  */
        lTime = time(0);
        if(!utStrIsSpaces(caDdd)) { /* 计算几天以内  */
            l = atol(caDdd);
            if(caUnit[0] == '1') { /* 天  */
                lTime1 = lTime - l* 86400L;
            }
            else if(caUnit[0] == '2') { /* 周  */
                lTime1 = lTime - l* 7 * 86400L;
            }
            else {
                lTime1 = lTime - l* 30 * 86400L;
            }
 //           strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime1));
            if(caBef[0] == '1') { /* 以内  */
                utStrAddF(caWhere," and b.vtime >= %lu ",lTime1);
                utStrAddF(caDelwhere," and vtime >= %lu ",lTime1);
            }
            else { /* 以前  */
                utStrAddF(caWhere," and b.vtime <= %lu ",lTime1);
                utStrAddF(caDelwhere," and vtime <= %lu ",lTime1);
            }
        }            
    }
    else { /* 时间范围  */
        if(!utStrIsSpaces(caSdate)) {
            utStrAddF(caWhere," and b.vtime >= %lu ",lstime);
            utStrAddF(caDelwhere," and vtime >= %lu ",lstime);
        }
        if(!utStrIsSpaces(caEdate)) {
            utStrAddF(caWhere," and b.vtime <= %lu ",letime);
            utStrAddF(caDelwhere," and vtime <= %lu ",letime);
        }
    }
    
    if(strlen(caUserid)==0){
	    if((!utStrIsSpaces(caDispname))&&(atol(caUsername_match)==0)) {
	    	lUserid=0;
	    	sprintf(sqlbuf,"select userid from ncuser where (dispname = '%s' or username='%s') ",caDispname,caDispname);
	    	pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
	        utStrAddF(caWhere," and b.userid=%lu ",lUserid);
	        
	       if(lUserid>0) sprintf(caUserid,"%lu",lUserid);
	       
	    }
       if((!utStrIsSpaces(caDispname))&&(atol(caUsername_match)==1)) {
	        sprintf(caWhere+strlen(caWhere)," and (a.username like '%c%s%c' or a.dispname like '%c%s%c') ",'%',caDispname,'%','%',caDispname,'%');
	       
	    }
	    if(!utStrIsSpaces(caMac)) {
	    	if(atol(caMac_match)==0){
		    	lUserid=0;
		    	sprintf(sqlbuf,"select userid from ncuser where mac='%s' ",caMac);
//		    	printf("sqlbuf=%s\n",sqlbuf);
		    	 pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
		        utStrAddF(caWhere," and b.userid=%lu ",lUserid);	      
	     }
	     else{
	     		sprintf(caWhere+strlen(caWhere)," and a.mac like '%c%s%c' ",'%',caMac,'%');
	    }
	   }
	  }
	  else{
	  	if(atol(caUserid)>0){
	  	  utStrAddF(caWhere," and b.userid=%lu ",atol(caUserid));
	  	}
	  }
    
    
    
     if(group_flag==1){
     	utStrAddF(caWhere," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu))",lGroupid0,lGroupid0);
     	
    }
    if((!utStrIsSpaces(caGroupid))&&(strcmp(cadel,"del")!=0)) {
    	  if(strcmp(caGroupid,"0")==0)
        	 utStrAddF(caWhere," and (a.groupid = %s ) ",caGroupid);
        else
        utStrAddF(caWhere," and (a.groupid = %s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
    }
    
    if((strlen(caIdfyno)!=0)&&(atol(caIdfyno_match)==0)){
    		lUserid=0;
	    	sprintf(sqlbuf,"select userid from ncuser where idfyno = '%s' ",caIdfyno);
	    	pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
	      utStrAddF(caWhere," and b.userid=%lu ",lUserid);
	 }
     if((strlen(caIdfyno)!=0)&&(atol(caIdfyno_match)==1)){
     	sprintf(caWhere+strlen(caWhere)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
    }
    

    if(!utStrIsSpaces(caSip)) {
    	if(atol(caIp_match)==0)
       utStrAddF(caWhere," and b.sip  =  %lu", ntohl(utComHostAddress(caSip)));
      else
      sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caSip,'%');
      
    }
    if(!utStrIsSpaces(caDip)) {
    	if(atol(caDip_match)==0)
        utStrAddF(caWhere," and b.dip = %lu",ntohl(utComHostAddress(caDip)));
      else
        sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.dip) like '%c%s%c' ",'%',caDip,'%');
      
    }
    if(!utStrIsSpaces(caDport)) {
        utStrAddF(caWhere," and b.port = %s",caDport);
        utStrAddF(caDelwhere," and port = %s",caDport);
    }
    
    
    if(!utStrIsSpaces(caService)) {
    	if(atol(caService)==2000){
    		utStrAddF(caWhere," and b.service>=2000 and b.service<=2999");
    	}
    	else if(atol(caService)==1000){
    		utStrAddF(caWhere," and b.service>=1000 and b.service<=1999 ");
    	}
    	else if(atol(caService)==3000){
    		utStrAddF(caWhere," and b.service>=3000 and b.service<=3999 ");
    	}
    	else if(atol(caService)==4000){
    		utStrAddF(caWhere," and b.service>=4000 and b.service<=4999 ");
    	}
    	else if(atol(caService)==7000){
    		utStrAddF(caWhere," and b.service>=7000 and b.service<=7999 ");
    	}
    	else if(atol(caService)==8000){
    		utStrAddF(caWhere," and b.service>=8000 and b.service<=8999 ");
    	}
    	else if(atol(caService)==9000){
    		utStrAddF(caWhere," and b.service>=9000 and b.service<=9999 ");
    	}
    	else if(atol(caService)==10000){
    		utStrAddF(caWhere," and b.service>=10000 and b.service<=10999 ");
    	}
    	else if(atol(caService)==11000){
    		utStrAddF(caWhere," and b.service>=11000 and b.service<=11999 ");
    	}
    	else if(atol(caService)==12000){
    		utStrAddF(caWhere," and b.service>=12000 and b.service<=12999 ");
    	}
    	else if(atol(caService)==13000){
    		utStrAddF(caWhere," and b.service>=13000 and b.service<=13999 ");
    	}
    	else if(atol(caService)==14000){
    		utStrAddF(caWhere," and b.service>=14000 and b.service<=14999 ");
    	}
    	else if(atol(caService)==15000){
    		utStrAddF(caWhere," and b.service>=15000 and b.service<=15999 ");
    	}
    	else if(atol(caService)==17000){
    		utStrAddF(caWhere," and b.service>=17000 and b.service<=17999 ");
    	}
    	else{
        utStrAddF(caWhere," and b.service = %s",caService);
        utStrAddF(caDelwhere," and service = %s",caService);
      }
    }
    if(!utStrIsSpaces(caProt)) {
        utStrAddF(caWhere," and b.trantype = %s",caProt);
        utStrAddF(caDelwhere," and trantype = %s",caProt);
    }
    if(lByt>0){
	    if(atoi(byt_rflag)==1){
	    	if(atoi(byt_in)==1){
	    		utStrAddF(caWhere," and b.ubytes<=%lu ",lByt);
	    		utStrAddF(caDelwhere," and ubytes<=%lu ",lByt);
	    	}
	    	else{
	    		utStrAddF(caWhere," and b.ubytes>=%lu ",lByt);
	    		utStrAddF(caDelwhere," and ubytes>=%lu ",lByt);
	        }
	    }
	    else if(atoi(byt_rflag)==2){
    	    if(atoi(byt_in)==1){
	    		utStrAddF(caWhere," and b.dbytes<=%lu ",lByt);
	    		utStrAddF(caDelwhere," and dbytes<=%lu ",lByt);
	    	}
	    	else{
	    		utStrAddF(caWhere," and b.dbytes>=%lu ",lByt);
	    		utStrAddF(caDelwhere," and dbytes>=%lu ",lByt);
	        }
	    }
       else {
           	if(atoi(byt_in)==1){
	    		utStrAddF(caWhere," and b.dbytes+b.ubytes<=%lu ",lByt);
	    		utStrAddF(caDelwhere," and dbytes+duytes<=%lu ",lByt);
	    	}
	    	else{
	    		utStrAddF(caWhere," and b.dbytes+b.ubytes>=%lu ",lByt);
	    		utStrAddF(caDelwhere," and dbytes+ubytes>=%lu ",lByt);
	        }
	    }
	 }

   
 
    
    if(!utStrIsSpaces(caOrder)) {
        if(strcmp(caOrder,"date")==0) {
            utStrAddF(caWhere," order by b.vtime  ");
//            utStrAddF(caWhere,",b.stime ");
        }
        else if(strcmp(caOrder,"sip") == 0) {
            utStrAddF(caWhere," order by b.sip ");
        }
        else if(strcmp(caOrder,"dip") == 0) {
            utStrAddF(caWhere," order by b.dip ");
        }
        else if(strcmp(caOrder,"name") == 0) {
            utStrAddF(caWhere," order by b.userid ");
        }
        else if(strcmp(caOrder,"service") == 0) {
            utStrAddF(caWhere," order by b.service desc ");
        }
        else if(strcmp(caOrder,"flow") == 0) {
            utStrAddF(caWhere," order by b.ubytes+b.dbytes ");
        }
        if(!utStrIsSpaces(caOrderby)) {
            if(caOrderby[0] == '1') {     
                utStrAddF(caWhere," desc ");
            }
        }
    }
  
    else {
    	utStrAddF(caWhere," order by b.vtime desc ");
    }
    
//    printf("caWhere=%s\n",caWhere);
    lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;  
    
    
    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }
    //记录数
    if(strstr(caWhere,"and")){
    	
    }
    else
    {	   
    	lStatrow=32;
    	  	
		    sprintf(caTemp,"select count(*) from  %s b %s %s",pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));	
		 
		    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
		    if(iReturn != 0) {
		        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
		           utSysLog("Count:%s",caTemp);
		        }
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0053IP查询"),ncLang("0550数据库出错 %s"),pasDbErrorMsg(NULL));
		            return 0;
		    }
   }
   printf("caWhere=%s\n",caWhere);
 			if(strstr(caWhere,"a.")){
 			   
 			   printf("select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype \
          from %s b,ncuser a %s b.userid=a.userid and %s limit %d,%d\n",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
 			   
          
 				  psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype \
          from %s b,ncuser a %s b.userid=a.userid and %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
			}
			else{
					printf("select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype \
          from %s b %s %s limit %d,%d\n",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
					
					
        psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype \
          from %s b %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
        }
     
   
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }
    
    
    
    

    /* 显示服务信息  */
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"dip",caDip);
    utPltPutVar(psDbHead,"dport",caDport);
    utPltPutVar(psDbHead,"prot",caProt);
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"bef",caBef);
    utPltPutVar(psDbHead,"dtype",caDtype);
    utPltPutVar(psDbHead,"stime",caStime0);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"byt_rflag",byt_rflag);
    utPltPutVar(psDbHead,"byt_in",byt_in);
    utPltPutVar(psDbHead,"byt",caByt);
    utPltPutVar(psDbHead,"osort",caOrder);
    utPltPutVar(psDbHead,"username_match",caUsername_match);
    
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caDispname,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		      strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
    
    utPltPutVar(psDbHead,"title_name",caTitlename);
     
    iReturn = 0;
    lIn=0;
    
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lVtime,
    															 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lUbytes,
                                   UT_TYPE_LONG,  4, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
    iNum = 0;                                   
    while(iReturn == 0) {
    	lIn++;
    
    	if(iNum<lRowNum){
	        iNum++;
	        
	        utPltSetCvtHtml(1);
	        utPltPutLoopVar(psDbHead,"dispname", iNum,
	             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
	             
	        psUser = ncUtlGetUserCont(psShmHead,lUserid); 
	        
	        if(psUser){
	        	utPltPutLoopVar(psDbHead,"mac",iNum,ncCvtMac(psUser->mac));
	        }
	        
	            
	        utPltSetCvtHtml(0);
	        utPltPutLoopVar(psDbHead,"sip",      iNum,utComHostIp(htonl(lSip)));
	        utPltPutLoopVar(psDbHead,"dip",      iNum,utComHostIp(htonl(lDip)));
	        utPltPutLoopVar(psDbHead,"sdate",    iNum,caSdate);
	        utPltPutLoopVar(psDbHead,"stime",    iNum,caStime);
	        utPltPutLoopVar(psDbHead,"conn",    iNum,utStrLtoF(lConntime,caTemp));
	        utPltPutLoopVar(psDbHead,"ubytes",  iNum,utStrLtoF(lUbytes,caTemp));
	        utPltPutLoopVar(psDbHead,"dbytes",   iNum,utStrLtoF(lDbytes,caTemp));
	        if(lService == 0) {
	            utPltPutLoopVar(psDbHead,"service",  iNum,"&nbsp;&nbsp;");
	        }
	        else {
	            utPltPutLoopVar(psDbHead,"service",  iNum,ncUtlGetServiceName(psShmHead,lService,"\0"));
	        }
	        utPltPutLoopVarF(psDbHead,"dport",    iNum,"%lu",lPort);
	        if(lType == 6) {
	            utPltPutLoopVar(psDbHead,"prot",   iNum,"TCP");
	        }
	        else {
	            utPltPutLoopVar(psDbHead,"prot",   iNum,"UDP");
	        } 
      }                       
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lVtime,
        													 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lUbytes,
                                   UT_TYPE_LONG,  4, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
    }
    while(iNum < 32 + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    
    if(strstr(caWhere,"and")){   	
    	lCount=lIn+lStartRec;    	
    }
   
    lTotPg = (lCount -1 ) / lRowNum + 1;
 
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    if(!utStrIsSpaces(caWhere)) {
        utPltPutVar(psDbHead,"where", caWhere);
    }
    if(!utStrIsSpaces(caWhere)) {
    	utPltPutVar(psDbHead,"caDelwhere",caDelwhere);
    }        
    pasDbCloseCursor(psCur);
    
    for(i=0;i<ncPatGetSumService();i++) {
           utPltPutLoopVarF(psDbHead,"id_s",i+1,"%lu",ncUtlGetServiceIdByNum(psShmHead,i));
           utPltPutLoopVar(psDbHead,"name_s",i+1,ncUtlGetServiceByNum(psShmHead,i));
    }
        
//输出网络服务大类
long lCgid;
char caName[32];
     	psCur = pasDbOpenSqlF("select groupid,name  from ncservicecgroup   order by groupid ");
                    
	    if(psCur == NULL) {
	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
	        return 0;
	    }
     
    lCgid=0;
    memset(caName,0,sizeof(caName));
    iNum=0;
	    iReturn = pasDbFetchInto(psCur,     	                           
	      	                           UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
	  	                             
   	
   	while(iReturn == 0 ||iReturn==1405) {
   		
    utPltPutLoopVarF(psDbHead,"cgid",iNum+1,"%d",lCgid);
    utPltPutLoopVar(psDbHead,"cgname",iNum+1,caName);
   	
   	iNum++;
   		
    lCgid=0;
    memset(caName,0,sizeof(caName));
	    iReturn = pasDbFetchInto(psCur,     	                           
	      	                           UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
        }  
    pasDbCloseCursor(psCur);
 
        
        
        
        
        
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    return 0;
}














/* IP高级查询  */

/*
int ncUtilIpHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead,char *pTable,char *pPlate)
{

	  char caTitlename[64];
	  int group_flag;
	  ncService *psService;
	  unsigned long lId_s;
	  char caName_s[32];
    char caWhere[512];
    char caStime0[6];
    char caEtime[6];
    char caStemptime[17];
    char caEtemptime[17];
    char caUserid[17];
    char byt_rflag[2];
    long lstime=0;
    long letime=0;
    char caByt[10];
    char byt_in[2];
    char cadel[17];
    unsigned long lByt=0;
    char caTotPg[16],caCurPg[16];
    char caUsername[32],caSdate[32];
    char caTemp[1024],caGroupid[12],caMac[20];
    char caEdate[20],caOrder[12],caOrderby[12];
    char caDdd[12],caUnit[12],caStatus[12],caDtype[12],caBef[12];
    char caSip[16],caDip[16],caSport[10],caDport[10];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0;
    unsigned long lTime,l,lTime1,lSip,lDip;
    int i,iNum;
    utPltDbHead *psDbHead;
   	int iReturn,iPos;
   	char caDispname[32],caProt[12],caService[12];
   	char caStime[32];
   	char caIdfyno[20];
    char caIp[20];
   	char ids[256];
   	int i2,lUserid2;
   	char caUsername_match[8],caIdfyno_match[8],caIp_match[8],caMac_match[8],caDip_match[8];
   	float lRat;
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount,lStatus1;
	int     lSumUser;
	char sqlbuf[2048]="";
	unsigned long lStatrow,lMintime,lMaxtime,lSum,lStarttime,lEndtime,lMaxin,lMinin,lIn,lVtime;
   	unsigned long lUbytes,lDbytes,lConntime,lService,lPort,lType;
    char caWhereStr[10];
  ncUserCont *psUser;
 	pasDbCursor *psCur;
 	char caDelwhere[512];
    strcpy(caDelwhere,"\0");
 	strcpy(caWhere,"\0");
	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncUtilIpHSearch");
 	}
 	
 	lStatrow=atol(utComGetVar_sd(psShmHead,"statrow","10000"));
 	lSum=0;
 	lMintime=0;
 	lMaxtime=0;
 	pasDbOneRecord("select count(*),min(vtime),max(vtime) from nciplog",0,
 												UT_TYPE_LONG,4,&lSum,
 												UT_TYPE_LONG,4,&lMintime,
 												UT_TYPE_LONG,4,&lMaxtime);
 	
	iReturn = utMsgGetSomeNVar(psMsgHead,30,
					        "mac",        UT_TYPE_STRING,18, caMac,
					        "userid",     UT_TYPE_STRING,14,caUserid,
	                "dispname",   UT_TYPE_STRING, 31, caDispname,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "sip",        UT_TYPE_STRING, 31, caSip,
	                "dip",        UT_TYPE_STRING, 31, caDip,
	                "dport",      UT_TYPE_STRING, 15, caDport,
	                "prot",       UT_TYPE_STRING, 11, caProt,
	                "service",       UT_TYPE_STRING, 11, caService,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "orderby",    UT_TYPE_STRING, 2, caOrderby,
	                "ddd",        UT_TYPE_STRING, 11,caDdd,
	                "unit",       UT_TYPE_STRING, 11,caUnit,
	                "bef",        UT_TYPE_STRING, 11, caBef,
	                "dtype",      UT_TYPE_STRING, 11,caDtype,
	                "stime",	  UT_TYPE_STRING, 5,caStime0,
	                "etime",      UT_TYPE_STRING, 5,caEtime,
	                "byt_rflag",  UT_TYPE_STRING, 1,byt_rflag,
	                "byt_in",     UT_TYPE_STRING, 1,byt_in,
	                "byt",        UT_TYPE_STRING, 9,caByt,
	                "ip",         UT_TYPE_STRING,15,caIp,
	                "del",        UT_TYPE_STRING,16,cadel,
	                "username_match",UT_TYPE_STRING,2,caUsername_match,
   	              "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
   	              "ip_match",    UT_TYPE_STRING,2,caIp_match,
   	              "mac_match",   UT_TYPE_STRING,2,caMac_match,
   	              "dip_match",   UT_TYPE_STRING,2,caDip_match);
 if(atol(caUserid)>0)
    strcpy(caDispname,ncUtlBufGetUsername(psShmHead,atol(caUserid)));  	              
                 	              
	strcpy(caStemptime,"\0");
	strcpy(caEtemptime,"\0");
	lByt=atol(caByt);
	lstime=0;
	letime=0;
	if(!utStrIsSpaces(caSdate)){
		if(utStrIsSpaces(caStime0)) {
			strcpy(caStime0,"00:00");
		}
		sprintf(caStemptime,"%s %s",caSdate,caStime0);
		lstime=utTimStrToLong("%Y/%m/%d %H:%M",caStemptime);
	}
	if(!utStrIsSpaces(caEdate)){
		if(utStrIsSpaces(caEtime)) {
			strcpy(caEtime,"00:00");
		}
		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		letime=utTimStrToLong("%Y/%m/%d %H:%M",caEtemptime);
	}	
	
	if(lstime==0)
		lStarttime=lMintime;
	else
	  lStarttime=lstime;
	  
	if(letime==0)
	  lEndtime=lMaxtime;
	 else
	  lEndtime=letime;
	
	
    lCurPg = 1;
    group_flag=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0&&(strcmp(cadel,"del")!=0)) { 
    	  group_flag=1;
       
    }
    
    memset(caWhere,0,sizeof(caWhere));
    if(group_flag==1||strlen(caUserid)>0||strlen(caGroupid)>0||strlen(caDispname)>0||strlen(caMac)>0||strlen(caSip)>0||strlen(caDip)>0||strlen(caDport)>0||strlen(caProt)>0||strlen(caService)>0||strlen(caIdfyno)>0){
    	if(strlen(caSdate)==0){
    		strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
    		strcpy(caStime0,"00:00");
    		sprintf(caStemptime,"%s %s",caSdate,caStime0);
		    lstime=utTimStrToLong("%Y-%m-%d %H:%M",caStemptime);
    	}
    	if(strlen(caEdate)==0){
    		strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
    		strcpy(caEtime,"23:59");
    		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		    letime=utTimStrToLong("%Y-%m-%d %H:%M",caEtemptime);
    	}
    }
    
   
   
    if(strcmp(caDtype,"unit") == 0) { 
        lTime = time(0);
        if(!utStrIsSpaces(caDdd)) { 
            l = atol(caDdd);
            if(caUnit[0] == '1') { 
                lTime1 = lTime - l* 86400L;
            }
            else if(caUnit[0] == '2') {
                lTime1 = lTime - l* 7 * 86400L;
            }
            else {
                lTime1 = lTime - l* 30 * 86400L;
            }
 //           strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime1));
            if(caBef[0] == '1') { 
                utStrAddF(caWhere," and b.vtime >= %lu ",lTime1);
                utStrAddF(caDelwhere," and vtime >= %lu ",lTime1);
            }
            else { 
                utStrAddF(caWhere," and b.vtime <= %lu ",lTime1);
                utStrAddF(caDelwhere," and vtime <= %lu ",lTime1);
            }
        }            
    }
    else { 
        if(!utStrIsSpaces(caSdate)) {
            utStrAddF(caWhere," and b.vtime >= %lu ",lstime);
            utStrAddF(caDelwhere," and vtime >= %lu ",lstime);
        }
        if(!utStrIsSpaces(caEdate)) {
            utStrAddF(caWhere," and b.vtime <= %lu ",letime);
            utStrAddF(caDelwhere," and vtime <= %lu ",letime);
        }
    }
    
    if(strlen(caUserid)==0){
	    if((!utStrIsSpaces(caDispname))&&(atol(caUsername_match)==0)) {
	    	lUserid=0;
	    	sprintf(sqlbuf,"select userid from ncuser where (dispname = '%s' or username='%s') ",caDispname,caDispname);
	    	pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
	        utStrAddF(caWhere," and b.userid=%lu ",lUserid);
	        
	       if(lUserid>0) sprintf(caUserid,"%lu",lUserid);
	       
	    }
       if((!utStrIsSpaces(caDispname))&&(atol(caUsername_match)==1)) {
	        sprintf(caWhere+strlen(caWhere)," and (a.username like '%c%s%c' or a.dispname like '%c%s%c') ",'%',caDispname,'%','%',caDispname,'%');
	       
	    }
	    if(!utStrIsSpaces(caMac)) {
	    	if(atol(caMac_match)==0){
		    	lUserid=0;
		    	sprintf(sqlbuf,"select userid from ncuser where mac='%s' ",caMac);
//		    	printf("sqlbuf=%s\n",sqlbuf);
		    	 pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
		        utStrAddF(caWhere," and b.userid=%lu ",lUserid);	      
	     }
	     else{
	     		sprintf(caWhere+strlen(caWhere)," and a.mac like '%c%s%c' ",'%',caMac,'%');
	    }
	   }
	  }
	  else{
	  	utStrAddF(caWhere," and b.userid=%lu ",atol(caUserid));
	  }
    
    
    
     if(group_flag==1){
     	utStrAddF(caWhere," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu))",lGroupid0,lGroupid0);
     	
    }
    if((!utStrIsSpaces(caGroupid))&&(strcmp(cadel,"del")!=0)) {
    	  if(strcmp(caGroupid,"0")==0)
        	 utStrAddF(caWhere," and (a.groupid = %s ) ",caGroupid);
        else
        utStrAddF(caWhere," and (a.groupid = %s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
    }
    
    if((strlen(caIdfyno)!=0)&&(atol(caIdfyno_match)==0)){
    		lUserid=0;
	    	sprintf(sqlbuf,"select userid from ncuser where idfyno = '%s' ",caIdfyno);
	    	pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
	      utStrAddF(caWhere," and b.userid=%lu ",lUserid);
	 }
     if((strlen(caIdfyno)!=0)&&(atol(caIdfyno_match)==1)){
     	sprintf(caWhere+strlen(caWhere)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
    }
    

    if(!utStrIsSpaces(caSip)) {
    	if(atol(caIp_match)==0)
       utStrAddF(caWhere," and b.sip  =  %lu", ntohl(utComHostAddress(caSip)));
      else
      sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caSip,'%');
      
    }
    if(!utStrIsSpaces(caDip)) {
    	if(atol(caDip_match)==0)
        utStrAddF(caWhere," and b.dip = %lu",ntohl(utComHostAddress(caDip)));
      else
        sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.dip) like '%c%s%c' ",'%',caDip,'%');
      
    }
    if(!utStrIsSpaces(caDport)) {
        utStrAddF(caWhere," and b.port = %s",caDport);
        utStrAddF(caDelwhere," and port = %s",caDport);
    }
    
    
    if(!utStrIsSpaces(caService)) {
    	if(atol(caService)==2000){
    		utStrAddF(caWhere," and b.service>=2000 and b.service<=2999");
    	}
    	else if(atol(caService)==1000){
    		utStrAddF(caWhere," and b.service>=1000 and b.service<=1999 ");
    	}
    	else if(atol(caService)==3000){
    		utStrAddF(caWhere," and b.service>=3000 and b.service<=3999 ");
    	}
    	else if(atol(caService)==4000){
    		utStrAddF(caWhere," and b.service>=4000 and b.service<=4999 ");
    	}
    	else if(atol(caService)==7000){
    		utStrAddF(caWhere," and b.service>=7000 and b.service<=7999 ");
    	}
    	else if(atol(caService)==8000){
    		utStrAddF(caWhere," and b.service>=8000 and b.service<=8999 ");
    	}
    	else if(atol(caService)==9000){
    		utStrAddF(caWhere," and b.service>=9000 and b.service<=9999 ");
    	}
    	else if(atol(caService)==10000){
    		utStrAddF(caWhere," and b.service>=10000 and b.service<=10999 ");
    	}
    	else if(atol(caService)==11000){
    		utStrAddF(caWhere," and b.service>=11000 and b.service<=11999 ");
    	}
    	else if(atol(caService)==12000){
    		utStrAddF(caWhere," and b.service>=12000 and b.service<=12999 ");
    	}
    	else if(atol(caService)==13000){
    		utStrAddF(caWhere," and b.service>=13000 and b.service<=13999 ");
    	}
    	else if(atol(caService)==14000){
    		utStrAddF(caWhere," and b.service>=14000 and b.service<=14999 ");
    	}
    	else if(atol(caService)==15000){
    		utStrAddF(caWhere," and b.service>=15000 and b.service<=15999 ");
    	}
    	else if(atol(caService)==17000){
    		utStrAddF(caWhere," and b.service>=17000 and b.service<=17999 ");
    	}
    	else{
        utStrAddF(caWhere," and b.service = %s",caService);
        utStrAddF(caDelwhere," and service = %s",caService);
      }
    }
    if(!utStrIsSpaces(caProt)) {
        utStrAddF(caWhere," and b.trantype = %s",caProt);
        utStrAddF(caDelwhere," and trantype = %s",caProt);
    }
    if(lByt>0){
	    if(atoi(byt_rflag)==1){
	    	if(atoi(byt_in)==1){
	    		utStrAddF(caWhere," and b.ubytes<=%lu ",lByt);
	    		utStrAddF(caDelwhere," and ubytes<=%lu ",lByt);
	    	}
	    	else{
	    		utStrAddF(caWhere," and b.ubytes>=%lu ",lByt);
	    		utStrAddF(caDelwhere," and ubytes>=%lu ",lByt);
	        }
	    }
	    else if(atoi(byt_rflag)==2){
    	    if(atoi(byt_in)==1){
	    		utStrAddF(caWhere," and b.dbytes<=%lu ",lByt);
	    		utStrAddF(caDelwhere," and dbytes<=%lu ",lByt);
	    	}
	    	else{
	    		utStrAddF(caWhere," and b.dbytes>=%lu ",lByt);
	    		utStrAddF(caDelwhere," and dbytes>=%lu ",lByt);
	        }
	    }
       else {
           	if(atoi(byt_in)==1){
	    		utStrAddF(caWhere," and b.dbytes+b.ubytes<=%lu ",lByt);
	    		utStrAddF(caDelwhere," and dbytes+duytes<=%lu ",lByt);
	    	}
	    	else{
	    		utStrAddF(caWhere," and b.dbytes+b.ubytes>=%lu ",lByt);
	    		utStrAddF(caDelwhere," and dbytes+ubytes>=%lu ",lByt);
	        }
	    }
	 }

   
 
    
    if(!utStrIsSpaces(caOrder)) {
        if(strcmp(caOrder,"date")==0) {
            utStrAddF(caWhere," order by b.vtime  ");
//            utStrAddF(caWhere,",b.stime ");
        }
        else if(strcmp(caOrder,"sip") == 0) {
            utStrAddF(caWhere," order by b.sip ");
        }
        else if(strcmp(caOrder,"dip") == 0) {
            utStrAddF(caWhere," order by b.dip ");
        }
        else if(strcmp(caOrder,"name") == 0) {
            utStrAddF(caWhere," order by b.userid ");
        }
        else if(strcmp(caOrder,"flow") == 0) {
            utStrAddF(caWhere," order by b.ubytes+b.dbytes ");
        }
        if(!utStrIsSpaces(caOrderby)) {
            if(caOrderby[0] == '1') {     
                utStrAddF(caWhere," desc ");
            }
        }
    }
  
    else {
    	utStrAddF(caWhere," order by b.vtime desc ");
    }
    
//    printf("caWhere=%s\n",caWhere);
    lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;  
    
    
    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }
    //记录数
    if(strstr(caWhere,"and")){
    	
    }
    else
    {	   
    	lStatrow=32;
    	  	
		    sprintf(caTemp,"select count(*) from  %s b %s %s",pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));	
		 
		    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
		    if(iReturn != 0) {
		        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
		           utSysLog("Count:%s",caTemp);
		        }
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0053IP查询"),ncLang("0550数据库出错 %s"),pasDbErrorMsg(NULL));
		            return 0;
		    }
   }
 			if(strstr(caWhere,"a.")){
 				  psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype \
          from %s b,ncuser a %s b.userid=a.userid and %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
			}
			else{
        psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype \
          from %s b %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
        }
        if(iDebugLevel == NC_DEBUG_MSGINFO) {        
            utSysLog("[NoUser]select b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
                 b.conntime,b.service,b.port,b.trantype \
                 from %s b %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lRowNum);
        }
   
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }
    
    
    
    

    
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"dip",caDip);
    utPltPutVar(psDbHead,"dport",caDport);
    utPltPutVar(psDbHead,"prot",caProt);
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"bef",caBef);
    utPltPutVar(psDbHead,"dtype",caDtype);
    utPltPutVar(psDbHead,"stime",caStime0);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"byt_rflag",byt_rflag);
    utPltPutVar(psDbHead,"byt_in",byt_in);
    utPltPutVar(psDbHead,"byt",caByt);
    utPltPutVar(psDbHead,"username_match",caUsername_match);
    
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caDispname,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		      strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
    
    utPltPutVar(psDbHead,"title_name",caTitlename);
     
    iReturn = 0;
    lIn=0;
    lMinin=3000000000L;
    lMaxin=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lVtime,
    															 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lUbytes,
                                   UT_TYPE_LONG,  4, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
    iNum = 0;                                   
    while(iReturn == 0) {
    	lIn++;
    	if(lVtime>lMaxin) lMaxin=lVtime;
    	if(lVtime<lMinin) lMinin=lVtime;
    	
    	if(iNum<lRowNum){
	        iNum++;
	        lStartRec++;
	        utPltSetCvtHtml(1);
	        utPltPutLoopVar(psDbHead,"dispname", iNum,
	             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
	             
	        psUser = ncUtlGetUserCont(psShmHead,lUserid); 
	        
	        if(psUser){
	        	utPltPutLoopVar(psDbHead,"mac",iNum,ncCvtMac(psUser->mac));
	        }
	        
	            
	        utPltSetCvtHtml(0);
	        utPltPutLoopVar(psDbHead,"sip",      iNum,utComHostIp(htonl(lSip)));
	        utPltPutLoopVar(psDbHead,"dip",      iNum,utComHostIp(htonl(lDip)));
	        utPltPutLoopVar(psDbHead,"sdate",    iNum,caSdate);
	        utPltPutLoopVar(psDbHead,"stime",    iNum,caStime);
	        utPltPutLoopVar(psDbHead,"conn",    iNum,utStrLtoF(lConntime,caTemp));
	        utPltPutLoopVar(psDbHead,"ubytes",  iNum,utStrLtoF(lUbytes,caTemp));
	        utPltPutLoopVar(psDbHead,"dbytes",   iNum,utStrLtoF(lDbytes,caTemp));
	        if(lService == 0) {
	            utPltPutLoopVar(psDbHead,"service",  iNum,"&nbsp;&nbsp;");
	        }
	        else {
	            utPltPutLoopVar(psDbHead,"service",  iNum,ncUtlGetServiceName(psShmHead,lService,"\0"));
	        }
	        utPltPutLoopVarF(psDbHead,"dport",    iNum,"%lu",lPort);
	        if(lType == 6) {
	            utPltPutLoopVar(psDbHead,"prot",   iNum,"TCP");
	        }
	        else {
	            utPltPutLoopVar(psDbHead,"prot",   iNum,"UDP");
	        } 
      }                       
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lVtime,
        													 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lUbytes,
                                   UT_TYPE_LONG,  4, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
    }
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    
    if(strstr(caWhere,"and")){
    	if(lMaxin==lMinin){
    		if(lIn<lStatrow)
    			 lCount=lIn;
    		else
    		lCount=lSum;
    	}
    	else{
    		if(lEndtime>lMaxtime) lEndtime=lMaxtime;
    		if(lStarttime<lMintime) lStarttime=lMintime;
    		
    		lRat=(float)((float)(lEndtime-lStarttime)/(float)(lMaxin-lMinin));
    		#ifdef LDEBUG 
//    		printf("lRat=%f\n",lRat);
//    		printf("lIn=%lu\n",lIn);
 //   		printf("lMaxin=%lu\n",lMaxin);
 //   		printf("lMinin=%lu\n",lMinin);
 //   		printf("lEendtime=%lu\n",lEndtime);
    		#endif
    		if(lEndtime>lMaxtime) lEndtime=lMaxtime;
    	lCount=(int)(lIn*lRat);
    	
    	if(lCount>lSum) lCount=lSum;
    	if(lIn<lStatrow) lCount=lIn;
       }
    	
    }
    
    lTotPg = (lCount -1 ) / lRowNum + 1;
 
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    if(!utStrIsSpaces(caWhere)) {
        utPltPutVar(psDbHead,"where", caWhere);
    }
    if(!utStrIsSpaces(caWhere)) {
    	utPltPutVar(psDbHead,"caDelwhere",caDelwhere);
    }        
    pasDbCloseCursor(psCur);
    
    for(i=0;i<ncPatGetSumService();i++) {
           utPltPutLoopVarF(psDbHead,"id_s",i+1,"%lu",ncUtlGetServiceIdByNum(psShmHead,i));
           utPltPutLoopVar(psDbHead,"name_s",i+1,ncUtlGetServiceByNum(psShmHead,i));
    }
        
//输出网络服务大类
long lCgid;
char caName[32];
     	psCur = pasDbOpenSqlF("select groupid,name  from ncservicecgroup   order by groupid ");
                    
	    if(psCur == NULL) {
	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
	        return 0;
	    }
     
    lCgid=0;
    memset(caName,0,sizeof(caName));
    iNum=0;
	    iReturn = pasDbFetchInto(psCur,     	                           
	      	                           UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
	  	                             
   	
   	while(iReturn == 0 ||iReturn==1405) {
   		
    utPltPutLoopVarF(psDbHead,"cgid",iNum+1,"%d",lCgid);
    utPltPutLoopVar(psDbHead,"cgname",iNum+1,caName);
   	
   	iNum++;
   		
    lCgid=0;
    memset(caName,0,sizeof(caName));
	    iReturn = pasDbFetchInto(psCur,     	                           
	      	                           UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
        }  
    pasDbCloseCursor(psCur);
 
        
        
        
        
        
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    return 0;
}
*/

/* http高级查询  */
int ncUtilHttpHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead,
                                char *pTable,char *pPlate)
{
	  int group_flag;
	  char caUrname[32];
	  char sqlbuf[1024];
    char caWhere[512];
    char caStime0[6];
    char caEtime[6];
    char caStemptime[17];
    char caEtemptime[17];
    long lstime=0;
    long letime=0;
    char caByt[10];
    char byt_in[2];
    unsigned long lByt=0;
    char caDelwhere[256];
    char caMac[20];
    char caTotPg[16],caCurPg[16],caUserid[16];
    char caUsername[32],caSdate[32];
    char caTemp[512],caGroupid[12];
    char caEdate[20],caOrder[12],caOrderby[12];
    char caDdd[12],caUnit[12],caStatus[12],caDtype[12],caBef[12];
    char caTitle[128];
    char caSip[16],caDip[16],caSport[10],caDport[10],caUrl[128],caUrl0[256];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0;
    char caHost[64];
    char caIdfyno[20];
    unsigned long lTime,l,lTime1,lSip;
    int i,iNum,iTitleMark=0;
    utPltDbHead *psDbHead;
   	int iReturn,iPos,lSumUser;
   	char caDispname[32],caProt[12],caService[12];
   	char caStime[32];
   	char caIp[20];
   	char ids[256];
   	int i2,lUserid2;
   	char caUri[128];
   	char caUrlid2[16];
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount,lStatus1;
   	unsigned long lUbytes,lDbytes,lConntime,lService,lPort,lType,lBytes;
   	unsigned long lStatrow,lMintime,lMaxtime,lSum,lStarttime,lEndtime,lMaxin,lMinin,lIn,lVtime;
   	float lRat;
 	  pasDbCursor *psCur;
 	  char caUsername_match[8],caIdfyno_match[8],caIp_match[8],caDip_match[8];
 	  char caUrlid[16],caWebtitle[64];
    char caWhereStr[10];
    char caTitlename[64];
    char caHost_match[32];
	strcpy(caWhere,"\0");
	strcpy(caDelwhere,"\0");
    if(strcasecmp(utComGetVar_sd(psShmHead,"HttpContent","No"),"Title")==0) {
        iTitleMark = 1;
    }

 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebQyeIp");
 	}
	iReturn = utMsgGetSomeNVar(psMsgHead,30,
					       "mac",        UT_TYPE_STRING, 18, caMac,
	                "dispname",   UT_TYPE_STRING, 31, caDispname,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "userid",     UT_TYPE_STRING,11,caUserid,
	                "sip",        UT_TYPE_STRING, 31, caSip,
	                "host",       UT_TYPE_STRING, 63, caHost,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "orderby",    UT_TYPE_STRING, 2, caOrderby,
	                "ddd",        UT_TYPE_STRING, 11,caDdd,
	                "unit",       UT_TYPE_STRING, 11,caUnit,
	                "bef",        UT_TYPE_STRING, 11, caBef,
	                "dtype",      UT_TYPE_STRING, 11,caDtype,
	                "stime",	  UT_TYPE_STRING, 5,caStime0,
	                "etime",      UT_TYPE_STRING, 5,caEtime,
	                "byt_in",     UT_TYPE_STRING, 1,byt_in,
	                "byt",        UT_TYPE_STRING, 9,caByt,
	                "dip",        UT_TYPE_STRING,15,caDip,
	                "idfyno",     UT_TYPE_STRING,18,caIdfyno,
	                 "ip",         UT_TYPE_STRING,15,caIp,
	                 "uri",       UT_TYPE_STRING,100,caUri,
	                 "username_match",UT_TYPE_STRING,2,caUsername_match,
   	               "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
   	               "ip_match",  UT_TYPE_STRING,2,caIp_match,
   	               "dip_match", UT_TYPE_STRING,2,caDip_match,
   	               "urlid",UT_TYPE_STRING,10,caUrlid,
   	               "urlid2",UT_TYPE_STRING,10,caUrlid2,
   	               "webtitle",UT_TYPE_STRING,63,caWebtitle,
   	               "host_match",UT_TYPE_STRING,30,caHost_match);
   utStrDelSpaces(caIp);           
   utStrDelSpaces(caDip);  
   utStrDelSpaces(caSip);
   utStrDelSpaces(caUri);   
   utStrDelSpaces(caHost);   
   utStrDelSpaces(caMac);           
   utStrDelSpaces(caDispname);
   if(strlen(caHost_match)==0) strcpy(caHost_match,"1");

if((strlen(caUrlid)==0)&&(strlen(caUrlid2)>0)) strcpy(caUrlid,caUrlid2);
 if(atol(caUserid)>0)
    strcpy(caDispname,ncUtlBufGetUsername(psShmHead,atol(caUserid)));  
  

	strcpy(caStemptime,"\0");
	strcpy(caEtemptime,"\0");
	lByt=atol(caByt);
	if(!utStrIsSpaces(caSdate)){
		if(utStrIsSpaces(caStime0)) {
			strcpy(caStime0,"00:00");
		}
		sprintf(caStemptime,"%s %s",caSdate,caStime0);
		lstime=utTimStrToLong("%Y/%m/%d %H:%M",caStemptime);
	}
	if(!utStrIsSpaces(caEdate)){
		if(utStrIsSpaces(caEtime)) {
			strcpy(caEtime,"00:00");
		}
		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		letime=utTimStrToLong("%Y/%m/%d %H:%M",caEtemptime);
	}	
	

	if(lstime==0)
		lStarttime=lMintime;
	else
	  lStarttime=lstime;
	  
	if(letime==0)
	  lEndtime=lMaxtime;
	 else
	  lEndtime=letime;
	
	lStatrow=atol(utComGetVar_sd(psShmHead,"statrow","10000"));
//  lStatrow=32;
 	lSum=0;
 
	                
    lCurPg = 1;    
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {  /* 部门权利  */
       group_flag=1;
    }
    
    memset(caWhere,0,sizeof(caWhere));
    if(group_flag==1||atol(caUserid)>0||strlen(caGroupid)>0||strlen(caDispname)>0||strlen(caMac)>0||strlen(caSip)>0||strlen(caDip)>0||strlen(caUri)>0||strlen(caUrlid)>0||strlen(caWebtitle)>0||strlen(caIdfyno)>0){
    	if(strlen(caSdate)==0){
    		strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
    		strcpy(caStime0,"00:00");
    		sprintf(caStemptime,"%s %s",caSdate,caStime0);
		    lstime=utTimStrToLong("%Y-%m-%d %H:%M",caStemptime);
    	}
    	if(strlen(caEdate)==0){
    		strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
    		strcpy(caEtime,"23:59");
    		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		    letime=utTimStrToLong("%Y-%m-%d %H:%M",caEtemptime);
    	}
    }
    
    
        if(strcmp(caDtype,"unit") == 0) { /* 按天数计  */
        lTime = time(0);
        if(!utStrIsSpaces(caDdd)) { /* 计算几天以内  */
            l = atol(caDdd);
            if(caUnit[0] == '1') { /* 天  */
                lTime1 = lTime - l* 86400L;
            }
            else if(caUnit[0] == '2') { /* 周  */
                lTime1 = lTime - l* 7 * 86400L;
            }
            else {
                lTime1 = lTime - l* 30 * 86400L;
            }
         //   strcpy(caSdate,utTimFormat("%Y/%m/%d",lTime1));
            if(caBef[0] == '1') { /* 以内  */
                utStrAddF(caWhere," and b.vtime >= %lu ",lTime1);
                utStrAddF(caDelwhere," and vtime >= %lu ",lTime1);
            }
            else { /* 以前  */
                utStrAddF(caWhere," and b.vtime <= %lu ",lTime1);
                utStrAddF(caDelwhere," and vtime <= %lu ",lTime1);
            }
        }            
    }
     else { /* 时间范围  */
        if(!utStrIsSpaces(caSdate)) {
            utStrAddF(caWhere," and b.vtime >= %lu ",lstime);
            utStrAddF(caDelwhere," and vtime >= %lu ",lstime);
        }
        if(!utStrIsSpaces(caEdate)) {
            utStrAddF(caWhere," and b.vtime <= %lu ",letime);
            utStrAddF(caDelwhere," and vtime <= %lu ",letime);
        }
    }
    
    
    
    
        lUserid = atol(caUserid);
        if(!utStrIsSpaces(caUserid) && lUserid > 0) {        
            utStrAddF(caWhere," and b.userid = %s",caUserid);
        }
    
         if(group_flag==1) {  /* 部门权利  */
        	
            sprintf(caWhere+strlen(caWhere)," and (b.groupid = %lu or b.groupid in (select groupid from ncgroup where pid=%lu ))",lGroupid0,lGroupid0);
        }
        
        if(!utStrIsSpaces(caGroupid)) {
        	 if(strcmp(caGroupid,"0")==0)
        	     utStrAddF(caWhere," and (b.groupid = %s ) ",caGroupid);
        	 else
               utStrAddF(caWhere," and (b.groupid = %s or b.groupid in (select groupid from ncgroup where pid=%s)) ",caGroupid,caGroupid);
        }
 if(strlen(caUserid)==0){   
    if((!utStrIsSpaces(caDispname))&&(atol(caUsername_match)==0)) {
    	lUserid=0;
    	sprintf(sqlbuf,"select userid from ncuser where dispname = '%s' ",caDispname);
    	pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
        utStrAddF(caWhere," and b.userid=%lu ",lUserid);
			if(lUserid>0) sprintf(caUserid,"%lu",lUserid);
    }
    if((!utStrIsSpaces(caDispname))&&(atol(caUsername_match)==1)) {
	        sprintf(caWhere+strlen(caWhere)," and b.userid in (select userid from ncuser where username like '%c%s%c' or dispname like '%c%s%c') ",'%',caDispname,'%','%',caDispname,'%');	       
	    }
    if(!utStrIsSpaces(caMac)) {
    	lUserid=0;
    	sprintf(sqlbuf,"select userid from ncuser where mac='%s' ",caMac);
    	pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
        utStrAddF(caWhere," and b.userid=%lu ",lUserid);
 				if(lUserid>0) sprintf(caUserid,"%lu",lUserid);
    }
  }
    if(strlen(caUri)!=0){
    	sprintf(caWhere+strlen(caWhere)," and b.uri like '%c%s%c' ",'%',caUri,'%');
    }

    if(strlen(caUrlid)!=0){
    	sprintf(caWhere+strlen(caWhere)," and b.tid=%s ",caUrlid);
    }
    if(strlen(caWebtitle)!=0){
    	sprintf(caWhere+strlen(caWhere)," and b.title like '%c%s%c' ",'%',caWebtitle,'%');
    }
    
    if(!utStrIsSpaces(caSip)) {
    	if(atol(caIp_match)==0)
        utStrAddF(caWhere," and b.sip  = %lu ",ntohl(utComHostAddress(caSip)));
       else
        sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caSip,'%');
       
    }
    if(!utStrIsSpaces(caDip)) {
    	if(atol(caDip_match)==0)
        utStrAddF(caWhere," and b.dip  = %lu ",ntohl(utComHostAddress(caDip)));
       else
        sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.dip) like '%c%s%c' ",'%',caDip,'%');
    }
    if((strlen(caIdfyno)!=0)&&(atol(caIdfyno_match)==0)) {
    	  lUserid=0;
	    	sprintf(sqlbuf,"select userid from ncuser where idfyno = '%s' ",caIdfyno);
	    	pasDbOneRecord(sqlbuf,0,UT_TYPE_LONG,4,&lUserid);
	      utStrAddF(caWhere," and b.userid=%lu ",lUserid);
    }
    if((strlen(caIdfyno)!=0)&&(atol(caIdfyno_match)==1)){
     	sprintf(caWhere+strlen(caWhere)," and userid in (select userid from ncuser where idfyno like '%c%s%c') ",'%',caIdfyno,'%');
    }

    if(!utStrIsSpaces(caHost)) {
    	if(atol(caHost_match)==1){
      	utStrAddF(caWhere," and b.host='%s'",caHost);
        utStrAddF(caDelwhere," and host ='%s'",caHost);
      }
      else{

        if(strcasecmp(utComGetVar_sd(psShmHead,"searchfulltext","No"),"Yes")==0){
 //       	  long mSum,mm;
 //       	  char caObj[100][50];
 //       	  mSum=nc_SepChar(caHost,'.',caObj);
//        	  	for( mm=0;mm<mSum;mm++){
//        	  		if(strcasecmp(caObj[mm],"com")!=0){
//        	  		utStrAddF(caWhere," and match(b.host) against('%s') ",caObj[mm]);
//        	  		utStrAddF(caDelwhere," and match(b.host) against('%s') ",caObj[mm]);

        	  		utStrAddF(caWhere," and match(b.host) against('%s') ",caHost);
        	  		utStrAddF(caDelwhere," and match(b.host) against('%s') ",caHost);
//        	  	  }
 //       	  	}
        	 
        }
        else{
	        utStrAddF(caWhere," and b.host like '%c%s%c'",'%',caHost,'%');
	        utStrAddF(caDelwhere," and host like '%c%s%c'",'%',caHost,'%');
        }
      }
    }
    
        if(lByt>0){
	    	if(atoi(byt_in)==1){
	    		utStrAddF(caWhere," and b.bytes<=%lu ",lByt);
	    		utStrAddF(caDelwhere," and bytes<=%lu ",lByt);
	    	}
	    	else{
	    		utStrAddF(caWhere," and b.bytes>=%lu ",lByt);
	    		utStrAddF(caDelwhere," and b.bytes>=%lu ",lByt);
	        }
	       
	 }
 
    

    if(!utStrIsSpaces(caOrder)) {
        if(strcmp(caOrder,"date")==0) {
            utStrAddF(caWhere," order by b.vtime ");
        }
        else if(strcmp(caOrder,"sip") == 0) {
            utStrAddF(caWhere," order by b.sip ");
        }
        else if(strcmp(caOrder,"host") == 0) {
            utStrAddF(caWhere," order by b.host ");
        }
        else if(strcmp(caOrder,"name") == 0) {
            utStrAddF(caWhere," order by b.userid ");
        }
        else if(strcmp(caOrder,"flow") == 0) {
            utStrAddF(caWhere," order by b.bytes ");
        }
        else {
        	utStrAddF(caWhere," order by b.vtime ");
        }
        if(!utStrIsSpaces(caOrderby)) {
            if(caOrderby[0] == '1') {     
                utStrAddF(caWhere," desc ");
            }
        }
    }
    else
    {
//    	 if(strcasecmp(utComGetVar_sd(psShmHead,"searchfulltext","No"),"Yes")!=0){  	
    	         utStrAddF(caWhere," order by b.vtime desc ");
//    	 }
    }
 printf("caWhere=%s\n",caWhere);
    lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;  

    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }
    if(strstr(caWhere,"and")){
    	
    }
   else{
    	lStatrow=32;
   sprintf(caTemp,"select count(*) from  %s b %s %s",pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));
   printf("caTemp=%s\n",caTemp);
//    sprintf(caTemp,"select count(*) from nchttplog ");
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0053IP查询"),ncLang("0550数据库出错 %s"),pasDbErrorMsg(NULL));
            return 0;
     }
  }
    lTotPg = (lCount -1 ) / lRowNum + 1;
    
    if(iTitleMark == 0) { /* 无标题  */  
    	        if(strstr(caWhere,"a.")){
    	        	 psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.uri,b.host,' ',b.sdate,b.stime,b.bytes,b.dip,b.dport,c.name,c.id \
                from %s b,ncuser a left join ncwebclass c on b.tid=c.id %s b.userid=a.userid and %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow); 
    	        }
    	        else{
               psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.uri,b.host,' ',b.sdate,b.stime,b.bytes,b.dip,b.dport,c.name,c.id \
                from %s b left join ncwebclass c on b.tid=c.id %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);   
              } 
    }
    else { 
    	       if(strstr(caWhere,"a.")){    
           		 psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.uri,b.host,b.title,b.sdate,b.stime,b.bytes,b.dip,b.dport,c.name,c.id \
                from %s b,ncuser a left join ncwebclass c on b.tid=c.id %s b.userid=a.userid and %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);  
              }
              else{
              	psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.uri,b.host,b.title,b.sdate,b.stime,b.bytes,b.dip,b.dport,c.name,c.id \
                 from %s b left join ncwebclass c on b.tid=c.id %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow); 
              }     
    }
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
    psDbHead = utPltInitDb();
    
    utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"host",caHost);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"bef",caBef);
    utPltPutVar(psDbHead,"dtype",caDtype);
    utPltPutVar(psDbHead,"stime",caStime0);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"byt_in",byt_in);
    utPltPutVar(psDbHead,"byt",caByt);
    utPltPutVar(psDbHead,"dip",caDip);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"uri",caUri);
    utPltPutVar(psDbHead,"username_match",caUsername_match);
    utPltPutVar(psDbHead,"urlid",caUrlid);
    utPltPutVar(psDbHead,"webtitle",caWebtitle);
    utPltPutVar(psDbHead,"osort",caOrder);
    utPltPutVar(psDbHead,"host_match",caHost_match);
    
     if(atol(caUserid)>0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caDispname,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		      strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
    
    utPltPutVar(psDbHead,"title_name",caTitlename);
    
    
    iNum = ncUtlQyeHttpOut(psShmHead,psCur,psDbHead,lRowNum,&lMinin,&lMaxin,&lIn);
   
   if(atol(caUrlid)>0){
    	sprintf(caTemp,"select name from ncwebclass where id=%s ",caUrlid);
   
    	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caUrname);

    	utPltPutVar(psDbHead,"urname",caUrname);
    } 

   if(strstr(caWhere,"and")){
   	lCount=lIn+lStartRec; 
   
  }
   
   
    lTotPg = (lCount -1 ) / lRowNum + 1;
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"userid", "%lu", lUserid);
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVar(psDbHead,"caDelwhere",caDelwhere);
    
    
    if(iTitleMark == 0)
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_httplog_hsear_dip.htm");
    else      
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    return 0;
}


int ncUtilQyehttp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead,
                    char *pTable,char *pPlate)
{
	  unsigned long urlid;
    char urlname[64];
    pasDbCursor *psCur;
    char caTemp[255];
    char caTotPg[16];
    static int pp;
    int  i,iReturn,iNum,iMark1,iMark2,lSumUser,iPos;
    char caSdate[32],caUserid[32],caUsername[36],caCurPg[16],caUrl[200],caUrl0[200];
    char caSmt[512],caSip[16],caDip[16],caStime[12],caGroupid[32];
    char caUri[129],caHost[65],caTitle[128];
    char caCount[16];
    char caDel[7];
    char caDelwhere[500];
    char sqlbuf[1024];
    unsigned long lUserid,lStart,lBytes,lSip;
    long lCount;
    int iTitleMark=0;
    long lCurPg,lRowNum,lStartRec,lTotPg,lPort,lGroupid0;
    char caSelect[256],caWhere[256];
    char caExport[16];
    ncUserInfo *psUserInfo;
    utPltDbHead *psDbHead;
    char caTitlename[32];
    char temp_group[32];
    char caWhereStr[10];
    char caEdate[20],caUrname[64];
    unsigned long lCurtime,lScurtime;
    unsigned long lwtype;
    char caStime0[8],caEtime[8],byt_in[8];
    char caByt[16],caIdfyno[20],caUsername_match[8];
    char caUrlid[16],caUrlid2[16],caWebtitle[64];
    float lRat,group_flag;
    unsigned long lStatrow,lMintime,lMaxtime,lSum,lStarttime,lEndtime,lMaxin,lMinin,lIn,lVtime;
    if(strcasecmp(utComGetVar_sd(psShmHead,"HttpContent","No"),"Title")==0) {
        iTitleMark = 1;
    }
    lCurtime=time(0);
    strcpy(caSdate,utTimFormat("%Y/%m/%d 00:00",time(0)));
    strcpy(caEdate,utTimFormat("%Y/%m/%d 23:59",time(0)));
    lScurtime=utTimStrToLong("%Y/%m/%d %H:%M",caSdate);
    lCurtime=utTimStrToLong("%Y/%m/%d %H:%M",caEdate);
   	iReturn = utMsgGetSomeNVar(psMsgHead,10,
   	                "lcount",     UT_TYPE_STRING, 12, caCount,
   	                "groupid",    UT_TYPE_STRING, 12, caGroupid,
   	                "userid",     UT_TYPE_STRING, 12, caUserid,
					"CurPg",      UT_TYPE_STRING, 12, caCurPg,
					"TotPg",      UT_TYPE_STRING, 12, caTotPg,
					"dispname",   UT_TYPE_STRING, 31, caUsername,
					"where",      UT_TYPE_STRING, 255, caWhere,
					"export",     UT_TYPE_STRING, 11,  caExport,
					"Del",        UT_TYPE_STRING, 6,   caDel,
					"caDelwhere", UT_TYPE_STRING, 500, caDelwhere);

					
    lUserid = atol(caUserid);
    
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
    	strcpy(caUsername,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }
    
    
    lCurPg = atol(caCurPg);
    lRowNum = 32;
    if(lCurPg <= 1 || utMsgGetVar_s(psMsgHead,"search.x")){
    	 lCurPg = 1;
    	 strcpy(caCount,"\0");
    	}
    lStartRec = (lCurPg - 1) * lRowNum;
    if(lStartRec <= 0)lStartRec = 0;
    if(!utStrIsSpaces(caExport)) {
        lStartRec = 0;
        lRowNum = 99999999L;
        lStatrow=99999999L;
       
    }
  lCount=0;




    group_flag=0;  
    if(utStrIsSpaces(caWhere)) {

        iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
        if(iReturn == 0 && lGroupid0 > 0) {  /* 部门权利  */
        	group_flag=1;
           
        }
        if(group_flag==1||(strlen(caGroupid)>0)||(strlen(caUserid)>0)){

	        utStrAddF(caWhere," and b.vtime>%lu and b.vtime<=%lu ",lScurtime,lCurtime); 
	        lStarttime=lScurtime;
	        lEndtime=lCurtime;
	      }    
       lUserid = atol(caUserid);
        if(!utStrIsSpaces(caUserid) && lUserid > 0) {        
            utStrAddF(caWhere," and b.userid = %s",caUserid);
        }
         if(group_flag==1) {  /* 部门权利  */
        	
            sprintf(caWhere+strlen(caWhere)," and (b.groupid = %lu or a.groupid in (select groupid from ncgroup where pid=%lu ))",lGroupid0,lGroupid0);
        }
        
        if(!utStrIsSpaces(caGroupid)) {
        	  if(strcmp(caGroupid,"0")==0)
        	     utStrAddF(caWhere," and (b.groupid = %s ) ",caGroupid);
        	  else
              utStrAddF(caWhere," and (b.groupid = %s or b.groupid in (select groupid from ncgroup where pid=%s)) ",caGroupid,caGroupid);
        }

      
    }

    if(strncmp(caWhere," and",4) == 0) {
        iPos = 4;
        strcpy(caWhereStr,"where");
    }
    else {
        iPos = 0;
        strcpy(caWhereStr,"\0");
    }

    if((utStrIsSpaces(caExport))&&(strstr(caWhere,"and")==NULL)){
      lStatrow=32;
        sprintf(caTemp,"select count(*) from  %s b %s %s",pTable,caWhereStr,ncUtlTrimOrder(caWhere+iPos));      
        iReturn = pasDbOneRecord(caTemp,0,
                            UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            if(iDebugLevel == NC_DEBUG_MSGINFO) {        
               utSysLog("Count:%s",caTemp);
            }
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0053IP查询"),ncLang("0550数据库出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
   //     lTotPg = (lCount -1 ) / lRowNum + 1;
    }
    else{
    	lStatrow=atol(utComGetVar_sd(psShmHead,"statrow","10000"));
    }
 /*  
   if(strstr(caWhere,"b.vtime")==NULL){
	        utStrAddF(caWhere," and b.vtime>%lu and b.vtime<=%lu ",lScurtime,lCurtime); 
	        lStarttime=lScurtime;
	        lEndtime=lCurtime;
	      }
*/
	  if(strstr(caWhere,"order by")==NULL)
	        utStrAddF(caWhere," order by b.vtime desc ");       
	
	   if(strncmp(caWhere," and",4) == 0) {
	        iPos = 4;
	        strcpy(caWhereStr,"where");
	    }
	    else {
	        iPos = 0;
	        strcpy(caWhereStr,"\0");
	    }
	      if(!utStrIsSpaces(caExport)) {
	      	lStartRec=0;
	      	lStatrow=99999;
	      	lStartRec=0;
	      }  
	    
    if(iTitleMark == 0) {
    	printf("select b.vtime,b.userid,b.sip,b.uri,b.host,' ',b.sdate,b.stime,b.bytes,b.dip,b.dport,c.name,b.wtype \
                from %s b left join ncwebclass c on b.tid=c.id %s %s limit %d,%d\n",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
                
              if(strstr(caWhere,"a.")){
               psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.uri,b.host,' ',b.sdate,b.stime,b.bytes,b.dip,b.dport,c.name,b.wtype \
                from %s b,ncuser a left join ncwebclass c on (b.tid=c.id) %s (b.userid=a.userid and %s) limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
              }
              else{  
              psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.uri,b.host,' ',b.sdate,b.stime,b.bytes,b.dip,b.dport,c.name,b.wtype \
                from %s b left join ncwebclass c on b.tid=c.id %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
             }
    }
    else {   
    	printf("select b.vtime,b.userid,b.sip,b.uri,b.host,b.title,b.sdate,b.stime,b.bytes,b.dip,b.dport,c.name,b.wtype \
                from %s b left join ncwebclass c on b.tid=c.id %s %s limit %d,%d\n",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow); 
            if(strstr(caWhere,"a.")){
            psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.uri,b.host,b.title,b.sdate,b.stime,b.bytes,b.dip,b.dport,c.name,b.wtype \
                from %s b,ncuser a left join ncwebclass c on b.tid=c.id %s b.userid=a.userid and %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
            }
            else{
            	 psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.uri,b.host,b.title,b.sdate,b.stime,b.bytes,b.dip,b.dport,c.name,b.wtype \
                from %s b left join ncwebclass c on b.tid=c.id %s %s limit %d,%d",pTable,caWhereStr,caWhere+iPos,lStartRec,lStatrow);
            }
    }

    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
    if(!utStrIsSpaces(caExport)) {
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        long id;
        sprintf(caFilename,"nchttp%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613网站访问查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,ncLang("0614网站访问明细表\n"));
        ncWebExportHttp(psShmHead,psCur,fp);
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        
         char caUsername2[32];
			   memset(caUsername2,0,sizeof(caUsername2));
			   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
			   ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"导出网站访问日志");
      
        
        
        return 0;
    }
    iReturn = 0;
    psDbHead = utPltInitDb();
    if(!utStrIsSpaces(caUsername)) {
        utPltPutVar(psDbHead,"username",  caUsername);
    }    
    iNum = ncUtlQyeHttpOut(psShmHead,psCur,psDbHead,lRowNum,&lMinin,&lMaxin,&lIn);

//printf("lMinin=%lu\n",lMinin);
//printf("lMaxin=%lu\n",lMaxin);
//printf("lIn=%lu\n",lIn); 
    if(strstr(caWhere,"and")){
	  lCount=lIn+lStartRec; 
    	
    }

   lTotPg = (lCount -1 ) / lRowNum + 1;
   
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"userid", "%lu", lUserid);

    utPltPutVar(psDbHead,"groupid",caGroupid);
//    if(!utStrIsSpaces(caUserid)) {
        utPltPutVar(psDbHead,"userid", caUserid);
        utPltPutVar(psDbHead,"dispname", caUsername);
        utPltPutVar(psDbHead,"name", caUsername);
//    }
    if(!utStrIsSpaces(caWhere)) {
        utPltPutVar(psDbHead,"where", caWhere);
        utPltPutVar(psDbHead,"caDelwhere",caDelwhere);
    } 
     utPltPutVar(psDbHead,"title_name",caTitlename);
    
    iReturn = utMsgGetSomeNVar(psMsgHead,15,
	                "host",       UT_TYPE_STRING, 25, caHost,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "stime",	  UT_TYPE_STRING, 5,caStime0,
	                "etime",      UT_TYPE_STRING, 5,caEtime,
	                "byt_in",     UT_TYPE_STRING, 1,byt_in,
	                "byt",        UT_TYPE_STRING, 9,caByt,
	                "dip",        UT_TYPE_STRING,15,caDip,
	                "idfyno",     UT_TYPE_STRING,18,caIdfyno,
	                 "sip",         UT_TYPE_STRING,15,caSip,
	                 "uri",       UT_TYPE_STRING,100,caUri,
	                 "username_match",UT_TYPE_STRING,2,caUsername_match,
   	               "urlid",UT_TYPE_STRING,10,caUrlid,
   	               "urlid2",UT_TYPE_STRING,10,caUrlid2,
   	               "webtitle",UT_TYPE_STRING,63,caWebtitle);
    
    if(strlen(caSdate)>0){
    	utPltPutVar(psDbHead,"sdate",caSdate);
    	utPltPutVar(psDbHead,"edate",caEdate);
    	utPltPutVar(psDbHead,"stime",caStime0);
    	utPltPutVar(psDbHead,"etime",caEtime);
    }
    else if(strstr(caWhere,"b.vtime")){
    	utPltPutVar(psDbHead,"sdate",utTimFormat("%Y/%m/%d",lScurtime));
//    	utPltPutVar(psDbHead,"stime",utTimFormat("%H:%M",lScurtime));
      utPltPutVar(psDbHead,"stime","00:00");
    	
    	utPltPutVar(psDbHead,"edate",utTimFormat("%Y/%m/%d",lCurtime));
    	utPltPutVar(psDbHead,"etime","23:59");
//    	utPltPutVar(psDbHead,"etime",utTimFormat("%H:%M",lCurtime));
    }
    utPltPutVar(psDbHead,"host",caHost);
    utPltPutVar(psDbHead,"byt_in",byt_in);
    utPltPutVar(psDbHead,"byt",caByt);
    utPltPutVar(psDbHead,"dip",caDip);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"uri",caUri);
    utPltPutVar(psDbHead,"username_match",caUsername_match);
    utPltPutVar(psDbHead,"urlid",caUrlid);
    utPltPutVar(psDbHead,"urlid2",caUrlid2);
    utPltPutVar(psDbHead,"webtitle",caWebtitle);
    
   if(atol(caUrlid)>0){
    	sprintf(caTemp,"select name from ncwebclass where id=%s ",caUrlid);
   
    	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caUrname);

    	utPltPutVar(psDbHead,"urname",caUrname);
    }
   
    if(iTitleMark == 0)
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_httplog_hsear.htm");
    else       
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    return 0;
}


/* 导出上网纪录  */
int ncWebExportHttp(utShmHead *psShmHead, pasDbCursor *psCur,FILE *fp)
{
    int iReturn;
    ncUserCont *psUser;
    char caTemp[32];
    unsigned long lwtype;
    char caUsername[32],caDispname[32],caGroupname[32];
    char caType[64],caName[128];
    char caUri[128],caTitle[128],caSdate[16],caStime[16],caDate[20],caHost[128];
    unsigned long lSip,lUserid,lBytes,dip,dport;
    int iCount;
    char caDipPort[20];
    long id;
    unsigned long lVtime;
    iCount=0;
    ncUtlPrintCsv(fp,12,UT_TYPE_STRING,ncLang("0711序号"),
                       UT_TYPE_STRING,ncLang("0693姓  名"),
                       UT_TYPE_STRING,ncLang("0762用户名"),
                       UT_TYPE_STRING,ncLang("0122本人IP"),
                       UT_TYPE_STRING,ncLang("0147部  门"),
                       UT_TYPE_STRING,ncLang("0241访问时间"),
                       UT_TYPE_STRING,ncLang("0618网址"),
                       UT_TYPE_STRING,ncLang("0396目标IP"),
                       UT_TYPE_STRING,ncLang("0685详细目录"),
                       UT_TYPE_STRING,ncLang("0624网址类别"),
                       UT_TYPE_STRING,ncLang("0125标题"),
                       UT_TYPE_STRING,ncLang("0369流量"));
    memset(caType,0,sizeof(caType));
    lwtype=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lVtime,
                                   UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_STRING,127,caUri,
                                   UT_TYPE_STRING,63, caHost,
                                   UT_TYPE_STRING,127,caTitle,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lBytes,
                                   UT_TYPE_ULONG,4,&dip,
                                   UT_TYPE_ULONG,4,&dport,
                                   UT_TYPE_STRING,30,caType,
                                   UT_TYPE_LONG,4,&lwtype);
    while(iReturn == 0||iReturn==1405) {
        iCount++;
        psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
            strcpy(caDispname, psUser->dispname);
            strcpy(caUsername, psUser->username);
            strcpy(caGroupname,psUser->psGroup->groupname);
        }
        else {
            sprintf(caDispname, "(%lu)",lUserid);
            sprintf(caUsername, "(%lu)",lUserid);
            sprintf(caGroupname, "%s",ncLang("0629未知"));
        }
        sprintf(caDate,"%s %s",caSdate,caStime);
        if(dip!=0)
        sprintf(caDipPort,"%s:%lu",utComHostIp(htonl(dip)),dport);
        ncLimGetWebNameByUrl(caHost,caType,caName);
     
        ncUtlPrintCsv(fp,12,UT_TYPE_LONG,iCount,
                       UT_TYPE_STRING,caDispname,
                       UT_TYPE_STRING,caUsername,
                       UT_TYPE_STRING,utComHostIp(htonl(lSip)),
                       UT_TYPE_STRING,caGroupname,
                       UT_TYPE_STRING,caDate,
                       UT_TYPE_STRING,caHost,
                       UT_TYPE_STRING,caDipPort,
                       UT_TYPE_STRING,caUri,                       
                       UT_TYPE_STRING,caType,
                       UT_TYPE_STRING,caTitle,
                       UT_TYPE_STRING,utStrLtoF(lBytes,caTemp));
    memset(caType,0,sizeof(caType));
    lwtype=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lVtime,
                                   UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_STRING,127,caUri,
                                   UT_TYPE_STRING,63, caHost,
                                   UT_TYPE_STRING,127,caTitle,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lBytes,
                                   UT_TYPE_ULONG,4,&dip,
                                   UT_TYPE_ULONG,4,&dport,
                                   UT_TYPE_STRING,30,caType,
                                   UT_TYPE_LONG,4,&lwtype);

    }
    return 0;
}


char *ncUtlTrimOrder(char *pWhere)
{
    static char caTemp[255];
    char *p;
    p = strstr(pWhere,"order");
    if(p) {
        memset(caTemp,0,sizeof(caTemp));
        memcpy(caTemp,pWhere,p-pWhere-1);
        return caTemp;
    }
    else {
        return pWhere;
    }
}


int ncUtlQyeHttpOut(utShmHead *psShmHead,pasDbCursor *psCur,utPltDbHead *psDbHead,int lRowNum,unsigned long *lMinin,unsigned long *lMaxin,unsigned long *lIn)
{
    int iReturn;
    unsigned long lUserid,lSip,lBytes,lVtime;
    unsigned long dip,dport;
    unsigned long lwtype;
    char caDipPort[32];
    int iNum;
    char caUrl[256],caHost[64],caTitle[128],caSdate[20],caStime[16],caDate[32];
    char caType[64],caName[128],caUrl0[200],caTemp[1024];
    ncUserCont *psUser;
    long lCount,id;
  
     
    *lIn=0;
    *lMinin=3000000000L;
    *lMaxin=0; 
    id=0;
    lwtype=0;
    memset(caType,0,sizeof(caType));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lVtime,
    															 UT_TYPE_LONG,  4, &lUserid,
                                   UT_TYPE_ULONG, 4, &lSip,
                                   UT_TYPE_STRING,127,caUrl,
                                   UT_TYPE_STRING,63,caHost,
                                   UT_TYPE_STRING,127,caTitle,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lBytes,
                                   UT_TYPE_ULONG,4,&dip,
                                   UT_TYPE_ULONG,4,&dport,
                                   UT_TYPE_STRING,30,caType,
                                   UT_TYPE_LONG,4,&lwtype);
    iNum = 0;                                   
    while(iReturn == 0||iReturn==1405)  {
    	*lIn=*lIn+1;
    	
    	if(iNum<lRowNum){
        iNum++;
        psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
            utPltPutLoopVar(psDbHead,"dispname", iNum,psUser->dispname);
            utPltPutLoopVar(psDbHead,"username", iNum,psUser->username);
            if(psUser->psGroup) {
                utPltPutLoopVar(psDbHead,"groupname", iNum,psUser->psGroup->groupname);
            }
        }
        memset(caDipPort,0,sizeof(caDipPort));
        if(dip!=0)
        sprintf(caDipPort,"%s:%lu",utComHostIp(htonl(dip)),dport);
        
  //      printf("caDipPort=%s\n",caDipPort);
        utPltPutLoopVar(psDbHead,"dip",iNum,caDipPort);
        utPltPutLoopVar(psDbHead,"sip",      iNum,utComHostIp(htonl(lSip)));
        sprintf(caDate,"%s %s",caSdate,caStime);
        utPltPutLoopVar(psDbHead,"sdate",    iNum,caDate);
        utPltPutLoopVar(psDbHead,"bytes",    iNum,utStrLtoF(lBytes,caTemp));
        sprintf(caUrl0,"%s://%s%s",ncHttpProtocol(lwtype),caHost,caUrl);
        strcpy(caUrl,caUrl0);
        caUrl[78]=0;
        utPltSetCvtHtml(1);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid); 
        utPltPutLoopVar(psDbHead,"url",   iNum,caUrl);
        utPltPutLoopVar(psDbHead,"urllnk",   iNum,caUrl0);
        if(strlen(caTitle)==0||strstr(caTitle,"404 Not Found")){
        	  long cid=0;
            ncUrlGetUrlInfo(caHost,caTitle,&cid);
        }
        utPltPutLoopVar(psDbHead,"title",   iNum,caTitle);
        ncLimGetWebNameByUrl(caHost,caType,caName);
        utPltPutLoopVar(psDbHead,"urltype", iNum,caType);
        utPltPutLoopVar(psDbHead,"urlname", iNum,caName);
    utPltSetCvtHtml(0);
 //        if(lwtype>0){
 //       	 sprintf(caTemp,"<a href=\"/pronline/Msg?FunName@ncWebDispHtmlFid&fid@%lu\" target=\"_blank\">网页快照</a>",lwtype);
 //          utPltPutLoopVar(psDbHead,"wykz",iNum,caTemp);
 //       }
  }      
        dip=0;
        dport=0;
        memset(caType,0,sizeof(caType));
        lwtype=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lVtime,
    															 UT_TYPE_LONG,  4, &lUserid,
                                   UT_TYPE_ULONG, 4, &lSip,
                                   UT_TYPE_STRING,127,caUrl,
                                   UT_TYPE_STRING,63,caHost,
                                   UT_TYPE_STRING,127,caTitle,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG,  4, &lBytes,
                                   UT_TYPE_ULONG,4,&dip,
                                   UT_TYPE_ULONG,4,&dport,
                                   UT_TYPE_STRING,30,caType,
                                   UT_TYPE_LONG,4,&lwtype);
                                  
    }
    pasDbCloseCursor(psCur);
   
    return iNum;
}


int ncUtlCvtAllCode(utShmHead *psShmHead)
{
    pasDbCursor *psCur;
    mail_header_info *psMailHead;
    struct LP_S {
        long sid;
        int code;
    } *lP;
    int iSum=0,i,iReturn,lCount;
    long lSid,iFlags;
    char caFile[32];
    char caFile1[128];
    char *pMailLog;
    
    iReturn = pasDbOneRecord("select count(*) from ncmailmon",0,UT_TYPE_LONG,4,&lCount);
    
    lP =  malloc(lCount*sizeof(struct LP_S));
    pMailLog = utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog");  
    psCur = pasDbOpenSqlF("select sid,mailfile,ccode from ncmailmon");
    if(psCur) {
         iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lSid,
                                        UT_TYPE_STRING,31,caFile);
         while(iReturn == 0) {
             sprintf(caFile1,"%s/mon/%s",pMailLog,caFile);
             psMailHead = mail_header(caFile1,&iFlags);
             if(psMailHead == NULL) {
                 printf(" Mail %s Error\n",caFile1);
             }
             else {
                lP[iSum].code = pasMilCcode();
                lP[iSum].sid = lSid;
                printf("%s  Code=%d \n",caFile1,lP[iSum].code);
                iSum++;
                mailHeadFree(psMailHead);
            }
            iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lSid,
                                        UT_TYPE_STRING,31,caFile);
         }
         pasDbCloseCursor(psCur);
         for(i=0;i<iSum;i++) {
             iReturn = pasDbExecSqlF("update ncmailmon set ccode=%lu where sid = %lu",lP[i].code,lP[i].sid);
         }
         pasDbCommit(NULL);
     }


    }
            
int ncUtlCvtAllCtlCode(utShmHead *psShmHead)
{
    pasDbCursor *psCur;
    mail_header_info *psMailHead;
    struct LP_S {
        long sid;
        int code;
    } *lP;
    int iSum=0,i,iReturn,lCount;
    long lSid,iFlags;
    char caFile[32];
    char caFile1[128];
    char *pMailLog;
    
    iReturn = pasDbOneRecord("select count(*) from ncmailctl",0,UT_TYPE_LONG,4,&lCount);
    
    lP =  malloc(lCount*sizeof(struct LP_S));
    pMailLog = utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog");  
    psCur = pasDbOpenSqlF("select sid,mailfile,ccode from ncmailctl");
    if(psCur) {
         iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lSid,
                                        UT_TYPE_STRING,31,caFile);
         while(iReturn == 0) {
             sprintf(caFile1,"%s/ctl/%s",pMailLog,caFile);
             psMailHead = mail_header(caFile1,&iFlags);
             if(psMailHead == NULL) {
                 printf(" Mail %s Error\n",caFile1);
             }
             else {
                lP[iSum].code = pasMilCcode();
                lP[iSum].sid = lSid;
                printf("%s  Code=%d \n",caFile1,lP[iSum].code);
                iSum++;
                mailHeadFree(psMailHead);
            }
            iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lSid,
                                        UT_TYPE_STRING,31,caFile);
         }
         pasDbCloseCursor(psCur);
         for(i=0;i<iSum;i++) {
             iReturn = pasDbExecSqlF("update ncmailctl set ccode=%lu where sid = %lu",lP[i].code,lP[i].sid);
         }
         pasDbCommit(NULL);
     }
    free(lP);

    }
            
//网络身份日志
int ncWebNetIdLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caTemp[2048];
    char caCond[1024],caSorta[12],caOrdera[12];
    char caDid[16],caGroupid[16],caNtype[16];
    char caDisp[32],caDisp2[32];
    char caUserid[16],caSdate[20],caEdate[20],caName1[64],caIp[20],caMac[20],caPosition[36];
    char caTotPg[16],caCurPg[16];
    char caUsername[64];
    char caStime[20],caEtime[20];
    char caDispname[32],caName[64],caRycx[32];
    unsigned long lNtype,lTime,lCurPg,lTotPg;
    char caIdfytype[16],caIdfyno[24];
    unsigned long lStime,lEtime,lUserid;
    int mCount,lCount;
    int i,iNum;
    char caExport[16];
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lRowNum,lStartRec,lTotRec;
   	char caUsername_match[8],caIdfyno_match[8],caNetid_match[8],caIp_match[8],caMac_match[8];
 	  pasDbCursor *psCur;
 	  char caUsername2[32];
 	  char caMsg[1024];
 	  memset(caUsername2,0,sizeof(caUsername2));
 	  dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
 	  
 	struct date_s {
 		unsigned long id;
 		char name[64];
 	} *pData;
 
 	char caTitlename[128];
 	unsigned long lCurtime;
  ncUserCont *psUser;
 	lCurtime=time(0);
 	
	iReturn = utMsgGetSomeNVar(psMsgHead,21,
	    "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			"CurPg",      UT_TYPE_STRING, 12, caCurPg,
			"userid",    UT_TYPE_STRING, 12, caUserid,
			"groupid",   UT_TYPE_STRING, 12, caGroupid,
			"mac",UT_TYPE_STRING,31,caMac,
			"username",UT_TYPE_STRING,63,caUsername,
			"sdate",UT_TYPE_STRING,19,caSdate,
			"edate",UT_TYPE_STRING,19,caEdate,
			"name1",UT_TYPE_STRING,63,caName1,
			"ntype",UT_TYPE_STRING,12,caNtype,
			"ip",UT_TYPE_STRING,15,caIp,
			"position",UT_TYPE_STRING,30,caPosition,
			"idfytype",UT_TYPE_STRING,8,caIdfytype,
			"idfyno",UT_TYPE_STRING,20,caIdfyno,
			"rycx",UT_TYPE_STRING,31,caRycx,
   	  "exp",     UT_TYPE_STRING,10,caExport,
   	  "username_match",UT_TYPE_STRING,2,caUsername_match,
   	  "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
   	  "netid_match",UT_TYPE_STRING,2,caNetid_match,
   	  "mac_match",UT_TYPE_STRING,2,caMac_match,
   	  "ip_match",UT_TYPE_STRING,2,caIp_match);
   	 
   	              	              
	sprintf(caTemp,"select count(*) from ncipindex ");
	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&mCount);
	pData=NULL;
	pData=malloc(sizeof(struct date_s)*(mCount+2));
	if(pData==NULL)
	{
		   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
       return 0;
	}
	   psCur = pasDbOpenSql("select id,name from ncipindex ",0);
	   if(psCur==NULL){
	   	  free(pData);
	   	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
       return 0;
	  }
	    iReturn = pasDbFetchInto(psCur,
	      	                               UT_TYPE_LONG,4,&lNtype,
	    	                               UT_TYPE_STRING,31,caName);
	    iNum = 0; 	                               
	    while((iReturn==0)||(iReturn==1405))
	    {
	  	pData[iNum].id=lNtype;
	  	strcpy(pData[iNum].name,caName);
	        iNum++;       
	       
	    iReturn = pasDbFetchInto(psCur,
	      	                             UT_TYPE_LONG,4,&lNtype,
	    	                               UT_TYPE_STRING,31,caName);
	    }
	    pasDbCloseCursor(psCur); 



	memset(caCond,0,sizeof(caCond));
 		

	 if(strlen(caUserid)!=0)
		sprintf(caCond+strlen(caCond)," and b.userid=%s ",caUserid);
	
		
		if(strlen(caUsername)!=0){
			if(atol(caUsername_match)==0)
			{
			lUserid=0;
			sprintf(caTemp,"select userid from ncuser where username='%s' or dispname='%s' ",caUsername,caUsername);
			pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lUserid);
			sprintf(caCond+strlen(caCond)," and b.userid=%lu ",lUserid);
		  }
		  else{
		  	sprintf(caCond+strlen(caCond)," and (a.username like '%c%s%c' or a.dispname like '%c%s%c') ",'%',caUsername,'%','%',caUsername,'%');
		  }
		}
		if(strlen(caIdfyno)!=0){
			if(strlen(caIdfytype)!=0){
					sprintf(caTemp+strlen(caTemp)," and b.idfytype='%s' ",caIdfytype);
				}
			if(atol(caIdfyno_match)==0){
				lUserid=0;
				sprintf(caTemp,"select userid from ncuser where idfyno='%s' ",caIdfyno);
				
				pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lUserid);
				sprintf(caCond+strlen(caCond)," and b.userid=%lu ",lUserid);
			}
			else{
				sprintf(caCond+strlen(caCond)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
			}
		}
		if(strlen(caSdate)!=0)
		sprintf(caCond+strlen(caCond)," and b.stime>=%lu ",utTimStrToLong("%Y/%m/%d %H:%M",caSdate));
		
		if(strlen(caEdate)!=0)
		sprintf(caCond+strlen(caCond)," and b.stime<=%lu ",utTimStrToLong("%Y/%m/%d %H:%M",caEdate));
		

		
		if(strlen(caIp)!=0){
			if(atol(caIp_match)==0)
		     sprintf(caCond+strlen(caCond)," and inet_ntoa(b.ip)='%s' ",caIp);
		   else
		     sprintf(caCond+strlen(caCond)," and inet_ntoa(b.ip) like '%c%s%c' ",'%',caIp,'%');
		}
		if(strlen(caNtype)!=0)
		sprintf(caCond+strlen(caCond)," and b.ntype=%s ",caNtype);
		
		if(strlen(caName1)!=0){
			if(atol(caNetid_match)==0)
		  sprintf(caCond+strlen(caCond)," and b.name='%s' ",caName1);
		  else
		     sprintf(caCond+strlen(caCond)," and b.name like '%c%s%c' ",'%',caName1,'%');
		    }
		if(strlen(caMac)!=0){
			if(atol(caMac_match)==0)
		    sprintf(caCond+strlen(caCond)," and b.mac='%s' ",caMac);
		  else
		    sprintf(caCond+strlen(caCond)," and b.mac like '%c%s%c' ",'%',caMac,'%');
		  }
		if(strlen(caPosition)!=0)
		sprintf(caCond+strlen(caCond)," and b.position='%s' ",caPosition);
		
		
		 if(strlen(caRycx)!=0){
		 	sprintf(caCond," and b.name='%s' ",caRycx);
 //   sprintf(caCond," and (b.name like '%c%s%c' or inet_ntoa(b.ip) like '%c%s%c' or b.position like '%c%s%c' ) ",'%',caRycx,'%','%',caRycx,'%','%',caRycx,'%');
    }
		
		if(strlen(caGroupid)>0){
			if(strcmp(caGroupid,"0")==0)
			 sprintf(caCond+strlen(caCond)," and a.groupid=%s ",caGroupid);
			else
			 sprintf(caCond+strlen(caCond)," and (a.groupid=%s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
		}
		strcat(caCond," order by b.stime desc ");
		    

    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;

 

    if(!utStrIsSpaces(caExport)){
    lRowNum = 999999;
    }
    else
    lRowNum=32;
    lStartRec = (lCurPg - 1) * lRowNum;    


  if(strstr(caCond,"a.")){
  	sprintf(caTemp,"select count(*) from ncnetidlog b,ncuser a  where b.userid=a.userid ");
  }
  else
    sprintf(caTemp,"select count(*) from ncnetidlog b  where 1=1");
    
 
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
    
  
printf("caTemp=%s\n",caTemp);
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);

    if(iReturn != 0) {
    	free(pData);
		   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
       return 0;
    }  
    if(strstr(caCond,"a.")){ 
    sprintf(caTemp,"select b.ntype,b.name,b.disp,b.stime,b.etime,b.userid,inet_ntoa(b.ip),b.mac,b.position from ncnetidlog b,ncuser a where b.userid=a.userid  ");
  }
  else{
  	    sprintf(caTemp,"select b.ntype,b.name,b.disp,b.stime,b.etime,b.userid,inet_ntoa(b.ip),b.mac,b.position from ncnetidlog b where 1=1  ");
  }
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
    
    sprintf(caTemp+strlen(caTemp)," limit %lu,%lu ",lStartRec,lRowNum);
    
//printf("caTemp=%s\n",caTemp);  
    psCur = pasDbOpenSql(caTemp,0);

    if(psCur == NULL) {
    	free(pData);
      utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
        return 0;
    }   

    if(!utStrIsSpaces(caExport)){
    	FILE *fp;
    	char caFilename[32];
    	char caFile[128];
    	char caPath[128];
        sprintf(caFilename,"ncnetlog%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
    	fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            if(pData) free(pData);
             utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
            return 0;
        }
        
        fprintf(fp,ncLang("2046网络身份使用日志\n"));
        ncUtlPrintCsv(fp,9,UT_TYPE_STRING,ncLang("1401网络账号"),
                           UT_TYPE_STRING,ncLang("1402网络服务类别"),
                           UT_TYPE_STRING,ncLang("1403使用时间"),
                           UT_TYPE_STRING,ncLang("1404结束时间"),
                           UT_TYPE_STRING,ncLang("1405上网IP"),
                           UT_TYPE_STRING,ncLang("0072MAC地址"),
                           UT_TYPE_STRING,ncLang("1406上网位置"),
                           UT_TYPE_STRING,ncLang("0728用户名"),
                           UT_TYPE_STRING,ncLang("1407特征信息"));

       lUserid=0;
       lNtype=0;
       memset(caDisp,0,sizeof(caDisp));
       lStime=0;
       lEtime=0;
       memset(caIp,0,sizeof(caIp));
       memset(caMac,0,sizeof(caMac));
       memset(caPosition,0,sizeof(caPosition));
       memset(caName,0,sizeof(caName));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lNtype,
   	             UT_TYPE_STRING,48,caName,
   	             UT_TYPE_STRING,48,caDisp,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_LONG,4,&lEtime,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,15,caIp,
   	             UT_TYPE_STRING,19,caMac,
   	             UT_TYPE_STRING,32,caPosition);
  	                              
        while((iReturn == 0)||(iReturn==1405)) { 
        	memset(caNtype,0,sizeof(caNtype));
        for(i=0;i<mCount;i++)
        {
        	if(pData[i].id==lNtype)
        	{
        		strcpy(caNtype,pData[i].name);
        		break;
        	}
        }
        
        psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
        	strcpy(caDisp2,psUser->dispname);
        }
        else{
        	strcpy(caDisp2,"");
        }
        strcpy(caStime,utTimFormat("%Y/%m/%d %H:%M",lStime));
        strcpy(caEtime,utTimFormat("%Y/%m/%d %H:%M",lEtime));
              
        ncUtlPrintCsv(fp,9,UT_TYPE_STRING,caName,
                       UT_TYPE_STRING,caNtype,
                       UT_TYPE_STRING,caStime,
                       UT_TYPE_STRING,caEtime,
                       UT_TYPE_STRING,caIp,
                       UT_TYPE_STRING,caMac,
                       UT_TYPE_STRING,caPosition,
                       UT_TYPE_STRING,caDisp2,
                       UT_TYPE_STRING,caDisp);
                     
       lUserid=0;
       lNtype=0;
       memset(caDisp,0,sizeof(caDisp));
       lStime=0;
       lEtime=0;
       memset(caIp,0,sizeof(caIp));
       memset(caMac,0,sizeof(caMac));
       memset(caPosition,0,sizeof(caPosition));
       memset(caName,0,sizeof(caName));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lNtype,
   	             UT_TYPE_STRING,48,caName,
   	             UT_TYPE_STRING,48,caDisp,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_LONG,4,&lEtime,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,15,caIp,
   	             UT_TYPE_STRING,19,caMac,
   	             UT_TYPE_STRING,32,caPosition);
        }
        
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        if(pData) free(pData);
        return 0;
       
    }
         
    psDbHead = utPltInitDb();
    

    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"username",caUsername);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"name",caName1);
    utPltPutVar(psDbHead,"ntype",caNtype);
    utPltPutVar(psDbHead,"ip",caIp);
    utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"rycx",caRycx);
    utPltPutVar(psDbHead,"idfytype",caIdfytype);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"username_match",caUsername_match);
    utPltPutVar(psDbHead,"idfyno_match",caIdfyno_match);
    utPltPutVar(psDbHead,"netid_match",caNetid_match);
    utPltPutVar(psDbHead,"mac_match",caMac_match);
    utPltPutVar(psDbHead,"ip_match",caIp_match);
    
       lUserid=0;
       lNtype=0;
       memset(caDisp,0,sizeof(caDisp));
       lStime=0;
       lEtime=0;
       memset(caIp,0,sizeof(caIp));
       memset(caMac,0,sizeof(caMac));
       memset(caPosition,0,sizeof(caPosition));
       memset(caName,0,sizeof(caName));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lNtype,
   	             UT_TYPE_STRING,48,caName,
   	             UT_TYPE_STRING,48,caDisp,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_LONG,4,&lEtime,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,15,caIp,
   	             UT_TYPE_STRING,19,caMac,
   	             UT_TYPE_STRING,32,caPosition);
   				     
   	
   	iNum = 0;
   	
   	while(iReturn == 0||iReturn==1405 && iNum < lRowNum) {
   		memset(caNtype,0,sizeof(caNtype));
   	for(i=0;i<mCount;i++)
        {
       	  if(pData[i].id==lNtype)
        	{
        		strcpy(caNtype,pData[i].name);
        		break;
        	}
        }		
   	
   	
        iNum++;
        utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum);
       utPltSetCvtHtml(1);  
       
        psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
        	 utPltPutLoopVar(psDbHead,"dispname",iNum,psUser->dispname);
        }    

        utPltPutLoopVar(psDbHead,"ntype",iNum,caNtype);
        if(lStime>0)
        utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%Y/%m/%d %H:%M",lStime));
        if(lEtime>0)
        utPltPutLoopVar(psDbHead,"etime", iNum,utTimFormat("%Y/%m/%d %H:%M",lEtime));
        utPltPutLoopVar(psDbHead,"name", iNum,caName);
	      utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
	      	      
        utPltPutLoopVar(psDbHead,"mac",iNum,caMac);
        utPltPutLoopVar(psDbHead,"position",iNum,caPosition);
        utPltPutLoopVar(psDbHead,"dispdesc",iNum,caDisp);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid);
       utPltSetCvtHtml(0);      
      
        lUserid=0;
       lNtype=0;
       memset(caDisp,0,sizeof(caDisp));
       memset(caIp,0,sizeof(caIp));
       memset(caMac,0,sizeof(caMac));
       memset(caPosition,0,sizeof(caPosition));
       memset(caName,0,sizeof(caName));
       lStime=0;
       lEtime=0;   
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lNtype,
   	             UT_TYPE_STRING,48,caName,
   	             UT_TYPE_STRING,48,caDisp,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_LONG,4,&lEtime,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,15,caIp,
   	             UT_TYPE_STRING,19,caMac,
   	             UT_TYPE_STRING,32,caPosition);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum+1 ;
    lTotRec = lCount;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
  utPltSetCvtHtml(0);
   
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);

    utPltPutVar(psDbHead,"rycx",caRycx);
    if(pData) free(pData);
          	
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_webnetidloglist.htm");
    
    ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),ncLang("1409查看虚拟身份使用日志"));
    return 0;
}

/* 显示高级查询界面  */               
int ncWebNetidDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    long iReturn,lGroupid,lGroupid0;
    char caSmt[256];
	char caService[12];
	char caFlag[8];
	pasDbCursor *psCur;
	unsigned long id;
	char caName[64];
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect;
    char caGroupid[32];
    char groupname[65]="";
    long groupid=0;
    long m=0;
    ncUserContHead  *psHead;
    char caSdate[24],caEdate[24],caSdate1[24],caEdate1[24];

    int i,iNum;
    unsigned long ltime;

    psDbHead = utPltInitDb();

   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
		            "flag",        UT_TYPE_STRING, 4, caFlag,
		            "groupid",     UT_TYPE_STRING, 12,caGroupid);
		            
		     ltime=time(0);
     
    ncUtlMonthDate(ltime,caSdate,caEdate);
    
    sprintf(caSdate1,"%s 00:00",caSdate);
    sprintf(caEdate1,"%s 23:59",caEdate);
    utPltPutVar(psDbHead,"sdate",caSdate1);
    utPltPutVar(psDbHead,"edate",caEdate1);

    psCur = pasDbOpenSql("select id,name from ncipindex order by name   ",0);
    iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&id,
    	                               UT_TYPE_STRING,63,caName);
    iNum = 0; 	                               
    while((iReturn==0)||(iReturn==1405))
    {
  	
        iNum++;       
        utPltPutLoopVarF(psDbHead,"id",   iNum,"%lu",id);
        utPltPutLoopVar(psDbHead,"name",iNum,caName);
    iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&id,
    	                               UT_TYPE_STRING,63,caName);
    }
    pasDbCloseCursor(psCur);


    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_netidlog_hsear.htm");
}




//搜索引擎使用日志
int ncWebSearhLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caTemp[2048];
    char caCond[1024];
    char caGroupid[16];
    char caDisp[36],caDisp2[36];
    char caUserid[16],caSdate[20],caEdate[20],caStime[16],caEtime[16],caSip[16];
    char caTotPg[16],caCurPg[16];
    char caUsername[64];
    char caDispname[36],caSname[32],caSvalue[128];
    char caStemptime[64],caEtemptime[64];
    unsigned long lNtype,lTime,lCurPg,lTotPg;
    char caIdfytype[16],caIdfyno[24];
    unsigned long lStime,lEtime,lUserid;
    char caGroupname[36];
    int lCount;
    int i,iNum;
    char caExport[16];
    char caMac[20];
    utPltDbHead *psDbHead;
    char caTitlename[128];
   	int iReturn,lstime,letime;
   	long lRowNum,lStartRec,lTotRec;
   	char caUsername_match[8],caIdfyno_match[8],caNetid_match[8],caIp_match[8],caMac_match[8];
 	  pasDbCursor *psCur;
 	 
  ncUserCont *psUser;

 	
	iReturn = utMsgGetSomeNVar(psMsgHead,21,
	    "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			"CurPg",      UT_TYPE_STRING, 12, caCurPg,
			"userid",    UT_TYPE_STRING, 12, caUserid,
			"groupid",   UT_TYPE_STRING, 12, caGroupid,
			"mac",UT_TYPE_STRING,31,caMac,
			"username",UT_TYPE_STRING,63,caUsername,
			"sdate",UT_TYPE_STRING,10,caSdate,
			"edate",UT_TYPE_STRING,10,caEdate,
			"stime",UT_TYPE_STRING,8,caStime,
			"etime",UT_TYPE_STRING,8,caEtime,
			"sname",UT_TYPE_STRING,16,caSname,
			"svalue",UT_TYPE_STRING,120,caSvalue,
			"sip",UT_TYPE_STRING,15,caSip,
			"mac",UT_TYPE_STRING,19,caMac,
			"idfytype",UT_TYPE_STRING,8,caIdfytype,
			"idfyno",UT_TYPE_STRING,20,caIdfyno,
   	  "exp",     UT_TYPE_STRING,10,caExport,
   	  "username_match",UT_TYPE_STRING,2,caUsername_match,
   	  "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
   	  "mac_match",UT_TYPE_STRING,2,caMac_match,
   	  "ip_match",UT_TYPE_STRING,2,caIp_match);
   	 
  // 	 sprintf(caTemp,"select dispname from ncsuser ");
  // 	 pasDbOneRecord(caTemp,0,UT_TYPE_STRING,60,caUsername);
  // 	 printf("caUsername=%s\n",caUsername);
   	              	              
  lstime=0;
	letime=0;
	if(utStrIsSpaces(caStime)){
		strcpy(caStime,"00:00");
	}
	if(utStrIsSpaces(caEdate)){
		strcpy(caEtime,"23:59");
	}
	
	
	
	if(!utStrIsSpaces(caSdate)){
		if(utStrIsSpaces(caStime)) {
			strcpy(caStime,"00:00");
		}
		sprintf(caStemptime,"%s %s",caSdate,caStime);
		lstime=utTimStrToLong("%Y/%m/%d %H:%M",caStemptime);
	}
	if(!utStrIsSpaces(caEdate)){
		if(utStrIsSpaces(caEtime)) {
			strcpy(caEtime,"23:59");
		}
		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		letime=utTimStrToLong("%Y/%m/%d %H:%M",caEtemptime);
	}	


	memset(caCond,0,sizeof(caCond));
 		

	 if(strlen(caUserid)!=0)
		sprintf(caCond+strlen(caCond)," and b.userid=%s ",caUserid);
	
		
		if(strlen(caUsername)!=0){
		
		  	sprintf(caCond+strlen(caCond)," and (a.username like '%c%s%c' or a.dispname like '%c%s%c') ",'%',caUsername,'%','%',caUsername,'%');
		 
		}
		if(strlen(caIdfyno)!=0){
							
				sprintf(caCond+strlen(caCond)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
			
		}
		if(strlen(caSdate)!=0)
		sprintf(caCond+strlen(caCond)," and b.stime>=%lu ",lstime);
		
		if(strlen(caEdate)!=0)
		sprintf(caCond+strlen(caCond)," and b.stime<=%lu ",letime);
		

		
		if(strlen(caSip)!=0){
			if(atol(caIp_match)==0)
		     sprintf(caCond+strlen(caCond)," and inet_ntoa(b.sip)='%s' ",caSip);
		   else
		     sprintf(caCond+strlen(caCond)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caSip,'%');
		}
	
		if(strlen(caMac)!=0){
			if(atol(caMac_match)==0)
		    sprintf(caCond+strlen(caCond)," and b.mac='%s' ",caMac);
		  else
		    sprintf(caCond+strlen(caCond)," and b.mac like '%c%s%c' ",'%',caMac,'%');
		  }
		if(strlen(caSname)>0){
			sprintf(caCond+strlen(caCond)," and b.sname like '%c%s%c' ",'%',caSname,'%');
		}
		
		if(strlen(caSvalue)!=0){
    sprintf(caCond+strlen(caCond)," and b.svalue like '%c%s%c' ",'%',caSvalue,'%');
    }
		
		if(strlen(caGroupid)>0){
			if(strcmp(caGroupid,"0")==0)
			 sprintf(caCond+strlen(caCond)," and a.groupid=%s ",caGroupid);
			else
			 sprintf(caCond+strlen(caCond)," and (a.groupid=%s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
		}
		strcat(caCond," order by b.stime desc ");
		    

    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;

 

    if(!utStrIsSpaces(caExport)){
    lRowNum = 999999;
    }
    else
    lRowNum=32;
    lStartRec = (lCurPg - 1) * lRowNum;    


  if(strstr(caCond,"a.")){
  	sprintf(caTemp,"select count(*) from ncsearchlog b,ncuser a  where b.userid=a.userid ");
  }
  else
    sprintf(caTemp,"select count(*) from ncsearchlog b  where 1=1");
    
 
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
    
  
printf("caTemp=%s\n",caTemp);
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);

    if(iReturn != 0) {
    	
		   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
       return 0;
    }  
    if(strstr(caCond,"a.")){ 
    sprintf(caTemp,"select b.userid,b.sname,inet_ntoa(b.sip),b.mac,b.stime,b.svalue from ncsearchlog b,ncuser a where b.userid=a.userid  ");
  }
  else{
  	    sprintf(caTemp,"select b.userid,b.sname,inet_ntoa(b.sip),b.mac,b.stime,b.svalue from ncsearchlog b where 1=1  ");
  }
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
    
    sprintf(caTemp+strlen(caTemp)," limit %lu,%lu ",lStartRec,lRowNum);
    
printf("caTemp=%s\n",caTemp);  
    psCur = pasDbOpenSql(caTemp,0);

    if(psCur == NULL) {
    
      utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
        return 0;
    }   

    if(!utStrIsSpaces(caExport)){
    	FILE *fp;
    	char caFilename[32];
    	char caFile[128];
    	char caPath[128];
        sprintf(caFilename,"ncsearchlog%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
    	fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
           
             utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
            return 0;
        }
        
        fprintf(fp,"搜索引擎使用日志\n");
        ncUtlPrintCsv(fp,6,UT_TYPE_STRING,"搜索引擎名称",
                           UT_TYPE_STRING,ncLang("1403使用时间"),
                           UT_TYPE_STRING,ncLang("1405上网IP"),
                           UT_TYPE_STRING,ncLang("0072MAC地址"),
                           UT_TYPE_STRING,ncLang("0728用户名"),
                           UT_TYPE_STRING,"搜索关键字");

       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caMac,0,sizeof(caMac));
       memset(caSname,0,sizeof(caSname));
       memset(caSvalue,0,sizeof(caSvalue));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,16,caSname,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,19,caMac,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,127,caSvalue);
  	                              
        while((iReturn == 0)||(iReturn==1405)) { 
        	
        
   	    psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
        	strcpy(caDisp2,psUser->dispname);
        	 if(psUser->psGroup)
	           strcpy(caGroupname,psUser->psGroup->groupname);
	         else{
	         	strcpy(caGroupname,"");
	        }
        	
        	
        }
        else{
        	strcpy(caDisp2,"");
        	strcpy(caGroupname,"");
        }
        
        strcpy(caStime,utTimFormat("%Y/%m/%d %H:%M",lStime));
              
        ncUtlPrintCsv(fp,6,UT_TYPE_STRING,caSname,
                           UT_TYPE_STRING,caStime,
                           UT_TYPE_STRING,caSip,
                           UT_TYPE_STRING,caMac,
                           UT_TYPE_STRING,caDisp2,
                           UT_TYPE_STRING,caSvalue);
                    
                     
       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caMac,0,sizeof(caMac));
       memset(caSname,0,sizeof(caSname));
       memset(caSvalue,0,sizeof(caSvalue));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,16,caSname,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,19,caMac,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,127,caSvalue);
        }
        
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
    
        return 0;
       
    }
         
    psDbHead = utPltInitDb();
    
  
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caUsername,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),"未知"));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
   
    utPltPutVar(psDbHead,"title",caTitlename);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"username",caUsername);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"stime",caStime);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"mac",caMac);
  
    utPltPutVar(psDbHead,"idfytype",caIdfytype);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"username_match",caUsername_match);

    utPltPutVar(psDbHead,"sname",caSname);
    utPltPutVar(psDbHead,"svalue",caSvalue);
   
 
          lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caMac,0,sizeof(caMac));
       memset(caSname,0,sizeof(caSname));
       memset(caSvalue,0,sizeof(caSvalue));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,16,caSname,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,19,caMac,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,127,caSvalue);
   	
   	iNum = 0;
   	
   	while(iReturn == 0||iReturn==1405 && iNum < lRowNum) {

   	     psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
        	strcpy(caDisp2,psUser->dispname);
        	 if(psUser->psGroup)
	           strcpy(caGroupname,psUser->psGroup->groupname);
	         else{
	         	strcpy(caGroupname,"");
	        }
        	
        	
        }
        else{
        	strcpy(caDisp2,"");
        	strcpy(caGroupname,"");
        }


iNum++;
        if(lStime>0)
        utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%Y/%m/%d %H:%M",lStime));
      
	      utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
	      utPltPutLoopVar(psDbHead,"sname",iNum,caSname);
	      utPltPutLoopVar(psDbHead,"groupname",iNum,caGroupname); 
        utPltPutLoopVar(psDbHead,"mac",iNum,caMac);

        utPltPutLoopVar(psDbHead,"dispname",iNum,caDisp2);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid);
       utPltSetCvtHtml(1); 
       utPltPutLoopVar(psDbHead,"svalue",iNum,caSvalue);
       
       
       utPltSetCvtHtml(0);      
      
      
       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caMac,0,sizeof(caMac));
       memset(caSname,0,sizeof(caSname));
       memset(caSvalue,0,sizeof(caSvalue));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,16,caSname,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,19,caMac,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,127,caSvalue);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum+1 ;
    lTotRec = lCount;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
  utPltSetCvtHtml(0);
   
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);

   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_websearchloglist.htm");
    
    return 0;
}

//数据库操作日志
int ncWebDbOperLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caTemp[2048];
    char caCond[1024],caSorta[12],caOrdera[12];
    char caGroupid[16];
    char caDisp[32],caDisp2[32];
    char caUserid[16],caSdate[20],caEdate[20],caName1[64],caIp[20],caMac[20],caPosition[36];
    char caTotPg[16],caCurPg[16];
    char caUsername[64];
    char caStime[20],caEtime[20];
    char caDispname[32],caName[64],caRycx[32];
    unsigned long lTime,lCurPg,lTotPg;
    char caIdfytype[16],caIdfyno[24];
    
    int mCount,lCount;
    int i,iNum;
    char caExport[16];
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lRowNum,lStartRec,lTotRec;
   	char caUsername_match[8],caIdfyno_match[8],caNetid_match[8],caIp_match[8],caMac_match[8];
 	  pasDbCursor *psCur;
 	  char caUsername2[32];
    char caTitle[36];
 	  char caSdate_all[20],caEdate_all[20];
 	  char caMsg[1024];
 	  char *p;
 	  long len;
 	  long lUserid,lStime,lDbtype,lDport,lOpttype,lStatus,lBytes;
 	  char caSip[20],caDbuser[32],caDbname[32],caSqlsmt[4004],caDbtype[16],caOpt[16],caStatus[16];
 	  char caDbtype_c[16],caOpttype_c[16],caSqlsmt_c[128];
 	  char caDip[20],caTid[16];
 	  char caTname[36],caDip_c[20];
 	  char caSqlsmt_temp[4004];
 	  memset(caUsername2,0,sizeof(caUsername2));
 	  dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
 	  
 	
 	char caTitlename[128];
 	unsigned long lCurtime;
  ncUserCont *psUser;
 	lCurtime=time(0);
printf("bbbaaa\n"); 	
	iReturn = utMsgGetSomeNVar(psMsgHead,21,
	    "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			"CurPg",      UT_TYPE_STRING, 12, caCurPg,
			"userid",    UT_TYPE_STRING, 10, caUserid,
			"groupid",   UT_TYPE_STRING, 10, caGroupid,
			"dip",       UT_TYPE_STRING,15,caDip_c,
			"tid",       UT_TYPE_STRING,10,caTid,
			"mac",UT_TYPE_STRING,20,caMac,
			"username",UT_TYPE_STRING,32,caUsername,
			"sdate",UT_TYPE_STRING,19,caSdate,
			"edate",UT_TYPE_STRING,19,caEdate,
			"ip",UT_TYPE_STRING,15,caIp,
			"position",UT_TYPE_STRING,30,caPosition,
			"idfytype",UT_TYPE_STRING,8,caIdfytype,
			"idfyno",UT_TYPE_STRING,20,caIdfyno,
			"stime",UT_TYPE_STRING,5,caStime,
			"etime",UT_TYPE_STRING,5,caEtime,
   	  "exp",     UT_TYPE_STRING,10,caExport,
   	  "dbtype",UT_TYPE_STRING,8,caDbtype_c,
   	  "opttype",UT_TYPE_STRING,8,caOpttype_c,
   	  "sqlsmt",UT_TYPE_STRING,64,caSqlsmt_c,
   	  "ip_match",UT_TYPE_STRING,2,caIp_match);
   	 
   	 memset(caTitle,0,sizeof(caTitle));
   	if(strlen(caUserid)>0){
   		  psUser = ncUtlGetUserContById(psShmHead,atol(caUserid));
        if(psUser) {
        	strcpy(caTitle,psUser->dispname);
        }
     }  
   	 if(strlen(caGroupid)>0){
   	 	sprintf(caTemp,"select groupname from ncgroup where groupid=%s ",caGroupid);
   	 	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caTitle);
   	}
   	   
   	              	              
    if(strlen(caStime)==0){
    	strcpy(caStime,"00:00");
    }
    if(strlen(caEtime)==0){
    	strcpy(caEtime,"23:59");
    }



	memset(caCond,0,sizeof(caCond));
 		

	 if(strlen(caUserid)!=0)
		sprintf(caCond+strlen(caCond)," and b.userid=%s ",caUserid);
	
		
		if(strlen(caUsername)!=0){
			if(atol(caUsername_match)==0)
			{
			lUserid=0;
			sprintf(caTemp,"select userid from ncuser where username='%s' or dispname='%s' ",caUsername,caUsername);
			pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lUserid);
			sprintf(caCond+strlen(caCond)," and b.userid=%lu ",lUserid);
		  }
		  else{
		  	sprintf(caCond+strlen(caCond)," and (a.username like '%c%s%c' or a.dispname like '%c%s%c') ",'%',caUsername,'%','%',caUsername,'%');
		  }
		}
		if(strlen(caIdfyno)!=0){

			if(atol(caIdfyno_match)==0){
				lUserid=0;
				sprintf(caTemp,"select userid from ncuser where idfyno='%s' ",caIdfyno);
				
				pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lUserid);
				sprintf(caCond+strlen(caCond)," and b.userid=%lu ",lUserid);
			}
			else{
				sprintf(caCond+strlen(caCond)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
			}
		}
		if(strlen(caSdate)!=0){
			sprintf(caSdate_all,"%s %s",caSdate,caStime);
		  sprintf(caCond+strlen(caCond)," and b.stime>=%lu ",utTimStrToLong("%Y/%m/%d %H:%M",caSdate_all));
	 }
		
		if(strlen(caEdate)!=0){
			sprintf(caEdate_all,"%s %s",caEdate,caEtime);
		  sprintf(caCond+strlen(caCond)," and b.stime<=%lu ",utTimStrToLong("%Y/%m/%d %H:%M",caEdate_all));
	  }

		
		if(strlen(caIp)!=0){
			if(atol(caIp_match)==0)
		     sprintf(caCond+strlen(caCond)," and inet_ntoa(b.sip)='%s' ",caIp);
		   else
		     sprintf(caCond+strlen(caCond)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caIp,'%');
		}
		if(strlen(caTid)>0){
			sprintf(caCond+strlen(caCond)," and b.tid=%s ",caTid);
		}
		if(strlen(caDip_c)>0){
			sprintf(caCond+strlen(caCond)," and inet_ntoa(b.dip)='%s' ",caDip_c);
		}
		
	  if(strlen(caDbtype_c)>0){
	  	sprintf(caCond+strlen(caCond)," and b.dbtype=%d ",atol(caDbtype_c));
	  }
	  if(strlen(caOpttype_c)>0){
	  	sprintf(caCond+strlen(caCond)," and b.opttype=%d ",atol(caOpttype_c));
	  }
	  if(strlen(caSqlsmt_c)>0){
	  	sprintf(caCond+strlen(caCond)," and b.sqlsmt like '%c%s%c' ",'%',caSqlsmt_c,'%');
	  }
		
		
		if(strlen(caGroupid)>0){
			if(strcmp(caGroupid,"0")==0)
			 sprintf(caCond+strlen(caCond)," and a.groupid=%s ",caGroupid);
			else
			 sprintf(caCond+strlen(caCond)," and (a.groupid=%s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
		}
		strcat(caCond," order by b.stime desc ");
		    

    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;

 

    if(!utStrIsSpaces(caExport)){
    lRowNum = 999999;
    }
    else
    lRowNum=32;
    lStartRec = (lCurPg - 1) * lRowNum;    


  if(strstr(caCond,"a.")){
  	sprintf(caTemp,"select count(*) from ncdbseslog b,ncuser a  where b.userid=a.userid ");
  }
  else
    sprintf(caTemp,"select count(*) from ncdbseslog b  where 1=1");
    
 
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
    
  
printf("caTemp=%s\n",caTemp);
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);

    if(iReturn != 0) {
    
		   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
       return 0;
    }  
    if(strstr(caCond,"a.")){ 
    sprintf(caTemp,"select b.userid,b.stime,b.dbtype,inet_ntoa(b.sip),inet_ntoa(b.dip),b.dport,b.dbuser,b.dbname,b.opttype,b.sqlsmt,b.status,b.bytes,b.mac from ncdbseslog b,ncuser a where b.userid=a.userid  ");
  }
  else{
  	    sprintf(caTemp,"select b.userid,b.stime,b.dbtype,inet_ntoa(b.sip),inet_ntoa(b.dip),b.dport,b.dbuser,b.dbname,b.opttype,b.sqlsmt,b.status,b.bytes,b.mac from ncdbseslog b where 1=1 ");
  }
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
    
    sprintf(caTemp+strlen(caTemp)," limit %lu,%lu ",lStartRec,lRowNum);
    
printf("caTemp=%s\n",caTemp);  
    psCur = pasDbOpenSql(caTemp,0);

    if(psCur == NULL) {
   
      utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
        return 0;
    }   

    if(!utStrIsSpaces(caExport)){
    	FILE *fp;
    	char caFilename[32];
    	char caFile[128];
    	char caPath[128];
        sprintf(caFilename,"ncnetlog%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
    	fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
          
             utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
            return 0;
        }
        
        fprintf(fp,"数据库操作日志\n");
        ncUtlPrintCsv(fp,11,UT_TYPE_STRING,"上网用户",
                           UT_TYPE_STRING,"源IP",
                           UT_TYPE_STRING,"源MAC",
                           UT_TYPE_STRING,ncLang("1403使用时间"),
                           UT_TYPE_STRING,"目标ip",
                           UT_TYPE_STRING,"操作类型",
                           UT_TYPE_STRING,"操作内容",
                           UT_TYPE_STRING,"数据库类型",
                           UT_TYPE_STRING,"数据库名/用户",
                           UT_TYPE_STRING,"状态",
                           UT_TYPE_STRING,"流量");

       lUserid=0;   
       memset(caDisp,0,sizeof(caDisp));
       lStime=0;
       lDbtype=0;
       memset(caSip,0,sizeof(caSip));
       memset(caDip,0,sizeof(caDip));
       lDport=0;
       memset(caDbuser,0,sizeof(caDbuser));
       memset(caDbname,0,sizeof(caDbname));
       lOpttype=0;
       memset(caSqlsmt,0,sizeof(caSqlsmt));
       lStatus=0;
       lBytes=0;
       memset(caMac,0,sizeof(caMac));
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_LONG,4,&lDbtype,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,15,caDip,
   	             UT_TYPE_LONG,4,&lDport,
   	             UT_TYPE_STRING,30,caDbuser,
   	             UT_TYPE_STRING,30,caDbname,
   	             UT_TYPE_LONG,4,&lOpttype,
   	             UT_TYPE_STRING,4000,caSqlsmt,
   	             UT_TYPE_LONG,4,&lStatus,
   	             UT_TYPE_LONG,4,&lBytes,
   	             UT_TYPE_STRING,19,caMac);
   	           
  	                              
        while((iReturn == 0)||(iReturn==1405)) { 
      
        
        psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
        	strcpy(caDisp2,psUser->dispname);
        }
        else{
        	strcpy(caDisp2,"");
        }
        strcpy(caStime,utTimFormat("%Y/%m/%d %H:%M",lStime));
     
      strcpy(caDbtype,ncGetDbtypebyid(lDbtype));
     strcpy(caOpt,ncGetDbOpnamebyid(lOpttype));
      
      if(lStatus==0){
      	strcpy(caStatus,"成功");
      }
      else{
      	strcpy(caStatus,"");
      } 
		   if(p=strstr(caSqlsmt,"values")){ 
		   	len=strlen(caSqlsmt)-strlen(p);  
		   	caSqlsmt[len]='\0';
		   	sprintf(caSqlsmt+strlen(caSqlsmt),"******");
		   }	
		   else if(p=strstr(caSqlsmt,"VALUES")){ 
		   		len=strlen(caSqlsmt)-strlen(p);  
		   		caSqlsmt[len]='\0';
		   } 	
		               
        ncUtlPrintCsv(fp,10,UT_TYPE_STRING,caDisp2,
                       UT_TYPE_STRING,caSip,
                       UT_TYPE_STRING,caStime,
                       UT_TYPE_STRING,caDip,
                       UT_TYPE_STRING,caMac,
                       UT_TYPE_STRING,caDbtype,
                       UT_TYPE_STRING,caSqlsmt,
                       UT_TYPE_STRING,caDbtype,
                       UT_TYPE_STRING,caDbname,
                       UT_TYPE_STRING,caStatus,
                       UT_TYPE_LONG,lBytes);
                       
                      
                     
       lUserid=0;   
       memset(caDisp,0,sizeof(caDisp));
       lStime=0;
       lDbtype=0;
       memset(caSip,0,sizeof(caSip));
       memset(caDip,0,sizeof(caDip));
       lDport=0;
       memset(caDbuser,0,sizeof(caDbuser));
       memset(caDbname,0,sizeof(caDbname));
       lOpttype=0;
       memset(caSqlsmt,0,sizeof(caSqlsmt));
       lStatus=0;
       lBytes=0;
          
        
    memset(caMac,0,sizeof(caMac));        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_LONG,4,&lDbtype,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,15,caDip,
   	             UT_TYPE_LONG,4,&lDport,
   	             UT_TYPE_STRING,30,caDbuser,
   	             UT_TYPE_STRING,30,caDbname,
   	             UT_TYPE_LONG,4,&lOpttype,
   	             UT_TYPE_STRING,4000,caSqlsmt,
   	             UT_TYPE_LONG,4,&lStatus,
   	             UT_TYPE_LONG,4,&lBytes,
   	             UT_TYPE_STRING,19,caMac);
        }
        
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
     ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"导出数据库操作日志");   
        return 0;
       
    }
         
    psDbHead = utPltInitDb();
    	
   

    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"username",caUsername);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"stime",caStime);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"ip",caIp);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"username_match",caUsername_match);
    utPltPutVar(psDbHead,"idfyno_match",caIdfyno_match);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"ip_match",caIp_match);
    utPltPutVar(psDbHead,"dbtype",caDbtype_c);
    utPltPutVar(psDbHead,"opttype",caOpttype_c);
    utPltPutVar(psDbHead,"sqlsmt",caSqlsmt_c);
    utPltPutVar(psDbHead,"tid",caTid);
    utPltPutVar(psDbHead,"dip",caDip_c);

   
    
       lUserid=0;   
       memset(caDisp,0,sizeof(caDisp));
       lStime=0;
       lDbtype=0;
       memset(caSip,0,sizeof(caSip));
       memset(caDip,0,sizeof(caDip));
       lDport=0;
       memset(caDbuser,0,sizeof(caDbuser));
       memset(caDbname,0,sizeof(caDbname));
       lOpttype=0;
       memset(caSqlsmt,0,sizeof(caSqlsmt));
       lStatus=0;
       lBytes=0;
          
        
       memset(caMac,0,sizeof(caMac));
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_LONG,4,&lDbtype,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,15,caDip,
   	             UT_TYPE_LONG,4,&lDport,
   	             UT_TYPE_STRING,30,caDbuser,
   	             UT_TYPE_STRING,30,caDbname,
   	             UT_TYPE_LONG,4,&lOpttype,
   	             UT_TYPE_STRING,4000,caSqlsmt,
   	             UT_TYPE_LONG,4,&lStatus,
   	             UT_TYPE_LONG,4,&lBytes,
   	             UT_TYPE_STRING,19,caMac);
   				     
   	
   	iNum = 0;
   	
   	while(iReturn == 0||iReturn==1405 && iNum < lRowNum) {
 		
		//屏蔽values下的内容


		   if(p=strstr(caSqlsmt,"values")){ 
		   	len=strlen(caSqlsmt)-strlen(p);  
		   	caSqlsmt[len]='\0';
		    sprintf(caSqlsmt+strlen(caSqlsmt),"values(******)");
		   }	
		   else if(p=strstr(caSqlsmt,"VALUES")){ 
		   		len=strlen(caSqlsmt)-strlen(p);  
		   		caSqlsmt[len]='\0';
		   		sprintf(caSqlsmt+strlen(caSqlsmt),"VALUES(******)");
		   } 	
        iNum++;
        utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum);
       utPltSetCvtHtml(1);  
     
        psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
        	 utPltPutLoopVar(psDbHead,"dispname",iNum,psUser->dispname);
        }    


    strcpy(caDbtype,ncGetDbtypebyid(lDbtype));
     strcpy(caOpt,ncGetDbOpnamebyid(lOpttype));
      
      if(lStatus==0){
      	strcpy(caStatus,"成功");
      }
      else{
      	strcpy(caStatus,"");
      } 

      
        if(lStime>0)
        utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%Y/%m/%d %H:%M",lStime));
      
	      utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
	      utPltPutLoopVar(psDbHead,"dip",iNum,caDip);
	      utPltPutLoopVarF(psDbHead,"dport",iNum,"%d",lDport);
	      utPltPutLoopVar(psDbHead,"dbusr",iNum,caDbuser);
	      utPltPutLoopVar(psDbHead,"dbname",iNum,caDbname);
	      utPltPutLoopVar(psDbHead,"sql",iNum,caSqlsmt);
	      utPltPutLoopVar(psDbHead,"dbtype",iNum,caDbtype);
	      utPltPutLoopVar(psDbHead,"optype",iNum,caOpt);
	      utPltPutLoopVar(psDbHead,"status",iNum,caStatus); 
	      utPltPutLoopVarF(psDbHead,"dbytes",iNum,"%d",lBytes);
        utPltPutLoopVar(psDbHead,"mac",iNum,caMac);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid);
       utPltSetCvtHtml(0);      
      
       lUserid=0;   
       memset(caDisp,0,sizeof(caDisp));
       lStime=0;
       lDbtype=0;
       memset(caSip,0,sizeof(caSip));
       memset(caDip,0,sizeof(caDip));
       lDport=0;
       memset(caDbuser,0,sizeof(caDbuser));
       memset(caDbname,0,sizeof(caDbname));
       lOpttype=0;
       memset(caSqlsmt,0,sizeof(caSqlsmt));
       lStatus=0;
       lBytes=0;
          
        
       memset(caMac,0,sizeof(caMac));
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_LONG,4,&lDbtype,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,15,caDip,
   	             UT_TYPE_LONG,4,&lDport,
   	             UT_TYPE_STRING,30,caDbuser,
   	             UT_TYPE_STRING,30,caDbname,
   	             UT_TYPE_LONG,4,&lOpttype,
   	             UT_TYPE_STRING,4000,caSqlsmt,
   	             UT_TYPE_LONG,4,&lStatus,
   	             UT_TYPE_LONG,4,&lBytes,
   	             UT_TYPE_STRING,19,caMac);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum+1 ;
    lTotRec = lCount;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    sprintf(caTemp,"select tname from ncdbmontype where tid=%s ",caTid); 
    memset(caTname,0,sizeof(caTname));
    pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caTname);

     utPltPutVar(psDbHead,"title",caTname);
    
  utPltSetCvtHtml(0);
   
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);

    utPltPutVar(psDbHead,"rycx",caRycx);

          	
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_websqlloglist.htm");
    
    ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"查看数据库操作日志");
    return 0;
}



//网站登录日志
int ncWebSiteLoginLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caTemp[2048];
    char caCond[1024];
    char caGroupid[16];
    char caDisp[36],caDisp2[36];
    char caUserid[16],caSdate[20],caEdate[20],caStime[16],caEtime[16],caSip[16];
    char caTotPg[16],caCurPg[16];
    char caUsername[64];
    char caDispname[36],caName[36],caUrl[128],caPassword[36];
    char caStemptime[64],caEtemptime[64];
    unsigned long lNtype,lTime,lCurPg,lTotPg;
    char caIdfytype[16],caIdfyno[24];
    unsigned long lStime,lEtime,lUserid;
    char caGroupname[36];
    int lCount;
    int i,iNum;
    char caExport[16];
    char caMac[20];
    utPltDbHead *psDbHead;
    char caTitlename[128];
   	int iReturn,lstime,letime;
   	long lRowNum,lStartRec,lTotRec;
   	char caUsername_match[8],caIdfyno_match[8],caNetid_match[8],caIp_match[8],caMac_match[8];
 	  pasDbCursor *psCur;
 	 
  ncUserCont *psUser;

 	
	iReturn = utMsgGetSomeNVar(psMsgHead,21,
	    "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			"CurPg",      UT_TYPE_STRING, 12, caCurPg,
			"userid",    UT_TYPE_STRING, 12, caUserid,
			"groupid",   UT_TYPE_STRING, 12, caGroupid,
			"mac",UT_TYPE_STRING,31,caMac,
			"username",UT_TYPE_STRING,63,caUsername,
			"sdate",UT_TYPE_STRING,10,caSdate,
			"edate",UT_TYPE_STRING,10,caEdate,
			"stime",UT_TYPE_STRING,8,caStime,
			"etime",UT_TYPE_STRING,8,caEtime,
			"url",UT_TYPE_STRING,127,caUrl,
			"name",UT_TYPE_STRING,31,caName,
			"sip",UT_TYPE_STRING,15,caSip,
			"mac",UT_TYPE_STRING,19,caMac,
			"idfytype",UT_TYPE_STRING,8,caIdfytype,
			"idfyno",UT_TYPE_STRING,20,caIdfyno,
   	  "exp",     UT_TYPE_STRING,10,caExport,
   	  "username_match",UT_TYPE_STRING,2,caUsername_match,
   	  "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
   	  "mac_match",UT_TYPE_STRING,2,caMac_match,
   	  "ip_match",UT_TYPE_STRING,2,caIp_match);
   	 
   	              	              
  lstime=0;
	letime=0;
	if(utStrIsSpaces(caStime)){
		strcpy(caStime,"00:00");
	}
	if(utStrIsSpaces(caEdate)){
		strcpy(caEtime,"23:59");
	}
	
	
	
	if(!utStrIsSpaces(caSdate)){
		if(utStrIsSpaces(caStime)) {
			strcpy(caStime,"00:00");
		}
		sprintf(caStemptime,"%s %s",caSdate,caStime);
		lstime=utTimStrToLong("%Y/%m/%d %H:%M",caStemptime);
	}
	if(!utStrIsSpaces(caEdate)){
		if(utStrIsSpaces(caEtime)) {
			strcpy(caEtime,"23:59");
		}
		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		letime=utTimStrToLong("%Y/%m/%d %H:%M",caEtemptime);
	}	


	memset(caCond,0,sizeof(caCond));
 		

	 if(strlen(caUserid)!=0)
		sprintf(caCond+strlen(caCond)," and b.userid=%s ",caUserid);
	
		
		if(strlen(caUsername)!=0){
		
		  	sprintf(caCond+strlen(caCond)," and (a.username like '%c%s%c' or a.dispname like '%c%s%c') ",'%',caUsername,'%','%',caUsername,'%');
		 
		}
		if(strlen(caIdfyno)!=0){
							
				sprintf(caCond+strlen(caCond)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
			
		}
		if(strlen(caSdate)!=0)
		sprintf(caCond+strlen(caCond)," and b.addtime>=%lu ",lstime);
		
		if(strlen(caEdate)!=0)
		sprintf(caCond+strlen(caCond)," and b.addtime<=%lu ",letime);
		

		
		if(strlen(caSip)!=0){
			if(atol(caIp_match)==0)
		     sprintf(caCond+strlen(caCond)," and inet_ntoa(b.sip)='%s' ",caSip);
		   else
		     sprintf(caCond+strlen(caCond)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caSip,'%');
		}
	
		
		if(strlen(caName)>0){
			sprintf(caCond+strlen(caCond)," and b.username like '%c%s%c' ",'%',caName,'%');
		}
		
		if(strlen(caUrl)!=0){
    sprintf(caCond+strlen(caCond)," and b.url like '%c%s%c' ",'%',caUrl,'%');
    }
		
		if(strlen(caGroupid)>0){
			if(strcmp(caGroupid,"0")==0)
			 sprintf(caCond+strlen(caCond)," and a.groupid=%s ",caGroupid);
			else
			 sprintf(caCond+strlen(caCond)," and (a.groupid=%s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
		}
		strcat(caCond," order by b.sid desc ");
		    

    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;

 

    if(!utStrIsSpaces(caExport)){
    lRowNum = 999999;
    }
    else
    lRowNum=32;
    lStartRec = (lCurPg - 1) * lRowNum;    


  if(strstr(caCond,"a.")){
  	sprintf(caTemp,"select count(*) from nchttpacclog b,ncuser a  where b.userid=a.userid ");
  }
  else
    sprintf(caTemp,"select count(*) from nchttpacclog b  where 1=1");
    
 
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
    
  
printf("caTemp=%s\n",caTemp);
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);

    if(iReturn != 0) {
    	
		   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
       return 0;
    }  
    if(strstr(caCond,"a.")){ 
    sprintf(caTemp,"select b.userid,b.username,inet_ntoa(b.sip),b.url,b.addtime,b.passwd from nchttpacclog b,ncuser a where b.userid=a.userid  ");
  }
  else{
  	    sprintf(caTemp,"select b.userid,b.username,inet_ntoa(b.sip),b.url,b.addtime,b.passwd from nchttpacclog b where 1=1  ");
  }
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
    
    sprintf(caTemp+strlen(caTemp)," limit %lu,%lu ",lStartRec,lRowNum);
    
printf("caTemp=%s\n",caTemp);  
    psCur = pasDbOpenSql(caTemp,0);

    if(psCur == NULL) {
    
      utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
        return 0;
    }   

    if(!utStrIsSpaces(caExport)){
 
    	FILE *fp;
    	char caFilename[32];
    	char caFile[128];
    	char caPath[128];
        sprintf(caFilename,"ncswebaccountlog%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
    	fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
           
             utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
            return 0;
        }
        
        fprintf(fp,"网站帐号登录日志\n");
        ncUtlPrintCsv(fp,6,UT_TYPE_STRING,"网址帐号",
                           UT_TYPE_STRING,ncLang("1403使用时间"),
                           UT_TYPE_STRING,ncLang("1405上网IP"),
                           UT_TYPE_STRING,"网址",
                           UT_TYPE_STRING,"部门",
                           UT_TYPE_STRING,ncLang("0728用户名"));
                          

       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caName,0,sizeof(caName));
       memset(caUrl,0,sizeof(caUrl));
       memset(caPassword,0,sizeof(caPassword));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,31,caName,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,127,caUrl,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,31,caPassword);
  	                              
        while((iReturn == 0)||(iReturn==1405)) { 
        	
        
        psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
        	strcpy(caDisp2,psUser->dispname);
        	   if(psUser->psGroup)
		           strcpy(caGroupname,psUser->psGroup->groupname);
		         else{
		         	strcpy(caGroupname,"");
		         	
		        }
        }
        else{
        	strcpy(caDisp2,"");
        	strcpy(caGroupname,"");
        }

        
        strcpy(caStime,utTimFormat("%Y/%m/%d %H:%M",lStime));
              
                     ncUtlPrintCsv(fp,6,UT_TYPE_STRING,caName,
                           UT_TYPE_STRING,caStime,
                           UT_TYPE_STRING,caSip,
                           UT_TYPE_STRING,caUrl,
                           UT_TYPE_STRING,caGroupname,
                           UT_TYPE_STRING,caDisp2); 
              
              
       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caName,0,sizeof(caName));
       memset(caUrl,0,sizeof(caUrl));
       memset(caPassword,0,sizeof(caPassword));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,31,caName,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,127,caUrl,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,31,caPassword);
                    

        }
        
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
    
        return 0;
       
    }
         
    psDbHead = utPltInitDb();
    
  
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caUsername,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),"未知"));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
   
    utPltPutVar(psDbHead,"title",caTitlename);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"username",caUsername);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"stime",caStime);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"mac",caMac);
  
    utPltPutVar(psDbHead,"idfytype",caIdfytype);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"username_match",caUsername_match);

    utPltPutVar(psDbHead,"url",caUrl);
    utPltPutVar(psDbHead,"name",caName);
   
       
       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caName,0,sizeof(caName));
       memset(caUrl,0,sizeof(caUrl));
       memset(caPassword,0,sizeof(caPassword));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,31,caName,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,127,caUrl,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,31,caPassword);
   	
   	iNum = 0;
   	
   	while(iReturn == 0||iReturn==1405 && iNum < lRowNum) {

   	     psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
        	strcpy(caDisp2,psUser->dispname);
        	 if(psUser->psGroup)
	           strcpy(caGroupname,psUser->psGroup->groupname);
	         else{
	         	strcpy(caGroupname,"");
	        }
        	
        	
        }
        else{
        	strcpy(caDisp2,"");
        	strcpy(caGroupname,"");
        }


iNum++;
        if(lStime>0)
        utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%Y/%m/%d %H:%M",lStime));
      
	      utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
	      utPltPutLoopVar(psDbHead,"name",iNum,caName);
	      if(strlen(caPassword)>0){
	      	utPltPutLoopVar(psDbHead,"password",iNum,"******");
	      }
	      utPltPutLoopVar(psDbHead,"groupname",iNum,caGroupname); 
       

        utPltPutLoopVar(psDbHead,"dispname",iNum,caDisp2);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid);
       utPltSetCvtHtml(1); 
        utPltPutLoopVar(psDbHead,"url",iNum,caUrl);
       
       
       utPltSetCvtHtml(0);      
      
       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caName,0,sizeof(caName));
       memset(caUrl,0,sizeof(caUrl));
       memset(caPassword,0,sizeof(caPassword));
          
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,31,caName,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,127,caUrl,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,31,caPassword);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum+1 ;
    lTotRec = lCount;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
  utPltSetCvtHtml(0);
   
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);

   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_websitloginlist.htm");
    
    return 0;
}


//网站会员注册日志
int ncWebSiteRegLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caTemp[2048];
    char caCond[1024];
    char caGroupid[16];
    char caDisp[36],caDisp2[36];
    char caUserid[16],caSdate[20],caEdate[20],caStime[16],caEtime[16],caSip[16];
    char caTotPg[16],caCurPg[16];
    char caUsername[64];
    char caDispname[36],caName[36],caUrl[128],caPassword[36];
    char caStemptime[64],caEtemptime[64];
    unsigned long lNtype,lTime,lCurPg,lTotPg;
    char caIdfytype[16],caIdfyno[24];
    unsigned long lStime,lEtime,lUserid;
    char caGroupname[36];
    int lCount;
    int i,iNum;
    char caExport[16];
    char caMac[20];
    utPltDbHead *psDbHead;
    char caTitlename[128],caEmail[64];
   	int iReturn,lstime,letime;
   	long lRowNum,lStartRec,lTotRec;
   	char caUsername_match[8],caIdfyno_match[8],caNetid_match[8],caIp_match[8],caMac_match[8];
 	  pasDbCursor *psCur;
 	 
  ncUserCont *psUser;

 	
	iReturn = utMsgGetSomeNVar(psMsgHead,21,
	    "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			"CurPg",      UT_TYPE_STRING, 12, caCurPg,
			"userid",    UT_TYPE_STRING, 12, caUserid,
			"groupid",   UT_TYPE_STRING, 12, caGroupid,
			"mac",UT_TYPE_STRING,31,caMac,
			"username",UT_TYPE_STRING,63,caUsername,
			"sdate",UT_TYPE_STRING,10,caSdate,
			"edate",UT_TYPE_STRING,10,caEdate,
			"stime",UT_TYPE_STRING,8,caStime,
			"etime",UT_TYPE_STRING,8,caEtime,
			"url",UT_TYPE_STRING,127,caUrl,
			"email",UT_TYPE_STRING,60,caEmail,
			"name",UT_TYPE_STRING,31,caName,
			"sip",UT_TYPE_STRING,15,caSip,
			"idfytype",UT_TYPE_STRING,8,caIdfytype,
			"idfyno",UT_TYPE_STRING,20,caIdfyno,
   	  "exp",     UT_TYPE_STRING,10,caExport,
   	  "username_match",UT_TYPE_STRING,2,caUsername_match,
   	  "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
   	  "mac_match",UT_TYPE_STRING,2,caMac_match,
   	  "ip_match",UT_TYPE_STRING,2,caIp_match);
   	 
   	              	              
  lstime=0;
	letime=0;
	if(utStrIsSpaces(caStime)){
		strcpy(caStime,"00:00");
	}
	if(utStrIsSpaces(caEdate)){
		strcpy(caEtime,"23:59");
	}
	
	
	
	if(!utStrIsSpaces(caSdate)){
		if(utStrIsSpaces(caStime)) {
			strcpy(caStime,"00:00");
		}
		sprintf(caStemptime,"%s %s",caSdate,caStime);
		lstime=utTimStrToLong("%Y/%m/%d %H:%M",caStemptime);
	}
	if(!utStrIsSpaces(caEdate)){
		if(utStrIsSpaces(caEtime)) {
			strcpy(caEtime,"23:59");
		}
		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		letime=utTimStrToLong("%Y/%m/%d %H:%M",caEtemptime);
	}	


	memset(caCond,0,sizeof(caCond));
 		

	 if(strlen(caUserid)!=0)
		sprintf(caCond+strlen(caCond)," and b.userid=%s ",caUserid);
	
		
		if(strlen(caUsername)!=0){
		
		  	sprintf(caCond+strlen(caCond)," and (a.username like '%c%s%c' or a.dispname like '%c%s%c') ",'%',caUsername,'%','%',caUsername,'%');
		 
		}
		if(strlen(caIdfyno)!=0){
							
				sprintf(caCond+strlen(caCond)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
			
		}
		if(strlen(caSdate)!=0)
		sprintf(caCond+strlen(caCond)," and b.addtime>=%lu ",lstime);
		
		if(strlen(caEdate)!=0)
		sprintf(caCond+strlen(caCond)," and b.addtime<=%lu ",letime);
		

		
		if(strlen(caSip)!=0){
			if(atol(caIp_match)==0)
		     sprintf(caCond+strlen(caCond)," and inet_ntoa(b.sip)='%s' ",caSip);
		   else
		     sprintf(caCond+strlen(caCond)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caSip,'%');
		}
	  if(strlen(caEmail)>0){
	  	sprintf(caCond+strlen(caCond)," and b.email like '%c%s%c' ",'%',caEmail,'%');
	  }
		
		if(strlen(caName)>0){
			sprintf(caCond+strlen(caCond)," and b.username like '%c%s%c' ",'%',caName,'%');
		}
		
		if(strlen(caUrl)!=0){
    sprintf(caCond+strlen(caCond)," and b.url like '%c%s%c' ",'%',caUrl,'%');
    }
		
		if(strlen(caGroupid)>0){
			if(strcmp(caGroupid,"0")==0)
			 sprintf(caCond+strlen(caCond)," and a.groupid=%s ",caGroupid);
			else
			 sprintf(caCond+strlen(caCond)," and (a.groupid=%s or a.groupid in (select groupid from ncgroup where pid=%s))",caGroupid,caGroupid);
		}
		strcat(caCond," order by b.addtime desc ");
		    

    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;

 

    if(!utStrIsSpaces(caExport)){
    lRowNum = 999999;
    }
    else
    lRowNum=32;
    lStartRec = (lCurPg - 1) * lRowNum;    


  if(strstr(caCond,"a.")){
  	sprintf(caTemp,"select count(*) from nchttpaccreg b,ncuser a  where b.userid=a.userid ");
  }
  else
    sprintf(caTemp,"select count(*) from nchttpaccreg b  where 1=1");
    
 
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
    
  
printf("caTemp=%s\n",caTemp);
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);

    if(iReturn != 0) {
    	
		   utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
       return 0;
    }  
    if(strstr(caCond,"a.")){ 
    sprintf(caTemp,"select b.userid,b.username,inet_ntoa(b.sip),b.url,b.addtime,b.passwd,b.email from nchttpaccreg b,ncuser a where b.userid=a.userid  ");
  }
  else{
  	    sprintf(caTemp,"select b.userid,b.username,inet_ntoa(b.sip),b.url,b.addtime,b.passwd,b.email from nchttpaccreg b where 1=1  ");
  }
    if(strlen(caCond)!=0)
    strcat(caTemp,caCond);
    
    sprintf(caTemp+strlen(caTemp)," limit %lu,%lu ",lStartRec,lRowNum);
    
printf("caTemp=%s\n",caTemp);  
    psCur = pasDbOpenSql(caTemp,0);

    if(psCur == NULL) {
    
      utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
        return 0;
    }   

    if(!utStrIsSpaces(caExport)){
 
    	FILE *fp;
    	char caFilename[32];
    	char caFile[128];
    	char caPath[128];
        sprintf(caFilename,"ncswebaccountlog%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
    	fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
           
             utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
            return 0;
        }
        
        fprintf(fp,"网站会员注册日志\n");
        ncUtlPrintCsv(fp,7,UT_TYPE_STRING,"网址帐号",
                           UT_TYPE_STRING,ncLang("1403使用时间"),
                           UT_TYPE_STRING,ncLang("1405上网IP"),
                           UT_TYPE_STRING,"网址",
                           UT_TYPE_STRING,"部门",
                           UT_TYPE_STRING,ncLang("0728用户名"),
                           UT_TYPE_STRING,"邮件地址");
                          

       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caName,0,sizeof(caName));
       memset(caUrl,0,sizeof(caUrl));
       memset(caPassword,0,sizeof(caPassword));
       memset(caEmail,0,sizeof(caEmail));
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,31,caName,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,127,caUrl,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,31,caPassword,
   	             UT_TYPE_STRING,63,caEmail);
  	                              
        while((iReturn == 0)||(iReturn==1405)) { 
        	
        
        psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
        	strcpy(caDisp2,psUser->dispname);
        	   if(psUser->psGroup)
		           strcpy(caGroupname,psUser->psGroup->groupname);
		         else{
		         	strcpy(caGroupname,"");
		         	
		        }
        }
        else{
        	strcpy(caDisp2,"");
        	strcpy(caGroupname,"");
        }

        
        strcpy(caStime,utTimFormat("%Y/%m/%d %H:%M",lStime));
              
                     ncUtlPrintCsv(fp,7,UT_TYPE_STRING,caName,
                           UT_TYPE_STRING,caStime,
                           UT_TYPE_STRING,caSip,
                           UT_TYPE_STRING,caUrl,
                           UT_TYPE_STRING,caGroupname,
                           UT_TYPE_STRING,caDisp2,
                           UT_TYPE_STRING,caEmail); 
              
              
       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caName,0,sizeof(caName));
       memset(caUrl,0,sizeof(caUrl));
       memset(caPassword,0,sizeof(caPassword));
          
       memset(caEmail,0,sizeof(caEmail));
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,31,caName,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,127,caUrl,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,31,caPassword,
   	             UT_TYPE_STRING,63,caEmail);
                    

        }
        
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
    
        return 0;
       
    }
         
    psDbHead = utPltInitDb();
    
  
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caUsername,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),"未知"));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
   
    utPltPutVar(psDbHead,"title",caTitlename);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"username",caUsername);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"stime",caStime);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"mac",caMac);
  
    utPltPutVar(psDbHead,"idfytype",caIdfytype);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"username_match",caUsername_match);

    utPltPutVar(psDbHead,"url",caUrl);
    utPltPutVar(psDbHead,"name",caName);
    utPltPutVar(psDbHead,"email",caEmail);
       
       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caName,0,sizeof(caName));
       memset(caUrl,0,sizeof(caUrl));
       memset(caPassword,0,sizeof(caPassword));
       memset(caEmail,0,sizeof(caEmail));
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,31,caName,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,127,caUrl,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,31,caPassword,
   	             UT_TYPE_STRING,63,caEmail);
   	
   	iNum = 0;
   	
   	while(iReturn == 0||iReturn==1405 && iNum < lRowNum) {

   	     psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser) {
        	strcpy(caDisp2,psUser->dispname);
        	 if(psUser->psGroup)
	           strcpy(caGroupname,psUser->psGroup->groupname);
	         else{
	         	strcpy(caGroupname,"");
	        }
        	
        	
        }
        else{
        	strcpy(caDisp2,"");
        	strcpy(caGroupname,"");
        }


iNum++;
        if(lStime>0)
        utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%Y/%m/%d %H:%M",lStime));
      
	      utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
	      utPltPutLoopVar(psDbHead,"name",iNum,caName);
	      if(strlen(caPassword)>0){
	      	utPltPutLoopVar(psDbHead,"password",iNum,"******");
	      }
	      utPltPutLoopVar(psDbHead,"groupname",iNum,caGroupname); 
       

        utPltPutLoopVar(psDbHead,"dispname",iNum,caDisp2);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid);
        utPltPutLoopVar(psDbHead,"email",iNum,caEmail);
       utPltSetCvtHtml(1); 
        utPltPutLoopVar(psDbHead,"url",iNum,caUrl);
       
       
       utPltSetCvtHtml(0);      
      
       lUserid=0;
       lStime=0;     
       memset(caSip,0,sizeof(caSip));
       memset(caName,0,sizeof(caName));
       memset(caUrl,0,sizeof(caUrl));
       memset(caPassword,0,sizeof(caPassword));
       memset(caEmail,0,sizeof(caEmail));
        
   	iReturn = pasDbFetchInto(psCur,
   	             UT_TYPE_LONG,4,&lUserid,
   	             UT_TYPE_STRING,31,caName,
   	             UT_TYPE_STRING,15,caSip,
   	             UT_TYPE_STRING,127,caUrl,
   	             UT_TYPE_LONG,4,&lStime,
   	             UT_TYPE_STRING,31,caPassword,
   	             UT_TYPE_STRING,63,caEmail);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum+1 ;
    lTotRec = lCount;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
  utPltSetCvtHtml(0);
   
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);

   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_websitreglist.htm");
    
    return 0;
}

char **ncUtlGetTable2(char *sdate, char *edate, char *prefix, unsigned long *lStartTime, unsigned long *lEndTime, int *lCount)
{
   	char stime[15] = "";
   	char etime[15] = "";
   	
   	snprintf(stime, sizeof(stime), "%c%c%c%c%c%c%c%c000000", sdate[0], sdate[1], sdate[2], sdate[3], sdate[5], sdate[6], sdate[8], sdate[9]);
   	snprintf(etime, sizeof(stime), "%c%c%c%c%c%c%c%c235959", edate[0], edate[1], edate[2], edate[3], edate[5], edate[6], edate[8], edate[9]);
   	*lStartTime = utTimStrToLong("%Y%m%d%H%M%S", stime);
   	
   	unsigned long lTime = utTimStrToLong("%Y%m%d%H%M%S", etime);
   	*lEndTime = lTime;
   	
   	unsigned long lStartTime1 = *lStartTime;
   	
   	int i = 0;
   	
   	char minYear[5] = "";
   	char maxYear[5] = "";
   	char minMon[3] = "";
   	char maxMon[3] = "";
   	
   	strcpy(minYear, utTimFormat("%Y", lStartTime1));
   	strcpy(maxYear, utTimFormat("%Y", lTime));
   	
   	strcpy(minMon, utTimFormat("%m", lStartTime1));
   	strcpy(maxMon, utTimFormat("%m", lTime));
   	
   	int iMin = atoi(minYear) * 12 + atoi(minMon);
   	int iMax = atoi(maxYear) * 12 + atoi(maxMon);
   	int strLen = strlen(prefix) + 7;
   	
   	*lCount = (iMax - iMin) + 1;
   	
   	 int a;
   	 int b;
   	 char **tmp = (char **)malloc(sizeof(char *)*((iMax - iMin) + 1));
   	 if(tmp != NULL)
   	 {
   	 	int j = 0;
   	 	for(i = iMin; i <= iMax; i++)
   	 	{
   	 		tmp[j] = (char *)malloc(strLen);
   	 		if(tmp[j] != NULL)
   	 		{
   	 			a = i/12;
   	 			b = i%12;
   	 			if(b == 0)
   	 			{
   	 				a--;
   	 				b = 12;
   	 			}
   	 			snprintf(tmp[j], strLen, "%s%04d%02d", prefix, a, b);
   	 		}
   	 		j++;
   	 	}	
   	 }
   	 return tmp;
}


int ncWebHttp_Ib(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

  char caType[32];
  char caExport[32];
  FILE *fp;
	char caFilename[32];
	char caFile[128];
	char caPath[128];
	char instr_t[8192] = "";	
	
	int iTime_flag = 0;
	int days = 0;   
	int iret = 0;
	int _break = 0;
	int i = 0;
	int lCount = 0;
	int num = 0;
	int iNum = 0;
	
	unsigned long did = 0;
	unsigned long iStart = 0;
	unsigned long iLimit = 0;
	unsigned long lCount1 = 0;
	unsigned long lStartTime = 0;
   	unsigned long lTime = time(0);
   	unsigned long lTotRec = 0;
	
   	char sdate[11] = "";
   	char edate[11] = "";
   	char time_flag[2] = "";
   	char start[17] = "";
   	char limit[17] = "";
   	char sort[17] = "";
   	char dir[9] = "";
   	char _host[65] = "";
   	char sql[4096] = "";
   	char _uri[129] = "";
   	char _sip[16] = "";
   	char _dip[16] = "";
   	char _idno[19] = "";
   	char _title[129] = "";
   	char _dispname[33] = "";
   	char corpname[129] = "";
   	char _groupid[33] = "";
   	char _did[33] = "";
   	char _uid[17] = "";
   	char instr[8192] = "";	
   	char sql2[256] = "";
   	char frmfile[256] = "";
   	char _keyword[33] = "";
   	char *tmpgbk = NULL;
	char **ptmp = NULL;	
	long iReturn;
	char caMac[20],caDispname[32],caGroupid[16],caUserid[16],caSip[20];
	char caHost[128],caSdate[20],caEdate[20],caOrder[16],caOrderby[16];
	char caStime0[16],caEtime[16],byt_in[12],caByt[16],caDip[20],caIdfyno[20],caUri[128];
	char caUrlid[16],caUrlid2[16],caWebtitle[128],caIp[16],caStemptime[20],caEtemptime[20];
	long lstime,letime;
	char caBef[16];
	char caTitlename[36];
	long lUserid,lByt,lCurPg,lTotPg;
	char caCurpg[16];
	char caGroupname[36],caDipPort[24],caDate[20],caTemp[4096],caUrl0[256],caTitle[256];
	char caName[32];
	ncUserCont *psUser;
	struct sttable
	{
		char tname[32];
		unsigned long lCount;
	};
	struct sttable tableinfo[120];//10年？
   	
   pasDbCursor *psCur = NULL;

   
   	utPltDbHead *psDbHead = utPltInitDb();
   	

	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,22,
					        "mac",        UT_TYPE_STRING, 18, caMac,
	                "dispname",   UT_TYPE_STRING, 31, caDispname,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "userid",     UT_TYPE_STRING,11,caUserid,
	                "sip",        UT_TYPE_STRING, 31, caSip,
	                "host",       UT_TYPE_STRING, 63, caHost,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "orderby",    UT_TYPE_STRING, 2, caOrderby,
	                "stime",	  UT_TYPE_STRING, 5,caStime0,
	                "etime",      UT_TYPE_STRING, 5,caEtime,
	                "byt_in",     UT_TYPE_STRING, 1,byt_in,
	                "byt",        UT_TYPE_STRING, 9,caByt,
	                "dip",        UT_TYPE_STRING,15,caDip,
	                "idfyno",     UT_TYPE_STRING,18,caIdfyno,
	                 "uri",       UT_TYPE_STRING,100,caUri, 
   	               "urlid",UT_TYPE_STRING,10,caUrlid,
   	               "urlid2",UT_TYPE_STRING,10,caUrlid2,
   	               "webtitle",UT_TYPE_STRING,63,caWebtitle,
   	               "export",     UT_TYPE_STRING,10,caExport,
   	               "CurPg",   UT_TYPE_STRING,10,caCurpg);
   lCurPg=atol(caCurpg);               
   	               
  lByt=atol(caByt);
   utStrDelSpaces(caIp);           
   utStrDelSpaces(caDip);  
   utStrDelSpaces(caSip);
   utStrDelSpaces(caUri);   
   utStrDelSpaces(caHost);   
   utStrDelSpaces(caMac);           
   utStrDelSpaces(caDispname);
   
   
  

if((strlen(caUrlid)==0)&&(strlen(caUrlid2)>0)) strcpy(caUrlid,caUrlid2);
 if(atol(caUserid)>0)
    strcpy(caDispname,ncUtlBufGetUsername(psShmHead,atol(caUserid))); 
    
  if(strlen(caSdate)==0){
     		strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
    		strcpy(caStime0,"00:00");
    		sprintf(caStemptime,"%s %s",caSdate,caStime0);
		    lstime=utTimStrToLong("%Y-%m-%d %H:%M",caStemptime);
		  }
	  if(strlen(caEdate)==0){
    		strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
    		strcpy(caEtime,"23:59");
    		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		    letime=utTimStrToLong("%Y-%m-%d %H:%M",caEtemptime);
    	}
    	
	if(strlen(caSdate) > 0 && strlen(caEdate) > 0)
	{
		ptmp = ncUtlGetTable2(caSdate, caEdate, "nchttplog_if_", &lStartTime, &lTime, &lCount);
	}

    utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"host",caHost);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"bef",caBef);
 
    utPltPutVar(psDbHead,"stime",caStime0);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"byt_in",byt_in);
    utPltPutVar(psDbHead,"byt",caByt);
    utPltPutVar(psDbHead,"dip",caDip);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"uri",caUri);
    utPltPutVar(psDbHead,"urlid",caUrlid);
    utPltPutVar(psDbHead,"webtitle",caWebtitle);
    utPltPutVar(psDbHead,"osort",caOrder);
     if(atol(caUserid)>0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caDispname,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		      strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
    
    utPltPutVar(psDbHead,"title_name",caTitlename);


	char _sdate[11] = "";
	char _edate[11] = "";
	strcpy(_sdate, utTimFormat("%Y/%m/%d", lStartTime));
	strcpy(_edate, utTimFormat("%Y/%m/%d", lTime));
	for(i = 0; i < lCount; i++)
	{
	
			lCount1 = 0;
			memset(sql, 0, sizeof(sql));
			snprintf(sql, sizeof(sql), "select count(*) from %s where stime >= %lu and stime <= %lu ",  ptmp[i], lStartTime, lTime);
			if(!utStrIsSpaces(caUserid) && lUserid > 0){
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and uid=%s ",caUserid);
			}
			 if(!utStrIsSpaces(caGroupid)) {
        	 if(strcmp(caGroupid,"0")==0){
        	 	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and gid=%s ",caGroupid);        	    
        	  }
        	 else{
        	 	 snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (gid=%s or gid in (select groupid from ncgroup where pid=%s)) ",caGroupid,caGroupid);

              }
       }
			if(!utStrIsSpaces(caDispname)){
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (username like '%c%s%c' or dispname like '%c%s%c') ",'%',caDispname,'%','%',caDispname,'%');
				
			}
			if(!utStrIsSpaces(caMac)) {
					snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (mac like '%c%s%c') ",'%',caMac,'%');
			
			}
			if(!utStrIsSpaces(caSip)) {   	
        snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and sip like '%c%s%c' ",'%',caSip,'%');     
      }
			if(!utStrIsSpaces(caDip)) {   	
        snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dip like '%c%s%c' ",'%',caDip,'%');     
      }
      if(strlen(caIdfyno)!=0){
      	 snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and idno like '%c%s%c' ",'%',caIdfyno,'%');   
      }
      if(!utStrIsSpaces(caHost)) {
      	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and host like '%c%s%c' ",'%',caHost,'%');
      }
      if(strlen(caUrlid)!=0){
      	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and urlid=%s ",caUrlid);
      }
			if(strlen(caUri)!=0){
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and uri like '%c%s%c' ",'%',caUri,'%');
			}
			if(lByt>0){
	    	if(atoi(byt_in)==1){
	    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and bytes<=%lu ",lByt);	    		
	    	}
	    	else{
	    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and bytes>=%lu ",lByt);	    		
	        }
	       
	    }
						
			
			printf("__line = %d, sql = %s\n", __LINE__, sql);
			pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &lCount1);
			if(lCount1 > 0)
			{
				strcpy(tableinfo[num].tname, ptmp[i]);
				tableinfo[num].lCount = lCount1;
				num++;
			}
//		}
		free(ptmp[i]);
	}
	free(ptmp);	
	for(i = 0; i < num; i++)
	{
		lTotRec += tableinfo[i].lCount;
	}
	utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);

	
	
	//
	for(i = num-1; i >= 0; i--)
	{
		printf("table[%d] = %s, _lCount1[%d] = %lu\n", i, tableinfo[i].tname, i, tableinfo[i].lCount);
	}
	//
	
	iNum = 0;
    iLimit = 32;
    if(lCurPg==0) lCurPg=1;
    iStart = (lCurPg - 1) * iLimit;  
	lTotPg = (lTotRec ) / iLimit + 1;
	
	utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
  utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
	
	if(strlen(caExport)>0){
		
		iStart=0;
		iLimit=99999;
				

		        sprintf(caFilename,"nchttplog%d.csv",utStrGetId());
		        sprintf(caPath,"%s/html","/home/ncmysql/nc");
		        sprintf(caFile,"%s/%s",caPath,caFilename);
		    	fp = fopen(caFile,"w");
		        if(fp == NULL) {
		            pasDbCloseCursor(psCur);
		         
		            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
		            return 0;
		        }
		        
		        fprintf(fp,"网站访问日志\n");
		        ncUtlPrintCsv(fp,9,UT_TYPE_STRING,"部门",
		                           UT_TYPE_STRING,"用户名",
		                           UT_TYPE_STRING,"网址类别",
		                           UT_TYPE_STRING,"上网时间",
		                           UT_TYPE_STRING,"源IP",
		                           UT_TYPE_STRING,"目标IP",
		                           UT_TYPE_STRING,"访问网址",
		                           UT_TYPE_STRING,"标题",
		                           UT_TYPE_STRING,"流量");
		                         
				
			  
		
		
	}
	
	for(i = num-1; i >= 0; i--)
	{
		memset(sql, 0, sizeof(sql));
		if(tableinfo[i].lCount <= iStart)
		{	
			printf("iStart=%d,lCount=%d\n",iStart,tableinfo[i].lCount);
			iStart-=tableinfo[i].lCount;
			continue;
		}	
		snprintf(sql, sizeof(sql), "select uid, gid, dispname, stime, sip, host, dport, uri, dip, title, bytes, urlid from %s where stime >= %lu and stime <= %lu ", tableinfo[i].tname, lStartTime, lTime);
		
			if(!utStrIsSpaces(caUserid) && lUserid > 0){
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and uid=%s ",caUserid);
			}
			 if(!utStrIsSpaces(caGroupid)) {
        	 if(strcmp(caGroupid,"0")==0){
        	 	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and gid=%s ",caGroupid);        	    
        	  }
        	 else{
        	 	 snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (gid=%s or gid in (select groupid from ncgroup where pid=%s)) ",caGroupid,caGroupid);

              }
       }
			if(!utStrIsSpaces(caDispname)){
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (username like '%c%s%c' or dispname like '%c%s%c') ",'%',caDispname,'%','%',caDispname,'%');
				
			}
			if(!utStrIsSpaces(caMac)) {
					snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (mac like '%c%s%c') ",'%',caMac,'%');
			
			}
			if(!utStrIsSpaces(caSip)) {   	
        snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and sip like '%c%s%c' ",'%',caSip,'%');     
      }
			if(!utStrIsSpaces(caDip)) {   	
        snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dip like '%c%s%c' ",'%',caDip,'%');     
      }
      if(strlen(caIdfyno)!=0){
      	 snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and idno like '%c%s%c' ",'%',caIdfyno,'%');   
      }
      if(!utStrIsSpaces(caHost)) {
      	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and host like '%c%s%c' ",'%',caHost,'%');
      }
      if(strlen(caUrlid)!=0){
      	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and urlid=%s ",caUrlid);
      }
			if(strlen(caUri)!=0){
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and uri like '%c%s%c' ",'%',caUri,'%');
			}
			if(lByt>0){
	    	if(atoi(byt_in)==1){
	    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and bytes<=%lu ",lByt);	    		
	    	}
	    	else{
	    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and bytes>=%lu ",lByt);	    		
	        }
	       
	    }
		snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " order by stime desc limit %lu, %lu", iStart, iLimit < tableinfo[i].lCount ? iLimit : tableinfo[i].lCount);
		printf("__line = %d, sql = %s\n", __LINE__, sql);
		
		if( tableinfo[i].lCount > iStart && tableinfo[i].lCount < iStart+iLimit )
		{
			iLimit = iLimit -  (tableinfo[i].lCount-iStart);
			iStart = 0;
		}
		else 
		{
			_break = 1;
		}
		psCur = pasDbOpenSql(sql, 0);
		if(psCur != NULL)
		{
			unsigned long uid = 0;
			unsigned long did = 0;
			char dispname[33] ="";
			unsigned long stime = 0;
			char sip[16] = "";
			char host[65] = "";
			unsigned long dport = 0;
			char uri[129] = "";
			char dip[16];
			char title[129] = "";
			unsigned long bytes = 0;
			unsigned long urlid = 0;
			char corpname[256] = "";
			char s_time[21] = "";
			char classname[33] = "";
			char caUrl[256];
			memset(title,0,sizeof(title));
			iret = pasDbFetchInto(psCur,
					 UT_TYPE_ULONG,  4,              &uid,
					 UT_TYPE_ULONG,  4,              &did,
					 UT_TYPE_STRING, sizeof(dispname)-1, dispname,
					 UT_TYPE_ULONG,  4,              &stime,
					 UT_TYPE_STRING, sizeof(sip)-1,  sip,
					 UT_TYPE_STRING, sizeof(host)-1, host,
					 UT_TYPE_ULONG,  4,              &dport,
					 UT_TYPE_STRING, sizeof(uri)-1,  uri,
					 UT_TYPE_STRING, sizeof(dip)-1,  dip,
					 UT_TYPE_STRING,sizeof(title)-1,title,
					 UT_TYPE_ULONG,  4,              &bytes,
					 UT_TYPE_ULONG,  4,              &urlid);
			iNum=0;
			while((iret == 0) || (iret == 1405))
			{
	    	
        iNum++;
        memset(caGroupname,0,sizeof(caGroupname));
        memset(caDispname,0,sizeof(caDispname));
        psUser = ncUtlGetUserContById(psShmHead,uid);
        utPltPutLoopVar(psDbHead,"dispname", iNum,dispname);
        if(psUser) {
        	strcpy(caDispname,psUser->dispname);
            
            utPltPutLoopVar(psDbHead,"username", iNum,psUser->username);
            if(psUser->psGroup) {
            	  strcpy(caGroupname,psUser->psGroup->groupname);
                utPltPutLoopVar(psDbHead,"groupname", iNum,psUser->psGroup->groupname);
            }
        }
        memset(caDipPort,0,sizeof(caDipPort));
    
        sprintf(caDipPort,"%s:%lu",dip,dport);
        
  //      printf("caDipPort=%s\n",caDipPort);
        utPltPutLoopVar(psDbHead,"dip",iNum,caDipPort);
        utPltPutLoopVar(psDbHead,"sip",      iNum,sip);
        strcpy(caDate,utTimFormat("%Y/%m/%d %H:%M:%S", stime));
       
        utPltPutLoopVar(psDbHead,"sdate",    iNum,caDate);
        utPltPutLoopVar(psDbHead,"bytes",    iNum,utStrLtoF(bytes,caTemp));
        sprintf(caUrl0,"http://%s%s",host,uri);
        strcpy(caUrl,caUrl0);
        caUrl[78]=0;
        utPltSetCvtHtml(1);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",uid); 
        utPltPutLoopVar(psDbHead,"url",   iNum,caUrl);
        utPltPutLoopVar(psDbHead,"urllnk",   iNum,caUrl0);
        if(strlen(title)==0||strstr(title,"404 Not Found")){
        	  long cid=0;
            ncUrlGetUrlInfo(host,title,&cid);
        }
        utPltPutLoopVar(psDbHead,"title",   iNum,title);
        ncLimGetWebNameByUrl(host,caType,caName);
        utPltPutLoopVar(psDbHead,"urltype", iNum,caType);
        utPltPutLoopVar(psDbHead,"urlname", iNum,caName);
        utPltSetCvtHtml(0);
				
				if(strlen(caExport)>0){
					 ncUtlPrintCsv(fp,9,UT_TYPE_STRING,caGroupname,
                          UT_TYPE_STRING,dispname,
                          UT_TYPE_STRING,caType,
                          UT_TYPE_STRING,caDate,
                          UT_TYPE_STRING,sip,
                          UT_TYPE_STRING,dip,
                          UT_TYPE_STRING,caUrl,
                          UT_TYPE_STRING,title,
                          UT_TYPE_LONG,bytes);
				 }
				
				
				urlid = did = stime = dport = bytes = 0;
				memset(dispname, 0, sizeof(dispname));
				memset(sip, 0, sizeof(sip));
				memset(host, 0, sizeof(host));
				memset(uri, 0, sizeof(uri));
				memset(dip, 0, sizeof(dip));
			memset(title,0,sizeof(title));
			iret = pasDbFetchInto(psCur,
					 UT_TYPE_ULONG,  4,              &uid,
					 UT_TYPE_ULONG,  4,              &did,
					 UT_TYPE_STRING, sizeof(dispname)-1, dispname,
					 UT_TYPE_ULONG,  4,              &stime,
					 UT_TYPE_STRING, sizeof(sip)-1,  sip,
					 UT_TYPE_STRING, sizeof(host)-1, host,
					 UT_TYPE_ULONG,  4,              &dport,
					 UT_TYPE_STRING, sizeof(uri)-1,  uri,
					 UT_TYPE_STRING, sizeof(dip)-1,  dip,
					 UT_TYPE_STRING,sizeof(title)-1,title,
					 UT_TYPE_ULONG,  4,              &bytes,
					 UT_TYPE_ULONG,  4,              &urlid);
			}			 
			pasDbCloseCursor(psCur);
		} 
		if(_break == 1)
			break; 
	}

	
	if(strlen(caExport)>0){
	 	    fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        utPltFreeDb(psDbHead);
        return 0;
	 	
	 	
	 }
	
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_httplog_hsear.htm");
	return iret;
}



int ncWebIp_Ib(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

  char caType[32];
  char caExport[32];
  FILE *fp;
	char caFilename[32];
	char caFile[128];
	char caPath[128];
	char instr_t[8192] = "";	
	
	int iTime_flag = 0;
	int days = 0;   
	int iret = 0;
	int _break = 0;
	int i = 0;
	int lCount = 0;
	int num = 0;
	int iNum = 0;
	
	unsigned long did = 0;
	unsigned long iStart = 0;
	unsigned long iLimit = 0;
	unsigned long lCount1 = 0;
	unsigned long lStartTime = 0;
   	unsigned long lTime = time(0);
   	unsigned long lTotRec = 0;
	
   	char sdate[11] = "";
   	char edate[11] = "";
   	char time_flag[2] = "";
   	char start[17] = "";
   	char limit[17] = "";
   	char sort[17] = "";
   	char dir[9] = "";
   	char _host[65] = "";
   	char sql[4096] = "";
   	char _uri[129] = "";
   	char _sip[16] = "";
   	char _dip[16] = "";
   	char _idno[19] = "";
   	char _title[129] = "";
   	char _dispname[33] = "";
   	char corpname[129] = "";
   	char _groupid[33] = "";
   	char _did[33] = "";
   	char _uid[17] = "";
   	char instr[8192] = "";	
   	char sql2[256] = "";
   	char frmfile[256] = "";
   	char _keyword[33] = "";
   	char *tmpgbk = NULL;
	char **ptmp = NULL;	
	long iReturn;
	char caMac[20],caDispname[32],caGroupid[16],caUserid[16],caSip[20];
	char caHost[128],caSdate[20],caEdate[20],caOrder[16],caOrderby[16];
	char caStime0[16],caEtime[16],byt_in[12],caByt[16],caDip[20],caIdfyno[20],caUri[128];
	char caUrlid[16],caUrlid2[16],caWebtitle[128],caIp[16],caStemptime[20],caEtemptime[20];
	long lstime,letime;
	char caBef[16];
	char caTitlename[36];
	long lUserid,lByt,lCurPg,lTotPg;
	char caCurpg[16];
	char caGroupname[36],caDipPort[24],caDate[20],caTemp[4096],caUrl0[256],caTitle[256];
	char caDport[16],caProt[16],caService[16],byt_rflag[8];
	ncUserCont *psUser;
	struct sttable
	{
		char tname[32];
		unsigned long lCount;
	};
	struct sttable tableinfo[120];//10年？
   	
   pasDbCursor *psCur = NULL;

   
   	utPltDbHead *psDbHead = utPltInitDb();
   	

	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,23,
					        "mac",        UT_TYPE_STRING,18, caMac,
					        "userid",     UT_TYPE_STRING,14,caUserid,
	                "dispname",   UT_TYPE_STRING, 31, caDispname,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "sip",        UT_TYPE_STRING, 31, caSip,
	                "dip",        UT_TYPE_STRING, 31, caDip,
	                "dport",      UT_TYPE_STRING, 15, caDport,
	                "prot",       UT_TYPE_STRING, 11, caProt,
	                "service",       UT_TYPE_STRING, 11, caService,
	                "idfyno",UT_TYPE_STRING,18,caIdfyno,
	                "sdate",      UT_TYPE_STRING, 19, caSdate,
	                "edate",      UT_TYPE_STRING, 19, caEdate,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "orderby",    UT_TYPE_STRING, 2, caOrderby,
	                "bef",        UT_TYPE_STRING, 11, caBef,
	                "stime",	  UT_TYPE_STRING, 5,caStime0,
	                "etime",      UT_TYPE_STRING, 5,caEtime,
	                "byt_rflag",  UT_TYPE_STRING, 1,byt_rflag,
	                "byt_in",     UT_TYPE_STRING, 1,byt_in,
	                "byt",        UT_TYPE_STRING, 9,caByt,
	                "ip",         UT_TYPE_STRING,15,caIp,
   	               "export",     UT_TYPE_STRING,10,caExport,
   	               "CurPg",   UT_TYPE_STRING,10,caCurpg);
   lCurPg=atol(caCurpg);               
   	  lByt=atol(caByt);             
   utStrDelSpaces(caIp);           
   utStrDelSpaces(caDip);  
   utStrDelSpaces(caMac);           
   utStrDelSpaces(caDispname);
   
   
  
 if(atol(caUserid)>0)
    strcpy(caDispname,ncUtlBufGetUsername(psShmHead,atol(caUserid))); 
    
  if(strlen(caSdate)==0){
     		strcpy(caSdate,utTimFormat("%Y/%m/%d",time(0)));
    		strcpy(caStime0,"00:00");
    		sprintf(caStemptime,"%s %s",caSdate,caStime0);
		    lstime=utTimStrToLong("%Y-%m-%d %H:%M",caStemptime);
		  }
	  if(strlen(caEdate)==0){
    		strcpy(caEdate,utTimFormat("%Y/%m/%d",time(0)));
    		strcpy(caEtime,"23:59");
    		sprintf(caEtemptime,"%s %s",caEdate,caEtime);
		    letime=utTimStrToLong("%Y-%m-%d %H:%M",caEtemptime);
    	}
    	
	if(strlen(caSdate) > 0 && strlen(caEdate) > 0)
	{
		ptmp = ncUtlGetTable2(caSdate, caEdate, "nciplog_if_", &lStartTime, &lTime, &lCount);
	}

    utPltPutVar(psDbHead,"mac",caMac);
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"dispname",caDispname);
    utPltPutVar(psDbHead,"groupid",caGroupid);
    utPltPutVar(psDbHead,"sip",caSip);
    utPltPutVar(psDbHead,"dip",caDip);
    utPltPutVar(psDbHead,"dport",caDport);
    utPltPutVar(psDbHead,"prot",caProt);
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"idfyno",caIdfyno);
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    utPltPutVar(psDbHead,"bef",caBef);
 
    utPltPutVar(psDbHead,"stime",caStime0);
    utPltPutVar(psDbHead,"etime",caEtime);
    utPltPutVar(psDbHead,"byt_rflag",byt_rflag);
    utPltPutVar(psDbHead,"byt_in",byt_in);
    utPltPutVar(psDbHead,"byt",caByt);
    utPltPutVar(psDbHead,"osort",caOrder);
   
     if(atol(caUserid)>0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    	strcpy(caDispname,caTitlename);
    }
    else if(strlen(caGroupid)!=0){   	    	
		      strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	
    
    utPltPutVar(psDbHead,"title_name",caTitlename);


	char _sdate[11] = "";
	char _edate[11] = "";
	strcpy(_sdate, utTimFormat("%Y/%m/%d", lStartTime));
	strcpy(_edate, utTimFormat("%Y/%m/%d", lTime));
	for(i = 0; i < lCount; i++)
	{
	
			lCount1 = 0;
			memset(sql, 0, sizeof(sql));
			snprintf(sql, sizeof(sql), "select count(*) from %s where stime >= %lu and stime <= %lu ",  ptmp[i], lStartTime, lTime);
			if(!utStrIsSpaces(caUserid) && lUserid > 0){
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and uid=%s ",caUserid);
			}
			 if(!utStrIsSpaces(caGroupid)) {
        	 if(strcmp(caGroupid,"0")==0){
        	 	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and gid=%s ",caGroupid);        	    
        	  }
        	 else{
        	 	 snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (gid=%s or gid in (select groupid from ncgroup where pid=%s)) ",caGroupid,caGroupid);

              }
       }
			if(!utStrIsSpaces(caDispname)){
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (username like '%c%s%c' or dispname like '%c%s%c') ",'%',caDispname,'%','%',caDispname,'%');
				
			}
			if(!utStrIsSpaces(caMac)) {
					snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (mac like '%c%s%c') ",'%',caMac,'%');
			
			}
			if(!utStrIsSpaces(caSip)) {   	
        snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and sip like '%c%s%c' ",'%',caSip,'%');     
      }
			if(!utStrIsSpaces(caDip)) {   	
        snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dip like '%c%s%c' ",'%',caDip,'%');     
      }
      if(strlen(caIdfyno)!=0){
      	 snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and idno like '%c%s%c' ",'%',caIdfyno,'%');   
      }
      if(!utStrIsSpaces(caDport)) {
      	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dport=%s ",caDport);
      }
      if(!utStrIsSpaces(caService)) {
    	if(atol(caService)==2000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=2000 service<=2999");
    	}
    	else if(atol(caService)==1000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=1000 service<=1999");
    	}
    	else if(atol(caService)==3000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=3000 service<=3999");
    	}
    	else if(atol(caService)==4000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=4000 service<=4999");
    	}
    	else if(atol(caService)==7000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=7000 service<=7999");
    	}
    	else if(atol(caService)==8000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=8000 service<=8999");
    	}
    	else if(atol(caService)==9000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=9000 service<=9999");
    	}
    	else if(atol(caService)==10000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=10000 service<=10999");
    	}
    	else if(atol(caService)==11000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=11000 service<=11999");
    	}
    	else if(atol(caService)==12000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=12000 service<=12999");
    	}
    	else if(atol(caService)==13000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=13000 service<=13999");
    	}
    	else if(atol(caService)==14000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=14000 service<=14999");
    	}
    	else if(atol(caService)==15000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=15000 service<=15999");
    	}
    	else if(atol(caService)==17000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=17000 service<=17999");

    	}
    	else{
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service=%s ",caService);
      }
    }
     
         if(!utStrIsSpaces(caProt)) {
       snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and trantype=%s ",caProt);
    }
    if(lByt>0){
	    if(atoi(byt_rflag)==1){
	    	if(atoi(byt_in)==1){
	    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and ubytes<=%lu ",lByt);
	    		
	    	}
	    	else{
	    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and ubytes>=%lu ",lByt);
	        }
	    }
	    else if(atoi(byt_rflag)==2){
    	    if(atoi(byt_in)==1){
    	    	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dbytes<=%lu ",lByt);
	    		
	    	}
	    	else{
	    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dbytes>=%lu ",lByt);
	    		
	        }
	    }
       else {
           	if(atoi(byt_in)==1){
           		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dbytes+ubytes<=%lu ",lByt);

	    	}
	    	else{
             snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dbytes+ubytes>=%lu ",lByt);
	        }
	    }
	 }
						
			
			printf("__line = %d, sql = %s\n", __LINE__, sql);
			pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &lCount1);
			if(lCount1 > 0)
			{
				strcpy(tableinfo[num].tname, ptmp[i]);
				tableinfo[num].lCount = lCount1;
				num++;
			}
//		}
		free(ptmp[i]);
	}
	free(ptmp);	
	for(i = 0; i < num; i++)
	{
		lTotRec += tableinfo[i].lCount;
	}
	utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);

	
	
	//
	for(i = num-1; i >= 0; i--)
	{
		printf("table[%d] = %s, _lCount1[%d] = %lu\n", i, tableinfo[i].tname, i, tableinfo[i].lCount);
	}
	//
	
	iNum = 0;
    iLimit = 32;
    if(lCurPg==0) lCurPg=1;
    iStart = (lCurPg - 1) * iLimit;  
	lTotPg = (lTotRec  ) / iLimit + 1;
	
	utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
  utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
	
	if(strlen(caExport)>0){
		
		iStart=0;
		iLimit=99999;
				

		        sprintf(caFilename,"nciplog%d.csv",utStrGetId());
		        sprintf(caPath,"%s/html","/home/ncmysql/nc");
		        sprintf(caFile,"%s/%s",caPath,caFilename);
		    	fp = fopen(caFile,"w");
		        if(fp == NULL) {
		            pasDbCloseCursor(psCur);
		         
		            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0546数据导出出错"),ncLang("0546数据导出出错"));
		            return 0;
		        }
		        
		        fprintf(fp,"IP访问日志\n");
    ncUtlPrintCsv(fp,9,
                       UT_TYPE_STRING,ncLang("0693姓  名"),
                       UT_TYPE_STRING,ncLang("0821源地址"),
                       UT_TYPE_STRING,ncLang("0398目标地址"),
                       UT_TYPE_STRING,ncLang("0686协  议"),
                       UT_TYPE_STRING,ncLang("0237访问端口"),
                       UT_TYPE_STRING,ncLang("0253服务"),
                       UT_TYPE_STRING,ncLang("0241访问时间"),
                       UT_TYPE_STRING,ncLang("0505上行流量"),
                       UT_TYPE_STRING,ncLang("0678下行流量"));
		                         
				
			  
		
		
	}
	
	for(i = num-1; i >= 0; i--)
	{
		memset(sql, 0, sizeof(sql));
		if(tableinfo[i].lCount <= iStart)
		{	
			printf("iStart=%d,lCount=%d\n",iStart,tableinfo[i].lCount);
			iStart-=tableinfo[i].lCount;
			continue;
		}	
		snprintf(sql, sizeof(sql), "select uid, gid, dispname, stime,sip,dip,ubytes,dbytes,conntime,service,dport,trantype from %s where stime >= %lu and stime <= %lu ", tableinfo[i].tname, lStartTime, lTime);
		
			if(!utStrIsSpaces(caUserid) && lUserid > 0){
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and uid=%s ",caUserid);
			}
			 if(!utStrIsSpaces(caGroupid)) {
        	 if(strcmp(caGroupid,"0")==0){
        	 	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and gid=%s ",caGroupid);        	    
        	  }
        	 else{
        	 	 snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (gid=%s or gid in (select groupid from ncgroup where pid=%s)) ",caGroupid,caGroupid);

              }
       }
			if(!utStrIsSpaces(caDispname)){
				snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (username like '%c%s%c' or dispname like '%c%s%c') ",'%',caDispname,'%','%',caDispname,'%');
				
			}
			if(!utStrIsSpaces(caMac)) {
					snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and (mac like '%c%s%c') ",'%',caMac,'%');
			
			}
			if(!utStrIsSpaces(caSip)) {   	
        snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and sip like '%c%s%c' ",'%',caSip,'%');     
      }
			if(!utStrIsSpaces(caDip)) {   	
        snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dip like '%c%s%c' ",'%',caDip,'%');     
      }
      if(strlen(caIdfyno)!=0){
      	 snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and idno like '%c%s%c' ",'%',caIdfyno,'%');   
      }
      if(!utStrIsSpaces(caDport)) {
      	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dport=%s ",caDport);
      }
      if(!utStrIsSpaces(caService)) {
    	if(atol(caService)==2000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=2000 service<=2999");
    	}
    	else if(atol(caService)==1000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=1000 service<=1999");
    	}
    	else if(atol(caService)==3000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=3000 service<=3999");
    	}
    	else if(atol(caService)==4000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=4000 service<=4999");
    	}
    	else if(atol(caService)==7000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=7000 service<=7999");
    	}
    	else if(atol(caService)==8000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=8000 service<=8999");
    	}
    	else if(atol(caService)==9000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=9000 service<=9999");
    	}
    	else if(atol(caService)==10000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=10000 service<=10999");
    	}
    	else if(atol(caService)==11000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=11000 service<=11999");
    	}
    	else if(atol(caService)==12000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=12000 service<=12999");
    	}
    	else if(atol(caService)==13000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=13000 service<=13999");
    	}
    	else if(atol(caService)==14000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=14000 service<=14999");
    	}
    	else if(atol(caService)==15000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=15000 service<=15999");
    	}
    	else if(atol(caService)==17000){
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service>=17000 service<=17999");

    	}
    	else{
    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and service=%s ",caService);
      }
    }
     
         if(!utStrIsSpaces(caProt)) {
       snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and trantype=%s ",caProt);
    }
    if(lByt>0){
	    if(atoi(byt_rflag)==1){
	    	if(atoi(byt_in)==1){
	    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and ubytes<=%lu ",lByt);
	    		
	    	}
	    	else{
	    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and ubytes>=%lu ",lByt);
	        }
	    }
	    else if(atoi(byt_rflag)==2){
    	    if(atoi(byt_in)==1){
    	    	snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dbytes<=%lu ",lByt);
	    		
	    	}
	    	else{
	    		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dbytes>=%lu ",lByt);
	    		
	        }
	    }
       else {
           	if(atoi(byt_in)==1){
           		snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dbytes+ubytes<=%lu ",lByt);

	    	}
	    	else{
             snprintf(sql+strlen(sql),sizeof(sql)-strlen(sql)," and dbytes+ubytes>=%lu ",lByt);
	        }
	    }
	 }
	 
		snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " order by stime desc limit %lu, %lu", iStart, iLimit < tableinfo[i].lCount ? iLimit : tableinfo[i].lCount);
		printf("__line = %d, sql = %s\n", __LINE__, sql);
		
		if( tableinfo[i].lCount > iStart && tableinfo[i].lCount < iStart+iLimit )
		{
			iLimit = iLimit -  (tableinfo[i].lCount-iStart);
			iStart = 0;
		}
		else 
		{
			_break = 1;
		}
		psCur = pasDbOpenSql(sql, 0);
		if(psCur != NULL)
		{
			unsigned long uid = 0;
			unsigned long did = 0;
			char dispname[33] ="";
			unsigned long stime = 0;
			char sip[16] = "";
			unsigned long dport = 0;
			char dip[16]="";
			char s_time[21] = "";
      unsigned ubytes=0;
      unsigned dbytes=0;
      unsigned long conntime=0;
      unsigned long service=0;
      unsigned long trantype=0;
			iret = pasDbFetchInto(psCur,
					 UT_TYPE_ULONG,  4,              &uid,
					 UT_TYPE_ULONG,  4,              &did,
					 UT_TYPE_STRING, sizeof(dispname)-1, dispname,
					 UT_TYPE_ULONG,  4,              &stime,
					 UT_TYPE_STRING, sizeof(sip)-1,  sip,
					 UT_TYPE_STRING,sizeof(dip)-1,dip,
					 UT_TYPE_ULONG,4,&ubytes,
					 UT_TYPE_ULONG,4,&dbytes,
					 UT_TYPE_ULONG,4,&conntime,
					 UT_TYPE_ULONG,4,&service,
					 UT_TYPE_ULONG,  4,&dport,
					 UT_TYPE_ULONG,4,&trantype);
					
			iNum=0;
			while((iret == 0) || (iret == 1405))
			{
	    	
        iNum++;
        memset(caGroupname,0,sizeof(caGroupname));
        memset(caDispname,0,sizeof(caDispname));
        psUser = ncUtlGetUserContById(psShmHead,uid);
        utPltSetCvtHtml(1);
        utPltPutLoopVar(psDbHead,"dispname", iNum,dispname);
        if(psUser) {
        	strcpy(caDispname,psUser->dispname);
            
            utPltPutLoopVar(psDbHead,"username", iNum,psUser->username);
            if(psUser->psGroup) {
            	  strcpy(caGroupname,psUser->psGroup->groupname);
                utPltPutLoopVar(psDbHead,"groupname", iNum,psUser->psGroup->groupname);
            }
        }
        utPltSetCvtHtml(0);
          utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",uid); 
          utPltPutLoopVar(psDbHead,"sip",      iNum,sip);
	        utPltPutLoopVar(psDbHead,"dip",      iNum,dip);
	          strcpy(caDate,utTimFormat("%Y/%m/%d %H:%M:%S", stime));
	        utPltPutLoopVar(psDbHead,"sdate",    iNum,caDate);
	        utPltPutLoopVar(psDbHead,"conn",    iNum,utStrLtoF(conntime,caTemp));
	        utPltPutLoopVar(psDbHead,"ubytes",  iNum,utStrLtoF(ubytes,caTemp));
	        utPltPutLoopVar(psDbHead,"dbytes",   iNum,utStrLtoF(dbytes,caTemp));
        
        	if(service== 0) {
	            utPltPutLoopVar(psDbHead,"service",  iNum,"&nbsp;&nbsp;");
	        }
	        else {
	            utPltPutLoopVar(psDbHead,"service",  iNum,ncUtlGetServiceName(psShmHead,service,"\0"));
	        }
	        utPltPutLoopVarF(psDbHead,"dport",    iNum,"%lu",dport);
	        if(trantype == 6) {
	        	strcpy(caType,"TCP");
	            utPltPutLoopVar(psDbHead,"prot",   iNum,"TCP");
	        }
	        else {
	        	  strcpy(caType,"UDP");
	            utPltPutLoopVar(psDbHead,"prot",   iNum,"UDP");
	        } 
        
        
        
   
				
				if(strlen(caExport)>0){
					 ncUtlPrintCsv(fp,9,
                          UT_TYPE_STRING,dispname,
                          UT_TYPE_STRING,sip,
                          UT_TYPE_STRING,dip,
                          UT_TYPE_STRING,caType,
                          UT_TYPE_LONG,dport,
                          UT_TYPE_STRING,ncCvtSetService(psShmHead,service),
                          UT_TYPE_STRING,caDate,
                          UT_TYPE_ULONG,ubytes,
                          UT_TYPE_ULONG,dbytes);

				 }
				
				
        uid=0;
        did=0;
        memset(dispname,0,sizeof(dispname));
        stime=0;
        memset(sip,0,sizeof(sip));
        memset(dip,0,sizeof(dip));
        ubytes=0;
        dbytes=0;
        conntime=0;
        service=0;
        dport=0;
        trantype=0;
			iret = pasDbFetchInto(psCur,
					 UT_TYPE_ULONG,  4,              &uid,
					 UT_TYPE_ULONG,  4,              &did,
					 UT_TYPE_STRING, sizeof(dispname)-1, dispname,
					 UT_TYPE_ULONG,  4,              &stime,
					 UT_TYPE_STRING, sizeof(sip)-1,  sip,
					 UT_TYPE_STRING,sizeof(dip)-1,dip,
					 UT_TYPE_ULONG,4,&ubytes,
					 UT_TYPE_ULONG,4,&dbytes,
					 UT_TYPE_ULONG,4,&conntime,
					 UT_TYPE_ULONG,4,&service,
					 UT_TYPE_ULONG,  4,&dport,
					 UT_TYPE_ULONG,4,&trantype);
			}			 
			pasDbCloseCursor(psCur);
		} 
		if(_break == 1)
			break; 
	}

	
	if(strlen(caExport)>0){
	 	    fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        utPltFreeDb(psDbHead);
        return 0;
	 	
	 	
	 }
	    for(i=0;i<ncPatGetSumService();i++) {
           utPltPutLoopVarF(psDbHead,"id_s",i+1,"%lu",ncUtlGetServiceIdByNum(psShmHead,i));
           utPltPutLoopVar(psDbHead,"name_s",i+1,ncUtlGetServiceByNum(psShmHead,i));
    }
        
//输出网络服务大类
long lCgid;
char caName[32];
     	psCur = pasDbOpenSqlF("select groupid,name  from ncservicecgroup   order by groupid ");
                    
	    if(psCur == NULL) {
	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
	        return 0;
	    }
     
    lCgid=0;
    memset(caName,0,sizeof(caName));
    iNum=0;
	    iReturn = pasDbFetchInto(psCur,     	                           
	      	                           UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
	  	                             
   	
   	while(iReturn == 0 ||iReturn==1405) {
   		
    utPltPutLoopVarF(psDbHead,"cgid",iNum+1,"%d",lCgid);
    utPltPutLoopVar(psDbHead,"cgname",iNum+1,caName);
   	
   	iNum++;
   		
    lCgid=0;
    memset(caName,0,sizeof(caName));
	    iReturn = pasDbFetchInto(psCur,     	                           
	      	                           UT_TYPE_LONG,4,&lCgid, 	                           
	  	                               UT_TYPE_STRING,31,caName);
        }  
    pasDbCloseCursor(psCur);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_iplog_hsear.htm");
	return iret;
}