/* 监控邮件查询  
      (1)监控邮件查询
      (2)导入的监控邮件查询
 */
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
#include "pasfile.h"
#include "ncpost.h"
#include "ncdef.h"
#include "pasutl.h"
#include "mail.h"
/* 监控邮件查询  */
int ncWebMailMonList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caConflag[12];
    long lMbflag;
    char caKey[1024],caMname[32]; 
    char cbcode[1024],caSerno[32];
    char caOk[16],caXzmb[16];
    char szBuf[1024];
    long lId;
    utPltDbHead *psDbHead;
    lMbflag=0;
    iReturn = utMsgGetSomeNVar(psMsgHead,1,"con_flag",   UT_TYPE_STRING,10,caConflag);
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
					utPltPutVar(psDbHead,"fname","ncWebMailMonList");
   	      utPltPutVar(psDbHead,"con_flag",caConflag);
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
			   }
			   else{
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","校正成功");
					utPltPutVar(psDbHead,"fname","ncWebMailMonList");
   	      utPltPutVar(psDbHead,"con_flag",caConflag);
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
   				}
				}
				else{
				
			   iReturn=ncMBAuth(psShmHead,caKey,caMname,caSerno,1);

			   if(iReturn<1){
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","验证失败");
					utPltPutVar(psDbHead,"fname","ncWebMailMonList");
   	      utPltPutVar(psDbHead,"con_flag",caConflag);
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
					   	utPltPutVar(psDbHead,"fname","ncWebMailMonList");
   	          utPltPutVar(psDbHead,"con_flag",caConflag);
   	          if(atol(caConflag)>0)
      					utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauthonline.htm");
      				else
   	          utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
   	          return 0;
				}                       
    }
    
    
    
    
    
    utMsgDelVar(psMsgHead,"action");
	  utMsgAdd_s(&psMsgHead,"action","06");
    ncOperWriteLog(psMsgHead);
    
    if(atol(caConflag)==5)
				ncUtlMailMonSearch(psShmHead,iFd,psMsgHead,"ncs/nc_mailmononlinelist.htm","ncmailmon");
    else
        iReturn = ncUtlMailMonList(psShmHead,iFd,psMsgHead,"ncmailmon","mail/nc_mailmon_mlist.htm");
    return iReturn;
}

/* 导入的监控邮件查询  */
int ncWebMailMonList_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    iReturn = ncUtlMailMonList(psShmHead,iFd,psMsgHead,"ncmaillogo1","mail/nc_mailmon_hmlist.htm");
    return iReturn;
}


/* 监控邮件高级查询  */
int ncWebMailMonHsear(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    iReturn = ncUtlMailMonSearch(psShmHead,iFd,psMsgHead,"mail/nc_mailmon_mlist.htm","ncmailmon");
    return iReturn;
}

/* 历史监控邮件高级查询  */
int ncWebMailMonHsear_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    iReturn = ncUtlMailMonSearch(psShmHead,iFd,psMsgHead,"mail/nc_mailmon_hmlist.htm","ncmaillogo1");
    return iReturn;
}

/* 读一封备份的邮件  */
int ncWebMailMonRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    return ncUtlMailReadOne(psShmHead,"mon",iFd,psMsgHead,"mail/nc_mailmonread_form.htm");
}
    


/* 删除邮件  */
int ncWebMailMonDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncUtlMailMonDel(psShmHead,iFd,psMsgHead,"ncmailmon");
    return ncWebMailMonList(psShmHead, iFd,psMsgHead);
}




/* 删除邮件  */
int ncWebMailMonDel_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncUtlMailMonDel(psShmHead,iFd,psMsgHead,"ncmaillogo1");
    return ncWebMailMonList_h(psShmHead, iFd,psMsgHead);
}


/* 转发选择的邮件  */
int ncWebMailMonFw(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncUtlMailMonFw(psShmHead,iFd,psMsgHead,"ncmailmon","mon");
    return ncWebMailMonList(psShmHead, iFd,psMsgHead);
}

int ncWebMailMonFw_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncUtlMailMonFw(psShmHead,iFd,psMsgHead,"ncmaillogo1");
    return ncWebMailMonList(psShmHead, iFd,psMsgHead);
}

int ncWebMailMonDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    return ncUtlMailMonDispSform(psShmHead,iFd,psMsgHead,"mail/nc_mail_monhsear.htm");
}

int ncWebMailMonDispSform_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    return ncUtlMailMonDispSform(psShmHead,iFd,psMsgHead,"mail/nc_mail_monhsear_h.htm");
}


/* 转发一个邮件  */
int ncWebMailMonFwOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caFile[64],*p;
    char caTempFile[256],caVar[32];
    char caSmail[64];
    char caSub[256];
    char caSub1[256];
    char caCont[256];
    char pFfile[128];
    char caType[16];
    int iSum,i,iReturn;
    unsigned long lSid;
utMsgPrintMsg(psMsgHead);   	
    iReturn = utMsgGetSomeNVar(psMsgHead,4,
                    "msgfile",        UT_TYPE_STRING, 63, caFile,
                    "smail",      UT_TYPE_STRING, 63, caSmail,
                    "sub",        UT_TYPE_STRING, 63, caSub,
                    "msgtype",      UT_TYPE_STRING, 15, caType);
    if(!utStrIsSpaces(caSmail)) {
  	       sprintf(caTempFile,"%s/%s/%s",utComGetVar_sd(psShmHead,"MailDir","."),caType,caFile);
           sprintf(caSub1,"Fw:%s",caSub);
           strcpy(caCont,ncLang("0847这是通过网络督察转发的邮件。\n"));
           sprintf(pFfile,"%s/tmp/%lu",utComGetVar_sd(psShmHead,"MailDir","."),utStrGetId());
utSysLog(" Forward: %s",caTempFile);
           iReturn = mail_digest(caTempFile,pFfile,"ProEIM",caSmail,
                                      caSub1,caCont,0);
           iReturn = sendMailConFromFile(pFfile,NULL);
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0788邮件查询"),ncLang("0810邮件已被转发"));
    }
    else {
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0788邮件查询"),ncLang("0795邮件地址不能为空"));
    }        
}



/* 显示 Upload清单 */

int ncWebUploadList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char caMac[20];
    char caWhere[1024],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caUserid[16],caGroupid[17];
    char caUsername[32],caTo[64],caFrom[64],caSub[128],caSdate[32];
    char caFile[128],caTemp[2048],caFlags[16],caType[64],caSear[16];
    char caSdate1[32],caEdate1[32];
    char caSdate2[20],caEdate2[20];
    char caCount[16];
    char caExport[12];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lAtt,lStat,lSize,lUserid0,lSid;
    int i,iNum;
    ncUserCont *psUser;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lTime=0;
   	long lTime1=0;
   	unsigned long lcurtime;
   	long l=0;
   	char caUrl[64];
   	char caHost[64];
   	char caAsize[12];
   	char caEdate[20];
   	char caOrder[12];
   	char caDdd[12];
   	char caUnit[12],caBef[12],caOrderby[12];
   	char caDtype[12];
   	char caConflag[12];
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount;
   	char caYsf[10],caMsize[12];
   	char caTitlename[32];
   	char caIdfyno[20];
   	char caContent[256],caAttfile[130];
   	char caIp[20];
   	char ids[256];
   	char caSip[16];
   	char caPosition[32];
   	char caUsername_match[8],caIdfyno_match[8],caIp_match[8],caMac_match[8];
   	int i2,lUserid2;
 	pasDbCursor *psCur;
 	char *pBuf,*pSelect;
 	char *pStr;
