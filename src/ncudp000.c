#include <stdio.h>
#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include "utoall.h"
#include "ncdef.h"
#include "nwdef.h"
#include "pasdb.h"
#include "pasutl.h"

static int iSock=0;
int lanUtlLoadCompyInfo(utShmHead *psShmHead);
nwDevInfo *nwUtlAddDevInfo(nwDevHead *psHead,nwSwitchHead *psFree,char *pName,char *pAttr,int lNum);
nwDevInfo *nwUtlAddDevInfo0(nwDevHead *psHead,nwSwitchHead *psFree,char *pName,int iNum);
char  *nwUtlGetFreeBuf(nwSwitchHead *psFree);
int nwUtlSysInit(utShmHead *psShmHead)
{
    nwCompInfo *psComp;
    unsigned long lMaxUser,lMaxDev;
    int iReturn;
    
    /* 初始化在线计算机列表  */
    iReturn = nwUtlLoadUserInfo(psShmHead);
    if(iReturn != 0) {
        ncSysLog(NC_LOG_ERROR," nwUtlLoadUserInfo Error");
        return (-1);
    }
    
    //初始话单位
    lanUtlLoadCompyInfo(psShmHead);
    
       /* 初始化部门  */
    iReturn = lanUtlLoadGroupInfo(psShmHead);
    if(iReturn != 0) {
        ncSysLog(NC_LOG_ERROR," nwUtlGroupInfo Error");
        return (-1);
    }
 
        /* 装载软件下发策略  */
    iReturn = lanUtlLoadSoftUpdate(psShmHead);
 //   if(iReturn != 0) {
//        ncSysLog(NC_LOG_ERROR," lanUtlLoadSoftUpdate");
//        return (-1);
//    }
 
 
    /* 装载内网策略  */
    iReturn = lanUtlLoadProcy(psShmHead);
    if(iReturn != 0) {
        ncSysLog(NC_LOG_ERROR,"lanUtlLoadProcy");
        return (-1);
    }
 
 printf("iReturn=%d\n",iReturn);
 
 /*装载进程控制策略*/
 /*
    iReturn = nwUtlLoadProcCtl(psShmHead);
    if(iReturn != 0) {
        ncSysLog(NC_LOG_ERROR," nwUtlLoadUserInfo Error");
        return (-1);
    }
   */ 
    iReturn = nwUtlInitDevBuf(psShmHead);
    if(iReturn != 0) {
        ncSysLog(NC_LOG_ERROR," nwUtlInitDevBuf Error");
        return (-1);
    }        
    iSock = utComGetUdpSock();
printf(" iSock==%d \n",iSock);    

    return 0;
}


