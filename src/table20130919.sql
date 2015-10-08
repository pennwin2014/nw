-- 2013/10/29 nwcompute version���ӳ��� alter table nwcompute modify version char(20);
-- 2013/10/21  dsuser ����opt varchar(255)   alter table dsuser add opt varchar(255);
--  2013/10/15 ���ӽ�����ĻID,nwprocscreen
---�ͻ�����Ϣ
drop     table if exists nwcompute;
create   table nwcompute (
    compid    int unsigned   primary key,                             -- �����ID
    devname   char(32)  default ' ',                                  -- ������
    groupid   int unsigned  default 0,                                -- ��������
    userid    int unsigned default 0,            		                  --  �û�ID
    ip        int unsigned  default 0,                                -- �ͻ���IP��ַ
    compname  char(32)    default ' ',                                -- �������
    mac       varchar(18) default ' ',                                -- MAC��ַ
    os        char(32)   default ' ',                                 -- ����ϵͳ�汾
    ie        char(32)   default ' ',                                 -- IE�汾 
    addtime   int unsigned,                                           -- ����ʱ��
    lasttime  int unsigned,                                           -- �������ʱ��
    version   char(20)  default ' ',                                  -- �ͻ��˰汾��
    authkey   char(24),                                               -- ��֤��Key
    status    int   default 0,                                        -- ״̬ 0 ����  1 ���� 9 ����
    htime     int unsigned default 0,                                 -- Ӳ������ʱ��
    stime     int unsigned default 0,                                 -- �������ʱ��
    ctime     int unsigned default 0                                  -- ������Ϣ����ʱ��
);
create index        nwcompute_ind0 on nwcompute (devname);
create index        nwcompute_ind1 on nwcompute (mac);
create index        nwcompute_ind2 on nwcompute (compname);
create index        nwcompute_ind3 on nwcompute (ip);
create index        nwcompute_ind4 on nwcompute (groupid);

----������Ϣ
drop     table if exists nwgroup;
create   table nwgroup (
    gid       int unsigned auto_increment  primary key,    -- ���к�
    gname  char(32),					   -- ��������	
    begip  int unsigned ,				   -- ��ʼIP		
    endip  int unsigned,				   -- ����IP		
    pid    int unsigned,				        -- �����ź�		
    content char(128) default ''			   -- ���ű�ע		
);
 insert into nwgroup(gid,gname,begip,endip,pid,content) values(999,'δ֪����',0,0,0,'');  

---- Ӳ����Ϣ
drop     table if exists nwhardware;
create   table nwhardware (
    sid       int unsigned auto_increment  primary key,    -- ���к�
    compid    int unsigned,                                -- �����ID
    userid    int unsigned default 0,            	   --  �û�ID	
    mac       char(20)  default '',                       --  MAC��ַ
    dname     char(16),                                    -- �豸��
    num       int,                                         -- ���
    InfoType   char(24),								   -- ��Ϣ����
    hardType   char(128),								   -- Ӳ������
    deviceDesc char(128),                                  -- �豸����
    dvalue    int unsigned,                                -- �豸��ֵ
    addtime   int unsigned,                                -- ��������
    lasttime  int unsigned                                 -- ����޸�ʱ��
);

create index nwhardware_ind1 on nwhardware (compid,dname);


---- �����Ϣ
drop     table if exists nwsoftware;
create   table nwsoftware (
    sid       int unsigned auto_increment  primary key,    -- ���к�
    compid    int unsigned,                                -- �����ID
    userid    int unsigned default 0,            	   --  �û�ID	
    mac       char(20)  default '',                       --  MAC��ַ
    sname     char(72),                                    -- �������
    demo     char(255),                                    -- �����ע
    stype     int,                                         -- ���
    ssize     int unsigned,                                -- �����С
    version   char(16),                                    -- �汾
    instime   char(16),                                    -- ��װʱ��
    addtime   int unsigned,                                -- ������޸�����
    lasttime  int unsigned                                 -- ����޸�ʱ��
);

create index nwsoftware_ind1 on nwsoftware (compid);
create unique index nwsoftware_ind2 on nwsoftware (compid,sname);

