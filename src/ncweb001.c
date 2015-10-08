#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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
#include <pcap.h>
#include "ncdef.h"
#include "nclimit.h"
#include "ncreport.h" 
extern int iDebugLevel;
static int ncUtlSortOnIp1(char *p1,char *p2);
 int ncSortHttpBufByLastTime(const void *p1,const void *p2);
static int ncWebShowIpSort(char *p1,char *p2);
static int ncWebIpExceptSort(char *p1,char *p2);
struct ncWebIpFilter_s {
    int gid;    /* 组  */
    int sf;     /* 源地址  0--之内  1--之外  */
    int df;     /* 目标地址 0--之内 1--之外  */
    int dr;     /* 0--出流量 1--进流量 2--全部  */
    unsigned long  slip;
    unsigned long  suip;
    unsigned long  dlip;
    unsigned long  duip;
    unsigned long  base;
    long           net;
    int  dport;
    unsigned long  fl;
    unsigned long fu;
    int  prot;
};

struct ncWebWebFilter_s {
    int gid;
    int sf;
    char url[128];
};

struct ncUtlNetIp_s {
    long   lId;
    long   lLip;
    long   lUip;
    unsigned long lBase;
};
long ncUtlIpNetIsIn(unsigned long lIp,struct ncUtlNetIp_s *psNet,int iSumNet);
struct ncUtlNetIp_s *ncUtlGetNetIpDesc(utShmHead *psShmHead,int *iSumNet);
char *ncWebUtlDestIp(unsigned long lIp,unsigned short nPort);
/* 显示最近访问的人员情况, 最多显示100条记录  
     ModiBy LiYunming 2002/5/22

 */
