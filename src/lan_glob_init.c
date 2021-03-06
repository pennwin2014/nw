#define PAS_SRCFILE      2999
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include <stdarg.h>
#include "utoall.h"
#include "pasdb.h"
#include "pasutl.h"
#include "ncdef.h"
#include "nwdef.h"
#include <stdio.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "ftplib.h"
static unsigned long lLasttime_procy = 0;
static unsigned long lLasttime_base = 0;

int lanGetPbaseTime(utShmHead *psShmHead)
{
    char caTemp[16];
    strcpy(caTemp, utComGetVar_sd(psShmHead, "bptime", ""));
    return atol(caTemp);
}
int lanGetPTime(utShmHead *psShmHead)
{
    char caTemp[16];
    strcpy(caTemp, utComGetVar_sd(psShmHead, "ptime", ""));
    return atol(caTemp);
}
int lanUtlLoadCompyInfo(utShmHead *psShmHead)
{
    int iReturn;
    utShmFreeHash(psShmHead, NC_LNK_NW_COMPY);
    iReturn = utShmHashInit(psShmHead, NC_LNK_NW_COMPY, 256, 256, sizeof(lanCompyInfo), 0, 64);
    if(iReturn != 0)
    {
        return (-1);
    }
    return 0;
}
int lanUtlLoadGroupInfo(utShmHead *psShmHead)
{

    unsigned long   lMaxGroup;
    int             iReturn, iRet;
    char            *p;
    unsigned long lGroupid, lBegip, lEndip, lPid;

    pasDbCursor *psCur;
    lanGroupInfo s;
    lMaxGroup = 500;

    //    utShmFreeHash(psShmHead,NC_LNK_NW_GROUP);
    //   iReturn = utShmHashInit(psShmHead,NC_LNK_NW_GROUP,
    //                lMaxGroup,lMaxGroup,sizeof(lanGroupInfo),0,4);
    //   if(iReturn < 0) {
    //        ncSysLog(NC_LOG_ERROR, "Share memory Error, nwUtlLoadGroupInfo");
    //        return (-1);
    //   }

    utShmFreeHash(psShmHead, NC_LNK_NW_GROUP); /* 先释放  */

    iReturn = utShmHashInit(psShmHead, NC_LNK_NW_GROUP, lMaxGroup, lMaxGroup, sizeof(lanGroupInfo), 0, 4);
    if(iReturn != 0)
    {
        ncSysLog(NC_LOG_ERROR, "Share Memory alloc Error at nwUtlLoadUserInfo");
        return (-1);
    }
    /* 将用户资料导入到内存中  */
    psCur = pasDbOpenSqlF("select gid,gname,begip,endip,pid from nwgroup");
    if(psCur == NULL)
    {
        ncSysLog(NC_LOG_ERROR, "Cursor open error %s at nwUtlLoadUserInfo", pasDbErrorMsg(NULL));
        return (-1);
    }

    memset(&s, 0, sizeof(s));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &s.lGroupid,
                             UT_TYPE_STRING, 31, s.caGroupname,
                             UT_TYPE_ULONG, 4, &s.lBegip,
                             UT_TYPE_ULONG, 4, &s.lEndip,
                             UT_TYPE_ULONG, 4, &s.lPid);

    while(iReturn == 0 || iReturn == 1405)
    {
        // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);

        iRet = utShmHashAdd(psShmHead, NC_LNK_NW_GROUP, &s, 1);
        if(iRet < 0)
        {
            ncSysLog(NC_LOG_ERROR, "Compute Add Error");
            pasDbCloseCursor(psCur);
            return (-1);
        }
        memset(&s, 0, sizeof(s));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &s.lGroupid,
                                 UT_TYPE_STRING, 31, s.caGroupname,
                                 UT_TYPE_ULONG, 4, &s.lBegip,
                                 UT_TYPE_ULONG, 4, &s.lEndip,
                                 UT_TYPE_ULONG, 4, &s.lPid);
    }
    pasDbCloseCursor(psCur);
    return 0;
}

char *lanGetGroup(utShmHead *psShmHead, unsigned long lGroupid)
{
    lanGroupInfo *psGroup;
    static char sGroup[36];
    psGroup = (lanGroupInfo *)utShmHashLook(psShmHead, NC_LNK_NW_GROUP, &lGroupid);
    if(psGroup)
    {
        strcpy(sGroup, psGroup->caGroupname);
    }
    else
    {
        strcpy(sGroup, "");
    }
    return &sGroup[0];
}

unsigned long lanGetGroupPid(utShmHead *psShmHead, unsigned long lGroupid)
{
    lanGroupInfo *psGroup;
    unsigned long lPid;
    psGroup = (lanGroupInfo *)utShmHashLook(psShmHead, NC_LNK_NW_GROUP, &lGroupid);
    if(psGroup)
    {
        lPid = psGroup->lPid;
    }
    else
    {
        lPid = 0;
    }
    return lPid;
}
/*
char *lanGetProcyFileByCompid(utShmHead *psShmHead,unsigned long lCompid,char *pFile,char *shortname){
     static char path[250];
     nwCompInfo *psComp;
     unsigned long lGroupid,lPid;
     char caTemp[1025];
     long iNum=0;
     memset(path,0,sizeof(path));
     sprintf(caTemp,"/home/ncmysql/nw/baseprocy/C%lu/%s",lCompid,pFile);
     if(utFileIsExist(caTemp)){
        strcpy(path,caTemp);
        return &path[0];
     }
     else{
         psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lCompid);
         if(psComp){
             sprintf(caTemp,"/home/ncmysql/nw/baseprocy/G%lu/%s",psComp->groupid,pFile);
             if(utFileIsExist(caTemp)){
                strcpy(path,caTemp);
                return &path[0];
             }


            lPid=lanGetGroupPid(psShmHead,psComp->groupid);
            while(lPid>0){
                 iNum++;
                 if(iNum>5) break;

                 sprintf(caTemp,"/home/ncmysql/nw/baseprocy/G%lu/%s",lPid,pFile);
                 if(utFileIsExist(caTemp)){
                    strcpy(path,caTemp);
                    return &path[0];
                 }


                 lGroupid=lPid;
                 lPid=lanGetGroupPid(psShmHead,psComp->groupid);
            }
         }
     }
      if(strlen(path)==0){

           sprintf(caTemp,"/home/ncmysql/nw/baseprocy/%s/%s",shortname,pFile);
                 if(utFileIsExist(caTemp)){
                    strcpy(path,caTemp);
                    return &path[0];
                 }
      }

    return &path[0];

}

*/

char *lanGetProcyFileByCompid(utShmHead *psShmHead, unsigned long lCompid, char *pFile, char *shortname)
{
    static char path[250];
    nwCompInfo *psComp;
    unsigned long lGroupid, lPid;
    char caTemp[1025];
    long iLen;
    FILE *fp2, *fp3;
    char *p;
    char caBuf[4096];
    long iNum = 0;
    memset(path, 0, sizeof(path));
    sprintf(path, "/home/ncmysql/nw/baseprocy/%lu", lCompid);
    if(!utFileIsExist(path))
    {
        char tmp[64] = "";
        sprintf(tmp, "mkdir -p %s", path);
        system(tmp);
    }
    sprintf(path, "/home/ncmysql/nw/baseprocy/%lu/%s", lCompid, pFile);
    fp2 = fopen(path, "w");
    iNum = 0;
    if(fp2)
    {
        sprintf(caTemp, "/home/ncmysql/nw/baseprocy/C%lu/%s", lCompid, pFile);
        if(utFileIsExist(caTemp))
        {

            fp3 = fopen(caTemp, "r");
            if(fp3)
            {
                memset(caBuf, 0, sizeof(caBuf));
                memset(caBuf, 0, sizeof(caBuf));
                p = fgets(caBuf, 500, fp3);
                while(p)
                {
                    if(strstr(p, "[law"))
                    {
                        fprintf(fp2, "[law%d]\r\n", iNum);
                        iNum++;
                    }
                    else
                    {
                        fprintf(fp2, "%s", caBuf);
                    }
                    memset(caBuf, 0, sizeof(caBuf));
                    p = fgets(caBuf, 500, fp3);
                }
                fclose(fp3);
            }

        }
        psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &lCompid);
        if(psComp)
        {
            sprintf(caTemp, "/home/ncmysql/nw/baseprocy/G%lu/%s", psComp->groupid, pFile);
            if(utFileIsExist(caTemp))
            {
                fp3 = fopen(caTemp, "r");
                if(fp3)
                {
                    memset(caBuf, 0, sizeof(caBuf));
                    p = fgets(caBuf, 500, fp3);
                    while(p)
                    {
                        if(strstr(p, "[law"))
                        {
                            fprintf(fp2, "[law%d]\r\n", iNum);
                            iNum++;
                        }
                        else
                        {
                            fprintf(fp2, "%s", caBuf);
                        }
                        memset(caBuf, 0, sizeof(caBuf));
                        p = fgets(caBuf, 500, fp3);
                    }

                    fclose(fp3);
                }
            }

            lPid = lanGetGroupPid(psShmHead, psComp->groupid);

            while(lPid > 0)
            {
                if(iNum > 5) break;
                sprintf(caTemp, "/home/ncmysql/nw/baseprocy/G%lu/%s", lPid, pFile);
                if(utFileIsExist(caTemp))
                {
                    fp3 = fopen(caTemp, "r");
                    if(fp3)
                    {
                        memset(caBuf, 0, sizeof(caBuf));
                        p = fgets(caBuf, 500, fp3);
                        while(p)
                        {
                            if(strstr(p, "[law"))
                            {
                                fprintf(fp2, "[law%d]\r\n", iNum);
                                iNum++;
                            }
                            else
                            {
                                fprintf(fp2, "%s", caBuf);
                            }
                            memset(caBuf, 0, sizeof(caBuf));
                            p = fgets(caBuf, 500, fp3);
                        }
                        fclose(fp3);
                    }
                }


                lGroupid = lPid;
                lPid = lanGetGroupPid(psShmHead, lGroupid);
            }
        }



        sprintf(caTemp, "/home/ncmysql/nw/baseprocy/%s/%s", shortname, pFile);
        if(utFileIsExist(caTemp))
        {
            fp3 = fopen(caTemp, "r");
            if(fp3)
            {
                memset(caBuf, 0, sizeof(caBuf));
                iLen = fread(caBuf, sizeof(char), 1460, fp3);
                if(iLen > 0)
                {
                    fprintf(fp2, "[law%d]\r\n", iNum);
                    fprintf(fp2, "%s", caBuf);
                    iNum++;
                }
                fclose(fp3);


            }
        }

        fclose(fp2);
    }
    return &path[0];

}



long lanGetFileTime(char *pFile)
{
    struct stat st;
    char buf[64];
    //  printf("pFile=%s\n",pFile);
    if(stat(pFile, &st) == 0 && S_ISREG(st.st_mode))
    {
        return st.st_mtime;
    }

    return 0;
}

//装载软件下发策略
int lanUtlLoadSoftUpdate(utShmHead *psShmHead)
{

    unsigned long   lMaxGroup;
    int             iReturn, iRet;
    char            *p;
    unsigned long lVflag;
    unsigned long lObjtype;
    unsigned long lObjid;
    unsigned long lSoftid;
    unsigned long lFlags;
    char caFilename[132];
    char caMd5[36];
    char caVersion[16];
    struct key_s
    {
        unsigned long lVflag;
        unsigned long lObjtype;
        unsigned long lObjid;
    };
    struct key_s stKey;

    pasDbCursor *psCur;
    lanSoftUpdate *pData;
    lMaxGroup = 5000;

    utShmFreeHash(psShmHead, NC_LNK_NW_SOFTUP);
    iReturn = utShmHashInit(psShmHead, NC_LNK_NW_SOFTUP,
                            lMaxGroup, lMaxGroup, sizeof(lanGroupInfo), 0, 12);
    if(iReturn < 0)
    {
        ncSysLog(NC_LOG_ERROR, "Share memory Error, lanUtlLoadSoftUpdate");
        return (-1);
    }

    /* 将软件升级资料导入到内存中  */
    psCur = pasDbOpenSqlF("select softid,objid,objtype,filename,version,flags,md5 from ncupsoftprocy,ncupsoftlist where softid=ncupsoftlist.sid and status=0 order by softid ");
    if(psCur == NULL)
    {
        ncSysLog(NC_LOG_ERROR, "Cursor open error %s at nwUtlLoadUserInfo", pasDbErrorMsg(NULL));
        return (-1);
    }

    lSoftid = 0;
    lObjid = 0;
    lObjtype = 0;
    memset(caFilename, 0, sizeof(caFilename));
    memset(caVersion, 0, sizeof(caVersion));
    lFlags = 0;
    memset(caMd5, 0, sizeof(caMd5));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lSoftid,
                             UT_TYPE_ULONG, 4, &lObjid,
                             UT_TYPE_ULONG, 4, &lObjtype,
                             UT_TYPE_STRING, 127, caFilename,
                             UT_TYPE_STRING, 15, caVersion,
                             UT_TYPE_ULONG, 4, &lFlags,
                             UT_TYPE_STRING, 32, caMd5);


    while(iReturn == 0 || iReturn == 1405)
    {
        // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);

        memset(&stKey, 0, sizeof(struct key_s));
        stKey.lVflag = lFlags;
        stKey.lObjtype = lObjtype;
        stKey.lObjid = lObjid;
        pData = (lanSoftUpdate*)utShmHashLookA(psShmHead, NC_LNK_NW_SOFTUP, &stKey);
        if(pData)
        {

            //writeLog("softDown.txt","here","in pData");

            if(lSoftid > pData->lSoftid)
            {
                pData->lSoftid = lSoftid;
                strcpy(pData->caFilename, caFilename);
                strcpy(pData->caMd5, caMd5);
                strcpy(pData->caVersion, caVersion);

            }
        }

        lSoftid = 0;
        lObjid = 0;
        lObjtype = 0;
        memset(caFilename, 0, sizeof(caFilename));
        memset(caVersion, 0, sizeof(caVersion));
        lFlags = 0;
        memset(caMd5, 0, sizeof(caMd5));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lSoftid,
                                 UT_TYPE_ULONG, 4, &lObjid,
                                 UT_TYPE_ULONG, 4, &lObjtype,
                                 UT_TYPE_STRING, 127, caFilename,
                                 UT_TYPE_STRING, 15, caVersion,
                                 UT_TYPE_ULONG, 4, &lFlags,
                                 UT_TYPE_STRING, 32, caMd5);
    }
    pasDbCloseCursor(psCur);

    return utShmHashRecord(psShmHead, NC_LNK_NW_SOFTUP);

}

