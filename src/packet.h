#ifndef _PACKET_H_
#define _PACKET_H_ 1

#define TRUE 1
#define FALSE 0
#define _CLIENT_TO_SERVER 1
#define _SERVER_TO_CLIENT 2
#define _NO_CALC_FEE      3
#define _RELATION_NO_EXIST 0
#define _RELATION_EXIST    1

#define PACKET_SLOT_SIZE  500
#define CONNECT_RELATION_SIZE 1000
#define INTERNAL_IP_SIZE 100

typedef struct vary {
    short vary_length;
    char vary_string[1];
} VARY;


struct PACKET_RAW_SLOT
  {
    char stat;
#define _PF_UNUSED 0
#define _PF_INUSED 1
    char buffer[40];
  };



struct CONNECT_RELATION
  {
    char stat;
#define _CF_UNUSED 0
#define _CF_INUSED 1
#define _CF_NOTSAVE 2
    u_int8_t internal_client[4];
    u_int8_t external_server[4];
    char     username[20];
    time_t   conn_time;
  };

struct INTERNAL_IP
  {
    char stat;
#define _IF_UNUSED  0
#define _IF_INUSED  1
#define _IF_CALCFEE 2
    char start[4];
    char end[4];
  };

struct SYSVAR {
  char pause_flag;
};
extern struct SYSVAR _sv;
extern struct PACKET_RAW_SLOT packet_slot[PACKET_SLOT_SIZE];
extern struct INTERNAL_IP internal_ip_table[INTERNAL_IP_SIZE];
extern struct CONNECT_RELATION connect_relation[CONNECT_RELATION_SIZE];
extern struct active_ip *active_ip_table;
extern FILE *logfile;
extern char  msg_logout[256];
extern char  msg_nologin[256];



#define DEBUG_LOG(INFO)  debug_log(__FILE__, __LINE__, INFO)
void init_log(void);
void debug_log(char *filename,int line,char *info);



void doProcessUpPacket (int packet_index);
void doProcessDownPacket (int packet_index);
char *get_source_ip (int index);
char *get_dest_ip (int index);
pcap_t *init_pcap (void);
void packethandler (unsigned char *ipaddrpoint,
	            const struct pcap_pkthdr *p_header,
	            const unsigned char *sp);
void collect_packet (void);
int get_packet_from_slot (void);
int the_ip_between (char *dest, char *start, char *end);
int is_internal_addr (char *ip_addr, int *calc_fee);
int is_client_to_server (struct tcphdr *tcph);
int category (int index);
void remove_packet_from_slot (int index);
void analyses_packet (void);
int active_table_exist (char *ip_addr, int *);
void distroy_userlink (int index);
int search_relation (int index);
void add_relation (int index, int);
void update_relation_table (int index, int active_index);
int packet_with_ACK_or_SYN (int index);
unsigned short get_packet_size (int index);
int get_sequnce_in_active_ip (char *ip_addr);
void record_up_flow (int index);
void record_down_flow (int index);
char *get_dest_ip (int index);
char *get_source_ip (int index);
void update_active_time(int index);
void doProcessUpPacket (int index);
void doProcessDownPacket (int index);
void save_data (void);
void init_shm(void);

void init_memory_table(void);

void load_internal_ip_table(void);


#endif

