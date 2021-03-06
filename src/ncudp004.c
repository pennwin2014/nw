#define  PAS_SRCFILE      4111 
#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include <stdlib.h>
#include "utoall.h"
#include "pasdb.h"
#include "pasutl.h"
#include "ncdef.h"
#include "nwdef.h"

/* 接收日志信息  */
int nwUdpLogInfo(utShmHead *psShmHead,
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead)
{

    int iReturn,i;
    unsigned long lSum,lSid,lIp,lCompId;
    char *p,*p1,caSid[16];
    char caVar[32],caType[10];
    nwCompIp        *psCompIp;
    char caIp[16];
    char caStime[32],caEtime[32];
    char caMod[12],caName[72];
    unsigned long lPid,lCpu,lCpurate,lMem,lBtime;
    unsigned long lSip,lDip,lPort,lProt,lBytes,lVister,lStime;
    
#ifdef LDEBUG
    printf(" It Is LogInfo ....\n");
    utMsgPrintMsg(psMsgHead);
#endif
    /* 数据报校验  */
    lCompId = psMsgHead->lReserve[0];
    lIp = ntohl(psSockAddr->sSin.sin_addr.s_addr);
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&lIp);
    if(psCompIp == NULL) {
        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                             "status",  UT_TYPE_ULONG,NW_ERR_RELOGIN);
        return 0;
    }
    if(psCompIp->compid != lCompId) {
        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                             "status",  UT_TYPE_ULONG,NW_ERR_RELOGIN);
        return 0;
    }
    if(!utMsgCheckSum(psMsgHead,psCompIp->caKey)) { /* 数据不合法  */
#ifdef LDEBUG
            unsigned long lCode;
            printf(" It is invalid message, CheckSum Error\n");
            lCode = utMd5Code( (char *)psMsgHead + sizeof(utMsgHead),
                        psMsgHead->lBytes - sizeof(utMsgHead),psCompIp->caKey);
            printf(" CheckSum==%lu   %lu    %lu \n",
                      lCode,ntohl(lCode),psMsgHead->lCheckSum);                        
#endif        
        return 0;
    }
    iReturn = utMsgGetSomeNVar(psMsgHead,1,
		"sum",    UT_TYPE_LONG, 4, &lSum);
#ifdef LDEBUG
    printf(" Sum.... %d \n",lSum);
