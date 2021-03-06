#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pasdb.h"
#include "utoplt01.h"

//繁体为 1  简体为0
#define BIG5 1


//邮件解码
char* MailCodeDecode(char* inputbuf, int datalen, char* encode, int *pOutLen)
{

    return NULL;
}



//UTF8 转码
char *utf8convert(utShmHead *psShmHead, char *src)
{

    char *strutf8 = NULL;
    if(src != NULL)
    {
        printf("nclang:%s\n",utComGetVar_sd(psShmHead,"nclang","\0"));
        if(strcmp(utComGetVar_sd(psShmHead, "nclang", "\0"), "big5") == 0)
        {
            strutf8 = convert("UTF-8", "BIG5", src);
        }
        else
        {
            strutf8 = convert("UTF-8", "GBK", src);
        }

        if(strutf8)
        {
            return strutf8;
        }
        else
        {
            return src;
        }
    }

    return src;
}

void switchtoUcode(char *src)
{
    char *p = src;
    while(*p != 0)
    {
        if(*p == '"')
        {
            *p = '\'';
        }
        p++;
    }
}

char *convertCSV(utShmHead *psShmHead, char *src)
{
    if(strcmp(utComGetVar_sd(psShmHead, "ProductSn", "\0"), "big5") == 0)
    {
        char *str = convert("GBK", "BIG5", src);
        if(str)
        {
            strcpy(src, str);
        }
    }

    return src;
}

int lan_group_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char sql[256] = "";
    pasDbCursor *psCur = NULL;
    int iret = 0;
    int iNum = 0;
    utPltDbHead *psDbHead = utPltInitDb();

    //权限
    char *auth = getDsGroupid();
    if(!utStrIsSpaces(auth))
    {
        snprintf(sql, sizeof(sql), "select gid,gname from nwgroup where gid in (%s) or pid in (%s)", auth, auth);
    }
    else
    {
        strcpy(sql, "select gid,gname from nwgroup");
    }

    printf("lan_group_search:%s\n", sql);
    psCur = pasDbOpenSql(sql, 0);

    if(psCur != NULL)
    {
        unsigned int id = 0;
        char gname[32] = "";

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_ULONG,  4,                     &id,
                                          UT_TYPE_STRING, sizeof(gname) - 1,       gname)) || 1405 == iret)
        {
            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }
            utPltPutLoopVarF(psDbHead, "id", iNum, "%u", id);
            utPltPutLoopVar(psDbHead, "text", iNum, gname);
        }
        pasDbCloseCursor(psCur);
    }

    utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_combo.htm");
    return 0;
}


int lan_employs_search(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utMsgPrintMsg(psMsgHead);

    char node_in[11] = "";
    char sql[512] = "";
    char sqlTmp[512] = "";

    pasDbCursor *psCur = NULL;
    utPltDbHead *psDbHead = utPltInitDb();

    utMsgGetSomeNVar(psMsgHead, 1,
                     "node",        UT_TYPE_STRING, 10, node_in);

    //权限
    char *auth = getDsGroupid();
    if(atoi(node_in) == 0 && !utStrIsSpaces(auth))
    {
        sprintf(sqlTmp, "and gid in (%s) or pid in (%s)", auth);
    }

    sprintf(sql, "select gid,gname,0 from nwgroup where pid = %s %s union all select compid,compname,1 from nwcompute where groupid = %s", node_in, sqlTmp, node_in);

    printf("%s\n", sql);
    psCur = pasDbOpenSql(sql, 0);

    if(NULL != psCur)
    {
        unsigned long id = 0;
        char text[64] = "";
        int leafFlag = 0;
        int iret = 0;
        int iNum = 0;

        while(0 == (iret = pasDbFetchInto(psCur,
                                          UT_TYPE_LONG,   4,                     &id,
                                          UT_TYPE_STRING, sizeof(text) - 1,            text,
                                          UT_TYPE_LONG,   4,                     &leafFlag)) || 1405 == iret)
        {

            iNum++;
            if(iNum > 1)
            {
                utPltPutLoopVar(psDbHead, "dh", iNum, ",");
            }

            utPltPutLoopVarF(psDbHead, "groupid", iNum, "%u", id);
            utPltPutLoopVar(psDbHead, "groupname", iNum, text);
            if(leafFlag)
            {
                utPltPutLoopVar(psDbHead, "leaf", iNum, "true");
            }
            else
            {
                utPltPutLoopVar(psDbHead, "leaf", iNum, "false");
            }
            utPltPutLoopVar(psDbHead, "checked", iNum, "false");
        }

        pasDbCloseCursor(psCur);
    }
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_tree_node.htm");
    return 0;
}

