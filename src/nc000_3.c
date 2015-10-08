#include <stdio.h>
#include "utoall.h"
#include "ncdef.h"
#include "ncrtl.h"
/* 初试化Web操作函数  */
int ncWebTestShowUserFee(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int  ncWebDispHtmlFid(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebTest(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestDownLoad(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPronetwaylog12345(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestHostPort(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncsWbsMsgAdd(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncsWebMsgAdd(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncsDispWbsMesg(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebLimWebClassNew(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowMainTitle(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetFeeProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebNextFeeProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBilFeeLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncProxCheckLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebModSomeUserFobflag(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBilFeeStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncGetHotelUserInfo_nt(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead);
int ncWebShowUseronlineip(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncGetHotelUserInfo_ntout(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead);
int ncWeDomainChangePasswd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSynDomain(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysDepUserByDomain(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysDomainOver(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSquidAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSquidNcAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int S(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead);
int ncWebBBSLog(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead);
int ncWebShowMobZc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncYyzBas_Save2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncYyzBase2(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_base_new(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);

int ncbrgsave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncDelBrg(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSave_base_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncDispReportWizard(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncReportWizardSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncReportPlateList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncReportHis(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowMoreWebSiteIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowHisReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispCmpReportDef(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncReportPlateList_comp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowCompReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncOnServiceStat_User(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowSelfWeb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispPorcylist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebGetMobPass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebGetMobPass_Cy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowHelpUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsReqAuthCenter(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncHelpUserLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebHelpUserList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncIPdepSet2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispLoginMob_hotel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBBSKeyrulelist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncProtocolDefine(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncDispGroupRzfs(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncGroupRzfsSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
//SYSLOG ip地址设置
int ncWebSyslog_Ip(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSyslog_IpSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_jingdong_reportall(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebServiceSortUserByTime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncReqTcpAuth_nojm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
//迈外迪web接口
int ncMwdLogin_out(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
//设置IP访问白名单
int ncWebSysSet_IPWhite(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSysSave_IPWhite(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncReqTcp_Msg(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSysSave_Zrtip(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSysSet_Zrtip(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebAuth_app(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


struct ncPltMenu_s {
    int    iFlags;
    char   caAllowed[10];
    char   caTitle[32];
    char   caIcon[128];
    char   caUrl[256];
} psMenu[512];
int mItem;      //记录菜单项


int ncPltReadMenu(utShmHead *psShmHead)
{
		char caConfFile[164];
    FILE *fp;
    char caBuffer[512];
    int iCount = 0;
    char *p;
    sprintf(caConfFile,"/home/ncmysql/nc/plate/menu/%s",utComGetVar_sd(psShmHead,"menufile","menu_v7.05.01.txt"));
    
    
    
    memset(psMenu,0,512*sizeof(struct ncPltMenu_s)); 
    fp = fopen(caConfFile,"r");
    if(fp == NULL) {
        printf(" Read File Error %s\n",caConfFile);
        return (-1);
    }
    p = fgets(caBuffer,511,fp);
    while(p) {
        if(!utStrIsSpaces(caBuffer)) {
            p = utStrSkipSpaces(caBuffer);
            switch (*p) {
                case '1':
                    psMenu[iCount].iFlags = 1;
                    p = p + 2;
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caTitle,31,"\n\r\t ");
                    iCount++;
                    break;
                case '2':
                    psMenu[iCount].iFlags = 2;
                    p = p + 2;
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caTitle,31,"\n\r\t ");
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caIcon,127,"\n\r\t ");
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caUrl,255,"\n\r\t ");
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caAllowed,2,"\n\r\t ");
                    iCount++;
                    break;
                case 'H':
                    psMenu[iCount].iFlags = 3;
                    iCount++;
                    break;
                case 'L':
                    psMenu[iCount].iFlags = 4;
                    iCount++;
                    break;
                case '0':
                    psMenu[iCount].iFlags = 0;
                    p = p + 2;
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caTitle,31,"\n\r\t ");
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caIcon,127,"\n\r\t ");
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caUrl,255,"\n\r\t ");
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caAllowed,2,"\n\r\t ");
                    iCount++;
                    break;
                case 'M':
                    psMenu[iCount].iFlags = 5;
                    p = p + 2;
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caTitle,31,"\n\r\t ");
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caIcon,127,"\n\r\t ");
                    p = utStrSkipSpaces(p);
                    p = utStrGetWord(p,psMenu[iCount].caUrl,255,"\n\r\t ");
                    iCount++;
                    break;
            }
        }
        p = fgets(caBuffer,511,fp);
    }
    fclose(fp);
    return iCount;
}


int ncInitWebFun(utShmHead *psShmHead)
{
    int iReturn;
    
    mItem=ncPltReadMenu(psShmHead);
    

    ncInitWebFun_v8();

    iReturn = pasSetTcpFunName("ncWebTestHostPort",ncWebTestHostPort,0);
    iReturn = pasSetTcpFunName("ncIPdepSet",  ncIPdepSet, 0);
    iReturn = pasSetTcpFunName("ncWebDispNetwork",  ncWebDispNetwork, 0);
    iReturn = pasSetTcpFunName("ncWebSetNetwork",  ncWebSetNetwork, 0);
    iReturn = pasSetTcpFunName("ncWebDelSomeUser", ncWebDelSomeUser, 0);
    iReturn = pasSetTcpFunName("ncWebAuth",      ncWebAuth,   0);
    iReturn = pasSetTcpFunName("ncWebModiPass",      ncWebModiPass,   0);
    iReturn = pasSetTcpFunName("ncWebDispModiPass",  ncWebDispModiPass,   0);
    iReturn = pasSetTcpFunName("ncTest2005",  ncTest2005,   0);
    iReturn = pasSetTcpFunName("ncWebListUserTree",  ncWebListUserTree, 0);
    iReturn = pasSetTcpFunName("ncWebListUserTree_w",  ncWebListUserTree_w, 0);
    iReturn = pasSetTcpFunName("ncWebDispUserAddForm",  ncWebDispUserAddForm, 0);
    iReturn = pasSetTcpFunName("ncWebUserAdd",  ncWebUserAdd, 0);
    iReturn = pasSetTcpFunName("ncWebDeptModi",  ncWebDeptModi, 0);
    iReturn = pasSetTcpFunName("ncWebDispDeptForm",  ncWebDispDeptForm, 0);
    iReturn = pasSetTcpFunName("ncWebDeptDel",  ncWebDeptDel, 0);
    iReturn = pasSetTcpFunName("ncWebDeptAdd",  ncWebDeptAdd, 0);
    iReturn = pasSetTcpFunName("ncWebShowUserStatus",  ncWebShowUserStatus, 0);
    iReturn = pasSetTcpFunName("ncWebReplace",  ncWebReplace, 0);
    iReturn = pasSetTcpFunName("ncWebReplace_mon",  ncWebReplace_mon, 0);
    iReturn = pasSetTcpFunName("ncWebQyeIp",    ncWebQyeIp, 0);
    iReturn = pasSetTcpFunName("ncWebCountIp",    ncWebCountIp, 0);
    iReturn = pasSetTcpFunName("ncWebUserSearch", ncWebUserSearch, 0);
    iReturn = pasSetTcpFunName("ncWebFlowCount", ncWebFlowCount, 0);
    iReturn = pasSetTcpFunName("ncWebShowNewvister", ncWebShowNewvister, 0);
    iReturn = pasSetTcpFunName("ncWebShowNewweb", ncWebShowNewweb, 0);
    iReturn = pasSetTcpFunName("ncWebQyehttp", ncWebQyehttp, 0);

    iReturn = pasSetTcpFunName("ncWebSortUserByFlow", ncWebSortUserByFlow, 0);
    iReturn = pasSetTcpFunName("ncWebSortWeb", ncWebSortWeb, 0);
    iReturn = pasSetTcpFunName("ncWebSortIp", ncWebSortIp, 0);
    iReturn = pasSetTcpFunName("ncWebSortService", ncWebSortService, 0);
    iReturn = pasSetTcpFunName("ncWebCount", ncWebCount, 0);
/* nc009.c */
   pasSetTcpFunName("ncWebSortUser",ncWebSortUser,0);
    pasSetTcpFunName("ncWebSortByWeb",ncWebSortByWeb,0);
    pasSetTcpFunName("ncFobSort",ncFobSort,0);
    pasSetTcpFunName("ncFobSortDIP",ncFobSortDIP,0);
    pasSetTcpFunName("ncSortDispForm",ncSortDispForm,0);
    pasSetTcpFunName("ncWebSortAll",ncWebSortAll,0);

    /* iReturn = pasSetTcpFunName("ncMailReadUploadHeads", ncMailReadUploadHeads, 0);
    iReturn = pasSetTcpFunName("ncUploadReadContent", ncUploadReadContent, 0); */
    iReturn = pasSetTcpFunName("ncWebAdminUserList", ncWebAdminUserList, 0);
    iReturn = pasSetTcpFunName("ncWebAdminDispUserForm", ncWebAdminDispUserForm, 0);
    iReturn = pasSetTcpFunName("ncWebAdminRoleList", ncWebAdminRoleList, 0);
    iReturn = pasSetTcpFunName("ncAdminRoleModi", ncAdminRoleModi, 0);
    iReturn = pasSetTcpFunName("ncWebAdminRole", ncWebAdminRole, 0);
    iReturn = pasSetTcpFunName("ncAdminUserSave", ncAdminUserSave, 0);
    /* iReturn = pasSetTcpFunName("ncMailUploadRemove", ncMailUploadRemove, 0); */
    iReturn = pasSetTcpFunName("ncWebCountTotalFlow", ncWebCountTotalFlow, 0);
    iReturn = pasSetTcpFunName("ncWebShutdown", ncWebShutdown, 0);
    iReturn = pasSetTcpFunName("ncWebInternetLogin", ncWebInternetLogin, 0);
    iReturn = pasSetTcpFunName("ncWebInternetLogout", ncWebInternetLogout, 0);
    iReturn = pasSetTcpFunName("ncWebBackList", ncWebBackList, 0);
    iReturn = pasSetTcpFunName("ncWebBackDelete", ncWebBackDelete, 0);
    iReturn = pasSetTcpFunName("ncWebBackDownload", ncWebBackDownload, 0);
    iReturn = pasSetTcpFunName("ncWebInternetModpass", ncWebInternetModpass, 0);
    iReturn = pasSetTcpFunName("ncFlowTimeDispForm", ncFlowTimeDispForm, 0);
    iReturn = pasSetTcpFunName("ncMailShowRule", ncMailShowRule, 0);
    iReturn = pasSetTcpFunName("ncMailDispOneRule", ncMailDispOneRule, 0);
    iReturn = pasSetTcpFunName("ncMailModiOneRule", ncMailModiOneRule, 0);
    iReturn = pasSetTcpFunName("ncMailDelSomeRule", ncMailDelSomeRule, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlList", ncWebMailCtlList, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlRead", ncWebMailCtlRead, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlDownload", ncWebMailCtlDownload, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlOutHtm", ncWebMailCtlOutHtm, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlDel", ncWebMailCtlDel, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlAuditlst", ncWebMailCtlAuditlst, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlAudit", ncWebMailCtlAudit, 0);

    iReturn = pasSetTcpFunName("ncWebListGroupTree", ncWebListGroupTree, 0);
    iReturn = pasSetTcpFunName("ncWebOutFlowChart", ncWebOutFlowChart, 0);
    iReturn = pasSetTcpFunName("ncWebCountIpChart", ncWebCountIpChart, 0);

    iReturn = pasSetTcpFunName("ncWebPrintWebUse", ncWebPrintWebUse, 0);

    iReturn = pasSetTcpFunName("ncMailMonListRule", ncMailMonListRule, 0);
    iReturn = pasSetTcpFunName("ncMailMonDispOne", ncMailMonDispOne, 0);
    iReturn = pasSetTcpFunName("ncMailMonModiOne", ncMailMonModiOne, 0);
    iReturn = pasSetTcpFunName("ncMailMonDelSome", ncMailMonDelSome, 0);

    iReturn = pasSetTcpFunName("ncWebUserExportCsv", ncWebUserExportCsv, 0);
    iReturn = pasSetTcpFunName("ncWebPrintWebUse_u", ncWebPrintWebUse_u, 0);

    iReturn = pasSetTcpFunName("ncUtilUserHSearch", ncUtilUserHSearch, 0);
    iReturn = pasSetTcpFunName("ncWebDispSuser", ncWebDispSuser, 0);


/* nc014.c */
    iReturn = pasSetTcpFunName("ncWebPrintNetUse", ncWebPrintNetUse, 0);
    iReturn = pasSetTcpFunName("ncWebuseDispNetForm", ncWebuseDispNetForm, 0);
    iReturn = pasSetTcpFunName("ncWebPrintNetUse_u", ncWebPrintNetUse_u, 0);
    iReturn = pasSetTcpFunName("ncWebPrintIcUse", ncWebPrintIcUse, 0);
    iReturn = pasSetTcpFunName("ncWebuseDispIcForm", ncWebuseDispIcForm, 0);
    iReturn = pasSetTcpFunName("ncWebPrintIcUse_u", ncWebPrintIcUse_u, 0);
    iReturn = pasSetTcpFunName("ncWebPrintNetUseWeek", ncWebPrintNetUseWeek, 0);
    iReturn = pasSetTcpFunName("ncWebNetuseDispWeekForm", ncWebNetuseDispWeekForm, 0);
    iReturn = pasSetTcpFunName("ncWebPrintNetUseBm", ncWebPrintNetUseBm, 0);


/* nc015.c */
    iReturn = pasSetTcpFunName("ncWebProxyLogin", ncWebProxyLogin, 0);
    iReturn = pasSetTcpFunName("ncWebProxyModpass", ncWebProxyModpass, 0);


/* nc016.c */

    iReturn = pasSetTcpFunName("ncWebBmTimeFlow", ncWebBmTimeFlow, 0);
    iReturn = pasSetTcpFunName("ncWebTimeControl", ncWebTimeControl, 0);
    iReturn = pasSetTcpFunName("ncWebTimeCtlSet", ncWebTimeCtlSet, 0);
    iReturn = pasSetTcpFunName("ncWebDispUserInfo", ncWebDispUserInfo, 0);
    iReturn = pasSetTcpFunName("ncWebBackupBaseInfo", ncWebBackupBaseInfo, 0);
    iReturn = pasSetTcpFunName("ncWebShowBackup", ncWebShowBackup, 0);
    iReturn = pasSetTcpFunName("ncWebBackFileDownload", ncWebBackFileDownload, 0);
    iReturn = pasSetTcpFunName("ncWebDelBackFile", ncWebDelBackFile, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlSearch", ncWebMailCtlSearch, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlDispSform", ncWebMailCtlDispSform, 0);

    iReturn = pasSetTcpFunName("ncMailCtlCheck", ncMailCtlCheck, 0);
    iReturn = pasSetTcpFunName("ncMailMonCheck", ncMailMonCheck, 0);

    iReturn = pasSetTcpFunName("ncMailAudit1", ncMailAudit1, 0);
    iReturn = pasSetTcpFunName("ncWebTimeCtlReSet", ncWebTimeCtlReSet, 0);
    
    /* nc015_1.c */
    iReturn = pasSetTcpFunName("ncWebDispLogin", ncWebDispLogin, 0);


    iReturn = pasSetTcpFunName("ncWebPopSearch", ncWebPopSearch, 0);
    iReturn = pasSetTcpFunName("ncWebDispPopForm", ncWebDispPopForm, 0);
    iReturn = pasSetTcpFunName("ncWebPopAdd", ncWebPopAdd, 0);
    iReturn = pasSetTcpFunName("ncWebDelSomePop", ncWebDelSomePop, 0);

    iReturn = pasSetTcpFunName("ncWebCountMail", ncWebCountMail, 0);
    iReturn = pasSetTcpFunName("ncWebCountMailByStatus", ncWebCountMailByStatus, 0);

    iReturn = pasSetTcpFunName("ncWebIpDispSform", ncWebIpDispSform, 0);
    iReturn = pasSetTcpFunName("ncWebIpHSearch", ncWebIpHSearch, 0);
    iReturn = pasSetTcpFunName("ncWebHttpDispSform", ncWebHttpDispSform, 0);
    iReturn = pasSetTcpFunName("ncWebHttpHSearch", ncWebHttpHSearch, 0);

    iReturn = pasSetTcpFunName("ncWebMailDispImport", ncWebMailDispImport, 0);
    iReturn = pasSetTcpFunName("ncWebMailSaveImport", ncWebMailSaveImport, 0);
    iReturn = pasSetTcpFunName("ncWebMailDoImport2", ncWebMailDoImport2, 0);
    iReturn = pasSetTcpFunName("ncWebMailDoImport", ncWebMailDoImport, 0);

    iReturn = pasSetTcpFunName("ncWebUseListGroupTree", ncWebUseListGroupTree, 0);
    iReturn = pasSetTcpFunName("ncWebuseDispForm", ncWebuseDispForm, 0);

    iReturn = pasSetTcpFunName("ncWebMailCtlHlist", ncWebMailCtlHlist, 0);

    iReturn = pasSetTcpFunName("ncWebMailCtlHistorySearch", ncWebMailCtlHistorySearch, 0);
    iReturn = pasSetTcpFunName("ncWebMailDispHSform", ncWebMailDispHSform, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlHistoryDel", ncWebMailCtlHistoryDel, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlRead2", ncWebMailCtlRead2, 0);


/* nc018.c */
    iReturn = pasSetTcpFunName("ncWebSystemSet_base", ncWebSystemSet_base, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSave_base", ncWebSystemSave_base, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSet_par", ncWebSystemSet_par, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSave_par", ncWebSystemSave_par, 0);

    iReturn = pasSetTcpFunName("ncWebSystemSet_fun", ncWebSystemSet_fun, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSave_fun", ncWebSystemSave_fun, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSet_back", ncWebSystemSet_back, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSave_back", ncWebSystemSave_back, 0);

    iReturn = pasSetTcpFunName("ncWebSystemSet_clean", ncWebSystemSet_clean, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSave_clean", ncWebSystemSave_clean, 0);

    iReturn = pasSetTcpFunName("ncWebSystemSet_port", ncWebSystemSet_port, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSave_port", ncWebSystemSave_port, 0);
    iReturn = pasSetTcpFunName("ncWebLimShowDefault", ncWebLimShowDefault, 0);
    iReturn = pasSetTcpFunName("ncWebLimSaveDefault", ncWebLimSaveDefault, 0);
    iReturn = pasSetTcpFunName("ncWebShowSystemInfo", ncWebShowSystemInfo, 0);
    iReturn = pasSetTcpFunName("ncWebSystem_Matain", ncWebSystem_Matain, 0);

    iReturn = pasSetTcpFunName("ncWebSystemSave_noweb", ncWebSystemSave_noweb, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSet_noweb", ncWebSystemSet_noweb, 0);

    iReturn = pasSetTcpFunName("ncWebSystemSet_Worktime", ncWebSystemSet_Worktime, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSave_Worktime", ncWebSystemSave_Worktime, 0);

    iReturn = pasSetTcpFunName("ncWebNcRestart", ncWebNcRestart, 0);
    iReturn = pasSetTcpFunName("ncWebNcResetAll", ncWebNcResetAll, 0);

    iReturn = pasSetTcpFunName("ncWebSystemSet_Billrate", ncWebSystemSet_Billrate, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSave_billrate", ncWebSystemSave_billrate, 0);

    iReturn = pasSetTcpFunName("ncWebPrintBill", ncWebPrintBill, 0);
    iReturn = pasSetTcpFunName("ncWebDispNetBillForm", ncWebDispNetBillForm, 0);

    iReturn = pasSetTcpFunName("ncWebSystemSet_virus", ncWebSystemSet_virus, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSave_virus", ncWebSystemSave_virus, 0);
    
    iReturn = pasSetTcpFunName("ncWebSystemSet_url", ncWebSystemSet_url, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSave_url", ncWebSystemSave_url, 0);
    iReturn = pasSetTcpFunName("ncProxySet",ncProxySet,0);
    iReturn = pasSetTcpFunName("ncProxySetSave",ncProxySetSave,0);

/* ncmil001.c               */
    iReturn = pasSetTcpFunName("ncMailMonFl_list", ncMailMonFl_list, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonDel_fl", ncWebMailMonDel_fl,  0);
    iReturn = pasSetTcpFunName("ncWebMailMonFw_fl", ncWebMailMonFw_fl, 0);
    iReturn = pasSetTcpFunName("ncMailFrmFl_list", ncMailFrmFl_list, 0);
    iReturn = pasSetTcpFunName("ncWebMailFrmDel_fl", ncWebMailFrmDel_fl, 0);
    iReturn = pasSetTcpFunName("ncWebShowMailBack", ncWebShowMailBack, 0);
    iReturn = pasSetTcpFunName("ncWebDelMailBack", ncWebDelMailBack, 0);
    iReturn = pasSetTcpFunName("ncWebCountMailMon", ncWebCountMailMon, 0);

    iReturn = pasSetTcpFunName("ncMailCtlFl_list", ncMailCtlFl_list, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlDel_fl", ncWebMailCtlDel_fl, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlFw_fl", ncWebMailCtlFw_fl, 0);
    iReturn = pasSetTcpFunName("ncWebCountMailCtl", ncWebCountMailCtl, 0);
    iReturn = pasSetTcpFunName("ncMailAuditOne", ncMailAuditOne, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlCvtCode", ncWebMailCtlCvtCode, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonCvtCode", ncWebMailMonCvtCode, 0);
    iReturn = pasSetTcpFunName("ncUtlMailReadSelf", ncUtlMailReadSelf, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonFlCvtCode", ncWebMailMonFlCvtCode, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlFlCvtCode", ncWebMailCtlFlCvtCode, 0);

/* ncmil002.c              */
    iReturn = pasSetTcpFunName("ncWebMailCtlExport", ncWebMailCtlExport, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonExport", ncWebMailMonExport, 0);

/* ncmil005.c 监控邮件查询 */
    iReturn = pasSetTcpFunName("ncWebMailMonList", ncWebMailMonList, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonList_h", ncWebMailMonList_h, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonHsear", ncWebMailMonHsear, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonHsear_h", ncWebMailMonHsear_h, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonDel", ncWebMailMonDel, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonDel_h", ncWebMailMonDel_h, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonFw", ncWebMailMonFw, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonFw_h", ncWebMailMonFw_h, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonDispSform", ncWebMailMonDispSform, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonDispSform_h", ncWebMailMonDispSform_h, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonRead", ncWebMailMonRead, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonFwOne", ncWebMailMonFwOne, 0);
    iReturn = pasSetTcpFunName("ncWebUploadList", ncWebUploadList, 0);
    iReturn = pasSetTcpFunName("ncWebUploadRead", ncWebUploadRead, 0);
    iReturn = pasSetTcpFunName("ncWebUploadDel", ncWebUploadDel, 0);
    iReturn = pasSetTcpFunName("ncWebMailExportDelete", ncWebMailExportDelete, 0);
    iReturn = pasSetTcpFunName("ncMailFrmCheck", ncMailFrmCheck, 0);
    iReturn = pasSetTcpFunName("ncWebPostRead", ncWebPostRead, 0);

/* ncmil006.c */
    iReturn = pasSetTcpFunName("ncWebMailDispExport", ncWebMailDispExport, 0);
    iReturn = pasSetTcpFunName("ncWebMailDoExport", ncWebMailDoExport, 0);
    iReturn = pasSetTcpFunName("ncWebMailShowExpStatus", ncWebMailShowExpStatus, 0);

    iReturn = pasSetTcpFunName("ncWebMailCtlSaveDefSet", ncWebMailCtlSaveDefSet, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlDispDefSet", ncWebMailCtlDispDefSet, 0);
    iReturn = pasSetTcpFunName("ncWebRefreshUser", ncWebRefreshUser, 0);


    iReturn = pasSetTcpFunName("ncWebUserModify", ncWebUserModify, 0);
    iReturn = pasSetTcpFunName("ncWebDispUserModiForm", ncWebDispUserModiForm, 0);
    iReturn = pasSetTcpFunName("ncWebDispUserInfo_c", ncWebDispUserInfo_c, 0);
    iReturn = pasSetTcpFunName("ncWebUserSave_c", ncWebUserSave_c, 0);

    iReturn = pasSetTcpFunName("ncWebCountMailMonByU", ncWebCountMailMonByU, 0);
    iReturn = pasSetTcpFunName("ncWebCountMailMonByBm",ncWebCountMailMonByBm, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonCountChart",ncWebMailMonCountChart, 0);

    iReturn = pasSetTcpFunName("ncWebCountMailCtlByU", ncWebCountMailCtlByU, 0);
    iReturn = pasSetTcpFunName("ncWebCountMailCtlByBm",ncWebCountMailCtlByBm, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlCountChart",ncWebMailCtlCountChart, 0);
    
    iReturn = pasSetTcpFunName("ncWebMailAddrPriCtlSet",ncWebMailAddrPriCtlSet, 0);

    /* ncmil007.c */
    iReturn = pasSetTcpFunName("ncMailMonTypeList", ncMailMonTypeList, 0);
    iReturn = pasSetTcpFunName("ncWebMailGroupEdit", ncWebMailGroupEdit, 0);
    iReturn = pasSetTcpFunName("ncWebSetMailGroup", ncWebSetMailGroup, 0);
    iReturn = pasSetTcpFunName("ncMailFrmTypeList", ncMailFrmTypeList, 0);
    iReturn = pasSetTcpFunName("ncWebSetFrmGroup", ncWebSetFrmGroup, 0);
    iReturn = pasSetTcpFunName("ncWebFrmGroupEdit", ncWebFrmGroupEdit, 0);
    iReturn = pasSetTcpFunName("ncMailCtlTypeList", ncMailCtlTypeList, 0);

    iReturn = pasSetTcpFunName("ncWebMailMonSaveDefault", ncWebMailMonSaveDefault, 0);
    iReturn = pasSetTcpFunName("ncWebMailMonDispDefault", ncWebMailMonDispDefault, 0);

    iReturn = pasSetTcpFunName("ncMailCtlFlList", ncMailCtlFlList, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlFl", ncWebMailCtlFl, 0);
    iReturn = pasSetTcpFunName("ncWebMailCtlFlEdit", ncWebMailCtlFlEdit, 0);

/* nc025.c */
    iReturn = pasSetTcpFunName("ncWebLimWebTypeTree", ncWebLimWebTypeTree, 0);
    iReturn = pasSetTcpFunName("ncWebLimWebList", ncWebLimWebList, 0);
    iReturn = pasSetTcpFunName("ncWebLimWebClass", ncWebLimWebClass, 0);
    
    iReturn = pasSetTcpFunName("ncWebWebPolClass", ncWebWebPolClass, 0);
    iReturn = pasSetTcpFunName("ncWebWebPolList", ncWebWebPolList, 0);


    iReturn = pasSetTcpFunName("ncWebLimWebGroup", ncWebLimWebGroup, 0);
    iReturn = pasSetTcpFunName("ncWebLimWebGroupEdit", ncWebLimWebGroupEdit, 0);
    iReturn = pasSetTcpFunName("ncWebLim_SetWebPol", ncWebLim_SetWebPol, 0);
    iReturn = pasSetTcpFunName("ncWebLimWebPolEdit", ncWebLimWebPolEdit, 0);
    iReturn = pasSetTcpFunName("ncWebLimWebPolList", ncWebLimWebPolList, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSet", ncWebSystemSet, 0);
    iReturn = pasSetTcpFunName("ncWebSystemSetSave", ncWebSystemSetSave, 0);
    iReturn = pasSetTcpFunName("ncWebLimWebListSearch", ncWebLimWebListSearch, 0);



/* nc026.c */
    iReturn = pasSetTcpFunName("ncWebLimIpList", ncWebLimIpList, 0);
    iReturn = pasSetTcpFunName("ncWebLimIpGroup", ncWebLimIpGroup, 0);
    iReturn = pasSetTcpFunName("ncWebLimIpGroupEdit", ncWebLimIpGroupEdit, 0);
/* nc027.c */
    iReturn = pasSetTcpFunName("ncWebLimDateList", ncWebLimDateList, 0);
    iReturn = pasSetTcpFunName("ncWebLimDateGroup", ncWebLimDateGroup, 0);
    iReturn = pasSetTcpFunName("ncWebLimDateGroupEdit", ncWebLimDateGroupEdit, 0);

/* nc028.c */
    iReturn = pasSetTcpFunName("ncWebLim_Dispmenu", ncWebLim_Dispmenu, 0);
    iReturn = pasSetTcpFunName("ncWebLim_Listout", ncWebLim_Listout, 0);
    iReturn = pasSetTcpFunName("ncWebLim_Listbase", ncWebLim_Listbase, 0);

    iReturn = pasSetTcpFunName("ncWebLimRuleGroup", ncWebLimRuleGroup, 0);
    iReturn = pasSetTcpFunName("ncWebLimRuleGroupEdit", ncWebLimRuleGroupEdit, 0);

    iReturn = pasSetTcpFunName("ncWebLimApp", ncWebLimApp, 0);
    iReturn = pasSetTcpFunName("ncWebLim_TestProcess", ncWebLim_TestProcess, 0);


/* nc029 */
    iReturn = pasSetTcpFunName("ncWebLim_TestIpService", ncWebLim_TestIpService, 0);
    iReturn = pasSetTcpFunName("ncWebLim_TestIplist", ncWebLim_TestIplist, 0);
    iReturn = pasSetTcpFunName("ncWebLim_TestLimit", ncWebLim_TestLimit, 0);
    iReturn = pasSetTcpFunName("ncWebTestUpload", ncWebTestUpload, 0);
    iReturn = pasSetTcpFunName("ncWeb_TestMailRule", ncWeb_TestMailRule, 0);

/* nc030  */
    iReturn = pasSetTcpFunName("ncWebLim_SetIpPol", ncWebLim_SetIpPol, 0);
    iReturn = pasSetTcpFunName("ncWebLimIpPolList", ncWebLimIpPolList, 0);
    iReturn = pasSetTcpFunName("ncWebLimIpPolEdit", ncWebLimIpPolEdit, 0);

    iReturn = pasSetTcpFunName("ncWebLim_SetWebPol1", ncWebLim_SetWebPol1, 0);
    iReturn = pasSetTcpFunName("ncWebLim_SetWebPol2", ncWebLim_SetWebPol2, 0);
    iReturn = pasSetTcpFunName("ncWebLim_DispWebPo", ncWebLim_DispWebPo, 0);
/* nc034.c  */
    iReturn = pasSetTcpFunName("ncWebUpgrade_1", ncWebUpgrade_1, 0);
    iReturn = pasSetTcpFunName("ncWebUpgrade_2", ncWebUpgrade_2, 0);
    iReturn = pasSetTcpFunName("ncWebUpgrade_Inst", ncWebUpgrade_Inst, 0);

/* nc038.c  */
    iReturn = pasSetTcpFunName("ncWebTcSet_base", ncWebTcSet_base, 0);
    iReturn = pasSetTcpFunName("ncWebTcSave_base", ncWebTcSave_base, 0);
    iReturn = pasSetTcpFunName("ncWebTcClassTree", ncWebTcClassTree, 0);
    iReturn = pasSetTcpFunName("ncWebTcDispClass", ncWebTcDispClass, 0);
    iReturn = pasSetTcpFunName("ncWebTcSaveClass", ncWebTcSaveClass, 0);
    iReturn = pasSetTcpFunName("ncWebTcUserList", ncWebTcUserList, 0);
    iReturn = pasSetTcpFunName("ncWebTcIpList", ncWebTcIpList, 0);

/* nc040.c   */
    iReturn = pasSetTcpFunName("ncWebDebug_DispPkgInfo", ncWebDebug_DispPkgInfo, 0);
    iReturn = pasSetTcpFunName("ncWebDebug_DispUserIp", ncWebDebug_DispUserIp, 0);
/* nc003.c */    
    iReturn = pasSetTcpFunName("ncWebListUserTree_mon", ncWebListUserTree_mon, 0);
    iReturn = pasSetTcpFunName("ncWebIpDispFilterForm", ncWebIpDispFilterForm, 0);
    iReturn = pasSetTcpFunName("ncWebIpSetFilter", ncWebIpSetFilter, 0);
    iReturn = pasSetTcpFunName("ncWebWebDispFilterForm", ncWebWebDispFilterForm, 0);
    iReturn = pasSetTcpFunName("ncWebWebSetFilter", ncWebWebSetFilter, 0);
    iReturn = pasSetTcpFunName("ncMailFlowDispForm", ncMailFlowDispForm, 0);
    iReturn=pasSetTcpFunName("ncMailFlowAvCount",ncMailFlowAvCount,0);
    iReturn=pasSetTcpFunName("ncJstxFlowCount",ncJstxFlowCount,0);
    iReturn=pasSetTcpFunName("ncJstxFlowDispForm",ncJstxFlowDispForm,0);
    
    iReturn=pasSetTcpFunName("ncDeptFlowDispForm",ncDeptFlowDispForm,0);
    iReturn=pasSetTcpFunName("DeptWebCount",DeptWebCount,0);

/* nc040.c */
    iReturn = pasSetTcpFunName("ncWebDebug_DispIptctl", ncWebDebug_DispIptctl, 0);
    iReturn = pasSetTcpFunName("ncWebDebug_DispIptbuf", ncWebDebug_DispIptbuf, 0);
    iReturn = pasSetTcpFunName("ncWebDebug_TestIpt", ncWebDebug_TestIpt, 0);


/* nc041.c */
    iReturn = pasSetTcpFunName("ncWebTestShowPkg", ncWebTestShowPkg, 0);
    iReturn = pasSetTcpFunName("ncWebIpNetFlowForm", ncWebIpNetFlowForm, 0);
    iReturn = pasSetTcpFunName("ncWebPrintNetFlow", ncWebPrintNetFlow, 0);
    iReturn = pasSetTcpFunName("ncWebAdmQyeSysLog", ncWebAdmQyeSysLog, 0);
    iReturn = pasSetTcpFunName("ncWebAdmQyeRunLog", ncWebAdmQyeRunLog, 0);
    iReturn = pasSetTcpFunName("ncWebTestDispContent", ncWebTestDispContent, 0);
    iReturn = pasSetTcpFunName("ncWebTestDispPkgFilter", ncWebTestDispPkgFilter, 0);
    iReturn = pasSetTcpFunName("ncWebTestShowPkgIndex", ncWebTestShowPkgIndex, 0);
    iReturn = pasSetTcpFunName("ncWebTestShowPkgHex", ncWebTestShowPkgHex, 0);
    iReturn = pasSetTcpFunName("ncWebTestShowPkgText", ncWebTestShowPkgText, 0);
    iReturn = pasSetTcpFunName("ncWebTestDispPkgMain", ncWebTestDispPkgMain, 0);
    
    
    
/* ncwebt001.c */
    iReturn = pasSetTcpFunName("ncWebTestShowAllUser", ncWebTestShowAllUser, 0);
    iReturn = pasSetTcpFunName("ncWebTestShowNetwork", ncWebTestShowNetwork, 0);

/* ncweb001.c */
    iReturn = pasSetTcpFunName("ncWebShowNewWarn", ncWebShowNewWarn, 0);
    iReturn = pasSetTcpFunName("ncWebWait", ncWebWait, 0);



/* ncweb002.c */
    iReturn = pasSetTcpFunName("ncWebTestShowWebDb", ncWebTestShowWebDb, 0);
    iReturn = pasSetTcpFunName("ncWebTestShowWebPol", ncWebTestShowWebPol, 0);
//    iReturn = pasSetTcpFunName("ncWebTestShowNetwork", ncWebTestShowNetwork, 0);

/* ncwebt003.c */
    iReturn = pasSetTcpFunName("ncWebTestShowIpBuf", ncWebTestShowIpBuf, 0);
    iReturn = pasSetTcpFunName("ncWebTestShowTcpBuf", ncWebTestShowTcpBuf, 0);
    iReturn = pasSetTcpFunName("ncWebTestShowIpExcept", ncWebTestShowIpExcept, 0);
    iReturn = pasSetTcpFunName("ncWebTestShowOnlineWeb", ncWebTestShowOnlineWeb, 0);

/* ncweb004.c */
    iReturn = pasSetTcpFunName("ncWebImList", ncWebImList, 0);
    iReturn = pasSetTcpFunName("ncWebImDispSform", ncWebImDispSform, 0);
    iReturn = pasSetTcpFunName("ncWebImHsearch", ncWebImHsearch, 0);
    iReturn = pasSetTcpFunName("ncWebImFilterSet", ncWebImFilterSet, 0);
    iReturn = pasSetTcpFunName("ncWebErrNewLog", ncWebErrNewLog, 0);
    iReturn = pasSetTcpFunName("ncWebErrHsearch", ncWebErrHsearch, 0);
    iReturn = pasSetTcpFunName("ncWebErrDispSform", ncWebErrDispSform, 0);
/* nc004 */
	iReturn = pasSetTcpFunName("ncWebModiSomeUserBm", ncWebModiSomeUserBm, 0);
    iReturn = pasSetTcpFunName("ncUtlServerLogin", ncUtlServerLogin, 0);
    iReturn = pasSetTcpFunName("ncUtlServerIsLogin", ncUtlServerIsLogin, 0);
    
/* nc023.c */
    iReturn = pasSetTcpFunName("ncWebSetLimSummary", ncWebSetLimSummary, 0);

//    iReturn = pasSetTcpFunName("ncWebSetLimSummary", ncWebSetLimSummary, 0);
//    iReturn = pasSetTcpFunName("ncUtlImList", ncUtlImList, 0);


/* ncweb003.c   有关树  */
	iReturn = pasSetTcpFunName("ncWebTreeDispGroup", ncWebTreeDispGroup, 0);
    iReturn = pasSetTcpFunName("ncWebTreeDispUser", ncWebTreeDispUser, 0);
    iReturn = pasSetTcpFunName("ncWebUserTree", ncWebUserTree, 0);
	iReturn = pasSetTcpFunName("ncWebTreeDispGroup_F", ncWebTreeDispGroup_F, 0);
    iReturn = pasSetTcpFunName("ncWebTreeDispUser_F", ncWebTreeDispUser_F, 0);

/* ncweb004.c   */
    iReturn = pasSetTcpFunName("ncWebWarnDispSform", ncWebWarnDispSform, 0);
    iReturn = pasSetTcpFunName("ncWebWarnNewLog", ncWebWarnNewLog, 0);

/* bil001.c     */
	iReturn = pasSetTcpFunName("ncWebBillUserSave", ncWebBillUserSave, 0);
    iReturn = pasSetTcpFunName("ncWebBilDispUserInfo", ncWebBilDispUserInfo, 0);
    iReturn = pasSetTcpFunName("ncWebBilUserList", ncWebBilUserList, 0);

    iReturn = pasSetTcpFunName("ncBillDispLogin", ncBillDispLogin, 0);
    iReturn = pasSetTcpFunName("ncBillAuth", ncBillAuth, 0);
    iReturn = pasSetTcpFunName("ncBillHeat", ncBillHeat, 0);
    iReturn = pasSetTcpFunName("ncBillLogout", ncBillLogout, 0);
    iReturn = pasSetTcpFunName("ncWebBilFeeList", ncWebBilFeeList, 0);
    iReturn = pasSetTcpFunName("ncWebBilDispUserFee", ncWebBilDispUserFee, 0);
    iReturn = pasSetTcpFunName("ncWebBillFeeOk", ncWebBillFeeOk, 0);

    iReturn = pasSetTcpFunName("ncWebBilDispUserPass", ncWebBilDispUserPass, 0);
    iReturn = pasSetTcpFunName("ncWebBilModiUserPass", ncWebBilModiUserPass, 0);
    iReturn = pasSetTcpFunName("ncWebBilFeeDetail", ncWebBilFeeDetail, 0);
    iReturn = pasSetTcpFunName("ncBillShowUserStatus", ncBillShowUserStatus, 0);


    iReturn = pasSetTcpFunName("ncBilFeeSetMenu", ncBilFeeSetMenu, 0);
    iReturn = pasSetTcpFunName("ncBilFeeSetTree", ncBilFeeSetTree, 0);
    iReturn = pasSetTcpFunName("ncBilFeeSetProc", ncBilFeeSetProc, 0);
    iReturn = pasSetTcpFunName("ncBilFeeSetBase", ncBilFeeSetBase, 0);
 
    iReturn = pasSetTcpFunName("ncToolsNatIp", ncToolsNatIp, 0);
    iReturn = pasSetTcpFunName("ncToolsNatPort", ncToolsNatPort, 0);
    iReturn = pasSetTcpFunName("ncToolsNatMASQUERADE", ncToolsNatMASQUERADE, 0);
    iReturn = pasSetTcpFunName("ncToolsRouteSet", ncToolsRouteSet, 0);
    
    iReturn = pasSetTcpFunName("ncWebShowUserFlow", ncWebShowUserFlow, 0);

    /* ncweb006.c   */
    iReturn = pasSetTcpFunName("ncWebShowMacIp", ncWebShowMacIp, 0);
    iReturn = pasSetTcpFunName("ncWebShowIpMac", ncWebShowIpMac, 0);
    iReturn = pasSetTcpFunName("ncWebCheckUser", ncWebCheckUser, 0);
    iReturn = pasSetTcpFunName("ncWebShowDbSyncStatus", ncWebShowDbSyncStatus, 0);
    iReturn = pasSetTcpFunName("ncWebOnlineUpdateUrl", ncWebOnlineUpdateUrl, 0);
    iReturn = pasSetTcpFunName("ncWebPronetwaylog12345", ncWebPronetwaylog12345, 0);
    
    /* ncweb007.c   */
   iReturn = pasSetTcpFunName("ncWebShowOnlineCompute", ncWebShowOnlineCompute, 0);
    
    /* ncweb008.c */
    iReturn = pasSetTcpFunName("ncWebSystemDelete", ncWebSystemDelete, 0);
    iReturn = pasSetTcpFunName("ncWebWarnLog", ncWebWarnLog, 0);
    iReturn=pasSetTcpFunName("ncWebSystemShow_time",ncWebSystemShow_time,0);
    iReturn=pasSetTcpFunName("ncWebSystemSave_time",ncWebSystemSave_time,0);
    iReturn=pasSetTcpFunName("ncWebNetSesLog",ncWebNetSesLog,0);
    iReturn=pasSetTcpFunName("ncWebDispNetSesSearch",ncWebDispNetSesSearch,0);

    
    /* ncutl_client.c */
    iReturn=pasSetTcpFunName("ncWebSystemSet_License",ncWebSystemSet_License,0);
    iReturn=pasSetTcpFunName("ncWebSystemSave_License",ncWebSystemSave_License,0);
    iReturn=pasSetTcpFunName("ncWebSystem_LicenseDown",ncWebSystem_LicenseDown,0);
    iReturn=pasSetTcpFunName("ncWebSystem_RestartNc",ncWebSystem_RestartNc,0);
    iReturn = pasSetTcpFunName("ncWebLimWebClassCx", ncWebLimWebClassCx, 0);
    
    //nc_warn_web.c
    iReturn=pasSetTcpFunName("nc_warn_md",nc_warn_md,0);
    iReturn=pasSetTcpFunName("nc_warn_hist",nc_warn_hist,0);
    
    iReturn = pasSetTcpFunName("ncDomainServerSet", ncDomainServerSet, 0);     
		iReturn=pasSetTcpFunName("ncCustDrNext",ncCustDrNext,0);                   
		iReturn=pasSetTcpFunName("userinfo_imp",userinfo_imp,0);                   
		iReturn=pasSetTcpFunName("ncWebDrNext",ncWebDrNext,0);                     
		iReturn=pasSetTcpFunName("webclass_imp",webclass_imp,0);                   
		                                                                           
		iReturn=pasSetTcpFunName("ncWebSystemSet_wd",ncWebSystemSet_wd,0);         
		iReturn=pasSetTcpFunName("ncWebSystemSave_wd",ncWebSystemSave_wd,0);       
		                                                                           
		iReturn=pasSetTcpFunName("ncCodeSystemSet",ncCodeSystemSet,0);             
		iReturn=pasSetTcpFunName("ncCodeSystemSave",ncCodeSystemSave,0);           
		iReturn=pasSetTcpFunName("ncWebTest",ncWebTest,0);  
		iReturn=pasSetTcpFunName("ncWebTestDownLoad",ncWebTestDownLoad,0);  
		         
//nc_reportmake.c
		iReturn=pasSetTcpFunName("ncWebReportMakeList",ncWebReportMakeList,0);
		iReturn=pasSetTcpFunName("ncWebReportMakeLog",ncWebReportMakeLog,0); 
		
		iReturn=pasSetTcpFunName("ncWebOnlineSearch",ncWebOnlineSearch,0); 
		iReturn=pasSetTcpFunName("nc_prxy_onlinuser",nc_prxy_onlinuser,0);
		
		iReturn=pasSetTcpFunName("ncWebNetVistLog",ncWebNetVistLog,0);
		iReturn=pasSetTcpFunName("ncWebDispNetSearch",ncWebDispNetSearch,0);
		
		iReturn=pasSetTcpFunName("ncExpchangeBaseInfo",ncExpchangeBaseInfo,0);
		iReturn=pasSetTcpFunName("ncExpchangePortToPos",ncExpchangePortToPos,0);
		
		
		//nc_showmainfirst.c
		iReturn=pasSetTcpFunName("ncWebShowServiceGrp",ncWebShowServiceGrp,0);
		iReturn=pasSetTcpFunName("ncWebShowOutflowGrp",ncWebShowOutflowGrp,0);
		iReturn=pasSetTcpFunName("ncWebMainSortWeb",ncWebMainSortWeb,0);
		iReturn=pasSetTcpFunName("ncWebShowMainOnlineuser",ncWebShowMainOnlineuser,0);
//		iReturn=pasSetTcpFunName("ncWebMainShowSumRec",ncWebMainShowSumRec,0);
		iReturn=pasSetTcpFunName("ncWebShowFirstRight",ncWebShowFirstRight,0);
		iReturn=pasSetTcpFunName("ncWebShowFirstLeft",ncWebShowFirstLeft,0);
		
		iReturn=pasSetTcpFunName("ncWebSystemSet_dev",ncWebSystemSet_dev,0);
		iReturn=pasSetTcpFunName("ncWebSystemSet_devrule",ncWebSystemSet_devrule,0);
		iReturn=pasSetTcpFunName("ncWebSysParStat",ncWebSysParStat,0);
		
		iReturn=pasSetTcpFunName("ncShowServiceLog",ncShowServiceLog,0);
		iReturn=pasSetTcpFunName("ncWebDispGroupUser",ncWebDispGroupUser,0);
		iReturn=pasSetTcpFunName("ncWebShowOutflowMore",ncWebShowOutflowMore,0);
		iReturn=pasSetTcpFunName("ncWebModSomeUserState",ncWebModSomeUserState,0);
		iReturn=pasSetTcpFunName("ncWebDispUserInfo2",ncWebDispUserInfo2,0);
	  iReturn=pasSetTcpFunName("ncsDispWbsMesg",ncsDispWbsMesg,0);
	  iReturn=pasSetTcpFunName("ncsWebMsgAdd",ncsWebMsgAdd,0);
	  iReturn=pasSetTcpFunName("ncsWbsMsgAdd",ncsWbsMsgAdd,0);
	  iReturn=pasSetTcpFunName("ncWebNetIdLog",ncWebNetIdLog,0);
	  iReturn=pasSetTcpFunName("ncWebNetidDispSform",ncWebNetidDispSform,0);
	  iReturn=pasSetTcpFunName("ncsMesgSendlog",ncsMesgSendlog,0);
	  iReturn=pasSetTcpFunName("ncWebBlackWhite",ncWebBlackWhite,0);
	  iReturn=pasSetTcpFunName("ncWBDrNext",ncWBDrNext,0);
	  iReturn=pasSetTcpFunName("whilteblank_imp",whilteblank_imp,0);
	  iReturn=pasSetTcpFunName("ncWebDispNews",ncWebDispNews,0);
	  iReturn=pasSetTcpFunName("ncWebNewsShowAdd",ncWebNewsShowAdd,0);
	  iReturn=pasSetTcpFunName("ncWebNewsSave",ncWebNewsSave,0);
	  iReturn=pasSetTcpFunName("ncsDispWbsMesg_tip",ncsDispWbsMesg_tip,0);
	  iReturn=pasSetTcpFunName("ncWebLimWebClassNew",ncWebLimWebClassNew,0);
	  iReturn=pasSetTcpFunName("ncWebShowMainTitle",ncWebShowMainTitle,0);
	  iReturn=pasSetTcpFunName("ncWebSetFeeProcy",ncWebSetFeeProcy,0);
	  iReturn=pasSetTcpFunName("ncWebNextFeeProcy",ncWebNextFeeProcy,0);
	  iReturn=pasSetTcpFunName("ncBilFeeLog",ncBilFeeLog,0);
	  iReturn=pasSetTcpFunName("ncBilFeeStat",ncBilFeeStat,0);
	  iReturn=pasSetTcpFunName("ncProxCheckLog",ncProxCheckLog,0);
	  
      iReturn = pasSetTcpFunName("ncWebTestShowUserFee", ncWebTestShowUserFee, 0);
      iReturn = pasSetTcpFunName("ncWebDispHtmlFid", ncWebDispHtmlFid, 0);
	  
	  iReturn = pasSetTcpFunName("ncWebSystemExcpSet_par",ncWebSystemExcpSet_par,0);
	  iReturn = pasSetTcpFunName("ncWebSystemExcpSave_par",ncWebSystemExcpSave_par,0);
	  iReturn=pasSetTcpFunName("ncWebSystemSet_Keyword",ncWebSystemSet_Keyword,0);
	  iReturn=pasSetTcpFunName("ncWebSystemSave_keyword",ncWebSystemSave_keyword,0);
	  iReturn=pasSetTcpFunName("ncWebTcRulePar",ncWebTcRulePar,0);
	  iReturn=pasSetTcpFunName("ncWebTreeDispGroup_New",ncWebTreeDispGroup_New,0);
	  
	  iReturn=pasSetTcpFunName("ncWebDispDeptForm_new",ncWebDispDeptForm_new,0);
	  iReturn=pasSetTcpFunName("ncWebDeptAdd_new",ncWebDeptAdd_new,0);
	  iReturn=pasSetTcpFunName("ncWebDeptDel_new",ncWebDeptDel_new,0);
	  iReturn=pasSetTcpFunName("ncWebDeptModi_new",ncWebDeptModi_new,0);
	  
	  iReturn=pasSetTcpFunName("ncWebTreeDispGroup_New2",ncWebTreeDispGroup_New2,0);
	  
	  iReturn=pasSetTcpFunName("ncWebclassSelect",ncWebclassSelect,0);
	  iReturn=pasSetTcpFunName("ncWebUploadList2",ncWebUploadList2,0);
	  
	  iReturn=pasSetTcpFunName("ncWebFeeUserList",ncWebFeeUserList,0);
	  iReturn=pasSetTcpFunName("ncWebFeeAddlog",ncWebFeeAddlog,0);
	  
	  iReturn=pasSetTcpFunName("ncWebAuth_imageUpload",ncWebAuth_imageUpload,0);
	  iReturn=pasSetTcpFunName("ncReportReceiver",ncReportReceiver,0);
	  iReturn=pasSetTcpFunName("ncReportTypeSet",ncReportTypeSet,0);
	  iReturn=pasSetTcpFunName("ncReportSendLog",ncReportSendLog,0);
    iReturn=pasSetTcpFunName("ncWebModSomeUserFobflag",ncWebModSomeUserFobflag,0);
    
    iReturn=pasSetTcpFunName("ncWebTreeDispGroup_nolimit",ncWebTreeDispGroup_nolimit,0);
    iReturn=pasSetTcpFunName("ncWebDeptModRootname_new",ncWebDeptModRootname_new,0);
    
    
    iReturn=pasSetTcpFunName("ncsDispRolelist",ncsDispRolelist,0);
    iReturn=pasSetTcpFunName("ncsDispRoleFunlist",ncsDispRoleFunlist,0);
    
    iReturn=pasSetTcpFunName("ncsWebRoleEdit",ncsWebRoleEdit,0);
    iReturn=pasSetTcpFunName("ncsRoleRightSave",ncsRoleRightSave,0);
    
    iReturn=pasSetTcpFunName("ncWebDisplayMbManager",ncWebDisplayMbManager,0);
    iReturn=pasSetTcpFunName("ncUserBindMb",ncUserBindMb,0);
    iReturn=pasSetTcpFunName("ncWebMbSave",ncWebMbSave,0);
    iReturn=pasSetTcpFunName("ncWebMbManagerSave",ncWebMbManagerSave,0);
    iReturn=pasSetTcpFunName("ncMbDrNext",ncMbDrNext,0);
    
        iReturn=pasSetTcpFunName("ncMbLogin",ncMbLogin,0);
        iReturn=pasSetTcpFunName("ncUserServiceDispForm",ncUserServiceDispForm,0);
        iReturn=pasSetTcpFunName("ncWebUserServiceStat",ncWebUserServiceStat,0);
        
        iReturn=pasSetTcpFunName("ncWebDhcpFixIp",ncWebDhcpFixIp,0);
        iReturn=pasSetTcpFunName("ncWebDHCPSet",ncWebDHCPSet,0);
        
        iReturn=pasSetTcpFunName("ncWebVistDispCon",ncWebVistDispCon,0);
        iReturn=pasSetTcpFunName("ncWebVistByUser",ncWebVistByUser,0);
        
        iReturn=pasSetTcpFunName("ncComputerList",ncComputerList,0);
        
        iReturn=pasSetTcpFunName("ncWebBilDispUserSimpFee",ncWebBilDispUserSimpFee,0);
        iReturn=pasSetTcpFunName("ncWebDispFeejs",ncWebDispFeejs,0);
        iReturn=pasSetTcpFunName("ncSimpFeeJs",ncSimpFeeJs,0);
        
        //南通酒店治安系统接口
        iReturn=pasSetTcpFunName("ncGetHotelUserInfo",ncGetHotelUserInfo,0);
        
        iReturn=pasSetTcpFunName("ncGetHotelUserInfo_nt",ncGetHotelUserInfo_nt,0);
        iReturn=pasSetTcpFunName("ncGetHotelUserInfo_ntout",ncGetHotelUserInfo_ntout,0);
        
        iReturn=pasSetTcpFunName("ncWebShowUseronlineip",ncWebShowUseronlineip,0);
        iReturn=pasSetTcpFunName("ncWebTreeDispService",ncWebTreeDispService,0);
        iReturn=pasSetTcpFunName("ncWebOnlieServiceStat",ncWebOnlieServiceStat,0);
        iReturn=pasSetTcpFunName("ncWebOnlieServiceUser",ncWebOnlieServiceUser,0);
        
        iReturn=pasSetTcpFunName("ncWebRuleSearch",ncWebRuleSearch,0);
        iReturn=pasSetTcpFunName("ncWebRuleDel",ncWebRuleDel,0);
        iReturn=pasSetTcpFunName("ncWebRuleDelAll",ncWebRuleDelAll,0);
        
        iReturn=pasSetTcpFunName("ncWebGenMenujs",ncWebGenMenujs,0);
        iReturn=pasSetTcpFunName("ncWebGenlayermenu",ncWebGenlayermenu,0);
        iReturn=pasSetTcpFunName("ncWebGenKjcd",ncWebGenKjcd,0);
        
 //nwweb001.c
        iReturn=pasSetTcpFunName("ncWebProcClassEdit",ncWebProcClassEdit,0);
        iReturn=pasSetTcpFunName("ncWebProcClass",ncWebProcClass,0);
        iReturn=pasSetTcpFunName("ncWebProcClassPlist",ncWebProcClassPlist,0);
        iReturn=pasSetTcpFunName("nwWebLim_Proc",nwWebLim_Proc,0);
        iReturn=pasSetTcpFunName("nwWebLim_Proclist",nwWebLim_Proclist,0);
        iReturn=pasSetTcpFunName("nwProcRuleSearch",nwProcRuleSearch,0);
        iReturn=pasSetTcpFunName("ncWebProcRuleDel",ncWebProcRuleDel,0);
        
        iReturn=pasSetTcpFunName("ncShowClientConf",ncShowClientConf,0);
        iReturn=pasSetTcpFunName("ncShowClientConfSave",ncShowClientConfSave,0);
        iReturn=pasSetTcpFunName("ncShowTestClientLink",ncShowTestClientLink,0);
        iReturn=pasSetTcpFunName("ncShowSnmpConf",ncShowSnmpConf,0);
        iReturn=pasSetTcpFunName("ncShowSnmpConfSave",ncShowSnmpConfSave,0);
        iReturn=pasSetTcpFunName("ncShowDHCPlog",ncShowDHCPlog,0);
        iReturn=pasSetTcpFunName("nwWebLim_Watchlist",nwWebLim_Watchlist,0);
        
        iReturn=pasSetTcpFunName("ncFirstHtmlConfirm",ncFirstHtmlConfirm,0);
        iReturn=pasSetTcpFunName("nwWebLim_Joinlist",nwWebLim_Joinlist,0);
        //准入规则
        iReturn=pasSetTcpFunName("ncWebJoinClassDet",ncWebJoinClassDet,0);
        iReturn=pasSetTcpFunName("ncSecondLogin",ncSecondLogin,0);
        
       // ncadt_001.c
       iReturn=pasSetTcpFunName("ncWebLimIpService",ncWebLimIpService,0);
       iReturn=pasSetTcpFunName("ncWebLimShowMon",ncWebLimShowMon,0);
       iReturn=pasSetTcpFunName("ncWebLimMonProlicyDet",ncWebLimMonProlicyDet,0);
       iReturn=pasSetTcpFunName("ncLimMonUser",ncLimMonUser,0);
       iReturn=pasSetTcpFunName("ncWebMailGroupEdit_adt",ncWebMailGroupEdit_adt,0);
       iReturn=pasSetTcpFunName("ncWebFrmGroupEdit_adt",ncWebFrmGroupEdit_adt,0);
       iReturn=pasSetTcpFunName("ncWebTcClassTree_adt",ncWebTcClassTree_adt,0);
       iReturn=pasSetTcpFunName("ncWebDeptlist",ncWebDeptlist,0);
       iReturn=pasSetTcpFunName("ncWebDispDeptForm_adt",ncWebDispDeptForm_adt,0);
       iReturn=pasSetTcpFunName("ncWebDispDeptFormProc_adt",ncWebDispDeptFormProc_adt,0);
       iReturn=pasSetTcpFunName("ncWebTcProcyEdit_adt",ncWebTcProcyEdit_adt,0);
       iReturn=pasSetTcpFunName("ncWebMailProcyEdit_adt",ncWebMailProcyEdit_adt,0);
       iReturn=pasSetTcpFunName("ncWebRuleSearch_adt",ncWebRuleSearch_adt,0);
       iReturn=pasSetTcpFunName("ncWebRuleDel_adt",ncWebRuleDel_adt,0);
       iReturn=pasSetTcpFunName("ncWebRuleMod_adt",ncWebRuleMod_adt,0);
       iReturn=pasSetTcpFunName("ncWebDelSomeDept",ncWebDelSomeDept,0);
       iReturn=pasSetTcpFunName("ncWebDeptModi_adt",ncWebDeptModi_adt,0);
       iReturn=pasSetTcpFunName("ncWebDispDeptAddFormProc",ncWebDispDeptAddFormProc,0);
       
       //ncadt_002.c
       iReturn=pasSetTcpFunName("ncToolPing",ncToolPing,0);
       iReturn=pasSetTcpFunName("ncConfigImport",ncConfigImport,0);
       iReturn=pasSetTcpFunName("ncConfigClear",ncConfigClear,0);
       iReturn=pasSetTcpFunName("ncWebDispNetwork_adt",ncWebDispNetwork_adt,0);
       iReturn=pasSetTcpFunName("ncWebSetNetwork_adt",ncWebSetNetwork_adt,0);
       
       iReturn=pasSetTcpFunName("ncWebLimShowDefault_adt",ncWebLimShowDefault_adt,0);
       iReturn=pasSetTcpFunName("ncWebLimSaveDefault_adt",ncWebLimSaveDefault_adt,0);
       
       iReturn=pasSetTcpFunName("ncWebSystemSet_base_adt",ncWebSystemSet_base_adt,0);
       iReturn=pasSetTcpFunName("ncWebSystemSave_base_adt",ncWebSystemSave_base_adt,0);
       iReturn=pasSetTcpFunName("ncWebSystemSet_fun_adt",ncWebSystemSet_fun_adt,0);
       iReturn=pasSetTcpFunName("ncWebSystemSave_fun_adt",ncWebSystemSave_fun_adt,0);
       iReturn=pasSetTcpFunName("ncWebLimWebClass_adt",ncWebLimWebClass_adt,0);
       
//ncweb_tcnew.c
       iReturn=pasSetTcpFunName("ncWebTcIntfaceTree",ncWebTcIntfaceTree,0);
       iReturn=pasSetTcpFunName("ncWebTcDispInterface",ncWebTcDispInterface,0);
       iReturn=pasSetTcpFunName("ncWebTcInterfaceSave",ncWebTcInterfaceSave,0);
       iReturn=pasSetTcpFunName("ncWebTcTdNew",ncWebTcTdNew,0);
       iReturn=pasSetTcpFunName("ncTcNewProcy",ncTcNewProcy,0);
       iReturn=pasSetTcpFunName("ncTcNewProcySave",ncTcNewProcySave,0);
       iReturn=pasSetTcpFunName("ncWebTcProcyDet",ncWebTcProcyDet,0);
       iReturn=pasSetTcpFunName("ncWebTcUserProcy",ncWebTcUserProcy,0);
       iReturn=pasSetTcpFunName("ncWebSoft_upload",ncWebSoft_upload,0);
       iReturn=pasSetTcpFunName("ncWebSoft_sjml",ncWebSoft_sjml,0);
       
       iReturn=pasSetTcpFunName("ncWebCountStatByDept",ncWebCountStatByDept,0);
       iReturn=pasSetTcpFunName("ncWebCountStatByUser",ncWebCountStatByUser,0);
       iReturn=pasSetTcpFunName("ncWebSortServiceByDate",ncWebSortServiceByDate,0);
       
       iReturn=pasSetTcpFunName("ncWebClassDispForm",ncWebClassDispForm,0);
       iReturn=pasSetTcpFunName("ncWebStatByWebclass",ncWebStatByWebclass,0);
       iReturn=pasSetTcpFunName("ncWebVlanIDtoPos",ncWebVlanIDtoPos,0);
       iReturn=pasSetTcpFunName("ncTcServiceType",ncTcServiceType,0);
       iReturn=pasSetTcpFunName("ncTcServiceTypeSave",ncTcServiceTypeSave,0);
       iReturn=pasSetTcpFunName("ncWebServiceTypeDet",ncWebServiceTypeDet,0);
       
       iReturn=pasSetTcpFunName("ncHttpsBlandWhite",ncHttpsBlandWhite,0);
       iReturn=pasSetTcpFunName("ncHttpsBlandWhite_Save",ncHttpsBlandWhite_Save,0);
       
       iReturn=pasSetTcpFunName("ncWebServiceStatByUser",ncWebServiceStatByUser,0);
       iReturn=pasSetTcpFunName("ncShowFirstServiceDet",ncShowFirstServiceDet,0);
       iReturn=pasSetTcpFunName("ncShowFirstWebByUser",ncShowFirstWebByUser,0);
       
       iReturn=pasSetTcpFunName("ncWeDomainChangePasswd",ncWeDomainChangePasswd,0);
       iReturn=pasSetTcpFunName("ncWebSynDomain",ncWebSynDomain,0);
       iReturn=pasSetTcpFunName("ncSysDepUserByDomain",ncSysDepUserByDomain,0);
       iReturn=pasSetTcpFunName("ncSysDomainOver",ncSysDomainOver,0);
       
       //ncipservice_new.c
       iReturn=pasSetTcpFunName("ncWebLimIpGroup_new",ncWebLimIpGroup_new,0);
       iReturn=pasSetTcpFunName("ncWebLimIpService_new",ncWebLimIpService_new,0);
       iReturn=pasSetTcpFunName("ncWebLim_Listout_new",ncWebLim_Listout_new,0);
       iReturn=pasSetTcpFunName("ncWebLim_Listout_fun",ncWebLim_Listout_fun,0);
       
       
       iReturn=pasSetTcpFunName("ncBlackWhiteBase",ncBlackWhiteBase,0);
       iReturn=pasSetTcpFunName("ncBlackWhiteBase_Save",ncBlackWhiteBase_Save,0);
       iReturn=pasSetTcpFunName("ncYyzBase",ncYyzBase,0);
       iReturn=pasSetTcpFunName("ncYyzBas_Save",ncYyzBas_Save,0);
       iReturn=pasSetTcpFunName("ncDomainServerSet_new",ncDomainServerSet_new,0);
       iReturn=pasSetTcpFunName("ncYyzLdapSet",ncYyzLdapSet,0);
       iReturn=pasSetTcpFunName("ncYyzLdapSet_Save",ncYyzLdapSet_Save,0);
       iReturn=pasSetTcpFunName("ncWebDeptMove_new",ncWebDeptMove_new,0);
       
       iReturn=pasSetTcpFunName("ncWebShowWebClassFirst",ncWebShowWebClassFirst,0);
       iReturn=pasSetTcpFunName("ncWebShowNetflowFirst",ncWebShowNetflowFirst,0);
       iReturn=pasSetTcpFunName("ncShowNetflowAjax",ncShowNetflowAjax,0);
       iReturn=pasSetTcpFunName("ncWebShowOutflowMore_new",ncWebShowOutflowMore_new,0);
       iReturn=pasSetTcpFunName("ncWebShowCpu",ncWebShowCpu,0);
       iReturn=pasSetTcpFunName("ncWebShowCpuAjx",ncWebShowCpuAjx,0);
       iReturn=pasSetTcpFunName("ncWebWorkTimeList",ncWebWorkTimeList,0);
       iReturn=pasSetTcpFunName("ncWebShowServiceClassFirst",ncWebShowServiceClassFirst,0);
       iReturn=pasSetTcpFunName("ncWebShowServiceFirstCx",ncWebShowServiceFirstCx,0);
       iReturn=pasSetTcpFunName("ncWebShowServiceBytime",ncWebShowServiceBytime,0);
       iReturn=pasSetTcpFunName("ncWebServiceFirstDet",ncWebServiceFirstDet,0);
       iReturn=pasSetTcpFunName("ncWebShowWebClassBytime",ncWebShowWebClassBytime,0);
       iReturn=pasSetTcpFunName("ncUserForbit",ncUserForbit,0);
       iReturn=pasSetTcpFunName("ncWebShowWebClassBytimeDet",ncWebShowWebClassBytimeDet,0);
       iReturn=pasSetTcpFunName("ncWebServiceBytimeDet",ncWebServiceBytimeDet,0);
       iReturn=pasSetTcpFunName("ncWebTcProcyTree",ncWebTcProcyTree,0);
       iReturn=pasSetTcpFunName("ncWebTcProcy_new",ncWebTcProcy_new,0);
       iReturn=pasSetTcpFunName("ncWebTcProcy_newSave",ncWebTcProcy_newSave,0);
       iReturn=pasSetTcpFunName("ncGenerateReport",ncGenerateReport,0);
       
       iReturn=pasSetTcpFunName("ncWebShowServiceDeptCx",ncWebShowServiceDeptCx,0);
       iReturn=pasSetTcpFunName("ncShowFirstServiceDepDet",ncShowFirstServiceDepDet,0);
       iReturn=pasSetTcpFunName("ncWebShowWebsiteBytime",ncWebShowWebsiteBytime,0);
       
       
       iReturn=pasSetTcpFunName("ncWebLim_SetIpPol_new",ncWebLim_SetIpPol_new,0);
       iReturn=pasSetTcpFunName("ncWebLim_servicedet",ncWebLim_servicedet,0);
       iReturn=pasSetTcpFunName("ncWebClass_TreeNew",ncWebClass_TreeNew,0);
       iReturn=pasSetTcpFunName("ncWebLimWebList_new",ncWebLimWebList_new,0);
       iReturn=pasSetTcpFunName("ncWebShowUrlCx",ncWebShowUrlCx,0);
       iReturn=pasSetTcpFunName("ncWebShowMove",ncWebShowMove,0);
       iReturn=pasSetTcpFunName("ncWebSortUserByFlow_75",ncWebSortUserByFlow_75,0);
       iReturn=pasSetTcpFunName("ncWebShowUserlog",ncWebShowUserlog,0);
       iReturn=pasSetTcpFunName("ncWebTreeDispWebClass",ncWebTreeDispWebClass,0);
       iReturn=pasSetTcpFunName("ncFlowTimeDispForm_new",ncFlowTimeDispForm_new,0);
       iReturn=pasSetTcpFunName("ncWebFlowCount_new",ncWebFlowCount_new,0);
       iReturn=pasSetTcpFunName("ncWebShowUserFlowBytime",ncWebShowUserFlowBytime,0);
       iReturn=pasSetTcpFunName("ncWebAllReport",ncWebAllReport,0);
       iReturn=pasSetTcpFunName("ncShowGServiceDet",ncShowGServiceDet,0);
       iReturn=pasSetTcpFunName("ncShowUserSortDet",ncShowUserSortDet,0);
       iReturn=pasSetTcpFunName("ncShowGWebClassDet",ncShowGWebClassDet,0);
       iReturn=pasSetTcpFunName("ncShowGWebSiteDet",ncShowGWebSiteDet,0);
       iReturn=pasSetTcpFunName("ncWebAllReport_user",ncWebAllReport_user,0);
       iReturn=pasSetTcpFunName("ncWebSortip_75",ncWebSortip_75,0);
       iReturn=pasSetTcpFunName("ncWebShowiplog",ncWebShowiplog,0);
       iReturn=pasSetTcpFunName("ncWebSortWeb_75",ncWebSortWeb_75,0);
       iReturn=pasSetTcpFunName("ncWebShowToplog",ncWebShowToplog,0);
       iReturn=pasSetTcpFunName("ncWebShowWebsitelog",ncWebShowWebsitelog,0);
       
       iReturn=pasSetTcpFunName("ncWebAllReport_user_PDF",ncWebAllReport_user_PDF,0);
       iReturn=pasSetTcpFunName("ncWebAllReport_PDF",ncWebAllReport_PDF,0);
       iReturn=pasSetTcpFunName("ncWebUrl_upload",ncWebUrl_upload,0);
       iReturn=pasSetTcpFunName("ncWebShowUrlUpdatelog",ncWebShowUrlUpdatelog,0);
       
       iReturn=pasSetTcpFunName("ncWebSortint_75",ncWebSortint_75,0);
       iReturn=pasSetTcpFunName("ncWebShowintlog",ncWebShowintlog,0);
       iReturn=pasSetTcpFunName("ncWebOnlineUpdateService",ncWebOnlineUpdateService,0);
       iReturn=pasSetTcpFunName("ncWebPat_upload",ncWebPat_upload,0);
       
       iReturn=pasSetTcpFunName("ncWebShowServiceDisCx",ncWebShowServiceDisCx,0);
       iReturn=pasSetTcpFunName("ncShowFirstServiceDisDet",ncShowFirstServiceDisDet,0);
       iReturn=pasSetTcpFunName("ncWebShowTrendsBytime",ncWebShowTrendsBytime,0);
       iReturn=pasSetTcpFunName("S",S,0);
       iReturn=pasSetTcpFunName("ncWebBBSLog",ncWebBBSLog,0);
       
       
       iReturn = pasSetTcpFunName("ncWebMailDownload", ncWebMailDownload, 0);		
		   iReturn = pasSetTcpFunName("ncWebGenReportHtmlDownLoad", ncWebGenReportHtmlDownLoad, 0);	
				iReturn = pasSetTcpFunName("ncReportReset", ncReportReset, 0);
		   iReturn = pasSetTcpFunName("ncWebMailDownload22", ncWebMailDownload22, 0);	
		   iReturn=pasSetTcpFunName("ncWebOnlieServiceStat_first",ncWebOnlieServiceStat_first,0);
		   iReturn=pasSetTcpFunName("ncWebLimNomonService",ncWebLimNomonService,0);
		   iReturn=pasSetTcpFunName("ncWebShowServiceDet",ncWebShowServiceDet,0);
		   
		   iReturn=pasSetTcpFunName("ncWebShowAllFlowStat",ncWebShowAllFlowStat,0);
		   iReturn=pasSetTcpFunName("ncGroupBandSet",ncGroupBandSet,0);
		   iReturn=pasSetTcpFunName("ncWebOnlieDeptBand",ncWebOnlieDeptBand,0);
		   iReturn=pasSetTcpFunName("ncWebOnlieDeptStat",ncWebOnlieDeptStat,0);
		   iReturn=pasSetTcpFunName("ncWebOnlieDeptStat",ncWebOnlieDeptStat,0);
		   
		   iReturn=pasSetTcpFunName("ncWebAuth_Define",ncWebAuth_Define,0);
		   iReturn=pasSetTcpFunName("ncWebAuth_DefinePrew",ncWebAuth_DefinePrew,0);
		   iReturn=pasSetTcpFunName("ncWebAuth_DefineSave",ncWebAuth_DefineSave,0);
		   iReturn=pasSetTcpFunName("ncWebAuth_DefineUpload",ncWebAuth_DefineUpload,0);
		   iReturn=pasSetTcpFunName("ncRestore_Factory",ncRestore_Factory,0);
		   
		   iReturn=pasSetTcpFunName("ncWebShowMobZc",ncWebShowMobZc,0);
		   
		   iReturn=pasSetTcpFunName("ncTsLoginSet",ncTsLoginSet,0);
		   iReturn=pasSetTcpFunName("ncTsLoginSetSave",ncTsLoginSetSave,0);
		   iReturn=pasSetTcpFunName("ncImclientDispForm",ncImclientDispForm,0);
		   iReturn=pasSetTcpFunName("ncWebImclientStat",ncWebImclientStat,0);
		   iReturn=pasSetTcpFunName("ncWebTreeDispUserVid",ncWebTreeDispUserVid,0);
		   iReturn=pasSetTcpFunName("ncTreeDispImclientUser",ncTreeDispImclientUser,0);
		   iReturn=pasSetTcpFunName("ncWebImclientStatCont",ncWebImclientStatCont,0);
		   iReturn=pasSetTcpFunName("ncImStatBySes",ncImStatBySes,0);
		   iReturn=pasSetTcpFunName("ncLoadIMses",ncLoadIMses,0);
		   iReturn=pasSetTcpFunName("ncImStatByRid",ncImStatByRid,0);
		   iReturn=pasSetTcpFunName("ncLoadIMses2",ncLoadIMses2,0);
		   iReturn=pasSetTcpFunName("ncLoadIMAccount",ncLoadIMAccount,0);
		   iReturn=pasSetTcpFunName("ncRecvSmbInfo",ncRecvSmbInfo,0);
		   iReturn=pasSetTcpFunName("ncWebShowMoveuser",ncWebShowMoveuser,0);
		   
		   iReturn=pasSetTcpFunName("ncWebTreeDispUserRid",ncWebTreeDispUserRid,0);
		   iReturn=pasSetTcpFunName("ncWebTcDispInterface2",ncWebTcDispInterface2,0);
		   iReturn=pasSetTcpFunName("ncWebTcInterfaceSave2",ncWebTcInterfaceSave2,0);
		   
		   iReturn=pasSetTcpFunName("ncImStatBySesDep",ncImStatBySesDep,0);
		   iReturn=pasSetTcpFunName("ncImStatBySesUser",ncImStatBySesUser,0);
		   iReturn=pasSetTcpFunName("ncShowImClientDet",ncShowImClientDet,0);
		   iReturn=pasSetTcpFunName("ncImStatBySesDep_corp",ncImStatBySesDep_corp,0);
		   
		   iReturn=pasSetTcpFunName("nc_opentest_mail",nc_opentest_mail,0);
		   iReturn=pasSetTcpFunName("nc_setest_mail",nc_setest_mail,0);
		   iReturn=pasSetTcpFunName("nc_testmailUpload",nc_testmailUpload,0);
		   iReturn=pasSetTcpFunName("ncMailSendSet_par",ncMailSendSet_par,0);
		   iReturn=pasSetTcpFunName("ncMailSendSave_par",ncMailSendSave_par,0);
		   iReturn=pasSetTcpFunName("ncWebAuthShowReport",ncWebAuthShowReport,0);
		   iReturn=pasSetTcpFunName("ncShowIpFromFile",ncShowIpFromFile,0);
		   
		   iReturn=pasSetTcpFunName("ncShowOnlineuserBand",ncShowOnlineuserBand,0);
		   iReturn=pasSetTcpFunName("ncWebServiceStatByUserIf",ncWebServiceStatByUserIf,0);
		   
		   iReturn=pasSetTcpFunName("ncWebError_WarnSet",ncWebError_WarnSet,0);
		   iReturn=pasSetTcpFunName("ncWebError_WarnSave",ncWebError_WarnSave,0);
		   
		   iReturn=pasSetTcpFunName("nc_smp_gatewayset",nc_smp_gatewayset,0);
		   iReturn=pasSetTcpFunName("nc_smp_gatewaysave",nc_smp_gatewaysave,0);
		   iReturn=pasSetTcpFunName("nc_smp_warntest",nc_smp_warntest,0);
		   iReturn=pasSetTcpFunName("ncWebDispModiPassLogin",ncWebDispModiPassLogin,0);
		   iReturn=pasSetTcpFunName("ncWarnContSet",ncWarnContSet,0);
		   iReturn=pasSetTcpFunName("ncShowWarnSendSms",ncShowWarnSendSms,0);
		   iReturn=pasSetTcpFunName("nc_smp_warnSend",nc_smp_warnSend,0);
		   iReturn=pasSetTcpFunName("ncShowSmsLog",ncShowSmsLog,0);
		   iReturn=pasSetTcpFunName("ncWebSearhLog",ncWebSearhLog,0);
		   iReturn=pasSetTcpFunName("ncWebUpServiceSave_par",ncWebUpServiceSave_par,0);
		   iReturn=pasSetTcpFunName("ncWebUpdateService",ncWebUpdateService,0);
		   iReturn=pasSetTcpFunName("ncWebUpdateServiceFirst",ncWebUpdateServiceFirst,0);
		   iReturn=pasSetTcpFunName("ncWebShowUpServerStatAjx",ncWebShowUpServerStatAjx,0);
		   iReturn=pasSetTcpFunName("ncMobileAuth_parset",ncMobileAuth_parset,0);
		   iReturn=pasSetTcpFunName("ncMobilePar_Save",ncMobilePar_Save,0);
		   iReturn=pasSetTcpFunName("ncSquidAuth",ncSquidAuth,0);
		   iReturn=pasSetTcpFunName("ncSquidNcAuth",ncSquidNcAuth,0);

		   
		   iReturn=pasSetTcpFunName("ncYyzBase2",ncYyzBase2,0);
		   iReturn=pasSetTcpFunName("ncYyzBas_Save2",ncYyzBas_Save2,0);
		   
		   iReturn=pasSetTcpFunName("ncShowMoreWebSite",ncShowMoreWebSite,0);
		   
		    iReturn=pasSetTcpFunName("ncWebAutoSave_Update",ncWebAutoSave_Update,0);
		    iReturn=pasSetTcpFunName("ncShowUpdateLog",ncShowUpdateLog,0);
		    iReturn=pasSetTcpFunName("ncShowServiceDip",ncShowServiceDip,0);
		    
		    iReturn=pasSetTcpFunName("ncWebLogin_par",ncWebLogin_par,0);
		    iReturn=pasSetTcpFunName("ncsWebLoginSave_par",ncsWebLoginSave_par,0);
		    
		    iReturn=pasSetTcpFunName("ncDiskRate_par",ncDiskRate_par,0);
		    iReturn=pasSetTcpFunName("ncDiskRateSave_par",ncDiskRateSave_par,0);
		    
		    
		    iReturn=pasSetTcpFunName("ncWebRuleSearch_3c",ncWebRuleSearch_3c,0);
		    iReturn=pasSetTcpFunName("ncWebNcExit",ncWebNcExit,0);
		    iReturn=pasSetTcpFunName("ncWebCountByWarnLevel",ncWebCountByWarnLevel,0);
		    iReturn=pasSetTcpFunName("ncWebTreeDispCompUser",ncWebTreeDispCompUser,0);
		    
		    iReturn=pasSetTcpFunName("ncWebShowHisScreen",ncWebShowHisScreen,0);
		    
//松江财税几个统计函数		    

		    iReturn=pasSetTcpFunName("ncWebShowUserFlow_sj",ncWebShowUserFlow_sj,0);
		    iReturn=pasSetTcpFunName("ncWebTreeDispGroup_nolimit_sj",ncWebTreeDispGroup_nolimit_sj,0);
		    iReturn=pasSetTcpFunName("ncWebOnlieServiceStat_sj",ncWebOnlieServiceStat_sj,0);
		    iReturn=pasSetTcpFunName("ncWebTreeDispUser_sj",ncWebTreeDispUser_sj,0);
		    
		   iReturn=pasSetTcpFunName("ncWebShowNetflowFirst_sj",ncWebShowNetflowFirst_sj,0);
		   iReturn=pasSetTcpFunName("ncWebOnlieDeptBand_sj",ncWebOnlieDeptBand_sj,0);
		   iReturn=pasSetTcpFunName("ncWebOnlieServiceStat_bigsj",ncWebOnlieServiceStat_bigsj,0);			   
		   iReturn=pasSetTcpFunName("ncWebShowServiceDeptCx_sj",ncWebShowServiceDeptCx_sj,0);
			 iReturn=pasSetTcpFunName("ncWebShowTrendsBytime_sj",ncWebShowTrendsBytime_sj,0);	
		   iReturn=pasSetTcpFunName("ncWebShowServiceDisCx_sj",ncWebShowServiceDisCx_sj,0);	
		   iReturn=pasSetTcpFunName("ncWebShowWebsiteBytime_sj",ncWebShowWebsiteBytime_sj,0);
		   iReturn=pasSetTcpFunName("ncWebShowUserFlowBytime_sj",ncWebShowUserFlowBytime_sj,0);
		   iReturn=pasSetTcpFunName("ncWebShowWebClassBytime_sj",ncWebShowWebClassBytime_sj,0);	
			   	   		    
		    iReturn=pasSetTcpFunName("ncWebUserTree_sj",ncWebUserTree_sj,0);

		    
		    iReturn=pasSetTcpFunName("ncUserForbit_sj",ncUserForbit_sj,0);
		    
		    
		    iReturn=pasSetTcpFunName("ncWebSystemSet_fwset",ncWebSystemSet_fwset,0);		    
		    iReturn=pasSetTcpFunName("ncWebFwSave_par",ncWebFwSave_par,0);
		    
		    iReturn=pasSetTcpFunName("ncWebSendBySnmp_set",ncWebSendBySnmp_set,0);		    
		    iReturn=pasSetTcpFunName("ncWebSendBySnmp_setSave",ncWebSendBySnmp_setSave,0);
		    
		    iReturn=pasSetTcpFunName("ncWebSystemSet_base_new",ncWebSystemSet_base_new,0);
		    
		    iReturn=pasSetTcpFunName("ncbrgsave",ncbrgsave,0);
		    iReturn=pasSetTcpFunName("ncDelBrg",ncDelBrg,0);
		    iReturn=pasSetTcpFunName("ncWebSave_base_new",ncWebSave_base_new,0);
		    
		    iReturn=pasSetTcpFunName("ncDispReportWizard",ncDispReportWizard,0);
		    
		    iReturn=pasSetTcpFunName("ncReportWizardSave",ncReportWizardSave,0);
		    iReturn=pasSetTcpFunName("ncReportPlateList",ncReportPlateList,0);
		    
		    iReturn=pasSetTcpFunName("ncReportHis",ncReportHis,0);
		    iReturn=pasSetTcpFunName("ncShowMoreWebSiteIp",ncShowMoreWebSiteIp,0);
		    iReturn=pasSetTcpFunName("ncShowHisReport",ncShowHisReport,0);
		    iReturn=pasSetTcpFunName("ncWebDispCmpReportDef",ncWebDispCmpReportDef,0);
		    iReturn=pasSetTcpFunName("ncReportPlateList_comp",ncReportPlateList_comp,0);
		    iReturn=pasSetTcpFunName("ncShowCompReport",ncShowCompReport,0);
		    
				iReturn=pasSetTcpFunName("ncOnServiceStat_User",ncOnServiceStat_User,0);    
		    iReturn=pasSetTcpFunName("ncOnServiceStat_Dept",ncOnServiceStat_Dept,0); 
		    
		    
		    iReturn=pasSetTcpFunName("ncProxServerSet",ncProxServerSet,0); 
		    iReturn=pasSetTcpFunName("ncProxyServiceSave",ncProxyServiceSave,0); 
		    
		    iReturn=pasSetTcpFunName("ncWebLim_LimFile",ncWebLim_LimFile,0); 
		    iReturn=pasSetTcpFunName("ncWebLim_LimSearch",ncWebLim_LimSearch,0); 
		    
		    iReturn=pasSetTcpFunName("ncWebFileLog",ncWebFileLog,0); 
		    iReturn=pasSetTcpFunName("ncWebLoginHeat",ncWebLoginHeat,0); 
		    		  
		    iReturn=pasSetTcpFunName("vpnServerPar",vpnServerPar,0); 	
		    iReturn=pasSetTcpFunName("vpnServerSave_par",vpnServerSave_par,0); 
		    
		    iReturn=pasSetTcpFunName("vpnClientPar",vpnClientPar,0); 
		    iReturn=pasSetTcpFunName("vpnClientSave_par",vpnClientSave_par,0); 
		    
		      
		    iReturn=pasSetTcpFunName("vpnDrNext",vpnDrNext,0); 
		    iReturn=pasSetTcpFunName("vpnGateToGateConnect",vpnGateToGateConnect,0); 
		    iReturn=pasSetTcpFunName("vpnAddGateConnect",vpnAddGateConnect,0); 
		    iReturn=pasSetTcpFunName("vpnGate2GateSave_par",vpnGate2GateSave_par,0); 
		    iReturn=pasSetTcpFunName("vpnClientCnf",vpnClientCnf,0); 
		    
		    iReturn=pasSetTcpFunName("vpnGenClientCnf",vpnGenClientCnf,0);
		    iReturn=pasSetTcpFunName("vpnuserlist",vpnuserlist,0);
		    iReturn=pasSetTcpFunName("vpnAddUser",vpnAddUser,0);
		    iReturn=pasSetTcpFunName("vpnUserSave",vpnUserSave,0);
		    iReturn=pasSetTcpFunName("vpnuseractive",vpnuseractive,0);
		    
		    iReturn=pasSetTcpFunName("vpnclientnet",vpnclientnet,0);
		    iReturn=pasSetTcpFunName("vpnClientNetAdd",vpnClientNetAdd,0);
		    iReturn=pasSetTcpFunName("vpnClientNetSave",vpnClientNetSave,0);
		    
		    		    
		    iReturn=pasSetTcpFunName("ncShowCompReport2",ncShowCompReport2,0); 
		    iReturn=pasSetTcpFunName("ncQQrulelist",ncQQrulelist,0);
		    
		    iReturn=pasSetTcpFunName("qqShowpassword",qqShowpassword,0);
		    iReturn=pasSetTcpFunName("qqDispQQLim",qqDispQQLim,0);
		    iReturn=pasSetTcpFunName("qqDispQQLimSave",qqDispQQLimSave,0);
		    
		    iReturn=pasSetTcpFunName("nc_jgftp_parset",nc_jgftp_parset,0);
		    iReturn=pasSetTcpFunName("nc_jgftp_save",nc_jgftp_save,0);
		    iReturn=pasSetTcpFunName("nc_jgftp_test",nc_jgftp_test,0);
		    iReturn=pasSetTcpFunName("ncWebLoginMob",ncWebLoginMob,0);
		    
		    iReturn=pasSetTcpFunName("ncWebDispLoginMob",ncWebDispLoginMob,0);
		    
		    iReturn=pasSetTcpFunName("ncWebMailReadSource",ncWebMailReadSource,0);
		    iReturn=pasSetTcpFunName("ncPass_procyset",ncPass_procyset,0);
		    iReturn=pasSetTcpFunName("ncPass_procy_Save",ncPass_procy_Save,0);
		    
		    iReturn=pasSetTcpFunName("ncImdetcontlist",ncImdetcontlist,0);
		    
		    iReturn=pasSetTcpFunName("ncWebSnaprulelist",ncWebSnaprulelist,0);
		    iReturn=pasSetTcpFunName("webDispSnapLim",webDispSnapLim,0);
		    iReturn=pasSetTcpFunName("ncWebShowUserDep",ncWebShowUserDep,0);
		    iReturn=pasSetTcpFunName("SnapLimSave",SnapLimSave,0);
		    
		    iReturn=pasSetTcpFunName("ncWebSnapLog",ncWebSnapLog,0);
		    iReturn=pasSetTcpFunName("ncWebReadSnap",ncWebReadSnap,0);
		    
		    iReturn=pasSetTcpFunName("ncWebBBSform",ncWebBBSform,0);
		    iReturn=pasSetTcpFunName("ncShowDeny",ncShowDeny,0);
		    iReturn=pasSetTcpFunName("ncShowDenySave",ncShowDenySave,0);
		    iReturn=pasSetTcpFunName("ncWebPlateVistDispCon",ncWebPlateVistDispCon,0);
		    
		    iReturn=pasSetTcpFunName("ncWebVistPlate",ncWebVistPlate,0);
//上网行为周报		    
		    iReturn=pasSetTcpFunName("ncWeekReport_Con",ncWeekReport_Con,0);
		    iReturn=pasSetTcpFunName("ncWeekReport",ncWeekReport,0);
		    
		    iReturn=pasSetTcpFunName("ncWebUserSearch_hotel",ncWebUserSearch_hotel,0);
		    iReturn=pasSetTcpFunName("ncHotelUser_add",ncHotelUser_add,0);
		    iReturn=pasSetTcpFunName("ncWebDelSomeUser_hotel",ncWebDelSomeUser_hotel,0);   
		    
		    iReturn=pasSetTcpFunName("ncUserFobflag_hotel",ncUserFobflag_hotel,0); 
		    iReturn=pasSetTcpFunName("ncDispAddHotelUser",ncDispAddHotelUser,0); 
		    iReturn=pasSetTcpFunName("ncHotelUser_Mody",ncHotelUser_Mody,0); 
		    iReturn=pasSetTcpFunName("ncWebShowSelfWeb",ncWebShowSelfWeb,0); 
		    iReturn=pasSetTcpFunName("ncWebDispPorcylist",ncWebDispPorcylist,0);
		    
		    iReturn=pasSetTcpFunName("ncBaseDrNext",ncBaseDrNext,0);
		    iReturn=pasSetTcpFunName("base_imp",base_imp,0);
		    
		    iReturn=pasSetTcpFunName("ncWebProcyDrNext",ncWebProcyDrNext,0);
		    iReturn=pasSetTcpFunName("webProcy_imp",webProcy_imp,0);
		    iReturn=pasSetTcpFunName("ncWebGetMobPass",ncWebGetMobPass,0);
		    
		    iReturn=pasSetTcpFunName("ncWebShowMailCtlBt",ncWebShowMailCtlBt,0);
		    iReturn=pasSetTcpFunName("ncWebShowMailCtlBmUserBt",ncWebShowMailCtlBmUserBt,0);
		    iReturn=pasSetTcpFunName("ncMailCtlDet",ncMailCtlDet,0);
		    
		    iReturn=pasSetTcpFunName("ncWebGetMobPass_Cy",ncWebGetMobPass_Cy,0);
		    iReturn=pasSetTcpFunName("ncWebShowHelpUser",ncWebShowHelpUser,0);
		    iReturn=pasSetTcpFunName("ncHelpUserLogin",ncHelpUserLogin,0);
		    iReturn=pasSetTcpFunName("ncWebHelpUserList",ncWebHelpUserList,0);
		    iReturn=pasSetTcpFunName("ncIPdepSet2",ncIPdepSet2,0);
		    iReturn=pasSetTcpFunName("ncProtocolDefine",ncProtocolDefine,0);
		    
		    iReturn=pasSetTcpFunName("ncBBSKeyrulelist",ncBBSKeyrulelist,0);
		    iReturn=pasSetTcpFunName("ncDispGroupRzfs",ncDispGroupRzfs,0);

		    iReturn=pasSetTcpFunName("ncGroupRzfsSave",ncGroupRzfsSave,0);
		    
		    iReturn=pasSetTcpFunName("ncWebSyslog_Ip",ncWebSyslog_Ip,0);
		    iReturn=pasSetTcpFunName("ncWebSyslog_IpSave",ncWebSyslog_IpSave,0);

		    iReturn = pasSetTcpFunNameS("ncsReqAuthCenter",ncsReqAuthCenter, NULL,0,PAS_CRYPT_TEA);
		    
		   iReturn=pasSetTcpFunName("ncProtocolDefine",ncProtocolDefine,0);	
		   iReturn=pasSetTcpFunName("ncWebCAdownload_driver",ncWebCAdownload_driver,0);
		   
         //idc
		   //idc
		   iReturn=pasSetTcpFunName("ncsIdcSearch",ncsIdcSearch,0);
			 iReturn=pasSetTcpFunName("ncsIdcSetKey",ncsIdcSetKey,0);
			 iReturn=pasSetTcpFunName("ncsIdcRule_maintain",ncsIdcRule_maintain,0);
			 iReturn=pasSetTcpFunName("ncsIdcRlue_am",ncsIdcRlue_am,0);
			 iReturn=pasSetTcpFunName("ncsIdcRulesSave",ncsIdcRulesSave,0);
			 iReturn=pasSetTcpFunName("ncsWebTreeIdcrule_nolimit",ncsWebTreeIdcrule_nolimit,0);
			 iReturn=pasSetTcpFunName("ncsIDC_RulekeyDrNext",ncsIDC_RulekeyDrNext,0);
			 iReturn=pasSetTcpFunName("ncs_Idc_key_imp",ncs_Idc_key_imp,0);
			 iReturn=pasSetTcpFunName("ncWebReplaceco_key",ncWebReplaceco_key,0);
			 iReturn=pasSetTcpFunName("ncsIdcRulekey_maintain",ncsIdcRulekey_maintain,0);
					   
		   //sjhl 
		   iReturn=pasSetTcpFunName("nc_Sjhl_webvisit",nc_Sjhl_webvisit,0);	
		   iReturn=pasSetTcpFunName("nc_Sjhl_hostvisit",nc_Sjhl_hostvisit,0);	
		   iReturn=pasSetTcpFunName("nc_Sjhl_hosttest",nc_Sjhl_hosttest,0);	
		   iReturn=pasSetTcpFunName("nc_Sjhl_historylog",nc_Sjhl_historylog,0);	
		   iReturn=pasSetTcpFunName("nc_Sjhl_onlineFrm",nc_Sjhl_onlineFrm,0);	
		   iReturn=pasSetTcpFunName("nc_Sjhl_starttest",nc_Sjhl_starttest,0);	
		   
		   iReturn=pasSetTcpFunName("ncWebProcyRuleDel",ncWebProcyRuleDel,0);	
		   iReturn=pasSetTcpFunName("ncWebProcyRuleDelAll",ncWebProcyRuleDelAll,0);	
		   iReturn=pasSetTcpFunName("ncWebTreeDispGroupProcy",ncWebTreeDispGroupProcy,0);	
		   iReturn=pasSetTcpFunName("ncProncyFpSave",ncProncyFpSave,0);	
		   iReturn=pasSetTcpFunName("ncWebTreeDispYhProcy",ncWebTreeDispYhProcy,0);	
		   
		   iReturn=pasSetTcpFunName("ncShowTestSupportLink",ncShowTestSupportLink,0);	
		   iReturn=pasSetTcpFunName("ncShowTestSupportSelect",ncShowTestSupportSelect,0);	
		   iReturn=pasSetTcpFunName("ncWebSave_Defworktime",ncWebSave_Defworktime,0);
		   iReturn=pasSetTcpFunName("ncIpSearch_ib",ncIpSearch_ib,0);
		   iReturn=pasSetTcpFunName("ncWebMon_cmd",ncWebMon_cmd,0);
		   iReturn=pasSetTcpFunName("ncWebFile_upload",ncWebFile_upload,0);
		   
		   iReturn=pasSetTcpFunName("ncWebVpnStartSet",ncWebVpnStartSet,0);
		   iReturn=pasSetTcpFunName("ncWebVpnStartSet_save",ncWebVpnStartSet_save,0);
		   
		   	   iReturn=pasSetTcpFunName("ncWebWhzxParSet",ncWebWhzxParSet,0);
		   iReturn=pasSetTcpFunName("ncWebWhzxParSet_save",ncWebWhzxParSet_save,0);
		   
		   iReturn=pasSetTcpFunName("ncWebDataCenter_Par",ncWebDataCenter_Par,0);		   
		   iReturn=pasSetTcpFunName("ncWebDataCenter_ParSave",ncWebDataCenter_ParSave,0);
		   
		   iReturn=pasSetTcpFunName("ncDnReg",ncDnReg,0);
		   iReturn=pasSetTcpFunName("ncDnLogin",ncDnLogin,0);
		   iReturn=pasSetTcpFunName("ncDnLogout",ncDnLogout,0);
		   
		   iReturn=pasSetTcpFunName("nc_jingdong_user",nc_jingdong_user,0);
		   iReturn=pasSetTcpFunName("nc_jingdong_url",nc_jingdong_url,0);
		   
		   iReturn=pasSetTcpFunName("ncWifjkAuth",ncWifjkAuth,0);
		   
		   iReturn=pasSetTcpFunName("ncsReqSendJGUser",ncsReqSendJGUser,0);
		   
		   iReturn=pasSetTcpFunName("ncWebDbOperLog",ncWebDbOperLog,0);
		   iReturn=pasSetTcpFunName("ncWebShowDbBt",ncWebShowDbBt,0);
		   iReturn=pasSetTcpFunName("ncWebShowDbBytime",ncWebShowDbBytime,0);
		   
		   iReturn=pasSetTcpFunName("ncUserPasswordProcy",ncUserPasswordProcy,0);
		   iReturn=pasSetTcpFunName("ncUserPasswordProcy_Save",ncUserPasswordProcy_Save,0);
		   
		   iReturn=pasSetTcpFunName("ncDbMonType",ncDbMonType,0);
		   iReturn=pasSetTcpFunName("ncWebDbMonDispDefault",ncWebDbMonDispDefault,0);
		   iReturn=pasSetTcpFunName("ncDbMonSaveDefault",ncDbMonSaveDefault,0);
		   
		   iReturn=pasSetTcpFunName("ncDbMonTypeDetList",ncDbMonTypeDetList,0);
		   
		   iReturn=pasSetTcpFunName("ncWebDeptlist_xm",ncWebDeptlist_xm,0);
		   
		   iReturn=pasSetTcpFunName("ncWebMulogin",ncWebMulogin,0);
		   
		   iReturn=pasSetTcpFunName("ncWebSiteLoginLog",ncWebSiteLoginLog,0);
		   iReturn=pasSetTcpFunName("ncWebSiteRegLog",ncWebSiteRegLog,0);
		   
		   iReturn=pasSetTcpFunName("ncWebSepCorpByDep",ncWebSepCorpByDep,0);
		   
		   iReturn=pasSetTcpFunName("ncShowQqAuth",ncShowQqAuth,0);
		    iReturn=pasSetTcpFunName("ncWebQQlogin",ncWebQQlogin,0);
		    
		   iReturn=pasSetTcpFunName("ncReqTcpAuth",ncReqTcpAuth,0);
       iReturn=pasSetTcpFunName("nc_jingdong_reportall",nc_jingdong_reportall,0);     
		   iReturn=pasSetTcpFunName("ncWebServiceSortUserByTime",ncWebServiceSortUserByTime,0);	
		   
		   iReturn=pasSetTcpFunName("ncReqTcpAuth_nojm",ncReqTcpAuth_nojm,0);	
		   
		   iReturn=pasSetTcpFunName("ncMwdLogin_out",ncMwdLogin_out,0);	
		   
		   iReturn=pasSetTcpFunName("ncWebSysSet_IPWhite",ncWebSysSet_IPWhite,0);
		   iReturn=pasSetTcpFunName("ncWebSysSave_IPWhite",ncWebSysSave_IPWhite,0);
		   iReturn=pasSetTcpFunName("ncReqTcp_Msg",ncReqTcp_Msg,0);
		   
		   iReturn=pasSetTcpFunName("ncWebSysSet_Zrtip",ncWebSysSet_Zrtip,0);
		   iReturn=pasSetTcpFunName("ncWebSysSave_Zrtip",ncWebSysSave_Zrtip,0);
		   
		   iReturn=pasSetTcpFunName("ncHzLogin",ncHzLogin,0);
		   iReturn=pasSetTcpFunName("ncHzLogout",ncHzLogout,0);
		   
		   iReturn=pasSetTcpFunName("ncWebLinSet_dev",ncWebLinSet_dev,0);
		   iReturn=pasSetTcpFunName("ncWebLinGroup_set",ncWebLinGroup_set,0);
       iReturn=pasSetTcpFunName("ncWebAuth_app",ncWebAuth_app,0);
	

	
    return 0;
}   
