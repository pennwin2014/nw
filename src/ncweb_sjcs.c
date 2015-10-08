#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "ncdef.h"
#include "pasdb.h"
#include "ncreport.h" 

/* 支持无穷级部门的用户树  */
/*
int ncWebTreeDispGroup_nolimit_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caLevel[16];
    char caPlate[128];
   	char caGroupname[36],caGroupid[16];
   	
   	char caName[32];
   	char caWhere[256];
   	char caSql[256];
   	char caCurPg[16],caTotRec[16];
   	char caPid[16];
   	unsigned long lLevel,lPid;
   	int  iNum;
  	char caFilter[256];
  	char caNo[16];
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec;
    utPltDbHead *psDbHead;
   	int iReturn,i;
   	char caVar[256],caFname[64],caBfname[64];
   	char caValue[256];
   	char caVar1[32];
   	char *p,*pVar;
   	char caOrderby[32];
#ifdef LDEBUG1 
    utMsgPrintMsg(psMsgHead);
#endif  	
   	iReturn = utMsgGetSomeNVar(psMsgHead,7,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"level",      UT_TYPE_STRING,10,caLevel,
					"groupid",        UT_TYPE_STRING,10,caPid,
					"num",        UT_TYPE_STRING,10,caNo,
					"var",        UT_TYPE_STRING,255,caVar,
					"fname",      UT_TYPE_STRING,63,caFname,
					"bfname",     UT_TYPE_STRING,63,caBfname);
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"System Error");
        return 0;
    }
    
    strcpy(caOrderby,utComGetVar_sd(psShmHead,"grouporder","groupid"));
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
   if(iReturn == 0 && lGroupid0 > 0) {
        sprintf(caWhere,"and (groupid = %lu or pid in(select groupid from ncgroup where groupid=%lu or pid=%lu)) ",lGroupid0,lGroupid0,lGroupid0);
    }
 printf("caWhere=%s\n",caWhere);   
  //  sprintf(caWhere+strlen(caWhere)," and level=%d ",atol(caLevel));

//    if(strlen(caPid)>0)
//      sprintf(caWhere+strlen(caWhere)," and pid=%d and level>0",atol(caPid));
//    else{
//    	 sprintf(caWhere+strlen(caWhere)," and pid=0 and level=0",atol(caPid));
//    }
    lRowNum = utComGetVar_ld(psShmHead,"GroupTreeRow",1500);
    lStartRec = (lCurPg - 1) * lRowNum;

    psCur = pasDbOpenSqlF("select groupname,groupid,level,pid from ncgroup where 1=1 %s  order by %s  limit %lu,%lu",caWhere,caOrderby,0,lRowNum);
    if(psCur == NULL) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    
      if(!utStrIsSpaces(caVar)) {
        p = caVar;
        strcpy(caValue,"\0");
        while(*p) {
            p =  utStrGetWord(p,caVar1,31,",;");
            if(!utStrIsSpaces(caVar1)) {
                if(pVar=utMsgGetVar_s(psMsgHead,caVar1)) {
                    utStrAddF(caValue,"&%s@%s",caVar1,pVar);
                }
                if(*p != 0) p++;
            }
            else {
                break;
            }
        }
// printf(" caValue == [%s] Var=%s\n",caValue,caVar);
        utPltPutVar(psDbHead,"par",caValue);
     }
    
    
    utPltPutVar(psDbHead,"fname",caFname);
    utPltPutVar(psDbHead,"var",caVar);
    utPltPutVar(psDbHead,"bfname",caBfname);
    memset(caGroupname,0,sizeof(caGroupname));
    lGroupid=0;
    lLevel=0;
    lPid=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,32,caGroupname,
                                   UT_TYPE_LONG,4,&lGroupid,
                                   UT_TYPE_LONG,4,&lLevel,
                                   UT_TYPE_LONG,4,&lPid);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caFname)) utPltPutVar(psDbHead,"fname",caFname);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        
  //      if((lLevel==0)&&(lPid==0))
          utPltPutLoopVarF(psDbHead,"nodep",iNum,"%d",lPid);
 //        else
 //         utPltPutLoopVarF(psDbHead,"nodep",iNum,"%d",lPid);
        if(lGroupid==0)
        	utPltPutLoopVarF(psDbHead,"nodes",iNum,"M%d",lGroupid);
        else
          utPltPutLoopVarF(psDbHead,"nodes",iNum,"%d",lGroupid);
        
        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum-1);
        utPltPutLoopVar(psDbHead,"groupname", iNum,caGroupname);
        utPltPutLoopVarF(psDbHead,"groupid",   iNum,"%ld",lGroupid);
        utPltPutLoopVarF(psDbHead,"level",iNum,"%d",lLevel);
        utPltPutLoopVarF(psDbHead,"pid",iNum,"%d",lPid);
        utPltPutLoopVar(psDbHead,"plate",    iNum,caPlate);
        utPltPutLoopVar(psDbHead,"fname",iNum,caFname);
		    memset(caGroupname,0,sizeof(caGroupname));
		    lGroupid=0;
		    lLevel=0;
		    lPid=0;
		    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,32,caGroupname,
		                                   UT_TYPE_LONG,4,&lGroupid,
		                                   UT_TYPE_LONG,4,&lLevel,
		                                   UT_TYPE_LONG,4,&lPid);
    }
    pasDbCloseCursor(psCur);
   
    utPltPutVar(psDbHead,"rootname",utComGetVar_sd(psShmHead,"rootname",ncLang("0269公司")));
    
   
    utPltPutVar(psDbHead,"no",caNo);
    utPltPutVar(psDbHead,"level",caLevel);
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}
*/
 static int iSort,iOrder;
  struct serviceonline_s{
 		  unsigned long id;
 		  long long inbyte;
 		  long long outbyte;
 		  unsigned long lasttime;
 		  long long band;
 	  };
 	  
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

