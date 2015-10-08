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
//京东
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


/* 通用类型*/
static int ncWebGeneralReport()
{
	return 0;
	
}









/* 网络出口流量 */
static void ncWebOutFlowReport()
{
	

	
}





/* 用户访问网站详细 */
static void ncWebSortUserReport()
{
	
 return;                
}





/* 用户上网时间,用户上网流量 */
static void ncWebSortUserReport2()
{

}


/* 邮件收发数量 */
static void ncWebMailMonCountReport()
{
	


	
}



/* 用户阻断详细 */
static void ncFobSortReport()
{
	
	
 return;                
}




/* 部门上网时间,用户上网流量 */
static void ncWebSortGroupReport()
{
	
 
 }

char *ncUtlGetGroupNameByid(utShmHead *,unsigned long, char*);
//拆分时间，用户流量专用
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
   int iFd                         PAS里面的 iFd, 后面用其调用了utPltFileDownload下载函数
   struct piechart_datasetf *chart 数据
   char **desc                     表头 如{"用户名", "部门", "流量(KB)", "百分比"}
   int rowrow                      chart 长度
   int column                      desc  长度
   int applen                      附加字段个数
   char *title                     标题
   char *unit                      单位标注 如 "单位(KB)"
   char *report_text
   char *filename_new              PDF文件名
    

 */
int ncWebShowServiceClassFirst_PDF_jd(int iFd, struct piechart_datasetf *chart, char **desc, char **descSum, int rowrow, int column, int applen, char *title, char *unit, char *report_text, char *filename_new)
{

}
//京东url
int ncWebShowServiceClassFirst_PDF_jdurl(int iFd, struct piechart_datasetf *chart, char **desc, char **descSum, int rowrow, int column, int applen, char *title, char *unit, char *report_text, char *filename_new)
{

}
/*
   int iFd                         PAS里面的 iFd, 后面用其调用了utPltFileDownload下载函数
   struct piechart_datasetf *chart 数据
   char **desc                     表头 如{"用户名", "部门", "流量(KB)", "百分比"}
   int rowrow                      chart 长度
   int column                      desc  长度
   int applen                      附加字段个数
   char *title                     标题
   char *unit                      单位标注 如 "单位(KB)"
   char *report_text
   char *filename_new              PDF文件名
    

 */
int ncWebShowServiceClassFirst_PDF(int iFd, struct piechart_datasetf *chart, char **desc, int rowrow, int column, int applen, char *title, char *unit, char *report_text, char *filename_new)
{

}

/*
   int iFd                         PAS里面的 iFd, 后面用其调用了utPltFileDownload下载函数
   struct piechart_datasetf *chart 数据
   char **desc                     表头 如{"用户名", "部门", "流量(KB)", "百分比"}
   int rowrow                      chart 长度
   int column                      desc  长度
   int applen                      附加字段个数
   char *title                     标题2009-8-25 9:49:06
   char *unit                      单位标注 如 "单位(KB)"
   char *report_text
   char *filename_new              PDF文件名
   int  dataflag                   dataflag(1:附加列第一列靠右对齐，2：附加列第二列靠右对齐，3：附加列全靠右对齐,默认全向左对齐) 

 */
int ncWebShowServiceSortFirst_PDF(int iFd, struct piechart_datasetf *chart, char **desc, int rowrow, int column, int applen, char *title, char *unit, char *report_text, char *filename_new,int dataflag)
{

}


/*
   int iFd                         PAS里面的 iFd, 后面用其调用了utPltFileDownload下载函数
   struct piechart_datasetf *chart 数据
   char **desc                     表头 如{"用户名", "部门", "流量(KB)", "百分比"}
   int rowrow                      chart 长度
   int column                      desc  长度
   int applen                      附加字段个数
   char *title                     标题
   char *unit                      单位标注 如 "单位(KB)"
   char *report_text
   char *filename_new              PDF文件名
   
   int  dataflag                   dataflag(1:附加列第一列靠右对齐，2：附加列第二列靠右对齐，3：附加列全靠右对齐,默认全向左对齐)  
   char *sendaddr                  发件人地址
   char *receiveaddr							 收件人地址
   char *mailtitle								 主题
   char *mailcontent               内容
   char *mailpath                  附件文件名带路径
   char *maildispn                 附件显示文件名
 */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~paf邮件功能
