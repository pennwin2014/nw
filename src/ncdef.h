/* ProEIM     ��Ȩ����: �Ϻ���������Ϣ�������޹�˾ 2000-2013
   
   2013/01/12      �޸� ncLimGroup Ϊ�༶����          
   2011/02/10        ������                            
   2009/02/16 Review �������ݽṹ                       
   2009/01/12 �� ncLimSummer ���� qospolid,mailctlid    
   #include  "ncshmdebug.h"   
   
*/
#include <regex.h>
#ifndef __NETCHARGDEF__
#define __NETCHARGDEF__
#define ncUtlMax(a,b) ((a) > (b) ? (a) : (b))
#define ncUtlMin(a,b) ((a) < (b) ? (a) : (b))
    
#include "ncbill.h"
#define   NC_ERROR_FILE          "../log/ncerror.log"
#define   NC_LOG_FILE            "../log/nclog.log"
#define   NC_VERSION             "V7.50.235"                      /* �汾��    */
#define   NC_RELEASE             "2013/08/07"                     /* ��������  */
#define   NC_MARK_READY          1   /* NC ������Ϣ   0--δ׼��   1--׼������  */


#define   NC_MARK_NOTREADY       0  /* δ׼��    */
#define   NC_MARK_ISREADY        1  /* ��׼��    */

#define   NC_MAX_PROCESS        16   /* ����������  */
#define   NC_MAX_CONTROL        32   /* �����Ʊ���  */
#define   NC_MAX_ONTIMEDO       16   /* ���ʱ����  */
#define   NC_DEFAULT_IP_CHECK   20
#define   NC_DEFAULT_IP_TIMEOUT 30
#define   NC_MAX_USERNUM        257
#define   NC_MAX_IPPKG          9999
#define   NC_MAX_HTTPNUM        999
#define   NC_MAX_LIMITED        100
#define   NC_MAX_MONITOR        100
#define   NC_MAX_INTERNAL       100
#define   NC_MAX_LASTIP         1000
#define   NC_MAX_DATASIZE       1000000000L     // ÿ�������ļ�����󳤶�

#define   NC_BANDWIDTH_NONE       0
#define   NC_BANDWIDTH_TC         1
#define   NC_BANDWIDTH_NSA        2

#define   NC_CONTENT_GET      10
#define   NC_CONTENT_POST1    11
#define   NC_CONTENT_POST2    12
#define   NC_CONTENT_POST3    13
#define   NC_CONTENT_SMTP1    21
#define   NC_CONTENT_SMTP2    22
#define   NC_CONTENT_POP3     23
#define   NC_MAX_NOMONWEB     50
#define   NC_PF_UNUSED        0
#define   NC_PF_INUSED        1

#define   NC_SHM_MAXLNK     200 /* �����ڴ����������������, ���ֵ�������NC_LNK�ж��������ֵ   */
/*  �����ڴ�����������ֵ                           */
#define   NC_LNK_THIRDAUTH  25  /* ��������֤���û�  */
#define   NC_LNK_ALLUSER    26  /* ȫ���û���Ϣ      */
#define   NC_LNK_MAILCTL    27  /* �ʼ�����          */
#define   NC_LNK_CONTROL    28  /* ���ʿ���          */
#define   NC_LNK_IPFLOW     29  /* ������            */
#define   NC_LNK_MAIL       30  /* �ʼ����ݰ�        */
#define   NC_LNK_IPPKG      31  /* IP���ݰ�          */
#define   NC_LNK_WEB        32  /* http���ݰ�        */
#define   NC_LNK_IPLIMIT    33  /* ����ʹ�õ�IP��ַ  */
#define   NC_LNK_NETWORK    34  /* Ҫ��ص�IP��ַ  Add 2002/9/6 By Liyunming Replace
                                         NC_LNK_MONITOR NC_LNK_INTERNAL */
#define NC_LNK_PKGLOG       35  /* ����װ�ؿ���  */
#define NC_LNK_USERINFO     36  /* �û���Ϣ          */
#define NC_LNK_SERVICE      37  /* ������֪�Ķ˿�    */
#define NC_LNK_MMONFILTER   38  /* �ʼ���ط������      */
#define NC_LNK_WEBLIMIT     39  /* Web��������       */
#define NC_LNK_IPLIST       40  /* IP��ַ�б�        */
#define NC_LNK_IPPOL        41  /* ������ʲ���      */
#define NC_LNK_WEBLIST      42  /* Web��ַ�б�        */
#define NC_LNK_IPPOLINDEX   43  /* ������ʲ��Ե�����  */
#define NC_LNK_GROUPINFO    44  /* ������Ϣ           */
#define NC_LNK_SUMMARY      45  /* ������Ϣ           */
#define NC_LNK_ONTIME       46  /* ��ʱ����           */
#define NC_LNK_PROCESS      47  /* ���̹���           */
#define NC_LNK_WEBPOLINDEX  48  /* ��վ���ʲ���       */
#define NC_LNK_WEBPOL       49  /* ��վ���ʲ���       */
#define NC_LNK_NTAUTH       50  /* NT��֤             */
#define NC_LNK_FMONFILTER   51  /* ����ع���       */
#define NC_LNK_MCTLFILTER   52  /* �����ʼ��������   */
#define NC_LNK_MAILPROXY    53  /* �շ��ʼ���Ŀ��IP��ַ  */
#define FIRE_LNK_DEFINE     54  /* �����б�            */
#define FIRE_LNK_TABLES     55  /* ����               */
#define FIRE_LNK_PLATE      56     /* ģ��      */
#define FIRE_LNK_SUMMARY    57     /* ������Ϣ  */
#define NC_LNK_ICUSER       58     /* IC���û�  */
#define NC_LNK_HTTPPROXY    59     /* http proxy    */
#define NC_LNK_POP3PROXY    60     /* pop3 proxy    */
#define NC_LNK_NOMONWEB     61     /* ���ص���ַ  */
#define NC_LNK_MSNPKGBUF    62     /* MSN���ݽ���������,��̬����     */
#define NC_LNK_MSNUSRBUF    63     /* MSN�����û�������,Hash��       */
#define NC_LNK_CONTROLBUF   64     /* ���ʿ��ƻ�����                 */
#define NC_LNK_BANDWIDTH    65     /* �������                       */
#define NC_LNK_PKGBUF       66     /* ���ݰ�������                   */
#define NC_LNK_CONTBUF      67     /* ��һ����������ݻ�����         */
#define NC_LNK_IPUSER       68     /* �û���IP����                   */
#define NC_LNK_DNSCASH      69     /* DNS Cash                       */
#define NC_LNK_HTTPCACHE    70     /* http cache                              */
#define NC_LNK_HTTPCACHEC   71     /* http cache content                      */
#define NC_LNK_ADMIPLIST    72     /* ����ԱIP��ַ, ������¼��¼ʧ�ܵ�IP��ַ  */
#define NC_LNK_IPEXCEPT     73     /* �����й��������IP��ַ��                */
#define NC_LNK_IPCONTACT    74     /* TCP������Ϣ                             */
#define NC_LNK_LANG         75    /* ���Զ��ձ�   */
#define NC_LNK_PKGSEQ       76    /* ���кŶ��ձ�, Hash��  */
#define NC_LNK_NCSRVINFO    77    /* �������ĵ��й���Ϣ    */
#define NC_LNK_NCPOLINFO    78    /* ���ʿ��Ʋ�������      */
#define NC_LNK_MACLIST      79    /* ���ص�MAC��ַ       */
#define NC_LNK_BILLTYPE     80    /* �ƷѲ���              */
#define NC_LNK_BILLRATE     81    /* �Ż�����              */
#define NC_LNK_BILLTIME     82    /* ʱ�����              */
#define NC_LNK_BILLFLOW     83    /* ��������              */
#define NC_LNK_PROXYIP      84    /* Proxy�ĵ�ַ�б�       */
#define NC_LNK_LOCK         85    /* ����Ϣ                */
#define NC_LNK_WEBNAME      86    /* ��ַ���Ӧ������      */
#define NC_LNK_COMPUTE      87    /* ����������б�        */
#define NC_LNK_DEVINFO      88    /* �豸��Ϣ����          */
#define NC_LNK_IPMACLIST    89    /* IP��ַ��MAC��ַ���ձ�          */
#define NC_LNK_MACIPLIST    90    /* MAC��ַ��IP��ַ������������ձ�*/
#define NC_LNK_DBSYNCINFO   91    /* ����ͬ��������Ϣ               */
#define NC_LNK_ONLINECOMP   92    /* ���߼�������                 */
#define NC_LNK_NCMAILLIST   93    /* �ʼ���ַ�ڰ�����               */
#define NC_LNK_GROUPIPLST   94    /* ȱʡIP�Ͳ��Ŷ��ձ�             */
#define NC_LNK_CLTSENDBUF   95    /* �ͻ���ʵʱ���ݷ��ͻ�����       */
#define NC_LNK_CLTFILTER    96    /* �ͻ��˸��ֹ��˹���             */
#define NC_LNK_INFOLIST     97    /* �����б�                       */
#define NC_LNK_WARNINFO     98    /* �澯��Ϣ                       */
#define NC_LNK_HASHDATA     99    /* Hash����                       */
#define NC_LNK_SWITCHINFO   100   /* ��������Ϣ                     */
#define NC_LNK_SWITCHPORT   101   /* �˿���Ϣ                       */
#define NC_LNK_NETSYSINFO   102   /* ����������Ϣ                   */
#define NC_LNK_NETCAPDEV    103   /* ץ����Ϣ                       */
#define NC_LNK_CASECTL      104   /* ����������Ϣ                   */
#define NC_LNK_CASEINDEX    105   /* ��������                       */
#define NC_LNK_BLACKWHITE   106   /* �ڰ�����                       */
#define NC_LNK_VIDLIST      107   /* ��������б�                   */
#define NC_LNK_VIDSET       108   /* ������ݼ�                     */
#define NC_LNK_LIMVID       109   /* ������ݵĿ���                 */
#define NC_LNK_DATEDESC     110   /* ���ڵ�����                     */
#define NC_LNK_VIDDROP      111   /* ���õ��������                 */
#define NC_LNK_SYNONUSER    112   /* ͬ���������û�                 */
#define NC_LNK_SENSCONT     113   /* �������ݼ����                 */
#define NC_LNK_ADUSER       114   /* Ad�û�״̬                     */
#define NC_LNK_HOTELINFO    115   /* ���ù���Ϣ                     */
#define NC_LNK_SNMPSWITCH   116   /* Snmp��������Ϣ                 */
#define NC_LNK_PROCCTL      117   /* �ͻ��˽��̿��Ʋ���             */
#define NC_LNK_NWWATCH      118   /* �ͻ��˼�ز���                 */
#define NC_LNK_DNSNAMESERVICE  119    /* ������������               */
#define NC_LNK_DNSIPSERVICE    120    /* Ip��ַ������ձ�           */
#define NC_LNK_NSACOMBUF       121    /* Nsa����ݻ���            */
#define NC_LNK_NWJOIN          122   /* �ͻ���׼�����               */
#define NC_LNK_QOSID           123   /* QOS Link                     */

#define NC_LNK_POSTACC         127   /* ���˺Ųɼ�����             */
#define NC_LNK_POSTFILTER      128   /* �����˹���                 */
#define NC_LNK_NWDATEPROCY     129   /* �������ڿ��Ʋ���*/
#define NC_LNK_DICTIONARY      130   /* �����ֵ�                      */
#define NC_LNK_SERVICEPAT      131   /* ��������ģʽ                  */
#define NC_LNK_PORTSERVICE     132   /* �˿ڷ�����ձ�                */
#define NC_LNK_NOMONLIST       133   /* ����IP�б�                  */
#define NC_LNK_COMMANDBUF      134   /* ���̿����������            */
#define NC_LNK_PKGFLOW         135   /* ������Ϣ                      */
#define NC_LNK_POSTSYN         136   /* ��ͬ����Ϣ                  */
#define NC_LNK_BLANCE          137   /* �����������                  */
#define NC_LNK_GROUPFLOW       138   /* ���ż�ʱ����������������������*/
#define NC_LNK_CONTEXT         139   /* ������Ʊ������HASH��������  */
#define NC_LNK_CLUSERVER       140   /* ˫��ϵͳ���������            */
#define NC_LNK_CLUCLIENT       141   /* ˫��ϵͳ���������            */

#define NC_LNK_ID2POSITION     142   /* �˿ں�λ�ö��ձ�              */
#define NC_LNK_POSITION2NAME   143   /* λ�ú��ÿͶ��ձ�              */
#define NC_LNK_QQNUMBER        144   /* Qq������Ϣ�ɼ�                */
#define NC_LNK_HTTPHASH        145   /* ���Http��Hash��              */
#define NC_LNK_FSSERVERLNK     146   /* �ļ�����������                */
#define NC_LNK_DUPLICETECONT   147   /* �ظ�����                      */
#define NC_LNK_DNSHOSTIP       148   /* Dns Buffer   ncdns.h          */
#define NC_LNK_DNSIPHOST       149   /* Dns Buffer   ncdns.h          */
#define NC_LNK_DYNAMICURL      150   /* ��̬URL                       */
#define NC_LNK_WARNACCOUNT     151   /* �ص��ע�ʺ�                  */
#define NC_LNK_CAPURL          152   /* �ɼ���URL��Ϣ                  */
#define NC_LNK_CAPURLSTAT      153   /* �ɼ���UR״̬L��Ϣ              */
#define NC_LNK_QQPKGBUF        154   /* �����ʱ���ݰ�                 */
#define NC_LNK_DBMONSESSION    155   /* ���ݿ�������Ϣ                 */
#define NC_LNK_GROUPDEPT       156   /* ��λ���Ŷ��ձ�                 */
#define NC_LNK_MWDUSER         157   /*����������û������            */
#define NC_LNK_DOPOSTTHREAD    158   /* ����������б�               */
#define NC_LNK_SMSMOBNO         159   /*����è��֤�ֻ����б�            */


