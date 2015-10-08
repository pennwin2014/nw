#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "dsutl.h"
#include "dsclt.h"
#include "nwdef.h"
#include "ncdef.h"


extern char *pHashNettype;
extern char *pHashWebclass;
extern char *pHashService;
#include <iconv.h>
int getCharset(char *, int);
//int wbs_setDateTime(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

 
int GetFileAttr(char * filename)
{
	if(!filename){
		return 0;
	}

	FILE * fp = fopen(filename,"r"); //C.zip in current directory, I use it as a test  
	if(!fp){
		return 0;
	}

    int    fd = fileno(fp);  
	struct stat buf;  

    fstat(fd, &buf); 
	fclose(fp);     
		
	return buf.st_mtime;//latest modification time (seconds passed from 01/01/00:00:00 1970 UTC)  
}

//ËΩ¨Êç¢ÁºñÁ†Å
static char *convert(const char *codefrom, const char *codeto, const char *src)
{
        char    to[32] = "";
        char    *out = NULL;
        char    *dst = NULL;

        iconv_t c;
        size_t  n_in  =  strlen(src);
        size_t  n_out =  2*n_in;

        snprintf(to, sizeof(to), "%s//IGNORE", codeto);
        if( (out=(char *)calloc(n_out+1, sizeof(char))) == NULL)
        {
                printf("allocate memory for %d bytes failed\n", n_out);
                return NULL;
        }
        dst=out;

        if((c = iconv_open(to, codefrom))==(iconv_t)(-1))
        {
                printf("codefrom = %s, codeto = %s, failed\n", codefrom, codeto);
                return NULL;
        }
        iconv(c, (char **)&src, &n_in, &dst, &n_out);
        iconv_close(c);
        return out;
}



char **ncsUtlGetTable(unsigned long lTime, int days, char *prefix, unsigned long *lStartTime, int *lCount)
{
   	char sdate[33] = "";
   	strcpy(sdate, utTimFormat("%Y-%m-%d", lTime));//ÂΩìÂâçÊó•Êúü
   	strcat(sdate, " 00:00:00");
   	*lStartTime = utTimStrToLong("%Y/%m/%d %H:%M:%S", sdate) - (days-1) * 86400;
   	
   	unsigned long lStartTime1 = *lStartTime;
   	
   	int i = 0;
   	
   	char minYear[5] = "";
   	char maxYear[5] = "";
   	char minMon[3] = "";
   	char maxMon[3] = "";
   	
   	strcpy(minYear, utTimFormat("%Y", lStartTime1));
   	strcpy(maxYear, utTimFormat("%Y", lTime));
   	
   	strcpy(minMon, utTimFormat("%m", lStartTime1));
   	strcpy(maxMon, utTimFormat("%m", lTime));
   	
   	int iMin = atoi(minYear) * 12 + atoi(minMon);
   	int iMax = atoi(maxYear) * 12 + atoi(maxMon);
   	int strLen = strlen(prefix) + 7;
   	
   	*lCount = (iMax - iMin) + 1;
   	
   	 int a;
   	 int b;
   	 char **tmp = (char **)malloc(sizeof(char *)*((iMax - iMin) + 1));
   	 if(tmp != NULL)
   	 {
   	 	int j = 0;
   	 	for(i = iMin; i <= iMax; i++)
   	 	{
   	 		tmp[j] = (char *)malloc(strLen);
   	 		if(tmp[j] != NULL)
   	 		{
   	 			a = i/12;
   	 			b = i%12;
   	 			if(b == 0)
   	 			{
   	 				a--;
   	 				b = 12;
   	 			}
   	 			snprintf(tmp[j], strLen, "%s%04d%02d", prefix, a, b);
   	 		}
   	 		j++;
   	 	}	
   	 }
   	 return tmp;
}

char **ncsUtlGetTable2(char *sdate, char *edate, char *prefix, unsigned long *lStartTime, unsigned long *lEndTime, int *lCount)
{
   	char stime[15] = "";
   	char etime[15] = "";
   	
   	snprintf(stime, sizeof(stime), "%c%c%c%c%c%c%c%c000000", sdate[0], sdate[1], sdate[2], sdate[3], sdate[5], sdate[6], sdate[8], sdate[9]);
   	snprintf(etime, sizeof(stime), "%c%c%c%c%c%c%c%c235959", edate[0], edate[1], edate[2], edate[3], edate[5], edate[6], edate[8], edate[9]);
   	*lStartTime = utTimStrToLong("%Y%m%d%H%M%S", stime);
   	
   	unsigned long lTime = utTimStrToLong("%Y%m%d%H%M%S", etime);
   	*lEndTime = lTime;
   	
   	unsigned long lStartTime1 = *lStartTime;
   	
   	int i = 0;
   	
   	char minYear[5] = "";
   	char maxYear[5] = "";
   	char minMon[3] = "";
   	char maxMon[3] = "";
   	
   	strcpy(minYear, utTimFormat("%Y", lStartTime1));
   	strcpy(maxYear, utTimFormat("%Y", lTime));
   	
   	strcpy(minMon, utTimFormat("%m", lStartTime1));
   	strcpy(maxMon, utTimFormat("%m", lTime));
   	
   	int iMin = atoi(minYear) * 12 + atoi(minMon);
   	int iMax = atoi(maxYear) * 12 + atoi(maxMon);
   	int strLen = strlen(prefix) + 7;
   	
   	*lCount = (iMax - iMin) + 1;
   	
   	 int a;
   	 int b;
   	 char **tmp = (char **)malloc(sizeof(char *)*((iMax - iMin) + 1));
   	 if(tmp != NULL)
   	 {
   	 	int j = 0;
   	 	for(i = iMin; i <= iMax; i++)
   	 	{
   	 		tmp[j] = (char *)malloc(strLen);
   	 		if(tmp[j] != NULL)
   	 		{
   	 			a = i/12;
   	 			b = i%12;
   	 			if(b == 0)
   	 			{
   	 				a--;
   	 				b = 12;
   	 			}
   	 			snprintf(tmp[j], strLen, "%s%04d%02d", prefix, a, b);
   	 		}
   	 		j++;
   	 	}	
   	 }
   	 return tmp;
}

static char *squeeze(char *s, int c)
{
        int i, j;
        for(i = j = 0; s[i]!= '\0'; i++)                                                                                                                       
        {                                                                                                                                                      
                if(s[i] != c)                                                                                                                                  
                {                                                                                                                                              
                        s[j++] = s[i];                                                                                                                         
                }                                                                                                                                              
        }                                                                                                                                                      
        s[j] = '\0';                                                                                                                                           
        return s;                                                                                                                                              
}

//Êñá‰ª∂Êó•ÂøóÊü•Áúã
int lan_log_getFileLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{    
 
	char caType[32];
	int iTime_flag = 0;
	int days = 0;   
	int iret = 0;
	int _break = 0;
	int i = 0;
	int lCount = 0;
	int num = 0;
	int iNum = 0;
	
	unsigned long did = 0;
	unsigned long iStart = 0;
	unsigned long iLimit = 0;
	unsigned long lCount1 = 0;
	unsigned long lStartTime = 0;
   	unsigned long lTime = time(0);
   	unsigned long lTotRec = 0;
	unsigned long lCompid1 = 0;
	
   	char sdate[11] = "";
   	char edate[11] = "";
	char stime1[11] = "";
   	char etime1[11] = "";
   	char time_flag[2] = "";
   	char start[17] = "";
   	char limit[17] = "";
   	char sort[16] = "";
   	char dir[16] = "";
   	char sql[4096] = "";
   	char _sip[16] = "";
  
   	char _groupid[33] = "";
   	char _did[33] = "";
   	char _uid[17] = "";
   	char sql2[256] = "";
   	char frmfile[256] = "";
   	char *tmpgbk = NULL;
	char **ptmp = NULL;	
	char _keyword[33] = "";
	long iReturn;
	char caExport[12];
	
	char ip1[24]="";
	char sip1[24]="";
	char department1[16]="";
	char procname1[72]="";
	char filename1[72]="";
	char mac1[72]="";
	char compid1[32]="";
	char compid[32]="";
	char mark1[32]="";	
		FILE *fp;
		char caFilename[32];
		char caFile[128];
		char caPath[128];

	struct sttable
	{
		char tname[32];
		unsigned long lCount;
	};
	struct sttable tableinfo[120];//10Âπ¥Ôºü
   	pasDbCursor *psCur = NULL;
 
//	strcpy(caFacs,getDsFacDid());
 // strcpy(caGroups,getDsGroupid());
	
	utPltDbHead *psDbHead = utPltInitDb();
	utMsgPrintMsg(psMsgHead);

    //pasDbConn *psConn;
	
		utMsgGetSomeNVar(psMsgHead, 21,
   				     "keyword",    UT_TYPE_STRING,  sizeof(_keyword)-1,   _keyword,
   				     "uid",        UT_TYPE_STRING,  sizeof(_uid)-1,       _uid,
   				     "ip1",       UT_TYPE_STRING,  sizeof(ip1)-1,     ip1,
   				     "sip1",        UT_TYPE_STRING,  sizeof(sip1)-1,       sip1,
   				     "department1",  UT_TYPE_STRING,  sizeof(department1)-1, department1,
   				     "procname1",        UT_TYPE_STRING,  sizeof(procname1)-1,       procname1,  
   				     "filename1",    UT_TYPE_STRING,  sizeof(filename1)-1,   filename1,
   				     "compid1",		UT_TYPE_STRING, sizeof(compid1)-1, compid1,
					  "compid",		UT_TYPE_STRING, sizeof(compid)-1, compid,
					 "mac1",		UT_TYPE_STRING, sizeof(mac1)-1, mac1,
   				     "mark1",      UT_TYPE_STRING,  sizeof(mark1)-1,       mark1,   				     
   				     "sdate", 	   UT_TYPE_STRING,  sizeof(sdate)-1,       sdate,
   				     "edate", 	   UT_TYPE_STRING,  sizeof(edate)-1,       edate,
					 "stime1", 	   UT_TYPE_STRING,  sizeof(stime1)-1,       stime1,
   				     "etime1", 	   UT_TYPE_STRING,  sizeof(etime1)-1,       etime1,
   				     "time_flag",  UT_TYPE_STRING,  sizeof(time_flag)-1,   time_flag,
   				     "start",      UT_TYPE_STRING,  sizeof(start)-1,       start,
   				     "limit",      UT_TYPE_STRING,  sizeof(limit)-1,       limit,
   				     "sort",       UT_TYPE_STRING,  sizeof(sort)-1,        sort,
   				     "dir",        UT_TYPE_STRING,  sizeof(dir)-1,         dir,
   				     "export2",    UT_TYPE_STRING,  10,  caExport);
		
		char msg[256] = "";
		if(!utStrIsSpaces(department1)){
			snprintf(msg,sizeof(msg),"≤È—Ø[%s]Œƒº˛»’÷æ",lanGetGroup(psShmHead,atoi(department1)));
		}else if(!utStrIsSpaces(compid1)){
			char *compname = findCompNameById(strtoul(compid1,NULL,10));
			snprintf(msg,sizeof(msg),"≤È—Ø[%s]Œƒº˛»’÷æ",compname);
			free(compname);
		}else {
			snprintf(msg,sizeof(msg),"≤È—Ø[»´≤ø]Œƒº˛»’÷æ");
		}
	 	writeSysLog(psMsgHead,"06","Success",msg);			  
					  
	
					  
	//ÊéíÂ∫èÈªòËÆ§Êó∂Èó¥
	if(strlen(sort)==0){
 	   strcpy(sort,"stime");
	}			  
		iTime_flag = atoi(time_flag);
   	switch(iTime_flag)
   	{
		case 0://‰ªäÂ§©
   			days = 0;
			break;
   		case 1://‰ªäÂ§©
   			days = 1;
   			break;
   		case 2://ÊúÄËøë‰∏âÂ§©
   			days = 3;
   			break;
   		case 3://ÊúÄËøë‰∏ÄÂë®
   			days = 7;
   			break;
   		case 4://ÊúÄËøë‰∏ÄÊúà
   			days = 30;
   			break;
   		case 5://ÊúÄËøë‰∏âÊúà
   			days = 90;
   			break;
   		case 6://ÊúÄËøëÂçäÂπ¥
   			days = 180;
   			break;
   		default:
   			days = 1;
   			break;
   	}	
	
	printf("lTime = %lu\n",lTime);
	if(strlen(sdate) > 0 && strlen(edate) > 0)
	{
		ptmp = ncsUtlGetTable2(sdate, edate, "nwfilelog_", &lStartTime, &lTime, &lCount);
	}
	else 
	{
		ptmp = ncsUtlGetTable(lTime, days, "nwfilelog_",  &lStartTime, &lCount);
		lTime=lTime+2*3600;
	}
		printf("ptmp = %s\n",*ptmp);
		printf("days = %d\n",days);		
		printf("limit = %s\n",limit);
		printf("lTime = %lu\n",lTime);
		printf("lTime = %s\n",utTimFormat("Y/%m/%d %H:%M:%S", lTime));
		printf("lStartTime = %lu\n",lStartTime);
		printf("lStartTime = %s\n",utTimFormat("Y/%m/%d %H:%M:%S", lStartTime));
	
	
	char *procname1_gbk = convert("UTF-8", "GBK", procname1);
	char *filename1_gbk = convert("UTF-8", "GBK", filename1);
	char *ip1_gbk = convert("UTF-8", "GBK", ip1);
	printf("%s\n",procname1_gbk);
	printf("%s\n",filename1_gbk);
//	char *_keyword_gbk = convert("UTF-8", "GBK", _keyword);
	char _sdate[11] = "";
	char _edate[11] = "";


	
	printf("stime1 = %s\n",stime1);
	printf("etime1 = %s\n",etime1);
	
	
		char _stime1[11] = "";
		char _etime1[11] = "";

		int j=0;
		if(strlen(stime1) > 0 && strlen(etime1) > 0)
		{
		for(i=0;i<10;i++){	
		if(stime1[i] != 58){
		_stime1[j] = stime1[i];
		j++;
		}
		else{
		_stime1[j]="\0";
		}
		}
		printf("_stime1 = %s\n",_stime1);
	
		
		j=0;
		for(i=0;i<10;i++){	
		if(etime1[i] != 58){
		_etime1[j] = etime1[i];
		j++;
		}	else{
		_etime1[j]="\0";
		}
		}
		printf("_etime1 = %s\n",_etime1);
		
		
		sprintf(_sdate,"%s%s",utTimFormat("%Y%m%d", lStartTime),_stime1);
		printf("_sdate= %s\n",_sdate);
		lStartTime = utTimStrToLong("%Y%m%d%H%M%S", _sdate);
	
		
		sprintf(_edate,"%s%s",utTimFormat("%Y%m%d", lTime),_etime1);
		printf("_edate= %s\n",_edate);
		lTime = utTimStrToLong("%Y%m%d%H%M%S", _edate);
		printf("lStartTime = %lu\n",lStartTime);
		printf("lTime = %lu\n",lTime);
		}
		
		printf("mark1 = %s \n",mark1);
		
		char _mark1[16] = {0};
		long lmark1=0;
		int _lmark1[20];
		int k=0;
		i =0;
		j=0;
		for(i=0; i<strlen(mark1); i++){
		if(mark1[i] != 44){
		_mark1[j]=mark1[i];
		j++;
		}else{
		_mark1[j]="\0";
		lmark1 = atol(_mark1);
		_lmark1[k] = lmark1;
		k++;
		j=0;
		}
		}
		printf("lmark1 = %d\n", lmark1);
		printf("_mark1 = %s\n", _mark1);
		lmark1 = atol(_mark1);
		_lmark1[k] = lmark1;
		k++;
		printf("lmark1 = %d\n", lmark1);
		char marks[128];
		strcpy(marks,mark1);
		for(i=0; i<k; i++){
		printf("_lmark1 = %d\n", _lmark1[i]);
		if(_lmark1[i]== 8 ){
		printf("have 8\n");
		//snprintf(mark1+strlen(mark1), sizeof(mark1)-strlen(mark1), ",4104,4360,4872,5384");
		sprintf(marks,"%s%s",mark1,",4104,4360,4872,5384");
		}
		}
		printf("mark1 = %s \n",marks);
		
		
//	lCompid1 = strtoul(compid1,NULL,10);
//	printf("lCompid1 = %lu\n",lCompid1);
	char *compid1_gbk = convert("UTF-8", "GBK", compid1);
	for(i = 0; i < lCount; i++)
	{
		snprintf(frmfile, sizeof(frmfile), "/usr/local/infobright/data/ncs/%s.frm", ptmp[i]);
		
//		if(access(frmfile, F_OK) == 0)
//		{
			memset(sql, 0, sizeof(sql));
		//	snprintf(sql, sizeof(sql), "select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, content  from %s where stime >= %lu and stime <= %lu ",  ptmp[i], lStartTime, lTime);
		//	snprintf(sql, sizeof(sql), "select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, content  from %s   where stime >= %lu  and stime <= %lu limit 0,5",  ptmp[i], lStartTime, lTime);
				snprintf(sql, sizeof(sql), "select count(*) from %s where stime >= %lu and stime <= %lu ",  ptmp[i], lStartTime, lTime);
			//»®œﬁ
			char *auth = getDsGroupid();
			if(!utStrIsSpaces(auth)){
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)," and groupid in (%s)",auth);
			}
			
			//ÃÌº”∞¥≤ø√≈≤È—ØÃıº˛
			if(!utStrIsSpaces(department1) && department1[0] != '0'){
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)," and groupid in (%s)",getGroupid(strtoul(department1,NULL,10)));
			}
			
			if((!utStrIsSpaces(mark1)) && strcmp(mark1,"0"))//Êñá‰ª∂Êìç‰ΩúÁ±ªÂûã
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and mark in (%s) ", marks);
			}
			if(!utStrIsSpaces(procname1) )//ËøõÁ®ãÂêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and procname like '%%%s%%'", procname1_gbk);
			}
			if(!utStrIsSpaces(filename1) )//Êñá‰ª∂ÂêçÊü•Ë∑ØÂæÑ
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and (soupath like '%%%s%%' or destpath like '%%%s%%')", filename1_gbk, filename1_gbk);
			
			}
			if(!utStrIsSpaces(ip1) )//IP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  (inet_ntoa(sip) like '%%%s%%'  or  mac like '%%%s%%' or procname like '%%%s%%' or  mark like '%%%s%%' or devname like '%%%s%%' or soupath like '%%%s%%' or destpath like '%%%s%%')",ip1_gbk,ip1_gbk,ip1_gbk,ip1_gbk,ip1_gbk,ip1_gbk,ip1_gbk);
			
			}
			if(!utStrIsSpaces(mac1) )//mac
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  mac like '%%%s%%'", mac1);
			
			}
			if(!utStrIsSpaces(sip1) )//È´òÁ∫ßIP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  inet_ntoa(sip) like '%%%s%%'", sip1);
			
			}
			if(!utStrIsSpaces(compid1) )//ËÆ°ÁÆóÊú∫ID
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and devname like '%%%s%%'", compid1_gbk);
			
			}
			if(!utStrIsSpaces(compid) )//ËÆ°ÁÆóÊú∫ID
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and compid = %s ", compid);
			
			}
	
			
			printf("sql = %s\n", sql);
			lCount1 = 0;
			iReturn = pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &lCount1);
	printf("num = %d\n",num);	
		printf("iReturn = %d\n",iReturn);
		printf("lCount1 = %d\n",lCount1);
			if(iReturn!=1146 && lCount1 > 0)
			{
				strcpy(tableinfo[num].tname, ptmp[i]);
				tableinfo[num].lCount = lCount1;
				num++;
			}
			printf("num = %d\n",num);
//		}
		free(ptmp[i]);
	}
	free(ptmp);	
	for(i = 0; i < num; i++)
	{
		lTotRec += tableinfo[i].lCount;
	}
	utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
	printf("lTotRec = %d\n",lTotRec);
	//
	for(i = num-1; i >= 0; i--)
	{
		printf("table[%d] = %s, _lCount1[%d] = %lu\n", i, tableinfo[i].tname, i, tableinfo[i].lCount);
	}
	//
	
	iNum = 0;
//	iStart = atol(start);
//	iLimit = atol(limit);
	
		if(strlen(caExport)>0){
		
		iStart=0;
		iLimit=99999;
			//	printf("ÂØºÂá∫Êñá‰ª∂Ë∑ØÂæÑ11111111111\n");
		        sprintf(caFilename,"lanfilelog%d.csv",utStrGetId());
		        sprintf(caPath,"%s/html","/home/ncmysql/ncs");
		        sprintf(caFile,"%s/%s",caPath,caFilename);
		    	fp = fopen(caFile,"w");
		        if(fp == NULL) {
		            pasDbCloseCursor(psCur);
					printf("ÂØºÂá∫Âá∫Èîô\n");
		           utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546Êï∞ÊçÆÂØºÂá∫Âá∫Èîô"),ncLang("0546Êï∞ÊçÆÂØºÂá∫Âá∫Èîô"));
		            return 0;
		        }
		        
		        fprintf(fp,"Œƒº˛≤Ÿ◊˜»’÷æ\n");
		        ncUtlPrintCsv(fp,9,
		                           UT_TYPE_STRING,"≤Ÿ◊˜¿‡–Õ",
		                           UT_TYPE_STRING," ±º‰",
		                           UT_TYPE_STRING,"≤ø√≈",
		                           UT_TYPE_STRING,"”√ªß",
		                           UT_TYPE_STRING,"IP",
		                           UT_TYPE_STRING,"Œƒº˛√˚",
		                           UT_TYPE_STRING,"Ω¯≥Ã√˚",
								   UT_TYPE_STRING,"‘¥¬∑æ∂",
								   UT_TYPE_STRING,"ƒøµƒ¬∑æ∂");
		                           
	}	else{
	//	printf("ÂØºÂá∫Êñá‰ª∂Ë∑ØÂæÑ222222222\n");
	iStart = atol(start);
	iLimit = atol(limit);
  }
	printf("num = %d\n",num);
  
	for(i = num-1; i >= 0; i--)
	{
	printf("999999999999999\n");
	memset(sql, 0, sizeof(sql));
	if(tableinfo[i].lCount <= iStart)
		{	
			iStart-=tableinfo[i].lCount;
			continue;
		}	
   // strcpy(sql,"select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, content from nwfilelog_201309 limit 0,5");
   	snprintf(sql, sizeof(sql), "select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, %s.content,gname, mac,gid from %s  left join nwgroup on(groupid = gid) where stime >= %lu  and stime <= %lu ", tableinfo[i].tname, tableinfo[i].tname, lStartTime, lTime);
	if((!utStrIsSpaces(department1)) && (strcmp(department1,"0") !=0) )//ÈÉ®Èó®
			{
				printf("department1 = %s\n",department1);
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and groupid in (%s) ", department1);
			}
			if((!utStrIsSpaces(mark1)) && strcmp(mark1,"0"))//Êñá‰ª∂Êìç‰ΩúÁ±ªÂûã
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and mark in (%s) ", marks);
			}
			if(!utStrIsSpaces(procname1) )//ËøõÁ®ãÂêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and procname like '%%%s%%'", procname1);
			}
			if(!utStrIsSpaces(filename1) )//Êñá‰ª∂ÂêçÊü•Ë∑ØÂæÑ
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and (soupath like '%%%s%%' or destpath like '%%%s%%')", filename1, filename1);
			
			}
			if(!utStrIsSpaces(ip1) )//IP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and (inet_ntoa(sip) like '%%%s%%'  or  mac like '%%%s%%' or procname like '%%%s%%' or  mark like '%%%s%%' or devname like '%%%s%%' or soupath like '%%%s%%' or destpath like '%%%s%%')",ip1_gbk,ip1_gbk,ip1_gbk,ip1_gbk,ip1_gbk,ip1_gbk,ip1_gbk);
			
			}
			if(!utStrIsSpaces(mac1) )//mac
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and mac like '%%%s%%'", mac1);
			
			}
			if(!utStrIsSpaces(sip1) )//È´òÁ∫ßIP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and inet_ntoa(sip) like '%%%s%%'", sip1);
			
			}
			if(!utStrIsSpaces(compid1) )//ËÆ°ÁÆóÊú∫ID
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and devname like '%%%s%%'", compid1);
			
			}
			if(!utStrIsSpaces(compid) )//ËÆ°ÁÆóÊú∫ID
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and compid = %s ", compid);
			
			}
			snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " order by %s %s limit %lu, %lu", sort, dir, iStart, iLimit < tableinfo[i].lCount ? iLimit : tableinfo[i].lCount);
		//snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " limit %lu, %lu", iStart, iLimit < tableinfo[i].lCount ? iLimit : tableinfo[i].lCount);		
		printf("__line = %d, sql = %s\n", __LINE__, sql);
			if( tableinfo[i].lCount > iStart && tableinfo[i].lCount < iStart+iLimit )
		{
			iLimit = iLimit -  (tableinfo[i].lCount-iStart);
			iStart = 0;
		}
		else 
		{
			_break = 1;
		}
	//tmpgbk = convert("UTF-8", "GBK", sql);
	//	if(tmpgbk == NULL)
	//		continue;
	//	printf("__line = %d, sql = %s\n", __LINE__, tmpgbk);
		psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
		{
				
			long long sid = 0;
			char  mark[12]="";	
			char  _mark[12]="";	
			unsigned int stime =0;
			char _stime[40] = "";//ËΩ¨Êç¢Êó∂Èó¥
			char  procname[72] ="";
			unsigned long department = 0;
			char devname[32] ="";
			char udisp[32] ="";
			char sip[24] ="";
			char soupath[256] ="";
			char destpath[256] ="";
			char content[256] ="";
			char gname[24]="";
			char mac[24]="";
			printf("1111\n");
			
			while(0==(iret = pasDbFetchInto(psCur,
					 UT_TYPE_LONG8,  8,               	&sid,
					 UT_TYPE_STRING, sizeof(mark)-1, 	mark,
					 UT_TYPE_ULONG,  8,              	&stime,
					 UT_TYPE_ULONG, 8,  &department,
					 UT_TYPE_STRING, sizeof(devname)-1,  devname,
					 UT_TYPE_STRING, sizeof(sip)-1,  	sip,
					 UT_TYPE_STRING, sizeof(procname)-1,   procname,
					 UT_TYPE_STRING, sizeof(soupath)-1,      soupath,
					 UT_TYPE_STRING, sizeof(destpath)-1,      destpath,
					 UT_TYPE_STRING, sizeof(content)-1 , content,
					 UT_TYPE_STRING, sizeof(gname)-1 , gname,
					 UT_TYPE_STRING, sizeof(mac)-1 , mac))  || 1405==iret)
			{
				iNum++;
				if(iNum > 1)
				{
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			//Ê†πÊçÆË∑ØÂæÑÂèñÊñá‰ª∂Âêç	
			int i =0;
			char _soupath[256] ="";
			char filename[32]="";
			char _filename[32]="";
			for(i=0;i<strlen(soupath);i++){
			//printf("%c\n",content[k-i-1]);
			_soupath[i]=soupath[strlen(soupath)-i-1];
			}
			sscanf(_soupath, "%[^/]", filename);
			for(i=0;i<strlen(filename);i++){
			_filename[i]=filename[strlen(filename)-i-1];
			}
			//Êñá‰ª∂Êìç‰ΩúÁ±ªÂûãËΩ¨Êç¢
		//	printf("%s\n",mark);
		//	printf("%d\n",atoi(mark));
			//int k = 3;
			switch(atoi(mark)){
			case 0:strcpy(_mark,"»´≤ø"); break;
			case 1:strcpy(_mark,"¥Úø™"); break;
			case 2:strcpy(_mark,"∏ƒ√˚"); break;
			case 3:strcpy(_mark,"…æ≥˝"); break;
			case 4:strcpy(_mark,"–ﬁ∏ƒ"); break;
			case 5:strcpy(_mark,"¥¥Ω®"); break;
			case 8:strcpy(_mark,"∏¥÷∆"); break;
			case 4104:strcpy(_mark,"±æª˙∏¥÷∆"); break;	
			case 4360:strcpy(_mark,"∏¥÷∆µΩU≈Ã"); break;
		//	case 4616:strcpy(_mark,"U≈Ã∏¥÷∆µΩ±æª˙"); break;
			case 4872:strcpy(_mark,"∏¥÷∆µΩπ≤œÌƒø¬º"); break;
		//	case 5128:strcpy(_mark,"Õ¯¬Á∏¥÷∆µΩ±æª˙"); break;
			case 5384:strcpy(_mark,"∏¥÷∆µΩ‘∆≈Ã"); break;
			case 5640:strcpy(_mark,"‘∆≈Ã∏¥÷∆µΩ±æª˙"); break;
			default:	
			break;
			}
	
				
				
				utPltPutLoopVarF(psDbHead,"sid",iNum,"%lld",sid);
				utPltPutLoopVarF(psDbHead,"mark",iNum,"%s",_mark);
				strcpy(_stime, utTimFormat("%Y/%m/%d %H:%M:%S", stime));//ËΩ¨Êç¢Êó∂Èó¥
				utPltPutLoopVarF(psDbHead,"stime",iNum,"%s",_stime);
				utPltPutLoopVarF(psDbHead,"department",iNum,"%lu",department);
				utPltPutLoopVarF(psDbHead,"devname",iNum,"%s",devname);
				utPltPutLoopVarF(psDbHead,"sip",iNum,"%s",sip);
				utPltPutLoopVarF(psDbHead,"filename",iNum,"%s",_filename);//Êñá‰ª∂Âêç	
				utPltPutLoopVarF(psDbHead, "procname", iNum, "%s", procname);//Á®ãÂ∫èÂêç
				utPltPutLoopVarF(psDbHead, "soupath", iNum, "%s", soupath);
				utPltPutLoopVarF(psDbHead, "destpath", iNum, "%s", destpath);
				utPltPutLoopVarF(psDbHead, "content", iNum, "%s", content);
				if(department == 0){
				
				utPltPutLoopVar(psDbHead, "gname", iNum,"»´≤ø");
				}
				else{
				utPltPutLoopVarF(psDbHead, "gname", iNum, "%s", gname);
				}
				utPltPutLoopVarF(psDbHead, "mac", iNum, "%s", mac);
			
					if(strlen(caExport)>0){
					 ncUtlPrintCsv(fp,10,
                          UT_TYPE_STRING, _mark,
                          UT_TYPE_STRING,_stime,
                          UT_TYPE_STRING,gname,
                          UT_TYPE_STRING,devname,
                          UT_TYPE_STRING,sip,
                          UT_TYPE_STRING,_filename,
						   UT_TYPE_STRING,procname,
						    UT_TYPE_STRING,soupath,
							 UT_TYPE_STRING,destpath);
					 }	
				 
			
			}
			pasDbCloseCursor(psCur);
			}
			free(tmpgbk);
			if(_break == 1)
			break; 
		}	
		//	utPltShowDb(psDbHead);
			//	free(_keyword_gbk);
				
		//	printf("22222\n");
		//	pasDbCloseCursor(psCur);
		 if(strlen(caExport)>0){
	 	    fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        utPltFreeDb(psDbHead);
        return 0;
	 }
	
			int tmp=utPltOutToHtml(iFd,psMsgHead,psDbHead,"v4/neiwang/rzck/lan_log_file.htm");
			printf("temp=%d\n",tmp);
			printf("iTime_flag=%d\n",iTime_flag);
		
			
	//	pasDbClose(psConn);
    return 0;
}

