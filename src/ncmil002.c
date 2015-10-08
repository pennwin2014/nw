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
#include "mail.h"
extern int iDebugLevel;
/* 显示规则  */
int ncMailShowRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    int iReturn,iNum;
    
    char caUserid[16],caFlags[16];
    char caRdesc[1024],caSmail[64],caTzmail[64];
    char caName[32],caRname[66],caFrom[66],caTo[66],caSub[66],caCont[66],caAttName[66];
    unsigned long lMsize,lAttSize,lRid,lActflags,lControl,lTzflags,lNum;
 	if(iDebugLevel == NC_DEBUG_MSGINFO) {
 	    utSysPrintMsg(psMsgHead,"ncMailShowRule");
 	}
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
					"id",    UT_TYPE_STRING, 12, caUserid,
					"flags", UT_TYPE_STRING, 12, caFlags);
    if(caFlags[0] == '2') {  /* 组  */
        psCur = pasDbOpenSqlF("select a.groupname,b.num,b.rid,b.rname,b.mailfrom,\
                           b.mailto,b.mailsub,b.mailcont,b.mailsize,b.attname,b.attsize,\
                           b.actflags,b.control,b.smail,b.tzflags,b.tzmail \
                           from ncgroup a, ncmailfilter b \
                           where a.groupid = b.userid and b.userid = %s and b.flags = %s order by b.num ",
                           caUserid,caFlags);
    }
    else if(caFlags[0] == '1') {  /* 用户  */
        psCur = pasDbOpenSqlF("select a.username,b.num,b.rid,b.rname,b.mailfrom,\
                           b.mailto,b.mailsub,b.mailcont,b.mailsize,b.attname,b.attsize, \
                           b.actflags,b.control,b.smail,b.tzflags,b.tzmail \
                           from ncuser a, ncmailfilter b  \
                           where a.userid = b.userid and b.userid = %s and b.flags = %s order by b.num",
                           caUserid,caFlags);
    }  
    else {
        psCur = pasDbOpenSqlF("select 'All',num,rid,rname,mailfrom,\
                           mailto,mailsub,mailcont,mailsize,attname,attsize, \
                           actflags,control,smail,tzflags,tzmail \
                           from ncmailfilter  \
                           where flags = %s order by num",caFlags);
    }    
    if(psCur==NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0801邮件控制"),ncLang("0554数据库出现问题，请与管理员联系"));
        return 0;
    }
    psDbHead = utPltInitDb();
    strcpy(caTzmail,"\0");
    strcpy(caSmail,"\0");
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName,
                                   UT_TYPE_LONG,   4,&lNum,
                                   UT_TYPE_LONG,   4,&lRid,
                                   UT_TYPE_STRING,63,caRname,
                                   UT_TYPE_STRING,63,caFrom,
                                   UT_TYPE_STRING,63,caTo,
                                   UT_TYPE_STRING,63,caSub,
                                   UT_TYPE_STRING,63,caCont,
                                   UT_TYPE_LONG,  4,&lMsize,
                                   UT_TYPE_STRING,63,caAttName,
                                   UT_TYPE_LONG,  4,&lAttSize,
                                   UT_TYPE_LONG,  4,&lActflags,
                                   UT_TYPE_LONG,  4,&lControl,
                                   UT_TYPE_STRING,63,caSmail,
                                   UT_TYPE_LONG,  4,&lTzflags,
                                   UT_TYPE_STRING,63,caTzmail);
    iNum = 0;
    while(iReturn == 0 || iReturn == 1405) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"rnum", iNum,"%ld",lNum);
        utPltPutLoopVarF(psDbHead,"num", iNum,"%ld",iNum);
        utPltPutLoopVar(psDbHead,"name", iNum,caName);
        utPltPutLoopVar(psDbHead,"id", iNum,caUserid);
        utPltPutLoopVar(psDbHead,"flags", iNum,caFlags);
        utPltPutLoopVarF(psDbHead,"rid", iNum,"%ld",lRid);
        utPltPutLoopVar(psDbHead,"rname",iNum,caRname);
        utPltPutLoopVar(psDbHead,"from", iNum,caFrom);
        utPltPutLoopVar(psDbHead,"to",   iNum,caTo);
        utPltPutLoopVar(psDbHead,"sub",  iNum,caSub);
        utPltPutLoopVar(psDbHead,"cont", iNum,caCont);
        utPltPutLoopVar(psDbHead,"attname",iNum,caAttName);
        utPltPutLoopVarF(psDbHead,"msize", iNum,"%ld",lMsize);
        utPltPutLoopVarF(psDbHead,"asize", iNum,"%ld",lAttSize);
        strcpy(caRdesc,"\0");
        if(!utStrIsSpaces(caFrom)) {
        	utStrAddF(caRdesc,ncLang("0219发件人包括:%s;"),caFrom);
        }
        if(!utStrIsSpaces(caTo)) {
        	utStrAddF(caRdesc,ncLang("0537收件人包括:%s;"),caTo);
        }
        if(!utStrIsSpaces(caSub)) {
        	utStrAddF(caRdesc,ncLang("0126标题包括:%s;"),caSub);
        }
        if(!utStrIsSpaces(caCont)) {
        	utStrAddF(caRdesc,ncLang("0410内容包括:%s;"),caCont);
        }
        if(lMsize > 0) {
        	utStrAddF(caRdesc,ncLang("0791邮件大于 %lu 字节;"),lMsize);
        }
        if(lAttSize > 0) {
        	utStrAddF(caRdesc,ncLang("0257附件大于 %lu 字节;"),lAttSize);
        }
        if(lActflags == 1) {
        	utStrAddF(caRdesc,ncLang("0114 保留备份;"));
        }
        else {
        	utStrAddF(caRdesc,ncLang("0136 不备份;"));
        }  
        if(lControl == 0) {
        	utStrAddF(caRdesc,ncLang("0853 正常发送;"));
        }
        else if(lControl==1) {
        	utStrAddF(caRdesc,ncLang("0341 拒绝发送;"));
        }
        else if(lControl==2) {
        	utStrAddF(caRdesc,ncLang("0876 转发到:%s;"),caSmail);
        }
        else if(lControl==3) {
        	utStrAddF(caRdesc,ncLang("0338 经过管理人员审核;"));
        }
        
        if(lTzflags == 1) {
        	utStrAddF(caRdesc,ncLang("0578 通知发件人;"));
        }        
        else if(lTzflags == 2) {
        	utStrAddF(caRdesc,ncLang("0579 通知收件人;"));
        }
        else if(lTzflags == 3){
        	utStrAddF(caRdesc,ncLang("0574 通知%s;"),caTzmail);
        }
        utPltPutLoopVar(psDbHead,"rdesc", iNum,caRdesc);
        
        iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caName,
                                   UT_TYPE_LONG,   4,&lNum,
                                   UT_TYPE_LONG,   4,&lRid,
                                   UT_TYPE_STRING,63,caRname,
                                   UT_TYPE_STRING,63,caFrom,
                                   UT_TYPE_STRING,63,caTo,
                                   UT_TYPE_STRING,63,caSub,
                                   UT_TYPE_STRING,63,caCont,
                                   UT_TYPE_LONG,  4,&lMsize,
                                   UT_TYPE_STRING,63,caAttName,
                                   UT_TYPE_LONG,  4,&lAttSize,
                                   UT_TYPE_LONG,  4,&lActflags,
                                   UT_TYPE_LONG,  4,&lControl,
                                   UT_TYPE_STRING,63,caSmail,
                                   UT_TYPE_LONG,  4,&lTzflags,
                                   UT_TYPE_STRING,63,caTzmail);
    }
    utPltPutVar(psDbHead, "name",caName);
    utPltPutVar(psDbHead, "id",  caUserid);
    utPltPutVar(psDbHead, "flags",caFlags);
    utPltPutVarF(psDbHead, "sum","%lu",iNum);
    pasDbCloseCursor(psCur);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mailctl_list.htm");
    return 0;
}

/* 显示一条记录  */
int ncMailDispOneRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caRid[16];
    utPltDbHead *psDbHead;
    int iReturn,iNum;
    char caUserid[16],caFlags[16];
    char caName[32],caRname[66],caFrom[66],caTo[66],caSub[66],caCont[66],caAttName[66];
    unsigned long lMsize,lAttSize,lRid,lActflags,lControl,lTzflags,lNum;
    char caSmail[64],caTzmail[64],caTzcontent[256];
//  utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
   	                "name",  UT_TYPE_STRING,31,caName,
					"rid",    UT_TYPE_STRING, 12, caRid,
					"id",    UT_TYPE_STRING, 12,  caUserid,
					"flags", UT_TYPE_STRING, 12,  caFlags);
    lRid = atol(caRid);
	if(lRid != 0) {
        iReturn = pasDbOneRecord("select rname,num,mailfrom,\
                           mailto,mailsub,mailcont,mailsize,attname,attsize, \
                           actflags,control,smail,tzflags,tzmail,tzcontent \
                           from ncmailfilter  \
                           where rid = :rid",1,
                           "rid",UT_TYPE_LONG,lRid,
                            UT_TYPE_STRING,63,caRname,
                            UT_TYPE_LONG, 4, &lNum,
                            UT_TYPE_STRING,63,caFrom,
                            UT_TYPE_STRING,63,caTo,
                            UT_TYPE_STRING,63,caSub,
                            UT_TYPE_STRING,63,caCont,
                            UT_TYPE_LONG,  4,&lMsize,
                            UT_TYPE_STRING,63,caAttName,
                            UT_TYPE_LONG,  4,&lAttSize,
                            UT_TYPE_LONG,  4,&lActflags,
                            UT_TYPE_LONG,  4,&lControl,
                            UT_TYPE_STRING,63,caSmail,
                            UT_TYPE_LONG,  4,&lTzflags,
                            UT_TYPE_STRING,63,caTzmail,
                            UT_TYPE_STRING,255,caTzcontent);
        if(iReturn == 0) {
            psDbHead = utPltInitDb();
            utPltPutVarF(psDbHead,"rid", "%ld",lRid);
            utPltPutVarF(psDbHead,"rnum", "%ld",lNum);
            utPltPutVar(psDbHead,"rname",caRname);
            utPltPutVar(psDbHead,"from", caFrom);
            utPltPutVar(psDbHead,"to",   caTo);
            utPltPutVar(psDbHead,"sub",  caSub);
            utPltPutVar(psDbHead,"cont", caCont);
            utPltPutVar(psDbHead,"attname",caAttName);
            utPltPutVarF(psDbHead,"msize", "%ld",lMsize);
            utPltPutVarF(psDbHead,"asize", "%ld",lAttSize);
            utPltPutVarF(psDbHead,"actflags", "%ld",lActflags);
            utPltPutVarF(psDbHead,"control", "%ld",lControl);
            utPltPutVarF(psDbHead,"tzflags", "%ld",lTzflags);
            utPltPutVar(psDbHead,"smail",caSmail);
            utPltPutVar(psDbHead,"tzmail",caTzmail);
            utPltPutVar(psDbHead,"tzcont",caTzcontent);
            utPltPutVar(psDbHead, "name",caName);
            utPltPutVar(psDbHead, "id",  caUserid);
            utPltPutVar(psDbHead, "flags",caFlags);
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mailctl_form.htm");
        }
        else {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0801邮件控制"),ncLang("0554数据库出现问题，请与管理员联系"));
            return 0;
        
        }
    }
    else {
        psDbHead = utPltInitDb();
        utPltPutVarF(psDbHead,"rnum", "%ld",0);
        utPltPutVarF(psDbHead,"actflags", "%ld",1);
        utPltPutVarF(psDbHead,"control", "%ld",0);
        utPltPutVarF(psDbHead,"tzflags", "%ld",0);
        utPltPutVar(psDbHead, "name",caName);
        utPltPutVar(psDbHead, "id",  caUserid);
        utPltPutVar(psDbHead, "flags",caFlags);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mailctl_form.htm");
    }
    return 0;
}

int ncMailModiOneRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    int iReturn,iNum;
    char caBuffer[1024],caAdd[16],caApp[16];
    char caUserid[16],caFlags[16],caRid[16],caMsize[16],caAttSize[16],caRnum[16];
    char caName[32],caRname[66],caFrom[66],caTo[66],caSub[66],caCont[66],caAttName[66];
    char caTcont[256],caTmail[64],caTflags[6],caSmail[64],caSflags[6],caAction[6];
    unsigned long lMsize,lAttSize,lRid;
//  utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,20,
   	                "app",    UT_TYPE_STRING, 12, caApp,
  	                "add",    UT_TYPE_STRING, 12, caAdd,
					"rid",    UT_TYPE_STRING, 12, caRid,
					"id",    UT_TYPE_STRING, 12,  caUserid,
					"flags", UT_TYPE_STRING, 12,  caFlags,
                    "rname", UT_TYPE_STRING, 63,  caRname,
                    "rnum",  UT_TYPE_STRING, 12,  caRnum,
                    "from",  UT_TYPE_STRING, 63,  caFrom,
                    "to",    UT_TYPE_STRING,63,caTo,
                    "sub",   UT_TYPE_STRING,63,caSub,
                    "cont",  UT_TYPE_STRING,63,caCont,
                    "msize", UT_TYPE_STRING,12,caMsize,
                    "attname",UT_TYPE_STRING,63,caAttName,
                    "attsize",UT_TYPE_STRING,12,caAttSize,
                    "actflags",UT_TYPE_STRING,2, caAction,
                    "control",UT_TYPE_STRING,2,caSflags,
                    "smail",UT_TYPE_STRING,63,caSmail,
                    "tzflags",UT_TYPE_STRING,2,caTflags,
                    "tmail",UT_TYPE_STRING,63,caTmail,
                    "tzcont",UT_TYPE_STRING,255,caTcont);
    if(utStrIsSpaces(caUserid) ) {
        strcpy(caUserid,"0");
    }
    sprintf(caMsize,"%d",atol(caMsize));
    sprintf(caAttSize,"%d",atol(caAttSize));
    sprintf(caRnum,"%d",atol(caRnum));
    if(!utStrIsSpaces(caApp)) {
        ncInitMailCtl(psShmHead);
    }
    else {
        if(utStrIsSpaces(caRid) || !utStrIsSpaces(caAdd)) {  /* 增加  */
      		lRid = pasGetSid("ncmailfilter","rid");
            iReturn = pasDbExecSqlF("insert into ncmailfilter (rid,num,rname,userid,flags,mailfrom,mailto,\
                           mailsub,mailcont,mailsize,attname,attsize,actflags,control,smail,\
                           tzflags,tzmail,tzcontent) values (%lu,%s,'%s',%s,%s,'%s',\
                           '%s','%s','%s',%s,'%s',%s,%s,%s,'%s',%s,'%s','%s')",
                           lRid,caRnum,caRname,caUserid,caFlags,caFrom,caTo,caSub,caCont,
                           caMsize,caAttName,caAttSize,caAction,caSflags,caSmail,caTflags,caTmail,
                           caTcont);
            sprintf(caBuffer,"insert into ncmailfilter (rid,rname,userid,flags,mailfrom,mailto,\
                           mailsub,mailcont,mailsize,attname,attsize,actflags,control,smail,\
                           tzflags,tzmail,tzcontent) values (%lu,'%s',%s,%s,'%s',\
                           '%s','%s','%s',%s,'%s',%s,%s,%s,'%s',%s,'%s','%s')",
                           lRid,caRname,caUserid,caFlags,caFrom,caTo,caSub,caCont,
                           caMsize,caAttName,caAttSize,caAction,caSflags,caSmail,caTflags,caTmail,
                           caTcont);                           
        }
        else { /* 修改  */
            lRid = atol(caRid);
            iReturn = pasDbExecSqlF("update ncmailfilter set rname='%s',num=%s,mailfrom='%s',\
                  mailto='%s',mailsub='%s',mailcont='%s',mailsize=%s,attname='%s',\
                  attsize=%s,actflags=%s,control=%s,smail='%s',tzflags=%s,tzmail='%s',\
                  tzcontent='%s' where rid = %lu",
                  caRname,caRnum,caFrom,caTo,caSub,caCont,caMsize,caAttName,caAttSize,caAction,
                  caSflags,caSmail,caTflags,caTmail,caTcont,lRid);
            sprintf(caBuffer,"update ncmailfilter set rname='%s',mailfrom='%s',\
                  mailto='%s',mailsub='%s',mailcont='%s',mailsize=%s,attname='%s',\
                  attsize=%s,actflags=%s,control=%s,smail='%s',tzflags=%s,tzmail='%s',\
                  tzcontent='%s' where rid = %lu",
                  caRname,caFrom,caTo,caSub,caCont,caMsize,caAttName,caAttSize,caAction,
                  caSflags,caSmail,caTflags,caTmail,caTcont,lRid);                          
        }
        if(iReturn == 0) {
            pasDbCommit(NULL);
        }
        else {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0801邮件控制"),ncLang("0545数据插入出错 %d"),iReturn);
            pasDbRollback(NULL);
        }
    }
    ncMailShowRule(psShmHead, iFd,psMsgHead);
    return 0;
}


int ncMailDelSomeRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn,iSum,i;
    unsigned long lRid;
    char caSum[16],caVar[32],*p;
//  utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"sum",   UT_TYPE_STRING, 12, caSum);
    iSum = atol(caSum);
    for(i=0;i<iSum;i++) {
       sprintf(caVar,"chk%d",i+1);
       p = utMsgGetVar_s(psMsgHead,caVar);
       if(p) {
           lRid = atol(p);
           iReturn = pasDbExecSqlF("delete from ncmailfilter where rid=%lu",lRid);
       }
    }
    pasDbCommit(NULL);
    ncMailShowRule(psShmHead, iFd,psMsgHead);
    return 0;
}

/* 察看备份邮件  */
int ncWebMailCtlList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    utPltDbHead *psDbHead;
    
    long lMbflag;
    char caKey[1024],caMname[32]; 
    char cbcode[1024],caSerno[32];
    char caOk[16],caXzmb[16];
    char szBuf[1024];
    long lId;
    
    
    dsCltGetSessionValue(1,"mbflag",UT_TYPE_LONG,sizeof(long),&lMbflag);

    if(lMbflag!=10){
    	  iReturn = utMsgGetSomeNVar(psMsgHead,3,"mname",  UT_TYPE_STRING,12,caMname,
                                         "ok",     UT_TYPE_STRING,10,caOk,
                                         "xzmb",   UT_TYPE_STRING,10,caXzmb);
          if(strlen(caMname)>0){              
          memset(caKey,0,sizeof(caKey)); 
          dsCltGetSessionValue(1,"id",UT_TYPE_LONG,sizeof(long),&lId);
				  sprintf(szBuf,"select mbserno from ncuserbindmb where userid=%d ",lId);

				  pasDbOneRecord(szBuf,0,UT_TYPE_STRING,16,caSerno);
				  sprintf(szBuf,"select mbkey from ncmbserno where mbserno='%s' ",caSerno);
				 
				  pasDbOneRecord(szBuf,0,UT_TYPE_STRING,1000,caKey); 
			  

			  if(strlen(caXzmb)>0){
			   iReturn=ncMBAuth(psShmHead,caKey,caMname,caSerno,2);
			   if(iReturn<1){
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","校正失败");
					utPltPutVar(psDbHead,"fname","ncWebMailCtlList");
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
			   }
			   else{
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","校正成功");
					utPltPutVar(psDbHead,"fname","ncWebMailCtlList");
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
   				}
				}
				else{
				
			   iReturn=ncMBAuth(psShmHead,caKey,caMname,caSerno,1);

			   if(iReturn<1){
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","验证失败");
					utPltPutVar(psDbHead,"fname","ncWebMailCtlList");
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
			   }
			   else{

			   	dsCltSetSessionValue(1,"mbflag",UT_TYPE_LONG,4,10);
			   }
			  }
			    
				
				
			}
			else{
					psDbHead = utPltInitDb();
					   	utPltPutVar(psDbHead,"fname","ncWebMailCtlList");

   	          utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
   	          return 0;
				}                       
    }
    
    
    iReturn = ncUtlMailCtlList(psShmHead,iFd,psMsgHead,"ncmailctl","mail/nc_mailctl_mlist.htm");
    return 0;
}

/* 监控历史记录查询  */
int ncWebMailCtlHlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    iReturn = ncUtlMailCtlList(psShmHead,iFd,psMsgHead,"ncmaillogo","mail/nc_mailctl_hlist.htm");
    return 0;
}