//读取配置文件
//-1: 错误
int readCnfNum(char *filePath, char *label)
{
    FILE *fp = NULL;

    if(NULL == (fp = fopen(filePath, "r")))
    {
        printf("read file failed!\n");
        return -1;
    }
    char buff[256] = "";

    int num = 0;

    while(NULL != fgets(buff, sizeof(buff) - 1, fp))
    {
        if(!strncmp(buff, label, strlen(label)))
        {
            num++;
        }
    }

    fclose(fp);

    return num;
}


void writeLog(char *fileName, char *name, char *content)
{
    char path[128] = "";

    snprintf(path, sizeof(path), "/home/ncmysql/nw/log/%s", fileName);

    FILE *fp = NULL;

    if(NULL == (fp = fopen(path, "a+")))
    {
        return;
    }

    time_t timep;
    time(&timep);

    char strTime[32] = "";

    strcpy(strTime, ctime(&timep));

    strTime[24] = '\0';

    fprintf(fp, "[%s]\t\t%s\t\t%s\n", strTime, name, content);

    fclose(fp);
}

char *findCompNameById(unsigned long compid)
{
    char *compName = (char *)malloc(32);
    memset(compName, 32, 0);
    pasDbCursor *psCur = NULL;

    char sql[128] = "";
    char tmp[16] = "";
    snprintf(sql, sizeof(sql), "select compname from nwcompute where compid = %lu", compid);
    psCur = pasDbOpenSql(sql, 0);

    if(NULL != psCur)
    {
        pasDbFetchInto(psCur, UT_TYPE_STRING, 32, compName);
        pasDbCloseCursor(psCur);
    }
    else
    {
        strcpy(compName, "");

    }

    return compName;
}

char *findGroupNameById(unsigned long groupid)
{
    char *groupName = (char *)malloc(32);
    memset(groupName, 32, 0);
    pasDbCursor *psCur = NULL;

    char sql[128] = "";
    char tmp[16] = "";
    snprintf(sql, sizeof(sql), "select gname from nwgroup where gid = %lu", groupid);
    psCur = pasDbOpenSql(sql, 0);

    if(NULL != psCur)
    {
        pasDbFetchInto(psCur, UT_TYPE_STRING, 32, groupName);
        pasDbCloseCursor(psCur);
    }
    else
    {
        strcpy(groupName, "");
    }

    return groupName;
}
/*
void AdjuestTextBuf(char* text){
    int len=strlen(text);
    if((len<0)||(len>256))return;
    if(text[len-1]>0)return;

    int count=0;
    int i;
    for(i=0;i<len;i++){
        if(text[i]<0)count++;
        if(count==2)count=0;
    }
    if(count==1)text[len-1]=0;

}
*/
void AdjuestTextBuf(char* text)
{
    int len = strlen(text);
    if((len < 0) || (len > 256))return;
    if(text[len - 1] > 0)return;

    int count = 0;
    int i;
    for(i = 0; i < len; i++)
    {
        if(text[i] < 0)
        {
            if(count == 1)
            {
                count = 0;
            }
            else
            {
                count++;
            }
        }
        else
        {
            if(count == 1)count = 0;
        }
    }
    if(count == 1)text[len - 1] = 0;
}

int insertToFile1(char *filePath, unsigned int position, char *str)
{
    FILE *fp = NULL;

    unsigned int fileSize = 0;

    fp = fopen(filePath, "rb+");
    if(NULL == fp)
    {
        printf("Open file failed!\n");
        return -1;
    }

    fseek(fp, position, SEEK_SET);

    fwrite(str, strlen(str), 1, fp) ;

    fclose(fp);

    return 0;
}

