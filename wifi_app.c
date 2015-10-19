#include "lan_md5.h"
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stddef.h"
#include <math.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "nwdef.h"
#include "utoall.h"
#define PI                      3.1415926
#define EARTH_RADIUS            6378.137        //地球近似半径




/* 用户验证  */
int ncWebWifiAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caUsername[33],caPassword[33],caIp[16];
   	char caKey[64],caRight[65];
   	char caModi[24];
   	char caPass0[32];
   	char caContuse[16];
   	char *pProdSn,*pHost,*pDispName;
   	char caCount[8];
   	char caTemp2[1024];
   	unsigned long lIp;
   	unsigned long lTime;
   	char caDispname[32];
   	int iCount,lAuthcount;
    int iLicense = 0;
    char caTemp[1024],caAuthcount[8];
    unsigned long lUsrlevel;
    unsigned long lExpire,lUsers,lStart,lLicenseTime;
    utPltDbHead *psDbHead= utPltInitDbHead();
    long lUserid;
    char caSql[1024];
   	int iReturn,lDef,lLogflag,lDefCon,lFlag;
   	char caMbserno[36];
   	char caPassword_t[36];
   	char caScreen[12];
   	long lGroupid;
   	long lId;
   	char caShortname[32] = "";
    strcpy(caModi,"\0");
    lTime = time(0);
    strcpy(caAuthcount,utComGetVar_sd(psShmHead,"AuthCount","3"));
    lAuthcount=atol(caAuthcount);
    lLicenseTime = utComGetVar_ld(psShmHead,"LicenseTime",5);
    lUserid=0;
    char type_in[8] = "";
    
	utMsgPrintMsg(psMsgHead);   
 // printf("pid=%d\n",getpid());
 // sleep(20);
  
    
   	iReturn = utMsgGetSomeNVar(psMsgHead,8,
   	      "ModiPass",  UT_TYPE_STRING, 16, caModi,
					"shortname",  UT_TYPE_STRING, 31, caShortname,
					"username",  UT_TYPE_STRING, 31, caUsername,
					"password",  UT_TYPE_STRING, 31, caPassword,
					"clientip",  UT_TYPE_STRING, 15, caIp,
					"contuse",   UT_TYPE_STRING, 11, caContuse,
					"screenheight",UT_TYPE_STRING,8,caScreen,
					"type",UT_TYPE_STRING,sizeof(type_in),type_in);
					strcpy(caPassword_t,caPassword);
	if(strlen(caScreen)==0) strcpy(caScreen,"768");
	utPltPutVar(psDbHead,"type",type_in);
	printf("pid=%d\n",getpid());
	//	sleep(15);	
	lGroupid=0;
	if(strlen(caUsername)>0){
		memset(caDispname,0,sizeof(caDispname));		
		sprintf(caTemp,"select id,dispname,groupid from wf_dsuser where name='%s' ",caUsername);
		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lUserid,UT_TYPE_STRING,30,caDispname,UT_TYPE_LONG,4,&lGroupid);
	}else{
	  	 //psDbHead = utPltInitDbHead();
	  	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");
	  	return 0;
	}
	if(strlen(caDispname)==0) 
		strcpy(caDispname,caUsername);
    /* 检查许可证  */
    pProdSn = utComGetVar_sd(psShmHead,"ProductSn","\0");
    iLicense = pasInstGetLicense(pProdSn,&lExpire,&lUsers,&lStart);
   
//    iLicense=1;
    if(iLicense != 1) { /* 许可证不正确  */
        //psDbHead = utPltInitDbHead();
        utPltPutVarF(psDbHead,"prodsn",pProdSn);
        pHost = pasInstGetHostInfo("eth0");
        if(pHost == NULL) {
            utPltPutVarF(psDbHead,"message", "System Error");
            ncsWriteSysLog(psMsgHead,caUsername,"01","Fail","License Error");
//            ncAdmWriteLog(caUsername,caIp,"License Error");
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");
            return 0;
        }
        utPltPutVarF(psDbHead,"hostinfo",pHost);
        pDispName = utComGetVar_sd(psShmHead,"YourDispName","\0");
        utPltPutVarF(psDbHead,"name",pDispName);
        ncsWriteSysLog(psMsgHead,caDispname,"01","Fail","License Error");
//        ncAdmWriteLog(caUsername,caIp,"License Error");
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main_licenseerror.htm");
    }
    if(utStrIsSpaces(caContuse) && lTime > lExpire - lLicenseTime * 86400L) { /* License 即将到期  */
        //psDbHead = utPltInitDbHead();
        utPltPutVarF(psDbHead,"prodsn",pProdSn);
        pHost = pasInstGetHostInfo("eth0");
        if(pHost == NULL) {
            utPltPutVarF(psDbHead,	"message", 	"System Error");
            ncAdmWriteLog(caDispname,	caIp,	"System Error");
            utPltOutToHtml(iFd,	psMsgHead,	psDbHead,	"nc/login2.htm");
            return 0;
        }
        utPltPutVarF(psDbHead,	"hostinfo",	pHost);
        pDispName = utComGetVar_sd(psShmHead,	"YourDispName",	"\0");
        utPltPutVarF(psDbHead,	"name",	pDispName);
        utPltPutVarF(psDbHead,	"sdate",	utTimFormat("%Y-%m-%d %H:%M:%S",	lExpire));
        utPltPutVarF(psDbHead,	"shortname",	caShortname);
        utPltPutVarF(psDbHead,	"username",		caUsername);
        utPltPutVarF(psDbHead,	"password",		caPassword);
        utPltPutVarF(psDbHead,	"ModiPass",		caModi);
        ncAdmWriteLog(caUsername,	caIp,	"License Expire");
        utPltOutToHtml(iFd,psMsgHead,psDbHead,	"nc/main_licenseexpire.htm");
        return 0;
    }
    lIp = utComHostAddress(caIp);
    if((iCount=ncAdmCheckAdmIp(psShmHead,caUsername,lIp))>lAuthcount) { /* 检查IP地址  */
        //psDbHead = utPltInitDbHead();
        utPltPutVarF(psDbHead,"authcount_set","%lu",lAuthcount);
        utPltPutVarF(psDbHead,"authcount","%lu",iCount);
        utPltPutVarF(psDbHead,"message", ncLang("0000您已连续%lu次输入出错，终端被锁定"),iCount);
        char caMsg[128];
        sprintf(caMsg,ncLang("0000您已连续%lu次输入出错，终端被锁定"),iCount);

        ncsWriteSysLog(psMsgHead,caDispname,"01","Fail",caMsg);
//        ncAdmWriteLog(caUsername,caIp,ncLang("0366连续三次登录不成功，?斩吮凰?));
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");
        return 0;
    }
    
    utStrDelSpaces(caUsername);
//    utStrToLower(caUsername);
    utStrDelSpaces(caPassword);
    
    if((strlen(caUsername)==0)||(strlen(caPassword)==0)){
    	 //psDbHead = utPltInitDbHead();
    	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");
    	 return 0;
    }
//printf("bbbbbbbbbbbbbbbbbb\n");    mark
	if(strcmp(utComGetVar_sd(psShmHead,"clouduser","No"),"Yes")==0){
		iReturn = dsCltUserLogin3(caUsername,caPassword,caShortname);
	}else{
		iReturn=0;
	}

	if(iReturn == 0){
	    iReturn = dsCltUserLogin(caUsername,caPassword,caIp);
	}
    if(iReturn != 0) {
        if(iReturn == (-1)) {
			//psDbHead = utPltInitDbHead();
			utPltPutVarF(psDbHead,"message", ncLang("0001用户不存在"));
			sprintf(caTemp2,ncLang("0002第%d次用户不存在"),iCount);
			ncsWriteSysLog(psMsgHead,caDispname,"01","Fail",caTemp2);
	 //           ncAdmWriteLog(caUsername,caIp,ncLang("0205第%d次用户不存在"),iCount);
			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");
		}else if(iReturn == (-2)) {
			//psDbHead = utPltInitDbHead();
			utPltPutVarF(psDbHead,"message", ncLang("0003口令不正确"));
	  //          ncAdmWriteLog(caUsername,caIp,ncLang("0203第%d次口令不正确"),iCount);	
			sprintf(caTemp2,ncLang("0004第%d次口令不正确"),iCount);
			ncsWriteSysLog(psMsgHead,caDispname,"01","Fail",caTemp2);
			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");
		}else if(iReturn == (-3)) {
			psDbHead = utPltInitDbHead();
			utPltPutVarF(psDbHead,"message", ncLang("0005公司简称不正确"));
	  //          ncAdmWriteLog(caUsername,caIp,ncLang("0203第%d次口令不正确"),iCount);		
			sprintf(caTemp2,ncLang("0006第%d公司简称不正确"),iCount);
			ncsWriteSysLog(psMsgHead,caDispname,"01","Fail",caTemp2);
			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");
		}else{
			//psDbHead = utPltInitDbHead();
	 //           ncAdmWriteLog(caUsername,caIp,ncLang("0204第%d次验证不正确"),iCount);
			sprintf(caTemp2,ncLang("0008第%d次验证不正确"),iCount);
			ncsWriteSysLog(psMsgHead,caDispname,"01","Fail",caTemp2);
			utPltPutVarF(psDbHead,"message", ncLang("0007验证不正确"));
			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");
		}
		return 0;
	}else{
		//psDbHead = utPltInitDb();
		if(strcasecmp(caPassword_t,"manager")==0){
			sprintf(caTemp,"select id from wf_dsuser where name='%s' ",caUsername);
			strcpy(caPassword_t,pProdSn);
			pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lId);
			iReturn = dsCltUserModi(lId,2,"name",UT_TYPE_STRING,caUsername,
										  "lkey",UT_TYPE_STRING,caPassword_t);
			sprintf(caTemp,"由于您使用了默认密码，因为安全的原因，系统自动把您的密码修改为：%s,请及时修改您的密码",caPassword_t);
			utPltPutVarF(psDbHead,"message", caTemp);
			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");                       
			printf("aaaaaaa\n");
		}	   
		//   printf("aaaaaaaaaaaaaaaaaaaaaaaaa\n");
		utPltPutVarF(psDbHead,"height","%d",atol(caScreen)-200);
		ncAdmSetAdmIp(psShmHead,caUsername,lIp);
		//        ncAdmWriteLog(caUsername,caIp,ncLang("0173成功登录到网络督察"));

		if(strcasecmp(utComGetVar_sd(psShmHead,"secondpasswd","No"),"YES") == 0){
			utPltPutVarF(psDbHead,"username",caUsername);
			dsCltSetSessionValue(1,"mbflag",UT_TYPE_LONG,4,10);
				//双密码验证
			utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_secondauth.htm");
			return 0;		
		}
		lDefCon = atol(utComGetVar_sd(psShmHead,"MbContDef","0"));

		lDef = atol(utComGetVar_sd(psShmHead,"MbLoginDef","0"));
		memset(caMbserno,0,sizeof(caMbserno));
		sprintf(caSql,"select mbserno,logflag,flag from wf_userbindmb where userid=%d ",lUserid);
		pasDbOneRecord(caSql,0,UT_TYPE_STRING,16,caMbserno,
								   UT_TYPE_LONG,4,&lLogflag,
								   UT_TYPE_LONG,4,&lFlag);

		if(strlen(caMbserno)>0){
			if(lFlag==0){
				dsCltSetSessionValue(1,"mbflag",UT_TYPE_LONG,4,10);
			}
			if(lLogflag==1){
				utPltPutVar(psDbHead,"fname","ncMbLogin");
				utPltPutVar(psDbHead,"mbserno",caMbserno);
					 
				ncsWriteSysLog(psMsgHead,caDispname,"01","Success",ncLang("0173成功登录到系统")); 
				  
				utPltPutVarF(psDbHead,"userid","%d",lUserid);
				utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
				return 0;
			}
		}else{
			if(lDefCon==0)
				dsCltSetSessionValue(1,"mbflag",UT_TYPE_LONG,4,10);
			if(lDef==1){
				utPltPutVarF(psDbHead,"message", ncLang("1555系统要求动态验证，但该用户没有绑定移动密宝"));
				utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/login2.htm");
				return 0;
			}
		}
		ncsWriteSysLog(psMsgHead,caDispname,"01","Success",ncLang("0173成功登录到系统")); 
	}
	if(strcmp(caUsername,"admin")){
		if(strlen(type_in)>0 && !strcmp(type_in,"1")){
			utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/download.html");
		}else{
			char pennName[32] = "";
			iReturn = dsCltGetMyInfo(1, "USERNAME", pennName);
			printf("type:%s\niReturn=[%d],pennName=[%s]\n",type_in, iReturn, pennName);
			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main_wifiapp.htm");
		}
	}else{
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/nwmanage.html");
	}
    return 0;
}

