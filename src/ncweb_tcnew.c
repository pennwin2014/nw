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


/* 接口参数定义树 */

/**********************************************************************/
int ncWebTcIntfaceTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    char caName[64];
    unsigned long lSid;
    int iReturn,iNum,i;
    utPltDbHead *psDbHead;
 	  pasDbCursor *psCur;
 	   long lGroupid0=0;
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }

    psCur = pasDbOpenSqlF("select cid,name from ncnsaconnect order by name");
    if(psCur == NULL) {
      
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0748用户带宽设置"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
   
	  lSid=0;
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,  4,&lSid,
      	                           UT_TYPE_STRING,15,caName);
      	                         
    iNum = 0;
    while((iReturn == 0)||(iReturn==1405)) {
    utPltPutLoopVarF(psDbHead,"sid",iNum+1,"%d",lSid);
    utPltPutLoopVar(psDbHead,"dispname",iNum+1,caName);
    iNum++;  
	  lSid=0;
    memset(caName,0,sizeof(caName));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,  4,&lSid,
      	                           UT_TYPE_STRING,15,caName);
    }
    pasDbCloseCursor(psCur);
   
	       
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_intfacetree.htm");
    return 0;
}

//显示接口界面
int ncWebTcDispInterface(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    long i,lSum;
    char caName[20];
    char caCid[16];
    unsigned long lCid,lIndev,lOutdev,lStatus,lOpt,lBandrate,lUprate;
   	int iReturn;
   	   long lGroupid0=0;
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
   	
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
   	                "cid", UT_TYPE_STRING, 10, caCid);
   	              
    lCid=atol(caCid);
        iReturn = pasDbOneRecord("select name,indev,outdev,status,opt,bandrate,uprate from ncnsaconnect where cid = :cid",1,
                            "cid",UT_TYPE_LONG,lCid,
                            UT_TYPE_STRING,15,caName,
                            UT_TYPE_LONG,4,&lIndev,
                            UT_TYPE_LONG,4,&lOutdev,
                            UT_TYPE_LONG,4,&lStatus,
                            UT_TYPE_LONG,4,&lOpt,
                            UT_TYPE_LONG,4,&lBandrate,
                            UT_TYPE_LONG,4,&lUprate);
                           
        if(iReturn != 0 && iReturn != PAS_DB_NULLVALUE) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0703修改通道"),ncLang("0572通道不存在 %d"),iReturn);
            return 0;
        }
        else {
    		psDbHead = utPltInitDb();
    	
            utPltPutVar(psDbHead,"cname",caName);
            utPltPutVarF(psDbHead,"indev","%d",lIndev);
            utPltPutVarF(psDbHead,"outdev","%d",lOutdev);
            utPltPutVarF(psDbHead,"status","%d",lStatus);
            utPltPutVarF(psDbHead,"opt","%d",lOpt);
            utPltPutVarF(psDbHead,"bandrate","%d",lBandrate);
            utPltPutVarF(psDbHead,"uprate","%d",lUprate);
            utPltPutVar(psDbHead,"cid",caCid);
        
        }
   
    lSum=atol(utComGetVar_sd(psShmHead,"MaxNsaDev","8"));
    for(i=0;i<lSum;i++){
    	utPltPutLoopVarF(psDbHead,"i",i+1,"%d",i);
    }


    utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_intfaceset.htm");
    return 0;
}


/* 修改接口参数 */
int ncWebTcInterfaceSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caName[20];
    char caCname[16];
    char caCid[16],caStatus[12],caIndev[12],caOutdev[12],caBandrate[12],caOpt[12],caUprate[12];
    char caApp[12];
   	char caTemp[1024];
    unsigned long lCid,lStatus,lIndev,lOutdev,lBandrate,lOpt,lCount,lUprate;
   	int iReturn;
//   	utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,9,
   	                "cid",    UT_TYPE_STRING, 10, caCid,
   	                "cname", UT_TYPE_STRING, 15, caCname,
   	                "status",UT_TYPE_STRING,2,caStatus,
   	                "indev",UT_TYPE_STRING,8,caIndev,
   	                "outdev",UT_TYPE_STRING,8,caOutdev,
   	                "bandrate",UT_TYPE_STRING,10,caBandrate,
   	                "uprate",  UT_TYPE_STRING,10,caUprate,
   	                "opt",     UT_TYPE_STRING, 10, caOpt,
   	                "app",      UT_TYPE_STRING, 11, caApp);
    lCid=atol(caCid);
    lStatus=atol(caStatus);
    lIndev=atol(caIndev);
    lOutdev=atol(caOutdev);
    lBandrate=atol(caBandrate);
    lOpt=atol(caOpt);
    lUprate=atol(caUprate);
   lCount=0;
   if(lStatus==1){
   	sprintf(caTemp,"select count(*) from ncnsaconnect where (indev=%d or outdev=%d or indev=%d or outdev=%d) and status=1 and cid!=%d ",lIndev,lIndev,lOutdev,lOutdev,lCid);
   	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
   	if(lCount>0){
   			utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("1467接口不能重复使用"),ncLang("1467接口不能重复使用"));
   			return 0;
   	}
   
  }
 
  iReturn = pasDbExecSqlF("update ncnsaconnect set name='%s',indev=%lu,outdev=%lu,status=%lu,opt=%lu,bandrate=%lu,lasttime=%d,uprate=%d where cid=%d ",
   caCname,lIndev,lOutdev,lStatus,lOpt,lBandrate,time(0),lUprate,lCid);
                    
            if(iReturn != 0) {
 		  		utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("1468修改接口参数"),ncLang("0563数据修改不正确 %s"),pasDbErrorMsg(NULL));
 				 pasDbRollback(NULL);
        		return 0;
        	}
   		    else{
   		    	pasDbCommit(NULL);
   		    }
   			
	if(!utStrIsSpaces(caApp)) {
		ncNsaCvtDbConnectToFile("/home/ncmysql/nc/bin/nsa.conf");

 

  //      if(strcasecmp(utComGetVar_sd(psShmHead,"BandWidthLimit","YES"),"YES") == 0) {
  //         	ncUtlTcWidthInit(psShmHead);
        }
	ncWebTcDispInterface(psShmHead,iFd,psMsgHead);
	return 0;
}



