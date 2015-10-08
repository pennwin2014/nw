/* ¼à¿ØÈí¼þ---ÓÐ¹ØWebµÄÓ¦ÓÃ
     Modify 2002/9/9   by Liyunming*/
#define PAS_SRCFILE 1004
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
char *pasUtlCvtSqlSelectTreeBm();

 
 
int ncWebListUserTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
   	char caUsername[32],caDispname[32],caGroupname[32],caGroup[32];
   	char caScroll[16],caOpennum[16],caFname[32];
   	int iNum1,iNum2;
   	long lUserid,lGroupid,lGroupid0;
    utPltDbHead *psDbHead;
   	int iReturn,i;

   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
					"plate",    UT_TYPE_STRING, 127, caPlate,
					"fname",    UT_TYPE_STRING, 31,  caFname);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682ÏÔÊ¾ÓÃ»§"),ncLang("0554Êý¾Ý¿â³öÏÖÎÊÌâ£¬ÇëÓë¹ÜÀíÔ±ÁªÏµ"));
        return 0;
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) { /* Ö¸¶¨ÓÃ»§  */
        psCur = pasDbOpenSqlF("select a.userid,a.username,a.dispname,b.groupname,a.groupid from ncuser a,\
                           ncgroup b where a.groupid = b.groupid  and a.groupid = %lu order by \
                           b.groupid,a.dispname",
                           lGroupid0);
    }
    else {
        psCur = pasDbOpenSql("select a.userid,a.username,a.dispname,b.groupname,a.groupid from ncuser a,\
                           ncgroup b where a.groupid = b.groupid order by b.groupid,a.dispname",0);
    }
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683ÏÔÊ¾ÓÃ»§Ê÷"),ncLang("0554Êý¾Ý¿â³öÏÖÎÊÌâ£¬ÇëÓë¹ÜÀíÔ±ÁªÏµ"));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,sizeof(long),&lUserid,
                                   UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,sizeof(long),&lGroupid);
    i = 0;
    strcpy(caGroup,"\0");
    iNum1 = 0;
    iNum2 = 0;
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    while(iReturn == 0) {
        utStrDelSpaces(caGroupname);
        utStrDelSpaces(caUsername);
        if(strcmp(caGroup,caGroupname) != 0) {
            iNum1++;
            utPltPutLoopVar(psDbHead,"groupname", iNum1,caGroupname);
            utPltPutLoopVarF(psDbHead,"groupid",   iNum1,"%ld",lGroupid);
            utPltPutLoopVar(psDbHead,"plate",    iNum1,caPlate);
            utPltPutLoopVar(psDbHead,"fname",    iNum1,caFname);
            strcpy(caGroup,caGroupname);
            iNum2=1;
        }
        else {
            iNum2++;
        }
        utPltPutLoopVar2(psDbHead, "username", iNum1,iNum2,caUsername);
        utPltPutLoopVar2(psDbHead, "dispname", iNum1,iNum2,caDispname);
        utPltPutLoopVar2(psDbHead, "plate",    iNum1,iNum2,caPlate);
        utPltPutLoopVar2(psDbHead, "fname",    iNum1,iNum2,caFname);
        utPltPutLoopVarF2(psDbHead,"userid", iNum1,iNum2,"%ld",lUserid);
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,sizeof(long),&lUserid,
                                   UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,sizeof(long),&lGroupid);
    }
    utPltPutVar(psDbHead, "fname",caFname);
    pasDbCloseCursor(psCur);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}


/* ÏÔÊ¾Ôö¼Ó/ÐÞ¸ÄÓÃ»§µÄ½çÃæ  */
int ncWebDispUserAddForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caIp[34],caMac[64],caAddtime[21];
    char caUserid[32];
   	char caUsername[34],caGroupname[34];
   	char caEmail[64],caLasttime[21];
   	char caPassword[32];
   	char caCond[32],caGroupid[16];
   	char caGid[16];
   	int iNum1;
   	long lUse;
   	long lUserid,lGroupid,lGroupid0,lLimit,lLimitid,lPost,lWebip,polid;
    utPltDbHead *psDbHead;
   	int iReturn;
   	int iModFlags;
   	char caSmt[256];
   	char *pBuf,*pSelect;
   	char  caUse[16];
   	unsigned long lTime;
   	unsigned long lAddtime,lLasttime,lIp;
   	char caDispname[32];
   	char caTemp[32];
   	char caLimitid[16],caWebip[16],caPost[16];
   	char caCompname[32];
   	char pVersion[16];
   	long lLocalflag,lExpiretime,lUsestatus,userlevel;
   	int iNum,lLevel,vlanid;
   	char caSex[8];
   	long groupid_flag;
   	long lTid;
   	char caTname[36];
   	long lDay;
   	char caPort[36],caPointphyaddr[16],caIdfytype[4],caIdfyno[20],caPointphyname[20],caIdfyunit[32],caCountry[4];
    psDbHead = utPltInitDb();
    groupid_flag=0;
    iReturn = dsCltGetMyInfo(1,"usrlevel",&lLevel);
    
    iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"gid",    UT_TYPE_STRING, 10, caGid);
    
    //È¡½»»»»ú¶Ë¿Ú
    if(lLevel==3){
        psCur = pasDbOpenSql("select position from ncswitchport where kfflag=1 order by position",0);
    }
    else{
    	psCur = pasDbOpenSql("select position from ncswitchport order by position",0);
    }
    if(psCur!= NULL) {
    memset(caPort,0,sizeof(caPort));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caPort);
                                   
    iNum = 0;
    while((iReturn == 0)||(iReturn==1405)) {  
            iNum++;
            utPltPutLoopVar(psDbHead,"port", iNum,caPort);
    	memset(caPort,0,sizeof(caPort));
    	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caPort);
    }
    pasDbCloseCursor(psCur);
   }
    
   	iReturn = utMsgGetSomeNVar(psMsgHead,3,
   	                "groupid", UT_TYPE_STRING, 15, caGroupid,     /* Î»ÖÃ  */
					"userid",  UT_TYPE_STRING, 15, caUserid,
					"cond",    UT_TYPE_STRING, 31, caCond);
    iModFlags = 0;
    lPost = 0;
    lWebip = 0;
    lLimitid = 0;
    lUserid = 0;
    vlanid=0;
    lTime = time(0);
    if(!utStrIsSpaces(caUserid)) { 
        iModFlags = 1;
        lLocalflag=0;
        lUserid = atol(caUserid);
        memset(caGroupname,0,sizeof(caGroupname));
        iReturn = pasDbOneRecord("select a.username,a.dispname,a.compname,b.groupname,a.groupid,a.ip,a.mac,a.addtime,a.useflags,a.lasttime,a.email,a.position,idfytype,idfyno,pointphyname,idfyunit,country,localflag,expiretime,usestatus,sex,userlevel,vlanid from ncuser a left join ncgroup b on a.groupid = b.groupid where userid=:userid",1,
                            "userid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_STRING,31,caUsername,
                            UT_TYPE_STRING,31,caDispname,
                            UT_TYPE_STRING,31,caCompname,
                            UT_TYPE_STRING,31,caGroupname,
                            UT_TYPE_LONG,  sizeof(long), &lGroupid,
                            UT_TYPE_ULONG, 4,&lIp,
                            UT_TYPE_STRING,64,caMac,
                            UT_TYPE_ULONG, 4,&lAddtime,
                            UT_TYPE_LONG,  4, &lUse,
                            UT_TYPE_STRING,4,&lLasttime,
                            UT_TYPE_STRING,63,caEmail,
                            UT_TYPE_STRING,32,caPort,
                            UT_TYPE_STRING,3,caIdfytype,
                            UT_TYPE_STRING,18,caIdfyno,
                            UT_TYPE_STRING,16,caPointphyname,
                            UT_TYPE_STRING,30,caIdfyunit,
                            UT_TYPE_STRING,3,caCountry,
                            UT_TYPE_LONG,4,&lLocalflag,
                            UT_TYPE_LONG,4,&lExpiretime,
                            UT_TYPE_LONG,4,&lUsestatus,
                            UT_TYPE_STRING,1,caSex,
                            UT_TYPE_LONG,4,&userlevel,
                            UT_TYPE_LONG,4,&vlanid);
        if(iReturn != 0 && iReturn != PAS_DB_NULLVALUE) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0743ÓÃ»§²»´æÔÚ %d"),iReturn);
            return 0;
        }
        else {
            utPltPutVar(psDbHead,"userid",    caUserid);
            utPltPutVar(psDbHead,"username",  caUsername);
            if(utStrIsSpaces(caDispname)) {
                strcpy(caDispname,caUsername);
            }
            utPltPutVar(psDbHead,"dispname",  caDispname);
            utPltPutVar(psDbHead,"compname",  caCompname);
            utPltPutVar(psDbHead,"groupname", caGroupname);
            utPltPutVarF(psDbHead,"groupid", "%ld",lGroupid);
            if(lIp != 0) {
                utPltPutVar(psDbHead,"ip", utComHostIp(htonl(lIp)));
            }
            utPltPutVar(psDbHead,"addtime", utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime));
            utPltPutVar(psDbHead,"mac", caMac);
            utPltPutVar(psDbHead,"lasttime",utTimFormat("%Y-%m-%d %H:%M:%S",lLasttime));
            utPltPutVar(psDbHead,"email", caEmail);
            utPltPutVar(psDbHead,"password", "********");
            
            utPltPutVar(psDbHead,"idfyno",caIdfyno);
            utPltPutVar(psDbHead,"idfyunit",caIdfyunit);
            utPltPutVar(psDbHead,"country",caCountry);
            utPltPutVar(psDbHead,"port",caPort);
            utPltPutVarF(psDbHead,"VlanID","%d",vlanid);
            utPltPutVar(psDbHead,"pointphyname",caPointphyname);
            utPltPutVar(psDbHead,"idfytype",caIdfytype);
            if(lLocalflag==1)
            utPltPutVar(psDbHead,"localflag","checked");
            
            if(userlevel>0)
            utPltPutVar(psDbHead,"userlevel","checked");
            
            if(lExpiretime>0){
            lDay=(lExpiretime-lAddtime+50)/86400;
             utPltPutVarF(psDbHead,"expiretime","%d",lDay);  
            }
        
            utPltPutVarF(psDbHead,"usestatus","%lu",lUsestatus);
            utPltPutVar(psDbHead,"sex",caSex);
        }
    }
    else {
    	long lPs;
        lGroupid = 0;
        lUse = 0;
        strcpy(caAddtime,utTimFormat("%Y-%m-%d %H:%M:%S",lTime));
        utPltPutVar(psDbHead,"addtime", caAddtime);
        utPltPutVar(psDbHead,"lasttime",caAddtime);
        utPltPutVar(psDbHead,"expiretime", utComGetVar_sd(psShmHead,"expiretime",""));
 //       utPltPutVar(psDbHead,"expiretime",utTimFormat("%Y-%m-%d %H:%M:%S",lTime+86400));
 //       utPltPutVarF(psDbHead,"username","%lu",lTime);
       
        lPs=rand() % 100000;
        utPltPutVarF(psDbHead,"password","%lu",lPs);
        utPltPutVar(psDbHead,"country","CHN");
        utPltPutVar(psDbHead,"sex","9");
        utPltPutVarF(psDbHead,"VlanID","%d",0);
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) { /* Ö¸¶¨ÓÃ»§  */
    	 groupid_flag=1;
        sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu or pid=%lu order by groupid",
                 lGroupid0,lGroupid0);
       pBuf = pasUtlCvtSqlSelect(caSmt);          
    }
    else {
    	
    	pBuf = pasUtlCvtSqlSelectTreeBm();
//			sprintf(caSmt,"select groupname,groupid from ncgroup order by groupid");
    }

  
    if(pBuf == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771ÓÃ»§ÐÅÏ¢"),ncLang("0153²¿ÃÅÁÐ±í²»´æÔÚµ"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    sprintf(caGroupid,"%d",lGroupid);
    pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
    free(pBuf);
    if(pSelect == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771ÓÃ»§ÐÅÏ¢"),ncLang("0404ÄÚ´æ³ö´í"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    utPltPutVar(psDbHead,"dept", pSelect);
    free(pSelect);
    sprintf(caUse,"%d",lUse);
    pSelect = pasUtlCvtInputSelect("use",caUse,ncLang("0852È«²¿¼à¿Ø,0;½ûÓÃ,1;Ö»¼à¿ØIP·ÃÎÊ,3;¼à¿ØIPºÍHttp,4;Ãâ¼à¿Ø,9;"));
    if(pSelect) {
        utPltPutVar(psDbHead,"use", pSelect);
        free(pSelect);
    }
    if(!utStrIsSpaces(caGroupid)) utPltPutVar(psDbHead,"groupid", caGroupid);
    if(!utStrIsSpaces(caCond)) utPltPutVar(psDbHead,"cond", caCond);


    
    if(lUserid > 0) {
        iReturn = pasDbOneRecord("select polid from nclimsumm where gtype = 1 and id = :userid",1,
                            "userid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_ULONG,4,&polid);
        if(iReturn != 0) {
            polid = 0;
        }
    }
    pBuf = pasUtlCvtSqlSelectA("select name,id from ncprocy",ncLang("0652ÎÞ,0;"));
    if(pBuf) {
        sprintf(caTemp,"%lu",polid);
        utPltPutVar(psDbHead,"polid2",caTemp);
        pSelect = pasUtlCvtInputSelect("polid",caTemp,pBuf);
        if(pSelect) {
            utPltPutVar(psDbHead,"polid", pSelect);
            free(pSelect);
        }
        free(pBuf);
    }
    
    utPltPutVar(psDbHead,"gid",caGid);
   
    
      //È¡ÓÊ¼þÀ¹½Ø²ßÂÔ £¬½«Æä·ÅÔÚÑ¡Ôñ¿òÖÐ  
      psCur=pasDbOpenSqlF("select id,name from ncmailstrategy order by dispname ");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213¶¨ÒåÓÊ¼þ·ÖÀà"),ncLang("0174³ö´í%s"),pasDbErrorMsg(NULL));
            return 0;
        }
     
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
      				   UT_TYPE_STRING,30,caTname);
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
	     
      	utPltPutLoopVarF(psDbHead,"id_m",iNum,"%lu",lTid);
      	utPltPutLoopVar(psDbHead,"name_m",iNum,caTname);
      	 iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
  				      UT_TYPE_STRING,30,caTname);
  				}
     
       pasDbCloseCursor(psCur); 
    
    
      //È¡´ø¿í²ßÂÔ £¬½«Æä·ÅÔÚÑ¡Ôñ¿òÖÐ  
      psCur=pasDbOpenSqlF("select id,name from nctcstrategy order by dispname ");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213¶¨ÒåÓÊ¼þ·ÖÀà"),ncLang("0174³ö´í%s"),pasDbErrorMsg(NULL));
            return 0;
        }
     
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
      				   UT_TYPE_STRING,30,caTname);
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
	     
      	utPltPutLoopVarF(psDbHead,"id_t",iNum,"%lu",lTid);
      	utPltPutLoopVar(psDbHead,"name_t",iNum,caTname);
      	 iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
  				      UT_TYPE_STRING,30,caTname);
  				}
     
       pasDbCloseCursor(psCur); 
    
    utPltPutVar(psDbHead,"servicecode",utComGetVar_sd(psShmHead,"NcName",""));
    
    utPltPutVar(psDbHead,"globpass",utComGetVar_sd(psShmHead,"globpass",""));         
    utPltPutVar(psDbHead,"authpwdflag",utComGetVar_sd(psShmHead,"authpwdflag","0"));
 
    
    
    
    strcpy(pVersion,utComGetVar_sd(psShmHead,"versiontype","0"));
    if(atol(pVersion)==2)
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_useredit_ts.htm");
    else if(atol(pVersion)==3)
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_useredit_dn.htm");
    else if(atol(pVersion)==4)
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_useredit_xx.htm");
    else    
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_useredit.htm");
    return 0;
}



/* ÏÔÊ¾ÐÞ¸ÄÓÃ»§µÄ½çÃæ  */
int ncWebDispUserModiForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caIp[34],caMac[64],caAddtime[21];
    char caUserid[32];
   	char caUsername[34],caGroupname[34];
   	char caPassword[32];
   	char caEmail[64],caLasttime[21];
   	char caCond[64],caGroupid[16],caGroupid0[16];
   	char caSmt[256];
   	char caPlate[128];
   	char caCurpg[16],caLimitid[16];
   	char caTemp[32];
    long userlevel;
   	int iNum1;
   	long lUse;
   	long lUserid,lGroupid,lGroupid0,lLimit,lLimitid,polid;
    utPltDbHead *psDbHead;
   	int iReturn;
   	int iModFlags;
   	int lLevel,vlanid;
   	char *pBuf,*pSelect;
   	char  caUse[16];
   	char  caCompname[32];
   	char pVersion[32];
   	unsigned long lTime,lPost,lWebip,lIp,lAddtime,lLasttime,lExpiretime,lUsestatus;
   	char caDispname[32];
   	char caPost[36],caWebip[16];
   	long lLocalflag;
    int iNum,group_flag;
    char caSex[8];
    long lTid;
    char caTname[32];
    
    unsigned long qospolid;
    unsigned long mailctlid;
    
   	char caPort[36],caPointphyaddr[16],caIdfytype[4],caIdfyno[20],caPointphyname[20],caIdfyunit[32],caCountry[4];
    psDbHead = utPltInitDb();
    iReturn = dsCltGetMyInfo(1,"usrlevel",&lLevel);
    group_flag=0;
   
    //È¡½»»»»ú¶Ë¿Ú
    if(lLevel==3){
        psCur = pasDbOpenSql("select position from ncswitchport where kfflag=1 order by position",0);
    }
    else{
    	psCur = pasDbOpenSql("select position from ncswitchport order by position",0);
    }
    if(psCur!= NULL) {
    memset(caPort,0,sizeof(caPort));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caPort);
                                   
    iNum = 0;
    while((iReturn == 0)||(iReturn==1405)) {  
            iNum++;
            utPltPutLoopVar(psDbHead,"port", iNum,caPort);
    	memset(caPort,0,sizeof(caPort));
    	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caPort);
    }
    pasDbCloseCursor(psCur);
   }
   	iReturn = utMsgGetSomeNVar(psMsgHead,5,
   	                "CurPg",   UT_TYPE_STRING, 15, caCurpg,
   	                "groupid0", UT_TYPE_STRING, 15, caGroupid0,
					"userid",  UT_TYPE_STRING, 15, caUserid,
					"plate",   UT_TYPE_STRING, 63, caPlate,
					"cond",    UT_TYPE_STRING, 31, caCond);
    lUserid = 0;
    if(!utStrIsSpaces(caUserid)) { 
        lUserid = atol(caUserid);
        memset(caGroupname,0,sizeof(caGroupname));
        lGroupid=0;
        lLocalflag=0;
        iReturn = pasDbOneRecord("select a.username,a.dispname,a.compname,b.groupname,a.groupid,a.ip,a.mac,a.addtime,a.useflags,a.lasttime,a.email,a.position,idfytype,idfyno,pointphyname,idfyunit,country,localflag,expiretime,usestatus,sex,userlevel,vlanid from ncuser a left join ncgroup b on a.groupid = b.groupid where a.userid=:userid",1,
                            "userid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_STRING,31,caUsername,
                            UT_TYPE_STRING,31,caDispname,
                            UT_TYPE_STRING,31,caCompname,
                            UT_TYPE_STRING,31,caGroupname,
                            UT_TYPE_LONG,  sizeof(long), &lGroupid,
                            UT_TYPE_ULONG, 4,&lIp,
                            UT_TYPE_STRING,64,caMac,
                            UT_TYPE_ULONG, 4,&lAddtime,
                            UT_TYPE_LONG,  4, &lUse,
                            UT_TYPE_LONG,4,&lLasttime,
                            UT_TYPE_STRING,63,caEmail,
                            UT_TYPE_STRING,32,caPort,
                            UT_TYPE_STRING,3,caIdfytype,
                            UT_TYPE_STRING,18,caIdfyno,
                            UT_TYPE_STRING,16,caPointphyname,
                            UT_TYPE_STRING,30,caIdfyunit,
                            UT_TYPE_STRING,3,caCountry,
                            UT_TYPE_LONG,4,&lLocalflag,
                            UT_TYPE_LONG,4,&lExpiretime,
                            UT_TYPE_LONG,4,&lUsestatus,
                            UT_TYPE_STRING,1,caSex,
                            UT_TYPE_LONG,4,&userlevel,
                            UT_TYPE_LONG,4,&vlanid);

        if(iReturn != 0 && iReturn != PAS_DB_NULLVALUE) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0743ÓÃ»§²»´æÔÚ %d"),iReturn);
            return 0;
        }
        else {
            utPltPutVar(psDbHead,"userid",    caUserid);
            utPltPutVar(psDbHead,"username",  caUsername);
            if(utStrIsSpaces(caDispname)) {
                strcpy(caDispname,caUsername);
            }
            utPltPutVar(psDbHead,"dispname",  caDispname);
            utPltPutVar(psDbHead,"compname",  caCompname);
            utPltPutVar(psDbHead,"groupname", caGroupname);
            utPltPutVarF(psDbHead,"groupid", "%ld",lGroupid);
            utPltPutVar(psDbHead,"ip", utComHostIp(htonl(lIp)));
            if(lAddtime>0){
            	utPltPutVar(psDbHead,"addtime", utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime));
            }
            utPltPutVar(psDbHead,"mac", caMac);
            if(lLasttime>0){
            	utPltPutVar(psDbHead,"lasttime", utTimFormat("%Y-%m-%d %H:%M:%S",lLasttime));
            }
            if(lExpiretime>0)
            utPltPutVar(psDbHead,"expiretime", utTimFormat("%Y-%m-%d %H:%M:%S",lExpiretime));
            utPltPutVar(psDbHead,"email", caEmail);
            utPltPutVar(psDbHead,"password", "********");
            utPltPutVar(psDbHead,"email", caEmail);
            utPltPutVar(psDbHead,"idfyno",caIdfyno);
            utPltPutVar(psDbHead,"idfyunit",caIdfyunit);
            utPltPutVar(psDbHead,"country",caCountry);
            utPltPutVar(psDbHead,"port",caPort);
            utPltPutVar(psDbHead,"pointphyname",caPointphyname);
            utPltPutVar(psDbHead,"idfytype",caIdfytype);
            if(lLocalflag==1)
            utPltPutVar(psDbHead,"localflag","checked");
            
            if(userlevel>0)
            utPltPutVar(psDbHead,"userlevel","checked");
            
            
            utPltPutVarF(psDbHead,"usestatus","%lu",lUsestatus);
            
            utPltPutVar(psDbHead,"sex",caSex);
            utPltPutVarF(psDbHead,"VlanID","%d",vlanid);
        }
    }
    else {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0743ÓÃ»§²»´æÔÚ %d"),iReturn);
        return 0;
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) { /* Ö¸¶¨ÓÃ»§  */
    	group_flag=1;
        sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu or pid=%lu order by groupid",
                 lGroupid0,lGroupid0);
       pBuf = pasUtlCvtSqlSelect(caSmt);           
    }
    else {
//		sprintf(caSmt,"select groupname,groupid from ncgroup order by groupid");
      pBuf = pasUtlCvtSqlSelectTreeBm();
    }

   
    if(pBuf == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771ÓÃ»§ÐÅÏ¢"),ncLang("0153²¿ÃÅÁÐ±í²»´æÔÚµ"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    sprintf(caGroupid,"%d",lGroupid);
    pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
    if(pSelect == NULL) {
        free(pBuf);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771ÓÃ»§ÐÅÏ¢"),ncLang("0404ÄÚ´æ³ö´í"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    utPltPutVar(psDbHead,"dept", pSelect);
    free(pSelect);
    sprintf(caUse,"%d",lUse);
    pSelect = pasUtlCvtInputSelect("use",caUse,ncLang("0852È«²¿¼à¿Ø,0;½ûÓÃ,1;Ö»¼à¿ØIP·ÃÎÊ,3;¼à¿ØIPºÍHttp,4;Ãâ¼à¿Ø,9;"));
    if(pSelect) {
        utPltPutVar(psDbHead,"use", pSelect);
        free(pSelect);
    }
    if(lUserid > 0) {
    	      polid = 0;
            qospolid=0;
            mailctlid=0;
        iReturn = pasDbOneRecord("select polid,qospolid,mailctlid from nclimsumm where gtype = 1 and id = :userid",1,
                            "userid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_ULONG,4,&polid,
                            UT_TYPE_ULONG,4,&qospolid,
                            UT_TYPE_ULONG,4,&mailctlid);
        if(iReturn != 0) {
            polid = 0;
            qospolid=0;
            mailctlid=0;
        }
    }
    pBuf = pasUtlCvtSqlSelectA("select name,id from ncprocy",ncLang("0652ÎÞ,0;"));
    if(pBuf) {
        sprintf(caTemp,"%lu",polid);
        utPltPutVar(psDbHead,"polid2",caTemp);
        pSelect = pasUtlCvtInputSelect("polid",caTemp,pBuf);
        if(pSelect) {
            utPltPutVar(psDbHead,"polid", pSelect);
            free(pSelect);
        }
        free(pBuf);
    }



    
    if(!utStrIsSpaces(caGroupid0)) utPltPutVar(psDbHead,"groupid0", caGroupid0);
    if(!utStrIsSpaces(caCond))    utPltPutVar(psDbHead,"cond", caCond);
    if(!utStrIsSpaces(caCurpg))   utPltPutVar(psDbHead,"CurPg", caCurpg);







 //È¡¿ØÖÆ²ßÂÔÐÅÏ¢ £¬½«Æä·ÅÔÚÑ¡Ôñ¿òÖÐ  
      psCur=pasDbOpenSqlF("select id,name from ncprocy order by name ");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213¶¨ÒåÓÊ¼þ·ÖÀà"),ncLang("0174³ö´í%s"),pasDbErrorMsg(NULL));
            return 0;
        }
     
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
      				   UT_TYPE_STRING,30,caTname);
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
	     
      	utPltPutLoopVarF(psDbHead,"id_p",iNum,"%lu",lTid);
      	utPltPutLoopVar(psDbHead,"name_p",iNum,caTname);
      	 iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
  				      UT_TYPE_STRING,30,caTname);
  				}
     
       pasDbCloseCursor(psCur); 
  
     //È¡´ø¿í²ßÂÔ £¬½«Æä·ÅÔÚÑ¡Ôñ¿òÖÐ  
      psCur=pasDbOpenSqlF("select id,name from nctcstrategy order by dispname ");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213¶¨ÒåÓÊ¼þ·ÖÀà"),ncLang("0174³ö´í%s"),pasDbErrorMsg(NULL));
            return 0;
        }
     
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
      				   UT_TYPE_STRING,30,caTname);
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
	     
      	utPltPutLoopVarF(psDbHead,"id_t",iNum,"%lu",lTid);
      	utPltPutLoopVar(psDbHead,"name_t",iNum,caTname);
      	 iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
  				      UT_TYPE_STRING,30,caTname);
  				}
     
       pasDbCloseCursor(psCur); 
    
      //È¡ÓÊ¼þÀ¹½Ø²ßÂÔ £¬½«Æä·ÅÔÚÑ¡Ôñ¿òÖÐ  
      psCur=pasDbOpenSqlF("select id,name from ncmailstrategy order by dispname ");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213¶¨ÒåÓÊ¼þ·ÖÀà"),ncLang("0174³ö´í%s"),pasDbErrorMsg(NULL));
            return 0;
        }
     
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
      				   UT_TYPE_STRING,30,caTname);
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
	     
      	utPltPutLoopVarF(psDbHead,"id_m",iNum,"%lu",lTid);
      	utPltPutLoopVar(psDbHead,"name_m",iNum,caTname);
      	 iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
  				      UT_TYPE_STRING,30,caTname);
  				}
     
       pasDbCloseCursor(psCur); 


       utPltPutVarF(psDbHead,"qospolid","%d",qospolid);
       utPltPutVarF(psDbHead,"mailctlid","%d",mailctlid);
       utPltPutVarF(psDbHead,"userstate","%d",lUse);        

    utPltPutVar(psDbHead,"servicecode",utComGetVar_sd(psShmHead,"NcName",""));

    if(utStrIsSpaces(caPlate)) {
    	strcpy(pVersion,utComGetVar_sd(psShmHead,"versiontype","0"));
    	if(atol(pVersion)==2)
    		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_usermodi_ts.htm");
    	else if(atol(pVersion)==3)
    		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_usermodi_dn.htm");
    	else if(atol(pVersion)==4)
    		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_usermodi_xx.htm");
    	else
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_usermodi.htm");
    }
    else {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    }
    return 0;
}