unsigned long findCompanyByGroupid(unsigned long groupid)
{
    char sql[64] = "";
    unsigned long pid = 0;

    sprintf(sql, "select pid from nwgroup where gid = %lu", groupid);
    //printf("sql: %s\n",sql);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &pid);

    if(pid != 0)
    {
        groupid = findCompanyByGroupid(pid);
    }

    return groupid;
}

unsigned long findGroupidByCompid(unsigned long compid)
{
    char sql[64] = "";
    unsigned long groupid = 0;

    sprintf(sql, "select groupid from nwcompute where compid = %lu", compid);
    //printf("sql: %s\n",sql);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &groupid);

    return groupid;
}

unsigned long findCompanyByCompid(unsigned long compid)
{
    char sql[64] = "";
    unsigned long groupid = 0;

    sprintf(sql, "select groupid from nwcompute where compid = %lu", compid);
    //printf("sql: %s\n",sql);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &groupid);

    return findCompanyByGroupid(groupid);
}

char *findShortNameById(unsigned long companyid)
{

    char *shortname = malloc(32);
    memset(shortname, 0, sizeof(shortname));
    char sql[128] = "";
    sprintf(sql, "select name from dsuser where groupid=%lu and name like '%%admin%%'", companyid);

    pasDbOneRecord(sql, 0, UT_TYPE_STRING, 31, shortname);

    shortname[strlen(shortname) - 5] = 0;

    return shortname;
}

int findShortName(char *username, char *shortname)
{

    if(username == NULL || shortname == NULL)
    {
        return -1;
    }

    char sql[128] = "";

    sprintf(sql, "select name from dsuser where groupid=(select groupid from dsuser where name='%s') and name like '%%admin%%'", username);

    pasDbOneRecord(sql, 0, UT_TYPE_STRING, 31, shortname);

    shortname[strlen(shortname) - 5] = 0;

    return 0;
}

int findUserLevel(char *username)
{
    if(username == NULL)
    {
        return -1;
    }

    char sql[128] = "";
    int userlevel = 0;

    sprintf(sql, "select usrlevel from dsuser where  name='%s'", username);
    pasDbOneRecord(sql, 0, UT_TYPE_LONG, sizeof(userlevel), &userlevel);

    return userlevel;
}

//查找子部门(或计算机) 并存储
/******************************************************************
* Params: groupid 父部门ID
*         buff    存储查找结果
*         flag    1:不查找计算机   2:一并查找计算机
*         level   部门等级
*         sum     总数
*******************************************************************/
int findChild(unsigned long groupid, char *buff, unsigned int *sum, int flag)
{

    if(!buff)
    {
        return -1;
    }

    char sql[256] = "";

    sprintf(sql, "select gid,gname from nwgroup where pid=%u", groupid);

    unsigned int irecSum = 0;
    char caResult[1024] = "";
    unsigned long groupid_tmp = 0;
    char gname_tmp[32] = "";

    pasDbRecordSet(sql, 1, 9999, &irecSum, caResult, 0);

    char *p = caResult;
    while(p && *p)
    {

        p = pasDbRecordValue(p, 1, 2,
                             UT_TYPE_ULONG, sizeof(groupid_tmp), &groupid_tmp,
                             UT_TYPE_STRING, sizeof(gname_tmp), gname_tmp);
        if(p)
        {
            sprintf(buff + strlen(buff), "id%ld:%u,name%ld:%s,pid%ld:%lu,leaf%ld:0\n", *sum, groupid_tmp, *sum, gname_tmp, *sum, groupid, *sum);
            (*sum)++;

            findChild(groupid_tmp, buff, sum, flag);

        }
    }

    if(flag == 2)
    {
        sprintf(sql, "select compid,compname from nwcompute where groupid = %u", groupid);

        pasDbCursor *psCur = NULL;
        psCur = pasDbOpenSql(sql, 0);

        if(psCur != NULL)
        {
            unsigned long compid_tmp = 0;
            char gname_tmp[32] = "";
            int iret = 0, iNum = 0;

            while(0 == (iret = pasDbFetchInto(psCur,
                                              UT_TYPE_ULONG, sizeof(compid_tmp), &compid_tmp,
                                              UT_TYPE_STRING, sizeof(gname_tmp) - 1, gname_tmp))  || 1405 == iret)
            {
                sprintf(buff + strlen(buff), "id%ld:%u,name%ld:%s,pid%ld:%lu,leaf%ld:1\n", *sum, compid_tmp, *sum, gname_tmp, *sum, groupid, *sum);
                (*sum)++;
            }

            pasDbCloseCursor(psCur);
        }
    }

    return 0;
}


