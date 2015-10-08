#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "ncpkg.h"
#include "pasdb.h"

/* 在线用户计费信息 */
int ncWebTestShowUserFee(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasHashInfo sHashInfo;
    ncUserInfo  *psUserInfo;
   	utPltDbHead *psDbHead;
	char caStart[12],caSum[12],*pHash;
	int iStart,iSum,iNum,iReturn,i;
    ncUserInfo *psUser;

	utMsgPrintMsg(psMsgHead);
	iReturn = utMsgGetSomeNVar(psMsgHead,2,
	                "start",    UT_TYPE_STRING,8, caStart,
	                "sum",      UT_TYPE_STRING,8, caSum);
    if(utStrIsSpaces(caStart)) {
        iStart = 1;
    }
    else {
        iStart = atol(caStart);
    }
    if(utStrIsSpaces(caSum)) {
        iSum = 999999L;
    }
    else {
        iSum = atol(caSum);
    }
    i = 0;
   	psDbHead = utPltInitDb();
    pHash = (char *)utShmHashHead(psShmHead,NC_LNK_USERINFO);
    psUser  = (ncUserInfo *)pasHashFirst(pHash,&sHashInfo);
    while(psUser && i < iStart - 1) {
        i ++;
        psUser = (ncUserInfo *)pasHashNextS(&sHashInfo);
    }
    iNum = 0;
    while(psUser) {
        iNum ++;
        utPltPutLoopVarF(psDbHead,"num",  iNum,"%d",iNum + i);
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",psUser->lId);
        if(psUser->psUser) {
            utPltPutLoopVar(psDbHead,"username",iNum,psUser->psUser->username);
            utPltPutLoopVar(psDbHead,"dispname",iNum,psUser->psUser->dispname);
            if(psUser->psUser->psGroup) {
                utPltPutLoopVar(psDbHead,"groupname",iNum,psUser->psUser->psGroup->groupname);
            }
            utPltPutLoopVar(psDbHead,"ip",iNum,utComHostIp(htonl(psUser->psUser->thisip)));
            utPltPutLoopVar(psDbHead,"starttime",iNum,utTimFormat("%y-%m-%d %H:%M:%S",psUser->lStartTime));
            utPltPutLoopVarF(psDbHead,"sumbytes",iNum,"%llu",psUser->lBytes[0] + psUser->lBytes[1]);
            utPltPutLoopVarF(psDbHead,"conntime",iNum,"%lu",psUser->lConnTime);
            utPltPutLoopVarF(psDbHead,"fee",iNum,"%12.3f",psUser->psUser->fee);
            utPltPutLoopVarF(psDbHead,"thisfee",iNum,"%12.3f",psUser->psUser->thisfee);
            utPltPutLoopVarF(psDbHead,"thisflow",iNum,"%llu",psUser->psUser->thisflow);
            utPltPutLoopVarF(psDbHead,"thistime",iNum,"%lu",psUser->psUser->thistime);
            utPltPutLoopVarF(psDbHead,"feeid",iNum,"%lu",psUser->psUser->sSumm.feeid);
            if(psUser->psUser->sSumm.psBill) {
                utPltPutLoopVar(psDbHead,"feename",iNum,psUser->psUser->sSumm.psBill->feename);
                utPltPutLoopVarF(psDbHead,"feetype",iNum,"%d",psUser->psUser->sSumm.psBill->feetype);
                utPltPutLoopVarF(psDbHead,"feeway",iNum,"%d",psUser->psUser->sSumm.psBill->feeway);
            }
        }
        psUser = (ncUserInfo *)pasHashNextS(&sHashInfo);
    }
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"test/nc_test_userfee.htm");
    return 0;
}
