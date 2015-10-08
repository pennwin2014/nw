/* ProEIM     版权所有: 上海新网程信息技术有限公司 2000-2013
   
   2013/01/12      修改 ncLimGroup 为多级部门          
   2011/02/10        李云明                            
   2009/02/16 Review 整个数据结构                       
   2009/01/12 在 ncLimSummer 增加 qospolid,mailctlid    
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
#define   NC_VERSION             "V7.50.235"                      /* 版本号    */
#define   NC_RELEASE             "2013/08/07"                     /* 发行日期  */
#define   NC_MARK_READY          1   /* NC 就绪信息   0--未准备   1--准备就绪  */


#define   NC_MARK_NOTREADY       0  /* 未准备    */
#define   NC_MARK_ISREADY        1  /* 已准备    */

#define   NC_MAX_PROCESS        16   /* 最大进程类型  */
#define   NC_MAX_CONTROL        32   /* 最大控制变量  */
#define   NC_MAX_ONTIMEDO       16   /* 最大定时函数  */
#define   NC_DEFAULT_IP_CHECK   20
#define   NC_DEFAULT_IP_TIMEOUT 30
#define   NC_MAX_USERNUM        257
#define   NC_MAX_IPPKG          9999
#define   NC_MAX_HTTPNUM        999
#define   NC_MAX_LIMITED        100
#define   NC_MAX_MONITOR        100
#define   NC_MAX_INTERNAL       100
#define   NC_MAX_LASTIP         1000
#define   NC_MAX_DATASIZE       1000000000L     // 每个数据文件的最大长度

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

#define   NC_SHM_MAXLNK     200 /* 共享内存中最大数据链表数, 这个值必须大于NC_LNK中定义的其它值   */
/*  共享内存存放数据链表值                           */
#define   NC_LNK_THIRDAUTH  25  /* 第三方认证的用户  */
#define   NC_LNK_ALLUSER    26  /* 全部用户信息      */
#define   NC_LNK_MAILCTL    27  /* 邮件控制          */
#define   NC_LNK_CONTROL    28  /* 访问控制          */
#define   NC_LNK_IPFLOW     29  /* 总流量            */
#define   NC_LNK_MAIL       30  /* 邮件数据包        */
#define   NC_LNK_IPPKG      31  /* IP数据包          */
#define   NC_LNK_WEB        32  /* http数据包        */
#define   NC_LNK_IPLIMIT    33  /* 限制使用的IP地址  */
#define   NC_LNK_NETWORK    34  /* 要监控的IP地址  Add 2002/9/6 By Liyunming Replace
                                         NC_LNK_MONITOR NC_LNK_INTERNAL */
#define NC_LNK_PKGLOG       35  /* 重新装载控制  */
#define NC_LNK_USERINFO     36  /* 用户信息          */
#define NC_LNK_SERVICE      37  /* 众所周知的端口    */
#define NC_LNK_MMONFILTER   38  /* 邮件监控分类规则      */
#define NC_LNK_WEBLIMIT     39  /* Web访问限制       */
#define NC_LNK_IPLIST       40  /* IP地址列表        */
#define NC_LNK_IPPOL        41  /* 服务访问策略      */
#define NC_LNK_WEBLIST      42  /* Web地址列表        */
#define NC_LNK_IPPOLINDEX   43  /* 服务访问策略的索引  */
#define NC_LNK_GROUPINFO    44  /* 部门信息           */
#define NC_LNK_SUMMARY      45  /* 总体信息           */
#define NC_LNK_ONTIME       46  /* 定时程序           */
#define NC_LNK_PROCESS      47  /* 进程管理           */
#define NC_LNK_WEBPOLINDEX  48  /* 网站访问策略       */
#define NC_LNK_WEBPOL       49  /* 网站访问策略       */
#define NC_LNK_NTAUTH       50  /* NT验证             */
#define NC_LNK_FMONFILTER   51  /* 表单监控规则       */
#define NC_LNK_MCTLFILTER   52  /* 拦截邮件分类规则   */
#define NC_LNK_MAILPROXY    53  /* 收发邮件的目标IP地址  */
#define FIRE_LNK_DEFINE     54  /* 参数列表            */
#define FIRE_LNK_TABLES     55  /* 规则               */
#define FIRE_LNK_PLATE      56     /* 模板      */
#define FIRE_LNK_SUMMARY    57     /* 总体信息  */
#define NC_LNK_ICUSER       58     /* IC卡用户  */
#define NC_LNK_HTTPPROXY    59     /* http proxy    */
#define NC_LNK_POP3PROXY    60     /* pop3 proxy    */
#define NC_LNK_NOMONWEB     61     /* 免监控的网址  */
#define NC_LNK_MSNPKGBUF    62     /* MSN数据交换缓冲区,静态数组     */
#define NC_LNK_MSNUSRBUF    63     /* MSN在线用户缓冲区,Hash表       */
#define NC_LNK_CONTROLBUF   64     /* 访问控制缓冲区                 */
#define NC_LNK_BANDWIDTH    65     /* 带宽管理                       */
#define NC_LNK_PKGBUF       66     /* 数据包缓冲区                   */
#define NC_LNK_CONTBUF      67     /* 进一步处理的内容缓冲区         */
#define NC_LNK_IPUSER       68     /* 用户和IP缓存                   */
#define NC_LNK_DNSCASH      69     /* DNS Cash                       */
#define NC_LNK_HTTPCACHE    70     /* http cache                              */
#define NC_LNK_HTTPCACHEC   71     /* http cache content                      */
#define NC_LNK_ADMIPLIST    72     /* 管理员IP地址, 用来记录登录失败的IP地址  */
#define NC_LNK_IPEXCEPT     73     /* 不进行管理的例外IP地址表                */
#define NC_LNK_IPCONTACT    74     /* TCP连线信息                             */
#define NC_LNK_LANG         75    /* 语言对照表   */
#define NC_LNK_PKGSEQ       76    /* 序列号对照表, Hash表  */
#define NC_LNK_NCSRVINFO    77    /* 控制中心的有关信息    */
#define NC_LNK_NCPOLINFO    78    /* 访问控制策略索引      */
#define NC_LNK_MACLIST      79    /* 免监控的MAC地址       */
#define NC_LNK_BILLTYPE     80    /* 计费策略              */
#define NC_LNK_BILLRATE     81    /* 优惠政策              */
#define NC_LNK_BILLTIME     82    /* 时间费率              */
#define NC_LNK_BILLFLOW     83    /* 流量费率              */
#define NC_LNK_PROXYIP      84    /* Proxy的地址列表       */
#define NC_LNK_LOCK         85    /* 锁信息                */
#define NC_LNK_WEBNAME      86    /* 网址库对应的名字      */
#define NC_LNK_COMPUTE      87    /* 内网计算机列表        */
#define NC_LNK_DEVINFO      88    /* 设备信息缓存          */
#define NC_LNK_IPMACLIST    89    /* IP地址、MAC地址对照表          */
#define NC_LNK_MACIPLIST    90    /* MAC地址、IP地址、计算机名对照表*/
#define NC_LNK_DBSYNCINFO   91    /* 数据同步在线信息               */
#define NC_LNK_ONLINECOMP   92    /* 在线计算机情况                 */
#define NC_LNK_NCMAILLIST   93    /* 邮件地址黑白名单               */
#define NC_LNK_GROUPIPLST   94    /* 缺省IP和部门对照表             */
#define NC_LNK_CLTSENDBUF   95    /* 客户端实时数据发送缓冲区       */
#define NC_LNK_CLTFILTER    96    /* 客户端各种过滤规则             */
#define NC_LNK_INFOLIST     97    /* 各种列表                       */
#define NC_LNK_WARNINFO     98    /* 告警信息                       */
#define NC_LNK_HASHDATA     99    /* Hash数据                       */
#define NC_LNK_SWITCHINFO   100   /* 交换机信息                     */
#define NC_LNK_SWITCHPORT   101   /* 端口信息                       */
#define NC_LNK_NETSYSINFO   102   /* 网络流量信息                   */
#define NC_LNK_NETCAPDEV    103   /* 抓包信息                       */
#define NC_LNK_CASECTL      104   /* 案件布控信息                   */
#define NC_LNK_CASEINDEX    105   /* 案件索引                       */
#define NC_LNK_BLACKWHITE   106   /* 黑白名单                       */
#define NC_LNK_VIDLIST      107   /* 虚拟身份列表                   */
#define NC_LNK_VIDSET       108   /* 虚拟身份集                     */
#define NC_LNK_LIMVID       109   /* 虚拟身份的控制                 */
#define NC_LNK_DATEDESC     110   /* 日期的描述                     */
#define NC_LNK_VIDDROP      111   /* 禁用的虚拟身份                 */
#define NC_LNK_SYNONUSER    112   /* 同步的在线用户                 */
#define NC_LNK_SENSCONT     113   /* 敏感内容及类别                 */
#define NC_LNK_ADUSER       114   /* Ad用户状态                     */
#define NC_LNK_HOTELINFO    115   /* 宾旅馆信息                     */
#define NC_LNK_SNMPSWITCH   116   /* Snmp交换机信息                 */
#define NC_LNK_PROCCTL      117   /* 客户端进程控制策略             */
#define NC_LNK_NWWATCH      118   /* 客户端监控策略                 */
#define NC_LNK_DNSNAMESERVICE  119    /* 域名服务描述               */
#define NC_LNK_DNSIPSERVICE    120    /* Ip地址服务对照表           */
#define NC_LNK_NSACOMBUF       121    /* Nsa命令传递缓存            */
#define NC_LNK_NWJOIN          122   /* 客户端准入规则               */
#define NC_LNK_QOSID           123   /* QOS Link                     */

#define NC_LNK_POSTACC         127   /* 表单账号采集规则             */
#define NC_LNK_POSTFILTER      128   /* 表单过滤规则                 */
#define NC_LNK_NWDATEPROCY     129   /* 内网日期控制策略*/
#define NC_LNK_DICTIONARY      130   /* 数据字典                      */
#define NC_LNK_SERVICEPAT      131   /* 服务特征模式                  */
#define NC_LNK_PORTSERVICE     132   /* 端口服务对照表                */
#define NC_LNK_NOMONLIST       133   /* 免监控IP列表                  */
#define NC_LNK_COMMANDBUF      134   /* 进程控制命令缓冲区            */
#define NC_LNK_PKGFLOW         135   /* 流量信息                      */
#define NC_LNK_POSTSYN         136   /* 表单同步信息                  */
#define NC_LNK_BLANCE          137   /* 流量均衡分配                  */
#define NC_LNK_GROUPFLOW       138   /* 部门即时流量　　　　　　　　　*/
#define NC_LNK_CONTEXT         139   /* 分析股票上下文HASH表　　　　  */
#define NC_LNK_CLUSERVER       140   /* 双机系统的相关数据            */
#define NC_LNK_CLUCLIENT       141   /* 双机系统的相关数据            */

#define NC_LNK_ID2POSITION     142   /* 端口和位置对照表              */
#define NC_LNK_POSITION2NAME   143   /* 位置和旅客对照表              */
#define NC_LNK_QQNUMBER        144   /* Qq号码信息采集                */
#define NC_LNK_HTTPHASH        145   /* 存放Http的Hash表              */
#define NC_LNK_FSSERVERLNK     146   /* 文件服务器缓存                */
#define NC_LNK_DUPLICETECONT   147   /* 重复内容                      */
#define NC_LNK_DNSHOSTIP       148   /* Dns Buffer   ncdns.h          */
#define NC_LNK_DNSIPHOST       149   /* Dns Buffer   ncdns.h          */
#define NC_LNK_DYNAMICURL      150   /* 动态URL                       */
#define NC_LNK_WARNACCOUNT     151   /* 重点关注帐号                  */
#define NC_LNK_CAPURL          152   /* 采集的URL信息                  */
#define NC_LNK_CAPURLSTAT      153   /* 采集的UR状态L信息              */
#define NC_LNK_QQPKGBUF        154   /* 存放临时数据包                 */
#define NC_LNK_DBMONSESSION    155   /* 数据库连接信息                 */
#define NC_LNK_GROUPDEPT       156   /* 单位部门对照表                 */
#define NC_LNK_MWDUSER         157   /*接收迈外迪用户缓存表            */
#define NC_LNK_DOPOSTTHREAD    158   /* 表单处理进程列表               */
#define NC_LNK_SMSMOBNO         159   /*短信猫认证手机队列表            */


