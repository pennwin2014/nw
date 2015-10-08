-- 2013/10/29 nwcompute version增加长度 alter table nwcompute modify version char(20);
-- 2013/10/21  dsuser 增加opt varchar(255)   alter table dsuser add opt varchar(255);
--  2013/10/15 增加进程屏幕ID,nwprocscreen
---客户机信息
drop     table if exists nwcompute;
create   table nwcompute (
    compid    int unsigned   primary key,                             -- 计算机ID
    devname   char(32)  default ' ',                                  -- 机器名
    groupid   int unsigned  default 0,                                -- 所属组名
    userid    int unsigned default 0,            		                  --  用户ID
    ip        int unsigned  default 0,                                -- 客户端IP地址
    compname  char(32)    default ' ',                                -- 计算机名
    mac       varchar(18) default ' ',                                -- MAC地址
    os        char(32)   default ' ',                                 -- 操作系统版本
    ie        char(32)   default ' ',                                 -- IE版本 
    addtime   int unsigned,                                           -- 加入时间
    lasttime  int unsigned,                                           -- 最后上线时间
    version   char(20)  default ' ',                                  -- 客户端版本号
    authkey   char(24),                                               -- 验证用Key
    status    int   default 0,                                        -- 状态 0 正常  1 禁用 9 免监控
    htime     int unsigned default 0,                                 -- 硬件更新时间
    stime     int unsigned default 0,                                 -- 软件更新时间
    ctime     int unsigned default 0                                  -- 控制信息更新时间
);
create index        nwcompute_ind0 on nwcompute (devname);
create index        nwcompute_ind1 on nwcompute (mac);
create index        nwcompute_ind2 on nwcompute (compname);
create index        nwcompute_ind3 on nwcompute (ip);
create index        nwcompute_ind4 on nwcompute (groupid);

----部门信息
drop     table if exists nwgroup;
create   table nwgroup (
    gid       int unsigned auto_increment  primary key,    -- 序列号
    gname  char(32),					   -- 部门名称	
    begip  int unsigned ,				   -- 开始IP		
    endip  int unsigned,				   -- 结束IP		
    pid    int unsigned,				        -- 父部门号		
    content char(128) default ''			   -- 部门备注		
);
 insert into nwgroup(gid,gname,begip,endip,pid,content) values(999,'未知部门',0,0,0,'');  

---- 硬件信息
drop     table if exists nwhardware;
create   table nwhardware (
    sid       int unsigned auto_increment  primary key,    -- 序列号
    compid    int unsigned,                                -- 计算机ID
    userid    int unsigned default 0,            	   --  用户ID	
    mac       char(20)  default '',                       --  MAC地址
    dname     char(16),                                    -- 设备名
    num       int,                                         -- 序号
    InfoType   char(24),								   -- 信息类型
    hardType   char(128),								   -- 硬件类型
    deviceDesc char(128),                                  -- 设备描述
    dvalue    int unsigned,                                -- 设备数值
    addtime   int unsigned,                                -- 加入日期
    lasttime  int unsigned                                 -- 最后修改时间
);

create index nwhardware_ind1 on nwhardware (compid,dname);


---- 软件信息
drop     table if exists nwsoftware;
create   table nwsoftware (
    sid       int unsigned auto_increment  primary key,    -- 序列号
    compid    int unsigned,                                -- 计算机ID
    userid    int unsigned default 0,            	   --  用户ID	
    mac       char(20)  default '',                       --  MAC地址
    sname     char(72),                                    -- 软件名称
    demo     char(255),                                    -- 软件备注
    stype     int,                                         -- 类别
    ssize     int unsigned,                                -- 软件大小
    version   char(16),                                    -- 版本
    instime   char(16),                                    -- 安装时间
    addtime   int unsigned,                                -- 加入或修改日期
    lasttime  int unsigned                                 -- 最后修改时间
);

create index nwsoftware_ind1 on nwsoftware (compid);
create unique index nwsoftware_ind2 on nwsoftware (compid,sname);

