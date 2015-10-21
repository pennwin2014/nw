#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoplt01.h"

static char *utf8convert(char *src)
{
    char *strutf8 = NULL;
    if(src != NULL)
    {
        strutf8 = (char*)convert("UTF-8", "GBK", src);

        if(strutf8)
        {
            return strutf8;
        }
        else
        {
            return src;
        }
    }
    return src;
}

char* getShortNameByGroupid(ulong lGroupid)
{
    static char shortname[256] = "";
    char sql[1024] = "";
    ulong pid = 0;
    memset(shortname, 0, sizeof(shortname));
    pid = findCompanyByGroupid(lGroupid);
    sprintf(sql, "select gname from nwgroup where gid = %lu", pid);
    pasDbOneRecord(sql, 0, UT_TYPE_STRING, sizeof(shortname) - 1, shortname);
    //printf("get shortname=%s, pid=%lu, groupid=%lu\n", shortname, pid, lGroupid);
    return shortname;
}

char* getLoginShortName()
{
    ulong lUserid = 0, lGroupid = 0;
    char sql[1024] = "";
    dsCltGetMyInfo(1, "Userid", &lUserid);
    sprintf(sql, "select groupid from dsuser where id = %lu", lUserid);
    printf("user=%lu,sql=[%s]\n", lUserid, sql);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &lGroupid);
    return getShortNameByGroupid(lGroupid);
}


int checkTableExists(char* tableName)
{
    char sql[1024] = "";
    int ret = 0;
    pasDbCursor *psCur = NULL;
    snprintf(sql, sizeof(sql), "select count(*) from %s", tableName);
    psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
    {
        ret = 1;
        pasDbCloseCursor(psCur);
    }
    return ret;
}


char* getNewLogTable(char* shortName, char* tableOri, int year, int month)
{
    static char newTable[256] = "";
    if((strlen(shortName) > 0) && (strcmp(shortName, "lan") != 0))
        snprintf(newTable, sizeof(newTable) - 1, "%s_%s_%04u%02u", shortName, tableOri, year, month);
    else
        snprintf(newTable, sizeof(newTable) - 1, "%s_%04u%02u", tableOri, year, month);
	char sql[1024] = "";
	if(!checkTableExists(newTable))
    {
        snprintf(sql, sizeof(sql) - 1, "create table %s like %s", newTable, tableOri);
        printf("not exist logTable=%s, do sql=%s\n", newTable, sql);
        pasDbExecSqlF(sql);
    }
    return newTable;
}

char* getNewTableName(char* shortName, char* tableName)
{
    static char newTable[256] = "";
    memset(newTable, 0, sizeof(newTable));
    if((strlen(shortName) > 0) && (strcmp(shortName, "lan") != 0))
        snprintf(newTable, sizeof(newTable) - 1, "%s_%s", shortName, tableName);
    else
        snprintf(newTable, sizeof(newTable) - 1, "%s", tableName);
	
    //printf("get newTable=%s\n", newTable);
    return newTable;
}


char* getNewTable(char* shortName, char* tableName)
{
    static char newTable[256] = "";
    memset(newTable, 0, sizeof(newTable));
    char sql[1024] = "";
    if((strlen(shortName) > 0) && (strcmp(shortName, "lan") != 0))
        snprintf(newTable, sizeof(newTable) - 1, "%s_%s", shortName, tableName);
    else
        snprintf(newTable, sizeof(newTable) - 1, "%s", tableName);

    //printf("get newTable=%s\n", newTable);
    //先查询是否有这张表
    if(!checkTableExists(newTable))
    {
        snprintf(sql, sizeof(sql) - 1, "create table %s like %s", newTable, tableName);
        printf("not exist newTable=%s, do sql=%s\n", newTable, sql);
        pasDbExecSqlF(sql);
    }
    return newTable;
}

