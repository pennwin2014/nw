#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "pasdb.h"
/* 对一些树的处理  */
int ncWebTreeDispGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
    char caValue[256];
    char caVar[256];
    char caVar1[32];
    char caBfname[32];
   	char caGroupname[32],caGroupid[16];
   	char caFname[32];
   	char caName[32];
   	char caWhere[256];
   	char caSql[256];
   	char caCurPg[16],caTotRec[16];
   	int  iNum;
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
    char *p,*pVar;
   	int iReturn,i;
//   	utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,7,
   	                "var",        UT_TYPE_STRING, 255, caVar,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"TotRec",     UT_TYPE_STRING, 12,  caTotRec,
   	                "name",       UT_TYPE_STRING, 63,  caName,
					"fname",      UT_TYPE_STRING, 31,  caFname,
					"bfname",     UT_TYPE_STRING, 31,  caBfname);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"System Error");
        return 0;
    }
    
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
    if(iReturn == 0 && lGroupid0 > 0) {
        sprintf(caWhere," where groupid = %lu ",lGroupid0);
    }
    if(utStrIsSpaces(caName)) {
        if(utStrIsSpaces(caWhere)) {
            sprintf(caWhere,"where groupname like '%c%s%c' ",'%',caName,'%');
        }
        else {
            utStrAddF(caWhere," and groupname like '%c%s%c' ",'%',caName,'%');
        }
    }
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    if(utStrIsSpaces(caTotRec)) {
        sprintf(caSql,"select count(*) from ncgroup %s",caWhere);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lTotRec);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"DataBase Error %s",pasDbErrorMsg(NULL));
            return 0;
        }
    }
    else {
        lTotRec = atol(caTotRec);
    }
    lRowNum = utComGetVar_ld(psShmHead,"GroupTreeRow",200);
    lStartRec = (lCurPg - 1) * lRowNum;  
    psCur = pasDbOpenSqlF("select groupname,groupid from ncgroup %s order by groupid limit %lu,%lu",caWhere,lStartRec,lRowNum);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caBfname)) utPltPutVar(psDbHead,"bfname",caBfname);
    else utPltPutVar(psDbHead,"bfname",caFname);
    if(!utStrIsSpaces(caFname)) utPltPutVar(psDbHead,"fname",caFname);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        utPltPutLoopVar(psDbHead,"groupname", iNum,caGroupname);
        utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%ld",lGroupid);
        utPltPutLoopVar(psDbHead,"plate",    iNum,caPlate);
        utPltPutLoopVar(psDbHead,"fname",iNum,caFname);
        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum-1);
        if(utStrIsSpaces(caBfname)) {
            utPltPutLoopVar(psDbHead,"bfname",iNum,caFname);
        }
        else {
            utPltPutLoopVar(psDbHead,"bfname",iNum,caBfname);
        }
        iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid);
    }
    pasDbCloseCursor(psCur);
    if(!utStrIsSpaces(caVar)) {
        p = caVar;
        strcpy(caValue,"\0");
        while(*p) {
            p =  utStrGetWord(p,caVar1,31,",;");
            if(!utStrIsSpaces(caVar1)) {
                if(pVar=utMsgGetVar_s(psMsgHead,caVar1)) {
                    utStrAddF(caValue,"&%s@%s",caVar1,pVar);
                }
                if(*p != 0) p++;
            }
            else {
                break;
            }    
        }
// printf(" caValue == [%s] Var=%s\n",caValue,caVar);        
        utPltPutVar(psDbHead,"par",caValue);
        utPltPutVar(psDbHead,"var",caVar);
    }        
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}
    

int ncWebTreeDispUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
   	char caGroupname[32];
   	char caUsername[32],caDispname[32];
   	char caFname[32];
   	char caName[32];
   	char caBfname[32];
   	char caWhere[2560];
   	char caVar[256];
   	char caValue[256];
   	char caVar1[32];
   	char caApp[16];
   	char *pVar,*p;
   	char caSql[4560];
   	char caPar[256];
   	char caIp[16],caMac[20],caComp[32];
   	char caPosition[32];
   	char caTemp[4560];
   	int  iNum;
   	char caGroupid[16],caCurPg[16],caTotRec[16];
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec,lUserid,lTotPg;
    utPltDbHead *psDbHead;
   	int iReturn,i;
   	
   	 long lGid;
   	long nGid[1000];
   	char caGroupbuf[2048];
   	char caGroups[2048];
   	long iNum1;       
    long lGid2;
   	