---- 程序和进程对照表
drop     table if exists nwproctosoft;
create   table nwproctosoft (
    sid       int unsigned auto_increment  primary key,    -- 序列号
    procname  char(64),                                    -- 进程名
    softname  char(64),                                    -- 软件名
    apptype   int unsigned				   -- 程序分类
    
);
create index nwproctosoft_ind1  on nwproctosoft (procname);

---- 进程使用日志 
drop     table if exists nwproclog_201309;
create   table nwproclog_201309 (
    sid       bigint auto_increment  primary key,    -- 序列号
    compid    int unsigned,                                -- 计算机ID
    userid    int unsigned default 0,            	   --  用户ID	
    mac       char(20)  default '',                       --  MAC地址
	  groupid   int unsigned default 0,                  -- 部门Id
    devname   char(32)  default '',                       -- 设备名
    udisp     char(32)  default '',                        -- 用户名
    sip	      int unsigned,                                -- sip
    procname  char(72),                                    -- 进程名称   
    screenid  int unsigned,                                -- 屏幕id	
    mark      char(1),                                     -- a 启动   d 停止
    pid       int,                                         -- 进程ID
    indexid   char(24),					   -- 进程唯一标识
    pindexid  char(24),					   -- 父进程唯一标识	
    stime     int unsigned,                                -- 启动时间
    btime     int unsigned,                                -- 退出时间    
    path      char(256),				   -- 进程完整路径
    mem       int unsigned,                                -- 内存占用
    cpu       int unsigned,                                -- CPU时间
    flags     int default 0,                                -- 用于统计
    upval     int unsigned default 0,                       -- 上行流量 20130917添加
    downval   int unsigned default 0,                       -- 下行流量 20130917添加
    nettime   int unsigned default 0                        --  网络活动时间 20130917添加
);

---- 文件操作日志 （每天一个表）
drop     table if exists nwfilelog_201309;
create   table nwfilelog_201309 (
    sid       bigint  auto_increment  primary key,    -- 序列号
    compid    int unsigned,                                -- 计算机ID
    userid    int unsigned default 0,            	   --  用户ID	
    mac       char(20)  default '',                       --  MAC地址
	  groupid   int unsigned default 0,                  -- 部门Id
    devname   char(32)  default ' ',                       -- 设备名
    udisp     char(32)  default '',                        -- 用户名
    sip	      int unsigned,                                -- sip
    mark      int,					   -- 文件操作类型
    indexid   char(24),					   -- 进程唯一标识
    stime     int unsigned,                                -- 操作时间
    procname  char(72),                                    -- 进程名称
    path      char(256),				   -- 文件完整路径
    content   char(256)				   -- 备注	
);

-- create index nwproclog_ind1  on nwfilelog(mark);
-- create index nwproclog_ind2  on nwfilelog(compid,stime);
#define	PRO_DEVICE_OP_MOUNT			4	//mount
#define	PRO_DEVICE_OP_UNMOUNT		5	//unmount
---- 外设操作日志 （每天一个表）
drop     table if exists nwremovelog_201309;
create   table nwremovelog_201309 (
    sid       bigint auto_increment  primary key,    -- 序列号
    compid    int unsigned,                                -- 计算机ID
    userid    int unsigned default 0,            	   --  用户ID	
    mac       char(20)  default '',                       --  MAC地址
    groupid   int unsigned default 0,                  -- 部门Id
    devname   char(32)  default ' ',                       -- 设备名
    udisp     char(32)  default '',                        -- 用户名
    sip	      int unsigned,                                -- sip
    procname  char(72),                                    -- 进程名称
    mark      char(1),					                           -- 操作类型 4插入，5拔出
    stime     int unsigned,                                -- 操作时间
    valuename  char(72),                                    -- 卷名称
    produname  char(72),                                    -- 产品名称
    hardname  char(72),                                    -- 硬件ID
    driversn  char(2)                                     -- 分配的盘符
);