/* 读一封备份的邮件  */
int ncWebMailCtlRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char *p;
    p = utMsgGetVar_s(psMsgHead,"stat");
    if(p) {
        if(p[0] == '1') {
            iReturn = ncUtlMailReadOne(psShmHead,"ctl",iFd,psMsgHead,"mail/nc_mailread_form_ctl1.htm");
        }
        else {
            iReturn = ncUtlMailReadOne(psShmHead,"ctl",iFd,psMsgHead,"mail/nc_mailread_form_ctl2.htm");
        }
    }            
    return 0;
}
    

int ncWebMailCtlRead2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    iReturn = ncUtlMailReadOne(psShmHead,"ctl",iFd,psMsgHead,"mail/nc_mailread_form2.htm");
    return 0;
}

int ncWebMailCtlDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    
    char caFile[128],caDispname[128],caType[128];
    char caTemp[256];
    int i,iReturn;
    char *pFile;
//  utMsgPrintMsg(psMsgHead);    
    iReturn = utMsgGetSomeNVar(psMsgHead,3,
                    "fname",      UT_TYPE_STRING, 127, caFile,
	                "disp",       UT_TYPE_STRING, 127,  caDispname,
	                "type",       UT_TYPE_STRING, 127,  caType);    

    sprintf(caTemp,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","."));
    if(strstr(caFile,caTemp) == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0645文件不正确"));
        return 0;
    }       
    for(i=strlen(caFile)-1;caFile[i]!='/' && i > 0;i--);
    if(i>0) {
        pFile = caFile + i + 1;
        caFile[i] = 0;
    }
    else {
        pFile = caFile;
    }
    if(utStrIsSpaces(caDispname)) {
        strcpy(caDispname,pFile);
    }
    return utPltFileDownload(iFd,caType,caFile,pFile,caDispname);
}


int ncWebMailCtlOutHtm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    
    char caFile[128];
    int i,iReturn;
    char caTemp[256];
    iReturn = utMsgGetSomeNVar(psMsgHead,1,
                    "fname",      UT_TYPE_STRING, 127, caFile);
    sprintf(caTemp,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","."));
    if(strstr(caFile,caTemp) == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0645文件不正确"));
        return 0;
    }       
    utPltHtmlFileOut(iFd,psMsgHead,caFile);
    return 0;
}




/* 删除邮件  */
int ncWebMailCtlDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caFile[64],*p;
    char caTempFile[256],caVar[32];
    int iSum,i,iReturn;
    unsigned long lSid;
//  utMsgPrintMsg(psMsgHead);   	
    for(i=0;i<40;i++) {
        sprintf(caVar,"chk%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            lSid = atol(p);
            iReturn = pasDbOneRecord("select mailfile from ncmailctl where sid = :sid",1,
                           "sid",UT_TYPE_LONG,lSid,
                           UT_TYPE_STRING,63,caFile);
            if(iReturn == 0) {
   	            sprintf(caTempFile,"%s/ctl/%s",utComGetVar_sd(psShmHead,"maildir","/home/ncmysql/nc/maillog"),caFile);
   	            remove(caTempFile);
   	            sprintf(caTempFile,"%s/ctl/%s.to",utComGetVar_sd(psShmHead,"maildir","/home/ncmysql/nc/maillog"),caFile);
   	            remove(caTempFile);
   	            sprintf(caTempFile,"%s/ctl/%s.ses",utComGetVar_sd(psShmHead,"maildir","/home/ncmysql/nc/maillog"),caFile);
   	            remove(caTempFile);
                iReturn = pasDbExecSqlF("delete from ncmailctl where sid = %lu ",lSid);
                iReturn = pasDbExecSqlF("delete from ncmailctli where mid = %lu ",lSid);
                if(iReturn == 0) {
                    pasDbCommit(NULL);
                }
            }
        }
    }
    return ncWebMailCtlList(psShmHead, iFd,psMsgHead);
}




/* 导出邮件  
        将选择的邮件导出到一个html文件中，然后打包
 */
int ncWebMailCtlExport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    char caFname[64];
    unsigned long lAsize,lSize;
    char caFile1[128],caFile2[128],caPath[128],caFile[128];
    char caTemp[128],caMsg[128];
    char caDate[64],caFrom[128],caTo[128],caSub[128];
    char *p,*pMailDir;
    char caVar[32];
    long lUserid;
    int iCount,iReturn,i;
    unsigned long lSid,lSdate;

#ifdef LDEBUG
printf(" Now Exp MailCtl \n");
utMsgPrintMsg(psMsgHead);   	
#endif
    iCount = 0;
    for(i=0;i<40;i++) {
        sprintf(caVar,"chk%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            lSid = atol(p);
            iReturn = pasDbOneRecord("select userid,mailfile,sdate,mailfrom,mailto,mailsub,mailasize,mailsize  from ncmailctl where sid = :sid",1,
                           "sid",UT_TYPE_LONG,lSid,
                            UT_TYPE_LONG, 4, &lUserid,
                            UT_TYPE_STRING,63,caFname,
                            UT_TYPE_ULONG,4, &lSdate,
                            UT_TYPE_STRING,127,caFrom,
                            UT_TYPE_STRING,127,caTo,
                            UT_TYPE_STRING,127,caSub,
                            UT_TYPE_LONG,  4,&lAsize,
                            UT_TYPE_LONG,  4,&lSize);
            if(iReturn == 0) {
                if(iCount == 0) {
                    pMailDir = utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog");
                    sprintf(caFile,"mexp%d",utStrGetId());
                    sprintf(caPath,"%s/tmp/%s",pMailDir,caFile);
                    if(!utFileIsExist(caPath)) {
                        if(mkdir(caPath,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)!=0) {
                            utSysLog(" Mkdir %s Error",caPath);
                            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0367临时目录创建失败"));
                            return (-1);
                        }
                    }
                    psDbHead = utPltInitDb();
                }
                iCount++;       
                utPltPutLoopVarF(psDbHead,"username", iCount,"%s",ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
                utPltPutLoopVarF(psDbHead,"from", iCount,"%s",caFrom);
                utPltPutLoopVarF(psDbHead,"to",   iCount,"%s",caTo);
                utPltPutLoopVarF(psDbHead,"sdate",iCount,"%s",utTimFormat("%Y-%m-%d %H:%M:%S",lSdate));
                utPltPutLoopVarF(psDbHead,"sub",  iCount,"%s",caSub);
                utPltPutLoopVarF(psDbHead,"msize",iCount,"%s",utStrLtoF(lSize,caTemp));
                utPltPutLoopVarF(psDbHead,"asize",iCount,"%s",utStrLtoF(lAsize,caTemp));
                sprintf(caMsg,"%lu%lu.eml", lUserid,utStrGetId());               
                utPltPutLoopVarF(psDbHead,"msg",iCount,"%s",caMsg);
                sprintf(caFile1,"%s/%s",caPath,caMsg);
                sprintf(caFile2,"%s/ctl/%s",pMailDir,caFname);
                utFilFilCopy(caFile2,caFile1);
            }
        }
    }
    if(iCount > 0) {
        /* 生成HTML文件  */
        sprintf(caFile1,"%s/index.htm",caPath);
        utPltFileToHtmlFile("mail/nc_mailexp_list.htm",caFile1,psDbHead);
        sprintf(caFile2,"%s.zip",caPath);
        iReturn = pasUtlZipFile(caFile2,caPath,"*.*");     
        utFilRmDir(caPath);
        if(iReturn == 0) {
            sprintf(caFile1,"%s/tmp/",pMailDir);
            sprintf(caTemp,"%s.zip",caFile);
            utPltFileDownload(iFd,"application/zip",caFile1,caTemp,caTemp);
            remove(caFile2);
            return 0;
        }
        else {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0680下载文件生成出错"));
            return 0;
        }
    }
    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0379没有选择要导出的邮件"));
    return 0;
}


/* 导出监控邮件  */
int ncWebMailMonExport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    char caFname[64];
    unsigned long lAsize,lSize;
    char caFile1[128],caFile2[128],caPath[128],caFile[128];
    char caTemp[128],caMsg[128];
    char caDate[64],caFrom[128],caTo[128],caSub[128];
    char *p,*pMailDir;
    char caVar[32];
    long lUserid;
    int iCount,iReturn,i;
    unsigned long lSid;

#ifdef LDEBUG
printf(" Now Exp MailMon \n");
utMsgPrintMsg(psMsgHead);   	
#endif
    iCount = 0;
    for(i=0;i<40;i++) {
        sprintf(caVar,"chk%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            lSid = atol(p);
            iReturn = pasDbOneRecord("select userid,mailfile,sdate,mailfrom,mailto,mailsub,mailasize,mailsize  from ncmailmon where sid = :sid",1,
                           "sid",UT_TYPE_LONG,lSid,
                            UT_TYPE_LONG, 4, &lUserid,
                            UT_TYPE_STRING,63,caFname,
                            UT_TYPE_STRING,23,caDate,
                            UT_TYPE_STRING,127,caFrom,
                            UT_TYPE_STRING,127,caTo,
                            UT_TYPE_STRING,127,caSub,
                            UT_TYPE_LONG,  4,&lAsize,
                            UT_TYPE_LONG,  4,&lSize);
            if(iReturn == 0) {
                if(iCount == 0) {
                    pMailDir = utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog");
                    sprintf(caFile,"mexp%d",utStrGetId());
                    sprintf(caPath,"%s/tmp/%s",pMailDir,caFile);
                    if(!utFileIsExist(caPath)) {
                        if(mkdir(caPath,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)!=0) {
                            utSysLog(" Mkdir %s Error",caPath);
                            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0367临时目录创建失败"));
                            return (-1);
                        }
                    }
                    psDbHead = utPltInitDb();
                }
                iCount++;       
                utPltPutLoopVarF(psDbHead,"username", iCount,"%s",ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
                utPltPutLoopVarF(psDbHead,"from", iCount,"%s",caFrom);
                utPltPutLoopVarF(psDbHead,"to",   iCount,"%s",caTo);
                utPltPutLoopVarF(psDbHead,"sdate",iCount,"%s",caDate);
                utPltPutLoopVarF(psDbHead,"sub",  iCount,"%s",caSub);
                utPltPutLoopVarF(psDbHead,"msize",iCount,"%s",utStrLtoF(lSize,caTemp));
                utPltPutLoopVarF(psDbHead,"asize",iCount,"%s",utStrLtoF(lAsize,caTemp));
                sprintf(caMsg,"%lu%lu.eml", lUserid,utStrGetId());               
                utPltPutLoopVarF(psDbHead,"msg",iCount,"%s",caMsg);
                sprintf(caFile1,"%s/%s",caPath,caMsg);
                sprintf(caFile2,"%s/mon/%s",pMailDir,caFname);
                utFilFilCopy(caFile2,caFile1);
            }
        }
    }
    if(iCount > 0) {
        /* 生成HTML文件  */
        sprintf(caFile1,"%s/index.htm",caPath);
        utPltFileToHtmlFile("mail/nc_mailexp_list.htm",caFile1,psDbHead);
        sprintf(caFile2,"%s.zip",caPath);
        iReturn = pasUtlZipFile(caFile2,caPath,"*.*");     
        utFilRmDir(caPath);
        if(iReturn == 0) {
            sprintf(caFile1,"%s/tmp/",pMailDir);
            sprintf(caTemp,"%s.zip",caFile);
            utPltFileDownload(iFd,"application/zip",caFile1,caTemp,caTemp);
            remove(caFile2);
            return 0;
        }
        else {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0680下载文件生成出错"));
            return 0;
        }
    }
    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0379没有选择要导出的邮件"));
    return 0;
}