//装载内网策略
int lanUtlLoadProcy(utShmHead *psShmHead)
{

    unsigned long   lMaxRecord;
    int             iReturn, iRet;
    lanPmain        *pData;
    lanProcyObj        *pData2;
    unsigned long lId, lDataid;
    long iIndex;
    pasDbCursor *psCur;
    unsigned long lSid, lConstatus;
    unsigned long lPid, lOptype, lSnap, lType, lModtime;
    char caFilename[128];
    char caLawname[32], caPname[64];
    char caTemp[1024];
    long lCount, index, lFreehaordid, lGtype, lStamp;
    char caUrl[64];
    long lUrlclass, lCtype, lFtime;
    long lTrantype, lSport, lEport;
    long lOthid, iNum;
    long lLtype, lLower, lLupper, lLtime, lUtime, lFlag;
    FILE *fp;
    char caSclass[64], caSname[64];
    char caHardid[128];
    struct key_s
    {
        unsigned long ctype;
        unsigned long id;
    };
    struct key_s stKey;
    struct key_b
    {
        unsigned long groupid;
        unsigned long computeid;
    };
    struct key_b    stKeyB;
    char caPrecompany[128];

    //装载计算机策略最后时间
    long lMaxUser = utComGetVar_ld(psShmHead, "MaxRegDev", 1500);
    utShmFreeHash(psShmHead, NC_LNK_NW_PLAST); /* 先释放  */
    iReturn = utShmHashInit(psShmHead, NC_LNK_NW_PLAST,
                            lMaxUser, lMaxUser, sizeof(lanProcyLast), 0, 8);
    if(iReturn != 0)
    {
        ncSysLog(NC_LOG_ERROR, "Share Memory alloc Error at nwUtlLoadUserInfo");
        return (-1);
    }


    lMaxRecord = utComGetVar_ld(psShmHead, "MaxLanProcy", 96L);
    // printf("md5=%s\n",getFilemd5("/home/ncmysql/nw/baseprocy/pronetway/Basetypeinfo.txt"));

    //  lanUtlFreeProcy(psShmHead);
    utShmFreeHash(psShmHead, NC_LNK_NW_PROCY);

    iReturn = utShmHashInit(psShmHead, NC_LNK_NW_PROCY,
                            lMaxRecord, lMaxRecord, sizeof(lanPmain), 0, 4);
    if(iReturn < 0)
    {
        //     printf("ddddddddddddddddd\n");
        ncSysLog(NC_LOG_ERROR, "Share memory Error, lanUtlLoadProcy");
        return (-1);
    }

    lMaxRecord = 100;

    //  lanUtlFreeProcy(psShmHead);
    utShmFreeHash(psShmHead, NC_LNK_NW_BLACKLIST);

    iReturn = utShmHashInit(psShmHead, NC_LNK_NW_BLACKLIST,
                            lMaxRecord, lMaxRecord, sizeof(lanBlackList), 0, 8);

    if(iReturn < 0)
    {
        //     printf("ddddddddddddddddd\n");
        ncSysLog(NC_LOG_ERROR, "Share memory Error, lanUtlLoadProcy");
        return (-1);
    }
    //printf("eeeeeeeeeeeeeeeee\n");
    /* 主策略导入到内存中  */
    psCur = pasDbOpenSqlF("select sid,lawname,constatus,modtime,flag from nwmainprocy  ");
    if(psCur == NULL)
    {
        ncSysLog(NC_LOG_ERROR, "Cursor open error %s at lanUtlLoadProcy", pasDbErrorMsg(NULL));
        return (-1);
    }

    lSid = 0;
    lConstatus = 0;
    lModtime = 0;
    lFlag = 0;
    memset(caLawname, 0, sizeof(caLawname));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lSid,
                             UT_TYPE_STRING, 23, caLawname,
                             UT_TYPE_ULONG, 4, &lConstatus,
                             UT_TYPE_ULONG, 4, &lModtime,
                             UT_TYPE_ULONG, 4, &lFlag);



    while(iReturn == 0 || iReturn == 1405)
    {
        if(lModtime > lLasttime_procy)
        {
            lLasttime_procy = lModtime;
        }
        if(lFlag == 0)
        {
            pData = (lanPmain*)utShmHashLookA(psShmHead, NC_LNK_NW_PROCY, &lSid);
            if(pData)
            {
                strcpy(pData->lawname, caLawname);
                pData->constatus = lConstatus;
                pData->sumfile = 0;
                pData->sumdev = 0;
                pData->sumlink = 0;
                pData->sumptype = 0;
                pData->sumpname = 0;
                pData->sumnet = 0;
                pData->sumurl = 0;
                pData->sumoth = 0;
                pData->sumref = 0;
            }
        }
        lSid = 0;
        lConstatus = 0;
        lModtime = 0;
        memset(caLawname, 0, sizeof(caLawname));
        lFlag = 0;
        iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lSid,
                                 UT_TYPE_STRING, 23, caLawname,
                                 UT_TYPE_ULONG, 4, &lConstatus,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lFlag);
    }
    pasDbCloseCursor(psCur);

    //加载黑白名单

    sprintf(caTemp, "select count(*),max(updatetime) from blacklistprocy");
    lCount = 0;
    lModtime = 0;
    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount,
                   UT_TYPE_LONG, 4, &lModtime);
    if(lModtime > lLasttime_procy)
    {
        lLasttime_procy = lModtime;
    }

    if(lCount > 0)
    {

        psCur = pasDbOpenSqlF("select blacktype,blackname,groupid,computeid,companyid,snap from blacklistprocy");
        if(psCur)
        {
            char blacktype[24] = "";
            char blackname[24] = "";
            unsigned long groupid = 0;
            unsigned long computeid = 0;
            char companyname[24] = "";
            char snap[2];
            lanBlackList *bData = NULL;

            printf("查数据完成!\n");

            while((iReturn = pasDbFetchInto(psCur,
                                            UT_TYPE_STRING, sizeof(blacktype), blacktype,
                                            UT_TYPE_STRING, sizeof(blackname), blackname,
                                            UT_TYPE_ULONG, sizeof(groupid), &groupid,
                                            UT_TYPE_ULONG, sizeof(computeid), &computeid,
                                            UT_TYPE_STRING, sizeof(companyname), companyname,
                                            UT_TYPE_STRING, sizeof(snap), snap)) == 0 || iReturn == 1405)
            {
                // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);

                stKeyB.groupid = groupid;
                stKeyB.computeid = computeid;
                printf("%s %s %lu %lu ", blacktype, blackname, groupid, computeid);
                bData = (lanBlackList*)utShmHashLookA(psShmHead, NC_LNK_NW_BLACKLIST, &stKeyB);
                if(bData)
                {
                    strcpy(bData->blacktype, blacktype);
                    strcpy(bData->blackname, blackname);
                    bData->groupid = groupid;
                    bData->computeid = computeid;
                    strcpy(bData->companyname, companyname);
                    strcpy(bData->snap, snap);
                }
                else
                {
                    printf("存数据不成功!\n");
                }

            }

            pasDbCloseCursor(psCur);
        }
    }



    //加载文件策略
    //sprintf(caTemp,"select count(*),max(nwprocyfile.modtime) from nwprocyfile,nwmainprocy where nwmainprocy.sid=nwprocyfile.pid ");
    //  printf("caTemp=%s\n",caTemp);
    sprintf(caTemp, "select count(*),max(updatetime) from nwfileprocy");
    lCount = 0;
    lModtime = 0;
    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount,
                   UT_TYPE_LONG, 4, &lModtime);
    if(lModtime > lLasttime_procy)
    {
        lLasttime_procy = lModtime;
    }

    /*  if(lCount>0){

        psCur = pasDbOpenSqlF("select pid,filename,optype,snap,type,max(nwprocyfile.modtime) from nwprocyfile,nwmainprocy where nwmainprocy.sid=nwprocyfile.pid and nwprocyfile.flag=0 and nwmainprocy.flag=0 group by pid,filename,optype,snap,type  ");
        if(psCur) {
            lPid=0;
            memset(caFilename,0,sizeof(caFilename));
            lOptype=0;
            lSnap=0;
            lType=0;
            lModtime=0;
                iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                               UT_TYPE_STRING,127,caFilename,
                                               UT_TYPE_ULONG,4,&lOptype,
                                               UT_TYPE_ULONG,4,&lSnap,
                                               UT_TYPE_ULONG,4,&lType,
                                               UT_TYPE_ULONG,4,&lModtime);



                while(iReturn == 0||iReturn==1405) {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);


                    pData=(lanPmain*)utShmHashLook(psShmHead,NC_LNK_NW_PROCY,&lPid);
                    if(pData){
                      if(pData->sumfile==0){
         //                 pData->psFile=(lanPfile*)malloc((lCount+1)*sizeof(lanPfile));
                      }
                      index=pData->sumfile;
                       pData->psFile[index].pid=lPid;
                      strcpy(pData->psFile[index].filename,caFilename);
       //               printf("i=%d,filename=%s\n",index,pData->psFile[index].filename);
                      pData->psFile[index].optype=lOptype;
                      pData->psFile[index].snap=lSnap;
                      pData->psFile[index].type=lType;
     //                 printf("caFile=%s\n",pData->psFile[pData->sumfile].filename);
                      pData->sumfile++;
                    }

                    lPid=0;
                    memset(caFilename,0,sizeof(caFilename));
                    lOptype=0;
                    lSnap=0;
                    lType=0;
                    lModtime=0;
                        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                                       UT_TYPE_STRING,127,caFilename,
                                                       UT_TYPE_ULONG,4,&lOptype,
                                                       UT_TYPE_ULONG,4,&lSnap,
                                                       UT_TYPE_ULONG,4,&lType,
                                                       UT_TYPE_ULONG,4,&lModtime);
            }
            pasDbCloseCursor(psCur);
        }
      }
    */
    //加载外设控制策略
    //   sprintf(caTemp,"select count(*),max(nwprocydev.modtime) from nwprocydev,nwmainprocy where nwmainprocy.sid=nwprocydev.pid ");
    lCount = 0;
    lModtime = 0;

    sprintf(caTemp, "select count(*),max(updatetime) from nwdeviceprocy");

    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount,
                   UT_TYPE_LONG, 4, &lModtime);
    if(lModtime > lLasttime_procy)
    {
        lLasttime_procy = lModtime;
    }
    /*  if(lCount>0){

        psCur = pasDbOpenSqlF("select pid,freehaordid,optype,snap,type,max(nwprocydev.modtime) from nwprocydev,nwmainprocy where nwmainprocy.sid=nwprocydev.pid and nwmainprocy.flag=0 and nwprocydev.flag=0 group by pid,freehaordid,optype,snap,type ");
        if(psCur) {
            lPid=0;
            lFreehaordid=0;
            lOptype=0;
            lSnap=0;
            lType=0;
            lModtime=0;
                iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                               UT_TYPE_ULONG,4,&lFreehaordid,
                                               UT_TYPE_ULONG,4,&lOptype,
                                               UT_TYPE_ULONG,4,&lSnap,
                                               UT_TYPE_ULONG,4,&lType,
                                               UT_TYPE_ULONG,4,&lModtime);



                while(iReturn == 0||iReturn==1405) {


                    pData=(lanPmain*)utShmHashLook(psShmHead,NC_LNK_NW_PROCY,&lPid);
                    if(pData){

                      if(pData->sumdev==0){
     //                     pData->psDev=(lanPdev*)malloc((lCount+1)*sizeof(lanPdev));
                     }

                      index=pData->sumdev;
                       pData->psDev[index].pid=lPid;
                      pData->psDev[index].freehaordid=lFreehaordid;
                     pData->psDev[index].optype=lOptype;
                     pData->psDev[index].snap=lSnap;
                     pData->psDev[index].type=lType;
      //              printf("freeid=%d,index=%d\n",pData->psDev[index].freehaordid,index);
                      pData->sumdev++;
                    }

                lPid=0;
                lFreehaordid=0;
                lOptype=0;
                lSnap=0;
                lType=0;
                lModtime=0;
                    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                                   UT_TYPE_ULONG,4,&lFreehaordid,
                                                   UT_TYPE_ULONG,4,&lOptype,
                                                   UT_TYPE_ULONG,4,&lSnap,
                                                   UT_TYPE_ULONG,4,&lType,
                                                   UT_TYPE_ULONG,4,&lModtime);
            }
            pasDbCloseCursor(psCur);
        }

       }
    */

    //加载非法外联控制策略
    //  sprintf(caTemp,"select count(*),max(nwprocylink.modtime) from nwprocylink,nwmainprocy where nwmainprocy.sid=nwprocylink.pid ");
    lCount = 0;
    lModtime = 0;

    sprintf(caTemp, "select count(*),max(updatetime) from nwoutlinkprocy");

    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount,
                   UT_TYPE_LONG, 4, &lModtime);
    if(lModtime > lLasttime_procy)
    {
        lLasttime_procy = lModtime;
    }
    /*  if(lCount>0){

        psCur = pasDbOpenSqlF("select pid,optype,snap,type,max(nwprocylink.modtime) from nwprocylink,nwmainprocy where nwmainprocy.sid=nwprocylink.pid and nwmainprocy.flag=0 and nwprocylink.flag=0 group by pid,optype,snap,type ");
        if(psCur) {
            lPid=0;
            lFreehaordid=0;
            lOptype=0;
            lSnap=0;
            lType=0;
            lModtime=0;
                iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                               UT_TYPE_ULONG,4,&lOptype,
                                               UT_TYPE_ULONG,4,&lSnap,
                                               UT_TYPE_ULONG,4,&lType,
                                               UT_TYPE_ULONG,4,&lModtime);



                while(iReturn == 0||iReturn==1405) {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);

                    pData=(lanPmain*)utShmHashLook(psShmHead,NC_LNK_NW_PROCY,&lPid);
                    if(pData){
                      if(pData->sumlink==0){
    //                      pData->psLink=(lanPlink*)malloc((lCount+1)*sizeof(lanPlink));
                      }
                      index=pData->sumlink;
                      pData->psLink[index].pid=lPid;
                      pData->psLink[index].optype=lOptype;
                      pData->psLink[index].snap=lSnap;
                      pData->psLink[index].type=lType;
     //                 printf("caFile=%s\n",pData->psFile[pData->sumfile].filename);
                      pData->sumlink++;
                    }

                lPid=0;
                lFreehaordid=0;
                lOptype=0;
                lSnap=0;
                lType=0;
                lModtime=0;
                    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                                   UT_TYPE_ULONG,4,&lOptype,
                                                   UT_TYPE_ULONG,4,&lSnap,
                                                   UT_TYPE_ULONG,4,&lType,
                                                   UT_TYPE_ULONG,4,&lModtime);
            }
            pasDbCloseCursor(psCur);
        }
       }
    */

    //加载程序类别控制策略
    //  sprintf(caTemp,"select count(*),max(nwprocypgtype.modtime) from nwprocypgtype,nwmainprocy where nwmainprocy.sid=nwprocypgtype.pid ");
    lCount = 0;
    lModtime = 0;
    sprintf(caTemp, "select count(*),max(updatetime) from nwappprocy");
    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount,
                   UT_TYPE_LONG, 4, &lModtime);
    if(lModtime > lLasttime_procy)
    {
        lLasttime_procy = lModtime;
    }
    /*  if(lCount>0){

        psCur = pasDbOpenSqlF("select pid,gtype,optype,snap,type,stamp,max(nwprocypgtype.modtime) from nwprocypgtype,nwmainprocy where nwmainprocy.sid=nwprocypgtype.pid and nwmainprocy.flag=0 and nwprocypgtype.flag=0 group by pid,gtype,optype,snap,type,stamp ");
        if(psCur) {
            lPid=0;
            lGtype=0;
            lOptype=0;
            lSnap=0;
            lType=0;
            lStamp=0;
            lModtime=0;
                iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                               UT_TYPE_ULONG,4,&lGtype,
                                               UT_TYPE_ULONG,4,&lOptype,
                                               UT_TYPE_ULONG,4,&lSnap,
                                               UT_TYPE_ULONG,4,&lType,
                                               UT_TYPE_ULONG,4,&lStamp,
                                               UT_TYPE_ULONG,4,&lModtime);



                while(iReturn == 0||iReturn==1405) {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);


                    pData=(lanPmain*)utShmHashLook(psShmHead,NC_LNK_NW_PROCY,&lPid);
                    if(pData){
                      if(pData->sumptype==0){
     //                     pData->psPtype=(lanPptype*)malloc((lCount+1)*sizeof(lanPptype));
                      }
                      index=pData->sumptype;
                      pData->psPtype[index].pid=lPid;
                      pData->psPtype[index].gtype=lGtype;
                      pData->psPtype[index].optype=lOptype;
                      pData->psPtype[index].snap=lSnap;
                      pData->psPtype[index].type=lType;
                      pData->psPtype[index].stamp=lStamp;
     //                 printf("caFile=%s\n",pData->psFile[pData->sumfile].filename);
                      pData->sumptype++;
                    }

                    lPid=0;
                    lGtype=0;
                    lOptype=0;
                    lSnap=0;
                    lType=0;
                    lStamp=0;
                    lModtime=0;
                        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                                       UT_TYPE_ULONG,4,&lGtype,
                                                       UT_TYPE_ULONG,4,&lOptype,
                                                       UT_TYPE_ULONG,4,&lSnap,
                                                       UT_TYPE_ULONG,4,&lType,
                                                       UT_TYPE_ULONG,4,&lStamp,
                                                       UT_TYPE_ULONG,4,&lModtime);
            }
            pasDbCloseCursor(psCur);
        }

       }
    */
    //加载程序控制策略
    /*  sprintf(caTemp,"select count(*),max(nwprocypname.modtime) from nwprocypname,nwmainprocy where nwmainprocy.sid=nwprocypname.pid ");
      lCount=0;
      lModtime=0;
      sprintf(caTemp,"select count(*),max(updatetime) from nwlinkprocy");
      pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount,
                              UT_TYPE_LONG,4,&lModtime);
      if(lModtime>lLasttime_procy){
              lLasttime_procy=lModtime;
        }
      if(lCount>0){

        psCur = pasDbOpenSqlF("select pid,pname,optype,snap,type,stamp,max(nwprocypname.modtime) from nwprocypname,nwmainprocy where nwmainprocy.sid=nwprocypname.pid and nwmainprocy.flag=0 and nwprocypname.flag=0 group by pid,pname,optype,snap,type,stamp ");
        if(psCur) {
            lPid=0;
            memset(caPname,0,sizeof(caPname));
            lOptype=0;
            lSnap=0;
            lType=0;
            lStamp=0;
            lModtime=0;
                iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                               UT_TYPE_STRING,63,caPname,
                                               UT_TYPE_ULONG,4,&lOptype,
                                               UT_TYPE_ULONG,4,&lSnap,
                                               UT_TYPE_ULONG,4,&lType,
                                               UT_TYPE_ULONG,4,&lStamp,
                                               UT_TYPE_ULONG,4,&lModtime);



                while(iReturn == 0||iReturn==1405) {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);

                    if(lModtime>lLasttime_procy){
                        lLasttime_procy=lModtime;
                    }
                    pData=(lanPmain*)utShmHashLook(psShmHead,NC_LNK_NW_PROCY,&lPid);
                    if(pData){
                      if(pData->sumpname==0){
     //                     pData->psPname=(lanPpname*)malloc((lCount+1)*sizeof(lanPpname));
                      }
                      index=pData->sumpname;
                      pData->psPname[index].pid=lPid;
                      strcpy(pData->psPname[index].pname,caPname);
                      pData->psPname[index].optype=lOptype;
                      pData->psPname[index].snap=lSnap;
                      pData->psPname[index].type=lType;
                      pData->psPname[index].stamp=lStamp;
     //                 printf("caFile=%s\n",pData->psFile[pData->sumfile].filename);
                      pData->sumpname++;
                    }

                    lPid=0;
                    memset(caPname,0,sizeof(caPname));
                    lOptype=0;
                    lSnap=0;
                    lType=0;
                    lStamp=0;
                    lModtime=0;
                        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                                       UT_TYPE_STRING,63,caPname,
                                                       UT_TYPE_ULONG,4,&lOptype,
                                                       UT_TYPE_ULONG,4,&lSnap,
                                                       UT_TYPE_ULONG,4,&lType,
                                                       UT_TYPE_ULONG,4,&lStamp,
                                                       UT_TYPE_ULONG,4,&lModtime);
            }
            pasDbCloseCursor(psCur);
        }
      }
    */

    //加载网络控制策略
    //  sprintf(caTemp,"select count(*),max(nwprocynet.modtime) from nwprocynet,nwmainprocy where nwmainprocy.sid=nwprocynet.pid ");
    lCount = 0;
    lModtime = 0;
    sprintf(caTemp, "select count(*),max(updatetime) from nwnetprocy");
    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount,
                   UT_TYPE_LONG, 4, &lModtime);
    if(lModtime > lLasttime_procy)
    {
        lLasttime_procy = lModtime;
    }
    /*  if(lCount>0){

        psCur = pasDbOpenSqlF("select pid,trantype,sport,eport,optype,snap,type,max(nwprocynet.modtime) from nwprocynet,nwmainprocy where nwmainprocy.sid=nwprocynet.pid and nwmainprocy.flag=0 and nwprocynet.flag=0 group by pid,trantype,sport,eport,optype,snap,type ");
        if(psCur) {
            lPid=0;
            lTrantype=0;
            lSport=0;
            lEport=0;
            lOptype=0;
            lSnap=0;
            lType=0;
            lStamp=0;
            lModtime=0;
                iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                               UT_TYPE_ULONG,4,&lTrantype,
                                               UT_TYPE_ULONG,4,&lSport,
                                               UT_TYPE_ULONG,4,&lEport,
                                               UT_TYPE_ULONG,4,&lOptype,
                                               UT_TYPE_ULONG,4,&lSnap,
                                               UT_TYPE_ULONG,4,&lType,
                                               UT_TYPE_ULONG,4,&lModtime);



                while(iReturn == 0||iReturn==1405) {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);


                    pData=(lanPmain*)utShmHashLook(psShmHead,NC_LNK_NW_PROCY,&lPid);
                    if(pData){
                      if(pData->sumnet==0){
     //                     pData->psPnet=(lanPnet*)malloc((lCount+1)*sizeof(lanPnet));
                      }
                      index=pData->sumnet;
                      pData->psPnet[index].pid=lPid;
                      pData->psPnet[index].sport=lSport;
                      pData->psPnet[index].eport=lEport;
                      pData->psPnet[index].trantype=lTrantype;
                      pData->psPnet[index].optype=lOptype;
                      pData->psPnet[index].snap=lSnap;
                      pData->psPnet[index].type=lType;

     //                 printf("caFile=%s\n",pData->psFile[pData->sumfile].filename);
                      pData->sumnet++;
                    }

                    lPid=0;
                    lTrantype=0;
                    lSport=0;
                    lEport=0;
                    lOptype=0;
                    lSnap=0;
                    lType=0;
                    lStamp=0;
                    lModtime=0;
                        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                                       UT_TYPE_ULONG,4,&lTrantype,
                                                       UT_TYPE_ULONG,4,&lSport,
                                                       UT_TYPE_ULONG,4,&lEport,
                                                       UT_TYPE_ULONG,4,&lOptype,
                                                       UT_TYPE_ULONG,4,&lSnap,
                                                       UT_TYPE_ULONG,4,&lType,
                                                       UT_TYPE_ULONG,4,&lModtime);
            }
            pasDbCloseCursor(psCur);
        }
      }
    */
    //加载Url控制策略
    //  sprintf(caTemp,"select count(*),max(nwprocyurl.modtime) from nwprocyurl,nwmainprocy where nwmainprocy.sid=nwprocyurl.pid ");
    lCount = 0;
    lModtime = 0;
    sprintf(caTemp, "select count(*),max(updatetime) from nwurlprocy");
    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount,
                   UT_TYPE_LONG, 4, &lModtime);
    if(lModtime > lLasttime_procy)
    {
        lLasttime_procy = lModtime;
    }
    /*  if(lCount>0){

        psCur = pasDbOpenSqlF("select pid,url,urlclass,optype,snap,type,max(nwprocyurl.modtime) from nwprocyurl,nwmainprocy where nwmainprocy.sid=nwprocyurl.pid and nwmainprocy.flag=0 and nwprocyurl.flag=0 group by pid,url,urlclass,optype,snap,type ");
        if(psCur) {
            lPid=0;
            memset(caUrl,0,sizeof(caUrl));
            lUrlclass=0;
            lOptype=0;
            lSnap=0;
            lType=0;
            lStamp=0;
            lModtime=0;
                iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                               UT_TYPE_STRING,63,caUrl,
                                               UT_TYPE_ULONG,4,&lUrlclass,
                                               UT_TYPE_ULONG,4,&lOptype,
                                               UT_TYPE_ULONG,4,&lSnap,
                                               UT_TYPE_ULONG,4,&lType,
                                               UT_TYPE_ULONG,4,&lModtime);



                while(iReturn == 0||iReturn==1405) {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);


                    pData=(lanPmain*)utShmHashLook(psShmHead,NC_LNK_NW_PROCY,&lPid);
                    if(pData){
                      if(pData->sumurl==0){
    //                      pData->psPurl=(lanPurl*)malloc((lCount+1)*sizeof(lanPurl));
                      }
                      index=pData->sumurl;
                      pData->psPurl[index].pid=lPid;
                      pData->psPurl[index].urlclass=lUrlclass;
                      strcpy(pData->psPurl[index].url,caUrl);
                      pData->psPurl[index].optype=lOptype;
                      pData->psPurl[index].snap=lSnap;
                      pData->psPurl[index].type=lType;

     //                 printf("caFile=%s\n",pData->psFile[pData->sumfile].filename);
                      pData->sumurl++;
                    }

                    lPid=0;
                    memset(caUrl,0,sizeof(caUrl));
                    lUrlclass=0;
                    lOptype=0;
                    lSnap=0;
                    lType=0;
                    lStamp=0;
                    lModtime=0;
                        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lPid,
                                                       UT_TYPE_STRING,63,caUrl,
                                                       UT_TYPE_ULONG,4,&lUrlclass,
                                                       UT_TYPE_ULONG,4,&lOptype,
                                                       UT_TYPE_ULONG,4,&lSnap,
                                                       UT_TYPE_ULONG,4,&lType,
                                                       UT_TYPE_ULONG,4,&lModtime);
            }
            pasDbCloseCursor(psCur);
        }
      }
    */

    //加载日志传送控制策略
    //  sprintf(caTemp,"select count(*),max(nwprocyurl.modtime) from nwprocyurl,nwmainprocy where nwmainprocy.sid=nwprocyurl.pid ");
    lCount = 0;
    lModtime = 0;
    sprintf(caTemp, "select count(*),max(updatetime) from nwlogprocy");
    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount,
                   UT_TYPE_LONG, 4, &lModtime);
    if(lModtime > lLasttime_procy)
    {
        lLasttime_procy = lModtime;
    }

    //加载其它控制策略
    sprintf(caTemp, "select count(*),max(nwprocyoth.modtime) from nwprocyoth,nwmainprocy where nwmainprocy.sid=nwprocyoth.pid ");
    lCount = 0;
    lModtime = 0;
    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount,
                   UT_TYPE_LONG, 4, &lModtime);
    if(lModtime > lLasttime_procy)
    {
        lLasttime_procy = lModtime;
    }
    if(lCount > 0)
    {

        psCur = pasDbOpenSqlF("select pid,ctype,ftime,optype,type,max(nwprocyoth.modtime) from nwprocyoth,nwmainprocy where nwmainprocy.sid=nwprocyoth.pid and nwmainprocy.flag=0 and nwprocyoth.flag=0 group by pid,ctype,ftime,optype,type ");
        if(psCur)
        {
            lPid = 0;
            lCtype = 0;
            lFtime = 0;
            lOptype = 0;
            lSnap = 0;
            lType = 0;
            lModtime = 0;
            iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lPid,
                                     UT_TYPE_ULONG, 4, &lCtype,
                                     UT_TYPE_ULONG, 4, &lFtime,
                                     UT_TYPE_ULONG, 4, &lOptype,
                                     UT_TYPE_ULONG, 4, &lType,
                                     UT_TYPE_ULONG, 4, &lModtime);



            while(iReturn == 0 || iReturn == 1405)
            {
                // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
                pData = (lanPmain*)utShmHashLook(psShmHead, NC_LNK_NW_PROCY, &lPid);
                if(pData)
                {
                    if(pData->sumoth == 0)
                    {
                        //                      pData->psPoth=(lanPoth*)malloc((lCount+1)*sizeof(lanPoth));
                    }
                    index = pData->sumoth;
                    pData->psPoth[index].pid = lPid;
                    pData->psPoth[index].ctype = lCtype;
                    pData->psPoth[index].ftime = lFtime;
                    pData->psPoth[index].optype = lOptype;
                    pData->psPoth[index].type = lType;

                    //                 printf("caFile=%s\n",pData->psFile[pData->sumfile].filename);
                    pData->sumoth++;
                }

                lPid = 0;
                lCtype = 0;
                lFtime = 0;
                lOptype = 0;
                lSnap = 0;
                lType = 0;
                lModtime = 0;
                iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lPid,
                                         UT_TYPE_ULONG, 4, &lCtype,
                                         UT_TYPE_ULONG, 4, &lFtime,
                                         UT_TYPE_ULONG, 4, &lOptype,
                                         UT_TYPE_ULONG, 4, &lType,
                                         UT_TYPE_ULONG, 4, &lModtime);
            }
            pasDbCloseCursor(psCur);
        }
    }

    //加载其它引用控制策略
    sprintf(caTemp, "select count(*),max(nwprocyref.modtime) from nwprocyref,nwmainprocy where nwmainprocy.sid=nwprocyref.pid ");
    lCount = 0;
    lModtime = 0;
    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount,
                   UT_TYPE_LONG, 4, &lModtime);
    if(lModtime > lLasttime_procy)
    {
        lLasttime_procy = lModtime;
    }

    pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);
    if(lCount > 0)
    {

        psCur = pasDbOpenSqlF("select pid,othid,max(nwprocyref.modtime) from nwprocyref,nwmainprocy where nwmainprocy.sid=nwprocyref.pid and nwmainprocy.flag=0 and nwprocyref.flag=0 group by pid,othid ");
        if(psCur)
        {
            lPid = 0;
            lOthid = 0;
            lModtime = 0;
            iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lPid,
                                     UT_TYPE_ULONG, 4, &lOthid,
                                     UT_TYPE_ULONG, 4, &lModtime);



            while(iReturn == 0 || iReturn == 1405)
            {
                // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);


                pData = (lanPmain*)utShmHashLook(psShmHead, NC_LNK_NW_PROCY, &lPid);
                if(pData)
                {

                    index = pData->sumref;
                    if(index < 20)
                    {
                        pData->refpid[index] = lOthid;

                        //                 printf("caFile=%s\n",pData->psFile[pData->sumfile].filename);
                        pData->sumref++;
                    }
                }

                lPid = 0;
                lOthid = 0;
                lModtime = 0;
                iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lPid,
                                         UT_TYPE_ULONG, 4, &lOthid,
                                         UT_TYPE_ULONG, 4, &lModtime);
            }
            pasDbCloseCursor(psCur);
        }
    }


    lMaxRecord = utComGetVar_ld(psShmHead, "MaProcyObj", 4096L);
    utShmFreeHash(psShmHead, NC_LNK_NW_PROCYOBJ);
    iReturn = utShmHashInit(psShmHead, NC_LNK_NW_PROCYOBJ,
                            lMaxRecord, lMaxRecord, sizeof(lanProcyObj), 0, 8);
    if(iReturn < 0)
    {
        ncSysLog(NC_LOG_ERROR, "Share memory Error, lanUtlLoadProcy");
        return (-1);
    }



    psCur = pasDbOpenSqlF("select pid,ctype,id,dataid,max(nwprocyobj.modtime),nwprocyobj.flag from nwprocyobj,nwmainprocy where nwmainprocy.sid=nwprocyobj.pid group by pid,ctype,id,dataid ");
    if(psCur)
    {
        lPid = 0;
        lCtype = 0;
        lId = 0;
        lDataid = 0;
        lModtime = 0;
        lFlag = 0;
        iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lPid,
                                 UT_TYPE_ULONG, 4, &lCtype,
                                 UT_TYPE_ULONG, 4, &lId,
                                 UT_TYPE_ULONG, 4, &lDataid,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lFlag);



        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }


            if(lFlag == 0)
            {
                memset(&stKey, 0, sizeof(struct key_s));
                stKey.ctype = lCtype;
                stKey.id = lId;

                pData2 = (lanProcyObj*)utShmHashLook(psShmHead, NC_LNK_NW_PROCYOBJ, &stKey);
                if(pData2)
                {

                    sprintf(pData2->dataids + strlen(pData2->dataids), ",%lu", lDataid);
                    if(lModtime > pData2->modtime)
                    {
                        pData2->modtime = lModtime;
                    }
                    sprintf(pData2->pids + strlen(pData2->pids), ",%lu", lPid);


                }
                else
                {
                    pData2 = (lanProcyObj*)utShmHashLookA(psShmHead, NC_LNK_NW_PROCYOBJ, &stKey);
                    if(pData2)
                    {
                        sprintf(pData2->dataids, "%lu", lDataid);
                        pData2->modtime = lModtime;
                        sprintf(pData2->pids, "%lu", lPid);
                    }
                }
            }
            lPid = 0;
            lCtype = 0;
            lId = 0;
            lDataid = 0;
            lModtime = 0;
            lFlag = 0;
            iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lPid,
                                     UT_TYPE_ULONG, 4, &lCtype,
                                     UT_TYPE_ULONG, 4, &lId,
                                     UT_TYPE_ULONG, 4, &lDataid,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lFlag);
        }
        pasDbCloseCursor(psCur);
    }



    if(!utFileIsExist("/home/ncmysql/nw/baseprocy/"))
    {
        system("mkdir -p /home/ncmysql/nw/baseprocy/");
    }

    char ppath[64] = "";
    char path[64] = "";

    sprintf(ppath, "/home/ncmysql/nw/baseprocy/");

    //生成通用软件库
    /*   sprintf(caTemp,"select nwsoftclass.id,nwsoftclass.name,nwsoftlist.name,nwsoftlist.pname,max(nwsoftlist.modtime) from nwsoftclass,nwsoftlist where nwsoftclass.id=nwsoftlist.id and nwsoftlist.createuser = '0'  group by nwsoftclass.id,nwsoftclass.name,nwsoftlist.name,nwsoftlist.pname order by nwsoftclass.id ");
       psCur = pasDbOpenSqlF(caTemp);

       writeLog("baseData.txt","search2",caTemp);
        if(psCur) {
            lId=0;
            iNum=0;
            memset(caSclass,0,sizeof(caSclass));
            memset(caSname,0,sizeof(caSname));
            memset(caPname,0,sizeof(caPname));
            lModtime=0;

                iReturn = pasDbFetchInto(psCur,
                            UT_TYPE_ULONG,4,&lId,
                            UT_TYPE_STRING,63,caSclass,
                            UT_TYPE_STRING,63,caSname,
                            UT_TYPE_STRING,63,caPname,
                            UT_TYPE_ULONG,4,&lModtime);

                while(iReturn == 0||iReturn==1405) {
                if(lLasttime_base<lModtime){
                    lLasttime_base=lModtime;
                }
              if(iNum==0){
                  fp=fopen("/home/ncmysql/nw/baseprocy/Program_type.txt","w");
              }
              fprintf(fp,"%s^%s^%s^%lu\r\n",caSclass,caSname,caPname,lId);
                    iNum++;
                    lId=0;
                    memset(caSclass,0,sizeof(caSclass));
                    memset(caSname,0,sizeof(caSname));
                    memset(caPname,0,sizeof(caPname));
                    lModtime=0;
                        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lId,
                                                       UT_TYPE_STRING,63,caSclass,
                                                       UT_TYPE_STRING,63,caSname,
                                                       UT_TYPE_STRING,63,caPname,
                                                       UT_TYPE_ULONG,4,&lModtime);
            }
            pasDbCloseCursor(psCur);
            if(iNum>0){
                fclose(fp);
            }
        }

       //生成用户公司文件夹
       psCur = NULL;
       sprintf(caTemp,"select compname from userapply");

       psCur = pasDbOpenSqlF(caTemp);
       writeLog("baseData.txt","search",caTemp);
        if(psCur){
          writeLog("baseData.txt","company","here ");
          char compname[32]="";
          while((iReturn = pasDbFetchInto(psCur,
                      UT_TYPE_STRING,sizeof(compname),compname)) == 0||iReturn==1405) {
                    writeLog("baseData.txt","company",compname);
            char tmp[64] = "";
                    memset(path,0,sizeof(path));
                sprintf(path,"%s%s/",ppath,compname);
            if(!utFileIsExist(path)) {
              sprintf(tmp,"mkdir -p %s",path);
              system(tmp);
                }
                sprintf(tmp,"cp %sProgram_type.txt %s",ppath,path);
                writeLog("baseData.txt","deal",tmp);
            system(tmp);
              }
              pasDbCloseCursor(psCur);
        }
    */
    char userid[24] = "";
    char olduserid[24] = "";
    // strcpy(olduserid, "-1");
    //生成黑白名单数据
    pasDbOneRecord("select max(updatetime) from blacklist", 0, UT_TYPE_LONG, 4, &lModtime);
    if(lLasttime_base < lModtime)
    {
        lLasttime_base = lModtime;
    }

    iNum = 0;

    psCur = pasDbOpenSqlF("select blacktype,blackname,blackvalue,blackstatus,userid from blacklist where flag=0 order by userid");
    if(psCur)
    {
        char blacktype[32] = "";
        char blackname[32] = "";
        char blackvalue[128] = "";
        unsigned long blackstatus = 0;

        while((iReturn = pasDbFetchInto(psCur,
                                        UT_TYPE_STRING, sizeof(blacktype), blacktype,
                                        UT_TYPE_STRING, sizeof(blackname), blackname,
                                        UT_TYPE_STRING, sizeof(blackvalue), blackvalue,
                                        UT_TYPE_ULONG, sizeof(blackstatus), &blackstatus,
                                        UT_TYPE_STRING, sizeof(userid) - 1, userid)) == 0 || iReturn == 1405)
        {
            printf("[userid=%s, olduserid=%s]\n", userid, olduserid);
            if(strcmp(userid, olduserid))
            {
                strcpy(olduserid, userid);
                sprintf(path, "%s%s/", ppath, userid);

                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "blacklist_type.txt");
                printf("[iNum=%d]\n", iNum);
                if(iNum > 0)
                {
                    fclose(fp);
                }
                //printf("基础数据生成：%s\n",path);
                fp = fopen(path, "w");
            }
            else
            {
                printf("do nothing\n");
            }
            pasLogs(PAS_SRCFILE, 1000, "genblackfile,file=%s,blackname=%s", path, blackname);
            fprintf(fp, "%s^%s^%s^%lu\r\n", blacktype, blackname, blackvalue, blackstatus);
            iNum++;
            memset(blacktype, 0, sizeof(blacktype));
            memset(blackname, 0, sizeof(blackname));
            memset(blackvalue, 0, sizeof(blackvalue));
            memset(userid, 0, sizeof(userid));
            blackstatus = 0;
        }

        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }
    printf("生成黑白名单基础数据完成\n");

    //生成基础数据文件
    psCur = pasDbOpenSqlF("select nwsoftclass.id,nwsoftclass.name,nwsoftlist.name,nwsoftlist.pname,max(nwsoftlist.modtime),nwsoftlist.createuser,nwsoftlist.flag from nwsoftclass,nwsoftlist where nwsoftclass.id=nwsoftlist.id group by nwsoftclass.id,nwsoftclass.name,nwsoftlist.name,nwsoftlist.pname order by createuser ");

    if(psCur)
    {
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        memset(caSclass, 0, sizeof(caSclass));
        memset(caSname, 0, sizeof(caSname));
        memset(caPname, 0, sizeof(caPname));
        memset(olduserid, 0, sizeof(olduserid));
        memset(path, 0, sizeof(path));
        lModtime = 0;
        unsigned long flag = 1;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lId,
                                 UT_TYPE_STRING, 63, caSclass,
                                 UT_TYPE_STRING, 63, caSname,
                                 UT_TYPE_STRING, 63, caPname,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_STRING, sizeof(userid), userid,
                                 UT_TYPE_ULONG, 4, &flag);

        sprintf(path, "%s", ppath);
        if(!utFileIsExist(path))
        {
            char tmp[64] = "";
            sprintf(tmp, "mkdir -p %s", path);
            system(tmp);
        }
        sprintf(path + strlen(path), "Program_type.txt");

        fp = fopen(path, "w");

        printf("基础数据下发\n");

        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(userid) == 0)
            {
                strcpy(userid, "0");
            }

            if(lLasttime_base < lModtime)
            {
                lLasttime_base = lModtime;
            }

            iNum++;

            if(flag == 0)
            {
                fprintf(fp, "%s^%s^%s^%lu\r\n", caSclass, caSname, caPname, lId);
            }

            lId = 0;
            iNum = 0;
            memset(caSclass, 0, sizeof(caSclass));
            memset(caSname, 0, sizeof(caSname));
            memset(caPname, 0, sizeof(caPname));
            memset(olduserid, 0, sizeof(olduserid));
            memset(path, 0, sizeof(path));
            lModtime = 0;
            unsigned long flag = 1;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lId,
                                     UT_TYPE_STRING, 63, caSclass,
                                     UT_TYPE_STRING, 63, caSname,
                                     UT_TYPE_STRING, 63, caPname,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_STRING, sizeof(userid), userid,
                                     UT_TYPE_ULONG, 4, &flag);

        }
        pasDbCloseCursor(psCur);
        fclose(fp);
    }













    //生成USB库文件
    printf("开始生成USB库文件\n");
    psCur = pasDbOpenSqlF("select nwusbclass.id,nwusblist.hardid,max(nwusblist.modtime),nwusblist.createuser from nwusbclass,nwusblist where nwusbclass.id=nwusblist.id  group by nwusbclass.id,nwusblist.hardid order by nwusblist.createuser");
    if(psCur)
    {
        iNum = 0;
        lId = 0;
        memset(caHardid, 0, sizeof(caHardid));
        lModtime = 0;
        strcpy(olduserid, "-1");
        //    memset(olduserid,0,sizeof(olduserid));
        memset(path, 0, sizeof(path));

        iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lId,
                                 UT_TYPE_STRING, 127, caHardid,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_STRING, sizeof(userid), userid);


        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(lLasttime_base < lModtime)
            {
                lLasttime_base = lModtime;
            }
            memset(&stKey, 0, sizeof(stKey));
            stKey.ctype = 3;
            if(strcmp(userid, olduserid))
            {
                strcpy(olduserid, userid);

                sprintf(path, "%s%s/", ppath, userid);

                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "u_type.txt");

                if(iNum > 0)
                {
                    if(fp)
                    {
                        fclose(fp);
                        fp = NULL;
                    }
                }
                printf("基础数据生成：%s\n", path);
                fp = fopen(path, "w");

            }
            fprintf(fp, "%lu^%s\r\n", lId, caHardid);
            iNum++;
            lId = 0;
            memset(caHardid, 0, sizeof(caHardid));
            lModtime = 0;
            iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lId,
                                     UT_TYPE_STRING, 127, caHardid,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_STRING, sizeof(userid), userid);
        }
        pasDbCloseCursor(psCur);

        if(iNum > 0)
        {
            struct stat fileStat;
            int res = fstat(fileno(fp), &fileStat);

            printf("关闭文件句柄：fp:%0x, %d\n", fp,  fileno(fp));
            if(fp && (res != -1))
            {
                fclose(fp);
                //fp = 0;
            }

            printf("关闭文件句柄成功！：%d\n", fileno(fp));

        }
    }


    //生成时间数据
    printf("开始生成时间数据库文件\n");
    psCur = pasDbOpenSqlF("select nwdatedes.id,max(nwlimdate.sid),nwlimdate.ltype,nwlimdate.llower,nwlimdate.lupper,nwlimdate.lltime,nwlimdate.lutime,max(nwlimdate.modtime),nwlimdate.createuser from nwdatedes,nwlimdate where nwdatedes.id=nwlimdate.id group by nwdatedes.id,nwlimdate.ltype,nwlimdate.llower,nwlimdate.lupper,nwlimdate.lltime,nwlimdate.lutime  order by nwlimdate.createuser");
    if(psCur)
    {
        iNum = 0;
        lId = 0;
        lSid = 0;
        lLtype = 0;
        lLower = 0;
        lLupper = 0;
        lLtime = 0;
        lUtime = 0;
        lModtime = 0;
        memset(olduserid, 0, sizeof(olduserid));
        memset(path, 0, sizeof(path));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lId,
                                 UT_TYPE_ULONG, 4, &lSid,
                                 UT_TYPE_ULONG, 4, &lLtype,
                                 UT_TYPE_ULONG, 4, &lLower,
                                 UT_TYPE_ULONG, 4, &lLupper,
                                 UT_TYPE_ULONG, 4, &lLtime,
                                 UT_TYPE_ULONG, 4, &lUtime,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_STRING, sizeof(userid), userid);



        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(lLasttime_base < lModtime)
            {
                lLasttime_base = lModtime;
            }
            if(strcmp(userid, olduserid))
            {
                strcpy(olduserid, userid);

                sprintf(path, "%s%s/", ppath, userid);

                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "time.ini");

                printf("time_in:%s\n", path);

                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");

            }
            fprintf(fp, "[law%d]\r\n", iNum);
            fprintf(fp, "id=%d\r\n", lId);
            fprintf(fp, "timeid=%d\r\n", lSid);
            fprintf(fp, "ltype=%d\r\n", lLtype);
            fprintf(fp, "llower=%d\r\n", lLower);
            fprintf(fp, "lupper=%d\r\n", lLupper);
            fprintf(fp, "lltime=%d\r\n", lLtime);
            fprintf(fp, "lutime=%d\r\n", lUtime);
            iNum++;
            lId = 0;
            lSid = 0;
            lLtype = 0;
            lLower = 0;
            lLupper = 0;
            lLtime = 0;
            lUtime = 0;
            lModtime = 0;
            iReturn = pasDbFetchInto(psCur, UT_TYPE_ULONG, 4, &lId,
                                     UT_TYPE_ULONG, 4, &lSid,
                                     UT_TYPE_ULONG, 4, &lLtype,
                                     UT_TYPE_ULONG, 4, &lLower,
                                     UT_TYPE_ULONG, 4, &lLupper,
                                     UT_TYPE_ULONG, 4, &lLtime,
                                     UT_TYPE_ULONG, 4, &lUtime,
                                     UT_TYPE_ULONG, 4, &lModtime);
        }
        pasDbCloseCursor(psCur);
        if((iNum > 0)  &&  fp)
        {
            fclose(fp);
            fp = NULL;
        }
    }


    printf("开始生成敏感进程屏幕库文件\n");
    psCur = pasDbOpenSqlF("select paramtype,keyname,keyvalue,detail,company,modtime from paramsmanage where paramtype<>1  and keyvalue<>'' and flag<>1 order by company");
    if(psCur)
    {
        unsigned int paramtype = 0;
        char keyname[32] = "";
        char keyvalue[32] = "";
        char detail[128] = "";

        memset(olduserid, 0, sizeof(olduserid));
        memset(path, 0, sizeof(path));
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, sizeof(paramtype), &paramtype,
                                 UT_TYPE_STRING, sizeof(keyname) - 1, keyname,
                                 UT_TYPE_STRING, sizeof(keyvalue) - 1, keyvalue,
                                 UT_TYPE_STRING, sizeof(detail) - 1,  detail,
                                 UT_TYPE_STRING, sizeof(userid) - 1,  userid,
                                 UT_TYPE_ULONG, sizeof(lModtime),  &lModtime);

        while(iReturn == 0 || iReturn == 1405)
        {

            if(lLasttime_base < lModtime)
            {
                lLasttime_base = lModtime;
            }
            //printf("adfdold:%s,new:%s\n",olduserid,userid);
            if(strcmp(userid, olduserid))
            {
                strcpy(olduserid, userid);

                sprintf(path, "%s%s/", ppath, userid);

                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "procscreen.ini");
                //printf("filePath:%s\n");

                if(iNum > 0)
                {
                    if(fp)
                    {
                        fclose(fp);
                        fp = NULL;
                    }
                }

                fp = fopen(path, "w");
            }
            fprintf(fp, "%s^%s^%s\r\n", keyname, keyvalue, detail);
            iNum++;
            memset(keyname, 0, sizeof(keyname));
            memset(keyvalue, 0, sizeof(keyvalue));
            memset(path, 0, sizeof(path));
            lModtime = 0;
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, sizeof(paramtype), &paramtype,
                                     UT_TYPE_STRING, sizeof(keyname) - 1, keyname,
                                     UT_TYPE_STRING, sizeof(keyvalue) - 1, keyvalue,
                                     UT_TYPE_STRING, sizeof(detail) - 1,  detail,
                                     UT_TYPE_STRING, sizeof(userid) - 1,  userid,
                                     UT_TYPE_ULONG, sizeof(lModtime),  &lModtime);
        }
        pasDbCloseCursor(psCur);

        if(iNum > 0)
        {
            if(fp)
            {
                fclose(fp);
                fp = NULL;
            }
        }
    }

    //生成参数库
    printf("生成参数库文件\n");
    //生成基础数据文件
    psCur = pasDbOpenSqlF("select paramtype,keyname,keyvalue,detail,company,modtime,flag from paramsmanage where  keyvalue!='' order by company,paramtype,keyname ");

    if(psCur)
    {
        char keyname[32], keyvalue[32], detail[128], company[32];

        unsigned long lParamtype;
        unsigned long flag;
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lParamtype = 0;
        memset(keyname, 0, sizeof(keyname));
        memset(keyvalue, 0, sizeof(keyvalue));
        memset(detail, 0, sizeof(detail));
        memset(company, 0, sizeof(company));
        memset(path, 0, sizeof(path));
        lModtime = 0;
        flag = 0;
        iNum = 0;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lParamtype,
                                 UT_TYPE_STRING, 31, keyname,
                                 UT_TYPE_STRING, 31, keyvalue,
                                 UT_TYPE_STRING, 127, detail,
                                 UT_TYPE_STRING, 31, company,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &flag);



        printf("基础参数下发\n");

        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(company) == 0)
            {
                strcpy(company, "0");
            }

            if(lLasttime_base < lModtime)
            {
                lLasttime_base = lModtime;
            }

            if(strcmp(company, caPrecompany) != 0)
            {
                strcpy(caPrecompany, company);
                sprintf(path, "%s%s/", ppath, company);
                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "Basetypeinfo.txt");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
            }
            if(flag == 0)
            {
                fprintf(fp, "%d^%s^%s^%s\r\n", lParamtype, keyname, keyvalue, detail);
            }



            lParamtype = 0;
            memset(keyname, 0, sizeof(keyname));
            memset(keyvalue, 0, sizeof(keyvalue));
            memset(detail, 0, sizeof(detail));
            memset(company, 0, sizeof(company));
            memset(path, 0, sizeof(path));
            lModtime = 0;
            flag = 0;


            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lParamtype,
                                     UT_TYPE_STRING, 31, keyname,
                                     UT_TYPE_STRING, 31, keyvalue,
                                     UT_TYPE_STRING, 127, detail,
                                     UT_TYPE_STRING, 31, company,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &flag);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }

    //生成HTTP外发文件策略
    printf("生成HTTP外发文件策略\n");

    psCur = pasDbOpenSqlF("select groupid,computerid,company,hosttype,hostmail,hostmailmode,httptype,httpname,httpmode,exptype,fileexp,fileexpmode,filesize,controlcode,flag,modtime,level from nwfilehttpprocy order by company,groupid,computerid ");


    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lHosttype, lHostmailmode, lHttptype, lHttpmode, lExptype, lFileexpmode, lFilesize, lControlcode, lLevel;
        char caCompany[24], caHostmail[24], caHttpname[24], caFileexp[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        lHosttype = 0;
        memset(caHostmail, 0, sizeof(caHostmail));
        lHostmailmode = 0;
        lHttptype = 0;
        memset(caHttpname, 0, sizeof(caHttpname));
        lHttpmode = 0;
        lExptype = 0;
        memset(caFileexp, 0, sizeof(caFileexp));
        lFileexpmode = 0;
        lFilesize = 0;
        lControlcode = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lHosttype,
                                 UT_TYPE_STRING, 23, caHostmail,
                                 UT_TYPE_ULONG, 4, &lHostmailmode,
                                 UT_TYPE_ULONG, 4, &lHttptype,
                                 UT_TYPE_STRING, 23, caHttpname,
                                 UT_TYPE_ULONG, 4, &lHttpmode,
                                 UT_TYPE_ULONG, 4, &lExptype,
                                 UT_TYPE_STRING, 23, caFileexp,
                                 UT_TYPE_ULONG, 4, &lFileexpmode,
                                 UT_TYPE_ULONG, 4, &lFilesize,
                                 UT_TYPE_ULONG, 4, &lControlcode,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("Http外发文件策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }



            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);
                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "httpoutfile.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "hosttype=%d\r\n", lHosttype);
                fprintf(fp, "hostmail=%s\r\n", caHostmail);
                fprintf(fp, "hostmailmode=%d\r\n", lHostmailmode);
                fprintf(fp, "httptype=%d\r\n", lHttptype);
                fprintf(fp, "httpname=%s\r\n", caHttpname);
                fprintf(fp, "httpmode=%d\r\n", lHttpmode);
                fprintf(fp, "mailsendtype=\r\n");
                fprintf(fp, "mailsend=\r\n");
                fprintf(fp, "mailsendmode=\r\n");
                fprintf(fp, "mailrevtype=\r\n");
                fprintf(fp, "mailrev=\r\n");
                fprintf(fp, "mailrevmode=\r\n");
                fprintf(fp, "exptype=%d\r\n", lExptype);
                fprintf(fp, "fileexp=%s\r\n", caFileexp);
                fprintf(fp, "fileexpmode=%d\r\n", lFileexpmode);
                fprintf(fp, "filesize=%d\r\n", lFilesize);
                fprintf(fp, "controlcode=%d\r\n", lControlcode);

            }



            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            lHosttype = 0;
            memset(caHostmail, 0, sizeof(caHostmail));
            lHostmailmode = 0;
            lHttptype = 0;
            memset(caHttpname, 0, sizeof(caHttpname));
            lHttpmode = 0;
            lExptype = 0;
            memset(caFileexp, 0, sizeof(caFileexp));
            lFileexpmode = 0;
            lFilesize = 0;
            lControlcode = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lHosttype,
                                     UT_TYPE_STRING, 23, caHostmail,
                                     UT_TYPE_ULONG, 4, &lHostmailmode,
                                     UT_TYPE_ULONG, 4, &lHttptype,
                                     UT_TYPE_STRING, 23, caHttpname,
                                     UT_TYPE_ULONG, 4, &lHttpmode,
                                     UT_TYPE_ULONG, 4, &lExptype,
                                     UT_TYPE_STRING, 23, caFileexp,
                                     UT_TYPE_ULONG, 4, &lFileexpmode,
                                     UT_TYPE_ULONG, 4, &lFilesize,
                                     UT_TYPE_ULONG, 4, &lControlcode,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }

    //生成外发文件优先策略

    printf("生成外发文件优先策略\n");
    psCur = pasDbOpenSqlF("select groupid,computerid,company,controlcode,modtime,level,flag from nwfilemainprocy order by company,groupid,computerid ");

    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lControlcode, lLevel;
        char caCompany[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        lControlcode = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lControlcode,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel,
                                 UT_TYPE_ULONG, 4, &flag);



        printf("外发文件优先策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);

            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }

            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {
                    sprintf(path, "%s%s/", ppath, caCompany);

                }
                else if(lGroupid != 0 && lComputerid == 0)
                {
                    sprintf(path, "%sG%lu/", ppath, lGroupid);

                }
                else if(lComputerid != 0)
                {
                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "gloaboutfile.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;

            if(flag == 0 && fp)
            {
                //      printf("path=%s\n",path);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "controlcode=%d\r\n", lControlcode);

            }



            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            lControlcode = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lControlcode,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel,
                                     UT_TYPE_ULONG, 4, &flag);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }



    //生成smtp外发文件策略
    printf("生成smtp外发文件策略\n");

    psCur = pasDbOpenSqlF("select groupid,computerid,company,hosttype,hostmail,hostmailmode,smtptype,smtpname,\
     smtpmode,mailsendtype,mailsend,mailsendmode,mailrevtype,mailRev,mailRevmode,exptype,fileexp,fileexpmode,filesize,controlcode,flag,modtime,level from nwfilesmtpprocy order by company,groupid,computerid ");


    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lHosttype, lHostmailmode, lSmtptype, lSmtpmode, lMailsendtype, lMailsendmode, lMailrevtype, lMailrevmode, lExptype, lFileexpmode, lFilesize, lControlcode, lLevel;
        char caCompany[24], caHostmail[24], caSmtpname[24], caFileexp[24], caMailsend[24], caMailrev[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        lHosttype = 0;
        memset(caHostmail, 0, sizeof(caHostmail));
        memset(caSmtpname, 0, sizeof(caSmtpname));
        memset(caMailsend, 0, sizeof(caMailsend));
        memset(caMailrev, 0, sizeof(caMailrev));
        lHostmailmode = 0;
        lSmtptype = 0;
        lSmtpmode = 0;
        lMailsendtype = 0;
        lMailsendmode = 0;
        lMailrevtype = 0;
        lMailrevmode = 0;
        lExptype = 0;
        memset(caFileexp, 0, sizeof(caFileexp));
        lFileexpmode = 0;
        lFilesize = 0;
        lControlcode = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lHosttype,
                                 UT_TYPE_STRING, 23, caHostmail,
                                 UT_TYPE_ULONG, 4, &lHostmailmode,
                                 UT_TYPE_ULONG, 4, &lSmtptype,
                                 UT_TYPE_STRING, 23, caSmtpname,
                                 UT_TYPE_ULONG, 4, &lSmtpmode,
                                 UT_TYPE_ULONG, 4, &lMailsendtype,
                                 UT_TYPE_STRING, 23, caMailsend,
                                 UT_TYPE_ULONG, 4, &lMailsendmode,
                                 UT_TYPE_ULONG, 4, &lMailrevtype,
                                 UT_TYPE_STRING, 23, caMailrev,
                                 UT_TYPE_ULONG, 4, &lMailrevmode,
                                 UT_TYPE_ULONG, 4, &lExptype,
                                 UT_TYPE_STRING, 23, caFileexp,
                                 UT_TYPE_ULONG, 4, &lFileexpmode,
                                 UT_TYPE_ULONG, 4, &lFilesize,
                                 UT_TYPE_ULONG, 4, &lControlcode,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("Http外发文件策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {

            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {
                    sprintf(path, "%s%s/", ppath, caCompany);

                }
                else if(lGroupid != 0 && lComputerid == 0)
                {
                    sprintf(path, "%sG%lu/", ppath, lGroupid);

                }
                else if(lComputerid != 0)
                {
                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "smtpoutfile.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "hosttype=%d\r\n", lHosttype);
                fprintf(fp, "hostmail=%s\r\n", caHostmail);
                fprintf(fp, "hostmailmode=%d\r\n", lHostmailmode);
                fprintf(fp, "smtptype=%d\r\n", lSmtptype);
                fprintf(fp, "smtpname=%s\r\n", caSmtpname);
                fprintf(fp, "smtpmode=%d\r\n", lSmtpmode);
                fprintf(fp, "mailsendtype=%d\r\n", lMailsendtype);
                fprintf(fp, "mailsend=%s\r\n", caMailsend);
                fprintf(fp, "mailsendmode=%d\r\n", lMailsendmode);
                fprintf(fp, "mailrevtype=%d\r\n", lMailrevtype);
                fprintf(fp, "mailrev=%s\r\n", caMailrev);
                fprintf(fp, "mailrevmode=%d\r\n", lMailrevmode);
                fprintf(fp, "exptype=%d\r\n", lExptype);
                fprintf(fp, "fileexp=%s\r\n", caFileexp);
                fprintf(fp, "fileexpmode=%d\r\n", lFileexpmode);
                fprintf(fp, "filesize=%d\r\n", lFilesize);
                fprintf(fp, "controlcode=%d\r\n", lControlcode);

            }



            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            lHosttype = 0;
            memset(caHostmail, 0, sizeof(caHostmail));
            memset(caSmtpname, 0, sizeof(caSmtpname));
            memset(caMailsend, 0, sizeof(caMailsend));
            memset(caMailrev, 0, sizeof(caMailrev));
            lHostmailmode = 0;
            lSmtptype = 0;
            lSmtpmode = 0;
            lMailsendtype = 0;
            lMailsendmode = 0;
            lMailrevtype = 0;
            lMailrevmode = 0;
            lExptype = 0;
            memset(caFileexp, 0, sizeof(caFileexp));
            lFileexpmode = 0;
            lFilesize = 0;
            lControlcode = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lHosttype,
                                     UT_TYPE_STRING, 23, caHostmail,
                                     UT_TYPE_ULONG, 4, &lHostmailmode,
                                     UT_TYPE_ULONG, 4, &lSmtptype,
                                     UT_TYPE_STRING, 23, caSmtpname,
                                     UT_TYPE_ULONG, 4, &lSmtpmode,
                                     UT_TYPE_ULONG, 4, &lMailsendtype,
                                     UT_TYPE_STRING, 23, caMailsend,
                                     UT_TYPE_ULONG, 4, &lMailsendmode,
                                     UT_TYPE_ULONG, 4, &lMailrevtype,
                                     UT_TYPE_STRING, 23, caMailrev,
                                     UT_TYPE_ULONG, 4, &lMailrevmode,
                                     UT_TYPE_ULONG, 4, &lExptype,
                                     UT_TYPE_STRING, 23, caFileexp,
                                     UT_TYPE_ULONG, 4, &lFileexpmode,
                                     UT_TYPE_ULONG, 4, &lFilesize,
                                     UT_TYPE_ULONG, 4, &lControlcode,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }



    //生成ftp外发文件策略
    printf("生成ftp外发文件策略\n");

    psCur = pasDbOpenSqlF("select groupid,computerid,company,ftpservertype,ftpserver,ftpservermode,ftptype,ftpname,ftpmode,exptype,fileexp,fileexpmode,filesize,controlcode,flag,modtime,level from nwfileftpprocy order by company,groupid,computerid ");


    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lFtptype, lFtpmode, lFtpservertype, lFtpservermode, lExptype, lFileexpmode, lFilesize, lControlcode, lLevel;
        char caCompany[24], caFtpserver[24], caFtpname[24], caFileexp[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        lFtpservertype = 0;
        memset(caFtpserver, 0, sizeof(caFtpserver));
        lFtpservermode = 0;
        lFtptype = 0;
        memset(caFtpname, 0, sizeof(caFtpname));
        lFtpmode = 0;
        lExptype = 0;
        memset(caFileexp, 0, sizeof(caFileexp));
        lFileexpmode = 0;
        lFilesize = 0;
        lControlcode = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lFtpservertype,
                                 UT_TYPE_STRING, 23, caFtpserver,
                                 UT_TYPE_ULONG, 4, &lFtpservermode,
                                 UT_TYPE_ULONG, 4, &lFtptype,
                                 UT_TYPE_STRING, 23, caFtpname,
                                 UT_TYPE_ULONG, 4, &lFtpmode,
                                 UT_TYPE_ULONG, 4, &lExptype,
                                 UT_TYPE_STRING, 23, caFileexp,
                                 UT_TYPE_ULONG, 4, &lFileexpmode,
                                 UT_TYPE_ULONG, 4, &lFilesize,
                                 UT_TYPE_ULONG, 4, &lControlcode,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("FTP外发文件策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }

            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "ftpoutfile.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "ftpservertype=%d\r\n", lFtpservertype);
                fprintf(fp, "ftpserver=%s\r\n", caFtpserver);
                fprintf(fp, "ftpservermode=%d\r\n", lFtpservermode);
                fprintf(fp, "ftptype=%d\r\n", lFtptype);
                fprintf(fp, "ftpname=%s\r\n", caFtpname);
                fprintf(fp, "ftpmode=%d\r\n", lFtpmode);
                fprintf(fp, "exptype=%d\r\n", lExptype);
                fprintf(fp, "fileexp=%s\r\n", caFileexp);
                fprintf(fp, "fileexpmode=%d\r\n", lFileexpmode);
                fprintf(fp, "filesize=%d\r\n", lFilesize);
                fprintf(fp, "controlcode=%d\r\n", lControlcode);

            }



            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            lFtpservertype = 0;
            memset(caFtpserver, 0, sizeof(caFtpserver));
            lFtpservermode = 0;
            lFtptype = 0;
            memset(caFtpname, 0, sizeof(caFtpname));
            lFtpmode = 0;
            lExptype = 0;
            memset(caFileexp, 0, sizeof(caFileexp));
            lFileexpmode = 0;
            lFilesize = 0;
            lControlcode = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lFtpservertype,
                                     UT_TYPE_STRING, 23, caFtpserver,
                                     UT_TYPE_ULONG, 4, &lFtpservermode,
                                     UT_TYPE_ULONG, 4, &lFtptype,
                                     UT_TYPE_STRING, 23, caFtpname,
                                     UT_TYPE_ULONG, 4, &lFtpmode,
                                     UT_TYPE_ULONG, 4, &lExptype,
                                     UT_TYPE_STRING, 23, caFileexp,
                                     UT_TYPE_ULONG, 4, &lFileexpmode,
                                     UT_TYPE_ULONG, 4, &lFilesize,
                                     UT_TYPE_ULONG, 4, &lControlcode,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }

    //生成chat外发文件策略
    printf("生成chat外发文件策略\n");

    psCur = pasDbOpenSqlF("select groupid,computerid,company,chatusertype,chatuser,usermode,chattype,chatname,chatmode,exptype,fileexp,fileexpmode,filesize,controlcode,flag,modtime,level from nwfilechatprocy order by company,groupid,computerid ");


    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lChatusertype, lChatusermode, lChattype, lChatmode, lExptype, lFileexpmode, lFilesize, lControlcode, lLevel;
        char caCompany[24], caChatuser[24], caChatname[24], caFileexp[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        lChatusertype = 0;
        memset(caChatuser, 0, sizeof(caChatuser));
        lChatusermode = 0;
        lChattype = 0;
        memset(caChatname, 0, sizeof(caChatname));
        lChatmode = 0;
        lExptype = 0;
        memset(caFileexp, 0, sizeof(caFileexp));
        lFileexpmode = 0;
        lFilesize = 0;
        lControlcode = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lChatusertype,
                                 UT_TYPE_STRING, 23, caChatuser,
                                 UT_TYPE_ULONG, 4, &lChatusermode,
                                 UT_TYPE_ULONG, 4, &lChattype,
                                 UT_TYPE_STRING, 23, caChatname,
                                 UT_TYPE_ULONG, 4, &lChatmode,
                                 UT_TYPE_ULONG, 4, &lExptype,
                                 UT_TYPE_STRING, 23, caFileexp,
                                 UT_TYPE_ULONG, 4, &lFileexpmode,
                                 UT_TYPE_ULONG, 4, &lFilesize,
                                 UT_TYPE_ULONG, 4, &lControlcode,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("Chat外发文件策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }

            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "chatoutfile.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "chatusertype=%d\r\n", lChatusertype);
                fprintf(fp, "chatuser=%s\r\n", caChatuser);
                fprintf(fp, "usermode=%d\r\n", lChatusermode);
                fprintf(fp, "chattype=%d\r\n", lChattype);
                fprintf(fp, "chatname=%s\r\n", caChatname);
                fprintf(fp, "chatmode=%d\r\n", lChatmode);
                fprintf(fp, "exptype=%d\r\n", lExptype);
                fprintf(fp, "fileexp=%s\r\n", caFileexp);
                fprintf(fp, "fileexpmode=%d\r\n", lFileexpmode);
                fprintf(fp, "filesize=%d\r\n", lFilesize);
                fprintf(fp, "controlcode=%d\r\n", lControlcode);

            }



            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            lChatusertype = 0;
            memset(caChatuser, 0, sizeof(caChatuser));
            lChatusermode = 0;
            lChattype = 0;
            memset(caChatname, 0, sizeof(caChatname));
            lChatmode = 0;
            lExptype = 0;
            memset(caFileexp, 0, sizeof(caFileexp));
            lFileexpmode = 0;
            lFilesize = 0;
            lControlcode = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lChatusertype,
                                     UT_TYPE_STRING, 23, caChatuser,
                                     UT_TYPE_ULONG, 4, &lChatusermode,
                                     UT_TYPE_ULONG, 4, &lChattype,
                                     UT_TYPE_STRING, 23, caChatname,
                                     UT_TYPE_ULONG, 4, &lChatmode,
                                     UT_TYPE_ULONG, 4, &lExptype,
                                     UT_TYPE_STRING, 23, caFileexp,
                                     UT_TYPE_ULONG, 4, &lFileexpmode,
                                     UT_TYPE_ULONG, 4, &lFilesize,
                                     UT_TYPE_ULONG, 4, &lControlcode,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }


    //生成云盘外发文件策略
    printf("生成云盘外发文件策略\n");

    psCur = pasDbOpenSqlF("select groupid,computerid,company,yundisktype,yundisk,yunmode,exptype,fileexp,fileexpmode,filesize,controlcode,flag,modtime,level from nwfilecloudprocy order by company,groupid,computerid ");


    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lYundisktype, lYunmode, lExptype, lFileexpmode, lFilesize, lControlcode, lLevel;
        char caCompany[24], caYundisk[24], caFileexp[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        lYunmode = 0;
        memset(caYundisk, 0, sizeof(caYundisk));
        lYundisktype = 0;
        lExptype = 0;
        memset(caFileexp, 0, sizeof(caFileexp));
        lFileexpmode = 0;
        lFilesize = 0;
        lControlcode = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lYundisktype,
                                 UT_TYPE_STRING, 23, caYundisk,
                                 UT_TYPE_ULONG, 4, &lYunmode,
                                 UT_TYPE_ULONG, 4, &lExptype,
                                 UT_TYPE_STRING, 23, caFileexp,
                                 UT_TYPE_ULONG, 4, &lFileexpmode,
                                 UT_TYPE_ULONG, 4, &lFilesize,
                                 UT_TYPE_ULONG, 4, &lControlcode,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("云盘外发文件策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {

            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "yunoutfile.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "yundisktype=%d\r\n", lYundisktype);
                fprintf(fp, "yundisk=%s\r\n", caYundisk);
                fprintf(fp, "yunmode=%d\r\n", lYunmode);
                fprintf(fp, "exptype=%d\r\n", lExptype);
                fprintf(fp, "fileexp=%s\r\n", caFileexp);
                fprintf(fp, "fileexpmode=%d\r\n", lFileexpmode);
                fprintf(fp, "filesize=%d\r\n", lFilesize);
                fprintf(fp, "controlcode=%d\r\n", lControlcode);

            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            lYunmode = 0;
            memset(caYundisk, 0, sizeof(caYundisk));
            lYundisktype = 0;
            lExptype = 0;
            memset(caFileexp, 0, sizeof(caFileexp));
            lFileexpmode = 0;
            lFilesize = 0;
            lControlcode = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lYundisktype,
                                     UT_TYPE_STRING, 23, caYundisk,
                                     UT_TYPE_ULONG, 4, &lYunmode,
                                     UT_TYPE_ULONG, 4, &lExptype,
                                     UT_TYPE_STRING, 23, caFileexp,
                                     UT_TYPE_ULONG, 4, &lFileexpmode,
                                     UT_TYPE_ULONG, 4, &lFilesize,
                                     UT_TYPE_ULONG, 4, &lControlcode,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }


    //生成U盘外发文件策略
    printf("生成U盘外发文件策略\n");

    psCur = pasDbOpenSqlF("select groupid,computerid,company,utype,umode,exptype,fileexp,fileexpmode,filesize,controlcode,flag,modtime,level from nwfileupanprocy \
     order by company,groupid,computerid ");


    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lUtype, lUmode, lExptype, lFileexpmode, lFilesize, lControlcode, lLevel;
        char caCompany[24], caFileexp[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        lUtype = 0;
        lUmode = 0;
        lExptype = 0;
        memset(caFileexp, 0, sizeof(caFileexp));
        lFileexpmode = 0;
        lFilesize = 0;
        lControlcode = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lUtype,
                                 UT_TYPE_ULONG, 4, &lUmode,
                                 UT_TYPE_ULONG, 4, &lExptype,
                                 UT_TYPE_STRING, 23, caFileexp,
                                 UT_TYPE_ULONG, 4, &lFileexpmode,
                                 UT_TYPE_ULONG, 4, &lFilesize,
                                 UT_TYPE_ULONG, 4, &lControlcode,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("U盘外发文件策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {

            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "udiskoutfile.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "utype=%d\r\n", lUtype);
                fprintf(fp, "umode=%d\r\n", lUmode);
                fprintf(fp, "exptype=%d\r\n", lExptype);
                fprintf(fp, "fileexp=%s\r\n", caFileexp);
                fprintf(fp, "fileexpmode=%d\r\n", lFileexpmode);
                fprintf(fp, "filesize=%d\r\n", lFilesize);
                fprintf(fp, "controlcode=%d\r\n", lControlcode);

            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            lUtype = 0;
            lUmode = 0;
            lExptype = 0;
            memset(caFileexp, 0, sizeof(caFileexp));
            lFileexpmode = 0;
            lFilesize = 0;
            lControlcode = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lUtype,
                                     UT_TYPE_ULONG, 4, &lUmode,
                                     UT_TYPE_ULONG, 4, &lExptype,
                                     UT_TYPE_STRING, 23, caFileexp,
                                     UT_TYPE_ULONG, 4, &lFileexpmode,
                                     UT_TYPE_ULONG, 4, &lFilesize,
                                     UT_TYPE_ULONG, 4, &lControlcode,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }


    //生成共享外发文件策略
    printf("生成共享外发文件策略\n");

    psCur = pasDbOpenSqlF("select groupid,computerid,company,fileout,fileread,exptype,fileexp,fileexpmode,filesize,controlcode,flag,modtime,level from nwfileshareprocy \
     order by company,groupid,computerid ");


    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lFileout, lFileread, lExptype, lFileexpmode, lFilesize, lControlcode, lLevel;
        char caCompany[24], caFileexp[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        lFileout = 0;
        lFileread = 0;
        lExptype = 0;
        memset(caFileexp, 0, sizeof(caFileexp));
        lFileexpmode = 0;
        lFilesize = 0;
        lControlcode = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lFileout,
                                 UT_TYPE_ULONG, 4, &lFileread,
                                 UT_TYPE_ULONG, 4, &lExptype,
                                 UT_TYPE_STRING, 23, caFileexp,
                                 UT_TYPE_ULONG, 4, &lFileexpmode,
                                 UT_TYPE_ULONG, 4, &lFilesize,
                                 UT_TYPE_ULONG, 4, &lControlcode,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("共享外发文件策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }

            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "shareoutfile.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "fileout=%d\r\n", lFileout);
                fprintf(fp, "fileread=%d\r\n", lFileread);
                fprintf(fp, "exptype=%d\r\n", lExptype);
                fprintf(fp, "fileexp=%s\r\n", caFileexp);
                fprintf(fp, "fileexpmode=%d\r\n", lFileexpmode);
                fprintf(fp, "filesize=%d\r\n", lFilesize);
                fprintf(fp, "controlcode=%d\r\n", lControlcode);

            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            lFileout = 0;
            lFileread = 0;
            lExptype = 0;
            memset(caFileexp, 0, sizeof(caFileexp));
            lFileexpmode = 0;
            lFilesize = 0;
            lControlcode = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lFileout,
                                     UT_TYPE_ULONG, 4, &lFileread,
                                     UT_TYPE_ULONG, 4, &lExptype,
                                     UT_TYPE_STRING, 23, caFileexp,
                                     UT_TYPE_ULONG, 4, &lFileexpmode,
                                     UT_TYPE_ULONG, 4, &lFilesize,
                                     UT_TYPE_ULONG, 4, &lControlcode,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }


    //生成其它进程外发文件策略
    printf("生成其它进程外发文件策略\n");

    psCur = pasDbOpenSqlF("select groupid,computerid,company,15,otherapp,bkother,exptype,fileexp,fileexpmode,filesize,controlcode,flag,modtime,level from nwfileotherprocy order by company,groupid,computerid ");


    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lOtherapptype, lBkother, lExptype, lFileexpmode, lFilesize, lControlcode, lLevel;
        char caCompany[24], caOtherapp[24][24], caFileexp[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        lOtherapptype = 0;
        memset(caOtherapp, 0, sizeof(caOtherapp));
        lBkother = 0;
        lExptype = 0;
        memset(caFileexp, 0, sizeof(caFileexp));
        lFileexpmode = 0;
        lFilesize = 0;
        lControlcode = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lOtherapptype,
                                 UT_TYPE_STRING, 23, caOtherapp,
                                 UT_TYPE_ULONG, 4, &lBkother,
                                 UT_TYPE_ULONG, 4, &lExptype,
                                 UT_TYPE_STRING, 23, caFileexp,
                                 UT_TYPE_ULONG, 4, &lFileexpmode,
                                 UT_TYPE_ULONG, 4, &lFilesize,
                                 UT_TYPE_ULONG, 4, &lControlcode,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("其它进程外发文件策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "otheroutfile.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "otherapptype=%d\r\n", lOtherapptype);
                fprintf(fp, "otherapp=%s\r\n", caOtherapp);
                fprintf(fp, "bkother=%d\r\n", lBkother);
                fprintf(fp, "exptype=%d\r\n", lExptype);
                fprintf(fp, "fileexp=%s\r\n", caFileexp);
                fprintf(fp, "fileexpmode=%d\r\n", lFileexpmode);
                fprintf(fp, "filesize=%d\r\n", lFilesize);
                fprintf(fp, "controlcode=%d\r\n", lControlcode);

            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            lOtherapptype = 0;
            memset(caOtherapp, 0, sizeof(caOtherapp));
            lBkother = 0;
            lExptype = 0;
            memset(caFileexp, 0, sizeof(caFileexp));
            lFileexpmode = 0;
            lFilesize = 0;
            lControlcode = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lOtherapptype,
                                     UT_TYPE_STRING, 23, caOtherapp,
                                     UT_TYPE_ULONG, 4, &lBkother,
                                     UT_TYPE_ULONG, 4, &lExptype,
                                     UT_TYPE_STRING, 23, caFileexp,
                                     UT_TYPE_ULONG, 4, &lFileexpmode,
                                     UT_TYPE_ULONG, 4, &lFilesize,
                                     UT_TYPE_ULONG, 4, &lControlcode,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }

    //生成HTTPs外发文件策略
    printf("生成HTTPs外发文件策略\n");

    psCur = pasDbOpenSqlF("select groupid,computerid,company,31,httpsip,httpsipmode,16,httpssoft,httpsmode,exptype,fileexp,fileexpmode,filesize,controlcode,flag,modtime,level from nwfilehttpsprocy order by company,groupid,computerid ");


    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lAddresstype, lAddressmode, lHttpstype, lHttpsmode, lExptype, lFileexpmode, lFilesize, lControlcode, lLevel;
        char caCompany[24], caHttpsip[24], caHttpssoft[24], caFileexp[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        lAddresstype = 0;
        memset(caHttpsip, 0, sizeof(caHttpsip));
        lAddressmode = 0;
        lHttpstype = 0;
        memset(caHttpssoft, 0, sizeof(caHttpssoft));
        lHttpsmode = 0;
        lExptype = 0;
        memset(caFileexp, 0, sizeof(caFileexp));
        lFileexpmode = 0;
        lFilesize = 0;
        lControlcode = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lAddresstype,
                                 UT_TYPE_STRING, 23, caHttpsip,
                                 UT_TYPE_ULONG, 4, &lAddressmode,
                                 UT_TYPE_ULONG, 4, &lHttpstype,
                                 UT_TYPE_STRING, 23, caHttpssoft,
                                 UT_TYPE_ULONG, 4, &lHttpsmode,
                                 UT_TYPE_ULONG, 4, &lExptype,
                                 UT_TYPE_STRING, 23, caFileexp,
                                 UT_TYPE_ULONG, 4, &lFileexpmode,
                                 UT_TYPE_ULONG, 4, &lFilesize,
                                 UT_TYPE_ULONG, 4, &lControlcode,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("Https外发文件策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }

            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "httpsoutfile.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "httpsapptype=%d\r\n", lHttpstype);
                fprintf(fp, "httpsapp=%s\r\n", caHttpssoft);
                fprintf(fp, "httpsappmode=%d\r\n", lHttpsmode);
                fprintf(fp, "addressnametype=%d\r\n", lAddresstype);
                fprintf(fp, "addressname=%s\r\n", caHttpsip);
                fprintf(fp, "addressmode=%d\r\n", lAddressmode);
                fprintf(fp, "exptype=%d\r\n", lExptype);
                fprintf(fp, "fileexp=%s\r\n", caFileexp);
                fprintf(fp, "fileexpmode=%d\r\n", lFileexpmode);
                fprintf(fp, "filesize=%d\r\n", lFilesize);
                fprintf(fp, "controlcode=%d\r\n", lControlcode);

            }


            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            lAddresstype = 0;
            memset(caHttpsip, 0, sizeof(caHttpsip));
            lAddressmode = 0;
            lHttpstype = 0;
            memset(caHttpssoft, 0, sizeof(caHttpssoft));
            lHttpsmode = 0;
            lExptype = 0;
            memset(caFileexp, 0, sizeof(caFileexp));
            lFileexpmode = 0;
            lFilesize = 0;
            lControlcode = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lAddresstype,
                                     UT_TYPE_STRING, 23, caHttpsip,
                                     UT_TYPE_ULONG, 4, &lAddressmode,
                                     UT_TYPE_ULONG, 4, &lHttpstype,
                                     UT_TYPE_STRING, 23, caHttpssoft,
                                     UT_TYPE_ULONG, 4, &lHttpsmode,
                                     UT_TYPE_ULONG, 4, &lExptype,
                                     UT_TYPE_STRING, 23, caFileexp,
                                     UT_TYPE_ULONG, 4, &lFileexpmode,
                                     UT_TYPE_ULONG, 4, &lFilesize,
                                     UT_TYPE_ULONG, 4, &lControlcode,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }


    //生成放行和禁止策略
    printf("生成放行和禁止策略\n");

    psCur = pasDbOpenSqlF("select groupid,computerid,company,freeapptype,freeapp,freeservertype,freeserver,blockapptype,blockapp,blockservertype,blockserver,flag,modtime,level from nwfileblockfreeprocy \
     order by company,groupid,computerid ");


    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lFreeapptype, lFreeservertype, lBlockapptype, lBlockservertype, lLevel;
        char caCompany[24], caFreeapp[24], caFreeserver[24], caBlockapp[24], caBlockserver[24];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");
        lId = 0;
        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        memset(caFreeapp, 0, sizeof(caFreeapp));
        memset(caFreeserver, 0, sizeof(caFreeserver));
        memset(caBlockserver, 0, sizeof(caBlockserver));
        memset(caBlockapp, 0, sizeof(caBlockapp));
        lFreeapptype = 0;
        lFreeservertype = 0;
        lBlockapptype = 0;
        lBlockservertype = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lFreeapptype,
                                 UT_TYPE_STRING, 23, caFreeapp,
                                 UT_TYPE_ULONG, 4, &lFreeservertype,
                                 UT_TYPE_STRING, 23, caFreeserver,
                                 UT_TYPE_ULONG, 4, &lBlockapptype,
                                 UT_TYPE_STRING, 23, caBlockapp,
                                 UT_TYPE_ULONG, 4, &lBlockservertype,
                                 UT_TYPE_STRING, 23, caBlockserver,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("放行禁止文件策略生成\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "blackfree.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "level=%d\r\n", lLevel);
                fprintf(fp, "freeapptype=%d\r\n", lFreeapptype);
                fprintf(fp, "freeapp=%s\r\n", caFreeapp);
                fprintf(fp, "freeservertype=%d\r\n", lFreeservertype);
                fprintf(fp, "freeserver=%s\r\n", caFreeserver);
                fprintf(fp, "blockapptype=%d\r\n", lBlockapptype);
                fprintf(fp, "blockapp=%s\r\n", caBlockapp);
                fprintf(fp, "blockservertype=%d\r\n", lBlockservertype);
                fprintf(fp, "blockserver=%s\r\n", caBlockserver);


            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            memset(caFreeapp, 0, sizeof(caFreeapp));
            memset(caFreeserver, 0, sizeof(caFreeserver));
            memset(caBlockserver, 0, sizeof(caBlockserver));
            memset(caBlockapp, 0, sizeof(caBlockapp));
            lFreeapptype = 0;
            lFreeservertype = 0;
            lBlockapptype = 0;
            lBlockservertype = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;

            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lFreeapptype,
                                     UT_TYPE_STRING, 23, caFreeapp,
                                     UT_TYPE_ULONG, 4, &lFreeservertype,
                                     UT_TYPE_STRING, 23, caFreeserver,
                                     UT_TYPE_ULONG, 4, &lBlockapptype,
                                     UT_TYPE_STRING, 23, caBlockapp,
                                     UT_TYPE_ULONG, 4, &lBlockservertype,
                                     UT_TYPE_STRING, 23, caBlockserver,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }



    //生成黑白名单策略
    sprintf(caTemp, "select groupid,computeid,userid,blacktype,blackname,snap,flag,updatetime,userlevel from blacklistprocy order by flag desc,userid,groupid,computeid ");
    pasLogs(PAS_SRCFILE, 1000, "caTemp=%s", caTemp);
    psCur = pasDbOpenSql(caTemp, 0);
    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lLevel, lUpdatetime, lUserlevel, lSnap;
        char caCompany[24], caBlacktype[32], caBlackname[32];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");

        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        memset(caBlacktype, 0, sizeof(caBlacktype));
        memset(caBlackname, 0, sizeof(caBlackname));
        lUpdatetime = 0;
        lUserlevel = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_STRING, 31, caBlacktype,
                                 UT_TYPE_STRING, 31, caBlackname,
                                 UT_TYPE_LONG, 4, &lSnap,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);

        printf("black list procy\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }

            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "blacklist.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
                pasLogs(PAS_SRCFILE, 1000, "genblackfile,file=%s", path);

            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                pasLogs(PAS_SRCFILE, 1000, "genblackfile,file=%s,blackname=%s", path, caBlackname);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "blacktype=%s\r\n", caBlacktype);
                fprintf(fp, "blackname=%s\r\n", caBlackname);
                fprintf(fp, "companyname=%s\r\n", caCompany);
                fprintf(fp, "snap=%lu\r\n", lSnap);
                fprintf(fp, "level=%d\r\n", lLevel);
            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            memset(caBlacktype, 0, sizeof(caBlacktype));
            memset(caBlackname, 0, sizeof(caBlackname));
            lUpdatetime = 0;
            lUserlevel = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_STRING, 31, caBlacktype,
                                     UT_TYPE_STRING, 31, caBlackname,
                                     UT_TYPE_LONG, 4, &lSnap,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }



    //生成文件策略
    sprintf(caTemp, "select groupid,compid,company,filename,controlmark,blocktime,timeid,type,snap,flag,updatetime,userlevel from nwfileprocy order by flag desc,company,compid ");
    psCur = pasDbOpenSql(caTemp, 0);
    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lLevel, lUpdatetime, lUserlevel, lSnap, lControlmark, lBlocktime, lTimeid, lType;
        char caCompany[24], caFile[256];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");

        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        memset(caFilename, 0, sizeof(caFilename));
        lControlmark = 0;
        lTimeid = 0;
        lType = 0;
        lUpdatetime = 0;
        lUserlevel = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_STRING, 250, caFilename,
                                 UT_TYPE_LONG, 4, &lControlmark,
                                 UT_TYPE_LONG, 4, &lBlocktime,
                                 UT_TYPE_LONG, 4, &lTimeid,
                                 UT_TYPE_LONG, 4, &lType,
                                 UT_TYPE_LONG, 4, &lSnap,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("文件策略\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {

            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);
                    //  printf("aaaaaaaaaa\n");
                }

                //  printf("000lLevel=%lu\n",lLevel);
                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "File_law.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "pid=1\r\n");
                fprintf(fp, "timeid=%lu\r\n", lTimeid);
                fprintf(fp, "filename=%s\r\n", caFilename);
                fprintf(fp, "optype=%lu\r\n", lControlmark);
                fprintf(fp, "snap=%lu\r\n", lSnap);
                //            printf("222222lLevel=%lu\n",lLevel);
                fprintf(fp, "level=%lu\r\n", lLevel);
                fprintf(fp, "type=%lu\r\n", lType);



            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            memset(caFilename, 0, sizeof(caFilename));
            lControlmark = 0;
            lTimeid = 0;
            lType = 0;
            lUpdatetime = 0;
            lUserlevel = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_STRING, 250, caFilename,
                                     UT_TYPE_LONG, 4, &lControlmark,
                                     UT_TYPE_LONG, 4, &lBlocktime,
                                     UT_TYPE_LONG, 4, &lTimeid,
                                     UT_TYPE_LONG, 4, &lType,
                                     UT_TYPE_LONG, 4, &lSnap,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }


    //生成外联策略
    sprintf(caTemp, "select groupid,compid,company,controlmark,blocktime,timeid,type,snap,flag,updatetime,userlevel from nwoutlinkprocy order by flag desc,company,compid ");
    psCur = pasDbOpenSql(caTemp, 0);
    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lLevel, lUpdatetime, lUserlevel, lSnap, lControlmark, lBlocktime, lTimeid, lType;
        char caCompany[24], caFile[256];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");

        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        memset(caFilename, 0, sizeof(caFilename));
        lControlmark = 0;
        lTimeid = 0;
        lType = 0;
        lUpdatetime = 0;
        lUserlevel = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_LONG, 4, &lControlmark,
                                 UT_TYPE_LONG, 4, &lBlocktime,
                                 UT_TYPE_LONG, 4, &lTimeid,
                                 UT_TYPE_LONG, 4, &lType,
                                 UT_TYPE_LONG, 4, &lSnap,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("外联策略\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "Outconnect_law.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "pid=1\r\n");
                fprintf(fp, "timeid=%lu\r\n", lTimeid);
                fprintf(fp, "optype=%lu\r\n", lControlmark);
                fprintf(fp, "snap=%lu\r\n", lSnap);
                fprintf(fp, "level=%lu\r\n", lLevel);
                fprintf(fp, "type=%lu\r\n", lType);



            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            memset(caFilename, 0, sizeof(caFilename));
            lControlmark = 0;
            lTimeid = 0;
            lType = 0;
            lUpdatetime = 0;
            lUserlevel = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_LONG, 4, &lControlmark,
                                     UT_TYPE_LONG, 4, &lBlocktime,
                                     UT_TYPE_LONG, 4, &lTimeid,
                                     UT_TYPE_LONG, 4, &lType,
                                     UT_TYPE_LONG, 4, &lSnap,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }


    //生成应用程序策略
    sprintf(caTemp, "select groupid,compid,company,controlmark,blocktime,timeid,type,snap,flag,updatetime,userlevel,apptype,runtime from nwappprocy order by flag desc,company,groupid,compid ");
    psCur = pasDbOpenSql(caTemp, 0);
    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lLevel, lUpdatetime, lUserlevel, lSnap, lControlmark, lBlocktime, lTimeid, lType, lApptype, lRuntime;
        char caCompany[24], caFile[256];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");

        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        memset(caFilename, 0, sizeof(caFilename));
        lControlmark = 0;
        lTimeid = 0;
        lType = 0;
        lUpdatetime = 0;
        lUserlevel = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;
        lApptype = 0;
        lRuntime = 0;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_LONG, 4, &lControlmark,
                                 UT_TYPE_LONG, 4, &lBlocktime,
                                 UT_TYPE_LONG, 4, &lTimeid,
                                 UT_TYPE_LONG, 4, &lType,
                                 UT_TYPE_LONG, 4, &lSnap,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel,
                                 UT_TYPE_ULONG, 4, &lApptype,
                                 UT_TYPE_ULONG, 4, &lRuntime);



        printf("应用程序策略\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "ProcessType_law.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "timeid=%lu\r\n", lTimeid);
                fprintf(fp, "pid=1\r\n");
                fprintf(fp, "optype=%lu\r\n", lControlmark);
                fprintf(fp, "snap=%lu\r\n", lSnap);
                fprintf(fp, "gtype=%lu\r\n", lApptype);
                fprintf(fp, "stamp=%lu\r\n", lRuntime);
                fprintf(fp, "level=%lu\r\n", lLevel);
                fprintf(fp, "type=%lu\r\n", lType);



            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            memset(caFilename, 0, sizeof(caFilename));
            lControlmark = 0;
            lTimeid = 0;
            lType = 0;
            lUpdatetime = 0;
            lUserlevel = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;
            lApptype = 0;
            lRuntime = 0;
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_LONG, 4, &lControlmark,
                                     UT_TYPE_LONG, 4, &lBlocktime,
                                     UT_TYPE_LONG, 4, &lTimeid,
                                     UT_TYPE_LONG, 4, &lType,
                                     UT_TYPE_LONG, 4, &lSnap,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel,
                                     UT_TYPE_ULONG, 4, &lApptype,
                                     UT_TYPE_ULONG, 4, &lRuntime);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }


    //生成外设策略
    sprintf(caTemp, "select groupid,compid,company,controlmark,blocktime,timeid,type,snap,flag,updatetime,userlevel,usbid,controlmark2 from nwdeviceprocy order by flag desc,company,groupid,compid ");
    psCur = pasDbOpenSql(caTemp, 0);
    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lLevel, lUpdatetime, lUserlevel, lSnap, lControlmark, lBlocktime, lTimeid, lType, lUsbid, lControlmark2;
        char caCompany[24], caFile[256];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");

        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        memset(caFilename, 0, sizeof(caFilename));
        lControlmark = 0;
        lTimeid = 0;
        lType = 0;
        lUpdatetime = 0;
        lUserlevel = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;
        lUsbid = 0;
        lControlmark2 = 0;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_LONG, 4, &lControlmark,
                                 UT_TYPE_LONG, 4, &lBlocktime,
                                 UT_TYPE_LONG, 4, &lTimeid,
                                 UT_TYPE_LONG, 4, &lType,
                                 UT_TYPE_LONG, 4, &lSnap,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel,
                                 UT_TYPE_ULONG, 4, &lUsbid,
                                 UT_TYPE_ULONG, 4, &lControlmark2);



        printf("USB策略\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "usb_law.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "timeid=%lu\r\n", lTimeid);
                fprintf(fp, "pid=1\r\n");
                fprintf(fp, "optype=%lu\r\n", lControlmark + lControlmark2);
                fprintf(fp, "snap=%lu\r\n", lSnap);
                fprintf(fp, "freehaordid=%lu\r\n", lUsbid);
                fprintf(fp, "level=%lu\r\n", lLevel);
                fprintf(fp, "type=%lu\r\n", lType);



            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            memset(caFilename, 0, sizeof(caFilename));
            lControlmark = 0;
            lTimeid = 0;
            lType = 0;
            lUpdatetime = 0;
            lUserlevel = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;
            lUsbid = 0;
            lControlmark2 = 0;
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_LONG, 4, &lControlmark,
                                     UT_TYPE_LONG, 4, &lBlocktime,
                                     UT_TYPE_LONG, 4, &lTimeid,
                                     UT_TYPE_LONG, 4, &lType,
                                     UT_TYPE_LONG, 4, &lSnap,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel,
                                     UT_TYPE_ULONG, 4, &lUsbid,
                                     UT_TYPE_ULONG, 4, &lControlmark2);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }


    //生成网址策略
    sprintf(caTemp, "select groupid,compid,company,keywords,blocktime,timeid,type,snap,flag,updatetime,userlevel from nwurlprocy order by flag desc,company,groupid,compid ");
    psCur = pasDbOpenSql(caTemp, 0);
    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lLevel, lUpdatetime, lUserlevel, lSnap, lControlmark, lBlocktime, lTimeid, lType;
        char caCompany[24], caKeywords[256];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");

        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        memset(caKeywords, 0, sizeof(caKeywords));
        lControlmark = 0;
        lTimeid = 0;
        lType = 0;
        lUpdatetime = 0;
        lUserlevel = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_STRING, 250, caKeywords,
                                 UT_TYPE_LONG, 4, &lBlocktime,
                                 UT_TYPE_LONG, 4, &lTimeid,
                                 UT_TYPE_LONG, 4, &lType,
                                 UT_TYPE_LONG, 4, &lSnap,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel);



        printf("网址策略\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }

            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "webfilter_law.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "pid=1\r\n");
                fprintf(fp, "usbclass=0\r\n");
                fprintf(fp, "url=%s\r\n", caKeywords);
                fprintf(fp, "optype=1\r\n"),
                        fprintf(fp, "snap=%lu\r\n", lSnap);
                fprintf(fp, "timeid=%lu\r\n", lTimeid);
                fprintf(fp, "level=%lu\r\n", lLevel);
                fprintf(fp, "type=%lu\r\n", lType);

            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            memset(caKeywords, 0, sizeof(caKeywords));
            lControlmark = 0;
            lTimeid = 0;
            lType = 0;
            lUpdatetime = 0;
            lUserlevel = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_STRING, 250, caKeywords,
                                     UT_TYPE_LONG, 4, &lBlocktime,
                                     UT_TYPE_LONG, 4, &lTimeid,
                                     UT_TYPE_LONG, 4, &lType,
                                     UT_TYPE_LONG, 4, &lSnap,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }



    //生成网络策略
    sprintf(caTemp, "select groupid,compid,company,blocktime,timeid,type,snap,flag,updatetime,userlevel,port,controlmark from nwnetprocy order by flag desc,company,groupid,compid ");
    psCur = pasDbOpenSql(caTemp, 0);
    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lLevel, lUpdatetime, lUserlevel, lSnap, lControlmark, lBlocktime, lTimeid, lType;
        char caCompany[24], caPort[256];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");

        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        memset(caPort, 0, sizeof(caPort));
        lControlmark = 0;
        lTimeid = 0;
        lType = 0;
        lUpdatetime = 0;
        lUserlevel = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_LONG, 4, &lBlocktime,
                                 UT_TYPE_LONG, 4, &lTimeid,
                                 UT_TYPE_LONG, 4, &lType,
                                 UT_TYPE_LONG, 4, &lSnap,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel,
                                 UT_TYPE_STRING, 200, caPort,
                                 UT_TYPE_ULONG, 4, &lControlmark);



        printf("网络策略\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }
            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {
                    sprintf(path, "%sG%lu/", ppath, lGroupid);

                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "Port_law.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "pid=1\r\n");
                fprintf(fp, "optype=1\r\n"),

                        fprintf(fp, "optype=1\r\n");
                fprintf(fp, "port=%s\r\n", caPort);
                fprintf(fp, "trantype=%lu\r\n", lControlmark);

                fprintf(fp, "snap=%lu\r\n", lSnap);
                fprintf(fp, "timeid=%lu\r\n", lTimeid);
                fprintf(fp, "level=%lu\r\n", lLevel);
                fprintf(fp, "type=%lu\r\n", lType);

            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            memset(caPort, 0, sizeof(caPort));
            lControlmark = 0;
            lTimeid = 0;
            lType = 0;
            lUpdatetime = 0;
            lUserlevel = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_LONG, 4, &lBlocktime,
                                     UT_TYPE_LONG, 4, &lTimeid,
                                     UT_TYPE_LONG, 4, &lType,
                                     UT_TYPE_LONG, 4, &lSnap,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel,
                                     UT_TYPE_STRING, 200, caPort,
                                     UT_TYPE_ULONG, 4, &lControlmark);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }


    //生成日志传输策略
    sprintf(caTemp, "select groupid,compid,nwlogprocy.company,controlmark,timelimit,timeid,type,snap,nwlogprocy.flag,updatetime,userlevel,paramsmanage.keyname,timelimit2 from nwlogprocy left join paramsmanage on paramsmanage.recno=nwlogprocy.controlmark2 order by nwlogprocy.flag desc,nwlogprocy.company,groupid,compid ");
    psCur = pasDbOpenSql(caTemp, 0);
    if(psCur)
    {
        unsigned long lGroupid, lComputerid, lLevel, lUpdatetime, lUserlevel, lSnap, lControlmark, lBlocktime, lTimeid, lType, lTimelimit, lTimelimit2;
        char caCompany[24], caKeyname[256];
        unsigned long flag;
        char caNowfield[128];
        strcpy(caPrecompany, "-1");

        iNum = 0;
        lGroupid = 0;
        lComputerid = 0;
        memset(caCompany, 0, sizeof(caCompany));
        memset(caKeyname, 0, sizeof(caKeyname));
        lControlmark = 0;
        lTimelimit = 0;
        lTimelimit2 = 0;
        lTimeid = 0;
        lType = 0;
        lUpdatetime = 0;
        lUserlevel = 0;
        flag = 0;
        lModtime = 0;
        lLevel = 0;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lComputerid,
                                 UT_TYPE_STRING, 23, caCompany,
                                 UT_TYPE_ULONG, 4, &lControlmark,
                                 UT_TYPE_ULONG, 4, &lTimelimit,
                                 UT_TYPE_LONG, 4, &lTimeid,
                                 UT_TYPE_LONG, 4, &lType,
                                 UT_TYPE_LONG, 4, &lSnap,
                                 UT_TYPE_ULONG, 4, &flag,
                                 UT_TYPE_ULONG, 4, &lModtime,
                                 UT_TYPE_ULONG, 4, &lLevel,
                                 UT_TYPE_STRING, 200, caKeyname,
                                 UT_TYPE_ULONG, 4, &lTimelimit2);



        printf("网络策略\n");
        //目录0 表示公司级 G123456 部门级 C123456计算机级
        while(iReturn == 0 || iReturn == 1405)
        {
            if(lGroupid == 0 && lComputerid == 0)
            {
                lLevel = 0;
            }
            else if(lGroupid != 0 && lComputerid == 0)
            {
                lLevel = 1;
            }
            else if(lComputerid != 0)
            {
                lLevel = 2;
            }

            // #printf(" LoadUser:%lu %s \n",s.compid,s.compname);
            if(strlen(caCompany) == 0)
            {
                strcpy(caCompany, "0");
            }

            if(lModtime > lLasttime_procy)
            {
                lLasttime_procy = lModtime;
            }
            sprintf(caNowfield, "%s_%lu_%lu", caCompany, lGroupid, lComputerid);

            if(strcmp(caNowfield, caPrecompany) != 0)
            {
                strcpy(caPrecompany, caNowfield);
                if(lGroupid == 0 && lComputerid == 0)
                {

                    sprintf(path, "%s%s/", ppath, caCompany);
                }
                else if(lGroupid != 0 && lComputerid == 0)
                {

                    sprintf(path, "%sG%lu/", ppath, lGroupid);
                }
                else if(lComputerid != 0)
                {

                    sprintf(path, "%sC%lu/", ppath, lComputerid);

                }


                if(!utFileIsExist(path))
                {
                    char tmp[64] = "";
                    sprintf(tmp, "mkdir -p %s", path);
                    system(tmp);
                }

                sprintf(path + strlen(path), "log_law.ini");
                if(iNum > 0)
                {
                    fclose(fp);
                }
                fp = fopen(path, "w");
                iNum++;
                iIndex = 0;
            }
            iIndex++;
            //            printf("path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
            if(flag == 0 && fp)
            {
                //               printf("111path=%s,flag=%d,hostmail=%s\n",path,flag,caHostmail);
                fprintf(fp, "[law%d]\r\n", iIndex - 1);
                fprintf(fp, "pid=1\r\n");
                fprintf(fp, "timelimit=%lu\r\n", lTimelimit);
                fprintf(fp, "optype=%lu\r\n", lControlmark);
                fprintf(fp, "timeid=%lu\r\n", lTimeid);
                fprintf(fp, "level=%lu\r\n", lLevel);
                fprintf(fp, "type=%lu\r\n", lType);
                fprintf(fp, "proctype=%s\r\n", caKeyname);
                fprintf(fp, "scrntime=%lu\r\n", lTimelimit2);

            }

            lGroupid = 0;
            lComputerid = 0;
            memset(caCompany, 0, sizeof(caCompany));
            memset(caKeyname, 0, sizeof(caKeyname));
            lControlmark = 0;
            lTimelimit = 0;
            lTimelimit2 = 0;
            lTimeid = 0;
            lType = 0;
            lUpdatetime = 0;
            lUserlevel = 0;
            flag = 0;
            lModtime = 0;
            lLevel = 0;
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lComputerid,
                                     UT_TYPE_STRING, 23, caCompany,
                                     UT_TYPE_ULONG, 4, &lControlmark,
                                     UT_TYPE_ULONG, 4, &lTimelimit,
                                     UT_TYPE_LONG, 4, &lTimeid,
                                     UT_TYPE_LONG, 4, &lType,
                                     UT_TYPE_LONG, 4, &lSnap,
                                     UT_TYPE_ULONG, 4, &flag,
                                     UT_TYPE_ULONG, 4, &lModtime,
                                     UT_TYPE_ULONG, 4, &lLevel,
                                     UT_TYPE_STRING, 200, caKeyname,
                                     UT_TYPE_ULONG, 4, &lTimelimit2);

        }
        pasDbCloseCursor(psCur);
        if(iNum > 0)
        {
            fclose(fp);
        }
    }




    sprintf(caTemp, "%lu", lLasttime_base);
    utComSetVar_s(psShmHead, "bptime", caTemp);

    sprintf(caTemp, "%lu", lLasttime_procy);
    utComSetVar_s(psShmHead, "ptime", caTemp);


    //   printf("black=%s\n",lanGetProcyFileByCompid(psShmHead,2519515015,"blackfree.ini","pronetway"));
    //   printf("chatoutfile.ini=%s\n",lanGetProcyFileByCompid(psShmHead,2519515015,"chatoutfile.ini","pronetway"));
    //     printf("sfftime=%lu\n",lanGetFileTime(lanGetProcyFileByCompid(psShmHead,2519515015,"chatoutfile.ini","pronetway")));
    return utShmHashRecord(psShmHead, NC_LNK_NW_PROCY);

}

