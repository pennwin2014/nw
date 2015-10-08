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
extern int iDebugLevel;
/* 显示软件进程表*/
int nwWebLimProcList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caWhere[255],caTemp[1024];
    char caCond[64],caName[64];
    char caTotPg[16],caCurPg[16],caTotRec[16];
    char caId[16],caModi[16],caDel[16],caSid[16],caAdd[16];
    char caApp[16];
    char caProcname[64],caDispname[64];
    char *p,*pBuf,*pSelect;
    int i,iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lStartRec,lSumUser,lCount,lCurPg,lTotRec,lRowNum,lSid1,lId1,lLport1,lUport1;
   	unsigned long lSid,lTotPg;
   	
 	pasDbCursor *psCur;
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"nwWebLimProcList");
 	}
	iReturn = utMsgGetSomeNVar(psMsgHead,10,
	                "app",        UT_TYPE_STRING,12,caApp,
	                "group",      UT_TYPE_STRING, 12, caId,
	                "add",        UT_TYPE_STRING, 12, caAdd,
	                "modi",       UT_TYPE_STRING, 12, caModi,
	                "del",        UT_TYPE_STRING, 12, caDel,
	                "sid",        UT_TYPE_STRING, 12, caSid,
                  "procname",   UT_TYPE_STRING, 63, caProcname,
                  "dispname",   UT_TYPE_STRING, 63, caDispname,
					"CurPg",      UT_TYPE_STRING, 12, caCurPg,
					"TotRec",     UT_TYPE_STRING, 12, caTotRec);
			
			
					
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
  
            lSid = pasGetSid("nwprocelist","sid");
            iReturn = pasDbExecSqlF("insert into nwprocelist(sid,id,procname,dispname) values (%lu,%s,'%s','%s')",
                                             lSid,caId,caProcname,caDispname);
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0311记录增加出错,%s"),pasDbErrorMsg(NULL));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                strcpy(caTotPg,"\0");
                pasDbCommit(NULL);
            }
        }    
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
   	    for(i=0;i<50;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
                iReturn = pasDbExecSqlF("delete from nwprocelist where sid = %s",p);
                if(iReturn == 0) {
                    strcpy(caTotPg,"\0");
                    pasDbCommit(NULL);
                }
   	        }
   	    }
    }
    else if(!utStrIsSpaces(caModi)) { /* 修改记录  */
        if(!utStrIsSpaces(caSid)) {
            
            iReturn = pasDbExecSqlF("update nwprocelist set id=%s,procname='%s',dispname='%s' where sid = %s",
                    caId,caProcname,caDispname,caSid);
            if(iReturn == 0) {
                pasDbCommit(NULL);
            }
            else {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0303记录修改出错"));
                pasDbRollback(NULL);
            }
        }
    }
    else if(!utStrIsSpaces(caApp)) { /* 应用  */
  //      ncWebLimApp(psShmHead,iFd,psMsgHead);
        return 0;
    }


 if(utStrIsSpaces(caId))
    sprintf(caTemp,"select count(*) from nwprocelist a  left join nwsoftclass b on a.id=b.id ");
    else
    sprintf(caTemp,"select count(*) from nwprocelist a left join nwsoftclass b on a.id=b.id where a.id=%s ",caId);
    
    
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
    if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0561数据统计出错"));
            return 0;
     }
     lTotRec = lCount;
    
    lRowNum = 30;
    lStartRec = (lCurPg - 1) * lRowNum;   
    if(utStrIsSpaces(caId))
        psCur = pasDbOpenSqlF("select a.sid,a.id,a.procname,a.dispname,b.name from nwprocelist a  left join nwsoftclass b on a.id=b.id order by a.id limit %d,%d",
                lStartRec,lRowNum);
    else{
    	  psCur = pasDbOpenSqlF("select a.sid,a.id,a.procname,a.dispname,b.name from nwprocelist a  left join nwsoftclass b on a.id=b.id where a.id=%s order by a.id limit %d,%d",
                caId,lStartRec,lRowNum);
    }  
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    psDbHead = utPltInitDb();
    lSid1=0;
    lId1=0;
    memset(caProcname,0,sizeof(caProcname));
    memset(caDispname,0,sizeof(caDispname));
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid1,
      	                           UT_TYPE_LONG,4,&lId1,
      	                           UT_TYPE_STRING,63,caProcname,
      	                           UT_TYPE_STRING,63,caDispname,
      	                           UT_TYPE_STRING,63,caName);
      	                           
  	                               
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%u",iNum);
        utPltPutLoopVarF(psDbHead,"lsid",   iNum,"%lu",lSid1);
        utPltPutLoopVarF(psDbHead,"lid",    iNum,"%lu", lId1);
        utPltPutLoopVar(psDbHead,"procname",iNum,caProcname);
        utPltPutLoopVar(psDbHead,"dispname",iNum,caDispname);
        utPltPutLoopVar(psDbHead,"name",iNum,caName);
       
    lSid1=0;
    lId1=0;
    memset(caProcname,0,sizeof(caProcname));
    memset(caDispname,0,sizeof(caDispname));
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid1,
      	                           UT_TYPE_LONG,4,&lId1,
      	                           UT_TYPE_STRING,63,caProcname,
      	                           UT_TYPE_STRING,63,caDispname,
      	                           UT_TYPE_STRING,63,caName);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lTotRec - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
   
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);

    pBuf = pasUtlCvtSqlSelectA("select name,id from nwsoftclass order by name",ncLang("0447全部,;"));
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("group",caId,pBuf);
        free(pBuf);
    }
    else {
        pSelect= pasUtlCvtInputSelect("group"," ",ncLang("0651无, ;"));
    }
    utPltPutVar(psDbHead,"group", pSelect);

    free(pSelect);
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nw_limproclist.htm");
    return 0;
}