/* 增加或修改记录信息  */
int wf_add_update_user(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caUserid[16];
    char caUsername[32];
    char caPassword1[32],caPassword2[32];
    char caGroupname[32],caDispname[32],caGroupid[16];
    char caLevel[16],caMyname[32];
    char caEmail[64];
    char caKey[32],caKey0[64];
    char caSum[16];
    char caSql[8192];
    char caVar[32];
    char *p;
    char caTelphone[64];
    char caHandphone[32];
    char caUserflags[8];
    char caOpt1[256];
    char caAddress[128],caUseflags[16];
    unsigned long lTime,lUserid,lUsrlevel,lGroupid,lRoleid;
    int iReturn,i,iSum;
    char caKey2[64],caKey22[64];
    char caFac[256];
 	char caMsg[1024];
 	utPltDbHead *psDbHead;

    memset(caGroupname,0,sizeof(caGroupname));
    lTime = time(0); 
	utMsgPrintMsg(psMsgHead);
	printf(" Pid==%d \n",getpid());
   	iReturn = utMsgGetSomeNVar(psMsgHead,17,
				"userid",      UT_TYPE_STRING, 10, caUserid,
				"name",        UT_TYPE_STRING, 31, caUsername,
				"dispname",    UT_TYPE_STRING, 31, caDispname,
   				"password",    UT_TYPE_STRING, 31, caPassword1,
   				"password2",   UT_TYPE_STRING, 31, caPassword2,
   				"key2",        UT_TYPE_STRING,31,caKey2,
   				"key22",       UT_TYPE_STRING,31,caKey22,
   				"groupid",     UT_TYPE_STRING, 10, caGroupid,
   				"usrlevel",    UT_TYPE_STRING, 31, caLevel,
   				"email",       UT_TYPE_STRING, 63, caEmail,
   				"sumright",    UT_TYPE_STRING, 10, caSum,
   				"telphone",    UT_TYPE_STRING, 63, caTelphone,
   				"handphone",   UT_TYPE_STRING, 31,caHandphone,
   				"useflags",    UT_TYPE_STRING, 2, caUseflags,
   				"opt1",        UT_TYPE_STRING,255,caOpt1,
   				"address",     UT_TYPE_STRING,127,caAddress,
   				"fac",         UT_TYPE_STRING,255,caFac);
    lUserid = atol(caUserid);
    lGroupid = atol(caGroupid);
    iSum = atol(caSum);
    if(strlen(caLevel)==0){
		printf("calLevel len==0");
		strcpy(caLevel,"1");
    }
	memset(caMyname, 0, sizeof(caMyname));
    iReturn = dsCltGetMyInfo(1,"USERNAME",caMyname); 
	
	printf("after dsCltGetMyInfo username=[%s]", caMyname);
    
    sprintf(caSql,"select count(*) from wf_dsuser where name = '%s' ",caUsername);
    unsigned long count = 0;
    pasDbOneRecord(caSql,0,UT_TYPE_ULONG,sizeof(count),&count);
    
    if(count==0 || !utStrIsSpaces(caUserid)){
	    lUsrlevel = atol(caLevel);
	    printf("recv userlevel = %d", lUsrlevel);
	    if(iReturn < 0) {
	        strcpy(caMyname,"\0");
	    }
	    if(strcmp(caPassword1,caPassword2) !=0 ) {
	         utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0013增加用户"), ncLang("0014口令不相等"));
	         return 0;
	    }
	    if(strcmp(caKey2,caKey22) !=0 ) {
	         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0013增加用户"),ncLang("0015第二密码不相等"));
	         return 0;
	    }
	    
	    if(utStrIsSpaces(caUserid) || lUserid == 0) { /* 新增记录  */
	        lUserid = dsCltGetSeque();
			printf("after dsCltGetSeque userid=[%lu]", lUserid);
	        if(lUserid > 0) {
	            if(utStrIsSpaces(caPassword1)) {
	               utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0013增加用户"),ncLang("0016口令不能为空"));
	               return 0;
	            }
	            sprintf(caKey0,"%s%s",caUsername,caPassword1);
	            utMd5Ascii22(caKey0,strlen(caKey0),NULL,caKey);  /* 口令加密  */
	            sprintf(caSql,"<DSSQL>insert into wf_dsuser (id,name,dispname,groupname,addtime,\
	                              moditime,lkey,lasttime,logcount,useflags,status,usrlevel,email,opt) values \
	                           (%lu,'%s','%s','%s',%lu,\
	                           %lu,'%s',%lu,0,0,0,\
	                           %d,'%s','%s')</DSSQL>\n",
	                           lUserid,caUsername,caDispname,caGroupname,lTime,lTime,
	                           caKey,lTime,lUsrlevel,caEmail,caOpt1);
				
	            for(i=0;i<iSum;i++) {
	                sprintf(caVar,"v%d",i);
	                p = utMsgGetVar_s(psMsgHead,caVar);
	                if(p) {
	                    lRoleid = atol(p);
	                    sprintf(caSql+strlen(caSql),"<DSSQL>insert into wf_dsuserrole(id,roleid,addtime,addby) values \
	                              (%lu,%lu,%lu,'%s')</DSSQL>\n",
	                           lUserid,lRoleid,lTime,caMyname);
	                }
	            }
	           
				printf("here : %s\n",caSql);
	            iReturn = dsCltSqlExecSome(caSql);
	            if(iReturn == 0) {
	            	psDbHead = utPltInitDb();
	            	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncadminuserlist.htm");                     
	            }
	            else {
	            	printf("error here\n");       	
	           		utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0013增加用户"),ncLang("0017用户增加出错"));
	           		return 0;
	        		}
	        }else {       	
	           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0013增加用户"),ncLang("0017用户增加出错"));
	           return 0;
	        }
	    }else { /* 修改记录  */
	    	memset(caSql,0,sizeof(caSql));
	        utStrAddF(caSql,"<DSSQL>update wf_dsuser set name='%s',dispname='%s',groupid=%lu,\
	                            groupname='%s',opt='%s',usrlevel=%d",
	                            caUsername,caDispname,lGroupid,caGroupname,caOpt1,lUsrlevel);
			printf("update sql=[%s]\n",caSql);
	        if(!utStrIsSpaces(caPassword1)) {
	            sprintf(caKey0,"%s%s",caUsername,caPassword1);
	            utMd5Ascii22(caKey0,strlen(caKey0),NULL,caKey);  /* 口令加密  */
	            utStrAddF(caSql,",lkey='%s'",caKey);
	        }
	        if((!utStrIsSpaces(caKey2))&&(strcasecmp(utComGetVar_sd(psShmHead,"wzflag","No"),"Yes")==0)) {
	           
	            utStrAddF(caSql,",key2='%s'",caKey2);
	        }
	        
	        
	        utStrAddF(caSql,",moditime=%lu,lasttime=%lu,useflags=%lu,email='%s' where id=%lu </DSSQL>\n",
	                     lTime,lTime,atol(caUseflags),caEmail,lUserid);
	                    
	        utStrAddF(caSql,"<DSSQL>delete from wf_dsuserrole where id = %lu </DSSQL>\n",lUserid);
	        for(i=0;i<iSum;i++) {
	            sprintf(caVar,"v%d",i);
	            p = utMsgGetVar_s(psMsgHead,caVar);
	            if(p) {
	                lRoleid = atol(p);
	                utStrAddF(caSql,"<DSSQL>insert into wf_dsuserrole(id,roleid,addtime,addby) values \
	                          (%lu,%lu,%lu,'%s')</DSSQL>\n",
	                       lUserid,lRoleid,lTime,caMyname);
	            }
	        }
	       
	        iReturn = dsCltSqlExecSome(caSql);
	        if(iReturn == 0) {
	            psDbHead = utPltInitDb();
	            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncadminuserlist.htm");
	        }
	        else {
				utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0013增加用户"),ncLang("0018用户修改错误%d"),iReturn);
	
	        }            
	    }
	}else{
	  	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0013增加用户"),ncLang("0019用户名已存在"));
	}
    return 0;
}