/* 	  
 	  
int ncWebShowUserFlow_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
   char caFlag[8];
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
    lSumUser=0;
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(iReturn != 0) lGroupid0 = 0;
   	iReturn = utMsgGetSomeNVar(psMsgHead,7,
   	                "cond",UT_TYPE_STRING, 20, caSear,
   	                "search",    UT_TYPE_STRING, 15, caSearBut,
					"CurPg",   UT_TYPE_STRING, 12, caCurpg,
					"sortby",  UT_TYPE_STRING, 2,  caSort,
					"orderby",   UT_TYPE_STRING, 2, caOrder,
					"export",    UT_TYPE_STRING,11,caExport,
					"flag",    UT_TYPE_STRING,2,caFlag);

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
             	
             	stService[j].id=psUserInfo->lId;
             	stService[j].inbyte=0;
             	stService[j].outbyte=0;
             	stService[j].lasttime=0;
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
             	stService[j].band=lBand;
             	
             	j++;
        }
      }
        psUserInfo = (ncUserInfo *)pasHashNext(&sHashInfo);
    }
 
 
 
 
 
 
 
   
//lSum为在线用户总数
    lSum=j;
    if(lSum <= 0) {
    	psDbHead = utPltInitDb();
    	
    	  iSum=0;
    	  i=0;
        while(iSum < 30) {
	        i++;
	        iSum++;
	        utPltPutLoopVarF(psDbHead,"snum", i,"%d",iSum);
	    	}       
    	free(stService);
    	utPltPutVarF(psDbHead,"CurPg","%d",1);
       utPltPutVarF(psDbHead,"TotPg","%d",1);
       utPltPutVarF(psDbHead,"TotRec","%d",0);
       
      if(atol(caFlag)==1){
  	 			utPltOutToHtml(iFd,psMsgHead,psDbHead,"sj/nc_list_onlineuser_f.htm");
  		}
  		else{
    	  utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_list_onlineuser_f.htm");
    	}
        return 0;
    }
 //按用户ID进行排序，为二分法累计流量做准备  
 temp_sort=iSort;
 temp_order=iOrder;
   iOrder=1;
   iSort=4;
   
   qsort(stService,lSum,sizeof(struct serviceonline_s),ncUtlSortOnService3); 
 
 iSort=temp_sort;
 iOrder=temp_order;   
    
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
         
 //获取USERID对应 stService的下标       
                	index=ncUtlLoolFlowByUserId(stService,lSum,psIp[i]->psUser->lId);
	   	  					if(index!=-1){
	                    stService[index].inbyte=stService[index].inbyte+psIp[i]->lBytes[1];
	          				  stService[index].outbyte=stService[index].outbyte+psIp[i]->lBytes[0];
	          				  if(psIp[i]->lLastTime>stService[index].lasttime){
	          				  	stService[index].lasttime=psIp[i]->lLastTime;
	          				  }
	      						}
         
  
        		  }
         } 
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
     
     if(!utStrIsSpaces(caExport)) { 
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
           strcpy(caIp,utComHostIp(htonl(psUserInfo->lIp)));     	
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
   
       psUserInfo = (ncUserInfo *)ncGetUserInfoById(psShmHead,stService[i].id);
       if(psUserInfo){
         strcpy(caIp,utComHostIp(htonl(psUserInfo->lIp)));
      	 if(psUserInfo->psUser){
           utPltPutLoopVar(psDbHead,"dispname", iNum+1,psUserInfo->psUser->dispname);
           utPltPutLoopVar(psDbHead,"username", iNum+1,psUserInfo->psUser->username);
         }
          if(psUserInfo->psUser->psGroup) {
                utPltPutLoopVar(psDbHead,"dept", iNum+1,psUserInfo->psUser->psGroup->groupname);
          }
        utPltPutLoopVarF(psDbHead,"num",  iNum+1,"%ld",i+1);
        if(stService[i].outbyte+stService[i].inbyte>0)
          utPltPutLoopVarF(psDbHead,"userid",iNum+1,"%d",stService[i].id);
          
          
        utPltPutLoopVar(psDbHead,"id",iNum+1,caService);
        utPltPutLoopVar(psDbHead,"ip",iNum+1,caIp);
        utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(stService[i].inbyte,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte,caTemp));
        
        utPltPutLoopVarF(psDbHead, "inbytes2",iNum+1,"%llu",stService[i].inbyte);
        utPltPutLoopVarF(psDbHead, "outbytes2",iNum+1,"%llu",stService[i].outbyte);
         utPltPutLoopVarF(psDbHead,"band2", iNum+1,"%llu", stService[i].band*8);                        
         
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
    
 //   printf("caFlag=%s\n",caFlag);
  if(atol(caFlag)==1){
  	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"sj/nc_list_onlineuser_f.htm");
  }
  else{
    	utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_list_onlineuser_fsj.htm");
    }
    
    return 0;
}
*/
static int ncUtlSortOnService2(struct serviceonline_s *p1,struct serviceonline_s *p2)
{
        long long l;
        l=(p2->inbyte+p2->outbyte)-(p1->inbyte+p1->outbyte);
        if(l>0) return 1;
        else if(l<0) return (-1);
        else return 0;
                  
}