int ncWebUserModify(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caUsername[33],caGroupid[33],caIp[20],caMac[33],caMacstr[42],caUse[10];
   	char caDispname[33],caLimit[32];
   	ncUserInfo *psUserInfo;
   	long lUserid,lGroupid;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lUse,lUserid0;
   	unsigned long lLimit,lLimitid,lPost,lWebip,lIp,lGroupid0,lTime;
   	unsigned long lTimeLimit,lFlowLimit;
   	int iPass,iAddFlags;
   	char caPassword[32],caPassword1[32];
   	char caDel[16],caMod[16];
   	char caDate[32],caUserid[16],caUsername0[32];
    char caEmail[64];
    char caCompname[32];
    char caAtest[16],caAddtime[32],caLasttime[32],caExpiretime[32],caUsestatus[10];
    char caLimitid[16];
    char caPost[36];
    char caOpt[2];
    char caWebip[16];
    char caPolid[16];
    char caTemp[1024];
    char caLocalflag[16];
    char caUserlevel[16];
    char caSex[8];
    char caVlanid[16];
    long group_flag;
    long lCount;
    unsigned long lExpiretime;
    char caPort[36],caPointphyaddr[16],caIdfytype[4],caIdfyno[20],caPointphyname[20],caIdfyunit[32],caCountry[4];
   char caUsername2[32];
   char caMpid[12],caTpid[12];
   ncUserCont *psUser;
   char caPass_b[32];
   memset(caPass_b,0,sizeof(caPass_b));
   memset(caUsername2,0,sizeof(caUsername2));
   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
   group_flag=0;
#ifdef LDEBUG    
    utMsgPrintMsg(psMsgHead);    
#endif
lTime=time(0);

   	iReturn = utMsgGetSomeNVar(psMsgHead,31,
   	                "userid",      UT_TYPE_STRING, 12, caUserid,
					"username",    UT_TYPE_STRING, 31, caUsername,
					"compname",    UT_TYPE_STRING, 31, caCompname,
					"groupid",     UT_TYPE_STRING, 12, caGroupid,
					"password",    UT_TYPE_STRING, 16, caPassword,
					"password1",   UT_TYPE_STRING, 16, caPassword1,
					"ip",          UT_TYPE_STRING, 20, caIp,
					"mac",         UT_TYPE_STRING, 40, caMacstr,
					"email",       UT_TYPE_STRING, 63, caEmail,
					"use",       UT_TYPE_STRING, 9,  caUse,
					"dispname",    UT_TYPE_STRING, 31, caDispname,
					"atest",       UT_TYPE_STRING, 12, caAtest,
					"addtime",     UT_TYPE_STRING, 31, caAddtime,
					"lasttime",    UT_TYPE_STRING, 31, caLasttime,
					"del",         UT_TYPE_STRING, 15, caDel,
					"polid",       UT_TYPE_STRING, 15, caPolid,
					"opt",         UT_TYPE_STRING, 1,  caOpt,
					"idfyno",      UT_TYPE_STRING,18,caIdfyno,
					"idfyunit",    UT_TYPE_STRING,30,caIdfyunit,
					"country",     UT_TYPE_STRING,3,caCountry,
					"port",        UT_TYPE_STRING,32,caPort,
					"pointphyname",UT_TYPE_STRING,16,caPointphyname,
					"idfytype",    UT_TYPE_STRING,3,caIdfytype,
					"localflag",   UT_TYPE_STRING,1,caLocalflag,
					"expiretime",  UT_TYPE_STRING,20,caExpiretime,
					"usestatus",   UT_TYPE_STRING,8,caUsestatus,
					"sex",         UT_TYPE_STRING,1,caSex,
					"userlevel",   UT_TYPE_STRING,1,caUserlevel,
					"VlanID",      UT_TYPE_STRING,10,caVlanid,
					"m_pid",       UT_TYPE_STRING,10,caMpid,
					"t_pid",       UT_TYPE_STRING,10,caTpid);
	 if(strlen(caExpiretime)>0){
	 	lExpiretime=utTimStrToLong("%Y-%m-%d %H:%M:%S",caExpiretime);
		}
		else{
			lExpiretime=0;
		}
	  if(strlen(caUsestatus)==0) strcpy(caUsestatus,"1");
	  
	  iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
	  if(iReturn == 0 && lGroupid0 > 0) {
	  	group_flag=1;
	  }
	  
	  
    if(!utStrIsSpaces(caAtest)) { /* ×Ô¶¯¼ì²âMACºÍ¼ÆËã»úÃû  */
        char *pSelect,*pBuf;
        char caSmt[255];
        if(!utStrIsValidIp(caIp)) {
            lIp = 0;
        }
        else {
            char *pDev;
            lIp = ntohl(pasIpcvtLong(caIp));     
            pDev = utComGetVar_s(psShmHead,"admindev");
            ncUtlGetMacAddress(lIp,caMac,caCompname,pDev);
//            iReturn = ncUtlGetMacFromWins(lIp,caMac,caCompname);
            strcpy(caMacstr,ncCvtMac(caMac));
        }
        psDbHead = utPltInitDb();
        utPltPutVar(psDbHead,"userid",caUserid);
        utPltPutVar(psDbHead,"username",caUsername);
        utPltPutVar(psDbHead,"dispname",caDispname);
        utPltPutVar(psDbHead,"password",caPassword);
        utPltPutVar(psDbHead,"password1",caPassword1);
        utPltPutVarF(psDbHead,"groupid", "%ld",lGroupid);
        utPltPutVar(psDbHead,"ip", caIp);
        utPltPutVar(psDbHead,"mac", caMacstr);
        utPltPutVar(psDbHead,"compname", caCompname);
        utPltPutVar(psDbHead,"lasttime", caLasttime);
        utPltPutVar(psDbHead,"addtime", caAddtime);
        utPltPutVar(psDbHead,"email", caEmail);
        
        utPltPutVar(psDbHead,"idfyno",caIdfyno);
        utPltPutVar(psDbHead,"idfyunit",caIdfyunit);
        utPltPutVar(psDbHead,"country",caCountry);
        utPltPutVar(psDbHead,"port",caPort);
				utPltPutVar(psDbHead,"VlanID",caVlanid);
        utPltPutVar(psDbHead,"pointphyname",caPointphyname);
        utPltPutVar(psDbHead,"idfytype",caIdfytype);
        
        if(atol(caLocalflag)==1)
        utPltPutVar(psDbHead,"localflag","checked");
        
        if(atol(caUserlevel)>0)
        utPltPutVar(psDbHead,"userlevel","checked");
        
        
        iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
        if(iReturn == 0 && lGroupid0 > 0) { /* Ö¸¶¨ÓÃ»§  */
        	
            sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu or pid=%lu order by groupid",
                 lGroupid0,lGroupid0);
        }
        else {
		    sprintf(caSmt,"select groupname,groupid from ncgroup order by groupid");
        }
        pBuf = pasUtlCvtSqlSelect(caSmt);
        if(pBuf) {
            pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
            free(pBuf);
            utPltPutVar(psDbHead,"dept", pSelect);
            free(pSelect);
        }
            
        pSelect = pasUtlCvtInputSelect("use",caUse,ncLang("0852Õý³£,0;½ûÓÃ,1;Ãâ¼à¿Ø,9;"));
        utPltPutVar(psDbHead,"use", pSelect);
        free(pSelect);
        utPltPutVar(psDbHead,"opt", caOpt);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_usermodi.htm");
        return 0;
    }

    utStrDelSpaces(caUsername);
    if(!utStrIsSpaces(caIp)) {
        if(!utStrIsValidIp(caIp)) {
            lIp = 0;
        }
        else {
            lIp = ntohl(utComHostAddress(caIp));            
        }
    }
    else {
        lIp = 0;
    }
    lGroupid = atol(caGroupid);
    if(utStrIsSpaces(caDispname)) {
        strcpy(caDispname,caUsername);
    }
    lLimit = atol(caLimit);
    if(utStrIsSpaces(caPassword) || strncmp(caPassword,"********",8)==0) {
        iPass = 0;
    }
    else {
        if(strcmp(caPassword,caPassword1)!=0) {
             utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0350¿ÚÁî²»Æ¥Åä"));
             return 0;
        }
        else {
        	strcpy(caPass_b,caPassword);
            sprintf(caPassword1,"%s%s",caUsername,caPassword);
            utMd5Ascii22(caPassword1,strlen(caPassword1),NULL,caPassword);
            iPass = 1;
        }
    }

    if(utStrIsSpaces(caUsername) || utStrIsSpaces(caGroupid)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0768ÓÃ»§Ãû»ò×éÃûÎª¿Õ"));
        return 0;
    }
    if(utStrIsSpaces(caUserid)) {
       utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0737ÓÃ»§IDÓÐÎÊÌâ"));
       return 0;
    }
    else {
    	lUserid = atol(caUserid);
    	if(!utStrIsSpaces(caDel)) { /* ÓÃ»§°´ÁËÉ¾³ý¼ü  */
    		    sprintf(caTemp,"delete from ncuser where userid=%s",caUserid);

          psUser = ncUtlGetUserContById(psShmHead,atol(caUserid));
          if(psUser){
          	sprintf(caTemp,"É¾³ýÓÃ»§Ãû:%s,IP:%s,MAC:%s",psUser->dispname,utComHostIp(htonl(psUser->ip)),ncCvtMac(psUser->mac));
          }
    		  ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
         	iReturn = pasDbExecSqlF("delete from ncuser where userid=%s",caUserid);
         	iReturn = pasDbExecSqlF("delete from ncusermail where userid=%s",caUserid);
         	
         	pasDbExecSqlF("delete from ncmobuser where username in (select username from ncuser where userid=%s)",caUserid);
         	
            ncWebDelUserFromBuf(psShmHead,lUserid);
//            if(group_flag==1)
//            	ncWebUserSearch_new(psShmHead, iFd, psMsgHead);
//            else
            	ncWebUserSearch(psShmHead, iFd, psMsgHead);
    	    return 0;
        }
    }
 /*
    if(strcasecmp(utComGetVar_sd(psShmHead,"AddUserCheck","Yes"),"YES")==0) {
        if(!utStrIsSpaces(caMacstr) && strcmp(caMacstr,"00:00:00:00:00:00")!=0) {
            utStrToLower(caMacstr);
            iReturn = ncCvtMacI(caMacstr,caMac);
            if(iReturn < 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0076MACµØÖ·¸ñÊ½²»¶Ô£¬ÕýÈ·µÄ¸ñÊ½Ó¦Îª xx:xx:xx:xx:xx:xx"));
                return 0;
            }
            sprintf(caTemp,"select userid,username from ncuser where mac = '%s' and userid <> %lu",caMacstr,lUserid);
  //          printf("caTemp=%s\n",caTemp);
            iReturn = pasDbOneRecord("select userid,username from ncuser where mac = :mac and userid <> :uid",
                      2,"mac",UT_TYPE_STRING,caMacstr,
                        "uid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_LONG,4,&lUserid0,
                            UT_TYPE_STRING,31,caUsername0);
            if(iReturn == 0 && lUserid != lUserid0) { /* MACµØÖ·ÖØ¸´  */
 /*               utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0078MACµØÖ·ÖØ¸´, ÓÃ»§Ãû %s"),caUsername0);
                return 0;
            }                
        }
        if(lIp > 0) {
            iReturn = pasDbOneRecord("select userid,username from ncuser where ip = :ip and userid <> :uid",
                         2,"ip",UT_TYPE_ULONG,lIp,
                           "uid",UT_TYPE_LONG,lUserid,
                         UT_TYPE_LONG,4,&lUserid0,
                         UT_TYPE_STRING,31,caUsername0);
            if(iReturn == 0 && lUserid != lUserid0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0061IPµØÖ·ºÍÓÃ»§%sÖØ¸´"),caUsername0);
                return 0;
            }
        }
    }
*/
    if((atol(caUsestatus)==1)&&(strlen(caPort)>0)&&(strcasecmp(utComGetVar_sd(psShmHead,"PortBase","No"),"Yes") == 0) ){
    	iReturn = pasDbExecSqlF("update ncuser set usestatus=0 where position='%s' and userid!=%s",caPort,caUserid);
    	psUserInfo = ncUtlGetUserInfoByPort(psShmHead,caPort);
        if(psUserInfo) {
            if(psUserInfo->psUser && psUserInfo->lId != atol(caUserid)) psUserInfo->psUser->cValid = 0;
            ncUtlDoLogout(psShmHead,psUserInfo," User info Modified");
        }
    }
    lUse = atol(caUse);
  	if(iPass == 0) {
    		    sprintf(caTemp,"update ncuser set username='%s',dispname='%s',compname='%s',groupid=%s,ip=%lu,useflags=%d,mac='%s',email='%s',addtime=%lu,position='%s',idfytype='%s',idfyno='%s',pointphyname='%s',idfyunit='%s',country='%s',expiretime=%lu,usestatus=%lu,sex='%s',vlanid=%d where userid=%s",
                                 caUsername,caDispname,caCompname,caGroupid,lIp,lUse,caMacstr,caEmail,lTime,caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,lExpiretime,atol(caUsestatus),caSex,atol(caVlanid),caUserid);
	      
	           ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
	           

      	iReturn = pasDbExecSqlF("update ncuser set username='%s',dispname='%s',compname='%s',groupid=%s,ip=%lu,useflags=%d,mac='%s',email='%s',addtime=%lu,position='%s',idfytype='%s',idfyno='%s',pointphyname='%s',idfyunit='%s',country='%s',localflag=%lu,expiretime=%lu,usestatus=%lu,sex='%s',userlevel=%d,vlanid=%d where userid=%s",
                                 caUsername,caDispname,caCompname,caGroupid,lIp,lUse,caMacstr,caEmail,lTime,caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,atol(caLocalflag),lExpiretime,atol(caUsestatus),caSex,atol(caUserlevel),atol(caVlanid),caUserid);
                                 
               //½«ÓÃ»§ÃÜÂëµÄÃ÷ÎÄÐ´Èëµ½	ncmobuser
		     char caPass[32];
		     	sprintf(caTemp,"select password from ncmobuser where username='%s' ",caUsername);
			  	iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,32,caPass);
			  	if(iReturn==1403){     
			  	
		        		iReturn=pasDbExecSqlF("insert into ncmobuser (username,password,tel) values('%s','%s','%s')",caUsername,caPass_b,caEmail);
			  	}
		      else{
		      
		      	iReturn=pasDbExecSqlF("update ncmobuser set tel='%s' where username='%s' ",caEmail,caUsername);
		      }                    
                                 
   	}
   	else {
            sprintf(caTemp,"update ncuser set username='%s',dispname='%s',compname='%s',groupid=%s,ip=%lu,password='%s',useflags=%d,mac='%s',email='%s',addtime=%lu,position='%s',idfytype='%s',idfyno='%s',pointphyname='%s',idfyunit='%s',country='%s',localflag=%lu,vlanid=%d where userid=%s",
                                 caUsername,caDispname,caCompname,caGroupid,lIp,caPassword,lUse,caMacstr,caEmail,lTime,caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,atol(caLocalflag),atol(caVlanid),caUserid);

           ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
                                
       	iReturn = pasDbExecSqlF("update ncuser set username='%s',dispname='%s',compname='%s',groupid=%s,ip=%lu,password='%s',useflags=%d,mac='%s',email='%s',addtime=%lu,position='%s',idfytype='%s',idfyno='%s',pointphyname='%s',idfyunit='%s',country='%s',localflag=%lu,expiretime=%lu,usestatus=%lu,sex='%s',userlevel=%d,vlanid=%d where userid=%s",
                                 caUsername,caDispname,caCompname,caGroupid,lIp,caPassword,lUse,caMacstr,caEmail,lTime,caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,atol(caLocalflag),lExpiretime,atol(caUsestatus),caSex,atol(caUserlevel),atol(caVlanid),caUserid);
                                 
                                 
                                 
            //½«ÓÃ»§ÃÜÂëµÄÃ÷ÎÄÐ´Èëµ½	ncmobuser
		     char caPass[32];
		     	sprintf(caTemp,"select password from ncmobuser where username='%s' ",caUsername);
			  	iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,32,caPass);
			  	if(iReturn==1403){     	
		        		iReturn=pasDbExecSqlF("insert into ncmobuser (username,password,tel) values('%s','%s','%s')",caUsername,caPass_b,caEmail);
			  	}
		      else{
		      	pasDbExecSqlF("update ncmobuser set password='%s',tel='%s' where username='%s' ",caPass_b,caEmail,caUsername);
		      }                      
                                 
                                 
                                 
   	}   
    
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0699ÐÞ¸Ä³ö´í"));
        pasDbRollback(NULL);
    }
    else {
        iReturn = ncMailCtlAddMailList(lUserid,caEmail);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0735ÓÃ»§EmailÖØ¸´"));
            pasDbRollback(NULL);
            return 0;
        }
        ncWebModiUserFromBuf(psShmHead,lUserid);
        pasDbCommit(NULL);
    }
    if(!utStrIsSpaces(caPolid)) {
        long polid,polid0;
        polid0 = atol(caPolid);
        lCount=0;
        iReturn = pasDbOneRecord("select count(*) from nclimsumm where gtype = 1 and id = :userid",1,
                            "userid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_LONG,4,&lCount);
        if(lCount== 0) {
            if(polid0 > 0||atol(caMpid)>0||atol(caTpid)>0) {
                unsigned long lSid = pasGetSid("nclimsumm","sid");
            sprintf(caTemp,"insert into nclimsumm (gtype,id,polid,qospolid,mailctlid) values \
                                                  (1,%lu,%lu,%lu,%lu) ",lUserid,polid0,atol(caTpid),atol(caMpid));
            ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
                iReturn = pasDbExecSqlF("insert into nclimsumm (sid,gtype,id,polid,qospolid,mailctlid) values \
                                                  (%lu,1,%lu,%lu,%lu,%lu) ",lSid,lUserid,polid0,atol(caTpid),atol(caMpid));
            }
        }
        else {
            if(polid0 >=0||atol(caMpid)>=0||atol(caTpid)>=0) {
            sprintf(caTemp,"update nclimsumm set polid = %lu where gtype = 1 and id = %lu",polid0,lUserid);
	           utMsgDelVar(psMsgHead,"opmsg");
	           utMsgAdd_s(&psMsgHead,"opmsg",caTemp);
	           ncOperWriteLog(psMsgHead);
            	
                iReturn = pasDbExecSqlF("update nclimsumm set polid = %lu,qospolid=%lu,mailctlid=%lu where gtype = 1 and id = %lu",
                                       polid0,atol(caTpid),atol(caMpid),lUserid);
            }
        }  
        if(iReturn != 0) {
            printf(" Insert Error %s \n", pasDbErrorMsg(NULL));
        }          
    }
  pasDbCommit(NULL);  
    utMsgDelVar(psMsgHead,"groupid");
//    if(group_flag==1){
//    	ncWebUserSearch_new(psShmHead, iFd, psMsgHead);
//    }
    if(caOpt[0] == 'O') { /* ÔÚÏßÓÃ»§  */
        ncWebShowUserStatus(psShmHead,iFd,psMsgHead);
    }
    else {
        ncWebUserSearch(psShmHead, iFd, psMsgHead);
    }
    return 0;
}



