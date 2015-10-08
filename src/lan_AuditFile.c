#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>
#include "string.h"
#include "utoall.h"
#include "utomon.h"
#include "ncdef.h"
#include "nwdef.h"
#include "pasdb.h"
#include "lan_md5.h"
#include <stdlib.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>


int8 pasGetLongTimeId();
int  GetAuditionCondition(char *compname, int applytime,int auditiontime,int auditresult,int auditorsel,char * auditor, char * sql, int sqllen);


int  Lan_GetAuditionUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		nameid[128];
	char		nickname[128];
	char		passwd[128];
	char		mobno[128];
	char		mail[128];
	char		qqno[128];
	char		grouplist[1024];
	int		heartbeattime=0;

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(nameid, 0, sizeof(nameid));
	memset(nickname, 0, sizeof(nickname));
	memset(passwd, 0, sizeof(passwd));
	memset(mobno, 0, sizeof(mobno));
	memset(mail, 0, sizeof(mail));
	memset(qqno, 0, sizeof(qqno));
	memset(grouplist, 0, sizeof(grouplist));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,1,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname);
	if( (1 != iReturn)||
	    (0==strlen(compname))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s",  iReturn,  compname);
		recodeLogInfo(" 获取审批用户列表","  接收参数失败!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	snprintf(sql , sizeof(sql)-1,  "select nameid, nickname, passwd, mobno, mail, qqno, grouplist"
		" from  nwAuditFileUser  where  compname=\'%s\'",  compname);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //查询数据库失败，记录日志
		recodeLogInfo(" 获取审批用户列表"," 查询审批用户表失败!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		int 	iReturn =0;
		int 	num=0;	
		char	str[8][24];
		memset(str, 0, sizeof(str));

		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 0);

		while(iReturn == 0){
			memset(nameid, 0, sizeof(nameid));
			iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(nameid)-1, nameid,
							UT_TYPE_STRING, sizeof(nickname)-1, nickname,
							UT_TYPE_STRING, sizeof(passwd)-1, passwd,
							UT_TYPE_STRING, sizeof(mobno)-1, mobno,
							UT_TYPE_STRING, sizeof(mail)-1, mail,
							UT_TYPE_STRING, sizeof(qqno)-1, qqno,
							UT_TYPE_STRING, sizeof(grouplist)-1, grouplist);
			if( (iReturn != 0)){
				continue;
			}

			snprintf(str[0], sizeof(str[0])-1, "nameid%d", num);
			snprintf(str[1], sizeof(str[0])-1, "nickname%d", num);
			snprintf(str[2], sizeof(str[0])-1, "passwd%d", num);
			snprintf(str[3], sizeof(str[0])-1, "mobno%d", num);
			snprintf(str[4], sizeof(str[0])-1, "mail%d", num);
			snprintf(str[5], sizeof(str[0])-1, "qqno%d", num);
			snprintf(str[6], sizeof(str[0])-1, "grouplist%d", num);
			num++;

			utMsgAdd_s(&psMsgHead, str[0],  nameid);
			utMsgAdd_s(&psMsgHead, str[1],  nickname);
			utMsgAdd_s(&psMsgHead, str[2],  passwd);
			utMsgAdd_s(&psMsgHead, str[3],  mobno);
			utMsgAdd_s(&psMsgHead, str[4],  mail);
			utMsgAdd_s(&psMsgHead, str[5],  qqno);
			utMsgAdd_s(&psMsgHead, str[6],  grouplist);

			//snprintf(mesg, sizeof(mesg)-1, "  num:%d, iReturn:%d,  nameid:%s",  num, iReturn, nameid);
			//recodeLogInfo(" 获取审批用户列表", mesg, sql);
		}
		pasDbCloseCursor(psCur);
		utMsgAdd_l(&psMsgHead, "state",  1);
		utMsgAdd_l(&psMsgHead, "number",  num);
		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}

	return 0;
}

int  Lan_AddAuditionUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		nameid[128];
	char		nickname[128];
	char		passwd[128];
	char		mobno[128];
	char		mail[128];
	char		qqno[128];
	char		grouplist[1024];

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(nameid, 0, sizeof(nameid));
	memset(nickname, 0, sizeof(nickname));
	memset(passwd, 0, sizeof(passwd));
	memset(mobno, 0, sizeof(mobno));
	memset(mail, 0, sizeof(mail));
	memset(qqno, 0, sizeof(qqno));
	memset(grouplist, 0, sizeof(grouplist));

	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));
	

	int iReturn = utMsgGetSomeNVar(psMsgHead,8,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
								 "nameid", UT_TYPE_STRING, sizeof(nameid)-1, nameid,
								 "nickname", UT_TYPE_STRING, sizeof(nickname)-1, nickname,
								 "passwd", UT_TYPE_STRING, sizeof(passwd)-1, passwd,
								 "mobno", UT_TYPE_STRING, sizeof(mobno)-1, mobno,
								 "mail", UT_TYPE_STRING, sizeof(mail)-1, mail,
								 "qqno", UT_TYPE_STRING, sizeof(qqno)-1, qqno,
								 "grouplist", UT_TYPE_STRING, sizeof(grouplist)-1, grouplist);
	if( (8 != iReturn)||
	    (0==strlen(compname))||
	    (0==strlen(nameid))||
	    (0==strlen(nickname))||
	    (0==strlen(passwd))||
	    (0==strlen(grouplist))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, nameid:%s, nickname:%s, passwd:%s, grouplist:%s",  
			iReturn,  compname, nameid, nickname, passwd, grouplist);
		recodeLogInfo(" 增加审批用户","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}
	
	snprintf(sql, sizeof(sql)-1, "insert into nwAuditFileUser(nameid, nickname, passwd, mobno, mail, qqno, grouplist, compname) "
		" values(\'%s\', \'%s\', \'%s\', \'%s\',\'%s\',\'%s\',\'%s\',\'%s\') ",
		nameid, nickname, passwd, mobno, mail, qqno, grouplist, compname);

	if(pasDbExecSql(sql, 0)){
		recodeLogInfo(" 增加审批用户", "ERROR: " , sql);
		
		int  count=0;
		snprintf(sql, sizeof(sql)-1, "select count(*) from nwAuditFileUser where nameid=\'%s\' and compname = \'%s\' ",  nameid,  compname);
		pasDbOneRecord(sql,0,UT_TYPE_LONG,4,&count);
		if(count >0 ){
			recodeLogInfo(" 增加审批用户", "该用户已存在: " , sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,2);
			return -3;
		}

		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-3);
		return -3;
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,1);
	return 0;
}

int  Lan_DeleteAuditionUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		nameid[128];

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(nameid, 0, sizeof(nameid));

	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));
	

	int iReturn = utMsgGetSomeNVar(psMsgHead,2,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
								 "nameid", UT_TYPE_STRING, sizeof(nameid)-1, nameid);
	if( (2 != iReturn)||
	    (0==strlen(compname))||
	    (0==strlen(nameid))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, nameid:%s",  iReturn,  compname);
		recodeLogInfo(" 删除审批用户","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}
			
	int  count=0;
	snprintf(sql, sizeof(sql)-1, "delete from nwAuditFileUser where nameid=\'%s\' and compname = \'%s\' ",  nameid,  compname);
	if(pasDbExecSql(sql, 0)){
		recodeLogInfo(" 删除审批用户", "ERROR: " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-3);
		return 0;
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,1);
	return 0;

}

int  Lan_DropAuditionUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		nameid[128];
	char		nickname[128];
	char		passwd[128];
	char		mobno[128];
	char		mail[128];
	char		qqno[128];
	char		grouplist[1024];

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(nameid, 0, sizeof(nameid));
	memset(nickname, 0, sizeof(nickname));
	memset(passwd, 0, sizeof(passwd));
	memset(mobno, 0, sizeof(mobno));
	memset(mail, 0, sizeof(mail));
	memset(qqno, 0, sizeof(qqno));
	memset(grouplist, 0, sizeof(grouplist));

	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));
	

	int iReturn = utMsgGetSomeNVar(psMsgHead,8,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
								 "nameid", UT_TYPE_STRING, sizeof(nameid)-1, nameid,
								 "nickname", UT_TYPE_STRING, sizeof(nickname)-1, nickname,
								 "passwd", UT_TYPE_STRING, sizeof(passwd)-1, passwd,
								 "mobno", UT_TYPE_STRING, sizeof(mobno)-1, mobno,
								 "mail", UT_TYPE_STRING, sizeof(mail)-1, mail,
								 "qqno", UT_TYPE_STRING, sizeof(qqno)-1, qqno,
								 "grouplist", UT_TYPE_STRING, sizeof(grouplist)-1, grouplist);
	if( (8 != iReturn)||
	    (0==strlen(compname))||
	    (0==strlen(nameid))||
	    (0==strlen(nickname))||
	    (0==strlen(passwd))||
	    (0==strlen(grouplist))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, nameid:%s, nickname:%s, passwd:%s, grouplist:%s",  
			iReturn,  compname, nameid, nickname, passwd, grouplist);
		recodeLogInfo(" 修改审批用户","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}
	
	snprintf(sql, sizeof(sql)-1, "update nwAuditFileUser set nickname=\'%s\', passwd=\'%s\', mobno=\'%s\', mail=\'%s\', qqno=\'%s\', grouplist=\'%s\' "
		" where nameid=\'%s\' and  compname=\'%s\' ",
		nickname, passwd, mobno, mail, qqno, grouplist, nameid, compname);

	if(pasDbExecSql(sql, 0)){
		recodeLogInfo(" 修改审批用户", "ERROR: " , sql);
		
		int  count=0;
		snprintf(sql, sizeof(sql)-1, "select count(*) from nwAuditFileUser where nameid=\'%s\' and compname = \'%s\' ",  nameid,  compname);
		pasDbOneRecord(sql,0,UT_TYPE_LONG,4,&count);
		if(count <= 0 ){
			recodeLogInfo(" 修改审批用户", "该用户不存在: " , sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,2);
			return -3;
		}

		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-3);
		return -3;
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂?辈挥? */ 1,"state",   UT_TYPE_LONG,1);
	return 0;

}

