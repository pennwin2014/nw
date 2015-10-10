#ifndef UTERRCODE_DEF
#define UTERRCODE_DEF

/* �������  */
/* ���������10λ�������
   AA BBB C DDDD
   ����:
   AA  ---- ��Ŀ���
   BBB ---- ��Ŀ�к������
   C   ---- �������
   DDDD---- ����Ĵ�������
*/

/* ��Ŀ���  */
#define PROCODE_PICQSERVER    1      /* PICQ Server */
#define PROCODE_PICQONLINE    2      /* ����������  */
#define PROCODE_UTILSHM       3      /* ʵ�ú���    */
#define PROCODE_UTILSTR       4      
#define PROCODE_UTILCOM       5
#define PROCODE_UTILMSG       6
#define PROCODE_MEETSRV       7      /* ���������  */


/* ��������  */
#define FUN_UTSHM_utStr01           1
#define FUN_pqcUtlReqNumber         10

#define FUN_utComTcpMsgSend         401
#define FUN_utComTcpMsgRead         402
#define FUN_utComTcpMsgRead000      403

/* ��������  */

#define ERR_TYPE_CODE         0    /* ��ϵͳ�涨 */
#define ERR_TYPE_DBMS         1    /* SQLCODE    */
#define ERR_TYPE_SYSTEM       2    /* errorno    */
#define ERR_TYPE_USER         9    /* �Զ���     */

/* ��ϵͳ�ڲ�ʹ��   */
#define ERR_CODE_PACKAGE      1    /* ���ݰ�����     */
#define ERR_CODE_VARNUM       2    /* ������������   */
#define ERR_CODE_INVMSGCODE   3    /* ��Ч�����ݴ��� */
#define ERR_CODE_PICQOFFLINE  4    /* PICQ������     */
#define ERR_CODE_CONFIRMREAD  5    /* ȷ����Ϣ����   */
#define ERR_CODE_WRITEERR     6    /* д����         */


#endif