/* Ôö¼ÓÓÃ»§  */
int ncWebUserAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caUsername[33],caGroupid[33],caIp[20],caMac[33],caMacstr[42],caUse[10];
   	char caDispname[33],caAddtime[32],caLasttime[32];
   	long lUserid,lGroupid;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lUse,lUserid0;
   	long lGroupid0;
   	unsigned long lIp,lExpiretime;
   	int iPass,iAddFlags;
   	char caPassword[32],caPassword1[32];
   	char caCompname[32];
   	char caDate[32],caUserid[16],caUsername0[32];
    char caOk[16],caSave[16],caEmail[64];
    char caAtest[16];
    char caPolid[16];
    char caTemp[1024];
    char caLocalflag[16];
    char caUserlevel[16];
    char caExpiretime[32];
    char caPort[36],caIdfytype[4],caIdfyno[20],caPointphyname[20],caIdfyunit[32],caCountry[4];
    ncUserInfo *psUserInfo;
    char caUsername2[32];
    char caSex[8];
    char caVlanid[12];
    char caMpid[12],caTpid[12];
    int group_flag,lUserid2;
    char caPass_b[32];
    group_flag=0;
    memset(caUsername2,0,sizeof(caUsername2));
    iReturn=dsCltGetSessionValue(2,"dispname",UT_TYPE_STRING,30,caUsername2,
                           "groupid",UT_TYPE_LONG,4,&lGroupid0);
    if((iReturn==0)&&(lGroupid0>0)){
    	group_flag=1;
    }
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
    strcpy(caDate,utTimFormat("%Y-%m-%d %H:%M:%S",time(0)));
   	iReturn = utMsgGetSomeNVar(psMsgHead,30,
   	      "userid",      UT_TYPE_STRING, 12, caUserid,
					"username",    UT_TYPE_STRING, 31, caUsername,
					"compname",    UT_TYPE_STRING, 31, caCompname,
					"groupid",     UT_TYPE_STRING, 12, caGroupid,
					"password",    UT_TYPE_STRING, 16, caPassword,
					"password1",   UT_TYPE_STRING, 16, caPassword1,
					"ip",          UT_TYPE_STRING, 20, caIp,
					"mac",         UT_TYPE_STRING, 40, caMacstr,
					"email",       UT_TYPE_STRING, 63, caEmail,
					"use",       UT_TYPE_STRING, 9,  caUse,
					"dispname",    UT_TYPE_STRING, 31, caDispname,
					"save",        UT_TYPE_STRING, 15, caSave,
					"atest",       UT_TYPE_STRING, 12, caAtest,
					"polid",       UT_TYPE_STRING, 15, caPolid,
					"addtime",     UT_TYPE_STRING, 31, caAddtime,
					"lasttime",    UT_TYPE_STRING, 31, caLasttime,
					"ok",          UT_TYPE_STRING, 15, caOk,
					"idfyno",      UT_TYPE_STRING,18,caIdfyno,
					"idfyunit",    UT_TYPE_STRING,30,caIdfyunit,
					"country",     UT_TYPE_STRING,3,caCountry,
					"port",        UT_TYPE_STRING,32,caPort,
					"pointphyname",UT_TYPE_STRING,16,caPointphyname,
					"idfytype",      UT_TYPE_STRING,3,caIdfytype,
					"localflag",   UT_TYPE_STRING,1,caLocalflag,
					"expiretime",  UT_TYPE_STRING,20,caExpiretime,
					"sex",         UT_TYPE_STRING,1,caSex,
					"userlevel",   UT_TYPE_STRING,1,caUserlevel,
					"VlanID",      UT_TYPE_STRING,10,caVlanid,
					"m_pid",       UT_TYPE_STRING,10,caMpid,
					"t_pid",       UT_TYPE_STRING,10,caTpid);

     lUserid2=0;
     iReturn = pasDbOneRecord("select userid from ncuser where username = :username ",
                         1,"username",UT_TYPE_STRING,caUsername,                  
                            UT_TYPE_LONG,4,&lUserid2);
     if(lUserid2>0){
     	
     	if(strcasecmp(utComGetVar_sd(psShmHead,"HotelVesion","No"),"Yes")==0){
     	
     	 pasDbExecSqlF("update ncuser set username='%d',usestatus=0,useflags=1 where userid=%d",time(0),lUserid2);
        }
        else{
        	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0781ÓÃ»§ÖØ¸´"));
            pasDbRollback(NULL);
            return 0;
        }                    
    }                     


	 if(strlen(caExpiretime)>0){
//	 	lExpiretime=utTimStrToLong("%Y-%m-%d %H:%M:%S",caExpiretime);
    lExpiretime=atol(caExpiretime);
	 	lExpiretime=time(0)+lExpiretime*86400;
		}
		else{
			lExpiretime=0;
		}
    if(!utStrIsSpaces(caAtest)) { /* ×Ô¶¯¼ì²âMACºÍ¼ÆËã»úÃû  */
        char *pSelect,*pBuf;
        char caSmt[255];
        if(!utStrIsValidIp(caIp)) {
            lIp = 0;
        }
        else {
            char *pDev;
            lIp = ntohl(pasIpcvtLong(caIp));     
            pDev = utComGetVar_s(psShmHead,"admindev");
            ncUtlGetMacAddress(lIp,caMac,caCompname,pDev);
            
//            iReturn = ncUtlGetMacFromWins(lIp,caMac,caCompname);
            strcpy(caMacstr,ncCvtMac(caMac));
        }
        psDbHead = utPltInitDb();
        utPltPutVar(psDbHead,"userid",caUserid);
        utPltPutVar(psDbHead,"username",caUsername);
        utPltPutVar(psDbHead,"dispname",caDispname);
        utPltPutVar(psDbHead,"password",caPassword);
        utPltPutVar(psDbHead,"password1",caPassword1);
        utPltPutVarF(psDbHead,"groupid", "%ld",lGroupid);
        utPltPutVar(psDbHead,"ip", caIp);
        utPltPutVar(psDbHead,"mac", caMacstr);
        utPltPutVar(psDbHead,"compname", caCompname);
        utPltPutVar(psDbHead,"lasttime", caLasttime);
        utPltPutVar(psDbHead,"addtime", caAddtime);
        utPltPutVar(psDbHead,"email", caEmail);
        
        utPltPutVar(psDbHead,"idfyno",caIdfyno);
        utPltPutVar(psDbHead,"idfyunit",caIdfyunit);
        utPltPutVar(psDbHead,"country",caCountry);
        utPltPutVar(psDbHead,"port",caPort);
        utPltPutVar(psDbHead,"VlanID",caVlanid);
        utPltPutVar(psDbHead,"pointphyname",caPointphyname);
        utPltPutVar(psDbHead,"idfytype",caIdfytype);
        utPltPutVar(psDbHead,"qospolid",caTpid);
        utPltPutVar(psDbHead,"mailctlid",caMpid);
        
        if(atol(caLocalflag)==1)
        utPltPutVar(psDbHead,"localflag","checked");
        
        if(atol(caUserlevel)>0)
        utPltPutVar(psDbHead,"userlevel","checked");
        
        iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
        if(iReturn == 0 && lGroupid0 > 0) { /* Ö¸¶¨ÓÃ»§  */
            sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu or pid=%lu order by groupid",
                 lGroupid0,lGroupid0);
        }
        else {
		    sprintf(caSmt,"select groupname,groupid from ncgroup order by groupid");
        }
        pBuf = pasUtlCvtSqlSelect(caSmt);
        if(pBuf) {
            pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
            free(pBuf);
            utPltPutVar(psDbHead,"dept", pSelect);
            free(pSelect);
        }
            
        pSelect = pasUtlCvtInputSelect("use",caUse,ncLang("0852Õý³£,0;½ûÓÃ,1;Ãâ¼à¿Ø,9;"));
        utPltPutVar(psDbHead,"use", pSelect);
        free(pSelect);
        if(group_flag==1){
        	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"group/nc_form_useredit_new.htm");
        		return 0;
        }
        else{
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_useredit.htm");
        return 0;
        }
    }
    utStrDelSpaces(caUsername);
    utStrDelSpaces(caIp);
    if(utStrIsSpaces(caIp)) {
        lIp = 0;
    }
    else {
        if(!utStrIsValidIp(caIp)) {
            lIp = 0;
        }
        else {
            lIp = ntohl(pasIpcvtLong(caIp));            
        }
    }
    lGroupid = atol(caGroupid);
    if(utStrIsSpaces(caDispname)) {
        strcpy(caDispname,caUsername);
    }
    if(utStrIsSpaces(caPassword)||strncmp(caPassword,"********",8)==0) {
        iPass = 0;
    }
    else {
        if(strcmp(caPassword,caPassword1)!=0) {
             utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0350¿ÚÁî²»Æ¥Åä"));
             return 0;
        }
        else {
        	strcpy(caPass_b,caPassword);
            sprintf(caPassword1,"%s%s",caUsername,caPassword);
            utMd5Ascii22(caPassword1,strlen(caPassword1),NULL,caPassword);
            iPass = 1;
        }
    }

    if(utStrIsSpaces(caUsername) || utStrIsSpaces(caGroupid)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0768ÓÃ»§Ãû»ò×éÃûÎª¿Õ"));
        return 0;
    }
    
    if(utStrIsSpaces(caUserid)) {
        lUserid =  pasGetSid("ncuser","userid");
        iAddFlags = 1;
        if(lUserid < 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0737ÓÃ»§IDÓÐÎÊÌâ"));
            return 0;
    	}        
    }
    else {
    	lUserid = atol(caUserid);
    	iAddFlags = 0;
    }

    if(iAddFlags) { /* Ôö¼Ó¼ÇÂ¼  */
        if(strcasecmp(utComGetVar_sd(psShmHead,"AddUserCheck","Yes"),"YES")==0) {
            if(!utStrIsSpaces(caMacstr)) {
                utStrToLower(caMacstr);
                iReturn = ncCvtMacI(caMacstr,caMac);
                if(iReturn < 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0076MACµØÖ·¸ñÊ½²»¶Ô£¬ÕýÈ·µÄ¸ñÊ½Ó¦Îª xx:xx:xx:xx:xx:xx"));
                    return 0;
                }
                iReturn = pasDbOneRecord("select userid,username from ncuser where mac = :mac and userid <> :uid",
                         2,"mac",UT_TYPE_STRING,caMacstr,
                           "uid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_LONG,4,&lUserid0,
                            UT_TYPE_STRING,31,caUsername0);
                if(iReturn == 0) { /* MACµØÖ·ÖØ¸´  */
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0078MACµØÖ·ÖØ¸´, ÓÃ»§Ãû %s"),caUsername0);
                    return 0;
                }                
            }
            if(lIp > 0) {
                iReturn = pasDbOneRecord("select userid,username from ncuser where ip = :ip and userid <> :uid",
                         2,"ip",UT_TYPE_ULONG,lIp,
                           "uid",UT_TYPE_LONG,lUserid,
                         UT_TYPE_LONG,4,&lUserid0,
                         UT_TYPE_STRING,31,caUsername0);
                if(iReturn == 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0061IPµØÖ·ºÍÓÃ»§%sÖØ¸´"),caUsername0);
                    return 0;
                }
            }
        }
        lUse = atol(caUse);
        if(iPass == 0) {
            sprintf(caTemp,"Add User:[insert into ncuser (userid,username,dispname,compname,groupid,ip,mac,addtime,lasttime,useflags,password,email,position,idfytype,idfyno,pointphyname,idfyunit,country) \
        	    values(%lu,'%s','%s','%s',%lu,%lu,'%s',%lu,%lu, %d,'********','%s','%s','%s','%s','%s','%s','%s')]",
                lUserid,caUsername,caDispname,caCompname,lGroupid,lIp,caMacstr,time(0),time(0),lUse,caEmail,caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry);

	        ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
            iReturn = pasDbExecSqlF("insert into ncuser (userid,username,dispname,compname,groupid,ip,mac,addtime,lasttime,useflags,password,email,position,idfytype,idfyno,pointphyname,idfyunit,country,localflag,expiretime,usestatus,sex,userlevel,vlanid) values \
                                                         (%lu,'%s','%s','%s',%lu,%lu,'%s',%lu,%lu, %d,'********','%s','%s','%s','%s','%s','%s','%s',%lu,%lu,1,'%s',%d,%d)",
                                                         lUserid,caUsername,caDispname,caCompname,lGroupid,lIp,caMacstr,time(0),time(0),lUse,caEmail,caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,atol(caLocalflag),lExpiretime,caSex,atol(caUserlevel),atol(caVlanid));
        }
        else {
        	 sprintf(caTemp,"insert into ncuser (userid,username,dispname,compname,groupid,ip,mac,addtime,lasttime,useflags,password,email,position,idfytype,idfyno,pointphyname,idfyunit,country,localflag,expiretime,usestatus,sex)\
        	    values(%lu,'%s','%s','%s','%lu',%lu,'%s',%lu,%lu,%d,'%s','%s','%s','%s','%s','%s','%s','%s',%lu,%lu,0,'%s')",
                 lUserid,caUsername,caDispname,caCompname,lGroupid,lIp,caMacstr,time(0),time(0),lUse,caPassword,caEmail,caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,atol(caLocalflag),lExpiretime,caSex);
           ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        	
            iReturn = pasDbExecSqlF("insert into ncuser (userid,username,dispname,compname,groupid,ip,mac,addtime,lasttime,useflags,password,email,position,idfytype,idfyno,pointphyname,idfyunit,country,localflag,expiretime,usestatus,sex,userlevel,vlanid) values \
                (%lu,'%s','%s','%s','%lu',%lu,'%s',%lu,%lu,%d,'%s','%s','%s','%s','%s','%s','%s','%s',%lu,%lu,1,'%s',%d,%d)",
                 lUserid,caUsername,caDispname,caCompname,lGroupid,lIp,caMacstr,time(0),time(0),lUse,caPassword,caEmail,caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,atol(caLocalflag),lExpiretime,caSex,atol(caUserlevel),atol(caVlanid));
        }        
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0781ÓÃ»§ÖØ¸´"));
            pasDbRollback(NULL);
        }
        else { /* ½«ÓÊ¼þµØÖ·¼ÓÈëÓÊ¼þÁÐ±í  */
 //½«ÓÃ»§ÃÜÂëµÄÃ÷ÎÄÐ´Èëµ½	ncmobuser
     char caPass[32];
     	sprintf(caTemp,"select password from ncmobuser where username='%s' ",caUsername);
	  	iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,32,caPass);
	  	if(iReturn==1403){     	
        		iReturn=pasDbExecSqlF("insert into ncmobuser (username,password,tel) values('%s','%s','%s')",caUsername,caPass_b,caEmail);
	  	}
      else{
      	pasDbExecSqlF("update ncmobuser set password='%s',tel='%s' where username='%s' ",caPass_b,caEmail,caUsername);
      }	
        	
        	
            iReturn = ncMailCtlAddMailList(lUserid,caEmail);
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0735ÓÃ»§EmailÖØ¸´"));
                pasDbRollback(NULL);
                return 0;
            }
            pasDbCommit(NULL);
            if((strlen(caPort)>0)&&(strcasecmp(utComGetVar_sd(psShmHead,"PortBase","No"),"Yes") == 0)){
    	        iReturn = pasDbExecSqlF("update ncuser set usestatus=0 where position='%s' and userid!=%s",caPort,caUserid);
    	        psUserInfo = ncUtlGetUserInfoByPort(psShmHead,caPort);
                if(psUserInfo) {
                    if(psUserInfo->psUser && psUserInfo->lId != atol(caUserid)) psUserInfo->psUser->cValid = 0;
                    ncUtlDoLogout(psShmHead,psUserInfo,"User Info Modified");
                }
            }
            sprintf(caUserid,"%d",lUserid);
            ncWebModiUserFromBuf(psShmHead,lUserid);
            utMsgAdd_s(&psMsgHead,"userid",caUserid);
        }
    }        
    else { /* ÐÞ¸Ä¼ÇÂ¼  */
        if(strcasecmp(utComGetVar_sd(psShmHead,"AddUserCheck","Yes"),"YES")==0) {
            if(!utStrIsSpaces(caMacstr)) {
                utStrToLower(caMacstr);
                iReturn = ncCvtMacI(caMacstr,caMac);
                if(iReturn < 0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0076MACµØÖ·¸ñÊ½²»¶Ô£¬ÕýÈ·µÄ¸ñÊ½Ó¦Îª xx:xx:xx:xx:xx:xx"));
                    return 0;
                }
                iReturn = pasDbOneRecord("select userid,username from ncuser where mac = :mac",
                       1,"mac",UT_TYPE_STRING,caMacstr,
                       UT_TYPE_LONG,4,&lUserid0,
                       UT_TYPE_STRING,31,caUsername0);
                if(iReturn == 0 && lUserid != lUserid0) { /* MACµØÖ·ÖØ¸´  */
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0078MACµØÖ·ÖØ¸´, ÓÃ»§Ãû %s"),caUsername0);
                    return 0;
                }                
            }
            if(lIp > 0) {
                iReturn = pasDbOneRecord("select userid,username from ncuser where ip = :ip",
                         1,"ip",UT_TYPE_ULONG,lIp,
                         UT_TYPE_LONG,4,&lUserid0,
                         UT_TYPE_STRING,31,caUsername0);
                if(iReturn == 0 && lUserid != lUserid0) {
                    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0061IPµØÖ·ºÍÓÃ»§%sÖØ¸´"),caUsername0);
                    return 0;
                }
            }
        }
        lUse = atol(caUse);
    	if(iPass == 0) {
    		  sprintf(caTemp,"Modify user:[update ncuser set username='%s',dispname='%s',compname='%s',groupid=%s,ip=%lu,lasttime=%lu,useflags=%d,mac='%s',email='%s',addtime=%lu,position='%s',idfytype='%s',idfyno='%s',pointphyname='%s',idfyunit='%s',country='%s' where userid=%s]",
                                 caUsername,caDispname,caCompname,caGroupid,lIp,time(0),lUse,caMacstr,caEmail,time(0),caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,caUserid);
	  
	         ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        	iReturn = pasDbExecSqlF("update ncuser set username='%s',dispname='%s',compname='%s',groupid=%s,ip=%lu,lasttime=%lu,useflags=%d,mac='%s',email='%s',addtime=%lu,position='%s',idfytype='%s',idfyno='%s',pointphyname='%s',idfyunit='%s',country='%s',localflag=%lu,sex='%s',userlevel=%d where userid=%s",
                                 caUsername,caDispname,caCompname,caGroupid,lIp,time(0),lUse,caMacstr,caEmail,time(0),caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,atol(caLocalflag),caSex,atol(caUserlevel),caUserid);
          if(iReturn==0){
          	
          	//½«ÓÃ»§ÃÜÂëµÄÃ÷ÎÄÐ´Èëµ½	ncmobuser
			     char caPass[32];
			     	sprintf(caTemp,"select password from ncmobuser where username='%s' ",caUsername);
				  	iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,32,caPass);
				  	if(iReturn==1403){     	
			        		iReturn=pasDbExecSqlF("insert into ncmobuser (username,password,tel) values('%s','%s','%s')",caUsername,caPass_b,caEmail);
				  	}
			      else{
	
			      	pasDbExecSqlF("update ncmobuser set tel='%s' where username='%s' ",caEmail,caUsername);
			      }
          	
          	
          }
    	}
    	else {
    		   sprintf(caTemp,"Modify user:[update ncuser set username='%s',dispname='%s',compname='%s',groupid=%s,ip=%lu,password='%s',lasttime=%lu,useflags=%d,mac='%s',email='%s',addtime=%lu,position='%s',idfytype='%s',idfyno='%s',pointphyname='%s',idfyunit='%s',country='%s' where userid=%s]",
                                 caUsername,caDispname,caCompname,caGroupid,lIp,caPassword,time(0),lUse,caMacstr,caEmail,time(0),caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,caUserid);
          ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
        	iReturn = pasDbExecSqlF("update ncuser set username='%s',dispname='%s',compname='%s',groupid=%s,ip=%lu,password='%s',lasttime=%lu,useflags=%d,mac='%s',email='%s',addtime=%lu,position='%s',idfytype='%s',idfyno='%s',pointphyname='%s',idfyunit='%s',country='%s',localflag=%lu,sex='%s',userlevel=%d,vlanid=%d where userid=%s",
                                 caUsername,caDispname,caCompname,caGroupid,lIp,caPassword,time(0),lUse,caMacstr,caEmail,time(0),caPort,caIdfytype,caIdfyno,caPointphyname,caIdfyunit,caCountry,atol(caLocalflag),caSex,atol(caUserlevel),atol(caVlanid),caUserid);
                                 
                                 
           
            //½«ÓÃ»§ÃÜÂëµÄÃ÷ÎÄÐ´Èëµ½	ncmobuser
			     char caPass[32];
			     	sprintf(caTemp,"select password from ncmobuser where username='%s' ",caUsername);
				  	iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,32,caPass);
				  	if(iReturn==1403){     	
			        		iReturn=pasDbExecSqlF("insert into ncmobuser (username,password,tel) values('%s','%s','%s')",caUsername,caPass_b,caEmail);
				  	}
			      else{
			      	pasDbExecSqlF("update ncmobuser set password='%s',tel='%s' where username='%s' ",caPass_b,caEmail,caUsername);
			      }
           
                                 
    	}        
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0699ÐÞ¸Ä³ö´í"));
            pasDbRollback(NULL);
        }
        else {
            iReturn = ncMailCtlAddMailList(lUserid,caEmail);
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0735ÓÃ»§EmailÖØ¸´"));
                pasDbRollback(NULL);
                return 0;
            }
            ncWebModiUserFromBuf(psShmHead,lUserid);
            pasDbCommit(NULL);
        }
    }
    if(!utStrIsSpaces(caPolid)) {
        long polid,polid0;
        polid0 = atol(caPolid);
        iReturn = pasDbOneRecord("select polid from nclimsumm where gtype = 1 and id = :userid",1,
                            "userid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_ULONG,4,&polid);
        if(iReturn != 0) {
           if(polid0 > 0||atol(caMpid)>0||atol(caTpid)>0) {
            unsigned long lSid = pasGetSid("nclimsumm","sid");
            sprintf(caTemp,"insert into nclimsumm (sid,gtype,id,polid) values(%lu,1,%lu,%lu) ",lSid,lUserid,polid0);
            ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
            
            
                iReturn = pasDbExecSqlF("insert into nclimsumm (sid,gtype,id,polid,qospolid,mailctlid) values \
                                                  (%lu,1,%lu,%lu,%lu,%lu) ",lSid,lUserid,polid0,atol(caTpid),atol(caMpid));
            }
        }
        else {
            if(polid0 > 0||atol(caMpid)>0||atol(caTpid)>0) {
            sprintf(caTemp,"update nclimsumm set polid = %lu where gtype = 1 and id = %lu",polid0,lUserid);
	           utMsgDelVar(psMsgHead,"opmsg");
	           utMsgAdd_s(&psMsgHead,"opmsg",caTemp);
	           ncOperWriteLog(psMsgHead);
            	
                iReturn = pasDbExecSqlF("update nclimsumm set polid = %lu,qospolid=%lu,mailctlid=%lu where gtype = 1 and id = %lu",
                                       polid0,atol(caTpid),atol(caMpid),lUserid);
            }
        }            
    }
    if(!utStrIsSpaces(caOk)) {
        psDbHead = utPltInitDb();
        if(iAddFlags) { /* Ôö¼Ó */
            utPltPutVar(psDbHead,"message",ncLang("0779ÓÃ»§Ôö¼Ósuccess"));
        }
        else {
            utPltPutVar(psDbHead,"message",ncLang("0776ÓÃ»§ÐÞ¸Äsuccess"));
        }    
        utPltPutVar(psDbHead,"userid",caUserid);
        utPltPutVar(psDbHead,"username",caUsername);
        utPltPutVar(psDbHead,"dispname",caDispname);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_msg_useredit.htm");
    }
    else {
        utMsgDelVar(psMsgHead,"userid");
        utMsgAdd_s(&psMsgHead,"userid",caUserid);
        ncWebDispUserAddForm(psShmHead,iFd,psMsgHead);
    }
    pasDbCommit(NULL);
    return 0;
}



/* ÏÔÊ¾²¿ÃÅ¹ÜÀíÒ³Ãæ  */
int ncWebDispDeptForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caGroupname[34];
   	int iNum1;
   	long lUserid,lGroupid;
   	char caGroupid[16];
   	char *pBuf,*pSelect;
    utPltDbHead *psDbHead;
   	int iReturn;

    psDbHead = utPltInitDb();
    pBuf = pasUtlCvtSqlSelect("select groupname,groupid from ncgroup where groupid <>0 order by groupid");
    if(pBuf == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771ÓÃ»§ÐÅÏ¢"),ncLang("0153²¿ÃÅÁÐ±í²»´æÔÚµ"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    strcpy(caGroupid,"\0");
    pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
    if(pSelect == NULL) {
        free(pBuf);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771ÓÃ»§ÐÅÏ¢"),ncLang("0404ÄÚ´æ³ö´í"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    utPltPutVar(psDbHead,"dept", pSelect);
    free(pSelect);
    
    pSelect= pasUtlCvtInputSelect("gid",caGroupid,pBuf);
    if(pSelect == NULL) {
        free(pBuf);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771ÓÃ»§ÐÅÏ¢"),ncLang("0404ÄÚ´æ³ö´í"));
        utPltFreeDb(psDbHead);
        return 0;
    }
    utPltPutVar(psDbHead,"mdept", pSelect);
    free(pSelect);
    free(pBuf);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_group.htm");
    return 0;
}

/* ²¿ÃÅÔö¼Ó  */
int ncWebDeptAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caGroupname[34];
   	long lGroupid;
   	int iReturn;
    char caTemp[1024];
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"groupname",    UT_TYPE_STRING, 31, caGroupname);
    utStrDelSpaces(caGroupname);
    if(utStrIsSpaces(caGroupname)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0837Ôö¼Ó²¿ÃÅ"),ncLang("0154²¿ÃÅÃû³ÆÎª¿Õ"));
        return 0;
    }
    lGroupid =  pasGetSid("ncgroup","groupid"); 
    
    sprintf(caTemp,"insert into ncgroup (groupname,groupid,addtime) values ('%s',%ld,%lu)",caGroupname,lGroupid,time(0));
	  utMsgDelVar(psMsgHead,"opmsg");
	  utMsgAdd_s(&psMsgHead,"opmsg",caTemp);
	  ncOperWriteLog(psMsgHead);
    iReturn = pasDbExecSqlF("insert into ncgroup (groupname,groupid,addtime) values ('%s',%ld,%lu)",caGroupname,lGroupid,time(0));
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0837Ôö¼Ó²¿ÃÅ"),ncLang("0163²¿ÃÅÔö¼Ó³ö´í"));
        pasDbRollback(NULL);
    }
    else  {
        pasDbCommit(NULL);
        ncWebDispDeptForm(psShmHead,iFd,psMsgHead);
    }
}

/* ²¿ÃÅÉ¾³ý  */
int ncWebDeptDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caGroupid[34];
   	int iReturn;
   	long lCount;
    long lGroupid;
    char caTemp[1024];
    char caUsername2[32];
   memset(caUsername2,0,sizeof(caUsername2));
   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"groupid",    UT_TYPE_STRING, 31, caGroupid);

    utStrDelSpaces(caGroupid);
    if(utStrIsSpaces(caGroupid)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0471É¾³ý²¿ÃÅ"),ncLang("0154²¿ÃÅÃû³ÆÃûÎª¿Õ"));
        return 0;
    }
    /* ÏÈ¼ì²éÏàÓ¦²¿ÃÅÖÐµÄÓÃ»§ÊÇ·ñ´æÔÚ  */
    lGroupid = atol(caGroupid);
    iReturn = pasDbOneRecord("select count(*) from ncuser where groupid = :gid",1,
                             "gid", UT_TYPE_LONG,lGroupid,
                             UT_TYPE_LONG,sizeof(long),&lCount);
    if(iReturn == 0 && lCount > 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0471É¾³ý²¿ÃÅ"),ncLang("0164²¿ÃÅÖÐ°üº¬ÓÐÓÃ»§£¬²»ÄÜÉ¾³ý"));
        return 0;
    }
    sprintf(caTemp,"delete  from ncgroup where groupid=%s ",caGroupid);
    ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
    iReturn = pasDbExecSqlF("delete  from ncgroup where groupid=%s ",caGroupid);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0471É¾³ý²¿ÃÅ"),ncLang("0156²¿ÃÅÉ¾³ý³ö´í"));
        pasDbRollback(NULL);
    }
    else  {
        pasDbCommit(NULL);
        ncWebDispDeptForm(psShmHead,iFd,psMsgHead);
    }
}