#endif
    lSid = 0;
    for(i=0;i<lSum;i++) {
        sprintf(caVar,"log%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            p1 = utStrGetWord(p,caSid,16,";");
            if(*p1 == ';') {
                lSid = atol(caSid);
                p1 = utStrGetWord(p1+1,caType,1,";");
                if(*p1 == ';') {
                    if(caType[0] == 'n') { /* 网络信息  */
                        iReturn = utStrGetSomeWordBy(p1+1,",\n\r",6,
                                UT_TYPE_ULONG,4,&lStime,
                                UT_TYPE_ULONG,8,&lProt,
                                UT_TYPE_STRING,16,caIp,
                                UT_TYPE_ULONG,8,&lPort,
                                UT_TYPE_ULONG,4,&lBytes,
                                UT_TYPE_LONG,4,&lVister);
#ifdef LDEBUG
                       printf("%d Netid:%lu [%s]\n",iReturn,lSid,p1+1);
#endif
                        if(iReturn == 6) {
                            unsigned long lSid=pasGetSid("nwnetlog","sid");
                            if(lVister == 0) {
                                lSip = lIp;
                                lDip = ntohl(pasIpcvtLong(caIp));
                            }
                            else {
                                lDip = lIp;
                                lSip = ntohl(pasIpcvtLong(caIp));
                            }
                            iReturn = pasDbExecSqlF("insert into nwnetlog(sid,compid,stime,sip,dip,dport,bytes,prot,flags,vister) values (%lu,%lu,%lu,%lu,%lu,%lu,%lu,%d,0,%d)",
                                                               lSid,lCompId,lStime,lSip,lDip,lPort,lBytes,lProt,lVister);
                            if(iReturn != 0) {
                                ncSysLog(NC_LOG_ERROR,"Insert Error %s ",pasDbErrorMsg(NULL));
                            }
                        }
                    }
                    else if(caType[0] == 'f') { /* 文件访问  */
#ifdef LDEBUG
                       printf("Fileid:%lu [%s]\n",lSid,p1+1);
#endif
                    }
                    else if(caType[0] == 'p') { /* 进程信息  */
#ifdef LDEBUG
                        printf("Procid:%lu [%s]\n",lSid,p1+1);
#endif
                        iReturn = utStrGetSomeWordBy(p1+1,";\n\r",8,
                                UT_TYPE_STRING,5,caMod,
                                UT_TYPE_LONG,4,&lPid,
                                UT_TYPE_STRING,71,caName,
                                UT_TYPE_STRING,31,caStime,
                                UT_TYPE_STRING,31,caEtime,
                                UT_TYPE_LONG,4,&lCpu,
                                UT_TYPE_LONG,4,&lCpurate,
                                UT_TYPE_ULONG,4,&lMem);
                       if(iReturn == 8) {
                            unsigned long lSid=pasGetSid("nwproclog","sid");
                            lBtime =  utTimStrToLong("%Y-%m-%d %H:%M:%S",caStime);
                            if(caMod[0] == 'd') {
                                lStime =  utTimStrToLong("%Y-%m-%d %H:%M:%S",caEtime);
                            }
                            else {
                                lStime = lBtime;
                            }
                            iReturn = pasDbExecSqlF("insert into nwproclog(sid,compid,mark,pid,stime,btime,procname,mem,cpu) values (%lu,%lu,'%s',%lu,%lu,%lu,'%s',%lu,%lu)",
                                                        lSid,lCompId,caMod,lPid,lStime,lBtime,caName,lMem,lCpu);
                            if(iReturn != 0) {
                                ncSysLog(NC_LOG_ERROR,"nwproclog insert error %s",pasDbErrorMsg(NULL));
                            }
                       }
                    }
                }
            }
        }
    }           
    pasDbCommit(NULL);   
     
    utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                            "lastnum", UT_TYPE_ULONG,lSid,
                            "status",  UT_TYPE_ULONG,0);
    return 0;
}

/* 取字符串中的数字  */
unsigned long nwUtlGetDigital(char *p)
{
    double d;
    int i;
    char caTemp[32];
    unsigned long l;
    for(i=0;p[i]!=0;i++) {
        if((p[i] >= '0' && p[i]<='9') || p[i] == '.') {
            caTemp[i] = p[i];
        }
        else {
            break;
        }
    }
    caTemp[i] = 0;
    d = atof(caTemp);
    l = d * 1024;
#ifdef LDEBUG
    printf(" %s  d== %10.2f  l==%lu\n",caTemp,d,l);
#endif
    return l;
}