//统计即时网络服务流量
 /*
int ncWebOnlieServiceStat_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
*/

int ncWebUserTree_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    char caFname[64];
    char caValue[256];
    char caVar[256],caVar1[32];
    char *p,*pVar;
    char  caPlate[128];
    char caBfname[32];
    int  iReturn;
// utMsgPrintMsg(psMsgHead);
    iReturn = utMsgGetSomeNVar(psMsgHead,4,
                    "fname",      UT_TYPE_STRING,63,   caFname,
                    "bfname",     UT_TYPE_STRING,31,   caBfname,
   	                "var",        UT_TYPE_STRING,255,  caVar,
					"plate",      UT_TYPE_STRING, 127, caPlate);
    psDbHead = utPltInitDb();
    if(!utStrIsSpaces(caPlate)) {
        utPltPutVar(psDbHead,"plate",caPlate);
    }
    if(!utStrIsSpaces(caFname)) {
        utPltPutVar(psDbHead,"fname",caFname);
    }
    if(utStrIsSpaces(caBfname)) {
        utPltPutVar(psDbHead,"bfname",caFname);
    }
    else {
        utPltPutVar(psDbHead,"bfname",caBfname);
    }
    if(!utStrIsSpaces(caVar)) {
        p = caVar;
        strcpy(caValue,"\0");
        while(*p) {
            p =  utStrGetWord(p,caVar1,31,",;");
            if(!utStrIsSpaces(caVar1)) {
// printf(" Varn==[%s]\n",caVar1);                
                if(pVar=utMsgGetVar_s(psMsgHead,caVar1)) {
                    utStrAddF(caValue,"&%s@%s",caVar1,pVar);
                }
                if(*p != 0) p++;
            }
            else {
                break;
            }    
        }
// printf(" Value==%s Var==%s \n",caValue,caVar);
        utPltPutVar(psDbHead,"par",caValue);
        utPltPutVar(psDbHead,"var",caVar);
    }        
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
}