unsigned long lpsCount,lptCount,ltCount;
 utMsgDelVar(psMsgHead,"action");
 utMsgAdd_s(&psMsgHead,"action","06");
 ncOperWriteLog(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,16,
	                "caWhere",    UT_TYPE_STRING, 500,caWhere,
	                "mtype",      UT_TYPE_STRING, 63, caType,
	                "userid",     UT_TYPE_STRING, 12, caUserid,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
									"CurPg",      UT_TYPE_STRING, 12, caCurPg,
									"search",     UT_TYPE_STRING, 12, caSear,
									"idfyno",UT_TYPE_STRING,18,caIdfyno,
   	                "cond",       UT_TYPE_STRING, 63, caCond,
   	                "TotRec",     UT_TYPE_STRING, 12, caCount,
   	                "groupid",    UT_TYPE_STRING, 16, caGroupid,
   	                "con_flag",   UT_TYPE_STRING,10,caConflag,
   	                "exp",        UT_TYPE_STRING, 10, caExport,
   	                "username_match",UT_TYPE_STRING,2,caUsername_match,
   	              "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
   	              "ip_match",    UT_TYPE_STRING,2,caIp_match,
   	              "mac_match",   UT_TYPE_STRING,2,caMac_match);
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),"未知"));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	                
   	                
    utStrDelSpaces(caWhere);
    sprintf(caCond0,"'%c%s%c'",'%',caCond,'%');
    

    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    if(!utStrIsSpaces(caSear)) {
        lCurPg = 1;
        strcpy(caCount,"\0");
    }
    lUserid0 = atol(caUserid);
    if((lUserid0>0)||(!utStrIsSpaces(caCond))||(!utStrIsSpaces(caGroupid))) strcpy(caWhere,"\0");
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {
        if(!utStrIsSpaces(caCond)) {  
        	
            sprintf(caWhere," and a.groupid = %lu ",lGroupid0);
            if(lUserid0 > 0) {
                utStrAddF(caWhere," and b.userid = %lu ",lUserid0);
            }
            utStrAddF(caWhere," and (b.sdate like %s or b.host like %s or b.url like %s)",
                                      caCond0,caCond0,caCond0);
        }
        else {
        	
            sprintf(caWhere,"  and a.groupid = %lu ",lGroupid0);
            if(lUserid0 > 0) {
                utStrAddF(caWhere," and b.userid = %lu ",lUserid0);
            }
        }
        
    }
   
    else {
        if(!utStrIsSpaces(caCond)) {  
          strcpy(caWhere,"\0");
            if(lUserid0 > 0) {
                utStrAddF(caWhere," and b.userid = %lu ",lUserid0);
            }
            utStrAddF(caWhere," and (b.sdate like %s or b.host like %s or b.url like %s )",
                                      caCond0,caCond0,caCond0);
        }
        else {
            if(lUserid0 > 0) {
                utStrAddF(caWhere," and b.userid = %lu ",lUserid0);
            }
        }
    }
 
    iReturn = utMsgGetSomeNVar(psMsgHead,21,
    				      "mac",           UT_TYPE_STRING,18,caMac,
	                "dispname",      UT_TYPE_STRING, 31, caUsername,
	                "url",   UT_TYPE_STRING, 63, caUrl,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "host",       UT_TYPE_STRING, 63, caHost,
	                "asize",      UT_TYPE_STRING, 11, caAsize,
	                "sdate",      UT_TYPE_STRING, 19, caSdate2,
	                "edate",      UT_TYPE_STRING, 19, caEdate2,
	                "sdate1",     UT_TYPE_STRING, 5,  caSdate1,
	                "edate1",     UT_TYPE_STRING, 5,  caEdate1,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "ddd",        UT_TYPE_STRING, 11,caDdd,
	                "unit",       UT_TYPE_STRING, 11,caUnit,
	                "dtype",      UT_TYPE_STRING, 11,caDtype,
	                "bef",        UT_TYPE_STRING,11,caBef,
	                "ysf",        UT_TYPE_STRING,8,caYsf,
	                "msize",      UT_TYPE_STRING,11,caMsize,
	                "orderby",    UT_TYPE_STRING,11, caOrderby,
	                "ip",         UT_TYPE_STRING,15,caIp,
	                "content",    UT_TYPE_STRING,250,caContent,
	                "attfile",    UT_TYPE_STRING,127,caAttfile);
    
	   strcpy(caSdate,ncMailCvtDate998(caSdate2));
     strcpy(caEdate,ncMailCvtDate998(caEdate2));
     

 
      if(atol(caConflag)>0){
	      lcurtime=time(0);
	      strcpy(caSdate,utTimFormat("%Y-%m-%d %H:%M:%S",lcurtime-86400));
	      strcpy(caEdate,utTimFormat("%Y-%m-%d %H:%M:%S",lcurtime+3600));
	           
	    }  
//printf("caWhere=%s\n",caWhere);              
    if(iReturn>0)
    {
    	strcpy(caWhere,"\0");
    	if(!utStrIsSpaces(caGroupid)) {
    		utStrAddF(caWhere," and a.groupid =%s ",caGroupid);
    	}
    	if(!utStrIsSpaces(caUserid)) {
    		utStrAddF(caWhere," and b.userid =%s ",caUserid);
    	}
    	if(!utStrIsSpaces(caUsername)&&utStrIsSpaces(caUserid)&&(atol(caUsername_match)==1)){
    		utStrAddF(caWhere," and (a.username like '%c%s%c' or a.dispname like '%c%s%c') ",'%',caUsername,'%','%',caUsername,'%');
    	}
    	if(!utStrIsSpaces(caUsername)&&utStrIsSpaces(caUserid)&&(atol(caUsername_match)==0)){
    		utStrAddF(caWhere," and (a.username='%s' or a.dispname='%s') ",caUsername,caUsername);
    	}
    	
    	
    	if(!utStrIsSpaces(caUrl)) {
    		utStrAddF(caWhere," and b.url like '%c%s%c' ",'%',caUrl,'%');
    	}
    	if(!utStrIsSpaces(caHost)) {
    		pStr=strstr(caHost,"http://");
    		
    		if((pStr)&&(strlen(caHost)>7))
    		utStrAddF(caWhere," and b.host like '%c%s%c' ",'%',pStr+7,'%');
    		else
    		utStrAddF(caWhere," and b.host like '%c%s%c' ",'%',caHost,'%');
    		
    
    	}
    	if(!utStrIsSpaces(caAsize))
    	{
	    	if(atoi(caAsize)==1) {
	    		sprintf(caWhere+strlen(caWhere)," and b.asize>0 ");
	    	}
	    	else
	    	{
	    		sprintf(caWhere+strlen(caWhere)," and b.asize<=0 ");
	    	}
    	}
    	 if((strlen(caIdfyno)!=0)&&(atol(caIdfyno_match)==0))
    	    sprintf(caWhere+strlen(caWhere)," and a.idfyno = '%s' ",caIdfyno);
	  
	  if((strlen(caIdfyno)!=0)&&(atol(caIdfyno_match)==1)){
     	sprintf(caWhere+strlen(caWhere)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
    }
    	if(!utStrIsSpaces(caMsize)){
    		if(atol(caYsf)==1)
    		   sprintf(caWhere+strlen(caWhere)," and b.msize>%lu ",atol(caMsize));
    		 else if(atol(caYsf)==2)
    		   sprintf(caWhere+strlen(caWhere)," and b.msize< %lu ",atol(caMsize));
    		  else if(atol(caYsf)==3)
    		   sprintf(caWhere+strlen(caWhere)," and b.msize= %lu ",atol(caMsize));
    		  }
    	
    	if(!utStrIsSpaces(caMac))
    	{
    		if(atol(caMac_match)==0)
    			sprintf(caWhere+strlen(caWhere)," and a.mac='%s' ",caMac);
    		else
    		  sprintf(caWhere+strlen(caWhere)," and a.mac like '%c%s%c' ",'%',caMac,'%');
    	}
    	if(!utStrIsSpaces(caIp))
    	{
    		if(atol(caIp_match)==0)
					sprintf(caWhere+strlen(caWhere)," and b.sip=%lu ",ntohl(utComHostAddress(caIp)));
				else
				  sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caIp,'%');
    }
    
    if(strlen(caContent)>0){
    	sprintf(caWhere+strlen(caWhere)," and b.content like '%c%s%c' ",'%',caContent,'%');
    }
    if(strlen(caAttfile)>0){
    	sprintf(caWhere+strlen(caWhere)," and b.attfile like '%c%s%c' ",'%',caAttfile,'%');
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
	           strcpy(caSdate,utTimFormat("%Y-%m-%d",lTime1));
	            if(caBef[0] == '1') { /* 以内  */
	                utStrAddF(caWhere," and b.sdate >= '%s' ",caSdate);
	               
	            }
	            else { /* 以前  */
	                utStrAddF(caWhere," and b.sdate <= '%s' ",caSdate);
	              
	            }
	        }            
	    }
	    else { /* 时间范围  */
	        if(!utStrIsSpaces(caSdate)) {
	            utStrAddF(caWhere," and b.sdate >= '%s %s' ",caSdate,caSdate1);
	        }
	        if(!utStrIsSpaces(caEdate)) {
	            utStrAddF(caWhere," and b.sdate <= '%s %s' ",caEdate,caEdate1);
	            
	        }
	    }
	    
	    
	    
    	if(!utStrIsSpaces(caOrder)) {
    		if(strcmp(caOrder,"date")==0)
    		{
    			if(atoi(caOrderby)==1)
    			utStrAddF(caWhere," order by b.sdate desc ");
    			else
    			utStrAddF(caWhere," order by b.sdate");
    		}
    		if(strcmp(caOrder,"host")==0)
    		{
    			if(atoi(caOrderby)==1)
    			utStrAddF(caWhere," order by b.host desc ");
    			else
    			utStrAddF(caWhere," order by b.host ");
    		}
    		if(strcmp(caOrder,"url")==0)
    		{
    			if(atoi(caOrderby)==1)
    			utStrAddF(caWhere," order by b.url desc ");
    			else
    			utStrAddF(caWhere," order by b.url ");
    		}
    		if(strcmp(caOrder,"name")==0)
    		{
    			if(atoi(caOrderby)==1)
    			utStrAddF(caWhere," order by b.userid desc ");
    			else
    			utStrAddF(caWhere," order by b.userid ");
    		}
    		if(strcmp(caOrder,"asize")==0)
    		{
    			if(atoi(caOrderby)==1)
    			utStrAddF(caWhere," order by b.msize desc ");
    			else
    			utStrAddF(caWhere," order by b.msize ");
    		}
    		
    	}
    }
    