int lan_compuser(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);
    char deal_in[8] = "";
    char sid_in[16] = "";
    char compname_in[256] = "";
    char compaddr_in[256] = "";
    char compsize_in[16] = "";
    char compphone_in[16] = "";
    char compfax_in[16] = "";
    char compweb_in[128] = "";
    char compmail_in[128] = "";
    char compband_in[16] = "";
    char applname_in[32] = "";
    char applsex_in[8] = "";
    char appljob_in[32] = "";
    char appldepart_in[128] = "";
    char applphone_in[16] = "";
    char applmobile_in[16] = "";
    char applmail_in[128] = "";
    char applqq_in[16] = "";
    char bindmail_in[24] = "";
    char shortname_in[16] = "";
    char clientnums_in[8] = "";
    char datebeg_in[24] = "";
    char days_in[8] = "";
    char keywords_in[32] = "";
    char start_in[8] = "";
    char limit_in[8] = "";
    char caSelShortNames[1024] = "";
    char caGids[1024] = "", caIds[1024] = "";

    utMsgGetSomeNVar(psMsgHead, 29,
                     "deal",             UT_TYPE_STRING, sizeof(deal_in) - 1,            deal_in,
                     "sid",              UT_TYPE_STRING, sizeof(sid_in) - 1,             sid_in,
                     "compname",     UT_TYPE_STRING, sizeof(compname_in) - 1,    compname_in,
                     "compaddr",     UT_TYPE_STRING, sizeof(compaddr_in) - 1,    compaddr_in,
                     "compsize",     UT_TYPE_STRING, sizeof(compsize_in) - 1,    compsize_in,
                     "compphone",  UT_TYPE_STRING, sizeof(compphone_in) - 1, compphone_in,
                     "compfax",      UT_TYPE_STRING, sizeof(compfax_in) - 1,     compfax_in,
                     "compweb",      UT_TYPE_STRING, sizeof(compweb_in) - 1,     compweb_in,
                     "compmail",     UT_TYPE_STRING, sizeof(compmail_in) - 1,    compmail_in,
                     "compband",     UT_TYPE_STRING, sizeof(compband_in) - 1,    compband_in,
                     "applname",     UT_TYPE_STRING, sizeof(applname_in) - 1,    applname_in,
                     "applsex",      UT_TYPE_STRING, sizeof(applsex_in) - 1,     applsex_in,
                     "appljob",      UT_TYPE_STRING, sizeof(appljob_in) - 1,     appljob_in,
                     "appldepart", UT_TYPE_STRING, sizeof(appldepart_in) - 1, appldepart_in,
                     "applphone",    UT_TYPE_STRING, sizeof(applphone_in) - 1,   applphone_in,
                     "applmobile", UT_TYPE_STRING, sizeof(applmobile_in) - 1, applmobile_in,
                     "applmail",     UT_TYPE_STRING, sizeof(applmail_in) - 1,    applmail_in,
                     "applqq",           UT_TYPE_STRING, sizeof(applqq_in) - 1,      applqq_in,
                     "bindmail",   UT_TYPE_STRING, sizeof(bindmail_in) - 1,  bindmail_in,
                     "shortname",  UT_TYPE_STRING, sizeof(shortname_in) - 1, shortname_in,
                     "clientnums",   UT_TYPE_STRING, sizeof(clientnums_in) - 1, clientnums_in,
                     "datebeg",      UT_TYPE_STRING, sizeof(datebeg_in) - 1,     datebeg_in,
                     "days",             UT_TYPE_STRING, sizeof(days_in) - 1,            days_in,
                     "keywords",     UT_TYPE_STRING, sizeof(keywords_in) - 1,    keywords_in,
                     "start",            UT_TYPE_STRING, sizeof(start_in) - 1,           start_in,
                     "limit",            UT_TYPE_STRING, sizeof(limit_in) - 1,           limit_in,
                     "selshortnames",          UT_TYPE_STRING, sizeof(caSelShortNames) - 1, caSelShortNames);


    char sql[1024] = "";
    char sqlTmp[512] = "";
    unsigned long datebeg = utTimStrToLong("%Y/%m/%d %H:%M:%S", datebeg_in);
    pasDbCursor *psCur = NULL;
    int iret = 0;
    if(!strcmp(deal_in, "insert"))
    {
        printf("compname_in=[%s]\n", compname_in);
        printf("insert '%s'\n", utf8convert(compname_in));
        char *compname_gbk = utf8convert(compname_in);
        char *shortname_gbk = utf8convert(shortname_in);
        sprintf(sql, "insert into userapply(compname,compaddr,compsize,compphone,compfax,compweb,compmail,compband,applname,applsex,appljob,appldepart,applphone,applmobile,applmail,applqq,bindmail,shortname,clientnum,datebeg,days,modtime)values('%s','%s',%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%u,%s,%u)", utf8convert(compname_in), utf8convert(compaddr_in), atoi(compsize_in), compphone_in, compfax_in, compweb_in, compmail_in, compband_in, utf8convert(applname_in), utf8convert(applsex_in), utf8convert(appljob_in), utf8convert(appldepart_in), applphone_in, applmobile_in, utf8convert(applmail_in), applqq_in, utf8convert(bindmail_in), utf8convert(shortname_in), clientnums_in, datebeg, days_in, time(0));

        printf("sql:insert apply:%s\n", sql);
        pasDbExecSqlF(sql);
        if(strlen(shortname_in))
        {
            sprintf(sql, "insert into nwgroup(gname,pid)values('%s',0)", shortname_gbk);
            //添加超级管理员帐户
            printf("sql:insert group:%s\n", sql);
            pasDbExecSqlF(sql);
        }

        unsigned int groupid = 0;

        sprintf(sql, "select gid from nwgroup where gname = '%s'", shortname_gbk);

        pasDbOneRecord(sql, 0, UT_TYPE_LONG, sizeof(groupid), &groupid);

        char temp[32] = "";
        char caKey0[64] = "";
        char caKey[64] = "";

        sprintf(temp, "%sadmin", shortname_in);
        int lUserid = dsCltGetSeque();

        sprintf(caKey0, "%s%s", temp, temp);
        utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey); /* 口令加密  */
        unsigned long ltime = time(0);

        sprintf(sql, "insert into dsuser(id,name,dispname,groupid,groupname,addtime,moditime,lkey,lasttime,logcount,useflags,status,usrlevel,email,opt) values(%lu,'%s','%s',%lu,'%s',%lu,%lu,'%s',%lu,0,0,0,1,'%s','%u')", lUserid, temp, compname_gbk, groupid, "", ltime, ltime, caKey, ltime, "", groupid);
        pasDbExecSqlF(sql);

        sprintf(sql, "insert into dsuserrole(id,roleid,addtime,addby) values(%lu,%lu,%lu,'%s')", lUserid, 102, ltime, "admin");
        pasDbExecSqlF(sql);

        //添加日志查看账户
        sprintf(temp, "%skeeper", shortname_in);
        lUserid = dsCltGetSeque();

        sprintf(caKey0, "%s%s", temp, temp);
        utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey); /* 口令加密  */

        sprintf(sql, "insert into dsuser(id,name,dispname,groupid,groupname,addtime,moditime,lkey,lasttime,logcount,useflags,status,usrlevel,email,opt) values(%lu,'%s','%s',%lu,'%s',%lu,%lu,'%s',%lu,0,0,0,1,'%s','%u')", lUserid, temp, compname_gbk, groupid, "", ltime, ltime, caKey, ltime, "", groupid);
        pasDbExecSqlF(sql);

        sprintf(sql, "insert into dsuserrole(id,roleid,addtime,addby) values(%lu,%lu,%lu,'%s')", lUserid, 103, ltime, "admin");
        pasDbExecSqlF(sql);

    }
    else if(!strcmp(deal_in, "update"))
    {
        sprintf(sql, "update userapply set compaddr='%s',compsize=%d,compphone='%s',compfax='%s',compweb='%s',compmail='%s',compband='%s',applname='%s',applsex='%s',appljob='%s',appldepart='%s',applphone='%s',applmobile='%s',applmail='%s',applqq='%s',clientnum=%s,datebeg=%u,days=%s,modtime=%u where sid=%s", utf8convert(compaddr_in), atoi(compsize_in), compphone_in, compfax_in, compweb_in, compmail_in, compband_in, utf8convert(applname_in), utf8convert(applsex_in), utf8convert(appljob_in), utf8convert(appldepart_in), applphone_in, applmobile_in, utf8convert(applmail_in), applqq_in, clientnums_in, datebeg, days_in, time(0), sid_in);
        printf("update :sql %s\n", sql);
        pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in, "select"))
    {
        if(!utStrIsSpaces(keywords_in))
        {
            char *keywords_gbk = (char*)utf8convert(keywords_in);
            sprintf(sqlTmp, " and (compname like '%%%s%%' or shortname like '%%%s%%')", keywords_gbk, keywords_gbk);
        }
    }
    else if(!strcmp(deal_in, "delete"))
    {
        if(strlen(caSelShortNames) > 0)
        {
            char *shortname_gbk = utf8convert(shortname_in);
            //1、得到nwgroup中gname in 简称的gid
            memset(sql, 0, sizeof(sql));
            sprintf(sql, "select gid from nwgroup where gname in (%s)", caSelShortNames);
            psCur = pasDbOpenSql(sql, 0);
            if(psCur != NULL)
            {
                unsigned long gid = 0;
                while(0 == (iret = pasDbFetchInto(psCur,
                                                  UT_TYPE_ULONG,  sizeof(gid), &gid)) || 1405 == iret)
                {
                    if(strlen(caGids) == 0)
                    {
                        snprintf(caGids, sizeof(caGids) - 1, "%lu", gid);
                    }
                    else
                    {
                        snprintf(caGids + strlen(caGids), sizeof(caGids) - 1, ",%lu", gid);
                    }
                }
                pasDbCloseCursor(psCur);
            }
            //2、删除nwgroup中gname in 简称的数据
            if(strlen(caGids) > 0)
            {
                memset(sql, 0, sizeof(sql));
                sprintf(sql, "delete from nwgroup where gname in (%s)", caSelShortNames);
                printf("delete :sql %s\n", sql);
                pasDbExecSqlF(sql);
                //3、得到dsuser中groupid in gids的ids
                memset(sql, 0, sizeof(sql));
                sprintf(sql, "select id from dsuser where groupid in (%s)", caGids);
                psCur = pasDbOpenSql(sql, 0);
                if(psCur != NULL)
                {
                    unsigned long lId = 0;
                    while(0 == (iret = pasDbFetchInto(psCur,
                                                      UT_TYPE_ULONG,  sizeof(lId), &lId)) || 1405 == iret)
                    {
                        if(strlen(caIds) == 0)
                        {
                            snprintf(caIds, sizeof(caIds) - 1, "%lu", lId);
                        }
                        else
                        {
                            snprintf(caIds + strlen(caIds), sizeof(caIds) - 1, ",%lu", lId);
                        }
                    }
                    pasDbCloseCursor(psCur);
                }
                //4、删除dsuser中groupid in gids的数据
                memset(sql, 0, sizeof(sql));
                sprintf(sql, "delete from dsuser where groupid in (%s)", caGids);
                printf("delete :sql %s\n", sql);
                pasDbExecSqlF(sql);
                //4.5、删除nwcompute中groupid in gids的数据
                memset(sql, 0, sizeof(sql));
                sprintf(sql, "delete from nwcompute where groupid in (%s)", caGids);
                printf("delete :sql %s\n", sql);
                pasDbExecSqlF(sql);
                //5、删除dsuserrole中id in ids的数据
                memset(sql, 0, sizeof(sql));
                sprintf(sql, "delete from dsuserrole where id in (%s)", caIds);
                printf("delete :sql %s\n", sql);
                pasDbExecSqlF(sql);
            }
            //6、删除userapply中shortname=简称的数据
            memset(sql, 0, sizeof(sql));
            sprintf(sql, "delete from userapply where shortname in (%s)", caSelShortNames);
            printf("delete :sql %s\n", sql);
            pasDbExecSqlF(sql);
        }

    }


    int iNum = 0;
    int count = 0;
    utPltDbHead *psDbHead = utPltInitDb();
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "select count(*) from userapply where 1=1 %s", sqlTmp);

    pasDbOneRecord(sql, 0, UT_TYPE_LONG, sizeof(count), &count);
    utPltPutVarF(psDbHead, "TotRec", "%d", count);
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "select sid,compname,compaddr,compsize,compphone,compfax,compweb,compmail,compband,applname,applsex,appljob,appldepart,applphone,applmobile,applmail,applqq,bindmail,shortname,clientnum,datebeg,days from userapply where 1=1 %s order by modtime desc limit %s,%s", sqlTmp, start_in, limit_in);

    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned long sid = 0;
        char compname[32] = "";
        char compaddr[256] = "";
        unsigned long compsize = 0;
        char compphone[16] = "";
        char compfax[16] = "";
        char compweb[128] = "";
        char compmail[128] = "";
        char compband[16] = "";
        char applname[32] = "";
        char applsex[8] = "";
        char appljob[32] = "";
        char appldepart[128] = "";
        char applphone[16] = "";
        char applmobile[16] = "";
        char applmail[128] = "";
        char applqq[16] = "";
        char bindmail[24] = "";
        char shortname[16] = "";
        unsigned int clientnums = 0;
        unsigned int datebegs = 0;
        unsigned int days = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  sizeof(sid),            &sid,
                                          UT_TYPE_STRING, sizeof(compname) - 1, compname,
                                          UT_TYPE_STRING, sizeof(compaddr) - 1, compaddr,
                                          UT_TYPE_ULONG,  sizeof(compsize),       &compsize,
                                          UT_TYPE_STRING, sizeof(compphone) - 1, compphone,
                                          UT_TYPE_STRING, sizeof(compfax) - 1,  compfax,
                                          UT_TYPE_STRING, sizeof(compweb) - 1,  compweb,
                                          UT_TYPE_STRING, sizeof(compmail) - 1, compmail,
                                          UT_TYPE_STRING, sizeof(compband) - 1, compband,
                                          UT_TYPE_STRING, sizeof(applname) - 1, applname,
                                          UT_TYPE_STRING, sizeof(applsex) - 1,  applsex,
                                          UT_TYPE_STRING, sizeof(appljob) - 1,  appljob,
                                          UT_TYPE_STRING, sizeof(appldepart) - 1, appldepart,
                                          UT_TYPE_STRING, sizeof(applphone) - 1, applphone,
                                          UT_TYPE_STRING, sizeof(applmobile) - 1, applmobile,
                                          UT_TYPE_STRING, sizeof(applmail) - 1, applmail,
                                          UT_TYPE_STRING, sizeof(applqq) - 1,       applqq,
                                          UT_TYPE_STRING, sizeof(bindmail) - 1, bindmail,
                                          UT_TYPE_STRING, sizeof(shortname) - 1, shortname,
                                          UT_TYPE_ULONG,  sizeof(clientnums), &clientnums,
                                          UT_TYPE_ULONG,  sizeof(datebegs),  &datebegs,
                                          UT_TYPE_ULONG,  sizeof(days),          &days)) || 1405 == iret)
        {

            iNum++;
            //向缓存里存数据
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }

            utPltPutLoopVarF(psDbHead, "sid", iNum, "%u", sid);
            utPltPutLoopVar(psDbHead, "compname", iNum, compname);
            utPltPutLoopVar(psDbHead, "compaddr", iNum, compaddr);
            utPltPutLoopVarF(psDbHead, "compsize", iNum, "%u", compsize);
            utPltPutLoopVar(psDbHead, "compphone", iNum, compphone);

            utPltPutLoopVar(psDbHead, "compfax", iNum, compfax);
            utPltPutLoopVar(psDbHead, "compweb", iNum, compweb);
            utPltPutLoopVar(psDbHead, "compmail", iNum, compmail);
            utPltPutLoopVar(psDbHead, "compband", iNum, compband);
            utPltPutLoopVar(psDbHead, "applname", iNum, applname);
            utPltPutLoopVar(psDbHead, "applsex", iNum, applsex);

            utPltPutLoopVar(psDbHead, "appljob", iNum, appljob);
            utPltPutLoopVar(psDbHead, "appldepart", iNum, appldepart);
            utPltPutLoopVar(psDbHead, "applphone", iNum, applphone);
            utPltPutLoopVar(psDbHead, "applmobile", iNum, applmobile);
            utPltPutLoopVar(psDbHead, "applmail", iNum, applmail);
            utPltPutLoopVar(psDbHead, "applqq", iNum, applqq);

            utPltPutLoopVar(psDbHead, "bindmail", iNum, bindmail);
            utPltPutLoopVar(psDbHead, "shortname", iNum, shortname);
            utPltPutLoopVarF(psDbHead, "days", iNum, "%d", days);
            utPltPutLoopVarF(psDbHead, "clientnums", iNum, "%u", clientnums);
            utPltPutLoopVar(psDbHead, "datebegs", iNum, utTimFormat("%Y/%m/%d", datebegs));
            utPltPutLoopVarF(psDbHead, "remaindays", iNum, "%d", time(0) > datebegs ? days - (time(0) - datebegs) / 3600 / 24 : days);

        }

        pasDbCloseCursor(psCur);
        utPltShowDb(psDbHead);
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/compuser.htm");
    }

    return 0;
}

