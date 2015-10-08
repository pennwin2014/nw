#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "nwdef.h"
#include "ncdef.h"
int lan_fileoutList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
  utMsgPrintMsg(psMsgHead);
  
  char deal_in[8] = "";
  char recno_in[8] = "";
  char filetype_in[16] = "";
  char hostmail_in[24] = "";
  char hostmailmode_in[24] = "";
  char httpname_in[24] = "";
  char httpmode_in[24] = "";
  char mailsend_in[24] = "";
  char mailRev_in[24] = "";
  char fileexp_in[24] = "";
  char fileexpmode_in[24] = "";
  char filesize_in[24] = "";
  char controlcode_in[24] = "";
  char mailRevmode_in[24]="";
  char mailsendmode_in[24]="";
  char smtpsoft_in[24]="";
  char smtpsoftmode_in[24]="";
  char ftpname_in[24]="";
  char ftpmode_in[24]="";
  char ftpsoft_in[24]="";
  char ftpsoftmode_in[24]="";
  char chataccount_in[32],chatsoft_in[32];
  char chataccountmode_in[24],chatsoftmode_in[24];
  char cloudsoft_in[24],cloudsoftmode_in[24];
  char compid_in[24] = "";
  char groupid_in[24] = "";
  char caIds[1024];
  utMsgGetSomeNVar(psMsgHead,28,
    "recids",       UT_TYPE_STRING,1000,caIds,
		"deal",		      UT_TYPE_STRING,	sizeof(deal_in)-1,		      deal_in,
		"recno",		    UT_TYPE_STRING,	sizeof(recno_in)-1,		      recno_in,
    "filetype",     UT_TYPE_STRING, sizeof(filetype_in)-1,      filetype_in,
    "hostmail",		  UT_TYPE_STRING, sizeof(hostmail_in)-1, 	    hostmail_in,
    "hostmailmode",	UT_TYPE_STRING, sizeof(hostmailmode_in)-1, 	hostmailmode_in,
    "httpname",     UT_TYPE_STRING, sizeof(httpname_in)-1,      httpname_in,
    "httpmode",     UT_TYPE_STRING, sizeof(httpmode_in)-1,      httpmode_in,
    "mailsend",		  UT_TYPE_STRING, sizeof(mailsend_in)-1, 	    mailsend_in,
    "mailRev",		  UT_TYPE_STRING, sizeof(mailRev_in)-1, 	    mailRev_in,
    "mailRevmode",  UT_TYPE_STRING,sizeof(mailRevmode_in)-1,    mailRevmode_in,
    "mailsendmode", UT_TYPE_STRING,sizeof(mailsendmode_in)-1,   mailsendmode_in,
    "smtpsoft",     UT_TYPE_STRING,sizeof(smtpsoft_in)-1,       smtpsoft_in,
    "smtpsoftmode", UT_TYPE_STRING,sizeof(smtpsoftmode_in)-1,   smtpsoftmode_in,
    "fileexp",      UT_TYPE_STRING, sizeof(fileexp_in)-1,       fileexp_in,
    "fileexpmode",  UT_TYPE_STRING, sizeof(fileexpmode_in)-1,   fileexpmode_in,
    "filesize",     UT_TYPE_STRING, sizeof(filesize_in)-1,      filesize_in,
    "controlcode",  UT_TYPE_STRING, sizeof(controlcode_in)-1,   controlcode_in,
    "ftpname",      UT_TYPE_STRING,sizeof(ftpname_in)-1,        ftpname_in,
    "ftpmode",      UT_TYPE_STRING,sizeof(ftpmode_in)-1,        ftpmode_in,
    "ftpsoft",      UT_TYPE_STRING,sizeof(ftpsoft_in)-1,        ftpsoft_in,
    "ftpsoftmode",  UT_TYPE_STRING,sizeof(ftpsoftmode_in)-1,    ftpsoftmode_in,
    "chataccount",  UT_TYPE_STRING,sizeof(chataccount_in)-1,    chataccount_in,
    "chatsoft",     UT_TYPE_STRING,sizeof(chatsoft_in)-1,       chatsoft_in,
    "chataccountmode",UT_TYPE_STRING,sizeof(chataccountmode_in)-1,  chataccountmode_in,
    "chatsoftmode",   UT_TYPE_STRING,sizeof(chatsoftmode_in)-1, chatsoftmode_in,
    "compid",       UT_TYPE_STRING, sizeof(compid_in)-1,        compid_in,
    "groupid",      UT_TYPE_STRING, sizeof(groupid_in)-1,       groupid_in);
  if(strcmp(utf8convert(psShmHead,hostmail_in),ncLang("0170无"))==0){
  	strcpy(hostmail_in,"");
  }
  if(strcmp(utf8convert(psShmHead,httpname_in),ncLang("0170无"))==0){
  	strcpy(httpname_in,"");
  }
  if(strcmp(utf8convert(psShmHead,mailsend_in),ncLang("0170无"))==0){
  	strcpy(mailsend_in,"");
  }
  if(strcmp(utf8convert(psShmHead,mailRev_in),ncLang("0170无"))==0){
  	strcpy(mailRev_in,"");
  }
  if(strcmp(utf8convert(psShmHead,smtpsoft_in),ncLang("0170无"))==0){
  	strcpy(smtpsoft_in,"");
  }
  if(strcmp(utf8convert(psShmHead,fileexp_in),ncLang("0170无"))==0){
  	strcpy(fileexp_in,"");
  }
  if(strcmp(utf8convert(psShmHead,ftpname_in),ncLang("0170无"))==0){
  	strcpy(ftpname_in,"");
  }
  if(strcmp(utf8convert(psShmHead,ftpsoft_in),ncLang("0170无"))==0){
  	strcpy(ftpsoft_in,"");
  }
  if(strcmp(utf8convert(psShmHead,chataccount_in),ncLang("0170无"))==0){
  	strcpy(chataccount_in,"");
  }
  if(strcmp(chatsoft_in,utf8convert(psShmHead,ncLang("0170无")))==0){
  	strcpy(chatsoft_in,"");
  }
  utMsgPrintMsg(psMsgHead);
  printf("======%s========%s\n",filetype_in,deal_in);
  if(strlen(filesize_in)==0){
  	strcpy(filesize_in,"0");
  }
  if(strlen(controlcode_in)==0){
  	strcpy(controlcode_in,"0");
  }
  char auth[32] = "";
	char shortname[32] = "";
	dsCltGetMyInfo(1,"USERNAME",auth);
	findShortName(auth,shortname);
	int userlevel=0;
	userlevel = findUserLevel(auth);
	
	char sql[4512] = "";
	char sql_tmp[1280] = "";
	
	pasDbCursor *psCur = NULL;
  utPltDbHead *psDbHead = utPltInitDb();
  
  int iret = 0;
  int iNum = 0;
  
  unsigned long compid = strtoul(compid_in,NULL,10);
  unsigned long groupid = strtoul(groupid_in,NULL,10);
	
	if(!utStrIsSpaces(compid_in)){
	  snprintf(sql_tmp,sizeof(sql_tmp)," and a.computerid=%lu",compid);
	}
	  char *authgroup = getDsGroupid();
	if((!utStrIsSpaces(groupid_in))&&(strcmp(filetype_in,"all"))!=0){
		char caGroups[1024];
		strcpy(caGroups,getGroupid(groupid));
	  snprintf(sql_tmp+strlen(sql_tmp),sizeof(sql_tmp)-strlen(sql_tmp)," and (a.groupid in (%s) or a.computerid in (select compid from nwcompute where groupid in (%s)))",caGroups,caGroups);
	}else if(!utStrIsSpaces(authgroup)){
		sprintf(sql_tmp+strlen(sql_tmp)," and (a.groupid in (%s) or a.computerid in (select compid from nwcompute where groupid in (%s)))",authgroup,authgroup);
	}	
	sprintf(sql_tmp+strlen(sql_tmp)," and a.flag=0 ");
  unsigned long sid = 0;
  char groupname[32] = "";
  char compname[32] = "";
  char hostmail[32] = "";
  unsigned long hostmailmode = 0;
  char httpname[32] = "";
  unsigned long httpmode = 0;
  char mailsend[32] = "";
  char mailrev[32] = "";
  char fileexp[32] = "";
  unsigned long fileexpmode = 0;
  unsigned long filesize = 0;
  unsigned long controlcode = 0;
  char smtpname[32]="";
  unsigned long  smtpmode,mailsendmode,mailrevmode;
  char ftpname[32],ftpserver[32];
  unsigned long ftpmode,ftpservermode;
  char chataccount[32],chatsoft[32];
  unsigned long chataccountmode,chatsoftmode;
  char cloudsoft[32];
  unsigned long cloudsoftmode;
  char modtime[24] = "";
  unsigned long level = 0;
  unsigned long utype,umode;
  char uname[32];
  long fileout,fileread;
  nwCompInfo *psComp;
  char caTemp[1024],devname[64];
  char caIp[20];
 
  if((!strcmp(filetype_in,"advance"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
  //    printf("adfggggg in\n");
      snprintf(sql,sizeof(sql),"delete from nwfilemainprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfilemainprocy(groupid,computerid,company,controlcode,createtime,modtime,level)values(%lu,%lu,'%s',%s,%lu,%lu,%d)",groupid,compid,shortname,controlcode_in,time(0),time(0),userlevel);
      
      writeLog("fileout.txt","advance",sql);
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
  //  	 snprintf(sql,sizeof(sql),"delete from nwfilemainprocy where sid=%s ",recno_in);
  snprintf(sql,sizeof(sql),"update  nwfilemainprocy set flag=1 where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	snprintf(sql,sizeof(sql),"update nwfilemainprocy set flag=1 where sid in (%s) ",caIds);
 //   	 snprintf(sql,sizeof(sql),"delete from nwfilemainprocy where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
    
    snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfilemainprocy as a  where company='%s' %s order by a.groupid",shortname,sql_tmp);
    
    writeLog("fileout.txt","advance",sql);  
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){    
     if(0!=strcmp(filetype_in,"all")) {iNum = 0;}
      sid = 0;
      memset(groupname,0,sizeof(groupname));
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      groupid=0;
      compid=0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
      memset(caIp,0,sizeof(caIp));
      while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),      		&sid,
          UT_TYPE_ULONG, 4, 	&groupid,
          UT_TYPE_ULONG, 4, 	&compid,
          UT_TYPE_ULONG,  sizeof(controlcode),  &controlcode,
          UT_TYPE_STRING, sizeof(modtime)-1, 		modtime,
          UT_TYPE_ULONG,  sizeof(level),        &level)) || 1405==iret){
            
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			
  			if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  				  strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);
  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
  			utPltPutLoopVarF(psDbHead,"controlcode",iNum,"%lu",controlcode);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level); 
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0150优先策略"));
  			memset(caTemp,0,sizeof(caTemp));
  			if(controlcode&0X00000001){
  				sprintf(caTemp+strlen(caTemp)," %s",ncLang("0151禁止HTTP外发"));
  			}
  			if(controlcode&0X00000002){
  				sprintf(caTemp+strlen(caTemp)," %s",ncLang("0152禁止SMTP外发"));
  			}
  			if(controlcode&0X00000004){
  				sprintf(caTemp+strlen(caTemp)," %s",ncLang("0153禁止FTP外发"));
  			}
  			if(controlcode&0X00000010){
  				sprintf(caTemp+strlen(caTemp)," %s",ncLang("0154禁止聊天软件外发"));
  			}
  			if(controlcode&0X00000020){
  				sprintf(caTemp+strlen(caTemp)," %s",ncLang("0155禁止云盘外发"));
  			}
  			if(controlcode&0X00000040){
  				sprintf(caTemp+strlen(caTemp)," %s",ncLang("0156禁止USB外发"));
  			}
  			if(controlcode&0X00000080){
  				sprintf(caTemp+strlen(caTemp)," %s",ncLang("0157禁止共享外发"));
  			}
  			if(controlcode&0X00000200){
  				sprintf(caTemp+strlen(caTemp)," %s",ncLang("0158禁止HTTPS外发"));
  			}
  			 if(controlcode&0X00000100){
  				sprintf(caTemp+strlen(caTemp)," %s",ncLang("0159禁止其它软件外发"));
  			}
  			utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
  			
  			memset(groupname,0,sizeof(groupname));
	      memset(devname,0,sizeof(devname));
	      memset(caIp,0,sizeof(caIp));
	      controlcode = 0;
	      memset(modtime,0,sizeof(modtime));
	      level = 0;
  			
      }
      pasDbCloseCursor(psCur);
    } 
  }
 // else
 if((!strcmp(filetype_in,"http"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
      snprintf(sql,sizeof(sql),"delete from nwfilehttpprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfilehttpprocy(groupid,computerid,company,hosttype,hostmail,hostmailmode,httptype,httpname,httpmode,exptype,fileexp,fileexpmode,filesize,controlcode,createtime,modtime,level)values(%lu,%lu,'%s',4,'%s',%s,3,'%s',%s,7,'%s',%s,%s,%s,%lu,%lu,%d)",groupid,compid,shortname,utf8convert(psShmHead,hostmail_in),hostmailmode_in,utf8convert(psShmHead,httpname_in),httpmode_in,utf8convert(psShmHead,fileexp_in),fileexpmode_in,filesize_in,controlcode_in,time(0),time(0),userlevel);
      writeLog("fileout.txt","http",sql);
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
    	 snprintf(sql,sizeof(sql),"update nwfilehttpprocy set flag=1 where sid=%s ",recno_in);
 //   	 snprintf(sql,sizeof(sql),"delete from nwfilehttpprocy where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	snprintf(sql,sizeof(sql),"update nwfilehttpprocy set flag=1 where sid in (%s) ",caIds);
//    	 snprintf(sql,sizeof(sql),"delete from nwfilehttpprocy where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
    
    
    
    
    
    snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.hostmail,a.hostmailmode,a.httpname,a.httpmode,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfilehttpprocy as a where company='%s' %s",shortname,sql_tmp);
    printf("sql=%s\n",sql);
    writeLog("fileout.txt","http",sql);
    
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){      
      if(0!=strcmp(filetype_in,"all")) {iNum = 0;}
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      memset(hostmail,0,sizeof(hostmail));
      hostmailmode = 0;
      memset(httpname,0,sizeof(httpname));
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
      
      while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),      		&sid,
          UT_TYPE_ULONG,4,&groupid,
          UT_TYPE_ULONG,4,&compid,
          UT_TYPE_STRING, sizeof(hostmail)-1, 	hostmail,
          UT_TYPE_ULONG,  sizeof(hostmailmode), &hostmailmode,
          UT_TYPE_STRING, sizeof(httpname)-1, 	httpname,
          UT_TYPE_ULONG,  sizeof(httpmode),     &httpmode,
          UT_TYPE_STRING, sizeof(fileexp)-1, 	  fileexp,
          UT_TYPE_ULONG,  sizeof(fileexpmode),  &fileexpmode,
          UT_TYPE_ULONG,  sizeof(filesize),     &filesize,
          UT_TYPE_ULONG,  sizeof(controlcode),  &controlcode,
          UT_TYPE_STRING, sizeof(modtime)-1, 		modtime,
          UT_TYPE_ULONG,  sizeof(level),        &level)) || 1405==iret){
            
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  					strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
        utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0160Http外发"));
  			utPltPutLoopVar(psDbHead,"hostmail",iNum,hostmail);
  			utPltPutLoopVarF(psDbHead,"hostmailmode",iNum,"%lu",hostmailmode);
  			utPltPutLoopVar(psDbHead,"httpname",iNum,httpname);
  			utPltPutLoopVarF(psDbHead,"httpmode",iNum,"%lu",httpmode);
  			utPltPutLoopVar(psDbHead,"fileexp",iNum,fileexp);
  			utPltPutLoopVarF(psDbHead,"fileexpmode",iNum,"%lu",fileexpmode);
  			utPltPutLoopVarF(psDbHead,"filesize",iNum,"%lu",filesize);
  			utPltPutLoopVarF(psDbHead,"controlcode",iNum,"%lu",controlcode);
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level);
  			memset(caTemp,0,sizeof(caTemp));
  			if(strlen(fileexp)>0){
  				if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"禁止文件类型:[%s]",fileexp);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"允许文件类型:[%s]",fileexp);
  				}
  			}
  			if(filesize>0){
  				 if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]禁止外发",filesize);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]允许止外发",filesize);
  				}
  			}
  			if(controlcode==1){
  				sprintf(caTemp+strlen(caTemp)," 需审批");
  			}
  			else if(controlcode==2){
  				sprintf(caTemp+strlen(caTemp)," 备份到服务器");
  			}
  			else if(controlcode==3){
  				sprintf(caTemp+strlen(caTemp)," 审批+备份");
  			}
  			if(strlen(hostmail)>0){
  				if(hostmailmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止邮箱主机类型:[%s]",hostmail);
  				}
  				else if(hostmailmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许邮箱主机类型:[%s]",hostmail);
  				}
  			}
  			if(strlen(httpname)>0){
  				if(httpmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止浏览器进程类型[%s]",httpname);
  				}
  				else if(httpmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许浏览器进程类型[%s]",httpname);
  				}
  			
      }
      utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
     }
      pasDbCloseCursor(psCur);
    } 
  }//else 
  if((!strcmp(filetype_in,"smtp"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
      snprintf(sql,sizeof(sql),"delete from nwfilesmtpprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfilesmtpprocy(groupid,computerid,company,hostmail,hostmailmode,smtpname,smtpmode,mailsend,mailRev,fileexp,fileexpmode,filesize,controlcode,createtime,modtime,level,hosttype,smtptype,mailsendtype,mailrevtype,exptype,mailsendmode,mailrevmode)values(%lu,%lu,'%s','%s',%s,'%s',%s,'%s','%s','%s',%s,%s,%s,%lu,%lu,%d,4,8,5,6,7,%lu,%lu)",groupid,compid,shortname,utf8convert(psShmHead,hostmail_in),hostmailmode_in,utf8convert(psShmHead,smtpsoft_in),smtpsoftmode_in,utf8convert(psShmHead,mailsend_in),utf8convert(psShmHead,mailRev_in),utf8convert(psShmHead,fileexp_in),fileexpmode_in,filesize_in,controlcode_in,time(0),time(0),userlevel,atol(mailsendmode_in),atol(mailRevmode_in));
      writeLog("fileout.txt","smtp",sql); 
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
    	snprintf(sql,sizeof(sql),"update nwfilesmtpprocy set flag=1 where sid=%s ",recno_in);
//    	 snprintf(sql,sizeof(sql),"delete from nwfilesmtpprocy where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	snprintf(sql,sizeof(sql),"update nwfilesmtpprocy set flag=1 where sid in (%s) ",caIds);
 //   	 snprintf(sql,sizeof(sql),"delete from nwfilesmtpprocy where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
    
    
    
     snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.hostmail,a.hostmailmode,a.smtpname,a.smtpmode,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level,mailsend,mailsendmode,mailrev,mailrevmode from nwfilesmtpprocy as a where company='%s' %s",shortname,sql_tmp);
   
    printf("sql=%s\n",sql);
    
    writeLog("fileout.txt","smtp",sql); 
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){      
     if(0!=strcmp(filetype_in,"all")) {iNum = 0;}
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      memset(hostmail,0,sizeof(hostmail));
      hostmailmode = 0;
      memset(httpname,0,sizeof(httpname));
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
      
      while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),      		&sid,
          UT_TYPE_ULONG,4,&groupid,
          UT_TYPE_ULONG,4,&compid,
          UT_TYPE_STRING, sizeof(hostmail)-1, 	hostmail,
          UT_TYPE_ULONG,  sizeof(hostmailmode), &hostmailmode,
          UT_TYPE_STRING, sizeof(httpname)-1, 	smtpname,
          UT_TYPE_ULONG,  sizeof(httpmode),     &smtpmode,
          UT_TYPE_STRING, sizeof(fileexp)-1, 	  fileexp,
          UT_TYPE_ULONG,  sizeof(fileexpmode),  &fileexpmode,
          UT_TYPE_ULONG,  sizeof(filesize),     &filesize,
          UT_TYPE_ULONG,  sizeof(controlcode),  &controlcode,
          UT_TYPE_STRING, sizeof(modtime)-1, 		modtime,
          UT_TYPE_ULONG,  sizeof(level),        &level,
          UT_TYPE_STRING, sizeof(mailsend)-1,   mailsend,
          UT_TYPE_ULONG,  4,&mailsendmode,
          UT_TYPE_STRING, sizeof(mailrev)-1,    mailrev,
          UT_TYPE_ULONG,  4,&mailrevmode)) || 1405==iret){
            
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			  			if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  					strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			 utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0161SMTP外发"));
  			
  			
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"compname",iNum,compname);
  			utPltPutLoopVar(psDbHead,"hostmail",iNum,hostmail);
  			utPltPutLoopVarF(psDbHead,"hostmailmode",iNum,"%lu",hostmailmode);
  			utPltPutLoopVar(psDbHead,"smtpname",iNum,smtpname);
  			utPltPutLoopVarF(psDbHead,"smtpmode",iNum,"%lu",smtpmode);
  			utPltPutLoopVar(psDbHead,"mailsend",iNum,mailsend);
  			utPltPutLoopVar(psDbHead,"mailrev",iNum,mailrev);
  			utPltPutLoopVarF(psDbHead,"mailsendmode",iNum,"%lu",mailsendmode);
  			utPltPutLoopVarF(psDbHead,"mailrevmode",iNum,"%lu",mailrevmode);
  			utPltPutLoopVar(psDbHead,"fileexp",iNum,fileexp);
  			utPltPutLoopVarF(psDbHead,"fileexpmode",iNum,"%lu",fileexpmode);
  			utPltPutLoopVarF(psDbHead,"filesize",iNum,"%lu",filesize);
  			utPltPutLoopVarF(psDbHead,"controlcode",iNum,"%lu",controlcode);
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level);
  			
  		memset(caTemp,0,sizeof(caTemp));
  			if(strlen(fileexp)>0){
  				if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"禁止文件类型:[%s]",fileexp);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"允许文件类型:[%s]",fileexp);
  				}
  			}
  			if(filesize>0){
  				 if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]禁止外发",filesize);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]允许止外发",filesize);
  				}
  			}
  			if(controlcode==1){
  				sprintf(caTemp+strlen(caTemp)," 需审批");
  			}
  			else if(controlcode==2){
  				sprintf(caTemp+strlen(caTemp)," 备份到服务器");
  			}
  			else if(controlcode==3){
  				sprintf(caTemp+strlen(caTemp)," 审批+备份");
  			}
  			if(strlen(hostmail)>0){
  				if(hostmailmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止邮箱主机类型:[%s]",hostmail);
  				}
  				else if(hostmailmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许邮箱主机类型:[%s]",hostmail);
  				}
  			}
  			if(strlen(smtpname)>0){
  				if(smtpmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止邮件服务器类型[%s]",smtpname);
  				}
  				else if(smtpmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许邮件服务器类型[%s]",smtpname);
  				}			
        }
  			
  			if(strlen(mailsend)>0){
  				if(mailsendmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止发件人类型[%s]",mailsend);
  				}
  				else if(mailsendmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许发件人类型[%s]",mailsend);
  				}
  			}
  			if(strlen(mailrev)>0){
  				if(mailrevmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止收件人类型[%s]",mailrev);
  				}
  				else if(mailrevmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许收件人类型[%s]",mailrev);
  				}
  			}
  				utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
  		
  		sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      memset(chataccount,0,sizeof(chataccount));
      memset(chatsoft,0,sizeof(chatsoft));
      chataccountmode = 0;
      chatsoftmode=0;
     
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
  		
      }
      pasDbCloseCursor(psCur);
    } 
  }//else
  if((!strcmp(filetype_in,"ftp"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
      snprintf(sql,sizeof(sql),"delete from nwfileftpprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfileftpprocy(groupid,computerid,company,ftpservertype,ftpserver,ftpservermode,ftptype,ftpname,ftpmode,exptype,fileexp,fileexpmode,filesize,controlcode,createtime,modtime,level) \
      values(%lu,%lu,'%s',10,'%s',%s,11,'%s',%s,7,'%s',%s,%s,%s,%lu,%lu,%d)",\
      groupid,compid,shortname,utf8convert(psShmHead,ftpsoft_in),ftpsoftmode_in,utf8convert(psShmHead,ftpname_in),ftpmode_in,utf8convert(psShmHead,fileexp_in),fileexpmode_in,filesize_in,controlcode_in,time(0),time(0),userlevel);
      
      writeLog("fileout.txt","ftp",sql); 
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
    	snprintf(sql,sizeof(sql),"update nwfileftpprocy set flag=1 where sid=%s ",recno_in);
//    	 snprintf(sql,sizeof(sql),"delete from nwfileftpprocy where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	snprintf(sql,sizeof(sql),"update nwfileftpprocy  set flag=1 where sid in (%s) ",caIds);
 //   	 snprintf(sql,sizeof(sql),"delete from nwfileftpprocy where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
    
    snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.ftpname,a.ftpmode,a.ftpserver,a.ftpservermode,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfileftpprocy as a where company='%s' %s",shortname,sql_tmp);
    printf("sql=%s\n",sql);
    writeLog("fileout.txt","ftp",sql);
    
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){      
     if(0!=strcmp(filetype_in,"all")) {iNum = 0;}
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      memset(ftpname,0,sizeof(ftpname));
      ftpmode = 0;
      ftpservermode=0;
      memset(ftpserver,0,sizeof(ftpserver));
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
      
      while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),      		&sid,
          UT_TYPE_ULONG,4,&groupid,
          UT_TYPE_ULONG,4,&compid,
          UT_TYPE_STRING, sizeof(ftpname)-1, 	ftpname,
          UT_TYPE_ULONG,  sizeof(ftpmode), &ftpmode,
          UT_TYPE_STRING, sizeof(ftpserver)-1, 	ftpserver,
          UT_TYPE_ULONG,  sizeof(ftpservermode),     &ftpservermode,
          UT_TYPE_STRING, sizeof(fileexp)-1, 	  fileexp,
          UT_TYPE_ULONG,  sizeof(fileexpmode),  &fileexpmode,
          UT_TYPE_ULONG,  sizeof(filesize),     &filesize,
          UT_TYPE_ULONG,  sizeof(controlcode),  &controlcode,
          UT_TYPE_STRING, sizeof(modtime)-1, 		modtime,
          UT_TYPE_ULONG,  sizeof(level),        &level)) || 1405==iret){
            
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  					strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
        utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0162FTP外发"));
  			utPltPutLoopVar(psDbHead,"ftpname",iNum,ftpname);
  			utPltPutLoopVarF(psDbHead,"ftpmode",iNum,"%lu",ftpmode);
  			utPltPutLoopVar(psDbHead,"ftpserver",iNum,ftpserver);
  			utPltPutLoopVarF(psDbHead,"ftpservermode",iNum,"%lu",ftpservermode);
  			utPltPutLoopVar(psDbHead,"fileexp",iNum,fileexp);
  			utPltPutLoopVarF(psDbHead,"fileexpmode",iNum,"%lu",fileexpmode);
  			utPltPutLoopVarF(psDbHead,"filesize",iNum,"%lu",filesize);
  			utPltPutLoopVarF(psDbHead,"controlcode",iNum,"%lu",controlcode);
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level);
  			memset(caTemp,0,sizeof(caTemp));
  			if(strlen(fileexp)>0){
  				if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"禁止文件类型:[%s]",fileexp);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"允许文件类型:[%s]",fileexp);
  				}
  			}
  			if(filesize>0){
  				 if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]禁止外发",filesize);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]允许止外发",filesize);
  				}
  			}
  			if(controlcode==1){
  				sprintf(caTemp+strlen(caTemp)," 需审批");
  			}
  			else if(controlcode==2){
  				sprintf(caTemp+strlen(caTemp)," 备份到服务器");
  			}
  			else if(controlcode==3){
  				sprintf(caTemp+strlen(caTemp)," 审批+备份");
  			}
  			if(strlen(ftpname)>0){
  				if(ftpmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止FTP地址类型:[%s]",ftpname);
  				}
  				else if(ftpmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许FTP地址类型:[%s]",ftpname);
  				}
  			}
  			if(strlen(ftpserver)>0){
  				if(ftpservermode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止ftp进程类型[%s]",ftpserver);
  				}
  				else if(ftpservermode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许ftp进程类型[%s]",ftpserver);
  				}
  			
      }
      utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
      
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      memset(chataccount,0,sizeof(chataccount));
      memset(chatsoft,0,sizeof(chatsoft));
      chataccountmode = 0;
      chatsoftmode=0;
     
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
      
     }
      pasDbCloseCursor(psCur);
    } 
  }//else 
  if((!strcmp(filetype_in,"chat"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
      snprintf(sql,sizeof(sql),"delete from nwfilechatprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfilechatprocy(groupid,computerid,company,chatuser,usermode,chatname,chatmode,fileexp,fileexpmode,filesize,controlcode,createtime,modtime,level,chatusertype,chattype,exptype)values(%lu,%lu,'%s','%s',%s,'%s',%s,'%s',%s,%s,%s,%lu,%lu,%d,13,12,7)",groupid,compid,shortname,utf8convert(psShmHead,chataccount_in),chataccountmode_in,utf8convert(psShmHead,chatsoft_in),chatsoftmode_in,utf8convert(psShmHead,fileexp_in),fileexpmode_in,filesize_in,controlcode_in,time(0),time(0),userlevel);
      
      writeLog("fileout.txt","chat",sql); 
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
    	 snprintf(sql,sizeof(sql),"update nwfilechatprocy set flag=1 where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	 snprintf(sql,sizeof(sql),"update nwfilechatprocy set flag=1 where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
    snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.chatuser,a.usermode,a.chatname,a.chatmode,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfilechatprocy as a where company='%s' %s",shortname,sql_tmp);
    printf("sql=%s\n",sql);
    writeLog("fileout.txt","chat",sql);
    
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){      
      if(0!=strcmp(filetype_in,"all")) {iNum = 0;}
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      memset(chataccount,0,sizeof(chataccount));
      memset(chatsoft,0,sizeof(chatsoft));
      chataccountmode = 0;
      chatsoftmode=0;
     
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
      
      while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),      		&sid,
          UT_TYPE_ULONG,4,&groupid,
          UT_TYPE_ULONG,4,&compid,
          UT_TYPE_STRING, sizeof(chataccount)-1, 	chataccount,
          UT_TYPE_ULONG,  sizeof(chataccountmode), &chataccountmode,
          UT_TYPE_STRING, sizeof(chatsoft)-1, 	chatsoft,
          UT_TYPE_ULONG,  sizeof(chatsoftmode),     &chatsoftmode,
          UT_TYPE_STRING, sizeof(fileexp)-1, 	  fileexp,
          UT_TYPE_ULONG,  sizeof(fileexpmode),  &fileexpmode,
          UT_TYPE_ULONG,  sizeof(filesize),     &filesize,
          UT_TYPE_ULONG,  sizeof(controlcode),  &controlcode,
          UT_TYPE_STRING, sizeof(modtime)-1, 		modtime,
          UT_TYPE_ULONG,  sizeof(level),        &level)) || 1405==iret){
            
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  					strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
        utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0163聊天软件外发"));
  			utPltPutLoopVar(psDbHead,"chataccount",iNum,chataccount);
  			utPltPutLoopVarF(psDbHead,"chataccountmode",iNum,"%lu",chataccountmode);
  			utPltPutLoopVar(psDbHead,"chatsoft",iNum,chatsoft);
  			utPltPutLoopVarF(psDbHead,"chatsoftmode",iNum,"%lu",chatsoftmode);
  			utPltPutLoopVar(psDbHead,"fileexp",iNum,fileexp);
  			utPltPutLoopVarF(psDbHead,"fileexpmode",iNum,"%lu",fileexpmode);
  			utPltPutLoopVarF(psDbHead,"filesize",iNum,"%lu",filesize);
  			utPltPutLoopVarF(psDbHead,"controlcode",iNum,"%lu",controlcode);
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level);
  			memset(caTemp,0,sizeof(caTemp));
  			if(strlen(fileexp)>0){
  				if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"禁止文件类型:[%s]",fileexp);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"允许文件类型:[%s]",fileexp);
  				}
  			}
  			if(filesize>0){
  				 if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]禁止外发",filesize);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]允许止外发",filesize);
  				}
  			}
  			if(controlcode==1){
  				sprintf(caTemp+strlen(caTemp)," 需审批");
  			}
  			else if(controlcode==2){
  				sprintf(caTemp+strlen(caTemp)," 备份到服务器");
  			}
  			else if(controlcode==3){
  				sprintf(caTemp+strlen(caTemp)," 审批+备份");
  			}
  			if(strlen(chataccount)>0){
  				if(chataccountmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止聊天帐号类型:[%s]",chataccount);
  				}
  				else if(chataccountmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许聊天帐号类型:[%s]",chataccount);
  				}
  			}
  			if(strlen(chatsoft)>0){
  				if(chatsoftmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止聊天软件类型[%s]",chatsoft);
  				}
  				else if(chatsoftmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许聊天软件类型[%s]",chatsoft);
  				}
  			
      }