if((atol(caConflag)>0)&&(strstr(caWhere,"like")==NULL)&&(strstr(caWhere,"b.asize")==NULL)&&(strstr(caWhere,"b.msize")==NULL)&&(strstr(caWhere,"userid")==NULL)&&(strstr(caWhere,"groupid")==NULL))
{
	sprintf(caTemp,"select count(*) from ncformmon ");
	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&ltCount);
	
	sprintf(caTemp,"select formtot,formtoday from ncstattoday ");
	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lpsCount,
	                        UT_TYPE_LONG,4,&lptCount);
	lCount=lptCount+ltCount-lpsCount;
	
	strcpy(caWhere,"\0");

}
else{
   if(strstr(caWhere,"a.")){
   		    sprintf(caTemp,"select count(*) from ncuser a,ncformmon b where a.userid = b.userid %s", caWhere);
   		     if(iDebugLevel==17)
   		     utSysLog("select count(*) from ncuser a,ncformmon b where a.userid = b.userid %s", caWhere);
  		     
	        iReturn = pasDbOneRecord(caTemp,0, UT_TYPE_LONG,4,&lCount);
	        if(iReturn != 0) {
	            utSysLog(" DataBase Count Error %s",pasDbErrorMsg(NULL));
	            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0788邮件查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
	            return 0;
	        }
	   }   	
   else{
    		sprintf(caTemp,"select count(*) from ncformmon b where 1=1 %s",caWhere);   		
    		pasDbOneRecord(caTemp,0, UT_TYPE_LONG,4,&lCount);   		
    	}
  }  	
           
   psDbHead = utPltInitDb(); 
   utPltPutVar(psDbHead,"caWhere",caWhere);
   
   if(!utStrIsSpaces(caExport))
     lRowNum=99999;
     else
     lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;
    
    if(strstr(caWhere,"a.")){
	    sprintf(caTemp,"select inet_ntoa(b.sip),b.sid,b.userid,b.sdate,b.upfile,b.host,b.url,b.asize,b.msize \
	                from ncuser a,ncformmon b where a.userid = b.userid %s",caWhere);
	     if(strstr(caTemp,"order by")==NULL)
	     sprintf(caTemp+strlen(caTemp)," order by b.sdate desc ");
	     
	     sprintf(caTemp+strlen(caTemp)," limit %lu,%lu",lStartRec,lRowNum);
	     
//	     printf("caTemp=%s\n",caTemp);
	     psCur = pasDbOpenSql(caTemp,0);
    	
      }
      else{  
      	sprintf(caTemp,"select inet_ntoa(b.sip),b.sid,b.userid,b.sdate,b.upfile,b.host,b.url,b.asize,b.msize \
                from ncformmon b where 1=1 %s ",caWhere);
         	     if(strstr(caTemp,"order by")==NULL)
	     sprintf(caTemp+strlen(caTemp)," order by b.sdate desc ");
	     
	     sprintf(caTemp+strlen(caTemp)," limit %lu,%lu",lStartRec,lRowNum);	     
	//     printf("caTemp=%s\n",caTemp);
	     psCur = pasDbOpenSql(caTemp,0);
      }
   #ifdef LDEBUG
	  printf("caTemp=%s\n",caTemp);
	#endif 
    if(psCur == NULL) {
        utPltFreeDb(psDbHead);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0788邮件查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    } 
           
    iReturn = 0;
    
     if(!utStrIsSpaces(caExport)) {
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        char fj[10];
        sprintf(caFilename,"ncwarn%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
    
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0194导出"),ncLang("0183打开文件出错"));
            return 0;
        }
        ncUtlPrintCsv(fp,1,UT_TYPE_STRING,ncLang("0593外发表单"));
        ncUtlPrintCsv(fp,5,UT_TYPE_STRING,ncLang("0694姓名"),
                       UT_TYPE_STRING,ncLang("0618网址"),
                       UT_TYPE_STRING,ncLang("0521时间"),
                       UT_TYPE_STRING,ncLang("0186大小"),
                       UT_TYPE_STRING,ncLang("0256附件"));

       memset(caSip,0,sizeof(caSip));
        iReturn = pasDbFetchInto(psCur,
        											UT_TYPE_STRING,15,caSip,
      	                      UT_TYPE_LONG,4,&lSid,
      	                      UT_TYPE_LONG,4,&lUserid,
  	                           UT_TYPE_STRING,23,caSdate,
  	                           UT_TYPE_STRING,63,caFile,
  	                           UT_TYPE_STRING,63,caTo,
  	                           UT_TYPE_STRING,63,caSub,
  	                           UT_TYPE_LONG,4,&lAtt,
  	                           UT_TYPE_LONG,4,&lSize);
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
        	
        	ncUtlPrintCsv(fp,5,UT_TYPE_STRING,caUsername,
                       UT_TYPE_STRING,caSub,
                       UT_TYPE_STRING,caSdate,
                       UT_TYPE_LONG,lSize,
                       UT_TYPE_STRING,fj); 
      memset(caSip,0,sizeof(caSip));      
  		iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                           UT_TYPE_STRING,23,caSdate,
  	                           UT_TYPE_STRING,63,caFile,
  	                           UT_TYPE_STRING,63,caTo,
  	                           UT_TYPE_STRING,63,caSub,
  	                           UT_TYPE_LONG,4,&lAtt,
  	                           UT_TYPE_LONG,4,&lSize);
  	}
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        return 0;
    }
    
    memset(caSip,0,sizeof(caSip));
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,23,caSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,63,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize);
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum&iNum<lCount) {
        iNum++;
        psUser = ncUtlGetUserCont(psShmHead,lUserid);
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        
        utPltSetCvtHtml(1);
        if(psUser){
        utPltPutLoopVar(psDbHead,"username", iNum,psUser->dispname);
        utPltPutLoopVar(psDbHead,"position", iNum,psUser->position);
      }
        utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVar(psDbHead,"file", iNum,caFile);
        utPltPutLoopVar(psDbHead,"host", iNum,caTo);
        utPltPutLoopVar(psDbHead,"url", iNum,caSub);
        utPltSetCvtHtml(0);
        utPltPutLoopVarF(psDbHead,"size", iNum,"%lu",lSize);
        if(lAtt > 0) {
            utPltPutLoopVar(psDbHead,"att", iNum,ncLang("0815有"));
        }
         memset(caSip,0,sizeof(caSip));
      	iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_STRING,15,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,23,caSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,63,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum + 1;
    i=0;
    while(iNum < lRowNum ) {
        iNum++;
        i++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"cond",caCond);
    utPltPutVar(psDbHead,"userid",caUserid);
  
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    
     utPltPutVar(psDbHead,"title_name",caTitlename);
    
    if(atol(caConflag)==1)
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/nc_uploadonlinelist.htm");
    else
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_upload_mlist.htm");
    return 0;
}



/* 显示 Upload清单(支持两级部门 */

int ncWebUploadList2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char caMac[20];
    char caWhere[1024],caCond[64],caCond0[64];
    char caTotPg[16],caCurPg[16],caUserid[16],caGroupid[17];
    char caUsername[32],caTo[64],caFrom[64],caSub[128],caSdate[32];
    char caFile[128],caTemp[2048],caFlags[16],caType[64],caSear[16];
    char caSdate1[32],caEdate1[32];
    char caSdate2[20],caEdate2[20];
    char caCount[16];
    char caExport[12];
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lAtt,lStat,lSize,lUserid0,lSid;
    int i,iNum;
    ncUserCont *psUser;
    utPltDbHead *psDbHead;
   	int iReturn;
   	long lTime=0;
   	long lTime1=0;
   	unsigned long lcurtime;
   	long l=0;
   	char caUrl[64];
   	char caHost[64];
   	char caAsize[12];
   	char caEdate[20];
   	char caOrder[12];
   	char caDdd[12];
   	char caUnit[12],caBef[12],caOrderby[12];
   	char caDtype[12];
   	char caConflag[12];
   	long lGroupid0,lRowNum,lflags,lStartRec,lCount;
   	char caYsf[10],caMsize[12];
   	char caTitlename[32];
   	char caIdfyno[20];
   	char caContent[256],caAttfile[130];
   	char caIp[20];
   	char ids[256];
   	char caSip[16],caDip[16];
   	char caPosition[32];
   	char caUsername_match[8],caIdfyno_match[8],caIp_match[8],caMac_match[8];
   	int i2,lUserid2;
   	char caLabname[16];
   	char caContent_d[1024];
 	pasDbCursor *psCur;
 	char *pBuf,*pSelect;
 	char *pStr;
 	char caPlate[256];
 	 	long lMbflag;
    char caKey[1024],caMname[32]; 
    char cbcode[1024],caSerno[32];
    char caOk[16],caXzmb[16];
    char szBuf[1024];
    long lId;
 	