static int iOrderip;
int ncWebShowNewvister(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
//    ncHttpBuffer *psHttp;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;
    struct ncWebIpFilter_s s;
    struct ncUtlNetIp_s *psNet;
    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[16];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
    int service_flag;
    char caTemp[4024];
   	char caOpt[2048],caOpt1[2048];
   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
    pasDbCursor *psCur;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
	utMsgDelVar(psMsgHead,"action");
	utMsgAdd_s(&psMsgHead,"action","06");
  ncOperWriteLog(psMsgHead);
	lGroupid1 = (-1);
    iReturn = utMsgGetSomeNVar(psMsgHead,9,
		"groupid",   UT_TYPE_STRING, 11, caGroupid,
		"userid",    UT_TYPE_STRING, 31, caUserid,
		"service",       UT_TYPE_STRING, 10, caService,
		"CurPg",   UT_TYPE_STRING, 12, caCurpg,
		"sortbyip",   UT_TYPE_STRING, 2, caOrder,
		"way",       UT_TYPE_STRING, 10, caWay,
		"plate",     UT_TYPE_STRING, 63, caPlate,
		"username",     UT_TYPE_STRING, 63, caUsername,
		"expvalue", UT_TYPE_STRING,2,caExp);



    iOrderip=atol(caOrder);
   if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	                          
    
	if(!utStrIsSpaces(caGroupid)) {
		lGroupid1 = atol(caGroupid);
	}
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
    
    if((iReturn==0)&&(lGroupid0>0)){
    	
    	  sprintf(caTemp,"select groupid from ncgroup where groupid=%d or pid=%d ",lGroupid0,lGroupid0);
        psCur=pasDbOpenSql(caTemp,0);
        if(psCur == NULL) {
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        	return 0;
    		}
    		lGid=0;
    		iNum=0;
    		memset(caOpt,0,sizeof(caOpt));
   	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    while((iReturn == 0||iReturn==1405))
   	    {
   	    	if(iNum==0){
   	    		sprintf(caOpt,"%d",lGid);
   	    	}
   	    	else{
   	    		sprintf(caOpt+strlen(caOpt),",%d",lGid);
   	    	}
   	    	iNum++;
   	      if(iNum>150) break;
   	    	lGid=0;
   	    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    }
   	     pasDbCloseCursor(psCur);
    	
    	
    }
  
  
      if(strlen(caGroupid)>0){
    	  if(strcmp(caGroupid,"0")==0)
    	    sprintf(caTemp,"select groupid from ncgroup where groupid=0 ");
    	   else
    	    sprintf(caTemp,"select groupid from ncgroup where groupid=%s or pid=%s ",caGroupid,caGroupid);

        psCur=pasDbOpenSql(caTemp,0);
        if(psCur == NULL) {
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        	return 0;
    		}
    		lGid=0;
    		iNum=0;
    		memset(caOpt1,0,sizeof(caOpt1));
   	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    while((iReturn == 0||iReturn==1405))
   	    {
   	    	if(iNum==0){
   	    		sprintf(caOpt1,"%d",lGid);
   	    	}
   	    	else{
   	    		sprintf(caOpt1+strlen(caOpt1),",%d",lGid);
   	    	}
   	    	iNum++;
   	      if(iNum>150) break;
   	    	lGid=0;
   	    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    }
   	     pasDbCloseCursor(psCur);
    	
    	
    }
  
  
    psDbHead = utPltInitDb();
    iCount = 0;
    if(!utStrIsSpaces(caUserid)) {
    	iUserFlags = 1;
    	lUserid = atol(caUserid);
    }
//    psHttp = (ncHttpBuffer *)utShmArray((utShmHead *)psShmHead,NC_LNK_WEB);
//    if(psHttp == NULL) {
//        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg1.htm","ncWebShowIp","Internal Error1 ");
//        return 0;
//    }
    /* 取过滤条件  */
    ncWebUtlCvtIpStr(psMsgHead,&s);  
    ncWebUtlCvtIpGet(psMsgHead,caFilter);
    /* 网段描述信息  */
    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);

    if(lMaxRec <= 0) {
        utPltPutVar(psDbHead,"filter",caFilter);        
        utPltPutVar(psDbHead, "plate",caPlate);
        utPltPutVarF(psDbHead,"rate","%lu",0);
        
        iSum=0;
    	  i=0;
        while(iSum < 30) {
	        i++;
	        iSum++;
	        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iSum);
	    	}       
     utPltPutVarF(psDbHead,"CurPg","%d",1);
     utPltPutVarF(psDbHead,"TotPg","%d",1);
     utPltPutVarF(psDbHead,"TotRec","%d",0);
        
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncnewvister.htm");
        return 0;
    }
   if(lMaxRec>100000)
    lMaxRec=100000;
    
    psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
    
   
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }        
    lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    
    if(lSumRec>100000)
    lSumRec=100000;
    
     psIp1 = (ncIp **) malloc((lSumRec+2) * sizeof(ncIp *));
    
    iSum = 0;
    iMaxRecord = utComGetVar_ld(psShmHead,"MaxNewIpRec",30);
    psNet = ncUtlGetNetIpDesc(psShmHead,&iSumNet);
    s.base = 0;
    for(i=0;i<iSumNet;i++) {
        if(psNet[i].lId == s.net || s.net == 0) {
            s.base = s.base + psNet[i].lBase;
        }
    }

   if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * iMaxRecord;



    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&(psIp[i]->cStatus!=NC_USER_NOMON)) {
            psUserInfo = psIp[i]->psUser;
            if(psUserInfo) {
            	sprintf(caGid0,"%d",psUserInfo->lGid);

         	     if( ((iUserFlags == 0 && (lGroupid0 ==0 || (IsInGroupids(caOpt,caGid0)==0) )) || (iUserFlags && psUserInfo->lId == lUserid))
         	     &&((strlen(caUsername)==0)||(strstr(psUserInfo->psUser->dispname,caUsername)||(strstr(psUserInfo->psUser->username,caUsername))))) {
        		      if(ncWebUtlIpFilter(psIp[i],&s,psUserInfo->lGid,psNet,iSumNet)&& (lGroupid1 == (-1) || IsInGroupids(caOpt1,caGid0)==0)) {
        		      	
        		      	if(atol(caService)==2000){

        		      		if((psIp[i]->lService>=2000)&&(psIp[i]->lService<=2999)){
        		      		
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		
        		      		}
        		      	}
        		      	else if(atol(caService)==1000){
        		      		if((psIp[i]->lService>=1000)&&(psIp[i]->lService<=1999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==3000){
        		      		if((psIp[i]->lService>=3000)&&(psIp[i]->lService<=3999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==4000){
        		      		if((psIp[i]->lService>=4000)&&(psIp[i]->lService<=4999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==5000){
        		      		if((psIp[i]->lService>=5000)&&(psIp[i]->lService<=5999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==6000){
        		      		if((psIp[i]->lService>=6000)&&(psIp[i]->lService<=6999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==7000){
        		      		if((psIp[i]->lService>=7000)&&(psIp[i]->lService<=7999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==8000){
        		      		if((psIp[i]->lService>=8000)&&(psIp[i]->lService<=8999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==9000){
        		      		if((psIp[i]->lService>=9000)&&(psIp[i]->lService<=9999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==10000){
        		      		if((psIp[i]->lService>=10000)&&(psIp[i]->lService<=10999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		        else if(atol(caService)==11000){
        		      		if((psIp[i]->lService>=11000)&&(psIp[i]->lService<=11999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==12000){
        		      		if((psIp[i]->lService>=12000)&&(psIp[i]->lService<=12999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==13000){
        		      		if((psIp[i]->lService>=13000)&&(psIp[i]->lService<=13999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==14000){
        		      		if((psIp[i]->lService>=14000)&&(psIp[i]->lService<=14999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==15000){
        		      		if((psIp[i]->lService>=15000)&&(psIp[i]->lService<=15999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(atol(caService)==17000){
        		      		if((psIp[i]->lService>=17000)&&(psIp[i]->lService<=17999)){
        		      			service_flag=1;
        		      		}
        		      		else{
        		      			service_flag=0;
        		      		}
        		      	
        		      	}
        		      	else if(strlen(caService)==0){
        		      		service_flag=1;
        		      		
        		      	}
        		      	else{
        		      		if(psIp[i]->lService==atol(caService)){
        		      		  service_flag=1;
        		      		}
        		      		 else{
        		      		  service_flag=0;
        		      		}
        		      		
        		      	}
        		     
        		      	if(service_flag==1){
        		      		psIp1[iSum]=psIp[i];
        		      		iSum++;
        		        }
        		      	
  /*      		      	
            		      utPltPutLoopVar(psDbHead,"loginname", iSum+1,psUserInfo->psUser->username);
            		      utPltPutLoopVar(psDbHead,"name", iSum+1,psUserInfo->psUser->dispname);
            		      utPltPutLoopVar(psDbHead,"dept", iSum+1,psUserInfo->psUser->psGroup->groupname);
        	              utPltPutLoopVar(psDbHead,"sip",  iSum+1,utComHostIp(htonl(psIp[i]->lSip)));
                          utPltPutLoopVar(psDbHead,"dip",  iSum+1,utComHostIp(htonl(psIp[i]->lDip)));
                          utPltPutLoopVarF(psDbHead,"port", iSum+1,"%d",psIp[i]->nDport);
                          utPltPutLoopVar(psDbHead,"service", iSum+1,ncUtlGetServiceName(psShmHead,psIp[i]->lService,"\0"));
                          utPltPutLoopVar(psDbHead, "StartTime",iSum+1,
                          utTimFormat("%H:%M:%S",psIp[i]->lLastTime));
                          utPltPutLoopVarF(psDbHead, "ConnTime",iSum+1,"%d",psIp[i]->lLastTime - psIp[i]->lStartTime);
                          utPltPutLoopVarF(psDbHead, "inbytes",iSum+1,"%llu",psIp[i]->lBytes[1]);
                          utPltPutLoopVarF(psDbHead, "outbytes",iSum+1,"%llu",psIp[i]->lBytes[0]);
                          if(psIp[i]->lUrl > 0) {  /* 显示网址  */
 /*                               char caUrl0[200];
                                char caUrl1[200];
                                ncUtlCvtJsStr(psHttp[psIp[i]->lUrl].caUrl,caUrl1);                                
      	                        sprintf(caUrl0,"http://%s%s",psHttp[psIp[i]->lUrl].caHost,
      	                            caUrl1);
                                utPltPutLoopVar(psDbHead, "url",iSum+1,caUrl0);
                          }
                          /* 流量比例  */
 /*                           if(lSumBytes > 0) {
                                if(s.dr > 1) {
                                    dFlow = (psIp[i]->lBytes[0] + psIp[i]->lBytes[1]) * 100 / lSumBytes;
                                }
                                else {
                                    dFlow = psIp[i]->lBytes[s.dr] * 100 / lSumBytes;
                                }

                            }
                            else {
                                dFlow = 0;
                            }
                            utPltPutLoopVarF(psDbHead, "flowrate",iSum+1,"%7.2f%c",dFlow,'%');
                            /* 带宽比例,最近30秒内的带宽   */
 /*                           if(lSumBand > 0) {
                                if(s.dr > 1) {
                                    if(psIp[i]->lStartTime > 0 && (lTime - psIp[i]->lStartTime > 30)) {
                                        lBand = (psIp[i]->lBytes[0] + psIp[i]->lBytes[1]) / (lTime - psIp[i]->lStartTime);
                                    }
                                    else {
                                        lBand = (psIp[i]->lBytes[0] + psIp[i]->lBytes[1]) / 30;
                                    }
                                }
                                else {
                                    if(psIp[i]->lStartTime > 0 && (lTime - psIp[i]->lStartTime > 30)) {
                                        lBand = psIp[i]->lBytes[s.dr] / (lTime - psIp[i]->lStartTime);
                                    }
                                    else {
                                        lBand = psIp[i]->lBytes[s.dr]  / 30;
                                    }
                                }
                            }
                            else {
                                lBand = 0;
                            }
                            utPltPutLoopVarF(psDbHead, "band",iSum+1,"%s",utStrLtoF8(lBand,caTemp));
*/
                            
             		  }
        
                }
            }
        }
    }
    

        if(iOrderip > 0) {
        qsort(psIp1, iSum,sizeof(ncIp *),ncUtlSortOnIp1);
    }
    
    
  
  
  
      if(atol(caExp)==1) {
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"nciponline%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
  
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0891阻断查询"),ncLang("0546数据导出出错"));
            return 0;
        }
        fprintf(fp,ncLang("1500实时IP连接记录\n"));
       
       
        ncUtlPrintCsv(fp,9,
                       UT_TYPE_STRING,ncLang("0693姓  名"),
                       UT_TYPE_STRING,ncLang("0147部门"),
                       UT_TYPE_STRING,ncLang("0521时间"),
                       UT_TYPE_STRING,ncLang("0253服务"),
                       UT_TYPE_STRING,ncLang("0820源IP地址"),
                       UT_TYPE_STRING,ncLang("0397目标IP地址"),
                       UT_TYPE_STRING,ncLang("0237端口"),
                       UT_TYPE_STRING,ncLang("0280进流量(Bytes)"),
                       UT_TYPE_STRING,ncLang("0279出流量(Bytes)"));
                     
        for(i = 0; i < 50000 && i< iSum;i++)
    		{
    			  psUserInfo = psIp1[i]->psUser;
		        if(psUserInfo){
		        	sprintf(temp_port,"%d",psIp1[i]->nDport);
		        	strcpy(temp_in,utStrLtoF8(psIp1[i]->lBytes[1],caTemp));
		       
		        	strcpy(temp_dip,utComHostIp(htonl(psIp1[i]->lDip)));

		        	ncUtlPrintCsv(fp,9,
                       UT_TYPE_STRING,psUserInfo->psUser->dispname,
                       UT_TYPE_STRING,psUserInfo->psUser->psGroup->groupname,
                       UT_TYPE_STRING,utTimFormat("%m-%d %H:%M:%S",psIp1[i]->lLastTime),
                       UT_TYPE_STRING,ncUtlGetServiceName(psShmHead,psIp1[i]->lService,"\0"),
                       UT_TYPE_STRING,utComHostIp(htonl(psIp1[i]->lSip)),
                       UT_TYPE_STRING,temp_dip,
                       UT_TYPE_STRING,temp_port,
                       UT_TYPE_STRING,temp_in,
                       UT_TYPE_STRING,utStrLtoF8(psIp1[i]->lBytes[0],caTemp));
		        	
		        }
		       
        }
        free(psIp);
        free(psIp1);
        free(psNet);
        fclose(fp);

        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        return 0;
    }
    
    
    
      lTotpg = (iSum - 1) / iMaxRecord + 1; 
      utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
      utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
      utPltPutVarF(psDbHead,"TotRec","%d",iSum);
     
     
     iNum=0;
      for(i = lStartRec; i < iSum && iNum < iMaxRecord;i++) {
      	psUserInfo = psIp1[i]->psUser;
      	            	utPltPutLoopVar(psDbHead,"loginname", iNum+1,psUserInfo->psUser->username);
            		      utPltPutLoopVar(psDbHead,"name", iNum+1,psUserInfo->psUser->dispname);
            		      utPltPutLoopVarF(psDbHead,"userid",iNum+1,"%lu",psUserInfo->psUser->userid);
            		     
            		      utPltPutLoopVar(psDbHead,"position",iNum+1,psUserInfo->psUser->position);
            		      if(psUserInfo->psUser->psGroup){
            		         utPltPutLoopVar(psDbHead,"dept", iNum+1,psUserInfo->psUser->psGroup->groupname);
            		        }
            		        utPltPutLoopVar(psDbHead,"mac",iNum+1,ncCvtMac(psUserInfo->psUser->mac));
        	              utPltPutLoopVar(psDbHead,"sip",  iNum+1,utComHostIp(htonl(psIp1[i]->lSip)));
                          utPltPutLoopVar(psDbHead,"dip",  iNum+1,utComHostIp(htonl(psIp1[i]->lDip)));
                          utPltPutLoopVarF(psDbHead,"port", iNum+1,"%d",psIp1[i]->nDport);
                          utPltPutLoopVarF(psDbHead,"sport", iNum+1,"%d",psIp1[i]->nSport);
                          
                          
                          
                        if(psIp1[i]->cType == 6) {
								            utPltPutLoopVar(psDbHead,"prot",   iNum+1,"TCP");
								        }
								        else {
								            utPltPutLoopVar(psDbHead,"prot",   iNum+1,"UDP");
								        } 
                          utPltPutLoopVar(psDbHead,"service", iNum+1,ncUtlGetServiceName(psShmHead,psIp1[i]->lService,"\0"));
                          utPltPutLoopVar(psDbHead, "StartTime",iNum+1,
                          utTimFormat("%H:%M:%S",psIp1[i]->lLastTime));
                          utPltPutLoopVarF(psDbHead, "ConnTime",iNum+1,"%d",psIp1[i]->lLastTime - psIp1[i]->lStartTime);
                          utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(psIp1[i]->lBytes[1],caTemp));
                          utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(psIp1[i]->lBytes[0],caTemp));
                          
                          if(lSumBytes > 0) {
                                if(s.dr > 1) {
                                    dFlow = (psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) * 100 / lSumBytes;
                                }
                                else {
                                    dFlow = psIp1[i]->lBytes[s.dr] * 100 / lSumBytes;
                                }

                            }
                            else {
                                dFlow = 0;
                            }
                          
                           utPltPutLoopVarF(psDbHead, "flowrate",iNum+1,"%7.2f%c",dFlow,'%');
                              if(lSumBand > 0) {
                                if(s.dr > 1) {
                                    if(psIp1[i]->lStartTime > 0 && (lTime - psIp1[i]->lStartTime > 30)) {
                                        lBand = 8*(psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) / (lTime - psIp1[i]->lStartTime);
                                    }
                                    else {
                                        lBand = 8*(psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) / 30;
                                    }
                                }
                                else {
                                    if(psIp1[i]->lStartTime > 0 && (lTime - psIp1[i]->lStartTime > 30)) {
                                        lBand = 8*psIp1[i]->lBytes[s.dr] / (lTime - psIp1[i]->lStartTime);
                                    }
                                    else {
                                        lBand = 8*psIp1[i]->lBytes[s.dr]  / 30;
                                    }
                                }
                            }
                            else {
                                lBand = 0;
                            }
                            utPltPutLoopVarF(psDbHead, "band",iNum+1,"%s",utStrLtoF8(lBand,caTemp));
                          
                          
                          
     iNum++;
    }
     i=0;
    while(iNum < iMaxRecord) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    lRate = lSumBand / 30;
    utPltPutVar(psDbHead, "username",caName);
    utPltPutVar(psDbHead, "way",caWay);
    utPltPutVar(psDbHead, "plate",caPlate);
    utPltPutVarF(psDbHead,"rate","%s",utStrLtoF8(lRate,caTemp));
    utPltPutVarF(psDbHead,"sumbytes","%llu",lSumBytes);
    utPltPutVar(psDbHead,"filter",caFilter);
    free(psIp);
    free(psIp1);
    free(psNet);
    utPltPutVar(psDbHead,"title_name",caTitlename);
    if(strcasecmp(utComGetVar_sd(psShmHead,"SaveSport","No"),"Yes") == 0){
    	    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncnewvister_sport.htm");
    }
    else{
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncnewvister.htm");
      }
    return 0;
}     

/*
    对即时ip进行排序
*/

static int ncUtlSortOnIp1(char *p1,char *p2)

{
    int i;
   ncIp *psIp1,*psIp2;
    memcpy(&psIp1,p1,4);
    memcpy(&psIp2,p2,4);    

        switch(iOrderip) {
            case 1: return strcasecmp(psIp1->psUser->psUser->dispname,psIp2->psUser->psUser->dispname);
            case 2: return strcasecmp(psIp1->psUser->psUser->psGroup->groupname,psIp2->psUser->psUser->psGroup->groupname);
            case 3: return psIp1->lSip - psIp2->lSip;
            case 4: return psIp1->lDip - psIp2->lDip;
            case 5: return psIp1->nDport - psIp2->nDport;
            case 6: return psIp2->lService - psIp1->lService;
            case 7: return psIp2->lBytes[0] - psIp1->lBytes[0];
            case 8: return psIp2->lBytes[1] - psIp1->lBytes[1];
            case 9: return psIp1->lStartTime - psIp2->lStartTime;
            case 10: return psIp1->psUser->lId-psIp2->psUser->lId;
        }
    
    return 1;
}



/* 从内存中取符合条件的当前IP访问情况  
              lSumBytes --- 总流量
              
 */
int ncWebGetCurIpVister(utShmHead *psShmHead,ncIp **psIp,unsigned long lMax,
                           unsigned long long *lSumBytes,
                           unsigned long long *lSumBand)
{
    char *pHash;
    ncIp *psIp0;
    pasHashInfo sHashInfo;    
    register int i;
    unsigned long  lSum;
    unsigned long long l1,l2;
    l1 = 0;
    l2 = 0;
    pHash = utShmHashHead(psShmHead,NC_LNK_IPPKG);
    if(pHash == NULL) {
        return (-1);
    }
    psIp0  = (ncIp *)pasHashFirst(pHash,&sHashInfo);
    lSum = 0;
    while(psIp0 && lSum < lMax) {
       psIp[lSum] = psIp0;
       l1 += psIp0->lBytes[0] + psIp0->lBytes[1];
       if(psIp0->lStartTime > 0 && (psIp0->lLastTime - psIp0->lStartTime > 30)) {
           l2 += (psIp0->lBytes[0] + psIp0->lBytes[1]) * 30 / (psIp0->lLastTime - psIp0->lStartTime);
       }
       else {
           l2 += psIp0->lBytes[0] + psIp0->lBytes[1];
       }
       lSum++;
       psIp0 = (ncIp *)pasHashNext(&sHashInfo);
    }
    qsort(psIp,lSum,sizeof(ncIp *),ncWebShowIpSort);
    *lSumBytes = l1;
    *lSumBand = l2;
    return lSum;
}

static int ncWebShowIpSort(char *p1,char *p2)
{
    ncIp *psIp1,*psIp2;
    memcpy(&psIp1,p1,4);
    memcpy(&psIp2,p2,4);    
    return (psIp2->lLastTime - psIp1->lLastTime);
}


/* 显示最近访问Web的人员情况  */
/*  20110803修改
int ncWebShowNewweb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iTitleMark=0;
    int iTitleOnly=0;
    int iSum;
    int iMaxRec;
    int iUserFlags;
    int iMaxRecord,iSumRec;
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
    long lUserid;
    long lGroupid0;
	long lGroupid1;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
	char caDipPort[30];
	char caUsername[32];
    char caUserid[32];
    ncHttpBuffer *psHttp;
    ncHttpBuffer **psHttp0;
    char caUrl0[255],caUrl[255];
    ncUserCont   *psUser;
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;
    struct ncWebWebFilter_s s;	
    char caFilter[256];
    char caTitlename[32];
    char caTemp[4024];
   	char caOpt[2048],caOpt1[2048];
   	long lGid;
   	char caGid0[20];
   	char caExp[16];
    pasDbCursor *psCur;
    char caUrlid[16],caWebtitle[64];
    ncLimHead *psLimHead;
    unsigned long lUrlid,lWebid;
    long cid;
    pasHashInfo sHashInfo;
    ncHttpHashBuffer *psHttp;
    unsigned char *pHash;
    
    
    char caClass[64],caTitle[128];
    iUserFlags = 0;
	lGroupid1 = (-1);
	utMsgDelVar(psMsgHead,"action");
	utMsgAdd_s(&psMsgHead,"action","06");
	ncOperWriteLog(psMsgHead);
    if(strcasecmp(utComGetVar_sd(psShmHead,"HttpContent","No"),"Title")==0) {
        iTitleMark = 1;
    }
    if(strcasecmp(utComGetVar_sd(psShmHead,"HttpTitleOnly","No"),"Yes")==0) {
        iTitleOnly = 1;
    }
    

    iReturn = utMsgGetSomeNVar(psMsgHead,9,
    "CurPg",   UT_TYPE_STRING, 12, caCurpg,
		"groupid",   UT_TYPE_STRING, 11, caGroupid,
		"userid",    UT_TYPE_STRING, 31, caUserid,
		"way",       UT_TYPE_STRING, 10, caWay,
		"plate",     UT_TYPE_STRING, 63, caPlate,
		"username",    UT_TYPE_STRING, 31, caUsername,
		"urlid",UT_TYPE_STRING,10,caUrlid,
   	"webtitle",UT_TYPE_STRING,63,caWebtitle,
   	"expvalue", UT_TYPE_STRING,2,caExp);
		
    psLimHead = (ncLimHead *)ncLimThisHead();
 
		if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	                          
	if(!utStrIsSpaces(caGroupid)) {
		lGroupid1 = atol(caGroupid);
	}
	 iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
    
   if((iReturn==0)&&(lGroupid0>0)){
    	
    	  sprintf(caTemp,"select groupid from ncgroup where groupid=%d or pid=%d ",lGroupid0,lGroupid0);
        psCur=pasDbOpenSql(caTemp,0);
        if(psCur == NULL) {
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        	return 0;
    		}
    		lGid=0;
    		iNum=0;
    		memset(caOpt,0,sizeof(caOpt));
   	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    while((iReturn == 0||iReturn==1405))
   	    {
   	    	if(iNum==0){
   	    		sprintf(caOpt,"%d",lGid);
   	    	}
   	    	else{
   	    		sprintf(caOpt+strlen(caOpt),",%d",lGid);
   	    	}
   	    	iNum++;
   	      if(iNum>150) break;
   	    	lGid=0;
   	    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    }
   	     pasDbCloseCursor(psCur);
    	
    	
    }
    
    if(strlen(caGroupid)>0){
    	  if(strcmp(caGroupid,"0")==0)
    	    sprintf(caTemp,"select groupid from ncgroup where groupid=0 ");
    	   else
    	    sprintf(caTemp,"select groupid from ncgroup where groupid=%s or pid=%s ",caGroupid,caGroupid);

        psCur=pasDbOpenSql(caTemp,0);
        if(psCur == NULL) {
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        	return 0;
    		}
    		lGid=0;
    		iNum=0;
    		memset(caOpt1,0,sizeof(caOpt1));
   	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    while((iReturn == 0||iReturn==1405))
   	    {
   	    	if(iNum==0){
   	    		sprintf(caOpt1,"%d",lGid);
   	    	}
   	    	else{
   	    		sprintf(caOpt1+strlen(caOpt1),",%d",lGid);
   	    	}
   	    	iNum++;
   	      if(iNum>150) break;
   	    	lGid=0;
   	    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    }
   	     pasDbCloseCursor(psCur);
    	
    	
    }
    
    
    
    
    
    if(!utStrIsSpaces(caUserid)) {
    	iUserFlags = 1;
    	lUserid = atol(caUserid);
    }





    psHttp = (ncHttpBuffer *)utShmArray((utShmHead *)psShmHead,NC_LNK_WEB);
    if(psHttp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg1.htm","ncWebShowIp","Internal Error1 ");
        return 0;
    }        
    iSumRec = 0;
    iMaxRecord = utShmArrayRecord(psShmHead,NC_LNK_WEB);
    ncWebUtlCvtWebStr(psMsgHead,&s);  
    ncWebUtlCvtIpGet(psMsgHead,caFilter);
    psHttp0 = (ncHttpBuffer **)malloc(iMaxRecord * sizeof(ncHttpBuffer *));
    if(psHttp0 == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm","ncWebShowIp","Internal Error1 ");
        return 0;
    }

    for(i=1;i<iMaxRecord;i++) {
        if(psHttp[i].cUseFlags == 1 ||psHttp[i].cUseFlags == 2 ) {
        	psUser = ncUtlGetUserContById(psShmHead,psHttp[i].userid);
        	if(psUser) {
        		sprintf(caGid0,"%d",psUser->groupid);
        		
            if((strlen(caUsername)==0)||strstr(psUser->dispname,caUsername)||strstr(psUser->username,caUsername)){
            	if( ((iUserFlags == 0 && (lGroupid0 == 0 || IsInGroupids(caOpt,caGid0)==0) ) || (iUserFlags && psUser->userid == lUserid))) {
           		    if(ncWebUtlWebFilter(&psHttp[i],&s,psUser->groupid)) {
        	       			if((strlen(caWebtitle)==0||(strlen(caWebtitle)>0&&strstr(psHttp[i].caTitle,caWebtitle)))&& (lGroupid1 == (-1) || IsInGroupids(caOpt1,caGid0)==0)){

                           lUrlid=0;
        	       			     ncLimGetWebId(psLimHead,psHttp[i].caHost,&lUrlid,&lWebid);

        	       			     if((strlen(caUrlid)==0)||(lUrlid==atol(caUrlid))){
										            if(iTitleOnly) {
										                if(!utStrIsSpaces(psHttp[i].caTitle)) {
										                    psHttp0[iSumRec] = &psHttp[i];
										                    iSumRec++;
										                }
										            }
										            else {
										                psHttp0[iSumRec] = &psHttp[i];
										                iSumRec++;
										            }
										         } 
								          }
								        }
								      }
								    }
								  }               
        }
    }
   
    qsort(psHttp0,iSumRec,sizeof(ncHttpBuffer *),ncSortHttpBufByLastTime);
    






      if(atol(caExp)==1) {
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"ncwebonline%d.txt",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
  
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0891阻断查询"),ncLang("0546数据导出出错"));
            return 0;
        }
     //   fprintf(fp,ncLang("1500实时IP连接记录\n"));
       
  /*     
        ncUtlPrintCsv(fp,9,
                       UT_TYPE_STRING,ncLang("0693姓  名"),
                       UT_TYPE_STRING,ncLang("0147部门"),
                       UT_TYPE_STRING,ncLang("0521时间"),
                       UT_TYPE_STRING,ncLang("0253服务"),
                       UT_TYPE_STRING,ncLang("0820源IP地址"),
                       UT_TYPE_STRING,ncLang("0397目标IP地址"),
                       UT_TYPE_STRING,ncLang("0237端口"),
                       UT_TYPE_STRING,ncLang("0280进流量(Bytes)"),
                       UT_TYPE_STRING,ncLang("0279出流量(Bytes)"));
    */ 
 /*                
        for(i = 0; i < 50000 && i< iSumRec;i++)
    		{

             fprintf(fp,"%s,%s\r\n",psHttp0[i]->caHost,psHttp0[i]->caTitle);
//		        	ncUtlPrintCsv(fp,2,
//                       UT_TYPE_STRING,psHttp0[i]->caHost,
//                       UT_TYPE_STRING,psHttp0[i]->caTitle);
                     	        	
		        }
		       
        
   free(psHttp0);
        fclose(fp);

        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        return 0;
    }








//	printf("iSumRec=%lu\n",iSumRec);
	
    psDbHead = utPltInitDb();

    
    iMaxRec = utComGetVar_ld(psShmHead,"MaxNewWebRec",50);
       if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * iMaxRec;
      lTotpg = (iSumRec - 1) / iMaxRec + 1; 
      utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
      utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
      utPltPutVarF(psDbHead,"TotRec","%d",iSumRec);
   iSum = 0;
    for(j=lStartRec;j<iSumRec&&iSum<iMaxRec;j++) {
        if(psHttp0[j]->cUseFlags > 0) {
            psUser = ncUtlGetUserContById(psShmHead,psHttp0[j]->userid);
           
           		        ncUtlCvtJsStr(psHttp0[j]->caUrl,caUrl);
     	                sprintf(caUrl0,"http://%s%s",psHttp0[j]->caHost,caUrl);
     	                strcpy(caUrl,caUrl0);
     	                caUrl[80]=0;
        	            utPltPutLoopVar(psDbHead,"sip",  iSum+1,utComHostIp(htonl(psHttp0[j]->lSip)));
  utPltSetCvtHtml(1);       	            
                        utPltPutLoopVar(psDbHead,"name", iSum+1,psUser->dispname);
                        if(psUser->psGroup)
                           utPltPutLoopVar(psDbHead,"dept", iSum+1,psUser->psGroup->groupname);
                           
                        utPltPutLoopVar(psDbHead,"url",  iSum+1,caUrl);
                        memset(caTitle,0,sizeof(caTitle));
                        cid=0;
                        ncUrlGetUrlInfo(psHttp0[j]->caHost,caTitle,&cid);
   //                     printf("caTitle=%s\n",caTitle);
                        if(strlen(psHttp0[j]->caTitle)>0)
                          utPltPutLoopVar(psDbHead,"title",iSum+1,psHttp0[j]->caTitle);
                        else
                           utPltPutLoopVar(psDbHead,"title",iSum+1,caTitle);
                           
                           utPltPutLoopVar(psDbHead,"urlclass",iSum+1,ncUrlGetClassNameById (cid,""));
                           
                        utPltPutLoopVar(psDbHead,"urllnk", iSum+1,caUrl0);
  utPltSetCvtHtml(0);            
                        utPltPutLoopVarF(psDbHead,"userid",iSum+1,"%lu",psUser->userid);
            		        utPltPutLoopVar(psDbHead,"position",iSum+1,psUser->position);
                        utPltPutLoopVar(psDbHead,"starttime",iSum+1,utTimFormat("%m-%d %H:%M:%S",psHttp0[j]->lLastTime));
                        
                        if(psHttp0[j]->lFid>0){
                        	sprintf(caTemp,"<a href=\"/pronline/Msg?FunName@ncWebDispHtmlFid&fid@%lu\" target=\"_blank\">网页快照</a>",psHttp0[j]->lFid);
                        utPltPutLoopVar(psDbHead,"wykz",iSum+1,caTemp);
                       }
                        
                        utPltPutLoopVarF(psDbHead,"bytes",iSum+1,"%lu",psHttp0[j]->lBytes);
                        if(psHttp0[j]->lDip!=0)
                        {
        									sprintf(caDipPort,"%s:%d",utComHostIp(htonl(psHttp0[j]->lDip)),psHttp0[j]->nDport);
        									utPltPutLoopVar(psDbHead,"dip",iSum+1,caDipPort);
        								}
      
            		    iSum++;
            	
   
    }
    
  }
  
   
     i=0;
    

    while(iSum < iMaxRec) {
        i++;
        iSum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iSum);
    }
    
    utPltPutVar(psDbHead, "way",caWay);
    utPltPutVar(psDbHead, "plate",caPlate);
    utPltPutVar(psDbHead, "filter",caFilter);
    utPltPutVar(psDbHead,"title_name",caTitlename);
 //   if(iTitleMark==1) {
    	
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncnewweb_title.htm");
//    }
//    else {
    
//        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncnewweb.htm");
//    }
    free(psHttp0);
    
    return 0;
}    
*/






int ncWebShowNewweb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iTitleMark=0;
    int iTitleOnly=0;
    int iSum;
    int iMaxRec;
    int iUserFlags;
    int iMaxRecord,iSumRec;
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
    long lUserid;
    long lGroupid0;
	long lGroupid1;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
	char caDipPort[30];
	char caUsername[32];
    char caUserid[32];

    ncHttpHashBuffer **psHttp0;
    char caUrl0[255],caUrl[255];
    ncUserCont   *psUser;
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;
    struct ncWebWebFilter_s s;	
    char caFilter[256];
    char caTitlename[32];
    char caTemp[4024];
   	char caOpt[2048],caOpt1[2048];
   	long lGid;
   	char caGid0[20];
   	char caExp[16];
    pasDbCursor *psCur;
    char caUrlid[16],caWebtitle[64];
    ncLimHead *psLimHead;
    unsigned long lUrlid,lWebid;
    long cid;
    pasHashInfo sHashInfo;
    ncHttpHashBuffer *psHttp;
    unsigned char *pHash;
    
    
    char caClass[64],caTitle[128];
    iUserFlags = 0;
	lGroupid1 = (-1);
	utMsgDelVar(psMsgHead,"action");
	utMsgAdd_s(&psMsgHead,"action","06");
	ncOperWriteLog(psMsgHead);
    if(strcasecmp(utComGetVar_sd(psShmHead,"HttpContent","No"),"Title")==0) {
        iTitleMark = 1;
    }
    if(strcasecmp(utComGetVar_sd(psShmHead,"HttpTitleOnly","No"),"Yes")==0) {
        iTitleOnly = 1;
    }
    


    iReturn = utMsgGetSomeNVar(psMsgHead,9,
    "CurPg",   UT_TYPE_STRING, 12, caCurpg,
		"groupid",   UT_TYPE_STRING, 11, caGroupid,
		"userid",    UT_TYPE_STRING, 31, caUserid,
		"way",       UT_TYPE_STRING, 10, caWay,
		"plate",     UT_TYPE_STRING, 63, caPlate,
		"username",    UT_TYPE_STRING, 31, caUsername,
		"urlid",UT_TYPE_STRING,10,caUrlid,
   	"webtitle",UT_TYPE_STRING,63,caWebtitle,
   	"expvalue", UT_TYPE_STRING,2,caExp);
		
 //   psLimHead = (ncLimHead *)ncLimThisHead();
 
		if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    }
    else if(strlen(caGroupid)!=0){   	    	
		        	strcpy(caTitlename,ncUtlGetGroupNameByid(psShmHead,atol(caGroupid),ncLang("0629未知")));		        	
		}		       
    else{
    	strcpy(caTitlename,"");
    }	                          
	if(!utStrIsSpaces(caGroupid)) {
		lGroupid1 = atol(caGroupid);
	}
	 iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
    
   if((iReturn==0)&&(lGroupid0>0)){
    	
    	  sprintf(caTemp,"select groupid from ncgroup where groupid=%d or pid=%d ",lGroupid0,lGroupid0);
        psCur=pasDbOpenSql(caTemp,0);
        if(psCur == NULL) {
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        	return 0;
    		}
    		lGid=0;
    		iNum=0;
    		memset(caOpt,0,sizeof(caOpt));
   	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    while((iReturn == 0||iReturn==1405))
   	    {
   	    	if(iNum==0){
   	    		sprintf(caOpt,"%d",lGid);
   	    	}
   	    	else{
   	    		sprintf(caOpt+strlen(caOpt),",%d",lGid);
   	    	}
   	    	iNum++;
   	      if(iNum>150) break;
   	    	lGid=0;
   	    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    }
   	     pasDbCloseCursor(psCur);
    	
    	
    }


    
    if(strlen(caGroupid)>0){
    	  if(strcmp(caGroupid,"0")==0)
    	    sprintf(caTemp,"select groupid from ncgroup where groupid=0 ");
    	   else
    	    sprintf(caTemp,"select groupid from ncgroup where groupid=%s or pid=%s ",caGroupid,caGroupid);

        psCur=pasDbOpenSql(caTemp,0);
        if(psCur == NULL) {
        	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        	return 0;
    		}
    		lGid=0;
    		iNum=0;
    		memset(caOpt1,0,sizeof(caOpt1));
   	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    while((iReturn == 0||iReturn==1405))
   	    {
   	    	if(iNum==0){
   	    		sprintf(caOpt1,"%d",lGid);
   	    	}
   	    	else{
   	    		sprintf(caOpt1+strlen(caOpt1),",%d",lGid);
   	    	}
   	    	iNum++;
   	      if(iNum>150) break;
   	    	lGid=0;
   	    	iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid);
   	    }
   	     pasDbCloseCursor(psCur);
    	
    	
    }
    
    
    
    
    
    if(!utStrIsSpaces(caUserid)) {
    	iUserFlags = 1;
    	lUserid = atol(caUserid);
    }



        
    iSumRec = 0;
    iMaxRecord = utShmHashRecord(psShmHead,NC_LNK_HTTPHASH);

    ncWebUtlCvtWebStr(psMsgHead,&s);  
    ncWebUtlCvtIpGet(psMsgHead,caFilter);
    psHttp0 = (ncHttpHashBuffer **)malloc(iMaxRecord * sizeof(ncHttpHashBuffer *));
    if(psHttp0 == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm","ncWebShowIp","Internal Error1 ");
        return 0;
    }
    pHash =  (unsigned char *)utShmHashHead(psShmHead,NC_LNK_HTTPHASH);
    
    

    if(pHash) {
        psHttp = (ncHttpHashBuffer *)pasHashFirst(pHash,&sHashInfo);
       while(psHttp) {
       	   psUser = ncUtlGetUserContById(psShmHead,psHttp->userid);
        	if(psUser) {
        		sprintf(caGid0,"%d",psUser->groupid);
       	        if((strlen(caUsername)==0)||strstr(psUser->dispname,caUsername)||strstr(psUser->username,caUsername)){
            	   if( ((iUserFlags == 0 && (lGroupid0 == 0 || IsInGroupids(caOpt,caGid0)==0) ) || (iUserFlags && psUser->userid == lUserid))) {
           		    if(ncWebUtlWebFilter(psHttp,&s,psUser->groupid)) {
        	       			if((strlen(caWebtitle)==0||(strlen(caWebtitle)>0&&strstr(psHttp->caTitle,caWebtitle)))&& (lGroupid1 == (-1) || IsInGroupids(caOpt1,caGid0)==0)){
        	       				
        	       				   lUrlid=0;
      	       			     ncLimGetWebId(psLimHead,psHttp->caHost,&lUrlid,&lWebid);
	        	       				if((strlen(caUrlid)==0)||(lUrlid==atol(caUrlid))){
	                                    if(iSumRec<iMaxRecord){
		        	       										psHttp0[iSumRec] = psHttp;
		        	     
										                    iSumRec++;
										                  }
		        	       				}
								        }
								      }
								    }
               }
       	
         }
         psHttp = (ncHttpHashBuffer *)pasHashNextS(&sHashInfo);
        }
     
   }

    qsort(psHttp0,iSumRec,sizeof(ncHttpHashBuffer *),ncSortHttpBufByLastTime);
    





      if(atol(caExp)==1) {
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        sprintf(caFilename,"ncwebonline%d.txt",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
  
        fp = fopen(caFile,"w");
        if(fp == NULL) {
            pasDbCloseCursor(psCur);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0891阻断查询"),ncLang("0546数据导出出错"));
            return 0;
        }
     //   fprintf(fp,ncLang("1500实时IP连接记录\n"));
       
    
        ncUtlPrintCsv(fp,9,
                       UT_TYPE_STRING,ncLang("0693姓  名"),
                       UT_TYPE_STRING,ncLang("0147部门"),
                       UT_TYPE_STRING,ncLang("0521时间"),
                       UT_TYPE_STRING,ncLang("0253服务"),
                       UT_TYPE_STRING,ncLang("0820源IP地址"),
                       UT_TYPE_STRING,ncLang("0397目标IP地址"),
                       UT_TYPE_STRING,ncLang("0237端口"),
                       UT_TYPE_STRING,ncLang("0280进流量(Bytes)"),
                       UT_TYPE_STRING,ncLang("0279出流量(Bytes)"));
    
                 
        for(i = 0; i < 50000 && i< iSumRec;i++)
    		{

             fprintf(fp,"%s,%s\r\n",psHttp0[i]->caHost,psHttp0[i]->caTitle);
//		        	ncUtlPrintCsv(fp,2,
//                       UT_TYPE_STRING,psHttp0[i]->caHost,
//                       UT_TYPE_STRING,psHttp0[i]->caTitle);
                     	        	
		        }
		       
        
          free(psHttp0);
          fclose(fp);

        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
        return 0;
    }





//	printf("iSumRec=%lu\n",iSumRec);
	
    psDbHead = utPltInitDb();

    
    iMaxRec = utComGetVar_ld(psShmHead,"MaxNewWebRec",50);
       if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * iMaxRec;
      lTotpg = (iSumRec - 1) / iMaxRec + 1; 
      utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
      utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
      utPltPutVarF(psDbHead,"TotRec","%d",iSumRec);
   iSum = 0;


    for(j=lStartRec;j<iSumRec&&iSum<iMaxRec;j++) {

            psUser = ncUtlGetUserContById(psShmHead,psHttp0[j]->userid);
      
          		        ncUtlCvtJsStr(psHttp0[j]->caUrl,caUrl);
     	                sprintf(caUrl0,"%s://%s%s",ncHttpProtocol(psHttp0[j]->cFlags),psHttp0[j]->caHost,caUrl);
     	                strcpy(caUrl,caUrl0);
     	                caUrl[80]=0;
        	            utPltPutLoopVar(psDbHead,"sip",  iSum+1,utComHostIp(htonl(psHttp0[j]->lSip)));
                      utPltSetCvtHtml(1);       	            
                        utPltPutLoopVar(psDbHead,"name", iSum+1,psUser->dispname);
                        if(psUser->psGroup)
                           utPltPutLoopVar(psDbHead,"dept", iSum+1,psUser->psGroup->groupname);
                           
                        utPltPutLoopVar(psDbHead,"url",  iSum+1,caUrl);
                        memset(caTitle,0,sizeof(caTitle));
                        cid=0;
                       ncUrlGetUrlInfo(psHttp0[j]->caHost,caTitle,&cid);

                        if(strlen(psHttp0[j]->caTitle)>0)
                          utPltPutLoopVar(psDbHead,"title",iSum+1,psHttp0[j]->caTitle);
                       else
                           utPltPutLoopVar(psDbHead,"title",iSum+1,caTitle);
                           
                           utPltPutLoopVar(psDbHead,"urlclass",iSum+1,ncUrlGetClassNameById (cid,""));
                           
                        utPltPutLoopVar(psDbHead,"urllnk", iSum+1,caUrl0);
  utPltSetCvtHtml(0);            
                        utPltPutLoopVarF(psDbHead,"userid",iSum+1,"%lu",psUser->userid);
            		        utPltPutLoopVar(psDbHead,"position",iSum+1,psUser->position);
                        utPltPutLoopVar(psDbHead,"starttime",iSum+1,utTimFormat("%m-%d %H:%M:%S",psHttp0[j]->lLastTime));
                        
                   
                        
                        utPltPutLoopVarF(psDbHead,"bytes",iSum+1,"%lu",psHttp0[j]->lBytes);
                        if(psHttp0[j]->lDip!=0)
                        {
        									sprintf(caDipPort,"%s:%d",utComHostIp(htonl(psHttp0[j]->lDip)),psHttp0[j]->nDport);
        									utPltPutLoopVar(psDbHead,"dip",iSum+1,caDipPort);
        								}
      
            		    iSum++;
            	
   

    
  }
  
   
     i=0;
    

    while(iSum < iMaxRec) {
        i++;
        iSum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iSum);
    }
 
    utPltPutVar(psDbHead, "way",caWay);
    utPltPutVar(psDbHead, "plate",caPlate);
    utPltPutVar(psDbHead, "filter",caFilter);
    utPltPutVar(psDbHead,"title_name",caTitlename);

  if(psHttp0){
    free(psHttp0);
  }
     utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncnewweb_title.htm");
    return 0;
} 





 int ncSortHttpBufByLastTime(const void *p1,const void *p2)
{
    ncHttpHashBuffer *psHttp1,*psHttp2;
    memcpy(&psHttp1,p1,4);
    memcpy(&psHttp2,p2,4);
    return (psHttp2->lLastTime - psHttp1->lLastTime);
}


/* 将输入的条件转换为一个Get的字符串  */
int ncWebUtlCvtIpStr(utMsgHead *psMsgHead, struct ncWebIpFilter_s *ps)
{
    int iReturn;
    char caGid[16];
    char caSf[2];
    char caSlip[16],caSuip[16],caDf[2],caDlip[16],caDuip[16],caDport[6],caFl[10],caFu[10],caProt[4];
    char caDr[8],caNet[8];
    iReturn = utMsgGetSomeNVar(psMsgHead,13,
            "net",     UT_TYPE_STRING, 6,  caNet,
		    "gid",     UT_TYPE_STRING, 15, caGid,
		    "dr",      UT_TYPE_STRING, 1,  caDr,
		    "sf",      UT_TYPE_STRING, 1,  caSf,
		    "slip",    UT_TYPE_STRING, 15, caSlip,
		    "suip",    UT_TYPE_STRING, 15, caSuip,
		    "df",      UT_TYPE_STRING, 1,  caDf,
		    "dlip",    UT_TYPE_STRING, 15, caDlip,
		    "duip",    UT_TYPE_STRING, 15, caDuip,
		    "dport",   UT_TYPE_STRING, 5,  caDport,
		    "fl",      UT_TYPE_STRING, 8,  caFl,
		    "fu",      UT_TYPE_STRING, 8,  caFu,
		    "prot",    UT_TYPE_STRING, 3,  caProt);
    memset(ps,0,sizeof(struct ncWebIpFilter_s));	    
    if(!utStrIsSpaces(caGid)) {
        ps->gid = atol(caGid);
    }
    else {
        ps->gid = (-1);
     }
    if(!utStrIsSpaces(caSf)) {
        ps->sf = atol(caSf);
    }
    if(!utStrIsSpaces(caSlip)) {
        ps->slip = ntohl(utComHostAddress(caSlip));
    }
    if(!utStrIsSpaces(caSuip)) {
        ps->suip = ntohl(utComHostAddress(caSuip));
    }
    else {
        ps->suip = ntohl(utComHostAddress("255.255.255.255"));
    }
        
    if(!utStrIsSpaces(caDf)) {
        ps->df = atol(caDf);
    }
    if(!utStrIsSpaces(caDlip)) {
        ps->dlip = ntohl(utComHostAddress(caDlip));
    }
    if(!utStrIsSpaces(caDuip)) {
        ps->duip = ntohl(utComHostAddress(caDuip));
    }
    else {
        ps->duip = ntohl(utComHostAddress("255.255.255.255"));
    }
    if(!utStrIsSpaces(caDport)) {
        ps->dport = atol(caDport);
    }
    if(!utStrIsSpaces(caFl)) {
        ps->fl = atol(caFl);
    }
    if(!utStrIsSpaces(caFu)) {
        ps->fu = atol(caFu);
    }
    else {
        ps->fu = 2000000000L;
    }
    if(!utStrIsSpaces(caProt)) {
        ps->prot = atol(caProt);
    }
    if(!utStrIsSpaces(caDr)) {
        ps->dr = atol(caDr);
    }
    else {
        ps->dr = 2;
    }
    if(!utStrIsSpaces(caNet)) {
        ps->net = atol(caNet);
    }
    else {
        ps->net = 0;
    }
    return 0;
}


/* 过滤IP, 返回 0--不要   1--保留  */
int ncWebUtlIpFilter(ncIp  *psIp, struct ncWebIpFilter_s *ps,long lGid,
    struct ncUtlNetIp_s *psNet,int iSumNet)
{
    if(ps->gid >= 0) {
        if(ps->gid != lGid) {
#ifdef LDEBUG
    printf(" gid==%d Gid==%d\n", ps->gid,lGid);
#endif    
            return 0;
        }
    }
    if(ps->sf == 0) {
        if(psIp->lSip > ps->suip || psIp->lSip < ps->slip) {
            return 0;
        }
    }
    else {
        if(psIp->lSip <= ps->suip && psIp->lSip >= ps->slip) {
            return 0;
        }
    }
    if(ps->df == 0) {
        if(psIp->lDip > ps->duip || psIp->lDip < ps->dlip) {
            return 0;
        }
    }
    else {
        if(psIp->lDip <= ps->duip && psIp->lDip >= ps->dlip) {
            return 0;
        }
    }
    if(ps->dport > 0 && psIp->nDport != ps->dport) {
        return 0;
    }
    if(ps->dr > 1) {  /* 全部  */
        if(ps->fl  > (unsigned long)( psIp->lBytes[0] + psIp->lBytes[1])  || ps->fu < (unsigned long)(psIp->lBytes[0] + psIp->lBytes[1])) {
            return 0;
        }
    }
    else {
        if(ps->fl  > (unsigned long) psIp->lBytes[ps->dr]  
                    || ps->fu < (unsigned long)psIp->lBytes[ps->dr]) {        
            return 0;
        }
    }
    if(ps->prot > 0 && ps->prot != psIp->cType) {
        return 0;
    }
    if(ps->net > 0) {
        if(ps->net != ncUtlIpNetIsIn(psIp->lDip,psNet,iSumNet)) {
            return 0;
        }
    }
    return 1;
}


long ncUtlIpNetIsIn(unsigned long lIp,struct ncUtlNetIp_s *psNet,int iSumNet)
{
    int i;
    for(i=0;i<iSumNet;i++) {
        if(lIp >= psNet[i].lLip && lIp <= psNet[i].lUip) {
            return psNet[i].lId;
        }
    }
    return 0;
}



/* 设置过滤条件  */
int ncWebWebSetFilter(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caFilter[256];
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif    
    psDbHead = utPltInitDb();
    ncWebUtlCvtIpGet(psMsgHead,caFilter);
    utPltPutVar(psDbHead,"flags","1");
    utPltPutVar(psDbHead,"filter",caFilter);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_webmon_left.htm");
}

/* 显示过滤设置画面 */
int ncWebWebDispFilterForm(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    
    char caGid[16];
    char caSf[2],caUrl[128];
    long iReturn,lGroupid,lGroupid0;
    char caSmt[256];
    utPltDbHead *psDbHead;
    char *pBuf,*pSelect;
    char caGroupid[12];
#ifdef LDEBUG
    utMsgPrintMsg(psShmHead);
#endif
    psDbHead = utPltInitDb();
    strcpy(caGroupid,"\0");
    iReturn = utMsgGetSomeNVar(psMsgHead,3,
		    "gid",     UT_TYPE_STRING, 15, caGid,
		    "sf",      UT_TYPE_STRING, 1,  caSf,
		    "url",    UT_TYPE_STRING, 127, caUrl);
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn == 0 && lGroupid0 > 0) {  /* 部门权利  */
        sprintf(caSmt,"select groupname,groupid from ncgroup where groupid = %lu order by groupname",
                  lGroupid0);
        pBuf = pasUtlCvtSqlSelectA(caSmt,"\0");
        sprintf(caGroupid,"%lu",lGroupid0);
    }
    else {
        pBuf = pasUtlCvtSqlSelectA("select groupname,groupid from ncgroup order by groupname",
               ncLang("0447全部, ;"));
    }
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("gid",caGid,pBuf);
        free(pBuf);
        utPltPutVar(psDbHead,"sgroup", pSelect);
        free(pSelect);
    }
    pSelect= pasUtlCvtInputSelect("sf",caSf,ncLang("0111包含,;不包含,1;"));
    utPltPutVar(psDbHead,"sf", pSelect);
    free(pSelect);    
    utPltPutVar(psDbHead,"url", caUrl);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_ip_webfilter.htm");
}


/* 将输入的条件转换为一个Get的字符串  */
int ncWebUtlCvtWebStr(utMsgHead *psMsgHead, struct ncWebWebFilter_s *ps)
{
    int iReturn;
    char caGid[16];
    char caSf[2];
    char caUrl[128];


    iReturn = utMsgGetSomeNVar(psMsgHead,3,
		    "gid",     UT_TYPE_STRING, 15, caGid,
		    "sf",      UT_TYPE_STRING, 1,  caSf,
		    "url",    UT_TYPE_STRING, 127, caUrl);
    memset(ps,0,sizeof(struct ncWebWebFilter_s));	    
    if(!utStrIsSpaces(caGid)) {
        ps->gid = atol(caGid);
    }
    else {
        ps->gid = (-1);
     }
    if(!utStrIsSpaces(caSf)) {
        ps->sf = atol(caSf);
    }
    if(!utStrIsSpaces(caUrl)) {
        strcpy(ps->url,caUrl);
    }
    return 0;
}


/* 过滤IP, 返回 0--不要   1--保留  */
int ncWebUtlWebFilter(ncHttpBuffer *psHttp, struct ncWebWebFilter_s *ps,long lGid)
{
    char caUrl[256];
#ifdef LDEBUG
    printf(" gid==%d url=%s  \n",
         ps->gid,ps->url);
#endif    
    if(ps->gid >= 0) {
        if(ps->gid != lGid) {
#ifdef LDEBUG
    printf(" gid==%d Gid==%d\n", ps->gid,lGid);
#endif    
            return 0;
        }
    }
    if(ps->url[0]!=0) {
        char caUrl[256];
        sprintf(caUrl,"%s%s",psHttp->caHost,psHttp->caUrl);
        if(ps->sf == 0) {
            if(!ncMailCtlStrStr(caUrl,ps->url)) {
#ifdef LDEBUG
    printf(" 1url==%s %s\n", caUrl,ps->url);
#endif    
                return 0;
            }
        }
        else {
            if(ncMailCtlStrStr(caUrl,ps->url)) {
#ifdef LDEBUG
    printf(" 2url==%s %s\n", caUrl,ps->url);
#endif    
                return 0;
            }
       }
    }            
    return 1;
}


struct ncUtlNetIp_s *ncUtlGetNetIpDesc(utShmHead *psShmHead,int *iSumNet)
{
    char *pFile;
    char caBuf[256];
    char *p;
    FILE *fp;
    struct ncUtlNetIp_s *psNet;
    unsigned long lBase;
    unsigned long lId;
    char caSip[32],caDip[32],caName[32];
    int iSum,iReturn;
    iSum = 0;
    
    psNet = malloc(sizeof(struct ncUtlNetIp_s) * 64);
    pFile = utComGetVar_s(psShmHead,"NetWorkIpFile");
    if(pFile == NULL) {
       *iSumNet = iSum;
        return psNet;
    }
    fp = fopen(pFile,"r");
    if(fp == NULL) {
       *iSumNet = iSum;
        return psNet;
    }
    p = fgets(caBuf,255,fp);
    while(p&&iSum<64) {
        if(caBuf[0] != '#' && !utStrIsSpaces(caBuf)) {
            iReturn = sscanf(caBuf,"%d %s %s %s %ld",&lId,caName,caSip,caDip,&lBase);
            if(iReturn != 5) {
                fclose(fp);
                *iSumNet = iSum;
                return psNet;
            }
            psNet[iSum].lId = lId;
            psNet[iSum].lLip = ntohl(utComHostAddress(caSip));
            psNet[iSum].lUip = ntohl(utComHostAddress(caDip));
            psNet[iSum].lBase = lBase;
            iSum++;
        }
        p = fgets(caBuf,255,fp);
    }
    fclose(fp);
    *iSumNet = iSum;
    return psNet;
}



/* 显示最近的阻断信息  */
int ncWebShowNewWarn(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn;
    int iSum;
    int  iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIpExcept **psIp;
    char caName[32];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;
    iUserFlags = 0;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
	lGroupid1 = (-1);
    iReturn = utMsgGetSomeNVar(psMsgHead,2,
		"groupid",   UT_TYPE_STRING, 11, caGroupid,
		"userid",    UT_TYPE_STRING, 31, caUserid);
	if(!utStrIsSpaces(caGroupid)) {
		lGroupid1 = atol(caGroupid);
	}
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
    psDbHead = utPltInitDb();
    if(!utStrIsSpaces(caUserid)) {
    	iUserFlags = 1;
    	lUserid = atol(caUserid);
    }
    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPEXCEPT);
    if(lMaxRec <= 0) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncwarn_do.htm");        
        return 0;
    }
    psIp = (ncIpExcept **) malloc(lMaxRec * sizeof(ncIpExcept *));
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }        
    lSumRec = ncWebGetCurIpExcept(psShmHead,psIp,lMaxRec);  /* 从内存中取当前IP地址  */
    iSum = 0;
    iMaxRecord = utComGetVar_ld(psShmHead,"MaxNewIpRec",50);
    for(i=0;i<lSumRec && iSum < iMaxRecord;i++) {
#ifdef LDEBUG
        printf(" Ip==%lu  Status=%d  Flags=%d\n",psIp[i]->lSip,psIp[i]->cStatus,iUserFlags);
#endif        
        if((psIp[i]->lSip > 0)&&(psIp[i]->cStatus>0)) {
            psUserInfo = ncGetUserInfoById(psShmHead,psIp[i]->lUserid);
            if(psUserInfo) {
         	     if( ((iUserFlags == 0 && (lGroupid0 == 0 || psUserInfo->lGid == lGroupid0) ) || (iUserFlags && psUserInfo->lId == lUserid))
				 	    && (lGroupid1 == (-1) || psUserInfo->lGid == lGroupid1)) {
            		      utPltPutLoopVar(psDbHead,"loginname", iSum+1,psUserInfo->psUser->username);
            		      utPltPutLoopVar(psDbHead,"name", iSum+1,psUserInfo->psUser->dispname);
            		      utPltPutLoopVar(psDbHead,"dept", iSum+1,psUserInfo->psUser->psGroup->groupname);
        	              utPltPutLoopVar(psDbHead,"sip",  iSum+1,utComHostIp(htonl(psIp[i]->lSip)));
        	              utPltPutLoopVar(psDbHead,"dip",  iSum+1,
        	                            ncWebUtlDestIp(psIp[i]->lDip,psIp[i]->nDport));
        	              utPltPutLoopVar(psDbHead,"mesg", iSum+1,ncUtlCvtZdyy(psShmHead,psIp[i]));
                          utPltPutLoopVar(psDbHead, "StartTime",iSum+1,
                                    utTimFormat("%m-%d %H:%M:%S",psIp[i]->lLastTime));
        		          iSum++;
                }
            }
            else { /* 无用户信息  */
                 if(utStrIsSpaces(caGroupid) && utStrIsSpaces(caUserid) && lGroupid0 == 0) {
      	              utPltPutLoopVar(psDbHead,"sip",  iSum+1,utComHostIp(htonl(psIp[i]->lSip)));
     	              utPltPutLoopVar(psDbHead,"dip",  iSum+1,
        	                            ncWebUtlDestIp(psIp[i]->lDip,psIp[i]->nDport));
       	              utPltPutLoopVar(psDbHead,"mesg", iSum+1,ncUtlCvtZdyy(psShmHead,psIp[i]));
                      utPltPutLoopVar(psDbHead, "StartTime",iSum+1,
                                utTimFormat("%m-%d %H:%M:%S",psIp[i]->lLastTime));
      		          iSum++;
                }
            
            }
        }
    }
    utPltPutVar(psDbHead, "username",caName);
    free(psIp);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncwarn_do.htm");
    return 0;
}    

char *ncWebUtlDestIp(unsigned long lIp,unsigned short nPort)
{
    static char caIp[32];
    strcpy(caIp,"\0");
    if(lIp != 0) {
        utStrAddF(caIp,"%s",utComHostIp(htonl(lIp)));
    }
    if(nPort != 0) {
        utStrAddF(caIp,":%lu",nPort);
    }
    return caIp;
    
}

    

int ncWebGetCurIpExcept(utShmHead *psShmHead,ncIp **psIp,unsigned long lMax)
{
    char *pHash;
    ncIpExcept *psIp0;
    pasHashInfo sHashInfo;    
    register int i;
    unsigned long  lSum;
    pHash = utShmHashHead(psShmHead,NC_LNK_IPEXCEPT);
    if(pHash == NULL) {
        return (-1);
    }
    psIp0  = (ncIpExcept *)pasHashFirst(pHash,&sHashInfo);
    lSum = 0;
    while(psIp0 && lSum < lMax) {
       psIp[lSum] = psIp0;
       lSum++;
       psIp0 = (ncIp *)pasHashNext(&sHashInfo);
    }
    qsort(psIp,lSum,sizeof(ncIpExcept *),ncWebIpExceptSort);
    return lSum;
}

static int ncWebIpExceptSort(char *p1,char *p2)
{
    ncIpExcept *psIp1,*psIp2;
    memcpy(&psIp1,p1,4);
    memcpy(&psIp2,p2,4);    
    return (psIp2->lLastTime - psIp1->lLastTime);
}
   
                
int ncWebWait(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caMessage[128];
    char caFname[64];
    char caOpt[12];
    char caPlate[128];
    utPltDbHead *psDbHead;
    iReturn = utMsgGetSomeNVar(psMsgHead,4,
		"fname",     UT_TYPE_STRING, 63, caFname,
		"message",   UT_TYPE_STRING, 127,caMessage,
		"opt",       UT_TYPE_STRING, 10, caOpt,
		"plate",     UT_TYPE_STRING, 127,caPlate);
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"fname",caFname);        
    utPltPutVar(psDbHead,"message",caMessage);
    utPltPutVar(psDbHead,"opt",caOpt);
    if(utStrIsSpaces(caPlate)) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncmsg_goto.htm");
    }
    else {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    }
    return 0;

}

int ncWebPronetwaylog12345(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caMessage[1024];
    char caTitle[256];
    char caMessage2[128];
    utPltDbHead *psDbHead;
  
    psDbHead = utPltInitDb();
    strcpy(caTitle,ncLang("1457网络督察——网络行为专家"));
    strcpy(caMessage,ncLang("1458本产品由上海新网程信息技术有限公司研发<BR>2005年9月"));
    utPltPutVar(psDbHead,"title",caTitle);  
    utPltPutVar(psDbHead,"message",caMessage);
   
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncmsg.htm");
 
    return 0;

}

int ncWebTest(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caMessage[1024];
    char caTitle[256];
    char caMessage2[128];
    utPltDbHead *psDbHead;
utMsgPrintMsg(psMsgHead);
    psDbHead = utPltInitDb();
    strcpy(caTitle,ncLang("1457网络督察——网络行为专家"));
    strcpy(caMessage,ncLang("1458本产品由上海新网程信息技术有限公司研发<BR>2005年9月"));
    utPltPutVar(psDbHead,"title",caTitle);
    utPltPutVar(psDbHead,"message",caMessage);

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncmsg.htm");

    return 0;

}

int ncWebTestDownLoad(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caMessage[1024];
    char caTitle[256];
    char caMessage2[128];
    utPltDbHead *psDbHead;
utMsgPrintMsg(psMsgHead);
    utPltFileDownload(iFd,"application/gzip","/home/ncmysql/ncc/bin","nc.tgz","nc.tgz");
    return 0;

}

//显示即时过滤条件设置
int ncWebOnlineSearch(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    long iNum,iReturn;
    char caFlag[8];
    char caTemp[1024];
    unsigned long id,userid;
    char caName[64];
    char caDispname[64];
    unsigned long urlid;
    char urlname[64];
    ncUserContHead  *psHead;
    ncUserCont      *psUser,*psUser0;
    int i;
    long group_flag,lGroupid0;
    pasHashInfo sHashInfo;
    pasDbCursor *psCur;
    
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if((iReturn==0)&&(lGroupid0>0)){
    	group_flag=1;
    }
    else{
    	group_flag=0;
    }
    
     psDbHead = utPltInitDb();
	iReturn = utMsgGetSomeNVar(psMsgHead,1,
			            "flag",       UT_TYPE_STRING, 6, caFlag);

 //用户
    psHead = (ncUserContHead *)utShmArray(psShmHead,NC_LNK_ALLUSER);
    if(psHead == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("0403内存不正确"));
         return 0;
    }
    psUser0 = (ncUserCont *)((char *)psHead + psHead->lContPos); 
   
     for(i=0,iNum=1;i<psHead->lSumUser && iNum<500;i++,iNum++) {
      psUser = &psUser0[i];
      utPltPutLoopVarF(psDbHead,"num",iNum,"%lu",iNum-1);
      utPltPutLoopVarF(psDbHead,"uid",iNum,"%lu",psUser->userid);
      
        utPltSetCvtHtml(1);
      utPltPutLoopVarF(psDbHead,"uname",iNum,"%s",psUser->dispname);
        utPltSetCvtHtml(0);
    
  }
    

      //部门

      strcpy(caTemp,"select groupid,groupname from ncgroup  ");
      if(group_flag==1){
      	sprintf(caTemp+strlen(caTemp)," where (groupid=%d or pid=%d )",lGroupid0,lGroupid0);
      }
	    psCur = pasDbOpenSql(caTemp,0);
	    if(psCur==NULL){
	    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("1491数据库没准备好,稍候再试"));
	    	return 0;
	    }
	    iReturn = pasDbFetchInto(psCur,
	      	                               UT_TYPE_LONG,4,&id,
	      	                               UT_TYPE_STRING,30,caName);
	    iNum = 0; 	                               
	    while((iReturn==0)||(iReturn==1405))
	    {
	  	
	        iNum++;       
	        utPltPutLoopVarF(psDbHead,"gid",   iNum,"%lu",id);
	        utPltPutLoopVar(psDbHead,"gname",iNum,caName);
	        iReturn = pasDbFetchInto(psCur,
	      	                               UT_TYPE_LONG,4,&id,
	      	                               UT_TYPE_STRING,30,caName);
	    }
	    pasDbCloseCursor(psCur);
	      
	     //网络服务
	     psCur = pasDbOpenSql("select id,name from ncipindex   ",0);
	    iReturn = pasDbFetchInto(psCur,
	      	                               UT_TYPE_LONG,4,&userid,
	      	                               UT_TYPE_STRING,63,caDispname);
	    iNum = 0; 	                               
	    while((iReturn==0)||(iReturn==1405))
	    {
	  	
	        iNum++;       
	        utPltPutLoopVarF(psDbHead,"id",   iNum,"%lu",userid);
	        utPltPutLoopVar(psDbHead,"name",iNum,caDispname);
	        iReturn = pasDbFetchInto(psCur,
	      	                               UT_TYPE_LONG,4,&userid,
	      	                               UT_TYPE_STRING,63,caDispname);
	    }
	    pasDbCloseCursor(psCur);
	    
	    if(atol(caFlag)==1){
	    	   //输出网络服务大类
			long lCgid;
			char caName[32];
			     	psCur = pasDbOpenSqlF("select groupid,name  from ncservicecgroup   order by groupid ");
			                    
				    if(psCur == NULL) {
				        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
				        return 0;
				    }
			     
			    lCgid=0;
			    memset(caName,0,sizeof(caName));
			    iNum=0;
				    iReturn = pasDbFetchInto(psCur,     	                           
				      	                           UT_TYPE_LONG,4,&lCgid, 	                           
				  	                               UT_TYPE_STRING,31,caName);
				  	                             
			   	
			   	while(iReturn == 0 ||iReturn==1405) {
			   		
			    utPltPutLoopVarF(psDbHead,"cgid",iNum+1,"%d",lCgid);
			    utPltPutLoopVar(psDbHead,"cgname",iNum+1,caName);
			   	
			   	iNum++;
			   		
			    lCgid=0;
			    memset(caName,0,sizeof(caName));
				    iReturn = pasDbFetchInto(psCur,     	                           
				      	                           UT_TYPE_LONG,4,&lCgid, 	                           
				  	                               UT_TYPE_STRING,31,caName);
			        }  
			    pasDbCloseCursor(psCur); 
	    	
      	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/ncs_iponline_query.htm");
      }
	    else if(atol(caFlag)==2)
      	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/ncs_useronline_query.htm");
      else if(atol(caFlag)==3){
      	
     	psCur = pasDbOpenSql("select id,name from ncwebclass order by name ",0);
    	iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&urlid,
    	                               UT_TYPE_STRING,63,urlname);
    	iNum = 0; 	                               
    	while((iReturn==0)||(iReturn==1405))
    	{
  	
        	iNum++;       
        	utPltPutLoopVarF(psDbHead,"urlid",   iNum,"%lu",urlid);
        	utPltPutLoopVar(psDbHead,"urlname",iNum,urlname);
    			iReturn = pasDbFetchInto(psCur,
      	                               UT_TYPE_LONG,4,&urlid,
    	                               UT_TYPE_STRING,63,urlname);
    	}
    		pasDbCloseCursor(psCur);
      	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/ncs_webwebonline_query.htm");
      }
      else if(atol(caFlag)==4)
      	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/ncs_formonline_query.htm");
      else if(atol(caFlag)==5)
      	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/ncs_mailonline_query.htm");	
      else if(atol(caFlag)==6){
      	long lSid;
      	//输出即时通讯服务
		    psCur = pasDbOpenSqlF("select sid,name from ncipindex where groupid=1 order by name ");
		          
		    if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
		        return 0;
		    }
		  
		   lSid=0;
		   memset(caName,0,sizeof(caName));
		    iReturn = pasDbFetchInto(psCur,
		      	                           UT_TYPE_LONG,4,&lSid,
		  	                               UT_TYPE_STRING,31,caName);
		  	                              
		  	                              
		   	iNum = 0;
		   	while(iReturn == 0 ||iReturn==1405) {
		        iNum++;
		       
		        utPltPutLoopVarF(psDbHead,"msid",   iNum,"%lu",lSid);
		        utPltPutLoopVar(psDbHead, "mname",  iNum,caName);
		    
		      
		    lSid=0;
		    memset(caName,0,sizeof(caName));
		    iReturn = pasDbFetchInto(psCur,
		      	                           UT_TYPE_LONG,4,&lSid,
		  	                               UT_TYPE_STRING,31,caName);
		    }
		    pasDbCloseCursor(psCur);
      	
      	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/ncs_imonline_query.htm");
      }
      else if(atol(caFlag)==7)
      	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/ncs_ftponline_query.htm");
      else if(atol(caFlag)==8)
      	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/ncs_erronline_query.htm");
      else if(atol(caFlag)==9)
      	utPltOutToHtml(iFd,psMsgHead,psDbHead,"ncs/ncs_warnonline_query.htm");
      return 0;
}
 
//显示某个用户即时ip信息
 
int ncWebShowUseronlineip(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
    ncHttpBuffer *psHttp;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;
    struct ncWebIpFilter_s s;
    struct ncUtlNetIp_s *psNet;
    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[16];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
    int service_flag;
    char caTemp[4024];
  
   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
   	
   struct ncSflow_s  sGservice[500];
   long lSumGindex;
   long lGroupid,lSid,index;
   	
    pasDbCursor *psCur;
    char caFlag[12];
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

    iReturn = utMsgGetSomeNVar(psMsgHead,4,
		"flag",   UT_TYPE_STRING, 2, caFlag,
		"userid",    UT_TYPE_STRING, 31, caUserid,
		"service",       UT_TYPE_STRING, 10, caService,
		"CurPg",   UT_TYPE_STRING, 12, caCurpg);
	
   //如果是大类
   
	if(strcmp(caFlag,"0")==0){
		 //将服务信息放在sGservice 中
   	 psCur=pasDbOpenSql("select sid,groupid,name from ncipindex order by sid  limit 0,500 ",0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
     }
     lSumGindex=0;
     
     lGroupid=0;
     memset(caName,0,sizeof(caName));
     lSid=0;
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
     																UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
          
      while((iReturn==0)||(iReturn==1405))
      {
      	sGservice[lSumGindex].lService=lSid;
      	sGservice[lSumGindex].lGroupid=lGroupid;
      	strcpy(sGservice[lSumGindex].caGname,caName);
      	sGservice[lSumGindex].lBytes=0;

      	lSumGindex++;
      	lGroupid=0;
      	lSid=0;
        memset(caName,0,sizeof(caName));
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
     																UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      }                               
      pasDbCloseCursor(psCur); 
	 }




 

    iOrderip=6;
   if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,atol(caUserid),"\0"));
    }   
    else{
    	strcpy(caTitlename,"");
    }	                          
    
   
    iCount = 0;
    if(!utStrIsSpaces(caUserid)) {
    	iUserFlags = 1;
    	lUserid = atol(caUserid);
    }
    psHttp = (ncHttpBuffer *)utShmArray((utShmHead *)psShmHead,NC_LNK_WEB);
    if(psHttp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg1.htm","ncWebShowIp","Internal Error1 ");
        return 0;
    }

    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
    if(lMaxRec <= 0) {
        return 0;
    }
   if(lMaxRec>200000)
    lMaxRec=200000;
    
    psDbHead = utPltInitDb();
    psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
    
   
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }        
    lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    
    if(lSumRec>200000)
    lSumRec=200000;
    
     psIp1 = (ncIp **) malloc((lSumRec+2) * sizeof(ncIp *));
    
    iSum = 0;
    iMaxRecord = utComGetVar_ld(psShmHead,"MaxNewIpRec",30);
  

   if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * iMaxRecord;



    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&(psIp[i]->cStatus!=NC_USER_NOMON)) {
            psUserInfo = psIp[i]->psUser;
            if((psUserInfo)&&(psUserInfo->lId == lUserid)) {
            	if(strcmp(caFlag,"0")==0){
            		index=ncUtlGetServicenumById(&sGservice,lSumGindex,psIp[i]->lService);
        	 			if(index!=-1){
        	 				if(atol(caService)==sGservice[index].lGroupid){
        	 	 					psIp1[iSum]=psIp[i];
        		      		iSum++;
        		      	}
        	 				}
            		
            	}
            	else{
         	     if(((strlen(caService)==0)||(strlen(caService)>0&&atol(caService)==psIp[i]->lService))){
         	    
        		      		psIp1[iSum]=psIp[i];
        		      		iSum++;
                           
             		  }
                 }
                }
            }
        }
   

        if(iOrderip > 0) {
        qsort(psIp1, iSum,sizeof(ncIp *),ncUtlSortOnIp1);
    }
    
    
  

    
    
      lTotpg = (iSum - 1) / iMaxRecord + 1; 
      utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
      utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
      utPltPutVarF(psDbHead,"TotRec","%d",iSum);
     
     lTime=time(0);
     iNum=0;
      for(i = lStartRec; i < iSum && iNum < iMaxRecord;i++) {
      	psUserInfo = psIp1[i]->psUser;
      	if(psUserInfo){
      	            	utPltPutLoopVar(psDbHead,"loginname", iNum+1,psUserInfo->psUser->username);
            		      utPltPutLoopVar(psDbHead,"name", iNum+1,psUserInfo->psUser->dispname);
            		      
            		 
            		      utPltPutLoopVarF(psDbHead,"userid",iNum+1,"%lu",psUserInfo->psUser->userid);
            		      utPltPutLoopVar(psDbHead,"position",iNum+1,psUserInfo->psUser->position);
            		      if(psUserInfo->psUser->psGroup){
            		        utPltPutLoopVar(psDbHead,"dept", iNum+1,psUserInfo->psUser->psGroup->groupname);
            		      }
        	              utPltPutLoopVar(psDbHead,"sip",  iNum+1,utComHostIp(htonl(psIp1[i]->lSip)));
                          utPltPutLoopVar(psDbHead,"dip",  iNum+1,utComHostIp(htonl(psIp1[i]->lDip)));
                          utPltPutLoopVarF(psDbHead,"port", iNum+1,"%d",psIp1[i]->nDport);
                          utPltPutLoopVar(psDbHead,"service", iNum+1,ncUtlGetServiceName(psShmHead,psIp1[i]->lService,"\0"));
                          utPltPutLoopVarF(psDbHead,"id",iNum+1,"%d",psIp1[i]->lService);
                          utPltPutLoopVar(psDbHead, "lasttime",iNum+1,
                          utTimFormat("%H:%M:%S",psIp1[i]->lLastTime));
                          
                          utPltPutLoopVar(psDbHead, "starttime",iNum+1,
                          utTimFormat("%H:%M:%S",psIp1[i]->lStartTime));
                          
                          
                          utPltPutLoopVarF(psDbHead, "ConnTime",iNum+1,"%d",psIp1[i]->lLastTime - psIp1[i]->lStartTime);
                          utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(psIp1[i]->lBytes[1],caTemp));
                          utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(psIp1[i]->lBytes[0],caTemp));
                          

                        
                          if(psIp1[i]->lStartTime > 0 && (lTime - psIp1[i]->lStartTime > 30)) {
                             lBand = 8*(psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) / (lTime - psIp1[i]->lStartTime);
                            }
                           else {
                              lBand = 8*(psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) / 30;
                              }
                               
                            utPltPutLoopVarF(psDbHead, "band",iNum+1,"%s",utStrLtoF8(lBand,caTemp));
                          
                          
                          
         iNum++;
       }
    }
     i=0;
    while(iNum < iMaxRecord) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
    lRate = lSumBand / 30;
    utPltPutVar(psDbHead, "username",caName);
   
    free(psIp);
    free(psIp1);
   
    utPltPutVar(psDbHead,"title_name",caTitlename);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncuservisterip.htm");
    return 0;
}  


 static int iSort,iOrder;
  struct serviceonline_s{
  	  unsigned long sip;
 		  unsigned long id;
 		  long long inbyte;
 		  long long outbyte;
 		  unsigned long lasttime;
 		  long long band;
 	  };
static int ncUtlSortOnService2(struct serviceonline_s *p1,struct serviceonline_s *p2)
{
        long long l;
        l=(p2->inbyte+p2->outbyte)-(p1->inbyte+p1->outbyte);
        if(l>0) return 1;
        else if(l<0) return (-1);
        else return 0;
                  
}

static int ncUtlSortOnService3(struct serviceonline_s *p1,struct serviceonline_s *p2)
{
	     
	      long long l;
	      
	      
	      struct serviceonline_s *PT;
	      if(iOrder==0){
	      	PT=p1;
	      	p1=p2;
	      	p2=PT;
	      }
	      
	    
	         switch(iSort) {
            case 1:
            case 2: 
            case 3: 
            case 4: return p1->id- p2->id;
            case 5: return p1->lasttime - p2->lasttime;
            case 6:        
					        l=(p1->inbyte+p1->outbyte)-(p2->inbyte+p2->outbyte);
					        if(l>0) return 1;
					        else if(l<0) return (-1);
					        else return 0;
            case 8:
            	    l=(p1->inbyte)-(p2->inbyte);
					        if(l>0) return 1;
					        else if(l<0) return (-1);
					        else return 0; 
					  case 7:
					       l=(p1->outbyte)-(p2->outbyte);
					        if(l>0) return 1;
					        else if(l<0) return (-1);
					        else return 0; 
					  case 9:
					  		 l=(p1->band)-(p2->band);
					        if(l>0) return 1;
					        else if(l<0) return (-1);
					        else return 0;
					  default: return 1;
					  }
					  return 1;
}




int ncUtlLoolFlowByUserId(struct serviceonline_s *arrIn,int iSumArr,unsigned long lUserid)
{
   int bottom,middle,top;
    int n;
    top = iSumArr - 1;
    bottom = 0;
    while(top >= bottom) {
        middle = (top + bottom) / 2;   
        if(arrIn[middle].id==lUserid) {
            return middle;
        }
        else if(arrIn[middle].id>lUserid) {
            top = middle -1;
        }
        else {
            bottom = middle + 1;
        }
    }
    return (-1);
}



//统计即时网络服务流量
 
int ncWebOnlieServiceStat(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
    ncHttpBuffer *psHttp;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;

    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[16];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
    int service_flag;
    char caTemp[4024];
  
   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
    pasDbCursor *psCur;
    char caFlag[12];
    long long lSumbytes;
 	  long sflag,m;
 	  float rate;
 	  long lBsteptime;
 	  float fRate;
 	  char caPdf[12];    
   struct piechart_datasetf *pChart;           //用于PDF文档输出
//   char *desc[]={"用户名","部门","流量","百分比"};
    int rowrow;                     
    int column; 
//    char **pTemp;
    char caTitle[128];
    char filename[128];
//    int m;
    column=4;
    long mm;
//    long lDate;
    char **desc;
    char caStatdate[128];
 	  
 	  lBsteptime=atol(utComGetVar_sd(psShmHead,"BandSteptime","15"));
 	  
	 struct serviceonline_s *stService;
   int lSumservice;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

    iReturn = utMsgGetSomeNVar(psMsgHead,5,
		"flag",   UT_TYPE_STRING, 2, caFlag,
		"userid",    UT_TYPE_STRING, 31, caUserid,
		"service",       UT_TYPE_STRING, 10, caService,
		"CurPg",   UT_TYPE_STRING, 12, caCurpg,
		"pdf",UT_TYPE_STRING,8,caPdf);
	
   lSumbytes=0;
    
   lSumservice=0;

 
    iCount = 0;
 

    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
    if(lMaxRec <= 0) {
    	 psDbHead = utPltInitDb();
    	 utPltPutVarF(psDbHead,"CurPg","%d",1);
       utPltPutVarF(psDbHead,"TotPg","%d",1);
       utPltPutVarF(psDbHead,"TotRec","%d",0);
       
        iSum=0;
    	  i=0;
         
       
       
       
    if(atol(caFlag)==1)
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_servicestatbt.htm");
    else
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncuserviceonlinestat.htm");
    
       
        return 0;
    }
   if(lMaxRec>200000)
    lMaxRec=200000;
    
    psDbHead = utPltInitDb();
    psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
    
   
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }        
    lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    
    if(lSumRec>200000)
    lSumRec=200000;
    
    
    iSum = 0;
    iMaxRecord = utComGetVar_ld(psShmHead,"MaxNewIpRec",30);
  

   if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * iMaxRecord;


   stService=malloc(300*sizeof(struct serviceonline_s));
   if(stService==NULL){
   	    
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),ncLang("0404内存出错"));
           return 0;
       
   }
   memset(stService,0,300*sizeof(struct serviceonline_s));
   lTime=time(0);
    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)) {
           
         sflag=0;
         for(m=0;m<lSumservice;m++){
         	   if(psIp[i]->lService==stService[m].id){
         	      sflag=1;
         	      stService[m].inbyte=stService[m].inbyte+psIp[i]->lBytes[1];
         	      stService[m].outbyte=stService[m].outbyte+psIp[i]->lBytes[0];
         	     
         	    if(psIp[i]->lStartTime > 0&&psIp[i]->lLastTime>0 && (lTime - psIp[i]->lStartTime > lBsteptime)) {
         	    	if((lBsteptime+psIp[i]->lLastTime>lTime)&&(psIp[i]->lLastTime>psIp[i]->lStartTime)){
         	    		if(lBsteptime+psIp[i]->lLastTime>lTime){
         	    			
//         	    			printf("m=%d,lastime=%d,lCurtime=%d,lStime=%d,byte=%llu,sid=%d\n",m,psIp[i]->lLastTime,lTime,psIp[i]->lStartTime,psIp[i]->lBytes[0] + psIp[i]->lBytes[1],psIp[i]->lService);
//         	          printf("lBsteptime=%d\n",lBsteptime);
         	    			
         	    		   fRate=(float)(lBsteptime+psIp[i]->lLastTime-lTime)/lBsteptime;
         	    		  }
         	    		  else{
         	    		  	fRate=0;
         	    		  }
        // 	    		printf("fRate=%f\n",fRate);
                   stService[m].band = stService[m].band+(psIp[i]->lBytes[0] + psIp[i]->lBytes[1])*fRate/(psIp[i]->lLastTime - psIp[i]->lStartTime);
                  }
                  else{
   //               	stService[m].band= stService[m].band+(psIp[i]->lBytes[0] + psIp[i]->lBytes[1]) /lBsteptime;
                  }
               
              }
             	else {
                 stService[m].band= stService[m].band+(psIp[i]->lBytes[0] + psIp[i]->lBytes[1]) /lBsteptime;
             	}
         	     
         	     
        
         	      
         	      
        		    break;
        		  }
         }
         if((sflag==0)&&(lSumservice<300)){
         	       		 
        		 stService[lSumservice].id=psIp[i]->lService;
        		 stService[lSumservice].inbyte=psIp[i]->lBytes[1];
         	   stService[lSumservice].outbyte=psIp[i]->lBytes[0];
        
    //	printf("mn=%d,lastime=%d,lCurtime=%d,lStime=%d,types=%llu,sid=%d\n",lSumservice,psIp[i]->lLastTime,lTime,psIp[i]->lStartTime,psIp[i]->lBytes[0] + psIp[i]->lBytes[1],psIp[i]->lService);    
         	    if(psIp[i]->lStartTime > 0&&psIp[i]->lLastTime>0 && (lTime - psIp[i]->lStartTime > lBsteptime)) {
         	    	if((lBsteptime+psIp[i]->lLastTime>lTime)&&psIp[i]->lLastTime>psIp[i]->lStartTime){
         	    	   if(lBsteptime+psIp[i]->lLastTime>lTime){
         	    		fRate=(float)(lBsteptime+psIp[i]->lLastTime-lTime)/lBsteptime; 
         	    	}
         	    	else{
         	    		fRate=0;
         	    	}   
      //   	    		printf("fRate=%f\n",fRate);
                   stService[lSumservice].band = (psIp[i]->lBytes[0] + psIp[i]->lBytes[1])*fRate/(psIp[i]->lLastTime - psIp[i]->lStartTime);
         	        }
 //               else
 //                 stService[lSumservice].band=(psIp[i]->lBytes[0] + psIp[i]->lBytes[1]) /lBsteptime;
              }
             	else {
                 stService[lSumservice].band=(psIp[i]->lBytes[0] + psIp[i]->lBytes[1]) /lBsteptime;
             	} 
            
         	   lSumservice++;
        }
                           
       }
        
              
           
     }
   

     if(lSumservice>0)
         qsort(stService,lSumservice,sizeof(struct serviceonline_s),ncUtlSortOnService2);

    
    
  

   
    
      lTotpg = (lSumservice - 1) / iMaxRecord + 1; 
      utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
      utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
     
     
   
     iNum=0;
      for(i = lStartRec; i < lSumservice && iNum < iMaxRecord;i++) {
 

        
      	strcpy(caService,ncUtlGetServiceName(psShmHead,stService[i].id,"\0"));
      	if(strlen(caService)>0){      		
       lSumbytes=lSumbytes+stService[i].outbyte+stService[i].inbyte;		
       utPltPutLoopVarF(psDbHead,"id",iNum+1,"%d",stService[i].id);
        utPltPutLoopVar(psDbHead,"service", iNum+1,ncUtlGetServiceName(psShmHead,stService[i].id,"\0"));
     
        utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(stService[i].inbyte,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte,caTemp));
        
        utPltPutLoopVarF(psDbHead, "subytes2",iNum+1,"%llu",stService[i].outbyte+stService[i].inbyte); 
        utPltPutLoopVarF(psDbHead, "subytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte+stService[i].inbyte,caTemp));  
              
        utPltPutLoopVarF(psDbHead, "band",iNum+1,"%s",utStrLtoF8(stService[i].band*8,caTemp));               
        utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);  
 
        
       iNum++;
     }
    }
     utPltPutVarF(psDbHead,"TotRec","%d",iNum);
     i=0;
     
     
     
     
   if(strlen(caPdf)>0){
	//表格标签
	 desc=malloc(6*sizeof(char*));
	 for(i=0;i<6;i++){
	 	desc[i]=malloc(32);
   }
   strcpy(desc[0],ncLang("1141网络服务名"));
   strcpy(desc[1],ncLang("1876进流量(字节)"));
   strcpy(desc[2],ncLang("1877出流量(字节)"));
   sprintf(desc[4],"%s(%s)",ncLang("1103总流量"),ncLang("0882字节"));
   strcpy(desc[3],ncLang("1821带宽(bps)"));
   strcpy(desc[5],ncLang("1827百分比%"));
     
 //输出到PDF中     
   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*(iNum+2));
    if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0404内存出错"));
      return 0;
   }
  
 
   	mm=0;
    for(i = 0; i < lSumservice && i< iMaxRecord;i++) {
   	   strcpy(caService,ncUtlGetServiceName(psShmHead,stService[i].id,"\0"));
      	if(strlen(caService)>0){  

   	 	
   	 			pChart[mm].name=strdup(caService);  
   	 			
   	 			if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(stService[i].outbyte+stService[i].inbyte)/lSumbytes);
   	 			}
 	 		
   	 			pChart[mm].percent=strdup(caTemp); 
   
   	 			pChart[mm].amount=(float)(stService[i].outbyte+stService[i].inbyte); 	
  			 
          pChart[mm].append=malloc(3*sizeof(char*));
          for(m=0;m<3;m++){
          	pChart[mm].append[m]=malloc(32);
          }
          strcpy(pChart[mm].append[0],utStrLtoF8(stService[i].inbyte,caTemp));
          strcpy(pChart[mm].append[1],utStrLtoF8(stService[i].outbyte,caTemp));
   	 		  strcpy(pChart[mm].append[2],utStrLtoF8(stService[i].band*8,caTemp));   
  	 		
        mm++;
        
      }
        
        
        
     
    }
     

     
     
     
     
     
      rowrow=mm;
 //  printf("rowrow=%d\n",rowrow);
    strcpy(caTitle,ncLang("1878实时网络服务流量"));  
    sprintf(filename,"%s.PDF",caTitle); 
    sprintf(caStatdate,ncLang("1879统计时间：%s"),	utTimFormat("%Y-%m-%d %H:%M:%S",time(0)));	

     ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,6,3, caTitle,ncLang("1880单位(字节)"),caStatdate,filename); 
    
