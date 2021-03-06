#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoplt01.h"


#define SYSTEM_LOG 1

char* gbkToStd(utShmHead *psShmHead, unsigned char *pStr)
{
    if(strcmp(utComGetVar_sd(psShmHead, "nclang", "\0"), "big5") == 0)
    {
        return pasGbkTobig5(pStr);
    }
    else
    {
        return pStr;
    }
}



int lan_systemLog_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
#if SYSTEM_LOG
    utMsgPrintMsg(psMsgHead);
#endif
    char ipaddr_in[16] = "";
    char action_in[8] = "";
    char result_in[8] = "";
    char keyword_in[128] = "";
    char sdate_in[16] = "";
    char edate_in[16] = "";
    char opname_in[32] = "";
    char descr_in[256] = "";
    char start_in[16] = "";
    char limit_in[16] = "";
    char sort_in[16] = "";
    char dir_in[16] = "";
	char caShortname[256] = "";
	strcpy(caShortname, getLoginShortName());

    utMsgGetSomeNVar(psMsgHead, 12,
                     "ipaddr",       UT_TYPE_STRING,  sizeof(ipaddr_in) - 1,       ipaddr_in,
                     "action",       UT_TYPE_STRING,  sizeof(action_in) - 1,       action_in,
                     "result",       UT_TYPE_STRING,  sizeof(result_in) - 1,       result_in,
                     "keyword",  UT_TYPE_STRING,  sizeof(keyword_in) - 1,      keyword_in,
                     "sdate",        UT_TYPE_STRING,  sizeof(sdate_in) - 1,            sdate_in,
                     "edate",        UT_TYPE_STRING,  sizeof(edate_in) - 1,            edate_in,
                     "opname",       UT_TYPE_STRING,  sizeof(opname_in) - 1,       opname_in,
                     "descr",        UT_TYPE_STRING,  sizeof(descr_in) - 1,            descr_in,
                     "start",        UT_TYPE_STRING,  sizeof(start_in) - 1,            start_in,
                     "limit",        UT_TYPE_STRING,  sizeof(limit_in) - 1,            limit_in,
                     "sort",         UT_TYPE_STRING,  sizeof(sort_in) - 1,             sort_in,
                     "dir",          UT_TYPE_STRING,  sizeof(dir_in) - 1,              dir_in);


    char sql[1024] = "";
    char sql_tmp[1024] = "";
    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();

    sdate_in[10] = 0;
    edate_in[10] = 0;

    char username[32] = "";
    dsCltGetMyInfo(1, "USERNAME", username);

    snprintf(sql_tmp, sizeof(sql_tmp), " and sdate >= '%s 00:00:00' and sdate <= '%s 23:59:59' ", sdate_in, edate_in);

    if(!utStrIsSpaces(ipaddr_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and ipaddr = '%s'", ipaddr_in);
    }
    if(!utStrIsSpaces(action_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and action = '%02d'", atoi(action_in));
    }
    if(!utStrIsSpaces(result_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and result = '%s'", result_in);
    }
    if(!utStrIsSpaces(opname_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and name = '%s'", opname_in);
    }
    if(!utStrIsSpaces(descr_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and descr like '%%%s%%'", descr_in);
    }

    if(!utStrIsSpaces(keyword_in))
    {
        char *keyword_gbk = utf8convert(psShmHead, keyword_in);
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and (ipaddr like '%%%s%%' or name like '%%%s%%' or descr like '%%%s%%')", keyword_gbk, keyword_gbk, keyword_gbk);
    }

    if(!utStrIsSpaces(sort_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " order by %s %s", sort_in, dir_in);
    }

    snprintf(sql, sizeof(sql), "select count(*) from %s where name in(select dispname from dsuser where groupid = (select groupid from dsuser where name='%s')) %s", getNewTable(caShortname,  "ncadminlog"),username, sql_tmp);

#if SYSTEM_LOG
    printf("sql_count:%s\n", sql);
#endif

    psCur = pasDbOpenSql(sql, 0);
    unsigned int db_count = 0;
    pasDbFetchInto(psCur, UT_TYPE_ULONG,  4, &db_count);
    pasDbCloseCursor(psCur);

    utPltPutVarF(psDbHead, "TotRec", "%u", db_count);

    snprintf(sql, sizeof(sql), "select sdate,name,ipaddr,descr,action,result from %s where name in(select name from dsuser where groupid = (select groupid from dsuser where name='%s')) %s limit %s,%s",  getNewTable(caShortname,  "ncadminlog"), username, sql_tmp, start_in, limit_in);

#if SYSTEM_LOG
    printf("sql:%s\n", sql);
#endif
    psCur = pasDbOpenSql(sql, 0);

    if(NULL != psCur)
    {
        char sdate[20] = "";
        char name[16] = "";
        char ipaddr[16] = "";
        char descr[256] = "";
        char action[4] = "";
        char result[10] = "";

        int iret = -1;
        int iNum = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_STRING, sizeof(sdate), sdate,
                                          UT_TYPE_STRING, sizeof(name), name,
                                          UT_TYPE_STRING, sizeof(ipaddr), ipaddr,
                                          UT_TYPE_STRING, sizeof(descr), descr,
                                          UT_TYPE_STRING, sizeof(action), action,
                                          UT_TYPE_STRING, sizeof(result), result)) || 1405 == iret)
        {


            if(!strcmp(result, "Success"))
            {
                strcpy(result, ncLang("0131���"));
            }
            else if(!strcmp(result, "Fail"))
            {
                strcpy(result, ncLang("0130ʧ��"));
            }
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            utPltPutLoopVar(psDbHead, "sdate", iNum, sdate);
            utPltSetCvtHtml(1);
            utPltPutLoopVar(psDbHead, "name", iNum, name);
            utPltPutLoopVar(psDbHead, "ipaddr", iNum, ipaddr);

            utPltPutLoopVar(psDbHead, "descr", iNum, gbkToStd(psShmHead, descr));
            utPltPutLoopVarF(psDbHead, "action", iNum, "%u", atoi(action));
            utPltPutLoopVar(psDbHead, "result", iNum, result);
            utPltSetCvtHtml(0);
        }
        pasDbCloseCursor(psCur);

#if SYSTEM_LOG
        utPltShowDb(psDbHead);
#endif

        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_system_log.htm");
    }

    return 0;
}


int writeSysLog(utMsgHead *psMsgHead, char *action, char *result, char *msg)
{
    char caIp[20];
    char username[32];
    char caMsg[1256];
    char caMsg_t[1256];
    char caDate[20];
    int iReturn;
    unsigned long lUserid;
    unsigned long lSid;

    utShmHead *psShmHead = utShmCurHead();
    if(strcasecmp(utComGetVar_sd(psShmHead, "WriteSyslog", "YES"), "No") == 0)
    {
        return 0;
    }

    iReturn = utMsgGetSomeNVar(psMsgHead, 1, "clientip",  UT_TYPE_STRING, sizeof(caIp) - 1, caIp);
    memset(caMsg, 0, sizeof(caMsg));
	

    if(strlen(msg) != 0)
    {
        //strcpy(caMsg, convert("GBK", "BIG5", msg));
        strcpy(caMsg, msg);
        strcpy(caDate, utTimFormat("%Y-%m-%d %H:%M:%S", time(0)));
        strcpy(caMsg_t, (char *)pasDbReplaceQuote1(caMsg));
        lSid = pasGetSid("ncadminlog", "sid");
        dsCltGetMyInfo(1, "USERNAME", username);		
        username[31] = '\0';

        iReturn = pasDbExecSqlF("insert into %s(sid,sdate,name,ipaddr,descr,action,result) values (%lu,'%s','%s','%s','%s','%s','%s')", getNewTable(getLoginShortName(),"ncadminlog"),lSid, caDate, username, caIp, caMsg_t, action, result);
        if(iReturn == 0)
        {
            pasDbCommit(NULL);
        }
    }
    return 0;

}

int checkrolename(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);

    char rolename_in[32] = "";

    utMsgGetSomeNVar(psMsgHead, 1, "rolename", UT_TYPE_STRING, sizeof(rolename_in) - 1, rolename_in);

    char caMyname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", caMyname);

    char sql[512] = "";
    int count = 0;

    sprintf(sql, "select count(*) from dsrole where name='%s' and addby='%s'", utf8convert(psShmHead, rolename_in), caMyname);

    printf("check sql:%s\n", sql);

    pasDbOneRecord(sql, 0, UT_TYPE_LONG, 4, &count);

    utPltDbHead *psDbHead = utPltInitDb();

    if(count == 0)
    {
        utPltPutVar(psDbHead, "buff", "{value:'success'}");
    }
    else
    {
        utPltPutVar(psDbHead, "buff", "{value:'failure'}");
    }

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_tmpPlate.htm");

    return 0;
}
