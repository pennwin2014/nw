#define  PAS_SRCFILE      1111
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>
#include "string.h"
#include "utoall.h"
#include "ncdef.h"
#include "nwdef.h"
#include "pasdb.h"
#define WRITE_DEBUG
#define LAN_STATUS_CONNECT  10001
#define LAN_STATUS_RUNING   10002

void writelog(char  *caMsg)
{
    FILE *fp;
#ifdef WRITE_DEBUG
    long lTime = time(0);
    fp = fopen("/home/ncmysql/nw/log/", "a");
    if(fp)
    {
        fprintf(fp, "%s_%s\n", utTimFormat("%Y-%m-%d %H:%M:%S", lTime), caMsg);
        fclose(fp);
    }
#endif
}

typedef struct _mac_mem
{
    struct _mac_mem* prv;
    struct _mac_mem* next;
    char   compid[16];    //计算机ID
    char   compname[32];  //机器名
    int    userid;        //用户ID
    char   username[16];  //用户名
    char   ip[4];         //IP
    char   mac[18];       //MAC地址
    char   os[32];        //操作系统版本
    char   version[20];   //客户端版本号
    int    lasttime;      //最后活动时间
    int    command;       //命令状态
    int    lasttolib;     //最后入库时间,超过配置时间长度，修改入库标记
    char   flags;         //内容修改标志
} MAC_MEM, *pMAC_MEM;

struct index
{
    char       compid[16];
    pMAC_MEM   machine;

};

MAC_MEM*            m_plist = NULL;                        //链表
unsigned char*   m_index = NULL;                        //索引
pasLHashInfo     sHashInfo;                             //key 机器id  value mac_mem结构指针
pthread_mutex_t  mylock = PTHREAD_MUTEX_INITIALIZER;    //互斥锁
int              g_bExit = 0;                           //入库线程是否退出


int nwUtlGetUserid(utShmHead * psShmHead, unsigned long lIp, char *mac, char *compname)
{
    return 999;
}
int nwUtlGetGroupid(utShmHead * psShmHead, unsigned long lIp, char *mac, char *compname)
{
    char *pHash;
    pasHashInfo  sHashInfo;
    lanGroupInfo  *psHashData;
    pHash = (unsigned char *)utShmHashHead(psShmHead, NC_LNK_NW_GROUP);
    if(pHash == NULL)
    {
        return 0;
    }
    psHashData = (lanGroupInfo *)pasHashFirst(pHash, &sHashInfo);
    while(psHashData)
    {

        if(lIp >= psHashData->lBegip && lIp <= psHashData->lEndip)
        {
            return psHashData->lGroupid;
        }
        psHashData = (lanGroupInfo *)pasHashNext(&sHashInfo);
    }

    return 0;
}