int nwUdpSysInfo(utShmHead *psShmHead,
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead)
{

    int iReturn,i,j;
    unsigned long lSum,lIp,lCompId;
    char caVar[32],*p;
    nwCompIp        *psCompIp;
    nwSwitchHead *psFree;
    char  caOs[16],caHdsum[16],caHdleft[16];
    char  caIe[32],caCdsum[16],caCdleft[16],caMdsum[16],caMdleft[16],caMemsum[16],caMemleft[16];
    char  caDriv[12],caSum[16],caLeft[16];
    long lHd,lCd,lMd,lCpu;
    

#ifdef LDEBUG
    printf(" It Is SysInfo ....\n");
    utMsgPrintMsg(psMsgHead);
#endif
    /* 数据报校验  */
    lCompId = psMsgHead->lReserve[0];
    lIp = ntohl(psSockAddr->sSin.sin_addr.s_addr);
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&lIp);
    if(psCompIp == NULL || psCompIp->psComp == NULL) {
        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                             "status",  UT_TYPE_ULONG,NW_ERR_RELOGIN);
        return 0;
    }
    if(psCompIp->compid != lCompId) {
        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                             "status",  UT_TYPE_ULONG,NW_ERR_RELOGIN);
        return 0;
    }
    if(!utMsgCheckSum(psMsgHead,psCompIp->caKey)) { /* 数据不合法  */
#ifdef LDEBUG
            printf(" It is invalid message, CheckSum Error\n");
#endif        
        return 0;
    }
   
    iReturn = utMsgGetSomeNVar(psMsgHead,14,
		"osinfo",      UT_TYPE_STRING,15,caOs,
		"harddisktot", UT_TYPE_STRING,15,caHdsum,
		"hdrdiskleft", UT_TYPE_STRING,15,caHdleft,
		"IE",          UT_TYPE_STRING,31,caIe,
		"cdromtotal",  UT_TYPE_STRING,15,caCdsum,
		"cdromleft",   UT_TYPE_STRING,15,caCdleft,
		"mdtotal",     UT_TYPE_STRING,15,caMdsum,
		"mdleft",      UT_TYPE_STRING,15,caMdleft,
		"memtot",      UT_TYPE_STRING,15,caMemsum,
		"memleft",      UT_TYPE_STRING,15,caMemleft,
		"lghdcount",      UT_TYPE_LONG,4,&lHd,
		"cdromcount",      UT_TYPE_LONG,4,&lCd,
		"mdcount",      UT_TYPE_LONG,4,&lMd,
		"cpurat",       UT_TYPE_LONG,4,&lCpu);
    utStrDelSpaces(caOs);
    utStrDelSpaces(caIe);
    if(strcmp(psCompIp->psComp->os,caOs) != 0) {
        nwUtlConfigLog(psCompIp->compid,ncLang("0168操作系统"),ncLang("0169操作系统[%s]变为[%s]%d"),psCompIp->psComp->os,caOs);
	    strcpy(psCompIp->psComp->os,caOs);
	    psCompIp->psComp->cmod = 1;
	}
	if(strcmp(psCompIp->psComp->ie,caIe) != 0) {
        nwUtlConfigLog(psCompIp->compid,ncLang("0902浏览器"),ncLang("0903浏览器[%s]变为[%s]"),psCompIp->psComp->ie,caIe);
	    strcpy(psCompIp->psComp->ie,caIe);
	    psCompIp->psComp->cmod = 1;
	}
    if(psCompIp->sDevHead.sysInfo == NULL) {
    	psFree = (nwSwitchHead *)utShmArray(psShmHead,NC_LNK_DEVINFO);
        psCompIp->sDevHead.sysInfo = (nwSysInfo *)nwUtlGetFreeBuf(psFree);
        if(psCompIp->sDevHead.sysInfo == NULL) {
            ncSysLog(NC_LOG_ERROR,"No Switch Buf...");
            return 0;
        }
    }
    psCompIp->sDevHead.sysInfo->lHdsum = nwUtlGetDigital(caHdsum);
    psCompIp->sDevHead.sysInfo->lHdleft = nwUtlGetDigital(caHdleft);
    psCompIp->sDevHead.sysInfo->lCdsum = nwUtlGetDigital(caCdsum);
    psCompIp->sDevHead.sysInfo->lCdleft = nwUtlGetDigital(caCdleft);
    psCompIp->sDevHead.sysInfo->lMdsum = nwUtlGetDigital(caMdsum);
    psCompIp->sDevHead.sysInfo->lMdleft = nwUtlGetDigital(caMdleft);
    psCompIp->sDevHead.sysInfo->lMemsum = nwUtlGetDigital(caMemsum);
    psCompIp->sDevHead.sysInfo->lMemleft = nwUtlGetDigital(caMemleft);
    psCompIp->sDevHead.sysInfo->lCpu = lCpu;
    psCompIp->sDevHead.sysInfo->lCdCount = lCd;
    psCompIp->sDevHead.sysInfo->lMdCount = lMd;
