/*  将网址库从表中导出来,并转换成新的类别
# 
#
 */
#include <stdio.h>
#include <stdlib.h>
#include "utoall.h"
#include "pasdef.h"
#include "ncdef.h"
#include "pasdb.h"

typedef struct ncClassList_s {  
    unsigned long   lOldId;
    unsigned long   lNewId;
} ncClassList;

typedef struct ncUrlList_s {
    unsigned long lSid;
    unsigned long lId1;
    unsigned long lId2;
    unsigned long lId3;
} ncUrlList;

typedef struct ncUrlClassName_s {
    unsigned long lId;
    char     caName[32];
} ncUrlClassName;

static ncUrlClassName psClassName[200];
static ncClassList    psClass[200];
static long iSumClass=0;
static long iSumClassName=0;
static char *ncGetClassName(long lId);

int main(int argc,char **argv)
{
    char caUser[32];
    char caPass[32];
    char caDbname[32];
    char caInfile[32];
    char caFile[64];
    char caOpt[16];
    int argval,iReturn,i;
    unsigned long lId,lIp;
    char caIp[64];
    pasDbConn *pasDb;
    strcpy(caUser,"nc");
    strcpy(caPass,"nc");
    strcpy(caDbname,"proeim_system");
    strcpy(caFile,"\0");
    strcpy(caOpt,"\0");
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
            case 'o':
                strcpy(caFile,(*argv));
                argc--;
                argv++;
                break;
            case 'f':
                strcpy(caInfile,(*argv));
                argc--;
                argv++;
                break;
            case 'a':
                strcpy(caOpt,(*argv));
                argc--;
                argv++;
                break;
            default:
                printf("ncexpweblist -a webclass -f infile -o outfile --- Cvt Text Class File to Sql\n");
                printf("ncexpweblist -a cvtv1tov2 -f classdictionary -o outfile                     \n");
                exit(0);
        }
    }
    
    if(strcasecmp(caOpt,"webclass")==0) { /* 将文本文件的网址库类别转换成sql格式  */
        /* ncexpweblist -a webclass -f infile -o outfile  */
        printf("Cvt Web Class....\n");
        ncUpdateWebClass(caInfile,caFile);
        exit(0);
    }
    pasDb = pasDbConnect(caUser,caPass,caDbname);
    if(pasDb == NULL) {
        printf(" Database Connect Error\n");
    }
    if(strcasecmp(caOpt,"imp")==0) {   /* 将网址库导出到文件  */
        iReturn = ncLoasClassName();
        iReturn =  ncImpUrlList(caFile);
    }
    if(strcasecmp(caOpt,"weblist")==0) {   /* 将网址库导出到文件  */
        iReturn = ncLoasClassName();
        iReturn =  ncImpWebList(caFile);
    }
    else if(strcasecmp(caOpt,"cvtv1tov2") == 0) { /* 将 v1的网址类别转换为v2 */
        printf("Cvt....\n");
        /* ncexpweblist -a cvtv1tov2 -f cvtclass -o outfile  */
        iReturn = ncCvtLoadClassFile(caInfile);
        if(iReturn <= 0) {
            printf(" Config File ncoldclass does not exist \n");
            return (-1);
        }
        ncCvtUrlListOld2New(caFile);
    }
    pasDbClose(NULL);
    return 0;
}

int ncCvtLoadClassFile(char *pFile)
{
    FILE *fp;
    char buf[256];
    char *p;
    char caId1[16],caId2[16];
    long lID1,lId2;
    int i,n;
    i = 0;
    fp = fopen(pFile,"r");
    if(fp == NULL) {
        printf("File Open Error \n",pFile);
        return (-1);
    }
    p = fgets(buf,256,fp);
    while(p) {
        p = utStrSkipSpaces(buf);
        if(*p != '#'  && !utStrIsSpaces(p)) {
            n = utStrGetSomeWordBySp(p,2,UT_TYPE_STRING,15,caId1,
                                         UT_TYPE_STRING,15,caId2);
            if(n==2 && utStrIsDigital(caId1) && utStrIsDigital(caId2)) {
                psClass[i].lOldId = atol(caId1);
                psClass[i].lNewId = atol(caId2);
    printf("[%d]  Old:%lu  New:%lu\n",i+1,psClass[i].lOldId,psClass[i].lNewId);
    
                i++;
            }
        }
        p = fgets(buf,256,fp);
    }
    iSumClass = i;
    printf("SumClacc = %lu \n",iSumClass);
    fclose(fp);
    return iSumClass;
}

int ncCvtOld2NewClass(long lId)
{
    int i;
    for(i=0;i<iSumClass;i++) {
        if(psClass[i].lOldId == lId) {
            return psClass[i].lNewId;
        }
    }
    return 0;
}




