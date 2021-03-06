/* 内网管理数据结构      */
#ifndef  __NWDEF__
#define  __NWDEF__
#define  NW_MSG_CODE_REQ  990118101L 
#define  NW_MSG_CODE_RES  990118102L
#define  NWVERSION  "20141222.101"
#define  NW_DES_KEY       "ProEIM2005"
#define NC_UDP_IDREQ      1001          /* 请求客户端ID   */
#define NC_UDP_REGREQ     1002          /* 用户登录请求   */
#define NC_UDP_HDWINFO    1003          /* 请求发送硬件信息  */    
#define NC_UDP_SOFTINFO   1004          /* 请求发送软件信息  */    
#define NC_UDP_CTLINFO    1005          /* 请求发送控制信息  */    
#define NC_UDP_TESTNET    1006          /* 网络测试          */
#define NC_UDP_HEARTBEAT  1008          /* 用户心跳信息      */
#define NC_UDP_ACCLOG     3001          /* 日志信息          */
#define NC_UDP_SYSINFO    3002          /* 系统信息          */
#define NC_UDP_PROCINFO   3003          /* 当前进程信息      */
#define NC_UDP_CAPSCREEN    3004          /* 客户端截屏信息    */
#define NC_UDP_CAPSCREENTCP 3014          /*接收截屏文件*/
#define NC_UDP_CPUMEMSTART  3005          /* CPU和内存连续监控开始 */
#define NC_UDP_CPUMEMSTOP   3006          /* CPU和内存连续监控结束 */


#define NC_UDP_KILLPROC   2001          /* 杀掉远程进程      */

#define NC_UDP_DEVSTATUS  2004
#define NC_UDP_APPINFO    2005
#define NC_UDP_APPQUIT    2010
#define NC_UDP_APPUNINSTALL    2012
#define NC_UDP_UPDATEINFO 2014

#define NC_CLIENT_LOGIN   5001
#define NC_CLIENT_BEAT    5002
#define NC_CLIENT_LOGSEND 5003
#define NC_CLIENT_PLOCY   5004
#define NC_CLIENT_SENDFILE 5005
#define NC_CLIENT_SCREEN   5006
#define NC_SCREEN_REQ      5007
#define NC_CLIENT_BEAT2    5010 
#define NC_CLIENT_REQPROCY    5012     //发送进程策略
#define NC_CLIENT_REQJOINPROCY    5014     //发送准入规则策略
#define NC_CLIENT_REQWATCHPROCY    5016     //发送网络监视策略
#define NC_CLIENT_REQDATE    5018     //发送时间策略
#define NC_CLIENT_IPMAC 5111

#define NC_NWPROCUPDATE_REQ      6001
#define NC_NWUPDATEREADY_REQ 6002
#define NC_NWPROCUPDATE_END 6003
#define NC_CLIENT_LOGIN2   7001
#define NC_CLIENT_EXIT2   7002
#define NC_CLIENT_KICKOFF   7009          //强制某个IP的客户端下线
#define NC_CLIENT_PASSCHG   7003         //修改域用户密码（插件）
#define NC_CLIENT_SYNDEPUSER   7004         //同步用户部门（插件)
#define NC_CLIENT_AUTHBYNT   7006         //域插件代理验证

#define NC_USBKEY_NOMONHEART   8001         //USBKEY免监控心跳信息
#define NC_USBKEY_MONLOGIN   8002         //审计USBKEY登录信息
#define NC_USBKEY_MONHEART   8003         //审计USBKEY心跳信息
#define NC_USBKEY_PASSLOGIN   8005         //审计USBKEY带密码验证

#define NW_LOG_SUCESS       0
#define NW_LOG_ISLOGIN      1                     /* 用户已登录      */
#define NW_LOG_ISEXIST      2                     /* 用户已存在      */
#define NW_ERR_NOEXIST      101                   /* 记录不存在      */
#define NW_ERR_NOLOGIN      102                   /* 客户端没有登录  */
#define NW_ERR_RELOGIN      103                   /* 用户必须重新登录  */
#define NW_ERR_INVKEY       104                   /* 校验的Key不存在   */

#define NW_ERR_MAXUSER      109                   /* 超出最大用户数  */
#define NW_ERR_OTHER        199                   /* 其它错误        */
#define NW_ERR_DBSYSTEM     200                   /* 数据系统出错    */
#define NW_ERR_DBINSERT     211                   /* 数据插入出错    */
#define NW_ERR_DBUPDATE     212                   /* 数据更新出错    */