/* 显示软件树  */
int ncSoftClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId;
   	char caName[64];
   	char caPlate[128];
 	pasDbCursor *psCur;
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"nwsoftclass");
 	}
	iReturn = utMsgGetSomeNVar(psMsgHead,1,
	                "plate",  UT_TYPE_STRING,127,caPlate);
 	
    psCur = pasDbOpenSqlF("select id,name from nwsoftclass");
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0087Web组信息出错"));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lId,
  	                               UT_TYPE_STRING,63,caName);
   	iNum = 0;
   	while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"id",   iNum,"%u",lId);
        utPltPutLoopVarF(psDbHead,"name", iNum,"%s",caName);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum);
        iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lId,
  	                               UT_TYPE_STRING,63,caName);
    }
    pasDbCloseCursor(psCur);
    utPltPutVar(psDbHead,"plate",caPlate);
    utPltPutVarF(psDbHead,"ipid","%lu",utStrGetId());
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}

int ncSoftClassEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId,lSid,i;
   	char caName[64];
   	char caPlate[128];
   	char caVar[64];

   	char caGid[16],caDid[16],caAdd[16],caDel[16];
	char caNname[32],caNid[16],caModi[16];
 	char *p;
 	pasDbCursor *psCur;
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncSoftClassEdit");
 	}
	iReturn = utMsgGetSomeNVar(psMsgHead,8,
	                "gname",  UT_TYPE_STRING,63,caName,
	                "sid",    UT_TYPE_STRING,12,caGid,
	                "del",    UT_TYPE_STRING,12,caDel,
	                "add",    UT_TYPE_STRING,12,caAdd,
	                "plate",  UT_TYPE_STRING,127,caPlate,
	                "nid",    UT_TYPE_STRING,12,caNid,
	                "modi",   UT_TYPE_STRING,12,caModi,
	                "nname",  UT_TYPE_STRING,31,caNname
	                );
    if(!utStrIsSpaces(caAdd)) { /* 增加一个组  */
        if(!utStrIsSpaces(caName) && utStrIsDigital(caGid)) {
            lSid = pasGetSid("nwsoftclass","id");
            iReturn = pasDbExecSqlF("insert into nwsoftclass(id,name,lasttime) values (%lu,'%s',%lu)",
                                             lSid,caName,time(0));
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
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0363类别名称为空或ID出错"));
            return 0;
       }            
            
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除一个组  */
    	
    	  for(i=0;i<999;i++) {
   	        sprintf(caVar,"ch%d",i+1);

   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
   	            lSid = atol(p);
	        iReturn = pasDbExecSqlF("delete from nwsoftclass where id= %lu",lSid);
		        if(iReturn != 0) {
	            pasDbRollback(NULL);
	        }                
	        else {
	            iReturn = pasDbExecSqlF("delete from nciplist where id= %lu",lSid);
	            pasDbCommit(NULL);
	        }            
   	        }
             	        
   	    }
    	

    }
    else if(!utStrIsSpaces(caModi) && !utStrIsSpaces(caName)) { /* 修改一个组  */
        iReturn = pasDbExecSqlF("update nwsoftclass set name='%s',lasttime=%lu where id = %s",
                         caName,time(0),caGid);
        pasDbCommit(NULL);
    }
    
    return ncSoftClass(psShmHead,iFd,psMsgHead);
    return 0;
}