---- ����ͽ��̶��ձ�
drop     table if exists nwproctosoft;
create   table nwproctosoft (
    sid       int unsigned auto_increment  primary key,    -- ���к�
    procname  char(64),                                    -- ������
    softname  char(64),                                    -- �����
    apptype   int unsigned				   -- �������
    
);
create index nwproctosoft_ind1  on nwproctosoft (procname);

---- ����ʹ����־ 
drop     table if exists nwproclog_201309;
create   table nwproclog_201309 (
    sid       bigint auto_increment  primary key,    -- ���к�
    compid    int unsigned,                                -- �����ID
    userid    int unsigned default 0,            	   --  �û�ID	
    mac       char(20)  default '',                       --  MAC��ַ
	  groupid   int unsigned default 0,                  -- ����Id
    devname   char(32)  default '',                       -- �豸��
    udisp     char(32)  default '',                        -- �û���
    sip	      int unsigned,                                -- sip
    procname  char(72),                                    -- ��������   
    screenid  int unsigned,                                -- ��Ļid	
    mark      char(1),                                     -- a ����   d ֹͣ
    pid       int,                                         -- ����ID
    indexid   char(24),					   -- ����Ψһ��ʶ
    pindexid  char(24),					   -- ������Ψһ��ʶ	
    stime     int unsigned,                                -- ����ʱ��
    btime     int unsigned,                                -- �˳�ʱ��    
    path      char(256),				   -- ��������·��
    mem       int unsigned,                                -- �ڴ�ռ��
    cpu       int unsigned,                                -- CPUʱ��
    flags     int default 0,                                -- ����ͳ��
    upval     int unsigned default 0,                       -- �������� 20130917���
    downval   int unsigned default 0,                       -- �������� 20130917���
    nettime   int unsigned default 0                        --  ����ʱ�� 20130917���
);

---- �ļ�������־ ��ÿ��һ����
drop     table if exists nwfilelog_201309;
create   table nwfilelog_201309 (
    sid       bigint  auto_increment  primary key,    -- ���к�
    compid    int unsigned,                                -- �����ID
    userid    int unsigned default 0,            	   --  �û�ID	
    mac       char(20)  default '',                       --  MAC��ַ
	  groupid   int unsigned default 0,                  -- ����Id
    devname   char(32)  default ' ',                       -- �豸��
    udisp     char(32)  default '',                        -- �û���
    sip	      int unsigned,                                -- sip
    mark      int,					   -- �ļ���������
    indexid   char(24),					   -- ����Ψһ��ʶ
    stime     int unsigned,                                -- ����ʱ��
    procname  char(72),                                    -- ��������
    path      char(256),				   -- �ļ�����·��
    content   char(256)				   -- ��ע	
);

-- create index nwproclog_ind1  on nwfilelog(mark);
-- create index nwproclog_ind2  on nwfilelog(compid,stime);
#define	PRO_DEVICE_OP_MOUNT			4	//mount
#define	PRO_DEVICE_OP_UNMOUNT		5	//unmount
---- ���������־ ��ÿ��һ����
drop     table if exists nwremovelog_201309;
create   table nwremovelog_201309 (
    sid       bigint auto_increment  primary key,    -- ���к�
    compid    int unsigned,                                -- �����ID
    userid    int unsigned default 0,            	   --  �û�ID	
    mac       char(20)  default '',                       --  MAC��ַ
    groupid   int unsigned default 0,                  -- ����Id
    devname   char(32)  default ' ',                       -- �豸��
    udisp     char(32)  default '',                        -- �û���
    sip	      int unsigned,                                -- sip
    procname  char(72),                                    -- ��������
    mark      char(1),					                           -- �������� 4���룬5�γ�
    stime     int unsigned,                                -- ����ʱ��
    valuename  char(72),                                    -- ������
    produname  char(72),                                    -- ��Ʒ����
    hardname  char(72),                                    -- Ӳ��ID
    driversn  char(2)                                     -- ������̷�
);

