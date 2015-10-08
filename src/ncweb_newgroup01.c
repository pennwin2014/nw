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

int IsInGroupids(char *ids,char *id){
	char *p,*p2;
	char caId[16];
	p2=ids;
	p = strstr(p2,",");
	while(p){
		memset(caId,0,sizeof(caId));
		strncpy(caId,p2,p-p2);	
	
		if(strcmp(caId,id)==0){
			return 0;
		}
		p2=p+1;
    p = strstr(p2,",");
  }
		memset(caId,0,sizeof(caId));
		strcpy(caId,p2);	
	
		if(strcmp(caId,id)==0){
			return 0;
		}
return -1;
}
/* 以下为改进后的用户树  */
int ncWebTreeDispGroup_New(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caLevel[16];
    char caPlate[128];
   	char caGroupname[36],caGroupid[16];
   	
   	char caName[32];
   	char caWhere[256];
   	char caSql[256];
   	char caCurPg[16],caTotRec[16];
   	char caPid[16];
   	unsigned long lLevel,lPid;
   	int  iNum;
  	char caFilter[256];
  	char caNo[16];
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
   	int iReturn,i;
   	char caVar[256],caFname[64],caBfname[64];
   	char caValue[256];
   	char caVar1[32];
   	char *p,*pVar;
#ifdef LDEBUG1 
    utMsgPrintMsg(psMsgHead);
#endif  	
   	iReturn = utMsgGetSomeNVar(psMsgHead,7,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"level",      UT_TYPE_STRING,10,caLevel,
					"groupid",        UT_TYPE_STRING,10,caPid,
					"num",        UT_TYPE_STRING,10,caNo,
					"var",        UT_TYPE_STRING,255,caVar,
					"fname",      UT_TYPE_STRING,63,caFname,
					"bfname",     UT_TYPE_STRING,63,caBfname);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"System Error");
        return 0;
    }
    
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
    if(iReturn == 0 && lGroupid0 > 0) {
        sprintf(caWhere,"and (groupid = %lu or pid=%lu) ",lGroupid0,lGroupid0);
    }
    
  //  sprintf(caWhere+strlen(caWhere)," and level=%d ",atol(caLevel));

//    if(strlen(caPid)>0)
//      sprintf(caWhere+strlen(caWhere)," and pid=%d ",atol(caPid));
//    else{
//    	 sprintf(caWhere+strlen(caWhere)," and pid=0 and level=0",atol(caPid));
//    }
// printf("select groupname,groupid,level,pid from ncgroup where 1=1 %s order by groupname limit %lu,%lu\n",caWhere,0,lRowNum);
    lRowNum = utComGetVar_ld(psShmHead,"GroupTreeRow",200);
    lStartRec = (lCurPg - 1) * lRowNum;  
    psCur = pasDbOpenSqlF("select groupname,groupid,level,pid from ncgroup where 1=1 %s order by groupname limit %lu,%lu",caWhere,0,lRowNum);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    
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
     }
    
    
    utPltPutVar(psDbHead,"fname",caFname);
    utPltPutVar(psDbHead,"var",caVar);
    utPltPutVar(psDbHead,"bfname",caBfname);
    memset(caGroupname,0,sizeof(caGroupname));
    lGroupid=0;
    lLevel=0;
    lPid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,32,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid,
                                   UT_TYPE_LONG,4,&lLevel,
                                   UT_TYPE_LONG,4,&lPid);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caFname)) utPltPutVar(psDbHead,"fname",caFname);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum-1);
        utPltPutLoopVar(psDbHead,"groupname", iNum,caGroupname);
        utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%ld",lGroupid);
        utPltPutLoopVarF(psDbHead,"level",iNum,"%d",lLevel);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%d",lPid);
        utPltPutLoopVar(psDbHead,"plate",    iNum,caPlate);
        utPltPutLoopVar(psDbHead,"fname",iNum,caFname);
		    memset(caGroupname,0,sizeof(caGroupname));
		    lGroupid=0;
		    lLevel=0;
		    lPid=0;
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,32,caGroupname,
		                                   UT_TYPE_LONG,4,&lGroupid,
		                                   UT_TYPE_LONG,4,&lLevel,
		                                   UT_TYPE_LONG,4,&lPid);
    }
    pasDbCloseCursor(psCur);
   
    utPltPutVar(psDbHead,"no",caNo);
    utPltPutVar(psDbHead,"level",caLevel);
    utPltPutVar(psDbHead,"corpname",utComGetVar_sd(psShmHead,"rootname",ncLang("0269公司")));
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}





