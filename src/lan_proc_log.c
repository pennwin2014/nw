#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "nwdef.h"

/*
*@���ƣ�void combine_table(char *table_name,struct tm *stime,struct tm *btime)
*@���ܣ����ݴ����ָ�����ڣ���ÿ���µı���������
*@������char *table_name �贫���ı���
*               struct tm *stime ��ʼʱ��
*               struct tm *btime ����ʱ��
*@����ֵ�� void
*/

void proc_combine_table(char *table_name, char *sdatet, char *edatet)
{
    char sdate[24] = "";
    char edate[24] = "";

    strcpy(sdate, sdatet);
    strcpy(edate, edatet);
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
        snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), "nwproclog_%4u%02u as new_table", syear, smonth);
    else
    {
        int i, j = smonth, iNum = 0;
        strcat(table_name, "(");

        pasDbCursor *psCur = NULL;
        for(i = syear; i <= eyear && j <= emonth;)
        {
            snprintf(sql, sizeof(sql), "select * from nwproclog_%4u%02u", i, j);
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

/*
*@���ƣ�int lan_proc_log_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
*@���ܣ�������ҳ�������Ĳ������в�ѯ���ݿ��г�����־����Ϣ���ٸ�����Ӧ��ģ�崫��ȥ
*@������utShmHead *psShmHead �����ڴ���
*               int iFd                          �ļ�������
*               utMsgHead *psMsgHead ��Ϣ���
*@����ֵ��0�ɹ�������ʧ��
*/
int lan_proc_log_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);

    char depart_in[16] = "";
    char compid_in[128] = "";
    char ip_in[16] = "";
    char sdate_in[24] = "";
    char edate_in[24] = "";
    char proc_name_in[32] = "";
    char comp_name_in[32] = "";
    char dev_name_in[32] = "";
    char user_name_in[32] = "";
    char page_start[8] = "";
    char page_limit[8] = "";
    char keyword_in[128] = "";
    char caExport[8] = "";
    char sort_in[32] = "";
    char dir_in[16] = "";
    char netTime_in[32] = "";
    char wndTime_in[32] = "";

    utMsgGetSomeNVar(psMsgHead, 17,
                     "depart",       UT_TYPE_STRING,  sizeof(depart_in) - 1,       depart_in,
                     "compId",       UT_TYPE_STRING,  sizeof(compid_in) - 1,       compid_in,
                     "ip",           UT_TYPE_STRING,  sizeof(ip_in) - 1,               ip_in,
                     "sdate",        UT_TYPE_STRING,  sizeof(sdate_in) - 1,            sdate_in,
                     "bdate",        UT_TYPE_STRING,  sizeof(edate_in) - 1,            edate_in,
                     "procName", UT_TYPE_STRING,  sizeof(proc_name_in) - 1,    proc_name_in,
                     "compName", UT_TYPE_STRING,  sizeof(comp_name_in) - 1,    comp_name_in,
                     "devName",  UT_TYPE_STRING,  sizeof(dev_name_in) - 1,     dev_name_in,
                     "userName", UT_TYPE_STRING,  sizeof(user_name_in) - 1,    user_name_in,
                     "keyword",  UT_TYPE_STRING,  sizeof(keyword_in) - 1,      keyword_in,
                     "caExport", UT_TYPE_STRING,  sizeof(caExport) - 1,            caExport,
                     "start",        UT_TYPE_STRING,  sizeof(page_start) - 1,      page_start,
                     "limit",        UT_TYPE_STRING,  sizeof(page_limit) - 1,      page_limit,
                     "sort",         UT_TYPE_STRING,  sizeof(sort_in) - 1,         sort_in,
                     "dir",          UT_TYPE_STRING,  sizeof(dir_in) - 1,      dir_in,
                     "netTime",  UT_TYPE_STRING,  sizeof(sort_in) - 1,         netTime_in,
                     "wndTime",  UT_TYPE_STRING,  sizeof(dir_in) - 1,      wndTime_in);

    char sql[2048] = "";
    char sql_tmp[2048] = "";
    pasDbCursor *psCur = NULL;
    int iret = 0;
    int iNum = 0;
    utPltDbHead *psDbHead = utPltInitDb();

    //ȡ����
    //����where �����ı�ʶ
    printf("proc before modi==%s,%s\n", sdate_in, edate_in);
    modifyDates(sdate_in, edate_in);
    printf("proc after modi==%s,%s\n", sdate_in, edate_in);

    unsigned long stime = utTimStrToLong("%Y/%m/%d %H:%M:%S", sdate_in);
    unsigned long etime = utTimStrToLong("%Y/%m/%d %H:%M:%S", edate_in);

    snprintf(sql_tmp, sizeof(sql_tmp), "where new_table.stime>=%u and new_table.stime<=%u", stime, etime);

    sdate_in[10] = '\0';
    edate_in[10] = '\0';

    //��Ӱ���������ѯ����
    if(!utStrIsSpaces(comp_name_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), "and (nwcompute.compname like '%%%s%%' or nwcompute.devname like '%%%s%%')", utf8convert(psShmHead, comp_name_in), utf8convert(psShmHead, comp_name_in));
    }

    if(!utStrIsSpaces(netTime_in))
    {
        unsigned long timeTmp = 0;
        switch(atoi(netTime_in))
        {
                //30 min
            case 1:
                timeTmp = 1800;
                break;
            case 2:
                timeTmp = 3600;
                break;
            case 3:
                timeTmp = 10800;
                break;
            case 4:
                timeTmp = 18000;
                break;
        }

        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and nettime > %u", timeTmp);
    }

    if(!utStrIsSpaces(wndTime_in))
    {
        unsigned long timeTmp = 0;;
        switch(atoi(wndTime_in))
        {
                //30 min
            case 1:
                timeTmp = 1800;
                break;
            case 2:
                timeTmp = 3600;
                break;
            case 3:
                timeTmp = 10800;
                break;
            case 4:
                timeTmp = 18000;
                break;
        }

        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and nettime > %u", timeTmp);
    }


    //Ȩ��

    char auth2[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth2);

    findShortName(auth2, shortname);

    char *auth = getDsGroupid();
    if(!utStrIsSpaces(auth))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.groupid in (%s)", auth);
    }

    //��Ӱ����Ų�ѯ����
    if(!utStrIsSpaces(depart_in) && depart_in[0] != '0')
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.groupid in (%s)", getGroupid(strtoul(depart_in, NULL, 10)));
    }

    //��Ӱ�����id��ѯ����
    if(!utStrIsSpaces(compid_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.compid = %s", compid_in);
    }

    //��Ӱ�ip��ѯ����
    if(!utStrIsSpaces(ip_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and inet_ntoa(new_table.sip) like '%%%s%%'", ip_in);
    }

    //��Ӱ��豸����ѯ����
    if(!utStrIsSpaces(dev_name_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.devname like '%%%s%%'", dev_name_in);
    }
    //��Ӱ��û�����ѯ����
    if(!utStrIsSpaces(user_name_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.udisp like '%%%s%%'", user_name_in);
    }

    //��Ӱ���������ѯ����
    if(!utStrIsSpaces(proc_name_in))
    {
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and new_table.procname like '%%%s%%'", utf8convert(psShmHead, proc_name_in));
    }
    char _sip[16] = "";
    if(!utStrIsSpaces(keyword_in))
    {

        char *keyword_gbk = utf8convert(psShmHead, keyword_in);
        snprintf(sql_tmp + strlen(sql_tmp), sizeof(sql_tmp) - strlen(sql_tmp), " and (nwcompute.devname like '%%%s%%' or nwcompute.compname like '%%%s%%' or new_table.procname like '%%%s%%' or inet_ntoa(new_table.sip) like '%%%s%%' or new_table.devname like '%%%s%%' or new_table.udisp like '%%%s%%' or pindexid like '%%%s%%')", keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk, keyword_gbk);
        free(keyword_gbk);
    }
    iNum = 0;
    //ȡ������
    unsigned int db_count = 0;
    char table_name[1024] = "";

    proc_combine_table(table_name, sdate_in, edate_in);

    snprintf(sql, sizeof(sql), "select count(*) from %s left join nwcompute on nwcompute.compid=new_table.compid %s and new_table.procname not in(select processname from nwprocignore where company='%s')", table_name, sql_tmp, shortname);

    writeLog("logCheck.txt", "procC", sql);
    psCur = pasDbOpenSql(sql, 0);
    if(NULL != psCur)
    {
        pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &db_count);
        pasDbCloseCursor(psCur);
    }
    utPltPutVarF(psDbHead, "TotRec", "%lu", db_count);

    memset(sql, 0, sizeof(sql));
    snprintf(sql, sizeof(sql), "select new_table.sid,new_table.compid,nwcompute.devname,new_table.userid,new_table.mac,new_table.groupid,new_table.devname,new_table.udisp,inet_ntoa(new_table.sip),new_table.procname,new_table.mark,new_table.pid,new_table.indexid,new_table.pindexid,new_table.stime,new_table.btime,new_table.path,mem,cpu,new_table.flags,nettime,wndtime from %s left join nwcompute on nwcompute.compid=new_table.compid %s and new_table.procname not in(select processname from nwprocignore where company='%s')", table_name, sql_tmp, shortname);

    //�������ļ�
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

        sprintf(caFilename, "ncsProcLog_%04u%02u%02u.csv", tm_tmp->tm_year + 1900, tm_tmp->tm_mon + 1, tm_tmp->tm_mday);
        //          sprintf(caFilename,"ncsuserlog%d.csv",utStrGetId());
        sprintf(caPath, "%s/html", "/home/ncmysql/nw");
        sprintf(caFile, "%s/%s", caPath, caFilename);
        fp = fopen(caFile, "w");
        if(fp == NULL)
        {
            pasDbCloseCursor(psCur);
            printf("open failed!\n");
            writeSysLog(psMsgHead, "06", "Fail", "����������־�ļ�ʧ��");
            //        utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0546���ݵ�������"),ncsLang("0546���ݵ�������"));
            return 0;
        }

        ncUtlPrintCsv(fp, 1, UT_TYPE_STRING, ncLang("0091����������־\n"));
        ncUtlPrintCsv(fp, 17, UT_TYPE_STRING, ncLang("0064���к�"),
                      UT_TYPE_STRING, ncLang("0065�����ID"),
                      UT_TYPE_STRING, ncLang("0066���������"),
                      UT_TYPE_STRING, ncLang("0067�û�ID"),
                      UT_TYPE_STRING, ncLang("0068MAC��ַ"),
                      UT_TYPE_STRING, ncLang("0069����"),
                      UT_TYPE_STRING, ncLang("0070�豸��"),
                      UT_TYPE_STRING, ncLang("0071�û���"),
                      UT_TYPE_STRING, ncLang("0072IP��ַ"),
                      UT_TYPE_STRING, ncLang("0073��������"),
                      UT_TYPE_STRING, ncLang("0084��������"),
                      UT_TYPE_STRING, ncLang("0075����ʱ��"),
                      UT_TYPE_STRING, ncLang("0084����ʱ��"),
                      UT_TYPE_STRING, ncLang("0085����·��"),
                      UT_TYPE_STRING, ncLang("0086ͳ��"),
                      UT_TYPE_STRING, ncLang("0087����ʱ��"),
                      UT_TYPE_STRING, ncLang("0088���ڻʱ��"));
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " limit 0,400");
    }
    else
    {
        if(!utStrIsSpaces(sort_in))
        {
            snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " order by new_table.%s %s", sort_in, dir_in);
        }
        else
        {
            snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " order by new_table.stime desc");
        }
        snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), " limit %d,%d", atoi(page_start), atoi(page_limit));
    }

    //�鿴sql���
    writeLog("logCheck.txt", "proc", sql);
    psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
    {
        //����Ҫȡ����������
        long long sid = 0;
        unsigned int    compid = 0;
        unsigned int    userid = 0;
        char                    mac[20] = "";
        unsigned int    groupid = 0;
        char                    devname[32] = "";
        char                    compname[32] = "";
        char                    udisp[32] = "";
        char                    sip[16] = "";
        char                    procname[72] = "";
        char                    procname16[128] = "";
        char                    mark[2] = "";
        char                    _mark[2] = "";
        int                   pid = -1;
        char                    indexid[24] = "";
        char                    pindexid[24] = "";
        unsigned int  stime = 0;
        unsigned int  btime = 0;
        char                    path[256] = "";
        unsigned int  mem = 0;
        unsigned int  cpu = 0;
        int                   flags = 0;
        unsigned long nettime = 0;
        unsigned long wndtime = 0;

        //ȡ�α�ָ�������
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_LONG8,  8,                     &sid,
                                          UT_TYPE_ULONG,  4,                         &compid,
                                          UT_TYPE_STRING, sizeof(compname) - 1,    compname,
                                          UT_TYPE_ULONG,  4,                         &userid,
                                          UT_TYPE_STRING, sizeof(mac) - 1,             mac,
                                          UT_TYPE_ULONG,  4,                         &groupid,
                                          UT_TYPE_STRING, sizeof(devname) - 1,     devname,
                                          UT_TYPE_STRING, sizeof(udisp) - 1,       udisp,
                                          UT_TYPE_STRING, sizeof(sip) - 1,             sip,
                                          UT_TYPE_STRING, sizeof(procname) - 1,    procname,
                                          UT_TYPE_STRING, sizeof(mark) - 1,            mark,
                                          UT_TYPE_LONG,   4,                         &pid,
                                          UT_TYPE_STRING, sizeof(indexid) - 1,     indexid,
                                          UT_TYPE_STRING, sizeof(pindexid) - 1,    pindexid,
                                          UT_TYPE_ULONG,  4,                         &stime,
                                          UT_TYPE_ULONG,  4,                         &btime,
                                          UT_TYPE_STRING, sizeof(path) - 1,        path,
                                          UT_TYPE_ULONG,  4,                         &mem,
                                          UT_TYPE_ULONG,  4,                         &cpu,
                                          UT_TYPE_LONG,   4,                                     &flags,
                                          UT_TYPE_ULONG,  4,                         &nettime,
                                          UT_TYPE_ULONG,  4,                                     &wndtime)) || 1405 == iret)
        {
            iNum++;
            char _stime[24] = "";
            char _btime[24] = "";
            char _wndtime[24] = "";
            char _nettime[24] = "";

            //���쳣����ʱ��ת�����ַ�����ʾ

            if(0 == btime)
            {
                strcpy(_btime, "0");
            }
            else
            {
                switchToRemainTime(_btime, btime);
            }

            if(0 == wndtime)
            {
                strcpy(_wndtime, "0");
            }
            else
            {
                switchToRemainTime(_wndtime, wndtime);
            }

            if(0 == nettime)
            {
                strcpy(_nettime, "0");
            }
            else
            {
                switchToRemainTime(_nettime, nettime);
            }

            strcpy(_stime, utTimFormat("%H:%M:%S %Y/%m/%d ", stime));
            nwCompInfo *comp_info = NULL;

            comp_info = utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);

            if(strlen(caExport) > 0)
            {
                ncUtlPrintCsv(fp, 17,
                              UT_TYPE_ULONG, sid,
                              UT_TYPE_ULONG, compid,
                              UT_TYPE_STRING, compname,
                              UT_TYPE_ULONG, userid,
                              UT_TYPE_STRING, mac,
                              UT_TYPE_STRING, lanGetGroup(psShmHead, groupid),
                              UT_TYPE_STRING, devname,
                              UT_TYPE_STRING, udisp,
                              UT_TYPE_STRING, sip,
                              UT_TYPE_STRING, procname,
                              UT_TYPE_STRING, _mark,
                              UT_TYPE_STRING, _stime,
                              UT_TYPE_STRING, _btime,
                              UT_TYPE_STRING, path,
                              UT_TYPE_ULONG, flags,
                              UT_TYPE_STRING, _wndtime,
                              UT_TYPE_STRING, _nettime);
            }
            else
            {
                //�򻺴��������
                if(iNum > 1)
                {
                    utPltPutLoopVar(psDbHead, "dh", iNum, ",");
                }
                utPltPutLoopVarF(psDbHead, "sid", iNum, "%lld", sid);
                if(comp_info)
                {
                    utPltPutLoopVar(psDbHead, "compName", iNum, comp_info->compname);
                }
                utPltPutLoopVarF(psDbHead, "userid", iNum, "%u", userid);
                utPltPutLoopVar(psDbHead, "mac", iNum, mac);
                utPltPutLoopVar(psDbHead, "groupid", iNum, lanGetGroup(psShmHead, groupid));
                utPltPutLoopVar(psDbHead, "devname", iNum, devname);
                utPltPutLoopVar(psDbHead, "compname", iNum, compname);
                utPltPutLoopVar(psDbHead, "udisp", iNum, udisp);
                utPltPutLoopVar(psDbHead, "sip", iNum, sip);
                pasStrCvtBin2Hex(procname, strlen(procname), procname16);
                utPltPutLoopVar(psDbHead, "procname16", iNum, procname16);
                utPltPutLoopVar(psDbHead, "procname", iNum, procname);
                utPltPutLoopVar(psDbHead, "mark", iNum, _mark);
                utPltPutLoopVarF(psDbHead, "pid", iNum, "%d", pid);
                utPltPutLoopVar(psDbHead, "indexid", iNum, indexid);
                utPltPutLoopVar(psDbHead, "pindexid", iNum, pindexid);
                //utPltPutLoopVar(psDbHead, "pindexid", iNum, convertCSV(psShmHead, pindexid));                
                utPltPutLoopVar(psDbHead, "stime", iNum, _stime);
                utPltPutLoopVar(psDbHead, "btime", iNum, _btime);
                utPltPutLoopVar(psDbHead, "wndtime", iNum, _wndtime);
                utPltPutLoopVar(psDbHead, "nettime", iNum, _nettime);
                if(strlen(path) >= 254)
                {
                    utPltPutLoopVar(psDbHead, "path", iNum, utStrGetCharHan(path, 252));
                }
                else
                {
                    utPltPutLoopVar(psDbHead, "path", iNum, path);
                }
                utPltPutLoopVarF(psDbHead, "mem", iNum, "%u", mem);
                utPltPutLoopVarF(psDbHead, "cpu", iNum, "%u", cpu);
                utPltPutLoopVarF(psDbHead, "flags", iNum, "%d", flags);

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
        writeSysLog(psMsgHead, "06", "Success", "����������־");
        return 0;
    }
    // utPltShowDb(psDbHead);
    char msg[256] = "";

    char *compname = findCompNameById(strtoul(compid_in, NULL, 10));
    if(!utStrIsSpaces(depart_in))
    {
        snprintf(msg, sizeof(msg), "��ѯ[%s]������־", lanGetGroup(psShmHead, atoi(depart_in)));
    }
    else if(!utStrIsSpaces(compid_in))
    {
        snprintf(msg, sizeof(msg), "��ѯ[%s]������־", compname);
    }
    else
    {
        snprintf(msg, sizeof(msg), "��ѯ[ȫ��]������־");
    }
    free(compname);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_log_proc.htm");
    writeSysLog(psMsgHead, "06", "Success", msg);
    return 0;
}

