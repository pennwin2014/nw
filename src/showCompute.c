#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utoall.h"
#include "pasdef.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "utoall.h"
#include "pasdb.h"
#include "ncdef.h"
#include "pasutl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <utoall.h>
#include "nwdef.h"
#include "pasdb.h"
#include <dirent.h>

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

    return tableName;
}

int testSql(void)
{
    utPltDbHead *psDbHead;
    psDbHead = utPltInitDb();
    char sql[1024] = "";
    char caName[33] = "";
    pasDbCursor *psCur = NULL;
    snprintf(sql, sizeof(sql) - 1, "select name from dsuser");
    int iReturn = 0;
    psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
    {
        while((iReturn == 0) || (1405 == iReturn))
        {
            memset(caName,  0, sizeof(caName));
            iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, sizeof(caName) - 1, caName);
			printf("name=%s\n");
        }
        pasDbCloseCursor(psCur);
    }else{
		printf("psCur NULL,sql=%s\n", sql);
	}
    return 0;
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
		testSql();
        ret = 0;//pasDbExecSqlF(sql);
        testSql();
        if(ret > 0)
        {
            printf("sql=%s,ret=%d\n", sql, ret);
            return 0;
        }
        //É¾³ý¸ÃÎÄ¼þ
        printf("load db success sql=[%s]\n", sql);
        sprintf(deleteStr, "rm -rf %s/%s", basePath, filename);
        printf("system(%s)\n", deleteStr);
        //system(deleteStr);
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

int loadImDB(utShmHead *psShmHead)
{
    DIR *dir;
    char basePath[1000] = "";
    char dateStr[9] = "";

    snprintf(dateStr, sizeof(dateStr), "%s", utTimFormat("%Y%m%d", time(0)));
    snprintf(basePath, sizeof(basePath) - 1, "/home/ncmysql/nw/keep/%s", dateStr);
    readFileList(basePath, dateStr);
    return 0;
}



int showOnlineCompute(utShmHead *psShmHead)
{
    pasHashInfo sHashInfo;
    uchar *pHash;
    int iReturn;
    long count = 0;
    char caMacStr[20] = "";
    char caIp[20] = "";
    nwCompInfo* psComp;
    pHash = (unsigned char *)utShmHashHead(psShmHead, NC_LNK_COMPUTE);
    if(pHash)
    {
        psComp = (nwCompInfo *)pasHashFirst(pHash, &sHashInfo);
        while(psComp)
        {
            strcpy(caIp, utComHostIp(htonl(psComp->ip)));
            printf("\n===============compid=%lu,index=%lu===========\n", psComp->compid, count++);
            printf("os=[%s],ver=[%s],cmpname=[%s],mac=[%s],ip=[%s]\n", psComp->os, psComp->version, psComp->compname,
                   pasCvtMacStr(psComp->mac, caMacStr), caIp);

            printf("uid=[%lu],stime=[%lu],koff=[%lu],ie=[%s],htm=[%lu]\n", psComp->userid,
                   psComp->stime, psComp->kickoff, psComp->ie, psComp->htime);
            printf("gid=[%lu],dname=[%s],ctm=[%lu],cmd=[%d],ckey=[%s],status=[%d]\n\n", psComp->groupid,
                   psComp->devname, psComp->ctime, psComp->cmod, psComp->caKey,
                   psComp->status);

            psComp = (nwCompInfo *)pasHashNextS(&sHashInfo);
        }
    }
    return 0;
}


int main(int argc, char **argv)
{
    utShmHead  *psShmHead;
    char caConfFile[64] = "";
    printf("in function main");
    strcpy(caConfFile, "pqconf_nc.cnf");
    psShmHead = utComShmOpen(caConfFile);
    if(psShmHead == NULL)
    {
        printf(" Share Memory not Init \n");
        exit(0);
    }
    else
    {
        printf("Share Memeory not null\n");
    }
    //showOnlineCompute(psShmHead);
    loadImDB(psShmHead);
    utShmClose(psShmHead);
    exit(0);
}

