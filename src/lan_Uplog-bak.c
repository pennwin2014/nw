#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>
#include "string.h"
#include "utoall.h"
#include "utomon.h"
#include "ncdef.h"
#include "nwdef.h"
#include "pasdb.h"
#include "lan_md5.h"
#include <stdlib.h>
#include <errno.h>



int8 pasGetLongTimeId();


#include <sys/stat.h>
#include <sys/types.h>

#define    PACKAGE_MAXLEN    50000
#define    JPG_MAXLEN         1024*1024

#define   LOG_DIR                "/home/ncmysql/nw/log/"
#define  SCREENJPG_DIR      "/home/ncmysql/nw/screenJPG/"  //��ĻJPGͼƬ��Ÿ�Ŀ¼
#define    SCREENALTERJPG_DIR "/home/ncmysql/nw/screenAlterJPG/"    //�澯JPGͼƬ��Ÿ�Ŀ¼
#define    PROCESSJPG_DIR     "/home/ncmysql/nw/processJPG/"        //����JPGͼƬ��Ÿ�Ŀ¼
#define    MYTEMP_DIR         "/home/ncmysql/nw/temp/"               //��ʱĿ¼
#define    MYLOG_FILE         "/home/ncmysql/nw/temp/log.dat"       //ϵͳ��־���Ŀ¼
#define    CONF_FILE           "/home/ncmysql/nw/conf/debug.conf"   //�����ļ����Ŀ¼
#define     EMAIL_DIR       "/home/ncmysql/nw/email/"   //Email�ļ�Ŀ¼
#define  SENSITIVEPROCJPG_DIR      "/home/ncmysql/nw/SensitiveProcJPG/"  //��ĻJPGͼƬ��Ÿ�Ŀ¼


char  ConfIP[512];


int    timeToString(unsigned int now, char *year, char *mon, char *day)
{

    if((year == NULL) || (mon == NULL) || (day == NULL) || (now == 0))
    {
        return -1;
    }

    struct tm *time1;
    time1 = (struct tm *)localtime(&now);

    //��ǰ���
    snprintf(year, 5, "%d", time1->tm_year + 1900);

    //��ǰ�·�
    if((time1->tm_mon + 1 > 0) && (time1->tm_mon + 1 <= 9))
        snprintf(mon, 3, "0%d", time1->tm_mon + 1);
    else
        snprintf(mon, 3, "%d", time1->tm_mon + 1);

    //��ǰ��
    if((time1->tm_mday > 0) && (time1->tm_mday <= 9))
        snprintf(day,  3, "0%d", time1->tm_mday);
    else
        snprintf(day,  3, "%d ", time1->tm_mday);

    return 0;
}




int  recodeLogInfo(char * str1, char * str2, char * str3)
{
    int    iRet = -1;
    int   iReturn = 0;
    char  destDir[1024];
    memset(destDir, 0, sizeof(destDir));
    snprintf(destDir, sizeof(destDir) - 1,  "%s", MYTEMP_DIR);
    iReturn = mkdir(destDir, S_IRWXU);

    //system("mkdir -p /home/ncmysql/nw/temp/");


    FILE * fp = fopen(MYLOG_FILE, "a+");
    if(!fp)
    {
        iRet = -1;
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        int filelen = ftell(fp);
        if(filelen > 10 * 1024 * 1024)
        {
            fclose(fp);
            fp = fopen(MYLOG_FILE, "w");
            if(fp == NULL)
                return -1;
        }
        char  year[10];
        char  mon[10];
        char  day[10];
        char  hour[10];
        char  min[10];
        char  sec[10];

        time_t  now;
        struct tm *time1;
        time(&now);
        time1 = localtime(&now);

        //��ǰ���
        snprintf(year, 5, "%d", time1->tm_year + 1900);

        //��ǰ�·�
        if((time1->tm_mon + 1 > 0) && (time1->tm_mon + 1 <= 9))
            snprintf(mon, 3, "0%d", time1->tm_mon + 1);
        else
            snprintf(mon, 3, "%d", time1->tm_mon + 1);


        //��ǰ��
        if((time1->tm_mday > 0) && (time1->tm_mday <= 9))
            snprintf(day,  3, "0%d", time1->tm_mday);
        else
            snprintf(day,  3, "%d", time1->tm_mday);
        //��ǰʱ
        snprintf(hour, 3, "%02d", time1->tm_hour);

        //��ǰ��
        snprintf(min, 3, "%02d", time1->tm_min);

        //��ǰ��
        snprintf(sec, 3, "%02d", time1->tm_sec);

        int pid = getpid();
        int  parentpid = getppid();
        fprintf(fp, "%s-%s-%s %s:%s:%s parentpid:%d pid:%d  ", year, mon, day, hour, min, sec, parentpid , pid);

        char  msg[512];
        memset(msg, 0, sizeof(msg));

        if(str1)
        {
            //snprintf(msg,sizeof(msg)-1,"%s",str1);
            fprintf(fp, "%s ", str1);
        }
        if(str2)
        {
            //snprintf(msg,sizeof(msg)-1,"%s",str2);
            fprintf(fp, "%s ", str2);
        }
        if(str3)
        {
            //snprintf(msg,sizeof(msg)-1,"%s",str3);
            fprintf(fp, "%s ", str3);
        }

        fprintf(fp, "\r\n");
        fclose(fp);
    }

    return iRet;
}

int   CheckIP(int  iFd)
{
    int        iRet = 0;
    unsigned int nIP = 0;
    unsigned int nPort = 0;
    char * currentIP = NULL;

    utComGetSockIp(iFd, &nIP, &nPort);
    currentIP =  utComHostIp(nIP);

    if(!currentIP)
    {
        recodeLogInfo("ip == NULL", "", "");
    }
    else
    {
        if(strlen(ConfIP) == 0)
        {

            iRet = 1;
        }
        else if(!strcmp(currentIP, ConfIP))
        {
            recodeLogInfo("��⵽��", ConfIP, "  ���͵�����");

            iRet = 1;
        }
    }

    return iRet;
}


///////////////////////////////////////////////////////////
//Funtion: ����������־Ŀ¼
//������
//      dir   : ������Ŀ¼·��
//      dirlen: dir �Ŀռ��??����Ϊ200)
//����ֵ��
//       0: �ɹ�
//      -1: ��������
//      -2: ����ʧ��
//////////////////////////////////////////////////////////
int  createDir(char *dir,  int dirlen)
{
    int iRet = 0;
    if((dir == NULL) || (dirlen < 200))
    {
        iRet = -1;
    }
    ///////////////��ѯ��ǰ�ļ�Ӧ�ô�ŵ�·��/////////
    char  year[10];
    char  mon[10];
    char  day[10];

    time_t  now;
    struct tm *time1;
    time(&now);
    time1 = localtime(&now);

    //��ǰ���
    snprintf(year, 5, "%d", time1->tm_year + 1900);

    //��ǰ�·�
    if((time1->tm_mon + 1 > 0) && (time1->tm_mon + 1 <= 9))
        snprintf(mon, 3, "0%d", time1->tm_mon + 1);
    else
        snprintf(mon, 3, "%d", time1->tm_mon + 1);

    //��ǰ��
    if((time1->tm_mday > 0) && (time1->tm_mday <= 9))
        snprintf(day,  3, "0%d", time1->tm_mday);
    else
        snprintf(day,  3, "%d", time1->tm_mday);

    //��ǰ�ļ�����·��
    snprintf(dir, dirlen, "%s%s%s%s", LOG_DIR, year,  mon,  day);

    ///////////////����Ŀ¼/////////////////////////
    iRet = mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO);
    return iRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funtion����ѯ��ǰ����Ӧ�ñ������ĸ��ļ���
//������
//      Userid :  Ҫ��ѯ�Ŀͻ��˵�ID??
//      filetype:   Ҫ��ѯ����־�ļ�����
//      filePath:  ?�ǰ�ļ���ŵ�·�?
//      savefile:  ���ղ鵽�ı������ݵ��ļ���
//      savefilelen:  savefile ?ֶ?Ŀռ䳤�ȣ������?00��
//��?�ֵ�?
//       0: ��ѯ�ɹ�
//      -1: ��������
////////////////////////////////////////////////////////////////
int    searchPos2(int Userid,  char *filetype, char *filePath, char *savefile,  int savefilelen)
{

    int iRet = -1;
    if((filetype == NULL) || (filePath == NULL) || (savefile == NULL) || (savefilelen < 300))
    {
        iRet = -1;
    }
    else
    {
        char    tableName[512];
        char    DefaultPath[512];
        char    comm[512];
        char    year[10];
        char    mon[10];
        char    day[10];

        memset(DefaultPath, 0, sizeof(DefaultPath));
        memset(tableName, 0, sizeof(tableName));
        memset(comm, 0, sizeof(comm));
        memset(year, 0, sizeof(year));
        memset(mon, 0, sizeof(mon));
        memset(day, 0, sizeof(day));


        time_t  now = 0;
        struct tm *time1 = NULL;
        time(&now);
        time1 = localtime(&now);

        //��ǰ���
        snprintf(year, 5, "%d", time1->tm_year + 1900);

        //��ǰ�·�
        if((time1->tm_mon + 1 > 0) && (time1->tm_mon + 1 <= 9))
            snprintf(mon, 3, "0%d", time1->tm_mon + 1);
        else
            snprintf(mon, 3, "%d", time1->tm_mon + 1);

        //��ǰ��
        if((time1->tm_mday > 0) && (time1->tm_mday <= 9))
            snprintf(day,  3, "0%d", time1->tm_mday);
        else
            snprintf(day,  3, "%d", time1->tm_mday);


        //��ǰ��Ϣ��ŵ����ݿ����
        snprintf(tableName, sizeof(tableName) - 1, "%s_%s%s", filetype, year,  mon);

        //Ĭ�ϵ�·��
        snprintf(savefile, savefilelen, "%s/%s_%s999999.dat", filePath, tableName, day);
		
        FILE *fp = fopen(savefile,  "a+");
        if(fp)
        {
            fseek(fp, 0L, SEEK_END);    //��λ���ļ�ĩβ
            long Filelen = ftell(fp);    //��ȡ�ļ���С
            fclose(fp);
            if(Filelen < 10 * 1024 * 1024)
            {
                return 0;
            }


            int num1, num2, num3, num4, num5, num6;
            for(num1 = 0; num1 <= 9;  num1 ++)
                for(num2 = 0; num2 <= 9; num2++)
                    for(num3 = 0; num3 <= 9; num3++)
                        for(num4 = 0; num4 <= 9; num4++)
                            for(num5 = 0; num5 <= 9; num5++)
                                for(num6 = 0; num6 <= 9; num6++)
                                {
                                    snprintf(DefaultPath, sizeof(DefaultPath) - 1, "%s/%s_%s%d%d%d%d%d%d.dat", filePath, tableName, day, num1, num2, num3, num4, num5, num6);
                                    FILE *file = fopen(DefaultPath, "r");
                                    if(file)
                                    {
                                        fclose(file);
                                    }
                                    else
                                    {
                                        snprintf(comm, sizeof(comm) - 1, "mv %s %s", savefile, DefaultPath);
                                        system(comm);
                                        return 0;
                                    }
                                }

        }
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Funtion����ѯ��ǰ����Ӧ�ñ�������?��ļ��?
//������
//      Userid :  Ҫ��ѯ�Ŀͻ��˵�ID��
//      filetype:   Ҫ��?�����־�ļ����?
//      filePath:  ��ǰ�ļ���ŵ�·��
//      savefile:  ���ղ鵽�ı�?����ݵ��ļ��?
//      savefilelen:  savefile �ֶ�?Ŀ?�䳤�ȣ�����?00��
//��?�ֵ�?
//       0: ��ѯ�ɹ�
//      -1: ��������
////////////////////////////////////////////////////////////////
int    searchPos(int Userid,  char *filetype, char *filePath, char *savefile,  int savefilelen)
{
    int iRet = -1;
    if((filetype == NULL) || (filePath == NULL) || (savefile == NULL) || (savefilelen < 300))
    {
        iRet = -1;
    }
    else
    {
        int first,  second,  third;
        for(first = 0; first <= 9; first++)
            for(second = 0; second <= 9 ; second++)
                for(third = 0;  third <= 9;  third++)
                {
                    snprintf(savefile, savefilelen, "%s/%d_%s_%d%d%d.log", filePath, Userid , filetype,  first, second, third);

                    FILE *fp = fopen(savefile, "r");
                    if(fp == NULL)
                    {
                        if((first == 0) && (second == 0)  && (third == 0))
                        {
                            return 0;
                        }
                        else
                        {
                            int number = first * 100 + second * 10 + third - 1;
                            if(number < 0)
                                return -1;
                            else if((number >= 0) && (number <= 9))
                            {
                                snprintf(savefile, savefilelen, "%s/%d_%s_00%d.log", filePath, Userid , filetype,  number);
                            }
                            else if((number >= 10) && (number <= 99))
                            {
                                snprintf(savefile, savefilelen, "%s/%d_%s_0%d.log", filePath, Userid , filetype,  number);
                            }
                            else
                            {
                                snprintf(savefile, savefilelen, "%s/%d_%s_%d.log", filePath, Userid , filetype,  number);
                            }

                            if((fp = fopen(savefile, "r")) != NULL)
                            {
                                fseek(fp, 0L, SEEK_END); //��λ���ļ�ĩβ
                                long Filelen = ftell(fp);    //��ȡ�ļ���С
                                if(Filelen < 100 * 1024 * 1024)
                                    return 0;
                                else
                                {
                                    number += 1;
                                    if(number < 0)
                                        return -1;
                                    else if((number >= 0) && (number <= 9))
                                    {
                                        snprintf(savefile, savefilelen, "%s/%d_%s_00%d.log", filePath, Userid , filetype,  number);
                                    }
                                    else if((number >= 10) && (number <= 99))
                                    {
                                        snprintf(savefile, savefilelen, "%s/%d_%s_0%d.log", filePath, Userid , filetype,  number);
                                    }
                                    else
                                    {
                                        snprintf(savefile, savefilelen, "%s/%d_%s_%d.log", filePath, Userid , filetype,  number);
                                    }
                                    if(fp = fopen(savefile, "w"))
                                    {
                                        fclose(fp);
                                        return 0;
                                    }
                                    else
                                        return -1;
                                }
                            }
                        }
                    }
                    else
                    {
                        fclose(fp);
                        fp = NULL;
                    }
                }//end for()
    }
    return iRet;
}

//��Webaddr����ȡhost�ֶ�
int GetHost(char *Webaddr, char * Host, int  len)
{
    if((Webaddr == NULL) || (Host == NULL) || (len <= 0))
    {
        return -1;
    }

    char    buffer[512];
    memset(buffer, 0, sizeof(buffer));
    char * pos1 = strstr(Webaddr, "//");
    if(pos1 != NULL)
    {
        pos1 += 2;
    }
    else
    {
        pos1 = Webaddr;
    }
    char * pos2 = strstr(pos1, "/");
    if(pos2 == NULL)
    {

        if(strchr(Webaddr, '.') == NULL)
        {
            return -1;
        }
        else
        {
            snprintf(Host, len, "%s", Webaddr);
            return 1;
        }
    }

    if(pos2 - pos1 > 0)
    {
        strncpy(buffer, pos1, pos2 - pos1);
        if(0 == strncmp(buffer, "www.", 4))
        {
            snprintf(Host, len, "%s",  buffer + 4);
        }
        else
        {
            snprintf(Host, len, "%s",  buffer);
        }
        return 1;
    }
    return 0;
}



//1���ļ�������־�ϴ�
int  Lan_FileLog_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    int   Nums   = 0;
    unsigned int   compid = 0;
    unsigned int   userid = 0;
    unsigned int       Blacktype = 0;
    unsigned int       BlackUptype = 0;
    char  mac[20];
    unsigned int   groupid = 0;
    unsigned int   groupid2 = 0;
    char  devname[32];
    char  username[25];
    unsigned int   sip  = 0;
    char  procname[73];
    int   Mark = 0;
    char  Indexid[25];
    unsigned int   Stime = 0;
    char  souPath[300];
    char  destPath[300];
    char  Content[50];

    char  Status = 0;
    int   Nums2  = 0;
    long  iReturn = 0;
    FILE   *fp = NULL;
    char      *data = NULL;

    FILE *ph_fp = NULL;
    char *phdata = NULL;
    char phdir[512];
    char phsavefile[512];

    memset(phdir, 0, sizeof(phdir));
    memset(phsavefile, 0, sizeof(phsavefile));

    char dir[512]; //����Ŀ¼
    char savefile[512];  //������ļ���
    //utMsgPrintMsg(psMsgHead);

    recodeLogInfo(" �����ļ���־1��" , "", "");

    memset(mac, 0, sizeof(mac));
    memset(devname, 0, sizeof(devname));
    memset(username, 0, sizeof(username));
    memset(procname,  0, sizeof(procname));
    memset(Indexid,   0, sizeof(Indexid));
    memset(souPath, 0, sizeof(souPath));
    memset(destPath,   0, sizeof(destPath));
    memset(Content, 0, sizeof(Content));
    memset(dir,  0, sizeof(dir));
    memset(savefile, 0, sizeof(savefile));

    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "compid",   UT_TYPE_ULONG, 4, &compid,
                               "userid",  UT_TYPE_ULONG,  4, &userid,
                               "Nums",    UT_TYPE_LONG,  4, &Nums
                              );
    char  string[2048];
    snprintf(string, sizeof(string) - 1, "compid=%u, userid=%d, Nums=%d", compid, userid, Nums);
    printf("get user log [%s]\n", string);
    recodeLogInfo(" �����ļ���־2��" , string, "");

    if(CheckIP(iFd) != 1)
    {
        Status = 1;
        Nums2 = Nums;

        pasTcpResponse(iFd, psMsgHead, NULL, 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("�ļ���־��" , "���˸���Ϣ��", "");
        return 0;
    }

    if((compid == 0))
    {
        Status = -1;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("�ļ���־��" , "compidΪ0", "");
        return -1;
    }

    //��̬���ٴ洢�ռ�
    data = (char *)malloc(PACKAGE_MAXLEN);
    if(!data)
    {
        Status = -2;
        Nums2 = 0;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("�ļ���־��" , "��̬���ٿռ�ʧ�ܣ�", "");

        return -1;
    }
    else
    {
        memset(data, 0, PACKAGE_MAXLEN);
    }
    phdata = (char *)malloc(PACKAGE_MAXLEN);
    if(!phdata)
        return -1;
    else
    {
        memset(phdata, 0, PACKAGE_MAXLEN);
    }

    int    Numbers = 0;
    char  str[14][51];
    memset(str, 0, 14 * 50);
    //��ѯ��ǰ�ͻ�����Ϣ
    nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);

    char sql[256];
    memset(sql, 0, sizeof(sql));

    sprintf(sql, "select groupid from nwcompute where compid=%lu", compid);

    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(groupid2), &groupid2);
	printf("groupid2=%d\n", groupid2);

    //��ȡ��Ϣ
    for(Numbers = 0; Numbers < Nums; Numbers++)
    {
        snprintf(str[0], 50, "mac%d", Numbers);
        snprintf(str[1], 50, "groupid%d", Numbers);
        snprintf(str[2], 50, "devname%d", Numbers);
        snprintf(str[3], 50, "username%d", Numbers);
        snprintf(str[4], 50, "sip%d", Numbers);
        snprintf(str[5], 50, "procname%d", Numbers);
        snprintf(str[6], 50, "Mark%d", Numbers);
        snprintf(str[7], 50, "Indexid%d", Numbers);
        snprintf(str[8], 50, "Stime%d", Numbers);
        snprintf(str[9], 50, "souPath%d", Numbers);
        snprintf(str[10], 50, "destPath%d", Numbers);
        snprintf(str[11], 50, "Content%d", Numbers);

        iReturn = utMsgGetSomeNVar(psMsgHead, 12,
                                   str[0],  UT_TYPE_STRING, sizeof(mac) - 1, mac,
                                   str[1],  UT_TYPE_LONG,   4,  &groupid,
                                   str[2], UT_TYPE_STRING,  sizeof(devname) - 1,  devname,
                                   str[3], UT_TYPE_STRING, sizeof(username) - 1, username,
                                   str[4], UT_TYPE_LONG,   4,  &sip,
                                   str[5], UT_TYPE_STRING, sizeof(procname) - 1, procname,
                                   str[6], UT_TYPE_LONG,   4,  &Mark,
                                   str[7], UT_TYPE_STRING, sizeof(Indexid) - 1, Indexid,
                                   str[8], UT_TYPE_LONG,   4,  &Stime,
                                   str[9], UT_TYPE_STRING, sizeof(souPath) - 1, souPath,
                                   str[10], UT_TYPE_STRING, sizeof(destPath) - 1, destPath,
                                   str[11], UT_TYPE_STRING, sizeof(Content) - 1, Content
                                  );


        if((strlen(mac) <= 0) &&
           (groupid == 0) &&
           (strlen(devname) <= 0) &&
           (strlen(username) <= 0) &&
           (sip == 0) &&
           (strlen(procname) <= 0) &&
           (Mark == 0) &&
           (strlen(Indexid)) &&
           (Stime == 0) &&
           (strlen(souPath) <= 0) &&
           (strlen(destPath) <= 0) &&
           (strlen(Content) <= 0))
        {
            recodeLogInfo("�յ�һ�����ļ���־", "", "");
            continue;
            Nums2++;
        }


        char mystring[512];
        memset(mystring, 0, sizeof(mystring));
        snprintf(mystring, sizeof(mystring) - 1, " compid=%u, stime=%u", compid, Stime);

        if(sip == 0)
        {
            recodeLogInfo(" �յ�һ���ļ���־:", mystring, "sip == 0");
        }
        if(Stime == 0)
        {
            recodeLogInfo(" �յ�һ���ļ���־:", mystring,  "Stime == 0");
        }
        if(strlen(Indexid) <= 0)
        {
            recodeLogInfo(" �յ�һ���ļ���־:", mystring, "Indexid == NULL");
        }
        if(strlen(souPath) <= 0)
        {
            recodeLogInfo(" �յ�һ���ļ���־:", mystring, "souPath == NULL");
        }


        //�������ݵ��ļ���
        unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
        if(psComp != NULL)
        {
            userid = psComp->userid;
            groupid = psComp->groupid;
        }

        //groupid = groupid2;

        int i ;
        for(i = 0; i < strlen(souPath);   i++)
        {
            if(souPath[i] == '\\')
                souPath[i]  = '/';
        }

        for(i = 0; i < strlen(destPath);   i++)
        {
            if(destPath[i] == '\\')
                destPath[i]  = '/';
        }
        if(Mark >= 0x3001 && Mark <= 0x300a)
        {


            //destPath  Blacktype:BlackUptype %d:%d
            Blacktype = atoi(destPath);
            char* p = strchr(destPath , ':');
            unsigned int filesize = 0;
            if(p != NULL)
            {
                BlackUptype = atoi(p + 1);
                p = strchr(p + 1, ':');
                if(p != NULL)
                    filesize = atoi(p + 1);
            }
            if(strlen(Indexid) <= 0)
                snprintf(Indexid, sizeof(Indexid) - 1, "%s", "undefined");

            snprintf(phdata + strlen(phdata), PACKAGE_MAXLEN - strlen(phdata) - 1, "%llu\t%u\t%u\t%s\t%u\t"
                     "%s\t%s\t%u\t%s\t"
                     "%d\t%s\t%u\t%s\t"
                     "%s\t%s\t%d\t%d\t%d\n",
                     currentTime, compid, userid, mac, groupid,
                     devname, username, sip, procname,
                     Mark, Indexid, Stime, souPath,
                     destPath, Content, Blacktype, BlackUptype, filesize);
            char dddd[512];
            memset(dddd, 0, sizeof(dddd));
            snprintf(dddd, sizeof(dddd) - 1, "compid:%u,mac:%s,groupid:%ld,devname:%s,username:%s,sip:%u,procname:%s,Mark:%ld,Indexid%s,Stime:%ld,souPath:%s,destPath:%s,Content:%s",
                     compid, mac, groupid, devname, username, sip, procname, Mark, Indexid, Stime, souPath, destPath, Content);
            recodeLogInfo("�ļ���־", "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", dddd);
            recodeLogInfo("�ļ���־", "1111111111111111111111111111111111111111111", phdata);
        }
        else
        {
            snprintf(data + strlen(data), PACKAGE_MAXLEN - strlen(data) - 1,  "%llu\t%u\t%u\t%s\t"
                     "%u\t%s\t%s\t%u\t"
                     "%s\t%d\t%s\t%u\t"
                     "%s\t%s\t%s\n",
                     currentTime, compid,    userid,       mac,
                     groupid,       devname,  username,  sip,
                     procname,   Mark,        Indexid,     Stime,
                     souPath,     destPath,   Content\
                    );
        }
        Nums2++;
    }//end for()

    //����¼��Nums2��ֵ�Ƿ��Nums���
    char  mesg[512];
    memset(mesg, 0, sizeof(mesg));
    snprintf(mesg, sizeof(mesg) - 1, "compid=%u, userid=%d, Nums=%d, success=%d", compid, userid, Nums, Nums2);
    if(Nums != Nums2)
    {
        Status = -3;
        Nums2 = 0;
        recodeLogInfo("�ļ���־��Error(Nums != success):", mesg, "");
		printf("�ļ���־��Error(Nums != success)");
    }
    else
    {
        //����������־Ŀ¼,��������?�Ŀ¼��ŵ�dir�У��Ա����ʹ�ø�Ŀ¼
        createDir(phdir,  sizeof(phdir));
		printf("phdir=%s\n", phdir);
        if(searchPos2(userid, "nwoutfilelog", phdir, phsavefile, sizeof(phdir)))
        {
            Status = -4;
            Nums2 = 0;
            recodeLogInfo("�ļ���־��Error:", "���Ҵ洢�ļ�ʧ�ܣ�", "");
        }
        else
        {
            snprintf(mesg, sizeof(mesg) - 1, "dir=%s$$$$$$$$$$$$$$$$$$$$savefile=%s", phdir, phsavefile);
            //recodeLogInfo("�ļ���־s", mesg,"");
            if((ph_fp = fopen(phsavefile, "ab")) == NULL)    //�򿪴洢�ļ�ʧ��
            {

            }
            else
            {
                pasFileSetFileLock(ph_fp);

                fwrite(phdata, 1, strlen(phdata) , ph_fp);

                //���ļ�����
                pasFileSetFileUnLock(ph_fp);

                fclose(ph_fp);
                ph_fp = NULL;

                Status = 1;

            }
        }
        createDir(dir,  sizeof(dir));
        //��ѯ��ǰ����Ӧ�ñ������ĸ��ļ��У� �����ѯ�ɹ�����򿪴��ļ�,ʧ���򷵻ش���
        if(searchPos2(userid, "nwfilelog", dir, savefile, sizeof(dir)))
        {
            Status = -4;
            Nums2 = 0;
			printf("search pos2 failed");
		}
        else
        {

            snprintf(mesg, sizeof(mesg) - 1, "dir=%s***********savefile=%s", dir, savefile);
			printf("�ļ���־=%s\n", mesg);
            recodeLogInfo("�ļ���־s", mesg, "");
            if((fp = fopen(savefile, "ab")) == NULL)    //�򿪴洢�ļ�ʧ��
            {
                Status  = -6;
                Nums2 = 0;

                time_t now;
                time(&now);
                char sql[2048];
                memset(sql, 0, sizeof(sql));
                snprintf(sql, sizeof(sql) - 1,  "insert into nwLogDebug(compid,stime, type, error, Info) values(%u,%u, \'%s\',%d,\'%s\')", compid, now, "�ļ��洢", errno, savefile);
                iReturn = pasDbExecSql(sql, 0);
                printf("�������ݿ�һ����Ϣ................iReturn=%d\n",  iReturn);
                recodeLogInfo("�ļ���־��Error:", "�򿪴洢�ļ�ʧ�ܣ�", savefile);
            }
            else    //���ļ��ɹ����򱣴����ݵ��ļ���
            {
                //���ļ�����
                pasFileSetFileLock(fp);
                fwrite(data, 1, strlen(data) ,  fp);
                //���ļ�����
                pasFileSetFileUnLock(fp);
                fclose(fp);
                fp = NULL;

                Status = 1;
				printf("����ɹ�--%s--len=%d\n", savefile, strlen(data));
            }
			/*
			//�ٱ���һ��
			memset(savefile, 0, sizeof(savefile));
			snprintf(savefile, sizeof(savefile)-1, "/home/ncmysql/nw/log_penn/log.dat");
			if((fp = fopen(savefile, "ab")) == NULL)    //�򿪴洢�ļ�ʧ��
            {
                Status  = -6;
                Nums2 = 0;

                time_t now;
                time(&now);
                char sql[2048];
                memset(sql, 0, sizeof(sql));
                snprintf(sql, sizeof(sql) - 1,  "insert into nwLogDebug(compid,stime, type, error, Info) values(%u,%u, \'%s\',%d,\'%s\')", compid, now, "�ļ��洢", errno, savefile);
                iReturn = pasDbExecSql(sql, 0);
                printf("1�������ݿ�һ����Ϣ................iReturn=%d\n",  iReturn);
                recodeLogInfo("1�ļ���־��Error:", "�򿪴洢�ļ�ʧ�ܣ�", savefile);
            }
            else    //���ļ��ɹ����򱣴����ݵ��ļ���
            {
                //���ļ�����
                pasFileSetFileLock(fp);
                fwrite(data, 1, strlen(data) ,  fp);
                //���ļ�����
                pasFileSetFileUnLock(fp);
                fclose(fp);
                fp = NULL;

                Status = 1;
				printf("111����ɹ�--%s--len=%d\n", savefile, strlen(data));
            }
            */
        }
        recodeLogInfo(" �����ļ���־3��" , mesg, "");
    }

    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                   "compid",   UT_TYPE_LONG,    compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Nums",     UT_TYPE_LONG,    Nums2);
    //printf("------------------------------------receive file log, compid = %u, Status = %d,Num=%d, Nums2 = %d\n", compid, Status,  Nums, Nums2);

    free(data);
    free(phdata);
    return 0;
}


