#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "nwdef.h"
#include "ncdef.h"


#define TIME_GROUP_FLAG 1
#define TIME_FLAG 1
#define WORK_TIME_FLAG 1
#define SOFT_FLAG 1
#define USB_GROUP_FLAG 1
#define USB_FLAG 1
#define GROUP_TREE_FLAG 1
#define CLIENT_FLAG 1



void dateNumtoStr(int fromDate, char *toDate)
{
    if(fromDate > 10000)
    {
        snprintf(toDate, 11, "%4u/%02u/%02u", fromDate / 10000, fromDate % 10000 / 100, fromDate % 100);
    }
    else if(fromDate > 100)
    {
        snprintf(toDate, 6, "%02u/%02u", fromDate / 100, fromDate % 100);
    }
    else
    {
        snprintf(toDate, 3, "%u", fromDate);
    }
}

unsigned long dateStrToNum(char *fromDate)
{
    unsigned int toDate = 0;
    char *tmp = NULL;

    if(strlen(fromDate) <= 2)
    {
        toDate = atoi(fromDate);
    }
    else if(strlen(fromDate) <= 5)
    {
        tmp = strtok(fromDate, "/");
        toDate = atoi(tmp) * 100;
        tmp = strtok(NULL, "/");
        toDate = toDate + atoi(tmp);
    }
    else if(strlen(fromDate) <= 10)
    {
        tmp = strtok(fromDate, "/");
        toDate = atoi(tmp) * 10000;
        tmp = strtok(NULL, "/");
        toDate = toDate + atoi(tmp) * 100;
        tmp = strtok(NULL, "/");
        toDate = toDate + atoi(tmp);
    }

    return toDate;
}

void timeNumtoStr(int fromTime, char *toTime)
{
    snprintf(toTime, 9, "%02u:%02u:%02u", fromTime / 10000, fromTime % 10000 / 100, fromTime % 100);
}

unsigned long timeStrToNum(char *fromTime)
{
    unsigned int toTime = 0;
    char *tmp = NULL;

    tmp = strtok(fromTime, ":");
    toTime = atoi(tmp) * 10000;
    tmp = strtok(NULL, ":");
    toTime = toTime + atoi(tmp) * 100;
    tmp = strtok(NULL, ":");
    toTime = toTime + atoi(tmp);

    return toTime;
}



// 时间树
int lan_time_group_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
#if TIME_GROUP_FLAG
    utMsgPrintMsg(psMsgHead);
#endif

    char deal_in[8] = "";
    char text_in[64] = "";
    char id_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 3,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "text", UT_TYPE_STRING,  sizeof(text_in) - 1,     text_in,
                     "treeId",       UT_TYPE_STRING,      sizeof(id_in) - 1,   id_in);


    char auth[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);

    char sql[1024] = "";
    char msg[2048] = "";
    if(!strcmp(deal_in, "insert"))
    {
        int iReturn;

        char *text_gbk = utf8convert(psShmHead, text_in);
        iReturn = pasDbExecSqlF("insert into nwdatedes (name,modtime,createuser) values ('%s',%lu,'%s')", text_gbk, time(0), auth);

        snprintf(msg, sizeof(msg), "增加了[%s]时间类别", text_gbk);
        free(text_gbk);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "update"))
    {
        int iReturn;
        char *text_gbk = utf8convert(psShmHead, text_in);
        iReturn = pasDbExecSqlF("update nwdatedes set name = '%s' where id = %s", text_gbk, id_in);

        snprintf(msg, sizeof(msg), "修改了时间类别为[%s]", text_gbk);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "delete"))
    {
        int iReturn;

        iReturn = pasDbExecSqlF("delete from nwdatedes where id = %s", id_in);
        snprintf(msg, sizeof(msg), "删除了时间类别id为[%s]", id_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else
    {
        snprintf(msg, sizeof(msg), "查询了时间类别");
        writeSysLog(psMsgHead, "06", "Success", msg);
    }

    snprintf(sql, sizeof(sql), "select id,name from nwdatedes where createuser = '%s' or createuser='0'  order by modtime", auth);

#if TIME_GROUP_FLAG
    printf("%s\n", sql);
#endif

    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();

    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned long id = 0;
        char name[64] = "";
        int iret = -1;
        int iNum = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  4,                     &id,
                                          UT_TYPE_STRING, sizeof(name) - 1,            name)) || 1405 == iret)
        {
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }

            utPltPutLoopVarF(psDbHead, "id", iNum, "%u", id);
            utPltPutLoopVar(psDbHead, "text", iNum, name);
        }
        pasDbCloseCursor(psCur);

    }

#if TIME_GROUP_FLAG
    utPltShowDb(psDbHead);
#endif

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_time_group.htm");

    return 0;
}
//时间内容
int lan_time_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
#if TIME_FLAG
    utMsgPrintMsg(psMsgHead);
#endif

    char deal_in[8] = "";
    char sid_in[128] = "";
    char text_in[64] = "";
    char ltype_in[8] = "";
    char llower_in[24] = "";
    char lupper_in[24] = "";
    char id_in[8] = "";
    char start_time_in[9] = "";
    char stop_time_in[9] = "";
    char start_in[8] = "";
    char limit_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 10,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "sid",  UT_TYPE_STRING,  sizeof(sid_in) - 1,      sid_in,
                     "id",   UT_TYPE_STRING,  sizeof(id_in) - 1,               id_in,
                     "ltype",        UT_TYPE_STRING,      sizeof(ltype_in) - 1,    ltype_in,
                     "llower",   UT_TYPE_STRING,  sizeof(llower_in) - 1,   llower_in,
                     "lupper",       UT_TYPE_STRING,      sizeof(lupper_in) - 1,   lupper_in,
                     "startTime",    UT_TYPE_STRING,  sizeof(start_time_in) - 1,   start_time_in,
                     "stopTime",     UT_TYPE_STRING,      sizeof(stop_time_in) - 1,    stop_time_in,
                     "start",    UT_TYPE_STRING,  sizeof(start_in) - 1,    start_in,
                     "limit",        UT_TYPE_STRING,      sizeof(limit_in) - 1,    limit_in);


    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);

    findShortName(auth, shortname);

    int start = atoi(start_in);
    int limit = atoi(limit_in);

    char sql[1024] = "";
    char sql_tmp[1024] = "";

    unsigned long llower_tmp = 0;
    unsigned long lupper_tmp = 0;
    unsigned long start_time_tmp = 0;
    unsigned long stop_time_tmp = 0;

    printf("starttime:%s\n", start_time_in);
    if(!utStrIsSpaces(llower_in))
    {
        llower_tmp = dateStrToNum(llower_in);
        lupper_tmp = dateStrToNum(lupper_in);
    }
    if(!utStrIsSpaces(start_time_in))
    {
        start_time_tmp = timeStrToNum(start_time_in);
        stop_time_tmp = timeStrToNum(stop_time_in);
    }

    time_t time_tmp;

    time(&time_tmp);
    char msg[204] = "";

    if(!strcmp(deal_in, "insert"))
    {
        int iReturn;
        snprintf(sql, sizeof(sql), "insert into nwlimdate (id,ltype,llower,lupper,lltime,lutime,modtime,createuser) values (%s,%s,%u,%u,%u,%u,%u,'%s')", id_in, ltype_in, llower_tmp, lupper_tmp, start_time_tmp, stop_time_tmp, time_tmp, shortname);
        iReturn = pasDbExecSqlF(sql);
        printf("sql:insert:%s\n", sql);

        snprintf(msg, sizeof(msg), "增加了时间对象到类别id[%s]", id_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "update"))
    {
        snprintf(sql, sizeof(sql), "update nwlimdate set id = %s,ltype = %s,llower = %u,lupper = %u,lltime = %u,lutime = %u,modtime = %u where sid = %s", id_in, ltype_in, llower_tmp, lupper_tmp, start_time_tmp, stop_time_tmp, time_tmp, sid_in);
        pasDbExecSqlF(sql);

        snprintf(msg, sizeof(msg), "修改了时间对象id为[%s]", sid_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "delete"))
    {
        pasDbExecSqlF("update nwlimdate set flag=1,modtime=%lu where sid in(%s)", time(0), sid_in);

        snprintf(msg, sizeof(msg), "删除了时间对象id为[%s]", sid_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "select"))
    {

        snprintf(msg, sizeof(msg), "查询时间对象", sid_in);
        writeSysLog(psMsgHead, "06", "Success", msg);
        if(!utStrIsSpaces(id_in))
        {
            snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and nwlimdate.id = %s", id_in);
        }

        if(!utStrIsSpaces(ltype_in))
        {
            snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and ltype = %s", ltype_in);
        }
    }


    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();
    int db_count = 0;

    snprintf(sql, sizeof(sql), "select count(*) from nwdatedes,nwlimdate where nwusblist.flag=0 and nwlimdate.flag=0 and nwdatedes.id=nwlimdate.id and nwlimdate.createuser='%s' %s order by nwlimdate.modtime", shortname, sql_tmp);

    psCur = pasDbOpenSql(sql, 0);
    pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &db_count);
    pasDbCloseCursor(psCur);
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);

    snprintf(sql, sizeof(sql), "select sid,name,ltype,llower,lupper,lltime,lutime from nwdatedes,nwlimdate where nwdatedes.id=nwlimdate.id and nwlimdate.createuser='%s' %s order by nwlimdate.modtime limit %u,%u", shortname, sql_tmp, start, limit);