/*
int ncWebTreeDispUser_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    pasDbCursor *psCur;
    char caPlate[128];
   	char caGroupname[32];
   	char caUsername[32],caDispname[32];
   	char caFname[32];
   	char caName[32];
   	char caBfname[32];
   	char caWhere[256];
   	char caVar[256];
   	char caValue[256];
   	char caVar1[32];
   	char caApp[16];
   	char *pVar,*p;
   	char caSql[256];
   	char caPar[256];
   	char caIp[16],caMac[20],caComp[32];
   	char caPosition[32];
   	char caTemp[256];
   	int  iNum;
   	char caGroupid[16],caCurPg[16],caTotRec[16];
   	long lGroupid,lGroupid0,lCurPg,lTotRec,lRowNum,lStartRec,lUserid,lTotPg;
    utPltDbHead *psDbHead;
   	int iReturn,i;
   	
   	 long lGid;
   	long nGid[1000];
   	char caGroupbuf[2048];
   	long iNum1;       
    long lGid2;
   	
//   	printf(" ncWebTreeDispUser \n");
    utMsgPrintMsg(psMsgHead);
   	iReturn = utMsgGetSomeNVar(psMsgHead,10,
   	                "var",        UT_TYPE_STRING,255,  caVar,
   	                "groupid",    UT_TYPE_STRING, 12,  caGroupid,
					"plate",      UT_TYPE_STRING, 127, caPlate,
					"CurPg",      UT_TYPE_STRING, 12,  caCurPg,
					"TotRec",     UT_TYPE_STRING, 12,  caTotRec,
   	                "name",       UT_TYPE_STRING, 63,  caName,
					"fname",      UT_TYPE_STRING, 31,  caFname,
					"app",        UT_TYPE_STRING, 12,  caApp,
					"bfname",     UT_TYPE_STRING, 31,  caBfname,
					"par",       UT_TYPE_STRING,255,caPar);
					
			
		if(strlen(caGroupid)>0){
	  lGid=atol(caGroupid);
	  sprintf(caGroupbuf,"%d",lGid);
	  iNum1=0;
	  	if(lGid>0){
	  	iNum1=0;
    	psCur = pasDbOpenSqlF("select groupid from ncgroup where pid=%d ",lGid); 
      lGid2=0;
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid2);                             
	    while((iReturn==0)||(iReturn==1405))
	    {
	    	  sprintf(caGroupbuf+strlen(caGroupbuf),",%d",lGid2);
	  	    nGid[iNum1]=lGid2;
	        iNum1++;       
	        lGid2=0;
	        if(iNum1>900) break;
	        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid2);
	    }
	    pasDbCloseCursor(psCur);
	   }
	   for(i=0;i<iNum1;i++){
	   	
	   	psCur = pasDbOpenSqlF("select groupid from ncgroup where pid=%d ",nGid[i]); 
      lGid2=0;
	    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid2);                             
	    while((iReturn==0)||(iReturn==1405))
	    {
	    	  if(strlen(caGroupbuf)>2000) break;
	    	  sprintf(caGroupbuf+strlen(caGroupbuf),",%d",lGid2);	 
	        lGid2=0;      
	        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lGid2);
	    }
	    pasDbCloseCursor(psCur);
	  }

}
			
			
			
			
					
    if(utStrIsSpaces(caPlate)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),ncLang("0554数据库出现问题，请与管理员联系"));
        return 0;
    }
    
    lGroupid = atol(caGroupid);
    iReturn = dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    strcpy(caWhere,"\0");
    if(iReturn == 0 && lGroupid0 > 0 && lGroupid != lGroupid0) {
        psDbHead = utPltInitDb();
        if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
        if(!utStrIsSpaces(caName)) utPltPutVar(psDbHead,"name",caName);
        utPltPutVarF(psDbHead, "TotRec","%lu",0);
        utPltPutVarF(psDbHead, "TotPg", "%lu",0);
        utPltPutVarF(psDbHead, "CurPg", "%lu",0);
        utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
        return 0;
    }
    else {
    	if((utStrIsSpaces(caName))&&(strlen(caGroupid)!=0)){
    		  if(strcmp(caGroupid,"0")==0){
    		  	sprintf(caWhere," where groupid=0 ");
    		  }
    		  else
        	  sprintf(caWhere," where groupid  in (%s) and usestatus=1 ",caGroupbuf);
        }
        else
        sprintf(caWhere," where usestatus=1 ");
    }
    if(!utStrIsSpaces(caName)) {
    	
        utStrAddF(caWhere," and ( dispname like '%c%s%c' or username like '%c%s%c' or inet_ntoa(ip)='%s' or position like '%c%s%c' ) ",'%',caName,'%','%',caName,'%',caName,'%',caName,'%');
    }
   
    lCurPg = atol(caCurPg);
    if(strcmp(caApp,"prev") == 0) {
//        lCurPg = lCurPg - 1;
    }
    else if(strcmp(caApp,"next") == 0) {
//        lCurPg = lCurPg + 1;
    }
    else {
        lCurPg = 1;
        strcpy(caTotRec,"\0");
    }
    if(lCurPg <= 0) lCurPg = 1;
    if(utStrIsSpaces(caTotRec) || utStrIsSpaces(caApp)) {
        sprintf(caSql,"select count(*) from ncuser %s",caWhere);
// printf("caSql=%s\n",caSql);
        iReturn = pasDbOneRecord(caSql,0,UT_TYPE_LONG,4,&lTotRec);
        if(iReturn != 0) {
            printf("1Sql:[select count(*) from ncuser %s ]\n",caWhere);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0682显示用户"),"DataBase Error %s",pasDbErrorMsg(NULL));
            return 0;
        }
    }
    else {
        lTotRec = atol(caTotRec);
    }
    lRowNum = utComGetVar_ld(psShmHead,"UserTreeRow",200);
    lTotPg = 1 + (lTotRec - 1) / lRowNum;
    if(lCurPg > lTotPg) lCurPg = lTotPg;

    lStartRec = (lCurPg - 1) * lRowNum;  

    psCur = pasDbOpenSqlF("select username,dispname,userid,inet_ntoa(ip),mac,compname,position from ncuser %s order by dispname limit %lu,%lu",caWhere,lStartRec,lRowNum);
    if(psCur == NULL) {
        printf("Sql:[select username,dispname,userid from ncuser %s order by userid limit %lu,%lu]\n",caWhere,lStartRec,lRowNum);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    memset(caIp,0,sizeof(caIp));
    memset(caMac,0,sizeof(caMac));
    memset(caComp,0,sizeof(caComp));
    memset(caPosition,0,sizeof(caPosition));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_STRING,15,caIp,
                                   UT_TYPE_STRING,18,caMac,
                                   UT_TYPE_STRING,30,caComp,
                                   UT_TYPE_STRING,30,caPosition);
    if(!utStrIsSpaces(caPlate)) utPltPutVar(psDbHead,"plate",caPlate);
    if(!utStrIsSpaces(caName)) utPltPutVar(psDbHead,"name",caName);
    iNum = 0;
    while(iReturn == 0) {
        iNum++;
        utPltPutLoopVarF(psDbHead,"userid",   iNum,"%lu",lUserid);
        utPltPutLoopVar(psDbHead,"fname",iNum,caFname);
        if(utStrIsSpaces(caBfname)) {
            utPltPutLoopVar(psDbHead,"bfname",iNum,caFname);
        }
        else {
            utPltPutLoopVar(psDbHead,"bfname",iNum,caBfname);
        }
        utPltPutLoopVarF(psDbHead,"iNum",iNum,"%d",iNum);
        sprintf(caTemp,"[帐号:%s]",caUsername);
        if(strlen(caIp)>0)
        sprintf(caTemp+strlen(caTemp),"[IP:%s]",caIp);
        
        if(strlen(caMac)>0)
        sprintf(caTemp+strlen(caTemp),"[MAC:%s]",caMac);
        
        if(strlen(caComp)>0)
        sprintf(caTemp+strlen(caTemp),"[计算机名:%s]",caComp);
        
        
 utPltSetCvtHtml(1);       
        utPltPutLoopVar(psDbHead, "dispname", iNum,caDispname);
        utPltPutLoopVar(psDbHead, "username", iNum,caUsername);
        utPltPutLoopVar(psDbHead,"userdesc",iNum,caTemp);
        if(strlen(caPosition)==0) strcpy(caPosition,caDispname);
        utPltPutLoopVar(psDbHead,"position",iNum,caPosition);
utPltSetCvtHtml(0);        
    memset(caIp,0,sizeof(caIp));
    memset(caMac,0,sizeof(caMac));
    memset(caComp,0,sizeof(caComp));
    memset(caPosition,0,sizeof(caPosition));
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_LONG,4,&lUserid,
                                   UT_TYPE_STRING,15,caIp,
                                   UT_TYPE_STRING,18,caMac,
                                   UT_TYPE_STRING,30,caComp,
                                   UT_TYPE_STRING,30,caPosition);
    }
    pasDbCloseCursor(psCur);
    utPltPutVarF(psDbHead, "TotRec","%lu",lTotRec);
    utPltPutVarF(psDbHead, "TotPg", "%lu",lTotPg);
    utPltPutVarF(psDbHead, "CurPg", "%lu",lCurPg);
    utPltPutVar(psDbHead, "groupid",caGroupid);
    
    memset(caValue,0,sizeof(caValue));
    if(!utStrIsSpaces(caVar)) {
        p = caVar;
        strcpy(caValue,"\0");
        while(*p) {
            p =  utStrGetWord(p,caVar1,31,",;");
            if(!utStrIsSpaces(caVar1)) {
// printf(" Varn==[%s]\n",caVar1);                
                if(pVar=utMsgGetVar_s(psMsgHead,caVar1)) {
                    utStrAddF(caValue,"&%s@%s",caVar1,pVar);
                }
                if(*p != 0) p++;
            }
            else {
                break;
            }    
        }


        
        utPltPutVar(psDbHead,"var",caVar);
    } 
    
    if(strlen(caValue)==0)
     utPltPutVar(psDbHead,"par",caPar);
     else
        utPltPutVar(psDbHead,"par",caValue);     
    if(!utStrIsSpaces(caFname)) {
        utPltPutVar(psDbHead,"fname",caFname);
    }
    if(utStrIsSpaces(caBfname)) {
        utPltPutVar(psDbHead,"bfname",caFname);
    }
    else {
        utPltPutVar(psDbHead,"bfname",caBfname);
    }
    
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caPlate);
    return 0;
}
*/