//Â±èÂπïÊó•ÂøóÊü•Áúã
int lan_log_getScreenLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{    
 
	char caType[32];
	int iTime_flag = 0;
	int days = 0;   
	int iret = 0;
	int _break = 0;
	int i = 0;
	int lCount = 0;
	int num = 0;
	int iNum = 0;
	
	unsigned long did = 0;
	unsigned long iStart = 0;
	unsigned long iLimit = 0;
	unsigned long lCount1 = 0;
	unsigned long lStartTime = 0;
   	unsigned long lTime = time(0);
   	unsigned long lTotRec = 0;
	unsigned long lCompid1 = 0;
   	char sdate[11] = "";
   	char edate[11] = "";
   	char time_flag[2] = "";
   	char start[17] = "";
   	char limit[17] = "";
   	char sort[16] = "";
   	char dir[16] = "";
   	char sql[4096] = "";
   	char _sip[16] = "";
  
   	char _groupid[33] = "";
   	char _did[33] = "";
   	char _uid[17] = "";
   	char sql2[256] = "";
   	char frmfile[256] = "";
   	char *tmpgbk = NULL;
	char **ptmp = NULL;	
	char _keyword[33] = "";
	long iReturn;
	char caExport[12];
	
	char ip1[24]="";
	char sip1[24]="";
	char department1[32]="";
	char mac1[24]="";
	char compid1[32]="";
	char udisp1[32]="";	
	char image1[80] = "";
		FILE *fp2;
		char caFilename[32];
		char caFile[128];
		char caPath[128];

	struct sttable
	{
		char tname[32];
		unsigned long lCount;
	};
	struct sttable tableinfo[120];//10Âπ¥Ôºü
   	pasDbCursor *psCur = NULL;
 
//	strcpy(caFacs,getDsFacDid());
 // strcpy(caGroups,getDsGroupid());
	
	utPltDbHead *psDbHead = utPltInitDb();
	utMsgPrintMsg(psMsgHead);

    //pasDbConn *psConn;
	
		utMsgGetSomeNVar(psMsgHead, 17,
   				     "keyword",    UT_TYPE_STRING,  sizeof(_keyword)-1,   _keyword,
   				     "uid",        UT_TYPE_STRING,  sizeof(_uid)-1,       _uid,
   				     "ip1",       UT_TYPE_STRING,  sizeof(ip1)-1,     ip1,
   				     "sip1",        UT_TYPE_STRING,  sizeof(sip1)-1,       sip1,
   				     "department1",  UT_TYPE_STRING,  sizeof(department1)-1, department1,
   				     "mac1",        UT_TYPE_STRING,  sizeof(mac1)-1,       mac1,  
   				     "compid1",    UT_TYPE_STRING,  sizeof(compid1)-1,   compid1,
					 "image1",    UT_TYPE_STRING,  sizeof(image1)-1,   image1,   				     
   				     "udisp1",        UT_TYPE_STRING,  sizeof(udisp1)-1,        udisp1,   				     
   				     "sdate", 	   UT_TYPE_STRING,  sizeof(sdate)-1,       sdate,
   				     "edate", 	   UT_TYPE_STRING,  sizeof(edate)-1,       edate,
   				     "time_flag",  UT_TYPE_STRING,  sizeof(time_flag)-1,   time_flag,
   				     "start",      UT_TYPE_STRING,  sizeof(start)-1,       start,
   				     "limit",      UT_TYPE_STRING,  sizeof(limit)-1,       limit,
   				     "sort",       UT_TYPE_STRING,  sizeof(sort)-1,        sort,
   				     "dir",        UT_TYPE_STRING,  sizeof(dir)-1,         dir,
   				     "export2",    UT_TYPE_STRING,  sizeof(caExport)-1,  caExport);
   				     
		char msg[256] = "";
		if(!utStrIsSpaces(department1)){
			snprintf(msg,sizeof(msg),"≤È—Ø[%s]∆¡ƒª»’÷æ",lanGetGroup(psShmHead,atoi(department1)));
		}else if(!utStrIsSpaces(compid1)){
			char *compname = findCompNameById(strtoul(compid1,NULL,10));
			snprintf(msg,sizeof(msg),"≤È—Ø[%s]∆¡ƒª»’÷æ",compname);
			free(compname);
		}else {
			snprintf(msg,sizeof(msg),"≤È—Ø[»´≤ø]∆¡ƒª»’÷æ");
		}
	 	writeSysLog(psMsgHead,"06","Success",msg);
	 	
	 	
	//ÊéíÂ∫èÈªòËÆ§Êó∂Èó¥
	if(strlen(sort)==0){
 	   strcpy(sort,"stime");
	}			  
		iTime_flag = atoi(time_flag);
   	switch(iTime_flag)
   	{
   		case 1://‰ªäÂ§©
   			days = 1;
   			break;
   		case 2:days = 3;
   			break;
   		case 3:days = 7;
   			break;
   		case 4:days = 30;
   			break;
   		case 5:days = 90;
   			break;
   		case 6:days = 180;
   			break;
   		default:
   			days = 1;
   			break;
   	}	
	
	if(strlen(sdate) > 0 && strlen(edate) > 0)
	{
		ptmp = ncsUtlGetTable2(sdate, edate, "ncscreenlog_", &lStartTime, &lTime, &lCount);
	}
	else 
	{
		ptmp = ncsUtlGetTable(lTime, days, "ncscreenlog_",  &lStartTime, &lCount);
		lTime=lTime+2*3600;
	}
		
	lCompid1 = strtoul(compid1,NULL,10);
	
	char *ip1_gbk = convert("UTF-8", "GBK", ip1);
	char _sdate[11] = "";
	char _edate[11] = "";
	strcpy(_sdate, utTimFormat("%Y/%m/%d", lStartTime));
	strcpy(_edate, utTimFormat("%Y/%m/%d", lTime));
			char *auth = getDsGroupid();
	for(i = 0; i < lCount; i++)
	{
		snprintf(frmfile, sizeof(frmfile), "/usr/local/infobright/data/ncs/%s.frm", ptmp[i]);
		

			memset(sql, 0, sizeof(sql));
			snprintf(sql, sizeof(sql), "select count(*) from %s where stime >= %lu and stime <= %lu ",  ptmp[i], lStartTime, lTime);
			//»®œﬁ
			if(!utStrIsSpaces(auth)){
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)," and groupid in (%s)",auth);
			}
			
			//ÃÌº”∞¥≤ø√≈≤È—ØÃıº˛
			if(!utStrIsSpaces(department1) && department1[0] != '0'){
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)," and groupid in (%s)",getGroupid(strtoul(department1,NULL,10)));
			}
		
			if(!utStrIsSpaces(mac1) )//ËøõÁ®ãÂêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and mac like '%%%s%%'", mac1);
			}
			if(!utStrIsSpaces(compid1) )//ËÆ°ÁÆóÊú∫ÂêçÊü•Ë∑ØÂæÑ
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and compid = %lu ", lCompid1);
			
			}
			if(!utStrIsSpaces(ip1) )//IP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  (inet_ntoa(sip) like '%%%s%%' or mac like '%%%s%%')", ip1_gbk, ip1_gbk);
			
			}
			if(!utStrIsSpaces(sip1) )//È´òÁ∫ßIP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  inet_ntoa(sip) like '%%%s%%'", sip1);
			
			}
	
			
			printf("sql = %s\n", sql);
			lCount1 = 0;
			iReturn = pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &lCount1);

			if(iReturn!=1146 && lCount1 > 0)
			{
				strcpy(tableinfo[num].tname, ptmp[i]);
				tableinfo[num].lCount = lCount1;
				num++;
			}
//		}
		free(ptmp[i]);
	}
	free(ptmp);	
	for(i = 0; i < num; i++)
	{
		lTotRec += tableinfo[i].lCount;
	}
	utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
	//
	
	iNum = 0;
//	iStart = atol(start);
//	iLimit = atol(limit);
	
		if(strlen(caExport)>0){
		
		iStart=0;
		iLimit=99999;
		        sprintf(caFilename,"lanscreenlog%d.csv",utStrGetId());
		        sprintf(caPath,"%s/html","/home/ncmysql/ncs");
		        sprintf(caFile,"%s/%s",caPath,caFilename);
		    	printf("caFile = %s\n",caFile);
				
				fp2 = fopen(caFile,"w");
				
		        if(fp2 == NULL) {
		            pasDbCloseCursor(psCur);
		            return 0;
		        }	
		        fprintf(fp2,"∆¡ƒª»’÷æ\n");
		        ncUtlPrintCsv(fp2,7,	
									UT_TYPE_STRING,"≤ø√≈",
									UT_TYPE_STRING,"µÁƒ‘",
									UT_TYPE_STRING,"”√ªß",
									UT_TYPE_STRING,"IP",
									UT_TYPE_STRING,"MAC",
									UT_TYPE_STRING," ±º‰",
									UT_TYPE_STRING,"Õº∆¨");
				
						
		                           
	}	else{
	iStart = atol(start);
	iLimit = atol(limit);
  }
  
	for(i = num-1; i >= 0; i--)
	{
	memset(sql, 0, sizeof(sql));
	if(tableinfo[i].lCount <= iStart)
		{	
			iStart-=tableinfo[i].lCount;
			continue;
		}	
   // strcpy(sql,"select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, content from nwfilelog_201309 limit 0,5");
   	snprintf(sql, sizeof(sql), "select sid, %s.groupid, %s.compid, udisp, inet_ntoa(sip), %s.mac, %s.stime, file, gname, nwcompute.compname, gid  from %s  left join nwgroup on (gid = %s.groupid) left join nwcompute on (nwcompute.compid = %s.compid)  where %s.stime >= %lu  and %s.stime <= %lu ", tableinfo[i].tname,tableinfo[i].tname,tableinfo[i].tname,tableinfo[i].tname, tableinfo[i].tname, tableinfo[i].tname, tableinfo[i].tname,tableinfo[i].tname,lStartTime,tableinfo[i].tname, lTime);
  // snprintf(sql, sizeof(sql), "select sid, groupid, compid, udisp, inet_ntoa(sip), mac, stime, file  from %s ", tableinfo[i].tname);
  	if(!utStrIsSpaces(auth)){
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)," and %s.groupid in (%s)",tableinfo[i].tname,auth);
			}
		if((!utStrIsSpaces(department1)) && (strcmp(department1,"0") !=0) )//ÈÉ®Èó®
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and %s.groupid in (%s) ", tableinfo[i].tname,getGroupid(strtoul(department1,NULL,10)));
			}
		
			if(!utStrIsSpaces(mac1) )//ËøõÁ®ãÂêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and %s.mac like '%%%s%%'", tableinfo[i].tname,mac1);
			}
			if(!utStrIsSpaces(compid1) )//ËÆ°ÁÆóÊú∫ÂêçÊü•Ë∑ØÂæÑ
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and %s.compid = %lu ", tableinfo[i].tname,lCompid1);
			
			}
			if(!utStrIsSpaces(ip1) )//IP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  (inet_ntoa(sip) like '%%%s%%' or %s.mac like '%%%s%%')", ip1_gbk, tableinfo[i].tname,ip1_gbk);
			
			}
			if(!utStrIsSpaces(sip1) )//È´òÁ∫ßIP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  inet_ntoa(sip) like '%%%s%%'", sip1);
			
			}
		snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " order by %s %s limit %lu, %lu", sort, dir, iStart, iLimit < tableinfo[i].lCount ? iLimit : tableinfo[i].lCount);
	//	snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " limit %lu, %lu", iStart, iLimit < tableinfo[i].lCount ? iLimit : tableinfo[i].lCount);		
		printf("__line = %d, sql = %s\n", __LINE__, sql);
			if( tableinfo[i].lCount > iStart && tableinfo[i].lCount < iStart+iLimit )
		{
			iLimit = iLimit -  (tableinfo[i].lCount-iStart);
			iStart = 0;
		}
		else 
		{
			_break = 1;
		}
//	tmpgbk = convert("UTF-8", "GBK", sql);
	//	if(tmpgbk == NULL)
	//		continue;
	//	printf("__line = %d, sql = %s\n", __LINE__, tmpgbk);
		psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
		{
			
			long long sid = 0;
			char  mark[12]="";	
			char  _mark[12]="";	
			unsigned int stime =0;
			char _stime[40] = "";//ËΩ¨Êç¢Êó∂Èó¥
			char compid[72] ="";
			unsigned long department = 0;
			char udisp[32] ="";
			char mac[32] ="";
			char sip[24] ="";
			char file[100] ="";
			char _file[200] ="";
			char gname[24] ="";
			char devname[32] = "";
						
			while(0==(iret = pasDbFetchInto(psCur,
					 UT_TYPE_LONG8,  8,               	&sid,
					 UT_TYPE_STRING, 8,  &department,
					 UT_TYPE_STRING, sizeof(compid)-1, 	compid,
					 UT_TYPE_STRING, sizeof(udisp)-1,  udisp,
					 UT_TYPE_STRING, sizeof(sip)-1,  	sip,
					 UT_TYPE_STRING, sizeof(mac)-1,   mac,
					 UT_TYPE_ULONG,  8,              	&stime,
					 UT_TYPE_STRING, sizeof(file)-1,      file,
					  UT_TYPE_STRING, sizeof(gname)-1,     gname,
					  UT_TYPE_STRING, sizeof(devname)-1,     devname
					))  || 1405==iret)
			{
				iNum++;
				if(iNum > 1)
				{
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			
				
			
			
				utPltPutLoopVarF(psDbHead,"sid",iNum,"%lld",sid);
				utPltPutLoopVarF(psDbHead,"gname",iNum,"%lu",department);
				utPltPutLoopVarF(psDbHead,"compid",iNum,"%s",compid);
				utPltPutLoopVarF(psDbHead,"udisp",iNum,"%s",udisp);//Êñá‰ª∂Âêç	
				utPltPutLoopVarF(psDbHead,"sip",iNum,"%s",sip);
				utPltPutLoopVarF(psDbHead, "mac", iNum, "%s", mac);
				strcpy(_stime, utTimFormat("%m/%d %H:%M:%S", stime));//ËΩ¨Êç¢Êó∂Èó¥
				utPltPutLoopVarF(psDbHead,"stime",iNum,"%s",_stime);
			
			//	sprintf(caPath,"%s/screenJPG","/home/ncmysql/nw");
		    //    sprintf(_file,"%s/%s",caPath,file);
				//utPltFileDownload(iFd,"image/jpeg",caPath,caFilename,caFilename);
				
				utPltPutLoopVarF(psDbHead, "file", iNum, "%s", file);
				utPltPutLoopVarF(psDbHead, "department", iNum, "%s", gname);
				utPltPutLoopVarF(psDbHead, "devname", iNum, "%s", devname);
		//	printf("1111\n");
			//	_mark_gbk = convert("UTF-8", "GBK", _mark);
			char *_gname_UTF8 = convert("GBK","UTF-8", gname);
			char *_devname_UTF8 = convert("GBK","UTF-8", devname);
				
						if(strlen(caExport)>0){
			
			ncUtlPrintCsv(fp2,7,
							UT_TYPE_STRING,_gname_UTF8,
							UT_TYPE_STRING,_devname_UTF8,	
							UT_TYPE_STRING,udisp,
							UT_TYPE_STRING,sip,
							UT_TYPE_STRING,mac,
							UT_TYPE_STRING,_stime,
							UT_TYPE_STRING,file
							);
					 }	
				 
			
			}
			pasDbCloseCursor(psCur);
			}
			free(tmpgbk);
			if(_break == 1)
			break; 
			}	
		
		if(strlen(caExport)>0){
	 	fclose(fp2);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
      //  remove(caFile);
        utPltFreeDb(psDbHead);
        return 0;
	 }
			
			int tmp=utPltOutToHtml(iFd,psMsgHead,psDbHead,"v4/neiwang/rzck/lan_log_screen.htm");
			
		
			
	//	pasDbClose(psConn);
    return 0;
}

//ÂëäË≠¶Êó•ÂøóÊü•Áúã
int lan_log_getAlarmLog(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{    
 
	char caType[32];
	int iTime_flag = 0;
	int days = 0;   
	int iret = 0;
	int _break = 0;
	int i = 0;
	int lCount = 0;
	int num = 0;
	int iNum = 0;
	
	unsigned long did = 0;
	unsigned long iStart = 0;
	unsigned long iLimit = 0;
	unsigned long lCount1 = 0;
	unsigned long lStartTime = 0;
   	unsigned long lTime = time(0);
   	unsigned long lTotRec = 0;
	unsigned long lCompid1 = 0;
	
   	char sdate[11] = "";
   	char edate[11] = "";
   	char time_flag[2] = "";
   	char start[17] = "";
   	char limit[17] = "";
   	char sort[16] = "";
   	char dir[16] = "";
   	char sql[4096] = "";
   	char _sip[16] = "";
  
   	char _groupid[33] = "";
   	char _did[33] = "";
   	char _uid[17] = "";
   	char sql2[256] = "";
   	char frmfile[256] = "";
   	char *tmpgbk = NULL;
	char **ptmp = NULL;	
	char **ptmp2 = NULL;	
	char _keyword[33] = "";
	long iReturn;
	char caExport[12];
	
	char ip1[24]="";
	char sip1[24]="";
	char department1[16]="";
	char compid1[24]="";
	char pid1[32]="";
	char mac1[32]="";
	char devname1[32]="";	
		FILE *fp;
		char caFilename[32];
		char caFile[128];
		char caPath[128];

	struct sttable
	{
		char tname[32];
		unsigned long lCount;
	};
	struct sttable tableinfo[120];//10Âπ¥Ôºü
	
	struct sttable2
	{
		char tname[32];
		unsigned long lCount;
	};
	struct sttable2 tableinfo2[120];//10Âπ¥Ôºü
	
   	pasDbCursor *psCur = NULL;
 
//	strcpy(caFacs,getDsFacDid());
 // strcpy(caGroups,getDsGroupid());
	
	utPltDbHead *psDbHead = utPltInitDb();
	utMsgPrintMsg(psMsgHead);

    //pasDbConn *psConn;
	
		utMsgGetSomeNVar(psMsgHead, 17,
   				     "keyword",    UT_TYPE_STRING,  sizeof(_keyword)-1,   _keyword,
   				     "uid",        UT_TYPE_STRING,  sizeof(_uid)-1,       _uid,
   				     "ip1",       UT_TYPE_STRING,  sizeof(ip1)-1,     ip1,
   				     "sip1",        UT_TYPE_STRING,  sizeof(sip1)-1,       sip1,
   				     "department1",  UT_TYPE_STRING,  sizeof(department1)-1, department1,
   				     "pid1",        UT_TYPE_STRING,  sizeof(pid1)-1,       pid1,  
   				     "mac1",    UT_TYPE_STRING,  sizeof(mac1)-1,   mac1,
   				     "devname1",   UT_TYPE_STRING,  sizeof(devname1)-1,    devname1, 
					"compid1",   UT_TYPE_STRING,  sizeof(compid1)-1,    compid1,   		
   				     "sdate", 	   UT_TYPE_STRING,  sizeof(sdate)-1,       sdate,
   				     "edate", 	   UT_TYPE_STRING,  sizeof(edate)-1,       edate,
   				     "time_flag",  UT_TYPE_STRING,  sizeof(time_flag)-1,   time_flag,
   				     "start",      UT_TYPE_STRING,  sizeof(start)-1,       start,
   				     "limit",      UT_TYPE_STRING,  sizeof(limit)-1,       limit,
   				     "sort",       UT_TYPE_STRING,  sizeof(sort)-1,        sort,
   				     "dir",        UT_TYPE_STRING,  sizeof(dir)-1,         dir,
   				     "export2",    UT_TYPE_STRING,  10,  caExport);
   				        				     
   	char msg[256] = "";
		if(!utStrIsSpaces(department1)){
			snprintf(msg,sizeof(msg),"≤È—Ø[%s]∏ÊæØ»’÷æ",lanGetGroup(psShmHead,atoi(department1)));
		}else if(!utStrIsSpaces(compid1)){
			char *compname = findCompNameById(strtoul(compid1,NULL,10));
			snprintf(msg,sizeof(msg),"≤È—Ø[%s]∏ÊæØ»’÷æ",compname);
			free(compname);
		}else {
			snprintf(msg,sizeof(msg),"≤È—Ø[»´≤ø]∏ÊæØ»’÷æ");
		}
	 	writeSysLog(psMsgHead,"06","Success",msg);
					  
	//ÊéíÂ∫èÈªòËÆ§Êó∂Èó¥
	if(strlen(sort)==0){
 	   strcpy(sort,"stime");
	}			  
		iTime_flag = atoi(time_flag);
   	switch(iTime_flag)
   	{
   		case 1://‰ªäÂ§©
   			days = 1;
   			break;
   		case 2://ÊúÄËøë‰∏âÂ§©
   			days = 3;
   			break;
   		case 3://ÊúÄËøë‰∏ÄÂë®
   			days = 7;
   			break;
   		case 4://ÊúÄËøë‰∏ÄÊúà
   			days = 30;
   			break;
   		case 5://ÊúÄËøë‰∏âÊúà
   			days = 90;
   			break;
   		case 6://ÊúÄËøëÂçäÂπ¥
   			days = 180;
   			break;
   		default:
   			days = 1;
   			break;
   	}	
	
	printf("lTime = %lu\n",lTime);
	if(strlen(sdate) > 0 && strlen(edate) > 0)
	{
		ptmp = ncsUtlGetTable2(sdate, edate, "nwwarnlog_", &lStartTime, &lTime, &lCount);
		ptmp2 = ncsUtlGetTable2(sdate, edate, "nwwarnscreen_", &lStartTime, &lTime, &lCount);
	}
	else 
	{
		ptmp = ncsUtlGetTable(lTime, days, "nwwarnlog_",  &lStartTime, &lCount);
		ptmp2 = ncsUtlGetTable(lTime, days, "nwwarnscreen_",  &lStartTime, &lCount);
		lTime=lTime+2*3600;
	}
		printf("ptmp = %s\n",*ptmp);
		printf("ptmp2 = %s\n",*ptmp2);
		printf("days = %d\n",days);		
		printf("limit = %s\n",limit);
		printf("lTime = %lu\n",lTime);
		printf("lTime = %s\n",utTimFormat("%Y/%m/%d", lTime));
		printf("lStartTime = %lu\n",lStartTime);
		printf("lStartTime = %s\n",utTimFormat("%Y/%m/%d", lStartTime));
		
		

	//char *procname1_gbk = convert("UTF-8", "GBK", procname1);
	//char *filename1_gbk = convert("UTF-8", "GBK", filename1);
//	printf("%s\n",procname1_gbk);
//	printf("%s\n",filename1_gbk);
//	char *_keyword_gbk = convert("UTF-8", "GBK", _keyword);
	char _sdate[11] = "";
	char _edate[11] = "";
	strcpy(_sdate, utTimFormat("%Y/%m/%d", lStartTime));
	strcpy(_edate, utTimFormat("%Y/%m/%d", lTime));
	printf("_sdate= %s\n",_sdate);
	printf("_edate= %s\n",_edate);
	
	printf("pid1 = %s\n",pid1);
	char *pid1_gbk = convert("UTF-8", "GBK", pid1);
	char *ip1_gbk = convert("UTF-8", "GBK", ip1);
	char *devname1_gbk = convert("UTF-8", "GBK", devname1);
	printf("pid1_gbk = %s\n",pid1_gbk);
	lCompid1 = strtoul(compid1,NULL,10);
	printf("lCompid1 = %lu\n",lCompid1);
	for(i = 0; i < lCount; i++)
	{
		snprintf(frmfile, sizeof(frmfile), "/usr/local/infobright/data/ncs/%s.frm", ptmp[i]);
		snprintf(frmfile, sizeof(frmfile), "/usr/local/infobright/data/ncs/%s.frm", ptmp2[i]);
		
//		if(access(frmfile, F_OK) == 0)
//		{
			memset(sql, 0, sizeof(sql));
		//	snprintf(sql, sizeof(sql), "select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, content  from %s where stime >= %lu and stime <= %lu ",  ptmp[i], lStartTime, lTime);
		//	snprintf(sql, sizeof(sql), "select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, content  from %s   where stime >= %lu  and stime <= %lu limit 0,5",  ptmp[i], lStartTime, lTime);
		
				snprintf(sql, sizeof(sql), "select count(*) from %s left join nwmainprocy on (nwmainprocy.sid = %s.pid) where stime >= %lu and stime <= %lu ",  ptmp[i], ptmp[i], lStartTime, lTime);
			//»®œﬁ
			char *auth = getDsGroupid();
			if(!utStrIsSpaces(auth)){
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)," and groupid in (%s)",auth);
			}
			
			//ÃÌº”∞¥≤ø√≈≤È—ØÃıº˛
			if(!utStrIsSpaces(department1) && department1[0] != '0'){
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql)," and groupid in (%s)",getGroupid(strtoul(department1,NULL,10)));
			}
		
		
			if((!utStrIsSpaces(mac1)) && strcmp(mac1,"0"))//Êñá‰ª∂Êìç‰ΩúÁ±ªÂûã
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and mac like '%%%s%%'", mac1);
			}
			if(!utStrIsSpaces(pid1) )//Ëß¶ÂèëÁ≠ñÁï•
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and lawname like '%%%s%%'", pid1_gbk);
			}
			if(!utStrIsSpaces(devname1) )//Êñá‰ª∂ÂêçÊü•Ë∑ØÂæÑ
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and devname like '%%%s%%'", devname1_gbk);
			
			}
			
			if(!utStrIsSpaces(ip1) )//IP
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  (inet_ntoa(sip) like '%%%s%%' or devname like '%%%s%%' or lawname  like '%%%s%%' or mac  like '%%%s%%' )", ip1_gbk, ip1_gbk, ip1_gbk, ip1_gbk);
			
			}
			if(!utStrIsSpaces(sip1) )//È´òÁ∫ßIP
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  inet_ntoa(sip) like '%%%s%%'", sip1);
			
			}
			if(!utStrIsSpaces(compid1) )//ËÆ°ÁÆóÊú∫ÂêçÊü•Ë∑ØÂæ?
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and compid = %lu ", lCompid1);
			
			}
			
			printf("sql = %s\n", sql);
			lCount1 = 0;
			iReturn = pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &lCount1);
		printf("num = %d\n",num);	
		printf("iReturn = %d\n",iReturn);
		printf("lCount1 = %d\n",lCount1);
			if(iReturn!=1146 && lCount1 > 0)
			{
				strcpy(tableinfo[num].tname, ptmp[i]);
				strcpy(tableinfo2[num].tname, ptmp2[i]);
				tableinfo[num].lCount = lCount1;
			//	tableinfo2[num].lCount = lCount1;
				num++;
			}
			printf("num = %d\n",num);
//		}
		free(ptmp[i]);
		free(ptmp2[i]);
	}
	free(ptmp);	
	free(ptmp2);	
	for(i = 0; i < num; i++)
	{
		lTotRec += tableinfo[i].lCount;
	}
	utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
	printf("lTotRec = %d\n",lTotRec);
	//
	for(i = num-1; i >= 0; i--)
	{
		printf("table[%d] = %s, _lCount1[%d] = %lu\n", i, tableinfo[i].tname, i, tableinfo[i].lCount);
	}
	//
	
	iNum = 0;