/* ²¿ÃÅÐÞ¸Ä  */
int ncWebDeptModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caGroupid[34],caName[32];
   	int iReturn;
   	long lCount;
    long lGroupid;
    char caTemp[1024];
    char caUsername2[32];
   memset(caUsername2,0,sizeof(caUsername2));
   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
					"gid",    UT_TYPE_STRING, 31, caGroupid,
					"newname",UT_TYPE_STRING, 31, caName);

    utStrDelSpaces(caGroupid);
    if(utStrIsSpaces(caGroupid)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0698ÐÞ¸Ä²¿ÃÅ"),ncLang("0154²¿ÃÅÃû³ÆÃûÎª¿Õ"));
        return 0;
    }
    /* ÏÈ¼ì²éÏàÓ¦²¿ÃÅÖÐµÄÓÃ»§ÊÇ·ñ´æÔÚ  */
    lGroupid = atol(caGroupid);
    sprintf(caTemp,"update ncgroup set groupname='%s',addtime=%lu where groupid=%s ",
                        caName,time(0),caGroupid);
	  utMsgDelVar(psMsgHead,"opmsg");
	  utMsgAdd_s(&psMsgHead,"opmsg",caTemp);
    ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp);
    iReturn = pasDbExecSqlF("update ncgroup set groupname='%s',addtime=%lu where groupid=%s ",
                        caName,time(0),caGroupid);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0698ÐÞ¸Ä²¿ÃÅ"),ncLang("0162²¿ÃÅÐÞ¸Ä³ö´í"));
        pasDbRollback(NULL);
    }
    else  {
        pasDbCommit(NULL);
        ncWebDispDeptForm(psShmHead,iFd,psMsgHead);
    }
}


/* É¾³ýÑ¡¶¨µÄÒ»Åú¼ÇÂ¼  */
int ncWebDelSomeUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int i,iReturn;
	char caTemp[16];
	char caTemp1[256];
	char *p;
	char iDel;
	char caUsername2[32];
	long lGroupid0,groupid_flag;
	ncUserCont *psUser;
	 ncUserInfo *psUserInfo;
  memset(caUsername2,0,sizeof(caUsername2));
  lGroupid0=0;
  groupid_flag=0;
 iReturn=dsCltGetSessionValue(2,"dispname",UT_TYPE_STRING,30,caUsername2,
                        "groupid", UT_TYPE_LONG,4,&lGroupid0);
 if((iReturn==0)&&(lGroupid0>0)){
 	groupid_flag=1;
}
	iDel = 0;
    utMsgPrintMsg(psMsgHead);
    for(i=0;i<60;i++) {
        sprintf(caTemp,"ch%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caTemp);
        if(p) {
        	   sprintf(caTemp1,"Delete user:[delete  from ncuser where userid=%s ]",p);
  //É¾³ýÓÃ»§Ê±ÏÈ½«Õâ¸öÓÃ»§ÌßÏÂÏß      	   
        	  psUserInfo = ncGetUserInfoById(psShmHead,atol(p));
            if(psUserInfo) {
                ncUtlDoLogout(psShmHead,psUserInfo,"User Kick Out");
            }
        	   
        	   
        	   
        	   
        	psUser = ncUtlGetUserContById(psShmHead,atol(p));
          if(psUser){
         
          	sprintf(caTemp1,"É¾³ýÓÃ»§Ãû:%s,IP:%s,MAC:%s",psUser->dispname,utComHostIp(htonl(psUser->ip)),ncCvtMac(psUser->mac));
          }
        	   
             ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp1);
          	iReturn = pasDbExecSqlF("delete from ncusermail where userid=%s",p);
          	
          	            	pasDbExecSqlF("delete from ncmobuser where username in (select username from ncuser where userid=%s)",p);
          	
            iReturn = pasDbExecSqlF("delete  from ncuser where userid=%s ",p);
            

         
            
            
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0489É¾³ýÓÃ»§"),ncLang("0731ÓÃ»§ %s É¾³ý³ö´í"),p);
                pasDbRollback(NULL);
                return 0;
            }
            else {
                unsigned long lUserid;
                lUserid = atol(p);
                ncWebDelUserFromBuf(psShmHead,lUserid);
            	iDel++;
            }
        }
    }
    if(iDel > 0) {
        pasDbCommit(NULL);
    }

    	return ncWebUserSearch(psShmHead, iFd,psMsgHead);
}

/* ÓÃ»§ÐÅÏ¢µ¼³ö  */
int ncWebUserExportCsv(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    FILE *fp;
    char caOutFile[128];
    char sqlbuf[2048];
    char caPath[64];
    char caFile[64];
    long lUserid,lGroupid;
    char caCond[128];
    char caCond0[64];
    char caUsername[32],caGroupname[32],caIp[16],caMac[32],caDispname[32];
    char caEmail[64],caDate[32];
    unsigned long lAddtime,lIp;
    int  iReturn;
     char caOpt[2048],caTemp[1024];
     char caPass[32];
   	long lGid,lGroupid0,iNum;
    pasDbCursor *psCur;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

    memset(caOpt,0,sizeof(caOpt));
    	iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    
    	if(iReturn == 0 && lGroupid0 > 0) {
    		sprintf(caTemp,"select groupid from ncgroup where groupid=%d or pid=%d ",lGroupid0,lGroupid0);
        psCur=pasDbOpenSql(caTemp,0);
        if(psCur == NULL) {
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746ÓÃ»§²éÑ¯"),ncLang("0171²éÑ¯Êý¾Ý³ö´í"));
        	return 0;
    		}
    		lGid=0;
    		iNum=0;
    		
   	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    while((iReturn == 0||iReturn==1405))
   	    {
   	    	if(iNum==0){
   	    		sprintf(caOpt,"%d",lGid);
   	    	}
   	    	else{
   	    		sprintf(caOpt+strlen(caOpt),",%d",lGid);
   	    	}
   	    	iNum++;
   	       
   	    	lGid=0;
   	    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    }
   	     pasDbCloseCursor(psCur);
      }


	utMsgGetSomeNVar(psMsgHead,1,"cond",UT_TYPE_STRING,127,caCond);
    sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog"));
    sprintf(caFile,"%lu.csv",utStrGetId());
    sprintf(caOutFile,"%s/%s",caPath,caFile);
    fp = fopen(caOutFile,"w");
    if(fp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0769ÓÃ»§Êý¾Ýµ¼³ö"),ncLang("0637ÎÄ¼þ %s´´½¨³ö´í"),caOutFile);
        return 0;
    }
    sprintf(sqlbuf,"SELECT a.userid,a.username,a.groupid,a.dispname,b.groupname,a.ip,a.mac,a.email,a.addtime,c.password from ncuser a,ncgroup b left join  ncmobuser c on (a.username=c.username) where a.groupid = b.groupid ");
    if(strlen(caOpt)>0){
    	sprintf(sqlbuf+strlen(sqlbuf)," and a.groupid in (%s) ",caOpt);
    }
    if(strlen(caCond)!=0)
    {
    	sprintf(caCond0,"'%c%s%c'",'%',caCond,'%');
    	 if(utStrIsValidIp(caCond)) {
                        lIp = ntohl(utComHostAddress(caCond));
                 	    sprintf(sqlbuf+strlen(sqlbuf),"  and (a.username like %s or a.dispname like %s or b.groupname like %s or a.mac like %s or a.email like %s or a.ip = %lu)",
                   		    caCond0,caCond0,caCond0,caCond0,caCond0,lIp);
          }
         else {
                sprintf(sqlbuf+strlen(sqlbuf),"  and (a.username like %s or a.dispname like %s or b.groupname like %s or a.mac like %s or a.email like %s)",
                   		     caCond0,caCond0,caCond0,caCond0,caCond0);
              }
    }
    
    sprintf(sqlbuf+strlen(sqlbuf)," order by a.groupid,a.userid ");
    
 //   printf("sqlbuf=%s\n",sqlbuf);
    psCur = pasDbOpenSql(sqlbuf,0);
    if(psCur == NULL) {
        fclose(fp);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746ÓÃ»§²éÑ¯"),ncLang("0171²éÑ¯Êý¾Ý³ö´í"));
        return 0;
    } 
    if(strcasecmp(utComGetVar_sd(psShmHead,"ExpUserpasswd","No"),"Yes")==0){
    	fprintf(fp,"ÓÃ»§ID,ÐÕÃû,µÇÂ¼Ãû,ÃÜÂë,×éÃû,×éID,IPµØÖ·,MACµØÖ·,ÊÖ»úºÅ/E-Mail,¼ÓÈëÊ±¼ä\n");
    }
    else{    
      fprintf(fp,ncLang("0736ÓÃ»§ID,ÐÕÃû,µÇÂ¼Ãû,×éÃû,×éID,IPµØÖ·,MACµØÖ·,E-Mail,¼ÓÈëÊ±¼ä\n"));
    }
    while(1) {
    	memset(caPass,0,sizeof(caPass));
      	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,31,caUsername,
  	                               UT_TYPE_LONG,4,&lGroupid,
  	                               UT_TYPE_STRING,31,caDispname,
  	                               UT_TYPE_STRING,31,caGroupname,
  	                               UT_TYPE_ULONG,4,&lIp,
  	                               UT_TYPE_STRING,31,caMac,
  	                               UT_TYPE_STRING,63,caEmail,
  	                               UT_TYPE_ULONG,4,&lAddtime,
  	                               UT_TYPE_STRING,30,caPass);
        if(iReturn == 0||iReturn==1405) {  
        	    if(strcasecmp(utComGetVar_sd(psShmHead,"ExpUserpasswd","No"),"Yes")==0){
        	    	fprintf(fp,"%lu,\"%s\",\"%s\",\"%s\",\"%s\",%lu,\"%s\",\"%s\",\"%s\",\"%s\"\n",
             lUserid,caDispname,caUsername,caPass,caGroupname,lGroupid,utComHostIp(htonl(lIp)),caMac,caEmail,utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime));

             }
        	   else{   
        	    	
    	          fprintf(fp,"%lu,\"%s\",\"%s\",\"%s\",%lu,\"%s\",\"%s\",\"%s\",\"%s\"\n",
             lUserid,caDispname,caUsername,caGroupname,lGroupid,utComHostIp(htonl(lIp)),caMac,caEmail,utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime));
            }
        }
        else {
            break;
        }
    }            
    fclose(fp);
    utPltFileDownload(iFd,"application/text",caPath,caFile,caFile);
    remove(caOutFile);
    return 0;
}

char *pasUtlCvtSqlSelectTreeBm()
{
     pasDbCursor *psCur;
     char *pBuf;
     unsigned long lMaxSize,l,l1;
     int iReturn;
     char caSmt[1024];
     char caTemp1[64],caTemp2[64];
     struct groupData_s{
     		unsigned long groupid;
     		char groupname[32];
     		unsigned long pid;
     } groupData;
     struct groupData_s *pGroup0,*pGroup1;
     unsigned long lGroupid,lCount,iNum,iNum1,lPid,m,i;
     char caGroupname[32];
     lMaxSize = 256;
     l=0;
     pGroup0=NULL;
     pGroup1=NULL;
     sprintf(caSmt,"select count(*) from ncgroup where level=0");
     lCount=0;
     pasDbOneRecord(caSmt,0,UT_TYPE_LONG,4,&lCount);
     if(lCount==0) return NULL;
     pGroup0=malloc(lCount*sizeof(struct groupData_s));
     if(pGroup0==NULL) return NULL;
     sprintf(caSmt,"select groupid,groupname,pid from ncgroup where level=0");
     psCur = pasDbOpenSql(caSmt,0);
     if(psCur == NULL) {
        return NULL;
     }
     iNum=0;
     lGroupid=0;
     memset(caGroupname,0,sizeof(caGroupname));
     lPid=0;
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,31,caGroupname,
                                    UT_TYPE_LONG,4,&lPid);
                                   
     while((iReturn==0)||(iReturn==1405)){
     	pGroup0[iNum].groupid=lGroupid;
     	strcpy(pGroup0[iNum].groupname,caGroupname);
     	pGroup0[iNum].pid=lPid;
     	
     	iNum++;
     	lGroupid=0;
	     memset(caGroupname,0,sizeof(caGroupname));
	     lPid=0;
	     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
	                                    UT_TYPE_STRING,31,caGroupname,
	                                    UT_TYPE_LONG,4,&lPid);
     }                               
     pasDbCloseCursor(psCur);
       
       
     sprintf(caSmt,"select count(*) from ncgroup where level=1");
     lCount=0;
     iNum1=0;
     pasDbOneRecord(caSmt,0,UT_TYPE_LONG,4,&lCount);
     if(lCount>0){
		     pGroup1=malloc(lCount*sizeof(struct groupData_s));
		     if(pGroup1==NULL) return NULL;
		     sprintf(caSmt,"select groupid,groupname,pid from ncgroup where level=1");
		     psCur = pasDbOpenSql(caSmt,0);
		     if(psCur == NULL) {
		        return NULL;
		     }
		     iNum1=0;
		     lGroupid=0;
		     memset(caGroupname,0,sizeof(caGroupname));
		     lPid=0;
		     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
		                                    UT_TYPE_STRING,31,caGroupname,
		                                    UT_TYPE_LONG,4,&lPid);
		                                    
		     while((iReturn==0)||(iReturn==1405)){
		     	pGroup1[iNum1].groupid=lGroupid;
		     	strcpy(pGroup1[iNum1].groupname,caGroupname);
		     	pGroup1[iNum1].pid=lPid;
		     	
		     	iNum1++;
		     	lGroupid=0;
			     memset(caGroupname,0,sizeof(caGroupname));
			     lPid=0;
			     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
			                                    UT_TYPE_STRING,31,caGroupname,
			                                    UT_TYPE_LONG,4,&lPid);
		     }                               
		     pasDbCloseCursor(psCur);  
       }

       pBuf = malloc(lMaxSize+l);
       memset(pBuf,0,sizeof(pBuf));
       for(i=0;i<iNum;i++){
       	   l1 = strlen(pGroup0[i].groupname) + 32;

	         if(lMaxSize < l1 + l+1) {
	            pBuf = realloc(pBuf,lMaxSize + 256);
	            if(pBuf == NULL) {
	            	free(pGroup0);
	            	free(pGroup1);
	            }
	            else{
                lMaxSize = lMaxSize + 256;
            }
           }
           l=l+l1;
           utStrAddF(pBuf,"%s,%d;",pGroup0[i].groupname,pGroup0[i].groupid);
           for(m=0;m<iNum1;m++){
           	if(pGroup0[i].groupid==pGroup1[m].pid){
           		l1 = strlen(pGroup1[m].groupname) + 32;
	         		if(lMaxSize < l1 + l+1) {
	            	pBuf = realloc(pBuf,lMaxSize + 256);
	            	if(pBuf == NULL) {
	            		free(pGroup0);
	            		free(pGroup1);
	            	}
	            	else{
                	lMaxSize = lMaxSize + 256;
            	}
            }
                l=l+l1;
           		  utStrAddF(pBuf,"&nbsp;&nbsp;.%s,%d;",pGroup1[m].groupname,pGroup1[m].groupid);
           	}
           }
          }
       if(pGroup0)                             
    			free(pGroup0);

    	if(pGroup1)
	       free(pGroup1);
   
   return pBuf;

}


char *pasUtlCvtSqlSelectTreeBmA(char *pStr)
{
     pasDbCursor *psCur;
     char *pBuf;
     unsigned long lMaxSize,l,l1;
     int iReturn;
     char caSmt[1024];
     char caTemp1[64],caTemp2[64];
     struct groupData_s{
     		unsigned long groupid;
     		char groupname[32];
     		unsigned long pid;
     } groupData;
     struct groupData_s *pGroup0,*pGroup1;
     unsigned long lGroupid,lCount,iNum,iNum1,lPid,m,i;
     char caGroupname[32];
     iNum1=0;
     l = strlen(pStr);
     lMaxSize = 256;
     pGroup0=NULL;
     pGroup1=NULL;
     sprintf(caSmt,"select count(*) from ncgroup where level=0");
     lCount=0;
     pasDbOneRecord(caSmt,0,UT_TYPE_LONG,4,&lCount);
     if(lCount==0) return NULL;
     pGroup0=malloc(lCount*sizeof(struct groupData_s));
     if(pGroup0==NULL) return NULL;
     sprintf(caSmt,"select groupid,groupname,pid from ncgroup where level=0");
     psCur = pasDbOpenSql(caSmt,0);
     if(psCur == NULL) {
        return NULL;
     }
     iNum=0;
     lGroupid=0;
     memset(caGroupname,0,sizeof(caGroupname));
     lPid=0;
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,31,caGroupname,
                                    UT_TYPE_LONG,4,&lPid);
                                   
     while((iReturn==0)||(iReturn==1405)){
     	pGroup0[iNum].groupid=lGroupid;
     	strcpy(pGroup0[iNum].groupname,caGroupname);
     	pGroup0[iNum].pid=lPid;
     	
     	iNum++;
     	lGroupid=0;
	     memset(caGroupname,0,sizeof(caGroupname));
	     lPid=0;
	     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
	                                    UT_TYPE_STRING,31,caGroupname,
	                                    UT_TYPE_LONG,4,&lPid);
     }                               
     pasDbCloseCursor(psCur);
       
       
     sprintf(caSmt,"select count(*) from ncgroup where level=1");
     lCount=0;
     pasDbOneRecord(caSmt,0,UT_TYPE_LONG,4,&lCount);
     if(lCount>0){
		     pGroup1=malloc(lCount*sizeof(struct groupData_s));
		     if(pGroup1==NULL) return NULL;
		     sprintf(caSmt,"select groupid,groupname,pid from ncgroup where level=1");
		     psCur = pasDbOpenSql(caSmt,0);
		     if(psCur == NULL) {
		        return NULL;
		     }
		     iNum1=0;
		     lGroupid=0;
		     memset(caGroupname,0,sizeof(caGroupname));
		     lPid=0;
		     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
		                                    UT_TYPE_STRING,31,caGroupname,
		                                    UT_TYPE_LONG,4,&lPid);
		                                    
		     while((iReturn==0)||(iReturn==1405)){
		     	pGroup1[iNum1].groupid=lGroupid;
		     	strcpy(pGroup1[iNum1].groupname,caGroupname);
		     	pGroup1[iNum1].pid=lPid;
		     	
		     	iNum1++;
		     	lGroupid=0;
			     memset(caGroupname,0,sizeof(caGroupname));
			     lPid=0;
			     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGroupid,
			                                    UT_TYPE_STRING,31,caGroupname,
			                                    UT_TYPE_LONG,4,&lPid);
		     }                               
		     pasDbCloseCursor(psCur);  
       }

       pBuf = malloc(lMaxSize+l);
       strcpy(pBuf,pStr);
       for(i=0;i<iNum;i++){
       	   l1 = strlen(pGroup0[i].groupname) + 32;
	         if(lMaxSize < l+l1 + 1) {
	            pBuf = realloc(pBuf,lMaxSize + 256);
	            if(pBuf == NULL) {
	            	free(pGroup0);
	            	free(pGroup1);
	            }
	            else{
                lMaxSize = lMaxSize + 256;
            }
           }
           utStrAddF(pBuf,"%s,%d;",pGroup0[i].groupname,pGroup0[i].groupid);
           l=l+l1;
           for(m=0;m<iNum1;m++){
           	if(pGroup0[i].groupid==pGroup1[m].pid){
           		l1 = strlen(pGroup1[m].groupname) + 32;
	         		if(lMaxSize < l+l1 + 1) {
	            	pBuf = realloc(pBuf,lMaxSize + 256);
	            	if(pBuf == NULL) {
	            		free(pGroup0);
	            		free(pGroup1);
	            	}
	            	else{
                	lMaxSize = lMaxSize + 256;
            	}
            }
            l=l+l1;
           		  utStrAddF(pBuf,"&nbsp;&nbsp;.%s,%d;",pGroup1[m].groupname,pGroup1[m].groupid);
           	}
           }
          }
       if(pGroup0)                             
    			free(pGroup0);
    	if(pGroup1)
	       free(pGroup1);
	    
   return pBuf;

}
//´æ·Å´ø¿í£¬ÉÏÍø¿ØÖÆºÍÓÊ¼þ²ßÂÔ
 struct tempProcy_s{
 	  int userid;                   //ÓÃ»§ID
 	  char tc_name[16];             //´ø¿í²ßÂÔ
 	  char mail_name[16];          //ÓÊ¼þÀ¹½Ø²ßÂÔ
 	  char procy_name[32];          //ÉÏÍø¿ØÖÆ²ßÂÔ
 	};	
int ncsUtlLoolProcyById(struct tempProcy_s *arrIn,int iSumArr,int id)
{
   int bottom,middle,top;
    int n;
    top = iSumArr - 1;
    bottom = 0;
   
    while(top >= bottom) {
        middle = (top + bottom) / 2;  

        if(arrIn[middle].userid==id) {
            return middle;
        }
        else if(arrIn[middle].userid>id) {
            top = middle -1;
        }
        else {
            bottom = middle + 1;
        }
    }
    return (-1);
}

//ÓÃ»§ÁÐ±í

int ncWebUserSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caWhere[2048],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caSort[16],caSortby[6];
    char caUsername[32],caGroupname[32],caMac[32],caIp[32],caDate[32];
    char caGroupid[16];
    char caDispname[36];
    char caSql[4024];
    long lLocalflag,lUsestatus;   
    char caEmail[128];
    char caPosition[36],caIdfyno[20];
    char idfytype[4],idfyunit[64],idfytype_desc[32];
    struct tempProcy_s *pProcy;
    char *pSelect,*pBuf;
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lCount,lIp,lAddtime,lExpiretime;
    int i,iNum;
    char *pSear;
    char caSmt[4512],caOrder[64];
    utPltDbHead *psDbHead;
   	int iReturn,lAddtime1;
   	long lGroupid0,lRowNum,lUseflags,lStartRec,lUseflags0,lTotRec;
   	char caLevel[16];
   	char caTemp[4024];
   	char caOpt[2048];
   	long lGid;
   	long nGid[1000];
   	char caGroupbuf[2048];
   	long iNum1;       
    long lGid2;
   	long group_flag;
   	long id,lSum,index;
   	char p_name[32],t_name[20],m_name[20];
   	long lPid;
 	pasDbCursor *psCur;
 	  unsigned char *pHash;    
    pasLHashInfo sHashInfo; 
 	 struct stData_g{
      unsigned long gid;
    	char caGroupname[32];
    };
    struct stData_g *psData;
 	  char caGroups[2048];
 	  strcpy(caGroups,getDsGroupid());
 	
// 	printf("caGroups=%s\n",caGroups);
#ifdef LDEBUG
    printf(" Pid == %d \n",getpid());
#endif    
// 	utMsgPrintMsg(psMsgHead);
 	    pHash = (unsigned char *)pasLHashInit(2500,2500,sizeof(struct stData_g),0,4);    
    if(pHash == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","·ÖÅäÄÚ´æ³ö´í","·ÖÅäÄÚ´æ³ö´í");
         return (-1);
    }
 	
 	    sprintf(caTemp,"select groupid,groupname from ncgroup limit 0,2499 ");
// printf("caTemp=%s\n",caTemp);
      psCur=pasDbOpenSql(caTemp,0);
     if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg2.htm",ncLang("0444È¡·þÎñ"),ncLang("0549Êý¾Ý¿â³ö´í"));
            return 0;
        }
        lGroupid=0;
        memset(caGroupname,0,sizeof(caGroupname));      
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&lGroupid,
      	                             UT_TYPE_STRING,31,caGroupname);

       while(iReturn==0||iReturn==1405)
       {
       	
       	 psData= (struct stData *)pasLHashLookA(pHash,&lGroupid);  
       	 if(psData){       	
       	 	strcpy(psData->caGroupname,caGroupname);
       	 }  
       	
        lGroupid=0;
        memset(caGroupname,0,sizeof(caGroupname));      
        iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&lGroupid,
      	                             UT_TYPE_STRING,31,caGroupname);
       }
      pasDbCloseCursor(psCur);  
 	
 	
 	//½«²ßÂÔ×°ÔØµ½ÄÚ´æ
  	

 	sprintf(caTemp,"select count(*) from nclimsumm where gtype=1 and (polid>0 or qospolid>0 or mailctlid>0) ");
 	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
 	pProcy=(struct tempProcy_s*)malloc((lCount+1)*sizeof(struct tempProcy_s));
 	
 	if(pProcy==NULL){
  	    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("1863·ÖÅäÄÚ´æ³ö´í"),ncLang("1863·ÖÅäÄÚ´æ³ö´í"));
        return 0;
  }
 	
 	sprintf(caTemp,"select a.id,b.name,c.name,d.name from nclimsumm a  \
 	 left join ncprocy b on (a.polid=b.id) left join ncmailstrategy c on (a.mailctlid=c.id) left join nctcstrategy d on (a.qospolid=d.id) where gtype=1 and (a.polid>0 or a.qospolid>0 or a.mailctlid>0) order by a.id ");