//带宽通道设置
int ncWebTcTdNew(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
    char *p;
    char caVar[32];
    int i=0;
   	long lId;
   	char caName[64];
   	char caPlate[128];
   	char caGid[16],caDid[16],caAdd[16],caDel[16];
	char caNname[32],caNid[16],caModi[16];
	char caTotPg[16],caCurPg[16],caCount[16];
	long lRowNum,lStartRec,lCurPg;
	long lCount,lTid,lTotPg;
	char caTemp[1024];
  char caQosname[32],caQtype[16],caBandrate[16],caQosid[16];
  long lQosmax,lQosmin,lLasttime,lQosid;
 	pasDbCursor *psCur;
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebTcTdNew");
 	}

	iReturn = utMsgGetSomeNVar(psMsgHead,10,
	                "qosname",  UT_TYPE_STRING,31,caQosname,
	                "qtype",    UT_TYPE_STRING,12,caQtype,
	                "bandrate",UT_TYPE_STRING,10,caBandrate,
	                "add",    UT_TYPE_STRING,12,caAdd,
	                "del",    UT_TYPE_STRING,12,caDel,
	                "modi",   UT_TYPE_STRING,12,caModi,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			            "CurPg",      UT_TYPE_STRING, 12, caCurPg,
   	              "qosid",     UT_TYPE_STRING, 10, caQosid
	                );
	                
   lCurPg = atol(caCurPg);
  
    if(lCurPg <= 0) lCurPg = 1;

    if(!utStrIsSpaces(caAdd)) { /* 增加一个组  */
        if(!utStrIsSpaces(caQosname)) {
        	sprintf(caTemp,"select count(*) from  ncnsaqos where qosname='%s'  ",caQosname);
        	lCount=0;
        	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        	if(lCount>0)
        	{
        	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("1469该名称已存在"),ncLang("1469该名称已存在"));
        	  return 0;
        	}
        	if(atol(caQtype)==1){
        		lQosmax=0;
        		lQosmin=atol(caBandrate);
        		if(lQosmin<=0) lQosmin=1024;
        	}
        	else{
        		lQosmin=0;
        		lQosmax=atol(caBandrate);
        		if(lQosmax<=0) lQosmax=1024;
        	}
            iReturn = pasDbExecSqlF("insert into ncnsaqos(qosname,qosmin,qosmax,lasttime) values ('%s',%d,%d,%d)",
                                             caQosname,lQosmin,lQosmax,time(0));

            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0251分类列表"),ncLang("0310记录增加出错"));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                pasDbCommit(NULL);
            }                
        }
        else {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0251分类列表"),ncLang("0362类别名称为空"));
            return 0;
       }            
            
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除一个组  */
    for(i=0;i<29;i++){
    	sprintf(caVar,"chk%d",i+1);
    	p=utMsgGetVar_s(psMsgHead,caVar);
    	if(p)
    	{
 	       iReturn = pasDbExecSqlF("delete from ncnsaqos where qosid= %s",p);
	        if(iReturn != 0) {
	            pasDbRollback(NULL);
	        }                
	        else {

	          
	            pasDbCommit(NULL);
	        } 
	}
	}           
    }
    else if(!utStrIsSpaces(caModi) && !utStrIsSpaces(caQosname)) { /* 修改一个组  */
       
          if(atol(caQtype)==1){
        		lQosmax=0;
        		lQosmin=atol(caBandrate);
        		if(lQosmin<=0) lQosmin=1024;
        	}
        	else{
        		lQosmin=0;
        		lQosmax=atol(caBandrate);
        		if(lQosmax<=0) lQosmax=1024;
        	}
         lQosid=atol(caQosid);
    
        iReturn = pasDbExecSqlF("update ncnsaqos set qosname='%s',qosmin=%d,qosmax=%d,lasttime=%d where qosid = %d",
                         caQosname,lQosmin,lQosmax,time(0),lQosid);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0700修改出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }                                     

        pasDbCommit(NULL);
    }
    
      lRowNum = 29;
      lStartRec = (lCurPg - 1) * lRowNum;    
      psDbHead = utPltInitDb();
      
      
      sprintf(caTemp,"select count(*) from ncnsaqos order by qosname ");
      iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213定义邮件分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
    
      psCur=pasDbOpenSqlF("select qosid,qosname,qosmin,qosmax,lasttime from ncnsaqos  order by qosname desc limit %d,%d ",lStartRec,lRowNum);
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
     lQosid=0;
     memset(caQosname,0,sizeof(caQosname));
     lQosmin=0;
     lQosmax=0;
     lLasttime=0;
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lQosid,
      				   UT_TYPE_STRING,31,caQosname,
      				   UT_TYPE_LONG,4,&lQosmin,
      				   UT_TYPE_LONG,4,&lQosmax,
      				   UT_TYPE_LONG,4,&lLasttime);
      iNum=0;
      while(iReturn==0&&iNum<lRowNum){
      	iNum++;
	      utPltPutLoopVarF(psDbHead,"iNum",iNum,"%lu",iNum);
	
      	utPltPutLoopVarF(psDbHead,"qosid",iNum,"%lu",lQosid);
      	utPltPutLoopVar(psDbHead,"qosname",iNum,caQosname);
        utPltPutLoopVar(psDbHead,"lasttime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lLasttime));

      	if((lQosmin==0)&&(lQosmax>0)){
      		utPltPutLoopVar(psDbHead,"qostype",iNum,ncLang("1470限制带宽"));
      		utPltPutLoopVarF(psDbHead,"qosband",iNum,"%d",lQosmax);
      	}
      	else if((lQosmin>0)&&(lQosmax==0)){
      		utPltPutLoopVar(psDbHead,"qostype",iNum,ncLang("1471保证带宽"));
      		utPltPutLoopVarF(psDbHead,"qosband",iNum,"%d",lQosmin);
      	}
     lQosid=0;
     memset(caQosname,0,sizeof(caQosname));
     lQosmin=0;
     lQosmax=0;
     lLasttime=0;
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lQosid,
      				   UT_TYPE_STRING,31,caQosname,
      				   UT_TYPE_LONG,4,&lQosmin,
      				   UT_TYPE_LONG,4,&lQosmax,
      				   UT_TYPE_LONG,4,&lLasttime);
  				}
       lTotPg = (lCount - 1) / lRowNum + 1;
       pasDbCloseCursor(psCur);
       utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
       utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
       utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
 //  printf("end\n"); 
 
   i=0;
    while(iNum < lRowNum) {
        i++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%ld",iNum);
        iNum++;
    }
 
    			 
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_tdset.htm");
    return 0;
}



/* 带宽策略*/
int ncTcNewProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lAid;
   	char caAclname[64];
   	char caPlate[128];
 	  pasDbCursor *psCur;

    psCur = pasDbOpenSqlF("select aid,aclname from ncnsaacl");
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("1472表ncnsaacl不存在"));
        return 0;
    }
    psDbHead = utPltInitDb();
    lAid=0;
    memset(caAclname,0,sizeof(caAclname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lAid,
  	                               UT_TYPE_STRING,31,caAclname);
   	iNum = 0;
   	while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%u",iNum);
        utPltPutLoopVarF(psDbHead,"id",   iNum,"%u",lAid);
        utPltPutLoopVarF(psDbHead,"name", iNum,"%s",caAclname);
		    lAid=0;
		    memset(caAclname,0,sizeof(caAclname));
		    iReturn = pasDbFetchInto(psCur,
		      	                           UT_TYPE_LONG,4,&lAid,
		  	                               UT_TYPE_STRING,31,caAclname);
    }
    pasDbCloseCursor(psCur);
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_procy.htm");
    return 0;
}

//带宽策略的保存
int ncTcNewProcySave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId,i,lSid;
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
  
            
        if(!utStrIsSpaces(caName)) {
            iReturn = pasDbExecSqlF("insert into ncnsaacl(aclname,lasttime) values ('%s',%d)",
                                             caName,time(0));
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

				        iReturn = pasDbExecSqlF("delete from ncnsaacl where aid= %lu",lSid);
				        if(iReturn != 0) {
				            pasDbRollback(NULL);
				        }                
				        else {
				         iReturn = pasDbExecSqlF("delete from ncnsaacllist where aid= %lu",lSid);
				            pasDbCommit(NULL);
				        }            
   	            

   	        }        
   	    }
    	
             
    }
    else if(!utStrIsSpaces(caModi) && !utStrIsSpaces(caName)) { /* 修改一个组  */
        iReturn = pasDbExecSqlF("update ncnsaacl set aclname='%s',lasttime=%d where aid = %s",
                         caName,time(0),caId);
        pasDbCommit(NULL);
    }
    return ncTcNewProcy(psShmHead,iFd,psMsgHead);
    return 0;
}   



 //带宽策略明细设置
int ncWebTcProcyDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
    char *p;
    char caVar[32];
    int i=0;
   	long lId;
   	char caName[64];
   	char caPlate[128];
   	char caGid[16],caDid[16],caAdd[16],caDel[16];
	char caNname[32],caNid[16],caModi[16];
	char caTotPg[16],caCurPg[16],caCount[16];
	long lRowNum,lStartRec,lCurPg;
	long lCount,lTid,lTotPg;
	char caTemp[1024];
  char caQosid[16];
  long lLasttime,lQosid;
  long lSid,lNum,lProt,lDtype,lQosway;
  long lSum,lSum1;
  struct servdate_s{
  	int id;
  	char name[32];
  } servdate[1000];
  struct cservdate_s{
  	int id;
  	char name[32];
  } cservdate[200];
  long lSersum,lCsersum,id;
  char name[32];
  char caServiceid[16],caCid[16];
  char caDip[256],caDport[64],caAclname[32],caAid[16],caNum[16],caProt[16],caDtype[16],caQosway[16],caSid[12],caQosname[32];
 	pasDbCursor *psCur;
 	char caCont[256];
 	long lMax;
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebTcProcyDet");
 	}
 
//网络服务
      psCur=pasDbOpenSqlF("select id,name from ncipindex order by id limit 0,999");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
		  lSersum=0;
		  id=0;
		  memset(name,0,sizeof(name));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);
                                  
      				 
      iNum=0;
      while(iReturn==0){
      	servdate[lSersum].id=id;
      	strcpy(servdate[lSersum].name,name);
      	lSersum++;
	     
       id=0;
		   memset(name,0,sizeof(name));
       iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);

  		}
     
       pasDbCloseCursor(psCur);


