#ifndef  ___PAS_MAILDEF___
#define  ___PAS_MAILDEF___  
#define  PM_LOG_EVENT           0                 /* �¼�                 */
#define  PM_LOG_ERROR           1                 /* ������Ϣ             */
#define  PM_LOG_MAILSEND        200               /* �ʼ����ͳɹ�           */
#define  PM_LOG_MAILPUT         201               /* ���ʼ��ŵ����гɹ�     */
#define  PM_ERR_SMTPTCP         101               /* SMTP ��������������  */
#define  PM_ERR_INVALIDTO       102               /* ��Ч���ռ���         */
#define  PM_ERR_NOMX            103               /* �Ҳ���MX��������������  */
#define  PM_ERR_SMTPSRV         104               /* �ʼ����������ӳ���     */
#define  PM_ERR_MSGFILE         105               /* �ʼ����ݲ�����         */

#define  PAS_MAILQ_ID           20061129L
typedef struct pasMailQFile_s {
    long id;                  /* �ļ�ID     */
    long count;               /* �ط�����   */
    long lasttime;            /* ���һ�η�������  */
    char to[128];
    char from[128];
} pasMailQFile;

#include "pasdef.h"    
/* pasutl_mail.c */
int pasMailSetESmtpAuth(int flags);
int pasMailSetSmtpAuth(char *pUser,char *pPass,char *pDomain);
char *get_mail_server( const char *remote, const char *local );
int pasSendMailBySmtp(char *msg,char *pTo,char *pFrom);
int pasSmtpSendSession(int iFd,char *pComm,...);
int pasCvtDosStr2Unix(char *pBuf,char *pBuf1,int l);
int pasMailDebug(int iFlags);
int pasMailSendStart();
int pasMailSendStart1();
int pasMailSendStart0();
int pasMailServerIsStart();
int pasMailLog(int status,char *pMsg,...);
int pasMailInitMailDir(char *pHome);
int pasSendMail(char *pMsg,char *pFrom,char *pTo);
int pasSendMailCont(char *pMsg,char *pFrom,char *pTo);

    
#endif
