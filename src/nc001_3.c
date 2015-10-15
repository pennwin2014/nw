#define PAS_SRCFILE   2101
#define ncmax(a,b) ((a) > (b) ? (a) : (b))
#define ncMin(a,b) ((a) < (b) ? (a) : (b))
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
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <dirent.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/resource.h>
#include "config.h"
#include "utoall.h"
#include "pasdb.h"
#include "ncdef.h"
#include "nclimit.h"
#include "ncpkg.h"
#include "pasutl.h"
void ncSignal_fatal(int sig);
void ncSignal_hup(int sig);
void ncReload_sig(int sig);
int ncStartProEimPkg(utShmHead *psShmHead);
int ncStartPkgProcess(utShmHead *psShmHead);
int ncStartOnTimeDo(utShmHead *psShmHead);
int ncCtlStartIpt(utShmHead *psShmHead);
int ncUtlStartPkgCtl(utShmHead *psShmHead);
int ncStartMailMon(utShmHead *psShmHead);
// int ncStartPostMon(utShmHead *psShmHead);
int ncCtlStartNcClient(utShmHead *psShmHead);
int ncCtlStartSendReal(utShmHead *psShmHead);
int ncCtlStartSendData(utShmHead *psShmHead);
int ncStartProEimTestPkg(utShmHead *psShmHead);
int ncUtlCkOnlineClient(utShmHead *psShmHead);
int ncStartNtAuth(utShmHead *psShmHead);
int ncStartPostMail(utShmHead *psShmHead);
int ncStartIc(utShmHead *psShmHead);
int ncMonSystem(utShmHead *psShmHead);
int ncUtlSynDbLog(utShmHead *psShmHead);
int ncUtlFileServer(utShmHead *psShmHead);
int ncUtlProcessDbLog(utShmHead *psShmHead);
int ncUtlAutoUpdate(utShmHead *psShmHead);
int  ncStartSysLog(utShmHead *psShmHead);
int ncMailAutoBack(utShmHead *psShmHead);
int ncAutoClean(utShmHead *psShmHead);
int ncUtlCheckKav(utShmHead *psShmHead);
int nwUtlCheckCompute(utShmHead *psShmHead);
int ncUtlDbCheckAndBack(utShmHead *psShmHead);
static unsigned long lRestartTime;
int ncGetSmtpProxy1(utShmHead *psShmHead);
int ncGetSmtpProxy2(utShmHead *psShmHead);
int ncGetSmtpProxy3(utShmHead *psShmHead);
int ncGetSmtpProxy4(utShmHead *psShmHead);
int ncUtlDMailSendStart(utShmHead *psShmHead);
int ncUtlPasProxy(utShmHead *psShmHead);
int ncUtlPollMsnFile(utShmHead *psShmHead);
int ncUtlCheckProxy(utShmHead *psShmHead);
int ncSwitchPoolSwitch(utShmHead *psShmHead);
int ncUtlStartCheckNetInfo(utShmHead *psShmHead);
int ncPkgStartGet(utShmHead *psShmHead);
int ncAutoStat(utShmHead *psShmHead);
int ncUserSysUserDb(utShmHead *psShmHead);
int ncUtlCvtCont(utShmHead *psShmHead);
int ncCtlCheckIptables(utShmHead *psShmHead);
int ncUtlCheckWorkTime(utShmHead *psShmHead);
int ncStartVidCheck(utShmHead *psShmHead);
int ncUtlTcCheckWidth(utShmHead *psShmHead);
int ncUtlCheckOnlineUser(utShmHead  *psShmHead);
int ncReportSend(utShmHead  *psShmHead);
int ncDbSysOnUserForRec(utShmHead *psShmHead);
int gdgStartGcb(utShmHead *psShmHead);
int ncSnmpPollSwitch(utShmHead *psShmHead);
int ncWarnCheckForbitUser(utShmHead *psShmHead);
int ncUtlCheckIpFlow(utShmHead  *psShmHead);    // 检查流量
int ncUrlCheckMemUrl(utShmHead *psShmHead);     // 检查网址库
int ncMetaCheckQosTime(utShmHead *psShmHead);
int ncGetGroupsFlow(utShmHead *psShmHead);
int ncPppGetPPPOEName(utShmHead *psShmHead);
int ncCluUdpServer(utShmHead *psShmHead);
int ncCluUdpClient(utShmHead *psShmHead);
int ncCheckBbsDup(utShmHead *psShmHead);
int ncWarnErr(utShmHead *psShmHead);
int ncCountLog(utShmHead *psShmHead0);
int ncUtlSynHotelUser(utShmHead *psShmHead);   // 同步酒店
int ncUtlAutoSysLdap(utShmHead *psShmHead);    // 自动同步域用户
int ncUtlChkDsclt(utShmHead *psShmHead);       // 自动检查登录超时
int ncUtlSynFileLog(utShmHead *psShmHead);     // 同步IPLOG和HttpLog日志文件

int ncAutoGenReport(utShmHead *psShmHead);     // 自动生成统计报表

int ncAutoSysJG(utShmHead *psShmHead);     // 同步金龟治安接口
int ncUtlAnyQqCheck(utShmHead *psShmHead);
int ncHttpCvtHttp2Snap(utShmHead *psShmHead);
int ncUtlDoOutDbLog(utShmHead *psShmHead);
int ncClientSendLogToDataServer(utShmHead *psShmHead);
int ncAutoSysJG1(utShmHead *psShmHead);
int ncAutoSysJG2(utShmHead *psShmHead);
int ncsSysNshUser(utShmHead *psShmHead);
int ncDoDrcom(utShmHead *psShmHead);
int ncStartSmtpProxy0(utShmHead *psShmHead);
int ncAutoSysWmd(utShmHead *psShmHead); //同步外迈迪用户到验证服务器
int ncCapUrlCheckAndSave(utShmHead *psShmHead);
int ncCapUrlAutoTest(utShmHead *psShmHead);
int ncGenIBDblog(utShmHead *psShmHead);
int ncAutoSysHuserFromS(utShmHead *psShmHead);
int ncDbmonCheckTempFile(utShmHead *psShmHead);
int ncDelwithMwduser(utShmHead *psShmHead);
int ncUtlStartLoad2Db(utShmHead *psShmHead, char *pLog);
int ncUtlCheckDevFlow(utShmHead *psShmHead);
int ncSmsSendMesg(utShmHead *psShmHead);
int ncAutoSysHuIMToUser(utShmHead *psShmHead);
int ncApPoolApMacAndSendSms(utShmHead *psShmHead);

int nwLoadFileToDb(utShmHead *psShmHead);
int lanAutoClean(utShmHead *psShmHead);
int lanAutoBack(utShmHead *psShmHead);
int lanLoadLogToDB(utShmHead *psShmHead);