//	iStart = atol(start);
//	iLimit = atol(limit);
	
		if(strlen(caExport)>0){
		
		iStart=0;
		iLimit=99999;
			//	printf("ÂØºÂá∫Êñá‰ª∂Ë∑ØÂæÑ11111111111\n");
		        sprintf(caFilename,"lanalarmlog%d.csv",utStrGetId());
		        sprintf(caPath,"%s/html","/home/ncmysql/ncs");
		        sprintf(caFile,"%s/%s",caPath,caFilename);
		    	fp = fopen(caFile,"w");
		        if(fp == NULL) {
		            pasDbCloseCursor(psCur);
					printf("ÂØºÂá∫Âá∫Èîô\n");
		           utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546Êï∞ÊçÆÂØºÂá∫Âá∫Èîô"),ncLang("0546Êï∞ÊçÆÂØºÂá∫Âá∫Èîô"));
		            return 0;
		        }
		        
		        fprintf(fp,"Œƒº˛»’÷æ\n");
		        ncUtlPrintCsv(fp,9,
		                           UT_TYPE_STRING," ±º‰",
		                           UT_TYPE_STRING,"≤ø√≈",
								    UT_TYPE_STRING,"º∆À„ª˙",
		                           UT_TYPE_STRING,"”√ªß",
								   UT_TYPE_STRING,"MAC",
		                           UT_TYPE_STRING,"IP",
		                           UT_TYPE_STRING,"¥•∑¢≤ﬂ¬‘",
		                           UT_TYPE_STRING,"∏ÊæØ∆¡ƒª",
								   UT_TYPE_STRING,"∏ÊæØ√Ë ˆ"
								 );
		                           
	}	else{
	//	printf("ÂØºÂá∫Êñá‰ª∂Ë∑ØÂæÑ222222222\n");
	iStart = atol(start);
	iLimit = atol(limit);
  }
	printf("num = %d\n",num);
  
	for(i = num-1; i >= 0; i--)
	{
//	printf("999999999999999\n");
	memset(sql, 0, sizeof(sql));
	if(tableinfo[i].lCount <= iStart)
		{	
			iStart-=tableinfo[i].lCount;
			continue;
		}	
   // strcpy(sql,"select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, content from nwfilelog_201309 limit 0,5");
   	snprintf(sql, sizeof(sql), "select %s.sid, mac, %s.stime, groupid, devname, inet_ntoa(sip),  udisp, %s.pid, file, mesg,gname,lawname, gid, nwmainprocy.sid  from %s  left join %s on (%s.screenid = %s.warnid ) left join nwgroup on (groupid = gid) left join nwmainprocy on(nwmainprocy.sid = %s.pid) where %s.stime >= %lu  and %s.stime <= %lu ", tableinfo[i].tname, tableinfo[i].tname,tableinfo[i].tname,tableinfo[i].tname, tableinfo2[i].tname,tableinfo[i].tname, tableinfo2[i].tname,tableinfo[i].tname,tableinfo[i].tname,lStartTime, tableinfo[i].tname,lTime);
		if((!utStrIsSpaces(department1)) && (strcmp(department1,"0") !=0) )//ÈÉ®Èó®
			{
				printf("department1 = %s\n",department1);
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and groupid in (%s) ", department1);
			}
			if((!utStrIsSpaces(mac1)) && strcmp(mac1,"0"))//Êñá‰ª∂Êìç‰ΩúÁ±ªÂûã
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and mac like '%%%s%%'", mac1);
			}
			if(!utStrIsSpaces(pid1) )//Ëß¶ÂèëÁ≠ñÁï•
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and lawname like '%%%s%%'", pid1_gbk);
			}
			if(!utStrIsSpaces(devname1) )//ËÆ°ÁÆóÊú∫Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and devname like '%%%s%%'", devname1);
			
			}
			
			if(!utStrIsSpaces(ip1) )//IP
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  (inet_ntoa(sip) like '%%%s%%' or devname like '%%%s%%' or lawname  like '%%%s%%' or mac  like '%%%s%%' )", ip1_gbk, ip1_gbk, ip1_gbk, ip1_gbk);
			
			}
			if(!utStrIsSpaces(sip1) )//È´òÁ∫ßIP
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  inet_ntoa(sip) like '%%%s%%'", sip1);
			
			}
			if(!utStrIsSpaces(compid1) )//ËÆ°ÁÆóÊú∫ÂêçÊü•Ë∑ØÂæ?
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and compid = %lu ", lCompid1);
			
			}
			snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " order by %s %s limit %lu, %lu", sort, dir, iStart, iLimit < tableinfo[i].lCount ? iLimit : tableinfo[i].lCount);
		//snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " limit %lu, %lu", iStart, iLimit < tableinfo[i].lCount ? iLimit : tableinfo[i].lCount);		
		printf("__line = %d, sql = %s\n", __LINE__, sql);
			if( tableinfo[i].lCount > iStart && tableinfo[i].lCount < iStart+iLimit )
		{
			iLimit = iLimit -  (tableinfo[i].lCount-iStart);
			iStart = 0;
		}
		else 
		{
			_break = 1;
		}
	//	tmpgbk = convert("UTF-8", "GBK", sql);
	//	if(tmpgbk == NULL)
	//		continue;
	//	printf("__line = %d, tempgbk = %s\n", __LINE__, tmpgbk);
		psCur = pasDbOpenSql(sql, 0);
    if(psCur != NULL)
		{
				
			long long sid = 0;
			long pid =0;
			long screenid = 0;
			long department = 0;
			char  mac[18]="";	
		//	char  _mark[12]="";	
			unsigned int stime =0;
			char _stime[40] = "";//ËΩ¨Êç¢Êó∂Èó¥
			char  procname[72] ="";
	//		char department[32] ="";
			char _department[32] ="";
			char devname[32] ="";
	//		char udisp[32] ="";
			char sip[24] ="";
			char file[80] ="";
			char _file[200] ="";
			char udisp[32] ="";
			char mesg[256] ="";
			char gname[24]="";
			char lawname[24]="";
			//char *_mark_gbk = NULL;
		//	char *_department_gbk = NULL;
			char *_gname_UTF = NULL;
			char *_lawname_UTF = NULL;
			printf("1111\n");
			
			while(0==(iret = pasDbFetchInto(psCur,
					 UT_TYPE_LONG8,  8,               	&sid,
					 UT_TYPE_STRING, sizeof(mac)-1, 	mac,
					 UT_TYPE_ULONG,  8,              	&stime,
					 UT_TYPE_ULONG, 8,  &department,
					 UT_TYPE_STRING, sizeof(devname)-1,  devname,
					 UT_TYPE_STRING, sizeof(sip)-1,  	sip,
					UT_TYPE_STRING, sizeof(udisp)-1,      udisp,					
					UT_TYPE_LONG, 8,   &pid,
					 UT_TYPE_STRING, sizeof(file)-1,   file,
					 UT_TYPE_STRING, sizeof(mesg)-1 , mesg,
					 UT_TYPE_STRING, sizeof(gname)-1 , gname,
					  UT_TYPE_STRING, sizeof(lawname)-1 , lawname))  || 1405==iret)
				{
				iNum++;
				if(iNum > 1)
				{
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			
				
				utPltPutLoopVarF(psDbHead,"sid",iNum,"%lld",sid);
				utPltPutLoopVarF(psDbHead,"mac",iNum,"%s",mac);
				strcpy(_stime, utTimFormat("%Y/%m/%d %H:%M:%S", stime));//ËΩ¨Êç¢Êó∂Èó¥
				utPltPutLoopVarF(psDbHead,"stime",iNum,"%s",_stime);
				utPltPutLoopVarF(psDbHead,"department",iNum,"%lu",department);
				utPltPutLoopVarF(psDbHead,"devname",iNum,"%s",devname);
				utPltPutLoopVarF(psDbHead,"sip",iNum,"%s",sip);
				utPltPutLoopVarF(psDbHead,"udisp",iNum,"%s",udisp);//Êñá‰ª∂Âêç	
				utPltPutLoopVarF(psDbHead, "pid", iNum, "%lu", pid);//Á®ãÂ∫èÂêç
				
			//	sprintf(caPath,"%s/screenAlterJPG","/home/ncmysql/nw");
		   //     sprintf(_file,"%s/%s",caPath,file);
				
			
				if(strcmp(file,"")==0){
					utPltPutLoopVar(psDbHead,"file",iNum,"0");
				}else{
					utPltPutLoopVarF(psDbHead, "file", iNum, "%s", file);
				}
				
				utPltPutLoopVarF(psDbHead, "mesg", iNum, "%s", mesg);
				utPltPutLoopVarF(psDbHead, "gname", iNum, "%s", gname);
				utPltPutLoopVarF(psDbHead, "lawname", iNum, "%s", lawname);
			
		//	printf("1111\n");
			//	_mark_gbk = convert("UTF-8", "GBK", _mark);
			//	char *_mark_UTF8 = convert("GBK","UTF-8", _mark_gbk);
			//	_gname_UTF = convert("GBK","UTF-8", gname);
			//	_lawname_UTF = convert("GBK","UTF-8", lawname);
						if(strlen(caExport)>0){
						ncUtlPrintCsv(fp,9,
                          UT_TYPE_STRING,_stime,
                          UT_TYPE_STRING,gname,
                          UT_TYPE_STRING,devname,
						  UT_TYPE_STRING,udisp,
						  UT_TYPE_STRING, mac,
                          UT_TYPE_STRING,sip,
						   UT_TYPE_STRING,lawname,
						   UT_TYPE_STRING,file,
						UT_TYPE_STRING, mesg);
					 }	
				 
			
			}
			pasDbCloseCursor(psCur);
			}
			free(tmpgbk);
			if(_break == 1)
			break; 
		}	
		//	utPltShowDb(psDbHead);
			//	free(_keyword_gbk);
				
		//	printf("22222\n");
		//	pasDbCloseCursor(psCur);
		 if(strlen(caExport)>0){
	 	    fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        utPltFreeDb(psDbHead);
        return 0;
	 }
	
			int tmp=utPltOutToHtml(iFd,psMsgHead,psDbHead,"v4/neiwang/rzck/lan_log_alarm.htm");
			printf("temp=%d\n",tmp);
			printf("iTime_flag=%d\n",iTime_flag);
		
			
	//	pasDbClose(psConn);
    return 0;
}


//Ëé∑ÂèñÂõæÁâáË∑ØÂæÑ
int nwWeb_ShowScreenJpg(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    unsigned long lSid;
    char caFile[256],caPath[128],caFilename[128],caSid[16];
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
        iReturn = utMsgGetSomeNVar(psMsgHead,1,
                        "file",  UT_TYPE_STRING, 255, caFile);
		printf("showJPG caFile = %s\n",caFile);
    
    memset(caPath,0,sizeof(caPath));
	 int position=0;
	 int i =0;
			while(caFile[strlen(caFile)-i] !='/'){
			position = strlen(caFile)-i;
			i++;
			}
			
			printf("postion =%d, i =%d\n",position,i);
			 strncpy(caPath,caFile,position);
			strcpy(caFilename,caFile+position);
			printf("caPath=%s,caFilename=%s\n",caPath,caFilename);
	//	strcpy(caPath,"/home/ncmysql/nw/screenJPG/1444845655/");
	//	strcpy(caFilename,"1379123256.bmp");
         utPltFileDownload(iFd,"image/jpeg",caPath,caFilename,caFilename);
		 printf("11111111\n");
         return 0;
}

//ÂÆûÊó∂ÁõëÊéß
int lan_ssjk_getScreen(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{    
 
	char caType[32];
	int iTime_flag = 0;
	int days = 0;   
	int iret = 0;
	int _break = 0;
	int i = 0;
	int lCount = 0;
	int num = 0;
	int iNum = 0;
	
	unsigned long did = 0;
	unsigned long iStart = 0;
	unsigned long iLimit = 0;
	unsigned long lCount1 = 0;
	unsigned long lStartTime = 0;
   	unsigned long lTime = time(0);
   	unsigned long lTotRec = 0;
	
   	char sdate[11] = "";
   	char edate[11] = "";
   	char time_flag[2] = "";
   	char start[17] = "";
   	char limit[17] = "";
   	char sort[16] = "";
   	char dir[16] = "";
   	char sql[4096] = "";
   	char _sip[16] = "";
  
   	char _groupid[33] = "";
   	char _did[33] = "";
   	char _uid[17] = "";
   	char sql2[256] = "";
   	char frmfile[256] = "";
   	char *tmpgbk = NULL;
	char **ptmp = NULL;	
	char _keyword[33] = "";
	long iReturn;
	char caExport[12];
	
	char ip1[24]="";
	char sip1[24]="";
	char department1[32]="";
	char mac1[24]="";
	char compid1[32]="";
	char udisp1[32]="";	
	char image1[80] = "";
		FILE *fp2;
		char caFilename[32];
		char caFile[128];
		char caPath[128];

	struct sttable
	{
		char tname[32];
		unsigned long lCount;
	};
	struct sttable tableinfo[120];//10Âπ¥Ôºü
   	pasDbCursor *psCur = NULL;
	char did1[16] = "";
	char did2[16] = "";
	char did3[16] = "";
	char did4[16] = "";
	char did5[16] = "";
	char did6[16] = "";
	char did7[16] = "";
	char did8[16] = "";
	char did9[16] = "";
	
	char gid1[16] = "";
	char gid2[16] = "";
	char gid3[16] = "";
	char gid4[16] = "";
	char gid5[16] = "";
	char gid6[16] = "";
	char gid7[16] = "";
	char gid8[16] = "";
	char gid9[16] = "";
	
//	strcpy(caFacs,getDsFacDid());
 // strcpy(caGroups,getDsGroupid());
	
	utPltDbHead *psDbHead = utPltInitDb();
	utMsgPrintMsg(psMsgHead);

    //pasDbConn *psConn;
	
		utMsgGetSomeNVar(psMsgHead, 27,
   				     "keyword",    UT_TYPE_STRING,  sizeof(_keyword)-1,   _keyword,
   				     "uid",        UT_TYPE_STRING,  sizeof(_uid)-1,       _uid,
   				//     "ip1",       UT_TYPE_STRING,  sizeof(ip1)-1,     ip1,
   				//     "sip1",        UT_TYPE_STRING,  sizeof(sip1)-1,       sip1,
   				//     "department1",  UT_TYPE_STRING,  sizeof(department1)-1, department1,
   				//     "mac1",        UT_TYPE_STRING,  sizeof(mac1)-1,       mac1,  
   				//     "compid1",    UT_TYPE_STRING,  sizeof(compid1)-1,   compid1,
				//	 "image1",    UT_TYPE_STRING,  sizeof(image1)-1,   image1,   				     
   				     "udisp1",        UT_TYPE_STRING,  sizeof(udisp1)-1,        udisp1,   				     
   				 //    "sdate", 	   UT_TYPE_STRING,  sizeof(sdate)-1,       sdate,
   				 //    "edate", 	   UT_TYPE_STRING,  sizeof(edate)-1,       edate,
   				     "time_flag",  UT_TYPE_STRING,  sizeof(time_flag)-1,   time_flag,
   				     "start",      UT_TYPE_STRING,  sizeof(start)-1,       start,
   				     "limit",      UT_TYPE_STRING,  sizeof(limit)-1,       limit,
   				     "sort",       UT_TYPE_STRING,  sizeof(sort)-1,        sort,
   				     "dir",        UT_TYPE_STRING,  sizeof(dir)-1,         dir,
   				     "export2",    UT_TYPE_STRING,  sizeof(caExport)-1,  caExport,
					 "did1",	UT_TYPE_STRING,  sizeof(did1)-1,  did1,
					  "did2",	UT_TYPE_STRING,  sizeof(did2)-1,  did2,
					   "did3",	UT_TYPE_STRING,  sizeof(did3)-1,  did3,
					    "did4",	UT_TYPE_STRING,  sizeof(did4)-1,  did4,
					"did5",	UT_TYPE_STRING,  sizeof(did5)-1,  did5,
					  "did6",	UT_TYPE_STRING,  sizeof(did6)-1,  did6,
					   "did7",	UT_TYPE_STRING,  sizeof(did7)-1,  did7,
					    "did8",	UT_TYPE_STRING,  sizeof(did8)-1,  did8,
						 "did9",	UT_TYPE_STRING,  sizeof(did9)-1,  did9,
					 "gid1", 	UT_TYPE_STRING,  sizeof(gid1)-1,  gid1,
					  "gid2", 	UT_TYPE_STRING,  sizeof(gid2)-1,  gid2,
					   "gid3", 	UT_TYPE_STRING,  sizeof(gid3)-1,  gid3,
					   "gid4", 	UT_TYPE_STRING,  sizeof(gid4)-1,  gid4,
					    "gid5", 	UT_TYPE_STRING,  sizeof(gid5)-1,  gid5,
					    "gid6", 	UT_TYPE_STRING,  sizeof(gid6)-1,  gid6,
						 "gid7", 	UT_TYPE_STRING,  sizeof(gid7)-1,  gid7,
						 "gid8", 	UT_TYPE_STRING,  sizeof(gid8)-1,  gid8,
						 "gid9", 	UT_TYPE_STRING,  sizeof(gid9)-1,  gid9
					 );
					  
	//ÊéíÂ∫èÈªòËÆ§Êó∂Èó¥
	if(strlen(sort)==0){
 	   strcpy(sort,"stime");
	}			  
		iTime_flag = atoi(time_flag);
   	switch(iTime_flag)
   	{
   		case 1://‰ªäÂ§©
   			days = 1;
   			break;
   		case 2://ÊúÄËøë‰∏âÂ§©
   			days = 3;
   			break;
   		case 3://ÊúÄËøë‰∏ÄÂë®
   			days = 7;
   			break;
   		case 4://ÊúÄËøë‰∏ÄÊúà
   			days = 30;
   			break;
   		case 5://ÊúÄËøë‰∏âÊúà
   			days = 90;
   			break;
   		case 6://ÊúÄËøëÂçäÂπ¥
   			days = 180;
   			break;
   		default:
   			days = 1;
   			break;
   	}	
	
	printf("lTime = %lu\n",lTime);
	if(strlen(sdate) > 0 && strlen(edate) > 0)
	{
		ptmp = ncsUtlGetTable2(sdate, edate, "ncscreenssjk_", &lStartTime, &lTime, &lCount);
	}
	else 
	{
		ptmp = ncsUtlGetTable(lTime, days, "ncscreenssjk_",  &lStartTime, &lCount);
		lTime=lTime+2*3600;
	}
		printf("ptmp = %s\n",*ptmp);
		printf("days = %d\n",days);		
		printf("limit = %s\n",limit);
		printf("lTime = %lu\n",lTime);
		printf("lTime = %s\n",utTimFormat("%Y/%m/%d", lTime));
		printf("lStartTime = %lu\n",lStartTime);
		printf("lStartTime = %s\n",utTimFormat("%Y/%m/%d", lStartTime));
		printf("gid1 = %s\n",gid1);
		printf("gid2 = %s\n",gid2);
		printf("gid3 = %s\n",gid3);
		printf("gid4 = %s\n",gid4);
		printf("did1 = %s\n",did1);
		printf("did2 = %s\n",did2);
		/*
		char *gid1_gbk;
		if(gid1 != ""){
		gid1_gbk = convert("UTF-8", "GBK", gid1);}
		char *gid2_gbk;
		if(gid2 != ""){
		gid2_gbk = convert("UTF-8", "GBK", gid2);}
		char *gid3_gbk;
		if(gid3 != ""){
		 gid3_gbk= convert("UTF-8", "GBK", gid3);}
		char *gid4_gbk = convert("UTF-8", "GBK", gid4);
		char *gid5_gbk = convert("UTF-8", "GBK", gid5);
		char *gid6_gbk = convert("UTF-8", "GBK", gid6);
		char *gid7_gbk = convert("UTF-8", "GBK", gid7);
		char *gid8_gbk = convert("UTF-8", "GBK", gid8);
		char *gid9_gbk = convert("UTF-8", "GBK", gid9);
		printf("gid1_gbk = %s\n", gid1_gbk);
		printf("gid2_gbk = %s\n", gid2_gbk);
		*/
	
		char _gid1[16] = "0";
		char _gid2[16] = "0";
		char _gid3[16] = "0";
		char _gid4[16] = "0";
		char _gid5[16] = "0";
		char _gid6[16] = "0";
		char _gid7[16] = "0";
		char _gid8[16] = "0";
		char _gid9[16] = "0";
		
		
		if(strcmp(gid1,"Âê¥ÂΩ¨") == 0){
			strcpy(_gid1,"wubin");
		}
		else if(strcmp(gid1,"Â≠ô‰ºü‰ºü") == 0){
			strcpy(_gid1,"sunweiwei");
		}else if(strcmp(gid1,"ÂÆãÊòéÊòé") == 0){
			strcpy(_gid1,"songmingming");
		}else if(strcmp(gid1,"Êùé‰∫ëÊñπ") == 0){
			strcpy(_gid1,"liyunfang");
		}else if(strcmp(gid1,"ËÉ°ÂøóÊ∞¥") == 0){
			strcpy(_gid1,"huzhishui");
		}
		
		printf("_gid1 = %s \n",_gid1);
		printf("_gid2 = %s \n",_gid2);
	//	printf("gid2_gbk = %s\n", gid2_gbk);
		
		if(strcmp(gid2,"ÂÆãÊòéÊòé") == 0){
			strcpy(_gid2,"songmingming");
				printf("WTFFF\n");
		}
		else if(strcmp(gid2,"Â≠ô‰ºü‰ºü") == 0){
			strcpy(_gid2,"sunweiwei");
		
		}else if(strcmp(gid2,"Âê¥ÂΩ¨") == 0){
			strcpy(_gid2,"wubin");
		}else if(strcmp(gid2,"Êùé‰∫ëÊñπ") == 0){
			strcpy(_gid2,"liyunfang");
		}else if(strcmp(gid2,"ËÉ°ÂøóÊ∞¥") == 0){
			strcpy(_gid2,"huzhishui");
		}
	
		if(strcmp(gid3,"ÂÆãÊòéÊòé") == 0){
			strcpy(_gid3,"songmingming");
				printf("WTFFF\n");
		}
		else if(strcmp(gid3,"Â≠ô‰ºü‰ºü") == 0){
			strcpy(_gid3,"sunweiwei");
		
		}else if(strcmp(gid3,"Âê¥ÂΩ¨") == 0){
			strcpy(_gid3,"wubin");
		}else if(strcmp(gid3,"Êùé‰∫ëÊñπ") == 0){
			strcpy(_gid3,"liyunfang");
		}else if(strcmp(gid3,"ËÉ°ÂøóÊ∞¥") == 0){
			strcpy(_gid3,"huzhishui");
		}
		
		if(strcmp(gid4,"ÂÆãÊòéÊòé") == 0){
			strcpy(_gid4,"songmingming");
				printf("WTFFF\n");
		}
		else if(strcmp(gid4,"Â≠ô‰ºü‰ºü") == 0){
			strcpy(_gid4,"sunweiwei");
		
		}else if(strcmp(gid4,"Âê¥ÂΩ¨") == 0){
			strcpy(_gid4,"wubin");
		}else if(strcmp(gid4,"Êùé‰∫ëÊñπ") == 0){
			strcpy(_gid4,"liyunfang");
		}else if(strcmp(gid4,"ËÉ°ÂøóÊ∞¥") == 0){
			strcpy(_gid4,"huzhishui");
		}
		
		if(strcmp(gid5,"Âê¥ÂΩ¨") == 0){
			strcpy(_gid5,"wubin");
		}
		else if(strcmp(gid5,"Â≠ô‰ºü‰ºü") == 0){
			strcpy(_gid5,"sunweiwei");
		}else if(strcmp(gid5,"ÂÆãÊòéÊòé") == 0){
			strcpy(_gid5,"songmingming");
		}else if(strcmp(gid5,"Êùé‰∫ëÊñπ") == 0){
			strcpy(_gid5,"liyunfang");
		}else if(strcmp(gid5,"ËÉ°ÂøóÊ∞¥") == 0){
			strcpy(_gid5,"huzhishui");
		}
		
		if(strcmp(gid6,"Âê¥ÂΩ¨") == 0){
			strcpy(_gid6,"wubin");
		}
		else if(strcmp(gid6,"Â≠ô‰ºü‰ºü") == 0){
			strcpy(_gid6,"sunweiwei");
		}else if(strcmp(gid6,"ÂÆãÊòéÊòé") == 0){
			strcpy(_gid6,"songmingming");
		}else if(strcmp(gid6,"Êùé‰∫ëÊñπ") == 0){
			strcpy(_gid6,"liyunfang");
		}else if(strcmp(gid6,"ËÉ°ÂøóÊ∞¥") == 0){
			strcpy(_gid6,"huzhishui");
		}
		
		if(strcmp(gid7,"Âê¥ÂΩ¨") == 0){
			strcpy(_gid7,"wubin");
		}
		else if(strcmp(gid7,"Â≠ô‰ºü‰ºü") == 0){
			strcpy(_gid7,"sunweiwei");
		}else if(strcmp(gid7,"ÂÆãÊòéÊòé") == 0){
			strcpy(_gid7,"songmingming");
		}else if(strcmp(gid7,"Êùé‰∫ëÊñπ") == 0){
			strcpy(_gid7,"liyunfang");
		}else if(strcmp(gid7,"ËÉ°ÂøóÊ∞¥") == 0){
			strcpy(_gid7,"huzhishui");
		}
		
		if(strcmp(gid8,"Âê¥ÂΩ¨") == 0){
			strcpy(_gid8,"wubin");
		}
		else if(strcmp(gid8,"Â≠ô‰ºü‰ºü") == 0){
			strcpy(_gid8,"sunweiwei");
		}else if(strcmp(gid8,"ÂÆãÊòéÊòé") == 0){
			strcpy(_gid8,"songmingming");
		}else if(strcmp(gid8,"Êùé‰∫ëÊñπ") == 0){
			strcpy(_gid8,"liyunfang");
		}else if(strcmp(gid8,"ËÉ°ÂøóÊ∞¥") == 0){
			strcpy(_gid8,"huzhishui");
		}
		
		if(strcmp(gid9,"Âê¥ÂΩ¨") == 0){
			strcpy(_gid9,"wubin");
		}
		else if(strcmp(gid9,"Â≠ô‰ºü‰ºü") == 0){
			strcpy(_gid9,"sunweiwei");
		}else if(strcmp(gid9,"ÂÆãÊòéÊòé") == 0){
			strcpy(_gid9,"songmingming");
		}else if(strcmp(gid9,"Êùé‰∫ëÊñπ") == 0){
			strcpy(_gid9,"liyunfang");
		}else if(strcmp(gid9,"ËÉ°ÂøóÊ∞¥") == 0){
			strcpy(_gid9,"huzhishui");
		}
		
		printf("_gid1 = %s \n",_gid1);
		printf("_gid2 = %s \n",_gid2);
		printf("_gid3 = %s \n",_gid3);
		printf("_gid4 = %s \n",_gid4);
		
//	char *compid_gbk = convert("UTF-8", "GBK", compid1);
//	char *mac1_gbk = convert("UTF-8", "GBK", mac1);
//	printf("%s\n",procname1_gbk);
//	printf("%s\n",filename1_gbk);
//	char *_keyword_gbk = convert("UTF-8", "GBK", _keyword);
	char _sdate[11] = "";
	char _edate[11] = "";
	strcpy(_sdate, utTimFormat("%Y/%m/%d", lStartTime));
	strcpy(_edate, utTimFormat("%Y/%m/%d", lTime));
	printf("_sdate= %s\n",_sdate);
	printf("edate= %s\n",_edate);
	for(i = 0; i < lCount; i++)
	{
		snprintf(frmfile, sizeof(frmfile), "/usr/local/infobright/data/ncs/%s.frm", ptmp[i]);
		
//		if(access(frmfile, F_OK) == 0)
//		{
			memset(sql, 0, sizeof(sql));
		//	snprintf(sql, sizeof(sql), "select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, content  from %s where stime >= %lu and stime <= %lu ",  ptmp[i], lStartTime, lTime);
		//	snprintf(sql, sizeof(sql), "select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, content  from %s   where stime >= %lu  and stime <= %lu limit 0,5",  ptmp[i], lStartTime, lTime);
		//	snprintf(sql, sizeof(sql), "select count(*) from %s where stime >= %lu and stime <= %lu ",  ptmp[i], lStartTime, lTime);
			snprintf(sql, sizeof(sql), "select count(*) from %s where stime <= %lu ",  ptmp[i], lTime);
		//		snprintf(sql, sizeof(sql), "select count(*) from %s ",  ptmp[i]);
		if((!utStrIsSpaces(department1)) && (strcmp(department1,"0") !=0) )//ÈÉ®Èó®
			{
				printf("department1 = %s\n",department1);
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and groupid in (%s) ", department1);
			}
			
			if(!utStrIsSpaces(gid1) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid1);
			}
			if(!utStrIsSpaces(gid2) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid2);
			}
			if(!utStrIsSpaces(gid3) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid3);
			}
			if(!utStrIsSpaces(gid4) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid4);
			}
			if(!utStrIsSpaces(gid5) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid5);
			}
			if(!utStrIsSpaces(gid6) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid6);
			}
			if(!utStrIsSpaces(gid7) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid7);
			}
			if(!utStrIsSpaces(gid8) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid8);
			}
			if(!utStrIsSpaces(gid9) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid9);
			}
			if(!utStrIsSpaces(mac1) )//ËøõÁ®ãÂêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and mac like '%%%s%%'", mac1);
			}
			if(!utStrIsSpaces(compid1) )//ËÆ°ÁÆóÊú∫ÂêçÊü•Ë∑ØÂæÑ
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and compid like '%%%s%%')", compid1);
			
			}
			if(!utStrIsSpaces(ip1) )//IP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  inet_ntoa(sip) like '%%%s%%'", ip1);
			
			}
			if(!utStrIsSpaces(sip1) )//È´òÁ∫ßIP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  inet_ntoa(sip) like '%%%s%%'", sip1);
			
			}
	
			
			printf("sql = %s\n", sql);
			lCount1 = 0;
			iReturn = pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &lCount1);
	printf("num = %d\n",num);	
		printf("iReturn = %d\n",iReturn);
		printf("lCount1 = %d\n",lCount1);
			if(iReturn!=1146 && lCount1 > 0)
			{
				strcpy(tableinfo[num].tname, ptmp[i]);
				tableinfo[num].lCount = lCount1;
				num++;
			}
			printf("num = %d\n",num);