-- create index nwproclog_ind1  on nwfilelog_201309(mark);
-- create index nwproclog_ind2  on nwfilelog_201309(compid,stime);

drop     table if exists ncimclient_201309;
create   table ncimclient_201309_201309 (
    sid       bigint auto_increment  primary key,
    compid    int unsigned,                                -- 计算机ID
    userid    int unsigned default 0,            	   --  用户ID	
    mac       char(20)  default '',                       --  MAC地址
    groupid   int unsigned default 0,                  -- 部门Id
    devname   char(32)  default ' ',                       -- 设备名
    udisp     char(32)  default '',                        -- 用户名
    sip	      int unsigned,                                -- sip
    procname  char(72),                                    -- 进程名称
    stime     int unsigned,        --  时间      
    service   int,                 --  服务类型
    funcode   int,                 --  服务代码 1.登录 2.退出  3.发送消息 4.发送文件  5.接收文件 6.接收消息
    mfrom     char(48),
    mto       char(48),
    mesg      varchar(255),         --  内容 
    sesid     int unsigned   default 0,      --   会话ID  
    myaccount char(48) default '',           --   本人帐号
    friendaccount char(48) default ''        --   对方帐号  
) max_rows=1000000000 avg_row_length=1500;

-- create index ncimclient_ind1  on ncimclient (stime);
-- create index ncimclient_ind2  on ncimclient (userid);
-- create index ncimclient_ind3  on ncimclient (compid);
-- create index ncimclient_ind4  on ncimclient (sesid);
-- create unique index ncimclient_ind6 on ncimclient(stime,userid,mesg);
-- create unique index nccontlog_ind6 on nccontlog(stime,userid,mesg);

-- 屏幕JPG日志  （每天一个表）
drop table if exists ncscreenlog_201309;
create table ncscreenlog_201309 (
    sid  	    bigint auto_increment  primary key,    -- 序列号
     compid    int unsigned,                                -- 计算机ID
    userid    int unsigned default 0,            	   --  用户ID
	  groupid   int unsigned default 0,                  -- 部门Id
    devname   char(32)  default ' ',                       -- 设备名
    udisp     char(32)  default '',                        -- 用户名
    mac       char(20)  default '',                       --  MAC地址
    sip	      int unsigned,                                -- sip
    stime     int,             -- 采集时间
    file      char(80)         -- 文件名
) max_rows=1000000000 avg_row_length=1500;

-- create index ncscreenlog_index1 on ncscreenlog (stime,userid);

-- 网页操作日志 （每天一个表）
drop     table if exists nwweblog_201309;
create   table nwweblog_201309 (
    sid       bigint auto_increment  primary key,    -- 序列号
     compid    int unsigned,                                -- 计算机ID
    userid    int unsigned default 0,            	   --  用户ID	
    mac       char(20)  default '',                       --  MAC地址
   groupid   int unsigned default 0,                  -- 部门Id
    devname   char(32)  default ' ',                       -- 设备名
    udisp     char(32)  default '',                        -- 用户名
    sip	      int unsigned,                                -- sip
    procname  char(72),                                    -- 进程名称
    stime     int unsigned,                                -- 操作时间
    urlclass  int unsigned,				                            -- 类型
    webaddr   char(128),					                        -- 网址
    subject   char(128),					                        -- 标题
    content   char(128)                                    -- 备注
);
-- create index nwproclog_ind1  on nwweblog(webaddr);
-- create index nwproclog_ind2  on nwweblog(compid,stime);

-- 主策略
drop     table if exists nwmainprocy;
create   table nwmainprocy (
    sid           int unsigned auto_increment  primary key,    -- 序列号
    lawname       char(24),                                    -- 策略名称
    constatus     int,                                         -- 控制状态   0 --离线有效 1-- 离线无效
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                    -- 创建人
    rev1           char(24),                                    -- 保留字段1 
    rev2           char(24),                                     -- 保留字段2
    flag           int default 0                                --  0 正常 8-删除
);