/*
*@���ƣ�lan_soft_class_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
*@���ܣ�������ҳ�������Ĳ������в�ѯ���ݿ���������͵���Ϣ���ٸ�����Ӧ��ģ�崫��ȥ
*@������utShmHead *psShmHead �����ڴ���
*               int iFd                          �ļ�������
*               utMsgHead *psMsgHead ��Ϣ���
*@����ֵ��0�ɹ�������ʧ��
*/
int lan_soft_class_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char sql[128] = "";
    pasDbCursor *psCur = NULL;
    int iret = 0;
    int iNum = 0;
    utPltDbHead *psDbHead = utPltInitDb();

    strcpy(sql, "select id,name from nwsoftclass");

    psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
    {
        unsigned int soft_id = 0;
        char soft_name[64] = "";
        int iNum = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  4,                                      &soft_id,
                                          UT_TYPE_STRING, sizeof(soft_name) - 1,    soft_name)) || 1405 == iret)
        {
            iNum++;
            //�򻺴��������
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            char *_soft_name = utf8convert(psShmHead, soft_name);
            utPltPutLoopVarF(psDbHead, "id", iNum, "%u", soft_id);
            utPltPutLoopVar(psDbHead, "name", iNum, _soft_name);
        }
        pasDbCloseCursor(psCur);
        //�鿴�� �������?
        //              utPltShowDb(psDbHead);
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_soft_class.htm");
    }
}