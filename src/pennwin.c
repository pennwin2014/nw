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

#include <sys/statfs.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>




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
        snprintf(sql, sizeof(sql) - 1, "load data local infile '%s/%s'  into table %s", basePath, filename, tableName);
        ret = pasDbExecSqlF(sql);
        if(ret > 0)
        {
            printf("sql=%s,ret=%d\n", sql, ret);
            return 0;
        }
        //删除该文件
        printf("load db success sql=[%s]\n", sql);
        sprintf(deleteStr, "rm -rf %s/%s", basePath, filename);
        printf("do cmd: %s\n", deleteStr);
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
        return 0;
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

static ulong total = 0;
static int sum(const char *fpath, const struct stat *sb, int typeflag)
{
    total += sb->st_size;
    return 0;
}

#define DATA_DISK_SECTION "/data"
#define DB_DISK_SECTION "/db"

int getDiskSize(char* sectionPath, uint8* totalDisk, uint8* availDisk){
	struct statfs diskInfo;
	statfs(sectionPath, &diskInfo);
	uint8 blocksize = diskInfo.f_bsize;
	*totalDisk = blocksize * diskInfo.f_blocks;
	*availDisk = diskInfo.f_bavail * blocksize; 
	/*
	uint8 totalsize = blocksize * diskInfo.f_blocks;
    uint8 freeDisk = diskInfo.f_bfree * blocksize; 
    uint8 availableDisk = diskInfo.f_bavail * blocksize; 
	printf("Total_size = %llu B = %llu KB = %llu MB = %llu GB\n",
					totalsize, totalsize>>10, totalsize>>20, totalsize>>30);
    printf("Disk_free = %llu MB = %llu GB\nDisk_available = %llu MB = %llu GB\n",
                freeDisk>>20, freeDisk>>30, availableDisk>>20, availableDisk>>30);
                */
	return 0;
}
ulong getFolderSize(char* dirPath)
{
    total = 0;

    if(!dirPath || access(dirPath, R_OK))
    {
        return 1;
    }

    if(ftw(dirPath, &sum, 1))
    {
        perror("ftw");
        return 2;
    }
    return total;
}

int deleteJpgFileByMonth(char* JpgDir, char* caShortName, char* dateStr)
{
	
	return 0;
}


//SCREENJPG_DIR  201509
ulong getJpgFileSizeByMonth(char* JpgDir, char* caShortName, char* dateStr)
{
    ulong lSize = 0;
    char jpgPath[256] = "";
    snprintf(jpgPath, sizeof(jpgPath) - 1, "%s%s", JpgDir, caShortName);
    //遍历该目录下的所有文件夹
    char tempDir1[256] = "";
    char tempDir2[256] = "";


    DIR *dir1 = NULL, *dir2 = NULL;
    struct dirent *ptr1 = NULL, *ptr2 = NULL;
    printf("jpgPath= [%s]\n", jpgPath);
    if((dir1 = opendir(jpgPath)) == NULL)
    {
        perror("Open dir error...");
        return 0;
    }

    while((ptr1 = readdir(dir1)) != NULL)
    {
        if(strcmp(ptr1->d_name, ".") == 0 || strcmp(ptr1->d_name, "..") == 0) ///current dir OR parrent dir
            continue;
        if(ptr1->d_type == 4)    ///dir
        {
            memset(tempDir1, '\0', sizeof(tempDir1));
            strcpy(tempDir1, jpgPath);
            strcat(tempDir1, "/");
            strcat(tempDir1, ptr1->d_name);
            printf("tempDir1= [%s]\n", tempDir1);

            //遍历二级目录
            if((dir2 = opendir(tempDir1)) == NULL)
            {
                perror("Open dir2 error...");
                continue;
            }
            while((ptr2 = readdir(dir2)) != NULL)
            {
                if(strcmp(ptr2->d_name, ".") == 0 || strcmp(ptr2->d_name, "..") == 0) ///current dir OR parrent dir
                    continue;
                if(ptr2->d_type == 4)    ///dir
                {
                    if(strncmp(ptr2->d_name, dateStr, 6) == 0)
                    {
                        memset(tempDir2, 0, sizeof(tempDir2));
                        snprintf(tempDir2, sizeof(tempDir2) - 1, "%s/%s", tempDir1, ptr2->d_name);
                        printf("tempDir2=%s,ptr2->d_reclen=%d\n", tempDir2, ptr2->d_reclen);
                        lSize += getFolderSize(tempDir2);
                        printf("tempDir2=%s, lSize=%lu\n", tempDir2, lSize);
                    }
                }
            }
            closedir(dir2);
        }
    }
    closedir(dir1);
    printf("======================finish lSize=%luB [%.2fMB]==============\n", lSize, (lSize * 1.0 / 1024.0) / 1024.0);
    return lSize;
}
#define  SCREENJPG_DIR      "/home/ncmysql/nw/screenJPG/"
#define    SCREENALTERJPG_DIR "/home/ncmysql/nw/screenAlterJPG/"
#define    PROCESSJPG_DIR     "/home/ncmysql/nw/processJPG/"

