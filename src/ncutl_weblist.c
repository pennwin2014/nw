/* 有关网址库的操作   
   2008/12/08  最后修改
   2008/06/20  By Liyunming
*/
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <depot.h>
#include "utoall.h"
#include "pasdb.h"
#include "ncdef.h"
#include "pasutl.h"
#include "ncurl.h"

/* 把网址库存放到散列文件中   
 */

int ncUrlCvtMySql2Qdbm(char *pDbFile,unsigned long lTime)
{
    DEPOT *dp;
    ncUrlRecord sRecord;
    unsigned char *pHash;
    pasDbCursor *psCur;
    char            caUrl[128];
    char            caName[128];
    char            caTemp[128];
    char            *p;
    unsigned long   lCount;
    unsigned long   lSid,lId,lLasttime;
    int iCount,iReturn;

    if(!utFileIsExist(pDbFile)){
        printf(" DbFile Does not exist,Now Create it %s \n",pDbFile);
        ncUrlGenUrlDb(pDbFile,2000000L);
    }
    dp = dpopen(pDbFile, DP_OWRITER,0);
    if(dp == NULL) {
        pasLog(PAS_LOG_ERROR,"DataBase File %s Open Error \n",pDbFile);
        return (-1);
    }
    
    ncUrlSynWebClassFromMySql(dp,lTime);
    psCur = pasDbOpenSqlF("select id,url,name,lcount,modtime from ncweblist where modtime >= %lu",lTime);
    if(psCur == NULL) {
        pasLog(PAS_LOG_ERROR,"ncWeblist Open Error %s",pasDbErrorMsg(NULL));
        return (-1);
    }
    iCount=0;
    iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lId,
                                   UT_TYPE_STRING,63 ,caUrl,
                                   UT_TYPE_STRING,63 ,caName,
                                   UT_TYPE_ULONG, 4, &lCount,
                                   UT_TYPE_ULONG, 4, &lLasttime);
    while(iReturn==0) {
        memset(&sRecord,0,sizeof(sRecord));
        utMd5(caUrl,strlen(caUrl),sRecord.urlCode);
        sRecord.urlValue.nId = lId;
        p = utStrGetWord(caName,caTemp,63,"\r\n");
        sRecord.urlValue.nLen = sprintf(sRecord.urlValue.caUrl,"%s,%s",caUrl,caTemp)+1;
        sRecord.urlValue.lCount = lCount;
        sRecord.urlValue.lLastTime = lLasttime;
        sRecord.urlValue.cFlags = NC_URL_UPFLAGS_DONE;
//        sRecord.urlValue.nLen = utDesEncrypt(sRecord.urlValue.caUrl,sRecord.urlValue.nLen,
//                          sRecord.urlValue.caUrl,NC_URL_KEY);
         sRecord.urlValue.nLen = pasTeaEnCode(NC_URL_KEY,sRecord.urlValue.caUrl,sRecord.urlValue.nLen);            
        iReturn = dpput(dp,sRecord.urlCode,16,(char *)&sRecord.urlValue,
                                              13+sRecord.urlValue.nLen,DP_DOVER);
        iCount++;
//        sRecord.urlValue.nLen = utDesDecrypt(sRecord.urlValue.caUrl,sRecord.urlValue.nLen,
//                          sRecord.urlValue.caUrl,NC_URL_KEY);
        iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lId,
                                   UT_TYPE_STRING,63 ,caUrl,
                                   UT_TYPE_STRING,63 ,caName,
                                   UT_TYPE_ULONG, 4, &lCount,
                                   UT_TYPE_ULONG, 4, &lLasttime);
        
    }
    pasDbCloseCursor(psCur);
    // ncUrlUpdateVersion(dp,NC_URL_SOFTVERSION,NC_URL_DBVERSION,time(0)); 
    dpclose(dp);
    ncSetLastInfo_l("UrlDbModTime",time(0));
    printf("Pid: %d Cvt %lu Record To DataBase\n",getpid(),iCount);
    pasLog(PAS_LOG_EVENT,"Cvt %lu Record 2 DataBase",iCount);
    return iCount;
}



