#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

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

int doWithFile(char* basePath, char* filename, int day)
{
    char restStr[128] = "";
	char tableName[128] = "";
	char sql[2048] = "";
	memset(sql, 0, sizeof(sql));
    snprintf(restStr, sizeof(restStr) - 1, "_%02d999999.dat", day);
	
    if(strcmp(filename + strlen(filename) - 13, restStr) == 0)
    {
    	strcpy(tableName, getTableName(filename));
        printf("tablename=%s\n", tableName);
		printf("do load file:%s/%s\n", basePath, filename);
		snprintf(sql, sizeof(sql)-1, "load data local infile '%s/%s'  into table %s\n", basePath, filename, tableName);
		printf("sql=%s", sql);
		return 1;
    }
    return 0;
}

int readFileList(char *basePath)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];
    int day = 10;
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
            if(!doWithFile(basePath, ptr->d_name, day))
            {
                printf("ignore file:%s/%s\n", basePath, ptr->d_name);
            }
        }
        else if(ptr->d_type == 10)    ///link file
        {
            if(!doWithFile(basePath, ptr->d_name, day))
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
            readFileList(base);
        }
    }
    closedir(dir);
    return 1;
}

int main(void)
{
    DIR *dir;
    char basePath[1000];
    ///get the file list
    memset(basePath, '\0', sizeof(basePath));
    int iYear = 0, iMonth = 0, iDay = 0;
    iYear = 2015;
    iMonth = 10;
    iDay = 10;
    snprintf(basePath, sizeof(basePath) - 1, "/home/ncmysql/nw/keep/%04d%02d%02d", iYear, iMonth, iDay);
    readFileList(basePath);
    return 0;
}