/* 显示增加/修改用户的界面  */
int wf_show_edit_user(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    unsigned long lId,lGroupid,lTime,lTime1,lLevel,lUserid;
    char caName[32],caGroup[32],caDispname[32],caEmail[64],caUserid[16];
    char caWhere[256],caRoleid[16],caGroupid[16];
    char *p;
    unsigned long groupid;
    char groupname[32];
    char caResult[16000],caResult1[8192],caSmt[256];
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect,caLevel[12];
   	int iReturn,iRetSum,i,j,k,iSum1,iSum2,iNum,iReturn1,lUseflags;
   	pasDbCursor *psCur;
   	char caAddress[128];
   	char caTelphone[64],caHandphone[32],caOpt[256];
   	char caTemp[1024];
   	char opt_desc[1024];
   	char caUsername2[32];
   	char caFac[256],fac_desc[1024];
 	char caMsg[1024];
	
	utMsgPrintMsg(psMsgHead);
	
 	dsCltGetSessionValue(2,"dispname",UT_TYPE_STRING,30,caUsername2,"id",UT_TYPE_LONG,4,&lId);
	iReturn = utMsgGetSomeNVar(psMsgHead,1,
   	                "groupid",UT_TYPE_STRING,12,caGroupid);
    psDbHead = utPltInitDb();
    
    psCur = pasDbOpenSql("select gid,gname from wf_group order by gid desc ",0);
    iReturn = pasDbFetchInto(psCur,  UT_TYPE_LONG,4,&groupid,
      	                             UT_TYPE_STRING,31,groupname);
    iNum = 0; 	                               
    while((iReturn==0)||(iReturn==1405))
    {
        iNum++;       
        utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%lu",groupid);
        utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
        iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&groupid,
      	                               UT_TYPE_STRING,31,groupname);
    }
    pasDbCloseCursor(psCur);
   
   // printf("增加用户: here2\n");
    
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"userid",    UT_TYPE_STRING, 15, caUserid);
    lUserid = atol(caUserid);
    if(!utStrIsSpaces(caUserid)) { /* 修改 */
        lUserid = atol(caUserid);
        memset(caName,0,sizeof(caName));
        memset(caDispname,0,sizeof(caDispname));
        memset(caAddress,0,sizeof(caAddress));
        memset(caTelphone,0,sizeof(caTelphone));
        memset(caHandphone,0,sizeof(caHandphone));
        memset(caOpt,0,sizeof(caOpt));
        memset(caEmail,0,sizeof(caEmail));
        memset(caFac,0,sizeof(caFac));
        sprintf(caTemp,"select id,name,dispname,groupid,usrlevel,useflags,opt,email from wf_dsuser where id=%lu ",lUserid);
        pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lUserid,
        											 UT_TYPE_STRING,31,caName,
        											 UT_TYPE_STRING,31,caDispname,
        											 UT_TYPE_LONG,4,&lGroupid,
        											 UT_TYPE_LONG,4,&lLevel,
        											 UT_TYPE_LONG,4,&lUseflags,			
        											 UT_TYPE_STRING,254,caOpt,
        											 UT_TYPE_STRING,63,caEmail);
        											
        sprintf(caTemp,"select gname from wf_group where gid=%d ",lGroupid);
        memset(caGroup,0,sizeof(caGroup));
        pasDbOneRecord(caTemp,0,UT_TYPE_STRING,31,caGroup);
    
        if(lUserid != 0)
            utPltPutVar(psDbHead,"readonly","readonly");
        utPltPutVarF(psDbHead,"userid","%lu",lUserid);
        utPltPutVar(psDbHead, "name",  caName);
        utPltPutVar(psDbHead, "dispname",  caDispname);
        utPltPutVar(psDbHead, "groupname", caGroup);
        utPltPutVarF(psDbHead,"groupid","%lu",lGroupid);
        utPltPutVar(psDbHead, "email",     caEmail);
        
        if(lLevel==0)
			utPltPutVar(psDbHead,"rightlev",ncLang("0009系统管理员"));
        else
			utPltPutVar(psDbHead,"rightlev",ncLang("0010一般操作员"));
        
        utPltPutVarF(psDbHead,"level","%d",lLevel);
		//增加用户级别的显示
		utPltPutVarF(psDbHead,"usrlevel","%d",lLevel);
        utPltPutVarF(psDbHead,"useflags","%d",lUseflags);
        utPltPutVar(psDbHead,"address",caAddress);
        utPltPutVar(psDbHead,"telphone",caTelphone);
        utPltPutVar(psDbHead,"handphone",caHandphone);
        utPltPutVar(psDbHead,"opt",caOpt);
        utPltPutVar(psDbHead,"fac",caFac);
        if(strstr(caOpt,"all")){
        	utPltPutVar(psDbHead,"opt_desc",ncLang("0011全部"));
        }
        else{
        	if(strlen(caOpt)>0){
                sprintf(caTemp,"select gname from wf_group where gid in (%s) ",caOpt);
				psCur = pasDbOpenSql(caTemp,0);
				if(psCur){
                    iReturn = pasDbFetchInto(psCur,
      	                        UT_TYPE_STRING,31,groupname);
                    iNum = 0; 	                               
                    while((iReturn==0)||(iReturn==1405))
                    {
  						if(iNum==0)
  							sprintf(opt_desc,"%s",groupname);
  						else
  							sprintf(opt_desc+strlen(opt_desc),",%s",groupname);
						iReturn = pasDbFetchInto(psCur,
      	                        UT_TYPE_STRING,31,groupname);
						iNum++;
                    }
                    pasDbCloseCursor(psCur);
                    utPltPutVar(psDbHead,"opt_desc",opt_desc);
                }
            }
        }       
        utPltPutVar(psDbHead,"email",caEmail);       
    }
    else { /* 增加新记录  */
    	sprintf(caTemp,"select usrlevel from wf_dsuser where id=%d ",lId);
		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lLevel);
        utPltPutVar(psDbHead,"groupid",caGroupid);
        utPltPutVar(psDbHead,"useflags","1");
        utPltPutVar(psDbHead,"opt_desc",ncLang("0011全部"));
        utPltPutVar(psDbHead,"opt","all");
        utPltPutVar(psDbHead,"rightlev",ncLang("0010一般操作员"));
        utPltPutVarF(psDbHead,"level","%d",lLevel);
    }
    
	char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	findShortName(auth,shortname);
    
    char sql[128] = "";
    sprintf(sql,"select keyname,keyvalue from wf_paramsmanage where paramtype=1 and company='%s' order by keyvalue",shortname);
	
	printf("参数设置：sql:%s\n",sql);
	psCur = pasDbOpenSql(sql, 0);
	
	if(psCur!=NULL){
		char keyname[32] = "";
		char keyvalue[32] = "";
		
		int iret = 0,iNum = 0;
		while(0==(iret = pasDbFetchInto(psCur,
			 UT_TYPE_STRING, sizeof(keyname)-1,  keyname,
			 UT_TYPE_STRING, sizeof(keyvalue)-1, keyvalue)) || 1405==iret){
			
			iNum++;
			if(iNum>1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}else{
				utPltPutLoopVar(psDbHead,"selected",iNum,"selected");
			}
			utPltPutLoopVar(psDbHead,"rightn",iNum,keyname);
			utPltPutLoopVar(psDbHead,"rightv",iNum,keyvalue);
		}
		
		pasDbCloseCursor(psCur);
	}
    
    /* 显示角色信息  */
    if(lUserid == 0) {
        iSum1 = 0;
    }
    else {
        sprintf(caSmt,"select roleid from wf_dsuserrole where id= %lu ",lUserid);
        
        iReturn = dsCltSqlRecordSet(caSmt,1,9999,&iSum1,caResult1);
        if(iReturn != 0) iSum1 = 0;
    }
    char caMyname[32] = "";
    dsCltGetMyInfo(1,"USERNAME",caMyname);
    sprintf(caSmt,"select id,dispname from wf_dsrole where addby = '%s'",caMyname);
    
    printf("增加用户: here4%s\n",caSmt);
    iReturn = dsCltSqlRecordSet(caSmt,1,9999,&iSum2,caResult);
    if(iReturn != 0) iSum2 = 0;
    iNum = 0;
    j = 0;

    //printf("增加用户: here4\n");
    p = caResult;
    char checked[8] = "";
    utPltPutVarF(psDbHead,"sumrole", "%d",iSum2);
    for(k=0;k<iSum2;k++) {
        p = utStrGetRecordValue(p,2,
                    UT_TYPE_STRING,12,caRoleid,
                    UT_TYPE_STRING,31,caDispname);
        if(j==0) {
            iNum++;
            if(iSum1>0 && utStrIsExistRecord(caResult1,1,1,caRoleid)) {
                strcpy(checked,"checked");
            }
            sprintf(caTemp,"<td  valign='bottom' height='21'> <div align='right'> <input type='checkbox' name='v%d' value='%s' %s></div></td><td  valign='bottom' height='21'>%s</td>",k,caRoleid,checked,caDispname);
            printf("caTemp=%s\n",caTemp);
            utPltPutLoopVar(psDbHead,"vara",iNum,caTemp);
            
            j++;
        }
        else if(j==1) {
          if(iSum1>0 && utStrIsExistRecord(caResult1,1,1,caRoleid)) {
              strcpy(checked,"checked");
          }
          sprintf(caTemp,"<td  valign='bottom' height='21'> <div align='right'> <input type='checkbox' name='v%d' value='%s' %s></div></td><td  valign='bottom' height='21'>%s</td>",k,caRoleid,checked,caDispname);
          printf("caTemp=%s\n",caTemp);
          utPltPutLoopVar(psDbHead,"varb",iNum,caTemp);
            j=0;
        }
    }
    
    //printf("增加用户: here5\n");
    utPltPutVarF(psDbHead,"sumright","%lu",iSum2);
    if(j==1) {
        utPltPutLoopVar(psDbHead,"roleb", iNum,"noneb");
        utPltPutLoopVar(psDbHead,"dispnameb", iNum,ncLang("0012预留"));
        utPltPutLoopVar(psDbHead,"checkb", iNum,"disabled");
    }

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"v4/ncadminuser.htm");
    return 0;
}