//禁止用户上网
int ncUserForbit_sj(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
     ncUserInfo *psUserInfo;
     unsigned long lSip;
     char caUserid[12],caUname[32],caIp[16],caStime[16];
    
		utMsgGetSomeNVar(psMsgHead,2,
		                             "ip",    UT_TYPE_STRING,15,caIp,
		                             "stime", UT_TYPE_STRING,10,caStime);
																
//printf("stime=%s\n",caStime);	
    psDbHead = utPltInitDb();
   
     lSip = ntohl(utComHostAddress(caIp));
//   printf("caIp=%s\n",caIp);
    psUserInfo = ncGetUserInfoByIp(psShmHead,lSip);
    if(psUserInfo){
   	   ncUtlSetUserStatus(psShmHead,psUserInfo,0,atol(caStime));
   	   utPltPutVar(psDbHead,"mesg","操作成功");
    }
    else{
    	utPltPutVar(psDbHead,"mesg","操作不成功");
    }
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"sj/userforbit.htm");
    
    return 0;
}





//统计即时网络服务某个人的流量
 
int ncOnServiceStat_User(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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

 	  unsigned char *pHash;
	  pasLHashInfo  sHashInfo;
 	  
	 struct serviceonline_s *stService;
	 struct serviceonline_s *psFlow;
   int lSumservice;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

    iReturn = utMsgGetSomeNVar(psMsgHead,3,		
		"service",       UT_TYPE_STRING, 10, caService,	
		"id",UT_TYPE_STRING,10,caGroupid,
		"pdf",UT_TYPE_STRING,8,caPdf);
//printf("caGroupid=%s,service=%s\n",caGroupid,caService);

   lSumbytes=0;
    
   lSumservice=0;

 
    iCount = 0;
   lBsteptime=atol(utComGetVar_sd(psShmHead,"BandSteptime","15"));

    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
    if(lMaxRec <= 0) {
    	
       
        iSum=0;
    	  i=0;
         
       
       

      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_servicestatbt.htm");
   
       
        return 0;
    }
    
    	pHash = (unsigned char *)pasLHashInit(20000,20000,sizeof(struct serviceonline_s),0,4); 
			if(pHash == NULL) {
		        printf(" Memory HttpHash Error \n");
		        return (-1);
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
  

  

   stService=malloc(3000*sizeof(struct serviceonline_s));
   if(stService==NULL){
   	    
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),ncLang("0404内存出错"));
           return 0;
       
   }
   memset(stService,0,3000*sizeof(struct serviceonline_s));
   lTime=time(0);
    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&psIp[i]->lService==atol(caService)) {
           
           if((psIp[i]->psUser)&&psIp[i]->psUser->lGid==atol(caGroupid)){           
            psFlow = (struct serviceonline_s *)pasLHashLookA(pHash,&psIp[i]->psUser->lId);
            if(psFlow){
            psFlow->inbyte=psFlow->inbyte+psIp[i]->lBytes[1];
            psFlow->outbyte=psFlow->outbyte+psIp[i]->lBytes[0];
            
                if(psIp[i]->lStartTime > 0&&psIp[i]->lLastTime>0 && (lTime - psIp[i]->lStartTime > lBsteptime)) {
         	    	if((lBsteptime+psIp[i]->lLastTime>lTime)&&(psIp[i]->lLastTime>psIp[i]->lStartTime)){
         	    		if(lBsteptime+psIp[i]->lLastTime>lTime){
         	    			
         	    		   fRate=(float)(lBsteptime+psIp[i]->lLastTime-lTime)/lBsteptime;
         	    		  }
         	    		  else{
         	    		  	fRate=0;
         	    		  }
        // 	    		printf("fRate=%f\n",fRate);
                   psFlow->band = psFlow->band+(psIp[i]->lBytes[0] + psIp[i]->lBytes[1])*fRate/(psIp[i]->lLastTime - psIp[i]->lStartTime);
                  }
                 
               
              }
             	else {
                 psFlow->band= psFlow->band+(psIp[i]->lBytes[0] + psIp[i]->lBytes[1]) /lBsteptime;
             	}
            
           
          }
        }                         
       }           
     }
   
   psFlow= (struct serviceonline_s *)pasLHashFirst(pHash,&sHashInfo);
   lSumservice=0;
   while(psFlow&&lSumservice<3000) {
   stService[lSumservice].id=psFlow->id;
   stService[lSumservice].inbyte=psFlow->inbyte;
   stService[lSumservice].outbyte=psFlow->outbyte;
   stService[lSumservice].band=psFlow->band;
   	
   	lSumservice++;
   	psFlow = (struct serviceonline_s *)pasLHashNextS(&sHashInfo);
   }
   free(pHash);