int  timeToStringEx(unsigned int now, char *year, char *mon)
{    
	if((year == NULL) || (mon == NULL) || (now == 0))    
	{        
		return -1;    
	}    
	struct tm *time1;    
	time1 = (struct tm *)localtime(&now);    
	//��  
	snprintf(year, 5, "%d", time1->tm_year + 1900);    
	//��   
	if((time1->tm_mon + 1 > 0) && (time1->tm_mon + 1 <= 9))
		snprintf(mon, 3, "0%d", time1->tm_mon + 1);    
	else        
		snprintf(mon, 3, "%d", time1->tm_mon + 1);      
	return 0;
}

//2��������־�ϴ�
int  Lan_PorcessLog_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    int   Nums   = 0;
    unsigned int   compid = 0;
    unsigned int   userid = 0;
    char  mac[20];
    unsigned int   groupid = 0;
    unsigned int    groupid2 = 0;
    char  devname[32];
    char  username[32];
    unsigned int   sip  = 0;
    char  procname[73];
    int    screenid = 0;

    char  Mark = 0;
    int   Pid  = 0;
    char  Indexid[25];
    char  Pindexid[25];
    unsigned int   Stime = 0;
    unsigned int   Btime = 0;
    int             nettime = 0;
    int             wndtime = 0;
    char  Path[257];
    unsigned int      mem = 0 ;
    unsigned int      cpu = 0;
    int      flags = 0;

    char  Status = '\0';
    int   Nums2 = 0;
    long  iReturn = 0;
    FILE   *fp = NULL;
    //char    *  data = NULL;

    char dir[300]; //���Ŀ¼
    char savefile[300];  //������ļ���
    char    mesg[512];
	//���ݿ����
    char sql_tmp[2048] = "";

    //utMsgPrintMsg(psMsgHead);
    recodeLogInfo(" ���ս�����־1��" , "", "");

    memset(mac, 0, sizeof(mac));
    memset(devname, 0, sizeof(devname));
    memset(username, 0, sizeof(username));
    memset(procname,  0, sizeof(procname));
    memset(Indexid, 0, sizeof(Indexid));
    memset(Pindexid, 0, sizeof(Pindexid));
    memset(Path, 0, sizeof(Path));
    memset(dir,  0, sizeof(dir));
    memset(savefile, 0, sizeof(savefile));
    memset(mesg, 0, sizeof(mesg));

    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "compid",  UT_TYPE_ULONG, 4, &compid,
                               "userid",  UT_TYPE_ULONG, 4, &userid,
                               "Nums",    UT_TYPE_LONG, 4, &Nums
                              );

    char  string[2048];
    snprintf(string, sizeof(string) - 1, "compid=%u, userid=%d, Nums=%d", compid, userid, Nums);
    recodeLogInfo(" ���ս�����־2��" , string, "");


    if(CheckIP(iFd) != 1)
    {
        Status = 1;
        Nums2 = 0;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("������־��", "���˸���Ϣ", "");
        return 0;
    }

    if((compid == 0))
    {
        Status = -3;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("������־��", "compidΪ��", "");
        return -1;

    }
/*
    //��̬���ٴ洢�ռ�
    data = (char *)malloc(PACKAGE_MAXLEN);
    if(!data)
    {
        Status = -4;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL,  3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("������־��" , "��̬���ٿռ�ʧ�ܣ�", "");
        return -1;
    }
    else
    {
        memset(data, 0, PACKAGE_MAXLEN);
    }
*/

    int   Numbers = 0;
    char  str[20][50];
    char  conver[50];
    memset(str, 0, 20 * 50);
    memset(conver, 0, sizeof(conver));

    //��ѯ��ǰ�ͻ�����Ϣ
    nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);

    char sql[256];
    memset(sql, 0, sizeof(sql));

    sprintf(sql, "select groupid from nwcompute where compid=%lu", compid);

    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(groupid2), &groupid2);

    long lCount = 0;
    for(Numbers = 0; Numbers < Nums; Numbers++)
    {
        snprintf(str[0], 50, "mac%d", Numbers);
        snprintf(str[1], 50, "groupid%d", Numbers);
        snprintf(str[2], 50, "devname%d", Numbers);
        snprintf(str[3], 50, "username%d", Numbers);
        snprintf(str[4], 50, "sip%d", Numbers);
        snprintf(str[5], 50, "procname%d", Numbers);
        snprintf(str[6], 50, "screenid", Numbers);
        snprintf(str[7], 50, "Mark%d", Numbers);
        snprintf(str[8], 50, "Pid%d", Numbers);
        snprintf(str[9], 50, "Indexid%d", Numbers);
        snprintf(str[10], 50, "Pindexid%d", Numbers);
        snprintf(str[11], 50, "Stime%d", Numbers);
        snprintf(str[12], 50, "Btime%d", Numbers);
        snprintf(str[13], 50, "Path%d", Numbers);
        snprintf(str[14], 50, "mem%d", Numbers);
        snprintf(str[15], 50, "cpu%d", Numbers);
        snprintf(str[16], 50, "flags%d", Numbers);
        snprintf(str[17], 50, "nettime%d", Numbers);
        snprintf(str[18], 50, "wndtime%d", Numbers);

        iReturn = utMsgGetSomeNVar(psMsgHead, 19,
                                   str[0],  UT_TYPE_STRING, sizeof(mac) - 1, mac,
                                   str[1],  UT_TYPE_LONG,   4,  &groupid,
                                   str[2], UT_TYPE_STRING,  sizeof(devname) - 1,  devname,
                                   str[3], UT_TYPE_STRING, sizeof(username) - 1, username,
                                   str[4], UT_TYPE_LONG,   4,  &sip,
                                   str[5], UT_TYPE_STRING, sizeof(procname) - 1, procname,
                                   str[6], UT_TYPE_LONG, 4, &screenid,
                                   str[7], UT_TYPE_STRUCT,   1, &Mark,
                                   str[8], UT_TYPE_LONG,   4,  &Pid,
                                   str[9], UT_TYPE_STRING, sizeof(Indexid) - 1, Indexid,
                                   str[10], UT_TYPE_STRING, sizeof(Pindexid) - 1, Pindexid,
                                   str[11], UT_TYPE_LONG,   4,  &Stime,
                                   str[12], UT_TYPE_LONG,   4,  &Btime,
                                   str[13], UT_TYPE_STRING, sizeof(Path) - 1, Path,
                                   str[14], UT_TYPE_LONG, 4, &mem,
                                   str[15], UT_TYPE_LONG, 4, &cpu,
                                   str[16], UT_TYPE_LONG, 4, &flags,
                                   str[17], UT_TYPE_LONG, 4, &nettime,
                                   str[18], UT_TYPE_LONG, 4, &wndtime
                                  );
        if((strlen(mac) <= 0) &&
           (groupid == 0) &&
           (strlen(devname) <= 0) &&
           (strlen(username) <= 0) &&
           (sip == 0) &&
           (strlen(procname) <= 0) &&
           (Mark == 0) &&
           (Pid == 0) &&
           (strlen(Indexid)) &&
           (strlen(Pindexid)) &&
           (Stime == 0) &&
           (Btime == 0) &&
           (strlen(Path) <= 0) &&
           (mem == 0) &&
           (cpu == 0) &&
           (flags == 0))
        {
            recodeLogInfo("�յ�һ���ս�����־", "", "");
            Nums2++;
            continue;
        }


        char mystring[512];
        memset(mystring, 0, sizeof(mystring));
        snprintf(mystring, sizeof(mystring) - 1, " compid=%u, stime=%u", compid, Stime);

        if(sip == 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "sip == 0");
        }
        if(Stime == 0)
        {
            recodeLogInfo(" �յ�һ��������־:",  mystring, "Stime == 0");
        }
        if(Btime == 0)
        {
            recodeLogInfo(" �յ�һ��������־:",  mystring, "Btime == 0");
        }
        if(strlen(Indexid) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:",  mystring, "Indexid == NULL");
        }
        if(strlen(Pindexid) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:",  mystring, "Pindexid == NULL");
        }
        if(strlen(Path) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:",  mystring, "Path == NULL");
        }

        snprintf(mesg, sizeof(mesg) - 1,  "compid:%u, stime:%u, procname: %s,  Path:%s",  compid,  stime,  procname,  Path);
        recodeLogInfo(" �յ�һ��������־:", "-----------------", mesg);


        //�������ݵ��ļ���
        //unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
        if(psComp != NULL)
        {
            userid = psComp->userid;
            groupid = psComp->groupid;
        }
        //groupid = groupid2;

        int i;
        for(i = 0; i < strlen(Path);   i++)
        {
            if(Path[i] == '\\')
                Path[i]  = '/';

        }

        if(screenid == -1)
        {
            screenid = 0;
        }
