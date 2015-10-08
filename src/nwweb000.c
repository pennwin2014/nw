#include <stdio.h>
#include "utoall.h"
#include "ncdef.h"
#include "nwdef.h"
/* 初试化内网Web操作函数  */
int nwWebShowOnlineCompute(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int  nwWebCompInfo_menu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompInfo_Base(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompInfo_HardInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompInfo_HardChange(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompInfo_SoftInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompInfo_SoftChange(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebTreeDispGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebTreeDispComp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwTcpScreenInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwTcpUpdaeDesk(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nwweb003.c */
int  nwWebCompMon_menu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompMon(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompMon_Netlog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompMon_CurProc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompMon_SysInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompMon_Screen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompMon_ShowScreen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompMon_Proclog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int  nwWebCompOn_menu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompMon_Cnflog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebScreenShowFrm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int nwWebLimProcList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSoftClassEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSoftClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebLim_Procy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int nwWebCompMon_Stop(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebCompMon_Uninstall(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncClientImList(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebUpdateDeskSoft(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUpdateDeskSoftSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeskSoft_upload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowDeskSoftUpdateLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int nwTcpScreenLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncInitNwWebFun(utShmHead *psShmHead)
{
    int iReturn;
    iReturn = pasSetTcpFunName("nwWebShowOnlineCompute",  nwWebShowOnlineCompute, 0);
    iReturn = pasSetTcpFunName("nwWebCompInfo_HardInfo",  nwWebCompInfo_HardInfo, 0);
    iReturn = pasSetTcpFunName("nwWebCompInfo_HardChange",  nwWebCompInfo_HardChange, 0);
    iReturn = pasSetTcpFunName("nwWebCompInfo_SoftInfo",  nwWebCompInfo_SoftInfo, 0);
    iReturn = pasSetTcpFunName("nwWebCompInfo_SoftChange",  nwWebCompInfo_SoftChange, 0);
    iReturn = pasSetTcpFunName("nwWebCompInfo_Base",  nwWebCompInfo_Base, 0);
    iReturn = pasSetTcpFunName("nwWebCompInfo_menu",  nwWebCompInfo_menu, 0);
    iReturn = pasSetTcpFunName("nwWebCompInfo",  nwWebCompInfo, 0);
    iReturn = pasSetTcpFunName("nwWebCompTree",  nwWebCompTree, 0);
    iReturn = pasSetTcpFunName("nwWebTreeDispGroup",  nwWebTreeDispGroup, 0);
    iReturn = pasSetTcpFunName("nwWebTreeDispComp",  nwWebTreeDispComp, 0);
    iReturn = pasSetTcpFunCode(NC_UDP_CAPSCREEN,  nwTcpScreenInfo, 0);
    iReturn = pasSetTcpFunCode(NC_NWUPDATEREADY_REQ,  nwTcpUpdaeDesk, 0);
    
     iReturn = pasSetTcpFunCode(NC_UDP_CAPSCREENTCP,  nwTcpScreenLog, 0);
    /* nwweb003.c */
    iReturn = pasSetTcpFunName("nwWebCompMon_menu",  nwWebCompMon_menu, 0);
    iReturn = pasSetTcpFunName("nwWebCompMon",  nwWebCompMon, 0);
    iReturn = pasSetTcpFunName("nwWebCompMon_Netlog",  nwWebCompMon_Netlog, 0);
    iReturn = pasSetTcpFunName("nwWebCompMon_CurProc",  nwWebCompMon_CurProc, 0);
    iReturn = pasSetTcpFunName("nwWebCompMon_SysInfo",  nwWebCompMon_SysInfo, 0);
    iReturn = pasSetTcpFunName("nwWebCompMon_Screen",  nwWebCompMon_Screen, 0);
    iReturn = pasSetTcpFunName("nwWebCompMon_ShowScreen",  nwWebCompMon_ShowScreen, 0);
    iReturn = pasSetTcpFunName("nwWebCompMon_Proclog",  nwWebCompMon_Proclog, 0);
    iReturn = pasSetTcpFunName("nwWebCompOn_menu",  nwWebCompOn_menu, 0);
    iReturn = pasSetTcpFunName("nwWebCompMon_Cnflog",  nwWebCompMon_Cnflog, 0);
    iReturn = pasSetTcpFunName("nwWebScreenShowFrm", nwWebScreenShowFrm, 0);
    
    //nw_newweb001.c
    iReturn = pasSetTcpFunName("nwWebLimProcList",  nwWebLimProcList, 0);
    iReturn = pasSetTcpFunName("ncSoftClassEdit",  ncSoftClassEdit, 0);
    iReturn = pasSetTcpFunName("ncSoftClass", ncSoftClass, 0);
    iReturn = pasSetTcpFunName("nwWebLim_Procy",nwWebLim_Procy,0);
    
    iReturn = pasSetTcpFunName("nwWebCompMon_Stop",nwWebCompMon_Stop,0);
    iReturn = pasSetTcpFunName("nwWebCompMon_Uninstall",nwWebCompMon_Uninstall,0);
    iReturn = pasSetTcpFunName("ncClientImList",ncClientImList,0);
    iReturn = pasSetTcpFunName("ncWebUpdateDeskSoft",ncWebUpdateDeskSoft,0);
    
    iReturn = pasSetTcpFunName("ncWebUpdateDeskSoftSave",ncWebUpdateDeskSoftSave,0);
    iReturn=pasSetTcpFunName("ncWebDeskSoft_upload",ncWebDeskSoft_upload,0);
    iReturn=pasSetTcpFunName("ncShowDeskSoftUpdateLog",ncShowDeskSoftUpdateLog,0);

    ncInitNwWebFun_new(psShmHead);
    ncInitNwWebFun_UdpBeat(psShmHead); //心跳
    ncInitNwWebFun_FileOP(psShmHead);  //文件操作
    ncInitNwWebFun_Uplog(psShmHead);  //日志上传
    ncInitNwWebFun_TcpComminute(psShmHead); //Tcp通讯
    ncInitNwWebFun_UpComputerInfo(psShmHead); //计算机的各种信息
    ncInitNwWebFun_OnlineInfo(psShmHead); //实时信息

    ncInitNwWebFun_WindowsConsole(psShmHead);   //windows控制台
    ncInitNwWebFun_SoftwareDistribution(psShmHead);  //软件分发
    ncInitNwWebFun_AuditFile(psShmHead);		    //文件审计
    ncInitNwWebFun_BackUpFile(psShmHead);	//外发文件备份
    
   ncInitNwWebPHTestFunc(psShmHead);		//web Test ByPh
   ncInitNwWebPHOutFileTaskFunc(psShmHead);		//web Test ByPh
    return 0;
}
