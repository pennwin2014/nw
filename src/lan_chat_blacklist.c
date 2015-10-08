#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoplt01.h"

int lanChatSearchBlackProcy(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);

    char deal_in[8] = "";
    char id_in[16] = "";
    char blacktype_in[32] = "";
    char blackname_in[32] = "";
    char groupid_in[16] = "";
    char computeid_in[16] = "";
    char start_in[8] = "";
    char limit_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 8,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "id", UT_TYPE_STRING,  sizeof(id_in) - 1,  id_in,
                     "blacktype",    UT_TYPE_STRING,  sizeof(blacktype_in) - 1,    blacktype_in,
                     "blackname",    UT_TYPE_STRING,  sizeof(blackname_in) - 1,    blackname_in,
                     "groupid",  UT_TYPE_STRING,  sizeof(groupid_in) - 1,  groupid_in,
                     "computeid",    UT_TYPE_STRING,  sizeof(computeid_in) - 1,    computeid_in,
                     "start",    UT_TYPE_STRING,  sizeof(start_in) - 1,    start_in,
                     "limit",    UT_TYPE_STRING,  sizeof(limit_in) - 1,    limit_in);

    char sql[512] = "";
    char sql_tmp[128] = "";
    unsigned int count = 0;
    utPltDbHead *psDbHead = utPltInitDb();
    int iret = -1;
    int userlevel = 0;

    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);

    findShortName(auth, shortname);
    userlevel = findUserLevel(auth);

    printf("用户级别:%d\n", userlevel);

    if(!strcmp(deal_in, "update"))
    {
        if(!utStrIsSpaces(computeid_in))
        {
            unsigned long groupid = 0;
            char sql[128] = "";
            sprintf(sql, "select groupid from nwcompute where compid=%s", computeid_in);
            pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(groupid), &groupid);
            sprintf(groupid_in, "%lu", groupid);
        }
        sprintf(sql, "insert blacklistprocy(blacktype,blackname,groupid,computeid,userid,userlevel,updatetime,flag)values('%s','%s',%lu,%lu,'%s',%d,%lu,0)", utf8convert(psShmHead, blacktype_in), utf8convert(psShmHead, blackname_in), strtoul(groupid_in, NULL, 10), strtoul(computeid_in, NULL, 10), shortname, userlevel, time(0));
        printf("insert %s\n", sql);
        iret = pasDbExecSqlF(sql);
        if(iret != 0)
        {
            sprintf(sql, "update blacklistprocy set blackname = '%s',updatetime=%lu,flag=0 where blacktype = '%s' and groupid=%lu and computeid=%lu ", utf8convert(psShmHead, blackname_in), time(0), utf8convert(psShmHead, blacktype_in), strtoul(groupid_in, NULL, 10), strtoul(computeid_in, NULL, 10));
            printf("黑白策略: %s\n", sql);
            pasDbExecSqlF(sql);
        }
    }
    else if(!strcmp(deal_in, "delete"))
    {
        memset(sql, 0, sizeof(memset));
        sprintf(sql, "update blacklistprocy set flag=1,updatetime=%lu where recno = %s", time(0), id_in);
        pasDbExecSqlF(sql);
    }

    if(!utStrIsSpaces(computeid_in))
    {
        sprintf(sql_tmp + strlen(sql_tmp), " and computeid=%s", computeid_in);
    }
    else   if(!utStrIsSpaces(groupid_in))
    {
        sprintf(sql_tmp, " and blacklistprocy.groupid in(%s)", getGroupid(strtoul(groupid_in, NULL, 10)));
    }

    char *authgroup = getDsGroupid();
    if(!utStrIsSpaces(authgroup))
    {
        sprintf(sql_tmp + strlen(sql_tmp), " and blacklistprocy.groupid in (%s)", authgroup);
    }

    sprintf(sql, "select count(*) from blacklistprocy where userid='%s' and flag=0 %s", shortname, sql_tmp);

    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count);
    utPltPutVarF(psDbHead, "TotRec", "%u", count);

    sprintf(sql, "select blacklistprocy.groupid,blacklistprocy.computeid,blacklistprocy.recno,blacktype,blackname,gname,inet_ntoa(ip),userlevel,keyname from blacklistprocy left join nwgroup on blacklistprocy.groupid=gid left join nwcompute on compid=computeid left join paramsmanage on userlevel=keyvalue and paramtype=1 and company='%s' where blacklistprocy.userid='%s' and blacklistprocy.flag=0 %s order by updatetime desc", shortname, shortname, sql_tmp);


    pasDbCursor *psCur = NULL;
    writeLog("blackList.txt", "search", sql);
    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned int recno = 0;
        unsigned long groupid = 0;
        unsigned long computeid = 0;
        char blacktype[32] = "";
        char blackname[32] = "";
        char gname[32] = "";
        char ip[16] = "";
        int userlevel1 = 0;
        char levelvalue[32] = "";

        int iret = 0, iNum = 0;
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG, sizeof(groupid),    &groupid,
                                          UT_TYPE_ULONG, sizeof(computeid),  &computeid,
                                          UT_TYPE_ULONG, sizeof(recno),  &recno,
                                          UT_TYPE_STRING, sizeof(blacktype) - 1,  blacktype,
                                          UT_TYPE_STRING, sizeof(blackname) - 1, blackname,
                                          UT_TYPE_STRING, sizeof(gname) - 1,   gname,
                                          UT_TYPE_STRING, sizeof(ip) - 1,   ip,
                                          UT_TYPE_LONG, sizeof(userlevel1),  &userlevel1,
                                          UT_TYPE_STRING, sizeof(levelvalue) - 1,  levelvalue))  || 1405 == iret)
        {

            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            utPltPutLoopVarF(psDbHead, "groupId", iNum, "%u", groupid);
            utPltPutLoopVarF(psDbHead, "computeId", iNum, "%u", computeid);
            utPltPutLoopVarF(psDbHead, "id", iNum, "%u", recno);
            utPltPutLoopVar(psDbHead, "blacktype", iNum, blacktype);
            utPltPutLoopVar(psDbHead, "blackname", iNum, blackname);
            utPltPutLoopVar(psDbHead, "groupName", iNum, gname);
            utPltPutLoopVar(psDbHead, "IP", iNum, ip);
            utPltPutLoopVarF(psDbHead, "levelremain", iNum, "%d", userlevel1 - userlevel);
            utPltPutLoopVar(psDbHead, "levelvalue", iNum, levelvalue);

            memset(ip, 0, sizeof(ip));
        }

        pasDbCloseCursor(psCur);
    }
    utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_blacklist.htm");

    return 0;
}