/* 显示二级树  */
int ncWebTreeDispGroup_New2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caLevel[16];
    char caPlate[128];
   	char caGroupname[36],caGroupid[16];
   	char caFname[32];
   	char caName[32];
   	char caWhere[256];
   	char caSql[256];
   	char caCurPg[16],caTotRec[16];
   	char caPid[16];
   	unsigned long lLevel,lPid;
   	int  iNum;
  	char caFilter[256];
  	char caNo[16];
  	long group_flag;
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
   	int iReturn,i;
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"level",      UT_TYPE_STRING,10,caLevel,
					"groupid",        UT_TYPE_STRING,10,caPid,
					"num",        UT_TYPE_STRING,10,caNo);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"System Error");
        return 0;
    }
    group_flag=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
    if(iReturn == 0 && lGroupid0 > 0) {
        sprintf(caWhere,"and ( pid=%lu) ",lGroupid0);
        group_flag=1;
    }
    
  //  sprintf(caWhere+strlen(caWhere)," and level=%d ",atol(caLevel));

    lRowNum = utComGetVar_ld(psShmHead,"GroupTreeRow",1200);
    lStartRec = (lCurPg - 1) * lRowNum;  
    psCur = pasDbOpenSqlF("select groupname,groupid,level,pid from ncgroup where 1=1 %s order by groupname limit %lu,%lu",caWhere,0,lRowNum);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    if(lGroupid0>0){
    	utPltPutVarF(psDbHead,"gid","%d",lGroupid0);
    	utPltPutVar(psDbHead,"gname",ncUtlGetGroupNameByid(psShmHead,lGroupid0,"\0"));
    }
    memset(caGroupname,0,sizeof(caGroupname));
    lGroupid=0;
    lLevel=0;
    lPid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,32,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid,
                                   UT_TYPE_LONG,4,&lLevel,
                                   UT_TYPE_LONG,4,&lPid);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caFname)) utPltPutVar(psDbHead,"fname",caFname);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum-1);
        utPltPutLoopVar(psDbHead,"groupname", iNum,caGroupname);
        utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%ld",lGroupid);
        utPltPutLoopVarF(psDbHead,"level",iNum,"%d",lLevel);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%d",lPid);
        utPltPutLoopVar(psDbHead,"plate",    iNum,caPlate);
        utPltPutLoopVar(psDbHead,"fname",iNum,caFname);
		    memset(caGroupname,0,sizeof(caGroupname));
		    lGroupid=0;
		    lLevel=0;
		    lPid=0;
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,32,caGroupname,
		                                   UT_TYPE_LONG,4,&lGroupid,
		                                   UT_TYPE_LONG,4,&lLevel,
		                                   UT_TYPE_LONG,4,&lPid);
    }
    pasDbCloseCursor(psCur);
   
   utPltPutVar(psDbHead,"no",caNo);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}