int nwUtlLoadProcCtl(utShmHead *psShmHead)
{
	/*
    pasDbCursor *psCur;
    nwProcCtl *psProc;
    nwCtlWatch *psNwwatch;
    nwDate *psDate;
    nwJoinCtl *psNwjoin;
    nwScreenRule *psNwscreen;
    unsigned lCount1,lCount2;
    long lStep;
    char caProcname[32];
    int iReturn,iCount;
    unsigned long lType,lId,lTimeType,lUrlid,lWarnid,lCid;
    unsigned long lStart,lStop,lConfig,lFlags,lSid,lVdate;
    long lStype,lCtype,lDid,lStime,lEtime;
    unsigned long lUid;
    long iProcCtlTime,lCount_watch;
    long lImrecord,lScreenrecord;
    long lCtl,lPid;
    char caVersion[12];
    char caT[20];
    long lLtype,lLower,lUpper;
    iProcCtlTime=ncGetLastInfo_l("ProcCtlLastTime",0);
   
   //将监视策略导入内存
    lCount_watch=0;
    iReturn = pasDbOneRecord("select count(*) from nwctlwatch ",0,UT_TYPE_LONG,4, &lCount_watch);
    if(iReturn != 0) {
        utSysLog(" Db Error [nwctlwatch] %s \n",pasDbErrorMsg(NULL));      
        return -1;
    }
    utShmFreeArray(psShmHead,NC_LNK_NWWATCH);
    iReturn = utShmInitArray(psShmHead,NC_LNK_NWWATCH,sizeof(nwCtlWatch),lCount_watch+1);
    psNwwatch = (nwCtlWatch *)utShmArray(psShmHead,NC_LNK_NWWATCH);
    if(psNwwatch == NULL) {
      
        utSysLog("psNwwatch Memory alloc error");
      
        return 0;
    }
    iCount=0;
    psCur = pasDbOpenSqlF("select stype,uid,imrecord,screenrecord from nwctlwatch order by stype ");
    if(psCur == NULL) {
        utSysLog(" nwctlwatch Open Cursor Error %s \n",pasDbErrorMsg(NULL));
       
        return 0;
    }
    lStype=0;
    lUid=0;
    lImrecord;
    lScreenrecord;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lStype,
                                   UT_TYPE_ULONG,  4, &lUid,
                                   UT_TYPE_LONG,4,&lImrecord,
                                   UT_TYPE_LONG,4,&lScreenrecord);
                                  
                                   
    while(iReturn == 0 || iReturn == PAS_DB_NULLVALUE) {
    	
    		psNwwatch[iCount].stype=lStype;
    		psNwwatch[iCount].uid=lUid;
    		psNwwatch[iCount].im=lImrecord;
    		psNwwatch[iCount].screen=lScreenrecord;
    		
    		iCount++;
    	
    lStype=0;
    lUid=0;
    lImrecord;
    lScreenrecord;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lStype,
                                   UT_TYPE_ULONG,  4, &lUid,
                                   UT_TYPE_LONG,4,&lImrecord,
                                   UT_TYPE_LONG,4,&lScreenrecord);
    	
    }	
    pasDbCloseCursor(psCur);
    
//将进程策略导入内存
   
    iReturn = pasDbOneRecord("select count(*) from nwctlproc where ctype=0 ",0,UT_TYPE_LONG,4, &lCount1);
    if(iReturn != 0) {
        utSysLog(" Db Error [nwctlproc] %s \n",pasDbErrorMsg(NULL));      
        return -1;
    }
    iReturn = pasDbOneRecord("select count(*) from nwctlproclist,nwproclist where pid=id and ctype=0 ",0,UT_TYPE_LONG,4, &lCount2);
    if(iReturn != 0) {
        utSysLog(" Db Error [nwctlproc] %s \n",pasDbErrorMsg(NULL));      
        return -1;
    }
    
// printf("proccount=%d\n",lCount1+lCount2);   
    
    if(lCount1+lCount2 == 0) {
        utSysLog(" No Record in nwctlproc");
        utShmFreeArray(psShmHead,NC_LNK_PROCCTL);
        iReturn = utShmInitArray(psShmHead,NC_LNK_PROCCTL,sizeof(nwProcCtl),1);
        psProc = (nwProcCtl *)utShmArray(psShmHead,NC_LNK_PROCCTL);
        psProc[0].stype=iProcCtlTime;
  //      return -1;
       goto kk;
    }
    
    utShmFreeArray(psShmHead,NC_LNK_PROCCTL);
    iReturn = utShmInitArray(psShmHead,NC_LNK_PROCCTL,sizeof(nwProcCtl),lCount1+lCount2+1);
    psProc = (nwProcCtl *)utShmArray(psShmHead,NC_LNK_PROCCTL);
    if(psProc == NULL) {
    //    pasDbCloseCursor(psCur);
        utSysLog("procctl Memory alloc error");
      
        return 0;
    }
    //将进程策略的最后更新时间放在第一个psProc[0].stype中
    psProc[0].stype=iProcCtlTime;
    psCur = pasDbOpenSqlF("select stype,uid,ctype,procname,did,stime,etime from nwctlproc where ctype=0 ");
    if(psCur == NULL) {
        utSysLog(" ncproc Open Cursor Error %s \n",pasDbErrorMsg(NULL));
       
        return 0;
    }
    
 

    iCount=1;
    lStype=0;
    lUid=0;
    lCtype=0;
    memset(caProcname,0,sizeof(caProcname));
    lDid=0;
    lStime=0;
    lEtime=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lStype,
                                   UT_TYPE_ULONG,  4, &lUid,
                                   UT_TYPE_LONG, 4, &lCtype,
                                   UT_TYPE_STRING,31,caProcname,
                                   UT_TYPE_LONG,  4, &lDid,
                                   UT_TYPE_LONG,  4, &lStime,
                                   UT_TYPE_LONG,  4,&lEtime);
                                   
    while(iReturn == 0 || iReturn == PAS_DB_NULLVALUE) {
    	if(strlen(caProcname)>0){
    		psProc[iCount].stype=lStype;
    		psProc[iCount].uid=lUid;
    		psProc[iCount].ctype=lCtype;
    		strcpy(psProc[iCount].pname,caProcname);
    		psProc[iCount].did=lDid;
    		strcpy(psProc[iCount].stime,ncLimCvtTime(lStime));
    		strcpy(psProc[iCount].etime,ncLimCvtTime(lEtime));
    		iCount++;
    	}
    	   lStype=0;
	    lUid=0;
	    lCtype=0;
	    memset(caProcname,0,sizeof(caProcname));
	    lDid=0;
	    lStime=0;
	    lEtime=0;
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lStype,
	                                   UT_TYPE_ULONG,  4, &lUid,
	                                   UT_TYPE_LONG, 4, &lCtype,
	                                   UT_TYPE_STRING,31,caProcname,
	                                   UT_TYPE_LONG,  4, &lDid,
	                                   UT_TYPE_LONG,  4, &lStime,
	                                   UT_TYPE_LONG,  4,&lEtime);
    	
    }	
    pasDbCloseCursor(psCur);
   
   
   
    psCur = pasDbOpenSqlF("select stype,uid,ctype,name,did,stime,etime from nwctlproclist,nwproclist where pid=id and ctype=0 ");
    if(psCur == NULL) {
        utSysLog(" ncproclist Open Cursor Error %s \n",pasDbErrorMsg(NULL));
        
        return 0;
    }
    
   

  
    lStype=0;
    lUid=0;
    lCtype=0;
    memset(caProcname,0,sizeof(caProcname));
    lDid=0;
    lStime=0;
    lEtime=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lStype,
                                   UT_TYPE_ULONG,  4, &lUid,
                                   UT_TYPE_LONG, 4, &lCtype,
                                   UT_TYPE_STRING,31,caProcname,
                                   UT_TYPE_LONG,  4, &lDid,
                                   UT_TYPE_LONG,  4, &lStime,
                                   UT_TYPE_LONG,  4,&lEtime);
                                   
    while(iReturn == 0 || iReturn == PAS_DB_NULLVALUE) {
    	if(strlen(caProcname)>0){
    		psProc[iCount].stype=lStype;
    		psProc[iCount].uid=lUid;
    		psProc[iCount].ctype=lCtype;
    		strcpy(psProc[iCount].pname,caProcname);
    		psProc[iCount].did=lDid;
    		strcpy(psProc[iCount].stime,ncLimCvtTime(lStime));
    		strcpy(psProc[iCount].etime,ncLimCvtTime(lEtime));
    		iCount++;
    	}
    	   lStype=0;
	    lUid=0;
	    lCtype=0;
	    memset(caProcname,0,sizeof(caProcname));
	    lDid=0;
	    lStime=0;
	    lEtime=0;
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lStype,
	                                   UT_TYPE_ULONG,  4, &lUid,
	                                   UT_TYPE_LONG, 4, &lCtype,
	                                   UT_TYPE_STRING,31,caProcname,
	                                   UT_TYPE_LONG,  4, &lDid,
	                                   UT_TYPE_LONG,  4, &lStime,
	                                   UT_TYPE_LONG,  4,&lEtime);
    	
    }	
    pasDbCloseCursor(psCur);
   
   
   
   
   
   //将准入规则导入内存
kk:
 
 lCount2=0;
    iReturn = pasDbOneRecord("select count(*) from nwctljoinlist,nwjoinclassdet where pid=id and ctype=0 ",0,UT_TYPE_LONG,4, &lCount2);
    printf("iReturna=%d\n",iReturn);
    if(iReturn != 0) {
        utSysLog(" Db Error [nwctljoinlist] %s \n",pasDbErrorMsg(NULL));      
        return -1;
    }
    
// printf("proccount=%d\n",lCount1+lCount2);   
    
 
   
    utShmFreeArray(psShmHead,NC_LNK_NWJOIN);
    iReturn = utShmInitArray(psShmHead,NC_LNK_NWJOIN,sizeof(nwJoinCtl),lCount2+1);
    psNwjoin = (nwJoinCtl *)utShmArray(psShmHead,NC_LNK_NWJOIN);
    if(psNwjoin == NULL) {
    //    pasDbCloseCursor(psCur);
        utSysLog("procctl Memory alloc error");
//  printf("vvvvvvvv\n");    
        return 0;
    }
    //将进程策略的最后更新时间放在第一个psProc[0].stype中
    psProc = (nwProcCtl *)utShmArray(psShmHead,NC_LNK_PROCCTL);
    psProc[0].stype=iProcCtlTime;
    iCount=0;
   
    psCur = pasDbOpenSqlF("select stype,uid,ctype,LOWER(name),did,stime,etime,version,ctl,pid from nwctljoinlist,nwjoinclassdet where pid=id and ctype=0 order by pid ");
    if(psCur == NULL) {
        utSysLog(" ncproclist Open Cursor Error %s \n",pasDbErrorMsg(NULL));
        
        return 0;
    }
    
   

  
    lStype=0;
    lUid=0;
    lCtype=0;
    memset(caProcname,0,sizeof(caProcname));
    lDid=0;
    lStime=0;
    lEtime=0;
    memset(caVersion,0,sizeof(caVersion));
    lCtl=0;
    lPid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lStype,
                                   UT_TYPE_ULONG,  4, &lUid,
                                   UT_TYPE_LONG, 4, &lCtype,
                                   UT_TYPE_STRING,31,caProcname,
                                   UT_TYPE_LONG,  4, &lDid,
                                   UT_TYPE_LONG,  4, &lStime,
                                   UT_TYPE_LONG,  4,&lEtime,
                                   UT_TYPE_STRING,9,caVersion,
                                   UT_TYPE_LONG,4,&lCtl,
                                   UT_TYPE_LONG,4,&lPid);
                                   
    while(iReturn == 0 || iReturn == PAS_DB_NULLVALUE) {
    	if(strlen(caProcname)>0){
    		psNwjoin[iCount].stype=lStype;
    		psNwjoin[iCount].uid=lUid;
    		psNwjoin[iCount].ctype=lCtype;
    		strcpy(psNwjoin[iCount].pname,caProcname);
    		psNwjoin[iCount].did=lDid;
    		strcpy(psNwjoin[iCount].stime,ncLimCvtTime(lStime));
    		strcpy(psNwjoin[iCount].etime,ncLimCvtTime(lEtime));
    		strcpy(psNwjoin[iCount].version,caVersion);
    		psNwjoin[iCount].ctl=lCtl;
    		psNwjoin[iCount].id=lPid;
    		iCount++;
    	}
    lStype=0;
    lUid=0;
    lCtype=0;
    memset(caProcname,0,sizeof(caProcname));
    lDid=0;
    lStime=0;
    lEtime=0;
    memset(caVersion,0,sizeof(caVersion));
    lCtl=0;
    lPid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lStype,
                                   UT_TYPE_ULONG,  4, &lUid,
                                   UT_TYPE_LONG, 4, &lCtype,
                                   UT_TYPE_STRING,31,caProcname,
                                   UT_TYPE_LONG,  4, &lDid,
                                   UT_TYPE_LONG,  4, &lStime,
                                   UT_TYPE_LONG,  4,&lEtime,
                                   UT_TYPE_STRING,9,caVersion,
                                   UT_TYPE_LONG,4,&lCtl,
                                   UT_TYPE_LONG,4,&lPid);
    	
    }	
    pasDbCloseCursor(psCur);
   
   
   
   
     //将日期导入内存

 
    lCount2=0;
    iReturn = pasDbOneRecord("select count(*) from nclimdate ",0,UT_TYPE_LONG,4, &lCount2);
   
    if(iReturn != 0) {
        utSysLog(" Db Error [nclimdate] %s \n",pasDbErrorMsg(NULL));      
        return -1;
    }
    
// printf("proccount=%d\n",lCount1+lCount2);   
 
    utShmFreeArray(psShmHead,NC_LNK_NWDATEPROCY);
    iReturn = utShmInitArray(psShmHead,NC_LNK_NWDATEPROCY,sizeof(nwDate),lCount2+1);
    psDate = (nwDate *)utShmArray(psShmHead,NC_LNK_NWDATEPROCY);
    if(psDate == NULL) {
    //    pasDbCloseCursor(psCur);
        utSysLog("psDate Memory alloc error");
//  printf("vvvvvvvv\n");    
        return 0;
    }
    //将进程策略的最后更新时间放在第一个psProc[0].stype中
   
    iCount=0;
   
    psCur = pasDbOpenSqlF("select id,ltype,llower,lupper from nclimdate  order by id,sid ");
    if(psCur == NULL) {
        utSysLog(" nclimdate Open Cursor Error %s \n",pasDbErrorMsg(NULL));
        
        return 0;
    }
    
   
    lId=0;
    lLtype=0;
    lLower=0;
    lUpper=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lId,
                                   UT_TYPE_LONG,  4, &lLtype,
                                   UT_TYPE_LONG, 4, &lLower,
                                   UT_TYPE_LONG,4,&lUpper);
                                  
                                   
    while(iReturn == 0 || iReturn == PAS_DB_NULLVALUE) {
    	
    		psDate[iCount].id=lId;
    		psDate[iCount].ltype=lLtype;
    		psDate[iCount].llower=lLower;
    		psDate[iCount].lupper=lUpper;
    		
    		iCount++;
    	
    lId=0;
    lLtype=0;
    lLower=0;
    lUpper=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lId,
                                   UT_TYPE_ULONG,  4, &lLtype,
                                   UT_TYPE_LONG, 4, &lLower,
                                   UT_TYPE_LONG,4,&lUpper);
    	
    }	
    pasDbCloseCursor(psCur); 
   

//将屏幕监控规则导入到内存
 lCount2=0;
    iReturn = pasDbOneRecord("select count(*) from nwscreenrule where 1=1 ",0,UT_TYPE_LONG,4, &lCount2);
    printf("iReturnscree=%d\n",iReturn);
    if(iReturn != 0) {
        utSysLog(" Db Error [nwctljoinlist] %s \n",pasDbErrorMsg(NULL));      
        return -1;
    }
    
// printf("proccount=%d\n",lCount1+lCount2);   
    
 
  //将进程策略的最后更新时间放在第一个psProc[0].stype中
    utShmFreeArray(psShmHead,NC_LNK_NWSCREEN);
    iReturn = utShmInitArray(psShmHead,NC_LNK_NWSCREEN,sizeof(nwScreenRule),lCount2+1);
    psNwscreen = (nwScreenRule *)utShmArray(psShmHead,NC_LNK_NWSCREEN);
    if(psNwscreen == NULL) {
    //    pasDbCloseCursor(psCur);
        utSysLog("procctl Memory alloc error");
//  printf("vvvvvvvv\n");    
        return 0;
    }
  
   
    psProc[0].stype=iProcCtlTime;
    iCount=0;
 //  printf("select stype,uid,ctype,did,stime,etime,step from nwscreenrule where 1=1 order by stype\n");
    psCur = pasDbOpenSqlF("select stype,uid,ctype,did,stime,etime,step from nwscreenrule where 1=1 order by stype ");
    if(psCur == NULL) {
        utSysLog(" ncproclist Open Cursor Error %s \n",pasDbErrorMsg(NULL));
        
        return 0;
    }
    
   

  
    lStype=0;
    lUid=0;
    lCtype=0;
    lStep=0;
    lDid=0;
    lStime=0;
    lEtime=0; 
    lCtl=0;
    lPid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lStype,
                                   UT_TYPE_ULONG,  4, &lUid,
                                   UT_TYPE_LONG, 4, &lCtype,
                                   UT_TYPE_LONG,  4, &lDid,
                                   UT_TYPE_LONG,  4, &lStime,
                                   UT_TYPE_LONG,  4,&lEtime,
                                   UT_TYPE_LONG,4,&lStep);
                                  
                                   
    while(iReturn == 0 || iReturn == PAS_DB_NULLVALUE) {
 
    		psNwscreen[iCount].stype=lStype;
    		psNwscreen[iCount].uid=lUid;
    		psNwscreen[iCount].ctype=lCtype;
    		
    		psNwscreen[iCount].did=lDid;
    		strcpy(psNwscreen[iCount].stime,ncLimCvtTime(lStime));
    		strcpy(psNwscreen[iCount].etime,ncLimCvtTime(lEtime));

    		psNwscreen[iCount].step=lStep;
    		iCount++;
   
    lStype=0;
    lUid=0;
    lCtype=0;
    lStep=0;
    lDid=0;
    lStime=0;
    lEtime=0; 
    lCtl=0;
    lPid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lStype,
                                   UT_TYPE_ULONG,  4, &lUid,
                                   UT_TYPE_LONG, 4, &lCtype,
                                   UT_TYPE_LONG,  4, &lDid,
                                   UT_TYPE_LONG,  4, &lStime,
                                   UT_TYPE_LONG,  4,&lEtime,
                                   UT_TYPE_LONG,4,&lStep);
    	
    }	
    pasDbCloseCursor(psCur);

printf("iCount=%d\n",iCount);

   
   */

    return 0;    
}