//printf("pid=%d\n",getpid());
//sleep(15); 
// printf("caTemp=%s\n",caTemp);	
 	  psCur = pasDbOpenSql(caTemp,0);
    if(psCur == NULL) {
    	free(pProcy);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0613ÍøÕ¾·ÃÎÊ²éÑ¯"),ncLang("0549Êý¾Ý¿â³ö´í"));
        return 0;
    }
    lSum=0;
    id=0;
    memset(p_name,0,sizeof(p_name));
    memset(t_name,0,sizeof(t_name));
    memset(m_name,0,sizeof(m_name));
     iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&id,
      	                             UT_TYPE_STRING,31,p_name,
      	                             UT_TYPE_STRING,16,m_name,
      	                             UT_TYPE_STRING,16,t_name);
      	                            
    while((iReturn==0)||(iReturn==1405))
    {
    	if(strlen(p_name)>0||strlen(t_name)>0||strlen(m_name)>0){
    	pProcy[lSum].userid=id;
    	strcpy(pProcy[lSum].tc_name,t_name);
    	strcpy(pProcy[lSum].mail_name,m_name);
    	strcpy(pProcy[lSum].procy_name,p_name);
    	lSum++;
      }
    	id=0;
      memset(p_name,0,sizeof(p_name));
      memset(t_name,0,sizeof(t_name));
      memset(m_name,0,sizeof(m_name));
      iReturn = pasDbFetchInto(psCur,
      	                             UT_TYPE_LONG,4,&id,
      	                             UT_TYPE_STRING,31,p_name,
      	                             UT_TYPE_STRING,16,m_name,
      	                             UT_TYPE_STRING,16,t_name);
    	                              
    }    
    pasDbCloseCursor(psCur);
    
    
    
 	
 	
 	
	iReturn = utMsgGetSomeNVar(psMsgHead,8,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					        "CurPg",      UT_TYPE_STRING, 12, caCurPg,
					        "groupid",    UT_TYPE_STRING, 12, caGroupid,
   	                "cond",       UT_TYPE_STRING, 63, caCond,
   	                "sort",       UT_TYPE_STRING, 15, caSort,
   	                "sortby",     UT_TYPE_STRING, 2,  caSortby,
   	                "where",      UT_TYPE_STRING, 510,caWhere,
   	                "level",      UT_TYPE_STRING, 10,caLevel );
if(strlen(caGroupid)>0){
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

}

    sprintf(caCond0,"'%c%s%c'",'%',caCond,'%');
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    if(pSear = utMsgGetVar_s(psMsgHead,"search")) {
       lCurPg = 1;
       strcpy(caWhere,"\0");
    }
    lGroupid0 = 0;
    if(utStrIsSpaces(caWhere)) {
    	iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
      group_flag=0;
    	if(iReturn == 0 && lGroupid0 > 0) {
    		group_flag=1;
    		
    		/*
  if(strcasecmp(utComGetVar_sd(psShmHead,"xmga_user","No"),"Yes")==0){
  	sprintf(caTemp,"select groupid from ncgroup where groupid=%d ",lGroupid0);
  }
  else{  		
    		sprintf(caTemp,"select groupid from ncgroup where groupid=%d or pid=%d ",lGroupid0,lGroupid0);
  }	
    		
        psCur=pasDbOpenSql(caTemp,0);
        if(psCur == NULL) {
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746ÓÃ»§²éÑ¯"),ncLang("0171²éÑ¯Êý¾Ý³ö´í"));
        	return 0;
    		}
    		lGid=0;
    		iNum=0;
    		memset(caOpt,0,sizeof(caOpt));
   	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    while((iReturn == 0||iReturn==1405))
   	    {
   	    	if(iNum==0){
   	    		sprintf(caOpt,"%d",lGid);
   	    	}
   	    	else{
   	    		sprintf(caOpt+strlen(caOpt),",%d",lGid);
   	    	}
   	    	iNum++;
   	       
   	    	lGid=0;
   	    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    	

   	    }
   	   
   	  
   	     pasDbCloseCursor(psCur);
   	     if(strlen(caOpt)==0){
   	    		sprintf(caOpt,"%d",lGroupid0);
   	    	}
   	   */  	
   	    	
   	  strcpy(caOpt,caGroups);  
   	//  printf("caOpt=%s\n",caOpt);  	
        	if(!utStrIsSpaces(caCond)) {
            	if(strcmp(caCond,ncLang("0928È«²¿¼à¿Ø")) == 0 ||strcmp(caCond,ncLang("0391Ãâ¼à¿Ø")) == 0 ||strcmp(caCond,ncLang("0331½ûÓÃ")) == 0  ) {
                	if(strcmp(caCond,ncLang("0928È«²¿¼à¿Ø"))==0) {
                    	lUseflags0 = 0;
                	}
                	else if(strcmp(caCond,ncLang("0331½ûÓÃ"))==0) {
                    	lUseflags0 = 1;
                	}
                	else {
                    	lUseflags0 = 9;
                	}
                	sprintf(caWhere,"where  a.groupid in (%s)  and (a.useflags = %d)",
                   	caOpt,lUseflags0);
                
            	}
            	else {
                    if(utStrIsValidIp(caCond)) {
                        lIp = ntohl(utComHostAddress(caCond));
                 	    sprintf(caWhere,"where  a.groupid in (%s)  and (a.username like %s or a.dispname like %s or b.groupname like %s or a.mac like %s or a.email like %s or a.ip = %lu or a.position='%s' or a.idfyno='%s')",
                   		    caOpt,caCond0,caCond0,caCond0,caCond0,caCond0,lIp,caCond,caCond);
                    }
                    else {
                  	    sprintf(caWhere,"where  a.groupid in (%s)  and (a.username like %s or a.dispname like %s or b.groupname like %s or a.mac like %s or a.email like %s or a.position='%s' or a.idfyno='%s')",
                   		     caOpt,caCond0,caCond0,caCond0,caCond0,caCond0,caCond,caCond);
                    }
            	}
        	}
        	else {
            	sprintf(caWhere,"where  a.groupid in(%s)",caOpt);
        	}
    	}
    	else {
    	    lGroupid0 = 0;
        	if(!utStrIsSpaces(caCond)) {   	                
            	if(strcmp(caCond,ncLang("0850Õý³£")) == 0 ||strcmp(caCond,ncLang("0391Ãâ¼à¿Ø")) == 0 ||strcmp(caCond,ncLang("0331½ûÓÃ")) == 0  ) {
                	if(strcmp(caCond,ncLang("0850Õý³£"))==0) {
                    	lUseflags0 = 0;
                	}
                	else if(strcmp(caCond,ncLang("0331½ûÓÃ"))==0) {
                    	lUseflags0 = 1;
                	}
                	else {
                    	lUseflags0 = 9;
                	}
 //               	sprintf(caWhere,"where a.userid > 10    and a.useflags = %d",
                	
                	sprintf(caWhere,"where  a.useflags = %d",
                   	lUseflags0);
            	}
            	else {
                    if(utStrIsValidIp(caCond)) {
                        lIp = ntohl(utComHostAddress(caCond));
                        sprintf(caWhere,"where  (a.username like %s or a.dispname like %s or b.groupname like %s or a.ip = %lu or a.mac like %s or a.email like %s or a.position='%s' or a.idfyno='%s')",
                   	        caCond0,caCond0,caCond0,lIp,caCond0,caCond0,caCond,caCond);
                        
 //               	    sprintf(caWhere,"where a.userid > 10  and (a.username like %s or a.dispname like %s or b.groupname like %s or a.ip = %lu or a.mac like %s or a.email like %s or a.position='%s' or a.idfyno='%s')",
 //                  	        caCond0,caCond0,caCond0,lIp,caCond0,caCond0,caCond,caCond);
                    }
                    else {
                    sprintf(caWhere,"where  (binary a.username like %s or binary a.dispname like %s or binary b.groupname like %s or a.mac like %s or a.email like %s or a.position='%s' or a.idfyno='%s')",
                           	caCond0,caCond0,caCond0,caCond0,caCond0,caCond,caCond);
 //                   	sprintf(caWhere,"where a.userid > 10  and (binary a.username like %s or binary a.dispname like %s or binary b.groupname like %s or a.mac like %s or a.email like %s or a.position='%s' or a.idfyno='%s')",
 //                          	caCond0,caCond0,caCond0,caCond0,caCond0,caCond,caCond);
                    }
            	}

        	}
        	else {
  //          	strcpy(caWhere,"where a.userid > 10 ");
  	strcpy(caWhere,"where 1=1 ");
        	}
    	}
    	if(!utStrIsSpaces(caGroupid)) {
    		if(atol(caGroupid)!=0){
    		sprintf(caWhere+strlen(caWhere)," and a.groupid in (%s) ",getGroupid(atol(caGroupid)));
    	}
    	else{
    		sprintf(caWhere+strlen(caWhere)," and a.groupid=0 ");
    	}
    //		if(atol(caLevel)==0){
 //   if(strcasecmp(utComGetVar_sd(psShmHead,"xmga_user","No"),"Yes")==0){
 //   	utStrAddF(caWhere," and a.groupid = %s ",caGroupid);
 //   }
 //   else{
//    			sprintf(caWhere+strlen(caWhere)," and a.groupid in (%s)",caGroupbuf);
//    		}
    			
    //		}
    //		else
   //     	utStrAddF(caWhere," and a.groupid = %s ",caGroupid);
    	}
    }
// printf("aaaaaaaaaaaaaa\n");   
    strcpy(caOrder,"\0");
   	if(!utStrIsSpaces(caSort)) {
       	utStrAddF(caOrder," order by %s ",caSort);
        if(caSortby[0] == '1') {     
             utStrAddF(caOrder," desc ");
        }
   	}

if(strstr(caOrder,"order")==NULL){
	strcpy(caOrder," order by a.userid desc ");
}
    
    lRowNum = 28;
    
    lRowNum=atol(utComGetVar_sd(psShmHead,"RowNum","28"));
    
    lStartRec = (lCurPg - 1) * lRowNum;    
    sprintf(caSmt,"select count(*) from ncuser a left join ncgroup b on a.groupid=b.groupid %s",caWhere); 
//printf("caSmt=%s\n",caSmt);
    iReturn = pasDbOneRecord(caSmt,0,
                            UT_TYPE_LONG,4,&lCount);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746ÓÃ»§²éÑ¯"),ncLang("0171²éÑ¯Êý¾Ý³ö´í"));
        return 0;
    } 
  printf("select a.useflags useflags,a.userid userid,a.dispname dispname,a.username username,b.groupname groupname,a.ip ip,a.mac mac,a.lasttime adate,a.email email,a.localflag,a.position,a.idfyno,a.addtime,a.idfytype,a.idfyunit,a.expiretime,a.usestatus,b.pid \
                   from ncuser a left join ncgroup b on (a.groupid=b.groupid) %s %s limit %lu,%lu\n",caWhere,caOrder,lStartRec,lRowNum);                       
    psCur = pasDbOpenSqlF("select a.useflags useflags,a.userid userid,a.dispname dispname,a.username username,b.groupname groupname,a.ip ip,a.mac mac,a.lasttime adate,a.email email,a.localflag,a.position,a.idfyno,a.addtime,a.idfytype,a.idfyunit,a.expiretime,a.usestatus,b.pid \
                   from ncuser a left join ncgroup b on (a.groupid=b.groupid) %s %s limit %lu,%lu",caWhere,caOrder,lStartRec,lRowNum);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746ÓÃ»§²éÑ¯"),ncLang("0171²éÑ¯Êý¾Ý³ö´í"));
        return 0;
    }        
    psDbHead = utPltInitDb();
    
    lLocalflag=0;
    memset(caGroupname,0,sizeof(caGroupname));
    memset(caPosition,0,sizeof(caPosition));
    memset(caIdfyno,0,sizeof(caIdfyno));
    memset(idfytype,0,sizeof(idfytype));
    memset(idfyunit,0,sizeof(idfyunit));
    lExpiretime=0;
    lPid=0;
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUseflags,
  	                               UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,30,caDispname,
  	                               UT_TYPE_STRING,31,caUsername,
  	                               UT_TYPE_STRING,30,caGroupname,
  	                               UT_TYPE_ULONG,4,&lIp,
  	                               UT_TYPE_STRING,31,caMac,
  	                               UT_TYPE_ULONG,4,&lAddtime,
  	                               UT_TYPE_STRING,63,caEmail,
  	                               UT_TYPE_LONG,4,&lLocalflag,
  	                               UT_TYPE_STRING,32,caPosition,
  	                               UT_TYPE_STRING,18,caIdfyno,
  	                               UT_TYPE_LONG,4,&lAddtime1,
  	                               UT_TYPE_STRING,3,idfytype,
  	                               UT_TYPE_STRING,30,idfyunit,
  	                               UT_TYPE_LONG,4,&lExpiretime,
  	                               UT_TYPE_LONG,4,&lUsestatus,
  	                               UT_TYPE_LONG,4,&lPid);
 
   	iNum = 0;
   	while((iReturn == 0||iReturn==1405) && iNum < lRowNum) {
        iNum++;
        if(lUseflags == 0) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0928È«²¿¼à¿Ø"));
        }
        else if(lUseflags == 1) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0331½ûÓÃ"));
        }
        else if(lUseflags == 3) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0929Ö»¼à¿ØIP·ÃÎÊ"));
        }
        else if(lUseflags == 4) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0930¼à¿ØIPºÍHttp"));
        }
        else if(lUseflags == 10) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0627Î´ÉèÖÃ"));
        }
        else {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0391Ãâ¼à¿Ø"));
        }

        if(lUseflags == 0) {
            utPltPutLoopVar(psDbHead,"status2", iNum,ncLang("1864ÔÊÐíÉÏÍø"));
        }
        else if(lUseflags == 1) {
            utPltPutLoopVar(psDbHead,"status2", iNum,ncLang("0331½ûÓÃ"));
        }


         index=ncsUtlLoolProcyById(pProcy,lSum,lUserid);
	    	
	    		if(index!=-1){
	    			if(strlen(pProcy[index].procy_name)>0)
	    				utPltPutLoopVar(psDbHead,"p_name",iNum,pProcy[index].procy_name);
	    			else
	    			  utPltPutLoopVar(psDbHead,"p_name",iNum,ncLang("0627Î´ÉèÖÃ"));
	    			
	    			if(strlen(pProcy[index].tc_name)>0)
	    				utPltPutLoopVar(psDbHead,"t_name",iNum,pProcy[index].tc_name);
	    			else 
	    			  utPltPutLoopVar(psDbHead,"t_name",iNum,ncLang("0627Î´ÉèÖÃ"));
	    			 
	    			if(strlen(pProcy[index].mail_name)>0)	 
	    				utPltPutLoopVar(psDbHead,"m_name",iNum,pProcy[index].mail_name);
	    			else
	    			  utPltPutLoopVar(psDbHead,"m_name",iNum,ncLang("0627Î´ÉèÖÃ"));
	    		}
	    		else{
	    			utPltPutLoopVar(psDbHead,"p_name",iNum,ncLang("0627Î´ÉèÖÃ"));
	    			utPltPutLoopVar(psDbHead,"t_name",iNum,ncLang("0627Î´ÉèÖÃ"));
	    			utPltPutLoopVar(psDbHead,"m_name",iNum,ncLang("0627Î´ÉèÖÃ"));
	    		}



        utPltPutLoopVarF(psDbHead,"useflags", iNum,"%d",lUseflags);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,caUsername);
        utPltPutLoopVar(psDbHead,"dispname", iNum,caDispname);
      	if(lPid>0){
      		psData= (struct stData_g *)pasLHashLook(pHash,&lPid);   
      		if(psData){
      			utPltPutLoopVarF(psDbHead,"groupname",iNum,"%s/%s",psData->caGroupname,caGroupname);
      		}
 //     		utPltPutLoopVarF(psDbHead,"groupname",iNum,"%s/%s",ncUtlGetGroupNameByid(psShmHead,lPid,"Î´Öª"),caGroupname);
      	}
      	else
      	utPltPutLoopVar(psDbHead,"groupname",iNum,caGroupname);
      	
        utPltPutLoopVar(psDbHead,"ip", iNum,utComHostIp(htonl(lIp)));
        utPltPutLoopVar(psDbHead,"mac", iNum,caMac);
        if(lAddtime>0)
        utPltPutLoopVar(psDbHead,"adate", iNum,utTimFormat("%Y-%m-%d %H:%M",lAddtime));
        
        if(lAddtime1>0)
        utPltPutLoopVar(psDbHead,"adddate", iNum,utTimFormat("%Y-%m-%d %H:%M",lAddtime1));
        
        if(lExpiretime>0)
        utPltPutLoopVar(psDbHead,"expiretime", iNum,utTimFormat("%Y-%m-%d %H:%M",lExpiretime));
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"position",iNum,caPosition);
        utPltPutLoopVar(psDbHead,"idfyno",iNum,caIdfyno);
        if(lLocalflag==1)
        	strcpy(caEmail,ncLang("0144²»ÔÊÐí"));
        else
        	strcpy(caEmail,ncLang("0827ÔÊÐí"));
        
        utPltPutLoopVar(psDbHead,"email", iNum,caEmail);
        utPltPutLoopVar(psDbHead,"idfyunit",iNum,idfyunit);
        
        if(strcmp(idfytype,"111")==0)
        	strcpy(idfytype_desc,ncLang("1865Éí·ÝÖ¤"));
        else if(strcmp(idfytype,"133")==0)
          strcpy(idfytype_desc,ncLang("1866Ñ§ÉúÖ¤"));
        else if(strcmp(idfytype,"123")==0)
          strcpy(idfytype_desc,ncLang("1867¾¯¹ÙÖ¤"));
        else if(strcmp(idfytype,"233")==0)
        	strcpy(idfytype_desc,ncLang("1868Ê¿±øÖ¤"));
        else if(strcmp(idfytype,"113")==0)
        	strcpy(idfytype_desc,ncLang("1869»§¿Ú±¡"));	
        else if(strcmp(idfytype,"414")==0)
          strcpy(idfytype_desc,ncLang("1870»¤ÕÕ"));
        else 
        	strcpy(idfytype_desc,ncLang("0431ÆäËü"));
        
        if(lUsestatus==0){
        	char aaa[150]="";
        	strcat(aaa,"<font color=red>");
        	strcat(aaa,ncLang("1871Ê§Ð§"));
        	strcat(aaa,"</font>");
        	utPltPutLoopVar(psDbHead,"ac_desc",iNum,aaa);
        	memset(aaa,0,sizeof(aaa));
           //  utPltPutLoopVar(psDbHead,"ac_desc",iNum,"<font color=red>Ê§Ð§</font>");
       }
         if(lUsestatus==1){
         char aaa[150]="";
        	strcat(aaa,"<font color=green>");
        	strcat(aaa,ncLang("0850Õý³£"));
        	strcat(aaa,"</font>");
        	utPltPutLoopVar(psDbHead,"ac_desc",iNum,aaa);
        	memset(aaa,0,sizeof(aaa));
        //utPltPutLoopVar(psDbHead,"ac_desc",iNum,"<font color=green>Õý³£</font>");
   
      }
        utPltPutLoopVar(psDbHead,"idfytype_desc",iNum,idfytype_desc);
        memset(caGroupname,0,sizeof(caGroupname));
        memset(caPosition,0,sizeof(caPosition));
        memset(caIdfyno,0,sizeof(caIdfyno));
    lLocalflag=0; 
    lExpiretime=0;
    memset(idfytype,0,sizeof(idfytype));
    memset(idfyunit,0,sizeof(idfyunit));
    lPid=0;    
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUseflags,
  	                               UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,30,caDispname,
  	                               UT_TYPE_STRING,31,caUsername,
  	                               UT_TYPE_STRING,30,caGroupname,
  	                               UT_TYPE_ULONG,4,&lIp,
  	                               UT_TYPE_STRING,31,caMac,
  	                               UT_TYPE_ULONG,4,&lAddtime,
  	                               UT_TYPE_STRING,63,caEmail,
  	                               UT_TYPE_LONG,4,&lLocalflag,
  	                               UT_TYPE_STRING,32,caPosition,
  	                               UT_TYPE_STRING,18,caIdfyno,
  	                               UT_TYPE_LONG,4,&lAddtime1,
  	                               UT_TYPE_STRING,3,idfytype,
  	                               UT_TYPE_STRING,30,idfyunit,
  	                               UT_TYPE_LONG,4,&lExpiretime,
  	                               UT_TYPE_LONG,4,&lUsestatus,
  	                               UT_TYPE_LONG,4,&lPid);
    }
    pasDbCloseCursor(psCur);
    if(lCount <= 1) {
        lTotPg = 1;
    }
    else {
        lTotPg = (lCount - 1) / lRowNum + 1;
    }
    lTotRec = lCount;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"sort", caSort);
    utPltPutVar(psDbHead,"sortby", caSortby);
    utPltPutVar(psDbHead,"cond",caCond);
    utPltPutVar(psDbHead,"where",caWhere);

    utPltPutVar(psDbHead,"groupid0",caGroupid);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    if(group_flag ==1) {
        sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu or pid=%lu order by groupname",
                  lGroupid0,lGroupid0);
        pBuf = pasUtlCvtSqlSelectA(caSmt,"\0");
    }
    else {
    	  pBuf = pasUtlCvtSqlSelectTreeBm();
 //       pBuf = pasUtlCvtSqlSelectA("select groupname,groupid from ncgroup order by pid,groupid",
 //              "\0");
    }
  
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("nbm","0",pBuf);
        free(pBuf);
        utPltPutVar(psDbHead,"sgroup", pSelect);
        free(pSelect);
    }
    free(pHash);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_list_user.htm");
    return 0;
}

/* ÐÞ¸ÄÑ¡¶¨ÓÃ»§µÄ²¿ÃÅ  */
int ncWebModiSomeUserBm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int i,iReturn;
	long lGroupid;
	char caTemp[1024];
	char caTemp1[1024];
	char *p;
	char *pBm;
	char caUsername2[32];
	int group_flag,lGroupid0;
	long lSumrow;
	group_flag=0;
  memset(caUsername2,0,sizeof(caUsername2));
  iReturn=dsCltGetSessionValue(2,"dispname",UT_TYPE_STRING,30,caUsername2,
                         "groupid",UT_TYPE_LONG,4,&lGroupid0);
  if((iReturn==0)&&(lGroupid0>0)){
  	group_flag=1;
  }
#ifdef LDEBUG1
	utMsgPrintMsg(psMsgHead);
	printf("Sleep 20 pid=%d \n",getpid());
	sleep(20);
#endif  
lSumrow=atol(utComGetVar_sd(psShmHead,"RowNum","50"));
	pBm = utMsgGetVar_s(psMsgHead,"nbm");
	if(pBm) {
	    if(!utStrIsSpaces(pBm)) {
	        lGroupid = atol(pBm);
            for(i=0;i<lSumrow;i++) {
                sprintf(caTemp,"ch%d",i+1);
                p = utMsgGetVar_s(psMsgHead,caTemp);
                if(p) {                	
                	sprintf(caTemp,"update ncuser set groupid = %lu where userid=%s ",lGroupid,p);
                	sprintf(caTemp1,"%s:[%s]",ncLang("1156ÐÞ¸ÄÓÃ»§²¿ÃÅ"),caTemp);
								  
								   ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp1);
                    iReturn = pasDbExecSqlF("update ncuser set groupid = %lu where userid=%s ",lGroupid,p);
                    if(iReturn != 0) {
                        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0161²¿ÃÅÐÞ¸Ä"),ncLang("0730ÓÃ»§ %s ²¿ÃÅÐÞ¸Ä³ö´í"),p);
                        pasDbRollback(NULL);
                        return 0;
                    }
                    else {
                    	pasDbCommit(NULL);
                        unsigned long lUserid;
                        lUserid = atol(p);
                        ncWebModiUserFromBuf(psShmHead,lUserid);
                    }
                }
            }
        }
    }

    	return ncWebUserSearch(psShmHead, iFd,psMsgHead);
}



int ncIsIpBase()
{
    return 1;
}


/* ÏÔÊ¾²¿ÃÅÊ÷  */
int ncWebListGroupTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
   	char caGroupname[32];
   	char caFname[32];
   	int iNum1;
   	long lGroupid,lGroupid0;
    utPltDbHead *psDbHead;
   	int iReturn,i;

   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
					"plate",    UT_TYPE_STRING, 127, caPlate,
					"fname",    UT_TYPE_STRING, 31,  caFname);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682ÏÔÊ¾ÓÃ»§"),ncLang("0554Êý¾Ý¿â³öÏÖÎÊÌâ£¬ÇëÓë¹ÜÀíÔ±ÁªÏµ"));
        return 0;
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) { /* Ö¸¶¨ÓÃ»§  */
        psCur = pasDbOpenSqlF("select groupname,groupid from ncgroup where groupid = %lu order by \
                           groupname",lGroupid0);
    }
    else {
        psCur = pasDbOpenSqlF("select groupname,groupid from ncgroup order by groupname");
    }
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683ÏÔÊ¾ÓÃ»§Ê÷"),ncLang("0554Êý¾Ý¿â³öÏÖÎÊÌâ£¬ÇëÓë¹ÜÀíÔ±ÁªÏµ"));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,sizeof(long),&lGroupid);
    i = 0;
    iNum1 = 0;
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    while(iReturn == 0) {
        iNum1++;
        utPltPutLoopVar(psDbHead,"groupname", iNum1,caGroupname);
        utPltPutLoopVarF(psDbHead,"groupid",   iNum1,"%ld",lGroupid);
        utPltPutLoopVar(psDbHead,"plate",    iNum1,caPlate);
        utPltPutLoopVar(psDbHead,"fname",    iNum1,caFname);
        iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,sizeof(long),&lGroupid);
    }
    utPltPutVar(psDbHead, "fname",caFname);
    pasDbCloseCursor(psCur);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}

/* ´ÓÄÚ´æÖÐÉ¾³ýÓÃ»§   */
int ncWebDelUserFromBuf(utShmHead *psShmHead,unsigned long lUserid)
{
    ncUserInfo *psUserInfo;
    ncUserCont *psUser;
    psUserInfo = ncGetUserInfoById(psShmHead,lUserid);
    if(psUserInfo) {
        ncUtlDoLogout(psShmHead,psUserInfo,"Del user from mem");
    }
    psUser = ncUtlGetUserCont(psShmHead,lUserid);
    if(psUser) {
        ncUtlDelUserCont(psShmHead,psUser);
    }
    return 0;
}

