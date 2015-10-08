/* 内网管理  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "ncdef.h"
#include "nwdef.h"

char *nwGetCompName(int iG,int iU);

/* 显示计算机监控菜单  */
int  nwWebCompMon_menu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caPlate[128],caGtype[16],caId[16],caOpt[2];
    int   iReturn;
    long lGtype,lId;
    utPltDbHead *psDbHead;
	iReturn = utMsgGetSomeNVar(psMsgHead,3,
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All 4-Polcy */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
	                "opt",     UT_TYPE_STRING, 1,  caOpt);
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"id",caId);
    utPltPutVar(psDbHead,"gtype",caGtype);
    utPltPutVar(psDbHead,"opt", caOpt);
    utPltPutVar(psDbHead,"dispname",nwGetCompName(atol(caGtype),atol(caId)));
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_mon_menu.htm");
    return 0;
}

   
/* 显示计算机有关信息  */
int nwWebCompMon(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caOpt[4];
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
					"opt",    UT_TYPE_STRING, 1,  caOpt);      /* 1--Base 2--HardInfo  3--HardLog  4--SoftInfo 5--Softlog  */
    switch (caOpt[0]) {
        case '1':
            nwWebCompMon_Netlog(psShmHead,iFd,psMsgHead);   /* 网络访问信息  */
            break;
        case '2':
            nwWebCompMon_CurProc(psShmHead,iFd,psMsgHead);
            break;
        case '3':
            nwWebCompMon_SysInfo(psShmHead,iFd,psMsgHead);
            break;
        case '4':
            nwWebCompMon_Screen(psShmHead,iFd,psMsgHead);
            break;
        case '5':
            nwWebCompMon_Proclog(psShmHead,iFd,psMsgHead);
            break;
        case '6':
            nwWebCompMon_Cnflog(psShmHead,iFd,psMsgHead);
            break;

    }
    return 0;
}

/* 显示计算机硬件信息  */
int nwWebCompMon_Netlog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caCurPg[12];
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lRowNum,lStartRec,lCurpg,lCompid;
    char caDevname[32];
    unsigned long lTotpg,lSid,lSip,lDip,lPort,lProt,lBytes,lVister,lStime,lDport;
    
    char caGroupid[16];
    
    int iNum,k;
    
    char caSmt[256],caSql[256];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    printf("nwWebCompMon_Netlog\n");
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
   	                "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId,
					"cond",   UT_TYPE_STRING, 31, caCond,
					"CurPg",      UT_TYPE_STRING, 11,  caCurPg);
    lId = atol(caId);
    if(utStrIsSpaces(caCurPg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurPg);
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }
    if(caGtype[0] == '2' || caGtype[0] == '3') {  /* 显示设备列表 */
        if(lGroupid0 > 0) {
            lGroupid = lGroupid0;
            sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lGroupid);
        }
        else {
            if(caGtype[0] == '2') {
                sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lId);
            }
            else {
                sprintf(caSmt," where a.compid = b.compid ");
            }
        }
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caSmt,"and (a.devname  like '%c%s%c' ",'%',caCond,'%');
            if(utStrIsValidIp(caCond)) {
                lIp = ntohl(pasIpcvtLong(caCond));
                utStrAddF(caSmt,"or  b.sip = %lu ",lIp);
                utStrAddF(caSmt,"or  b.dip = %lu ",lIp);
            }
            if(utStrIsDigital(caCond)) {
                lPort = atol(caCond);
                utStrAddF(caSmt,"or  b.dport = %lu ",lPort);
            }
            if(strcasecmp(caCond,"Tcp")==0) {
                utStrAddF(caSmt,"or  b.prot = 6 ",lPort);
            }
            else if(strcasecmp(caCond,"Udp")==0) {
                utStrAddF(caSmt,"or  b.prot = 17 ",lPort);
            }
            utStrAddF(caSmt,")");
        }
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwcompute a,nwnetlog b %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
        psCur = pasDbOpenSqlF("select b.sid,a.compid,a.devname,a.groupid,b.stime,b.sip,b.dip,b.dport,b.bytes,b.prot,b.vister from nwcompute a,nwnetlog b %s order by b.stime desc limit %lu,%lu",caSmt,lStartRec,lRowNum);
        if(psCur == NULL) {
printf("SQL:select b.sid,a.compid,a.devname,a.groupid,b.stime,b.sip,b.dip,b.dport,b.bytes,b.prot,b.vister from nwcompute a,nwnetlog b %s order by b.stime desc limit %lu,%lu\n",caSmt,lStartRec,lRowNum);            
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_ULONG,4,&lSip,
                                       UT_TYPE_ULONG,4,&lDip,
                                       UT_TYPE_ULONG,4,&lDport,
                                       UT_TYPE_ULONG,4,&lBytes,
                                       UT_TYPE_ULONG,4,&lProt,
                                       UT_TYPE_ULONG,4,&lVister);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"groupname", iNum,
                                   ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
            utPltPutLoopVarF(psDbHead,"compid", iNum,"%lu",lCompid);
            utPltPutLoopVar(psDbHead,"devname", iNum,caDevname);
            utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%y-%m-%d %H:%M:%S",lStime));
            utPltPutLoopVar(psDbHead,"sip", iNum,utComHostIp(htonl(lSip)));
            utPltPutLoopVar(psDbHead,"dip", iNum,utComHostIp(htonl(lDip)));
            utPltPutLoopVarF(psDbHead,"dport", iNum,"%d",lDport);
            if(lProt == 6) {
                utPltPutLoopVar(psDbHead,"prot", iNum,"Tcp");
            }
            else if(lProt == 17) {
                utPltPutLoopVar(psDbHead,"prot", iNum,"Udp");
            }
            else if(lProt == 1) {
                utPltPutLoopVar(psDbHead,"prot", iNum,"ICMP");
            }
            if(lVister == 0) {
                utPltPutLoopVar(psDbHead,"vister", iNum,ncLang("0231访问"));
            }
            else {
                utPltPutLoopVar(psDbHead,"vister", iNum,ncLang("0117被访问"));
            }
            utPltPutLoopVarF(psDbHead,"bytes", iNum,"%lu",lBytes);
                
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_ULONG,4,&lSip,
                                       UT_TYPE_ULONG,4,&lDip,
                                       UT_TYPE_ULONG,4,&lDport,
                                       UT_TYPE_ULONG,4,&lBytes,
                                       UT_TYPE_ULONG,4,&lProt,
                                       UT_TYPE_ULONG,4,&lVister);
        }
        pasDbCloseCursor(psCur);
        k=0;
        lRowNum = 34;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"cond", caCond);
        utPltPutVar(psDbHead,"gtype",caGtype);
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_netlog.htm");
    }
    else {  /* 显示单个用户  */
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSmt," where  compid = %lu ",lId);
        k = 0;
        if(!utStrIsSpaces(caCond)) {
            
            if(utStrIsValidIp(caCond)) {
                lIp = ntohl(pasIpcvtLong(caCond));
                utStrAddF(caSmt,"and (sip = %lu or dip = %lu ",lIp,lIp);
                k = 1;
            }
            if(utStrIsDigital(caCond)) {
                lPort = atol(caCond);
                if(k==1) {
                    utStrAddF(caSmt,"or  dport = %lu ",lPort);
                }
                else {
                    utStrAddF(caSmt,"and (dport = %lu ",lPort);
                    k = 1;
                }
            }
            if(strcasecmp(caCond,"Tcp")==0) {
                if(k==1) {
                    utStrAddF(caSmt,"or  prot = 6 ",lPort);
                }
                else {
                    utStrAddF(caSmt,"and (prot = 6 ",lPort);
                    k = 1;
                }
            }
            else if(strcasecmp(caCond,"Udp")==0) {
                if(k==1) {
                    utStrAddF(caSmt,"or  prot = 17 ",lPort);
                }
                else {
                    utStrAddF(caSmt,"and (prot = 17 ",lPort);
                    k=1;
                }
                    
            }
            if(k==1) {
                utStrAddF(caSmt,")");
            }
        }

        
        sprintf(caSql,"select count(*) from  nwnetlog %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
        psCur = pasDbOpenSqlF("select sid,stime,sip,dip,dport,bytes,prot,vister from nwnetlog  %s order by stime desc  limit %lu,%lu",caSmt,lStartRec,lRowNum);
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
                
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_ULONG,4,&lSip,
                                       UT_TYPE_ULONG,4,&lDip,
                                       UT_TYPE_ULONG,4,&lDport,
                                       UT_TYPE_ULONG,4,&lBytes,
                                       UT_TYPE_ULONG,4,&lProt,
                                       UT_TYPE_ULONG,4,&lVister);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%y-%m-%d %H:%M:%S",lStime));
            utPltPutLoopVar(psDbHead,"sip", iNum,utComHostIp(htonl(lSip)));
            utPltPutLoopVar(psDbHead,"dip", iNum,utComHostIp(htonl(lDip)));
            utPltPutLoopVarF(psDbHead,"dport", iNum,"%d",lDport);
            if(lProt == 6) {
                utPltPutLoopVar(psDbHead,"prot", iNum,"Tcp");
            }
            else if(lProt == 17) {
                utPltPutLoopVar(psDbHead,"prot", iNum,"Udp");
            }
            else if(lProt == 1) {
                utPltPutLoopVar(psDbHead,"prot", iNum,"ICMP");
            }
            if(lVister == 0) {
                utPltPutLoopVar(psDbHead,"vister", iNum,ncLang("0231访问"));
            }
            else {
                utPltPutLoopVar(psDbHead,"vister", iNum,ncLang("0117被访问"));
            }
            utPltPutLoopVarF(psDbHead,"bytes", iNum,"%lu",lBytes);
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_ULONG,4,&lSip,
                                       UT_TYPE_ULONG,4,&lDip,
                                       UT_TYPE_ULONG,4,&lDport,
                                       UT_TYPE_ULONG,4,&lBytes,
                                       UT_TYPE_ULONG,4,&lProt,
                                       UT_TYPE_ULONG,4,&lVister);

        }
        pasDbCloseCursor(psCur);
        k=0;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltPutVar(psDbHead,"cond", caCond);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_netlog_u.htm");
    }
}