#define NC_SHA_POSTDO          20
/* �첽����������ļ�                    */
#define NC_ERR_FILEMARK     "/tmp/ncerrormark.tmp"

/* ���ʿ��ƵĿ��Ʒ�ʽ           */
#define NC_IPCONTROL_BY_NONE          0
#define NC_IPCONTROL_BY_ICMP          1
#define NC_IPCONTROL_BY_IPT           2
#define NC_IPCONTROL_BY_TCP           3
#define NC_IPCONTROL_BY_NETWORK       4
#define NC_IPCONTROL_BY_NETWORKP2P    5
#define NC_IPCONTROL_BY_NSA           6
#define NC_IPCONTROL_BY_NOCTL         9

/* ����  0--9999 Ϊϵͳʹ��
         10000 �Ժ�Ϊ�Զ���
 */
#define NC_SERVICE_OTHER          0        /* δ֪����           */
#define NC_SERVICE_TELNET         12001    /* Telnet             */
#define NC_SERVICE_GAME           2000     /* ��Ϸ               */
#define NC_SERVICE_DHCP           101      /* DHCP����            */
#define NC_SERVICE_HTTP           103      /* Web����             */
#define NC_SERVICE_RADIUS         114      /* RadiusЭ��          */
#define NC_SERVICE_DNS            117      /* DNSЭ��             */
#define NC_SERVICE_HTTPUPLOAD     4006     /* Web����             */
#define NC_SERVICE_HTTPPROXY      3001     /* Http����           */
#define NC_SERVICE_HTTP1          3000     /* Web����            */
#define NC_SERVICE_SMTP           13001    /* SMTP����           */
#define NC_SERVICE_POP            13002    /* POP3����           */
#define NC_SERVICE_QQ             1001     /* QQ                 */
#define NC_SERVICE_VIDEO          7        /* ��Ƶ               */
#define NC_SERVICE_P2P            8        /* P2P����            */
#define NC_SERVICE_STOCK          9        /* ��Ʊ               */
#define NC_SERVICE_MSN            1002     /* MSN                */
#define NC_SERVICE_ICQ            1008     /* ICQ                */
#define NC_SERVICE_MMS            5016     /* MMS                */
#define NC_SERVICE_FTP            4002     /* FTP                */
#define NC_SERVICE_P2P_BASE       8000
#define NC_SERVICE_SSH            12002    /* ssh                */
#define NC_SERVICE_MSNP2P         29000    /* �ڲ�ʹ�� */ 
#define NC_SERVICE_HTTPS          116 
#define NC_SERVICE_QQFILE         4052     /* QQ���ļ�   */
#define NC_SERVICE_MSNFILE        4053     /* Msn�ļ�   */
#define NC_SERVICE_DNS            117
#define NC_SERVICE_WEBMAIL        13008  
#define NC_SERVICE_FLASH          146   /* Flash��Ƶ        */
#define NC_SERVICE_CKPAUTH        88    /* Check Point ��֤ */
#define NC_SERVICE_YHMSG          1006  /* �Ż�ͨ     */
#define NC_SERVICE_WYPP           1007  /* ��������   */
#define NC_SERVICE_XLUC           1005  /* ����UC     */
#define NC_SERVICE_SOUQ           1010  /* ��Q        */
#define NC_SERVICE_SKYPE          1004  /* SKYPE      */
#define NC_SERVICE_FETION         1012  /* ����       */

#define NC_SERVICE_LZGAME         2001  /* ������Ϸ  */
#define NC_SERVICE_GAMEONLINE     2002  /* �й���Ϸ����  */
#define NC_SERVICE_CQGAME         2003  /* ����  */
#define NC_SERVICE_BFGAME         2004  /* �߷�  */
#define NC_SERVICE_QQGAME         2201  /* QQ��Ϸ        */
#define NC_SERVICE_DFGAME         2202  /* �ط���Ϸ����  */

#define NC_SERVICE_PXYLSTD        3003  /* ��ɫͨ��      */
#define NC_SERVICE_SQUID          3006  /* Squid ����    */


#define NC_WEBCLASS_VIDEO         5019  /* ������Ƶ��ַ���  */
#define NC_SERVICE_RTSP           5016


/* 2009/05/09 ����������  */
#define NC_P2P_QQLIVE             7001  /* QQlive     */
#define NC_P2P_PPLIVE             7002  /* PPlive|UUsee     */
#define NC_P2P_PPSTREAM           7003  /* PPStream   */
#define NC_P2P_FEIDIAN            7004  /* �е�   */
#define NC_P2P_UUSEE              7005  /* UUsee      */
#define NC_P2P_LEIKE              7006  /* �ڿ�      */
#define NC_P2P_MYSEE              7007  /* MySee      */
#define NC_P2P_QINYULE            7008  /* ������      */
#define NC_P2P_BBsee              7009  /* BBsee      */
#define NC_P2P_REALMEDIA          7010  /* RealMedia  */
#define NC_P2P_WINMEDIA           7011  /* WinMedia   */
#define NC_P2P_QVOD               7012  /* �첥       */
#define NC_P2P_SOPCAST            7013  /* Sopcast    */
#define NC_P2P_EDK                8001  /* edonkey  */
#define NC_P2P_GNU                8002  /* Gnutella  */
#define NC_P2P_KAZAA              8003  /* KaZaA  */
#define NC_P2P_DC                 8004  /* DirectConnect  */
#define NC_P2P_BIT                8005  /* BitTorrent  */
#define NC_P2P_ARES               8006  /* Ares  */
#define NC_P2P_SOUL               8007  /* SoulSeek  */
#define NC_P2P_WinMX              8008  /* WinMX  */
#define NC_P2P_MUTE               8009  /* mute  */
#define NC_P2P_XDCC               8010  /* xdcc  */
#define NC_P2P_WASTE              8011  /* waste  */
#define NC_P2P_APPLE              8012  /* appleJuice */
#define NC_P2P_XUNLEI             8013  /* Ѹ��       */
#define NC_P2P_PPDDT              8014  /* PP���ͨ     */
#define NC_P2P_KUGOU              6002  /* KuGou       */
#define NC_P2P_EMUTE              8001  /* eMute       */
#define NC_P2P_POCO               8018  /* POCO        */
#define NC_P2P_BEIDUXB            8019  /* �ٶ��°�    */
#define NC_P2P_QQCYCLONE          8020  /* QQ �������� */

#define NC_FILE_HTTP              4001  /* Http����  */
#define NC_FILE_FTP               4002  /* FTP����  */

#define NC_STOCK_ZHESHAN          9015  /* ����֤ȯ  */
#define NC_STOCK_GUOXIN           9014  /* ����֤ȯ  */
#define NC_STOCK_FANGZHEN         9037  /* ����֤ȯ  */

#define NC_SERVICE_SYBASE         16001
#define NC_SERVICE_ORACLE         16002
#define NC_SERVICE_MSSQL          16003
#define NC_SERVICE_MYSQL          16004
#define NC_SERVICE_DB2            16005
#define NC_SERVICE_INFORMIX       16006


#define NC_SERVICE_BBS            20010 /* ��̳�˺�         */

#define NC_SERVICE_ACCOUNT        30000 /* ������           */
#define NC_SERVICE_WHITE          33333 /* ������           */
#define NC_SERVICE_BLACK          33334 /* ������           */
#define NC_SERVICE_MUSTLOGIN      33335 /* �����¼�ķ���    */

#define NC_DUPLICE_IM         1
#define NC_DUPLICE_FORM       2
#define NC_DUPLICE_MAIL       3
#define NC_DUPLICE_SEARCH     4
#define NC_DUPLICE_HTTPSNAP   5
#define NC_DUPLICE_IMLOGIN    6
#define NC_DUPLICE_IMLOGOUT   7
#define NC_DUPLICE_IPIN       8




#define NC_HTTP_GET       1
#define NC_HTTP_POST      2
#define NC_HTTP_RTSP      3
#define NC_HTTP_MMS       4
#define NC_HTTP_HTML      5
#define NC_HTTP_HTTPS     6
#define NC_HTTP_FTP       7


#define NC_QQ_LOGIN       1
#define NC_QQ_LOGOUT      2
#define NC_QQ_SENDMSG     3

#define NC_IM_LOGIN       1     /* ��¼  */
#define NC_IM_LOGOUT      2     /* �˳�  */
#define NC_IM_SENDMSG     3     /* ������Ϣ  */
#define NC_IM_SENDFILE    4     /* �����ļ�  */
#define NC_IM_AUDIT       7     /* ��Ƶ      */
#define NC_IM_VIDEO       8     /* ��Ƶ      */
#define NC_IM_RECVMSG     6     /* ������Ϣ  */
#define NC_IM_RECVFILE    5     /* �����ļ�  */


/* nMark �ĺ���  */
#define NC_FTP_PORTINFO   141
#define NC_FTP_PASVINFO   142

#define NC_FTP_PORTMARK   143   /* ����ͨ��IP����  */
#define NC_FTP_PASVMARK   144   /* ����ͨ��IP����  */
#define NC_FTP_COMMMARK   145   /* ����ͨ��TCP����  */

/* cFun �ĺ���     */
#define NC_FTP_COMM       146     /* ����      */ 
#define NC_FTP_DATA       147     /* һ������  */
#define NC_FTP_PORTDOWN   148     /* �����ļ�  */
#define NC_FTP_PORTUP     149     /* �ϴ��ļ�  */
#define NC_FTP_PASVDOWN   150     /* �����ļ�  */
#define NC_FTP_PASVUP     151     /* �ϴ��ļ�  */

#define NC_FTP_SAVEUP     1401   /* �����ϴ�  */
#define NC_FTP_SAVEDOWN   1402   /* ��������  */
#define NC_FTP_SAVEALL    1400   /* ����ȫ��  */



/* ���뷽ʽ    */
#define NC_ACCESS_BRIDGE  0      // ����
#define NC_ACCESS_GATEWAY 1      // ����
#define NC_ACCESS_BYPASS  2      // ��·
#define NC_ACCESS_NSA     3      // ��·

/* �ַ���  */
#define NC_CCODE_GB       0     /* gbk bg2312 */
#define NC_CCODE_BIG5     1     /* big5       */
#define NC_CCODE_UTF8     2     /* utf-8      */
#define NC_CCODE_JP       3     /* iso-2022-jp */
#define NC_CCODE_AUTO     0xff  /* �Զ�         */

#define NC_PROTOCOL_ICMP  1
#define NC_PROTOCOL_IGMP  2
#define NC_PROTOCOL_TCP   6
#define NC_PROTOCOL_UDP   17

#define NC_PROTOCOL_ARP   125
#define NC_PROTOCOL_RARP  126

/*   Define the Debug Level   */
#define NC_DEBUG_SHOWTCP      1
#define NC_DEBUG_SHOWCONTROL  2
#define NC_DEBUG_SHOWUDP      3
#define NC_DEBUG_SHOWICMP     4
#define NC_DEBUG_SHOWIGMP     5
#define NC_DEBUG_SHOWCHECKMAC 6
#define NC_DEBUG_SHOWSMTP     7
#define NC_DEBUG_SHOWPOP      8
#define NC_DEBUG_NTAUTH       9
#define NC_DEBUG_UPLOAD       10
#define NC_DEBUG_IPCONTROL    11
#define NC_DEBUG_MAILAUDIT    12     /* �ʼ����  */
#define NC_DEBUG_MAILNON      13     /* �ʼ����  */
#define NC_DEBUG_TIMECTL      14     /* ʱ�����  */
#define NC_DEBUG_MSGINFO      15     /* ��ʾ���ݰ�   */
#define NC_DEBUG_CHECKNETIP   16     /* ���������� */
#define NC_DEBUG_THIRDAUTH    17     /* ��������֤   */
#define NC_DEBUG_SAVEIPBUF    18     /* ��ʱ���滺�����е�IP��Ϣ  */

/* �йش��󼶱�       */
#define NC_LOG_SUCESS          PAS_LOG_SUCESS      /* һ����־         */
#define NC_LOG_ERROR           PAS_LOG_ERROR       /* ���ش���         */
#define NC_LOG_WARN            PAS_LOG_WARN       /* ������Ϣ         */
#define NC_LOG_EVENT           PAS_LOG_EVENT1     /* ��Ҫ�¼�         */
#define NC_LOG_DEBUG           PAS_LOG_DEBUG      /* ������Ϣ         */

#define NC_DEBUG_UPDATE       119     /* ���������ĵ�����Ϣ    */
#define NC_DEBUG_MAILCTL      120     /* �ʼ����              */

/* ��ֹԭ�����    */
#define NC_DENY_NONE           0    /* ֱ�ӽ�ֹ    */
#define NC_DENY_NORMAL         1    /* һ�����    */
#define NC_DENY_WEBPOL         2    /* ��ַ�����  */
#define NC_DENY_SERVICE        3    /* ��������    */
#define NC_DENY_SERVICEPOL     4    /* �������    */
#define NC_DENY_WEBURL         5    /* ��ַ����    */

#define NC_EXCEPT_FORBIT        1    /* һ���ֹ   */
#define NC_EXCEPT_IPPOL         2    /* IP��ַ     */
#define NC_EXCEPT_WEBPOL        3    /* ��ַ����� */
#define NC_EXCEPT_IPNET         4    /* IP��ֹ���� */
#define NC_EXCEPT_IPSERVICE     5    /* IP��������  */
#define NC_EXCEPT_IPPORT        6    /* ��ֹ����IP��ַ�Ͷ˿�  */
#define NC_EXCEPT_WEBURL        7    /* ��ֹ����ָ����URL */

/*���cStatus����, 0 ����,δ֪
                   1 �����������
                  >5 ��ֹ����
 */
#define NC_USER_NOUSE           0        /* Nouse                  */
#define NC_USER_NORMAL          1        /* ���ȫ��               */
#define NC_USER_NOMON           2        /* �û�����             */
#define NC_USER_MONIP           3        /* ���IP��ַ             */
#define NC_USER_MONHTTP         4        /* ���Http               */

