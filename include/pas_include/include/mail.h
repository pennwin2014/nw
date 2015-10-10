#ifndef _X_MAIL_HEAD_
#define _X_MAIL_HEAD_

/*
 * Name:	mail_header.h
		�����ʼ���ͷ��Ϣ
*/

typedef struct xContent{
	char *pContent;
	unsigned long len;
	unsigned long lSize;
}mailContent;


typedef struct
{
        int             isnew;                  /* new is 1 or not new is 0     */
        char            *addressF;              /* address */
        char            *personF;
        char            *addressT;              /* address   */
        char            *personT;
        char            *addressCc;             /* address   */
        char            *personCc;
        char            *addressBcc;             /* address   */
        char            *personBcc;
        char            *subject;               /* subject item   */
        char            *date;                  /* date item      */
        unsigned        long size;              /* message size   */
        int             important;              /*msg important 0 1 2  */
}mail_header_info;

struct  mattach {
	char displayname[256];     /*Ҫ��ʾ�ĸ�����*/
        char filename[256];        /* ����ĸ�����  */
        unsigned long filesize;         /* ������С    */
        char content_type[128];         /* ��������*/
        char cid[128];               /* ����CID  */
        struct  mattach *next;       /* ��һ������  */
} ;
typedef struct mattach  mail_attach;

typedef struct {
	    int  contype;
        char *textcon;
        char *htmlcon;
        char *charset;  
        mail_attach *attach;                      
} mail_body;


int conAdd_v(mailContent *hContent,char *pValue,unsigned long lLen);
mailContent *initContent(unsigned long  lMailSize);
int conCopy(mailContent *fContent,mailContent *tContent);


mail_header_info *mail_header(char *mailfn, int *errFlag);
int mailHeadFree(mail_header_info *mailHead);

/*����˵����
mailfn�� �ʼ�url
dir:     �ʼ��������ŵ�ַ
prefile�� �ʼ������󸽼���ŵ�ǰ׺
precid��  cid�滻 precid+�ʼ������󸽼���ŵ�����
errFlag=-1  �ļ�������
errFlag=-2  �����ż����
-3	    rfc2047_decode_simple error
-4          lseek error
-5          �����ļ�������
-6          ���ļ����� 
-7          д�ļ�����
-8       
-10        cid �滻   

*/

mail_body *mail_getbody(char *mailfn, char *dir, char *prefile, char *precid, int *errFlag);
void freeBody(mail_body *mBody);


/*�����鵵*/
/*
mimebuf:
	file1\nfile2\nfile3

fn:     Ҫ�����·��
conflag: 0--Text  1--HTML
*/

int mail_digest(char *mimebuf,char *fn,char *from,char *to,char *subject,char *content,int conflag);

/*

���룺
gcc -o test test.c -I../include ../lib/maillib.a 

*/

/*headercache*/
/*��Ϣͷ�Ļ����봦��*/
int newMailCache(char *maildir,char *mailfn);



#endif