#define NC_SHA_POSTDO          20
/* 异步控制命令交换文件                    */
#define NC_ERR_FILEMARK     "/tmp/ncerrormark.tmp"

/* 访问控制的控制方式           */
#define NC_IPCONTROL_BY_NONE          0
#define NC_IPCONTROL_BY_ICMP          1
#define NC_IPCONTROL_BY_IPT           2
#define NC_IPCONTROL_BY_TCP           3
#define NC_IPCONTROL_BY_NETWORK       4
#define NC_IPCONTROL_BY_NETWORKP2P    5
#define NC_IPCONTROL_BY_NSA           6
#define NC_IPCONTROL_BY_NOCTL         9

/* 服务  0--9999 为系统使用
         10000 以后为自定义
 */
#define NC_SERVICE_OTHER          0        /* 未知服务           */
#define NC_SERVICE_TELNET         12001    /* Telnet             */
#define NC_SERVICE_GAME           2000     /* 游戏               */
#define NC_SERVICE_DHCP           101      /* DHCP服务            */
#define NC_SERVICE_HTTP           103      /* Web服务             */
#define NC_SERVICE_RADIUS         114      /* Radius协议          */
#define NC_SERVICE_DNS            117      /* DNS协议             */
#define NC_SERVICE_HTTPUPLOAD     4006     /* Web服务             */
#define NC_SERVICE_HTTPPROXY      3001     /* Http代理           */
#define NC_SERVICE_HTTP1          3000     /* Web服务            */
#define NC_SERVICE_SMTP           13001    /* SMTP服务           */
#define NC_SERVICE_POP            13002    /* POP3服务           */
#define NC_SERVICE_QQ             1001     /* QQ                 */
#define NC_SERVICE_VIDEO          7        /* 视频               */
#define NC_SERVICE_P2P            8        /* P2P服务            */
#define NC_SERVICE_STOCK          9        /* 股票               */
#define NC_SERVICE_MSN            1002     /* MSN                */
#define NC_SERVICE_ICQ            1008     /* ICQ                */
#define NC_SERVICE_MMS            5016     /* MMS                */
#define NC_SERVICE_FTP            4002     /* FTP                */
#define NC_SERVICE_P2P_BASE       8000
#define NC_SERVICE_SSH            12002    /* ssh                */
#define NC_SERVICE_MSNP2P         29000    /* 内部使用 */ 
#define NC_SERVICE_HTTPS          116 
#define NC_SERVICE_QQFILE         4052     /* QQ传文件   */
#define NC_SERVICE_MSNFILE        4053     /* Msn文件   */
#define NC_SERVICE_DNS            117
#define NC_SERVICE_WEBMAIL        13008  
#define NC_SERVICE_FLASH          146   /* Flash视频        */
#define NC_SERVICE_CKPAUTH        88    /* Check Point 验证 */
#define NC_SERVICE_YHMSG          1006  /* 雅虎通     */
#define NC_SERVICE_WYPP           1007  /* 网易泡泡   */
#define NC_SERVICE_XLUC           1005  /* 新浪UC     */
#define NC_SERVICE_SOUQ           1010  /* 搜Q        */
#define NC_SERVICE_SKYPE          1004  /* SKYPE      */
#define NC_SERVICE_FETION         1012  /* 飞信       */

#define NC_SERVICE_LZGAME         2001  /* 联众游戏  */
#define NC_SERVICE_GAMEONLINE     2002  /* 中国游戏在线  */
#define NC_SERVICE_CQGAME         2003  /* 传奇  */
#define NC_SERVICE_BFGAME         2004  /* 边锋  */
#define NC_SERVICE_QQGAME         2201  /* QQ游戏        */
#define NC_SERVICE_DFGAME         2202  /* 地方游戏在线  */

#define NC_SERVICE_PXYLSTD        3003  /* 绿色通道      */
#define NC_SERVICE_SQUID          3006  /* Squid 代理    */


#define NC_WEBCLASS_VIDEO         5019  /* 网上视频网址类别  */
#define NC_SERVICE_RTSP           5016


/* 2009/05/09 修正　　　  */
#define NC_P2P_QQLIVE             7001  /* QQlive     */
#define NC_P2P_PPLIVE             7002  /* PPlive|UUsee     */
#define NC_P2P_PPSTREAM           7003  /* PPStream   */
#define NC_P2P_FEIDIAN            7004  /* 沸点   */
#define NC_P2P_UUSEE              7005  /* UUsee      */
#define NC_P2P_LEIKE              7006  /* 磊客      */
#define NC_P2P_MYSEE              7007  /* MySee      */
#define NC_P2P_QINYULE            7008  /* 青娱乐      */
#define NC_P2P_BBsee              7009  /* BBsee      */
#define NC_P2P_REALMEDIA          7010  /* RealMedia  */
#define NC_P2P_WINMEDIA           7011  /* WinMedia   */
#define NC_P2P_QVOD               7012  /* 快播       */
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
#define NC_P2P_XUNLEI             8013  /* 迅雷       */
#define NC_P2P_PPDDT              8014  /* PP点点通     */
#define NC_P2P_KUGOU              6002  /* KuGou       */
#define NC_P2P_EMUTE              8001  /* eMute       */
#define NC_P2P_POCO               8018  /* POCO        */
#define NC_P2P_BEIDUXB            8019  /* 百度下巴    */
#define NC_P2P_QQCYCLONE          8020  /* QQ 超级旋风 */

#define NC_FILE_HTTP              4001  /* Http下载  */
#define NC_FILE_FTP               4002  /* FTP下载  */

#define NC_STOCK_ZHESHAN          9015  /* 浙商证券  */
#define NC_STOCK_GUOXIN           9014  /* 国信证券  */
#define NC_STOCK_FANGZHEN         9037  /* 方正证券  */

#define NC_SERVICE_SYBASE         16001
#define NC_SERVICE_ORACLE         16002
#define NC_SERVICE_MSSQL          16003
#define NC_SERVICE_MYSQL          16004
#define NC_SERVICE_DB2            16005
#define NC_SERVICE_INFORMIX       16006


#define NC_SERVICE_BBS            20010 /* 论坛账号         */

#define NC_SERVICE_ACCOUNT        30000 /* 白名单           */
#define NC_SERVICE_WHITE          33333 /* 白名单           */
#define NC_SERVICE_BLACK          33334 /* 黑名单           */
#define NC_SERVICE_MUSTLOGIN      33335 /* 必须登录的服务    */

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

#define NC_IM_LOGIN       1     /* 登录  */
#define NC_IM_LOGOUT      2     /* 退出  */
#define NC_IM_SENDMSG     3     /* 发送消息  */
#define NC_IM_SENDFILE    4     /* 发送文件  */
#define NC_IM_AUDIT       7     /* 音频      */
#define NC_IM_VIDEO       8     /* 视频      */
#define NC_IM_RECVMSG     6     /* 接收消息  */
#define NC_IM_RECVFILE    5     /* 接收文件  */


/* nMark 的含义  */
#define NC_FTP_PORTINFO   141
#define NC_FTP_PASVINFO   142

#define NC_FTP_PORTMARK   143   /* 数据通道IP连接  */
#define NC_FTP_PASVMARK   144   /* 数据通道IP连接  */
#define NC_FTP_COMMMARK   145   /* 命令通道TCP连接  */

/* cFun 的含义     */
#define NC_FTP_COMM       146     /* 命令      */ 
#define NC_FTP_DATA       147     /* 一般数据  */
#define NC_FTP_PORTDOWN   148     /* 下载文件  */
#define NC_FTP_PORTUP     149     /* 上传文件  */
#define NC_FTP_PASVDOWN   150     /* 下载文件  */
#define NC_FTP_PASVUP     151     /* 上传文件  */

#define NC_FTP_SAVEUP     1401   /* 保存上传  */
#define NC_FTP_SAVEDOWN   1402   /* 保存下载  */
#define NC_FTP_SAVEALL    1400   /* 保存全部  */



/* 接入方式    */
#define NC_ACCESS_BRIDGE  0      // 网桥
#define NC_ACCESS_GATEWAY 1      // 网关
#define NC_ACCESS_BYPASS  2      // 旁路
#define NC_ACCESS_NSA     3      // 旁路

/* 字符集  */
#define NC_CCODE_GB       0     /* gbk bg2312 */
#define NC_CCODE_BIG5     1     /* big5       */
#define NC_CCODE_UTF8     2     /* utf-8      */
#define NC_CCODE_JP       3     /* iso-2022-jp */
#define NC_CCODE_AUTO     0xff  /* 自动         */

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
#define NC_DEBUG_MAILAUDIT    12     /* 邮件审计  */
#define NC_DEBUG_MAILNON      13     /* 邮件监控  */
#define NC_DEBUG_TIMECTL      14     /* 时间控制  */
#define NC_DEBUG_MSGINFO      15     /* 显示数据包   */
#define NC_DEBUG_CHECKNETIP   16     /* 检查网络情况 */
#define NC_DEBUG_THIRDAUTH    17     /* 第三方验证   */
#define NC_DEBUG_SAVEIPBUF    18     /* 定时保存缓冲区中的IP信息  */

/* 有关错误级别       */
#define NC_LOG_SUCESS          PAS_LOG_SUCESS      /* 一般日志         */
#define NC_LOG_ERROR           PAS_LOG_ERROR       /* 严重错误         */
#define NC_LOG_WARN            PAS_LOG_WARN       /* 警告信息         */
#define NC_LOG_EVENT           PAS_LOG_EVENT1     /* 重要事件         */
#define NC_LOG_DEBUG           PAS_LOG_DEBUG      /* 调试信息         */

#define NC_DEBUG_UPDATE       119     /* 在线升级的调试信息    */
#define NC_DEBUG_MAILCTL      120     /* 邮件监控              */

/* 禁止原因代码    */
#define NC_DENY_NONE           0    /* 直接禁止    */
#define NC_DENY_NORMAL         1    /* 一般禁用    */
#define NC_DENY_WEBPOL         2    /* 网址库策略  */
#define NC_DENY_SERVICE        3    /* 服务内容    */
#define NC_DENY_SERVICEPOL     4    /* 服务策略    */
#define NC_DENY_WEBURL         5    /* 网址禁用    */

#define NC_EXCEPT_FORBIT        1    /* 一般禁止   */
#define NC_EXCEPT_IPPOL         2    /* IP地址     */
#define NC_EXCEPT_WEBPOL        3    /* 网址库策略 */
#define NC_EXCEPT_IPNET         4    /* IP禁止上网 */
#define NC_EXCEPT_IPSERVICE     5    /* IP服务限制  */
#define NC_EXCEPT_IPPORT        6    /* 禁止访问IP地址和端口  */
#define NC_EXCEPT_WEBURL        7    /* 禁止访问指定的URL */

/*针对cStatus而言, 0 正常,未知
                   1 正常允许访问
                  >5 禁止访问
 */
#define NC_USER_NOUSE           0        /* Nouse                  */
#define NC_USER_NORMAL          1        /* 监控全部               */
#define NC_USER_NOMON           2        /* 用户免监控             */
#define NC_USER_MONIP           3        /* 监控IP地址             */
#define NC_USER_MONHTTP         4        /* 监控Http               */