//显示系统用户管理
int wf_get_system_users(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	pasDbCursor *psCur;
	utPltDbHead *psDbHead;
	long iReturn;
	char caTemp[2048];
	long lRowNum;
	long status;
	char caName[64];
	long iNum,lCurPg,lStartRec,lCount;
	long lTime,lLevel;
	char caOpt[256];
	char caTelphone[64],caHandphone[32],caGroup[32],caEmail[64],caDispname[32];
	char caGroupid[32],caLimit[16],caPage[16],caDel[16],caSort[20],caDir[16],caKeyword[64],caSelsid[1024],caStart[16]; 
    long lId=0;

 	   
	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,10,
			"groupid",      UT_TYPE_STRING,  	12,		caGroupid,
			"limit",        UT_TYPE_STRING,  	10, 	caLimit,
			"page",         UT_TYPE_STRING,  	10, 	caPage,  
			"del",          UT_TYPE_STRING,		8,		caDel,
			"sort",         UT_TYPE_STRING,		15,		caSort,
			"dir",          UT_TYPE_STRING,		8,		caDir,
			"keyword",      UT_TYPE_STRING,		30,		caKeyword,
			"selsid",       UT_TYPE_STRING,		1000,	caSelsid,			             
			"start",        UT_TYPE_STRING,		10,		caStart,
			"del",			UT_TYPE_STRING,		10,		caDel);  
            
	
	lRowNum = atol(caLimit);
	lCurPg	= atol(caPage);
	if(lCurPg<=0)
		lCurPg=1;	
	lStartRec = atol(caStart);
	printf("lCurPg=%d, lRowNum");
		
	psDbHead = utPltInitDb(); 
	//先检查是否是删除请求
	if(strcmp(caDel,"del")==0 && strlen(caSelsid)>0){
		int userid = 0;
		char lUserId[24] = "";
		char rUserId[24] = "";
		char mUserId[24] = "";
		iReturn = dsCltGetMyInfo(1,"Userid",&userid);
		if(iReturn != 0){
			printf("dsCltGetMyInfo failed!!");
			return 0;
		}
		snprintf(lUserId, sizeof(lUserId)-1, "%d,", userid);
		snprintf(rUserId, sizeof(rUserId)-1, ",%d", userid);
		snprintf(mUserId, sizeof(mUserId)-1, "%d", userid);
		//检查有没有删除自身或者admin
		if(strncmp(mUserId, caSelsid, strlen(mUserId)) == 0){
			printf("\ndelete self is not allowed\n");
			return 0;
		}else{
			if(strstr(lUserId, caSelsid)){
				printf("\ndelete self is not allowed\n");
				return 0;
			}
			if(strstr(rUserId, caSelsid)){
				printf("\ndelete self is not allowed\n");
				return 0;
			}
		}		
	  	printf("[delete from  wf_dsuser  where id in(%s)]\n",caSelsid);
	  	pasDbExecSqlF("delete from  wf_dsuser  where id in(%s)",caSelsid);
	}   
	//统计总数
	sprintf(caTemp,"select count(*) from wf_dsuser where 1=1 ");
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);	
	//开始查询
	sprintf(caTemp,"select id, name, dispname, groupname, email, addtime, usrlevel, status from wf_dsuser ");    
    sprintf(caTemp+strlen(caTemp)," order by id desc limit %d,%d", lStartRec, lRowNum);    	
  	printf("sql=[%s]\n",caTemp); 	
    psCur = pasDbOpenSql(caTemp,0);
   
    if(psCur == NULL) {
		printf("psCur == null");
        return 0;
    }
                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) 
	{
		memset(caName,0,sizeof(caName));
		memset(caDispname,0,sizeof(caDispname));
		memset(caGroup,0,sizeof(caGroup));
		memset(caHandphone,0,sizeof(caHandphone));
		memset(caTelphone,0,sizeof(caTelphone));
		memset(caEmail,0,sizeof(caEmail));
		memset(caOpt,0,sizeof(caOpt));
		status = 0;
		iReturn=pasDbFetchInto(psCur,
			UT_TYPE_LONG,4,&lId,
            UT_TYPE_STRING,31,caName,
            UT_TYPE_STRING,31,caDispname,
            UT_TYPE_STRING,31,caGroup,
            UT_TYPE_STRING,63,caEmail,
			UT_TYPE_LONG, 4, &lTime,
            UT_TYPE_LONG, 4, &lLevel,                              
            UT_TYPE_LONG, 4, &status);
                                   
      	                              
		if((iReturn!=0)&&(iReturn!=1405)) 
		{
			printf("break for iReturn=%d"+iReturn);
			break;	
		}
			
        iNum++;
		printf("iNum=%d\n", iNum);
		if(iNum>1){
			utPltPutLoopVar(psDbHead,"dh",iNum,",");
		}
        utPltPutLoopVarF(psDbHead,"num", iNum,"%lu",iNum);
        utPltPutLoopVarF(psDbHead,"userid", iNum,"%lu",lId);
        utPltPutLoopVar(psDbHead,"username",iNum,caName);
        utPltPutLoopVar(psDbHead,"dispname",iNum,caDispname);
        utPltPutLoopVar(psDbHead,"groupname",iNum,caName);
        utPltPutLoopVar(psDbHead,"email",iNum,caEmail);
        utPltPutLoopVar(psDbHead,"addtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lTime));
		if(status == 0){
			utPltPutLoopVar(psDbHead,"status",iNum, "正常");
		}else{
			utPltPutLoopVar(psDbHead,"status",iNum, "禁用");
		}
		
		if(lLevel == 0){
			utPltPutLoopVar(psDbHead,"level",iNum, "信息录入");
		}else{
			utPltPutLoopVar(psDbHead,"level",iNum, "系统维护");
		}
        utPltPutLoopVar(psDbHead,"telphone",iNum,caTelphone);
        utPltPutLoopVar(psDbHead,"handphone",iNum,caHandphone);
        //utPltPutLoopVar(psDbHead,"opt", iNum, ncLang("0011全部"));
		if(iNum+lStartRec>=lCount){
			break;
		}
	}
    pasDbCloseCursor(psCur);  
	utPltPutVarF(psDbHead,"TotRec","%d", lCount);	   
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v4/ncs_sysuser_list.htm");
	return 0;
}

int wf_get_wifi_from_bssid(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
	utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();
	char client_bssid[32]="";
	char ssid[128]="";
	char bssid[32]="";
	char zerocode[32]="";
	char lat[32] = "";
	char lng[32] = "";
	char moduser[24]="";
	long modtime = 0;
	char apmac[24]="";
	char sql[1024]="";
	int iReturn=0;
	iReturn=utMsgGetSomeNVar(psMsgHead, 1,   	   
					"bssid",	UT_TYPE_STRING, sizeof(client_bssid)-1,	client_bssid);
	printf("get client_bssid=[%s]", client_bssid);			
	snprintf(sql,sizeof(sql)-1,"select ssid,bssid,zerocode,lat,lng,moduser,modtime,apmac from wf_center_ap where bssid='%s'", client_bssid);
	printf("sql=[%s]",sql);
	iReturn = pasDbOneRecord(sql, 0, 
		UT_TYPE_STRING, 	sizeof(ssid),    	ssid,
		UT_TYPE_STRING, 	sizeof(bssid),   	bssid,
		UT_TYPE_STRING, 	sizeof(zerocode),	zerocode,
		UT_TYPE_STRING, 	sizeof(lat),        lat,
		UT_TYPE_STRING, 	sizeof(lng),        lng,
		UT_TYPE_STRING,     sizeof(moduser), 	moduser,
		UT_TYPE_LONG,		sizeof(modtime),    &modtime,
		UT_TYPE_STRING,     sizeof(apmac),		apmac);
	
	printf("iReturn=[%d]", iReturn);
	if(iReturn&&(iReturn != PAS_DB_NULLVALUE))
	{
		//没找到记录
		utPltPutVar(psDbHead, "result", "0");
	}else{
		utPltPutVar(psDbHead, "result", "1");
		utPltPutVar(psDbHead, "ssid", ssid);
		utPltPutVar(psDbHead, "bssid", bssid);
		utPltPutVar(psDbHead, "zerocode", zerocode);
		utPltPutVar(psDbHead, "lat", lat);
		utPltPutVar(psDbHead, "lng", lng);
		utPltPutVar(psDbHead, "moduser", moduser);
		char realTime[64]="";
		time_t lt=(time_t)modtime;
		struct tm* ptm = localtime(&lt);  
		strftime(realTime,  sizeof(realTime), "%H:%M:%S %Y/%m/%d", ptm);
		utPltPutVar(psDbHead, "modtime", realTime);
		utPltPutVar(psDbHead, "apmac", apmac);
	}
	utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/wifi/wifi_check_bssid.htm");
	return 0;
}