//   	printf(" ncWebTreeDispUser \n");
    utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,10,
   	                "var",        UT_TYPE_STRING,255,  caVar,
   	                "groupid",    UT_TYPE_STRING, 12,  caGroupid,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"TotRec",     UT_TYPE_STRING, 12,  caTotRec,
   	                "name",       UT_TYPE_STRING, 63,  caName,
					"fname",      UT_TYPE_STRING, 31,  caFname,
					"app",        UT_TYPE_STRING, 12,  caApp,
					"bfname",     UT_TYPE_STRING, 31,  caBfname,
					"par",       UT_TYPE_STRING,255,caPar);
					
			memset(caGroupbuf,0,sizeof(caGroupbuf));
			
		if(atol(caGroupid)>0){
			strcpy(caGroupbuf,getGroupid(atol(caGroupid)));
/*			
	  lGid=atol(caGroupid);
	  sprintf(caGroupbuf,"%d",lGid);
	  iNum1=0;
	  	if(lGid>0){
	  	iNum1=0;
    	psCur = pasDbOpenSqlF("select groupid from ncgroup where pid=%d ",lGid); 
      lGid2=0;
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid2);                             
	    while((iReturn==0)||(iReturn==1405))
	    {
	    	  sprintf(caGroupbuf+strlen(caGroupbuf),",%d",lGid2);
	  	    nGid[iNum1]=lGid2;
	        iNum1++;       
	        lGid2=0;
	        if(iNum1>900) break;
	        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid2);
	    }
	    pasDbCloseCursor(psCur);
	   }
	   for(i=0;i<iNum1;i++){
	   	
	   	psCur = pasDbOpenSqlF("select groupid from ncgroup where pid=%d ",nGid[i]); 
      lGid2=0;
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid2);                             
	    while((iReturn==0)||(iReturn==1405))
	    {
	    	  if(strlen(caGroupbuf)>2000) break;
	    	  sprintf(caGroupbuf+strlen(caGroupbuf),",%d",lGid2);	 
	        lGid2=0;      
	        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid2);
	    }
	    pasDbCloseCursor(psCur);
	  }
*/
}
			

			
			
					
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),ncLang("0554数据库出现问题，请与管理员联系"));
        return 0;
    }
    
    lGroupid = atol(caGroupid);
    strcpy(caGroups,getDsGroupid());
    
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    
    strcpy(caWhere,"\0");
/*    if(iReturn == 0 && lGroupid0 > 0 && lGroupid != lGroupid0) {
        psDbHead = utPltInitDb();
        if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
        if(!utStrIsSpaces(caName)) utPltPutVar(psDbHead,"name",caName);
        utPltPutVarF(psDbHead, "TotRec","%lu",0);
        utPltPutVarF(psDbHead, "TotPg", "%lu",0);
        utPltPutVarF(psDbHead, "CurPg", "%lu",0);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
        return 0;
    }
*/
//    else {
    	if((utStrIsSpaces(caName))&&(strlen(caGroupid)!=0)){
    		  if(strcmp(caGroupid,"0")==0){
    		  	sprintf(caWhere," where groupid=0 ");
    		  }
    		  else
        	  sprintf(caWhere," where groupid  in (%s) and usestatus=1 ",caGroupbuf);
        }
        else
        sprintf(caWhere," where usestatus=1 ");
 //   }
    if(!utStrIsSpaces(caName)) {
    	
        utStrAddF(caWhere," and ( dispname like '%c%s%c' or username like '%c%s%c' or inet_ntoa(ip)='%s' or position like '%c%s%c'  or mac like '%c%s%c' ) ",'%',caName,'%','%',caName,'%',caName,'%',caName,'%','%',caName,'%');
    }
    if(strlen(caGroups)>0){
    	utStrAddF(caWhere," and groupid in (%s) ",caGroups);
    }
   
    lCurPg = atol(caCurPg);
    if(strcmp(caApp,"prev") == 0) {
//        lCurPg = lCurPg - 1;
    }
    else if(strcmp(caApp,"next") == 0) {
//        lCurPg = lCurPg + 1;
    }
    else {
        lCurPg = 1;
        strcpy(caTotRec,"\0");
    }
    if(lCurPg <= 0) lCurPg = 1;
    if(utStrIsSpaces(caTotRec) || utStrIsSpaces(caApp)) {
        sprintf(caSql,"select count(*) from ncuser %s",caWhere);

        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lTotRec);
        if(iReturn != 0) {
            printf("1Sql:[select count(*) from ncuser %s ]\n",caWhere);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"DataBase Error %s",pasDbErrorMsg(NULL));
            return 0;
        }
    }
    else {
        lTotRec = atol(caTotRec);
    }
    lRowNum = utComGetVar_ld(psShmHead,"UserTreeRow",200);
    lTotPg = 1 + (lTotRec - 1) / lRowNum;
    if(lCurPg > lTotPg) lCurPg = lTotPg;

    lStartRec = (lCurPg - 1) * lRowNum;  