unsigned long lpsCount,lptCount,ltCount,group_flag;
 utMsgDelVar(psMsgHead,"action");
 utMsgAdd_s(&psMsgHead,"action","06");
 ncOperWriteLog(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,13,
	                "caWhere",    UT_TYPE_STRING, 500,caWhere,
	                "mtype",      UT_TYPE_STRING, 63, caType,
	                "userid",     UT_TYPE_STRING, 12, caUserid,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
									"CurPg",      UT_TYPE_STRING, 12, caCurPg,
									"search",     UT_TYPE_STRING, 12, caSear,
									"idfyno",UT_TYPE_STRING,18,caIdfyno,
   	                "cond",       UT_TYPE_STRING, 63, caCond,
   	                "TotRec",     UT_TYPE_STRING, 12, caCount,
   	                "groupid",    UT_TYPE_STRING, 16, caGroupid,
   	                "con_flag",   UT_TYPE_STRING,10,caConflag,
   	                "exp",        UT_TYPE_STRING, 10, caExport,
   	                "labname",    UT_TYPE_STRING,8,caLabname);



       lMbflag=0;
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
					utPltPutVar(psDbHead,"fname","ncWebUploadList2");
					utPltPutVar(psDbHead,"plate",caPlate);
   	      utPltPutVar(psDbHead,"con_flag",caConflag);
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
			   }
			   else{
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","校正成功");
					utPltPutVar(psDbHead,"fname","ncWebUploadList2");
					utPltPutVar(psDbHead,"plate",caPlate);
   	      utPltPutVar(psDbHead,"con_flag",caConflag);
			   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
			   	return 0;
   				}
				}
				else{
				
			   iReturn=ncMBAuth(psShmHead,caKey,caMname,caSerno,1);

			   if(iReturn<1){
			   	psDbHead = utPltInitDb();
			   	utPltPutVar(psDbHead,"mesg","验证失败");
					utPltPutVar(psDbHead,"fname","ncWebUploadList2");
					utPltPutVar(psDbHead,"plate",caPlate);
   	      utPltPutVar(psDbHead,"con_flag",caConflag);
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
					   	utPltPutVar(psDbHead,"fname","ncWebUploadList2");
   	          utPltPutVar(psDbHead,"con_flag",caConflag);
   	          utPltPutVar(psDbHead,"plate",caPlate);
   	          if(atol(caConflag)>0)
      					utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauthonline.htm");
      				else
   	          	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ds/ds_mbauth.htm");
   	          return 0;
				}                       
    }



   	              
   	    iReturn = utMsgGetSomeNVar(psMsgHead,26,
   	             "dip", UT_TYPE_STRING,15,caDip,
    				      "idfyno",           UT_TYPE_STRING,18,caIdfyno,
	                "dispname",      UT_TYPE_STRING, 31, caUsername,
	                "url",   UT_TYPE_STRING, 63, caUrl,
	                "groupid",    UT_TYPE_STRING, 11, caGroupid,
	                "host",       UT_TYPE_STRING, 63, caHost,
	                "asize",      UT_TYPE_STRING, 11, caAsize,
	                "sdate",      UT_TYPE_STRING, 19, caSdate2,
	                "edate",      UT_TYPE_STRING, 19, caEdate2,
	                "sdate1",     UT_TYPE_STRING, 5,  caSdate1,
	                "edate1",     UT_TYPE_STRING, 5,  caEdate1,
	                "osort",      UT_TYPE_STRING, 11, caOrder,
	                "ddd",        UT_TYPE_STRING, 11,caDdd,
	                "unit",       UT_TYPE_STRING, 11,caUnit,
	                "dtype",      UT_TYPE_STRING, 11,caDtype,
	                "bef",        UT_TYPE_STRING,11,caBef,
	                "ysf",        UT_TYPE_STRING,8,caYsf,
	                "msize",      UT_TYPE_STRING,11,caMsize,
	                "orderby",    UT_TYPE_STRING,11, caOrderby,
	                "sip",         UT_TYPE_STRING,15,caIp,
	                "content",    UT_TYPE_STRING,250,caContent,
	                "attfile",    UT_TYPE_STRING,127,caAttfile,
	                "username_match",UT_TYPE_STRING,2,caUsername_match,
   	              "idfyno_match",UT_TYPE_STRING,2,caIdfyno_match,
   	              "ip_match",    UT_TYPE_STRING,2,caIp_match,
   	              "mac_match",   UT_TYPE_STRING,2,caMac_match);
             	              
   	              
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
   	                
    utStrDelSpaces(caWhere);
 
    lCurPg = atol(caCurPg);
    if(lCurPg <= 0) lCurPg = 1;
    if(!utStrIsSpaces(caSear)) {
        lCurPg = 1;
        strcpy(caCount,"\0");
    }
    lUserid0 = atol(caUserid);
    if((lUserid0>0)||(!utStrIsSpaces(caCond))||(!utStrIsSpaces(caGroupid))) strcpy(caWhere,"\0");
    
    
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {
    	group_flag=1;    
    }
    if(strlen(caUrl)>0||group_flag==1||strlen(caGroupid)>0||strlen(caUserid)>0||strlen(caHost)>0||strlen(caAsize)>0||strlen(caIp)>0||strlen(caContent)>0||strlen(caAttfile)>0||strlen(caUrl)>0||atol(caConflag)>0){
    	if(strlen(caSdate2)==0){
    		strcpy(caSdate2,utTimFormat("%Y-%m-%d",time(0)));
    	}
    	if(strlen(caEdate2)==0){
    		strcpy(caEdate2,utTimFormat("%Y-%m-%d",time(0)));
    	}
    	if(strlen(caSdate1)==0){
    		strcpy(caSdate1,"00:00");
    	}
    	if(strlen(caEdate1)==0){
    		strcpy(caEdate1,"23:59");
    	}
    }
  
   	 strcpy(caSdate,ncMailCvtDate998(caSdate2));
     strcpy(caEdate,ncMailCvtDate998(caEdate2)); 

   if(strlen(caLabname)>0){
   	 if(strlen(caWhere)==0){
		   	if(group_flag==1||strlen(caUserid)>0||strlen(caGroupid)>0||atol(caConflag)>0){
		   		sprintf(caWhere+strlen(caWhere)," and b.sdate >= '%s %s' ",caSdate,caSdate1);
		   		sprintf(caWhere+strlen(caWhere)," and b.sdate <= '%s %s' ",caEdate,caEdate1);
		  	}
		  	if(lUserid0>0){
		  		utStrAddF(caWhere," and b.userid = %lu ",lUserid0);
		  	}
		  	
		  	  
		    if(group_flag==1){
		    	sprintf(caWhere+strlen(caWhere)," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu ))",lGroupid0,lGroupid0);
		    }
		    if(strlen(caGroupid)>0){
		    	  if(strcmp(caGroupid,"0")==0)
        	     utStrAddF(caWhere," and (a.groupid = %s ) ",caGroupid);
        	  else
		    			sprintf(caWhere+strlen(caWhere)," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu ))",atol(caGroupid),atol(caGroupid));
		    }
	  	}
	  }   
   else{
   			strcpy(caWhere,"\0");
   		   if(!utStrIsSpaces(caSdate)) {
	            utStrAddF(caWhere," and b.sdate >= '%s %s' ",caSdate,caSdate1);
	        }
	        if(!utStrIsSpaces(caEdate)) {
	            utStrAddF(caWhere," and b.sdate <= '%s %s' ",caEdate,caEdate1);
	            
	        }
	        
	      if(atol(caUserid)>0) {
    			utStrAddF(caWhere," and b.userid =%s ",caUserid);
	    	}
	    	if(!utStrIsSpaces(caUsername)&&utStrIsSpaces(caUserid)&&(atol(caUsername_match)==1)){
	    		utStrAddF(caWhere," and (a.username like '%c%s%c' or a.dispname like '%c%s%c') ",'%',caUsername,'%','%',caUsername,'%');
	    	}
	    	if(!utStrIsSpaces(caUsername)&&utStrIsSpaces(caUserid)&&(atol(caUsername_match)==0)){
	    		utStrAddF(caWhere," and (a.username='%s' or a.dispname='%s') ",caUsername,caUsername);
	    	}
	        
	        if(group_flag==1){
		    	sprintf(caWhere+strlen(caWhere)," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu ))",lGroupid0,lGroupid0);
		    }
		    if(strlen(caGroupid)>0){
		    	sprintf(caWhere+strlen(caWhere)," and (a.groupid=%lu or a.groupid in (select groupid from ncgroup where pid=%lu ))",atol(caGroupid),atol(caGroupid));
		    }
	      if(!utStrIsSpaces(caUrl)) {
	    		utStrAddF(caWhere," and b.url like '%c%s%c' ",'%',caUrl,'%');
	    	}
	    	if(!utStrIsSpaces(caHost)) {
	    		pStr=strstr(caHost,"http://");
	    		
	    		if((pStr)&&(strlen(caHost)>7))
	    		utStrAddF(caWhere," and b.host like '%c%s%c' ",'%',pStr+7,'%');
	    		else
	    		utStrAddF(caWhere," and b.host like '%c%s%c' ",'%',caHost,'%');
	    		
	    
	    	}
	    	if(!utStrIsSpaces(caAsize))
	    	{
		    	if(atoi(caAsize)==1) {
		    		sprintf(caWhere+strlen(caWhere)," and b.asize>0 ");
		    	}
		    	else
		    	{
		    		sprintf(caWhere+strlen(caWhere)," and b.asize<=0 ");
		    	}
	    	}
	    	 if((strlen(caIdfyno)!=0)&&(atol(caIdfyno_match)==0))
	    	    sprintf(caWhere+strlen(caWhere)," and a.idfyno = '%s' ",caIdfyno);
		  
		  if((strlen(caIdfyno)!=0)&&(atol(caIdfyno_match)==1)){
	     	sprintf(caWhere+strlen(caWhere)," and a.idfyno like '%c%s%c' ",'%',caIdfyno,'%');
	    }
	    	if(!utStrIsSpaces(caMsize)){
	    		if(atol(caYsf)==1)
	    		   sprintf(caWhere+strlen(caWhere)," and b.msize>%lu ",atol(caMsize));
	    		 else if(atol(caYsf)==2)
	    		   sprintf(caWhere+strlen(caWhere)," and b.msize< %lu ",atol(caMsize));
	    		  else if(atol(caYsf)==3)
	    		   sprintf(caWhere+strlen(caWhere)," and b.msize= %lu ",atol(caMsize));
	    		  }
	    	
	    	if(!utStrIsSpaces(caIp))
	    	{
	    		if(atol(caIp_match)==0)
						sprintf(caWhere+strlen(caWhere)," and b.sip=%lu ",ntohl(utComHostAddress(caIp)));
					else
					  sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.sip) like '%c%s%c' ",'%',caIp,'%');
	    }
	    if(!utStrIsSpaces(caDip))
	    	{
	    		
					  sprintf(caWhere+strlen(caWhere)," and inet_ntoa(b.dip) like '%c%s%c' ",'%',caDip,'%');
	    }
	    
	    
	    if(strlen(caContent)>0){
	    	strcpy(caContent_d,pasCharsetCvtP(PAS_CCODE_GB,PAS_CCODE_UTF8,caContent)); 
	    	sprintf(caWhere+strlen(caWhere)," and (b.content like '%c%s%c' or b.content like '%c%s%c') ",'%',caContent,'%','%',caContent_d,'%');
	    }
	    if(strlen(caAttfile)>0){
	    	sprintf(caWhere+strlen(caWhere)," and b.attfile like '%c%s%c' ",'%',caAttfile,'%');
	    }
	   
	        
   	
   }