/*
        snprintf(data + strlen(data), PACKAGE_MAXLEN - strlen(data) - 1,   "%llu\t%u\t%u\t%s\t"
                 "%u\t%s\t%s\t%u\t"
                 "%s\t%d\t%d\t%d\t%s\t"
                 "%s\t%u\t%u\t%s\t"
                 "%d\t%d\t%d\t%d\t%d\n",
                 currentTime, compid,      userid ,    mac,
                 groupid,     devname,     username,   sip,
                 procname,    screenid,   Mark,       Pid,     Indexid,
                 Pindexid,    Stime,       Btime,      Path,
                 mem,         cpu,         flags,      nettime,   wndtime
                );
*/
		//ֱ�Ӳ��뵽���ݿ�
	    //1�����ݵ�ǰ�������ɱ���
	    char table_name[1024] = "";
		char  year[10];   
		char  mon[10];   
		memset(year, 0, sizeof(year));   
		memset(mon, 0 , sizeof(mon));
		time_t now;            
		time(&now);            
		timeToStringEx(now, year, mon);
		int syear = atoi(year);
    	int smonth = atoi(mon);
		snprintf(table_name + strlen(table_name), 1024 - strlen(table_name), "nwproclog_%4u%02u", syear, smonth);
		//2����ѯ�Ƿ��Ѿ����ڣ�����Ѿ���������£�����Ͳ���
		memset(sql_tmp, 0, 2048);		
        snprintf(sql_tmp, sizeof(sql_tmp) - 1, "select count(*) from %s  where indexid=\'%s\'",   table_name, Indexid);
        lCount = 0;		
		pasDbOneRecord(sql_tmp, 0, UT_TYPE_ULONG, 4, &lCount);
		//printf("sql=[%s],lcount=%d\n", sql_tmp, lCount);
		memset(sql_tmp, 0, 2048);
		if(lCount >0){
			snprintf(sql_tmp, sizeof(sql_tmp)-1, "update %s  set btime=%u,wndtime=%d where indexid=\'%s\'", table_name,Btime,wndtime, Indexid);
			//printf("update sql=[%s]\n", sql_tmp);
			if(pasDbExecSql(sql_tmp, 0)){
				recodeLogInfo(" ���½�����־", " �������ݿ�ʧ��", sql_tmp );
				break;
			}
		}else{
			snprintf(sql_tmp, sizeof(sql_tmp)-1, "insert into %s(compid, userid,  mac, groupid, devname, udisp, sip, procname, screenid, mark, pid, indexid, pindexid, stime, btime, path, mem, cpu, flags, nettime, wndtime) values(%u,%u,\'%s\',%u,\'%s\',\'%s\',%u,\'%s\',%d,%d,%d,\'%s\',\'%s\',%u,%u,\'%s\',%d,%d,%d,%d,%d) ", table_name, compid, userid, mac, groupid,  devname,  username, sip,
                 procname,    screenid,   Mark,       Pid,     Indexid,
                 Pindexid,    Stime,       Btime,      Path,
                 mem,         cpu,         flags,      nettime,   wndtime);
			//printf("insert sql=[%s]\n", sql_tmp);
			if(pasDbExecSql(sql_tmp, 0)){
				recodeLogInfo(" ���������־", "�������ݿ�ʧ�ܣ� " , sql_tmp);
				break;
			}
		}		
	    Nums2++;
    }// end for()


    //����¼��Nums2��ֵ�Ƿ��Nums���
    memset(mesg, 0, sizeof(mesg));
    snprintf(mesg, sizeof(mesg) - 1, "compid=%u, userid=%d, Nums=%d, success=%d", compid, userid, Nums, Nums2);
    if(Nums != Nums2)
    {
        Status = -3;
        Nums2 = 0;
        recodeLogInfo("������־��Error(Nums != success):", mesg, "");

    }
    else
    {
    	/*
        //����������־Ŀ¼,��������?�Ŀ¼��ŵ�dir�У��Ա����ʹ�ø�Ŀ¼
        createDir(dir,  sizeof(dir));

        //��ѯ��ǰ����Ӧ�ñ������ĸ��ļ��У� �����ѯ�ɹ�����򿪴��ļ�,ʧ���򷵻ش���
        if(searchPos2(userid, "nwproclog", dir, savefile, sizeof(dir)))
        {
            Status = -4;
            Nums2 = 0;
            recodeLogInfo("������־��Error:", "���Ҵ洢�ļ�ʧ�ܣ�", "");
        }
        else
        {
            if((fp = fopen(savefile, "a+")) == NULL)    //�򿪴洢�ļ�ʧ��
            {
                Status  = -5;
                Nums2 = 0;
                recodeLogInfo("������־��Error:", "�򿪴洢�ļ�ʧ�ܣ�", savefile);
            }
            else    //���ļ��ɹ����򱣴����ݵ��ļ���
            {
                //���ļ�����
                pasFileSetFileLock(fp);

                fwrite(data, 1, strlen(data) ,  fp);

                //���ļ�����
                pasFileSetFileUnLock(fp);

                fclose(fp);
                fp = NULL;

                Status = 1;

            }
        }
		*/
        recodeLogInfo(" ���ս�����־3��" , mesg, "");
    }


    pasTcpResponse(iFd, psMsgHead, NULL, /* ?�Կ���?����? */ 3,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Nums",     UT_TYPE_LONG,     Nums2);
   // free(data);
    return 0;
}

//3��������־�ϴ�
int  Lan_RmoveLog_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int   Nums   = 0;
    unsigned int   compid = 0;
    unsigned int   userid = 0;
    char  mac[50];
    unsigned int   groupid = 0;
    unsigned int    groupid2 = 0;
    char  devname[50];
    char  username[50];
    unsigned int   sip  = 0;
    char  procname[100];

    char  Mark = '\0';
    unsigned int   Stime = 0;
    char  Valuename[128];
    char  Produname[128];
    char  Hardname[128];
    char  Driversn[24];

    char  Status = 0;
    int   Nums2 = 0;
    long  iReturn = 0;
    FILE   *fp = NULL;
    char     *data = NULL;

    char dir[512]; //����Ŀ¼
    char savefile[512];  //������ļ���
    utMsgPrintMsg(psMsgHead);

    char         sql[2048];
    char            mesg[512];
    pasDbCursor *psCur = NULL;
    memset(sql, 0, sizeof(sql));
    memset(mesg, 0, sizeof(mesg));


    memset(mac, 0, sizeof(mac));
    memset(devname, 0, sizeof(devname));
    memset(username, 0, sizeof(username));
    memset(procname, 0, sizeof(procname));
    memset(Valuename, 0, sizeof(Valuename));
    memset(Produname, 0, sizeof(Produname));
    memset(Hardname, 0, sizeof(Hardname));
    memset(Driversn,  0, sizeof(Driversn));

    memset(dir,  0, sizeof(dir));
    memset(savefile, 0, sizeof(savefile));



    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "compid",  UT_TYPE_ULONG,  4, &compid,
                               "userid",  UT_TYPE_ULONG,  4, &userid,
                               "Nums",    UT_TYPE_LONG,  4, &Nums
                              );

    if(CheckIP(iFd) != 1)
    {
        Status = 1;
        Nums2 = Nums;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("������־��", "���˸���Ϣ", "");
        return 0;
    }


    if((compid == 0))
    {
        Status = -1;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("������־��", "compidΪ��", "");
        return -1;
    }

    //��̬���ٴ洢�ռ�
    data = (char *)malloc(PACKAGE_MAXLEN);
    if(!data)
    {
        Status = -2;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        //recodeLogInfo("������־��" , """);
        return -1;
    }
    else
    {
        memset(data, 0, PACKAGE_MAXLEN);
    }

    //��ȡ��Ϣ
    int   Numbers = 0;
    char  str[12][50];
    memset(str, 0, 12 * 50);

    // �ѯ��ǰ�ͻ������?
    nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
    char Mysql[256];
    memset(Mysql, 0, sizeof(Mysql));

    sprintf(Mysql, "select groupid from nwcompute where compid=%lu", compid);

    pasDbOneRecord(Mysql, 0, UT_TYPE_ULONG, sizeof(groupid2), &groupid2);


    for(Numbers = 0; Numbers < Nums; Numbers++)
    {
        snprintf(str[0], 50, "mac%d", Numbers);
        snprintf(str[1], 50, "groupid%d", Numbers);
        snprintf(str[2], 50, "devname%d", Numbers);
        snprintf(str[3], 50, "username%d", Numbers);
        snprintf(str[4], 50, "sip%d", Numbers);
        snprintf(str[5], 50, "procname%d", Numbers);
        snprintf(str[6], 50, "Mark%d", Numbers);
        snprintf(str[7], 50, "Stime%d", Numbers);
        snprintf(str[8], 50, "Valuename%d", Numbers);
        snprintf(str[9], 50, "Produname%d", Numbers);
        snprintf(str[10], 50, "Hardname%d", Numbers);
        snprintf(str[11], 50, "Driversn%d", Numbers);


        iReturn = utMsgGetSomeNVar(psMsgHead, 12,
                                   str[0],  UT_TYPE_STRING, sizeof(mac) - 1, mac,
                                   str[1],  UT_TYPE_LONG,   4,  &groupid,
                                   str[2], UT_TYPE_STRING,  sizeof(devname) - 1,  devname,
                                   str[3], UT_TYPE_STRING, sizeof(username) - 1, username,
                                   str[4], UT_TYPE_LONG,   4,  &sip,
                                   str[5], UT_TYPE_STRING, sizeof(procname) - 1, procname,
                                   str[6], UT_TYPE_STRUCT,   1, &Mark,
                                   str[7], UT_TYPE_LONG,   4,  &Stime,
                                   str[8],  UT_TYPE_STRING, sizeof(Valuename) - 1, Valuename,
                                   str[9],  UT_TYPE_STRING, sizeof(Produname) - 1, Produname,
                                   str[10],  UT_TYPE_STRING, sizeof(Hardname) - 1, Hardname,
                                   str[11],  UT_TYPE_STRING, sizeof(Driversn) - 1,  Driversn
                                  );
        if((strlen(mac) <= 0) &&
           (groupid == 0) &&
           (strlen(devname) <= 0) &&
           (strlen(username) <= 0) &&
           (sip == 0) &&
           (strlen(procname) <= 0) &&
           (Mark == 0) &&
           (Stime == 0) &&
           (strlen(Valuename) <= 0) &&
           (strlen(Produname) <= 0) &&
           (strlen(Hardname) <= 0) &&
           (strlen(Driversn) <= 0))
        {
            recodeLogInfo(" �յ�һ����������־", "", "");
            Nums2++;
            continue;
        }

        //������־��Ϣ
        char mystring[512];
        memset(mystring, 0, sizeof(mystring));
        snprintf(mystring, sizeof(mystring) - 1, " compid=%u, stime=%u", compid, Stime);

        if(sip == 0)
        {
            recodeLogInfo(" �յ�һ������?��?", mystring, "ip = 0");
        }
        if(Stime == 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Stime == 0");
        }
        if(strlen(Valuename) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Valuename== NULL");
        }
        if(strlen(Produname) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Produname== NULL");
        }
        if(strlen(Hardname) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Hardname== NULL");
        }
        if(strlen(Driversn) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Driversn== NULL");
        }

        //�������ݵ��ļ���
        unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
        if(psComp != NULL)
        {
            userid = psComp->userid;
            groupid = psComp->groupid;
        }
        //groupid = groupid2;

        snprintf(data + strlen(data), PACKAGE_MAXLEN - strlen(data) - 1, "%llu\t%u\t%u\t%s\t"
                 "%u\t%s\t%s\t%u\t"
                 "%s\t%d\t%u\t%s\t"
                 "%s\t%s\t%s\n",
                 currentTime,    compid,      userid ,        mac,
                 groupid,          devname,    username,    sip,
                 procname,      Mark,          Stime,         Valuename,
                 Produname,    Hardname,  Driversn);

        Nums2++;


        //      if(Mark == 0){//������Ϣ
        //          recodeLogInfo(" �յ�һ������������־.....","", "");
        //      }else if(Mark == 100){//USBʵʱ��Ϣ
        //������Ϣ���µ�nwusblist����
        if(strlen(Hardname) > 0)
        {
            char  string[512];
            memset(string, 0, sizeof(string));

            //���ҵ�ǰ��������.�˾�?
            char *company = findShortNameById(findCompanyByCompid(compid));
            if(NULL == company)
            {
                snprintf(mesg, sizeof(mesg) - 1, "  ����compid���ڹ�˾ʧ��!  compid:%u",  compid);
                recodeLogInfo("" , "" , mesg);
                continue;
            }

            snprintf(mesg, sizeof(mesg) - 1, "  ����compid���ڹ�˾!  compid:%u, company:%s",  compid, company);
            recodeLogInfo("" , "" , mesg);
            snprintf(sql, sizeof(sql) - 1, "select count(*) from nwusblist  where hardid=\'%s\'and createuser=\'%s\'",   Hardname, company);
            long lCount = 0;
            pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &lCount);
            if(lCount > 0)
            {
                snprintf(sql , sizeof(sql) - 1, " update  nwusblist  set  name=\'%s\',  produname=\'%s\',  volname=\'%s\',  sip=%u, compid=%u,  modtime=%u ,createuser=\'%s\' where hardid=\'%s\' and compid=%u ; ",
                         Valuename,  Produname, Driversn, sip, compid, Stime,  company,  Hardname, compid);

                if(pasDbExecSql(sql, 0) != 0)
                {
                    snprintf(string, sizeof(string) - 1, "sizeof(sql)= %d, hardid=%s, stime=%u, id=%u ,Do SQL Error, sqlcode is  %d ", sizeof(sql), Hardname, Stime, compid,  pasDbSqlCode(psCur));
                    recodeLogInfo("��nwusblist���ݱ��и�������ʧ�ܣ�" , string , sql);
                }
                recodeLogInfo("8888888888888888888888888888888888888888888888888888" , string , sql);

            }
            else
            {
                snprintf(sql, sizeof(sql) - 1, "insert into nwusblist(name, produname, volname, hardid, sip, compid, stime ,modtime, createuser)  values(\'%s\', \'%s\', \'%s\', \'%s\',  %u, %u, %u,%u,  \'%s\')",
                         Valuename,  Produname,  Driversn, Hardname, sip, compid, Stime,  Stime,  company);

                if(pasDbExecSql(sql, 0))
                {
                    snprintf(string, sizeof(string) - 1, "hardid=%s, stime=%u, compid=%u ,Do SQL Error, sqlcode is  %d ",  Hardname, Stime, compid,  pasDbSqlCode(NULL));
                    recodeLogInfo("�����¼��nwusblist?��ݱ���ʧ�ܣ?", string, sql);
                }
                recodeLogInfo("99999999999999999999999999999999999999999999999999999999" , string , sql);
            }
        }

    }//end for()

    //����¼��Nums2��ֵ�Ƿ��Nums �?
    memset(mesg, 0, sizeof(mesg));
    snprintf(mesg, sizeof(mesg) - 1, "compid=%u, userid=%d, Nums=%d, success=%d,data=%s", compid, userid, Nums, Nums2, data);
    if(Nums != Nums2)
    {
        Status = -3;
        Nums2 = 0;
        recodeLogInfo("������־��Error(Nums != success):", mesg, "");

    }
    else
    {
        Status = 1;
        if(strlen(data) > 0)
        {

            //����������־Ŀ¼,��������?�Ŀ¼��ŵ�dir�У�? �����ʹ?�Ŀ�?
            createDir(dir,  sizeof(dir));

            //��ѯ��ǰ����Ӧ�ñ�����?ĸ��ļ��У?�����ѯ�ɹ�����򿪴��ļ�,ʧ���򷵻ش���
            if(searchPos2(userid, "nwremovelog", dir, savefile, sizeof(dir)))
            {
                Status = -4;
                Nums2 = 0;
                recodeLogInfo("������־��Error:", "���Ҵ洢�ļ�ʧ�ܣ�", "");
            }
            else
            {
                if((fp = fopen(savefile, "a+")) == NULL)    //�򿪴洢�ļ�ʧ��
                {
                    Status  = -5;
                    Nums2 = 0;
                    recodeLogInfo("������־��Error:", "�򿪴洢�ļ�ʧ�ܣ�", savefile);
                }
                else    //���ļ��ɹ����򱣴����ݵ��ļ���
                {

                    pasFileSetFileLock(fp);//���ļ�����

                    int len = fwrite(data, 1, strlen(data) ,  fp);

                    fseek(fp, 0, SEEK_END);
                    int len3 = ftell(fp);

                    pasFileSetFileUnLock(fp);//���ļ�����

                    fclose(fp);
                }
            }
        }
        recodeLogInfo(" ����������־3��" , mesg, "");
    }
    //printf("------------------------------------receive process log, compid = %u, Status = %d, Nums = %d, Nums2 = %d\n", compid, Status,  Nums, Nums2);

    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                   "compid",   UT_TYPE_LONG,    compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Nums",     UT_TYPE_LONG,     Nums2);
    free(data);
    return 0;
}