#define NC_USER_FORBIDCTL       5        /* ���ʿ��Ʊ���ֹ         */
#define NC_USER_FORBIDLIMIT     6        /* �û����ʳ�������       */
#define NC_USER_FORBIDNOUSER    7        /* �û�������             */
#define NC_USER_FORBIDMACERR    8        /* MAC��ַ��һ��          */
#define NC_USER_USRFORBID       9        /* �û�����               */
#define NC_USER_FORBIDAUTH      10       /* �û���֤               */
#define NC_USER_FORBIDNAMEERR   11       /* ���������һ��         */
#define NC_USER_FORBIDIPERR     12       /* MAC��ַ��һ��          */

/* ��ϼ���  */
#define NC_FORBID_USER       1        /* �û���ֹ���ʻ�����  */
#define NC_FORBID_SERVICE    2        /* �û���ֹ����ĳ������ */
#define NC_FORBID_IP         3        /* ��ֹ����ĳ��IP��ַ   */
#define NC_FORBID_IPPORT     4        /* ��ֹ����ĳ��IP��PORT */
#define NC_FORBID_IPCONTRACT 5        /* ���ĳ������         */

/* ������ʽ    */
#define NC_DEF_IPADDRESS    1        /* ����IP��ַ����      */
#define NC_DEF_COMPUTER     2        /* ���ݼ������������  */
#define NC_DEF_MACADDRESS   3        /* Mac��ַ����         */
#define NC_DEF_VLANID       4        /* ����VlanID����      */
#define NC_DEF_RAND         5        /* �������            */
#define NC_DEF_PORTNAME     6        /* �˿�����            */

/* �ʼ�����     */
#define NC_MCTL_QMAIL       1        /* Qmail ת��          */
#define NC_MCTL_SMTPPROXY   2        /* Smtp Proxy          */
#define NC_MCTL_POP3PROXY   3        /* Pop3 Proxy          */

#define NC_MCTL_NONE       0     
#define NC_MCTL_SEND       1         /* ֱ�ӷ���            */
#define NC_MCTL_REJECT     2         /* �ܾ�����            */
#define NC_MCTL_FORWARD    3         /* ת����ָ������      */
#define NC_MCTL_WAIT       4         /* �ȴ����            */

#define NC_NAT_NATIP         1        /* Ip��ַӳ��   */
#define NC_NAT_NATPORT       2        /* �˿��ض���   */
#define NC_NAT_NATSNAT       3        /* Դ��ַαװ   */
#define NC_NAT_NATNAP        4        /* Դ��ַαװ   */

#define NC_NAT_ROUTE         8        /* ·������     */

#define NC_DBCHK_EXTENDED    1
#define NC_DBCHK_CHANGED     2
#define NC_DBCHK_FAST        3

#define NC_IPCONTROL_ID       918123123L
#define NC_TABLES_REMOTEFILE       6 

#define NC_SIGNAL_MAX        8        /* ����ź���          */
#define NC_SIGNAL_RESETPKG   0        /* ���³�ʼ�����ݰ�    */
#define NC_SIGNAL_REFRESH    1        /* ����ˢ��            */
#define NC_SIGNAL_RELOAD     2        /* ����װ��            */
#define NC_SIGNAL_MYSTAT     3        /* �ҵ�״̬            */
#define NC_SIGNAL_RELOADIPT  4        /* ����װ��            */
#define NC_SIGNAL_RELOADCONT 5        /* ����װ��            */
#define NC_SIGNAL_RELOADPKG  6        /* ����װ��            */

#define nc_copt_default    '^'        /* ȱʡ��ƥ�䷽ʽ      */

/* ���º����ڼ���ĺϷ���       */
#define NC_TABLES_NCNETSENSLOG          0
#define NC_TABLES_NCHTTPLOG             1
#define NC_TABLES_NCIPLOG               2
#define NC_TABLES_NCMAILMON             3
#define NC_TABLES_NCFORMMON             4
#define NC_TABLES_NCBBSMON              5
#define NC_TABLES_NCCONTLOG             6
#define NC_TABLES_NCWARNLOG             7
#define NC_TABLES_NCERRLOG              8
#define NC_TABLES_NCSEARCHLOG           9



/* �����ֲ����ڵ�ʱ��,��ʲô������  */
#define NC_IFNONAME_IP       1
#define NC_IFNONAME_MAC      2
#define NC_IFNONAME_VLAN     3
#define NC_IFNONAME_UNKNOW   4
#define NC_IFNONAME_OTHER    9


#define NC_SAVELOG_NONE      0
#define NC_SAVELOG_MYSQL     1
#define NC_SAVELOG_FILE      2
#define NC_SAVELOG_LOADFILE  3


#define NC_MATCH_FILENAME    1
#define NC_MATCH_CONTENT     2
#define NC_MATCH_ATTFILE     3
#define NC_MATCH_SUBJECT     4

typedef struct ncMatchCont_s {
    char                *pMatchKey;      // ƥ��ؼ���
    unsigned short      iCount;          // ƥ�����
    unsigned char       cType;           // ƥ����� �μ�����ĺ궨��
    unsigned char       cMatch;          // ƥ�䷽ʽ ����
    struct ncMatchCont_s *next;
} ncMatchCont;

 
typedef struct ncIpAddress_s {
    unsigned long    lIp;
    unsigned short   nPort;
    unsigned char    cFlags;
    unsigned char    cRev;
} ncIpAddress;


#define NC_CAP_VTUNE         2


/* �û���һЩͳ����Ϣ, ���ݰ�ƽ��ÿ����ͳ��һ��  */
typedef struct ncUserCountInfo_s {
    unsigned long       lSTime;        /* ����ʱ��, �ϴ�ͳ��ʱ��   */
    unsigned long       lCTime;        /* ����ʱ��, ����ͳ��ʱ��   */
    unsigned long long  lSBytes[2];    /* �ֽ���,��һ���ֽ���     */
    unsigned long long  lSPkg[2];      /* ���ݰ�����,��һ�����ݰ� */
    unsigned long lSIp;                /* ����IP��������,��һ��IP��ַ  */
    unsigned long lSMail[2];           /* �ʼ��շ�����,��һ���ʼ���    */
    unsigned long long  lCBytes[2];    /* ��ǰ�ֽ���       */
    unsigned long long  lCPkg[2];      /* ��ǰ���ݰ�����   */
    unsigned long lCIp;                /* ��ǰIp������     */
    unsigned long lCMail[2];           /* ��ǰ�շ��ʼ����� */
} ncUserCountInfo;



/* �û���Ϣ  */
typedef struct ncUserAuthInfo_s {
    unsigned char username[32];     // �û���
    unsigned char mac[6];           // Mac��ַ
    unsigned char dispname[32];     // ��ʾ��
    unsigned char groupname[32];    // ����
    unsigned char compname[32];     // �������
    unsigned char password[32];
    unsigned char position[32];     // ����λ�ã������
    unsigned char idfytype[4];      // ֤�����
    unsigned char idfyno[20];       // ֤������
    unsigned long vlan;            // VlanId
    unsigned char sex;              // �Ա� 1-�� 2-Ů 0-δ֪
    unsigned char flags;            // ������Դ
    unsigned char cRev[2];
} ncUserAuthInfo;


/* �ۺϲ��� �ͱ� nclimsumm��Ӧ  
   ����޸�ʱ�� 2009/1/12
   

 */
typedef struct ncLimSummer_s {
    unsigned char ipaccess;
    unsigned char httpaccess;
    unsigned char postsend;
    unsigned char httpbyip;

    unsigned char limitunit;         /* ���Ƶ�λ  1--ÿ��  2--ÿ��  3--ÿ�� 4--ȫ��  */
    unsigned char overctl;           /* �������ƺ�Ĵ���ʽ    
                                     0- ʹ�÷������    overid -- ����ID          overnum -- ���
                                     1- ʹ����ַ�����  overid -- ��ַ�����ID    overnum -- ��� 
                                     */
    unsigned char ipbind;
    unsigned char macbind;

    unsigned char namebind;
    unsigned char onlyone;
    short         overnum;

    unsigned long long flowlimit;    
    unsigned long timelimit;    
    unsigned long overid;    
    unsigned long feeid;  
    unsigned long polid;  
    ncBill        *psBill;
/*  ����Ϊ 2006/02/05 ����  */
    unsigned long   warntype;          /* �澯��� 0--���澯  1--�澯    */
    unsigned long   pkgmax;            /* ÿ�������ݰ�������             */
    unsigned long   flowmax;           /* ÿ���ӵ�����                   */
    unsigned long   ipmax;              /* IP������                       */
    unsigned long   mailmax;            /* �ʼ��շ�����                   */

    unsigned char   pkgflags;           /* ���� 0--ȫ�� 1--����  2--����   */
    unsigned char   flowflags;          /* ���� 0--ȫ�� 1--����  2--����   */
    unsigned char   mailflags;          /* 0--ȫ��  1--����   2--����      */
    unsigned char   accflags;           /* ��������� 0--������ 1--����    */

    unsigned long   qospolid;           /* ����������                    */
    unsigned long   mailctlid;          /* �ʼ����ز���                    */

    unsigned char   http80chk;          /* ���80�˿ڵ�httpЭ�� 0 �����  1 ���  9 δ���� */
    unsigned char   r1;                 /* ����   */
    unsigned char   r2;
    unsigned char   r3;
}  ncLimSummer;

typedef struct ncLimPolInfo_s {        /* ���ʿ��Ʋ�����Ϣ  */
    unsigned long lId;                 /* ����ID            */
    char           caName[16];         /* ��������          */
    ncLimSummer    sSumm;              /* �ۺϲ���          */
    unsigned short nLimIp;             /* Ip������ʼ���    */
    unsigned short nLimWeb;            /* Web������ʼ���   */
} ncLimPolInfo;

   

/*   ��Ĺ���  ��С�������� 
     2013/01/12 ����
 */
typedef struct ncLimGroup_s {    /* ����Ϣ  */
    unsigned long  lId;
    char           groupname[32];
    ncLimSummer    sSumm;
    unsigned short nLimIp;
    unsigned short nLimWeb;
    ncLimPolInfo   *psMypol;
    unsigned  long long thisflow;     /* ��ǰ����                  */
    unsigned  long thistime;          /* ��ǰʱ��                  */
    unsigned  long long flowlimit;    /* ��������                  */
    unsigned  long timelimit;         /* ʱ������                  */
    unsigned  long overid;            /* ������ʹ�õķ������      */
    unsigned  long resettime;         /* ��������ʱ��              */
    unsigned  char limitunit;         /* ���Ƶ�λ  9--���� 1--ÿ��  2--ÿ��  3--ÿ�� 4--ȫ��  */
    unsigned  char cmod;              /* 0--����  1--�޸�                  */
    short     overnum;                /* ������Ե����, -1 ��ʾ����       */
    unsigned  long long lBytes[2];
    unsigned  long lDid;              /* ��Ӧ�ĵ�λID,���ڹ������ģ�1��������Ӧ�����λ  */
    unsigned  long lNum;              /* ��Ӧ�� ncGroupDept �е����                     */
    unsigned  long lPid;              /* ��һ������               New                    */
    struct    ncLimGroup_s   *preGroup;  /* ��һ������ָ��           New                    */
} ncLimGroup;


typedef struct ncUserCont_s {        /* �û���Ϣ       */
    unsigned long  userid;           /* 0--�û�������  */
    char           username[32];     /* �û���         */
    char           dispname[32];     /* ��ʾ��         */
    char           compname[32];     /* �������       */
    char           position[32];     /* λ��           */
    char           password[24];     /* ����           */
    char           cardid[20];       /* ���֤����     */
    char           idtype[4];        /* ֤�����       */
    unsigned long  groupid;          /* ��ID           */
    unsigned long  ip;               /* IP��ַ         */
    unsigned long  thisip;           /* ��ǰ�ͻ���IP��ַ */    
    unsigned long  thistime;         /* ��ǰ����ʱ��   */
    unsigned long  resettime;        /* ��ʼ��ʱ��     */
    long     long  thisflow;         /* ��ǰ����       */
    unsigned long  lasttime;         /* ����޸�ʱ��   */
    ncLimSummer    sSumm;            /* ���������Ϣ   */
    ncLimGroup     *psGroup;
    ncLimPolInfo   *psMypol;
    unsigned short nLimWeb;          /* ��վ���Ʒ���, ��Ŵ�1��ʼ   */
    unsigned short nLimIp;           /* IP���Ʒ���     */
    double         fee;              /* �˻����       */
    double         thisfee;          /* �ۼƷ���       */

    unsigned char  mac[6];
    unsigned char  useflags;         /* �û�״̬  1 ����    9 ����       */
    unsigned char  modflags;         /* �޸ı�ʶ  0--û��  1--����   2--�޸�   9--ɾ��*/

    unsigned char  userbase;         /* �û�����ʽ                                  */
    unsigned char  cStatus;          /*  1--����  2--����   9-����  */
    unsigned char  cProxy;           /*  ����  1,3 Http����    2,3 Socks����            */
    unsigned char  cCaseCtl;         /* ���ظ澯 0--����  1-���ظ澯                    */

    unsigned char  cNameStatus;      /* ���ֵ�����  0--����  1--��ʱ, �����û����ڼ��������׼ȷ������ip��ַ����,����������ȷ�����ֺ��Զ�����   */
    unsigned char  cInDb;            /* 0-- 1-������  0-������,��Ҫ�����ڴ�����ݿ�ͬ��  9-��¼�ѱ�ɾ��          */
    unsigned char  cWins;            /* 0-- ����      1-��Ҫ���¼��mac��name���ձ�                */
    unsigned char  cBusy;            /* 0-- ����      1-�û���Ϣ�����޸�   */

    unsigned char  cValid;           /* 0-- ��Ч      1-��Ч               */
    unsigned char  cForbFor;         /* ʧЧԭ�� 0--����  1--��������  ����*/
    unsigned char  cSex;             /* �Ա� 1--��  2--Ů     */
    unsigned char  cFrom;            /* �ͻ���Ϣ��Դ,��δ����                  */
    unsigned long  nVlanId;          /* Vlan Id, �Ի���VlanId������Ч      */
    unsigned long  expiretime;       /* ʧЧʱ��                           */
    unsigned long  lAddTime;         // ����ʱ��
    unsigned long  lNum;             /* �ڼ�¼�е����                     */
} ncUserCont;