/* ¸üÐÂÄÚ´æÖÐµÄÓÃ»§ÐÅÏ¢  */
int ncWebModiUserFromBuf(utShmHead *psShmHead,unsigned long lUserid)
{
    long l;
    int i,iReturn;
    ncUserInfo *psUserInfo;
    ncUserCont *psUser,sUser;
    
    char caName[32],caDispname[32],caMac[32],caIp[16],caPosition[32];
    unsigned long lGroupid,lUseflags,lLimit,lLower,lTimelimit,lFlowLimit,lIp;
    unsigned long lLimitid,lPostFlags,lWebIpFlags,lUse,lValid,lExpire;
    unsigned long lVlanid;
    memset(&sUser,0,sizeof(sUser));
    sUser.userid = lUserid;
    
    iReturn = pasDbOneRecord("select username,password,dispname,compname,position,groupid, \
                       ip,mac,useflags,thistime,thisflow,resettime,fee,usebase,usestatus,expiretime,vlanid,idfyno,idfytype  \
                       from ncuser where userid = :userid",
                         1,"userid",UT_TYPE_LONG,lUserid,
                                   UT_TYPE_STRING,31,sUser.username,
                                   UT_TYPE_STRING,31,sUser.password,
                                   UT_TYPE_STRING,31,sUser.dispname,
                                   UT_TYPE_STRING,31,sUser.compname,
                                   UT_TYPE_STRING,31,sUser.position,
                                   UT_TYPE_LONG,  sizeof(long),&sUser.groupid,
                                   UT_TYPE_ULONG,4,&sUser.ip,
                                   UT_TYPE_STRING,17,caMac,
                                   UT_TYPE_LONG,  sizeof(long),&sUser.useflags,
                                   UT_TYPE_ULONG,  sizeof(long),&sUser.thistime,
                                   UT_TYPE_LONG8, sizeof(long long),&sUser.thisflow,
                                   UT_TYPE_ULONG, 4, &sUser.resettime,
                                   UT_TYPE_FLOAT, 8, &sUser.fee,
                                   UT_TYPE_LONG,4,&lUse,
                                   UT_TYPE_LONG,4,&lValid,
                                   UT_TYPE_ULONG,4,&lExpire,
                                   UT_TYPE_ULONG,4,&lVlanid,
                                   UT_TYPE_STRING,19,sUser.cardid,
                                   UT_TYPE_STRING,3,sUser.idtype);
    pasLogs(PAS_SRCFILE,1000,"ncWebModiUserFromBuf Userid=%lu  Name:[%s] iReturn=%lu",lUserid,sUser.username,iReturn);
    if(iReturn == 0) { 
        sUser.userbase = lUse;
        sUser.cValid = lValid;
        sUser.expiretime = lExpire;
        ncCvtMacI(caMac,sUser.mac);
        sUser.nVlanId=lVlanid;
        psUser = ncUtlGetUserContById(psShmHead,lUserid);
        if(psUser == NULL) {
            sUser.cValid = 1;
            sUser.expiretime = 0;
            ncUtlAddUserCont(psShmHead,&sUser);
        }
        else {
            sUser.cInDb = 1;
            ncUtlModiUserCont(psShmHead,&sUser);
        }
        psUserInfo = ncGetUserInfoById(psShmHead,lUserid);
        if(psUserInfo) {
            if(psUserInfo->cLogin != NC_NETWORK_AUTH) {
                ncUtlDoLogout(psShmHead,psUserInfo,"Modi Buf User");
            }
        }
        
    }
    return 0;
}

/* ¸ù¾Ý GroupidÈ¡ÓÃ»§Ãû  */
char *ncUtlGroupname(unsigned lGroupid)
{
    static char caGroupname[32];
    int iReturn;
    iReturn = pasDbOneRecord("select groupname from ncgroup where groupid = :gid",
                          1,"gid",UT_TYPE_LONG,lGroupid,
                          UT_TYPE_STRING,30,caGroupname);
    if(iReturn == 0) {
        return caGroupname;
    }
    else {
        strcpy(caGroupname,ncLang("0634Î´ÖªÓÃ»§"));
        return caGroupname;
    }
}



/* ÏÔÊ¾Ôö¼Ó/ÐÞ¸ÄÓÃ»§µÄ½çÃæ  */
int ncWebDispUserInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caIp[34],caMac[64],caAddtime[21];
    char caUserid[32];
   	char caUsername[34],caGroupname[34];
   	char caEmail[64],caLasttime[21];
   	int iNum1;
   	long lUse;
   	long lUserid,lGroupid,lGroupid0,lLimit;
    utPltDbHead *psDbHead;
   	int iReturn;
   	int iModFlags;
   	char caSmt[256];
   	char *pBuf,*pSelect;
   	char caGroupid[32],caUse[16];
   	unsigned long lTime;
   	char caDispname[32];
   	char caScroll[16],caOpennum[16];
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"userid",    UT_TYPE_STRING, 15, caUserid);
    iModFlags = 0;
    lUserid = atol(caUserid);
    lTime = time(0);
    if(!utStrIsSpaces(caUserid)) { 
        psDbHead = utPltInitDb();
        lUserid = atol(caUserid);
        memset(caGroupname,0,sizeof(caGroupname));
        lGroupid=0;
        iReturn = pasDbOneRecord("select a.username,a.dispname,b.groupname,a.groupid,a.ip,a.mac,a.addtime,a.useflags,a.ratelimit,a.lasttime,a.email from ncuser a left join ncgroup b on a.groupid = b.groupid where userid=:userid",1,
                            "userid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_STRING,31,caUsername,
                            UT_TYPE_STRING,31,caDispname,
                            UT_TYPE_STRING,32,caGroupname,
                            UT_TYPE_LONG,  sizeof(long), &lGroupid,
                            UT_TYPE_STRING,31,caIp,
                            UT_TYPE_STRING,64,caMac,
                            UT_TYPE_STRING,20,caAddtime,
                            UT_TYPE_LONG,  4, &lUse,
                            UT_TYPE_LONG,  4, &lLimit,
                            UT_TYPE_STRING,20,caLasttime,
                            UT_TYPE_STRING,63,caEmail);
        if(iReturn != 0 && iReturn != PAS_DB_NULLVALUE) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0743ÓÃ»§²»´æÔÚ %d"),iReturn);
            return 0;
        }
        else {
            utPltPutVar(psDbHead,"userid",    caUserid);
            utPltPutVar(psDbHead,"username",  caUsername);
            if(utStrIsSpaces(caDispname)) {
                strcpy(caDispname,caUsername);
            }
            utPltPutVar(psDbHead,"dispname",  caDispname);
            utPltPutVar(psDbHead,"groupname", caGroupname);
            utPltPutVarF(psDbHead,"groupid", "%ld",lGroupid);
            utPltPutVar(psDbHead,"ip", caIp);
            utPltPutVar(psDbHead,"addtime", caAddtime);
            utPltPutVar(psDbHead,"mac", caMac);
            utPltPutVar(psDbHead,"lasttime", caLasttime);
            utPltPutVar(psDbHead,"email", caEmail);
            if(lLimit != 0) {
                utPltPutVarF(psDbHead,"limit", "%ld",lLimit);
            }
        }
        if(lUse == 0) {
            utPltPutVar(psDbHead,"status", ncLang("0850Õý³£"));
        }
        else if(lUse == 1) {
            utPltPutVar(psDbHead,"status", ncLang("0331½ûÓÃ"));
        }
        else {
            utPltPutVar(psDbHead,"status", ncLang("0391Ãâ¼à¿Ø"));
        }
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_userinfo.htm");
    }
    else {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0774ÓÃ»§ÐÅÏ¢²»ÕýÈ· %d"),iReturn);
        return 0;
    }
    return 0;
}


/* ÏÔÊ¾Êý¾Ý¿âÖÐµÄÓÃ»§ÐÅÏ¢£¬Í¬Ê±ÏÔÊ¾ÔÚÏßÇé¿ö
*/
int ncWebDispUserInfo_c(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caIp[34],caMac[64],caAddtime[21];
    char caUserid[32];
   	char caUsername[34],caGroupname[34],caCompname[32];
   	char caEmail[64],caLasttime[21];
   	int iNum1;
   	long lUse;
   	long lUserid,lGroupid,lGroupid0,lLasttime,lAddtime;
    utPltDbHead *psDbHead;
   	int iReturn;
   	int iModFlags;
   	char caSmt[256];
   	char *pBuf,*pSelect;
   	char caGroupid[32],caUse[16];
   	unsigned long lTime;
   	char caDispname[32];
   	char caPlate[128];
   	char caIp0[32];
   	unsigned long lIp;
   	ncUserInfo *psUserInfo;
   	iReturn = utMsgGetSomeNVar(psMsgHead,3,
					"userid",    UT_TYPE_STRING, 15, caUserid,
					"plate",     UT_TYPE_STRING, 127,caPlate,
                    "ip",        UT_TYPE_STRING, 19, caIp0);
    lUserid = atol(caUserid);
    lTime = time(0);
    if(!utStrIsSpaces(caUserid)) { 
        psDbHead = utPltInitDb();
        lUserid = atol(caUserid);
        iReturn = pasDbOneRecord("select a.username,a.dispname,a.compname,b.groupname,a.groupid,a.ip,a.mac,a.addtime,a.useflags,a.lasttime,a.email from ncuser a left join ncgroup b on a.groupid = b.groupid where userid=:userid",1,
                            "userid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_STRING,31,caUsername,
                            UT_TYPE_STRING,31,caDispname,
                            UT_TYPE_STRING,31,caCompname,
                            UT_TYPE_STRING,32,caGroupname,
                            UT_TYPE_LONG,  sizeof(long), &lGroupid,
                            UT_TYPE_ULONG,  4, &lIp,
                            UT_TYPE_STRING,64,caMac,
                            UT_TYPE_ULONG,4,&lAddtime,
                            UT_TYPE_LONG,  4, &lUse,
                            UT_TYPE_ULONG,4,&lLasttime,
                            UT_TYPE_STRING,63,caEmail);
        if(iReturn != 0 && iReturn != PAS_DB_NULLVALUE) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0744ÓÃ»§²»´æÔÚ %d %s"),iReturn,pasDbErrorMsg(NULL));
            return 0;
        }
        utPltPutVar(psDbHead,"userid",    caUserid);
        utPltPutVar(psDbHead,"username",  caUsername);
        if(utStrIsSpaces(caDispname)) {
            strcpy(caDispname,caUsername);
        }
        utPltPutVar(psDbHead,"dispname",  caDispname);
        iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
        if(iReturn == 0 && lGroupid0 > 0) { /* Ö¸¶¨ÓÃ»§  */
            sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu order by groupid",
                        lGroupid0);
        }
        else {
            sprintf(caSmt,"select groupname,groupid from ncgroup order by groupid");
        }
        pBuf = pasUtlCvtSqlSelect(caSmt);
        if(pBuf == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771ÓÃ»§ÐÅÏ¢"),ncLang("0153²¿ÃÅÁÐ±í²»´æÔÚµ"));
            utPltFreeDb(psDbHead);
            return 0;
        }
        sprintf(caGroupid,"%d",lGroupid);
        pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
        if(pSelect == NULL) {
            free(pBuf);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0771ÓÃ»§ÐÅÏ¢"),ncLang("0404ÄÚ´æ³ö´í"));
            utPltFreeDb(psDbHead);
            return 0;
        }
        utPltPutVar(psDbHead,"dept", pSelect);
        free(pSelect);
        utPltPutVar(psDbHead,"ip", utComHostIp(htonl(lIp)));
        utPltPutVar(psDbHead,"addtime", utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime));
        utPltPutVar(psDbHead,"mac", caMac);
        utPltPutVar(psDbHead,"compname", caCompname);
        utPltPutVar(psDbHead,"lasttime", utTimFormat("%Y-%m-%d %H:%M:%S",lLasttime));
        utPltPutVar(psDbHead,"email", caEmail);
        sprintf(caUse,"%d",lUse);
        pSelect = pasUtlCvtInputSelect("use",caUse,ncLang("0852Õý³£,0;½ûÓÃ,1;Ãâ¼à¿Ø,9;"));
        if(pSelect) {
            utPltPutVar(psDbHead,"use", pSelect);
            free(pSelect);
        }
        lIp = ntohl(utComHostAddress(caIp0));
        psUserInfo = ncGetUserInfoByIp(psShmHead,lIp);
        if(psUserInfo) {
            char caMac0[32];
            strcpy(caMac0,ncCvtMac(psUserInfo->mac));
            utPltPutVar(psDbHead,"cip", caIp0);
            utPltPutVar(psDbHead,"cmac", caMac0);
        }
        if(utStrIsSpaces(caPlate)) {
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_userinfo.htm");
        }
        else {
            utPltPutVar(psDbHead,"plate", caPlate);
            utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
        }
        return 0;
    }
    else {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0774ÓÃ»§ÐÅÏ¢²»ÕýÈ· %d"),iReturn);
        return 0;
    }
    return 0;
}

/* ±£´æµ±Ç°ÐÞ¸ÄÐÅÏ¢  */
int ncWebUserSave_c(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
   	char caUsername[33],caGroupid[33],caIp[20],caMac[33],caMacstr[42],caUse[10];
   	char caDispname[33],caLimit[32];
   	long lUserid,lGroupid;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lUse,lUserid0;
   	unsigned long lLasttime;
   	int iPass,iAddFlags;
   	char caPassword[32],caPassword1[32];
   	char caDel[16],caMod[16];
   	char caDate[32],caUserid[16],caUsername0[32],caCompname[32];
    char caEmail[64];
   	iReturn = utMsgGetSomeNVar(psMsgHead,9,
   	                "userid",      UT_TYPE_STRING, 12, caUserid,
					"username",    UT_TYPE_STRING, 31, caUsername,
					"groupid",     UT_TYPE_STRING, 12, caGroupid,
					"ip",          UT_TYPE_STRING, 20, caIp,
					"mac",         UT_TYPE_STRING, 40, caMacstr,
					"compname",    UT_TYPE_STRING, 31, caCompname,
					"email",       UT_TYPE_STRING, 63, caEmail,
					"use",       UT_TYPE_STRING, 9,  caUse,
					"dispname",    UT_TYPE_STRING, 31, caDispname);
    utStrDelSpaces(caUsername);
    utStrDelSpaces(caIp);
    lGroupid = atol(caGroupid);
    if(utStrIsSpaces(caDispname)) {
        strcpy(caDispname,caUsername);
    }
    if(utStrIsSpaces(caUsername) || utStrIsSpaces(caGroupid)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0768ÓÃ»§Ãû»ò×éÃûÎª¿Õ"));
        return 0;
    }
    if(utStrIsSpaces(caUserid)) {
       utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0737ÓÃ»§IDÓÐÎÊÌâ"));
       return 0;
    }
    else {
        lUserid = atol(caUserid);
    }
    if(!utStrIsSpaces(caMacstr) && strcmp(caMacstr,"00:00:00:00:00:00")!=0) {
        utStrToLower(caMacstr);
        iReturn = ncCvtMacI(caMacstr,caMac);
        if(iReturn < 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0076MACµØÖ·¸ñÊ½²»¶Ô£¬ÕýÈ·µÄ¸ñÊ½Ó¦Îª xx:xx:xx:xx:xx:xx"));
            return 0;
        }
/*
        iReturn = pasDbOneRecord("select userid,username from ncuser where mac = :mac and userid <> :uid",
                      2,"mac",UT_TYPE_STRING,caMacstr,
                        "uid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_LONG,4,&lUserid0,
                            UT_TYPE_STRING,31,caUsername0);
        if(iReturn == 0 ) { /* MACµØÖ·ÖØ¸´  */
 /*               utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0078MACµØÖ·ÖØ¸´, ÓÃ»§Ãû %s"),caUsername0);
                return 0;
        } 
*/               
    }
    if(!utStrIsSpaces(caIp) && strcmp(caIp,"0.0.0.0")!=0) {
        if(!utStrIsValidIp(caIp)) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0542ÊäÈëµÄIPµØÖ·²»·ûºÏÒªÇó"));
            return 0;
        }
/*        iReturn = pasDbOneRecord("select userid,username from ncuser where ip = :ip and userid <> :uid",2,
                           "ip",UT_TYPE_ULONG,ntohl(utComHostAddress(caIp)),
                           "uid",UT_TYPE_LONG,lUserid,
                         UT_TYPE_LONG,4,&lUserid0,
                         UT_TYPE_STRING,31,caUsername0);
        if(iReturn == 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0061IPµØÖ·ºÍÓÃ»§%sÖØ¸´"),caUsername0);
            return 0;
        }
*/
    }
    lUse = atol(caUse);
    iReturn = pasDbExecSqlF("update ncuser set username='%s',dispname='%s',compname='%s',groupid=%s,ip=%lu,lasttime=%lu,useflags=%d,mac='%s',email='%s' where userid=%s",
                                 caUsername,caDispname,caCompname,caGroupid,ntohl(utComHostAddress(caIp)),time(0),lUse,caMacstr,caEmail,caUserid);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0699ÐÞ¸Ä³ö´í"));
        pasDbRollback(NULL);
    }
    else {
        iReturn = ncMailCtlAddMailList(lUserid,caEmail);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0839Ôö¼ÓÓÃ»§"),ncLang("0735ÓÃ»§EmailÖØ¸´"));
            pasDbRollback(NULL);
            return 0;
        }
        ncWebModiUserFromBuf(psShmHead,lUserid);
        pasDbCommit(NULL);
    }
    ncWebDispUserInfo_c(psShmHead,iFd,psMsgHead);
 //   ncWebShowUserStatus(psShmHead, iFd, psMsgHead);
    return 0;
}


/* ÏÔÊ¾ÓÃ»§Ê÷´øÓÐÊ±¼ä  */
int ncWebListUserTree_w(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
   	char caUsername[32],caDispname[32],caGroupname[32],caGroup[32];
   	char caScroll[16],caOpennum[16],caFname[32],caTime[32];
   	char caSdate[32],caEdate[32],caSort[32],caRownum[32];
   	int iNum1,iNum2;
   	long lUserid,lGroupid,lGroupid0;
    utPltDbHead *psDbHead;
   	int iReturn,i;

   	iReturn = utMsgGetSomeNVar(psMsgHead,6,
					"plate",    UT_TYPE_STRING, 127, caPlate,
					"sdate",    UT_TYPE_STRING, 31,  caSdate,
					"edate",    UT_TYPE_STRING, 31,  caEdate,
					"sort",     UT_TYPE_STRING, 11, caSort,
					"time",     UT_TYPE_STRING, 31, caTime,
					"recnum",   UT_TYPE_STRING, 11, caRownum);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682ÏÔÊ¾ÓÃ»§"),ncLang("0554Êý¾Ý¿â³öÏÖÎÊÌâ£¬ÇëÓë¹ÜÀíÔ±ÁªÏµ"));
        return 0;
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) { /* Ö¸¶¨ÓÃ»§  */
        psCur = pasDbOpenSqlF("select a.userid,a.username,a.dispname,b.groupname,a.groupid from ncuser a,\
                           ncgroup b where a.groupid = b.groupid  and a.groupid = %lu order by \
                           b.groupid,a.dispname",
                           lGroupid0);
    }
    else {
        psCur = pasDbOpenSql("select a.userid,a.username,a.dispname,b.groupname,a.groupid from ncuser a,\
                           ncgroup b where a.groupid = b.groupid order by b.groupid,a.dispname",0);
    }
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683ÏÔÊ¾ÓÃ»§Ê÷"),ncLang("0554Êý¾Ý¿â³öÏÖÎÊÌâ£¬ÇëÓë¹ÜÀíÔ±ÁªÏµ"));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,sizeof(long),&lUserid,
                                   UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,sizeof(long),&lGroupid);
    i = 0;
    strcpy(caGroup,"\0");
    iNum1 = 0;
    iNum2 = 0;
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caSdate)) utPltPutVar(psDbHead,"sdate",caSdate);
    if(!utStrIsSpaces(caEdate)) utPltPutVar(psDbHead,"edate",caEdate);
    if(!utStrIsSpaces(caTime)) utPltPutVar(psDbHead,"time",caTime);
    if(!utStrIsSpaces(caSort)) utPltPutVar(psDbHead,"sort",caSort);
    if(!utStrIsSpaces(caRownum)) utPltPutVar(psDbHead,"recnum",caRownum);
    while(iReturn == 0) {
        utStrDelSpaces(caGroupname);
        utStrDelSpaces(caUsername);
        if(strcmp(caGroup,caGroupname) != 0) {
            iNum1++;
            utPltPutLoopVar(psDbHead,"groupname", iNum1,caGroupname);
            utPltPutLoopVarF(psDbHead,"groupid",   iNum1,"%ld",lGroupid);
            utPltPutLoopVar(psDbHead,"plate",    iNum1,caPlate);
            strcpy(caGroup,caGroupname);
            utPltPutLoopVar(psDbHead,"sdate",    iNum1,caSdate);
            utPltPutLoopVar(psDbHead,"edate",    iNum1,caEdate);
            utPltPutLoopVar(psDbHead,"time",    iNum1,caTime);
            utPltPutLoopVar(psDbHead,"sort",     iNum1,caSort);
            utPltPutLoopVar(psDbHead,"recnum",   iNum1,caRownum);
            iNum2=1;
        }
        else {
            iNum2++;
        }
        utPltPutLoopVar2(psDbHead, "username", iNum1,iNum2,caUsername);
        utPltPutLoopVar2(psDbHead, "dispname", iNum1,iNum2,caDispname);
        utPltPutLoopVar2(psDbHead, "plate",    iNum1,iNum2,caPlate);
        utPltPutLoopVar2(psDbHead, "sdate",    iNum1,iNum2,caSdate);
        utPltPutLoopVar2(psDbHead, "edate",    iNum1,iNum2,caEdate);
        utPltPutLoopVar2(psDbHead, "time",    iNum1,iNum2,caTime);
        utPltPutLoopVar2(psDbHead, "sort",     iNum1,iNum2,caSort);
        utPltPutLoopVar2(psDbHead, "recnum",     iNum1,iNum2,caRownum);
        utPltPutLoopVarF2(psDbHead,"userid", iNum1,iNum2,"%ld",lUserid);
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,sizeof(long),&lUserid,
                                   UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_STRING,31,caGroupname,
                                   UT_TYPE_LONG,sizeof(long),&lGroupid);
    }
    pasDbCloseCursor(psCur);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}

/* ÏÔÊ¾¸ß¼¶ÓÃ»§²éÑ¯  */
int ncWebDispSuser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    long iReturn,lGroupid,lGroupid0;
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect;
    char caSmt[256];
    char caGroupid[12];
    psDbHead = utPltInitDb();
    strcpy(caGroupid,"\0");
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {  /* ²¿ÃÅÈ¨Àû  */
        sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu order by groupname",
                  lGroupid0);
        pBuf = pasUtlCvtSqlSelectA(caSmt,"\0");
        sprintf(caGroupid,"%lu",lGroupid0);
    }
    else {
        pBuf = pasUtlCvtSqlSelectA("select groupname,groupid from ncgroup where groupid > 0 order by groupname",
               ncLang("0447È«²¿, ;"));
    }
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
        free(pBuf);
        utPltPutVar(psDbHead,"sgroup", pSelect);
        free(pSelect);
    }
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_form_usersear.htm");
}


int ncUtilUserHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char caWhere[256];
	char caSmt[512];
    char caTotPg[16],caCurPg[16];
    char *pSelect;
    unsigned long lUserid,lGroupid,lGroupid0,lCurPg,lTotPg,lTotRec;
    unsigned long lTime,l,lCount,lStartRec,lRowNum,lUseflags;
    int i,iNum;
    utPltDbHead *psDbHead;
   	int iReturn;
   	char caDispname[32],caUsername[32],caGroupname[32],caDate[32];
   	char caGroupid[12],caIp[16],caMac[20],caAtime[6],caLtime[6],caStatus[6];
   	char caEmail[64],caSort[16],caSortby[6],caAunit[6],caAfw[6],caLunit[6],caLfw[6];
   	char caOrder[64];
   	char caCond[32];
   	unsigned long lIp,lAddtime;
 	pasDbCursor *psCur;
	char *pBuf;
#ifdef LDEBUG
	utMsgPrintMsg(psMsgHead);
