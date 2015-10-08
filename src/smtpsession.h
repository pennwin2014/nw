/****************************smtpsession.h************************/
#define ERRLEN	25
#define BUFFSIZE	1461	
#define PASSOK	1
#define ERRORBUF	"451 Service unavailable\r\n"
#define GOAHEAD	"354 go ahead\r\n"
#define DATAREADYLEN	14
#define ACKOK	"250 ok\r\n"
#define OKLEN	8

struct data_buff
{
	struct data_buff *next;
	struct data_buff *prev;
	struct data_buff_head *list;
	unsigned short datalen;
	unsigned short iscommand;
	char data[515];//smtp:max is 512,esmtp maybe bigger than 512
};
struct data_buff_head
{
	struct data_buff *next;
	struct data_buff *prev;
	unsigned long ip_src;
	unsigned long ip_dest;
	unsigned short port_src;
	unsigned short port_dest;
	long qlen;
};
typedef struct data_buff_head*  ncMailSmtpSession;
/*申请一个节点，已初始化*/
struct data_buff *alloc_data_buff();
/*释放一个节点*/
void free_data_buff(struct data_buff *datab);
/*释放整个链表*/
void free_all(struct data_buff_head *datab_head);
/*初始化链表头*/
void init_buff_head(struct data_buff_head *datab_head);
struct data_buff * datab_peek(struct data_buff_head *list_);
struct data_buff * datab_peek_tail(struct data_buff_head *list_);
void datab_queue_head_init(struct data_buff_head *list);
/*将节点插入在链表的第一个位置*/
void datab_queue_head(struct data_buff_head *list,struct data_buff *newdata);
/*将节点插入在链表的尾部*/
void datab_queue_tail(struct data_buff_head *list,struct data_buff *newdata);
/*取得第一个节点*/
struct data_buff * datab_dequeue(struct data_buff_head *list);
void datab_insert(struct data_buff *old,struct data_buff *newdata);
void datab_append(struct data_buff *old,struct data_buff *newdata);
/*从链表中删除一个节点*/
void datab_unlink(struct data_buff *datab);
/*取得最后一个节点*/
struct data_buff * datab_dequeue_tail(struct data_buff_head *list);
/*
**	链表复制
**	如果失败返回-1 否则返回0
*/
int duplink(struct data_buff_head *src,struct data_buff_head *dest);
int session2file(struct data_buff_head *datab_head,char *szFile);
int file2session(char *szFile,struct data_buff_head *datab_head);
int copy_to_buff(struct data_buff *datab,char *szSess,int iSessLen);
int save2link(struct data_buff_head *datab_head,char *sBuf,int iBuflen,int flag);
void resetlink(struct data_buff_head *datab_head);
void errorack(int iSock);
int goaheadack(int iSock);
int okack(int iSock);
int closeack(int iSock,unsigned long lDip);