#define NC_USER_FORBIDCTL       5        /* 访问控制被禁止         */
#define NC_USER_FORBIDLIMIT     6        /* 用户访问超出限制       */
#define NC_USER_FORBIDNOUSER    7        /* 用户不存在             */
#define NC_USER_FORBIDMACERR    8        /* MAC地址不一致          */
#define NC_USER_USRFORBID       9        /* 用户禁用               */
#define NC_USER_FORBIDAUTH      10       /* 用户验证               */
#define NC_USER_FORBIDNAMEERR   11       /* 计算机名不一致         */
#define NC_USER_FORBIDIPERR     12       /* MAC地址不一致          */

/* 阻断级别  */
#define NC_FORBID_USER       1        /* 用户禁止访问互联网  */
#define NC_FORBID_SERVICE    2        /* 用户禁止访问某个服务 */
#define NC_FORBID_IP         3        /* 禁止访问某个IP地址   */
#define NC_FORBID_IPPORT     4        /* 禁止访问某个IP和PORT */
#define NC_FORBID_IPCONTRACT 5        /* 阻断某个连接         */

/* 命名方式    */
#define NC_DEF_IPADDRESS    1        /* 根据IP地址命名      */
#define NC_DEF_COMPUTER     2        /* 根据计算机名字命名  */
#define NC_DEF_MACADDRESS   3        /* Mac地址命名         */
#define NC_DEF_VLANID       4        /* 根据VlanID命名      */
#define NC_DEF_RAND         5        /* 随机名字            */
#define NC_DEF_PORTNAME     6        /* 端口名称            */

/* 邮件拦截     */
#define NC_MCTL_QMAIL       1        /* Qmail 转发          */
#define NC_MCTL_SMTPPROXY   2        /* Smtp Proxy          */
#define NC_MCTL_POP3PROXY   3        /* Pop3 Proxy          */

#define NC_MCTL_NONE       0     
#define NC_MCTL_SEND       1         /* 直接发送            */
#define NC_MCTL_REJECT     2         /* 拒绝发送            */
#define NC_MCTL_FORWARD    3         /* 转发到指定邮箱      */
#define NC_MCTL_WAIT       4         /* 等待审计            */

#define NC_NAT_NATIP         1        /* Ip地址映射   */
#define NC_NAT_NATPORT       2        /* 端口重定向   */
#define NC_NAT_NATSNAT       3        /* 源地址伪装   */
#define NC_NAT_NATNAP        4        /* 源地址伪装   */

#define NC_NAT_ROUTE         8        /* 路由配置     */

#define NC_DBCHK_EXTENDED    1
#define NC_DBCHK_CHANGED     2
#define NC_DBCHK_FAST        3

#define NC_IPCONTROL_ID       918123123L
#define NC_TABLES_REMOTEFILE       6 

#define NC_SIGNAL_MAX        8        /* 最大信号量          */
#define NC_SIGNAL_RESETPKG   0        /* 重新初始化数据包    */
#define NC_SIGNAL_REFRESH    1        /* 重新刷新            */
#define NC_SIGNAL_RELOAD     2        /* 重新装载            */
#define NC_SIGNAL_MYSTAT     3        /* 我的状态            */
#define NC_SIGNAL_RELOADIPT  4        /* 重新装载            */
#define NC_SIGNAL_RELOADCONT 5        /* 重新装载            */
#define NC_SIGNAL_RELOADPKG  6        /* 重新装载            */

#define nc_copt_default    '^'        /* 缺省的匹配方式      */

/* 以下宏用于检查表的合法性       */
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



/* 当名字不存在的时候,用什么来代替  */
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
    char                *pMatchKey;      // 匹配关键字
    unsigned short      iCount;          // 匹配次数
    unsigned char       cType;           // 匹配类别 参见上面的宏定义
    unsigned char       cMatch;          // 匹配方式 保留
    struct ncMatchCont_s *next;
} ncMatchCont;

 
typedef struct ncIpAddress_s {
    unsigned long    lIp;
    unsigned short   nPort;
    unsigned char    cFlags;
    unsigned char    cRev;
} ncIpAddress;


#define NC_CAP_VTUNE         2


/* 用户的一些统计信息, 数据包平均每分钟统计一次  */
typedef struct ncUserCountInfo_s {
    unsigned long       lSTime;        /* 保存时间, 上次统计时间   */
    unsigned long       lCTime;        /* 经历时间, 本次统计时间   */
    unsigned long long  lSBytes[2];    /* 字节数,上一次字节数     */
    unsigned long long  lSPkg[2];      /* 数据包数量,上一次数据包 */
    unsigned long lSIp;                /* 新增IP连接数量,上一次IP地址  */
    unsigned long lSMail[2];           /* 邮件收发数量,上一次邮件数    */
    unsigned long long  lCBytes[2];    /* 当前字节数       */
    unsigned long long  lCPkg[2];      /* 当前数据包数量   */
    unsigned long lCIp;                /* 当前Ip连接数     */
    unsigned long lCMail[2];           /* 当前收发邮件数量 */
} ncUserCountInfo;



/* 用户信息  */
typedef struct ncUserAuthInfo_s {
    unsigned char username[32];     // 用户名
    unsigned char mac[6];           // Mac地址
    unsigned char dispname[32];     // 显示名
    unsigned char groupname[32];    // 组名
    unsigned char compname[32];     // 计算机名
    unsigned char password[32];
    unsigned char position[32];     // 上网位置，房间号
    unsigned char idfytype[4];      // 证件类别
    unsigned char idfyno[20];       // 证件号码
    unsigned long vlan;            // VlanId
    unsigned char sex;              // 性别 1-男 2-女 0-未知
    unsigned char flags;            // 数据来源
    unsigned char cRev[2];
} ncUserAuthInfo;


/* 综合策略 和表 nclimsumm对应  
   最后修改时间 2009/1/12
   

 */
typedef struct ncLimSummer_s {
    unsigned char ipaccess;
    unsigned char httpaccess;
    unsigned char postsend;
    unsigned char httpbyip;

    unsigned char limitunit;         /* 限制单位  1--每日  2--每周  3--每月 4--全部  */
    unsigned char overctl;           /* 超出限制后的处理方式    
                                     0- 使用服务策略    overid -- 策略ID          overnum -- 序号
                                     1- 使用网址库策略  overid -- 网址库策略ID    overnum -- 序号 
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
/*  以下为 2006/02/05 增加  */
    unsigned long   warntype;          /* 告警类别 0--不告警  1--告警    */
    unsigned long   pkgmax;            /* 每分钟数据包的数量             */
    unsigned long   flowmax;           /* 每分钟的流量                   */
    unsigned long   ipmax;              /* IP连接数                       */
    unsigned long   mailmax;            /* 邮件收发数量                   */

    unsigned char   pkgflags;           /* 方向 0--全部 1--发送  2--接收   */
    unsigned char   flowflags;          /* 方向 0--全部 1--发送  2--接收   */
    unsigned char   mailflags;          /* 0--全部  1--发送   2--接收      */
    unsigned char   accflags;           /* 超出后控制 0--不控制 1--禁用    */

    unsigned long   qospolid;           /* 带宽管理策略                    */
    unsigned long   mailctlid;          /* 邮件拦截策略                    */

    unsigned char   http80chk;          /* 检查80端口的http协议 0 不检查  1 检查  9 未设置 */
    unsigned char   r1;                 /* 保留   */
    unsigned char   r2;
    unsigned char   r3;
}  ncLimSummer;

typedef struct ncLimPolInfo_s {        /* 访问控制策略信息  */
    unsigned long lId;                 /* 策略ID            */
    char           caName[16];         /* 策略名称          */
    ncLimSummer    sSumm;              /* 综合参数          */
    unsigned short nLimIp;             /* Ip控制起始序号    */
    unsigned short nLimWeb;            /* Web控制起始序号   */
} ncLimPolInfo;

   

/*   组的管理  从小到大排列 
     2013/01/12 更新
 */
typedef struct ncLimGroup_s {    /* 组信息  */
    unsigned long  lId;
    char           groupname[32];
    ncLimSummer    sSumm;
    unsigned short nLimIp;
    unsigned short nLimWeb;
    ncLimPolInfo   *psMypol;
    unsigned  long long thisflow;     /* 当前流量                  */
    unsigned  long thistime;          /* 当前时间                  */
    unsigned  long long flowlimit;    /* 流量限制                  */
    unsigned  long timelimit;         /* 时间限制                  */
    unsigned  long overid;            /* 超出后使用的服务策略      */
    unsigned  long resettime;         /* 数据重置时间              */
    unsigned  char limitunit;         /* 限制单位  9--不限 1--每日  2--每周  3--每月 4--全部  */
    unsigned  char cmod;              /* 0--正常  1--修改                  */
    short     overnum;                /* 服务策略的序号, -1 表示禁用       */
    unsigned  long long lBytes[2];
    unsigned  long lDid;              /* 对应的单位ID,用于管理中心，1个场所对应多个单位  */
    unsigned  long lNum;              /* 对应于 ncGroupDept 中的序号                     */
    unsigned  long lPid;              /* 上一级部门               New                    */
    struct    ncLimGroup_s   *preGroup;  /* 上一级部门指针           New                    */
} ncLimGroup;


typedef struct ncUserCont_s {        /* 用户信息       */
    unsigned long  userid;           /* 0--用户不存在  */
    char           username[32];     /* 用户名         */
    char           dispname[32];     /* 显示名         */
    char           compname[32];     /* 计算机名       */
    char           position[32];     /* 位置           */
    char           password[24];     /* 口令           */
    char           cardid[20];       /* 身份证号码     */
    char           idtype[4];        /* 证件类别       */
    unsigned long  groupid;          /* 组ID           */
    unsigned long  ip;               /* IP地址         */
    unsigned long  thisip;           /* 当前客户端IP地址 */    
    unsigned long  thistime;         /* 当前所用时间   */
    unsigned long  resettime;        /* 初始化时间     */
    long     long  thisflow;         /* 当前流量       */
    unsigned long  lasttime;         /* 最后修改时间   */
    ncLimSummer    sSumm;            /* 总体控制信息   */
    ncLimGroup     *psGroup;
    ncLimPolInfo   *psMypol;
    unsigned short nLimWeb;          /* 网站限制访问, 序号从1开始   */
    unsigned short nLimIp;           /* IP限制访问     */
    double         fee;              /* 账户余额       */
    double         thisfee;          /* 累计费用       */

    unsigned char  mac[6];
    unsigned char  useflags;         /* 用户状态  1 禁用    9 免监控       */
    unsigned char  modflags;         /* 修改标识  0--没改  1--新增   2--修改   9--删除*/

    unsigned char  userbase;         /* 用户管理方式                                  */
    unsigned char  cStatus;          /*  1--正常  2--免监控   9-禁用  */
    unsigned char  cProxy;           /*  代理  1,3 Http代理    2,3 Socks代理            */
    unsigned char  cCaseCtl;         /* 布控告警 0--正常  1-布控告警                    */

    unsigned char  cNameStatus;      /* 名字的性质  0--正常  1--临时, 新增用户由于计算机名不准确，可用ip地址代替,当出现了正确的名字后自动更正   */
    unsigned char  cInDb;            /* 0-- 1-库中有  0-库中无,主要用于内存和数据库同步  9-记录已被删除          */
    unsigned char  cWins;            /* 0-- 正常      1-需要重新检查mac、name对照表                */
    unsigned char  cBusy;            /* 0-- 正常      1-用户信息正在修改   */

    unsigned char  cValid;           /* 0-- 无效      1-有效               */
    unsigned char  cForbFor;         /* 失效原因 0--永久  1--费用问题  　　*/
    unsigned char  cSex;             /* 性别 1--男  2--女     */
    unsigned char  cFrom;            /* 客户信息来源,尚未启用                  */
    unsigned long  nVlanId;          /* Vlan Id, 对基于VlanId管理有效      */
    unsigned long  expiretime;       /* 失效时间                           */
    unsigned long  lAddTime;         // 加入时间
    unsigned long  lNum;             /* 在记录中的序号                     */
} ncUserCont;

#define NC_FORBIDFOR_FEE           1         /* 计费控制   */