/* 显示部门管理页面  */
int ncWebDispDeptForm_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caGroupname[34];
   	char caTemp1[1024];
   	int iNum1,i,lGid;
   	long lUserid,lGroupid;
   	char caGroupid[16];
   	char caOrderby[32];
   	char *pBuf,*pSelect;
    utPltDbHead *psDbHead;
    char caTemp[1024];
   	int iReturn;
    char caLevel[12],caPid[12];
    char caGid[16];
    char caPlate[128];
    long lGroupid0,group_flag;
    char caVar[256],caFname[256];
    char gn[100][32];
    long lTgid;
    group_flag=0;
    
    strcpy(caOrderby,utComGetVar_sd(psShmHead,"grouporder","groupname"));
   iReturn=dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,4,&lGroupid0);
	  if((iReturn==0)&&(lGroupid0>0)){
	  	group_flag=1;
	  	strcpy(caLevel,"1");
	  	
	  }
	  else
	   lGroupid0=0;
    
   	iReturn = utMsgGetSomeNVar(psMsgHead,6,
   	      "gid2",        UT_TYPE_STRING,10,caGid,
					"level",      UT_TYPE_STRING,10,caLevel,
					"pid",        UT_TYPE_STRING,10,caPid,
					"plate",      UT_TYPE_STRING,100,caPlate,
					"var",        UT_TYPE_STRING,255,caVar,
					"fname",      UT_TYPE_STRING,255,caFname);




    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"level",caLevel);
    utPltPutVar(psDbHead,"pid",caPid);
    
    utPltPutVar(psDbHead,"var",caVar);
    utPltPutVar(psDbHead,"fname",caFname);
    utPltPutVar(psDbHead,"gid",caGid);
    memset(caGroupname,0,sizeof(caGroupname));
    memset(caTemp1,0,sizeof(caTemp1));
 
    if(group_flag==1){
    	
    	iNum1=0;
    	iReturn=0;
    
    lTgid=lGroupid0;
    	while(1){
    		sprintf(caTemp,"select groupname,pid from ncgroup where groupid=%d and groupid!=0 ",lTgid);
    		memset(caGroupname,0,sizeof(caGroupname));

    	  iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,31,caGroupname,
    	                        UT_TYPE_LONG,4,&lGid);
    	  if(strlen(caGroupname)>0){
    	  	lTgid=lGid;
    	    strcpy(gn[iNum1],caGroupname); 
    	    iNum1++;
    	  }  
    	  else{
    	  	break;
    	  }                
    	}
    	
    	for(i=0;i<iNum1;i++){
    		printf("caTemp1=%s\n",caTemp1);
    		sprintf(caTemp1+strlen(caTemp1),"/%s",gn[iNum1-i-1]);
    	}
      if(strlen(caTemp1)>0)
    	   utPltPutVar(psDbHead,"gname",caTemp1);
    	
    	
    	
    	 sprintf(caTemp,"select groupname from ncgroup where groupid=%d ",lGroupid0);

    	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,32,caGroupname);
    	utPltPutVar(psDbHead,"gname",caGroupname);
    }
    else if(strlen(caGid)>0){
    	lGid=atol(caGid);
    	
    	iNum1=0;
    	iReturn=0;
    
    	while(1){
    		sprintf(caTemp,"select groupname,pid from ncgroup where groupid=%d and groupid!=0 ",lGid);

    		memset(caGroupname,0,sizeof(caGroupname));
    	  iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,31,caGroupname,
    	                        UT_TYPE_LONG,4,&lGid);
    	  if(strlen(caGroupname)>0){
    	    strcpy(gn[iNum1],caGroupname); 
    	    iNum1++;
    	  }  
    	  else{
    	  	break;
    	  }                
    	}
    	for(i=0;i<iNum1;i++){
    		sprintf(caTemp1+strlen(caTemp1),"/%s",gn[iNum1-i-1]);
    	}
      if(strlen(caTemp1)>0)
    	   utPltPutVar(psDbHead,"gname",caTemp1);
    }
    
    
    if(strlen(caTemp1)==0){
    	utPltPutVar(psDbHead,"gname",ncLang("0269公司"));
    }
    
    sprintf(caTemp,"select groupname,groupid from ncgroup where groupid <>0 ");
    if(strlen(caGid)>0){
    	sprintf(caTemp+strlen(caTemp)," and pid=%s and level!=0 order by %s desc ",caGid,caOrderby);
    }
    else if(group_flag==1){
    	sprintf(caTemp+strlen(caTemp)," and pid =%d  order by %s desc ",lGroupid0,caOrderby);
    }
    else{
    	sprintf(caTemp+strlen(caTemp)," and level=0 order by %s desc ",caOrderby);
    }
    printf("caTemp=%s\n",caTemp);
    pBuf = pasUtlCvtSqlSelect(caTemp);
    if(pBuf == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771用户信息"),ncLang("0153部门列表不存在"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    strcpy(caGroupid,"\0");
    pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
    if(pSelect == NULL) {
        free(pBuf);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771用户信息"),ncLang("0404内存出错"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    utPltPutVar(psDbHead,"dept", pSelect);
    free(pSelect);
    
    pSelect= pasUtlCvtInputSelect("gid",caGroupid,pBuf);
    if(pSelect == NULL) {
        free(pBuf);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771用户信息"),ncLang("0404内存出错"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    utPltPutVar(psDbHead,"mdept", pSelect);
    
    
    pSelect= pasUtlCvtInputSelect("mgid",caGroupid,pBuf);
    if(pSelect == NULL) {
        free(pBuf);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771用户信息"),ncLang("0404内存出错"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    utPltPutVar(psDbHead,"mmdept", pSelect);
    
    
    free(pSelect);
    free(pBuf);
    
    if(strlen(caPlate)>0){
    	 utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    	 return 0;
    }
    
   utPltPutVar(psDbHead,"rootname",utComGetVar_sd(psShmHead,"rootname",ncLang("0269公司")));
    
    
    if(group_flag==1)
       utPltOutToHtml(iFd,psMsgHead,psDbHead,"group/nc_form_group_new2.htm");
    else
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_group_new.htm");
    return 0;
}

/* 部门增加  */
int ncWebDeptAdd_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caGroupname[34];
   	long lGroupid;
   	int iReturn;
    char caTemp[1024];
    char caGid[16];
    char caLevel[16];
    int group_flag;
    long lGroupid0;
    long iLevel;
    group_flag=0;

    
   	iReturn = utMsgGetSomeNVar(psMsgHead,3,
					"groupname",    UT_TYPE_STRING, 32, caGroupname,
					"gid2",          UT_TYPE_STRING,10,  caGid,
					"level",  UT_TYPE_STRING,10,caLevel);
					
	 iReturn=dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,4,&lGroupid0);
	  if((iReturn==0)&&(lGroupid0>0)){
	  	group_flag=1;
	  	strcpy(caLevel,"1");
	  	sprintf(caGid,"%d",lGroupid0);
	  }
	  else{
	   lGroupid0=0;
    }			
			
		if(strlen(caLevel)==0||strcmp(caGid,"0")==0){
			iLevel=0;
		}
		else{
			iLevel=atol(caLevel)+1;
		}			
    utStrDelSpaces(caGroupname);
    if(utStrIsSpaces(caGroupname)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0837增加部门"),ncLang("0154部门名称为空"));
        return 0;
    }
    lGroupid =  pasGetSid("ncgroup","groupid"); 
    
    sprintf(caTemp,"insert into ncgroup (groupname,groupid,addtime,pid) values ('%s',%ld,%lu,%lu)",caGroupname,lGroupid,time(0),atol(caGid));
	  utMsgDelVar(psMsgHead,"opmsg");
	  utMsgAdd_s(&psMsgHead,"opmsg",caTemp);
	  ncOperWriteLog(psMsgHead);
    iReturn = pasDbExecSqlF("insert into ncgroup (groupname,groupid,addtime,pid,level) values ('%s',%ld,%lu,%lu,%lu)",caGroupname,lGroupid,time(0),atol(caGid),iLevel);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0837增加部门"),ncLang("0163部门增加出错"));
        pasDbRollback(NULL);
    }
    else  {
        pasDbCommit(NULL);
        ncWebDispDeptForm_new(psShmHead,iFd,psMsgHead);
    }
}

