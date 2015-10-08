#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoplt01.h"

int lan_userReg(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);

    char compName_in[32] = "";
    char compAddr_in[256] = "";
    char compSize_in[16] = "";
    char compPhone_in[16] = "";
    char compFax_in[16] = "";
    char compWeb_in[128] = "";
    char compMail_in[128] = "";
    char compBand_in[11] = "";
    char applName_in[32] = "";
    char applSex_in[8] = "";
    char applJob_in[32] = "";
    char applDepart_in[128] = "";
    char applPhone_in[16] = "";
    char applMobile_in[16] = "";
    char applMail_in[128] = "";
    char applQQ_in[16] = "";
    char clientNum_in[16] = "";
    char otherInfo_in[256] = "";
    char requirement_in[16] = "";

    utPltDbHead *psDbHead = utPltInitDb();

    utMsgGetSomeNVar(psMsgHead, 19,
                     "compName", UT_TYPE_STRING,  sizeof(compName_in) - 1,     compName_in,
                     "compAddr", UT_TYPE_STRING,  sizeof(compAddr_in) - 1,     compAddr_in,
                     "compSize", UT_TYPE_STRING,  sizeof(compSize_in) - 1, compSize_in,
                     "compPhone",    UT_TYPE_STRING,  sizeof(compPhone_in) - 1,    compPhone_in,
                     "compFax",  UT_TYPE_STRING,  sizeof(compFax_in) - 1,  compFax_in,
                     "compWeb",  UT_TYPE_STRING,  sizeof(compWeb_in) - 1,  compWeb_in,
                     "compMail", UT_TYPE_STRING,  sizeof(compMail_in) - 1,     compMail_in,
                     "compBand", UT_TYPE_STRING,  sizeof(compBand_in) - 1,     compBand_in,
                     "applName", UT_TYPE_STRING,  sizeof(applName_in) - 1,     applName_in,
                     "applSex",  UT_TYPE_STRING,  sizeof(applSex_in) - 1,  applSex_in,
                     "applJob",  UT_TYPE_STRING,  sizeof(applJob_in) - 1, applJob_in,
                     "applDepart",   UT_TYPE_STRING,  sizeof(applDepart_in) - 1,   applDepart_in,
                     "applPhone",    UT_TYPE_STRING,  sizeof(applPhone_in) - 1,    applPhone_in,
                     "applMobile",   UT_TYPE_STRING,  sizeof(applMobile_in) - 1,   applMobile_in,
                     "applMail", UT_TYPE_STRING,  sizeof(applMail_in) - 1,     applMail_in,
                     "applQQ",   UT_TYPE_STRING,  sizeof(applQQ_in) - 1,   applQQ_in,
                     "clientNum",    UT_TYPE_STRING,  sizeof(clientNum_in) - 1,    clientNum_in,
                     "qtContext",    UT_TYPE_STRING,  sizeof(otherInfo_in) - 1,    otherInfo_in,
                     "requirement",  UT_TYPE_STRING,  sizeof(requirement_in) - 1, requirement_in);

    /*  char *compName_gbk = convert("UTF-8", "GBK", compName_in);
        char *compAddr_gbk = convert("UTF-8", "GBK", compAddr_in);
        char *applName_gbk = convert("UTF-8", "GBK", applName_in);
        char *applSex_gbk = convert("UTF-8", "GBK", applSex_in);
        char *applJob_gbk = convert("UTF-8", "GBK", applJob_in);
        char *applDepart_gbk = convert("UTF-8", "GBK", applDepart_in);
        char *otherInfo_gbk = convert("UTF-8", "GBK", otherInfo_in);
    */

    char sql[1024] = "";

    sprintf(sql, "insert into userapply(compname,compaddr,compsize,compphone,compfax,compweb,compmail,compband,applname,applsex,appljob,appldepart,applphone,applmobile,applmail,applqq,clientnum,otherinfor,requirement,modtime)values('%s','%s',%s,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%s,'%s',%s,%u)", compName_in, compAddr_in, compSize_in, compPhone_in, compFax_in, compWeb_in, compMail_in, compBand_in, applName_in, applSex_in, applJob_in, applDepart_in, applPhone_in, applMobile_in, applMail_in, applQQ_in, clientNum_in, otherInfo_in, requirement_in, time(0));

    int ireturn = pasDbExecSqlF(sql);
    printf("spply-sql:%s\n", sql);

    if(ireturn == 0)
    {
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/apply/applySuccess.htm");
    }

    return 0;
}