int wf_get_city_zerocode(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();
	char province_code[64] = "";
	char sql[1024]="";
	int iReturn=0;
	iReturn=utMsgGetSomeNVar(psMsgHead,1,   	   
					"province_code",        UT_TYPE_STRING,  sizeof(province_code)-1,      province_code);
	
	snprintf(sql,sizeof(sql)-1,"select city_code, city_name from wf_city_code where province_code=%s", province_code);
	int iNum=0;
	int iret = 0;
	pasDbCursor *psCur = NULL;
	printf("sql=[%s]\n",sql);
	psCur = pasDbOpenSql(sql, 0);
	if(psCur!=NULL){
		long city_code = 0;
		char city_name[64]="";
		while(0==(iret=pasDbFetchInto(psCur,		
			UT_TYPE_LONG,           4,                          &city_code,
			UT_TYPE_STRING,			sizeof(city_name),			city_name)) || 1405 == iret){

			iNum++;
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead, "city_code", iNum, "%d", city_code);
			utPltPutLoopVar(psDbHead, "city_name", iNum, city_name);
			
			//清空容器
			city_code = 0;
			memset(city_name, 0, sizeof(city_name));
		}	
	}
	pasDbCloseCursor(psCur);
	utPltPutVarF(psDbHead,"totalCount","%lu",iNum);
	printf("final iNum = %lu\n",iNum);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/wifi/wifi_city_list.htm");
	return 0;
}

int wf_get_province_code(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();
	char zerocode[32]="";
	char sql[1024]="";
	int iReturn=0;
	iReturn=utMsgGetSomeNVar(psMsgHead,1,   	   
					"zerocode",        UT_TYPE_STRING, sizeof(zerocode)-1,      zerocode);
	
	snprintf(sql,sizeof(sql)-1,"select province_code, province_name from wf_province_code");
	int iNum=0;
	int iret = 0;
	pasDbCursor *psCur = NULL;
	printf("%s\n",sql);
	psCur = pasDbOpenSql(sql, 0);
	if(psCur!=NULL){
		long province_code = 0;
		char province_name[64]="";
		while(0==(iret=pasDbFetchInto(psCur,		
			UT_TYPE_LONG,           4,                          &province_code,
			UT_TYPE_STRING,			sizeof(province_name),			province_name)) || 1405 == iret){

			iNum++;
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead, "province_code", iNum, "%d", province_code);
			utPltPutLoopVar(psDbHead, "province_name", iNum, province_name);
			
			//清空容器
			province_code = 0;
			memset(province_name, 0, sizeof(province_name));
		}	
	}
	pasDbCloseCursor(psCur);
	utPltPutVarF(psDbHead,"totalCount","%lu",iNum);
	printf("final iNum = %lu\n",iNum);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/wifi/wifi_province_list.htm");
	return 0;
}

void completezeroInfo(char province_name[128], char city_name[64], char zerocode[32])
{
	printf("raw [%s]", province_name);
	sprintf(province_name+strlen(province_name), " %s", city_name);
	printf("middle [%s]", province_name);
	sprintf(province_name+strlen(province_name), " %s", zerocode);
	printf("final [%s]", province_name);
}

int wf_query_wifi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();
	char searchfmt[32]="";
	char sql[1024]="";
	char bdate_in[24] = "";
	char edate_in[24] = "";
	int iReturn=0;
	char auditFlag[24] = "";
	char caAudit[24] = "";
	char caSelsid[1024] = "";
	
	utMsgGetSomeNVar(psMsgHead,6,
		"searchfmt",        UT_TYPE_STRING, 	sizeof(searchfmt)-1,      	searchfmt,
		"sdate",  			UT_TYPE_STRING, 	sizeof(bdate_in)-1,   		bdate_in,
		"edate",  			UT_TYPE_STRING, 	sizeof(edate_in)-1,   		edate_in,
		"auditFlag",        UT_TYPE_STRING,   	sizeof(auditFlag)-1,        auditFlag,
		"selsid",           UT_TYPE_STRING,		sizeof(caSelsid)-1,	        caSelsid,	
		"audit",            UT_TYPE_STRING,     sizeof(caAudit)-1,          caAudit);
		
			
	//先检查是否是审核请求
	if(strcmp(caAudit,"audit")==0 && strlen(caSelsid)>0){
	  	printf("[update wf_center_ap set auditFlag='1'  where bssid in(%s)]\n", caSelsid);
	  	pasDbExecSqlF("update wf_center_ap set auditFlag='1'  where bssid in(%s)", caSelsid);
	} 
	
	unsigned long stime=utTimStrToLong("%Y-%m-%d", bdate_in);
	unsigned long etime=utTimStrToLong("%Y-%m-%d", edate_in)+86400;
	
	snprintf(sql,sizeof(sql)-1,"select a.recno, a.ssid, a.zerocode, a.bssid, a.passwd, a.mobid, a.type, a.modtime, a.content, a.passwdtype, a.auditFlag, a.lat, a.lng, a.apmac, a.moduser, d.city_name, d.province_name from wf_center_ap a left join (select b.city_code,b.city_name,c.province_name from wf_city_code b left join wf_province_code c on b.province_code = c.province_code ) d on a.zerocode = d.city_code where 1=1 ");
	if(!utStrIsSpaces(searchfmt)){
		sprintf(sql+strlen(sql)," and (zerocode like '%c%s%c'", '%', searchfmt, '%');
		sprintf(sql+strlen(sql)," or ssid like '%c%s%c'", '%', searchfmt, '%');
		sprintf(sql+strlen(sql)," or bssid like '%c%s%c'", '%', searchfmt, '%');
		sprintf(sql+strlen(sql)," or apmac like '%c%s%c'", '%', searchfmt, '%');
		sprintf(sql+strlen(sql)," or moduser like '%c%s%c')", '%', searchfmt, '%');
	}
	if(!utStrIsSpaces(bdate_in)){
		sprintf(sql+strlen(sql), " and modtime > %lu", stime);
	}
	if(!utStrIsSpaces(edate_in)){
		sprintf(sql+strlen(sql), " and modtime < %lu", etime);
	}
	printf("recv auditFlag=[%s]", auditFlag);
	int sqlAuditFlag = 0;
	char recvAuditFlag = auditFlag[0];
	//auditFlag 1---全部, 2---未审核, 3---审核
	if(recvAuditFlag!= '1'){
		if(recvAuditFlag == '2'){
			sqlAuditFlag = 0;
		}else if(recvAuditFlag == '3'){
			sqlAuditFlag = 1;
		}else{
			sqlAuditFlag = 2;
		}
		sprintf(sql+strlen(sql), " and auditFlag = %d", sqlAuditFlag);
	}
	
	int iNum=0;
	int iret = 0;
	pasDbCursor *psCur = NULL;
	printf("[%s]\n", sql);
	psCur = pasDbOpenSql(sql, 0);
	
	if(psCur!=NULL){
		long recno = 0;
		char zerocode[32]="";
		char ssid[128]="";
		char bssid[32]="";
		char passwd[24]="";
		char mobid[128]="";
		char type[48]="";
		long modtime=0;
		char content[128]="";
		long passwdtype = 0;
		char lat[32]="";
		char lng[32]="";
		char apmac[24]="";
		time_t lt;
		char realTime[64]="";
		char moduser[24]="";
		char city_name[64]="";
		char province_name[128]="";
		while(0==(iret=pasDbFetchInto(psCur,		
			UT_TYPE_LONG,           4,                          &recno,
			UT_TYPE_STRING,			sizeof(ssid),				ssid,
			UT_TYPE_STRING,			sizeof(zerocode),		    zerocode,
			UT_TYPE_STRING,			sizeof(bssid),				bssid,
			UT_TYPE_STRING,			sizeof(passwd),				passwd,
			UT_TYPE_STRING,			sizeof(mobid),				mobid,
			UT_TYPE_STRING,			sizeof(type),				type,
			UT_TYPE_LONG,			4,							&modtime,
			UT_TYPE_STRING,			sizeof(content),			content,
			UT_TYPE_LONG,			4,							&passwdtype,
			UT_TYPE_LONG,			4,							&sqlAuditFlag,			
			UT_TYPE_STRING,			sizeof(lat),				lat,
			UT_TYPE_STRING,			sizeof(lng),				lng,
			UT_TYPE_STRING,			sizeof(apmac),				apmac,
			UT_TYPE_STRING,			sizeof(moduser),			moduser,
			UT_TYPE_STRING,			sizeof(city_name),			city_name,
			UT_TYPE_STRING,			sizeof(province_name),		province_name)) || 1405 == iret){

			iNum++;
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVarF(psDbHead, "recno", iNum, "%d", recno);
			utPltPutLoopVar(psDbHead, "ssid", iNum, ssid);
			//根据区域号码查询真实名
			completezeroInfo(province_name, city_name, zerocode);
			utPltPutLoopVar(psDbHead, "zerocode", iNum, province_name);
			utPltPutLoopVar(psDbHead, "bssid", iNum, bssid);
			utPltPutLoopVar(psDbHead, "passwd", iNum, passwd);
			utPltPutLoopVar(psDbHead, "mobid", iNum, mobid);
			utPltPutLoopVar(psDbHead, "type", iNum, type);
			
			lt=(time_t)modtime;
			struct tm* ptm = localtime(&lt);  
			//sprintf(realTime, "%s\n", asctime(localtime(&lt)));
			strftime(realTime,  sizeof(realTime), "%H:%M:%S %Y/%m/%d", ptm);
			utPltPutLoopVar(psDbHead, "modtime", iNum, realTime);			
			utPltPutLoopVar(psDbHead, "content", iNum, content);
			utPltPutLoopVarF(psDbHead, "passwdtype", iNum, "%d", passwdtype);		
			if(sqlAuditFlag == 1){
				utPltPutLoopVar(psDbHead, "auditFlag", iNum, "已审核");
			}else{
				utPltPutLoopVar(psDbHead, "auditFlag", iNum, "未审核");
			}
			utPltPutLoopVar(psDbHead,"lat",iNum, lat);
			utPltPutLoopVar(psDbHead,"lng",iNum, lng);
			utPltPutLoopVar(psDbHead, "apmac", iNum, apmac);
			utPltPutLoopVar(psDbHead, "moduser", iNum, moduser);
			
			//清空容器
			recno = 0;
			memset(ssid, 0, sizeof(ssid));
			memset(bssid, 0, sizeof(bssid));
			memset(passwd, 0, sizeof(passwd));
			memset(mobid, 0, sizeof(mobid));
			memset(type, 0, sizeof(type));
			modtime = 0;
			memset(content, 0, sizeof(content));
			passwdtype = 0;
			sqlAuditFlag = 0;
			memset(lat, 0, sizeof(lat));
			memset(lng, 0, sizeof(lng));
			memset(apmac, 0, sizeof(apmac));
			memset(moduser, 0, sizeof(moduser));
			memset(realTime, 0, sizeof(realTime));
			memset(city_name, 0, sizeof(city_name));
			memset(province_name, 0, sizeof(province_name));
		}	
	}
	pasDbCloseCursor(psCur);
	
	utPltPutVarF(psDbHead,"TotRec","%lu",iNum);
	printf("final iNum = %lu\n",iNum);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/wifi/wifi_query_list.htm");
	return 0;
}

