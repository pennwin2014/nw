#define PAS_SRCFILE      3000
#include <stdio.h>
#include <stdlib.h>
#include "utoall.h"
#include "ncdef.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "nwdef.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


#define     WT_BIND_MACFILELENOLD       (1024*1024-256)
#define     WT_BIND_MACFILELEN64        (3*1024*1024-256)
#define     WT_BIND_MACFILELEN32        (3*1024*1024-256)

char *getDsGroupid();
char *getGroupid(long lGroupid);

//int wbs_setDateTime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
//将逗号分割的字符拆分成一个数组
static int sepcharbydh2(char *in, char caOut[200][256])
{
    char *p, *p2;
    long lCount;
    long m = 0;
    lCount = 0;
    if(strlen(in) == 0) return 0;
    p = in;
    while(*p)
    {
        //  printf("lCount=%d\n",lCount);
        if(*p == ',')
        {
            caOut[lCount][m] = '\0';
            m = 0;
            p++;
            lCount++;
            if(lCount >= 200) return lCount;
        }
        else
        {
            if(m < 256)
            {
                caOut[lCount][m] = *p;
                p++;
                m++;
            }
        }
    }
    return lCount + 1;
}


// 计算的MD5值

char * getFilemd5(char *pFile)
{
    static char caMd5[128];
    unsigned long lFilelen;
    FILE *fp;
    char md5buf[33] = "";
    char s_comm[128];
    memset(caMd5, 0, sizeof(caMd5));
    memset(md5buf, 0, sizeof(md5buf));
    snprintf(s_comm, sizeof(s_comm), "/usr/bin/md5sum %s", pFile);
    fp = popen(s_comm, "r");
    if(fp != NULL)
    {
        fread(md5buf, sizeof(char), sizeof(md5buf) - 1, fp);
        md5buf[sizeof(md5buf) - 1] = '\0';
        pclose(fp);
        strcpy(caMd5, md5buf);
    }
    else
    {
        strcpy(caMd5, "");
    }

    return &caMd5[0];
}



//部门电脑树
int lanTree_depComp(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caLevel[16];
    char caPlate[128];
    char caGroupname[72], caGroupid[16];
    char caTemp[4024];
    char caName[32];
    char caWhere[256];

    char caCurPg[16], caTotRec[16];
    char caPid[16];
    unsigned long lLevel, lPid;
    int  iNum;
    char caFilter[256];
    char caNo[16];
    unsigned long lGroupid, lGroupid0, lCurPg, lTotRec, lRowNum, lStartRec;
    utPltDbHead *psDbHead;
    int iReturn, i;

    char caValue[256];
    char caVar1[32];
    char caOpt[256];
    char caGroups[1024];
    char caFacs[1024];
    unsigned long lId;
    char *p, *pVar;
    unsigned char *pHash;
    char caCname[72];

    char caFiled[128];
    long lCount;

    char caCname1[72];
    char caSql[2048];
    char caKeyword[64], caKeyword1[64];
    char caState[16];
    char caSip[16], caMac[20], caCompname[36];
    nwCompInfo *psComp;
    nwCompIp   *psCompIp;
    long lLx;
#ifdef LDEBUG1
    utMsgPrintMsg(psMsgHead);
#endif
    utMsgPrintMsg(psMsgHead);

    memset(caCname, 0, sizeof(caCname));
    iReturn = utMsgGetSomeNVar(psMsgHead, 4,
                               "node",        UT_TYPE_STRING, 10, caPid,
                               "keyword",       UT_TYPE_STRING, 60, caKeyword1,
                               "state",       UT_TYPE_STRING, 10, caState,
                               "plate",       UT_TYPE_STRING, 100, caPlate);
    memset(caKeyword, 0, sizeof(caKeyword));
    if(strlen(caKeyword1) > 0)
    {
        char *tmp = utf8convert(psShmHead, caKeyword1);
    }
    //    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    //    strcpy(caWhere,"\0");

    /*
         memset(caOpt,0,sizeof(caOpt));
        iReturn = dsCltGetMyInfo(1,"Userid",&lId);
        if(iReturn==0){
         pasDbOneRecord("select opt from dsuser where id=:id",1,"id",UT_TYPE_LONG,lId,UT_TYPE_STRING,255,caOpt);
        }

    */

    //    lRowNum = utComGetVar_ld(psShmHead,"GroupTreeRow",300);



    iNum = 0;

    char auth[20];
    strcpy(auth, getDsGroupid());

    psDbHead = utPltInitDb();
    if(strlen(caKeyword) == 0)
    {
        //      lStartRec = (lCurPg - 1)*300;
        printf("auth=%s\n", auth);
        if(atol(caPid) == 0 && !utStrIsSpaces(auth))
        {
            sprintf(caTemp, "select gname,gid,pid from nwgroup where gid = %d", atoi(auth));
        }
        else
        {
            sprintf(caTemp, "select gname,gid,pid from nwgroup where 1=1 ");
        }
        if(atol(caPid) != 0)
            sprintf(caTemp + strlen(caTemp), " and pid=%d ", atol(caPid));
        sprintf(caTemp + strlen(caTemp), " order by gname limit %lu,%lu ", 0, 300);
        printf("caTemp=%s\n", caTemp);
        psCur = pasDbOpenSql(caTemp, 0);

        if(psCur == NULL)
        {
            utWebDispMsg(iFd, psMsgHead, "ncs/ncmsg_back.htm", "部门查询", "数据库出错");
            //        free(pHash);
            return 0;
        }

        char caGroupnameT[32] = "";


        memset(caGroupname, 0, sizeof(caGroupname));
        lGroupid = 0;
        lPid = 0;
        iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 31, caGroupname,
                                 UT_TYPE_LONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lPid);

        while(iReturn == 0 || iReturn == 1405)
        {


            if(iNum > 0)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum + 1, ",");
            }
            iNum++;
            if(lPid == 0)
            {
                strcpy(caGroupnameT, ncLang("0011全部"));
            }
            else
            {
                strcpy(caGroupnameT, caGroupname);
            }

            utPltPutLoopVar(psDbHead, "groupname", iNum, utStrGetCharHan(caGroupnameT, 48));

            utPltPutLoopVar(psDbHead, "ctext", iNum, utStrGetCharHan(caGroupname, 48));
            //utPltSetCvtHtml(0);
            utPltPutLoopVarF(psDbHead, "groupid",   iNum, "%lu", lGroupid);
            //        psData = (struct stData *)pasLHashLook(pHash,&lGroupid);
            //         if(psData){
            //            printf("id=%d,lcount=%d\n",psData->id,psData->count);
            utPltPutLoopVar(psDbHead, "leaf", iNum, "false");
            utPltPutLoopVar(psDbHead, "cls", iNum, "folder");
            utPltPutLoopVar(psDbHead, "dtype", iNum, "2");

            //         }
            //        else{
            //            utPltPutLoopVar(psDbHead,"leaf", iNum,"true");
            //            utPltPutLoopVar(psDbHead,"cls", iNum,"file");


            //         }


            utPltPutLoopVarF(psDbHead, "iNum", iNum, "%d", iNum - 1);

            utPltPutLoopVarF(psDbHead, "pid", iNum, "%lu", lPid);

            memset(caGroupname, 0, sizeof(caGroupname));
            lGroupid = 0;
            lLevel = 0;
            lPid = 0;
            iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 31, caGroupname,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lPid);
        }
        pasDbCloseCursor(psCur);
    }

    //     free(pHash);
    utPltPutVar(psDbHead, "rootname", utComGetVar_sd(psShmHead, "rootname", ncLang("0011全部")));





    sprintf(caTemp, "select devname,compid,groupid,inet_ntoa(ip),mac,compname from nwcompute where 1=1 ");

    if(strlen(caKeyword) == 0)
    {

        if(atol(caPid) == 0)
        {
            if(!utStrIsSpaces(auth))
            {
                sprintf(caTemp + strlen(caTemp), " and groupid=%d ", 99999);
            }
            else
            {
                sprintf(caTemp + strlen(caTemp), " and groupid not in (select gid from nwgroup ) ");
            }
        }
        else
        {
            printf("caPid= %s\n", caPid);
            sprintf(caTemp + strlen(caTemp), " and groupid=%d ", atol(caPid));

        }
    }
    else
    {
        sprintf(caTemp + strlen(caTemp), "and (devname like '%c%s%c' or compname like '%c%s%c' or inet_ntoa(ip) like '%c%s%c' or mac like '%c%s%c' ) ", '%', caKeyword, '%', '%', caKeyword, '%', '%', caKeyword, '%', '%', caKeyword, '%');



    }
    char sql[128] = "";
    char auth1[32] = "";
    char display[32] = "";
    int count1 = 0;

    dsCltGetMyInfo(1, "USERNAME", auth1);

    sprintf(sql, "select count(*) from usercommon where auth = '%s'", auth1);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count1);

    if(count1 > 0)
    {
        sprintf(sql, "select display from usercommon where auth = '%s'", auth1);
        pasDbOneRecord(sql, 0, UT_TYPE_STRING, sizeof(display), display);
    }
    else
    {
        strcpy(display, "compname");
    }

    sprintf(caTemp + strlen(caTemp), " order by devname limit %lu,%lu", 0, 300);
    printf("caTemp=%s\n", caTemp);
    psCur = pasDbOpenSql(caTemp, 0);
    //DebugLog("11111111111111111111","222222222222222222222222222",caTemp);
    if(psCur != NULL)
    {
        memset(caGroupname, 0, sizeof(caGroupname));
        memset(caSip, 0, sizeof(caSip));
        lGroupid = 0;
        lPid = 0;
        memset(caMac, 0, sizeof(caMac));
        memset(caCompname, 0, sizeof(caCompname));
        //devname,compid,groupid,inet_ntoa(ip),mac,compname
        iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 68, caGroupname,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lPid,
                                 UT_TYPE_STRING, 15, caSip,
                                 UT_TYPE_STRING, 18, caMac,
                                 UT_TYPE_STRING, 31, caCompname);

        while(iReturn == 0)
        {

            if(strcasecmp(display, "ip") == 0)
            {
                strcpy(caFiled, caSip);
            }
            else if(strcasecmp(display, "mac") == 0)
            {
                strcpy(caFiled, caMac);
            }
            else if(strcasecmp(display, "devname") == 0)
            {
                strcpy(caFiled, caGroupname);
            }
            else if(strcasecmp(display, "compname") == 0)
            {
                strcpy(caFiled, caCompname);
            }
            else
            {
                strcpy(caFiled, caCompname);
            }


            lLx = 2;
            psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &lGroupid);
            if(psComp)
            {
                psCompIp = (nwCompIp *)utShmHashLook(psShmHead, NC_LNK_ONLINECOMP, &lGroupid);
                if(psCompIp)
                {
                    lLx = 1;
                }
            }
            //  printf("lLx=%d\n",lLx);
            if(lLx == atol(caState) || strlen(caState) == 0)
            {

                if(iNum > 0)
                {
                    utPltPutLoopVar(psDbHead, "dh", iNum + 1, ",");
                }
                iNum++;

                if(lLx == 2)
                {
                    sprintf(caTemp, "<font color=#bbbbbb>%s(%s)</font>", utStrGetCharHan(caFiled, 48), ncLang("0117离线"));
                    utPltPutLoopVar(psDbHead, "groupname", iNum, caTemp);
                }
                else
                {
                    utPltPutLoopVar(psDbHead, "groupname", iNum, utStrGetCharHan(caFiled, 48));
                }
                utPltPutLoopVar(psDbHead, "check", iNum, ",\"checked\":false");
                utPltPutLoopVar(psDbHead, "ctext", iNum, utStrGetCharHan(caFiled, 48));
                utPltPutLoopVarF(psDbHead, "did",   iNum, "%lu", lGroupid);
                utPltPutLoopVarF(psDbHead, "groupid",   iNum, "%lu", lGroupid);

                utPltPutLoopVar(psDbHead, "leaf", iNum, "true");
                utPltPutLoopVar(psDbHead, "cls", iNum, "file");

                utPltPutLoopVar(psDbHead, "dtype", iNum, "0");
                utPltPutLoopVar(psDbHead, "sip", iNum, caSip);
                sprintf(caTemp, ncLang("0029计算机名:%s,ip地址：%s,mac地址:%s"), caCompname, caSip, caMac);
                utPltPutLoopVar(psDbHead, "qtip", iNum, caTemp);

                utPltPutLoopVarF(psDbHead, "iNum", iNum, "%d", iNum - 1);
                utPltPutLoopVar(psDbHead, "treecls", iNum, "treecompt");
                utPltPutLoopVarF(psDbHead, "pid", iNum, "%d", lPid);
            }
            memset(caSip, 0, sizeof(caSip));
            memset(caGroupname, 0, sizeof(caGroupname));
            lGroupid = 0;
            lLevel = 0;
            lPid = 0;
            memset(caMac, 0, sizeof(caMac));
            memset(caCompname, 0, sizeof(caCompname));
            iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 68, caGroupname,
                                     UT_TYPE_ULONG, 4, &lGroupid,
                                     UT_TYPE_ULONG, 4, &lPid,
                                     UT_TYPE_STRING, 15, caSip,
                                     UT_TYPE_STRING, 18, caMac,
                                     UT_TYPE_STRING, 31, caCompname);

        }
        pasDbCloseCursor(psCur);
    }


    utPltShowDb(psDbHead);
    if(strlen(caPlate) > 0)
    {
        utPltOutToHtml(iFd, psMsgHead, psDbHead, caPlate);
    }
    else
    {
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "lan/ncs_depcompute_tree.htm");
    }

    return 0;
}

//主策略树
int lanTreeProcy(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caTemp[1024];
    char sqlbuf[1024];
    long lFlag, lKid;
    int  iNum;
    utPltDbHead *psDbHead;
    int iReturn, i;
    long lCount;
    char caName[64];
    long lId, lSid;
    char caPlate[128];
    char caPid[16];
    char caPid1[16];
    long lPid;
    long lSumRec = 0;
    char caConstatus[16];
    char caDel[16], caUpdate[16], caSelsid[1024], caKeyname[64], caSid[16], caTemp_d[64];
    char caLawname[36];
    long lConstatus;
#ifdef LDEBUG1
    utMsgPrintMsg(psMsgHead);
#endif
    utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead, 9,
                               "del",           UT_TYPE_STRING, 8, caDel,
                               "keyname",       UT_TYPE_STRING, 30, caKeyname,
                               "selsid", UT_TYPE_STRING, 1000, caSelsid,
                               "update", UT_TYPE_STRING, 10, caUpdate,
                               "sid", UT_TYPE_STRING, 10, caSid,
                               "del", UT_TYPE_STRING, 10, caDel,
                               "plate", UT_TYPE_STRING, 100, caPlate,
                               "constatus",  UT_TYPE_STRING, 10, caConstatus,
                               "node", UT_TYPE_STRING, 10, caPid1);



    if(strlen(caKeyname) > 0)
    {
        //   strcpy(caKeyword,pasCvtGBK(2,caKeyword1,caTemp,60));
        char *tmp = utf8convert(psShmHead, caKeyname);
    }

    lConstatus = atol(caConstatus);

    int lId1 = 0;
    int iReturn1 = dsCltGetMyInfo(1, "Userid", &lId1);
    if(iReturn1 != 0 || lId1 <= 0)
    {
        lId1 = 0;
    }
    char caMyname[32] = "";
    iReturn1 = dsCltGetMyInfo(1, "USERNAME", caMyname);

    printf("lan_web_init:%s\n", caMyname);
    if(iReturn1 != 0)
    {
        strcpy(caMyname, "");
    }

    char msg[1024] = "";
    if(strcmp(caUpdate, "update") == 0)
    {
        if(strlen(caSid) == 0)
        {
            sprintf(caTemp, "select count(*) from nwmainprocy where lawname='%s' ", caKeyname);
            lCount = 0;
            pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);
            if(lCount == 0)
            {

                //pasDbOneRecord("select dispname from dsuser where id=:id",1,"id",UT_TYPE_LONG,lId,UT_TYPE_STRING,31,username);

                pasDbExecSqlF("insert into nwmainprocy(lawname,constatus,createtime,modtime,createuser,rev1,rev2) values ('%s',%d,%d,%d,'%s','','')", caKeyname, lConstatus, time(0), time(0), caMyname);
                snprintf(msg, sizeof(msg), "添加主策略[%s]", caKeyname);
                writeSysLog(psMsgHead, "06", "Success", msg);
            }
            else
            {

                pasDbExecSqlF("update nwmainprocy set flag = 0, modtime=%d,constatus=%d,createuser = '%s' where lawname='%s' ", time(0), lConstatus, caMyname, caKeyname);
            }
        }
        else
        {

            sprintf(caTemp, "select count(*) from nwmainprocy where lawname='%s' and sid!=%s ", caKeyname, caSid);
            lCount = 0;
            pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);
            if(lCount == 0)
            {

                snprintf(msg, sizeof(msg), "修改主策略[%s]", caSelsid);
                writeSysLog(psMsgHead, "06", "Success", msg);
                pasDbExecSqlF("update nwmainprocy set flag =0, lawname='%s',modtime=%d,constatus=%d where sid=%s ", caKeyname, time(0), lConstatus, caSid);
            }
        }

    }
    if(strcmp(caDel, "del") == 0 && strlen(caSelsid) > 0)
    {
        iReturn = pasDbExecSqlF("update nwmainprocy set flag = 8 where sid in (%s)", caSelsid);

        snprintf(msg, sizeof(msg), "删除主策略[%s]", caSelsid);
        writeSysLog(psMsgHead, "06", "Success", msg);
        if(iReturn == 0)
        {
            iReturn = pasDbExecSqlF("update nwprocyfile set flag = 8, modtime=%d where pid in(%s)", time(0), caSelsid);
            iReturn = pasDbExecSqlF("update nwprocydev set flag = 8, modtime=%d where pid  in(%s)", time(0), caSelsid);
            iReturn = pasDbExecSqlF("update nwprocylink set flag = 8, modtime=%d where pid  in(%s)", time(0), caSelsid);
            iReturn = pasDbExecSqlF("update nwprocypgtype set flag = 8, modtime=%d where pid  in(%s)", time(0), caSelsid);
            iReturn = pasDbExecSqlF("update nwprocypname set flag = 8, modtime=%d where pid  in(%s)", time(0), caSelsid);
            iReturn = pasDbExecSqlF("update nwprocynet set flag = 8, modtime=%d where pid  in(%s)", time(0), caSelsid);
            iReturn = pasDbExecSqlF("update nwprocyurl set flag = 8, modtime=%d where pid  in(%s)", time(0), caSelsid);
            iReturn = pasDbExecSqlF("update nwprocyoth set flag = 8, modtime=%d where pid  in(%s)", time(0), caSelsid);
            iReturn = pasDbExecSqlF("update nwprocyref set flag = 8, modtime=%d where pid  in(%s)", time(0), caSelsid);
            iReturn = pasDbExecSqlF("update nwprocyref set flag = 8, modtime=%d where othid  in(%s)", time(0), caSelsid);
            iReturn = pasDbExecSqlF("update nwprocyobj set flag = 8, modtime=%d where pid  in(%s)", time(0), caSelsid);
        }


    }





    iNum = 0;

    psDbHead = utPltInitDb();

    char tmp[64] = "";
    if(strcmp(caMyname, "admin"))
    {
        sprintf(tmp, " and createuser = '%s'", caMyname);
    }
    else
    {
        strcpy(tmp, "");
    }

    sprintf(sqlbuf, "select sid,lawname,constatus from nwmainprocy where flag = 0 %s", tmp);

    psCur = pasDbOpenSql(sqlbuf, 0);
    if(psCur == NULL)
    {

        return 0;
    }
    lSid = 0;
    memset(caLawname, 0, sizeof(caLawname));
    lConstatus = 0;
    iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lSid,
                             UT_TYPE_STRING, 23, caLawname,
                             UT_TYPE_LONG, 4, &lConstatus);


    while(iReturn == 0 || iReturn == 1405)
    {

        if(iNum > 0)
        {
            utPltPutLoopVar(psDbHead, "dh", iNum + 1, ",");
        }
        iNum++;
        utPltSetCvtHtml(1);

        if(lConstatus == 1)
        {
            utPltPutLoopVarF(psDbHead, "groupname", iNum, "%s_(离线无效)", caLawname);
        }
        else
        {
            utPltPutLoopVarF(psDbHead, "groupname", iNum, "%s", caLawname);
        }




        utPltSetCvtHtml(0);
        utPltPutLoopVarF(psDbHead, "id", iNum, "%d", lSid);
        utPltPutLoopVar(psDbHead, "pname", iNum, caLawname);
        utPltPutLoopVarF(psDbHead, "status", iNum, "%d", lConstatus);

        lSid = 0;
        memset(caLawname, 0, sizeof(caLawname));
        lConstatus = 0;
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lSid,
                                 UT_TYPE_STRING, 23, caLawname,
                                 UT_TYPE_LONG, 4, &lConstatus);
    }
    pasDbCloseCursor(psCur);


    //utPltShowDb(psDbHead);

    //     utPltPutVarF(psDbHead,"sum","%d",lSumRec);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "lan/lan_procy_checktree.htm");



    return 0;
}


//显示文件策略
int lan_Procy_File(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    long iReturn, lCount;
    char caStart[16], caTemp[2048], caCurPg[16];
    long lCurPg, lRowNum, lStartRec;
    char caTemp_d[128], caStatus[16];
    long iNum;
    unsigned long lOptype, lSnap, lType, lSid, lPid, lModtime, lIndex;
    char caPid[16], caLimit[16], caPage[16], caDel[16], caDir[16], caKeyword[64], caSelsid[1024], caUpdate[16], caNtype[16], caNetid[64], caSid[16];
    char caOptype[16], caSnap[16], caLxtype[16], caFilename[128], caCreateuser[64];
    utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead, 14,
                               "pid",            UT_TYPE_STRING, 10, caPid,
                               "limit",            UT_TYPE_STRING, 10, caLimit,
                               "page",          UT_TYPE_STRING, 10, caPage,
                               "dir",           UT_TYPE_STRING, 8, caDir,
                               "keyword",       UT_TYPE_STRING, 30, caKeyword,
                               "selsid", UT_TYPE_STRING, 1000, caSelsid,
                               "filename", UT_TYPE_STRING, 127, caFilename,
                               "optype", UT_TYPE_STRING, 10, caOptype,
                               "snap", UT_TYPE_STRING, 10, caSnap,
                               "lxtype", UT_TYPE_STRING, 10, caLxtype,
                               "update", UT_TYPE_STRING, 10, caUpdate,
                               "sid", UT_TYPE_STRING, 10, caSid,
                               "start", UT_TYPE_STRING, 10, caStart,
                               "del", UT_TYPE_STRING, 10, caDel);

    if(strlen(caKeyword) > 0)
    {
        strcpy(caTemp_d, pasCvtGBK(2, caKeyword, caTemp, 63));
        strcpy(caKeyword, caTemp_d);
    }

    char * filename_gbk = utf8convert(psShmHead, caFilename);
    lRowNum = atol(caLimit);
    if(lCurPg <= 0) lCurPg = 1;
    lStartRec = atol(caStart);

    psDbHead = utPltInitDb();
    lOptype = atol(caOptype);
    lSnap = atol(caSnap);
    lType = atol(caLxtype);

    char caMyname[32] = "";
    char sql_tmp[128] = "";
    dsCltGetMyInfo(1, "USERNAME", caMyname);
    if(strcmp(caMyname, "admin"))
    {
        sprintf(sql_tmp, " and createuser = '%s'", caMyname);
    }

    if(strcmp(caUpdate, "update") == 0)
    {
        if(strlen(caSid) == 0)
        {

            sprintf(caTemp, "select sid from nwprocyfile where pid=%lu and optype=%lu and filename='%s' and snap=%lu and type=%d ", atol(caPid), lOptype, filename_gbk, lSnap, lType);
            lSid = 0;
            pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lSid);
            if(lSid > 0)
            {
                pasDbExecSqlF("update nwprocyfile set flag = 0, optype=%lu,filename='%s',snap=%d,type=%d,modtime=%d where sid=%lu  ", lOptype, filename_gbk, lSnap, lType, time(0), lSid);
                writeSysLog(psMsgHead, "04", "Success", "修改了文件策略");
            }
            else
            {
                pasDbExecSqlF("insert into nwprocyfile(pid,filename,optype,snap,type,createtime,modtime,createuser) \
	       		values(%lu,'%s',%lu,%lu,%lu,%lu,%lu,'%s')", \
                              atol(caPid), filename_gbk, lOptype, lSnap, lType, time(0), time(0), caMyname);

                writeSysLog(psMsgHead, "04", "Success", "添加了文件策略");
            }
        }
        else
        {
            lSid = atol(caSid);
            pasDbExecSqlF("update nwprocyfile set flag = 0, optype=%lu,filename='%s',snap=%d,type=%d,modtime=%d where sid=%lu  ", lOptype, filename_gbk, lSnap, lType, time(0), lSid);
            writeSysLog(psMsgHead, "04", "Success", "修改了文件策略");
        }

    }
    if(strcmp(caDel, "del") == 0 && strlen(caSelsid) > 0)
    {
        pasDbExecSqlF("update nwprocyfile set flag = 8 where sid in(%s)", caSelsid);
        writeSysLog(psMsgHead, "06", "Success", "删除了文件策略");
    }




    sprintf(caTemp, "select count(*) from ncscasenetid where pid=%d %s", atol(caPid), sql_tmp);

    if(strlen(caKeyword) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and filename like '%c%s%c' ", '%', caKeyword, '%');
    }
    lCount = 0;
    iReturn = pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);


    sprintf(caTemp, "select sid,pid,filename,optype,snap,type,modtime,createuser from nwprocyfile  where flag = 0 and pid=%d %s", atol(caPid), sql_tmp);

    if(strlen(caKeyword) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and filename like '%c%s%c' ", '%', caKeyword, '%');
    }
    sprintf(caTemp + strlen(caTemp), " order by sid desc limit %d,%d", lStartRec, lRowNum);

    printf("caTemp=%s\n", caTemp);


    psCur = pasDbOpenSql(caTemp, 0);

    if(psCur == NULL)
    {
        //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561数据统计出错"),ncsLang("0171查询数据出错"));
        return 0;
    }


    iReturn = 0;
    iNum = 0;
    while((iReturn == 0 || iReturn == 1405) && iNum < lRowNum)
    {

        lSid = 0;
        lPid = 0;
        memset(caFilename, 0, sizeof(caFilename));
        lOptype = 0;
        lSnap = 0;
        lType = 0;
        lModtime = 0;
        memset(caCreateuser, 0, sizeof(caCreateuser));

        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_LONG, 4, &lSid,
                                 UT_TYPE_LONG, 4, &lPid,
                                 UT_TYPE_STRING, 127, caFilename,
                                 UT_TYPE_ULONG, 4, &lOptype,
                                 UT_TYPE_LONG, 4, &lSnap,
                                 UT_TYPE_LONG, 4, &lType,
                                 UT_TYPE_LONG, 4, &lModtime,
                                 UT_TYPE_STRING, 23, caCreateuser);





        if((iReturn != 0) && (iReturn != 1405)) break;
        iNum++;
        if(iNum > 1)
        {
            utPltPutLoopVar(psDbHead, "dh", iNum, ",");
        }
        utPltPutLoopVarF(psDbHead, "sid", iNum, "%lu", lSid);
        utPltPutLoopVarF(psDbHead, "num", iNum, "%lu", iNum - 1);
        utPltPutLoopVarF(psDbHead, "pid", iNum, "%lu", lPid);
        utPltPutLoopVarF(psDbHead, "optype", iNum, "%lu", lOptype);
        utPltPutLoopVarF(psDbHead, "snap", iNum, "%lu", lSnap);
        utPltPutLoopVarF(psDbHead, "type", iNum, "%lu", lType);
        utPltPutLoopVar(psDbHead, "filename", iNum, caFilename);
        utPltPutLoopVar(psDbHead, "createuser", iNum, caCreateuser);
        memset(caTemp, 0, sizeof(caTemp));
        lIndex = 0;
        if(lOptype & 0x01)
        {
            if(lIndex > 0)
            {
                sprintf(caTemp + strlen(caTemp), ",");
            }
            lIndex++;
            sprintf(caTemp + strlen(caTemp), "禁止文件复制到USB");
        }
        if(lOptype & 0x02)
        {
            if(lIndex > 0)
            {
                sprintf(caTemp + strlen(caTemp), ",");
            }
            lIndex++;
            sprintf(caTemp + strlen(caTemp), "禁止文件复制到共享目录 ");
        }
        if(lOptype & 0x04)
        {
            if(lIndex > 0)
            {
                sprintf(caTemp + strlen(caTemp), ",");
            }
            lIndex++;
            sprintf(caTemp + strlen(caTemp), "禁止文件复制到云盘 ");
        }
        utPltPutLoopVar(psDbHead, "optype_desc", iNum, caTemp);
        if(lSnap == 2)
        {
            utPltPutLoopVar(psDbHead, "snap_desc", iNum, "截屏");
        }
        else
        {
            utPltPutLoopVar(psDbHead, "snap_desc", iNum, "不截屏");
        }
        if(lType == 0)
        {
            utPltPutLoopVar(psDbHead, "type_desc", iNum, "离线有效");
        }
        else if(lType == 1)
        {
            utPltPutLoopVar(psDbHead, "type_desc", iNum, "离线无效");
        }
        utPltPutLoopVar(psDbHead, "createuser", iNum, caCreateuser);
        utPltPutLoopVar(psDbHead, "modtime", iNum, utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));

    }
    pasDbCloseCursor(psCur);


    utPltPutVarF(psDbHead, "TotRec", "%d", lCount);

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "lan/procy/ncs_procy_file_list.htm");
    return 0;
}