-- create index nwproclog_ind1  on nwfilelog_201309(mark);
-- create index nwproclog_ind2  on nwfilelog_201309(compid,stime);

drop     table if exists ncimclient_201309;
create   table ncimclient_201309_201309 (
    sid       bigint auto_increment  primary key,
    compid    int unsigned,                                -- �����ID
    userid    int unsigned default 0,            	   --  �û�ID	
    mac       char(20)  default '',                       --  MAC��ַ
    groupid   int unsigned default 0,                  -- ����Id
    devname   char(32)  default ' ',                       -- �豸��
    udisp     char(32)  default '',                        -- �û���
    sip	      int unsigned,                                -- sip
    procname  char(72),                                    -- ��������
    stime     int unsigned,        --  ʱ��      
    service   int,                 --  ��������
    funcode   int,                 --  ������� 1.��¼ 2.�˳�  3.������Ϣ 4.�����ļ�  5.�����ļ� 6.������Ϣ
    mfrom     char(48),
    mto       char(48),
    mesg      varchar(255),         --  ���� 
    sesid     int unsigned   default 0,      --   �ỰID  
    myaccount char(48) default '',           --   �����ʺ�
    friendaccount char(48) default ''        --   �Է��ʺ�  
) max_rows=1000000000 avg_row_length=1500;

-- create index ncimclient_ind1  on ncimclient (stime);
-- create index ncimclient_ind2  on ncimclient (userid);
-- create index ncimclient_ind3  on ncimclient (compid);
-- create index ncimclient_ind4  on ncimclient (sesid);
-- create unique index ncimclient_ind6 on ncimclient(stime,userid,mesg);
-- create unique index nccontlog_ind6 on nccontlog(stime,userid,mesg);

-- ��ĻJPG��־  ��ÿ��һ����
drop table if exists ncscreenlog_201309;
create table ncscreenlog_201309 (
    sid  	    bigint auto_increment  primary key,    -- ���к�
     compid    int unsigned,                                -- �����ID
    userid    int unsigned default 0,            	   --  �û�ID
	  groupid   int unsigned default 0,                  -- ����Id
    devname   char(32)  default ' ',                       -- �豸��
    udisp     char(32)  default '',                        -- �û���
    mac       char(20)  default '',                       --  MAC��ַ
    sip	      int unsigned,                                -- sip
    stime     int,             -- �ɼ�ʱ��
    file      char(80)         -- �ļ���
) max_rows=1000000000 avg_row_length=1500;

-- create index ncscreenlog_index1 on ncscreenlog (stime,userid);

-- ��ҳ������־ ��ÿ��һ����
drop     table if exists nwweblog_201309;
create   table nwweblog_201309 (
    sid       bigint auto_increment  primary key,    -- ���к�
     compid    int unsigned,                                -- �����ID
    userid    int unsigned default 0,            	   --  �û�ID	
    mac       char(20)  default '',                       --  MAC��ַ
   groupid   int unsigned default 0,                  -- ����Id
    devname   char(32)  default ' ',                       -- �豸��
    udisp     char(32)  default '',                        -- �û���
    sip	      int unsigned,                                -- sip
    procname  char(72),                                    -- ��������
    stime     int unsigned,                                -- ����ʱ��
    urlclass  int unsigned,				                            -- ����
    webaddr   char(128),					                        -- ��ַ
    subject   char(128),					                        -- ����
    content   char(128)                                    -- ��ע
);
-- create index nwproclog_ind1  on nwweblog(webaddr);
-- create index nwproclog_ind2  on nwweblog(compid,stime);

-- ������
drop     table if exists nwmainprocy;
create   table nwmainprocy (
    sid           int unsigned auto_increment  primary key,    -- ���к�
    lawname       char(24),                                    -- ��������
    constatus     int,                                         -- ����״̬   0 --������Ч 1-- ������Ч
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                    -- ������
    rev1           char(24),                                    -- �����ֶ�1 
    rev2           char(24),                                     -- �����ֶ�2
    flag           int default 0                                --  0 ���� 8-ɾ��
);