int lanUtlFreeProcy(utShmHead *psShmHead)
{
    /*
        long lRecord;
         char *pHash;
         pasHashInfo sHashInfo;
         lanPmain        *pData;
         pHash = (char *)utShmHashHead(psShmHead,NC_LNK_NW_PROCY);
        if(pHash == NULL) {
          utShmFreeHash(psShmHead,NC_LNK_NW_PROCY);
            return 0;
        }

          pData  = (lanPmain *)pasHashFirst(pHash,&sHashInfo);
        while(pData) {
             if(pData->sumfile>0){
                 pData->sumfile=0;
                 free(pData->psFile);
             }
             if(pData->sumdev>0){
                 pData->sumdev=0;
                 free(pData->psDev);
             }
             if(pData->sumlink>0){
                pData->sumlink=0;
                free(pData->psLink);
            }
            if(pData->sumptype>0){
                pData->sumptype=0;
                free(pData->psPtype);
            }
            if(pData->sumpname>0){
                pData->sumpname=0;
                free(pData->psPname);
                }
                if(pData->sumnet>0){
                    pData->sumnet=0;
                    free(pData->psPnet);
                }
                if(pData->sumurl>0){
                    pData->sumurl=0;
                    free(pData->psPurl);
                }
                if(pData->sumoth>0){
                    pData->sumoth=0;
                    free(pData->psPoth);
                }
            pData  = (lanPmain *)pasHashNext(&sHashInfo);
        }


    //  lRecord=utShmHashRecord(psShmHead,NC_LNK_NW_PROCY);
    //  printf("lRecord=%d\n",lRecord);


        utShmFreeHash(psShmHead,NC_LNK_NW_PROCY);
        */
    return 0;
}