//   if(strstr(caWhere,"order by")==NULL)
//   utStrAddF(caWhere," order by b.sdate desc ");
printf("caWhere=%s\n",caWhere);              


   if(strstr(caWhere,"a.")){
   		    sprintf(caTemp,"select count(*) from ncuser a,ncformmon b where a.userid = b.userid %s", caWhere);
   		     if(iDebugLevel==17)
   		     utSysLog("select count(*) from ncuser a,ncformmon b where a.userid = b.userid %s", caWhere);
  		     
	        iReturn = pasDbOneRecord(caTemp,0, UT_TYPE_LONG,4,&lCount);
	        if(iReturn != 0) {
	            utSysLog(" DataBase Count Error %s",pasDbErrorMsg(NULL));
	            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0788邮件查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
	            return 0;
	        }
	   }   	
   else{
    		sprintf(caTemp,"select count(*) from ncformmon b where 1=1 %s",caWhere);   		
    		pasDbOneRecord(caTemp,0, UT_TYPE_LONG,4,&lCount);   		
    	}
     
   psDbHead = utPltInitDb(); 
   
  if(strlen(caSdate2)==0){
    strcpy(caSdate2,utTimFormat("%Y/%m/%d",time(0)));
  }
  if(strlen(caEdate2)==0){
    strcpy(caEdate2,utTimFormat("%Y/%m/%d",time(0)));
  }
   if(strlen(caSdate1)==0){
   	strcpy(caSdate1,"00:00");
  }
  if(strlen(caEdate1)==0){
  	strcpy(caEdate1,"23:59");
  }
   utPltPutVar(psDbHead,"caWhere",caWhere);
   utPltPutVar(psDbHead,"groupid",caGroupid);
   utPltPutVar(psDbHead,"host",caHost);
   utPltPutVar(psDbHead,"dispname",caUsername);
   utPltPutVar(psDbHead,"asize",caAsize);
   utPltPutVar(psDbHead,"sdate",caSdate2);
   utPltPutVar(psDbHead,"edate",caEdate2);
   utPltPutVar(psDbHead,"sdate1",caSdate1);
   utPltPutVar(psDbHead,"edate1",caEdate1);
   utPltPutVar(psDbHead,"msize",caMsize);
   utPltPutVar(psDbHead,"ip",caIp);
   utPltPutVar(psDbHead,"content",caContent);
   utPltPutVar(psDbHead,"attfile",caAttfile);
   utPltPutVar(psDbHead,"username_match",caUsername_match);
   utPltPutVar(psDbHead,"idfyno",caIdfyno);
   utPltPutVar(psDbHead,"dip",caDip);
   utPltPutVar(psDbHead,"url",caUrl);
   utPltPutVar(psDbHead,"osort",caOrder);
   
   if(!utStrIsSpaces(caExport)){
     lRowNum=99999;
     lCurPg=1;
    }
     else
     lRowNum = 32;
    lStartRec = (lCurPg - 1) * lRowNum;
    
    if(strstr(caWhere,"a.")){
	    sprintf(caTemp,"select inet_ntoa(b.sip),b.sid,b.userid,b.sdate,b.upfile,b.host,b.url,b.asize,b.msize,inet_ntoa(b.dip) \
	                from ncuser a,ncformmon b where a.userid = b.userid %s",caWhere);
	                
	                
	      if(!utStrIsSpaces(caOrder)) {
	        if(strcmp(caOrder,"date")==0) {
	            sprintf(caTemp+strlen(caTemp)," order by b.sdate ");
	        }        
		      else if(strcmp(caOrder,"name")==0){
		      	sprintf(caTemp+strlen(caTemp)," order by b.userid ");
		      }
	     }          
	     if(strstr(caTemp,"order by")==NULL)
	     sprintf(caTemp+strlen(caTemp)," order by b.sdate desc ");
	     
	     
	     
	     sprintf(caTemp+strlen(caTemp)," limit %lu,%lu",lStartRec,lRowNum);
	     
	     printf("caTemp=%s\n",caTemp);
	     psCur = pasDbOpenSql(caTemp,0);
    	
      }
      else{  
      	sprintf(caTemp,"select inet_ntoa(b.sip),b.sid,b.userid,b.sdate,b.upfile,b.host,b.url,b.asize,b.msize,inet_ntoa(b.dip) \
                from ncformmon b where 1=1 %s ",caWhere);
                
        if(!utStrIsSpaces(caOrder)) {
	        if(strcmp(caOrder,"date")==0) {
	            sprintf(caTemp+strlen(caTemp)," order by b.sdate ");
	        }        
		      else if(strcmp(caOrder,"name")==0){
		      	sprintf(caTemp+strlen(caTemp)," order by b.userid ");
		      }
	     }       
                
        if(strstr(caTemp,"order by")==NULL)
	     sprintf(caTemp+strlen(caTemp)," order by b.sdate desc ");
	     
	     sprintf(caTemp+strlen(caTemp)," limit %lu,%lu",lStartRec,lRowNum);	     
//     printf("caTemp=%s\n",caTemp);
	     psCur = pasDbOpenSql(caTemp,0);
      }
   #ifdef LDEBUG
//	  printf("caTemp=%s\n",caTemp);
	#endif 
    if(psCur == NULL) {
        utPltFreeDb(psDbHead);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0788邮件查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    } 
           
    iReturn = 0;
    
     if(!utStrIsSpaces(caExport)) {
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        char fj[10];
        sprintf(caFilename,"ncwarn%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
    
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0194导出"),ncLang("0183打开文件出错"));
            return 0;
        }
        ncUtlPrintCsv(fp,1,UT_TYPE_STRING,ncLang("0593外发表单"));
        ncUtlPrintCsv(fp,6,UT_TYPE_STRING,ncLang("0694姓名"),
                       UT_TYPE_STRING,ncLang("0820源IP"),
                       UT_TYPE_STRING,ncLang("0618网址"),
                       UT_TYPE_STRING,ncLang("0521时间"),
                       UT_TYPE_STRING,ncLang("0186大小"),
                       UT_TYPE_STRING,ncLang("0256附件"));

       memset(caSip,0,sizeof(caSip));
       memset(caDip,0,sizeof(caDip));
        iReturn = pasDbFetchInto(psCur,
        											UT_TYPE_STRING,15,caSip,
      	                      UT_TYPE_LONG,4,&lSid,
      	                      UT_TYPE_LONG,4,&lUserid,
  	                           UT_TYPE_STRING,23,caSdate,
  	                           UT_TYPE_STRING,63,caFile,
  	                           UT_TYPE_STRING,63,caTo,
  	                           UT_TYPE_STRING,63,caSub,
  	                           UT_TYPE_LONG,4,&lAtt,
  	                           UT_TYPE_LONG,4,&lSize,
  	                           UT_TYPE_STRING,15,caDip);
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
       sprintf(caTemp,"%s%s",caTo,caSub);	
        	ncUtlPrintCsv(fp,6,UT_TYPE_STRING,caUsername,
        	             UT_TYPE_STRING,caSip,
                       UT_TYPE_STRING,caTemp,
                       UT_TYPE_STRING,caSdate,
                       UT_TYPE_LONG,lSize,
                       UT_TYPE_STRING,fj); 
      memset(caSip,0,sizeof(caSip)); 
      memset(caDip,0,sizeof(caDip));     
  		iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                           UT_TYPE_STRING,23,caSdate,
  	                           UT_TYPE_STRING,63,caFile,
  	                           UT_TYPE_STRING,63,caTo,
  	                           UT_TYPE_STRING,63,caSub,
  	                           UT_TYPE_LONG,4,&lAtt,
  	                           UT_TYPE_LONG,4,&lSize,
  	                           UT_TYPE_STRING,15,caDip);
  	}
        
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
           char caUsername2[32];
			   memset(caUsername2,0,sizeof(caUsername2));
			   dsCltGetSessionValue(1,"dispname",UT_TYPE_STRING,30,caUsername2);
			   ncsWriteSysLog(psMsgHead,caUsername2,"06",ncLang("1408成功"),"导出表单日志");
 
        
        return 0;
    }
    
    memset(caSip,0,sizeof(caSip));
    memset(caDip,0,sizeof(caDip));
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,23,caSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,63,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_STRING,15,caDip);
   	iNum = 0;
   	while(iReturn == 0 && iNum < lRowNum&iNum<lCount) {
        iNum++;
        psUser = ncUtlGetUserCont(psShmHead,lUserid);
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        
        utPltSetCvtHtml(1);
        if(psUser){
        utPltPutLoopVar(psDbHead,"username", iNum,psUser->dispname);
        utPltPutLoopVar(psDbHead,"position", iNum,psUser->position);
      }
        utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVar(psDbHead,"file", iNum,caFile);
        utPltPutLoopVar(psDbHead,"host", iNum,caTo);
        utPltPutLoopVar(psDbHead,"url", iNum,caSub);
        utPltPutLoopVar(psDbHead,"dip",iNum,caDip);
        utPltSetCvtHtml(0);
        utPltPutLoopVarF(psDbHead,"size", iNum,"%lu",lSize);
        if(lAtt > 0) {
            utPltPutLoopVar(psDbHead,"att", iNum,ncLang("0815有"));
        }
    memset(caSip,0,sizeof(caSip));
    memset(caDip,0,sizeof(caDip));
   	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,23,caSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,63,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_STRING,15,caDip);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum + 1;
    i=0;
    while(iNum < lRowNum ) {
        iNum++;
        i++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"cond",caCond);
    utPltPutVar(psDbHead,"userid",caUserid);
  
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    
     utPltPutVar(psDbHead,"title_name",caTitlename);
    
    if(atol(caConflag)==1)
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/nc_uploadonlinelist.htm");
    else
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_upload_mlist.htm");
    return 0;
}