-- �Ӳ���_�ļ�����
drop     table if exists nwprocyfile;
create   table nwprocyfile (
    sid           int unsigned auto_increment  primary key,     -- ���к�
    pid           int unsigned,                                 -- ����������
    filename       char(128),                                    -- �����ļ��� ��Ϊȫ�����ļ���������������·����Ҳ�������ļ�����
    optype         int,                                         -- ����״̬   1 --2-- ��ֹ�ļ����Ƶ����� 4--��ֹ�ļ����Ƶ�����
    snap           int,                                         --  ��������   0--������  2-����
    type           int,                                         --  0-������Ч   1-������Ч
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                     -- ������
    flag           int default 0                                --  0 ���� 8-ɾ��
);

-- �Ӳ���_�������
drop     table if exists nwprocydev;
create   table nwprocydev (
    sid           int unsigned auto_increment  primary key,     -- ���к�
    pid           int unsigned,                                 -- ����������
    optype         int,                                         -- ����״̬   1 --����U�� 2--  ��ֹU�̶� 4--��ֹU��д 8--��ֹ���̿�¼
    freehaordid    int,                                         -- ����ָ��U�̿�ID, 0-������
    snap           int,                                         --  ��������   0--������  2-����
    type           int,                                         --  0-������Ч   1-������Ч
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                     -- ������
    flag           int default 0                                --  0 ���� 8-ɾ��
);

-- �Ӳ���_�Ƿ��������Ʋ���
drop     table if exists nwprocylink;
create   table nwprocylink (
    sid           int unsigned auto_increment  primary key,     -- ���к�
    pid           int unsigned,                                 -- ����������
    optype         int,                                         -- ����״̬   1 --��ֹ������������ 2--  ��ֹwif 4--��ֹADSL 8-- ��ֹVPN 16--��ֹ������������
    snap           int,                                         --  ��������   0--������  2-����
    type           int,                                         --  0-������Ч   1-������Ч
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                     -- ������
    flag           int default 0                                --  0 ���� 8-ɾ��
);


-- �Ӳ���_Ӧ�ó������������
drop     table if exists nwprocypgtype;
create   table nwprocypgtype (
    sid           int unsigned auto_increment  primary key,     -- ���к�
    pid           int unsigned,                                 -- ����������
    gtype         int unsigned,                                 -- �������ID 
    optype         int,                                         -- ����״̬   1 --��ֹ����  2-��������
    snap           int,                                         --  ��������   0--������  2-����
    stamp          int,                                         --  ��������ʱ�� ���� 0��������
    type           int,                                         --  0-������Ч   1-������Ч
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                     -- ������
    flag           int default 0                                --  0 ���� 8-ɾ��
);

-- �Ӳ���_���̿��Ʋ���
drop     table if exists nwprocypname;
create   table nwprocypname (
    sid           int unsigned auto_increment  primary key,     -- ���к�
    pid           int unsigned,                                 -- ����������
    pname         char(64),                                     -- ��������
    optype         int,                                         -- ����״̬   1 --��ֹ����  2-�������� 4-��ֹ�����ļ� 8-��¼�����ļ�����
    stamp          int,                                         --  ��������ʱ�� ���� 0��������
    snap           int,                                         --  ��������   0--������  2-����
    type           int,                                         --  0-������Ч   1-������Ч
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                     -- ������
    flag           int default 0                                --  0 ���� 8-ɾ��
);

-- �Ӳ���_������Ʋ���
drop     table if exists nwprocynet;
create   table nwprocynet (
    sid           int unsigned auto_increment  primary key,     -- ���к�
    pid           int unsigned,                                 -- ����������
    trantype          int unsigned,                             -- 17 TCP 6 UDP
    sport         int unsigned,                                 -- ��ʼ�˿�
    eport         int unsigned,                                 -- ��ֹ�˿�
    optype         int,                                         -- ����״̬   1 --��ֹ  2-����
    type           int,                                         --  0-������Ч   1-������Ч
    snap           int,                                         --  ��������   0--������  2-����
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                     -- ������
    flag           int default 0                                --  0 ���� 8-ɾ��
);