//网络服务类别
      psCur=pasDbOpenSqlF("select tid,sname from ncservicetype limit 0,199");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
		  lCsersum=0;
		  id=0;
		  memset(name,0,sizeof(name));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);
                                  
      				 
      iNum=0;
      while(iReturn==0){
      	cservdate[lCsersum].id=id;
      	strcpy(cservdate[lCsersum].name,name);
      	lCsersum++;
	     
       id=0;
		   memset(name,0,sizeof(name));
       iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);

  		}
     
       pasDbCloseCursor(psCur);





	iReturn = utMsgGetSomeNVar(psMsgHead,16,
	                "aid",  UT_TYPE_STRING,10,caAid,
	                "num",    UT_TYPE_STRING,10,caNum,
	                "dip",UT_TYPE_STRING,254,caDip,
	                "prot",UT_TYPE_STRING,10,caProt,
	                "dtype",UT_TYPE_STRING,10,caDtype,
	                "dport",UT_TYPE_STRING,63,caDport,
	                "serviceid",UT_TYPE_STRING,10,caServiceid,
	                "cid",UT_TYPE_STRING,10,caCid,
	                "qosway",UT_TYPE_STRING,8,caQosway,
	                "qosid", UT_TYPE_STRING,10,caQosid,
	                "add",    UT_TYPE_STRING,12,caAdd,
	                "del",    UT_TYPE_STRING,12,caDel,
	                "modi",   UT_TYPE_STRING,12,caModi,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			            "CurPg",      UT_TYPE_STRING, 12, caCurPg,
			            "sid",        UT_TYPE_STRING,10,caSid);
   	             
	                
   lCurPg = atol(caCurPg); 
    if(lCurPg <= 0) lCurPg = 1;

 


  if(utStrIsSpaces(caAid)){
  	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("1473请先选择策略，然后添加策略明细"),ncLang("1473请先选择策略，然后添加策略明细"));
    return 0;
  }
  
  sprintf(caTemp,"select aclname from ncnsaacl where aid=%s ",caAid);
  pasDbOneRecord(caTemp,0,UT_TYPE_STRING,31,caAclname);
  

  
    lSum=1;
    if(atol(caDtype)==0){
    	strcpy(caCont,caDport);
    }
    else if(atol(caDtype)==1){
    	strcpy(caCont,caServiceid);
    }
    else if(atol(caDtype)==2){
    	strcpy(caCont,caCid);
    }
    else{
    	strcpy(caCont,caDport);
    }
    if(!utStrIsSpaces(caAdd)) { /* 增加  */
    	lSum=1;
        if(!utStrIsSpaces(caDip)) {
          lSum=utStrCountChar(caDip,',')+1;
 //         printf("lSum=%d\n",lSum);
        }
        if(atol(caDtype)==0){
        	 lSum1=1;
          	lSum1=utStrCountChar(caDport,',')+1;
 //         	printf("lSum1=%d\n",lSum1);
          	lSum=lSum*lSum1;
          }
       
       sprintf(caTemp,"insert into ncnsaacllist(aid,num,dip,prot,dtype,dport,qosway,qosid,aclcount,lasttime) values (%d,%d,'%s',%d,%d,'%s',%d,%d,%d,%d)",
                                             atol(caAid),atol(caNum),caDip,atol(caProt),atol(caDtype),caCont,atol(caQosway),atol(caQosid),lSum,time(0));
 //      printf("caTemp=%s\n",caTemp);
          
          iReturn = pasDbExecSqlF("insert into ncnsaacllist(aid,num,dip,prot,dtype,dport,qosway,qosid,aclcount,lasttime) values (%d,%d,'%s',%d,%d,'%s',%d,%d,%d,%d)",
                                             atol(caAid),atol(caNum),caDip,atol(caProt),atol(caDtype),caCont,atol(caQosway),atol(caQosid),lSum,time(0));

            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0251分类列表"),ncLang("0310记录增加出错"));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                pasDbCommit(NULL);
            }                
          
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除一个组  */
    for(i=0;i<29;i++){
    	sprintf(caVar,"chk%d",i+1);
    	p=utMsgGetVar_s(psMsgHead,caVar);
    	if(p)
    	{
 	       iReturn = pasDbExecSqlF("delete from ncnsaacllist where sid= %s",p);
	        if(iReturn != 0) {
	            pasDbRollback(NULL);
	        }                
	        else {

	          
	            pasDbCommit(NULL);
	        } 
	}
	}           
    }
    else if(!utStrIsSpaces(caModi) && !utStrIsSpaces(caSid)) { /* 修改一个组  */
    	  lSum=1;
        if(!utStrIsSpaces(caDip)) {
          lSum=utStrCountChar(caDip,',')+1;
        }
        if(atol(caDtype)==0){
        	lSum1=1;
          	lSum1=utStrCountChar(caDport,',')+1;
          	lSum=lSum*lSum1;
          }
       
    
    
      
        iReturn = pasDbExecSqlF("update ncnsaacllist set num=%d,dip='%s',prot=%d,dtype=%d,dport='%s',qosway=%d,qosid=%d,aclcount=%d,lasttime=%d where sid = %d",
                         atol(caNum),caDip,atol(caProt),atol(caDtype),caCont,atol(caQosway),atol(caQosid),lSum,time(0),atol(caSid));
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0700修改出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }                                     

        pasDbCommit(NULL);
    }
    
      lRowNum = 25;
      lStartRec = (lCurPg - 1) * lRowNum;    
      psDbHead = utPltInitDb();
      
      utPltPutVar(psDbHead,"aclname",caAclname);
      utPltPutVar(psDbHead,"aid",caAid);
      
      
		  lMax=0;
		  sprintf(caTemp,"select max(num)+1 from ncnsaacllist where aid=%s ",caAid);
		  pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lMax);
		  if(lMax==0) lMax=1;
      
      
      utPltPutVarF(psDbHead,"num","%d",lMax);
      sprintf(caTemp,"select count(*) from ncnsaacllist a,ncnsaacl d where a.aid=d.aid and a.aid=%s  ",caAid);
      iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213定义邮件分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
 
 //sprintf(caTemp,"select a.sid,a.num,a.dip,a.prot,a.dtype,a.dport,a.qosway,a.qosid,b.qosname,d.aclname from ncnsaacllist a,ncnsaacl d  left join ncnsaqos b on (a.qosid=b.qosid) where a.aid=d.aid and a.aid=%s order by a.num limit %d,%d \n",caAid,lStartRec,lRowNum);  
// printf("caTemp=%s\n",caTemp); 
      psCur=pasDbOpenSqlF("select a.sid,a.num,a.dip,a.prot,a.dtype,a.dport,a.qosway,a.qosid,b.qosname,d.aclname from ncnsaacllist a,ncnsaacl d  left join ncnsaqos b on (a.qosid=b.qosid) where a.aid=d.aid and a.aid=%s order by a.num limit %d,%d ",caAid,lStartRec,lRowNum);
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
			lSid=0;
			lNum=0;
			memset(caDip,0,sizeof(caDip));
			lProt=0;
			lDtype=0;
			memset(caDport,0,sizeof(caDport));
			lQosway=0;
			lQosid=0;
			memset(caQosname,0,sizeof(caQosname));
			memset(caAclname,0,sizeof(caAclname));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                   UT_TYPE_LONG,4,&lNum,
                                   UT_TYPE_STRING,254,caDip,
                                   UT_TYPE_LONG,4,&lProt,
                                   UT_TYPE_LONG,4,&lDtype,
                                   UT_TYPE_STRING,63,caDport,
                                   UT_TYPE_LONG,4,&lQosway,
                                   UT_TYPE_LONG,4,&lQosid,
                                   UT_TYPE_STRING,31,caQosname,
                                   UT_TYPE_STRING,31,caAclname);
   		 
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
      	utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
	      utPltPutLoopVarF(psDbHead,"iNum",iNum,"%lu",iNum);	
      	utPltPutLoopVarF(psDbHead,"qosid",iNum,"%lu",lQosid);
      	utPltPutLoopVar(psDbHead,"qosname",iNum,caQosname);
        utPltPutLoopVarF(psDbHead,"pri",iNum,"%d",lNum);
        utPltPutLoopVar(psDbHead,"dip",iNum,caDip);
        utPltPutLoopVarF(psDbHead,"prot",iNum,"%d",lProt);
        if(lProt==6)
          utPltPutLoopVar(psDbHead,"prot_desc",iNum,"TCP");
        else if(lProt==17)
          utPltPutLoopVar(psDbHead,"prot_desc",iNum,"UDP");
        else if(lProt==0)
          utPltPutLoopVar(psDbHead,"prot_desc",iNum,ncLang("1474任意"));
          
         
        utPltPutLoopVarF(psDbHead,"dtype",iNum,"%d",lDtype);
        utPltPutLoopVar(psDbHead,"port",iNum,caDport);
        utPltPutLoopVarF(psDbHead,"qosway",iNum,"%d",lQosway);
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%d",lSid);
        if(lQosid>0)
            utPltPutLoopVar(psDbHead,"qosname1",iNum,caQosname);
        else
            utPltPutLoopVar(psDbHead,"qosname1",iNum,ncLang("1475不限"));
         
        utPltPutLoopVarF(psDbHead,"dtype",iNum,"%d",lDtype);
        utPltPutLoopVarF(psDbHead,"qosway",iNum,"%d",lQosway);
           
        if(lDtype==0)
            utPltPutLoopVar(psDbHead,"type_desc",iNum,ncLang("1615端口"));
        else if(lDtype==1)
            utPltPutLoopVar(psDbHead,"type_desc",iNum,ncLang("1647网络服务"));
        else if(lDtype==2)
            utPltPutLoopVar(psDbHead,"type_desc",iNum,ncLang("1476服务类别"));
        else if(lDtype==3)
           utPltPutLoopVar(psDbHead,"type_desc",iNum,ncLang("1477网址库类别"));
               
        if(lQosway==0)
           utPltPutLoopVar(psDbHead,"qosway_desc",iNum,ncLang("0331禁用"));
        else if(lQosway==1)
           utPltPutLoopVar(psDbHead,"qosway_desc",iNum,ncLang("1478共享"));
        else if(lQosway==2)
           utPltPutLoopVar(psDbHead,"qosway_desc",iNum,ncLang("1479部门独用(在一个部门内共享)"));
        else if(lQosway==3)
           utPltPutLoopVar(psDbHead,"qosway_desc",iNum,ncLang("1480用户独用")); 
            
        utPltPutLoopVar(psDbHead,"aclname",iNum,caAclname);

      
      if(lDtype==0){
        strcpy(caCont,caDport);
      }
      else if(lDtype==1){
      	memset(caCont,0,sizeof(caCont));
      	for(i=0;i<lSersum;i++){
      		if(servdate[i].id==atol(caDport)){
      			strcpy(caCont,servdate[i].name);
      			break;
      		}
      	}
      }
      else if(lDtype==2){
      	memset(caCont,0,sizeof(caCont));
      	for(i=0;i<lCsersum;i++){
      		if(cservdate[i].id==atol(caDport)){
      			strcpy(caCont,cservdate[i].name);
      			break;
      		}
      	}
      }
      else{
      	strcpy(caCont,caDport);
      }
      utPltPutLoopVar(psDbHead,"cont",iNum,caCont);
      
			lSid=0;
			lNum=0;
			memset(caDip,0,sizeof(caDip));
			lProt=0;
			lDtype=0;
			memset(caDport,0,sizeof(caDport));
			lQosway=0;
			lQosid=0;
			memset(caQosname,0,sizeof(caQosname));
			memset(caAclname,0,sizeof(caAclname));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                   UT_TYPE_LONG,4,&lNum,
                                   UT_TYPE_STRING,254,caDip,
                                   UT_TYPE_LONG,4,&lProt,
                                   UT_TYPE_LONG,4,&lDtype,
                                   UT_TYPE_STRING,63,caDport,
                                   UT_TYPE_LONG,4,&lQosway,
                                   UT_TYPE_LONG,4,&lQosid,
                                   UT_TYPE_STRING,31,caQosname,
                                   UT_TYPE_STRING,31,caAclname);
  				}
       lTotPg = (lCount - 1) / lRowNum + 1;
       pasDbCloseCursor(psCur);
       utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
       utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
       utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
 //  printf("end\n"); 
 
   i=0;
    while(iNum < lRowNum) {
        i++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%ld",iNum);
        iNum++;
    }
 
 
 for(i=0;i<lSersum;i++){
 	utPltPutLoopVarF(psDbHead,"id",i+1,"%d",servdate[i].id);
 	utPltPutLoopVar(psDbHead,"sname",i+1,servdate[i].name);
}
for(i=0;i<lCsersum;i++){
	utPltPutLoopVarF(psDbHead,"cid",i+1,"%d",cservdate[i].id);
	utPltPutLoopVar(psDbHead,"cname",i+1,cservdate[i].name);
}
 
 //输出带宽通道
 i=0;
      psCur=pasDbOpenSqlF("select qosid,qosname from ncnsaqos limit 0,1000");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
		  
		  id=0;
		  memset(name,0,sizeof(name));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);
                                  
      				 
      i=0;
      while(iReturn==0){
        utPltPutLoopVarF(psDbHead,"qosid2",i+1,"%d",id);
        utPltPutLoopVar(psDbHead,"qosname2",i+1,name);
      	i++;
	     
       id=0;
		   memset(name,0,sizeof(name));
       iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);

  		}
     
       pasDbCloseCursor(psCur);
 
 
    			 
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_procydet.htm");
    return 0;
}