/* 读一封备份的邮件  */
int ncWebUploadRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncPostDataContent *psContent;
    char      *pTempDir,*pMsgFile;
    char      caTempDir[128],caFile[128],caFile2[128],caFile1[128];
    char      caMsgFile[128],caUserid[16];
    int i,iNum,iFlags,iAttFlags,fd;
    utPltDbHead *psDbHead;
   	int iReturn;
//utMsgPrintMsg(psMsgHead);   	
    iReturn = utMsgGetSomeNVar(psMsgHead,2,
                    "userid",      UT_TYPE_STRING, 12, caUserid,
	                "msgfile",     UT_TYPE_STRING, 63, caMsgFile);
    if(utStrIsSpaces(caMsgFile)) { /* 邮件无内容  */
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0785邮件"),ncLang("0655无内容"));
        return 0;
    }
    
 	/* 读邮件头信息  */
 	sprintf(caFile2,"%s/frm/%s",utComGetVar_sd(psShmHead,"MailDir","."),caMsgFile);
    sprintf(caTempDir,"%s/tmp/%08d",utComGetVar_sd(psShmHead,"MailDir","."),utStrGetId());
    if(utFileIsExist(caTempDir)!=1) {
        if(mkdir(caTempDir,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)!=0) {
            utSysLog(" Dir Mkerror %s",caTempDir);
            return 0;
        }
    }
    
    psContent = ncReadPostData(caFile2,caTempDir);
    if(psContent == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0636文件"),ncLang("0654无具体数据"));
        return 0;
    }
    else {
        psDbHead=utPltInitDbHead();
        utPltPutVar(psDbHead,"username",psContent->caName);
        utPltPutVar(psDbHead,"hostname",psContent->caHost);
        utPltPutVar(psDbHead,"date", utTimFormat("%Y-%m-%d %H:%M:%S",psContent->lTime));
        if(psContent->pValue) {
            utPltPutVar(psDbHead,"content",psContent->pValue);
        }        
        if(psContent->psFileList) {
            int iNum=0;
            ncPosFileList *psList;
            psList = psContent->psFileList;
            while(psList) {
                iNum++;
                utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum);
                utPltPutLoopVar(psDbHead,"userid",iNum,caUserid);
                utPltPutLoopVar(psDbHead,"ofile",iNum,psList->pOriginFile);
                for(i=strlen(psList->pOriginFile)-1;
                   psList->pOriginFile[i]!=92 && psList->pOriginFile[i]!="/" && i>=0;i--);
                utPltPutLoopVar(psDbHead,"disp",iNum,psList->pOriginFile + i + 1);
                utPltPutLoopVar(psDbHead,"type",iNum,psList->pType);
                utPltPutLoopVarF(psDbHead,"bytes",iNum,"%lu",psList->lSize);
                utPltPutLoopVar(psDbHead,"htmldir",iNum,caTempDir);
                utPltPutLoopVar(psDbHead,"outfile",iNum,psList->pOutFile);
                psList = psList->psNext;
            }
        }
        ncPostDataFree(psContent);
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_upload_content.htm");
    	return 0;
 	}
    return 0;
}
    



int ncMailFrmCheck(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncUserCont  *psUser;
    ncPostHead  *psHead;
    pasFile     *ps;
    ncPostCont *psCont;
    char       *pMailDir;
    char      caTempDir[128],caFile[128],caDfile[128],caSfile[128];
    char      caMsgFile[128],caUserid[16];
    char      caTime[32],caDispname[32],caGroupname[32];
    int i,iNum,i1,i2,l,iCode=0,iCode1;
    unsigned long lUserid,lTime;
    utPltDbHead *psDbHead;
   	int iReturn;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif   	
    iReturn = utMsgGetSomeNVar(psMsgHead,3,
                    "userid",      UT_TYPE_STRING, 12, caUserid,
	                "fname",     UT_TYPE_STRING, 63, caMsgFile,
	                "time",        UT_TYPE_STRING, 20, caTime);
    if(utStrIsSpaces(caMsgFile)) { /* 邮件无内容  */
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0127表单"),ncLang("0655无内容"));
        return 0;
    }
    lUserid = atol(caUserid);
 	/* 读邮件头信息  */
 	pMailDir = utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog");
 	sprintf(caFile,  "%s/frm/%s",pMailDir,caMsgFile);
    sprintf(caTempDir,"%s/tmp/%08d",pMailDir,utStrGetId());
    if(utFileIsExist(caTempDir)!=1) {
        if(mkdir(caTempDir,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)!=0) {
            utSysLog(" Dir Mkerror %s",caTempDir);
            return 0;
        }
    }
    
    ps = pasOpenFile(caFile);
    if(ps == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0127表单"),ncLang("0131表单文件打开出错"));
        return 0;
    }
    l = strlen(caFile);
    if(caFile[l-2] == '-') {
        iCode = atol(&caFile[l-1]);
    }
    else {
        iCode = pasDefaultCode();
    }
    
    psDbHead = utPltInitDb();
    psUser = ncUtlGetUserContById(psShmHead,lUserid);
    if(psUser) {
        strcpy(caDispname,psUser->dispname);
        if(psUser->psGroup) {
            strcpy(caGroupname,psUser->psGroup->groupname);
        }
        else {
            strcpy(caGroupname,"unknow");
        }
    }
    else {
        strcpy(caDispname," ");
        strcpy(caGroupname,"\0");
    }
    utPltPutVar(psDbHead,"username",caDispname);
    utPltPutVar(psDbHead,"groupname",caGroupname);
    utPltPutVar(psDbHead,"date",caTime);
    psHead = ncPostGetPostData(ps);
    iNum = 0;
    while(psHead) {
        iCode1 = ncPostCharsetCode(psHead);
        if(iCode1 >= 0) {
            iCode = iCode1;
        }
        if(iCode != pasDefaultCode()) {
#ifdef LDEBUG1
            printf(" CvtCode %d \n",iCode);
#endif
            ncPostCvtDefaultCode(psHead,iCode);
        }
        
        iNum++;
        utPltPutLoopVar(psDbHead,"host",iNum,psHead->pHost?psHead->pHost:"\0");
        utPltPutLoopVar(psDbHead,"url",iNum,psHead->pUrl?psHead->pUrl:"\0");
        psCont = psHead->psCont;
        i1 = 0;
        i2 = 0;
        while(psCont) {
            if(psCont->lFileId > 0) {  /* 附件  */
                i1++;
                sprintf(caDfile,"%s/%lu",caTempDir,psCont->lFileId);
                link(ncPostFileName(psCont->lFileId,caSfile),caDfile);
                utPltPutLoopVar2(psDbHead,"fvar",iNum,i1,psCont->pVar?psCont->pVar:"\0");
                utPltPutLoopVar2(psDbHead,"fvalue",iNum,i1,psCont->pValue?psCont->pValue:"\0");
                if(psCont->pValue) {
                    for(i=strlen(psCont->pValue)-1;
                       psCont->pValue[i]!=92 && psCont->pValue[i]!='/' && i>=0;i--);
                    utPltPutLoopVar2(psDbHead,"disp",iNum,i1,psCont->pValue + i + 1);
                }
                utPltPutLoopVar2(psDbHead,"ftype", iNum,i1,psCont->pContentType?psCont->pContentType:"\0");
                utPltPutLoopVar2(psDbHead,"fname", iNum,i1,caDfile);
            }
            else {
                if(psCont->pValue && !utStrIsSpaces(psCont->pValue)) {
                    i2++;
                    utPltPutLoopVar2(psDbHead,"vvar",iNum,i2,psCont->pVar?psCont->pVar:"\0");
                    utPltPutLoopVar2(psDbHead,"vvalue",iNum,i2,psCont->pValue?psCont->pValue:"\0");
                }                
            }
            psCont = psCont->next;
        }
        ncPostFreeHttpHead(psHead);
        psHead = ncPostGetPostData(ps);
    }
    pasCloseFile(ps);
   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_postcontent.htm");
   	return 0;
    
    
    
    
    
    
    
    
}