ulong getDbTableSize(char* tableName)
{
    pasDbCursor *psCur = NULL;
    char sql[2512] = "";
    sprintf(sql, "show table status like '%c%s%c'", '%', tableName, '%');
    psCur = pasDbOpenSql(sql, 0);
    if(psCur == NULL)
    {
        printf("open sql [%s] error\n", sql);
        return -1;
    }
    char tmpData[128] = "";
    char caName[128] = "";
    ulong dataLenght = 0;
    memset(tmpData, 0, sizeof(tmpData));
    int iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_STRING, sizeof(caName) - 1, caName,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData,
                                 UT_TYPE_LONG, 4, &dataLenght,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData);
    printf("sql=%s, iRet=%d\n", sql, iReturn);

    /*
    while((iReturn == 0)|| (1405 == iReturn))
    {
        printf("caName=%s, dataLength=%lu\n", caName, dataLenght);
        dataLenght = 0;
        memset(tmpData, 0, sizeof(tmpData));
        memset(caName, 0, sizeof(caName));
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_STRING, sizeof(caName) - 1, caName,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData,
                                 UT_TYPE_LONG, 4, &dataLenght,
                                 UT_TYPE_STRING, sizeof(tmpData) - 1, tmpData);

    }
    */
    pasDbCloseCursor(psCur);
    return dataLenght;
}

char* getMonthTableName(char* caShortName, char* tableName, char* tableMonth)
{
    char caOriTable[256] = "";
    sprintf(caOriTable, "%s_%s", tableName, tableMonth);
    return getNewTableName(caShortName, caOriTable);
}

int getCleanDetailByMonth(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char caMonth[7] = "";
	char caDelete[7] = "";
    char caShortName[128] = "";
	//传递的待删除的项目
	char caScreen[32] = "";
    int iReturn = 0;
    utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead, 1,
                               "month",       UT_TYPE_STRING,  sizeof(caMonth) - 1,        caMonth,
                               "delete",      UT_TYPE_STRING,  sizeof(caDelete) - 1,        caDelete,
                               "f_screen",    UT_TYPE_STRING,  sizeof(caScreen) - 1,        caScreen);

    strcpy(caShortName, getLoginShortName());
    utPltDbHead *psDbHead = utPltInitDb();
	if(strlen(caDelete)>0){
		//删除该月对应明细
		if(strlen(caScreen)>0){
			deleteJpgFileByMonth(SCREENJPG_DIR, caShortName, caMonth);
		}
	}
    //数据库
    utPltPutVar(psDbHead, "month", caMonth);
    utPltPutVarF(psDbHead, "ncimclient", "%lu", getDbTableSize(getMonthTableName(caShortName, "ncimclient", caMonth)));
    utPltPutVarF(psDbHead, "nwweblog", "%lu", getDbTableSize(getMonthTableName(caShortName, "nwweblog", caMonth)));
    utPltPutVarF(psDbHead, "nwproclog", "%lu", getDbTableSize(getMonthTableName(caShortName, "nwproclog", caMonth)));
    utPltPutVarF(psDbHead, "nwcomputelog", "%lu", getDbTableSize(getMonthTableName(caShortName, "nwcomputelog", caMonth)));
    utPltPutVarF(psDbHead, "nwoutfilelog", "%lu", getDbTableSize(getMonthTableName(caShortName, "nwoutfilelog", caMonth)));
    utPltPutVarF(psDbHead, "nwfilelog", "%lu", getDbTableSize(getMonthTableName(caShortName, "nwfilelog", caMonth)));
    utPltPutVarF(psDbHead, "nwprocscreen", "%lu", getDbTableSize(getMonthTableName(caShortName, "nwprocscreen", caMonth)));
    //文件
    ulong jpgSize = getJpgFileSizeByMonth(SCREENJPG_DIR, caShortName, caMonth);
    utPltPutVarF(psDbHead, "screen", "%lu", jpgSize);
    utPltPutVarF(psDbHead, "alter", "%lu", 0);
    utPltPutVarF(psDbHead, "process", "%lu", 0);

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "clean/clean_month_detail.htm");
    return 0;
}