int lanUtlPrintProcy(utShmHead *psShmHead)
{
    long lRecord, i;
    char *pHash;
    pasHashInfo sHashInfo;
    lanPmain        *pData;
    //   printf("aaaaaaaaaaaaaaaaa\n");
    pHash = (char *)utShmHashHead(psShmHead, NC_LNK_NW_PROCY);
    if(pHash == NULL)
    {
        //   utShmFreeHash(psShmHead,NC_LNK_NW_PROCY);
        //   printf("eeeeeeeeeeeeeeeeeeeeeeeeeeee\n");
        return 0;
    }
    //printf("bbbbbbbbbbbbbbbbbbbbbbbbb\n");
    pData  = (lanPmain *)pasHashFirst(pHash, &sHashInfo);
    while(pData)
    {
        //       printf("filesum=%d\n",pData->sumfile);
        if(pData->sumfile > 0)
        {

            for(i = 0; i < pData->sumfile; i++)
            {
                //              printf("i=%d,pid=%d,filename=%s\n",i,pData->psFile[i].pid,pData->psFile[i].filename);
            }
            if(pData->sumdev > 0)
            {

            }
            if(pData->sumlink > 0)
            {

            }
            if(pData->sumptype > 0)
            {

            }
            if(pData->sumpname > 0)
            {

            }

        }
        if(pData->sumurl > 0)
        {

        }
        if(pData->sumoth > 0)
        {

        }
        pData  = (lanPmain *)pasHashNext(&sHashInfo);
    }


    return 0;
}