int lanGetSoftUpdateId(utShmHead *psShmHead, long lVflag, unsigned long compid, char *pVer)
{
    nwCompInfo *psComp;
    lanSoftUpdate *pData;
    long lSid;
    struct key_s
    {
        unsigned long lVflag;
        unsigned long lObjtype;
        unsigned long lObjid;
    };
    struct key_s stKey;
    lSid = 0;
    psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
    if(psComp)
    {
        memset(&stKey, 0, sizeof(struct key_s));
        //类别为计算机
        stKey.lVflag = lVflag;
        stKey.lObjtype = 2;
        stKey.lObjid = compid;
        pData = (lanSoftUpdate*)utShmHashLook(psShmHead, NC_LNK_NW_SOFTUP, &stKey);

        if(pData)
        {
            recodeLogInfo("lanGetSoftUpdateId Log ...", "----------", "");
            char flag[16] = "";
            sprintf(flag, "%lu", compid);
            writeLog("softDown.txt", "compid", flag);
            recodeLogInfo("lanGetSoftUpdateId Log ...", "-----compid-----", flag);
            sprintf(flag, "%lu", pData->lSoftid);
            writeLog("softDown.txt", "softidNew", flag);
            recodeLogInfo("lanGetSoftUpdateId Log ...", "-----softidNew-----", flag);
            sprintf(flag, "%lu", psComp->stime);
            writeLog("softDown.txt", "softidOld", flag);
            recodeLogInfo("lanGetSoftUpdateId Log ...", "-----softidOld-----", flag);
            char mesg[512] = "";
            snprintf(mesg, sizeof(mesg), "lVflag=%lu,caVersion=%s,pVer=%s", lVflag, pData->caVersion, pVer);
            recodeLogInfo("lanGetSoftUpdateId Log ...", "----------", mesg);
            if((pData->lSoftid) > (psComp->stime) && strcmp(pData->caVersion, pVer) != 0)
            {
                //if(strcmp(pData->caVersion,pVer)!=0){
                lSid = pData->lSoftid;
            }
        }

        // 判断通用类别符合不符合条件
        stKey.lVflag = 0;
        stKey.lObjtype = 2;
        stKey.lObjid = compid;

        pData = (lanSoftUpdate*)utShmHashLook(psShmHead, NC_LNK_NW_SOFTUP, &stKey);
        if(pData)
        {
            if(pData->lSoftid > psComp->stime && strcmp(pData->caVersion, pVer) != 0)
            {
                if(pData->lSoftid > lSid)
                {
                    lSid = pData->lSoftid;
                }
            }
        }

        //类别为部门
        stKey.lVflag = lVflag;
        stKey.lObjtype = 1;
        stKey.lObjid = psComp->groupid;


        pData = (lanSoftUpdate*)utShmHashLook(psShmHead, NC_LNK_NW_SOFTUP, &stKey);
        if(pData)
        {

            if(pData->lSoftid > psComp->stime && strcmp(pData->caVersion, pVer) != 0)
            {
                if(pData->lSoftid > lSid)
                {
                    lSid = pData->lSoftid;
                    return lSid;
                }
            }
        }






        char sql[128] = "";
        unsigned long groupid = 0;

        sprintf(sql, "select pid from nwgroup where gid = %lu", psComp->groupid);
        pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &groupid);



        while(groupid != 0)
        {

            stKey.lObjid = groupid;
            pData = (lanSoftUpdate*)utShmHashLook(psShmHead, NC_LNK_NW_SOFTUP, &stKey);
            if(pData)
            {

                if(pData->lSoftid > psComp->stime && strcmp(pData->caVersion, pVer) != 0)
                {
                    if(pData->lSoftid > lSid)
                    {
                        lSid = pData->lSoftid;
                        break;
                    }
                }
            }
            memset(sql, 0, sizeof(sql));
            sprintf(sql, "select pid from nwgroup where gid = %lu", groupid);
            pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &groupid);
        }


        // 判断通用类别符合不符合条件
        stKey.lVflag = 0;
        stKey.lObjtype = 1;
        stKey.lObjid = psComp->groupid;
        pData = (lanSoftUpdate*)utShmHashLook(psShmHead, NC_LNK_NW_SOFTUP, &stKey);
        if(pData)
        {
            if(pData->lSoftid > psComp->stime && strcmp(pData->caVersion, pVer) != 0)
            {
                if(pData->lSoftid > lSid)
                {
                    lSid = pData->lSoftid;
                }
            }
        }

        //类别为全部
        stKey.lVflag = lVflag;
        stKey.lObjtype = 0;
        stKey.lObjid = 0;
        pData = (lanSoftUpdate*)utShmHashLook(psShmHead, NC_LNK_NW_SOFTUP, &stKey);
        if(pData)
        {
            if(pData->lSoftid > psComp->stime && strcmp(pData->caVersion, pVer) != 0)
            {
                if(pData->lSoftid > lSid)
                {
                    lSid = pData->lSoftid;
                }
            }
        }
        // 判断通用类别符合不符合条件
        stKey.lVflag = 0;
        stKey.lObjtype = 0;
        stKey.lObjid = 0;
        pData = (lanSoftUpdate*)utShmHashLook(psShmHead, NC_LNK_NW_SOFTUP, &stKey);
        if(pData)
        {
            if(pData->lSoftid > psComp->stime && strcmp(pData->caVersion, pVer) != 0)
            {
                if(pData->lSoftid > lSid)
                {
                    lSid = pData->lSoftid;
                }
            }
        }


        return lSid;

    }
    else
    {
        return 0;
    }


    return 0;
}



