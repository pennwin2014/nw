#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoplt01.h"

//Just for test
#define SEACH_FLAG 1
#define DETAIL_FLAG 0

void chat_combine_table(char *table_name, char *sdate, char *edate)
{
	char caShortName[256]="";
	strcpy(caShortName, getLoginShortName());
    //writeLog("logCheck.txt","sdate",sdate);
    //writeLog("logCheck.txt","edate",edate);
    sdate[4] = '\0';
    edate[4] = '\0';

    sdate[7] = '\0';
    edate[7] = '\0';

    char sql[1024] = "";

    int syear = atoi(sdate);
    int smonth = atoi(sdate + 5);
    int eyear = atoi(edate);
    int emonth = atoi(edate + 5);
    char data[1024] = "";
    writeLog("logCheck.txt", "---------------27-------------", "-----");
    if(syear == eyear && smonth == emonth)
    {
        writeLog("logCheck.txt", "---------------29-------------", "-----");
        //snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), "ncimclient_%4u%02u as new_table", syear, smonth);
        snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), "%s as new_table", getNewLogTable(caShortName, "ncimclient", syear, smonth) );
    }
    else
    {
        snprintf(data, sizeof(data) - 1, "syear=%d	eyear=%d	smonth=%d	emonth=%d", syear, eyear, smonth, emonth);
        writeLog("logCheck.txt", "---------------33-------------", data);
        int i, j = smonth, iNum = 0;
        strcat(table_name, "(");

        pasDbCursor *psCur = NULL;
        //for(i=syear;i<=eyear&&j<=emonth;){
        for(i = syear; i <= eyear && (i < eyear ? j <= 12 : j <= emonth);)
        {
            //snprintf(sql, sizeof(sql), "select * from ncimclient_%4u%02u", i, j);
            snprintf(sql, sizeof(sql), "select * from %s", getNewLogTable(caShortName, "ncimclient", i, j));
            /////////////////////////////////////////////////////////////////////////////////////////////
            writeLog("logCheck.txt", "----------------------------", sql);
            psCur = pasDbOpenSql(sql, 0);
            if(psCur != NULL)
            {
                if(iNum > 0)
                {
                    strcat(table_name, " union all ");
                }
                iNum++;
                snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), sql);
                pasDbCloseCursor(psCur);
            }

            if(j > 11)
            {
                i++;
                j = 0;
            }
            j++;
        }
        snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), ")new_table");
    }

}

void chat_combine_table_and_condition(char *table_name, char *sdate, char *edate, char* condSql)
{
	char caShortName[256]="";
	strcpy(caShortName, getLoginShortName());
    //writeLog("logCheck.txt","sdate",sdate);
    //writeLog("logCheck.txt","edate",edate);
    sdate[4] = '\0';
    edate[4] = '\0';

    sdate[7] = '\0';
    edate[7] = '\0';

    char sql[1024] = "";

    int syear = atoi(sdate);
    int smonth = atoi(sdate + 5);
    int eyear = atoi(edate);
    int emonth = atoi(edate + 5);
    char data[1024] = "";
    writeLog("logCheck.txt", "---------------27-------------", "-----");
    if(syear == eyear && smonth == emonth)
    {
        writeLog("logCheck.txt", "---------------29-------------", "-----");
        snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), "%s %s",  getNewLogTable(caShortName, "ncimclient", syear, smonth), condSql);
    }
    else
    {
        snprintf(data, sizeof(data) - 1, "syear=%d	eyear=%d	smonth=%d	emonth=%d", syear, eyear, smonth, emonth);
        writeLog("logCheck.txt", "---------------33-------------", data);
        int i, j = smonth, iNum = 0;
        strcat(table_name, "(");

        pasDbCursor *psCur = NULL;
        //for(i=syear;i<=eyear&&j<=emonth;){
        for(i = syear; i <= eyear && (i < eyear ? j <= 12 : j <= emonth);)
        {
            //snprintf(sql, sizeof(sql), "select * from ncimclient_%4u%02u %s", i, j, condSql);
            snprintf(sql, sizeof(sql), "select * from %s %s", getNewLogTable(caShortName, "ncimclient", i, j), condSql);
            /////////////////////////////////////////////////////////////////////////////////////////////
            writeLog("logCheck.txt", "----------------------------", sql);
            psCur = pasDbOpenSql(sql, 0);
            if(psCur != NULL)
            {
                if(iNum > 0)
                {
                    strcat(table_name, " union all ");
                }
                iNum++;
                snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), sql);
                pasDbCloseCursor(psCur);
            }

            if(j > 11)
            {
                i++;
                j = 0;
            }
            j++;
        }
        snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), ")new_table");
    }

}

