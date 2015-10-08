#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoall.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "nwdef.h"

void web_combine_table(char *table_name, char *sdate, char *edate)
{
    sdate[4] = '\0';
    edate[4] = '\0';

    sdate[7] = '\0';
    edate[7] = '\0';

    char sql[64] = "";

    int syear = atoi(sdate);
    int smonth = atoi(sdate + 5);
    int eyear = atoi(edate);
    int emonth = atoi(edate + 5);

    if(syear == eyear && smonth == emonth)
        snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), "nwweblog_%4u%02u as new_table", syear, smonth);
    else
    {
        int i, j = smonth, iNum = 0;
        strcat(table_name, "(");

        pasDbCursor *psCur = NULL;
        for(i = syear; i <= eyear && j <= emonth;)
        {
            snprintf(sql, sizeof(sql), "select * from nwweblog_%4u%02u", i, j);
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
    printf("table_name:%s\n", table_name);


}


int lan_log_web(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);

    char keyword_in[64] = "";
    char ip_in[16] = "";
    char compid_in[128] = "";
    char webaddr_in[128] = "";
    char comp_name_in[24] = "";
    char mac_in[24] = "";
    char udisp_in[32] = "";
    char group_in[8] = "";
    char sdate_in[24] = "";
    char edate_in[24] = "";
    char start_in[8] = "";
    char limit_in[8] = "";
    char caExport[8] = "";
    char sort_in[16] = "";
    char dir_in[16] = "";

    utMsgGetSomeNVar(psMsgHead, 15,
                     "keyword",     UT_TYPE_STRING,  sizeof(keyword_in) - 1,     keyword_in,
                     "sip",         UT_TYPE_STRING,  sizeof(ip_in) - 1,          ip_in,
                     "compid",     UT_TYPE_STRING,  sizeof(compid_in) - 1,       compid_in,
                     "groupid",     UT_TYPE_STRING,  sizeof(group_in) - 1,       group_in,
                     "compName",      UT_TYPE_STRING,  sizeof(comp_name_in) - 1,   comp_name_in,
                     "webaddr",     UT_TYPE_STRING,  sizeof(webaddr_in) - 1,     webaddr_in,
                     "mac",         UT_TYPE_STRING,  sizeof(mac_in) - 1,         mac_in,
                     "udisp",       UT_TYPE_STRING,  sizeof(udisp_in) - 1,         udisp_in,
                     "sdate",         UT_TYPE_STRING,  sizeof(sdate_in) - 1,       sdate_in,
                     "edate",         UT_TYPE_STRING,  sizeof(edate_in) - 1,       edate_in,
                     "start",       UT_TYPE_STRING,  sizeof(start_in) - 1,       start_in,
                     "limit",       UT_TYPE_STRING,  sizeof(limit_in) - 1,       limit_in,
                     "sort",        UT_TYPE_STRING,  sizeof(sort_in) - 1,        sort_in,
                     "dir",         UT_TYPE_STRING,  sizeof(dir_in) - 1,         dir_in,
                     "caExport",    UT_TYPE_STRING,  10,                       caExport
                    );

    char sql[2048] = "";
    char sql_tmp[2048] = "";
    pasDbCursor *psCur = NULL;
    int iret = 0;
    int iNum = 0;
    utPltDbHead *psDbHead = utPltInitDb();

    //取数据
    //增加where 条件的标识
   // printf("web before modi==%s,%s\n", sdate_in, edate_in);
    modifyDates(sdate_in, edate_in);
   // printf("web after modi==%s,%s\n", sdate_in, edate_in);

    int stime = utTimStrToLong("%Y/%m/%d %H:%M:%S", sdate_in);
    int etime = utTimStrToLong("%Y/%m/%d %H:%M:%S", edate_in);

    sdate_in[10] = '\0';
    edate_in[10] = '\0';

    snprintf(sql_tmp, sizeof(sql_tmp), "where new_table.stime>=%u and new_table.stime<=%u", stime, etime);

    //添加按电脑名查询条件
    if(!utStrIsSpaces(comp_name_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and (nwcompute.compname like '%%%s%%' or nwcompute.devname like '%%%s%%')", utf8convert(psShmHead, comp_name_in), utf8convert(psShmHead, comp_name_in));
    }
    if(!utStrIsSpaces(compid_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.compid = %s", compid_in);
    }
    //添加按用户名查询条件
    if(!utStrIsSpaces(udisp_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.udisp like '%%%s%%'", udisp_in);
    }

    //权限
    char *auth = getDsGroupid();
    if(!utStrIsSpaces(auth))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.groupid in (%s)", auth);
    }

    //添加按部门查询条件
    if(!utStrIsSpaces(group_in) && group_in[0] != '0')
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.groupid in (%s)", getGroupid(strtoul(group_in, NULL, 10)));
    }

    //添加按ip查询条件
    if(!utStrIsSpaces(ip_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and inet_ntoa(new_table.sip) like '%%%s%%'", ip_in);
    }

    //添加按设备名查询条件
    if(!utStrIsSpaces(webaddr_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.webaddr like '%%%s%%'", webaddr_in);
    }
    //添加按用户名查询条件
    if(!utStrIsSpaces(mac_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.mac like '%%%s%%'", mac_in);
    }

    char _sip[16] = "";
    if(!utStrIsSpaces(keyword_in))
    {
        char *keyword_gbk = utf8convert(psShmHead, keyword_in);

        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and (nwcompute.compname like '%%%s%%' or nwcompute.devname like '%%%s%%' or inet_ntoa(new_table.sip) like '%%%s%%' or new_table.devname like '%%%s%%' or new_table.udisp like '%%%s%%' or webaddr like '%%%s%%' or subject like '%%%s%%')", keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk);
        free(keyword_gbk);
    }
    iNum = 0;
    //取表总数
    unsigned int db_count = 0;
    char table_name[1024] = "";

    web_combine_table(table_name, sdate_in, edate_in);

    snprintf(sql, sizeof(sql), "select count(*) from %s left join nwcompute on nwcompute.compid=new_table.compid %s", table_name, sql_tmp);
    writeLog("logCheck.txt", "webC", sql);
    psCur = pasDbOpenSql(sql, 0);
    if(NULL != psCur)
    {
        pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &db_count);
        pasDbCloseCursor(psCur);
    }
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);

    memset(sql, 0, sizeof(sql));
    snprintf(sql, sizeof(sql), "select new_table.sid, new_table.compid, new_table.userid, new_table.mac, new_table.groupid, new_table.devname, new_table.udisp, inet_ntoa(nwcompute.ip), procname, new_table.stime, urlclass, webaddr, subject from %s left join nwcompute on nwcompute.compid=new_table.compid %s", table_name, sql_tmp);

    //导出到文件
    FILE *fp;
    char caFilename[32];
    char caFile[128];
    char caPath[128];

    if(strlen(caExport) > 0)
    {
        time_t time_tmp;
        struct tm *tm_tmp;
        time(&time_tmp);
        tm_tmp = gmtime(&time_tmp);

        sprintf(caFilename, "ncsweblog_%04u%02u%02u.csv", tm_tmp->tm_year + 1900, tm_tmp->tm_mon + 1, tm_tmp->tm_mday);
        //          sprintf(caFilename,"ncsuserlog%d.csv",utStrGetId());
        sprintf(caPath, "%s/html", "/home/ncmysql/nw");
        sprintf(caFile, "%s/%s", caPath, caFilename);
        fp = fopen(caFile, "w");
        if(fp == NULL)
        {
            pasDbCloseCursor(psCur);
            printf("open failed!\n");
            writeSysLog(psMsgHead, "06", "Fail", "创建外设日志文件失败");
            //        utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0546数据导出出错"),ncsLang("0546数据导出出错"));
            return 0;
        }

        ncUtlPrintCsv(fp, 1, UT_TYPE_STRING, ncLang("0090网页日志\n"));
        ncUtlPrintCsv(fp, 14, UT_TYPE_STRING, ncLang("0064序号"),
                      UT_TYPE_STRING, ncLang("0065计算机ID"),
                      UT_TYPE_STRING, ncLang("0067用户ID"),
                      UT_TYPE_STRING, ncLang("0068MAC地址"),
                      UT_TYPE_STRING, ncLang("0069部门"),
                      UT_TYPE_STRING, ncLang("0070设备名"),
                      UT_TYPE_STRING, ncLang("0071用户名"),
                      UT_TYPE_STRING, ncLang("0072IP地址"),
                      UT_TYPE_STRING, ncLang("0073进程名称"),
                      UT_TYPE_STRING, ncLang("0075启动时间"),
                      UT_TYPE_STRING, ncLang("0080路径"),
                      UT_TYPE_STRING, ncLang("0081网址"),
                      UT_TYPE_STRING, ncLang("0082主题"),
                      UT_TYPE_STRING, ncLang("0083内容"));
    }
    else
    {
        if(!utStrIsSpaces(sort_in))
        {
            snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " order by %s %s", sort_in, dir_in);
        }
        else
        {
            snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " order by stime desc");
        }
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " limit %d,%d", atoi(start_in), atoi(limit_in));
    }

    //查看sql语句

    writeLog("logCheck.txt", "web", sql);
    psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
    {
        //定义要取的数据类型
        unsigned long long sid = 0;
        unsigned int    compid = 0;
        unsigned int    userid = 0;
        char                    mac[20] = "";
        unsigned int    groupid = 0;
        char          _groupid[36] = "";
        char                    devname[32] = "";
        char                    udisp[32] = "";
        char          sip[24] = "";
        char                    procname[72] = "";
        unsigned int    stime = 0;
        unsigned int  urlclass = 0;
        char                    webaddr[128] = "";
        char          subject[128] = "";
        char                    webaddrT[128] = "";
        char          subjectT[128] = "";
        //char          content[128] = "";
        char                    s_time[24] = "";

        //取游标指向的数据
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_LONG8,  8,                     &sid,
                                          UT_TYPE_ULONG,  4,                         &compid,
                                          UT_TYPE_ULONG,  4,                         &userid,
                                          UT_TYPE_STRING, sizeof(mac) - 1,             mac,
                                          UT_TYPE_ULONG,  4,                         &groupid,
                                          UT_TYPE_STRING, sizeof(devname) - 1,     devname,
                                          UT_TYPE_STRING, sizeof(udisp) - 1,       udisp,
                                          UT_TYPE_STRING, sizeof(sip) - 1,         sip,
                                          UT_TYPE_STRING, sizeof(procname) - 1,    procname,
                                          UT_TYPE_ULONG,  4,                         &stime,
                                          UT_TYPE_LONG, 4,                   &urlclass,
                                          UT_TYPE_STRING, sizeof(webaddr) - 1,   webaddr,
                                          UT_TYPE_STRING, sizeof(subject) - 1,   subject)) || 1405 == iret)
        {
            iNum++;
            char _stime[15] = "";

            strcpy(_stime, utTimFormat("%m/%d %H:%M:%S", stime));

            nwCompInfo *comp_info = NULL;

            comp_info = (nwCompInfo*)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);

            //向缓存里存数据
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }

            strcpy(_groupid, (char *)lanGetGroup(psShmHead, groupid));
            strcpy(s_time, utTimFormat("%Y/%m/%d %H:%M:%S", stime));

            utPltPutLoopVarF(psDbHead, "sid", iNum, "%llu", sid);
            utPltPutLoopVarF(psDbHead, "compid", iNum, "%lu", compid);
            utPltPutLoopVarF(psDbHead, "userid", iNum, "%u", userid);
            utPltPutLoopVar(psDbHead, "mac", iNum, mac);
            utPltPutLoopVar(psDbHead, "groupid", iNum, _groupid);
            utPltPutLoopVar(psDbHead, "devname", iNum, devname);
            utPltPutLoopVar(psDbHead, "udisp", iNum, udisp);
            utPltPutLoopVarF(psDbHead, "sip", iNum, "%s", sip);
            utPltPutLoopVar(psDbHead, "procname", iNum, procname);
            utPltPutLoopVar(psDbHead, "stime", iNum, _stime);
            utPltPutLoopVarF(psDbHead, "urlclass", iNum, "%u", urlclass);
            //  utPltSetCvtHtml(1);
            AdjuestTextBuf(webaddr);
            //pasStrCvtUcode(webaddr,webaddrT);
            utPltPutLoopVar(psDbHead, "webaddr", iNum, webaddr);
            AdjuestTextBuf(subject);
            //pasStrCvtUcode(subject,subjectT);
            utPltPutLoopVar(psDbHead, "subject", iNum, subject);
            //  utPltSetCvtHtml(0);

            if(strlen(caExport) > 0)
            {
                ncUtlPrintCsv(fp, 14, UT_TYPE_LONG8, sid,
                              UT_TYPE_ULONG, compid,
                              UT_TYPE_ULONG, userid,
                              UT_TYPE_STRING, mac,
                              UT_TYPE_STRING, _groupid,
                              UT_TYPE_STRING, devname,
                              UT_TYPE_STRING, udisp,
                              UT_TYPE_STRING, sip,
                              UT_TYPE_STRING, procname,
                              UT_TYPE_STRING, s_time,
                              UT_TYPE_LONG, urlclass,
                              UT_TYPE_STRING, webaddr,
                              UT_TYPE_STRING, subject);
            }
        }

        pasDbCloseCursor(psCur);

    }

    if(strlen(caExport) > 0)
    {
        printf("download!\n");
        fclose(fp);
        utPltFileDownload(iFd, "application/text", caPath, caFilename, caFilename);
        remove(caFile);
        utPltFreeDb(psDbHead);
        writeSysLog(psMsgHead, "06", "Success", "导出网页日志");
        return 0;
    }
    char msg[256] = "";

    char *compname = findCompNameById(strtoul(compid_in, NULL, 10));
    if(!utStrIsSpaces(group_in))
    {
        snprintf(msg, sizeof(msg), "查询[%s]网页日志", lanGetGroup(psShmHead, atoi(group_in)));
    }
    else if(!utStrIsSpaces(compid_in))
    {
        snprintf(msg, sizeof(msg), "查询[%s]网页日志", compname);
    }
    else
    {
        snprintf(msg, sizeof(msg), "查询[全部]网页日志");
    }
    free(compname);
    //utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/neiwang/rzcx/ncs_web_list.htm");

    writeSysLog(psMsgHead, "06", "Success", msg);

    return 0;
}