#if TIME_FLAG
    printf("%s\n", sql);
#endif

    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned long sid = 0;
        char time_group[32] = "";
        int type = 0;
        int lower = 0;
        int upper = 0;
        int start_time = 0;
        int stop_time = 0;

        int iret = -1;
        int iNum = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  4,                 &sid,
                                          UT_TYPE_STRING, sizeof(time_group), time_group,
                                          UT_TYPE_LONG,   4,                 &type,
                                          UT_TYPE_LONG,  4,                      &lower,
                                          UT_TYPE_LONG,  4,                      &upper,
                                          UT_TYPE_LONG,  4,                      &start_time,
                                          UT_TYPE_LONG,  4,                      &stop_time)) || 1405 == iret)
        {
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            char lower_tmp2[24] = "";
            char upper_tmp2[24] = "";
            char start_time_tmp2[24] = "";
            char stop_time_tmp2[24] = "";

            dateNumtoStr(lower, lower_tmp2);
            dateNumtoStr(upper, upper_tmp2);
            timeNumtoStr(start_time, start_time_tmp2);
            timeNumtoStr(stop_time, stop_time_tmp2);

            utPltPutLoopVarF(psDbHead, "sid", iNum, "%u", sid);
            utPltPutLoopVarF(psDbHead, "timeGroup", iNum, time_group);
            utPltPutLoopVarF(psDbHead, "type", iNum, "%u", type);
            utPltPutLoopVar(psDbHead, "lower", iNum, lower_tmp2);
            utPltPutLoopVar(psDbHead, "upper", iNum, upper_tmp2);
            utPltPutLoopVar(psDbHead, "startTime", iNum, start_time_tmp2);
            utPltPutLoopVar(psDbHead, "stopTime", iNum, stop_time_tmp2);
        }
        pasDbCloseCursor(psCur);

    }

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_time.htm");

    return 0;
}

//软件树
int lan_soft_class_search_base(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
#if TIME_GROUP_FLAG
    utMsgPrintMsg(psMsgHead);
#endif

    char deal_in[8] = "";
    char text_in[64] = "";
    char id_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 3,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "text", UT_TYPE_STRING,  sizeof(text_in) - 1,     text_in,
                     "treeId",       UT_TYPE_STRING,      sizeof(id_in) - 1,   id_in);


    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);

    findShortName(auth, shortname);

    char sql[1024] = "";
    char msg[2048] = "";
    if(!strcmp(deal_in, "insert"))
    {
        time_t time_tmp;
        time(&time_tmp);
        int iReturn;

        char *text_gbk = utf8convert(psShmHead, text_in);

        iReturn = pasDbExecSqlF("insert into nwsoftclass (name,modtime,createuser) values ('%s','%u','%s')", text_gbk, time_tmp, shortname);


        snprintf(msg, sizeof(msg), "增加软件类别[%s]", text_gbk);
        writeSysLog(psMsgHead, "03", "Success", msg);
        free(text_gbk);
    }
    else if(!strcmp(deal_in, "update"))
    {
        int iReturn;
        char *text_gbk = utf8convert(psShmHead, text_in);
        iReturn = pasDbExecSqlF("update nwsoftclass set name = '%s' where id = %s", text_gbk, id_in);

        snprintf(msg, sizeof(msg), "修改了软件类别为[%s]", text_gbk);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "delete"))
    {
        int iReturn;

        iReturn = pasDbExecSqlF("delete from nwsoftclass where id = %s and createuser<>'0'", id_in);

        snprintf(msg, sizeof(msg), "删除了软件类别id为[%s]", id_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else
    {
        snprintf(msg, sizeof(msg), "查询了软件类别");
        writeSysLog(psMsgHead, "06", "Success", msg);
    }

    snprintf(sql, sizeof(sql), "select id,name from nwsoftclass where (createuser='%s' or createuser='0') order by modtime", shortname);

#if TIME_GROUP_FLAG
    printf("%s\n", sql);
#endif

    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();

    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned long id = 0;
        char name[64] = "";
        int iret = -1;
        int iNum = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  4,                     &id,
                                          UT_TYPE_STRING, sizeof(name) - 1,            name)) || 1405 == iret)
        {
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }

            utPltPutLoopVarF(psDbHead, "id", iNum, "%u", id);
            utPltPutLoopVar(psDbHead, "text", iNum, name);
        }
        pasDbCloseCursor(psCur);

    }
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_time_group.htm");

    return 0;
}
//软件内容
int lan_soft_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
#if SOFT_FLAG
    utMsgPrintMsg(psMsgHead);