/* 数据自动整理  */
int lanAutoClean(utShmHead *psShmHead)
{
    char sDate[12], sTime[12];
    unsigned long lKeep, lKeepBack, lMax, lAutoCleanTime;
    unsigned long lTime, lTime0, lStartTime, lThisTime;
    int iReturn;
    char caSdate[32], caStime[32], caSdate1[32], caPath[128], caFile[128];
    unsigned long lBackupTime;
    char caComd[64];
    char *pAuto, *p;



    iReturn = pasConnect(psShmHead);
    if(iReturn < 0)
    {
        pasLog(PAS_LOG_ERROR, "Start AutoClean, Database connect error");
        sleep(60);
        return 0;
    }
    pasLog(PAS_LOG_EVENT, "Start AutoClean Sucess Pid is %d", getpid());
    while(1)
    {
        strcpy(caComd, utComGetVar_sd(psShmHead, "imclean", ""));
        lAutoCleanTime = ncGetLastInfo_l("AutoCleanTime", 0);
        lTime = time(0);
        ncCvtStime(lTime, caSdate, caStime);
        ncCvtStime(lAutoCleanTime, caSdate1, caStime);
        pasLogs(PAS_SRCFILE, 1000, " LastCleanTime:%s   ThisCleanTime:%s ", caSdate, caSdate1);
        lStartTime = time(0);
        if((strcasecmp(caSdate, caSdate1) == 0) && strcmp(caComd, "clean") != 0) /* 当天已经做过备份  */
        {
            //          ncDelTempFile(psShmHead);          /* 删除临时文件   */
            //        sleep(3600);
            sleep(10);
            continue;
        }
        else
        {
            p = utComGetVar_sd(psShmHead, "CleanupTime", "00:00:00");

            if((ncUtlIsInSetTime(lTime, p) == 0) && strcmp(caComd, "clean") != 0)
            {
                /* 在指定时间的两个小时以外  */
                //            ncDelTempFile(psShmHead);     /* 删除临时文件   */
                sleep(10);
                continue;
            }
            else
            {
                //处理自动清理
                lKeep = utComGetVar_ld(psShmHead, "InstantKeepMon", 2);
                lanCleanTable(lKeep, "ncimclient");
                lKeep = utComGetVar_ld(psShmHead, "InstantBakKeep", 2);
                lanCleanBackFile(lKeep, "ncimclient");

                lKeep = utComGetVar_ld(psShmHead, "ScreenKeepMon", 2);
                lanCleanTable(lKeep, "ncscreenlog");
                lanCleanFile(lKeep, "/home/ncmysql/nw/screenJPG");
                lKeep = utComGetVar_ld(psShmHead, "ScreenBakKeep", 2);
                lanCleanBackFile(lKeep, "ncscreen");
                lanCleanBackFile(lKeep, "screenfile");

                lKeep = utComGetVar_ld(psShmHead, "WebKeepMon", 2);
                lanCleanTable(lKeep, "nwweblog");
                lKeep = utComGetVar_ld(psShmHead, "WebBakKeep", 2);
                lanCleanBackFile(lKeep, "nwweblog");

                lKeep = utComGetVar_ld(psShmHead, "ProcKeepMon", 2);
                lanCleanTable(lKeep, "nwproclog");
                lanCleanTable(lKeep, "nwprocscreen");
                lanCleanFile(lKeep, "/home/ncmysql/nw/processJPG");
                lKeep = utComGetVar_ld(psShmHead, "ProcBakKeep", 2);
                lanCleanBackFile(lKeep, "nwproclog");

                lKeep = utComGetVar_ld(psShmHead, "DevKeepMon", 2);
                lanCleanTable(lKeep, "nwremovelog");
                lKeep = utComGetVar_ld(psShmHead, "DevBakKeep", 2);
                lanCleanBackFile(lKeep, "nwremovelog");


                lKeep = utComGetVar_ld(psShmHead, "FileKeepMon", 2);
                lanCleanTable(lKeep, "nwfilelog");
                lKeep = utComGetVar_ld(psShmHead, "FileBakKeep", 2);
                lanCleanBackFile(lKeep, "nwfilelog");


                lKeep = utComGetVar_ld(psShmHead, "WarnKeepMon", 2);
                lanCleanTable(lKeep, "nwwarnlog");
                lanCleanTable(lKeep, "nwwarnscreen");
                lanCleanFile(lKeep, "/home/ncmysql/nw/screenAlterJPG");
                lKeep = utComGetVar_ld(psShmHead, "WarnBakKeep", 2);
                lanCleanBackFile(lKeep, "nwwarnlog");
                lanCleanBackFile(2, "nwbase");

                utComSetVar_s(psShmHead, "imclean", "");
            }
        }

        //       lThisTime = time(0);
        //       if(lThisTime < lStartTime + 3600) {
        //           sleep(lStartTime + 3600 - lThisTime);
        //       }
        sleep(10);
    }
    return 0;
}