/* 设备缓存主要用来保存数据同步时当前设备信息。目的是为提高系统的效率。
   
    MaxDevBuf  --- 缓存中最大记录数
 */

int nwUtlInitDevBuf(utShmHead *psShmHead)
{
    unsigned long lMaxDev,lSize;
    int         iReturn,i;
    char        *p;
    nwSwitchHead   *psHead;
    nwSwitchBuf   *psDev;
    
    lMaxDev = utComGetVar_ld(psShmHead,"MaxDevBuf",5000);
    lSize = (lMaxDev + 1) * sizeof(nwSwitchBuf);
    utShmFreeArray(psShmHead,NC_LNK_DEVINFO);
    iReturn = utShmInitArray(psShmHead,NC_LNK_DEVINFO,1,lSize);
    if(iReturn < 0) {
        ncSysLog(NC_LOG_ERROR,"NC_LNK_DEVINFO init Error(share mem)");
        return (-1);
    }
    p = utShmArray(psShmHead,NC_LNK_DEVINFO);
    memset(p,0,lSize);
    psHead = (nwSwitchHead *)p;
    psDev  = (nwSwitchBuf *)(p + sizeof(nwSwitchHead));
    for(i=0;i<lMaxDev-1;i++) {
        psDev[i].next = &psDev[i+1];
    }
    psDev[lMaxDev-1].next = NULL;
    psHead->first = psDev;
    return 0;
}