//    printf("aaaaa\n"); 
//释放内存     
   for(i=0;i<rowrow;i++){
	      free(pChart[i].name);
	      free(pChart[i].percent);
	      for(m=0;m<3;m++){
	      	free(pChart[i].append[m]);
	      }
	      if(pChart[i].append) free(pChart[i].append);
   }  
   if(pChart) free(pChart);
   
   for(i=0;i<6;i++){
   	   free(desc[i]);
   }
    if(desc) free(desc);
  
   
   
   
   
   
     utPltFreeDb(psDbHead);
     free(psIp);
     free(stService);
     return 0;
    }  
     
     
     
     
     
     
 //   printf("88888888caFlag=%s\n",caFlag); 
     
     
     
     
    free(psIp);
    free(stService);
    
    if(atol(caFlag)==1)
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_servicestatbt.htm");
    else
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncuserviceonlinestat.htm");
    
    return 0;
}

/*
//统计每个用户即时网络服务流量
 
int ncWebOnlieServiceUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
    int pre_id;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;

    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[16];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
   
    char caTemp[4024];

   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
    pasDbCursor *psCur;
    char caFlag[12];
    char caIp[20];
 	  long lSum,service_flag,m,lService;
 	  char caId[20];
	 struct serviceonline_s *stService;
   int lSumUser;
   struct ncSflow_s  sGservice[600];
   long lSumGindex;
   float fRate;
   long lBsteptime;
   long lGroupid,lSid,index;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);   
    if(iReturn != 0) lGroupid0 = 0;



  lBsteptime=atol(utComGetVar_sd(psShmHead,"BandSteptime","15"));

    lSumUser=0;
    iReturn = utMsgGetSomeNVar(psMsgHead,3,
		"service",       UT_TYPE_STRING, 10, caService,
		"CurPg",   UT_TYPE_STRING, 12, caCurpg,
		"flag",    UT_TYPE_STRING,6,   caFlag);
	
	if(atol(caFlag)==0){
		 //将服务信息放在sGservice 中
   	 psCur=pasDbOpenSql("select sid,groupid,name from ncipindex order by sid  limit 0,600 ",0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
     }
     lSumGindex=0;
     
     lGroupid=0;
     memset(caName,0,sizeof(caName));
     lSid=0;
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
     																UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
          
      while((iReturn==0)||(iReturn==1405))
      {
      	sGservice[lSumGindex].lService=lSid;
      	sGservice[lSumGindex].lGroupid=lGroupid;
      	strcpy(sGservice[lSumGindex].caGname,caName);
      	sGservice[lSumGindex].lBytes=0;

      	lSumGindex++;
      	lGroupid=0;
      	lSid=0;
        memset(caName,0,sizeof(caName));
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
     																UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      }                               
      pasDbCloseCursor(psCur); 
	 }
	
		
	 lService=atol(caService);
   lSumUser = utShmHashRecord(psShmHead,NC_LNK_USERINFO);
    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
    if(lMaxRec <= 0) {
    	 psDbHead = utPltInitDb();
    	 
    	  iSum=0;
    	  i=0;
        while(iSum < 30) {
	        i++;
	        iSum++;
	        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iSum);
	    	}       
    	 
    	 utPltPutVarF(psDbHead,"CurPg","%d",1);
       utPltPutVarF(psDbHead,"TotPg","%d",1);
       utPltPutVarF(psDbHead,"TotRec","%d",0);
       utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncuserviceonlineuser.htm");
    
        return 0;
    }
   if(lMaxRec>200000)
    lMaxRec=200000;
    

    psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
    
   
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }   
    
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"flag",caFlag);
    lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    
    if(lSumRec>200000)
    lSumRec=200000;
    
    psIp1 = (ncIp **) malloc((lSumRec+2) * sizeof(ncIp *));
    iSum = 0;
    iSum = 0;
    iMaxRecord = utComGetVar_ld(psShmHead,"MaxNewIpRec",30);
  

   if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * iMaxRecord;


   stService=malloc((lSumUser+1)*sizeof(struct serviceonline_s));
   if(stService==NULL){
   	    
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),ncLang("0404内存出错"));
           return 0;
       
   }
   memset(stService,0,(lSumUser+1)*sizeof(struct serviceonline_s));
   lSum=0;
   iSum=0;
    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&(psIp[i]->cStatus!=NC_USER_NOMON)&&(psIp[i]->psUser)) {   
        sprintf(caId,"%d",psIp[i]->lService);
        
        service_flag=0;
        if(atol(caFlag)==0){
        	 index=ncUtlGetServicenumById(&sGservice,lSumGindex,psIp[i]->lService);
        	 if(index!=-1){
        	 	if(lService==sGservice[index].lGroupid)
        	 	 service_flag=1;
        	 }
        }       
        else {
          	if(psIp[i]->lService==lService){
          		service_flag=1;
          	}
          }
          
      
          if(service_flag==1){         	
                 psIp1[iSum]=psIp[i];
        		     iSum++;
            }
          }
       }
       iOrderip=10;
       qsort(psIp1, iSum,sizeof(ncIp *),ncUtlSortOnIp1); 
       
               pre_id=-1;
        lSum=0;
        lTime=time(0);
        for(i=0;i<iSum;i++){
        	if(pre_id!=psIp1[i]->psUser->lId){
        		stService[lSum].id=psIp1[i]->psUser->lId;
        		stService[lSum].inbyte=psIp1[i]->lBytes[1];
            stService[lSum].outbyte=psIp1[i]->lBytes[0];
            stService[lSum].lasttime=psIp1[i]->lLastTime;
    //      printf("id=%d,lastime=%d,lCurtime=%d,lStime=%d,byte=%llu,sid=%d\n",psIp1[i]->psUser->lId,psIp1[i]->lLastTime,lTime,psIp1[i]->lStartTime,psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1],psIp1[i]->lService);                 	
            if(psIp1[i]->lStartTime > 0 &&psIp1[i]->lLastTime>0&& (lTime - psIp1[i]->lStartTime > lBsteptime)) {
            	    if((lBsteptime+psIp1[i]->lLastTime>lTime)&&(psIp1[i]->lLastTime>psIp1[i]->lStartTime)){
         	    		if(lBsteptime+psIp1[i]->lLastTime>lTime){
         	    			
         	    		
         	    			
         	    		   fRate=(float)(lBsteptime+psIp1[i]->lLastTime-lTime)/lBsteptime;
         	    		  }
         	    		  else{
         	    		  	fRate=0;
         	    		  }
 
 //        	    		printf("1fRate=%f\n",fRate);
                   stService[lSum].band = (psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1])*fRate/(psIp1[i]->lLastTime - psIp1[i]->lStartTime);
                  }
                  else{
   //               	stService[lSum].band= (psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) /lBsteptime;
                  }
            }
             else {
                  stService[lSum].band= (psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) /lBsteptime;
            }
            pre_id=psIp1[i]->psUser->lId;
            lSum++;
        	} 
        	else{

        		stService[lSum-1].inbyte=stService[lSum-1].inbyte+psIp1[i]->lBytes[1];
            stService[lSum-1].outbyte=stService[lSum-1].outbyte+psIp1[i]->lBytes[0];
           	if(stService[lSum-1].lasttime<psIp1[i]->lLastTime)
              	   	stService[lSum-1].lasttime=psIp1[i]->lLastTime;
              	   	
  //           printf("2id=%d,lastime=%d,lCurtime=%d,lStime=%d,byte=%llu,sid=%d\n",psIp1[i]->psUser->lId,psIp1[i]->lLastTime,lTime,psIp1[i]->lStartTime,psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1],psIp1[i]->lService);      
                if(psIp1[i]->lStartTime > 0&&psIp1[i]->lLastTime>0 && (lTime - psIp1[i]->lStartTime > lBsteptime)) {
         	    	if((lBsteptime+psIp1[i]->lLastTime>lTime)&&(psIp1[i]->lLastTime>psIp1[i]->lStartTime)){
         	    		if(lBsteptime+psIp1[i]->lLastTime>lTime){
         	    			

       //  	          printf("lBsteptime=%d\n",lBsteptime);
         	    			
         	    		   fRate=(float)(lBsteptime+psIp1[i]->lLastTime-lTime)/lBsteptime;
         	    		  }
         	    		  else{
         	    		  	fRate=0;
         	    		  }
   //    	    		printf("2fRate=%f\n",fRate);
                   stService[lSum-1].band = stService[lSum-1].band+(psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1])*fRate/(psIp1[i]->lLastTime - psIp1[i]->lStartTime);
                  }
                  else{
 //                 	stService[lSum-1].band= stService[lSum-1].band+(psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) /lBsteptime;
                  }
               
              }
             	else {
                stService[lSum-1].band= stService[lSum-1].band+(psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) /lBsteptime;
             	}
             
             
             
              	   	
        	}
        }
       
       
       
       
        
          
          
             
 //  printf("lSum=%d\n",lSum);           
              
      lTotpg = (lSum - 1) / iMaxRecord + 1; 
      utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
      utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
     
     
   
     iNum=0;
     
     if(lSum>0)
     qsort(stService,lSum,sizeof(struct serviceonline_s),ncUtlSortOnService2); 
     
      for(i = lStartRec; i < lSum && iNum < iMaxRecord;i++) {
   
//   printf("serivceid=%d\n",stService[i].id);
 //  printf("inbytes=%d\n",stService[i].inbyte);
   
       psUserInfo = (ncUserInfo *)ncGetUserInfoById(psShmHead,stService[i].id);
       if(psUserInfo){
       	if(psUserInfo->lGid==lGroupid0||lGroupid0==0)
        {
       	
         strcpy(caIp,utComHostIp(htonl(psUserInfo->lIp)));
      	 if(psUserInfo->psUser){
           utPltPutLoopVar(psDbHead,"dispname", iNum+1,psUserInfo->psUser->dispname);
          
         }
          if(psUserInfo->psUser->psGroup) {
                utPltPutLoopVar(psDbHead,"dept", iNum+1,psUserInfo->psUser->psGroup->groupname);
          }
        utPltPutLoopVarF(psDbHead,"userid",iNum+1,"%d",stService[i].id);
        utPltPutLoopVar(psDbHead,"id",iNum+1,caService);
        utPltPutLoopVar(psDbHead,"ip",iNum+1,caIp);
        utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(stService[i].inbyte,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte,caTemp));
        
        utPltPutLoopVarF(psDbHead, "subytes2",iNum+1,"%llu",stService[i].outbyte+stService[i].inbyte); 
        utPltPutLoopVarF(psDbHead, "subytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte+stService[i].inbyte,caTemp));                 
        utPltPutLoopVar(psDbHead,"flag",iNum+1,caFlag);                
        utPltPutLoopVar(psDbHead,"band", iNum+1, utStrLtoF8(8*stService[i].band,caTemp));                         
       iNum++;
       
      }
       
     }
    }
     utPltPutVarF(psDbHead,"TotRec","%d",iNum);
     i=0;
    while(iNum < iMaxRecord) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
     
    free(psIp);
    free(stService);
    free(psIp1);
  
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncuserviceonlineuser.htm");
    
    return 0;
}



/* 在线用户即时流量 
       原nc008.c 2008/09/07修改 
 */
 