void getNwBasename(char *fullname,  char *basename, int maxlen)
{
    char *p = fullname;
    char *p1 = NULL;
    while((p1 = strstr(p, "/")) != NULL)
    {
        p = p1 + 1;
    }
    memcpy(basename, p, maxlen);

}

void getNwDirName(char *fullname,  char *dirname, int maxlen)
{
    char *p = fullname;
    char *p1 = NULL;
    while((p1 = strstr(p, "/")) != NULL)
    {
        p = p1 + 1;
    }
    p1 = strstr(fullname, "/home/ncmysql/nw/");
    p1 = p1 + strlen("/home/ncmysql/nw/");


    if(p - p1 <= maxlen)
    {
        memcpy(dirname, p1, p - p1 - 1);
        dirname[p - p1 - 1] = '\0';
    }

}


void sendNwFtpFile(char *locale, netbuf *conn)
{
    struct stat stbuf;
    if(stat(locale, &stbuf) == -1)
    {
        fprintf(stdout, "fsize: can not access %s\n", locale);
        return;
    }
    if((stbuf.st_mode & S_IFMT) == S_IFDIR)
    {
        dirwalk(locale, conn, sendNwFtpFile);
    }
    else
    {
        int iRet = 0;
        int iReturn = 0;


        char dirname[128];
        getNwDirName(locale, dirname, 128);//得到目录

        char basename[125];
        getNwBasename(locale,  basename, 128);//得到文件名
        printf("local=%s,basename=%s,dirname=%s\n", locale, basename, dirname);
        iRet = FtpChdir(dirname, conn);
        if(iRet)//Success
        {
            iReturn = FtpPut(locale, basename, 'I', conn);
        }
        else//Failed
        {
            iReturn = FtpMkdir(dirname, conn);
            if(iReturn)//Mkdir Success
            {
                iReturn = FtpChdir(dirname, conn);
                if(iReturn)//Change dir Success
                {
                    iReturn = FtpPut(locale, basename, 'I', conn);
                }
            }
        }

        iRet = FtpChdir("/", conn);
    }
}