int lanChatDealBlackList(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);

    char deal_in[8] = "";
    char recno_in[16] = "";
    char blacktype_in[32] = "";
    char blackname_in[32] = "";
    char blackvalue_in[32] = "";
    char blackstatus_in[2] = "";

    utMsgGetSomeNVar(psMsgHead, 6,
                     "deal", UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "recno", UT_TYPE_STRING,  sizeof(recno_in) - 1,    recno_in,
                     "blacktype",    UT_TYPE_STRING,  sizeof(blacktype_in) - 1,    blacktype_in,
                     "blackname",    UT_TYPE_STRING,  sizeof(blackname_in) - 1,    blackname_in,
                     "blackvalue",   UT_TYPE_STRING,  sizeof(blackvalue_in) - 1,   blackvalue_in,
                     "blackstatus",  UT_TYPE_STRING,  sizeof(blackstatus_in) - 1,  blackstatus_in);

    int iret = -1;
    char sql[512] = "";
    utPltDbHead *psDbHead = utPltInitDb();
    int count = -1;

    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);
	printf("chat auth=[%s]\n", auth);
    findShortName(auth, shortname);

    //判断黑白名单是否有重复
    sprintf(sql, "select count(*) from blacklist where blacktype='%s' and blackname='%s' and blackvalue='%s' and flag=1", blacktype_in, utf8convert(psShmHead, blackname_in), blackvalue_in);
    pasDbOneRecord(sql, 0, UT_TYPE_LONG, sizeof(count), &count);

    if(count > 0)
    {
        sprintf(sql, "delete from blacklist where blacktype='%s' and blackname='%s' and blackvalue='%s' and flag=1", blacktype_in, utf8convert(psShmHead, blackname_in), blackvalue_in);
        pasDbExecSqlF(sql);
    }
    if(!strcmp(deal_in, "insert"))
    {
        sprintf(sql, "insert blacklist(blacktype,blackname,blackvalue,blackstatus,updatetime,userid)values('%s','%s','%s',%d,%u,'%s')", utf8convert(psShmHead, blacktype_in), utf8convert(psShmHead, blackname_in), utf8convert(psShmHead, blackvalue_in), atoi(blackstatus_in), time(0), shortname);

        printf("黑白名单：sql:%s\n", sql);
        iret = pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in, "update"))
    {
        sprintf(sql, "update blacklist set blackvalue='%s',updatetime=%lu,flag=0 where recno = %s", utf8convert(psShmHead, blackvalue_in), time(0), recno_in);

        printf("黑白名单：sql:%s\n", sql);
        iret = pasDbExecSqlF(sql);
    }
    else if(!strcmp(deal_in, "delete"))
    {
        sprintf(sql, "update blacklist set flag=1,updatetime=%lu where recno = %s", time(0), recno_in);

        printf("黑白名单：sql:%s\n", sql);
        iret = pasDbExecSqlF(sql);
    }

    if(iret == 0)
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