#endif

    char deal_in[8] = "";
    char sid_in[128] = "";
    char soft_name_in[64] = "";
    char proc_name_in[64] = "";
    char keywords_in[64] = "";
    char id_in[8] = "";
    char start_in[8] = "";
    char limit_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 8,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "sid",  UT_TYPE_STRING,  sizeof(sid_in) - 1,      sid_in,
                     "id",   UT_TYPE_STRING,  sizeof(id_in) - 1,               id_in,
                     "softName",     UT_TYPE_STRING,      sizeof(soft_name_in) - 1,    soft_name_in,
                     "procName",     UT_TYPE_STRING,      sizeof(proc_name_in) - 1,    proc_name_in,
                     "keywords",     UT_TYPE_STRING,      sizeof(keywords_in) - 1,     keywords_in,
                     "start",    UT_TYPE_STRING,  sizeof(start_in) - 1,    start_in,
                     "limit",        UT_TYPE_STRING,      sizeof(limit_in) - 1,    limit_in);


    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);

    findShortName(auth, shortname);

    char sql[1024] = "";
    char sql_tmp[1024] = "";
    int start = atoi(start_in);
    int limit = atoi(limit_in);

    time_t time_tmp;
    time(&time_tmp);
    char msg[2048] = "";
    if(!strcmp(deal_in, "insert"))
    {
        utf8convert(psShmHead, soft_name_in);
        pasDbExecSqlF("insert into nwsoftlist (id,name,pname,modtime,createuser) values ('%s','%s','%s','%u','%s')", id_in, utf8convert(psShmHead, soft_name_in), proc_name_in, time_tmp, shortname);

        snprintf(msg, sizeof(msg), "增加了软件对象[%s]到类别id[%s]", utf8convert(psShmHead, soft_name_in), id_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "update"))
    {
        pasDbExecSqlF("update nwsoftlist set id = '%s',name = '%s',pname = '%s',modtime = '%u' where sid = '%s'", id_in, utf8convert(psShmHead, soft_name_in), proc_name_in, time_tmp, sid_in);
        snprintf(msg, sizeof(msg), "修改了软件id[%s]的软件名为[%s]", sid_in, utf8convert(psShmHead, soft_name_in));
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "delete"))
    {
        pasDbExecSqlF("update nwsoftlist set flag=1,modtime=%lu where sid in(%s) and createuser <> '0'", time(0), sid_in);
        snprintf(msg, sizeof(msg), "删除了软件id为[%s]", sid_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "select"))
    {

        snprintf(msg, sizeof(msg), "查询了软件");
        writeSysLog(psMsgHead, "06", "Success", msg);
        if(!utStrIsSpaces(id_in))
        {
            snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and nwsoftlist.id = %s", id_in);
        }

        if(!utStrIsSpaces(keywords_in))
        {
            snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and nwsoftlist.name like '%%%s%%'", utf8convert(psShmHead, keywords_in));
        }
    }


    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();
    int db_count = 0;

    snprintf(sql, sizeof(sql), "select count(*) from nwsoftlist,nwsoftclass where nwsoftlist.flag=0 and nwsoftclass.id=nwsoftlist.id and (nwsoftlist.createuser='%s' or nwsoftlist.createuser='0') %s order by nwsoftlist.modtime", shortname, sql_tmp);
    printf("count:%s\n", sql);
    psCur = pasDbOpenSql(sql, 0);
    pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &db_count);
    pasDbCloseCursor(psCur);
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);

    snprintf(sql, sizeof(sql), "select sid,nwsoftclass.name,nwsoftlist.name,pname,nwsoftlist.createuser from nwsoftclass,nwsoftlist where nwsoftlist.flag=0 and nwsoftlist.id=nwsoftclass.id and (nwsoftlist.createuser='%s'or nwsoftlist.createuser='0') %s order by nwsoftlist.modtime limit %u,%u", shortname, sql_tmp, start, limit);

#if SOFT_FLAG
    printf("%s\n", sql);
#endif

    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned long sid = 0;
        char soft_class[64] = "";
        char soft_name[64] = "";
        char proc_name[64] = "";
        char createuser[32] = "";

        int iret = -1;
        int iNum = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  4,                 &sid,
                                          UT_TYPE_STRING, sizeof(soft_class), soft_class,
                                          UT_TYPE_STRING, sizeof(soft_name), soft_name,
                                          UT_TYPE_STRING, sizeof(proc_name), proc_name,
                                          UT_TYPE_STRING, sizeof(createuser), createuser)) || 1405 == iret)
        {
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            utPltPutLoopVarF(psDbHead, "sid", iNum, "%u", sid);
            utPltPutLoopVar(psDbHead, "softClass", iNum, soft_class);
            utPltPutLoopVar(psDbHead, "softName", iNum, soft_name);
            utPltPutLoopVar(psDbHead, "procName", iNum, proc_name);
            utPltPutLoopVar(psDbHead, "createuser", iNum, createuser);
        }
        pasDbCloseCursor(psCur);
    }

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_soft.htm");

    return 0;
}

//USB树
int lan_USB_class_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
#if USB_GROUP_FLAG
    utMsgPrintMsg(psMsgHead);
#endif

    char deal_in[8] = "";
    char text_in[64] = "";
    char id_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 3,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "text", UT_TYPE_STRING,  sizeof(text_in) - 1,     text_in,
                     "treeId",       UT_TYPE_STRING,      sizeof(id_in) - 1,   id_in);


    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);

    findShortName(auth, shortname);

    char sql[1024] = "";
    char msg[2048] = "";
    if(!strcmp(deal_in, "insert"))
    {
        time_t time_tmp;
        time(&time_tmp);
        int iReturn;

        char *text_gbk = utf8convert(psShmHead, text_in);
        sprintf(sql, "insert into nwusbclass (name,modtime,createuser) values ('%s','%u','%s')", text_gbk, time_tmp, shortname);
        printf("USB类型：%s\n", sql);
        iReturn = pasDbExecSqlF(sql);

        snprintf(msg, sizeof(msg), "增加了USB类别[%s]", text_gbk);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "update"))
    {
        char *text_gbk = utf8convert(psShmHead, text_in);
        pasDbExecSqlF("update nwusbclass set name = '%s' where id = %s", text_gbk, id_in);

        snprintf(msg, sizeof(msg), "修改了id为[%s]USB类别名[%s]", id_in, text_gbk);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "delete"))
    {
        pasDbExecSqlF("delete from nwusbclass where id = %s", id_in);

        snprintf(msg, sizeof(msg), "删除了id为[%s]USB类别", id_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else
    {
        snprintf(msg, sizeof(msg), "查询USB类别");
        writeSysLog(psMsgHead, "06", "Success", msg);
    }

    snprintf(sql, sizeof(sql), "select id,name from nwusbclass where createuser='%s' or createuser='' order by modtime", shortname);

    writeLog("base.txt", "usb", sql);

#if USB_GROUP_FLAG
    printf("%s\n", sql);
#endif

    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();

    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned long id = 0;
        char name[64] = "";
        int iret = -1;
        int iNum = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  4,                     &id,
                                          UT_TYPE_STRING, sizeof(name) - 1,            name)) || 1405 == iret)
        {
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }

            utPltPutLoopVarF(psDbHead, "id", iNum, "%u", id);
            utPltPutLoopVar(psDbHead, "text", iNum, name);
        }
        pasDbCloseCursor(psCur);

    }

#if USB_GROUP_FLAG
    utPltShowDb(psDbHead);
#endif

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_time_group.htm");

    return 0;
}
//USB内容
int lan_USB_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
#if USB_FLAG
    utMsgPrintMsg(psMsgHead);