/* 向客户端发送消息  */
int nwUtlSendUdpMsg(nwCompIp *psComp,utMsgHead *psMsgHead)
{
    int iSock1,iReturn;
    utComSockAddr sSockAddr;
/*
    if(iSock == 0) {
        iSock = utComGetUdpSock();
    }
    if(iSock == 0) {
        iSock = utComUdpSock();
    }
    */
    iSock1 = utComUdpSock();
 //   printf(" Send UdpMessage %s:%d   %d iSock==%d\n",utComHostIp(htonl(psComp->ip)),psComp->nUdp,htons(psComp->nUdp),iSock1);
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
    utComSetAddr0(&sSockAddr,htonl(psComp->ip),htons(psComp->nUdp));
    iReturn = utComUdpMsgSendTo(iSock1,&sSockAddr,psMsgHead);
    close(iSock1);
    return iReturn;
}

/* 向客户端发送同步请求  */
int nwUtlReqSynInfo(nwCompIp *psComp,int iFun,int iStart)
{
	   utMsgHead *psMsgHead;
    psMsgHead = utMsgInit(NW_MSG_CODE_REQ,iFun,utStrGetId(),4096);
    utMsgAddSomeVar(&psMsgHead,1,"start",UT_TYPE_LONG,iStart);
    
    
    
    nwUtlSendUdpMsg(psComp,psMsgHead);
    utMsgFree(psMsgHead);
    return 0;
	

}

/* 向客户端发送进程策略  */
int nwUtlSendProcCtl(utShmHead *psShmHead,nwCompIp *psComp,int iFun,int iStart)
{
	int lSj;
	int lCount,iNum;
    utMsgHead *psMsgHead=NULL;
    nwProcCtl *psProc;
    nwCtlWatch *psNwwatch;
    nwJoinCtl *psNwjoin;
    char ctlvar[16],ctlcont[128];
    int i;
    long lIm,lScreen;

    psProc = (nwProcCtl *)utShmArray(psShmHead,NC_LNK_PROCCTL);
    if(psProc==NULL){
    	return -1;
    }
    lSj=0;
    lCount=utShmArrayRecord(psShmHead,NC_LNK_PROCCTL);
	 
	 
	    
    iNum=0;
    for(i=1;i<lCount;i++){

    	if((psComp->compid==psProc[i].uid&&psProc[i].stype==1)||(psProc[i].stype==3)||(psComp->psComp&&psComp->psComp->groupid==psProc[i].uid&&psProc[i].stype==2)){
    		lSj++;
    		if(iNum==0){
    	    psMsgHead = utMsgInit(NW_MSG_CODE_REQ,iFun,utStrGetId(),4096);
          utMsgAddSomeVar(&psMsgHead,1,"start",UT_TYPE_LONG,iStart);
        }    	
        iStart++;	
    		sprintf(ctlvar,"Ctl%d",iNum+1);
    		sprintf(ctlcont,"P;%d;%s,%s,%s,%d,0",iNum,psProc[i].stime,psProc[i].etime,psProc[i].pname,psProc[i].ctype);
    		utMsgAdd_s(&psMsgHead,ctlvar,ctlcont);
    		iNum++;
    		if(iNum==12||i>=lCount){
    			 utMsgAdd_l(&psMsgHead,"mtime",psProc[0].stype);
           utMsgAdd_l(&psMsgHead,"sum",iNum);
 //          utMsgPrintMsg(psMsgHead);
           nwUtlSendUdpMsg(psComp,psMsgHead);
           utMsgFree(psMsgHead);
           sleep(1);
           iNum=0;
    		}
    	}
    	
    	
    }
    if((lCount==1)||(lSj==0)) {
    	   psMsgHead = utMsgInit(NW_MSG_CODE_REQ,iFun,utStrGetId(),4096);
         utMsgAddSomeVar(&psMsgHead,1,"start",UT_TYPE_LONG,1);
         sprintf(ctlcont,"T;0;0,0,0,0,0");
         iNum=1;
         sprintf(ctlvar,"Ctl1");
    		utMsgAdd_s(&psMsgHead,ctlvar,ctlcont);
         
    }
    
    
   if(psMsgHead){
   	    	utMsgAdd_l(&psMsgHead,"mtime",psProc[0].stype);
           utMsgAdd_l(&psMsgHead,"sum",iNum);
 //       utMsgPrintMsg(psMsgHead);
           nwUtlSendUdpMsg(psComp,psMsgHead);
           utMsgFree(psMsgHead);

  }


    //发送监视策略
    psNwwatch = (nwCtlWatch *)utShmArray(psShmHead,NC_LNK_NWWATCH);
    if(psNwwatch==NULL){
    	return -1;
    }
    lCount=utShmArrayRecord(psShmHead,NC_LNK_NWWATCH);

     lIm=9;
     lScreen=9;
     for(i=0;i<lCount-1;i++){
     	
  
       	if((psComp->compid==psNwwatch[i].uid&&psNwwatch[i].stype==1)||(psNwwatch[i].stype==3)||(psComp->psComp&&psComp->psComp->groupid==psNwwatch[i].uid&&psNwwatch[i].stype==2)){
       		if(psNwwatch[i].im!=9&&lIm==9){
       			lIm=psNwwatch[i].im;
       		}
       		if(psNwwatch[i].screen!=9&&lScreen==9){
       			lScreen=psNwwatch[i].screen;
       		}
       		
       		
       		

       		
       		if((lIm!=9&&lScreen!=9)||((i>=lCount-2)&&(lIm!=9||lScreen!=9))){
       			
       	  if(lIm==9) lIm=1;
       	  if(lScreen==9) lScreen=1;
       	  
       	  psMsgHead = utMsgInit(NW_MSG_CODE_REQ,iFun,utStrGetId(),4096);
       		if(psMsgHead==NULL){
       			return 0;
       		}
          utMsgAddSomeVar(&psMsgHead,1,"start",UT_TYPE_LONG,0);
          
          utMsgAdd_l(&psMsgHead,"mtime",psProc[0].stype);
          utMsgAdd_l(&psMsgHead,"sum",1);
           sprintf(ctlvar,"Ctl%d",1);
   		     sprintf(ctlcont,"S;1;%d,%d",lIm,lScreen);
   		    utMsgAdd_s(&psMsgHead,ctlvar,ctlcont);
//          utMsgPrintMsg(psMsgHead);   		  
   		  
           nwUtlSendUdpMsg(psComp,psMsgHead);
                   
           utMsgFree(psMsgHead);
           break;
          }
       	}
      }


//发送准入规则

    psNwjoin = (nwJoinCtl *)utShmArray(psShmHead,NC_LNK_NWJOIN);
    if(psNwjoin==NULL){
    	printf("hhhhhhhhhh\n");
    	return -1;
    }
    lSj=0;
    lCount=utShmArrayRecord(psShmHead,NC_LNK_NWJOIN);
 //   printf("lCount=%d\n",lCount);
    iStart=1;
    iNum=0;
    for(i=0;i<lCount;i++){

    	if((psComp->compid==psNwjoin[i].uid&&psNwjoin[i].stype==1)||(psNwjoin[i].stype==3)||(psComp->psComp&&psComp->psComp->groupid==psNwjoin[i].uid&&psNwjoin[i].stype==2)){
    		lSj++;
    		if(iNum==0){
    	    psMsgHead = utMsgInit(NW_MSG_CODE_REQ,iFun,utStrGetId(),4096);
          utMsgAddSomeVar(&psMsgHead,1,"start",UT_TYPE_LONG,iStart);
        }    	
        iStart++;	
    		sprintf(ctlvar,"Ctl%d",iNum+1);
    		sprintf(ctlcont,"J;%d;%s,%s,%s,%s,%d,%d",iNum,psNwjoin[i].stime,psNwjoin[i].etime,psNwjoin[i].pname,psNwjoin[i].version,psNwjoin[i].ctl,psNwjoin[i].id);
    		utMsgAdd_s(&psMsgHead,ctlvar,ctlcont);
    		iNum++;
    		if(iNum==12||i>=lCount){
    			 utMsgAdd_l(&psMsgHead,"mtime",psProc[0].stype);
           utMsgAdd_l(&psMsgHead,"sum",iNum);
 //          utMsgPrintMsg(psMsgHead);
           nwUtlSendUdpMsg(psComp,psMsgHead);
           utMsgFree(psMsgHead);
           sleep(1);
           iNum=0;
    		}
    	}
    	
    	
    }

    if((lCount==1)||(lSj==0)) {
    	   psMsgHead = utMsgInit(NW_MSG_CODE_REQ,iFun,utStrGetId(),4096);
         utMsgAddSomeVar(&psMsgHead,1,"start",UT_TYPE_LONG,1);
         sprintf(ctlcont,"M;0;0,0,0,0,0");
         iNum=1;
         sprintf(ctlvar,"Ctl1");
    		utMsgAdd_s(&psMsgHead,ctlvar,ctlcont);
  
    }
    
    
   if(psMsgHead){
   	    	utMsgAdd_l(&psMsgHead,"mtime",psProc[0].stype);
           utMsgAdd_l(&psMsgHead,"sum",iNum);
 //    utMsgPrintMsg(psMsgHead);
           nwUtlSendUdpMsg(psComp,psMsgHead);
           utMsgFree(psMsgHead);

  }













    return 0;
}

