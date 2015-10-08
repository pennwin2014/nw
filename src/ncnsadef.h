/*  有关加速卡的控制               */
/*  2009/12/05   支持NSA4          */
/*  2009/6/24    重新修改          */
/*  2008/12/10   By Liyunming      */
#ifndef __NCNSADEF__
#define __NCNSADEF__
#define NC_NSA_VERSION     "NSA5.01.03"
#define NC_NSA_RELEASE     "2012/12/03"

#define NC_NSA_MAXNSAID     8

#define NC_NSA_OPT_FWD          0x40
#define NC_NSA_OPT_FWD_MIRROR   0x60
#define NC_NSA_OPT_HOST         0x10
#define NC_NSA_OPT_DROP         0x08


/* SSN表交换信息      */
typedef struct ncNsaSsnBuf_s {	/* 共40个字节              */
	unsigned long  lSip;	/* 源IP地址              */
	unsigned long  lDip;	/* 目标IP地址            */
	unsigned short nSport;	/* 源端口                */
	unsigned short nDport;	/* 目标端口              */
	char cProt;		        /* 协议    0--任意  6--TCP   17--UDP    */
	char cMirror;		    /* 0-不镜像   1-镜像           */
	char cDrop;		        /* 0-不丢弃   1-丢弃           */
	char cDev_s;	     	/* 发送数据包的接口号，正向,SPI接口ID+1         */
	char cDev_r;	     	/* 发送数据包的接口号，反向,SPI接口ID+1         */
	char way;
	char vc_id;
	char cFrom;             /*  1--来自交互命令                 */
	unsigned long qoslv1id_s;	/* 1级lLevel正向     上行      */
	unsigned long qoslv2id_s;	/* 2级lLevel正向               */
	unsigned long qoslv1id_r;	/* 1级lLevel反向     下行      */
	unsigned long qoslv2id_r;	/* 2级lLevel反向               */
	long lTtl;		/* 老化时间秒，若为 0不修改，转换为ssn内部单位 /30   */
} ncNsaSsnBuf;


/* QOS设置信息

 */
typedef struct ncNsaQosBuf_s {
	unsigned long lLevel;	/* lLevel                      */
	unsigned long lQosId;	/* QOS  ID                     */
	unsigned long lMax;	/* 最大带宽限制，每个单位8K字节 */
	unsigned long lMin;	/* 最小带宽限 QOS               */
	unsigned char cPrio;	/* 优先级,暂时不用              */
	unsigned char cFrom;    /* 命令来源                     */
	unsigned char cRev[18];	/* 保留                         */
	unsigned long lPrevQosId;
} ncNsaQosBuf;

/* CrossLink 命令信息   */
typedef struct ncNsaCrossLink_s {
	int iInDev;		    /* 内网ID    */
	int iOutDev;		/* 外网ID    */
	int iOpt;           /* 操作      */
	int iUp;
	int iMirror;
	char cFrom;         /* 命令来源   */
	char cRev[19];		/* 保留      */
} ncNasCrossLink;


/* 端口重定向, 本系统只支持将端口重定向到指定的IP地址和端口   */
typedef struct ncNsaRedir_s {
	unsigned long  lSip;	/* 源IP地址              */
	unsigned long  lDip;	/* 目标IP地址            */
	unsigned short nSport;	/* 源端口                */
	unsigned short nDport;	/* 目标端口              */
	char cProt;		        /* 协议    0--任意  6--TCP   17--UDP    */
	char cMirror;		    /* 0-不镜像   1-镜像           */
	char cDrop;		        /* 0-不丢弃   1-丢弃           */
	char cDev_s;	        /* 发送数据包的接口号，正向,SPI接口ID+1                           */
	char cDev_r;	        /* 发送数据包的接口号，反向,SPI接口ID+1                           */
	char cOpt;              /* 0-修改源端口和地址  1-修改目标端口和地址     */
	unsigned short nMport;  /* 修改后的端口   */
	unsigned long  lMip;    /* 修改后的地址   */
	unsigned char cFrom;    /* 命令来源       */
	unsigned cRev[15];      /* 保留           */
} ncNsaRedir;


/* 关键字   */
typedef struct ncNsaPatBuf_s {
	unsigned short scop;	/* Scop              */
	unsigned short id;	    /* Id                 */
	unsigned short len;	    /* 关键字长度         */
	unsigned char  Rev[2];	/* Id              */
	unsigned char  patstr[32];
} ncNsaPatBuf;

