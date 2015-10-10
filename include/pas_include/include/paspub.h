#ifndef __PUBLIC_H
#define __PUBLIC_H

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include "utoall.h"
#include "utopc01.h"
#include "utotrs01.h"
#include "utoplt01.h"

/*---------------------------------------------------------------
  �������ƣ�pubExecSQL()
  ����˵����ִ��SQL��䣬���Զ�����Ӧ�Ĵ���
            �����������psMsgHead��(���贫�롢��������ʱ��ʹ��NULL):
                CurPg -- ��ʾ�����Ҫ��ʾ��CurPgҳ
                Row   -- ��ʾ����������ΪRowPg
                Col   -- ��ʾ��ʾ��ά�б�����ΪCol
                �Լ�������placeholder�ı���
            ��������׷����psMsgHead�У�ͬʱ���뵽psDbHead��:
                CurPg -- ��ʾ�����Ҫ��ʾ��CurPgҳ
                TotPg  -- ��ҳ��
                TotRec -- �ܼ�¼��
                Row   -- ��ʾ����������ΪRowPg
                Col   -- ��ʾ��ʾ��ά�б�����ΪCol
                �Լ�������placeholder�ı���
                ��select-list��ֵ                
  ����˵����[in]pConn   -- DB_CONNECT�ṹָ��
            [in]sql     -- SQL����ַ�������'\0'��β
            [in][out]psMsgHead -- utMsgHead�ṹָ��
            [in][out]psDbHead  -- utPltDbHead�ṹָ��
  ����ֵ��  SQL�������
---------------------------------------------------------------*/
int pubExecSQL(/*[in]*/DB_CONNECT* pConn, 
               /*[in]*/char* sql, 
               /*[in][out]*/utMsgHead** psMsgHead,
               /*[in][out]*/utPltDbHead* psDbHead);
               

#endif