#define LAN_CLIENT_UPDATE   0X00000001            /*通知客户端升级*/
#define LAN_CLIENT_CONFIG   0X00000002            /*获取客户端配置*/
#define LAN_CLIENT_SOPER      0X00000004         /*客户端服务操作*/
#define LAN_CLIENT_MSG      0X00000008            /*下发消息*/
#define LAN_CLIENT_SCREEN   0X00000010            /*抓屏*/
#define LAN_CLIENT_CONTROL   0X00000020            /*远程操作*/
#define LAN_CLIENT_COPER    0X00000040            /*客户端操作*/
#define LAN_CLIENT_PROCY    0X00000080            /*客户端下发策略*/
#define LAN_CLIENT_HEART    0X00000100            /*更换心跳时间*/
#define LAN_CLIENT_RELOGIN  0X00000200            /*要求重新登录*/
#define LAN_CLIENT_PROC    0X00000400            /*查看进程信息*/


#define NC_LNK_NW_GROUP          170      /* 内网单位组 */
#define NC_LNK_NW_SOFTUP          171      /* 内网软件升级策略 */
#define NC_LNK_NW_PROCY          172      /* 内网软件升级策略 */
#define NC_LNK_NW_PROCYOBJ       173      /* 策略下发对象 */
#define NC_LNK_NW_COMPY          174      /* 公司信息 */
#define NC_LNK_NW_BLACKLIST      175      /* 黑白名单信息 */
#define NC_LNK_NW_PLAST      176      /*  计算机最后策略时间 */
#define NC_LNK_NW_CLEAN      177      /*统计待清理的日志*/  
/* 部门信息  */
typedef struct lanGroupInfo_s {
	  unsigned long  lGroupid;
    char            caGroupname[32];         /* 部门名称*/
    unsigned long  lBegip;                   /*起始IP*/
    unsigned long  lEndip;                   /*结束IP*/ 
    unsigned long  lPid;                     /*父节点ID*/                                      
} lanGroupInfo;

/* 公司  */
typedef struct lanCompyInfo_s {
    char           caCompny[64];         /* 公司名称*/
    unsigned long  gatwayip;              /*出口IP*/
    unsigned long  lR1;                   /*保留*/ 
    unsigned long  lR2;                   /*保留*/  
    unsigned long  lR3;                   /*保留*/  
    char           caR[32];               /*保留*/
} lanCompyInfo;

/* 软件升级策略信息  */
typedef struct lanSoftUpdate_s {
	  unsigned long  lVflag;                    /*客户端类别 0-对全部有效 1-对XP 32位win7有效 2-对win7、8(64位） 有效  3-win8(32)*/
    unsigned long  lObjtype;                  /*对象类型 0，全部 1-部门 2-计算机*/
    unsigned long  lObjid;                    /*对象ID*/
    unsigned long  lSoftid;                   /*软件id*/
    char  caFilename[128];                     /*文件名称*/
    char caMd5[36];                            /*MD5值*/
    char caVersion[16];                        /*版本*/                                 
} lanSoftUpdate;


/* 设备信息  */
typedef struct nwDevInfo_s {
    char            caName[16];         /* 设备名称                                         */
    unsigned short  nNum;               /* 序号，若多个从0开始                              */
    unsigned char   cFlags;             /* 0--正常  1--不变   M--修改   A--新增  D--删除    */
    unsigned char   cRev;
    unsigned long   lSid;               /* 对应数据库中的SID, 若为0，表示为新记录           */
    char            caAtt[128];         /* 属性                                             */
    unsigned long   lValue;             /* 数字值，若无用0表示                              */
    struct nwDevInfo_s *next;           /* 下一条记录, NULL 无下面记录                      */
} nwDevInfo;

/* 软件信息  */
typedef struct nwSoftInfo_s {
    char            caName[72];         /* 软件名称    */
    char            cType;              /* 类别        */
    unsigned char   cFlags;             /* 0--正常  M--修改   A--新增  D--删除    */
    char            caRev[38];          /* 保留        */
    char            caVersion[16];      /* 版本号      */
    char            caDate[16];         /* 安装日期    */
    unsigned long   lSid;               /* 对应数据库中的SID, 若为0，表示为新记录           */
    unsigned long   lSize;              /* 大小    KB    */
    struct nwSoftInfo_s *next;          /* 下一条记录, NULL 无下面记录                      */
} nwSoftInfo;

