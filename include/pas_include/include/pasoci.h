#ifndef __OCI_H
#define __OCI_H
#include <oci.h>
#define CNM_MAX         32          /*max length of column name*/
#define COL_MAX         255         /*max count of columns*/

#define SQL_CMD_UNKNOWN     1       /*unknown command*/
#define SQL_CMD_SELECT      2       /*select command*/
#define SQL_CMD_INSERT      3       /*insert command*/
#define SQL_CMD_UPDATE      4       /*update command*/
#define SQL_CMD_DELETE      5       /*delete command*/
    
typedef struct
{    
    text *cbuf; /*column name*/
    sb4 cbufl;          /*length of the column name*/
    sb2 dbsize;         /*maximum size*/
    sb4 dsize;          /*maximum display size*/
    sb2 dbtype;         /*internal datatype code*/
    sb2 prec;           /*precision of numeric items*/
    sb2 scale;          /*scale of numerics*/
    sb2 nullok;         /*whether null values are permitted in the column*/    
} DB_DESCR;             /*describe the column info for select-list*/    

/*typedef OCIDescribe DB_DESCR;*/
    
typedef struct
{
    ub1* name;          /*A pointer to select-list name*/
    sword namel;        /*length of the select-list name*/
    ub1* buf;           /*A pointer to the output variable*/
    sword bufl;         /*length of the variable*/
    sword ftype;        /*The external datatype to which the select-list item is to be converted*/
    sb2 ind;            /*indicates the result of fetching select-list item*/
    ub2 rlen;           /*length of returned data*/
    ub2 rcode;          /*column return code after the fetch*/
} DB_DEFINE;            /*define output variables for select-list*/
       
typedef struct
{    
    OCIEnv *pEnv;
    OCIServer *pSrv;
    OCISession *pSession;
    OCISvcCtx *pSvc;
    OCIError *pErr;
} DB_CONNECT;
    
typedef struct
{        
    char *pSql;
    OCIStmt *pStmt;
    DB_CONNECT *pConn;
    DB_DEFINE* pDef;
    int nDef;
    int bDescribed;
} DB_CURSOR;


/*---------------------------------------------------------------
  �������ƣ�dbConnect()
  ����˵�����������ݿ�
  ����˵����[in]uid     -- ���ݿ��û���
            [in]passwd  -- ���ݿ�����
            [in]service -- ���ݿ������
  ����ֵ��  ���ɹ�  -- NULL
            �ɹ�    -- DB_CONNECT�ṹָ��
---------------------------------------------------------------*/
DB_CONNECT* dbConnect(char* uid, char* passwd, char* service);

/*---------------------------------------------------------------
  �������ƣ�dbDisconnect()
  ����˵�����Ͽ����ݿ����ӣ��ͷ�pConnָ���buffer
  ����˵����[in]pConn   -- DB_CONNECT�ṹָ��
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbDisconnect(DB_CONNECT* pConn);

/*---------------------------------------------------------------
  �������ƣ�dbOpen()
  ����˵���������ݿ��α�
  ����˵����[in]pConn   -- DB_CONNECT�ṹָ��
  ����ֵ��  ���ɹ�  -- NULL
            �ɹ�    -- DB_CURSOR�ṹָ��
---------------------------------------------------------------*/
DB_CURSOR* dbOpen(DB_CONNECT* pConn);