#endif

    char deal_in[8] = "";
    char sid_in[128] = "";
    char usb_name_in[24] = "";
    char prod_name_in[24] = "";
    char id_in[16] = "";
    char old_id_in[128] = "";
    char vol_name_in[24] = "";
    char hard_id_in[24] = "";
    char bind_user_in[24] = "";
    char create_user_in[24] = "";
    char sort_in[16] = "";
    char dir_in[8] = "";

    char start_in[8] = "";
    char limit_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 13,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "sid",  UT_TYPE_STRING,  sizeof(sid_in) - 1,      sid_in,
                     "id",   UT_TYPE_STRING,  sizeof(id_in) - 1,               id_in,
                     "oldId", UT_TYPE_STRING,  sizeof(old_id_in) - 1,      old_id_in,
                     "USBName",      UT_TYPE_STRING,      sizeof(usb_name_in) - 1,     usb_name_in,
                     "prodName",     UT_TYPE_STRING,      sizeof(prod_name_in) - 1,    prod_name_in,
                     "volName",      UT_TYPE_STRING,      sizeof(vol_name_in) - 1,     vol_name_in,
                     "hardId",       UT_TYPE_STRING,      sizeof(hard_id_in) - 1,  hard_id_in,
                     "bindUser",     UT_TYPE_STRING,      sizeof(bind_user_in) - 1,    bind_user_in,
                     "sort", UT_TYPE_STRING,  sizeof(sort_in) - 1,     sort_in,
                     "dir",      UT_TYPE_STRING,      sizeof(dir_in) - 1,  dir_in,
                     "start",    UT_TYPE_STRING,  sizeof(start_in) - 1,    start_in,
                     "limit",        UT_TYPE_STRING,      sizeof(limit_in) - 1,    limit_in);


    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);

    findShortName(auth, shortname);

    char sql[1024] = "";
    char sql_tmp[1024] = "";
    int start = atoi(start_in);
    int limit = atoi(limit_in);

    time_t time_tmp;
    time(&time_tmp);

    char *usb_name_gbk = utf8convert(psShmHead, usb_name_in);
    char *prod_name_gbk = utf8convert(psShmHead, prod_name_in);
    char *hard_id_gbk = utf8convert(psShmHead, hard_id_in);
    char *vol_name_gbk  = utf8convert(psShmHead, vol_name_in);
    char *bind_user_gbk  = utf8convert(psShmHead, bind_user_in);

    char msg[2048] = "";

    if(!strcmp(deal_in, "insert"))
    {
        pasDbExecSqlF("insert into nwusblist (id,name,produname,volname,hardid,binduser,modtime,createuser) values ('%s','%s','%s','%s','%s','%s','%u','%s')", id_in, usb_name_gbk, prod_name_gbk, vol_name_gbk, hard_id_gbk, bind_user_gbk, time_tmp, shortname);

        snprintf(msg, sizeof(msg), "增加了USB对象[%s]", usb_name_gbk);
        writeSysLog(psMsgHead, "03", "Success", msg);

    }
    else if(!strcmp(deal_in, "update"))
    {
        if(!utStrIsSpaces(old_id_in))
        {
            printf("%s\n", old_id_in);
            pasDbExecSqlF("update nwusblist set id = '%s',modtime = '%u' where sid in (%s)", id_in, time_tmp, old_id_in);

            snprintf(msg, sizeof(msg), "移动id[%s]USB对象到类别id[%s]", old_id_in, id_in);
            writeSysLog(psMsgHead, "03", "Success", msg);
        }
        else
        {
            pasDbExecSqlF("update nwusblist set id = '%s',name = '%s',produname = '%s',volname = '%s',hardid = '%s',binduser = '%s',modtime = '%u' where sid = '%s'", id_in, usb_name_gbk, prod_name_gbk, vol_name_gbk, hard_id_gbk, bind_user_gbk, time_tmp, sid_in);

            snprintf(msg, sizeof(msg), "修改USB对象[%s]", sid_in);
            writeSysLog(psMsgHead, "03", "Success", msg);
        }
    }
    else if(!strcmp(deal_in, "delete"))
    {
        pasDbExecSqlF("update nwusblist set flag=1,modtime=%lu where sid in (%s)", time(0), sid_in);
        snprintf(msg, sizeof(msg), "删除USB对象[%s]", sid_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "select"))
    {
        snprintf(msg, sizeof(msg), "查询USB对象");
        writeSysLog(psMsgHead, "06", "Success", msg);
        if(!utStrIsSpaces(id_in))
        {
            snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and nwusblist.id = %s", id_in);
        }
    }

    free(usb_name_gbk);
    free(prod_name_gbk);
    free(vol_name_gbk);
    free(bind_user_gbk);
    free(hard_id_gbk);

    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();
    int db_count = 0;

    snprintf(sql, sizeof(sql), "select count(*) from nwusblist,nwusbclass where nwusblist.flag=0 and nwusbclass.id=nwusblist.id and (nwusblist.createuser='%s' or nwusblist.createuser='') %s order by nwusblist.modtime", shortname, sql_tmp);
#if USB_FLAG
    printf("sql_count:%s\n", sql);
#endif
    psCur = pasDbOpenSql(sql, 0);
    pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &db_count);
    pasDbCloseCursor(psCur);
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);

    if(utStrIsSpaces(sort_in))
    {
        strcpy(sort_in, "stime");
        strcpy(dir_in, "DESC");
    }

    snprintf(sql, sizeof(sql), "select sid,nwusbclass.name,nwusblist.name,nwusblist.stime,produname,volname,hardid,binduser,nwusblist.createuser from nwusbclass,nwusblist where nwusblist.flag=0 and nwusblist.id=nwusbclass.id and (nwusblist.createuser='%s' or nwusblist.createuser='') %s order by %s %s limit %u,%u", shortname, sql_tmp, sort_in, dir_in, start, limit);

    writeLog("base.txt", "usb", sql);

    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned long sid = 0;
        char usb_class[64] = "";
        unsigned int stime = 0;
        char usb_name[24] = "";
        char prod_name[24] = "";
        char vol_name[24] = "";
        char hard_id[128] = "";
        char bind_user[24] = "";
        char create_user[24] = "";
        int iret = -1;
        int iNum = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  4,                  &sid,
                                          UT_TYPE_STRING, sizeof(usb_class), usb_class,
                                          UT_TYPE_STRING, sizeof(usb_name), usb_name,
                                          UT_TYPE_ULONG,  4,                  &stime,
                                          UT_TYPE_STRING, sizeof(prod_name), prod_name,
                                          UT_TYPE_STRING, sizeof(vol_name), vol_name,
                                          UT_TYPE_STRING, sizeof(hard_id), hard_id,
                                          UT_TYPE_STRING, sizeof(bind_user), bind_user,
                                          UT_TYPE_STRING, sizeof(create_user), create_user)) || 1405 == iret)
        {
            char *text_gbk = NULL;
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            utPltPutLoopVarF(psDbHead, "sid", iNum, "%u", sid);
            utPltPutLoopVar(psDbHead, "usbClass", iNum, usb_class);
            utPltPutLoopVar(psDbHead, "usbName", iNum, usb_name);
            utPltPutLoopVar(psDbHead, "stime", iNum, utTimFormat("%m/%d %H:%M:%S", stime));
            utPltPutLoopVar(psDbHead, "prodName", iNum, prod_name);
            utPltPutLoopVar(psDbHead, "volName", iNum, vol_name);
            utPltPutLoopVar(psDbHead, "hardId", iNum, hard_id);
            utPltPutLoopVar(psDbHead, "bindUser", iNum, bind_user);
            utPltPutLoopVar(psDbHead, "createUser", iNum, create_user);
        }
        pasDbCloseCursor(psCur);
    }

#if USB_FLAG
    utPltShowDb(psDbHead);
#endif

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_usb.htm");

    return 0;
}

//工作时间
int lan_work_time_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
#if WORK_TIME_FLAG
    utMsgPrintMsg(psMsgHead);