/* 部门删除  */
int ncWebDeptDel_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caGroupid[34];
   	int iReturn;
   	long lCount;
    long lGroupid,lPid;
    char caTemp[1024];
    char caUsername2[32];
    char caGid[16];
    char caLevel[16];
   memset(caUsername2,0,sizeof(caUsername2));
   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
   	iReturn = utMsgGetSomeNVar(psMsgHead,3,
					"groupid",    UT_TYPE_STRING, 31, caGroupid,
					"gid",        UT_TYPE_STRING, 10,caGid,
					"level",  UT_TYPE_STRING,10,caLevel);

    utStrDelSpaces(caGroupid);
    if(utStrIsSpaces(caGroupid)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0471删除部门"),ncLang("0154部门名称名为空"));
        return 0;
    }
    /* 先检查相应部门中的用户是否存在  */
    lGroupid = atol(caGroupid);
    iReturn = pasDbOneRecord("select count(*) from ncuser where groupid = :gid",1,
                             "gid2", UT_TYPE_LONG,lGroupid,
                             UT_TYPE_LONG,sizeof(long),&lCount);
    if(iReturn == 0 && lCount > 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0471删除部门"),ncLang("0164部门中包含有用户，不能删除"));
        return 0;
    }
    if(strlen(caGroupid)>0){
    	 lPid=atol(caGroupid);
    	 printf("select count(*) from ncgroup where pid =%lu\n",lPid);
    	 
    	 iReturn = pasDbOneRecord("select count(*) from ncgroup where pid = :gid",1,
                             "gid", UT_TYPE_LONG,lPid,
                             UT_TYPE_LONG,sizeof(long),&lCount);
	    if(iReturn == 0 && lCount > 0) {
	        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0471删除部门"),ncLang("0164部门中包含有用户，不能删除"));
	        return 0;
	    }
    }
    sprintf(caTemp,"delete  from ncgroup where groupid=%s ",caGroupid);
    ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
    iReturn = pasDbExecSqlF("delete  from ncgroup where groupid=%s ",caGroupid);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0471删除部门"),ncLang("0156部门删除出错"));
        pasDbRollback(NULL);
    }
    else  {
        pasDbCommit(NULL);
        ncWebDispDeptForm_new(psShmHead,iFd,psMsgHead);
    }
}


