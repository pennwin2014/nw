#include "lan_md5.h"
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stddef.h"
#include <math.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "nwdef.h"
#include "utoall.h"
#include <dirent.h>

#define PI                      3.1415926
#define EARTH_RADIUS            6378.137        //地球近似半径


int pennwin_set_data(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char msg[256] = "";
    char num[256] = "";
    int iReturn = 0;
    utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead, 2,
                               "num",       UT_TYPE_STRING,  sizeof(num) - 1,        num,
                               "msg",       UT_TYPE_STRING,  sizeof(msg) - 1,        msg);
    time_t local_time = 0;
    time(&local_time);
    char sql[2512] = "";
    utPltDbHead *psDbHead = utPltInitDb();
    snprintf(sql, sizeof(sql) - 1,  "insert into people_distribute(userid, collect_time, collect_number, mesg) values(%u, %u, \'%s\', \'%s\')", 1, local_time, num, msg);
    printf("sql = %s\n",  sql);
    int iret = pasDbExecSqlF(sql);
    if(iret == 0)
    {
        utPltPutVar(psDbHead, "buff", "{success:true,value:'success'}");
    }
    else
    {
        utPltPutVar(psDbHead, "buff", "{success:true,value:'failure'}");
    }
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_tmpPlate.htm");
    return 0;
}

int pennwin_get_data(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char sql[1024] = "";
    char userid[56] = "";
    int iReturn = 0;
    utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead, 1,
                               "userid",        UT_TYPE_STRING,  sizeof(userid) - 1,  userid);
    printf("userid=%s\n", userid);
    utPltDbHead *psDbHead = utPltInitDb();

    snprintf(sql, sizeof(sql) - 1, "select collect_time, collect_number from people_distribute where userid=%s", userid);
    int iNum = 0;
    int iret = 0;
    pasDbCursor *psCur = NULL;
    printf("%s\n", sql);
    psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
    {
        long collect_number = 0;
        long collect_time = 0;
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_LONG,           4,          &collect_time,
                                          UT_TYPE_LONG,           4,          &collect_number)) || 1405 == iret)
        {

            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            utPltPutLoopVarF(psDbHead, "collect_number", iNum, "%d", collect_number);
            utPltPutLoopVarF(psDbHead, "collect_time", iNum, "%d", collect_time);

            //清空容器
            collect_number = 0;
            collect_time = 0;
        }
    }
    pasDbCloseCursor(psCur);
    utPltPutVarF(psDbHead, "totalCount", "%lu", iNum);
    printf("final iNum = %lu\n", iNum);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/pennwin.htm");
    return 0;
}


int getCharCount(char* str, char desChar)
{
    int iCount = 0, i = 0;
    for(i = 0; i < strlen(str); i++)
    {
        if(str[i] == desChar)
            iCount ++;
    }
    return iCount;
}

int getCharPos(char* str, char desChar)
{
    int i = 0;
    for(i = 0; i < strlen(str); i++)
    {
        if(str[i] == desChar)
        {
            return i;
        }
    }
    return -1;
}

int createTableByName(char* tableName)
{
    int pos1 = 0, pos2 = 0;
    char templateName[128] = "";
    char sql[1024] = "";
	printf("tableName=[%s],", tableName);
    //判断有几个_，大于等于2个则认为是lan的，否则加上简称
    if(getCharCount(tableName, '_') > 1)
    {
        //netway_nwoutfilelog_201510 netway_nwfilelog_201510
        //获取得到第一个_的序号
        pos1 = getCharPos(tableName, '_');
        if(pos1 <= 0)
            return 1;
        //获取得到第二个_的序号
        pos2 = pos1 + getCharPos(tableName + pos1 + 1, '_');
        if(pos2 <= 0)
            return 1;
		printf("pos1=%d, pos2=%d\n", pos1, pos2);
        memcpy(templateName, tableName + pos1 + 1, pos2 - pos1);
    }
    else
    {
        //e.g.nwoutfilelog_201510  
        //获取得到第一个_的序号
        pos1 = getCharPos(tableName, '_');
		printf("pos1=%d\n", pos1);
        memcpy(templateName, tableName, pos1);
    }
    printf("============templateName=[%s]=========\n", templateName);
    snprintf(sql, sizeof(sql) - 1, "create table %s like %s", tableName, templateName);
    printf("not exist logTable=%s, do sql=%s\n", tableName, sql);
    pasDbExecSqlF(sql);
    return 0;
}