#endif

    char deal_in[8] = "";
    char sid_in[128] = "";
    char text_in[64] = "";
    char ltype_in[8] = "";
    char llower_in[24] = "";
    char lupper_in[24] = "";
    char start_time_in[24] = "";
    char stop_time_in[24] = "";
    char time_flag_in[2] = "";
    char id_in[8] = "";
    char start_in[8] = "";
    char limit_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 11,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "sid",  UT_TYPE_STRING,  sizeof(sid_in) - 1,      sid_in,
                     "id",   UT_TYPE_STRING,  sizeof(id_in) - 1,               id_in,
                     "ltype",        UT_TYPE_STRING,      sizeof(ltype_in) - 1,    ltype_in,
                     "llower",   UT_TYPE_STRING,  sizeof(llower_in) - 1,   llower_in,
                     "lupper",       UT_TYPE_STRING,      sizeof(lupper_in) - 1,   lupper_in,
                     "startTime",    UT_TYPE_STRING,  sizeof(start_time_in) - 1,   start_time_in,
                     "stopTime",     UT_TYPE_STRING,      sizeof(stop_time_in) - 1,    stop_time_in,
                     "timeFlag",     UT_TYPE_STRING,      sizeof(time_flag_in) - 1,    time_flag_in,
                     "start",    UT_TYPE_STRING,  sizeof(start_in) - 1,    start_in,
                     "limit",        UT_TYPE_STRING,      sizeof(limit_in) - 1,    limit_in);


    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);

    findShortName(auth, shortname);

    char sql[1024] = "";
    char sql_tmp[1024] = "";
    int start = atoi(start_in);
    int limit = atoi(limit_in);

    unsigned long llower_tmp;
    unsigned long lupper_tmp;
    unsigned long start_time_tmp;
    unsigned long stop_time_tmp;

    if(!utStrIsSpaces(llower_in))
    {
        llower_tmp = dateStrToNum(llower_in);
        lupper_tmp = dateStrToNum(lupper_in);
    }
    if(!utStrIsSpaces(start_time_in))
    {
        start_time_tmp = timeStrToNum(start_time_in);
        stop_time_tmp = timeStrToNum(stop_time_in);
    }

    char msg[2048] = "";
    if(!strcmp(deal_in, "insert"))
    {
        pasDbExecSqlF("insert into ncworktime (timetype,llower,lupper,starttime,stoptime,flags,createuser) values ('%s',%u,%u,%u,%u,'%s','%s')", ltype_in, llower_tmp, lupper_tmp, start_time_tmp, stop_time_tmp, time_flag_in, shortname);

        snprintf(msg, sizeof(msg), "增加了工作时间");
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "update"))
    {
        pasDbExecSqlF("update ncworktime set timetype = '%s',llower = '%u',lupper = '%u',starttime = '%u',stoptime = '%u',flags = '%s' where sid = '%s'", ltype_in, llower_tmp, lupper_tmp, start_time_tmp, stop_time_tmp, time_flag_in, sid_in);

        snprintf(msg, sizeof(msg), "修改了工作时间id:[%s]", sid_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "delete"))
    {
        pasDbExecSqlF("update ncworktime set flag=1,modtime=%lu where sid in(%s)", time(0), sid_in);

        snprintf(msg, sizeof(msg), "删除了工作时间id:[%s]", sid_in);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }
    else if(!strcmp(deal_in, "select"))
    {
        snprintf(msg, sizeof(msg), "查询工作时间");
        writeSysLog(psMsgHead, "06", "Success", msg);

        if(!utStrIsSpaces(time_flag_in))
        {
            snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and flags = %s", time_flag_in);
        }

        if(!utStrIsSpaces(ltype_in))
        {
            snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and timetype = %s", ltype_in);
        }
    }

    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();
    int db_count = 0;

    snprintf(sql, sizeof(sql), "select count(*) from ncworktime where ncworktime.flag=0 and createuser='%s' %s order by sid", shortname, sql_tmp);
#if WORK_TIME_FLAG
    printf("count:%s\n", sql);
#endif

    psCur = pasDbOpenSql(sql, 0);
    pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &db_count);
    pasDbCloseCursor(psCur);
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);

    snprintf(sql, sizeof(sql), "select sid,timetype,llower,lupper,starttime,stoptime,flags from ncworktime where  ncworktime.flag=0 and createuser='%s' %s order by sid limit %u,%u", shortname, sql_tmp, start, limit);

#if WORK_TIME_FLAG
    printf("%s\n", sql);
#endif

    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned long sid = 0;
        char time_group[32] = "";
        int type = 0;
        int lower = 0;
        int upper = 0;
        int start_time = 0;
        int stop_time = 0;
        int time_flag = 0;

        int iret = -1;
        int iNum = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  4,                 &sid,
                                          UT_TYPE_LONG,   4,                 &type,
                                          UT_TYPE_LONG,  4,                      &lower,
                                          UT_TYPE_LONG,  4,                      &upper,
                                          UT_TYPE_LONG,  4,                      &start_time,
                                          UT_TYPE_LONG,  4,                      &stop_time,
                                          UT_TYPE_LONG,  4,                      &time_flag)) || 1405 == iret)
        {
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            printf("stop_time: %d\n", stop_time);
            char lower_tmp2[24] = "";
            char upper_tmp2[24] = "";
            char start_time_tmp2[24] = "";
            char stop_time_tmp2[24] = "";

            dateNumtoStr(lower, lower_tmp2);
            dateNumtoStr(upper, upper_tmp2);
            timeNumtoStr(start_time, start_time_tmp2);
            timeNumtoStr(stop_time, stop_time_tmp2);

            utPltPutLoopVarF(psDbHead, "sid", iNum, "%u", sid);
            utPltPutLoopVarF(psDbHead, "timeFlag", iNum, "%u", time_flag);
            utPltPutLoopVarF(psDbHead, "timeType", iNum, "%u", type);
            utPltPutLoopVar(psDbHead, "lower", iNum, lower_tmp2);
            utPltPutLoopVar(psDbHead, "upper", iNum, upper_tmp2);
            utPltPutLoopVar(psDbHead, "startTime", iNum, start_time_tmp2);
            utPltPutLoopVar(psDbHead, "stopTime", iNum, stop_time_tmp2);
        }
        pasDbCloseCursor(psCur);

    }

#if WORK_TIME_FLAG
    utPltShowDb(psDbHead);
#endif

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_work_time.htm");

    return 0;
}

//部门树
int lan_group_tree_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
#if GROUP_TREE_FLAG
    utMsgPrintMsg(psMsgHead);