/* 当前进程情况  

 */
int nwWebCompMon_CurProc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caTemp[32];
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lRowNum,lTime;
    nwCompIp  *psCompIp;
    nwCompInfo *psComp;
    nwProcInfo *psProc;
    char temp_compname[64];
    char temp_pid[16];
    int iNum,k;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    printf("nwWebCompMon_CurProc\n");
    utMsgPrintMsg(psMsgHead);
#endif
lRowNum = 34;
   	iReturn = utMsgGetSomeNVar(psMsgHead,3,
   	                "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId,
					"cond",   UT_TYPE_STRING, 31, caCond);
   

    lId = atol(caId);
    psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lId);
    if(psComp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0506设备不存在"));
        return 0;
    }
    if(psComp->status == 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0262该设备不在线上"));
        return 0;
    } 
    strcpy(temp_compname,psComp->compname); 
         
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&psComp->ip);
    if(psCompIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0262该设备不在线上"));
        return 0;
    }
    /* 显示进程信息  */
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }
  
    if(utMsgGetVar_s(psMsgHead,"killproc")) { /* 杀掉选择的进程  */
        utMsgHead *psMsgHead0;
        char caVar[16];
        char *p;
        long lPid;
        int i;
        psMsgHead0 = utMsgInit(NW_MSG_CODE_REQ,NC_UDP_KILLPROC,utStrGetId(),4096);
        iNum = 0;
        
        for(i=0;i<lRowNum;i++) {
            sprintf(caVar,"chk%d",i+1);
            p = utMsgGetVar_s(psMsgHead,caVar);
            if(p) {
                iNum++;
                sprintf(caVar,"pid%d",iNum);
         //       lPid = atol(p);
                sprintf(temp_pid,"%s",p);
  
                utMsgAdd_s(&psMsgHead0,caVar,temp_pid);
            }
        }
        if(iNum > 0) {
            utMsgAdd_l(&psMsgHead0,"sum",iNum);
            nwUtlSendUdpMsg(psCompIp,psMsgHead0);
        }
        utMsgFree(psMsgHead0);
    }
    
    nwUtlReqSynInfo(psCompIp,NC_UDP_PROCINFO,1);
    lTime = time(0);
    lCount = 0;
    while(lCount < 5) { /* 等待接收信息  */
        if(psCompIp->cProcInfo == 0) {
            if(lTime < time(0) - 2) {
                lTime = time(0);
                nwUtlReqSynInfo(psCompIp,NC_UDP_PROCINFO,1);
                lCount++;
            }
            usleep(100);
        }
        else {
            break;
        }
    }
    psDbHead = utPltInitDb();
    psProc = psCompIp->sDevHead.firstProc;
    iNum = 0;
    
    utPltPutVar(psDbHead,"syntime",utTimFormat("%Y-%m-%d %H:%M:%S",psCompIp->sDevHead.lProcTime)); 
    while(psProc) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"num", iNum,"%lu",iNum);
//        utPltPutLoopVarF(psDbHead,"pid", iNum,"%lu",psProc->iPid);
        utPltPutLoopVarF(psDbHead,"name", iNum,"%s",psProc->caName);
//        utPltPutLoopVarF(psDbHead,"stime", iNum,"%s",psProc->stime);
        utPltPutLoopVar(psDbHead,"cputime", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",psProc->cputime));
        utPltPutLoopVarF(psDbHead,"cpurate", iNum,"%lu",psProc->cpurate-psProc->cputime);
        utPltPutLoopVar(psDbHead,"path",iNum,psProc->cRev);
//        utPltPutLoopVarF(psDbHead,"mem",     iNum,"%s",utStrLtoF(psProc->mem,caTemp));
        psProc = psProc->next;
    }
    k=0;
    while(iNum < lRowNum) {
        k++;
        utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
        iNum++;
    }
    utPltPutVar(psDbHead,"devname",temp_compname);
    utPltPutVar(psDbHead,"id",   caId);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_process.htm");
}