/*
*@名称：lan_chat_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
*@功能：根据面页传过来的部门ID和计算机ID参数进行查询数据库中聊天记录，再根据相应的模板传回去
*@参数：utShmHead *psShmHead 共享内存句柄
*               int iFd                          文件描述符
*               utMsgHead *psMsgHead 消息句柄
*@返回值：0成功，其他失败
*/
int lan_chat_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

#if SEACH_FLAG
    //显示传递的参数
    utMsgPrintMsg(psMsgHead);
#endif

    char page_page[3] = "";
    char page_start[4] = "";
    char page_limit[4] = "";
    char keywords_in[128] = "";
    char my_account_in[16] = "";
    char friend_account_in[16] = "";
    char ip_in[16] = "";
    char compid_in[16] = "";
    char groupid_in[16] = "";
    char style_in[8] = "";
    char sdate_in[24] = "";
    char edate_in[24] = "";
    char server_in[8] = "";

    char export_in[4] = "";

    utMsgGetSomeNVar(psMsgHead, 14,
                     "groupId",      UT_TYPE_STRING,  sizeof(groupid_in) - 1,      groupid_in,
                     "compId",           UT_TYPE_STRING,  sizeof(compid_in) - 1,       compid_in,
                     "keywords",     UT_TYPE_STRING,  sizeof(keywords_in) - 1,     keywords_in,
                     "styleId",      UT_TYPE_STRING,  sizeof(style_in) - 1,    style_in,
                     "ip",       UT_TYPE_STRING,  sizeof(ip_in) - 1,   ip_in,
                     "server",       UT_TYPE_STRING,  sizeof(server_in) - 1,   server_in,
                     "myAccount",        UT_TYPE_STRING,  sizeof(my_account_in) - 1,   my_account_in,
                     "friendAccount",        UT_TYPE_STRING,  sizeof(friend_account_in) - 1,   friend_account_in,
                     "sdate",            UT_TYPE_STRING,  sizeof(sdate_in) - 1,            sdate_in,
                     "edate",            UT_TYPE_STRING,  sizeof(edate_in) - 1,            edate_in,
                     "export",           UT_TYPE_STRING,  sizeof(export_in) - 1,       export_in,
                     "page",             UT_TYPE_STRING,  sizeof(page_page) - 1,       page_page,
                     "start",            UT_TYPE_STRING,  sizeof(page_start) - 1,      page_start,
                     "limit",            UT_TYPE_STRING,  sizeof(page_limit) - 1,      page_limit);

    if(!utStrIsSpaces(export_in))
    {
        strcpy(style_in, "ui");
    }

    char sql[1024] = "";
    char sql_tmp[1024] = "";
    char table_name[1024] = "";
    pasDbCursor *psCur = NULL;
    int iret = 0;
    int iNum = 0;
    utPltDbHead *psDbHead = utPltInitDb();
    //utTimStrToLong("%Y/%m/%d %H:%M:%S", sdate)


    if(utStrIsSpaces(sdate_in))
    {
        time_t tmp;
        time(&tmp);
        strcpy(sdate_in, utTimFormat("%Y/%m/%d %H:%M:%S", tmp));
        strcpy(edate_in, sdate_in);

        sdate_in[10] = '\0';
        edate_in[10] = '\0';

        strcat(sdate_in, " 00:00:00");
        strcat(edate_in, " 23:59:59");
    }

	//printf("before modi==%s,%s\n", sdate_in, edate_in);
	modifyDates(sdate_in, edate_in);
	//printf("after modi==%s,%s\n", sdate_in, edate_in);

    int stime = utTimStrToLong("%Y/%m/%d %H:%M:%S", sdate_in);
    int etime = utTimStrToLong("%Y/%m/%d %H:%M:%S", edate_in);

    

    snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " where stime>=%u and stime <=%u", stime, etime);

    if(!utStrIsSpaces(my_account_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and myaccount like '%%%s%%'", utf8convert(psShmHead, my_account_in));
    }
    if(!utStrIsSpaces(friend_account_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and friendaccount like '%%%s%%'", utf8convert(psShmHead, friend_account_in));
    }

    if(!utStrIsSpaces(ip_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and inet_ntoa(sip) like '%%%s%%'", ip_in);
    }

    if(!utStrIsSpaces(server_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and service = %s", server_in);
    }

    //权限
    char *auth = getDsGroupid();
    if(!utStrIsSpaces(auth))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and groupid in (%s)", auth);
    }
    //添加按部门查询条件
    if(!utStrIsSpaces(groupid_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and groupid in (%s)", getGroupid(strtoul(groupid_in, NULL, 10)));
    }

    if(!utStrIsSpaces(compid_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and compid = %s", compid_in);
    }

    if(!utStrIsSpaces(keywords_in))
    {
        char *keywords_gbk = convert("UTF-8", "GBK", keywords_in);
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and (inet_ntoa(sip) like '%%%s%%' or mfrom like '%%%s%%' or mto like '%%%s%%' or mesg like '%%%s%%')", keywords_gbk, keywords_gbk, keywords_gbk, keywords_gbk);
    }
	
	chat_combine_table_and_condition(table_name, sdate_in, edate_in, sql_tmp);
/*
    if(!utStrIsSpaces(style_in))
    {
        snprintf(sql, sizeof(sql), "select count(*) from %s %s", table_name, sql_tmp);
    }
    else
    {
        snprintf(sql, sizeof(sql), "select count(*) from (select * from %s %s group by myaccount,friendaccount)newtable2", table_name, sql_tmp);
    }
	*/
	if(!utStrIsSpaces(style_in))
    {
        snprintf(sql, sizeof(sql), "select count(*) from (select count(stime) from %s group by stime) aa", table_name);
    }
    else
    {
        snprintf(sql, sizeof(sql), "select count(*) from (select * from %s group by myaccount,friendaccount)newtable2", table_name);
    }
	printf("count sql=[%s]\n", sql);
    writeLog("logCheck.txt", "chatC", sql);
    int db_count = 0;
    psCur = pasDbOpenSql(sql, 0);

    if(NULL != psCur)
    {
        pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &db_count);
        pasDbCloseCursor(psCur);
    }
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);
/*
    if(!utStrIsSpaces(style_in))
    {
        snprintf(sql, sizeof(sql), "select inet_ntoa(sip),myaccount,friendaccount,service,stime,mfrom,mto,mesg from %s %s order by stime desc,sid desc ", table_name, sql_tmp);
    }
    else
    {
        snprintf(sql, sizeof(sql), "select inet_ntoa(sip),myaccount,friendaccount,service from %s %s group by myaccount,friendaccount", table_name, sql_tmp);
        //snprintf(sql,sizeof(sql),"select inet_ntoa(sip),myaccount,friendaccount,service from %s %s group by myaccount,friendaccount",table_name);
    }
*/
    if(!utStrIsSpaces(style_in))
    {
        snprintf(sql, sizeof(sql), "select inet_ntoa(sip),myaccount,friendaccount,service,stime,mfrom,mto,mesg from %s group by stime order by stime desc,sid desc ", table_name);
    }
    else
    {
        snprintf(sql, sizeof(sql), "select inet_ntoa(sip),myaccount,friendaccount,service from %s group by myaccount,friendaccount", table_name);
        //snprintf(sql,sizeof(sql),"select inet_ntoa(sip),myaccount,friendaccount,service from %s %s group by myaccount,friendaccount",table_name);
    }

    //导出到文件
    FILE *fp;
    char caFilename[128] = "";
    char caFile[128] = "";
    char caPath[128] = "";
    if(strlen(export_in) > 0)
    {
        time_t time_tm;
        time(&time_tm);
        struct tm *time_tmp = gmtime(&time_tm);

        sprintf(caFilename, "chatLog_%04u%02u%02u.csv", time_tmp->tm_year + 1900, time_tmp->tm_mon + 1, time_tmp->tm_mday);
        sprintf(caPath, "%s/html", "/home/ncmysql/nw");
        sprintf(caFile, "%s/%s", caPath, caFilename);
        fp = fopen(caFile, "w");
        if(fp == NULL)
        {
            pasDbCloseCursor(psCur);
            return 0;
        }
        //printf("111111111111111\n");
        ncUtlPrintCsv(fp, 1, UT_TYPE_STRING, ncLang("0105聊天日志"));
        ncUtlPrintCsv(fp, 5,
                      UT_TYPE_STRING, ncLang("0116工具"),
                      UT_TYPE_STRING, ncLang("0094时间"),
                      UT_TYPE_STRING, ncLang("0106发送"),
                      UT_TYPE_STRING, ncLang("0107接收"),
                      UT_TYPE_STRING, ncLang("0108内容"));
    }
    else
    {
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " limit %d,%d", atoi(page_start), atoi(page_limit));
    }

    writeLog("logCheck.txt", "chat", sql);
	printf("sql=%s\n", sql);
    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned int comp_id = 0;
        unsigned int group_id = 0;
        char group[32] = "";
        char sip[16] = "";
        char my_account[48] = "";
        char friend_account[48] = "";
        char my_accountT[48] = "";
        char friend_accountT[48] = "";
        char my_account16[256] = "";
        char friend_account16[256] = "";
        char mfrom[128] = "";
        char mto[128] = "";
        char mfromT[256] = "";
        char mtoT[256] = "";
        unsigned int stime = 0;
        char mesg[256] = "";
        char mesgT[256] = "";
        unsigned int service = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_STRING, sizeof(sip) - 1, sip,
                                          UT_TYPE_STRING, sizeof(my_account) - 1, my_account,
                                          UT_TYPE_STRING, sizeof(friend_account) - 1, friend_account,
                                          UT_TYPE_ULONG,  4,                  &service,
                                          UT_TYPE_ULONG,  4,                  &stime,
                                          UT_TYPE_STRING, sizeof(mfrom) - 1, mfrom,
                                          UT_TYPE_STRING, sizeof(mto) - 1, mto,
                                          UT_TYPE_STRING, sizeof(mesg) - 1, mesg)) || iret == 1405)
        {
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }

            utPltPutLoopVar(psDbHead, "sip", iNum, sip);

            pasStrCvtUcode(my_account, my_accountT);
            utPltPutLoopVar(psDbHead, "myAccount", iNum, my_accountT);
            pasStrCvtUcode(friend_account, friend_accountT);
            utPltPutLoopVar(psDbHead, "friendAccount", iNum, friend_accountT);

            pasStrCvtBin2Hex(my_account, strlen(my_account), my_account16);
            pasStrCvtBin2Hex(friend_account, strlen(friend_account), friend_account16);

            utPltPutLoopVar(psDbHead, "myAccount16", iNum, my_account16);
            utPltPutLoopVar(psDbHead, "friendAccount16", iNum, friend_account16);

            if(!utStrIsSpaces(style_in))
            {
                utPltPutLoopVar(psDbHead, "stime", iNum, utTimFormat("%Y/%m/%d %H:%M:%S", stime));

                pasStrCvtUcode(mfrom, mfromT);
                utPltPutLoopVar(psDbHead, "mfrom", iNum, mfromT);
                pasStrCvtUcode(mto, mtoT);
                utPltPutLoopVar(psDbHead, "mto", iNum, mtoT);
                switchtoUcode(mesg);
                pasStrCvtUcode(mesg, mesgT);
                utPltPutLoopVar(psDbHead, "mesg", iNum, mesgT);

            }

            utPltPutLoopVarF(psDbHead, "server", iNum, "%u", service);

            if(strlen(export_in) > 0)
            {
                char tmp[16] = "";
                char tmpT[16] = "";

                //printf("222222222222222222\n");
                switch(service)
                {
                    case 1001:
                        strcpy(tmp, "QQ");
                        break;
                    case 1002:
                        strcpy(tmp, ncLang("0102阿里旺旺"));
                        break;
                    case 1003:
                        strcpy(tmp, "SKYPE");
                        break;
                    case 1004:
                        strcpy(tmp, ncLang("0103飞信"));
                        break;
                    case 1005:
                        strcpy(tmp, ncLang("0104新浪UC"));
                        break;
                    case 1007:
                        strcpy(tmp, "LINE");
                        break;
                }
                //printf("222222222333333\n");
                ncUtlPrintCsv(fp, 5,
                              UT_TYPE_STRING, tmp,
                              UT_TYPE_STRING, utTimFormat("%Y/%m/%d %H:%M:%S", stime),
                              UT_TYPE_STRING, convertCSV(psShmHead, mfrom),
                              UT_TYPE_STRING, convertCSV(psShmHead, mto),
                              UT_TYPE_STRING, convertCSV(psShmHead, mesg));
                //printf("2222222244444\n");
            }
        }
        pasDbCloseCursor(psCur);

        if(strlen(export_in) > 0)
        {

            //printf("3333333333333333333\n");
            fclose(fp);
            utPltFileDownload(iFd, "application/text", caPath, caFilename, caFilename);
            remove(caFile);
            utPltFreeDb(psDbHead);
            writeSysLog(psMsgHead, "06", "Success", "导出聊天日志");
            return 0;
        }
    }
    char msg[256] = "";

    if(!utStrIsSpaces(groupid_in))
    {
        snprintf(msg, sizeof(msg), "查询[%s]聊天日志", lanGetGroup(psShmHead, atoi(groupid_in)));
    }
    else if(!utStrIsSpaces(compid_in))
    {
        char *compname = findCompNameById(strtoul(compid_in, NULL, 10));
        snprintf(msg, sizeof(msg), "查询[%s]聊天日志", compname);
        free(compname);
    }
    else
    {
        snprintf(msg, sizeof(msg), "查询[全部]聊天日志");
    }

    //utPltShowDb(psDbHead);

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_chat_list.htm");
    writeSysLog(psMsgHead, "06", "Success", msg);

    return 0;
}