-- �Ӳ���_��ַ���Ʋ���
drop     table if exists nwprocyurl;
create   table nwprocyurl (
    sid           int unsigned auto_increment  primary key,     -- ���к�
    pid           int unsigned,                                 -- ����������
    url           char(64),                                     -- url
    urlclass      int unsigned,                                 -- ��ַ�� 0--ȫ��
    optype         int,                                         -- ����״̬   1 --��ֹ  2-����
    snap           int,                                         --  ��������   0--������  2-����
    type           int,                                         --  0-������Ч   1-������Ч
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                     -- ������
    flag           int default 0                                --  0 ���� 8-ɾ��
);

-- �Ӳ���_�������Ʋ���
drop     table if exists nwprocyoth;
create   table nwprocyoth (
    sid           int unsigned auto_increment  primary key,     -- ���к�
    pid           int unsigned,                                 -- ����������
    ctype         int,                                          -- ���� 1-��Ļ  2-IM���
    ftime         int unsigned,                                 -- ���ڽ�������ʾ������� ��λ����
    optype         int,                                         -- ����״̬   0-ͣ�� ����IM��ʾ��¼��1 --���ã�����IM ��ʾ����¼
    type           int,                                         --  0-������Ч   1-������Ч
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                     -- ������
    flag           int default 0                                --  0 ���� 8-ɾ��
);

-- �Ӳ���_��������Ԥ�������
drop     table if exists nwprocyref;
create   table nwprocyref (
    sid           int unsigned auto_increment  primary key,     -- ���к�
    pid           int unsigned,                                 -- ����������
    othid         int unsigned,                                 -- ��������������
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                     -- ������
    flag           int default 0                                --  0 ���� 8-ɾ��
);


--   �澯��־
drop     table if exists nwwarnlog_201309;
create   table nwwarnlog_201309 (
    sid       bigint auto_increment  primary key,    -- ���к�
    compid    int unsigned,                                -- �����ID
    userid    int unsigned default 0,            	   --  �û�ID	
    mac       char(20)  default '',                       --  MAC��ַ
    groupid   int unsigned default 0,                     -- ����Id
    devname   char(32)  default ' ',                       -- �豸��
    udisp     char(32)  default '',                        -- �û���
    sip	      int unsigned,                                -- sip
    stime     int unsigned,                                -- ����ʱ��
    screenid  int unsigned,                                -- ��ĻID
    pid       int unsigned,                                -- ��������ID
    mesg   char(256)                                      -- �澯����
);


-- �澯��Ļ��־  
drop table if exists nwwarnscreen_201309;
create table nwwarnscreen_201309 (
    sid       int unsigned auto_increment  primary key,    -- ���к� 
    warnid    int unsigned,                                -- �͸澯��־��screenid����
    stime     int,             -- �ɼ�ʱ��
    file      char(80)         -- �ļ���                  
) ;

-- ������Ļ��־  
drop table if exists nwprocscreen_201309;
create table nwprocscreen_201309 (
    sid       int unsigned auto_increment  primary key,    -- ���к� 
    procid    int unsigned,                                -- �ͽ�����־��screenid����
    stime     int,             -- �ɼ�ʱ��
    file      char(80)         -- �ļ���                  
) ;

--   �����·�����
drop     table if exists nwprocyobj;
create   table nwprocyobj (
    sid           int unsigned auto_increment  primary key,     -- ���к�
    pid           int unsigned,                                 -- ����������
    ctype         int,                                          -- ���� 0-ȫ��  1-���� 2-�����
    id            int unsigned,                                 -- 0 ȫ�� ����ctype �����ǲ���ID,Ҳ�����Ǽ����ID
    stime         int unsigned,                                 -- ��ʼʱ�� ��ʽ080000  ��ʾ8ʱ0��0�� ��ʱ������
    etime         int unsigned,                                 -- ��ֹʱ�� ��ʽ235959  ��ʾ23ʱ59��59�� ��ʱ������
    dataid        int,                                          -- ����ʱ�����ID  ����ڼ��� 0,��ʾû������
    createtime     int unsigned,                                -- ����ʱ��
    modtime        int unsigned,                                -- �޸�ʱ��
    createuser     char(24),                                     -- ������
    flag           int default 0                                --  0 ���� 8-ɾ��
);