int lanChatSearchNameList(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    char blacktype_in[32] = "";

    utMsgGetSomeNVar(psMsgHead, 1,
                     "blacktype",    UT_TYPE_STRING,  sizeof(blacktype_in) - 1,    blacktype_in);

    char sql[512] = "";
    utPltDbHead *psDbHead = utPltInitDb();

    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);

    findShortName(auth, shortname);

    sprintf(sql, "select distinct blackname from blacklist where blacktype='%s' and userid='%s' and flag=0", utf8convert(psShmHead, blacktype_in), shortname);

    pasDbCursor *psCur = NULL;
    printf("黑白名单：sql:%s\n", sql);
    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned int recno = 0;
        char blacktype[32] = "";
        char blackname[32] = "";
        char gname[32] = "";
        char ip[16] = "";

        int iret = 0, iNum = 0;
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_STRING, sizeof(blackname) - 1, blackname))  || 1405 == iret)
        {

            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            utPltPutLoopVarF(psDbHead, "id", iNum, "%u", iNum);
            utPltPutLoopVar(psDbHead, "blackname", iNum, blackname);
        }
    }
    utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_namelist.htm");


    return 0;
}

int lanChatSearchBlackList(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);
    char deal_in[32] = "";
    char blacktype_in[32] = "";
    char blackname_in[32] = "";
    char blackstatus_in[32] = "";

    utMsgGetSomeNVar(psMsgHead, 4,
                     "deal",             UT_TYPE_STRING,  sizeof(deal_in) - 1,     deal_in,
                     "blacktype",    UT_TYPE_STRING,  sizeof(blacktype_in) - 1,    blacktype_in,
                     "blackname",    UT_TYPE_STRING,  sizeof(blackname_in) - 1,    blackname_in,
                     "blackstatus",  UT_TYPE_STRING,  sizeof(blackstatus_in) - 1,  blackstatus_in);

    char sql[512] = "";
    utPltDbHead *psDbHead = utPltInitDb();
    pasDbCursor *psCur = NULL;

    sprintf(sql, "select recno,blackvalue from blacklist where blacktype='%s' and blackname='%s' and blackstatus= %s and flag=0", utf8convert(psShmHead, blacktype_in), utf8convert(psShmHead, blackname_in), blackstatus_in);

    printf("黑白名单：sql:%s\n", sql);
    psCur = pasDbOpenSql(sql, 0);

    FILE *fp;
    char caFilename[32];
    char caFile[128];
    char caPath[128];

    if(!strcmp(deal_in, "out"))
    {
        sprintf(caFilename, "nw%s.csv", blackname_in);
        strcpy(caPath, "/home/ncmysql/nw/html");
        sprintf(caFile, "%s/%s", caPath, caFilename);
        fp = fopen(caFile, "w");
        if(fp == NULL)
        {
            pasDbCloseCursor(psCur);
            printf("open failed!\n");
            return 0;
        }

        fprintf(fp, "%s%s名单\n", blackname_in, atoi(blackstatus_in) == 1 ? "白" : "黑");
        ncUtlPrintCsv(fp, 1, UT_TYPE_STRING, "网址");
    }


    if(psCur != NULL)
    {
        unsigned int recno = 0;
        char blackvalue[128] = "";

        int iret = 0, iNum = 0;
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG, sizeof(recno),  &recno,
                                          UT_TYPE_STRING, sizeof(blackvalue) - 1,  blackvalue)) || 1405 == iret)
        {

            if(!strcmp(deal_in, "out"))
            {
                ncUtlPrintCsv(fp, 1, UT_TYPE_STRING, blackvalue);
            }
            else
            {
                iNum++;
                if(iNum > 1)
                {
                    utPltPutLoopVar(psDbHead, "dh", iNum, ",");
                }
                utPltPutLoopVarF(psDbHead, "id", iNum, "%u", recno);
                utPltPutLoopVar(psDbHead, "blackvalue", iNum, blackvalue);
            }
        }
        pasDbCloseCursor(psCur);
    }

    if(!strcmp(deal_in, "out"))
    {
        fclose(fp);
        utPltFileDownload(iFd, "application/text", caPath, caFilename, caFilename);
        remove(caFile);
        utPltFreeDb(psDbHead);
        return 0;
    }

    utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_blackvaluelist.htm");
    return 0;
}