/* 请求截屏信息  */
int nwUtlReqScreenInfo(utShmHead *psShmHead,nwCompIp *psComp)
{
    utMsgHead *psMsgHead;
    long lPort;
    unsigned long lSid;
    psMsgHead = utMsgInit(NW_MSG_CODE_REQ,NC_UDP_CAPSCREEN,utStrGetId(),4096);
    lPort = utComGetVar_ld(psShmHead,"TcpPort",8195);
    lSid = utStrGetId();
    utMsgAddSomeVar(&psMsgHead,3,"tcpport",UT_TYPE_LONG,lPort,
                                 "sid",UT_TYPE_LONG,lSid,
                                 "scal",UT_TYPE_LONG,atol(psComp->cRev));

    nwUtlSendUdpMsg(psComp,psMsgHead);
    utMsgFree(psMsgHead);
    return 0;
}

/* 请求客户端升级命令  */
int nwUtlReqUpdateInfo(utShmHead *psShmHead,nwCompIp *psComp,char *pFile)
{
    utMsgHead *psMsgHead;
    long lPort,iSize;
    char caFile[128];
    unsigned long lSid;
    sprintf(caFile,"%s/html/soft/%s",utComGetVar_sd(psShmHead,"NcHome","/home/ncmysql/nc"),pFile);
    iSize = utFileSize(caFile);
    if(iSize>0){
    psMsgHead = utMsgInit(NW_MSG_CODE_REQ,NC_NWPROCUPDATE_REQ,utStrGetId(),4096);
    lPort = utComGetVar_ld(psShmHead,"TcpPort",8195);

    lSid = utStrGetId();
    utMsgAddSomeVar(&psMsgHead,3,"tcpport",UT_TYPE_LONG,lPort,
                                 "filelen",UT_TYPE_LONG,iSize,
                                 "filename",UT_TYPE_STRING,pFile);
                                
    nwUtlSendUdpMsg(psComp,psMsgHead);
    utMsgFree(psMsgHead);
  } 
    return 0;
}

/* 客户端升级结束命令  */
int nwUtlReqUpdateEnd(utShmHead *psShmHead,nwCompIp *psComp)
{
    utMsgHead *psMsgHead;
  
    psMsgHead = utMsgInit(NW_MSG_CODE_REQ,NC_NWPROCUPDATE_END,utStrGetId(),4096);
    

    utMsgAddSomeVar(&psMsgHead,1,"UPDATEEND",UT_TYPE_LONG,1);
                                 
                                
    nwUtlSendUdpMsg(psComp,psMsgHead);
    utMsgFree(psMsgHead);
  
    return 0;
}

/* 返回  a--增加  d--删除  m--修改  */
int nwUtlCvtDevInfo(char *pIn,char *pDname,char *pAttr,unsigned long *lValue)
{
    
    char *p;
    char caMark[2],caDev[32],caValue[32],caProvide[64];
    unsigned long long ll;
    
    p = pIn;
    strcpy(caMark,"\0");
    strcpy(caDev,"\0");
    strcpy(pAttr,"\0");
    strcpy(caValue,"\0");
    strcpy(caProvide,"\0");
    p = utStrGetWord(p,caMark,1,";");    
    if(*p != ';') {
        return (-1);
    }
    else {
        p++;
    }

    p = utStrGetWord(p,caDev,31,";");    
    if(*p != ';') {
        return (-1);
    }
    else {
        p++;
    }
    
    p = utStrGetWord(p,pAttr,127,";");    
    if(*p != ';') {
        return (-1);
    }
    else {
        p++;
    }
   
    p = utStrGetWord(p,caProvide,127,";");    
    if(*p != ';') {
        return (-1);
    }
    else {
        p++;
    }
    p = utStrGetWord(p,caValue,31,";");    
    
    if(strncasecmp(caDev,"Processor",9) == 0) {
        strcpy(pDname,ncLang("0178处理器"));
        *lValue = atol(caValue);
        if(*lValue!=0) {
            utStrAddF(pAttr,ncLang("0035,主频:%lu"),*lValue);
        }
    }
    else if(strncasecmp(caDev,"BaseBoard",9) == 0) {
        strcpy(pDname,ncLang("0869主板"));
        *lValue = 0;
    }
    else if(strncasecmp(caDev,"DiskDrive",9) == 0) {
        strcpy(pDname,ncLang("0727硬盘"));
        sscanf(caValue,"%llu",&ll);
        if(ll > 0) {
            *lValue = ll / 1073741824L ;
            utStrAddF(pAttr,ncLang("0032,容量:%lu GB"),*lValue);
        }
    }
    else if(strncasecmp(caDev,"PhysicalMemory",14) == 0) {
        strcpy(pDname,ncLang("0402内存"));
        sscanf(caValue,"%llu",&ll);
        if(ll > 0) {
            *lValue = ll / 1048576L ;
            utStrAddF(pAttr,ncLang("0033,容量:%lu MB"),*lValue);
        }
    }
    else if(strncasecmp(caDev,"VideoController",14) == 0) {
        strcpy(pDname,ncLang("0681显卡"));
        sscanf(caValue,"%llu",&ll);
        if(ll > 0) {
            *lValue = ll / 1048576L ;
            utStrAddF(pAttr,ncLang("0034,显存:%lu MB"),*lValue);
        }
    }
    else if(strncasecmp(caDev,"SoundDevice",11) == 0) {
        strcpy(pDname,ncLang("0514声卡"));
        *lValue = 0;
    }
    else if(strncasecmp(caDev,"NetworkAdapter",14) == 0) {
        strcpy(pDname,ncLang("0607网络设备"));
        *lValue = 0;
    }
    else if(strncasecmp(caDev,"CDROMDrive",10) == 0) {
        strcpy(pDname,ncLang("0273光驱"));
        *lValue = 0;
    }
    else if(strncasecmp(caDev,"Keyboard",8) == 0) {
        strcpy(pDname,ncLang("0325键盘"));
        *lValue = 0;
    }
    else if(strncasecmp(caDev,"Printer",7) == 0) {
        strcpy(pDname,ncLang("0185打印机"));
        *lValue = 0;
    }
    else if(strncasecmp(caDev,"PointingDevice",13) == 0) {
        strcpy(pDname,ncLang("0207点设备"));
        *lValue = 0;
    }
    else {
        strncpy(pDname,caDev,15);
        *lValue = 0;
    }
    return caMark[0];
}

        
/* 显示缓存链表的内容  */    
int nwUtlPrintDevInfo(nwDevHead *psDevHead)
{
    nwDevInfo *ps;
    int iNum=0;
    printf(" Dev Info...\n");
    ps = psDevHead->firstDev;
    while(ps) {
        iNum++;
        printf("[%3d][%-16s][%d] [%c] [%8lu] [%9lu]%s\n",iNum,
            ps->caName,ps->nNum,ps->cFlags,ps->lSid,ps->lValue,ps->caAtt);
        ps = ps->next;
    }
    return 0;
}