int isDateFolder(char* caFolderName)
{
    if(strlen(caFolderName) != 8)
        return 0;
    int i = 0;
    for(i = 0; i < 4; i++)
    {
        if((caFolderName[i] > '9') || (caFolderName[i] < '0'))
        {
            return 0;
        }
    }
    char caMonth[3] = "";
    memcpy(caMonth, caFolderName + 4, 2);
    int iMonth = atoi(caMonth);
    if((iMonth < 1) || (iMonth > 12))
        return 0;
    char caDate[3] = "";
    memcpy(caDate, caFolderName + 6, 2);
    int iDate = atoi(caDate);
    if((iDate < 0) || (iDate > 31))
        return 0;
    return 1;
}

int calcLogTables(utShmHead *psShmHead, char* tableBaseName, char* caShortName)
{
	char caMonthStr[7] = "";
    char caCleanKey[135] = "";
	ulong lSize = 0;
	lanCleanData* psData = NULL;
	memset(caMonthStr, 0, sizeof(caMonthStr));
	//1、遍历得到该简称下所有像tableBaseName的表名(会包含年月信息caMonthStr)
	memcpy(caMonthStr,"201509", 6);
	//2、计算该表的空间lSize
	lSize = 123456;
	//3、组装该表所该对应的key
	memset(caCleanKey, 0, sizeof(caCleanKey));
	snprintf(caCleanKey, sizeof(caCleanKey) - 1, "%s%s", caMonthStr, caShortName);
    psData = (lanCleanData *)utShmHashLookA(psShmHead, NC_LNK_NW_CLEAN, caCleanKey);
    if(psData)
    {	
    	//3、根据tableBaseName的不同，给psData中的不同数据赋值
		psData->lImClient += lSize;
    }

	memset(caMonthStr, 0, sizeof(caMonthStr));
	//1、遍历得到该简称下所有像tableBaseName的表名(会包含年月信息caMonthStr)
	memcpy(caMonthStr,"201510", 6);
	//2、计算该表的空间lSize
	lSize = 13456;
	//3、组装该表所该对应的key
	memset(caCleanKey, 0, sizeof(caCleanKey));
	snprintf(caCleanKey, sizeof(caCleanKey) - 1, "%s%s", caMonthStr, caShortName);
    psData = (lanCleanData *)utShmHashLookA(psShmHead, NC_LNK_NW_CLEAN, caCleanKey);
    if(psData)
    {	
    	//3、根据tableBaseName的不同，给psData中的不同数据赋值
		psData->lImClient += lSize;
    }
	return 0;
}