/* 显示在线计算机信息  */
int lan_OnlineCompute(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    nwCompInfo     *psComp;    /* 计算机信息  */
    nwCompIp       *psCompIp;
    ncUserInfo     *psUserInfo;
    pasHashInfo sHashInfo;
    char *pHash;
    int iNum, iReturn, i, k;
    unsigned long lGroupid0, lGroupid;
    utPltDbHead *psDbHead;
    char caCond[32], caSear[16], caGroupid[16], caCurpg[16];
    char caPlate[64];
    char mac[6];
    long lRowNum, lCurpg, lStartRec, lTotpg;
    char caKeyword[64], caLimit[16], caStart[16], caPage[16], caDir[16];
    char caTemp_d[128], caTemp[1024], caCompid[16];
    long lCurPg;
    unsigned long lIp, lCompid;
    char caMove[16], caSelsid[1024], caMid[16];
    char caOut[200][256];
    char caMac_t[64], caIp_t[64];
    //   nwCompInfo *psComp;
    //  utMsgPrintMsg(psMsgHead);
    lGroupid = 0;
    lIp = 0;

    iReturn = utMsgGetSomeNVar(psMsgHead, 10,
                               "keyword",       UT_TYPE_STRING, 30, caKeyword,
                               "groupid",       UT_TYPE_STRING, 15, caGroupid,
                               "compid",        UT_TYPE_STRING, 10, caCompid,
                               "limit",         UT_TYPE_STRING, 10, caLimit,
                               "start",         UT_TYPE_STRING, 10, caStart,
                               "page",          UT_TYPE_STRING, 10, caPage,
                               "dir",           UT_TYPE_STRING, 8, caDir,
                               "selsid",        UT_TYPE_STRING, 1000, caSelsid,
                               "move",          UT_TYPE_STRING, 10, caMove,
                               "mid",           UT_TYPE_STRING, 10, caMid);
    lCompid = strtoul(caCompid, NULL, 10);
    if(!utStrIsSpaces(caGroupid))
    {
        lGroupid = atol(caGroupid);
    }

    if(strlen(caKeyword) > 0)
    {
        strcpy(caTemp_d, pasCvtGBK(2, caKeyword, caTemp, 63));
        strcpy(caKeyword, caTemp_d);
    }
    lRowNum = atol(caLimit);
    if(lRowNum == 0)
    {
        lRowNum = 40;
    }
    if(lCurPg <= 0) lCurPg = 1;
    lStartRec = atol(caStart);


    if(strlen(caMove) > 0 && strlen(caSelsid) > 0)
    {
        iReturn = pasDbExecSqlF("update nwcompute set groupid=%d where compid in (%s) ", atol(caMid), caSelsid);
        if(iReturn == 0)
        {
            int num = sepcharbydh2(caSelsid, caOut);
            for(i = 0; i < num; i++)
            {
                lCompid = strtoul(caOut[i], NULL, 10);
                psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &lCompid);
                if(psComp)
                {
                    psComp->groupid = atol(caMid);
                }
            }
        }
    }

    //权限
    char authGroup[4096] = "";
    char childGroup[4096] = "";
    char *auth = getDsGroupid();

    if(!utStrIsSpaces(auth))
    {
        sprintf(authGroup, ",%s,", auth);
    }

    if(!utStrIsSpaces(caGroupid))
    {
        sprintf(childGroup, "%s", getGroupid(strtoul(caGroupid, NULL, 10)));
    }

    writeLog("ssjk.txt", "OnlineCom", childGroup);

    if(utStrIsValidIp(caKeyword))
    {
        lIp = ntohl(pasIpcvtLong(caKeyword));
    }
    else
    {
        lIp = 0;
    }
    if(ncUtlStrIsMacAddress(caKeyword))
    {
        ncCvtMacI(caKeyword, mac);
    }
    else
    {
        memset(mac, 0, 6);
    }
    psDbHead = utPltInitDb();
    pHash = (char *)utShmHashHead(psShmHead, NC_LNK_ONLINECOMP);
    //printf("aaaaaaaaaaaaaaaaa\n");
    if(pHash == NULL)
    {
        //      utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0209调试软件"),ncLang("0833在线信息不存在"));
        return 0;
    }

    psCompIp  = (nwCompIp *)pasHashFirst(pHash, &sHashInfo);
    i = 0;
    char groupidTmp[16] = "";

    while(psCompIp && i < lStartRec)
    {
        strcpy(caIp_t, utComHostIp(htonl(psCompIp->ip)));
        strcpy(caMac_t, ncCvtMac(psCompIp->psComp->mac));
        if(psCompIp->psComp)
        {
            if(psCompIp->psComp->groupid != 0)
            {
                sprintf(groupidTmp, ",%u,", psCompIp->psComp->groupid);
            }
            else
            {
                strcpy(groupidTmp, "999999");
            }
            if((strlen(caKeyword) == 0 || strstr(psCompIp->psComp->devname, caKeyword) || strstr(caIp_t, caKeyword) ||
                strstr(psCompIp->psComp->compname, caKeyword) || strstr(caMac_t, caKeyword)) &&
               (lGroupid == 0 || psCompIp->psComp->groupid == lGroupid || strstr(childGroup, groupidTmp)) && (lCompid == 0 || psCompIp->compid == lCompid) &&
               (utStrIsSpaces(authGroup) || strstr(authGroup, groupidTmp)))
            {


                i++;
            }
        }
        psCompIp  = (nwCompIp *)pasHashNext(&sHashInfo);
    }
    iNum = 0;
    while(psCompIp && iNum < lRowNum)
    {
        if(psCompIp->psComp)
        {
            if(psCompIp->psComp->groupid != 0)
            {
                sprintf(groupidTmp, ",%u,", psCompIp->psComp->groupid);
            }
            else
            {
                strcpy(groupidTmp, "99999999");
            }
            strcpy(caIp_t, utComHostIp(htonl(psCompIp->ip)));
            strcpy(caMac_t, ncCvtMac(psCompIp->psComp->mac));
            if((strlen(caKeyword) == 0 || strstr(psCompIp->psComp->devname, caKeyword) || strstr(caIp_t, caKeyword) ||
                strstr(psCompIp->psComp->compname, caKeyword) || strstr(caMac_t, caKeyword)) &&
               (lGroupid == 0 || psCompIp->psComp->groupid == lGroupid || strstr(childGroup, groupidTmp)) && (lCompid == 0 || psCompIp->compid == lCompid) && (utStrIsSpaces(authGroup) || strstr(authGroup, groupidTmp)))
            {

                i++;
                iNum++;
                if(iNum > 1)
                {
                    utPltPutLoopVar(psDbHead, "dh", iNum, ",");
                }
                utPltPutLoopVarF(psDbHead, "num", iNum, "%d", iNum);
                utPltPutLoopVarF(psDbHead, "compid", iNum, "%lu", psCompIp->compid);
                utPltPutLoopVar(psDbHead, "ip",    iNum, utComHostIp(htonl(psCompIp->ip)));
                utPltPutLoopVar(psDbHead, "mac",   iNum, ncCvtMac(psCompIp->psComp->mac));
                utPltPutLoopVar(psDbHead, "compname", iNum, psCompIp->psComp->compname);
                utPltPutLoopVar(psDbHead, "devname", iNum, psCompIp->psComp->devname);
                utPltPutLoopVarF(psDbHead, "flow", iNum, "%lu", psCompIp->lFlow);
                utPltPutLoopVarF(psDbHead, "netlink", iNum, "%lu", psCompIp->lNetlink);
                utPltPutLoopVarF(psDbHead, "groupid", iNum, "%lu", psCompIp->psComp->groupid);
                utPltPutLoopVar(psDbHead, "version", iNum, psCompIp->psComp->version);
                utPltPutLoopVar(psDbHead, "gname", iNum, lanGetGroup(psShmHead, psCompIp->psComp->groupid));

                //printf("onlinecompute:compname:%s\tgroupid:%lu\n",psCompIp->psComp->compname,psCompIp->psComp->groupid);
                // printf("allgroup:%s\n",authGroup);


                //            psUserInfo = ncGetUserInfoById(psShmHead,psCompIp->psComp->userid);

                utPltPutLoopVar(psDbHead, "logtime", iNum, utTimFormat("%m-%d %H:%M:%S", psCompIp->lLoginTime));
                utPltPutLoopVar(psDbHead, "lasttime", iNum, utTimFormat("%m-%d %H:%M:%S", psCompIp->lLastTime));
            }
        }
        psCompIp  = (nwCompIp *)pasHashNext(&sHashInfo);
    }


    while(psCompIp)
    {
        if(psCompIp->psComp)
        {
            if((strlen(caKeyword) == 0 || strstr(psCompIp->psComp->devname, caKeyword) || lIp& psCompIp->ip == lIp ||
                strstr(psCompIp->psComp->compname, caKeyword) || memcmp(psCompIp->psComp->mac, mac, 6) == 0) &&
               (lGroupid == 0 || psCompIp->psComp->groupid == lGroupid) && (lCompid == 0 || psCompIp->compid == lCompid))
            {

                i++;
            }
        }
        psCompIp  = (nwCompIp *)pasHashNext(&sHashInfo);
    }

    //  lTotpg = (i - 1) / lRowNum + 1;
    utPltPutVarF(psDbHead, "TotRec", "%d", i);

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "lan/ssjk/ncs_onlinecomp_list.html");

    return 0;
}

//下面的代码由管理中心移植过来
//根?萑ㄏ?获取单位组ID组合
char *getDsGroupid()
{
    static char caReturn[1024];
    char caIds[1024], caTempid[1024];
    char caOpt[256];
    int iReturn, lId, iNum;
    pasDbCursor *psCur;
    memset(caReturn, 0, sizeof(caReturn));
    lId = 0;

    iReturn = dsCltGetMyInfo(1, "Userid", &lId);
    //printf("dsiReturn=%d\n",iReturn);
    if(iReturn != 0 || lId <= 0)
    {
        return &caReturn[0];
    }

    memset(caOpt, 0, sizeof(caOpt));
    pasDbOneRecord("select opt from dsuser where id=:id", 1, "id", UT_TYPE_LONG, lId, UT_TYPE_STRING, 255, caOpt);
    if(strstr(caOpt, "all"))
    {
        return &caReturn[0];
    }



    sprintf(caReturn, "%s", caOpt);
    sprintf(caIds, "%s", caOpt);

    while(strlen(caIds) > 0)
    {
        memset(caTempid, 0, sizeof(caTempid));
        iNum = 0;
        //printf("select gid from nwgroup where pid in(%s)\n ",caIds);
        psCur = pasDbOpenSqlF("select gid from nwgroup where pid in(%s) ", caIds);
        if(psCur)
        {
            iReturn = 0;
            iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId);
            while(iReturn == 0)
            {
                if(iNum == 0)
                {
                    sprintf(caTempid, "%d", lId);
                }
                else
                {
                    sprintf(caTempid + strlen(caTempid), ",%d", lId);
                }

                iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId);

                iNum++;

            }
        }
        pasDbCloseCursor(psCur);

        if(strlen(caTempid) > 0)
        {
            sprintf(caReturn + strlen(caReturn), ",%s", caTempid);
        }
        strcpy(caIds, caTempid);
    }



    return &caReturn[0];
}


//由部门ID获取部门组ID组合

char *getGroupid(long lGroupid)
{
    static char caReturn[1024];
    char caIds[1024], caTempid[1024];
    int iReturn, lId, iNum;
    pasDbCursor *psCur;
    memset(caReturn, 0, sizeof(caReturn));


    sprintf(caReturn, "%d", lGroupid);
    sprintf(caIds, "%d", lGroupid);

    while(strlen(caIds) > 0)
    {
        memset(caTempid, 0, sizeof(caTempid));
        iNum = 0;
        //      printf("select groupid from ncsgroup where pid in(%s)\n ",caIds);
        psCur = pasDbOpenSqlF("select gid from nwgroup where pid in(%s) ", caIds);
        if(psCur)
        {
            iReturn = 0;
            iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId);
            while(iReturn == 0)
            {
                if(iNum == 0)
                {
                    sprintf(caTempid, "%d", lId);
                }
                else
                {
                    sprintf(caTempid + strlen(caTempid), ",%d", lId);
                }

                iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId);

                iNum++;

            }
        }
        pasDbCloseCursor(psCur);

        if(strlen(caTempid) > 0)
        {
            sprintf(caReturn + strlen(caReturn), ",%s", caTempid);
        }
        strcpy(caIds, caTempid);
    }



    return &caReturn[0];
}



//部门树AJAX通讯

int ncsTreeGroup_v4(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caLevel[16];
    char caPlate[128];
    char caGroupname[32], caGroupid[16];

    char caName[32];
    char caWhere[256];
    char caSql[2048];
    char caTemp[4096];
    char caCurPg[16], caTotRec[16];
    char caPid[16];
    unsigned long lLevel, lPid;
    int  iNum;
    char caFilter[256];
    char caNo[16];
    unsigned long lGroupid, lGroupid0, lCurPg, lTotRec, lRowNum, lStartRec;
    utPltDbHead *psDbHead;
    int iReturn, i;
    char caVar[256], caFname[64], caBfname[64];
    char caValue[256];
    char caVar1[32];
    char caOpt[256];
    long lId;
    char *p, *pVar;
    unsigned char *pHash;
    pasLHashInfo  sHashInfo;
    struct stData
    {
        unsigned long id;
        unsigned long count;
    };
    long lCount;
    struct stData *psData;

    char caGroups[1024];

    //  printf("aaaaaaaaaaaa\n");
    //获取权限组ID

    strcpy(caGroups, getDsGroupid());
    printf("caGroups=[%s]\n", caGroups);

#ifdef LDEBUG1
    utMsgPrintMsg(psMsgHead);
#endif
    utMsgPrintMsg(psMsgHead);

    iReturn = utMsgGetSomeNVar(psMsgHead, 4,
                               "plate",      UT_TYPE_STRING, 127, caPlate,
                               "node",        UT_TYPE_STRING, 10, caPid,
                               "fname",      UT_TYPE_STRING, 63, caFname,
                               "bfname",     UT_TYPE_STRING, 63, caBfname);


    //    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere, "\0");




    lRowNum = utComGetVar_ld(psShmHead, "GroupTreeRow", 500);

    pHash = (unsigned char *)pasLHashInit(lRowNum, lRowNum, sizeof(struct stData), 0, 4);

    if(pHash == NULL)
    {
        printf(" Memory HttpHash Error \n");
        return (-1);
    }
    //  printf("select count(pid),pid from ncsgroup where pid in (select groupid from ncsgroup where pid=%d) group by pid limit 0,%d\n",atol(caPid),lRowNum);

    if(strlen(caGroups) > 0)
    {
        printf("[get pid = %s]\n", caPid);
        if(strlen(caPid) == 0)
        {
            sprintf(caTemp, "select pid from nwgroup where pid not in (%s) and gid in (%s) ", caGroups, caGroups);
            lPid = 0;
            printf("when get pid sql=[%s]\n", caTemp);
            pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lPid);
            sprintf(caPid, "%d", lPid);
        }
    }
    printf("[finish pid = %s]\n", caPid);
    if(strncmp(caPid, "all", 3) == 0)
    {
        psCur = pasDbOpenSqlF("select count(pid),pid from  nwgroup where pid in (select gid from nwgroup) group by pid limit 0,%d", lRowNum);
    }
    else
    {
        psCur = pasDbOpenSqlF("select count(pid),pid from  nwgroup where pid in (select gid from nwgroup where pid=%d) group by pid limit 0,%d", atol(caPid), lRowNum);
    }

    if(psCur == NULL)
    {
        free(pHash);
        return 0;
    }
    lGroupid = 0;
    lCount = 0;
    iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lCount,
                             UT_TYPE_ULONG, 4, &lGroupid);
    while(iReturn == 0)
    {
        psData = (struct stData *)pasLHashLookA(pHash, &lGroupid);
        if(psData)
        {
            psData->count = lCount;
        }
        lGroupid = 0;
        lCount = 0;
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lCount,
                                 UT_TYPE_ULONG, 4, &lGroupid);
    }
    pasDbCloseCursor(psCur);


    lStartRec = (lCurPg - 1) * lRowNum;

    if(strlen(caGroups) > 0)
    {
        if(strncmp(caPid, "all", 3) == 0)
        {
            sprintf(caSql, "select gname,gid,pid from nwgroup where gid in (%s)", caGroups);
        }
        else
        {
            sprintf(caSql, "select gname,gid,pid from nwgroup where gid in (%s) and pid=%d", caGroups, atol(caPid));
        }
    }
    else
    {
        if(strncmp(caPid, "all", 3) == 0)
        {
            sprintf(caSql, "select gname,gid,pid from nwgroup");
        }
        else
        {
            sprintf(caSql, "select gname,gid,pid from nwgroup where pid=%d ", atol(caPid));
        }
    }
    sprintf(caSql + strlen(caSql), " order by gname limit %lu,%lu", 0, lRowNum);
    psCur = pasDbOpenSql(caSql, 0);
    printf("[caSql=%s]\n", caSql);
    if(psCur == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "ncs/ncmsg_back.htm", "部门查询", "数据库出错");
        free(pHash);
        return 0;
    }
    psDbHead = utPltInitDb();




    utPltPutVar(psDbHead, "fname", caFname);
    utPltPutVar(psDbHead, "bfname", caBfname);
    memset(caGroupname, 0, sizeof(caGroupname));
    lGroupid = 0;
    lPid = 0;
    iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 31, caGroupname,
                             UT_TYPE_ULONG, 4, &lGroupid,
                             UT_TYPE_ULONG, 4, &lPid);

    iNum = 0;
    while(iReturn == 0)
    {

        if(iNum > 0)
        {
            utPltPutLoopVar(psDbHead, "dh", iNum + 1, ",");
        }
        iNum++;
        if(lPid == 0)
        {
            utPltPutLoopVar(psDbHead, "groupname", iNum, ncLang("0011全部"));
        }
        else
        {
            utPltPutLoopVar(psDbHead, "groupname", iNum, caGroupname);
        }

        utPltPutLoopVarF(psDbHead, "groupid",   iNum, "%lu", lGroupid);
        psData = (struct stData *)pasLHashLook(pHash, &lGroupid);
        if(psData)
        {
            //            printf("id=%d,lcount=%d\n",psData->id,psData->count);
            utPltPutLoopVar(psDbHead, "leaf", iNum, "false");
            utPltPutLoopVar(psDbHead, "cls", iNum, "folder");
        }
        else
        {
            utPltPutLoopVar(psDbHead, "leaf", iNum, "true");
            utPltPutLoopVar(psDbHead, "cls", iNum, "file");


        }


        utPltPutLoopVarF(psDbHead, "iNum", iNum, "%d", iNum - 1);

        utPltPutLoopVarF(psDbHead, "pid", iNum, "%lu", lPid);
        utPltPutLoopVar(psDbHead, "plate",    iNum, caPlate);
        utPltPutLoopVar(psDbHead, "fname", iNum, caFname);
        memset(caGroupname, 0, sizeof(caGroupname));
        lGroupid = 0;
        lLevel = 0;
        lPid = 0;
        iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 31, caGroupname,
                                 UT_TYPE_ULONG, 4, &lGroupid,
                                 UT_TYPE_ULONG, 4, &lPid);
    }
    pasDbCloseCursor(psCur);
    free(pHash);
    utPltPutVar(psDbHead, "rootname", utComGetVar_sd(psShmHead, "rootname", ncLang("0011全部")));

    // utPltShowDb(psDbHead);
    if(strlen(caPlate) > 0)
    {
        utPltOutToHtml(iFd, psMsgHead, psDbHead, caPlate);
    }
    else
    {
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_group_tree.htm");
    }



    return 0;
}