int  Lan_AuditionUserExist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		nameid[128];

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(nameid, 0, sizeof(nameid));

	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));
	

	int iReturn = utMsgGetSomeNVar(psMsgHead,2,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
								 "nameid", UT_TYPE_STRING, sizeof(nameid)-1, nameid);
	if( (2 != iReturn)||
	    (0==strlen(compname))||
	    (0==strlen(nameid))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, nameid:%s",  iReturn,  compname);
		recodeLogInfo(" 判断审批用户是否存在","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}
			
	int  count=0;
	snprintf(sql, sizeof(sql)-1, "select count(*) from nwAuditFileUser where nameid=\'%s\' and compname = \'%s\' ",  nameid,  compname);
	pasDbOneRecord(sql,0,UT_TYPE_LONG,4,&count);
	if(count >0 ){
		recodeLogInfo(" 判断审批用户是否存在", "该用户存在: " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,1);
		return 0;
	}

	recodeLogInfo(" 判断审批用户是否存在", "该用户不存在: " , sql);
	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,2);
	return 0;
}

int  Lan_GetAuditionList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		file[128];
	char		path[256];
	char		md5[128];
	unsigned int	compid = 0;
	char		computername[128];
	int 		groupid=0;
	char		groupname[128];
	char		nickname[128];
	char		mynickname[128];
	unsigned int	applytime = 0;
	unsigned int	audittime = 0;
	int		auditresult= 0;
	char		auditor[128];
	int		auditorsel=0;
	char		auditopinion[128];
	char		auditornickname[128];

	char		limit[2048];
	char		limit2[2048];
	char		sql[2048];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(file, 0, sizeof(file));
	memset(path, 0, sizeof(path));
	memset(md5, 0, sizeof(md5));
	memset(computername, 0, sizeof(computername));
	memset(groupname, 0, sizeof(groupname));
	memset(nickname, 0, sizeof(nickname));
	memset(mynickname, 0, sizeof(mynickname));
	memset(auditor, 0, sizeof(auditor));
	memset(auditopinion, 0, sizeof(auditopinion));
	memset(auditornickname, 0, sizeof(auditornickname));
	memset(limit, 0, sizeof(limit));
	memset(limit2, 0, sizeof(limit2));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,8,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							"compid", UT_TYPE_ULONG, 4, &compid,
							"groupid", UT_TYPE_LONG, 4, &groupid,
							"applytime", UT_TYPE_LONG, 4, &applytime,
							"audittime", UT_TYPE_LONG, 4, &audittime,
							"auditresult", UT_TYPE_LONG, 4, &auditresult,
							"auditorsel", UT_TYPE_LONG, 4, &auditorsel,
							"auditor", UT_TYPE_STRING, sizeof(auditor)-1, auditor);
	if( (8 != iReturn)||
	    (0==strlen(compname))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, compid:%u, groupid:%d, applytime:%d,  audittime:%d, auditresult:%d, auditor:%s",  
			iReturn,  compname, compid,  groupid,  applytime,  audittime, auditresult,  auditor);
		recodeLogInfo(" 获取审批清单","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	snprintf(limit,  sizeof(limit)-1, " 1=1  ");
	if(GetAuditionCondition(compname, applytime, audittime, auditresult, auditorsel, auditor,  limit, sizeof(limit)-1) <0){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, compid:%u, groupid:%d, applytime:%d,  audittime:%d, auditresult:%d, auditor:%s",  
			iReturn,  compname, compid,  groupid,  applytime,  audittime, auditresult,  auditor);
		recodeLogInfo(" 获取审批清单","  解析参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	recodeLogInfo(" 获取审批清单  limit:",  limit, "");


	snprintf(limit2,  sizeof(limit2)-1, " 1=1 ");
	if(compid > 0){
		snprintf(limit2, sizeof(limit2)-1,  " nwcompute.compid=%u",  compid);		
	}else if(groupid > 0){
		snprintf(limit2,  sizeof(limit2)-1, " ( 1=0  ");
		snprintf(limit2+strlen(limit2),  sizeof(limit2)-strlen(limit2)-1,  " or groupid= %u",  groupid);
		GetChildGroup(groupid,  limit2, sizeof(limit2)-strlen(limit2)-1);
		snprintf(limit2+strlen(limit2),  sizeof(limit2)-strlen(limit2)-1, " ) ");
	}

	//recodeLogInfo(" 获取审批清单"," 查询审批用户表limit2!", limit2);

	snprintf(sql, sizeof(sql)-1, "select  file, path, MD5,  nwAuditFileAudition.compid,  applytime, audittime, auditresult, auditor, auditcontent ,"
		" nwcompute.compname,  nwcompute.devname,  nwcompute.groupid, nwgroup.gname,  nwAuditFileNickname.nickname , nwAuditFileUser.nickname"
		" from (nwAuditFileAudition inner join  (nwcompute inner join nwgroup on  (nwcompute.groupid = nwgroup.gid  and %s)) "
		" on (nwAuditFileAudition.compid = nwcompute.compid  and nwAuditFileAudition.compname = \'%s\' and  %s)) "
		" left join  nwAuditFileNickname on (nwAuditFileAudition.compid = nwAuditFileNickname.compid )"
		" left join  nwAuditFileUser  on(nwAuditFileAudition.auditor = nwAuditFileUser.nameid) "
		" group by file, path, MD5, nwAuditFileAudition.compid,  applytime, audittime, auditresult, auditor, auditcontent  ,nwcompute.compname,  nwcompute.devname,  nwcompute.groupid, nwgroup.gname,  nwAuditFileNickname.nickname ,nwAuditFileUser.nickname "
		" order by  nwAuditFileAudition.sid  desc" ,
		limit2,  compname ,limit);

	recodeLogInfo(" 获取审批清单"," 查询审批用户表!", sql);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //查询数据库失败，记录日志
		recodeLogInfo(" 获取审批清单"," 查询审批用户表失败!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		int 	iReturn =0;
		int 	num=0;	
		char	str[24][48];
		memset(str, 0, sizeof(str));

		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 0);

		while( (iReturn == 0) || (1405 == iReturn)){
			memset(file, 0, sizeof(file));
			memset(mynickname,  0, sizeof(mynickname));
			iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(file)-1, file,
							UT_TYPE_STRING, sizeof(path)-1, path,
							UT_TYPE_STRING, sizeof(md5)-1, md5,
							UT_TYPE_ULONG, 4, &compid,
							UT_TYPE_ULONG, 4, &applytime,
							UT_TYPE_ULONG, 4, &audittime,
							UT_TYPE_LONG, 4, &auditresult,
							UT_TYPE_STRING, sizeof(auditor)-1, auditor,
							UT_TYPE_STRING, sizeof(auditopinion)-1, auditopinion,
							UT_TYPE_STRING, sizeof(computername)-1, computername,
							UT_TYPE_STRING, sizeof(nickname)-1, nickname,
							UT_TYPE_LONG, 4, &groupid,
							UT_TYPE_STRING, sizeof(groupname)-1, groupname,
							UT_TYPE_STRING, sizeof(mynickname)-1, mynickname,
							UT_TYPE_STRING, sizeof(auditornickname)-1, auditornickname);
			if( (0 != iReturn) && (1405 != iReturn) ){
				snprintf(mesg, sizeof(mesg)-1, "iReturn :%d", iReturn);
				recodeLogInfo(" 获取审批清单"," 查询审批用户表!", mesg);
				continue;
			}

			snprintf(str[0], sizeof(str[0])-1, "file%d", num);
			snprintf(str[1], sizeof(str[0])-1, "path%d", num);
			snprintf(str[2], sizeof(str[0])-1, "md5%d", num);
			snprintf(str[3], sizeof(str[0])-1, "compid%d", num);
			snprintf(str[4], sizeof(str[0])-1, "computername%d", num);
			snprintf(str[5], sizeof(str[0])-1, "nickname%d", num);
			snprintf(str[6], sizeof(str[0])-1, "groupid%d", num);
			snprintf(str[7], sizeof(str[0])-1, "groupname%d", num);
			snprintf(str[8], sizeof(str[0])-1, "auditor%d", num);
			snprintf(str[9], sizeof(str[0])-1, "auditresult%d", num);
			snprintf(str[10], sizeof(str[0])-1, "audittime%d", num);
			snprintf(str[11], sizeof(str[0])-1, "auditopinion%d", num);
			snprintf(str[12], sizeof(str[0])-1, "applytime%d", num);
			snprintf(str[13], sizeof(str[0])-1, "auditornickname%d", num);
			num++;

			utMsgAdd_s(&psMsgHead, str[0],  file);
			utMsgAdd_s(&psMsgHead, str[1],  path);
			utMsgAdd_s(&psMsgHead, str[2],  md5);
			utMsgAdd_l(&psMsgHead, str[3],  compid);
			utMsgAdd_s(&psMsgHead, str[4],  computername);
			
			if(0 != strlen(mynickname)){
				utMsgAdd_s(&psMsgHead, str[5],  mynickname);
			}else{
				utMsgAdd_s(&psMsgHead, str[5],  nickname);
			}
			utMsgAdd_l(&psMsgHead, str[6],  groupid);
			utMsgAdd_s(&psMsgHead, str[7],  groupname);
			utMsgAdd_s(&psMsgHead, str[8],  auditor);
			utMsgAdd_l(&psMsgHead, str[9],  auditresult);
			utMsgAdd_l(&psMsgHead, str[10],  audittime);
			utMsgAdd_s(&psMsgHead, str[11],  auditopinion);
			utMsgAdd_l(&psMsgHead, str[12],  applytime);
			utMsgAdd_s(&psMsgHead, str[13],  auditornickname);

			//snprintf(mesg, sizeof(mesg)-1, "  num:%d, iReturn:%d,  nameid:%s",  num, iReturn, nameid);
			//recodeLogInfo(" 获取文件审计清单", mesg, sql);
		}
		pasDbCloseCursor(psCur);
		utMsgAdd_l(&psMsgHead, "state",  1);
		utMsgAdd_l(&psMsgHead, "number",  num);
		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}

	return 0;
}