static char *ncLimCvtTime2(long lStime)
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
int nwWebLim_Procy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caMessage[556],caMessage1[556];
    char caMod[16],caDel[16],caAdd[16],caApp[16],caCurPg[16];
    char caSum[16],caGtype[16],caFlags[16],caId[16],caRnum[16],caService[16];
    char caSid[16],caLimctl[16],caDateid[16],caEtime[16],caStime[16];
    char caName[64];
    char *pBuf,*pSelect,*p;
    unsigned long lLip;
    long  lStart,lStartRec,lCurPg,lTotPg;
    long  lRnum,lSum,lGtype,lFlags,lId,lDateid,lLimctl,lStime,lEtime,lSid,lService;
    long  lLport,lUport,lProt,lCount,lCount1,lRowNum;
    int   iReturn,i,lPid,iNum,lStarttime,lStoptime,lNum;
    char caTemp[1024];
    unsigned long lId1,lTotRec;
    char caName1[64];
    char caPid[32];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,15,
	                "sum",     UT_TYPE_STRING, 12, caSum,    /* 记录总数                     */
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All 4-Polcy */
	                "flags",   UT_TYPE_STRING, 12, caFlags,   /* 1--Internet 2--Services ..   */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
                   "rnum",    UT_TYPE_STRING, 12, caRnum,
                   "pid",     UT_TYPE_STRING,12,caPid,
	                "add",     UT_TYPE_STRING, 12, caAdd,
	                "mod",     UT_TYPE_STRING, 12, caMod,
	                "del",     UT_TYPE_STRING, 12, caDel,
	                "sid",     UT_TYPE_STRING, 12, caSid,
	                "limctl",  UT_TYPE_STRING, 12, caLimctl,
	                "etime",      UT_TYPE_STRING, 8,  caEtime,
	                "stime",      UT_TYPE_STRING, 8,  caStime,
	                "app",       UT_TYPE_STRING,12,caApp,
	                "CurPg",     UT_TYPE_STRING, 12, caCurPg);
         
    lGtype = atol(caGtype);
    lFlags = atol(caFlags);
    lId = atol(caId);
    lPid=atol(caPid);
    lRnum=atol(caRnum);
		if(strlen(caStime)==0){
			strcpy(caStime,"00:00:00");
		}
		if(strlen(caEtime)==0){
			strcpy(caEtime,"23:59:59");
		}
	
       lStime = ncUtlIsValidTime(caStime);
       lEtime = ncUtlIsValidTime(caEtime);
       if(lStime < 0 || lEtime < 0) {
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0435起始或终止时间不正确"));
          return 0;
       }
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */

    	sprintf(caTemp,"insert into nwlimproc(sid,gtype,id,pid,num,\
                     starttime,stoptime,\
                     flags) values \
                    (0,%d,%lu,%lu,%d,%lu,%lu,%d)",
                     lGtype,lId,lPid,lRnum,lStime,lEtime,lFlags);                 
    printf("caTemp=%s\n",caTemp);
                iReturn = pasDbExecSqlF("insert into nwlimproc(sid,gtype,id,pid,num,\
                     starttime,stoptime,\
                     flags,lasttime) values \
                    (0,%d,%lu,%lu,%d,%lu,%lu,%d,%lu)",
                     lGtype,lId,lPid,lRnum,lStime,lEtime,lFlags,time(0));
                if(iReturn != 0) {

                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0314记录增加出错, 可能为记录重复"));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                    pasDbCommit(NULL);
                   
                }
              
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
    	
        char caVar[16];
   	    for(i=0;i<24;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
   	           
   	  	  	            	
                    iReturn = pasDbExecSqlF("delete from nwlimproc where sid = %s",p);
                }
               
                if(iReturn == 0) {
                    pasDbCommit(NULL);
                }
   	        }
    }
    else if(!utStrIsSpaces(caMod)) { /* 修改记录  */
        lSid = atol(caSid);
        
        lStime = ncUtlIsValidTime(caStime);
        lEtime = ncUtlIsValidTime(caEtime);
        if(lStime < 0 || lEtime < 0) {
              utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0435起始或终止时间不正确"));
              return 0;
        }	                
        iReturn = pasDbExecSqlF("update nwlimproc set gtype=%lu,id=%lu,num=%lu,pid=%lu,starttime=%lu,stoptime=%lu,flags=%lu,lasttime=%lu where sid = %lu",
                           lGtype,lId,lRnum,lPid,lStime,lEtime,lFlags,time(0),lSid);
                if(iReturn != 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0308记录修改出错, 可能为记录重复 %s"),pasDbErrorMsg(NULL));
                    pasDbRollback(NULL);
                    return 0;
                }
                else {
                    pasDbCommit(NULL);
                  
                }

    }
    else if(!utStrIsSpaces(caApp)) { /* 应用  */
   //     ncWebLimApp(psShmHead,iFd,psMsgHead);
        return 0;    
    }
    
    psDbHead = utPltInitDb();
    
    lCount = 0;
   
    if(lGtype==3)
    	sprintf(caTemp,"select count(*) from nwlimproc where gtype=3 ");
    else 
      sprintf(caTemp,"select count(*) from nwlimproc where gtype=%d and id=%d ",lGtype,lId); 
    
    pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
    
    
	   lCurPg = atol(caCurPg);
		if(lCurPg == 0) lCurPg = 1;
		lRowNum = 24;
		lStart = (lCurPg - 1) * lRowNum; 
    
    lTotRec = lCount;
    
    if(lGtype==3){
    	sprintf(caTemp,"select a.sid,a.pid,a.num,a.starttime,a.stoptime,a.flags,b.name from nwlimproc a left join nwsoftclass b on a.pid=b.id where a.gtype=3 limit %d,%d",lStart,lRowNum);
    	psCur=pasDbOpenSqlF("select a.sid,a.pid,a.num,a.starttime,a.stoptime,a.flags,b.name from nwlimproc a left join nwsoftclass b on a.pid=b.id where a.gtype=3 limit %d,%d",lStart,lRowNum);
    }
    else{
      psCur=pasDbOpenSqlF("select a.sid,a.pid,a.num,a.starttime,a.stoptime,a.flags,b.name from nwlimproc a left join nwsoftclass b on a.pid=b.id where a.gtype=%d and a.id=%lu limit %d,%d",lGtype,lId,lStart,lRowNum);
      sprintf(caTemp,"select a.sid,a.pid,a.num,a.starttime,a.stoptime,a.flags,b.name from nwlimproc a left join nwsoftclass b on a.pid=b.id where a.gtype=%d and a.id=%lu limit %d,%d",lGtype,lId,lStart,lRowNum);
    }
    printf("caTemp=%s\n",caTemp);
  
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    lSid=0;
    lPid=0;
    lNum=0;
    lStarttime=0;
    lStoptime=0;
    lFlags=0;
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lPid,
      	                           UT_TYPE_LONG,4,&lNum,
      	                           UT_TYPE_LONG,4,&lStarttime,
      	                           UT_TYPE_LONG,4,&lStoptime,
      	                           UT_TYPE_LONG,4,&lFlags,
      	                           UT_TYPE_STRING,63,caName);
 
    iNum = 0;
   	while((iReturn == 0 ||iReturn==1405)&& iNum < lRowNum) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%u",iNum);
        utPltPutLoopVarF(psDbHead,"lrnum", iNum,"%lu",lNum);
        utPltPutLoopVarF(psDbHead,"lsid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"lpid",    iNum,"%lu", lPid);
        utPltPutLoopVar(psDbHead,"starttime",iNum,ncLimCvtTime2(lStarttime));
        utPltPutLoopVar(psDbHead,"stoptime",iNum,ncLimCvtTime2(lStoptime));
        utPltPutLoopVarF(psDbHead,"flags",iNum,"%lu",lFlags);
        utPltPutLoopVar(psDbHead,"name",iNum,caName);
        
        if(lFlags==0)
        	strcpy(caTemp,"允许使用软件");
        else
          strcpy(caTemp,"禁止使用软件");
          
        sprintf(caTemp+strlen(caTemp),"%s",caName);
        
        utPltPutLoopVar(psDbHead,"lctldes",iNum,caTemp);
        
    lSid=0;
    lPid=0;
    lNum=0;
    lStarttime=0;
    lStoptime=0;
    lFlags=0;
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lPid,
      	                           UT_TYPE_LONG,4,&lNum,
      	                           UT_TYPE_LONG,4,&lStarttime,
      	                           UT_TYPE_LONG,4,&lStoptime,
      	                           UT_TYPE_LONG,4,&lFlags,
      	                           UT_TYPE_STRING,63,caName);
        
    }
    pasDbCloseCursor(psCur);     	            
     
     
     
     
     
   psCur=pasDbOpenSqlF("select id,name from nwsoftclass order by name ");    
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    lId1=0;
    memset(caName1,0,sizeof(caName1));   
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lId1,
      	                           UT_TYPE_STRING,63,caName1);
    iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
        iNum++;      
        utPltPutLoopVarF(psDbHead,"id1",   iNum,"%lu",lId1);       
        utPltPutLoopVar(psDbHead,"name1",iNum,caName1);
        
    lId1=0;
    memset(caName1,0,sizeof(caName1));   
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lId1,
      	                           UT_TYPE_STRING,63,caName1);
        
    }
    pasDbCloseCursor(psCur);  
     
     
     
    utPltPutVar(psDbHead,"stime","00:00:00");
    utPltPutVar(psDbHead,"etime","23:59:59");
    utPltPutVarF(psDbHead,"flags","%d",lFlags);
    utPltPutVarF(psDbHead,"gtype","%lu",lGtype);
    utPltPutVarF(psDbHead,"id","%lu",lId);
    utPltPutVar(psDbHead,"appname",ncLimGetAppName(lGtype,lId));
   
    utPltPutVarF(psDbHead,"CurPg", "%d",lCurPg);
    lTotPg = (lCount ) / lRowNum + 1;
    utPltPutVarF(psDbHead,"TotPg", "%d",lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%d",lTotRec);


   

    i = 0;
    while(lCount < lRowNum) {
        i++;
        lCount++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",lCount);
    }
    

   utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nw_lim_softlist.htm");
    
    return 0;
}