int ncUrlSynWebClassFromMySql(DEPOT *dp,unsigned long lTime)
{
    int iMaxClass=200;
    int iModi=0;
    int iCount,iReturn;
    ncUrlWebName *ps;
    char caName[32];
   
    ps = (ncUrlWebName *)malloc(iMaxClass * sizeof(ncUrlWebName));
    if(ps == NULL) {
        pasLog(PAS_LOG_ERROR,"Web Class Memory alloc error");
        return (-1);
    }
    iCount = ncUrlReadAllWebClass(dp,ps,iMaxClass);
    if(iCount >= 0) {
         int i;
         pasDbCursor    *psCur;
         long           lId;
         char           caname[32];
         psCur = pasDbOpenSqlF("select id,name from ncwebclass where modtime > %lu and status = 1 and flags != 99 ",lTime);
         if(psCur) {
            iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lId,
                                       UT_TYPE_STRING,31, caName);
            while(iReturn == 0) {
                for(i=0;i<iCount;i++) {
                    if(ps[i].lId == lId) {
                        if(strcmp(ps[i].name,caName)!=0) {
                            strcpy(ps[i].name,caName);
                            iModi = 1;
                            pasLog(PAS_LOG_EVENT," Class Modi: %s ==> %s Id=%04d",
                                        ps[i].name,caName,lId);
                        }
                        break;
                    }
                }
                if(i == iCount && iCount < iMaxClass) {
                    ps[i].lId = lId;
                    strcpy(ps[i].name,caName);
                    iModi = 1;
                    pasLog(PAS_LOG_EVENT," Class Add: %s %04d ",
                                       ps[i].name,lId);
                    iCount++;
                }
                iReturn = pasDbFetchInto(psCur,UT_TYPE_LONG,  4, &lId,
                                       UT_TYPE_STRING,31, caName);
            
            }
            pasDbCloseCursor(psCur);
        }
        if(iModi) {
            ncUrlSaveAllWebClass(dp,ps,iCount);
        }
    }
    free(ps);
    return 0;
}



/* 初始化网址库装载内存
    iFlags --- 0 放在普通内存中
               1 放在共享内存中

    iMaxUrl --- 在内存中存放的最大Url数
   返回
   0--正确
   <0--不正确
*/
int ncUrlInitUrlBuf(int iFlags,int iMaxUrl,char *pDbFile)
{
    
    unsigned long lSize;
    int         iReturn;
    ncUrlBufHead *psHead;
    char caEngver[16],caDbVer[16],caUpdate[32];
    int  iSumRecord;
    lSize = pasLHashBufSize(iMaxUrl,iMaxUrl,sizeof(ncUrlCode));
    if(iFlags) {
        utShmHead *psShmHead=utShmCurHead();
        if(psShmHead == NULL) {
            return (-1);
        }
        utShmFreeArray(psShmHead,NC_LNK_WEBLIST);
        iReturn = utShmInitArray(psShmHead,NC_LNK_WEBLIST,lSize + sizeof(ncUrlBufHead),1);
        if(iReturn != 0) {
            pasLog(PAS_LOG_ERROR,"Memory Init Error at ncUrlInitUrlBuf");
            return (-1);
        }
        psHead = (ncUrlBufHead *)utShmArray(psShmHead,NC_LNK_WEBLIST);
    }
    else {
        if(psHead) {
            free(psHead);
        }
        psHead = (ncUrlBufHead *)malloc(lSize + sizeof(ncUrlBufHead));
        if(psHead == NULL) {
            pasLog(PAS_LOG_ERROR,"Memory Init Error at ncUrlInitUrlBuf");
            return (-1);
        }
    }
    memset(psHead,0,lSize + sizeof(ncUrlBufHead));
    psHead->lSize = lSize;
    psHead->lMaxUrl = iMaxUrl;
    psHead->cLock = 1;
    psHead->cFull = 0;
    psHead->cShm = iFlags;
    psHead->pHash = (unsigned char *)psHead + sizeof(ncUrlBufHead);
    pasLHashInit0(psHead->pHash,iMaxUrl,iMaxUrl,sizeof(ncUrlCode),0,16);
    iReturn = ncUrlOpenUrlDb(pDbFile);
    ncUrlLoadWebClassName();
    ncUrlGetUrlVersion(caEngver,caDbVer,caUpdate,&iSumRecord);
    printf(" Load  Url Sucess \n");
    printf(" UrlDb EngVer:[%s]  DbVer:[%s] LastUpdate:[%s],SumUrl=%lu\n",
            caEngver,caDbVer,caUpdate,iSumRecord);
    ncUrlSetUrlBufHead(psHead);
    return iReturn;    
}

/* 初始化网址库

 */
int ncUrlInitShmUrl(char *pDbFile)
{
    utShmHead *psShmHead=utShmCurHead();
    unsigned long lTime;
    int iReturn;
    int iAutoAddUrl,iDomainLevel;
    unsigned long lMaxUrl;
    if(psShmHead == NULL) {
        return (-1);
    }
    lMaxUrl = utComGetVar_ld(psShmHead,"MaxUrl",1000000L);
    if(strcasecmp(utComGetVar_sd(psShmHead,"AutoAddUrl","Yes"),"Yes")==0) {
        iAutoAddUrl = 1;
    }
    else {
        iAutoAddUrl = 0;
    }
    iDomainLevel = utComGetVar_ld(psShmHead,"DomainLevel",2);
    ncUrlSetUrlFlags(iAutoAddUrl,iDomainLevel);
    lTime = ncGetLastInfo_l("UrlDbModTime",0);
    ncUrlCvtMySql2Qdbm(pDbFile,lTime);
    iReturn =  ncUrlInitUrlBuf(1,lMaxUrl,pDbFile);
    return iReturn;
}

/*  检查网址库内存，将过期的网址删除  */


