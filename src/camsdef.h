#ifdef NC_HUAWEI_CAMS
struct OnlineUserListInfo //�����û�����ϸ��Ϣ
{
	long userID;//�û���ţ�ϵͳ��Ψһ��ʶ�û��ı��
	char userName[32];//�ʺ���
	char serviceName[24];//��¼��
	char beginTime[20];//���߿�ʼʱ��
	char nasIp[16];//�����豸IP��ַ
	
	char nasSlot[10];//����ۺ�
	char nasSubSlot[10];//�����Ӳۺ�
	char nasPort[6];//�����豸�˿ں�
	char vlanId[8];//VLAN ID
	char frameIp[16];//�û�IP��ַ
	
  char macAddress[20];//�û�MAC��ַ
  char acctSessionId[10];//�ƷѻỰ��ʶ
  char proxyType[8];//��������
  char proxyUser[32];//�����û���
  char proxyIpS[16];//��������ַ
  
  char proxyIpD[16];//����ת����ַ
  int safeState;//��ȫ״̬
  char clientVersion[10];//�ͻ��˰汾
  char clientLang[10];//�ͻ�������
  char acl[1024];//ACL
  
  char dept[64];//�����Ǹ�����Ϣ, Ϊ�ַ���.
 
};
#endif

#ifdef NC_HUAWEI_IMC
struct OnlineUserListInfo //�����û�����ϸ��Ϣ
{
        long userID;//�û���ţ�ϵͳ��Ψһ��ʶ�û��ı��
        
        
        char accountName[32];
        char computerName[32];
        char fullName[32];
        char loginName[32];
        char serviceName[32];//��������
        
        
        char macAddress[20];//�û�MAC��ַ
        char dept[64];//�����Ǹ�����Ϣ, Ϊ�ַ���.
        
};
#endif

typedef struct ncSynUserInfo_s {
    unsigned long  ip;          /* IP��ַ                       */
    unsigned char  mac[6];      /* Mac��ַ                      */
    unsigned char  flags;       /* ��¼��Դ 1--�̶����� 8--Wins 9--DHCP */
    unsigned char  status;      /* ״̬ 0-������  1-����                */
    unsigned char  uname[32];   /* �û���      */
    unsigned char  cname[32];   /* ������      */
    unsigned char  dept[32];    /* ����      */
    unsigned long  lLastTime;   /* ������ʱ��  */
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