int getbssid(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();

	char zerocode[32]="";
	char APP_SRC[15]="";
	char sql[1024]="";
	
	int iReturn=0;
	iReturn=utMsgGetSomeNVar(psMsgHead,2,   	   
					"zerocode",      UT_TYPE_STRING, sizeof(zerocode)-1,      zerocode,
					"APP_SRC",		 UT_TYPE_STRING, sizeof(APP_SRC)-1,       APP_SRC);


	snprintf(sql,sizeof(sql)-1,"select bssid,lat,lng from wf_center_ap where zerocode='%s'",zerocode);
	int iNum=0;
	int iret = 0;
	pasDbCursor *psCur = NULL;
	printf("sql=[%s]\n",sql);
	psCur = pasDbOpenSql(sql, 0);
	char tmpBssid[24]="";
	char tmpLat[32]="";
	char tmpLng[32]="";
	if(psCur!=NULL){
		memset(tmpBssid,0,sizeof(tmpBssid));
		memset(tmpLat,0,sizeof(tmpLat));
		memset(tmpLng,0,sizeof(tmpLng));
		while(0==(iret=pasDbFetchInto(psCur,
			UT_TYPE_STRING,			sizeof(tmpBssid),		tmpBssid,
			UT_TYPE_STRING,			sizeof(tmpLat),		    tmpLat,
			UT_TYPE_STRING,			sizeof(tmpLng),			tmpLng)) || 1405 == iret)
		{
			iNum++;
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			utPltPutLoopVar(psDbHead, "bssid",iNum, tmpBssid);
			utPltPutLoopVar(psDbHead, "lat", iNum, tmpLat);
			utPltPutLoopVar(psDbHead, "lng", iNum, tmpLng);
		}	
		utPltPutVarF(psDbHead, "status", "%d", 0);
	}else{
		utPltPutVarF(psDbHead, "status", "%d", 1);
	}
	pasDbCloseCursor(psCur);
	utPltPutVarF(psDbHead, "retlen", "%lu", iNum);
	//utMsgPrintMsg(psDbHead);
	printf("iNum=[%lu]\n", iNum);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/wifi/wifi_get_bssid.htm");
}

int getapmacfrombssid(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
	utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();
	char ssid[128]="";
	char bssid[32]="";
	char APP_SRC[15]="";
	char apmac[24]="";
	char sql[1024]="";
	int iReturn=0;
	char queryBssid[24]="";
	iReturn=utMsgGetSomeNVar(psMsgHead, 3,   	   
					"ssid",		UT_TYPE_STRING, sizeof(ssid)-1,		ssid,
					"bssid",	UT_TYPE_STRING, sizeof(bssid)-1,	bssid,
					"APP_SRC",	UT_TYPE_STRING, sizeof(APP_SRC)-1,	APP_SRC);
					
	snprintf(sql,sizeof(sql)-1,"select bssid,apmac from wf_center_ap where bssid='%s'", bssid);
	printf("sql=[%s]",sql);
	iReturn = pasDbOneRecord(sql, 0, 
		UT_TYPE_STRING, 24, queryBssid,
		UT_TYPE_STRING, 24, apmac);
	
	printf("iReturn=[%d]", iReturn);
	if(iReturn&&(iReturn != PAS_DB_NULLVALUE))
	{
		utPltPutVar(psDbHead, "status", "1");
	}else{
		if(strncmp(bssid, queryBssid, sizeof(bssid)-1) == 0){
			utPltPutVar(psDbHead, "apmac", apmac);
			utPltPutVar(psDbHead, "status", "0");
		}else{	 
			utPltPutVar(psDbHead, "status", "1");
		}
	}
	utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/wifi/wifi_get_apmac_from_bssid.htm");
	return 0;
}

int wf_share_wifi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	utMsgPrintMsg(psMsgHead);
	utPltDbHead *psDbHead = utPltInitDb();
	char ssid[128]="";
	char zerocode[32]="";
	char bssid[32]="";
	char apmac[24]="";
	char passwd[24]="";
	char mobid[128]="";
	char type[48]="";
	char content[128]="";
	int iReturn=0;
	char sql[1024]="";
	int lCount=0;
	char lat[11]="";
	char lng[11]="";
	char passwdtype[11]="";
	
	int userid = 0;
	char caMyname[32] = "";
    iReturn = dsCltGetMyInfo(1, "USERNAME", caMyname);
	//iReturn = dsCltGetMyInfo(1, "Userid", &userid);
	printf("\nafter dsCltGetMyInfo caMyname=[%s],iReturn=[%d]\n", caMyname, iReturn);
	iReturn = dsCltGetMyInfo(1,"Userid",&userid);
	printf("\nafter dsCltGetMyInfo userid=[%d],iReturn=[%d]\n", userid, iReturn);
	
	iReturn=utMsgGetSomeNVar(psMsgHead,11,   	   
					"ssid",            UT_TYPE_STRING, sizeof(ssid)-1,          ssid,
					"zerocode",        UT_TYPE_STRING, sizeof(zerocode)-1,      zerocode,
					"passwd",		   UT_TYPE_STRING, sizeof(passwd)-1,        passwd,
					"mobid",		   UT_TYPE_STRING, sizeof(mobid)-1,         mobid,
					"type",			   UT_TYPE_STRING, sizeof(type)-1,          type,
					"passwdtype",      UT_TYPE_STRING, sizeof(passwdtype)-1,    passwdtype,
					"content",		   UT_TYPE_STRING, sizeof(content)-1,       content,
					"lat",			   UT_TYPE_STRING, sizeof(lat)-1,             lat,
					"lng",			   UT_TYPE_STRING, sizeof(lng)-1,             lng,
					"apmac",            UT_TYPE_STRING, sizeof(apmac)-1,           apmac,
					"bssid",           UT_TYPE_STRING, sizeof(bssid)-1,         bssid);
	snprintf(sql,sizeof(sql)-1,"select count(*) from wf_center_ap where bssid='%s'",bssid);
	printf("%s",sql);
	pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
	//printf("----------------%d----------------",lCount);
	if(lCount>=1){
		 utPltPutVar(psDbHead,"buff","{success:false,mesg:\'WIFI已被分享\'}");
    	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
		 return 0;
	}else{
		time_t local;
		time(&local);
		memset(sql,0,sizeof(sql)-1);
		snprintf(sql,sizeof(sql)-1,"insert into wf_center_ap (ssid,zerocode,bssid,apmac,passwd,mobid,type,modtime,moduser, content,passwdtype,lat,lng) values('%s','%s','%s','%s','%s','%s','%s',%d,'%s','%s',%s,%s,%s)",ssid,zerocode,bssid,apmac,passwd,mobid,type,local,caMyname,content,passwdtype,lat,lng);
		printf("sql=[%s]",sql);
		pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
		utPltPutVar(psDbHead,"buff","{success:true,mesg:\'WIFI分享成功\'}");
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	}
	return 0;


 }
// 求弧庿
double radian(double d)
{
    return d * PI / 180.0;   //角度1? = π / 180
}

//计算距离 单位为千籿
double get_distance(double lat1, double lng1, double lat2, double lng2)
{
    double radLat1 = radian(lat1);
    double radLat2 = radian(lat2);
    double a = radLat1 - radLat2;
    double b = radian(lng1) - radian(lng2);

    double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));

    dst = dst * EARTH_RADIUS;
    dst= round(dst * 10000) / 10000;
    return dst;
}

 int wf_getnearby_wifi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  utMsgPrintMsg(psMsgHead);
  utPltDbHead *psDbHead = utPltInitDb();

	 char zerocode[32]="";
	 char lat[32]="";
	 char lng[32]="";
	 char sql[1024]="";
	 char range[32]="";
	
	 int iReturn=0;
  iReturn=utMsgGetSomeNVar(psMsgHead,4,   	   
					"zerocode",        UT_TYPE_STRING, sizeof(zerocode)-1,      zerocode,
					"range",		   UT_TYPE_STRING, sizeof(range)-1,			range,
					"lat",			   UT_TYPE_STRING, sizeof(lat)-1,             lat,
					"lng",			   UT_TYPE_STRING, sizeof(lng)-1,             lng);


snprintf(sql,sizeof(sql)-1,"select ssid,bssid,lat,lng from wf_center_ap where zerocode='%s'",zerocode);
int iNum=0;
int iret = 0;
pasDbCursor *psCur = NULL;
//printf("%s\n",sql);
psCur = pasDbOpenSql(sql, 0);
if(psCur!=NULL){
			char tempssid[32]="";
			char tempbssid[32]="";
			char tempx[32]="";
			char tempy[32]="";
			while(0==(iret=pasDbFetchInto(psCur,
				UT_TYPE_STRING,			sizeof(tempssid),			tempssid,
				UT_TYPE_STRING,			sizeof(tempbssid),		    tempbssid,
				UT_TYPE_STRING,			sizeof(tempx),				tempx,
				UT_TYPE_STRING,			sizeof(tempy),				tempy
				)) || 1405 == iret){
				//float a=atof(lat);
				//printf("----------------x=%lf----y=%lf-----tempx=%lf----tempy=%lf----------",a,atof(lng),tempx,tempy);
				printf("1111111111\n");
			double distance=get_distance(atof(lat),atof(lng),atof(tempx),atof(tempy))*1000;
			//printf("------------------%lf---------------------",distance);
			if(distance>atof(range)){
				continue ;
			}
				//printf("2222222222\n");
			iNum++;
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			//printf("3333333333\n");
			utPltPutLoopVar(psDbHead,"ssid",iNum,tempssid);
			utPltPutLoopVar(psDbHead,"bssid",iNum,tempbssid);
			utPltPutLoopVar(psDbHead,"lat",iNum,tempx);
			utPltPutLoopVar(psDbHead,"lng",iNum,tempy);
			memset(tempssid,0,sizeof(tempssid)-1);
			memset(tempbssid,0,sizeof(tempbssid)-1);
			memset(tempy,0,sizeof(tempy)-1);
			memset(tempx,0,sizeof(tempx)-1);
			}
	
	}
	pasDbCloseCursor(psCur);
	utPltPutVarF(psDbHead,"TotRec","%lu",iNum);
	//utMsgPrintMsg(psDbHead);
	printf("%lu\n",iNum);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/wifi/wifi_list_nearby.htm");
  return 0;

}