//      printf("caTemp=%s\n",caTemp);
      utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
      
      
          sid = 0;
		      groupid=0;
		      compid=0;
		      memset(groupname,0,sizeof(groupname));
		      memset(compname,0,sizeof(compname));
		      memset(chataccount,0,sizeof(chataccount));
		      memset(chatsoft,0,sizeof(chatsoft));
		      chataccountmode = 0;
		      chatsoftmode=0;
		     
		      memset(devname,0,sizeof(devname));
		      memset(caIp,0,sizeof(caIp));
		      httpmode = 0;
		      memset(fileexp,0,sizeof(fileexp));
		      fileexpmode = 0;
		      filesize = 0;
		      controlcode = 0;
		      memset(modtime,0,sizeof(modtime));
		      level = 0;
     }
      pasDbCloseCursor(psCur);
    } 
  }//else 
  if((!strcmp(filetype_in,"cloud"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
      snprintf(sql,sizeof(sql),"delete from nwfilecloudprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfilecloudprocy(groupid,computerid,company,yundisk,yunmode,fileexp,fileexpmode,filesize,controlcode,createtime,modtime,level,yundisktype,exptype)values(%lu,%lu,'%s','%s',%s,'%s',%s,%s,%s,%lu,%lu,%d,14,7)",groupid,compid,shortname,utf8convert(psShmHead,chatsoft_in),chatsoftmode_in,utf8convert(psShmHead,fileexp_in),fileexpmode_in,filesize_in,controlcode_in,time(0),time(0),userlevel);
      
      writeLog("fileout.txt","cloud",sql);
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
    	 snprintf(sql,sizeof(sql),"update nwfilecloudprocy set flag=1 where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	 snprintf(sql,sizeof(sql),"update nwfilecloudprocy set flag=1 where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
    
   snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.yundisk,a.yunmode,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfilecloudprocy as a where company='%s' %s",shortname,sql_tmp);
   
    writeLog("fileout.txt","chat",sql);
    
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){      
    if(0!=strcmp(filetype_in,"all"))  {iNum = 0;}
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      
      memset(cloudsoft,0,sizeof(cloudsoft));
      cloudsoftmode=0;
     
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
      
      while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),      		&sid,
          UT_TYPE_ULONG,4,&groupid,
          UT_TYPE_ULONG,4,&compid,
          UT_TYPE_STRING, sizeof(cloudsoft)-1, 	cloudsoft,
          UT_TYPE_ULONG,  sizeof(cloudsoftmode), &cloudsoftmode,
          UT_TYPE_STRING, sizeof(fileexp)-1, 	  fileexp,
          UT_TYPE_ULONG,  sizeof(fileexpmode),  &fileexpmode,
          UT_TYPE_ULONG,  sizeof(filesize),     &filesize,
          UT_TYPE_ULONG,  sizeof(controlcode),  &controlcode,
          UT_TYPE_STRING, sizeof(modtime)-1, 		modtime,
          UT_TYPE_ULONG,  sizeof(level),        &level)) || 1405==iret){
            
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  					strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
        utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0164云盘外发"));
  			utPltPutLoopVar(psDbHead,"cloudsoft",iNum,cloudsoft);
  			utPltPutLoopVarF(psDbHead,"cloudsoftmode",iNum,"%lu",cloudsoftmode);
  	
  			utPltPutLoopVar(psDbHead,"fileexp",iNum,fileexp);
  			utPltPutLoopVarF(psDbHead,"fileexpmode",iNum,"%lu",fileexpmode);
  			utPltPutLoopVarF(psDbHead,"filesize",iNum,"%lu",filesize);
  			utPltPutLoopVarF(psDbHead,"controlcode",iNum,"%lu",controlcode);
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level);
  			memset(caTemp,0,sizeof(caTemp));
  			if(strlen(fileexp)>0){
  				if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"禁止文件类型:[%s]",fileexp);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"允许文件类型:[%s]",fileexp);
  				}
  			}
  			if(filesize>0){
  				 if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]禁止外发",filesize);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]允许止外发",filesize);
  				}
  			}
  			if(controlcode==1){
  				sprintf(caTemp+strlen(caTemp)," 需审批");
  			}
  			else if(controlcode==2){
  				sprintf(caTemp+strlen(caTemp)," 备份到服务器");
  			}
  			else if(controlcode==3){
  				sprintf(caTemp+strlen(caTemp)," 审批+备份");
  			}
  			if(strlen(cloudsoft)>0){
  				if(cloudsoftmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止云盘类型:[%s]",cloudsoft);
  				}
  				else if(cloudsoftmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许云盘类型:[%s]",cloudsoft);
  				}
  			}
  		
