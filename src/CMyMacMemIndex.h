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
	int	   tolibtime; //�ϴ����ʱ�䣬��������ʱ�䳤�ȣ��޸������
    int    Command;
	int    Lasttolib;
	char   flags;

}MAC_MEM,*pMAC_MEM;

struct index {
	char       macid[16];
	pMAC_MEM   machine;

};


	MAC_MEM*	    m_plist;
	pasLHashInfo    sHashInfo; //key ����id  value mac_mem�ṹָ��
	unsigned char*  m_inddex;



	/*
	��¼���̣�new MAC_MEM  ��ֵ����������m_inddex  ������ͷ
	*/
	bool	Login();

	/*
	find index from macid	�������޸����ʱ�䣬�뵽����ͷ�� 
	*/
	bool	ConnectStatus();

	/*
	����ͷ��ȡһ������������Ϊ�գ�����ʱ�䳬��1���ӣ������������������򣬴����������뵽����β��
	*/
	BOOL	UpdateToLib();




#endif //CMYMACMEMINDEX88345834588