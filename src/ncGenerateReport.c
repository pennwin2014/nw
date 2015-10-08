#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <iconv.h>
#include <locale.h>
#include <sys/stat.h>
#include "pasdb.h"

#include "ncreport.h"
#include "ncdef.h"

#define REPORT_PER_DAY   0  
#define REPORT_PER_WEEK  1  
#define REPORT_PER_MONTH 2 
#define REPORT_PER_NONE  3  



int utPltFileDownload(int, char *, char *, char *, char *);
int utMsgGetSomeNVar(utMsgHead *, int,...);
char *utStrLtoF(long, char *);



struct gd_color{
	int r;
	int g;
	int b;
};


/*
static struct gd_color report_color[] = {
	 {0x00, 0xfa, 0x9a},
	 {0xff, 0x00, 0x00},
	 {0xcc, 0xff, 0x33},
	 {0x99, 0xcc, 0x00},
	 {0x00, 0xff, 0xff},
	 {0xd2, 0x69, 0x1e},
   {0x80, 0x00, 0x80},
   {0x6b, 0x8e, 0x23},
   {0xc0, 0xc0, 0xc0},
   {0xff, 0xff, 0x00},
   {0xff, 0x00, 0xff},
};
*/


static struct gd_color report_color[] = {
	 {0x00, 0xfa, 0x9a},
	 {0xff, 0x00, 0x00},
	 {0x00, 0xeb, 0xcd},
	 {0xa5, 0x2a, 0x2a},
	 {0x77, 0x88, 0x99},
	 {0xe2, 0x69, 0x1e},
   {0x80, 0x00, 0x80},
   {0x6b, 0x8e, 0x23},
   {0xc0, 0xc0, 0xc0},
   {0xff, 0xff, 0x00},
   {0xff, 0x00, 0xff},
};


struct chart_dataset {
	int  id;
	char *date;
	
	char *name;
	unsigned long  *dataset;
};


struct user_data{
		  char dispname[32];
    	char groupname[32];
    	long userid;
    	long long ip;
    	long long bytes;
    	long long lcount;
    	unsigned long ctime;
    	
    	
    	char **l_host;
    	long long *l_bytes;
    	long long *l_lcount;
    	unsigned long *l_ctime;
    	int 	num;
    };
    
    
struct table_data {
 	 int  flag;
 	 char *id;
 	 char *groupname;
 	 char *dispname;
 	 char *ip;
 	 char *host;
 	 char *ctime;
 	 long long bytes;
 	 long long lcount;
 	};
 	
 	
struct piechart_dataset{
	char *name;
	unsigned long  amount;
};




static int cmp1(struct piechart_dataset *i, struct piechart_dataset *j)
{
  return (j->amount - i->amount);
}


static int cmp2(struct chart_dataset *i, struct chart_dataset *j)
{
   return (j->dataset[2] - i->dataset[2]);
}


static int cmp3(struct chart_dataset *i, struct chart_dataset *j)
{
    return (i->id - j->id);
}


static int cmp4(struct chart_dataset *i, struct chart_dataset *j)
{
   return (j->dataset[3] - i->dataset[3]);
}

static int cmp5(unsigned long *i, unsigned long *j)
{
   return (int)(*j - *i);
}



static int cmp6(float *i, float *j)
{
   return (int)(*j - *i);
}

static int cmp7(struct piechart_datasetf *i, struct piechart_datasetf *j)
{
  return (j->amount - i->amount);
}








static unsigned long sumd(unsigned long *dataset, int j)
{
	unsigned long sum = 0.0;
	int n = 0;
	for(n = 0; n <= j; n++)
	{
		sum += dataset[n];
	}
	return sum;
}



static float sumf(float *dataset, int j)
{
	float sum = 0.0;
	int n = 0;
	for(n = 0; n <= j; n++)
	{
		sum += dataset[n];
	}
	return sum;
}
static char *convert(const char *codefrom, const char *codeto, const char *src)
{
	
}




char *str_delstr(char *p1,const char *p2)
{
  
}
void remove_watermark(char *filename)
{
	
}


static int draw_histogram_piechart(struct piechart_dataset *dataset, int len, char *font, char *filename, unsigned long total_y, int start_x, char *s)
{
	

  return 0;
  
}






static int draw_histogram_piechartf(struct piechart_datasetf *dataset, int len, char *font, char *filename, unsigned long total_y, int start_x, char *s)
{
	
	
  return 0;
  
}
//����
static int draw_histogram_piechartf_jd(struct piechart_datasetf *dataset, int len, char *font, char *filename, unsigned long total_y, int start_x, char *s)
{
	

  return 0;
  
}

static int draw_histogram(struct piechart_dataset *dataset, int len, char *font, char *filename, unsigned long total_y, int start_x, char *s)
{
	
 return 0;
}