/* 关键字规则  */
typedef struct ncNsaPatRule_s {
	unsigned char  rule_type;	/* Scop              */
	unsigned char  protocol;	    /* Id                 */
	unsigned char  action;	    /* 关键字长度         */
	unsigned char  cRev;	/* Id              */
	unsigned long  lSip;	/* Id              */
	unsigned long  lDip;	/* Id              */
	unsigned short nSport;	/* Id              */
	unsigned short nDport;	/* Id              */
	unsigned short nScop;	/* Id              */
	unsigned short nId;	    /* Id              */
	unsigned char  caRev[20];
} ncNsaPatRule;


/* cCom的含义    */
#define NC_NSA_SETQOS       1	/* 设置或更新QOS */
#define NC_NSA_DELQOS       2	/* 删除QOS       */
#define NC_NSA_RESETQOS     3	/* 重置QOS       */
#define NC_NSA_SETSSN       4	/* 设置SSN表     */
#define NC_NSA_DELSSN       5	/* 删除SSN表     */
#define NC_NSA_RESETSSN     6	/* 重置SSN表     */
#define NC_NSA_SETCROSS     7	/* 设置交叉连接  */
#define NC_NSA_SETREDIR     8   /* 端口重定向    */
#define NC_NSA_DELREDIR     9   /* 端口重定向    */
#define NC_NSA_SHOWQOS      10   /* 端口重定向    */
#define NC_NSA_SETSSNMIRROR 11	/* 设置SSN表     */
#define NC_NSA_SHOWSSN      12
#define NC_NSA_SHOWQOSID    13  /* 显示指定QosID的属性  */
#define NC_NSA_SETDEVUP     14  /* 设置网络设备  */
#define NC_NSA_SHOWDEV      15  /* 显示网口状态   */
#define NC_NSA_SETPAT       16  /* 设置关键字     */
#define NC_NSA_COMPPAT      17  /* 编译关键字      */
#define NC_NSA_SETPATRULE   18  /* 设置关键字规则  */
#define NC_NSA_DELPAT       19  /* 设置关键字规则  */
#define NC_NSA_SHOWKEYLOG   20  /* 显示关键字日志  */
#define NC_NSA_SHOWPKG      21  /* 查看数据包状况  */
#define NC_NSA_SYNPKG       22  /* 同步数据包      */


/* 交换内存结构   */
typedef struct ncNsaComBuf_s {
	unsigned char  cCom;	    /* 命令，常见上面定义的宏           */
	unsigned char  cUse;	    /* 0--空闲  1--就绪   2--正在操作   */
	unsigned short nLen;	    /* 记录长度                         */
	unsigned long  lCheckSum;	/* 校验码，检查本记录的有效性       */
	union {
		ncNsaSsnBuf ssn;
		ncNsaQosBuf qos;
		ncNasCrossLink cross;
		ncNsaRedir     redir;
		ncNsaPatBuf    pat;
		ncNsaPatRule   prule;
		char buf[40];
	} s;
} ncNsaComBuf;

typedef struct ncNsaComHead_s {
	int iLock;		            /* 0--空闲  1--正在操作        */
	unsigned long lMax;	        /* 最大记录数                     */
	unsigned long lCur;	        /* 当前命令位置,抓包进程更新的    */
	unsigned long lDo;	        /* 正在处理记录    */
	unsigned long lStartTime;	/* 启动时间        */
	unsigned long lLastTime;	/* 最后修改时间    */
	unsigned char cStartMark;   /* 创建时值为 0, 一旦开始操作后将变为 1  */
	unsigned char cRev[3];
} ncNsaComHead;

typedef struct ncNsaCom_s {
	ncNsaComHead *psHead;
	ncNsaComBuf  *psBuf;
} ncNsaCom;

/* 索引  */
typedef struct ncNsaComBufIndex_s {
	int nsa_id;
	unsigned long lSum;
	unsigned long lPos;
} ncNsaComBufIndex;

/* ACL控制表   */
typedef struct ncNsaAclSsn_s {
	ncNsaSsnBuf ssn;
	struct ncNsaAclSsn_s *next;
} ncNsaAclSsn;

/* 传递指针    */
typedef struct ncNsaDataBuf_s {
    ncNsaComBuf   *psBuf;
    char          *pHead;
} ncNsaDataBuf;

#endif