//4����ҳ?�־�ϴ?
int  Lan_WebLog_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int   Nums   = 0;
    unsigned int   compid = 0;
    unsigned int   userid = 0;
    char  mac[50];
    unsigned int   groupid = 0;
    unsigned int    groupid2 = 0;
    char  devname[50];
    char  username[50];
    unsigned int   sip  = 0;
    char  procname[100];

    unsigned int   Stime = 0;
    unsigned int      urlClass = 0;
    char  Webaddr[129];
    char  Subject[129];
    char  Content[129];
    char  Host[129];

    char  Status = 0;
    int   Nums2 = 0;
    long  iReturn = 0;
    FILE   *fp = NULL;
    char     *data = NULL;
    char    buffer[1024];
    char    mesg[512];

    char dir[512]; //����Ŀ¼
    char savefile[512];  //�������?��?
    //utMsgPrintMsg(psMsgHead);

    memset(mac, 0, sizeof(mac));
    memset(devname, 0, sizeof(devname));
    memset(username, 0, sizeof(username));
    memset(procname, 0, sizeof(procname));
    memset(Webaddr, 0, sizeof(Webaddr));
    memset(Subject, 0, sizeof(Subject));
    memset(Content, 0, sizeof(Content));
    memset(dir,  0, sizeof(dir));
    memset(savefile, 0, sizeof(savefile));
    memset(Host, 0, sizeof(Host));
    memset(buffer, 0, sizeof(buffer));
    memset(mesg, 0, sizeof(mesg));

    //����������־Ŀ¼,����������Ŀ¼��ŵ�dir?У��?����ʹ�ø�Ŀ�?
    createDir(dir,  300);
    recodeLogInfo("WEB_LOG", "**********SSSSSSSSSSSSSSSSSSS***************", "00000000000000000000");
    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "compid",  UT_TYPE_ULONG,  4,  &compid,
                               "userid",  UT_TYPE_ULONG,  4,  &userid,
                               "Nums",    UT_TYPE_LONG,  4,  &Nums
                              );

    if(CheckIP(iFd) != 1)
    {
        Status = 1;
        Nums2 = Nums;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("��ҳ��־��", "���˸���Ϣ", "");
        return 0;
    }
    //recodeLogInfo("WEB_LOG","**********SSSSSSSSSSSSSSSSSSS***************", "1111111111111111111111");
    if((compid == 0))
    {
        Status = -2;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("��ҳ��־��", "compidΪ��", "");
        return -1;
    }
    //recodeLogInfo("WEB_LOG","**********SSSSSSSSSSSSSSSSSSS***************", "3333333333333333333333333333");
    //��̬���ٴ洢�ռ�
    data = (char *)malloc(PACKAGE_MAXLEN);
    if(!data)
    {
        Status = -3;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ?�Կ����ʱ���? */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        //recodeLogInfo("��ҳ��־��" , "��̬ ?ٿռ�ʧ�ܣ?, "");
        return -1;
    }
    else
    {
        memset(data, 0, PACKAGE_MAXLEN);
    }

    //recodeLogInfo("WEB_LOG","**********SSSSSSSSSSSSSSSSSSS***************", "22222222222222222222222");
    //��ȡ��Ϣ
    int   Numbers = 0;
    char  str[11][50];
    memset(str, 0, 11 * 50);

    //��ѯ��ǰ�ͻ�����.?
    nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);

    char sql[256];
    memset(sql, 0, sizeof(sql));

    sprintf(sql, "select groupid from nwcompute where compid=%lu", compid);

    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(groupid2), &groupid2);


    for(Numbers = 0; Numbers < Nums; Numbers++)
    {
        snprintf(str[0], 50, "mac%d", Numbers);
        snprintf(str[1], 50, "groupid%d", Numbers);
        snprintf(str[2], 50, "devname%d", Numbers);
        snprintf(str[3], 50, "username%d", Numbers);
        snprintf(str[4], 50, "sip%d", Numbers);
        snprintf(str[5], 50, "procname%d", Numbers);
        snprintf(str[6], 50, "Stime%d", Numbers);
        snprintf(str[7], 50, "urlClass%d", Numbers);
        snprintf(str[8], 50, "Webaddr%d", Numbers);
        snprintf(str[9], 50, "Subject%d", Numbers);
        snprintf(str[10], 50, "Content%d", Numbers);

        iReturn = utMsgGetSomeNVar(psMsgHead, 11,
                                   str[0],  UT_TYPE_STRING, sizeof(mac) - 1, mac,
                                   str[1],  UT_TYPE_LONG,   4,  &groupid,
                                   str[2], UT_TYPE_STRING,  sizeof(devname) - 1,  devname,
                                   str[3], UT_TYPE_STRING, sizeof(username) - 1, username,
                                   str[4], UT_TYPE_LONG,   4,  &sip,
                                   str[5], UT_TYPE_STRING, sizeof(procname) - 1, procname,
                                   str[6], UT_TYPE_LONG,   4,  &Stime,
                                   str[7], UT_TYPE_LONG,   4,  &urlClass,
                                   str[8], UT_TYPE_STRING, sizeof(Webaddr) - 1, Webaddr,
                                   str[9], UT_TYPE_STRING, sizeof(Subject) - 1, Subject,
                                   str[10], UT_TYPE_STRING, sizeof(Content) - 1, Content
                                  );
        /*
            if( (strlen(mac)<=0 ) &&
            (groupid == 0)&&
            (strlen(devname) <=0) &&
            (strlen(username)<=0)&&
            (strlen(procname)<=0) &&
            (Stime == 0)&&
            (urlClass==0)&&
            (strlen(Webaddr) <=0)&&
            (strlen(Subject) <=0)){
                recodeLogInfo("�յ�һ������ҳ��־","", "");
                Nums2++;
                continue;
            }

            */
        //recodeLogInfo("WEB_LOG","*************************", Webaddr);
        char * ulang  = utComGetVar_sd(psShmHead, "nclang", "Yes");


        if(0 == strcmp(ulang  ,  "big5"))
        {
            //��ַ
            memset(buffer, 0, sizeof(buffer));
            if(strncmp(Webaddr, "[BIG5]", 6) == 0)
            {

                if(myGBKToBIG5(Webaddr,  buffer, sizeof(buffer) - 1) < 0)
                {
                    Nums2++;
                    continue;
                }
                else
                {
                    snprintf(Webaddr, sizeof(Webaddr) - 1, "%s",  buffer);
                }

            }
            //����
            memset(buffer, 0, sizeof(buffer));
            if(strncmp(Subject, "[BIG5]", 6) == 0)
            {
                if(myGBKToBIG5(Subject,  buffer, sizeof(buffer) - 1) < 0)
                {
                    Nums2++;
                    continue;
                }
                else
                {
                    snprintf(Subject, sizeof(Subject) - 1, "%s",  buffer);
                }
            }
        }

        //recodeLogInfo("WEB_LOG","111111111111111111111111", Webaddr);
        char mystring[512];
        memset(mystring, 0, sizeof(mystring));
        snprintf(mystring, sizeof(mystring) - 1, " compid=%u, stime=%u", compid, Stime);
        if(sip == 0)
        {
            recodeLogInfo(" ��?�һ����ҳ���?", mystring,  "ip = 0");
        }
        if(Stime == 0)
        {
            recodeLogInfo(" �յ�һ����ҳ��־:", mystring,  "Stime == 0");
        }
        if(strlen(Webaddr) <= 0)
        {
            recodeLogInfo(" �յ�һ����ҳ��־:", mystring,  "Webaddr== NULL");
            //Nums2++;
            //continue;
        }
        if(strlen(Subject) <= 0)
        {
            recodeLogInfo(" �յ�һ����ҳ��־:", mystring,  "Subject== NULL");
        }
        //recodeLogInfo("WEB_LOG","00000000000000000000000for...........", Webaddr);

        //�������ݵ��ļ���
        unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
        if(psComp != NULL)
        {
            userid = psComp->userid;
            groupid = psComp->groupid;
        }
        //groupid = groupid2;

        //recodeLogInfo("WEB_LOG","2222222222222222222222222222", Webaddr);
        //��Webaddr����ȡhost�ֶ�
        int Ret = GetHost(Webaddr, Host, sizeof(Host) - 1);
        if(Ret <= 0)
        {
            Nums2++;
            continue;
        }
        recodeLogInfo("WEB_LOG", "33333333333333333333333333", Webaddr);
        snprintf(data + strlen(data), PACKAGE_MAXLEN - strlen(data) - 1, "%llu\t%u\t%u\t%s\t"
                 "%u\t%s\t%s\t%u\t"
                 "%s\t%u\t%u\t%s\t"
                 "%s\t%s\t%s\n",
                 currentTime,    compid,      userid ,    mac,
                 groupid,        devname,    username,    sip,
                 procname,       Stime,      urlClass,    Webaddr,
                 Subject,        Content,    Host
                );

        Nums2++;

    }//end for()

    //����¼��Nums2��?��Ƿ��Nums���
    memset(mesg, 0, sizeof(mesg));
    snprintf(mesg, sizeof(mesg) - 1, "compid=%u, userid=%d, Nums=%d, success=%d", compid, userid, Nums, Nums2);
    if(Nums != Nums2)
    {
        Status = -3;
        Nums2 = 0;
        recodeLogInfo("��ҳ��־��Error(Nums != success):", mesg, "");

    }
    else
    {
        //����������־Ŀ¼,��������?�Ŀ¼��ŵ�dir�У��Ա����ʹ�ø�Ŀ¼
        createDir(dir,  sizeof(dir));

        //��ѯ��ǰ����Ӧ�ñ������ĸ��ļ��У� �����ѯ�ɹ�����򿪴��ļ�,ʧ���򷵻ش���
        if(searchPos2(userid, "nwweblog", dir, savefile, sizeof(dir)))
        {
            Status = -4;
            Nums2 = 0;
            recodeLogInfo("��ҳ��־��Error:", "���Ҵ洢�ļ�ʧ�ܣ�", "");
        }
        else
        {
            if((fp = fopen(savefile, "a+")) == NULL)    //�򿪴洢�ļ�ʧ��
            {
                Status  = -5;
                Nums2 = 0;
                recodeLogInfo("��ҳ��־��Error:", "�򿪴洢�ļ�ʧ�ܣ�", savefile);
            }
            else    //���ļ��ɹ����򱣴����ݵ��ļ���
            {
                //���ļ�����
                pasFileSetFileLock(fp);

                fwrite(data, 1, strlen(data) ,  fp);

                //���ļ�?��?
                pasFileSetFileUnLock(fp);

                fclose(fp);
                fp = NULL;

                FILE * ff = fopen("/home/ncmysql/nw/temp/web.log", "a+");
                if(ff)
                {
                    fwrite(data, 1, strlen(data) ,  ff);
                    fclose(ff);
                }

                Status = 1;
            }
        }
        recodeLogInfo(" ������ҳ��־3��" , mesg, "");
    }


    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                   "compid",   UT_TYPE_LONG,    compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Nums",     UT_TYPE_LONG,    Nums2);
    free(data);
    return 0;
}

//5?�������־�ϴ?
int  Lan_ChatLog_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int   Nums   = 0;
    unsigned int   compid = 0;
    unsigned int   userid = 0;
    char  mac[50];
    unsigned int   groupid = 0;
    unsigned int    groupid2 = 0;
    char  devname[50];
    char  username[50];
    unsigned int   sip  = 0;
    char  procname[100];

    unsigned int   Stime  = 0;
    int   Service = 0;
    int   Funcode = 0;
    char  Mfrom[50];
    char  Mto[50];
    char  Mesg[256];
    char  Myaccount[50];
    char  Friendaccount[50];

    char  Status = 0;
    int   Nums2 = 0;
    long  iReturn = 0;
    FILE   *fp = NULL;
    char      *data = NULL;

    char dir[300]; //����Ŀ¼
    char savefile[300];  //?�����ļ��?
    //utMsgPrintMsg(psMsgHead);

    memset(mac, 0, sizeof(mac));
    memset(devname, 0, sizeof(devname));
    memset(username, 0, sizeof(username));
    memset(procname, 0, sizeof(procname));
    memset(Mfrom, 0, sizeof(Mfrom));
    memset(Mto, 0, sizeof(Mto));
    memset(Mesg,  0,  sizeof(Mesg));
    memset(Myaccount,  0, sizeof(Myaccount));
    memset(Friendaccount,  0, sizeof(Friendaccount));
    memset(dir,  0, sizeof(dir));
    memset(savefile, 0, sizeof(savefile));

    //����������־Ŀ¼,����������Ŀ¼���?�dir�У��Ա�?���ʹ�ø�Ŀ�?
    createDir(dir,  300);

    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "compid",  UT_TYPE_ULONG,  4, &compid,
                               "userid",  UT_TYPE_ULONG,  4, &userid,
                               "Nums",    UT_TYPE_LONG,  4, &Nums
                              );

    if(CheckIP(iFd) != 1)
    {
        Status = 1;
        Nums2 = Nums;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("������־��", "���˸���Ϣ", "");
        return 0;
    }


    if((compid == 0))
    {
        Status = -3;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("������־:", "compidΪ��", "");
        return -1;
    }

    //��̬���ٴ洢�ռ�
    data = (char *)malloc(PACKAGE_MAXLEN);
    if(!data)
    {
        Status = -4;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ�� �ʱ���? */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("������־:", "��̬���ٿռ�ʧ��", "");
        return -1;
    }
    else
    {
        memset(data, 0, PACKAGE_MAXLEN);
    }

    //��ȡ��??
    int   Numbers = 0;
    char  str[14][50];
    char  conver[50];
    memset(str, 0, 14 * 50);
    memset(conver, 0, sizeof(conver));

    //��ѯ��ǰ.ͻ������?
    nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
    char sql[256];
    memset(sql, 0, sizeof(sql));

    sprintf(sql, "select groupid from nwcompute where compid=%lu", compid);

    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(groupid2), &groupid2);


    for(Numbers = 0; Numbers < Nums; Numbers++)
    {
        snprintf(str[0], 50, "mac%d", Numbers);
        snprintf(str[1], 50, "groupid%d", Numbers);
        snprintf(str[2], 50, "devname%d", Numbers);
        snprintf(str[3], 50, "username%d", Numbers);
        snprintf(str[4], 50, "sip%d", Numbers);
        snprintf(str[5], 50, "procname%d", Numbers);

        snprintf(str[6], 50, "Stime%d", Numbers);
        snprintf(str[7], 50, "Service%d", Numbers);
        snprintf(str[8], 50, "Funcode%d", Numbers);
        snprintf(str[9], 50, "Mfrom%d", Numbers);
        snprintf(str[10], 50, "Mto%d", Numbers);
        snprintf(str[11], 50, "Mesg%d", Numbers);
        snprintf(str[12], 50, "Myaccount%d", Numbers);
        snprintf(str[13], 50, "Friendaccount%d", Numbers);

        iReturn = utMsgGetSomeNVar(psMsgHead, 14,
                                   str[0],  UT_TYPE_STRING, sizeof(mac) - 1, mac,
                                   str[1],  UT_TYPE_LONG,   4,  &groupid,
                                   str[2], UT_TYPE_STRING,  sizeof(devname) - 1,  devname,
                                   str[3], UT_TYPE_STRING, sizeof(username) - 1, username,
                                   str[4], UT_TYPE_LONG,   4,  &sip,
                                   str[5], UT_TYPE_STRING, sizeof(procname) - 1, procname,
                                   str[6], UT_TYPE_ULONG,   4,  &Stime,
                                   str[7], UT_TYPE_LONG,   4,  &Service,
                                   str[8], UT_TYPE_LONG,   4,  &Funcode ,
                                   str[9], UT_TYPE_STRING, sizeof(Mfrom) - 1, Mfrom,
                                   str[10], UT_TYPE_STRING, sizeof(Mto) - 1, Mto,
                                   str[11], UT_TYPE_STRING, sizeof(Mesg) - 1, Mesg,
                                   str[12], UT_TYPE_STRING, sizeof(Myaccount) - 1,  Myaccount,
                                   str[13], UT_TYPE_STRING, sizeof(Friendaccount) - 1, Friendaccount
                                  );

        if((strlen(mac) <= 0) &&
           (groupid == 0) &&
           (strlen(devname) <= 0) &&
           (strlen(username) <= 0) &&
           (sip == 0) &&
           (strlen(procname) <= 0) &&
           (Stime == 0) &&
           (Service == 0) &&
           (Funcode == 0) &&
           (strlen(Mfrom) <= 0) &&
           (strlen(Mto) <= 0) &&
           (strlen(Mesg) <= 0) &&
           (strlen(Myaccount) <= 0) &&
           (strlen(Friendaccount) <= 0))
        {
            recodeLogInfo(" �յ�һ��������־", "", "");
            Nums++;
            continue;
        }

        char mystring[512];
        memset(mystring, 0, sizeof(mystring));
        snprintf(mystring, sizeof(mystring) - 1, " compid=%u, stime=%u", compid, Stime);

        if(sip == 0)
        {
            recodeLogInfo(" �յ�һ��������־", mystring, "ip = 0");
        }
        if(Stime == 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Stime == 0");
        }
        if(strlen(mac) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "mac== NULL");
        }
        if(strlen(Mfrom) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Mfrom== NULL");
        }
        if(strlen(Mto) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Mto== NULL");
        }
        if(strlen(Mesg) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Mesg== NULL");
        }
        if(strlen(Myaccount) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Myaccount== NULL");
        }
        if(strlen(Friendaccount) <= 0)
        {
            recodeLogInfo(" �յ�һ��������־:", mystring, "Friendaccount == NULL");
        }

        memset(mystring, 0, sizeof(mystring));
        snprintf(mystring, sizeof(mystring) - 1, " Mfrom=%s, Mto=%s,Mesg=%s", Mfrom, Mto, Mesg);
        recodeLogInfo(" �յ�һ��������־:", mystring, "");

        //�������ݵ��ļ���
        unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
        if(psComp != NULL)
        {
            userid = psComp->userid;
            groupid = psComp->groupid;
        }
        //groupid = groupid2;

        //��.�from
        if(0 == strncmp(Mfrom, "[BIG5]",  6))
        {
            char * Myfrom =  convert("BIG5", "GBK",  Mfrom + 6);
            if(Myfrom == NULL)
            {
                recodeLogInfo(" �յ�һ��BIG5������?�from:", "BIG5 to GBK  error", Mfrom);
                snprintf(Mfrom, sizeof(Mfrom) - 1,  "%s", Mfrom + 6);
            }
            else
            {
                recodeLogInfo(" �յ�һ��BIG5����.�־from:", "BIG5 to GBK  success", Myfrom);
                snprintf(Mfrom, sizeof(Mfrom), "%s",  Myfrom);
            }
        }
        else if(0 == strncmp(Mfrom, "[GBK]",  5))
        {
            recodeLogInfo(" �յ�һ��GBK������־:from", "BIG5 to GBK  success", Mfrom);
            snprintf(Mfrom, sizeof(Mfrom) - 1,  "%s", Mfrom + 5);
        }

        //���͵�to
        if(0 == strncmp(Mto, "[BIG5]",  6))
        {
            char * Myto =    convert("BIG5", "GBK",  Mto + 6);
            if(Myto == NULL)
            {
                recodeLogInfo(" �յ�һ��BIG5������־to:", "BIG5 to GBK  error", Mto);
                snprintf(Mto, sizeof(Mto) - 1,  "%s", Mto + 6);
            }
            else
            {
                recodeLogInfo(" �յ�һ��BIG5������־to:", "BIG5 to GBK  success", Myto);
                snprintf(Mto, sizeof(Mto), "%s",  Myto);
            }
        }
        else if(0 == strncmp(Mto, "[GBK]",  5))
        {
            recodeLogInfo(" �յ�һ��GBK������־:to", "BIG5 to GBK  success", Mto);
            snprintf(Mto, sizeof(Mto) - 1,  "%s", Mto + 5);
        }

        //�ҵ��˺�myaccount
        if(0 == strncmp(Myaccount, "[BIG5]",  6))
        {
            char * MyMAccount =  convert("BIG5", "GBK",  Myaccount + 6);
            if(MyMAccount == NULL)
            {
                recodeLogInfo(" �յ�һ��BIG5������־Myaccount:", "BIG5 to GBK  error", Myaccount);
                snprintf(Myaccount, sizeof(Myaccount) - 1,  "%s", Myaccount + 6);
            }
            else
            {
                recodeLogInfo(" �յ�һ��BIG5������־Myaccount:", "BIG5 to GBK  success", MyMAccount);
                snprintf(Myaccount, sizeof(Myaccount), "%s",  MyMAccount);
            }
        }
        else if(0 == strncmp(Myaccount, "[GBK]",  5))
        {
            recodeLogInfo(" �յ�һ��GBK������־:Myaccount", "BIG5 to GBK  success", Myaccount);
            snprintf(Myaccount, sizeof(Myaccount) - 1,  "%s", Myaccount + 5);
        }

        //�Է��˺�friendaccount
        if(0 == strncmp(Friendaccount, "[BIG5]",  6))
        {
            char * MyFriendaccount = convert("BIG5", "GBK",  Friendaccount + 6);
            if(MyFriendaccount == NULL)
            {
                recodeLogInfo(" �յ�һ��BIG5.�����־from:", "BIG5 to GBK  error", Friendaccount);
                snprintf(Friendaccount, sizeof(Friendaccount) - 1,  "%s", Friendaccount + 6);
            }
            else
            {
                recodeLogInfo(" �յ�һ �BIG5����?�־from:", "BIG5 to GBK  success", MyFriendaccount);
                snprintf(Friendaccount, sizeof(Friendaccount), "%s",  MyFriendaccount);
            }
        }
        else if(0 == strncmp(Friendaccount, "[GBK]",  5))
        {
            recodeLogInfo(" �յ�һ��GBK?������?from", "BIG5 to GBK  success", Friendaccount);
            snprintf(Friendaccount, sizeof(Friendaccount) - 1,  "%s", Friendaccount + 5);
        }

        //��Ϣmesg
        if(0 == strncmp(Mesg, "[BIG5]",  6))
        {
            char * MyMesg =  convert("BIG5", "GBK",  Mesg + 6);
            if(MyMesg == NULL)
            {
                recodeLogInfo(" �յ�һ.�BIG5������־:", "BIG5 to GBK  error", Mesg);
                snprintf(Mesg, sizeof(Mesg) - 1,  "%s", Mesg + 6);
            }
            else
            {
                recodeLogInfo(" �յ�һ��BIG5������־:", "BIG5 to GBK  success", MyMesg);
                snprintf(Mesg, sizeof(Mesg), "%s",  MyMesg);
            }
        }
        else if(0 == strncmp(Mesg, "[GBK]",  5))
        {
            recodeLogInfo(" �յ�һ��GBK������־:", "BIG5 to GBK  success", Mesg);
            snprintf(Mesg, sizeof(Mesg) - 1,  "%s", Mesg + 5);
        }


        int x = 0;
        int    length = strlen(Mesg);
        for(x = 0;  x < length;  x++)
        {
            if((Mesg[x] == '\n') || (Mesg[x] == '\r') || (Mesg[x] == '\t'))
            {
                Mesg[x] = ' ';
            }
        }

        //�õ�MD5ֵ
        unsigned  char md5_code[512];
        unsigned  char   md5_res[64];
        memset(md5_code, 0, sizeof(md5_code));
        memset(md5_res, 0, sizeof(md5_res));

        snprintf(md5_code, sizeof(md5_code) - 1, "%s%s", Mfrom, Mto);
        lan_GetMD5(md5_code, strlen(md5_code),  md5_res);

        snprintf(data + strlen(data), PACKAGE_MAXLEN - sizeof(data) - 1,   "%llu\t%u\t%u\t%s\t"
                 "%u\t%s\t%s\t%u\t"
                 "%s\t%u\t%d\t%d\t"
                 "%s\t%s\t%s\t%s\t"
                 "%s\t%s\n",
                 currentTime,    compid,      userid ,    mac,
                 groupid,        devname,    username,    sip,
                 procname,       Stime,      Service,    Funcode,
                 Mfrom,          Mto,        Mesg,        md5_res,
                 Myaccount,     Friendaccount
                );

        Nums2++;
    }//end for()


    //����¼��Nums2��ֵ��?��Nums���
    char  mesg[512];
    memset(mesg, 0, sizeof(mesg));
    snprintf(mesg, sizeof(mesg) - 1, "compid=%u, userid=%d, Nums=%d, success=%d", compid, userid, Nums, Nums2);
    if(Nums != Nums2)
    {
        Status = -3;
        Nums2 = 0;
        recodeLogInfo("������־��Error(Nums != success):", mesg, "");

    }
    else
    {
        //��������?�־Ŀ�?��������?�Ŀ¼��ŵ�dir�У��Ա����ʹ�ø�Ŀ¼
        createDir(dir,  sizeof(dir));

        //��ѯ��ǰ����Ӧ�ñ������ĸ��ļ��У� �����ѯ�ɹ�����򿪴��ļ�,ʧ���򷵻ش���
        if(searchPos2(userid, "ncimclient", dir, savefile, sizeof(dir)))
        {
            Status = -4;
            Nums2 = 0;
            recodeLogInfo("������־��Error:", "���Ҵ洢�ļ�ʧ�ܣ�", "");
        }
        else
        {
            if((fp = fopen(savefile, "a+")) == NULL)    //�򿪴洢�ļ�ʧ��
            {
                Status  = -5;
                Nums2 = 0;
                recodeLogInfo("������־��Error:", "�򿪴洢�ļ�ʧ�ܣ�", savefile);
            }
            else    //���ļ��ɹ����򱣴����ݵ��ļ���
            {
                //���ļ���??
                pasFileSetFileLock(fp);

                fwrite(data, 1, strlen(data) ,  fp);
                FILE * file = fopen("/home/ncmysql/nw/char.dat",  "a+");
                if(!file)
                {
                    recodeLogInfo("��¼������Ϣʧ��", "", "");
                }
                else
                {
                    fwrite(data, 1, strlen(data), file);
                    fclose(file);
                }

                //���ļ�����
                pasFileSetFileUnLock(fp);

                fclose(fp);
                fp = NULL;

                Status = 1;
            }
        }
        recodeLogInfo(" ��������???��" , mesg, "");
    }


    //printf("������־��compid= %d, Status=%d, Num = %d, SuccessNum=%d\n", compid, Status,Nums, Nums2);

    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                   "compid",   UT_TYPE_LONG,    compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Nums",     UT_TYPE_LONG,     Nums2);
    free(data);
    return 0;
}