-- 子策略_文件控制
drop     table if exists nwprocyfile;
create   table nwprocyfile (
    sid           int unsigned auto_increment  primary key,     -- 序列号
    pid           int unsigned,                                 -- 关联主策略
    filename       char(128),                                    -- 过滤文件名 空为全部，文件名包含，可以是路径，也可以是文件类型
    optype         int,                                         -- 控制状态   1 --2-- 禁止文件复制到网络 4--禁止文件复制到云盘
    snap           int,                                         --  截屏快照   0--不截屏  2-截屏
    type           int,                                         --  0-离线有效   1-离线无效
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                     -- 创建人
    flag           int default 0                                --  0 正常 8-删除
);

-- 子策略_外设控制
drop     table if exists nwprocydev;
create   table nwprocydev (
    sid           int unsigned auto_increment  primary key,     -- 序列号
    pid           int unsigned,                                 -- 关联主策略
    optype         int,                                         -- 控制状态   1 --禁用U盘 2--  禁止U盘读 4--禁止U盘写 8--禁止光盘刻录
    freehaordid    int,                                         -- 放行指定U盘库ID, 0-不限制
    snap           int,                                         --  截屏快照   0--不截屏  2-截屏
    type           int,                                         --  0-离线有效   1-离线无效
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                     -- 创建人
    flag           int default 0                                --  0 正常 8-删除
);

-- 子策略_非法外联控制策略
drop     table if exists nwprocylink;
create   table nwprocylink (
    sid           int unsigned auto_increment  primary key,     -- 序列号
    pid           int unsigned,                                 -- 关联主策略
    optype         int,                                         -- 控制状态   1 --禁止增加网络连接 2--  禁止wif 4--禁止ADSL 8-- 禁止VPN 16--禁止离线连接网络
    snap           int,                                         --  截屏快照   0--不截屏  2-截屏
    type           int,                                         --  0-离线有效   1-离线无效
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                     -- 创建人
    flag           int default 0                                --  0 正常 8-删除
);


-- 子策略_应用程序控制类别策略
drop     table if exists nwprocypgtype;
create   table nwprocypgtype (
    sid           int unsigned auto_increment  primary key,     -- 序列号
    pid           int unsigned,                                 -- 关联主策略
    gtype         int unsigned,                                 -- 程序类别ID 
    optype         int,                                         -- 控制状态   1 --禁止运行  2-启动截屏
    snap           int,                                         --  截屏快照   0--不截屏  2-截屏
    stamp          int,                                         --  限制运行时间 分钟 0，不限制
    type           int,                                         --  0-离线有效   1-离线无效
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                     -- 创建人
    flag           int default 0                                --  0 正常 8-删除
);

-- 子策略_进程控制策略
drop     table if exists nwprocypname;
create   table nwprocypname (
    sid           int unsigned auto_increment  primary key,     -- 序列号
    pid           int unsigned,                                 -- 关联主策略
    pname         char(64),                                     -- 进程名称
    optype         int,                                         -- 控制状态   1 --禁止运行  2-启动截屏 4-禁止导出文件 8-记录导出文件内容
    stamp          int,                                         --  限制运行时间 分钟 0，不限制
    snap           int,                                         --  截屏快照   0--不截屏  2-截屏
    type           int,                                         --  0-离线有效   1-离线无效
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                     -- 创建人
    flag           int default 0                                --  0 正常 8-删除
);

-- 子策略_网络控制策略
drop     table if exists nwprocynet;
create   table nwprocynet (
    sid           int unsigned auto_increment  primary key,     -- 序列号
    pid           int unsigned,                                 -- 关联主策略
    trantype          int unsigned,                             -- 17 TCP 6 UDP
    sport         int unsigned,                                 -- 起始端口
    eport         int unsigned,                                 -- 终止端口
    optype         int,                                         -- 控制状态   1 --禁止  2-允许
    type           int,                                         --  0-离线有效   1-离线无效
    snap           int,                                         --  截屏快照   0--不截屏  2-截屏
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                     -- 创建人
    flag           int default 0                                --  0 正常 8-删除
);