#define NC_IPBASE_PRE    999
#define NC_MACBASE_PRE   111
#define NC_IDBASE_PRE    222

/* 通过Hash表来建立索引  */
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
    unsigned long lHashPos;          /* Hash表的偏移量    */
    unsigned long lContPos;          /* 用户内容的偏移量  */
    unsigned long lLasttime;         /* 最后修改时间      */
    int           iModFlags;         /* 修改标识 0--未改过  1--已作修改  */
    unsigned long lProxyCheckTime;   /* proxy最后检查时间      */
    unsigned char cProxyCheck;       /* 0--正常  1--立即检查   */
    unsigned char cLock;
    unsigned char cLockAdd;
    unsigned char cRev;
    ncUserCont    *psUser;
    char          *psHash;
} ncUserContHead;

#define NC_LOGIN_IP         2      /* 通过IP地址管理上网    */
#define NC_LOGIN_MAC        3      /* 通过MAC地址管理上网   */
#define NC_LOGIN_IC         4
#define NC_LOGIN_AUTH       4      /* 通过本地验证管理上网  */
#define NC_LOGIN_THIRD      4      /* 通过本地验证管理上网  */
#define NC_LOGIN_CLIENT     5      /* 通过本地验证管理上网  */
#define NC_LOGIN_BILLAUTH   4      /* 计费验证              */

#define NC_LOGIN_REMOTE1    100    /* 远程验证  cRev 的值   */
typedef struct ncUserInfo_s {      /* 用户信息       */
    unsigned long  lIp;            /* 0 表示没有用户 */
    unsigned long  lId;            /* 用户Id  0--没用                           */
    unsigned long  lGid;           /* 组Id   groupid=0 未知用户  1=服务器        */
    unsigned char  cUseflags;      /* 使用状态0--正常 1--禁用  9--免监控         */
    unsigned char  cStatus;        /*         99--退出                          */
    unsigned char  mac[6];         /* MAC地址                                   */
    unsigned long  lStartTime;     /* 开始连接时间      */
    unsigned long  lLastTime;      /* 最后访问时间      */
    unsigned long  lLastCountTime; /* 最后一次计算时间  */
    unsigned long  lConnTime;      /* 本次连接时间,从连接时间起算   */
    unsigned long  lSaveTime;      /* 上次保存的连接时间  */
    long8          lSaveBytes[2];  /* 上次保存的流量 */
    long8          lBytes[2];      /* 0-总的上行流量 1--下行流量 从连接时间开始计算     */
    long8          lSumflow;       /* 从开始计时到当前流量总数                      */
    unsigned long  lSumtime;       /* 从开始计时到当前时间总数     */
    unsigned long  lLastSaveTime;  /* 上次操作数据库时间   */
    unsigned long  lPkg[2];        /* 数据包数量 0:dat 1:syn  根据该值来判断TCP连接是否正常 */
    ncUserCont     *psUser;        /* 针对用户内容         */
    unsigned long  lCltLastTime;   /* 和客户端最后交互信息     */
    unsigned long  lCltId;         /* 客户端ID                 */
    ncUserCountInfo sCount;        /* 统计信息 2006/02/06 增加  */
    unsigned char  cLogin;         /* 登录方式    2 基于IP地址  3 基于MAC地址 4 本机验证 5 第三方验证  6 客户端方式 */
    unsigned char  cMacCheck;      /* 0--NoCheck 1--Check  */
    unsigned char  cProxy;         /* > 0 proxy            */
    unsigned char  cRev;           /* 在 httpproxy验证的时候，该值若为 1,不允许登录    在第一次访问弹出时，若为0-表示没弹过 */
    unsigned long  lSid;           /* 上网ID                                          */
    unsigned char  position[32];   /* 当前位置                                        */
    char           chkkey[16];     /* Key值                                           */
    unsigned long   lQos;           /* 当前用户带宽                                   */
    unsigned char   R1;             /* 暂时禁止用户上网 0-正常 1-禁用    */
    unsigned char   R2;             /* 带宽管理相关                      */
    unsigned short  R4;            /* P2P 标识  1-迅雷         */
    unsigned long   lRtime;         /* 最后禁止上网时间               */
    unsigned long   lStop;          /* 规则作用的最后时间             */
    unsigned long   lServiceTime;   /* 服务识别时间                   */
    unsigned char   cSaveDb;        /* 0--未保存  1--已保存到数据库   */
    unsigned char   cRev2[3];       /* 保留                           */
} ncUserInfo;

/* 备份的在线信息  */
typedef struct ncUserBack_s {      /* 用户信息       */
    unsigned long  lIp;            /* 0 表示没有用户 */
    unsigned long  lId;            /* 用户Id  0--没用                          */
    unsigned long  lGid;           /* 组Id   groupid=0 未知用户  1=服务器      */
    unsigned char  cUseflags;      /* 使用状态0--正常 1--禁用  9--免监控            */
    unsigned char  cStatus;        /*                                               */
    unsigned char  mac[6];         /* MAC地址           */
    unsigned long  lStartTime;     /* 开始连接时间      */
    unsigned long  lLastTime;      /* 最后访问时间      */
    unsigned long  lLastCountTime; /* 最后一次计算时间  */
    unsigned long  lConnTime;      /* 本次连接时间,从连接时间起算   */
    long8          lBytes[2];      /* 0-总的上行流量 1--下行流量 从连接时间开始计算     */
    unsigned long  lPkg[2];        /* 数据包数量 0:dat 1:syn  根据该值来判断TCP连接是否正常 */
    unsigned long  lCltLastTime;   /* 和客户端最后交互信息     */
    unsigned long  lCltId;         /* 客户端ID                 */
    ncUserCountInfo sCount;        /* 统计信息 2006/02/06 增加  */
    unsigned char  cLogin;         /* 登录方式    2 基于IP地址  3 基于MAC地址 4 本机验证 5 第三方验证  6 客户端方式 */
    unsigned char  cMacCheck;      /* 0--NoCheck 1--Check  */
    unsigned char  cProxy;         /* > 0 proxy            */
    unsigned char  cRev;           /* 在 httpproxy验证的时候，该值若为 1,不允许登录   */
    unsigned long  lSid;           /* 上网ID                                          */
    unsigned char  position[32];   /* 当前位置                                        */
    struct ncUserBack_s   *next;
} ncUserBack;

/* 系统锁信息         */
#define NC_LOCK_SYSREFRESH       0       /* 系统刷新     */
#define NC_LOCK_UPDATEURL        1       /* 在线升级URL  */
#define NC_LOCK_AUTOCLEAN        2       /* 自动整理     */
#define NC_LOCK_SMTPPROXY        3      
#define NC_LOCK_GETUSER          4       /* 取用户信息   */
#define NC_LOCK_OUTLOG           5 

 /*  锁记录            */
typedef struct ncLockInfo_s {
    int  iLock;                  /*  1--锁定    0--锁定  */
    unsigned long lTimeout;      /*  超时时间            */
    unsigned long lLasttime;     /*  上锁时间            */
    int  iPid;                   /*  进程号              */
    char     caDesc[16];         /*  描述                */
    char     caMsg[128];         /*  当前信息            */
} ncLockInfo;    

/* 
    # --- 开头
    ^ --- 包含
    = --- 等于
    ! --- 不
    $ --- 结束    

 */
typedef struct ncUtlStrStr_hs {
    unsigned char        cMark;         /* 带结构     */
    unsigned char        cSum;          /* 总个数     */
    unsigned char        nLen;          /* 长度       */
    unsigned char        cOpt;          /* 0--正常  1--不      */
} ncUtlStrStr_h;

typedef struct ncUtlStrStr_cs {
    unsigned char        cOpt;          /*  0--正常  1--不  */
    unsigned char        cFlags;        /*  操作符  =#$^    */
    unsigned short       nLen;          /* 长度             */
    char     caWord[256];                /* 字符串内容       */
} ncUtlStrStr_c;


typedef struct ncThreadInfo_s {
    int      iSumThread;                 /* 线程总数  */
    unsigned long lStartTime;            /* 进程起始时间  */
    unsigned long lLastTime;             /* 最后操作时间  */
    int      iPid;                       /* 进程ID        */
} ncThreadInfo;

typedef struct ncProxyProcessInfo_s {
    int iMaxThread;            /* 每个进程最大线程数  */
    int iMaxProcess;           /* 最大进程数          */
    int iMinProcess;           /* 最小进程数          */
    int iSumProcess;           /* 总计进程数          */
    ncThreadInfo *psThread;
} ncProxyProcessInfo;


typedef struct ncIcUserinfo_s {
    unsigned long  lSip;      /* 源IP地址   */
    unsigned long  lUserid;   /* 用户ID     */
    char           username[32];
    unsigned long  lStartTime;
    unsigned long  lLastTime;
} ncIcUser;


typedef struct ncMailIpList_s {
    unsigned long  lSip;      /* 源IP地址   */
    unsigned short nSport;    /* 源端口     */
    unsigned short nDport;    /* 目标端口   */
    unsigned long  lDip;      /* 目标IP地址 */
    unsigned long  lLastTime;
    int            iFlags;    /* 0--发送  1--接收  */
} ncMailIpList;


/* Proxy的Ip地址和端口    */
typedef struct ncProxyIpList_s {
    unsigned long  lIp;       /* IP地址      */
    unsigned short nPort;     /* 端口        */
    unsigned short nRev;      /* 保留        */
} ncProxyIpList;


typedef struct ncIpPortHead_s {
    ncProxyIpList  *psList;
    unsigned int   iMax;          /* 分配的地址最大总数  */
    unsigned int   iSum;
} ncIpPortHead;

    
typedef struct pasSearchHead_s {
    char caFname[32];        /* 字段名               */
    char caDispname[32];     /* 显示名               */
    int  iSize;
    int  iMaxLen;
    int  iType;              /* 输入类型  1-文本  2-选择框  */
    char *pValue;
} pasSearchHead;

/* 内存中的用户信息  */
typedef struct ncUserBuf_s {       /* 用户信息                */
    long     lUserid;              /* 用户Id  0--没用         */
    char     caUsername[32];       /* 姓名                    */
    char     caDispname[32];       /* 显示名                  */
    long     lGroupid;             /* 部门ID                  */
    char     caGroupname[32];      /* 部门名称                */
} ncUserBuf;

/* 用户信息的缓存
         本结构主要用于用户信息的Case, 在系统中存放在一个Hash表中
 */
typedef struct ncUserBufIp_s {
	unsigned long lSip;           /* IP地址  */
	int      iFlags;              /* 状态 0--未用       1--已在线  2--用户超时  3--用户不存在
	                                      4--MAC不一致  7--禁用    9--免监控
	                               */
	unsigned long lLastTime;      /* 最后访问时间  */
} ncUserBufIp;


/* 第三方认证的用户信息  */
typedef struct ncThirdAuth_s {
    char          caUsername[32];    /* 用户名                */
    int           iIndex;            /* 在用户信息中的索引号  */
    unsigned long lIp;               /* IP地址 0 --表示没有内容 */
    unsigned long lLastTime;         /* 最后更新时间          */
} ncThirdAuth;

typedef struct ncNtAuth_s {
    unsigned long lIp;               /* IP地址                    */
    unsigned long lSip;              /* 比较IP地址                */
    char          caName[32];
    char          caDomain[32];      /* 域名                       */
    unsigned long lLastTime;         /* 最后操作时间               */
    char          caKey[16];
    char          caMd5[32];         /* 向客户端发放的Key,16位     */
    int           ccode;
} ncNtAuth;

typedef struct ncIcAuth_s {
    unsigned long lIp;               /* IP地址             */
    char          caName[32];
    unsigned long lLastTime;         /* 最后操作时间       */
} ncIcAuth;



/* 用户同步信息   */
typedef struct ncNtUser_s {        /* NT用户信息     */
    char          caName[32];      /* 姓名           */
    unsigned long lIp;             /* IP地址         */
    int           iStatus;        /* 1--登陆  0--退出  */
} ncNtUser;