int Login(utShmHead *psShmHead, utComSockAddr *psSockAddr, utMsgHead *psMsgHead)
{
    unsigned long  compid = 0;
    char compname[33];
    char username[17];
    char ip[17];
    char mac[19];
    char caMaci[12];
    char os[33];
    char version[21];
    char AuthCode[17];
    char caPsn[32];
    int  status = 0;
    unsigned long lUserid, lCount, lGroupid;
    unsigned long lIp, lGatewayIp;
    char caGatewayIp[20], caCompany[64];
    char caTcpport[20], caTemp[1024], caClientversion[32];
    char ie[32], caVflag[16], caSid[16];
    char caGroup[64];
    nwCompInfo *psComp;
    nwCompIp   *psCompIp;

    long iReturn = 0;
    //utMsgPrintMsg(psMsgHead);
    lGatewayIp = ntohl(psSockAddr->sSin.sin_addr.s_addr);

    memset(compname, 0 , sizeof(compname));
    memset(username, 0, sizeof(username));
    memset(ip, 0, sizeof(ip));
    memset(mac, 0, sizeof(mac));
    memset(os, 0, sizeof(os));
    memset(version, 0, sizeof(version));
    memset(AuthCode, 0, sizeof(AuthCode));
    memset(ie, 0, sizeof(ie));
    lGroupid = 0;
    memset(caCompany, 0, sizeof(caCompany));
    iReturn = utMsgGetSomeNVar(psMsgHead, 12,
                               "compid",      UT_TYPE_ULONG, 4,  &compid,
                               "groupid",     UT_TYPE_ULONG, 4, &lGroupid,
                               "compname",    UT_TYPE_STRING, 32,  compname,
                               "username",    UT_TYPE_STRING, 16,  username,
                               "ip",          UT_TYPE_STRING, 16,  ip,
                               "mac",         UT_TYPE_STRING, 18,  mac,
                               "os",          UT_TYPE_STRING, 32,  os,
                               "ie",          UT_TYPE_STRING, 31, ie,
                               "version",     UT_TYPE_STRING, 16,  version,
                               "AuthCode",    UT_TYPE_STRING, 16,  AuthCode,
                               "vflag",       UT_TYPE_STRING, 10, caVflag,
                               "company",     UT_TYPE_STRING, 63, caCompany);


    lIp = ntohl(pasIpcvtLong(ip));
    //     ncCvtMacI(caMac,mac);
    strcpy(caPsn, utComGetVar_sd(psShmHead, "ProductSN", ""));
    strcpy(caTcpport, utComGetVar_sd(psShmHead, "TcpPort", "9185"));
    strcpy(caClientversion, utComGetVar_sd(psShmHead, "ClientVersion", ""));
	sprintf(caTemp, "compid=%lu, ip:%s 发送login请求,compname=%s\n", compid ,ip, compname);
    pasLogs(PAS_SRCFILE, 1000, caTemp);
	if((compid == 1293458015)||(strcmp(ip, "192.168.20.214")==0))
	{
		sprintf(caTemp, "1293458015 ip:%s 发送login请求,compname=%s\n", ip, compname);
        pasLogs(PAS_SRCFILE, 1000, caTemp);
	}
    //printf("---------------------------caPsn=%s,  AuthCode=%s\n",  caPsn,  AuthCode);
    if(strcasecmp(caPsn, AuthCode) != 0)
    {
        //授权码不对，不能登录
        sprintf(caTemp, "ip:%s 授权码不对，返回-1", ip);
        pasLogs(PAS_SRCFILE, 1000, caTemp);
        utComUdpResponse1(psShmHead, psSockAddr, psMsgHead, 0, 1,
                          "status",  UT_TYPE_LONG, (-1));

        sprintf(caTemp, "strcasecmp error! login:compid=%lu,compname=%s,username=%s,ip=%s,mac=%s,os=%s,ie=%s,version=%s,AuthCode=%s,vflag=%s,groupid=%lu", compid, compname, username, ip, mac, os, ie, version, AuthCode, caVflag, lGroupid);
        writelog(caTemp);
        recodeLogInfo("lan_UdpBeat Log ...", "999999", caTemp);
        return 0;
    }
	
    psComp = (nwCompInfo *)utShmHashLookA(psShmHead, NC_LNK_COMPUTE, &compid);
    if(psComp)
    {
        psComp->ip = lIp;
        if(strlen(psComp->devname) == 0)
        {
            strcpy(psComp->devname, username);             //设备名如果为空，用username代替，如果用户修改过?飧?侄?不去更改?
        }
        strcpy(psComp->compname, compname);
        ncCvtMacI(mac, psComp->mac);
        strcpy(psComp->version, version);
        strcpy(psComp->caKey, AuthCode);
        if(psComp->userid == 0)
        {
            //id为0，获取USERID
            lUserid = nwUtlGetUserid(psShmHead, lIp, mac, compname);
            psComp->userid = lUserid;
        }
        strcpy(caGroup, (char*)lanGetGroup(psShmHead, psComp->groupid));
        if(strlen(caGroup) == 0)
        {
            psComp->groupid = 0;
        }
        if(psComp->groupid == 0)
        {
            if(lGroupid == 0)
            {
                lGroupid = nwUtlGetGroupid(psShmHead, lIp, mac, compname);
            }
            psComp->groupid = lGroupid;
        }
        //检查数据库里有没有存在这个计算机信息，如果存在更新数据库记录，否则添加
        lCount = 0;
        sprintf(caTemp, "select count(*) from nwcompute where compid=%lu ", compid);
        //printf("caTemp=%s\n",caTemp);
        pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lCount);


        if(lCount == 0)
        {
            iReturn = pasDbExecSqlF("insert into nwcompute(compid,ip,groupid,userid,devname,compname,mac,os,version,addtime,lasttime,authkey,ie) \
                  	 values (%lu,%lu,%lu,%lu,'%s','%s','%s','%s','%s',%lu,%lu,'%s','%s')",
                                    compid, lIp, psComp->groupid, psComp->userid, psComp->devname, psComp->compname, mac, os, version, time(0), time(0), AuthCode, ie);
        }
        else
        {
            iReturn = pasDbExecSqlF("update nwcompute set ip=%lu,compname='%s',mac='%s',os='%s',version='%s',lasttime=%lu,ie='%s',status=0 where compid=%lu ", \
                                    lIp, psComp->compname, mac, os, version, time(0), ie, compid);

        }
        if(iReturn != 0)                          //没法操作数据库，返回登录不成功-3
        {
            utComUdpResponse1(psShmHead, psSockAddr, psMsgHead, 0, 1,
                              "status",  UT_TYPE_LONG, (-3));
            sprintf(caTemp, "ip:%s 插入数据库出错，返回-3", ip);
            pasLogs(PAS_SRCFILE, 1000, caTemp);

            sprintf(caTemp, "utComUdpResponse1 error! login:compid=%lu,compname=%s,username=%s,ip=%s,mac=%s,os=%s,ie=%s,version=%s,AuthCode=%s,vflag=%s,groupid=%lu", compid, compname, username, ip, mac, os, ie, version, AuthCode, caVflag, lGroupid);
            writelog(caTemp);
            //recodeLogInfo("lan_UdpBeat Log ...","88888888",caTemp);
            return 0;
        }



    }
    else                         //没法添加，返回登录不成功-2
    {
        utComUdpResponse1(psShmHead, psSockAddr, psMsgHead, 0, 1,
                          "status",  UT_TYPE_LONG, (-2));
        sprintf(caTemp, "ip:%s 没法添加到内存，返回-2", ip);
        pasLogs(PAS_SRCFILE, 1000, caTemp);

        sprintf(caTemp, "utComUdpResponse1-- error! login:compid=%lu,compname=%s,username=%s,ip=%s,mac=%s,os=%s,ie=%s,version=%s,AuthCode=%s,vflag=%s,groupid=%lu", compid, compname, username, ip, mac, os, ie, version, AuthCode, caVflag, lGroupid);
        writelog(caTemp);
        //recodeLogInfo("lan_UdpBeat Log ...","777777",caTemp);
        return 0;
    }


    psCompIp = (nwCompIp *)utShmHashLookA(psShmHead, NC_LNK_ONLINECOMP, &compid);
    if(psCompIp == NULL)
    {

        utComUdpResponse1(psShmHead, psSockAddr, psMsgHead, 0, 1,
                          "status",  UT_TYPE_LONG, (-4));
        sprintf(caTemp, "ip:%s 没法添加到在线用户，返回-4", ip);
        pasLogs(PAS_SRCFILE, 1000, caTemp);

        sprintf(caTemp, "utComUdpResponse1-0 error! login:compid=%lu,compname=%s,username=%s,ip=%s,mac=%s,os=%s,ie=%s,version=%s,AuthCode=%s,vflag=%s,groupid=%lu", compid, compname, username, ip, mac, os, ie, version, AuthCode, caVflag, lGroupid);
        writelog(caTemp);
        //recodeLogInfo("lan_UdpBeat Log ...","66666666",caTemp);
        return 0;
    }
    else
    {
        psCompIp->psComp = psComp;
        psComp->status = 1;
        psCompIp->lLoginTime = time(0);
        psCompIp->lLastTime = time(0);
        psCompIp->ip  = lIp;
        psCompIp->compid = compid;


        //判断一下是否有升级包
        char caUKey[32], caBkip[32];
        long lVflag = atol(caVflag);
        long lSid = lanGetSoftUpdateId(psShmHead, lVflag, compid, version);
        sprintf(caSid, "%d", lSid);

        strcpy(caUKey, utComGetVar_sd(psShmHead, "unstall_key", ""));
        strcpy(caBkip, utComGetVar_sd(psShmHead, "bkip", "192.168.10.16"));

        sprintf(caTemp, "lanGetSoftUpdateId login:compid=%lu,compname=%s,username=%s,ip=%s,mac=%s,os=%s,ie=%s,version=%s,AuthCode=%s,vflag=%s,groupid=%lu upid=%d", compid, compname, username, ip, mac, os, ie, version, AuthCode, caVflag, lGroupid, lSid);
        writelog(caTemp);
        recodeLogInfo("lan_UdpBeat Log ...", "55555555", caTemp);


        //处理一些额外信息
        unsigned long lBindip, lContstus, lDebugstatus;
        lBindip = 0;
        lContstus = 0;
        lDebugstatus = 0;
        sprintf(caTemp, "select bindip,contstus,debugstatus from nwcompute where compid=%lu ", compid);
        pasDbOneRecord(caTemp, 0, UT_TYPE_ULONG, 4, &lBindip,
                       UT_TYPE_ULONG, 4, &lContstus,
                       UT_TYPE_ULONG, 4, &lDebugstatus);
        if(lContstus == 0x02)
        {
            lanCompyInfo *psCompy;
            psCompy = (lanCompyInfo *)utShmHashLookA(psShmHead, NC_LNK_NW_COMPY, caCompany);
            if(psCompy)
            {
                psCompy->gatwayip = lGatewayIp;
                sprintf(caTemp, "company:%s,gatewayip:%lu", caCompany, lGatewayIp);
                writelog(caTemp);
                //recodeLogInfo("lan_UdpBeat Log ...","444444444",caTemp);
            }
        }
        utComUdpResponse1(psShmHead, psSockAddr, psMsgHead, 0, 11,
                          "status",  UT_TYPE_ULONG, 0,
                          "basetime", UT_TYPE_ULONG, time(0),
                          "userid", UT_TYPE_ULONG, lUserid,
                          "tcpport", UT_TYPE_STRING, caTcpport,
                          "version", UT_TYPE_STRING, caClientversion,
                          "upid", UT_TYPE_STRING, caSid,
                          "uninstallkey", UT_TYPE_STRING, caUKey,
                          "bkip", UT_TYPE_STRING, caBkip,
                          "bindip", UT_TYPE_ULONG, lBindip,
                          "contstus", UT_TYPE_ULONG, lContstus,
                          "debugstatus", UT_TYPE_ULONG, lDebugstatus
                         );

        sprintf(caTemp, "softupdate:compid=%lu,compname=%s,username=%s,ip=%s,mac=%s,os=%s,ie=%s,version=%s,AuthCode=%s,upid=%s,uninstallkey=%s,bkip=%s,bindip=%lu,contstus=%lu,debugstatus=%lu", compid, compname, username, ip, mac, os, ie, version, AuthCode, caSid, caUKey, caBkip, lBindip, lContstus, lDebugstatus);
        writelog(caTemp);
        //recodeLogInfo("lan_UdpBeat Log ...","33333333",caTemp);
        return 0;

    }
    utComUdpResponse1(psShmHead, psSockAddr, psMsgHead, 0, 1,
                      "status",  UT_TYPE_LONG, (-9));

    sprintf(caTemp, "ip:%s 未知错误，返回-9", ip);
    pasLogs(PAS_SRCFILE, 1000, caTemp);
    return 0;

    /*


        //先检查当前机器是否存在索引表中
        if(m_index)
        {
                    struct index * lookindex =(struct index *)pasLHashLook(m_index,compid);
                    if(lookindex)
            {
                //对资源进行加锁
                pthread_mutex_lock(&mylock);

                        time_t  now;
                time(&now);
                lookindex->machine->lasttime = now;
                lookindex->machine->lasttolib  =  now;
                lookindex->machine->flags = '0';

                status = lookindex->machine->command;

                //对资源进行解锁
                pthread_mutex_unlock(&mylock);
    printf("compid = %s\n", lookindex->machine->compid);
    printf("compname = %s\n", lookindex->machine->compname);
    printf("username = %s\n", lookindex->machine->username);
    printf("ip = %s\n",  lookindex->machine->ip);
    printf("lasttime = %d\n",  lookindex->machine->lasttime);
    printf("flags=%c\n",  lookindex->machine->flags);

                utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                                             "status",  UT_TYPE_LONG, status,
                                 "Userid",  UT_TYPE_LONG, Userid);
                return 1;
            }
    */
    /*  }


        //创建新MAC_MEM结点并给其赋值
        pMAC_MEM  node =  (pMAC_MEM)malloc(sizeof(MAC_MEM));

        time_t  now;
        time(&now);
        node->lasttime = now;
        node->lasttolib  =  now;
        node->flags = '0';
        strncpy(node->compid, compid,16);
        strncpy(node->compname, compname,32);
        strncpy(node->username,  username,16);
        strncpy(node->ip,   ip,16);
        strncpy(node->mac, mac,16);
        strncpy(node->os,  os,32);
        strncpy(node->version, version,20);


        //对资源进行加锁
        pthread_mutex_lock(&mylock);

        //把新创建的MAC_MEM结点加入到循环双向链表中
        if(NULL == m_plist)
        {
            m_plist = node;
            node->next = node;
            node->prv   = node;
        }
        else
        {
            node->next = m_plist;
            node->prv   = m_plist->prv;
            m_plist->prv  = node;
            m_plist = node;
        }

        //如果索引表为空，则初始化索引m_index
        if(NULL == m_index)
        {
            //初始化
                m_index = (unsigned char *)pasLHashInit(30,30,sizeof(struct index),0,16);
                if(m_index == NULL) {
                        status = 0;
            }
            else
            {
                status = 1;
            }
        }

        //把新创建的MAC_MEM结点加入到索引m_index中
        struct index * addindex = (struct index *)pasLHashLookA(m_index, compid);
            if(addindex){
                  addindex->machine = node;
             }

        //对资源进行解锁
        pthread_mutex_unlock(&mylock);

        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                                            "status",  UT_TYPE_LONG, status,
                                 "Userid",  UT_TYPE_LONG, Userid);

        return 1;
    }
    */

}
//当前结点node出链
int         outLink(pMAC_MEM  node)
{
    int  iRet = 0;
    if(node)
    {
        node->prv->next  = node->next;
        node->next->prv  = node->prv;
        iRet = 1;
    }
    return iRet;
}

