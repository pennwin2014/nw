/*  从文本文件中导入Web地址库
          2004/09/12
 */
#include <stdio.h>
#include <stdlib.h>
#include "utoall.h"
#include "pasdef.h"
#include "ncdef.h"
#include "pasdb.h"
long lMaxId=1;
int  iFlags = 9;

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
    strcpy(caFile,"weblist.txt");
    strcpy(caUser,"nc");
    strcpy(caPass,"nc");
    strcpy(caDbname,"nc");
    argv++;
    argc--;
    while(argc) {
        if(**argv != '-') {
            printf("nccvtweb [-d configfile] \n");
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
            case 'g':
                iFlags = atol(*argv);
                argc--;
                argv++;
                break;
            case 'm':
                lMaxId = atol(*argv);
                argc--;
                argv++;
                break;
            default:
                printf("nccvturl -u nc -p nc -d nc -f file -g flags \n");
                exit(0);
        }
    }
    pasDb = pasDbConnect(caUser,caPass,caDbname);
    if(pasDb == NULL) {
        printf(" Database Connect Error\n");
    }
    iReturn = ncDbCvtWebList(caFile);
    pasDbClose(NULL);
    return 0;
}



int ncDbCvtWebList(char *caFile)
{
    FILE *fp,*fpo;
    char caBuffer[256];
    char *p;
    long lCount=0,lMaxid;
    unsigned long lSid,lId;
    char caType1[64],caUrl[128],caName[64],caUrl1[128],caName1[128];
    int iReturn,i,n;
    int iCount=0;


    if(lMaxId == 0) {
        iReturn = pasDbOneRecord("select max(sid) from ncwebclass",0,
                            UT_TYPE_LONG,4,&lMaxid);
        if(iReturn != 0) {
            printf(" DataBase Error \n");
            return 0;
        }
    }
    lId = lMaxId;
    fpo = fopen("web.log","w");
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
            if(caBuffer[0] == '<') {
                p = utStrGetWord(caBuffer+1,caType1,63,".>");
                if(!utStrIsSpaces(caType1)) {
                    iReturn = pasDbOneRecord("select id from  ncwebclass where name=:name and flags=:flags",2,
                            "name",  UT_TYPE_STRING,utStrDelSpaces(caType1),
                            "flags", UT_TYPE_LONG, iFlags,
                            UT_TYPE_LONG,4,&lId);
                    if(iReturn != 0) {
                        lId++;
                        iReturn = pasDbExecSqlF("insert into ncwebclass(id,name,lcount,modtime,flags) \
                              values (%lu,'%s',%lu,%lu,%lu)",
                                          lId,caType1,0,time(0),iFlags);
                        if(iReturn != 0) {
                            printf(" Insert Table Error \n");
                        }
                        else {
                            printf(" Id==%lu  Name==[%s]  \n",lId,caType1);
                        }
                    }
                }
            }
            else {
                p = utStrSkipSpaces(caBuffer);
                if(*p) {
                    n = sscanf(p,"%s %s",caUrl1,caName1);
                    if(strncasecmp(caUrl1,"http://",7) == 0) {
                        p = p + 7;
                        p = utStrGetWord(p,caUrl,127,"/ ");
                    }
                    else if(strncasecmp(caUrl1,"ftp://",6) == 0) {
                        p = p + 6;
                        p = utStrGetWord(p,caUrl,127,"/ ");
                    }
                    else {
                        p = utStrGetWord(p,caUrl,127,"/ ");
                    }
                    if(n == 2) {
                        p = utStrGetWord(caName1,caName,127," ");
                    }
                    else {
                        strcpy(caName,"\0");
                    }
                    if(!utStrIsSpaces(caUrl)) {
                        iReturn = pasDbExecSqlF("insert into ncweblist(sid,id,url,name,modtime,flags)\
                                 values (0,%lu,'%s','%s',%lu,%d)",
                                        lId,caUrl,caName,time(0),iFlags);
                        if(iReturn != 0) {
                            fprintf(fpo,"Error:insert into ncweblist(sid,id,url,name,modtime,flags)\
                                 values (0,%lu,'%s','%s',%lu,%d)\n",
                                        lId,caUrl,caName,time(0),iFlags);
                        }
                    }
                }
            }
        } 
        p = fgets(caBuffer,255,fp);
    }
    fclose(fp);
    fclose(fpo);
    
}