/* 进程信息  */
typedef struct nwProcInfo_s {    /* 156字节   */
    char            caName[72];         /* 进程名称    */
    unsigned long   iPid;               /* 进程号    不用  */
    char            stime[20];          /* 进程开始时间 不用*/
    long            cputime;            /* CPU时间     改为进程开始时间 */
    long            cpurate;            /* CPU占有率    改为进程最后使用时间*/
    unsigned long   mem;                /* 内存         */
    char            cFlags;
    char            cRev[79];
    struct nwProcInfo_s *next;          /* 下一条记录, NULL 无下面记录                      */
} nwProcInfo;

struct nwLDriver_s {
    char     caName[4];
    unsigned long lSum;
    unsigned long lLeft;
};

typedef struct nwSysInfo_s {    /* 156字节   */
    unsigned long   lHdsum;
    unsigned long   lHdleft;
    unsigned long   lCdsum;
    unsigned long   lCdleft;
    unsigned long   lMdsum;
    unsigned long   lMdleft;
    unsigned long   lMemsum;
    unsigned long   lMemleft;
    unsigned long   lDiskSum;
    unsigned long   lCdCount;
    unsigned long   lMdCount;
    unsigned long   lCpu;
    struct   nwLDriver_s sD[9];
} nwSysInfo;


/* 数据交换缓冲区  */
typedef struct nwSwitchBuf_s {
    char                 caBuf[188];
    struct nwSwitchBuf_s *next;           /* 下一条记录, NULL 无下面记录                      */
} nwSwitchBuf;

typedef struct nwSwitchHead_s {
    nwSwitchBuf         *first;                     /* 第一条记录                 */
} nwSwitchHead;



/* 头信息  */
typedef struct nwDevHead_s {
    nwSoftInfo          *firstSoft;                 /* 软件同步信息      */
    nwDevInfo           *firstDev;                  /* 硬件同步信息      */
    nwProcInfo          *firstProc;                 /* 当前进程信息      */
    nwSysInfo           *sysInfo;                   /* 系统信息          */
    unsigned long       lScrId;                     /* 屏幕保存的ID      */
    unsigned long       lProcTime;                  /* 最后一次同步时间　*/
    unsigned long       lDevTime;                   /* 最后一次同步时间　 */
    unsigned long       lSoftTime;                  /* 最后一次同步时间　 */
    unsigned long       lSysTime;                   /* 系统信息更新时间   */
    unsigned long       lScrTime;                   /* 屏幕信息更新时间   */
} nwDevHead;

    
typedef struct nwCompInfo_s {
    unsigned long  compid;             /* 终端ID        */
    unsigned char  devname[16];        /* 设备名称      */
    unsigned long  ip;                 /* 客户端IP地址  */
    unsigned long  groupid;             
    unsigned long  userid;
    unsigned char  compname[16];
    unsigned char  os[32];
    unsigned char  ie[32];     
    unsigned char  mac[6];              /* MAC地址       */
    unsigned char  cmod;                /* 修改状态 0--正常  1--已修改,需要和数据库同步   */
    unsigned char  status;              /* 状态 0--离线   1--在线        */
    unsigned char  version[20];   
    unsigned char  caKey[18];           /* Key,用户客户服务器检验的的key */
    unsigned long  htime;               /* 硬件最后更新时间  1--正在同步   */
    unsigned long  stime;               /* 软件最后更新时间  存放ncupsoftprocy中的SID  */
    unsigned long  ctime;               /* 控制信息最后更新时间    */
    unsigned long  kickoff;             /*1-被另一个客户端强制踢下线*/
} nwCompInfo;

/* 在线信息  */
typedef struct nwCompIp_s {
	  unsigned long  compid;               /* 终端ID        */
    unsigned long  ip;                   /* 客户端IP地址  */
    unsigned long  lLoginTime;
    unsigned long  lLastTime;           /* 最后更新时间        */
    unsigned long  lSynTime;            /* 最后一次更新时间    */
    unsigned short nUdp;                /* 客户端UDP端口 */
    unsigned short nTcp;                /* 客户端TCP端口 */
    unsigned long lFlow;                /*网络流量*/
    unsigned long lNetlink;             /*网络连接数*/
    unsigned char  caKey[18];
    unsigned char  status;              
    unsigned char  cSysInfo;            /* 当前系统信息 0--未同步  1--同步          */  
    unsigned char  cProcInfo;           /* 当前进程信息 0--无进程信息 1--有进程信息 */
    unsigned char  cScreen;             /* 当前屏幕信息 0--未准备     1--准备完毕   */
    unsigned char  cRev[2];            
    nwCompInfo     *psComp;
    nwDevHead      sDevHead;           /* 正在同步的缓存信息      */
    unsigned long lCtrl;               /*控制命令*/
} nwCompIp;