//      printf("caTemp=%s\n",caTemp);
      utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      memset(chataccount,0,sizeof(chataccount));
      memset(chatsoft,0,sizeof(chatsoft));
      chataccountmode = 0;
      chatsoftmode=0;
     
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
      
      
     }
      pasDbCloseCursor(psCur);
    } 
  }
  //else 
  if((!strcmp(filetype_in,"upan"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
      snprintf(sql,sizeof(sql),"delete from nwfileupanprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfileupanprocy(groupid,computerid,company,utype,umode,fileexp,fileexpmode,filesize,controlcode,createtime,modtime,level,exptype)values(%lu,%lu,'%s','%s',%s,'%s',%s,%s,%s,%lu,%lu,%d,7)",groupid,compid,shortname,utf8convert(psShmHead,hostmail_in),hostmailmode_in,utf8convert(psShmHead,fileexp_in),fileexpmode_in,filesize_in,controlcode_in,time(0),time(0),userlevel);
      
      writeLog("fileout.txt","upan",sql);
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
    	 snprintf(sql,sizeof(sql),"update nwfileupanprocy set flag=1 where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	 snprintf(sql,sizeof(sql),"update nwfileupanprocy set flag=1 where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
   // snprintf(sql,sizeof(sql),"select a.sid,nwgroup.gname,nwcompute.devname,a.utype,a.umode,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfileupanprocy as a left join nwgroup on a.groupid=nwgroup.gid left join nwcompute on a.computerid=nwcompute.compid where company='%s' %s",shortname,sql_tmp);
    
     snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.utype,a.umode,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level,b.name from nwfileupanprocy as a,nwusbclass b where a.utype=b.id and company='%s' %s",shortname,sql_tmp);
  
    
    writeLog("fileout.txt","upan",sql);  
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){      
     if(0!=strcmp(filetype_in,"all")) {iNum = 0;}
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      
      memset(cloudsoft,0,sizeof(cloudsoft));
      cloudsoftmode=0;
     
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      memset(uname,0,sizeof(uname));
      utype=0;
      level = 0;
      
      while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),&sid,
          UT_TYPE_ULONG,4,&groupid,
          UT_TYPE_ULONG,4,&compid,
          UT_TYPE_ULONG,4,&utype,
          UT_TYPE_ULONG,4,&umode,
          UT_TYPE_STRING, sizeof(fileexp)-1, 	  fileexp,
          UT_TYPE_ULONG,  sizeof(fileexpmode),  &fileexpmode,
          UT_TYPE_ULONG,  sizeof(filesize),     &filesize,
          UT_TYPE_ULONG,  sizeof(controlcode),  &controlcode,
          UT_TYPE_STRING, sizeof(modtime)-1, 		modtime,
          UT_TYPE_ULONG,  sizeof(level),        &level,
          UT_TYPE_STRING,sizeof(uname),uname)) || 1405==iret){
            
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
  			if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  					strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			utPltPutLoopVarF(psDbHead,"utype",iNum,"%lu",utype);
  			utPltPutLoopVarF(psDbHead,"umode",iNum,"%lu",umode);
  			
  			utPltPutLoopVar(psDbHead,"fileexp",iNum,fileexp);
  			utPltPutLoopVarF(psDbHead,"fileexpmode",iNum,"%lu",fileexpmode);
  			utPltPutLoopVarF(psDbHead,"filesize",iNum,"%lu",filesize);
  			utPltPutLoopVarF(psDbHead,"controlcode",iNum,"%lu",controlcode);
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level);
  			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0165U盘外发"));
  	    memset(caTemp,0,sizeof(caTemp));
  			if(strlen(fileexp)>0){
  				if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"禁止文件类型:[%s]",fileexp);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"允许文件类型:[%s]",fileexp);
  				}
  			}
  			if(filesize>0){
  				 if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]禁止外发",filesize);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]允许止外发",filesize);
  				}
  			}
  			if(controlcode==1){
  				sprintf(caTemp+strlen(caTemp)," 需审批");
  			}
  			else if(controlcode==2){
  				sprintf(caTemp+strlen(caTemp)," 备份到服务器");
  			}
  			else if(controlcode==3){
  				sprintf(caTemp+strlen(caTemp)," 审批+备份");
  			}
  			if(strlen(uname)>0){
  				if(umode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止USB类型:[%s]",uname);
  				}
  				else if(umode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许USB类型:[%s]",uname);
  				}
  			}
  		
      utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
  			
  			      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      
      memset(cloudsoft,0,sizeof(cloudsoft));
      cloudsoftmode=0;
     
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      memset(uname,0,sizeof(uname));
      utype=0;
      level = 0;
      }
      pasDbCloseCursor(psCur);
    } 
  }
  //else 
  if((!strcmp(filetype_in,"share"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
      snprintf(sql,sizeof(sql),"delete from nwfileshareprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfileshareprocy(groupid,computerid,company,fileout,fileread,fileexp,fileexpmode,filesize,controlcode,createtime,modtime,level,exptype)values(%lu,%lu,'%s',%s,%s,'%s',%s,%s,%s,%lu,%lu,%d,7)",groupid,compid,shortname,hostmailmode_in,httpmode_in,utf8convert(psShmHead,fileexp_in),fileexpmode_in,filesize_in,controlcode_in,time(0),time(0),userlevel);
      
      writeLog("fileout.txt","share",sql);
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
    	 snprintf(sql,sizeof(sql),"update nwfileshareprocy set flag=1 where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	 snprintf(sql,sizeof(sql),"update nwfileshareprocy set flag=1 where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
    
    
     snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.fileout,a.fileread,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfileshareprocy as a where company='%s' %s",shortname,sql_tmp);
  
    
    writeLog("fileout.txt","share",sql);  
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){      
     if(0!=strcmp(filetype_in,"all")) {iNum = 0;}
      sid = 0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      fileout=0;
      fileread=0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
      
         while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),&sid,
          UT_TYPE_ULONG,4,&groupid,
          UT_TYPE_ULONG,4,&compid,
          UT_TYPE_ULONG,4,&fileout,
          UT_TYPE_ULONG,4,&fileread,
          UT_TYPE_STRING, sizeof(fileexp)-1, 	  fileexp,
          UT_TYPE_ULONG,  sizeof(fileexpmode),  &fileexpmode,
          UT_TYPE_ULONG,  sizeof(filesize),     &filesize,
          UT_TYPE_ULONG,  sizeof(controlcode),  &controlcode,
          UT_TYPE_STRING, sizeof(modtime)-1, 		modtime,
          UT_TYPE_ULONG,  sizeof(level),        &level)) || 1405==iret){
            
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  					strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
  			utPltPutLoopVarF(psDbHead,"fileout",iNum,"%lu",fileout);
  			utPltPutLoopVarF(psDbHead,"fileread",iNum,"%lu",fileread);
  			utPltPutLoopVar(psDbHead,"fileexp",iNum,fileexp);
  			utPltPutLoopVarF(psDbHead,"fileexpmode",iNum,"%lu",fileexpmode);
  			utPltPutLoopVarF(psDbHead,"filesize",iNum,"%lu",filesize);
  			utPltPutLoopVarF(psDbHead,"controlcode",iNum,"%lu",controlcode);
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level);
  			
  			
  			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0166共享目录外发"));
  	    memset(caTemp,0,sizeof(caTemp));
  			if(strlen(fileexp)>0){
  				if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"禁止文件类型:[%s]",fileexp);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"允许文件类型:[%s]",fileexp);
  				}
  			}
  			if(filesize>0){
  				 if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]禁止外发",filesize);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]允许止外发",filesize);
  				}
  			}
  			if(controlcode==1){
  				sprintf(caTemp+strlen(caTemp)," 需审批");
  			}
  			else if(controlcode==2){
  				sprintf(caTemp+strlen(caTemp)," 备份到服务器");
  			}
  			else if(controlcode==3){
  				sprintf(caTemp+strlen(caTemp)," 审批+备份");
  			}
  
  				if(fileout==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止共享目录外发文件");
  				}
  				if(fileread==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止共享目录读文件");
  				}
  				
  			
  		
      utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
  			
  			
  			sid = 0;
	      memset(groupname,0,sizeof(groupname));
	      memset(compname,0,sizeof(compname));
	      memset(devname,0,sizeof(devname));
	      memset(caIp,0,sizeof(caIp));
	      fileout=0;
	      fileread=0;
	      compid=0;
	      groupid=0;
	      memset(fileexp,0,sizeof(fileexp));
	      fileexpmode = 0;
	      filesize = 0;
	      controlcode = 0;
	      memset(modtime,0,sizeof(modtime));
	      level = 0;
      }
      pasDbCloseCursor(psCur);
    } 
  }//else 
  if((!strcmp(filetype_in,"https"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
      snprintf(sql,sizeof(sql),"delete from nwfilehttpsprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfilehttpsprocy(groupid,computerid,company,httpsip,httpsipmode,httpssoft,httpsmode,fileexp,fileexpmode,filesize,controlcode,createtime,modtime,level,exptype)values(%lu,%lu,'%s','%s',%s,'%s',%s,'%s',%s,%s,%s,%lu,%lu,%d,7)",groupid,compid,shortname,utf8convert(psShmHead,hostmail_in),hostmailmode_in,utf8convert(psShmHead,httpname_in),httpmode_in,utf8convert(psShmHead,fileexp_in),fileexpmode_in,filesize_in,controlcode_in,time(0),time(0),userlevel);
      
      writeLog("fileout.txt","https",sql);
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
    	 snprintf(sql,sizeof(sql),"update nwfilehttpsprocy set flag=1 where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	 snprintf(sql,sizeof(sql),"update nwfilehttpsprocy set flag=1 where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
    
    snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.httpsip,a.httpsipmode,a.httpssoft,a.httpsmode,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfilehttpsprocy as a where company='%s' %s",shortname,sql_tmp);
    printf("sql=%s\n",sql);
    writeLog("fileout.txt","http",sql);
    
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){      
     if(0!=strcmp(filetype_in,"all")) {iNum = 0;}
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      memset(hostmail,0,sizeof(hostmail));
      hostmailmode = 0;
      memset(httpname,0,sizeof(httpname));
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      level = 0;
      
      while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),      		&sid,
          UT_TYPE_ULONG,4,&groupid,
          UT_TYPE_ULONG,4,&compid,
          UT_TYPE_STRING, sizeof(hostmail)-1, 	hostmail,
          UT_TYPE_ULONG,  sizeof(hostmailmode), &hostmailmode,
          UT_TYPE_STRING, sizeof(httpname)-1, 	httpname,
          UT_TYPE_ULONG,  sizeof(httpmode),     &httpmode,
          UT_TYPE_STRING, sizeof(fileexp)-1, 	  fileexp,
          UT_TYPE_ULONG,  sizeof(fileexpmode),  &fileexpmode,
          UT_TYPE_ULONG,  sizeof(filesize),     &filesize,
          UT_TYPE_ULONG,  sizeof(controlcode),  &controlcode,
          UT_TYPE_STRING, sizeof(modtime)-1, 		modtime,
          UT_TYPE_ULONG,  sizeof(level),        &level)) || 1405==iret){
            
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  					strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
        utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0167Https外发"));
  			utPltPutLoopVar(psDbHead,"hostmail",iNum,hostmail);
  			utPltPutLoopVarF(psDbHead,"hostmailmode",iNum,"%lu",hostmailmode);
  			utPltPutLoopVar(psDbHead,"httpname",iNum,httpname);
  			utPltPutLoopVarF(psDbHead,"httpmode",iNum,"%lu",httpmode);
  			utPltPutLoopVar(psDbHead,"fileexp",iNum,fileexp);
  			utPltPutLoopVarF(psDbHead,"fileexpmode",iNum,"%lu",fileexpmode);
  			utPltPutLoopVarF(psDbHead,"filesize",iNum,"%lu",filesize);
  			utPltPutLoopVarF(psDbHead,"controlcode",iNum,"%lu",controlcode);
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level);
  			memset(caTemp,0,sizeof(caTemp));
  			if(strlen(fileexp)>0){
  				if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"禁止文件类型:[%s]",fileexp);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"允许文件类型:[%s]",fileexp);
  				}
  			}
  			if(filesize>0){
  				 if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]禁止外发",filesize);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]允许止外发",filesize);
  				}
  			}
  			if(controlcode==1){
  				sprintf(caTemp+strlen(caTemp)," 需审批");
  			}
  			else if(controlcode==2){
  				sprintf(caTemp+strlen(caTemp)," 备份到服务器");
  			}
  			else if(controlcode==3){
  				sprintf(caTemp+strlen(caTemp)," 审批+备份");
  			}
  			if(strlen(hostmail)>0){
  				if(hostmailmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止https地址类型:[%s]",hostmail);
  				}
  				else if(hostmailmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许https地址类型:[%s]",hostmail);
  				}
  			}
  			if(strlen(httpname)>0){
  				if(httpmode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止HTTPS软件类型[%s]",httpname);
  				}
  				else if(httpmode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许HTTPS软件类型[%s]",httpname);
  				}
  			
      }
      utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
     }
      pasDbCloseCursor(psCur);
    } 
  }
 // else
 if((!strcmp(filetype_in,"other"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
      snprintf(sql,sizeof(sql),"delete from nwfileotherprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfileotherprocy(groupid,computerid,company,otherapp,bkother,fileexp,fileexpmode,filesize,controlcode,createtime,modtime,level,exptype)values(%lu,%lu,'%s','%s',%s,'%s',%s,%s,%s,%lu,%lu,%d,7)",groupid,compid,shortname,utf8convert(psShmHead,hostmail_in),hostmailmode_in,utf8convert(psShmHead,fileexp_in),fileexpmode_in,filesize_in,controlcode_in,time(0),time(0),userlevel);
      
      writeLog("fileout.txt","upan",sql);
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
    	 snprintf(sql,sizeof(sql),"update nwfileotherprocy set flag=1 where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	 snprintf(sql,sizeof(sql),"update nwfileotherprocy set flag=1 where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
   // snprintf(sql,sizeof(sql),"select a.sid,nwgroup.gname,nwcompute.devname,a.utype,a.umode,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfileupanprocy as a left join nwgroup on a.groupid=nwgroup.gid left join nwcompute on a.computerid=nwcompute.compid where company='%s' %s",shortname,sql_tmp);
    
     snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.otherapp,a.bkother,a.fileexp,a.fileexpmode,a.filesize,a.controlcode,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfileotherprocy as a where company='%s' %s",shortname,sql_tmp);
  
    
    writeLog("fileout.txt","upan",sql);  
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){      
    if(0!=strcmp(filetype_in,"all"))  {iNum = 0;}
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      
      memset(cloudsoft,0,sizeof(cloudsoft));
      cloudsoftmode=0;
     
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      memset(uname,0,sizeof(uname));
      utype=0;
      level = 0;
      
      while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),&sid,
          UT_TYPE_ULONG,4,&groupid,
          UT_TYPE_ULONG,4,&compid,
          UT_TYPE_STRING,sizeof(uname),uname,
          UT_TYPE_ULONG,4,&umode,
          UT_TYPE_STRING, sizeof(fileexp)-1, 	  fileexp,
          UT_TYPE_ULONG,  sizeof(fileexpmode),  &fileexpmode,
          UT_TYPE_ULONG,  sizeof(filesize),     &filesize,
          UT_TYPE_ULONG,  sizeof(controlcode),  &controlcode,
          UT_TYPE_STRING, sizeof(modtime)-1, 		modtime,
          UT_TYPE_ULONG,  sizeof(level),&level)) || 1405==iret){
            
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
  			if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  					strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			utPltPutLoopVarF(psDbHead,"utype",iNum,"%s",uname);
  			utPltPutLoopVarF(psDbHead,"umode",iNum,"%lu",umode);
  			
  			utPltPutLoopVar(psDbHead,"fileexp",iNum,fileexp);
  			utPltPutLoopVarF(psDbHead,"fileexpmode",iNum,"%lu",fileexpmode);
  			utPltPutLoopVarF(psDbHead,"filesize",iNum,"%lu",filesize);
  			utPltPutLoopVarF(psDbHead,"controlcode",iNum,"%lu",controlcode);
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level);
  			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0168其它软件外发"));
  	    memset(caTemp,0,sizeof(caTemp));
  			if(strlen(fileexp)>0){
  				if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"禁止文件类型:[%s]",fileexp);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"允许文件类型:[%s]",fileexp);
  				}
  			}
  			if(filesize>0){
  				 if(fileexpmode==1){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]禁止外发",filesize);
  				}
  				else if(fileexpmode==2){
  					sprintf(caTemp+strlen(caTemp),"大小超过:[%d]允许止外发",filesize);
  				}
  			}
  			if(controlcode==1){
  				sprintf(caTemp+strlen(caTemp)," 需审批");
  			}
  			else if(controlcode==2){
  				sprintf(caTemp+strlen(caTemp)," 备份到服务器");
  			}
  			else if(controlcode==3){
  				sprintf(caTemp+strlen(caTemp)," 审批+备份");
  			}
  			if(strlen(uname)>0){
  				if(umode==1){
  					sprintf(caTemp+strlen(caTemp)," 禁止软件类型:[%s]",uname);
  				}
  				else if(umode==2){
  					sprintf(caTemp+strlen(caTemp)," 允许软件类型:[%s]",uname);
  				}
  			}
  		
      utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
  			
  	  sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      
      memset(cloudsoft,0,sizeof(cloudsoft));
      cloudsoftmode=0;
     
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      httpmode = 0;
      memset(fileexp,0,sizeof(fileexp));
      fileexpmode = 0;
      filesize = 0;
      controlcode = 0;
      memset(modtime,0,sizeof(modtime));
      memset(uname,0,sizeof(uname));
      utype=0;
      level = 0;
      }
      pasDbCloseCursor(psCur);
    } 
  }
 // else 
 if((!strcmp(filetype_in,"freeblock"))||0==strcmp(filetype_in,"all")){
    if(!strcmp(deal_in,"insert")){
      snprintf(sql,sizeof(sql),"delete from nwfileblockfreeprocy where groupid=%lu and computerid=%lu and company='%s'",groupid,compid,shortname);
      pasDbExecSqlF(sql);
      snprintf(sql,sizeof(sql),"insert nwfileblockfreeprocy(groupid,computerid,company,freeapptype,freeapp,freeservertype,freeserver,blockapptype,blockapp,blockservertype,blockserver,createtime,modtime,level,createuser) \
      values(%lu,%lu,'%s',32,'%s',33,'%s',34,'%s',35,'%s',%lu,%lu,%d,'%s')",\
      groupid,compid,shortname,utf8convert(psShmHead,hostmail_in),utf8convert(psShmHead,smtpsoft_in),utf8convert(psShmHead,mailsend_in),utf8convert(psShmHead,mailRev_in),time(0),time(0),userlevel,shortname);
      writeLog("fileout.txt","smtp",sql); 
      pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delete")){
    	snprintf(sql,sizeof(sql),"update nwfileblockfreeprocy set flag=1 where sid=%s ",recno_in);
//    	 snprintf(sql,sizeof(sql),"delete from nwfilesmtpprocy where sid=%s ",recno_in);
       pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in,"delall")){
    	snprintf(sql,sizeof(sql),"update nwfileblockfreeprocy set flag=1 where sid in (%s) ",caIds);
 //   	 snprintf(sql,sizeof(sql),"delete from nwfilesmtpprocy where sid in (%s) ",caIds);
       pasDbExecSqlF(sql);
    }
    
  
     snprintf(sql,sizeof(sql),"select a.sid,a.groupid,a.computerid,a.freeApp,a.freeServer,a.blockApp,a.blockServer,from_unixtime(a.modtime,'%%Y/%%m/%%d %%H:%%I:%%S'),a.level from nwfileblockfreeprocy as a where company='%s' %s",shortname,sql_tmp);
    char caFreeapp[32],caFreeserver[32],caBlockapp[32],caBlockserver[32];
    printf("sql=%s\n",sql);
    writeLog("fileout.txt","smtp",sql); 
    psCur = pasDbOpenSql(sql,0);
    if(psCur != NULL){      
	  if(0!=strcmp(filetype_in,"all"))  
	    {
		  iNum = 0;
		 }
	 
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      memset(caFreeapp,0,sizeof(caFreeapp));
      memset(caFreeserver,0,sizeof(caFreeserver));
      memset(caBlockapp,0,sizeof(caBlockapp));
      memset(caBlockserver,0,sizeof(caBlockserver));
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      memset(modtime,0,sizeof(modtime));
      level = 0;
      while(0==(iret = pasDbFetchInto(psCur,
          UT_TYPE_ULONG,  sizeof(sid),      		&sid,
          UT_TYPE_ULONG,4,&groupid,
          UT_TYPE_ULONG,4,&compid,
          UT_TYPE_STRING, sizeof(caFreeapp)-1, 	caFreeapp,
          UT_TYPE_STRING, sizeof(caFreeserver)-1, 	caFreeserver,
          UT_TYPE_STRING, sizeof(caBlockapp)-1, 	  caBlockapp,
          UT_TYPE_STRING, sizeof(caBlockserver)-1, caBlockserver,
          UT_TYPE_STRING,sizeof(modtime)-1,modtime,
          UT_TYPE_LONG,4,&level
         )) || 1405==iret){
		//printf("iNum=%d\n",iNum);
		//printf("CaBlockapp=%s\n",caBlockapp);
        iNum++;
  			if(iNum>1){
  					utPltPutLoopVar(psDbHead,"dh",iNum,",");
  			}
  			
  			 if(compid>0){
  				psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
  				if(psComp){
  					strcpy(groupname,lanGetGroup(psShmHead,psComp->groupid));
  					strcpy(devname,psComp->devname);
  					strcpy(caIp,utComHostIp(htonl(psComp->ip)));
  				}
  			}
  			else if(groupid>0){
  				strcpy(groupname,lanGetGroup(psShmHead,groupid));
  			}
  			utPltPutLoopVar(psDbHead,"devname",iNum,devname);  			
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"ip",iNum,caIp);
  			utPltPutLoopVarF(psDbHead,"levelname",iNum,"%lu",level); 
  			utPltPutLoopVar(psDbHead,"procytype",iNum,ncLang("0169放行和禁止"));
  			
  			
  			utPltPutLoopVarF(psDbHead,"recno",iNum,"%lu",sid);
  			utPltPutLoopVar(psDbHead,"groupname",iNum,groupname);
  			utPltPutLoopVar(psDbHead,"compname",iNum,compname);
  			
  			utPltPutLoopVar(psDbHead,"freeapp",iNum,caFreeapp);
  	
  			utPltPutLoopVar(psDbHead,"freeserver",iNum,caFreeserver);
  			utPltPutLoopVar(psDbHead,"blockapp",iNum,caBlockapp);
  			utPltPutLoopVar(psDbHead,"blockserver",iNum,caBlockserver);
  			
  			utPltPutLoopVar(psDbHead,"modtime",iNum,modtime);
  			utPltPutLoopVarF(psDbHead,"level",iNum,"%lu",level);
  			
  			memset(caTemp,0,sizeof(caTemp));
  			if(strlen(caFreeapp)>0){
  				
  				sprintf(caTemp+strlen(caTemp),"放行软件类型:[%s]",caFreeapp);
  				
  			}
  			if(strlen(caFreeserver)>0){
  				sprintf(caTemp+strlen(caTemp),"放行服务器:[%s]",caFreeserver);
  			}
  			
  			
  			if(strlen(caBlockapp)>0){  			
  					sprintf(caTemp+strlen(caTemp)," 禁止软件类型:[%s]",caBlockapp);  				
  			}
  			if(strlen(caBlockserver)>0){
  				sprintf(caTemp+strlen(caTemp)," 禁止服务器:[%s]",caBlockserver);
  			}
  		
  		
  				utPltPutLoopVar(psDbHead,"controlcode_desc",iNum,caTemp);
				//printf("caTemp=%s\n",caTemp);
  		
      sid = 0;
      groupid=0;
      compid=0;
      memset(groupname,0,sizeof(groupname));
      memset(compname,0,sizeof(compname));
      memset(caFreeapp,0,sizeof(caFreeapp));
      memset(caFreeserver,0,sizeof(caFreeserver));
      memset(caBlockapp,0,sizeof(caBlockapp));
      memset(caBlockserver,0,sizeof(caBlockserver));
      memset(devname,0,sizeof(devname));
      memset(caIp,0,sizeof(caIp));
      memset(modtime,0,sizeof(modtime));
      level = 0;
  		
      }
      pasDbCloseCursor(psCur);
    } 
  }
 // }
  
  

 
  
  //utPltShowDb(psDbHead);
  utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_fileout_list.htm");
  return 0;
}