/* 启动部分进程  */

int ncStartSomeProcess(utShmHead *psShmHead)
{
    int iCtlPid, iPid1, iPid, iStatus;
    ncProcessHead *psFunHead;
    unsigned long lTime, lStartTime;

    ncStartSetAllProcess(psShmHead);

    //  ncUtlStartProcess(psShmHead);
    lStartTime = time(0);
    //   printf("StartSome Process .....\n");
    iPid = fork();
    if(iPid == 0)
    {

        iCtlPid = ncUtlStartProcess(psShmHead);
        while(1)
        {
            lTime = time(0);
            if(iCtlPid > 0)
            {
                iPid1 = 1;
                while(iPid1 > 0)
                {
                    iPid1 = waitpid(-1, &iStatus, WNOHANG);

                    if(iPid1 == iCtlPid)
                    {
                        iStatus = WEXITSTATUS(iStatus);
                        pasLog(PAS_LOG_EVENT, "[nc]Pid %d Exit Status is %d\n", iPid1, iStatus);
                        printf(" Pid1 %d Exit Status is %d  MyPid=%d\n", iPid1, iStatus, getpid());
                        if(iPid1 > 0) kill(iPid1, 9);
                        ncUtlStopProcess(psShmHead);
                        iCtlPid = ncUtlStartProcess(psShmHead);
                    }
                }
                // 检查进程
                psFunHead = (ncProcessHead *)utShmArray(psShmHead, NC_LNK_PROCESS);
                if(psFunHead)
                {
                    if(lTime > psFunHead->lLastTime + 300 && lTime  > 30 + lStartTime)
                    {
                        pasLog(PAS_LOG_ERROR, " Main Process TimeOut, Restart Process  %lu %lu %lu", lTime, psFunHead->lLastTime, lTime - psFunHead->lLastTime);
                        if(iCtlPid > 0) kill(iCtlPid, 9);
                        ncUtlStopProcess(psShmHead);
                        iCtlPid = ncUtlStartProcess(psShmHead);
                    }
                }
                sleep(3);
            }
            else
            {
                sleep(10);
                iCtlPid = ncUtlStartProcess(psShmHead);
            }
        }
        exit(0);
    }
    else
    {
        return iPid;
    }

}