/* 删除邮件  */
int ncWebUploadDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caFile[64],*p;
    char caTempFile[256],caVar[32];
    int iSum,i,iReturn;
    unsigned long lSid;
 	
    for(i=0;i<32;i++) {
        sprintf(caVar,"chk%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            lSid = atol(p);
            iReturn = pasDbOneRecord("select upfile from ncformmon where sid = :sid",1,
                           "sid",UT_TYPE_LONG,lSid,
                           UT_TYPE_STRING,63,caFile);
            if(iReturn == 0) {
   	            sprintf(caTempFile,"%s/%s",utComGetVar_sd(psShmHead,"MailDir","."),caFile);
   	            remove(caTempFile);
                iReturn = pasDbExecSqlF("delete from ncformmon where sid = %lu ",lSid);
                if(iReturn == 0) {
                    iReturn = pasDbExecSqlF("delete from ncformmoni where mid = %lu ",lSid);
                    pasDbCommit(NULL);
                }
            }
        }
    }
    return ncWebUploadList(psShmHead, iFd,psMsgHead);
}





                      
/* 显示条件输入画面  */
int ncUtlMailMonDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead,char *pPlate)
{
    long iReturn,lGroupid,lGroupid0;
    char caSmt[256];
    char caType[64],caFlags[12];
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect;
    char caGroupid[12];
    unsigned long userid;
    char caDispname[64];
    pasDbCursor *psCur;
    char caSdate[24],caEdate[24],caSdate1[24],caEdate1[24];
    char caUsername[32];
    ncUserContHead  *psHead;
    ncUserCont      *psUser,*psUser0;
    int i,iNum;
    unsigned long ltime;
  	iReturn = utMsgGetSomeNVar(psMsgHead,2,
	                "username",UT_TYPE_STRING, 31, caUsername,
	                "groupid",UT_TYPE_STRING,31,caGroupid);
    psDbHead = utPltInitDb();
    
    ltime=time(0);            
	  ncUtlMonthDate(ltime,caSdate,caEdate);               
	  //用户
    psHead = (ncUserContHead *)utShmArray(psShmHead,NC_LNK_ALLUSER);
    if(psHead == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("0403内存不正确"));
         return 0;
    }
    psUser0 = (ncUserCont *)((char *)psHead + psHead->lContPos); 
   
	    for(i=0,iNum=1;i<psHead->lSumUser ;i++) {
      psUser = &psUser0[i];
      if(strlen(caGroupid)>0){
      	if(psUser->groupid==atol(caGroupid)){
		      utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
		      utPltPutLoopVarF(psDbHead,"uid",iNum,"%lu",psUser->userid);
		      utPltPutLoopVarF(psDbHead,"uname",iNum,"%s",psUser->dispname);
		      iNum++;
	      	if(iNum>500) break;
	      }
	    }
	    else{
	    	    utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
      			utPltPutLoopVarF(psDbHead,"uid",iNum,"%lu",psUser->userid);
      			utPltPutLoopVarF(psDbHead,"uname",iNum,"%s",psUser->dispname);
	    	iNum++;
	    	if(iNum>500) break;
	    }
    
  }
  utPltPutVarF(psDbHead,"sum","%lu",iNum);
    
    if(!utStrIsSpaces(caUsername)) {
        utPltPutVar(psDbHead,"username", caUsername);
    }
    strcpy(caGroupid,"\0");
    psCur = pasDbOpenSql("select groupid,groupname from ncgroup   ",0);
    iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&userid,
    	                               UT_TYPE_STRING,63,caDispname);
    iNum = 0; 	                               
    while((iReturn==0)||(iReturn==1405))
    {
  	
        iNum++;       
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%lu",userid);
        utPltPutLoopVar(psDbHead,"dispname",iNum,caDispname);
    iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&userid,
      	                               UT_TYPE_STRING,63,caDispname);
    }
    pasDbCloseCursor(psCur); 
    pBuf = pasUtlCvtSqlSelectA("select distinct rname,rname from mailfilter1",
               ncLang("0455全部,;"));
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("mtype",caType,pBuf);
        free(pBuf);
        utPltPutVar(psDbHead,"smtype", pSelect);
        free(pSelect);
    }
    pSelect= pasUtlCvtInputSelect("flags",caFlags,ncLang("0448全部, ;发送邮件,1;接收邮件,2;WebMail,3;"));
    utPltPutVar(psDbHead,"sflags", pSelect);
    free(pSelect);
    
    utPltPutVar(psDbHead,"sdate",caSdate);
    utPltPutVar(psDbHead,"edate",caEdate);
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,pPlate);
}


/* 删除邮件  */
int ncUtlMailMonDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead,char *pTable)
{
    char caFile[64],*p;
    char caTemp[256];
    char caTempFile[256],caVar[32];
    int iSum,i,iReturn;
    unsigned long lSid;
    for(i=0;i<30;i++) {
        sprintf(caVar,"chk%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            lSid = atol(p);
            sprintf(caTemp,"select mailfile from %s where sid = %lu",
                      pTable,lSid);
            iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_STRING,63,caFile);
            if(iReturn == 0) {
   	            sprintf(caTempFile,"%s/%s",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"),caFile);
   	            remove(caTempFile);
                iReturn = pasDbExecSqlF("delete from %s where sid = %lu ",pTable,lSid);
                if(iReturn == 0) {
                    pasDbExecSqlF("delete from %si where mid = %lu",pTable,lSid);
                    pasDbCommit(NULL);
                }
            }
        }
    }
    return 0;
}


/* 转发选择的邮件  */
int ncUtlMailMonFw(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead,char *pTable,char *pType)
{
    char caTemp[256];
    char caFile[64],*p;
    char caTempFile[256],caVar[32];
    char caSmail[64];
    char caSub[256];
    char caSub1[256];
    char caCont[256];
    char pFfile[128];
    int iSum,i,iReturn;
    unsigned long lSid;

    iReturn = utMsgGetSomeNVar(psMsgHead,1,
                    "smail",      UT_TYPE_STRING, 63, caSmail);
    if(!utStrIsSpaces(caSmail)) {
        for(i=0;i<30;i++) {
            sprintf(caVar,"chk%d",i+1);
            p = utMsgGetVar_s(psMsgHead,caVar);
            if(p) {
                lSid = atol(p);
                sprintf(caTemp,"select mailfile,mailsub from %s where sid = %lu",
                              pTable,lSid);
                iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_STRING,63,caFile,
                                                  UT_TYPE_STRING,127,caSub);
                if(iReturn == 0) {
   	                sprintf(caTempFile,"%s/%s/%s",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"),pType,caFile);
                    sprintf(caSub1,"Fw:%s",caSub);
                    strcpy(caCont,ncLang("0848这是一封由网络督察转发的邮件。\n"));
                    sprintf(pFfile,"%s/tmp/%lu",utComGetVar_sd(psShmHead,"MailDir","."),utStrGetId());
                    iReturn = mail_digest(caTempFile,pFfile,"ProEIM",caSmail,
                                      caSub1,caCont,0);
                    iReturn = sendMailConFromFile(pFfile,NULL);
                }   	            
            }
        }
    }
    return 0;
}


