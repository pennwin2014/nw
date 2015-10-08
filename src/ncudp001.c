#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include "utoall.h"
#include "ncdef.h"
#include "nwdef.h"
#define NC_TESTUDP 18000
#define NC_CMD1UDP  18001
#define NC_CMD2UDP  18002
int ncTestUdp(utShmHead *psShmHead, utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int ncTestTcp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncCmd1Udp(utShmHead *psShmHead, utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int ncCmd2Udp(utShmHead *psShmHead, utComSockAddr *psSockAddr,utMsgHead *psMsgHead);

int nwUdpReqId(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpLogin(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpHeartInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpNetTest(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpHardInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpSoftInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);

int nwUdpLogInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpSysInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpProcessInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);

int ncUdpDevStatus(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int ncUdpAppList(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int ncUdpProList(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int ncUdpProStop(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int ncUdpSysInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int ncUdpOther(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpLogin_n2(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpExit_n2(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpPassChg_n2(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpLogin_n(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpHeartInfo_n(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpHeartInfo_p1(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpLogInfo_n(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpSendProcyInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpSendJoinProcyInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpSendWatchProcyInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpSendDateInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);

//ncudp_usbkey.c
int nwUdpHeart_noMonKey(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpLogin_Monkey(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int usbKeyUdpHeartInfo(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpLogin_Passkey(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpRecvIpMac(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int nwUdpKickOff(utShmHead *psShmHead,utComSockAddr *psSockAddr,utMsgHead *psMsgHead);
int ncUdpInitFun(utShmHead *psShmHead)
{
    utComSetUdpFun(NC_UDP_IDREQ,nwUdpReqId);
    utComSetUdpFun(NC_UDP_REGREQ,nwUdpLogin);
    utComSetUdpFun(NC_UDP_HEARTBEAT,nwUdpHeartInfo);
    utComSetUdpFun(NC_UDP_TESTNET,nwUdpNetTest);
    utComSetUdpFun(NC_UDP_HDWINFO,nwUdpHardInfo);
    utComSetUdpFun(NC_UDP_SOFTINFO,nwUdpSoftInfo);

    utComSetUdpFun(NC_UDP_ACCLOG,nwUdpLogInfo);
    utComSetUdpFun(NC_UDP_SYSINFO,nwUdpSysInfo);
    utComSetUdpFun(NC_UDP_PROCINFO,nwUdpProcessInfo);
        

    
    utComSetUdpFun(NC_UDP_DEVSTATUS,ncUdpDevStatus);
    utComSetUdpFun(NC_UDP_APPINFO,ncUdpAppList);
    utComSetUdpFun(NC_UDP_APPQUIT,ncUdpOther);
    utComSetUdpFun(NC_UDP_CAPSCREEN,ncUdpOther);
    utComSetUdpFun(NC_UDP_UPDATEINFO,ncUdpOther);
    
    
    
    utComSetUdpFun(NC_CLIENT_LOGIN,nwUdpLogin_n);
    utComSetUdpFun(NC_CLIENT_BEAT,nwUdpHeartInfo_n);
    utComSetUdpFun(NC_CLIENT_LOGSEND,nwUdpLogInfo_n);
    utComSetUdpFun(NC_CLIENT_LOGIN2,nwUdpLogin_n2);
    utComSetUdpFun(NC_CLIENT_EXIT2,nwUdpExit_n2);
    utComSetUdpFun(NC_CLIENT_PASSCHG,nwUdpPassChg_n2);
    
    utComSetUdpFun(NC_CLIENT_BEAT2,nwUdpHeartInfo_p1);
    utComSetUdpFun(NC_CLIENT_REQPROCY,nwUdpSendProcyInfo);
    utComSetUdpFun(NC_CLIENT_REQJOINPROCY,nwUdpSendJoinProcyInfo);
    utComSetUdpFun(NC_CLIENT_REQWATCHPROCY,nwUdpSendWatchProcyInfo);
    utComSetUdpFun(NC_CLIENT_REQDATE,nwUdpSendDateInfo);
    
    utComSetUdpFun(NC_USBKEY_NOMONHEART,nwUdpHeart_noMonKey);
    utComSetUdpFun(NC_USBKEY_MONLOGIN,nwUdpLogin_Monkey);
    
    utComSetUdpFun(NC_USBKEY_MONHEART,usbKeyUdpHeartInfo);
    utComSetUdpFun(NC_USBKEY_PASSLOGIN,nwUdpLogin_Passkey);
    
    utComSetUdpFun(NC_CLIENT_IPMAC,nwUdpRecvIpMac);
    utComSetUdpFun(NC_CLIENT_KICKOFF,nwUdpKickOff);
    
    
  //     utComSetUdpFun(NC_TESTUDP,ncTestUdp);
 //      pasSetTcpFunNameS("ncTestTcp",ncTestTcp,NULL,0,PAS_CRYPT_TEA);
       
//        pasSetTcpFunName("ncTestTcp",ncTestTcp,NULL,0,0);
    return 0;
}



