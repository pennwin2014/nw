#ifndef __NCRTLDEF__
#define __NCRTLDEF__

/* nc000.c */
int ncGetHttpInfo(char *pIn,int iLen,char *pHost,char *pUrl,
       char *pAgent);
int ncStrGetVar(char *pIn,int inLen,char *pVar,char *pValue,
                int outLen);
int ncStrGetUrl(char *pIn,int iLen,char *caUrl,int outlen);
int ncUtlConfirmService(char *pBuffer,unsigned long lBytes,int *iContentFlags);
int ncInitSystemInfo(utShmHead *psShmHead);
char *ncWebSubStr(char *pBuf,int iLen,char *pSub,int iFlags);
int ncUtlStartNetsee(char *pCommand);
int ncUtlRestartNc(char *pCommand,char  *caConfFile);
char *ncUtlInstallPath(char *pRun);
int ncStartCollectPkg0(utShmHead *psShmHead);
int ncIPdepSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncDomainServerSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


/* nc001.c */
int ncStartCollectPkgDebug(utShmHead *psShmHead);
int ncInitIpFlow(utShmHead *psShmHead);
int ncCheckIp(utShmHead  *psShmHead);
int ncProcessCheck(utShmHead  *psShmHead);
int ncLimReLoad(utShmHead *psShmHead);
ncUserInfo *ncCheckNetworkIp(utShmHead *psShmHead,ncIpHead *psIpHead,int *iFlags0);
int ncGetService(utShmHead *psShmHead,unsigned char cProtocol,unsigned short nPort);
unsigned long  ncUtlGetThatDays(unsigned long lTime);
/* nc002.c */
int ncInitIpPkg(utShmHead *psShmHead);
int ncInitInternal(utShmHead *psShmHead);
int ncInitLimit(utShmHead *psShmHead);
int ncInitMonitor(utShmHead *psShmHead);
int ncInitUserInfo(utShmHead *psShmHead);
ncUserInfo *ncGetEmptyUserInfo(utShmHead *psShmHead,ncUserInfo *psUserInfo,
                        unsigned long lIp);
ncUserInfo *ncGetEmptyUserInfo1(ncUserInfo *psUserInfo,
                        char *caMac);
ncUserInfo *ncGetUserInfo3(utShmHead *psShmHead,
                     long lUserid, unsigned char *pMac);