printf("select username,dispname,userid,inet_ntoa(ip),mac,compname,position from ncuser %s  limit %lu,%lu\n",caWhere,lStartRec,lRowNum);

    psCur = pasDbOpenSqlF("select username,dispname,userid,inet_ntoa(ip),mac,compname,position from ncuser %s  limit %lu,%lu",caWhere,lStartRec,lRowNum);
    if(psCur == NULL) {
        printf("Sql:[select username,dispname,userid from ncuser %s order by userid limit %lu,%lu]\n",caWhere,lStartRec,lRowNum);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    memset(caIp,0,sizeof(caIp));
    memset(caMac,0,sizeof(caMac));
    memset(caComp,0,sizeof(caComp));
    memset(caPosition,0,sizeof(caPosition));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_STRING,15,caIp,
                                   UT_TYPE_STRING,18,caMac,
                                   UT_TYPE_STRING,30,caComp,
                                   UT_TYPE_STRING,30,caPosition);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caName)) utPltPutVar(psDbHead,"name",caName);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%lu",lUserid);
        utPltPutLoopVar(psDbHead,"fname",iNum,caFname);
        if(utStrIsSpaces(caBfname)) {
            utPltPutLoopVar(psDbHead,"bfname",iNum,caFname);
        }
        else {
            utPltPutLoopVar(psDbHead,"bfname",iNum,caBfname);
        }
        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum);
        sprintf(caTemp,"[帐号:%s]",caUsername);
        if(strlen(caIp)>0)
        sprintf(caTemp+strlen(caTemp),"[IP:%s]",caIp);
        
        if(strlen(caMac)>0)
        sprintf(caTemp+strlen(caTemp),"[MAC:%s]",caMac);
        
        if(strlen(caComp)>0)
        sprintf(caTemp+strlen(caTemp),"[电脑名:%s]",caComp);
        
        
 utPltSetCvtHtml(1);       
        utPltPutLoopVar(psDbHead, "dispname", iNum,caDispname);
        utPltPutLoopVar(psDbHead, "username", iNum,caUsername);
        utPltPutLoopVar(psDbHead,"userdesc",iNum,caTemp);
        if(strlen(caPosition)==0) strcpy(caPosition,caDispname);
        utPltPutLoopVar(psDbHead,"position",iNum,caPosition);
utPltSetCvtHtml(0);        
    memset(caIp,0,sizeof(caIp));
    memset(caMac,0,sizeof(caMac));
    memset(caComp,0,sizeof(caComp));
    memset(caPosition,0,sizeof(caPosition));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_STRING,15,caIp,
                                   UT_TYPE_STRING,18,caMac,
                                   UT_TYPE_STRING,30,caComp,
                                   UT_TYPE_STRING,30,caPosition);
    }
    pasDbCloseCursor(psCur);
    utPltPutVarF(psDbHead, "TotRec","%lu",lTotRec);
    utPltPutVarF(psDbHead, "TotPg", "%lu",lTotPg);
    utPltPutVarF(psDbHead, "CurPg", "%lu",lCurPg);
    utPltPutVar(psDbHead, "groupid",caGroupid);
    
    memset(caValue,0,sizeof(caValue));
    if(!utStrIsSpaces(caVar)) {
        p = caVar;
        strcpy(caValue,"\0");
        while(*p) {
            p =  utStrGetWord(p,caVar1,31,",;");
            if(!utStrIsSpaces(caVar1)) {
// printf(" Varn==[%s]\n",caVar1);                
                if(pVar=utMsgGetVar_s(psMsgHead,caVar1)) {
                    utStrAddF(caValue,"&%s@%s",caVar1,pVar);
                }
                if(*p != 0) p++;
            }
            else {
                break;
            }    
        }


        
        utPltPutVar(psDbHead,"var",caVar);
    } 
    
    if(strlen(caValue)==0)
     utPltPutVar(psDbHead,"par",caPar);
     else
        utPltPutVar(psDbHead,"par",caValue);     
    if(!utStrIsSpaces(caFname)) {
        utPltPutVar(psDbHead,"fname",caFname);
    }
    if(utStrIsSpaces(caBfname)) {
        utPltPutVar(psDbHead,"bfname",caFname);
    }
    else {
        utPltPutVar(psDbHead,"bfname",caBfname);
    }
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}