//6����?��¼��ϴ?
int  Lan_EventLog_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int   Nums   = 0;
    unsigned int   compid = 0;
    unsigned int   userid = 0;
    char  mac[50];
    unsigned int   groupid = 0;
    unsigned int    groupid2 = 0;
    char  devname[50];
    char  username[50];
    unsigned int   sip  = 0;
    char  procname[100];

    int   Libtype  = 0;
    int   Eventtype = 0;
    int   Stime = 0;
    char  Name[50];
    char  Content[150];

    char  Status  = 0;
    int   Nums2 = 0;
    long  iReturn = 0;
    FILE   *fp = NULL;
    char dir[300]; //����Ŀ¼
    char savefile[300];  //������ļ���
    //utMsgPrintMsg(psMsgHead);

    memset(mac, 0, sizeof(mac));
    memset(devname, 0, sizeof(devname));
    memset(username, 0, sizeof(username));
    memset(procname, 0, sizeof(procname));
    memset(Name, 0, sizeof(Name));
    memset(Content, 0, sizeof(Content));
    memset(dir,  0, sizeof(dir));
    memset(savefile, 0, sizeof(savefile));

    //����������־Ŀ¼,����������Ŀ¼��ŵ�dir�У��Ա����ʹ��.�Ŀ�?
    createDir(dir,  300);

    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "compid",  UT_TYPE_ULONG,  4, &compid,
                               "userid",  UT_TYPE_LONG,  4, &userid,
                               "Nums",    UT_TYPE_LONG,  4, &Nums
                              );

    if(CheckIP(iFd) != 1)
    {
        Status = 1;
        Nums2 = Nums;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);
        recodeLogInfo("�赲�¼���־��", "���˸���Ϣ", "");
        return 0;
    }


    if((compid == 0) && (userid == 0))
    {
        Status = 0;
        Nums2 = -3;
        recodeLogInfo("�赲�¼���־��", "compidΪ��", "");

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);
        return -1;
    }

    //��ѯ��ǰ?���Ӧ�ñ���� �ĸ��ļ�??�����ѯ�ɹ����� � ��??
    if(!searchPos2(userid, "nweventlog", dir, savefile, 300))
    {
        if((fp = fopen(savefile, "a+")) != NULL)  //���ļ��ɹ����򱣴����ݵ��ļ���
        {
            //���ļ�����
            pasFileSetFileLock(fp);

            int   Numbers = 0;
            char  str[12][50];
            char  conver[50];
            memset(str, 0, 12 * 50);
            memset(conver, 0, sizeof(conver));

            //��ѯ?�ǰ�ͻ����??
            nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
            char sql[256];
            memset(sql, 0, sizeof(sql));

            sprintf(sql, "select groupid from nwcompute where compid=%lu", compid);

            pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(groupid2), &groupid2);


            for(Numbers = 0; Numbers < Nums; Numbers++)
            {
                snprintf(str[0], 50, "mac%d", Numbers);
                snprintf(str[1], 50, "groupid%d", Numbers);
                snprintf(str[2], 50, "devname%d", Numbers);
                snprintf(str[3], 50, "username%d", Numbers);
                snprintf(str[4], 50, "sip%d", Numbers);
                snprintf(str[5], 50, "procname%d", Numbers);

                snprintf(str[6], 50, "Libtype%d", Numbers);
                snprintf(str[7], 50, "Eventtype%d", Numbers);
                snprintf(str[8], 50, "Stime%d", Numbers);
                snprintf(str[9], 50, "Name%d", Numbers);
                snprintf(str[10], 50, "Content%d", Numbers);

                iReturn = utMsgGetSomeNVar(psMsgHead, 11,
                                           str[0],  UT_TYPE_STRING, sizeof(mac) - 1, mac,
                                           str[1],  UT_TYPE_LONG,   4,  &groupid,
                                           str[2], UT_TYPE_STRING,  sizeof(devname) - 1,  devname,
                                           str[3], UT_TYPE_STRING, sizeof(username) - 1, username,
                                           str[4], UT_TYPE_LONG,   4,  &sip,
                                           str[5], UT_TYPE_STRING, sizeof(procname) - 1, procname,
                                           str[6], UT_TYPE_LONG,   4, &Libtype,
                                           str[7], UT_TYPE_LONG,   4, &Eventtype,
                                           str[8], UT_TYPE_LONG,   4, &Stime,
                                           str[9], UT_TYPE_STRING, sizeof(Name) - 1,  Name,
                                           str[10], UT_TYPE_STRING, sizeof(Content) - 1, Content
                                          );
                if((strlen(mac) <= 0) &&
                   (groupid == 0) &&
                   (strlen(devname) <= 0) &&
                   (strlen(username) <= 0) &&
                   (sip == 0) &&
                   (strlen(procname) <= 0) &&
                   (Stime == 0) &&
                   (Libtype == 0) &&
                   (Eventtype == 0) &&
                   (strlen(Name) <= 0) &&
                   (strlen(Content) <= 0))
                {
                    recodeLogInfo("�յ�һ���յ��赲ʱ����־", "", "");
                    continue;
                }


                //�������ݵ��ļ���
                unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
                snprintf(conver, 50, "%llu", currentTime);
                fwrite(conver, strlen(conver), 1, fp);
                fwrite("\t" ,   1, 1,  fp);  //.á�\t  ��ֶηֿ?

                snprintf(conver, 50, "%u", compid);
                fwrite(conver, strlen(conver), 1, fp);
                fwrite("\t" ,   1, 1,  fp);  //�á�\t������?ηֿ?

                if(psComp != NULL)
                {
                    snprintf(conver, 50, "%u", psComp->userid);
                    fwrite(conver, strlen(conver), 1, fp);
                    fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�
                }
                else
                {
                    snprintf(conver, 50, "%u", userid);
                    fwrite(conver, strlen(conver), 1, fp);
                    fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�

                }

                fwrite(mac,  strlen(mac), 1, fp);
                fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�


                //groupid = groupid2;
                if(psComp != NULL)
                {
                    snprintf(conver, 50, "%u", groupid);
                    fwrite(conver, strlen(conver), 1, fp);
                    fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�
                }
                else
                {
                    snprintf(conver, 50, "%u", groupid);
                    fwrite(conver, strlen(conver), 1, fp);
                    fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�
                }

                fwrite(devname, strlen(devname), 1, fp);
                fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�

                fwrite(username,  strlen(username), 1, fp);
                fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�

                snprintf(conver, 50, "%u", sip);
                fwrite(conver,  strlen(conver),  1, fp);
                fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�

                fwrite(procname,  strlen(procname),  1,  fp);
                fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�

                snprintf(conver, 50, "%d", Libtype);
                fwrite(conver, strlen(conver), 1, fp);
                fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�

                snprintf(conver, 50, "%d", Eventtype);
                fwrite(conver, strlen(conver), 1, fp);
                fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�

                snprintf(conver, 50, "%u", Stime);
                fwrite(conver, strlen(conver), 1, fp);
                fwrite("\t" ,   1, 1,  fp);  //�á�\t�����ֶηֿ�

                fwrite(Name,  strlen(Name), 1, fp);
                fwrite("\t" ,   1, 1,  fp);  //�á�\t����?ֶ?ֿ?

                fwrite(Content,  strlen(Content), 1, fp);
                fwrite("\n" ,   1, 1,  fp);  //�á�\n���� ?

                Nums2++;
            }//end for()

            Status = 1;

            //?��ļ����?
            pasFileSetFileUnLock(fp);

            fclose(fp);
            fp = NULL;

        }//end fopen()
    }//end searchPos()

    if(Nums > Nums2)
    {
        Nums2 = 0;
        char findError1[512];
        char findError2[512];

        memset(findError1, 0, sizeof(findError1));
        memset(findError2, 0, sizeof(findError2));

        snprintf(findError1, sizeof(findError1) - 1, "compid=%u, Status = %d, Nums=%d", compid, Status, Nums);
        snprintf(findError2, sizeof(findError2) - 1, "Nums2=%d", Nums2);
        recodeLogInfo("�赲�¼���־��", findError1, findError2);

    }

    //printf("�赲�¼���־��compid= %d, Status=%d, Num = %d, SuccessNum=%d\n", compid, Status,Nums, Nums2);

    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Nums",     UT_TYPE_LONG,     Nums2);
}


//7����Ļ��־�ϴ�
int Lan_ScreenLog_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int   Nums   = 0;
    unsigned int   compid = 0;
    unsigned int   userid = 0;
    char  mac[50];
    unsigned int   groupid = 0;
    unsigned int    groupid2 = 0;
    char  devname[50];
    char    mydevname[128];
    char  username[50];
    unsigned int   sip  = 0;
    char  procname[100];

    unsigned int stime  = 0;
    char    file[80];
    char    destPath[512];
    char    sql[20480];
    char    sql1[20480];
    char    sql2[20480];

    char  Status  = 0;
    int   Nums2 = 0;
    long  iReturn = 0;
    FILE   *fp = NULL;
    char dir[512]; // ���Ŀ�?
    char savefile[512];  //������ļ�??
    //utMsgPrintMsg(psMsgHead);

    char  year[10];
    char  mon[10];
    char  day[10];

    memset(year, 0, sizeof(year));
    memset(mon, 0 , sizeof(mon));
    memset(day, 0, sizeof(day));

    memset(mac, 0, sizeof(mac));
    memset(devname, 0, sizeof(devname));
    memset(mydevname, 0, sizeof(mydevname));
    memset(username, 0, sizeof(username));
    memset(procname, 0, sizeof(procname));
    memset(file, 0, sizeof(file));
    memset(destPath, 0, sizeof(destPath));
    memset(sql,  0, sizeof(sql));
    memset(sql1, 0, sizeof(sql1));
    memset(sql2, 0, sizeof(sql2));
    memset(dir,  0, sizeof(dir));
    memset(savefile, 0, sizeof(savefile));

    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "compid",  UT_TYPE_LONG,  4, &compid,
                               "userid",  UT_TYPE_LONG,  4, &userid,
                               "Nums",    UT_TYPE_LONG,  4, &Nums
                              );

    if(CheckIP(iFd) != 1)
    {
        Status = 1;
        Nums2 = Nums;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("��Ļ��־��", "���˸���Ϣ", "");
        return 0;
    }


    if((compid == 0))
    {
        Status = 0;
        Nums2 = -3;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, Status,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("��Ļ��־��", "compidΪ��", "");
        return -1;
    }

    int   Numbers = 0;
    char  str[12][50];
    char  conver[50];
    memset(str, 0, 12 * 50);
    memset(conver, 0, sizeof(conver));

    //����������־?��?����������Ŀ¼��ŵ�dir�У��Ա����ʹ�ø�Ŀ¼
    createDir(dir,  sizeof(512));

    //��ѯ��ǰ?ͻ������?
    nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
    char Mysql[256];
    memset(Mysql, 0, sizeof(Mysql));

    //sprintf(Mysql,"select groupid from nwcompute where compid=%lu",compid);

    //pasDbOneRecord(Mysql,0,UT_TYPE_ULONG,sizeof(groupid2),&groupid2);


    if(psComp)
    {
        snprintf(mydevname, sizeof(mydevname) - 1, "%s", psComp->devname);
        groupid2 = psComp->groupid;
    }

    for(Numbers = 0; Numbers < Nums; Numbers++)
    {
        snprintf(str[0], 50, "mac%d", Numbers);
        snprintf(str[1], 50, "groupid%d", Numbers);
        snprintf(str[2], 50, "devname%d", Numbers);
        snprintf(str[3], 50, "username%d", Numbers);
        snprintf(str[4], 50, "sip%d", Numbers);
        snprintf(str[5], 50, "Stime%d", Numbers);
        snprintf(str[6], 50, "file%d", Numbers);

        iReturn = utMsgGetSomeNVar(psMsgHead, 7,
                                   str[0],  UT_TYPE_STRING, sizeof(mac) - 1, mac,
                                   str[1],  UT_TYPE_LONG,   4,  &groupid,
                                   str[2], UT_TYPE_STRING,  sizeof(devname) - 1,  devname,
                                   str[3], UT_TYPE_STRING, sizeof(username) - 1, username,
                                   str[4], UT_TYPE_LONG,   4,  &sip,
                                   str[5], UT_TYPE_LONG,   4, &stime,
                                   str[6], UT_TYPE_STRING, sizeof(file) - 1,  file
                                  );


        char mystring[512];
        memset(mystring, 0, sizeof(mystring));
        snprintf(mystring, sizeof(mystring) - 1, " compid=%u, stime=%u", compid, stime);

        if(sip == 0)
        {
            recodeLogInfo(" �յ�һ����Ļ��־", mystring, "ip = 0");
        }
        if(stime == 0)
        {
            recodeLogInfo(" �յ�һ����Ļ��־:", mystring, "Stime == 0");
        }
        if(strlen(mac) <= 0)
        {
            recodeLogInfo(" �յ�һ����Ļ��־:", mystring, "mac== NULL");
        }
        if(strlen(devname) <= 0)
        {
            recodeLogInfo(" �յ�һ����Ļ��־:", mystring, "devname== NULL");
        }
        if(strlen(file) <= 0)
        {
            recodeLogInfo(" �յ�һ����Ļ��־:", mystring, "file== NULL");
        }


        //�ѿͻ�?˵�ǰʱ��ת�����ַ����?����ɹ����� ��Ϊ��ĻͼƬ���Ŀ¼�������÷������ĵ�ǰʱ����������Ŀ¼
        if(timeToString(stime, year, mon, day) == -1)
        {
            time_t now;
            time(&now);
            timeToString(now, year, mon, day);
        }
        snprintf(destPath, sizeof(destPath) - 1,  "%s%u/%s%s%s/%u.jpg", SCREENJPG_DIR, compid, year, mon, day, stime);
        recodeLogInfo("��Ļ��־��",  destPath, "");


        //�������ݵ����ݿ���
        unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��

        if(psComp != NULL)
        {
            userid = psComp->userid;
            groupid = psComp->groupid;
        }
        //groupid = groupid2;

        snprintf(sql, sizeof(sql) - 1,  "insert into ncscreenlog_%s%s( compid, userid, groupid,  devname, udisp, mac, sip, stime, file, sid) values(%u, %u, %u, \'%s\', \'%s\', \'%s\', %u, %u, \'%s\', %llu)",
                 year, mon, compid, userid,  groupid, devname, mydevname, mac, sip, stime, destPath, currentTime);
        //printf("sql = %s\n",  sql);

        iReturn = pasDbExecSql(sql, 0);
        //printf("iReturn = %d\n", iReturn );
        if(iReturn != 0)
        {
            iReturn = pasDbExecSqlF("create table ncscreenlog_%s%s ( \
    									sid  	    bigint auto_increment  primary key,   \
     									compid    int unsigned,\
									userid    int unsigned default 0,     \
	 								groupid   int unsigned default 0,\
									devname   char(32)  default ' ',      \
									udisp     char(32)  default '',       \
    									mac       char(20)  default '',       \
   			 						sip	      int unsigned,           \
    									stime     int,        \
    									file      char(80)    \
							);", year, mon);
            iReturn = pasDbExecSql(sql, 0);
            //printf("iReturn = %u,\n", iReturn);
        }

        if(0 == iReturn)
        {
            Nums2++;
        }

    }//end for()

    if(Nums != Nums2)
    {
        Nums2 = 0;
        char findError1[512];
        memset(findError1, 0, sizeof(findError1));
        snprintf(findError1, sizeof(findError1) - 1, "compid=%u, Nums=%d,Nums2=%d", compid, Nums, Nums2);
        recodeLogInfo("��Ļ��־��", findError1, "");
    }
    else
    {
        Status = 1;
    }

    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Nums",     UT_TYPE_LONG,     Nums2);
    return 0;
}