#endif

    char nodeid_in[16] = "";
    char gid_in[16] = "";
    char move_to_id_in[16] = "";
    char deal_in[8] = "";
    char gname_in[32] = "";
    char sip_in[16] = "";
    char eip_in[16] = "";
    char content_in[128] = "";

    utMsgGetSomeNVar(psMsgHead, 8,
                     "id",   UT_TYPE_STRING,  sizeof(nodeid_in) - 1,   nodeid_in,
                     "gid",  UT_TYPE_STRING,  sizeof(gid_in) - 1,  gid_in,
                     "toid", UT_TYPE_STRING,  sizeof(move_to_id_in) - 1,   move_to_id_in,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "gname",    UT_TYPE_STRING,  sizeof(gname_in) - 1,    gname_in,
                     "sip",  UT_TYPE_STRING,  sizeof(sip_in) - 1,  sip_in,
                     "eip",  UT_TYPE_STRING,  sizeof(eip_in) - 1,  eip_in,
                     "content",  UT_TYPE_STRING,  sizeof(content_in) - 1,  content_in);

    if(!strcmp(nodeid_in, "root"))
    {
        strcpy(nodeid_in, "0");
    }

    char sql[1024] = "";
    char msg[2048] = "";
    if(!utStrIsSpaces(deal_in))
    {
        char *group_name_gbk = utf8convert(psShmHead, gname_in);
        char *group_content_gbk = utf8convert(psShmHead, content_in);

        //修改内存里的部门信息

        if(!strcmp(deal_in, "insert"))
        {
            snprintf(sql, sizeof(sql), "insert into nwgroup(gname,begip,endip,pid,content)values('%s',%u,%u,%s,'%s')", group_name_gbk, htonl(inet_addr(sip_in)), htonl(inet_addr(eip_in)), gid_in, group_content_gbk);
            printf("sql:%s\n", sql);
            pasDbExecSqlF(sql);

            snprintf(msg, sizeof(msg), "增加了[%s]部门到[%s]下", group_name_gbk, lanGetGroup(psShmHead, strtoul(gid_in, NULL, 10)));
            writeSysLog(psMsgHead, "05", "Success", msg);

            unsigned long gid = 0;
            pasDbOneRecord("select max(gid) from nwgroup", 0, UT_TYPE_LONG, 4, &gid);
            if(gid != 0)
            {
                lanGroupInfo *psGroup;
                psGroup = (lanGroupInfo *)utShmHashLookA(psShmHead, NC_LNK_NW_GROUP, &gid);
                if(psGroup)
                {
                    strcpy(psGroup->caGroupname, group_name_gbk);
                }
            }
        }
        else if(!strcmp(deal_in, "update"))
        {
            pasDbExecSqlF("update nwgroup set gname = '%s',begip = %u,endip = %u,content = '%s' where gid = %s", group_name_gbk, htonl(inet_addr(sip_in)), htonl(inet_addr(eip_in)), group_content_gbk, gid_in);

            snprintf(msg, sizeof(msg), "修改了[%s]部门为[%s]", lanGetGroup(psShmHead, strtoul(gid_in, NULL, 10)), group_name_gbk);
            writeSysLog(psMsgHead, "05", "Success", msg);

            char *groupid_tmp = gid_in;
            char *groupid_p = gid_in;
            unsigned long groupid = 0;
            lanGroupInfo *psGroup;
            while(*groupid_tmp != '\0')
            {
                if(*groupid_tmp == ',')
                {
                    *groupid_tmp = '\0';
                    groupid = strtoul(groupid_p, NULL, 10);
                    psGroup = (lanGroupInfo *)utShmHashLookA(psShmHead, NC_LNK_NW_GROUP, &groupid);
                    if(psGroup)
                    {
                        strcpy(psGroup->caGroupname, group_name_gbk);
                    }
                    groupid_p = groupid_tmp + 1;
                }
                groupid_tmp++;
            }

        }
        else if(!strcmp(deal_in, "delete"))
        {
            pasDbExecSqlF("delete from nwgroup where gid in (%s) or pid in (%s)", gid_in, gid_in);
            //操作日志
            char *groupid_tmp = gid_in;
            char *groupid_p = gid_in;
            unsigned long groupid = 0;
            snprintf(msg, sizeof(msg), "删除了部门");
            while(*groupid_tmp != '\0')
            {
                if(*groupid_tmp == ',')
                {
                    *groupid_tmp = '\0';
                    groupid = strtoul(groupid_p, NULL, 10);
                    snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), "[%s]", lanGetGroup(psShmHead, groupid));
                    groupid_p = groupid_tmp + 1;
                }
                groupid_tmp++;
            }
            groupid = strtoul(groupid_p, NULL, 10);
            snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), "[%s]", lanGetGroup(psShmHead, groupid));
            writeSysLog(psMsgHead, "05", "Success", msg);
        }
        else if(!strcmp(deal_in, "move"))
        {
            pasDbExecSqlF("update nwgroup set pid = %s where gid = %s", move_to_id_in, gid_in);
            snprintf(msg, sizeof(msg), "移动部门[%s]到", lanGetGroup(psShmHead, strtoul(gid_in, NULL, 10)));
            snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), "[%s]部门", lanGetGroup(psShmHead, strtoul(move_to_id_in, NULL, 10)));
            writeSysLog(psMsgHead, "05", "Success", msg);
        }
    }


    snprintf(sql, sizeof(sql), "select gid,gname,inet_ntoa(begip),inet_ntoa(endip),content,pid from nwgroup");

    if(*nodeid_in == '0')
    {
        //权限
        char *auth = getDsGroupid();
        if(!utStrIsSpaces(auth))
        {
            snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " where gid = %d", atoi(auth));
        }

    }
    else
    {
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " where pid = %s", nodeid_in);
    }

#if GROUP_TREE_FLAG
    printf("sql:%s\n", sql);
#endif

    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();

    psCur = pasDbOpenSql(sql, 0);
    printf("here\n");
    if(NULL != psCur)
    {
        unsigned int id = 0;
        char gname[32] = "";
        char begip[16] = "";
        char endip[16] = "";
        char content[256] = "";
        int iret = -1;
        int iNum = 0;
        unsigned long pid = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG, 4, &id,
                                          UT_TYPE_STRING, sizeof(gname), gname,
                                          UT_TYPE_STRING, sizeof(begip), begip,
                                          UT_TYPE_STRING, sizeof(endip), endip,
                                          UT_TYPE_STRING, sizeof(content), content,
                                          UT_TYPE_ULONG, sizeof(pid), &pid)) || 1405 == iret)
        {
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }

            utPltPutLoopVarF(psDbHead, "id", iNum, "%u", id);
            utPltSetCvtHtml(1);
            if(pid == 0)
            {
                utPltPutLoopVar(psDbHead, "gname", iNum, ncLang("0011全部"));
            }
            else
            {
                utPltPutLoopVar(psDbHead, "gname", iNum, gname);
            }
            utPltPutLoopVar(psDbHead, "begip", iNum, begip);
            utPltPutLoopVar(psDbHead, "endip", iNum, endip);
            utPltPutLoopVar(psDbHead, "content", iNum, content);
            utPltSetCvtHtml(0);
        }
        pasDbCloseCursor(psCur);
#if GROUP_TREE_FLAG
        utPltShowDb(psDbHead);
#endif
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_group_tree.htm");
    }

    return 0;
}
//客户端管理
int lan_client_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
#if CLIENT_FLAG
    utMsgPrintMsg(psMsgHead);