/*---------------------------------------------------------------
  �������ƣ�dbClose()
  ����˵�����ر����ݿ��α�
  ����˵����[in]pCursor -- DB_CURSOR�ṹָ��
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbClose(DB_CURSOR* pCursor);

/*---------------------------------------------------------------
  �������ƣ�dbParse()
  ����˵��������SQL���
  ����˵����[in]pCursor -- DB_CURSOR�ṹָ��
            [in]sqlstmt -- SQL����ַ�������'\0'��β
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbParse(DB_CURSOR* pCursor, char* sqlstmt);

/*---------------------------------------------------------------
  �������ƣ�dbBind()
  ����˵������SQL����еı�����Ӧ�ó����еı���
  ����˵����[in]pCursor -- DB_CURSOR�ṹָ��
            [in]sqlvar  -- SQL����еı���������'\0'��β
            [in]progv   -- �����еı���
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbBind(DB_CURSOR* pCursor, char* sqlvar, char* progv);

/*---------------------------------------------------------------
  �������ƣ�dbDescribe()
  ����˵����ȡ��SQL�����select-list������
  ����˵����[in]pCursor -- DB_CURSOR�ṹָ��
            [in]pos     -- select-list����ţ���1��ʼ����
            [in/out]pDescr  -- DB_DESCR�ṹָ��
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbDescribe(DB_CURSOR* pCursor, int pos, DB_DESCR* pDescr);

/*---------------------------------------------------------------
  �������ƣ�dbFetch()
  ����˵����ȡ���α�����λ�õļ�¼�������α��ƶ�����һ����¼
  ����˵����[in/out]pCursor -- DB_CURSOR�ṹָ��            
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbFetch(DB_CURSOR* pCursor);

/*---------------------------------------------------------------
  �������ƣ�dbFieldName()
  ����˵����ȡ��select-list������
  ����˵����[in]pCursor -- DB_CURSOR�ṹָ��
            [in]iPos    -- select-list����ţ���1��ʼ����
            [in/out]pName -- ָ����select-listֵ��buffer
            [in/out]pLen -- ָ��buffer���ȣ��������غ�ָ��ʵ�ʳ���
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbFieldName(DB_CURSOR* pCursor, int iPos, char* pName, int* pLen);

/*---------------------------------------------------------------
  �������ƣ�dbFieldValue()
  ����˵����ȡ��select-list��ֵ
  ����˵����[in]pCursor -- DB_CURSOR�ṹָ��
            [in]iPos    -- select-list����ţ���1��ʼ����
            [in/out]pVal -- ָ����select-listֵ��buffer
            [in/out]pLen -- ָ��buffer���ȣ��������غ�ָ��ʵ�ʳ���
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbFieldValue(DB_CURSOR* pCursor, int iPos, char* pVal, int* pLen);

/*---------------------------------------------------------------
  �������ƣ�dbExec()
  ����˵����exec sql statement and define select-list��
  ����˵����[in][out]pCursor  -- DB_CURSOR�ṹָ��
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbExec(DB_CURSOR* pCursor);

/*---------------------------------------------------------------
  �������ƣ�dbCommit()
  ����˵�����ύ�����ݿ�ĸ��¡�
  ����˵����[in]pConn   -- DB_CONNECT�ṹָ��            
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbCommit(DB_CONNECT* pConn);

/*---------------------------------------------------------------
  �������ƣ�dbRollback()
  ����˵����ȡ�����ϴ�commit֮������ݿ�ĸ��¡�
  ����˵����[in]pConn   -- DB_CONNECT�ṹָ��            
  ����ֵ��  ���ɹ�  -- 0
            �ɹ�    -- 1
---------------------------------------------------------------*/
int dbRollback(DB_CONNECT* pConn);

/*---------------------------------------------------------------
  �������ƣ�dbPrintError()
  ����˵������ʾ���ݿ����������Ϣ��
  ����˵����[in]pCursor  -- DB_CURSOR�ṹָ��
  ����ֵ��  �޷���
---------------------------------------------------------------*/
void dbPrintError(DB_CURSOR* pCursor);

/*---------------------------------------------------------------
  �������ƣ�dbGetLastError()
  ����˵����ȡ�����ݿ��������š�
  ����˵����[in]pCursor  -- DB_CURSOR�ṹָ��
  ����ֵ��  �����
---------------------------------------------------------------*/
int dbGetLastError(DB_CURSOR* pCursor);

/*---------------------------------------------------------------
  �������ƣ�dbSqlCommandType()
  ����˵����ȡ��Sql�������͡�
  ����˵����[in]sqlstmt  -- Sql���ָ��
  ����ֵ��  
            SQL_CMD_UNKNOWN -- δ֪
            SQL_CMD_SELECT -- select
            SQL_CMD_INSERT -- insert
            SQL_CMD_UPDATE -- update
            SQL_CMD_DELETE -- delete
---------------------------------------------------------------*/
int dbSqlCommandType(char* sqlstmt);

#endif