static int draw_histogram_trendchartf(struct chart_datasetf *dataset, char **desc, int row, int column, char *font, char *filename,char *s,unsigned long total_y)
{
	
 return 0;
  
}


/* ͨ������*/
static int ncWebGeneralReport()
{
	return 0;
	
}









/* ����������� */
static void ncWebOutFlowReport()
{
	

	
}





/* �û�������վ��ϸ */
static void ncWebSortUserReport()
{
	
 return;                
}





/* �û�����ʱ��,�û��������� */
static void ncWebSortUserReport2()
{

}


/* �ʼ��շ����� */
static void ncWebMailMonCountReport()
{
	


	
}



/* �û������ϸ */
static void ncFobSortReport()
{
	
	
 return;                
}




/* ��������ʱ��,�û��������� */
static void ncWebSortGroupReport()
{
	
 
 }

char *ncUtlGetGroupNameByid(utShmHead *,unsigned long, char*);
//���ʱ�䣬�û�����ר��
int ncWeb_split()
{

}
int ncWebAllReport_PDF(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

}                                      
	
	
	



int ncWebAllReport_user_PDF(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

}






int ncGenerateReport(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{

}

/*
   int iFd                         PAS����� iFd, �������������utPltFileDownload���غ���
   struct piechart_datasetf *chart ����
   char **desc                     ��ͷ ��{"�û���", "����", "����(KB)", "�ٷֱ�"}
   int rowrow                      chart ����
   int column                      desc  ����
   int applen                      �����ֶθ���
   char *title                     ����
   char *unit                      ��λ��ע �� "��λ(KB)"
   char *report_text
   char *filename_new              PDF�ļ���
    

 */
int ncWebShowServiceClassFirst_PDF_jd(int iFd, struct piechart_datasetf *chart, char **desc, char **descSum, int rowrow, int column, int applen, char *title, char *unit, char *report_text, char *filename_new)
{

}
//����url
int ncWebShowServiceClassFirst_PDF_jdurl(int iFd, struct piechart_datasetf *chart, char **desc, char **descSum, int rowrow, int column, int applen, char *title, char *unit, char *report_text, char *filename_new)
{

}
/*
   int iFd                         PAS����� iFd, �������������utPltFileDownload���غ���
   struct piechart_datasetf *chart ����
   char **desc                     ��ͷ ��{"�û���", "����", "����(KB)", "�ٷֱ�"}
   int rowrow                      chart ����
   int column                      desc  ����
   int applen                      �����ֶθ���
   char *title                     ����
   char *unit                      ��λ��ע �� "��λ(KB)"
   char *report_text
   char *filename_new              PDF�ļ���
    

 */
int ncWebShowServiceClassFirst_PDF(int iFd, struct piechart_datasetf *chart, char **desc, int rowrow, int column, int applen, char *title, char *unit, char *report_text, char *filename_new)
{

}

/*
   int iFd                         PAS����� iFd, �������������utPltFileDownload���غ���
   struct piechart_datasetf *chart ����
   char **desc                     ��ͷ ��{"�û���", "����", "����(KB)", "�ٷֱ�"}
   int rowrow                      chart ����
   int column                      desc  ����
   int applen                      �����ֶθ���
   char *title                     ����2009-8-25 9:49:06
   char *unit                      ��λ��ע �� "��λ(KB)"
   char *report_text
   char *filename_new              PDF�ļ���
   int  dataflag                   dataflag(1:�����е�һ�п��Ҷ��룬2�������еڶ��п��Ҷ��룬3��������ȫ���Ҷ���,Ĭ��ȫ�������) 

 */
int ncWebShowServiceSortFirst_PDF(int iFd, struct piechart_datasetf *chart, char **desc, int rowrow, int column, int applen, char *title, char *unit, char *report_text, char *filename_new,int dataflag)
{

}


/*
   int iFd                         PAS����� iFd, �������������utPltFileDownload���غ���
   struct piechart_datasetf *chart ����
   char **desc                     ��ͷ ��{"�û���", "����", "����(KB)", "�ٷֱ�"}
   int rowrow                      chart ����
   int column                      desc  ����
   int applen                      �����ֶθ���
   char *title                     ����
   char *unit                      ��λ��ע �� "��λ(KB)"
   char *report_text
   char *filename_new              PDF�ļ���
   
   int  dataflag                   dataflag(1:�����е�һ�п��Ҷ��룬2�������еڶ��п��Ҷ��룬3��������ȫ���Ҷ���,Ĭ��ȫ�������)  
   char *sendaddr                  �����˵�ַ
   char *receiveaddr							 �ռ��˵�ַ
   char *mailtitle								 ����
   char *mailcontent               ����
   char *mailpath                  �����ļ�����·��
   char *maildispn                 ������ʾ�ļ���
 */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~paf�ʼ�����
int ncWebShowServiceMail_PDF(int iFd, struct piechart_datasetf *chart, char **desc, int rowrow, int column, int applen, char *title, char *unit, char *report_text, char *filename_new,int dataflag,char *sendaddr,char *receiveaddr,char *mailtitle,char *mailcontent,char *mailpath,char *maildispn)
{

}
/*
   int iFd                         PAS����� iFd, �������������utPltFileDownload���غ���
   struct piechart_datasetf *chart ����
   char **desc                     ��ͷ
   int rowrow                      chart ����
   int column                      desc  ����
   char *title                     ����
   char *unit                      ��λ��ע �� "��λ(KB)"
   char *report_text               
   char *filename_new              PDF�ļ���
   
   
*/
int ncWebShowServiceBytime_mail_PDF(int iFd, struct chart_datasetf *chart, char **desc, int rowrow, int column, char *title, char *unit, char *report_text, char *filename_new,char *sendaddr,char *receiveaddr,char *mailtitle,char *mailcontent,char *mailpath,char *maildispn)
{

}
/*
   int iFd                         PAS����� iFd, �������������utPltFileDownload���غ���
   struct piechart_datasetf *chart ����
   char **desc                     ��ͷ
   int rowrow                      chart ����
   int column                      desc  ����
   char *title                     ����
   char *unit                      ��λ��ע �� "��λ(KB)"
   char *report_text               
   char *filename_new              PDF�ļ���
*/
int ncWebShowServiceBytime_PDF(int iFd, struct chart_datasetf *chart, char **desc, int rowrow, int column, char *title, char *unit, char *report_text, char *filename_new)
{

}
//����ncWebGeneralReport9������pdfͼ							len==20,totalʵ�ʼ�¼��
static int draw_histogram_piechart9(struct piechart_dataset *dataset, int len,int total, char *font, char *filename, unsigned long total_y, int start_x, char *s)
{

}
//��ַ��������������
int ncWebGeneralReport9()
{

}

int ncWebGeneralRepeat2()
{

}
//������Ϊ��������-ncreport���̷��ͺ���
char *ncWebAllReport_PDF_outgiving(utShmHead *psShmHead,int iFd, int hp_typeflag,char hp_week_mon[16],char hp_week_sun[16],char hp_month_fir[16],char hp_month_las[16],int hp_ttype,int hp_groupid,char hp_mail[64],int folder_count)
{ 

}	
//�ۺϱ���  �û�ѭ����״ͼ�ӿ�
static int draw_complex_serverzzt_jd(struct piechart_datasetf *dataset, int len, char *font, char *filename, unsigned long total_y, int start_x, char *s,int pagenum,int rowtotal)
{

}
//�ۺϱ���--�û�ͳ�ƽӿ�
int ncWebShowServiceComplex_PDF_djuser()
{printf("==== user === \n");

}
//�ۺϱ���-- urlͳ�ƽӿ�
int ncWebShowServiceComplex_PDF_jdurl()
{	

}
//�������� -top5ѭ����״ͼ�ӿ�
static int draw_histogram_zzt_djtop5(struct piechart_datasetf *dataset, int len, char *font, char *filename, unsigned long total_y, int start_x, char *s)
{

}
//�ۺϱ���-- url������ϸ�ӿ�
int ncWebShowServiceComplex_PDF_jdcycleurl()
{		printf("=== url��ϸ === \n");

}
//�������� service-top5ѭ����״ͼ�ӿ�
static int draw_histogram_zzt_djservicetop5(struct piechart_datasetf *dataset, int len, char *font, char *filename, unsigned long total_y, int start_x, char *s)
{

}
//�ۺϱ���--�������ӿ�
int ncWebShowComplex_PDF_jdcycleservice()
{		printf("=== Services��ϸ === \n");

}
//�ۺϱ���-- ����ſ�
int ncWebShowServiceComplex_PDF_jdall()
{		printf("=== ����ſ� === \n");

}
// ����ʱ���ۺϱ���
//char *nc_jingdong_reportall_PDF_outgiving(utShmHead *psShmHead,int iFd, int hp_typeflag,char hp_week_mon[16],char hp_week_sun[16],char hp_month_fir[16],char hp_month_las[16],int hp_ttype,int hp_groupid,char hp_mail[64],int folder_count,char dj_uname_t[254],char dj_ip_t[254],char dj_site_t[299],int totalnum,int detail_num)
char *nc_jingdong_reportall_PDF_outgiving(utShmHead *psShmHead,int iFd, int hp_typeflag,char hp_week_mon[16],char hp_week_sun[16],char hp_month_fir[16],char hp_month_las[16],int hp_ttype,int hp_groupid,char hp_mail[64],int folder_count,char dj_uname_t[254],char dj_ip_t[254],char dj_site_t[299])
{  

}