#define NC_FORBIDFOR_FEE           1         /* �Ʒѿ���   */


#define NC_IPBASE_PRE    999
#define NC_MACBASE_PRE   111
#define NC_IDBASE_PRE    222

/* ͨ��Hash������������  */
typedef struct ncUserIpIndex_s {
    unsigned long pre;
    unsigned long ip;
    unsigned long lIndex;
} ncUserIpIndex;

typedef struct ncUserMacIndex_s {
    unsigned short pre;
    unsigned char  mac[6];
    unsigned long  lIndex;
} ncUserMacIndex;    

typedef struct ncUserIdIndex_s {
    unsigned long pre;
    unsigned long lId;
    unsigned long lIndex;
} ncUserIdIndex;    

typedef struct ncUserContHead_s {
    unsigned long lMaxUser;
    unsigned long lSumUser;
    unsigned long lHashPos;          /* Hash���ƫ����    */
    unsigned long lContPos;          /* �û����ݵ�ƫ����  */
    unsigned long lLasttime;         /* ����޸�ʱ��      */
    int           iModFlags;         /* �޸ı�ʶ 0--δ�Ĺ�  1--�����޸�  */
    unsigned long lProxyCheckTime;   /* proxy�����ʱ��      */
    unsigned char cProxyCheck;       /* 0--����  1--�������   */
    unsigned char cLock;
    unsigned char cLockAdd;
    unsigned char cRev;
    ncUserCont    *psUser;
    char          *psHash;
} ncUserContHead;

#define NC_LOGIN_IP         2      /* ͨ��IP��ַ��������    */
#define NC_LOGIN_MAC        3      /* ͨ��MAC��ַ��������   */
#define NC_LOGIN_IC         4
#define NC_LOGIN_AUTH       4      /* ͨ��������֤��������  */
#define NC_LOGIN_THIRD      4      /* ͨ��������֤��������  */
#define NC_LOGIN_CLIENT     5      /* ͨ��������֤��������  */
#define NC_LOGIN_BILLAUTH   4      /* �Ʒ���֤              */

#define NC_LOGIN_REMOTE1    100    /* Զ����֤  cRev ��ֵ   */
typedef struct ncUserInfo_s {      /* �û���Ϣ       */
    unsigned long  lIp;            /* 0 ��ʾû���û� */
    unsigned long  lId;            /* �û�Id  0--û��                           */
    unsigned long  lGid;           /* ��Id   groupid=0 δ֪�û�  1=������        */
    unsigned char  cUseflags;      /* ʹ��״̬0--���� 1--����  9--����         */
    unsigned char  cStatus;        /*         99--�˳�                          */
    unsigned char  mac[6];         /* MAC��ַ                                   */
    unsigned long  lStartTime;     /* ��ʼ����ʱ��      */
    unsigned long  lLastTime;      /* ������ʱ��      */
    unsigned long  lLastCountTime; /* ���һ�μ���ʱ��  */
    unsigned long  lConnTime;      /* ��������ʱ��,������ʱ������   */
    unsigned long  lSaveTime;      /* �ϴα��������ʱ��  */
    long8          lSaveBytes[2];  /* �ϴα�������� */
    long8          lBytes[2];      /* 0-�ܵ��������� 1--�������� ������ʱ�俪ʼ����     */
    long8          lSumflow;       /* �ӿ�ʼ��ʱ����ǰ��������                      */
    unsigned long  lSumtime;       /* �ӿ�ʼ��ʱ����ǰʱ������     */
    unsigned long  lLastSaveTime;  /* �ϴβ������ݿ�ʱ��   */
    unsigned long  lPkg[2];        /* ���ݰ����� 0:dat 1:syn  ���ݸ�ֵ���ж�TCP�����Ƿ����� */
    ncUserCont     *psUser;        /* ����û�����         */
    unsigned long  lCltLastTime;   /* �Ϳͻ�����󽻻���Ϣ     */
    unsigned long  lCltId;         /* �ͻ���ID                 */
    ncUserCountInfo sCount;        /* ͳ����Ϣ 2006/02/06 ����  */
    unsigned char  cLogin;         /* ��¼��ʽ    2 ����IP��ַ  3 ����MAC��ַ 4 ������֤ 5 ��������֤  6 �ͻ��˷�ʽ */
    unsigned char  cMacCheck;      /* 0--NoCheck 1--Check  */
    unsigned char  cProxy;         /* > 0 proxy            */
    unsigned char  cRev;           /* �� httpproxy��֤��ʱ�򣬸�ֵ��Ϊ 1,�������¼    �ڵ�һ�η��ʵ���ʱ����Ϊ0-��ʾû���� */
    unsigned long  lSid;           /* ����ID                                          */
    unsigned char  position[32];   /* ��ǰλ��                                        */
    char           chkkey[16];     /* Keyֵ                                           */
    unsigned long   lQos;           /* ��ǰ�û�����                                   */
    unsigned char   R1;             /* ��ʱ��ֹ�û����� 0-���� 1-����    */
    unsigned char   R2;             /* ����������                      */
    unsigned short  R4;            /* P2P ��ʶ  1-Ѹ��         */
    unsigned long   lRtime;         /* ����ֹ����ʱ��               */
    unsigned long   lStop;          /* �������õ����ʱ��             */
    unsigned long   lServiceTime;   /* ����ʶ��ʱ��                   */
    unsigned char   cSaveDb;        /* 0--δ����  1--�ѱ��浽���ݿ�   */
    unsigned char   cRev2[3];       /* ����                           */
} ncUserInfo;

/* ���ݵ�������Ϣ  */
typedef struct ncUserBack_s {      /* �û���Ϣ       */
    unsigned long  lIp;            /* 0 ��ʾû���û� */
    unsigned long  lId;            /* �û�Id  0--û��                          */
    unsigned long  lGid;           /* ��Id   groupid=0 δ֪�û�  1=������      */
    unsigned char  cUseflags;      /* ʹ��״̬0--���� 1--����  9--����            */
    unsigned char  cStatus;        /*                                               */
    unsigned char  mac[6];         /* MAC��ַ           */
    unsigned long  lStartTime;     /* ��ʼ����ʱ��      */
    unsigned long  lLastTime;      /* ������ʱ��      */
    unsigned long  lLastCountTime; /* ���һ�μ���ʱ��  */
    unsigned long  lConnTime;      /* ��������ʱ��,������ʱ������   */
    long8          lBytes[2];      /* 0-�ܵ��������� 1--�������� ������ʱ�俪ʼ����     */
    unsigned long  lPkg[2];        /* ���ݰ����� 0:dat 1:syn  ���ݸ�ֵ���ж�TCP�����Ƿ����� */
    unsigned long  lCltLastTime;   /* �Ϳͻ�����󽻻���Ϣ     */
    unsigned long  lCltId;         /* �ͻ���ID                 */
    ncUserCountInfo sCount;        /* ͳ����Ϣ 2006/02/06 ����  */
    unsigned char  cLogin;         /* ��¼��ʽ    2 ����IP��ַ  3 ����MAC��ַ 4 ������֤ 5 ��������֤  6 �ͻ��˷�ʽ */
    unsigned char  cMacCheck;      /* 0--NoCheck 1--Check  */
    unsigned char  cProxy;         /* > 0 proxy            */
    unsigned char  cRev;           /* �� httpproxy��֤��ʱ�򣬸�ֵ��Ϊ 1,�������¼   */
    unsigned long  lSid;           /* ����ID                                          */
    unsigned char  position[32];   /* ��ǰλ��                                        */
    struct ncUserBack_s   *next;
} ncUserBack;

/* ϵͳ����Ϣ         */
#define NC_LOCK_SYSREFRESH       0       /* ϵͳˢ��     */
#define NC_LOCK_UPDATEURL        1       /* ��������URL  */
#define NC_LOCK_AUTOCLEAN        2       /* �Զ�����     */
#define NC_LOCK_SMTPPROXY        3      
#define NC_LOCK_GETUSER          4       /* ȡ�û���Ϣ   */
#define NC_LOCK_OUTLOG           5 

 /*  ����¼            */
typedef struct ncLockInfo_s {
    int  iLock;                  /*  1--����    0--����  */
    unsigned long lTimeout;      /*  ��ʱʱ��            */
    unsigned long lLasttime;     /*  ����ʱ��            */
    int  iPid;                   /*  ���̺�              */
    char     caDesc[16];         /*  ����                */
    char     caMsg[128];         /*  ��ǰ��Ϣ            */
} ncLockInfo;    

/* 
    # --- ��ͷ
    ^ --- ����
    = --- ����
    ! --- ��
    $ --- ����    

 */
typedef struct ncUtlStrStr_hs {
    unsigned char        cMark;         /* ���ṹ     */
    unsigned char        cSum;          /* �ܸ���     */
    unsigned char        nLen;          /* ����       */
    unsigned char        cOpt;          /* 0--����  1--��      */
} ncUtlStrStr_h;

typedef struct ncUtlStrStr_cs {
    unsigned char        cOpt;          /*  0--����  1--��  */
    unsigned char        cFlags;        /*  ������  =#$^    */
    unsigned short       nLen;          /* ����             */
    char     caWord[256];                /* �ַ�������       */
} ncUtlStrStr_c;


typedef struct ncThreadInfo_s {
    int      iSumThread;                 /* �߳�����  */
    unsigned long lStartTime;            /* ������ʼʱ��  */
    unsigned long lLastTime;             /* ������ʱ��  */
    int      iPid;                       /* ����ID        */
} ncThreadInfo;

typedef struct ncProxyProcessInfo_s {
    int iMaxThread;            /* ÿ����������߳���  */
    int iMaxProcess;           /* ��������          */
    int iMinProcess;           /* ��С������          */
    int iSumProcess;           /* �ܼƽ�����          */
    ncThreadInfo *psThread;
} ncProxyProcessInfo;


typedef struct ncIcUserinfo_s {
    unsigned long  lSip;      /* ԴIP��ַ   */
    unsigned long  lUserid;   /* �û�ID     */
    char           username[32];
    unsigned long  lStartTime;
    unsigned long  lLastTime;
} ncIcUser;


typedef struct ncMailIpList_s {
    unsigned long  lSip;      /* ԴIP��ַ   */
    unsigned short nSport;    /* Դ�˿�     */
    unsigned short nDport;    /* Ŀ��˿�   */
    unsigned long  lDip;      /* Ŀ��IP��ַ */
    unsigned long  lLastTime;
    int            iFlags;    /* 0--����  1--����  */
} ncMailIpList;


/* Proxy��Ip��ַ�Ͷ˿�    */
typedef struct ncProxyIpList_s {
    unsigned long  lIp;       /* IP��ַ      */
    unsigned short nPort;     /* �˿�        */
    unsigned short nRev;      /* ����        */
} ncProxyIpList;


typedef struct ncIpPortHead_s {
    ncProxyIpList  *psList;
    unsigned int   iMax;          /* ����ĵ�ַ�������  */
    unsigned int   iSum;
} ncIpPortHead;

    
typedef struct pasSearchHead_s {
    char caFname[32];        /* �ֶ���               */
    char caDispname[32];     /* ��ʾ��               */
    int  iSize;
    int  iMaxLen;
    int  iType;              /* ��������  1-�ı�  2-ѡ���  */
    char *pValue;
} pasSearchHead;

/* �ڴ��е��û���Ϣ  */
typedef struct ncUserBuf_s {       /* �û���Ϣ                */
    long     lUserid;              /* �û�Id  0--û��         */
    char     caUsername[32];       /* ����                    */
    char     caDispname[32];       /* ��ʾ��                  */
    long     lGroupid;             /* ����ID                  */
    char     caGroupname[32];      /* ��������                */
} ncUserBuf;

/* �û���Ϣ�Ļ���
         ���ṹ��Ҫ�����û���Ϣ��Case, ��ϵͳ�д����һ��Hash����
 */
typedef struct ncUserBufIp_s {
	unsigned long lSip;           /* IP��ַ  */
	int      iFlags;              /* ״̬ 0--δ��       1--������  2--�û���ʱ  3--�û�������
	                                      4--MAC��һ��  7--����    9--����
	                               */
	unsigned long lLastTime;      /* ������ʱ��  */
} ncUserBufIp;


/* ��������֤���û���Ϣ  */
typedef struct ncThirdAuth_s {
    char          caUsername[32];    /* �û���                */
    int           iIndex;            /* ���û���Ϣ�е�������  */
    unsigned long lIp;               /* IP��ַ 0 --��ʾû������ */
    unsigned long lLastTime;         /* ������ʱ��          */
} ncThirdAuth;

typedef struct ncNtAuth_s {
    unsigned long lIp;               /* IP��ַ                    */
    unsigned long lSip;              /* �Ƚ�IP��ַ                */
    char          caName[32];
    char          caDomain[32];      /* ����                       */
    unsigned long lLastTime;         /* ������ʱ��               */
    char          caKey[16];
    char          caMd5[32];         /* ��ͻ��˷��ŵ�Key,16λ     */
    int           ccode;
} ncNtAuth;

typedef struct ncIcAuth_s {
    unsigned long lIp;               /* IP��ַ             */
    char          caName[32];
    unsigned long lLastTime;         /* ������ʱ��       */
} ncIcAuth;



/* �û�ͬ����Ϣ   */
typedef struct ncNtUser_s {        /* NT�û���Ϣ     */
    char          caName[32];      /* ����           */
    unsigned long lIp;             /* IP��ַ         */
    int           iStatus;        /* 1--��½  0--�˳�  */
} ncNtUser;

/* ���û���Ϣ    */
typedef struct ncAdUser_s {
    unsigned long lIp;
    char          caName[32];
    char          mac[6];
    unsigned long lLastTime;
}ncAdUser;