int lanCalcClean(utShmHead *psShmHead)
{
    int iReturn = 0;
    ulong lShortCount = 0;
    iReturn = pasConnect(psShmHead);
    if(iReturn < 0)
    {
        printf("conn psShmHead fail iReturn=%d\n", iReturn);
        sleep(60);
        return 0;
    }
    else
    {
        //printf("conn psShmHead success iReturn=%d\n", iReturn);
    }
    pasDbCursor *psCur;
    char caMonthStr[7] = "";
    char caShortName[128] = "";
    char caCleanKey[135] = "";
    char sql[1024] = "";
    ulong lFolderSize = 0, lIndex = 0;

    while(1)
    {
        utShmFreeHash(psShmHead, NC_LNK_NW_CLEAN);
        utShmHashInit(psShmHead, NC_LNK_NW_CLEAN, 2000, 2000, sizeof(lanCleanData), 0, 135);
        //1、查表得到所有的公司简称存放在哈希表caShortName
        memset(sql, 0, sizeof(sql));
        snprintf(sql, sizeof(sql), "select ifnull(count(shortname), 0) from userapply");
        iReturn = pasDbOneRecord(sql, 0, UT_TYPE_LONG, 4, &lShortCount);
        char** caShortNameList = (char**)malloc(sizeof(char*) * (lShortCount + 1));
        memset(sql, 0, sizeof(sql));
        snprintf(sql, sizeof(sql), "select shortname from userapply");
        psCur = pasDbOpenSqlF(sql);

        if(psCur)
        {
            lIndex = 0;
            strcpy(caShortName, "lan");
            iReturn = 0;
            while(iReturn == 0 || iReturn == 1405)
            {
                caShortNameList[lIndex] = (char*)malloc(128 * sizeof(char));
                memset(caShortNameList[lIndex], 0, 128);
                memcpy(caShortNameList[lIndex], caShortName, 128);
                //printf("caShortName=%s\n", caShortName);
                memset(caShortName, 0, sizeof(caShortName));
                iReturn = pasDbFetchInto(psCur,
                                         UT_TYPE_STRING, sizeof(caShortName), caShortName);
                lIndex++;
            }
            pasDbCloseCursor(psCur);
        }
        for(lIndex = 0; lIndex <= lShortCount; lIndex++)
        {
            // printf("caShortNameList[lIndex]=%s\n", caShortNameList[lIndex]);
            char jpgPath[256] = "";
            snprintf(jpgPath, sizeof(jpgPath) - 1, "%s%s", SCREENJPG_DIR, caShortNameList[lIndex]);
            //2、遍历该公司简称对应的目录下的每个compid目录
            char tempDir1[256] = "";
            char tempDir2[256] = "";
            DIR *dir1 = NULL, *dir2 = NULL;
            struct dirent *ptr1 = NULL, *ptr2 = NULL;
            if((dir1 = opendir(jpgPath)) == NULL)
            {
                printf("Open dir error...");
                continue;
            }
            while((ptr1 = readdir(dir1)) != NULL)
            {
                if(strcmp(ptr1->d_name, ".") == 0 || strcmp(ptr1->d_name, "..") == 0)
                    continue;
                if(ptr1->d_type == 4)    ///dir
                {
                    memset(tempDir1, '\0', sizeof(tempDir1));
                    strcpy(tempDir1, jpgPath);
                    strcat(tempDir1, "/");
                    strcat(tempDir1, ptr1->d_name);
                    //printf("rr tempDir1= [%s]\n", tempDir1);

                    //遍历该compid目录下的日期目录caFolederName
                    if((dir2 = opendir(tempDir1)) == NULL)
                    {
                        perror("Open dir2 error...");
                        continue;
                    }
                    while((ptr2 = readdir(dir2)) != NULL)
                    {
                        if(strcmp(ptr2->d_name, ".") == 0 || strcmp(ptr2->d_name, "..") == 0) ///current dir OR parrent dir
                            continue;
                        if(ptr2->d_type == 4)    ///dir
                        {
                            //2.5、对目录名做有效性检查
                            if(isDateFolder(ptr2->d_name))
                            {
                                memset(tempDir2, 0, sizeof(tempDir2));
                                snprintf(tempDir2, sizeof(tempDir2) - 1, "%s/%s", tempDir1, ptr2->d_name);
                                lFolderSize = getFolderSize(tempDir2);
                                memset(caMonthStr, 0, sizeof(caMonthStr));
                                //3、读取每个文件夹名中的月份caMonthStr，组成caCleanKey=caMonthStr+caShortName
                                memcpy(caMonthStr, ptr2->d_name, 6);
                                memset(caCleanKey, 0, sizeof(caCleanKey));
                                snprintf(caCleanKey, sizeof(caCleanKey) - 1, "%s%s", caMonthStr, caShortNameList[lIndex]);
                                //printf("caCleanKey=%s,tempDir2=%s,lFolderSize=%lu\n", caCleanKey, tempDir2, lFolderSize);
                                //4、读取共享内存中该key对应的数据并累加里面对应的lScreenJpg
                                lanCleanData* psData = (lanCleanData *)utShmHashLookA(psShmHead, NC_LNK_NW_CLEAN, caCleanKey);
                                if(psData)
                                {
                                    memcpy(psData->cleanKey, caCleanKey, sizeof(psData->cleanKey));
                                    //遍历screenJPg文件夹
                                    // printf("psData->lScreenJpg=%lu\n", psData->lScreenJpg);
                                    psData->lScreenJpg += lFolderSize;
                                }
                            }
                        }
                    }
                    closedir(dir2);
                }
            }
            closedir(dir1);
			//统计数据库相关
        	calcLogTables(psShmHead, "ncimclient", caShortNameList[lIndex]);
			calcLogTables(psShmHead, "nwoutfilelog", caShortNameList[lIndex]);
			calcLogTables(psShmHead, "nwfilelog", caShortNameList[lIndex]);
        }
        //5、释放内存
        for(lIndex = 0; lIndex <= lShortCount; lIndex++)
        {
            free(caShortNameList[lIndex]);
        }
        free(caShortNameList);
        
        sleep(30);
    }
    return 0;
}