int nwUtlPrintSoftInfo(nwDevHead *psDevHead)
{
    nwSoftInfo *ps;
    int iNum=0;
    printf(" Soft Info...\n");
    ps = psDevHead->firstSoft;
    while(ps) {
        iNum++;
        printf("[%3d][%-16s] [%c] [%c] [%8lu] [%9lu] [%s] [%s]\n",iNum,
            ps->caName,ps->cType,ps->cFlags,ps->lSid,ps->lSize,ps->caDate,ps->caVersion);
        ps = ps->next;
    }
    return 0;
}


            
/* 将硬件信息装载到内存中  */
int nwUtlLoadHardInfo(nwCompIp *psCompIp,nwSwitchHead *psFree)
{
    
    int             iReturn;
    pasDbCursor *psCur;
    nwDevInfo *psDevInfo;
    unsigned long lSid,lValue,lNum;
    char caDname[16],caDattr[128];
    
    nwUtlFreeDevInfo(&psCompIp->sDevHead,psFree);
    /* 将用户资料导入到内存中  */
    psCur = pasDbOpenSqlF("select sid,dname,num,dattr,dvalue from nwhardware where compid = %lu",psCompIp->compid);
    if(psCur == NULL) {
        ncSysLog(NC_LOG_ERROR,"Cursor open error %s at nwUtlLoadHardInfo",pasDbErrorMsg(NULL));
        return (-1);
    }
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                    UT_TYPE_STRING,15,caDname,
                                    UT_TYPE_LONG, 4, &lNum,
                                    UT_TYPE_STRING,127,caDattr,
                                    UT_TYPE_ULONG, 4,&lValue);
    while(iReturn == 0) {
#ifdef LDEBUG
        printf(" lSid=%lu Name:%s  Attr:[%s] Value=%lu\n",
                    lSid,caDname,caDattr,lValue);
#endif        
        psDevInfo = (nwDevInfo *)nwUtlAddDevInfo0(&psCompIp->sDevHead,psFree,caDname,lNum);
        if(psDevInfo) {
            psDevInfo->cFlags = 'D';
            strcpy(psDevInfo->caAtt,caDattr);
            psDevInfo->lValue = lValue;
            psDevInfo->lSid = lSid;
        }
        else {
            ncSysLog(NC_LOG_ERROR,"no Switch Buf ");
            pasDbCloseCursor(psCur);
            nwUtlFreeDevInfo(&psCompIp->sDevHead,psFree);
            pasDbCloseCursor(psCur);
            return (-1);
        }
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                    UT_TYPE_STRING,15,caDname,
                                    UT_TYPE_LONG, 4, &lNum,
                                    UT_TYPE_STRING,127,caDattr,
                                    UT_TYPE_ULONG, 4,&lValue);
    }        
    pasDbCloseCursor(psCur);
    
    return 0;
}

/* 释放全部硬件信息,不更新数据库 */
int nwUtlFreeDevInfo(nwDevHead *psHead,nwSwitchHead *psFree)
{
    nwDevInfo *psDev,*psDev0;
    psDev = psHead->firstDev;
    while(psDev) {
        psDev0 = psDev;
        psDev = psDev->next;
        nwUtlFreeBuf(psFree,(nwSwitchBuf  *)psDev0);
    }
    psHead->firstDev = NULL;
    psHead->lDevTime = 0;
    return 0;
}

int nwUtlFreeSoftInfo(nwDevHead *psHead,nwSwitchHead *psFree)
{
    nwSoftInfo *psDev,*psDev0;
    psDev = psHead->firstSoft;
    while(psDev) {
        psDev0 = psDev;
        psDev = psDev->next;
        nwUtlFreeBuf(psFree,(nwSwitchBuf  *)psDev0);
    }
    psHead->firstSoft = NULL;
    psHead->lSoftTime = 0;
    return 0;
}

int nwUtlFreeProcInfo(nwDevHead *psHead,nwSwitchHead *psFree)
{
    nwProcInfo *psDev,*psDev0;
    psDev = psHead->firstProc;
    while(psDev) {
        psDev0 = psDev;
        psDev = psDev->next;
        nwUtlFreeBuf(psFree,(nwSwitchBuf  *)psDev0);
    }
    psHead->firstProc = NULL;
    psHead->lProcTime = 0;
    return 0;
}

/* 释放全部硬件信息，并将相关内容更新到数据库中  */
int nwUtlSynDevInfo(nwDevHead *psHead,nwSwitchHead *psFree,unsigned long lCompId)
{
    nwDevInfo *psDev,*psDev0;
    int iReturn;
    char caAtt[128];
    psDev = psHead->firstDev;
    unsigned long lSid;
    while(psDev) {
        strcpy(caAtt,pasDbReplaceQuote(psDev->caAtt));
        switch (psDev->cFlags) {
            case  'D':      /* 删除  */
                if(psDev->lSid > 0) {
                     iReturn = pasDbExecSqlF("delete from nwhardware where sid = %lu",psDev->lSid);
                     if(iReturn != 0) {
                         ncSysLog(NC_LOG_ERROR,"Delete nwhardware Error %s",pasDbErrorMsg(NULL));
                     }
                     else {
                        unsigned long lSid=pasGetSid("nwhardlog","sid");
                         iReturn = pasDbExecSqlF("insert into nwhardlog (sid,compid,mtime,mflags,dname,dvalue) values \
                             (%lu,%lu,%lu,%d,'%s','%s')",
                             lSid,lCompId,time(0),psDev->cFlags,psDev->caName,caAtt);
                     }
                }
                break;
            case  'M':      /* 修改  */
                if(psDev->lSid > 0) {
                     iReturn = pasDbExecSqlF("update nwhardware set dname='%s',num=%lu,dattr='%s',dvalue=%lu,lasttime=%lu where sid = %lu",
                                   psDev->caName,psDev->nNum,caAtt,psDev->lValue,time(0),psDev->lSid);
                     if(iReturn != 0) {
#ifdef LDEBUG
                         printf("UpdateError:[update nwhardware set dname='%s',num=%lu,dattr='%s',dvalue=%lu,lasttime=%lu where sid = %lu]\n",
                                   psDev->caName,psDev->nNum,caAtt,psDev->lValue,time(0),psDev->lSid);
#endif
                         ncSysLog(NC_LOG_ERROR,"Update nwhardware Error %s",pasDbErrorMsg(NULL));
                     }
                     else {
                        unsigned long lSid=pasGetSid("nwhardlog","sid");
                        
#ifdef LDEBUG
                         printf("Update:[update nwhardware set dname='%s',num=%lu,dattr='%s',dvalue=%lu,lasttime=%lu where sid = %lu]\n",
                                   psDev->caName,psDev->nNum,caAtt,psDev->lValue,time(0),psDev->lSid);
#endif
                         iReturn = pasDbExecSqlF("insert into nwhardlog (sid,compid,mtime,mflags,dname,dvalue) values \
                             (%lu,%lu,%lu,%d,'%s','%s')",
                             lSid,lCompId,time(0),psDev->cFlags,psDev->caName,caAtt);
                     }
                        
                }
                break;
            case  'A':      /* 新增   */
               lSid=pasGetSid("nwhardware","sid");
                
               iReturn = pasDbExecSqlF("insert into nwhardware (sid,compid,dname,num,dattr,dvalue,addtime,lasttime) values (%lu,%lu,'%s',%lu,'%s',%lu,%lu,%lu)",
                                    lSid,lCompId,psDev->caName,psDev->nNum,caAtt,psDev->lValue,time(0),time(0));
               if(iReturn != 0) {
                    ncSysLog(NC_LOG_ERROR,"Insert nwhardware Error %s",pasDbErrorMsg(NULL));
               }
               else {
                   lSid=pasGetSid("nwhardlog","sid");
                   iReturn = pasDbExecSqlF("insert into nwhardlog (sid,compid,mtime,mflags,dname,dvalue) values \
                             (%lu,%lu,%lu,%d,'%s','%s')",
                             lSid,lCompId,time(0),psDev->cFlags,psDev->caName,caAtt);
               }                
               break;
        }
        psDev0 = psDev->next;
        nwUtlFreeBuf(psFree,(nwSwitchBuf  *)psDev);
        psDev = psDev0;
    }
    psHead->firstDev = NULL;
    psHead->lDevTime = 0;
    pasDbCommit(NULL);
    return 0;
}