/* 数据自动备份  */
int lanAutoBack(utShmHead *psShmHead)
{
    char caSdate[32];
    char caSdate1[32];
    char caStime[32];
    char caComd[64];
    long lAutoBackTime;
    long lTime, lStartTime;
    long iReturn, lBackScreenTime, i;
    char caBtime[20];
    char *pAuto, *p;
    iReturn = pasConnect(psShmHead);
    if(iReturn < 0)
    {
        ncSysLog(NC_LOG_ERROR, "Start AutoBackup, Database connect error");
        sleep(60);
        return 0;
    }
    while(1)
    {
        if(strcasecmp(utComGetVar_sd(psShmHead, "StartBack", "Yes"), "Yes") == 0)
        {
            strcpy(caComd, utComGetVar_sd(psShmHead, "imback", ""));
            lAutoBackTime = ncGetLastInfo_l("BackTime", 0);
            lTime = time(0);
            ncCvtStime(lTime, caSdate, caStime);
            ncCvtStime(lAutoBackTime, caSdate1, caStime);
            pasLogs(PAS_SRCFILE, 1000, " LastCleanTime:%s   ThisCleanTime:%s ", caSdate, caSdate1);
            lStartTime = time(0);
            if((strcasecmp(caSdate, caSdate1) == 0) && strcmp(caComd, "back") != 0) /* 当天已经做过备份  */
            {

                sleep(10);
                continue;
            }
            else
            {
                p = utComGetVar_sd(psShmHead, "BackTime", "23:00:00");

                if((ncUtlIsInSetTime(lTime, p) == 0) && strcmp(caComd, "back") != 0)
                {
                    sleep(10);
                    continue;
                }
                else
                {
                    //备份
                    if(strcasecmp(utComGetVar_sd(psShmHead, "BackBaseDB", "Yes"), "YES") == 0)
                    {
                        lanUtlBackupBaseInfo(psShmHead);
                    }
                    if(strcasecmp(utComGetVar_sd(psShmHead, "BackScreen", "Yes"), "YES") == 0)
                    {
                        lBackScreenTime = ncGetLastInfo_l("BackScreenTime", 0);
                        if(lBackScreenTime < lTime - 10 * 3600 * 24)
                        {
                            lBackScreenTime = lTime - 10 * 3600 * 24;
                        }
                        strcpy(caBtime, utTimFormat("%Y%m%d", lBackScreenTime));
                        lBackScreenTime = utTimStrToLong("%Y%m%d", caBtime);

                        for(i = lBackScreenTime; i < lTime - 3600 * 24; i = i + 86400)
                        {
                            strcpy(caBtime, utTimFormat("%Y%m%d", i));
                            lanBackScreenFile(caBtime, "/home/ncmysql/nw/screenJPG");
                            ncSetLastInfo_l("BackScreenTime", i);
                        }                                     //备份屏幕文件
                    }
                    //远程备份
                    if(strcasecmp(utComGetVar_sd(psShmHead, "StartRmBack", "No"), "FtpBackup") == 0) /* 启用远端FTP备份  */
                    {


                        char caHost[64];
                        char caUser[64];
                        char caPass[64];
                        char caTemp[1024];
                        long lBackRemoteTime;
                        strcpy(caHost, utComGetVar_sd(psShmHead, "rip", "\0"));
                        strcpy(caUser, utComGetVar_sd(psShmHead, "ruser", "\0"));
                        strcpy(caPass, utComGetVar_sd(psShmHead, "rpass", "\0"));
                        netbuf *conn = ftpConnectLogin(caHost, caUser, caPass);
                        int iRet = 0;
                        if(conn != NULL)
                        {
                            if((iRet = FtpChdir("keep", conn)) == 0)//Change Directory Failed
                            {
                                iRet = FtpMkdir("keep", conn);
                                iRet = FtpChdir("keep", conn);
                            }

                            lBackRemoteTime = ncGetLastInfo_l("BackRemoteTime", 0);
                            if(lBackRemoteTime < lTime - 10 * 3600 * 24)
                            {
                                lBackRemoteTime = lTime - 10 * 3600 * 24;
                            }
                            strcpy(caBtime, utTimFormat("%Y%m%d", lBackRemoteTime));
                            lBackRemoteTime = utTimStrToLong("%Y%m%d", caBtime);

                            for(i = lBackRemoteTime; i < lTime - 3600 * 24; i = i + 86400)
                            {
                                strcpy(caBtime, utTimFormat("%Y%m%d", i));
                                sprintf(caTemp, "/home/ncmysql/nw/keep/%s", caBtime);
                                dirwalk(caTemp, conn, sendNwFtpFile);
                                ncSetLastInfo_l("BackRemoteTime", i);
                            }
                            iRet = FtpClose(conn);
                        }
                    }
                    utComSetVar_s(psShmHead, "imback", "");
                }
                ncSetLastInfo_l("BackTime", lTime);
            }

        }
        sleep(20);
    }
}