/*停止客户端进程*/
int nwWebCompMon_Stop(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caTemp[32];
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lRowNum,lTime;
    nwCompIp  *psCompIp;
    nwCompInfo *psComp;
    ncUserInfo *psUserInfo;
    utMsgHead *psMsgHead0;
    char temp_compname[64];
    char temp_pid[16];
    int iNum,k;
    utPltDbHead *psDbHead;
printf("pid=%d\n",getpid());

   lRowNum = 34;
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,"id",UT_TYPE_STRING, 15, caId);
    lId = atol(caId);
    psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lId);
    if(psComp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0506设备不存在"));
        return 0;
    }
    if(psComp->status == 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0262该设备不在线上"));
        return 0;
    } 
    strcpy(temp_compname,psComp->compname); 
         
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&psComp->ip);
    if(psCompIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0262该设备不在线上"));
        return 0;
    }
     
   psMsgHead0 = utMsgInit(NW_MSG_CODE_REQ,NC_UDP_APPQUIT,utStrGetId(),4096);
                 
   nwUtlSendUdpMsg(psCompIp,psMsgHead0);       
   utMsgFree(psMsgHead0);
    
    nwUtlReqSynInfo(psCompIp,NC_UDP_APPQUIT,1);

    psDbHead = utPltInitDb();
    
     psUserInfo = ncGetUserInfoByIp(psShmHead,psCompIp->ip);
 
   if(psUserInfo) {
  
       ncUtlDoLogout(psShmHead,psUserInfo,"User Kick Out");
   }
    
   nwUtlLogOut(psShmHead,psCompIp);
   
 
   
   
   
   
   
   
    utPltPutVar(psDbHead,"id",   caId);
    utPltPutVar(psDbHead,"cname",temp_compname);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_stop_tip.htm");
}

/*卸载客户端*/
int nwWebCompMon_Uninstall(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caTemp[32];
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lRowNum,lTime;
    nwCompIp  *psCompIp;
    nwCompInfo *psComp;
    utMsgHead *psMsgHead0;
    char temp_compname[64];
    char temp_pid[16];
    int iNum,k;
    utPltDbHead *psDbHead;
    ncUserInfo *psUserInfo;

   	iReturn = utMsgGetSomeNVar(psMsgHead,1,"id",UT_TYPE_STRING, 15, caId);
    lId = atol(caId);
    psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lId);
    if(psComp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0506设备不存在"));
        return 0;
    }
    if(psComp->status == 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0262该设备不在线上"));
        return 0;
    } 
    strcpy(temp_compname,psComp->compname); 
         
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&psComp->ip);
    if(psCompIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0262该设备不在线上"));
        return 0;
    }
     
   psMsgHead0 = utMsgInit(NW_MSG_CODE_REQ,NC_UDP_APPUNINSTALL,utStrGetId(),4096);
                 
   nwUtlSendUdpMsg(psCompIp,psMsgHead0);       
   utMsgFree(psMsgHead0);
    
    nwUtlReqSynInfo(psCompIp,NC_UDP_APPUNINSTALL,1);

    psDbHead = utPltInitDb();
   psUserInfo = ncGetUserInfoByIp(psShmHead,psCompIp->ip);
   if(psUserInfo) {
       ncUtlDoLogout(psShmHead,psUserInfo,"User Kick Out");
   }
    
   nwUtlLogOut(psShmHead,psCompIp);

   
   
    utPltPutVar(psDbHead,"id",   caId);
    utPltPutVar(psDbHead,"cname",temp_compname);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_stop_tip.htm");
}


/* 当前系统信息

 */
int nwWebCompMon_SysInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16];
    char caTemp[32];
    int i;
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lTime;
    nwCompIp  *psCompIp;
    nwCompInfo *psComp;
    nwSysInfo *psSysInfo;
    unsigned long lUsed;
    unsigned long lLeft;
    unsigned long lUsedrat;
    unsigned long lLeftrat;
    int iNum,k;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    printf("nwWebCompMon_SysInfo\n");
    utMsgPrintMsg(psMsgHead);
#endif
   ncOperWriteLog(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
   	                "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId);
//    if(caGtype[0] == '2' || caGtype[0] == '3') {
//        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0441请点击左边的设备名，显示相应计算机的当前信息"));
//        return 0;
 //   }

    lId = atol(caId);
    psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lId);
    if(psComp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0506设备不存在"));
        return 0;
    }
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&psComp->ip);
    if(psCompIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0507设备不在线"));
        return 0;
    }        
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }

    psCompIp->cSysInfo = 0;
    nwUtlReqSynInfo(psCompIp,NC_UDP_SYSINFO,1);
usleep(500);
//    sleep(1);
    lTime = time(0);
    lCount = 0;