/* 转换网址库数据   */
int ncCvtUrlListOld2New(char *pOut)
{
    pasDbCursor *psCur;
    ncUrlList *ps;    
    long lCount;
    FILE *fp=NULL;
    fp = NULL;
    long lRecord,lSid,lId1,lId2,lId3;
    int iReturn,i;
    iReturn = pasDbOneRecord("select count(*) from unkownurl where url = 'proeimv2.pronetway.com'",
                                          0,UT_TYPE_LONG,4,&lCount);
    if(iReturn == 0 && lCount > 0) { /* 检查这个版本是否已转换过了  */
        printf(" Table unkownurl is already new version\n");
        return (-1);
    }
    lRecord = pasDbSumRecord("unkownurl");
    if(lRecord <= 0) {
        return 0;
    }
    ps = malloc(sizeof(ncUrlList) * lRecord);
    if(ps == NULL) {
        printf(" Memory Error \n");
        return 0;
    }
    psCur = pasDbOpenSqlF("select sid,id,rid,eid from unkownurl");
    if(psCur == NULL) {
        printf(" CurOpen Error \n");
        return (-1);
    }
    iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG,4,&lSid,
                                    UT_TYPE_ULONG,4,&lId1,
                                    UT_TYPE_ULONG,4,&lId2,
                                    UT_TYPE_ULONG,4,&lId3);
    i = 0;
    while(iReturn == 0) {
        ps[i].lSid = lSid;
        ps[i].lId1 = ncCvtOld2NewClass(lId1);
        ps[i].lId2 = ncCvtOld2NewClass(lId2);
        ps[i].lId3 = ncCvtOld2NewClass(lId3);
        if(ps[i].lId3 == 0) {
            if(ps[i].lId2 == 0) {
                ps[i].lId3 = ps[i].lId1;
            }
            else {
                ps[i].lId3 = ps[i].lId2;
            }
        }
        i++;
        iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG,4,&lSid,
                                    UT_TYPE_ULONG,4,&lId1,
                                    UT_TYPE_ULONG,4,&lId2,
                                    UT_TYPE_ULONG,4,&lId3);
    }
    pasDbCloseCursor(psCur);
    lRecord = i;
    /* 替换   */
    if(!utStrIsSpaces(pOut)) {
        fp = fopen(pOut,"w");
        iReturn = 0;
    }
    for(i=0;i<lRecord;i++) {
        if(fp) {
            fprintf(fp,"update unkownurl set id = %lu,rid=%lu,eid=%lu where sid = %lu;\n",
                    ps[i].lId1,ps[i].lId2,ps[i].lId3,ps[i].lSid);
        }
        else {
            iReturn = pasDbExecSqlF("update unkownurl set id = %lu,rid=%lu,eid=%lu where sid = %lu",
                  ps[i].lId1,ps[i].lId2,ps[i].lId3,ps[i].lSid);
        }
        if(iReturn != 0) {
            printf("Insert Error %s update unkownurl set id = %lu,rid=%lu,eid=%lu where sid = %lu\n",
                    pasDbErrorMsg(NULL),
                    ps[i].lId1,ps[i].lId2,ps[i].lId3,ps[i].lSid);
        }
    }
    if(fp == NULL) {
        iReturn = pasDbExecSqlF("insert into unkownurl (url,id,rid,eid,addtime,file,fname,ftime,sname,stime,title,rname,rtime,flan) value ('proeimv2.pronetway.com',9999,9999,9999,0,' ',' ',0,' ',0,' ',' ',0,2");
        pasDbCommit(NULL);
    }
    else {
      fclose(fp);
    }
    free(ps);
    printf(" Record Replace %lu\n",lRecord);
    return 0;
}

/* 将数据库中的记录倒出来  
#
# ClassName  新闻娱乐
  ClassId    1102
       www.pronetway.com   新浪网          新闻娱乐   新闻娱乐   新闻娱乐
       www.pronetway.com   新浪网          
       www.pronetway.com   新浪网          
       www.pronetway.com   新浪网          
         
 */

int ncImpUrlList(char *pFile)
{
    pasDbCursor *psCur;
    ncUrlList *ps;    
    FILE *fp;
    long lId,lId1,lId2,lId3,lFlags;
    int i,iReturn;
    char caUrl[64],caTitle[128];
    lId = (-1);
    fp = fopen(pFile,"w");
    if(fp == NULL) {
        printf("File %s Open Error \n",pFile);
        return (-1);
    }
    psCur = pasDbOpenSqlF("select id,rid,eid,url,title,flag  from unkownurl order by eid desc");
    if(psCur == NULL) {
        printf(" CurOpen Error %s \n",pasDbErrorMsg(NULL));
        return (-1);
    }
    iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG,4,&lId1,
                                    UT_TYPE_ULONG,4,&lId2,
                                    UT_TYPE_ULONG,4,&lId3,
                                    UT_TYPE_STRING,63,caUrl,
                                    UT_TYPE_STRING,127,caTitle,
                                    UT_TYPE_ULONG,4,&lFlags);
    i = 0;
    while(iReturn == 0 || iReturn == 1405) {
        if(lId != lId3) {
            fprintf(fp,"# ClassName:%s \n",ncGetClassName(lId3));
            fprintf(fp,"ClassId:%04d \n",lId3);
            lId = lId3;
        }
        fprintf(fp,"    %-31s %s    %d  %-15s  %-15s\n",
                caUrl,caTitle,lFlags,ncGetClassName(lId1),ncGetClassName(lId2));  
        iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG,4,&lId1,
                                    UT_TYPE_ULONG,4,&lId2,
                                    UT_TYPE_ULONG,4,&lId3,
                                    UT_TYPE_STRING,63,caUrl,
                                    UT_TYPE_STRING,127,caTitle,
                                    UT_TYPE_ULONG,4,&lFlags);
    }
    pasDbCloseCursor(psCur);
    fclose(fp);
    return 0;
}