int utStrSepChar(char *pIn,char pOut[100][16])
{
    char *p;
    char *p1;
    int iCount;
    p = pIn;
  
    iCount=1;
    while(p) {
    	p1=strstr(p,",");
    	if(p1){
    		if(iCount>98) break;
	    	memset(pOut[iCount-1],0,sizeof(pOut[iCount-1]));
	    	strncpy(pOut[iCount-1],p,p1-p);
	    	iCount++;
	    	p=p1+1;
	    }
	    else{
	    	break;
	    }
  	   
    }
    strcpy(pOut[iCount-1],p);
    
    return iCount;
}

 //使用者带宽设置
int ncWebTcUserProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
    char *p;
    char caVar[32];
    int i=0;
   	long lId;
   	char caName[64];
   	char caPlate[128];
   	char caGid[16],caDid[16],caAdd[16],caDel[16];
	char caNname[32],caNid[16],caModi[16];
	char caTotPg[16],caCurPg[16],caCount[16];
	long lRowNum,lStartRec,lCurPg;
	long lCount,lTid,lTotPg;
	char caTemp[1024];
  char caQosid[16];
  long lLasttime,lQosid;
  long lSid,lNum,lProt,lDtype,lQosway,id;
  long lSum,lSum1;

  char name[32];
  long *pIn;
  char caAid[16],caSid[12],caSip[20],caEip[20];
 	pasDbCursor *psCur;
 	char caCont[256];
 	char caCheID[1024];
 	char caUtype[16],caGroupid[16],caAclname[32];
 	char pOut[100][16];
 	unsigned long lDid,lUtype,lUid,lEip,lAid;
 	char caStime[64],caName_d[32];
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncWebTcUserProcy");
 	}
 



	iReturn = utMsgGetSomeNVar(psMsgHead,15,
	                "aid",  UT_TYPE_STRING,10,caAid,
	                "did",    UT_TYPE_STRING,10,caDid,
	                "stime",UT_TYPE_STRING,63,caStime,
	                "utype",UT_TYPE_STRING,8,caUtype,
	                "sip",UT_TYPE_STRING,16,caSip,
	                "eip",UT_TYPE_STRING,16,caEip,
	                "groupid",UT_TYPE_STRING,10,caGroupid,
	                "cheID",UT_TYPE_STRING,1000,caCheID,
	                "aid",UT_TYPE_STRING,10,caAid,
	                "add",    UT_TYPE_STRING,12,caAdd,
	                "del",    UT_TYPE_STRING,12,caDel,
	                "modi",   UT_TYPE_STRING,12,caModi,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			            "CurPg",      UT_TYPE_STRING, 12, caCurPg,
			            "sid",        UT_TYPE_STRING,10,caSid);
  
  
 // printf("cheID=%s\n",caCheID); 	             
	                
   lCurPg = atol(caCurPg); 
    if(lCurPg <= 0) lCurPg = 1;

 lUid=0;
 lEip=0;
    if(!utStrIsSpaces(caAdd)) { /* 增加  */
    	
    	if(atol(caUtype)==0){
    		lUid=0;
    		lEip=0;
    	}
    	else if(atol(caUtype)==1){
    		lUid=atol(caGroupid);
    		lEip=0;
    	}
    	else if(atol(caUtype)==3){
    		 if(utStrIsValidIp(caSip) && utStrIsValidIp(caEip)) {
            
             lUid = ntohl(utComHostAddress(caSip));            
             lEip = ntohl(utComHostAddress(caEip)); 
    		
    	}
     }
     else if(atol(caUtype)==2){
     	//用户带宽分配
     	if(strlen(caCheID)>0){
     	   lSum=utStrSepChar(caCheID,pOut);
     	   for(i=0;i<lSum;i++){
     	   	lUid=atol(pOut[i]);
     	   	lEip=0;
     	   	 iReturn = pasDbExecSqlF("insert into ncnsauser(did,stime,utype,uid,eip,aid,lasttime) values (%d,'%s',%d,%lu,%lu,%lu,%lu)",
                                             atol(caDid),caStime,atol(caUtype),lUid,lEip,atol(caAid),time(0));
     	   }
       }
     	
    }
    if(atol(caUtype)!=2){
          iReturn = pasDbExecSqlF("insert into ncnsauser(did,stime,utype,uid,eip,aid,lasttime) values (%d,'%s',%d,%lu,%lu,%lu,%lu)",
                                             atol(caDid),caStime,atol(caUtype),lUid,lEip,atol(caAid),time(0));

            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0251分类列表"),ncLang("0310记录增加出错"));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                pasDbCommit(NULL);
            }  
          }              
          
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除一个组  */
    for(i=0;i<27;i++){
    	sprintf(caVar,"chk%d",i+1);
    	p=utMsgGetVar_s(psMsgHead,caVar);
    	if(p)
    	{
 	       iReturn = pasDbExecSqlF("delete from ncnsauser where sid= %s",p);
	        if(iReturn != 0) {
	            pasDbRollback(NULL);
	        }                
	        else {

	          
	            pasDbCommit(NULL);
	        } 
	}
	}           
    }
    else if(!utStrIsSpaces(caModi) && !utStrIsSpaces(caSid)) { /* 修改一个组  */
    	 
      if(atol(caUtype)==0){
    		lUid=0;
    		lEip=0;
    	}
    	else if(atol(caUtype)==1){
    		lUid=atol(caGroupid);
    		lEip=0;
    	}
    	else if(atol(caUtype)==3){
    		 if(utStrIsValidIp(caSip) && utStrIsValidIp(caEip)) {
            
             lUid = ntohl(utComHostAddress(caSip));            
             lEip = ntohl(utComHostAddress(caEip)); 
    		
    	}
    	
     }
     else if(atol(caUtype)==2){
     	lUid=atol(caCheID);
     	lEip=0;
     }
    
        iReturn = pasDbExecSqlF("update ncnsauser set did=%lu,stime='%s',utype=%d,uid=%lu,eip=%lu,aid=%lu,lasttime=%d where sid = %d",
                         atol(caDid),caStime,atol(caUtype),lUid,lEip,atol(caAid),time(0),atol(caSid));
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0700修改出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }                                     

        pasDbCommit(NULL);
    }
    
      lRowNum = 23;
      lStartRec = (lCurPg - 1) * lRowNum;    
      psDbHead = utPltInitDb();
     
      sprintf(caTemp,"select count(*) from ncnsauser  ");
      iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213定义邮件分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
 
 //sprintf(caTemp,"select a.sid,a.num,a.dip,a.prot,a.dtype,a.dport,a.qosway,a.qosid,b.qosname,d.aclname from ncnsaacllist a,ncnsaacl d  left join ncnsaqos b on (a.qosid=b.qosid) where a.aid=d.aid and a.aid=%s order by a.num limit %d,%d \n",caAid,lStartRec,lRowNum);  