#endif
    char deal_in[8] = "";
    char keyword_in[128] = "";
    char mid_in[16] = "";
    char devname_in[32] = "";
    char compid_in[640] = "";
    char groupid_in[16] = "";
    char sort_in[16] = "";
    char dir_in[16] = "";
    char start_in[8] = "";
    char limit_in[8] = "";
    char status_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 11,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "keyword",  UT_TYPE_STRING,  sizeof(keyword_in) - 1,  keyword_in,
                     "mid",  UT_TYPE_STRING,  sizeof(mid_in) - 1,  mid_in,
                     "devName",  UT_TYPE_STRING,  sizeof(devname_in) - 1,  devname_in,
                     "compid",   UT_TYPE_STRING,  sizeof(compid_in) - 1,   compid_in,
                     "groupid",  UT_TYPE_STRING,  sizeof(groupid_in) - 1,  groupid_in,
                     "status",   UT_TYPE_STRING,  sizeof(status_in) - 1,   status_in,
                     "sort", UT_TYPE_STRING,  sizeof(sort_in) - 1,     sort_in,
                     "dir",  UT_TYPE_STRING,  sizeof(dir_in) - 1,  dir_in,
                     "start",    UT_TYPE_STRING,  sizeof(start_in) - 1,    start_in,
                     "limit",    UT_TYPE_STRING,  sizeof(limit_in) - 1,    limit_in);

    char sql_tmp[1024] = " where 1=1 ";
    char sql[1024] = "";

    char msg[2048] = "";

    if(!strcmp(deal_in, "update"))
    {
        char *devname_gbk = utf8convert(psShmHead, devname_in);
        unsigned int compid = strtoul(compid_in, NULL, 10);
        nwCompInfo *psCompInfo = NULL;
        psCompInfo = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
        if(!utStrIsSpaces(mid_in))
        {
            snprintf(sql, sizeof(sql), "update nwcompute set groupid = %s,devname = '%s' where compid = %s", mid_in, devname_gbk, compid_in);
            if(psCompInfo)
            {
                psCompInfo->groupid = atoi(mid_in);
                strcpy(psCompInfo->devname, devname_gbk);
                snprintf(msg, sizeof(msg), "修改了[%s]客户端的部门为[%s]和用户名为[%s]", psCompInfo->compname, lanGetGroup(psShmHead, strtoul(mid_in, NULL, 10)), devname_gbk);
            }
        }
        else
        {
            snprintf(sql, sizeof(sql), "update nwcompute set devname = '%s' where compid = %s", devname_gbk, compid_in);
            if(psCompInfo)
            {
                strcpy(psCompInfo->devname, devname_gbk);
                snprintf(msg, sizeof(msg), "修改了[%s]客户端的用户名为[%s]", psCompInfo->compname, devname_gbk);
            }
        }
        writeSysLog(psMsgHead, "05", "Success", msg);
        pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in, "move"))
    {
        pasDbExecSqlF("update nwcompute set groupid = %s where compid in (%s)", mid_in, compid_in);
        char *compid_p = compid_in;
        char *compid_tmp = compid_in;

        unsigned long compid = 0;
        nwCompInfo *psCompInfo = NULL;

        snprintf(msg, sizeof(msg), "移动");
        while(*compid_tmp != '\0')
        {
            if(*compid_tmp == ',')
            {
                *compid_tmp = '\0';
                compid = strtoul(compid_p, NULL, 10);
                psCompInfo = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
                if(psCompInfo)
                {
                    psCompInfo->groupid = atoi(mid_in);
                    snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), "[%s]", psCompInfo->compname);
                }
                compid_p = compid_tmp + 1;
            }
            compid_tmp++;
        }
        compid = strtoul(compid_p, NULL, 10);
        psCompInfo = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
        if(psCompInfo)
        {
            psCompInfo->groupid = atoi(mid_in);
            snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), "[%s]", psCompInfo->compname);
        }
        snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), "]到部门[%s]", lanGetGroup(psShmHead, strtoul(mid_in, NULL, 10)));
        writeSysLog(psMsgHead, "05", "Success", msg);
    }
    else if(!strcmp(deal_in, "delete"))
    {
        pasDbExecSqlF("delete from nwcompute where compid in (%s)", compid_in);
        char *compid_p = compid_in;
        char *compid_tmp = compid_in;

        unsigned long compid = 0;
        nwCompInfo *psCompInfo = NULL;

        snprintf(msg, sizeof(msg), "删除了");
        char *compname = NULL;
        while(*compid_tmp != '\0')
        {
            if(*compid_tmp == ',')
            {
                *compid_tmp = '\0';
                compid = strtoul(compid_p, NULL, 10);
                compname = findCompNameById(strtoul(compid_in, NULL, 10));
                if(psCompInfo)
                {
                    psCompInfo->groupid = atoi(mid_in);
                    snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), "[%s]", compname);
                }
                compid_p = compid_tmp + 1;
            }
            compid_tmp++;
        }
        free(compname);
        compid = strtoul(compid_p, NULL, 10);
        compname = findCompNameById(strtoul(compid_in, NULL, 10));
        snprintf(msg + strlen(msg), sizeof(msg) - strlen(msg), "[%s]", compname);
        writeSysLog(psMsgHead, "05", "Success", msg);
    }
    else if(!strcmp(deal_in, "bind"))
    {
        pasDbExecSqlF("update nwcompute set bindip = ip where compid in (%s)", compid_in);
    }
    else if(!strcmp(deal_in, "unbind"))
    {
        pasDbExecSqlF("update nwcompute set bindip = '' where compid in (%s)", compid_in);
    }
    else if(!utStrIsSpaces(compid_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and compid = %s", compid_in);
        char *compname = findCompNameById(strtoul(compid_in, NULL, 10));
        snprintf(msg, sizeof(msg), "查询[%s]客户端管理信息", compname);
        free(compname);
        writeSysLog(psMsgHead, "06", "Success", msg);
    }
    //权限
    char *auth = getDsGroupid();
    if(!utStrIsSpaces(auth))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and groupid in (%s)", auth);
    }
    if(!utStrIsSpaces(groupid_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and groupid in (%s)", getGroupid(strtoul(groupid_in, NULL, 10)));
    }
    if(atoi(status_in) == 2)
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and status=2");
    }

    if(!utStrIsSpaces(keyword_in))
    {
        char *keyword_gbk = utf8convert(psShmHead, keyword_in);
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and (compname like '%%%s%%' or devname like '%%%s%%' or inet_ntoa(ip) like '%%%s%%' or mac like '%%%s%%')", keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk);
    }

    if(strlen(msg) == 0)
    {
        snprintf(msg, sizeof(msg), "查询[全部]客户端管理信息");
        writeSysLog(psMsgHead, "06", "Success", msg);
    }

    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();

    int db_count = 0;

    snprintf(sql, sizeof(sql), "select count(*) from nwcompute %s", sql_tmp);

#if CLIENT_FLAG
    printf("sql_count:%s\n", sql);
#endif

    psCur = pasDbOpenSql(sql, 0);
    pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &db_count);
    pasDbCloseCursor(psCur);
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);

    snprintf(sql, sizeof(sql), "select compname,inet_ntoa(ip),devname,version,FROM_UNIXTIME(addtime,'%%Y/%%m/%%d %%H:%%I:%%S'),FROM_UNIXTIME(lasttime,'%%Y/%%m/%%d %%H:%%I:%%S'),groupid,compid,mac,status,inet_ntoa(bindip) from nwcompute %s", sql_tmp);

    if(!utStrIsSpaces(sort_in))
    {
        if(!strcmp(sort_in, "logTime"))
        {
            strcpy(sort_in, "addtime");
        }
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " order by %s %s", sort_in, dir_in);
    }

    snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " limit %s,%s", start_in, limit_in);

#if CLIENT_FLAG
    printf("sql:%s\n", sql);
#endif
    writeLog("client.txt", "select", sql);
    psCur = pasDbOpenSql(sql, 0);
    nwCompIp *psCompIp = NULL;

    if(psCur != NULL)
    {
        unsigned int sid = 0;
        char compute_name[32] = "";
        char ip[16] = "";
        char bindip[16] = "";
        char dev_name[32] = "";
        char version[20] = "";
        char log_time[24] = "";
        char last_time[24] = "";
        unsigned int group_id = 0;
        unsigned int comp_id = 0;
        char mac[18] = "";
        int iNum = 0;
        int iret = -1;
        unsigned int status = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_STRING, sizeof(compute_name), compute_name,
                                          UT_TYPE_STRING, sizeof(ip), ip,
                                          UT_TYPE_STRING, sizeof(dev_name), dev_name,
                                          UT_TYPE_STRING, sizeof(version), version,
                                          UT_TYPE_STRING, sizeof(log_time), log_time,
                                          UT_TYPE_STRING, sizeof(last_time), last_time,
                                          UT_TYPE_ULONG, sizeof(group_id), &group_id,
                                          UT_TYPE_ULONG, sizeof(comp_id), &comp_id,
                                          UT_TYPE_STRING, sizeof(mac), mac,
                                          UT_TYPE_ULONG, sizeof(status), &status,
                                          UT_TYPE_STRING, sizeof(bindip), bindip
                                         )) || 1405 == iret)
        {

            psCompIp = (nwCompIp *)utShmHashLook(psShmHead, NC_LNK_ONLINECOMP, &comp_id);
            if(atoi(status_in) == 1 && !psCompIp)
            {
                continue;
            }

            if(atoi(status_in) == 3 && (psCompIp || status == 2))
            {
                continue;
            }

            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            utPltPutLoopVar(psDbHead, "compName", iNum, compute_name);
            utPltPutLoopVar(psDbHead, "ip", iNum, ip);
            utPltPutLoopVar(psDbHead, "devName", iNum, dev_name);
            utPltPutLoopVar(psDbHead, "version", iNum, version);
            utPltPutLoopVar(psDbHead, "logTime", iNum, log_time);
            if(psCompIp)
            {
                utPltPutLoopVar(psDbHead, "lastTime", iNum, utTimFormat("%Y/%m/%d %H:%M:%S", psCompIp->lLastTime));
            }
            else
            {
                utPltPutLoopVar(psDbHead, "lastTime", iNum, last_time);
            }
            utPltPutLoopVar(psDbHead, "groupName", iNum, lanGetGroup(psShmHead, group_id));
            utPltPutLoopVarF(psDbHead, "compId", iNum, "%u", comp_id);

            if(!psCompIp && status != 2)
            {
                status = 3;
            }
            utPltPutLoopVarF(psDbHead, "status", iNum, "%u", status);
            utPltPutLoopVar(psDbHead, "mac", iNum, mac);
            utPltPutLoopVar(psDbHead, "bindip", iNum, bindip);
        }
        pasDbCloseCursor(psCur);