/*    while(lCount < 5) { /* 等待接收信息  */
 /*      if(psCompIp->cSysInfo == 0) {
            if(lTime < time(0) - 2) {
                lTime = time(0);
                nwUtlReqSynInfo(psCompIp,NC_UDP_SYSINFO,1);
                lCount++;
            }
            usleep(100);
        }
        else {
            break;
        }
    }
*/
    psDbHead = utPltInitDb();
   
    iNum = 0;
    utPltPutVarF(psDbHead,"compid","%lu",psComp->compid);
    utPltPutVar(psDbHead,"devname",psComp->devname);
    utPltPutVar(psDbHead,"compname",psComp->compname);
    utPltPutVar(psDbHead,"os",psComp->os);
    utPltPutVar(psDbHead,"ie",psComp->ie);
    utPltPutVar(psDbHead,"ip",utComHostIp(htonl(psComp->ip)));
    utPltPutVar(psDbHead,"mac",ncCvtMac(psComp->mac));
    utPltPutVar(psDbHead,"version",psComp->version);
    if(psCompIp) {
        utPltPutVar(psDbHead,"status",ncLang("0831在线"));
        utPltPutVar(psDbHead,"syntime",utTimFormat("%Y-%m-%d %H:%M:%S",psCompIp->sDevHead.lSysTime)); 
        psSysInfo = psCompIp->sDevHead.sysInfo;
        if(psSysInfo) {
            utPltPutVar(psDbHead,"hdsum",utStrLtoF(psSysInfo->lHdsum,caTemp));
            utPltPutVar(psDbHead,"hdleft",utStrLtoF(psSysInfo->lHdleft,caTemp));
            utPltPutVar(psDbHead,"cdsum",utStrLtoF(psSysInfo->lCdsum,caTemp));
            utPltPutVar(psDbHead,"cdleft",utStrLtoF(psSysInfo->lCdleft,caTemp));
            utPltPutVar(psDbHead,"mdsum",utStrLtoF(psSysInfo->lMdsum,caTemp));
            utPltPutVar(psDbHead,"mdleft",utStrLtoF(psSysInfo->lMdleft,caTemp));
            utPltPutVar(psDbHead,"memsum",utStrLtoF(psSysInfo->lMemsum,caTemp));
            utPltPutVar(psDbHead,"memleft",utStrLtoF(psSysInfo->lMemleft,caTemp));
            utPltPutVarF(psDbHead,"hdcount","%d",psSysInfo->lDiskSum);
            utPltPutVarF(psDbHead,"cdcount","%d",psSysInfo->lCdCount);
            utPltPutVarF(psDbHead,"mdcount","%d",psSysInfo->lMdCount);
            
            if(psSysInfo->lMemsum>0){
            	lUsedrat=150-150*((float)psSysInfo->lMemleft/psSysInfo->lMemsum);
            	lLeftrat=150*((float)psSysInfo->lMemleft/psSysInfo->lMemsum);

            utPltPutVarF(psDbHead,"memusedrat","%d",lUsedrat);
            utPltPutVarF(psDbHead,"memleftrat","%d",lLeftrat);
          }
          else{
          	utPltPutVar(psDbHead,"memusedrat","0");
          	utPltPutVar(psDbHead,"memleftrat","0");
          }
          
         if(psSysInfo->lHdsum>0){
         	  lUsedrat=150-150*((float)psSysInfo->lHdleft/psSysInfo->lHdsum);
         	  lLeftrat=150*((float)psSysInfo->lHdleft/psSysInfo->lHdsum);
        	
            utPltPutVarF(psDbHead,"hdusedrat","%d",lUsedrat);
            utPltPutVarF(psDbHead,"hdfreerat","%d",lLeftrat);
          }
          else{
          	utPltPutVar(psDbHead,"hdusedrat","0");
          	utPltPutVar(psDbHead,"hdfreerat","0");
          }
          
         if(psSysInfo->lCdsum>0){
         	  lUsedrat=150-150*((float)psSysInfo->lCdleft/psSysInfo->lCdsum);
         	  lLeftrat=150*((float)psSysInfo->lCdleft/psSysInfo->lCdsum);
            utPltPutVarF(psDbHead,"cdusedrat","%d",lUsedrat);
            utPltPutVarF(psDbHead,"cdfreerat","%d",lLeftrat);
          }
          else{
          	utPltPutVar(psDbHead,"cdusedrat","0");
          	utPltPutVar(psDbHead,"cdfreerat","0");
          }
          
         if(psSysInfo->lMdsum>0){
         	lUsedrat=150-150*((float)psSysInfo->lMdleft/psSysInfo->lMdsum);
         	lLeftrat=150*((float)psSysInfo->lMdleft/psSysInfo->lMdsum);
            utPltPutVarF(psDbHead,"moveusedrat","%d",lUsedrat);
            utPltPutVarF(psDbHead,"movefreerat","%d",lLeftrat);
          }
          else{
          	utPltPutVar(psDbHead,"moveusedrat","0");
          	utPltPutVar(psDbHead,"movefreerat","0");
          }
            
            utPltPutVarF(psDbHead,"cpu","%lu",psSysInfo->lCpu);
            iNum = 0;
#ifdef LDEBUG
            printf(" SUmDisk==%d \n",psSysInfo->lDiskSum);
#endif
            for(i=0;i<psSysInfo->lDiskSum;i++) {
                iNum ++;
                printf(" %s  %lu %lu\n",psSysInfo->sD[i].caName,psSysInfo->sD[i].lSum,psSysInfo->sD[i].lLeft);

                utPltPutLoopVar(psDbHead,"driver", iNum,psSysInfo->sD[i].caName);
                utPltPutLoopVar(psDbHead,"dsum",   iNum,utStrLtoF(psSysInfo->sD[i].lSum,caTemp));
                utPltPutLoopVar(psDbHead,"dleft",  iNum,utStrLtoF(psSysInfo->sD[i].lLeft,caTemp));
                
          if(psSysInfo->sD[i].lSum>0){
          	lUsedrat=150-150*((float)psSysInfo->sD[i].lLeft/psSysInfo->sD[i].lSum);
          	lLeftrat=150*((float)psSysInfo->sD[i].lLeft/psSysInfo->sD[i].lSum);
            utPltPutLoopVarF(psDbHead,"driveusedrat",iNum,"%d",lUsedrat);
            utPltPutLoopVarF(psDbHead,"drivefreerat",iNum,"%d",lLeftrat);
          }
          else{
          	utPltPutLoopVar(psDbHead,"driveusedrat",iNum,"0");
          	utPltPutLoopVar(psDbHead,"drivefreerat",iNum,"0");
          }
            }
        }
    }
    else {
        utPltPutVar(psDbHead,"status",ncLang("0146不在线"));
    }

#ifdef LDEBUG
    printf(" caId==%s \n",caId);
#endif
    utPltPutVar(psDbHead,"id",caId);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_sysinfo.htm");
    return 0;
}


int nwWebCompMon_Screen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16];
    char caPath[128],caFilename[32],caFile[128];
    utPltDbHead *psDbHead;
    int i;
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lTime;
    nwCompIp  *psCompIp;
    nwCompInfo *psComp;
    nwCtlWatch *psNwwatch;
    char caScal[6];
    int iNum,k;
#ifdef LDEBUG
    printf("nwWebCompMon_Screen\n");
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,3,
   	       "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId,
					"scal",   UT_TYPE_STRING,2,caScal);
    if(caGtype[0] == '2' || caGtype[0] == '3') {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0441请点击左边的设备名，显示相应计算机的当前信息"));
        return 0;
    }

    lId = atol(caId);
    psComp = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lId);
    if(psComp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0506设备不存在"));
        return 0;
    }
    
    

    
    
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&psComp->ip);
    if(psCompIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0745用户不在线"));
        return 0;
    }
           
           
    psNwwatch = (nwCtlWatch *)utShmArray(psShmHead,NC_LNK_NWWATCH);
    if(psNwwatch==NULL){
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","共享内存错误","共享内存错误");
    	return -1;
    }
    lCount=utShmArrayRecord(psShmHead,NC_LNK_NWWATCH);
     for(i=0;i<lCount-1;i++){
       	if((psCompIp->compid==psNwwatch[i].uid&&psNwwatch[i].stype==1)||(psNwwatch[i].stype==3)||(psCompIp->psComp&&psCompIp->psComp->groupid==psNwwatch[i].uid&&psNwwatch[i].stype==2)){
       		if(psNwwatch[i].screen==0){
       			utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","客户端不允许监视屏幕","客户端不允许监视屏");
       			return 0;
       		}
       	}
      }        
           
           
           
           
            
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }
    if(lGroupid0 > 0 && psCompIp->psComp->groupid != lGroupid0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0378没有权限访问该用户"));
        return 0;
    }
#ifdef LDEBUG
    printf("Send Screen ... \n");