// printf("caTemp=%s\n",caTemp); 
      psCur=pasDbOpenSqlF("select a.sid,a.did,a.stime,a.utype,a.uid,a.eip,a.aid,b.name,c.aclname from ncnsauser a left join ncnsaacl c on (a.aid=c.aid) left join nclimdatedes b on (a.did=b.id) order by a.sid desc  limit %d,%d ",lStartRec,lRowNum);
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
			lSid=0;
			lDid=0;
			memset(caStime,0,sizeof(caStime));
			lUtype=0;
			lUid=0;
			lEip=0;
			lAid=0;
			memset(caName_d,0,sizeof(caName_d));
			memset(caAclname,0,sizeof(caAclname));
			
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                   UT_TYPE_LONG,4,&lDid,
                                   UT_TYPE_STRING,63,caStime,
                                   UT_TYPE_LONG,4,&lUtype,
                                   UT_TYPE_ULONG,4,&lUid,
                                   UT_TYPE_ULONG,4,&lEip,
                                   UT_TYPE_LONG,4,&lAid,
                                   UT_TYPE_STRING,31,caName_d,
                                   UT_TYPE_STRING,31,caAclname);
                                   
   		 
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
      	utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
	      utPltPutLoopVarF(psDbHead,"iNum",iNum,"%lu",iNum);	
      	utPltPutLoopVarF(psDbHead,"did",iNum,"%lu",lDid);
      	utPltPutLoopVar(psDbHead,"stime",iNum,caStime);
        utPltPutLoopVarF(psDbHead,"utype",iNum,"%d",lUtype);
        utPltPutLoopVarF(psDbHead,"uid",iNum,"%lu",lUid);
        utPltPutLoopVarF(psDbHead,"aid",iNum,"%lu",lAid);
        
        
        if(lDid==0)
           utPltPutLoopVar(psDbHead,"name_d",iNum,ncLang("0383每天"));
        else
           utPltPutLoopVar(psDbHead,"name_d",iNum,caName_d);
        
        utPltPutLoopVar(psDbHead,"aclname",iNum,caAclname);   
           
       if(lUtype==0){
          utPltPutLoopVar(psDbHead,"type_desc",iNum,ncLang("0446全部"));
          utPltPutLoopVar(psDbHead,"name_obj",iNum,ncLang("0650无"));
        }
       else if(lUtype==1){
          utPltPutLoopVar(psDbHead,"type_desc",iNum,ncLang("0147部门"));
          utPltPutLoopVar(psDbHead,"name_obj",iNum,ncUtlGetGroupNameByid(psShmHead,lUid,ncLang("0629未知")));
        }
       else if(lUtype==2){
          utPltPutLoopVar(psDbHead,"type_desc",iNum,ncLang("0729用户"));
          utPltPutLoopVar(psDbHead,"name_obj",iNum,ncUtlBufGetUsername(psShmHead,lUid,ncLang("0629未知")));
        }
       else if(lUtype==3){
          utPltPutLoopVar(psDbHead,"type_desc",iNum,ncLang("1481网络"));
          strcpy(caSip,utComHostIp(htonl(lUid)));
          strcpy(caEip,utComHostIp(htonl(lEip)));
          
        utPltPutLoopVar(psDbHead,"eip",iNum,caEip);
        utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
          
          
          sprintf(caCont,"%s-%s",caSip,caEip);
          utPltPutLoopVar(psDbHead,"name_obj",iNum,caCont);
        }
       else{
          utPltPutLoopVar(psDbHead,"type_desc",iNum,ncLang("0446全部"));
          utPltPutLoopVar(psDbHead,"name_obj",iNum,ncLang("0650无"));
        }
          
       
      

      
			lSid=0;
			lDid=0;
			memset(caStime,0,sizeof(caStime));
			lUtype=0;
			lUid=0;
			lEip=0;
			lAid=0;
			memset(caName_d,0,sizeof(caName_d));
			memset(caAclname,0,sizeof(caAclname));
			
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                   UT_TYPE_LONG,4,&lDid,
                                   UT_TYPE_STRING,63,caStime,
                                   UT_TYPE_LONG,4,&lUtype,
                                   UT_TYPE_ULONG,4,&lUid,
                                   UT_TYPE_ULONG,4,&lEip,
                                   UT_TYPE_LONG,4,&lAid,
                                   UT_TYPE_STRING,31,caName_d,
                                   UT_TYPE_STRING,31,caAclname);
  				}
       lTotPg = (lCount - 1) / lRowNum + 1;
       pasDbCloseCursor(psCur);
       utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
       utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
       utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
 //  printf("end\n"); 
 
   i=0;
    while(iNum < lRowNum) {
        i++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%ld",iNum);
        iNum++;
    }
 
 //输出带宽策略
 i=0;
      psCur=pasDbOpenSqlF("select aid,aclname from ncnsaacl ");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
		  
		  id=0;
		  memset(name,0,sizeof(name));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);
                                  
      				 
      i=0;
      while(iReturn==0){
        utPltPutLoopVarF(psDbHead,"aid_d",i+1,"%d",id);
        utPltPutLoopVar(psDbHead,"aname_d",i+1,name);
      	i++;
	     
       id=0;
		   memset(name,0,sizeof(name));
       iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);

  		}     
       pasDbCloseCursor(psCur);
 
  //输出日期
    i=0;
      psCur=pasDbOpenSqlF("select id,name from nclimdatedes ");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
		  
		  id=0;
		  memset(name,0,sizeof(name));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);
                                  
      				 
      i=0;
      while(iReturn==0){
        utPltPutLoopVarF(psDbHead,"id_d2",i+1,"%d",id);
        utPltPutLoopVar(psDbHead,"name_d2",i+1,name);
      	i++;
	     
       id=0;
		   memset(name,0,sizeof(name));
       iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);

  		}
     
       pasDbCloseCursor(psCur);
 
 
   //输出部门
    i=0;
      psCur=pasDbOpenSqlF("select groupid,groupname from ncgroup");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
		  
		  id=0;
		  memset(name,0,sizeof(name));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);
                                  
      				 
      i=0;
      while(iReturn==0){
        utPltPutLoopVarF(psDbHead,"id_g",i+1,"%d",id);
        utPltPutLoopVar(psDbHead,"name_g",i+1,name);
      	i++;
	     
       id=0;
		   memset(name,0,sizeof(name));
       iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);

  		}
     
       pasDbCloseCursor(psCur);
 
 
    			 
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_userprocy.htm");
    return 0;
}


/* 带宽网络服务类别定义*/
int ncTcServiceType(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lTid;
   	char caSname[32];
   	char caPlate[128];
 	  pasDbCursor *psCur;

    psCur = pasDbOpenSqlF("select tid,sname from ncservicetype");
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("1472表ncnsaacl不存在"));
        return 0;
    }
    psDbHead = utPltInitDb();
    lTid=0;
    memset(caSname,0,sizeof(caSname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lTid,
  	                               UT_TYPE_STRING,16,caSname);
   	iNum = 0;
   	while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%u",iNum);
        utPltPutLoopVarF(psDbHead,"id",   iNum,"%u",lTid);
        utPltPutLoopVarF(psDbHead,"name", iNum,"%s",caSname);
    lTid=0;
    memset(caSname,0,sizeof(caSname));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lTid,
  	                               UT_TYPE_STRING,16,caSname);
    }
    pasDbCloseCursor(psCur);
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_servicetype.htm");
    return 0;
}


//网络服务类别保存
int ncTcServiceTypeSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
   	long lId,i,lSid;
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
  
            
        if(!utStrIsSpaces(caName)) {
            iReturn = pasDbExecSqlF("insert into ncservicetype(sname,lasttime) values ('%s',%d)",
                                             caName,time(0));
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

				        iReturn = pasDbExecSqlF("delete from ncservicetype where tid= %lu",lSid);
				        if(iReturn != 0) {
				            pasDbRollback(NULL);
				        }                
				        else {
				         iReturn = pasDbExecSqlF("delete from ncservicelist where tid= %lu",lSid);
				            pasDbCommit(NULL);
				        }            
   	            

   	        }        
   	    }
    	
             
    }
    else if(!utStrIsSpaces(caModi) && !utStrIsSpaces(caName)) { /* 修改一个组  */
        iReturn = pasDbExecSqlF("update ncservicetype set sname='%s',lasttime=%d where tid = %s",
                         caName,time(0),caId);
        pasDbCommit(NULL);
    }
    return ncTcServiceType(psShmHead,iFd,psMsgHead);
    return 0;
}




 //网络服务类别明细设置
int ncWebServiceTypeDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
    char *p;
    char caVar[32];
    int i=0;
   	long lId;
   	char caName[64];
   	char caPlate[128];
   	char caGid[16],caDid[16],caAdd[16],caDel[16];
	char caNname[32],caNid[16],caModi[16];
	char caTotPg[16],caCurPg[16],caCount[16];
	long lRowNum,lStartRec,lCurPg;
	long lCount,lTid,lTotPg;
	char caTemp[1024];
  char caQosid[16];
  long lLasttime,lQosid;
  long lSid,lNum;
  long lSum,lSum1;
  char caTid[16];
  long lSersum,lServiceid,id;
  char name[32];
  char caServiceid[16],caSid[16];
 char caSname[32];
 	pasDbCursor *psCur;
 	char caCont[256];
 	long lMax;
	iReturn = utMsgGetSomeNVar(psMsgHead,8,
	                "tid",  UT_TYPE_STRING,10,caTid,
	                "serviceid",    UT_TYPE_STRING,10,caServiceid,
	                "add",    UT_TYPE_STRING,12,caAdd,
	                "del",    UT_TYPE_STRING,12,caDel,
	                "modi",   UT_TYPE_STRING,12,caModi,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			            "CurPg",      UT_TYPE_STRING, 12, caCurPg,
			            "sid",        UT_TYPE_STRING,10,caSid);
memset(caSname,0,sizeof(caSname));
 if(strlen(caTid)>0){
 	sprintf(caTemp,"select sname from ncservicetype where tid=%s ",caTid);
 	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,16,caSname);
}


   	             
	                
   lCurPg = atol(caCurPg); 
    if(lCurPg <= 0) lCurPg = 1;

 


  if(utStrIsSpaces(caTid)){
  	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("1482请先选择服务类别，然后添加服务类别明细"),ncLang("1482请先选择服务类别，然后添加服务类别明细"));
    return 0;
  }
  

  
   
    if(!utStrIsSpaces(caAdd)) { /* 增加  */
     
       sprintf(caTemp,"insert into ncservicelist(tid,serviceid,lasttime) values (%d,%d,%d)",
                                            atol(caTid),atol(caServiceid),time(0));
 //      printf("caTemp=%s\n",caTemp);
          
          iReturn = pasDbExecSqlF("insert into ncservicelist(tid,serviceid,lasttime) values (%d,%d,%d)",
                                            atol(caTid),atol(caServiceid),time(0));

            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0251分类列表"),ncLang("0310记录增加出错"));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                pasDbCommit(NULL);
            }                
          
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除一个组  */
    for(i=0;i<29;i++){
    	sprintf(caVar,"chk%d",i+1);
    	p=utMsgGetVar_s(psMsgHead,caVar);
    	if(p)
    	{
 	       iReturn = pasDbExecSqlF("delete from ncservicelist where sid= %s",p);
	        if(iReturn != 0) {
	            pasDbRollback(NULL);
	        }                
	        else {

	          
	            pasDbCommit(NULL);
	        } 
	}
	}           
    }
    else if(!utStrIsSpaces(caModi) && !utStrIsSpaces(caSid)) { /* 修改一个组  */
    	  
      
        iReturn = pasDbExecSqlF("update ncservicelist set serviceid=%d,lasttime=%d where sid = %d",
                         atol(caServiceid),time(0),atol(caSid));
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0700修改出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }                                     

        pasDbCommit(NULL);
    }
    
      lRowNum = 25;
      lStartRec = (lCurPg - 1) * lRowNum;    
      psDbHead = utPltInitDb();
      
      utPltPutVar(psDbHead,"sname",caSname);
      utPltPutVar(psDbHead,"tid",caTid);
      
      
		 
      sprintf(caTemp,"select count(*) from ncservicelist a,ncipindex b where a.tid=b.id and a.tid=%s  ",caTid);
      iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213定义邮件分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }

      psCur=pasDbOpenSqlF("select a.sid,a.serviceid,a.lasttime,b.name from ncservicelist a,ncipindex b  where a.serviceid=b.id and a.tid=%d order by a.sid limit %d,%d ",atol(caTid),lStartRec,lRowNum);
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
			lSid=0;
			lServiceid=0;
			lLasttime=0;
			memset(caName,0,sizeof(caName));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                   UT_TYPE_LONG,4,&lServiceid,
                                   UT_TYPE_LONG,4,&lLasttime,
                                   UT_TYPE_STRING,31,caName);
                                
   		 
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
      	utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
	      utPltPutLoopVarF(psDbHead,"iNum",iNum,"%lu",iNum);	
        utPltPutLoopVarF(psDbHead,"serviceid",iNum,"%lu",lServiceid);
        utPltPutLoopVar(psDbHead,"sname",iNum,caName);
        utPltPutLoopVar(psDbHead,"lasttime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lLasttime));
      
			lSid=0;
			lServiceid=0;
			lLasttime=0;
			memset(caName,0,sizeof(caName));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                   UT_TYPE_LONG,4,&lServiceid,
                                   UT_TYPE_LONG,4,&lLasttime,
                                   UT_TYPE_STRING,31,caName);
  				}
       lTotPg = (lCount - 1) / lRowNum + 1;
       pasDbCloseCursor(psCur);
       utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
       utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
       utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
 //  printf("end\n"); 
 
   i=0;
    while(iNum < lRowNum) {
        i++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%ld",iNum);
        iNum++;
    }
 
 //网络服务
      psCur=pasDbOpenSqlF("select id,name from ncipindex where id not in (select serviceid from ncservicelist where tid =%s ) order by id limit 0,999",caTid);
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
		  lSersum=0;
		  id=0;
		  memset(name,0,sizeof(name));
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);
                                  
      				 
      iNum=0;
      while(iReturn==0){
      	
      	utPltPutLoopVarF(psDbHead,"id",iNum+1,"%d",id);
      	utPltPutLoopVar(psDbHead,"name",iNum+1,name);
      	iNum++;
	     
       id=0;
		   memset(name,0,sizeof(name));
       iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&id,                                  
                                   UT_TYPE_STRING,31,name);

  		}
     
       pasDbCloseCursor(psCur);

 
 
    			 
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_servicetypedet.htm");
    return 0;
}



/* 显示带宽策略树 */
int ncWebTcProcyTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caCurPg[16],caTotRec[16];
    char caSid[16],caModi[16],caDel[16],caAdd[16],caFlags[16],caStatus[16];
    char caName[32],caApp[16],caName1[32];
    char caPlate[64];
    char caVister[16];
    char caModlb[16],caRowNum[16];
    char *pSelect,*p;
    int i,iNum,iReturn;
    long lSid,lSid1,lFlags,lFlags1,lCount,lCount1,lStatus,lStatus1,lId;
    utPltDbHead *psDbHead;
   	long lStartRec,lCurPg,lTotRec,lRowNum,lTotPg,lVister,lVister1;
   	char flag_desc[32];
   	char caFlag[12];
   	long lFlag;
 	pasDbCursor *psCur;
 	char caTemp[1024];
#ifdef LDEBUG 	
    utMsgPrintMsg(psMsgHead);
