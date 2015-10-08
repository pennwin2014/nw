#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pasdb.h"
#include "utoplt01.h"
#include "nwdef.h"
#include "ncdef.h"

#define     PRO_CENTER_COMMANDID_RESTART        0x00000800      //重启客户端机器
#define     PRO_CENTER_COMMANDID_CLOSE          0x00001000      //关闭客户端机器
#define     PRO_CENTER_COMMANDID_UNLOAD         0x00002000      //卸载客户端


int lan_onLineComp(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
  char compid_in[16] = "";
  char deal_in[8] = "";
	
	utMsgGetSomeNVar(psMsgHead,2,
		"compid",UT_TYPE_STRING,sizeof(compid_in)-1,compid_in,
		"deal",UT_TYPE_STRING,sizeof(deal_in)-1,deal_in);
		
		unsigned int compid = strtoul(compid_in,NULL,10);
		nwCompIp *psCompIp = NULL;
		psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
		
		
  	utPltDbHead *psDbHead = utPltInitDb();
		
	 char msg[256] = "";
	 char *compname = findCompNameById(compid);
		switch(atoi(deal_in)){
			case 0:
				if(psCompIp){
					psCompIp->lCtrl|=PRO_CENTER_COMMANDID_CLOSE;
					snprintf(msg,sizeof(msg),"关闭了电脑[%s]",compname);
				}
				break;
			case 1:
				if(psCompIp){
					psCompIp->lCtrl|=PRO_CENTER_COMMANDID_RESTART;
					snprintf(msg,sizeof(msg),"重启了电脑[%s]",compname);
				}
				break;
			case 2:
				if(psCompIp){
					psCompIp->lCtrl|=PRO_CENTER_COMMANDID_UNLOAD;
					snprintf(msg,sizeof(msg),"卸载了电脑[%s]上的客户端",compname);
					pasDbExecSqlF("update nwcompute set status=2 where compid=%s",compid_in);
				}
				break;
			default: ;			
		}
		
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"v8/smm/lan_submit_success.htm");
	 writeSysLog(psMsgHead,"06","Success",msg);
	return 0;
}