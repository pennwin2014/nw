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

/* 显示缓存中IP包的连接数  */
int ncWebTestShowIpBuf(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncIp *psIp;
    utPltDbHead *psDbHead;
    pasHashInfo sHashInfo;    
    int iReturn;
    char caStart[12],caLine[12],caSip[16];
    unsigned long lLine,lStart,lSip;
    char *pHash;
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
   
    printf(" Show Ip Buffer  Pid is %d \n",getpid());
    utMsgPrintMsg(psMsgHead);
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,3,
	                "start",    UT_TYPE_STRING,8, caStart,
	                "line",     UT_TYPE_STRING,8, caLine,
	                "sip",      UT_TYPE_STRING,15,caSip);
    if(utStrIsSpaces(caStart)) {
        lStart = 1;
    }
    else {
        lStart = atol(caStart);
    }
    if(utStrIsSpaces(caLine)) {
        lLine = 50;
    }
    else {
        lLine = atol(caLine);
    }
    if(utStrIsSpaces(caSip)) {
        lSip = 0;
    }
    else {
        lSip = ntohl(utComHostAddress(caSip));
    }
    printf(" 111111111111111\n");
    pHash = utShmHashHead(psShmHead,NC_LNK_IPPKG);
    if(pHash == NULL) {
        return (-1);
    }
    psDbHead = utPltInitDb();

    psIp  = (ncIp *)pasHashFirst(pHash,&sHashInfo);
    iNum = 0;
    i = 1;
    printf(" 222222222222 lStart==%d\n",lStart);
    utPltPutVarF(psDbHead,"start", "%lu",lStart);

    while(psIp && i < lStart) {
        if(lSip == 0 || psIp->lSip == lSip) {
            i++;
        }
        psIp = (ncIp *)pasHashNext(&sHashInfo);
    }
    printf(" 3333333333222222 lLine==%d\n",lLine);
    utPltPutVar(psDbHead,"lsip", caSip);
    utPltPutVarF(psDbHead,"line","%lu",lLine);
    iNum = 0;
    while(iNum < lLine && psIp) {
        printf(" 4444333333222222 lLine==%d iNum==%d\n",lLine,iNum);

        if(lSip == 0 || psIp->lSip == lSip) {
            iNum++;
            utPltPutLoopVar(psDbHead,"sip",iNum,utComHostIp(htonl(psIp->lSip)));
printf("Sip == %s \n",utComHostIp(htonl(psIp->lSip)));
            utPltPutLoopVar(psDbHead,"dip",iNum,utComHostIp(htonl(psIp->lDip)));
            utPltPutLoopVarF(psDbHead,"dport",iNum,"%u",psIp->nDport);
            utPltPutLoopVarF(psDbHead,"prot",iNum,"%d",psIp->cType);
            utPltPutLoopVarF(psDbHead,"ltime",iNum,"%s",utTimFormat("%H:%M:%S",psIp->lLastTime));
            utPltPutLoopVarF(psDbHead,"stop",iNum,"%lu",psIp->lStop);
            utPltPutLoopVarF(psDbHead,"ubytes",iNum,"%llu",psIp->lBytes[0]);
            utPltPutLoopVarF(psDbHead,"dbytes",iNum,"%llu",psIp->lBytes[1]);
            utPltPutLoopVarF(psDbHead,"status",iNum,"%d",psIp->cStatus);
            utPltPutLoopVarF(psDbHead,"use",iNum,"%d",psIp->cUseFlags);
            utPltPutLoopVarF(psDbHead,"service",iNum,"%u",psIp->lService);
            utPltPutLoopVarF(psDbHead,"url",iNum,"%lu",psIp->lUrl);
printf("kkkSip == %s  %lu\n",utComHostIp(htonl(psIp->lSip)),psIp->psUser);
            if(psIp->psUser) {
                  utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",psIp->psUser->lId);
                  if(psIp->psUser->psUser) {
                      utPltPutLoopVarF(psDbHead,"name",iNum,"%s",psIp->psUser->psUser->dispname);
                  }
            }
            utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",i);
            i++;
printf("2Sip == %s \n",utComHostIp(htonl(psIp->lSip)));
        }
        psIp = (ncIp *)pasHashNext(&sHashInfo);
    }
    utPltPutVarF(psDbHead,"next", "%lu",i);
    utPltPutVarF(psDbHead,"sum", "%lu",pasHashRecord(pHash));
    utPltPutVarF(psDbHead,"max", "%lu",pasHashMaxRecord(pHash));
printf(" Stop...\n");    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"test/nc_test_ipbuf.htm");
}