//		}
		free(ptmp[i]);
	}
	free(ptmp);	
	for(i = 0; i < num; i++)
	{
		lTotRec += tableinfo[i].lCount;
	}
	utPltPutVarF(psDbHead,"TotRec", "%lu", lTotRec);
	printf("lTotRec = %d\n",lTotRec);
	//
	for(i = num-1; i >= 0; i--)
	{
		printf("table[%d] = %s, _lCount1[%d] = %lu\n", i, tableinfo[i].tname, i, tableinfo[i].lCount);
	}
	//
	
	iNum = 0;
//	iStart = atol(start);
//	iLimit = atol(limit);
	
		if(strlen(caExport)>0){
		
		iStart=0;
		iLimit=99999;
				printf("ÂØºÂá∫Êñá‰ª∂Ë∑ØÂæÑ11111111111\n");
		        sprintf(caFilename,"lanscreenssjk%d.csv",utStrGetId());
		        sprintf(caPath,"%s/html","/home/ncmysql/ncs");
		        sprintf(caFile,"%s/%s",caPath,caFilename);
		    	printf("caFile = %s\n",caFile);
				
				fp2 = fopen(caFile,"w");
				
		        if(fp2 == NULL) {
					printf("22222222222\n");	
		            pasDbCloseCursor(psCur);
					printf("ÂØºÂá∫Âá∫Èîô\n");
		           utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncLang("0546Êï∞ÊçÆÂØºÂá∫Âá∫Èîô"),ncLang("0546Êï∞ÊçÆÂØºÂá∫Âá∫Èîô"));
		            return 0;
		        }
		        	printf("33333333333\n");		
		        fprintf(fp2,"Â±èÂπïÊó•Âøó\n");
		        ncUtlPrintCsv(fp2,8,	
									UT_TYPE_STRING,"Â∫èÂè∑",
									UT_TYPE_STRING,"ÈÉ®Èó®ID",
									UT_TYPE_STRING,"ËÆ°ÁÆóÊú∫ID",
									UT_TYPE_STRING,"Áî®Êà∑Âêç",
									UT_TYPE_STRING,"IP",
									UT_TYPE_STRING,"MAC",
									UT_TYPE_STRING,"Êó∂Èó¥",
									UT_TYPE_STRING,"ÂõæÁâá");
					printf("44444444444\n");	
				
						
		                           
	}	else{
		printf("ÂØºÂá∫Êñá‰ª∂Ë∑ØÂæÑ222222222\n");
	iStart = atol(start);
	iLimit = atol(limit);
  }
	printf("num = %d\n",num);
  
	for(i = num-1; i >= 0; i--)
	{
//	printf("999999999999999\n");
	memset(sql, 0, sizeof(sql));
	if(tableinfo[i].lCount <= iStart)
		{	
			iStart-=tableinfo[i].lCount;
			continue;
		}	
   // strcpy(sql,"select sid, mark, stime, groupid, devname, inet_ntoa(sip), procname, soupath, destpath, content from nwfilelog_201309 limit 0,5");
   	snprintf(sql, sizeof(sql), "select sid, groupid, compid, udisp, inet_ntoa(sip), mac, stime, file  from %s   where stime >= %lu  and stime <= %lu ", tableinfo[i].tname, lStartTime, lTime);
	 	snprintf(sql, sizeof(sql), "select sid, groupid, compid, udisp, inet_ntoa(sip), mac, stime, file  from %s   where stime <= %lu ", tableinfo[i].tname, lTime);
  // snprintf(sql, sizeof(sql), "select sid, groupid, compid, udisp, inet_ntoa(sip), mac, stime, file  from %s ", tableinfo[i].tname);
	if((!utStrIsSpaces(department1)) && (strcmp(department1,"0") !=0) )//ÈÉ®Èó®
			{
				printf("department1 = %s\n",department1);
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and groupid in (%s) ", department1);
			}
			
			
			if(!utStrIsSpaces(gid1) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid1);
			}
			if(!utStrIsSpaces(gid2) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid2);
			}
			if(!utStrIsSpaces(gid3) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid3);
			}
			if(!utStrIsSpaces(gid4) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid4);
			}
			if(!utStrIsSpaces(gid5) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid5);
			}
			if(!utStrIsSpaces(gid6) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid6);
			}
			if(!utStrIsSpaces(gid7) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid7);
			}
			if(!utStrIsSpaces(gid8) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid8);
			}
			if(!utStrIsSpaces(gid9) )//Áî®Êà∑Âêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and udisp like '%%%s%%'", _gid9);
			}
			if(!utStrIsSpaces(mac1) )//ËøõÁ®ãÂêç
			{
				snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and mac like '%%%s%%'", mac1);
			}
			if(!utStrIsSpaces(compid1) )//ËÆ°ÁÆóÊú∫ÂêçÊü•Ë∑ØÂæÑ
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and compid like '%%%s%%')", compid1);
			
			}
			if(!utStrIsSpaces(ip1) )//IP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  inet_ntoa(sip) like '%%%s%%'", ip1);
			
			}
			if(!utStrIsSpaces(sip1) )//È´òÁ∫ßIP
			{
					snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " and  inet_ntoa(sip) like '%%%s%%'", sip1);
			
			}
		snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " order by %s %s limit %lu, %lu", sort, dir, iStart, iLimit < tableinfo[i].lCount ? iLimit : tableinfo[i].lCount);
	//	snprintf(sql+strlen(sql), sizeof(sql)-strlen(sql), " limit %lu, %lu", iStart, iLimit < tableinfo[i].lCount ? iLimit : tableinfo[i].lCount);		
		printf("__line = %d, sql = %s\n", __LINE__, sql);
			if( tableinfo[i].lCount > iStart && tableinfo[i].lCount < iStart+iLimit )
		{
			iLimit = iLimit -  (tableinfo[i].lCount-iStart);
			iStart = 0;
		}
		else 
		{
			_break = 1;
		}
	tmpgbk = convert("UTF-8", "GBK", sql);
		if(tmpgbk == NULL)
			continue;
		printf("__line = %d, sql = %s\n", __LINE__, tmpgbk);
		psCur = pasDbOpenSql(tmpgbk, 0);
    if(psCur != NULL)
		{
			
			long sid = 0;
			char  mark[12]="";	
			char  _mark[12]="";	
			unsigned int stime =0;
			char _stime[40] = "";//ËΩ¨Êç¢Êó∂Èó¥
			char compid[72] ="";
			char department[32] ="";
			char _department[32] ="";
			char udisp[32] ="";
			char mac[32] ="";
			char sip[24] ="";
			char file[100] ="";
			char _file[200] ="";
			char *_department_gbk = NULL;
			printf("1111\n");
						
			while(0==(iret = pasDbFetchInto(psCur,
					 UT_TYPE_LONG,  8,               	&sid,
					 UT_TYPE_STRING, sizeof(department)-1,  department,
					 UT_TYPE_STRING, sizeof(compid)-1, 	compid,
					 UT_TYPE_STRING, sizeof(udisp)-1,  udisp,
					 UT_TYPE_STRING, sizeof(sip)-1,  	sip,
					 UT_TYPE_STRING, sizeof(mac)-1,   mac,
					 UT_TYPE_ULONG,  8,              	&stime,
					 UT_TYPE_STRING, sizeof(file)-1,      file
					))  || 1405==iret)
			{
				iNum++;
				if(iNum > 1)
				{
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			
					switch(atoi(department)){
			case 0:strcpy(_department,"ÂÖ®ÈÉ®"); break;
			case 1:strcpy(_department,"‰∫∫‰∫ãÈÉ®"); break;
			case 2:strcpy(_department,"Á†îÂèëÈÉ®"); break;
			case 3:strcpy(_department,"Â∏ÇÂú∫ÈÉ®"); break;
			case 4:strcpy(_department,"ÈîÄÂîÆÈÉ®"); break;
			case 5:strcpy(_department,"Ë¥¢Âä°ÈÉ®"); break;
			case 6:strcpy(_department,"Áª¥‰øÆÈÉ®"); break;
			case 7:strcpy(_department,"‰∫ßÂìÅÈÉ®"); break;
			default:	
			break;
			}
	
			//ËΩ¨Á†Å
			_department_gbk = convert("UTF-8", "GBK", _department);
			
			
				utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",sid);
				utPltPutLoopVarF(psDbHead,"department",iNum,"%s",_department_gbk);
				utPltPutLoopVarF(psDbHead,"compid",iNum,"%s",compid);
				utPltPutLoopVarF(psDbHead,"udisp",iNum,"%s",udisp);//Êñá‰ª∂Âêç	
				utPltPutLoopVarF(psDbHead,"sip",iNum,"%s",sip);
				utPltPutLoopVarF(psDbHead, "mac", iNum, "%s", mac);
				strcpy(_stime, utTimFormat("%m/%d %H:%M:%S", stime));//ËΩ¨Êç¢Êó∂Èó¥
				utPltPutLoopVarF(psDbHead,"stime",iNum,"%s",_stime);
			
			//	sprintf(caPath,"%s/screenJPG","/home/ncmysql/nw");
		    //    sprintf(_file,"%s/%s",caPath,file);
				//utPltFileDownload(iFd,"image/jpeg",caPath,caFilename,caFilename);
				
				utPltPutLoopVarF(psDbHead, "file", iNum, "%s", file);
		
		//	printf("1111\n");
			//	_mark_gbk = convert("UTF-8", "GBK", _mark);
			char *_department_UTF8 = convert("GBK","UTF-8", _department_gbk);
				
						if(strlen(caExport)>0){
						printf("_stime = %s\n",_stime);	
			
			ncUtlPrintCsv(fp2,8,
							UT_TYPE_LONG,sid,
							UT_TYPE_STRING,_department_UTF8,
							UT_TYPE_STRING,compid,	
							UT_TYPE_STRING,udisp,
							UT_TYPE_STRING,sip,
							UT_TYPE_STRING,mac,
							UT_TYPE_STRING,_stime,
							UT_TYPE_STRING,file
							);
					 }	
				 
			
			}
			pasDbCloseCursor(psCur);
			}
		
			
			
			free(tmpgbk);
			if(_break == 1)
			break; 
			}	
			
		if(lTotRec == 0){
			printf("heiping!!!!!!!\n");
			snprintf(sql, sizeof(sql), "select sid, groupid, compid, udisp, inet_ntoa(sip), mac, stime, file  from  ncscreenssjk_201310 where udisp = '0'");
			psCur = pasDbOpenSql(sql, 0);
			 if(psCur != NULL)
		{
			
			long sid = 0;
			char  mark[12]="";	
			char  _mark[12]="";	
			unsigned int stime =0;
			char _stime[40] = "";//ËΩ¨Êç¢Êó∂Èó¥
			char compid[72] ="";
			char department[32] ="";
			char _department[32] ="";
			char udisp[32] ="";
			char mac[32] ="";
			char sip[24] ="";
			char file[100] ="";
			char _file[200] ="";
			char *_department_gbk = NULL;
			printf("1111\n");
						
			while(0==(iret = pasDbFetchInto(psCur,
					 UT_TYPE_LONG,  8,               	&sid,
					 UT_TYPE_STRING, sizeof(department)-1,  department,
					 UT_TYPE_STRING, sizeof(compid)-1, 	compid,
					 UT_TYPE_STRING, sizeof(udisp)-1,  udisp,
					 UT_TYPE_STRING, sizeof(sip)-1,  	sip,
					 UT_TYPE_STRING, sizeof(mac)-1,   mac,
					 UT_TYPE_ULONG,  8,              	&stime,
					 UT_TYPE_STRING, sizeof(file)-1,      file
					))  || 1405==iret)
			{
				iNum++;
				if(iNum > 1)
				{
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
			switch(atoi(department)){
			case 0:strcpy(_department,"ÂÖ®ÈÉ®"); break;
			case 1:strcpy(_department,"‰∫∫‰∫ãÈÉ®"); break;
			case 2:strcpy(_department,"Á†îÂèëÈÉ®"); break;
			case 3:strcpy(_department,"Â∏ÇÂú∫ÈÉ®"); break;
			case 4:strcpy(_department,"ÈîÄÂîÆÈÉ®"); break;
			case 5:strcpy(_department,"Ë¥¢Âä°ÈÉ®"); break;
			case 6:strcpy(_department,"Áª¥‰øÆÈÉ®"); break;
			case 7:strcpy(_department,"‰∫ßÂìÅÈÉ®"); break;
			default:	
			break;
			}
	
			//ËΩ¨Á†Å
			_department_gbk = convert("UTF-8", "GBK", _department);
			
			
				utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",sid);
				utPltPutLoopVarF(psDbHead,"department",iNum,"%s",_department_gbk);
				utPltPutLoopVarF(psDbHead,"compid",iNum,"%s",compid);
				utPltPutLoopVarF(psDbHead,"udisp",iNum,"%s",udisp);//Êñá‰ª∂Âêç	
				utPltPutLoopVarF(psDbHead,"sip",iNum,"%s",sip);
				utPltPutLoopVarF(psDbHead, "mac", iNum, "%s", mac);
				strcpy(_stime, utTimFormat("%m/%d %H:%M:%S", stime));//ËΩ¨Êç¢Êó∂Èó¥
				utPltPutLoopVarF(psDbHead,"stime",iNum,"%s",_stime);
			
			//	sprintf(caPath,"%s/screenJPG","/home/ncmysql/nw");
		    //    sprintf(_file,"%s/%s",caPath,file);
				//utPltFileDownload(iFd,"image/jpeg",caPath,caFilename,caFilename);
				
				utPltPutLoopVarF(psDbHead, "file", iNum, "%s", file);
		
		//	printf("1111\n");
			//	_mark_gbk = convert("UTF-8", "GBK", _mark);
			char *_department_UTF8 = convert("GBK","UTF-8", _department_gbk);
				}
			pasDbCloseCursor(psCur);
			}
			
			}	
			
			
	//	utPltShowDb(psDbHead);
		if(strlen(caExport)>0){
	 	fclose(fp2);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
      //  remove(caFile);
        utPltFreeDb(psDbHead);
		printf("9999999\n");
        return 0;
	 }
			
			int tmp=utPltOutToHtml(iFd,psMsgHead,psDbHead,"v4/neiwang/rzck/lan_log_screen.htm");
			printf("temp=%d\n",tmp);
			printf("iTime_flag=%d\n",iTime_flag);
		
			
	//	pasDbClose(psConn);
    return 0;
}




//ÊòæÁ§∫Â§ñËÆæÁ≠ñÁï•
int lan_Procy_Device(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	 pasDbCursor *psCur;
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16];
	  long iNum;
	  unsigned long lOptype,lSnap,lType,lSid,lPid,lModtime,lIndex,lFreehaordid;
	 char caPid[16],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[16],usbName[24];
	 char caOptype[16],caSnap[16],caLxtype[16],caFilename[128],caCreateuser[64],caFreehaordid[16];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,14,
	                 "pid",            UT_TYPE_STRING, 10, caPid,
			             "limit",            UT_TYPE_STRING, 10, caLimit,
			             "page",          UT_TYPE_STRING, 10, caPage,  
			             "dir",           UT_TYPE_STRING,8,caDir,
			             "keyword",       UT_TYPE_STRING,30,caKeyword,
			             "selsid",UT_TYPE_STRING,1000,caSelsid,
			             "filename",UT_TYPE_STRING,10,caFreehaordid,
			             "optype",UT_TYPE_STRING,10,caOptype,
			             "snap",UT_TYPE_STRING,10,caSnap,
			             "lxtype",UT_TYPE_STRING,10,caLxtype,
			             "update",UT_TYPE_STRING,10,caUpdate,
			             "sid",UT_TYPE_STRING,10,caSid,
			             "start",UT_TYPE_STRING,10,caStart,
			             "del",UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }          
			              

	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	  
	  psDbHead = utPltInitDb(); 
	  lOptype=atol(caOptype);
	  lSnap=atol(caSnap);
	 
	  lFreehaordid=atol(caFreehaordid);
	  
	  lType=atol(caLxtype);
	  if(strcmp(caUpdate,"update")==0){
	  	if(strlen(caSid)==0){
	  		sprintf(caTemp,"select sid from nwprocydev where pid=%lu and optype=%lu and freehaordid=%lu and snap=%lu and type=%d ",atol(caPid),lOptype, lFreehaordid,lSnap,lType);
	  		lSid=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid);
	  		if(lSid>0){
	  			pasDbExecSqlF("update nwprocydev set flag = 0,optype=%lu,freehaordid=%lu,snap=%d,type=%d,modtime=%d where sid=%lu  ",lOptype, lFreehaordid,lSnap,lType,time(0),lSid);
	 		writeSysLog(psMsgHead,"04","Success","…æ≥˝¡ÀÕ‚…Ë≤ﬂ¬‘");
	  		}
	  		else{
	  			 pasDbExecSqlF("insert into nwprocydev(pid,freehaordid,optype,snap,type,createtime,modtime,createuser) \
	       		values(%lu,%lu,%lu,%lu,%lu,%lu,%lu,'%s')",\
	       		atol(caPid), lFreehaordid,lOptype,lSnap,lType,time(0),time(0),"");
	 				writeSysLog(psMsgHead,"04","Success","‘ˆº”¡ÀÕ‚…Ë≤ﬂ¬‘");
	  		}
	  	}
	  	else{	  	
	  		    lSid=atol(caSid);	
	  				pasDbExecSqlF("update nwprocydev set flag = 0, optype=%lu,freehaordid=%lu,snap=%d,type=%d,modtime=%d where sid=%lu  ",lOptype, lFreehaordid,lSnap,lType,time(0),lSid);
	 			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒ¡ÀÕ‚…Ë≤ﬂ¬‘");
	  	}
	  	
	  }
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  	pasDbExecSqlF("update nwprocydev set flag = 8, modtime=%d where sid in(%s)",time(0),caSelsid);
	 		writeSysLog(psMsgHead,"04","Success","…æ≥˝¡ÀÕ‚…Ë≤ﬂ¬‘");
	  }
	  
	  
	  
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid where pid=%d ",atol(caPid));
   
    if(strlen(caKeyword)>0){
    	sprintf(caTemp+strlen(caTemp)," and freehaordid like '%c%s%c' ",'%',caKeyword,'%');
    }
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	  
	  
	    sprintf(caTemp,"select sid,pid,freehaordid,optype,snap,type,nwprocydev.modtime,createuser,name,id from nwprocydev left join nwusbclass on(id = freehaordid) where flag = 0 and pid=%d ",atol(caPid));
    	
     if(strlen(caKeyword)>0){
    	 sprintf(caTemp+strlen(caTemp)," and freehaordid like '%c%s%c' ",'%',caKeyword,'%');
     }
    	sprintf(caTemp+strlen(caTemp)," order by sid desc limit %d,%d",lStartRec,lRowNum);
    	
  	printf("caTemp=%s\n",caTemp);
    	

        psCur = pasDbOpenSql(caTemp,0);
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561 ˝æ›Õ≥º∆≥ˆ¥Ì"),ncsLang("0171≤È—Ø ˝æ›≥ˆ¥Ì"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   	
     lSid=0;
     lPid=0;
     lFreehaordid=0;
     lOptype=0;
     lSnap=0;
     lType=0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   	 
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                               UT_TYPE_LONG,4,&lFreehaordid,
      	                               UT_TYPE_ULONG,4,&lOptype,
      	                               UT_TYPE_LONG,4,&lSnap,
      	                               UT_TYPE_LONG,4,&lType,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser,
									   UT_TYPE_STRING,23,usbName);
      	                             
      	                              
      	                                                             
      	                              
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"optype",iNum,"%lu",lOptype);
        utPltPutLoopVarF(psDbHead,"snap",iNum,"%lu",lSnap);
        utPltPutLoopVarF(psDbHead,"type",iNum,"%lu",lType);
        utPltPutLoopVarF(psDbHead,"filename",iNum,"%lu",lFreehaordid);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
		utPltPutLoopVarF(psDbHead,"usbname",iNum,"%s",usbName);
        memset(caTemp,0,sizeof(caTemp));
        lIndex=0;
        if(lOptype&0x01){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp)," , ");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚”√U≈Ã");
        }
        if(lOptype&0x02){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp)," , ");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚÷πU≈Ã∂¡");
        }
        if(lOptype&0x04){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp)," , ");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚÷πU≈Ã–¥");
        }
		if(lOptype&0x08){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp)," , ");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚÷ππ‚≈ÃøÃ¬º ");
        }
        utPltPutLoopVar(psDbHead,"optype_desc",iNum,caTemp);
        if(lSnap==2){
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"Ωÿ∆¡");
        }
        else{
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"≤ªΩÿ∆¡");
        }
        if(lType==0){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿Îœﬂ”––ß");
        }
        else if(lType==1){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿ÎœﬂŒﬁ–ß");
        }
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
    pasDbCloseCursor(psCur);
	  
		utPltShowDb(psDbHead);
	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	  
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_file_list.htm");
	  return 0;
}

//ÊòæÁ§∫ÈùûÊ≥ïÂ§ñËÅîÁ≠ñÁï•
int lan_Procy_Link(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	 pasDbCursor *psCur;
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16];
	  long iNum;
	  unsigned long lOptype,lSnap,lType,lSid,lPid,lModtime,lIndex;
	 char caPid[16],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[16];
	 char caOptype[16],caSnap[16],caLxtype[16],caCreateuser[64];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,13,
	                 "pid",            UT_TYPE_STRING, 10, caPid,
			             "limit",            UT_TYPE_STRING, 10, caLimit,
			             "page",          UT_TYPE_STRING, 10, caPage,  
			             "dir",           UT_TYPE_STRING,8,caDir,
			             "keyword",       UT_TYPE_STRING,30,caKeyword,
			             "selsid",UT_TYPE_STRING,1000,caSelsid,
			         //    "filename",UT_TYPE_STRING,10,caFreehaordid,
			             "optype",UT_TYPE_STRING,10,caOptype,
			             "snap",UT_TYPE_STRING,10,caSnap,
			             "lxtype",UT_TYPE_STRING,10,caLxtype,
			             "update",UT_TYPE_STRING,10,caUpdate,
			             "sid",UT_TYPE_STRING,10,caSid,
			             "start",UT_TYPE_STRING,10,caStart,
			             "del",UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }          
			              

	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	  
	  psDbHead = utPltInitDb(); 
	  lOptype=atol(caOptype);
	  lSnap=atol(caSnap);
	//  lFreehaordid=atol(caFreehaordid);
	  lType=atol(caLxtype);
	  if(strcmp(caUpdate,"update")==0){
	  	if(strlen(caSid)==0){
	  		sprintf(caTemp,"select sid from nwprocylink where pid=%lu and optype=%lu and snap=%lu and type=%d ",atol(caPid),lOptype,lSnap,lType);
	  		lSid=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid);
	  		if(lSid>0){
	  			pasDbExecSqlF("update nwprocylink set flag=0,optype=%lu,snap=%d,type=%d,modtime=%d where sid=%lu  ",lOptype,lSnap,lType,time(0),lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒ¡À∑«∑®Õ‚¡™≤ﬂ¬‘");
	  		}
	  		else{
	  			 pasDbExecSqlF("insert into nwprocylink(pid,optype,snap,type,createtime,modtime,createuser) \
	       		values(%lu,%lu,%lu,%lu,%lu,%lu,'%s')",\
	       		atol(caPid), lOptype,lSnap,lType,time(0),time(0),"");
	  			writeSysLog(psMsgHead,"04","Success","‘ˆº”¡À∑«∑®Õ‚¡™≤ﬂ¬‘");
	  		}
	  	}
	  	else{	  	
	  		    lSid=atol(caSid);	
	  				pasDbExecSqlF("update nwprocylink set flag=0,optype=%lu,snap=%d,type=%d,modtime=%d where sid=%lu  ",lOptype,lSnap,lType,time(0),lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒ¡À∑«∑®Õ‚¡™≤ﬂ¬‘");
	  	}
	  	
	  }
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  	pasDbExecSqlF("update nwprocylink  set flag = 8, modtime=%d where sid in(%s)",time(0),caSelsid);
	  	writeSysLog(psMsgHead,"04","Success","…æ≥˝¡ÀŒƒº˛≤ﬂ¬‘");
	  }
	  
	  
	  
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid where pid=%d ",atol(caPid));
   
    
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	  
	  
	    sprintf(caTemp,"select sid,pid,optype,snap,type,modtime,createuser from nwprocylink  where flag = 0 and pid=%d ",atol(caPid));
    	sprintf(caTemp+strlen(caTemp)," order by sid desc limit %d,%d",lStartRec,lRowNum);
    	
  	printf("caTemp=%s\n",caTemp);
    	

        psCur = pasDbOpenSql(caTemp,0);
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561 ˝æ›Õ≥º∆≥ˆ¥Ì"),ncsLang("0171≤È—Ø ˝æ›≥ˆ¥Ì"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   	
     lSid=0;
     lPid=0;
     lOptype=0;
     lSnap=0;
     lType=0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   	 
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                               UT_TYPE_ULONG,4,&lOptype,
      	                               UT_TYPE_LONG,4,&lSnap,
      	                               UT_TYPE_LONG,4,&lType,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser);
      	                             
      	                              
      	                                                             
      	                              
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"optype",iNum,"%lu",lOptype);
        utPltPutLoopVarF(psDbHead,"snap",iNum,"%lu",lSnap);
        utPltPutLoopVarF(psDbHead,"type",iNum,"%lu",lType);
     //   utPltPutLoopVarF(psDbHead,"filename",iNum,"%lu",lFreehaordid);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        memset(caTemp,0,sizeof(caTemp));
        lIndex=0;
        if(lOptype&0x01){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp)," , ");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚÷π‘ˆº”Õ¯¬Á¡¨Ω”");
        }
        if(lOptype&0x02){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp)," , ");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚÷πWIFI");
        }
        if(lOptype&0x04){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp)," , ");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚÷πADSL");
        }
		if(lOptype&0x08){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp)," , ");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚÷πVPN");
        }
		if(lOptype&0x10){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp)," , ");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚÷π¿Îœﬂ¡¨Ω”Õ¯¬Á");
        }
        utPltPutLoopVar(psDbHead,"optype_desc",iNum,caTemp);
        if(lSnap==2){
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"Ωÿ∆¡");
        }
        else{
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"≤ªΩÿ∆¡");
        }
        if(lType==0){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿Îœﬂ”––ß");
        }
        else if(lType==1){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿ÎœﬂŒﬁ–ß");
        }
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
    pasDbCloseCursor(psCur);
	  

	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	  
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_file_list.htm");
	  return 0;
}