//显示单位组列表
int ncsWebClientClassList_v4(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char caWhere[512], caCond[64], caCond0[64];
    char caTotPg[16], caCurPg[16], caSort[16], caSortby[6];
    char caGroupname[32];
    char caGroupid[16];

    char caTemp[2024];

    char caAll[1024];


    unsigned long lGroupid, lPid, lCurPg, lTotPg;
    int lCount;
    int i, iNum, lId;
    char caGroupcode[16];

    char caSmt[4048], caOrder[64];
    char caDel[16], caExport[16];
    utPltDbHead *psDbHead;
    int iReturn;
    long lGroupid0, lRowNum, lUseflags, lStartRec, lUseflags0, lTotRec;
    char servicecode1[24], dispname1[68], address1[130], ceoname1[32], handphone1[34], contact1[34], telphone1[68], email1[130];
    char ip1[20], servicestate1[4], jointype1[4], area1[72], areaman1[32], joincode1[8];
    pasDbCursor *psCur;
    char caUsername2[32];
    char caMsg[1024];
    char caOnlineflag[16];
    long lMonflag = 0;
    unsigned long lCurtime, lTime, lRuntime;
    unsigned long lCase;
    char caYear[30];
    char caPlate[128];
    char caOpt[256];
    char caPage[16], caLimit[16];
    char caDir[12];
    FILE *fp;
    char pFile[256], caBuf[128];
    unsigned char *pHash;
    struct stData
    {
        unsigned long id;
        char caGroupname[36];
        long lCount;
    };
    long lCount_d;
    struct stData *psData;
    char caTemp_d[256];
    char caSesid[1024];
    long lSumHash, js_flag;
    long lGroupid3;
    char caUpdate[12], caPid[64];
    char caGroups[1024];
    char caGroupname2[36], caGroupcode2[20], caGroupid2[20];
    memset(caAll, 0, sizeof(caAll));
    strcpy(caGroups, getDsGroupid());

    memset(caOpt, 0, sizeof(caOpt));
    iReturn = dsCltGetMyInfo(1, "Userid", &lId);
    if(iReturn == 0)
    {
        pasDbOneRecord("select opt from dsuser where id=:id", 1, "id", UT_TYPE_LONG, lId, UT_TYPE_STRING, 255, caOpt);
    }

    dsCltGetSessionValue(1, "dispname", UT_TYPE_STRING, 30, caUsername2);
    iReturn = dsCltGetMyInfo(1, "Userid", &lId);
    js_flag = 0;



    utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead, 13,
                               "groupid",          UT_TYPE_STRING, 10, caGroupid,
                               "groupid2",          UT_TYPE_STRING, 12, caGroupid2,
                               "sort",            UT_TYPE_STRING, 15, caSort,
                               "dir",          UT_TYPE_STRING, 8,  caDir,
                               "del",             UT_TYPE_STRING, 10, caDel,
                               "export",          UT_TYPE_STRING, 10, caExport,
                               "groupname",       UT_TYPE_STRING, 31, caGroupname2,
                               "groupcode",       UT_TYPE_STRING, 16, caGroupcode2,
                               "page",            UT_TYPE_STRING, 10, caPage,
                               "limit",           UT_TYPE_STRING, 10, caLimit,
                               "selsid", UT_TYPE_STRING, 1000, caSesid,
                               "update", UT_TYPE_STRING, 10, caUpdate,
                               "pid", UT_TYPE_STRING, 60, caPid);

    if(strlen(caGroupname2) > 0)
    {
        strcpy(caTemp_d, pasCvtGBK(2, caGroupname2, caTemp, 64));
        strcpy(caGroupname, caTemp_d);
    }
    if(strlen(caGroupcode2) > 0)
    {
        strcpy(caTemp_d, pasCvtGBK(2, caGroupcode2, caTemp, 15));
        strcpy(caGroupcode, caTemp_d);
    }

    if(strlen(caPid) > 0)
    {
        strcpy(caTemp_d, pasCvtGBK(2, caPid, caTemp, 64));
        strcpy(caPid, caTemp_d);

        sprintf(caTemp, "select groupid from ncsgroup where groupname='%s' ", caPid);
        //printf("caTemp=%s\n",caTemp);
        lGroupid3 = 0;
        pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lGroupid3);
        if(lGroupid3 > 0)
        {
            sprintf(caPid, "%d", lGroupid3);
        }
    }



    if(strcmp(caUpdate, "update") == 0)
    {
        lPid = atol(caPid);
        lGroupid = atol(caGroupid2);
        if(lGroupid > 0)
        {
            if((lGroupid != lPid) && strlen(caGroupname) > 0)
            {
                pasDbExecSqlF("update ncsgroup set groupname='%s',pid=%d,groupcode='%s' where groupid=%d ", caGroupname, lPid, caGroupcode, lGroupid);
            }
        }
        else
        {
            lGroupid = utMd5Code(caGroupname, strlen(caGroupname), "ffff");
            if(lGroupid > 2000000000) lGroupid = lGroupid - 2000000000;

            if(lGroupid > 2000000000) lGroupid = lGroupid - 2000000000;

            sprintf(caTemp, "select count(*) from ncsgroup where groupid=%d ", lGroupid);
            lCount = 0;
            pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);
            if(lCount > 0)
            {
                pasDbExecSqlF("update ncsgroup set groupname='%s',pid=%d,groupcode='%s' where groupid=%d ", caGroupname, lPid, caGroupcode, lGroupid);
            }
            else
            {
                pasDbExecSqlF("insert into ncsgroup(groupid,groupname,descr,pid,groupcode) values(%d,'%s','%s',%d,'%s')", lGroupid, caGroupname, caGroupname, lPid, caGroupcode);
            }
        }
        strcpy(caGroupcode2, "");
        strcpy(caGroupname2, "");
        strcpy(caGroupid2, "");
    }

    if(!utStrIsSpaces(caDel))    /* 删除选中的项目*/
    {
        if(strlen(caSesid) > 0)
        {
            //printf("delete from ncsuser where groupid in(%s)\n",caSesid);
            iReturn = pasDbExecSqlF("delete from ncsuser where groupid in(%s)", caSesid);

            iReturn = pasDbExecSqlF("delete from ncsgroup where groupid in(%s)", caSesid);
            //printf("delete from ncsgroup where groupid in(%s)\n",caSesid);
            sprintf(caMsg, "delete from ncsgroup where groupid in(%s) ", caSesid);
            if(iReturn == 0)
            {
                //          ncsWriteSysLog(psMsgHead,caUsername2,"05","成功",caMsg);
            }
            else
            {
                //             ncsWriteSysLog(psMsgHead,caUsername2,"05","失败",caMsg);
            }
        }

    }

    lCurPg = atol(caPage);//当前页面
    if(lCurPg <= 0) lCurPg = 1 ;//如若当前页面小于0，置1


    if(strlen(caLimit) > 0)
    {
        lRowNum = atol(caLimit);
    }
    else
    {
        lRowNum = 40; //每一页面行数
    }

    lStartRec = (lCurPg - 1) * lRowNum;


    sprintf(caSmt, "select count(*) from ncsgroup where 1=1");

    if(strlen(caGroupname2) != 0)
    {
        sprintf(caSmt + strlen(caSmt), "  and groupname like '%c%s%c' ", '%', caGroupname2, '%');
    }

    if(strlen(caGroupcode2) > 0)
    {
        sprintf(caSmt + strlen(caSmt), " and groupcode=%s ", caGroupcode2);
    }
    if(strlen(caGroupid) > 0)
    {
        sprintf(caSmt + strlen(caSmt), " and groupid in (%s) ", getGroupid(atol(caGroupid)));
    }
    if(strlen(caGroups) > 0)
    {
        sprintf(caSmt + strlen(caSmt), " and groupid in (%s) ", caGroups);
    }

    // printf("caSmt=%s\n",caSmt);
    iReturn = pasDbOneRecord(caSmt, 0, UT_TYPE_LONG, 4, &lCount);


    if(iReturn != 0)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", "用户查询", "查询数据出错 ");
        return 0;
    }

    //  lSumHash=lCount*1.3+1;
    lSumHash = 1000;
    //printf("lSumHash=%d\n",lSumHash);
    pHash = (unsigned char *)pasLHashInit(lSumHash, lSumHash, sizeof(struct stData), 0, 4);
    if(pHash == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", "分配内存出错", "分配内存出错");
        return (-1);
    }



    sprintf(caTemp, "select groupid,groupname from ncsgroup  ");
    psCur = pasDbOpenSql(caTemp, 0);
    lGroupid = 0;
    memset(caGroupname, 0, sizeof(caGroupname));
    iReturn = pasDbFetchInto(psCur,
                             UT_TYPE_LONG, 4, &lGroupid,
                             UT_TYPE_STRING, 32, caGroupname);



    while((iReturn == 0) || (iReturn == 1405))
    {

        psData = (struct stData *)pasLHashLookA(pHash, &lGroupid);
        if(psData)
        {
            strcpy(psData->caGroupname, caGroupname);
        }
        lGroupid = 0;
        memset(caGroupname, 0, sizeof(caGroupname));
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_LONG, 4, &lGroupid,
                                 UT_TYPE_STRING, 32, caGroupname);
    }
    pasDbCloseCursor(psCur);


    /*
      sprintf(caTemp,"select count(*),groupid from ncsuser group by groupid ");
      psCur = pasDbOpenSql(caTemp,0);
         lCount_d=0;
         lGroupid=0;
          iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG,4,&lCount_d,
                                       UT_TYPE_LONG,4,&lGroupid);



      while((iReturn==0)||(iReturn==1405))
      {

           psData = (struct stData *)pasLHashLookA(pHash,&lGroupid);
         if(psData){
          psData->lCount=lCount_d;
        }
         lCount_d=0;
         lGroupid=0;
          iReturn = pasDbFetchInto(psCur,
                                       UT_TYPE_LONG,4,&lCount_d,
                                       UT_TYPE_LONG,4,&lGroupid);
      }
      pasDbCloseCursor(psCur);


      */



    sprintf(caSmt, "select groupid,groupname,pid,groupcode from ncsgroup where 1=1");
    if(strlen(caGroupid) > 0)
    {
        sprintf(caSmt + strlen(caSmt), " and groupid in (%s) ", getGroupid(atol(caGroupid)));
    }
    if(strlen(caGroups) > 0)
    {
        sprintf(caSmt + strlen(caSmt), " and groupid in (%s) ", caGroups);
    }
    if(strlen(caGroupname2) != 0)
    {
        sprintf(caSmt + strlen(caSmt), "  and groupname like '%c%s%c' ", '%', caGroupname2, '%');
    }

    if(strlen(caGroupcode2) > 0)
    {
        sprintf(caSmt + strlen(caSmt), " and groupcode=%s ", caGroupcode2);
    }

    if(strlen(caSort) > 0)
    {
        sprintf(caSmt + strlen(caSmt), "  order by pid limit %lu,%lu", lStartRec, lRowNum);
    }
    else
    {
        sprintf(caSmt + strlen(caSmt), "  order by pid limit %lu,%lu", lStartRec, lRowNum);
    }
    //printf("caSmt=%s\n",caSmt);

    psCur = pasDbOpenSql(caSmt, 0);
    if(psCur == NULL)
    {
        free(pHash);
        //    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncsLang("0613网站访问查询"),ncsLang("0549数据库出错"));
        return 0;
    }


    psDbHead = utPltInitDb();


    lGroupid = 0;
    lPid = 0;
    memset(caGroupcode, 0, sizeof(caGroupcode));
    memset(caGroupname, 0, sizeof(caGroupname));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lGroupid,
                             UT_TYPE_STRING, 30, caGroupname,
                             UT_TYPE_LONG, 4, &lPid,
                             UT_TYPE_STRING, 10, caGroupcode);
    iNum = 0;

    while(iReturn == 0 || iReturn == 1405 && iNum < lRowNum)
    {
        if(iNum > 0)
        {
            utPltPutLoopVar(psDbHead, "dh", iNum + 1, ",");
        }
        iNum++;
        utPltPutLoopVar(psDbHead, "groupname", iNum, caGroupname);
        utPltPutLoopVarF(psDbHead, "groupid", iNum, "%d", lGroupid);
        utPltPutLoopVar(psDbHead, "groupcode", iNum, caGroupcode);
        utPltPutLoopVarF(psDbHead, "pid", iNum, "%d", lPid);

        psData = (struct stData *)pasLHashLook(pHash, &lPid);
        if(psData)
        {
            utPltPutLoopVar(psDbHead, "pname", iNum, psData->caGroupname);

        }

        else
        {

            utPltPutLoopVar(psDbHead, "pname", iNum, "");

        }
        psData = (struct stData *)pasLHashLook(pHash, &lGroupid);
        if(psData)
        {
            utPltPutLoopVarF(psDbHead, "count", iNum, "%d", psData->lCount);
        }

        lGroupid = 0;
        lPid = 0;
        memset(caGroupcode, 0, sizeof(caGroupcode));
        memset(caGroupname, 0, sizeof(caGroupname));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lGroupid,
                                 UT_TYPE_STRING, 30, caGroupname,
                                 UT_TYPE_LONG, 4, &lPid,
                                 UT_TYPE_STRING, 10, caGroupcode);
    }
    pasDbCloseCursor(psCur);
    lTotPg = (lCount - 1) / lRowNum + 1;
    lTotRec = lCount;
    i = 0;

    utPltPutVar(psDbHead, "sort", caSort);
    utPltPutVar(psDbHead, "sortby", caSortby);

    utPltPutVarF(psDbHead, "TotRec", "%lu", lTotRec);
    utPltPutVarF(psDbHead, "TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead, "CurPg", "%lu", lCurPg);
    utPltPutVar(psDbHead, "groupid", caGroupid);

    if(pHash) free(pHash);


    //   ncsWriteSysLog(psMsgHead,caUsername2,"06","成功","查看上网服务场所列表信息");


    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_dwfzgl_list.htm");


    return 0;
}


//单位组树——供comp选择使用

int ncsTreeGroup_comp_v4(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caLevel[16];
    char caPlate[128];
    char caGroupname[32], caGroupid[16];

    char caName[32];
    char caWhere[256];
    char caSql[256];
    char caCurPg[16], caTotRec[16];
    char caPid[16];
    unsigned long lLevel, lPid;
    int  iNum;
    char caFilter[256];
    char caNo[16];
    unsigned long lGroupid, lGroupid0, lCurPg, lTotRec, lRowNum, lStartRec;
    utPltDbHead *psDbHead;
    int iReturn, i;
    char caVar[256], caFname[64], caBfname[64];
    char caValue[256];
    char caVar1[32];
    char caOpt[256];
    char caCode[16];
    long lId;
    char *p, *pVar;
    unsigned char *pHash;
    pasLHashInfo  sHashInfo, sHashInfo2;
    struct stData
    {
        unsigned long groupid;
        unsigned long pid;
        char caGroupname[32];
        char caCode[12];
        struct stData * pArea;  //子节点
        long lSum;             //子节点个数
    };
    char caTemp[2048];
    long lCount, lSum, lSum2, m, n;
    char caGroups[1024];
    //获取权限组ID
    strcpy(caGroups, getDsGroupid());
    struct stData *psData, *pAreastatdata, *psData2, *psData3;


#ifdef LDEBUG1
    utMsgPrintMsg(psMsgHead);
#endif
    utMsgPrintMsg(psMsgHead);



    //   iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere, "\0");
    //   if(iReturn == 0 && lGroupid0 > 0) {
    //       sprintf(caWhere,"and (groupid = %lu or pid=%lu) ",lGroupid0,lGroupid0);
    //   }

    //  sprintf(caWhere+strlen(caWhere)," and level=%d ",atol(caLevel));

    //    if(strlen(caPid)>0)
    //      sprintf(caWhere+strlen(caWhere)," and pid=%d and level>0",atol(caPid));
    //    else{
    //       sprintf(caWhere+strlen(caWhere)," and pid=0 and level=0",atol(caPid));
    //    }
    //    memset(caOpt,0,sizeof(caOpt));
    //   iReturn = dsCltGetMyInfo(1,"Userid",&lId);
    //   if(iReturn==0){
    //      pasDbOneRecord("select opt from dsuser where id=:id",1,"id",UT_TYPE_LONG,lId,UT_TYPE_STRING,255,caOpt);
    //    }



    lRowNum = utComGetVar_ld(psShmHead, "GroupTreeRow", 1000);

    pHash = (unsigned char *)pasLHashInit(lRowNum, lRowNum, sizeof(struct stData), 0, 4);

    if(pHash == NULL)
    {
        printf(" Memory HttpHash Error \n");
        return (-1);
    }
    sprintf(caTemp, "select pid,gid,gname from nwgroup where 1=1 ");

    //  if(!strstr(caOpt,"all")&&(strlen(caOpt)>0)){
    //          sprintf(caTemp+strlen(caTemp)," and (groupid in (%s) or groupid in (select groupid from ncsgroup where pid in (%s))) ",caOpt,caOpt);
    //   }
    if(strlen(caGroups) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and gid in (%s) ", caGroups);
    }
    //printf("caTemp=%s\n",caTemp);
    psCur = pasDbOpenSql(caTemp, 0);

    if(psCur == NULL)
    {
        free(pHash);
        return 0;
    }
    lGroupid = 0;
    lPid = 0;
    memset(caGroupname, 0, sizeof(caGroupname));
    memset(caCode, 0, sizeof(caCode));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lPid,
                             UT_TYPE_LONG, 4, &lGroupid,
                             UT_TYPE_STRING, 31, caGroupname);

    while(iReturn == 0 || iReturn == 1405)
    {
        psData = (struct stData *)pasLHashLookA(pHash, &lGroupid);
        if(psData)
        {
            psData->pid = lPid;
            strcpy(psData->caGroupname, caGroupname);
            //        strcpy(psData->caCode,caCode);
        }
        lGroupid = 0;
        lPid = 0;
        memset(caGroupname, 0, sizeof(caGroupname));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lPid,
                                 UT_TYPE_LONG, 4, &lGroupid,
                                 UT_TYPE_STRING, 31, caGroupname);
    }
    pasDbCloseCursor(psCur);


    pAreastatdata = (struct stData*)malloc((1000) * sizeof(struct stData));
    if(pAreastatdata == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", "分配内存出错", "分配内存出错");
        return 0;
    }
    memset(pAreastatdata, 0, (1000)*sizeof(struct stData));
    lSum = 0;

    psData = (struct stData *)pasLHashFirst(pHash, &sHashInfo);

    while(psData && lSum < 1000)
    {
        psData2 = (struct stData *)pasLHashLook(pHash, &psData->pid);
        //找不到，代表是根
        if(psData2 == NULL)
        {
            pAreastatdata[lSum].pid = psData->pid;
            pAreastatdata[lSum].groupid = psData->groupid;
            strcpy(pAreastatdata[lSum].caGroupname, psData->caGroupname);

            //分配子节点
            pAreastatdata[lSum].pArea = (struct stData*)malloc((100) * sizeof(struct stData));
            memset(pAreastatdata[lSum].pArea, 0, 100 * sizeof(struct stData));
            //获取下个节点的数据
            psData3 = (struct stData *)pasLHashFirst(pHash, &sHashInfo2);
            lSum2 = 0;
            while(psData3 && lSum2 < 100)
            {
                if(pAreastatdata[lSum].groupid == psData3->pid)
                {
                    pAreastatdata[lSum].pArea[lSum2].pid = psData3->pid;
                    pAreastatdata[lSum].pArea[lSum2].groupid = psData3->groupid;

                    strcpy(pAreastatdata[lSum].pArea[lSum2].caGroupname, psData3->caGroupname);
                    pAreastatdata[lSum].lSum++;
                    lSum2++;
                }
                psData3 = (struct stData *)pasLHashNext(&sHashInfo2);
            }

            lSum++;
        }

        psData = (struct stData *)pasLHashNext(&sHashInfo);
    }
    iNum = 0;
    psDbHead = utPltInitDb();

    for(i = 0; i < lSum; i++)
    {
        if(iNum > 0)
        {
            utPltPutLoopVar(psDbHead, "dhf", iNum + 1, ",");
        }
        utPltPutLoopVar(psDbHead, "area", iNum + 1, pAreastatdata[i].caGroupname);
        utPltPutLoopVarF(psDbHead, "groupid", iNum + 1, "%d", pAreastatdata[i].groupid);

        if(pAreastatdata[i].lSum > 0)
        {
            utPltPutLoopVar(psDbHead, "iconCls", iNum + 1, "task-folder");
        }
        else
        {
            utPltPutLoopVar(psDbHead, "iconCls", iNum + 1, "task");
        }
        //输出第二层节点
        for(m = 0; m < pAreastatdata[i].lSum; m++)
        {
            if(m > 0)
            {
                utPltPutLoopVar2(psDbHead, "dhs", iNum + 1, m + 1, ",");
            }
            utPltPutLoopVarF2(psDbHead, "groupid", iNum + 1, m + 1, "%d", pAreastatdata[i].pArea[m].groupid);
            utPltPutLoopVar2(psDbHead, "area", iNum + 1, m + 1, pAreastatdata[i].pArea[m].caGroupname);

            //输出第三层节点
            n = 0;
            psData = (struct stData *)pasLHashFirst(pHash, &sHashInfo);

            while(psData && n < 100)
            {
                if(pAreastatdata[i].pArea[m].groupid == psData->pid)
                {
                    if(n > 0)
                    {
                        utPltPutLoopVar3(psDbHead, "dht", iNum + 1, m + 1, n + 1, ",");
                    }
                    utPltPutLoopVar3(psDbHead, "area", iNum + 1, m + 1, n + 1, psData->caGroupname);
                    utPltPutLoopVarF3(psDbHead, "groupid", iNum + 1, m + 1, n + 1, "%d", psData->groupid);

                    n++;
                }
                psData = (struct stData *)pasLHashNext(&sHashInfo);

            }


        }

        iNum++;
    }

    if(pHash)  free(pHash);
    for(i = 0; i < lSum; i++)
    {
        free(pAreastatdata[lSum].pArea);
    }
    free(pAreastatdata);


    utPltPutVar(psDbHead, "rootname", utComGetVar_sd(psShmHead, "rootname", "全部部门"));

    //utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_group_select_tree.htm");


    return 0;
}
//显示角色功能列表列表
int ncsDispRoleFunlist_v4(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    int iReturn, iNum, iSum1, iSum2, k;
    long lStatus, lId, j;
    char caResult1[16000];
    char caResult2[16000];
    char caSmt[1256];
    long lPasid;
    char caPasid[16], caRoleid[16];
    char caName[32], caDispname[32], caName2[32];
    char *p, *p2;
    long lCount;
    char caTemp[1024];

    utMsgPrintMsg(psMsgHead);

    //    iReturn = utMsgGetSomeNVar(psMsgHead,1,
    //                  "roleid", UT_TYPE_STRING,12,caRoleid);
    pasDbOneRecord("select id from dsappsrv", 0, UT_TYPE_LONG, 4, &lPasid);
    iReturn = dsCltGetMyInfo(1, "Userid", &lId);
    if(iReturn != 0 || lId <= 0)
    {
        lId = 0;
    }
    lCount = 0;
    if(lId > 0)
    {
        sprintf(caTemp, "select count(*) from dsuserrole where roleid=101 and id=%d ", lId);
        pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);
    }
    if(lCount > 0 || lId == 0)
    {
        sprintf(caSmt, "select name,dispname from dscltright where pasid=%d", lPasid);
    }
    else
    {
        sprintf(caSmt, "select distinct lright,dscltright.dispname from dsroleattr,dsrole,dsuserrole,dscltright where dsrole.id=dsroleattr.roleid and dsuserrole.roleid=dsrole.id and dscltright.name=lright and dsuserrole.id=%d ", lId);

    }
    //printf("caSmt=%s\n",caSmt);
    /* 根据Roleid和Pasid取相应的权限信息  */

    /*
    sprintf(caSmt,"select name from dsrole where id=%d ",atol(caRoleid));
    pasDbOneRecord(caSmt,0,UT_TYPE_STRING,31,caName2);

     sprintf(caSmt,"select lright from dsroleattr where roleid=%s and pasid=%d",caRoleid,lPasid);

     iReturn = dsCltSqlRecordSet(caSmt,1,9999,&iSum1,caResult1);

     if(iReturn != 0 && iReturn != 1403 ) {
         iSum1 = 0;
         strcpy(caResult1,"\0");
     }
    */


    iReturn = dsCltSqlRecordSet(caSmt, 1, 9999, &iSum2, caResult2);

    psDbHead = utPltInitDb();
    p = caResult2;
    iNum = 0;
    j = 0;


    for(k = 0; k < iSum2; k++)
    {
        p = utStrGetRecordValue(p, 2,
                                UT_TYPE_STRING, 31, caName,
                                UT_TYPE_STRING, 31, caDispname);


        iNum++;
        if(iNum > 1)
        {
            utPltPutLoopVar(psDbHead, "dh", iNum, ",");
        }
        utPltPutLoopVarF(psDbHead, "vara", iNum, "v%d", k);
        utPltPutLoopVar(psDbHead, "namea", iNum, caName);
        utPltPutLoopVar(psDbHead, "dispnamea", iNum, caDispname);
        //         if(iSum1>0 && utStrIsExistRecord(caResult1,1,1,caName)) {
        //            utPltPutLoopVar(psDbHead,"checka", iNum,"checked");
        //          }
        //          else{
        //            utPltPutLoopVar(psDbHead,"checka", iNum,"checked");
        //          }

    }

    utPltPutVarF(psDbHead, "TotRec", "%d", iSum2);
    //     utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_rolefun_list.htm");
    return 0;
}