//分析审计列表搜索条件
int  GetAuditionCondition(char *compname,  int applytime,int auditiontime,int auditresult,int auditorsel,char * auditor, char * sql, int sqllen)
{
	if( (NULL == compname)||
	    (NULL == auditor)||
	    (NULL == sql)||
	   (sqllen<= 0)){
		return -1;
	}

	//计算当天零时零分零秒时间值
	struct tm * time_now;
	struct tm   time_tm;
	time_t StartTime;
	time_t now;
	time(&now);

	time_now = localtime(&now);
	time_tm.tm_year = time_now->tm_year;
	time_tm.tm_mon = time_now->tm_mon;
	time_tm.tm_mday = time_now->tm_mday;
	time_tm.tm_hour = 0;
	time_tm.tm_min = 0;
	time_tm.tm_sec = 0;
	
	StartTime = mktime(&time_tm);

	//申请时间
	if((applytime!=-1) && (applytime != 0)){
		if(applytime  == 1){  //今天
	
		}else if(applytime  == 2){//近三天
			StartTime -= 2*24*60*60; 
		}else if(applytime  == 3){//近一周
			StartTime -= 6*24*60*60; 
		}else if(applytime  == 4){//近?辉?
			StartTime -= 29*24*60*60; 
		}else if(applytime  == 5){//近三月
			StartTime -= 3*30*24*60*60; 
		}else if(applytime  == 6){//近半年
			StartTime -= 6*30*24*60*60;
		}

		snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)-1, " and (applytime >= %u and applytime <=%u) ", StartTime, now);
	}

	char	mesg[512];
	memset(mesg, 0, sizeof(mesg));
	_snprintf(mesg, sizeof(mesg)-1,  "audittime:%d, auditresult:%d ",   auditiontime,auditresult );


	//审批时间
	if((-1 != auditiontime) && (0 != auditiontime) &&( (2==auditresult) || (3==auditresult) )){
		if(auditiontime== 1){  //今天
	
		}else if(auditiontime == 2){//近三天
			StartTime -= 2*24*60*60; 
		}else if(auditiontime== 3){//近一周
			StartTime -= 6*24*60*60; 
		}else if(auditiontime== 4){//近一月
			StartTime -= 29*24*60*60; 
		}else if(auditiontime== 5){//近三月
			StartTime -= 3*30*24*60*60; 
		}else if(auditiontime== 6){//近半年
			StartTime -= 6*30*24*60*60;
		}

		snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)-1, " and (audittime >= %u and audittime <=%u) ", StartTime, now);
		recodeLogInfo("审批时间 sql:", sql, "");
	}

	//审批人
	if((-1 != auditorsel) && (0 != auditorsel)){
		char	nickid[128];
		char	mysql[1024];
		memset(nickid, 0, sizeof(nickid));
		memset(mysql, 0, sizeof(mysql));
		snprintf(mysql, sizeof(mysql)-1, "select nameid  from nwAuditFileUser where nickname =\'%s\' and compname = \'%s\' ",  auditor,  compname);
		pasDbOneRecord(mysql,0,UT_TYPE_STRING,sizeof(nickid)-1,  nickid);

		snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)-1, " and auditor=\'%s\' ", nickid);
	}

	//审批结果
	if((-1 != auditresult) && (0 != auditresult)){
		//1 --- 未审批
		//2 --- 同意
		//3 --- 拒绝
		snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)-1, " and auditresult=%d ", auditresult);
	}

	return 1;
}

//递归获取子部门
int GetChildGroup(unsigned int groupid,  char *limit, int len)
{
	if( (NULL == limit )||
	   (len <= 0)){
		return -1;
	}

	int	iSum = 0;
	int	gid = 0;
	char	gname[128];
	char 	caResult[3096];
	char	sql[1024];

	memset(gname, 0, sizeof(gname));
	memset(caResult, 0, sizeof(caResult));
	memset(sql, 0, sizeof(sql));
  
  	snprintf(sql,  sizeof(sql)-1,  "select gid, gname from nwgroup where pid=%u",groupid);
  
  	//查询记录集
 	pasDbRecordSet(sql,1,9999,&iSum,caResult,0);

  	char *p = caResult;
  	while(p && *p) {

    		p = pasDbRecordValue(p,1,2,
        			UT_TYPE_ULONG,sizeof(gid),&gid,
        			UT_TYPE_STRING,sizeof(gname)-1, gname);
  	  	if(p){
     			 snprintf(limit+strlen(limit),  len-strlen(limit),  " or groupid= %u",  gid);
         			 GetChildGroup(gid, limit, len);
    		}
  	}
	return 1;
}