#endif	
    lStartRec = 0;
	iReturn = utMsgGetSomeNVar(psMsgHead,15,
					"username",   UT_TYPE_STRING, 31, caUsername,
	                "dispname",   UT_TYPE_STRING, 31, caDispname,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "ip",         UT_TYPE_STRING, 15, caIp,
	                "mac",        UT_TYPE_STRING, 19, caMac,
	                "addtime",    UT_TYPE_STRING, 5,  caAtime,
	                "lasttime",   UT_TYPE_STRING, 5,  caLtime,
	                "status",     UT_TYPE_STRING, 5,  caStatus,
	                "email",      UT_TYPE_STRING, 63, caEmail,
	                "sort",       UT_TYPE_STRING, 15, caSort,
	                "aunit",      UT_TYPE_STRING, 2,  caAunit,
	                "afw",        UT_TYPE_STRING, 2,  caAfw,
	                "lunit",      UT_TYPE_STRING, 2,  caLunit,
	                "lfw",        UT_TYPE_STRING, 2,  caLfw,
	                "sortby",    UT_TYPE_STRING, 2,  caSortby);
	strcpy(caWhere,"\0");
    lRowNum = 28;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"where a.userid > 10 and a.groupid = b.groupid");
    if(iReturn == 0 && lGroupid0 > 0) {  /* ²¿ÃÅÈ¨Àû  */
        sprintf(caWhere," and a.groupid = %lu ",lGroupid0);
    }
    if(!utStrIsSpaces(caGroupid)) {
        utStrAddF(caWhere," and a.groupid = %s",caGroupid);
    }
    if(!utStrIsSpaces(caUsername)) {
        utStrAddF(caWhere," and a.username like '%c%s%c'",'%',caUsername,'%');
    }
    if(!utStrIsSpaces(caDispname)) {
        utStrAddF(caWhere," and a.dispname like '%c%s%c'",'%',caDispname,'%');
    }
    if(!utStrIsSpaces(caIp)) {
        utStrAddF(caWhere," and a.ip = %lu", ntohl(utComHostAddress(caIp)));
    }
    if(!utStrIsSpaces(caEmail)) {
        utStrAddF(caWhere," and a.email like '%c%s%c'",'%',caEmail,'%');
    }
	if(!utStrIsSpaces(caStatus)) {
		utStrAddF(caWhere," and a.useflags = %s ",caStatus);
    }
    if(!utStrIsSpaces(caMac)) {
        utStrAddF(caWhere," and a.mac like '%c%s%c'",'%',caMac,'%');
    }
    if(!utStrIsSpaces(caAtime)) {
    	l = atol(caAtime);
    	lTime = time(0);
    	if(caAunit[0] == '3') {
    		lTime = lTime - 3600 * 24 * 30 * l;
    	}
    	else if(caAunit[0] == '2') {
    		lTime = lTime - 3600 * 24 * 7 * l;
    	}
    	else {
    		lTime = lTime - 3600 * 24 * l;
        }
        if(caAfw[0] == '0') {  /* ÒÔÄÚ  */
        	utStrAddF(caWhere," and a.addtime >= %lu",lTime);
        }
        else if(caAfw[0] == '1') { /* ÒÔÍâ  */
        	utStrAddF(caWhere," and a.addtime <= %lu",lTime);
        }
        else {
        	utStrAddF(caWhere," and a.addtime == %lu",lTime);
        }
    }
    

    if(!utStrIsSpaces(caLtime)) {
    	l = atol(caLtime);
    	lTime = time(0);
    	if(caLunit[0] == '3') {
    		lTime = lTime - 3600 * 24 * 30 * l;
    	}
    	else if(caLunit[0] == '2') {
    		lTime = lTime - 3600 * 24 * 7 * l;
    	}
    	else {
    		lTime = lTime - 3600 * 24 * l;
        }
        if(caLfw[0] == '0') {  /* ÒÔÄÚ  */
        	utStrAddF(caWhere," and a.lasttime >= %lu",lTime);
        }
        else if(caLfw[0] == '1') { /* ÒÔÍâ  */
        	utStrAddF(caWhere," and a.lasttime <=  %lu",lTime);
        }
        else {
        	utStrAddF(caWhere," and a.lasttime == %lu ",lTime);
        }
    }
    strcpy(caOrder,"\0");
    if(!utStrIsSpaces(caSort)) {
       	utStrAddF(caOrder," order by %s ",caSort);
        if(caSortby[0] == '1') {     
             utStrAddF(caOrder," desc ");
        }
    }
    sprintf(caSmt,"select count(*) from ncuser a,ncgroup b  %s",caWhere); 
    iReturn = pasDbOneRecord(caSmt,0,
                            UT_TYPE_LONG,4,&lCount);
#ifdef LDEBUG
    printf(" iReturn == %d lCount==%d\n",iReturn,lCount);
    printf(" caSmt==[%s]\n",caSmt);
    printf(" caOrder==[%s] StartRec==%lu,lRowNum==%lu\n",caOrder,lStartRec,lRowNum);
    sprintf(caSmt,"select a.useflags useflags,a.userid userid,a.dispname dispname,a.username username,b.groupname groupname,a.ip ip,a.mac mac,a.lasttime adate,a.email email \
                   from ncuser a,ncgroup b %s %s limit %lu,%lu",caWhere,caOrder,lStartRec,lRowNum);
    printf(" caSmt==[%s]\n",caSmt);
    
#endif   
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746ÓÃ»§²éÑ¯"),ncLang("0172²éÑ¯Êý¾Ý³ö´í %s"),pasDbErrorMsg(NULL));
        return 0;
    }   

                        
    psCur = pasDbOpenSqlF("select a.useflags useflags,a.userid userid,a.dispname dispname,a.username username,b.groupname groupname,a.ip ip,a.mac mac,a.lasttime adate,a.email email \
                   from ncuser a,ncgroup b %s %s limit %lu,%lu",caWhere,caOrder,lStartRec,lRowNum);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746ÓÃ»§²éÑ¯"),ncLang("0172²éÑ¯Êý¾Ý³ö´í %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
    psDbHead = utPltInitDb();
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUseflags,
  	                               UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,31,caDispname,
  	                               UT_TYPE_STRING,31,caUsername,
  	                               UT_TYPE_STRING,30,caGroupname,
  	                               UT_TYPE_ULONG,4,&lIp,
  	                               UT_TYPE_STRING,31,caMac,
  	                               UT_TYPE_ULONG,4,&lAddtime,
  	                               UT_TYPE_STRING,63,caEmail);
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
        iNum++;
        if(lUseflags == 0) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0928È«²¿¼à¿Ø"));
        }
        else if(lUseflags == 1) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0331½ûÓÃ"));
        }
        else if(lUseflags == 3) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0929Ö»¼à¿ØIP·ÃÎÊ"));
        }
        else if(lUseflags == 4) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0930¼à¿ØIPºÍHttp"));
        }
        else {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0391Ãâ¼à¿Ø"));
        }
        utPltPutLoopVarF(psDbHead,"useflags", iNum,"%d",lUseflags);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,caUsername);
        utPltPutLoopVar(psDbHead,"dispname", iNum,caDispname);
        utPltPutLoopVar(psDbHead,"groupname", iNum,caGroupname);
        utPltPutLoopVar(psDbHead,"ip", iNum,utComHostIp(htonl(lIp)));
        utPltPutLoopVar(psDbHead,"mac", iNum,caMac);
        utPltPutLoopVar(psDbHead,"adate", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime));
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"email", iNum,caEmail);
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUseflags,
  	                               UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,31,caDispname,
  	                               UT_TYPE_STRING,31,caUsername,
  	                               UT_TYPE_STRING,30
  	                               ,caGroupname,
  	                               UT_TYPE_ULONG,4,&lIp,
  	                               UT_TYPE_STRING,31,caMac,
  	                               UT_TYPE_ULONG,4,&lAddtime,
  	                               UT_TYPE_STRING,63,caEmail);
    }

    if(lCount > 0) {
        lTotPg = (lCount - 1) / lRowNum + 1;
    }
    else {
        lTotPg = 0;
    }
    lTotRec = lCount;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"sort", caSort);
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVar(psDbHead,"sortby",caSortby);
    utPltPutVar(psDbHead,"where",caWhere);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", 1);
    if(lGroupid0 > 0) {
        sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu order by groupname",
                  lGroupid0);
        pBuf = pasUtlCvtSqlSelectA(caSmt,"\0");
    }
    else {
        pBuf = pasUtlCvtSqlSelectA("select groupname,groupid from ncgroup order by groupname",
               "\0");
    }
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("nbm","0",pBuf);
        free(pBuf);
        utPltPutVar(psDbHead,"sgroup", pSelect);
        free(pSelect);
    }
	
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_list_user.htm");
    return 0;
   
}
    

/* ²âÊÔÓÃ   */
int ncTest2005(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    unsigned long lVersion;
#ifdef LDEBUG
utMsgPrintMsg(psMsgHead);    
#endif
    utMsgGetSomeNVar(psMsgHead,1,
                    "version",  UT_TYPE_LONG,  4,&lVersion);
    pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* ÃÜÔ¿£¬ÔÝÊ±²»ÓÃ  */
                    0,
                    "status",    UT_TYPE_LONG,0);
    return 0;
}

/* ²»ÔÊÐíÐÞ¸ÄÃÜÂë  */
int ncWebModSomeUserState(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int i,iReturn;
	char caTemp[16];
	char caTemp1[256];
	char *p;
	char iDel;
	int group_flag,lGroupid0;
  group_flag=0;
  
	iDel = 0;
	  iReturn=dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,4,&lGroupid0);
    if((iReturn==0)&&(lGroupid0>0)){
    	group_flag=1;
    }
	
    for(i=0;i<30;i++) {
        sprintf(caTemp,"ch%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caTemp);
        if(p) {
        	  
          	iReturn = pasDbExecSqlF("update ncuser set localflag=1 where userid=%s",p);
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0489É¾³ýÓÃ»§"),ncLang("0731ÓÃ»§ %s É¾³ý³ö´í"),p);
                pasDbRollback(NULL);
                return 0;
            }
            else {
            	iDel++;
            }
        }
    }
    if(iDel > 0) {
        pasDbCommit(NULL);
    }
 //   if(group_flag==1)
 //   	return ncWebUserSearch_new(psShmHead, iFd,psMsgHead);
//    else
    	return ncWebUserSearch(psShmHead, iFd,psMsgHead);
}



/* ÔÊÐí»ò½ûÖ¹ÉÏÍø  */
int ncWebModSomeUserFobflag(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int i,iReturn;
	char caTemp[16];
	char caTemp1[256];
	char caForbflag[16];
	char *p;
	char iDel;
	int group_flag,lGroupid0;
  group_flag=0;
  
	iDel = 0;
	memset(caForbflag,0,sizeof(caForbflag));
	iReturn = utMsgGetSomeNVar(psMsgHead,1,"forbflag",   UT_TYPE_STRING, 2, caForbflag);
	
    for(i=0;i<100;i++) {
        sprintf(caTemp,"ch%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caTemp);
        if(p) {
        	  if(atol(caForbflag)==0)
          		iReturn = pasDbExecSqlF("update ncuser set useflags=%d,expiretime=0 where userid=%s",atol(caForbflag),p);
          	else
          	  iReturn = pasDbExecSqlF("update ncuser set useflags=%d where userid=%s",atol(caForbflag),p);
          	
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0489É¾³ýÓÃ»§"),ncLang("0731ÓÃ»§ %s É¾³ý³ö´í"),p);
                pasDbRollback(NULL);
                return 0;
            }
            else {
            	ncWebModiUserFromBuf(psShmHead,atol(p));
            	iDel++;
            }
        }
    }
    if(iDel > 0) {
        pasDbCommit(NULL);
    }
 //   if(group_flag==1)
 //   	return ncWebUserSearch_new(psShmHead, iFd,psMsgHead);
//    else
    	return ncWebUserSearch(psShmHead, iFd,psMsgHead);
}


/* ÏÔÊ¾ÓÃ»§ÐÅÏ¢*/
int ncWebDispUserInfo2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caIp[34],caMac[64],caAddtime[21];
    char caUserid[32];
   	char caUsername[34],caGroupname[34];
   	char caEmail[64],caLasttime[21];
   	char caPassword[32];
   	char caCond[32],caGroupid[16];
   	int iNum1;
   	long lUse;
   	long lUserid,lGroupid,lGroupid0,lLimit,lLimitid,lPost,lWebip,polid;
    utPltDbHead *psDbHead;
   	int iReturn;
   	int iModFlags;
   	char caSmt[256];
   	char *pBuf,*pSelect;
   	char  caUse[16];
   	unsigned long lTime;
   	unsigned long lAddtime,lLasttime,lIp;
   	char caDispname[32];
   	char caTemp[32];
   	char caLimitid[16],caWebip[16],caPost[16];
   	char caCompname[32];
   	long lLocalflag;
   	
   	long qospolid,mailctlid,lTid,iNum;
   	char caTname[32];
   	
   	char caPort[36],caPointphyaddr[16],caIdfytype[4],caIdfyno[20],caPointphyname[20],caIdfyunit[32],caCountry[4];
    psDbHead = utPltInitDb();
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,"userid",  UT_TYPE_STRING, 15, caUserid);
				
    lPost = 0;
    lWebip = 0;
    lLimitid = 0;
    lUserid = 0;

    if(!utStrIsSpaces(caUserid)) { 
        iModFlags = 1;
        lLocalflag=0;
        lUserid = atol(caUserid);
        iReturn = pasDbOneRecord("select a.username,a.dispname,a.compname,b.groupname,a.groupid,a.ip,a.mac,a.addtime,a.useflags,a.lasttime,a.email,a.position,idfytype,idfyno,pointphyname,idfyunit,country,localflag from ncuser a left join ncgroup b on a.groupid = b.groupid where userid=:userid",1,
                            "userid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_STRING,31,caUsername,
                            UT_TYPE_STRING,31,caDispname,
                            UT_TYPE_STRING,31,caCompname,
                            UT_TYPE_STRING,31,caGroupname,
                            UT_TYPE_LONG,  sizeof(long), &lGroupid,
                            UT_TYPE_ULONG, 4,&lIp,
                            UT_TYPE_STRING,64,caMac,
                            UT_TYPE_ULONG, 4,&lAddtime,
                            UT_TYPE_LONG,  4, &lUse,
                            UT_TYPE_STRING,4,&lLasttime,
                            UT_TYPE_STRING,63,caEmail,
                            UT_TYPE_STRING,32,caPort,
                            UT_TYPE_STRING,3,caIdfytype,
                            UT_TYPE_STRING,18,caIdfyno,
                            UT_TYPE_STRING,16,caPointphyname,
                            UT_TYPE_STRING,30,caIdfyunit,
                            UT_TYPE_STRING,3,caCountry,
                            UT_TYPE_LONG,4,&lLocalflag);
        if(iReturn != 0 && iReturn != PAS_DB_NULLVALUE) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0705ÐÞ¸ÄÓÃ»§"),ncLang("0743ÓÃ»§²»´æÔÚ %d"),iReturn);
            return 0;
        }
        else {
            utPltPutVar(psDbHead,"userid",    caUserid);
            utPltPutVar(psDbHead,"username",  caUsername);
            if(utStrIsSpaces(caDispname)) {
                strcpy(caDispname,caUsername);
            }
            utPltPutVar(psDbHead,"dispname",  caDispname);
            utPltPutVar(psDbHead,"compname",  caCompname);
            utPltPutVar(psDbHead,"groupname", caGroupname);
            utPltPutVarF(psDbHead,"groupid", "%ld",lGroupid);
            if(lIp != 0) {
                utPltPutVar(psDbHead,"ip", utComHostIp(htonl(lIp)));
            }
            utPltPutVar(psDbHead,"addtime", utTimFormat("%Y-%m-%d %H:%M:%S",lAddtime));
            utPltPutVar(psDbHead,"mac", caMac);
            utPltPutVar(psDbHead,"lasttime",utTimFormat("%Y-%m-%d %H:%M:%S",lLasttime));
            utPltPutVar(psDbHead,"email", caEmail);
            utPltPutVar(psDbHead,"password", "********");
            
            utPltPutVar(psDbHead,"idfyno",caIdfyno);
            utPltPutVar(psDbHead,"idfyunit",caIdfyunit);
            utPltPutVar(psDbHead,"country",caCountry);
            utPltPutVar(psDbHead,"port",caPort);
        
            utPltPutVar(psDbHead,"pointphyname",caPointphyname);
            utPltPutVar(psDbHead,"idfytype",caIdfytype);
            if(lLocalflag==1)
            utPltPutVar(psDbHead,"localflag","checked");
          sprintf(caUse,"%d",lUse);
    pSelect = pasUtlCvtInputSelect("use",caUse,ncLang("0852È«²¿¼à¿Ø,0;½ûÓÃ,1;Ö»¼à¿ØIP·ÃÎÊ,3;¼à¿ØIPºÍHttp,4;Ãâ¼à¿Ø,9;"));
    if(pSelect) {
        utPltPutVar(psDbHead,"use", pSelect);
        free(pSelect);
    }      
        }
    }
  
  
 
    
    if(lUserid > 0) {
    	      polid = 0;
            qospolid=0;
            mailctlid=0;
        iReturn = pasDbOneRecord("select polid,qospolid,mailctlid from nclimsumm where gtype = 1 and id = :userid",1,
                            "userid",UT_TYPE_LONG,lUserid,
                            UT_TYPE_ULONG,4,&polid,
                            UT_TYPE_ULONG,4,&qospolid,
                            UT_TYPE_ULONG,4,&mailctlid);
        if(iReturn != 0) {
            polid = 0;
            qospolid=0;
            mailctlid=0;
        }
    }
    pBuf = pasUtlCvtSqlSelectA("select name,id from ncprocy",ncLang("0652ÎÞ,0;"));
    if(pBuf) {
        sprintf(caTemp,"%lu",polid);
        utPltPutVar(psDbHead,"polid2",caTemp);
        pSelect = pasUtlCvtInputSelect("polid",caTemp,pBuf);
        if(pSelect) {
            utPltPutVar(psDbHead,"polid", pSelect);
            free(pSelect);
        }
        free(pBuf);
    }
    
    utPltPutVar(psDbHead,"netcode",utComGetVar_sd(psShmHead,"NcName"," "));
    utPltPutVar(psDbHead,"areacode",utComGetVar_sd(psShmHead,"areacode"," "));
    
    
    
    
     //È¡¿ØÖÆ²ßÂÔÐÅÏ¢ £¬½«Æä·ÅÔÚÑ¡Ôñ¿òÖÐ  
      psCur=pasDbOpenSqlF("select id,name from ncprocy order by name ");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213¶¨ÒåÓÊ¼þ·ÖÀà"),ncLang("0174³ö´í%s"),pasDbErrorMsg(NULL));
            return 0;
        }
     
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
      				   UT_TYPE_STRING,30,caTname);
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
	     
      	utPltPutLoopVarF(psDbHead,"id_p",iNum,"%lu",lTid);
      	utPltPutLoopVar(psDbHead,"name_p",iNum,caTname);
      	 iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
  				      UT_TYPE_STRING,30,caTname);
  				}
     
       pasDbCloseCursor(psCur); 
  
     //È¡´ø¿í²ßÂÔ £¬½«Æä·ÅÔÚÑ¡Ôñ¿òÖÐ  
      psCur=pasDbOpenSqlF("select id,name from nctcstrategy order by dispname ");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213¶¨ÒåÓÊ¼þ·ÖÀà"),ncLang("0174³ö´í%s"),pasDbErrorMsg(NULL));
            return 0;
        }
     
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
      				   UT_TYPE_STRING,30,caTname);
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
	     
      	utPltPutLoopVarF(psDbHead,"id_t",iNum,"%lu",lTid);
      	utPltPutLoopVar(psDbHead,"name_t",iNum,caTname);
      	 iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
  				      UT_TYPE_STRING,30,caTname);
  				}
     
       pasDbCloseCursor(psCur); 
    
      //È¡ÓÊ¼þÀ¹½Ø²ßÂÔ £¬½«Æä·ÅÔÚÑ¡Ôñ¿òÖÐ  
      psCur=pasDbOpenSqlF("select id,name from ncmailstrategy order by dispname ");
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213¶¨ÒåÓÊ¼þ·ÖÀà"),ncLang("0174³ö´í%s"),pasDbErrorMsg(NULL));
            return 0;
        }
     
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
      				   UT_TYPE_STRING,30,caTname);
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
	     
      	utPltPutLoopVarF(psDbHead,"id_m",iNum,"%lu",lTid);
      	utPltPutLoopVar(psDbHead,"name_m",iNum,caTname);
      	 iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lTid,
  				      UT_TYPE_STRING,30,caTname);
  				}
     
       pasDbCloseCursor(psCur); 


       utPltPutVarF(psDbHead,"qospolid","%d",qospolid);
       utPltPutVarF(psDbHead,"mailctlid","%d",mailctlid);
       utPltPutVarF(psDbHead,"userstate","%d",lUse);
    
    
    
    
    
    
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_userinfo_2.htm");
    
    return 0;
}


//¾ÆµêÇ°Ì¨µÇ¼ÇÏµÍ³
int ncWebUserSearch_hotel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caWhere[512],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caSort[16],caSortby[6];
    char caUsername[32],caGroupname[32],caMac[32],caIp[32],caDate[32];
    char caGroupid[16];
    char caDispname[36];
    char caSql[2024];
    long lLocalflag,lUsestatus;   
    char caEmail[128];
    char caPosition[36],caIdfyno[20];
    char idfytype[4],idfyunit[64],idfytype_desc[32];
    struct tempProcy_s *pProcy;
    char *pSelect,*pBuf;
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lCount,lIp,lAddtime,lExpiretime;
    int i,iNum;
    char *pSear;
    char caSmt[2512],caOrder[64];
    utPltDbHead *psDbHead;
   	int iReturn,lAddtime1;
   	long lGroupid0,lRowNum,lUseflags,lStartRec,lUseflags0,lTotRec;
   	char caLevel[16];
   	char caTemp[4024];
   	char caOpt[2048];
   	long lGid;
   	long nGid[1000];
   	char caGroupbuf[2048];
   	long iNum1;       
    long lGid2;
   	long group_flag;
   	long id,lSum,index;
   	char p_name[32],t_name[20],m_name[20];
   	long lPid;
 	pasDbCursor *psCur;
#ifdef LDEBUG
    printf(" Pid == %d \n",getpid());
#endif    
// 	utMsgPrintMsg(psMsgHead);
 	
 	
 	
 	
	iReturn = utMsgGetSomeNVar(psMsgHead,8,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					        "CurPg",      UT_TYPE_STRING, 12, caCurPg,
					        "groupid",    UT_TYPE_STRING, 12, caGroupid,
   	                "cond",       UT_TYPE_STRING, 63, caCond,
   	                "sort",       UT_TYPE_STRING, 15, caSort,
   	                "sortby",     UT_TYPE_STRING, 2,  caSortby,
   	                "where",      UT_TYPE_STRING, 510,caWhere,
   	                "level",      UT_TYPE_STRING, 10,caLevel );


    sprintf(caCond0,"'%c%s%c'",'%',caCond,'%');
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    if(pSear = utMsgGetVar_s(psMsgHead,"search")) {
       lCurPg = 1;
       strcpy(caWhere,"\0");
    }
    lGroupid0 = 0;
    if(utStrIsSpaces(caWhere)) {
    	iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
      group_flag=0;
    	if(iReturn == 0 && lGroupid0 > 0) {
    		group_flag=1;
    		sprintf(caTemp,"select groupid from ncgroup where groupid=%d or pid=%d ",lGroupid0,lGroupid0);
        psCur=pasDbOpenSql(caTemp,0);
        if(psCur == NULL) {
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746ÓÃ»§²éÑ¯"),ncLang("0171²éÑ¯Êý¾Ý³ö´í"));
        	return 0;
    		}
    		lGid=0;
    		iNum=0;
    		memset(caOpt,0,sizeof(caOpt));
   	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    while((iReturn == 0||iReturn==1405))
   	    {
   	    	if(iNum==0){
   	    		sprintf(caOpt,"%d",lGid);
   	    	}
   	    	else{
   	    		sprintf(caOpt+strlen(caOpt),",%d",lGid);
   	    	}
   	    	iNum++;
   	       
   	    	lGid=0;
   	    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    	

   	    }
   	     pasDbCloseCursor(psCur);
   	     if(strlen(caOpt)==0){
   	    		sprintf(caOpt,"%d",lGroupid0);
   	    	}
        	if(!utStrIsSpaces(caCond)) {
            	if(strcmp(caCond,ncLang("0928È«²¿¼à¿Ø")) == 0 ||strcmp(caCond,ncLang("0391Ãâ¼à¿Ø")) == 0 ||strcmp(caCond,ncLang("0331½ûÓÃ")) == 0  ) {
                	if(strcmp(caCond,ncLang("0928È«²¿¼à¿Ø"))==0) {
                    	lUseflags0 = 0;
                	}
                	else if(strcmp(caCond,ncLang("0331½ûÓÃ"))==0) {
                    	lUseflags0 = 1;
                	}
                	else {
                    	lUseflags0 = 9;
                	}
                	sprintf(caWhere,"where  a.groupid in (%s)  and (a.useflags = %d)",
                   	caOpt,lUseflags0);
                
            	}
            	else {
                    if(utStrIsValidIp(caCond)) {
                        lIp = ntohl(utComHostAddress(caCond));
                 	    sprintf(caWhere,"where  a.groupid in (%s)  and (a.username like %s or a.dispname like %s or b.groupname like %s or a.mac like %s or a.email like %s or a.ip = %lu or a.position='%s' or a.idfyno='%s')",
                   		    caOpt,caCond0,caCond0,caCond0,caCond0,caCond0,lIp,caCond,caCond);
                    }
                    else {
                  	    sprintf(caWhere,"where  a.groupid in (%s)  and (a.username like %s or a.dispname like %s or b.groupname like %s or a.mac like %s or a.email like %s or a.position='%s' or a.idfyno='%s')",
                   		     caOpt,caCond0,caCond0,caCond0,caCond0,caCond0,caCond,caCond);
                    }
            	}
        	}
        	else {
            	sprintf(caWhere,"where  a.groupid in(%s)",caOpt);
        	}
    	}
    	else {
    	    lGroupid0 = 0;
        	if(!utStrIsSpaces(caCond)) {   	                
            	if(strcmp(caCond,ncLang("0850Õý³£")) == 0 ||strcmp(caCond,ncLang("0391Ãâ¼à¿Ø")) == 0 ||strcmp(caCond,ncLang("0331½ûÓÃ")) == 0  ) {
                	if(strcmp(caCond,ncLang("0850Õý³£"))==0) {
                    	lUseflags0 = 0;
                	}
                	else if(strcmp(caCond,ncLang("0331½ûÓÃ"))==0) {
                    	lUseflags0 = 1;
                	}
                	else {
                    	lUseflags0 = 9;
                	}
 //               	sprintf(caWhere,"where a.userid > 10    and a.useflags = %d",
                	
                	sprintf(caWhere,"where  a.useflags = %d",
                   	lUseflags0);
            	}
            	else {
                    if(utStrIsValidIp(caCond)) {
                        lIp = ntohl(utComHostAddress(caCond));
                        sprintf(caWhere,"where  (a.username like %s or a.dispname like %s or b.groupname like %s or a.ip = %lu or a.mac like %s or a.email like %s or a.position='%s' or a.idfyno='%s')",
                   	        caCond0,caCond0,caCond0,lIp,caCond0,caCond0,caCond,caCond);
                        
 //               	    sprintf(caWhere,"where a.userid > 10  and (a.username like %s or a.dispname like %s or b.groupname like %s or a.ip = %lu or a.mac like %s or a.email like %s or a.position='%s' or a.idfyno='%s')",
 //                  	        caCond0,caCond0,caCond0,lIp,caCond0,caCond0,caCond,caCond);
                    }
                    else {
                    sprintf(caWhere,"where  (binary a.username like %s or binary a.dispname like %s or binary b.groupname like %s or a.mac like %s or a.email like %s or a.position='%s' or a.idfyno='%s')",
                           	caCond0,caCond0,caCond0,caCond0,caCond0,caCond,caCond);
 //                   	sprintf(caWhere,"where a.userid > 10  and (binary a.username like %s or binary a.dispname like %s or binary b.groupname like %s or a.mac like %s or a.email like %s or a.position='%s' or a.idfyno='%s')",
 //                          	caCond0,caCond0,caCond0,caCond0,caCond0,caCond,caCond);
                    }
            	}

        	}
        	else {
  //          	strcpy(caWhere,"where a.userid > 10 ");
  	strcpy(caWhere,"where 1=1 ");
        	}
    	}
    	if(!utStrIsSpaces(caGroupid)) {
    //		if(atol(caLevel)==0){
    			sprintf(caWhere+strlen(caWhere)," and a.groupid in (%s)",caGroupbuf);
    			
    //		}
    //		else
   //     	utStrAddF(caWhere," and a.groupid = %s ",caGroupid);
    	}
    }
    
    strcpy(caOrder,"\0");
   	if(!utStrIsSpaces(caSort)) {
       	utStrAddF(caOrder," order by %s ",caSort);
        if(caSortby[0] == '1') {     
             utStrAddF(caOrder," desc ");
        }
   	}