int ncWebMailCtlCvtCode(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caFile[64],*p;
    char caTempFile[256],caVar[32];
    char caFlags[4];
    int iSum,i,iReturn,iCount=0;
    unsigned long lSid,lCode;
#ifdef LDEBUG
utMsgPrintMsg(psMsgHead);   
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,1,
	                "flags",     UT_TYPE_STRING, 1, caFlags);
    p = utMsgGetVar_s(psMsgHead,"ccode");
    if(p) {
        lCode = atol(p);
    }
    else {
        lCode = 0;
    }
	                	
    for(i=0;i<40;i++) {
        sprintf(caVar,"chk%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            lSid = atol(p);
            iCount++;
            iReturn = pasDbExecSqlF("update ncmailctl set ccode = %d where sid=%lu ",
                            lCode,lSid);
        }
    }
    if(iCount>0) {
        pasDbCommit(NULL);
    }
    if(caFlags[0] == '4' || caFlags[0]=='5') {
        return ncWebMailCtlAuditlst(psShmHead, iFd,psMsgHead);
    }
    else {
        return ncWebMailCtlList(psShmHead, iFd,psMsgHead);
    }
}




int ncWebMailMonCvtCode(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char *p;
    char caVar[32];
    int i,iReturn,iCount=0;
    unsigned long lSid,lCode;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);   	
#endif    
    p = utMsgGetVar_s(psMsgHead,"ccode");
    if(p) {
        lCode = atol(p);
    }
    else {
        lCode = 0;
    }

    for(i=0;i<40;i++) {
        sprintf(caVar,"chk%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            lSid = atol(p);
            printf("update ncmailmon set ccode=%d where sid=%lu\n ",
                            lCode,lSid);           
            iReturn = pasDbExecSqlF("update ncmailmon set ccode=%d where sid=%lu ",
                            lCode,lSid);
            printf("iReturn=%d\n",iReturn);
            iCount++;
        }
    }
    if(iCount>0) {
        pasDbCommit(NULL);
    }
    return ncWebMailMonList(psShmHead, iFd,psMsgHead);
}



/* 删除历史邮件  */
int ncWebMailCtlHistoryDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caFile[64],*p;
    char caTempFile[256],caVar[32];
    int iSum,i,iReturn;
    unsigned long lSid;
//  utMsgPrintMsg(psMsgHead);   	
    for(i=0;i<30;i++) {
        sprintf(caVar,"chk%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            lSid = atol(p);
            iReturn = pasDbOneRecord("select mailfile from ncmaillogo where sid = :sid",1,
                           "sid",UT_TYPE_LONG,lSid,
                           UT_TYPE_STRING,63,caFile);
            if(iReturn == 0) {
   	            sprintf(caTempFile,"%s/ctl/%s",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"),caFile);
   	            remove(caTempFile);
   	            sprintf(caTempFile,"%s/ctl/%s.to",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"),caFile);
   	            remove(caTempFile);
                iReturn = pasDbExecSqlF("delete from ncmaillogo where sid = %lu ",lSid);
                if(iReturn == 0) {
                    pasDbCommit(NULL);
                }
            }
        }
    }
    return ncWebMailCtlHlist(psShmHead, iFd,psMsgHead);
}



/* 邮件审计 
    2003/1/26 Modify By Liyunming
 */
int ncWebMailCtlAuditlst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caWhere[512],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caFlags[16];
    char caUsername[32],caTo[64],caFrom[64],caSub[128],caSdate[32];
    char caFile[128],caTemp[256];
    char caSmail[64];
    char caCount[16];
    long lUserid,lGroupid,lCurPg,lTotPg,lAtt,lStat,lSize,lUserid0,lSid,lStatus,lCode;
    int i,iNum;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lGroupid0,lRowNum,lflags,lStartRec,lSumUser,lCount;
   	unsigned long lSdate;
 	pasDbCursor *psCur;
 	
 	  long lMbflag;
    char caKey[1024],caMname[32]; 
    char cbcode[1024],caSerno[32];
    char caOk[16],caXzmb[16];
    char szBuf[1024];
    long lId;
 	
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif  



 	  dsCltGetSessionValue(1,"mbflag",UT_TYPE_LONG,sizeof(long),&lMbflag);

    if(lMbflag!=10){
    	  iReturn = utMsgGetSomeNVar(psMsgHead,3,"mname",  UT_TYPE_STRING,12,caMname,
                                         "ok",     UT_TYPE_STRING,10,caOk,
                                         "xzmb",   UT_TYPE_STRING,10,caXzmb);
          if(strlen(caMname)>0){              
          memset(caKey,0,sizeof(caKey)); 
          dsCltGetSessionValue(1,"id",UT_TYPE_LONG,sizeof(long),&lId);
				  sprintf(szBuf,"select mbserno from ncuserbindmb where userid=%d ",lId);

				  pasDbOneRecord(szBuf,0,UT_TYPE_STRING,16,caSerno);
				  sprintf(szBuf,"select mbkey from ncmbserno where mbserno='%s' ",caSerno);
				 
				  pasDbOneRecord(szBuf,0,UT_TYPE_STRING,1000,caKey); 
			  

			  if(strlen(caXzmb)>0){
			   iReturn=ncMBAuth(psShmHead,caKey,caMname,caSerno,2);
			   if(iReturn<1){
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","校正失败");
					utPltPutVar(psDbHead,"fname","ncWebMailCtlAuditlst");
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
			   }
			   else{
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","校正成功");
					utPltPutVar(psDbHead,"fname","ncWebMailCtlAuditlst");
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
   				}
				}
				else{
				
			   iReturn=ncMBAuth(psShmHead,caKey,caMname,caSerno,1);

			   if(iReturn<1){
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","验证失败");
					utPltPutVar(psDbHead,"fname","ncWebMailCtlAuditlst");
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
			   }
			   else{

			   	dsCltSetSessionValue(1,"mbflag",UT_TYPE_LONG,4,10);
			   }
			  }
			    
				
				
			}
			else{
					psDbHead = utPltInitDb();
					   	utPltPutVar(psDbHead,"fname","ncWebMailCtlAuditlst");

   	          utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
   	          return 0;
				}                       
    }



   	
	iReturn = utMsgGetSomeNVar(psMsgHead,4,
	                "flags",     UT_TYPE_STRING, 12, caFlags,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					"CurPg",      UT_TYPE_STRING, 12, caCurPg,
					"lcount",     UT_TYPE_STRING, 12,caCount);
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {
         if(caFlags[0] == '4') {
             sprintf(caWhere,"where a.groupid = %lu and b.mailstat = 4",lGroupid0);
         }
         else {
             sprintf(caWhere,"where a.groupid = %lu and (b.mailstat = 4 or b.mailstat=5)",lGroupid0);
         }            
    }
    else {
         if(caFlags[0] == '4') {
             sprintf(caWhere,"where  b.mailstat = 4");
         }
         else {
             sprintf(caWhere,"where (b.mailstat = 4 or b.mailstat = 5)");
         }            
    }
    utStrAddF(caWhere," order by b.sdate desc");
    if( strstr(caWhere,"a.")) { /* 涉及用户信息  */
        sprintf(caTemp,"select count(*) from ncmailctl b,ncuser a %s",caWhere);
    }
    else {
        sprintf(caTemp,"select count(*) from ncmailctl b %s",caWhere);
    }
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
    if(iDebugLevel == NC_DEBUG_MSGINFO) {        
       utSysLog("[Count=%d]%s",lCount,caTemp);
    }
    if(iReturn < 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
    }

    lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;   
    if( strstr(caWhere,"a.")) { /* 涉及用户信息  */
        psCur = pasDbOpenSqlF("select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.smail,b.ccode \
                from ncmailctl b,ncuser a  %s limit %d,%d",caWhere,lStartRec,lRowNum);
    }
    else {
        psCur = pasDbOpenSqlF("select b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.mailstat,b.status,b.smail,b.ccode \
                from ncmailctl b %s limit %d,%d",caWhere,lStartRec,lRowNum);
    }        
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
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
  	                               UT_TYPE_STRING,63,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lStat,
  	                               UT_TYPE_LONG,4,&lStatus,
  	                               UT_TYPE_STRING,63,caSmail,
  	                               UT_TYPE_LONG,4,&lCode);
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lSdate));
        utPltPutLoopVar(psDbHead,"file", iNum,caFile);
        utPltPutLoopVar(psDbHead,"from", iNum,caFrom);
        utPltPutLoopVar(psDbHead,"flags", iNum,caFlags);
        utPltPutLoopVar(psDbHead,"to", iNum,caTo);
        utPltPutLoopVarF(psDbHead,"ccode", iNum,"%d",lCode);
        utPltPutLoopVar(psDbHead,"sub", iNum,pasCvtGBK(lCode,caSub,caTemp,128));
        utPltPutLoopVar(psDbHead,"smail", iNum,caSmail);
        utPltPutLoopVarF(psDbHead,"size", iNum,"%lu",lSize);
        if(lStat == 4) {
           utPltPutLoopVar(psDbHead,"stat", iNum,ncLang("0626未处理"));
           utPltPutLoopVar(psDbHead,"status", iNum,"0");
        }
        else {
           utPltPutLoopVar(psDbHead,"status", iNum,"1");
            if(lStatus == 1) {
               utPltPutLoopVar(psDbHead,"stat", iNum,ncLang("0722已发送"));
            }
            else if(lStatus == 2) {                
               utPltPutLoopVar(psDbHead,"stat", iNum,ncLang("0340拒绝"));
            }            
            else {
               utPltPutLoopVar(psDbHead,"stat", iNum,ncLang("0874转发"));
            }
        }
        if(lAtt > 0) {
            utPltPutLoopVar(psDbHead,"att", iNum,ncLang("0815有"));
        }
      	iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_ULONG,4,&lSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,63,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lStat,
  	                               UT_TYPE_LONG,4,&lStatus,
  	                               UT_TYPE_STRING,63,caSmail,
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
    utPltPutVar(psDbHead,"cond",caCond);
    utPltPutVar(psDbHead,"flags",caFlags);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    if(caFlags[0] == '4') {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mailctl_audit.htm");
    }
    else {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mailctl_audit1.htm");
    }        
    return 0;
}