/* 显示用户树  */
int ncWebUserTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    char caFname[64];
    char caValue[256];
    char caVar[256],caVar1[32];
    char *p,*pVar;
    char  caPlate[128];
    char caBfname[32];
    int  iReturn;
// utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead,4,
                    "fname",      UT_TYPE_STRING,63,   caFname,
                    "bfname",     UT_TYPE_STRING,31,   caBfname,
   	                "var",        UT_TYPE_STRING,255,  caVar,
					"plate",      UT_TYPE_STRING, 127, caPlate);
    psDbHead = utPltInitDb();
    if(!utStrIsSpaces(caPlate)) {
        utPltPutVar(psDbHead,"plate",caPlate);
    }
    if(!utStrIsSpaces(caFname)) {
        utPltPutVar(psDbHead,"fname",caFname);
    }
    if(utStrIsSpaces(caBfname)) {
        utPltPutVar(psDbHead,"bfname",caFname);
    }
    else {
        utPltPutVar(psDbHead,"bfname",caBfname);
    }
    if(!utStrIsSpaces(caVar)) {
        p = caVar;
        strcpy(caValue,"\0");
        while(*p) {
            p =  utStrGetWord(p,caVar1,31,",;");
            if(!utStrIsSpaces(caVar1)) {
// printf(" Varn==[%s]\n",caVar1);                
                if(pVar=utMsgGetVar_s(psMsgHead,caVar1)) {
                    utStrAddF(caValue,"&%s@%s",caVar1,pVar);
                }
                if(*p != 0) p++;
            }
            else {
                break;
            }    
        }
// printf(" Value==%s Var==%s \n",caValue,caVar);
        utPltPutVar(psDbHead,"par",caValue);
        utPltPutVar(psDbHead,"var",caVar);
    }        
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
}



/* 以下为改进后的用户树  */
int ncWebTreeDispGroup_F(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
   	char caGroupname[32],caGroupid[16];
   	char caFname[32];
   	char caName[32];
   	char caWhere[256];
   	char caSql[256];
   	char caCurPg[16],caTotRec[16];
   	int  iNum;
  	char caFilter[256];
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
   	int iReturn,i;
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"TotRec",     UT_TYPE_STRING, 12,  caTotRec,
   	                "name",       UT_TYPE_STRING, 63,  caName,
					"fname",      UT_TYPE_STRING, 31,  caFname);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"System Error");
        return 0;
    }
    
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
    if(iReturn == 0 && lGroupid0 > 0) {
        sprintf(caWhere,"where groupid = %lu ",lGroupid0);
    }
    if(utStrIsSpaces(caName)) {
        if(utStrIsSpaces(caWhere)) {
            sprintf(caWhere,"where groupname like '%c%s%c' ",'%',caName,'%');
        }
        else {
            utStrAddF(caWhere," and groupname like '%c%s%c' ",'%',caName,'%');
        }
    }
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    if(utStrIsSpaces(caTotRec)) {
        sprintf(caSql,"select count(*) from ncgroup %s",caWhere);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lTotRec);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"DataBase Error %s",pasDbErrorMsg(NULL));
            return 0;
        }
    }
    else {
        lTotRec = atol(caTotRec);
    }
    lRowNum = utComGetVar_ld(psShmHead,"GroupTreeRow",200);
    lStartRec = (lCurPg - 1) * lRowNum;  
    psCur = pasDbOpenSqlF("select groupname,groupid from ncgroup %s order by groupid limit %lu,%lu",caWhere,lStartRec,lRowNum);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caFname)) utPltPutVar(psDbHead,"fname",caFname);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        utPltPutLoopVar(psDbHead,"groupname", iNum,caGroupname);
        utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%ld",lGroupid);
        utPltPutLoopVar(psDbHead,"plate",    iNum,caPlate);
        utPltPutLoopVar(psDbHead,"fname",iNum,caFname);
        iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid);
    }
    pasDbCloseCursor(psCur);
    ncWebUtlCvtIpGet(psMsgHead,caFilter);
    utPltPutVar(psDbHead,"filter",caFilter);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}
    

