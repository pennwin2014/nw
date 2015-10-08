/*告警通知*/
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "ncdef.h"
#include "ncwarn.h"

/* 告警通知 走马灯 */
int nc_warn_md(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    ncWarnSystem **pWarnSystem;
   	int iReturn;
    int i,lSum;
    char caMsg[256];
    char caFlag[8];
    ncUserCont *psUser;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif    
    iReturn = utMsgGetSomeNVar(psMsgHead,1,
   	                "flag",   UT_TYPE_STRING, 2, caFlag);
    pWarnSystem=ncWarnGetWarnInfo(psShmHead, 0,0,&lSum);
    if(pWarnSystem==NULL)
    {
        lSum = 0;
    }
    psDbHead = utPltInitDb();
    if(lSum>0) utPltPutVar(psDbHead,"flag","1");
//    printf("lSum=%ld\n",lSum);
    for(i=0;i<lSum;i++){
    	if(i%2==0)
    	utPltPutLoopVar(psDbHead,"bcolor",i+1,"#F5F5F5");
    	else
    	utPltPutLoopVar(psDbHead,"bcolor",i+1,"#e5F5F5");
    	
    	psUser = ncUtlGetUserContById(psShmHead,pWarnSystem[i]->lUserid);
      if(psUser) {
      	utPltPutLoopVar(psDbHead,"username",i+1,psUser->dispname);
      	utPltPutLoopVar(psDbHead,"ip",i+1,utComHostIp(htonl(psUser->ip)));
        }
    	utPltPutLoopVarF(psDbHead,"num",i+1,"%lu",i+1);
    	utPltPutLoopVar(psDbHead,"msg",i+1,pWarnSystem[i]->caMesg);
    	utPltPutLoopVar(psDbHead,"lasttime",i+1,utTimFormat("%H:%M:%S",pWarnSystem[i]->lLastTime));
    	if(i>20) break;
		}
    if(pWarnSystem) free(pWarnSystem);
    
    if(atol(caFlag)==1)
       utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/curwarn_info.htm");
    else
       utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/md.htm");
 
    return 0;
}

//历史查询
int nc_warn_hist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    ncWarnSystem **pWarnSystem;
   	int iReturn;
    int i,lSum;
    char caMsg[256];
    char caFlag[8];
    ncUserCont *psUser;

    pWarnSystem=ncWarnGetWarnInfo(psShmHead, 0,1,&lSum);
    if(pWarnSystem==NULL)
    {
        lSum = 0;
    }
    psDbHead = utPltInitDb();
   if(lSum==0){
   	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/warn_msg.htm");
   	return 0;
  }
   
    for(i=0;i<lSum;i++){
    	if(i%2==0)
    	utPltPutLoopVar(psDbHead,"bcolor",i+1,"#F5F5F5");
    	else
    	utPltPutLoopVar(psDbHead,"bcolor",i+1,"#e5F5F5");
    	
    	psUser = ncUtlGetUserContById(psShmHead,pWarnSystem[i]->lUserid);
      if(psUser) {
      	utPltPutLoopVar(psDbHead,"username",i+1,psUser->dispname);
      	utPltPutLoopVar(psDbHead,"ip",i+1,utComHostIp(htonl(psUser->ip)));
        }
    	utPltPutLoopVarF(psDbHead,"num",i+1,"%lu",i+1);
    	utPltPutLoopVar(psDbHead,"msg",i+1,pWarnSystem[i]->caMesg);
    	utPltPutLoopVar(psDbHead,"lasttime",i+1,utTimFormat("%H:%M:%S",pWarnSystem[i]->lLastTime));
    	if(i>10) break;
		}
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/hiswarn_info.htm");
    return 0;
}