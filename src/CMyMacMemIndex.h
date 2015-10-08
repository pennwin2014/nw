#if!defined CMYMACMEMINDEX88345834588
#define CMYMACMEMINDEX88345834588

#include <stdio.h>
#include <time.h>
#include "string.h"
#include "utoall.h"
#include "ncdef.h"
#include "nwdef.h"
#include "pasdb.h"

typedef struct _mac_mem{
	struct _mac_mem* prv;
	struct _mac_mem* next;
	char   compid[16];
    char   compname[32];
	int    userid;
	char   Username[16];
	char   ip[4];
	char   mac[18];
	char   os[32];
	char   version[20];
    int    lasttime;
	int	   tolibtime; //上次入库时间，超过配置时间长度，修改入库标记
    int    Command;
	int    Lasttolib;
	char   flags;

}MAC_MEM,*pMAC_MEM;

struct index {
	char       macid[16];
	pMAC_MEM   machine;

};


	MAC_MEM*	    m_plist;
	pasLHashInfo    sHashInfo; //key 机器id  value mac_mem结构指针
	unsigned char*  m_inddex;



	/*
	登录过程，new MAC_MEM  赋值，创建索引m_inddex  加入链头
	*/
	bool	Login();

	/*
	find index from macid	脱链，修改最后活动时间，入到链的头部 
	*/
	bool	ConnectStatus();

	/*
	从链头获取一条，如果入库标记为空，而且时间超过1分钟，结束遍历操作，否则，处理，脱链，入到链的尾部
	*/
	BOOL	UpdateToLib();




#endif //CMYMACMEMINDEX88345834588