//结点node加入到链头
int inLink(pMAC_MEM  node)
{
    int iRet = 0;
    if(node)
    {
        node->next = m_plist;
        node->prv  =  m_plist->prv;
        m_plist = node;
        iRet = 1;
    }
    return iRet;
}



int ConnectStatus(utShmHead *psShmHead, utComSockAddr *psSockAddr, utMsgHead *psMsgHead)
{
    unsigned int    compid;
    char                    caTemp[1024];
    int                 Userid = 0;
    int                 status = 0;
    long                    iReturn = 0;
    unsigned long bTime, pTime;
    lanProcyObj     *pData2;
    //  utMsgPrintMsg(psMsgHead);
    nwCompInfo      *psComp;
    nwCompIp          *psCompIp;
    struct key_s
    {
        unsigned long ctype;
        unsigned long id;
    };
    struct key_s  stKey;
    struct key_b
    {
        unsigned long groupid;
        unsigned long computeid;
    };
    struct key_b    stKeyB;
    unsigned long   lIp;
    unsigned long   lGatewayIp, lLocal;
    char caGatewayIp[20], caCompany[64], caLocalinfo[32];
    char caIp[20];
    lanCompyInfo *psCompy;
    lanBlackList *pData3;
    memset(caCompany, 0, sizeof(caCompany));
    iReturn = utMsgGetSomeNVar(psMsgHead, 4,
                               "compid", UT_TYPE_ULONG, 4, &compid,
                               "Userid", UT_TYPE_ULONG,   4, &Userid,
                               "company", UT_TYPE_STRING,  63, caCompany,
                               "localinfo", UT_TYPE_STRING, 4, caLocalinfo);
    sprintf(caTemp, "compid=%lu,userid=%lu,company=%s,localinfo=%s\n", compid, Userid, caCompany, caLocalinfo);
    //pasLogs(PAS_SRCFILE, 1000, caTemp);
    psComp = (nwCompInfo *)utShmHashLook(psShmHead, NC_LNK_COMPUTE, &compid);
    if(psComp)
    {
        lIp = ntohl(psSockAddr->sSin.sin_addr.s_addr);
        psComp->ip = lIp;
        //      lIp=psComp->ip;
        psCompIp = (nwCompIp *)utShmHashLook(psShmHead, NC_LNK_ONLINECOMP, &compid);
        if(psCompIp)
        {
            psCompIp->lLastTime = time(0);
            //           psCompIp->lCtrl;
            bTime = lanGetPbaseTime(psShmHead);
            pTime = lanGetPTime(psShmHead);

            memset(&stKeyB, 0, sizeof(struct key_b));
            stKeyB.groupid = 0;
            stKeyB.computeid = compid;
            pData3 = (lanBlackList*)utShmHashLook(psShmHead, NC_LNK_NW_BLACKLIST, &stKeyB);
            if(!pData3)
            {
                memset(&stKeyB, 0, sizeof(struct key_b));
                stKeyB.groupid = psComp->groupid;
                stKeyB.computeid = 0;

                pData3 = (lanBlackList*)utShmHashLook(psShmHead, NC_LNK_NW_BLACKLIST, &stKeyB);
            }


            //全局策略
            memset(&stKey, 0, sizeof(struct key_s));
            stKey.ctype = 0;
            stKey.id = 0;
            pData2 = (lanProcyObj*)utShmHashLook(psShmHead, NC_LNK_NW_PROCYOBJ, &stKey);
            if(pData2)
            {
                if(pData2->modtime > pTime)
                {
                    pTime = pData2->modtime;
                }
            }
            //部门策略
            memset(&stKey, 0, sizeof(struct key_s));
            stKey.ctype = 1;
            stKey.id = psComp->groupid;
            pData2 = (lanProcyObj*)utShmHashLook(psShmHead, NC_LNK_NW_PROCYOBJ, &stKey);
            if(pData2)
            {
                if(pData2->modtime > pTime)
                {
                    pTime = pData2->modtime;
                }
            }
            //计算机策略
            memset(&stKey, 0, sizeof(struct key_s));
            stKey.ctype = 2;
            stKey.id = compid;
            pData2 = (lanProcyObj*)utShmHashLook(psShmHead, NC_LNK_NW_PROCYOBJ, &stKey);
            if(pData2)
            {
                if(pData2->modtime > pTime)
                {
                    pTime = pData2->modtime;
                }
            }
            lLocal = 0;
            if(strcmp(caLocalinfo, "get") == 0)
            {
                lGatewayIp = ntohl(psSockAddr->sSin.sin_addr.s_addr);
                strcpy(caGatewayIp, utComHostIp(htonl(lGatewayIp)));
                strcpy(caIp, utComHostIp(htonl(lIp)));
                if(strncmp(caIp, "192.168", 7) != 0 && strncmp(caIp, "10.", 3) != 0 && strncmp(caIp, "172.", 4) != 0)
                {
                    lLocal = 1;
                }
                else if(lIp == lGatewayIp)
                {
                    lLocal = 0;
                }
                else
                {
                    psCompy = (lanCompyInfo *)utShmHashLook(psShmHead, NC_LNK_NW_COMPY, caCompany);
                    if(psCompy)
                    {
                        if(lGatewayIp != psCompy->gatwayip)
                        {
                            lLocal = 1;
                        }
                    }
                }
            }
            utComUdpResponse1(psShmHead, psSockAddr, psMsgHead, 0, 4,
                              "status",  UT_TYPE_ULONG, psCompIp->lCtrl,
                              "ptime",     UT_TYPE_ULONG, pTime,
                              "bptime",    UT_TYPE_ULONG, bTime,
                              "localinfo", UT_TYPE_ULONG, lLocal);

            sprintf(caTemp, "ip:%s status:%lu,btime:%lu,ptime:%lu,lLocal:%lu", utComHostIp(htonl(lIp)), psCompIp->lCtrl, bTime, pTime, lLocal);
            //pasLogs(PAS_SRCFILE, 1000, caTemp);
            //recodeLogInfo("ConnectStatus Log ...","----------",caTemp);
            return 0;
        }
        else
        {
            sprintf(caTemp, "compidip=%lu,userid=%lu\n", compid, Userid);
            //pasLogs(PAS_SRCFILE, 1000, caTemp);
        }
    }
    else
    {
        sprintf(caTemp, "compid22=%lu,userid=%lu\n", compid, Userid);
        //pasLogs(PAS_SRCFILE, 1000, caTemp);
    }

    utComUdpResponse1(psShmHead, psSockAddr, psMsgHead, 0, 2,
                      "status",   UT_TYPE_LONG, LAN_CLIENT_RELOGIN,
                      "ptime",     UT_TYPE_ULONG, 0);
    sprintf(caTemp, "ip:%s 重新登录，返回200", utComHostIp(htonl(lIp)));
    //pasLogs(PAS_SRCFILE, 1000, caTemp);

    return 1;
}

