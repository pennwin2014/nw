#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "pasutl.h"
#include "ncdef.h"
#include "ncanyprot.h"
#include <iconv.h>



#define SERVICE_STOCK_GTJA 10008 
#define SERVICE_VOID_PP    7026  
#define SERVICE_P2P_P2P    8030
/* 初始化数据包内容跟踪上下文HASH表*/
int ncInitStockHash(utShmHead *psShmHead)
{
    int   iMaxRecord;
    iMaxRecord = utComGetVar_ld(psShmHead, "MaxContext",10000);
    utShmFreeHash(psShmHead,NC_LNK_CONTEXT);
    utShmHashInit(psShmHead,NC_LNK_CONTEXT,iMaxRecord,iMaxRecord,
                                  sizeof(StockContext),0,4);
    return 0;
}

int ncInitMwdHash(utShmHead *psShmHead)
{
    int   iMaxRecord;
    iMaxRecord = utComGetVar_ld(psShmHead, "MaxMwd",1000);
    utShmFreeHash(psShmHead,NC_LNK_MWDUSER);
    utShmHashInit(psShmHead,NC_LNK_MWDUSER,iMaxRecord,iMaxRecord,
                                  sizeof(ncMwduser),0,32);
    return 0;
}



/* 处理迈外迪发过来的用户信息 */
int ncDelwithMwduser(utShmHead *psShmHead)
{
    unsigned long lTimeOut,lTime;
    unsigned char *pHash;
    ncMwduser  *psHashData,*psHashData0;
    pasHashInfo  sHashInfo;
    int iReturn;
    unsigned long lIp;
    char caIdno[20],caDispname[32],caPosition[36],caName[36];
    ncUserCont *psUser;
    iReturn = pasConnect(psShmHead);
    if(iReturn<0) {
    	  sleep(60);
        return 0;
    }
//     sleep(15);
    
    while(1){
    pHash = (unsigned char *)utShmHashHead(psShmHead,NC_LNK_MWDUSER);
    if(pHash == NULL) {
      
        return (-1);
    }
    else {

        psHashData = (ncMwduser *)pasHashFirst(pHash,&sHashInfo);
        while(psHashData) {
               lIp = ntohl(pasIpcvtLong(psHashData->caIp));
      //         printf("lIp=%lu,username=%s,dispname=%s,mac=%s\n",lIp,psHashData->caName,psHashData->caDispname,psHashData->caMac);
               iReturn = ncMwdUserLogin(psShmHead,psHashData->caName,psHashData->caDispname,lIp,psHashData->caMac);
    
               if(iReturn == 0) {
               	        ncUserInfo *psUserInfo;
                        psUserInfo = (ncUserInfo *)ncGetUserInfoByIp(psShmHead,lIp);
                        if(psUserInfo) {

			                      	  psUser = ncUtlGetUserContById(psShmHead,psUserInfo->lId);
			                      	iReturn=pasDbExecSqlF("update ncuser set username='%s',dispname='%s',idfytype='%s',idfyno='%s',idfyunit='%s',country='%s',sex='%s',position='%s' where userid=%d ",
			                      	psHashData->caName,psHashData->caDispname,psHashData->caIdtype,psHashData->caIdno,psHashData->caAddr,psHashData->caCountry,psHashData->caSex,psHashData->caPosition,psUserInfo->lId);
			                      	  psUser = ncUtlGetUserContById(psShmHead,psUserInfo->lId);
			                      	  
    														if(psUser) {
    															strcpy(psUser->cardid,psHashData->caIdno);
    															strcpy(psUser->dispname,psHashData->caDispname);
    															strcpy(psUser->position,psHashData->caPosition);
    															strcpy(psUser->username,psHashData->caName);
    														}
//			                      }
                        }
               	
               	
              }

                psHashData0 = psHashData;
                psHashData= (ncMwduser *)pasHashNextS(&sHashInfo);
                pasHashDel(pHash,psHashData0);
           
        }
    }
    sleep(1);
  }
}




