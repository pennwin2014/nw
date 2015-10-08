#include <stdio.h>
#include "utoall.h"
#include "ncdef.h"

//int wbs_setDateTime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_proc_log_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_soft_class_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_chat_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_chat_detail_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_RTC_hard_info_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_RTC_base_info_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_time_group_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_time_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_soft_class_search_base(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_soft_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_USB_class_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_USB_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_work_time_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_comp_soft_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_comp_service_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_group_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_comp_disk_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_comp_net_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_group_tree_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_client_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_softUpdateLog_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_softUpdateDetailLog_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_systemLog_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_ssProc_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_setupUpload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_procy_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_procyInfo_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_onLineComp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_sspm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_employs_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_getAccess(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_getLink(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_setScreenHeart(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_getOnlineSreen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_getOnlineComp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int getHardList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int lan_userReg(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_appAdmin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int getApplyInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_getOnlineSreen2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int lan_sspmdwpic(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int lan_compuser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanchgpsd(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int checkrolename(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int checkcompname(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int checkshortname(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


int lanSearchBlackProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanSearchNameList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanDealBlackList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanSearchBlackList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int lanChatSearchBlackProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanChatSearchNameList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanChatDealBlackList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanChatSearchBlackList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


int lanWebStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanPesWebStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);