int getCleanPercent(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned char *pHash;
    lanCleanData *psCleanData;
    pasHashInfo sHashInfo;

    char caShortName[128] = "";
    char caMonthStr[7] = "";
    strcpy(caShortName, getLoginShortName());
    utPltDbHead *psDbHead = utPltInitDb();
    ulong iNum = 0;
    pHash = (unsigned char *)utShmHashHead(psShmHead, NC_LNK_NW_CLEAN);
    if(pHash)
    {
        psCleanData = (lanCleanData *)pasHashFirst(pHash, &sHashInfo);
        while(psCleanData)
        {
            //printf("caShortName=%s,cleanKey=%s\n", caShortName, psCleanData->cleanKey);
            if(strstr(psCleanData->cleanKey, caShortName))
            {
                iNum++;
                if(iNum > 1)
                {
                    utPltPutLoopVar(psDbHead, "dh", iNum, ",");
                }
                memset(caMonthStr, 0, sizeof(caMonthStr));
                memcpy(caMonthStr, psCleanData->cleanKey, 6);
                utPltPutLoopVar(psDbHead, "month", iNum, caMonthStr);
                utPltPutLoopVarF(psDbHead, "lsize", iNum, "%lu", psCleanData->lScreenJpg);
				utPltPutLoopVarF(psDbHead, "ldbsize", iNum, "%lu", psCleanData->lImClient);
            }
            psCleanData = (lanCleanData *)pasHashNextS(&sHashInfo);
        }
    }
	uint8 totalDisk = 0, availDisk = 0;
	uint8 totalDbDisk = 0, availDbDisk = 0;
	getDiskSize(DATA_DISK_SECTION, &totalDisk, &availDisk);
	getDiskSize(DB_DISK_SECTION, &totalDbDisk, &availDbDisk);
	printf("%llu, %llu, %llu, %llu\n", totalDisk, availDisk, totalDbDisk, availDbDisk);
	utPltPutVarF(psDbHead, "TotalDisk", "%llu", totalDisk);
	utPltPutVarF(psDbHead, "AvailDisk", "%llu", availDisk);
	utPltPutVarF(psDbHead, "TotalDbDisk", "%llu", totalDbDisk);
	utPltPutVarF(psDbHead, "AvailDbDisk", "%llu", availDbDisk);
    utPltPutVarF(psDbHead, "TotRec", "%lu", iNum);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "clean/clean_percent.htm");
    return 0;
}



int pennwin_initFun(utShmHead *psShmHead)
{
    int iReturn;
    iReturn = pasSetTcpFunName("pennwin_get_data", pennwin_get_data, 0);
    iReturn = pasSetTcpFunName("pennwin_set_data", pennwin_set_data, 0);
    iReturn = pasSetTcpFunName("getCleanDetailByMonth", getCleanDetailByMonth, 0);
    iReturn = pasSetTcpFunName("getCleanPercent", getCleanPercent, 0);
    return 0;
}