int ncWebTreeDispUser_F(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
   	char caGroupname[32];
   	char caUsername[32],caDispname[32];
   	char caPosition[32];
   	char caApp[16];
   	char caFname[32];
   	char caName[32];
   	char caWhere[256];
   	char caSql[556];
   	char caFilter[256];
   	int  iNum;
   	char *p1=NULL,*p2=NULL;
   	char caGroupid[16],caCurPg[16],caTotRec[16];
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec,lUserid,lTotPg;
    utPltDbHead *psDbHead;
   	int iReturn,i;
    utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,7,
   	                "app",        UT_TYPE_STRING, 12,  caApp,
   	                "groupid",    UT_TYPE_STRING, 12,  caGroupid,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"TotRec",     UT_TYPE_STRING, 12,  caTotRec,
   	                "name",       UT_TYPE_STRING, 63,  caName,
					"fname",      UT_TYPE_STRING, 31,  caFname);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),ncLang("0554数据库出现问题，请与管理员联系"));
        return 0;
    }
    
    lGroupid = atol(caGroupid);
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
    if(iReturn == 0 && lGroupid0 > 0 && lGroupid != lGroupid0) {
        psDbHead = utPltInitDb();
        if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
        if(!utStrIsSpaces(caName)) utPltPutVar(psDbHead,"name",caName);
        utPltPutVarF(psDbHead, "TotRec","%lu",0);
        utPltPutVarF(psDbHead, "TotPg", "%lu",0);
        utPltPutVarF(psDbHead, "CurPg", "%lu",0);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
        return 0;
    }
    else {
        sprintf(caWhere," where groupid = %lu ",lGroupid);
    }
    if(!utStrIsSpaces(caName)) {
        utStrAddF(caWhere," and (dispname like '%c%s%c' or username like '%c%s%c' or position like '%c%s%c') ",'%',caName,'%','%',caName,'%','%',caName,'%');
    }
    printf("caWhere=%s\n",caWhere);
    lCurPg = atol(caCurPg);
    if(strcmp(caApp,"prev") == 0) {
//        lCurPg = lCurPg - 1;
    }
    else if(strcmp(caApp,"next") == 0) {
//        lCurPg = lCurPg + 1;
    }
    else {
        lCurPg = 1;
        strcpy(caTotRec,"\0");
    }
    if(lCurPg <= 0) lCurPg = 1;
    if(utStrIsSpaces(caTotRec)|| utStrIsSpaces(caApp)) {
        sprintf(caSql,"select count(*) from ncuser %s",caWhere);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lTotRec);
        if(iReturn != 0) {
            printf("1Sql:[select count(*) from ncuser %s ]\n",caWhere);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"DataBase Error %s",pasDbErrorMsg(NULL));
            return 0;
        }
    }
    else {
        lTotRec = atol(caTotRec);
    }
    lRowNum = utComGetVar_ld(psShmHead,"UserTreeRow",200);
    lTotPg = 1 + (lTotRec - 1) / lRowNum;
    if(lCurPg > lTotPg) lCurPg = lTotPg;

    lStartRec = (lCurPg - 1) * lRowNum;  
    psCur = pasDbOpenSqlF("select username,dispname,userid,position from ncuser %s order by dispname limit %lu,%lu",caWhere,lStartRec,lRowNum);
    if(psCur == NULL) {
        printf("Sql:[select username,dispname,userid from ncuser %s order by userid limit %lu,%lu]\n",caWhere,lStartRec,lRowNum);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    memset(caPosition,0,sizeof(caPosition));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_STRING,30,caPosition);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caFname)) utPltPutVar(psDbHead,"fname",caFname);
    if(!utStrIsSpaces(caName)) utPltPutVar(psDbHead,"name",caName);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%lu",lUserid);
 utPltSetCvtHtml(1);       
        utPltPutLoopVar(psDbHead, "dispname", iNum,caDispname);
        utPltPutLoopVar(psDbHead, "username", iNum,caUsername);
        if(strlen(caPosition)==0) strcpy(caPosition,caDispname);
        utPltPutLoopVar(psDbHead,"position",iNum,caPosition);
 utPltSetCvtHtml(0);       
    memset(caPosition,0,sizeof(caPosition));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_STRING,30,caPosition);
    }
    pasDbCloseCursor(psCur);
    ncWebUtlCvtIpGet(psMsgHead,caFilter);
    if(!utStrIsSpaces(caFname)) {
        utPltPutVar(psDbHead,"fname",caFname);
    }
    utPltPutVar(psDbHead,"filter",caFilter);
    utPltPutVarF(psDbHead, "TotRec","%lu",lTotRec);
    utPltPutVarF(psDbHead, "TotPg", "%lu",lTotPg);
    utPltPutVarF(psDbHead, "CurPg", "%lu",lCurPg);
    utPltPutVarF(psDbHead, "groupid","%ld",lGroupid);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}