#ifdef LDEBUG
    printf("lHdsum = %lu   lCdsum=%lu lMemsum=%lu \n",
            psCompIp->sDevHead.sysInfo->lHdsum,psCompIp->sDevHead.sysInfo->lCdsum,
            psCompIp->sDevHead.sysInfo->lMemsum);
            
#endif 
    j = 0;
    for(i=0;i<lHd && j < 10;i++) {
        sprintf(caVar,"hddisk%d",i);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
            iReturn = utStrGetSomeWordBy(p,";",3,
                          UT_TYPE_STRING,7,caDriv,
                          UT_TYPE_STRING,15,caSum,
                          UT_TYPE_STRING,15,caLeft);
#ifdef LDEBUG
            printf(" iReturn=%d  p=[%s]  [%s] [%s] [%s] \n",
                    iReturn,p,caDriv,caSum,caLeft);
#endif
            if(iReturn == 3) {
                strncpy(psCompIp->sDevHead.sysInfo->sD[j].caName,caDriv,3);
                psCompIp->sDevHead.sysInfo->sD[j].lSum = nwUtlGetDigital(caSum);
                psCompIp->sDevHead.sysInfo->sD[j].lLeft = nwUtlGetDigital(caLeft);
#ifdef LDEBUG
                printf(" [%s] Sum=%lu Left=%lu \n",
                     psCompIp->sDevHead.sysInfo->sD[j].caName,
                     psCompIp->sDevHead.sysInfo->sD[j].lSum,
                     psCompIp->sDevHead.sysInfo->sD[j].lLeft);
#endif
                j++;
            }
        }
    }
    psCompIp->sDevHead.sysInfo->lDiskSum = j;   
    psCompIp->cSysInfo = 1;
    psCompIp->sDevHead.lSysTime = time(0);
    utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                            "status",  UT_TYPE_ULONG,0);
    return 0;
}


/* 进程信息  
       保存在内存中
 */
int nwUdpProcessInfo(utShmHead *psShmHead,
            utComSockAddr *psSockAddr,utMsgHead *psMsgHead)
{

    int iReturn,i;
    unsigned long lSum,lIp,lCompId;
    char caVar[32];
    nwCompIp        *psCompIp;
    char caStime[32],caEtime[32],caName[72];
    char *p;
    char caBytes[12],caSend[12];
    char caPath[128];
    unsigned long lPid,lMem,lCpu,lCpuRate,lTotal,lStart;
    nwSwitchHead    *psFree;
    nwProcInfo      *psProc;
#ifdef LDEBUG
    printf(" It Is Process ....\n");
    utMsgPrintMsg(psMsgHead);
#endif
    /* 数据报校验  */
    lCompId = psMsgHead->lReserve[0];
    lIp = ntohl(psSockAddr->sSin.sin_addr.s_addr);
    psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&lIp);
    if(psCompIp == NULL) {
        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                             "status",  UT_TYPE_ULONG,NW_ERR_RELOGIN);
        return 0;
    }
    if(psCompIp->compid != lCompId) {
        utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,1,
                             "status",  UT_TYPE_ULONG,NW_ERR_RELOGIN);
        return 0;
    }
#ifdef LDEBUG
    printf(" Check Valid Message \n");
#endif    
    if(!utMsgCheckSum(psMsgHead,psCompIp->caKey)) { /* 数据不合法  */
#ifdef LDEBUG
            unsigned long lCode;
            printf(" It is invalid message, CheckSum Error\n");
            lCode = utMd5Code( (char *)psMsgHead + sizeof(utMsgHead),
                        psMsgHead->lBytes - sizeof(utMsgHead),psCompIp->caKey);
            printf(" CheckSum==%lu   %lu    %lu \n",
                      lCode,ntohl(lCode),psMsgHead->lCheckSum);                        
#endif        
        return 0;
    }
    lSum = 0;
    iReturn = utMsgGetSomeNVar(psMsgHead,3,
		"total",    UT_TYPE_LONG,4,&lTotal,
		"start",    UT_TYPE_LONG,4,&lStart,
		"sum",      UT_TYPE_LONG,4,&lSum);