/* 部门修改  */
int ncWebDeptModi_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caGroupid[34],caName[32];
   	int iReturn;
   	long lCount;
    long lGroupid,lGid;
    char caTemp[4024];
    char caUsername2[32];
    char caGid[16],caLevel[16];
    char caOpt[2048];
    long lGroupid0,group_flag;
    char caGroupflag[8];
   memset(caUsername2,0,sizeof(caUsername2));
   lGroupid0=0;
   group_flag=0;
   iReturn=dsCltGetSessionValue(2,"dispname",UT_TYPE_STRING,30,caUsername2,
                          "groupid", UT_TYPE_LONG,4,&lGroupid0);
   if((iReturn==0)&&(lGroupid0>0)){
   		group_flag=1;
   }
   	iReturn = utMsgGetSomeNVar(psMsgHead,5,
   	      "gid2",    UT_TYPE_STRING, 10, caGid,
					"gid",    UT_TYPE_STRING, 31, caGroupid,
					"newname",UT_TYPE_STRING, 31, caName,
					"level",  UT_TYPE_STRING,10,caLevel,
					"group_flag",UT_TYPE_STRING,1,caGroupflag);
   	if(atol(caGroupflag)==1){
   		group_flag=1;
    }
    utStrDelSpaces(caGroupid);
    if(utStrIsSpaces(caGroupid)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0698修改部门"),ncLang("0154部门名称名为空"));
        return 0;
    }
    /* 先检查相应部门中的用户是否存在  */
    lGroupid = atol(caGroupid);
    sprintf(caTemp,"update ncgroup set groupname='%s',addtime=%lu where groupid=%s ",
                        caName,time(0),caGroupid);
	  utMsgDelVar(psMsgHead,"opmsg");
	  utMsgAdd_s(&psMsgHead,"opmsg",caTemp);
    ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
    iReturn = pasDbExecSqlF("update ncgroup set groupname='%s',addtime=%lu where groupid=%s ",
                        caName,time(0),caGroupid);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0698修改部门"),ncLang("0162部门修改出错"));
        pasDbRollback(NULL);
    }
    else  {
        pasDbCommit(NULL);
        if(group_flag==1)
        utMsgAdd_s(&psMsgHead,"group_flag","1");
        ncWebDispDeptForm_new(psShmHead,iFd,psMsgHead);
    }
}

//显示网址库类别
int ncWebclassSelect(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    long iNum,iReturn;
    long urlid;
    char urlname[64];
    pasDbCursor *psCur;
    psDbHead = utPltInitDb();
    psCur = pasDbOpenSql("select id,name from ncwebclass order by name ",0);
    iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&urlid,
    	                               UT_TYPE_STRING,63,urlname);
    iNum = 0; 	                               
    while((iReturn==0)||(iReturn==1405))
    {
  	
        iNum++;       
        utPltPutLoopVarF(psDbHead,"urlid",   iNum,"%lu",urlid);
        utPltPutLoopVar(psDbHead,"urlname",iNum,urlname);
    iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&urlid,
    	                               UT_TYPE_STRING,63,urlname);
    }
    pasDbCloseCursor(psCur);
     utPltOutToHtml(iFd,psMsgHead,psDbHead,"group/nc_webclass.htm");
      
      return 0;
}







/* 支持无穷级部门的用户树  */

int ncWebTreeDispGroup_nolimit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caLevel[16];
    char caPlate[128];
   	char caGroupname[36],caGroupid[16];
   	
   	char caName[32];
   	char caWhere[256];

   	char caCurPg[16],caTotRec[16];
   	char caPid[16];
   	unsigned long lLevel,lPid;
   	int  iNum;
  	char caFilter[256];
  	char caNo[16];
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
   	int iReturn,i;
   	char caVar[256],caFname[64],caBfname[64];
   	char caValue[256];
   	char caVar1[32];
   	char *p,*pVar;
   	char caOrderby[32];
   	char caGroupname_c[64];
   	char caSql[1024];
   	long lRflag=0;
   	char caGroupids[4048];
    long groupids[100];

    memset(caGroupids,0,sizeof(caGroupids));
#ifdef LDEBUG1 
    utMsgPrintMsg(psMsgHead);
