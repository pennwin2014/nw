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
#include "nclimit.h"
/* 显示网址库的内容  */
int ncWebTestShowWebDb(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncLimHead   *psLim;
    utPltDbHead *psDbHead;
    unsigned long lTime;
    struct timeval tm0,tm1;
    int i,j,j1,iReturn,iNum,iCount;
    long lFlags,lStart;
    char caStart[16];
    char caFlags[16],caName[64];
    double f;
    unsigned long lTime1,lTimeu;
    long lGroupid0=0;
#ifdef LLLLL
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }
    
#ifdef LDEBUG
    printf("Pid == %d\n",getpid());
    utMsgPrintMsg(psMsgHead);
#endif
 	iReturn = utMsgGetSomeNVar(psMsgHead,3,
 	                "name",        UT_TYPE_STRING,63,caName,
 	                "startrec",    UT_TYPE_STRING,12,caStart);
    lStart = atol(caStart);
    if(lStart < 1) lStart = 1;
    
    psLim = (ncLimHead *)ncLimInitHead();
    if(psLim == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("0403内存不正确"));
         return 0;
    }
    
    psDbHead = utPltInitDb();
    if(!utStrIsSpaces(caName)) {
        int iDo=0;
        int bottom,middle,top,iReturn;
        long lWebId;
        unsigned long lTime;
        int iCount=0;
        ncLimWebList     *psWeb;
        if(psLim->sWebList.psWeb == NULL) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("0622网址库不存在"));
            return 0;
        }
        psWeb = psLim->sWebList.psWeb;
        lTime = time(0);
        gettimeofday(&tm0,NULL);
        for(i=0;i<lStart;i++) {
            iCount=0;
            top =   psLim->sWebList.lSumWeb - 1;
            bottom = 0;
            while(top >= bottom) {
                iCount++;
                middle = (top + bottom) / 2;
                if( (iReturn=strncmp(caName,psWeb[middle].url,psWeb[middle].iLen)) == 0) { /* 找到了 */
                    iDo = 1;
                    break;
                }
                else if(iReturn < 0) {
                    top = middle -1;
                }
                else {
                    bottom = middle + 1;
                }
            }
        }
        if(iDo == 1) {
            utPltPutLoopVarF(psDbHead,"num", 1,"%lu",middle);
            utPltPutLoopVarF(psDbHead,"url", 1,"%s",psWeb[middle].url);
            utPltPutLoopVarF(psDbHead,"len", 1,"%lu",psWeb[middle].iLen);
            utPltPutLoopVarF(psDbHead,"id", 1,"%lu",psWeb[middle].lId);
            utPltPutVarF(psDbHead,"count","%lu",iCount);
        }
        gettimeofday(&tm1,NULL);
        lTime1 = tm1.tv_sec - 1 - tm0.tv_sec;
        lTimeu = 1000000L + tm1.tv_usec - tm0.tv_usec;
        f = (double)(lTime1 * 1000000L + lTimeu) / (double)lStart;
        utPltPutVarF(psDbHead,"time","%12.6f",f);
        printf(" F=== %12.6f Count=%lu  %lu\n",f,iCount,lTime1 * 1000000L + lTimeu);
        
    }
    else {
        lStart = atol(caStart);
        utPltPutVarF(psDbHead,"sumrec","%lu",psLim->sWebList.lSumWeb);
        iNum = 0;
        for(i=lStart;i<psLim->sWebList.lSumWeb &&iNum <500;i++) {
             iNum++;
             utPltPutLoopVarF(psDbHead,"num", iNum,"%lu",i+1);
             utPltPutLoopVarF(psDbHead,"url", iNum,"%s",psLim->sWebList.psWeb[i].url);
             utPltPutLoopVarF(psDbHead,"len", iNum,"%lu",psLim->sWebList.psWeb[i].iLen);
             utPltPutLoopVarF(psDbHead,"id", iNum,"%lu",psLim->sWebList.psWeb[i].lId);
        }
    }
    utPltOutToHtml(iFd,psMsgHead,psDbHead,"test/nc_test_webdb.htm");
#endif    
    return 0;
}

/* 显示Web访问策略      */
int ncWebTestShowWebPol(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncLimHead   *psHead;
    utPltDbHead *psDbHead;
    unsigned long lTime;
    int i,j,j1,iReturn,iNum,iCount;
    long lFlags,lStart;
    char caStart[16];
    char caFlags[16],caName[64];
   long lGroupid0=0;
       //增加网络权限判断
     dsCltGetSessionValue(1,"groupid",UT_TYPE_LONG,sizeof(long),&lGroupid0);
    if(lGroupid0>0)
    {
    	utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0510设置网络"),ncLang("0925您所属的管理组无该功能的权限"));
    	return 0;
    }

    utMsgPrintMsg(psMsgHead);
 	iReturn = utMsgGetSomeNVar(psMsgHead,3,
 	                "name",        UT_TYPE_STRING,63,caName,
 	                "startrec",    UT_TYPE_STRING,12,caStart);
    psHead = (ncLimHead *)ncLimInitHead();
    if(psHead == NULL) {
         utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0208调试"),ncLang("0403内存不正确"));
         return 0;
    }
    psDbHead = utPltInitDb();
//    utPltPutVarF(psDbHead,"lSumWeb","%lu",psHead->sWebList.lSumWeb);
    utPltPutVarF(psDbHead,"lSumPol","%lu",psHead->sWebList.lSumPol);
    utPltPutVarF(psDbHead,"lSumPolIndex","%lu",psHead->sWebList.lSumPolIndex);
    for(i=0;i<psHead->sWebList.lSumPolIndex;i++) {
        utPltPutLoopVarF(psDbHead,"indexnum", i+1,"%lu",i+1);
        utPltPutLoopVarF(psDbHead,"lid", i+1,"%lu",psHead->sWebList.psPolIndex[i].lId);
        utPltPutLoopVarF(psDbHead,"lstart", i+1,"%lu",psHead->sWebList.psPolIndex[i].lStart);
        utPltPutLoopVarF(psDbHead,"lsum", i+1,"%lu",psHead->sWebList.psPolIndex[i].lSum);
        utPltPutLoopVarF(psDbHead,"iflags", i+1,"%lu",psHead->sWebList.psPolIndex[i].iFlags);
        pasDbOneRecord("select name from ncwebpoltype where pid = :id",1,
                                        "id",UT_TYPE_LONG,psHead->sWebList.psPolIndex[i].lId,
                                        UT_TYPE_STRING,63,caName);
        utPltPutLoopVarF(psDbHead,"indexname", i+1,"%s",caName);
        j1=0;
        for(j=psHead->sWebList.psPolIndex[i].lStart;j1<psHead->sWebList.psPolIndex[i].lSum;j++) {
            j1++;
            utPltPutLoopVarF2(psDbHead,"num", i+1,j1, "%lu",j1);
            utPltPutLoopVarF2(psDbHead,"id", i+1,j1, "%lu",psHead->sWebList.psPol[j].lId);
            utPltPutLoopVarF2(psDbHead,"flags", i+1,j1, "%lu",psHead->sWebList.psPol[j].iFlags);
            pasDbOneRecord("select name2 from ncwebtype where sid = :id",1,
                                        "id",UT_TYPE_LONG,psHead->sWebList.psPol[j].lId,
                                        UT_TYPE_STRING,63,caName);
            utPltPutLoopVarF2(psDbHead,"url",i+1,j1, "%s",caName);
        }
     }
     utPltOutToHtml(iFd,psMsgHead,psDbHead,"test/nc_test_weblist.htm");
     return 0;
}    