/* 域用户信息    */
typedef struct ncAdUser_s {
    unsigned long lIp;
    char          caName[32];
    char          mac[6];
    unsigned long lLastTime;
}ncAdUser;


/* 表的结构信息  */
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
    char caSeq[4];       /* 序号               */
    char caField[32];
} ncDbIndex;



#define NC_NETWORK_NOMAL          0    /* 正常访问          */
#define NC_NETWORK_NAMEBASE       1    /* 基于名字          */
#define NC_NETWORK_INTERNET       99   /* Internet网        */
#define NC_NETWORK_IPBASE         2    /* 基于IP地址        */
#define NC_NETWORK_MACBASE        3    /* 基于MAC地址       */
#define NC_NETWORK_AUTH           4    /* 本机验证          */
#define NC_NETWORK_NTAUTH         5    /* 第三方验证        */
#define NC_NETWORK_NOMON          6    /* 免监控            */
#define NC_NETWORK_FORBID         7    /* 禁用              */
#define NC_NETWORK_SERVER         8    /* 服务器            */
#define NC_NETWORK_CLIENT         9    /* 基于客户端ID      */
#define NC_NETWORK_OUTSIDE        10   /* 外部              */
#define NC_NETWORK_PORT           11   /* 基于端口          */
#define NC_NETWORK_VLANID         12   /* 基于VLan ID 管理  */
#define NC_NETWORK_SPECLOGIN      13   /* 特定登录          */
#define NC_NETWORK_ADDUSER        88   // 自动添加用户


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
#define NC_NAMED_WEBNAMEPOSUSER   120             // 帐号位置用户




#define NC_NETWORK_L2             2
#define NC_NETWORK_L3             3




typedef struct ncIpNetwork_s {      /* 内部IP地址    */
    unsigned long   lIpLower;       /* 起始IP地址    */
    unsigned long   lIpUpper;       /* 终止IP地址    */
    unsigned long   lAuthIp;        /* 验证服务器IP地址  */
    unsigned short  nAuthPort;      /* 验证服务器端口号  */
    unsigned short  iFlags;         /* 见宏定义      */
    unsigned long   lId;             /* 记录ID        */
} ncIpNetwork;

/* Ip的头部信息  */
typedef struct ncIpHead_s {
	unsigned long  lSip;       /* 源IP地址     */
	unsigned long  lDip;       /* 目标IP地址   */
	unsigned short nDport;     /* 目标端口     */
	unsigned short nSport;     /* 源端口       */
	char     caSmac[6];        /* 源MAC地址    */
	char     caDmac[6];        /* 目标MAC地址  */
	unsigned char   cTran;            /* 传送方向 0--正常 1--反向    */
	unsigned char   cProt;            /* 协议  6 UDP      17 TCP     */
	unsigned char   cIpif:4;          /* 进入设备序号                */
    unsigned char   cEpif:4;	      /* 出去设备的设备号 nsa中使用  */
    unsigned char   cDev;             /* 数据包进入的端口号          */
	unsigned long   nVid;             /* Vlan Id  */
	unsigned long   lSeq;
	unsigned long   lAckSeq;
} ncIpHead;


typedef struct ncMsnPkg_s {
	unsigned long  lSip;       /* 源IP地址     */
	unsigned long  lDip;       /* 目标IP地址   */
	unsigned short nDport;     /* 目标端口     */
	unsigned short nSport;     /* 源端口       */
	unsigned long  lLen;       /* 数据包长度   */
} ncMsnPkg;



/* 限制访问  */
typedef struct ncIpLimited_s {       /* 限制访问的Ip地址  */
    unsigned long   lUid;              /* 用户Id  0 表示全部  */
    unsigned long   lIpLower;          /* 下限  */
    unsigned long   lIpUpper;          /* 上限  */
    unsigned char   cTranType;         /* 传送类型  0--全部   1--UDP  2--TCP          */
    unsigned char   cTimeType;         /* 时间类型                                    */
    unsigned short  nLowerport;        /* 端口号下限                                  */
    unsigned short  nUpperport;        /* 端口号上限                                  */
    unsigned char   cService;          /* 服务类型                                    */
    unsigned char   cFlags;            /* 限制方式 1--外部网  2--服务  3--IP地址和端口 */
    unsigned long   lStart;            /* 起始时间 HHMMSS                             */
    unsigned long   lEnd;              /* 终止时间 HHMMSS                             */
    unsigned char   cVister;           /* 0 禁止   1 允许                             */
    unsigned char   cReserve[3];       /* 保留  */
} ncIpLimited;


typedef struct ncWebLimited_s {        /* 限制访问的网站      */
    unsigned long   lUid;              /* 用户Id  0 表示全部  */
    char            caUrl[64];         /* 网站地址            */
    unsigned long   lStart;            /* 起始时间 HHMMSS     */
    unsigned long   lEnd;              /* 终止时间 HHMMSS     */
    unsigned char   cTimeType;         /* 时间类型            */
    unsigned char   cVister;           /* 0  禁止  1 允许     */
    unsigned char   cReserve[2];       /* 保留                */
} ncWebLimited;

/* The Value of cType */
#define NC_PROT_TCP      6
#define NC_PROT_UDP     17
/* 此处的源地址为客户端地址  */
typedef struct ncIp_s {
    unsigned long  lSip;             /* 内部IP地址                    */
    unsigned long  lDip;             /* 外部IP地址                    */
    unsigned short nDport;           /* 目标端口号                    */
    unsigned short nSport;           /* 源端口, 缺省情况下为 0, 只有当代理的时候才为非 0  */
    unsigned char  cType;            /* 协议    6--TCP 17 UDP         */
    unsigned char  cMark;            /* 标识,在不同的地方有不同的用处 */    
    unsigned char  cFileMark;        /* 文件保留属性 1-下载文件 2-FTP数据连接    */
    unsigned char  cCode;            /* 保留  */
    unsigned long  lStartTime;       /* 起始时间                     */
    unsigned long  lLastTime;        /* 最后修改时间                 */
    unsigned long  lStop;            /* 访问控制检查时间 hhmmss， 若该值为0表示使用超时时间  */
    unsigned long  lQos;             /* Qos序号                      */
    long8          lBytes[2];        /* 0-上行字节数  1-下行字节数   */
    unsigned char  cStatus;          /* 0--新   1--正常  2--禁用     */
    unsigned char  cUseFlags;        /* 使用标识 0--没内容 1--有内容 2--已处理 3--已保存   */
    unsigned char  caMac[6];         /* 源Mac地址                    */
    ncUserInfo     *psUser;          /* 用户信息，若为空表示用户信息不定    */
    unsigned short lService;         /* 服务                                */
    unsigned char  cFun;             /* 功能呢                              */
    unsigned char  cContinue;        /* 协议是否继续分析 0--不继续  1--继续 */
    unsigned long  lUrl;             /* 访问http                            */
    unsigned short nPreService;      /* 预分析协议                          */
    unsigned short nCount;           /* 服务出现次数                        */
    unsigned char  cProService[8];   /* 协议标识,用于协议的上下文分析       */
    char    *p;                      /* 保留指针  */
} ncIp;



/* TCP连线信息  
      此处的源地址为内网地址
*/
/* Value of cConnect   */
#define NC_TCPSTAT_SYN     			1
#define NC_TCPSTAT_ESTABLISHED    	2
#define NC_TCPSTAT_CLOSED             9
/* Value of cDict      */
#define NC_DICT_SEND           0
#define NC_DICT_RECEIVE        1
typedef struct ncTcp_s {
    unsigned long  lSip;              /* 内部IP地址                   */
    unsigned long  lDip;              /* 外部IP地址                   */
    unsigned short nDport;            /* 目标端口号                   */
    unsigned short nSport;            /* 源端口号                     */
    unsigned long  lBytes;            /* 流量                         */
    unsigned long  lLastTime;         /* 最后修改时间                 */
    unsigned char  cConnect;          /* 连接状态  1 Syn  2 建立  9 Close   */
    unsigned char  cDiction;          /* 方向 0 正向   1 反向               */
    unsigned char  cStatus;           /*      0 未知   1 正常      2 禁用   */
    unsigned char  cProt;             /* 协议   6-TCP  17 UDP               */
    unsigned char  cFlags;            /* 用于具体协议中的标识               */
    unsigned char  cProxy;            /* 0 -- 正常   1 -- 基于Proxy的协议   */
    unsigned char  cIsOld;            /* 0--新包 1--老  */
    unsigned char  cVid;              /* 虚拟通道ID                         */
    ncIp           *psIp;             /* 对应的IP信息,NULL 表示该连接不可用 */
    unsigned long  lStop;             /* 规则作用的终止时间                 */
    unsigned long  lSeq;              /* 上行序列号                          */
    unsigned long  lAck_seq;          /* 下行序列号                          */
    unsigned long  lSid;              /* Session ID                          */
    unsigned long  lUrl;              /* Url序号                             */
    unsigned long  lQos;              /* Qos序号                             */
    unsigned long  lSetTime;          /* 设置时间                            */
    unsigned long  lBase;             /* 备用在快照中为基准位置              */
    char *p;                          /* 保留指针                            */
} ncTcp;


#define NC_TCPFLAGS_QQACTIVE      166               // 该标志表示需要发送应答数据包

/*    2005/10/17 修改
      例外的IP连接, Key=12    
         cStatus --- 禁用原因
         ID      --- 规则ID
 */
typedef struct ncIpExcept_s {
    unsigned long  lSip;             /* 内部IP地址                         */
    unsigned long  lDip;             /* 外部IP地址                         */
    unsigned short nDport;           /* 目标端口号                         */
    unsigned char  cStatus;          /* 0-允许  1-阻断告警  2--允许告警  3--阻断              */
    unsigned char  cType;            /* 类别    NC_DENY_NONE           0     直接禁止    
                                                NC_DENY_NORMAL         1     一般禁用 
                                                NC_DENY_WEBPOL         2     网址库策略
                                                NC_DENY_SERVICE        3     服务内容 
                                                NC_DENY_SERVICEPOL     4     服务策略
                                                NC_DENY_WEBURL         5     网址禁用
                                      */
    unsigned long  lRid;             /* 规则ID                             */
    unsigned long  lUserid;          /* 用户ID, Userid为0表示非受控数据包  */
    unsigned long  lStartTime;       /* 最后修改时间                       */
    unsigned long  lLastTime;        /* 最后修改时间                       */
    unsigned long  lSaveTime;        /* 数据库更新时间                     */
    unsigned long  lCount;           /* 数据包的数量                       */
    unsigned long  lService;         /* 网络服务ID                         */
    unsigned long  lwarnid;          /* 告警ID                             */
    unsigned char  cProt;            /* 协议    6--TCP 17 UDP              */
    unsigned char  cFlags;           /* 禁用状态 1-阻断告警 2-允许告警 3-阻断  */
    unsigned char  cRev[2];          /* 保留                               */
    unsigned long  lSid;             /* 对应于本地ID                       */
    unsigned long  lSsid;            /* 对应于管理中心的ID                 */
    unsigned long  lUsid;            /* 登陆ID                             */
    char           caMsg[96];        /* 补充信息                           */
} ncIpExcept;


typedef struct ncHttpBuffer_s {
    unsigned long  lSip;             /* 源IP地址     */
    unsigned long  lStartTime;       /* 起始时间     */
    unsigned long  lLastTime;        /* 终止时间     */
    unsigned long  lBytes;           /* 流量         */
    unsigned long  lDip;             /* 目标地址     */
    unsigned short nDport;           /* 目标端口     */
    unsigned char  caMac[6];         /* 源Mac地址    */
    unsigned char  cUseFlags;        /* 标识，0--无内容 1--有内容 2--可以处理 3--已保存  */
    unsigned char  cMethod;          /* Http方法     */
    unsigned char  cContent;         /* 保存内容标志 0-无  1-开始  2-有内容   */
    unsigned char  cZip;             /* 0-无压缩  1-有压缩  */
    unsigned long  userid;
    unsigned long  groupid;
    unsigned long  lUsid;            /* 登陆ID                                  */
    unsigned long  lFid;             /* 当保存内容的时候，该值为 > 0,文件ID     */
    char     caHost[64];
    char     caUrl[128];
    char     caTitle[128];             /* 标题 2005/01/05 加入      */
} ncHttpBuffer;