//printf("lSumservice=%d\n",lSumservice);
     if(lSumservice>0)
         qsort(stService,lSumservice,sizeof(struct serviceonline_s),ncUtlSortOnService2);

    
    
  

   

     
     
   
     iNum=0;
      for(i = 0; i < lSumservice && iNum < 20;i++) {
 

        
      	strcpy(caService,ncUtlBufGetUsername(psShmHead,stService[i].id,"\0"));
      	if(strlen(caService)>0){      		
       lSumbytes=lSumbytes+stService[i].outbyte+stService[i].inbyte;		
       utPltPutLoopVarF(psDbHead,"id",iNum+1,"%d",stService[i].id);
       
       
       utPltSetCvtHtml(1);
        utPltPutLoopVar(psDbHead,"service", iNum+1,ncUtlBufGetUsername(psShmHead,stService[i].id,"\0"));
     utPltSetCvtHtml(0);
        utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(stService[i].inbyte,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte,caTemp));
        
        utPltPutLoopVarF(psDbHead, "subytes2",iNum+1,"%llu",stService[i].outbyte+stService[i].inbyte); 
        utPltPutLoopVarF(psDbHead, "subytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte+stService[i].inbyte,caTemp));  
              
        utPltPutLoopVarF(psDbHead, "band",iNum+1,"%s",utStrLtoF8(stService[i].band*8,caTemp));               
        utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);  
 
        
       iNum++;
     }
    }
     
     i=0;
     
          utPltPutVarF(psDbHead,"TotRec","%d",iNum);  
     
     
   if(strlen(caPdf)>0){
	//表格标签
	 desc=malloc(6*sizeof(char*));
	 for(i=0;i<6;i++){
	 	desc[i]=malloc(32);
   }
   strcpy(desc[0],"用户名");
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
   	   strcpy(caService,ncUtlBufGetUsername(psShmHead,stService[i].id,"\0"));
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
    

      utPltOutToHtml(iFd,psMsgHead,psDbHead,"sj/ncuserviceonlinestat.htm");
   
    return 0;
}