int     readTableStruct1(char *filename, char sql[])
{
    int        iRet = 0;
    FILE *file;

    char *first = NULL;
    char  lineData[2000];
    char  sqlCreate[2500];

    memset(lineData, 0, sizeof(lineData));
    memset(sql, 0, sizeof(sql));

    int start = 0;

    if(filename == NULL)
        return 0;

    file = fopen(filename, "rb+");
    if(file)
    {
        while(!feof(file))
        {

            fgets(lineData, 2000, file);

            first = strtok(lineData, "]--");

            if(first   &&  !strcmp(first, "[lan_nwcompute:"))
            {
                start = 1;
                continue;
            }
            else if(first   &&  !strcmp(first, "[/lan_nwcompute"))
            {
                break;
            }

            if(first  &&  start == 1)
            {
                strcat(sql, first);
            }

        }
        fclose(file);
        iRet = 1;
    }
    else
    {
        printf("open file'UdpBeat.sql ' fail!");
        iRet = 0;
    }
    return iRet;
}

//把头结点入库,并插入到链尾
int     tolib()
{
    char   sql[2500];
    long   iReturn = 0;

    if(m_plist)
    {
        //更新入库时间
        time_t  now;
        time(&now);
        m_plist->lasttolib = now;

        //更改入库标记
        m_plist->flags  = '0';

        //把头结点插入到最后
        m_plist = m_plist->next;

        //把当前结点数据写入到数据库中
        readTableStruct1("../src/table.sql", sql);
        int  iReturn = pasDbExecSql(sql, 0);
        if(iReturn == 0) //创建表成功！
        {
            snprintf(sql, 2500, "insert into  \
				lan_nwcompute(  compid,  \
						ip,       \
						compname, \
						mac,      \
						os,       \
						lasttime, \
						version,  \
						status    \
					 )               \
				values(%s, %s, %s, %s, %s,%d,%s,%d)", \
                     m_plist->compid,         \
                     m_plist->ip,            \
                     m_plist->compname,      \
                     m_plist->mac,           \
                     m_plist->os,            \
                     m_plist->lasttime,      \
                     m_plist->version,       \
                     m_plist->command        \
                    );
            iReturn = pasDbExecSql(sql, 0);
        }
        else //创建表失败
        {
            printf("创建表失败！");
        }


    }

    return 1;

}