/*
*@名称：lan_chat_detail_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
*@功能：根据面页传过来的本人账号和对方账号参数进行查询数据库中聊天记录的详细内容，再根据相应的模板传回去
*@参数：utShmHead *psShmHead 共享内存句柄
*               int iFd                          文件描述符
*               utMsgHead *psMsgHead 消息句柄
*@返回值：0成功，其他失败
*/
int lan_chat_detail_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    char my_account_in[512] = "";
    char friend_account_in[512] = "";
    char my_account[48] = "";
    char friend_account[48] = "";
    char page_page[3] = "";
    char page_start[4] = "";
    char page_limit[4] = "";
    char sdate_in[24] = "";
    char edate_in[24] = "";

    utMsgGetSomeNVar(psMsgHead, 7,
                     "myAccount",    UT_TYPE_STRING,  sizeof(my_account_in) - 1,   my_account_in,
                     "friendAccount",        UT_TYPE_STRING,  sizeof(friend_account_in) - 1,       friend_account_in,
                     "sdate",            UT_TYPE_STRING,  sizeof(sdate_in) - 1,        sdate_in,
                     "edate",            UT_TYPE_STRING,  sizeof(edate_in) - 1,        edate_in,
                     "page",             UT_TYPE_STRING,  sizeof(page_page) - 1,       page_page,
                     "start",            UT_TYPE_STRING,  sizeof(page_start) - 1,      page_start,
                     "limit",            UT_TYPE_STRING,  sizeof(page_limit) - 1,      page_limit);


    if(utStrIsSpaces(sdate_in))
    {
        time_t tmp;
        time(&tmp);
        strcpy(sdate_in, utTimFormat("%Y/%m/%d %H:%M:%S", tmp));
        strcpy(edate_in, sdate_in);

        sdate_in[10] = '\0';
        edate_in[10] = '\0';

        strcat(sdate_in, " 00:00:00");
        strcat(edate_in, " 23:59:59");
    }


    int stime = utTimStrToLong("%Y/%m/%d %H:%M:%S", sdate_in);
    int etime = utTimStrToLong("%Y/%m/%d %H:%M:%S", edate_in);

    char table_name[1024] = "";
    chat_combine_table(table_name, sdate_in, edate_in);

    utPltDbHead *psDbHead = utPltInitDb();

    if(!strlen(my_account_in) > 0 || !strlen(friend_account_in) > 0)
    {
        strcpy(my_account_in, "notexit");
        strcpy(friend_account_in, "notexit");
    }

    pasStrCvtHex2Bin(my_account_in, my_account);
    pasStrCvtHex2Bin(friend_account_in, friend_account);

    char sql[1024] = "";
    pasDbCursor *psCur = NULL;
    int iret = 0;

    snprintf(sql, sizeof(sql), "select count(*) from %s where myaccount = '%s' and friendaccount = '%s' and stime >= %u and stime <= %u order by stime,sid", table_name, my_account, friend_account, stime, etime);

    int db_count = 0;
    psCur = pasDbOpenSql(sql, 0);
    pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &db_count);
    pasDbCloseCursor(psCur);
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);

    snprintf(sql, sizeof(sql), "select stime,mesg,friendaccount,mfrom from %s where myaccount='%s' and friendaccount='%s' and stime >=%u and stime <=%u order by stime,sid limit %d,%d", table_name, my_account, friend_account, stime, etime, atoi(page_start), atoi(page_limit));

    psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
    {
        unsigned int stime = 0;
        char mfrom[48] = "";
        char mfromT[48] = "";
        char my_account[48] = "";
        char my_accountT[48] = "";
        char mesg[256] = "";
        char mesgT[256] = "";
        char color[16] = "";
        int iNum = 0;
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  4,                  &stime,
                                          UT_TYPE_STRING, sizeof(mesg) - 1, mesg,
                                          UT_TYPE_STRING, sizeof(my_account) - 1, my_account,
                                          UT_TYPE_STRING, sizeof(mfrom) - 1, mfrom)) || iret == 1405)
        {
            iNum++;
            if(strncmp(my_account, mfrom, strlen(my_account)))
            {
                strcpy(color, "#0000ff");
            }
            else
            {
                strcpy(color, "#ff0000");
            }
            //向缓存里存数据
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }

            utPltPutLoopVar(psDbHead, "time", iNum, utTimFormat("%Y/%m/%d %H:%M:%S", stime));
            utPltPutLoopVar(psDbHead, "color", iNum, color);
            pasStrCvtUcode(mfrom, mfromT);
            utPltPutLoopVar(psDbHead, "account", iNum, mfromT);
            switchtoUcode(mesg);
            pasStrCvtUcode(mesg, mesgT);
            utPltPutLoopVar(psDbHead, "mesg", iNum, mesgT);
        }
        pasDbCloseCursor(psCur);

#if DETAIL_FLAG
        utPltShowDb(psDbHead);
#endif

        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_chat_detail_list.htm");
    }


    return 0;
}
