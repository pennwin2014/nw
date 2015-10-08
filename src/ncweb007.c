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

/* 显示在线计算机信息  */
int ncWebShowOnlineCompute(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    nwCompInfo     *psComp;    /* 计算机信息  */
    nwCompIp       *psCompIp;
    ncUserInfo     *psUserInfo;
    pasHashInfo sHashInfo;
    char *pHash;
    int iNum,iReturn,i,k;
    unsigned long lGroupid0,lGroupid;
    utPltDbHead *psDbHead;
    char caCond[32],caSear[16],caGroupid[16],caCurpg[16];
    char caPlate[64];
    char mac[6];
    long lRowNum,lCurpg,lStartRec,lTotpg;
    unsigned long lIp;

    lRowNum = 33;
    if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * lRowNum;
   
    psDbHead = utPltInitDb();
    pHash = (char *)utShmHashHead(psShmHead,NC_LNK_ONLINECOMP);
    if(pHash == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0209调试软件"),ncLang("0833在线信息不存在"));
        return 0;
    }
    
    psCompIp  = (nwCompIp *)pasHashFirst(pHash,&sHashInfo);
    i = 0;
    iNum=0;
    while(psCompIp && i < 1000) {
        if(psCompIp->psComp) {
                iNum++;
                utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
                utPltPutLoopVarF(psDbHead, "compid",iNum,"%lu",psCompIp->compid);
                utPltPutLoopVar(psDbHead, "ip",    iNum,utComHostIp(htonl(psCompIp->ip)));
                utPltPutLoopVar(psDbHead, "mac",   iNum,ncCvtMac(psCompIp->psComp->mac));
                utPltPutLoopVar(psDbHead, "compname", iNum,psCompIp->psComp->compname);
                utPltPutLoopVar(psDbHead, "devname", iNum,psCompIp->psComp->devname);
                utPltPutLoopVar(psDbHead,"os",iNum,psCompIp->psComp->os);
                utPltPutLoopVarF(psDbHead,"htime",iNum,"%lu",psCompIp->psComp->htime);
                utPltPutLoopVarF(psDbHead,"stime",iNum,"%lu",psCompIp->psComp->stime);
                utPltPutLoopVarF(psDbHead,"ctime",iNum,"%lu",psCompIp->psComp->ctime);
                utPltPutLoopVarF(psDbHead, "groupid", iNum,"%lu",psCompIp->psComp->groupid);
        }
        psCompIp  = (nwCompIp *)pasHashNext(&sHashInfo);
    }
   

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nw/nc_allonlinecomp.htm");
   
    return 0;
} 