char* getTableName(char* filename)
{
    static char tableName[128];
    memset(tableName, 0, sizeof(tableName));
    long nameLength = 0;
    nameLength = strlen(filename);
    if(nameLength > 13)
    {
        memcpy(tableName, filename, nameLength - 13);
    }
    //检测该表是否存在，不存在则根据模版创建该表
    if(!checkTableExists(tableName))
    {
        createTableByName(tableName);
    }

    return tableName;
}

int doWithFile(char* basePath, char* filename, char* day)
{
    char restStr[128] = "";
    char tableName[128] = "";
    char sql[2048] = "";
    char deleteStr[1024] = "";
    int ret = 0;

    memset(sql, 0, sizeof(sql));
    snprintf(restStr, sizeof(restStr) - 1, "_%s999999.dat", day);

    if(strcmp(filename + strlen(filename) - 13, restStr) == 0)
    {
        strcpy(tableName, getTableName(filename));
        printf("==========tablename=%s=================\n", tableName);
        printf("do load file:%s/%s\n", basePath, filename);
        snprintf(sql, sizeof(sql) - 1, "load data local infile '%s/%s'  into table %s\n", basePath, filename, tableName);
        printf("sql=%s\n", sql);
        ret = pasDbExecSqlF(sql);
        if(ret > 0)
        {
            printf("sql=%s,ret=%d\n", sql, ret);
            return 0;
        }
        //删除该文件
        printf("load db success sql=[%s]\n", sql);
        sprintf(deleteStr, "rm -rf %s/%s", basePath, filename);
        printf("system(%s)\n", deleteStr);
        system(deleteStr);
        return 1;
    }
    return 0;
}

int readFileList(char *basePath, char* dateStr)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000] = "";

    if((dir = opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while((ptr = readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
        {
            if(!doWithFile(basePath, ptr->d_name, dateStr + 6))
            {
                printf("ignore file:%s/%s\n", basePath, ptr->d_name);
            }
        }
        else if(ptr->d_type == 10)    ///link file
        {
            if(!doWithFile(basePath, ptr->d_name, dateStr + 6))
            {
                printf("ignore link file:%s/%s\n", basePath, ptr->d_name);
            }
        }
        else if(ptr->d_type == 4)    ///dir
        {
            memset(base, '\0', sizeof(base));
            strcpy(base, basePath);
            strcat(base, "/");
            strcat(base, ptr->d_name);
            readFileList(base, dateStr);
        }
    }
    closedir(dir);
    return 1;
}

int lanLoadLogToDB(utShmHead *psShmHead)
{
    DIR *dir;
    char basePath[1000] = "";
    char dateStr[9] = "";
    int iReturn = pasConnect(psShmHead);
    if(iReturn < 0)
    {
        pasLog(PAS_LOG_ERROR, "Start LoadLogToDB, Database connect error");
        sleep(60);
        return 0;
    }

    snprintf(dateStr, sizeof(dateStr), "%s", utTimFormat("%Y%m%d", time(0)));
    snprintf(basePath, sizeof(basePath) - 1, "/home/ncmysql/nw/log/%s", dateStr);
    readFileList(basePath, dateStr);
    return 0;
}


int pennwin_initFun(utShmHead *psShmHead)
{
    int iReturn;
    iReturn = pasSetTcpFunName("pennwin_get_data", pennwin_get_data, 0);
    iReturn = pasSetTcpFunName("pennwin_set_data", pennwin_set_data, 0);
    return 0;
}