#endif
    psCompIp->cScreen = 0;
    strcpy(psCompIp->cRev,caScal);
    nwUtlReqScreenInfo(psShmHead,psCompIp);

    lTime = time(0);
    lCount = 0;
    while(lCount < 5) { /* 等待接收信息  */
        if(psCompIp->cScreen == 0) {
            if(lTime < time(0) - 2) {
                lTime = time(0);
                nwUtlReqScreenInfo(psShmHead,psCompIp);
                lCount++;
            }
            usleep(100);
        }
        else {
            break;
        }
    }

#ifdef LDEBUG
    printf("Send Screen ... %lu\n",psCompIp->sDevHead.lScrId);
#endif
    if(psCompIp->sDevHead.lScrId > 0) {
         sprintf(caPath,"%s/tmp/screen",utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog"));
         if(!utFileIsExist(caPath)){
         	mkdir(caPath,555);
        }
         sprintf(caFilename,"%lu.jpg",psCompIp->sDevHead.lScrId);
         sprintf(caFile,"%s/%s",caPath,caFilename);

         if(utFileIsExist(caFile)) {
            psDbHead = utPltInitDb();
            utPltPutVar(psDbHead,"syntime",utTimFormat("%y-%m-%d %H:%M:%S",psCompIp->sDevHead.lScrTime));
            utPltPutVarF(psDbHead,"sid","%lu",psCompIp->sDevHead.lScrId);
            utPltPutVarF(psDbHead,"devname","%s",psCompIp->psComp->devname);
            utPltPutVar(psDbHead,"id",caId);
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_mon_screen.htm");
            return 0;
         }
    }
    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0347客户端屏幕没抓到"));
    return 0;
}


/* 当前屏幕信息
 */
int nwWebCompMon_ShowScreen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    unsigned long lSid;
    char caFile[128],caPath[128],caFilename[32],caSid[16];
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,1,
   	                "sid",  UT_TYPE_STRING, 11, caSid);
    lSid = atol(caSid);
    if(lSid > 0) {
         sprintf(caPath,"%s/tmp/screen",utComGetVar_sd(psShmHead,"maillog","/home/ncmysql/nc/maillog"));
         sprintf(caFilename,"%lu.jpg",lSid);
         sprintf(caFile,"%s/%s",caPath,caFilename);
         utPltFileDownload(iFd,"image/jpeg",caPath,caFilename,caFilename);
         return 0;
    }
    utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0193当前进程"),ncLang("0347客户端屏幕没抓到"));
    return 0;
}



/* 显示程序日志 */
int nwWebCompMon_Proclog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caCurPg[12];
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lRowNum,lStartRec,lCurpg,lCompid;
    char caDevname[32],caMark[8],caProcname[72];
    unsigned long lTotpg,lSid,lMem,lCpu,lPid,lStime,lBtime;
    char caGroupid[16];
    
    int iNum,k;
    
    char caSmt[256],caSql[256];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    printf("nwWebCompMon_Proclog\n");
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
   	                "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId,
					"cond",   UT_TYPE_STRING, 31, caCond,
					"CurPg",      UT_TYPE_STRING, 11,  caCurPg);
    lId = atol(caId);
    if(utStrIsSpaces(caCurPg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurPg);
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }
    if(caGtype[0] == '2' || caGtype[0] == '3') {  /* 显示设备列表 */
        if(lGroupid0 > 0) {
            lGroupid = lGroupid0;
            sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lGroupid);
        }
        else {
            if(caGtype[0] == '2') {
                sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lId);
            }
            else {
                sprintf(caSmt," where a.compid = b.compid ");
            }
        }
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caSmt,"and (a.devname  like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,"or  b.procname like '%c%s%c') ",'%',caCond,'%');
        }
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwcompute a,nwproclog b %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
        psCur = pasDbOpenSqlF("select b.sid,a.compid,a.devname,a.groupid,b.mark,b.pid,b.stime,b.btime,b.procname,b.mem,b.cpu from nwcompute a,nwproclog b %s order by b.stime desc limit %lu,%lu",caSmt,lStartRec,lRowNum);
        if(psCur == NULL) {
printf("SQL:select b.sid,a.compid,a.devname,a.groupid,b.mark,b.pid,b.stime,b.btime,b.procname,b.mem,b.cpu from nwcompute a,nwproclog b %s order by b.stime desc limit %lu,%lu\n",caSmt,lStartRec,lRowNum);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_STRING,1,caMark,
                                       UT_TYPE_ULONG,4,&lPid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_ULONG,4,&lBtime,
                                       UT_TYPE_STRING,71,caProcname,
                                       UT_TYPE_ULONG,4,&lMem,
                                       UT_TYPE_ULONG,4,&lCpu);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"groupname", iNum,
                                   ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
            utPltPutLoopVarF(psDbHead,"compid", iNum,"%lu",lCompid);
            utPltPutLoopVar(psDbHead,"devname", iNum,caDevname);
            if(caMark[0] == 'a') {
                utPltPutLoopVar(psDbHead,"mark", iNum,ncLang("0436启动"));
            }
            else {
                utPltPutLoopVar(psDbHead,"mark", iNum,ncLang("0571停止"));
            }
            utPltPutLoopVarF(psDbHead,"pid", iNum,"%lu",lPid);
            if(lStime<3000000000)
            	utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%y-%m-%d %H:%M:%S",lStime));
            else
            	utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%y-%m-%d %H:%M:%S",time(0)));
            
            if(lStime<3000000000)
            	utPltPutLoopVar(psDbHead,"btime", iNum,utTimFormat("%y-%m-%d %H:%M:%S",lBtime));
            else
              utPltPutLoopVar(psDbHead,"btime", iNum,"");
              
            utPltPutLoopVarF(psDbHead,"name", iNum,"%s",caProcname);
            utPltPutLoopVarF(psDbHead,"mem", iNum,"%lu",lMem);
            utPltPutLoopVarF(psDbHead,"cpu", iNum,"%lu",lCpu);
                
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_STRING,1,caMark,
                                       UT_TYPE_ULONG,4,&lPid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_ULONG,4,&lBtime,
                                       UT_TYPE_STRING,71,caProcname,
                                       UT_TYPE_ULONG,4,&lMem,
                                       UT_TYPE_ULONG,4,&lCpu);
        }
        pasDbCloseCursor(psCur);
        k=0;
        lRowNum = 34;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"cond", caCond);
        utPltPutVar(psDbHead,"gtype",caGtype);
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_proclog.htm");
    }
    else {  /* 显示单个用户  */
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSmt," where  compid = %lu ",lId);
        k = 0;
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caSmt,"and (procname like '%c%s%c') ",'%',caCond,'%');
        }
        sprintf(caSql,"select count(*) from  nwproclog %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
        psCur = pasDbOpenSqlF("select sid,mark,pid,stime,btime,procname,mem,cpu from nwproclog  %s order by stime desc  limit %lu,%lu",caSmt,lStartRec,lRowNum);
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
                
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_STRING,1,caMark,
                                       UT_TYPE_ULONG,4,&lPid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_ULONG,4,&lBtime,
                                       UT_TYPE_STRING,71,caProcname,
                                       UT_TYPE_ULONG,4,&lMem,
                                       UT_TYPE_ULONG,4,&lCpu);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            if(caMark[0] == 'a') {
                utPltPutLoopVar(psDbHead,"mark", iNum,ncLang("0436启动"));
            }
            else {
                utPltPutLoopVar(psDbHead,"mark", iNum,ncLang("0571停止"));
            }
            utPltPutLoopVarF(psDbHead,"pid", iNum,"%lu",lPid);
        if(lStime<3000000000)
            	utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%y-%m-%d %H:%M:%S",lStime));
            else
            	utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%y-%m-%d %H:%M:%S",time(0)));
            
            if(lStime<3000000000)
            	utPltPutLoopVar(psDbHead,"btime", iNum,utTimFormat("%y-%m-%d %H:%M:%S",lBtime));
            else
              utPltPutLoopVar(psDbHead,"btime", iNum,"");
            utPltPutLoopVarF(psDbHead,"name", iNum,"%s",caProcname);
            utPltPutLoopVarF(psDbHead,"mem", iNum,"%lu",lMem);
            utPltPutLoopVarF(psDbHead,"cpu", iNum,"%lu",lCpu);

            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_STRING,1,caMark,
                                       UT_TYPE_ULONG,4,&lPid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_ULONG,4,&lBtime,
                                       UT_TYPE_STRING,71,caProcname,
                                       UT_TYPE_ULONG,4,&lMem,
                                       UT_TYPE_ULONG,4,&lCpu);
        }
        pasDbCloseCursor(psCur);
        k=0;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltPutVar(psDbHead,"cond", caCond);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_proclog_u.htm");
    }
}