/* 邮件审计  */
int ncWebMailCtlAudit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caFile[64],*p;
    char pFfile[128];
    char caOpt[16];
    char caSub[256];
    char caSub1[256];
    char caCont[256];
    char caTempFile[256],caVar[32],caSmail[64];
    int iSum,i,iReturn,iProxy;
    unsigned long lSid;
    if(strcasecmp(utComGetVar_sd(psShmHead,"SmtpProxy","qmail"),"qmail") == 0) {
        iProxy = 0;
    }
    else {
        iProxy = 1;
    }
    
	iReturn = utMsgGetSomeNVar(psMsgHead,2,
	                "opt",     UT_TYPE_STRING, 12, caOpt,
	                "smail",      UT_TYPE_STRING, 63, caSmail);
    for(i=0;i<36;i++) {
        sprintf(caVar,"chk%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            lSid = atol(p);
            iReturn = pasDbOneRecord("select mailfile,mailsub from ncmailctl where sid = :sid",1,
                           "sid",UT_TYPE_LONG,lSid,
                           UT_TYPE_STRING,63,caFile,
                           UT_TYPE_STRING,127,caSub);
    	    sprintf(caTempFile,"%s/ctl/%s",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"),caFile);
            if(iReturn == 0) {
                if(caOpt[0]=='1') { /* 删除 */
   	                remove(caTempFile);
      	            sprintf(caTempFile,"%s/ctl/%s.to",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"),caFile);
   	                remove(caTempFile);
      	            sprintf(caTempFile,"%s/ctl/%s.ses",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"),caFile);
   	                remove(caTempFile);
                    iReturn = pasDbExecSqlF("delete from ncmailctl where sid = %lu ",lSid);
                    iReturn = pasDbExecSqlF("delete from ncmailctli where mid = %lu ",lSid);
                    if(iDebugLevel == NC_DEBUG_MAILAUDIT) {
                        utSysLog("[MailAudit] Delete Mail Sub=%s File=%s",caSub,caTempFile);
                    }
                    if(iReturn == 0) {
                        pasDbCommit(NULL);
                    }
                }
                else if(caOpt[0]=='2') { /* 发送 */
                    if(iProxy == 0) {
                        iReturn = ncMailSendMailTo1(caTempFile,NULL);
                    }
                    else {
                        iReturn = ncUtlMailSmtpSendBySenFile(caTempFile);
                    }
                   if(iDebugLevel == NC_DEBUG_MAILAUDIT) {
                        utSysLog("[MailAudit] Send Mail Sub=%s File=%s",caSub,caTempFile);
                   }
                   if(iReturn == 0) {
                       iReturn = pasDbExecSqlF("update ncmailctl set status = 1,mailstat=5 where sid = %lu ",lSid);
                       if(iReturn == 0) {
                            pasDbCommit(NULL);
                       }
                   }
                }
                else if(caOpt[0]=='4') { /* 转发  */
                    if(!utStrIsSpaces(caSmail)) {
                        sprintf(caSub1,"Fw:%s",caSub);
                        strcpy(caCont,(char *)ncLang("0848这是一封由网络督察转发的邮件。\n"));
                        sprintf(pFfile,"%s/tmp/%lu",utComGetVar_sd(psShmHead,"MailDir","."),utStrGetId());
                        iReturn = mail_digest(caTempFile,pFfile,"ProEiM",caSmail,
                                      caSub1,caCont,0);
                        iReturn = sendMailConFromFile(pFfile,NULL);
                        if(iDebugLevel == NC_DEBUG_MAILAUDIT) {
                            utSysLog("[MailAudit] FwordTo:[%s] Sub=%s caTempFIle=%s File=%s  Return=%d",
                                   caSmail,caSub,caTempFile,pFfile,iReturn);
                        }
                        if(iReturn == 0) {
                             iReturn = pasDbExecSqlF("update ncmailctl set status = 3,mailstat=5 where sid = %lu ",lSid);
                             if(iReturn == 0) {
                                pasDbCommit(NULL);
                             }
                        }
                    }
                }
                else if(caOpt[0]=='3'){  /* 拒绝  */
                    if(iDebugLevel == NC_DEBUG_MAILAUDIT) {
                        utSysLog("[MailAudit] Reject the Mail Sub=%s",caSub);
                    }
                    iReturn = pasDbExecSqlF("update ncmailctl set status = 2,mailstat=5 where sid = %lu ",lSid);
                    if(iReturn == 0) {
                        pasDbCommit(NULL);
                    }
                }
            }
        }
    }
    return ncWebMailCtlAuditlst(psShmHead, iFd,psMsgHead);
}








               
int ncWebMailCtlDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    long iReturn,lGroupid,lGroupid0;
    char caSmt[256];
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect;
    char caGroupid[12];
    psDbHead = utPltInitDb();
    strcpy(caGroupid,"\0");
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {  /* 部门权利  */
        sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu order by groupname",
                  lGroupid0);
        pBuf = (char *)pasUtlCvtSqlSelectA(caSmt,"\0");
        sprintf(caGroupid,"%lu",lGroupid0);
    }
    else {
        pBuf = (char *)pasUtlCvtSqlSelectA("select groupname,groupid from ncgroup where groupid > 0 order by groupname",
               (char *)ncLang("0447全部, ;"));
    }
    if(pBuf) {
        pSelect= (char *)pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
        free(pBuf);
        utPltPutVar(psDbHead,"sgroup", pSelect);
        free(pSelect);
    }    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mail_hsear.htm");
}

/* 历史邮件查询条件  */               
int ncWebMailDispHSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    long iReturn,lGroupid,lGroupid0;
    char caSmt[256];
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect;
    char caGroupid[12];
    psDbHead = utPltInitDb();
    strcpy(caGroupid,"\0");
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {  /* 部门权利  */
        sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu order by groupname",
                  lGroupid0);
        pBuf = (char *)pasUtlCvtSqlSelectA(caSmt,"\0");
        sprintf(caGroupid,"%lu",lGroupid0);
    }
    else {
        pBuf = (char *)pasUtlCvtSqlSelectA("select groupname,groupid from ncgroup where groupid > 0 order by groupname",
               (char *)ncLang("0447全部, ;"));
    }
    if(pBuf) {
        pSelect= (char *)pasUtlCvtInputSelect("groupid",caGroupid,pBuf);
        free(pBuf);
        utPltPutVar(psDbHead,"sgroup", pSelect);
        free(pSelect);
    }    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mail_hhsear.htm");
}

           
/* 拦截邮件查询  */
int ncWebMailCtlSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    iReturn = ncUtlMailCtlSearch(psShmHead,iFd,psMsgHead,"mail/nc_mailctl_mlist.htm","ncmailctl");
    return iReturn;
}

/* 历史拦截邮件查询  */
int ncWebMailCtlHistorySearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    iReturn = ncUtlMailCtlSearch(psShmHead,iFd,psMsgHead,"mail/nc_mailctl_hlist.htm","ncmaillogo");
    return iReturn;
}