//8����ĻJPGͼƬ�ϴ�
int Lan_ScreenJPG_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int compid = 0;
    unsigned int userid   = 0;
    char         souPath[512];
    char         destDir[512];
    unsigned int stime = 0;
    char        *buffer =  NULL;
    unsigned int   bufferLength = 0;

    int     iRet = -1;
    int     Status = 0;
    int     Nums2 = 0;
    int     iReturn = 0;

    memset(souPath, 0, sizeof(souPath));
    memset(destDir, 0, sizeof(destDir));

    //  utMsgPrintMsg(psMsgHead);

    buffer = (char *)malloc(JPG_MAXLEN + 1);
    if(!buffer)
    {
        Status = -3;
        stime = 0;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Stime",    UT_TYPE_LONG, stime
                      );

        recodeLogInfo("����һ����Ļ��ͼ��", "��̬���ٿռ�ʧ��", "");
        return -1;
    }
    else
    {
        memset(buffer, 0 , JPG_MAXLEN + 1);
    }


    iReturn = utMsgGetSomeNVar(psMsgHead, 6,
                               "compid",  UT_TYPE_LONG,  4, &compid,
                               "userid",  UT_TYPE_LONG,  4, &userid,
                               "file",    UT_TYPE_STRING,  sizeof(souPath) - 1, souPath,
                               "currentTime", UT_TYPE_LONG, 4, &stime ,
                               "buffer",  UT_TYPE_STRUCT, JPG_MAXLEN, buffer,
                               "bufferLength", UT_TYPE_LONG, 4, &bufferLength
                              );

    char  year[10];
    char  mon[10];
    char  day[10];

    memset(year, 0, sizeof(year));
    memset(mon, 0, sizeof(mon));
    memset(day, 0, sizeof(day));

    //�ѿͻ��˵�ǰʱ��ת�����ַ����� ��?��ɹ?��?.�Ϊ��ĻͼƬ���Ŀ¼�������÷������ĵ�ǰʱ����������Ŀ�?
    if(timeToString(stime, year, mon, day) == -1)
    {
        time_t now;
        time(&now);
        timeToString(now, year, mon, day);
    }


    //����ͼƬ���Ŀ??
    snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/", SCREENJPG_DIR, compid, year, mon, day);
    iReturn = mkdir(destDir, S_IRWXU);

    if(iReturn != 0)
    {
        snprintf(destDir, sizeof(destDir) - 1,  "%s", SCREENJPG_DIR);
        iReturn = mkdir(destDir, S_IRWXU);

        snprintf(destDir, sizeof(destDir) - 1,  "%s%u/", SCREENJPG_DIR, compid);
        iReturn = mkdir(destDir, S_IRWXU);

        snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/", SCREENJPG_DIR, compid, year, mon, day);
        iReturn = mkdir(destDir, S_IRWXU);
    }


    //ͼƬ���·��
    snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/%u.jpg", SCREENJPG_DIR, compid, year, mon, day, stime);
    //printf("destDir=%s\n", destDir);

    FILE *fp = fopen(destDir,   "ab");
    if(NULL == fp)
    {
        Status = -1;

    }
    else
    {
        int saveLength = fwrite(buffer, 1, bufferLength, fp);
        fclose(fp);

        if((bufferLength == 0) || (saveLength < bufferLength))
        {
            char   receive[1024];
            char   save[1024];
            memset(receive, 0, sizeof(receive) - 1);
            memset(save, 0, sizeof(save) - 1);
            snprintf(receive, sizeof(receive) - 1,  "compid=%u,  ���մ� ? %d", compid, bufferLength);
            snprintf(save, sizeof(save) - 1,  "�洢��С: %d", saveLength);
            recodeLogInfo("error�����յ�һ����Ļ��ͼ��", receive, save);
            Status = 0;
        }
        else
        {
            Status = 1;
            char   receive[1024];
            char   save[1024];
            memset(receive, 0, sizeof(receive) - 1);
            memset(save, 0, sizeof(save) - 1);
            snprintf(receive, sizeof(receive) - 1,  "compid=%u, stime: %u,  filepath=%s", compid, stime, destDir);
            //snprintf(save, sizeof(save)-1,  "�洢?��? %d", saveLength);
            recodeLogInfo(" ���յ�һ����Ļ��ͼ��", receive, "");

        }
        //Status = 1;
    }

    //printf("Status = %d\n",   Status);
    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Stime",    UT_TYPE_LONG, stime
                  );

    free(buffer);
    return 0;
}

//9���澯��־�ϴ�
int Lan_ScreenAlterLog_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int   Nums   = 0;
    unsigned int   compid = 0;
    unsigned int   userid = 0;
    char  mac[50];
    unsigned int   groupid = 0;
    unsigned int    groupid2 = 0;
    char  devname[50];
    char  username[50];
    unsigned int   sip  = 0;
    char  procname[100];

    unsigned int stime  = 0;
    unsigned int screenid = 0;
    int      pid = 0;
    char    mesg[256];
    char    sql[20480];

    char  Status  = 0;
    int   Nums2 = 0;
    long  iReturn = 0;
    FILE   *fp = NULL;
    char dir[512]; //����Ŀ¼
    char savefile[512];  //������ļ���
    //utMsgPrintMsg(psMsgHead);

    char  year[8];
    char  mon[8];
    char   day[8];

    memset(year, 0, sizeof(year));
    memset(mon, 0, sizeof(mon));
    memset(day, 0, sizeof(day));

    memset(mac, 0, sizeof(mac));
    memset(devname, 0, sizeof(devname));
    memset(username, 0, sizeof(username));
    memset(procname, 0, sizeof(procname));
    memset(mesg, 0, sizeof(mesg));
    memset(sql,  0, sizeof(sql));
    memset(dir,  0, sizeof(dir));
    memset(savefile, 0, sizeof(savefile));

    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "compid",  UT_TYPE_LONG,  4, &compid,
                               "userid",  UT_TYPE_LONG,  4, &userid,
                               "Nums",    UT_TYPE_LONG,  4, &Nums
                              );

    if(CheckIP(iFd) != 1)
    {
        Status = 1;
        Nums2 = Nums;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("�澯��־��", "���˸���Ϣ", "");
        return 0;
    }


    if((compid == 0))
    {
        Status = -1;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��?�����ʱ���? */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("�澯��־��", "compidΪ��", "");
        return -1;
    }


    int   Numbers = 0;
    char  str[12][50];
    char  conver[50];
    memset(str, 0, 12 * 50);
    memset(conver, 0, sizeof(conver));

    //����������־Ŀ¼,����������Ŀ¼��ŵ�dir�У��Ա����ʹ�ø�Ŀ¼
    createDir(dir,  sizeof(dir));

    //��ѯ��ǰ�ͻ�����Ϣ
    nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
    char Mysql[256];
    memset(Mysql, 0, sizeof(Mysql));

    sprintf(Mysql, "select groupid from nwcompute where compid=%lu", compid);

    pasDbOneRecord(Mysql, 0, UT_TYPE_ULONG, sizeof(groupid2), &groupid2);

    for(Numbers = 0; Numbers < Nums; Numbers++)
    {
        snprintf(str[0], 50, "mac%d", Numbers);
        snprintf(str[1], 50, "groupid%d", Numbers);
        snprintf(str[2], 50, "devname%d", Numbers);
        snprintf(str[3], 50, "username%d", Numbers);
        snprintf(str[4], 50, "sip%d", Numbers);
        snprintf(str[5], 50, "stime%d", Numbers);
        snprintf(str[6], 50, "screenid%d", Numbers);
        snprintf(str[7], 50, "pid%d", Numbers);
        snprintf(str[8], 50, "mesg%d", Numbers);

        iReturn = utMsgGetSomeNVar(psMsgHead, 9,
                                   str[0],  UT_TYPE_STRING, sizeof(mac) - 1, mac,
                                   str[1],  UT_TYPE_LONG,   4,  &groupid,
                                   str[2], UT_TYPE_STRING,  sizeof(devname) - 1,  devname,
                                   str[3], UT_TYPE_STRING, sizeof(username) - 1, username,
                                   str[4], UT_TYPE_LONG,   4,  &sip,
                                   str[5], UT_TYPE_LONG,   4, &stime,
                                   str[6], UT_TYPE_LONG, 4, &screenid,
                                   str[7],  UT_TYPE_LONG, 4, &pid,
                                   str[8], UT_TYPE_STRING, sizeof(mesg) - 1,  mesg
                                  );

        char  mycompid[512];
        memset(mycompid, 0 , sizeof(mycompid));
        snprintf(mycompid, sizeof(mycompid) - 1, " compid=%u ,stime=%u",  compid, stime);
        if(sip == 0)
        {
            recodeLogInfo(" �յ�һ����Ļ�澯��־:", mycompid, "ip = 0");
        }
        if(stime == 0)
        {
            recodeLogInfo(" �յ�һ����Ļ�澯��־:", mycompid,  "Stime == 0");
        }
        if(screenid == 0)
        {
            recodeLogInfo(" �յ�һ����Ļ�澯��־:", mycompid,  "screenid== 0");
        }
        if(strlen(mac) <= 0)
        {
            recodeLogInfo(" ��??һ����Ļ�澯��?", mycompid, "mac== NULL");
        }
        if(strlen(devname) <= 0)
        {
            recodeLogInfo(" �յ�һ����Ļ�澯???", mycompid,  "devname== NULL");
        }
        if(strlen(mesg) <= 0)
        {
            recodeLogInfo(" �յ�һ����Ļ�澯��־:", mycompid,  "mesg== NULL");
        }


        //�������ݵ����ݿ���
        unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��

        if(psComp != NULL)
        {
            userid = psComp->userid;
            groupid = psComp->groupid;
        }
        //groupid = groupid2;


        time_t  now;
        time(&now);
        timeToString(now, year, mon, day);

        //printf("current time = %s%s,  compid = %u, groupid = %u, userid=%u\n", year, mon, compid,  groupid,userid);

        snprintf(sql, sizeof(sql) - 1,  "insert into nwwarnlog_%s%s( compid, userid, groupid,  devname, udisp, mac, sip, stime, screenid,pid, mesg,  sid) values(%u, %u, %u, \'%s\', \'%s\', \'%s\', %u, %u, %u, %u,  \'%s\', %llu)",
                 year, mon, compid, userid,  groupid, devname, username, mac, sip, stime, screenid, pid,  mesg, currentTime);
        //printf("sql = %s\n",  sql);

        iReturn = pasDbExecSql(sql, 0);
        //printf("iReturn = %d\n", iReturn );
        if(iReturn != 0)
        {
            iReturn = pasDbExecSqlF("create table nwwarnlog_%s%s ( \
    									sid  	  bigint auto_increment  primary key,   \
     									compid    int unsigned,\
									userid    int unsigned default 0,     \
	 								groupid   int unsigned default 0,\
									devname   char(32)  default ' ',      \
									udisp     char(32)  default '',       \
    									mac       char(20)  default '',       \
   			 						sip	  int unsigned, \
    									stime     int unsigned,\
    									screenid  int unsigned,\
    									pid       int unsigned,\
    									mesg   	  char(256)\
									);", year, mon);
            iReturn = pasDbExecSql(sql, 0);
            //printf("iReturn = %u,\n", iReturn);

        }

        if(0 == iReturn)
        {
            Nums2++;
        }
        else
        {
            break;
        }

    }//end for()


    if(Nums > Nums2)
    {
        Nums2 = 0;
        char findError1[512];
        memset(findError1, 0, sizeof(findError1));
        snprintf(findError1, sizeof(findError1) - 1, "compid=%d, Status = %d, Nums=%d, Nums2=%d", compid, Status, Nums, Nums2);

        recodeLogInfo("�澯��־��", findError1, "");
    }
    else
    {
        Status = 1;
    }

    //�����澯��Ļ��
    iReturn = pasDbExecSqlF("create table nwwarnscreen_%s%s ( \
    									  	sid       bigint auto_increment  primary key,\
  										warnid    int unsigned,\
    										stime     int,\
    										file      char(80)\
     										);", year, mon);



    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Nums",     UT_TYPE_LONG,     Nums2);
    return 0;
}




//10���澯JPGͼƬ�ϴ�
int Lan_ScreenAlterJPG_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int compid = 0;
    unsigned int userid   = 0;
    unsigned int screenid = 0;
    unsigned int stime = 0;
    char         destDir[512];
    unsigned long long currentTime = pasGetLongTimeId(); // �ȡʱ�?
    char         *buffer = NULL;
    unsigned int   bufferLength = 0;

    char    sql[20480];
    int     iRet = -1;
    int     Status = 0;
    int     Nums2 = 0;
    int     iReturn = -1;

    memset(destDir, 0, sizeof(destDir));
    memset(sql, 0, sizeof(sql));


    buffer = (char *)malloc(JPG_MAXLEN + 1);
    if(!buffer)
    {
        Status = -3;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 2,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1);
        recodeLogInfo("���յ�һ����Ļ�澯��ͼ��", "��̬���ٿռ�ʧ��", "");
        return -1;
    }
    else
    {
        memset(buffer, 0, JPG_MAXLEN + 1);
    }


    //utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead, 6,
                               "compid",  UT_TYPE_LONG,  4, &compid,
                               "userid",  UT_TYPE_LONG,  4, &userid,
                               "screenid",    UT_TYPE_LONG,  4, &screenid,
                               "currentTime", UT_TYPE_LONG, 4, &stime,
                               "buffer",  UT_TYPE_STRUCT, JPG_MAXLEN, buffer,
                               "bufferLength", UT_TYPE_LONG, 4, &bufferLength
                              );

    char  year[10];
    char  mon[10];
    char  day[10];

    memset(year, 0, sizeof(year));
    memset(mon, 0, sizeof(mon));
    memset(day, 0, sizeof(day));

    //�ѿͻ��˵�ǰʱ��ת�����ַ����� ����ɹ����� ��Ϊ��Ļ?�Ƭ���Ŀ¼�������÷������ĵ�ǰʱ�?����?��Ŀ�?
    if(timeToString(stime, year, mon, day) == -1)
    {
        time_t now;
        time(&now);
        timeToString(now, year, mon, day);
    }


    //����ͼƬ���Ŀ¼
    snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/" , SCREENALTERJPG_DIR, compid, year, mon, day);
    iReturn = mkdir(destDir, S_IRWXU);

    if(iReturn != 0)
    {
        snprintf(destDir, sizeof(destDir) - 1,  "%s", SCREENALTERJPG_DIR);
        iReturn = mkdir(destDir, S_IRWXU);

        snprintf(destDir, sizeof(destDir) - 1,  "%s%u/", SCREENALTERJPG_DIR, compid);
        iReturn = mkdir(destDir, S_IRWXU);

        snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/", SCREENALTERJPG_DIR, compid, year, mon, day);
        iReturn = mkdir(destDir, S_IRWXU);
    }

    //ͼƬ���·��
    snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/%u.jpg", SCREENALTERJPG_DIR, compid, year, mon, day, stime);
    //printf("destDir=%s\n", destDir);

    char  myreturn[512];
    memset(myreturn, 0, sizeof(myreturn));
    snprintf(myreturn, sizeof(myreturn) - 1, "compid=%u, ireturn = %d, destDir=%s", compid, iReturn, destDir);
    recodeLogInfo("���յ�һ����Ļ�澯��ͼ, ����·�� ��", myreturn, "");


    FILE *fp = fopen(destDir,   "ab");
    if(NULL == fp)
    {
        char   receive[1024];
        snprintf(receive, sizeof(receive) - 1,  "compid=%u,  ��ͼƬ���?��? %s", compid, destDir);
        recodeLogInfo("error�����յ�һ?���Ļ�澯���? ��?��?��ʱʧ��?��", receive, "");

        Status = -1;

    }
    else
    {
        int saveLength = fwrite(buffer, 1, bufferLength, fp);
        fclose(fp);

        if((bufferLength == 0) || (saveLength != bufferLength))
        {
            char   receive[1024];
            char   save[1024];
            snprintf(receive, sizeof(receive) - 1,  "compid=%u,  ���մ�С: %d", compid, bufferLength);
            snprintf(save, sizeof(save) - 1,  "�洢��С: %d", saveLength);
            recodeLogInfo("error�����յ�һ���澯��־", receive, save);

            Status = 0;
        }
        else
        {

            time_t  now;
            struct tm *time1;
            time(&now);
            time1 = localtime(&now);

            //��ǰ���
            snprintf(year, 5, "%d", time1->tm_year + 1900);
            //��ǰ�·�
            if((time1->tm_mon + 1 > 0) && (time1->tm_mon + 1 <= 9))
                snprintf(mon, 3, "0%d", time1->tm_mon + 1);
            else
                snprintf(mon, 3, "%d", time1->tm_mon + 1);

            snprintf(sql, sizeof(sql) - 1,  "insert into nwwarnscreen_%s%s( warnid, stime, file, sid) values(%u, %u, \'%s\', %llu)", year, mon, screenid, stime, destDir, currentTime);
            //printf("sql = %s\n",  sql);

            iReturn = pasDbExecSql(sql, 0);
            //printf("iReturn = %d\n", iReturn );
            if(iReturn != 0)
            {
                iReturn = pasDbExecSqlF("create table nwwarnscreen_%s%s ( \
    									  	sid       bigint auto_increment  primary key,\
  										warnid    int unsigned,\
    										stime     int,\
    										file      char(80)\
     										);", year, mon);
                iReturn = pasDbExecSql(sql, 0);
                //printf("iReturn = %u,\n", iReturn);
            }

            if(0 == iReturn)
            {
                Status = 1;
            }
            else
            {
                Status = -2;
                recodeLogInfo("���յ�һ����Ļ�澯��ͼ:", sql,  "");
            }
        }
    }

    //printf("Status = %d\n",   Status);
    char   receive[1024] = {0};
    snprintf(receive, sizeof(receive) - 1,  "compid=%u, stime=%u, status=%d, path=%s", compid, stime, Status, destDir);
    recodeLogInfo(" ���յ�һ����Ļ�澯��ͼ��", receive, "");

    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ��?? */ 2,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1);
    return 0;
}