int wf_checkuser_vip(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  utMsgPrintMsg(psMsgHead);
  utPltDbHead *psDbHead = utPltInitDb();
char mobid[128]="";
char sql[1024]="";
int iReturn=0;
int lCount=0;
iReturn=utMsgGetSomeNVar(psMsgHead,1,
					"mobid",		   UT_TYPE_STRING, sizeof(mobid)-1,         mobid);
//printf("-------------------\nsql=%s\n----------------------",sql);
snprintf(sql,sizeof(sql)-1,"select count(*) from wf_center_ap where mobid='%s'",mobid);
pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
if(lCount>0){
	 utPltPutVar(psDbHead,"buff","{success:true}");
}else{
 
	 utPltPutVar(psDbHead,"buff","{success:false}");

}
utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
return 0;
}

int wf_feedback(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  utMsgPrintMsg(psMsgHead);
  utPltDbHead *psDbHead = utPltInitDb();
char imei[32]="";
char sql[1024]="";
char address[128]="";
char info[1024]="";
char version[11]="";
int iReturn=0;
int lCount=0;
iReturn=utMsgGetSomeNVar(psMsgHead,4,
					"address",		   UT_TYPE_STRING, sizeof(address)-1,         address,
					"imei",		   UT_TYPE_STRING, sizeof(imei)-1,      imei,
					"info",			UT_TYPE_STRING, sizeof(info)-1,         info,
					"version",			UT_TYPE_STRING, sizeof(version)-1,         version);
//printf("-------------------\nsql=%s\n----------------------",sql);
time_t local;
time(&local);
snprintf(sql,sizeof(sql)-1,"insert into nw.wf_center_ap_feedback (address,imei,info,modtime,version) values(\'%s\',\'%s\',\'%s\',%d,'%s')",address,imei,utf8convert(psShmHead,info),local,version);
printf("-----%s------\n",sql);
pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
printf("------------%d------------",lCount);
if(lCount>0){
	 utPltPutVar(psDbHead,"buff","{success:true}");
}else{
 
	 utPltPutVar(psDbHead,"buff","{success:false}");

}
utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
return 0;
}


//-----------------------------------------------------------------------------------------------------
int check_app_version(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	

	return 0;
}

int gethtml_filter(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utPltDbHead *psDbHead = utPltInitDb();
	 utMsgPrintMsg(psMsgHead);

	 int readlen=0;
	 FILE* fp=fopen("/db/nw_home/plate/v8/wifi/html_filter.htm","rb");
	 if(fp!=NULL){
		fseek(fp,0,SEEK_END);
		int filelen=ftell(fp);
		if(filelen){
			char* pbuffer=malloc(filelen+1);
			memset(pbuffer,0,filelen+1);
			fseek(fp,0,SEEK_SET);
			readlen=fread(pbuffer,1,filelen,fp);
			
			utPltPutVar(psDbHead,"buff",pbuffer);
			free(pbuffer);
		}
		fclose(fp);
	 }
/*{
		 char tmpbuf[128];
		 memset(tmpbuf,0,sizeof(tmpbuf));
		 sprintf(tmpbuf,"errno:%d",errno);
		 utPltPutVar(psDbHead,"buff",tmpbuf);
	 }
	 */
	if(!readlen)utPltPutVar(psDbHead,"buff","[]");
	 
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	return 0;
}



int feedbackhtml_filter(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utPltDbHead *psDbHead = utPltInitDb();
	 utMsgPrintMsg(psMsgHead);
char html[128*1024]="";
//char pOut[128*1024]="";
char  filename[256]={0};
int lCount=0;
int iReturn=0;
iReturn=utMsgGetSomeNVar(psMsgHead,1,
					"html",		   UT_TYPE_STRING, sizeof(html)-1,         html);
	time_t local;
	time(&local);
	//pasStrCvtHex2Bin(html,pOut);
	//pasStrCvtBin2Hex(html,strlen(html),pOut);
	sprintf(filename,"/db/nw_home/log/errorlog.txt");	
	printf("-------1------------------------------------------------%s----------------------------------------------------------\n",filename);
	FILE* fp=fopen(filename,"a+");
	if(fp!=NULL){
		fprintf(fp,"%s\r\n",html);
		fclose(fp);
	}
/*
	printf("-------1-----------------------------errno:%d-------------------%s----------------------------------------------------------\n",errno,html);
*/
	 utPltPutVar(psDbHead,"buff","{success:true}");
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	return 0;
}


int wf_updatelocal_wifi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  utMsgPrintMsg(psMsgHead);
  utPltDbHead *psDbHead = utPltInitDb();

	 char zerocode[32]="";
	 char sql[1024]="";
	 char lasttime[32]="";
	
	 int iReturn=0;
  iReturn=utMsgGetSomeNVar(psMsgHead,2,   	   
					"zerocode",        UT_TYPE_STRING, sizeof(zerocode)-1,      zerocode,
					"lasttime",		   UT_TYPE_STRING, sizeof(lasttime)-1,		lasttime);


snprintf(sql,sizeof(sql)-1,"select ssid,bssid,lat,lng,passwd,zerocode,type from wf_center_ap where zerocode='%s' and modtime>%s",zerocode,lasttime);
int iNum=0;
int iret = 0;
pasDbCursor *psCur = NULL;
//printf("%s\n",sql);
psCur = pasDbOpenSql(sql, 0);
if(psCur!=NULL){
			char tempssid[32]="";
			char tempbssid[32]="";
			char tempx[32]="";
			char tempy[32]="";
			char passwd[32]="";
			char tempzerocode[16]="";
			char temptype[11]="";
			while(0==(iret=pasDbFetchInto(psCur,
				UT_TYPE_STRING,			sizeof(tempssid),			tempssid,
				UT_TYPE_STRING,			sizeof(tempbssid),		    tempbssid,
				UT_TYPE_STRING,			sizeof(tempx),				tempx,
				UT_TYPE_STRING,			sizeof(tempy),				tempy,
				UT_TYPE_STRING,			sizeof(passwd),				passwd,
				UT_TYPE_STRING,			sizeof(tempzerocode),		tempzerocode,
				UT_TYPE_STRING,			sizeof(temptype),			temptype
				)) || 1405 == iret){
				
			iNum++;
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			//printf("3333333333\n");
			utPltPutLoopVar(psDbHead,"ssid",iNum,tempssid);
			utPltPutLoopVar(psDbHead,"bssid",iNum,tempbssid);
			utPltPutLoopVar(psDbHead,"lat",iNum,tempx);
			utPltPutLoopVar(psDbHead,"lng",iNum,tempy);
			utPltPutLoopVar(psDbHead,"passwd",iNum,passwd);
			utPltPutLoopVar(psDbHead,"zerocode",iNum,tempzerocode);
			utPltPutLoopVar(psDbHead,"type",iNum,temptype);
			memset(tempssid,0,sizeof(tempssid)-1);
			memset(tempbssid,0,sizeof(tempbssid)-1);
			memset(tempy,0,sizeof(tempy)-1);
			memset(tempx,0,sizeof(tempx)-1);
			memset(temptype,0,sizeof(temptype)-1);
			}
	
	}
	pasDbCloseCursor(psCur);
	utPltPutVarF(psDbHead,"TotRec","%lu",iNum);
	//utMsgPrintMsg(psDbHead);
	printf("%lu\n",iNum);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/wifi/wifi_updatelocal_list.htm");
  return 0;

}

//char *ver   pronetway_123546.apk


bool GetNodeItem(char* buffer,char* key,char* value,int vallen)
{
	char* pos=strstr(buffer,key);
	if(pos==NULL)return false;
	pos+=(strlen(key)+1);
	snprintf(value,vallen,"%s",pos);
	char* posend=strchr(value,'\r');
	if(posend!=NULL)(*posend)=0;
	return true;
}
bool CheckCurrentVer(char* ver,char* outpath,int outlen)
{
	char filename[256]={0};
	char verinfo[128]={0};
	char tmpbuf[128]={0};
	snprintf(verinfo,sizeof(verinfo)-1,ver);
	
	sprintf(filename,"/db/nw_home/log/appver.cnf");
	FILE* fp=fopen(filename,"rb");
	//printf("%s\n",filename);
	if(fp==NULL)return false;
	//printf("--1---");
	fseek(fp,0,SEEK_END);
	int filelen=ftell(fp);
	fseek(fp,0,SEEK_SET);
	char* pbuffer=malloc(filelen+1);
	memset(pbuffer,0,filelen+1);
	fread(pbuffer,1,filelen,fp);
	fclose(fp);
	if(!GetNodeItem(pbuffer,"ver",tmpbuf,sizeof(tmpbuf)-1)){
		free(pbuffer) ;
	//printf("--2---");
		return false;
	}
	if(strcmp(tmpbuf,verinfo)==0){
		//版本没有变化
		free(pbuffer);
	//printf("--3---");
		return false;
	}
	if(!GetNodeItem(pbuffer,"path",outpath,outlen)){
		//没有取到对应版本路径
		free(pbuffer);
		//printf("--4---");
		return false;
	}
	free(pbuffer);
	return true;

}