void *  UpdateToLib(void)
{
    while(!g_bExit && m_index != NULL)
    {
        int g_nTotalLows =  pasLHashRecord(m_index);  //索引表中结点总数
        int count = 0;
        while(m_plist)
        {
            //当前头结点入库
            if('1' == m_plist->flags)
            {
                //对资源进行加锁
                pthread_mutex_lock(&mylock);

                //当前头结点入库
                tolib();

                //对资源进行解锁
                pthread_mutex_unlock(&mylock);

            }
            else //如果当前结点标记为空，则结束本次遍历
            {
                break;
            }


            count++;
            if(count > (g_nTotalLows - 1))break;

        }
        sleep(60);
    }

}
//心跳机制----连接
int Lan_Status_Connect(utShmHead *psShmHead, utComSockAddr *psSockAddr, utMsgHead *psMsgHead)
{
    Login(psShmHead, psSockAddr, psMsgHead);
}

//心跳机制---运行
int Lan_Status_Runing(utShmHead *psShmHead, utComSockAddr *psSockAddr, utMsgHead *psMsgHead)
{
    ConnectStatus(psShmHead, psSockAddr, psMsgHead);
}


int ncInitNwWebFun_UdpBeat(utShmHead *psShmHead)
{

    //     pthread_t id1;

    /*创建线程*/

    //      int ret = pthread_create(&id1, NULL, (void *)UpdateToLib, NULL);

    //初始化互斥锁
    //  ret = pthread_mutex_init(&mylock,NULL);

    utComSetUdpFun(LAN_STATUS_CONNECT, Lan_Status_Connect);
    utComSetUdpFun(LAN_STATUS_RUNING, Lan_Status_Runing);

    printf("UdpBeat   init   ok!\n\n");
}