/* ��Ľṹ��Ϣ  */
typedef struct ncDbField_s {
    int  iFlags;
    char caField[32];
    char caType[32];
    char caNull[32];
  	char caKey[32];
  	char caDefault[64];
  	char caExtra[32];
} ncDbField;

typedef struct ncDbIndex_s {
    int  iFlags;
    char caIndex[32];
    char caUnique[4];    /* 0-- Unique  1-- No */
    char caSeq[4];       /* ���               */
    char caField[32];
} ncDbIndex;



#define NC_NETWORK_NOMAL          0    /* ��������          */
#define NC_NETWORK_NAMEBASE       1    /* ��������          */
#define NC_NETWORK_INTERNET       99   /* Internet��        */
#define NC_NETWORK_IPBASE         2    /* ����IP��ַ        */
#define NC_NETWORK_MACBASE        3    /* ����MAC��ַ       */
#define NC_NETWORK_AUTH           4    /* ������֤          */
#define NC_NETWORK_NTAUTH         5    /* ��������֤        */
#define NC_NETWORK_NOMON          6    /* ����            */
#define NC_NETWORK_FORBID         7    /* ����              */
#define NC_NETWORK_SERVER         8    /* ������            */
#define NC_NETWORK_CLIENT         9    /* ���ڿͻ���ID      */
#define NC_NETWORK_OUTSIDE        10   /* �ⲿ              */
#define NC_NETWORK_PORT           11   /* ���ڶ˿�          */
#define NC_NETWORK_VLANID         12   /* ����VLan ID ����  */
#define NC_NETWORK_SPECLOGIN      13   /* �ض���¼          */
#define NC_NETWORK_ADDUSER        88   // �Զ�����û�


#define NC_NAMED_IP                90  
#define NC_NAMED_MAC               91  
#define NC_NAMED_NAME              92
#define NC_NAMED_PORT              93
#define NC_NAMED_PORTPOS           94
#define NC_NAMED_PORTPOSLOG        95
#define NC_NAMED_VLANID            96
#define NC_NAMED_VLANIDPOS         97
#define NC_NAMED_VLANIDPOSLOG      98
#define NC_NAMED_VLANIDPOSUSER     99
#define NC_NAMED_VLANIDUSER       100
#define NC_NAMED_MACUSER          101

#define NC_NAMED_WINS             102              // __FROM + 100
#define NC_NAMED_THIRD            103
#define NC_NAMED_RADIUS           111
#define NC_NAMED_CAMS             112
#define NC_NAMED_POP3             115
#define NC_NAMED_PPPOE            117
#define NC_NAMED_QQNUMBER         118
#define NC_NAMED_WEBACCOUNT       119
#define NC_NAMED_WEBNAMEPOSUSER   120             // �ʺ�λ���û�




#define NC_NETWORK_L2             2
#define NC_NETWORK_L3             3




typedef struct ncIpNetwork_s {      /* �ڲ�IP��ַ    */
    unsigned long   lIpLower;       /* ��ʼIP��ַ    */
    unsigned long   lIpUpper;       /* ��ֹIP��ַ    */
    unsigned long   lAuthIp;        /* ��֤������IP��ַ  */
    unsigned short  nAuthPort;      /* ��֤�������˿ں�  */
    unsigned short  iFlags;         /* ���궨��      */
    unsigned long   lId;             /* ��¼ID        */
} ncIpNetwork;

/* Ip��ͷ����Ϣ  */
typedef struct ncIpHead_s {
	unsigned long  lSip;       /* ԴIP��ַ     */
	unsigned long  lDip;       /* Ŀ��IP��ַ   */
	unsigned short nDport;     /* Ŀ��˿�     */
	unsigned short nSport;     /* Դ�˿�       */
	char     caSmac[6];        /* ԴMAC��ַ    */
	char     caDmac[6];        /* Ŀ��MAC��ַ  */
	unsigned char   cTran;            /* ���ͷ��� 0--���� 1--����    */
	unsigned char   cProt;            /* Э��  6 UDP      17 TCP     */
	unsigned char   cIpif:4;          /* �����豸���                */
    unsigned char   cEpif:4;	      /* ��ȥ�豸���豸�� nsa��ʹ��  */
    unsigned char   cDev;             /* ���ݰ�����Ķ˿ں�          */
	unsigned long   nVid;             /* Vlan Id  */
	unsigned long   lSeq;
	unsigned long   lAckSeq;
} ncIpHead;


typedef struct ncMsnPkg_s {
	unsigned long  lSip;       /* ԴIP��ַ     */
	unsigned long  lDip;       /* Ŀ��IP��ַ   */
	unsigned short nDport;     /* Ŀ��˿�     */
	unsigned short nSport;     /* Դ�˿�       */
	unsigned long  lLen;       /* ���ݰ�����   */
} ncMsnPkg;



/* ���Ʒ���  */
typedef struct ncIpLimited_s {       /* ���Ʒ��ʵ�Ip��ַ  */
    unsigned long   lUid;              /* �û�Id  0 ��ʾȫ��  */
    unsigned long   lIpLower;          /* ����  */
    unsigned long   lIpUpper;          /* ����  */
    unsigned char   cTranType;         /* ��������  0--ȫ��   1--UDP  2--TCP          */
    unsigned char   cTimeType;         /* ʱ������                                    */
    unsigned short  nLowerport;        /* �˿ں�����                                  */
    unsigned short  nUpperport;        /* �˿ں�����                                  */
    unsigned char   cService;          /* ��������                                    */
    unsigned char   cFlags;            /* ���Ʒ�ʽ 1--�ⲿ��  2--����  3--IP��ַ�Ͷ˿� */
    unsigned long   lStart;            /* ��ʼʱ�� HHMMSS                             */
    unsigned long   lEnd;              /* ��ֹʱ�� HHMMSS                             */
    unsigned char   cVister;           /* 0 ��ֹ   1 ����                             */
    unsigned char   cReserve[3];       /* ����  */
} ncIpLimited;


typedef struct ncWebLimited_s {        /* ���Ʒ��ʵ���վ      */
    unsigned long   lUid;              /* �û�Id  0 ��ʾȫ��  */
    char            caUrl[64];         /* ��վ��ַ            */
    unsigned long   lStart;            /* ��ʼʱ�� HHMMSS     */
    unsigned long   lEnd;              /* ��ֹʱ�� HHMMSS     */
    unsigned char   cTimeType;         /* ʱ������            */
    unsigned char   cVister;           /* 0  ��ֹ  1 ����     */
    unsigned char   cReserve[2];       /* ����                */
} ncWebLimited;

/* The Value of cType */
#define NC_PROT_TCP      6
#define NC_PROT_UDP     17
/* �˴���Դ��ַΪ�ͻ��˵�ַ  */
typedef struct ncIp_s {
    unsigned long  lSip;             /* �ڲ�IP��ַ                    */
    unsigned long  lDip;             /* �ⲿIP��ַ                    */
    unsigned short nDport;           /* Ŀ��˿ں�                    */
    unsigned short nSport;           /* Դ�˿�, ȱʡ�����Ϊ 0, ֻ�е������ʱ���Ϊ�� 0  */
    unsigned char  cType;            /* Э��    6--TCP 17 UDP         */
    unsigned char  cMark;            /* ��ʶ,�ڲ�ͬ�ĵط��в�ͬ���ô� */    
    unsigned char  cFileMark;        /* �ļ��������� 1-�����ļ� 2-FTP��������    */
    unsigned char  cCode;            /* ����  */
    unsigned long  lStartTime;       /* ��ʼʱ��                     */
    unsigned long  lLastTime;        /* ����޸�ʱ��                 */
    unsigned long  lStop;            /* ���ʿ��Ƽ��ʱ�� hhmmss�� ����ֵΪ0��ʾʹ�ó�ʱʱ��  */
    unsigned long  lQos;             /* Qos���                      */
    long8          lBytes[2];        /* 0-�����ֽ���  1-�����ֽ���   */
    unsigned char  cStatus;          /* 0--��   1--����  2--����     */
    unsigned char  cUseFlags;        /* ʹ�ñ�ʶ 0--û���� 1--������ 2--�Ѵ��� 3--�ѱ���   */
    unsigned char  caMac[6];         /* ԴMac��ַ                    */
    ncUserInfo     *psUser;          /* �û���Ϣ����Ϊ�ձ�ʾ�û���Ϣ����    */
    unsigned short lService;         /* ����                                */
    unsigned char  cFun;             /* ������                              */
    unsigned char  cContinue;        /* Э���Ƿ�������� 0--������  1--���� */
    unsigned long  lUrl;             /* ����http                            */
    unsigned short nPreService;      /* Ԥ����Э��                          */
    unsigned short nCount;           /* ������ִ���                        */
    unsigned char  cProService[8];   /* Э���ʶ,����Э��������ķ���       */
    char    *p;                      /* ����ָ��  */
} ncIp;



/* TCP������Ϣ  
      �˴���Դ��ַΪ������ַ
*/
/* Value of cConnect   */
#define NC_TCPSTAT_SYN     			1
#define NC_TCPSTAT_ESTABLISHED    	2
#define NC_TCPSTAT_CLOSED             9
/* Value of cDict      */
#define NC_DICT_SEND           0
#define NC_DICT_RECEIVE        1
typedef struct ncTcp_s {
    unsigned long  lSip;              /* �ڲ�IP��ַ                   */
    unsigned long  lDip;              /* �ⲿIP��ַ                   */
    unsigned short nDport;            /* Ŀ��˿ں�                   */
    unsigned short nSport;            /* Դ�˿ں�                     */
    unsigned long  lBytes;            /* ����                         */
    unsigned long  lLastTime;         /* ����޸�ʱ��                 */
    unsigned char  cConnect;          /* ����״̬  1 Syn  2 ����  9 Close   */
    unsigned char  cDiction;          /* ���� 0 ����   1 ����               */
    unsigned char  cStatus;           /*      0 δ֪   1 ����      2 ����   */
    unsigned char  cProt;             /* Э��   6-TCP  17 UDP               */
    unsigned char  cFlags;            /* ���ھ���Э���еı�ʶ               */
    unsigned char  cProxy;            /* 0 -- ����   1 -- ����Proxy��Э��   */
    unsigned char  cIsOld;            /* 0--�°� 1--��  */
    unsigned char  cVid;              /* ����ͨ��ID                         */
    ncIp           *psIp;             /* ��Ӧ��IP��Ϣ,NULL ��ʾ�����Ӳ����� */
    unsigned long  lStop;             /* �������õ���ֹʱ��                 */
    unsigned long  lSeq;              /* �������к�                          */
    unsigned long  lAck_seq;          /* �������к�                          */
    unsigned long  lSid;              /* Session ID                          */
    unsigned long  lUrl;              /* Url���                             */
    unsigned long  lQos;              /* Qos���                             */
    unsigned long  lSetTime;          /* ����ʱ��                            */
    unsigned long  lBase;             /* �����ڿ�����Ϊ��׼λ��              */
    char *p;                          /* ����ָ��                            */
} ncTcp;


#define NC_TCPFLAGS_QQACTIVE      166               // �ñ�־��ʾ��Ҫ����Ӧ�����ݰ�

/*    2005/10/17 �޸�
      �����IP����, Key=12    
         cStatus --- ����ԭ��
         ID      --- ����ID
 */
typedef struct ncIpExcept_s {
    unsigned long  lSip;             /* �ڲ�IP��ַ                         */
    unsigned long  lDip;             /* �ⲿIP��ַ                         */
    unsigned short nDport;           /* Ŀ��˿ں�                         */
    unsigned char  cStatus;          /* 0-����  1-��ϸ澯  2--����澯  3--���              */
    unsigned char  cType;            /* ���    NC_DENY_NONE           0     ֱ�ӽ�ֹ    
                                                NC_DENY_NORMAL         1     һ����� 
                                                NC_DENY_WEBPOL         2     ��ַ�����
                                                NC_DENY_SERVICE        3     �������� 
                                                NC_DENY_SERVICEPOL     4     �������
                                                NC_DENY_WEBURL         5     ��ַ����
                                      */
    unsigned long  lRid;             /* ����ID                             */
    unsigned long  lUserid;          /* �û�ID, UseridΪ0��ʾ���ܿ����ݰ�  */
    unsigned long  lStartTime;       /* ����޸�ʱ��                       */
    unsigned long  lLastTime;        /* ����޸�ʱ��                       */
    unsigned long  lSaveTime;        /* ���ݿ����ʱ��                     */
    unsigned long  lCount;           /* ���ݰ�������                       */
    unsigned long  lService;         /* �������ID                         */
    unsigned long  lwarnid;          /* �澯ID                             */
    unsigned char  cProt;            /* Э��    6--TCP 17 UDP              */
    unsigned char  cFlags;           /* ����״̬ 1-��ϸ澯 2-����澯 3-���  */
    unsigned char  cRev[2];          /* ����                               */
    unsigned long  lSid;             /* ��Ӧ�ڱ���ID                       */
    unsigned long  lSsid;            /* ��Ӧ�ڹ������ĵ�ID                 */
    unsigned long  lUsid;            /* ��½ID                             */
    char           caMsg[96];        /* ������Ϣ                           */
} ncIpExcept;


typedef struct ncHttpBuffer_s {
    unsigned long  lSip;             /* ԴIP��ַ     */
    unsigned long  lStartTime;       /* ��ʼʱ��     */
    unsigned long  lLastTime;        /* ��ֹʱ��     */
    unsigned long  lBytes;           /* ����         */
    unsigned long  lDip;             /* Ŀ���ַ     */
    unsigned short nDport;           /* Ŀ��˿�     */
    unsigned char  caMac[6];         /* ԴMac��ַ    */
    unsigned char  cUseFlags;        /* ��ʶ��0--������ 1--������ 2--���Դ��� 3--�ѱ���  */
    unsigned char  cMethod;          /* Http����     */
    unsigned char  cContent;         /* �������ݱ�־ 0-��  1-��ʼ  2-������   */
    unsigned char  cZip;             /* 0-��ѹ��  1-��ѹ��  */
    unsigned long  userid;
    unsigned long  groupid;
    unsigned long  lUsid;            /* ��½ID                                  */
    unsigned long  lFid;             /* ���������ݵ�ʱ�򣬸�ֵΪ > 0,�ļ�ID     */
    char     caHost[64];
    char     caUrl[128];
    char     caTitle[128];             /* ���� 2005/01/05 ����      */
} ncHttpBuffer;