#endif  	
   	iReturn = utMsgGetSomeNVar(psMsgHead,8,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"level",      UT_TYPE_STRING,10,caLevel,
					"groupid",        UT_TYPE_STRING,10,caPid,
					"num",        UT_TYPE_STRING,10,caNo,
					"var",        UT_TYPE_STRING,255,caVar,
					"fname",      UT_TYPE_STRING,63,caFname,
					"bfname",     UT_TYPE_STRING,63,caBfname,
					"groupname",  UT_TYPE_STRING,60,caGroupname_c);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"System Error");
        return 0;
    }
    
    strcpy(caOrderby,utComGetVar_sd(psShmHead,"grouporder","groupid"));
    lGroupid0=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");

    
    
    if(iReturn == 0 && lGroupid0 > 0){
    	
    	strcpy(caGroupids,getGroupid(lGroupid0));
  /*
			sprintf(caGroupids,"%d",lGroupid0);
			
	//取子部门的ID
//printf("select groupid from ncgroup where pid=%s limit 0,99\n",caGroupid2);
//printf("select groupid from ncgroup where pid=%d limit 0,99 \n",lGroupid0);
	   		psCur=pasDbOpenSqlF("select groupid from ncgroup where pid=%d limit 0,99 ",lGroupid0);
	   		lGroupid=0;
	   		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid);
                                                                   
	     iNum=0;
	   
	     while(iReturn==0||iReturn==1405){
	     	sprintf(caGroupids+strlen(caGroupids),",%d",lGroupid);
	      groupids[iNum]=lGroupid;
	   		lGroupid=0;
	   		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid);
	   		iNum++;
	    }
	     pasDbCloseCursor(psCur); 
	     for(i=0;i<iNum;i++){
//	     	printf("select groupid from ncgroup where pid=%d limit 0,99\n ",groupids[i]);
	     		  psCur=pasDbOpenSqlF("select groupid from ncgroup where pid=%d limit 0,99 ",groupids[i]);
			   		lGroupid=0;
			   		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid);
		                                                                   
			    
			   
			     while(iReturn==0||iReturn==1405){
			     	sprintf(caGroupids+strlen(caGroupids),",%d",lGroupid);
			     
			   		lGroupid=0;
			   		iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid);
			   		
			    }
			     pasDbCloseCursor(psCur); 
	     }
	   	*/	
		}
    
    
    
    
    
    
    
    sprintf(caSql,"select groupname,groupid,level,pid from ncgroup where 1=1 ");
   if(strlen(caGroupids)>0) {

   	lRflag=1;
   	
    sprintf(caSql+strlen(caSql)," and groupid in (%s) ",caGroupids);	
//        sprintf(caSql+strlen(caSql),"and (groupid = %lu or pid in (select groupid from ncgroup where groupid=%lu or pid=%lu)  ) ",lGroupid0,lGroupid0,lGroupid0);
    }
    
    
    if(strlen(caGroupname_c)>0){
    	 	lRflag=1;
    	sprintf(caSql+strlen(caSql)," and groupname like '%c%s%c' ",'%',caGroupname_c,'%');
    }
   
  //  sprintf(caWhere+strlen(caWhere)," and level=%d ",atol(caLevel));

//    if(strlen(caPid)>0)
//      sprintf(caWhere+strlen(caWhere)," and pid=%d and level>0",atol(caPid));
//    else{
//    	 sprintf(caWhere+strlen(caWhere)," and pid=0 and level=0",atol(caPid));
//    }
//printf("caSql=%s\n",caSql);
    lRowNum = utComGetVar_ld(psShmHead,"GroupTreeRow",1500);
    lStartRec = (lCurPg - 1) * lRowNum;
    sprintf(caSql+strlen(caSql)," order by %s  limit %lu,%lu",caOrderby,0,lRowNum);
    
//     printf("caSqle=%s\n",caSql);
 //sprintf(caSql+strlen%s  order by %s  limit %lu,%lu",caWhere,caOrderby,0,lRowNum);
    psCur = pasDbOpenSql(caSql,0);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    
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
     }
    
    
    utPltPutVar(psDbHead,"fname",caFname);
    utPltPutVar(psDbHead,"var",caVar);
    utPltPutVar(psDbHead,"bfname",caBfname);
    utPltPutVar(psDbHead,"groupname",caGroupname_c);
    memset(caGroupname,0,sizeof(caGroupname));
    lGroupid=0;
    lLevel=0;
    lPid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,32,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid,
                                   UT_TYPE_LONG,4,&lLevel,
                                   UT_TYPE_LONG,4,&lPid);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caFname)) utPltPutVar(psDbHead,"fname",caFname);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        
  //      if((lLevel==0)&&(lPid==0))
         if(lRflag==1&&lGroupid0==lGroupid){
         	utPltPutLoopVarF(psDbHead,"nodep",iNum,"%d",0);
        }
        else{
          utPltPutLoopVarF(psDbHead,"nodep",iNum,"%d",lPid);
        }
 //        else
 //         utPltPutLoopVarF(psDbHead,"nodep",iNum,"%d",lPid);
        if(lGroupid==0)
        	utPltPutLoopVarF(psDbHead,"nodes",iNum,"M%d",lGroupid);
        else
          utPltPutLoopVarF(psDbHead,"nodes",iNum,"%d",lGroupid);
        
        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum-1);
        utPltPutLoopVar(psDbHead,"groupname", iNum,caGroupname);
        utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%ld",lGroupid);
        utPltPutLoopVarF(psDbHead,"level",iNum,"%d",lLevel);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%d",lPid);
        utPltPutLoopVar(psDbHead,"plate",    iNum,caPlate);
        utPltPutLoopVar(psDbHead,"fname",iNum,caFname);
		    memset(caGroupname,0,sizeof(caGroupname));
		    lGroupid=0;
		    lLevel=0;
		    lPid=0;
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,32,caGroupname,
		                                   UT_TYPE_LONG,4,&lGroupid,
		                                   UT_TYPE_LONG,4,&lLevel,
		                                   UT_TYPE_LONG,4,&lPid);
    }
    pasDbCloseCursor(psCur);
   
    utPltPutVar(psDbHead,"rootname",utComGetVar_sd(psShmHead,"rootname",ncLang("0269公司")));
    
   
    utPltPutVar(psDbHead,"no",caNo);
    utPltPutVar(psDbHead,"level",caLevel);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}