//显示角色列表
int ncsRolelist_v4(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    long iReturn, lCount;
    char caStart[16], caTemp[2048], caCurPg[16];
    char caContent[64], caKtype[8], caCaseflags[8];
    long lCurPg, lRowNum, lStartRec, sid, nettype, flags;
    char caName[64], caNetname[32], caTemp_d[2048], caStatus[16];
    char caRightcheck[2048], caRolename[64];
    char caTid[16], caKid[16];
    long lSid, lTid;
    char caPre[36];
    long iNum;
    long lktype, lcaseflags, ltid;
    char caCid[16], caLimit[16], caPage[16], caDel[16], caDir[16], caKeyword[64], caSelsid[1024], caUpdate[16], caNtype[16], caNetid[64], caSid[16];
    long preid, lId;
    char caGroupname[32];
    long lTime, lLevel, i;

    long lPasid;
    pasLHashInfo sHashInfo;
    char *pHash;
    struct rData_s
    {
        unsigned long id;
        char caFun[2048];
        char caUser[2048];
    };
    struct rData_s *psData;
    char caOut[200][256];
    long lSum;
    char caRoleid[16];
    char *tmp;
    memset(caOut, 0, 200 * 256);

    utMsgPrintMsg(psMsgHead);
    /* 根据Roleid和Pasid取相应的权限信息  */
    pasDbOneRecord("select id from dsappsrv", 0, UT_TYPE_LONG, 4, &lPasid);



    iReturn = utMsgGetSomeNVar(psMsgHead, 11,
                               "limit",            UT_TYPE_STRING, 10, caLimit,
                               "page",          UT_TYPE_STRING, 10, caPage,
                               "dir",           UT_TYPE_STRING, 8, caDir,
                               "keyword",       UT_TYPE_STRING, 30, caKeyword,
                               "selsid", UT_TYPE_STRING, 1000, caSelsid,
                               "start", UT_TYPE_STRING, 10, caStart,
                               "del", UT_TYPE_STRING, 10, caDel,
                               "update", UT_TYPE_STRING, 10, caUpdate,
                               "rolename", UT_TYPE_STRING, 31, caRolename,
                               "rightcheck", UT_TYPE_STRING, 2000, caRightcheck,
                               "roleid", UT_TYPE_STRING, 10, caRoleid);

    lRowNum = atol(caLimit);
    if(lCurPg <= 0) lCurPg = 1;
    lStartRec = atol(caStart);

    psDbHead = utPltInitDb();
    char msg[1024] = "";
    if(strcmp(caDel, "del") == 0 && strlen(caSelsid) > 0)
    {
        //     printf("delete from  dsuser  where id in(%s)\n",caSelsid);
        iReturn = pasDbExecSqlF("delete from dsuserrole where roleid  in(%s) and roleid!=101 ", caSelsid);
        iReturn = pasDbExecSqlF("delete from dsroleattr where roleid  in(%s) and roleid!=101 ", caSelsid);

        if(iReturn == 0)
        {
            pasDbExecSqlF("delete from dsrole where id  in(%s) and id!=101 ", caSelsid);
        }
        snprintf(msg, sizeof(msg), "删除了权限角色[id:%s]", caSelsid);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }

    char caMyname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", caMyname);


    if((strcmp(caUpdate, "update") == 0) && strlen(caRolename) > 0)
    {
        //printf("caRightcheck=%s\n",caRightcheck);
        lCount = sepcharbydh2(caRightcheck, caOut);

        sprintf(caTemp, "select id from dsrole where name='%s' ", utf8convert(psShmHead, caRolename));
        lId = 0;
        pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lId);
        if(lId == 0)
        {
            lId = pasGetSid("dsrole", "id");

            iReturn = pasDbExecSqlF("insert into dsrole(id,name,dispname,descr,addtime,moditime,addby) values (%lu,'%s','%s','%s',%lu,%lu,'%s')", lId, utf8convert(psShmHead, caRolename), utf8convert(psShmHead, caRolename), utf8convert(psShmHead, caRolename), time(0), time(0), caMyname);

            snprintf(msg, sizeof(msg), "增加了权限角色[%s]", utf8convert(psShmHead, caRolename));
            writeSysLog(psMsgHead, "03", "Success", msg);
            if(iReturn == 0)
            {
                pasDbExecSqlF("delete from dsroleattr where roleid=%d ", lId);
                //       printf("delete from dsroleattr where roleid=%d \n",lId);
                for(i = 0; i < lCount; i++)
                {
                    pasDbExecSqlF("insert into dsroleattr(roleid,pasid,flags,lright) values(%d,%d,0,'%s')", lId, lPasid, utf8convert(psShmHead, caOut[i]));
                }
            }
        }
        else
        {

            iReturn = pasDbExecSqlF("update dsrole set name='%s',dispname='%s',descr='%s',moditime=%lu where id=%d ", utf8convert(psShmHead, caRolename), utf8convert(psShmHead, caRolename), utf8convert(psShmHead, caRolename), time(0), lId);
            pasDbExecSqlF("delete from dsroleattr where roleid=%d ", lId);
            for(i = 0; i < lCount; i++)
            {
                if(strlen(caOut[i]) > 0)
                    pasDbExecSqlF("insert into dsroleattr(roleid,pasid,flags,lright) values(%d,%d,0,'%s')", lId, lPasid, utf8convert(psShmHead, caOut[i]));
            }
            snprintf(msg, sizeof(msg), "修改了权限角色[%s]", utf8convert(psShmHead, caRolename));
            writeSysLog(psMsgHead, "03", "Success", msg);
        }

    }
    //授权
    if((strcmp(caUpdate, "grant") == 0) && (strlen(caRoleid) > 0))
    {
        lCount = sepcharbydh2(caRightcheck, caOut);
        iReturn = pasDbExecSqlF("delete from dsuserrole where roleid=%d", atol(caRoleid));
        if(lCount > 0)
        {

            if(iReturn == 0)
            {
                for(i = 0; i < lCount; i++)
                {
                    pasDbExecSqlF("insert into dsuserrole(id,roleid,addtime,addby) values(%d,%d,%d,'')", atol(caOut[i]), atol(caRoleid), time(0));
                }
            }
        }
        snprintf(msg, sizeof(msg), "授权给了[id:%s]", caRoleid);
        writeSysLog(psMsgHead, "03", "Success", msg);
    }


    //将权限功能列表放到HASH表中

    sprintf(msg, "select count(*) from dsrole");

    lCount = 0;
    pasDbOneRecord(msg, 0, UT_TYPE_LONG, 4, &lCount);
    if(lCount < 100)
    {
        lSum = 100;
    }
    else
    {
        lSum = lCount * 1.2;
    }
    // printf("lSum=%d\n",lSum);
    pHash = (unsigned char *)pasLHashInit(lSum, lSum, sizeof(struct rData_s), 0, 4);
    if(pHash == NULL)
    {
        printf("init hash error\n");
        return 0;
    }

    sprintf(caTemp, "select roleid,lright from dsroleattr where pasid=%d order by lright", lPasid);



    psCur = pasDbOpenSql(caTemp, 0);
    //printf("caTemp=%s\n",caTemp);
    if(psCur == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "ncs/ncmsg_back.htm", "部门查询", "查询数据出错2 ");
        return 0;
    }
    lId = 0;
    memset(caGroupname, 0, sizeof(caGroupname));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId,
                             UT_TYPE_STRING, 31, caGroupname);


    while(iReturn == 0 || iReturn == 1405)
    {

        psData = (struct rData_s *)pasLHashLookA(pHash, &lId);
        if(psData)
        {
            if(strlen(psData->caFun) == 0)
            {
                strcpy(psData->caFun, caGroupname);
            }
            else
            {
                sprintf(psData->caFun + strlen(psData->caFun), ",%s", caGroupname);
            }
        }

        lId = 0;
        memset(caGroupname, 0, sizeof(caGroupname));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId,
                                 UT_TYPE_STRING, 31, caGroupname);
    }

    pasDbCloseCursor(psCur);

    //将角色对象放到hash中

    sprintf(caTemp, "select a.roleid,b.dispname from dsuserrole a,dsuser b where a.id=b.id group by b.dispname,a.roleid;");
    psCur = pasDbOpenSql(caTemp, 0);
    //printf("caTemp=%s\n",caTemp);
    if(psCur == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "ncs/ncmsg_back.htm", "部门查询", "查询数据出错2 ");
        return 0;
    }
    lId = 0;
    memset(caGroupname, 0, sizeof(caGroupname));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId,
                             UT_TYPE_STRING, 31, caGroupname);


    while(iReturn == 0 || iReturn == 1405)
    {

        psData = (struct rData_s *)pasLHashLookA(pHash, &lId);
        if(psData)
        {
            if(strlen(psData->caUser) == 0)
            {
                strcpy(psData->caUser, caGroupname);
            }
            else
            {
                sprintf(psData->caUser + strlen(psData->caUser), ",%s", caGroupname);
            }
        }

        lId = 0;
        memset(caGroupname, 0, sizeof(caGroupname));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId,
                                 UT_TYPE_STRING, 31, caGroupname);
    }

    pasDbCloseCursor(psCur);

    long lCount2;
    iReturn = dsCltGetMyInfo(1, "Userid", &lId);
    if(iReturn != 0 || lId <= 0)
    {
        lId = 0;
    }
    if(lId > 0)
    {
        sprintf(caTemp, "select count(*) from dsuserrole where id=%d ", lId);
        pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount2);
    }


    if(lCount2 > 0 || lId == 0)
    {
        sprintf(caTemp, "select count(*) from dsrole where addby='%s' ", caMyname);
    }
    else
    {
        sprintf(caTemp, "select count(distinct roleid) from dsrole,dsuserrole where dsrole.id=roleid and dsrole.addby='%s'", caMyname);
    }
    if(strlen(caKeyword) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and (name like '%c%s%c') ", '%', utf8convert(psShmHead, caKeyword), '%');
    }
    lCount = 0;

    //printf("caTemp=%s\n",caTemp);
    iReturn = pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);

    if(lCount2 > 0 || lId == 0)
    {
        sprintf(caTemp, "select id,name from dsrole where dsrole.addby='%s'", caMyname);
    }
    else
    {
        sprintf(caTemp, "select distinct roleid,name from dsrole ,dsuserrole where dsrole.id=roleid and dsuserrole.addby='%s'", caMyname);
    }

    if(strlen(caKeyword) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and (name like '%c%s%c') ", '%', utf8convert(psShmHead, caKeyword), '%');
    }
    sprintf(caTemp + strlen(caTemp), " order by name limit %d,%d", lStartRec, lRowNum);

    printf("caTemp=%s\n", caTemp);


    psCur = pasDbOpenSql(caTemp, 0);

    if(psCur == NULL)
    {
        //   utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561数据统计出错"),ncsLang("0171查询数据出错"));
        return 0;
    }


    iReturn = 0;
    iNum = 0;
    while((iReturn == 0 || iReturn == 1405) && iNum < lRowNum)
    {

        lId = 0;
        memset(caName, 0, sizeof(caName));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId,
                                 UT_TYPE_STRING, 31, caName);




        if((iReturn != 0) && (iReturn != 1405)) break;
        iNum++;
        if(iNum > 1)
        {
            utPltPutLoopVar(psDbHead, "dh", iNum, ",");
        }
        utPltPutLoopVarF(psDbHead, "num", iNum, "%lu", iNum);
        utPltPutLoopVarF(psDbHead, "userid", iNum, "%lu", lId);
        utPltPutLoopVar(psDbHead, "username", iNum, caName);

        psData = pasLHashLook(pHash, &lId);
        if(psData)
        {
            utPltPutLoopVar(psDbHead, "fun", iNum, psData->caFun);
            utPltPutLoopVar(psDbHead, "dx", iNum, psData->caUser);
        }



    }
    pasDbCloseCursor(psCur);


    utPltPutVarF(psDbHead, "TotRec", "%d", lCount);

    //  ncsWriteSysLog(psMsgHead,caUsername2,"06","成功","查看系统用户列表信息");

    free(pHash);

    //  utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_role_list.htm");
    return 0;
}
/*将字符串按分隔符‘,’分拆，
返回值为分拆的数量，caQue 输入的分拆字符串，caObj,分拆成的字符串两级指针*/
int ncs_SepChar0(char *caQue, char c, char caObj[100][50])
{
    int i = 0, j = 0, z = 0, flag = 0;
    char caTemp[100][50];
    if(strlen(caQue) == 0)
        return 0;
    while((*caQue) != '\0')
    {
        if(flag == 0)
            strcpy(caTemp[i], "");
        if((*caQue) == c)
        {
            caQue++;
            caTemp[i][j] = '\0';
            i++;
            j = 0;
            flag = 0;
        }
        else
        {
            caTemp[i][j] = (*caQue);
            caQue++;
            j++;
            flag = 1;
        }
        if(j > 49 || i > 99)
            break;
    }
    caTemp[i][j] = '\0';
    for(z = 0; z < i + 1; z++)
    {
        strcpy(caObj[z], caTemp[z]);
    }
    return i + 1;
}
//判断 是否包含这个ID,是返回1，否返回0
//pMg-1111,1111,11133格式的字符串，id为某个字符串的ID值
int isGroupID(char *pMg, char *id)
{
    char caSerID[100][50];
    long num, i;

    num = ncs_SepChar0(pMg, ',', caSerID);
    for(i = 0; i < num; i++)
    {
        if(strcmp(id, caSerID[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

//显示系统用户管理
int ncsSysUserlist_v4(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    long iReturn, lCount;
    char caStart[16], caTemp[2048], caCurPg[16];
    char caContent[64], caKtype[8], caCaseflags[8];
    long lCurPg, lRowNum, lStartRec, sid, nettype, flags;
    char caName[64], caNetname[32], caTemp_d[128], caStatus[16];
    char caTid[16], caKid[16];
    long lSid, lTid;
    char caPre[36];
    long iNum;
    long lktype, lcaseflags, ltid;
    char caCid[16], caLimit[16], caPage[16], caDel[16], caDir[16], caKeyword[64], caSelsid[1024], caUpdate[16], caNtype[16], caNetid[64], caSid[16];
    long preid, lId;
    char caGroupname[32];
    long lTime, lLevel, i;
    char opt_desc[1024], caOpt[256];
    char caSort[20];
    char caTelphone[64], caHandphone[32], caGroup[32], caEmail[64], caDispname[32], caGroupid[32];
    struct groupdate
    {
        char id[16];
        char name[32];
    };
    struct groupdate *pGdate, *pFdate;
    char caCode[16], caFac[256], fac_desc[256];
    long lFcount, lId_s;
    struct roledate
    {
        long id;
        char name[512];
    };
    struct roledate *pRole;
    long lRolecount, lGroupcount;

    char caGroups[1024];
    strcpy(caGroups, getDsGroupid());//部门id的列表
    printf("[group list = %s]\n", caGroups);
    char caMyname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", caMyname);//pronetwayadmin


    lId_s = 0;
    iReturn = dsCltGetMyInfo(1, "Userid", &lId_s);



    //用户角色
    pasDbOneRecord("select count(distinct id) from dsuserrole", 0, UT_TYPE_LONG, 4, &lCount);

    pRole = (struct roledate*)malloc(lCount * sizeof(struct roledate));
    if(pRole == NULL)
    {
        //       utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm","部门查询","查询数据出错1 ");
        return 0;
    }

    sprintf(caTemp, "select b.id,a.dispname from dsuserrole b,dsrole a where a.id=b.roleid and b.addby='%s' order by b.id", caMyname);

    psCur = pasDbOpenSql(caTemp, 0);

    if(psCur == NULL)
    {
        //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm","部门查询","查询数据出错2 ");
        return 0;
    }
    lId = 0;
    memset(caGroupname, 0, sizeof(caGroupname));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId,
                             UT_TYPE_STRING, 31, caGroupname);
    lRolecount = 0;
    preid = -1;

    while(iReturn == 0 || iReturn == 1405)
    {
        if(preid != lId)
        {
            pRole[lRolecount].id = lId;

            strcpy(pRole[lRolecount].name, caGroupname);
            lRolecount++;
            preid = lId;
        }
        else
        {
            if(strlen(pRole[lRolecount - 1].name) < 490)
                sprintf(pRole[lRolecount - 1].name + strlen(pRole[lRolecount - 1].name), ",%s", caGroupname);

        }
        lId = 0;
        memset(caGroupname, 0, sizeof(caGroupname));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId,
                                 UT_TYPE_STRING, 31, caGroupname);
    }
    pasDbCloseCursor(psCur);

    //管理区域
    pasDbOneRecord("select count(*) from nwgroup", 0, UT_TYPE_LONG, 4, &lCount);
    pGdate = (struct groupdate*)malloc(lCount * sizeof(struct groupdate));
    if(pGdate == NULL)
    {
        //      utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm","部门查询","查询???? ");
        return 0;
    }

    sprintf(caTemp, "select gid,gname from nwgroup");


    psCur = pasDbOpenSql(caTemp, 0);

    if(psCur == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "ncs/ncmsg_back.htm", "部门查询", "查询数据出错4 ");
        return 0;
    }
    lId = 0;
    memset(caGroupname, 0, sizeof(caGroupname));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId,
                             UT_TYPE_STRING, 31, caGroupname);
    lGroupcount = 0;
    while(iReturn == 0 || iReturn == 1405)
    {
        sprintf(pGdate[lGroupcount].id, "%d", lId);
        strcpy(pGdate[lGroupcount].name, caGroupname);
        lGroupcount++;
        lId = 0;
        memset(caGroupname, 0, sizeof(caGroupname));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId,
                                 UT_TYPE_STRING, 31, caGroupname);
    }

    pasDbCloseCursor(psCur);







    utMsgPrintMsg(psMsgHead);
    memset(caPre, 0, sizeof(caPre));
    iReturn = utMsgGetSomeNVar(psMsgHead, 10,
                               "groupid", UT_TYPE_STRING, 12, caGroupid,
                               "limit",            UT_TYPE_STRING, 10, caLimit,
                               "page",          UT_TYPE_STRING, 10, caPage,
                               "del",           UT_TYPE_STRING, 8, caDel,
                               "sort",          UT_TYPE_STRING, 15, caSort,
                               "dir",           UT_TYPE_STRING, 8, caDir,
                               "keyword",       UT_TYPE_STRING, 30, caKeyword,
                               "selsid", UT_TYPE_STRING, 1000, caSelsid,
                               "start", UT_TYPE_STRING, 10, caStart,
                               "del", UT_TYPE_STRING, 10, caDel);
    if(strcmp(caSort, "username") == 0)
    {
        strcpy(caSort, "name");
    }
    else if(strcmp(caSort, "groupname") == 0)
    {
        strcpy(caSort, "a.groupid");
    }
    if(strlen(caSort) == 0)
    {
        strcpy(caSort, "a.groupid");
    }
    if(strlen(caDir) == 0)
    {
        strcpy(caDir, "desc");
    }


    if(strlen(caKeyword) > 0)
    {
        strcpy(caTemp_d, pasCvtGBK(2, caKeyword, caTemp, 63));
        strcpy(caKeyword, caTemp_d);
    }


    lRowNum = atol(caLimit);
    if(lCurPg <= 0) lCurPg = 1;
    lStartRec = atol(caStart);

    psDbHead = utPltInitDb();


    if(strcmp(caDel, "del") == 0 && strlen(caSelsid) > 0)
    {
        //     printf("delete from  dsuser  where id in(%s)\n",caSelsid);
        pasDbExecSqlF("delete from  dsuser  where id in(%s)", caSelsid);
    }




    sprintf(caTemp, "select count(*) from dsuser where 1=1 ");
    if(strlen(caGroupid) > 0)
        sprintf(caTemp + strlen(caTemp), " and a.groupid in(%s)  ", getGroupid(atol(caGroupid)));
    if(strlen(caGroups) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and a.groupid in (%s) ", caGroups);
    }

    if(strlen(caKeyword) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and (a.name like '%c%s%c' or a.groupname like '%c%s%c' or a.dispname like '%c%s%c' or a.email like '%c%s%c') ", '%', caKeyword, '%', '%', caKeyword, '%', '%', caKeyword, '%', '%', caKeyword, '%');
    }


    lCount = 0;
    iReturn = pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);


    //sprintf(caTemp, "select a.id,a.name,a.dispname,a.groupname,a.addtime,a.email,a.usrlevel,b.gname,opt from dsuser a left join nwgroup b on a.groupid=b.gid where a.groupid=(select groupid from dsuser where name='%s') and a.dispname <> (select dispname from dsuser where name = '%s')", caMyname, caMyname);
    sprintf(caTemp, "select a.id,a.name,a.dispname,a.groupname,a.addtime,a.email,a.usrlevel,b.gname,opt from dsuser a left join nwgroup b on a.groupid=b.gid where a.dispname <> (select dispname from dsuser where name = '%s')", caMyname, caMyname);

    if(strlen(caGroupid) > 0)
        sprintf(caTemp + strlen(caTemp), " and a.groupid in(%s)  ", getGroupid(atol(caGroupid)));
    if(strlen(caGroups) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and a.groupid in (%s) ", caGroups);
    }

    if(strlen(caKeyword) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and (a.name like '%c%s%c' or a.groupname like '%c%s%c' or a.dispname like '%c%s%c' or a.email like '%c%s%c') ", '%', caKeyword, '%', '%', caKeyword, '%', '%', caKeyword, '%', '%', caKeyword, '%');
    }
    sprintf(caTemp + strlen(caTemp), " order by %s %s limit %d,%d", caSort, caDir, lStartRec, lRowNum);

    printf("caTemp=%s\n", caTemp);


    psCur = pasDbOpenSql(caTemp, 0);

    if(psCur == NULL)
    {
        //      utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561数据统计出错"),ncsLang("0171查询数据出错"));
        return 0;
    }


    iReturn = 0;
    iNum = 0;
    while((iReturn == 0 || iReturn == 1405) && iNum < lRowNum)
    {

        memset(caGroupname, 0, sizeof(caGroupname));
        memset(caHandphone, 0, sizeof(caHandphone));
        memset(caTelphone, 0, sizeof(caTelphone));
        memset(caEmail, 0, sizeof(caEmail));
        memset(caDispname, 0, sizeof(caDispname));
        memset(caOpt, 0, sizeof(caOpt));
        memset(caFac, 0, sizeof(caFac));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lId,
                                 UT_TYPE_STRING, 31, caName,
                                 UT_TYPE_STRING, 31, caDispname,
                                 UT_TYPE_STRING, 31, caGroup,
                                 UT_TYPE_LONG,   4, &lTime,
                                 UT_TYPE_STRING, 63, caEmail,
                                 UT_TYPE_LONG, 4, &lLevel,
                                 UT_TYPE_STRING, 31, caGroupname,
                                 UT_TYPE_STRING, 255, caOpt);





        if((iReturn != 0) && (iReturn != 1405)) break;
        iNum++;
        if(iNum > 1)
        {
            utPltPutLoopVar(psDbHead, "dh", iNum, ",");
        }
        utPltPutLoopVarF(psDbHead, "num", iNum, "%lu", iNum);
        utPltPutLoopVarF(psDbHead, "userid", iNum, "%lu", lId);
        utPltPutLoopVar(psDbHead, "username", iNum, caName);
        utPltPutLoopVar(psDbHead, "dispname", iNum, caDispname);
        utPltPutLoopVar(psDbHead, "groupname", iNum, caGroupname);
        utPltPutLoopVar(psDbHead, "email", iNum, caEmail);
        utPltPutLoopVar(psDbHead, "addtime", iNum, utTimFormat("%Y-%m-%d %H:%M:%S", lTime));
        //       utPltPutLoopVar(psDbHead,"level",iNum,ncsGetFirstNameById(lLevel));
        utPltPutLoopVar(psDbHead, "telphone", iNum, caTelphone);
        utPltPutLoopVar(psDbHead, "handphone", iNum, caHandphone);

        if(strstr(caOpt, "all") || (strlen(caOpt) == 0))
        {
            utPltPutLoopVar(psDbHead, "opt", iNum, ncLang("0011全部"));
        }
        else
        {
            memset(opt_desc, 0, sizeof(opt_desc));
            for(i = 0; i < lGroupcount; i++)
            {
                //        if(strstr(caOpt,pGdate[i].id))
                if(isGroupID(caOpt, pGdate[i].id) == 1)
                    sprintf(opt_desc + strlen(opt_desc), "%s ", pGdate[i].name);

            }
            utPltPutLoopVar(psDbHead, "opt", iNum, opt_desc);
        }





        for(i = 0; i < lRolecount; i++)
        {
            if(pRole[i].id == lId)
            {
                utPltPutLoopVar(psDbHead, "role", iNum, pRole[i].name);
                break;
            }
        }

    }
    pasDbCloseCursor(psCur);


    utPltPutVarF(psDbHead, "TotRec", "%d", lCount);

    //  ncsWriteSysLog(psMsgHead,caUsername2,"06","成功","查看系统用户列表信息");

    free(pGdate);
    //  free(pFdate);
    free(pRole);


    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_sysuser_list.htm");
    return 0;
}

/* 显示增加/修改用户的界面  */
int ncsWebAdminDispUserForm(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned long lId, lGroupid, lTime, lTime1, lLevel, lUserid;
    char caName[32], caGroup[32], caDispname[32], caEmail[64], caUserid[16];
    char caWhere[256], caRoleid[16], caGroupid[16];
    char *p;
    unsigned long groupid;
    char groupname[32];
    char caResult[16000], caResult1[8192], caSmt[256];
    utPltDbHead *psDbHead;
    char *pBuf, *pSelect, caLevel[12];
    int iReturn, iRetSum, i, j, k, iSum1, iSum2, iNum, iReturn1, lUseflags;
    pasDbCursor *psCur;
    char caAddress[128];
    char caTelphone[64], caHandphone[32], caOpt[256];
    char caTemp[1024];
    char opt_desc[1024];
    char caUsername2[32];
    char caFac[256], fac_desc[1024];
    char caMsg[1024];
    dsCltGetSessionValue(2, "dispname", UT_TYPE_STRING, 30, caUsername2, "id", UT_TYPE_LONG, 4, &lId);
    iReturn = utMsgGetSomeNVar(psMsgHead, 1,
                               "groupid", UT_TYPE_STRING, 12, caGroupid);
    psDbHead = utPltInitDb();

    // printf("增加用户: here1\n");
    psCur = pasDbOpenSql("select gid,gname from nwgroup order by gid desc ", 0);
    iReturn = pasDbFetchInto(psCur,
                             UT_TYPE_LONG, 4, &groupid,
                             UT_TYPE_STRING, 31, groupname);
    iNum = 0;
    while((iReturn == 0) || (iReturn == 1405))
    {

        iNum++;
        utPltPutLoopVarF(psDbHead, "groupid",   iNum, "%lu", groupid);
        utPltPutLoopVar(psDbHead, "groupname", iNum, groupname);
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_LONG, 4, &groupid,
                                 UT_TYPE_STRING, 31, groupname);
    }
    pasDbCloseCursor(psCur);



    // printf("增加用户: here2\n");

    iReturn = utMsgGetSomeNVar(psMsgHead, 1,
                               "userid",    UT_TYPE_STRING, 15, caUserid);
    lUserid = atol(caUserid);
    if(!utStrIsSpaces(caUserid))   /* 修改 */
    {
        lUserid = atol(caUserid);
        memset(caName, 0, sizeof(caName));
        memset(caDispname, 0, sizeof(caDispname));
        memset(caAddress, 0, sizeof(caAddress));
        memset(caTelphone, 0, sizeof(caTelphone));
        memset(caHandphone, 0, sizeof(caHandphone));
        memset(caOpt, 0, sizeof(caOpt));
        memset(caEmail, 0, sizeof(caEmail));
        memset(caFac, 0, sizeof(caFac));
        sprintf(caTemp, "select id,name,dispname,groupid,usrlevel,useflags,opt,email from dsuser where id=%lu ", lUserid);
        pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lUserid,
                       UT_TYPE_STRING, 31, caName,
                       UT_TYPE_STRING, 31, caDispname,
                       UT_TYPE_LONG, 4, &lGroupid,
                       UT_TYPE_LONG, 4, &lLevel,
                       UT_TYPE_LONG, 4, &lUseflags,
                       UT_TYPE_STRING, 254, caOpt,
                       UT_TYPE_STRING, 63, caEmail);

        sprintf(caTemp, "select gname from nwgroup where gid=%d ", lGroupid);
        memset(caGroup, 0, sizeof(caGroup));
        pasDbOneRecord(caTemp, 0, UT_TYPE_STRING, 31, caGroup);


        if(lUserid != 0)
            utPltPutVar(psDbHead, "readonly", "readonly");

        utPltPutVarF(psDbHead, "userid", "%lu", lUserid);
        utPltPutVar(psDbHead, "name",  caName);
        utPltPutVar(psDbHead, "dispname",  caDispname);
        utPltPutVar(psDbHead, "groupname", caGroup);
        utPltPutVarF(psDbHead, "groupid", "%lu", lGroupid);
        utPltPutVar(psDbHead, "email",     caEmail);

        if(lLevel == 0)
            utPltPutVar(psDbHead, "rightlev", ncLang("0009系统管理员"));
        else
            utPltPutVar(psDbHead, "rightlev", ncLang("0010一般操作员"));

        utPltPutVarF(psDbHead, "level", "%d", lLevel);
        utPltPutVarF(psDbHead, "useflags", "%d", lUseflags);
        utPltPutVar(psDbHead, "address", caAddress);
        utPltPutVar(psDbHead, "telphone", caTelphone);
        utPltPutVar(psDbHead, "handphone", caHandphone);
        utPltPutVar(psDbHead, "opt", caOpt);
        utPltPutVar(psDbHead, "fac", caFac);
        if(strstr(caOpt, "all"))
        {
            utPltPutVar(psDbHead, "opt_desc", ncLang("0011全部"));
        }
        else
        {
            if(strlen(caOpt) > 0)
            {
                sprintf(caTemp, "select gname from nwgroup where gid in (%s) ", caOpt);
                psCur = pasDbOpenSql(caTemp, 0);
                if(psCur)
                {
                    iReturn = pasDbFetchInto(psCur,
                                             UT_TYPE_STRING, 31, groupname);
                    iNum = 0;
                    while((iReturn == 0) || (iReturn == 1405))
                    {
                        if(iNum == 0)
                            sprintf(opt_desc, "%s", groupname);
                        else
                            sprintf(opt_desc + strlen(opt_desc), ",%s", groupname);
                        iReturn = pasDbFetchInto(psCur,
                                                 UT_TYPE_STRING, 31, groupname);
                        iNum++;
                    }
                    pasDbCloseCursor(psCur);
                    utPltPutVar(psDbHead, "opt_desc", opt_desc);
                }
            }
        }



        utPltPutVar(psDbHead, "email", caEmail);

    }
    else   /* 增加新记录  */
    {
        sprintf(caTemp, "select usrlevel from dsuser where id=%d ", lId);
        //    printf("caTemp=%s\n",caTemp);
        pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lLevel);

        utPltPutVar(psDbHead, "groupid", caGroupid);
        utPltPutVar(psDbHead, "useflags", "1");
        utPltPutVar(psDbHead, "opt_desc", ncLang("0011全部"));
        utPltPutVar(psDbHead, "opt", "all");
        utPltPutVar(psDbHead, "rightlev", ncLang("0010一般操作员"));
        utPltPutVarF(psDbHead, "level", "%d", lLevel);
    }

    char auth[32] = "";
    char shortname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", auth);

    findShortName(auth, shortname);

    char sql[128] = "";
    sprintf(sql, "select keyname,keyvalue from paramsmanage where paramtype=1 and company='%s' order by keyvalue", shortname);

    printf("参数设置：sql:%s\n", sql);
    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        char keyname[32] = "";
        char keyvalue[32] = "";

        int iret = 0, iNum = 0;
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_STRING, sizeof(keyname) - 1,  keyname,
                                          UT_TYPE_STRING, sizeof(keyvalue) - 1, keyvalue)) || 1405 == iret)
        {

            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            else
            {
                utPltPutLoopVar(psDbHead, "selected", iNum, "selected");
            }
            utPltPutLoopVar(psDbHead, "rightn", iNum, keyname);
            utPltPutLoopVar(psDbHead, "rightv", iNum, keyvalue);
        }

        pasDbCloseCursor(psCur);
    }


    // printf("增加用户: here3\n");

    /* 显示角色信息  */
    if(lUserid == 0)
    {
        iSum1 = 0;
    }
    else
    {
        sprintf(caSmt, "select roleid from dsuserrole where id= %lu ", lUserid);

        iReturn = dsCltSqlRecordSet(caSmt, 1, 9999, &iSum1, caResult1);
        if(iReturn != 0) iSum1 = 0;
    }
    char caMyname[32] = "";
    dsCltGetMyInfo(1, "USERNAME", caMyname);
    sprintf(caSmt, "select id,dispname from dsrole where addby = '%s'", caMyname);

    printf("增加用户: here4%s\n", caSmt);
    iReturn = dsCltSqlRecordSet(caSmt, 1, 9999, &iSum2, caResult);
    if(iReturn != 0) iSum2 = 0;
    iNum = 0;
    j = 0;

    //printf("增加用户: here4\n");
    p = caResult;
    char checked[8] = "";
    utPltPutVarF(psDbHead, "sumrole", "%d", iSum2);
    for(k = 0; k < iSum2; k++)
    {
        p = utStrGetRecordValue(p, 2,
                                UT_TYPE_STRING, 12, caRoleid,
                                UT_TYPE_STRING, 31, caDispname);
        if(j == 0)
        {
            iNum++;
            if(iSum1 > 0 && utStrIsExistRecord(caResult1, 1, 1, caRoleid))
            {
                strcpy(checked, "checked");
            }
            sprintf(caTemp, "<td  valign='bottom' height='21'> <div align='right'> <input type='checkbox' name='v%d' value='%s' %s></div></td><td  valign='bottom' height='21'>%s</td>", k, caRoleid, checked, caDispname);
            printf("caTemp=%s\n", caTemp);
            utPltPutLoopVar(psDbHead, "vara", iNum, caTemp);

            j++;
        }
        else if(j == 1)
        {
            if(iSum1 > 0 && utStrIsExistRecord(caResult1, 1, 1, caRoleid))
            {
                strcpy(checked, "checked");
            }
            sprintf(caTemp, "<td  valign='bottom' height='21'> <div align='right'> <input type='checkbox' name='v%d' value='%s' %s></div></td><td  valign='bottom' height='21'>%s</td>", k, caRoleid, checked, caDispname);
            printf("caTemp=%s\n", caTemp);
            utPltPutLoopVar(psDbHead, "varb", iNum, caTemp);
            j = 0;
        }
    }

    //printf("增加用户: here5\n");
    utPltPutVarF(psDbHead, "sumright", "%lu", iSum2);
    if(j == 1)
    {
        utPltPutLoopVar(psDbHead, "roleb", iNum, "noneb");
        utPltPutLoopVar(psDbHead, "dispnameb", iNum, ncLang("0012预留"));
        utPltPutLoopVar(psDbHead, "checkb", iNum, "disabled");
    }

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncadminuser.htm");
    return 0;
}