void clearQuotation(char *str)
{
    char *tmp = str + strlen(str) - 1;

    while(tmp != str - 1)
    {
        if(*tmp == '"')
        {
            strcpy(tmp, tmp + 1);
        }
        tmp--;
        //printf("%s\n",str);
    }
    str[strlen(str)] = 0;
}

void clearN(char *str)
{
    char *tmp = str + strlen(str) - 1;

    while(tmp != str - 1)
    {
        if(*tmp == '\n' || *tmp == ' ' || *tmp == '\t')
        {
            strcpy(tmp, tmp + 1);
        }
        tmp--;
        //printf("%s\n",str);
    }
    str[strlen(str)] = 0;
}

void switchToRemainTime(char *des, unsigned long src)
{
    unsigned long res = 0;
    unsigned long mod = 0;

    if(des == NULL)
    {
        return;
    }
    //显示剩余小时数
    res = src / 3600;
    mod = src % 3600;
    if(res > 0)
    {
        sprintf(des + strlen(des), ncLang("0025%02lu时"), res);
    }
    //显示剩余分钟数
    res = mod / 60;
    mod = mod % 60;
    if(res > 0)
    {
        sprintf(des + strlen(des), ncLang("0026%02lu分"), res);
    }
    //显示剩余秒??
    mod = mod % 60;
    sprintf(des + strlen(des), ncLang("0027%02lu秒"), mod);
}

void switchToRemainTime1(char *des, unsigned long src)
{
    unsigned long res = 0;
    unsigned long mod = 0;

    if(des == NULL)
    {
        return;
    }

    //显示剩余小时数
    res = src / 3600;
    mod = src % 3600;
    if(res > 0)
    {
        sprintf(des + strlen(des), "%02lu:", res);
    }
    //显示剩余分钟数
    res = mod / 60;
    mod = mod % 60;
    if(res > 0)
    {
        sprintf(des + strlen(des), "%02lu:", res);
    }
    //显示剩余秒数
    mod = mod % 60;
    sprintf(des + strlen(des), "%02lu", mod);
}

void switchFileSize(char *des, unsigned long src)
{
    unsigned long res = 0;
    unsigned long mod = 0;

    if(des == NULL)
    {
        return;
    }
    if(src == 0)
    {
        sprintf(des, "");
    }
    else if(src > 1024 * 1024 * 102.4)
    {
        sprintf(des, "%.1fG", src * 1.0 / 1024 / 1024 / 1024);
    }
    else if(src > 1024 * 102.4)
    {
        sprintf(des, "%.1fM", src * 1.0 / 1024 / 1024);
    }
    else if(src > 102.4)
    {
        sprintf(des, "%.1fkb", src * 1.0 / 1024);
    }
    else
    {
        sprintf(des, "%luB", src);
    }
}

