/* ���ʿ���  */
#ifndef __NCCONTROL_DEF__
#define __NCCONTROL_DEF__   1
#define NC_TABLES_ADDDROP          0     /* ����һ����ֹ����      */
#define NC_TABLES_CHECKTABLE       1     /* ���iptables ������   */
#define NC_TABLES_RESETTABLE       2     /* ����Iptables������    */
#define NC_TABLES_REDICTPORT       3     /* �˿��ض���            */
#define NC_TABLES_CANCELREDICT     4     /* ȡ���˿��ض���        */
#define NC_TABLES_DELSIP           5     /* ɾ��ָ��Դ��ַ�Ĺ���  */
#define NC_TABLES_ADDDROPS         6     /* ����һ���������Ľ�ֹ����    */
#define NC_TABLES_REDICTPORTS      7     /* ����һ�����������ض������  */
#define NC_TABLES_ADDDROPP2P       8     /* ����һ��P2P����             */
#define NC_TCWIDTH_SETUSER         11    /* ����һ���û��Ĵ���          */
#define NC_TABLES_NONE             99    /* �Ѿ�����              */ 
/*****************************************/
#define NC_TABLES_SIGNAL           5     /* ��ָ�����̷���Ϣ      */
#define NC_TABLES_REMOTEFILE       6     /* Զ���ļ�ϵͳ          */
#define NC_TABLES_DISPERROR        7
typedef struct IpControHead_s {
    unsigned char  cMod;             /* �޸ı�־, 0 û���޸� 1 �޸�       */
    unsigned char  cCheck;           /* ����ʶ, 0 ����     1 ���ڼ��   */
    unsigned char  cRev[2];          /* ����                              */
    unsigned long  lCheckTime;       /* �����ʱ��                      */
    unsigned long  lCur;             /* ��ǰ���ڲ���                      */
    unsigned long  lDo;              /* ��ǰ���ڴ���                      */
    unsigned long  lMax;             /* �����������¼��              */
} IpControlHead;

typedef struct IpControlInfo_s {
    unsigned long  lSip;             /* ��ʼIP                      */
    unsigned long  lDip;
    unsigned short nDport;
    unsigned char  cProt;            /* Э��                                 */
    unsigned char  cComm;            /* ���� 0--Add 1--Del 2--Flush          */
    unsigned long  lStop;            /* ��ֹʱ�� hhmmss                      */
    unsigned long  lLastTime;        /* ������ʱ��,0 ��ʾ�ü�¼û��        */
} IpControlInfo;

// ���ʿ��Ʒ�ʽ
#define NC_IPT_NETWORK    1
#define NC_IPT_LOCAL      0
#define IPT_COM_SYSTEM    101
#define IPT_COM_IPTABLES  102
#define IPT_COM_TC        103
#define IPT_COM_KEY       "abc^&*543567WE"
typedef struct IptCommand_s {
    unsigned short nFun;              /* ����  0--System����  1--Iptables 2--Tc   */
    unsigned short nLen;              /* ����, ���ܺ��                           */
    unsigned long  lCheck;            /* У��λ,ԭ��У��                          */
} IptCommand;


/* Ip Tables����    */
typedef struct NcIptBuffer_s {
    char Command[132];                // ����
    unsigned long lTime;              // �����ʱ��
} NcIptBuffer;

    
#endif