/* 增加或修改记录信息  */
int ncsAdminUserSave(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char caUserid[16];
    char caUsername[32];
    char caPassword1[32], caPassword2[32];
    char caGroupname[32], caDispname[32], caGroupid[16];
    char caLevel[16], caMyname[32];
    char caEmail[64];
    char caKey[32], caKey0[64];
    char caSum[16];
    char caSql[8192];
    char caVar[32];
    char *p;
    char caTelphone[64];
    char caHandphone[32];
    char caUserflags[8];
    char caOpt1[256];
    char caAddress[128], caUseflags[16];
    unsigned long lTime, lUserid, lUsrlevel, lGroupid, lRoleid;
    int iReturn, i, iSum;
    char caUsername2[32];
    char caKey2[64], caKey22[64];
    char caFac[256];
    char caMsg[1024];
    utPltDbHead *psDbHead;
    memset(caUsername2, 0, sizeof(caUsername2));
    dsCltGetSessionValue(1, "dispname", UT_TYPE_STRING, 30, caUsername2);
    memset(caGroupname, 0, sizeof(caGroupname));
    lTime = time(0);
    utMsgPrintMsg(psMsgHead);
    printf(" Pid==%d \n", getpid());
    iReturn = utMsgGetSomeNVar(psMsgHead, 17,
                               "userid",      UT_TYPE_STRING, 10, caUserid,
                               "name",        UT_TYPE_STRING, 31, caUsername,
                               "dispname",    UT_TYPE_STRING, 31, caDispname,
                               "password",    UT_TYPE_STRING, 31, caPassword1,
                               "password2",   UT_TYPE_STRING, 31, caPassword2,
                               "key2",        UT_TYPE_STRING, 31, caKey2,
                               "key22",       UT_TYPE_STRING, 31, caKey22,
                               "groupid",     UT_TYPE_STRING, 10, caGroupid,
                               "usrlevel",    UT_TYPE_STRING, 31, caLevel,
                               "email",       UT_TYPE_STRING, 63, caEmail,
                               "sumright",    UT_TYPE_STRING, 10, caSum,
                               "telphone",    UT_TYPE_STRING, 63, caTelphone,
                               "handphone",   UT_TYPE_STRING, 31, caHandphone,
                               "useflags",    UT_TYPE_STRING, 2, caUseflags,
                               "opt1",        UT_TYPE_STRING, 255, caOpt1,
                               "address",     UT_TYPE_STRING, 127, caAddress,
                               "fac",         UT_TYPE_STRING, 255, caFac);
    lUserid = atol(caUserid);
    lGroupid = atol(caGroupid);
    iSum = atol(caSum);
    if(strlen(caLevel) == 0)
    {
        strcpy(caLevel, "1");
    }

    iReturn = dsCltGetMyInfo(1, "USERNAME", caMyname);
    sprintf(caSql, "select groupid from dsuser where name = '%s'", caMyname);

    unsigned long groupid = 0;
    pasDbOneRecord(caSql, 0, UT_TYPE_ULONG, sizeof(groupid), &groupid);

    sprintf(caSql, "select count(*) from dsuser where name = '%s' and groupid = %u", caUsername, groupid);
    unsigned long count = 0;
    pasDbOneRecord(caSql, 0, UT_TYPE_ULONG, sizeof(count), &count);

    if(count == 0 || !utStrIsSpaces(caUserid))
    {
        lUsrlevel = atol(caLevel);

        if(iReturn < 0)
        {
            strcpy(caMyname, "\0");
        }
        if(strcmp(caPassword1, caPassword2) != 0)
        {
            utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0013增加用户"), ncLang("0014口令不相等"));
            return 0;
        }
        if(strcmp(caKey2, caKey22) != 0)
        {
            utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0013增加用户"), ncLang("0015第二密码不相等"));
            return 0;
        }



        if(utStrIsSpaces(caUserid) || lUserid == 0)   /* 新增记录  */
        {
            lUserid = dsCltGetSeque();
            if(lUserid > 0)
            {
                if(utStrIsSpaces(caPassword1))
                {
                    utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0013增加用户"), ncLang("0016口令不能为空"));
                    return 0;
                }
                sprintf(caKey0, "%s%s", caUsername, caPassword1);
                utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey); /* 口令加密  */
                sprintf(caSql, "<DSSQL>insert into dsuser (id,name,dispname,groupid,groupname,addtime,\
	                              moditime,lkey,lasttime,logcount,useflags,status,usrlevel,email,opt) values \
	                           (%lu,'%s','%s','%u','%s',%lu,\
	                           %lu,'%s',%lu,0,0,0,\
	                           %d,'%s','%s')</DSSQL>\n",
                        lUserid, caUsername, caDispname, lGroupid, caGroupname, lTime, lTime,
                        caKey, lTime, lUsrlevel, caEmail, caOpt1);

                for(i = 0; i < iSum; i++)
                {
                    sprintf(caVar, "v%d", i);
                    p = utMsgGetVar_s(psMsgHead, caVar);
                    if(p)
                    {
                        lRoleid = atol(p);
                        sprintf(caSql + strlen(caSql), "<DSSQL>insert into dsuserrole(id,roleid,addtime,addby) values \
	                              (%lu,%lu,%lu,'%s')</DSSQL>\n",
                                lUserid, lRoleid, lTime, caMyname);
                    }
                }

                printf("here : %s\n", caSql);
                iReturn = dsCltSqlExecSome(caSql);
                if(iReturn == 0)
                {
                    psDbHead = utPltInitDb();
                    utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/ncadminuserlist.htm");
                    //           ncsWebAdminUserList(psShmHead, iFd,psMsgHead);
                    //          ncsWriteSysLog(psMsgHead,caUsername2,"05","成功",caSql);
                }
                else
                {
                    printf("error here\n");
                    utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0013增加用户"), ncLang("0017用户增加出错"));
                    return 0;
                }
            }
            else
            {
                utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0013增加用户"), ncLang("0017用户增加出错"));
                return 0;
            }
        }
        else   /* 修改记录  */
        {
            memset(caSql, 0, sizeof(caSql));
            utStrAddF(caSql, "<DSSQL>update dsuser set name='%s',dispname='%s',groupid=%lu,\
	                            groupname='%s',opt='%s',usrlevel=%d",
                      caUsername, caDispname, lGroupid, caGroupname, caOpt1, lUsrlevel);

            if(!utStrIsSpaces(caPassword1))
            {
                sprintf(caKey0, "%s%s", caUsername, caPassword1);
                utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey); /* 口令加密  */
                utStrAddF(caSql, ",lkey='%s'", caKey);
            }
            if((!utStrIsSpaces(caKey2)) && (strcasecmp(utComGetVar_sd(psShmHead, "wzflag", "No"), "Yes") == 0))
            {

                utStrAddF(caSql, ",key2='%s'", caKey2);
            }


            utStrAddF(caSql, ",moditime=%lu,lasttime=%lu,useflags=%lu,email='%s' where id=%lu </DSSQL>\n",
                      lTime, lTime, atol(caUseflags), caEmail, lUserid);

            utStrAddF(caSql, "<DSSQL>delete from dsuserrole where id = %lu </DSSQL>\n", lUserid);
            for(i = 0; i < iSum; i++)
            {
                sprintf(caVar, "v%d", i);
                p = utMsgGetVar_s(psMsgHead, caVar);
                if(p)
                {
                    lRoleid = atol(p);
                    utStrAddF(caSql, "<DSSQL>insert into dsuserrole(id,roleid,addtime,addby) values \
	                          (%lu,%lu,%lu,'%s')</DSSQL>\n",
                              lUserid, lRoleid, lTime, caMyname);
                }
            }

            iReturn = dsCltSqlExecSome(caSql);
            if(iReturn == 0)
            {
                psDbHead = utPltInitDb();
                utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/ncadminuserlist.htm");
                //           ncsWriteSysLog(psMsgHead,caUsername2,"05","成功",caSql);
            }
            else
            {
                utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0013增加用户"), ncLang("0018用户修改错误%d"), iReturn);

            }
        }
    }
    else
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0013增加用户"), ncLang("0019用户名已存在"));
    }
    return 0;
}


//显示软件下载界面
int lanSoftlist_v4(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    long iReturn, lCount;
    char caStart[16], caTemp[2048], caCurPg[16];

    long lCurPg, lRowNum, lStartRec;
    char caTemp_d[128];
    long lSid;
    long iNum;
    char caLimit[16], caPage[16], caDel[16], caDir[16], caKeyword[64], caSelsid[1024], caUpdate[16], caSid[16];
    char caSort[20];
    char caSoftname[32], caFilename[128], caVersion[16], caOpertor[32];
    long lFilesize, lUploadtime, lStatus, lFlags, lLasttime;
    char caStatus[16];
    char caApp[32];
    utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead, 11,
                               "limit",            UT_TYPE_STRING, 10, caLimit,
                               "page",          UT_TYPE_STRING, 10, caPage,
                               "del",           UT_TYPE_STRING, 8, caDel,
                               "sort",          UT_TYPE_STRING, 15, caSort,
                               "dir",           UT_TYPE_STRING, 8, caDir,
                               "keyword",       UT_TYPE_STRING, 30, caKeyword,
                               "selsid", UT_TYPE_STRING, 1000, caSelsid,
                               "status", UT_TYPE_STRING, 10, caStatus,
                               "start", UT_TYPE_STRING, 10, caStart,
                               "del", UT_TYPE_STRING, 10, caDel,
                               "app", UT_TYPE_STRING, 10, caApp);

    if(strlen(caKeyword) > 0)
    {
        strcpy(caTemp_d, pasCvtGBK(2, caKeyword, caTemp, 63));
        strcpy(caKeyword, caTemp_d);
    }


    lRowNum = atol(caLimit);
    if(lCurPg <= 0) lCurPg = 1;
    lStartRec = atol(caStart);

    psDbHead = utPltInitDb();



    if(strcmp(caDel, "del") == 0 && strlen(caSelsid) > 0)
    {
        //     printf("delete from  dsuser  where id in(%s)\n",caSelsid);
        sprintf(caTemp, "select filename from ncupsoftlist where sid in(%s)", caSelsid);
        psCur = pasDbOpenSql(caTemp, 0);
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_STRING, 127, caFilename);
        while(iReturn == 0 || iReturn == 1405)
        {
            sprintf(caTemp, "/home/ncmysql/nw/upload/soft/%s", caFilename);
            remove(caTemp);
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_STRING, 127, caFilename);
        }
        pasDbCloseCursor(psCur);


        pasDbExecSqlF("delete from  ncupsoftlist  where sid in(%s)", caSelsid);

    }




    sprintf(caTemp, "select count(*) from ncupsoftlist where 1=1 ");
    if(strlen(caStatus) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and status=%s ", caStatus);
    }

    if(strlen(caKeyword) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and (softname like '%c%s%c' or filename like '%c%s%c' or version like '%c%s%c' or note like '%c%s%c') ", '%', caKeyword, '%', '%', caKeyword, '%', '%', caKeyword, '%', '%', caKeyword, '%');
    }


    lCount = 0;
    iReturn = pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);


    sprintf(caTemp, "select sid,softname,filename,version,filesize,uploadtime,status,flags,lasttime,opertor from ncupsoftlist  where 1=1 ");

    if(strlen(caStatus) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and status=%s ", caStatus);
    }

    if(strlen(caKeyword) > 0)
    {
        sprintf(caTemp + strlen(caTemp), " and (softname like '%c%s%c' or filename like '%c%s%c' or version like '%c%s%c' or note like '%c%s%c') ", '%', caKeyword, '%', '%', caKeyword, '%', '%', caKeyword, '%', '%', caKeyword, '%');
    }
    sprintf(caTemp + strlen(caTemp), " order by sid desc limit %d,%d", lStartRec, lRowNum);

    printf("caTemp=%s\n", caTemp);


    psCur = pasDbOpenSql(caTemp, 0);

    if(psCur == NULL)
    {
        //      utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561数据统计出错"),ncsLang("0171查询数据出错"));
        return 0;
    }


    iReturn = 0;
    iNum = 0;
    while((iReturn == 0 || iReturn == 1405) && iNum < lRowNum)
    {

        lSid = 0;
        memset(caSoftname, 0, sizeof(caSoftname));
        memset(caFilename, 0, sizeof(caFilename));
        memset(caVersion, 0, sizeof(caVersion));
        lFilesize = 0;
        lUploadtime = 0;
        lStatus = 0;
        lFlags = 0;
        lLasttime = 0;
        memset(caOpertor, 0, sizeof(caOpertor));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_LONG, 4, &lSid,
                                 UT_TYPE_STRING, 31, caSoftname,
                                 UT_TYPE_STRING, 127, caFilename,
                                 UT_TYPE_STRING, 15, caVersion,
                                 UT_TYPE_LONG,   4, &lFilesize,
                                 UT_TYPE_LONG, 4, &lUploadtime,
                                 UT_TYPE_LONG, 4, &lStatus,
                                 UT_TYPE_LONG, 4, &lFlags,
                                 UT_TYPE_LONG, 4, &lLasttime,
                                 UT_TYPE_STRING, 31, caOpertor);






        if((iReturn != 0) && (iReturn != 1405)) break;
        iNum++;
        if(iNum > 1)
        {
            utPltPutLoopVar(psDbHead, "dh", iNum, ",");
        }
        utPltPutLoopVarF(psDbHead, "num", iNum, "%lu", iNum);
        utPltPutLoopVarF(psDbHead, "sid", iNum, "%lu", lSid);
        utPltPutLoopVar(psDbHead, "softname", iNum, caSoftname);
        utPltPutLoopVar(psDbHead, "filename", iNum, caFilename);
        utPltPutLoopVar(psDbHead, "filesize", iNum, utStrLtoF(lFilesize, caTemp));
        utPltPutLoopVar(psDbHead, "version", iNum, caVersion);
        utPltPutLoopVar(psDbHead, "uploadtime", iNum, utTimFormat("%Y-%m-%d %H:%M:%S", lUploadtime));
        utPltPutLoopVar(psDbHead, "lasttime", iNum, utTimFormat("%Y-%m-%d %H:%M:%S", lLasttime));
        //       utPltPutLoopVar(psDbHead,"level",iNum,ncsGetFirstNameById(lLevel));
        utPltPutLoopVar(psDbHead, "opertor", iNum, caOpertor);
        utPltPutLoopVarF(psDbHead, "status", iNum, "%d", lStatus);
        utPltPutLoopVarF(psDbHead, "flags", iNum, "%d", lFlags);

        if(lStatus == 0)
        {
            utPltPutLoopVar(psDbHead, "status_desc", iNum, ncLang("0020可用"));
        }
        else if(lStatus == 1)
        {
            utPltPutLoopVar(psDbHead, "status_desc", iNum, ncLang("0021不可用"));
        }
        if(lFlags == 0)
        {
            utPltPutLoopVar(psDbHead, "flags_desc", iNum, ncLang("0022对全部适用"));
        }
        else if(lFlags == 1)
        {
            utPltPutLoopVar(psDbHead, "flags_desc", iNum, "XP win7(32)");
        }
        else if(lFlags == 2)
        {
            utPltPutLoopVar(psDbHead, "flags_desc", iNum, ncLang("0023win7、8(64位)"));
        }
        else if(lFlags == 3)
        {
            utPltPutLoopVar(psDbHead, "flags_desc", iNum, "win8(32)");
        }




    }
    pasDbCloseCursor(psCur);


    utPltPutVarF(psDbHead, "TotRec", "%d", lCount);

    //  ncsWriteSysLog(psMsgHead,caUsername2,"06","成功","查看系统用户列表信息");

    if(!utFileIsExist("/home/ncmysql/nw/upload/"))
    {
        system("mkdir -p /home/ncmysql/nw/upload/");
        system("chmod 777 -Rf /home/ncmysql/nw/upload");
    }
    if(!utFileIsExist("/home/ncmysql/nw/upload/soft/"))
    {
        system("mkdir -p /home/ncmysql/nw/upload/soft/");
        system("chmod 777 -Rf /home/ncmysql/nw/upload/soft/");
    }
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_softupload_list.htm");
    return 0;
}

int pmUtlUploadFile(char *pIn, char *pLocal, char *pRemote, char *pType)
{
    char *p, *p0;
    strcpy(pLocal, "\0"); //本地上传文件名字
    strcpy(pRemote, "\0"); //服务器存储位置
    strcpy(pType, "\0");
    p = strstr(pIn, ";");
    if(*p == ';')
    {
        memcpy(pLocal, pIn, p - pIn);

        pLocal[p - pIn] = 0;
        p++;
        p0 = strstr(p, ";");
        if(*p0 == ';')
        {
            memcpy(pRemote, p, p0 - p);
            pRemote[p0 - p] = 0;
            strcpy(pType, p0 + 1);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
    return 0;
}

//保存
int lanSoftUpload_save(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    long iReturn, lCount;
    char caSoftname[32], caStatus[16], caVersion[16], caFlags[16], caFilename[128], caFilesize[16];
    char caMd5[64], caNote[256], caSid[16];
    char caTemp[1024];
    long lSid;



    //    utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead, 9,
                               "softname",            UT_TYPE_STRING, 31, caSoftname,
                               "status",          UT_TYPE_STRING, 8, caStatus,
                               "version",           UT_TYPE_STRING, 15, caVersion,
                               "flags",          UT_TYPE_STRING, 8, caFlags,
                               "filename",           UT_TYPE_STRING, 120, caFilename,
                               "filesize",       UT_TYPE_STRING, 10, caFilesize,
                               "md5", UT_TYPE_STRING, 60, caMd5,
                               "note", UT_TYPE_STRING, 200, caNote,
                               "sid", UT_TYPE_STRING, 10, caSid);
    char msg[160] = "";
    snprintf(msg, sizeof(msg), "上传了升级包[%s]", caFilename);
    writeSysLog(psMsgHead, "11", "Success", msg);

    if(strlen(caSoftname) > 0 && strlen(caFilename) > 0)
    {
        //      printf("aaaaaaaaaaaaaaaaaaaaaaa\n");
        if(strlen(caSid) > 0)
        {
            pasDbExecSqlF("update ncupsoftlist set softname='%s',filename='%s',version='%s',filesize=%d,note='%s',opertor='%s',status=%d,flags=%d,md5='%s',lasttime=%d where sid=%s ",
                          caSoftname, caFilename, caVersion, atol(caFilesize), caNote, "", atol(caStatus), atol(caFlags), caMd5, time(0), caSid);
        }
        else
        {
            sprintf(caTemp, "select count(*) from ncupsoftlist where filename='%s' and softname='%s' ", caFilename, caSoftname);
            lCount = 0;
            //              printf("caTemp=%s\n",caTemp);
            pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);
            if(lCount == 0)
            {

                pasDbExecSqlF("insert into ncupsoftlist(softname,filename,version,filesize,note,opertor,uploadtime,status,flags,lcount,md5,lasttime) \
							   values('%s','%s','%s',%d,'%s','',%d,%d,%d,0,'%s',%d) ",
                              caSoftname, caFilename, caVersion, atol(caFilesize), caNote, time(0), atol(caStatus), atol(caFlags), caMd5, time(0));
                pasDbOneRecord("select max(sid) from ncupsoftlist ", 0, UT_TYPE_LONG, 4, &lSid);
                utMsgDelVar(psMsgHead, "sid");
                sprintf(caSid, "%d", lSid);
                utMsgAdd_s(&psMsgHead, "sid", caSid);

                //               utMsgPrintMsg(psMsgHead);
            }
        }
    }
    else
    {
        //              printf("bbbbbbbbbbbbbbbbb\n");
    }

    lanDispSoftUpload(psShmHead, iFd, psMsgHead);
    return 0;
}

char *strrstr(const char *haystack, const char *needle)
{
    char *r = NULL;

    if(!needle[0])
        return (char*)haystack + strlen(haystack);
    while(1)
    {
        char *p = strstr(haystack, needle);
        if(!p)
            return r;
        r = p;
        haystack = p + 1;
    }
}

/* 保存上传文件 */
int ncupSoftSave(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);
    int  iReturn = 0;
    utPltDbHead *psDbHead = NULL;
    char caFile[256] = "";
    char sid[17] = "";
    char *p;
    char caLocal[128] = "";
    char caRemote[128] = "";
    char caRemote_new[128] = "";
    char caType[128] = "";
    unsigned long ltime = time(0);
    char caDispname[128];
    char caTemp[1024];
    iReturn = utMsgGetSomeNVar(psMsgHead, 1,
                               "filename",   UT_TYPE_STRING, sizeof(caFile) - 1, caFile);


    pmUtlUploadFile(caFile, caLocal, caRemote, caType);

    printf(" caFile==[%s],caLocal=%s,caRemote=%s\n", caFile, caLocal, caRemote);


    char caMyname[32] = "";
    char sql[256] = "";
    dsCltGetMyInfo(1, "USERNAME", caMyname);

    sprintf(sql, "select groupid from dsuser where name = '%s'", caMyname);

    unsigned long groupid = 0;
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(groupid), &groupid);


    /* 取得文件名*/
    psDbHead = utPltInitDbHead();
    p = strrstr(caRemote, "/");
    if(p != NULL)
    {
        strcpy(caDispname, ++p);
        utPltPutVar(psDbHead, "dispname", caDispname);
        //     printf("p=%s\n",caDispname);

    }
    snprintf(caRemote_new, sizeof(caRemote_new), "/home/ncmysql/nw/upload/soft/%s", caDispname);
    sprintf(caTemp, "mv %s %s", caRemote, caRemote_new);

    system(caTemp);

    char desFile[256] = "";

    sprintf(desFile, "/home/ncmysql/nw/html/soft/%s", caDispname);
    if(!strcmp(caDispname, "pronetsetupx86.exe") || !strcmp(caDispname, "pronetsetupwin8_32.exe") || !strcmp(caDispname, "pronetsetup64.exe"))
    {
        sprintf(caTemp, "cp /home/ncmysql/nw/upload/soft/%s /home/ncmysql/nw/html/soft/", caDispname);

        system(caTemp);

        char conf[256] = "";
        snprintf(conf, sizeof(conf), "<centerip>%s</centerip><centerid>%s</centerid><centerudpport>%s</centerudpport><setuptype>%s</setuptype><groupid>%lu</groupid>", utComGetVar_sd(psShmHead, "AdminIp", "PK360817"), utComGetVar_sd(psShmHead, "ProductSN", "PK360817"), utComGetVar_sd(psShmHead, "Udpport", "10004"), 0, groupid);

        FILE * file = fopen(desFile, "a+");

        if(file)
        {
            char buffer1[24];
            memset(buffer1, '1', sizeof(buffer1));
            fwrite(buffer1, 1, sizeof(buffer1), file);
            fwrite(conf, 1, sizeof(conf), file);
            fclose(file);
        }

        if(!strcmp(caDispname, "pronetsetupx86.exe"))
        {
            insertToFile1(desFile, WT_BIND_MACFILELENOLD, conf);
        }
        else if(!strcmp(caDispname, "pronetsetupwin8_32.exe"))
        {
            insertToFile1(desFile, WT_BIND_MACFILELEN32, conf);
        }
        else if(!strcmp(caDispname, "pronetsetup64.exe"))
        {
            insertToFile1(desFile, WT_BIND_MACFILELEN64, conf);
        }
    }

    /* 计算MD5 校验值 */
    char s_comm[512] = "";
    snprintf(s_comm, sizeof(s_comm), "/usr/bin/md5sum %s", caRemote_new);
    char md5buf[33] = "";
    FILE *fp = popen(s_comm, "r");
    if(fp != NULL)
    {
        fread(md5buf, sizeof(char), sizeof(md5buf) - 1, fp);
        md5buf[sizeof(md5buf) - 1] = '\0';
        pclose(fp);
        utPltPutVar(psDbHead,  "md5buf", md5buf);
    }
    /* 计算MD5 校验值 */

    utPltPutVarF(psDbHead, "uptime",  "%lu", ltime);
    utPltPutVarF(psDbHead, "fsize", "%lu", utFileSize(caRemote_new));
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/ncs_pic_addok.html");
    return 0;
}


/* 显示上传软件界面 */
int lanDispSoftUpload(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    int  iReturn = 0;
    utPltDbHead *psDbHead = NULL;
    char caSoftname[32], caFilename[128], caVersion[16], caNote[256], caMd5[36], caSid[20];
    long lFilesize, lStatus, lFlags;
    char caTemp[1024];

    iReturn = utMsgGetSomeNVar(psMsgHead, 1, "sid",   UT_TYPE_STRING, 10, caSid);

    psDbHead = utPltInitDbHead();
    if(strlen(caSid) > 0 && atol(caSid) > 0)
    {
        sprintf(caTemp, "select softname,filename,version,filesize,note,status,flags,md5 from ncupsoftlist where sid=%s ", caSid);
        memset(caSoftname, 0, sizeof(caSoftname));
        memset(caFilename, 0, sizeof(caFilename));
        memset(caVersion, 0, sizeof(caVersion));
        memset(caNote, 0, sizeof(caNote));
        lFilesize = 0;
        lStatus = 0;
        lFlags = 0;
        memset(caMd5, 0, sizeof(caMd5));
        pasDbOneRecord(caTemp, 0, UT_TYPE_STRING, 31, caSoftname,
                       UT_TYPE_STRING, 127, caFilename,
                       UT_TYPE_STRING, 15, caVersion,
                       UT_TYPE_LONG, 4, &lFilesize,
                       UT_TYPE_STRING, 254, caNote,
                       UT_TYPE_LONG, 4, &lStatus,
                       UT_TYPE_LONG, 4, &lFlags,
                       UT_TYPE_STRING, 32, caMd5);
        utPltPutVar(psDbHead, "softname", caSoftname);
        utPltPutVar(psDbHead, "filename", caFilename);
        utPltPutVar(psDbHead, "version", caVersion);
        utPltPutVarF(psDbHead, "filesize", "%d", lFilesize);
        utPltPutVar(psDbHead, "note", caNote);
        utPltPutVarF(psDbHead, "status", "%d", lStatus);
        utPltPutVarF(psDbHead, "flags", "%d", lFlags);
        utPltPutVar(psDbHead, "md5", caMd5);
        utPltPutVar(psDbHead, "sid", caSid);
    }


    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/lant_softupload_first.htm");
    return 0;
}