int ncWebShowUserFlow(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
   long lband;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo,**psUser;
    char       *pHash;
    pasHashInfo sHashInfo;
    int pre_id;
    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[16];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
   
    char caTemp[4024];

   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
    pasDbCursor *psCur;
    char caFlag[12];
    char caIp[20];
  unsigned long long lSumFlow,lBytes[2];
  unsigned long lSumTime;
    double  d1,d2;
 	  long lSum,service_flag,m,lService;
 	  char caId[20];
	 struct serviceonline_s *stService;
   int lSumUser;
    char caSort[12];
    char caSear[32],caSearBut[32];
    char caMac[32],caSet[32],caExport[16];
   long index;
   long temp_order,temp_sort;
   unsigned char *pHash_u;
	 pasLHashInfo  sHashInfo2;
   struct serviceonline_s *psFlow;
   
   
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
    lSumUser=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
   	iReturn = utMsgGetSomeNVar(psMsgHead,6,
   	                "cond",UT_TYPE_STRING, 20, caSear,
   	                "search",    UT_TYPE_STRING, 15, caSearBut,
					"CurPg",   UT_TYPE_STRING, 12, caCurpg,
					"sortby",  UT_TYPE_STRING, 2,  caSort,
					"orderby",   UT_TYPE_STRING, 2, caOrder,
					"export",    UT_TYPE_STRING,11,caExport);

    if(utStrIsSpaces(caSort)) {
        iSort = 6;
    }
    else {
        iSort = atol(caSort);					
    }
    if(caOrder[0] == '1') {
        iOrder = 1;
    }
    else {
        iOrder = 0;
    }

//printf("pid=%d\n",getpid());
//sleep(15);

    pHash = utShmHashHead(psShmHead,NC_LNK_USERINFO);
    if(pHash == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_space.htm",ncLang("0445权限检查"),ncLang("0400内部错误"));
        return 0;
    }
   lSumUser=0;
   lSumUser = utShmHashRecord(psShmHead,NC_LNK_USERINFO);
    
   stService=malloc((lSumUser+1)*sizeof(struct serviceonline_s));
   if(stService==NULL){
   	    
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),ncLang("0404内存出错"));
           return 0;
       
   }
   memset(stService,0,(lSumUser+1)*sizeof(struct serviceonline_s));
   pHash_u = (unsigned char *)pasLHashInit(40000,40000,sizeof(struct serviceonline_s),0,4); 
			if(pHash_u == NULL) {
		        printf(" Memory HttpHash Error \n");
		        return (-1);
		}
 
 
    psUserInfo = (ncUserInfo *)pasHashFirst(pHash,&sHashInfo);
     j = 0;