int nwUtlSynSoftInfo(nwDevHead *psHead,nwSwitchHead *psFree,unsigned long lCompId)
{
    nwSoftInfo *psSoft,*psSoft0;
    int iReturn;
    char caAtt[128];
    unsigned long lSid;
    psSoft = psHead->firstSoft;
    while(psSoft) {
        strcpy(caAtt,pasDbReplaceQuote(psSoft->caName));
        switch (psSoft->cFlags) {
            case  'D':      /* 删除  */
                if(psSoft->lSid > 0) {
                     iReturn = pasDbExecSqlF("delete from nwsoftware where sid = %lu",psSoft->lSid);
                     if(iReturn != 0) {
                         ncSysLog(NC_LOG_ERROR,"Delete nwsoftware Error %s",pasDbErrorMsg(NULL));
                     }
                     else {
                        unsigned long lSid=pasGetSid("nwsoftlog","sid");
                         iReturn = pasDbExecSqlF("insert into nwsoftlog (sid,compid,mtime,mflags,sname) values \
                             (%lu,%lu,%lu,%d,'%s')",
                             lSid,lCompId,time(0),psSoft->cFlags,caAtt);
                     }
                }
                break;
            case  'M':      /* 修改  */
                if(psSoft->lSid > 0) {
                     iReturn = pasDbExecSqlF("update nwsoftware set sname='%s',stype=%d,ssize=%lu,version='%s',instime='%s',lasttime=%lu where sid = %lu",
                                   caAtt,psSoft->cType,psSoft->lSize,psSoft->caVersion,psSoft->caDate,time(0),psSoft->lSid);
                     if(iReturn != 0) {
#ifdef LDEBUG
                         printf("[UpdateError]update nwsoftware set sname='%s',stype=%d,ssize=%lu,version='%s',instime='%s',lasttime=%lu where sid = %lu\n",
                                   caAtt,psSoft->cType,psSoft->lSize,psSoft->caVersion,psSoft->caDate,time(0),psSoft->lSid);
#endif
                         ncSysLog(NC_LOG_ERROR,"Update nwhardware Error %s",pasDbErrorMsg(NULL));
                     }
                     else {
#ifdef LDEBUG
                         printf("Update]update nwsoftware set sname='%s',stype=%d,ssize=%lu,version='%s',instime='%s',lasttime=%lu where sid = %lu\n",
                                   caAtt,psSoft->cType,psSoft->lSize,psSoft->caVersion,psSoft->caDate,time(0),psSoft->lSid);
#endif
                         iReturn = pasDbExecSqlF("insert into nwsoftlog (sid,compid,mtime,mflags,sname) values \
                             (0,%lu,%lu,%d,'%s')",
                             lCompId,time(0),psSoft->cFlags,caAtt);
                     }
                        
                }
                break;
            case  'A':      /* 新增   */
               lSid=pasGetSid("nwsoftware","sid");
                
               iReturn = pasDbExecSqlF("insert into nwsoftware (sid,compid,sname,stype,ssize,version,instime,addtime,lasttime) \
                                  values (%lu,%lu,'%s',%d,%lu,'%s','%s',%lu,%lu)",
                                    lSid,lCompId,psSoft->caName,psSoft->cType,psSoft->lSize,psSoft->caVersion,psSoft->caDate,time(0),time(0));
               if(iReturn != 0) {
                    ncSysLog(NC_LOG_ERROR,"Insert nwsoftware Error %s",pasDbErrorMsg(NULL));
               }
               else {
                    unsigned long lSid=pasGetSid("nwsoftlog","sid");
                    iReturn = pasDbExecSqlF("insert into nwsoftlog (sid,compid,mtime,mflags,sname) values \
                             (%lu,%lu,%lu,%d,'%s')",
                             lSid,lCompId,time(0),psSoft->cFlags,caAtt);
               }                
               break;
        }
        psSoft0 = psSoft->next;
        nwUtlFreeBuf(psFree,(nwSwitchBuf  *)psSoft);
        psSoft = psSoft0;
    }
    psHead->firstSoft = NULL;
    psHead->lSoftTime = 0;
    pasDbCommit(NULL);
    
    return 0;
}

/* 增加一个硬件信息  
                    pName和iNum唯一决定，若相等则替换
 */
nwDevInfo *nwUtlAddDevInfo(nwDevHead *psHead,nwSwitchHead *psFree,char *pName,char *pAttr,int lNum)
{
    nwDevInfo *psDevInfo;
    psDevInfo = psHead->firstDev;
    psHead->lDevTime = time(0);
    while(psDevInfo) {
        if(strcmp(psDevInfo->caName,pName) == 0 && strcmp(psDevInfo->caAtt,pAttr) == 0&&psDevInfo->nNum==lNum) {
            return psDevInfo;
        }
        else {
            psDevInfo = psDevInfo->next;
        }
    }
    psDevInfo = (nwDevInfo *)nwUtlGetFreeBuf(psFree);
    if(psDevInfo == NULL) {
        ncSysLog(NC_LOG_ERROR,"No Switch Buf...");
        return NULL;
    }
    else {
        strcpy(psDevInfo->caName,pName);
        psDevInfo->nNum = 0;
        psDevInfo->lSid = 0;
        psDevInfo->cFlags = 0;
        psDevInfo->next = psHead->firstDev;
        psHead->firstDev = psDevInfo;
        return psDevInfo;
    }
}