#ifdef LDEBUG
    printf(" iReturn=%d lTotal=%d lStart=%lu lSum=%lu \n",
           iReturn,lTotal,lStart,lSum);
#endif

    psFree = (nwSwitchHead *)utShmArray(psShmHead,NC_LNK_DEVINFO);
    if(lStart == 1) { /* 开始  */
        nwUtlFreeProcInfo(&psCompIp->sDevHead,psFree);
    }

    for(i=0;i<lSum;i++) {
        sprintf(caVar,"proc%d",i+1);
        p = utMsgGetVar_s(psMsgHead,caVar);
        if(p) {
        	    memset(caPath,0,sizeof(caPath));
            iReturn = utStrGetSomeWordBy(p+2,";\n\r",4,              
                   UT_TYPE_STRING,71,caName,
                   UT_TYPE_LONG,4,&lCpu,
                   UT_TYPE_LONG,4,&lCpuRate,
                   UT_TYPE_STRING,78,caPath);
                 
            if(iReturn == 4) {
                psProc = nwUtlAddProcInfo(&psCompIp->sDevHead,psFree,caName);
                if(psProc) {
                                     
                    psProc->cputime = lCpu;
                    psProc->cpurate = lCpuRate;
                    strcpy(psProc->cRev,caPath);
                    
#ifdef LDEBUG                    
                printf(" iReturn=%d  Pid=%lu  Name:[%s] Time:[%s] Cpu:[%lu] CpuRate:[%lu] Mem:[%lu]\n",iReturn,
                      lPid,caName,caStime,lCpu,lCpuRate,lMem);
#endif
                }
            }
        }
    }
   
    if(lStart + lSum > lTotal) {
        psCompIp->cProcInfo = 1;
    }
    utComUdpResponse1(psShmHead,psSockAddr,psMsgHead,0,2,
                            "status",  UT_TYPE_ULONG,0,
                            "start",   UT_TYPE_ULONG,lStart+lSum);
    return 0;
}

/* 
   检查用户信息，并进行相关处理
*/