int getLatestVersion(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utPltDbHead *psDbHead = utPltInitDb();
	 utMsgPrintMsg(psMsgHead);
	char  version[256]={0};
	char  latestUrl[256]={0};
	int lCount=0;
	int iReturn=0;
	iReturn=utMsgGetSomeNVar(psMsgHead,1,"version",		   UT_TYPE_STRING, sizeof(version)-1,         version);
	printf("%s\n",version);
	bool isUpdata=CheckCurrentVer(version,latestUrl,256);
	printf("%b\n",isUpdata);
	char buff[512]="";
	if(isUpdata){
		snprintf(buff,sizeof(buff)-1,"{success:true,url:'%s'}",latestUrl);
	}else{
		snprintf(buff,sizeof(buff)-1,"{success:false,url:''}");
	}

	 utPltPutVar(psDbHead,"buff",buff);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	return 0;
}


int upload_kkt_module_lib(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utPltDbHead *psDbHead = utPltInitDb();
	 utMsgPrintMsg(psMsgHead);
	char  weburl[48]={0};
	char  mobid[24]={0};
	char  getsmsid[24]={0};
	char  passid[24]={0};
	char  authid[24]={0};
	char  smsbegstr[24]={0};
	char  username[48]={0};
	int lCount=0;
	int iReturn=0;
	iReturn=utMsgGetSomeNVar(psMsgHead,7,
		"weburl",		   UT_TYPE_STRING, sizeof(weburl)-1,         weburl,
		"mobid",		   UT_TYPE_STRING, sizeof(mobid)-1,         mobid,
		"getsmsid",		   UT_TYPE_STRING, sizeof(getsmsid)-1,       getsmsid,
		"passid",		   UT_TYPE_STRING, sizeof(passid)-1,         passid,
		"authid",		   UT_TYPE_STRING, sizeof(authid)-1,         authid,
		"smsbegstr",		   UT_TYPE_STRING, sizeof(smsbegstr)-1,  smsbegstr,
		"username",		   UT_TYPE_STRING, sizeof(username)-1,  username);
	printf("weburl=%s\n mobid=%s\n getsmsid=%s\n passid=%s\n authid=%s\n smsbegstr=%s\n username=%s\n",weburl,mobid,getsmsid,passid,authid,smsbegstr,username);
	char sql[1024]="";
	snprintf(sql,sizeof(sql)-1,"select count(*) from kkt_module where url='%s'",weburl);
	printf("%s\n",sql);
	pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
		char buff[512]="";
		memset(sql,0,sizeof(sql));
		printf("lCount=%d\n",lCount);
		time_t modtime;
		time(&modtime);
		printf("lCount=%d\n",lCount);
	if(lCount==0){
		snprintf(sql,sizeof(sql)-1,"insert into kkt_module(url,mobid,getsmsid,passid,authid,smsbegstr,modtime,username) values('%s','%s','%s','%s','%s','%s',%ld,'%s')",weburl,mobid,getsmsid,passid,authid,smsbegstr,modtime,username);
		printf("insert sql=%s\n",sql);
		snprintf(buff,sizeof(buff)-1,"{success:true,url:'%s',action:'insert'}",weburl);
	}else{
		
		snprintf(sql,sizeof(sql)-1,"update kkt_module set mobid='%s',getsmsid='%s',passid='%s',authid='%s',smsbegstr='%s',modtime=%ld,username='%s' where url='%s'",mobid,getsmsid,passid,authid,smsbegstr,modtime,username,weburl);
		printf("update sql=%s\n",sql);
		snprintf(buff,sizeof(buff)-1,"{success:true,url:'%s',action:'update'}",weburl);
	}
	pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
	utPltPutVar(psDbHead,"buff",buff);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	return 0;
}


int download_kkt_module_lib(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utPltDbHead *psDbHead = utPltInitDb();
	 utMsgPrintMsg(psMsgHead);
	char lasttime[11]="";
	int lCount=0;
	int iReturn=0;
	iReturn=utMsgGetSomeNVar(psMsgHead,1,"lasttime",		   UT_TYPE_STRING, sizeof(lasttime)-1,  lasttime);
	printf("lasttime=%s\n",lasttime);
	char sql[1024]="";
	snprintf(sql,sizeof(sql)-1,"select url,mobid,getsmsid,passid,authid,smsbegstr,username from kkt_module where modtime>%s",lasttime);
	printf("%s\n",sql);
	pasDbCursor *psCur = NULL;
//printf("%s\n",sql);
psCur = pasDbOpenSql(sql, 0);
int iret=0;
int iNum=0;
if(psCur!=NULL){
			char  weburl[48]={0};
			char  mobid[24]={0};
			char  getsmsid[24]={0};
			char  passid[24]={0};
			char  authid[24]={0};
			char  smsbegstr[24]={0};
			char  username[48]={0};
			while(0==(iret=pasDbFetchInto(psCur,
				UT_TYPE_STRING,			sizeof(weburl),			weburl,
				UT_TYPE_STRING,			sizeof(mobid),		    mobid,
				UT_TYPE_STRING,			sizeof(getsmsid),		getsmsid,
				UT_TYPE_STRING,			sizeof(passid),			passid,
				UT_TYPE_STRING,			sizeof(authid),			authid,
				UT_TYPE_STRING,			sizeof(smsbegstr),		smsbegstr,
				UT_TYPE_STRING,			sizeof(username),		username
				)) || 1405 == iret){
				
			iNum++;
			if(iNum > 1){
				utPltPutLoopVar(psDbHead,"dh",iNum,",");
			}
			//printf("3333333333\n");
			utPltPutLoopVar(psDbHead,"weburl",iNum,weburl);
			utPltPutLoopVar(psDbHead,"mobid",iNum,mobid);
			utPltPutLoopVar(psDbHead,"getsmsid",iNum,getsmsid);
			utPltPutLoopVar(psDbHead,"passid",iNum,passid);
			utPltPutLoopVar(psDbHead,"authid",iNum,authid);
			utPltPutLoopVar(psDbHead,"smsbegstr",iNum,smsbegstr);
			utPltPutLoopVar(psDbHead,"username",iNum,username);
			memset(weburl,0,sizeof(weburl)-1);
			memset(mobid,0,sizeof(mobid)-1);
			memset(getsmsid,0,sizeof(getsmsid)-1);
			memset(passid,0,sizeof(passid)-1);
			memset(authid,0,sizeof(authid)-1);
			memset(smsbegstr,0,sizeof(smsbegstr)-1);
			memset(username,0,sizeof(username)-1);
			}
	
	}
	pasDbCloseCursor(psCur);
	utPltPutVarF(psDbHead,"TotRec","%lu",iNum);
	//utMsgPrintMsg(psDbHead);
	printf("%lu\n",iNum);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/wifi/kkt_module_lib_list.htm");
	return 0;
}
int checkportaluser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utPltDbHead *psDbHead = utPltInitDb();
	 utMsgPrintMsg(psMsgHead);

	char  username[32]={0};
	char  passwd[32]={0};
	char buff[1024]="";
	int lCount=0;
	int iReturn=0;
	iReturn=utMsgGetSomeNVar(psMsgHead,2,
		"username",		   UT_TYPE_STRING, sizeof(username)-1,         username,
		"passwd",		   UT_TYPE_STRING, sizeof(passwd)-1,         passwd);
	printf("%s\n   %s\n",username,passwd);
	char sql[1024]="";
	snprintf(sql,sizeof(sql)-1,"select count(*) from portaltooluser where username='%s' and passwd='%s'",username,passwd);
	printf("%s\n",sql);
	pasDbOneRecord(sql,0,UT_TYPE_ULONG,4,&lCount);
	if(lCount>0){
		snprintf(buff,sizeof(buff)-1,"{success:true}");
	}else{
		snprintf(buff,sizeof(buff)-1,"{success:false}");
	}
	utPltPutVar(psDbHead,"buff",buff);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_tmpPlate.htm");
	return 0;
}






 int wf_init_Funname(utShmHead *psShmHead){
	int iReturn=0;
	iReturn = pasSetTcpFunName("checkportaluser", checkportaluser, 0);
	iReturn = pasSetTcpFunName("download_kkt_module_lib", download_kkt_module_lib, 0);
	iReturn = pasSetTcpFunName("upload_kkt_module_lib", upload_kkt_module_lib, 0);
	iReturn = pasSetTcpFunName("getLatestVersion", getLatestVersion, 0);
	iReturn = pasSetTcpFunName("ncWebWifiAuth", ncWebWifiAuth, 0);
	iReturn = pasSetTcpFunName("wf_add_update_user", wf_add_update_user, 0);
	iReturn = pasSetTcpFunName("wf_show_edit_user", wf_show_edit_user, 0);
	iReturn = pasSetTcpFunName("wf_get_system_users", wf_get_system_users, 0);
	iReturn = pasSetTcpFunName("wf_get_wifi_from_bssid", wf_get_wifi_from_bssid, 0);
	iReturn = pasSetTcpFunName("wf_get_city_zerocode", wf_get_city_zerocode, 0);
	iReturn = pasSetTcpFunName("wf_get_province_code", wf_get_province_code, 0);
	iReturn = pasSetTcpFunName("wf_query_wifi", wf_query_wifi, 0);
	iReturn = pasSetTcpFunName("getbssid", getbssid, 0);
	iReturn = pasSetTcpFunName("getapmacfrombssid", getapmacfrombssid, 0);
	iReturn = pasSetTcpFunName("wf_updatelocal_wifi",wf_updatelocal_wifi,0);
	iReturn = pasSetTcpFunName("feedbackhtml_filter",feedbackhtml_filter,0);
	iReturn = pasSetTcpFunName("gethtml_filter",gethtml_filter,0);
	iReturn = pasSetTcpFunName("wf_feedback",wf_feedback,0);
	iReturn = pasSetTcpFunName("wf_checkuser_vip",wf_checkuser_vip,0);
	iReturn = pasSetTcpFunName("wf_share_wifi",wf_share_wifi,0);
	iReturn = pasSetTcpFunName("wf_getnearby_wifi",wf_getnearby_wifi,0);
	return 0;
}