/* 邮件处理  */
int ncMailCtlCheck(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    
    mail_header_info *psMailHead;
    mail_body *psMailBody;
    mail_attach   *psAtt;
    char      *pTempDir,*pMsgFile;
    char      caTempDir[128],caFile[128],caFile2[128],caFile1[128];
    char      caMsgFile[128],caUserid[16];
    int i,iNum,iFlags,iAttFlags,fd;
    char caMd5[32],caSid[32];
    utPltDbHead *psDbHead;
   	int iReturn;
//  utMsgPrintMsg(psMsgHead);   	
    iReturn = utMsgGetSomeNVar(psMsgHead,2,
                    "sid",         UT_TYPE_STRING, 31,caSid,
	                "fname",       UT_TYPE_STRING, 63, caMsgFile);
    utMd5Ascii22(caMsgFile,strlen(caMsgFile),"pronetway",caMd5);
    ncMilReplaceAt(caMd5);
    if(strcmp(caMd5,caSid) != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0628未授权的操作"));
        return 0;
    }        
	                
    if(utStrIsSpaces(caMsgFile)) { /* 邮件无内容  */
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0785邮件"),ncLang("0809邮件无内容"));
        return 0;
    }
    
 	/* 读邮件头信息  */
    pTempDir = utComGetVar_sd(psShmHead,"MailDir",".");
 	sprintf(caFile2,"%s/ctl/%s",pTempDir,caMsgFile);
 	psMailHead = mail_header(caFile2,&iFlags);
    if(psMailHead == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0808邮件头不正确"));
        return 0;
    }
    sprintf(caTempDir,"%s/tmp/%lu",pTempDir,utStrGetId());
    if(utFileIsExist(caTempDir)!=1) {
        if(mkdir(caTempDir,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)!=0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0515 生成用户目录错误"));
            return 0;
        }
    }
    
    for(i=strlen(caMsgFile)-1;caMsgFile[i]!='/' && i > 0;i--);
    if(i>0) {
        pMsgFile = caMsgFile + i + 1;
    }
    else {
        pMsgFile = caMsgFile;
    }
    printf("caTempDir==[%s]\n",caTempDir);
    sprintf(caFile1,"%s/%s",caTempDir,pMsgFile);
    psMailBody = mail_getbody(caFile2,caTempDir,caFile1,"/pronline/Msg?FunName@ncWebMailCtlDownload&fname@",&iFlags);
    if(psMailBody == NULL) {
        mailHeadFree(psMailHead);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0808邮件头不正确"));
    }
    

   	/* 生成邮件头  */
    psDbHead=utPltInitDbHead();
    if(psMailHead->addressF) {
        utPltPutVar(psDbHead,"from",psMailHead->addressF);
    }
    if(psMailHead->personF) {
        utPltPutVar(psDbHead,"fromp",psMailHead->personF);
    }
    if(psMailHead->addressT) {
        utPltPutVar(psDbHead,"to",psMailHead->addressT);
    }
    if(psMailHead->personT) {
        utPltPutVar(psDbHead,"top",psMailHead->personT);
    }
    if(psMailHead->addressCc) {
        utPltPutVar(psDbHead,"cc",psMailHead->addressCc);
    }
    if(psMailHead->personCc) {
        utPltPutVar(psDbHead,"ccp",psMailHead->personCc);
    }
    if(psMailHead->addressBcc) {
        utPltPutVar(psDbHead,"bcc",psMailHead->addressBcc);
    }
    if(psMailHead->subject) {
        utPltPutVar(psDbHead,"sub",psMailHead->subject);
    }
    
    if(psMailHead->date) {
        utPltPutVar(psDbHead,"date",psMailHead->date);
    }
    /* 正文  */

    if(psMailBody->contype == 0) { /* 文本  */
        sprintf(caFile,"%s/%s.txt",caTempDir,pMsgFile);
        fd = open(caFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if(fd == (-1)) {
		    mailHeadFree(psMailHead);
            freeBody(psMailBody);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0648文件生成错"));
		    return 0;
		}
		if(psMailBody->textcon) {
       	    write(fd, psMailBody->textcon, strlen(psMailBody->textcon));
       	}
       	close(fd);
        utPltPutVar(psDbHead,"type","text/plain");
        utPltPutVar(psDbHead,"dfile","file.txt");
    }
    else {  /* html */
        sprintf(caFile,"%s/%s.htm",caTempDir,pMsgFile);
        fd = open(caFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if(fd == (-1)) {
		    mailHeadFree(psMailHead);
            freeBody(psMailBody);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0648文件生成错"));
		    return 0;
		}
		if(psMailBody->htmlcon) {
       	    write(fd, psMailBody->htmlcon, strlen(psMailBody->htmlcon));
       	}
       	close(fd);
        utPltPutVar(psDbHead,"type","text/html");
        utPltPutVar(psDbHead,"dfile","file.htm");
    }
    utPltPutVar(psDbHead,"msgfile",caMsgFile);
    utPltPutVar(psDbHead,"fname",caFile);
    utPltPutVar(psDbHead,"sid",caSid);
    if(psMailBody->attach) {
        psAtt = psMailBody->attach;
        iNum = 0;
        while(psAtt) {
            iNum++;
            utPltPutLoopVar(psDbHead,"disp",iNum,psAtt->displayname);
            utPltPutLoopVar(psDbHead,"name",iNum,psAtt->filename);
            utPltPutLoopVarF(psDbHead,"fsize",iNum,"%lu",psAtt->filesize);
            utPltPutLoopVar(psDbHead,"type",iNum,psAtt->content_type);
            psAtt = psAtt->next;
        }
    }
    mailHeadFree(psMailHead);
    freeBody(psMailBody);
  	utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mailread_form_ctl3.htm");
    return 0;
}


/* 邮件处理  */
int ncMailMonCheck(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    
    mail_header_info *psMailHead;
    mail_body *psMailBody;
    mail_attach   *psAtt;
    char      *pTempDir,*pMsgFile;
    char      caTempDir[128],caFile[128],caFile2[128],caFile1[128];
    char      caMsgFile[128],caUserid[16];
    int i,iNum,iFlags,iAttFlags,fd;
    char caMd5[32],caSid[32];
    utPltDbHead *psDbHead;
   	int iReturn;
    iReturn = utMsgGetSomeNVar(psMsgHead,2,
                    "sid",         UT_TYPE_STRING, 31,caSid,
	                "fname",       UT_TYPE_STRING, 63, caMsgFile);
    utMd5Ascii22(caMsgFile,strlen(caMsgFile),"pronetway",caMd5);
    ncMilReplaceAt(caMd5);
    if(strcmp(caMd5,caSid) != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0628未授权的操作"));
        return 0;
    }        
	                
    if(utStrIsSpaces(caMsgFile)) { /* 邮件无内容  */
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0785邮件"),ncLang("0809邮件无内容"));
        return 0;
    }
    
 	/* 读邮件头信息  */
    pTempDir = utComGetVar_sd(psShmHead,"MailDir",".");
 	sprintf(caFile2,"%s/mon/%s",pTempDir,caMsgFile);
 	psMailHead = mail_header(caFile2,&iFlags);
    if(psMailHead == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0808邮件头不正确"));
        return 0;
    }
    sprintf(caTempDir,"%s/tmp/%lu",pTempDir,utStrGetId());
    if(utFileIsExist(caTempDir)!=1) {
        if(mkdir(caTempDir,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)!=0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0515 生成用户目录错误"));
            return 0;
        }
    }
    
    for(i=strlen(caMsgFile)-1;caMsgFile[i]!='/' && i > 0;i--);
    if(i>0) {
        pMsgFile = caMsgFile + i + 1;
    }
    else {
        pMsgFile = caMsgFile;
    }
    sprintf(caFile1,"%s/%s",caTempDir,pMsgFile);
    psMailBody = mail_getbody(caFile2,caTempDir,caFile1,"/pronline/Msg?FunName@ncWebMailCtlDownload&fname@",&iFlags);
    if(psMailBody == NULL) {
        mailHeadFree(psMailHead);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0808邮件头不正确"));
    }
    

   	/* 生成邮件头  */
    psDbHead=utPltInitDbHead();
    if(psMailHead->addressF) {
        utPltPutVar(psDbHead,"from",psMailHead->addressF);
    }
    if(psMailHead->personF) {
        utPltPutVar(psDbHead,"fromp",psMailHead->personF);
    }
    if(psMailHead->addressT) {
        utPltPutVar(psDbHead,"to",psMailHead->addressT);
    }
    if(psMailHead->personT) {
        utPltPutVar(psDbHead,"top",psMailHead->personT);
    }
    if(psMailHead->addressCc) {
        utPltPutVar(psDbHead,"cc",psMailHead->addressCc);
    }
    if(psMailHead->personCc) {
        utPltPutVar(psDbHead,"ccp",psMailHead->personCc);
    }
    if(psMailHead->addressBcc) {
        utPltPutVar(psDbHead,"bcc",psMailHead->addressBcc);
    }
    if(psMailHead->subject) {
        utPltPutVar(psDbHead,"sub",psMailHead->subject);
    }
    
    if(psMailHead->date) {
        utPltPutVar(psDbHead,"date",psMailHead->date);
    }
    /* 正文  */

    if(psMailBody->contype == 0) { /* 文本  */
        sprintf(caFile,"%s/%s.txt",caTempDir,pMsgFile);
        fd = open(caFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if(fd == (-1)) {
		    mailHeadFree(psMailHead);
            freeBody(psMailBody);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0648文件生成错"));
		    return 0;
		}
		if(psMailBody->textcon) {
       	    write(fd, psMailBody->textcon, strlen(psMailBody->textcon));
       	}
       	close(fd);
        utPltPutVar(psDbHead,"type","text/plain");
        utPltPutVar(psDbHead,"dfile","file.txt");
    }
    else {  /* html */
        sprintf(caFile,"%s/%s.htm",caTempDir,pMsgFile);
        fd = open(caFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if(fd == (-1)) {
		    mailHeadFree(psMailHead);
            freeBody(psMailBody);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0648文件生成错"));
		    return 0;
		}
		if(psMailBody->htmlcon) {
       	    write(fd, psMailBody->htmlcon, strlen(psMailBody->htmlcon));
       	}
       	close(fd);
        utPltPutVar(psDbHead,"type","text/html");
        utPltPutVar(psDbHead,"dfile","file.htm");
    }
    utPltPutVar(psDbHead,"msgfile",caMsgFile);
    utPltPutVar(psDbHead,"fname",caFile);
    utPltPutVar(psDbHead,"sid",caSid);
    if(psMailBody->attach) {
        psAtt = psMailBody->attach;
        iNum = 0;
        while(psAtt) {
            iNum++;
            utPltPutLoopVar(psDbHead,"disp",iNum,psAtt->displayname);
            utPltPutLoopVar(psDbHead,"name",iNum,psAtt->filename);
            utPltPutLoopVarF(psDbHead,"fsize",iNum,"%lu",psAtt->filesize);
            utPltPutLoopVar(psDbHead,"type",iNum,psAtt->content_type);
            psAtt = psAtt->next;
        }
    }
    mailHeadFree(psMailHead);
    freeBody(psMailBody);
  	utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_mailread_form_mon1.htm");
    return 0;
}


  
/* 邮件审计---通过邮件  */
int ncMailAudit1(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caFile[128],*p;
    char pFfile[128];
    char caMsg[64];
    char caOpt[16];
    char caSub[256];
    char caSub1[256];
    char caCont[256],caSid[32],caMd5[32];
    char caTempFile[256],caVar[32],caSmail[64];
    int iSum,i,iReturn;
    unsigned long lSid;
    int iProxy;
    if(strcasecmp(utComGetVar_sd(psShmHead,"SmtpProxy","qmail"),"qmail") == 0) {
        iProxy = 0;
    }
    else {
        iProxy = 1;
    }
//  utMsgPrintMsg(psMsgHead);    
	iReturn = utMsgGetSomeNVar(psMsgHead,3,
	                "sid",     UT_TYPE_STRING, 31,caSid,
	                "opt",     UT_TYPE_STRING, 12, caOpt,
	                "msgfile", UT_TYPE_STRING, 63, caFile);
    
    utMd5Ascii22(caFile,strlen(caFile),"pronetway",caMd5);
    ncMilReplaceAt(caMd5);
printf(" Md5==%s   Sid==%s \n",caMd5,caSid);
    if(strcmp(caMd5,caSid) != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0628未授权的操作"));
        return 0;
    }        
    iReturn = pasDbOneRecord("select sid,mailsub from ncmailctl where mailfile = :mf",1,
                           "mf",UT_TYPE_STRING,caFile,
                           UT_TYPE_LONG,4,&lSid,
                           UT_TYPE_STRING,127,caSub);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0787邮件不存在"));
        return 0;
    }
    sprintf(caTempFile,"%s/ctl/%s",utComGetVar_sd(psShmHead,"MailDir","."),caFile);
    if(caOpt[0]=='2') { /* 发送 */
        if(iProxy == 0) {
            iReturn = ncMailSendMailTo1(caTempFile,NULL);
        }
        else {
            iReturn = ncUtlMailSmtpSendBySenFile(caTempFile);
        }
        utSysLog("[MailAudit] Send Mail Sub=%s File=%s  iReturn==%d",caSub,caTempFile,iReturn);
        if(iReturn == 0) {
             iReturn = pasDbExecSqlF("update ncmailctl set status = 1,mailstat=5 where sid = %lu",lSid);
             if(iReturn == 0) {
                 pasDbCommit(NULL);
                 ncUtlMailReadOne(psShmHead,"ctl",iFd,psMsgHead,"mail/nc_mailread_form_ctl1.htm");                 
                 return 0;
             }
         }
     }
     else if(caOpt[0]=='3'){  /* 拒绝  */
         if(iDebugLevel == NC_DEBUG_MAILAUDIT) {
              utSysLog("[MailAudit] Reject the Mail Sub=%s",caSub);
         }
         iReturn = pasDbExecSqlF("update ncmailctl set status = 2,mailstat=5 where sid = %lu ",lSid);
         if(iReturn == 0) {
              utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0786邮件被拒绝发送"));
              pasDbCommit(NULL);
              return 0;
         }
     }
     utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0789邮件处理出错"));
      return 0;
}





int ncUtlMailReadSelf(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
    char caType[16];
    char caPlate[128];
    int iReturn;
    iReturn = utMsgGetSomeNVar(psMsgHead,2,
                    "mailtype",    UT_TYPE_STRING, 12, caType,
                    "plate",       UT_TYPE_STRING, 127, caPlate);
    return ncUtlMailReadOne(psShmHead,caType, iFd,psMsgHead,caPlate);
}

//返回个数
int nc_SepChar(char *caQue,char c,char caObj[100][50])
{
        int i=0,j=0,z=0,flag=0;
        char caTemp[100][50];
        if(strlen(caQue) == 0)
                return 0;
        while( (*caQue)!= '\0' )
        {
              if(flag==0)
              {
                strcpy(caTemp[i],"");
                                }
                if( (*caQue) == c )
                {
                    caQue++;
                    caTemp[i][j] = '\0';
                    i++;
                    j=0;
                    flag=0;
                }
                else
                {
                    caTemp[i][j] = (*caQue);
                    caQue++;
                    j++;
                    flag=1;
                }
                if(j>49||i>99)
                {
                        break;
                }
        }
        caTemp[i][j]='\0';
        for(z=0;z<i+1;z++)
        {
        	memset(caObj[z],0,sizeof(caObj[z]));
        	strncpy(caObj[z],caTemp[z],31);
     //       strcpy(caObj[z],caTemp[z]);
        }
        return i+1;
}



/* 读一封邮件
   pType== 'ctl' 'mon'
 */
int ncUtlMailReadOne(utShmHead *psShmHead,char *pType, int iFd,utMsgHead *psMsgHead,char *pPlate)
{
    
    mail_header_info *psMailHead;
    mail_body *psMailBody;
    mail_attach *psAtt;
    int iRet;
    int lCount;
    char *pTempDir,*pMsgFile;
    char *pSelect;
    char caTempDir[128],caFile[128],caFile2[128],caFile1[128];
    char caMsgFile[128],caUserid[16];
    char caCode[16],caCvt[16];
    char caMailto[128];
    char caObj[100][50];
    char caOut[10204];

    int i,iNum,iFlags,iAttFlags,fd;
    utPltDbHead *psDbHead;
    char caMsid[16],caMailfrom[64],caMailsub[128],caSdate[20];
    char caTemp[6000],caContent[6000];
   	int iReturn,lCode;
   	
  //  utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead,4,
                    "ccode",       UT_TYPE_STRING, 12, caCode,
                    "userid",      UT_TYPE_STRING, 12, caUserid,
	                  "msgfile",     UT_TYPE_STRING, 63, caMsgFile,
	                  "msid",        UT_TYPE_STRING,10,caMsid);
#ifdef LDEBUG
    printf(" MsgFile==%s \n",caMsgFile);
#endif
    if(utStrIsSpaces(caMsgFile)) { /* 邮件无内容  */
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0785邮件"),ncLang("0809邮件无内容"));
        return 0;
    }
    lCode = atol(caCode);
    pTempDir = utComGetVar_sd(psShmHead,"MailDir",".");
 	sprintf(caFile2,"%s/%s/%s",pTempDir,pType,caMsgFile);
 	psMailHead = mail_header(caFile2,&iFlags);
    if(psMailHead == NULL) {
    	    	 if(strlen(caMsid)>0){
    	  		sprintf(caTemp,"select sdate,mailfrom,mailto,mailsub,ccode,content from ncmailmon where sid=%s ",caMsid);
    	  		memset(caSdate,0,sizeof(caSdate));
    	  		memset(caMailfrom,0,sizeof(caMailfrom));
    	  		memset(caMailto,0,sizeof(caMailto));
    	  		memset(caMailsub,0,sizeof(caMailsub));
    	  		lCode=0;
    	  		pasDbOneRecord(caTemp,0,UT_TYPE_STRING,19,caSdate,
    	  		                        UT_TYPE_STRING,63,caMailfrom,
    	  		                        UT_TYPE_STRING,63,caMailto,
    	  		                        UT_TYPE_STRING,127,caMailsub,
    	  		                        UT_TYPE_LONG,4,&lCode,
    	  		                        UT_TYPE_STRING,6000,caContent);
    	  		      psDbHead=utPltInitDbHead();
	
					 
					        utPltPutVar(psDbHead,"from",caMailfrom);
					        utPltPutVar(psDbHead,"to",caMailto);
					        utPltPutVar(psDbHead,"sub",(char *)pasCvtGBK(lCode,caMailsub,caTemp,255));
					        utPltPutVar(psDbHead,"content",(char *)pasCvtGBK(lCode,caContent,caTemp,5999));
					        utPltPutVar(psDbHead,"date",caSdate);
					        utPltPutVar(psDbHead,"msid",caMsid);
					        
					        utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/ncs_mailmonread_form_db.htm");
					        return 0;
					        
				  }
    	
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0808邮件头不正确"));
        return 0;
    }
    sprintf(caTempDir,"%s/tmp/%lu",pTempDir,utStrGetId());
    if(utFileIsExist(caTempDir)!=1) {
        if(mkdir(caTempDir,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)!=0) {
		    mailHeadFree(psMailHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0515 生成用户目录错误"));
            return 0;
        }
    }
    
    for(i=strlen(caMsgFile)-1;caMsgFile[i]!='/' && i > 0;i--);
    if(i>0) {
        pMsgFile = caMsgFile + i + 1;
    }
    else {
        pMsgFile = caMsgFile;
    }
    sprintf(caFile1,"%s/%s",caTempDir,pMsgFile);
    psMailBody = mail_getbody(caFile2,caTempDir,caFile1,"/pronline/Msg?FunName@ncWebMailCtlDownload&fname@",&iFlags);
    if(psMailBody == NULL) {
        mailHeadFree(psMailHead);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0808邮件头不正确"));
        return 0;
    }
 #ifdef LDEBUG
    printf(" Begin Out==%s \n",caMsgFile);
#endif
   

   	/* 生成邮件头  */
    psDbHead=utPltInitDbHead();
    utPltPutVar(psDbHead,"msgfile",caMsgFile);
    if(psMailHead->personF) {
        utPltPutLoopVar(psDbHead,"fromp", iNum,pasCvtGBK(lCode,psMailHead->personF,caTemp,128));
    }
    if(psMailHead->addressF) {
        utPltPutVar(psDbHead,"from",psMailHead->addressF);
    }

    if(psMailHead->addressT) {
        utPltPutVar(psDbHead,"to",psMailHead->addressT);
    }
    if(psMailHead->personT) {
        utPltPutVar(psDbHead,"top",psMailHead->personT);
    }
    if(psMailHead->addressCc) {
        utPltPutVar(psDbHead,"cc",psMailHead->addressCc);
    }
    if(psMailHead->personCc) {
        utPltPutVar(psDbHead,"ccp",psMailHead->personCc);
    }
    if(psMailHead->addressBcc) {
        utPltPutVar(psDbHead,"bcc",psMailHead->addressBcc);
    }
    else{
    	memset(caMailto,0,sizeof(caMailto));
    	
    	 if(strlen(caMsid)>0){
    	 	sprintf(caTemp,"select mailto from ncmailmon where sid=%s ",caMsid);
    	}
    	else{
    	
    	sprintf(caTemp,"select mailto from ncmailmon where userid=%d and mailfile='%s' ",atol(caUserid),caMsgFile);
    }
    	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,127,caMailto);
   	
    	if(strlen(caMailto)>0){
        memset(caTemp,0,sizeof(caTemp));
    		lCount=nc_SepChar(caMailto,';',caObj);
    		for(i=0;i<lCount;i++){
    			
    			if(psMailHead->addressCc){
    				if(strstr(psMailHead->addressCc,caObj[i]))
    				 continue;
    			}
    			if(psMailHead->addressT){
    				if(strstr(psMailHead->addressT,caObj[i]))
    				 continue;
    			}
    			
    			 sprintf(caTemp+strlen(caTemp),"%s ",caObj[i]);  
    			 
    			
    		}
    		utPltPutVar(psDbHead,"bcc",caTemp);
    	}
    }
    	
    	
    
    if(psMailHead->subject) {
    	iRet=pasCharIsUtf8(psMailHead->subject,strlen(psMailHead->subject));
    	memset(caTemp,0,sizeof(caTemp));
    	if(iRet){
    		pasUtf8ToGBK(psMailHead->subject,caTemp,255);
    	}
    	else{
    		pasCvtGBK(lCode,psMailHead->subject,caTemp,255);
    	}
        utPltPutVar(psDbHead,"sub",caTemp);
    }
    
    if(psMailHead->date) {
        utPltPutVar(psDbHead,"date",psMailHead->date);
    }
    /* 正文  */
    utPltPutVar(psDbHead,"userid",caUserid);
    utPltPutVar(psDbHead,"msgfile",caMsgFile);
    utPltPutVar(psDbHead,"plate",pPlate);
    utPltPutVar(psDbHead,"mailtype",pType);

    if(psMailBody->contype == 0) { /* 文本  */
        sprintf(caFile,"%s/%s.txt",caTempDir,pMsgFile);
        fd = open(caFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if(fd == (-1)) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0648文件生成错"));
		    return 0;
		}
		if(psMailBody->textcon) {
            long lSize;
            char *pTemp;
		    lSize = strlen(psMailBody->textcon)+100;
		    pTemp = malloc(lSize);
		    
		    iRet=pasCharIsUtf8(psMailBody->textcon,strlen(psMailBody->textcon));
		    if(iRet){
		    	pasUtf8ToGBK(psMailBody->textcon,pTemp,lSize-1);
		    }
		    else{
		    		pasCvtGBK(lCode,psMailBody->textcon,pTemp,lSize-1);
		     }
		    
		    
		    
		    ncMilAddBr(fd,pTemp);
		    free(pTemp);
       	}
       	close(fd);
        utPltPutVar(psDbHead,"type","text/plain");
        utPltPutVar(psDbHead,"dfile","file.txt");
    }
    else {  /* html */
        sprintf(caFile,"%s/%s.htm",caTempDir,pMsgFile);
        fd = open(caFile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if(fd == (-1)) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0214读邮件"),ncLang("0648文件生成错"));
		    return 0;
		}
		if(psMailBody->htmlcon) {
            long lSize;
            char *pTemp;
		    lSize = strlen(psMailBody->htmlcon)+100;
		    pTemp = malloc(lSize);
		    
		    iRet=pasCharIsUtf8(psMailBody->htmlcon,strlen(psMailBody->htmlcon));
		    if(iRet){
		    	pasUtf8ToGBK(psMailBody->htmlcon,pTemp,lSize-1);
		    }
		    else{
		    		pasCvtGBK(lCode,psMailBody->htmlcon,pTemp,lSize-1);
		     }
       	    write(fd, pTemp,strlen(pTemp));
       	    free(pTemp);
       	}
       	close(fd);
        utPltPutVar(psDbHead,"type","text/html");
        utPltPutVar(psDbHead,"dfile","file.htm");
    }
 #ifdef LDEBUG
    printf("1111 Begin Out==%s \n",caMsgFile);
#endif

    utPltPutVar(psDbHead,"fname",caFile);
    if(psMailBody->attach) {
        psAtt = psMailBody->attach;
        iNum = 0;
        while(psAtt) {
            if(strcasecmp(psAtt->content_type,"application/ms-tnef") == 0) {
                /* Following is add by Liyunming 2004/07/01 
                   Read the winmail.dat格式 邮件                     */
                struct WinMailLst_s {
                    char                *name;
                    char                *dispname;
                    unsigned            long lSize;
                    struct WinMailLst_s   *next;
                } *ps,*ps0;
                iReturn = ncMilExpWinmail(psAtt->filename,caTempDir,&ps);
                if(iReturn < 0) {
                    iNum++;
                    iRet=pasCharIsUtf8(psAtt->displayname, strlen(psAtt->displayname));
                    if(iRet){
                    	pasUtf8ToGBK(psAtt->displayname,caOut,10000);
                    	utPltPutLoopVar(psDbHead,"disp",iNum,caOut);
                    }
                    else{
                    	utPltPutLoopVar(psDbHead,"disp",iNum,psAtt->displayname);
 //                   utPltPutLoopVar(psDbHead,"disp",iNum,pasCvtGBK(lCode,psAtt->displayname,caTemp,255));
                  }
                    utPltPutLoopVar(psDbHead,"name",iNum,psAtt->filename);
                    utPltPutLoopVarF(psDbHead,"fsize",iNum,"%lu",psAtt->filesize);
                    utPltPutLoopVar(psDbHead,"type",iNum,psAtt->content_type);
                }
                else {
                    ps0 = ps;
                    while(ps) {
                        iNum++;
                        iRet=pasCharIsUtf8(ps->name, strlen(ps->name));
                     if(iRet){
                    	pasUtf8ToGBK(ps->name,caOut,10000);
                    	utPltPutLoopVar(psDbHead,"disp",iNum,caOut);
                    }
                    else{
                    	utPltPutLoopVar(psDbHead,"disp",iNum,ps->name);
 //                   utPltPutLoopVar(psDbHead,"disp",iNum,pasCvtGBK(lCode,psAtt->displayname,caTemp,255));
                    }
                        
                        
 //                       utPltPutLoopVar(psDbHead,"disp",iNum,pasCvtGBK(lCode,ps->dispname,caTemp,255));
                        utPltPutLoopVar(psDbHead,"name",iNum,ps->name);
                        utPltPutLoopVarF(psDbHead,"fsize",iNum,"%lu",ps->lSize);
                        utPltPutLoopVar(psDbHead,"type",iNum,"\0");
                        ps = ps->next;
                    }        
                    freewinmaillist(ps0);
                }
            }
            else {
                iNum++;
                
                  iRet=pasCharIsUtf8(psAtt->displayname, strlen(psAtt->displayname));
                    if(iRet){
                    	pasUtf8ToGBK(psAtt->displayname,caOut,10000);
                    	utPltPutLoopVar(psDbHead,"disp",iNum,caOut);
                    }
                    else{
                    	utPltPutLoopVar(psDbHead,"disp",iNum,psAtt->displayname);
 //                   utPltPutLoopVar(psDbHead,"disp",iNum,pasCvtGBK(lCode,psAtt->displayname,caTemp,255));
                  }
                
                
   //             utPltPutLoopVar(psDbHead,"disp",iNum,pasCvtGBK(lCode,psAtt->displayname,caTemp,255));
                utPltPutLoopVar(psDbHead,"name",iNum,psAtt->filename);
                utPltPutLoopVarF(psDbHead,"fsize",iNum,"%lu",psAtt->filesize);
                utPltPutLoopVar(psDbHead,"type",iNum,psAtt->content_type);
            }
            psAtt = psAtt->next;
        }
    }
 #ifdef LDEBUG
    printf("33333 Begin Out==%s \n",caMsgFile);
#endif
    pSelect= (char *)pasUtlCvtInputSelect("ccode",caCode,(char *)ncLang("0926简体,0;繁体,1;UTF8,2;日文,3;"));
    utPltPutVar(psDbHead,"ccode", pSelect);
    free(pSelect);

    mailHeadFree(psMailHead);
    freeBody(psMailBody);
  	utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
    return 0;
}



/* 单封邮件的处理  */
int ncMailAuditOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caFile[64],*p;
    char pFfile[128];
    char caSend[16],caReject[12],caFw[12];
    char caSub[128],caSid[32],caMd5[32];
    
    char caTempFile[256],caVar[32],caSmail[64];
    char *pMailDir;
    int  iProxy;
    int iSum,i,iReturn;
    unsigned long lSid;
	iReturn = utMsgGetSomeNVar(psMsgHead,4,
	                "send",    UT_TYPE_STRING,11, caSend,
	                "reject",  UT_TYPE_STRING,11, caReject,
	                "fw",      UT_TYPE_STRING,11, caFw,
	                "msgfile",   UT_TYPE_STRING, 63, caFile);
    if(strcasecmp(utComGetVar_sd(psShmHead,"SmtpProxy","qmail"),"qmail") == 0) {
        iProxy = 0;
    }
    else {
        iProxy = 1;
    }
    pMailDir = utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog");
    iReturn = pasDbOneRecord("select sid,mailsub from ncmailctl where mailfile = :mf",1,
                           "mf",UT_TYPE_STRING,caFile,
                           UT_TYPE_LONG,4,&lSid,
                           UT_TYPE_STRING,127,caSub);
    if(iReturn != 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0787邮件不存在"));
        return 0;
    }
    sprintf(caTempFile,"%s/ctl/%s",pMailDir,caFile);
    if(!utStrIsSpaces(caSend)) {
        if(iProxy == 0) {
            iReturn = ncMailSendMailTo1(caTempFile,NULL);
        }
        else {
            iReturn = ncUtlMailSmtpSendBySenFile(caTempFile);
        }
        if(iReturn == 0) {
             iReturn = pasDbExecSqlF("update ncmailctl set status = 1,mailstat=5 where sid = %lu",lSid);
             if(iReturn == 0) {
                 utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0421您的邮件已发送"));
                 pasDbCommit(NULL);
                 return 0;
             }
         }
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0927邮件发送出错"));
         return 0;
     }
     else if(!utStrIsSpaces(caReject)) {  /* 拒绝  */
         iReturn = pasDbExecSqlF("update ncmailctl set status = 2,mailstat=5 where sid = %lu ",lSid);
         if(iReturn == 0) {
              utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0786邮件被拒绝发送"));
              pasDbCommit(NULL);
              return 0;
         }
     }
     else if(!utStrIsSpaces(caFw)) { /* 转发  */
         ncWebMailMonFwOne(psShmHead,iFd,psMsgHead);
     }
     else {
        ncUtlMailReadSelf(psShmHead,iFd,psMsgHead);
     }
     return 0;
}



/* 查看邮件原文  */
int ncWebMailReadSource(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    char caFname[64];
    unsigned long lAsize,lSize;
    char caFile1[128],caFile2[128],caPath[128],caFile[128];
    char caTemp[128],caMsg[128];
    char caDate[64],caFrom[128],caTo[128],caSub[128];
    char *p,*pMailDir;
    char caVar[32];
    long lUserid;
    int iCount,iReturn,i;
    unsigned long lSid;
    char caSid[16];
#ifdef LDEBUG
printf(" Now Exp MailMon \n");
utMsgPrintMsg(psMsgHead);   	
#endif
 utMsgGetSomeNVar(psMsgHead,1,"sid",    UT_TYPE_STRING,10, caSid);
 lSid=atol(caSid);
 iReturn=pasDbOneRecord("select mailfile from ncmailmon where sid = :sid",1,
                           "sid",UT_TYPE_LONG,lSid,
                            UT_TYPE_STRING,63,caFname);
 if(iReturn==0){
 	 pMailDir = utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog");
//   strcpy(caFile,caFname);
   char *p;
   char caFile1[128];
    memset(caTemp,0,sizeof(caTemp));
   	strncpy(caTemp,caFname,5);
   p=strstr(caFname,"/");
   if((p)&&strlen(p)>4) {
   	

   	
   	strcpy(caFile,p+4);
 
   	
   	sprintf(caFile1,"%s.eml",caFile);
   	
   	sprintf(caPath,"%s/mon/%s",pMailDir,caTemp);
 //  	printf("caPath=%s\n",caPath);
   	 utPltFileDownload(iFd,"message/rfc822",caPath,caFile,caFile1);
          
     return 0;
   	
  }
  }
                    
            

    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0214读邮件"),ncLang("0379没有选择要导出的邮件"));
  
    return 0;
}