//带宽取用户信息中的记录，进出流量取IPLOG记录
  lSum=0;
   for(i=0;i<lSumUser && psUserInfo;i++) {
        strcpy(caIp,utComHostIp(htonl(psUserInfo->lIp)));
        strcpy(caMac,ncCvtMac(psUserInfo->mac));
        if(psUserInfo->psUser&&psUserInfo->psUser->psGroup){
        if((psUserInfo->lBytes[0] + psUserInfo->lBytes[1] > 0 && (lGroupid0 == 0 || psUserInfo->lGid == lGroupid0)) &&
            (utStrIsSpaces(caSear) || strstr(psUserInfo->psUser->dispname,caSear) ||
             strstr(psUserInfo->psUser->psGroup->groupname,caSear)
             ||strstr(caIp,caSear) || strstr(caMac,caSear) )) {
             	
  //           	stService[j].id=psUserInfo->lId;
  //           	stService[j].inbyte=0;
  //           	stService[j].outbyte=0;
  //           	stService[j].lasttime=0;
          psFlow = (struct serviceonline_s *)pasLHashLookA(pHash_u,&psUserInfo->lIp);
        if(psFlow){
          psFlow->id=psUserInfo->lId;
  
  //计算带宽           	
             	lBytes[0] = psUserInfo->sCount.lSBytes[0] + psUserInfo->sCount.lCBytes[0] ;
   	          lBytes[1] = psUserInfo->sCount.lSBytes[1] + psUserInfo->sCount.lCBytes[1];
              lSumTime = time(0) - psUserInfo->sCount.lSTime; 	
       				if(lSumTime < 15) {  
       	    			lBand = (lBytes[0] + lBytes[1]) / 15;
       				}
       				else {
       	    			lBand = (lBytes[0] + lBytes[1]) / lSumTime;
        			}
  //           	stService[j].band=lBand;
             	psFlow->band=lBand;
 //            	j++;
            }
        }
      }
        psUserInfo = (ncUserInfo *)pasHashNext(&sHashInfo);
    }
 
 
 
 
 
 
 
   

 //按用户ID进行排序，为二分法累计流量做准备  
// temp_sort=iSort;
// temp_order=iOrder;
//   iOrder=1;
//   iSort=4;
   
//   qsort(stService,lSum,sizeof(struct serviceonline_s),ncUtlSortOnService3); 
 
// iSort=temp_sort;
// iOrder=temp_order;   
    
   lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
    
   if(lMaxRec>1000000)
    lMaxRec=1000000;
    

    psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
   
   
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }        
    lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    
    if(lSumRec>1000000)
    lSumRec=1000000;
    
//    psIp1 = (ncIp **) malloc((lSumRec+2) * sizeof(ncIp *));
    iSum = 0;
    iMaxRecord = utComGetVar_ld(psShmHead,"MaxNewIpRec",30);
  

   if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * iMaxRecord;


//在IP记录中统计用户的进出流量

   iSum=0;
    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&(psIp[i]->cStatus!=NC_USER_NOMON)&&(psIp[i]->psUser)) {   
        strcpy(caIp,utComHostIp(htonl(psIp[i]->psUser->lIp)));

           if( (lGroupid0 == 0 || psIp[i]->psUser->lGid == lGroupid0) &&
            (utStrIsSpaces(caSear) || strstr(psIp[i]->psUser->psUser->dispname,caSear) ||
             strstr(psIp[i]->psUser->psUser->psGroup->groupname,caSear)
             ||strstr(caIp,caSear) || strstr(caMac,caSear) )) {
         
              psFlow = (struct serviceonline_s *)pasLHashLook(pHash_u,&psIp[i]->lSip);
			        if(psFlow){
			        	psFlow->inbyte=psFlow->inbyte+psIp[i]->lBytes[1];
			        	psFlow->outbyte=psFlow->outbyte+psIp[i]->lBytes[0];
			        	 if(psIp[i]->lLastTime>psFlow->lasttime){
				          	psFlow->lasttime=psIp[i]->lLastTime;
				          }
			         }
 //获取USERID对应 stService的下标       
    //            	index=ncUtlLoolFlowByUserId(stService,lSum,psIp[i]->psUser->lId);
	  // 	  					if(index!=-1){
	  //                  stService[index].inbyte=stService[index].inbyte+psIp[i]->lBytes[1];
	  //        				  stService[index].outbyte=stService[index].outbyte+psIp[i]->lBytes[0];
	 //         				  if(psIp[i]->lLastTime>stService[index].lasttime){
	 //         				  	stService[index].lasttime=psIp[i]->lLastTime;
	//          				  }
	//      						}
         
  
        		  }
         } 
      } 
      
       psFlow= (struct serviceonline_s *)pasLHashFirst(pHash_u,&sHashInfo2);
       lSum=0;
       while(psFlow){
       	  stService[lSum].id=psFlow->id;
       	  stService[lSum].inbyte=psFlow->inbyte;
       	  stService[lSum].outbyte=psFlow->outbyte;
       	  stService[lSum].lasttime=psFlow->lasttime;
       	  stService[lSum].sip=psFlow->sip;
       	  stService[lSum].band=psFlow->band;
       	  lSum++;
       	psFlow = (struct serviceonline_s *)pasLHashNextS(&sHashInfo2);
      }
      
      
      for(i=0;i<lSum;i++){
       	if(stService[i].outbyte+stService[i].inbyte==0){
       		stService[i].band=0;
       	}
       	if(stService[i].band==0){
       		stService[i].outbyte=0;
       		stService[i].inbyte=0;
       	}
      }  
        	
     
   
     iNum=0;
     
     if((lSum>0)&&(iSort > 0))
     qsort(stService,lSum,sizeof(struct serviceonline_s),ncUtlSortOnService3); 
     
     if(!utStrIsSpaces(caExport)) { /* 导出  */
        FILE *fp;
        char caFilename[32];
        char caFile[128];
        char caPath[128];
        char caDispname[64],caUsername[32];
        char caIp[20],caInbyte[20],caOutbyte[20],caSumbyte[20],caBand[20];
        char caGroupname[64];
        sprintf(caFilename,"ncuser%d.csv",utStrGetId());
        sprintf(caPath,"%s/tmp",utComGetVar_sd(psShmHead,"MailDir","/home/ncmysql/nc/maillog"));
        sprintf(caFile,"%s/%s",caPath,caFilename);
        fp = fopen(caFile,"w");
        
        if(fp == NULL) {
            free(psIp);
           
            free(stService);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0834在线用户"),ncLang("0546数据导出出错"));
            return 0;
        }
         fprintf(fp,ncLang("0835在线用户实时流量(时间:%s)\n"),utTimFormat("%Y-%m-%d %H:%M:%S",time(0)));
        ncUtlPrintCsv(fp,9,UT_TYPE_STRING,ncLang("0711序号"),
                       UT_TYPE_STRING,ncLang("0692姓    名"),
                       UT_TYPE_STRING,ncLang("0728用 户 名"),
                       UT_TYPE_STRING,ncLang("0122本人IP"),
                       UT_TYPE_STRING,ncLang("0147部    门"),
                       UT_TYPE_STRING,ncLang("0280即时进流量"),
                       UT_TYPE_STRING,ncLang("0279即时出流量"),
                       UT_TYPE_STRING,ncLang("0284即时总流量"),
                       UT_TYPE_STRING,ncLang("1821带宽(bps)")
                       
                       );
       for(i = 0; i < lSum;i++) {
       	memset(caDispname,0,sizeof(caDispname));
       	memset(caUsername,0,sizeof(caUsername));
       	memset(caIp,0,sizeof(caIp));
       	memset(caInbyte,0,sizeof(caInbyte));
       	memset(caOutbyte,0,sizeof(caOutbyte));
       	memset(caGroupname,0,sizeof(caGroupname));
       	memset(caSumbyte,0,sizeof(caSumbyte));
       	memset(caBand,0,sizeof(caBand)); 
        psUserInfo = (ncUserInfo *)ncGetUserInfoById(psShmHead,stService[i].id);
       	if(psUserInfo){
           strcpy(caIp,utComHostIp(htonl(stService[i].sip)));     	
           strcpy(caDispname,psUserInfo->psUser->dispname);
           strcpy(caUsername,psUserInfo->psUser->username);
         }
          if(psUserInfo->psUser->psGroup) {
           strcpy(caGroupname,psUserInfo->psUser->psGroup->groupname);
          }
          strcpy(caInbyte,utStrLtoF8(stService[i].inbyte,caTemp));
          strcpy(caOutbyte,utStrLtoF8(stService[i].outbyte,caTemp));
          strcpy(caSumbyte,utStrLtoF8(stService[i].inbyte + stService[i].outbyte,caTemp));
          strcpy(caBand,utStrLtoF8(stService[i].band*8,caTemp));
          
          
          
         ncUtlPrintCsv(fp,9,UT_TYPE_LONG,i+1,
                                UT_TYPE_STRING,caDispname,
                                UT_TYPE_STRING,caUsername,
                                UT_TYPE_STRING,caIp,
                                UT_TYPE_STRING,caGroupname,
                                UT_TYPE_STRING,caInbyte,
                                UT_TYPE_STRING,caOutbyte,
                                UT_TYPE_STRING,caSumbyte,
                                UT_TYPE_STRING,caBand);
                                
                                
        }
        fclose(fp);
        utPltFileDownload(iFd,"application/text",caPath,caFilename,caFilename);
        remove(caFile);
            free(psIp);
            free(pHash_u);
            free(stService);
        return 0;
        
        
     }
      psDbHead = utPltInitDb();   
      lTotpg = (lSum - 1) / iMaxRecord + 1; 
      utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
      utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
    
     
     
      for(i = lStartRec; i < lSum && iNum < iMaxRecord;i++) {
   
//   printf("serivceid=%d\n",stService[i].id);
 //  printf("inbytes=%d\n",stService[i].inbyte);
    utPltSetCvtHtml(1);
       psUserInfo = (ncUserInfo *)ncGetUserInfoById(psShmHead,stService[i].id);
       if(psUserInfo){
         strcpy(caIp,utComHostIp(htonl(stService[i].sip)));
      	 if(psUserInfo->psUser){
           utPltPutLoopVar(psDbHead,"dispname", iNum+1,psUserInfo->psUser->dispname);
           utPltPutLoopVar(psDbHead,"username", iNum+1,psUserInfo->psUser->username);
         }
          if(psUserInfo->psUser->psGroup) {
                utPltPutLoopVar(psDbHead,"dept", iNum+1,psUserInfo->psUser->psGroup->groupname);
          }
          
     utPltSetCvtHtml(0);     
        utPltPutLoopVarF(psDbHead,"num",  iNum+1,"%ld",i+1);
        if(stService[i].outbyte+stService[i].inbyte>0)
          utPltPutLoopVarF(psDbHead,"userid",iNum+1,"%d",stService[i].id);
          
          
        utPltPutLoopVar(psDbHead,"id",iNum+1,caService);
        utPltPutLoopVar(psDbHead,"ip",iNum+1,caIp);
        utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(stService[i].inbyte,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte,caTemp));
        utPltPutLoopVar(psDbHead,"sumbytes", iNum+1,utStrLtoF8(stService[i].inbyte + stService[i].outbyte,caTemp));
        utPltPutLoopVarF(psDbHead, "subytes2",iNum+1,"%llu",stService[i].outbyte+stService[i].inbyte); 
        utPltPutLoopVarF(psDbHead, "subytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte+stService[i].inbyte,caTemp)); 
        
         if(stService[i].lasttime>0)               
        		utPltPutLoopVar(psDbHead,"lasttime",iNum+1,utTimFormat("%m-%d %H:%M:%S",stService[i].lasttime)); 
        		               
        utPltPutLoopVar(psDbHead,"band", iNum+1, utStrLtoF8(stService[i].band*8,caTemp));                        
       iNum++;
     }
    }
     utPltPutVarF(psDbHead,"TotRec","%d",lSum);
     i=0;
    while(iNum < iMaxRecord) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
     
            free(psIp);
         
            free(stService);
            free(pHash_u);
  
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_list_onlineuser_f.htm");
    
    return 0;
}





//统计首页当前用户在线这段时间的网络流量
 
int ncWebServiceStatByUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
    ncHttpBuffer *psHttp;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    unsigned long long lSum_unkownin,lSum_unkownout,lSum_unkown;
    char unkownid[2560];
    char caName[128],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;

   strcpy(unkownid,"0");
   lSum_unkownin=0;
   lSum_unkownout=0;
   lSum_unkown=0;
    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[16];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
    int service_flag;
    char caTemp[4024];
  
   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
    pasDbCursor *psCur;
    char caFlag[12];
    char caSumbytes[20];
    long long lSum,lUsum;
 	  long sflag,m;
	 struct serviceonline_s *stService;
   int lSumservice,lService;
   long long lSumbytes,lUbytes,lDbytes,lTot;
   
   char sumbytes_desc[256];
   
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
//printf("begin\n");
  lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
  if(lMaxRec>200000)
  lMaxRec=200000;
 

    
  psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
  lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    if(lSumRec>200000)
    lSumRec=200000;
    
printf("lSumRec=%d\n",lSumRec);   
    
   stService=malloc(500*sizeof(struct serviceonline_s));
   if(stService==NULL){
   	    
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),ncLang("0404内存出错"));
           return 0;
       
   }
    memset(stService,0,500*sizeof(struct serviceonline_s));

//printf("lSumservice=%d\n",lSumservice);
    iReturn = utMsgGetSomeNVar(psMsgHead,2,"userid",    UT_TYPE_STRING, 10, caUserid,
                                            "sum",      UT_TYPE_STRING, 18,caSumbytes);
                                            
    lSum=atoll(caSumbytes);
    
 //   printf("lSum=%llu\n",lSum);                                    
                                            
    lSumservice=0;
       lTime=time(0);
    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&(psIp[i]->cStatus!=NC_USER_NOMON)&&psIp[i]->psUser&&atol(caUserid)==psIp[i]->psUser->lId) {
           
         sflag=0;
         for(m=0;m<lSumservice;m++){
         	   if(psIp[i]->lService==stService[m].id){
         	      sflag=1;
//         	  printf("m=%d\n",m);
         	      stService[m].inbyte=stService[m].inbyte+psIp[i]->lBytes[1];
         	      stService[m].outbyte=stService[m].outbyte+psIp[i]->lBytes[0];
        	      
         	      
        		    break;
        		  }
         }
         if((sflag==0)&&(lSumservice<500)){
        		 
        		 stService[lSumservice].id=psIp[i]->lService;
        		 stService[lSumservice].inbyte=psIp[i]->lBytes[1];
         	   stService[lSumservice].outbyte=psIp[i]->lBytes[0];
         	        	 
         	   lSumservice++;
         }
                           
       }
          
     }
    

//printf("midd\n");    
	psDbHead = utPltInitDb();
	  psUserInfo = (ncUserInfo *)ncGetUserInfoById(psShmHead,atol(caUserid));
	 if(psUserInfo&&atol(caUserid)>0){
	 	
	 	
	 	  utPltPutVar(psDbHead,"stime",utTimFormat("%Y-%m-%d %H:%M:%S",psUserInfo->lStartTime));
	 	  utPltPutVar(psDbHead,"etime",utTimFormat("%Y-%m-%d %H:%M:%S",psUserInfo->lLastTime));
	 	  utPltPutVar(psDbHead,"userid",caUserid);
     	utPltPutVarF(psDbHead,"lstime","%d",psUserInfo->lStartTime);
	 	  utPltPutVarF(psDbHead,"letime","%d",psUserInfo->lLastTime);
      utPltPutVar(psDbHead,"dispname",psUserInfo->psUser->dispname);
      sprintf(caTemp,"select sum(ubytes+dbytes) tb,sum(ubytes),sum(dbytes),service from nciplog where vtime>=%d and vtime<=%d and userid=%d group by service order by tb desc",psUserInfo->lStartTime,psUserInfo->lLastTime,atol(caUserid));
 
 printf("caTemp=%s\n",caTemp);     
      psCur=pasDbOpenSql(caTemp,0);
      if(psCur==NULL){
 
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("1492操作iplog出错"),ncLang("1492操作iplog出错"));
            return 0;
        }
     
    lSumbytes=0;
    lUbytes=0;
    lDbytes=0;
    lService=0;

      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lSumbytes,
                                   UT_TYPE_LONG8,8,&lUbytes,
                                   UT_TYPE_LONG8,8,&lDbytes,
                                   UT_TYPE_LONG,4,&lService);
                                 
     
      iNum=0;
      while(iReturn==0){
      	sflag=0;
      	for(i=0;i<lSumservice;i++){
      	if(stService[i].id==lService){
      		   stService[i].inbyte=stService[i].inbyte+lDbytes;
         	   stService[i].outbyte=stService[i].outbyte+lUbytes;
         	   sflag=1;
         	  break;
         	}    
         	}
         	if(sflag==0) {
         		stService[lSumservice].id=lService;
         		stService[lSumservice].inbyte=lDbytes;
         		stService[lSumservice].outbyte=lUbytes;
         		lSumservice++;
         	}   
      	
     	
      	  
    lSumbytes=0;
    lUbytes=0;
    lDbytes=0;
    lService=0;
//utPltSetCvtHtml(1);
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG8,8,&lSumbytes,
                                   UT_TYPE_LONG8,8,&lUbytes,
                                   UT_TYPE_LONG8,8,&lDbytes,
                                   UT_TYPE_LONG,4,&lService);
       }
    
       pasDbCloseCursor(psCur);
  
// printf("lSumervice=%d\n",lSumservice);     
       if(lSumservice>0)
         qsort(stService,lSumservice,sizeof(struct serviceonline_s),ncUtlSortOnService2);