int lanchgpsd(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    char shortname[32] = "";
    char username[32] = "";
    char password1[16] = "";
    char password2[16] = "";
    char caKey[32], caKey0[64];
    char caKey2[64], caKey22[64];

    utMsgGetSomeNVar(psMsgHead, 4,
                     "shortname",    UT_TYPE_STRING, sizeof(shortname),  shortname,
                     "username",     UT_TYPE_STRING, sizeof(username),       username,
                     "password1",    UT_TYPE_STRING, sizeof(password1),  password1,
                     "password2",  UT_TYPE_STRING, sizeof(password2),    password2);


    sprintf(caKey0, "%s%s", username, password1);
    utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey);

    char sql[512] = "";
    utPltDbHead *psDbHead = utPltInitDb();
    if(strcmp(utComGetVar_sd(psShmHead, "clouduser", "No"), "Yes") == 0)
    {
        sprintf(sql, "select count(*) from dsuser where name = '%s' and lkey = '%s' and groupid = (select groupid from dsuser where name='%sadmin')", username, caKey, shortname);
    }
    else
    {
        sprintf(sql, "select count(*) from dsuser where name = '%s' and lkey = '%s' ", username, caKey);
    }
    printf("修改密码: sql=%s\n", sql);
    unsigned int count = 0;
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(count), &count);

    if(count > 0)
    {
        sprintf(caKey0, "%s%s", username, password2);
        utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey);
        sprintf(sql, "update dsuser set lkey = '%s' where name = '%s'", caKey, username);
        pasDbExecSqlF(sql);
        utPltPutVar(psDbHead, "message", "修改完成");
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
    }
    else
    {
        utPltPutVar(psDbHead, "message", "密码不正确");
    }

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/chgpsdres.htm");
    return 0;
}

int checkcompname(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    char compname_in[32] = "";

    utMsgGetSomeNVar(psMsgHead, 1, "compname", UT_TYPE_STRING, sizeof(compname_in) - 1, compname_in);

    char sql[512] = "";
    int count = 0;

    sprintf(sql, "select count(*) from userapply where compname='%s'", utf8convert(compname_in));

    pasDbOneRecord(sql, 0, UT_TYPE_LONG, sizeof(count), &count);

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

int checkshortname(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char shortname_in[32] = "";

    utMsgGetSomeNVar(psMsgHead, 1, "shortname", UT_TYPE_STRING, sizeof(shortname_in) - 1, shortname_in);

    char sql[512] = "";
    int count = 0;

    sprintf(sql, "select count(*) from userapply where shortname='%s'", utf8convert(shortname_in));

    pasDbOneRecord(sql, 0, UT_TYPE_LONG, sizeof(count), &count);

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
