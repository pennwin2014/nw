#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "nwdef.h"

void waishe_combine_table(char *table_name, char *sdate, char *edate)
{
    char caShortName[256] = "";
    strcpy(caShortName, getLoginShortName());

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
    {
        //snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), "nwremovelog_%4u%02u as new_table", syear, smonth);
        snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), "%s as new_table", getNewLogTable(caShortName, "nwremovelog", syear, smonth));

    }
    else
    {
        int i, j = smonth, iNum = 0;
        strcat(table_name, "(");

        pasDbCursor *psCur = NULL;
        for(i = syear; i <= eyear && j <= emonth;)
        {
            //snprintf(sql, sizeof(sql), "select * from nwremovelog_%4u%02u", i, j);
            snprintf(sql, sizeof(sql), "select * from %s", getNewLogTable(caShortName, "nwremovelog", i, j));

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

int lan_log_peri(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);

    char depart_in[8] = "";
    char compid_in[128] = "";
    char ip_in[16] = "";
    char sdate_in[24] = "";
    char edate_in[24] = "";
    char produ_name_in[24] = "";
    char comp_name_in[24] = "";
    char dev_name_in[24] = "";
    char mark_in[2] = "";
    char page_start[8] = "";
    char page_limit[8] = "";
    char keyword_in[64] = "";
    char caExport[8] = "";
    char sort_in[16] = "";
    char dir_in[16] = "";

    utMsgGetSomeNVar(psMsgHead, 15,
                     "depart",       UT_TYPE_STRING,  sizeof(depart_in) - 1,       depart_in,
                     "compId",       UT_TYPE_STRING,  sizeof(compid_in) - 1,       compid_in,
                     "ip",               UT_TYPE_STRING,  sizeof(ip_in) - 1,               ip_in,
                     "sdate",            UT_TYPE_STRING,  sizeof(sdate_in) - 1,            sdate_in,
                     "bdate",            UT_TYPE_STRING,  sizeof(edate_in) - 1,            edate_in,
                     "compName", UT_TYPE_STRING,  sizeof(comp_name_in) - 1,    comp_name_in,
                     "produName",    UT_TYPE_STRING,  sizeof(produ_name_in) - 1,   produ_name_in,
                     "devName",  UT_TYPE_STRING,  sizeof(dev_name_in) - 1,     dev_name_in,
                     "mark", UT_TYPE_STRING,  sizeof(mark_in) - 1,     mark_in,
                     "keyword",      UT_TYPE_STRING,  sizeof(keyword_in) - 1,      keyword_in,
                     "caExport", UT_TYPE_STRING,  sizeof(caExport) - 1,            caExport,
                     "start",            UT_TYPE_STRING,  sizeof(page_start) - 1,      page_start,
                     "limit",            UT_TYPE_STRING,  sizeof(page_limit) - 1,      page_limit,
                     "sort",         UT_TYPE_STRING,  sizeof(sort_in) - 1,         sort_in,
                     "dir",          UT_TYPE_STRING,  sizeof(dir_in) - 1,      dir_in);

    char sql[2048] = "";
    char sql_tmp[2048] = "";
    pasDbCursor *psCur = NULL;
    int iret = 0;
    int iNum = 0;
    utPltDbHead *psDbHead = utPltInitDb();

    //取数据
    //增加where 条件的标识

    // printf("peri before modi==%s,%s\n", sdate_in, edate_in);
    modifyDates(sdate_in, edate_in);
    // printf("peri after modi==%s,%s\n", sdate_in, edate_in);

    int stime = utTimStrToLong("%Y/%m/%d %H:%M:%S", sdate_in);
    int etime = utTimStrToLong("%Y/%m/%d %H:%M:%S", edate_in);

    sdate_in[10] = '\0';
    edate_in[10] = '\0';

    snprintf(sql_tmp, sizeof(sql_tmp), "where new_table.stime>=%u and new_table.stime<=%u", stime, etime);

    //添加按电脑名查询条件
    if(!utStrIsSpaces(comp_name_in))
    {
        sprintf(sql_tmp + strlen(sql_tmp), " and (nwcompute.devname like '%%%s%%' or nwcompute.compname like '%%%s%%')", utf8convert(psShmHead, comp_name_in), utf8convert(psShmHead, comp_name_in));
    }
    if(!utStrIsSpaces(compid_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.compid = %s", compid_in);
    }

    //权限
    char *auth = getDsGroupid();
    if(!utStrIsSpaces(auth))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.groupid in (%s)", auth);
    }

    //添加按部门查询条件
    if(!utStrIsSpaces(depart_in) && depart_in[0] != '0')
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.groupid in (%s)", getGroupid(strtoul(depart_in, NULL, 10)));
    }

    //添加按ip查询条件
    if(!utStrIsSpaces(ip_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and inet_ntoa(new_table.sip) like '%%%s%%'", ip_in);
    }

    //添加按产品名查询条件
    if(!utStrIsSpaces(produ_name_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and produname = '%s'", produ_name_in);
    }

    //添加按设备名查询条件
    if(!utStrIsSpaces(dev_name_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.devname like '%%%s%%'", dev_name_in);
    }

    //添加按进程名查询条件
    if(!utStrIsSpaces(mark_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and mark = '%s'", mark_in);
    }
    else
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and mark in (4,5)");
    }

    char _sip[16] = "";
    if(!utStrIsSpaces(keyword_in))
    {
        char *keyword_gbk = utf8convert(psShmHead, keyword_in);

        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and (nwcompute.compname like '%%%s%%' or nwcompute.devname like '%%%s%%' or produname like '%%%s%%' or inet_ntoa(new_table.sip) like '%%%s%%' or new_table.devname like '%%%s%%' or hardname like '%%%s%%')", keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk);
        free(keyword_gbk);
    }
    iNum = 0;
    //取表总数
    unsigned int db_count = 0;
    char table_name[1024] = "";

    waishe_combine_table(table_name, sdate_in, edate_in);

    snprintf(sql, sizeof(sql), "select count(*) from %s left join nwcompute on nwcompute.compid=new_table.compid %s", table_name, sql_tmp);

    writeLog("logCheck.txt", "periC", sql);
    psCur = pasDbOpenSql(sql, 0);
    if(NULL != psCur)
    {
        pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &db_count);
        pasDbCloseCursor(psCur);
    }
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);


    memset(sql, 0, sizeof(sql));
    snprintf(sql, sizeof(sql), "select new_table.sid, new_table.compid,nwcompute.compname, new_table.userid, new_table.mac, new_table.groupid, new_table.devname, udisp, inet_ntoa(new_table.sip), procname, mark, new_table.stime, valuename, produname, hardname, driversn from %s left join nwcompute on nwcompute.compid=new_table.compid %s", table_name, sql_tmp);

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

        sprintf(caFilename, "ncsFileLog_%04u%02u%02u.csv", tm_tmp->tm_year + 1900, tm_tmp->tm_mon + 1, tm_tmp->tm_mday);
        //          sprintf(caFilename,"ncsuserlog%d.csv",utStrGetId());
        sprintf(caPath, "%s/html", "/home/ncmysql/nw");
        sprintf(caFile, "%s/%s", caPath, caFilename);
        fp = fopen(caFile, "w");
        if(fp == NULL)
        {
            pasDbCloseCursor(psCur);
            writeSysLog(psMsgHead, "06", "Fail", "创建外设日志文件失败");
            //        utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0546数据导出出错"),ncsLang("0546数据导出出错"));
            return 0;
        }

        ncUtlPrintCsv(fp, 1, UT_TYPE_STRING, ncLang("0089外设日志"));
        ncUtlPrintCsv(fp, 16, UT_TYPE_STRING, ncLang("0064序号"),
                      UT_TYPE_STRING, ncLang("0065计算机ID"),
                      UT_TYPE_STRING, ncLang("0066计算机名称"),
                      UT_TYPE_STRING, ncLang("0067用户ID"),
                      UT_TYPE_STRING, ncLang("0068MAC地址"),
                      UT_TYPE_STRING, ncLang("0069部门ID"),
                      UT_TYPE_STRING, ncLang("0070设备名"),
                      UT_TYPE_STRING, ncLang("0071用户名"),
                      UT_TYPE_STRING, ncLang("0072IP地址"),
                      UT_TYPE_STRING, ncLang("0073进程名称"),
                      UT_TYPE_STRING, ncLang("0074动作"),
                      UT_TYPE_STRING, ncLang("0075启动时间"),
                      UT_TYPE_STRING, ncLang("0076卷名称"),
                      UT_TYPE_STRING, ncLang("0077产品名称"),
                      UT_TYPE_STRING, ncLang("0078硬件名称"),
                      UT_TYPE_STRING, ncLang("0079分配的盘符"));
    }
    else
    {
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " order by new_table.stime desc limit %d,%d", atoi(page_start), atoi(page_limit));
    }

    //查看sql语句
    writeLog("logCheck.txt", "peri", sql);
    char devname[32] = "";
    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        //定义要取的数据类型
        long long sid = 0;
        unsigned int    compid = 0;
        unsigned int    userid = 0;
        char                    compname[32] = "";
        char                    mac[20] = "";
        unsigned int    groupid = 0;
        char          _groupid[6] = "";
        char                    udisp[32] = "";
        char            sip[24] = "";
        char                    procname[72] = "";
        char                    mark[2] = "";
        char          _mark[2] = "";
        unsigned int    stime = 0;
        char          valuename[72] = "";
        char                    produname[72] = "";
        char          hardname[128] = "";
        char          driversn[2] = "";
        char s_time[21] = "";

        //取游标指向的数据
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_LONG8,  sizeof(sid),           &sid,
                                          UT_TYPE_ULONG,  sizeof(compid),            &compid,
                                          UT_TYPE_STRING, sizeof(compname) - 1,    compname,
                                          UT_TYPE_ULONG,  sizeof(userid),      &userid,
                                          UT_TYPE_STRING, sizeof(mac) - 1,             mac,
                                          UT_TYPE_ULONG,  sizeof(groupid),     &groupid,
                                          UT_TYPE_STRING, sizeof(devname) - 1,     devname,
                                          UT_TYPE_STRING, sizeof(udisp) - 1,       udisp,
                                          UT_TYPE_STRING, sizeof(sip) - 1,             sip,
                                          UT_TYPE_STRING, sizeof(procname) - 1,    procname,
                                          UT_TYPE_STRING, sizeof(mark) - 1,            mark,
                                          UT_TYPE_ULONG,  sizeof(stime),       &stime,
                                          UT_TYPE_STRING, sizeof(valuename) - 1, valuename,
                                          UT_TYPE_STRING, sizeof(produname) - 1, produname,
                                          UT_TYPE_STRING, sizeof(hardname) - 1,  hardname,
                                          UT_TYPE_STRING, sizeof(driversn) - 1,  driversn)) || 1405 == iret)
        {
            iNum++;
            char _stime[15] = "";

            strcpy(_stime, utTimFormat("%H:%M:%S %Y/%m/%d", stime));

            strcpy(s_time, utTimFormat("%Y/%m/%d %H:%M:%S", stime));
            if(strlen(caExport) > 0)
            {
                ncUtlPrintCsv(fp, 16, UT_TYPE_LONG8, sid,
                              UT_TYPE_ULONG, compid,
                              UT_TYPE_STRING, compname,
                              UT_TYPE_ULONG, userid,
                              UT_TYPE_STRING, mac,
                              UT_TYPE_STRING, _groupid,
                              UT_TYPE_STRING, devname,
                              UT_TYPE_STRING, udisp,
                              UT_TYPE_STRING, sip,
                              UT_TYPE_STRING, procname,
                              UT_TYPE_STRING, _mark,
                              UT_TYPE_STRING, s_time,
                              UT_TYPE_STRING, valuename,
                              UT_TYPE_STRING, produname,
                              UT_TYPE_STRING, hardname,
                              UT_TYPE_STRING, driversn);
            }
            else
            {
                //向缓存里存数据
                if(iNum > 1)
                {
                    utPltPutLoopVar(psDbHead, "dh", iNum, ",");
                }


                utPltPutLoopVarF(psDbHead, "sid", iNum, "%llu", sid);
                utPltPutLoopVarF(psDbHead, "compid", iNum, "%u", compid);
                utPltPutLoopVarF(psDbHead, "userid", iNum, "%u", userid);
                utPltPutLoopVar(psDbHead, "compname", iNum, compname);
                utPltPutLoopVar(psDbHead, "mac", iNum, mac);
                utPltPutLoopVar(psDbHead, "groupid", iNum, lanGetGroup(psShmHead, groupid));
                utPltPutLoopVar(psDbHead, "devname", iNum, devname);
                utPltPutLoopVar(psDbHead, "udisp", iNum, udisp);
                utPltPutLoopVarF(psDbHead, "sip", iNum, "%s", sip);
                utPltPutLoopVar(psDbHead, "procname", iNum, procname);
                utPltPutLoopVar(psDbHead, "mark", iNum, mark);
                utPltPutLoopVar(psDbHead, "stime", iNum, _stime);
                utPltPutLoopVar(psDbHead, "valuename", iNum, valuename);
                utPltPutLoopVar(psDbHead, "produname", iNum, produname);
                utPltPutLoopVar(psDbHead, "hardname", iNum, hardname);
                utPltPutLoopVar(psDbHead, "driversn", iNum, driversn);
            }
        }

        pasDbCloseCursor(psCur);

    }

    if(strlen(caExport) > 0)
    {
        fclose(fp);
        utPltFileDownload(iFd, "application/text", caPath, caFilename, caFilename);
        remove(caFile);
        utPltFreeDb(psDbHead);

        writeSysLog(psMsgHead, "06", "Success", "导出外设日志");
        return 0;
    }
    char msg[256] = "";

    if(!utStrIsSpaces(depart_in))
    {
        snprintf(msg, sizeof(msg), "查询[%s]外设日志", lanGetGroup(psShmHead, atoi(depart_in)));
    }
    else if(!utStrIsSpaces(compid_in))
    {
        char *compname = findCompNameById(strtoul(compid_in, NULL, 10));
        snprintf(msg, sizeof(msg), "查询[%s]外设日志", compname);
        free(compname);
    }
    else
    {
        snprintf(msg, sizeof(msg), "查询[全部]外设日志");
    }
    //utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_peri_list.htm");

    writeSysLog(psMsgHead, "06", "Success", msg);
    return 0;
}