//显示软件升级下发策略
int lan_softupdate_procy(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    long iReturn, lCount;
    char caStart[16], caTemp[2048], caCurPg[16];
    long lCurPg, lRowNum, lStartRec;
    struct data_s
    {
        long id;                             //软件ID
        char caSoftname[32];
        char caFilename[128];
        long flags;
        long stime;
        char object[4096];
    };
    struct data_s *psData;
    nwCompInfo *psComp;
    char *pHash;
    pasLHashInfo sHashInfo;
    char caSid[16], caSoftname[32], caFilename[128];
    unsigned long lFlags, lSid, lObjid, lObjtype, iNum, lStime;
    char caDisp[32];
    char caKid[16], caSeid[1024];
    char caObj[100][50];
    char caLimit[16], caPage[16], caDel[16], caDir[16], caKeyword[64], caSelsid[1024], caUpdate[16];
    char caApp[32];
    utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead, 12,
                               "limit",         UT_TYPE_STRING, 10, caLimit,
                               "page",          UT_TYPE_STRING, 10, caPage,
                               "del",           UT_TYPE_STRING, 8, caDel,
                               "dir",           UT_TYPE_STRING, 8, caDir,
                               "selsid",        UT_TYPE_STRING, 1000, caSelsid,
                               "kid",           UT_TYPE_STRING, 10, caKid,
                               "seid",          UT_TYPE_STRING, 1000, caSeid,
                               "update",        UT_TYPE_STRING, 10, caUpdate,
                               "sid",           UT_TYPE_STRING, 10, caSid,
                               "start",         UT_TYPE_STRING, 10, caStart,
                               "del",           UT_TYPE_STRING, 10, caDel,
                               "app",           UT_TYPE_STRING, 10, caApp);

    char msg[2048] = "";
    //  printf("caApp=%s\n",caApp);
    if(strcmp(caApp, "app") == 0)
    {
        writeLog("softUpdate.txt", "test", "here");
        iReturn = lanUtlLoadSoftUpdate(psShmHead);
        printf("iReturn=%d\n", iReturn);

        //snprintf(msg,sizeof(msg),"应用下发软件[%s]?噶?,caKid);
        //writeSysLog(psMsgHead,"11","Success",msg);
    }
    if(strcmp(caUpdate, "update") == 0)
    {
        long num, i;
        char caType[16], caId[16];
        num = ncs_SepChar0(caSeid, ',', caObj);
        for(i = 0; i < num; i++)
        {
            //      printf("caObj=%s\n",caObj[i]);
            utStrGetSomeWordBy(caObj[i], "_", 2, UT_TYPE_STRING, 10, caType, UT_TYPE_STRING, 10, caId);
            //            printf("caType=%s,caId=%s\n",caType,caId);
            sprintf(caTemp, "select count(*) from ncupsoftprocy where softid=%s and objid=%s and objtype=%s ", caKid, caId, caType);
            lCount = 0;
            pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);
            if(lCount == 0)
            {
                pasDbExecSqlF("insert into ncupsoftprocy(softid,objid,objtype,lasttime) values(%s,%s,%s,%d) ", caKid, caId, caType, time(0));

                snprintf(msg, sizeof(msg), "添加下发软件[%s]指令", caKid);
                writeSysLog(psMsgHead, "11", "Success", msg);
            }
        }
    }
    if(strcmp(caDel, "del") == 0 && strlen(caSelsid) > 0)
    {
        pasDbExecSqlF("delete from ncupsoftprocy where softid in (%s) ", caSelsid);

        snprintf(msg, sizeof(msg), "删除了下发指令[%s]", caSelsid);
        writeSysLog(psMsgHead, "11", "Success", msg);
    }




    pHash = (unsigned char *)pasLHashInit(500, 500, sizeof(struct data_s), 0, 4);
    if(pHash == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", "分配内存出错", "分配内存出错");
        return 0;
    }

    sprintf(caTemp, "select ncupsoftlist.sid,softname,filename,flags,objid,objtype,ncupsoftprocy.lasttime from ncupsoftlist,ncupsoftprocy where ncupsoftlist.sid=softid and status=0 order by ncupsoftlist.sid desc ");
    psCur = pasDbOpenSql(caTemp, 0);
    if(psCur)
    {
        lSid = 0;
        memset(caSoftname, 0, sizeof(caSoftname));
        memset(caFilename, 0, sizeof(caFilename));
        lFlags = 0;
        lObjid = 0;
        lObjtype = 0;
        lStime = 0;
        iReturn = pasDbFetchInto(psCur,
                                 UT_TYPE_LONG, 4, &lSid,
                                 UT_TYPE_STRING, 31, caSoftname,
                                 UT_TYPE_STRING, 127, caFilename,
                                 UT_TYPE_LONG, 4, &lFlags,
                                 UT_TYPE_ULONG, 4, &lObjid,
                                 UT_TYPE_LONG, 4, &lObjtype,
                                 UT_TYPE_LONG, 4, &lStime);
        while((iReturn == 0) || (iReturn == 1405))
        {
            psData = (struct data_s *)pasLHashLookA(pHash, &lSid);
            memset(caDisp, 0, sizeof(caDisp));
            if(psData)
            {
                strcpy(psData->caSoftname, caSoftname);
                strcpy(psData->caFilename, caFilename);
                psData->flags = lFlags;
                psData->stime = lStime;
                if(lObjtype == 0 && lObjid == 0)
                {
                    strcpy(caDisp, "全部");
                }
                else if(lObjtype == 1 && lObjid > 0)
                {
                    strcpy(caDisp, lanGetGroup(psShmHead, lObjid));
                }
                else if(lObjtype == 2 && lObjid > 0)
                {
                    psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &lObjid);
                    if(psComp)
                    {
                        strcpy(caDisp, psComp->compname);
                    }
                }


                if(strlen(psData->object) == 0)
                {
                    strcpy(psData->object, caDisp);
                }
                else if(strlen(psData->object) < 4000)
                {
                    sprintf(psData->object + strlen(psData->object), ",%s", caDisp);
                }
            }

            lSid = 0;
            memset(caSoftname, 0, sizeof(caSoftname));
            memset(caFilename, 0, sizeof(caFilename));
            lFlags = 0;
            lObjid = 0;
            lObjtype = 0;
            lStime = 0;
            iReturn = pasDbFetchInto(psCur,
                                     UT_TYPE_LONG, 4, &lSid,
                                     UT_TYPE_STRING, 31, caSoftname,
                                     UT_TYPE_STRING, 127, caFilename,
                                     UT_TYPE_LONG, 4, &lFlags,
                                     UT_TYPE_ULONG, 4, &lObjid,
                                     UT_TYPE_LONG, 4, &lObjtype,
                                     UT_TYPE_LONG, 4, &lStime);
        }

        pasDbCloseCursor(psCur);

    }





    lRowNum = atol(caLimit);
    if(lCurPg <= 0) lCurPg = 1;
    lStartRec = atol(caStart);

    psDbHead = utPltInitDb();




    psData = (struct data_s *)pasLHashFirst(pHash, &sHashInfo);
    iNum = 0;
    int i = 0;
    while(psData)
    {
        i++;
        if(i < lStartRec)
        {
            goto nextrecord;
        }
        if(iNum >= lRowNum)
        {
            goto nextrecord;
        }

        iNum++;
        if(iNum > 1)
        {
            utPltPutLoopVar(psDbHead, "dh", iNum, ",");
        }
        utPltPutLoopVar(psDbHead, "softname", iNum, psData->caSoftname);
        utPltPutLoopVar(psDbHead, "filename", iNum, psData->caFilename);
        utPltPutLoopVar(psDbHead, "obj", iNum, psData->object);
        utPltPutLoopVarF(psDbHead, "sid", iNum, "%d", psData->id);
        if(psData->flags == 0)
        {
            utPltPutLoopVar(psDbHead, "os", iNum, ncLang("0011全部"));
        }
        else if(psData->flags == 1)
        {
            utPltPutLoopVar(psDbHead, "os", iNum, ncLang("0134xp,win7 32位"));
        }
        else if(psData->flags == 2)
        {
            utPltPutLoopVar(psDbHead, "os", iNum, ncLang("0023win7、8(64位)"));
        }
        else if(psData->flags == 3)
        {
            utPltPutLoopVar(psDbHead, "os", iNum, ncLang("0135win832位"));
        }

        utPltPutLoopVar(psDbHead, "stime", iNum, utTimFormat("%y-%m-%d %H:%M:%S", psData->stime));


nextrecord:
        psData = (struct data_s *)pasLHashNext(&sHashInfo);
    }






    lCount = i;


    utPltPutVarF(psDbHead, "TotRec", "%d", lCount);
    free(pHash);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/lan_softdown_xf_list.htm");
    return 0;
}

//显示软件升级请求
int Lan_UpdateSoftReq(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    recodeLogInfo(" 客户端更新：", "收到客户端更新请求！", "");

    char caCompid[16], caUpid[16];
    char caTemp[1024];
    char caFilename[128], caMd5[36];
    char caFullname[256];
    unsigned long lCompid;
    utMsgGetSomeNVar(psMsgHead, 2, "compid", UT_TYPE_ULONG, 4, &lCompid,
                     "upid",   UT_TYPE_STRING, 10, caUpid);


    char  str[512];
    memset(str, 0, sizeof(str));
    snprintf(str,  sizeof(str) - 1,   "compid=%u, upid=%d", lCompid, caUpid);
    recodeLogInfo(" 客户端更新：", "提取客户端更新请求信息成功！", "");

    if(strlen(caUpid) > 0)
    {
        sprintf(caTemp, "select filename,md5 from ncupsoftlist where sid=%s ", caUpid);
        memset(caFilename, 0, sizeof(caFilename));
        memset(caMd5, 0, sizeof(caMd5));
        pasDbOneRecord(caTemp, 0, UT_TYPE_STRING, 127, caFilename,
                       UT_TYPE_STRING, 32, caMd5);
        if(strlen(caFilename) > 0 && strlen(caMd5) > 0)
        {
            sprintf(caFullname, "/home/ncmysql/nw/upload/soft/%s", caFilename);
            long iSize = 0;
            iSize = utFileSize(caFullname);
            pasTcpResponse(iFd, psMsgHead, NULL, 3,
                           "filename",  UT_TYPE_STRING, caFilename,
                           "filelenth", UT_TYPE_ULONG, iSize,
                           "Md5",       UT_TYPE_STRING, caMd5);
            char  mesg[512];
            memset(mesg, 0, sizeof(mesg));
            snprintf(mesg, sizeof(mesg) - 1, "filename = %s, filelength=%d,filelen=%u Md5=%s", caFilename,  iSize, iSize,  caMd5);
            recodeLogInfo("客户端更新：", mesg, "");

        }
        else
        {
            pasTcpResponse(iFd, psMsgHead, NULL, 2,
                           "filename",  UT_TYPE_STRING, "",
                           "filelenth", UT_TYPE_ULONG, 0);
        }

    }


    return 0;
}

//软件升级
int Lan_UpdateSoftBegin(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    recodeLogInfo(" 客户端更新：", "收到客户端更新请求！", "");

    char caCompid[16], caUpid[16];
    char caTemp[1024];
    char caFilename[128], caMd5[36];
    char caFullname[256];
    long iSize;
    char *pBuf;
    long iWrite;
    int iBytes;
    unsigned long lBytes;
    unsigned long lCompid;
    FILE *fp;
    utMsgGetSomeNVar(psMsgHead, 2, "compid", UT_TYPE_ULONG, 4, &lCompid,
                     "upid",   UT_TYPE_STRING, 10, caUpid);



    if(strlen(caUpid) > 0)
    {
        sprintf(caTemp, "select filename,md5 from ncupsoftlist where sid=%s ", caUpid);
        memset(caFilename, 0, sizeof(caFilename));
        memset(caMd5, 0, sizeof(caMd5));
        pasDbOneRecord(caTemp, 0, UT_TYPE_STRING, 127, caFilename,
                       UT_TYPE_STRING, 32, caMd5);
        if(strlen(caFilename) > 0 && strlen(caMd5) > 0)
        {
            sprintf(caFullname, "/home/ncmysql/nw/upload/soft/%s", caFilename);
            iSize = utFileSize(caFullname);
            if(iSize <= 0)
            {
                printf("file not exist\n");
                return (-1);
            }

            pBuf = malloc(iSize + 1);
            if(pBuf == NULL)
            {
                printf(" Memory Alloc Error \n");
                return (-1);
            }
            memset(pBuf, 0, iSize + 1);
            fp = fopen(caFullname, "rb");

            iBytes = fread(pBuf, 1, iSize, fp);
            if(iBytes > 0)
            {
                int len = 0;
                int pos = 0;
                int i = 0;
                int blocknums = 10;
                for(i = 0; i < blocknums; i++)
                {
                    pos = (iBytes / blocknums) * i;
                    len = iBytes / blocknums;
                    if(i == (blocknums - 1))
                    {
                        len = iBytes - (iBytes / blocknums) * (blocknums - 1);
                    }
                    iWrite = write(iFd, pBuf + pos, len);
                }
                //iWrite = write(iFd,pBuf,iBytes);

                sprintf(caTemp, "iBytes=%lu,iWrite=%lu\n", iBytes, iWrite);
                //sleep(2);
                writelog(caTemp);
                if(iWrite != iBytes)
                {
                    fclose(fp);
                    free(pBuf);
                    return 0;
                }

            }

            fclose(fp);
            free(pBuf);
            return 0;

        }
    }
    return 0;
}


//报告软件审计完毕状态
int Lan_UpdateSoftReport(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    char caCompid[16], caUpid[16], caStatus[16];
    char caTemp[1024];
    unsigned long lCompid;
    nwCompInfo *psComp;
    utMsgGetSomeNVar(psMsgHead, 3, "compid", UT_TYPE_ULONG, 4, &lCompid,
                     "upid",   UT_TYPE_STRING, 10, caUpid,
                     "status", UT_TYPE_STRING, 10, caStatus);

    //   printf("lCompid=%lu\n",lCompid);
    char  str[512];

    if(strlen(caUpid) > 0)
    {
        pasDbExecSqlF("insert into ncupsoftlog (softid,compid,stime,status) values (%s,%lu,%d,%d)", caUpid, lCompid, time(0), atol(caStatus));
        if(atol(caStatus) == 1)
        {
            //修改客户端状态
            //       lCompid=strtoul(caCompid,NULL,10);
            psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &lCompid);
            if(psComp)
            {
                psComp->stime = atol(caUpid);

                pasDbExecSqlF("update nwcompute set stime=%s where compid=%lu ", caUpid, lCompid);
            }

            pasTcpResponse(iFd, psMsgHead, NULL, 1, "status",  UT_TYPE_STRING, "1");

        }
        else
        {

            pasTcpResponse(iFd, psMsgHead, NULL, 1, "status",  UT_TYPE_STRING, "2");
        }
    }

    return 0;
}
//系统刷新
int ncsSysReflesh_v4(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    int iReturn;

    //    utMsgPrintMsg(psMsgHead);
    psDbHead = utPltInitDb();
    //  iReturn=ncsResetSystem(psShmHead);
    lanUtlLoadProcy(psShmHead);
    if(iReturn >= 0)
    {
        utPltPutVar(psDbHead, "mesg", "true");
    }
    else
    {
        utPltPutVar(psDbHead, "mesg", "false");
    }
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_msg.htm");
    return 0;
}


int Lan_GetBaseProcyFileReq(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    char caCompid[16], caUpid[16];
    char caTemp[1024];
    char caFilename[128], caMd5[36];
    char caFullname_p[256], caFullname_t[256], caFullname_b[256], caFullname_u[256], caFullname_s[256];
    char s_comm[512] = "";
    char md5buf[33] = "";
    FILE *fp;
    unsigned long lCompid;
    unsigned long lFilelen_pt, lFilelen_time, lFilelen_ut, lFilelen_bt, lFilelen_st;
    char caMd5_pt[36], caMd5_ut[36], caMd5_time[36], caMd5_bt[36], caMd5_st[36];
    int iNum;
    utMsgGetSomeNVar(psMsgHead, 1, "compid", UT_TYPE_ULONG, 4, &lCompid);

    unsigned long shortid = findCompanyByCompid(lCompid);
    char sql[128] = "";
    char shortname[32] = "";
    sprintf(sql, "select name from dsuser where groupid = %lu and name like '%%admin%%'", shortid);

    pasDbOneRecord(sql, 0, UT_TYPE_STRING, sizeof(shortname), shortname);

    shortname[strlen(shortname) - 5] = 0;

    if(strlen(shortname) == 0)
    {
        strcpy(shortname, "0");
    }
    //printf("策略基础文件下发 公司简称：%s %s\n",sql,shortname);

    iNum = 0;
    // 计算Program_type.txt的MD5值
    sprintf(caFullname_p, "/home/ncmysql/nw/baseprocy/Program_type.txt");
    writeLog("baseData.txt", "soft", caFullname_p);
    if(!utFileIsExist(caFullname_p))
    {
        lFilelen_pt = 0;
        strcpy(caMd5_pt, "");
    }
    else
    {
        lFilelen_pt = utFileSize(caFullname_p);
        /* 计算MD5 校验值 */
        strcpy(caMd5_pt, getFilemd5(caFullname_p));
    }

    // 计算blacklist_type.txt的MD5值
    // sprintf(caFullname_b, "/home/ncmysql/nw/baseprocy/blacklist_type.txt");
    sprintf(caFullname_b, "/home/ncmysql/nw/baseprocy/%s/blacklist_type.txt", shortname);
    if(!utFileIsExist(caFullname_b))
    {
        lFilelen_bt = 0;
        strcpy(caMd5_bt, "");
    }
    else
    {
        lFilelen_bt = utFileSize(caFullname_b);
        strcpy(caMd5_bt, getFilemd5(caFullname_b));
    }


    // 计算u_type.txt的MD5值
    sprintf(caFullname_u, "/home/ncmysql/nw/baseprocy/%s/u_type.txt", shortname);
    if(!utFileIsExist(caFullname_u))
    {
        lFilelen_ut = 0;
        strcpy(caMd5_ut, "");
    }
    else
    {
        lFilelen_ut = utFileSize(caFullname_u);
        strcpy(caMd5_ut, getFilemd5(caFullname_u));
    }

    // 计算time.ini的MD5值
    sprintf(caFullname_t, "/home/ncmysql/nw/baseprocy/%s/time.ini", shortname);
    if(!utFileIsExist(caFullname_t))
    {
        lFilelen_time = 0;
        strcpy(caMd5_time, "");
    }
    else
    {
        lFilelen_time = utFileSize(caFullname_t);
        strcpy(caMd5_time, getFilemd5(caFullname_t));
    }

    // 计算procscreen.ini的MD5值
    sprintf(caFullname_s, "/home/ncmysql/nw/baseprocy/%s/procscreen.ini", shortname);
    if(!utFileIsExist(caFullname_s))
    {
        lFilelen_st = 0;
        strcpy(caMd5_st, "");
    }
    else
    {
        lFilelen_st = utFileSize(caFullname_s);
        strcpy(caMd5_st, getFilemd5(caFullname_s));
    }
    //Basetypeinfo.txt
    char caFullname_base[128];
    char caMd5_base[36];
    unsigned long lFilelen_base = 0;
    sprintf(caFullname_base, "/home/ncmysql/nw/baseprocy/%s/Basetypeinfo.txt", shortname);
    if(!utFileIsExist(caFullname_base))
    {
        lFilelen_base = 0;
        strcpy(caMd5_base, "");
    }
    else
    {
        lFilelen_base = utFileSize(caFullname_base);
        strcpy(caMd5_base, getFilemd5(caFullname_base));
    }


    printf("caMd5_base:%s\n", caMd5_base);
    pasTcpResponse(iFd, psMsgHead, NULL, 25,
                   "sum",  UT_TYPE_LONG, 6,
                   "Pathname_1", UT_TYPE_STRING, caFullname_p,
                   "Filename_1", UT_TYPE_STRING, "Program_type.txt",
                   "Filelenth_1", UT_TYPE_ULONG, lFilelen_pt,
                   "MD5_1", UT_TYPE_STRING, caMd5_pt,
                   "Pathname_2", UT_TYPE_STRING, caFullname_u,
                   "Filename_2", UT_TYPE_STRING, "u_type.txt",
                   "Filelenth_2", UT_TYPE_ULONG, lFilelen_ut,
                   "MD5_2", UT_TYPE_STRING, caMd5_ut,
                   "Pathname_3", UT_TYPE_STRING, caFullname_t,
                   "Filename_3", UT_TYPE_STRING, "time.ini",
                   "Filelenth_3", UT_TYPE_ULONG, lFilelen_time,
                   "MD5_3", UT_TYPE_STRING, caMd5_time,
                   "Pathname_4", UT_TYPE_STRING, caFullname_b,
                   "Filename_4", UT_TYPE_STRING, "blacklist_type.txt",
                   "Filelenth_4", UT_TYPE_ULONG, lFilelen_bt,
                   "MD5_4", UT_TYPE_STRING, caMd5_bt,
                   "Pathname_5", UT_TYPE_STRING, caFullname_s,
                   "Filename_5", UT_TYPE_STRING, "procscreen.ini",
                   "Filelenth_5", UT_TYPE_ULONG, lFilelen_st,
                   "MD5_5", UT_TYPE_STRING, caMd5_st,
                   "Pathname_6", UT_TYPE_STRING, caFullname_base,
                   "Filename_6", UT_TYPE_STRING, "Basetypeinfo.txt",
                   "Filelenth_6", UT_TYPE_ULONG, lFilelen_base,
                   "MD5_6", UT_TYPE_STRING, caMd5_base);
    return 0;
}



int Lan_FileDownLoad(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    char caCompid[16], caUpid[16];
    char caTemp[1024];
    char caFilename[128], caMd5[36];
    char caFullname[256];
    long iSize;
    char *pBuf;
    long iWrite;
    int iBytes;
    unsigned long lBytes;
    unsigned long lCompid;
    nwCompInfo        *psComp;
    FILE *fp;
    utMsgGetSomeNVar(psMsgHead, 2, "compid", UT_TYPE_ULONG, 4, &lCompid,
                     "filename",   UT_TYPE_STRING, 127, caFilename);

    psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &lCompid);
    char caCip[32];
    memset(caCip, 0, sizeof(caCip));
    if(psComp)
    {
        strcpy(caCip, utComHostIp(htonl(psComp->ip)));
    }

    pasLogs(PAS_SRCFILE, 1000, "下载策略文件 ip:%s, 文件名:%s", caCip, caFilename);
    char mesg[1024] = "";
    snprintf(mesg, sizeof(mesg) - 1, "下载策略文件 ip:%s, 文件名:%s", caCip, caFilename);
    recodeLogInfo("Receive the download strategy file request...", "@ @ @ @ @ @ @ @ @ @ @ @ @", mesg);
    if(strlen(caFilename) > 0)
    {

        iSize = utFileSize(caFilename);
        if(iSize <= 0)
        {
            //printf("file not exist\n");
            return (-1);
        }

        pBuf = malloc(iSize + 1);
        if(pBuf == NULL)
        {
            printf(" Memory Alloc Error \n");
            return (-1);
        }
        memset(pBuf, 0, iSize + 1);
        fp = fopen(caFilename, "rb");

        iBytes = fread(pBuf, 1, iSize, fp);
        if(iBytes > 0)
        {

            iWrite = write(iFd, pBuf, iBytes);
            if(iWrite != iBytes)
            {
                fclose(fp);
                free(pBuf);
                return 0;
            }

        }

        fclose(fp);
        free(pBuf);
        return 0;

    }

    return 0;
}