#endif    
 char caUsername2[32];
  memset(caUsername2,0,sizeof(caUsername2));
 dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
	iReturn = utMsgGetSomeNVar(psMsgHead,7,	                
	                "flag",      UT_TYPE_STRING, 10, caFlag,
	                "qosid",        UT_TYPE_STRING, 12, caSid,
	                "qosname",       UT_TYPE_STRING, 31, caName, 
	                "add",        UT_TYPE_STRING, 12, caAdd,
	                "modi",       UT_TYPE_STRING, 12, caModi,
	                "del",        UT_TYPE_STRING, 12, caDel,
	                "app",        UT_TYPE_STRING, 12, caApp);
				
 
    lSid = atol(caSid);		
    lFlag = atol(caFlag);
   if(lFlag==1){
   	 strcpy(flag_desc,ncLang("1824一级"));
   }
   else{
   	lFlag=2;
      strcpy(flag_desc,ncLang("1825二级"));
    }
  
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
        if(utStrIsSpaces(caName)) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0360类别不能为空"));
            return 0;
        }
        else {
            lSid=0;
            pasDbOneRecord("select max(qosid) from ncnsaqos ",0,UT_TYPE_LONG,4,&lSid);
            
            if(lSid<100){
            	lSid=100+1;
            }
            else{
            	lSid=lSid+1;
            }
            
            sprintf(caTemp,"insert into ncnsaqos(qosid,qosname,qoslevel,lasttime) values (%lu,'%s',%d,%d)", lSid,caName,lFlag,time(0));				 		
				 		 ncsWriteSysLog(psMsgHead,caUsername2,"04","success",caTemp);
            iReturn = pasDbExecSqlF("insert into ncnsaqos(qosid,qosname,qoslevel,lasttime) values (%lu,'%s',%d,%d)", lSid,caName,lFlag,time(0));
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0311记录增加出错%s"),pasDbErrorMsg(NULL));
                pasDbRollback(NULL);
                return 0;
            }        
            else {
                strcpy(caTotRec,"\0");
                pasDbCommit(NULL);
            }
        }
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
       
   	    for(i=0;i<300;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
   	            lSid = atol(p);
   	            sprintf(caTemp,"delete from ncnsaqos  where qosid = %lu",lSid);
   	//            printf("caTemp=%s\n",caTemp);
				 		    ncsWriteSysLog(psMsgHead,caUsername2,"04","success",caTemp);
                iReturn = pasDbExecSqlF("delete from ncnsaqos  where qosid = %lu",lSid);
   	        }
            strcpy(caTotRec,"\0");   	        
   	    }
    }
    else if(!utStrIsSpaces(caModi)) { /* 修改记录  */
        if(!utStrIsSpaces(caSid)) {
        	  sprintf(caTemp,"update ncnsaqos set qosname='%s',lasttime=%d  where qosid = %lu",
                    caName,time(0),lSid);
				 		 ncsWriteSysLog(psMsgHead,caUsername2,"04","success",caTemp);
            iReturn = pasDbExecSqlF("update ncnsaqos set qosname='%s',lasttime=%d  where qosid = %lu",
                    caName,time(0),lSid);
            if(iReturn == 0) {
                pasDbCommit(NULL);
            }
            else {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0305记录修改出错, %s"),pasDbErrorMsg(NULL));
                pasDbRollback(NULL);
            }
        }
        strcpy(caTotRec,"\0");
    }
   
       
 // printf("select qosid,qosname from ncnsaqos where qoslevel=%d order by qosid\n ",lFlag);
   
    psCur = pasDbOpenSqlF("select qosid,qosname from ncnsaqos where qoslevel=%d order by qosid ",lFlag);
                         
   
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }
    psDbHead = utPltInitDb();
    lSid1=0;
    memset(caName1,0,sizeof(caName1));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid1,
  	                               UT_TYPE_STRING,31,caName1);
  	                             
   	iNum = 0;
   	while(iReturn == 0 || iReturn==1405) {
        iNum++;

        utPltPutLoopVarF(psDbHead,"num",   iNum,"%u",iNum);
        utPltPutLoopVarF(psDbHead,"lid",   iNum,"%lu",lSid1);
        utPltPutLoopVar(psDbHead, "lname",  iNum,caName1);
       
    lSid1=0;
    memset(caName1,0,sizeof(caName1));
    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid1,
  	                               UT_TYPE_STRING,31,caName1);
    }
    pasDbCloseCursor(psCur);
    utPltPutVar(psDbHead,"title",flag_desc);
    utPltPutVar(psDbHead,"flag",caFlag);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_webtcprocytree.htm");
   
    return 0;
}


//显示带宽策略
int ncWebTcProcy_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    char caTemp[1024];
    long i,lSum;
    long lQosid;
    char caName[20];
    char caQosid[16];
    char caQosname[32];
    long iNum;
    long lQosminup,lQosmaxup,lQosmindown,lQosmaxdown,lQos0,lQoslevel,lQostype;
   	int iReturn;
   	   long lGroupid0=0;
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
   	
   	iReturn = utMsgGetSomeNVar(psMsgHead,1, "qosid", UT_TYPE_STRING, 10, caQosid);
   	              
    lQosid=atol(caQosid);
    memset(caQosname,0,sizeof(caQosname));
    lQosminup=0;
    lQosmaxup=0;
    lQosmindown=0;
    lQosmaxdown=0;
    lQos0=0;
    lQoslevel=0;
    lQostype=0;
        iReturn = pasDbOneRecord("select qosname,qosminup,qosmaxup,qosmindown,qosmaxdown,qos0,qoslevel,qostype from ncnsaqos where qosid = :cid",1,
                            "cid",UT_TYPE_LONG,lQosid,
                            UT_TYPE_STRING,30,caQosname,
                            UT_TYPE_LONG,4,&lQosminup,
                            UT_TYPE_LONG,4,&lQosmaxup,
                            UT_TYPE_LONG,4,&lQosmindown,
                            UT_TYPE_LONG,4,&lQosmaxdown,
                            UT_TYPE_LONG,4,&lQos0,
                            UT_TYPE_LONG,4,&lQoslevel,
                            UT_TYPE_LONG,4,&lQostype);
                          
                           
        if(iReturn != 0 && iReturn != PAS_DB_NULLVALUE) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0703修改通道"),ncLang("0572通道不存在 %d"),iReturn);
            return 0;
        }
        else {
    		psDbHead = utPltInitDb();
    	
    	utPltPutVar(psDbHead,"qosid",caQosid);
    	if(lQoslevel==1){
    		utPltPutVar(psDbHead,"title",ncLang("1824一级"));
    	}
    	else{
    		utPltPutVar(psDbHead,"title",ncLang("1825二级"));
    	}
            utPltPutVar(psDbHead,"qosname",caQosname);
            utPltPutVarF(psDbHead,"qosminup","%d",lQosminup);
            utPltPutVarF(psDbHead,"qosmaxup","%d",lQosmaxup);
            utPltPutVarF(psDbHead,"qosmindown","%d",lQosmindown);
            utPltPutVarF(psDbHead,"qosmaxdown","%d",lQosmaxdown);
            utPltPutVarF(psDbHead,"qos0","%d",lQos0);
            utPltPutVarF(psDbHead,"qostype","%d",lQostype);
            utPltPutVarF(psDbHead,"qoslevel","%d",lQoslevel);
        
        }
   
   if(lQoslevel==1){
       psCur=pasDbOpenSqlF("select qosname from ncnsaqos where  qos0=%d ",lQosid);
        if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
        memset(caQosname,0,sizeof(caQosname));
        memset(caTemp,0,sizeof(caTemp));
        iReturn=pasDbFetchInto(psCur,UT_TYPE_STRING,30,caQosname);
        if(iReturn==0||iReturn==1405)
           strcpy(caTemp,ncLang("1418下级带宽策略："));
        while(iReturn==0||iReturn==1405){
        	
        	sprintf(caTemp+strlen(caTemp),"[%s]&nbsp;",caQosname);
        	memset(caQosname,0,sizeof(caQosname));
           iReturn=pasDbFetchInto(psCur,UT_TYPE_STRING,30,caQosname);
        }
        pasDbCloseCursor(psCur);
        if(strlen(caTemp)>0){
        	utPltPutVar(psDbHead,"nexprocy",caTemp);
    }
   }
   else{
   	//对于二级策略输出上级策略
   	 psCur=pasDbOpenSqlF("select qosid,qosname from ncnsaqos where  qoslevel=1 ");
        if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
        memset(caQosname,0,sizeof(caQosname));
        lQosid=0;
        iReturn=pasDbFetchInto(psCur, UT_TYPE_LONG,4,&lQosid,
        															UT_TYPE_STRING,30,caQosname);
        iNum=0;
        while(iReturn==0||iReturn==1405){
        	utPltPutLoopVarF(psDbHead,"qsid",iNum+1,"%d",lQosid);
        	utPltPutLoopVar(psDbHead,"qname",iNum+1,caQosname);
        	iNum++;
        memset(caQosname,0,sizeof(caQosname));
        lQosid=0;
        iReturn=pasDbFetchInto(psCur, UT_TYPE_LONG,4,&lQosid,
        															UT_TYPE_STRING,30,caQosname);
        }
        pasDbCloseCursor(psCur);
   	
   	
  }
   if(lQoslevel==1){
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_procynew_first.htm");
   }
   else
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_procynew_secord.htm");
  
    return 0;
}


/* 显示带宽策略树 */
int ncWebTcProcy_newSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   
 utPltDbHead *psDbHead;
  char caFlag[12],caSid[16],caName[32],caQosminup[12],caQosmaxup[12],caQosmindown[12];
  char caQosmaxdown[12],caQos0[12],caQoslevel[12],caQostype[12];	
 	pasDbCursor *psCur;
 	char caTemp[1024];
 	long iReturn;
#ifdef LDEBUG 	
    utMsgPrintMsg(psMsgHead);