/* 显示缓存中TCP连线  */
int ncWebTestShowTcpBuf(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncTcp *psTcp;
    utPltDbHead *psDbHead;
    pasHashInfo sHashInfo;    
    int iReturn;
    char caStart[12],caLine[12],caSip[16];
    unsigned long lLine,lStart,lSip;
    char *pHash;
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
printf(" Show Tcp Buff \n");
    utMsgPrintMsg(psMsgHead);
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,3,
	                "start",    UT_TYPE_STRING,8, caStart,
	                "line",     UT_TYPE_STRING,8, caLine,
	                "sip",      UT_TYPE_STRING,15,caSip);
    if(utStrIsSpaces(caStart)) {
        lStart = 1;
    }
    else {
        lStart = atol(caStart);
    }
    if(utStrIsSpaces(caLine)) {
        lLine = 50;
    }
    else {
        lLine = atol(caLine);
    }
    if(utStrIsSpaces(caSip)) {
        lSip = 0;
    }
    else {
        lSip = ntohl(utComHostAddress(caSip));
    }
    pHash = utShmHashHead(psShmHead,NC_LNK_IPCONTACT);
    if(pHash == NULL) {
        return (-1);
    }
    psTcp  = (ncTcp *)pasHashFirst(pHash,&sHashInfo);
    iNum = 0;
    i = 1;
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"lsip", caSip);
    utPltPutVarF(psDbHead,"line","%lu",lLine);
    utPltPutVarF(psDbHead,"start","%lu",lStart);
    while(psTcp && i < lStart) {
        if(lSip == 0 || psTcp->lSip == lSip) {
            i++;
        }
        psTcp = (ncTcp *)pasHashNext(&sHashInfo);
    }
    iNum = 0;
    while(iNum < lLine && psTcp) {
        if(lSip == 0 || psTcp->lSip == lSip) {
            iNum++;
            utPltPutLoopVar(psDbHead,"sip",iNum,utComHostIp(htonl(psTcp->lSip)));
            utPltPutLoopVar(psDbHead,"dip",iNum,utComHostIp(htonl(psTcp->lDip)));
            utPltPutLoopVarF(psDbHead,"sport",iNum,"%u",psTcp->nSport);
            utPltPutLoopVarF(psDbHead,"dport",iNum,"%u",psTcp->nDport);
            utPltPutLoopVarF(psDbHead,"ltime",iNum,"%s",utTimFormat("%H:%M:%S",psTcp->lLastTime));
            utPltPutLoopVarF(psDbHead,"status",iNum,"%d",psTcp->cStatus);
            utPltPutLoopVarF(psDbHead,"diction",iNum,"%d",psTcp->cDiction);
            utPltPutLoopVarF(psDbHead,"connect",iNum,"%d",psTcp->cConnect);
            utPltPutLoopVarF(psDbHead,"stop",iNum,"%lu",psTcp->lStop);
            utPltPutLoopVarF(psDbHead,"seq",iNum,"%lu",psTcp->lSeq);
            utPltPutLoopVarF(psDbHead,"ack",iNum,"%lu",psTcp->lAck_seq);
            utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",i);
	     if(psTcp->psIp) {
		 	utPltPutLoopVarF(psDbHead,"ip",iNum,"%lu",psTcp->psIp);
	     }
            i++;
        }
        psTcp = (ncTcp *)pasHashNext(&sHashInfo);
    }
    utPltPutVarF(psDbHead,"next", "%lu",i);
    utPltPutVarF(psDbHead,"sum", "%lu",pasHashRecord(pHash));
    utPltPutVarF(psDbHead,"max", "%lu",pasHashMaxRecord(pHash));
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"test/nc_test_tcpbuf.htm");
}



/* 例外访问记录  */
int ncWebTestShowIpExcept(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncIpExcept *psTcp;
    utPltDbHead *psDbHead;
    pasHashInfo sHashInfo;    
    int iReturn;
    char caStart[12],caLine[12],caSip[16];
    unsigned long lLine,lStart,lSip;
    char *pHash;
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
	                "line",     UT_TYPE_STRING,8, caLine,
	                "sip",      UT_TYPE_STRING,15,caSip);
    if(utStrIsSpaces(caStart)) {
        lStart = 1;
    }
    else {
        lStart = atol(caStart);
    }
    if(utStrIsSpaces(caLine)) {
        lLine = 50;
    }
    else {
        lLine = atol(caLine);
    }
    if(utStrIsSpaces(caSip)) {
        lSip = 0;
    }
    else {
        lSip = ntohl(utComHostAddress(caSip));
    }
    pHash = utShmHashHead(psShmHead,NC_LNK_IPEXCEPT);
    if(pHash == NULL) {
        return (-1);
    }
    psTcp  = (ncIpExcept *)pasHashFirst(pHash,&sHashInfo);
    iNum = 0;
    i = 1;
    while(psTcp && i < lStart) {
        if(lSip == 0 || psTcp->lSip == lSip) {
            i++;
        }
        psTcp = (ncIpExcept *)pasHashNext(&sHashInfo);
    }
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"lsip", caSip);
    utPltPutVarF(psDbHead,"line","%lu",lLine);
    iNum = 0;
    while(iNum < lLine && psTcp) {
        if(lSip == 0 || psTcp->lSip == lSip) {
            iNum++;
            utPltPutLoopVar(psDbHead,"sip",iNum,utComHostIp(htonl(psTcp->lSip)));
            utPltPutLoopVar(psDbHead,"dip",iNum,utComHostIp(htonl(psTcp->lDip)));
            utPltPutLoopVarF(psDbHead,"dport",iNum,"%u",psTcp->nDport);
            utPltPutLoopVarF(psDbHead,"ltime",iNum,"%s",utTimFormat("%H:%M:%S",psTcp->lLastTime));
            utPltPutLoopVarF(psDbHead,"count",iNum,"%lu",psTcp->lCount);
            utPltPutLoopVarF(psDbHead,"prot",iNum,"%d",psTcp->cType);
            utPltPutLoopVarF(psDbHead,"status",iNum,"%d",psTcp->cStatus);
            utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",psTcp->lUserid);
            utPltPutLoopVarF(psDbHead,"rid",iNum,"%lu",psTcp->lRid);
            utPltPutLoopVarF(psDbHead,"msg",iNum,"%s",psTcp->caMsg);
            utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",i);
            i++;
        }
        psTcp = (ncIpExcept *)pasHashNext(&sHashInfo);
    }
    utPltPutVarF(psDbHead,"start", "%lu",i);
    utPltPutVarF(psDbHead,"sum", "%lu",pasHashRecord(pHash));
    utPltPutVarF(psDbHead,"max", "%lu",pasHashMaxRecord(pHash));
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"test/nc_test_ipexcept.htm");
}