//清除表
int lanCleanTable(int lKeep, char *pTable)
{
    char caTemp[1024];
    pasDbCursor *psCur;
    char caTable[64];
    char caTables[100][64];
    char caTime[20], caKeepTable[64];
    long lTime;
    int iNum, lTable;
    int iReturn, i;
    sprintf(caTemp, "show tables like '%c%s%c' ", '%', pTable, '%');
    psCur = pasDbOpenSql(caTemp, 0);
    if(psCur == NULL)
    {
        return -1;
    }
    lTime = time(0);
    strcpy(caTime, utTimFormat("%Y%m", lTime));
    lTable = atol(caTime) - lKeep;
    sprintf(caKeepTable, "%s_%d", pTable, lTable);

    memset(caTable, 0, sizeof(caTable));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 63, caTable);
    iNum = 0;

    while(iReturn == 0)
    {
        if((strcmp(caTable, caKeepTable) < 0) && (strcmp(caTable, pTable) != 0))
        {
            strcpy(caTables[iNum], caTable);
            iNum++;
        }

        memset(caTable, 0, sizeof(caTable));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 63, caTable);
    }
    pasDbCloseCursor(psCur);
    for(i = 0; i < iNum; i++)
    {
        pasDbExecSqlF("drop table %s;", caTables[i]);
    }
    return 0;
}


int lanCleanFile(int lKeep, char *pDir)
{
    char    caFileName[256], caFileName2[256];
    struct  stat st, st1;
    struct dirent *d, *d2;
    char caKeepDir[64];
    DIR *dir, *dir2;
    int iReturn, lTable, lTime;
    char caTime[24];
    iReturn = stat(pDir, &st);
    if(iReturn != 0)  /* 目录不存在  */
    {
        return 0;
    }
    lTime = time(0);
    strcpy(caTime, utTimFormat("%Y%m", lTime));
    lTable = atol(caTime) - lKeep;
    sprintf(caKeepDir, "%d", lTable);


    if(S_ISDIR(st.st_mode) && (dir = opendir(pDir)))
    {
        while(d = readdir(dir))
        {
            if(d->d_name[0] == '.') continue;
            sprintf(caFileName, "%s/%s", pDir, d->d_name);
            if(stat(caFileName, &st1) == 0)
            {
                if(S_ISDIR(st1.st_mode) && (dir2 = opendir(caFileName)))
                {
                    while(d2 = readdir(dir2))
                    {
                        if(d2->d_name[0] == '.') continue;
                        sprintf(caFileName2, "%s/%s", caFileName, d2->d_name);
                        if(strcmp(d2->d_name, caKeepDir) < 0)
                        {

                            //            rmdir(caFileName2);
                            utFilRmDir(caFileName2);
                        }
                    }
                    closedir(dir2);
                }
            }
        }
        closedir(dir);
    }


    //rmdir(pDir);
    //utFilRmDir(caFileName);

    return 0;
}



/* 备份基础数据  */
int lanUtlBackupBaseInfo(utShmHead *psShmHead)
{
    char *pTime;
    char caUser[64], caPass[64], caDbname[64];
    char caTemp[1024];
    char caFile[128], caFile1[128];
    char caPath[128], caFname[64], caBack[128], caListFile[128];
    int iReturn;
    char *pBackDir;
    char caSdate[32], caStime[12];
    unsigned long lSdate, lEdate, lSize;
    pBackDir = utComGetVar_sd(psShmHead, "Load2DbBakPath", "/home/ncmysql/nw/keep");
    pTime = ncGetLastInfo("NcBaseBackDate", "2005/05/01");
    lSdate = ncCvtLongTime(caSdate, "00:00:00");

    strcpy(caUser, utComGetVar_sd(psShmHead, "dbuser", "nc"));
    strcpy(caPass, utComGetVar_sd(psShmHead, "dbpassword", "nc"));
    strcpy(caDbname, utComGetVar_sd(psShmHead, "dbname", "nc"));
    strcpy(caSdate, utTimFormat("%Y%m%d", lSdate));
    sprintf(caBack, "%s/%s", pBackDir, caSdate);
    if(utFileIsExist(caBack) != 1)
    {
        if(mkdir(caBack, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
        {
            ncSysLog(NC_LOG_ERROR, " Dir Mkerror %s", caBack);
            return (-1);
        }
    }
    //拷贝pqconf_nc.cnf
    sprintf(caTemp, "cp /home/ncmysql/nw/bin/pqconf_nc.cnf %s/.", caBack);
    system(caTemp);
    sprintf(caListFile, "%s/nwbase-%s.dmp", caBack, caSdate);
    sprintf(caTemp, "/usr/local/mysql/bin/mysqldump -u %s -p%s --add-drop-table ",
            caUser, caPass);
    utStrAddF(caTemp, "-r %s  %s  nwcompute nwgroup nwmainprocy nwprocyfile nwprocydev nwprocylink nwprocypgtype nclastinfo",
              caListFile, caDbname);
    utStrAddF(caTemp, " nwprocypname nwprocynet nwprocyurl nwprocyoth nwprocyref nwprocyobj nwdatedes nwlimdate nwsoftclass nwsoftlist nwusbclass nwusblist ncworktime ");
    utStrAddF(caTemp, " dsappsrv dssequen dsuser dsrole dspasrole dsroleattr dscltright dsuserrole ");
    iReturn = system(caTemp);
    //    printf("caTemp=%s\n",caTemp);
    if(iReturn == 0)
    {
        lSize = utFileSize(caListFile);
        sprintf(caFile1, "nwbase-%s.dmp", caSdate);
        //        ncMailWriteBackLog(caBack,caFile1,lSize,caSdate,1,"ncbase");
        //        ncSysLog(NC_LOG_EVENT,"ncbase Backup Ok  at %s\n",caListFile);
        ncSetLastInfo("NcBaseBackDate", caSdate);
    }
    return 0;
}



int lanBackScreenFile(char *pDate, char *pDir)
{
    char    caFileName[256], caFileName2[256];
    struct  stat st, st1;
    struct dirent *d, *d2;
    char caKeepDir[64];
    DIR *dir, *dir2;
    int iReturn, lTable, lTime;
    char caTime[24];
    char caTemp[1024], caComm[2048];
    long iNum, index;
    iReturn = stat(pDir, &st);
    if(iReturn != 0)  /* 目录不存在  */
    {
        return 0;
    }

    //   lTime=time(0);
    //    strcpy(caTime,utTimFormat("%Y%m",lTime));
    //    lTable=atol(caTime)-lKeep;
    //    sprintf(caKeepDir,"%d",lTable);


    if(S_ISDIR(st.st_mode) && (dir = opendir(pDir)))
    {
        iNum = 0;
        memset(caTemp, 0, sizeof(caTemp));
        index = 0;
        while(d = readdir(dir))
        {
            if(d->d_name[0] == '.') continue;
            sprintf(caFileName, "%s/%s", pDir, d->d_name);
            if(stat(caFileName, &st1) == 0)
            {
                if(S_ISDIR(st1.st_mode) && (dir2 = opendir(caFileName)))
                {
                    while(d2 = readdir(dir2))
                    {
                        if(d2->d_name[0] == '.') continue;
                        sprintf(caFileName2, "%s/%s", caFileName, d2->d_name);
                        if(strcmp(d2->d_name, pDate) == 0)
                        {
                            iNum++;
                            sprintf(caTemp + strlen(caTemp), " %s", caFileName2);
                            if(iNum > 10)
                            {
                                index++;
                                sprintf(caComm, "tar cvfz /home/ncmysql/nw/keep/%s/screenfile_%s_%d.tgz %s", pDate, pDate, index, caTemp);
                                system(caComm);
                                iNum = 0;
                                memset(caTemp, 0, sizeof(caTemp));
                            }

                        }
                    }
                    closedir(dir2);
                }
            }
        }

        if(iNum > 0)
        {
            index++;
            sprintf(caComm, "tar cvfz /home/ncmysql/nw/keep/%s/screenfile_%s_%d.tgz %s", pDate, pDate, index, caTemp);
            system(caComm);
            printf("caComm=%s\n", caComm);
            iNum = 0;
            memset(caTemp, 0, sizeof(caTemp));
        }
        closedir(dir);
    }


    //rmdir(pDir);
    //utFilRmDir(caFileName);

    return 0;
}


//删除备份文件
int lanCleanBackFile(int lKeep, char *pFile)
{
    char    caFileName[256], caFileName2[256];
    struct  stat st, st1;
    struct dirent *d, *d2;
    char caKeepDir[64];
    DIR *dir, *dir2;
    int iReturn, lTable, lTime;
    char caTime[24];
    char caDir[128];
    char caCback[128];
    long iNum;
    char caTemp[1024];
    sprintf(caDir, "/home/ncmysql/nw/keep");
    iReturn = stat(caDir, &st);
    if(iReturn != 0)  /* 目录不存在  */
    {
        return 0;
    }
    lTime = time(0);
    lTable = lTime - 30 * 3600 * 24 * lKeep;
    //    strcpy(caTime,utTimFormat("%Y%m",lTime));
    //    lTable=atol(caTime)-lKeep;
    strcpy(caCback, utTimFormat("%Y%m", lTable));
    //    sprintf(caCback,"%d",lTable);
    if(S_ISDIR(st.st_mode) && (dir = opendir(caDir)))
    {
        while(d = readdir(dir))
        {
            if(d->d_name[0] == '.') continue;
            sprintf(caFileName, "%s/%s", caDir, d->d_name);
            if(stat(caFileName, &st1) == 0)
            {
                //                   printf("caFileName=%s,caCback=%s\n",caFileName,caCback);
                if(strcmp(d->d_name, caCback) < 0)
                {
                    //                   printf("aaaaa\n");
                    if(S_ISDIR(st1.st_mode) && (dir2 = opendir(caFileName)))
                    {
                        iNum = 0;
                        while(d2 = readdir(dir2))
                        {
                            if(d2->d_name[0] == '.') continue;
                            if(strstr(d2->d_name, pFile))
                            {
                                sprintf(caTemp, "%s/%s", caFileName, d2->d_name);
                                remove(caTemp);
                            }
                            else
                            {
                                iNum++;
                            }
                        }
                        if(iNum == 0)
                        {
                            //  printf("caFilename2=%s\n",caFileName);
                            utFilRmDir(caFileName);
                        }
                    }


                }
            }
        }
        closedir(dir);
    }


    //rmdir(pDir);
    //utFilRmDir(caFileName);

    return 0;
}

//显示liecene数
int lan_showstatus(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    utPltDbHead *psDbHead;


    psDbHead = utPltInitDb();

    //  printf("caTemp11=%s\n",caTemp);
    char caProSn[32];
    unsigned long lExpire, lUsers, lStart, lCount;
    strcpy(caProSn, utComGetVar_sd(psShmHead, "ProductSN", ""));
    pasInstGetLicense(caProSn, &lExpire, &lUsers, &lStart);

    lCount = 0;

    pasDbOneRecord("select count(*) from nwcompute ", 0, UT_TYPE_LONG, 4, &lCount);
    utPltPutVar(psDbHead, "psn", caProSn);
    utPltPutVarF(psDbHead, "zccompute", "%lu", lCount);
    utPltPutVarF(psDbHead, "licuser", "%lu", lUsers);
    utPltPutVarF(psDbHead, "onuser", "%lu", utShmHashRecord(psShmHead, NC_LNK_ONLINECOMP));

    utPltPutVar(psDbHead, "nwversion", NWVERSION);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "nw/lan_show_firststatus.htm");

    return 0;
}