-- 子策略_网址控制策略
drop     table if exists nwprocyurl;
create   table nwprocyurl (
    sid           int unsigned auto_increment  primary key,     -- 序列号
    pid           int unsigned,                                 -- 关联主策略
    url           char(64),                                     -- url
    urlclass      int unsigned,                                 -- 网址库 0--全部
    optype         int,                                         -- 控制状态   1 --禁止  2-允许
    snap           int,                                         --  截屏快照   0--不截屏  2-截屏
    type           int,                                         --  0-离线有效   1-离线无效
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                     -- 创建人
    flag           int default 0                                --  0 正常 8-删除
);

-- 子策略_其它控制策略
drop     table if exists nwprocyoth;
create   table nwprocyoth (
    sid           int unsigned auto_increment  primary key,     -- 序列号
    pid           int unsigned,                                 -- 关联主策略
    ctype         int,                                          -- 类型 1-屏幕  2-IM监控
    ftime         int unsigned,                                 -- 对于截屏，表示截屏间隔 单位分钟
    optype         int,                                         -- 控制状态   0-停用 对于IM表示记录，1 --启用，对于IM 表示不记录
    type           int,                                         --  0-离线有效   1-离线无效
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                     -- 创建人
    flag           int default 0                                --  0 正常 8-删除
);

-- 子策略_引用其他预定义策略
drop     table if exists nwprocyref;
create   table nwprocyref (
    sid           int unsigned auto_increment  primary key,     -- 序列号
    pid           int unsigned,                                 -- 关联主策略
    othid         int unsigned,                                 -- 引用其他主策略
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                     -- 创建人
    flag           int default 0                                --  0 正常 8-删除
);


--   告警日志
drop     table if exists nwwarnlog_201309;
create   table nwwarnlog_201309 (
    sid       bigint auto_increment  primary key,    -- 序列号
    compid    int unsigned,                                -- 计算机ID
    userid    int unsigned default 0,            	   --  用户ID	
    mac       char(20)  default '',                       --  MAC地址
    groupid   int unsigned default 0,                     -- 部门Id
    devname   char(32)  default ' ',                       -- 设备名
    udisp     char(32)  default '',                        -- 用户名
    sip	      int unsigned,                                -- sip
    stime     int unsigned,                                -- 操作时间
    screenid  int unsigned,                                -- 屏幕ID
    pid       int unsigned,                                -- 触发策略ID
    mesg   char(256)                                      -- 告警描述
);


-- 告警屏幕日志  
drop table if exists nwwarnscreen_201309;
create table nwwarnscreen_201309 (
    sid       int unsigned auto_increment  primary key,    -- 序列号 
    warnid    int unsigned,                                -- 和告警日志的screenid关联
    stime     int,             -- 采集时间
    file      char(80)         -- 文件名                  
) ;

-- 进程屏幕日志  
drop table if exists nwprocscreen_201309;
create table nwprocscreen_201309 (
    sid       int unsigned auto_increment  primary key,    -- 序列号 
    procid    int unsigned,                                -- 和进程日志的screenid关联
    stime     int,             -- 采集时间
    file      char(80)         -- 文件名                  
) ;

--   策略下发对象
drop     table if exists nwprocyobj;
create   table nwprocyobj (
    sid           int unsigned auto_increment  primary key,     -- 序列号
    pid           int unsigned,                                 -- 关联主策略
    ctype         int,                                          -- 类型 0-全部  1-部门 2-计算机
    id            int unsigned,                                 -- 0 全部 按照ctype 可以是部门ID,也可以是计算机ID
    stime         int unsigned,                                 -- 起始时间 格式080000  表示8时0分0秒 暂时不用了
    etime         int unsigned,                                 -- 终止时间 格式235959  表示23时59分59秒 暂时不用了
    dataid        int,                                          -- 例外时间对象ID  比如节假日 0,表示没有例外
    createtime     int unsigned,                                -- 创建时间
    modtime        int unsigned,                                -- 修改时间
    createuser     char(24),                                     -- 创建人
    flag           int default 0                                --  0 正常 8-删除
);