//ÊòæÁ§∫Â∫îÁî®Á®ãÂ∫èÁ≠ñÁï•
int lan_Procy_Pgtype(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	 pasDbCursor *psCur;
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16];
	  long iNum;
	  unsigned long lOptype,lStamp,lType,lSid,lPid,lModtime,lIndex,lGtype,lSnap;
	 char caPid[16],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[16],softName[24],caSnap[16];
	 char caOptype[16],caStamp[16],caLxtype[16],caGtype[128],caCreateuser[64];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,15,
	                 "pid",            UT_TYPE_STRING, 10, caPid,
			             "limit",            UT_TYPE_STRING, 10, caLimit,
			             "page",          UT_TYPE_STRING, 10, caPage,  
			             "dir",           UT_TYPE_STRING,8,caDir,
			             "keyword",       UT_TYPE_STRING,30,caKeyword,
			             "selsid",UT_TYPE_STRING,1000,caSelsid,
			             "gtype",UT_TYPE_STRING,127,caGtype,
			             "optype",UT_TYPE_STRING,10,caOptype,
						 "snap",UT_TYPE_STRING,10,caSnap,
			             "stamp",UT_TYPE_STRING,10,caStamp,
			             "lxtype",UT_TYPE_STRING,10,caLxtype,
			             "update",UT_TYPE_STRING,10,caUpdate,
			             "sid",UT_TYPE_STRING,10,caSid,
			             "start",UT_TYPE_STRING,10,caStart,
			             "del",UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }          
			              
		
	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	//  char * caPname_gbk = convert("UTF-8", "GBK", caPname);
	  lGtype=atol(caGtype);
	  psDbHead = utPltInitDb(); 
	  lOptype=atol(caOptype);
	  lSnap=atol(caSnap);
	  lStamp=atol(caStamp);
	  lType=atol(caLxtype);
	  if(strcmp(caUpdate,"update")==0){
	  	if(strlen(caSid)==0){
	  		sprintf(caTemp,"select sid from nwprocypgtype where pid=%lu and optype=%lu and snap=%lu and gtype=%lu and stamp=%lu and type=%d ",atol(caPid),lOptype,lSnap,lGtype,lStamp,lType);
	  		lSid=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid);
	  		if(lSid>0){
	  			pasDbExecSqlF("update nwprocypgtype set flag=0,optype=%lu,snap=%lu,gtype=%lu,stamp=%d,type=%d,modtime=%d where sid=%lu  ",lOptype,lSnap,lGtype,lStamp,lType,time(0),lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒ¡À”¶”√≥Ã–Ú≤ﬂ¬‘");
	  		}
	  		else{
	  			 pasDbExecSqlF("insert into nwprocypgtype(pid,gtype,optype,snap,stamp,type,createtime,modtime,createuser) \
	       		values(%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,'%s')",\
	       		atol(caPid),lGtype,lOptype,lSnap,lStamp,lType,time(0),time(0),"");
	  			writeSysLog(psMsgHead,"04","Success","ÃÌº”¡À”¶”√≥Ã–Ú≤ﬂ¬‘");
	  		}
	  	}
	  	else{	  	
	  		    lSid=atol(caSid);	
	  				pasDbExecSqlF("update nwprocypgtype set flag=0,optype=%lu,snap=%lu,gtype=%lu,stamp=%d,type=%d,modtime=%d where sid=%lu  ",lOptype,lSnap,lGtype,lStamp,lType,time(0),lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒ¡À”¶”√≥Ã–Ú≤ﬂ¬‘");
	  	}
	  	
	  }
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  	pasDbExecSqlF("update nwprocypgtype  set flag = 8, modtime=%d where sid in(%s)",time(0),caSelsid);
	  			writeSysLog(psMsgHead,"04","Success","…æ≥˝¡À”¶”√≥Ã–Ú≤ﬂ¬‘");
	  }
	  
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid where pid=%d ",atol(caPid));
   
    if(strlen(caKeyword)>0){
    	sprintf(caTemp+strlen(caTemp)," and gtype like '%c%s%c' ",'%',caKeyword,'%');
    }
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	  
	  
	    sprintf(caTemp,"select sid,pid,gtype,optype,snap,stamp,type,nwprocypgtype.modtime,createuser,name,id from nwprocypgtype  left join nwsoftclass on (id = gtype) where flag = 0 and pid=%d ",atol(caPid));
    	
     if(strlen(caKeyword)>0){
    	 sprintf(caTemp+strlen(caTemp)," and gtype like '%c%s%c' ",'%',caKeyword,'%');
     }
    	sprintf(caTemp+strlen(caTemp)," order by sid desc limit %d,%d",lStartRec,lRowNum);
    	
  	printf("caTemp=%s\n",caTemp);
    	

        psCur = pasDbOpenSql(caTemp,0);
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561 ˝æ›Õ≥º∆≥ˆ¥Ì"),ncsLang("0171≤È—Ø ˝æ›≥ˆ¥Ì"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   	
     lSid=0;
     lPid=0;
	 lGtype = 0;
   //  memset(caPname_gbk,0,sizeof(caPname_gbk));
     lOptype=0;
	 lSnap=0;
     lStamp=0;
     lType=0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   	 
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                               UT_TYPE_LONG,4,&lGtype,
      	                               UT_TYPE_ULONG,4,&lOptype,
									   UT_TYPE_LONG,4,&lSnap,
      	                               UT_TYPE_LONG,4,&lStamp,
      	                               UT_TYPE_LONG,4,&lType,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser,
									   UT_TYPE_STRING,23,softName);
      	                             
      	                              
      	                                                             
      	                              
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"optype",iNum,"%lu",lOptype);
		utPltPutLoopVarF(psDbHead,"snap",iNum,"%lu",lSnap);
        utPltPutLoopVarF(psDbHead,"stamp",iNum,"%lu",lStamp);
        utPltPutLoopVarF(psDbHead,"type",iNum,"%lu",lType);
        utPltPutLoopVarF(psDbHead,"gtype",iNum,"%lu",lGtype);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
		 utPltPutLoopVarF(psDbHead,"softname",iNum,"%s",softName);
        memset(caTemp,0,sizeof(caTemp));
        lIndex=0;
        if(lOptype == 2){
			  utPltPutLoopVar(psDbHead,"optype_desc",iNum,"∆Ù∂ØΩÿ∆¡");
		}
		else if(lOptype == 0){
		 utPltPutLoopVar(psDbHead,"optype_desc",iNum,"‘ –Ì‘À––");
		}
		else if(lOptype == 1){
		 utPltPutLoopVar(psDbHead,"optype_desc",iNum,"Ω˚÷π‘À––");
		}
     //   utPltPutLoopVar(psDbHead,"optype_desc",iNum,caTemp);
        if(lSnap==2){
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"Ωÿ∆¡");
        }
        else{
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"≤ªΩÿ∆¡");
        }
        if(lType==0){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿Îœﬂ”––ß");
        }
        else if(lType==1){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿ÎœﬂŒﬁ–ß");
        }
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
    pasDbCloseCursor(psCur);
	  
//	utPltShowDb(psDbHead);
	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	  
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_pgtype_list.htm");
	  return 0;
}



//ÊòæÁ§∫ËøõÁ®ãÁ≠ñÁï•
int lan_Procy_Pname(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	 pasDbCursor *psCur;
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16];
	  long iNum;
	  unsigned long lOptype,lStamp,lType,lSid,lPid,lModtime,lIndex,lSnap;
	 char caPid[16],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[16],caSnap[16];
	 char caOptype[16],caStamp[16],caLxtype[16],caPname[128],caCreateuser[64];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,15,
	                 "pid",            UT_TYPE_STRING, 10, caPid,
			             "limit",            UT_TYPE_STRING, 10, caLimit,
			             "page",          UT_TYPE_STRING, 10, caPage,  
			             "dir",           UT_TYPE_STRING,8,caDir,
			             "keyword",       UT_TYPE_STRING,30,caKeyword,
			             "selsid",UT_TYPE_STRING,1000,caSelsid,
			             "filename",UT_TYPE_STRING,127,caPname,
			             "optype",UT_TYPE_STRING,10,caOptype,
						  "snap",UT_TYPE_STRING,10,caSnap,
			             "stamp",UT_TYPE_STRING,10,caStamp,
			             "lxtype",UT_TYPE_STRING,10,caLxtype,
			             "update",UT_TYPE_STRING,10,caUpdate,
			             "sid",UT_TYPE_STRING,10,caSid,
			             "start",UT_TYPE_STRING,10,caStart,
			             "del",UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }          
			              
		
	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	  char * caPname_gbk = convert("UTF-8", "GBK", caPname);
	  
	  psDbHead = utPltInitDb(); 
	  lOptype=atol(caOptype);
	   lSnap=atol(caSnap);
	  lStamp=atol(caStamp);
	  lType=atol(caLxtype);
	  if(strcmp(caUpdate,"update")==0){
	  	if(strlen(caSid)==0){
	  		sprintf(caTemp,"select sid from nwprocypname where pid=%lu and optype=%lu and snap=%lu and pname='%s' and stamp=%lu and type=%d ",atol(caPid),lOptype,lSnap,caPname_gbk,lStamp,lType);
	  		lSid=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid);
	  		if(lSid>0){
	  			pasDbExecSqlF("update nwprocypname set flag=0,optype=%lu,snap=%lu,pname='%s',stamp=%d,type=%d,modtime=%d where sid=%lu  ",lOptype,lSnap,caPname_gbk,lStamp,lType,time(0),lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒΩ¯≥Ãøÿ÷∆≤ﬂ¬‘");
	  		}
	  		else{
	  			 pasDbExecSqlF("insert into nwprocypname(pid,pname,optype,snap,stamp,type,createtime,modtime,createuser) \
	       		values(%lu,'%s',%lu,%lu,%lu,%lu,%lu,%lu,'%s')",\
	       		atol(caPid),caPname_gbk,lOptype,lSnap,lStamp,lType,time(0),time(0),"");
	       		writeSysLog(psMsgHead,"04","Success","‘ˆº”Ω¯≥Ãøÿ÷∆≤ﬂ¬‘");
	  		}
	  	}
	  	else{	  	
	  		    lSid=atol(caSid);	
	  				pasDbExecSqlF("update nwprocypname set flag=0,optype=%lu,snap=%lu,pname='%s',stamp=%d,type=%d,modtime=%d where sid=%lu  ",lOptype,lSnap,caPname_gbk,lStamp,lType,time(0),lSid);
	  				writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒΩ¯≥Ãøÿ÷∆≤ﬂ¬‘");
	  	}
	  	
	  }
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  	pasDbExecSqlF("update nwprocypname set flag=8, modtime=%d where sid in(%s)",time(0),caSelsid);
	  	writeSysLog(psMsgHead,"04","Success","…æ≥˝Ω¯≥Ãøÿ÷∆≤ﬂ¬‘");
	  }
	  
	  
	  
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid where pid=%d ",atol(caPid));
   
    if(strlen(caKeyword)>0){
    	sprintf(caTemp+strlen(caTemp)," and pname like '%c%s%c' ",'%',caKeyword,'%');
    }
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	  
	  
	    sprintf(caTemp,"select sid,pid,pname,optype,snap,stamp,type,modtime,createuser from nwprocypname  where flag=0 and pid=%d ",atol(caPid));
    	
     if(strlen(caKeyword)>0){
    	 sprintf(caTemp+strlen(caTemp)," and pname like '%c%s%c' ",'%',caKeyword,'%');
     }
    	sprintf(caTemp+strlen(caTemp)," order by sid desc limit %d,%d",lStartRec,lRowNum);
    	
  	printf("caTemp=%s\n",caTemp);
    	

        psCur = pasDbOpenSql(caTemp,0);
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561 ˝æ›Õ≥º∆≥ˆ¥Ì"),ncsLang("0171≤È—Ø ˝æ›≥ˆ¥Ì"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   	
     lSid=0;
     lPid=0;
     memset(caPname_gbk,0,sizeof(caPname_gbk));
     lOptype=0;
	 lSnap=0;
     lStamp=0;
     lType=0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   	 
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                               UT_TYPE_STRING,127,caPname_gbk,
      	                               UT_TYPE_ULONG,4,&lOptype,
									   UT_TYPE_LONG,4,&lSnap,
      	                               UT_TYPE_LONG,4,&lStamp,
      	                               UT_TYPE_LONG,4,&lType,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser);
      	                             
      	                              
      	                                                             
      	                              
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"optype",iNum,"%lu",lOptype);
		utPltPutLoopVarF(psDbHead,"snap",iNum,"%lu",lSnap);
        utPltPutLoopVarF(psDbHead,"stamp",iNum,"%lu",lStamp);
        utPltPutLoopVarF(psDbHead,"type",iNum,"%lu",lType);
        utPltPutLoopVar(psDbHead,"filename",iNum,caPname_gbk);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        memset(caTemp,0,sizeof(caTemp));
        lIndex=0;
        if(lOptype&0x01){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp),",");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚÷π‘À––");
        }
        if(lOptype&0x02){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp),",");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"∆Ù∂ØΩÿ∆¡");
        }
        if(lOptype&0x04){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp),",");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"Ω˚÷πµº≥ˆŒƒº˛");
        }
		 if(lOptype&0x08){
        	if(lIndex>0){
        		sprintf(caTemp+strlen(caTemp),",");
        	}
        	lIndex++;
        	sprintf(caTemp+strlen(caTemp),"º«¬ºµº≥ˆŒƒº˛ƒ⁄»›");
        }
        utPltPutLoopVar(psDbHead,"optype_desc",iNum,caTemp);
        if(lSnap==2){
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"Ωÿ∆¡");
        }
        else{
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"≤ªΩÿ∆¡");
        }
        if(lType==0){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿Îœﬂ”––ß");
        }
        else if(lType==1){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿ÎœﬂŒﬁ–ß");
        }
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
    pasDbCloseCursor(psCur);
	  
	//	utPltShowDb(psDbHead);
	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	  
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_pname_list.htm");
	  return 0;
}





//ÊòæÁ§∫ÁΩëÂùÄÁ≠ñÁï•
int lan_Procy_Url(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
		pasDbCursor *psCur;
	
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16];
	  long iNum;
	  unsigned long lOptype,lStamp,lType,lSid,lPid,lModtime,lIndex,lUrlclass,lSnap;
	 char caPid[16],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[16],urlclassName[16],caSnap[16];
	 char caOptype[16],caUrlclass[16],caLxtype[16],caUrl[128],caCreateuser[64];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,15,
	                 "pid",            UT_TYPE_STRING, 10, caPid,
			             "limit",            UT_TYPE_STRING, 10, caLimit,
			             "page",          UT_TYPE_STRING, 10, caPage,  
			             "dir",           UT_TYPE_STRING,8,caDir,
			             "keyword",       UT_TYPE_STRING,30,caKeyword,
			             "selsid",UT_TYPE_STRING,1000,caSelsid,
			             "url",UT_TYPE_STRING,127,caUrl,
			             "optype",UT_TYPE_STRING,10,caOptype,
						 "snap",UT_TYPE_STRING,10,caSnap,
			             "urlclass",UT_TYPE_STRING,10,caUrlclass,
			             "lxtype",UT_TYPE_STRING,10,caLxtype,
			             "update",UT_TYPE_STRING,10,caUpdate,
			             "sid",UT_TYPE_STRING,10,caSid,
			             "start",UT_TYPE_STRING,10,caStart,
			             "del",UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }          
			              
		
	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	  char * caUrl_gbk = convert("UTF-8", "GBK", caUrl);
	  
	  psDbHead = utPltInitDb(); 
	  lOptype=atol(caOptype);
	  lSnap=atol(caSnap);
	  lUrlclass=atol(caUrlclass);
	  lType=atol(caLxtype);
	  if(strcmp(caUpdate,"update")==0){
	  	if(strlen(caSid)==0){
	  		sprintf(caTemp,"select sid from nwprocyurl where pid=%lu and optype=%lu and url='%s' and urlclass=%lu and type=%d ",atol(caPid),lOptype,caUrl_gbk,lUrlclass,lType);
	  		lSid=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid);
	  		if(lSid>0){
	  			pasDbExecSqlF("update nwprocyurl set flag=0, optype=%lu, snap=%lu, url='%s',urlclass=%d,type=%d,modtime=%d where sid=%lu  ",lOptype,lSnap,caUrl_gbk,lUrlclass,lType,time(0),lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒÕ¯÷∑øÿ÷∆≤ﬂ¬‘");
	  		}
	  		else{
	  			 pasDbExecSqlF("insert into nwprocyurl(pid,url,optype,snap,urlclass,type,createtime,modtime,createuser) \
	       		values(%lu,'%s',%lu,%lu,%lu,%lu,%lu,%lu,'%s')",\
	       		atol(caPid),caUrl_gbk,lOptype,lSnap,lUrlclass,lType,time(0),time(0),"");
	  			writeSysLog(psMsgHead,"04","Success","‘ˆº”Õ¯÷∑øÿ÷∆≤ﬂ¬‘");
	  		}
	  	}
	  	else{	  	
	  		    lSid=atol(caSid);	
	  				pasDbExecSqlF("update nwprocyurl set flag = 0, optype=%lu,snap=%lu,url='%s',urlclass=%d,type=%d,modtime=%d where sid=%lu  ",lOptype,lSnap,caUrl_gbk,lUrlclass,lType,time(0),lSid);
	  		writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒÕ¯÷∑øÿ÷∆≤ﬂ¬‘");
	  	}
	  	
	  }
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  	pasDbExecSqlF("update nwprocyurl set flag = 8, modtime=%d where sid in(%s)",time(0),caSelsid);
	  	writeSysLog(psMsgHead,"04","Success","…æ≥˝Õ¯÷∑øÿ÷∆≤ﬂ¬‘");
	  }
	  
	  
	  
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid where pid=%d ",atol(caPid));
   
    if(strlen(caKeyword)>0){
    	sprintf(caTemp+strlen(caTemp)," and url like '%c%s%c' ",'%',caKeyword,'%');
    }
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	  
	  
	    sprintf(caTemp,"select sid,pid,url,optype,snap,urlclass,type,nwprocyurl.modtime,createuser,name,id from nwprocyurl left join ncwebclass on (id = urlclass) where flag=0 and pid=%d ",atol(caPid));
    	
     if(strlen(caKeyword)>0){
    	 sprintf(caTemp+strlen(caTemp)," and url like '%c%s%c' ",'%',caKeyword,'%');
     }
    	sprintf(caTemp+strlen(caTemp)," order by sid desc limit %d,%d",lStartRec,lRowNum);
    	
  	printf("caTemp=%s\n",caTemp);
    	

        psCur = pasDbOpenSql(caTemp,0);
		
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561ÀΩﬂùÕ≥›Ü‘∂’≠"),ncsLang("0171”©—ØÀΩﬂù‘∂’≠"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   	
     lSid=0;
     lPid=0;
	 lSnap=0;
     memset(caUrl,0,sizeof(caUrl));
     lOptype=0;
     lUrlclass=0;
     lType=0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   	 
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                               UT_TYPE_STRING,127,caUrl,
      	                               UT_TYPE_ULONG,4,&lOptype,
									   UT_TYPE_ULONG,4,&lSnap,
      	                               UT_TYPE_LONG,4,&lUrlclass,
      	                               UT_TYPE_LONG,4,&lType,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser,
									   UT_TYPE_STRING,15,urlclassName);
      	                             
      	                              
      	                                                             
      	                              
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"optype",iNum,"%lu",lOptype);
		utPltPutLoopVarF(psDbHead,"snap",iNum,"%lu",lSnap);
        utPltPutLoopVarF(psDbHead,"urlclass",iNum,"%lu",lUrlclass);
        utPltPutLoopVarF(psDbHead,"type",iNum,"%lu",lType);
        utPltPutLoopVar(psDbHead,"url",iNum,caUrl);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
		utPltPutLoopVarF(psDbHead,"urlclass_desc",iNum,"%s",urlclassName);
      //  memset(caTemp,0,sizeof(caTemp));
      //  utPltPutLoopVar(psDbHead,"optype_desc",iNum,caTemp);
	  
		
		      if(lSnap==2){
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"Ωÿ∆¡");
        }
       else{
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"≤ªΩÿ∆¡");
        }
		
        if(lOptype==1){
        	utPltPutLoopVar(psDbHead,"optype_desc",iNum,"Ω˚÷π");
        }
        else if(lOptype==2){
        	utPltPutLoopVar(psDbHead,"optype_desc",iNum,"‘ –Ì");
        }
        if(lType==0){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿Îœﬂ”––ß");
        }
        else if(lType==1){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿ÎœﬂŒﬁ–ß");
        }
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
    pasDbCloseCursor(psCur);
	  utPltShowDb(psDbHead);
	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_url_list.htm");
	  return 0;
}



//ÊòæÁ§∫ÁΩëÁªúÁ≠ñÁï•
int lan_Procy_Net(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
		pasDbCursor *psCur;
	
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16];
	  long iNum;
	  unsigned long lOptype,lStamp,lType,lSid,lPid,lModtime,lIndex,lEport,lSport,lTrantype,lSnap;
	 char caPid[16],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[16],urlclassName[16],caSnap[16];
	 char caOptype[16],caEport[16],caLxtype[16],caSport[16],caCreateuser[64],caTrantype[16];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,16,
	                 "pid",            UT_TYPE_STRING, 10, caPid,
			             "limit",            UT_TYPE_STRING, 10, caLimit,
			             "page",          UT_TYPE_STRING, 10, caPage,  
			             "dir",           UT_TYPE_STRING,8,caDir,
			             "keyword",       UT_TYPE_STRING,30,caKeyword,
			             "selsid",UT_TYPE_STRING,1000,caSelsid,
						 "trantype",UT_TYPE_STRING,15,caTrantype,
			             "sport",UT_TYPE_STRING,15,caSport,
						 "eport",UT_TYPE_STRING,15,caEport,
			             "optype",UT_TYPE_STRING,10,caOptype,
						  "snap",UT_TYPE_STRING,10,caSnap,
			             "lxtype",UT_TYPE_STRING,10,caLxtype,
			             "update",UT_TYPE_STRING,10,caUpdate,
			             "sid",UT_TYPE_STRING,10,caSid,
			             "start",UT_TYPE_STRING,10,caStart,
			             "del",UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }          
			              
		
	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	//  char * caUrl_gbk = convert("UTF-8", "GBK", caUrl);
	  
	  psDbHead = utPltInitDb(); 
	  lTrantype=atol(caTrantype);
	  lOptype=atol(caOptype);
	    lSnap=atol(caSnap);
	  lSport=atol(caSport);
	  lEport=atol(caEport);
	  lType=atol(caLxtype);
	  if(strcmp(caUpdate,"update")==0){
	  	if(strlen(caSid)==0){
	  		sprintf(caTemp,"select sid from nwprocynet where pid=%lu and optype=%lu and snap=%lu and sport=%lu and eport=%lu and type=%d and trantype = %lu",atol(caPid),lOptype,lSnap,lSport,lEport,lType,lTrantype);
	  		lSid=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid);
	  		if(lSid>0){
	  			pasDbExecSqlF("update nwprocynet set flag=0, optype=%lu,snap=%lu,sport=%lu,eport=%d,type=%d,modtime=%d,trantype=%lu where sid=%lu  ",lOptype,lSnap,lSport,lEport,lType,time(0),lTrantype,lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒÕ¯¬Áøÿ÷∆≤ﬂ¬‘");
	  		}
	  		else{
	  			 pasDbExecSqlF("insert into nwprocynet(pid,sport,optype,snap,eport,type,createtime,modtime,createuser,trantype) \
	       		values(%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,'%s',%lu)",\
	       		atol(caPid),lSport,lOptype,lSnap,lEport,lType,time(0),time(0),"",lTrantype);
	       		writeSysLog(psMsgHead,"04","Success","‘ˆº”Õ¯¬Áøÿ÷∆≤ﬂ¬‘");
	  		}
	  	}
	  	else{	  	
	  		    lSid=atol(caSid);	
	  				pasDbExecSqlF("update nwprocynet set flag=0, optype=%lu,snap=%lu,sport=%lu,eport=%d,type=%d,modtime=%d,trantype=%lu where sid=%lu  ",lOptype,lSnap,lSport,lEport,lType,time(0),lTrantype,lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒÕ¯¬Áøÿ÷∆≤ﬂ¬‘");
	  	}
	  	
	  }
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  	pasDbExecSqlF("update nwprocynet set flag=8, modtime=%d where sid in(%s)",time(0),caSelsid);
	  	writeSysLog(psMsgHead,"04","Success","…æ≥˝Õ¯¬Áøÿ÷∆≤ﬂ¬‘");
	  }
	  
	  
	  
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid where pid=%d ",atol(caPid));
   
    
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	  
	  
	    sprintf(caTemp,"select sid,pid,sport,optype,snap,eport,type,modtime,createuser,trantype from nwprocynet where flag=0 and pid=%d ",atol(caPid));
    	
     
    	sprintf(caTemp+strlen(caTemp)," order by sid desc limit %d,%d",lStartRec,lRowNum);
    	
  	printf("caTemp=%s\n",caTemp);
    	

        psCur = pasDbOpenSql(caTemp,0);
		
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561ÀΩﬂùÕ≥›Ü‘∂’≠"),ncsLang("0171”©—ØÀΩﬂù‘∂’≠"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   	
     lSid=0;
     lPid=0;
	 lSport=0;
  //   memset(caUrl,0,sizeof(caUrl));
     lOptype=0;
	 lSnap=0;
     lEport=0;
     lType=0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   	 
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                               UT_TYPE_LONG,4,&lSport,
      	                               UT_TYPE_ULONG,4,&lOptype,
									    UT_TYPE_LONG,4,&lSnap,
      	                               UT_TYPE_LONG,4,&lEport,
      	                               UT_TYPE_LONG,4,&lType,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser,
									   UT_TYPE_LONG,4,&lTrantype);
      	                             
      	                              
      	                                                             
      	                              
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"optype",iNum,"%lu",lOptype);
		 utPltPutLoopVarF(psDbHead,"snap",iNum,"%lu",lSnap);
        utPltPutLoopVarF(psDbHead,"eport",iNum,"%lu",lEport);
        utPltPutLoopVarF(psDbHead,"type",iNum,"%lu",lType);
        utPltPutLoopVarF(psDbHead,"sport",iNum,"%lu",lSport);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
		utPltPutLoopVarF(psDbHead,"trantype",iNum,"%lu",lTrantype);
      //  memset(caTemp,0,sizeof(caTemp));
      //  utPltPutLoopVar(psDbHead,"optype_desc",iNum,caTemp);
	  
		 if(lSnap==2){
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"Ωÿ∆¡");
        }
       else{
        	utPltPutLoopVar(psDbHead,"snap_desc",iNum,"≤ªΩÿ∆¡");
        }
        if(lOptype==1){
        	utPltPutLoopVar(psDbHead,"optype_desc",iNum,"Ω˚÷π");
        }
        else if(lOptype==2){
        	utPltPutLoopVar(psDbHead,"optype_desc",iNum,"‘ –Ì");
        }
		
        if(lType==0){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿Îœﬂ”––ß");
        }
        else if(lType==1){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿ÎœﬂŒﬁ–ß");
        }
		if(lTrantype==17){
        	utPltPutLoopVar(psDbHead,"trantype_desc",iNum,"TCP");
        }
        else if(lTrantype==6){
        	utPltPutLoopVar(psDbHead,"trantype_desc",iNum,"UDP");
        }
		
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
    pasDbCloseCursor(psCur);
	  utPltShowDb(psDbHead);
	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_net_list.htm");
	  return 0;
}