int GetFileItem(char* filename, char* keybuf, char* outbuf, int maxlen)
{
    //writeLog("filetest.txt","rfilePath",filename);
    FILE* fp = fopen(filename, "rb");
    if(fp == NULL)return -1;
    //writeLog("filetest.txt","rfileOpen","true");
    int filelen = 0;
    fseek(fp, 0, SEEK_END);
    filelen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* pbuffer = (char *)malloc(filelen + 1);
    memset(pbuffer, 0, filelen + 1);
    fread(pbuffer, 1, filelen, fp);

    //writeLog("filetest.txt","rfileContext",pbuffer);
    fclose(fp);
    pbuffer[filelen] = 0;
    char* pkey = strstr(pbuffer, keybuf);
    if(pkey != NULL)
    {
        pkey += strlen(keybuf);
        char* posend = strstr(pkey, "\r\n");
        if(posend == NULL)
            posend = strstr(pkey, "\n");
        if(posend == NULL)
            snprintf(outbuf, maxlen - 1, "%s", pkey);
        else
        {
            int len = posend - pkey;
            if(len > maxlen)
                len = maxlen;
            memcpy(outbuf, pkey, len);
        }
        free(pbuffer);
        return 0;

    }
    return -2;

}
int SetFileItem(char* filename, char* keybuf, char* valbuf)
{

    //writeLog("filetest.txt","valbuf",valbuf);
    //writeLog("filetest.txt","wrfilePath",filename);
    FILE* fp = fopen(filename, "rb");
    if(fp == NULL)return -1;

    //writeLog("filetest.txt","wrfileOpen","true");
    int filelen = 0;
    fseek(fp, 0, SEEK_END);
    filelen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* pbuffer = (char *)malloc(filelen + 1);
    memset(pbuffer, 0, filelen + 1);
    fread(pbuffer, 1, filelen, fp);
    fclose(fp);

    pbuffer[filelen] = 0;
    char* pkey = strstr(pbuffer, keybuf);
    if(pkey != NULL)
    {
        pkey += strlen(keybuf);
        int vallen = strlen(valbuf);
        char* pnewbuf = (char *)malloc(filelen + vallen);
        memset(pnewbuf, 0, filelen + vallen);
        int len = pkey - pbuffer;
        memcpy(pnewbuf, pbuffer, len);

        char* posend = strstr(pkey, "\r\n");
        if(posend == NULL)
            posend = strstr(pkey, "\n");
        if(posend == NULL)
            sprintf(pnewbuf + strlen(pnewbuf), "%s", valbuf);
        else
        {
            sprintf(pnewbuf + strlen(pnewbuf), "%s", valbuf);
            sprintf(pnewbuf + strlen(pnewbuf), "%s", posend);

        }
        fp = fopen(filename, "wb");
        if(fp != NULL)
        {
            //writeLog("filetest.txt","wwfileOpen","true");
            //writeLog("filetest.txt","wwfileContext",pnewbuf);
            fwrite(pnewbuf, 1, strlen(pnewbuf), fp);
            fclose(fp);
        }
        free(pnewbuf);
        free(pbuffer);
        return 0;

    }
    else
    {
        char* pnewbuf = (char *)malloc(filelen + strlen(valbuf));
        memset(pnewbuf, 0, filelen + strlen(valbuf));
        sprintf(pnewbuf, "%s", pbuffer);
        int len = strlen(pnewbuf);
        if(pnewbuf[len - 2] != '\r')
            sprintf(pnewbuf + strlen(pnewbuf), "\r\n%s%s\r\n", keybuf, valbuf);
        else
            sprintf(pnewbuf + strlen(pnewbuf), "%s%s\r\n", keybuf, valbuf);

        fp = fopen(filename, "wb");
        if(fp != NULL)
        {
            fwrite(pnewbuf, 1, strlen(pnewbuf), fp);
            fclose(fp);
        }
        free(pnewbuf);
        free(pbuffer);
        return 0;
    }
    return -2;

}

char *strtoLow(char *src)
{
    char *p = src;

    while(*p != 0)
    {
        if(*p >= 'A' && *p <= 'Z')
        {
            *p = *p + 32;
        }
        p++;
    }

    return src;
}

int isCompany(unsigned long groupid)
{
    unsigned long groupid2 = -1;
    char sql[128] = "";
    sprintf(sql, "select pid from nwgroup where gid=%lu ", groupid);
    printf("====%s\n", sql);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(groupid2), &groupid2);

    printf("groupid%lu,groupid2:%lu\n", groupid, groupid2);

    if(groupid2 == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