/* 在线Htpp请求*/
int ncWebTestShowOnlineWeb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

    	int i,iNum,iReturn,j;
    	int iMaxRecord;
    	ncHttpBuffer *psHttp;
    	utPltDbHead *psDbHead;
	char caStart[12];
	char caLine[12];
	char caSip[16];
	unsigned long lSip,lStart,lLine;
   long lGroupid0=0;
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }

	utMsgPrintMsg(psMsgHead);
	
	iReturn = utMsgGetSomeNVar(psMsgHead,3,
	                "start",    UT_TYPE_STRING,8, caStart,
	                "line",     UT_TYPE_STRING,8, caLine,
	                "sip",      UT_TYPE_STRING,15,caSip);
    	if(utStrIsSpaces(caStart)) {
        	lStart = 1;
    	}
    	else {
        	lStart = atol(caStart);
    	}
    	if(utStrIsSpaces(caLine)) {
        	lLine = 50;
    	}
    	else {
        	lLine = atol(caLine);
    	}
    	if(utStrIsSpaces(caSip)) {
        	lSip = 0;
    	}
    	else {
        	lSip = ntohl(utComHostAddress(caSip));
    	}
   	psHttp = (ncHttpBuffer *)utShmArray((utShmHead *)psShmHead,NC_LNK_WEB);
printf(" 111111111\n");		

    	if(psHttp == NULL) {
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_bak.htm","ncWebShowIp","Internal Error1 ");
        	return 0;
    	}
printf(" 2222222222222\n");		
    	iMaxRecord = utShmArrayRecord(psShmHead,NC_LNK_WEB);
    	psDbHead = utPltInitDb();
	iNum = 1;
	utPltPutVarF(psDbHead,"start","%lu",lStart);
	utPltPutVarF(psDbHead,"ip","%s",caSip);
	utPltPutVarF(psDbHead,"line","%lu",lLine);
	utPltPutVarF(psDbHead,"max","%lu",iMaxRecord);
printf(" 3333\n");		

    	for(j=lStart;j<iMaxRecord&&iNum <= lLine;j++) {
        	if(psHttp[j].cUseFlags > 0) {
			if(lSip == 0 || psHttp[j].lSip == lSip) {
printf(" 4444\n");		
                        	utPltPutLoopVarF(psDbHead,"num",  iNum,"%d",j);
                        	utPltPutLoopVar(psDbHead,"url",  iNum,psHttp[j].caUrl);
                        	utPltPutLoopVar(psDbHead,"host",  iNum,psHttp[j].caHost);
			       utPltPutLoopVar(psDbHead,"sip",  iNum,
				   	utComHostIp(htonl(psHttp[j].lSip)));
							
			       utPltPutLoopVar(psDbHead,"starttime",  iNum,
				   	utTimFormat("%H:%M:%S",psHttp[j].lStartTime));
				
			       utPltPutLoopVar(psDbHead,"lasttime",  iNum,
				   	utTimFormat("%H:%M:%S",psHttp[j].lLastTime));
			       utPltPutLoopVarF(psDbHead,"bytes",  iNum,"%lu",psHttp[j].lBytes);
			       utPltPutLoopVarF(psDbHead,"userid",  iNum,"%lu",psHttp[j].userid);
			       utPltPutLoopVarF(psDbHead,"flags",  iNum,"%d",psHttp[j].cUseFlags);
printf(" 555\n");		
				   iNum++;
            		}
              }
    	}
printf(" 777\n");		
	utPltPutVarF(psDbHead,"next","%lu",j);
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"test/nc_test_onlineweb.htm");
    	return 0;
}