int Lan_ProcessJPG_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int compid = 0;
    unsigned int userid   = 0;
    unsigned int screenid = 0;
    unsigned int   stime = 0;
    char         destDir[512];
    unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
    char          *buffer = NULL;
    unsigned int   bufferLength = 0;

    char    sql[20480];
    int     iRet = -1;
    int     Status = 0;
    int     Nums2 = 0;
    int     iReturn = -1;

    memset(destDir, 0, sizeof(destDir));
    memset(sql, 0, sizeof(sql));


    //utMsgPrintMsg(psMsgHead);

    buffer = (char *)malloc(JPG_MAXLEN + 1);
    if(!buffer)
    {
        Status = -3;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 2,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1);

        recodeLogInfo(" error�����յ�һ�����̽�ͼ:", "��̬���ٿռ�ʧ��", "");
        return -1;
    }
    else
    {
        memset(buffer, 0, JPG_MAXLEN + 1);
    }

    //��ȡ���յ�����Ϣ
    iReturn = utMsgGetSomeNVar(psMsgHead, 6,
                               "compid",  UT_TYPE_LONG,  4, &compid,
                               "userid",  UT_TYPE_LONG,  4, &userid,
                               "screenid",    UT_TYPE_LONG,  4, &screenid,
                               "stime", UT_TYPE_LONG, 4, &stime,
                               "buffer",  UT_TYPE_STRUCT, JPG_MAXLEN, buffer,
                               "bufferLength", UT_TYPE_LONG, 4, &bufferLength
                              );

    char  year[10];
    char  mon[10];
    char  day[10];

    memset(year, 0, sizeof(year));
    memset(mon, 0, sizeof(mon));
    memset(day, 0, sizeof(day));


    //�ѿͻ��˵�ǰʱ��ת�����ַ����� ����ɹ����� ��Ϊ?�ĻͼƬ���Ŀ¼�������÷������ĵ�ǰʱ����������Ŀ�?
    if(timeToString(stime, year, mon, day) == -1)
    {
        time_t now;
        time(&now);
        timeToString(now, year, mon, day);
    }


    //����ͼƬ���Ŀ¼
    snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/", PROCESSJPG_DIR, compid, year, mon, day);
    iReturn = mkdir(destDir, S_IRWXU);

    if(iReturn != 0)
    {
        snprintf(destDir, sizeof(destDir) - 1,  "%s", PROCESSJPG_DIR);
        iReturn = mkdir(destDir, S_IRWXU);

        snprintf(destDir, sizeof(destDir) - 1,  "%s%u/", PROCESSJPG_DIR, compid);
        iReturn = mkdir(destDir, S_IRWXU);

        snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/", PROCESSJPG_DIR, compid, year, mon, day);
        iReturn = mkdir(destDir, S_IRWXU);
    }



    //ͼƬ���·��
    snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/%u.jpg", PROCESSJPG_DIR, compid, year, mon, day, currentTime);
    //printf("destDir=%s\n", destDir);

    FILE *fp = fopen(destDir,   "ab");
    if(NULL == fp)
    {
        char   receive[1024];
        snprintf(receive, sizeof(receive) - 1,  "compid=%u,  ��ͼƬ���·��: %s", compid, destDir);
        recodeLogInfo(" error�����յ�һ�����̽�ͼ:", receive, "");

        Status = -1;

    }
    else
    {
        int saveLength = fwrite(buffer, 1, bufferLength,  fp);
        fclose(fp);

        if((bufferLength == 0) || (saveLength < bufferLength))
        {
            char   receive[1024];
            char   save[1024];
            snprintf(receive, sizeof(receive) - 1,  "compid=%u,  ���մ�С: %d", compid, bufferLength);
            snprintf(save, sizeof(save) - 1,  "�洢��С: %d", saveLength);
            recodeLogInfo("error�����յ�һ�����̽�ͼ:", receive, save);

            Status = 0;
        }
        else
        {
            time_t  now;
            struct tm *time1;
            time(&now);
            time1 = localtime(&now);

            //��ǰ���
            snprintf(year, 5, "%d", time1->tm_year + 1900);
            //��ǰ�·�
            if((time1->tm_mon + 1 > 0) && (time1->tm_mon + 1 <= 9))
                snprintf(mon, 3, "0%d", time1->tm_mon + 1);
            else
                snprintf(mon, 3, "%d", time1->tm_mon + 1);

            snprintf(sql, sizeof(sql) - 1,  "insert into nwprocscreen_%s%s( procid, stime, file, sid) values(%u, %u, \'%s\', %llu)", year, mon, screenid, stime, destDir, currentTime);
            //printf("sql = %s\n",  sql);

            iReturn = pasDbExecSql(sql, 0);
            //printf("iReturn = %d\n", iReturn );
            if(iReturn != 0)
            {
                iReturn = pasDbExecSqlF("create table nwprocscreen_%s%s ( \
    									  	sid       bigint auto_increment  primary key,\
  										procid    int unsigned,\
    										stime     int,\
    										file      char(80)\
     										);", year, mon);
                iReturn = pasDbExecSql(sql, 0);
                //printf("iReturn = %u,\n", iReturn);
            }

            if(0 == iReturn)
            {
                Status = 1;
            }
            else
            {
                Status = -1;
            }
        }
    }

    //printf("Status = %d\n",   Status);

    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 2,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1);

    free(buffer);
    return 0;
}

int Lan_MacRunInfo_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int   compid = 0;  //-- �����ID
    unsigned int   userid = 0;  //-- �û�ID
    char        mac[20];        //-- MAC��ַ
    unsigned int   groupid = 0; //-- ����Id
    unsigned int   groupid2 = 0;    //-- ����Id
    char        devname[32];    //-- �豸��
    char        username[25];   //-- �û���
    unsigned int    startmactime;    //-- ����ʱ��
    unsigned int    idletimelen;     //-- ����ʱ��
    unsigned int    activetime; //-- �ʱ��
    unsigned int    closemactime;   //-- �ػ�ʱ��

    char  Status = 0;
    long  iReturn = 0;

    char    mesg[512];


    recodeLogInfo(" ���յ��Կ��ػ���־1��" , "", "");

    memset(mac, 0, sizeof(mac));
    memset(devname, 0, sizeof(devname));
    memset(username, 0, sizeof(username));
    memset(mesg, 0, sizeof(mesg));

    iReturn = utMsgGetSomeNVar(psMsgHead, 10,
                               "compid",   UT_TYPE_ULONG, 4, &compid,
                               "userid",  UT_TYPE_ULONG,  4, &userid,
                               "mac",    UT_TYPE_STRING,  sizeof(mac) - 1, mac,
                               "groupid", UT_TYPE_LONG,  4, &groupid ,
                               "devname", UT_TYPE_STRING,  sizeof(devname) - 1, devname,
                               "udisp", UT_TYPE_STRING,  sizeof(username) - 1, username,
                               "startmactime", UT_TYPE_LONG,  4, &startmactime,
                               "idletimelen", UT_TYPE_LONG,  4, &idletimelen,
                               "activetime", UT_TYPE_LONG,  4, &activetime,
                               "closemactime", UT_TYPE_LONG,  4, &closemactime
                              );

    char  string[2048];
    snprintf(string, sizeof(string) - 1, "compid=%u, userid=%d", compid, userid);
    recodeLogInfo(" ���յ��Կ��ػ���־2��" , string, "");

    if(CheckIP(iFd) != 1)
    {
        Status = 1;

        pasTcpResponse(iFd, psMsgHead, NULL, 2,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1);

        recodeLogInfo("���Կ��ػ���־��" , "���˸���Ϣ��", "");
        return 0;
    }

    if(compid == 0)
    {
        Status = -1;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 2,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1);

        recodeLogInfo("���Կ��ػ���־��" , "compidΪ0", "");
        return -1;
    }

    //��ѯ��ǰ�ͻ�����Ϣ
    char sql[1024];
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "select groupid from nwcompute where compid=%lu", compid);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(groupid2), &groupid2);

    //  groupid2 = groupid;
    recodeLogInfo(" ���յ��Կ��ػ���־3��" , "", "");

    time_t  now;
    struct tm *time1;
    time(&now);
    time1 = localtime(&now);

    char    year[24];
    char    mon[24];
    memset(year, 0, sizeof(year));
    memset(mon, 0, sizeof(mon));
    //��ǰ���
    snprintf(year, 5, "%d", time1->tm_year + 1900);
    //?�ǰ�·?
    if((time1->tm_mon + 1 > 0) && (time1->tm_mon + 1 <= 9))
        snprintf(mon, 3, "0%d", time1->tm_mon + 1);
    else
        snprintf(mon, 3, "%d", time1->tm_mon + 1);

    int myCompid = 0;
    snprintf(sql, sizeof(sql) - 1, "select compid  from  nwcomputelog_%s%s   where compid=%u and  startmactime=%u",  year, mon, compid,  startmactime);
    iReturn = pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &myCompid);
    if((iReturn == 0) && (myCompid != 0))
    {
        snprintf(sql, sizeof(sql) - 1, "update nwcomputelog_%s%s  set userid=%u, mac= \'%s\',  groupid=%u,  devname= \'%s\',  udisp= \'%s\',  idletime=%u, activetime=%u, closemactime=%u  where compid=%u and startmactime=%u",
                 year, mon, userid, mac, groupid2, devname, username, idletimelen, activetime, closemactime, compid, startmactime);

        iReturn = pasDbExecSql(sql, 0);

        snprintf(mesg, sizeof(mesg) - 1, "iReturn:%d");
        recodeLogInfo(" ���յ��Կ��ػ���־4��" , "update", mesg);
    }
    else
    {
        snprintf(sql, sizeof(sql) - 1,  "insert into nwcomputelog_%s%s( compid, userid, mac, groupid, devname, udisp, startmactime, idletime, activetime, closemactime) "
                 "values(%u,%u, \'%s\',%u,\'%s\',\'%s\',%u, %u, %u, %u)",
                 year, mon, compid, userid, mac, groupid2, devname, username, startmactime, idletimelen, activetime, closemactime);
        iReturn = pasDbExecSql(sql, 0);

        recodeLogInfo(" ���յ��Կ��ػ���־4��" , "insert", "");
    }

    if(iReturn == 0)
    {
        Status = 1;
    }

    //  recodeLogInfo(" ���յ��Կ��ػ���־5.? , "", "");
    pasTcpResponse(iFd, psMsgHead, NULL, 2,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status);

    //  recodeLogInfo(" ���յ��Կ��ػ���־6��" , "", "");
    return 0;
}

int Lan_MailFile_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int    compid = 0; //-- �����ID
    unsigned int    len = 0;
    char    extension[24];
    char    filepath[1024];
    char *  data = NULL;
    char    mesg[512];

    char  Status = 0;
    long  iReturn = 0;

    memset(mesg, 0, sizeof(mesg));
    memset(extension, 0, sizeof(extension));
    memset(filepath, 0, sizeof(filepath));

    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "compid",   UT_TYPE_ULONG, 4, &compid,
                               "len",  UT_TYPE_ULONG,  4, &len,
                               "extension",    UT_TYPE_STRING,  sizeof(extension) - 1, extension);

    snprintf(mesg, sizeof(mesg) - 1, "�����ʼ��ļ�1��compid:%u,len:%d, extension:%s", compid, len, extension);
    recodeLogInfo(mesg, "", "");


    if(CheckIP(iFd) != 1)
    {
        Status = 1;

        pasTcpResponse(iFd, psMsgHead, NULL, 2,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1);

        recodeLogInfo("�����ʼ��ļ���" , "���˸���Ϣ��", "");
        return 0;
    }

    if((len <= 0) || (compid == 0))
    {
        Status = -1;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 2,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1);

        //recodeLogInfo(" �� �ʼ��ļ�? , "compid ? || filelen<=0", "");
        return -1;
    }

    data = malloc(len + 2);
    if(data == NULL)
    {
        Status = -2;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 2,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1);

        recodeLogInfo("�����ʼ��ļ���" , "��̬���ٿռ�ʧ��", "");
        return -1;

    }

    //�����ݰ�����ȡdata�ֶ�.��?
    memset(data, 0, len + 2);
    iReturn = utMsgGetSomeNVar(psMsgHead, 1,    "data",    UT_TYPE_STRING,  len + 1, data);

    //filepathΪ�ļ�����·��
    snprintf(filepath, sizeof(filepath) - 1, "%s",  EMAIL_DIR);
    mkdir(filepath, S_IRWXU);

    snprintf(filepath, sizeof(filepath) - 1, "%s%u",  EMAIL_DIR,  compid);
    mkdir(filepath, S_IRWXU);

    unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��
    snprintf(mesg, sizeof(mesg) - 1, "�����ʼ��ļ�2��currentTime:%llu", currentTime);
    recodeLogInfo(mesg, "", "");

    time_t  now;
    struct tm *time1;
    time(&now);
    time1 = localtime(&now);

    char    year[24];
    char    mon[24];
    memset(year, 0, sizeof(year));
    memset(mon, 0, sizeof(mon));
    //��ǰ���
    snprintf(year, 5, "%d", time1->tm_year + 1900);
    //��ǰ�·�
    if((time1->tm_mon + 1 > 0) && (time1->tm_mon + 1 <= 9))
    {
        snprintf(mon, 3, "0%d", time1->tm_mon + 1);
    }
    else
    {
        snprintf(mon, 3, "%d", time1->tm_mon + 1);
    }

    snprintf(filepath, sizeof(filepath) - 1, "%s%u/%s%s",  EMAIL_DIR,  compid,  year, mon);
    mkdir(filepath, S_IRWXU);

    snprintf(filepath, sizeof(filepath) - 1, "%s%u/%s%s/%llu.%s",  EMAIL_DIR,  compid,  year, mon, currentTime, extension);
    FILE *fp =  fopen(filepath, "w");
    if(!fp)
    {
        Status = -3;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 2,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1);

        recodeLogInfo("�����ʼ��ļ���" , "���ļ�ʧ��", filepath);
        return -1;
    }

    //��data��Ϣ���浽�ļ���
    int length = fwrite(data, 1, len, fp);
    fclose(fp);

    if(len != length)
    {
        Status = -4;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 2,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1);

        snprintf(mesg, sizeof(mesg) - 1, "ԭ����;%d,  ���泤��:%d",   len,  length);
        recodeLogInfo("�����ʼ��ļ���" , "�ʼ���������������ݶ�ʧ", mesg);
        return -1;
    }

    snprintf(mesg, sizeof(mesg) - 1, "�����ʼ��ļ�3���ɹ������ʼ�");
    recodeLogInfo(mesg, "", "");


    Status = 1;
    pasTcpResponse(iFd, psMsgHead, NULL, 3,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "serverFilepath", UT_TYPE_STRING,  filepath);

    return 0;
}

int Lan_EmailLog_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int    compid = 0;     //-- ����ID
    unsigned int    groupid = 0;    //-- ��ID
    unsigned int    userid = 0;     //-- �û�ID
    char        company[32];        //-- ��˾��
    int     stime = 0;          //-- ��ʼʱ��(���ã�
    char        sip[15];            //-- ԴIP(���ã�
    char        dip[15];            //-- Ŀ��IP��ַ(���ã�

    char        mailfrom[255];      //-- ������(���ã�
    char        mailto[255];        //-- �ռ���(���ã�
    char        mailsub[255];       //-- ����(���ã�
    char        host[255];      //-- ����(���ã�
    int     mailasize = 0;      //-- ������С(���ã�
    int     mailsize = 0;       //-- �ʼ���С(���ã�
    int     flags = 0;      //-- �ʼ�״̬ 1--SMTP  2--POP3  3--WebMail(���ã�
    int     ccode = 0;      //-- ����(���ã�
    char        affixinfo[256];     //������Ϣ
    char        bodyinfo[128];      //--   ������Ϣ
    char        filepath[255];      //--����ʼ��ļ���(���ã�

    char    Status = 0;
    int iReturn = 0;
    int Nums = 0;
    int Nums2 = 0;

    memset(sip, 0, sizeof(sip));
    memset(dip, 0, sizeof(dip));
    memset(mailfrom, 0, sizeof(mailfrom));
    memset(mailto,  0, sizeof(mailto));
    memset(mailsub, 0, sizeof(mailsub));
    memset(host, 0, sizeof(host));
    memset(affixinfo, 0, sizeof(affixinfo));
    memset(bodyinfo, 0, sizeof(bodyinfo));
    memset(filepath, 0, sizeof(filepath));

    iReturn = utMsgGetSomeNVar(psMsgHead, 1, "Nums",    UT_TYPE_LONG,  4, &Nums);
    if(CheckIP(iFd) != 1)
    {
        Status = 1;
        Nums2 = Nums;

        pasTcpResponse(iFd, psMsgHead, NULL, 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("�ʼ���־��" , "���˸���Ϣ��", "");
        return 0;
    }

    if(Nums <= 0)
    {
        Status = -1;
        Nums2 = 0;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("�ʼ���־��" , "NumsΪ0", "");
        return -1;
    }


    char  str[18][50];
    memset(str, 0, 16 * 50);

    //��ȡ��Ϣ
    int    Numbers = 0;
    for(Numbers = 0; Numbers < Nums; Numbers++)
    {
        snprintf(str[0], 50, "compid%d", Numbers);
        snprintf(str[1], 50, "groupid%d", Numbers);
        snprintf(str[2], 50, "userid%d", Numbers);
        snprintf(str[3], 50, "company%d", Numbers);
        snprintf(str[4], 50, "stime%d", Numbers);
        snprintf(str[5], 50, "sip%d", Numbers);
        snprintf(str[6], 50, "dip%d", Numbers);
        snprintf(str[7], 50, "mailfrom%d", Numbers);
        snprintf(str[8], 50, "mailto%d", Numbers);
        snprintf(str[9], 50, "mailsub%d", Numbers);
        snprintf(str[10], 50, "host%d", Numbers);
        snprintf(str[11], 50, "mailasize%d", Numbers);
        snprintf(str[12], 50, "mailsize%d", Numbers);
        snprintf(str[13], 50, "flags%d", Numbers);
        snprintf(str[14], 50, "ccode%d", Numbers);
        snprintf(str[15], 50, "affixinfo%d", Numbers);
        snprintf(str[16], 50, "bodyinfo%d", Numbers);
        snprintf(str[17], 50, "filepath%d", Numbers);

        iReturn = utMsgGetSomeNVar(psMsgHead, 18,
                                   str[0],  UT_TYPE_ULONG, 4, &compid ,
                                   str[1],  UT_TYPE_ULONG,   4,  &groupid,
                                   str[2], UT_TYPE_ULONG,  4,  &userid,
                                   str[3], UT_TYPE_STRING,  sizeof(company) - 1,  company,
                                   str[4], UT_TYPE_ULONG, 4, &stime,
                                   str[5], UT_TYPE_STRING, sizeof(sip) - 1,  sip,
                                   str[6], UT_TYPE_STRING, sizeof(dip) - 1, dip,
                                   str[7], UT_TYPE_STRING, sizeof(mailfrom) - 1,  mailfrom,
                                   str[8], UT_TYPE_STRING, sizeof(mailto) - 1, mailto,
                                   str[9], UT_TYPE_STRING, sizeof(mailsub) - 1, mailsub,
                                   str[10], UT_TYPE_STRING, sizeof(host) - 1, host,
                                   str[11], UT_TYPE_ULONG, 4, &mailasize,
                                   str[12], UT_TYPE_ULONG, 4, &mailsize ,
                                   str[13], UT_TYPE_ULONG, 4, &flags ,
                                   str[14], UT_TYPE_ULONG, 4, &ccode ,
                                   str[15], UT_TYPE_STRING, sizeof(affixinfo) - 1, affixinfo,
                                   str[16], UT_TYPE_STRING, sizeof(bodyinfo) - 1, bodyinfo,
                                   str[17], UT_TYPE_STRING, sizeof(filepath) - 1, filepath);

        char mesg[512];
        memset(mesg, 0, sizeof(mesg));
        snprintf(mesg, sizeof(mesg) - 1, "�����ʼ���־3��mailsize:%d,  flags:%d" , mailsize, flags);
        recodeLogInfo(mesg, "", "");


        time_t  now;
        struct tm *time1;
        time(&now);
        time1 = localtime(&now);

        char    year[24];
        char    mon[24];
        memset(year, 0, sizeof(year));
        memset(mon, 0, sizeof(mon));
        //��ǰ���
        snprintf(year, 5, "%d", time1->tm_year + 1900);
        //��ǰ�·�
        if((time1->tm_mon + 1 > 0) && (time1->tm_mon + 1 <= 9))
        {
            snprintf(mon, 3, "0%d", time1->tm_mon + 1);
        }
        else
        {
            snprintf(mon, 3, "%d", time1->tm_mon + 1);
        }

        char sql[2048];
        memset(sql, 0, sizeof(sql));
        snprintf(sql, sizeof(sql) - 1,  "insert into ncsmailmon_if_%s%s(compid,groupid, userid, company, stime,sip, dip, mailfrom, mailto, mailsub, host, mailasize, mailsize, flags, ccode, affixinfo,bodyinfo,filepath)"
                 " values(%u,%u, %u, \'%s\',%u,\'%s\',\'%s\',  \'%s\',  \'%s\', \'%s\', \'%s\', %d, %d, %d, %d, \'%s\',\'%s\',\'%s\')",
                 year, mon,  compid, groupid, userid, company, stime, sip, dip, mailfrom, mailto, mailsub, host, mailasize, mailsize , flags , ccode , affixinfo, bodyinfo, filepath);
        if(pasDbExecSql(sql, 0) == 0)
        {
            Nums2++;
        }
        else
        {
            snprintf(mesg, sizeof(mesg) - 1, "�����ʼ���־ʧ��.�path:%s" , filepath);
            recodeLogInfo(mesg, "  �ʼ���ɾ����", "");

            remove(filepath);
        }
    }

    Status = 1;
    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                   "compid",   UT_TYPE_LONG,    compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Nums",     UT_TYPE_LONG,    Nums2);

    return 0;
}