//统计即时网络服务部门的流量
 
int ncOnServiceStat_Dept(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
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
 	  unsigned char *pHash;
	  pasLHashInfo  sHashInfo;
 	  
	 struct serviceonline_s *stService;
	 struct serviceonline_s *psFlow;
   int lSumservice;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

    iReturn = utMsgGetSomeNVar(psMsgHead,2,		
		"service",       UT_TYPE_STRING, 10, caService,	
		"pdf",UT_TYPE_STRING,8,caPdf);
	

   lSumbytes=0;
    
   lSumservice=0;

 
    iCount = 0;
   lBsteptime=atol(utComGetVar_sd(psShmHead,"BandSteptime","15"));

    lMaxRec = utShmHashRecord(psShmHead,NC_LNK_IPPKG);
    if(lMaxRec <= 0) {
    	
       
        iSum=0;
    	  i=0;
         
       
       

      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/iframe_servicestatbt.htm");
   
       
        return 0;
    }
    
    	pHash = (unsigned char *)pasLHashInit(20000,20000,sizeof(struct serviceonline_s),0,4); 
			if(pHash == NULL) {
		        printf(" Memory HttpHash Error \n");
		        return (-1);
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
  

  

   stService=malloc(3000*sizeof(struct serviceonline_s));
   if(stService==NULL){
   	    
           utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0560数据统计"),ncLang("0404内存出错"));
           return 0;
       
   }
   memset(stService,0,3000*sizeof(struct serviceonline_s));
   lTime=time(0);
    for(i=0;i<lSumRec;i++) {
        if((psIp[i]->lSip > 0)&&psIp[i]->lService==atol(caService)) {
           
           if(psIp[i]->psUser){           
            psFlow = (struct serviceonline_s *)pasLHashLookA(pHash,&psIp[i]->psUser->lGid);
            if(psFlow){
            psFlow->inbyte=psFlow->inbyte+psIp[i]->lBytes[1];
            psFlow->outbyte=psFlow->outbyte+psIp[i]->lBytes[0];
            
                if(psIp[i]->lStartTime > 0&&psIp[i]->lLastTime>0 && (lTime - psIp[i]->lStartTime > lBsteptime)) {
         	    	if((lBsteptime+psIp[i]->lLastTime>lTime)&&(psIp[i]->lLastTime>psIp[i]->lStartTime)){
         	    		if(lBsteptime+psIp[i]->lLastTime>lTime){
         	    			
         	    		   fRate=(float)(lBsteptime+psIp[i]->lLastTime-lTime)/lBsteptime;
         	    		  }
         	    		  else{
         	    		  	fRate=0;
         	    		  }
        // 	    		printf("fRate=%f\n",fRate);
                   psFlow->band = psFlow->band+(psIp[i]->lBytes[0] + psIp[i]->lBytes[1])*fRate/(psIp[i]->lLastTime - psIp[i]->lStartTime);
                  }
                 
               
              }
             	else {
                 psFlow->band= psFlow->band+(psIp[i]->lBytes[0] + psIp[i]->lBytes[1]) /lBsteptime;
             	}
            
           
          }
        }                         
       }           
     }
   
   psFlow= (struct serviceonline_s *)pasLHashFirst(pHash,&sHashInfo);
   lSumservice=0;
   while(psFlow&&lSumservice<3000) {
   stService[lSumservice].id=psFlow->id;
   stService[lSumservice].inbyte=psFlow->inbyte;
   stService[lSumservice].outbyte=psFlow->outbyte;
   stService[lSumservice].band=psFlow->band;
   	
   	lSumservice++;
   	psFlow = (struct serviceonline_s *)pasLHashNextS(&sHashInfo);
   }
   free(pHash);

//printf("lSumservice=%d\n",lSumservice);
     if(lSumservice>0)
         qsort(stService,lSumservice,sizeof(struct serviceonline_s),ncUtlSortOnService2);

    
    
  

   

     
     
   
     iNum=0;
      for(i = 0; i < lSumservice && iNum < 20;i++) {
 

        
      	strcpy(caService,ncUtlGetGroupNameByid(psShmHead,stService[i].id,ncLang("0629未知")));
      	if(strlen(caService)>0){      		
       lSumbytes=lSumbytes+stService[i].outbyte+stService[i].inbyte;		
       utPltPutLoopVarF(psDbHead,"id",iNum+1,"%d",stService[i].id);
       
       
       utPltSetCvtHtml(1);
        utPltPutLoopVar(psDbHead,"service", iNum+1,ncUtlGetGroupNameByid(psShmHead,stService[i].id,ncLang("0629未知")));
     utPltSetCvtHtml(0);
        utPltPutLoopVarF(psDbHead, "inbytes",iNum+1,"%s",utStrLtoF8(stService[i].inbyte,caTemp));
        utPltPutLoopVarF(psDbHead, "outbytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte,caTemp));
        
        utPltPutLoopVarF(psDbHead, "subytes2",iNum+1,"%llu",stService[i].outbyte+stService[i].inbyte); 
        utPltPutLoopVarF(psDbHead, "subytes",iNum+1,"%s",utStrLtoF8(stService[i].outbyte+stService[i].inbyte,caTemp));  
              
        utPltPutLoopVarF(psDbHead, "band",iNum+1,"%s",utStrLtoF8(stService[i].band*8,caTemp));               
        utPltPutLoopVarF(psDbHead,"num",iNum+1,"%d",iNum);  
 
        
       iNum++;
     }
    }
     
     i=0;
     
          utPltPutVarF(psDbHead,"TotRec","%d",iNum);  
     
     
   if(strlen(caPdf)>0){
	//表格标签
	 desc=malloc(6*sizeof(char*));
	 for(i=0;i<6;i++){
	 	desc[i]=malloc(32);
   }
   strcpy(desc[0],"部门名");
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
   	   strcpy(caService,ncUtlGetGroupNameByid(psShmHead,stService[i].id,ncLang("0629未知")));
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
    

      utPltOutToHtml(iFd,psMsgHead,psDbHead,"sj/ncuserviceonlinestat_dept.htm");
   
    return 0;
}