--  ʱ���������  
drop table if exists  nwdatedes;
create table nwdatedes (
    id        int  primary key,
    name       char(32),
    modtime        int unsigned                                -- �޸�ʱ��
 );
drop table  if exists nwlimdate;
create table nwlimdate (
    sid        int auto_increment  primary key,  -- ��¼�����к�  
    id         int,                              -- ���ڶ���ID  
    ltype      int,                              -- ���� 1--Day 2--Week 3--Month 4--Year      
    llower     int unsigned,                     -- �������� 1-YYYYMMDD 2--0/6 3--1/31 4--MMDD 
    lupper     int unsigned,
    lltime     int unsigned,                     --  ÿ������ HHMISS 
    lutime     int unsigned,                     --  ÿ������ HHMISS
    modtime        int unsigned                  -- �޸�ʱ��
);
create index nwlimdate_index1 on nclimdate(id);


--  ������Ͷ���
create table nwsoftclass(
    id        int auto_increment  primary key,  -- ����ID  
    name       char(64),                        -- ����������
    modtime        int unsigned default 0       -- �޸�ʱ��
 );
insert into nwsoftclass(id,name) values(1,'��Ƶ���');
insert into nwsoftclass(id,name) values(2,'���칤��');
insert into nwsoftclass(id,name) values(3,'�����');
insert into nwsoftclass(id,name) values(4,'��Ϸ����');
insert into nwsoftclass(id,name) values(5,'������Ϸ');
insert into nwsoftclass(id,name) values(6,'�������');
insert into nwsoftclass(id,name) values(7,'��ȫɱ��');
insert into nwsoftclass(id,name) values(8,'ϵͳ����');
insert into nwsoftclass(id,name) values(9,'���ع���');
insert into nwsoftclass(id,name) values(10,'�칫���');
insert into nwsoftclass(id,name) values(11,'�ֻ�����');
insert into nwsoftclass(id,name) values(12,'���뷨');
insert into nwsoftclass(id,name) values(13,'ͼ��ͼ��');
insert into nwsoftclass(id,name) values(14,'��Ʊ����');
insert into nwsoftclass(id,name) values(15,'�Ķ�����');
insert into nwsoftclass(id,name) values(16,'����Ӧ��');
insert into nwsoftclass(id,name) values(17,'�����ֽ');
insert into nwsoftclass(id,name) values(18,'����ѧϰ');
insert into nwsoftclass(id,name) values(19,'ѹ����¼');
insert into nwsoftclass(id,name) values(20,'�����ֽ');
insert into nwsoftclass(id,name) values(21,'��̿���');
insert into nwsoftclass(id,name) values(22,'��ҵ���');
insert into nwsoftclass(id,name) values(23,'��¼���');
insert into nwsoftclass(id,name) values(24,'�������');
-- ��������ϸ
create table nwsoftlist(
    sid        int auto_increment  primary key,  -- ���ID 
    id         int,                              -- ������� 
    name       char(64),                         -- �������
    pname      char(64),                         -- ������
    modtime        int unsigned default 0                               -- �޸�ʱ��
 );

--  USB�����
drop table if exists  nwusbclass;
create table nwusbclass(
    id        int primary key auto_increment,  -- ����ID  
    name       char(64),                        -- USB���������
    modtime        int unsigned default 0                               -- �޸�ʱ��
 );
ALTER TABLE nwusbclass  AUTO_INCREMENT = 2;
insert into nwusbclass (id,name,modtime) values(0,'δ����',0);
-- USB����ϸ
create table nwusblist(
    sid        int auto_increment  primary key,  -- ��¼ID 
    id         int,                              -- USB���� 
    name       char(24),                         -- ��ʾ����
    produname  char(24),                         -- ��Ʒ����
    volname    char(24)��                        -- ����
    hardid     char(128),                         -- Ӳ��ID
    binduser   char(24),                         -- ��ע
    modtime        int unsigned default 0��                              -- �޸�ʱ��
    createuser     char(24)                      --  �޸���
 );