int  Lan_SubmitAuditionResult(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	unsigned int	compid = 0;
	char		file[128];
	char		md5[128];
	char		auditor[128];
	char		auditcontent[128];
	int		audittime =0 ;
	int		auditresult =0;

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(file, 0, sizeof(file));
	memset(md5, 0, sizeof(md5));
	memset(auditor, 0, sizeof(auditor));
	memset(auditcontent, 0, sizeof(auditcontent));

	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,8,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							"compid",  UT_TYPE_ULONG, 4, &compid,
							 "file", UT_TYPE_STRING, sizeof(file)-1, file,
							 "MD5", UT_TYPE_STRING, sizeof(md5)-1, md5,
							 "auditor", UT_TYPE_STRING, sizeof(auditor)-1, auditor,
							 "auditopinion", UT_TYPE_STRING, sizeof(auditcontent)-1, auditcontent,
							"auditresult",  UT_TYPE_LONG, 4, &auditresult,
							"audittime",  UT_TYPE_LONG, 4, &audittime);
	if( (8 != iReturn)||
	    (0==strlen(compname))||
	    (0==compid)||
	    (0==strlen(md5))||
	    (0==strlen(file))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, compid:%u, file:%s, md5:%s, auditresult:%d, auditor:%s",  
			iReturn,  compname, compid, file, md5, auditresult,  auditor);
		recodeLogInfo(" 提交审计结果","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	snprintf(sql, sizeof(sql)-1, "update nwAuditFileAudition  set auditor=\'%s\',  auditcontent=\'%s\',  audittime=%u, auditresult=%d "
		" where compname=\'%s\'  and  compid=%u and file=\'%s\'  and MD5=\'%s\' ",  
		auditor, auditcontent, audittime, auditresult,  compname, compid,  file, md5);

	if(pasDbExecSql(sql, 0)){
		recodeLogInfo(" 提交审计结果", "操作数据库失败！ " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-3);
		return -3;
	}


	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,1);
	return 0;
}

int  Lan_GetAuditionChatList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		myaccountid[128];
	unsigned int	compid = 0;
	int 		groupid=0;
	char		groupname[128];
	char		machine[128];
	char		senderid[128];
	char		sendername[128];
	char		recverid[128];
	char		recvername[128];
	char		content[1024];
	unsigned int	timeval = 0;

	char		nickname[128];
	char		mynickname[128];

	char		limit2[1024];
	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(myaccountid, 0, sizeof(myaccountid));
	memset(groupname, 0, sizeof(groupname));
	memset(senderid, 0, sizeof(senderid));
	memset(sendername, 0, sizeof(sendername));
	memset(recverid, 0, sizeof(recverid));
	memset(recvername, 0, sizeof(recvername));
	memset(content, 0, sizeof(content));
	memset(nickname, 0, sizeof(nickname));
	memset(mynickname, 0, sizeof(mynickname));
	memset(limit2, 0, sizeof(limit2));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,4,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							"myaccountid", UT_TYPE_STRING, sizeof(myaccountid)-1, myaccountid,
							"compid", UT_TYPE_ULONG, 4, &compid,
							"groupid", UT_TYPE_ULONG, 4, &groupid);
	if( (4 != iReturn)||
	    (0==strlen(compname))||
	    (0 == strlen(myaccountid))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, myaccountid:%s, compid:%u, groupid:%d",  iReturn,  compname, myaccountid, compid,  groupid);
		recodeLogInfo(" 获取聊天记录简记信息","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, myaccountid:%s, compid:%u, groupid:%d",  iReturn,  compname, myaccountid, compid,  groupid);
	recodeLogInfo(" 获取聊天记录简记信息","  接收参数!", mesg);

	snprintf(limit2,  sizeof(limit2)-1, " 1=1 ");
	if(compid > 0){
		snprintf(limit2, sizeof(limit2)-1,  " nwcompute.compid=%u",  compid);		
	}else if(groupid > 0){
		snprintf(limit2,  sizeof(limit2)-1, " ( 1=0  ");
		snprintf(limit2+strlen(limit2),  sizeof(limit2)-strlen(limit2)-1,  " or groupid= %u",  groupid);
		GetChildGroup(groupid,  limit2, sizeof(limit2)-strlen(limit2)-1);
		snprintf(limit2+strlen(limit2),  sizeof(limit2)-strlen(limit2)-1, " ) ");
	}

	recodeLogInfo(" 获取聊天记录简记信息","  获取聊天记录简记信息limit2!", limit2);

	snprintf(sql, sizeof(sql)-1, "select  senderid, recverid, nwAuditFileChat.content,  timeval, "
		" nwcompute.compname,  nwcompute.devname,  nwcompute.groupid, nwgroup.gname,  nwAuditFileNickname.nickname "
		" from (nwAuditFileChat inner join  (nwcompute inner join nwgroup on  (nwcompute.groupid = nwgroup.gid  and %s)) "
		" on (nwAuditFileChat.compname = \'%s\' and  (senderid=\'%s\' or  recverid=\'%s\')  and timeval=(select max(timeval)  from nwAuditFileChat  where compname=\'%s\'  and  (senderid=\'%s\' or  recverid=\'%s\') )"
		" and (cast(nwAuditFileChat.senderid  as unsigned) = nwcompute.compid  or  cast(nwAuditFileChat.recverid  as unsigned)= nwcompute.compid) )) "
		" left join  nwAuditFileNickname on (nwcompute.compid = nwAuditFileNickname.compid )"
		" group by senderid , recverid,nwcompute.compname,  nwcompute.devname,  nwcompute.groupid, nwgroup.gname,  nwAuditFileNickname.nickname ",  
		limit2,  compname ,myaccountid,myaccountid,compname ,myaccountid,myaccountid);

	recodeLogInfo(" 获取聊天记录简记信息","  获取聊天记录简记信息limit2!", sql);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //查询数据库失败，记录日志
		recodeLogInfo(" 获取聊天记录简记信息"," 查询聊天表失败!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		int 	iReturn =0;
		int 	num=0;
		char	str[24][48];
		memset(str, 0, sizeof(str));

		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 0);

		while( (iReturn == 0) || (1405 == iReturn)){
			memset(mynickname,  0, sizeof(mynickname));
			iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(senderid)-1, senderid,
							UT_TYPE_STRING, sizeof(recverid)-1, recverid,
							UT_TYPE_STRING, sizeof(content)-1, content,
							UT_TYPE_ULONG, 4, &timeval,
							UT_TYPE_STRING, sizeof(machine)-1, machine,
							UT_TYPE_STRING, sizeof(nickname)-1, nickname,
							UT_TYPE_LONG, 4, &groupid,
							UT_TYPE_STRING, sizeof(groupname)-1, groupname,
							UT_TYPE_STRING, sizeof(mynickname)-1, mynickname);
			if( (0 != iReturn) && (1405 != iReturn) ){
				snprintf(mesg, sizeof(mesg)-1, "iReturn :%d", iReturn);
				recodeLogInfo(" 获取聊天记录简记信息","  退出查询", mesg);
				continue;
			}

			//snprintf(mesg, sizeof(mesg)-1, "iReturn :%d, timeval:%u", iReturn,  timeval);
			//recodeLogInfo(" 获取聊天记录简记信息","  timeval  ", mesg);


			snprintf(str[0], sizeof(str[0])-1, "senderid%d", num);
			snprintf(str[1], sizeof(str[0])-1, "sendername%d", num);
			snprintf(str[2], sizeof(str[0])-1, "recverid%d", num);
			snprintf(str[3], sizeof(str[0])-1, "recvername%d", num);
			snprintf(str[4], sizeof(str[0])-1, "content%d", num);
			snprintf(str[5], sizeof(str[0])-1, "timeval%d", num);
			snprintf(str[6], sizeof(str[0])-1, "machine%d", num);
			snprintf(str[7], sizeof(str[0])-1, "groupid%d", num);
			snprintf(str[8], sizeof(str[0])-1, "groupname%d", num);
			num++;

			utMsgAdd_s(&psMsgHead, str[0],  senderid);
			utMsgAdd_s(&psMsgHead, str[2],  recverid);
			if(0 != strlen(mynickname)){
				utMsgAdd_s(&psMsgHead, str[1],  mynickname);
				utMsgAdd_s(&psMsgHead, str[3],  mynickname);
			}else{
				utMsgAdd_s(&psMsgHead, str[1],  nickname);
				utMsgAdd_s(&psMsgHead, str[3],  nickname);
			}

			//snprintf(mesg, sizeof(mesg)-1, "iReturn :%d, timeval:%u,  str[5]:%s", iReturn,  timeval, str[5]);
			//recodeLogInfo(" 获取聊天记录简记信息","  timeval  ", mesg);


			utMsgAdd_s(&psMsgHead, str[4],  content);
			utMsgAdd_l(&psMsgHead, str[5],  timeval);
			utMsgAdd_l(&psMsgHead, str[7],  groupid);
			utMsgAdd_s(&psMsgHead, str[8],  groupname);
		}
		pasDbCloseCursor(psCur);
		utMsgAdd_l(&psMsgHead, "state",  1);
		utMsgAdd_l(&psMsgHead, "number",  num);
		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}

	return 0;
}