#endif    
 char caUsername2[32];

	iReturn = utMsgGetSomeNVar(psMsgHead,10,	                
	                "flag",      UT_TYPE_STRING, 10, caFlag,
	                "qosid",        UT_TYPE_STRING, 12, caSid,
	                "qosname",       UT_TYPE_STRING, 31, caName, 
	                "qosminup",     UT_TYPE_STRING,10,caQosminup,
	                "qosmaxup",     UT_TYPE_STRING,10,caQosmaxup,
	                "qosmindown",   UT_TYPE_STRING,10,caQosmindown,
	                "qosmaxdown",   UT_TYPE_STRING,10,caQosmaxdown,
	                "qos0",         UT_TYPE_STRING,10,caQos0,
	                "qoslevel",     UT_TYPE_STRING,10,caQoslevel,
	                "qostype",      UT_TYPE_STRING,10,caQostype);
	               
	if(atol(caSid)>0){
		iReturn = pasDbExecSqlF("update ncnsaqos set qosname='%s',qosminup=%d,qosmaxup=%d,qosmindown=%d,qosmaxdown=%d,qos0=%d,qoslevel=%d,qostype=%d,lasttime=%d  where qosid = %lu",
                    caName,atol(caQosminup),atol(caQosmaxup),atol(caQosmindown),atol(caQosmaxdown),atol(caQos0),atol(caQoslevel),atol(caQostype),time(0),atol(caSid));
            if(iReturn == 0) {
                pasDbCommit(NULL);
            }
            else {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0086Web地址列表"),ncLang("0305记录修改出错, %s"),pasDbErrorMsg(NULL));
                pasDbRollback(NULL);
            }
		
	}
 
 ncWebTcProcy_new(psShmHead,iFd,psMsgHead);
 
         
   
    return 0;
}
static int IsInGroupids(char *ids,char *id){
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
//针对多路带宽控制
//显示接口界面
int ncWebTcDispInterface2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    long i,lSum;
    char caName[20];
    char caCid[16];
    int ethcount;
    char caIndevadd[36],caOutdevadd[36];
    char temp_indev[32],temp_outdev[32],temp_i[32];
    unsigned long lCid,lIndev,lOutdev,lStatus,lOpt,lBandrate,lUprate;
    long pppcount,m;
   	int iReturn;
   	long lGroupid0=0;
   	ethcount=0;
   	memset(temp_indev,0,sizeof(temp_indev));
   	memset(temp_outdev,0,sizeof(temp_outdev));
   	
    ethcount=atol(utComGetVar_sd(psShmHead,"ethcount","3")); 
    pppcount=atol(utComGetVar_sd(psShmHead,"pppcount","0")); 
   	   
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
   	
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
   	                "cid", UT_TYPE_STRING, 10, caCid);
   	              
    lCid=atol(caCid);
    memset(caIndevadd,0,sizeof(caIndevadd));
    memset(caOutdevadd,0,sizeof(caOutdevadd));
        iReturn = pasDbOneRecord("select name,indev,outdev,status,opt,bandrate,uprate,indevadd,outdevadd from ncnsaconnect where cid = :cid",1,
                            "cid",UT_TYPE_LONG,lCid,
                            UT_TYPE_STRING,15,caName,
                            UT_TYPE_LONG,4,&lIndev,
                            UT_TYPE_LONG,4,&lOutdev,
                            UT_TYPE_LONG,4,&lStatus,
                            UT_TYPE_LONG,4,&lOpt,
                            UT_TYPE_LONG,4,&lBandrate,
                            UT_TYPE_LONG,4,&lUprate,
                            UT_TYPE_STRING,32,caIndevadd,
                            UT_TYPE_STRING,32,caOutdevadd);
                           
        if(iReturn != 0 && iReturn != PAS_DB_NULLVALUE) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0703修改通道"),ncLang("0572通道不存在 %d"),iReturn);
            return 0;
        }
        else {
    		psDbHead = utPltInitDb();
 sprintf(temp_indev,"%d",lIndev);
 sprintf(temp_outdev,"%d",lOutdev); 
   	
            utPltPutVar(psDbHead,"cname",caName);
//            utPltPutVarF(psDbHead,"indev","%d",lIndev);
//            utPltPutVarF(psDbHead,"outdev","%d",lOutdev);
            utPltPutVarF(psDbHead,"status","%d",lStatus);
            utPltPutVarF(psDbHead,"opt","%d",lOpt);
            utPltPutVarF(psDbHead,"bandrate","%d",lBandrate);
            utPltPutVarF(psDbHead,"uprate","%d",lUprate);
            utPltPutVar(psDbHead,"cid",caCid);
        
        }
   
//    lSum=atol(utComGetVar_sd(psShmHead,"MaxNsaDev","8"));
    for(i=0;i<ethcount;i++){
    	utPltPutLoopVarF(psDbHead,"i",i+1,"%d",i);
    	sprintf(temp_i,"%d",i);

    	if(i==lIndev||(IsInGroupids(caIndevadd,temp_i)==0)){
    		utPltPutLoopVar(psDbHead,"brethin",i+1,"checked");
    	}
    	if(i==lOutdev||(IsInGroupids(caOutdevadd,temp_i)==0)){
    		utPltPutLoopVar(psDbHead,"brethout",i+1,"checked");
    	}
    }

    for(m=0;m<pppcount;m++){
    	utPltPutLoopVarF(psDbHead,"pi",m+1,"%d",m+100);
    	 utPltPutLoopVarF(psDbHead,"m",m+1,"%d",m);
    	utPltPutLoopVarF(psDbHead,"mi",m+1,"%d",i);
    	sprintf(temp_i,"%d",m+100);
    	if(m+100==lIndev||strstr(caIndevadd,temp_i)){
    		utPltPutLoopVar(psDbHead,"pppin",m+1,"checked");
    	}
    	if(m+100==lOutdev||strstr(caOutdevadd,temp_i)){
    		utPltPutLoopVar(psDbHead,"pppout",m+1,"checked");
    	}
    	i++;
    }





   utPltPutVarF(psDbHead,"ethcount","%d",ethcount+pppcount);
   
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"tcnew/nctc_intfaceset2.htm");
    return 0;
}


/* 修改多路带宽接口参数 */
int ncWebTcInterfaceSave2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caName[20];
    char caCname[16];
    char caCid[16],caStatus[12],caIndev[12],caOutdev[12],caBandrate[12],caOpt[12],caUprate[12];
    char caApp[12];
   	char caTemp[1024];
   	char caEthcount[12],caIn[36],caOut[36],indev_temp[16],outdev_temp[16];
   	long num_in,num_out,i;
   	char *p,*p1;
    unsigned long lCid,lStatus,lIndev,lOutdev,lBandrate,lOpt,lCount,lUprate;
   	int iReturn;
//   	utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,10,
   	                "cid",    UT_TYPE_STRING, 10, caCid,
   	                "cname", UT_TYPE_STRING, 15, caCname,
   	                "status",UT_TYPE_STRING,2,caStatus,
   	                "indev",UT_TYPE_STRING,8,caIndev,
   	                "outdev",UT_TYPE_STRING,8,caOutdev,
   	                "bandrate",UT_TYPE_STRING,10,caBandrate,
   	                "uprate",  UT_TYPE_STRING,10,caUprate,
   	                "opt",     UT_TYPE_STRING, 10, caOpt,
   	                "app",      UT_TYPE_STRING, 11, caApp,
   	                "ethcount",UT_TYPE_STRING,8,caEthcount);
   	num_in=0;
   	num_out=0;
   	strcpy(caIn," ");
   	strcpy(caOut," ");                
   	for(i=0;i<atol(caEthcount);i++){
   		sprintf(indev_temp,"indev%d",i);
   		sprintf(outdev_temp,"outdev%d",i);
   		p = utMsgGetVar_s(psMsgHead,indev_temp);
   		if(p){
   			if(num_in==0){
   				lIndev=atol(p);
   			}
   			else if(num_in==1){
   				sprintf(caIn,"%s",p);
   			}
   			else{
   				sprintf(caIn+strlen(caIn),",%s",p);
   			}
   			num_in++;
   		}
   		
   		
      p1=utMsgGetVar_s(psMsgHead,outdev_temp);
       if(p1){
       	if(strstr(caIn,p1)||(lIndev==atol(p1))){
       		  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("1468修改接口参数"),"内外网接口不能重复",pasDbErrorMsg(NULL));				
        		return 0;
       	}
   			if(num_out==0){
   				lOutdev=atol(p1);
   			}
   			else if(num_out==1){
   				sprintf(caOut,"%s",p1);
   			}
   			else{
   				sprintf(caOut+strlen(caOut),",%s",p1);
   			}
   			num_out++;
   		}
      
      
   		
   	}                
   	                
   	   if(num_in==0||num_out==0){
   	   	 		utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("1468修改接口参数"),"不允许不选接口",pasDbErrorMsg(NULL));				
        		return 0;
   	  }             
   	                
    lCid=atol(caCid);
    lStatus=atol(caStatus);

    lBandrate=atol(caBandrate);
    lOpt=atol(caOpt);
    lUprate=atol(caUprate);
   lCount=0;
  
 
  iReturn = pasDbExecSqlF("update ncnsaconnect set name='%s',indev=%lu,outdev=%lu,status=%lu,opt=%lu,bandrate=%lu,lasttime=%d,uprate=%d,indevadd='%s',outdevadd='%s' where cid=%d ",
   caCname,lIndev,lOutdev,lStatus,lOpt,lBandrate,time(0),lUprate,caIn,caOut,lCid);
                    
            if(iReturn != 0) {
 		  		utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("1468修改接口参数"),ncLang("0563数据修改不正确 %s"),pasDbErrorMsg(NULL));
 				 pasDbRollback(NULL);
        		return 0;
        	}
   		    else{
   		    	pasDbCommit(NULL);
   		    }
   			
	if(!utStrIsSpaces(caApp)) {
		ncNsaCvtDbConnectToFile("/home/ncmysql/nc/bin/nsa.conf");

 

  //      if(strcasecmp(utComGetVar_sd(psShmHead,"BandWidthLimit","YES"),"YES") == 0) {
  //         	ncUtlTcWidthInit(psShmHead);
        }
	ncWebTcDispInterface2(psShmHead,iFd,psMsgHead);
	return 0;
}