int ncUrlCheckMemUrl(utShmHead *psShmHead)
{
    ncUrlBufHead *psHead;
    pasLHashInfo    sHashInfo;
    unsigned        long lTime,lTimeOut,lCheckTime,lNextCheck,lUrlCheck=0;
    ncUrlCode       *ps,*ps0;
    int iReturn;
    lNextCheck = 0;
    psHead = (ncUrlBufHead *)utShmArray(psShmHead,NC_LNK_WEBLIST);
    while(psHead == NULL) {
        psHead = (ncUrlBufHead *)utShmArray(psShmHead,NC_LNK_WEBLIST);
        sleep(10);
    }
    lTimeOut = utComGetVar_ld(psShmHead,"UrlTimeOut",3600);
    lCheckTime = utComGetVar_ld(psShmHead,"UrlCheckTime",1800);
    while(1) {
        lTime = time(0);
        if(psHead->cUpMark || lTime > lUrlCheck) { /* 网址库更新  */
            /* 检查更新的网址   */
            iReturn = ncUrlCheckUpdateUrl("/home/ncmysql/nc/log/urlupdate");
            psHead->cUpMark = 0;
            lUrlCheck = lTime + 3600;
        }
        lTime = time(0);
        if(lTime > lNextCheck) {
            ps = (ncUrlCode *)pasLHashFirst(psHead->pHash,&sHashInfo);
            while(ps) {
                ps0 = ps;
                ps = (ncUrlCode *)pasLHashNextS(&sHashInfo);
                if(ps0->lLastTime < lTime - lTimeOut) {
                    pasLHashDel(psHead->pHash,ps0);
                }
            }
            lNextCheck = lTime + lCheckTime;
        }
        sleep(10);
    }
    return 0;
}
    
/* 将文本格式的文件转换为更新文件格式    

 */                
int ncUrlCvtTxt2Upfile(char *pTxtFile,char *pUpFile,int iFlags)
{
    FILE *fp;
    FILE *fpo;
    FILE *fpe;
    char *p;
    char buf[256];
    char caTemp1[128],caTemp2[128],caTemp3[128];
    int n,lId,lId0,lCount=0;
    char caId[32];
    ncLoasClassName();
    fp = fopen(pTxtFile,"r");
    if(fp == NULL) {
        printf("File %s open error\n",pTxtFile);
        return (-1);
    }

    if(iFlags == 0) {
        fpo = fopen(pUpFile,"w");
    }
    else {
        fpo = fopen(pUpFile,"r");
    }
    if(fpo == NULL) {
        fclose(fp);
        printf("File %s Open Error \n",pUpFile);
        return (-1);
    }
    fpe = fopen("urlerror.log","w");
    p = fgets(buf,255,fp);
    while(p) {
        p = utStrSkipSpaces(buf);
        if(*p != '#' && !utStrIsSpaces(p)) {
            strcpy(caTemp1,"\0");
            strcpy(caTemp2,"\0");
            strcpy(caTemp3,"\0");
            n = utStrGetSomeWordBySp(p,3,UT_TYPE_STRING,63,caTemp1,
                                         UT_TYPE_STRING,63,caTemp2,
                                         UT_TYPE_STRING,63,caTemp3);
            p = utStrSkipSpaces(caTemp1);
            if(p && *p == '[') { /* 编号或内容  */
                p = utStrGetWord(p+1,caId,31,"] ");
                if(utStrIsDigital(caId)) {
                    lId = atol(caId);
                }
                else {
                    lId = ncUrlClassId(caId);
                }
                if(utStrIsDoMain(caTemp2)) {
                    ncUrlPutUpdateUrl(fpo,lId,caTemp2,caTemp3);
                    lCount++;
                }
                else {
                    fprintf(fpe," Error Url [%s]\n",buf);
                }
            }
            else {
                if(strcasecmp(caTemp1,"ClassName") == 0) {
                    lId0 = ncUrlClassId(caTemp2);
                }
                else if(strcasecmp(caTemp1,"AddClass") == 0) {
                    lId = atol(caTemp2);
                    if(lId > 0 && !utStrIsSpaces(caTemp3)) {
                        ncUrlPutUpdateClass(fpo,lId,utStrGetHanZi(caTemp3,63));
                        lCount++;
                    }
                }
                else if(strcasecmp(caTemp1,"DelUrl") == 0) {
                    if(!utStrIsSpaces(caTemp2)) {
                        ncUrlDelUpdateUrl(fpo,caTemp2);
                        lCount++;
                    }
                }
                else if(strcasecmp(caTemp1,"DelClass") == 0) {
                    if(!utStrIsSpaces(caTemp2)) {
                        ncUrlDelUpdateClass(fpo,caTemp2);
                        lCount++;
                    }
                }
                else if(utStrIsDoMain(caTemp1)) {
                    ncUrlPutUpdateUrl(fpo,lId0,caTemp1,utStrGetHanZi(caTemp2,63));
                    lCount++;
                }
                else {
                    fprintf(fpe," Error Url [%s]\n",buf);
                }
            }
        }
        p = fgets(buf,256,fp);
    }
    fclose(fp);
    fclose(fpo);
    fclose(fpe);
    ncFreeClassName();
    return lCount;
}