//ÊòæÁ§∫ÂÖ∂‰ªñÁ≠ñÁï•
int lan_Procy_Oth(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
		pasDbCursor *psCur;
	
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16];
	  long iNum;
	  unsigned long lOptype,lStamp,lType,lSid,lPid,lModtime,lIndex,lFtime,lCtype;
	 char caPid[16],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[16],urlclassName[16];
	 char caOptype[16],caFtime[16],caLxtype[16],caCtype[16],caCreateuser[64];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,14,
	                 "pid",            UT_TYPE_STRING, 10, caPid,
			             "limit",            UT_TYPE_STRING, 10, caLimit,
			             "page",          UT_TYPE_STRING, 10, caPage,  
			             "dir",           UT_TYPE_STRING,8,caDir,
			             "keyword",       UT_TYPE_STRING,30,caKeyword,
			             "selsid",UT_TYPE_STRING,1000,caSelsid,
						
			             "ctype",UT_TYPE_STRING,15,caCtype,
						 "ftime",UT_TYPE_STRING,15,caFtime,
			             "optype",UT_TYPE_STRING,10,caOptype,
			             "lxtype",UT_TYPE_STRING,10,caLxtype,
			             "update",UT_TYPE_STRING,10,caUpdate,
			             "sid",UT_TYPE_STRING,10,caSid,
			             "start",UT_TYPE_STRING,10,caStart,
			             "del",UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }          
			              
		
	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	//  char * caUrl_gbk = convert("UTF-8", "GBK", caUrl);
	  
	  psDbHead = utPltInitDb(); 
	  lOptype=atol(caOptype);
	  lCtype=atol(caCtype);
	  lFtime=atol(caFtime);
	  lType=atol(caLxtype);
	  if(strcmp(caUpdate,"update")==0){
	  	if(strlen(caSid)==0){
	
	  		sprintf(caTemp,"select sid from nwprocyoth where pid=%lu and optype=%lu and ctype=%lu and ftime=%lu and type=%d",atol(caPid),lOptype,lCtype,lFtime,lType);
	  		lSid=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid);
			printf("lSid=%lu\n",lSid);
	  		if(lSid>0){
		
	  			pasDbExecSqlF("update nwprocyoth set flag=0, optype=%lu,ctype=%lu,ftime=%d,type=%d,modtime=%d where sid=%lu  ",lOptype,lCtype,lFtime,lType,time(0),lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒ∆¡ƒª°¢ µ ±¡ƒÃÏøÿ÷∆≤ﬂ¬‘");
	  		}
	  		else{
	
	  			 pasDbExecSqlF("insert into nwprocyoth(pid,ctype,optype,ftime,type,createtime,modtime,createuser) \
	       		values(%lu,%lu,%lu,%lu,%lu,%lu,%lu,'%s')",\
	       		atol(caPid),lCtype,lOptype,lFtime,lType,time(0),time(0),"");
	       		writeSysLog(psMsgHead,"04","Success","‘ˆº”∆¡ƒª°¢ µ ±¡ƒÃÏøÿ÷∆≤ﬂ¬‘");
	  		}
	  	}
	  	else{	 
		
	  		    lSid=atol(caSid);	
	  				pasDbExecSqlF("update nwprocyoth set flag=0, optype=%lu,ctype=%lu,ftime=%d,type=%d,modtime=%d where sid=%lu  ",lOptype,lCtype,lFtime,lType,time(0),lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒ∆¡ƒª°¢ µ ±¡ƒÃÏøÿ÷∆≤ﬂ¬‘");
	  	}
	  	
	  }
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  	pasDbExecSqlF("update nwprocyoth set flag=8, modtime=%d where sid in(%s)",time(0),caSelsid);
	  	writeSysLog(psMsgHead,"04","Success","…æ≥˝∆¡ƒª°¢ µ ±¡ƒÃÏøÿ÷∆≤ﬂ¬‘");
	  }
	  
	  
	  
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid where pid=%d ",atol(caPid));
   
    
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	  
	  
	    sprintf(caTemp,"select sid,pid,ctype,optype,ftime,type,modtime,createuser from nwprocyoth where flag=0 and pid=%d ",atol(caPid));
    	
     
    	sprintf(caTemp+strlen(caTemp)," order by sid desc limit %d,%d",lStartRec,lRowNum);
    	
  	printf("caTemp=%s\n",caTemp);
    	

        psCur = pasDbOpenSql(caTemp,0);
		
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561ÀΩﬂùÕ≥›Ü‘∂’≠"),ncsLang("0171”©—ØÀΩﬂù‘∂’≠"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   	
     lSid=0;
     lPid=0;
	 lCtype=0;
  //   memset(caUrl,0,sizeof(caUrl));
     lOptype=0;
     lFtime=0;
     lType=0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   	 
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                               UT_TYPE_LONG,4,&lCtype,
      	                               UT_TYPE_ULONG,4,&lOptype,
      	                               UT_TYPE_LONG,4,&lFtime,
      	                               UT_TYPE_LONG,4,&lType,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser
									   );
      	                             
      	                              
      	                                                             
      	                              
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"optype",iNum,"%lu",lOptype);
        utPltPutLoopVarF(psDbHead,"ftime",iNum,"%lu",lFtime);
        utPltPutLoopVarF(psDbHead,"type",iNum,"%lu",lType);
        utPltPutLoopVarF(psDbHead,"ctype",iNum,"%lu",lCtype);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
      //  memset(caTemp,0,sizeof(caTemp));
      //  utPltPutLoopVar(psDbHead,"optype_desc",iNum,caTemp);
	  
		
        if(lOptype==0){
        	utPltPutLoopVar(psDbHead,"optype_desc",iNum,"Õ£”√");
        }
        else if(lOptype==1){
        	utPltPutLoopVar(psDbHead,"optype_desc",iNum,"∆Ù”√");
        }
        if(lType==0){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿Îœﬂ”––ß");
        }
        else if(lType==1){
        	utPltPutLoopVar(psDbHead,"type_desc",iNum,"¿ÎœﬂŒﬁ–ß");
        }
		if(lCtype==1){
        	utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"∆¡ƒª");
        }
        else if(lCtype==2){
        	utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"IMº‡øÿ");
        }
		
    //    utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
    pasDbCloseCursor(psCur);
//	  utPltShowDb(psDbHead);
	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_oth_list.htm");
	  return 0;
}

//ÊòæÁ§∫Â∫îÁî®Á®ãÂ∫èÁ≠ñÁï•
int lan_Procy_Ref(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	 pasDbCursor *psCur;
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16];
	  long iNum;
	  unsigned long lOptype,lStamp,lType,lSid,lPid,lModtime,lIndex,lOthid;
	 char caPid[16],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[16],lawName[24];
	 char caOptype[16],caStamp[16],caLxtype[16],caOthid[16],caCreateuser[64];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,11,
	                 "pid",            UT_TYPE_STRING, 10, caPid,
			             "limit",            UT_TYPE_STRING, 10, caLimit,
			             "page",          UT_TYPE_STRING, 10, caPage,  
			             "dir",           UT_TYPE_STRING,8,caDir,
			             "keyword",       UT_TYPE_STRING,30,caKeyword,
			             "selsid",UT_TYPE_STRING,1000,caSelsid,
			             "othid",UT_TYPE_STRING,15,caOthid,
			             "update",UT_TYPE_STRING,10,caUpdate,
			             "sid",UT_TYPE_STRING,10,caSid,
			             "start",UT_TYPE_STRING,10,caStart,
			             "del",UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }	
	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	//  char * caPname_gbk = convert("UTF-8", "GBK", caPname);
	  lOthid=atol(caOthid);
	  psDbHead = utPltInitDb(); 
	  lOptype=atol(caOptype);
	  lStamp=atol(caStamp);
	  lType=atol(caLxtype);
	  if(strcmp(caUpdate,"update")==0){
	  	if(strlen(caSid)==0){
			printf("pid =%lu, othid=%lu\n",atol(caPid),lOthid);
	  		sprintf(caTemp,"select sid from nwprocyref where pid=%lu and othid=%lu ",atol(caPid),lOthid);
	  		lSid=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid);
	  		if(lSid>0){
	  			pasDbExecSqlF("update nwprocyref set flag=0, othid=%lu, modtime=%d where sid=%lu  ",lOthid,time(0),lSid);
	  			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒ◊‘∂®“Â≤ﬂ¬‘");
	  		}
	  		else{
	  			 pasDbExecSqlF("insert into nwprocyref(pid,othid,createtime,modtime,createuser) \
	       		values(%lu,%lu,%lu,%lu,'%s')",\
	       		atol(caPid),lOthid,time(0),time(0),"");
	       		writeSysLog(psMsgHead,"04","Success","‘ˆº”◊‘∂®“Â≤ﬂ¬‘");
	  		}
	  	}
	  	else{	  	
	  		    lSid=atol(caSid);	
	  				pasDbExecSqlF("update nwprocyref set flag=0, othid=%lu,modtime=%d where sid=%lu  ",lOthid,time(0),lSid);
	       		writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒ◊‘∂®“Â≤ﬂ¬‘");
	  	}
	  	
	  }
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  	pasDbExecSqlF("update nwprocyref set flag=8, modtime=%d where sid in(%s)",time(0),caSelsid);
	    writeSysLog(psMsgHead,"04","Success","…æ≥˝◊‘∂®“Â≤ﬂ¬‘");
	  }
	  
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid where pid=%d ",atol(caPid));
   
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	  
	  
	    sprintf(caTemp,"select nwprocyref.sid as sid,pid,othid,nwprocyref.modtime,nwprocyref.createuser, lawname, nwmainprocy.sid as sid2 from nwprocyref left join nwmainprocy on (nwmainprocy.sid = nwprocyref.othid) where nwprocyref.flag=0 and pid=%d ",atol(caPid));
    	
     
    	sprintf(caTemp+strlen(caTemp)," order by sid desc limit %d,%d",lStartRec,lRowNum);
    	
  	printf("caTemp=%s\n",caTemp);
    	

        psCur = pasDbOpenSql(caTemp,0);
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561 ˝æ›Õ≥º∆≥ˆ¥Ì"),ncsLang("0171≤È—Ø ˝æ›≥ˆ¥Ì"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   	
     lSid=0;
     lPid=0;
	 lOthid = 0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   //	 memset(lawName,0,sizeof(lawName));
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                               UT_TYPE_LONG,4,&lOthid,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser,
									   UT_TYPE_STRING,23,lawName);
           
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"othid",iNum,"%lu",lOthid);
		 utPltPutLoopVarF(psDbHead,"lawname",iNum,"%s",lawName);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
		pasDbCloseCursor(psCur);
	//  utPltShowDb(psDbHead);
	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_ref_list.htm");
	  return 0;
}



//ÁΩëÂùÄÂ∫ì

int lan_WebClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	printf("webclass start!!!!\n");
	 pasDbCursor *psCur;
	// utPltDbHead *psDbHead;
	 utPltDbHead *psDbHead = utPltInitDb();
	char caTemp[128];
	sprintf(caTemp,"select id, name from ncwebclass ");	
  	printf("caTemp=%s\n",caTemp);
	
    psCur = pasDbOpenSql(caTemp,0);
	if(psCur != NULL)
	{
	int iReturn=0;
   	int iNum=0;
	long id =0;
	char name[24];
	while(0==(iReturn = pasDbFetchInto(psCur,
      	                     UT_TYPE_LONG,4,&id,
      	                     UT_TYPE_STRING,23,name))  || 1405==iReturn)
	 {
	//	printf("iReturn = %d\n",iReturn);									   
        iNum++;
	//	printf("iNum = %d\n",iNum);
      if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
	//	printf("2222222222222\n");
        utPltPutLoopVarF(psDbHead,"id",iNum,"%lu", id);
		//	printf("3333333333333333\n");
		utPltPutLoopVarF(psDbHead,"name",iNum,"%s",name);
	//		printf("4444444444\n");
		}
	//	printf("5555555555\n");
		pasDbCloseCursor(psCur);
	}
	
	//	printf("webclass  omf!!!!\n");
	//	utPltShowDb(psDbHead);
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_webclass_list.htm");
		printf("webclass  end!!!!\n");
      
	  return 0;

}

//USBÂ∫ì

int lan_UsbClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	printf("usbclass start!!!!\n");
	 pasDbCursor *psCur;
	// utPltDbHead *psDbHead;
	 utPltDbHead *psDbHead = utPltInitDb();
	char caTemp[128];
	sprintf(caTemp,"select id, name from nwusbclass ");	
  	printf("caTemp=%s\n",caTemp);
	
    psCur = pasDbOpenSql(caTemp,0);
	if(psCur != NULL)
	{
	int iReturn=0;
   	int iNum=0;
	long id =0;
	char name[24];
	while(0==(iReturn = pasDbFetchInto(psCur,
      	                     UT_TYPE_LONG,4,&id,
      	                     UT_TYPE_STRING,23,name))  || 1405==iReturn)
	 {								   
        iNum++;
      if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"id",iNum,"%lu", id);
		utPltPutLoopVarF(psDbHead,"name",iNum,"%s",name);
		}
		pasDbCloseCursor(psCur);
	}

		utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_webclass_list.htm");
		printf("usbclass  end!!!!\n");
      
	  return 0;

}

//ËΩØ‰ª∂Á±ªÂûãÂ∫ì

int lan_SoftClass(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	printf("softclass start!!!!\n");
	 pasDbCursor *psCur;
	 utPltDbHead *psDbHead = utPltInitDb();
	char caTemp[128];
	sprintf(caTemp,"select id, name from nwsoftclass");	
  	printf("caTemp=%s\n",caTemp);
	
    psCur = pasDbOpenSql(caTemp,0);
	if(psCur != NULL)
	{
	int iReturn=0;
   	int iNum=0;
	long id =0;
	char name[24];
	while(0==(iReturn = pasDbFetchInto(psCur,
      	                     UT_TYPE_LONG,4,&id,
      	                     UT_TYPE_STRING,23,name))  || 1405==iReturn)
	 {								   
        iNum++;
      if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"id",iNum,"%lu", id);
		utPltPutLoopVarF(psDbHead,"name",iNum,"%s",name);
		}
		pasDbCloseCursor(psCur);
	}
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_webclass_list.htm");
		printf("softclass  end!!!!\n");
      
	  return 0;

}

//‰∏ªÁ≠ñÁï•

int lan_MainProcy(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	printf("mainprocy start!!!!\n");
	 pasDbCursor *psCur;
	 utPltDbHead *psDbHead = utPltInitDb();
	 char caPid[24];
	 long lPid;
	utMsgPrintMsg(psMsgHead);
	utMsgGetSomeNVar(psMsgHead,1,
                        "pid",  UT_TYPE_STRING, 23, caPid);
	 
	 
	char caTemp[128];
	lPid=atol(caPid);
	printf("pid = %d \n",lPid);
	sprintf(caTemp,"select sid, lawname from nwmainprocy where flag=0 and sid <> %d",lPid);	
  	printf("caTemp=%s\n",caTemp);
	
    psCur = pasDbOpenSql(caTemp,0);
	if(psCur != NULL)
	{
	int iReturn=0;
   	int iNum=0;
	long id =0;
	char name[24];
	while(0==(iReturn = pasDbFetchInto(psCur,
      	                     UT_TYPE_LONG,4,&id,
      	                     UT_TYPE_STRING,23,name))  || 1405==iReturn)
	 {								   
        iNum++;
      if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"id",iNum,"%lu", id);
		utPltPutLoopVarF(psDbHead,"name",iNum,"%s",name);
		}
		pasDbCloseCursor(psCur);
	}
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_webclass_list.htm");
		printf("mainprocy  end!!!!\n");   
	  return 0;
}

//Êó∂Èó¥ÂØπË±°Â∫ì

int lan_Datedes(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	printf("datedes start!!!!\n");
	 pasDbCursor *psCur;
	 utPltDbHead *psDbHead = utPltInitDb();
	char caTemp[128];
	sprintf(caTemp,"select id, name from nwdatedes");	
  	printf("caTemp=%s\n",caTemp);
	
    psCur = pasDbOpenSql(caTemp,0);
	if(psCur != NULL)
	{
	int iReturn=0;
   	int iNum=0;
	long id =0;
	char name[24];
	while(0==(iReturn = pasDbFetchInto(psCur,
      	                     UT_TYPE_LONG,4,&id,
      	                     UT_TYPE_STRING,23,name))  || 1405==iReturn)
	 {								   
        iNum++;
      if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"id",iNum,"%lu", id);
		utPltPutLoopVarF(psDbHead,"name",iNum,"%s",name);
		}
		pasDbCloseCursor(psCur);
	}
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_webclass_list.htm");
		printf("datedes  end!!!!\n");    
	  return 0;
}

//Êó∂Èó¥ÂØπË±°Â∫ì

int lan_Groups(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	printf("Groups start!!!!\n");
	 pasDbCursor *psCur;
	 utPltDbHead *psDbHead = utPltInitDb();
	char caTemp[128];
	sprintf(caTemp,"select gid, gname from nwgroup");	
  	printf("caTemp=%s\n",caTemp);
	
    psCur = pasDbOpenSql(caTemp,0);
	if(psCur != NULL)
	{
	int iReturn=0;
   	int iNum=0;
	long id =0;
	char name[24];
	while(0==(iReturn = pasDbFetchInto(psCur,
      	                     UT_TYPE_LONG,8,&id,
      	                     UT_TYPE_STRING,23,name))  || 1405==iReturn)
	 {								   
        iNum++;
      if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"id",iNum,"%lu", id);
		utPltPutLoopVarF(psDbHead,"name",iNum,"%s",name);
		}
		pasDbCloseCursor(psCur);
	}
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_webclass_list.htm");
		printf("Groups  end!!!!\n");    
	  return 0;
}

//Á≠ñÁï•‰∏ãÂèë
int lan_Procy_Obj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	 pasDbCursor *psCur;
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16],sort[16],dir[16];
	  long iNum;
	  unsigned long lCtype,lEtime,lDataid,lSid,lPid,lModtime,lIndex,lStime,lId;
	 char caPid[1024],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[1024],dateName[32],caId[2048],lawName[24], compName[32],gName[32];
	 char caCtype[1024],caEtime[16],caDataid[16],caStime[16],caCreateuser[64];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,17,
						 "pid",     UT_TYPE_STRING, 1023, caPid,
			             "limit",   UT_TYPE_STRING, 10, caLimit,
						  "sort",    UT_TYPE_STRING,  sizeof(sort)-1,        sort,
						"dir",      UT_TYPE_STRING,  sizeof(dir)-1,         dir,
			             "page",    UT_TYPE_STRING, 10, caPage,  
			             "dir",    	UT_TYPE_STRING,8,caDir,
			             "keyword", UT_TYPE_STRING,30,caKeyword,
			             "selsid",	UT_TYPE_STRING,1023,caSelsid,
			             "stime",	UT_TYPE_STRING,15,caStime,
			             "ctype",	UT_TYPE_STRING,1023,caCtype,
						 "id",		UT_TYPE_STRING,2047,caId,
			             "etime",	UT_TYPE_STRING,15,caEtime,
			             "dataid",	UT_TYPE_STRING,15,caDataid,
			             "update",	UT_TYPE_STRING,10,caUpdate,
			             "sid",		UT_TYPE_STRING,1023,caSid,
			             "start",	UT_TYPE_STRING,10,caStart,
			             "del",		UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }          
	//	printf("%s\n",caStime);	              
		char _caStime[16];
		char _caEtime[16];
		int i=0;
		int j=0;
		for(i=0;i<10;i++){	
		if(caStime[i] != 58){
		_caStime[j] = caStime[i];
		j++;
		}
		}
		printf("_caStime = %s\n",_caStime);
		j=0;
		for(i=0;i<10;i++){	
		if(caEtime[i] != 58){
		_caEtime[j] = caEtime[i];
		j++;
		}
		}
		printf("_caEtime = %s\n",_caEtime);
		printf("caCtype = %s\n",caCtype);
		
		
		//¿‡–Õ
		char _caCtype[16];
		long _lCtype[200];
		int k=0;
		j=0;
		for(i=0; i<strlen(caCtype); i++){
		if(caCtype[i] != 44){
		_caCtype[j]=caCtype[i];
		j++;
		}else{
		_caCtype[j]="\0";
		lCtype=atol(_caCtype);
		_lCtype[k] = lCtype;
		k++;
		j=0;
		}
		}
		
		for(i=0; i<k; i++)
		printf("_lCtype = %lu\n", _lCtype[i]);
		
		//œ¬∑¢∂‡∏ˆ∂‘œÛ
		char _caId[16];
		long _lId[200];
		printf("caId = %s\n",caId);	
		j=0;
		k=0;
		for(i=0;i<strlen(caId);i++){
		if(caId[i] != 44){
		_caId[j] = caId[i];
		j++;
		}
		else{
		_caId[j]="\0";
		if(_lCtype[k] == 2){
		lId = strtoul(_caId,NULL,10);
		_lId[k] = lId;
		}else if(_lCtype[k] == 1){
	//	printf("_caId = %s\n",_caId);
		lId =atol(_caId);
	//	printf("lId = %lu \n",lId);
		_lId[k] = lId;
		}else if(_lCtype[k] == 0)
		{
		_lId[k] = 0;
		}
	//	_caId = NULL;
		k++;
		j=0;
		}
		}
		for(i=0; i<k; i++)
		printf("_lId = %lu\n", _lId[i]);
		//÷˜≤ﬂ¬‘
		char _caPid[16];
		long _lPid[200];
		int p=0;
		j=0;
		for(i=0; i<strlen(caPid); i++){
		if(caPid[i] != 44){
		_caPid[j]=caPid[i];
		j++;
		}else{
		_caPid[j]="\0";
		lPid=atol(_caPid);
		_lPid[p] = lPid;
		p++;
		j=0;
		}
		}
		
		for(i=0; i<p; i++)
		printf("_lPid = %lu\n", _lPid[i]);
		
		printf("sort1 = %s \n",sort);
		if(strcmp(sort,"ctype_desc")==0){
			strcpy(sort ,"ctype");
			printf("sort2 = %s \n",sort);
		}
		if(strcmp(sort,"id_desc")==0){
			strcpy(sort ,"id1");
			printf("sort2 = %s \n",sort);
		}
		
	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	//  char * caPname_gbk = convert("UTF-8", "GBK", caPname);
	  lStime=atol(_caStime);
	  psDbHead = utPltInitDb(); 
	  
	//	printf("caId = %s\n",caId);
	//	lId=strtoul(caId,NULL,10);
	//	printf("lId = %lu\n",lId);
	  
	  lEtime=atol(_caEtime);
	  lDataid=atol(caDataid);
	  
	  if(strcmp(caUpdate,"update")==0){
	  
		for(j=0;j<p;j++){
	  	if(strlen(caSid)==0){
		for(i=0;i<k;i++){
	  		sprintf(caTemp,"select sid from nwprocyobj where pid=%lu and ctype=%lu and id=%lu ",_lPid[j],_lCtype[i],_lId[i]);
	  		lSid=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid);
	  		if(lSid>0){
				printf("update\n");
	  			pasDbExecSqlF("update nwprocyobj set pid=%lu,ctype=%lu,id=%lu,stime=%lu,etime=%d,dataid=%d,modtime=%d where sid=%lu  ",_lPid[j],_lCtype[i],_lId[i],lStime,lEtime,lDataid,time(0),lSid);
	  		}
	  		else{
				printf("insert\n");
	  			pasDbExecSqlF("insert into nwprocyobj(pid,stime,ctype,id,etime,dataid,createtime,modtime,createuser) \
	       		values(%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,'%s')",\
	       		_lPid[j],lStime,_lCtype[i],_lId[i],lEtime,lDataid,time(0),time(0),"");
	  		}
			}
	  	}
	  	else{	  	
	  		    lSid=atol(caSid);	
	  				pasDbExecSqlF("update nwprocyobj set pid=%lu, ctype=%lu,id=%lu,stime=%lu,etime=%d,dataid=%d,modtime=%d where sid=%lu  ",_lPid[j],lCtype,lId,lStime,lEtime,lDataid,time(0),lSid);
	  	}
	  }

	  
	  }
	  
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  	pasDbExecSqlF("delete from nwprocyobj  where sid in(%s)",caSelsid);
	  }
	  
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid");
   
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	  
	  
	    sprintf(caTemp,"select nwprocyobj.sid as sid1,nwprocyobj.pid,ctype,nwprocyobj.id as id1,nwprocyobj.stime,nwprocyobj.etime,nwprocyobj.dataid,nwprocyobj.modtime,nwprocyobj.createuser,name,lawname,compname,gname,nwdatedes.id,nwmainprocy.sid, compid,gid from nwprocyobj  left join nwdatedes on (dataid = nwdatedes.id) left join nwmainprocy on (nwmainprocy.sid = nwprocyobj.pid) left join nwcompute on (compid = nwprocyobj.id) left join nwgroup on(gid = nwprocyobj.id)");
    	
    
    sprintf(caTemp+strlen(caTemp)," order by %s %s limit %d,%d",sort,dir,lStartRec,lRowNum);
    	
  	printf("caTemp=%s\n",caTemp);
    	

        psCur = pasDbOpenSql(caTemp,0);
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561 ˝æ›Õ≥º∆≥ˆ¥Ì"),ncsLang("0171≤È—Ø ˝æ›≥ˆ¥Ì"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   	
     lSid=0;
     lPid=0;
	 lStime = 0;
   //  memset(caPname_gbk,0,sizeof(caPname_gbk));
     lCtype=0;
	 lId=0;
     lEtime=0;
     lDataid=0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   	 
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                             
      	                               UT_TYPE_ULONG,4,&lCtype,
									   UT_TYPE_LONG,4,&lId,
									   UT_TYPE_LONG,4,&lStime,
      	                               UT_TYPE_LONG,4,&lEtime,
      	                               UT_TYPE_LONG,4,&lDataid,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser,
									   UT_TYPE_STRING,31,dateName,
									    UT_TYPE_STRING,23,lawName,
										UT_TYPE_STRING,31,compName,
										UT_TYPE_STRING,31,gName);
      	                             
      	                              
      	                                                             
      	                              
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"ctype",iNum,"%lu",lCtype);
		utPltPutLoopVarF(psDbHead,"id",iNum,"%lu",lId);
        utPltPutLoopVarF(psDbHead,"etime",iNum,"%lu",lEtime);
        utPltPutLoopVarF(psDbHead,"dataid",iNum,"%lu",lDataid);
        utPltPutLoopVarF(psDbHead,"stime",iNum,"%lu",lStime);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
		if(lDataid == 0){
		utPltPutLoopVar(psDbHead,"datename",iNum,"Œﬁ");
		}
		else{
		 utPltPutLoopVarF(psDbHead,"datename",iNum,"%s",dateName);
		 }
		 utPltPutLoopVarF(psDbHead,"lawname",iNum,"%s",lawName);
		  utPltPutLoopVarF(psDbHead,"compname",iNum,"%s",compName);
		  utPltPutLoopVarF(psDbHead,"gname",iNum,"%s",gName);
		  if(lCtype == 1){
		    utPltPutLoopVarF(psDbHead,"id_desc",iNum,"%s",gName);
		  }
		  else if(lCtype == 2){
			utPltPutLoopVarF(psDbHead,"id_desc",iNum,"%s",compName);
		  }else	if(lCtype==0){
        	utPltPutLoopVar(psDbHead,"id_desc",iNum,"»´≤ø");
        }
     //   memset(caTemp,0,sizeof(caTemp));
        lIndex=0;

		long hour,min,sec;
		hour = lStime/10000;
		min = (lStime%10000)/100;
		sec = (lStime%10000)%100;
	//	printf("hour = %u, min = %u, sec = %u \n",hour,min,sec);
	
		if(hour < 10){
		sprintf(caTemp,"0%u",hour);
		}
		else{
		sprintf(caTemp,"%u",hour);
		}
		if(min < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",min);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",min);
		}	
		if(sec < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",sec);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",sec);
		}
	//	printf("caTemp=%s\n",caTemp);
		utPltPutLoopVarF(psDbHead,"stime_desc",iNum,"%s",caTemp);
	
		//long hour,min,sec;
		hour = lEtime/10000;
		min = (lEtime%10000)/100;
		sec = (lEtime%10000)%100;
	//	printf("hour = %u, min = %u, sec = %u \n",hour,min,sec);
		
		if(hour < 10){
		sprintf(caTemp,"0%u",hour);
		}
		else{
		sprintf(caTemp,"%u",hour);
		}
		if(min < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",min);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",min);
		}	
		if(sec < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",sec);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",sec);
		}
	//	printf("caTemp=%s\n",caTemp);
		utPltPutLoopVarF(psDbHead,"etime_desc",iNum,"%s",caTemp);	
		
		
        if(lCtype == 2){
			  utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"º∆À„ª˙");
		}
		else if(lCtype == 0){
		 utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"»´≤ø");
		}
		else if(lCtype == 1){
		 utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"≤ø√≈");
		}
     //   utPltPutLoopVar(psDbHead,"optype_desc",iNum,caTemp);
	
      

        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
    pasDbCloseCursor(psCur);
	  
	// utPltShowDb(psDbHead);
	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	  
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_obj_list.htm");
	  return 0;
}