//获取策略文件列表
int Lan_GetProcyFileReq(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    nwCompInfo      *psComp;
    nwCompIp            *psCompIp;

    char        mesg[1024] = "";
    char    caCompid[16],
            caUpid[16];
    char    caTemp[1024];
    char    caFilename[128],
            caMd5[36];
    char    caFullname[256];
    char    s_comm[512] = "";
    char    md5buf[33] = "";
    FILE    *fp;
    unsigned long lCompid;
    unsigned long lFilelen_pt, lFilelen_time, lFilelen_ut, lFilelen_bt;
    char caMd5_pt[36], caMd5_ut[36], caMd5_time[36], caMd5_bt[36];
    char caFile_blist[128], caPath_blist[128];
    int iNum, num;
    char caObj[100][50];
    char caObj_date[100][50];


    char caPids[1024], caDataids[1024], caMd5_main[36];
    unsigned long i, m, iRecord, lPid, lFilelen_main;
    char caPath[128], caPathfile[256];

    char caFile_file[128], caPath_file[128], caMd5_file[36];
    unsigned long lFilelen_file = 0;
    char caFile_dev[128], caPath_dev[128], caMd5_dev[36];
    unsigned  long lFilelen_dev = 0;

    char caFile_link[128], caPath_link[128], caMd5_link[36];
    unsigned long lFilelen_link = 0;

    char caFile_url[128], caPath_url[128], caMd5_url[36];
    unsigned long lFilelen_url = 0;
    char caFile_oth[128], caPath_oth[128], caMd5_oth[36];
    unsigned  long lFilelen_oth = 0;
    char caFile_net[128], caPath_net[128], caMd5_net[36];
    unsigned long lFilelen_net = 0;
    char caFile_pname[128], caPath_pname[128], caMd5_pname[36];
    unsigned long lFilelen_pname = 0;
    char caFile_ptype[128], caPath_ptype[128], caMd5_ptype[36];
    unsigned long lFilelen_ptype = 0;
    char caFile_log[128], caPath_log[128], caMd5_log[36];
    unsigned long lFilelen_log;
    //  lanUtlPrintProcy(psShmHead);

    //  lanBlackList *pData3;

    memset(caFile_file, 0, sizeof(caFile_file));
    memset(caPath_file, 0, sizeof(caPath_file));

    memset(caPath, 0, sizeof(caPath));
    memset(caPathfile, 0, sizeof(caPathfile));
    memset(caMd5_main, 0, sizeof(caMd5_main));
    memset(caMd5_file, 0, sizeof(caMd5_file));
    memset(caMd5_dev, 0, sizeof(caMd5_dev));

    lFilelen_file = 0;
    lFilelen_main = 0;
    lFilelen_dev = 0;
    lFilelen_link = 0;
    lFilelen_log = 0;

    utMsgPrintMsg(psMsgHead);
    utMsgGetSomeNVar(psMsgHead, 1, "compid", UT_TYPE_ULONG, 4, &lCompid);
    iNum = 0;

    snprintf(mesg, sizeof(mesg) - 1, "lCompid=%lu", lCompid);
    //recodeLogInfo("接收获取下载策略清单请求...","@@@@@@@@@@@@@",mesg);
    recodeLogInfo(" receives strategy download listing request... ", "@ @ @ @ @ @ @ @ @ @ @ @ @", mesg);
    pasLogs(PAS_SRCFILE, 1000, "reqprocy=%lu", lCompid);


    psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &lCompid);
    char caCip[32];
    if(psComp)
    {
        strcpy(caCip, utComHostIp(htonl(psComp->ip)));
    }
    unsigned long compGroupid = findCompanyByCompid(lCompid);
    int iret = -1;
    char sql[1024] = "";

    pasDbCursor *psCur = NULL;


    char shortname[32] = "";
    sprintf(sql, "select name from dsuser where groupid = %lu and name like '%%admin%%'", compGroupid);
    pasDbOneRecord(sql, 0, UT_TYPE_STRING, sizeof(shortname), shortname);

    shortname[strlen(shortname) - 5] = 0;

    if(strlen(shortname) == 0)
    {
        strcpy(shortname, "0");
    }




    //   pasLogs(PAS_SRCFILE,1000," LastCleanTime:%s   ThisCleanTime:%s ",caSdate,caSdate1);
    //获取外联策略
    strcpy(caFile_link, "Outconnect_law.ini");
    strcpy(caPath_link, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_link, shortname));
    if(!utFileIsExist(caPath_link))
    {
        lFilelen_link = 0;
        strcpy(caMd5_link, "");
    }
    else
    {
        lFilelen_link = utFileSize(caPath_link);
        strcpy(caMd5_link, getFilemd5(caPath_link));
    }
    //获取网址策略文件
    strcpy(caFile_url, "webfilter_law.ini");
    strcpy(caPath_url, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_url, shortname));
    if(!utFileIsExist(caPath_url))
    {
        lFilelen_url = 0;
        strcpy(caMd5_url, "");
    }
    else
    {
        lFilelen_url = utFileSize(caPath_url);
        strcpy(caMd5_url, getFilemd5(caPath_url));
    }

    //获取网络策略文件
    strcpy(caFile_net, "Port_law.ini");
    strcpy(caPath_net, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_net, shortname));
    if(!utFileIsExist(caPath_net))
    {
        lFilelen_net = 0;
        strcpy(caMd5_net, "");
    }
    else
    {
        lFilelen_net = utFileSize(caPath_net);
        strcpy(caMd5_net, getFilemd5(caPath_net));
    }

    //获取应用程序策略文件
    strcpy(caFile_ptype, "ProcessType_law.ini");
    strcpy(caPath_ptype, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_ptype, shortname));
    if(!utFileIsExist(caPath_ptype))
    {
        lFilelen_ptype = 0;
        strcpy(caMd5_ptype, "");
    }
    else
    {
        lFilelen_ptype = utFileSize(caPath_ptype);
        strcpy(caMd5_ptype, getFilemd5(caPath_ptype));
    }

    //获取外设策略文件
    strcpy(caFile_dev, "usb_law.ini");
    strcpy(caPath_dev, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_dev, shortname));
    if(!utFileIsExist(caPath_dev))
    {
        lFilelen_dev = 0;
        strcpy(caMd5_dev, "");
    }
    else
    {
        lFilelen_dev = utFileSize(caPath_dev);
        strcpy(caMd5_dev, getFilemd5(caPath_dev));
    }

    //获取文件策略文件
    strcpy(caFile_file, "File_law.ini");
    strcpy(caPath_file, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_file, shortname));
    if(!utFileIsExist(caPath_file))
    {
        lFilelen_file = 0;
        strcpy(caMd5_file, "");
    }
    else
    {
        lFilelen_file = utFileSize(caPath_file);
        strcpy(caMd5_file, getFilemd5(caPath_file));
    }



    //获取黑白URL策略文件
    strcpy(caFile_blist, "blacklist.ini");
    strcpy(caPath_blist, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_blist, shortname));
    if(!utFileIsExist(caPath_blist))
    {
        lFilelen_bt = 0;
        strcpy(caMd5_bt, "");
    }
    else
    {
        lFilelen_bt = utFileSize(caPath_blist);
        strcpy(caMd5_bt, getFilemd5(caPath_blist));
    }

    //获取日志传输策略文件
    strcpy(caFile_log, "log_law.ini");
    strcpy(caPath_log, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_log, shortname));
    if(!utFileIsExist(caPath_log))
    {
        lFilelen_log = 0;
        strcpy(caMd5_log, "");
    }
    else
    {
        lFilelen_log = utFileSize(caPath_log);
        strcpy(caMd5_log, getFilemd5(caPath_log));
    }



    //获取外发文件优先策略
    char caFile_outadvance[128], caPath_outadvance[128], caMd5_outadvance[36];
    unsigned long lFilelen_outadvance = 0;
    strcpy(caFile_outadvance, "gloaboutfile.ini");
    strcpy(caPath_outadvance, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_outadvance, shortname));
    if(!utFileIsExist(caPath_outadvance))
    {
        lFilelen_outadvance = 0;
        strcpy(caMd5_outadvance, "");
    }
    else
    {
        lFilelen_outadvance = utFileSize(caPath_outadvance);
        strcpy(caMd5_outadvance, getFilemd5(caPath_outadvance));
    }
    //获取SMTP外发文件策略
    char caFile_outsmtp[128], caPath_outsmtp[128], caMd5_outsmtp[36];
    unsigned long lFilelen_outsmtp = 0;
    strcpy(caFile_outsmtp, "smtpoutfile.ini");
    strcpy(caPath_outsmtp, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_outsmtp, shortname));
    if(!utFileIsExist(caPath_outsmtp))
    {
        lFilelen_outsmtp = 0;
        strcpy(caMd5_outsmtp, "");
    }
    else
    {
        lFilelen_outsmtp = utFileSize(caPath_outsmtp);
        strcpy(caMd5_outsmtp, getFilemd5(caPath_outsmtp));
    }

    //获取ftp外发文件策略
    char caFile_outftp[128], caPath_outftp[128], caMd5_outftp[36];
    unsigned long lFilelen_outftp = 0;
    strcpy(caFile_outftp, "ftpoutfile.ini");
    strcpy(caPath_outftp, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_outftp, shortname));
    if(!utFileIsExist(caPath_outftp))
    {
        lFilelen_outftp = 0;
        strcpy(caMd5_outftp, "");
    }
    else
    {
        lFilelen_outftp = utFileSize(caPath_outftp);
        strcpy(caMd5_outftp, getFilemd5(caPath_outftp));
    }

    //获取Http外发文件策略
    char caFile_outhttp[128], caPath_outhttp[128], caMd5_outhttp[36];
    unsigned long lFilelen_outhttp = 0;
    strcpy(caFile_outhttp, "httpoutfile.ini");
    strcpy(caPath_outhttp, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_outhttp, shortname));
    if(!utFileIsExist(caPath_outhttp))
    {
        lFilelen_outhttp = 0;
        strcpy(caMd5_outhttp, "");
    }
    else
    {
        lFilelen_outhttp = utFileSize(caPath_outhttp);
        strcpy(caMd5_outhttp, getFilemd5(caPath_outhttp));
    }

    //获取chat外发文件策略
    char caFile_outchat[128], caPath_outchat[128], caMd5_outchat[36];
    unsigned long lFilelen_outchat = 0;
    strcpy(caFile_outchat, "chatoutfile.ini");
    strcpy(caPath_outchat, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_outchat, shortname));
    if(!utFileIsExist(caPath_outchat))
    {
        lFilelen_outchat = 0;
        strcpy(caMd5_outchat, "");
    }
    else
    {
        lFilelen_outchat = utFileSize(caPath_outchat);
        strcpy(caMd5_outchat, getFilemd5(caPath_outchat));
    }

    //获取云盘外发文件策略
    char caFile_outyundisk[128], caPath_outyundisk[128], caMd5_outyundisk[36];
    unsigned long lFilelen_outyundisk = 0;
    strcpy(caFile_outyundisk, "yunoutfile.ini");
    strcpy(caPath_outyundisk, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_outyundisk, shortname));
    if(!utFileIsExist(caPath_outyundisk))
    {
        lFilelen_outyundisk = 0;
        strcpy(caMd5_outyundisk, "");
    }
    else
    {
        lFilelen_outyundisk = utFileSize(caPath_outyundisk);
        strcpy(caMd5_outyundisk, getFilemd5(caPath_outyundisk));
    }

    //获取U盘外发文件策略
    char caFile_udisk[128], caPath_udisk[128], caMd5_udisk[36];
    unsigned long lFilelen_udisk = 0;
    strcpy(caFile_udisk, "udiskoutfile.ini");
    strcpy(caPath_udisk, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_udisk, shortname));
    if(!utFileIsExist(caPath_udisk))
    {
        lFilelen_udisk = 0;
        strcpy(caMd5_udisk, "");
    }
    else
    {
        lFilelen_udisk = utFileSize(caPath_udisk);
        strcpy(caMd5_udisk, getFilemd5(caPath_udisk));
    }

    //获取共享外发文件策略
    char caFile_share[128], caPath_share[128], caMd5_share[36];
    unsigned long lFilelen_share = 0;
    strcpy(caFile_share, "shareoutfile.ini");
    strcpy(caPath_share, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_share, shortname));
    if(!utFileIsExist(caPath_share))
    {
        lFilelen_share = 0;
        strcpy(caMd5_share, "");
    }
    else
    {
        lFilelen_share = utFileSize(caPath_share);
        strcpy(caMd5_share, getFilemd5(caPath_share));
    }

    //获取其它进程外发文件策略
    char caFile_outother[128], caPath_outother[128], caMd5_outother[36];
    unsigned long lFilelen_outother = 0;
    strcpy(caFile_outother, "otheroutfile.ini");
    strcpy(caPath_outother, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_outother, shortname));
    if(!utFileIsExist(caPath_outother))
    {
        lFilelen_outother = 0;
        strcpy(caMd5_outother, "");
    }
    else
    {
        lFilelen_outother = utFileSize(caPath_outother);
        strcpy(caMd5_outother, getFilemd5(caPath_outother));
    }

    //获取HTTPS外发文件策略
    char caFile_outhttps[128], caPath_outhttps[128], caMd5_outhttps[36];
    unsigned long lFilelen_outhttps = 0;
    strcpy(caFile_outhttps, "httpsoutfile.ini");
    strcpy(caPath_outhttps, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_outhttps, shortname));
    if(!utFileIsExist(caPath_outhttps))
    {
        lFilelen_outhttps = 0;
        strcpy(caMd5_outhttps, "");
    }
    else
    {
        lFilelen_outhttps = utFileSize(caPath_outhttps);
        strcpy(caMd5_outhttps, getFilemd5(caPath_outhttps));
    }

    //获取放行和禁止外发文件策略
    char caFile_outfreeblock[128], caPath_outfreeblock[128], caMd5_outfreeblock[36];
    unsigned long lFilelen_outfreeblock = 0;
    strcpy(caFile_outfreeblock, "blackfree.ini");
    strcpy(caPath_outfreeblock, lanGetProcyFileByCompid(psShmHead, lCompid, caFile_outfreeblock, shortname));
    if(!utFileIsExist(caPath_outfreeblock))
    {
        lFilelen_outfreeblock = 0;
        strcpy(caMd5_outfreeblock, "");
    }
    else
    {
        lFilelen_outfreeblock = utFileSize(caPath_outfreeblock);
        strcpy(caMd5_outfreeblock, getFilemd5(caPath_outfreeblock));
    }
    pasLogs(PAS_SRCFILE, 1000, "ip:%s,filelink:%s  md5link:%s, fileurl:%s md5url:%s,filenet:%s  md5net:%s,fileptype:%s  md5ptype:%s,\
  filedev:%s md5dev:%s,filefile:%s  md5file:%s,fileblist:%s  md5blist:%s,filelog:%s md5log:%s,fileoadvance:%s md5oadvance:%s,\
  filesmtp:%s md5smtp:%s,fileftp:%s md5ftp:%s,filehttp:%s md5http:%s,filechat:%s md5chat:%s,fileyundisk:%s md5yundisk:%s,\
  fileudisk:%s md5udisk:%s,fileshare:%s md5share:%s,fileoth:%s  md5fileoth:%s,filehttps:%s md5filehttps:%s,filefreeblock:%s md5freeblock:%s", \
            caCip, caPath_link, caMd5_link, caPath_url, caMd5_url, caPath_net, caMd5_net, caPath_ptype, caMd5_ptype, caPath_dev, caMd5_dev, caPath_file, caMd5_file, \
            caPath_blist, caMd5_bt, caPath_log, caMd5_log, caPath_outadvance, caMd5_outadvance, caPath_outsmtp, caMd5_outsmtp, caPath_outftp, caMd5_outftp, \
            caPath_outhttp, caMd5_outhttp, caPath_outchat, caMd5_outchat, caPath_outyundisk, caMd5_outyundisk, caPath_udisk, caMd5_udisk, caPath_share, caMd5_share, \
            caPath_outother, caMd5_outother, caPath_outhttps, caMd5_outhttps, caPath_outfreeblock, caMd5_outfreeblock);

    pasTcpResponse(iFd, psMsgHead, NULL, 77,
                   "sum",  UT_TYPE_LONG, 19,
                   "Filename_1", UT_TYPE_STRING, caFile_link,
                   "Pathname_1", UT_TYPE_STRING, caPath_link,
                   "Filelenth_1", UT_TYPE_ULONG, lFilelen_link,
                   "MD5_1",       UT_TYPE_STRING, caMd5_link,
                   "Filename_2", UT_TYPE_STRING, caFile_url,
                   "Pathname_2", UT_TYPE_STRING, caPath_url,
                   "Filelenth_2", UT_TYPE_ULONG, lFilelen_url,
                   "MD5_2",      UT_TYPE_STRING, caMd5_url,
                   "Filename_3", UT_TYPE_STRING, caFile_net,
                   "Pathname_3", UT_TYPE_STRING, caPath_net,
                   "Filelenth_3", UT_TYPE_ULONG, lFilelen_net,
                   "MD5_3",       UT_TYPE_STRING, caMd5_net,
                   "Filename_4", UT_TYPE_STRING, caFile_ptype,
                   "Pathname_4", UT_TYPE_STRING, caPath_ptype,
                   "Filelenth_4", UT_TYPE_ULONG, lFilelen_ptype,
                   "MD5_4",       UT_TYPE_STRING, caMd5_ptype,
                   "Filename_5", UT_TYPE_STRING, caFile_dev,
                   "Pathname_5", UT_TYPE_STRING, caPath_dev,
                   "Filelenth_5", UT_TYPE_ULONG, lFilelen_dev,
                   "MD5_5",       UT_TYPE_STRING, caMd5_dev,
                   "Filename_6", UT_TYPE_STRING, caFile_file,
                   "Pathname_6", UT_TYPE_STRING, caPath_file,
                   "Filelenth_6", UT_TYPE_ULONG, lFilelen_file,
                   "MD5_6",       UT_TYPE_STRING, caMd5_file,
                   "Filename_7", UT_TYPE_STRING, caFile_blist,
                   "Pathname_7", UT_TYPE_STRING, caPath_blist,
                   "Filelenth_7", UT_TYPE_ULONG, lFilelen_bt,
                   "MD5_7",       UT_TYPE_STRING, caMd5_bt,
                   "Filename_8", UT_TYPE_STRING, caFile_log,
                   "Pathname_8", UT_TYPE_STRING, caPath_log,
                   "Filelenth_8", UT_TYPE_ULONG, lFilelen_log,
                   "MD5_8",       UT_TYPE_STRING, caMd5_log,
                   "Filename_9", UT_TYPE_STRING, caFile_outadvance,
                   "Pathname_9", UT_TYPE_STRING, caPath_outadvance,
                   "Filelenth_9", UT_TYPE_ULONG, lFilelen_outadvance,
                   "MD5_9",       UT_TYPE_STRING, caMd5_outadvance,
                   "Filename_10", UT_TYPE_STRING, caFile_outsmtp,
                   "Pathname_10", UT_TYPE_STRING, caPath_outsmtp,
                   "Filelenth_10", UT_TYPE_ULONG, lFilelen_outsmtp,
                   "MD5_10",       UT_TYPE_STRING, caMd5_outsmtp,
                   "Filename_11", UT_TYPE_STRING, caFile_outftp,
                   "Pathname_11", UT_TYPE_STRING, caPath_outftp,
                   "Filelenth_11", UT_TYPE_ULONG, lFilelen_outftp,
                   "MD5_11",       UT_TYPE_STRING, caMd5_outftp,
                   "Filename_12", UT_TYPE_STRING, caFile_outhttp,
                   "Pathname_12", UT_TYPE_STRING, caPath_outhttp,
                   "Filelenth_12", UT_TYPE_ULONG, lFilelen_outhttp,
                   "MD5_12",       UT_TYPE_STRING, caMd5_outhttp,
                   "Filename_13", UT_TYPE_STRING, caFile_outchat,
                   "Pathname_13", UT_TYPE_STRING, caPath_outchat,
                   "Filelenth_13", UT_TYPE_ULONG, lFilelen_outchat,
                   "MD5_13",       UT_TYPE_STRING, caMd5_outchat,
                   "Filename_14", UT_TYPE_STRING, caFile_outyundisk,
                   "Pathname_14", UT_TYPE_STRING, caPath_outyundisk,
                   "Filelenth_14", UT_TYPE_ULONG, lFilelen_outyundisk,
                   "MD5_14",       UT_TYPE_STRING, caMd5_outyundisk,
                   "Filename_15", UT_TYPE_STRING, caFile_udisk,
                   "Pathname_15", UT_TYPE_STRING, caPath_udisk,
                   "Filelenth_15", UT_TYPE_ULONG, lFilelen_udisk,
                   "MD5_15",       UT_TYPE_STRING, caMd5_udisk,
                   "Filename_16", UT_TYPE_STRING, caFile_share,
                   "Pathname_16", UT_TYPE_STRING, caPath_share,
                   "Filelenth_16", UT_TYPE_ULONG, lFilelen_share,
                   "MD5_16",       UT_TYPE_STRING, caMd5_share,
                   "Filename_17", UT_TYPE_STRING, caFile_outother,
                   "Pathname_17", UT_TYPE_STRING, caPath_outother,
                   "Filelenth_17", UT_TYPE_ULONG, lFilelen_outother,
                   "MD5_17",       UT_TYPE_STRING, caMd5_outother,
                   "Filename_18", UT_TYPE_STRING, caFile_outhttps,
                   "Pathname_18", UT_TYPE_STRING, caPath_outhttps,
                   "Filelenth_18", UT_TYPE_ULONG, lFilelen_outhttps,
                   "MD5_18",       UT_TYPE_STRING, caMd5_outhttps,
                   "Filename_19", UT_TYPE_STRING, caFile_outfreeblock,
                   "Pathname_19", UT_TYPE_STRING, caPath_outfreeblock,
                   "Filelenth_19", UT_TYPE_ULONG, lFilelen_outfreeblock,
                   "MD5_19",       UT_TYPE_STRING, caMd5_outfreeblock);
    return 0;
}

//权限配置文件
int ncsSysUserRoleConfig_v4(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caTemp[1024];
    int  iNum;
    char caFun[2048];
    char caTool[2048];
    long lLevel;
    utPltDbHead *psDbHead;
    int iReturn, i;
    long lCount;
    char caName[64], caSy[64] = "";
    long lId, lSid;

    dsCltGetSessionValue(1, "id", UT_TYPE_LONG, sizeof(long), &lId);

    utMsgPrintMsg(psMsgHead);
    memset(caFun, 0, sizeof(caFun));



    sprintf(caTemp, "select lright from dsuser a,dsuserrole b,dsroleattr c,dscltright d where a.id=b.id and b.roleid=c.roleid and d.name=lright and a.id=%d group by lright", lId);

    printf("caTemp=%s\n", caTemp);
    psCur = pasDbOpenSqlF(caTemp);


    if(psCur == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "ncs/ncmsg_back.htm", "部门查询", "数据库出错");
        //        free(pHash);
        return 0;
    }

    memset(caName, 0, sizeof(caName));
    iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 31, caName);
    iNum = 0;
    //char caNameBig5[32] = "";
    while(iReturn == 0 || iReturn == 1405)
    {
        //strcpy(caNameBig5,convert("GBK", "BIG5", caName));
        if(iNum == 0)
        {
            //strcpy(caFun,caNameBig5);
            strcpy(caFun, caName);
        }
        else
        {
            //sprintf(caFun+strlen(caFun),",%s",caNameBig5);
            sprintf(caFun + strlen(caFun), ",%s", caName);
        }
        iNum++;

        memset(caName, 0, sizeof(caName));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 31, caName);
    }
    pasDbCloseCursor(psCur);

    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead, "sy", caSy);
    utPltPutVar(psDbHead, "fun", caFun);



    // utPltShowDb(psDbHead);

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_right_config.htm");


    return 0;
}




/* 显示全局参数?柚? */
int lanParmSet(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    utPltDbHead *psDbHead;
    utMsgPrintMsg(psMsgHead);
    psDbHead = utPltInitDb();

    char sql[512] = "";
    char auth[32] = "";
    char display[32] = "";
    int count = 0;

    dsCltGetMyInfo(1, "USERNAME", auth);

    sprintf(sql, "select count(*) from usercommon where auth = '%s'", auth);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count);

    if(count > 0)
    {
        sprintf(sql, "select display from usercommon where auth = '%s'", auth);
        pasDbOneRecord(sql, 0, UT_TYPE_STRING, sizeof(display), display);
    }
    else
    {
        strcpy(display, "compname");
    }

    utPltPutVar(psDbHead, "filedname", display);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/set/lan_glob_set.html");

    return 0;
}

//保存全局参数设置
int lanParmSet_Save(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);
    char *pConfig;
    pasConfList *psConfig;
    int iReturn;

    utPltDbHead *psDbHead;
    char caFiledname[20];
    char unstall_key[16] = "";
    iReturn = utMsgGetSomeNVar(psMsgHead, 2,
                               "filedname", UT_TYPE_STRING, 15, caFiledname,
                               "unstall_key", UT_TYPE_STRING, 15, unstall_key);

    psDbHead = utPltInitDb();
    //    pConfig = pasGetConfigFileName();
    //    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0880自动备份设定"), ncLang("0427配置文件出错"));
        return 0;
    }

    //      writeSysLog(psMsgHead,"03","Success","保存了全局参数设置");
    //     pasUtlModiConfig(psConfig,"dispgname",caFiledname);
    //     pasUtlModiConfig(psConfig,"unstall_key",unstall_key);

    char sql[512] = "";
    char auth[32] = "";
    int count = 0;

    dsCltGetMyInfo(1, "USERNAME", auth);

    //printf("caFiledname=%s,%s\n",caFiledname,auth);

    sprintf(sql, "select count(*) from usercommon where auth = '%s'", auth);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count);

    if(count > 0)
    {
        sprintf(sql, "update usercommon set display='%s' where auth='%s'", caFiledname, auth);
    }
    else
    {
        sprintf(sql, "insert into usercommon (display,auth)values('%s','%s')", caFiledname, auth);
    }

    pasDbExecSqlF(sql);
    //    pasUtlSaveConfig(pConfig,psConfig);
    //    pasUtlFreeConfig(psConfig);

    //    ncUtlReReadConFig(psShmHead);
    //   ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
    utPltPutVar(psDbHead, "mesg", "成功修改");
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_msg.htm");

    return 0;
}

/* 显示IP参数设置  */
int lanIpParmSet(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    utPltDbHead *psDbHead;
    utMsgPrintMsg(psMsgHead);
    psDbHead = utPltInitDb();

    utPltPutVar(psDbHead, "AdminIp", utComGetVar_sd(psShmHead, "AdminIp", ""));
    utPltPutVar(psDbHead, "AdminPort", utComGetVar_sd(psShmHead, "AdminPort", ""));

    utPltPutVar(psDbHead, "eth0IP", utComGetVar_sd(psShmHead, "eth0IP", ""));
    utPltPutVar(psDbHead, "eth0Mask", utComGetVar_sd(psShmHead, "eth0Mask", ""));

    utPltPutVar(psDbHead, "eth1IP", utComGetVar_sd(psShmHead, "eth1IP", ""));
    utPltPutVar(psDbHead, "eth1Mask", utComGetVar_sd(psShmHead, "eth1Mask", ""));

    utPltPutVar(psDbHead, "defgateway", utComGetVar_sd(psShmHead, "defgateway", ""));
    utPltPutVar(psDbHead, "dnsserver1", utComGetVar_sd(psShmHead, "dnsserver1", ""));

    utPltPutVar(psDbHead, "dnsserver2", utComGetVar_sd(psShmHead, "dnsserver2", ""));

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/set/lan_ipaddress_set.html");

    return 0;
}

//保存IP地址设置
int lanIpParmSet_save(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char *pConfig;
    pasConfList *psConfig;
    int iReturn;
    utPltDbHead *psDbHead;
    char caApp[16];
    char caAdminIp[16], caAdminPort[16], caEth0IP[16], caEth0Mask[16], caEth1IP[16], caEth1Mask[16], caDefgateway[16], caDnsserver1[16], caDnsserver2[16];
    iReturn = utMsgGetSomeNVar(psMsgHead, 10, "AdminIp", UT_TYPE_STRING, 15, caAdminIp,
                               "AdminPort", UT_TYPE_STRING, 6, caAdminPort,
                               "eth0IP",  UT_TYPE_STRING, 15, caEth0IP,
                               "eth0Mask", UT_TYPE_STRING, 15, caEth0Mask,
                               "eth1IP", UT_TYPE_STRING, 15, caEth1IP,
                               "eth1Mask", UT_TYPE_STRING, 15, caEth1Mask,
                               "defgateway", UT_TYPE_STRING, 15, caDefgateway,
                               "dnsserver1", UT_TYPE_STRING, 15, caDnsserver1,
                               "dnsserver2", UT_TYPE_STRING, 15, caDnsserver2,
                               "app", UT_TYPE_STRING, 10, caApp);

    psDbHead = utPltInitDb();
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0880自动备份设定"), ncLang("0427配置文件出错"));
        return 0;
    }


    pasUtlModiConfig(psConfig, "AdminIp", caAdminIp);
    pasUtlModiConfig(psConfig, "AdminPort", caAdminPort);

    pasUtlModiConfig(psConfig, "eth0IP", caEth0IP);
    pasUtlModiConfig(psConfig, "eth0Mask", caEth0Mask);

    pasUtlModiConfig(psConfig, "eth1IP", caEth1IP);
    pasUtlModiConfig(psConfig, "eth1Mask", caEth1Mask);

    pasUtlModiConfig(psConfig, "defgateway", caDefgateway);
    pasUtlModiConfig(psConfig, "dnsserver1", caDnsserver1);

    pasUtlModiConfig(psConfig, "dnsserver2", caDnsserver2);

    pasUtlSaveConfig(pConfig, psConfig);
    pasUtlFreeConfig(psConfig);

    ncUtlReReadConFig(psShmHead);
    //   ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
    utPltPutVar(psDbHead, "mesg", "成功修改");
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_msg.htm");
    lanSystemGenNetFile(psShmHead, "/home/ncmysql/nw/bin/startnet");
    if(strlen(caApp) > 0)
    {
        system("cd /home/ncmysql/nw/httpd/bin;./apachectl restart");
        system("/home/ncmysql/nw/bin/startnet.undo");
        system("/home/ncmysql/nw/bin/startnet");
        writeSysLog(psMsgHead, "03", "Success", "应用了IP地址参数设置");
    }
    else
    {
        writeSysLog(psMsgHead, "03", "Success", "保存了IP地址参数设置");
    }


    return 0;
}