---- ������Ϣ
drop     table if exists nwservice;
create   table nwservice (
    sid       int unsigned auto_increment  primary key,    -- ���к�
    compid    int unsigned,                                -- �����ID
    userid    int unsigned default 0,            	  --  �û�ID	
    mac       char(20)  default '',                        --  MAC��ַ
    display   char(72),                                    -- ��ʾ����
    InfoType     char(24),                                 -- ��Ϣ����
    sname     char(72),                                    -- ��������
    path      char(255),                                   -- ·��
    demo      char(255),                                   -- ��������
    status         char(20),                                                 -- ״̬
    stype     char(20),                                    -- ��������
    log     char(20),                                	  -- ��¼Ϊ
    addtime   int unsigned,                                -- ������޸�����
    lasttime  int unsigned                                 -- ����޸�ʱ��
);

drop table  if exists  ncworktime;
create table ncworktime (
    sid         int  auto_increment primary key,
    timetype    int,               --       1-ĳ��  2-ÿ�� 3-ÿ��  4-ÿ��
    llower      int unsigned,      -- �������� 1-YYYYMMDD 2--0/6 3--1/31 4--MMDD 
    lupper      int unsigned,
    starttime   int,               --       ��ʼʱ�� hhmmss
    stoptime    int,               --       ����ʱ��
    flags       smallint           -- 0--�ϰ�ʱ�� 1--���ϰ�ʱ�� 
);

insert into ncworktime (sid,timetype,llower,lupper,starttime,stoptime,flags) values(1,2,1,5,80000,115959,0);
insert into ncworktime (sid,timetype,llower,lupper,starttime,stoptime,flags) values(2,2,1,5,130000,165959,0);
insert into ncworktime (sid,timetype,llower,lupper,starttime,stoptime,flags) values(3,4,501,502,0,235959,1);
insert into ncworktime (sid,timetype,llower,lupper,starttime,stoptime,flags) values(4,4,1001,1007,0,235959,1);


-- ���������
drop     table if exists ncupsoftlist; 
create   table ncupsoftlist (
    sid        int unsigned auto_increment  primary key,    -- ���к�
    softname   char(32),                                    -- �������
    filename   char(128),            	                      --  �ļ���
    version    char(16)  default '',                        --  �汾
    filesize    int(10),                                    -- �ļ���С
    note        char(255),                                  -- ���˵��
    opertor     char(32),                                   -- �ϴ���
    uploadtime  int(10),                                    -- �ϴ�ʱ��
    status      int(10),                                    -- ״̬ 0-���� 1-������
    flags       int(10),                                     -- 0-��ȫ����Ч 1-��XP 32λwin7��Ч 2-��win7��8(64λ�� ��Ч  3-win8(32)
    lcount      int(10),                                    -- ���ش���
    md5         char(32),                                	      -- MD5ֵH
    lasttime  int unsigned                                 -- ����޸�ʱ��
);
-- ��������·�
drop     table if exists ncupsoftprocy;
create   table ncupsoftprocy (
    sid        int unsigned auto_increment  primary key,    -- ���к�
    softid     int unsigned,                                    -- ���ID��ncupsoftlist�е�sid��Ӧ
    objid      int unsigned,            	                      --  ����ID ����ȫ��  idΪ0
    objtype    int unsigned default 0,                         --  �������� 0��ȫ�� 1-���� 2-�����
    lasttime  int unsigned                                 -- ����޸�ʱ��
);

-- ���������־
drop     table if exists ncupsoftlog;
create   table ncupsoftlog (
    sid        int unsigned auto_increment  primary key,        -- ���к�
    softid     int unsigned,                                    -- ���ID��ncupsoftlist�е�sid��Ӧ
    compid     int unsigned,            	                      --  ���id
    stime      int unsigned default 0,                          --  ������������ʱ��
    status     int unsigned                                     -- ����״̬   1-�ɹ� 2-���ɹ�
);