int lan_appAdmin(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);

    char username_in[32] = "";
    char password_in[32] = "";

    utPltDbHead *psDbHead = utPltInitDb();

    utMsgGetSomeNVar(psMsgHead, 2,
                     "username", UT_TYPE_STRING,  sizeof(username_in) - 1,     username_in,
                     "password", UT_TYPE_STRING,  sizeof(password_in) - 1,     password_in);

    char sql[256] = "";

    sprintf(sql, "select count(*) from applymanage where username='%s' and password='%s'", username_in, password_in);

    int count = 0;

    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count);

    if(count > 0)
    {
    	utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/nwmanage.html");
     //   utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/apply/nwuserlist.htm");
    }
    else
    {
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/apply/nwusers.htm");
    }

    return 0;
}

int getApplyInfo(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);

    char sdate_in[24] = "";
    char edate_in[24] = "";
    char exp_in[8] = "";
    char start_in[24] = "";
    char limit_in[8] = "";

    utMsgGetSomeNVar(psMsgHead, 5,
                     "sdate",    UT_TYPE_STRING,  sizeof(sdate_in) - 1,    sdate_in,
                     "edate",    UT_TYPE_STRING,  sizeof(edate_in) - 1,    edate_in,
                     "exp",      UT_TYPE_STRING,  sizeof(exp_in) - 1,      exp_in,
                     "start",    UT_TYPE_STRING,  sizeof(start_in) - 1,    start_in,
                     "limit",        UT_TYPE_STRING,  sizeof(limit_in) - 1,    limit_in);

    char sql[512] = "";
    char sql_tmp[256] = "";
    unsigned int count = 0;
    utPltDbHead *psDbHead = utPltInitDb();

    sdate_in[10] = '\0';
    edate_in[10] = '\0';

    if(!utStrIsSpaces(sdate_in))
    {
        sprintf(sql_tmp, " and modtime>=%u", utTimStrToLong("%Y/%m/%d", sdate_in));
    }

    if(!utStrIsSpaces(edate_in))
    {
        sprintf(sql_tmp + strlen(sql_tmp), " and modtime<=%u", utTimStrToLong("%Y/%m/%d", edate_in) + 86399);
    }

    //导出到文件
    FILE *fp;
    char caFilename[32];
    char caFile[128];
    char caPath[128];

    if(strlen(exp_in) > 2)
    {
        time_t time_tmp;
        struct tm *tm_tmp;
        time(&time_tmp);
        tm_tmp = gmtime(&time_tmp);

        sprintf(caFilename, "nwappuser%04u%02u%02u.csv", tm_tmp->tm_year + 1900, tm_tmp->tm_mon + 1, tm_tmp->tm_mday);
        sprintf(caPath, "%s/html", "/home/ncmysql/nw");
        sprintf(caFile, "%s/%s", caPath, caFilename);
        fp = fopen(caFile, "w");
        if(fp == NULL)
        {
            printf("open failed!\n");
            return 0;
        }
        fprintf(fp, "内网用户申请信息\n");
        ncUtlPrintCsv(fp, 19,
                      UT_TYPE_STRING, "申请时间",
                      UT_TYPE_STRING, "公司名称",
                      UT_TYPE_STRING, "公司地址",
                      UT_TYPE_STRING, "公司规模",
                      UT_TYPE_STRING, "公司座机",
                      UT_TYPE_STRING, "公司传真",
                      UT_TYPE_STRING, "公司网站",
                      UT_TYPE_STRING, "公司邮箱",
                      UT_TYPE_STRING, "公司带宽",
                      UT_TYPE_STRING, "申请人姓名",
                      UT_TYPE_STRING, "申请人性别",
                      UT_TYPE_STRING, "申请人职位",
                      UT_TYPE_STRING, "申请人部门",
                      UT_TYPE_STRING, "申请人座机",
                      UT_TYPE_STRING, "申请人手机",
                      UT_TYPE_STRING, "申请人邮箱",
                      UT_TYPE_STRING, "申请人QQ",
                      UT_TYPE_STRING, "申请客户端",
                      UT_TYPE_STRING, "申请需求");
    }

    sprintf(sql, "select count(*) from userapply where 1=1 %s", sql_tmp);

    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count);

    utPltPutVarF(psDbHead, "TotRec", "%u", count);

    sprintf(sql, "select compname,compaddr,compsize,compphone,compfax,compweb,compmail,compband,applname,applsex,appljob,appldepart,applphone,applmobile,applmail,applqq,clientnum,requirement,otherinfor,from_unixtime(modtime) from userapply where 1=1 %s limit %s,%s", sql_tmp, start_in, limit_in);


    pasDbCursor *psCur = NULL;
    printf("用户申请：%s\n", sql);
    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        char compName[32] = "";
        char compAddr[256] = "";
        unsigned int compSize = 0;
        char compPhone[16] = "";
        char compFax[16] = "";
        char compWeb[128] = "";
        char compMail[128] = "";
        char compBand[16] = "";
        char applName[32] = "";
        char applSex[8] = "";
        char applJob[32] = "";
        char applDepart[128] = "";
        char applPhone[16] = "";
        char applMobile[16] = "";
        char applMail[128] = "";
        char applQQ[16] = "";
        unsigned int clientNum = 0;
        char otherInfo[256] = "";
        unsigned int requirement = 0;
        char modeTime[24] = "";

        char tmp[128] = "";
        int iret = 0, iNum = 0;
        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_STRING, sizeof(compName) - 1,    compName,
                                          UT_TYPE_STRING, sizeof(compAddr) - 1,  compAddr,
                                          UT_TYPE_ULONG,  4,                         &compSize,
                                          UT_TYPE_STRING, sizeof(compPhone) - 1, compPhone,
                                          UT_TYPE_STRING, sizeof(compFax) - 1,   compFax,
                                          UT_TYPE_STRING, sizeof(compWeb) - 1,   compWeb,
                                          UT_TYPE_STRING, sizeof(compMail) - 1,  compMail,
                                          UT_TYPE_STRING, sizeof(compBand) - 1,    compBand,
                                          UT_TYPE_STRING, sizeof(applName) - 1 , applName,
                                          UT_TYPE_STRING, sizeof(applSex) - 1 ,    applSex,
                                          UT_TYPE_STRING, sizeof(applJob) - 1 ,    applJob,
                                          UT_TYPE_STRING, sizeof(applDepart) - 1, applDepart,
                                          UT_TYPE_STRING, sizeof(applPhone) - 1, applPhone,
                                          UT_TYPE_STRING, sizeof(applMobile) - 1, applMobile,
                                          UT_TYPE_STRING, sizeof(applMail) - 1 ,   applMail,
                                          UT_TYPE_STRING, sizeof(applQQ) - 1 ,     applQQ,
                                          UT_TYPE_ULONG,  4,                         &clientNum,
                                          UT_TYPE_ULONG,  4,                                     &requirement,
                                          UT_TYPE_STRING, sizeof(otherInfo) - 1, otherInfo,
                                          UT_TYPE_STRING, sizeof(modeTime) - 1, modeTime))  || 1405 == iret)
        {
            printf("申请用户：循环里\n ");
            if(requirement % 2)
            {
                sprintf(tmp, "网站访问管理,");
            }
            requirement = requirement / 2;

            if(requirement % 2)
            {
                sprintf(tmp + strlen(tmp), "邮件审计,");
            }
            requirement = requirement / 2;

            if(requirement % 2)
            {
                sprintf(tmp + strlen(tmp), "带宽管理,");
            }
            requirement = requirement / 2;

            if(requirement % 2)
            {
                sprintf(tmp + strlen(tmp), "USB管理,");
            }
            requirement = requirement / 2;

            if(requirement % 2)
            {
                sprintf(tmp + strlen(tmp), "文件管理,");
            }
            requirement = requirement / 2;

            if(requirement % 2)
            {
                sprintf(tmp + strlen(tmp), "上网行为管理,");
            }
            requirement = requirement / 2;

            if(requirement % 2)
            {
                sprintf(tmp + strlen(tmp), "硬件、软件管理,");
            }
            requirement = requirement / 2;

            if(requirement % 2)
            {
                sprintf(tmp + strlen(tmp), otherInfo);
            }

            if(strlen(exp_in) > 2)
            {
                ncUtlPrintCsv(fp, 19,
                              UT_TYPE_STRING, modeTime,
                              UT_TYPE_STRING, compName,
                              UT_TYPE_STRING, compAddr,
                              UT_TYPE_ULONG, compSize,
                              UT_TYPE_STRING, compPhone,
                              UT_TYPE_STRING, compFax,
                              UT_TYPE_STRING, compWeb,
                              UT_TYPE_STRING, compMail,
                              UT_TYPE_STRING, compBand,
                              UT_TYPE_STRING, applName,
                              UT_TYPE_STRING, applSex,
                              UT_TYPE_STRING, applJob,
                              UT_TYPE_STRING, applDepart,
                              UT_TYPE_STRING, applPhone,
                              UT_TYPE_STRING, applMobile,
                              UT_TYPE_STRING, applMail,
                              UT_TYPE_STRING, applQQ,
                              UT_TYPE_ULONG, clientNum,
                              UT_TYPE_STRING, tmp);
            }
            else
            {

                printf("申请用户：循环里2\n ");
                iNum++;
                if(iNum > 1)
                {
                    utPltPutLoopVar(psDbHead, "dh", iNum, ",");
                }

                utPltPutLoopVar(psDbHead, "compname", iNum, compName);
                utPltPutLoopVar(psDbHead, "compaddr", iNum, compAddr);
                utPltPutLoopVarF(psDbHead, "compsize", iNum, "%u", compSize);
                utPltPutLoopVar(psDbHead, "compphone", iNum, compPhone);
                utPltPutLoopVar(psDbHead, "compfax", iNum, compFax);
                utPltPutLoopVar(psDbHead, "compweb", iNum, compWeb);
                utPltPutLoopVar(psDbHead, "compmail", iNum, compMail);
                utPltPutLoopVar(psDbHead, "compband", iNum, compBand);
                utPltPutLoopVar(psDbHead, "applname", iNum, applName);
                if(applSex[0] == 'M')
                {
                    utPltPutLoopVar(psDbHead, "applsex", iNum, "男");
                }
                else
                {
                    utPltPutLoopVar(psDbHead, "applsex", iNum, "女");
                }
                utPltPutLoopVar(psDbHead, "appljob", iNum, applJob);
                utPltPutLoopVar(psDbHead, "appldepart", iNum, applDepart);
                utPltPutLoopVar(psDbHead, "applphone", iNum, applPhone);
                utPltPutLoopVar(psDbHead, "applmobile", iNum, applMobile);
                utPltPutLoopVar(psDbHead, "applmail", iNum, applMail);
                utPltPutLoopVar(psDbHead, "applqq", iNum, applQQ);
                utPltPutLoopVarF(psDbHead, "clientnum", iNum, "%u", clientNum);
                utPltPutLoopVar(psDbHead, "modtime", iNum, modeTime);

                utPltPutLoopVar(psDbHead, "requirement", iNum, tmp);
            }
        }
        pasDbCloseCursor(psCur);
    }
    if(strlen(exp_in) > 2)
    {
        fclose(fp);
        utPltFileDownload(iFd, "application/text", caPath, caFilename, caFilename);
        remove(caFile);
    }
    else
    {

        printf("申请用户：模板中\n ");
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/apply/applyInfo.htm");
    }

    return 0;
}