int ncPatFunDoStock(ncTcp *psTcp,char *pkg,int iLen,ncRetService *psRet){
	long lMd5;
	char caMd5[32];
	utShmHead *psShmHead;
	StockContext *psStock;
	psShmHead = utShmCurHead();
    if(psShmHead == NULL) {
        return 0;
    }
	if(iLen==121){

		if(psTcp->cDiction==0){
				sprintf(caMd5,"%lu%lu%u",psTcp->lSip,psTcp->lDip,psTcp->nDport);
			}
			else{
				sprintf(caMd5,"%lu%lu%u",psTcp->lDip,psTcp->lSip,psTcp->nSport);
			}

			lMd5=utMd5Code(caMd5,strlen(caMd5),"ffff");

		psStock=(StockContext *)utShmHashLookA(psShmHead,NC_LNK_CONTEXT,&lMd5);
		if(psStock){
			psStock->lLasttime=time(0);
			psStock->lLen=38;
			memcpy(psStock->cont,pkg+8,38);	
			
//				psRet->nService=SERVICE_STOCK_GTJA;
				return 0;	
		}		
	}
	else{
		if(iLen==264){
				if(psTcp->cDiction==0){
	         sprintf(caMd5,"%lu%lu%u",psTcp->lSip,psTcp->lDip,psTcp->nDport);
		    }
		    else{
			      sprintf(caMd5,"%lu%lu%u",psTcp->lDip,psTcp->lSip,psTcp->nSport);
		     }
	
		    lMd5=utMd5Code(caMd5,strlen(caMd5),"ffff");
	  
				psStock=(StockContext *)utShmHashLook(psShmHead,NC_LNK_CONTEXT,&lMd5);
			  if(psStock){
	/*		  
			  	int i;
			  	printf("222len=%d\n",psStock->lLen);
			  	for(i=0;i<psStock->lLen;i++){
			  		printf("%02x",pkg[2+i]);
			  	}
			  	printf("\n");
			  	
			  	printf("3333len=%d\n",psStock->lLen);
			  	for(i=0;i<psStock->lLen;i++){
			  		printf("%02x",psStock->cont[i]);
			  	}
			  	printf("\n");
	*/		  	
			  	
			  	if(memcmp(pkg+8,psStock->cont,psStock->lLen)==0){
			  	
			  		psRet->cFun=17;
			  		psRet->nService=SERVICE_STOCK_GTJA;
			  		psStock->lLasttime=time(0);
			  		return 1;
			  	}
			  	else{
			  		return 2;
			  	}
			  }
			  else{
			  	return 2;
			  }
			
			
		}
	}
	return 2;
}



/* 检查数据包内容跟踪上下文HASH表表中的内容是否超时  */
int ncCheckStockHashData(utShmHead *psShmHead)
{
    unsigned long lTimeOut,lTime;
    unsigned char *pHash;
    StockContext  *psHashData,*psHashData0;
    pasHashInfo  sHashInfo;
    if(psShmHead == NULL) {
        return 0;
    }
    pHash = (unsigned char *)utShmHashHead(psShmHead,NC_LNK_CONTEXT);
    if(pHash == NULL) {
      
        return (-1);
    }
    else {
        lTime = time(0);
        psHashData = (StockContext *)pasHashFirst(pHash,&sHashInfo);
        while(psHashData) {
            if(psHashData->lLasttime < lTime - 60) { /* 超时  */
                psHashData0 = psHashData;
                psHashData= (StockContext *)pasHashNextS(&sHashInfo);
                pasHashDel(pHash,psHashData0);
            }
            else {
                psHashData= (StockContext *)pasHashNextS(&sHashInfo);
            }
        }
    }
}



//检查是否是皮皮影视

int ncPatFunDoPP(ncTcp *psTcp,char *pkg,int iLen,ncRetService *psRet){
	long lMd5;
	char caMd5[32];
	utShmHead *psShmHead;
	StockContext *psStock;
	psShmHead = utShmCurHead();
    if(psShmHead == NULL) {
        return 0;
    }
	if(iLen==1156||iLen==138){
  
      if((psTcp->nSport==14451)&&(psTcp->cDiction==0)){
      	psRet->nService=SERVICE_VOID_PP;
      	return 1;
      }
      else if((psTcp->nDport==14451)&&(psTcp->cDiction==1)){
      	psRet->nService=SERVICE_VOID_PP;
      	return 1;
      }
	}
	else if((iLen==1056)||(iLen==109)||(iLen==1270)){
		    if((psTcp->nSport==7600)&&(psTcp->cDiction==0)){
      	psRet->nService=SERVICE_P2P_P2P;
      	return 1;
      }
      else if((psTcp->nDport==7600)&&(psTcp->cDiction==1)){
      	psRet->nService=SERVICE_P2P_P2P;
      	return 1;
      }
		  else{
		  	return 2;
		  }
		  
		
		
	}
  else if((iLen==21)||(iLen==55)||(iLen==14)){
		    if((psTcp->nSport==5041)&&(psTcp->cDiction==0)){
      	psRet->nService=7002;
      	return 1;
      }
      else if((psTcp->nDport==5041)&&(psTcp->cDiction==1)){
      	psRet->nService=7002;
      	return 1;
      }
      else{
      	return 2;
      }
		
		
		
	}
	
	
	
	return 2;
}








/* 短信猫发送短信 */
int ncSmsSendMesg(utShmHead *psShmHead)
{
   
}