--  时间对象描述  
drop table if exists  nwdatedes;
create table nwdatedes (
    id        int  primary key,
    name       char(32),
    modtime        int unsigned                                -- 修改时间
 );
drop table  if exists nwlimdate;
create table nwlimdate (
    sid        int auto_increment  primary key,  -- 记录的序列号  
    id         int,                              -- 日期对象ID  
    ltype      int,                              -- 类型 1--Day 2--Week 3--Month 4--Year      
    llower     int unsigned,                     -- 日期下限 1-YYYYMMDD 2--0/6 3--1/31 4--MMDD 
    lupper     int unsigned,
    lltime     int unsigned,                     --  每日上限 HHMISS 
    lutime     int unsigned,                     --  每日下限 HHMISS
    modtime        int unsigned                  -- 修改时间
);
create index nwlimdate_index1 on nclimdate(id);


--  软件类型定义
create table nwsoftclass(
    id        int auto_increment  primary key,  -- 类型ID  
    name       char(64),                        -- 软件类别名称
    modtime        int unsigned default 0       -- 修改时间
 );
insert into nwsoftclass(id,name) values(1,'视频软件');
insert into nwsoftclass(id,name) values(2,'聊天工具');
insert into nwsoftclass(id,name) values(3,'浏览器');
insert into nwsoftclass(id,name) values(4,'游戏娱乐');
insert into nwsoftclass(id,name) values(5,'网络游戏');
insert into nwsoftclass(id,name) values(6,'音乐软件');
insert into nwsoftclass(id,name) values(7,'安全杀毒');
insert into nwsoftclass(id,name) values(8,'系统工具');
insert into nwsoftclass(id,name) values(9,'下载工具');
insert into nwsoftclass(id,name) values(10,'办公软件');
insert into nwsoftclass(id,name) values(11,'手机数码');
insert into nwsoftclass(id,name) values(12,'输入法');
insert into nwsoftclass(id,name) values(13,'图形图像');
insert into nwsoftclass(id,name) values(14,'股票网银');
insert into nwsoftclass(id,name) values(15,'阅读翻译');
insert into nwsoftclass(id,name) values(16,'网络应用');
insert into nwsoftclass(id,name) values(17,'主题壁纸');
insert into nwsoftclass(id,name) values(18,'教育学习');
insert into nwsoftclass(id,name) values(19,'压缩刻录');
insert into nwsoftclass(id,name) values(20,'主题壁纸');
insert into nwsoftclass(id,name) values(21,'编程开发');
insert into nwsoftclass(id,name) values(22,'行业软件');
insert into nwsoftclass(id,name) values(23,'刻录软件');
insert into nwsoftclass(id,name) values(24,'其它软件');
-- 软件类别明细
create table nwsoftlist(
    sid        int auto_increment  primary key,  -- 软件ID 
    id         int,                              -- 软件类型 
    name       char(64),                         -- 软件名称
    pname      char(64),                         -- 进程名
    modtime        int unsigned default 0                               -- 修改时间
 );

--  USB库类别
drop table if exists  nwusbclass;
create table nwusbclass(
    id        int primary key auto_increment,  -- 类型ID  
    name       char(64),                        -- USB库类别名称
    modtime        int unsigned default 0                               -- 修改时间
 );
ALTER TABLE nwusbclass  AUTO_INCREMENT = 2;
insert into nwusbclass (id,name,modtime) values(0,'未定义',0);
-- USB库明细
create table nwusblist(
    sid        int auto_increment  primary key,  -- 记录ID 
    id         int,                              -- USB类型 
    name       char(24),                         -- 显示名称
    produname  char(24),                         -- 产品名称
    volname    char(24)，                        -- 卷名
    hardid     char(128),                         -- 硬件ID
    binduser   char(24),                         -- 备注
    modtime        int unsigned default 0，                              -- 修改时间
    createuser     char(24)                      --  修改人
 );