int  Lan_DropClientNickname(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	unsigned int	compid = 0;
	char		clientid[128];
	char		clientname[128];

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(clientid, 0, sizeof(clientid));
	memset(clientname, 0, sizeof(clientname));

	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,3,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							 "clientid", UT_TYPE_STRING, sizeof(clientid)-1, clientid,
							 "clientname", UT_TYPE_STRING, sizeof(clientname)-1, clientname);
	if( (3 != iReturn)||
	    (0==strlen(compname))||
	    (0==strlen(clientid))||
	    (0==strlen(clientname))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, clientid:%s, clientname:%s",  iReturn,  compname, clientid, clientname);
		recodeLogInfo(" 修改客户端昵称","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	//snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, clientid:%s, clientname:%s",  iReturn,  compname, clientid, clientname);
	//recodeLogInfo(" 修改客户端昵称","  接收参数!", mesg);

	int  count = 0;
	snprintf(sql, sizeof(sql)-1, "select count(*) from nwAuditFileNickname where compname=\'%s\'  and  compid=%u ", compname, strtoul(clientid, NULL, 10));
	pasDbOneRecord(sql,0,UT_TYPE_LONG,4,&count);
	if(count >0){
		snprintf(sql, sizeof(sql)-1, "update nwAuditFileNickname  set nickname=\'%s\' where compname=\'%s\'  and  compid=%u ",  clientname, compname, strtoul(clientid, NULL, 10));
		if(pasDbExecSql(sql, 0)){
			recodeLogInfo(" 修改客户端昵称", "操作数据库失败！ " , sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-3);
			return -3;
		}
	}else{
		snprintf(sql, sizeof(sql)-1, "insert into  nwAuditFileNickname(compid, nickname,  compname) values(%u,  \'%s\', \'%s\') ", strtoul(clientid, NULL, 10), clientname, compname);
		if(pasDbExecSql(sql, 0)){
			recodeLogInfo(" 插入客户端昵称", "操作数据库失败！ " , sql);
			pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-3);
			return -3;
		}
	}

	//recodeLogInfo(" 修改客户端昵称", "操作数据库！ " , sql);

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,1);
	return 0;
}


int  Lan_SendMessage(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		senderid[128];
	char		recverid[128];
	char		content[1024];
	unsigned int  	timeval = 0;

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(senderid, 0, sizeof(senderid));
	memset(recverid, 0, sizeof(recverid));
	memset(content, 0, sizeof(content));

	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,5,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							 "senderid", UT_TYPE_STRING, sizeof(senderid)-1, senderid,
							 "recverid", UT_TYPE_STRING, sizeof(recverid)-1, recverid,
							 "content", UT_TYPE_STRING, sizeof(content)-1, content,
							 "timeval", UT_TYPE_LONG, 4, &timeval	);
	if( (5 != iReturn)||
	    (0==strlen(compname))||
	    (0==strlen(senderid))||
	    (0==strlen(recverid))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, senderid:%s, recverid:%s",  iReturn,  compname, senderid, recverid);
		recodeLogInfo(" 发送信息","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	//snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, clientid:%s, clientname:%s",  iReturn,  compname, clientid, clientname);
	//recodeLogInfo(" 发送信息","  接收参数!", mesg);

	snprintf(sql, sizeof(sql)-1, "insert into  nwAuditFileChat(senderid, recverid, content, timeval, compname)  values(\'%s\',\'%s\', \'%s\',%u,\'%s\') ",  
		senderid,  recverid,  content,  timeval,  compname);
	if(pasDbExecSql(sql, 0)){
		recodeLogInfo(" 发送信息", "操作数据库失败！ " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-3);
		return -3;
	}
	
	//recodeLogInfo(" 发送信息", "操作数据库！ " , sql);

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,1);
	return 0;
}

int  Lan_TestNewMessage(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		myaccountid[128];
	char		youraccountid[128];
	char		content[1024];
	unsigned int  	timeval = 0;

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(myaccountid, 0, sizeof(myaccountid));
	memset(youraccountid, 0, sizeof(youraccountid));
	memset(content, 0, sizeof(content));

	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,3,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							 "myaccountid", UT_TYPE_STRING, sizeof(myaccountid)-1, myaccountid,
							 "youraccountid", UT_TYPE_STRING, sizeof(youraccountid)-1, youraccountid);
	if( (3 != iReturn)||
	    (0==strlen(compname))||
	    (0==strlen(myaccountid))||
	    (0==strlen(youraccountid))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, myaccountid:%s, youraccountid:%s",  iReturn,  compname, myaccountid, youraccountid);
		recodeLogInfo(" 检测有么有未读信息","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	//snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, clientid:%s, clientname:%s",  iReturn,  compname, clientid, clientname);
	//recodeLogInfo(" 检测有么有未读信息","  接收参数!", mesg);

	snprintf(sql, sizeof(sql)-1, "select min(timeval) timeval   from   nwAuditFileChat  where  compname=\'%s\' and  senderid=\'%s\'  and  recverid = \'%s\' and  state=0 ", compname,  youraccountid,  myaccountid);
	pasDbOneRecord(sql,0,UT_TYPE_LONG,4,&timeval);
	if(!timeval){
		recodeLogInfo(" 检测有么有未读信息", "没有未读信息！ " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,2);
		return 0;
	}
		
	//recodeLogInfo("检测有么有未读信息", "操作数据库！ " , sql);

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,1);
	return 0;
}

int  Lan_GetNewMessage(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		myaccountid[128];
	char		youraccountid[128];

	char		senderid[128];
	char		recverid[128];
	char		content[1024];
	unsigned int	timeval = 0;

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(myaccountid, 0, sizeof(myaccountid));
	memset(youraccountid, 0, sizeof(youraccountid));
	memset(senderid, 0, sizeof(senderid));
	memset(recverid, 0, sizeof(recverid));
	memset(content, 0, sizeof(content));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

		int iReturn = utMsgGetSomeNVar(psMsgHead,3,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							 "myaccountid", UT_TYPE_STRING, sizeof(myaccountid)-1, myaccountid,
							 "youraccountid", UT_TYPE_STRING, sizeof(youraccountid)-1, youraccountid);
	if( (3 != iReturn)||
	    (0==strlen(compname))||
	    (0==strlen(myaccountid))||
	    (0==strlen(youraccountid))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, myaccountid:%s, youraccountid:%s",  iReturn,  compname, myaccountid, youraccountid);
		recodeLogInfo(" 获取未读信息","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	//snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, clientid:%s, clientname:%s",  iReturn,  compname, clientid, clientname);
	//recodeLogInfo(" 获取未读信息","  接收参数!", mesg);

	snprintf(sql, sizeof(sql)-1, "select min(timeval) timeval   from   nwAuditFileChat  where  compname=\'%s\' and  senderid=\'%s\'  and  recverid = \'%s\' and  state=0 ", compname,  youraccountid,  myaccountid);
	pasDbOneRecord(sql,0,UT_TYPE_LONG,4,&timeval);
	if(!timeval){
		recodeLogInfo(" 检测有么有未读信息", "没有未读信息！ " , sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,2);
		return 0;
	}
		
	//recodeLogInfo("获取未读信息", "操作数据库！ " , sql);

	snprintf(sql, sizeof(sql)-1, "select senderid, recverid, content, timeval  from  nwAuditFileChat  "
		" where compname=\'%s\' and  ((senderid=\'%s\'  and  recverid = \'%s\')  or (senderid=\'%s\'  and  recverid = \'%s\') ) "
		" and  state=0 and  timeval >= %u  order by  timeval", compname,  youraccountid,  myaccountid,  myaccountid, youraccountid,   timeval);


	recodeLogInfo("获取未读信息", "操作数据库！ " , sql);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //查询数据库失败，记录日志
		recodeLogInfo("  获取未读信息"," 查询聊天表失败!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		int 	iReturn =0;
		int 	num=0;
		char	str[24][48];
		memset(str, 0, sizeof(str));

		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 0);

		while( (iReturn == 0) || (1405 == iReturn)){
			memset(senderid,  0, sizeof(senderid));
			iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(senderid)-1, senderid,
							UT_TYPE_STRING, sizeof(recverid)-1, recverid,
							UT_TYPE_STRING, sizeof(content)-1, content,
							UT_TYPE_ULONG, 4, &timeval);

			if( (0 != iReturn) && (1405 != iReturn) ){
				snprintf(mesg, sizeof(mesg)-1, "iReturn :%d", iReturn);
				recodeLogInfo(" 获取未读信息","  退出查询", mesg);
				continue;
			}

			//snprintf(mesg, sizeof(mesg)-1, "iReturn :%d, timeval:%u", iReturn,  timeval);
			//recodeLogInfo("获取未读信息","  timeval  ", mesg);


			snprintf(str[0], sizeof(str[0])-1, "senderid%d", num);
			snprintf(str[1], sizeof(str[0])-1, "recverid%d", num);
			snprintf(str[2], sizeof(str[0])-1, "content%d", num);
			snprintf(str[3], sizeof(str[0])-1, "timeval%d", num);
			num++;

			utMsgAdd_s(&psMsgHead, str[0],  senderid);
			utMsgAdd_s(&psMsgHead, str[1],  recverid);
			utMsgAdd_s(&psMsgHead, str[2],  content);
			utMsgAdd_l(&psMsgHead, str[3],  timeval);

		}
		pasDbCloseCursor(psCur);
		utMsgAdd_l(&psMsgHead, "state",  1);
		utMsgAdd_l(&psMsgHead, "number",  num);
		MypasMsgResponse(iFd,psMsgHead,NULL);

		snprintf(sql , sizeof(sql)-1, "update nwAuditFileChat  set state=1   where  senderid=\'%s\'  and recverid=\'%s\' ",   youraccountid,  myaccountid);
		if(pasDbExecSql(sql, 0)){
			recodeLogInfo(" 获取未读信息", " 更新数据库“未读”为“已读”失败！" , sql);
		}
		
		return 0;
	}

	return 0;
}