/* 显示计算机监控菜单  */
int  nwWebCompOn_menu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caPlate[128],caGtype[16],caId[16],caOpt[2];
    int   iReturn;
    long lGtype,lId;
    utPltDbHead *psDbHead;
	iReturn = utMsgGetSomeNVar(psMsgHead,3,
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All 4-Polcy */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
	                "opt",     UT_TYPE_STRING, 1,  caOpt);
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"id",caId);
    utPltPutVar(psDbHead,"gtype",caGtype);
    utPltPutVar(psDbHead,"opt", caOpt);
    utPltPutVar(psDbHead,"dispname",nwGetCompName(atol(caGtype),atol(caId)));
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_online_menu.htm");
    return 0;
}


/* 显示程序日志 */
int nwWebCompMon_Cnflog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16],caCond[32],caCurPg[12];
    long lId,lGroupid0,lGroupid,lCount,lNum;
    unsigned long lIp,lRowNum,lStartRec,lCurpg,lCompid;
    char caDevname[32],caName[32],caValues[128];
    unsigned long lTotpg,lSid,lStime;
    char caGroupid[16];
    
    int iNum,k;
    
    char caSmt[256],caSql[256];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    printf("nwWebCompMon_Cnflog\n");
    utMsgPrintMsg(psMsgHead);
#endif
   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
   	                "gtype",  UT_TYPE_STRING, 11, caGtype,     /* 2--All  1--Group  0--User  */
					"id",     UT_TYPE_STRING, 15, caId,
					"cond",   UT_TYPE_STRING, 31, caCond,
					"CurPg",      UT_TYPE_STRING, 11,  caCurPg);
    lId = atol(caId);
    if(utStrIsSpaces(caCurPg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurPg);
    }
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) {
        lGroupid0 = 0;
    }
    if(caGtype[0] == '2' || caGtype[0] == '3') {  /* 显示设备列表 */
        if(lGroupid0 > 0) {
            lGroupid = lGroupid0;
            sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lGroupid);
        }
        else {
            if(caGtype[0] == '2') {
                sprintf(caSmt," where a.compid = b.compid and a.groupid = %lu ",lId);
            }
            else {
                sprintf(caSmt," where a.compid = b.compid ");
            }
        }
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caSmt,"and (a.devname  like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,"or  b.cname like '%c%s%c' ",'%',caCond,'%');
            utStrAddF(caSmt,"or  b.cvalues like '%c%s%c')",'%',caCond,'%');
        }
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSql,"select count(*) from  nwcompute a,nwconfiglog b %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
        psCur = pasDbOpenSqlF("select b.sid,a.compid,a.devname,a.groupid,b.sid,b.mtime,b.cname,b.cvalues from nwcompute a,nwconfiglog b %s order by b.mtime desc limit %lu,%lu",caSmt,lStartRec,lRowNum);
        if(psCur == NULL) {
printf("SQL:select b.sid,a.compid,a.devname,a.groupid,b.sid,b.stime,b.cname,b.cvalues from nwcompute a,nwconfiglog b %s order by b.mtime desc limit %lu,%lu \n",caSmt,lStartRec,lRowNum);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_STRING,31,caName,
                                       UT_TYPE_STRING,127,caValues);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"groupname", iNum,
                                   ncUtlGetGroupNameByid(psShmHead,lGroupid,"\0"));
            utPltPutLoopVarF(psDbHead,"compid", iNum,"%lu",lCompid);
            utPltPutLoopVar(psDbHead,"devname", iNum,caDevname);
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lStime));
            utPltPutLoopVarF(psDbHead,"name", iNum,"%s",caName);
            utPltPutLoopVarF(psDbHead,"values", iNum,"%s",caValues);
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lCompid,
                                       UT_TYPE_STRING,31,caDevname,
                                       UT_TYPE_ULONG,4,&lGroupid,
                                       UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_STRING,31,caName,
                                       UT_TYPE_STRING,127,caValues);
        }
        pasDbCloseCursor(psCur);
        k=0;
        lRowNum = 34;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"cond", caCond);
        utPltPutVar(psDbHead,"gtype",caGtype);
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_cnflog.htm");
    }
    else {  /* 显示单个用户  */
        lRowNum = 34;
        lStartRec = (lCurpg - 1) * lRowNum;
        sprintf(caSmt," where  compid = %lu ",lId);
        k = 0;
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caSmt,"and (cname like '%c%s%c' or cvalues like '%c%s%c') ",'%',caCond,'%','%',caCond,'%');
        }
        sprintf(caSql,"select count(*) from  nwconfiglog %s ",caSmt);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            lCount = 0;
        }
        psCur = pasDbOpenSqlF("select sid,mtime,cname,cvalues from nwconfiglog  %s order by mtime desc  limit %lu,%lu",caSmt,lStartRec,lRowNum);
        if(psCur == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
            return 0;
        }
        psDbHead = utPltInitDb();
                
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_STRING,31,caName,
                                       UT_TYPE_STRING,127,caValues);
        iNum = 0;
        while(iReturn == 0) {
            iNum++;
            utPltPutLoopVarF(psDbHead,"sid", iNum,"%lu",lSid);
            utPltPutLoopVar(psDbHead,"stime", iNum,utTimFormat("%Y-%m-%d %H:%M:%S",lStime));
            utPltPutLoopVarF(psDbHead,"name", iNum,"%s",caName);
            utPltPutLoopVarF(psDbHead,"values", iNum,"%s",caValues);
            iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                       UT_TYPE_ULONG,4,&lStime,
                                       UT_TYPE_STRING,31,caName,
                                       UT_TYPE_STRING,127,caValues);
        }
        pasDbCloseCursor(psCur);
        k=0;
        while(iNum < lRowNum) {
            k++;
            utPltPutLoopVarF(psDbHead,"snum", k,"%ld",iNum);
            iNum++;
        }
        if(lCount == 0) { 
            lTotpg = 0;
        }
        else {
            lTotpg = (lCount - 1) / lRowNum + 1;
        }
        utPltPutVar(psDbHead,"id",   caId);
        utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
        utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
        utPltPutVarF(psDbHead,"TotRec","%d",lCount);
        utPltPutVar(psDbHead,"cond", caCond);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_comp_cnflog_u.htm");
    }
}