---- 服务信息
drop     table if exists nwservice;
create   table nwservice (
    sid       int unsigned auto_increment  primary key,    -- 序列号
    compid    int unsigned,                                -- 计算机ID
    userid    int unsigned default 0,            	  --  用户ID	
    mac       char(20)  default '',                        --  MAC地址
    display   char(72),                                    -- 显示名称
    InfoType     char(24),                                 -- 信息类型
    sname     char(72),                                    -- 服务名称
    path      char(255),                                   -- 路径
    demo      char(255),                                   -- 服务描述
    status         char(20),                                                 -- 状态
    stype     char(20),                                    -- 启动类型
    log     char(20),                                	  -- 登录为
    addtime   int unsigned,                                -- 加入或修改日期
    lasttime  int unsigned                                 -- 最后修改时间
);

drop table  if exists  ncworktime;
create table ncworktime (
    sid         int  auto_increment primary key,
    timetype    int,               --       1-某日  2-每周 3-每月  4-每年
    llower      int unsigned,      -- 日期下限 1-YYYYMMDD 2--0/6 3--1/31 4--MMDD 
    lupper      int unsigned,
    starttime   int,               --       开始时间 hhmmss
    stoptime    int,               --       结束时间
    flags       smallint           -- 0--上班时间 1--不上班时间 
);

insert into ncworktime (sid,timetype,llower,lupper,starttime,stoptime,flags) values(1,2,1,5,80000,115959,0);
insert into ncworktime (sid,timetype,llower,lupper,starttime,stoptime,flags) values(2,2,1,5,130000,165959,0);
insert into ncworktime (sid,timetype,llower,lupper,starttime,stoptime,flags) values(3,4,501,502,0,235959,1);
insert into ncworktime (sid,timetype,llower,lupper,starttime,stoptime,flags) values(4,4,1001,1007,0,235959,1);


-- 升级软件包
drop     table if exists ncupsoftlist; 
create   table ncupsoftlist (
    sid        int unsigned auto_increment  primary key,    -- 序列号
    softname   char(32),                                    -- 软件名称
    filename   char(128),            	                      --  文件名
    version    char(16)  default '',                        --  版本
    filesize    int(10),                                    -- 文件大小
    note        char(255),                                  -- 软件说明
    opertor     char(32),                                   -- 上传人
    uploadtime  int(10),                                    -- 上传时间
    status      int(10),                                    -- 状态 0-可用 1-不可用
    flags       int(10),                                     -- 0-对全部有效 1-对XP 32位win7有效 2-对win7、8(64位） 有效  3-win8(32)
    lcount      int(10),                                    -- 下载次数
    md5         char(32),                                	      -- MD5值H
    lasttime  int unsigned                                 -- 最后修改时间
);
-- 软件升级下发
drop     table if exists ncupsoftprocy;
create   table ncupsoftprocy (
    sid        int unsigned auto_increment  primary key,    -- 序列号
    softid     int unsigned,                                    -- 软件ID和ncupsoftlist中的sid对应
    objid      int unsigned,            	                      --  对象ID 对于全部  id为0
    objtype    int unsigned default 0,                         --  对象类型 0，全部 1-部门 2-计算机
    lasttime  int unsigned                                 -- 最后修改时间
);

-- 软件升级日志
drop     table if exists ncupsoftlog;
create   table ncupsoftlog (
    sid        int unsigned auto_increment  primary key,        -- 序列号
    softid     int unsigned,                                    -- 软件ID和ncupsoftlist中的sid对应
    compid     int unsigned,            	                      --  软件id
    stime      int unsigned default 0,                          --  对象类型升级时间
    status     int unsigned                                     -- 升级状态   1-成功 2-不成功
);