int  Lan_GetAllMessage(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		myaccountid[128];
	char		youraccountid[128];

	char		senderid[128];
	char		recverid[128];
	char		content[1024];
	unsigned int	timeval = 0;

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(myaccountid, 0, sizeof(myaccountid));
	memset(youraccountid, 0, sizeof(youraccountid));
	memset(senderid, 0, sizeof(senderid));
	memset(recverid, 0, sizeof(recverid));
	memset(content, 0, sizeof(content));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

		int iReturn = utMsgGetSomeNVar(psMsgHead,3,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							 "myaccountid", UT_TYPE_STRING, sizeof(myaccountid)-1, myaccountid,
							 "youraccountid", UT_TYPE_STRING, sizeof(youraccountid)-1, youraccountid);
	if( (3 != iReturn)||
	    (0==strlen(compname))||
	    (0==strlen(myaccountid))||
	    (0==strlen(youraccountid))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, myaccountid:%s, youraccountid:%s",  iReturn,  compname, myaccountid, youraccountid);
		recodeLogInfo(" 获取历史信息","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	snprintf(sql, sizeof(sql)-1, "select senderid, recverid, content, timeval  from  nwAuditFileChat  "
		" where compname=\'%s\' and  ((senderid=\'%s\'  and  recverid = \'%s\')  or (senderid=\'%s\'  and  recverid = \'%s\') ) "
		" order by  timeval", compname,  youraccountid,  myaccountid,  myaccountid, youraccountid,   timeval);


	//recodeLogInfo("获取历史信息", "操作数据库！ " , sql);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //查询数据库失败，记录日志
		recodeLogInfo(" 获取历史信息"," 查询聊天表失败!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		int 	iReturn =0;
		int 	num=0;
		char	str[24][48];
		memset(str, 0, sizeof(str));

		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 0);

		while( (iReturn == 0) || (1405 == iReturn)){
			memset(senderid,  0, sizeof(senderid));
			iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(senderid)-1, senderid,
							UT_TYPE_STRING, sizeof(recverid)-1, recverid,
							UT_TYPE_STRING, sizeof(content)-1, content,
							UT_TYPE_ULONG, 4, &timeval);

			if( (0 != iReturn) && (1405 != iReturn) ){
				snprintf(mesg, sizeof(mesg)-1, "iReturn :%d", iReturn);
				recodeLogInfo(" 获取历史信息","  退出查询", mesg);
				continue;
			}

			//snprintf(mesg, sizeof(mesg)-1, "iReturn :%d, timeval:%u", iReturn,  timeval);
			//recodeLogInfo("获取历史信息","  timeval  ", mesg);

			snprintf(str[0], sizeof(str[0])-1, "senderid%d", num);
			snprintf(str[1], sizeof(str[0])-1, "recverid%d", num);
			snprintf(str[2], sizeof(str[0])-1, "content%d", num);
			snprintf(str[3], sizeof(str[0])-1, "timeval%d", num);
			num++;

			utMsgAdd_s(&psMsgHead, str[0],  senderid);
			utMsgAdd_s(&psMsgHead, str[1],  recverid);
			utMsgAdd_s(&psMsgHead, str[2],  content);
			utMsgAdd_l(&psMsgHead, str[3],  timeval);

		}
		pasDbCloseCursor(psCur);
		utMsgAdd_l(&psMsgHead, "state",  1);
		utMsgAdd_l(&psMsgHead, "number",  num);
		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}

	return 0;
}


int  Lan_GetPersonWhoSendNewMessageToMe(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	char		myaccountid[128];
	char		youraccountid[128];

	char		senderid[128];
	char		sendername[128];
	char		recverid[128];
	char		content[1024];
	unsigned int	timeval = 0;

	char		nickname[128];
	char		nickname2[128];

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(myaccountid, 0, sizeof(myaccountid));
	memset(youraccountid, 0, sizeof(youraccountid));
	memset(senderid, 0, sizeof(senderid));
	memset(sendername, 0, sizeof(sendername));
	memset(recverid, 0, sizeof(recverid));
	memset(content, 0, sizeof(content));
	memset(nickname, 0, sizeof(nickname));
	memset(nickname2, 0, sizeof(nickname2));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

		int iReturn = utMsgGetSomeNVar(psMsgHead,2,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							 "myaccountid", UT_TYPE_STRING, sizeof(myaccountid)-1, myaccountid);
	if( (2 != iReturn)||
	    (0==strlen(compname))||
	    (0==strlen(myaccountid))){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, myaccountid:%s",  iReturn,  compname, myaccountid);
		recodeLogInfo(" 获取发送新信息给我的联系人","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

/*	snprintf(sql, sizeof(sql)-1, "select senderid, devname,  nickname from  "
		" (nwAuditFileChat inner join nwcompute  on(nwAuditFileChat.compname=\'%s\' and   recverid = \'%s\' and state = 0  and  nwAuditFileChat.senderid = nwcompute.compid)) "
		" left join  nwAuditFileNickname on( cast(nwAuditFileChat.senderid as  unsigned ) = nwAuditFileNickname.compid )  "
		" group by senderid,  devname, nickname",
		 compname,  myaccountid);

*/
	snprintf(sql, sizeof(sql)-1, " select senderid, devname, nwAuditFileNickname.nickname usernickname, nwAuditFileUser.nickname adminnickname "
		" from (select senderid from nwAuditFileChat where compname=\'%s\' and   recverid = \'%s\' and state=0) as a "
		" left join nwcompute on( cast(a.senderid as unsigned) =nwcompute.compid) "
		" left join nwAuditFileNickname on( cast(a.senderid as unsigned )= nwAuditFileNickname.compid   and  nwAuditFileNickname.compname=\'%s\') "
		" left join nwAuditFileUser on( a.senderid = nameid   and  nwAuditFileUser.compname=\'%s\') "
		" group by senderid,  devname, usernickname, adminnickname ",
		 compname,  myaccountid, compname, compname);


	//recodeLogInfo("获取发送新信息给我的联系人", "操作数据库！ " , sql);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //查询数据库失败，记录日志
		recodeLogInfo(" 获取发送新信息给我的联系人"," 查询聊天表失败!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		int 	iReturn =0;
		int 	num=0;
		char	str[24][48];
		memset(str, 0, sizeof(str));

		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 0);

		while( (iReturn == 0) || (1405 == iReturn)){
			memset(sendername,  0, sizeof(sendername));
			memset(nickname,  0, sizeof(nickname));
			memset(nickname2,  0, sizeof(nickname2));
			iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(senderid)-1, senderid,
							UT_TYPE_STRING, sizeof(sendername)-1, sendername,
							UT_TYPE_STRING, sizeof(nickname)-1, nickname,
							UT_TYPE_STRING, sizeof(nickname2)-1, nickname2);

			if( (0 != iReturn) && (1405 != iReturn) ){
				snprintf(mesg, sizeof(mesg)-1, "iReturn :%d", iReturn);
				recodeLogInfo(" 获取发送新信息给我的联系人","  退出查询", mesg);
				continue;
			}

			//snprintf(mesg, sizeof(mesg)-1, "iReturn :%d, timeval:%u", iReturn,  timeval);
			//recodeLogInfo("获取发送新信息给我的联系人","  timeval  ", mesg);

			snprintf(str[0], sizeof(str[0])-1, "senderid%d", num);
			snprintf(str[1], sizeof(str[0])-1, "sendername%d", num);
			num++;

			utMsgAdd_s(&psMsgHead, str[0],  senderid);
			if(0 != strlen(nickname2)){
				utMsgAdd_s(&psMsgHead, str[1],  nickname2);
			}else{
				if(0 != strlen(nickname)){
					utMsgAdd_s(&psMsgHead, str[1],  nickname);
				}else if(0 != strlen(sendername)){
					utMsgAdd_s(&psMsgHead, str[1],  sendername);
				}else{
					utMsgAdd_s(&psMsgHead, str[1],  senderid);
				}
			}
		}
		pasDbCloseCursor(psCur);
		utMsgAdd_l(&psMsgHead, "state",  1);
		utMsgAdd_l(&psMsgHead, "number",  num);
		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}

	return 0;
}