int nwUtlCheckCompute(utShmHead *psShmHead)
{
    char *pHash;
    nwCompIp   *psCompIp,*psCompIp0;
    ncUserInfo *psUserInfo;
    pasHashInfo sHashInfo;
    char caTemp[1024];
    char caDeskVersion[16];
    char caMesg[256];
    char caSoftName[128];
    unsigned long lTime;
    int iReturn;
    char caServerIp[20],caServerPort[16];
    nwSwitchHead *psFree;
    int  iSock;
    unsigned long lIp;
    long nPort,tPort;
    utMsgHead *psMsgHead2;
   struct lancomp_s{
    	char compname[32];
    	char mac[6];
    	unsigned long ip;
    	char devname[32];
    };
    struct lancomp_s psCompInfo[50];
    long iNum=0;
    memset(&psCompInfo,0,50*sizeof(struct lancomp_s));
    strcpy(caServerIp,utComGetVar_sd(psShmHead,"ncip","127.0.0.1"));
		strcpy(caServerPort,utComGetVar_sd(psShmHead,"ncport","9185"));
    tPort=atol(caServerPort);
    lIp = utComHostAddress(caServerIp);
    nPort=ntohs(tPort);
//    iReturn = pasConnect(psShmHead);
 //   if(iReturn != 0) {
 //       ncSysLog(NC_LOG_DEBUG," DataBase Connect Error \n");
 //       sleep(30);
 //       return 0;
 //   }
    while(1) {
        pHash = utShmHashHead(psShmHead,NC_LNK_ONLINECOMP);
        while(pHash == NULL) {
            printf(" Compute Online Not ready !\n");
            sleep(10);
            pHash = utShmHashHead(psShmHead,NC_LNK_ONLINECOMP);
        }
#ifdef LDEBUG
        printf(" Check Online Compute ... \n");
#endif
        psCompIp0 = (nwCompIp *)pasHashFirst(pHash,&sHashInfo);
        lTime = time(0);
//        psFree = (nwSwitchHead *)utShmArray(psShmHead,NC_LNK_DEVINFO);
        while(psCompIp0) {
            psCompIp = psCompIp0;
            psCompIp0 = (nwCompIp *)pasHashNextS(&sHashInfo);
          
            if(psCompIp->lLastTime < lTime - 180) { /* 客户已离线  */
#ifdef LDEBUG
                printf(" psCompip:%lu [%s] offline \n",psCompIp->compid,
                      psCompIp->psComp->devname?psCompIp->psComp->devname:"\0");
#endif
						

   //             nwUtlFreeDevHead(&psCompIp->sDevHead,psFree);
                /* 离线  */
                psCompIp->psComp->status = 0;
                utShmHashDel(psShmHead,NC_LNK_ONLINECOMP,psCompIp);
            }
            else{
//发送同步信息，将在线客户端信息发送到网络督察
            	if(strcasecmp(utComGetVar_sd(psShmHead,"StartSynComp","No"),"Yes")==0){
            		   strcpy(psCompInfo[iNum].compname,psCompIp->psComp->compname);
            		   strcpy(psCompInfo[iNum].devname,psCompIp->psComp->devname);
            		   psCompInfo[iNum].ip=psCompIp->ip;
            		   memcpy(psCompInfo[iNum].mac,psCompIp->psComp->mac,6);
            		    sprintf(caTemp,"compname=%s,ip=%s",psCompInfo[iNum].compname,utComHostIp(htonl(psCompInfo[iNum].ip)));
            		   	 pasLogs(PAS_SRCFILE,1000,caTemp); 
            		   iNum++;
            		   if(iNum>10){
  
            		  	 psMsgHead2 = pasTcpRequest(lIp,nPort,
												                         0,        /* CheckSum */
												                         0,        /* Encrypt  */
												                         "nwSysCompuInfo",
												                         0,       /* Sid */
												                         0,       /* Rid  */
												                         30,      /* 超时  */
												                         PAS_DEF_KEY,    /* key   */
												                         2,
												                         "sum",UT_TYPE_LONG,iNum,
												                          "data",UT_TYPE_STRUCT,&psCompInfo,iNum*sizeof(struct lancomp_s));
												                         
										 iNum=0;
										 	  if(psMsgHead2) {
	                 //  	   printf("ok\n");
	                   	  utMsgFree(psMsgHead2);
	                    }
										 
										 
										 }
												                        
												                         
											                      

            	}
            }
 //           nwUtlCheckDevHeadTimeOut(&psCompIp->sDevHead,psFree,60);
        }
        
//   printf("iNum=%d\n",iNum);     
        if(strcasecmp(utComGetVar_sd(psShmHead,"StartSynComp","No"),"Yes")==0){
        	if(iNum>0){
        		
        		            	if(strcasecmp(utComGetVar_sd(psShmHead,"StartSynComp","No"),"Yes")==0){
					            		   if(iNum>0){
					            		  	 psMsgHead2 = pasTcpRequest(lIp,nPort,
																	                         0,        /* CheckSum */
																	                         0,        /* Encrypt  */
																	                         "nwSysCompuInfo",
																	                         0,       /* Sid */
																	                         0,       /* Rid  */
																	                         30,      /* 超时  */
																	                         PAS_DEF_KEY,    /* key   */
																	                         2,
																	                         "sum",UT_TYPE_LONG,iNum,
																	                          "data",UT_TYPE_STRUCT,&psCompInfo,iNum*sizeof(struct lancomp_s));
																	                         
															 iNum=0;
															
																	                        
																	                         
																                      
						                   if(psMsgHead2) {
						                   	printf("ok11\n");
						                   	utMsgFree(psMsgHead2);
						                   }
						                  }
						                 }
        		
        	}
        }
        
        
        
        sleep(10);
    }
    return 0;
}

//load文件到数据库
int nwLoadFileToDb(utShmHead *psShmHead)
{
   pasUtlLoad2Db(psShmHead);
    return 0;
}