//Á≠ñÁï•‰∏ãÂèë
int lan_Procy_Obj2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	 pasDbCursor *psCur;
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16],sort[16],dir[16];
	  long iNum;
	  unsigned long lCtype,lEtime,lDataid,lSid,lPid,lModtime,lIndex,lStime,lId;
	  char caPid[1024],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[1024],dateName[32],caId[2048],lawName[24], compName[32],gName[32];
	  char caCtype[1024],caEtime[16],caDataid[16],caStime[16],caCreateuser[64];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,17,
						 "pid",     UT_TYPE_STRING, 1023, caPid,
			             "limit",   UT_TYPE_STRING, 10, caLimit,
						  "sort",    UT_TYPE_STRING,  sizeof(sort)-1,        sort,
						"dir",      UT_TYPE_STRING,  sizeof(dir)-1,         dir,
			             "page",    UT_TYPE_STRING, 10, caPage,  
			             "dir",    	UT_TYPE_STRING,8,caDir,
			             "keyword", UT_TYPE_STRING,30,caKeyword,
			             "selsid",	UT_TYPE_STRING,1023,caSelsid,
			             "stime",	UT_TYPE_STRING,15,caStime,
			             "ctype",	UT_TYPE_STRING,1023,caCtype,
						 "id",		UT_TYPE_STRING,2047,caId,
			             "etime",	UT_TYPE_STRING,15,caEtime,
			             "dataid",	UT_TYPE_STRING,15,caDataid,
			             "update",	UT_TYPE_STRING,10,caUpdate,
			             "sid",		UT_TYPE_STRING,1023,caSid,
			             "start",	UT_TYPE_STRING,10,caStart,
			             "del",		UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }          
	//	printf("%s\n",caStime);	              
		char _caStime[16];
		char _caEtime[16];
		int i=0;
		int j=0;
		for(i=0;i<10;i++){	
		if(caStime[i] != 58){
		_caStime[j] = caStime[i];
		j++;
		}
		}
		printf("_caStime = %s\n",_caStime);
		j=0;
		for(i=0;i<10;i++){	
		if(caEtime[i] != 58){
		_caEtime[j] = caEtime[i];
		j++;
		}
		}
		printf("_caEtime = %s\n",_caEtime);
		printf("caCtype = %s\n",caCtype);
		
		
		//¿‡–Õ
		char _caCtype[16];
		long _lCtype[200];
		int k=0;
		j=0;
		for(i=0; i<strlen(caCtype); i++){
		if(caCtype[i] != 44){
		_caCtype[j]=caCtype[i];
		j++;
		}else{
		_caCtype[j]="\0";
		lCtype=atol(_caCtype);
		printf("lCtype = %lu", lCtype);
		_lCtype[k] = lCtype;
		k++;
		j=0;
		}
		}
		
		for(i=0; i<k; i++){
		printf("_lCtype = %lu\n", _lCtype[i]);}
		
		//œ¬∑¢∂‡∏ˆ∂‘œÛ
		char _caId[16];
		long _lId[200];
		printf("caId = %s\n",caId);	
		j=0;
		k=0;
		for(i=0;i<strlen(caId);i++){
		if(caId[i] != 44){
		_caId[j] = caId[i];
		j++;
		}
		else{
		_caId[j]="\0";
		if(_lCtype[k] == 2){
		lId = strtoul(_caId,NULL,10);
		_lId[k] = lId;
		}else if(_lCtype[k] == 1){
	//	printf("_caId = %s\n",_caId);
		lId =atol(_caId);
	//	printf("lId = %lu \n",lId);
		_lId[k] = lId;
		}else if(_lCtype[k] == 0)
		{
		_lId[k] = 0;
		}
	//	_caId = NULL;
		k++;
		j=0;
		}
		}
		for(i=0; i<k; i++){
		printf("_lId = %lu\n", _lId[i]);}
		//÷˜≤ﬂ¬‘
		char _caPid[16];
		long _lPid[200];
		int p=0;
		j=0;
		for(i=0; i<strlen(caPid); i++){
		if(caPid[i] != 44){
		_caPid[j]=caPid[i];
		j++;
		}else{
		_caPid[j]="\0";
		lPid=atol(_caPid);
		_lPid[p] = lPid;
		p++;
		j=0;
		}
		}
		
		for(i=0; i<p; i++)
		printf("_lPid = %lu\n", _lPid[i]);
		
		printf("sort1 = %s \n",sort);
		if(strcmp(sort,"ctype_desc")==0){
			strcpy(sort ,"ctype");
			printf("sort2 = %s \n",sort);
		}
		if(strcmp(sort,"id_desc")==0){
			strcpy(sort ,"id1");
			printf("sort2 = %s \n",sort);
		}
		
	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	//  char * caPname_gbk = convert("UTF-8", "GBK", caPname);
	  lStime=atol(_caStime);
	  psDbHead = utPltInitDb(); 
	  
	//	printf("caId = %s\n",caId);
	//	lId=strtoul(caId,NULL,10);
	//	printf("lId = %lu\n",lId);
	  
	  lEtime=atol(_caEtime);
	  lDataid=atol(caDataid);
	  unsigned long lSid1;
	  if(strcmp(caUpdate,"update")==0){
		 
		for(j=0;j<p;j++){
			pasDbExecSqlF("update nwprocyobj set flag = 8, modtime=%d where pid = %lu",time(0),_lPid[j]);
		//	pasDbExecSqlF("delete from nwprocyobj where pid = %lu",_lPid[j]);
			writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒœ¬∑¢≤ﬂ¬‘");
		for(i=0;i<k;i++){
			sprintf(caTemp,"select sid from nwprocyobj where pid = %lu and ctype = %lu and id = %lu",_lPid[j],_lCtype[i],_lId[i]);
	  		lSid1=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid1);
	  		if(lSid1>0){
				printf("update\n");
	  			pasDbExecSqlF("update nwprocyobj set flag = 0,modtime=%d, dataid=%lu where sid=%lu  ",time(0),lDataid,lSid1);
					writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒœ¬∑¢≤ﬂ¬‘");
	  		}
	  		else{
	  			printf("insert\n");
	  			pasDbExecSqlF("insert into nwprocyobj(pid,stime,ctype,id,etime,dataid,createtime,modtime,createuser,flag) \
	       		values(%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,'%s',0)",\
	       		_lPid[j],lStime,_lCtype[i],_lId[i],lEtime,lDataid,time(0),time(0),"");
					writeSysLog(psMsgHead,"04","Success","‘ˆº”œ¬∑¢≤ﬂ¬‘");
				}
	  		
				}
			}
		}
		  if(strcmp(caUpdate,"add")==0){
			printf("add new \n");
		  	for(i=0;i<k;i++){
			sprintf(caTemp,"select sid from nwprocyobj where pid = %lu and ctype = %lu and id = %lu",_lPid[j],_lCtype[i],_lId[i]);
	  		lSid1=0;
	  		pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lSid1);
	  		if(lSid1>0){
				printf("update\n");
	  			pasDbExecSqlF("update nwprocyobj set flag = 0,modtime=%d, dataid=%lu where sid=%lu  ",time(0),lDataid,lSid1);
					writeSysLog(psMsgHead,"04","Success","–ﬁ∏ƒœ¬∑¢≤ﬂ¬‘");
	  		}
	  		else{
	  			printf("insert\n");
	  			pasDbExecSqlF("insert into nwprocyobj(pid,stime,ctype,id,etime,dataid,createtime,modtime,createuser,flag) \
	       		values(%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,'%s',0)",\
	       		_lPid[j],lStime,_lCtype[i],_lId[i],lEtime,lDataid,time(0),time(0),"");
					writeSysLog(psMsgHead,"04","Success","‘ˆº”œ¬∑¢≤ﬂ¬‘");
				}
	  		
				}
		}
	  
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  //	pasDbExecSqlF("delete from nwprocyobj  where pid in(%s)",caSelsid);
	  printf("del\n");
	  printf("caSelsid=%s\n",caSelsid);
	   printf("modtime=%d\n",time(0));
		pasDbExecSqlF("update nwprocyobj set flag = 8, modtime=%d where pid in(%s)",time(0),caSelsid);
		
		writeSysLog(psMsgHead,"04","Success","…æ≥˝œ¬∑¢≤ﬂ¬‘");
	  }
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid");
   
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	printf("lCount = %d\n",lCount);  
	  
	int pids[1024];
	int numbers = 0;
	char objs[2048];
	char objs1[2048];
	char compnames[64];
	char gnames[64];
	char gnames1[64];
	int ctype=0;
	unsigned long id=0;
	char all[12] = "»´≤ø";
	 sprintf(caTemp,"select count(*) from (select pid from nwprocyobj where flag = 0 group by pid)a");
	 iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&numbers);
	printf("numbers = %d\n",numbers);  	
	
	sprintf(caTemp,"select pid from nwprocyobj where flag = 0 group by pid");
	printf("caTemp=%s\n",caTemp);
    psCur = pasDbOpenSql(caTemp,0);	
	
   //int i =0;
   	                              
   	iReturn=0;
   	iNum=0;
	
	printf("111111111111\n");
   for(i = 0; i<numbers; i++ ){
   			 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4, &pids[i]);
			printf("pids[%d] = %d \n", i, pids[i]);
			}	  
		//pasDbCloseCursor(psCur);
	  	printf("2222222222222\n");
		 pasDbCloseCursor(psCur);
		
		
	iReturn=0;
   	iNum=0;	
	int count =0;
	 for(i = 0; i<numbers; i++ ){	
	 
	 
	// select compname, gname,  ctype, id, nwprocyobj.pid,  compid, groupid, gid from nwprocyobj left join nwcompute on (compid = id) left join nwgroup on (gid = id) where nwprocyobj.pid = %d order by ctype
	 sprintf(caTemp,"select compname, gname, gname1, ctype, id, nwprocyobj.pid,nwgroup.pid as grouppid from nwprocyobj left join (select compname, gname as gname1, compid, groupid from nwcompute left join nwgroup on (gid = groupid))a on (compid = id) left join nwgroup on (gid = id) where nwprocyobj.pid = %d and nwprocyobj.flag = 0 order by ctype",pids[i]);
		printf("caTemp=%s\n",caTemp);
//		printf("33333333333\n");
			
		psCur = pasDbOpenSql(caTemp,0);	
//		printf("4444444444444\n");
	   	iReturn=0;
		if(psCur == NULL) {
		printf("aaaaaaaaaa\n");
        return 0;
		}
		memset(objs,0,2048);
		memset(objs1,0,2048);
		iNum++;
		count = 0;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
	   printf("bbbbbbbbbbbbbbb\n");
			while((iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_STRING,60 , compnames,
									   UT_TYPE_STRING,60 , gnames,
									   UT_TYPE_STRING,60, gnames1,
									   UT_TYPE_LONG,4 , &ctype,
									   UT_TYPE_ULONG,8, &id))==0 || iReturn ==1405){
			
		
		
	//	printf("compnames[%d] = %s\n",i,compnames);
	//	printf("gnames[%d] = %s\n",i,gnames);
	//	printf("gnames1[%d] = %s\n",i,gnames1);
					   
		if(count ==0){
				if(ctype==0){
				snprintf(objs+strlen(objs),sizeof(objs)-strlen(objs),"%s",all);
				//snprintf(objs1+strlen(objs1),sizeof(objs1)-strlen(objs1),"%s",all);
				}else{
				snprintf(objs+strlen(objs),sizeof(objs)-strlen(objs),"%s%s",compnames,gnames);
				//snprintf(objs1+strlen(objs1),sizeof(objs1)-strlen(objs1),"%s%s%s",compnames,gnames,gnames1);
				}
				snprintf(objs1+strlen(objs1),sizeof(objs1)-strlen(objs1),"%lu",id);
		}
		else{		
			if(ctype==0){
				snprintf(objs+strlen(objs),sizeof(objs)-strlen(objs),",%s",all);
				//snprintf(objs1+strlen(objs1),sizeof(objs1)-strlen(objs1),",%s",all);
				}else{
				snprintf(objs+strlen(objs),sizeof(objs)-strlen(objs),",%s%s",compnames,gnames);
				//snprintf(objs1+strlen(objs1),sizeof(objs1)-strlen(objs1),",%s%s%s",compnames,gnames,gnames1);
				}
				snprintf(objs1+strlen(objs1),sizeof(objs1)-strlen(objs1),",%lu",id);
		}
		count++;
		memset(compnames,0,64);
		memset(gnames,0,64);
		memset(gnames1,0,64);
		ctype = 0;
		id =0;
		}
		printf("objs = %s\n",objs);
		utPltPutLoopVarF(psDbHead,"compname",iNum,"%s",objs);
		utPltPutLoopVarF(psDbHead,"compname1",iNum,"%s",objs1);
		pasDbCloseCursor(psCur);
		}
			
			
	//  	printf("33333333333\n");
	    sprintf(caTemp,"select nwprocyobj.sid as sid1,nwprocyobj.pid,ctype,nwprocyobj.id as id1,nwprocyobj.stime,nwprocyobj.etime,nwprocyobj.dataid,nwprocyobj.modtime,nwprocyobj.createuser,name,lawname,gname,nwdatedes.id,nwmainprocy.sid, compid,gid from nwprocyobj  left join nwdatedes on (dataid = nwdatedes.id) left join nwmainprocy on (nwmainprocy.sid = nwprocyobj.pid) left join nwcompute on (compid = nwprocyobj.id) left join nwgroup on(gid = nwprocyobj.id) where nwprocyobj.flag = 0 group by nwprocyobj.pid");
    	
    
    sprintf(caTemp+strlen(caTemp)," order by %s %s limit %d,%d",sort,dir,lStartRec,lRowNum);
    	
  	printf("caTemp=%s\n",caTemp);

     psCur = pasDbOpenSql(caTemp,0);
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561 ˝æ›Õ≥º∆≥ˆ¥Ì"),ncsLang("0171≤È—Ø ˝æ›≥ˆ¥Ì"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;

   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   		
     lSid=0;
     lPid=0;
	 lStime = 0;
   //  memset(caPname_gbk,0,sizeof(caPname_gbk));
     lCtype=0;
	 lId=0;
     lEtime=0;
     lDataid=0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   	 
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                             
      	                               UT_TYPE_ULONG,4,&lCtype,
									   UT_TYPE_LONG,4,&lId,
									   UT_TYPE_LONG,4,&lStime,
      	                               UT_TYPE_LONG,4,&lEtime,
      	                               UT_TYPE_LONG,4,&lDataid,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser,
									   UT_TYPE_STRING,31,dateName,
									    UT_TYPE_STRING,23,lawName,
								//		UT_TYPE_STRING,31,compName,
										UT_TYPE_STRING,31,gName);
      	                             
      	                              
      	                                                             
      	                              
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"ctype",iNum,"%lu",lCtype);
		utPltPutLoopVarF(psDbHead,"id",iNum,"%lu",lId);
        utPltPutLoopVarF(psDbHead,"etime",iNum,"%lu",lEtime);
        utPltPutLoopVarF(psDbHead,"dataid",iNum,"%lu",lDataid);
        utPltPutLoopVarF(psDbHead,"stime",iNum,"%lu",lStime);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
		if(lDataid == 0){
		utPltPutLoopVar(psDbHead,"datename",iNum,"Œﬁ");
		}
		else{
		 utPltPutLoopVarF(psDbHead,"datename",iNum,"%s",dateName);
		 }
		 utPltPutLoopVarF(psDbHead,"lawname",iNum,"%s",lawName);
		//  utPltPutLoopVarF(psDbHead,"compname",iNum,"%s",compName);
		  utPltPutLoopVarF(psDbHead,"gname",iNum,"%s",gName);
		  if(lCtype == 1){
	//	    utPltPutLoopVarF(psDbHead,"id_desc",iNum,"%s",gName);
		  }
		  else if(lCtype == 2){
	//		utPltPutLoopVarF(psDbHead,"id_desc",iNum,"%s",compName);
		  }else	if(lCtype==0){
        	utPltPutLoopVar(psDbHead,"id_desc",iNum,"»´≤ø");
        }
     //   memset(caTemp,0,sizeof(caTemp));
        lIndex=0;

		long hour,min,sec;
		hour = lStime/10000;
		min = (lStime%10000)/100;
		sec = (lStime%10000)%100;
	//	printf("hour = %u, min = %u, sec = %u \n",hour,min,sec);
	
		if(hour < 10){
		sprintf(caTemp,"0%u",hour);
		}
		else{
		sprintf(caTemp,"%u",hour);
		}
		if(min < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",min);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",min);
		}	
		if(sec < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",sec);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",sec);
		}
	//	printf("caTemp=%s\n",caTemp);
		utPltPutLoopVarF(psDbHead,"stime_desc",iNum,"%s",caTemp);
	
		//long hour,min,sec;
		hour = lEtime/10000;
		min = (lEtime%10000)/100;
		sec = (lEtime%10000)%100;
	//	printf("hour = %u, min = %u, sec = %u \n",hour,min,sec);
		
		if(hour < 10){
		sprintf(caTemp,"0%u",hour);
		}
		else{
		sprintf(caTemp,"%u",hour);
		}
		if(min < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",min);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",min);
		}	
		if(sec < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",sec);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",sec);
		}
	//	printf("caTemp=%s\n",caTemp);
		utPltPutLoopVarF(psDbHead,"etime_desc",iNum,"%s",caTemp);	
		
        if(lCtype == 2){
			  utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"º∆À„ª˙");
		}
		else if(lCtype == 0){
		 utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"»´≤ø");
		}
		else if(lCtype == 1){
		 utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"≤ø√≈");
		}
     //   utPltPutLoopVar(psDbHead,"optype_desc",iNum,caTemp);
	
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
    pasDbCloseCursor(psCur);
	  
	// utPltShowDb(psDbHead);
	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	    	printf("33333333333\n");
				printf("lCount = %d\n",lCount);
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_obj_list.htm");
	  return 0;
}


/*
//Á≠ñÁï•‰∏ãÂèë
int lan_Procy_TimeObj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	 pasDbCursor *psCur;
	  utPltDbHead *psDbHead;
	  long iReturn,lCount;
	  char caStart[16],caTemp[2048],caCurPg[16];
	  long lCurPg,lRowNum,lStartRec;
	  char caTemp_d[128],caStatus[16],sort[16],dir[16];
	  long iNum;
	  unsigned long lCtype,lEtime,lDataid,lSid,lPid,lModtime,lIndex,lStime,lId;
	 char caPid[1024],caLimit[16],caPage[16],caDel[16],caDir[16],caKeyword[64],caSelsid[1024],caUpdate[16],caNtype[16],caNetid[64],caSid[1024],dateName[32],caId[16],caGId[16],lawName[24],Ids[1024],Ids1[1024],Ids2[1024], compName[32],gName[32];
	 char caCtype[1024],caEtime[16],caDataid[16],caStime[16],caCreateuser[64];
	  utMsgPrintMsg(psMsgHead);
	  iReturn = utMsgGetSomeNVar(psMsgHead,18,
						 "pid",     UT_TYPE_STRING, 1023, caPid,
						 "sort",    UT_TYPE_STRING,  sizeof(sort)-1,        sort,
						"dir",      UT_TYPE_STRING,  sizeof(dir)-1,         dir,
			             "limit",   UT_TYPE_STRING, 10, caLimit,
			             "page",    UT_TYPE_STRING, 10, caPage,  
			             "dir",    	UT_TYPE_STRING,8,caDir,
			             "keyword", UT_TYPE_STRING,30,caKeyword,
			             "selsid",	UT_TYPE_STRING,1023,caSelsid,
			             "stime",	UT_TYPE_STRING,15,caStime,
			             "ctype",	UT_TYPE_STRING,1023,caCtype,
						 "id",		UT_TYPE_STRING,15,caId,
						 "gid",		UT_TYPE_STRING,15,caGId,
			             "etime",	UT_TYPE_STRING,15,caEtime,
			             "dataid",	UT_TYPE_STRING,15,caDataid,
			             "update",	UT_TYPE_STRING,10,caUpdate,
			             "sid",		UT_TYPE_STRING,1023,caSid,
			             "start",	UT_TYPE_STRING,10,caStart,
			             "del",		UT_TYPE_STRING,10,caDel);  
	
	  if(strlen(caKeyword)>0){
	  	strcpy(caTemp_d,pasCvtGBK(2,caKeyword,caTemp,63));
	  	strcpy(caKeyword,caTemp_d);
	  }          
	//	printf("%s\n",caStime);	              
		char _caStime[16];
		char _caEtime[16];
		int i=0;
		int j=0;
		for(i=0;i<10;i++){	
		if(caStime[i] != 58){
		_caStime[j] = caStime[i];
		j++;
		}
		}
		printf("_caStime = %s\n",_caStime);
		j=0;
		for(i=0;i<10;i++){	
		if(caEtime[i] != 58){
		_caEtime[j] = caEtime[i];
		j++;
		}
		}

		
		printf("caId = %s \n",caId);
	  lRowNum=atol(caLimit);
	  if(lCurPg<=0) lCurPg=1;
	  lStartRec = atol(caStart);
	//  char * caPname_gbk = convert("UTF-8", "GBK", caPname);
	  lStime=atol(_caStime);
	  psDbHead = utPltInitDb(); 
	  unsigned long numbers = 0;
	  if(strlen(caGId)>0){ //»Áπ˚¥´¿¥µƒ «≤ø√≈ID£¨ ≤È’“∏√≤ø√≈œ¬µƒÀ˘”–º∆À„ª˙ID
		strcpy(Ids,caGId);
		
		  printf("GroupId\n");
			
		int mark1 = 1; //—≠ª∑±È¿˙≤ø√≈ ˝æ›ø‚£¨÷±µΩ»´≤øŒ™÷π
		printf("Ids1 = %s \n",Ids1);
		printf("Ids2 = %s \n",Ids2);
	do{
			strcpy(Ids2,Ids1);
			memset(Ids1,0,sizeof(Ids1));
		//	strcpy(Ids1,Ids);
			numbers = 0;
		sprintf(caTemp,"select gid from nwgroup where pid in (%s) group by gid", Ids);
		psCur = pasDbOpenSql(caTemp,0);	
		printf("1111111111111111\n");
	   	iReturn=0;
		if(psCur == NULL) {
		printf("aaaaaaaaaa\n");
        return 0;
		}
	
			while((iReturn = pasDbFetchInto(psCur,
									   UT_TYPE_ULONG,8, &numbers))==0 || iReturn ==1405){
			
	 printf("numbers = %lu\n",numbers);
	 
	 if(strlen(Ids) == 0){
	 sprintf(Ids+strlen(Ids),"%lu",numbers);
	  }
	  else{
	   sprintf(Ids+strlen(Ids),",%lu",numbers);
	  }
	  
	  if(strlen(Ids1) == 0){
		sprintf(Ids1+strlen(Ids1),"%lu",numbers);
	  }
	  else{
	   sprintf(Ids1+strlen(Ids1),",%lu",numbers);
	  }
	 	 printf("Ids = %s\n",Ids);
	
	}
	
	  pasDbCloseCursor(psCur);
		
	}while(strcmp(Ids1,Ids2));
	
	memset(Ids1,0,sizeof(Ids1));
	memset(Ids2,0,sizeof(Ids2));
	printf("222222222222222\n");
	do{
			strcpy(Ids2,Ids1);
			memset(Ids1,0,sizeof(Ids1));
		//	strcpy(Ids1,Ids);
			numbers = 0;
		sprintf(caTemp,"select pid from nwgroup where gid in (%s) group by pid", Ids);
		psCur = pasDbOpenSql(caTemp,0);	
		printf("3333333333\n");
	   	iReturn=0;
		if(psCur == NULL) {
		printf("aaaaaaaaaa\n");
        return 0;
		}
	 //  printf("bbbbbbbbbbbbbbb\n");
			while((iReturn = pasDbFetchInto(psCur,
									   UT_TYPE_ULONG,8, &numbers))==0 || iReturn ==1405){
			
	 printf("numbers = %lu\n",numbers);
	 
	 if(strlen(Ids) == 0){
	 sprintf(Ids+strlen(Ids),"%lu",numbers);
	  }
	  else{
	   sprintf(Ids+strlen(Ids),",%lu",numbers);
	  }
	  
	  if(strlen(Ids1) == 0){
		sprintf(Ids1+strlen(Ids1),"%lu",numbers);
	  }
	  else{
	   sprintf(Ids1+strlen(Ids1),",%lu",numbers);
	  }
	 	 printf("Ids = %s\n",Ids);
	
	}
	
	  pasDbCloseCursor(psCur);
		
	}while(strcmp(Ids1,Ids2));
	
	
	
		numbers = 0;
		sprintf(caTemp,"select compid from nwcompute where groupid in (%s)",Ids);
		psCur = pasDbOpenSql(caTemp,0);	
//		printf("4444444444444\n");
	   	iReturn=0;
		if(psCur == NULL) {
		printf("aaaaaaaaaa\n");
        return 0;
		}
	   printf("bbbbb11111\n");
			while((iReturn = pasDbFetchInto(psCur,
									   UT_TYPE_ULONG,8, &numbers))==0 || iReturn ==1405){
			
	 printf("numbers = %lu\n",numbers);
	 if(strlen(Ids) == 0){
	 sprintf(Ids+strlen(Ids),"%lu",numbers);
	
	  }
	  else{
	   sprintf(Ids+strlen(Ids),",%lu",numbers);
	  }
	 	 printf("Ids = %s\n",Ids);

	}
	  pasDbCloseCursor(psCur);
	  }
	  
		numbers = 0;
	    if(strlen(caId)>0){ //»Áπ˚¥´¿¥µƒ «º∆À„ª˙ID£¨≤È’“º∆À„ª˙À˘ Ù≤ø√≈
		strcpy(Ids,caId);
		int mark = 1;
		sprintf(caTemp,"select groupid from nwcompute where compid = %s",caId);
		psCur = pasDbOpenSql(caTemp,0);	
//		printf("4444444444444\n");
	   	iReturn=0;
		if(psCur == NULL) {
		printf("aaaaaaaaaa\n");
        return 0;
		}
	   printf("bbbbb22222\n");
			while((iReturn = pasDbFetchInto(psCur,
									   UT_TYPE_ULONG,8, &numbers))==0 || iReturn ==1405){
			
	 printf("numbers = %lu\n",numbers);
	  if(numbers == 0 ){
		mark = 0;
	 }
	 if(strlen(Ids) == 0){
	 sprintf(Ids+strlen(Ids),"%lu",numbers);
	
	  }
	  else{
	   sprintf(Ids+strlen(Ids),",%lu",numbers);
	  }
	 	 printf("Ids = %s\n",Ids);

	}
	numbers = 0;
	pasDbCloseCursor(psCur);
	 //—≠ª∑±È¿˙≤ø√≈ ˝æ›ø‚£¨÷±µΩ»´≤øŒ™÷π
	while(mark == 1){
	sprintf(caTemp,"select pid from nwgroup where gid in (%s) group by pid",Ids);
	psCur = pasDbOpenSql(caTemp,0);	
//		printf("4444444444444\n");
	   	iReturn=0;
		if(psCur == NULL) {
		printf("aaaaaaaaaa\n");
		mark = 0;
        return 0;
		}
	   printf("bbbbb33333\n");
			while((iReturn = pasDbFetchInto(psCur,
									   UT_TYPE_ULONG,8, &numbers))==0 || iReturn ==1405){
			
	 printf("numbers = %lu\n",numbers);
	 if(numbers == 0 ){
		mark = 0;
	 }
	 if(strlen(Ids) == 0){
	 sprintf(Ids+strlen(Ids),"%lu",numbers);
	
	  }
	  else{
	   sprintf(Ids+strlen(Ids),",%lu",numbers);
	  }
	 	 printf("Ids = %s\n",Ids);

	}
	  pasDbCloseCursor(psCur);
	 }
	 }
	  
	  
	  
	  lEtime=atol(_caEtime);
	  lDataid=atol(caDataid);
	  
	  if(strcmp(caDel,"del")==0&&strlen(caSelsid)>0){
	  	pasDbExecSqlF("delete from nwprocyobj  where sid in(%s)",caSelsid);
	  }
	  
	  sprintf(caTemp,"select count(*) from ncscasenetid");
   
    lCount=0;
    iReturn = pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
	  
	    sprintf(caTemp,"select nwprocyobj.sid as sid1,nwprocyobj.pid,ctype,nwprocyobj.id as id1,nwprocyobj.stime,nwprocyobj.etime,nwprocyobj.dataid,nwprocyobj.modtime,nwprocyobj.createuser,name,lawname,compname,gname,nwdatedes.id,nwmainprocy.sid, compid,gid from nwprocyobj  left join nwdatedes on (dataid = nwdatedes.id) left join nwmainprocy on (nwmainprocy.sid = nwprocyobj.pid) left join nwcompute on (compid = nwprocyobj.id) left join nwgroup on(gid = nwprocyobj.id)  where nwprocyobj.flag = 0");
    if(strcmp(caId,"0")){
		sprintf(Ids+strlen(Ids),",0");
		 sprintf(caTemp+strlen(caTemp)," and nwprocyobj.id in (%s) ",Ids);
	}	
    
    sprintf(caTemp+strlen(caTemp)," order by %s %s limit %d,%d",sort,dir,lStartRec,lRowNum);
  	printf("caTemp=%s\n",caTemp);
    	

        psCur = pasDbOpenSql(caTemp,0);
   
    if(psCur == NULL) {
   //     utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm",ncsLang("0561 ˝æ›Õ≥º∆≥ˆ¥Ì"),ncsLang("0171≤È—Ø ˝æ›≥ˆ¥Ì"));
        return 0;
    }
   
   	                              
   	iReturn=0;
   	iNum=0;
   	while((iReturn == 0 ||iReturn==1405 )&&iNum<lRowNum) {
   	
     lSid=0;
     lPid=0;
	 lStime = 0;
   //  memset(caPname_gbk,0,sizeof(caPname_gbk));
     lCtype=0;
	 lId=0;
     lEtime=0;
     lDataid=0;
     lModtime=0;
     memset(caCreateuser,0,sizeof(caCreateuser));
   	 
   	 iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&lSid,
      	                               UT_TYPE_LONG,4,&lPid,
      	                             
      	                               UT_TYPE_ULONG,4,&lCtype,
									   UT_TYPE_LONG,4,&lId,
									   UT_TYPE_LONG,4,&lStime,
      	                               UT_TYPE_LONG,4,&lEtime,
      	                               UT_TYPE_LONG,4,&lDataid,
      	                               UT_TYPE_LONG,4,&lModtime,
      	                               UT_TYPE_STRING,23,caCreateuser,
									   UT_TYPE_STRING,31,dateName,
									    UT_TYPE_STRING,23,lawName,
										UT_TYPE_STRING,31,compName,
										UT_TYPE_STRING,31,gName);
      	                             
      	                              
      	                                                             
      	                              
      	                              
   	if((iReturn!=0)&&(iReturn!=1405)) break;	
        iNum++;
       if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
        utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
        utPltPutLoopVarF(psDbHead,"ctype",iNum,"%lu",lCtype);
		utPltPutLoopVarF(psDbHead,"id",iNum,"%lu",lId);
        utPltPutLoopVarF(psDbHead,"etime",iNum,"%lu",lEtime);
        utPltPutLoopVarF(psDbHead,"dataid",iNum,"%lu",lDataid);
        utPltPutLoopVarF(psDbHead,"stime",iNum,"%lu",lStime);
        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
		if(lDataid == 0){
		utPltPutLoopVar(psDbHead,"datename",iNum,"Œﬁ");
		}
		else{
		 utPltPutLoopVarF(psDbHead,"datename",iNum,"%s",dateName);
		 }
		 utPltPutLoopVarF(psDbHead,"lawname",iNum,"%s",lawName);
		  utPltPutLoopVarF(psDbHead,"compname",iNum,"%s",compName);
		  utPltPutLoopVarF(psDbHead,"gname",iNum,"%s",gName);
		  if(lCtype == 1){
		    utPltPutLoopVarF(psDbHead,"id_desc",iNum,"%s",gName);
		  }
		  else if(lCtype == 2){
			utPltPutLoopVarF(psDbHead,"id_desc",iNum,"%s",compName);
		  }else	if(lCtype==0){
        	utPltPutLoopVar(psDbHead,"id_desc",iNum,"»´≤ø");
        }
     //   memset(caTemp,0,sizeof(caTemp));
        lIndex=0;

		long hour,min,sec;
		hour = lStime/10000;
		min = (lStime%10000)/100;
		sec = (lStime%10000)%100;
	//	printf("hour = %u, min = %u, sec = %u \n",hour,min,sec);
	
		if(hour < 10){
		sprintf(caTemp,"0%u",hour);
		}
		else{
		sprintf(caTemp,"%u",hour);
		}
		if(min < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",min);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",min);
		}	
		if(sec < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",sec);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",sec);
		}
	//	printf("caTemp=%s\n",caTemp);
		utPltPutLoopVarF(psDbHead,"stime_desc",iNum,"%s",caTemp);
	
		//long hour,min,sec;
		hour = lEtime/10000;
		min = (lEtime%10000)/100;
		sec = (lEtime%10000)%100;
	//	printf("hour = %u, min = %u, sec = %u \n",hour,min,sec);
		
		if(hour < 10){
		sprintf(caTemp,"0%u",hour);
		}
		else{
		sprintf(caTemp,"%u",hour);
		}
		if(min < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",min);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",min);
		}	
		if(sec < 10){
		sprintf(caTemp+strlen(caTemp),":0%u",sec);
		}
		else{
		sprintf(caTemp+strlen(caTemp),":%u",sec);
		}
	//	printf("caTemp=%s\n",caTemp);
		utPltPutLoopVarF(psDbHead,"etime_desc",iNum,"%s",caTemp);	
		
		
        if(lCtype == 2){
			  utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"º∆À„ª˙");
		}
		else if(lCtype == 0){
		 utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"»´≤ø");
		}
		else if(lCtype == 1){
		 utPltPutLoopVar(psDbHead,"ctype_desc",iNum,"≤ø√≈");
		}
     //   utPltPutLoopVar(psDbHead,"optype_desc",iNum,caTemp);
	
      

        utPltPutLoopVar(psDbHead,"createuser",iNum,caCreateuser);
        utPltPutLoopVar(psDbHead,"modtime",iNum,utTimFormat("%Y-%m-%d %H:%M:%S", lModtime));
			
    }
    pasDbCloseCursor(psCur);
	  
	// utPltShowDb(psDbHead);
	  utPltPutVarF(psDbHead,"TotRec","%d",lCount);
	  
	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_procy_obj_list.htm");
	  return 0;
}
*/