/* 显示自动整理参数设置  */
int lanCleanParmSet(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    utPltDbHead *psDbHead;
    utMsgPrintMsg(psMsgHead);
    psDbHead = utPltInitDb();

    utPltPutVar(psDbHead, "StartClean", utComGetVar_sd(psShmHead, "StartClean", "Yes"));
    utPltPutVar(psDbHead, "CleanupTime", utComGetVar_sd(psShmHead, "CleanupTime", "01:00:00"));

    utPltPutVar(psDbHead, "InstantKeepMon", utComGetVar_sd(psShmHead, "InstantKeepMon", "2"));
    utPltPutVar(psDbHead, "InstantBakKeep", utComGetVar_sd(psShmHead, "InstantBakKeep", "2"));

    utPltPutVar(psDbHead, "ScreenKeepMon", utComGetVar_sd(psShmHead, "ScreenKeepMon", "2"));
    utPltPutVar(psDbHead, "ScreenBakKeep", utComGetVar_sd(psShmHead, "ScreenBakKeep", "2"));

    utPltPutVar(psDbHead, "WebKeepMon", utComGetVar_sd(psShmHead, "WebKeepMon", "2"));
    utPltPutVar(psDbHead, "WebBakKeep", utComGetVar_sd(psShmHead, "WebBakKeep", "2"));

    utPltPutVar(psDbHead, "ProcKeepMon", utComGetVar_sd(psShmHead, "ProcKeepMon", "2"));
    utPltPutVar(psDbHead, "ProcBakKeep", utComGetVar_sd(psShmHead, "ProcBakKeep", "2"));


    utPltPutVar(psDbHead, "FileKeepMon", utComGetVar_sd(psShmHead, "FileKeepMon", "2"));
    utPltPutVar(psDbHead, "FileBakKeep", utComGetVar_sd(psShmHead, "FileBakKeep", "2"));

    utPltPutVar(psDbHead, "WarnKeepMon", utComGetVar_sd(psShmHead, "WarnKeepMon", "2"));
    utPltPutVar(psDbHead, "WarnBakKeep", utComGetVar_sd(psShmHead, "WarnBakKeep", "2"));

    utPltPutVar(psDbHead, "DevKeepMon", utComGetVar_sd(psShmHead, "DevKeepMon", "2"));
    utPltPutVar(psDbHead, "DevBakKeep", utComGetVar_sd(psShmHead, "DevBakKeep", "2"));

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/set/lan_cleanparm_set.html");

    return 0;
}



//保存自动整理参数设置
int lanCleanParmSet_Save(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char *pConfig;
    pasConfList *psConfig;
    int iReturn;
    utPltDbHead *psDbHead;
    char caApp[16];
    char caStartClean[16], caCleanupTime[16], caInstantKeepMon[16], caInstantBakKeep[16];
    char caScreenKeepMon[16], caScreenBakKeep[16], caWebKeepMon[16], caWebBakKeep[16];
    char caProcKeepMon[16], caProcBakKeep[16], caDevKeepMon[16], caDevBakKeep[16];
    char caFileKeepMon[16], caFileBakKeep[16], caWarnKeepMon[16], caWarnBakKeep[16];
    iReturn = utMsgGetSomeNVar(psMsgHead, 17, "StartClean", UT_TYPE_STRING, 15, caStartClean,
                               "CleanupTime", UT_TYPE_STRING, 15, caCleanupTime,
                               "InstantKeepMon",  UT_TYPE_STRING, 15, caInstantKeepMon,
                               "InstantBakKeep", UT_TYPE_STRING, 15, caInstantBakKeep,
                               "ScreenKeepMon", UT_TYPE_STRING, 15, caScreenKeepMon,
                               "ScreenBakKeep", UT_TYPE_STRING, 15, caScreenBakKeep,
                               "WebKeepMon", UT_TYPE_STRING, 15, caWebKeepMon,
                               "WebBakKeep", UT_TYPE_STRING, 15, caWebBakKeep,
                               "ProcKeepMon", UT_TYPE_STRING, 15, caProcKeepMon,
                               "ProcBakKeep",  UT_TYPE_STRING, 15, caProcBakKeep,
                               "DevKeepMon", UT_TYPE_STRING, 15, caDevKeepMon,
                               "DevBakKeep", UT_TYPE_STRING, 15, caDevBakKeep,
                               "FileKeepMon", UT_TYPE_STRING, 15, caFileKeepMon,
                               "FileBakKeep", UT_TYPE_STRING, 15, caFileBakKeep,
                               "WarnKeepMon", UT_TYPE_STRING, 15, caWarnKeepMon,
                               "WarnBakKeep", UT_TYPE_STRING, 15, caWarnBakKeep,
                               "app", UT_TYPE_STRING, 10, caApp);

    psDbHead = utPltInitDb();
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0880自动备份设定"), ncLang("0427配置文件出错"));
        return 0;
    }


    pasUtlModiConfig(psConfig, "StartClean", caStartClean);
    pasUtlModiConfig(psConfig, "CleanupTime", caCleanupTime);

    pasUtlModiConfig(psConfig, "InstantKeepMon", caInstantKeepMon);
    pasUtlModiConfig(psConfig, "InstantBakKeep", caInstantBakKeep);

    pasUtlModiConfig(psConfig, "ScreenKeepMon", caScreenKeepMon);
    pasUtlModiConfig(psConfig, "ScreenBakKeep", caScreenBakKeep);
    pasUtlModiConfig(psConfig, "WebKeepMon", caWebKeepMon);
    pasUtlModiConfig(psConfig, "WebBakKeep", caWebBakKeep);
    pasUtlModiConfig(psConfig, "ProcKeepMon", caProcKeepMon);

    pasUtlModiConfig(psConfig, "ProcBakKeep", caScreenKeepMon);
    pasUtlModiConfig(psConfig, "DevKeepMon", caDevKeepMon);
    pasUtlModiConfig(psConfig, "DevBakKeep", caDevBakKeep);
    pasUtlModiConfig(psConfig, "FileKeepMon", caFileKeepMon);
    pasUtlModiConfig(psConfig, "FileBakKeep", caFileBakKeep);

    pasUtlModiConfig(psConfig, "WarnKeepMon", caWarnKeepMon);
    pasUtlModiConfig(psConfig, "WarnBakKeep", caWarnBakKeep);

    pasUtlSaveConfig(pConfig, psConfig);
    pasUtlFreeConfig(psConfig);

    ncUtlReReadConFig(psShmHead);
    //   ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
    utPltPutVar(psDbHead, "mesg", "成功修改");
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_msg.htm");

    if(strlen(caApp) > 0)
    {
        utComSetVar_s(psShmHead, "imclean", "clean");
        writeSysLog(psMsgHead, "03", "Success", "手动执行了数据存储参数设置");
    }
    else
    {
        writeSysLog(psMsgHead, "03", "Success", "保存了数据存储参数设置");
    }


    return 0;
}

//测试是否登录，用作首页检测是否需要重新登录
int ncsTestRight_v4(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    unsigned long iLicense, lExpire, lUsers, lStart;
    char *pProdSn;
    psDbHead = utPltInitDb();

    pProdSn = utComGetVar_sd(psShmHead, "ProductSn", "\0");

    iLicense = pasInstGetLicense(pProdSn, &lExpire, &lUsers, &lStart);

    if(iLicense != 1)
    {
        utPltPutVar(psDbHead, "mesg", "true");
        utPltPutVarF(psDbHead, "userid", "许可证已于%s到期,请及时更新许可证,否则会造成某些功能不正常。", utTimFormat("%Y-%m-%d %H:%M:%S", lExpire));
        utPltPutVar(psDbHead, "reason", "license");
    }
    else
    {
        utPltPutVar(psDbHead, "mesg", "true");
        utPltPutVar(psDbHead, "userid", "ok");
    }




    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_msg.htm");
    return 0;
}

/* 显示自动备份参数设置  */
int lanBackParmSet(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    utPltDbHead *psDbHead;
    utMsgPrintMsg(psMsgHead);
    psDbHead = utPltInitDb();

    utPltPutVar(psDbHead, "StartBack", utComGetVar_sd(psShmHead, "StartBack", "Yes"));
    utPltPutVar(psDbHead, "BackTime", utComGetVar_sd(psShmHead, "BackTime", "23:00:00"));

    utPltPutVar(psDbHead, "BackBaseDB", utComGetVar_sd(psShmHead, "BackBaseDB", "Yes"));
    utPltPutVar(psDbHead, "BackIm", utComGetVar_sd(psShmHead, "BackIm", "Yes"));

    utPltPutVar(psDbHead, "BackScreen", utComGetVar_sd(psShmHead, "BackScreen", "Yes"));

    utPltPutVar(psDbHead, "BackWeblog", utComGetVar_sd(psShmHead, "BackWeblog", "Yes"));
    utPltPutVar(psDbHead, "BackProcess", utComGetVar_sd(psShmHead, "BackProcess", "Yes"));

    utPltPutVar(psDbHead, "BackDev", utComGetVar_sd(psShmHead, "BackDev", "Yes"));
    utPltPutVar(psDbHead, "BackFile", utComGetVar_sd(psShmHead, "BackFile", "Yes"));


    utPltPutVar(psDbHead, "BackWarnlog", utComGetVar_sd(psShmHead, "BackWarnlog", "Yes"));
    utPltPutVar(psDbHead, "StartRmBack", utComGetVar_sd(psShmHead, "StartRmBack", "Yes"));

    utPltPutVar(psDbHead, "rip", utComGetVar_sd(psShmHead, "rip", ""));
    utPltPutVar(psDbHead, "rhost", utComGetVar_sd(psShmHead, "rhost", ""));

    utPltPutVar(psDbHead, "ruser", utComGetVar_sd(psShmHead, "ruser", ""));
    utPltPutVar(psDbHead, "rpass", utComGetVar_sd(psShmHead, "rpass", ""));
    utPltPutVar(psDbHead, "rshare", utComGetVar_sd(psShmHead, "rshare", ""));
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/set/lan_backparm_set.html");

    return 0;
}


//保存自动备份参数设置
int lanBackParmSet_Save(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char *pConfig;
    pasConfList *psConfig;
    int iReturn;
    utPltDbHead *psDbHead;
    char caApp[16];
    char caStartBack[16], caBackTime[16], caBackBaseDB[16], caBackIm[16], caBackScreen[16], caBackWeblog[16];
    char caBackProcess[16], caBackDev[16], caBackFile[16], caBackWarnlog[16], caStartRmBack[16];
    char caRip[16], caRhost[16], caRuser[16], caRpass[16], caRshare[16];
    iReturn = utMsgGetSomeNVar(psMsgHead, 17, "StartBack", UT_TYPE_STRING, 15, caStartBack,
                               "BackTime", UT_TYPE_STRING, 15, caBackTime,
                               "BackBaseDB",  UT_TYPE_STRING, 15, caBackBaseDB,
                               "BackIm", UT_TYPE_STRING, 15, caBackIm,
                               "BackScreen", UT_TYPE_STRING, 15, caBackScreen,
                               "BackWeblog", UT_TYPE_STRING, 15, caBackWeblog,
                               "BackProcess", UT_TYPE_STRING, 15, caBackProcess,
                               "BackDev", UT_TYPE_STRING, 15, caBackDev,
                               "BackFile", UT_TYPE_STRING, 15, caBackFile,
                               "BackWarnlog",  UT_TYPE_STRING, 15, caBackWarnlog,
                               "StartRmBack", UT_TYPE_STRING, 15, caStartRmBack,
                               "rip", UT_TYPE_STRING, 15, caRip,
                               "rhost", UT_TYPE_STRING, 15, caRhost,
                               "ruser", UT_TYPE_STRING, 15, caRuser,
                               "rpass", UT_TYPE_STRING, 15, caRpass,
                               "rshare", UT_TYPE_STRING, 15, caRshare,
                               "app", UT_TYPE_STRING, 10, caApp);

    if(strlen(caBackBaseDB) == 0)
    {
        strcpy(caBackBaseDB, "No");
    }
    if(strlen(caBackIm) == 0)
    {
        strcpy(caBackIm, "No");
    }
    if(strlen(caBackScreen) == 0)
    {
        strcpy(caBackScreen, "No");
    }
    if(strlen(caBackWeblog) == 0)
    {
        strcpy(caBackWeblog, "No");
    }
    if(strlen(caBackProcess) == 0)
    {
        strcpy(caBackProcess, "No");
    }
    if(strlen(caBackDev) == 0)
    {
        strcpy(caBackDev, "No");
    }
    if(strlen(caBackFile) == 0)
    {
        strcpy(caBackFile, "No");
    }
    if(strlen(caBackWarnlog) == 0)
    {
        strcpy(caBackWarnlog, "No");
    }

    psDbHead = utPltInitDb();
    pConfig = pasGetConfigFileName();
    psConfig = pasUtlReadConfig(pConfig);
    if(psConfig == NULL)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0880自动备份设定"), ncLang("0427配置文件出错"));
        return 0;
    }


    pasUtlModiConfig(psConfig, "StartBack", caStartBack);
    pasUtlModiConfig(psConfig, "BackTime", caBackTime);

    pasUtlModiConfig(psConfig, "BackBaseDB", caBackBaseDB);
    pasUtlModiConfig(psConfig, "BackIm", caBackIm);

    pasUtlModiConfig(psConfig, "BackScreen", caBackScreen);
    pasUtlModiConfig(psConfig, "BackWeblog", caBackWeblog);
    pasUtlModiConfig(psConfig, "BackProcess", caBackProcess);
    pasUtlModiConfig(psConfig, "BackDev", caBackDev);
    pasUtlModiConfig(psConfig, "BackFile", caBackFile);

    pasUtlModiConfig(psConfig, "BackWarnlog", caBackWarnlog);
    pasUtlModiConfig(psConfig, "StartRmBack", caStartRmBack);
    pasUtlModiConfig(psConfig, "rip", caRip);
    pasUtlModiConfig(psConfig, "rhost", caRhost);
    pasUtlModiConfig(psConfig, "ruser", caRuser);

    pasUtlModiConfig(psConfig, "rpass", caRpass);
    pasUtlModiConfig(psConfig, "rshare", caRshare);

    pasUtlSaveConfig(pConfig, psConfig);
    pasUtlFreeConfig(psConfig);

    ncUtlReReadConFig(psShmHead);
    //   ncUtlReloadConfig(psShmHead,NC_PID_COLLECT,NC_RELOAD_PQCONFIG,1);
    utPltPutVar(psDbHead, "mesg", "成功修改");
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_msg.htm");

    if(strlen(caApp) > 0)
    {
        utComSetVar_s(psShmHead, "imback", "back");

        writeSysLog(psMsgHead, "06", "Success", "手动执行自动备份参数设置");
    }
    else
    {
        writeSysLog(psMsgHead, "03", "Success", "保存自动备份参数设置");
    }


    return 0;
}

//测试远程备份
int lanBackParmSet_Test(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char *pConfig;
    pasConfList *psConfig;
    int iReturn;
    utPltDbHead *psDbHead;
    char caApp[16];
    char caStartBack[16], caBackTime[16], caBackBaseDB[16], caBackIm[16], caBackScreen[16], caBackWeblog[16];
    char caBackProcess[16], caBackDev[16], caBackFile[16], caBackWarnlog[16], caStartRmBack[16];
    char caRip[16], caRhost[16], caRuser[16], caRpass[16], caRshare[16];
    char caMount[128], caMessage[128];
    iReturn = utMsgGetSomeNVar(psMsgHead, 6,
                               "StartRmBack", UT_TYPE_STRING, 15, caStartRmBack,
                               "rip", UT_TYPE_STRING, 15, caRip,
                               "rhost", UT_TYPE_STRING, 15, caRhost,
                               "ruser", UT_TYPE_STRING, 15, caRuser,
                               "rpass", UT_TYPE_STRING, 15, caRpass,
                               "rshare", UT_TYPE_STRING, 15, caRshare);




    psDbHead = utPltInitDb();
    strcpy(caMount, "/home/ncmysql/nw/keep/remote");
    if(strcasecmp(caStartRmBack, "Yes") == 0) //测试SAMBA
    {
        printf("caMount=%s\n", caMount);
        ncSendRemoteFile(NC_REMOTE_MOUNTTEST, caRip, caRhost, caRuser, caRpass, caRshare, caMount, NULL, NULL, NULL, caMessage);
        printf("caMessage=%s\n", caMessage);
        if(strstr(caMessage, "系统连接不成功"))
        {
            strcpy(caMessage, "系统连接不成功!");
        }
        utPltPutVar(psDbHead, "mesg", caMessage);
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_msg.htm");
        return 0;
    }

    if(strcasecmp(caStartRmBack, "FtpBackup") == 0) //测试 FTP
    {
        int iRet, iReturn;
        iReturn = -1;


        char caHost[64];
        char caUser[64];
        char caPass[64];

        //     strcpy(caHost,utComGetVar_sd(psShmHead,"mremotebakip","\0"));
        //     strcpy(caUser,utComGetVar_sd(psShmHead,"mremotebakuser","\0"));
        //      strcpy(caPass,utComGetVar_sd(psShmHead,"mremotebakpass","\0"));


        FILE *fp = fopen("test.file", "w");
        fprintf(fp, "this is only a test file \n");
        fclose(fp);

        //fprintf(stdout, "host = %s, user = %s, pass = %s\n", caHost,caUser, caPass);
        printf("host = %s, user = %s, pass = %s\n", caRip, caRuser, caRpass);
        iRet = pasFtpConnect(caRip, 21, caRuser, caRpass);
        if(iRet < 0)
        {
            printf(" Ftp Connect Error iReturn==%d \n", iRet);


        }
        else
        {

            iReturn = pasFtpSend(iRet, "test.file", "I");

            // iReturn = pasFtpSend(iRet, "test.file", "test.file", "I");
            if(iReturn < 0)
            {
                printf(" Send File Error  iReturn==%d\n", iReturn);
            }
            pasFtpClose(iRet);
        }
        if(iRet >= 0 && iReturn >= 0)
        {
            utPltPutVar(psDbHead, "mesg", "文件已经成功发送!");

        }
        else
        {
            utPltPutVar(psDbHead, "mesg", "文件发送失败!");


        }

        printf("cccccc\n");
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_msg.htm");


    }

    return 0;
}




/* 生成网络设置配置文件  */
int lanSystemGenNetFile(utShmHead *psShmHead, char *pGenFIle)
{
    utPltDbHead *psDbHead;
    FILE *fp, *fpi, *fpo;
    char *p, *p1, *pFile;
    char caCommand[128];
    char caTemp[256], caDev[32];
    char caRoute[32], caMask[32], caGate[32], caVar[16];
    char caBrName[32], caBrDev[32], caIp[32];
    long lRoute;
    int i, iSumDev;
    char *p2;

    fp = fopen(pGenFIle, "w");
    if(fp == NULL)
    {
        utSysLog(" File Open Error \n");
        return (-1);
    }
    sprintf(caTemp, "%s.undo", pGenFIle);
    fpo = fopen(caTemp, "w");
    if(fpo == NULL)
    {
        return (-1);
    }

    fprintf(fp, "# This File Generate By nw \n");
    fprintf(fp, "# Powered By Pronetway\n");
    fprintf(fp, "\n# Config Network Interface Ip Address\n");

    /* 网络配置   */
    for(i = 0; i < 2; i++)
    {
        sprintf(caVar, "eth%dIP", i);
        sprintf(caMask, "eth%dMask", i);
        sprintf(caDev, "eth%d", i);
        p = utComGetVar_sd(psShmHead, caVar, "\0");
        p2 = utComGetVar_sd(psShmHead, caMask, "\0");
        if(strlen(p) > 0 && strlen(p2) > 0)
        {
            fprintf(fp, "/sbin/ifconfig  %s  %s netmask %s\n", caDev, p, p2);
        }

    }

    fprintf(fp, "# End Config Bridge\n");


    /* 缺省网关  */
    if(p = utComGetVar_s(psShmHead, "defgateway"))
    {
        if(!utStrIsSpaces(p))
        {
            fprintf(fp, "/sbin/route add default  gateway  %s\n", p);
            fprintf(fpo, "/usr/sbin/brctl delif pronc eth0");
            fprintf(fpo, "/usr/sbin/brctl delif pronc eth1");
            fprintf(fpo, "/sbin/ifconfig pronc down");
            fprintf(fpo, "/usr/sbin/brctl delbr pronc");
            fprintf(fpo, "/sbin/route del default  gateway  %s\n", p);
        }
    }
    fclose(fpo);


    fclose(fp);
    sprintf(caCommand, "/bin/chmod ugoa+rx %s", pGenFIle);
    system(caCommand);
    sprintf(caTemp, "%s.undo", pGenFIle);
    sprintf(caCommand, "/bin/chmod ugoa+rx %s", caTemp);
    system(caCommand);

    /* DNS 配置  */
    fp = fopen("/etc/resolv.conf", "w");
    if(p = utComGetVar_s(psShmHead, "dnsserver1"))
    {
        if(!utStrIsSpaces(p))
            fprintf(fp, "nameserver %s\n", p);
    }
    if(p = utComGetVar_s(psShmHead, "dnsserver2"))
    {
        if(!utStrIsSpaces(p))
            fprintf(fp, "nameserver %s\n", p);
    }
    fflush(fp);
    fclose(fp);
    /* 修改apache中的配置  */
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead, "adminport", utComGetVar_sd(psShmHead, "AdminPort", "80"));

    p = utPltFileToHtml("nc/httpd.conf", psDbHead);
    if(p)
    {
        p = utStrSkipSpaces(p);
        rename("/home/ncmysql/nw/httpd/conf/httpd.conf", "/home/ncmysql/nw/httpd/conf/httpd.conf.bak");
        fp = fopen("/home/ncmysql/nw/httpd/conf/httpd.conf", "w");
        if(fp)
        {
            fwrite(p, 1, strlen(p), fp);
            fflush(fp);
            fclose(fp);
        }
    }
    return 0;
}


//关机和重启
int lanShutDown(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char *pConfig;
    pasConfList *psConfig;
    int iReturn;
    utPltDbHead *psDbHead;
    char caFiledname[20];
    long lOpt;
    char caUsername2[32];
    memset(caUsername2, 0, sizeof(caUsername2));
    dsCltGetSessionValue(1, "dispname", UT_TYPE_STRING, 30, caUsername2);
    iReturn = utMsgGetSomeNVar(psMsgHead, 1, "filedname", UT_TYPE_STRING, 15, caFiledname);

    if(caFiledname[0] == '6')
    {
        lOpt = 6;
        ncsWriteSysLog(psMsgHead, caUsername2, "11", "success", ncLang("1234系统重启"));
    }
    else
    {
        lOpt = 0;
        ncsWriteSysLog(psMsgHead, caUsername2, "11", "success", ncLang("1233系统关机"));
    }
    //printf("caFiledname=%s\n",caFiledname);
    psDbHead = utPltInitDb();
    utSysShutdown(lOpt);

    utPltPutVar(psDbHead, "mesg", ncLang("0129正在操作, 请稍候..."));
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v4/ncs_msg.htm");

    return 0;
}



int lan_recover_system(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);
    utPltDbHead *psDbHead = utPltInitDb();
    utPltFreeDb(psDbHead);

    printf("[reset system]\n");
    if(!utFileIsExist("/home/ncmysql/nw/bin/recover"))
    {
        utPltPutVar(psDbHead, "buff", "{success:false, mesg:\'reset fail\'}");
    }
    else
    {
        system("chmod -Rf 777 /home/ncmysql/nw/bin/recover");
        system("./recover");
        pasKillProcess(NULL, "nw");
        exit(0);
        utPltPutVar(psDbHead, "buff", "{success:true, mesg:\'reset success\'}");
    }
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_tmpPlate.htm");
    return 0;
}


int lanWeb_initFun(utShmHead *psShmHead)
{
    int iReturn;
    iReturn = pasSetTcpFunName("lan_recover_system", lan_recover_system, 0);
    iReturn = pasSetTcpFunName("lanTree_depComp", lanTree_depComp, 0);
    iReturn = pasSetTcpFunName("lanTreeProcy", lanTreeProcy, 0);
    iReturn = pasSetTcpFunName("lan_Procy_File", lan_Procy_File, 0);
    iReturn = pasSetTcpFunName("lan_OnlineCompute", lan_OnlineCompute, 0);

    iReturn = pasSetTcpFunName("ncsDispRoleFunlist_v4", ncsDispRoleFunlist_v4, 0);
    iReturn = pasSetTcpFunName("ncsRolelist_v4", ncsRolelist_v4, 0);
    iReturn = pasSetTcpFunName("ncsSysUserlist_v4", ncsSysUserlist_v4, 0);
    iReturn = pasSetTcpFunName("ncsWebAdminDispUserForm", ncsWebAdminDispUserForm, 0);
    iReturn = pasSetTcpFunName("ncsAdminUserSave", ncsAdminUserSave, 0);
    iReturn = pasSetTcpFunName("ncsTreeGroup_v4", ncsTreeGroup_v4, 0);
    iReturn = pasSetTcpFunName("ncsTreeGroup_comp_v4", ncsTreeGroup_comp_v4, 0);
    iReturn = pasSetTcpFunName("lanSoftlist_v4", lanSoftlist_v4, 0);
    iReturn = pasSetTcpFunName("lanSoftUpload_save", lanSoftUpload_save, 0);
    iReturn = pasSetTcpFunName("ncupSoftSave", ncupSoftSave, 0);
    iReturn = pasSetTcpFunName("lanDispSoftUpload", lanDispSoftUpload, 0);
    iReturn = pasSetTcpFunName("lan_softupdate_procy", lan_softupdate_procy, 0);
    iReturn = pasSetTcpFunName("Lan_UpdateSoftReq", Lan_UpdateSoftReq, 0);
    iReturn = pasSetTcpFunName("Lan_UpdateSoftReport", Lan_UpdateSoftReport, 0);
    iReturn = pasSetTcpFunName("Lan_UpdateSoftBegin", Lan_UpdateSoftBegin, 0);
    iReturn = pasSetTcpFunName("ncsSysReflesh_v4", ncsSysReflesh_v4, 0);
    iReturn = pasSetTcpFunName("Lan_GetBaseProcyFileReq", Lan_GetBaseProcyFileReq, 0);
    iReturn = pasSetTcpFunName("Lan_FileDownLoad", Lan_FileDownLoad, 0);

    iReturn = pasSetTcpFunName("Lan_GetProcyFileReq", Lan_GetProcyFileReq, 0);
    iReturn = pasSetTcpFunName("ncsSysUserRoleConfig_v4", ncsSysUserRoleConfig_v4, 0);

    iReturn = pasSetTcpFunName("lanParmSet", lanParmSet, 0);
    iReturn = pasSetTcpFunName("lanParmSet_Save", lanParmSet_Save, 0);

    iReturn = pasSetTcpFunName("lanIpParmSet", lanIpParmSet, 0);

    iReturn = pasSetTcpFunName("lanIpParmSet_save", lanIpParmSet_save, 0);

    iReturn = pasSetTcpFunName("lanCleanParmSet", lanCleanParmSet, 0);

    iReturn = pasSetTcpFunName("lanCleanParmSet_Save", lanCleanParmSet_Save, 0);
    iReturn = pasSetTcpFunName("ncsTestRight_v4", ncsTestRight_v4, 0);
    iReturn = pasSetTcpFunName("lanBackParmSet", lanBackParmSet, 0);
    iReturn = pasSetTcpFunName("lanBackParmSet_Save", lanBackParmSet_Save, 0);
    iReturn = pasSetTcpFunName("lanShutDown", lanShutDown, 0);
    iReturn = pasSetTcpFunName("lanBackParmSet_Test", lanBackParmSet_Test, 0);

    return 0;
}