//显示策略分配部门选择界面
int ncWebTreeDispGroupProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
    char caValue[256];
    char caVar[256];
    char caVar1[32];
    char caBfname[32];
   	char caGroupname[32],caGroupid[16];
   	char caFname[32];
   	char caName[32];
   	char caWhere[256];
   	char caSql[256];
   	char caCurPg[16],caTotRec[16];
   	char caTemp[1024];
   	int  iNum;
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
    char *p,*pVar;
   	int iReturn,i,lCount;
   	char caId[16];
//   	utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,7,
   	                "id",        UT_TYPE_STRING, 10, caId,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"TotRec",     UT_TYPE_STRING, 12,  caTotRec,
   	                "gname",       UT_TYPE_STRING, 63,  caName,
					"fname",      UT_TYPE_STRING, 31,  caFname,
					"bfname",     UT_TYPE_STRING, 31,  caBfname);
        psDbHead = utPltInitDb();
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
    sprintf(caTemp,"select count(*) from ncgroup where 1=1 ");
    if(iReturn == 0 && lGroupid0 > 0) {
    	sprintf(caTemp+strlen(caTemp)," and groupid = %lu ",lGroupid0);      
    }
    if(!utStrIsSpaces(caName)) {
    	sprintf(caTemp+strlen(caTemp)," and groupname like '%c%s%c' ",'%',caName,'%');
       
    }
    pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
    utPltPutVarF(psDbHead,"sum","%d",lCount);
    utPltPutVar(psDbHead,"id",caId);
    
    strcpy(caTemp,"select groupname,groupid from ncgroup where 1=1 ");
    
    if(iReturn == 0 && lGroupid0 > 0) {
    	sprintf(caTemp+strlen(caTemp)," and groupid = %lu ",lGroupid0);
      
    }
    if(!utStrIsSpaces(caName)) {
    	sprintf(caTemp+strlen(caTemp)," and groupname like '%c%s%c' ",'%',caName,'%');
       
    }
     sprintf(caTemp+strlen(caTemp)," order by pid ");
     
    psCur = pasDbOpenSql(caTemp,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }

    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid);
   
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        utPltPutLoopVar(psDbHead,"groupname", iNum,caGroupname);
        utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%ld",lGroupid);
       
        utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum-1);
       
        iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid);
    }
    pasDbCloseCursor(psCur);
      
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_procy_deptree.htm");
    return 0;
}


//显示策略分配用户选择界面
int ncWebTreeDispYhProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
    char caValue[256];
    char caVar[256];
    char caVar1[32];
    char caBfname[32];
   	char caGroupname[32],caGroupid[16];
   	char caFname[32];
   	char caName[32];
   	char caWhere[256];
   	char caSql[256];
   	char caCurPg[16],caTotRec[16];
   	char caTemp[1024];
   	int  iNum;
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
    char *p,*pVar;
   	int iReturn,i,lCount;
   	char caDispname[32],caUsername[32],caIp[20],caMac[20];
   	long lUserid;
   	char caId[16];
   	char *pBuf,*pSelect;