//≤ø√≈µÁƒ‘ ˜
int lanTree_depComp2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caLevel[16];
    char caPlate[128];
   	char caGroupname[72],caGroupid[16];
   	char caTemp[4024];
   	char caName[32];
   	char caWhere[256];

   	char caCurPg[16],caTotRec[16];
   	char caPid[16];
   	unsigned long lLevel,lPid;
   	int  iNum;
  	char caFilter[256];
  	char caNo[16];
   	unsigned long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
   	int iReturn,i;

   	char caValue[256];
   	char caVar1[32];
   	char caOpt[256];
   	char caGroups[1024];
   	char caFacs[1024];
   	unsigned long lId;
   	char *p,*pVar;
   	unsigned char *pHash;
   	char caCname[72];

    char caFiled[128];
    long lCount;
   
    char caCname1[72];
    char caSql[2048];
    char caKeyword[64],caKeyword1[64];
    char caState[16];
    char caSip[16],caMac[20],caCompname[36];
	char caCompnames[2048];
    nwCompInfo *psComp;
    nwCompIp   *psCompIp;
    long lLx;
#ifdef LDEBUG1 
    utMsgPrintMsg(psMsgHead);
#endif  	
    utMsgPrintMsg(psMsgHead);
   
    memset(caCname,0,sizeof(caCname));
   	iReturn = utMsgGetSomeNVar(psMsgHead,5,
					"node",        UT_TYPE_STRING,10,caPid,
					"keyword",       UT_TYPE_STRING,60,caKeyword1,
					"state",       UT_TYPE_STRING,10,caState,
					"plate",       UT_TYPE_STRING,100,caPlate,
					"compnames", 	UT_TYPE_STRING, 2040, caCompnames);
					
    memset(caKeyword,0,sizeof(caKeyword));
    if(strlen(caKeyword1)>0){
   // 	strcpy(caKeyword,pasCvtGBK(2,caKeyword1,caTemp,60));
    	char *tmp=convert("UTF-8", "GBK", caKeyword1);
    	if(tmp){
    		strcpy(caKeyword,tmp);
    		free(tmp);
    	}
    }
//    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
//    strcpy(caWhere,"\0");

/*
     memset(caOpt,0,sizeof(caOpt)); 
    iReturn = dsCltGetMyInfo(1,"Userid",&lId);
    if(iReturn==0){
     pasDbOneRecord("select opt from dsuser where id=:id",1,"id",UT_TYPE_LONG,lId,UT_TYPE_STRING,255,caOpt);
    } 

*/

//    lRowNum = utComGetVar_ld(psShmHead,"GroupTreeRow",300);
      printf("caCompnames = %s \n",caCompnames);
  
  
  	


	
	char objs[2048];
	int objs1[2048];
	unsigned long id=0;
	unsigned long grouppid=0;
	unsigned long groupid=0;
	unsigned long ctype=0;
	char all[12] = "»´≤ø";
	iReturn=0;
   	iNum=0;	
	int count =0;
	
	//»®œﬁ
	char *auth = getDsGroupid();
	char sql_tmp[1024] = "";
	if(!utStrIsSpaces(auth)){
		snprintf(sql_tmp, sizeof(sql_tmp)," and groupid in (%s)",auth);
	}
	
	if(strlen(caCompnames) >0){
	 sprintf(caTemp,"select nwgroup.pid as grouppid, id, groupid, ctype, nwprocyobj.pid from nwprocyobj left join nwcompute  on (compid = id) left join nwgroup on (gid = id) where nwprocyobj.pid = %lu %s order by ctype",atol(caCompnames),sql_tmp);
		printf("caTemp=%s\n",caTemp);
		
		psCur = pasDbOpenSql(caTemp,0);	
	
	   	iReturn=0;
		if(psCur == NULL) {
		
        return 0;
		}
		
	//   printf("bbbbbbbbbbbbbbb\n");
			while((iReturn = pasDbFetchInto(psCur,
      	             UT_TYPE_ULONG,8 , &grouppid,
									   UT_TYPE_ULONG,8, &id,
									   UT_TYPE_ULONG,8,&groupid,
									   UT_TYPE_ULONG,8,&ctype))==0 || iReturn ==1405){
			
		
				if(ctype == 0){
				objs1[count] = id;
				}
				else{
				objs1[count] = groupid;
				}
				
				count++;
		
		ctype=0;
		groupid=0;
		grouppid =0;
		id =0;
		}
		
		for(i =0; i < count; i++){
		printf("objs1[%d] = %lu\n",i,objs1[i]);
		}
	//	utPltPutLoopVarF(psDbHead,"compname",iNum,"%s",objs);
		pasDbCloseCursor(psCur);
	}
			
	
	char * compnames_UTF = convert("UTF-8", "GBK", caCompnames);
	printf("caCompnames_UTF = %s \n", compnames_UTF);
	
    iNum = 0;
    
     psDbHead = utPltInitDb();
    if(strlen(caKeyword)==0){
//	    lStartRec = (lCurPg - 1) * 300;  
		     sprintf(caTemp,"select gname,gid,pid from nwgroup where 1=1 ");		
		    sprintf(caTemp+strlen(caTemp)," and pid=%d ",atol(caPid));
		    sprintf(caTemp+strlen(caTemp)," order by gname limit %lu,%lu ",0,300);
		    printf("caTemp=%s\n",caTemp);
		    psCur=pasDbOpenSql(caTemp,0);
		  
		    if(psCur == NULL) {
		    	  utWebDispMsg(iFd,psMsgHead,"ncs/ncmsg_back.htm","≤ø√≈≤È—Ø"," ˝æ›ø‚≥ˆ¥Ì");
		//    	  free(pHash);
		        return 0;
		    }
		   
		    
		     
		
		    memset(caGroupname,0,sizeof(caGroupname));
		    lGroupid=0;
		    lPid=0;
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
		                                   UT_TYPE_LONG,4,&lGroupid,
		                                   UT_TYPE_ULONG,4,&lPid);
		
		 while(iReturn == 0||iReturn==1405) {


		         if(iNum>0){
		         	utPltPutLoopVar(psDbHead,"dh",iNum+1,",");
		         }
		         iNum++;
		         utPltSetCvtHtml(1);  
		         utPltPutLoopVar(psDbHead,"groupname", iNum,utStrGetCharHan(caGroupname,48));
		         utPltPutLoopVar(psDbHead,"ctext",iNum,utStrGetCharHan(caGroupname,48));
		         utPltSetCvtHtml(0);  
		         utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%lu",lGroupid);
		 //        psData = (struct stData *)pasLHashLook(pHash,&lGroupid);       
		 //        	if(psData){
		  //       	printf("id=%d,lcount=%d\n",psData->id,psData->count);
		          	utPltPutLoopVar(psDbHead,"leaf", iNum,"false");
		         		utPltPutLoopVar(psDbHead,"cls", iNum,"folder");
		         		utPltPutLoopVar(psDbHead,"dtype",iNum,"2");
		         		
		 //        	}
		  //        else{
		  //        	utPltPutLoopVar(psDbHead,"leaf", iNum,"true");
		  //       		utPltPutLoopVar(psDbHead,"cls", iNum,"file");
		          	
		
		 //         }
		        
		
		        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum-1);
		
		        utPltPutLoopVarF(psDbHead,"pid",iNum,"%lu",lPid);
						if(count == 0){
						utPltPutLoopVar(psDbHead,"expanded",iNum,"true");
						}
						else{
						for(i=0; i < count; i++){
						printf("objs1[%d] = %lu, lGroupid = %lu\n",i,objs1[i],lGroupid);
						if(objs1[i] == lGroupid){
						printf("111111111111111\n");
						utPltPutLoopVar(psDbHead,"expanded",iNum,"true");
						i--;
						break;
						}
						else{
						printf("222222222222222\n");
						}
						if(i+1 == count){
						utPltPutLoopVar(psDbHead,"expanded",iNum,"true");
						printf("333333333333\n");
						}
						}	
						}
							
				    memset(caGroupname,0,sizeof(caGroupname));
				    lGroupid=0;
				    lLevel=0;
				    lPid=0;
				    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caGroupname,
				                                   UT_TYPE_ULONG,4,&lGroupid,
				                                   UT_TYPE_ULONG,4,&lPid);
				  }
				  pasDbCloseCursor(psCur);
		    }
		    
		//     free(pHash);
		    utPltPutVar(psDbHead,"rootname",utComGetVar_sd(psShmHead,"rootname","»´≤ø"));
    	
    	
     
     
     
    	sprintf(caTemp,"select devname,compid,groupid,inet_ntoa(ip),mac,compname from nwcompute where 1=1 ");
  
    if(strlen(caKeyword)==0){
    	   
		    	if(strlen(caPid)==0){
		    		  sprintf(caTemp+strlen(caTemp)," and groupid not in (select gid from nwgroup ) ");
		    		  
		    	}
		    	else{
		    		  sprintf(caTemp+strlen(caTemp)," and groupid=%d ",atol(caPid));
		    		 		
		    	}
     }
     else{
     	   sprintf(caTemp+strlen(caTemp),"and (devname like '%c%s%c' or compname like '%c%s%c' or inet_ntoa(ip) like '%c%s%c' or mac like '%c%s%c' ) ",'%',caKeyword,'%','%',caKeyword,'%','%',caKeyword,'%','%',caKeyword,'%');
     	  
     	 
     	
     }
      sprintf(caTemp+strlen(caTemp)," order by devname limit %lu,%lu",0,300);
      printf("caTemp=%s\n",caTemp);
       psCur = pasDbOpenSql(caTemp,0);
    	 if(psCur!=NULL){
    	    memset(caGroupname,0,sizeof(caGroupname));
    	    memset(caSip,0,sizeof(caSip));
			    lGroupid=0;
			    lPid=0;
			    memset(caMac,0,sizeof(caMac));
			    memset(caCompname,0,sizeof(caCompname));
			    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,68,caGroupname,
			                                   UT_TYPE_ULONG,4,&lGroupid,
			                                   UT_TYPE_ULONG,4,&lPid,
			                                   UT_TYPE_STRING,15,caSip,
			                                   UT_TYPE_STRING,18,caMac,
			                                   UT_TYPE_STRING,31,caCompname);
			
			    while(iReturn == 0) {
			         
			        if(strcasecmp(utComGetVar_sd(psShmHead,"dispgname","compname"),"ip")==0){
			        	strcpy(caFiled,caSip);
			        }
			        else if(strcasecmp(utComGetVar_sd(psShmHead,"dispgname","compname"),"mac")==0){
			        	strcpy(caFiled,caMac);
			        }
			        else if(strcasecmp(utComGetVar_sd(psShmHead,"dispgname","compname"),"devname")==0){
			        	strcpy(caFiled,caGroupname);
			        }
			        else if(strcasecmp(utComGetVar_sd(psShmHead,"dispgname","compname"),"compname")==0){
			        	strcpy(caFiled,caCompname);
			        }
			        else{
			        	strcpy(caFiled,caCompname);
			        }
			         
			         
			      lLx=2;  
		        psComp =  (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&lGroupid);
	          if(psComp) {
	          	psCompIp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&lGroupid);
	          	if(psCompIp){
	          	   lLx=1;
	            }
	          }
	//	printf("lLx=%d\n",lLx);	        
			        if(lLx==atol(caState)||strlen(caState)==0){
			        
			         if(iNum>0){
			         	utPltPutLoopVar(psDbHead,"dh",iNum+1,",");
			         }
			         iNum++;
			        
			        if(lLx==2){
			        	 sprintf(caTemp,"<font color=#bbbbbb>%s(%s)</font>",utStrGetCharHan(caFiled,48),"¿Îœﬂ");
			        	 utPltPutLoopVar(psDbHead,"groupname", iNum,caTemp);
			        }
			        else{ 
			          utPltPutLoopVar(psDbHead,"groupname", iNum,utStrGetCharHan(caFiled,48));
			        }
			          utPltPutLoopVar(psDbHead,"ctext",iNum,utStrGetCharHan(caFiled,48));
		//	         utPltSetCvtHtml(0);  
			         utPltPutLoopVarF(psDbHead,"did",   iNum,"%lu",lGroupid);

			          	utPltPutLoopVar(psDbHead,"leaf", iNum,"true");
			         	utPltPutLoopVar(psDbHead,"cls", iNum,"file");
			          	
			            utPltPutLoopVar(psDbHead,"dtype",iNum,"0");
			            utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
			            sprintf(caTemp,"º∆À„ª˙√˚:%s,ipµÿ÷∑£∫%s,macµÿ÷∑:%s",caCompname,caSip,caMac);
			            utPltPutLoopVar(psDbHead,"qtip",iNum,caTemp);
			
			            utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum-1);
			            utPltPutLoopVar(psDbHead,"treecls",iNum,"treecompt");
			            utPltPutLoopVarF(psDbHead,"pid",iNum,"%d",lPid);
						
						if(count == 0){
						utPltPutLoopVar(psDbHead,"expanded",iNum,"false");
			
						}else{
						for(i =0; i < count; i ++){
						printf("objs1[%d] = %lu, lGroupid = %lu\n",i,objs1[i],lGroupid);
						if(objs1[i] == lGroupid){
						printf("111111111111111\n");
						utPltPutLoopVar(psDbHead,"expanded",iNum,"true");
						break;
						}
						else{
						printf("222222222222222\n");
						utPltPutLoopVar(psDbHead,"expanded",iNum,"false");
						}
						}
						}
						
			      }
			        memset(caSip,0,sizeof(caSip));
					    memset(caGroupname,0,sizeof(caGroupname));
					    lGroupid=0;
					    lLevel=0;
					    lPid=0;
			    memset(caMac,0,sizeof(caMac));
			    memset(caCompname,0,sizeof(caCompname));
			    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,68,caGroupname,
			                                   UT_TYPE_ULONG,4,&lGroupid,
			                                   UT_TYPE_ULONG,4,&lPid,
			                                   UT_TYPE_STRING,15,caSip,
			                                   UT_TYPE_STRING,18,caMac,
			                                   UT_TYPE_STRING,31,caCompname);
			    
			  }
			    pasDbCloseCursor(psCur);
    	  }
    	
    //	printf("666666666666666\n");
// utPltShowDb(psDbHead);
        if(strlen(caPlate)>0){
        	utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
        }
        else{
    	      utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/ncs_depcompute_tree2.htm");
    	  } 
    return 0;
}




int lan_ssjk_getScreen2(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead){
	utMsgPrintMsg(psMsgHead);
	
	char compids_in[128] = "";
	utMsgGetSomeNVar(psMsgHead,1,"compids",UT_TYPE_STRING,sizeof(compids_in)-1,compids_in);
	
	unsigned long compid = 0;
	char *compid_t = compids_in;
	char *compid_tmp = compids_in;
	nwCompIp *psCompIp_tmp = NULL;
	nwCompInfo *comp_info = NULL;
	utPltDbHead *psDbHead = utPltInitDb();
	int iNum = 0;
	char file[256] = "";
	unsigned int pic_time;
	unsigned int time_def;
	time_t now;
	time(&now);
	
	utPltPutVarF(psDbHead,"TotRec","%u",9);
	while(*compid_tmp != '\0'){
		if(*compid_tmp == ','){
			
			*compid_tmp = 0;
			compid = strtoul(compid_t,NULL,10);
			psCompIp_tmp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
		  iNum++;  
			if(psCompIp_tmp){
				psCompIp_tmp->lCtrl=LAN_CLIENT_SCREEN;
			}
		
			comp_info = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
			if(comp_info){
				if(iNum > 1){
					utPltPutLoopVar(psDbHead,"dh",iNum,",");
				}
				utPltPutLoopVar(psDbHead,"compname",iNum,comp_info->compname);
				
				utPltPutLoopVar(psDbHead,"sip",iNum,"%u",comp_info->ip);
				utPltPutLoopVar(psDbHead, "gname", iNum,lanGetGroup(psShmHead,comp_info->groupid));
				utPltPutLoopVarF(psDbHead, "compid", iNum, "%lu",compid);
				snprintf(file,sizeof(file),"/home/ncmysql/nw/online/screen/%lu.jpg",compid);
				pic_time = GetFileAttr(file);
				time_def = now - pic_time;;
				if(time_def < 60){
					utPltPutLoopVar(psDbHead, "file", iNum,file);
				}else{
					utPltPutLoopVar(psDbHead, "file", iNum, "/home/ncmysql/nw/online/screen/heiping.jpg");
				}
			}
			
			compid_t = compid_tmp+1;
		}
		compid_tmp++;		
	}
	iNum++;
	if(iNum > 1){
		utPltPutLoopVar(psDbHead,"dh",iNum,",");
	}
	compid = strtoul(compid_t,NULL,10);
	psCompIp_tmp = (nwCompIp *)utShmHashLook(psShmHead,NC_LNK_ONLINECOMP,&compid);
    
	if(psCompIp_tmp){
		psCompIp_tmp->lCtrl=LAN_CLIENT_SCREEN;
	}

	comp_info = (nwCompInfo *)utShmHashLook(psShmHead,NC_LNK_COMPUTE,&compid);
	if(comp_info){
		utPltPutLoopVar(psDbHead,"compname",iNum,comp_info->compname);
		utPltPutLoopVarF(psDbHead,"sip",iNum,"%u",comp_info->ip);
		utPltPutLoopVar(psDbHead, "gname", iNum,lanGetGroup(psShmHead,comp_info->groupid));
		utPltPutLoopVarF(psDbHead, "compid", iNum, "%lu",compid);
		snprintf(file,sizeof(file),"/home/ncmysql/nw/online/screen/%lu.jpg",compid);
				 
		pic_time = GetFileAttr(file);
		time_def = now - pic_time;;
		if(time_def < 60){
			utPltPutLoopVar(psDbHead, "file", iNum,file);
		}else{
			utPltPutLoopVar(psDbHead, "file", iNum, "/home/ncmysql/nw/online/screen/heiping.jpg");
		}
	}
	
	utPltShowDb(psDbHead);
	utPltOutToHtml(iFd,psMsgHead,psDbHead,"v4/neiwang/ssjk/lan_ssjk_sspm.htm");
	return 0;
}

//ÁªüËÆ°Á≠ñÁï•Ë¢´ÂÖ∂‰ªñÁ≠ñÁï•Áî®Ê¨°Êï∞
int lan_Procy_count_procyref(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	printf("lan_Procy_count_procyref start!!!!\n");
	 pasDbCursor *psCur;
	// utPltDbHead *psDbHead;
	 utPltDbHead *psDbHead = utPltInitDb();
	char caTemp[128];
	char sid[24];
	utMsgPrintMsg(psMsgHead);

    //pasDbConn *psConn;
	
		utMsgGetSomeNVar(psMsgHead, 1,
   				     "sid",    UT_TYPE_STRING,  sizeof(sid)-1,   sid
					 );
	printf("sid = %s\n",sid);				 
	sprintf(caTemp,"select count(*) from nwprocyref where othid in (%s)",sid);	
  	printf("caTemp=%s\n",caTemp);
	
    psCur = pasDbOpenSql(caTemp,0);
	if(psCur != NULL)
	{
	int iReturn=0;
   	int iNum=0;
	long id =0;
	char name[24];
	while(0==(iReturn = pasDbFetchInto(psCur,
      	                     UT_TYPE_LONG,4,&id))  || 1405==iReturn)
	 {
	//	printf("iReturn = %d\n",iReturn);									   
        iNum++;
	//	printf("iNum = %d\n",iNum);
      if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
	//	printf("2222222222222\n");
        utPltPutLoopVarF(psDbHead,"id",iNum,"%lu", id);
		//	printf("3333333333333333\n");
	//	utPltPutLoopVarF(psDbHead,"name",iNum,"%s",name);
	//		printf("4444444444\n");
		}
	//	printf("5555555555\n");
		pasDbCloseCursor(psCur);
	}
	
	//	printf("webclass  omf!!!!\n");
		utPltShowDb(psDbHead);
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_webclass_list.htm");
		printf("lan_Procy_count_procyref  end!!!!\n");
      
	  return 0;

}

//ÁªüËÆ°Á≠ñÁï•‰∏ãÂèëÊ¨°Êï∞
int lan_Procy_count_procyobj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
	printf("lan_Procy_count_procyobj start!!!!\n");
	 pasDbCursor *psCur;
	// utPltDbHead *psDbHead;
	 utPltDbHead *psDbHead = utPltInitDb();
	char caTemp[128];
	char sid[24];
	utMsgPrintMsg(psMsgHead);

    //pasDbConn *psConn;
	
		utMsgGetSomeNVar(psMsgHead, 1,
   				     "sid",    UT_TYPE_STRING,  sizeof(sid)-1,   sid
					 );
	printf("sid = %s\n",sid);				 
	sprintf(caTemp,"select count(*) from nwprocyobj where pid in (%s)",sid);	
  	printf("caTemp=%s\n",caTemp);
	
    psCur = pasDbOpenSql(caTemp,0);
	if(psCur != NULL)
	{
	int iReturn=0;
   	int iNum=0;
	long id =0;
	char name[24];
	while(0==(iReturn = pasDbFetchInto(psCur,
      	                     UT_TYPE_LONG,4,&id))  || 1405==iReturn)
	 {
	//	printf("iReturn = %d\n",iReturn);									   
        iNum++;
	//	printf("iNum = %d\n",iNum);
      if(iNum>1){
       	utPltPutLoopVar(psDbHead,"dh",iNum,",");
       }
	//	printf("2222222222222\n");
        utPltPutLoopVarF(psDbHead,"id",iNum,"%lu", id);
		//	printf("3333333333333333\n");
	//	utPltPutLoopVarF(psDbHead,"name",iNum,"%s",name);
	//		printf("4444444444\n");
		}
	//	printf("5555555555\n");
		pasDbCloseCursor(psCur);
	}
	
	//	printf("webclass  omf!!!!\n");
		utPltShowDb(psDbHead);
		utPltOutToHtml(iFd,psMsgHead,psDbHead,"lan/procy/ncs_webclass_list.htm");
		printf("lan_Procy_count_procyobj  end!!!!\n");
      
	  return 0;

}

int lanWeb_initFun003(utShmHead *psShmHead)
{
    int iReturn;
//    iReturn = pasSetTcpFunName("wbs_getDateTime",wbs_getDateTime, 0);
	iReturn = pasSetTcpFunName("lan_log_getFileLog",lan_log_getFileLog,0);//Êñá‰ª∂Êó•ÂøóÊü•Áúã
	iReturn = pasSetTcpFunName("lan_log_getScreenLog",lan_log_getScreenLog,0);//Â±èÂπïÊó•ÂøóÊü•Áúã
	iReturn = pasSetTcpFunName("lan_log_getAlarmLog",lan_log_getAlarmLog,0);//ÂëäË≠¶Êó•ÂøóÊü•Áúã
	iReturn = pasSetTcpFunName("nwWeb_ShowScreenJpg", nwWeb_ShowScreenJpg, 0);
//	iReturn = pasSetTcpFunName("lan_ssjk_getScreen",lan_ssjk_getScreen,0);//Â±èÂπïÂÆûÊó∂ÁõëÊéß
	iReturn = pasSetTcpFunName("lan_Procy_Device",lan_Procy_Device, 0);//ÊòæÁ§∫Êñá‰ª∂Á≠ñÁï•
	iReturn = pasSetTcpFunName("lan_Procy_Link",lan_Procy_Link, 0);//ÊòæÁ§∫ÈùûÊ≥ïÂ§ñËÅîÁ≠ñÁï•
	iReturn = pasSetTcpFunName("lan_Procy_Pgtype",lan_Procy_Pgtype, 0);//ÊòæÁ§∫Â∫îÁî®Á®ãÂ∫èÁ≠ñÁï•
	iReturn = pasSetTcpFunName("lan_Procy_Pname",lan_Procy_Pname, 0);//ÊòæÁ§∫ËøõÁ®ãÁ≠ñÁï•
	iReturn = pasSetTcpFunName("lan_Procy_Net",lan_Procy_Net, 0);//ÊòæÁ§∫ÁΩëÁªúÁ≠ñÁï•
	iReturn = pasSetTcpFunName("lan_Procy_Url",lan_Procy_Url, 0);//ÊòæÁ§∫ÁΩëÂùÄÁ≠ñÁï•
	iReturn = pasSetTcpFunName("lan_Procy_Oth",lan_Procy_Oth, 0);//ÊòæÁ§∫ÂÖ∂‰ªñÁ≠ñÁï•
	iReturn = pasSetTcpFunName("lan_Procy_Ref",lan_Procy_Ref, 0);//ÊòæÁ§∫È¢ÑÂÆö‰πâÁ≠ñÁï•
	iReturn = pasSetTcpFunName("lan_WebClass",lan_WebClass, 0);//ÁΩëÂùÄÂ∫ì
	iReturn = pasSetTcpFunName("lan_UsbClass",lan_UsbClass, 0);//USBÂ∫ì
	iReturn = pasSetTcpFunName("lan_SoftClass",lan_SoftClass, 0);//ËΩØ‰ª∂Á±ªÂûã
	iReturn = pasSetTcpFunName("lan_MainProcy",lan_MainProcy, 0);//‰∏ªÁ≠ñÁï•Â∫ì
	iReturn = pasSetTcpFunName("lan_Datedes",lan_Datedes, 0);//Êó∂Èó¥ÂØπË±°Â∫ì
	iReturn = pasSetTcpFunName("lan_Groups",lan_Groups, 0);//ÈÉ®Èó®Â∫ì
	iReturn = pasSetTcpFunName("lan_Procy_Obj",lan_Procy_Obj, 0);//Á≠ñÁï•‰∏ãÂèë
	iReturn = pasSetTcpFunName("lan_Procy_Obj2",lan_Procy_Obj2, 0);//Á≠ñÁï•‰∏ãÂèë
//	iReturn = pasSetTcpFunName("lan_Procy_TimeObj",lan_Procy_TimeObj, 0);//Á≠ñÁï•‰∏ãÂèë
	iReturn = pasSetTcpFunName("lanTree_depComp2",lanTree_depComp2, 0);//
	iReturn = pasSetTcpFunName("lan_ssjk_getScreen2",lan_ssjk_getScreen2,0);//Â±èÂπïÂÆûÊó∂ÁõëÊéß2
	iReturn = pasSetTcpFunName("lan_Procy_count_procyref",lan_Procy_count_procyref,0);//ÁªüËÆ°Á≠ñÁï•Ë¢´ÂÖ∂‰ªñÁ≠ñÁï•Áî®Ê¨°Êï∞
	iReturn = pasSetTcpFunName("lan_Procy_count_procyobj",lan_Procy_count_procyobj,0);//ÁªüËÆ°Á≠ñÁï•‰∏ãÂèëÊ¨°Êï∞
   return 0;
}