#if CLIENT_FLAG
        utPltShowDb(psDbHead);
#endif
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_client.htm");
    }
}

int getHardList(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char sql[1024];

    strcpy(sql, "select gname,compname,os,nwhardware.mac,hardType,deviceDesc,from_unixtime(nwhardware.addtime),from_unixtime(nwhardware.lasttime) from nwcompute left join nwhardware on nwhardware.compid=nwcompute.compid left join nwgroup on groupid=gid where InfoType='hardtype'");

    //权限
    char *auth = getDsGroupid();
    if(!utStrIsSpaces(auth))
    {
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " and groupid in (%s)", auth);
    }

    FILE *fp;
    char caFilename[32];
    char caFile[128];
    char caPath[128];

    sprintf(caFilename, "nwdevicelist.csv");
    sprintf(caPath, "%s/html", "/home/ncmysql/nw");
    sprintf(caFile, "%s/%s", caPath, caFilename);
    fp = fopen(caFile, "w");
    if(fp == NULL)
    {
        printf("open failed!\n");
        writeSysLog(psMsgHead, "06", "Fail", "创建设备清单文件失败");
        return 0;
    }

    ncUtlPrintCsv(fp, 1, UT_TYPE_STRING, ncLang("0109设备清单"));
    ncUtlPrintCsv(fp, 8, UT_TYPE_STRING, ncLang("0069部门"),
                  UT_TYPE_STRING, ncLang("0066计算机名称"),
                  UT_TYPE_STRING, ncLang("0110操作系统"),
                  UT_TYPE_STRING, ncLang("0068MAC地址"),
                  UT_TYPE_STRING, ncLang("0111硬件类型"),
                  UT_TYPE_STRING, ncLang("0112设备名"),
                  UT_TYPE_STRING, ncLang("0113装入时间"),
                  UT_TYPE_STRING, ncLang("0114最近使用时间"));

    pasDbCursor *psCur = NULL;
    int iret = 0;
    int iNum = 0;
    utPltDbHead *psDbHead = utPltInitDb();
    psCur = pasDbOpenSql(sql, 0);

    if(psCur)
    {
        char gname[32] = "";
        char compname[32] = "";
        char os[32] = "";
        char mac[18] = "";
        char hardType[24] = "";
        char devname[128] = "";
        char addtime[24] = "";
        char lasttime[24] = "";

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_STRING,    sizeof(gname) - 1,        gname,
                                          UT_TYPE_STRING,    sizeof(compname) - 1, compname,
                                          UT_TYPE_STRING,    sizeof(os) - 1,           os,
                                          UT_TYPE_STRING,  sizeof(mac) - 1,            mac,
                                          UT_TYPE_STRING,    sizeof(hardType) - 1, hardType,
                                          UT_TYPE_STRING,    sizeof(devname) - 1,  devname,
                                          UT_TYPE_STRING,    sizeof(addtime) - 1,  addtime,
                                          UT_TYPE_STRING,    sizeof(lasttime) - 1, lasttime)) || 1405 == iret)
        {
            ncUtlPrintCsv(fp, 8,
                          UT_TYPE_STRING, gname,
                          UT_TYPE_STRING, compname,
                          UT_TYPE_STRING, os,
                          UT_TYPE_STRING, mac,
                          UT_TYPE_STRING, hardType,
                          UT_TYPE_STRING, devname,
                          UT_TYPE_STRING, addtime,
                          UT_TYPE_STRING, lasttime);
        }
        pasDbCloseCursor(psCur);
        fclose(fp);
        utPltFileDownload(iFd, "application/text", caPath, caFilename, caFilename);
        remove(caFile);
        utPltFreeDb(psDbHead);
        writeSysLog(psMsgHead, "06", "Success", "导出设备清单");
    }

    return 0;
}

int lanIgnoreProc(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    char deal_in[16] = "";
    char id_in[16] = "";
    char keywords_in[64] = "";
    char procname_in[256] = "";
    char procdetail_in[256] = "";
    char start_in[8] = "";
    char limit_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 7,
                     "deal",         UT_TYPE_STRING,  sizeof(deal_in) - 1,         deal_in,
                     "id",             UT_TYPE_STRING,  sizeof(id_in) - 1,           id_in,
                     "keywords",     UT_TYPE_STRING,  sizeof(keywords_in) - 1,   keywords_in,
                     "procname",     UT_TYPE_STRING,  sizeof(procname_in) - 1,   procname_in,
                     "procydetail",  UT_TYPE_STRING,  sizeof(procdetail_in) - 1, procdetail_in,
                     "start",          UT_TYPE_STRING,  sizeof(start_in) - 1,      start_in,
                     "limit",          UT_TYPE_STRING,  sizeof(limit_in) - 1,      limit_in);
    char sql[1024] = "";
    char sql_tmp[512] = "";
    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);
    findShortName(auth, shortname);

    char procname[64] = "";
    pasStrCvtHex2Bin(procname_in, procname);

    if(!strcmp(deal_in, "move"))
    {
        sprintf(sql, "insert nwprocignore(processname,content,company)values('%s','%s','%s')", procname, utf8convert(psShmHead, procdetail_in), shortname);
    }
    else if(!strcmp(deal_in, "delete"))
    {
        sprintf(sql, "delete from nwprocignore where recno=%s", id_in);
    }

    writeLog("ignoreProc.txt", "excl", sql);
    pasDbExecSqlF(sql);

    if(!utStrIsSpaces(keywords_in))
    {
        sprintf(sql_tmp, " and processname like '%%%s%%' or content like '%%%s%%'", utf8convert(psShmHead, keywords_in), utf8convert(psShmHead, keywords_in));
    }

    sprintf(sql, "select count(*) from nwprocignore where company = '%s' %s", shortname, sql_tmp);
    unsigned int db_count = 0;

    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();

    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(db_count), &db_count);
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);

    sprintf(sql, "select recno,processname,content from nwprocignore where company = '%s' %s limit %s,%s", shortname, sql_tmp, start_in, limit_in);

    writeLog("ignoreProc.txt", "search", sql);
    psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
    {
        unsigned long recno = 0;
        char procname[64] = "";
        char detail[128] = "";

        int iret = 0;
        int iNum = 0;
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  sizeof(recno),      &recno,
                                          UT_TYPE_STRING, sizeof(procname) - 1, procname,
                                          UT_TYPE_STRING, sizeof(detail) - 1,   detail)) || 1405 == iret)
        {
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            utPltPutLoopVarF(psDbHead, "id", iNum, "%lu", recno);
            utPltPutLoopVar(psDbHead, "name", iNum, procname);
            utPltPutLoopVar(psDbHead, "detail", iNum, detail);
        }
        pasDbCloseCursor(psCur);
    }

    utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_ignoreProc.htm");

    return 0;
}