if(strstr(caOrder,"order")==NULL){
	strcpy(caOrder," order by a.userid desc ");
}
    
    lRowNum = 28;
    
    lRowNum=atol(utComGetVar_sd(psShmHead,"RowNum","28"));
    
    lStartRec = (lCurPg - 1) * lRowNum;    
    sprintf(caSmt,"select count(*) from ncuser a left join ncgroup b on a.groupid=b.groupid %s",caWhere); 
//printf("caSmt=%s\n",caSmt);
    iReturn = pasDbOneRecord(caSmt,0,
                            UT_TYPE_LONG,4,&lCount);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746ÓÃ»§²éÑ¯"),ncLang("0171²éÑ¯Êý¾Ý³ö´í"));
        return 0;
    } 
                          
    psCur = pasDbOpenSqlF("select a.useflags useflags,a.userid userid,a.dispname dispname,a.username username,b.groupname groupname,a.ip ip,a.mac mac,a.lasttime adate,a.email email,a.localflag,a.position,a.idfyno,a.addtime,a.idfytype,a.idfyunit,a.expiretime,a.usestatus,b.pid \
                   from ncuser a left join ncgroup b on (a.groupid=b.groupid) %s %s limit %lu,%lu",caWhere,caOrder,lStartRec,lRowNum);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746ÓÃ»§²éÑ¯"),ncLang("0171²éÑ¯Êý¾Ý³ö´í"));
        return 0;
    }        
    psDbHead = utPltInitDb();
    
    lLocalflag=0;
    memset(caGroupname,0,sizeof(caGroupname));
    memset(caPosition,0,sizeof(caPosition));
    memset(caIdfyno,0,sizeof(caIdfyno));
    memset(idfytype,0,sizeof(idfytype));
    memset(idfyunit,0,sizeof(idfyunit));
    lExpiretime=0;
    lPid=0;
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUseflags,
  	                               UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,30,caDispname,
  	                               UT_TYPE_STRING,31,caUsername,
  	                               UT_TYPE_STRING,30,caGroupname,
  	                               UT_TYPE_ULONG,4,&lIp,
  	                               UT_TYPE_STRING,31,caMac,
  	                               UT_TYPE_ULONG,4,&lAddtime,
  	                               UT_TYPE_STRING,63,caEmail,
  	                               UT_TYPE_LONG,4,&lLocalflag,
  	                               UT_TYPE_STRING,32,caPosition,
  	                               UT_TYPE_STRING,18,caIdfyno,
  	                               UT_TYPE_LONG,4,&lAddtime1,
  	                               UT_TYPE_STRING,3,idfytype,
  	                               UT_TYPE_STRING,30,idfyunit,
  	                               UT_TYPE_LONG,4,&lExpiretime,
  	                               UT_TYPE_LONG,4,&lUsestatus,
  	                               UT_TYPE_LONG,4,&lPid);
 
   	iNum = 0;
   	while((iReturn == 0||iReturn==1405) && iNum < lRowNum) {
        iNum++;
        if(lUseflags == 0) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0928È«²¿¼à¿Ø"));
        }
        else if(lUseflags == 1) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0331½ûÓÃ"));
        }
        else if(lUseflags == 3) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0929Ö»¼à¿ØIP·ÃÎÊ"));
        }
        else if(lUseflags == 4) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0930¼à¿ØIPºÍHttp"));
        }
        else if(lUseflags == 10) {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0627Î´ÉèÖÃ"));
        }
        else {
            utPltPutLoopVar(psDbHead,"status", iNum,ncLang("0391Ãâ¼à¿Ø"));
        }

        if(lUseflags == 0) {
            utPltPutLoopVar(psDbHead,"status2", iNum,ncLang("1864ÔÊÐíÉÏÍø"));
        }
        else if(lUseflags == 1) {
            utPltPutLoopVar(psDbHead,"status2", iNum,ncLang("0331½ûÓÃ"));
        }



        utPltPutLoopVarF(psDbHead,"useflags", iNum,"%d",lUseflags);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,caUsername);
        utPltPutLoopVar(psDbHead,"dispname", iNum,caDispname);
      	if(lPid>0){
      		utPltPutLoopVarF(psDbHead,"groupname",iNum,"%s/%s",ncUtlGetGroupNameByid(psShmHead,lPid,"Î´Öª"),caGroupname);
      	}
      	else
      	utPltPutLoopVar(psDbHead,"groupname",iNum,caGroupname);
      	
        utPltPutLoopVar(psDbHead,"ip", iNum,utComHostIp(htonl(lIp)));
        utPltPutLoopVar(psDbHead,"mac", iNum,caMac);
        if(lAddtime>0)
        utPltPutLoopVar(psDbHead,"adate", iNum,utTimFormat("%Y-%m-%d %H:%M",lAddtime));
        
        if(lAddtime1>0)
        utPltPutLoopVar(psDbHead,"adddate", iNum,utTimFormat("%Y-%m-%d %H:%M",lAddtime1));
        
        if(lExpiretime>0)
        utPltPutLoopVar(psDbHead,"expiretime", iNum,utTimFormat("%Y-%m-%d %H:%M",lExpiretime));
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"position",iNum,caPosition);
        utPltPutLoopVar(psDbHead,"idfyno",iNum,caIdfyno);
        if(lLocalflag==1)
        	strcpy(caEmail,ncLang("0144²»ÔÊÐí"));
        else
        	strcpy(caEmail,ncLang("0827ÔÊÐí"));
        
        utPltPutLoopVar(psDbHead,"email", iNum,caEmail);
        utPltPutLoopVar(psDbHead,"idfyunit",iNum,idfyunit);
        
        if(strcmp(idfytype,"111")==0)
        	strcpy(idfytype_desc,ncLang("1865Éí·ÝÖ¤"));
        else if(strcmp(idfytype,"133")==0)
          strcpy(idfytype_desc,ncLang("1866Ñ§ÉúÖ¤"));
        else if(strcmp(idfytype,"123")==0)
          strcpy(idfytype_desc,ncLang("1867¾¯¹ÙÖ¤"));
        else if(strcmp(idfytype,"233")==0)
        	strcpy(idfytype_desc,ncLang("1868Ê¿±øÖ¤"));
        else if(strcmp(idfytype,"113")==0)
        	strcpy(idfytype_desc,ncLang("1869»§¿Ú±¡"));	
        else if(strcmp(idfytype,"414")==0)
          strcpy(idfytype_desc,ncLang("1870»¤ÕÕ"));
        else 
        	strcpy(idfytype_desc,ncLang("0431ÆäËü"));
        
        if(lUsestatus==0){
        	char aaa[150]="";
        	strcat(aaa,"<font color=red>");
        	strcat(aaa,ncLang("1871Ê§Ð§"));
        	strcat(aaa,"</font>");
        	utPltPutLoopVar(psDbHead,"ac_desc",iNum,aaa);
        	memset(aaa,0,sizeof(aaa));
           //  utPltPutLoopVar(psDbHead,"ac_desc",iNum,"<font color=red>Ê§Ð§</font>");
       }
         if(lUsestatus==1){
         char aaa[150]="";
        	strcat(aaa,"<font color=green>");
        	strcat(aaa,ncLang("0850Õý³£"));
        	strcat(aaa,"</font>");
        	utPltPutLoopVar(psDbHead,"ac_desc",iNum,aaa);
        	memset(aaa,0,sizeof(aaa));
        //utPltPutLoopVar(psDbHead,"ac_desc",iNum,"<font color=green>Õý³£</font>");
   
      }
        utPltPutLoopVar(psDbHead,"idfytype_desc",iNum,idfytype_desc);
        memset(caGroupname,0,sizeof(caGroupname));
        memset(caPosition,0,sizeof(caPosition));
        memset(caIdfyno,0,sizeof(caIdfyno));
    lLocalflag=0; 
    lExpiretime=0;
    memset(idfytype,0,sizeof(idfytype));
    memset(idfyunit,0,sizeof(idfyunit));
    lPid=0;    
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUseflags,
  	                               UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,30,caDispname,
  	                               UT_TYPE_STRING,31,caUsername,
  	                               UT_TYPE_STRING,30,caGroupname,
  	                               UT_TYPE_ULONG,4,&lIp,
  	                               UT_TYPE_STRING,31,caMac,
  	                               UT_TYPE_ULONG,4,&lAddtime,
  	                               UT_TYPE_STRING,63,caEmail,
  	                               UT_TYPE_LONG,4,&lLocalflag,
  	                               UT_TYPE_STRING,32,caPosition,
  	                               UT_TYPE_STRING,18,caIdfyno,
  	                               UT_TYPE_LONG,4,&lAddtime1,
  	                               UT_TYPE_STRING,3,idfytype,
  	                               UT_TYPE_STRING,30,idfyunit,
  	                               UT_TYPE_LONG,4,&lExpiretime,
  	                               UT_TYPE_LONG,4,&lUsestatus,
  	                               UT_TYPE_LONG,4,&lPid);
    }
    pasDbCloseCursor(psCur);
    if(lCount <= 1) {
        lTotPg = 1;
    }
    else {
        lTotPg = (lCount - 1) / lRowNum + 1;
    }
    lTotRec = lCount;
    i = 0;
    while(iNum < lRowNum + 1) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"sort", caSort);
    utPltPutVar(psDbHead,"sortby", caSortby);
    utPltPutVar(psDbHead,"cond",caCond);
    utPltPutVar(psDbHead,"where",caWhere);

    utPltPutVar(psDbHead,"groupid0",caGroupid);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
   
   if(strcasecmp(utComGetVar_sd(psShmHead,"idcardtype",""),"hs") == 0){
   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"vpn/hotel_hs_userlist.htm");
  	}
  	else if(strcasecmp(utComGetVar_sd(psShmHead,"idcardtype",""),"hs200z") == 0){
   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"vpn/hotel_hs200z_userlist.htm");
  	}
  	else{
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"vpn/hotel_userlist.htm");
  	}
    return 0;
}


//µÇ¼ÇÓÃ»§
int ncHotelUser_add(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
  char caDispname[32],caPostion[32],caIdtype[12],caIdfyno[20],caTime[12];
	long g_iUserCount = 0;
  int iReturn;
  char caBuf[128];
	char strVarName[32] = {0};
	char *pVarValue = NULL;
	int i = 0;
	int iLen;
	long lUserid2;
	long  lUserid;
	long lVlanid;
	char caIdfyno_t[20];
	char caSql[1024];
	char caPassword1[32],caPass[32],caPassword[32];
	char caAddr[36],caAddr_temp[36];
	char caIdfytype[16],caSex[12],caIdfyunit[36];
	   	iReturn = utMsgGetSomeNVar(psMsgHead,8,
   	      "dispname",      UT_TYPE_STRING, 30, caDispname,
					"idfytype",    UT_TYPE_STRING, 3, caIdfytype,
					"sex",        UT_TYPE_STRING, 2, caSex,
					"position",     UT_TYPE_STRING, 30, caPostion,
					"password",    UT_TYPE_STRING, 16, caPassword,
					"password1",   UT_TYPE_STRING, 16, caPassword1,
					"idfyno",      UT_TYPE_STRING,18,caIdfyno,
					"idfyunit",        UT_TYPE_STRING, 30, caIdfyunit);
				
				utStrDelSpaces(caDispname);
				utStrDelSpaces(caIdfytype);
				utStrDelSpaces(caSex);
				utStrDelSpaces(caPostion);
				utStrDelSpaces(caPassword);
				utStrDelSpaces(caIdfyno);
				utStrDelSpaces(caIdfyunit);
				
				
			   iReturn = pasDbOneRecord("select userid,idfyno from ncuser where username = :username ",
                         1,"username",UT_TYPE_STRING,caPostion,                  
                            UT_TYPE_LONG,4,&lUserid2,
                            UT_TYPE_STRING,19,caIdfyno_t);
			   if(lUserid2>0){
			   	  if(strcmp(caIdfyno,caIdfyno_t)!=0){
			   	  
			   	      pasDbExecSqlF("update ncuser set username='%s_%d',usestatus=0,useflags=1,vlanid=0 where userid=%d",caPostion,time(0),lUserid2);
			   	    }
			   	    else{
			   	    		lVlanid=0;
								  if(strcasecmp(utComGetVar_sd(psShmHead,"GetVlanId","Yes"),"YES") == 0){
								  	
								  	sprintf(caSql,"select vlanid from ncvlanidposition where position='%s' ",caPostion);
	
								  	pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lVlanid);
								  } 
								
			   	    	 pasDbExecSqlF("update ncuser set usestatus=1,useflags=0,vlanid=%d where userid=%d",lVlanid,lUserid2);
			   	    	 ncWebModiUserFromBuf(psShmHead,lUserid2); 
			   	    }
			   }
			   if(strcmp(caIdfyno,caIdfyno_t)!=0)
			   {
			     if(strlen(caPassword)==0){
				   iLen=strlen(caIdfyno);
				   if(iLen>6){
				   	  strcpy(caPass,&caIdfyno[iLen-6]);
			  
				   }
				   else{
				   	  strcpy(caPass,caIdfyno);
				   }
			    }
			    else{
			    	strcpy(caPass,caPassword);
			    }
				  sprintf(caPassword1,"%s%s",caPostion,caPass);
				  utMd5Ascii22(caPassword1,strlen(caPassword1),NULL,caPassword);	
				  lUserid =  pasGetSid("ncuser","userid");
				  
				  if(strlen(caIdfyunit)>0){
				  	strcpy(caAddr,utStrGetCharHan(caIdfyunit,30));
				  }
				  else{
				  	strcpy(caAddr,"");
				  }
				   lVlanid=0;
				  if(strcasecmp(utComGetVar_sd(psShmHead,"GetVlanId","Yes"),"YES") == 0){
				  	
				  	sprintf(caSql,"select vlanid from ncvlanidposition where position='%s' ",caPostion);
				  	pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lVlanid);
				  } 
		
				 printf("insert into ncuser (userid,groupid,username,dispname,addtime,lasttime,password,position,idfytype,idfyno,usestatus,idfyunit,vlanid) values \
	                (%lu,0,'%s','%s',%lu,%lu,'%s','%s','%s','%s',1,'%s',%lu)\n",
	                 lUserid,caPostion,caDispname,time(0),time(0),caPassword,caPostion,caIdfytype,caIdfyno,caAddr,lVlanid); 
	                 
           
				  pasDbExecSqlF("insert into ncuser (userid,groupid,username,dispname,addtime,lasttime,password,position,idfytype,idfyno,usestatus,idfyunit,vlanid,sex) values \
	                (%lu,0,'%s','%s',%lu,%lu,'%s','%s','%s','%s',1,'%s',%lu,'%s')",
	                 lUserid,caPostion,caDispname,time(0),time(0),caPassword,caPostion,caIdfytype,caIdfyno,caAddr,lVlanid,caSex);
	
				  ncWebModiUserFromBuf(psShmHead,lUserid); 
			  }
			  
//			ncUtlPutHotel2Buf(psShmHead,pVarValue);
	
	

      pasDbCommit(NULL);
      
       ncWebUserSearch_hotel(psShmHead,iFd,psMsgHead);
      
	return 0;
}


/* É¾³ýÑ¡¶¨µÄÂÃ¹ÝÓÃ»§¼ÇÂ¼  */
int ncWebDelSomeUser_hotel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int i,iReturn;
	char caTemp[16];
	char caTemp1[256];
	char *p;
	char iDel;
	char caUsername2[32];
	long lGroupid0,groupid_flag;
  memset(caUsername2,0,sizeof(caUsername2));
  lGroupid0=0;
  groupid_flag=0;
 iReturn=dsCltGetSessionValue(2,"dispname",UT_TYPE_STRING,30,caUsername2,
                        "groupid", UT_TYPE_LONG,4,&lGroupid0);
 if((iReturn==0)&&(lGroupid0>0)){
 	groupid_flag=1;
}
	iDel = 0;
    utMsgPrintMsg(psMsgHead);
    for(i=0;i<60;i++) {
        sprintf(caTemp,"ch%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caTemp);
        if(p) {
        	   sprintf(caTemp1,"Delete user:[delete  from ncuser where userid=%s ]",p);
             ncsWriteSysLog(psMsgHead,caUsername2,"05","success",caTemp1);
          	iReturn = pasDbExecSqlF("delete from ncusermail where userid=%s",p);
          	
          	            	pasDbExecSqlF("delete from ncmobuser where username in (select username from ncuser where userid=%s)",p);
          	
            iReturn = pasDbExecSqlF("delete  from ncuser where userid=%s ",p);
            

         
            
            
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0489É¾³ýÓÃ»§"),ncLang("0731ÓÃ»§ %s É¾³ý³ö´í"),p);
                pasDbRollback(NULL);
                return 0;
            }
            else {
                unsigned long lUserid;
                lUserid = atol(p);
                ncWebDelUserFromBuf(psShmHead,lUserid);
            	iDel++;
            }
        }
    }
    if(iDel > 0) {
        pasDbCommit(NULL);
    }

    	return ncWebUserSearch_hotel(psShmHead, iFd,psMsgHead);
}


/* ÔÊÐí»ò½ûÖ¹ÉÏÍø  */
int ncUserFobflag_hotel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	int i,iReturn;
	char caTemp[16];
	char caTemp1[256];
	char caForbflag[16];
	char *p;
	char iDel;
	int group_flag,lGroupid0;
  group_flag=0;
  
	iDel = 0;
	memset(caForbflag,0,sizeof(caForbflag));
	iReturn = utMsgGetSomeNVar(psMsgHead,1,"forbflag",   UT_TYPE_STRING, 2, caForbflag);
	
    for(i=0;i<100;i++) {
        sprintf(caTemp,"ch%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caTemp);
        if(p) {
        	  if(atol(caForbflag)==0)
          		iReturn = pasDbExecSqlF("update ncuser set useflags=%d,expiretime=0 where userid=%s",atol(caForbflag),p);
          	else
          	  iReturn = pasDbExecSqlF("update ncuser set useflags=%d where userid=%s",atol(caForbflag),p);
          	
            if(iReturn != 0) {
                utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0489É¾³ýÓÃ»§"),ncLang("0731ÓÃ»§ %s É¾³ý³ö´í"),p);
                pasDbRollback(NULL);
                return 0;
            }
            else {
            	ncWebModiUserFromBuf(psShmHead,atol(p));
            	iDel++;
            }
        }
    }
    if(iDel > 0) {
        pasDbCommit(NULL);
    }
 //   if(group_flag==1)
 //   	return ncWebUserSearch_new(psShmHead, iFd,psMsgHead);
//    else
    	return ncWebUserSearch_hotel(psShmHead, iFd,psMsgHead);
}

//ÏÔÊ¾ÐÞ¸ÄÓÃ»§½çÃæ
int ncDispAddHotelUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;    
    char *p;
    char caSid[16];
    int i=0;
    long lUseflags;
    char caTemp[1024];
  long lCount;
  char caDispname[32],caSex[8],caIdfytype[8],caIdfyno[20],caIdfyunit[32],caPosition[32],caUserid[12];
  char caGname[36];
	iReturn = utMsgGetSomeNVar(psMsgHead,1,"userid",      UT_TYPE_STRING, 10,  caUserid);
					                
	psDbHead = utPltInitDb();


if(strlen(caUserid)>0){
	sprintf(caTemp,"select dispname,sex,idfytype,idfyno,idfyunit,position,useflags from ncuser where userid=%s ",caUserid);
	printf("caTemp=%s\n",caTemp);
	memset(caDispname,0,sizeof(caDispname));
  memset(caSex,0,sizeof(caSex));
  memset(caIdfytype,0,sizeof(caIdfytype));
  memset(caIdfyno,0,sizeof(caIdfyno));
  memset(caPosition,0,sizeof(caPosition));
  lUseflags=0;
	iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caDispname,
	                                UT_TYPE_STRING,1,caSex,
	                                UT_TYPE_STRING,3,caIdfytype,
	                                UT_TYPE_STRING,18,caIdfyno,
	                                UT_TYPE_STRING,30,caIdfyunit,
	                                UT_TYPE_STRING,30,caPosition,
	                                UT_TYPE_LONG,4,&lUseflags);
	utPltPutVar(psDbHead,"dispname",caDispname);
	utPltPutVar(psDbHead,"sex",caSex);
	utPltPutVar(psDbHead,"idfytype",caIdfytype);
	utPltPutVar(psDbHead,"idfyno",caIdfyno);
	utPltPutVar(psDbHead,"idfyunit",caIdfyunit);
	utPltPutVar(psDbHead,"position",caPosition);
  utPltPutVar(psDbHead,"userid",caUserid);
	utPltPutVarF(psDbHead,"useflags","%d",lUseflags);                        
}



	
     
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"vpn/hotel_edituser.htm");
    
    return 0;
}



//ÐÞ¸ÄÓÃ»§
int ncHotelUser_Mody(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
  char caDispname[32],caPostion[32],caIdtype[12],caIdfyno[20],caTime[12];
	long g_iUserCount = 0;
  int iReturn;
  char caBuf[128];
	char strVarName[32] = {0};
	char *pVarValue = NULL;
	int i = 0;
	int iLen;
	long lUserid2;
	long  lUserid;
	long lVlanid;
	char caIdfyno_t[20];
	char caSql[1024];
	char caPassword1[32],caPass[32],caPassword[32];
	char caAddr[36],caAddr_temp[36];
	char caIdfytype[16],caSex[12],caIdfyunit[36];
	char caUserid[12],caUserflags[8],caPosition[32];
	 iReturn = utMsgGetSomeNVar(psMsgHead,10,
   	      "dispname",      UT_TYPE_STRING, 30, caDispname,
					"idfytype",    UT_TYPE_STRING, 3, caIdfytype,
					"sex",        UT_TYPE_STRING, 2, caSex,
					"position",     UT_TYPE_STRING, 30, caPostion,
					"password",    UT_TYPE_STRING, 16, caPassword,
					"password1",   UT_TYPE_STRING, 16, caPassword1,
					"idfyno",      UT_TYPE_STRING,18,caIdfyno,
					"idfyunit",        UT_TYPE_STRING, 30, caIdfyunit,
					"userid",      UT_TYPE_STRING,10,caUserid,
					"useflags",    UT_TYPE_STRING,2,caUserflags);
				
				utStrDelSpaces(caDispname);
				utStrDelSpaces(caIdfytype);
				utStrDelSpaces(caSex);
				utStrDelSpaces(caPostion);
				utStrDelSpaces(caPassword);
				utStrDelSpaces(caIdfyno);
				utStrDelSpaces(caIdfyunit);
				
				

			   if(strlen(caPassword)>0){
				   sprintf(caPassword1,"%s%s",caPostion,caPassword);
				   utMd5Ascii22(caPassword1,strlen(caPassword1),NULL,caPassword);	
				   pasDbExecSqlF("update ncuser set username='%s',password='%s',dispname='%s',position='%s',idfytype='%s',idfyno='%s',idfyunit='%s',useflags=%s,sex='%s' where userid=%s",caPostion,caPassword,caDispname,caPostion,caIdfytype,caIdfyno,caIdfyunit,caUserflags,caSex,caUserid);
				   
				   
			   }
			   else{
			  
			   		pasDbExecSqlF("update ncuser set username='%s',dispname='%s',position='%s',idfytype='%s',idfyno='%s',idfyunit='%s',useflags=%s,sex='%s' where userid=%s",caPostion,caDispname,caPostion,caIdfytype,caIdfyno,caIdfyunit,caUserflags,caSex,caUserid);
			   }
			  

			  ncWebModiUserFromBuf(psShmHead,atol(caUserid));
	
	
      pasDbCommit(NULL);
      
       ncDispAddHotelUser(psShmHead,iFd,psMsgHead);
      
	return 0;
}