//printf("fffffff\n");         
        iNum=0;
        
        lTot=0;
        for(i=0;i<lSumservice;i++){
        	strcpy(caService,ncUtlGetServiceName(psShmHead,stService[i].id,"\0"));

        			lTot=lTot+stService[i].inbyte+stService[i].outbyte;

      }
       if(lTot<lSum){
       	lTot=lSum;
       } 
       else{
       	lSum=lTot;
      }
   utPltSetCvtHtml(1);    
   lUsum=0;
    
       for(i=0;i<lSumservice;i++){
       	strcpy(caService,ncUtlGetServiceName(psShmHead,stService[i].id,"\0"));
      	if(strlen(caService)>0){
      		
      		iNum++; 
       	utPltPutLoopVarF(psDbHead,"iNum",iNum,"%lu",iNum);
       	lSumbytes=stService[i].inbyte+stService[i].outbyte;
	      utPltPutLoopVarF(psDbHead,"sumbytes",iNum,"%llu",lSumbytes);
	      
	      lUsum=lUsum+lSumbytes;
	      
	      utPltPutLoopVar(psDbHead,"sumbytes_1",iNum,utStrLtoF8(lSumbytes,caTemp));
	      
	      utPltPutLoopVar(psDbHead,"name",iNum,caService);
	      
	      utPltPutLoopVarF(psDbHead,"service",iNum,"%d",stService[i].id);
	      
	      utPltPutLoopVarF(psDbHead,"stime",iNum,"%d",psUserInfo->lStartTime);
	 	    utPltPutLoopVarF(psDbHead,"etime",iNum,"%d",psUserInfo->lLastTime);
	 	    utPltPutLoopVar(psDbHead,"userid",iNum,caUserid);
	      
	      
	      
	      
	      if(lTot==0)
	        utPltPutLoopVar(psDbHead,"rate",iNum,"100");
	      else
	        utPltPutLoopVarF(psDbHead,"rate",iNum,"%0.2f",(float)lSumbytes/lTot*100);
	     
	      	
	      utPltPutLoopVarF(psDbHead,"name_desc",iNum,"%-16s",caService);
	      if(lSumbytes>1024*1024)
	        utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum,"%8.02fM",(float)lSumbytes/(1024*1024));
	      else if(lSumbytes>1024)
	        utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum,"%8.02fK",(float)lSumbytes/(1024)); 
	     else
          utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum,"%9d",lSumbytes); 
          
        if(stService[i].inbyte>1024*1024)  
          utPltPutLoopVarF(psDbHead,"inbytes",iNum,"%8.02fM",(float)stService[i].inbyte/(1024*1024));
        else if(stService[i].outbyte>1024)
          utPltPutLoopVarF(psDbHead,"inbytes",iNum,"%8.02fK",(float)stService[i].inbyte/(1024));
        else
          utPltPutLoopVarF(psDbHead,"inbytes",iNum,"%9d",stService[i].inbyte);
          
          
        if(stService[i].outbyte>1024*1024)  
          utPltPutLoopVarF(psDbHead,"outbyte",iNum,"%8.02fM",(float)stService[i].outbyte/(1024*1024));
        else if(stService[i].outbyte>1024)
          utPltPutLoopVarF(psDbHead,"outbyte",iNum,"%8.02fK",(float)stService[i].outbyte/(1024));
        else
          utPltPutLoopVarF(psDbHead,"outbyte",iNum,"%9d",stService[i].outbyte);
          
          utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum-1);
	      }
	      else{
	      	lSum_unkownin=lSum_unkownin+stService[i].inbyte;
	      	lSum_unkownout=lSum_unkownout+stService[i].outbyte;
	      	lSum_unkown=lSum_unkown+stService[i].inbyte+stService[i].outbyte;
	      	sprintf(unkownid+strlen(unkownid),",%d",stService[i].id);
	      	
	      }
      }
       utPltSetCvtHtml(0);  
       
    }
   
     free(psIp);
     free(stService);
 lSum_unkown=lSum-lUsum;
 if(lSum_unkown>0){
 	utPltSetCvtHtml(1);  
 	
       utPltPutLoopVar(psDbHead,"service",iNum+1,unkownid);
       utPltPutLoopVarF(psDbHead,"iNum",iNum+1,"%lu",iNum+1);
     
     	  if(lTot==0)
	        utPltPutLoopVar(psDbHead,"rate",iNum+1,"100");
	      else
	        utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)lSum_unkown/lTot*100);
          utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
     
     	  utPltPutLoopVarF(psDbHead,"sumbytes",iNum+1,"%llu",lSum_unkown);
     	  
     	  utPltPutLoopVar(psDbHead,"sumbytes_1",iNum+1,utStrLtoF8(lSum_unkown,caTemp));
     	  
     	  
     	  
	      utPltPutLoopVar(psDbHead,"name",iNum+1,ncLang("1490未知服务"));
     
      utPltPutLoopVarF(psDbHead,"name_desc",iNum+1,"%-16s",ncLang("1490未知服务"));
     
        if(lSum_unkownout>1024*1024)  
          utPltPutLoopVarF(psDbHead,"outbyte",iNum+1,"%8.02fM",(float)lSum_unkownout/(1024*1024));
        else if(lSum_unkownout>1024)
          utPltPutLoopVarF(psDbHead,"outbyte",iNum+1,"%8.02fK",(float)lSum_unkownout/(1024));
        else
          utPltPutLoopVarF(psDbHead,"outbyte",iNum+1,"%9d",iNum+1,lSum_unkownout);
     
    
        if(lSum_unkownin>1024*1024)  
          utPltPutLoopVarF(psDbHead,"inbytes",iNum+1,"%8.02fM",(float)lSum_unkownin/(1024*1024));
        else if(lSum_unkownin>1024){
        	
          utPltPutLoopVarF(psDbHead,"inbytes",iNum+1,"%8.02fK",(float)lSum_unkownin/(1024));
        }
        else
          utPltPutLoopVarF(psDbHead,"inbytes",iNum+1,"%9d",lSum_unkownin);
 
     
        if(lSum_unkown>1024*1024)  
          utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum+1,"%8.02fM",(float)lSum_unkown/(1024*1024));
        else if(lSum_unkown>1024)
          utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum+1,"%8.02fK",(float)lSum_unkown/(1024));
        else
          utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum+1,"%9d",lSum_unkown);
     utPltSetCvtHtml(0);  
     
     	   utPltPutLoopVarF(psDbHead,"stime",iNum+1,"%d",psUserInfo->lStartTime);
	 	    utPltPutLoopVarF(psDbHead,"etime",iNum+1,"%d",psUserInfo->lLastTime);
	 	    utPltPutLoopVar(psDbHead,"userid",iNum+1,caUserid);
     
    }
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_servicestatbtbyuser.htm");
  
    return 0;
}


//显示首页某种服务的明细

int ncShowFirstServiceDet(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
                   
{
	  unsigned long urlid;
    char urlname[64];
    pasDbCursor *psCur;
    char caTemp[1055];
    char caTotPg[16];
    static int pp;
    int  i,iReturn,iNum,iMark1,iMark2,lSumUser,iPos;
    char caSdate[32],caUserid[32],caUsername[36],caCurPg[16],caUrl[200],caUrl0[200];
    char caSmt[512],caSip[16],caDip[16],caStime[12],caGroupid[32];
    char caUri[129],caHost[65],caTitle[128];
    char caCount[16];
    char caDel[7];
    char caDelwhere[500];
    char sqlbuf[1024];
    unsigned long lUserid,lStart,lSip;
    long lCount;
    int iTitleMark=0;
    long lCurPg,lRowNum,lStartRec,lTotPg,lPort,lGroupid0;
    char caSelect[256],caWhere[256];
    char caExport[16];
    ncUserCont *psUser;
    utPltDbHead *psDbHead;
    char caTitlename[32];
    char temp_group[32];
    char caWhereStr[10];
    char caEdate[20],caUrname[64],caName[64];
    unsigned long lCurtime,lScurtime;
    unsigned long lwtype;
    char caStime0[8],caEtime[8],byt_in[8];
    char caByt[16],caIdfyno[20],caUsername_match[8];
    char caUrlid[16],caUrlid2[16],caWebtitle[64];
    float lRat,group_flag;
    unsigned long lStatrow,lMintime,lMaxtime,lSum,lStarttime,lEndtime,lMaxin,lMinin,lIn,lVtime;
    long lStime,lEtime,lService,dport;
    unsigned long dip;
    char caService[256],caType[32],caDipPort[32],caDate[20];
    unsigned long lDip;
    long long lUbytes,lDbytes,lBytes;
	  long lConntime,lType;
	  char caToday[20];
	  long lFlags,lSid,lAtt,lSize,lCode;
	  char caFile[256],caFrom[64],caTo[64],caSub[128];
	  char caMesg[256];
	  char caService_temp[256],t_s[20];
	  long lFuncode,lTime;
    ncIp **psIp,**psIp1;
    long lMaxRec,iMaxRecord;
    long lSumRec;
    long iSum;
    ncUserInfo  *psUserInfo;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
   	iReturn = utMsgGetSomeNVar(psMsgHead,6,
   	                "service",     UT_TYPE_STRING, 250, caService,
   	                "userid",     UT_TYPE_STRING, 10, caUserid,
   	                "stime",      UT_TYPE_STRING,10,caStime,
   	                "etime",      UT_TYPE_STRING,10,caEtime,
					          "CurPg",      UT_TYPE_STRING, 12, caCurPg,
					          "TotPg",      UT_TYPE_STRING, 12, caTotPg);
				
		if(strstr(caService,",")){                                   //未知服务
			sprintf(caService_temp,"%s,",caService);
		}
					
    lUserid = atol(caUserid);
    
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
    	strcpy(caUsername,caTitlename);
    }
//    lStime=atol(caStime);
//    lEtime=atol(caEtime);
strcpy(caToday,utTimFormat("%Y-%m-%d 00:00:00",time(0)));
lStime=utTimStrToLong("%Y-%m-%d %H:%M:%S",caToday);
lEtime=time(0);
if(strstr(caService,",")==NULL)
     lService=atol(caService);
else
     lService=0;
    
    lCurPg = atol(caCurPg);
    lRowNum = 32;
    if(lCurPg <= 1){
    	 lCurPg = 1;
    	
    	}
    lStartRec = (lCurPg - 1) * lRowNum;
    if(lStartRec <= 0)lStartRec = 0;
   
   
   psDbHead = utPltInitDb();
 
 iSum=0;
 //显示实时IP记录
 
   lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
   if(lMaxRec>200000){
    		lMaxRec=200000;
    }
   if(lMaxRec>0){
   	 psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
      
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }
   	lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);    
    if(lSumRec>200000)
    lSumRec=200000;
    
     psIp1 = (ncIp **) malloc((lSumRec+2) * sizeof(ncIp *));
    
   	iSum=0;
   	iNum=0;
    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&(psIp[i]->cStatus!=NC_USER_NOMON)) {
            psUserInfo = psIp[i]->psUser;
            if((psUserInfo)&&(psUserInfo->lId == lUserid)) {
            	if(strstr(caService,",")){                             //未知服务的IP
            		sprintf(t_s,"%d,",psIp[i]->lService);
            		if(strstr(caService_temp,t_s)){
            			    psIp1[iSum]=psIp[i];
        		      		iSum++;
            		}
            	}
            	else{
         	     if(atol(caService)==psIp[i]->lService){
         	    
        		      		psIp1[iSum]=psIp[i];
        		      		iSum++;
                           
             		  }
                 }
                }
               
            }
        }
   	   
   	   for(i = lStartRec; i < iSum && iNum <lRowNum;i++) {
   	   
   	         	psUserInfo = psIp1[i]->psUser;
      	if(psUserInfo){
      	            	
            		      utPltPutLoopVar(psDbHead,"dispname", iNum+1,psUserInfo->psUser->dispname);
            		      utPltPutLoopVarF(psDbHead,"userid",iNum+1,"%lu",psUserInfo->psUser->userid);
            		      
        	              utPltPutLoopVar(psDbHead,"sip",  iNum+1,utComHostIp(htonl(psIp1[i]->lSip)));
                          utPltPutLoopVar(psDbHead,"dip",  iNum+1,utComHostIp(htonl(psIp1[i]->lDip)));
                        utPltPutLoopVarF(psDbHead,"dport", iNum+1,"%d",psIp1[i]->nDport);
                          utPltPutLoopVar(psDbHead,"service", iNum+1,ncUtlGetServiceName(psShmHead,psIp1[i]->lService,"\0"));
                          
        if(psIp1[i]->cType == 6) {
            utPltPutLoopVar(psDbHead,"prot",   iNum,"TCP");
        }
        else {
            utPltPutLoopVar(psDbHead,"prot",   iNum,"UDP");
        } 
                          
                          
 //                         utPltPutLoopVarF(psDbHead,"id",iNum+1,"%d",psIp1[i]->lService);
//                          utPltPutLoopVar(psDbHead, "lasttime",iNum+1,
//                          utTimFormat("%H:%M:%S",psIp1[i]->lLastTime));
                          
                          utPltPutLoopVar(psDbHead, "sdate",iNum+1,
                          utTimFormat("%H:%M:%S",psIp1[i]->lStartTime));
                          
                          
                         
                          utPltPutLoopVarF(psDbHead, "dbytes",iNum+1,"%s",utStrLtoF8(psIp1[i]->lBytes[1],caTemp));
                          utPltPutLoopVarF(psDbHead, "ubytes",iNum+1,"%s",utStrLtoF8(psIp1[i]->lBytes[0],caTemp));
                          
                          utPltPutLoopVar(psDbHead,"sumbytes",iNum+1,utStrLtoF8(psIp1[i]->lBytes[1]+psIp1[i]->lBytes[0],caTemp));
                          
                  iNum++;
            }
   	   
   	   
   	   
   	   
   	   
   	}
   	
   	
   	
   	
   	
   	free(psIp);
    free(psIp1);
   	
   }
 
 
 
 
 
 
 
 
 
 
 
    lCount=0;
//    if(lService==3){
//	    sprintf(caTemp,"select count(*) from nchttplog where vtime>=%d and vtime<=%d and userid=%d ",lStime,lEtime,lUserid);
	   
//	    pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
//   }
//   else if(lService==4||lService==5){
//   	 sprintf(caTemp,"select count(*) from ncmailmon where sdate>='%s' and userid=%d ",caToday,lUserid);
//   	 pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
//   }
//   else if(lService==6||lService==10||lService==11||lService==21||lService==1004||lService==1011||lService==1007){
//   	sprintf(caTemp,"select count(*) from nccontlog where stime>=%d and stime<=%d and userid=%d ",lStime,lEtime,lUserid);
//   	pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
//  }
//   else{
    lStime=atol(caStime);
    lEtime=atol(caEtime);
    if(strstr(caService,","))
        sprintf(caTemp,"select count(*) from nciplog where vtime>=%d and vtime<=%d and userid=%d and service in (%s)",lStime,lEtime,lUserid,caService);
     else
   		sprintf(caTemp,"select count(*) from nciplog where vtime>=%d and vtime<=%d and userid=%d and service=%d",lStime,lEtime,lUserid,lService);
	 
	    pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
//  }



lCount=lCount+iSum;




    lTotPg = (lCount -1 ) / lRowNum + 1;
 
   
    utPltPutVarF(psDbHead,"stime","%d",lStime);
    utPltPutVarF(psDbHead,"etime","%d",lEtime);
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
    utPltPutVarF(psDbHead,"userid", "%lu", lUserid);   
    utPltPutVar(psDbHead,"title_name",caTitlename);
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"servicename",ncCvtSetService(psShmHead,atol(caService)));
   
    utPltPutVar(psDbHead,"sdate",utTimFormat("%Y-%m-%d %H:%M:%S",lStime));
    utPltPutVar(psDbHead,"edate",utTimFormat("%Y-%m-%d %H:%M:%S",lEtime));
   
   
 /*   if(lService==3){
//           psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.uri,b.host,b.title,b.sdate,b.stime,b.bytes,b.dip,b.dport,b.name,b.wtype \
//		                from (select vtime,userid,sip,uri,host,title,sdate,stime,bytes,dip,dport,c.name,wtype from nchttplog left join ncwebclass c on (tid=c.id) where vtime>=%d and vtime<=%d ) b where b.userid=%d order by b.vtime desc limit %d,%d",lStime,lEtime,lUserid,lStartRec,32);
		            
       
		       
		    
//		    if(psCur == NULL) {
//		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
//		        return 0;
		    }        
		
		    iReturn = 0;
		    
		        dip=0;
		        dport=0;
		        memset(caType,0,sizeof(caType));
		        lwtype=0;
		        lVtime=0;
		        lUserid=0;
		        memset(caUrl,0,sizeof(caUrl));
		        memset(caHost,0,sizeof(caHost));
		        memset(caTitle,0,sizeof(caTitle));
		        memset(caSdate,0,sizeof(caSdate));
		        memset(caStime,0,sizeof(caStime));
		        lBytes=0;
		   
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lVtime,
		    															 UT_TYPE_LONG,  4, &lUserid,
		                                   UT_TYPE_ULONG, 4, &lSip,
		                                   UT_TYPE_STRING,127,caUrl,
		                                   UT_TYPE_STRING,63,caHost,
		                                   UT_TYPE_STRING,127,caTitle,
		                                   UT_TYPE_STRING,12,caSdate,
		                                   UT_TYPE_STRING,8,caStime,
		                                   UT_TYPE_LONG8,  8, &lBytes,
		                                   UT_TYPE_ULONG,4,&dip,
		                                   UT_TYPE_LONG,4,&dport,
		                                   UT_TYPE_STRING,30,caType,
		                                   UT_TYPE_LONG,4,&lwtype);

		    iNum = 0;                                   
		    while(iReturn == 0||iReturn==1405)  {
		    	
		    	if(iNum<lRowNum){
		        iNum++;
		        psUser = ncUtlGetUserContById(psShmHead,lUserid);
		        if(psUser) {
		            utPltPutLoopVar(psDbHead,"dispname", iNum,psUser->dispname);
		            utPltPutLoopVar(psDbHead,"username", iNum,psUser->username);
		            if(psUser->psGroup) {
		                utPltPutLoopVar(psDbHead,"groupname", iNum,psUser->psGroup->groupname);
		            }
		        }
		        memset(caDipPort,0,sizeof(caDipPort));
		        if(dip!=0)
		        sprintf(caDipPort,"%s:%lu",utComHostIp(htonl(dip)),dport);
		        utPltPutLoopVar(psDbHead,"dip",iNum,caDipPort);
		        utPltPutLoopVar(psDbHead,"sip",      iNum,utComHostIp(htonl(lSip)));
		        sprintf(caDate,"%s %s",caSdate+5,caStime);
		        utPltPutLoopVar(psDbHead,"sdate",    iNum,caDate);
		        utPltPutLoopVar(psDbHead,"bytes",    iNum,utStrLtoF8(lBytes,caTemp));
		        sprintf(caUrl0,"http://%s%s",caHost,caUrl);
		        strcpy(caUrl,caUrl0);
		        caUrl[78]=0;
		        utPltSetCvtHtml(1);
		        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid); 
		        utPltPutLoopVar(psDbHead,"url",   iNum,caUrl);
		        utPltPutLoopVar(psDbHead,"urllnk",   iNum,caUrl0);
		        utPltPutLoopVar(psDbHead,"title",   iNum,caTitle);
		        ncLimGetWebNameByUrl(psShmHead,caHost,caType,caName);
		        utPltPutLoopVar(psDbHead,"urltype", iNum,caType);
		        }
		
		
		    utPltSetCvtHtml(0);
		       
		      		       
		        dip=0;
		        dport=0;
		        memset(caType,0,sizeof(caType));
		        lwtype=0;
		        lVtime=0;
		        lUserid=0;
		        memset(caUrl,0,sizeof(caUrl));
		        memset(caHost,0,sizeof(caHost));
		        memset(caTitle,0,sizeof(caTitle));
		        memset(caSdate,0,sizeof(caSdate));
		        memset(caStime,0,sizeof(caStime));
		        lBytes=0;
		   
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_ULONG,4,&lVtime,
		    															 UT_TYPE_LONG,  4, &lUserid,
		                                   UT_TYPE_ULONG, 4, &lSip,
		                                   UT_TYPE_STRING,127,caUrl,
		                                   UT_TYPE_STRING,63,caHost,
		                                   UT_TYPE_STRING,127,caTitle,
		                                   UT_TYPE_STRING,12,caSdate,
		                                   UT_TYPE_STRING,8,caStime,
		                                   UT_TYPE_LONG8,  8, &lBytes,
		                                   UT_TYPE_ULONG,4,&dip,
		                                   UT_TYPE_LONG,4,&dport,
		                                   UT_TYPE_STRING,30,caType,
		                                   UT_TYPE_LONG,4,&lwtype);

		                                  
		    
		  }
		    pasDbCloseCursor(psCur);
		    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_showfirsthttplog.htm");
		    return 0;
		}
		else if(lService==4||lService==5){
		//	邮件
		
		    psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),b.flags,b.sid,b.userid,b.sdate,b.mailfile,b.mailfrom,b.mailto,b.mailsub,b.mailasize,b.mailsize,b.ccode \
                from  ncmailmon b  where b.sdate>='%s' and b.userid=%d order by b.sid desc limit %d,%d",caToday,lUserid,lStartRec,32);
		
          
		    
		    if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
		        return 0;
		    }        
		
		    iReturn = 0;
		    memset(caSip,0,sizeof(caSip));
		    lFlags=0;
		    lSid=0;
		    lUserid=0;
		    memset(caSdate,0,sizeof(caSdate));
		    memset(caFile,0,sizeof(caFile));
		    memset(caFrom,0,sizeof(caFrom));
		    memset(caTo,0,sizeof(caTo));
		    memset(caSub,0,sizeof(caSub));
		    lAtt=0;
		    lSize=0;
		    lCode=0;
		    
	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
  																 UT_TYPE_LONG,4,&lFlags,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,23,caSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lCode);

		    iNum = 0;                                   
		    while(iReturn == 0||iReturn==1405)  {
		    	
		    	if(iNum<lRowNum){
		        iNum++;
        utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
        if(lFlags==2)
        	utPltPutLoopVar(psDbHead,"flags_desc",iNum,"收邮件");
        else if(lFlags==1)
        	utPltPutLoopVar(psDbHead,"flags_desc",iNum,"发邮件");
        else if(lFlags==3)
          utPltPutLoopVar(psDbHead,"flags_desc",iNum,"WebMail");
         
        psUser=ncUtlBufGetUserInfo(psShmHead,lUserid);
        if(psUser){
        	utPltPutLoopVar(psDbHead,"position",iNum,psUser->position);
        }   
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVarF(psDbHead,"num", iNum,"%d",iNum);
        utPltPutLoopVar(psDbHead,"sdate", iNum,caSdate);
        utPltPutLoopVar(psDbHead,"file", iNum,caFile);
        
        utPltSetCvtHtml(1); 
        utPltPutLoopVar(psDbHead,"from", iNum,pasCvtGBK(lCode,caFrom,caTemp,128));
        utPltPutLoopVar(psDbHead,"to", iNum,pasCvtGBK(lCode,caTo,caTemp,128));
        ncUtlStrCvtCtl(caSub);
        utPltPutLoopVar(psDbHead,"sub", iNum,pasCvtGBK(lCode,caSub,caTemp,255));
        utPltSetCvtHtml(0); 
        utPltPutLoopVarF(psDbHead,"size", iNum,"%lu",lSize);
        utPltPutLoopVarF(psDbHead,"ccode", iNum,"%lu",lCode);
        if(lAtt > 0) {
            utPltPutLoopVar(psDbHead,"att", iNum,ncLang("0815有"));
        }
		      }
		      		       
		    memset(caSip,0,sizeof(caSip));
		    lFlags=0;
		    lSid=0;
		    lUserid=0;
		    memset(caSdate,0,sizeof(caSdate));
		    memset(caFile,0,sizeof(caFile));
		    memset(caFrom,0,sizeof(caFrom));
		    memset(caTo,0,sizeof(caTo));
		    memset(caSub,0,sizeof(caSub));
		    lAtt=0;
		    lSize=0;
		    lCode=0;
		    
	iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
  																 UT_TYPE_LONG,4,&lFlags,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
  	                               UT_TYPE_STRING,23,caSdate,
  	                               UT_TYPE_STRING,63,caFile,
  	                               UT_TYPE_STRING,63,caFrom,
  	                               UT_TYPE_STRING,63,caTo,
  	                               UT_TYPE_STRING,127,caSub,
  	                               UT_TYPE_LONG,4,&lAtt,
  	                               UT_TYPE_LONG,4,&lSize,
  	                               UT_TYPE_LONG,4,&lCode);

		                                  
		    
		  }
		    pasDbCloseCursor(psCur);
		    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_showfirstmaillog.htm");
		    return 0;
		}		
		else if(lService==6||lService==10||lService==11||lService==21||lService==1004||lService==1011||lService==1007){
			
			printf("select inet_ntoa(b.sip),b.sid,b.userid,b.stime,b.service,b.mfrom,b.mto,b.mesg,b.funcode \
                from nccontlog b where b.stime>=%d and b.stime<=%d and b.userid=%d order by b.stime desc limit %d,32\n",lStime,lEtime,lUserid,lStartRec);
			
			 psCur = pasDbOpenSqlF("select inet_ntoa(b.sip),b.sid,b.userid,b.stime,b.service,b.mfrom,b.mto,b.mesg,b.funcode \
                from nccontlog b where b.stime>=%d and b.stime<=%d and b.userid=%d order by b.stime desc limit %d,32",lStime,lEtime,lUserid,lStartRec);
       if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
		        return 0;
		    }  
		 memset(caMesg,0,sizeof(caMesg));
    memset(caTo,0,sizeof(caTo));
    memset(caFrom,0,sizeof(caFrom));
    memset(caSip,0,sizeof(caSip));
    lFuncode=0;
    iNum=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
      	                           UT_TYPE_LONG,4,&lSid,
      	                           UT_TYPE_LONG,4,&lUserid,
      	                           UT_TYPE_ULONG,4,&lTime,
  	                               UT_TYPE_LONG,4,&lService,
  	                               UT_TYPE_STRING,47,caFrom,
  	                               UT_TYPE_STRING,47,caTo,
  	                               UT_TYPE_STRING,255,caMesg,
  	                               UT_TYPE_LONG,4,&lFuncode);
		   	while(iReturn == 0||iReturn==1405){
		   		iNum++;
		   		    	utPltPutLoopVar(psDbHead,"service", iNum,ncUtlImName(lService));
        utPltPutLoopVarF(psDbHead,"num",   iNum,"%lu",i);
        utPltPutLoopVarF(psDbHead,"sid",   iNum,"%lu",lSid);
        utPltPutLoopVar(psDbHead,"sip",iNum,caSip);
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%d",lUserid);
        utPltPutLoopVar(psDbHead,"username", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltPutLoopVar(psDbHead,"sdate", iNum,utTimFormat("%m-%d %H:%M:%S",lTime));
        utPltPutLoopVar(psDbHead,"from", iNum,caFrom);
        utPltPutLoopVar(psDbHead,"to", iNum,caTo);
        if(lFuncode==1)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"登录");
        else if(lFuncode==2)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"退出");
        else if(lFuncode==3)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"发送消息");
        else if(lFuncode==4)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"发送文件");
        else if(lFuncode==5)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"接收文件");
        else if(lFuncode==6)
           utPltPutLoopVar(psDbHead,"funcode_desc",iNum,"接收消息");
		 		 memset(caMesg,0,sizeof(caMesg));
		    memset(caTo,0,sizeof(caTo));
		    memset(caFrom,0,sizeof(caFrom));
		    memset(caSip,0,sizeof(caSip));
		    lFuncode=0;
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSip,
		      	                           UT_TYPE_LONG,4,&lSid,
		      	                           UT_TYPE_LONG,4,&lUserid,
		      	                           UT_TYPE_ULONG,4,&lTime,
		  	                               UT_TYPE_LONG,4,&lService,
		  	                               UT_TYPE_STRING,47,caFrom,
		  	                               UT_TYPE_STRING,47,caTo,
		  	                               UT_TYPE_STRING,255,caMesg,
		  	                               UT_TYPE_LONG,4,&lFuncode);
		   		
		   	}    
		    pasDbCloseCursor(psCur);
		    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_showfirstimlog.htm");
		   	        
		}
		else {
			*/
			//iplog
			if(strstr(caService,","))
			   psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype \
          from (select vtime,userid,sip,dip,sdate,stime,ubytes,dbytes,conntime,service,port,trantype from nciplog where vtime>=%d and vtime<=%d ) b  where  b.userid=%d and b.service in (%s) order by b.vtime desc limit %d,%d",lStime,lEtime,lUserid,caService,lStartRec+iNum,32);
		   
			else{
       psCur = pasDbOpenSqlF("select b.vtime,b.userid,b.sip,b.dip,b.sdate,b.stime,b.ubytes,b.dbytes,\
          b.conntime,b.service,b.port,b.trantype \
          from (select vtime,userid,sip,dip,sdate,stime,ubytes,dbytes,conntime,service,port,trantype from nciplog where vtime>=%d and vtime<=%d ) b  where  b.userid=%d and b.service=%d order by b.vtime desc limit %d,%d",lStime,lEtime,lUserid,lService,lStartRec,32);

        }

		    if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
		        return 0;
		    }        
		
		    iReturn = 0;
		    
		        lVtime=0;
		        lUserid=0;
		        lSip=0;
		        lDip=0;
		        memset(caSdate,0,sizeof(caSdate));
		        memset(caStime,0,sizeof(caStime));
		        lUbytes=0;
		        lDbytes=0;
		        lConntime=0;
		        lService=0;
		        lPort=0;
		        lType=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lVtime,
    															 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG8,  8, &lUbytes,
                                   UT_TYPE_LONG8,  8, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
		    iNum = 0;                                   
		    while(iReturn == 0||iReturn==1405)  {
		    	
		    	if(iNum<lRowNum){
		    		iNum++;
        utPltPutLoopVar(psDbHead,"dispname", iNum,
             ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
        utPltSetCvtHtml(0);  
        utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid);  
        utPltPutLoopVar(psDbHead,"sip",      iNum,utComHostIp(htonl(lSip)));
        utPltPutLoopVar(psDbHead,"dip",      iNum,utComHostIp(htonl(lDip)));
        utPltPutLoopVar(psDbHead,"sdate",    iNum,caSdate);
        utPltPutLoopVar(psDbHead,"stime",    iNum,caStime);
        utPltPutLoopVar(psDbHead,"conn",    iNum,utStrLtoF(lConntime,caTemp));
        utPltPutLoopVar(psDbHead,"ubytes",  iNum,utStrLtoF8(lUbytes,caTemp));
        utPltPutLoopVar(psDbHead,"dbytes",   iNum,utStrLtoF8(lDbytes,caTemp));
        utPltPutLoopVar(psDbHead,"sumbytes",iNum,utStrLtoF8(lDbytes+lUbytes,caTemp));
 //       printf("lService=%d\n",lService);
        if(lService == 0) {
            utPltPutLoopVar(psDbHead,"service",  iNum,"&nbsp;&nbsp;");
        }
        else {
            utPltPutLoopVar(psDbHead,"service",  iNum,ncCvtSetService(psShmHead,lService));
        }
        
        utPltPutLoopVarF(psDbHead,"dport",    iNum,"%lu",lPort);
        if(lType == 6) {
            utPltPutLoopVar(psDbHead,"prot",   iNum,"TCP");
        }
        else {
            utPltPutLoopVar(psDbHead,"prot",   iNum,"UDP");
        } 
		     }  
		        lVtime=0;
		        lUserid=0;
		        lSip=0;
		        lDip=0;
		        memset(caSdate,0,sizeof(caSdate));
		        memset(caStime,0,sizeof(caStime));
		        lUbytes=0;
		        lDbytes=0;
		        lConntime=0;
		        lService=0;
		        lPort=0;
		        lType=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lVtime,
    															 UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_ULONG,4,&lSip,
                                   UT_TYPE_ULONG,4,&lDip,
                                   UT_TYPE_STRING,12,caSdate,
                                   UT_TYPE_STRING,12,caStime,
                                   UT_TYPE_LONG8,  8, &lUbytes,
                                   UT_TYPE_LONG8,  8, &lDbytes,
                                   UT_TYPE_LONG,  4, &lConntime,
                                   UT_TYPE_LONG,  4, &lService,
                                   UT_TYPE_LONG,  4, &lPort,
                                   UT_TYPE_LONG,  4, &lType);
		       

		                                  
		    
		  }
		    pasDbCloseCursor(psCur);
		    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_showfirstiplog.htm");
			
			

    
    return 0;
}