int  Lan_GetAuditState(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	unsigned int	compid = 0;
	char		file[128];
	char		md5[128];

	char		sql[1024];
	char		mesg[512];

	memset(compname, 0, sizeof(compname));
	memset(file, 0, sizeof(file));
	memset(md5, 0, sizeof(md5));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,4,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							 "compid", UT_TYPE_ULONG, 4, &compid,
							 "file", UT_TYPE_STRING, sizeof(file)-1, file,
							 "MD5", UT_TYPE_STRING, sizeof(md5)-1, md5);

	
	if( (iReturn != 4)||
	    (0 == strlen(compname))||
	    (0 == compid)||
	   (0 == strlen(md5))){
		snprintf(mesg, sizeof(mesg)-1,  "compname:%s, compid:%u,  file:%s,  MD5:%s",  compname, compid, file,  md5);
		recodeLogInfo(" 获取文件审计状态"," 参数有误", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG, -1);
		return -1;
	}

	snprintf(sql,  sizeof(sql)-1,  "select  count(*),  auditresult  from nwAuditFileAudition  where  compname=\'%s\'  and  compid=%u  and  MD5=\'%s\'  group by auditresult ",  compname,  compid,  md5);
	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //查询数据库失败，记录日志
		recodeLogInfo("获取文件审计状态"," 查询失败!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		int	count = 0;
		int   	filestate = 0;
		iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &count,
						UT_TYPE_LONG, 4, &filestate);
		pasDbCloseCursor(psCur);

							
		if(count <= 0){
			snprintf(mesg, sizeof(mesg)-1,  "compname:%s, compid:%u,  file:%s,  MD5:%s",  compname, compid, file,  md5);
			recodeLogInfo("获取文件审计状态"," 该文件不在审计列表中!", mesg);
			pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2,"state",   UT_TYPE_LONG,    1,
									 	 "filestate",   UT_TYPE_LONG,    -1);
			return 0;
		}
		
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2,"state",   UT_TYPE_LONG,    1,
									  "filestate",   UT_TYPE_LONG,    filestate);
		return 0;		
	
	}
	return 0;
}

int  Lan_CommitAudit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	unsigned int	compid = 0;
	char		file[128];
	char		path[1024];
	char		md5[128];
	unsigned int	applytime=0;
	int		count = 0;

	char		sql[1024];
	char		mesg[512];

	memset(compname, 0, sizeof(compname));
	memset(file, 0, sizeof(file));
	memset(path, 0, sizeof(path));
	memset(md5, 0, sizeof(md5));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,6,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							 "compid", UT_TYPE_ULONG, 4, &compid,
							 "file", UT_TYPE_STRING, sizeof(file)-1, file,
							 "path", UT_TYPE_STRING, sizeof(path)-1, path,
							 "MD5", UT_TYPE_STRING, sizeof(md5)-1, md5,
							 "applytime", UT_TYPE_ULONG, 4, &applytime);

	
	if( (iReturn != 6)||
	    (0 == strlen(compname))||
	    (0 == compid)||
	    (0 == strlen(md5))||
	    (0 == applytime)){
		snprintf(mesg, sizeof(mesg)-1,  "compname:%s, compid:%u,  file:%s,  MD5:%s, applytime:%u",  compname, compid, file,  md5,applytime);
		recodeLogInfo(" 提交文件审计"," 参数有误", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG, -1);
		return -1;
	}


	snprintf(sql,  sizeof(sql)-1,  "select  count(*) from nwAuditFileAudition  where  compname=\'%s\'  and  compid=%u  and  MD5=\'%s\' ",  compname,  compid,  md5);
	pasDbOneRecord(sql,0,UT_TYPE_LONG,4,&count);							
	if(count > 0){
		snprintf(mesg, sizeof(mesg)-1,  "compname:%s, compid:%u,  file:%s,  MD5:%s",  compname, compid, file,  md5);
		recodeLogInfo("提交文件审计"," 该文件已在审计列表中!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -4);
		return -4;
	}
		
	snprintf(sql,  sizeof(sql)-1,  "insert into nwAuditFileAudition(file,  path,  MD5, applytime,  compid,  compname)  values(\'%s\',  \'%s\', \'%s\', %u,  %u, \'%s\') ",   
		file,  path,  md5, applytime,compid,  compname);
	if(pasDbExecSql(sql, 0)){
		recodeLogInfo(" 提交文件审计", "操作数据库失败！ " , sql);

		snprintf(mesg, sizeof(mesg)-1,  "error:%d",  errno);
		recodeLogInfo(" 提交文件审计", "操作数据库失败！ " , mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-5);
		return -5;
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    1);
	return 0;
}

int  Lan_CancelAudit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	unsigned int	compid = 0;
	int		count = 0;
	int		sucessnum=0;
	char		file[128];
	char		md5[128];

	char		sql[1024];
	char		mesg[512];

	memset(compname, 0, sizeof(compname));
	memset(file, 0, sizeof(file));
	memset(md5, 0, sizeof(md5));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,3,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							 "compid", UT_TYPE_ULONG, 4, &compid,
							 "count", UT_TYPE_ULONG, 4, &count);

	
	if( (iReturn != 3)||
	    (0 == strlen(compname))||
	    (0 == compid)){
		snprintf(mesg, sizeof(mesg)-1,  "compname:%s, compid:%u, count:%u",  compname, compid, count);
		recodeLogInfo(" 撤销文件审计"," 参数有误", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG, -1);
		return -1;
	}

	int number = 0;
	char	str[24][48];
	memset(str, 0, sizeof(str));

	for(number = 0;  number< count ; number++){
		snprintf(str[0],  sizeof(str[0])-1, "file%d",  number);
		snprintf(str[1],  sizeof(str[0])-1, "MD5%d",  number);

		int iReturn = utMsgGetSomeNVar(psMsgHead,2,  str[0] , UT_TYPE_STRING, sizeof(file)-1, file,
							         str[1],  UT_TYPE_STRING, sizeof(md5)-1, md5);

		if( (2 != iReturn) ||
		    (0 == strlen(md5))){
			continue;
		}

		snprintf(sql,  sizeof(sql)-1,  " delete from  nwAuditFileAudition   where  compname=\'%s\'   and compid=%u  and MD5=\'%s\'  ",  compname,  compid,  md5);
		if(pasDbExecSql(sql, 0)){
			recodeLogInfo(" 撤销文件审计", "操作数据库失败！ " , sql);
			continue;
		}

		sucessnum++;
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2,"state",   UT_TYPE_LONG,    1,
								  "successnum",   UT_TYPE_LONG,    sucessnum);
	
	return 0;
}