// Http Hash Buffer
typedef struct ncHttpHashBuffer_s {
    unsigned long  lSip;             /* ԴIP��ַ     */
    char           caHost[64];       /* Host         */
    char           caUrl[128];       /* Url          */    
    unsigned long  lStartTime;       /* ��ʼʱ��     */
    unsigned long  lLastTime;        /* ��ֹʱ��     */
    unsigned long  lBytes;           /* ����         */
    unsigned long  lDip;             /* Ŀ���ַ     */
    unsigned short nDport;           /* Ŀ��˿�     */
    unsigned char  caMac[6];         /* ԴMac��ַ    */
    unsigned long  userid;
    unsigned long  groupid;
    unsigned long  lUsid;            /* ��½ID       */
    unsigned long  lCount;
    char           caTitle[127];     /* ����         */
    char           cFlags;           /* Mothed       */
} ncHttpHashBuffer;




// �µ�HttpBuf�ṹ

typedef struct ncHttpBuf_s {
    unsigned long  lLastTime;        /* ��ֹʱ��     */
    unsigned char  cUseFlags;        /* ��ʶ��0--������ 1--������ 2--���Դ��� 3--�ѱ���  */
    unsigned char  cMethod;          /* Http����     */
    unsigned char  cContent;         /* �������ݱ�־ 0-��  1-��ʼ  2-������   */
    unsigned char  cZip;             /* 0-��ѹ��  1-��ѹ��                    */
    ncHttpHashBuffer      *psHash;   /* ��Ӧ�� ncHttpHashBuffer��ָ��         */
    char     caHost[64];             /* Host                                  */
    char     caUrl[128];             /* Url                                   */
} ncHttpBuf;





typedef struct ncService_s {
    char             caName[32];      /* ��������  */
    unsigned long    lServices;       /* ��������  */
} ncService;




/* �ʼ���ع��˹���  */
typedef struct ncMailFilter_s {
    int    tid;                  /* ���ID */
    char   uname[128];
    char   caFrom[128];
    char   caTo[128];
    char   caSub[128];
    char   caCont[128];
    char   caAtt[128];
    int    lSize;
    int    lAsize;
    int    lTonum;
    char   cStype;            /* 0--ȫ��  1--����  2--����  */
    char   cUflags;           /* 1--��    2--�û�           */
    char   cTzflags;          /* 0--��֪ͨ  1--֪ͨ         */
    char   cSaveflags;        /* 0--������  1--����         */
    char   caTzmail[64];
} ncMailFilter;


/* �����ʼ�������� */
typedef struct ncMailCtlType_s {
    int    tid;                  /* ���ID */
    char   uname[128];
    char   caFrom[128];
    char   caTo[128];
    char   caSub[128];
    char   caCont[128];
    char   caAtt[128];
    int    lSize;
    int    lAsize;
    int    lTonum;
    char   cStype;            /* 0--ȫ��  1--����  2--����  */
    char   cUflags;           /* 1--��    2--�û�           */
} ncMailCtlType;

typedef struct ncUploadFilter_s {
    int    tid;                  /* ���ID */
    char   uname[128];
    char   caHost[128];
    char   caUrl[128];
    char   caCont[128];
    char   caAtt[128];
    int    lSize;
    int    lAsize;
    char   cUflags;           /* 1--��    2--�û�           */
    char   cTzflags;          /* 0--��֪ͨ  1--֪ͨ         */
    char   cSaveflags;        /* 0--������  1--����         */
    char   cReserve;          /* ����                       */
    char   caTzmail[64];
} ncUploadFilter;



typedef struct ncPostFileList_s {
    char     pOriginFile[128];   /* Դ�ļ�    */
    char     pType[12];          /* �ļ�����  */
    char     pOutFile[64];
    unsigned long lSize;
    struct ncPostFileList_s *psNext;
} ncPosFileList;



typedef struct ncPostDataContent_s {
    unsigned long  lBytes;         /* ���ֽ���  */
    unsigned long  lTime;          /* ʱ��      */
    char           caName[32];     /* �û���    */
    char           caHost[128];    /* ������    */
    char           caUrl[128];     /* ����URL   */
    char           *pValue;        /* ��������  */
    int            iSum;           /* ��������  */
    ncPosFileList  *psFileList;    /* �ļ��б�  */
} ncPostDataContent;


typedef struct ncPostFList_s {
    char     *pOriginFile;       /* Դ�ļ�     */
    char     *pType;             /* �ļ�����   */
    char     *pOutFile;          /* ����ļ�   */
    unsigned long lSize;
    struct ncPostFileList_s *psNext;
} ncPostFList;

typedef struct ncPostVarList_s {
    char          *pVar;              /* ������    */
    char          *pValue;            /* ����      */
    struct ncPostVarList_s *psNext;
} ncPosVarList;

/* ������(New)  */
typedef struct ncPostDataList_s {
    unsigned long  lBytes;         /* ���ֽ���  */
    unsigned long  lTime;          /* ʱ��      */
    unsigned long  lUserid;        /* �û�ID    */
    char           caHost[128];    /* ������    */
    char           caUrl[128];     /* ����URL   */
    int            iSumVar;        /* ��������  */
    ncPosVarList   *psVarList;     /* �����б�  */
    int            iSumFile;       /* ��������  */
    ncPosFileList  *psFileList;    /* �ļ��б�  */
} ncPostDataList;



#define NC_IPPKG_UPFLOW      0
#define NC_IPPKG_DOWNFLOW    1

typedef struct ncIpPkgInfo_s {
    long8    lIpFlow[2];           /*  0 Upload Bytea  1  DownBytes  */
    unsigned long lStartTime;      /*  ��ʼʱ��                      */
    long8    lSumPkg;              /*  �����ݰ�                      */
    unsigned long lpPkgPerSec;     /*  ÿ�����ݰ�                    */
    unsigned long lStartCountTime; /*  ��������ʼʱ��                */
    long8    lPkgCount;            /*  ���ݰ�������                  */
    unsigned long lCount;          /*  ���ʴ���                      */
    unsigned long lStartControl;   /*  ʱ����Ƶ���ʼʱ��            */
    char          caFilename[128];
} ncIpPkgInfo;

typedef struct ncSumControl_s {
    unsigned long  lStartTime;       /* ��ʱ��                     */
    unsigned long  lLastTime;        /* ������ʱ��                 */
    unsigned long  lTimeOut;         /* ���ʱʱ��                 */
    unsigned short nLock;            /* ��  0--����   1--���ڼ��    */
    unsigned short nR1;              /*                               */
    unsigned long  lR1[2];           /* ����                          */
} ncSumControl;

/* ������Ϣ ��1��ʼ  */
#define NC_CONTROL_NOUSE           0
#define NC_CONTROL_DELETE          1
#define NC_CONTROL_TABDROP         5
#define NC_CONTROL_MODPORT         6
#define NC_CONTROL_DISPERR         7

/* Iptables �Ŀ���̨����           */


struct ncTestRemote_s {     /* Զ���ļ�����  */
    char  caIp[16];
    char  caHost[64];
    char  caUser[32];
    char  caPass[32];
    char  caMount[64];
    char  caShare[32];
    char  caSdate[12];
    char  caEdate[12];
    char  caOpt[12];       /* ����       */
};

/* ��ΪRemoteMountʱ cPort��ֵΪ���������  */
#define NC_REMOTE_MOUNT         1
#define NC_REMOTE_MOUNTTEST     2
#define NC_REMOTE_MAILBACK      3
#define NC_REMOTE_UNMOUNT       4
#define NC_REMOTE_MAILLOAD      5
#define NC_REMOTE_MAILEXPORT    6    /* ���ݵ���  */
typedef struct ncControl_s {
    unsigned long  lSip;             /* ��ʼIP    */
    unsigned long  lDip;
    unsigned short nDport;
    unsigned char  cProt;            /* Э��                         */
    unsigned char  cUse;             /* ������ĺ�                   */
    unsigned long  lLastTime;        /* ��ʼʱ��                     */
    unsigned long  lStop;            /* ��ֹʱ��                     */
    int            iIndex;
} ncControl;

typedef struct ncIpControlInfo_s {
    unsigned long  lSip;             /* ��ʼIP                      */
    unsigned long  lDip;
    unsigned short nDport;
    unsigned char  cProt;            /* Э��                        */
    unsigned char  cComm;            /* ���� 0--Add 1--Del 2--Flush */
    unsigned long  lStop;            /* ��ֹʱ�� hhmmss             */
    unsigned long  lId;              /* ControlId                   */
} ncIpControlInfo;

typedef struct ncDevList_s {
    char caDev[32];
    char caIp[32];
    char caNetmask[32];
    char caBoard[32];
    struct ncDevList_s *next;
} ncDevList;


#define  NC_PID_COLLECT        1     /* ���ݲɼ�     */
#define  NC_PID_SYSLOG         0     /* ϵͳ��־     */
#define  NC_PID_TABLECTL       2     /* IpTable����  */
#define  NC_PID_NTSYNC         3     /* ��ͬ��       */
#define  NC_PID_ONTIME         4     /* ��ʱ         */
#define  NC_PID_ICAUTH         5     /* ��������֤   */
#define  NC_PID_CHECKSOFT      6     /* ���Software */
#define  NC_PID_SMTPPROXY      7     /* Smtp Proxy   */
#define  NC_PID_LISTEN25       8     /* ��������     */
#define  NC_PID_HTTPPROXY      9     /* Http Proxy   */
#define  NC_PID_POP3PROXY     10     /* Pop3 Proxy   */
#define  NC_PID_NTAUTH        11     /* NT�����֤   */
#define  NC_PID_GETPKG        12     /* ���ݴ���      */
#define  NC_PID_PKGDO         13     /* ���ݲɼ�      */
#define  NC_PID_ASYNC         14     /* �첽��־����  */
#define  NC_PID_MAILMON       15     /* �ʼ����ݼ��  */
#define  NC_PID_POSTMON       16     /* �����ݼ��  */
#define  NC_PID_CHECKMAC      17     /* Mac��ַ���   */
#define  NC_PID_SYSMON        18     /* Mac��ַ���   */
#define  NC_PID_DBSYNC        19     /* ����ͬ������  */
#define  NC_PID_FILESRV       20     /* �ļ�������   */
#define  NC_PID_DODBLOG       21     /* �������ݿ�ͬ����Ϣ  */
#define  NC_PID_UPDATE        22     /* �Զ���������       */
#define  NC_PID_MAILFILTER    23     /* �ʼ����       */
#define  NC_PID_AUTOBAK       24     /* �Զ����ݽ���  */
#define  NC_PID_AUTOCLEAN     25     /* �Զ��������  */
#define  NC_PID_VIRUSCHECK    26     /* ��鲡��ɨ������  */
#define  NC_PID_NWCHECKCOMP5  27     /* ����������߼������  */
#define  NC_PID_DBCHECK       28     /* ���ݿ�����޸�      */  
#define  NC_PID_IMFILE        29     /* ���MSN�ļ�     */
#define  NC_PID_PROXYCHECK    30     /* ���Proxy       */
#define  NC_PID_PORTBASE      31     /* ���ڶ˿ڹ���    */
#define  NC_PID_CHECKNETINFO  32     /* ���������Ϣ    */
 
#define  NC_RELOAD_ALL            0  /* ȫ��        */
#define  NC_RELOAD_NETWORK        1  /* ��������    */
#define  NC_RELOAD_LIMIT          2  /* ��������    */
#define  NC_RELOAD_MAILMONFILTER  3  /* �ʼ����˹��� */
#define  NC_RELOAD_PQCONFIG       4  /* �������ò��� */
#define  NC_RELOAD_DEBUG          5  /* Debug����    */
/* ������Ϣ  
     �����ڹ����ڴ���
 */
typedef struct ncProcessInfo_s {
    int           iPid;                /* ����ID                                        */
    unsigned long lStartTime;          /* ����ʱ��                                      */
    unsigned long lTimeOut;            /* ��ʱʱ��                                      */
    unsigned long lOntime;             /* ��ÿ��ָ��ʱ����������                        */
    unsigned short nPrio;              /* ���ȼ�                                        */
    unsigned short nNum;               /* ����˳���                                    */
    unsigned char caControl[28];       /* ������Ϣ                                      */
    char     caName[32];               /* ��������                                      */
    int      (*fFunName)(utShmHead *); /* ������                                        */
    unsigned long lLastTime;           /* ����������ʱ��                              */
    unsigned long lStepTime;           /* �����ʱ�䣬ͨ��������ʱ���������������    */
    int      iErrorNo;                 /* ���������������                              */
    int      iFlags;                   /* 0--��  1--�ڲ�����  2--�ⲿ   8--��������     */
    unsigned long lRunSec;             /* CPUʱ�� ��                                    */
    unsigned long lRunUsec;            /* CPUʱ�� ����                                  */
    unsigned long lSysSec;             /* ϵͳʱ��                                      */
    unsigned long lSysUsec;            /* ռ���ڴ�                                      */
    unsigned long lGetTime;            /* ȡ��ʱ��                                      */
} ncProcessInfo;

/* ���̹����еĵ�һ����¼   */
typedef struct ncProcessHead_s {
    int             iPid;                      /* �����̵Ľ��̺�    */
    unsigned long   lStartTime;                /* ����ʱ��          */
    int             iDo;                       /* �����ʶ 0--����  */
    int             iFlags;                    /* iFlags == 0-���������Ϣ 1--����������Ϣ 2---ֱ�ӻָ� */
    unsigned long   lLastTime;                 /* ����������ʱ��  */
    
} ncProcessHead;


    