int ncGetMonNum(utShmHead *psShmHead, unsigned long lId);
int ncGetLimitNum(utShmHead *psShmHead,unsigned long lId);
int ncUtlCheckIcBuf(utShmHead *psShmHead);
int ncStartIc(utShmHead *psShmHead);
/* nc003.c */
int ncWebAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserAccess(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUserStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebReplace(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebReplace_mon(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebQyeIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebModiPass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebFlowCount(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowNewvister(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncFlowTimeDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
char *ncCvtL2TimeStr(unsigned long lTime);

/* int ncWebSetupInternet_save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupInternet_list(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupInternet_form(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupWeb_list(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupWeb_save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetupWeb_form(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
*/
int ncWebShowNewweb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShutdown(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispModiPass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncMailFlowDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailFlowAvCount(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncJstxFlowDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncJstxFlowCount(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncDeptFlowDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int DeptWebCount(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc004.c */

int ncWebDelSomeUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserAddForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebListUserTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebListUserTree_w(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispDeptForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAssignUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminUserList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminDispUserForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminRoleList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncAdminRoleModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdminRole(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncAdminUserSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebListGroupTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserModify(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserModiForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserInfo_c(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserSave_c(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptModi(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserExportCsv(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncUtilUserHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispSuser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebModiSomeUserBm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncTest2005(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc005.c */
int ncWebDispNetwork(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetNetwork(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc006.c */
int ncDelCountLog(utShmHead *psShmHead0);
int ncDelDetailLog(utShmHead *psShmHead0);
int ncCountLog(utShmHead *psShmHead0);
int ncDelTempFile(utShmHead *psShmHead0);

/* nc007.c  */
char *ncCvtConntime(unsigned long lConntime);
int ncWebCountIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
char *ncCvtSetService(utShmHead *psShmHead,int lService);
char *ncCvtMac(unsigned char *caMac);
int ncWebCountTotalFlow(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOutFlowChart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountIpChart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc008.c */
int ncWebQyehttp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCount(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebInternetLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebInternetLogout(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebInternetModpass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserBindWidthSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebIpHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebIpDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebHttpHSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebHttpDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebRefreshUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebShowUserFlow(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int nc_prxy_onlinuser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc009.c */
int ncWebSortUserByFlow(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortWeb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailReadUploadHeads(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPrintWebUse_u(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortByWeb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncFobSort(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncFobSortDIP(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSortDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortAll(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* ncmil001.c */
int ncMailMonFl_list(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDel_fl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFw_fl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailFrmFl_list(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailFrmDel_fl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowMailBack(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDelMailBack(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailMon(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailCtlFl_list(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDel_fl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlFw_fl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailCtl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFlCvtCode(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlFlCvtCode(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncmil002.c */
int ncMailAutoBack(utShmHead *psShmHead);
int ncMailShowRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailDispOneRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailModiOneRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailDelSomeRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlOutHtm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlAuditlst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlAudit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailCtlCheck(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailMonCheck(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailAudit1(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlHlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlHistorySearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDispHSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlHistoryDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlRead2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailAuditOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlCvtCode(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonCvtCode(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncUtlMailReadSelf(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlExport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonExport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* ncmail003.c */
int ncMailMonListRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailMonDispOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailMonModiOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailMonDelSome(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncmil005.c */
int ncWebMailMonList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonList_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonHsear(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonHsear_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDel_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFw(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFw_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDispSform_h(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFwOne(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebUploadList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUploadRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUploadDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebMailExportDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPostRead(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* ncmil004.c */
int ncMailFilterProcess(utShmHead *psShmHead);
int ncMailFrmCheck(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncmail006.c */
int ncWebPopSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispPopForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPopAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDelSomePop(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailByStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDispImport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailSaveImport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDoImport2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDoImport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebMailDispExport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailDoExport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailShowExpStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


int ncWebMailCtlSaveDefSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlDispDefSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebCountMailMonByU(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailMonByBm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonCountChart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebCountMailCtlByU(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountMailCtlByBm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlCountChart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebMailAddrPriCtlSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncmil007.c */
int ncMailMonTypeList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailGroupEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetMailGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailFrmTypeList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetFrmGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebFrmGroupEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailCtlTypeList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonSaveDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonDispDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailMonFl_list(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailCtlFlList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlFl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailCtlFlEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc014.c */
int ncWebPrintNetUse(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebuseDispNetForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPrintNetUse_u(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPrintIcUse(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebuseDispIcForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPrintIcUse_u(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc015.c */
int ncSendCheckTables(utShmHead *p);
int ncConStartTableControl(utShmHead *psShmHead);
int ncSendResetTables(utShmHead *psShmHead);
int ncWebProxyLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebProxyModpass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc016.c */
int ncTimCtl(utShmHead *psShmHead);
int ncWebTimeCtlReSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc011.c */

ncPostDataContent *ncReadPostData(char *caFile, char *pOutFile);
/* nc013.c */
int ncBackup(utShmHead *psShmHead);
/* nc014.c */
int ncWebPrintWebUse(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebUseListGroupTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebuseDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPrintNetUseWeek(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebNetuseDispWeekForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPrintNetUseBm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


/* nc015_1   */
int ncWebDispLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncutl001.c */
char *ncMailCtl_GetFile();

/* nc016 */
int ncWebTimeCtlSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTimeControl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackupBaseInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowBackup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBackFileDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDelBackFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBmTimeFlow(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc018.c */
int ncWebSystemSet_base(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_base(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_par(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_par(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_fun(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_fun(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_back(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_back(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_clean(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_clean(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_port(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_port(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimSaveDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimShowDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowSystemInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystem_Matain(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_noweb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_noweb(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_Worktime(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_Worktime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_billrate(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_Billrate(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebPrintBill(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispNetBillForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_virus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_virus(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_url(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_url(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncProxySet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncProxySetSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc019.c */
int ncStartIcAuth(utShmHead *psShmHead);
/* nc021.c */
unsigned long *ncLimGetThisDateId(int *iSum,unsigned long lTime);

/* nc023.c */
int ncWebSetLimSummary(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc025.c */
int ncWebLimWebTypeTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimWebList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimWebClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebWebPolClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebWebPolList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebLimWebGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimWebGroupEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_SetWebPol(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimWebPolEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimWebPolList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSetSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimWebListSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc026.c */
int ncWebLimIpList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimIpGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimIpGroupEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc027.c */
int ncWebLimDateList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimDateGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimDateGroupEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc028.c */
int ncWebLim_Dispmenu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_Listout(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_Listbase(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimRuleGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimRuleGroupEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimApp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc029 */
int ncWebLim_TestIplist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_TestProcess(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_TestLimit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_TestIpService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWeb_TestMailRule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc030.c */
int ncWebLim_SetIpPol(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimIpPolList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimIpPolEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebLim_SetWebPol1(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_SetWebPol2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_DispWebPo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebNcRestart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebNcResetAll(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc038.c   */
int ncWebTcSet_base(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebTcSave_base(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcDispClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcSaveClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcUserList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcClassTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcIpList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* nc040.c */
int ncWebDebug_DispPkgInfo(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebDebug_DispUserIp(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebAdmQyeRunLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncutl01.c */
int ncUtlInitUserBuf(utShmHead *psShmHead);
int ncTest0000(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebTestUpload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc034.c  */
int ncWebUpgrade_1(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUpgrade_2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUpgrade_Inst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* proxy_004  */
int ncStartSmtpProxy(utShmHead *psShmHead);
/* proxy_005  */
int ncStartPop3Proxy(utShmHead *psShmHead);
/* proxy_006   */
int ncStartHttpProxy(utShmHead *psShmHead);
/* nc002.c      */
int ncStartNtAuth(utShmHead *psShmHead);
/* nc003.c */
int ncWebListUserTree_mon(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebIpDispFilterForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebWebDispFilterForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebIpSetFilter(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebWebSetFilter(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nc040.c */
int ncWebDebug_DispIptctl(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebDebug_DispIptbuf(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebDebug_TestIpt(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);

/* nc041.c */
int ncWebTestShowPkg(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebIpNetFlowForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPrintNetFlow(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAdmQyeSysLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestDispContent(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestDispPkgFilter(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestShowPkgIndex(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestShowPkgHex(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestShowPkgText(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestDispPkgMain(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* ncweb001.c  */
int ncWebShowNewWarn(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebWait(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlineSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* ncweb002.c */
int ncUtlServerLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncUtlServerIsLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncweb003.c */
int ncWebTreeDispGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispGroup_F(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispUser_F(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncweb004.c */
int ncWebWarnNewLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebWarnDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


/* ncwebt001.c */
int ncWebTestShowAllUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestShowNetwork(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncwebt002.c  */
int ncWebTestShowWebDb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestShowWebPol(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


/* ncwebt003.c */
int ncWebTestShowIpBuf(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestShowTcpBuf(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestShowIpExcept(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTestShowOnlineWeb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* ncweb004.c */
int ncWebImList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebImDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebImHsearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebImFilterSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebErrNewLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebErrHsearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebErrDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* bil001.c */
int ncWebBilDispUserInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBillUserSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBilUserList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBillDispLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBillAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBillHeat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBillLogout(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBilFeeList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBilDispUserFee(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBillFeeOk(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBilDispUserPass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBilModiUserPass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebBilFeeDetail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBillShowUserStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBilFeeSetMenu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBilFeeSetTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBilFeeSetProc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBilFeeSetBase(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBilEditPromit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/* nctool01.c  */
int ncToolsNatIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncToolsNatPort(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncToolsNatMASQUERADE(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncToolsRouteSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


/* ncweb006.c */
int ncWebShowMacIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowIpMac(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCheckUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowDbSyncStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlineUpdateUrl(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/* ncweb007.c */
int ncWebShowOnlineCompute(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


/* ncweb008.c */
int ncWebSystemDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebWarnLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemShow_time(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_time(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebNetSesLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispNetSesSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncContrlCenterSet_Save(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncContrlCenterSet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);

/* ncutl_license.htm */
int ncWebSystemSet_License(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_License(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystem_LicenseDown(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystem_RestartNc(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebLimWebClassCx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncCustDrNext(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int userinfo_imp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDrNext(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int webclass_imp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebSystemSet_wd(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_wd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//nc_warn_web.c
int nc_warn_md(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_warn_hist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncCodeSystemSet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncCodeSystemSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//nc_reportmake.c
int ncWebReportMakeList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebReportMakeLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebNetVistLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispNetSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncExpchangeBaseInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncExpchangePortToPos(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//nc_showmainfirst.c
int ncWebShowServiceGrp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowOutflowGrp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMainSortWeb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowMainOnlineuser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
//int ncWebMainShowSumRec(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowFirstRight(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowFirstLeft(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_dev(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_devrule(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSysParStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowServiceLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispGroupUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowOutflowMore(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebModSomeUserState(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispUserInfo2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebNetIdLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebNetidDispSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsMesgSendlog(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebBlackWhite(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWBDrNext(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int whilteblank_imp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispNews(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebNewsShowAdd(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebNewsSave(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncsDispWbsMesg_tip(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);

int ncWebSystemExcpSet_par(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemExcpSave_par(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_Keyword(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_keyword(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcRulePar(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispGroup_New(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispDeptForm_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptAdd_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptDel_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptModi_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispGroup_New2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebclassSelect(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUploadList2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebFeeUserList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebFeeAddlog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebAuth_imageUpload(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncReportReceiver(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncReportTypeSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncReportSendLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebNetParSet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebNetParSetSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebNetInterFaceSet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncNetInterFaceSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebTreeDispGroup_nolimit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptModRootname_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncsDispRolelist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsDispRoleFunlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsWebRoleEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsRoleRightSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebDisplayMbManager(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncUserBindMb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMbSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebMbManagerSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMbLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMbDrNext(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncMbLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncUserServiceDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserServiceStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebDhcpFixIp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDHCPSet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);

int ncWebVistDispCon(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebVistByUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncComputerList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebBilDispUserSimpFee(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispFeejs(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSimpFeeJs(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncGetHotelUserInfo(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead);

int ncWebTreeDispService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlieServiceStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlieServiceUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebRuleSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebRuleDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebRuleDelAll(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebGenMenujs(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebGenlayermenu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebGenKjcd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowOnlineCompute(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebProcClassEdit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebProcClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebProcClassPlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebLim_Proc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebLim_Proclist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwProcRuleSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebProcRuleDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowClientConf(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncShowClientConfSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowTestClientLink(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncShowSnmpConfSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowSnmpConf(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncShowDHCPlog(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int nwWebLim_Watchlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncFirstHtmlConfirm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nwWebLim_Joinlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebJoinClassDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSecondLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//安达通
int ncWebLimIpService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimShowMon(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimMonProlicyDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncLimMonUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailGroupEdit_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebFrmGroupEdit_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcClassTree_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispDeptForm_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispDeptFormProc_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcProcyEdit_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailProcyEdit_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebRuleSearch_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebRuleDel_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebRuleMod_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDelSomeDept(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptModi_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispDeptAddFormProc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncToolPing(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncConfigImport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebDispNetwork_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSetNetwork_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimShowDefault_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimSaveDefault_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_base_adt(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_base_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSystemSet_fun_adt(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSystemSave_fun_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncConfigClear(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimWebClass_adt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
//ncweb_tcnew.c
int ncWebTcIntfaceTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcDispInterface(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcInterfaceSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcTdNew(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncTcNewProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncTcNewProcySave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcProcyDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcUserProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebSoft_upload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSoft_sjml(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountStatByDept(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountStatByUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortServiceByDate(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebClassDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebStatByWebclass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebVlanIDtoPos(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncTcServiceType(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncTcServiceTypeSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebServiceTypeDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncHttpsBlandWhite(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncHttpsBlandWhite_Save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebServiceStatByUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowFirstServiceDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowFirstWebByUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


//ncipservice_new.c   网络服务新界面
int ncWebLimIpGroup_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimIpService_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_Listout_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_Listout_fun(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncBlackWhiteBase(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncBlackWhiteBase_Save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncYyzBase(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
 
int ncYyzBas_Save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncDomainServerSet_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncYyzLdapSet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncYyzLdapSet_Save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDeptMove_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebShowWebClassFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowNetflowFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowNetflowAjax(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowOutflowMore_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebShowCpu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowCpuAjx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebWorkTimeList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebShowServiceClassFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowServiceFirstCx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowServiceBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebServiceFirstDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowWebClassBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncUserForbit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowWebClassBytimeDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebServiceBytimeDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcProcyTree(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcProcy_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcProcy_newSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncGenerateReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowServiceDeptCx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowFirstServiceDepDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowWebsiteBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


int ncWebLim_SetIpPol_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_servicedet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebClass_TreeNew(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLimWebList_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUrlCx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowMove(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebSortUserByFlow_75(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUserlog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispWebClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncFlowTimeDispForm_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebFlowCount_new(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUserFlowBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAllReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowGServiceDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowUserSortDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowGWebClassDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowGWebSiteDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAllReport_user(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortip_75(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowiplog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortWeb_75(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowToplog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowWebsitelog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebAllReport_user_PDF(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAllReport_PDF(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUrl_upload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUrlUpdatelog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSortint_75(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowintlog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebOnlineUpdateService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebPat_upload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebShowServiceDisCx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowFirstServiceDisDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispWebClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebShowTrendsBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebMailDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncReportReset(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebGenReportHtmlDownLoad(utShmHead *psShmHead,int iFd,char *caCond,char *caOrder,char *caFile,int flag,long lStime,long lEtime,char *caRname,char *out_flag,char *down_flag);
int ncWebMailDownload22(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlieServiceStat_first(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebLimNomonService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowServiceDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowAllFlowStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncGroupBandSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlieDeptBand(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlieDeptStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlieDeptStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebAuth_Define(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebAuth_DefinePrew(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebAuth_DefineSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAuth_DefineUpload(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncRestore_Factory(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncTsLoginSet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncTsLoginSetSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncImclientDispForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebImclientStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispUserVid(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncTreeDispImclientUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebImclientStatCont(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncImStatBySes(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncLoadIMses(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncImStatByRid(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncLoadIMses2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncLoadIMAccount(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncRecvSmbInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowMoveuser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispUserRid(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcDispInterface2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTcInterfaceSave2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncImStatBySesDep(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncImStatBySesUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncShowImClientDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncImStatBySesDep_corp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int nc_opentest_mail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_setest_mail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_testmailUpload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncMailSendSet_par(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailSendSave_par(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncShowIpFromFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebAuthShowReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowOnlineuserBand(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebServiceStatByUserIf(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebError_WarnSet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebError_WarnSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int nc_smp_gatewayset(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_smp_gatewaysave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int nc_smp_warntest(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispModiPassLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWarnContSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowWarnSendSms(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_smp_warnSend(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowSmsLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSearhLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebUpServiceSave_par(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUpdateService(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUpdateServiceFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUpServerStatAjx(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncMobilePar_Save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMobileAuth_parset(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);

int ncShowMoreWebSite(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebAutoSave_Update(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebRuleSearch_3c(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowUpdateLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowServiceDip(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLogin_par(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncsWebLoginSave_par(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncDiskRate_par(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncDiskRateSave_par(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebNcExit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebCountByWarnLevel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispCompUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebShowHisScreen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//松江财税局项目的函数
int ncWebTreeDispGroup_nolimit_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUserFlow_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlieServiceStat_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserTree_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispUser_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncUserForbit_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncOnServiceStat_Dept(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


int ncWebSystemSet_fwset(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebFwSave_par(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebSendBySnmp_set(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSendBySnmp_setSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//厦门漳州应付招标
int ncProxServerSet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncProxyServiceSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_LimFile(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLim_LimSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebFileLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLoginHeat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int vpnServerPar(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int vpnServerSave_par(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int vpnClientPar(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int vpnClientSave_par(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int vpnDrNext(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int vpnGateToGateConnect(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int vpnAddGateConnect(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int vpnGate2GateSave_par(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int vpnClientCnf(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int vpnGenClientCnf(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int vpnuserlist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int vpnAddUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int vpnUserSave(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int vpnuseractive(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int vpnclientnet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int vpnClientNetAdd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int vpnClientNetSave(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncQQrulelist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int qqShowpassword(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int qqShowpassword(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int qqDispQQLim(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int qqDispQQLimSave(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncShowCompReport2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int nc_jgftp_parset(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_jgftp_save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_jgftp_test(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebLoginMob(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDispLoginMob(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMailReadSource(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncPass_procyset(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);

int ncPass_procy_Save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncImdetcontlist(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSnaprulelist(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int webDispSnapLim(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUserDep(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int SnapLimSave(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSnapLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebReadSnap(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebBBSform(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowDeny(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncShowDenySave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebPlateVistDispCon(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebVistPlate(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWeekReport_Con(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWeekReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebUserSearch_hotel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncHotelUser_add(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead);
int ncWebDelSomeUser_hotel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncUserFobflag_hotel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncDispAddHotelUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncHotelUser_Mody(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead);

//松江
int ncWebOnlieServiceStat_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowNetflowFirst_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlieDeptBand_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
//int ncWebShowWebClassBytime_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebOnlieServiceStat_bigsj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);   //实时，客户不要了
//int ncWebOnlieServiceStat_trend(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);	 //实时流量趋势图 未完成，客户提出不用实时

int ncWebShowServiceDeptCx_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead); 
int ncWebTreeDispGroup_nolimit_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead); 
int ncWebTreeDispUser_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead); 
int ncWebShowTrendsBytime_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead); 
int ncWebShowServiceDisCx_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead); 
int ncWebShowWebsiteBytime_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead); 
int ncWebShowUserFlowBytime_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowUserFlow_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowWebClassBytime_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncBaseDrNext(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int base_imp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebProcyDrNext(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int webProcy_imp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowMailCtlBt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowMailCtlBmUserBt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncMailCtlDet(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncProtocolDefine(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebCAdownload_driver(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);  //CA

int nc_Sjhl_webvisit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);        //世纪互联  
int nc_Sjhl_hostvisit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_Sjhl_hosttest(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_Sjhl_historylog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_Sjhl_onlineFrm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_Sjhl_starttest(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


//idc
int ncsIdcSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsIdcSetKey(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsIdcRule_maintain(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead,char groupid_idc[15]);
int ncsIdcRlue_am(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsIdcRulesSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsWebTreeIdcrule_nolimit(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsIDC_RulekeyDrNext(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncs_Idc_key_imp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebReplaceco_key(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncsIdcRulekey_maintain(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysDomainProcy(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncSysDomainProcySave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysDomainDept(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncSysDomainDeptSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysDomainUserFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysDomainUserNext(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysUserByOu(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysDomainDeptfirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysUserByGroup(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysUserByCompany(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysUserByDept(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncSysUserByAlluser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncYyzBase_new(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncYyzBas_new_Save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebProcyRuleDel(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebProcyRuleDelAll(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispGroupProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncProncyFpSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebTreeDispYhProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncShowTestSupportLink(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncShowTestSupportSelect(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebSave_Defworktime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncIpSearch_ib(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebMon_cmd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebFile_upload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebVpnStartSet_save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebVpnStartSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebWhzxParSet_save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebWhzxParSet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDataCenter_Par(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncWebDataCenter_ParSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncDnReg(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncDnLogin(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncDnLogout(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
//京东
int nc_jingdong_user(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int nc_jingdong_url(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//等等科技web认证接口
int ncWifjkAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//接收金硅数据
int ncsReqSendJGUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//数据库操作日志及统计
int ncWebDbOperLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowDbBt(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebShowDbBytime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncDbMonType(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebDbMonDispDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncDbMonSaveDefault(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncDbMonTypeDetList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//系统密码策略
int ncUserPasswordProcy(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int ncUserPasswordProcy_Save(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebDeptlist_xm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
//本地验证已经有同样帐号登录，提示选择登录
int ncWebMulogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSiteLoginLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncWebSiteRegLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int ncWebSepCorpByDep(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int ncShowQqAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
	int ncWebQQlogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
	int ncReqTcpAuth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
//汉庭接口	
	int ncHzLogin(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
	int ncHzLogout(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
//链路设置	
	int ncWebLinSet_dev(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
	int ncWebLinGroup_set(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
#endif