/* 导出邮件整理  */
int ncWebMailExportDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caMailctl[12],caMailmon[12];
    char caSdate[32],caEdate[32];
    int lCount1,lCount2,iReturn;
    utPltDbHead *psDbHead;
    iReturn = utMsgGetSomeNVar(psMsgHead,4,
                    "mailctl",  UT_TYPE_STRING, 11, caMailctl,
                    "mailmon",  UT_TYPE_STRING, 11, caMailmon,
					"sdate",    UT_TYPE_STRING, 11, caSdate,
					"edate",    UT_TYPE_STRING, 11, caEdate
					); 
    if(!utStrIsSpaces(caMailctl)) { /* 整理导出的拦截邮件  */
        lCount1 = ncUtlDelMailLog(psShmHead,"ncmaillogo","ctl",caSdate,caEdate);
        if(lCount1 < 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0812邮件整理"),ncLang("0813邮件整理出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
    }
    if(!utStrIsSpaces(caMailmon)) { /* 整理导出的监控邮件  */
        lCount2 = ncUtlDelMailLog(psShmHead,"ncmaillogo1","mon",caSdate,caEdate);
        if(lCount2 < 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0812邮件整理"),ncLang("0813邮件整理出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
    }
    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0812邮件整理"),ncLang("0814邮件整理完毕<p>共删除 %d封拦截邮件 %d封监控邮件"),lCount1,lCount2);
    return 0;
}


int ncUtlDelMailLog(utShmHead *psShmHead,char *pTables,char *pDir,char *pSdate,char *pEdate)
{
    pasDbCursor *psCur;
    char caSdate[32],caEdate[32];
    char caFile[128],caFile0[128];
    char *pMailDir;
    int i,lCount,iReturn,iStat;
    lCount = 0;
    for(i=0;i<10;i++) {
        if(pSdate[i] == '/') caSdate[i]='-';
        else caSdate[i] = pSdate[i];
        if(pEdate[i] == '/') caEdate[i]='-';
        else caEdate[i] = pEdate[i];
    }
    strcpy(caEdate+10," 23:59:59");
    strcpy(caSdate+10," 00:00:00");
    pMailDir = utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog");
    psCur =  pasDbOpenSqlF("select mailfile from %s  where sdate >= '%s' and sdate <= '%s' ",
                           pTables,caSdate,caEdate);
    if(psCur) {
     	iReturn = 0;
     	while(iReturn == 0) {
      	    iReturn = pasDbFetchInto(psCur,
      	                           UT_TYPE_STRING,63,caFile);
            if(iReturn == 0) {
                sprintf(caFile0,"%s/%s/%s",pMailDir,caFile);
                iStat = remove(caFile0);
                if(iStat != 0) {
                    printf(" Remove File %s Error \n",caFile0);
                }
                else {
                    lCount ++;
                }
            }
        }
        pasDbCloseCursor(psCur);
     }
     else {
     	printf(" Open Cursor Error %s \n",pasDbErrorMsg(NULL));
     	return (-1);
     }
     if(lCount > 0) {
         iReturn = pasDbExecSqlF("delete from %s  where sdate >= '%s' and sdate <= '%s' ",
                   pTables,caSdate,caEdate);
         if(iReturn == 0) {
     	    pasDbCommit(NULL);
         }
     }
     return lCount;
}


/* 读一个表单文件  */
int ncWebPostRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncUserCont  *psUser;
    ncPostHead  *psHead;
    pasFile     *ps;
    ncPostCont *psCont;
    char       *pMailDir;
    char      caTempDir[128],caFile[128],caDfile[128],caSfile[128];
    char      caMsgFile[128],caUserid[16];
    char      caTime[32],caDispname[32],caGroupname[32];
    int i,iNum,i1,i2,l,iCode=0,iCode1;
    unsigned long lUserid,lTime;
     char caOut[10204];
    int iRet;
    char caUrl[256],caSubject[256],caTemp[1024],caContent[1024];
    utPltDbHead *psDbHead;
   	int iReturn;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif   	
    iReturn = utMsgGetSomeNVar(psMsgHead,3,
                    "userid",      UT_TYPE_STRING, 12, caUserid,
	                "msgfile",     UT_TYPE_STRING, 63, caMsgFile,
	                "time",        UT_TYPE_STRING, 20, caTime);
	 utStrReplaceWith(caTime,"?"," ");               
	                
//                printf("caTime=%s\n",caTime);
    if(utStrIsSpaces(caMsgFile)) { /* 邮件无内容  */
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0127表单"),ncLang("0655无内容"));
        return 0;
    }
    lUserid = atol(caUserid);
 	/* 读邮件头信息  */
 	pMailDir = utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog");
 	sprintf(caFile,  "%s/frm/%s",pMailDir,caMsgFile);
    sprintf(caTempDir,"%s/tmp/%08d",pMailDir,utStrGetId());
    if(utFileIsExist(caTempDir)!=1) {
        if(mkdir(caTempDir,S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)!=0) {
            utSysLog(" Dir Mkerror %s",caTempDir);
            return 0;
        }
    }
    
    ps = pasOpenFile(caFile);
    if(ps == NULL) {
    	/*
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_close.htm",ncLang("0127表单"),ncLang("0131表单文件打开出错"));
        return 0;
        */
        memset(caDispname,0,sizeof(caDispname));
        memset(caUrl,0,sizeof(caUrl));

        memset(caSubject,0,sizeof(caSubject));
        memset(caContent,0,sizeof(caContent));
        sprintf(caTemp,"select dispname,url,subject,content from ncbbsmon where userid=%s and upfile='%s' ",caUserid,caMsgFile);
        pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caDispname,
                                UT_TYPE_STRING,120,caUrl,
                                UT_TYPE_STRING,255,caSubject,
                                UT_TYPE_STRING,1000,caContent);
                                
       psDbHead = utPltInitDb();    
       utPltSetCvtHtml(1);
       
    psUser = ncUtlGetUserContById(psShmHead,atol(caUserid));
    if(psUser) {
        strcpy(caDispname,psUser->dispname);
        if(psUser->psGroup) {
            strcpy(caGroupname,psUser->psGroup->groupname);
        }
        else {
            strcpy(caGroupname,"unknow");
        }
    }
    else {
        strcpy(caDispname,"unknow");
        strcpy(caGroupname,"\0");
    }
       
       
       utPltPutVar(psDbHead,"username",caDispname);
       utPltPutVar(psDbHead,"groupname",caGroupname);
       utPltPutVar(psDbHead,"date",caTime);
       utPltPutLoopVar(psDbHead,"host",1,caUrl);
       utPltPutLoopVar2(psDbHead,"vvar",1,1,"主题");
       pasUtf8ToGBK(caSubject,caOut,255);			                    	
       utPltPutLoopVar2(psDbHead,"vvalue",1,1,caOut);
       utPltPutLoopVar2(psDbHead,"vvar",1,2,"内容");
       pasUtf8ToGBK(caContent,caOut,1000);	
       utPltPutLoopVar2(psDbHead,"vvalue",1,2,caOut);
       
       utPltSetCvtHtml(0);	   
       utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_postcontent.htm");    
       return 0;             
        
    }
    l = strlen(caFile);
    if(caFile[l-2] == '-') {
        iCode = atol(&caFile[l-1]);
    }
    else {
        iCode = pasDefaultCode();
    }
    
    psDbHead = utPltInitDb();
    psUser = ncUtlGetUserContById(psShmHead,lUserid);
    if(psUser) {
        strcpy(caDispname,psUser->dispname);
        if(psUser->psGroup) {
            strcpy(caGroupname,psUser->psGroup->groupname);
        }
        else {
            strcpy(caGroupname,"unknow");
        }
    }
    else {
        strcpy(caDispname,"unknow");
        strcpy(caGroupname,"\0");
    }
    utPltPutVar(psDbHead,"username",caDispname);
    utPltPutVar(psDbHead,"groupname",caGroupname);
    utPltPutVar(psDbHead,"date",caTime);
    psHead = ncPostGetPostData(ps);
    iNum = 0;
    
    while(psHead) {
        iCode1 = ncPostCharsetCode(psHead);
        if(iCode1 >= 0) {
            iCode = iCode1;
        }
        iNum++;
        if(iCode != pasDefaultCode()) {
#ifdef LDEBUG1
            printf(" CvtCode %d \n",iCode);
#endif
            ncPostCvtDefaultCode(psHead,iCode);
        }
        
        utPltPutLoopVar(psDbHead,"host",iNum,psHead->pHost?psHead->pHost:"\0");
        utPltPutLoopVar(psDbHead,"url",iNum,psHead->pUrl?psHead->pUrl:"\0");
        psCont = psHead->psCont;
        i1 = 0;
        i2 = 0;
        while(psCont) {
            if(psCont->lFileId > 0) {  /* 附件  */
                i1++;
                sprintf(caDfile,"%s/%lu",caTempDir,psCont->lFileId);
                link(ncPostFileName(psCont->lFileId,caSfile),caDfile);
                utPltPutLoopVar2(psDbHead,"fvar",iNum,i1,psCont->pVar?psCont->pVar:"\0");
                
                        if(psCont->pValue){
                    	  iRet=pasCharIsUtf8(psCont->pValue, strlen(psCont->pValue));
                    	      if(iRet){
			                    	pasUtf8ToGBK(psCont->pValue,caOut,10000);
			                    	utPltPutLoopVar2(psDbHead,"fvalue",iNum,i1,caOut);
			                    }
                    	     else{
                    	     	utPltPutLoopVar2(psDbHead,"fvalue",iNum,i1,psCont->pValue);
                    	    }
                    }   
                	
                	
    //            utPltPutLoopVar2(psDbHead,"fvalue",iNum,i1,psCont->pValue?psCont->pValue:"\0");
                if(psCont->pValue) {
                    for(i=strlen(psCont->pValue)-1;
                       psCont->pValue[i]!=92 && psCont->pValue[i]!='/' && i>=0;i--);
                       
                        iRet=pasCharIsUtf8(psCont->pValue+ i + 1, strlen(psCont->pValue+ i + 1));
                        if(iRet){
                        	pasUtf8ToGBK(psCont->pValue+ i + 1,caOut,10000);
			                    	utPltPutLoopVar2(psDbHead,"disp",iNum,i1,caOut);
                        }
                        else{
                         utPltPutLoopVar2(psDbHead,"disp",iNum,i1,psCont->pValue + i + 1);
                        }
                }
                utPltPutLoopVar2(psDbHead,"ftype", iNum,i1,psCont->pContentType?psCont->pContentType:"\0");
                utPltPutLoopVar2(psDbHead,"fname", iNum,i1,caDfile);
            }
            else {
                if(psCont->pValue && !utStrIsSpaces(psCont->pValue)) {
                    i2++;
                    utPltPutLoopVar2(psDbHead,"vvar",iNum,i2,psCont->pVar?psCont->pVar:"\0");
                    	
                    utPltSetCvtHtml(1);	
                    
                    if(psCont->pValue){
                    	  iRet=pasCharIsUtf8(psCont->pValue, strlen(psCont->pValue));
                    	      if(iRet){
			                    	pasUtf8ToGBK(psCont->pValue,caOut,10000);
			                    	utPltPutLoopVar2(psDbHead,"vvalue",iNum,i2,caOut);
			                    }
                    	     else{
                    	     	utPltPutLoopVar2(psDbHead,"vvalue",iNum,i2,psCont->pValue);
                    	    }
                    }
                    
  //                  utPltPutLoopVar2(psDbHead,"vvalue",iNum,i2,psCont->pValue?psCont->pValue:"\0");
                    utPltSetCvtHtml(0);
                }                
            }
            psCont = psCont->next;
        }
        ncPostFreeHttpHead(psHead);
        psHead = ncPostGetPostData(ps);
    }
    pasCloseFile(ps);
   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"mail/nc_postcontent.htm");
   	return 0;
}