int ncWebShowServiceMail_PDF(int iFd, struct piechart_datasetf *chart, char **desc, int rowrow, int column, int applen, char *title, char *unit, char *report_text, char *filename_new,int dataflag,char *sendaddr,char *receiveaddr,char *mailtitle,char *mailcontent,char *mailpath,char *maildispn)
{

}
/*
   int iFd                         PAS里面的 iFd, 后面用其调用了utPltFileDownload下载函数
   struct piechart_datasetf *chart 数据
   char **desc                     表头
   int rowrow                      chart 长度
   int column                      desc  长度
   char *title                     标题
   char *unit                      单位标注 如 "单位(KB)"
   char *report_text               
   char *filename_new              PDF文件名
   
   
*/
int ncWebShowServiceBytime_mail_PDF(int iFd, struct chart_datasetf *chart, char **desc, int rowrow, int column, char *title, char *unit, char *report_text, char *filename_new,char *sendaddr,char *receiveaddr,char *mailtitle,char *mailcontent,char *mailpath,char *maildispn)
{

}
/*
   int iFd                         PAS里面的 iFd, 后面用其调用了utPltFileDownload下载函数
   struct piechart_datasetf *chart 数据
   char **desc                     表头
   int rowrow                      chart 长度
   int column                      desc  长度
   char *title                     标题
   char *unit                      单位标注 如 "单位(KB)"
   char *report_text               
   char *filename_new              PDF文件名
*/
int ncWebShowServiceBytime_PDF(int iFd, struct chart_datasetf *chart, char **desc, int rowrow, int column, char *title, char *unit, char *report_text, char *filename_new)
{

}
//辅助ncWebGeneralReport9函数画pdf图							len==20,total实际记录数
static int draw_histogram_piechart9(struct piechart_dataset *dataset, int len,int total, char *font, char *filename, unsigned long total_y, int start_x, char *s)
{

}
//网址请求数辅助函数
int ncWebGeneralReport9()
{

}

int ncWebGeneralRepeat2()
{

}
//上网行为分析报告-ncreport进程发送函数
char *ncWebAllReport_PDF_outgiving(utShmHead *psShmHead,int iFd, int hp_typeflag,char hp_week_mon[16],char hp_week_sun[16],char hp_month_fir[16],char hp_month_las[16],int hp_ttype,int hp_groupid,char hp_mail[64],int folder_count)
{ 

}	
//综合报表  用户循环柱状图接口
static int draw_complex_serverzzt_jd(struct piechart_datasetf *dataset, int len, char *font, char *filename, unsigned long total_y, int start_x, char *s,int pagenum,int rowtotal)
{

}
//综合报表--用户统计接口
int ncWebShowServiceComplex_PDF_djuser()
{printf("==== user === \n");

}
//综合报表-- url统计接口
int ncWebShowServiceComplex_PDF_jdurl()
{	

}
//东京海上 -top5循环柱状图接口
static int draw_histogram_zzt_djtop5(struct piechart_datasetf *dataset, int len, char *font, char *filename, unsigned long total_y, int start_x, char *s)
{

}
//综合报表-- url个人明细接口
int ncWebShowServiceComplex_PDF_jdcycleurl()
{		printf("=== url明细 === \n");

}
//东京海上 service-top5循环柱状图接口
static int draw_histogram_zzt_djservicetop5(struct piechart_datasetf *dataset, int len, char *font, char *filename, unsigned long total_y, int start_x, char *s)
{

}
//综合报表--网络服务接口
int ncWebShowComplex_PDF_jdcycleservice()
{		printf("=== Services明细 === \n");

}
//综合报表-- 总体概况
int ncWebShowServiceComplex_PDF_jdall()
{		printf("=== 总体概况 === \n");

}
// 上网时间综合报表
//char *nc_jingdong_reportall_PDF_outgiving(utShmHead *psShmHead,int iFd, int hp_typeflag,char hp_week_mon[16],char hp_week_sun[16],char hp_month_fir[16],char hp_month_las[16],int hp_ttype,int hp_groupid,char hp_mail[64],int folder_count,char dj_uname_t[254],char dj_ip_t[254],char dj_site_t[299],int totalnum,int detail_num)
char *nc_jingdong_reportall_PDF_outgiving(utShmHead *psShmHead,int iFd, int hp_typeflag,char hp_week_mon[16],char hp_week_sun[16],char hp_month_fir[16],char hp_month_las[16],int hp_ttype,int hp_groupid,char hp_mail[64],int folder_count,char dj_uname_t[254],char dj_ip_t[254],char dj_site_t[299])
{  

}