typedef struct nwProcCtl_s {    /* 进程策略   */
    unsigned long   stype;      /*1－计算机 2－部门 3－全部*/
    unsigned long   uid;        /*stype=1 计算机id stype=2 部门id */
    unsigned long   ctype;      /*0－禁止使用 1－允?硎褂?/
    unsigned long   did;        /*0－每天*/
    char            stime[12];   /*起始时间 hhmmss*/
    char            etime[12];   /*终止时间 hhmmss*/
    char            pname[32];  /*进程名*/
} nwProcCtl;
  
typedef struct nwCtlWatch_s {    /* 内网监控策略   */
    unsigned long   stype;      /*1－计算机 2－部门 3－全部*/
    unsigned long   uid;        /*stype=1 计算机id stype=2 部门id */
    unsigned long   im;        /*0－不记录IM 1－记录IM*/
    unsigned long   screen;        /*0－不记录screen 1-记录screen*/
} nwCtlWatch;  

typedef struct nwJoinCtl_s {    /* 准入策略   */
    unsigned long   stype;      /*1－计算机 2－部门 3－全部*/
    unsigned long   uid;        /*stype=1 计算机id stype=2 部门id */
    unsigned long   ctype;      /*0－启用 1－停止使用*/
    unsigned long   did;        /*0－每天*/
    char            stime[12];   /*起始时间 hhmmss*/
    char            etime[12];   /*终止时间 hhmmss*/
    char            pname[32];  /*进程名*/
    char            version[12]; /*版本*/
    unsigned long   ctl;         /*0-允许上网 1－禁止上网*/
    unsigned long   id;         /*1-防毒软件 2－操作系统*/
} nwJoinCtl;
  
typedef struct nwDate_s {    /* 策略中的日期   */
    unsigned long   id;      /*日期ID*/
    unsigned long   ltype;        /*类型 1--Day 2--Week 3--Month 4--Year*/
    unsigned long   llower;        /*时间下限 1-YYYYMMDD 2--0/6 3--1/31 4--MMDD */
    unsigned long   lupper;         /*时间上限 1-YYYYMMDD 2--0/6 3--1/31 4--MMDD*/
} nwDate;  


typedef struct lanPfile_s {    /* 文件控制策略*/
    unsigned long   pid;      /*主策略ID*/
    char filename[128];        /*文件名称*/
    unsigned long   optype;    /*控制状态*/
    unsigned long   snap;        /*截屏快照   0--不截屏  2-截屏*/
    unsigned long   type;        /*0-离线有效   1-离线无效*/
} lanPfile;  

typedef struct lanPdev_s {    /* 外设控制策略*/
    unsigned long   pid;      /*主策略ID*/
    unsigned long   optype;    /*控制状态*/
    unsigned long   freehaordid; /*放行指定U盘库ID, 0-不限制*/
    unsigned long   snap;        /*截屏快照   0--不截屏  2-截屏*/
    unsigned long   type;        /*0-离线有效   1-离线无效*/
} lanPdev; 

typedef struct lanPlink_s {    /* 非法外联控制策略*/
    unsigned long   pid;      /*主策略ID*/
    unsigned long   optype;    /*控制状态*/
    unsigned long   snap;        /*截屏快照   0--不截屏  2-截屏*/
    unsigned long   type;        /*0-离线有效   1-离线无效*/
} lanPlink; 

typedef struct lanPptype_s {    /* 程序类别控制策略*/
    unsigned long   pid;      /*主策略ID*/
    unsigned long   gtype;    /*程序类别*/
    unsigned long   optype;    /*控制状态*/
    unsigned long   stamp;     /*限制时间间隔*/
    unsigned long   snap;        /*截屏快照   0--不截屏  2-截屏*/
    unsigned long   type;        /*0-离线有效   1-离线无效*/
} lanPptype;

typedef struct lanPpname_s {    /* 进程控制策略*/
    unsigned long   pid;       /*主策略ID*/
    char pname[64];            /*程序类别*/
    unsigned long   optype;    /*控制状态*/
    unsigned long   stamp;     /*限制时间间隔*/
    unsigned long   snap;        /*截屏快照   0--不截屏  2-截屏*/
    unsigned long   type;        /*0-离线有效   1-离线无效*/
} lanPpname;

