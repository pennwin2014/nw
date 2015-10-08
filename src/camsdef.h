#ifdef NC_HUAWEI_CAMS
struct OnlineUserListInfo //在线用户的详细信息
{
	long userID;//用户编号，系统中唯一标识用户的编号
	char userName[32];//帐号名
	char serviceName[24];//登录名
	char beginTime[20];//上线开始时间
	char nasIp[16];//接入设备IP地址
	
	char nasSlot[10];//接入槽号
	char nasSubSlot[10];//接入子槽号
	char nasPort[6];//接入设备端口号
	char vlanId[8];//VLAN ID
	char frameIp[16];//用户IP地址
	
  char macAddress[20];//用户MAC地址
  char acctSessionId[10];//计费会话标识
  char proxyType[8];//代理类型
  char proxyUser[32];//代理用户名
  char proxyIpS[16];//代理接入地址
  
  char proxyIpD[16];//代理转出地址
  int safeState;//安全状态
  char clientVersion[10];//客户端版本
  char clientLang[10];//客户端语言
  char acl[1024];//ACL
  
  char dept[64];//部门是附加信息, 为字符型.
 
};
#endif

#ifdef NC_HUAWEI_IMC
struct OnlineUserListInfo //在线用户的详细信息
{
        long userID;//用户编号，系统中唯一标识用户的编号
        
        
        char accountName[32];
        char computerName[32];
        char fullName[32];
        char loginName[32];
        char serviceName[32];//中文姓名
        
        
        char macAddress[20];//用户MAC地址
        char dept[64];//部门是附加信息, 为字符型.
        
};
#endif

typedef struct ncSynUserInfo_s {
    unsigned long  ip;          /* IP地址                       */
    unsigned char  mac[6];      /* Mac地址                      */
    unsigned char  flags;       /* 记录来源 1--固定不变 8--Wins 9--DHCP */
    unsigned char  status;      /* 状态 0-不在线  1-在线                */
    unsigned char  uname[32];   /* 用户名      */
    unsigned char  cname[32];   /* 主机名      */
    unsigned char  dept[32];    /* 部门      */
    unsigned long  lLastTime;   /* 最后操作时间  */
} ncSynUserInfo;

#ifdef _cplusplus
#include "soapCamsWebServiceSoapBindingProxy.h"
extern "C"
{
#endif
/*
int (*ws_login) (char *userName, char *password);
int (*ws_logout)();
int (*ws_queryOnlineUserList) (struct OnlineUserListInfo r[], int *iLen);
int (*ws_queryOnlineUserListByIp) (char *ip, struct OnlineUserListInfo r[], int *iLen);
*/

#ifdef _cplusplus
}
#endif