/* 部门修改根名称  */
int ncWebDeptModRootname_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caGroupid[34],caName[32];
   	int iReturn;
   	long lCount;
    long lGroupid,lGid;
    char caTemp[4024];

    long lGroupid0,group_flag;
		char *pConfig;
		pasConfList *psConfig;
    char caRoot[32];
    char caId[20];
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"rootname",UT_TYPE_STRING, 31, caRoot);

   pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0880自动备份设定"),ncLang("0427配置文件出错"));
        return 0;
    }
  
      pasUtlModiConfig(psConfig,"rootname",caRoot);
      
      pasUtlSaveConfig(pConfig,psConfig);
      pasUtlFreeConfig(psConfig);
       ncUtlReReadConFig(psShmHead);
      
        ncWebDispDeptForm_new(psShmHead,iFd,psMsgHead);   
}




/* 网络服务树 */
/*
int ncWebTreeDispService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
   	char caName[32];
    long lId,iReturn,iNum;
    char caId[20];
    utPltDbHead *psDbHead;
#ifdef LDEBUG1 
    utMsgPrintMsg(psMsgHead);
#endif  	
   	
    
    
    psCur = pasDbOpenSqlF("select name,id from ncipindex where 1=1  order by id limit %lu,%lu",0,300);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示网络服务树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();

    utPltPutLoopVar(psDbHead,"nodep",1,"0");
    utPltPutLoopVarF(psDbHead,"nodes",1,"%d",1000);
    utPltPutLoopVarF(psDbHead,"iNum",1,"%d",0);
    utPltPutLoopVar(psDbHead,"name", 1,"IM");
    utPltPutLoopVarF(psDbHead,"id",1,"%ld",1000);
    
    
    utPltPutLoopVar(psDbHead,"nodep",2,"0");
    utPltPutLoopVarF(psDbHead,"nodes",2,"%d",3000);
    utPltPutLoopVarF(psDbHead,"iNum",2,"%d",0);
    utPltPutLoopVar(psDbHead,"name", 2,ncLang("1610敏感服务"));
    utPltPutLoopVarF(psDbHead,"id",2,"%ld",3000);
    
    utPltPutLoopVar(psDbHead,"nodep",3,"0");
    utPltPutLoopVarF(psDbHead,"nodes",3,"%d",4000);
    utPltPutLoopVarF(psDbHead,"iNum",3,"%d",0);
    utPltPutLoopVar(psDbHead,"name", 3,"Mail");
    utPltPutLoopVarF(psDbHead,"id",3,"%ld",4000);
    
    memset(caName,0,sizeof(caName));
    lId=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName,
                                   UT_TYPE_LONG,4,&lId);
    iNum = 3;
    while(iReturn == 0) {
        iNum++;
        
        sprintf(caId,"%d",lId);
        
         if((caId[0]=='2'||caId[0]=='7'||caId[0]=='8'||caId[0]=='9')&&strlen(caId)==4)
           utPltPutLoopVarF(psDbHead,"nodep",iNum,"%c",caId[0]);
         else if(lId==4||lId==5)
           utPltPutLoopVar(psDbHead,"nodep",iNum,"4000");
         else if(lId==6||lId==10||lId==11||(caId[0]=='1'&&strlen(caId)==4))
         	utPltPutLoopVar(psDbHead,"nodep",iNum,"1000");
         else if(caId[0]=='3'&&strlen(caId)==4)
          utPltPutLoopVar(psDbHead,"nodep",iNum,"3000");
         else
           utPltPutLoopVar(psDbHead,"nodep",iNum,"0");
           
        if(lId==0)
        	utPltPutLoopVarF(psDbHead,"nodes",iNum,"M%d",lId);
        else
          utPltPutLoopVarF(psDbHead,"nodes",iNum,"%d",lId);
        
        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum-1);
        utPltPutLoopVar(psDbHead,"name", iNum,caName);
        utPltPutLoopVarF(psDbHead,"id",iNum,"%ld",lId);
       
    memset(caName,0,sizeof(caName));
    lId=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName,
                                   UT_TYPE_LONG,4,&lId);
    }
    pasDbCloseCursor(psCur);
   
  

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"tree/tree_service.htm");
    return 0;
}



/* 网络服务树 */
int ncWebTreeDispService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
   	char caName[32];
    long lId,iReturn,iNum;
    char caId[20];
    struct gservice_s{
    	int id;
    	char name[32];
    };
    struct gservice_s stGservice[20];
    long lSum;
    int i;
    char caPlate[128];
    utPltDbHead *psDbHead;