typedef struct lanPnet_s {    /* 网络控制策略*/
    unsigned long   pid;       /*主策略ID*/
    unsigned long   trantype;    /*网络协议*/
    unsigned long   sport;       /*源端口*/
    unsigned long   eport;       /*目标端口*/
    unsigned long   optype;    /*控制状态*/
    unsigned long   snap;        /*截屏快照   0--不截屏  2-截屏*/
    unsigned long   type;        /*0-离线有效   1-离线无效*/
} lanPnet;

typedef struct lanPurl_s {    /* 网址控制策略*/
    unsigned long   pid;       /*主策略ID*/
    char url[64];              /*URL地址*/
    unsigned long   urlclass;  /*网址库ID*/
    unsigned long   optype;    /*控制状态*/
    unsigned long   snap;        /*截屏快照   0--不截屏  2-截屏*/
    unsigned long   type;        /*0-离线有效   1-离线无效*/
} lanPurl;

typedef struct lanPoth_s {    /* 其它控制策*/
    unsigned long   pid;       /*主策略ID*/
    unsigned long   ctype;    /*类型*/
    unsigned long   ftime;    /*截屏时间间隔*/
    unsigned long   optype;    /*控制状态*/
    unsigned long   snap;        /*截屏快照   0--不截屏  2-截屏*/
    unsigned long   type;        /*0-离线有效   1-离线无效*/
} lanPoth;

typedef struct lanBlackList_s {    /* 黑白名单*/
    unsigned long   groupid;    	/*部门id*/
    unsigned long   computeid;    /*计算机id*/
    char   blacktype[32];    			 /*名单类型*/
    char   blackname[32];    			/*名单名称*/
    char   snap[2];								/*截屏快照*/
    char   companyname[24];				/**/
} lanBlackList;

typedef struct lanCleanData_s {    /* 清理数据*/
	char cleanKey[135];/*月份+公司简称201509lan*/
    ulong lScreenJpg;    /*文件:屏幕图片*/
	ulong lImClient; /*数据库:里聊天日志*/
	ulong lWebLog; /*数据库:网页日志*/
	ulong lProcLog; /*数据库:进程日志*/
	ulong lComputeLog; /*数据库:开关机日志*/
	ulong lOutFileLog; /*数据库:外发文件日志*/
	ulong lFileLog; /*数据库:文件日志*/
	ulong lProcScreen; /*数据库:屏幕日志*/	
} lanCleanData;


typedef struct lanPmain_s {    /* 主控制策略*/
    unsigned long   pid;       /*主策略ID*/
    char lawname[24];          /*主策略名称*/
    unsigned long   constatus;    /*状态*/
    unsigned long sumfile;        /*文件控制策略数*/
    unsigned long sumdev;
    unsigned long sumlink;
    unsigned long sumptype;
    unsigned long sumpname;
    unsigned long sumurl;
    unsigned long sumnet;
    unsigned long sumoth;
    unsigned long sumref;
    unsigned long refpid[20];        /*引用其它策略*/
    lanPfile psFile[100];              /*文件控制策略*/  
    lanPdev psDev[100];
    lanPlink psLink[100];  
    lanPptype psPtype[100];  
    lanPpname psPname[100];
    lanPnet psPnet[100]; 
    lanPurl psPurl[100];  
    lanPoth psPoth[100];
} lanPmain;



/* 策略下发对象*/
typedef struct lanProcyObj_s {
	  unsigned long  ctype;                    /*对象类别0-全部  1-部门 2-计算机*/
    unsigned long  id;                       /*对象id 0全部*/
    char         pids[1024];                 /*策略ID,以逗号分割*/
    char   dataids[1024];                   /*日期id*/  
    unsigned long  modtime;                  /*修改时间*/                           
} lanProcyObj;

/* 计算机策略最新时间*/
typedef struct lanProcyLast_s {
	  unsigned long  ctype;                  /*类别 2-计算机 1-部门 0-单位*/
	  unsigned long  id;                    /*ID 计算机ID 部门ID  或单位shortname生成的MD5数字*/
    unsigned long  basetime;                    /*基础数据最近时间*/
    unsigned long  proctime;                    /*策略最近时间*/
    unsigned long  reserv1;                     /*保留字段*/
    unsigned long  reserv2;                     /*保留字段*/
    unsigned long reserv3;                      /*保留字段*/          
} lanProcyLast;

#endif