int Lan_MyPorcessLog_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int   Nums   = 0;
    unsigned int   compid = 0;
    unsigned int   userid = 0;
    char  mac[50];
    unsigned int   groupid = 0;
    unsigned int    groupid2 = 0;
    char  devname[50];
    char  username[50];
    unsigned int   sip  = 0;
    char  procname[128];
    int      procpos = 0;

    unsigned int stime  = 0;
    char    file[80];
    char    destPath[512];
    char    sql[2048];

    char  Status  = 0;
    int   Nums2 = 0;
    long  iReturn = 0;
    FILE   *fp = NULL;
    char dir[512]; //����Ŀ¼
    char savefile[512];  //������ļ�??
    //utMsgPrintMsg(psMsgHead);

    char  year[10];
    char  mon[10];
    char  day[10];

    memset(year, 0, sizeof(year));
    memset(mon, 0 , sizeof(mon));
    memset(day, 0, sizeof(day));

    memset(mac, 0, sizeof(mac));
    memset(devname, 0, sizeof(devname));
    memset(username, 0, sizeof(username));
    memset(procname, 0, sizeof(procname));
    memset(file, 0, sizeof(file));
    memset(destPath, 0, sizeof(destPath));
    memset(sql,  0, sizeof(sql));
    memset(dir,  0, sizeof(dir));
    memset(savefile, 0, sizeof(savefile));

    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "compid",  UT_TYPE_LONG,  4, &compid,
                               "userid",  UT_TYPE_LONG,  4, &userid,
                               "Nums",    UT_TYPE_LONG,  4, &Nums
                              );

    if(CheckIP(iFd) != 1)
    {
        Status = 1;
        Nums2 = Nums;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��?�����ʱ���? */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("���н�����־��", "���˸���Ϣ", "");
        return 0;
    }


    if((compid == 0))
    {
        Status = 0;
        Nums2 = -3;

        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, 1,
                       "Nums",     UT_TYPE_LONG,     Nums2);

        recodeLogInfo("���н�����־��", "compidΪ��", "");
        return -1;
    }

    int   Numbers = 0;
    char  str[12][50];
    char  conver[50];
    memset(str, 0, 12 * 50);
    memset(conver, 0, sizeof(conver));

    //����������־?��?����������Ŀ¼��ŵ�dir�У��Ա����ʹ�ø�Ŀ¼
    createDir(dir,  sizeof(512));

    //��ѯ��ǰ�ͻ�����Ϣ
    nwCompInfo *psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
    char Mysql[256];
    memset(Mysql, 0, sizeof(Mysql));
    sprintf(Mysql, "select groupid from nwcompute where compid=%lu", compid);
    pasDbOneRecord(Mysql, 0, UT_TYPE_ULONG, sizeof(groupid2), &groupid2);

    for(Numbers = 0; Numbers < Nums; Numbers++)
    {
        snprintf(str[0], 50, "mac%d", Numbers);
        snprintf(str[1], 50, "groupid%d", Numbers);
        snprintf(str[2], 50, "devname%d", Numbers);
        snprintf(str[3], 50, "username%d", Numbers);
        snprintf(str[4], 50, "sip%d", Numbers);
        snprintf(str[5], 50, "Stime%d", Numbers);
        snprintf(str[6], 50, "file%d", Numbers);
        snprintf(str[7], 50, "procname%d", Numbers);
        snprintf(str[8], 50, "procpos%d", Numbers);

        iReturn = utMsgGetSomeNVar(psMsgHead, 9,
                                   str[0],  UT_TYPE_STRING, sizeof(mac) - 1, mac,
                                   str[1],  UT_TYPE_LONG,   4,  &groupid,
                                   str[2], UT_TYPE_STRING,  sizeof(devname) - 1,  devname,
                                   str[3], UT_TYPE_STRING, sizeof(username) - 1, username,
                                   str[4], UT_TYPE_LONG,   4,  &sip,
                                   str[5], UT_TYPE_LONG,   4, &stime,
                                   str[6], UT_TYPE_STRING, sizeof(file) - 1,  file,
                                   str[7], UT_TYPE_STRING, sizeof(procname) - 1,  procname,
                                   str[8], UT_TYPE_LONG,   4,  &procpos);


        char mystring[512];
        memset(mystring, 0, sizeof(mystring));
        snprintf(mystring, sizeof(mystring) - 1, " compid=%u, stime=%u", compid, stime);

        if(sip == 0)
        {
            recodeLogInfo(" �յ�һ�����н�����־", mystring, "ip = 0");
        }
        if(stime == 0)
        {
            recodeLogInfo(" �յ�һ�����н�����־:", mystring, "Stime == 0");
        }
        if(strlen(mac) <= 0)
        {
            recodeLogInfo(" �յ�һ�����н�����־:", mystring, "mac== NULL");
        }
        if(strlen(devname) <= 0)
        {
            recodeLogInfo(" �յ�һ�����н�����־:", mystring, "devname== NULL");
        }
        if(strlen(file) <= 0)
        {
            recodeLogInfo(" �յ�һ�����н�����־:", mystring, "file== NULL");
        }


        if(timeToString(stime, year, mon, day) == -1)
        {
            time_t now;
            time(&now);
            timeToString(now, year, mon, day);
        }
        snprintf(destPath, sizeof(destPath) - 1,  "%s%u/%s%s%s/%u.jpg", SENSITIVEPROCJPG_DIR, compid, year, mon, day, stime);
        recodeLogInfo("���н�����־��",  destPath, "");


        //�������ݵ����ݿ���
        unsigned long long currentTime = pasGetLongTimeId(); //��ȡʱ��

        if(psComp != NULL)
        {
            userid = psComp->userid;
            groupid = psComp->groupid;
        }
        //groupid = groupid2;

        int   count = 0;
        snprintf(sql, sizeof(sql) - 1, "select  count(*)  from nwprocscreen_%s%s  where  compid=%u and  proscreen=\'%s\' and  stime=%u",   year, mon,  compid,  procname,  stime);
        pasDbOneRecord(sql, 0, UT_TYPE_ULONG, sizeof(count), &count);
        if(count > 0)
        {
            recodeLogInfo("���н�����־��",  "count>0", sql);
            Nums2++;
            continue;
        }
        else
        {
            recodeLogInfo("���н�����־��",  "count <= 0", sql);
        }

        snprintf(sql, sizeof(sql) - 1,  "insert into nwprocscreen_%s%s( compid, userid, groupid,  devname, udisp, mac, proscreen, sip, stime, file,mousepos, sid) "
                 " values(%u, %u, %u, \'%s\', \'%s\', \'%s\', \'%s\', %u, %u, \'%s\',%u,  %llu)",
                 year, mon, compid, userid,  groupid, devname, username, mac, procname, sip, stime, destPath, procpos,  currentTime);
        iReturn = pasDbExecSql(sql, 0);
        if(0 == iReturn)
        {
            Nums2++;
        }
        else
        {
            //recodeLogInfo("���н�����־��",  "������־ʧ ?, sql);
        }

    }//end for()

    if(Nums != Nums2)
    {
        Nums2 = 0;
        char findError1[512];
        memset(findError1, 0, sizeof(findError1));
        snprintf(findError1, sizeof(findError1) - 1, "compid=%u, Nums=%d,Nums2=%d", compid, Nums, Nums2);
        recodeLogInfo("���н�����־��", findError1, "");
    }
    else
    {
        Status = 1;
    }

    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ���� ����? */ 3,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, 1,
                   "Nums",     UT_TYPE_LONG,     Nums2);
    return 0;
}




//8����ĻJPGͼƬ.??
int Lan_MyProcessJPG_Up(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned int compid = 0;
    unsigned int userid   = 0;
    char         souPath[512];
    char         destDir[512];
    unsigned int stime = 0;
    char        *buffer =  NULL;
    unsigned int   bufferLength = 0;

    int     iRet = -1;
    int     Status = 0;
    int     Nums2 = 0;
    int     iReturn = 0;

    memset(souPath, 0, sizeof(souPath));
    memset(destDir, 0, sizeof(destDir));

    //  utMsgPrintMsg(psMsgHead);

    buffer = (char *)malloc(JPG_MAXLEN + 1);
    if(!buffer)
    {
        Status = -3;
        stime = 0;
        pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                       "compid",   UT_TYPE_LONG,   compid,
                       "Status",   UT_TYPE_STRUCT,  &Status, Status,
                       "Stime",    UT_TYPE_LONG, stime
                      );

        recodeLogInfo("����һ����Ļ��ͼ��", "��̬���ٿռ�ʧ��", "");
        return -1;
    }
    else
    {
        memset(buffer, 0 , JPG_MAXLEN + 1);
    }


    iReturn = utMsgGetSomeNVar(psMsgHead, 6,
                               "compid",  UT_TYPE_LONG,  4, &compid,
                               "userid",  UT_TYPE_LONG,  4, &userid,
                               "file",    UT_TYPE_STRING,  sizeof(souPath) - 1, souPath,
                               "currentTime", UT_TYPE_LONG, 4, &stime ,
                               "buffer",  UT_TYPE_STRUCT, JPG_MAXLEN, buffer,
                               "bufferLength", UT_TYPE_LONG, 4, &bufferLength
                              );

    char  year[10];
    char  mon[10];
    char  day[10];

    memset(year, 0, sizeof(year));
    memset(mon, 0, sizeof(mon));
    memset(day, 0, sizeof(day));

    //�ѿͻ��˵�ǰʱ��ת���� ַ�� ?��?��ɹ?��?��Ϊ��ĻͼƬ���Ŀ¼�������÷������ĵ�ǰʱ����������Ŀ¼
    if(timeToString(stime, year, mon, day) == -1)
    {
        time_t now;
        time(&now);
        timeToString(now, year, mon, day);
    }

    //����ͼƬ���Ŀ??
    snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/", SENSITIVEPROCJPG_DIR, compid, year, mon, day);
    iReturn = mkdir(destDir, S_IRWXU);

    if(iReturn != 0)
    {
        snprintf(destDir, sizeof(destDir) - 1,  "%s", SENSITIVEPROCJPG_DIR);
        iReturn = mkdir(destDir, S_IRWXU);

        snprintf(destDir, sizeof(destDir) - 1,  "%s%u/", SENSITIVEPROCJPG_DIR, compid);
        iReturn = mkdir(destDir, S_IRWXU);

        snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/", SENSITIVEPROCJPG_DIR, compid, year, mon, day);
        iReturn = mkdir(destDir, S_IRWXU);
    }


    //ͼƬ���·��
    snprintf(destDir, sizeof(destDir) - 1,  "%s%u/%s%s%s/%u.jpg", SENSITIVEPROCJPG_DIR, compid, year, mon, day, stime);
    //printf("destDir=%s\n", destDir);

    FILE *fp = fopen(destDir,   "wb");
    if(NULL == fp)
    {
        Status = -1;
        recodeLogInfo(" ���յ�һ�����н��̽�ͼ��", "���ļ�ʧ��", destDir);

    }
    else
    {
        int saveLength = fwrite(buffer, 1, bufferLength, fp);
        fclose(fp);

        if((bufferLength == 0) || (saveLength < bufferLength))
        {
            char   receive[1024];
            char   save[1024];
            memset(receive, 0, sizeof(receive) - 1);
            memset(save, 0, sizeof(save) - 1);
            snprintf(receive, sizeof(receive) - 1,  "compid=%u,  ���մ�С: %d", compid, bufferLength);
            snprintf(save, sizeof(save) - 1,  "�洢��С: %d", saveLength);
            recodeLogInfo("error�����յ�һ�����н��̽�ͼ��", receive, save);
            Status = 0;
        }
        else
        {
            Status = 1;
            char   receive[1024];
            char   save[1024];
            memset(receive, 0, sizeof(receive) - 1);
            memset(save, 0, sizeof(save) - 1);
            snprintf(receive, sizeof(receive) - 1,  "compid=%u, stime: %u,  filepath=%s", compid, stime, destDir);
            //snprintf(save, sizeof(save)-1,  "�洢?��? %d", saveLength);
            //recodeLogInfo(" ���յ�һ����.н��̽�ͼ�?, receive, "");

        }
        Status = 1;
    }

    //printf("Status = %d\n",   Status);
    pasTcpResponse(iFd, psMsgHead, NULL, /* ��Կ����ʱ����  */ 3,
                   "compid",   UT_TYPE_LONG,   compid,
                   "Status",   UT_TYPE_STRUCT,  &Status, Status,
                   "Stime",    UT_TYPE_LONG, stime
                  );

    free(buffer);
    return 0;
}

int ncInitNwWebFun_Uplog(utShmHead *psShmHead)
{
    pasSetTcpFunName("Lan_FileLog_Up",     Lan_FileLog_Up,    0);
    pasSetTcpFunName("Lan_PorcessLog_Up",  Lan_PorcessLog_Up, 0);
    pasSetTcpFunName("Lan_ProcessJPG_Up",  Lan_ProcessJPG_Up, 0);
    pasSetTcpFunName("Lan_RmoveLog_Up",    Lan_RmoveLog_Up,   0);
    pasSetTcpFunName("Lan_WebLog_Up",      Lan_WebLog_Up,     0);
    pasSetTcpFunName("Lan_ChatLog_Up",     Lan_ChatLog_Up,    0);
    pasSetTcpFunName("Lan_ScreenLog_Up",   Lan_ScreenLog_Up,     0);
    pasSetTcpFunName("Lan_ScreenJPG_Up",   Lan_ScreenJPG_Up,     0);
    pasSetTcpFunName("Lan_ScreenAlterLog_Up",   Lan_ScreenAlterLog_Up,     0);
    pasSetTcpFunName("Lan_ScreenAlterJPG_Up",   Lan_ScreenAlterJPG_Up,     0);
    pasSetTcpFunName("Lan_EventLog_Up",    Lan_EventLog_Up,   0);
    pasSetTcpFunName("Lan_MacRunInfo_Up",    Lan_MacRunInfo_Up,   0);
    pasSetTcpFunName("Lan_MailFile_Up",    Lan_MailFile_Up,   0);
    pasSetTcpFunName("Lan_EmailLog_Up",    Lan_EmailLog_Up,   0);
    pasSetTcpFunName("Lan_MyPorcessLog_Up",  Lan_MyPorcessLog_Up, 0);
    pasSetTcpFunName("Lan_MyProcessJPG_Up",  Lan_MyProcessJPG_Up, 0);
    /*
        pasSetTcpFunName("Lan_FileLog_Up",     Lan_FileLog_Up,    NULL,0,0);
        pasSetTcpFunName("Lan_PorcessLog_Up",  Lan_PorcessLog_Up, NULL,0,0);
        pasSetTcpFunName("Lan_ProcessJPG_Up",  Lan_ProcessJPG_Up, NULL,0,0);
        pasSetTcpFunName("Lan_RmoveLog_Up",    Lan_RmoveLog_Up,   NULL,0,0);
        pasSetTcpFunName("Lan_WebLog_Up",      Lan_WebLog_Up,     NULL,0,0);
        pasSetTcpFunName("Lan_ChatLog_Up",     Lan_ChatLog_Up,    NULL,0,0);
        pasSetTcpFunName("Lan_ScreenLog_Up",   Lan_ScreenLog_Up,     NULL,0,0);
        pasSetTcpFunName("Lan_ScreenJPG_Up",   Lan_ScreenJPG_Up,     NULL,0,0);
        pasSetTcpFunName("Lan_ScreenAlterLog_Up",   Lan_ScreenAlterLog_Up,     NULL,0,0);
        pasSetTcpFunName("Lan_ScreenAlterJPG_Up",   Lan_ScreenAlterJPG_Up,     NULL,0,0);
        pasSetTcpFunName("Lan_EventLog_Up",    Lan_EventLog_Up,   NULL,0,0);
        pasSetTcpFunName("Lan_MacRunInfo_Up",    Lan_MacRunInfo_Up,   NULL,0,0);
        pasSetTcpFunName("Lan_MailFile_Up",    Lan_MailFile_Up,   NULL,0,0);
        pasSetTcpFunName("Lan_EmailLog_Up",    Lan_EmailLog_Up,   NULL,0,0);
        pasSetTcpFunName("Lan_MyPorcessLog_Up",  Lan_MyPorcessLog_Up, NULL,0,0);
        pasSetTcpFunName("Lan_MyProcessJPG_Up",  Lan_MyProcessJPG_Up, NULL,0,0);
    */

    FILE *fp = NULL;
    char   buffer[2048];
    char  *first = NULL;
    char  *second = NULL;
    memset(buffer, 0, sizeof(buffer));
    memset(ConfIP, 0, sizeof(ConfIP));

    /*
        fp = fopen( CONF_FILE ,"r");
        if(fp == NULL){

        }else{
            fgets(buffer, sizeof(buffer)-1,  fp);

            first = strtok(buffer, " =");
            second = strtok(NULL, "   =");

            if(first  && second && (strcmp(first, "analyzerip")) ){
                snprintf(ConfIP, sizeof(ConfIP)-1, "%s", second);
                recodeLogInfo(second, ConfIP,"");

            }
            fclose(fp);
        }
    */
    return 0;
}