/* ������Ϣ     */   
#define NC_PROC_END         2     /* ���̱�ǿ����ֹ   */
#define NC_PROC_RESET       8     /* ������Ҫ����  */
#define NC_PROC_STOP        9     /* ������Ҫֹͣ  */


typedef struct ncEimProcess_s {
    char     caName[32];           /* ����                          */
    char     caPath[64];           /* ·��                          */
    char     caFun[32];            /* ��������                      */
    int      iPrio;                /* ���ȼ�                        */
    int      iPid;                 /* 0 ��ʾû������                */
    unsigned long lStartTime;      /* ����ʱ��                      */
    unsigned long lTimeOut;        /* ��ʱʱ��                      */
    unsigned long lOnTime;         /* ��ÿ��ָ��ʱ����������        */
    unsigned char caControl[32];   /* ������Ϣ                      */
    int      iStatus;              /* 0--������   1--����   -1 ���� */
    int      iFlags;               /* 1--������   2--�����         */
} ncEimProcess;


#define  NC_ONTIME_IPCHECK        0   /* ��ʱ���IP����             */
#define  NC_ONTIME_PROCESSCHECK   1   /* ��ʱ������               */
#define  NC_ONTIME_COUNT          2   /* ����ͳ��                   */
#define  NC_ONTIME_DELDETAILLOG   3   /* �����ϸ��Ϣ               */
#define  NC_ONTIME_DELTEMPFILE    4   /* ɾ����ʱ�ļ�               */
#define  NC_ONTIME_DATABACK       5   /* ���ݱ���                   */
#define  NC_ONTIME_CHECKIPTAB     6   /* ���IPtables               */
#define  NC_ONTIME_RELOADCNF      7   /* �����������               */
#define  NC_ONTIME_TIMECONTROL    8   /* ���ʱ�����               */
#define  NC_ONLINE_RESTARTNC      9   /* ��������NC                 */


typedef struct ncOnTimeFun_s {
    unsigned long lLastTime;            /* ���һ��ִ��ʱ��  */
    unsigned long lUsedTime;            /* ���һ��ִ��ʹ��ʱ��  */
    unsigned long lCount;               /* ִ�д���      */
    unsigned long lStepTime;            /* ���ʱ��      */
    unsigned long lOnTime;              /* ָ��ʱ�� hhmmss       */
    int      iStatus;                   /* 0--����  1--�ȴ� 2--����ִ��  */
    char     caName[32];                /* ��������      */
    int   (*fFunName)(utShmHead *);     /* ִ�к�������  */
} ncOnTimeFun;


/* �ʼ����˹���  */
typedef struct ncMailCtl_s {
    unsigned long lId;
    char caUname[128];
    char caFrom[128];
    char caTo[128];
    char caSub[128];
    char caCont[128];
    char caAttname[128];
    unsigned long lMsize;
    unsigned long lAsize;
    unsigned long lTonum;
    unsigned char cFlags;     /* ���Ʊ�ʶ  2--���� 1--��  0-- ȫ�� */
    unsigned char cActflags;  /* ����ʽ 0--���� 1--������  */
    unsigned char control;    /* �ʼ����� 1--���� 2--�ܾ�  3--����ָ����ַ  4--�ȴ�ȷ�� */
    unsigned char cTzflags;   /* ֪ͨ��ʽ 0--��֪ͨ  1--֪ͨ������  2--�ռ��� 3--ָ������  */
    unsigned char cStype;     /* ������� 0--ȫ��                                          */
    unsigned char cReserv[3];
    char caSmail[64];         /* ת������        */
    char caTzmail[64];        /* ֪ͨ�ʺ�        */
    char caTzcont[128];       /* ֪ͨ����        */
} ncMailCtl;

/* Add 2002/11/30 */
typedef struct ncFieldList_s {
    char   caFname[32];
    char   cType;          /* ���� n-���� s-�ַ� */
    char   cFlags;
    short  nLen;
} ncFieldList;

/* ������Ϣ  */
extern int iDebugLevel;
#define NC_DEBUG_ERROR    1            /* ������Ϣ  */
#define NC_DEBUG_WARN     2
#define NC_DEBUG_LOG      4
#define NC_DEBUG_MSG      8
#define NC_DEBUG_SHMLOAD 16

#define NC_DEBUG_TCOUT   116           /* ���TC������Ϣ  */



/* gtype    ������    */
#define NC_GTYPE_LOCAL      0           /* ����       */
#define NC_GTYPE_USER       1           /* �����û�   */
#define NC_GTYPE_GROUP      2           /* ��         */
#define NC_GTYPE_COMPANY    3           /* ��˾       */
#define NC_GTYPE_POLICY     4           /* ����       */
#define NC_GTYPE_CENTER     8           /* ����       */
#define NC_GTYPE_SYSTEM     9           /* ϵͳ       */



/* DHCP���ݽṹ  
   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     op (1)    |   htype (1)   |   hlen (1)    |   hops (1)    |
   +---------------+---------------+---------------+---------------+
   |                            xid (4)                            |
   +-------------------------------+-------------------------------+
   |           secs (2)            |           flags (2)           |
   +-------------------------------+-------------------------------+
   |                          ciaddr  (4)                          |
   +---------------------------------------------------------------+
   |                          yiaddr  (4)                          |
   +---------------------------------------------------------------+
   |                          siaddr  (4)                          |
   +---------------------------------------------------------------+
   |                          giaddr  (4)                          |
   +---------------------------------------------------------------+
   |                                                               |
   |                          chaddr  (16)                         |
   |                                                               |
   |                                                               |
   +---------------------------------------------------------------+
   |                                                               |
   |                          sname   (64)                         |
   +---------------------------------------------------------------+
   |                                                               |
   |                          file    (128)                        |
   +---------------------------------------------------------------+
   |                                                               |
   |                          options (variable)                   |
   +---------------------------------------------------------------+
   */
#define MACIP_FROM_PKG           0      // ��Դ�����ݰ�
#define MACIP_FROM_ARP           1
#define MACIP_FROM_WINS          2
#define MACIP_FROM_THIRD         3
#define MACIP_FROM_CLIENT        4
#define MACIP_FROM_SNMP          5
#define MACIP_FROM_SWITCH        6
#define MACIP_FROM_DHCP          7
#define MACIP_FROM_POST          8
#define MACIP_FROM_FIXED         9
#define MACIP_FROM_PROXY         10
#define MACIP_FROM_RADIUS        11
#define MACIP_FROM_CAMS          12
#define MACIP_FROM_PORT          13
#define MACIP_FROM_VLAN          14
#define MACIP_FROM_POP3          15    // POP2�ʺ�
#define MACIP_FROM_PPPOE         17    // PPPOE�ʺ�
#define MACIP_FROM_QQ            18    // QQ�ʺ�
#define MACIP_FROM_LOCALAUTH     20    // ������֤   LocalAuth






#define DHCP_MSG_DHCPDISCOVER    1
#define DHCP_MSG_DHCPOFFER       2
#define DHCP_MSG_DHCPREQUEST     3
#define DHCP_MSG_DHCPDECLINE     4
#define DHCP_MSG_DHCPACK         5
#define DHCP_MSG_DHCPNAK         6
#define DHCP_MSG_DHCPRELEASE     7
#define NC_MAIPFILE_ID           19650429L
typedef struct ncDhcp_s {
    unsigned char  op;       /* 1:from Client  2:from Server */
    unsigned char  htype;    /* 1:Ethernet, 7:Arcnet         */
    unsigned char  hlen;     /* MAC��ַ���� 6                */
    unsigned char  hops;     /* ת������                     */
    unsigned long  xid;      /* Transaction ID               */
    unsigned short secs;
    unsigned short flags;
    unsigned long  ciaddr;   /* ĿǰClient��ʹ�õ�IP��ַ     */
    unsigned long  yiaddr;   /* Ҫ�����IP��ַ               */
    unsigned long  siaddr;
    unsigned long  giaddr;
    unsigned char  chaddr[16];
    unsigned char  sname[64];
    unsigned char  file[128];
} ncDhcp;



typedef struct ncComputeMacIp_s {
    unsigned char  mac[6];      /* Mac��ַ                      */
    char           flags;       /* ��¼��Դ 1--�̶����� 8--Wins 9--DHCP */
    char           status;
    unsigned long  ip;          /* Ip��ַ                       */
    unsigned char  cname[16];   /* ������,����15λ�Զ��ص�      */
} ncComputeMacIp;

#define NC_ARPATTR_MAXLEN      80
#define NC_ARPATTR_USERNAME     1
#define NC_ARPATTR_DISPNAME     2
#define NC_ARPATTR_COMPUTER     3
#define NC_ARPATTR_POSITION     4
#define NC_ARPATTR_CARDID       5
#define NC_ARPATTR_VLANID       6
#define NC_ARPATTR_GROUP        7
#define NC_ARPATTR_ROLE         8


/* �����ݴ�����ڴ��У����ڲ�����Ӧ�����������Ϣ    */
typedef struct ncComputeIpMac_s {
    unsigned long  ip;          /* Ip��ַ                                   */
    char           flags;       /* ��¼��Դ 1--�̶����� 8--Wins 9--DHCP      */
    char           status;      /* ��ǰ״̬ 0--������  1--����               */
    unsigned char  mac[6];      /* MAC��ַ                                  */
    unsigned long  lLastTime;   /* ������ʱ��                              */
    unsigned char  attr[80];    /* ����, u:uname,d:disp,g:group             */
} ncComputeIpMac;


/* �ֵ�����    */
#define NC_DICT_MAXVAR           8    // һ����¼���������������
#define NC_DICT_USERNAME         1    // �û���
#define NC_DICT_DISPNAME         2    // ��ʾ��
#define NC_DICT_MAC              3    // MAC��ַ
#define NC_DICT_GROUP            4    // ��������
#define NC_DICT_COMPNAME         5    // �������
#define NC_DICT_POSITION         6    // λ��
#define NC_DICT_ROLE             7    // �û���ɫ

/* �ֵ���Դ    */
#define NC_DICTFROM_USERDEFINE   1    // �û��Զ�
#define NC_DICTFROM_CLIENT       2    // �ͻ���
#define NC_DICTFROM_NTDOMAIN     3    // NT��


/* �ֵ�����    */
#define NC_DICTFLAGS_DYNAMIC     1    // ��̬����, �����ʵ����ݸ��ݳ�ʱʱ���Զ�ɾ��
#define NC_DICTFLAGS_KEEP        2    // ����,�����ʵ����ݽ���Զ���ڴ���



/* �����ֵ���Ϣ   
   #   IP       From   Content                                        LastTime
   192.168.0.1  1      "1liyunm;2������;311:11:22:22:11;4���۲�"     "2009/03/29 11:12:12"
 */
typedef struct ncOnlineDictionary_s {
    unsigned  long  ip;            /* �ͻ���IP��ַ                          */
    unsigned  long  lasttime;      /* ������ʱ��                          */
    unsigned  char  cFrom;         /* ��Դ,�� NT�ͻ���,DHCP,���            */
    unsigned  char  cFlags;        /* ��ʱ��ʶ 0--��̬  1--��̬  */
    unsigned  char  cRev[2];
    unsigned  char  content[116];  /* �ֵ�����         */
} ncOnlineDict;

typedef struct ncDictionaryVar_s {
    int             cType;           /* ���       */
    unsigned char   caValue[32];     /* �ֵ�����   */
} ncDictVar;


#define NC_SWITCH_TL_SL3226P  2001
#define NC_SWITCH_TL_SL3210   2002


/* �������˿���Ϣ  */
typedef struct ncSwitchPort_s {
    unsigned long  lSid;          /* ���к�              */
    unsigned long  lSwid;         /* ������ID             */    
    char           caPort[16];    /* �˿�                 */
    char           caName[32];    /* λ��                 */
    unsigned char  mac[6];
    char           status;        /* �˿�״̬  2--Down 1-Up   0 None    */
    char           flags;         /* 1 ��������  0-- δ��������   9 ����       */
} ncSwitchPort;

typedef struct ncSwitchInfo_s {
    unsigned long  lSwid;         /* ������ID             */
    char           caName[32];    /* ����������           */
    char           caType[32];    /* ����                 */
    unsigned long  ip;            /* Ip��ַ               */
    unsigned short nPort;         /* �˿�                 */
    char           status;        /* ״̬                 */
    char           flags;
    char           caUsername[16];
    char           caPassword[16];
    int            iStart;
    int            iSumPort;
    unsigned long  lLastTime;     /* ���ʱ��             */
} ncSwitchInfo;

/* ����������ʽ   */
typedef struct ncSwitchDo_s {
    char  caType[32];          /* ����������   */
    int   iWay;                /* ������ʽ     23-Telnet 80-Http 62-SNMP  */
    int   (*fSwitchGetMac)(ncSwitchInfo *,ncSwitchPort *,int);    /* ȡMac��ַ�Ͷ˿� */
} ncSwitchDo;

/* �������б�   */
typedef struct ncSwitchList_s {
    char  caType[32];          /* ����������   */
    int   iFlags;
    int   iWay;
} ncSwitchList;


typedef struct ncUtlFileList_s {
    char name[64];                  /* �ļ���     */
    unsigned long lSize;            /* �ļ�����   */
} ncUtlFileList;


typedef struct ncUtlDBSynInfo_s {
    int           iFileServerId;    /* �ļ��������Ľ��̺� */
    int           iProcessId;       /* ���ݴ������ID     */
    unsigned long lStartTime;       /* ��ʱ��           */
    unsigned long lMailMon;         /* ����ʼ�������     */
    unsigned long lFormMon;
    unsigned long lMailCtl;
    unsigned long lDbLog;
    unsigned long long lBytes;      /* ͬ��������       */
    unsigned long lLastTime;        /* ���һ��ͬ��ʱ�� */
    unsigned long lLastDoTime;      /* ���һ�δ���ʱ��  */
    unsigned long lUndoFile;        /* δ������ļ����� */
    char     caDoFile[128];         /* ��ǰ���ڴ��͵��ļ�  */
    char     caCurFile[128];        /* ���ڴ���            */
} ncUtlDBSynInfo;