#ifdef LDEBUG1 
    utMsgPrintMsg(psMsgHead);
#endif  	
   	
    lSum=0;
    
    iReturn = utMsgGetSomeNVar(psMsgHead,1,"plate",UT_TYPE_STRING, 63, caPlate);
    
    
    psCur = pasDbOpenSqlF("select name,groupid from ncservicecgroup where 1=1  order by name limit %lu,%lu",0,20);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示网络服务树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();

    
    memset(caName,0,sizeof(caName));
    lId=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName,
                                   UT_TYPE_LONG,4,&lId);
    
    
    
    
    while(iReturn == 0) {    	
    	  stGservice[lSum].id=lId;
        strcpy(stGservice[lSum].name,caName);   
        
		    utPltPutLoopVarF(psDbHead,"nodep",lSum+1,"0");
		  	utPltPutLoopVarF(psDbHead,"nodes",lSum+1,"%d",lId);
		  	utPltPutLoopVar(psDbHead,"name",lSum+1,caName);
		  	utPltPutLoopVarF(psDbHead,"id",lSum+1,"%d",lId);
        utPltPutLoopVar(psDbHead,"flag",lSum+1,"0");
        lSum++;    
		    memset(caName,0,sizeof(caName));
		    lId=0;
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName,
		                                   UT_TYPE_LONG,4,&lId);
    }
    pasDbCloseCursor(psCur);
 iNum=0;  
  for(i=0;i<lSum;i++){

  	psCur = pasDbOpenSqlF("select name,sid from ncipindex where groupid=%d order by name ",stGservice[i].id);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示网络服务树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
  	lId=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName,
                                   UT_TYPE_LONG,4,&lId); 	
  	while(iReturn == 0) {    	
    	 utPltPutLoopVarF(psDbHead,"nodep",lSum+1+iNum,"M%d",stGservice[i].id);
    	 utPltPutLoopVar(psDbHead,"name",lSum+1+iNum,caName);
    	 utPltPutLoopVarF(psDbHead,"id",lSum+1+iNum,"%d",lId);
       utPltPutLoopVarF(psDbHead,"nodes",lSum+1+iNum,"%d",lId);
       utPltPutLoopVar(psDbHead,"flag",lSum+1+iNum,"1");
		    memset(caName,0,sizeof(caName));
		    lId=0;
		    iNum++;
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName,
		                                   UT_TYPE_LONG,4,&lId);
    }
    pasDbCloseCursor(psCur);
  	
  	
  }
  
  
  
if(strlen(caPlate)==0)
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"tree/tree_service.htm");
else
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    
    
    return 0;
}







/* 移动部门*/
int ncWebDeptMove_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caGroupid[34],caName[32];
   	int iReturn;
   	long lCount,lLevel;
    long lGroupid,lGid;
    char caTemp[4024];
    char caUsername2[32];
    char caGid[16],caLevel[16];
    char caOpt[2048];
    long lGroupid0,group_flag;

    char caNewgid[16],caMgid[16];
   memset(caUsername2,0,sizeof(caUsername2));
   lGroupid0=0;
   group_flag=0;
   iReturn=dsCltGetSessionValue(2,"dispname",UT_TYPE_STRING,30,caUsername2,
                          "groupid", UT_TYPE_LONG,4,&lGroupid0);
  
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
   	      "mnewgid",    UT_TYPE_STRING, 10, caNewgid,
					"mgid",    UT_TYPE_STRING, 10, caMgid);
	
   sprintf(caTemp,"select level from ncgroup where groupid=%d ",atol(caNewgid));
   lLevel=0;
   pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lLevel);
   if(atol(caNewgid)>0){
   	lLevel=lLevel+1;
   }
    sprintf(caTemp,"update ncgroup set pid=%d where groupid=%d ",atol(caNewgid),atol(caMgid));
	  utMsgDelVar(psMsgHead,"opmsg");
	  utMsgAdd_s(&psMsgHead,"opmsg",caTemp);
    ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
    if(atol(caNewgid)==0)
       iReturn = pasDbExecSqlF("update ncgroup set pid=%d,level=0 where groupid=%d ",atol(caNewgid),atol(caMgid));
    else
      iReturn = pasDbExecSqlF("update ncgroup set pid=%d,level=%d where groupid=%d ",atol(caNewgid),lLevel,atol(caMgid));
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0698修改部门"),ncLang("0162部门修改出错"));
        pasDbRollback(NULL);
    }
    else  {
        pasDbCommit(NULL);
      
        ncWebDispDeptForm_new(psShmHead,iFd,psMsgHead);
    }
}