/* 开始使用，不判断重复  */
nwDevInfo *nwUtlAddDevInfo0(nwDevHead *psHead,nwSwitchHead *psFree,char *pName,int iNum)
{
    nwDevInfo *psDevInfo;
    psDevInfo = (nwDevInfo *)nwUtlGetFreeBuf(psFree);
    if(psDevInfo == NULL) {
        ncSysLog(NC_LOG_ERROR,"No Switch Buf...");
        return NULL;
    }
    else {
        strcpy(psDevInfo->caName,pName);
        psDevInfo->nNum = iNum;
        psDevInfo->lSid = 0;
        psDevInfo->cFlags = 'D';
        psDevInfo->next = psHead->firstDev;
        psHead->firstDev = psDevInfo;
        return psDevInfo;
    }
}

nwSoftInfo *nwUtlAddSoftInfo(nwDevHead *psHead,nwSwitchHead *psFree,char *pName)
{
    nwSoftInfo *psSoftInfo;
    psHead->lSoftTime = time(0);
    psSoftInfo = psHead->firstSoft;
    while(psSoftInfo) {
        if(strcmp(psSoftInfo->caName,pName) == 0) {
            return psSoftInfo;
        }
        else {
            psSoftInfo = psSoftInfo->next;
        }
    }
    psSoftInfo = (nwSoftInfo *)nwUtlGetFreeBuf(psFree);
    if(psSoftInfo == NULL) {
        ncSysLog(NC_LOG_ERROR,"No Switch Buf...");
        return NULL;
    }
    else {
        psSoftInfo->lSid = 0;
        psSoftInfo->cFlags = 0;
        strcpy(psSoftInfo->caName,pName);
        psSoftInfo->next = psHead->firstSoft;
        psHead->firstSoft = psSoftInfo;
        return psSoftInfo;
    }
}

nwSoftInfo *nwUtlAddSoftInfo0(nwDevHead *psHead,nwSwitchHead *psFree,char *pName)
{
    nwSoftInfo *psSoftInfo;
    psSoftInfo = psHead->firstSoft;
    psSoftInfo = (nwSoftInfo *)nwUtlGetFreeBuf(psFree);
    if(psSoftInfo == NULL) {
        ncSysLog(NC_LOG_ERROR,"No Switch Buf...");
        return NULL;
    }
    else {
        psSoftInfo->lSid = 0;
        psSoftInfo->cFlags = 'D';
        strcpy(psSoftInfo->caName,pName);
        psSoftInfo->next = psHead->firstSoft;
        psHead->firstSoft = psSoftInfo;
        return psSoftInfo;
    }
}


nwProcInfo *nwUtlAddProcInfo(nwDevHead *psHead,nwSwitchHead *psFree,char *pName)
{
    nwProcInfo *psProcInfo;
    psHead->lProcTime = time(0);
    psProcInfo = psHead->firstProc;
    while(psProcInfo) {
        if(strcasecmp(psProcInfo->caName,pName)==0) {
            return psProcInfo;
        }
        else {
            psProcInfo = psProcInfo->next;
        }
    }
    psProcInfo = (nwProcInfo *)nwUtlGetFreeBuf(psFree);
    if(psProcInfo == NULL) {
        ncSysLog(NC_LOG_ERROR,"No Switch Buf...");
        return NULL;
    }
    else {
        strcpy(psProcInfo->caName ,pName);
        psProcInfo->cFlags = 0;
        psProcInfo->next = psHead->firstProc;
        psHead->firstProc = psProcInfo;
        return psProcInfo;
    }
}


/* 取一条空闲的数据结构  */
char  *nwUtlGetFreeBuf(nwSwitchHead *psFree)
{
    nwSwitchBuf  *ps;
    if(psFree->first == NULL) {
        return NULL;
    }
    else {
        ps = psFree->first;
        psFree->first = psFree->first->next;
        return (char *)ps;
    }
}

/* 释放一条记录   */
int nwUtlFreeBuf(nwSwitchHead *psFree,nwSwitchBuf  *ps)
{
    ps->next = psFree->first;
    psFree->first = ps;
    return 0;
}

int nwUtlLoadSoftInfo(nwCompIp *psCompIp,nwSwitchHead *psFree)
{
      
    int             iReturn;
    pasDbCursor *psCur;
    nwSoftInfo  *psSoftInfo;
    unsigned long lSid,lType,lSize;
    char caSname[72],caVersion[16],caInstdate[16];
    nwUtlFreeSoftInfo(&psCompIp->sDevHead,psFree);
    /* 将用户资料导入到内存中  */
    psCur = pasDbOpenSqlF("select sid,sname,version,stype,instime,ssize  from nwsoftware where compid = %lu",psCompIp->compid);
    if(psCur == NULL) {
        ncSysLog(NC_LOG_ERROR,"Cursor open error %s at nwUtlLoadSoftInfo",pasDbErrorMsg(NULL));
        return (-1);
    }
    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                    UT_TYPE_STRING,71,caSname,
                                    UT_TYPE_STRING,15, caVersion,
                                    UT_TYPE_LONG,  4,  &lType,
                                    UT_TYPE_STRING,15, caInstdate,
                                    UT_TYPE_ULONG,4,&lSize);
    while(iReturn == 0) {
#ifdef LDEBUG
        printf(" lSid=%lu Name:%s  Inst:[%s] Type=%lu lSize=%lu\n",
                    lSid,caSname,caInstdate,lType,lSize);
#endif        
        psSoftInfo = (nwSoftInfo *)nwUtlAddSoftInfo0(&psCompIp->sDevHead,psFree,caSname);
        if(psSoftInfo) {
            psSoftInfo->cFlags = 'D';
            strcpy(psSoftInfo->caVersion,caVersion);
            strcpy(psSoftInfo->caDate,caInstdate);
            psSoftInfo->cType = (char)lType;
            psSoftInfo->lSid = lSid;
            psSoftInfo->lSize = lSize;
        }
        else {
            ncSysLog(NC_LOG_ERROR,"no Switch Buf ");
            pasDbCloseCursor(psCur);
            nwUtlFreeSoftInfo(&psCompIp->sDevHead,psFree);
            pasDbCloseCursor(psCur);
            return (-1);
        }
        iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lSid,
                                    UT_TYPE_STRING,71,caSname,
                                    UT_TYPE_STRING,15, caVersion,
                                    UT_TYPE_LONG,  4,  &lType,
                                    UT_TYPE_STRING,15, caInstdate,
                                    UT_TYPE_ULONG,4,&lSize);
    }        
    pasDbCloseCursor(psCur);
    return 0;
} 


int nwUtlCvtSoftInfo(char *pIn,char *caSname,char *caType,char *caInstdate,char *caVersion,unsigned *lSize)
{
    
    char *p;
    char caMark[2],caSize[16];
    *lSize = 0;
    p = pIn;
    strcpy(caMark,"\0");
    strcpy(caSname,"\0");
    strcpy(caInstdate,"\0");
    strcpy(caVersion,"\0");
    strcpy(caSize,"\0");
    strcpy(caType,"\0");
    p = utStrGetWord(p,caMark,1,";");    
    if(*p != ';') {
        return (-1);
    }
    else {
        p++;
    }
    p = utStrGetWord(p,caType,15,";");    
    if(*p != ';') {
        return (-1);
    }
    else {
        p++;
    }

    p = utStrGetWord(p,caSname,71,";");    
    if(*p != ';') {
        return (-1);
    }
    else {
        p++;
    }
    
    p = utStrGetWord(p,caInstdate,15,";");    
    if(*p == ';') {
        p++;
        p = utStrGetWord(p,caVersion,15,";");    
        if(*p == ';') {
            p++;
            p = utStrGetWord(p,caSize,15,";\r\n");    
            if(utStrIsSpaces(caSize)) {
                *lSize = 0;
            }
            else {
                *lSize = atol(caSize);
            }
        }
    }
    return caMark[0];
}