int ncStartSetAllProcess(utShmHead *psShmHead)
{
    int iReturn, iLicense;
    int i;
    char *p, *pName, *pName1, *pName2, *pName3;
    unsigned long lTime;
    char *pProdSn;

    signal(SIGHUP, ncSignal_hup);
    signal(SIGINT, ncSignal_fatal);
    signal(SIGQUIT, ncSignal_fatal);
    signal(SIGILL, ncSignal_fatal);
    signal(SIGTRAP, ncSignal_fatal);
    signal(SIGIOT, ncSignal_fatal);
    signal(SIGFPE, ncSignal_fatal);
    signal(SIGTERM, ncSignal_fatal);
    signal(SIGPIPE, ncSignal_hup);
    signal(SIGUSR1, ncSignal_hup);
    signal(SIGUSR2, ncSignal_hup);

    ncUtlInitProcess(psShmHead, 80);

    /* 日志服务器  */
    //   ncUtlSetDoProcess(psShmHead,"SetShmTime",0,0,utShmSetTime,"Yes",71000L,0);
    //   ncUtlSetDoProcess(psShmHead,"SysLog",0,0,ncStartSysLog,"Yes",71000L,0);
    //   ncUtlSetPkgCapProcess(psShmHead);
    //    ncUtlSetDoProcess(psShmHead,"IptCtl",0,1,ncCtlStartIpt,"Yes",7201L,0);
    //    if(ncUtlProcessIsSet(psShmHead,"IptCtl","Yes")) {
    //        ncUtlSetDoProcess(psShmHead,"CheckIpt",0,1,ncCtlCheckIptables,"Yes",7213L,0);
    //    }
    //   ncUtlSetDoProcess(psShmHead,"AnyCont",0,3,ncUtlStartPkgCtl,"Yes",7120L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo",0,1,ncStartPkgProcess,"Yes",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo1",0,1,ncStartPkgProcess,"No",7300L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo2",0,1,ncStartPkgProcess,"No",7200L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo3",0,1,ncStartPkgProcess,"No",7400L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo4",0,1,ncStartPkgProcess,"No",7500L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo5",0,1,ncStartPkgProcess,"No",7600L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo6",0,1,ncStartPkgProcess,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo7",0,1,ncStartPkgProcess,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo8",0,1,ncStartPkgProcess,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo9",0,1,ncStartPkgProcess,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo10",0,1,ncStartPkgProcess,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo11",0,1,ncStartPkgProcess,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo12",0,1,ncStartPkgProcess,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo13",0,1,ncStartPkgProcess,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo14",0,1,ncStartPkgProcess,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PkgDo15",0,1,ncStartPkgProcess,"No",7100L,0);

    ncUtlSetDoProcess(psShmHead, "OnTime", 0, 10, ncStartOnTimeDo, "Yes", 7400L, 0);
    ncUtlSetDoProcess(psShmHead, "ChkDsclt", 0, 5, ncUtlChkDsclt, "No", 9100L, 0);
    ncUtlSetDoProcess(psShmHead, "DeskMgr", 0, 5, nwUtlCheckCompute, "Yes", 7100L, 0); // 内网管理

    ncUtlSetDoProcess(psShmHead, "LoadDb", 0, 5, nwLoadFileToDb, "No", 71000L, 0); // 内网管理
    ncUtlSetDoProcess(psShmHead, "Back", 0, 5, lanAutoBack, "Yes", 36000L, 0);
    ncUtlSetDoProcess(psShmHead, "Clean", 0, 5, lanAutoClean, "No", 36000L, 0);
    ncUtlSetDoProcess(psShmHead, "LogToDb", 0, 5, lanLoadLogToDB, "Yes", 36000L, 0);


    //    ncUtlSetDoProcess(psShmHead,"Count",0,20,ncCountLog,"Yes",71000L,0);
    //    ncUtlSetDoProcess(psShmHead,"MailMon",0,11,ncStartMailMon,"Yes",7230L,0);
    //    ncUtlSetDoProcess(psShmHead,"PostMon",0,11,ncStartPostMail,"Yes",6100L,0);
    //   ncUtlSetDoProcess(psShmHead,"ImFile",0,11,ncUtlPollMsnFile,"No",8100L,0);
    //    ncUtlSetDoProcess(psShmHead,"CheckWins",0,1,ncUtlCkOnlineClient,"Yes",7200L,0);
    //    ncUtlSetDoProcess(psShmHead,"VidOnline",0,6,ncStartVidCheck,"Yes",71000L,0);
    //    if(strcasecmp(utComGetVar_sd(psShmHead,"StartNcClient","No"),"Yes")==0) {
    //        ncUtlSetDoProcess(psShmHead,"NcClient",0,6,ncCtlStartNcClient,"Yes",7300L,0);
    //        ncUtlSetDoProcess(psShmHead,"NcCltSndReal",0,6,ncCtlStartSendReal,"Yes",7400L,0);
    //        ncUtlSetDoProcess(psShmHead,"NcCltSndData",0,6,ncCtlStartSendData,"Yes",7500L,0);

    //        ncUtlSetDoProcess(psShmHead,"PasProxy",0,5,ncUtlPasProxy,"Yes",7300L,0);
    //    }
    //    if(strcasecmp(utComGetVar_sd(psShmHead,"StartSendGA","No"),"Yes")==0) {
    //        if(strcasecmp(utComGetVar_sd(psShmHead,"StartNcClient","No"),"No")==0) {
    //            ncUtlSetDoProcess(psShmHead,"SendGA",0,6,ncCtlStartNcClient,"Yes",7300L,0);
    //            ncUtlSetDoProcess(psShmHead,"VidOnline",0,6,ncStartVidCheck,"Yes",71000L,0);
    //        }
    //    }
    //    ncUtlSetDoProcess(psShmHead,"NtAuth",0,5,ncStartNtAuth,"No",9100L,0);
    //    ncUtlSetDoProcess(psShmHead,"SysMon",0,5,ncMonSystem, "No",7600L,0);

    //    if(strcasecmp(utComGetVar_sd(psShmHead,"StartSynSystem","No"),"Yes")==0) {
    //        ncUtlSetDoProcess(psShmHead,"DbSync",0,5,ncUtlSynDbLog,"Yes",36000L,0);
    //        ncUtlSetDoProcess(psShmHead,"FileSrv",0,5,ncUtlFileServer,"Yes",7100L,0);
    //        ncUtlSetDoProcess(psShmHead,"DoDbLog",0,5,ncUtlProcessDbLog,"Yes",7100L,0);
    //        if( strcasecmp(utComGetVar_sd(psShmHead,"SaveIpLog","Yes"),"File")==0) {
    //            ncUtlSetDoProcess(psShmHead,"SynFileLog",0,9,ncUtlSynFileLog,"Yes",71000L,0);
    //        }
    //    }

    //    ncUtlSetDoProcess(psShmHead,"CluClient",0,8,ncCluUdpClient,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"CluServer",0,8,ncCluUdpServer,"No",7100L,0);

    //    ncUtlSetDoProcess(psShmHead,"ChkDupBbs",0,5,ncCheckBbsDup,"No",7100L,0);

    //    ncUtlSetDoProcess(psShmHead,"ChkOnUser",0,5,ncUtlCheckOnlineUser,"Yes",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"AutoUpdate",0,5,ncUtlAutoUpdate,"No",9100L,0);

    //    ncUtlSetDoProcess(psShmHead,"SysLdap",0,5,ncUtlAutoSysLdap,"No",9100L,0);

    //同步金龟治安接口
    //    ncUtlSetDoProcess(psShmHead,"SysJGUser",0,5,ncAutoSysJG,"No",9100L,0);
    //    ncUtlSetDoProcess(psShmHead,"SysJGUser1",0,5,ncAutoSysJG1,"No",9100L,0);
    //    ncUtlSetDoProcess(psShmHead,"SysJGUser2",0,5,ncAutoSysJG2,"No",9100L,0);
    //将外迈迪注册用户同步到管理中心
    //    ncUtlSetDoProcess(psShmHead,"SysWmdUser",0,5,ncAutoSysWmd,"No",9100L,0);
    //从管理中心同步酒店住客信息
    //    ncUtlSetDoProcess(psShmHead,"SysHUFC",0,5,ncAutoSysHuserFromS,"No",9100L,0);


    //启动厦门治安系统接口
    //    ncUtlSetDoProcess(psShmHead,"XMZAJK",0,5,ncDoDrcom,"No",9100L,0);
    //启动农商行同步
    //    ncUtlSetDoProcess(psShmHead,"SysNshUser",0,5,ncsSysNshUser,"No",9100L,0);

    //同步派博关键字
    //    ncUtlSetDoProcess(psShmHead,"Pbkeyword",0,5,ncAutoSysHuIMToUser,"No",9100L,0);



    //    ncUtlSetDoProcess(psShmHead,"AutoBack",0,5,ncMailAutoBack,"Yes",36000L,0);
    //    ncUtlSetDoProcess(psShmHead,"AutoClean",0,5,ncAutoClean,"Yes",36000L,0);
    //    ncUtlSetDoProcess(psShmHead,"VirusCheck",0,5,ncUtlCheckKav,"No",7100L,0);

    //    ncUtlSetDoProcess(psShmHead,"DbCheck",0,40,ncUtlDbCheckAndBack,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"ClientAuth",0,5,ncStartIc,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"MailSend",0,5,ncUtlDMailSendStart,"Yes",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"ProxyCheck",0,20,ncUtlCheckProxy,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"NetMon",0,20,ncUtlStartCheckNetInfo,"Yes",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"PortBase",0,5,ncSwitchPoolSwitch,"No",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"ncAutoStat",0,20,ncAutoStat,"Yes",7100L,0);
    //    ncUtlSetDoProcess(psShmHead,"CvtCont",0,20,ncUtlCvtCont,"No",71000L,0);
    //    ncUtlSetDoProcess(psShmHead,"CkWorkTime",0,20,ncUtlCheckWorkTime,"No",71000L,0);
    //    ncUtlSetDoProcess(psShmHead,"CkIpFlow",0,18,ncUtlCheckIpFlow,"Yes",71000L,0);
    //    ncUtlSetDoProcess(psShmHead,"CkFbitUser",0,20,ncWarnCheckForbitUser,"Yes",71000L,0);
    /*  检查 meta的老化   */
    //    if(strncasecmp(utComGetVar_sd(psShmHead,"IpControlBy","meta"),"meta",4)==0) {
    //        ncUtlSetDoProcess(psShmHead,"meta",0,20,ncMetaCheckQosTime,"Yes",71000L,0);
    //    }
    //    else {
    //        if(strcasecmp(utComGetVar_sd(psShmHead,"BandWidthLimit","No"),"Yes")==0) {
    //            ncUtlSetDoProcess(psShmHead,"WidthBand",0,20,ncUtlTcCheckWidth,"Yes",71000L,0);
    //        }
    //    }
    //    if(strcasecmp(utComGetVar_sd(psShmHead,"AutoRecFromDb","NO"),"YES") == 0) {
    //        ncUtlSetDoProcess(psShmHead,"RecFromDb",0,30,ncDbSysOnUserForRec,"Yes",71000L,0);
    //    }
    //    ncUtlSetDoProcess(psShmHead,"ReportSend",0,30,ncReportSend,"No",71000L,0);
    //    ncUtlSetDoProcess(psShmHead,"GCB",0,30,gdgStartGcb,"No",71000L,0);
    //    ncUtlSetDoProcess(psShmHead,"SnmpMac",0,30,ncSnmpPollSwitch,"No",71000L,0);
    //    ncUtlSetDoProcess(psShmHead,"CheckUrl",0,100,ncUrlCheckMemUrl,"Yes",71000L,0);

    //对阻断日志进行告警
    //     ncUtlSetDoProcess(psShmHead,"WarnErr",0,20,ncWarnErr,"No",7100L,0);
    //自动生成报表
    //     ncUtlSetDoProcess(psShmHead,"GenReport",0,20,ncAutoGenReport,"No",9100L,0);


    /* 部门即时流量采集   */
    //    ncUtlSetDoProcess(psShmHead,"GroupFlow",0,100,ncGetGroupsFlow,"No",71000L,0);
    /* PPP账号的采集   */
    ////    if(strcasecmp(utComGetVar_sd(psShmHead,"PppOeUserSyn","No"),"Process")==0) { /* 独立进程处理  */
    //       char caDev[32];
    //        for(i=0;i<16;i++) {
    //            sprintf(caDev,"PPP-eth%d",i);
    //            ncUtlSetDoProcess(psShmHead,caDev,0,50,ncPppGetPPPOEName,"No",7100L,0);
    //        }
    //    }

    //    ncUtlSetDoProcess(psShmHead,"SynHotel",0,6,ncUtlSynHotelUser,"No",71000L,0);
    //   if(strcasecmp(utComGetVar_sd(psShmHead,"GetQqContent","No"),"Yes")==0) {
    //        ncUtlSetDoProcess(psShmHead,"QqSession",0,120,ncUtlAnyQqCheck,"Yes",7100L,0);
    //    }
    //    if(strcasecmp(utComGetVar_sd(psShmHead,"HttpContent","No"),"No")!=0) {
    //        ncUtlSetDoProcess(psShmHead,"HtmlSnap",0,2,ncHttpCvtHttp2Snap,"Yes",7100L,0);
    //    }

    //  生成infobright数据库文件
    //    if(strcasecmp(utComGetVar_sd(psShmHead,"StartGenIBDblog","No"),"No")!=0) {
    //       ncUtlSetDoProcess(psShmHead,"GenIBDblog",0,2,ncGenIBDblog,"Yes",7100L,0);
    //    }
    //  处理迈外迪缓存里的用户信息
    // if(strcasecmp(utComGetVar_sd(psShmHead,"StartMwdUser","No"),"No")!=0) {
    //    ncUtlSetDoProcess(psShmHead,"MwdUser",0,2,ncDelwithMwduser,"Yes",7100L,0);
    //  }
    //利用短信猫进行验证
    // if(strcasecmp(utComGetVar_sd(psShmHead,"StartSmsModen","No"),"Yes")==0) {
    //    ncUtlSetDoProcess(psShmHead,"SmsModen",0,2,ncSmsSendMesg,"No",7100L,0);
    //  }

    //    if(strcasecmp(utComGetVar_sd(psShmHead,"StartFSSSend","No"),"No")!=0) {
    //        ncUtlSetDoProcess(psShmHead,"GenDbLog",0,20,ncUtlDoOutDbLog,"Yes",71000L,0);
    //        ncUtlSetDoProcess(psShmHead,"FSSSend",0,21, ncClientSendLogToDataServer,"Yes",71000L,0);
    //    }

    //   pName = utComGetVar_sd(psShmHead,"AnyUrlCapHost","no");
    //   pName1 = utComGetVar_sd(psShmHead,"AnyUrlCapStatus","no");
    //   if(strcasecmp(pName,"Yes")==0 || strcasecmp(pName1,"Yes")==0) {
    //        ncUtlSetDoProcess(psShmHead,"CheckUrlCap",0,21, ncCapUrlCheckAndSave,"Yes",71000L,0);
    //        ncUtlSetDoProcess(psShmHead,"TestHostTime",0,21, ncCapUrlAutoTest,"Yes",71000L,0);

    //   }
    //    if(strcasecmp(utComGetVar_sd(psShmHead,"SmtpProxy","No"),"Pronetway")==0) {
    //        ncUtlSetDoProcess(psShmHead,"SMTPProxy",0,21, ncStartSmtpProxy0,"No",71000L,0);
    //    }
    //    if(strcasecmp(utComGetVar_sd(psShmHead,"DataBaseMon","No"),"Yes")==0) {
    //        ncUtlSetDoProcess(psShmHead,"DbMonCheck",0,15, ncDbmonCheckTempFile,"Yes",71000L,0);
    //    }
    //    ncUtlSetDoProcess(psShmHead,"ChkNetDev",0,30, ncUtlCheckDevFlow,"No",71000L,0);

    //    ncUtlSetDoProcess(psShmHead,"Load2Db",0,15, ncUtlStartLoad2Db,"No",71000L,0);

    //    ncUtlSetDoProcess(psShmHead,"PoolApMac",0,15, ncApPoolApMacAndSendSms,"No",71000L,0);

    return 0;
}