typedef struct ncMailList_s {
    char mail[60];                  /* �ʼ���ַ */
    char cRev[2];
    char cdo;                       /* ����ʽ */
    char cflags;
} ncMailList;

typedef struct ncGroupIpList_s {
    unsigned long lSip;
    unsigned long lEip;
    unsigned long lGroupid;
} ncGroupIpList;

#define  NC_PID_GETSMTPIP1    29     /* ȡ��Proxy���ձ�  */
#define  NC_PID_GETSMTPIP2    30     /* ȡ��Proxy���ձ�  */
#define  NC_PID_GETSMTPIP3    31     /* ȡ��Proxy���ձ�  */
#define  NC_PID_GETSMTPIP4    32     /* ȡ��Proxy���ձ�  */
#define  NC_PID_MAILSEND      33     /* �����ʼ�����ϵͳ  */
#define  NC_PID_PASPROXY      34     /* ����PASProxy����  */
#define  NC_PID_NCCLIENT      35

/* ��Ʒ������Ϣ, ��1024�ֽ�
     �����ļ���DES�㷨����
 */
#define NC_PKS_ID             20060928L
typedef struct ncProdSysInfo_s {
    unsigned long  lSysId;          /* ϵͳID            */
    unsigned long  lLen;            /* �ļ�����          */
    unsigned long  lCheck;          /* У��λ,ʹ��Md5(ID)����,������ȷ��ϵͳ���������� */
    unsigned long  lCreateTime;     /* ����ʱ��          */
    unsigned long  lMgrIp1;         /* �������ĵ�ַ      */
    unsigned long  lMgrIp2;         /* �������ĵ�ַ      */
    unsigned long  lProdSrv1;       /* ��Ʒ���ĵ�ַ      */
    unsigned long  lProdSrv2;       /* ��Ʒ���ĵ�ַ      */
    unsigned short nMgrPort;        /* �������Ķ˿�      */
    unsigned short nMgrProxy;       /* ��������Proxy�˿� */
    unsigned short nProdPort;       /* ��Ʒ���Ķ˿�      */
    unsigned short nUpdatePort;     /* �����˿�          */
    unsigned long  lRev[2];
    unsigned char  prodid[32];      /* ��Ʒ���к�        */
    unsigned char  uname[64];       /* ��Ȩ�û�          */
    unsigned char  dbname[32];      /* ���ݿ�����        */
    unsigned char  dbuser[16];      /* ���ݿ��û�        */
    unsigned char  dbpass[16];      /* ���ݿ����        */
    unsigned char  cltname[32];     /* �ͻ�������        */
    unsigned char  cltkey[32];      /* ��¼��Կ          */
    unsigned char  UpdateHost[64];  /* ��Ʒ����������    */
} ncProdSysInfo;

typedef struct ncCaseWarnMsg_s {
    unsigned long lCid;
    unsigned long lSid;                /* ����ID, x00000001   */
    unsigned char acid;               /* ��������ID  */    
    char          caMsg[255];
} ncCaseWarnMsg;


/* �ڰ�����     MAC��ַӦ��Ψһ
   ��mac��ַΪ������б�
 */

typedef struct ncBlackWhite_s {
    unsigned long  ip;
    unsigned char  position[32];
    unsigned char  mac[6];
    unsigned char  flags;
    unsigned char  cLen;
} ncBlackWhite;



/* ���������������Ϣ
 */
typedef struct  ncVidList_s {
    unsigned long   lSip;          //      --- ԴIP��ַ
    unsigned short  lVtype;        //      --- ����������
    unsigned short  nNum;          //      --- ���
    unsigned char   caName[40];    //      --- �����������
    unsigned char   caDisp[64];    //      --- ���������ʾ��
    unsigned char   caPass[24];    //      --  ����,����
    unsigned long   lSid;          //      --- ��Ӧ�����ݿ��е�Sid
    unsigned long   lDip;          //      --- Ŀ��IP��ַ
    unsigned long   lStartTime;    //      --- ��ʼ����ʱ��
    unsigned long   lLastTime;     //      --- ������ʱ��
    unsigned long   lWarnTime;     //      --- �澯ʱ��
    unsigned long   lUserId;       //      --- ��Ӧ���û�ID
    unsigned long   lUsid;         //      --- ��Ӧ��������Sid
    unsigned char   flags;         //      --- 1-����   0-����   9-����
    unsigned char   status:4;      //      --- 0-����   1-��ֹʹ��
    unsigned char   cUp:4;         //      --- �Ƿ��ϴ�����������
    unsigned short  nRev;          //      --- ��¼����
}   ncVidList;




/* ������ݿ���,������������ڵĶ��Ǳ����õ�

 */
typedef struct  ncVidDrop_s {
    unsigned long  lSip;          //      --- ԴIP��ַ
    unsigned long  lVtype;        //      --- ����������
    unsigned long  lLastTime;     //      --- ������ʱ��
    unsigned char  cStatus;       //      --- ״̬
    unsigned char  cFun;          //      --- ��ֹ�Ĺ���
    unsigned char  rev[2];        //      --- ����
} ncVidDrop;


/* ͬ�������û�  */
typedef struct ncSynOnUser_s {
    unsigned long  ip;            /* Ip��ַ                       */
    unsigned char  mac[6];
    char           flags;         /* ��¼��Դ 1--�̶����� 8--Wins 9--DHCP */
    char           status;        /* ״̬ 0--������  1-����   */
    char           name[32];      /* �û��ʺ�   */
    char           dispname[32];  /* ��ʾ��     */
    char           group[32];     /* ����       */
    unsigned long  lasttime;      /* ������ʱ��  */
} ncSynOnUser;


typedef struct ncSensContent_s {
    unsigned char  ctype;         /* ƥ�����     0-����  1-������ʽ  2-��չ����  */
    unsigned char  ccase;         /* ��Сд����   0-������  1-����                  */
    unsigned short len;           /* �ַ�������                                     */
    regex_t        sreg;          /* ����������  */
} ncSensContent;

/* �������ݼ����   */

typedef struct ncSensContIndex_s {
    unsigned long  id;        /* ���      */
    char           name[32];  /* ˵��      */
    unsigned long  sum;       /* ����      */
    ncSensContent  *psCont;    
} ncSensContIndex;


typedef struct ncSensContHead_s {
    int  iSumIndex;
    ncSensContIndex *psIndex;
} ncSensContHead;


typedef struct ncCommandBuf_s {
    int    iResetFlags;          /* ����������ʶ   1--����ȫ��NC   2--������Web���NC   3--ֹͣ��Web���NC   4-ֹͣȫ��NC  5-����ָ��NC */
    int    iLock;                /* 1-����      */
    char   caText[128];          /* ˵��        */
    char   caName[32];           /* ��������    */
} ncCommandBuf;

// ���ؾ����豸
#define NC_PKGBLAN_MAX  16
typedef struct ncPkgBlance_s {
    long long   lUBytes;                /* ����             */
    long long   lDBytes;                /* ����             */
    long long   lUPkg;                  /* �������ݰ�       */
    long long   lDPkg;                  /* �������ݰ�����   */
    unsigned short nMark;               /* Markֵ           */
    long        lBase;                  /* ����             */
} ncPkgBlance;

typedef struct ncPkgBlanceHead_s {
    long        lSum;                   /* ��������         */
    ncPkgBlance *psBlan;
} ncPkgBlanceHead;

    

#define NC_MAX_NAMECACHE      1000   // ������ֻ���
#define NC_CACHE_WEBCLASS  1
#define NC_CACHE_DEVNAME   2
#define NC_CACHE_POLTYPE   3
#define NC_CACHE_BILLTYPE  4

#define NC_IPCTL_BY_NONE          0
#define NC_IPCTL_BY_ICMP          1
#define NC_IPCTL_BY_IPT           2
#define NC_IPCTL_BY_TCP           3
#define NC_IPCTL_BY_NETWORK       4
#define NC_IPCTL_BY_NETWORKP2P    5
#define NC_IPCTL_BY_META          6
#define NC_IPCTL_BY_NSA           7
#define NC_IPCTL_BY_NOCTL         9


typedef struct ncGroupFlow_s {
    unsigned long lGroupId;
    unsigned char caName[32];
    unsigned long lTime;                   /* ����ʱ��             */
    unsigned long long lUp[2];             /* 0--�ϴΡ�����--����  */
    unsigned long long lDown[2];
} ncGroupFlow;


// Mac��ַ��   
// NC_LNK_MACIPLIST 90
// flags �ĺ���
//   10-19  �������
//   
#define NC_MACTABFROM_PPPOE         20      // PPPOE


typedef struct ncMacTables_s {
    unsigned char  mac[6];      /* Mac��ַ                      */
    unsigned char  flags;       /* ��¼��Դ PPPOE,...           */
    unsigned char  status;      /* 1 ������  0 δ������         */
    unsigned long  ip;          /* Ip��ַ                       */
    unsigned long  lasttime;    /* ������ʱ��                 */    
    unsigned char  name[32];    /* ����                         */
} ncMacTables;


/* ��Ź�Ʊ�����Ĺؼ�������*/
typedef struct ncStockContext_s {
    unsigned long       lId;        /* ���� */
    unsigned long       lLen;        /* ���ݳ��� */
    unsigned long       lLasttime;   /*���ʱ��*/
    unsigned char       cont[64];    /*����*/
} StockContext;


/* �������Ͻ��յ��û���Ϣ*/
typedef struct ncMwduser_s {
	  char caName[32];               /*�ʺ�*/
	  char caDispname[32];           /*����*/
	  char caSex[4];                  /*�Ա�*/
	  char caIdtype[4];               /*֤������*/
	  char caIdno[20];                /*֤������*/
	  char caAddr[64];                /*��ͥ��ַ*/
	  char caMobile[20];              /*�ֻ���*/
	  char caIp[16];                 /*IP��ַ*/
	  char caMac[20];                 /*mac��ַ*/
	  char caTime[16];                /*ʱ��*/
	  char caPosition[32];            /*λ��*/
	  char caCountry[4];              /*���Ҵ���*/
} ncMwduser;

/* ��Ŷ���è�ֻ��û�*/
typedef struct ncSmsuser_s {
	  char caMobno[20];               /*�ֻ���*/
	  char caPass[32];                /*����*/
} ncSmsuser;


/* ��֤��Ϣ    */
typedef struct ncAuthRoute_s {
    unsigned    long    lSip;
    unsigned    long    lEip;
    unsigned    long    lAuthIp;
    unsigned    short   nPort;
    unsigned    short   nRev;
    char                caPlate[64];
}   ncAuthRoute;

/*����������֤���ص��û���Ϣ*/
 typedef struct ncHoteluser_s{
   	char caDispname[32];
   	char caPosition[32];
   	char caIdfytype[4];
   	char caIdfyno[20];
   	char caFamaddr[152];
   	char caCountry[4];
   	char caSex[8]; 
   	char caUsername[32]; 
    } ncHoteluser;
//ũ������֤�û�
typedef struct ncNshUser_s{
 	  char caUsername[32];            //Ա����
 	  char caDispname[32];
 	  char ustat[8];                  //״̬    00--�ʺ�δ���� 001-��֤ͨ�� 002-�˻���ͣ�� 003-��֤δͨ�� p01 ��ʼ���룬ǿ��Ҫ���޸����� p02 �����ѹ���
 	  char nettime[12];               //��������ʱ��
 	  char startdate[20];             //����������ʼʱ��
 	  char enddate[20];               //��������ʱ��
 	  char orgname[64];               //����     
    } ncNshUser;
//�Ϻ����׼�����֤
 typedef struct ncHyuser_s {
           char caIdno[24];
           char caIdtype[8];
           char caName[20];
           char caRoom[20];
   } ncHyuser;   
    
// VlanID��λ�ö��ձ�

typedef struct ncVlanIdPos_s {
    unsigned long  lVlanId;                     // ����
    char           caPosition[32];              // λ��
} ncVlanIdPos;

// λ�ú��������ձ�
typedef struct ncPositionName_s {
    char           caPosition[32];              // λ��
    char           caPass[16];                  // ����
    char           caName[32];                  // ����
    unsigned long  lSid;                        // ��Ӧ��Sid
} ncPositionName;


/*    �ַ���ƥ��       ? -- ����   *--�����
      01 02 03 * 04 ? 05
      
      Start,Len
      Off,Len
      
*/

typedef struct ncUtlStrPat_s {
    short Len;
    short type;                     // 0--?    1-*   2--Cont
    unsigned char caCont[64];
} ncUtlStrPat;


// ���ŵ�λ���ݰ�����
// ��0��λ�ñ������ȱʡ��λ
typedef struct ncGroupDept_s {
    uint4  lSnum;           // �ڷ������˵�λ��
    uint4  lDid;            // �ڷ������˵�Id
    uchar  caDname[32];
    long long   lUp;
    long long    lDown;
    uint4  lUser;
} ncGroupDept;



// �������Э�����  */

typedef struct ncAnyNetIdFun_s {
    char caName[32];
    int (*fFunName)(utShmHead *psShmHead,ncTcp *psTcp,int iD,char *pkg,int iLen,uint4 *lService,int4 *iFlags,char *pName,char *pDisp,char *pPass);
} ncAnyNetIdFun;

typedef struct ncAnyNetIdHead_s {
    int iSumTcp;
    int iMaxTcp;
    int iSumUdp;
    int iMaxUdp;
    ncAnyNetIdFun *psUdp;
    ncAnyNetIdFun *psTcp;
    
} ncAnyNetIdHead;


#define     NC_QOS_GTYPE     2
#define     NC_QOS_ALL       3
#define     NC_DEFAULT_GROUP   99999999L

typedef struct ncLineGroupInfo_s {
    uint4  lGtype;                   // 1--����  2--��  3--ȫ��
    uint4  lGroupid;
    uint4  lLineid;
    uint4  lQosid;
    char   caLinename[32];
} ncLineGroupInfo;


#include "ncalloc.h"
#endif