// Http Hash Buffer
typedef struct ncHttpHashBuffer_s {
    unsigned long  lSip;             /* 源IP地址     */
    char           caHost[64];       /* Host         */
    char           caUrl[128];       /* Url          */    
    unsigned long  lStartTime;       /* 起始时间     */
    unsigned long  lLastTime;        /* 终止时间     */
    unsigned long  lBytes;           /* 流量         */
    unsigned long  lDip;             /* 目标地址     */
    unsigned short nDport;           /* 目标端口     */
    unsigned char  caMac[6];         /* 源Mac地址    */
    unsigned long  userid;
    unsigned long  groupid;
    unsigned long  lUsid;            /* 登陆ID       */
    unsigned long  lCount;
    char           caTitle[127];     /* 标题         */
    char           cFlags;           /* Mothed       */
} ncHttpHashBuffer;




// 新的HttpBuf结构

typedef struct ncHttpBuf_s {
    unsigned long  lLastTime;        /* 终止时间     */
    unsigned char  cUseFlags;        /* 标识，0--无内容 1--有内容 2--可以处理 3--已保存  */
    unsigned char  cMethod;          /* Http方法     */
    unsigned char  cContent;         /* 保存内容标志 0-无  1-开始  2-有内容   */
    unsigned char  cZip;             /* 0-无压缩  1-有压缩                    */
    ncHttpHashBuffer      *psHash;   /* 对应于 ncHttpHashBuffer的指针         */
    char     caHost[64];             /* Host                                  */
    char     caUrl[128];             /* Url                                   */
} ncHttpBuf;





typedef struct ncService_s {
    char             caName[32];      /* 服务名称  */
    unsigned long    lServices;       /* 服务类型  */
} ncService;




/* 邮件监控过滤规则  */
typedef struct ncMailFilter_s {
    int    tid;                  /* 类别ID */
    char   uname[128];
    char   caFrom[128];
    char   caTo[128];
    char   caSub[128];
    char   caCont[128];
    char   caAtt[128];
    int    lSize;
    int    lAsize;
    int    lTonum;
    char   cStype;            /* 0--全部  1--发送  2--接收  */
    char   cUflags;           /* 1--组    2--用户           */
    char   cTzflags;          /* 0--不通知  1--通知         */
    char   cSaveflags;        /* 0--不保存  1--保存         */
    char   caTzmail[64];
} ncMailFilter;


/* 拦截邮件分类规则 */
typedef struct ncMailCtlType_s {
    int    tid;                  /* 类别ID */
    char   uname[128];
    char   caFrom[128];
    char   caTo[128];
    char   caSub[128];
    char   caCont[128];
    char   caAtt[128];
    int    lSize;
    int    lAsize;
    int    lTonum;
    char   cStype;            /* 0--全部  1--发送  2--接收  */
    char   cUflags;           /* 1--组    2--用户           */
} ncMailCtlType;

typedef struct ncUploadFilter_s {
    int    tid;                  /* 类别ID */
    char   uname[128];
    char   caHost[128];
    char   caUrl[128];
    char   caCont[128];
    char   caAtt[128];
    int    lSize;
    int    lAsize;
    char   cUflags;           /* 1--组    2--用户           */
    char   cTzflags;          /* 0--不通知  1--通知         */
    char   cSaveflags;        /* 0--不保存  1--保存         */
    char   cReserve;          /* 保留                       */
    char   caTzmail[64];
} ncUploadFilter;



typedef struct ncPostFileList_s {
    char     pOriginFile[128];   /* 源文件    */
    char     pType[12];          /* 文件类型  */
    char     pOutFile[64];
    unsigned long lSize;
    struct ncPostFileList_s *psNext;
} ncPosFileList;



typedef struct ncPostDataContent_s {
    unsigned long  lBytes;         /* 总字节数  */
    unsigned long  lTime;          /* 时间      */
    char           caName[32];     /* 用户名    */
    char           caHost[128];    /* 主机名    */
    char           caUrl[128];     /* 访问URL   */
    char           *pValue;        /* 数据内容  */
    int            iSum;           /* 附件总数  */
    ncPosFileList  *psFileList;    /* 文件列表  */
} ncPostDataContent;


typedef struct ncPostFList_s {
    char     *pOriginFile;       /* 源文件     */
    char     *pType;             /* 文件类型   */
    char     *pOutFile;          /* 输出文件   */
    unsigned long lSize;
    struct ncPostFileList_s *psNext;
} ncPostFList;

typedef struct ncPostVarList_s {
    char          *pVar;              /* 变量名    */
    char          *pValue;            /* 内容      */
    struct ncPostVarList_s *psNext;
} ncPosVarList;

/* 表单内容(New)  */
typedef struct ncPostDataList_s {
    unsigned long  lBytes;         /* 总字节数  */
    unsigned long  lTime;          /* 时间      */
    unsigned long  lUserid;        /* 用户ID    */
    char           caHost[128];    /* 主机名    */
    char           caUrl[128];     /* 访问URL   */
    int            iSumVar;        /* 变量总数  */
    ncPosVarList   *psVarList;     /* 变量列表  */
    int            iSumFile;       /* 附件总数  */
    ncPosFileList  *psFileList;    /* 文件列表  */
} ncPostDataList;



#define NC_IPPKG_UPFLOW      0
#define NC_IPPKG_DOWNFLOW    1

typedef struct ncIpPkgInfo_s {
    long8    lIpFlow[2];           /*  0 Upload Bytea  1  DownBytes  */
    unsigned long lStartTime;      /*  起始时间                      */
    long8    lSumPkg;              /*  总数据包                      */
    unsigned long lpPkgPerSec;     /*  每秒数据包                    */
    unsigned long lStartCountTime; /*  记数的起始时间                */
    long8    lPkgCount;            /*  数据包记书数                  */
    unsigned long lCount;          /*  访问次数                      */
    unsigned long lStartControl;   /*  时间控制的起始时间            */
    char          caFilename[128];
} ncIpPkgInfo;

typedef struct ncSumControl_s {
    unsigned long  lStartTime;       /* 起动时间                     */
    unsigned long  lLastTime;        /* 最后操作时间                 */
    unsigned long  lTimeOut;         /* 最长超时时间                 */
    unsigned short nLock;            /* 锁  0--正常   1--正在检查    */
    unsigned short nR1;              /*                               */
    unsigned long  lR1[2];           /* 保留                          */
} ncSumControl;

/* 控制信息 从1开始  */
#define NC_CONTROL_NOUSE           0
#define NC_CONTROL_DELETE          1
#define NC_CONTROL_TABDROP         5
#define NC_CONTROL_MODPORT         6
#define NC_CONTROL_DISPERR         7

/* Iptables 的控制台命令           */


struct ncTestRemote_s {     /* 远程文件测试  */
    char  caIp[16];
    char  caHost[64];
    char  caUser[32];
    char  caPass[32];
    char  caMount[64];
    char  caShare[32];
    char  caSdate[12];
    char  caEdate[12];
    char  caOpt[12];       /* 内容       */
};

/* 当为RemoteMount时 cPort的值为具体的命令  */
#define NC_REMOTE_MOUNT         1
#define NC_REMOTE_MOUNTTEST     2
#define NC_REMOTE_MAILBACK      3
#define NC_REMOTE_UNMOUNT       4
#define NC_REMOTE_MAILLOAD      5
#define NC_REMOTE_MAILEXPORT    6    /* 数据导出  */
typedef struct ncControl_s {
    unsigned long  lSip;             /* 起始IP    */
    unsigned long  lDip;
    unsigned short nDport;
    unsigned char  cProt;            /* 协议                         */
    unsigned char  cUse;             /* 见上面的宏                   */
    unsigned long  lLastTime;        /* 起始时间                     */
    unsigned long  lStop;            /* 终止时间                     */
    int            iIndex;
} ncControl;

typedef struct ncIpControlInfo_s {
    unsigned long  lSip;             /* 起始IP                      */
    unsigned long  lDip;
    unsigned short nDport;
    unsigned char  cProt;            /* 协议                        */
    unsigned char  cComm;            /* 命令 0--Add 1--Del 2--Flush */
    unsigned long  lStop;            /* 终止时间 hhmmss             */
    unsigned long  lId;              /* ControlId                   */
} ncIpControlInfo;

typedef struct ncDevList_s {
    char caDev[32];
    char caIp[32];
    char caNetmask[32];
    char caBoard[32];
    struct ncDevList_s *next;
} ncDevList;


#define  NC_PID_COLLECT        1     /* 数据采集     */
#define  NC_PID_SYSLOG         0     /* 系统日志     */
#define  NC_PID_TABLECTL       2     /* IpTable控制  */
#define  NC_PID_NTSYNC         3     /* 域同步       */
#define  NC_PID_ONTIME         4     /* 定时         */
#define  NC_PID_ICAUTH         5     /* 第三方验证   */
#define  NC_PID_CHECKSOFT      6     /* 检查Software */
#define  NC_PID_SMTPPROXY      7     /* Smtp Proxy   */
#define  NC_PID_LISTEN25       8     /* 侦听外网     */
#define  NC_PID_HTTPPROXY      9     /* Http Proxy   */
#define  NC_PID_POP3PROXY     10     /* Pop3 Proxy   */
#define  NC_PID_NTAUTH        11     /* NT域的验证   */
#define  NC_PID_GETPKG        12     /* 数据处理      */
#define  NC_PID_PKGDO         13     /* 数据采集      */
#define  NC_PID_ASYNC         14     /* 异步日志操作  */
#define  NC_PID_MAILMON       15     /* 邮件内容监控  */
#define  NC_PID_POSTMON       16     /* 表单内容监控  */
#define  NC_PID_CHECKMAC      17     /* Mac地址检查   */
#define  NC_PID_SYSMON        18     /* Mac地址检查   */
#define  NC_PID_DBSYNC        19     /* 数据同步进程  */
#define  NC_PID_FILESRV       20     /* 文件服务器   */
#define  NC_PID_DODBLOG       21     /* 处理数据库同步信息  */
#define  NC_PID_UPDATE        22     /* 自动在线升级       */
#define  NC_PID_MAILFILTER    23     /* 邮件监控       */
#define  NC_PID_AUTOBAK       24     /* 自动备份进程  */
#define  NC_PID_AUTOCLEAN     25     /* 自动整理进程  */
#define  NC_PID_VIRUSCHECK    26     /* 检查病毒扫描引擎  */
#define  NC_PID_NWCHECKCOMP5  27     /* 内网检查在线计算机机  */
#define  NC_PID_DBCHECK       28     /* 数据库检查和修复      */  
#define  NC_PID_IMFILE        29     /* 检查MSN文件     */
#define  NC_PID_PROXYCHECK    30     /* 检查Proxy       */
#define  NC_PID_PORTBASE      31     /* 基于端口管理    */
#define  NC_PID_CHECKNETINFO  32     /* 检查网络信息    */
 
#define  NC_RELOAD_ALL            0  /* 全部        */
#define  NC_RELOAD_NETWORK        1  /* 网络设置    */
#define  NC_RELOAD_LIMIT          2  /* 访问限制    */
#define  NC_RELOAD_MAILMONFILTER  3  /* 邮件过滤规则 */
#define  NC_RELOAD_PQCONFIG       4  /* 网络配置参数 */
#define  NC_RELOAD_DEBUG          5  /* Debug参数    */
/* 进程信息  
     保存在共享内存中
 */