/* 检查进程是否需要启动   */
int ncUtlProcessIsSet(utShmHead *psShmHead, char *pName, char *pDef)
{
    char caName[48];
    sprintf(caName, "Start%s", pName);
    if(strcasecmp(utComGetVar_sd(psShmHead, caName, pDef), "Yes") == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}





/* 进程管理程序   */
int ncUtlInitProcess(utShmHead *psShmHead, int iMaxFun)
{
    ncProcessInfo *psFun;
    ncProcessHead *psFunHead;
    int iReturn;
    utShmFreeArray(psShmHead, NC_LNK_PROCESS);
    iReturn = utShmInitArray(psShmHead, NC_LNK_PROCESS, sizeof(ncProcessInfo), iMaxFun);
    if(iReturn < 0)
    {
        return (-1);
    }
    psFun = (ncProcessInfo *)utShmArray(psShmHead, NC_LNK_PROCESS);
    if(psFun == NULL)
    {
        return (-1);
    }
    memset(psFun, 0, iMaxFun * sizeof(ncProcessInfo));
    psFun[0].iFlags = 1;
    strncpy(psFun[0].caName, "Manager", 31);
    psFunHead = (ncProcessHead *)psFun;
    psFunHead->iPid = getpid();
    psFunHead->lStartTime = time(0);
    psFunHead->iDo = 0;
    return 0;
}


/* 设置进程   */


int ncUtlSetProcessName(utShmHead *psShmHead,
                        char *pName, int (*fFunName)(utShmHead *), char *pDef, unsigned long lStepTime, unsigned long lOnTime)
{
    int     iSum, i;
    ncProcessInfo *psFun;
    char            caOn[32];
    unsigned long   lTime;
    char *p;
    sprintf(caOn, "Start%s", pName);
    p = utComGetVar_sd(psShmHead, caOn, pDef);
    if(strcasecmp(p, "No") == 0)
    {
        return 0;
    }
    sprintf(caOn, "Reset%s", pName);
    lTime = utComGetVar_ld(psShmHead, caOn, 0);
    if(lTime > 0)
    {
        lStepTime = lTime;
    }
    sprintf(caOn, "OnSet%s", pName);
    lTime = utComGetVar_ld(psShmHead, caOn, 0);
    if(lTime > 0)
    {
        lOnTime = lTime;
    }


    psFun = (ncProcessInfo *)utShmArray(psShmHead, NC_LNK_PROCESS);
    if(psFun == NULL)
    {
        ncSysLog(NC_LOG_ERROR, " Memory not init ");
        return (-1);
    }
    iSum = utShmArrayRecord(psShmHead, NC_LNK_PROCESS);
    for(i = 1; i < iSum; i++)
    {
        if(strcasecmp(psFun[i].caName, pName) == 0)
        {
            if(psFun[i].iPid > 0)
            {
                kill(psFun[i].iPid, 9);
            }
            psFun[i].iFlags = 1;
            psFun[i].lOntime = lOnTime;
            psFun[i].fFunName = fFunName;
            strncpy(psFun[i].caName, pName, 31);
            psFun[i].nPrio = 0;
            psFun[i].nNum =  1;
            psFun[i].lTimeOut = lStepTime;
            return i;
        }
    }
    for(i = 1; i < iSum; i++)
    {
        if(psFun[i].iFlags == 0 && utStrIsSpaces(psFun[i].caName))
        {
            if(psFun[i].iPid > 0)
            {
                kill(psFun[i].iPid, 9);
            }
            psFun[i].iFlags = 1;
            psFun[i].lOntime = lOnTime;
            psFun[i].fFunName = fFunName;
            strncpy(psFun[i].caName, pName, 31);
            psFun[i].lTimeOut = lStepTime;
            psFun[i].nPrio = 0;
            psFun[i].nNum =  1;
            return i;
        }
    }
    return (-1);
}


/* 停止一个进程  */
int ncUtlStopOneProcess(utShmHead *psShmHead, char *pName)
{
    int iSum, i;
    ncProcessInfo *psFun;
    psFun = (ncProcessInfo *)utShmArray(psShmHead, NC_LNK_PROCESS);
    if(psFun == NULL)
    {
        pasLog1(PAS_LOG_EVENT, " NC_LNK_PROCESS not define \n");
        return (-1);
    }
    iSum = utShmArrayRecord(psShmHead, NC_LNK_PROCESS);
    for(i = 1; i < iSum; i++)
    {
        if(strncasecmp(psFun[i].caName, pName, strlen(pName)) == 0)
        {
            if(psFun[i].iPid > 0)
            {
                psFun[i].iFlags = NC_PROC_STOP;
            }
        }
    }
    return 0;
}

int ncUtlStartOneProcess(utShmHead *psShmHead, char *pName)
{
    int iSum, i;
    ncProcessInfo *psFun;
    psFun = (ncProcessInfo *)utShmArray(psShmHead, NC_LNK_PROCESS);
    if(psFun == NULL)
    {
        printf(" Process Error \n");
        return (-1);
    }
    iSum = utShmArrayRecord(psShmHead, NC_LNK_PROCESS);
    for(i = 1; i < iSum; i++)
    {
        if(strncasecmp(psFun[i].caName, pName, strlen(pName)) == 0)
        {
            psFun[i].iFlags = NC_PROC_RESET;
        }
    }
    return 0;
}


int ncUtlStopProcess(utShmHead *psShmHead)
{
    int iSum, i;
    ncProcessInfo *psFun;
    psFun = (ncProcessInfo *)utShmArray(psShmHead, NC_LNK_PROCESS);
    if(psFun == NULL)
    {
        printf(" Process Error \n");
        return (-1);
    }
    iSum = utShmArrayRecord(psShmHead, NC_LNK_PROCESS);
    for(i = 1; i < iSum; i++)
    {
        if(psFun[i].iPid > 0)
        {
            kill(psFun[i].iPid, 9);
            psFun[i].iPid = 0;
        }
    }
    return 0;
}


/* 启动定义的进程

 */
int ncUtlStartProcess(utShmHead *psShmHead)
{
    ncProcessHead *psFunHead;
    struct tm    *tm;
    unsigned long lThisTime, lThisDayTime, lThisDay, lStepTime;
    unsigned long lLastLicenseTime = 0;
    int iPid, iPid1, iRet, iFlags;
    int iReturn = 0;
    int iError = 0;
    int iSum, i, j, iCount, iStatus, k = 0;
    ncProcessInfo *psFun;
    unsigned long lTime = 0;
    iPid = fork();
    if(iPid < 0)
    {
        pasLog1(PAS_LOG_ERROR, " Start Main Process  Error");
        return (-1);
    }
    else if(iPid > 0)
    {
        return iPid;
    }
    lTime = time(0);

    while(1)
    {
        psFun = (ncProcessInfo *)utShmArray(psShmHead, NC_LNK_PROCESS);
        if(psFun == NULL)
        {
            pasLog1(PAS_LOG_EVENT, "[nc]Process Memory not init ");
            return (-1);
        }
        psFun[0].iPid = getpid();
        psFunHead = (ncProcessHead *)psFun;
        iSum = utShmArrayRecord(psShmHead, NC_LNK_PROCESS);
        if(psFunHead->iDo == NC_PROC_RESET)
        {
            iRet = 0;
            printf("Start ncUtlStartProcess --------- Pid:%d psFunHead->iDo=%d %d\n",
                   getpid(), psFunHead->iDo, NC_PROC_RESET);
            pasLog4(PAS_LOG_EVENT, "Start ncUtlStartProcess --------- Pid:%d psFunHead->iDo=%d %d\n",
                    getpid(),
                    psFunHead->iDo,
                    NC_PROC_RESET);
            psFunHead->iDo = 0;
            iFlags = psFunHead->iFlags;
            if(iFlags == 1)    /* 保留在线信息  */
            {
                pasLog1(PAS_LOG_EVENT, "Reset System ncProcBackUpOnlineInfo ");
                iRet = ncProcBackUpOnlineInfo(psShmHead);
                pasLog2(PAS_LOG_EVENT, "Reset System ncProcBackUpOnlineInfo OK, iRet=%d", iRet);
            }
            pasLog1(PAS_LOG_EVENT, "Stop All Process\n");
            for(i = 1; i < iSum; i++)
            {
                if(psFun[i].iFlags == 1 && psFun[i].iPid > 0)
                {
                    kill(psFun[i].iPid, 9);
                    psFun[i].iPid = 0;
                }
            }
            sleep(1);
            iPid1 = 1;
            while(iPid1 > 0)
            {
                iPid1 = waitpid(-1, &iStatus, WNOHANG);
                if(iPid1 > 0)
                {
                    iStatus = WEXITSTATUS(iStatus);
                    pasLog3(PAS_LOG_EVENT, "[waitpid]Pid %d Exit Status is %d\n", iPid1, iStatus);
                }
            }
            /* 初始化   */
            iReturn = pasConnect(psShmHead); /* 连接数据库  */
            if(iReturn != 0)
            {
                pasLog1(PAS_LOG_ERROR, "DataBase Connect Error\n");
                return (-1);
            }
            else
            {
                pasLog1(PAS_LOG_EVENT, "DataBase Connect Sucess\n");
            }
            ncInitProeimMem(psShmHead, 0);
            if((iRet == 1 && iFlags == 1) || iFlags == 2)
            {
                pasLog1(PAS_LOG_EVENT, "Recover Online Info\n");
                ncProcRecoverOnlineInfo(psShmHead);
            }
            else
            {
                pasLog3(PAS_LOG_EVENT, "Not Recover Online Info iRet=%d  iFlags=%d\n", iRet, iFlags);
            }
            pasDbClose(NULL);

            ncStartSetAllProcess(psShmHead);
            psFun = (ncProcessInfo *)utShmArray(psShmHead, NC_LNK_PROCESS);
            if(psFun == NULL)
            {
                pasLog1(PAS_LOG_EVENT, "[nc]Process Memory not init ");
                return (-1);
            }
            psFunHead = (ncProcessHead *)psFun;
            iSum = utShmArrayRecord(psShmHead, NC_LNK_PROCESS);
            for(i = 1; i < iSum; i++)
            {
                if(psFun[i].iFlags == 1 || psFun[i].iFlags == 2)
                {
                    ncProcStartThisProcess(psShmHead, &psFun[i]);
                }
            }
            /*
            if(pasGetMyPid("../log/pastcppid.pid") > 0) {
                pasLog2(PAS_LOG_EVENT," Restart Tcp Service.....pasTcpIsStart:%d \n",pasTcpIsStart());
                ncRestartTcpServer(psShmHead);
            }
            */
            psFunHead->iFlags = 0;
            psFunHead->lLastTime = time(0);
        }
        else if(psFunHead->iDo == NC_PROC_STOP)   /* 停止所有进程  */
        {
            pasLog1(PAS_LOG_EVENT, "Stop All Process\n");
            psFunHead->iDo = 0;
            if(psFunHead->iFlags == 1)
            {
                iRet = ncProcBackUpOnlineInfo(psShmHead);
            }
            for(i = 1; i < iSum; i++)
            {
                if(psFun[i].iFlags == 1 && psFun[i].iPid > 0)
                {
                    kill(psFun[i].iPid, 9);
                    psFun[i].iPid = 0;
                    psFun[i].iFlags == 2;
                }
            }
            iPid1 = 1;
            while(iPid1 > 0)
            {
                iPid1 = waitpid(-1, &iStatus, WNOHANG);
                if(iPid1 > 0)
                {
                    iStatus = WEXITSTATUS(iStatus);
                    pasLog3(PAS_LOG_EVENT, "[nc]Pid %d Exit Status is %d\n", iPid1, iStatus);
                }
            }
            psFunHead->iFlags = 1;
            psFunHead->lLastTime = time(0);
        }
        else
        {
            lThisTime = time(0);
            tm = localtime(&lThisTime);
            lThisDayTime = tm->tm_hour * 10000L + tm->tm_min * 100 + tm->tm_sec;
            lThisDay = (1900 + tm->tm_year) * 10000 + (tm->tm_mon + 1) * 100 + tm->tm_mday;

            for(i = 1; i < iSum; i++)
            {
                if(psFun[i].iFlags == NC_PROC_RESET)    /* 重置当前进程   */
                {
                    if(psFun[i].iPid > 0)
                    {
                        kill(psFun[i].iPid, 9);
                        psFun[i].iPid = 0;
                    }
                    pasLog2(PAS_LOG_EVENT, "NC_PROC_RESET,Start %s  ", psFun[i].caName);
                    ncProcStartThisProcess(psShmHead, &psFun[i]);
                }
                else if(psFun[i].iFlags == NC_PROC_STOP)    /* 重置当前进程   */
                {
                    if(psFun[i].iPid > 0)
                    {
                        kill(psFun[i].iPid, 9);
                        psFun[i].iPid = 0;
                        psFun[i].iFlags = 2;

                    }
                    pasLog2(PAS_LOG_EVENT, "NC_PROC_STOP,Stop %s  ", psFun[i].caName);
                }
                else if(psFun[i].iFlags == 1 && psFun[i].iPid == 0)
                {
                    pasLog2(PAS_LOG_EVENT, "NC_PROC_START,Start %s  ", psFun[i].caName);
                    ncProcStartThisProcess(psShmHead, &psFun[i]);
                }
                else   /* 检查进程状态/超时等   */
                {
                    if(psFun[i].lOntime > 0)      /* 定时  */
                    {
                        unsigned long lThisStartTime = ncUtlGetThatDays(psFun[i].lStartTime);
                        if(lThisStartTime  <  ncUtlGetThatDays(lThisTime) &&
                           lThisDayTime >= psFun[i].lOntime && psFun[i].lStartTime > 0)
                        {
                            pasLog2(PAS_LOG_EVENT, "Process %s is Ontime, Reset by system ", psFun[i].caName);
                            if(psFun[i].iPid > 0) kill(psFun[i].iPid, 9);
                            psFun[i].lStartTime = lThisTime;
                        }
                    }
                    else
                    {
                        if(lThisTime - psFun[i].lStartTime > psFun[i].lTimeOut
                           && psFun[i].lTimeOut > 0 && psFun[i].lStartTime > 0)
                        {
                            pasLog2(PAS_LOG_EVENT, "Process %s  is Time out, Reset by system ", psFun[i].caName);
                            if(psFun[i].iPid > 0) kill(psFun[i].iPid, 9);
                            psFun[i].lStartTime = lThisTime;
                        }
                    }
                }
            }
            psFunHead->lLastTime = time(0);
        }
        iPid1 = 1;
        while(iPid1 > 0)
        {
            iPid1 = waitpid(-1, &iStatus, WNOHANG);
            if(iPid1 > 0)
            {
                if(WIFEXITED(iStatus))
                {
                    iStatus = WEXITSTATUS(iStatus);
                    pasLog3(PAS_LOG_EVENT, "[nc]Pid %d Exit Status is %d\n", iPid1, iStatus);
                }
                else if(WIFSIGNALED(iStatus))   /* 异常退出  */
                {
                    iStatus = WTERMSIG(iStatus);
                    pasLog3(PAS_LOG_ERROR, "[nc]Pid %d Exit by term signal %d\n", iPid1, iStatus);
                    iError ++;
                    if(time(0) - lTime > 600)
                    {
                        lTime = time(0);
                        iError = 0;
                    }
                    if(iError > 300)
                    {
                        pasLog1(PAS_LOG_ERROR, "[nc001_3]Error is too mach,system should restart");
                        ncSysRestartNc("Process Error, Rsetart Nc System");
                        iError = 0;
                    }
                }
                else if(WIFSTOPPED(iStatus))
                {
                    iStatus = WSTOPSIG(iStatus);
                    pasLog3(PAS_LOG_ERROR, "[nc]Pid %d Exit by stop signal %d\n", iPid1, iStatus);
                }
                else
                {
                    pasLog3(PAS_LOG_ERROR, "[nc]Pid %d Exit by abnormally %d\n", iPid1, iStatus);
                }
                if(iStatus != SIGUSR1 && iStatus != SIGALRM)   /* 出错  */
                {
                    for(i = 1; i < iSum; i++)
                    {
                        if(psFun[i].iPid == iPid1)
                        {
                            pasLog3(PAS_LOG_ERROR, "Stop Process %s  Pid==%lu \n", psFun[i].caName, psFun[i].iPid);
                            psFun[i].iPid = 0;
                            k = i;
                            if(iPid1 > 0) kill(iPid1, 9);
                        }
                    }
                }
            }
            if(lLastLicenseTime < time(0) - 1800)    /* 每隔半个小时检查一次 License  */
            {
                if(ncProcCheckSoftLicense(psShmHead) != 1)
                {
                    pasLog1(PAS_LOG_ERROR, "License Error\n");
                    ncUtlStopAllNcProcess(psShmHead);
                }
                lLastLicenseTime = time(0);
            }
        }
        sleep(5);
    }
}


/* 启动当前进程  */
int ncProcStartThisProcess(utShmHead *psShmHead, ncProcessInfo *psFun)
{
    int iPid2;
    int iReturn;
    iPid2 = fork();
    if(iPid2 == 0)
    {
        pasLog3(PAS_LOG_EVENT, "[nc]Start Process %s  Pid==%lu ", psFun->caName, getpid());
        printf(" Start Process [%s]  Pid==%lu \n", psFun->caName, getpid());
        if(strcasecmp(utComGetVar_sd(psShmHead, "ProcessDebug", "No"), "Yes") == 0)
        {
            printf(" Waiting %s 20s  Pid is %d \n", psFun->caName, getpid());
            sleep(20);
        }
        if(psFun->nNum > 0)
        {
            sleep(psFun->nNum);
        }
        iReturn = psFun->fFunName(psShmHead);
        if(iReturn < 0)
        {
            printf(" Start Process [%s] Error \n", psFun->caName);
            pasLog(PAS_LOG_EVENT, "[nc]Start Process %s  Error %d ", psFun->caName, iReturn);
        }
        exit(0);
    }
    else if(iPid2 > 0)
    {
        psFun->iPid = iPid2;
        psFun->iFlags = 1;
        psFun->lStartTime = time(0);
        return iPid2;
    }
    else
    {
        return (-1);
    }
}


/* 检查License 信息
    = 1 --正确
   != 1 --不正确
   */

int ncProcCheckSoftLicense(utShmHead *psShmHead)
{
    int iPid, iReturn;
    char *prodSn;
    int iCount = 0;
    prodSn = utComGetVar_s(psShmHead, "ProductSN");
    while(prodSn == NULL && iCount < 30)
    {
        pasLog(PAS_LOG_ERROR, "Get Prodsn Count is %d\n", iCount);
        iCount++;
        prodSn = utComGetVar_s(psShmHead, "ProductSN");
        sleep(1);
    }
    if(prodSn == NULL)
    {
        pasLog(PAS_LOG_ERROR, "Invalid Software License \n");
        return (-1);
    }
    else
    {
        iReturn = pasInstCheck(prodSn);
        if(iReturn != 1)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}






/* 重新启动各种进程  */
int ncUtlRestartProcess(utShmHead *psShmHead, int iFlags)
{
    ncProcessHead *psFunHead = (ncProcessHead *)utShmArray(psShmHead, NC_LNK_PROCESS);;
    if(psFunHead == NULL)
    {
        pasLog1(PAS_LOG_ERROR, "[ncUtlRestartProcess]psFunHead=NULL");
        return (-1);
    }
    if(psFunHead->iDo == NC_PROC_RESET)
    {
        psFunHead->iFlags = iFlags;
        pasLog2(PAS_LOG_ERROR, "[ncUtlRestartProcess]Already iFlags=%d", iFlags);
    }
    else
    {
        long lTime;
        lTime = utComGetVar_ld(psShmHead, "ResetTimeOut", 1);
        pasLog2(PAS_LOG_EVENT, "[ncUtlRestartProcess]iFlags=%d", iFlags);
        psFunHead->iDo = NC_PROC_RESET;
        psFunHead->iFlags = iFlags;
        if(lTime > 0)
        {
            sleep(lTime);   /* 延迟5秒  */
        }
    }
    return 0;
}

/* 停止各种进程的运行   */
int ncUtlStopAllNcProcess(utShmHead *psShmHead, int iFlags)
{
    ncProcessHead *psFunHead = (ncProcessHead *)utShmArray(psShmHead, NC_LNK_PROCESS);
    if(psFunHead == NULL)
    {
        return (-1);
    }
    psFunHead->iDo = NC_PROC_STOP;
    psFunHead->iFlags = iFlags;
    return 0;
}


int ncUtlDMailSendStart(utShmHead *psShmHead)
{
    if(strcasecmp(utComGetVar_sd(psShmHead, "MailDebug", "no"), "Yes") == 0)
    {
        pasMailDebug(1);
    }
    if(strcasecmp(utComGetVar_sd(psShmHead, "MailSmtpAuth", "no"), "Yes") == 0)
    {
        char *pUser, *pPass, *pServer;
        pUser = utComGetVar_s(psShmHead, "MailAccount");
        pPass = utComGetVar_s(psShmHead, "MailPassword");
        pServer = utComGetVar_s(psShmHead, "MailServer");
        if(pUser && pPass && pServer)
        {
            pasMailSetSmtpAuth(pUser, pPass, pServer);
        }
        if(strcasecmp(utComGetVar_sd(psShmHead, "MailESmtp", "Yes"), "No") == 0)
        {
            pasMailSetESmtpAuth(0);
        }
    }

    return pasMailSendStart0();
}



/* 设置抓包程序  */
int ncUtlSetPkgCapProcess(utShmHead *psShmHead)
{
    ncPkgCapDevHead *psDevHead;
    char caName[32];
    char *p;
    unsigned long lTime;
    int i;
    psDevHead = ncPkgGetCapDevHead();
    p = utComGetVar_sd(psShmHead, "IpControlBy", "meta");
    if(strcasecmp(p, "nsa5") == 0 || strcasecmp(p, "nsa6") == 0 || strcasecmp(p, "dna") == 0)
    {
        return 0;
    }
    lTime = utComGetVar_ld(psShmHead, "ResetCap", 6000);
    for(i = 0; i < psDevHead->iSumDev; i++)
    {
        sprintf(caName, "Cap-%s", psDevHead->psDev[i].caName);
        ncUtlSetDoProcess(psShmHead, caName, 0, 1, ncPkgStartGet, "Yes", lTime, 0);
    }
    return 0;
}


/* 重起抓报程序  */
int ncUtlRestartCapProcess(utShmHead *psShmHead)
{
    pasLog(PAS_LOG_EVENT, " Restart Cap Process \n");
    printf("Restart Cap Process  \n");
    ncUtlStopOneProcess(psShmHead, "cap-");
    ncUtlStartOneProcess(psShmHead, "cap-");
    return 0;
}







/* 取控制信号  */
ncProcessInfo *ncUtlGetSignal(utShmHead *psShmHead)
{
    int iPid;
    int iSum, i;
    ncProcessInfo *psFun;
    iPid = getpid();
    psFun = (ncProcessInfo *)utShmArray(psShmHead, NC_LNK_PROCESS);
    if(psFun == NULL)
    {
        return NULL;
    }
    iSum = utShmArrayRecord(psShmHead, NC_LNK_PROCESS);
    for(i = 1; i < iSum; i++)
    {
        if(psFun[i].iPid == iPid)
        {
            return &psFun[i];
        }
    }
    return NULL;
}


int ncUtlSetMyProcLastTime(utShmHead *psShmHead, unsigned long lTime)
{
    ncProcessInfo *psFun;
    psFun = (ncProcessInfo *)ncUtlGetSignal(psShmHead);
    if(psFun)
    {
        psFun->lLastTime = lTime;
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned long ncUtlGetMyProcLastTime(utShmHead *psShmHead)
{
    ncProcessInfo *psFun;
    psFun = (ncProcessInfo *)ncUtlGetSignal(psShmHead);
    if(psFun)
    {
        return psFun->lLastTime;
    }
    else
    {
        return 0;
    }
}



/* 取本人的进程状态  */
int ncUtlGetMyStatus(ncProcessInfo *psFun)
{
    struct rusage u;
    unsigned long lTime;
    getrusage(RUSAGE_SELF, &u);
    psFun->lRunSec =  u.ru_utime.tv_sec;
    psFun->lRunUsec = u.ru_utime.tv_usec;
    psFun->lSysSec =  u.ru_stime.tv_sec;
    psFun->lSysUsec = u.ru_stime.tv_usec;
    psFun->lGetTime = time(0);
    return 0;
}


/* 设置进程启动状态
   替代  ncUtlSetProcessName
 */
int ncUtlSetDoProcess(utShmHead *psShmHead,
                      char *pName,                          // 进程名
                      int  iPrio,                           // 进程执行优先级
                      int  iNum,                            // 进程执行顺序号
                      int (*fFunName)(utShmHead *),         // 执行函数
                      char *pDef,                           // 缺省是否启动
                      unsigned long lStepTime,              // 重新启动时间
                      unsigned long lOnTime)                // 固定启动时间
{
    int     iSum, i;
    ncProcessInfo *psFun;
    char            caOn[32];
    unsigned long   lTime;
    char *p;
    sprintf(caOn, "Start%s", pName);
    p = utComGetVar_sd(psShmHead, caOn, pDef);
    if(strcasecmp(p, "No") == 0)
    {
        return 0;
    }
    sprintf(caOn, "Reset%s", pName);
    lStepTime = utComGetVar_ld(psShmHead, caOn, lStepTime);
    sprintf(caOn, "OnSet%s", pName);
    lOnTime = utComGetVar_ld(psShmHead, caOn, lOnTime);
    sprintf(caOn, "Prio%s", pName);
    iPrio = utComGetVar_ld(psShmHead, caOn, iPrio);

    psFun = (ncProcessInfo *)utShmArray(psShmHead, NC_LNK_PROCESS);
    if(psFun == NULL)
    {
        pasLog(PAS_LOG_ERROR, " Memory not init ");
        return (-1);
    }
    iSum = utShmArrayRecord(psShmHead, NC_LNK_PROCESS);
    for(i = 1; i < iSum; i++)
    {
        if(strcasecmp(psFun[i].caName, pName) == 0)
        {
            if(psFun[i].iPid > 0)
            {
                kill(psFun[i].iPid, 9);
            }
            psFun[i].iFlags = 1;
            psFun[i].lOntime = lOnTime;
            psFun[i].fFunName = fFunName;
            strncpy(psFun[i].caName, pName, 31);
            psFun[i].lTimeOut = lStepTime;
            psFun[i].nPrio = iPrio;
            psFun[i].nNum =  iNum;
            return i;
        }
    }
    for(i = 0; i < iSum; i++)
    {
        if(psFun[i].iFlags == 0)
        {
            if(psFun[i].iPid > 0)
            {
                kill(psFun[i].iPid, 9);
            }
            psFun[i].iFlags = 1;
            psFun[i].lOntime = lOnTime;
            psFun[i].fFunName = fFunName;
            strncpy(psFun[i].caName, pName, 31);
            psFun[i].lTimeOut = lStepTime;
            psFun[i].nPrio = iPrio;
            psFun[i].nNum =  iNum;
            return i;
        }
    }
    return (-1);
}


char *ncUtlGetMyProcessName(utShmHead *psShmHead)
{
    int iPid;
    int i;
    ncProcessInfo *psProcess;
    iPid = getpid();
    psProcess = (ncProcessInfo *)utShmArray(psShmHead, NC_LNK_PROCESS);
    if(psProcess == NULL)
    {
        return NULL;
    }
    for(i = 0; i < utShmArrayRecord(psShmHead, NC_LNK_PROCESS); i++)
    {
        if(psProcess[i].iPid == iPid)
        {
            return psProcess[i].caName;
        }
    }
    return NULL;
}