//访问某网址用户流量排行
int ncShowFirstWebByUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
                   
{
	  unsigned long urlid;
    char urlname[64];
    pasDbCursor *psCur;
    char caTemp[255];
    char caTotPg[16];
    static int pp;
    int  i,iReturn,iNum,iMark1,iMark2,lSumUser,iPos;
    char caSdate[32],caUserid[32],caUsername[36],caCurPg[16];
    char caSmt[512],caSip[16],caDip[16],caStime[12],caGroupid[32];
    char caUri[129],caHost[65],caTitle[128];
    char caCount[16];
    char caDel[7];
    long lSumcount;
    long long lSumbytes;
    char sqlbuf[1024];
    unsigned long lUserid,lStart,lBytes,lSip;
    long lCount;
    int iTitleMark=0;
    long lCurPg,lRowNum,lStartRec,lTotPg,lPort,lGroupid0;

    char caExport[16];
    ncUserCont *psUser;
    utPltDbHead *psDbHead;

    char caWhereStr[10];
    char caEdate[20],caUrname[64];
    char caTitlename[64];
   long lStime,lEtime,lService,dip,dport;
   char caUrl[64];
   
	 char caToday[20];

   	iReturn = utMsgGetSomeNVar(psMsgHead,4,
   	                "host_url",     UT_TYPE_STRING, 63, caUrl,
   	                "userid",     UT_TYPE_STRING, 10, caUserid,
					          "CurPg",      UT_TYPE_STRING, 12, caCurPg,
					          "TotPg",      UT_TYPE_STRING, 12, caTotPg);
				

					
    lUserid = atol(caUserid);
    
    if(strlen(caUserid)!=0){
    	strcpy(caTitlename,ncUtlBufGetUsername(psShmHead,lUserid,"\0"));
    	strcpy(caUsername,caTitlename);
    }
//    lStime=atol(caStime);
//    lEtime=atol(caEtime);
	strcpy(caToday,utTimFormat("%Y/%m/%d",time(0)));
lStime=utTimStrToLong("%Y-%m-%d %H:%M:%S",caToday);
lEtime=time(0);
   
    lCurPg = atol(caCurPg);
    lRowNum = 32;
    if(lCurPg <= 1){
    	 lCurPg = 1;
    	
    	}
    lStartRec = (lCurPg - 1) * lRowNum;
    if(lStartRec <= 0)lStartRec = 0;
   
   
   psDbHead = utPltInitDb();
 
    lCount=0;
  
	    sprintf(caTemp,"select count(distinct userid) from nchttpcount where sdate>='%s' and host='%s' ",caToday,caUrl);	  
	printf("caTemp=%s\n",caTemp); 
	    pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
 
    lTotPg = (lCount -1 ) / lRowNum + 1;
 
   
    utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
    utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
    utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
   
    utPltPutVar(psDbHead,"host_url",caUrl);
   
    utPltPutVar(psDbHead,"sdate",utTimFormat("%Y-%m-%d %H:%M:%S",lStime));
    utPltPutVar(psDbHead,"edate",utTimFormat("%Y-%m-%d %H:%M:%S",lEtime));
   
   
   
    psCur = pasDbOpenSqlF("select userid,sum(lcount),sum(bytes) sb from nchttpcount where sdate>='%s' and host='%s' group by userid order by sb desc limit %d,%d ",caToday,caUrl,lStartRec,32);	               		   		    
		if(psCur == NULL) {
		        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0172查询数据出错 %s"),pasDbErrorMsg(NULL));
		        return 0;
		}        
		
		    iReturn = 0;
		    
		    lUserid=0;
		    lSumcount=0;
		    lSumbytes=0;
		   
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
		    															 UT_TYPE_LONG,  4, &lSumcount,
		                                   UT_TYPE_LONG8, 8, &lSumbytes);
		                                 

		    iNum = 0;                                   
		    while(iReturn == 0||iReturn==1405)  {
		    	
		    
		        iNum++;
		        psUser = ncUtlGetUserContById(psShmHead,lUserid);
		        if(psUser) {
		            utPltPutLoopVar(psDbHead,"dispname", iNum,psUser->dispname);
		           
		            if(psUser->psGroup) {
		                utPltPutLoopVar(psDbHead,"groupname", iNum,psUser->psGroup->groupname);
		            }
		            utPltPutLoopVar(psDbHead,"ip",iNum,utComHostIp(htonl(psUser->ip)));

		           utPltPutLoopVar(psDbHead,"bytes",    iNum,utStrLtoF8(lSumbytes,caTemp));

		           utPltPutLoopVarF(psDbHead,"userid",iNum,"%lu",lUserid); 
		       
		           utPltPutLoopVarF(psDbHead,"sumcount", iNum,"%lu",lSumcount);
		        }
		
		   
		 		 lUserid=0;
		    lSumcount=0;
		    lSumbytes=0;
		   
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lUserid,
		    															 UT_TYPE_LONG,  4, &lSumcount,
		                                   UT_TYPE_LONG8, 8, &lSumbytes);
		                                  
		    
		  }
		    pasDbCloseCursor(psCur);
		    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_showfirstwebbyuser.htm");
		    return 0;
	
    return 0;
}









//统计首页即时网络细分服务流量
 
int ncWebOnlieServiceStat_first(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
    ncHttpBuffer *psHttp;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;

    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[64];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
    int service_flag;
    char caTemp[4024];
  
   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
    pasDbCursor *psCur;
    char caFlag[12];
    long long lSumbytes;
 	  long sflag,m;
 	  float rate;
 	  long lBsteptime;
 	  float fRate;
 	  char caPdf[12];    
   struct piechart_datasetf *pChart;           //用于PDF文档输出
//   char *desc[]={"用户名","部门","流量","百分比"};
    int rowrow;                     
    int column; 
//    char **pTemp;
    char caTitle[128];
    char filename[128];
//    int m;
    column=4;
    long mm;
//    long lDate;
    char **desc;
    char caStatdate[128];
 	  
 	  lBsteptime=atol(utComGetVar_sd(psShmHead,"BandSteptime","15"));
 	  
	 struct serviceonline_s *stService;
   int lSumservice;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);   
    if(iReturn != 0) lGroupid0 = 0;

    iReturn = utMsgGetSomeNVar(psMsgHead,3,
		"serv_flag",   UT_TYPE_STRING, 2, caFlag,
		"CurPg",   UT_TYPE_STRING, 12, caCurpg,
		"pdf",UT_TYPE_STRING,8,caPdf);
	
   lSumbytes=0;
    
   lSumservice=0;

 
    iCount = 0;
 

    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
    if(lMaxRec <= 0) {
    	 psDbHead = utPltInitDb();
    	 utPltPutVarF(psDbHead,"CurPg","%d",1);
       utPltPutVarF(psDbHead,"TotPg","%d",1);
       utPltPutVarF(psDbHead,"TotRec","%d",0);
       
        iSum=0;
    	  i=0;
         
       
       
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncuserviceonlinestat_first.htm");
    
       
        return 0;
    }
   if(lMaxRec>400000)
    lMaxRec=400000;
    
    psDbHead = utPltInitDb();
    psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
    
   
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }        
    lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    
    if(lSumRec>400000)
    lSumRec=400000;
    
    
    iSum = 0;
    iMaxRecord = utComGetVar_ld(psShmHead,"MaxNewIpRec",30);
  

   if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * iMaxRecord;


   stService=malloc(500*sizeof(struct serviceonline_s));
   if(stService==NULL){
   	    
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),ncLang("0404内存出错"));
           return 0;
       
   }
   memset(stService,0,500*sizeof(struct serviceonline_s));
   lTime=time(0);
    for(i=0;i<lSumRec;i++) {
 //判断是不是本部门的服务应用   	
    	if(((psIp[i]->psUser)&&(psIp[i]->psUser->lGid==lGroupid0))||lGroupid0==0)
    	{
        if((psIp[i]->lSip > 0)) {
           
         sflag=0;
         for(m=0;m<lSumservice;m++){
         	   if(psIp[i]->lService==stService[m].id){
         	      sflag=1;
         	      stService[m].inbyte=stService[m].inbyte+psIp[i]->lBytes[1];
         	      stService[m].outbyte=stService[m].outbyte+psIp[i]->lBytes[0];
       	      
         	      
        		    break;
        		  }
         }
         if((sflag==0)&&(lSumservice<500)){
         	       		 
        		 stService[lSumservice].id=psIp[i]->lService;
        		 stService[lSumservice].inbyte=psIp[i]->lBytes[1];
         	   stService[lSumservice].outbyte=psIp[i]->lBytes[0];
        
            
         	   lSumservice++;
        }
                           
       }
      }
                       
     }
   

     if(lSumservice>0)
         qsort(stService,lSumservice,sizeof(struct serviceonline_s),ncUtlSortOnService2);

    
    
  

   
    
    
     
   
     iNum=0;
      for(i = 0; i < 10 ;i++) {
 

        
      	strcpy(caService,ncUtlGetServiceName(psShmHead,stService[i].id,"\0"));
      	if(strlen(caService)>0){      		
       lSumbytes=lSumbytes+stService[i].outbyte+stService[i].inbyte;		
       utPltPutLoopVarF(psDbHead,"id",iNum+1,"%d",stService[i].id);
        utPltPutLoopVar(psDbHead,"service", iNum+1,ncUtlGetServiceName(psShmHead,stService[i].id,"\0"));
     
        utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(stService[i].inbyte,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte,caTemp));
        
        utPltPutLoopVarF(psDbHead, "subytes2",iNum+1,"%llu",stService[i].outbyte+stService[i].inbyte); 
        utPltPutLoopVarF(psDbHead, "subytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte+stService[i].inbyte,caTemp));  
              
              
        utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);  
 
        
       iNum++;
     }
    }
     utPltPutVarF(psDbHead,"TotRec","%d",iNum);
     i=0;
     
     
     
     
   if(strlen(caPdf)>0){
	//表格标签
	 desc=malloc(6*sizeof(char*));
	 for(i=0;i<6;i++){
	 	desc[i]=malloc(32);
   }
   strcpy(desc[0],ncLang("1141网络服务名"));
   strcpy(desc[1],ncLang("1876进流量(字节)"));
   strcpy(desc[2],ncLang("1877出流量(字节)"));
   sprintf(desc[4],"%s(%s)",ncLang("1103总流量"),ncLang("0882字节"));
   strcpy(desc[3],ncLang("1821带宽(bps)"));
   strcpy(desc[5],ncLang("1827百分比%"));
     
 //输出到PDF中     
   pChart=(struct piechart_datasetf*)malloc(sizeof(struct piechart_datasetf)*(iNum+2));
    if(pChart==NULL){
  	  utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0404内存出错"));
      return 0;
   }
  
 
   	mm=0;
    for(i = 0; i < lSumservice && i< iMaxRecord;i++) {
   	   strcpy(caService,ncUtlGetServiceName(psShmHead,stService[i].id,"\0"));
      	if(strlen(caService)>0&&mm<=10){  

   	 	
   	 			pChart[mm].name=strdup(caService);  
   	 			
   	 			if(lSumbytes==0){
   	 				sprintf(caTemp,"0.00");
   	 			}
   	 			else{
   	 			  sprintf(caTemp,"%0.2f",(float)100*(stService[i].outbyte+stService[i].inbyte)/lSumbytes);
   	 			}
 	 		
   	 			pChart[mm].percent=strdup(caTemp); 
   
   	 			pChart[mm].amount=(float)(stService[i].outbyte+stService[i].inbyte); 	
  			 
          pChart[mm].append=malloc(3*sizeof(char*));
          for(m=0;m<3;m++){
          	pChart[mm].append[m]=malloc(32);
          }
          strcpy(pChart[mm].append[0],utStrLtoF8(stService[i].inbyte,caTemp));
          strcpy(pChart[mm].append[1],utStrLtoF8(stService[i].outbyte,caTemp));
   	 		  strcpy(pChart[mm].append[2],utStrLtoF8(stService[i].band*8,caTemp));   
  	 		
        mm++;
        
      }
        
        
        
     
    }
     

     
     
     
     
     
      rowrow=mm;
 //  printf("rowrow=%d\n",rowrow);
    strcpy(caTitle,ncLang("1878实时网络服务流量"));  
    sprintf(filename,"%s.PDF",caTitle); 
    sprintf(caStatdate,ncLang("1879统计时间：%s"),	utTimFormat("%Y-%m-%d %H:%M:%S",time(0)));	

//printf("pid=%d\n",getpid());
//sleep(20);

     ncWebShowServiceClassFirst_PDF(iFd,pChart,desc,rowrow,6,3, caTitle,ncLang("1880单位(字节)"),caStatdate,filename); 
    
//    printf("aaaaa\n"); 
//释放内存     
   for(i=0;i<rowrow;i++){
	      free(pChart[i].name);
	      free(pChart[i].percent);
	      for(m=0;m<3;m++){
	      	free(pChart[i].append[m]);
	      }
	      if(pChart[i].append) free(pChart[i].append);
   }  
   if(pChart) free(pChart);
   
   for(i=0;i<6;i++){
   	   free(desc[i]);
   }
    if(desc) free(desc);
  
   
   
   
   
   
     utPltFreeDb(psDbHead);
     free(psIp);
     free(stService);
     return 0;
    }  
     
     
     
     
     
     
//    printf("caFlag=%s\n",caFlag); 
     
     
     
     
    free(psIp);
    free(stService);
    

    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncuserviceonlinestat_first.htm");
    
    return 0;
}



//显示首页在线用户即时流量

int ncShowOnlineuserBand(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    unsigned long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
   long lband;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo,**psUser;
    char       *pHash;
    pasHashInfo sHashInfo;
    int pre_id;
    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[16];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
   
    char caTemp[4024];

   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
    pasDbCursor *psCur;
    char caFlag[12];
    char caIp[20];
  unsigned long long lSumFlow,lBytes[2];
  unsigned long lSumTime;
    double  d1,d2;
 	  long lSum,service_flag,m,lService;
 	  char caId[20];
	 struct serviceonline_s *stService;
   int lSumUser;
    char caSort[12];
    char caSear[32],caSearBut[32];
    char caMac[32],caSet[32],caExport[16];
   long index;
   long temp_order,temp_sort;
   unsigned long lCur,lRuntime;
   char caYear[32];
   
   unsigned char *pHash_u;
	 pasLHashInfo  sHashInfo2;
   struct serviceonline_s *psFlow;
   
   lCur=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    
    
    if(iReturn != 0) lGroupid0 = 0;
   
    lSumUser=0;
   

  


//sleep(15);

    pHash = utShmHashHead(psShmHead,NC_LNK_USERINFO);
    if(pHash == NULL) {

        return 0;
    }
   lSumUser=0;
   lSumUser = utShmHashRecord(psShmHead,NC_LNK_USERINFO);
    
   stService=malloc((lSumUser+1)*sizeof(struct serviceonline_s));
   if(stService==NULL){
   	    
  
           return 0;
       
   }
   memset(stService,0,(lSumUser+1)*sizeof(struct serviceonline_s));
 


    	pHash_u = (unsigned char *)pasLHashInit(40000,40000,sizeof(struct serviceonline_s),0,4); 
			if(pHash_u == NULL) {
		        printf(" Memory HttpHash Error \n");
		        return (-1);
		  }
		  
    psUserInfo = (ncUserInfo *)pasHashFirst(pHash,&sHashInfo);
     j = 0;


//带宽取用户信息中的记录，进出流量取IPLOG记录
  lSum=0;
   for(i=0;i<lSumUser && psUserInfo;i++) {
   //     strcpy(caIp,utComHostIp(htonl(psUserInfo->lIp)));
    //    strcpy(caMac,ncCvtMac(psUserInfo->mac));
//        printf("ip=%s\n",caIp);
      //  if((psUserInfo->lBytes[0] + psUserInfo->lBytes[1] > 0 ) {
        psFlow = (struct serviceonline_s *)pasLHashLookA(pHash_u,&psUserInfo->lIp);
        if(psFlow){
          psFlow->id=psUserInfo->lId;
            	
  //           	stService[j].id=psUserInfo->lId;
   //          	stService[j].inbyte=0;
   //          	stService[j].outbyte=0;
   //          	stService[j].lasttime=0;
    //         	strcpy(stService[j].caIp,caIp);
  //计算带宽           	
             	lBytes[0] = psUserInfo->sCount.lSBytes[0] + psUserInfo->sCount.lCBytes[0] ;
   	          lBytes[1] = psUserInfo->sCount.lSBytes[1] + psUserInfo->sCount.lCBytes[1];
              lSumTime = time(0) - psUserInfo->sCount.lSTime; 	
       				if(lSumTime < 15) {  
       	    			lBand = (lBytes[0] + lBytes[1]) / 15;
       				}
       				else {
       	    			lBand = (lBytes[0] + lBytes[1]) / lSumTime;
        			}
        			psFlow->band=lBand;
//             	stService[j].band=lBand;
             	
 //            	j++;
        }
        psUserInfo = (ncUserInfo *)pasHashNext(&sHashInfo);
    }
 
 
 
 

 
 
   

 //按用户ID进行排序，为二分法累计流量做准备  
// temp_sort=iSort;
// temp_order=iOrder;
 //  iOrder=1;
 //  iSort=4;
 //  qsort(stService,lSum,sizeof(struct serviceonline_s),ncUtlSortOnService3); 
 
 //iSort=temp_sort;
 //iOrder=temp_order;   
    
   lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
   
   lCur=lMaxRec; 
   if(lMaxRec>1000000)
    lMaxRec=1000000;
    

    psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
   
   
    if(psIp == NULL) {

        return 0;
    }        
    lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    
    if(lSumRec>1000000)
    lSumRec=1000000;
    

    iSum = 0;
 
//在IP记录中统计用户的进出流量

   iSum=0;
    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&(psIp[i]->cStatus!=NC_USER_NOMON)&&(psIp[i]->psUser)) {   
  //      strcpy(caIp,utComHostIp(htonl(psIp[i]->psUser->lIp)));

        psFlow = (struct serviceonline_s *)pasLHashLook(pHash_u,&psIp[i]->lSip);
        if(psFlow){
        	psFlow->inbyte=psFlow->inbyte+psIp[i]->lBytes[1];
        	psFlow->outbyte=psFlow->outbyte+psIp[i]->lBytes[0];
        	 if(psIp[i]->lLastTime>psFlow->lasttime){
	          	psFlow->lasttime=psIp[i]->lLastTime;
	          }
         }

         } 
      } 
       psFlow= (struct serviceonline_s *)pasLHashFirst(pHash_u,&sHashInfo2);
       lSum=0;
       while(psFlow){
       	  stService[lSum].id=psFlow->id;
       	  stService[lSum].inbyte=psFlow->inbyte;
       	  stService[lSum].outbyte=psFlow->outbyte;
       	  stService[lSum].lasttime=psFlow->lasttime;
       	  stService[lSum].sip=psFlow->sip;
       	  stService[lSum].band=psFlow->band;
       	  lSum++;
       	psFlow = (struct serviceonline_s *)pasLHashNextS(&sHashInfo2);
      }
      
      //lSum为在线用户总数

    if(lSum <= 0) {
    	psDbHead = utPltInitDb();
    	
    	  iSum=0;
    	  i=0;
        while(iSum < 10) {
	        i++;
	        iSum++;
	        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iSum);
	    	}       
    	free(stService);
      free(pHash_u);
      free(psIp);
       pasDbOneRecord("select count(*) from ncuser ",0,UT_TYPE_LONG,4,&lSumUser);
  
		  utPltPutVarF(psDbHead,"maxuser","%lu",lSumUser);
		  utPltPutVarF(psDbHead,"curuser","%lu",lSum);
		  
		    lTime=time(0);
		    lRuntime=getjiffies();
		   
		    strcpy(caYear,utTimFormat(ncLang("1206%Y年%m月%d日 %H:%M:%S"),lTime));
		  
		    utPltPutVarF(psDbHead,"curip","%lu",lCur);
		    utPltPutVar(psDbHead,"curtime",caYear);
		    utPltPutVar(psDbHead,"runtime",ncCvtConntime2(lRuntime));
    
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_usersortflow.htm");
      return 0;
    }
      
      
      
      
      
      
      
      
      for(i=0;i<lSum;i++){
       	if(stService[i].outbyte+stService[i].inbyte==0){
       		stService[i].band=0;
       	}
       	if(stService[i].band==0){
       		stService[i].outbyte=0;
       		stService[i].inbyte=0;
       	}
      }  
        	
     
   
     iNum=0;
     
     if(lSum>0){
     	 temp_sort=iSort;
       temp_order=iOrder;
       iOrder=0;
       iSort=6;
     	
     	
     qsort(stService,lSum,sizeof(struct serviceonline_s),ncUtlSortOnService3); 
     
      iSort=temp_sort;
      iOrder=temp_order; 
    }

      psDbHead = utPltInitDb();   
     
     
      for(i = 0; i < lSum && iNum < 10;i++) {
   
//   printf("serivceid=%d\n",stService[i].id);
   
       psUserInfo = (ncUserInfo *)ncGetUserInfoById(psShmHead,stService[i].id);
       if(psUserInfo){
         strcpy(caIp,utComHostIp(htonl(stService[i].sip)));

        
        if(psUserInfo->lGid==lGroupid0||lGroupid0==0)
        {
        
        
        
         if(psUserInfo->psUser){
           utPltPutLoopVar(psDbHead,"name", iNum+1,psUserInfo->psUser->dispname);
           
         }
        
        
        utPltPutLoopVarF(psDbHead,"num",  iNum+1,"%ld",i+1);

          utPltPutLoopVarF(psDbHead,"userid",iNum+1,"%d",stService[i].id);
          
          
        utPltPutLoopVar(psDbHead,"ip",iNum+1,caIp);
        
        utPltPutLoopVar(psDbHead,"sumbytes", iNum+1,utStrLtoF8(stService[i].inbyte + stService[i].outbyte,caTemp));
       
         		               
        utPltPutLoopVar(psDbHead,"band", iNum+1, utStrLtoF8(stService[i].band*8,caTemp));                        
       iNum++;
      }
     }
    }
 

     i=0;
    while(iNum <10) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
     
            free(psIp);
            free(pHash_u);
            free(stService);
    
   pasDbOneRecord("select count(*) from ncuser ",0,UT_TYPE_LONG,4,&lSumUser);
  
  utPltPutVarF(psDbHead,"maxuser","%lu",lSumUser);
  utPltPutVarF(psDbHead,"curuser","%lu",lSum);
  
    lTime=time(0);
    lRuntime=getjiffies();
   
    strcpy(caYear,utTimFormat(ncLang("1206%Y年%m月%d日 %H:%M:%S"),lTime));
  
    utPltPutVarF(psDbHead,"curip","%lu",lCur);
    utPltPutVar(psDbHead,"curtime",caYear);
    utPltPutVar(psDbHead,"runtime",ncCvtConntime2(lRuntime));
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_usersortflow.htm");
}