//   	utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,7,
   	                "id",        UT_TYPE_STRING, 10, caId,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"groupid",     UT_TYPE_STRING, 12,  caGroupid,
   	       "gname",       UT_TYPE_STRING, 63,  caName,
					"fname",      UT_TYPE_STRING, 31,  caFname,
					"bfname",     UT_TYPE_STRING, 31,  caBfname);
        psDbHead = utPltInitDb();
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
    sprintf(caTemp,"select count(*) from ncuser where 1=1 ");
    if(iReturn == 0 && lGroupid0 > 0) {
    	sprintf(caTemp+strlen(caTemp)," and groupid = %lu ",lGroupid0);      
    }
    if(!utStrIsSpaces(caName)) {
    	sprintf(caTemp+strlen(caTemp)," and (dispname like '%c%s%c' or username like '%c%s%c' or inet_ntoa(ip) like '%c%s%c' or mac like '%c%s%c') ",'%',caName,'%','%',caName,'%','%',caName,'%','%',caName,'%');
       
    }
    if(strlen(caGroupid)>0){
    	sprintf(caTemp+strlen(caTemp)," and groupid=%s ",caGroupid);
    }
    pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
    utPltPutVarF(psDbHead,"sum","%d",lCount);
    utPltPutVar(psDbHead,"id",caId);
    
    strcpy(caTemp,"select dispname,username,userid,inet_ntoa(ip),mac,groupid from ncuser where 1=1 ");
    
    if(iReturn == 0 && lGroupid0 > 0) {
    	sprintf(caTemp+strlen(caTemp)," and groupid = %lu ",lGroupid0);
      
    }
    if(!utStrIsSpaces(caName)) {
    	sprintf(caTemp+strlen(caTemp)," and (dispname like '%c%s%c' or username like '%c%s%c' or inet_ntoa(ip) like '%c%s%c' or mac like '%c%s%c') ",'%',caName,'%','%',caName,'%','%',caName,'%','%',caName,'%');
       
    }
    if(strlen(caGroupid)>0){
    	sprintf(caTemp+strlen(caTemp)," and groupid=%s ",caGroupid);
    }
     sprintf(caTemp+strlen(caTemp)," order by username limit 0,500 ");
printf("caTemp=%s\n",caTemp);     
    psCur = pasDbOpenSql(caTemp,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    memset(caDispname,0,sizeof(caDispname));
    memset(caUsername,0,sizeof(caUsername));
    lUserid=0;
    memset(caIp,0,sizeof(caIp));
    memset(caMac,0,sizeof(caMac));
    lGroupid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_STRING,30,caUsername,
                                   UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_STRING,15,caIp,
                                   UT_TYPE_STRING,19,caMac,
                                   UT_TYPE_LONG,4,&lGroupid);
   
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        sprintf(caTemp,"%s (",caDispname);
        if(strlen(caIp)>0){
        	sprintf(caTemp+strlen(caTemp),"IP:%s ",caIp);
        }
        if(strlen(caMac)>0){
        	sprintf(caTemp+strlen(caTemp),"MAC:%s ",caMac);
        }
        sprintf(caTemp+strlen(caTemp),")");
        utPltPutLoopVar(psDbHead,"dispname", iNum,caTemp);
        utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
        utPltPutLoopVar(psDbHead,"mac",iNum,caMac);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%ld",lUserid);
        
        utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%ld",lGroupid);
       
        utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum-1);
       
    memset(caDispname,0,sizeof(caDispname));
    memset(caUsername,0,sizeof(caUsername));
    lUserid=0;
    memset(caIp,0,sizeof(caIp));
    memset(caMac,0,sizeof(caMac));
    lGroupid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_STRING,30,caUsername,
                                   UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_STRING,15,caIp,
                                   UT_TYPE_STRING,19,caMac,
                                   UT_TYPE_LONG,4,&lGroupid);
    }
    pasDbCloseCursor(psCur);
      
      
    pBuf=pasUtlCvtSqlSelectTreeBmA(ncLang("0455全部,;"));
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
        free(pBuf);
        utPltPutVar(psDbHead,"sgroup", pSelect);
        free(pSelect);
    } 
      
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_procy_usertree.htm");
    return 0;
}