typedef struct ncProcessInfo_s {
    int           iPid;                /* 进程ID                                        */
    unsigned long lStartTime;          /* 启动时间                                      */
    unsigned long lTimeOut;            /* 超时时间                                      */
    unsigned long lOntime;             /* 在每天指定时间重新启动                        */
    unsigned short nPrio;              /* 优先级                                        */
    unsigned short nNum;               /* 启动顺序号                                    */
    unsigned char caControl[28];       /* 控制信息                                      */
    char     caName[32];               /* 功能名称                                      */
    int      (*fFunName)(utShmHead *); /* 函数名                                        */
    unsigned long lLastTime;           /* 进程最后操作时间                              */
    unsigned long lStepTime;           /* 最大间隔时间，通常超过该时间表明进程有问题    */
    int      iErrorNo;                 /* 进程启动出错次数                              */
    int      iFlags;                   /* 0--无  1--内部进程  2--外部   8--进程重启     */
    unsigned long lRunSec;             /* CPU时间 秒                                    */
    unsigned long lRunUsec;            /* CPU时间 豪秒                                  */
    unsigned long lSysSec;             /* 系统时间                                      */
    unsigned long lSysUsec;            /* 占用内存                                      */
    unsigned long lGetTime;            /* 取样时间                                      */
} ncProcessInfo;

/* 进程管理中的第一条记录   */
typedef struct ncProcessHead_s {
    int             iPid;                      /* 本进程的进程号    */
    unsigned long   lStartTime;                /* 启动时间          */
    int             iDo;                       /* 处理标识 0--正常  */
    int             iFlags;                    /* iFlags == 0-清掉在线信息 1--保留在线信息 2---直接恢复 */
    unsigned long   lLastTime;                 /* 进程最后操作时间  */
    
} ncProcessHead;


    

/* 进程信息     */   
#define NC_PROC_END         2     /* 进程被强迫终止   */
#define NC_PROC_RESET       8     /* 进程需要重启  */
#define NC_PROC_STOP        9     /* 进程需要停止  */


typedef struct ncEimProcess_s {
    char     caName[32];           /* 别名                          */
    char     caPath[64];           /* 路径                          */
    char     caFun[32];            /* 功能名称                      */
    int      iPrio;                /* 优先级                        */
    int      iPid;                 /* 0 表示没有启动                */
    unsigned long lStartTime;      /* 启动时间                      */
    unsigned long lTimeOut;        /* 超时时间                      */
    unsigned long lOnTime;         /* 在每天指定时间重新启动        */
    unsigned char caControl[32];   /* 控制信息                      */
    int      iStatus;              /* 0--不启动   1--启动   -1 出错 */
    int      iFlags;               /* 1--单进程   2--多进程         */
} ncEimProcess;


#define  NC_ONTIME_IPCHECK        0   /* 定时检查IP缓存             */
#define  NC_ONTIME_PROCESSCHECK   1   /* 定时检查进程               */
#define  NC_ONTIME_COUNT          2   /* 数据统计                   */
#define  NC_ONTIME_DELDETAILLOG   3   /* 清除详细信息               */
#define  NC_ONTIME_DELTEMPFILE    4   /* 删除临时文件               */
#define  NC_ONTIME_DATABACK       5   /* 数据备份                   */
#define  NC_ONTIME_CHECKIPTAB     6   /* 检查IPtables               */
#define  NC_ONTIME_RELOADCNF      7   /* 检查配置重栽               */
#define  NC_ONTIME_TIMECONTROL    8   /* 检查时间控制               */
#define  NC_ONLINE_RESTARTNC      9   /* 重新启动NC                 */


typedef struct ncOnTimeFun_s {
    unsigned long lLastTime;            /* 最后一次执行时间  */
    unsigned long lUsedTime;            /* 最后一次执行使用时间  */
    unsigned long lCount;               /* 执行次数      */
    unsigned long lStepTime;            /* 间隔时间      */
    unsigned long lOnTime;              /* 指定时间 hhmmss       */
    int      iStatus;                   /* 0--空闲  1--等待 2--正在执行  */
    char     caName[32];                /* 功能名称      */
    int   (*fFunName)(utShmHead *);     /* 执行函数名称  */
} ncOnTimeFun;


/* 邮件过滤规则  */
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
    unsigned char cFlags;     /* 限制标识  2--个人 1--组  0-- 全部 */
    unsigned char cActflags;  /* 处理方式 0--备份 1--不备份  */
    unsigned char control;    /* 邮件控制 1--正常 2--拒绝  3--发到指定地址  4--等待确认 */
    unsigned char cTzflags;   /* 通知方式 0--不通知  1--通知发件人  2--收件人 3--指定邮箱  */
    unsigned char cStype;     /* 发送类别 0--全部                                          */
    unsigned char cReserv[3];
    char caSmail[64];         /* 转发邮箱        */
    char caTzmail[64];        /* 通知帐号        */
    char caTzcont[128];       /* 通知内容        */
} ncMailCtl;

/* Add 2002/11/30 */
typedef struct ncFieldList_s {
    char   caFname[32];
    char   cType;          /* 类型 n-数字 s-字符 */
    char   cFlags;
    short  nLen;
} ncFieldList;

/* 调试信息  */
extern int iDebugLevel;
#define NC_DEBUG_ERROR    1            /* 错误信息  */
#define NC_DEBUG_WARN     2
#define NC_DEBUG_LOG      4
#define NC_DEBUG_MSG      8
#define NC_DEBUG_SHMLOAD 16

#define NC_DEBUG_TCOUT   116           /* 输出TC调试信息  */



/* gtype    组类型    */
#define NC_GTYPE_LOCAL      0           /* 本地       */
#define NC_GTYPE_USER       1           /* 单个用户   */
#define NC_GTYPE_GROUP      2           /* 组         */
#define NC_GTYPE_COMPANY    3           /* 公司       */
#define NC_GTYPE_POLICY     4           /* 策略       */
#define NC_GTYPE_CENTER     8           /* 中心       */
#define NC_GTYPE_SYSTEM     9           /* 系统       */



/* DHCP数据结构  
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
#define MACIP_FROM_PKG           0      // 来源于数据包
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
#define MACIP_FROM_POP3          15    // POP2帐号
#define MACIP_FROM_PPPOE         17    // PPPOE帐号
#define MACIP_FROM_QQ            18    // QQ帐号
#define MACIP_FROM_LOCALAUTH     20    // 本地验证   LocalAuth






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
    unsigned char  hlen;     /* MAC地址长度 6                */
    unsigned char  hops;     /* 转发次数                     */
    unsigned long  xid;      /* Transaction ID               */
    unsigned short secs;
    unsigned short flags;
    unsigned long  ciaddr;   /* 目前Client所使用的IP地址     */
    unsigned long  yiaddr;   /* 要分配的IP地址               */
    unsigned long  siaddr;
    unsigned long  giaddr;
    unsigned char  chaddr[16];
    unsigned char  sname[64];
    unsigned char  file[128];
} ncDhcp;