int  Lan_GetAuditFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	unsigned int	compid = 0;
	int		number = 0;

	char		file[128];
	char		path[512];
	char		MD5[128];
	char		auditor[1024];
	char		nickname[128];

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(file, 0, sizeof(file));
	memset(path, 0, sizeof(path));
	memset(MD5, 0, sizeof(MD5));
	memset(auditor, 0, sizeof(auditor));
	memset(nickname, 0, sizeof(nickname));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

		int iReturn = utMsgGetSomeNVar(psMsgHead,2,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
								"compid", UT_TYPE_LONG, 4, &compid);

	if( (2 != iReturn)||
	    (0==strlen(compname))||
	    (0==compid)){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, compid:%u",  iReturn,  compname, compid);
		recodeLogInfo(" 获取新的通过审计的文件","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	snprintf(sql, sizeof(sql)-1, "select file, path,  MD5,auditor, nickname  from  "
		" (select  file, path, MD5, auditor  from  nwAuditFileAudition  where nwAuditFileAudition.compname=\'%s\'  and  nwAuditFileAudition.compid=%u  and  nwAuditFileAudition.auditresult= 2  and  nwAuditFileAudition.state=0   )  a  "
		" left join  nwAuditFileUser "	
		" on(nwAuditFileUser.compname = \'%s\'    and  a.auditor=nwAuditFileUser.nameid)",
		 compname,  compid, compname);

	pasDbCursor *psCur = pasDbOpenSql(sql,0);
	if(psCur == NULL) { //查询数据库失败，记录日志
		recodeLogInfo(" 获取新的通过审计的文件"," 查询失败!", sql);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,    -3);
		return -3;
        }else{
		int 	iReturn =0;
		int 	num=0;
		char	str[24][48];
		memset(str, 0, sizeof(str));

		MypasMsgHeadInit(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 0);

		while( (iReturn == 0) || (1405 == iReturn)){
			memset(nickname,  0, sizeof(nickname));
			iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(file)-1, file,
							UT_TYPE_STRING, sizeof(path)-1, path,
							UT_TYPE_STRING, sizeof(MD5)-1, MD5,
							UT_TYPE_STRING, sizeof(auditor)-1, auditor,
							UT_TYPE_STRING, sizeof(nickname)-1, nickname);

			if( (0 != iReturn) && (1405 != iReturn) ){
				snprintf(mesg, sizeof(mesg)-1, "iReturn :%d", iReturn);
				recodeLogInfo(" 获取新的通过审计的文件","  退出查询", mesg);
				continue;
			}

			//snprintf(mesg, sizeof(mesg)-1, "iReturn :%d, timeval:%u", iReturn,  timeval);
			//recodeLogInfo("获取新的通过审计的文件","  timeval  ", mesg);

			snprintf(str[0], sizeof(str[0])-1, "file%d", num);
			snprintf(str[1], sizeof(str[0])-1, "path%d", num);
			snprintf(str[2], sizeof(str[0])-1, "MD5%d", num);
			snprintf(str[3], sizeof(str[0])-1, "auditor%d", num);
			snprintf(str[4], sizeof(str[0])-1, "auditornickname%d", num);
			num++;

			utMsgAdd_s(&psMsgHead, str[0],  file);
			utMsgAdd_s(&psMsgHead, str[1],  path);
			utMsgAdd_s(&psMsgHead, str[2],  MD5);
			utMsgAdd_s(&psMsgHead, str[3],  auditor);
			utMsgAdd_s(&psMsgHead, str[4],  nickname);	
		}
		pasDbCloseCursor(psCur);
		utMsgAdd_l(&psMsgHead, "state",  1);
		utMsgAdd_l(&psMsgHead, "number",  num);
		MypasMsgResponse(iFd,psMsgHead,NULL);
		return 0;
	}

	return 0;
}

int  Lan_ReturnAuditFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	unsigned int	compid = 0;
	int		count=0;
	int		successnum=0;
	char		file[128];
	char		path[512];
	char		MD5[128];

	char		str[24][48];

	char		sql[1024];
	char		mesg[1024];

	memset(compname, 0, sizeof(compname));
	memset(file, 0, sizeof(file));
	memset(path, 0, sizeof(path));
	memset(MD5, 0, sizeof(MD5));

	memset(str, 0, sizeof(str));
	memset(sql, 0, sizeof(sql));
	memset(mesg, 0, sizeof(mesg));

	int iReturn = utMsgGetSomeNVar(psMsgHead,3,  "compname", UT_TYPE_STRING, sizeof(compname)-1, compname,
							"compid",  UT_TYPE_ULONG, 4, &compid,
							"count",  UT_TYPE_LONG, 4, &count);
	if( (3 != iReturn)||
	    (0==strlen(compname))||
	    (0==compid)){
		snprintf(mesg, sizeof(mesg)-1, "iReturn:%d,  compname:%s, compid:%u, count:%d",  iReturn,  compname, compid, count);
		recodeLogInfo(" 已通过审计的文件已经通知客户端","  接收参数有误!", mesg);
		pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 1,"state",   UT_TYPE_LONG,-2);
		return -2;
	}

	int num = 0;
	for(num=0;    num < count;  num++){

		snprintf(str[0],  sizeof(str[0])-1,    "file%d",  num);
		snprintf(str[1],  sizeof(str[0])-1,    "path%d",  num);
		snprintf(str[2],  sizeof(str[0])-1,    "MD5%d",  num);

		memset(file, 0, sizeof(file));
		memset(path, 0, sizeof(path));
		memset(MD5, 0, sizeof(MD5));

		int iReturn = utMsgGetSomeNVar(psMsgHead,3,  str[0], UT_TYPE_STRING, sizeof(file)-1, file,
							str[1], UT_TYPE_STRING, sizeof(path)-1, path,
							str[2], UT_TYPE_STRING, sizeof(MD5)-1, MD5);

		if( (3 != iReturn) ||
		 (0 == strlen(MD5))){
			continue;
		}
		
		snprintf(sql, sizeof(sql)-1, "update nwAuditFileAudition  set state=1   where compname=\'%s\'  and  compid=%u and  MD5=\'%s\' ",  compname, compid,  MD5);
		
		if(pasDbExecSql(sql, 0)){
			recodeLogInfo(" 已通过审计的文件已经通知客户端", "更新数据库失败！ " , sql);
		}else{
			successnum++;
		}
	}

	pasTcpResponse(iFd,psMsgHead,NULL, /* 密钥，暂时不用  */ 2,"state",   UT_TYPE_LONG,1,
								"successnum",   UT_TYPE_LONG,successnum);

	return 0;
}
/*
int  Lan_MyFunc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	char		compname[128];
	unsigned int	compid = 0;
	memset(compname, 0, sizeof(compname));
	int iReturn = utMsgGetSomeNVar(psMsgHead,2,  "data1", UT_TYPE_STRING, sizeof(compname)-1, compname,
							"data2",  UT_TYPE_ULONG, 4, &compid);
	if( (2!= iReturn)||
	    (0==strlen(compname))||
	    (0==compid)){
		return 0;
	}
	pasTcpResponse(iFd,psMsgHead,NULL, 2,"state", UT_TYPE_STRING, compname,
						"nickname",   UT_TYPE_LONG,compid);
	return 0;
}
*/
int ncInitNwWebFun_AuditFile(utShmHead *psShmHead)
{
	pasSetTcpFunName("Lan_GetAuditionUser",     Lan_GetAuditionUser,    NULL,0,0);
	pasSetTcpFunName("Lan_AddAuditionUser",     Lan_AddAuditionUser,    NULL,0,0);
	pasSetTcpFunName("Lan_DeleteAuditionUser",     Lan_DeleteAuditionUser,    NULL,0,0);
	pasSetTcpFunName("Lan_DropAuditionUser",     Lan_DropAuditionUser,    NULL,0,0);
	pasSetTcpFunName("Lan_AuditionUserExist",     Lan_AuditionUserExist,    NULL,0,0);
	pasSetTcpFunName("Lan_GetAuditionList",     Lan_GetAuditionList,    NULL,0,0);
	pasSetTcpFunName("Lan_SubmitAuditionResult",     Lan_SubmitAuditionResult,    NULL,0,0);
	pasSetTcpFunName("Lan_GetAuditionChatList",     Lan_GetAuditionChatList,    NULL,0,0);
	pasSetTcpFunName("Lan_DropClientNickname",     Lan_DropClientNickname,    NULL,0,0);


	pasSetTcpFunName("Lan_SendMessage",    Lan_SendMessage,    NULL,0,0);
	pasSetTcpFunName("Lan_TestNewMessage",    Lan_TestNewMessage,    NULL,0,0);
	pasSetTcpFunName("Lan_GetNewMessage",    Lan_GetNewMessage,    NULL,0,0);
	pasSetTcpFunName("Lan_GetAllMessage",    Lan_GetAllMessage,    NULL,0,0);
	pasSetTcpFunName("Lan_GetPersonWhoSendNewMessageToMe",    Lan_GetPersonWhoSendNewMessageToMe,    NULL,0,0);

	pasSetTcpFunName("Lan_GetAuditState",    Lan_GetAuditState,    NULL,0,0);
	pasSetTcpFunName("Lan_CommitAudit",    Lan_CommitAudit,    NULL,0,0);
	pasSetTcpFunName("Lan_CancelAudit",    Lan_CancelAudit,    NULL,0,0);

	pasSetTcpFunName("Lan_GetAuditFile",    Lan_GetAuditFile,    NULL,0,0);
	pasSetTcpFunName("Lan_ReturnAuditFile",    Lan_ReturnAuditFile,    NULL,0,0);

	//pasSetTcpFunName("Lan_MyFunc",    Lan_MyFunc,    NULL,0,0);

	return 0;
}