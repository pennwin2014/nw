#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "ncuser.h"
/* 显示内存中的用户信息  */
int ncWebTestShowAllUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    pasHashInfo sHashInfo;    
    int iReturn;
    char caStart[12],caSum[12];
    char caDisp[12];
    unsigned long lSum,lStart;
    ncUserContHead  *psHead;
    char *pHash;
    ncUserIpIndex   *psIpIndex;
    ncUserMacIndex  *psMacIndex;
    ncUserIdIndex   *psIdIndex;
    ncUserCont      *psUser,*psUser0;
    int i,iNum;

   long lGroupid0=0;
       //增加网络权限判断
    dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,3,
	                "start",    UT_TYPE_STRING,8, caStart,
	                "rownum",      UT_TYPE_STRING,8, caSum,
	                "Disp",     UT_TYPE_STRING,8, caDisp);
    if(utStrIsSpaces(caStart)) {
        lStart = 1;
    }
    else {
        lStart = atol(caStart);
    }
    if(utStrIsSpaces(caSum)) {
        lSum = 200;
    }
    else {
        lSum = atol(caSum);
    }

    psHead = (ncUserContHead *)utShmArray(psShmHead,NC_LNK_ALLUSER);
    if(psHead == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("0403内存不正确"));
         return 0;
    }
    psUser0 = (ncUserCont *)((char *)psHead + psHead->lContPos);    
    pHash = (char *)psHead + psHead->lHashPos;
    psDbHead = utPltInitDb();
    utPltPutVarF(psDbHead,"maxuser","%lu", psHead->lMaxUser);
    utPltPutVarF(psDbHead,"sumuser","%lu", psHead->lSumUser);
    utPltPutVarF(psDbHead,"hashpos","%lu", psHead->lHashPos);
    utPltPutVarF(psDbHead,"contpos","%lu", psHead->lContPos);
    utPltPutVarF(psDbHead,"modflags","%lu",psHead->iModFlags);
    /* 显示索引信息  */
    switch(caDisp[0]) {
        case '1':   /* 按索引     */
            psIdIndex  = (ncUserIdIndex *)pasHashFirst(pHash,&sHashInfo);
            i=1;
            while(psIdIndex && i < lStart) {
                psIdIndex  = (ncUserIdIndex *)pasHashNext(&sHashInfo);
                i++;
            }
            iNum = 0;
            while(psIdIndex && iNum < lSum) {
                if(psIdIndex->pre == NC_IDBASE_PRE) {
                    utPltPutLoopVarF(psDbHead,"key",iNum,"%lu",psIdIndex->lId);
                }
                else if(psIdIndex->pre == NC_IPBASE_PRE) {
                    psIpIndex = (ncUserIpIndex *)psIdIndex;
                    utPltPutLoopVar(psDbHead,"key",iNum,utComHostIp(htonl(psIpIndex->ip)));
                }
                else {
                    psMacIndex = (ncUserMacIndex *)psIdIndex;
                    utPltPutLoopVar(psDbHead,"key",iNum,ncCvtMac(psMacIndex->mac));
                }
                psUser = &psUser0[psIdIndex->lIndex];
                utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum);
                utPltPutLoopVarF(psDbHead,"index",iNum,"%lu",psIdIndex->lIndex);
                utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",psUser->userid);
                utPltPutLoopVarF(psDbHead,"username",iNum,"%s",psUser->username);
                utPltPutLoopVarF(psDbHead,"dispname",iNum,"%s",psUser->dispname);
                if(psUser->psGroup) {
                    utPltPutLoopVarF(psDbHead,"groupname",iNum,"%s",psUser->psGroup->groupname);
                }
                utPltPutLoopVarF(psDbHead,"groupid",iNum,"%lu",psUser->groupid);
                utPltPutLoopVarF(psDbHead,"limitid",iNum,"%lu",psUser->sSumm.polid);
                utPltPutLoopVarF(psDbHead,"usebase",iNum,"%d",psUser->userbase);
                utPltPutLoopVarF(psDbHead,"mod",iNum,"%d",psUser->modflags);
                utPltPutLoopVarF(psDbHead,"mac",iNum,"%s",ncCvtMac(psUser->mac));
                utPltPutLoopVarF(psDbHead,"ip",iNum,"%s",utComHostIp(htonl(psUser->ip)));
                utPltPutLoopVarF(psDbHead,"lasttime",iNum,"%s",utTimFormat("%H:%M:%S",psUser->lasttime));
                utPltPutLoopVarF(psDbHead,"cstatus",iNum,"%d",psUser->cStatus);
                utPltPutLoopVarF(psDbHead,"userflags",iNum,"%d",psUser->useflags);
                utPltPutLoopVarF(psDbHead,"position",iNum,"%s",psUser->position);
                psIdIndex  = (ncUserIdIndex *)pasHashNext(&sHashInfo);
                i++;
                iNum++;
            }
            break;
        default:  /* 没有索引     */
            for(i=lStart-1,iNum=1;i<psHead->lSumUser && iNum<lSum+1;i++,iNum++) {
                psUser = &psUser0[i];
                utPltPutLoopVar(psDbHead,"key",iNum,"000");
                utPltPutLoopVarF(psDbHead,"index",iNum,"%lu",i);
                utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum);
                utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",psUser->userid);
                utPltPutLoopVarF(psDbHead,"username",iNum,"%s",psUser->username);
                utPltPutLoopVarF(psDbHead,"dispname",iNum,"%s",psUser->dispname);
                if(psUser->psGroup) {
                    utPltPutLoopVarF(psDbHead,"groupname",iNum,"%s",psUser->psGroup->groupname);
                }
                utPltPutLoopVarF(psDbHead,"groupid",iNum,"%lu",psUser->groupid);
                utPltPutLoopVarF(psDbHead,"limitid",iNum,"%lu",psUser->sSumm.polid);
                utPltPutLoopVarF(psDbHead,"usebase",iNum,"%d",psUser->userbase);
                utPltPutLoopVarF(psDbHead,"mod",iNum,"%d",psUser->modflags);
                utPltPutLoopVarF(psDbHead,"mac",iNum,"%s",ncCvtMac(psUser->mac));
                utPltPutLoopVarF(psDbHead,"ip",iNum,"%s",utComHostIp(htonl(psUser->ip)));
                utPltPutLoopVarF(psDbHead,"lasttime",iNum,"%s",utTimFormat("%H:%M:%S",psUser->lasttime));
                utPltPutLoopVarF(psDbHead,"cstatus",iNum,"%d",psUser->cStatus);
                utPltPutLoopVarF(psDbHead,"userflags",iNum,"%d",psUser->useflags);
                utPltPutLoopVarF(psDbHead,"position",iNum,"%s",psUser->position);
            }
            break;
    }
    utPltPutVarF(psDbHead,"start", "%lu",lStart);
    utPltPutVarF(psDbHead,"sum", "%lu",lSum);
    utPltPutVarF(psDbHead,"disp","%s",caDisp);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"test/nc_test_alluser.htm");        
    return 0;
}


/* 显示网络设置信息  */
int ncWebTestShowNetwork(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    
    ncIpNetwork  *psNetwork;
    int iSum,i;
    utPltDbHead *psDbHead;
       long lGroupid0=0;
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
    
    psNetwork = (ncIpNetwork *)utShmArray(psShmHead,NC_LNK_NETWORK);
    if(psNetwork == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("0403内存不正确"));
         return 0;
    }
    iSum = utShmArrayRecord(psShmHead,NC_LNK_NETWORK);
    psDbHead = utPltInitDb();
    for(i=0;i<iSum;i++) {
       utPltPutLoopVar(psDbHead,"lower",i+1,utComHostIp(htonl(psNetwork[i].lIpLower)));
       utPltPutLoopVar(psDbHead,"upper",i+1,utComHostIp(htonl(psNetwork[i].lIpUpper)));
       utPltPutLoopVarF(psDbHead,"flags",i+1,"%d",psNetwork[i].iFlags);
       utPltPutLoopVarF(psDbHead,"num",i+1,"%d",i+1);
    }        
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"test/nc_test_network.htm");        
    return 0;
}