//统计首页某用户即时网络流量
 
int ncWebServiceStatByUserIf(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
    ncHttpBuffer *psHttp;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    unsigned long long lSum_unkownin,lSum_unkownout,lSum_unkown;
    char unkownid[2560];
    char caName[128],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;

   strcpy(unkownid,"0");
   lSum_unkownin=0;
   lSum_unkownout=0;
   lSum_unkown=0;
    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[16];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
    int service_flag;
    char caTemp[4024];
  
   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
    pasDbCursor *psCur;
    char caFlag[12];
    char caSumbytes[20];
    long long lSum,lUsum;
 	  long sflag,m;
	 struct serviceonline_s *stService;
   int lSumservice,lService;
   long long lSumbytes,lUbytes,lDbytes,lTot;
   
   char sumbytes_desc[256];
   
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
//printf("begin\n");
  lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
  if(lMaxRec>200000)
  lMaxRec=200000;
 

    
  psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
  lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    if(lSumRec>200000)
    lSumRec=200000;
    
//printf("lSumRec=%d\n",lSumRec);   
    
   stService=malloc(500*sizeof(struct serviceonline_s));
   if(stService==NULL){
   	    
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),ncLang("0404内存出错"));
           return 0;
       
   }
    memset(stService,0,500*sizeof(struct serviceonline_s));

//printf("lSumservice=%d\n",lSumservice);
    iReturn = utMsgGetSomeNVar(psMsgHead,2,"userid",    UT_TYPE_STRING, 10, caUserid,
                                            "sum",      UT_TYPE_STRING, 18,caSumbytes);
                                            
    lSum=atoll(caSumbytes);
    
 //   printf("lSum=%llu\n",lSum);                                    
                                            
    lSumservice=0;
       lTime=time(0);
    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&(psIp[i]->cStatus!=NC_USER_NOMON)&&psIp[i]->psUser&&atol(caUserid)==psIp[i]->psUser->lId) {
           
         sflag=0;
         for(m=0;m<lSumservice;m++){
         	   if(psIp[i]->lService==stService[m].id){
         	      sflag=1;
//         	  printf("m=%d\n",m);
         	      stService[m].inbyte=stService[m].inbyte+psIp[i]->lBytes[1];
         	      stService[m].outbyte=stService[m].outbyte+psIp[i]->lBytes[0];
        	      
         	      
        		    break;
        		  }
         }
         if((sflag==0)&&(lSumservice<500)){
        		 
        		 stService[lSumservice].id=psIp[i]->lService;
        		 stService[lSumservice].inbyte=psIp[i]->lBytes[1];
         	   stService[lSumservice].outbyte=psIp[i]->lBytes[0];
         	        	 
         	   lSumservice++;
         }
                           
       }
          
     }
    

//printf("midd\n");    
	psDbHead = utPltInitDb();
	  psUserInfo = (ncUserInfo *)ncGetUserInfoById(psShmHead,atol(caUserid));
	 if(psUserInfo&&atol(caUserid)>0){
	 	
	 	
	 	  utPltPutVar(psDbHead,"stime",utTimFormat("%Y-%m-%d %H:%M:%S",psUserInfo->lStartTime));
	 	  utPltPutVar(psDbHead,"etime",utTimFormat("%Y-%m-%d %H:%M:%S",psUserInfo->lLastTime));
	 	  utPltPutVar(psDbHead,"userid",caUserid);
     	utPltPutVarF(psDbHead,"lstime","%d",psUserInfo->lStartTime);
	 	  utPltPutVarF(psDbHead,"letime","%d",psUserInfo->lLastTime);
      utPltPutVar(psDbHead,"dispname",psUserInfo->psUser->dispname);
 
    
// printf("lSumervice=%d\n",lSumservice);     
       if(lSumservice>0)
         qsort(stService,lSumservice,sizeof(struct serviceonline_s),ncUtlSortOnService2);

//printf("fffffff\n");         
        iNum=0;
        
        lTot=0;
        for(i=0;i<lSumservice;i++){
        	strcpy(caService,ncUtlGetServiceName(psShmHead,stService[i].id,"\0"));

        			lTot=lTot+stService[i].inbyte+stService[i].outbyte;

      }
       if(lTot<lSum){
       	lTot=lSum;
       } 
       else{
       	lSum=lTot;
      }
   utPltSetCvtHtml(1);    
   lUsum=0;
    
       for(i=0;i<lSumservice;i++){
       	strcpy(caService,ncUtlGetServiceName(psShmHead,stService[i].id,"\0"));
      	if(strlen(caService)>0){
      		
      		iNum++; 
       	utPltPutLoopVarF(psDbHead,"iNum",iNum,"%lu",iNum);
       	lSumbytes=stService[i].inbyte+stService[i].outbyte;
	      utPltPutLoopVarF(psDbHead,"sumbytes",iNum,"%llu",lSumbytes);
	      
	      lUsum=lUsum+lSumbytes;
	      
	      utPltPutLoopVar(psDbHead,"sumbytes_1",iNum,utStrLtoF8(lSumbytes,caTemp));
	      
	      utPltPutLoopVar(psDbHead,"name",iNum,caService);
	      
	      utPltPutLoopVarF(psDbHead,"service",iNum,"%d",stService[i].id);
	      
	      utPltPutLoopVarF(psDbHead,"stime",iNum,"%d",psUserInfo->lStartTime);
	 	    utPltPutLoopVarF(psDbHead,"etime",iNum,"%d",psUserInfo->lLastTime);
	 	    utPltPutLoopVar(psDbHead,"userid",iNum,caUserid);
	      
	      
	      
	      
	      if(lTot==0)
	        utPltPutLoopVar(psDbHead,"rate",iNum,"100");
	      else
	        utPltPutLoopVarF(psDbHead,"rate",iNum,"%0.2f",(float)lSumbytes/lTot*100);
	     
	      	
	      utPltPutLoopVarF(psDbHead,"name_desc",iNum,"%-16s",caService);
	      if(lSumbytes>1024*1024)
	        utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum,"%8.02fM",(float)lSumbytes/(1024*1024));
	      else if(lSumbytes>1024)
	        utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum,"%8.02fK",(float)lSumbytes/(1024)); 
	     else
          utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum,"%9d",lSumbytes); 
          
        if(stService[i].inbyte>1024*1024)  
          utPltPutLoopVarF(psDbHead,"inbytes",iNum,"%8.02fM",(float)stService[i].inbyte/(1024*1024));
        else if(stService[i].outbyte>1024)
          utPltPutLoopVarF(psDbHead,"inbytes",iNum,"%8.02fK",(float)stService[i].inbyte/(1024));
        else
          utPltPutLoopVarF(psDbHead,"inbytes",iNum,"%9d",stService[i].inbyte);
          
          
        if(stService[i].outbyte>1024*1024)  
          utPltPutLoopVarF(psDbHead,"outbyte",iNum,"%8.02fM",(float)stService[i].outbyte/(1024*1024));
        else if(stService[i].outbyte>1024)
          utPltPutLoopVarF(psDbHead,"outbyte",iNum,"%8.02fK",(float)stService[i].outbyte/(1024));
        else
          utPltPutLoopVarF(psDbHead,"outbyte",iNum,"%9d",stService[i].outbyte);
          
          utPltPutLoopVarF(psDbHead,"num",iNum,"%d",iNum-1);
	      }
	      else{
	      	lSum_unkownin=lSum_unkownin+stService[i].inbyte;
	      	lSum_unkownout=lSum_unkownout+stService[i].outbyte;
	      	lSum_unkown=lSum_unkown+stService[i].inbyte+stService[i].outbyte;
	      	sprintf(unkownid+strlen(unkownid),",%d",stService[i].id);
	      	
	      }
      }
       utPltSetCvtHtml(0);  
       
    }
   
     free(psIp);
     free(stService);
 lSum_unkown=lSum-lUsum;
 if(lSum_unkown>0){
 	utPltSetCvtHtml(1);  
 	
       utPltPutLoopVar(psDbHead,"service",iNum+1,unkownid);
       utPltPutLoopVarF(psDbHead,"iNum",iNum+1,"%lu",iNum+1);
     
     	  if(lTot==0)
	        utPltPutLoopVar(psDbHead,"rate",iNum+1,"100");
	      else
	        utPltPutLoopVarF(psDbHead,"rate",iNum+1,"%0.2f",(float)lSum_unkown/lTot*100);
          utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);
     
     	  utPltPutLoopVarF(psDbHead,"sumbytes",iNum+1,"%llu",lSum_unkown);
     	  
     	  utPltPutLoopVar(psDbHead,"sumbytes_1",iNum+1,utStrLtoF8(lSum_unkown,caTemp));
     	  
     	  
     	  
	      utPltPutLoopVar(psDbHead,"name",iNum+1,ncLang("1490未知服务"));
     
      utPltPutLoopVarF(psDbHead,"name_desc",iNum+1,"%-16s",ncLang("1490未知服务"));
     
        if(lSum_unkownout>1024*1024)  
          utPltPutLoopVarF(psDbHead,"outbyte",iNum+1,"%8.02fM",(float)lSum_unkownout/(1024*1024));
        else if(lSum_unkownout>1024)
          utPltPutLoopVarF(psDbHead,"outbyte",iNum+1,"%8.02fK",(float)lSum_unkownout/(1024));
        else
          utPltPutLoopVarF(psDbHead,"outbyte",iNum+1,"%9d",iNum+1,lSum_unkownout);
     
    
        if(lSum_unkownin>1024*1024)  
          utPltPutLoopVarF(psDbHead,"inbytes",iNum+1,"%8.02fM",(float)lSum_unkownin/(1024*1024));
        else if(lSum_unkownin>1024){
        	
          utPltPutLoopVarF(psDbHead,"inbytes",iNum+1,"%8.02fK",(float)lSum_unkownin/(1024));
        }
        else
          utPltPutLoopVarF(psDbHead,"inbytes",iNum+1,"%9d",lSum_unkownin);
 
     
        if(lSum_unkown>1024*1024)  
          utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum+1,"%8.02fM",(float)lSum_unkown/(1024*1024));
        else if(lSum_unkown>1024)
          utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum+1,"%8.02fK",(float)lSum_unkown/(1024));
        else
          utPltPutLoopVarF(psDbHead,"sumbytes_desc",iNum+1,"%9d",lSum_unkown);
     utPltSetCvtHtml(0);  
     
     	   utPltPutLoopVarF(psDbHead,"stime",iNum+1,"%d",psUserInfo->lStartTime);
	 	    utPltPutLoopVarF(psDbHead,"etime",iNum+1,"%d",psUserInfo->lLastTime);
	 	    utPltPutLoopVar(psDbHead,"userid",iNum+1,caUserid);
     
    }
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_servicestatbtbyuser.htm");
  
    return 0;
}




//统计每个用户即时网络服务流量
 
int ncWebOnlieServiceUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int i,iNum,iReturn,j;
    int iCount;
    int iSum;
    int iUserFlags;
    long lMaxRec,iMaxRecord;
    long lUserid;
    long lSumRec;
    ncIp **psIp,**psIp1;
    int pre_id;
    unsigned long lTime,lTime2;
    unsigned long long lSumBytes,lSumBand,lBand,lRate;
    char caName[32],caWay[12];
    char caPlate[64];
	char caGroupid[32];
    char caUserid[32];
    long lGroupid0;
	long lGroupid1;
	double dFlow;
    char caFilter[256];
    utPltDbHead *psDbHead;
    ncUserInfo  *psUserInfo;

    int    iSumNet;
    lTime = time(0);
    iUserFlags = 0;
    lSumBytes = 0;
    char caService[16];
    char caUsername[64];
    char caTitlename[32];
    long lRowNum,lCurpg,lTotpg,lStartRec;
    char caCurpg[12],caOrder[12];
   
    char caTemp[4024];

   	long lGid;
   	char caGid0[20];
   	char caExp[20];
   	char temp_port[20],temp_in[20],temp_dip[20];
    pasDbCursor *psCur;
    char caFlag[12];
    char caIp[20];
 	  long lSum,service_flag,m,lService;
 	  char caId[20];
	 struct serviceonline_s *stService;
   int lSumUser;
   struct ncSflow_s  sGservice[600];
   long lSumGindex;
   float fRate;
   long lBsteptime;
   long lGroupid,lSid,index;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);   
    if(iReturn != 0) lGroupid0 = 0;



  lBsteptime=atol(utComGetVar_sd(psShmHead,"BandSteptime","15"));

    lSumUser=0;
    iReturn = utMsgGetSomeNVar(psMsgHead,3,
		"service",       UT_TYPE_STRING, 10, caService,
		"CurPg",   UT_TYPE_STRING, 12, caCurpg,
		"flag",    UT_TYPE_STRING,6,   caFlag);
	
	if(atol(caFlag)==0){
		 //将服务信息放在sGservice 中
   	 psCur=pasDbOpenSql("select sid,groupid,name from ncipindex order by sid  limit 0,600 ",0);
  	  if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0746用户查询"),ncLang("0171查询数据出错"));
        return 0;
     }
     lSumGindex=0;
     
     lGroupid=0;
     memset(caName,0,sizeof(caName));
     lSid=0;
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
     																UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
          
      while((iReturn==0)||(iReturn==1405))
      {
      	sGservice[lSumGindex].lService=lSid;
      	sGservice[lSumGindex].lGroupid=lGroupid;
      	strcpy(sGservice[lSumGindex].caGname,caName);
      	sGservice[lSumGindex].lBytes=0;

      	lSumGindex++;
      	lGroupid=0;
      	lSid=0;
        memset(caName,0,sizeof(caName));
     iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
     																UT_TYPE_LONG,4,&lGroupid,
                                    UT_TYPE_STRING,30,caName);
      }                               
      pasDbCloseCursor(psCur); 
	 }
	
		
	 lService=atol(caService);
   lSumUser = utShmHashRecord(psShmHead,NC_LNK_USERINFO);
    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
    if(lMaxRec <= 0) {
    	 psDbHead = utPltInitDb();
    	 
    	  iSum=0;
    	  i=0;
        while(iSum < 30) {
	        i++;
	        iSum++;
	        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iSum);
	    	}       
    	 
    	 utPltPutVarF(psDbHead,"CurPg","%d",1);
       utPltPutVarF(psDbHead,"TotPg","%d",1);
       utPltPutVarF(psDbHead,"TotRec","%d",0);
       utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncuserviceonlineuser.htm");
    
        return 0;
    }
   if(lMaxRec>200000)
    lMaxRec=200000;
    

    psIp = (ncIp **) malloc(lMaxRec * sizeof(ncIp *));
    
   
    if(psIp == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","Current Ip",
                               "Memory Error");
        return 0;
    }   
    
    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"service",caService);
    utPltPutVar(psDbHead,"flag",caFlag);
    lSumRec = ncWebGetCurIpVister(psShmHead,psIp,lMaxRec,&lSumBytes,&lSumBand);
    
    if(lSumRec>200000)
    lSumRec=200000;
    
    psIp1 = (ncIp **) malloc((lSumRec+2) * sizeof(ncIp *));
    iSum = 0;
    iSum = 0;
    iMaxRecord = utComGetVar_ld(psShmHead,"MaxNewIpRec",30);
  

   if(utStrIsSpaces(caCurpg)) {
        lCurpg = 1;
    }
    else {
        lCurpg = atol(caCurpg);
    }
    lStartRec = (lCurpg - 1) * iMaxRecord;


   stService=malloc((lSumUser+1)*sizeof(struct serviceonline_s));
   if(stService==NULL){
   	    
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),ncLang("0404内存出错"));
           return 0;
       
   }
   memset(stService,0,(lSumUser+1)*sizeof(struct serviceonline_s));
   lSum=0;
   iSum=0;
    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&(psIp[i]->cStatus!=NC_USER_NOMON)&&(psIp[i]->psUser)) {   
        sprintf(caId,"%d",psIp[i]->lService);
        
        service_flag=0;
        if(atol(caFlag)==0){
        	 index=ncUtlGetServicenumById(&sGservice,lSumGindex,psIp[i]->lService);
        	 if(index!=-1){
        	 	if(lService==sGservice[index].lGroupid)
        	 	 service_flag=1;
        	 }
        }       
        else {
          	if(psIp[i]->lService==lService){
          		service_flag=1;
          	}
          }
          
      
          if(service_flag==1){         	
                 psIp1[iSum]=psIp[i];
        		     iSum++;
            }
          }
       }
       iOrderip=10;
 //printf("iSum=%d\n",iSum);
       qsort(psIp1, iSum,sizeof(ncIp *),ncUtlSortOnIp1);        
        pre_id=-1;
        lSum=0;
        lTime=time(0);
        for(i=0;i<iSum;i++){
        	if((lSum<lSumUser)&&(psIp1[i]->psUser)){
        	if(pre_id!=psIp1[i]->psUser->lId){
        		stService[lSum].id=psIp1[i]->psUser->lId;
        		stService[lSum].inbyte=psIp1[i]->lBytes[1];
            stService[lSum].outbyte=psIp1[i]->lBytes[0];
            stService[lSum].lasttime=psIp1[i]->lLastTime;
    //      printf("id=%d,lastime=%d,lCurtime=%d,lStime=%d,byte=%llu,sid=%d\n",psIp1[i]->psUser->lId,psIp1[i]->lLastTime,lTime,psIp1[i]->lStartTime,psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1],psIp1[i]->lService);                 	
            if(psIp1[i]->lStartTime > 0 &&psIp1[i]->lLastTime>0&& (lTime - psIp1[i]->lStartTime > lBsteptime)) {
            	    if((lBsteptime+psIp1[i]->lLastTime>lTime)&&(psIp1[i]->lLastTime>psIp1[i]->lStartTime)){
         	    		if(lBsteptime+psIp1[i]->lLastTime>lTime){
         	    			
         	    		
         	    			
         	    		   fRate=(float)(lBsteptime+psIp1[i]->lLastTime-lTime)/lBsteptime;
         	    		  }
         	    		  else{
         	    		  	fRate=0;
         	    		  }
 
 //        	    		printf("1fRate=%f\n",fRate);
                   if(psIp1[i]->lLastTime - psIp1[i]->lStartTime>0){
                        stService[lSum].band = (psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1])*fRate/(psIp1[i]->lLastTime - psIp1[i]->lStartTime);
                    }
                  }
                  else{
   //               	stService[lSum].band= (psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) /lBsteptime;
                  }
            }
             else {
                  stService[lSum].band= (psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) /lBsteptime;
            }
            pre_id=psIp1[i]->psUser->lId;
            lSum++;
        	} 
        	else{

        		stService[lSum-1].inbyte=stService[lSum-1].inbyte+psIp1[i]->lBytes[1];
            stService[lSum-1].outbyte=stService[lSum-1].outbyte+psIp1[i]->lBytes[0];
           	if(stService[lSum-1].lasttime<psIp1[i]->lLastTime)
              	   	stService[lSum-1].lasttime=psIp1[i]->lLastTime;
              	   	
  //           printf("2id=%d,lastime=%d,lCurtime=%d,lStime=%d,byte=%llu,sid=%d\n",psIp1[i]->psUser->lId,psIp1[i]->lLastTime,lTime,psIp1[i]->lStartTime,psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1],psIp1[i]->lService);      
                if(psIp1[i]->lStartTime > 0&&psIp1[i]->lLastTime>0 && (lTime - psIp1[i]->lStartTime > lBsteptime)) {
         	    	if((lBsteptime+psIp1[i]->lLastTime>lTime)&&(psIp1[i]->lLastTime>psIp1[i]->lStartTime)){
         	    		if(lBsteptime+psIp1[i]->lLastTime>lTime){
         	    			

       //  	          printf("lBsteptime=%d\n",lBsteptime);
         	    			
         	    		   fRate=(float)(lBsteptime+psIp1[i]->lLastTime-lTime)/lBsteptime;
         	    		  }
         	    		  else{
         	    		  	fRate=0;
         	    		  }
   //    	    		printf("2fRate=%f\n",fRate);
                   stService[lSum-1].band = stService[lSum-1].band+(psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1])*fRate/(psIp1[i]->lLastTime - psIp1[i]->lStartTime);
                  }
                  else{
 //                 	stService[lSum-1].band= stService[lSum-1].band+(psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) /lBsteptime;
                  }
               
              }
             	else {
                stService[lSum-1].band= stService[lSum-1].band+(psIp1[i]->lBytes[0] + psIp1[i]->lBytes[1]) /lBsteptime;
             	}
             
             
             
              	   	
        	}
        }
       }
       
       
       
        
          
          
             
// printf("lSum=%d\n",lSum);           
              
      lTotpg = (lSum - 1) / iMaxRecord + 1; 
      utPltPutVarF(psDbHead,"CurPg","%d",lCurpg);
      utPltPutVarF(psDbHead,"TotPg","%d",lTotpg);
     
     
   
     iNum=0;
     
     if(lSum>0)
     qsort(stService,lSum,sizeof(struct serviceonline_s),ncUtlSortOnService2);
      
printf("aaaaa\n");
 
      for(i = lStartRec; i < lSum && iNum < iMaxRecord;i++) {
   
//   printf("serivceid=%d\n",stService[i].id);
 //  printf("inbytes=%d\n",stService[i].inbyte);

       psUserInfo = (ncUserInfo *)ncGetUserInfoById(psShmHead,stService[i].id);
       if(psUserInfo){
       	if(psUserInfo->lGid==lGroupid0||lGroupid0==0)
        {
  utPltSetCvtHtml(1);    	
         strcpy(caIp,utComHostIp(htonl(psUserInfo->lIp)));
      	 if(psUserInfo->psUser){
           utPltPutLoopVar(psDbHead,"dispname", iNum+1,psUserInfo->psUser->dispname);
           if(psUserInfo->psUser->psGroup) {
                utPltPutLoopVar(psDbHead,"dept", iNum+1,psUserInfo->psUser->psGroup->groupname);
         }
         }
  utPltSetCvtHtml(0);
        utPltPutLoopVarF(psDbHead,"userid",iNum+1,"%d",stService[i].id);
        utPltPutLoopVar(psDbHead,"id",iNum+1,caService);
        utPltPutLoopVar(psDbHead,"ip",iNum+1,caIp);
        utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(stService[i].inbyte,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte,caTemp));
        
        utPltPutLoopVarF(psDbHead, "subytes2",iNum+1,"%llu",stService[i].outbyte+stService[i].inbyte); 
        utPltPutLoopVarF(psDbHead, "subytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte+stService[i].inbyte,caTemp));                 
        utPltPutLoopVar(psDbHead,"flag",iNum+1,caFlag);                
        utPltPutLoopVar(psDbHead,"band", iNum+1, utStrLtoF8(8*stService[i].band,caTemp));                         
       iNum++;
       
      }
       
     }
    }
     utPltPutVarF(psDbHead,"TotRec","%d",lSum);
     i=0;
    while(iNum < iMaxRecord) {
        i++;
        iNum++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iNum);
    }
//printf("bbbbb\n");     
    free(psIp);
    free(stService);
    free(psIp1);
  
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/ncuserviceonlineuser.htm");
    
    return 0;
}