//显示抓屏主画面
int nwWebScreenShowFrm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caGtype[16],caId[16];
    char cheId[256];
    char idg[40][16];
    int iNum=0;
    int m,n;
    int sumr,sumc,iHeight;
    char *p,*p2;
    int i;
    int iLen=0;
    utPltDbHead *psDbHead;

   	iReturn = utMsgGetSomeNVar(psMsgHead,2,
   	      "cheID",  UT_TYPE_STRING, 255, cheId,     
					"id",     UT_TYPE_STRING, 15, caId);
   
   if((strlen(caId)!=0)&&(strlen(cheId)==0)){
   	strcpy(cheId,caId);
  }
  if(strlen(cheId)!=0){
   	p=&cheId[0];
   	p2=strstr(p,",");
   	while(1){
   		if(p2){
   		iLen=p2-p;

   		memcpy(idg[iNum],p,iLen); 
   		idg[iNum][iLen]='\0';
   		iNum++;
   		p=p2+1;
   		p2=strstr(p,",");
   	  }
   	  else{
   	  	strcpy(idg[iNum],p);
   	  	break;
   	  }
	   	
   }

	 switch(iNum){
	 	case 0 :
	 		sumr=1;
	 		sumc=1;
	 		iHeight=560;
	 		break;
	 	case 1:
	    sumr=1;
	    sumc=2;
	    iHeight=560;
	    break;
	  case 2:
	  case 3:
	  	sumr=2;
	  	sumc=2;
	  	iHeight=280;
	  	break;
	  case 4:
	  case 5:
	  	sumr=2;
	  	sumc=3;
	  	iHeight=280;
	  	break;
	  case 6:
	  case 7:
	  case 8:
	  	sumr=3;
	  	sumc=3;
	  	iHeight=186;
	  	break;
	  default:
	   sumr=1;
	 	 sumc=1;	
	 		break;
	  }
	  i=0;
	 psDbHead = utPltInitDb();
	  for(m=0;m<sumr;m++){
	  	for(n=0;n<sumc;n++){
	  		utPltPutLoopVar2(psDbHead,"id",m+1,n+1,idg[i]);
	 		utPltPutLoopVarF2(psDbHead,"iheight",m+1,n+1,"%lu",iHeight);
	 		utPltPutLoopVarF2(psDbHead,"scal",m+1,n+1,"%lu",sumc);
	  		i++;
	  		if(i>iNum) break;
	  	}
	  }
  
}
    
    
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nw_screen_frm.htm");
    return 0;
  
}

/* 当前客户端即时通讯  */
int ncClientImList(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead)
{
    char caWhere[512],caCond[64],caCond0[64],caDel[16];
    char caTotPg[16],caCurPg[16],caUserid[16]="";
    char caUsername[32],caTo[64],caFrom[64],caMesg[256]="";
    char caTemp[256];
    char caCount[32];
    char caExport[16];
    char caSip[16];
	char caVar[32];
	char caPlate[64];
	char caGroupid[32]="";
	char camid[17]="";
	long lStime=0;
	long lEtime=0;
	char *p;
	int group_flag;
	char caSdate_temp[24],caEdate_temp[24],caSdate[20],caEdate[20],caSdate1[8],caEdate1[8];
    char caWhereStr[6]="where";
    unsigned long lUserid,lGroupid,lCurPg,lTotPg,lUserid0,lSid,lTime,lService;
    int i,iNum,iPos;
    utPltDbHead *psDbHead;
   	int iReturn;
   	char caSearch[16];
   	char desMesg[256]="";
   	long lGroupid0,lRowNum,lStartRec,lCount,lSumUser,lGroupid9;
   	long sum_count=0;
   	unsigned long lFuncode;
   	char username[33]="";
   	char groupname[33]="";
   	 struct msginfo_s {
   		unsigned long lUserid;
   		unsigned long lSid;
   		unsigned long lTime;
   		unsigned long lService;
   		unsigned long lFuncode;
   		char caFrom[48];
   		char caTo[48];
   		char caMesg[256];
   		char sip[16];
   		char dispname[32];
   	};
   	char caConflag[16];
   	struct msginfo_s *pMsgInfo;
   	unsigned long lcurtime;
   	char caImdir[256];
   	char caMsgfile[256];
   	char caMsg[300];
   	char caMsg1[300],caMsg2[300];
   	char caDir[128],caFile[128];
   	char caCont[256];
   	char caPathfile[256];
   	char caDispname[32];
   	char *p1,*p2;
   	int iLen;
   	char caTitlename[32];
   	unsigned long lpsCount,lptCount,ltCount;
 	pasDbCursor *psCur;
 	ncUserCont *psUser;
 	char caDevname[32];
 	char caIdfyno[20],caService[12],caFuncode[12],caIp[16];
   psDbHead = utPltInitDb();
//utMsgPrintMsg(psMsgHead); 
  utMsgDelVar(psMsgHead,"action");
	utMsgAdd_s(&psMsgHead,"action","06");
  ncOperWriteLog(psMsgHead);

	
	iReturn = utMsgGetSomeNVar(psMsgHead,4,		           
	                "id",     UT_TYPE_STRING, 12, caUserid,	                
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
					        "CurPg",      UT_TYPE_STRING, 12, caCurPg,
   	              "cond",       UT_TYPE_STRING, 63, caCond);
   memset(caDevname,0,sizeof(caDevname));               
   if(strlen(caUserid)>0){
   	sprintf(caTemp,"select devname from nwcompute where compid=%s ",caUserid);
   
   	pasDbOneRecord(caTemp,0,UT_TYPE_STRING,30,caDevname);
  }             
    sprintf(caCond0,"'%c%s%c'",'%',caCond,'%');

  
        lCurPg = atol(caCurPg);
        if(lCurPg <= 0) lCurPg = 1;
    
     memset(caWhere,0,sizeof(caWhere));
        if(!utStrIsSpaces(caUserid)){
        	utStrAddF(caWhere," and b.compid=%s ",caUserid);
        }    
        if(!utStrIsSpaces(caCond)) {
            utStrAddF(caWhere," and (b.mfrom like %s or b.mto like %s or b.mesg like %s or a.username like %s or a.dispname like %s)",
                                      caCond0,caCond0,caCond0,caCond0,caCond0);
        }
 
    lRowNum = 36;
    
    pMsgInfo = malloc(lRowNum * sizeof(struct msginfo_s));
    if(pMsgInfo == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","ncMsgInfo",
                               "Memory Error");
        return 0;
    }        
    memset(pMsgInfo,0,sizeof(pMsgInfo));
    lStartRec = (lCurPg - 1) * lRowNum;  
 


  //      if(strlen(caCond)>0){
        	sprintf(caTemp,"select count(*) from  ncimclient b,ncuser a where b.userid=a.userid %s",caWhere);
 //       }