int ncImpWebList(char *pFile)
{
    pasDbCursor *psCur;
    ncUrlList *ps;    
    FILE *fp;
    long lId,lId1,lId2,lId3,lFlags;
    int i,iReturn;
    char caUrl[64],caTitle[128];
    lId = (-1);
    fp = fopen(pFile,"w");
    if(fp == NULL) {
        printf("File %s Open Error \n",pFile);
        return (-1);
    }
    psCur = pasDbOpenSqlF("select id,url,name from ncweblist order by id desc");
    if(psCur == NULL) {
        printf(" CurOpen Error %s \n",pasDbErrorMsg(NULL));
        return (-1);
    }
    iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG,4,&lId1,
                                    UT_TYPE_STRING,63,caUrl,
                                    UT_TYPE_STRING,127,caTitle);
    i = 0;
    while(iReturn == 0 || iReturn == 1405) {
        if(lId != lId1) {
            fprintf(fp,"# ClassName:%s \n",ncGetClassName(lId1));
            fprintf(fp,"ClassId:%04d \n",lId1);
            lId = lId1;
        }
        fprintf(fp,"    %-31s %s   %-15s\n",
                caUrl,caTitle,ncGetClassName(lId1));  
    iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG,4,&lId1,
                                    UT_TYPE_STRING,63,caUrl,
                                    UT_TYPE_STRING,127,caTitle);
    }
    pasDbCloseCursor(psCur);
    fclose(fp);
    return 0;
}



/* 装载网址类别  */

int ncLoasClassName()
{
    pasDbCursor *psCur;
    long lId,lId1;
    int i,iReturn;
    char caName[64];
    psCur = pasDbOpenSqlF("select id,name from ncwebclass order by id");
    if(psCur == NULL) {
        printf(" CurOpen Error %s \n",pasDbErrorMsg(NULL));
        return (-1);
    }
    iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG,4,&lId1,
                                    UT_TYPE_STRING,63,caName);
    i = 0;
    while(iReturn == 0) {
        psClassName[i].lId = lId1;
        strcpy(psClassName[i].caName,caName);
        i++;
        iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG,4,&lId1,
                                    UT_TYPE_STRING,63,caName);
    }
    pasDbCloseCursor(psCur);
    iSumClassName = i;
    return iSumClassName;
}

static char *ncGetClassName(long lId)
{
    int i;
    static char caTemp[1];
    caTemp[1] = 0;
    for(i=0;i<iSumClassName;i++) {
        if(psClassName[i].lId == lId) {
            return psClassName[i].caName;
        }
    }
    return caTemp;
}


/* 更新网址类别  */
int ncUpdateWebClass(char *pFile,char *pOut)
{
    FILE *fp,*fpo;
    char buf[256];
    char *p;
    char caId[16],caName[64],caClass[64];
    int iSumClass,i,n,l;
    i = 0;
    fp = fopen(pFile,"r");
    if(fp == NULL) {
        printf("File Open Error \n",pFile);
        return (-1);
    }
    fpo = fopen(pOut,"w");
    if(fpo == NULL) {
        printf(" OutFile %s Open Error \n",pOut);
        return (-1);
    }
    fprintf(fpo,"delete from ncwebclass;\n");
    p = fgets(buf,256,fp);
    while(p) {
        p = utStrSkipSpaces(buf);
        if(*p != '#'  && !utStrIsSpaces(p)) {
            n = utStrGetSomeWordBySp(p,2,UT_TYPE_STRING,15,caId,
                                         UT_TYPE_STRING,63,caName);
            if(n==2 && utStrIsDigital(caId) && !utStrIsSpaces(caName)) {
                l = strlen(caName);
                for(i=l-1;i>0;i--) {
                    if(caName[i] == '.') {
                        strcpy(caClass,caName+i+1);
                        break;
                    }
                }
                if(i==0) {
                    strcpy(caClass,caName);
                }
                fprintf(fpo,"insert into ncwebclass(id,name,lcount,modtime,status,flags) values (%s,'%s',0,%lu,1,9);\n",
                        caId,caClass,time(0));
            }
        }
        p = fgets(buf,256,fp);
    }
    fclose(fp);
    fclose(fpo);
    return 0;
}    


