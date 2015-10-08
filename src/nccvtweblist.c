/*  从文本文件中导入IP列表
    格式:
    #
    <内部工作地址>
    www.pronetway.com
    mail.pronetway.com
    <国外IP地址>
    
    202.109.72.29
    <上海IP地址>
    202.109.72.29

 */
#include <stdio.h>
#include <stdlib.h>
#include "utoall.h"
#include "pasdef.h"
#include "ncdef.h"
#include "pasdb.h"
typedef struct ncWebList_s {       /* 限制访问的Web地址  */
    unsigned long   lId;
    char     url[64];
} ncWebList;


typedef struct ncIpListIndex_s {       /* 限制访问的Ip地址  */
    unsigned long   lId;               /*                             */
    unsigned long   lStart;            /*  */
    unsigned long   lSum;              /*  */
} ncIpListIndex;


typedef struct ncWebListHead_s {       /* 限制访问的Ip地址  */
    ncWebList        *psWeb;
    ncIpListIndex    *psIndex;
    unsigned long    lSumWeb;
    unsigned long    lSumIndex;
} ncWebListHead;


int main(int argc,char **argv)
{
    char caUser[32];
    char caPass[32];
    char caDbname[32];
    char caFile[64];
    int argval,iReturn,i;
    unsigned long lId,lIp;
    char caIp[64];
    pasDbConn *pasDb;
    ncWebListHead sIpHead;
    strcpy(caFile,"weblist.txt");
    strcpy(caUser,"nc");
    strcpy(caPass,"nc");
    strcpy(caDbname,"nc");
    argv++;
    argc--;
    while(argc) {
        if(**argv != '-') {
            printf("ncicauth [-d configfile] \n");
            exit(0);
        }
        argval = *(*argv + 1);
        argc--;
        argv++;
        switch(argval) {
            case 'u':
                strcpy(caUser,(*argv));
                argc--;
                argv++;
                break;
            case 'p':
                strcpy(caPass,(*argv));
                argc--;
                argv++;
                break;
            case 'd':
                strcpy(caDbname,(*argv));
                argc--;
                argv++;
                break;
            case 'f':
                strcpy(caFile,(*argv));
                argc--;
                argv++;
                break;
            default:
                printf("nccvtiplist -u nc -p nc -d nc -o aa.log \n");
                exit(0);
        }
    }
    pasDb = pasDbConnect(caUser,caPass,caDbname);
    if(pasDb == NULL) {
        printf(" Database Connect Error\n");
    }
    iReturn = ncDbCvtWebList(caFile);
    ncLimitInitWebList(&sIpHead);
    ncLimitPrintWebList(&sIpHead);
    pasDbClose(NULL);
    printf(" Id==");
    scanf("%d",&lId);
    while(1) {
        printf(" Host==");
        scanf("%s",caIp);
        lIp = ntohl(utComHostAddress(caIp));
        for(i=0;i<1000000L;i++) {
           iReturn = ncLimitIsWebList(&sIpHead,lId,caIp);
        }
        printf(" lId=%lu Host=%s iReturn==%d\n",lId,caIp,iReturn);
    }
    return 0;
}



int ncDbCvtWebList(char *caFile)
{
    FILE *fp;
    char caBuffer[256];
    char *p;
    int iReturn;
    unsigned long lLip,lUip,lGid;
    char caGroupname[32],caTemp[64];
    fp = fopen(caFile,"r");
    if(fp == NULL) {
        printf(" File %s open error\n",caFile);
        return 0;
    }
    p = fgets(caBuffer,255,fp);
    while(p) {
        if(utStrIsSpaces(caBuffer) || caBuffer[0] == '#') { /* 注释  */
            p = fgets(caBuffer,255,fp);
            continue;
        }
        else {
            p = utStrSkipSpaces(caBuffer);
            if(*p == 0) {
                p = fgets(caBuffer,255,fp);
                continue;
            }
            if(*p == '<') {
                p = utStrGetWord(p+1,caGroupname,31,">");
                if(*p != '>') {
                    printf("Invalid Groupname \n");
                    p = fgets(caBuffer,255,fp);
                    continue;
                }
                iReturn = pasDbOneRecord("select id from ncwebindex where name = :name",1,
                        "name", UT_TYPE_STRING,caGroupname,
                        UT_TYPE_LONG,4, &lGid);
                if(iReturn != 0) {
                    lGid = utStrGetId();
                    iReturn = pasDbExecSqlF("insert into ncwebindex(id,name) values (%lu,'%s')",
                         lGid,caGroupname);
                    if(iReturn != 0) {
                        printf(" Insert ncipindx Error \n");
                    }
                }
            }
            else {
                p = utStrGetWord(p,caTemp,63," \n\r\t");
                if(!utStrIsSpaces(caTemp)) {
                    iReturn = pasDbExecSqlF("insert into ncweblist(sid,id,url) values (0,%lu,'%s')",
                         lGid,caTemp);
                    if(iReturn != 0) {
                        printf(" Insert nciplist Error \n");
                    }
                }
            }
        }
        p = fgets(caBuffer,255,fp);
    }
    fclose(fp);
    
}