//        else
 //       	sprintf(caTemp,"select count(*) from  ncimclient b,ncuser a where 1=1 %s",caWhere);
       
        iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0301记录查询出错 %s"),pasDbErrorMsg(NULL));
            return 0;
        }
  
 //       if(strlen(caCond)>0){
        	        psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),b.sid,b.userid,b.stime,b.service,b.mfrom,b.mto,b.mesg,b.funcode,a.dispname \
                from  ncimclient b,ncuser a where b.userid=a.userid %s order by b.stime desc limit %d,%d",caWhere,lStartRec,lRowNum);
//        }
//        else{
//        psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),b.sid,b.userid,b.stime,b.service,b.mfrom,b.mto,b.mesg,b.funcode \
//                from  ncimclient b where 1=1 %s order by b.sid desc limit %d,%d",caWhere,lStartRec,lRowNum);
//        }
  #ifdef LDEBUG
	 printf("caWhere=%s\n",caWhere);
	#endif     

    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
        return 0;
    }        
   
    iReturn = 0;
    utPltSetCvtHtml(0);
    utPltPutVar(psDbHead,"cond",caCond);
  
    utPltSetCvtHtml(1);
    lUserid=0;
    memset(caMesg,0,sizeof(caMesg));
    memset(caTo,0,sizeof(caTo));
    memset(caFrom,0,sizeof(caFrom));
    memset(caSip,0,sizeof(caSip));
    memset(caDispname,0,sizeof(caDispname));
    lFuncode=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,16,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_LONG,4,&lService,
  	                               UT_TYPE_STRING,47,caFrom,
  	                               UT_TYPE_STRING,47,caTo,
  	                               UT_TYPE_STRING,255,caMesg,
  	                               UT_TYPE_LONG,4,&lFuncode,
  	                               UT_TYPE_STRING,31,caDispname);
   	iNum = 0;
  
   	while((iReturn == 0||iReturn==1405) && iNum < lRowNum&& iNum < lCount) {
   	    i++;
      /*将caMesg按50字符宽度加回车*/ 

 		if(!utStrIsSpaces(caMesg)){
 			    if(lFuncode==4||lFuncode==5){
      	  strcpy(pMsgInfo[iNum].caMesg,caMesg);
      	}
      	else
	        ncStrFixedCol(caMesg,pMsgInfo[iNum].caMesg,48,255);
        } 
        else
        strcpy(pMsgInfo[iNum].caMesg,"");
        
        strcpy(pMsgInfo[iNum].dispname,caDispname);
        strcpy(pMsgInfo[iNum].sip,caSip);
        pMsgInfo[iNum].lUserid=lUserid;
        pMsgInfo[iNum].lSid=lSid;
        pMsgInfo[iNum].lTime=lTime;
        pMsgInfo[iNum].lService=lService;
        strcpy(pMsgInfo[iNum].caFrom,caFrom);
        strcpy(pMsgInfo[iNum].caTo,caTo);
        
        pMsgInfo[iNum].lFuncode=lFuncode;               
         iNum++;
    lUserid=0;  
    memset(caMesg,0,sizeof(caMesg));
    memset(caTo,0,sizeof(caTo));
    memset(caFrom,0,sizeof(caFrom));
    memset(caSip,0,sizeof(caSip));
    lFuncode=0; 
    memset(caDispname,0,sizeof(caDispname));       
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,16,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_LONG,4,&lService,
  	                               UT_TYPE_STRING,47,caFrom,
  	                               UT_TYPE_STRING,47,caTo,
  	                               UT_TYPE_STRING,255,caMesg,
  	                               UT_TYPE_LONG,4,&lFuncode,
  	                               UT_TYPE_STRING,31,caDispname);
   }
    pasDbCloseCursor(psCur);
    

    
    for(i=0;i<iNum;i++)
    {
    	utPltPutLoopVar(psDbHead,"service", iNum-i,ncUtlImName(pMsgInfo[i].lService));
        utPltPutLoopVarF(psDbHead,"num",   iNum-i,"%lu",i);
        utPltPutLoopVarF(psDbHead,"sid",   iNum-i,"%lu",pMsgInfo[i].lSid);
        utPltPutLoopVar(psDbHead,"sip",iNum-i,pMsgInfo[i].sip);
        utPltPutLoopVarF(psDbHead,"userid",   iNum-i,"%d",pMsgInfo[i].lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum-i,pMsgInfo[i].dispname);
//             ncUtlBufGetUsername(psShmHead,pMsgInfo[i].lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"sdate", iNum-i,utTimFormat("%m-%d %H:%M:%S",pMsgInfo[i].lTime));
        utPltPutLoopVar(psDbHead,"from", iNum-i,pMsgInfo[i].caFrom);
        utPltPutLoopVar(psDbHead,"to", iNum-i,pMsgInfo[i].caTo);
        if(pMsgInfo[i].lFuncode==1)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,"登录");
        else if(pMsgInfo[i].lFuncode==2)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,"退出");
        else if(pMsgInfo[i].lFuncode==3)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,"发送消息");
        else if(pMsgInfo[i].lFuncode==4)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,"发送文件");
        else if(pMsgInfo[i].lFuncode==5)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,"接收文件");
        else if(pMsgInfo[i].lFuncode==6)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum-i,"接收消息");
 
 
   
		  	  
        utPltPutLoopVar(psDbHead,"mesg", iNum-i,pMsgInfo[i].caMesg);
        
        psUser=ncUtlBufGetUserInfo(psShmHead,pMsgInfo[i].lUserid);
        if(psUser){
        	if(psUser->psGroup->groupname)
        	   utPltPutLoopVar(psDbHead,"dept",iNum-i,psUser->psGroup->groupname);
        	   
        	   
        	utPltPutLoopVar(psDbHead,"position",iNum-i,psUser->position);
        }
        
    }
    
    lTotPg = (lCount - 1) / lRowNum + 1;
    i = 0;
    while(iNum < lRowNum ) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    utPltPutVar(psDbHead,"devname",caDevname);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
   free(pMsgInfo); 
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nc_clientimonlinelist.htm");
   
    return 0;
}