int lan_scanDownload(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int lan_webfileUp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_searchParamValue(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int lan_searchParamName(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_dealParams(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_SenseCombo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanparamsType(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_procyList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanProcessStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanSearchProcType(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanSearchProc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanSearchCompStatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_mailSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanDownPDFReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanDownCSVReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanIgnoreProc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lan_comp_log_search(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int lanFirstCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanRuntimeCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanUsetimeCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanIdleRateCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanDayCompStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int lanLastingTimeStatics(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int lan_fileoutList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int test_llc(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

//企业android 版
int appEnpLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int appEnpZxjsj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int lan_showstatus(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

int lanWeb_initFun001(utShmHead *psShmHead)
{
    int iReturn;
 //   iReturn = pasSetTcpFunName("wbs_getDateTime",wbs_getDateTime, 0);
 		//日志查看
		iReturn = pasSetTcpFunName("lan_proc_log_search",lan_proc_log_search,0);
		iReturn = pasSetTcpFunName("lan_soft_class_search",lan_soft_class_search,0);
		iReturn = pasSetTcpFunName("lan_mailSearch",lan_mailSearch,0);
		iReturn = pasSetTcpFunName("lan_comp_log_search",lan_comp_log_search,0);
		
		//实时监控
		iReturn = pasSetTcpFunName("lan_chat_search",lan_chat_search,0);
		iReturn = pasSetTcpFunName("lan_chat_detail_search",lan_chat_detail_search,0);
		iReturn = pasSetTcpFunName("lan_ssProc_search",lan_ssProc_search,0);
		iReturn = pasSetTcpFunName("lan_sspm",lan_sspm,0);
		iReturn = pasSetTcpFunName("lan_getAccess",lan_getAccess,0);
		iReturn = pasSetTcpFunName("lan_getLink",lan_getLink,0);
		iReturn = pasSetTcpFunName("lan_getOnlineSreen",lan_getOnlineSreen,0);
		iReturn = pasSetTcpFunName("lan_setScreenHeart",lan_setScreenHeart,0);
		iReturn = pasSetTcpFunName("lan_getOnlineComp",lan_getOnlineComp,0);
		iReturn = pasSetTcpFunName("lan_getOnlineSreen2",lan_getOnlineSreen2,0);
		iReturn = pasSetTcpFunName("lan_sspmdwpic",lan_sspmdwpic,0);
		//计算机信息
		iReturn = pasSetTcpFunName("lan_RTC_hard_info_search",lan_RTC_hard_info_search,0);
		iReturn = pasSetTcpFunName("lan_RTC_base_info_search",lan_RTC_base_info_search,0);
		iReturn = pasSetTcpFunName("lan_comp_soft_search",lan_comp_soft_search,0);
		iReturn = pasSetTcpFunName("lan_comp_service_search",lan_comp_service_search,0);
		iReturn = pasSetTcpFunName("lan_comp_disk_search",lan_comp_disk_search,0);
		iReturn = pasSetTcpFunName("lan_comp_net_search",lan_comp_net_search,0);
		iReturn = pasSetTcpFunName("lan_client_search",lan_client_search,0);
		//基础数据维护
		iReturn = pasSetTcpFunName("lan_time_group_search",lan_time_group_search,0);
		iReturn = pasSetTcpFunName("lan_time_search",lan_time_search,0);
		iReturn = pasSetTcpFunName("lan_work_time_search",lan_work_time_search,0);
		iReturn = pasSetTcpFunName("lan_soft_class_search_base",lan_soft_class_search_base,0);
		iReturn = pasSetTcpFunName("lan_soft_search",lan_soft_search,0);
		iReturn = pasSetTcpFunName("lan_USB_class_search",lan_USB_class_search,0);
		iReturn = pasSetTcpFunName("lan_USB_search",lan_USB_search,0);
		iReturn = pasSetTcpFunName("lan_group_tree_search",lan_group_tree_search,0);
		iReturn = pasSetTcpFunName("getHardList",getHardList,0);
		iReturn = pasSetTcpFunName("lanparamsType",lanparamsType,0);
		iReturn = pasSetTcpFunName("lanIgnoreProc",lanIgnoreProc,0);
		
		//客户端升级
		iReturn = pasSetTcpFunName("lan_softUpdateLog_search",lan_softUpdateLog_search,0);
		iReturn = pasSetTcpFunName("lan_softUpdateDetailLog_search",lan_softUpdateDetailLog_search,0);

		//系统日志
		iReturn = pasSetTcpFunName("lan_systemLog_search",lan_systemLog_search,0);

		//工具
		iReturn = pasSetTcpFunName("lan_group_search",lan_group_search,0);
		iReturn = pasSetTcpFunName("lan_employs_search",lan_employs_search,0);
		
		//升级包生成
		iReturn = pasSetTcpFunName("lan_setupUpload",lan_setupUpload,0);
		iReturn = pasSetTcpFunName("lan_scanDownload",lan_scanDownload,0);
		
		//策略
		iReturn = pasSetTcpFunName("lan_SenseCombo",lan_SenseCombo,0);
		iReturn = pasSetTcpFunName("lan_procyInfo_search",lan_procyInfo_search,0);
		iReturn = pasSetTcpFunName("lan_procyList",lan_procyList,0);
		iReturn = pasSetTcpFunName("lan_dealParams",lan_dealParams,0);
		iReturn = pasSetTcpFunName("lan_searchParamValue",lan_searchParamValue,0);
		iReturn = pasSetTcpFunName("lan_searchParamName",lan_searchParamName,0);
		iReturn = pasSetTcpFunName("lan_fileoutList",lan_fileoutList,0);
		//在线计算机
		iReturn = pasSetTcpFunName("lan_onLineComp",lan_onLineComp,0);		
		
		//用户申请
		iReturn = pasSetTcpFunName("lan_userReg",lan_userReg,0);
		iReturn = pasSetTcpFunName("lan_appAdmin",lan_appAdmin,0);
		iReturn = pasSetTcpFunName("getApplyInfo",getApplyInfo,0);
		
		//公司管理
		iReturn = pasSetTcpFunName("lan_compuser",lan_compuser,0);
		
		iReturn = pasSetTcpFunName("lanchgpsd",lanchgpsd,0);
		iReturn = pasSetTcpFunName("checkrolename",checkrolename,0);
		iReturn = pasSetTcpFunName("checkcompname",checkcompname,0);
		iReturn = pasSetTcpFunName("checkshortname",checkshortname,0);
		
		//企业Android版
		iReturn = pasSetTcpFunName("appEnpLogin",appEnpLogin,0);
		iReturn = pasSetTcpFunName("appEnpZxjsj",appEnpZxjsj,0);
		
		//黑白名单
		iReturn = pasSetTcpFunName("lanSearchBlackProcy",lanSearchBlackProcy,0);
		iReturn = pasSetTcpFunName("lanSearchNameList",lanSearchNameList,0);
		iReturn = pasSetTcpFunName("lanDealBlackList",lanDealBlackList,0);
		iReturn = pasSetTcpFunName("lanSearchBlackList",lanSearchBlackList,0);
		
		//聊天黑白名单
		iReturn = pasSetTcpFunName("lanChatSearchBlackProcy",lanChatSearchBlackProcy,0);
		iReturn = pasSetTcpFunName("lanChatSearchNameList",lanChatSearchNameList,0);
		iReturn = pasSetTcpFunName("lanChatDealBlackList",lanChatDealBlackList,0);
		iReturn = pasSetTcpFunName("lanChatSearchBlackList",lanChatSearchBlackList,0);
		
		//统计
		iReturn = pasSetTcpFunName("lanWebStatics",lanWebStatics,0);
		iReturn = pasSetTcpFunName("lanPesWebStatics",lanPesWebStatics,0);
		iReturn = pasSetTcpFunName("lanProcessStatics",lanProcessStatics,0);
		iReturn = pasSetTcpFunName("lanSearchProcType",lanSearchProcType,0);
		iReturn = pasSetTcpFunName("lanSearchProc",lanSearchProc,0);
		iReturn = pasSetTcpFunName("lanSearchCompStatus",lanSearchCompStatus,0);
		iReturn = pasSetTcpFunName("lanDownPDFReport",lanDownPDFReport,0);
		iReturn = pasSetTcpFunName("lanDownCSVReport",lanDownCSVReport,0);
		iReturn = pasSetTcpFunName("lanFirstCompStatics",lanFirstCompStatics,0);
		iReturn = pasSetTcpFunName("lanRuntimeCompStatics",lanRuntimeCompStatics,0);
		iReturn = pasSetTcpFunName("lanUsetimeCompStatics",lanUsetimeCompStatics,0);
		iReturn = pasSetTcpFunName("lanIdleRateCompStatics",lanIdleRateCompStatics,0);
		iReturn = pasSetTcpFunName("lanDayCompStatics",lanDayCompStatics,0);
		iReturn = pasSetTcpFunName("lanLastingTimeStatics",lanLastingTimeStatics,0);
		
		//文件上传
		iReturn = pasSetTcpFunName("lan_webfileUp",lan_webfileUp,0);
		iReturn=pasSetTcpFunName("test_llc",test_llc,0);
		
		iReturn=pasSetTcpFunName("lan_showstatus",lan_showstatus,0);
		//家庭版函数注册
		//iReturn=pasSetTcpFunName("fm_init_Funname",fm_init_Funname,0);
    return 0;
}