typedef struct ncComputeMacIp_s {
    unsigned char  mac[6];      /* Mac地址                      */
    char           flags;       /* 记录来源 1--固定不变 8--Wins 9--DHCP */
    char           status;
    unsigned long  ip;          /* Ip地址                       */
    unsigned char  cname[16];   /* 主机名,超过15位自动截掉      */
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


/* 该数据存放在内存中，用于查找相应机器的相关信息    */
typedef struct ncComputeIpMac_s {
    unsigned long  ip;          /* Ip地址                                   */
    char           flags;       /* 记录来源 1--固定不变 8--Wins 9--DHCP      */
    char           status;      /* 当前状态 0--不在线  1--在线               */
    unsigned char  mac[6];      /* MAC地址                                  */
    unsigned long  lLastTime;   /* 最后操作时间                              */
    unsigned char  attr[80];    /* 属性, u:uname,d:disp,g:group             */
} ncComputeIpMac;


/* 字典属性    */
#define NC_DICT_MAXVAR           8    // 一条记录允许最大属性数量
#define NC_DICT_USERNAME         1    // 用户名
#define NC_DICT_DISPNAME         2    // 显示名
#define NC_DICT_MAC              3    // MAC地址
#define NC_DICT_GROUP            4    // 部门名称
#define NC_DICT_COMPNAME         5    // 计算机名
#define NC_DICT_POSITION         6    // 位置
#define NC_DICT_ROLE             7    // 用户角色

/* 字典来源    */
#define NC_DICTFROM_USERDEFINE   1    // 用户自定
#define NC_DICTFROM_CLIENT       2    // 客户端
#define NC_DICTFROM_NTDOMAIN     3    // NT域


/* 字典性质    */
#define NC_DICTFLAGS_DYNAMIC     1    // 动态生成, 该性质的内容根据超时时间自动删除
#define NC_DICTFLAGS_KEEP        2    // 保存,该性质的内容将永远在内存中



/* 在线字典信息   
   #   IP       From   Content                                        LastTime
   192.168.0.1  1      "1liyunm;2李域民;311:11:22:22:11;4销售部"     "2009/03/29 11:12:12"
 */
typedef struct ncOnlineDictionary_s {
    unsigned  long  ip;            /* 客户端IP地址                          */
    unsigned  long  lasttime;      /* 最后操作时间                          */
    unsigned  char  cFrom;         /* 来源,如 NT客户端,DHCP,域等            */
    unsigned  char  cFlags;        /* 超时标识 0--静态  1--动态  */
    unsigned  char  cRev[2];
    unsigned  char  content[116];  /* 字典内容         */
} ncOnlineDict;

typedef struct ncDictionaryVar_s {
    int             cType;           /* 类别       */
    unsigned char   caValue[32];     /* 字典内容   */
} ncDictVar;


#define NC_SWITCH_TL_SL3226P  2001
#define NC_SWITCH_TL_SL3210   2002


/* 交换机端口信息  */
typedef struct ncSwitchPort_s {
    unsigned long  lSid;          /* 序列号              */
    unsigned long  lSwid;         /* 交换机ID             */    
    char           caPort[16];    /* 端口                 */
    char           caName[32];    /* 位置                 */
    unsigned char  mac[6];
    char           status;        /* 端口状态  2--Down 1-Up   0 None    */
    char           flags;         /* 1 作过操作  0-- 未作过操作   9 集连       */
} ncSwitchPort;

typedef struct ncSwitchInfo_s {
    unsigned long  lSwid;         /* 交换机ID             */
    char           caName[32];    /* 交换机名称           */
    char           caType[32];    /* 类型                 */
    unsigned long  ip;            /* Ip地址               */
    unsigned short nPort;         /* 端口                 */
    char           status;        /* 状态                 */
    char           flags;
    char           caUsername[16];
    char           caPassword[16];
    int            iStart;
    int            iSumPort;
    unsigned long  lLastTime;     /* 最后时间             */
} ncSwitchInfo;

/* 交换机处理方式   */
typedef struct ncSwitchDo_s {
    char  caType[32];          /* 交换机类型   */
    int   iWay;                /* 操作方式     23-Telnet 80-Http 62-SNMP  */
    int   (*fSwitchGetMac)(ncSwitchInfo *,ncSwitchPort *,int);    /* 取Mac地址和端口 */
} ncSwitchDo;

/* 交换机列表   */
typedef struct ncSwitchList_s {
    char  caType[32];          /* 交换机类型   */
    int   iFlags;
    int   iWay;
} ncSwitchList;


typedef struct ncUtlFileList_s {
    char name[64];                  /* 文件名     */
    unsigned long lSize;            /* 文件长度   */
} ncUtlFileList;


typedef struct ncUtlDBSynInfo_s {
    int           iFileServerId;    /* 文件服务器的进程号 */
    int           iProcessId;       /* 数据处理进程ID     */
    unsigned long lStartTime;       /* 起动时间           */
    unsigned long lMailMon;         /* 监控邮件的数量     */
    unsigned long lFormMon;
    unsigned long lMailCtl;
    unsigned long lDbLog;
    unsigned long long lBytes;      /* 同步的流量       */
    unsigned long lLastTime;        /* 最后一次同步时间 */
    unsigned long lLastDoTime;      /* 最后一次处理时间  */
    unsigned long lUndoFile;        /* 未处理的文件数量 */
    char     caDoFile[128];         /* 当前正在传送的文件  */
    char     caCurFile[128];        /* 正在处理            */
} ncUtlDBSynInfo;

typedef struct ncMailList_s {
    char mail[60];                  /* 邮件地址 */
    char cRev[2];
    char cdo;                       /* 处理方式 */
    char cflags;
} ncMailList;

typedef struct ncGroupIpList_s {
    unsigned long lSip;
    unsigned long lEip;
    unsigned long lGroupid;
} ncGroupIpList;

#define  NC_PID_GETSMTPIP1    29     /* 取得Proxy对照表  */
#define  NC_PID_GETSMTPIP2    30     /* 取得Proxy对照表  */
#define  NC_PID_GETSMTPIP3    31     /* 取得Proxy对照表  */
#define  NC_PID_GETSMTPIP4    32     /* 取得Proxy对照表  */
#define  NC_PID_MAILSEND      33     /* 启动邮件发送系统  */
#define  NC_PID_PASPROXY      34     /* 启动PASProxy功能  */
#define  NC_PID_NCCLIENT      35

/* 产品敏感信息, 共1024字节
     整个文件用DES算法加密
 */
#define NC_PKS_ID             20060928L
typedef struct ncProdSysInfo_s {
    unsigned long  lSysId;          /* 系统ID            */
    unsigned long  lLen;            /* 文件长度          */
    unsigned long  lCheck;          /* 校验位,使用Md5(ID)计算,若不正确，系统将不能启动 */
    unsigned long  lCreateTime;     /* 创建时间          */
    unsigned long  lMgrIp1;         /* 管理中心地址      */
    unsigned long  lMgrIp2;         /* 管理中心地址      */
    unsigned long  lProdSrv1;       /* 产品中心地址      */
    unsigned long  lProdSrv2;       /* 产品中心地址      */
    unsigned short nMgrPort;        /* 管理中心端口      */
    unsigned short nMgrProxy;       /* 管理中心Proxy端口 */
    unsigned short nProdPort;       /* 产品中心端口      */
    unsigned short nUpdatePort;     /* 升级端口          */
    unsigned long  lRev[2];
    unsigned char  prodid[32];      /* 产品序列号        */
    unsigned char  uname[64];       /* 授权用户          */
    unsigned char  dbname[32];      /* 数据库名称        */
    unsigned char  dbuser[16];      /* 数据库用户        */
    unsigned char  dbpass[16];      /* 数据库口令        */
    unsigned char  cltname[32];     /* 客户端名称        */
    unsigned char  cltkey[32];      /* 登录密钥          */
    unsigned char  UpdateHost[64];  /* 产品升级服务器    */
} ncProdSysInfo;

typedef struct ncCaseWarnMsg_s {
    unsigned long lCid;
    unsigned long lSid;                /* 策略ID, x00000001   */
    unsigned char acid;               /* 管理中心ID  */    
    char          caMsg[255];
} ncCaseWarnMsg;


/* 黑白名单     MAC地址应该唯一
   以mac地址为排序的列表
 */

typedef struct ncBlackWhite_s {
    unsigned long  ip;
    unsigned char  position[32];
    unsigned char  mac[6];
    unsigned char  flags;
    unsigned char  cLen;
} ncBlackWhite;



/* 虚拟身份上下线信息
 */
typedef struct  ncVidList_s {
    unsigned long   lSip;          //      --- 源IP地址
    unsigned short  lVtype;        //      --- 虚拟身份类别
    unsigned short  nNum;          //      --- 序号
    unsigned char   caName[40];    //      --- 虚拟身份名称
    unsigned char   caDisp[64];    //      --- 虚拟身份显示名
    unsigned char   caPass[24];    //      --  密码,若有
    unsigned long   lSid;          //      --- 对应于数据库中的Sid
    unsigned long   lDip;          //      --- 目标IP地址
    unsigned long   lStartTime;    //      --- 开始上线时间
    unsigned long   lLastTime;     //      --- 最后访问时间
    unsigned long   lWarnTime;     //      --- 告警时间
    unsigned long   lUserId;       //      --- 对应的用户ID
    unsigned long   lUsid;         //      --- 对应于上网的Sid
    unsigned char   flags;         //      --- 1-上线   0-正在   9-离线
    unsigned char   status:4;      //      --- 0-正常   1-禁止使用
    unsigned char   cUp:4;         //      --- 是否上传到管理中心
    unsigned short  nRev;          //      --- 记录总数
}   ncVidList;




/* 虚拟身份控制,出现在这个表内的都是被禁用的

 */
typedef struct  ncVidDrop_s {
    unsigned long  lSip;          //      --- 源IP地址
    unsigned long  lVtype;        //      --- 虚拟身份类别
    unsigned long  lLastTime;     //      --- 最后访问时间
    unsigned char  cStatus;       //      --- 状态
    unsigned char  cFun;          //      --- 禁止的功能
    unsigned char  rev[2];        //      --- 备用
} ncVidDrop;


/* 同步在线用户  */
typedef struct ncSynOnUser_s {
    unsigned long  ip;            /* Ip地址                       */
    unsigned char  mac[6];
    char           flags;         /* 记录来源 1--固定不变 8--Wins 9--DHCP */
    char           status;        /* 状态 0--不在线  1-在线   */
    char           name[32];      /* 用户帐号   */
    char           dispname[32];  /* 显示名     */
    char           group[32];     /* 组名       */
    unsigned long  lasttime;      /* 最后更新时间  */
} ncSynOnUser;


typedef struct ncSensContent_s {
    unsigned char  ctype;         /* 匹配类别     0-正文  1-正则表达式  2-扩展正则  */
    unsigned char  ccase;         /* 大小写敏感   0-不敏感  1-敏感                  */
    unsigned short len;           /* 字符串长度                                     */
    regex_t        sreg;          /* 编译后的内容  */
} ncSensContent;

/* 敏感内容及类别   */

typedef struct ncSensContIndex_s {
    unsigned long  id;        /* 类别      */
    char           name[32];  /* 说明      */
    unsigned long  sum;       /* 数量      */
    ncSensContent  *psCont;    
} ncSensContIndex;


typedef struct ncSensContHead_s {
    int  iSumIndex;
    ncSensContIndex *psIndex;
} ncSensContHead;


typedef struct ncCommandBuf_s {
    int    iResetFlags;          /* 重新启动标识   1--重启全部NC   2--重启除Web外的NC   3--停止除Web外的NC   4-停止全部NC  5-重启指定NC */
    int    iLock;                /* 1-锁定      */
    char   caText[128];          /* 说明        */
    char   caName[32];           /* 进程名称    */
} ncCommandBuf;

// 负载均衡设备
#define NC_PKGBLAN_MAX  16
typedef struct ncPkgBlance_s {
    long long   lUBytes;                /* 上行             */
    long long   lDBytes;                /* 下行             */
    long long   lUPkg;                  /* 上行数据包       */
    long long   lDPkg;                  /* 下行数据包数量   */
    unsigned short nMark;               /* Mark值           */
    long        lBase;                  /* 基数             */
} ncPkgBlance;

typedef struct ncPkgBlanceHead_s {
    long        lSum;                   /* 出口数量         */
    ncPkgBlance *psBlan;
} ncPkgBlanceHead;

    

#define NC_MAX_NAMECACHE      1000   // 最大名字缓存
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
    unsigned long lTime;                   /* 持续时间             */
    unsigned long long lUp[2];             /* 0--上次　　１--本次  */
    unsigned long long lDown[2];
} ncGroupFlow;


// Mac地址表   
// NC_LNK_MACIPLIST 90
// flags 的含义
//   10-19  虚拟身份
//   
#define NC_MACTABFROM_PPPOE         20      // PPPOE


typedef struct ncMacTables_s {
    unsigned char  mac[6];      /* Mac地址                      */
    unsigned char  flags;       /* 记录来源 PPPOE,...           */
    unsigned char  status;      /* 1 操作过  0 未操作过         */
    unsigned long  ip;          /* Ip地址                       */
    unsigned long  lasttime;    /* 最后更新时间                 */    
    unsigned char  name[32];    /* 名字                         */
} ncMacTables;


/* 存放股票上下文关键字内容*/
typedef struct ncStockContext_s {
    unsigned long       lId;        /* 主键 */
    unsigned long       lLen;        /* 内容长度 */
    unsigned long       lLasttime;   /*最近时间*/
    unsigned char       cont[64];    /*内容*/
} StockContext;


/* 存放迈外迪接收到用户信息*/
typedef struct ncMwduser_s {
	  char caName[32];               /*帐号*/
	  char caDispname[32];           /*姓名*/
	  char caSex[4];                  /*性别*/
	  char caIdtype[4];               /*证件号码*/
	  char caIdno[20];                /*证件号码*/
	  char caAddr[64];                /*家庭地址*/
	  char caMobile[20];              /*手机号*/
	  char caIp[16];                 /*IP地址*/
	  char caMac[20];                 /*mac地址*/
	  char caTime[16];                /*时间*/
	  char caPosition[32];            /*位置*/
	  char caCountry[4];              /*国家代码*/
} ncMwduser;

/* 存放短信猫手机用户*/
typedef struct ncSmsuser_s {
	  char caMobno[20];               /*手机号*/
	  char caPass[32];                /*密码*/
} ncSmsuser;


/* 验证信息    */
typedef struct ncAuthRoute_s {
    unsigned    long    lSip;
    unsigned    long    lEip;
    unsigned    long    lAuthIp;
    unsigned    short   nPort;
    unsigned    short   nRev;
    char                caPlate[64];
}   ncAuthRoute;

/*管理中心验证返回的用户信息*/
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
//农商行验证用户
typedef struct ncNshUser_s{
 	  char caUsername[32];            //员工号
 	  char caDispname[32];
 	  char ustat[8];                  //状态    00--帐号未激活 001-验证通过 002-账户已停用 003-验证未通过 p01 初始密码，强制要求修改密码 p02 密码已过期
 	  char nettime[12];               //允许上网时间
 	  char startdate[20];             //允许上网开始时间
 	  char enddate[20];               //上网结束时间
 	  char orgname[64];               //部门     
    } ncNshUser;
//上海浩易集中验证
 typedef struct ncHyuser_s {
           char caIdno[24];
           char caIdtype[8];
           char caName[20];
           char caRoom[20];
   } ncHyuser;   
    
// VlanID和位置对照表

typedef struct ncVlanIdPos_s {
    unsigned long  lVlanId;                     // 索引
    char           caPosition[32];              // 位置
} ncVlanIdPos;

// 位置和姓名对照表
typedef struct ncPositionName_s {
    char           caPosition[32];              // 位置
    char           caPass[16];                  // 密码
    char           caName[32];                  // 姓名
    unsigned long  lSid;                        // 对应的Sid
} ncPositionName;


/*    字符串匹配       ? -- 单个   *--任意个
      01 02 03 * 04 ? 05
      
      Start,Len
      Off,Len
      
*/

typedef struct ncUtlStrPat_s {
    short Len;
    short type;                     // 0--?    1-*   2--Cont
    unsigned char caCont[64];
} ncUtlStrPat;


// 部门单位数据包数量
// 第0个位置保存的是缺省单位
typedef struct ncGroupDept_s {
    uint4  lSnum;           // 在服务器端的位置
    uint4  lDid;            // 在服务器端的Id
    uchar  caDname[32];
    long long   lUp;
    long long    lDown;
    uint4  lUser;
} ncGroupDept;



// 虚拟身份协议分析  */

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
    uint4  lGtype;                   // 1--个人  2--组  3--全部
    uint4  lGroupid;
    uint4  lLineid;
    uint4  lQosid;
    char   caLinename[32];
} ncLineGroupInfo;


#include "ncalloc.h"
#endif
